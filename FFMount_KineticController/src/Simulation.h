#ifndef __ParkYohoSimulation__
#define __ParkYohoSimulation__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"
//#include "ofxXmlSettings.h"
#include "ofEventUtils.h"
#include "ofxSvg.h"
#define MIN_ROT -30
#define MAX_ROT 30

class
	Simulation {
		public:
        Simulation(){};
			void setup(int w);
			void update();
			void draw(int x, int y, int w, int h);
        
            void setMouseControllable(bool t);
        
            vector<ofxSVG> svg;
            vector<float> rotate;
        
        void setRotate(int id, float r);
        ofEasyCam cam;
        
        ofFbo fbo;

	};


#endif
