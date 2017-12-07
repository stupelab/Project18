
#pragma once

#include "ofxGui.h"
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "particleManager.h"
#include "AudioManager.h"
#include "MidiManager.h"
#include "KinectManager.h"
// Windows users:
// You MUST install the libfreenect kinect drivers in order to be able to use
// ofxKinect. Plug in the kinect and point your Windows Device Manager to the
// driver folder in:
//
//     ofxKinect/libs/libfreenect/platform/windows/inf
//
// This should install the Kinect camera, motor, & audio drivers.
//
// You CANNOT use this driver and the OpenNI driver with the same device. You
// will have to manually update the kinect device to use the libfreenect drivers
// and/or uninstall/reinstall it in Device Manager.
//
// No way around the Windows driver dance, sorry.

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS




class ofApp : public ofBaseApp{
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
    void silohuettePoints( ofxCvGrayscaleImage grayImage, int num_particles);
       
	
	//Kinect SEtup
    KinectManager mKinectManager;
    
    ofImage FBimage;
    ofImage mask;
    //Shader variables
    ofShader shader, linesShader;	//Shade
    ofFbo fbo , fbo2 , fbo3;			//Buffer for intermediate drawing
    ofImage image, background, lines;		//Testing Sunflower image
	
	// used for viewing the point cloud
	ofEasyCam easyCam;
    
    //interaction variables
   // float distortAmount;
    
    //Spectrum Analysis for shading
    ofFloatImage spectrumImage;
    
    //Gui Parameters
    ofxPanel gui;
    ofxFloatSlider threshold , farThresholdSlider , historySlider, lifeTime, friction, bornRate, distortAmount , distortRate, smooth , num_lines;
    ofXml settings;
    ofxToggle bThreshWithOpenCV;
    bool showGui, bLearnBakground;

    
    //Particle System
    ofxCvGrayscaleImage postImage; // grayscale depth image
  
    vector<Particle> p;	  //Particles
    vector <ofVec2f> particle_pos;
    vector <ofVec2f> particle_vel;
 
 //   float bornRate;       //Particles born rate per second
    float bornCount;      //Integrated number of particles to born
    
    float history;        //Control parameter for trails
    float time0;          //Time value for computing dt
    vector <float> target_i;
    vector <float> target_j;
    
    
    //Midi I/O
    MidiManager mMidiManager;
    
    //Audio I/O
    AudioManager mAudio;
    
    //FLow distort

    ofxCv::FlowFarneback flow;
    int stepSize, xSteps, ySteps;
    ofImage vels;

    
};
