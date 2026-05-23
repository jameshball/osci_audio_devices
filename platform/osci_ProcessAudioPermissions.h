/*
  ==============================================================================

    osci_ProcessAudioPermissions.h
    Created for osci-render

    Utility for checking macOS 14.2+ process audio capture availability.

  ==============================================================================
*/

#pragma once

#include <juce_core/system/juce_TargetPlatform.h>

#include <functional>

#include "../osci_audio_devices_config.h"

#if JUCE_MAC && OSCI_AUDIO_DEVICES_ENABLE_SYSTEM_AUDIO

namespace ProcessAudioPermissions
{
    // Returns true if running macOS 14.2+ where AudioHardwareCreateProcessTap is available.
    bool isProcessTapAvailable();

  enum class AudioCapturePermissionStatus
  {
    unknown,
    denied,
    authorized,
    unavailable
  };

  // Checks permission for kTCCServiceAudioCapture using TCC SPI when available.
  AudioCapturePermissionStatus getAudioCapturePermissionStatus();

  // Requests permission for kTCCServiceAudioCapture (best-effort, async).
  // Returns false if the SPI is unavailable.
  bool requestAudioCapturePermission (std::function<void (bool granted)> completion);
}

#endif
