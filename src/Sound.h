#pragma once
#include <vector>

using namespace std;

class Sound
{
public:
	Sound(vector <double>, int, int);
	const vector <double> getAudioSignal() const;
	const int getSampleCount() const;
	const int getSampleRate() const;
	vector <double> audioSignal;
	const int sampleRate;
	const int sampleCount;
};
Sound getSoundFromFile(const char * filepath);
