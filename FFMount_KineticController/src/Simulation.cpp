#include "Simulation.h"

//--------------------------------------------------------------
void Simulation::setup() {
    
    for(int i=0; i < 12; i++){

        ofxSVG s;
        s.load("wing.svg");
        svg.push_back(s);
        rotate.push_back(0);
        
        fbo.allocate(800, 800, GL_RGBA);
    }
}

//--------------------------------------------------------------
void Simulation::update() {
    
}

//--------------------------------------------------------------
void Simulation::draw() {
    
    fbo.begin();
    ofClear(255,255,255, 0);

    
    cam.begin();
    for(int i=0; i < 6; i++){
        ofPushMatrix();
            ofRotate(180 );
            ofTranslate(130,40,50*-i);
            ofRotate(rotate[i*2], 0, 0, 180);
            ofTranslate(-130, -40);
            svg[i*2].draw();
        ofPopMatrix();
        ofPushMatrix();
            ofRotate(180 , 180 , 0, 0);
            ofTranslate(-255+130,40,-50*-i-10);
            ofRotate(rotate[i*2+1], 0, 0, 180);
            ofTranslate(-130, -40);
            svg[i*2+1].draw();
        ofPopMatrix();
    }
    cam.end();
    fbo.end();
    fbo.draw(0,200,800,800);
}

void Simulation::setRotate(int id, float r) {
    rotate[id] = r;
}
