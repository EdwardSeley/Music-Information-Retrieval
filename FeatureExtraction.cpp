#include "stdafx.h"
#include <iostream>
#include "Sound.h"
#include "AudioFrame.h"
#include <fstream>
#include "AudioProcessing.h"
#include <vector>
#undef main
using namespace std;



vector <double> calculateMFCC(AudioFrame frame)
{
	vector <double> signal = frame.signal;
	int sampleCount = frame.sampleCount;
	int sampleRate = frame.sampleRate;

	vector <double> powerSpectrum = calculatePowerSpectrum(signal);
	const int bankCount = 40;
	vector <double> filterBanks = calculateFilterBanks(powerSpectrum, sampleRate, bankCount);
	vector <double> logFilterBanks(bankCount);
	for (int i = 0; i < bankCount; i++)
	{
		logFilterBanks[i] = log10(filterBanks[i]);
	}
	return calculateDCT(logFilterBanks, bankCount);
}

bool isUnvoiced(AudioFrame frame)
{
	vector <double> signal = frame.signal;
	int sampleCount = frame.sampleCount;
	if (countZeroCrossings(signal) > 90)
		return true;
	
	int mid_length = int(round(sampleCount / 2));
	vector <double> firstHalf(&signal[0], &signal[mid_length - 1]);
	vector <double> secondHalf(&signal[mid_length], &signal[sampleCount-1]);

	if (mean(absoluteValue(firstHalf)) < 0.005)
		return true;
	if (mean(absoluteValue(secondHalf)) < 0.005)
		return true;
	if (mean(absoluteValue(signal)) < 0.005)
		return true;
	return false;
}

double predictPitch(AudioFrame frame)
{
	vector <double> signal = frame.signal;
	int sampleCount = frame.sampleCount;
	int sampleRate = frame.sampleRate;

	if (isUnvoiced(frame))
		return 0;
	vector <double> corr = calculateAutoCorrelation(signal);
	vector <double> diffs = calculateDiffs(corr);
	int firstMinimumIndex = 0;
	for (int i = 0; i < sampleCount/2; i++)
	{
		if (diffs[i] > 0)
		{
			firstMinimumIndex = i;
			break;
		}
	}
	vector <double> afterMin(&corr[firstMinimumIndex], &corr[sampleCount/2]);
	int peakIndex = argMax(afterMin, sampleCount/2 - firstMinimumIndex) + firstMinimumIndex;
	return sampleRate / peakIndex;
}

int main()
{
	Sound sound = getSoundFromFile("example.wav");
	vector <AudioFrame> audioFrames = getAudioFrames(sound, 640, 320);
	AudioFrame audioFrame = audioFrames[23];
	double pitch = predictPitch(audioFrame);
	cout << "pitch: " << pitch << endl;
	system("pause");
	return 0;
}