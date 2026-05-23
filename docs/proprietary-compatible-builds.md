# Proprietary-Compatible Builds

`osci_audio_devices` is publicly distributed under the GPLv3 license declared by the JUCE module and repository license files.

This module contains JUCE-dependent audio device infrastructure intended for use only in products with suitable JUCE licensing.

Set `OSCI_AUDIO_DEVICES_ENABLE_SYSTEM_AUDIO=1` to compile the platform-native system-audio capture device types. The flag defaults to `OSCI_PREMIUM` for existing products.

The custom device selector is derived from JUCE's device selector implementation. Proprietary consumers must use this module only with suitable JUCE commercial or other non-GPL-compatible licensing for the consuming product.

Windows system-audio capture uses miniaudio when enabled. Keep the miniaudio notice with products that redistribute that dependency.

This is an engineering policy document, not legal advice.
