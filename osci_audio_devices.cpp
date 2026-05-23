#include "osci_audio_devices.h"

#include "components/osci_CustomAudioDeviceSelectorComponent.cpp"

#if JUCE_WINDOWS && OSCI_AUDIO_DEVICES_ENABLE_SYSTEM_AUDIO
#include "platform/osci_WindowsLoopbackAudioDeviceType.cpp"
#endif
