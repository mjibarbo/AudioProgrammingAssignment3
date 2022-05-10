/*
  ==============================================================================

    LFO.h
    Created: 27 Apr 2022 7:40:13pm
    Author:  mjiba

  ==============================================================================
*/

#pragma once
#include "Oscillator.h"

class LFO

{
    enum class WaveType { Sine, Triangle, Square, Saw };

public:

    //-------------------------------------------------------------------------
    /**
    Set LFO sample rate

    @param _sampleRate

    */
    void setSampleRate(float _sampleRate)
    {
        osc.setSampleRate(_sampleRate);
    }

    //-------------------------------------------------------------------------
    /**
    Set LFO frequency

    @param _frequency

    */
    void setFrequency(float _frequency)
    {

        osc.setFrequency(_frequency);
    }

    //-------------------------------------------------------------------------
    /**
    Select LFO wavetype

    @param newWaveType

    */
    void setWaveType(WaveType newWaveType)
    {
        switch (newWaveType)
        {
        case WaveType::Sine:

            waveOutput = osc.sineProcess();

            break;

        case WaveType::Triangle:

            waveOutput = osc.triangleProcess();

            break;

        case WaveType::Square:

            waveOutput = osc.squareProcess();

            break;

        case WaveType::Saw:

            waveOutput = osc.sawProcess();

            break;

        default:

            waveOutput = osc.sineProcess();


        }
    }

    //-------------------------------------------------------------------------
    /**
    Set the modulator wavetype from the parameter pointer

    @param wavetypePointer

    */
    void setWaveTypeFromParameterPointer(std::atomic<float>* waveTypeIn)
    {
        waveTypeParamInt = static_cast<int>(*waveTypeIn); //Casting the wavetype to an integer
        wavetype = static_cast<WaveType>(waveTypeParamInt); //Casting the wavetype to a WaveType enum 
        setWaveType(wavetype); //Setting the wavetype 
    }


    float process()
    {
        return waveOutput;
    }

private:

    Oscillator osc; //Instance of an oscillator 

    //Wavetype selection parameters 
    int waveTypeParamInt;

    LFO::WaveType wavetype;

    float waveOutput; //Variable for the LFO output 




};
