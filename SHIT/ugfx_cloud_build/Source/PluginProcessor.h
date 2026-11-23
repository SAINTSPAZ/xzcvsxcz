#pragma once
#include <JuceHeader.h>

class UGFXAudioProcessor : public juce::AudioProcessor
{
public:
    UGFXAudioProcessor();
    ~UGFXAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "UGFX"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 3.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    double sr = 44100.0;
    juce::AudioBuffer<float> delayBuffer, stutterBuffer;
    int delayWrite = 0, stutterWrite = 0;
    int stutterHold = 0;
    float flangerPhase = 0.0f, ghostPhase = 0.0f, helicopterPhase = 0.0f;
    juce::dsp::IIR::Filter<float> lowpassL, lowpassR;
    juce::Random random;
    float crushHoldL = 0.0f, crushHoldR = 0.0f;
    int crushCounter = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UGFXAudioProcessor)
};
