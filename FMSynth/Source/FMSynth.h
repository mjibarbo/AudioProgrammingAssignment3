/*
  ==============================================================================

    FMSynth.h
    Created: 7 Mar 2020 4:27:57pm
    Author:  Tom Mudd

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"
#include "Modulator.h"
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

public:

    FMSynthVoice() {}


    //-------------------------------------------------------------------------
    /**
     Set the envelope parameters from pointers

     @param attackPointer
     @param decayPointer
     @param sustainPointer
     @param releasePointer

     */

    void setEnvelopeParameterPointers(

        std::atomic<float>* attackPointer,
        std::atomic<float>* decayPointer,
        std::atomic<float>* sustainPointer,
        std::atomic<float>* releasePointer
   

    )
    {
        attack1 = attackPointer;
        decay1 = decayPointer;
        sustain1 = sustainPointer;
        release1 = releasePointer;

    }

    //--------------------------------------------------------------------------
    /**
    Function to initialise sample rate

     @param sampleRate

    */
    void setSampleRate(float sampleRate)
    {
        modulator.setSampleRate(sampleRate);
        carrier.setSampleRate(sampleRate);
        lfo1.setSampleRate(sampleRate);
        env1.setSampleRate(sampleRate);

        //Smooth values initialization (all values are being set to have a ramp lenght of 0.5 seconds)

        smoothAmount.reset(sampleRate, 0.5);
        smoothRatio.reset(sampleRate, 0.5);
        smoothLFOFreq.reset(sampleRate, 0.5);
        smoothGain.reset(sampleRate, 0.5);

        smoothAmount.setCurrentAndTargetValue(0.0f);
        smoothAmount.setCurrentAndTargetValue(0.0f);
        smoothAmount.setCurrentAndTargetValue(0.0f);
        smoothGain.setCurrentAndTargetValue(0.0f);
    }

    //--------------------------------------------------------------------------
    /**
    Set the modulator amount and ratio from the parameter pointers
    @param amountPointer
    @param ratioPointer

    */
    void setModulatorParametersFromPointers(
        std::atomic<float>* amountPointer,
        std::atomic<float>* ratioPointer
    )
    {
        amount1 = amountPointer;
        ratio1 = ratioPointer;

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

        //Envelope initialization

        env1.reset();
        env1.noteOn();

        //Envelope Parameters

        juce::ADSR::Parameters env1Params;

        env1Params.attack = *attack1;
        env1Params.decay = *decay1;
        env1Params.release = *release1;
        env1Params.sustain = *sustain1;

        env1.setParameters(env1Params);

    }

    //--------------------------------------------------------------------------
    /**
    Set LFO Frequency from Parameter Pointer

    @param frequencyPointer
    */
    void setLFOFrequencyFromParameterPointer(std::atomic<float>* frequencyPointer)
    {
        lfoFreq = frequencyPointer;
    
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
    
    /**
    Set wavetype for the modulator and LFO from parameter pointers

    @param wavetypePointer
    @param lfoWavetypePointer
    */
     void setWaveTypeFromParameterPointer(std::atomic<float>* wavetypePointer, std::atomic<float>* lfoWavetypePointer)
    {
       
       waveType = wavetypePointer;
        lfowaveType = lfoWavetypePointer;
        
    }

     /**
     Set gain from parameter pointer

     @param wavetypePointer
     @param lfoWavetypePointer
     */
     void setGainFromParameterPointer(std::atomic<float>* gainPointer)
     {

         gain = gainPointer;

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
            //Smoothed Values Target Setting
            smoothAmount.setTargetValue(*amount1);
            smoothRatio.setTargetValue(*ratio1);
            smoothLFOFreq.setTargetValue(*lfoFreq);
            smoothGain.setTargetValue(*gain);


            // iterate through the necessary number of samples (from startSample up to startSample + numSamples)
            for (int sampleIndex = startSample;   sampleIndex < (startSample+numSamples);   sampleIndex++)
            {
                //Smoothed Values Process

                float amountVal = smoothAmount.getNextValue();
                float ratioVal = smoothRatio.getNextValue();
                float lfoFreqVal = smoothLFOFreq.getNextValue();
                float gainVal = smoothGain.getNextValue();

                //LFO Process

                lfo1.setFrequency(lfoFreqVal);
                lfo1.setWaveTypeFromParameterPointer(lfowaveType);
                lfo1.process();

                //Modulator Process

                modulator.setFrequency(frequency + ratioVal); //Setting the frequency of the modulator to be the incoming MIDI + the value set in the ratio parameter
                modulator.setWaveTypeFromParameterPointer(waveType);
                float modulatorProcess = ((modulator.process() * 0.5f) + 0.5f)* (((lfo1.process() * 0.5f) + 0.5f) * (amountVal - 50) + 50); // Set the modulator output to be within the range 50 to the value set in the amount parameter
              
                //Carrier Process

                carrier.setFrequency(modulatorProcess);
               
                float carrierProcess = carrier.sineProcess() * 0.5f; //Reducing the final output gain to prevent clipping

                //Envelope Process

                float envValue = env1.getNextSample();
           

                float currentSample = (carrierProcess * gainVal) * envValue; //Applying the envelope to the carrier output
                
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

    //Modulator 

    Modulator modulator;

    std::atomic<float>* gain;

    //Modulator Parameters

    float frequency;
    std::atomic<float>* amount1;
    std::atomic<float>* ratio1;

    //Modulator Wavetype selection parameters 

    std::atomic<float>* waveType;

    //Carrier 

    Oscillator carrier;

    //LFO

    LFO lfo1;

    //LFO DSP parammeters 

    std::atomic<float>* lfoFreq;

    //LFO Wavetype selection parameters 

    std::atomic<float>* lfowaveType;

    //Envelope parameters 

    juce::ADSR env1;

    std::atomic<float>* attack1; 
    std::atomic<float>* decay1;
    std::atomic<float>* sustain1;
    std::atomic<float>* release1;

    

    //Smoothed Values

    juce::SmoothedValue<float> smoothAmount, smoothRatio, smoothLFOFreq, smoothGain;
   
};
