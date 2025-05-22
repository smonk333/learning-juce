#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      keyboardComponent(audioProcessor.getKeyboardState(),
      juce::MidiKeyboardComponent::horizontalKeyboard)
{
    addAndMakeVisible(keyboardComponent);
    setSize (600, 200);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void PluginEditor::resized()
{
    keyboardComponent.setBounds (10, 10, getWidth() - 20, getHeight() - 20);
}
