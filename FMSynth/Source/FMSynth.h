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
#include "LFO.h"

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
        std::atomic<float>* release1In,
        std::atomic<float>* attack2In,
        std::atomic<float>* decay2In,
        std::atomic<float>* sustain2In,
        std::atomic<float>* release2In,
        std::atomic<float>* attack3In,
        std::atomic<float>* decay3In,
        std::atomic<float>* sustain3In,
        std::atomic<float>* release3In,
        std::atomic<float>* attack4In,
        std::atomic<float>* decay4In,
        std::atomic<float>* sustain4In,
        std::atomic<float>* release4In

    )
    {
        attack1 = attack1In;
        decay1 = decay1In;
        sustain1 = sustain1In;
        release1 = release1In;

        attack2 = attack2In;
        decay2 = decay2In;
        sustain2 = sustain2In;
        release2 = release2In;

        attack3 = attack3In;
        decay3 = decay3In;
        sustain3 = sustain3In;
        release3 = release3In;

        attack4 = attack4In;
        decay4 = decay4In;
        sustain4 = sustain4In;
        release4 = release4In;
    }

    /**
    Function to initialise sample rate

     @param sampleRate

    */


    void setSampleRate(float sampleRate)
    {
        operator1.setSampleRate(sampleRate);
        operator2.setSampleRate(sampleRate);
        operator3.setSampleRate(sampleRate);
        operator4.setSampleRate(sampleRate);
        lfo1.setSampleRate(sampleRate);

    }

    void setOperatorDSPFromParameterPointer(std::atomic<float>* amountIn, std::atomic<float>* ratioIn)
    {
        amount1 = amountIn;
        ratio1 = ratioIn;
    

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
        float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber) + *ratio1;

        operator1.setFrequency(frequency);

        operator1.setEnvelope(*attack1, *decay1, *sustain1, *release1);


       
    }

    void setFrequencyFromParameterPointer(std::atomic<float>* frequencyIn)
    {
        lfo1.setFrequency(frequencyIn);
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
    

    void setWaveTypeFromParameterPointer(std::atomic<float>* waveTypeIn, std::atomic<float>* lfowaveTypeIn)
    {
       
        waveType = waveTypeIn;
        lfowaveType = lfowaveTypeIn;

        
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

                //Operator

                float envValue = operator1.getEnvelopeVal();

                operator1.setWaveTypeFromParameterPointer(waveType);
                
                float operator1Process = operator1.process(amount1) * envValue;

                //LFO

                lfo1.setWaveTypeFromParameterPointer(lfowaveType);
                lfo1.process();
           

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

    //Operator DSP Parameters

    std::atomic<float>* amount1;
    std::atomic<float>* ratio1;

    //Operator Wavetype selection parameters 

    std::atomic<float>* waveType;

    //Operator Envelope parameters 

    std::atomic<float>* attack1; 
    std::atomic<float>* decay1;
    std::atomic<float>* sustain1;
    std::atomic<float>* release1;

    std::atomic<float>* attack2;
    std::atomic<float>* decay2;
    std::atomic<float>* sustain2;
    std::atomic<float>* release2;

    std::atomic<float>* attack3;
    std::atomic<float>* decay3;
    std::atomic<float>* sustain3;
    std::atomic<float>* release3;

    std::atomic<float>* attack4;
    std::atomic<float>* decay4;
    std::atomic<float>* sustain4;
    std::atomic<float>* release4;

    //LFO Wavetype selection parameters 

    std::atomic<float>* lfowaveType;


    // Operators

    Operator operator1, operator2, operator3, operator4;

    //LFO

    LFO lfo1;


   
};
