#include "AudioManager.h"


AudioManager::AudioManager(){}
AudioManager::~AudioManager(){}

void AudioManager::setup(){
    
    //Set buffer size and fill it by zeros
    buffer.resize( N, 0.0 );
    
    //Start the sound output in stereo (2 channels)
    //and sound input in mono (1 channel)
    
    left = new float[256];
    right = new float[256];
    
    left2 = new float[256];
    right2 = new float[256];
    
    
    c1.printDeviceList();
    c1.setDeviceID(2); // in
    c1.setup(0, 2, 44100, 256, 4);
    c1.setInput(this);
    
    c2.printDeviceList();
    c2.setDeviceID(1); // out
    c2.setup(2, 0, 44100, 256, 4);
    c2.setOutput(this);
}

//--------------------------------------------------------------
//Audio input
void AudioManager::audioReceived( float *input, int bufferSize, int nChannels )
{
    // samples are "interleaved"
    for (int i = 0; i < bufferSize; i++){
        left[i] = input[i*2];
        right[i] = input[i*2+1];
    }
    //bufferCounter++;
}


//--------------------------------------------------------------
//Audio output
void AudioManager::audioOut(float *output, int bufferSize, int nChannels)
{
    
    for (int i=0; i<bufferSize; i++) {
        //Push current audio sample value from buffer
        //into both channels of output.
        //Also global volume value is used
        output[ 2*i ] = left[i] * volume;
        output[ 2*i + 1 ] = right[i] * volume;
        //Shift to the next audio sample
        //playPos++;
        //When the end of buffer is reached, playPos sets to 0
        //So we hear looped sound
        //playPos %= N;
    }
    
}
