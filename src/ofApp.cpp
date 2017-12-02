#include "ofApp.h"

//-------------------------------------------------------------/

void ofApp::setup() {
    
     ofBackground(0,0,0);
    //GUI parameters
    

    gui.setup();
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
    gui.loadFromFile("settings.xml");
    showGui = true;
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    //Set buffer size and fill it by zeros
    buffer.resize( N, 0.0 );
    
    //Start the sound output in stereo (2 channels)
    //and sound input in mono (1 channel)
    
    left = new float[256];
    right = new float[256];
    
    left2 = new float[256];
    right2 = new float[256];
    
    
    c1.printDeviceList();
    c1.setDeviceID(2); // in
    c1.setup(0, 2, 44100, 256, 4);
    c1.setInput(this);
    
     c2.printDeviceList();
    c2.setDeviceID(1); // out
    c2.setup(2, 0, 44100, 256, 4);
    c2.setOutput(this);
    //Midi Routing
    
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
    
    
    
    //Set up sound sample
  //  sound.loadSound( "surface.wav" );
    
    //Set spectrum values to 0
    /*for (int i=0; i<N; i++) {
        spectrum[i] = 0.0f;
    }*/
    
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
	// print the intrinsic IR sensor values
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}
	
#ifdef USE_TWO_KINECTS
	kinect2.init();
	kinect2.open();
#endif
	
	colorImg.allocate(kinect.width, kinect.height);
   // mask.allocate(kinect.width, kinect.heigh);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);

	
	nearThreshold = 150; //initial value 230, now 150 (Kinect) (200 OpenCv)
	farThreshold = 60;
//	bThreshWithOpenCV = true;
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 10;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawPointCloud = false;
    
    
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
    nearThreshold = threshold;
    history = historySlider;
    param.lifeTime = lifeTime;
    param.friction = friction;
    farThreshold = farThresholdSlider;
		
	kinect.update();
    
    //Updating Kinect and OpenCVKinect Images
    
    updateKinectData(  );
    
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
    
    FBimage = grayImage.getPixels();
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
    
    
   /* for( int i=0; i< particle_points.size(); i++ ){
        
        ofLog(OF_LOG_NOTICE,"size " + ofToString(  particle_points.size() )) ;
        ofLog(OF_LOG_NOTICE,"x position:  " + ofToString(  particle_points[i].x )) ;
        
        ofSetColor(255,0,0,100);
        ofDrawCircle(particle_points[i].x, particle_points[i].y , 3);
    }
    */
    contourFinder.findContours(postImage, 30, (postImage.width*postImage.height)/2, 60, true);
    
    
    
    
    
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
    float  y = 3 * sin( time * linesRate);
    
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
       // fbo.getTexture().draw( 0, 0 );
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
    
    text << "control: " << midiMessage.control;
    ofDrawBitmapString(text.str(), 20, 144);
    text.str(""); // clear
    ofDrawRectangle(20, 154, ofMap(midiMessage.control, 0, 127, 0, ofGetWidth()-40), 20);
        
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
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    
    // make a copy of the latest message
    midiMessage = msg;
    if( midiMessage.control == 8){
     distortAmount = ofMap(midiMessage.value, 0.0, 127.0, 0.0, 500.0);
    }else if( midiMessage.control == 11){
        num_lines = ofMap(midiMessage.value, 0.0, 127.0, 0.0, 20.0);
    }
   
  
    
    
}


void ofApp::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
}

void ofApp::updateKinectData(   ){
    // there is a new frame and we are connected
    if(kinect.isFrameNew()) {
        
        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(kinect.getDepthPixels());
        
        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        if(bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
            
            
        } else {
            
            // or we do it ourselves - show people how they can work with the pixels
            ofPixels & pix = grayImage.getPixels();
            int numPixels = pix.size();
            for(int i = 0; i < numPixels; i++) {
                if(pix[i] > nearThreshold ) {
                    pix[i] = 0;
                } else {
                    pix[i] = 255;
                }
            }
        }
        
        // update the cv images
        grayImage.flagImageChanged();
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
     //   contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
    }
}

//--------------------------------------------------------------
//Audio input
void ofApp::audioReceived( float *input, int bufferSize, int nChannels )
{
    // samples are "interleaved"
     for (int i = 0; i < bufferSize; i++){
     left[i] = input[i*2];
     right[i] = input[i*2+1];
     }
     //bufferCounter++;
}


//--------------------------------------------------------------
//Audio output
void ofApp::audioOut(float *output, int bufferSize, int nChannels)
{
    
    for (int i=0; i<bufferSize; i++) {
        //Push current audio sample value from buffer
        //into both channels of output.
        //Also global volume value is used
        output[ 2*i ] = left[i] * volume;
        output[ 2*i + 1 ] = right[i] * volume;
        //Shift to the next audio sample
        //playPos++;
        //When the end of buffer is reached, playPos sets to 0
        //So we hear looped sound
        //playPos %= N;
    }
    
}


//--------------------------------------------------------------
void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
    gui.saveToFile("settings.xml");

    
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
		case'p':
			bDrawPointCloud = !bDrawPointCloud;
			break;
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
			
		case '1':
			kinect.setLed(ofxKinect::LED_GREEN);
			break;
			
		case '2':
			kinect.setLed(ofxKinect::LED_YELLOW);
			break;
			
		case '3':
			kinect.setLed(ofxKinect::LED_RED);
			break;
			
		case '4':
			kinect.setLed(ofxKinect::LED_BLINK_GREEN);
			break;
			
		case '5':
			kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
			break;
			
		case '0':
			kinect.setLed(ofxKinect::LED_OFF);
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
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
    float dist = kinect.getDistanceAt(x, y);
    ofLog(OF_LOG_NOTICE,"distancia" + ofToString( dist ) ) ;
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
