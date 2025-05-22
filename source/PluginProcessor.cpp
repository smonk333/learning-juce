#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthComponents.h"

//==============================================================================
PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synthAudioSource.prepareToPlay (samplesPerBlock, sampleRate);
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    synthAudioSource.releaseResources();
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::AudioSourceChannelInfo bufferToFill (&buffer, 0, buffer.getNumSamples());
    buffer.clear();
    synthAudioSource.getNextAudioBlock (bufferToFill);

    //=========================================================================
    // commented out for testing the synthesizer class
    //=========================================================================

    // // handle midi messages
    // for (const auto metadata : midiMessages) {
    //     const auto msg = metadata.getMessage();
    //     if (msg.isNoteOn()) {
    //         // find a free voice or steal the oldest one
    //         auto it = std::find_if(voices.begin(), voices.end(),
    //             [](const Voice& v) {
    //                 return !v.active;
    //             });
    //         if (it == voices.end() && voices.size() < maxVoices) {
    //             voices.push_back({});
    //             it = std::prev(voices.end());
    //         }
    //         if (it != voices.end()) {
    //             int velocity = msg.getVelocity();
    //             it->midiNote = msg.getNoteNumber();
    //             double freq = juce::MidiMessage::getMidiNoteInHertz(it->midiNote);
    //             it->angleDelta = 2.0 * juce::MathConstants<double>::pi * freq / getSampleRate();
    //             it->currentAngle = 0.0;
    //             it->level = 0.2f * (static_cast<float>(velocity) / 127.0f);
    //             it->active = true;
    //         }
    //     } else if (msg.isNoteOff()) {
    //         for (auto& v : voices) {
    //             if (v.active && v.midiNote == msg.getNoteNumber())
    //                 v.active = false;
    //         }
    //     }
    // }
    //
    // int activeVoices = 0;
    // for (const auto& v : voices)
    //     if (v.active) ++activeVoices;
    //
    // // render audio
    // for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
    //     float value = 0.0f;
    //     for (auto& v : voices) {
    //         if (v.active) {
    //             value += static_cast<float>(std::sin(v.currentAngle)) * v.level;
    //             v.currentAngle += v.angleDelta;
    //         }
    //     }
    //     // normalize to avoid clipping
    //     if (activeVoices > 0)
    //         value /= static_cast<float>(activeVoices);
    //     for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    //         buffer.setSample(channel, sample, value);
    // }


}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

