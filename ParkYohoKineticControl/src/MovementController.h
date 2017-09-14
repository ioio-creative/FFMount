#ifndef __ParkYohoMovementController__
#define __ParkYohoMovementController__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"

class
MovementController {
public:
    
    ofxPanel curveControls;
    ofParameterGroup parametersDebug;
    ofXml settings;
    
    vector<ofParameter<int>> curves;
    
    ofPolyline trail;
    float x,y;
    float increment;
    float angle;
    
    ofPoint curveCoord;
    int curveW;
    int curveH;
    
    int max_x_pos;
    int max_y_pos;
    
    int numOfCables;
    
    
    MovementController(){
        
        
    }
    
    void setup(int cableNum, int x1, int y1,int w, int h, int x_range, int y_range){
    
        max_x_pos = x_range;
        max_y_pos = y_range;
        
        curveControls.setup("CurveReadWrite", "settings.xml", x1+w+10, y1);
        for(int i=0; i< 8; i++){

                ofParameter<int> a;
                        if(i==0){
                            a.set("C" + ofToString(i),10,0,400); //lx,ly,rx,ry
                        }
                        else if(i==1){
                                a.set("C" + ofToString(i),40,0,100); //lx,ly,rx,ry
                        }
                        else if(i==2){
                            a.set("D" + ofToString(i),max_x_pos/2,0,max_x_pos); //lx,ly,rx,ry
                        }
                        else{
            a.set("C" + ofToString(i),100,0,1000);//lx,ly,rx,ry
            }
            curves.push_back(a);
            curveControls.add(curves[i]);
            
        }
        int eW = 80;
        curveControls.setSize(eW, 100);
        curveControls.setWidthElements(eW);
        
        curveCoord = ofPoint(x1,y1);
        curveW = w;
        curveH = h;
        numOfCables = cableNum;
     

    }
    
    vector<ofPoint> getPoints(){
        vector<ofPoint> p;
        p.clear();
        for(int i=0; i< numOfCables; i++){
            p.push_back(trail.getPointAtPercent((float)i /numOfCables));
            p[i].y = p[i].y * curves[1] + (curves[2]-max_x_pos/2)*4;
            p[i].x = p[i].x;
        }
        
        return p;
        
    }
    
    void setPoints(int a, int b, int c,int d){
        curves[0] = a;
        curves[1] = b;
        curves[2] = c;
        curves[3] = d;
    }
    
    void update(){
        x=0;
        
        trail.clear();
        angle = 0;
        increment = (float)curves[0]/1000/TWO_PI;
        for(int i=0; i< curveW;i++){
            x++;
            angle+=increment;
            if (angle>=TWO_PI) { //if the angle is more than or equal to two PI (a full rotation measured in Radians) then make it zero.
                angle=0;
            }
            y=(curveH/2)+ (curveH/2)*sin(angle+ curves[3]);
            trail.addVertex(ofPoint(curveCoord.x+ x,curveCoord.y+ y));
        }
    };
    
    void draw(){
        // ofEnableSmoothing();
        
        ofSetColor(255,0,0);
        
        ofDrawRectangle(curveCoord.x, curveCoord.y, curveW, curveH);
        ofSetColor(20);
        trail.draw();
        for(int i=0; i< numOfCables; i++){
            ofSetColor(0, 140, 255);
            ofPoint p = trail.getPointAtPercent((float)i/numOfCables);
            ofDrawCircle(p,5);
        }
        
        // ofDisableSmoothing();
        curveControls.draw();
    };
    
};


#endif
