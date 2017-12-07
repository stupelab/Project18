#include "ofApp.h"

using namespace cv;
using namespace ofxCv;
//-------------------------------------------------------------/

void ofApp::setup() {
    
     ofBackground(0,0,0);
    //GUI parameters

    gui.setup("Project");
    gui.loadFromFile("settings.xml");
    gui.add( threshold.setup("nearThreshold", 150, 10, 300));
    gui.add( farThresholdSlider.setup("farThreshold", 60, 0, 10));
    gui.add( historySlider.setup("history", 0.5, 0, 1));
    gui.add( lifeTime.setup( "lifeTime",2, 0, 5 ));
    gui.add( friction.setup("friction" ,0.05 , 0, 0.1 ));
    gui.add( bornRate.setup("BornRate" ,1000 , 20 , 5000 ));
    gui.add( distortAmount.setup("distortAmount" ,30 , 2,  300   ));
    gui.add( distortRate.setup("rate" ,0.5 , 0.05,  20   ));
    gui.add( num_lines.setup("num_lines" ,5 , 1,  20   ));
    gui.add( smooth.setup("smooth" , 0.7 , 0.05, 1.0   ));
    
    
    gui.add(bThreshWithOpenCV.setup("Kinect/OpenCV", true));
  
    showGui = true;
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    mAudio.setup();
    //Midi Routing
    mMidiManager.setup(&gui);

	ofSetLogLevel(OF_LOG_VERBOSE);
	
    mKinectManager.setup(&gui );
	
    
    
 //   FBimage.allocate(kinect.width, kinect.height,OF_IMAGE_COLOR );
    
   
    //Loading shaders *********************************************
   // shader.load( "shaderVert.c", "shaderFrag.c" );
    shader.load( "wave/shaderVert.c", "wave/shaderFrag.c" );
    linesShader.load( "lines/shaderVert.c", "lines/shaderFrag.c" );
  //  shader.load("geom/shaderVert.c", "geom/shaderFrag.c", "geom/shaderGeom.c" );
    
    fbo.allocate( ofGetWidth(), ofGetHeight() );
    fbo2.allocate( ofGetWidth(), ofGetHeight() );
    
    FBimage.allocate(mKinectManager.kinect.getWidth(), mKinectManager.kinect.getHeight(), ofImageType::OF_IMAGE_GRAYSCALE);
    postImage.allocate(mKinectManager.kinect.getWidth(), mKinectManager.kinect.getHeight());
    vels.allocate(mKinectManager.kinect.getWidth(), mKinectManager.kinect.getHeight(), ofImageType::OF_IMAGE_GRAYSCALE);
    lines.load( "lines.jpg" );
    lines.resize( ofGetWidth(), ofGetHeight());
    

    
    //ParticleSystem parameters *********************************************
    
    //Set up parameters
    param.setup();
    history = 0.9;
    bornRate = 1500;
    
    bornCount = 0;
    time0 = ofGetElapsedTimef();
    
    //OPtical Flow ********************
    ofSetFrameRate(120);
    stepSize = 8;
    xSteps = mKinectManager.kinect.width / stepSize;
    ySteps = mKinectManager.kinect.height/ stepSize;
   

}

//--------------------------------------------------------------
void ofApp::update() {
    
    
    //Compute dt
    const float time = ofGetElapsedTimef();
    const float dt = ofClamp( time - time0, 0, 0.1 );
    time0 = time;
    
	//updating values from gui interface

    history = historySlider;
    param.lifeTime = lifeTime;
    param.friction = friction;
	
    mKinectManager.bThreshWithOpenCV = bThreshWithOpenCV;
    mKinectManager.nearThreshold = threshold;
    mKinectManager.farThreshold = farThresholdSlider;
   // mKinectManager.update();
    
    //Optical Flow **********************
    mKinectManager.update( &gui );

    if(mKinectManager.kinect.isFrameNew()) {
        flow.setWindowSize(stepSize);
        
        ofImage img;
        img = mKinectManager.kinect.getPixels();
        img.mirror(false, true);
        cv::Mat imgMat ;
        imgMat = toCv(img);
        flow.calcOpticalFlow(imgMat);
        
        FBimage = mKinectManager.grayImage.getPixels();
        postImage = FBimage.getPixels();
        postImage.erode();
        postImage.dilate();
        
        postImage.mirror(false, true);
        ofPixels mask;
        mask = postImage.getPixels();
        
        bornCount += dt * bornRate;      //Update bornCount value
        
        particle_pos.resize(bornCount);
        particle_vel.resize(bornCount);

        int i = 0;
        float distortionStrength = 4;
        for(int y = 1; y + 1 < ySteps; y++) {
            for(int x = 1; x + 1 < xSteps; x++) {
                if(  mask.getColor( x*stepSize, y*stepSize ).r < 0.5  ){
                    int i = y * xSteps + x;
                    ofVec2f position(x * stepSize, y * stepSize);
                  //  ofRectangle area(position - ofVec2f(stepSize, stepSize) / 2, stepSize, stepSize);
                  //  ofVec2f offset = flow.getAverageFlowInRegion(area);
                    ofVec2f offset = flow.getFlowOffset(position.x, position.y);
                    
                    position.x = ofMap( position.x, 0, mKinectManager.kinect.width, 0, ofGetWidth() );
                    position.y = ofMap( position.y, 0, mKinectManager.kinect.height, 0, ofGetHeight() );
                    
                    
                    float red = ofMap( offset.length(), 0 , 2, 0 ,255 );
                    ofColor col = ofColor(red,0,0);
                    vels.setColor(x * stepSize, y * stepSize, col );
                    if( offset.length() > 1 ){
                        particle_vel.push_back( offset );
                        particle_pos.push_back( position );
                    }

                }
                i++;
            }
        }
        vels.update();
        
        
        if ( bornCount >= 1 ) {          //It's time to born particle(s)
            int bornN = int( bornCount );//How many born
            bornCount -= bornN;          //Correct bornCount value
            for (int i=0; i<bornN; i++) {
                int random_index =  int(ofRandom(particle_vel.size()));
                Particle newP;
                if( particle_vel.size() > i ){
                    newP.setup(  ofVec2f(  particle_pos[ random_index ].x
                                         , particle_pos[ random_index ].y )
                               , ofVec2f( particle_vel[ random_index ].x,
                                         particle_vel[ random_index ].y )
                               );            //Start a new particle}
                }
                p.push_back( newP );     //Add this particle to array
            }
        }
        
    }//end Kinect New Frame Analysis
    
    particle_pos.clear();
    particle_vel.clear();
    //Particles
    
    
    //Born new particles

    
    //Delete inactive particles
    int i=0;
    while (i < p.size()) {
        if ( !p[i].live ) {
            p.erase( p.begin() + i );
        }
        else {
            i++;
        }
    }
    
  /*  FBimage = mKinectManager.grayImage.getPixels();
    postImage = FBimage.getPixels();
    postImage.erode();
    postImage.dilate();
    postImage.resize( ofGetWidth(), ofGetHeight() );
    postImage.mirror(false, true);
    //Getting new partiles inside mask indexes

   // silohuettePoints(postImage, bornCount);

   */
    

  //  param.mask = postImage.getPixels();

    //Update the particles
    for (int i=0; i<p.size(); i++) {
        p[i].update( dt );
    }
  
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
}

//--------------------------------------------------------------
void ofApp::draw() {
	
    ofBackground(0);
    float time = ofGetElapsedTimef();

   // postImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    for (int i=0; i<p.size(); i++) {
        p[i].draw();

    }
    
    /*
   fbo2.begin();
   //     postImage.invert();
    postImage.dilate();
    postImage.erode();
    postImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    fbo2.end();
    
    fbo.begin();
    
    ofBackground(0, 0, 0);
        ofEnableAlphaBlending();         //Enable transparency
    
        float alpha = (1-historySlider) * 255;
        ofFill();
        ofSetColor( 0, 0, 0, alpha );
        ofRectangle( 0, 0, ofGetWidth(), ofGetHeight() );

    
    ofSetColor(255,255,255,220);
    
    linesShader.begin();		//Enable the shader
    linesShader.setUniform1f( "time", time );	//Passing float parameter "time" to shader
    linesShader.setUniform1f( "num_lines", num_lines );	//Passing float parameter "num_li" to shader
    linesShader.setUniform1f( "rate", distortRate );	//Passing float parameter "rate" to shader

    linesShader.setUniform1f( "distorsion", distortAmount );    //Passing float parameter "distortAmount" to shader
    linesShader.setUniformTexture( "mask", fbo2.getTexture(), 2 );
    linesShader.setUniform2f("resolution", ofVec2f(ofGetWidth(),ofGetHeight()) );
    // fbo.getTexture().draw( 0, 0 );
    fbo.getTexture().draw(0,0);
    
    linesShader.end();
    
    fbo.end();


    shader.begin();		//Enable the shader
        shader.setUniform1f( "time", time );	//Passing float parameter "time" to shader
        shader.setUniform1f( "distorsion", distortAmount );    //Passing float parameter "distortAmount" to shader
        shader.setUniform1f( "smooth", smooth );    //Passing float parameter "distortAmount" to shader

        shader.setUniformTexture( "mask", fbo2.getTexture(), 2 );
        shader.setUniform2f("resolution", ofVec2f(ofGetWidth(),ofGetHeight()) );
    
        fbo.getTexture().draw(0,0);
    
    shader.end();
    
    */

    /*
    
			
#ifdef USE_TWO_KINECTS
		kinect2.draw(420, 320, 400, 300);
#endif
	}
	
	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
        
    if(kinect.hasAccelControl()) {
        reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
        << ofToString(kinect.getMksAccel().y, 2) << " / "
        << ofToString(kinect.getMksAccel().z, 2) << endl;
    } else {
        reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
		<< "motor / led / accel controls are not currently supported" << endl << endl;
    }
    
	reportStream << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
	<< "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
	<< "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << f.nBlobs
	<< ", fps: " << ofGetFrameRate() << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl;

    if(kinect.hasCamTiltControl()) {
    	reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }
    
	ofDrawBitmapString(reportStream.str(), 20, 652);
    */

        
    if(!showGui){
        gui.draw();
    }
    
}



void ofApp::silohuettePoints( ofxCvGrayscaleImage img , int num_particles ){
    
    particle_pos.resize(bornCount);
    target_i.resize(ofGetWidth());
    target_j.resize(ofGetHeight());
    vector <ofVec2f> target_particle;
    target_particle.resize(bornCount);
    ofImage pixels;
    pixels = img.getPixels();
    //Primero cojo los puntos blancos de la silueta
    for( int i=0; i< img.getWidth(); i++ ){
        for( int j=0; j< img.getHeight(); j++ ){
            if(  pixels.getColor( i, j ).r < 0.5    ){
                target_i.push_back(i);
                target_j.push_back(j);
                
            }
        }
    }
}


//--------------------------------------------------------------
void ofApp::exit() {
    
	mKinectManager.kinect.setCameraTiltAngle(0); // zero the tilt on exit
	mKinectManager.kinect.close();
	
    gui.saveToFile("settings.xml");

    
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {

		case 'w':
			mKinectManager.kinect.enableDepthNearValueWhite(!mKinectManager.kinect.isDepthNearValueWhite());
			break;
		case 'o':
			mKinectManager.kinect.setCameraTiltAngle(mKinectManager.angle); // go back to prev tilt
			mKinectManager.kinect.open();
			break;
			
		case 'c':
			mKinectManager.kinect.setCameraTiltAngle(0); // zero the tilt
			mKinectManager.kinect.close();
			break;
		case OF_KEY_UP:
			mKinectManager.angle++;
			if(mKinectManager.angle>30) mKinectManager.angle=30;
			mKinectManager.kinect.setCameraTiltAngle(mKinectManager.angle);
			break;
			
		case OF_KEY_DOWN:
			mKinectManager.angle--;
			if(mKinectManager.angle<-30) mKinectManager.angle=-30;
			mKinectManager.kinect.setCameraTiltAngle(mKinectManager.angle);
			break;
        case 'S' : {
            ofFileDialogResult res;
            res = ofSystemSaveDialog("preset.xml", "Saving Preset");
            if(res.bSuccess) gui.saveToFile(res.filePath);
            break;}
        case 'h':
            showGui = !showGui;
            break;
        case 'x':
            bLearnBakground = true;
        case 'F':
            ofToggleFullscreen();
        case 's':
            ofSaveScreen("screenshot"+ ofToString(ofRandom(0,1000),0)+".png");
            break;
            //Save Preset
        case 'L':{
            ofFileDialogResult resload;
            resload = ofSystemLoadDialog("preset.xml", "Loading Preset");
            resload.filePath ="/of_v0.9.8_osx_release/apps/myApps/Project18/bin/data/";
            if(resload.bSuccess) gui.loadFromFile(resload.filePath);
            break;}

	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}
