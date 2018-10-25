#include "stdafx.h"
#include <iostream>
#include "Sound.h"
#include "AudioFrame.h"
#include <fstream>
#include <vector>
#include "AudioProcessing.h"
#include "Eigen/Dense"
#include <functional>
#undef main

using namespace std;
using namespace Eigen;


vector <double> calculateMFCC(AudioFrame frame)
{
	vector <double> signal = frame.signal;
	int sampleCount = signal.size();
	int sampleRate = frame.sampleRate;

	vector <double> powerSpectrum = calculatePowerSpectrum(signal);
	const int bankCount = 40;
	vector <double> filterBanks = calculateFilterBanks(powerSpectrum, sampleRate, bankCount);
	vector <double> logFilterBanks(bankCount);
	for (int i = 0; i < bankCount; i++)
	{
		logFilterBanks[i] = log10(filterBanks[i]);
	}
	return calculateDCT(logFilterBanks);
}

double sigmoid(double val)
{
	return 1 / (1 + exp(-val));
}

bool VoiceActivityDetection(AudioFrame frame, vector <double> corr)
{
	vector <double> signal = frame.signal;
	int sampleCount = signal.size();
	int mid_length = int(round(sampleCount/2));
	if (mean(absoluteValue(signal)) == 0)
		return false;
	if (getZeroCrossings(signal).size() / sampleCount > 0.25)
		return false;
	
	vector <double> firstHalf(&signal[0], &signal[mid_length - 1]);
	vector <double> secondHalf(&signal[mid_length], &signal[sampleCount - 1]);
	
	double volume = mean(absoluteValue(signal));
	double volumeBalance = (mean(absoluteValue(firstHalf)) - mean(absoluteValue(secondHalf))) / volume;
	
	Map <VectorXd> signalLag(&signal[1], sampleCount-1);
	Map <VectorXd> signalReduced(&signal[0], sampleCount-1);
	double normalized_corr_coeff = signalLag.dot(signalReduced) / pow(signalLag.array().pow(2).sum() * signalReduced.array().pow(2).sum(), 0.5);
	Map <VectorXd> signalVec(&signal[0], sampleCount);
	double logEnergy = log(signalVec.array().pow(2).sum());
	double correlationCrossingRate = (double) getZeroCrossings(corr).size() / corr.size();
	
	Vector4d features = { normalized_corr_coeff, logEnergy, volumeBalance, correlationCrossingRate};
	Vector4d weights = { 1.28793598,   0.91974709,  -2.33882374, -15.2427123 };
	bool voiceActivityEstimate = bool(round(sigmoid(features.dot(weights) + 3.42915147)));
	return voiceActivityEstimate;
}

double detectPitch(AudioFrame frame, bool onlyHasVoice)
{
	vector <double> signal = frame.signal;
	int sampleCount = signal.size();
	int sampleRate = frame.sampleRate;

	vector <double> corr = calculateAutocorrelation(signal);
	
	if (onlyHasVoice & !VoiceActivityDetection(frame, corr))
		return 0;

	vector <int> maxima = getPeaks(corr);
	if (maxima.size() > 100)
		return 0;
	vector <int> averages = averageLocalClusters(maxima);
	sort(averages.begin(), averages.end(), [&](int a, int b) {return corr[a] > corr[b]; });
	
	if (averages.size() < 3)
		return 0;

	int peak;
	
	if (averages[0] < 30)
		averages.erase(averages.begin());

	if (corr[averages[0]] < 0.3)
	{
		sort(maxima.begin(), maxima.end(), [&](int a, int b) {return corr[a] > corr[b]; });
		maxima.erase(maxima.begin());
		peak = maxima[0];
	}
	else {
		peak = averages[0];
	}

	double pitch = (double) sampleRate / peak;
	
	if (pitch > 500)
		return 0;
	else
		return pitch;
}
