#pragma once
#include "ofxOsc.h"

#define NUM_MSG_STRINGS  20
class AVPOscManager: public ofBaseApp{
    
    public:
    AVPOscManager();
    void setup( int port );
    void update();
    void draw();
    
    ofTrueTypeFont font;
    ofxOscReceiver receiver;

    int current_msg_string;
    string msg_strings[NUM_MSG_STRINGS];
    float timers[NUM_MSG_STRINGS];
    
    //Particle Parameters
    float history, lifeTime , friction , bornRate;
    // FX Parameters
    float distortAmount;
};
