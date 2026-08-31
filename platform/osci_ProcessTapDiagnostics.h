#pragma once

#include <array>
#include <mach/mach_time.h>

// Temporary, opt-in diagnostics. The IO callback only records numbers into a
// preallocated SPSC queue; aggregation, formatting and logging run on the timer.
class ProcessTapDiagnostics : private juce::Timer {
public:
    struct Record {
        uint64_t entered = 0, finished = 0;
        double inputSampleTime = 0;
        float peak = 0;
        int inputFrames = 0, outputFrames = 0;
        bool validSampleTime = false, nullInput = false;
    };

    ProcessTapDiagnostics(double rate, int frames, AudioDeviceID physical, AudioDeviceID aggregate)
        : sampleRate(rate), configuredFrames(frames), physicalDevice(physical), aggregateDevice(aggregate) {
        mach_timebase_info_data_t timebase {};
        mach_timebase_info(&timebase);
        ticksToMs = double(timebase.numer) / double(timebase.denom) / 1.0e6;
        juce::Logger::writeToLog("ProcessTap diagnostics START " + juce::Time::getCurrentTime().toISO8601(true)
            + " rate=" + juce::String(rate) + " frames=" + juce::String(frames));
        startTimer(1000);
    }

    ~ProcessTapDiagnostics() override {
        stopTimer();
        timerCallback();
    }

    Record begin(const AudioBufferList* input, const AudioBufferList* output, const AudioTimeStamp* timestamp) const noexcept {
        Record record;
        record.entered = mach_absolute_time();
        record.validSampleTime = timestamp != nullptr && (timestamp->mFlags & kAudioTimeStampSampleTimeValid) != 0;
        if (record.validSampleTime) {
            record.inputSampleTime = timestamp->mSampleTime;
        }
        record.inputFrames = frameCount(input);
        record.outputFrames = frameCount(output);
        record.nullInput = input == nullptr || input->mNumberBuffers == 0;
        if (input != nullptr) {
            for (UInt32 b = 0; b < input->mNumberBuffers; ++b) {
                const auto& buffer = input->mBuffers[b];
                const auto* data = static_cast<const float*>(buffer.mData);
                record.nullInput |= data == nullptr;
                if (data != nullptr) {
                    for (size_t i = 0; i < buffer.mDataByteSize / sizeof(float); ++i) {
                        record.peak = juce::jmax(record.peak, std::abs(data[i]));
                    }
                }
            }
        }
        return record;
    }

    void end(Record record) noexcept {
        record.finished = mach_absolute_time();
        int start1, size1, start2, size2;
        queue.prepareToWrite(1, start1, size1, start2, size2);
        if (size1 != 0) {
            records[static_cast<size_t>(start1)] = record;
            queue.finishedWrite(1);
        } else {
            dropped.fetch_add(1, std::memory_order_relaxed);
        }
    }

private:
    static int deviceFrames(AudioDeviceID device) {
        UInt32 frames = 0, size = sizeof(frames);
        AudioObjectPropertyAddress address { kAudioDevicePropertyBufferFrameSize, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };
        const auto status = AudioObjectGetPropertyData(device, &address, 0, nullptr, &size, &frames);
        return status == noErr ? int(frames) : -1;
    }

    static int frameCount(const AudioBufferList* buffers) noexcept {
        if (buffers == nullptr || buffers->mNumberBuffers == 0 || buffers->mBuffers[0].mNumberChannels == 0) {
            return 0;
        }
        const auto& buffer = buffers->mBuffers[0];
        return int(buffer.mDataByteSize / (sizeof(float) * buffer.mNumberChannels));
    }

    void timerCallback() override {
        int count = 0, silent = 0, nulls = 0, mismatches = 0, timestampGaps = 0, overBudget = 0;
        int minFrames = std::numeric_limits<int>::max(), maxFrames = 0;
        int minOutputFrames = std::numeric_limits<int>::max(), maxOutputFrames = 0;
        double maxInterval = 0, maxDuration = 0, totalDuration = 0, longestSilence = 0;
        float peak = 0;
        int start1, size1, start2, size2;
        queue.prepareToRead(queue.getNumReady(), start1, size1, start2, size2);
        const auto consume = [&](int start, int size) {
            for (int i = start; i < start + size; ++i) {
                const auto& r = records[static_cast<size_t>(i)];
                ++count;
                const double duration = double(r.finished - r.entered) * ticksToMs;
                totalDuration += duration;
                maxDuration = juce::jmax(maxDuration, duration);
                overBudget += duration > 1000.0 * configuredFrames / sampleRate;
                if (previous.entered != 0) {
                    maxInterval = juce::jmax(maxInterval, double(r.entered - previous.entered) * ticksToMs);
                    if (r.validSampleTime && previous.validSampleTime) {
                        timestampGaps += std::abs(r.inputSampleTime - previous.inputSampleTime - previous.inputFrames) > 0.5;
                    }
                }
                silent += r.peak == 0;
                nulls += r.nullInput;
                mismatches += r.inputFrames != r.outputFrames;
                minFrames = juce::jmin(minFrames, r.inputFrames);
                maxFrames = juce::jmax(maxFrames, r.inputFrames);
                minOutputFrames = juce::jmin(minOutputFrames, r.outputFrames);
                maxOutputFrames = juce::jmax(maxOutputFrames, r.outputFrames);
                peak = juce::jmax(peak, r.peak);
                silentFrames = r.peak == 0 ? silentFrames + r.inputFrames : 0;
                longestSilence = juce::jmax(longestSilence, 1000.0 * double(silentFrames) / sampleRate);
                previous = r;
            }
        };
        consume(start1, size1);
        consume(start2, size2);
        queue.finishedRead(size1 + size2);
        juce::Logger::writeToLog("ProcessTap diagnostics " + juce::Time::getCurrentTime().toISO8601(true)
            + " configured=" + juce::String(configuredFrames) + " callbacks=" + juce::String(count)
            + " physicalFrames=" + juce::String(deviceFrames(physicalDevice)) + " aggregateFrames=" + juce::String(deviceFrames(aggregateDevice))
            + " inputFrames=" + juce::String(count == 0 ? 0 : minFrames) + ".." + juce::String(maxFrames)
            + " outputFrames=" + juce::String(count == 0 ? 0 : minOutputFrames) + ".." + juce::String(maxOutputFrames)
            + " maxGapMs=" + juce::String(maxInterval, 3) + " maxCallbackMs=" + juce::String(maxDuration, 3)
            + " meanCallbackMs=" + juce::String(count == 0 ? 0 : totalDuration / count, 6)
            + " overBudget=" + juce::String(overBudget) + " zeroBlocks=" + juce::String(silent)
            + " longestZeroMs=" + juce::String(longestSilence, 3) + " peak=" + juce::String(peak, 6)
            + " nullInput=" + juce::String(nulls) + " sizeMismatch=" + juce::String(mismatches)
            + " timestampGaps=" + juce::String(timestampGaps) + " droppedRecords=" + juce::String(dropped.exchange(0)));
    }

    const double sampleRate;
    const int configuredFrames;
    const AudioDeviceID physicalDevice, aggregateDevice;
    double ticksToMs = 0;
    std::array<Record, 16384> records {};
    juce::AbstractFifo queue { int(records.size()) };
    std::atomic<int> dropped { 0 };
    Record previous;
    int64_t silentFrames = 0;
};
