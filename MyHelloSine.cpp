//-----------------------------------------------------------------------------
// name: MyHelloSine.cpp
// desc: real-time sine wave
//
// author: Nikhil Goel (nmgoel@stanford.edu)
//   date: fall 2015
//   uses: RtAudio by Gary Scavone
//-----------------------------------------------------------------------------
#include "RtAudio.h"
#include <math.h>
#include <iostream>
#include <cstdlib>
using namespace std;

/* #defines and globals */

// A sample is a discrete time signal derived from a continuous signal
#define SAMPLE double

// RtAudio's data format type. Normalized between +- 1
#define MY_FORMAT RTAUDIO_FLOAT64

// Sample Rate = (avg. # of samples)/second = 1/T, where T is sampling interval
#define MY_SRATE 44100

// number of channels
#define MY_CHANNELS 2

#define MY_PIE 3.14159265358979

// global for frequency
SAMPLE g_freq = 440;

// global sample number variable
SAMPLE g_t = 0;

/*TODO:
 * @funtion callme The RtAudioCallback function.
 * @param outputBuffer Pointer to the buffer that holds the output.
 * @param inputBuffer Pointer to the buffer that holds the input.
 * @param numFrames The number of sample frames held by input buffer
    and written to output buffer.
 * @param streamTime The number of seconds (double) that the signal streams.
 * @param status Notifies if there is an output/input overflow/underflow.
 * @param data Pointer to optional data provided by the client
    when opening the stream (default = NULL).
   @return Zero to maintain normal stream. One to stop the stream and drain the
   output buffer. Two to abort the stream immediately.
 */
int callme(void *outputBuffer, void *inputBuffer, int numFrames,
     double streamTime, RtAudioStreamStatus status, void *data ) {
         
}
