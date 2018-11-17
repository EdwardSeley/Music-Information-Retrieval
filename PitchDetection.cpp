#include "stdafx.h"
#include <iostream>
#include "Include/AudioFrame.h"
#include <vector>
#include "Include/AudioProcessing.h"
#include "Include/FeatureExtraction.h"
#include "map"
#include <functional>
#include <ctime>
#include <fstream>
#include <vector>
#include "Eigen/Dense"
#include <boost/filesystem.hpp>
#include "windows.h"
#include <thread>
#undef main

using namespace std;

map <double, string> NOTES = 
{ 
	{ 0, "None"}, {16.35, "C0"}, {17.32, "C#0"}, {18.35, "D0"}, {19.45, "D#0"}, {20.6, "E0"}, {21.83, "F0"}, {23.12, "F#0"}, {24.5, "G0"},
	{25.96, "G#0"}, {27.5, "A0"}, {29.14, "A#0"}, {30.87, "B0"}, {32.7, "C1"}, {34.65, "C#1"}, {36.71, "D1"}, {38.89, "D#1"}, {41.2, "E1"},
	{43.65, "F1"}, {46.25, "F#1"}, {49.0, "G1"}, {51.91, "G#1"}, {55.0, "A1"}, {58.27, "A#1"}, {61.74,"B1"}, {65.41, "C2"},
	{69.3, "C#2"}, {73.42, "D2"}, {77.78, "D#2"}, {82.41, "E2"}, {87.31, "F2"}, {92.5, "F#2"}, {98.0, "G2"}, {103.83, "G#2"},
	{110.0, "A2"}, {116.54, "A#2"}, {123.47, "B2"}, {130.81, "C3"}, {138.59, "C#3"}, {146.83, "D3"}, {155.56, "D#3"}, {164.81, "E3"},
	{174.61, "F3"}, {185.0, "F#3"}, {196.0, "G3"}, {207.65, "G#3"}, {220.0, "A3"}, {233.08, "A#3"}, {246.94, "B3"}, {261.63, "C4"},
	{277.18, "C#4"}, {293.66, "D4"}, {311.13, "D#4"}, {329.63, "E4"}, {349.23, "F4"}, {369.99, "F#4"}, {392.0, "G4"}, {415.3, "G#4"}, {440.0, "A4"},
	{466.16, "A#4"}, {493.88, "B4"} 
};

vector <double> freqs = 
{ 
	0.0, 16.35, 17.32, 18.35, 19.45, 20.6, 21.83, 23.12, 24.5, 25.96, 27.5, 29.14, 30.87, 32.7,
	34.65, 36.71, 38.89, 41.2, 43.65, 46.25, 49.0, 51.91, 55.0, 58.27, 61.74, 65.41, 69.3, 73.42, 77.78, 
	82.41, 87.31, 92.5, 98.0, 103.83, 110.0, 116.54, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 
	185.0, 196.0, 207.65, 220.0, 233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.0, 
	415.3, 440.0, 466.16, 493.88 
};

string frequencyToNote(double frequency)
{
	sort(freqs.begin(), freqs.end(), [&](int a, int b) {return abs(a - frequency) < abs(b - frequency); });
	return NOTES[freqs[0]];
}

int startServer(string fileName) {
	string command = "node Web/server.js " + fileName + ".csv";
	system(command.c_str());
	return 0;
}

namespace filesys = boost::filesystem;

int main(int argc, const char * argv[])
{
	int FRAME_SIZE = 32;
	int FRAME_SHIFT = (int)FRAME_SIZE / 2;
	const char * FILE_PATH;
	bool detectOnlyVoicedPitches = false;
	bool convertToNotes = false;
	bool createMusicSheet = false;

	if (argc > 1)
	{
		FILE_PATH = argv[1];
	}
	else {
		cout << "Please specify a WAV file." << endl;
		return 0;
	}

	if (argc > 2)
	{
		FRAME_SIZE = atoi(argv[2]);
	}
	if (argc > 3)
	{
		FRAME_SHIFT = atoi(argv[3]);
	}
	if (argc > 4)
	{
		if (string(argv[4]) == "VOICED")
			detectOnlyVoicedPitches = true;
	}
	if (argc > 5)
	{
		if (string(argv[5]) == "NOTES")
			convertToNotes = true;
	}
	if (argc > 6)
	{
		if (string(argv[6]) == "SHEET")
			createMusicSheet = true;
	}

	Sound sound = getSoundFromFile(FILE_PATH);
	vector <AudioFrame> audioFrames = getAudioFrames(sound, FRAME_SIZE, FRAME_SHIFT);

	vector <double> pitches(audioFrames.size());
	
	time_t startingTime;
	time_t endingTime;
	time(&startingTime);
	
	#pragma omp parallel for
	for (int i = 0; i < audioFrames.size(); i++)
	{
		AudioFrame audioFrame = audioFrames[i];
		pitches[i] = detectPitch(audioFrame, detectOnlyVoicedPitches);
	}
	
	vector <double> filteredPitches = removeSpikesAndValleys(pitches);
	

	ofstream outputFile;
	filesys::path pathObj(FILE_PATH);
	string name = pathObj.stem().string();
	string outputFilePath = "Output/" + name + ".csv";
	outputFile.open(outputFilePath);

	if (convertToNotes)
	{
		for (int i = 0; i < audioFrames.size(); i++)
		{
			outputFile << audioFrames[i].time << "," << frequencyToNote(filteredPitches[i]) << "," << endl;
		}
		outputFile.close();
		if (createMusicSheet) {
			string command = "node Web//server.js " + name + ".csv";
			thread serverThread(startServer, name);
			ShellExecute(0, 0, L"http://localhost:8080", 0, 0, SW_SHOW);
			system("pause");
		}
	}
	else
	{
		for (int i = 0; i < audioFrames.size(); i++)
		{
			outputFile << audioFrames[i].time << "," << filteredPitches[i] << "," << endl;
		}
		outputFile.close();
	}
	
	return 0;
}