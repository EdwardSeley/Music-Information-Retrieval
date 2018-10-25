#pragma once
#include <vector>

using namespace std;

void windowSignal(vector <double> signal, int sampleCount);
vector <double> downMix(double * signal, int sampleCount, int channels);
void DCOffset(vector <double> signal, int sampleCount);
