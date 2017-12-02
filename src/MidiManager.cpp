#include "MidiManager.h"


MidiManager::MidiManager(){}

void MidiManager::setup(ofxPanel* panel){
    
    mPanel = panel;
    // print input ports to console
    midiIn.listPorts(); // via instance
    //ofxMidiIn::listPorts(); // via static as well
    
    // open port by number (you may need to change this)
    //midiIn.openPort(2);
    midiIn.openPort("iPhone Bluetooth");	// by name
    //midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port
    
    // don't ignore sysex, timing, & active sense messages,
    // these are ignored by default
    midiIn.ignoreTypes(false, false, false);
    
    // add ofApp as a listener
    midiIn.addListener(this);
    
    // print received messages to the console
    midiIn.setVerbose(true);
}

//--------------------------------------------------------------
void MidiManager::newMidiMessage(ofxMidiMessage& msg) {
    
    // make a copy of the latest message
    midiMessage = msg;
    if( midiMessage.control == 8){
        mPanel->getFloatSlider("distortAmount")= ofMap(midiMessage.value, 0.0, 127.0, 0.0, 500.0);
    }else if( midiMessage.control == 11){
        mPanel->getFloatSlider("num_lines") = ofMap(midiMessage.value, 0.0, 127.0, 0.0, 20.0);
    }    
    
}

void MidiManager::draw(){
 
    text << "control: " << midiMessage.control;
    ofDrawBitmapString(text.str(), 20, 144);
    text.str(""); // clear
    ofDrawRectangle(20, 154, ofMap(midiMessage.control, 0, 127, 0, ofGetWidth()-40), 20);
}
