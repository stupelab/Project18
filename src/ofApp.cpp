#include "ofApp.h"

using namespace cv;
using namespace ofxCv;
//-------------------------------------------------------------/

void ofApp::setup() {
    
     ofBackground(0,0,0);
    //GUI parameters
    

    gui.setup("Project");
    gui.loadFromFile("settings.xml");
    gui.add(threshold.setup("nearThreshold", 150, 10, 300));
    gui.add(farThresholdSlider.setup("farThreshold", 60, 0, 10));
    gui.add(historySlider.setup("history", 0.5, 0, 1));
    gui.add(lifeTime.setup( "lifeTime",2, 0, 5 ));
    gui.add( friction.setup("friction" ,0.05 , 0, 0.1 ));
    gui.add( linesRate.setup("linesRate" ,10 , 5, 30 ));
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
	
	ofSetFrameRate(60);
	
    
    
 //   FBimage.allocate(kinect.width, kinect.height,OF_IMAGE_COLOR );
    
   
    //Loading shaders *********************************************
   // shader.load( "shaderVert.c", "shaderFrag.c" );
    shader.load( "wave/shaderVert.c", "wave/shaderFrag.c" );
    linesShader.load( "lines/shaderVert.c", "lines/shaderFrag.c" );
  //  shader.load("geom/shaderVert.c", "geom/shaderFrag.c", "geom/shaderGeom.c" );
    
    fbo.allocate( ofGetWidth(), ofGetHeight() );
    fbo2.allocate( ofGetWidth(), ofGetHeight() );
    
    lines.load( "lines.jpg" );
    lines.resize( ofGetWidth(), ofGetHeight());
    

    
    //ParticleSystem parameters *********************************************
    
    //Set up parameters
    param.setup();
    history = 0.9;
    bornRate = 1500;
    
    bornCount = 0;
    time0 = ofGetElapsedTimef();
    
    
    
}

//--------------------------------------------------------------
void ofApp::update() {
    
	//updating values from gui interface

    history = historySlider;
    param.lifeTime = lifeTime;
    param.friction = friction;
	
    mKinectManager.bThreshWithOpenCV = bThreshWithOpenCV;
    mKinectManager.update();
  
    //Particles
    
    //Compute dt
    float time = ofGetElapsedTimef();
    float dt = ofClamp( time - time0, 0, 0.1 );
    time0 = time;
    

    
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
    
    
    //Born new particles
    bornCount += dt * bornRate;      //Update bornCount value
    
    FBimage = mKinectManager.grayImage.getPixels();
    postImage = FBimage.getPixels();
    postImage.erode();
    postImage.dilate();
    postImage.resize( ofGetWidth(), ofGetHeight() );
    postImage.mirror(false, true);
    //Getting new partiles inside mask indexes

   // silohuettePoints(postImage, bornCount);

    vector <float> target_i;
    vector <float> target_j;
    target_i.resize(ofGetWidth());
    target_j.resize(ofGetHeight());
    ofImage pixels;
    pixels = postImage.getPixels();
    //Cojo los puntos blancos de la silueta
    for( int i=0; i< postImage.getWidth(); i++ ){
        for( int j=0; j< postImage.getHeight(); j++ ){
            if(  pixels.getColor( i, j ).r < 0.5    ){
                target_i.push_back(i);
                target_j.push_back(j);
            }
        }
    }
    
    if ( bornCount >= 1 ) {          //It's time to born particle(s)
        int bornN = int( bornCount );//How many born
        bornCount -= bornN;          //Correct bornCount value
        for (int i=0; i<bornN; i++) {
             int random_index =  int(ofRandom(target_j.size()));
            Particle newP;
            newP.setup(  ofVec2f(target_i[ random_index  ], target_j[ random_index  ] )  );            //Start a new particle
            p.push_back( newP );     //Add this particle to array
        }
    }
    particle_points.clear();
    
    param.mask = postImage.getPixels();
    //Update the particles
    for (int i=0; i<p.size(); i++) {
        p[i].update( dt );
    }
    

    //Shaders configuration
    
  
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
}

//--------------------------------------------------------------
void ofApp::draw() {
	
   // ofBackground(0,0,0);
    
    float time = ofGetElapsedTimef();
    
    //contourFinder.findContours(postImage, 30, (postImage.width*postImage.height)/2, 60, true);
    
    
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
    
    
    
    //Disable transparency

  //  contourFinder.draw(0, 0, ofGetWidth(), ofGetHeight());
  

    /*
    
	if(bDrawPointCloud) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
	} else {
        
	// draw from the live kinect
	//	kinect.drawDepth(10, 10, 400, 300);
	//	kinect.draw(420, 10, 400, 300);
	//kinect.drawDepth(0,0, ofGetWidth(), ofGetHeight());
       
    //fbo mask
    
        fbo.begin();
            postImage.draw(0, 0, ofGetWidth(), ofGetHeight());
        fbo.end();
        
       //color img
        fbo2.begin();
        kinect.draw(0, 0, ofGetWidth(), ofGetHeight());
        fbo2.end();
        
        //Reading contents of rendered fbo.
        //Below we will use it for line drawing
        ofPixels pixels;
        fbo2.readToPixels(pixels);

        
        shader.begin();
        
        shader.setUniform1f( "time", time );
        shader.setUniform1f( "distortAmount", distortAmount );
        shader.setUniformTexture( "mask", fbo.getTexture(), 2 );
        shader.setUniformTexture( "texture1", fbo2.getTexture(), 1 );
        shader.setUniformTexture( "lines", lines, 3 );
        
        
        //Draw lines (through vertex->geometry->fragment shaders)
        ofSetColor( 255, 255, 255 );
        int stepx = 10;
        int stepy = 10;
        float len = 20;
        //Scan fbo pixels colors and search for not-transparent pixels
        for (int y=0; y< ofGetHeight(); y+=stepy) {
            for (int x=0; x< ofGetWidth(); x+=stepx) {
                if ( pixels.getColor( x, y ).a > 0 ) {
                    ofDrawLine( x, y, x, y - len );
                    
                }
            }
        }
        
        fbo2.draw(0,0);
        
        
        shader.end();

   

		
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
    
    particle_points.resize(bornCount);
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
			
		case '1':
			mKinectManager.kinect.setLed(ofxKinect::LED_GREEN);
			break;
			
		case '2':
			mKinectManager.kinect.setLed(ofxKinect::LED_YELLOW);
			break;
			
		case '3':
			mKinectManager.kinect.setLed(ofxKinect::LED_RED);
			break;
			
		case '4':
			mKinectManager.kinect.setLed(ofxKinect::LED_BLINK_GREEN);
			break;
			
		case '5':
			mKinectManager.kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
			break;
			
		case '0':
			mKinectManager.kinect.setLed(ofxKinect::LED_OFF);
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
        case 'h':
            showGui = !showGui;
            break;
        case 'x':
            bLearnBakground = true;
        case 'f':
            ofToggleFullscreen();
        case 's':
            ofSaveScreen("screenshot"+ ofToString(ofRandom(0,1000),0)+".png");
            //Save Preset
        case'S' :
                ofFileDialogResult res;
                res = ofSystemSaveDialog("preset.xml", "Saving Preset");
                if(res.bSuccess) gui.saveToFile(res.filePath);
                break;
        
            

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
