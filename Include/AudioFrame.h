#pragma once
#include "Sound.h"
#include <vector>

using namespace std;

class AudioFrame
{
public:
	AudioFrame(vector<double> signal, double time, int sampleRate);
	vector <double> signal;
	double time;
	int sampleRate;
};

vector <AudioFrame> getAudioFrames(Sound sound, int frameSize, int frameShift);