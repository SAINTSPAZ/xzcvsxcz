#include "PluginProcessor.h"
#include "PluginEditor.h"

static std::atomic<float>* getP (juce::AudioProcessorValueTreeState& s, const char* id)
{ return s.getRawParameterValue (id); }

UGFXAudioProcessor::UGFXAudioProcessor()
: AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                  .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
  parameters (*this, nullptr, "UGFX_STATE", createParameterLayout())
{}

juce::AudioProcessorValueTreeState::ParameterLayout UGFXAudioProcessor::createParameterLayout()
{
    using P = juce::AudioParameterFloat;
    using B = juce::AudioParameterBool;
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    auto addKnob = [&] (const char* id, const char* name, float lo, float hi, float def)
    { l.add (std::make_unique<P>(id, name, juce::NormalisableRange<float>(lo, hi), def)); };
    auto addSwitch = [&] (const char* id, const char* name)
    { l.add (std::make_unique<B>(id, name, false)); };
    addKnob("flanger", "Flanger", 0, 1, 0);
    addKnob("ghost", "Ghost", 0, 1, 0);
    addKnob("underwater", "Underwater", 0, 1, 0);
    addKnob("bitcrush", "Bitcrush", 0, 1, 0);
    addKnob("reverb", "Reverb", 0, 1, 0);
    addKnob("distortion", "Distortion", 0, 1, 0);
    addKnob("delay", "Delay", 0, 1, 0);
    addKnob("helicopter", "Helicopter", 0, 1, 0);
    addKnob("stutter", "Stutter", 0, 1, 0);
    addKnob("speed", "Speed", 0, 1, 0.25f);
    addKnob("pan", "Pan", -1, 1, 0);
    addKnob("mix", "Mix", 0, 1, 1);
    return l;
}

bool UGFXAudioProcessor::isBusesLayoutSupported (const BusesLayout& b) const
{
    return b.getMainInputChannelSet() == juce::AudioChannelSet::mono()
        || b.getMainInputChannelSet() == juce::AudioChannelSet::stereo();
}

void UGFXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    delayBuffer.setSize(2, (int) (sr * 2.5) + samplesPerBlock);
    delayBuffer.clear(); delayWrite = 0;
    stutterBuffer.setSize(2, (int) (sr * 0.5) + samplesPerBlock);
    stutterBuffer.clear(); stutterWrite = 0; stutterHold = 0;
    lowpassL.reset(); lowpassR.reset();
    juce::dsp::ProcessSpec spec { sr, (juce::uint32) samplesPerBlock, 1 };
    lowpassL.prepare(spec); lowpassR.prepare(spec);
    flangerPhase = ghostPhase = helicopterPhase = 0.0f;
    crushCounter = 0;
}

void UGFXAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = buffer.getNumChannels(), n = buffer.getNumSamples();
    const float mix = *getP(parameters,"mix");
    const float fl = *getP(parameters,"flanger"), gh = *getP(parameters,"ghost");
    const float uw = *getP(parameters,"underwater"), bc = *getP(parameters,"bitcrush");
    const float rv = *getP(parameters,"reverb"), ds = *getP(parameters,"distortion");
    const float dl = *getP(parameters,"delay"), hp = *getP(parameters,"helicopter");
    const float st = *getP(parameters,"stutter"), speed = *getP(parameters,"speed");
    const float pan = *getP(parameters,"pan");
    juce::AudioBuffer<float> dry(buffer.getNumChannels(), n);
    dry.makeCopyOf(buffer, true);

    auto* L = buffer.getWritePointer(0);
    auto* R = channels > 1 ? buffer.getWritePointer(1) : nullptr;
    auto* dL = delayBuffer.getWritePointer(0); auto* dR = delayBuffer.getWritePointer(1);
    auto* sL = stutterBuffer.getWritePointer(0); auto* sR = stutterBuffer.getWritePointer(1);
    auto lpCoeff = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        sr, juce::jmax(80.0f, 18000.0f - uw * 17400.0f));
    lowpassL.coefficients = lpCoeff; lowpassR.coefficients = lpCoeff;
    const int delaySize = delayBuffer.getNumSamples(), stSize = stutterBuffer.getNumSamples();
    const float rate = 0.15f + speed * 9.0f;

    for (int i=0; i<n; ++i)
    {
        float xL=L[i], xR=R ? R[i] : xL;
        const float phase = flangerPhase;
        const int modDelay = (int)(sr * (0.002f + 0.008f * (0.5f + 0.5f * std::sin(phase))));
        int rd = (delayWrite - modDelay + delaySize) % delaySize;
        float delayedL=dL[rd], delayedR=dR[rd];
        xL += delayedL * fl * 0.75f; xR += delayedR * fl * 0.75f;
        flangerPhase += juce::MathConstants<float>::twoPi * 0.18f / (float)sr;
        if (flangerPhase > juce::MathConstants<float>::twoPi) flangerPhase -= juce::MathConstants<float>::twoPi;

        float gate = gh > 0.001f ? (0.18f + 0.82f * (0.5f + 0.5f * std::sin(ghostPhase))) : 1.0f;
        ghostPhase += juce::MathConstants<float>::twoPi * (2.0f + rate * 2.0f) / (float)sr;
        if (ghostPhase > juce::MathConstants<float>::twoPi) ghostPhase -= juce::MathConstants<float>::twoPi;
        xL *= (1.0f - gh) + gh * gate; xR *= (1.0f - gh) + gh * gate;

        xL = lowpassL.processSample(xL); xR = lowpassR.processSample(xR);

        if (bc > 0.001f && (++crushCounter % juce::jmax(1, (int)(1 + bc*22))) == 0)
        { crushHoldL=xL; crushHoldR=xR; }
        if (bc > 0.001f) { xL = xL*(1-bc) + crushHoldL*bc; xR = xR*(1-bc) + crushHoldR*bc; }

        if (ds > 0.001f)
        { float drive=1.0f+ds*24.0f; xL=std::tanh(xL*drive); xR=std::tanh(xR*drive); }

        if (dl > 0.001f)
        {
            int tap = (delayWrite - (int)(sr*(0.12f + speed*0.38f)) + delaySize) % delaySize;
            xL += dL[tap] * dl * 0.55f; xR += dR[tap] * dl * 0.55f;
        }
        dL[delayWrite]=xL; dR[delayWrite]=xR;
        delayWrite=(delayWrite+1)%delaySize;

        if (rv > 0.001f)
        {
            // Lightweight multi-tap room smear using the shared delay memory.
            int t1 = (delayWrite - (int)(sr * 0.031f) + delaySize) % delaySize;
            int t2 = (delayWrite - (int)(sr * 0.047f) + delaySize) % delaySize;
            int t3 = (delayWrite - (int)(sr * 0.071f) + delaySize) % delaySize;
            float roomL = (dL[t1] + dL[t2] * 0.7f + dL[t3] * 0.5f) * 0.22f;
            float roomR = (dR[t1] + dR[t2] * 0.7f + dR[t3] * 0.5f) * 0.22f;
            xL = xL * (1.0f - rv * 0.55f) + roomL * rv;
            xR = xR * (1.0f - rv * 0.55f) + roomR * rv;
        }

        float trem = hp > 0.001f ? (0.45f + 0.55f * (0.5f+0.5f*std::sin(helicopterPhase))) : 1.0f;
        helicopterPhase += juce::MathConstants<float>::twoPi * (5.0f+speed*18.0f)/(float)sr;
        if (helicopterPhase > juce::MathConstants<float>::twoPi) helicopterPhase -= juce::MathConstants<float>::twoPi;
        xL *= (1-hp)+hp*trem; xR *= (1-hp)+hp*trem;

        sL[stutterWrite]=xL; sR[stutterWrite]=xR;
        if (st > 0.001f)
        {
            int period=juce::jmax(32,(int)(sr/(4.0f+rate*8.0f)));
            int pos=(int)(stutterWrite%period);
            if (pos < period/2) { int back=(stutterWrite-pos+stSize)%stSize; xL=sL[back]; xR=sR[back]; }
            else { xL*=1.0f-st; xR*=1.0f-st; }
        }
        stutterWrite=(stutterWrite+1)%stSize;

        float leftGain=std::sqrt(0.5f*(1.0f-pan));
        float rightGain=std::sqrt(0.5f*(1.0f+pan));
        xL *= leftGain*1.4142f; xR *= rightGain*1.4142f;
        L[i]=dry.getSample(0,i)*(1-mix)+xL*mix;
        if (R) R[i]=dry.getSample(1,i)*(1-mix)+xR*mix;
    }
}

juce::AudioProcessorEditor* UGFXAudioProcessor::createEditor() { return new UGFXAudioProcessorEditor(*this); }

void UGFXAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{ auto s=parameters.copyState(); std::unique_ptr<juce::XmlElement> xml(s.createXml()); copyXmlToBinary(*xml,dest); }
void UGFXAudioProcessor::setStateInformation (const void* data, int size)
{ std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data,size)); if(xml && xml->hasTagName(parameters.state.getType())) parameters.replaceState(juce::ValueTree::fromXml(*xml)); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new UGFXAudioProcessor(); }
