#include "Simulation.h"

//--------------------------------------------------------------
void Simulation::setup(int w) {
    
    for(int i=0; i < w * 2; i++){

        ofxSVG s;
        s.load("wing.svg");
        svg.push_back(s);
        rotate.push_back(0);
        
        fbo.allocate(800, 800, GL_RGBA);
    }
}

//--------------------------------------------------------------
void Simulation::update() {
    fbo.begin();
    ofClear(255,255,255, 0);
    
    
    cam.begin();
    for(int i=0; i < 6; i++){
        ofPushMatrix();
        ofRotate(180 );
        ofTranslate(130,40  - (i*25),50*-i);
        ofRotate(rotate[i*2], 0, 0, 180);
        ofTranslate(-130, -40);
        svg[i*2].draw();
        ofPopMatrix();
        ofPushMatrix();
        ofRotate(180 , 180 , 0, 0);
        ofTranslate(-255+130,40  - (i*25),-50*-i-10);
        ofRotate(rotate[i*2+1], 0, 0, 180);
        ofTranslate(-130, -40);
        svg[i*2+1].draw();
        ofPopMatrix();
    }
    cam.end();
    fbo.end();
}

//--------------------------------------------------------------
void Simulation::draw(int x, int y, int w, int h) {
    

    fbo.draw(x,y,w,h);
}

void Simulation::setRotate(int id, float r) {
    rotate[id] = r;
}

void Simulation::setMouseControllable(bool t){
    if(t){
        cam.enableMouseInput();
    }else{
        cam.disableMouseInput();
    }

}
