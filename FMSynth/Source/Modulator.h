/*
  ==============================================================================

    Operator.h
    Created: 21 Apr 2022 6:04:19pm
    Author:  mjiba

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"

class Modulator

{
    enum class WaveType { Sine, Triangle, Square, Saw };

public: 

    //-------------------------------------------------------------------------
    /**
    Set modulator sample rate

    @param _sampleRate

    */
    void setSampleRate(float _sampleRate)
    {
        osc.setSampleRate(_sampleRate);
        
    }

    //-------------------------------------------------------------------------
    /**
    Set modulator frequency

    @param _frequency

    */
    void setFrequency(float _frequency)
    {
        osc.setFrequency(_frequency);
    }   

   

   //-------------------------------------------------------------------------
   /**
   Select modulator wavetype

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

      
   //-------------------------------------------------------------------------
   /**
   Set the modulator wavetype from the parameter pointer

   @param wavetypePointer

   */
   void setWaveTypeFromParameterPointer(std::atomic<float>* wavetypePointer)
   {
       waveTypeParamInt = static_cast<int>(*wavetypePointer); //Casting the wavetype to an integer
       wavetype = static_cast<WaveType>(waveTypeParamInt); //Casting the wavetype to a WaveType enum 
       setWaveType(wavetype); //Setting the wavetype 
   }


   float process()
   {
       
       return output;
   }


private:

    
    Oscillator osc; //Instance of an oscillator


    //Wavetype selection parameters 

    int waveTypeParamInt;

    Modulator::WaveType wavetype;


    float output; //Variable for the modulator output

};