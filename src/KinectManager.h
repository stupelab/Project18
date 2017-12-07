#pragma once
#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"

class KinectManager: public ofBaseApp{

public:
    KinectManager();
    void setup( ofxPanel* panel );
    void update( ofxPanel* panel  );
    
    
    ofxKinect kinect;
    
#ifdef USE_TWO_KINECTS
    ofxKinect kinect2;
#endif
    bool bThreshWithOpenCV;
    
    ofxCvColorImage colorImg;
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofxCvGrayscaleImage 	grayBg, grayDiff;
    
    ofxCvContourFinder contourFinder;

    int nearThreshold;
    int farThreshold;
    
    int angle; //angle of kinect device
    ofxPanel* mPanel;

};
