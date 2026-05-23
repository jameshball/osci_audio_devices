# Third-Party Notices

## JUCE-Derived Components

`components/osci_CustomAudioDeviceSelectorComponent.*` is derived from JUCE's `AudioDeviceSelectorComponent`. The original JUCE notices are retained in the source files and must remain with copies or substantial portions of those files.

## miniaudio

The Windows loopback device implementation uses miniaudio when `OSCI_AUDIO_DEVICES_ENABLE_SYSTEM_AUDIO=1` on Windows. The module includes miniaudio as `third_party/miniaudio`.

miniaudio is available under the Unlicense or MIT No Attribution. Keep `third_party/miniaudio/LICENSE` with source distributions, and include the required miniaudio notice with binary distributions if you choose the MIT No Attribution path.
