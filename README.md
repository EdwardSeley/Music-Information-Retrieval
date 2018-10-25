# Music Information Retrieval Library
A signal processing library that extracts musical information from audio files. The library processes an audio signal as a sequence of audio frames, with variable sample length and overlap. These frames are then used to predict the frame's pitch (using autocorrelation) or calculate the MFCC (Mel Frequency Cepstral Coefficients).

## Examples
```
$ ./PitchDetection.exe example.wav 32 16 VOICED
(0.016, 0 ), (0.032, 0 ), (0.048, 0 ), (0.064, 145.985 ), (0.08, 130.719 ), (0.096, 136.986 ), (0.112, 141.844 ), (0.128, 147.059 )...

$ ./PitchDetection.exe example.wav 32 16 VOICED NOTES
(0.016, None ), (0.032, None ), (0.048, None ), (0.064, D3 ), (0.08, C3 ), (0.096, C#3 ), (0.112, C#3 ), (0.128, D3 ) ...
```
