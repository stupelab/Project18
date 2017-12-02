#pragma once
#include "ofMain.h"


class AudioManager: public ofBaseApp{

public:
    
     AudioManager();
     ~AudioManager();

    void setup();
    //Function for receiving audio
    void audioReceived( float *input, int bufferSize, int nChannels );
    //Function for generating audio
    void audioOut( float *output, int bufferSize, int nChannels );
    
    
    
    float * left;
    float * right;
    
    float * left2;
    float * right2;
    
    //Object for sound output and input setup
    ofSoundStream c1;
    ofSoundStream c2;
    
    //Constants
    const int sampleRate = 44100;           //Sample rate of sound
    
    const float volume = 0.5;	//Output sound volume
    
    //Variables
    vector<float> buffer;		//PCM buffer of sound sample
    int recPos = 0;				//Current recording position in the buffer
    int playPos = 0;			//Current playing position in the buffer
    
    int recordingEnabled = 1;	//Is recording enabled
    int playingEnabled = 0;		//Is playing enabled
        const int N = 256;		//Number of bands in spectrum
    
};
