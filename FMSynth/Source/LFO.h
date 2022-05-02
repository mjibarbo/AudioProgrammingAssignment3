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

    enum class Route {Op1Amount,Op1Ratio,Op2Amount,Op2Ratio};

public:

    void setSampleRate(float _sampleRate)
    {
        osc.setSampleRate(_sampleRate);
    }

    void setFrequency(std::atomic<float>* frequencyIn)
    {

        osc.setFrequency(*frequencyIn);
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

    void setRoute(Route newRoute)
    {
        switch (newRoute)
        {
        case Route::Op1Amount:

            lfoOutput = (((waveOutput * 0.5f) + 0.5f) * (1000 - 50) + 50);

            break;

        case Route::Op1Ratio:

            lfoOutput = (((waveOutput * 0.5f) + 0.5f) * (1000 - 0) + 0);

            break;

        case Route::Op2Amount:

            lfoOutput = (((waveOutput * 0.5f) + 0.5f) * (1000 - 50) + 50);

            break;

        case Route::Op2Ratio:

            lfoOutput = (((waveOutput * 0.5f) + 0.5f) * (1000 - 0) + 0);

            break;


        }

    }

    void setRouteFromParameterPointer(int routeIn)
    {
        
        route = static_cast<Route>(routeIn);
        setRoute(route);
    }

    float process()
    {
        return lfoOutput;
    }

private:

    Oscillator osc;

    float waveOutput;

    float lfoOutput;

    int waveTypeParamInt;

    int routeParamInt;

    LFO::WaveType wavetype;

    LFO::Route route;


};
