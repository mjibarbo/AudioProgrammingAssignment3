#pragma once
#include <iostream>
#include <cmath>

class Oscillator
{

public:


	void setSampleRate(int newSampleRate) // Setter for sample rate 
	{
		sampleRate = newSampleRate;
	}


	void setFrequency(float newFrequency) // Setter for frequency
	{
		frequency = newFrequency;

	}


	float sineProcess()
	{
		phaseDelta = frequency / float(sampleRate);

		//Oscillator Phase Formula
		phase += phaseDelta;

		if (phase >= 1)
		{
			phase -= 1.0;
		}


		//Sine Oscillator Formula
		return sin(phase * 2 * 3.1415927f);
	}

	float triangleProcess()
	{
		phaseDelta = frequency / float(sampleRate);

		//Oscillator Phase Formula
		phase += phaseDelta;

		if (phase >= 1)
		{
			phase -= 1.0;
		}

		//Triangle Oscillator Formula
		return fabs(phase - 0.5) - 0.25;
	}

	float squareProcess()
	{
		phaseDelta = frequency / float(sampleRate);

		//Oscillator Phase Formula
		phase += phaseDelta;

		if (phase >= 1)
		{
			phase -= 1.0;
		}

		//Square Oscillator Formula 
		float outVal = 0.5;
		if (phase > 0.5)
			outVal = -0.5;

		return outVal;
	}

	float sawProcess()
	{
		phaseDelta = frequency / float(sampleRate);

		//Oscillator Phase Formula
		phase += phaseDelta;

		if (phase >= 1)
		{
			phase -= 1.0;
		}

		//Sawtooth Oscillator Formula 
		
		return phase - floor(phase);
	}

private:

	float phase = 0.0f;
	float phaseDelta;
	float frequency;
	int sampleRate;
};
