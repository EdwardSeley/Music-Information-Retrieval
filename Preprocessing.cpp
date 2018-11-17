#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <complex>
#include <vector>
#include "fftw3.h"

using namespace std;

void windowSignal(vector <double> signal, int sampleCount)
{
	for (int sample = 0; sample < sampleCount; sample++)
	{
		double multiplier = 0.5 - 0.5 * cos((2 * M_PI*sample) / (sampleCount - 1));
		signal[sample] = multiplier * signal[sample];
	}
}

void DCOffset(vector <double> signal, int sampleCount)
{
	double sampleSum = 0;
	for (int sample = 0; sample < sampleCount; sample++)
	{
		sampleSum += signal[sample];
	}
	double sampleAverage = sampleSum / sampleCount;

	for (int sample = 0; sample < sampleCount; sample++)
	{
		signal[sample] = signal[sample] - sampleAverage;
	}
}

vector <double> downMix(double * signal, int sampleCount, int channelCount)
{
	vector <double> monoSignal(sampleCount);
	for (int sample = 0; sample < sampleCount; sample++)
	{
		double channelSignalSum = 0;
		for (int channel = 0; channel < channelCount; channel++)
		{
			channelSignalSum = channelSignalSum + signal[sample * channelCount + channel];
		}
		monoSignal[sample] = channelSignalSum / channelCount;
	}
	return monoSignal;
}

