#pragma once
#include <vector>
#include "AudioFrame.h"

using namespace std;


vector <double> calculateMFCC(AudioFrame frame);
bool VoiceActivityDetection(AudioFrame frame);
double detectPitch(AudioFrame frame);