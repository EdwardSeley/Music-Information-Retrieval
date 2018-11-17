# Music Information Retrieval Library
A signal processing library that extracts musical information from audio files. The library processes an audio signal as a sequence of audio frames, with variable sample length and overlap. These frames are then used to predict the frame's pitch (using autocorrelation) or calculate the MFCC (Mel Frequency Cepstral Coefficients).

## Install
This repository utilizes the FFTW (The Fastest Fourier Transform in the West) library for fourier calculations, the Eigen library for matrix calculations, and the 

## Command Line Interface
usage: ./PitchDetection AUDIO FRAME_SIZE FRAME_JUMP VOICED/UNVOICED

Here AUDIO is a WAV audio file, FRAME_SIZE is the size of the audio sample from which the pitch is detected, FRAME_JUMP is how far the next frame is from the beginning of the current frame (By default, each frame overlaps by half its size), and VOICED is whether the pitch is coming from a human voice or an instrument.

## Examples

### Detecting Raw Pitches
```
$ ./PitchDetection example.wav 32 16 VOICED
(0.016, 0 ), (0.032, 0 ), (0.048, 0 ), (0.064, 145.985 ), (0.08, 130.719 ), (0.096, 136.986 ), (0.112, 141.844 ), (0.128, 147.059 )...
```

### Converting Detected Pitches to Notes
 ```
$ ./PitchDetection example.wav 32 16 VOICED NOTES
(0.016, None ), (0.032, None ), (0.048, None ), (0.064, D3 ), (0.08, C3 ), (0.096, C#3 ), (0.112, C#3 ), (0.128, D3 ) ...
```

### Generating Music Sheet
```
$ ./PitchDetection Resources/Audio-Samples/example.wav 32 16 VOICED NOTES SHEET
```
After the pitches have been recorded onto a CSV file, a Node.js server is started that reads the pitches from the CSV and creates a music transcription sheet is that is displayed at a local host (localhost:8080). 
![alt text](https://github.com/EdwardSeley/Music-Information-Retrieval/blob/master/Web/example_music_sheet.png)
