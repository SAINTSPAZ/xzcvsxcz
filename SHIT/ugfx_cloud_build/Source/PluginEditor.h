#pragma once
#include "PluginProcessor.h"

class UGFXAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit UGFXAudioProcessorEditor (UGFXAudioProcessor&);
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    UGFXAudioProcessor& audio;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::Label> labels;
    juce::OwnedArray<SliderAttachment> sliderAttachments;
    juce::OwnedArray<juce::ToggleButton> buttons;
    juce::OwnedArray<ButtonAttachment> buttonAttachments;
    void addKnob(const juce::String&, const juce::String&, juce::Rectangle<int>);
    void addSwitch(const juce::String&, const juce::String&, juce::Rectangle<int>);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UGFXAudioProcessorEditor)
};
