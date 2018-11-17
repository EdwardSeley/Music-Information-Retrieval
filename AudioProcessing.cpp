#include "stdafx.h"
#include <math.h>
#include "Include/Sound.h"
#include "Include/AudioFrame.h"
#include <complex>
#include "fftw3.h"
#include "Include/AudioProcessing.h"
#include <vector>
#include <numeric> 
#include <map>
#include "Eigen/Dense"
#include <omp.h>


#define _USE_MATH_DEFINES
#include <iostream>
using namespace std;
using namespace Eigen;

constexpr auto M_PI = 3.14159265358979323846;

map <int, MatrixXd> sinusoidFactorCache;
map <int, vector <double>> indicesCache;
map <vector<double>, vector<double>> correlationCache;
int TYPICAL_BLOCK_SIZE = 1411;

MatrixXd calculateSinusoidFactor(int LENGTH);
MatrixXd sinusoidFactor1411 = calculateSinusoidFactor(TYPICAL_BLOCK_SIZE);

vector <double> calculateFFT(vector <double> signal)
{
	vector <double> amplitudes(signal.size());
	fftw_plan p;
	#pragma omp critical
	{
		p = fftw_plan_r2r_1d(signal.size(), &(signal)[0], &(amplitudes)[0], FFTW_R2HC, FFTW_ESTIMATE);
	}
	fftw_execute(p);
	#pragma omp critical
	{
		fftw_destroy_plan(p);
	}
	return amplitudes;
}

fftw_complex * calculateComplexFFT(vector <double> signal)
{
	fftw_complex * amplitudes = new fftw_complex[signal.size()];
	fftw_plan p;
	#pragma omp critical
	{
		p = fftw_plan_dft_r2c_1d(signal.size(), &(signal)[0], amplitudes, FFTW_ESTIMATE);
	}
	fftw_execute(p);
	#pragma omp critical
	{
		fftw_destroy_plan(p);
	}
	return amplitudes;
}

vector <double> calculateInverseFFT(fftw_complex * amplitudes, int sampleCount)
{
	vector <double> signal(sampleCount);
	fftw_plan p;
	#pragma omp critical
	{
		p = fftw_plan_dft_c2r_1d(sampleCount, amplitudes, &(signal)[0], FFTW_ESTIMATE);
	}
	fftw_execute(p);
	#pragma omp critical
	{
		fftw_destroy_plan(p);
	}
	return signal;
}

vector <double> calculateDCT(vector <double> signal)
{
	vector <double> coefficients(signal.size());
	fftw_plan p;
	#pragma omp critical
	{
		p = fftw_plan_r2r_1d(signal.size(), signal.data(), &(coefficients)[0], FFTW_REDFT10, FFTW_ESTIMATE);
	}
	fftw_execute(p);
	#pragma omp critical
	{
		fftw_destroy_plan(p);
	}
	return coefficients;
}

vector <double> calculatePowerSpectrum(vector <double> signal)
{
	vector <double> amplitudes = calculateFFT(signal);
	int powerSpectrumLength = signal.size() / 2 + 1;
	vector <double> powerSpectrum(powerSpectrumLength);
	powerSpectrum[0] = pow(amplitudes[0], 2);
	for (int i = 1; i < powerSpectrumLength; i++)
	{
		if (i == signal.size() - i)
		{
			powerSpectrum[i] = pow(amplitudes[i], 2);
		}
		powerSpectrum[i] = (pow(amplitudes[i], 2)
			+ pow(amplitudes[signal.size() - i], 2)) / powerSpectrumLength;
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

vector <double> calculateFilterBanks(vector <double> powerSpectrum, int sampleRate, int bankCount)
{
	int bandWidth = sampleRate / 2;
	double melFreqBandwidth = 2595 * log10(1 + bandWidth / 700);
	vector <double> frequencies = calculatePositiveFrequencies(powerSpectrum.size(), sampleRate);
	vector <double> frequencyBins(powerSpectrum.size());
	for (int i = 0; i < bankCount; i++)
	{
		double melFrequency = 50 + i * melFreqBandwidth / (bankCount - 1);
		frequencyBins[i] = 700 * (pow(10, melFrequency / 2595) - 1);
	}
	vector <double> filterBanks(bankCount);
	for (int bin_index = 0; bin_index < bankCount; bin_index++)
	{
		double energyValue = 0;
		for (int freq_index = 0; freq_index < powerSpectrum.size(); freq_index++)
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
		filterBanks[bin_index] = energyValue / powerSpectrum.size();
	}
	return filterBanks;
}

vector <double> calculateAutocorrelation(vector <double> signal)
{
	vector <double> paddedSignal = getZeroPaddedSeq(signal, signal.size());
	int paddedSize = signal.size() * 2;
	fftw_complex * forwardSignalFFT = calculateComplexFFT(paddedSignal);
	fftw_complex * backwardSignalFFT = calculateComplexFFT(reverseSeq(paddedSignal));
	fftw_complex * product = new fftw_complex[paddedSize];
	for (int i = 0; i < paddedSize; i++)
	{
		product[i][0] = forwardSignalFFT[i][0] * backwardSignalFFT[i][0] - forwardSignalFFT[i][1] * backwardSignalFFT[i][1];
		product[i][1] = forwardSignalFFT[i][1] * backwardSignalFFT[i][0] + forwardSignalFFT[i][0] * backwardSignalFFT[i][1];
	}
	vector <double> fullAutoCorrelation = calculateInverseFFT(product, paddedSize);
	vector <double> autoCorrelation(fullAutoCorrelation.begin(), fullAutoCorrelation.begin() + signal.size());

	delete[] forwardSignalFFT;
	delete[] backwardSignalFFT;
	delete[] product;
	
	return autoCorrelation;
}

vector <double> calculateDiffs(vector <double> signal)
{
	vector <double> diffs(signal.size() - 1);
	for (int i = 0; i < signal.size() - 1; i++)
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

vector <int> getZeroCrossings(vector <double> signal)
{
	vector<int> zeroCrossings;

	for (int i = 0; i < signal.size(); i++)
	{
		if (i == signal.size() - 1)
			continue;
		if (signal[i] == 0 and signal[i + 1] != 0)
			zeroCrossings.push_back(i);
		if ((signal[i] < 0 and signal[i + 1] > 0) or (signal[i] > 0 and signal[i + 1] < 0))
			zeroCrossings.push_back(i);
	}
	return zeroCrossings;
}

vector <double> arange(int size)
{
	vector <double> vec(size);
	for (int i = 0; i < size; i++)
		vec[i] = i;
	return vec;
}

MatrixXd calculateSinusoidFactor(int LENGTH)
{
	vector <double> t;
	if (indicesCache.count(LENGTH))
	{
		t = indicesCache[LENGTH];
	}
	else
	{
		t = arange(LENGTH);
		indicesCache[LENGTH] = t;
	}
	Map<MatrixXd> t_matrix(t.data(), LENGTH, 1);
	MatrixXd indices = M_PI / LENGTH * t_matrix * t_matrix.transpose();
	MatrixXd sinusoid(LENGTH, LENGTH);
	sinusoid = Eigen::sin(indices.array());
	return sinusoid;
}

vector <double> calculateDerivative(vector <double> seq)
{
	vector <double> dct = calculateDCT(seq);
	const int LENGTH = seq.size();
	vector <double> t;
	if (indicesCache.count(LENGTH))
	{
		t = indicesCache[LENGTH];
	}
	else
	{
		t = arange(LENGTH);
		indicesCache[LENGTH] = t;
	}
	Map<ArrayXd> dctVector(dct.data(), LENGTH);
	Map<ArrayXd> indicesMatrix(t.data(), LENGTH);
	MatrixXd derivativeMatrix = -2 * M_PI * dctVector * indicesMatrix;
	MatrixXd derivative;
	if (LENGTH == 1411)
	{
		derivative = (derivativeMatrix.transpose() * sinusoidFactor1411).array();
	}
	else {
		
		MatrixXd sinusoidFactor;
		if (sinusoidFactorCache.count(LENGTH))
		{
			sinusoidFactor = sinusoidFactorCache[LENGTH];
		}
		else
		{
			sinusoidFactor = calculateSinusoidFactor(LENGTH);
			sinusoidFactorCache[LENGTH] = sinusoidFactor;
		}
		derivative = (derivativeMatrix.transpose() * sinusoidFactor).array();

	}
	vector <double> deriv(derivative.data(), derivative.data() + derivative.size());
	return deriv;
}

vector <int> getPeaks(vector <double> seq)
{
	vector <double> deriv = calculateDerivative(seq);
	vector <int> zeroCrossings = getZeroCrossings(deriv);
	vector <int> maxima;
	for (int i = 0; i < zeroCrossings.size(); i++)
	{
		if (seq[zeroCrossings[i]] > 0)
			maxima.push_back(zeroCrossings[i]);
	}
	return maxima;
}

vector <int> averageLocalClusters(vector <int> seq)
{
	vector <int> averages;
	vector <int> cluster;
	int window_size = 10;
	for (int i = 0; i < seq.size(); i++)
	{
		cluster.push_back(seq[i]);
		if (i == seq.size() - 1 or abs(seq[i] - seq[i + 1]) > window_size or abs(seq[i] - cluster[0]) > window_size)
		{
			int average = int(round(accumulate(cluster.begin(), cluster.end(), 0.0) / cluster.size()));
			averages.push_back(average);
			cluster.clear();
		}
	}
	return averages;
}

double mean(vector <double> seq)
{
	double sum = 0;
	for (int i = 0; i < seq.size(); i++)
	{
		sum += seq[i];
	}
	return sum / seq.size();
}

vector <double> absoluteValue(vector <double> seq)
{
	vector <double> absoluteValueSeq(seq.size());
	for (int i = 0; i < seq.size(); i++)
	{
		absoluteValueSeq[i] = abs(seq[i]);
	}
	return absoluteValueSeq;
}

vector <double> getZeroPaddedSeq(vector <double> seq, int paddingSize)
{
	vector <double> zeroPadded(seq.size() + paddingSize);
	for (int i = 0; i < seq.size() + paddingSize; i++)
	{
		if (i >= seq.size())
			zeroPadded[i] = 0;
		else
			zeroPadded[i] = seq[i];
	}
	return zeroPadded;
}

vector <double> reverseSeq(vector <double> seq)
{
	vector <double> reverse(seq.size());
	for (int i = 0; i < seq.size(); i++)
	{
		reverse[i] = seq[seq.size() - 1 - i];
	}
	return reverse;
}

vector <double> removeSpikesAndValleys(vector <double> seq)
{
	vector <double> smoothSeq;

	int i = 0;
	while (i < seq.size())
	{
		double val = seq[i];
		vector <double> surroudingValues;
		for (int j = i; j < seq.size() && (val != 0 && seq[j] != 0 || val == 0 and seq[j] == 0); j++)
		{
			surroudingValues.push_back(seq[j]);
			i = j;
		}
		i += 1;

		if (surroudingValues.size() == 1 && i != 0 && i != seq.size() - 1 && val == 0)
		{
			smoothSeq.push_back((seq[i - 1] + seq[i + 1]) / 2);
			continue;
		}
		if (surroudingValues.size() < 3)
		{
			for (int m = 0; m < surroudingValues.size(); m++)
				smoothSeq.push_back(0);
			continue;
		}
		double averageSurroundingVal = mean(surroudingValues);

		for (int k = 0; k < surroudingValues.size(); k++)
		{

			if (val == 0)
				smoothSeq.push_back(surroudingValues[k]);
			else if (surroudingValues[k] > 1.3 * averageSurroundingVal)
				smoothSeq.push_back(averageSurroundingVal);
			else
				smoothSeq.push_back(surroudingValues[k]);
		}
	}
	return smoothSeq;
}