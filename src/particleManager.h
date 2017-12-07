#pragma once


#include "ofMain.h"


#define WORK_GROUP_SIZE 256

//Control parameters class
class Params {
public:
    void setup();
    ofPoint eCenter;    //Emitter center
    float eRad;         //Emitter radius
    float velRad;       //Initial velocity limit
    float lifeTime;     //Lifetime in seconds
    
    float rotate;   //Direction rotation speed in angles per second
    
    float force;       //Attraction/repulsion force inside emitter
    float spinning;    //Spinning force inside emitter
    float friction;    //Friction, in the range [0, 1]
   
    ofImage mask;
};



extern Params param; //Declaration of a global variable

//Particle class
class Particle {
public:
    Particle();                //Class constructor
    void setup( ofVec2f pos,  ofVec2f vel );              //Start particle
    void update( float dt);   //Recalculate physics
    void draw();               //Draw particle
    
    ofPoint pos;               //Position
    ofPoint vel;               //Velocity
    float time;                //Time of living
    float lifeTime;            //Allowed lifetime
    bool live;                 //Is particle live
    bool  isMask;              //Is particle inside mask
    ofColor hue;
    ofColor hue_die;
    float alpha;
};


