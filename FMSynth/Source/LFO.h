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

    void setFrequency(float _frequency)
    {
        osc.setFrequency(_frequency);
    }

    void setWaveTypeFromParameterPointer(std::atomic<float>* waveTypeIn)
    {
        waveTypeParamInt = static_cast<int>(*waveTypeIn);
    }

    void setWaveType(WaveType newWaveType)
    {
        switch (static_cast<WaveType>(waveTypeParamInt))
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

    float process()
    {
        output = osc.sineProcess();
        return output;
    }

private:

    Oscillator osc;

    float output;

    int waveTypeParamInt;


};
