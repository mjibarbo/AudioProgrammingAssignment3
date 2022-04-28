/*
  ==============================================================================

    Operator.h
    Created: 21 Apr 2022 6:04:19pm
    Author:  mjiba

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"

class Operator

{
    enum class WaveType { Sine, Triangle, Square, Saw };

public: 

    void setSampleRate(float _sampleRate)
    {
        osc.setSampleRate(_sampleRate);
        env1.setSampleRate(_sampleRate);
    }

    void setFrequency(float _frequency)
    {
        osc.setFrequency(_frequency);
    }
    
    void setEnvelope(float _attack1, float _decay1, float _release1, float _sustain1)
    {

        env1.reset();
        env1.noteOn();

        juce::ADSR::Parameters env1Params;

        env1Params.attack = _attack1;
        env1Params.decay = _decay1;
        env1Params.release = _release1;
        env1Params.sustain = _sustain1;

        env1.setParameters(env1Params);
    }

  

    void stopNote()
    {
        env1.noteOff();

    }

   

    //-------------------------------------------------------------------------
 /**
  Select wave type

  @param newWaveType

  */

   void setWaveType(WaveType newWaveType)
   {

       switch (newWaveType)
       {
       case WaveType::Sine:

           output = osc.sineProcess();

           break;

       case WaveType::Triangle:

           output = osc.triangleProcess();

           break;

       case WaveType::Square:

           output = osc.squareProcess();

           break;

       case WaveType::Saw:

           output = osc.sawProcess();

           break;

       default:

           output = osc.sineProcess();


       }
   }

   void setWaveTypeFromParameterPointer(std::atomic<float>* waveTypeIn)
   {
       waveTypeParamInt = static_cast<int>(*waveTypeIn);
       wavetype = static_cast<WaveType>(waveTypeParamInt);
       setWaveType(wavetype);
   }

   float getEnvelopeVal()
   {
       envVal = env1.getNextSample();
       return envVal;
   }

   float process(std::atomic<float>* amountIn)
   {
       float amount = *amountIn;
       //output = osc.sineProcess() * amount;
       return output;
   }


private:

    
    Oscillator osc;

    //Oscillator Parameters

    float frequency;

    //Envelope Parameters

    std::atomic<float>* attack1;
    std::atomic<float>* decay1;
    std::atomic<float>* sustain1;
    std::atomic<float>* release1;
    
    juce::ADSR env1;

    float envVal;

    //Wavetype selection parameters 

    int waveTypeParamInt;

    Operator::WaveType wavetype;

    float output;

};