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

    void setSampleRate(float _sampleRate)
    {
        osc.setSampleRate(_sampleRate);
    }

    void setFrequency(float frequencyIn)
    {

        osc.setFrequency(frequencyIn);
    }

  
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

    void setWaveTypeFromParameterPointer(std::atomic<float>* waveTypeIn)
    {
        waveTypeParamInt = static_cast<int>(*waveTypeIn);
        wavetype = static_cast<WaveType>(waveTypeParamInt);
        setWaveType(wavetype);
    }


    float process()
    {
        return waveOutput;
    }

private:

    Oscillator osc;

    float waveOutput;

    int waveTypeParamInt;

    LFO::WaveType wavetype;




};
