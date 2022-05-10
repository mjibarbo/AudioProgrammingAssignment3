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
        
    }

    void setFrequency(float _frequency)
    {
        osc.setFrequency(_frequency);
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


   float process()
   {
       
       return output;
   }


private:

    
    Oscillator osc;

    //Oscillator Parameters

    float frequency;
    std::atomic<float>* amount;
    std::atomic<float>* ratio;

    //Wavetype selection parameters 

    int waveTypeParamInt;

    Operator::WaveType wavetype;


    float output;

};