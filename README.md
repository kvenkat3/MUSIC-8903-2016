# MUSIC-8903-2016
Repo for the class Audio Software Engineering Spring 2016

1.
Our process function is not in place. Our modified signal is a separate output. 
Our code works for all sample rates and channel configurations.
The maximum allowed delay is 20ms, regardless of incoming audio sample rate.
Parameters that can be controlled via command line are the input .wav file, frequency of modulation, 
and depth of modulation in seconds.

4.
Our command line format is: wav file, frequency of modulation (Hz), modulation depth (s)

5.
We used the an impulse for our first test. Values were comparable with MATLAB with 1E-5.
For our second test we used the ‘2note_test.wav’ file. Values were comparable with MATLAB with a 1.0235E-4 maximum error.

6.
We implemented the following:
  - output equals delayed input when modulation amplitude is 0
  - DC input stays DC ouput regardless of parametrization
  - varying input block size
  - zero input signal
  - zero frequency input which returns the original signal with a fixed delay.
