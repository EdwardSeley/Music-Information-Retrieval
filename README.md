# Music Information Retrieval Library
A signal processing library that extracts musical information from audio files. The library processes an audio signal as a sequence of audio frames, with variable sample length and overlap. These frames are then used to predict the frame's pitch (using autocorrelation) or calculate the MFCC (Mel Frequency Cepstral Coefficients).

## Example
```
$ ./PitchDetection.exe example.wav 32 16 VOICED NOTES
(0.016, None ), (0.032, None ), (0.048, None ), (0.064, D3 ), (0.08, C3 ), (0.096, C#3 ), (0.112, C#3 ), (0.128, D3 ), (0.144, D#3 ), (0.16, D#3 ), (0.176, None ), (0.192, None ), (0.208, None ), (0.224, G1 ), (0.24, D#3 ), (0.256, G3 ), (0.272, F3 ), (0.288, F3 ), (0.304, F3 ), (0.32, F3 ), (0.336, F3 ), (0.352, F3 ), (0.368, F3 ), (0.384, F3 ), (0.4, F3 ), (0.416, E3 ), (0.432, D#3 ) ...
```
