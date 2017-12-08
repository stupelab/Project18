#pragma once
#include "ofMain.h"
#include "ofxAudioAnalyzer.h"


struct DescriptorData{
    
    float rms;
    float power;
    float pitchFreq;
    float pitchFreqNorm;
    float pitchConf;
    float pitchSalience;
    float hfc;
    float hfcNorm;
    float specComp;
    float specCompNorm;
    float centroid;
    float centroidNorm;
    float inharmonicity;
    float dissonance;
    float rollOff;
    float rollOffNorm;
    float oddToEven;
    float oddToEvenNorm;
    float strongPeak;
    float strongPeakNorm;
    float strongDecay;
    float strongDecayNorm;
    
    vector<float> spectrum;
    vector<float> melBands;
    vector<float> mfcc;
    vector<float> hpcp;
    
    vector<float> tristimulus;
    
    bool isOnset;
    
};



class AudioManager: public ofBaseApp{

public:
    
     AudioManager();
     ~AudioManager();

    void setup();
    //Function for receiving audio
   // void audioReceived( float *input, int bufferSize, int nChannels );
    void audioIn(float *input, int bufferSize, int nChannels);
    void audioOut(float *output, int bufferSize, int nChannels);
    //Function for generating audio
   // void audioOut( float *output, int bufferSize, int nChannels );
    void updateDescriptors();
    
    
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
    vector<float> buffer;
    ofSoundBuffer soundBuffer;
    int recordingEnabled = 1;	//Is recording enabled
    int playingEnabled = 0;		//Is playing enabled
        const int N = 256;		//Number of bands in spectrum
    
    ofxAudioAnalyzer audioAnalyzer;
    
    mutex audioMutex;
    ofSoundBuffer lastBuffer;
    
    DescriptorData descriptors;
    bool isAnalyzing;
    float smoothing = 0.2;
    
};

