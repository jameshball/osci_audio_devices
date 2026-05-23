/*
  ==============================================================================

   This file is part of the osci-render audio devices module
   Copyright (c) 2026 James H Ball

  ==============================================================================
*/

#pragma once

#include "osci_audio_devices_config.h"

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.

 BEGIN_JUCE_MODULE_DECLARATION

  ID:                osci_audio_devices
  vendor:            jameshball
  version:           1.0.0
  name:              osci audio devices
  description:       Shared JUCE audio device helpers for osci apps
  website:           https://osci-render.com
  license:           GPLv3
  minimumCppStandard: 20

  dependencies:      juce_core, juce_audio_devices, juce_audio_utils, juce_gui_basics, osci_gui

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <osci_gui/osci_gui.h>

#include "platform/osci_ProcessAudioDeviceType.h"
#include "platform/osci_ProcessAudioPermissions.h"
#include "platform/osci_WindowsLoopbackAudioDeviceType.h"
#include "components/osci_CustomAudioDeviceSelectorComponent.h"
