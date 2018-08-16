#include "stdafx.h"
#include <math.h>
#include "Sound.h"
#include "AudioFrame.h"
#include <complex>
#include "fftw3.h"
#include "AudioProcessing.h"
#include <vector>

#define _USE_MATH_DEFINES
#include <iostream>
using namespace std;

vector <double> calculateFFT(vector <double> signal, int sampleCount)
{
	vector <double> amplitudes(sampleCount);
	fftw_plan p = fftw_plan_r2r_1d(sampleCount, &(signal)[0], &(amplitudes)[0], FFTW_R2HC, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
	return amplitudes;
}

fftw_complex * calculateComplexFFT(vector <double> signal, int sampleCount)
{
	fftw_complex * amplitudes = new fftw_complex[sampleCount];
	fftw_plan p = fftw_plan_dft_r2c_1d(sampleCount, &(signal)[0], amplitudes, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
	return amplitudes;
}

vector <double> calculateInverseFFT(fftw_complex * amplitudes, int sampleCount)
{
	vector <double> signal(sampleCount);
	fftw_plan p = fftw_plan_dft_c2r_1d(sampleCount, amplitudes, &(signal)[0], FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
	return signal;
}

vector <double> calculateDCT(vector <double> signal, int sampleCount)
{
	vector <double> coefficients(sampleCount * 2);
	fftw_plan p = fftw_plan_r2r_1d(sampleCount, signal.data(), &(coefficients)[0], FFTW_REDFT10, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);
	return coefficients;
}

vector <double> calculatePowerSpectrum(vector <double> signal, int sampleCount)
{
	vector <double> amplitudes = calculateFFT(signal, sampleCount);
	int powerSpectrumLength = sampleCount / 2 + 1;
	vector <double> powerSpectrum(powerSpectrumLength);
	powerSpectrum[0] = pow(amplitudes[0], 2);
	for (int i = 1; i < powerSpectrumLength; i++)
	{
		if (i == sampleCount - i)
		{
			powerSpectrum[i] = pow(amplitudes[i], 2);
		}
		powerSpectrum[i] = (pow(amplitudes[i], 2)
			+ pow(amplitudes[sampleCount - i], 2)) / powerSpectrumLength;
	}
	return powerSpectrum;
}

vector <double> calculatePositiveFrequencies(int size, int sampleRate)
{
	vector <double> frequencies(size);
	for (int i = 0; i < size; i++)
	{
		frequencies[i] = i * (sampleRate / (2 * size));
	}
	return frequencies;
}

vector <double> calculateFilterBanks(vector <double> powerSpectrum, int fftSize, int sampleRate, int bankCount)
{
	int bandWidth = sampleRate / 2;
	double melFreqBandwidth = 2595 * log10(1 + bandWidth / 700);
	vector <double> frequencies = calculatePositiveFrequencies(fftSize, sampleRate);
	vector <double> frequencyBins(fftSize);
	for (int i = 0; i < bankCount; i++)
	{
		double melFrequency = 50 + i * melFreqBandwidth / (bankCount - 1);
		frequencyBins[i] = 700 * (pow(10, melFrequency / 2595) - 1);
	}
	vector <double> filterBanks(bankCount);
	for (int bin_index = 0; bin_index < bankCount; bin_index++)
	{
		double energyValue = 0;
		for (int freq_index = 0; freq_index < fftSize; freq_index++)
		{
			int bin_size;
			if (bin_index == 0)
			{
				bin_size = frequencyBins[bin_index];
			}
			else {
				bin_size = frequencyBins[bin_index] - frequencyBins[bin_index - 1];
			}
			int distance_from_bin_center = abs(frequencies[freq_index] - frequencyBins[bin_index]);
			if (distance_from_bin_center < bin_size)
			{
				energyValue += powerSpectrum[freq_index] * distance_from_bin_center / bin_size;
			}
		}
		filterBanks[bin_index] = energyValue / fftSize;
	}
	return filterBanks;
}

vector <double> calculateAutoCorrelation(vector <double> signal, int sampleCount)
{
	vector <double> paddedSignal = getZeroPaddedSeq(signal, sampleCount, sampleCount);
	int paddedSize = sampleCount * 2;
	fftw_complex * forwardSignalFFT = calculateComplexFFT(paddedSignal, paddedSize);
	fftw_complex * backwardSignalFFT = calculateComplexFFT(reverseSeq(paddedSignal, paddedSize), paddedSize);
	fftw_complex * product = new fftw_complex[paddedSize];
	for (int i = 0; i < paddedSize; i++)
	{
		product[i][0] = forwardSignalFFT[i][0] * backwardSignalFFT[i][0] - forwardSignalFFT[i][1] * backwardSignalFFT[i][1];
		product[i][1] = forwardSignalFFT[i][1] * backwardSignalFFT[i][0] + forwardSignalFFT[i][0] * backwardSignalFFT[i][1];
	}
	vector <double> fullAutoCorrelation = calculateInverseFFT(product, paddedSize);
	vector <double> autoCorrelation(fullAutoCorrelation.begin(), fullAutoCorrelation.begin() + sampleCount);
	return autoCorrelation;
}

vector <double> calculateDiffs(vector <double> signal, int sampleCount)
{
	vector <double> diffs(sampleCount-1);
	for (int i = 0; i < sampleCount - 1; i++)
	{
		diffs[i] = signal[i + 1] - signal[i];
	}
	return diffs;
}

int argMax(vector <double> signal, int sampleCount)
{
	int maximumIndex = 0;
	for (int i = 0; i < sampleCount; i++)
	{
		if (signal[i] > signal[maximumIndex])
		{
			maximumIndex = i;
		}
	}
	return maximumIndex;
}

int countZeroCrossings(vector <double> signal, int sampleCount)
{
	int zeroCrossings = 0;
	for (int i = 0; i < sampleCount; i++)
	{
		if (i == sampleCount - 1)
			continue;
		if (signal[i] == 0 and signal[i + 1] != 0)
			zeroCrossings++;
		if ((signal[i] < 0 and signal[i + 1] > 0) or (signal[i] > 0 and signal[i + 1] < 0))
			zeroCrossings++;
	}
	return zeroCrossings;
}

double mean(vector <double> seq, int size)
{
	double sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum += seq[i];
	}
	return sum / size;
}

vector <double> absoluteValue(vector <double> seq, int size)
{
	vector <double> absoluteValueSeq(size);
	for (int i = 0; i < size; i++)
	{
		absoluteValueSeq[i] = abs(seq[i]);
	}
	return absoluteValueSeq;
}

vector <double> getZeroPaddedSeq(vector <double> seq, int size, int paddingSize)
{
	vector <double> zeroPadded(size + paddingSize);
	for (int i = 0; i < size + paddingSize; i++)
	{
		if (i >= size)
			zeroPadded[i] = 0;
		else
			zeroPadded[i] = seq[i];
	}
	return zeroPadded;
}

vector <double> reverseSeq(vector <double> seq, int size)
{
	vector <double> reverse(size);
	for (int i = 0; i < size; i++)
	{
		reverse[i] = seq[size - 1 - i];
	}
	return reverse;
}