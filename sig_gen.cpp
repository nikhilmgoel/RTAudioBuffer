//-----------------------------------------------------------------------------
// name: sig_gen.cpp
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

/* ----------------------#defines-------------------- */

// A sample is a discrete time signal derived from a continuous signal
#define SAMPLE double

// RtAudio's data format type. Normalized between +- 1
#define MY_FORMAT RTAUDIO_FLOAT64

// Sample Rate = (avg. # of samples)/second = 1/T, where T is sampling interval
#define MY_SRATE 44100

// number of channels
#define MY_CHANNELS 2

#define MY_PIE 3.14159265358979

/* ----------------------globals--------------------- */

// frequency
SAMPLE g_freq = 440;

// sample number 
SAMPLE g_t = 0;

// wave width
SAMPLE g_width = 0;

// type of signal requested by user, enumerated in function determine_signal
int g_sig = 0;

/*---------------------------------------------------- */

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
            switch(g_sig) {
                case 1: // sine
                    buffer[i * MY_CHANNELS] = sin(2 * MY_PIE * g_freq * g_t / MY_SRATE);
                    break;
                case 2: // saw
                    break;
                case 3: // pulse
                    break;
                case 4: // noise (white noise)
                    buffer[i * MY_CHANNELS] = (rand() % 100) / 10;
                    break;
                case 5: // impulse

                    // signal sample is at the fundamental period of the given frequency
                    if (sin(2 * MY_PIE * g_freq * g_t / MY_SRATE) > 0) { 
                        buffer[i * MY_CHANNELS] = 1;
                    } else {
                        buffer[i * MY_CHANNELS] = 0;
                    }
                    break;
                default: return 2;
            }

             // copy signal into odd-indexed slots of the buffer
             for(int j = 1; j < MY_CHANNELS; j++)
                 buffer[i * MY_CHANNELS + j] = buffer[i * MY_CHANNELS];

             // increment sample number
             g_t += 1.0;
         }
         return 0;
}

/*
 * @funtion determine_signal Returns an integer based on the type of command given.
 * @param arg command given by user input
 */
int determine_signal(int argc, const char *argv[]) {
    string arg = string(argv[1]);
    char *endptr = 0;

    // checks third argument: frequency
    if (argc > 2) {
        g_freq = strtod(argv[2], &endptr);
        if (*endptr != '\0' || endptr == argv[2]) {
            cout << "The frequency you entered is not a double. Using default frequency 440 Hz." << endl;
            g_freq = 440;
        } 
    }

    // checks fourth argument: width
    if (argc > 3) {
        g_width = strtod(argv[3], &endptr);
        if (*endptr != '\0' || endptr == argv[3]) {
            cout << "The width you entered is not a double. Using default width 1." << endl;
            g_width = 1;
        } 
    }

    // sine
    if (arg == "--sine") {
        return 1;
    }

    // saw
    else if (arg == "--saw"){
        return 2;
    }

    // pulse
    else if (arg == "--pulse"){
        return 3;
    }

    // noise
    else if (arg == "--noise"){
        return 4;
    }

    //impulse
    else if (arg == "--impulse") {
        return 5;
    } 

    // arg is some other input not defined by this program
    else {
        return -1;
    }
}

int main(int argc, char const *argv[]) {

    // error: ./sig_gen with no additional arguments
    if (argc <= 1) {
        cout << "Not enough arguments. Must give type of signal generation." << endl;
        exit(1);
    }

    // error: more than four arguments
    if (argc > 4) {
        cout << "Ignoring extraneous arguments..." << endl;
    }

    // determines which type of signal user wants to generate
    g_sig = determine_signal(argc, argv);

    // error: invalid waveform type 
    if (g_sig < 0) {
        cout << "Must provide a valid type of waveform. --sine, --saw, --noise, --pulse, --impulse." << endl;
        exit(1);
    }

    // instantiate RtAudio object
    RtAudio audio;

    // frame size
    unsigned int bufferFrames = 512;
    unsigned int bufferBytes = 0;

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
