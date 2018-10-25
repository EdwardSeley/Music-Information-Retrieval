#include "stdafx.h"
#include <math.h>
#include <vector>
#include "Sound.h"
#include "AudioFrame.h"
#include "Preprocessing.h"

#undef main
using namespace std;

AudioFrame::AudioFrame(vector <double> signal, double seconds, int rate) : signal(signal),
time(seconds),
sampleRate(rate)
{

}

vector<AudioFrame> getAudioFrames(Sound sound, int frameDuration, int durationShift)
{
	const vector <double> samples = sound.audioSignal;
	const int sampleCount = sound.sampleCount;
	const int sampleRate = sound.sampleRate;

	vector <AudioFrame> audioFrames;

	int frameSize = int(round((double)((double) frameDuration * sampleRate) / 1000));
	int frameShift = int(round((double)((double) durationShift * sampleRate) / 1000));
	
	for (int sample = 0; sample < sampleCount; sample += frameShift)
	{
		int endSample;
		if (sample + frameSize > sampleCount)
			endSample = sampleCount;
		else
			endSample = sample + frameSize;
		vector <double> frameSignal(samples.begin() + sample, samples.begin() + endSample);
		double time = (double)(sample + endSample) / (2 * sampleRate);
		AudioFrame frame(frameSignal, time, sampleRate);
		audioFrames.push_back(frame);
	}
	return audioFrames;
}
