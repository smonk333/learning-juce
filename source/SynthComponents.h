//
// Created by gboling on 5/22/2025.
//
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

#ifndef SYNTHCOMPONENTS_H
#define SYNTHCOMPONENTS_H

struct SineWaveSound : public juce::SynthesiserSound
{
    bool appliesToNote(int) override { return true;}
    bool appliesToChannel(int) override { return true; }
};

struct SineWaveVoice : public juce::SynthesiserVoice
{
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override
    {
        DBG("Midi note received");
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();
        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }

    void stopNote (float, bool allowTailOff) override
    {
        DBG("MIDI note stopped");
        if (allowTailOff && tailOff == 0.0)
            tailOff = 1.0;
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void renderNextBlock (juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = (float) (std::sin (currentAngle) * level * tailOff);
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;
                    tailOff *= 0.99;

                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = static_cast<float> (std::sin (currentAngle) * level);
                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }
    void pitchWheelMoved(int /*newPitchWheelValue*/) override
    {
        // Handle pitch wheel changes if needed
    }

    void controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/) override
    {
        // Handle MIDI controller changes if needed
    }
private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
};

class SynthAudioSource : public juce::AudioSource
{
public:
    explicit SynthAudioSource (juce::MidiKeyboardState& keyState) : keyboardState (keyState)
    {
        for (auto i = 0; i < 4; ++i)
            synth.addVoice (new SineWaveVoice());
        synth.addSound (new SineWaveSound());
    }

    void prepareToPlay (int, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate (sampleRate);
    }

    void releaseResources() override {}

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();
        juce::MidiBuffer incomingMidi;
        keyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);
        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    }

private:
    juce::MidiKeyboardState& keyboardState;
    juce::Synthesiser synth;
};

#endif //SYNTHCOMPONENTS_H
