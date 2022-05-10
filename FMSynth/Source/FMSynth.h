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
        std::atomic<float>* release1In
   

    )
    {
        attack1 = attack1In;
        decay1 = decay1In;
        sustain1 = sustain1In;
        release1 = release1In;

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
        env1.setSampleRate(sampleRate);

    }

    void setOperatorDSPFromParameterPointer(
        std::atomic<float>* amountIn, 
        std::atomic<float>* ratioIn
    )
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
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        

        

        //Envelope 

        env1.reset();
        env1.noteOn();

        juce::ADSR::Parameters env1Params;

        env1Params.attack = *attack1;
        env1Params.decay = *decay1;
        env1Params.release = *release1;
        env1Params.sustain = *sustain1;

        env1.setParameters(env1Params);


       
    }

    void setLFOFrequencyFromParameterPointer(std::atomic<float>* frequencyIn)
    {
        lfoFreq = frequencyIn;

        
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
        
        env1.noteOff();
        ending = true;
        
    }
    

    void setWaveTypeFromParameterPointer(std::atomic<float>* waveTypeIn, std::atomic<float>* waveTypeIn2, std::atomic<float>* lfowaveTypeIn)
    {
       
        waveType = waveTypeIn;
        waveType2 = waveTypeIn2;
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

                //LFO

                lfo1.setFrequency(lfoFreq);
                lfo1.setWaveTypeFromParameterPointer(lfowaveType);
                lfo1.process();

                //Operator 1
                operator1.setFrequency(frequency + *ratio1);
                operator1.setWaveTypeFromParameterPointer(waveType);
                operator1Process = ((operator1.process() * 0.5f) + 0.5f)* (((lfo1.process() * 0.5f) + 0.5f) * (*amount1 - 50) + 50); // Set the operator 1 output to be within the range 50 - 1000
              
             

                //Operator 2

                operator2.setWaveTypeFromParameterPointer(waveType2);
                operator2.setFrequency(operator1Process);
               
                 float operator2Process = operator2.process();

                //Envelope

                float envValue = env1.getNextSample();
           

                float currentSample = (operator2Process * 0.1) * envValue; 
                
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

    float frequency;
    std::atomic<float>* amount1;
    std::atomic<float>* ratio1;

    //Operator Wavetype selection parameters 

    std::atomic<float>* waveType;

    //Operator 2 DSP Parameters

    std::atomic<float>* amount2;
    std::atomic<float>* ratio2;

    float operator1Process;

    //Operator Wavetype selection parameters 

    std::atomic<float>* waveType2;

    //Envelope parameters 

    std::atomic<float>* attack1; 
    std::atomic<float>* decay1;
    std::atomic<float>* sustain1;
    std::atomic<float>* release1;

    juce::ADSR env1;

    //LFO DSP parammeters 

    std::atomic<float>* lfoFreq;

    //LFO Wavetype selection parameters 

    std::atomic<float>* lfowaveType;

    //LFO Wavetype selection parameters 

    int lfoRouteInt;

    // Operators

    Operator operator1, operator2, operator3, operator4;

    //LFO

    LFO lfo1;


   
};
