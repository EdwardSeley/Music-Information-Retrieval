#include "stdafx.h"
#include <math.h>
#include <vector>
#include "Sound.h"
#include "AudioFrame.h"
#include "Preprocessing.h"

#undef main
using namespace std;

AudioFrame::AudioFrame(vector <double> signal, double seconds, int rate, int samples) : signal(signal),
time(seconds),
sampleRate(rate),
sampleCount(samples)
{
	DCOffset(signal, sampleCount);
	windowSignal(signal, sampleCount);
}

vector<AudioFrame> getAudioFrames(Sound sound, int frameSize, int frameShift)
{
	const vector <double> totalSamples = sound.audioSignal;
	const int sampleCount = sound.sampleCount;
	const int sampleRate = sound.sampleRate;

	int numOfFrames = floor(sampleCount / frameShift);
	vector <AudioFrame> audioFrames;

	int sample = 0;
	int frameIndex = 0;
	while (sample < sampleCount - frameSize + 1)
	{
		vector <double> frameSignal(frameSize);
		
		int startingSample = sample;
		for (int i = 0; i < frameSize && sample < sampleCount; i++)
		{
			frameSignal[i] = totalSamples[sample];
			sample++;
		}
		int endingSample = sample - 1;
		double time = (double) (startingSample + endingSample) / (2 * sampleRate);
		AudioFrame frame = AudioFrame(frameSignal, time, sampleRate, frameSize);
		audioFrames.push_back(frame);
		sample = sample - frameSize + frameShift;
		frameIndex++;
	}
	return audioFrames;
}