#pragma once
#include "fftw3.h"
#include <vector>

vector <double> calculateFFT(vector <double> signal, int sampleCount);
fftw_complex * calculateComplexFFT(vector <double> signal, int sampleCount);
vector <double> calculateInverseFFT(fftw_complex * amplitudes, int sampleCount);
vector <double> calculateDCT(vector <double> signal, int size);
vector <double> calculatePowerSpectrum(vector <double> signal, int sampleCount);
vector <double> calculatePositiveFrequencies(int size, int sampleRate);
vector <double> calculateFilterBanks(vector <double> powerSpectrum, int fftSize, int sampleRate, int bankCount);
vector <double> calculateAutoCorrelation(vector <double> signal, int sampleCount);
vector <double> calculateDiffs(vector <double> signal, int sampleCount);
int argMax(vector <double> signal, int sampleCount);
int countZeroCrossings(vector <double> signal, int sampleCount);
double mean(vector <double> signal, int size);
vector <double> absoluteValue(vector <double> signal, int size);
vector <double> getZeroPaddedSeq(vector <double> signal, int size, int paddingSize);
vector <double> reverseSeq(vector <double> signal, int size);