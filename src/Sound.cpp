// GettingStarted.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "sndfile.h"
#include "Sound.h"
#include "Preprocessing.h"

#undef main
using namespace std;

Sound::Sound(vector <double> signal, int samples, int rate) : audioSignal(signal), sampleCount(samples), sampleRate(rate)
{

}

const int Sound::getSampleCount() const
{
	return this->sampleCount;
}

const int Sound::getSampleRate() const
{
	return this->sampleRate;
}

const vector <double> Sound::getAudioSignal() const
{
	return this->audioSignal;
}

Sound getSoundFromFile(const char * filepath)
{
	SF_INFO audioInfo;
	SNDFILE * sndFile = sf_open(filepath, SFM_READ, &audioInfo);

	double * buffer = new double[audioInfo.channels * audioInfo.frames];
	sf_readf_double(sndFile, buffer, audioInfo.frames);


	vector <double> signal = downMix(buffer, audioInfo.frames, audioInfo.channels);
	delete[] buffer;

	sf_close(sndFile);
	return Sound(signal, audioInfo.frames, audioInfo.samplerate);
}
