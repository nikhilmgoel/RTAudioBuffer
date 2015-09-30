//-----------------------------------------------------------------------------
// name: sig_gen.cpp
// desc: real-time waveforms!
//
// author: Nikhil Goel (nmgoel@stanford.edu)
//   date: fall 2015, Music256a taught by Ge Wang (righteous!)
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

// amplitude definitions
#define MAX_AMP 1.0
#define MIN_AMP -1.0
#define BASELINE 0

/* ----------------------globals--------------------- */

// frequency
SAMPLE g_freq = 440;

// sample number 
SAMPLE g_t = 0;

// wave width (default square wave)
SAMPLE g_width = 0.5;

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
 * @return Zero to maintain normal stream. One to stop the stream and drain the
           output buffer. Two to abort the stream immediately.
 */
int audio_callback(void *outputBuffer, void *inputBuffer, unsigned int numFrames,
     double streamTime, RtAudioStreamStatus status, void *data) {

         // Fundamental period of the wave.
         double period = MY_SRATE / g_freq;

         // Position of the current sample relative to the current wave period.
         int sample_pos = (int) g_t % (int) period;

         // Difference between period and the width. Used by the right part of a saw tooth.
         double rmdr = period - g_width;

         // Difference in value between consecutive samples for the left portion of the saw.
         double left_sample_step = (MAX_AMP * 2) / g_width;

         // Difference in value between consecutive samples for the right portion of the saw.
         double right_sample_step = (MAX_AMP * 2) / rmdr;

         // stderr prints info and err messages (info about callback here)
         cerr << ".";

         // outputBuffer points to array of SAMPLEs
         SAMPLE *buffer = (SAMPLE *) outputBuffer;
         for(int i = 0; i < numFrames; i++)
         {
            /* Creates different waveforms based on user input by first generating a signal in the 
             * even-indexed slots of the buffer. */
            switch(g_sig) {

                // sine
                case 1: 
                    buffer[i * MY_CHANNELS] = sin(2 * MY_PIE * g_freq * g_t / MY_SRATE);
                    break;

                // saw
                case 2: 

                    /* Produces left portion of saw wave (pos. slope). Steps up the left portion's 
                       hypotenuse with values based on the current sample position. */
                    if (sample_pos <= g_width) {
                        buffer[i * MY_CHANNELS] = left_sample_step * sample_pos;
                    } 

                    /* Produces right portion of saw wave (neg. slope). Steps down the right portion's
                       hypotenuse. Uses the difference of the period and the current position. */                    
                    else {
                        buffer[i * MY_CHANNELS] = right_sample_step * (period - sample_pos);
                    }
                    break;

                // pulse    
                case 3: 

                    /* (Sample % period) <= the width, or delay time, of the wave.
                     * Produces rectangular wave above the baseline. */
                    if (sample_pos <= g_width) {
                        buffer[i * MY_CHANNELS] = MAX_AMP;
                    } 

                    /* (Sample % period) >= the width, or delay time, of the wave.
                     * Produces rectangular wave below the baseline. */                    
                    else if (sample_pos >= g_width) {
                        buffer[i * MY_CHANNELS] = MIN_AMP;
                    }
                    break;

                // noise (white noise to be more specific)    
                case 4: 
                    buffer[i * MY_CHANNELS] = (rand() % 100) / 100;
                    break;

                // impulse train    
                case 5: 

                    // signal sample shoots an impulse at the given frequency's fundamental period
                    if (sample_pos == 0) { 
                        buffer[i * MY_CHANNELS] = MAX_AMP;
                    } else {
                        buffer[i * MY_CHANNELS] = BASELINE;
                    }
                    break;

                // if none of the above, stop the stream immediately.   
                default: 
                    return 2;
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
            Allows me to easily use a switch statement in the audio callback function
            for the different waveforms.
 * @param argc Number of command line arguments.
 * @param argv Array of strings containing command line arguments.
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

        // if no width given, use default width
        if (argc == 3) cout << "No width given. Using 0.5 as default width." << endl;
    }

    // checks fourth argument: width
    if (argc > 3) {
        g_width = strtod(argv[3], &endptr);
        if (*endptr != '\0' || endptr == argv[3]) {
            cout << "The width you entered is not a double. Using default width 1." << endl;
            g_width = 1;
        } else if (g_width >= 1.0 || g_width <= 0) {
            cout << "Must enter a width in the range (0, 1)." << endl;
            exit(1);
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
        cout << "Input should be of form ./sig_gen [type] [frequency] [width]...";
        cout << "frequency and width are optional." << endl;
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
