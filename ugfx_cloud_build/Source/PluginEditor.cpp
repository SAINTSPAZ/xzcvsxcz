#include "PluginEditor.h"

static const juce::Colour pink(0xffffeaf5), magenta(0xffc02b91), darkPink(0xff870052), ink(0xff20232b);

UGFXAudioProcessorEditor::UGFXAudioProcessorEditor(UGFXAudioProcessor& p) : AudioProcessorEditor(&p), audio(p)
{
    setSize(560, 440);
    addKnob("speed","Speed",{30,340,145,76});
    addKnob("pan","Pan",{207,340,145,76});
    addKnob("mix","Mix",{385,340,145,76});
    addKnob("reverb","Reverb",{28,240,145,82});
    addKnob("distortion","Distortion",{207,240,145,82});
    addKnob("delay","Delay",{385,240,145,82});
    addSwitch("bitcrush","Bitcrush",{30,190,150,32});
    addSwitch("ghost","Ghost",{205,190,150,32});
    addSwitch("underwater","Underwater",{380,190,150,32});
    addSwitch("helicopter","Helicopter",{205,300,150,30});
    addSwitch("stutter","Stutter",{205,315,150,30});
    // Additional horizontal amount controls for the modulation effects.
    addKnob("flanger","Flanger",{155,115,250,70});
}
void UGFXAudioProcessorEditor::addKnob(const juce::String& id,const juce::String& name,juce::Rectangle<int> area)
{
    auto* s=sliders.add(new juce::Slider());
    s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s->setTextBoxStyle(juce::Slider::TextBoxBelow,false,62,18);
    s->setColour(juce::Slider::rotarySliderFillColourId,magenta);
    s->setColour(juce::Slider::thumbColourId,ink);
    s->setColour(juce::Slider::textBoxTextColourId,ink);
    s->setColour(juce::Slider::textBoxBackgroundColourId,pink);
    addAndMakeVisible(s);
    sliderAttachments.add(new SliderAttachment(audio.parameters,id,*s));
    auto* l=labels.add(new juce::Label()); l->setText(name,juce::dontSendNotification);
    l->setJustificationType(juce::Justification::centred); l->setColour(juce::Label::textColourId,darkPink);
    l->setFont(juce::Font(14.0f,juce::Font::bold)); addAndMakeVisible(l);
    s->setBounds(area); l->setBounds(area.getX(),area.getY()-22,area.getWidth(),20);
}
void UGFXAudioProcessorEditor::addSwitch(const juce::String& id,const juce::String& name,juce::Rectangle<int> area)
{
    // Horizontal amount fader: 0 is bypass, 1 is maximum effect.
    auto* s=sliders.add(new juce::Slider());
    s->setSliderStyle(juce::Slider::LinearHorizontal);
    s->setTextBoxStyle(juce::Slider::TextBoxRight,false,42,18);
    s->setColour(juce::Slider::trackColourId,magenta);
    s->setColour(juce::Slider::thumbColourId,ink);
    s->setColour(juce::Slider::textBoxTextColourId,ink);
    s->setColour(juce::Slider::textBoxBackgroundColourId,pink);
    s->setTooltip(name + " amount");
    addAndMakeVisible(s);
    sliderAttachments.add(new SliderAttachment(audio.parameters,id,*s));
    s->setBounds(area);
}
void UGFXAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(pink);
    g.setColour(darkPink); g.fillRoundedRectangle(125.0f,18.0f,310.0f,72.0f,12.0f);
    g.setColour(juce::Colours::white); g.setFont(juce::Font(38.0f,juce::Font::bold));
    g.drawFittedText("UGFX",125,24,310,58,juce::Justification::centred,1);
    g.setColour(magenta); g.fillRoundedRectangle(18.0f,100.0f,524.0f,4.0f,2.0f);
    g.setColour(darkPink); g.setFont(juce::Font(11.0f));
    g.drawText("UNDERGROUND FX  •  VST3",18,420,524,14,juce::Justification::centred);
}
void UGFXAudioProcessorEditor::resized() {}
