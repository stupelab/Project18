#include "AVPOscManager.h"


AVPOscManager::AVPOscManager(){}

void AVPOscManager::setup( int port ){
    
    //OSC Receiver Conf
    cout << "listening for osc messages on port " << port << "\n";
    receiver.setup(port);
    
    
};
void AVPOscManager::update( ){
    
    newMsg = false;
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        
        newMsg = true;
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(&m);

        // check for mouse moved message
        if(m.getAddress() == "/test"){
            current_msg_string = receiver.getNextMessage(&m);
            ofLogNotice() << "test:  " << m.getArgAsInt32(0) ;
        }else if( m.getAddress() == "/particle/history" ){
            history = ofMap(m.getArgAsFloat(0), 0 ,1 ,0 ,1 );
        }else if( m.getAddress() == "/particle/lifetime" ){
            lifeTime = ofMap(m.getArgAsFloat(0), 0 ,1 ,0 ,5 );
        }else if( m.getAddress() == "/particle/friction" ){
            friction = ofMap(m.getArgAsFloat(0), 0 ,1 ,0 ,0.1);
        }else if( m.getAddress() == "/particle/bornRate" ){
            bornRate = ofMap(m.getArgAsFloat(0), 0 ,1 ,20 , 5000  );;
        }else if( m.getAddress() == "/fx/distortAmount" ){
            distortAmount = ofMap(m.getArgAsFloat(0), 0 ,1 ,2 , 300  );;
        }
        else{
            // unrecognized message: display on the bottom of the screen
            string msg_string;
            msg_string = m.getAddress();
            msg_string += ": ";
            for(int i = 0; i < m.getNumArgs(); i++){
                // get the argument type
                msg_string += m.getArgTypeName(i);
                msg_string += ":";
                // display the argument - make sure we get the right type
                if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                    msg_string += ofToString(m.getArgAsInt32(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                    msg_string += ofToString(m.getArgAsFloat(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                    msg_string += m.getArgAsString(i);
                }
                else{
                    msg_string += "unknown";
                }
            }
            // add to the list of strings to display
            msg_strings[current_msg_string] = msg_string;
            timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
            current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
            // clear the next line
            msg_strings[current_msg_string] = "";
        }
        
    }

    
};


void AVPOscManager::draw( ){

    for(int i = 0; i < NUM_MSG_STRINGS; i++){
        ofDrawBitmapString(msg_strings[i], 10, 40 + 15 * i);
    }
}
