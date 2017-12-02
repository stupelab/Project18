
#pragma once

#include "ofxGui.h"
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "particleManager.h"
#include "ofxMidi.h"


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




class ofApp : public ofBaseApp , public ofxMidiListener {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void drawPointCloud();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
    void updateKinectData(  );
    void silohuettePoints( ofxCvGrayscaleImage grayImage, int num_particles);
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    
	
	ofxKinect kinect;
	
#ifdef USE_TWO_KINECTS
	ofxKinect kinect2;
#endif
	
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvGrayscaleImage 	grayBg, grayDiff;
    ofxCvGrayscaleImage postImage; // grayscale depth image
	
	ofxCvContourFinder contourFinder;
	
//	bool bThreshWithOpenCV;
	bool bDrawPointCloud;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
    
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
    const int N = 256;		//Number of bands in spectrum
   // float spectrum[];	//Smoothed spectrum value
   // ofSound sound1;
    
    bool showGui, bLearnBakground;

    
    //Gui Parameters
    ofxPanel gui;
    ofxFloatSlider threshold , farThresholdSlider , historySlider, lifeTime, friction, linesRate, distortAmount , distortRate, smooth , num_lines;
    ofXml settings;
    ofxToggle bThreshWithOpenCV;
    
    //Particle System
  
    vector<Particle> p;	  //Particles
    vector <ofVec2f> particle_points;
 
    float bornRate;       //Particles born rate per second
    float bornCount;      //Integrated number of particles to born
    
    float history;        //Control parameter for trails
    float time0;          //Time value for computing dt
    vector <float> target_i;
    vector <float> target_j;
    
    
    //Midi I/O
    stringstream text;
    
    ofxMidiIn midiIn;
    ofxMidiMessage midiMessage;
    
    //Audio I/O
    
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
    

    
};
