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

/*
 * @funtion audio_callback The RtAudioCallback function.
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
int audio_callback(void *outputBuffer, void *inputBuffer, unsigned int numFrames,
     double streamTime, RtAudioStreamStatus status, void *data ) {

         // stderr prints info and err messages (info about callback here)
         cerr << ".";

         // outputBuffer points to array of SAMPLEs
         SAMPLE *buffer = (SAMPLE *) outputBuffer;
         for(int i = 0; i < numFrames; i++)
         {
             // generate signal in even-indexed slots of the buffer
             buffer[i * MY_CHANNELS] = sin(2 * MY_PIE * g_freq * g_t / MY_SRATE);

             // copy signal into odd-indexed slots of the buffer
             for(int j = 1; j < MY_CHANNELS; j++)
                 buffer[i * MY_CHANNELS + j] = buffer[i * MY_CHANNELS];

             // increment sample number
             g_t += 1.0;
         }
         return 0;
}

int main(int argc, char const *argv[]) {

    std::cout << "hello" << std::endl;

    // instantiate RtAudio object
    RtAudio audio;

    unsigned int bufferBytes = 0;

    // frame size
    unsigned int bufferFrames = 512;

    // check for audio devices
    if(audio.getDeviceCount() < 1)
    {
        cout << "no audio devices found!" << endl;
        exit(1);
    }

    // let RtAudio print messages to stderr.
    audio.showWarnings(true);

    // set input and output parameters
    RtAudio::StreamParameters iParams, oParams;
    iParams.deviceId = audio.getDefaultInputDevice();
    iParams.nChannels = MY_CHANNELS;
    iParams.firstChannel = 0;
    oParams.deviceId = audio.getDefaultOutputDevice();
    oParams.nChannels = MY_CHANNELS;
    oParams.firstChannel = 0;

    // create stream options
    RtAudio::StreamOptions options;

    try {
        // open a stream
        audio.openStream(&oParams, &iParams, MY_FORMAT, MY_SRATE, &bufferFrames,
            &audio_callback, (void *) &bufferBytes, &options);
    }
    catch(RtError& e)
    {
        cout << e.getMessage() << endl;
        exit(1);
    }

    // compute
    bufferBytes = bufferFrames * MY_CHANNELS * sizeof(SAMPLE);

    // test RtAudio functionality for reporting latency.
    cout << "stream latency: " << audio.getStreamLatency() << " frames" << endl;

    // go for it
    try {
        // start stream
        audio.startStream();

        // get input
        char input;
        std::cout << "running... press <enter> to quit (buffer frames: " << bufferFrames << ")" << endl;
        std::cin.get(input);

        // stop the stream.
        audio.stopStream();
    }
    catch(RtError& e)
    {
        // print error message
        cout << e.getMessage() << endl;
        goto cleanup;
    }

    cleanup:
        // close if open
        if(audio.isStreamOpen())
            audio.closeStream();

    return 0;
}
