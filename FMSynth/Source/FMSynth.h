/*
  ==============================================================================

    FMSynth.h
    Created: 7 Mar 2020 4:27:57pm
    Author:  Tom Mudd

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"
#include "Operator.h"

// ===========================
// ===========================
// SOUND
class FMSynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote      (int) override      { return true; }
    //--------------------------------------------------------------------------
    bool appliesToChannel   (int) override      { return true; }
};




// =================================
// =================================
// Synthesiser Voice - your synth code goes in here

/*!
 @class YourSynthVoice
 @abstract struct defining the DSP associated with a specific voice.
 @discussion multiple YourSynthVoice objects will be created by the Synthesiser so that it can be played polyphicially
 
 @namespace none
 @updated 2019-06-18
 */
class FMSynthVoice : public juce::SynthesiserVoice
{
    enum class WaveType {Sine, Triangle, Square, Saw};

public:

    FMSynthVoice() {}


    //-------------------------------------------------------------------------
    /**
     Set the envelope parameter pointers after

     @param attack
     @param decay
     @param sustain
     @param release

     */

    void setEnvelopeParameterPointers(

        std::atomic<float>* attack1In,

        std::atomic<float>* decay1In,

        std::atomic<float>* sustain1In,

        std::atomic<float>* release1In

    )
    {
        attack1 = attack1In;
        decay1 = decay1In;
        sustain1 = attack1In;
        release1 = attack1In;
    }

    /**
    Function to initialise sample rate

     @param sampleRate

    */


    void initialise(float sampleRate)
    {
        operator1.setSampleRate(sampleRate);

    }
  

    //--------------------------------------------------------------------------
    /**
     What should be done when a note starts

     @param midiNoteNumber
     @param velocity
     @param SynthesiserSound unused variable
     @param / unused variable
     */
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        playing = true;
        ending = false; 
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

        operator1.setFrequency(frequency);

        operator1.setEnvelope(*attack1, *decay1, *sustain1, *release1);

       
    }

    //--------------------------------------------------------------------------
    /// Called when a MIDI noteOff message is received
    /**
     What should be done when a note stops

     @param / unused variable
     @param allowTailOff bool to decie if the should be any volume decay
     */
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        
        operator1.stopNote();
        ending = true;
        
    }
    

    void setWaveTypeParameterPointer(std::atomic<float>* waveTypeIn)
    {
        operator1.setWaveTypeParameterPointer(waveTypeIn);
    }
 

    //--------------------------------------------------------------------------
    /**
     The Main DSP Block: Put your DSP code in here
     
     If the sound that the voice is playing finishes during the course of this rendered block, it must call clearCurrentNote(), to tell the synthesiser that it has finished

     @param outputBuffer pointer to output
     @param startSample position of first sample in buffer
     @param numSamples number of smaples in output buffer
     */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (playing) // check to see if this voice should be playing
        {
            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample;   sampleIndex < (startSample+numSamples);   sampleIndex++)
            {
                
                
                float operator1Process = operator1.process() * operator1.getEnvelopeVal();
           

                float currentSample = operator1Process; 
                
                // for each channel, write the currentSample float to the output
                for (int chan = 0; chan<outputBuffer.getNumChannels(); chan++)
                {
                    // The output sample is scaled by 0.2 so that it is not too loud by default
                    outputBuffer.addSample (chan, sampleIndex, currentSample * 0.2);
                }

                if (ending)
                {
                    if (envValue < 0.0001f)
                    {
                        clearCurrentNote();
                        playing = false;
                    }

                }
            }
        }
    }
    //--------------------------------------------------------------------------
    void pitchWheelMoved(int) override {}
    //--------------------------------------------------------------------------
    void controllerMoved(int, int) override {}
    //--------------------------------------------------------------------------
    /**
     Can this voice play a sound. I wouldn't worry about this for the time being

     @param sound a juce::SynthesiserSound* base class pointer
     @return sound cast as a pointer to an instance of YourSynthSound
     */
    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<FMSynthSound*> (sound) != nullptr;
    }
    //--------------------------------------------------------------------------
private:
    //--------------------------------------------------------------------------
    // Set up any necessary variables here
    /// Should the voice be playing?
    bool playing = false;
    bool ending = false; 


    //DSP Parameters

    float frequency;

    float envValue = operator1.getEnvelopeVal();

    //Wavetype selection parameters 

    std::atomic<float>* waveType;

    //Envelope parameters 

    std::atomic<float>* attack1; 
    std::atomic<float>* decay1;
    std::atomic<float>* sustain1;
    std::atomic<float>* release1;


    // Operators

    Operator operator1; //operator2, operator3, operator4;


   
};
