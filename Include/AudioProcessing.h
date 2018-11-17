#pragma once
#include "fftw3.h"
#include <vector>

vector <double> calculateFFT(vector <double> signal);
fftw_complex * calculateComplexFFT(vector <double> signal);
vector <double> calculateInverseFFT(fftw_complex * amplitudes, int sampleCount);
vector <double> calculateDCT(vector <double> signal);
vector <double> calculatePowerSpectrum(vector <double> signal);
vector <double> calculatePositiveFrequencies(int size, int sampleRate);
vector <double> calculateFilterBanks(vector <double> powerSpectrum, int sampleRate, int bankCount);
vector <double> calculateDiffs(vector <double> signal);
vector <double> calculateAutocorrelation(vector <double> signal);
vector <double> calculateDerivative(vector <double> seq);
vector <int> getZeroCrossings(vector <double> signal);
vector <int> averageLocalClusters(vector <int> seq);
int argMax(vector <double> signal, int sampleCount);
double mean(vector <double> signal);
vector <double> absoluteValue(vector <double> signal);
vector <double> getZeroPaddedSeq(vector <double> signal, int paddingSize);
vector <double> reverseSeq(vector <double> signal);
vector <double> removeSpikesAndValleys(vector <double> seq);
vector <int> getPeaks(vector <double> seq);