#pragma once
#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxGui.h"

class MidiManager: public ofBaseApp, public ofxMidiListener{
    
public:
    MidiManager();
    void setup(ofxPanel* panel);
    void draw();
    void newMidiMessage(ofxMidiMessage& eventArgs);

    stringstream text;
    
    ofxMidiIn midiIn;
    ofxMidiMessage midiMessage;
    
    ofxPanel* mPanel;
};
