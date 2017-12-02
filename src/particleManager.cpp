#include "particleManager.h"


//--------------------------------------------------------------
//----------------------  Params -------------------------------
//--------------------------------------------------------------
Params param;        //Definition of global variable

void Params::setup( ) {
    eCenter = ofPoint( ofGetWidth() / 2, ofGetHeight() / 2 );
    eRad = 100;
    velRad = 0;
    lifeTime = 2.0;
    
    rotate = 0;
    force = 1000;
    spinning = 1000;
    friction = 0.05;
    
    
}

Particle::Particle() {
    live = false;
    isMask = true;
}

//--------------------------------------------------------------
ofPoint randomPointInCircle( float maxRad ){
    ofPoint pnt;
    float rad = ofRandom( 0, maxRad );
    float angle = ofRandom( 0, M_TWO_PI );
    pnt.x = cos( angle ) * rad;
    pnt.y = sin( angle ) * rad;
    return pnt;
}

//--------------------------------------------------------------
void Particle::setup(  ofVec2f position ) {
    pos = position ;
    vel = ofVec2f( ofRandom( 5 ), ofRandom(5));
   // pos = param.eCenter + randomPointInCircle( param.eRad );
   // vel = randomPointInCircle( param.velRad );
    time = 0;
    lifeTime = param.lifeTime;
    live = true;
    hue = ofColor( 155, 66, 244 , 255);
    hue_die = ofColor( 66, 244, 238 ,200);
    alpha = 255;
}

//--------------------------------------------------------------
void Particle::update( float dt ){
    
    if ( live ) {
        //Rotate vel
        vel.rotate( 0, 0, param.rotate * dt );
        
        ofPoint acc;         //Acceleration
        ofPoint delta = pos - param.eCenter;
        float len = delta.length();
      /*  if ( ofInRange( len, 0, param.eRad ) ) {
            delta.normalize();
            
            //Attraction/repulsion force
            acc += delta * param.force;
            
            //Spinning force
            acc.x += -delta.y * param.spinning;
            acc.y += delta.x * param.spinning;
        }*/
        acc = ofPoint( 3, 3 );
        vel += acc * dt;            //Euler method
        vel *= ( 1 - param.friction );  //Friction
        
        //Update pos
        pos += vel * dt;    //Euler method
        
        //Update time and check if particle should die
        time += dt;
        if ( time >= lifeTime ) {
       
            live = false;   //Particle is now considered as died
        }
        
        //Si la particula se encuentra fuera de la mascara augmentamos su contador lifeTime
        if( param.mask.getColor( (pos.x), int(pos.y) ).r > 0.5 && isMask){
            lifeTime = param.lifeTime*0.85;
            acc = acc*-1.0;
          //  hue_die = ofColor::papayaWhip ;
            
            hue_die = ofColor(255, 44, 66);
            hue_die.a = 120;
            isMask = false;
        }
        
        //Updating color
        float lerpFactor = ofMap( time, 0, lifeTime, 0,  1  );
        hue = hue.lerp( hue_die , lerpFactor ) ;
        
    }
}

//--------------------------------------------------------------
void Particle::draw(){
    if ( live ) {
        //Compute size
        float size = ofMap(
                           fabs(time - lifeTime/2), 0, lifeTime/2, 3, 1 );
        
        //Compute color
        //hue.a = alpha;
        ofSetColor( hue );
        
        ofDrawCircle( pos, size );  //Draw particle
    }
}
