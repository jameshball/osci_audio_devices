/*
  ==============================================================================

   This file is derived from JUCE's AudioDeviceSelectorComponent.
   Original code copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

   All types are prefixed with "Custom" and placed outside namespace juce to
   avoid ODR violations when member layouts diverge from JUCE's originals.

  ==============================================================================
*/

#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <osci_gui/osci_gui.h>

#include "../osci_audio_devices_config.h"

namespace juce
{

//==============================================================================
/**
    A component containing controls to let the user change the audio settings of
    an AudioDeviceManager object.

    This is a customised fork of juce::AudioDeviceSelectorComponent with support
    for combined capture device types (Process Audio on macOS, Windows Loopback).

    All internal types are prefixed with "Custom" to prevent ODR violations
    with JUCE's identically-named originals that are compiled via the
    amalgamated include_juce_audio_utils.cpp.

    @see AudioDeviceManager
*/
class JUCE_API CustomAudioDeviceSelectorComponent : public Component,
                                                     private ChangeListener
{
public:
    CustomAudioDeviceSelectorComponent (AudioDeviceManager& deviceManager,
                                        int minAudioInputChannels,
                                        int maxAudioInputChannels,
                                        int minAudioOutputChannels,
                                        int maxAudioOutputChannels,
                                        bool showMidiInputOptions,
                                        bool showMidiOutputSelector,
                                        bool showChannelsAsStereoPairs,
                                        bool hideAdvancedOptionsWithButton);

    ~CustomAudioDeviceSelectorComponent() override;

    AudioDeviceManager& deviceManager;

    void setItemHeight (int itemHeight);
    int getItemHeight() const noexcept      { return itemHeight; }
    ListBox* getMidiInputSelectorListBox() const noexcept;

    void resized() override;
    void childBoundsChanged (Component* child) override;

private:
    void handleBluetoothButton();
    void updateDeviceType();
    void changeListenerCallback (ChangeBroadcaster*) override;
    void updateAllControls();

    osci::OverlayLookAndFeel overlayLookAndFeel;
    std::unique_ptr<ComboBox> deviceTypeDropDown;
    std::unique_ptr<Label> deviceTypeDropDownLabel;
    std::unique_ptr<Component> audioDeviceSettingsComp;
    String audioDeviceSettingsCompType;
    int itemHeight = 0;
    const int minOutputChannels, maxOutputChannels, minInputChannels, maxInputChannels;
    const bool showChannelsAsStereoPairs;
    const bool hideAdvancedOptionsWithButton;

    class CustomMidiInputSelectorComponentListBox;
    class CustomMidiOutputSelector;

    Array<MidiDeviceInfo> currentMidiOutputs;
    std::unique_ptr<CustomMidiInputSelectorComponentListBox> midiInputsList;
    std::unique_ptr<CustomMidiOutputSelector> midiOutputSelector;
    std::unique_ptr<Label> midiInputsLabel, midiOutputLabel;
    std::unique_ptr<TextButton> bluetoothButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioDeviceSelectorComponent)
};

} // namespace juce
