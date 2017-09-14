//
//  KineticController.h
//  ParkYohoKineticControl
//
//  Created by lucas on 2017-03-08.
//
//

#ifndef __ParkYohoKineticControl__KineticController__
#define __ParkYohoKineticControl__KineticController__

#include <stdio.h>
#include "ofMain.h"

class Cable
{
    /*
     holds data and visualises single cable
     
     ?might also do interpolation to from one position?
     
     */
public:
    float leftX = 1;//TODO 0 is leftmost
    float rightX = 1;//TODO 0 is rightmost
    float leftY, rightY;//0 is top position, 1 bottom
    int index; //use index for visualisation position
    int totalCount;

    void draw()
    {
        int zPos = index * drawDepth;

        //float extendCenteredCables = (2-((rightX+1)-(1-leftX)))*100;//extend lenght when cables get closer to center

        float leftXcorr = ofMap(leftX, 0, 2, 2,0);
        float cDist = abs(leftXcorr-rightX);
       // ofLog() <<" leftX : " << leftX << " RightX : " << rightX;
        float extendCenteredCables = ofMap(cDist,0,1,100,0) + 250 ;
        ofPushMatrix();
        ofTranslate(0, 100, zPos - totalCount * drawDepth/2);//translate to position, minus center value
        ofRotateY(10);
        ofFill();
        ofSetColor(150, 150, 255, 60);
        ofDrawBox(0, 0, 0, drawWidth, 10, drawDepth-2);//draw top support
        ofNoFill();
        ofSetColor(255);
        float leftTop = -100-leftX * (drawWidth/-2.);
        float rightTop = 100- rightX * (drawWidth/2.);
        ofDrawBezier(leftTop, 0, 0, leftTop, -leftY * 400 - extendCenteredCables, 0, rightTop, -rightY * 400- extendCenteredCables, 0, rightTop, 0, 0);
        ofPopMatrix();
    }
    int drawWidth = 200;
    int drawDepth = 20;


};

class KinecticVisualisation
{
public:
    
    KinecticVisualisation()
    {
        cablesPositions.assign(cables, Cable());
        for(int i = 0; i < cablesPositions.size(); i++)
        {
            cablesPositions[i].index = i;
            cablesPositions[i].totalCount = cables;
        }
        
        for(int i = 0; i < cables; i++)
        {
            leftX.push_back(0);
            leftY.push_back(0);
            rightX.push_back(0);
            rightY.push_back(0);
        }
        
    }
    
    void set(int cableNum, int arduinoID,int s, float lx,float ly,float rx,float ry)
    {
        style = s;
        leftX[arduinoID] = lx;
        leftY[arduinoID] = ly;
        rightX[arduinoID] = rx;
        rightY[arduinoID] = ry;
        cables = cableNum;
    }
    
    ofVec4f get(int arduinoID)
    {
        return ofVec4f(leftX[arduinoID],leftY[arduinoID],rightX[arduinoID],rightY[arduinoID]);
        
    }
    
    void draw()
    {
        //temporary test positions
        for(int i = 0; i < cablesPositions.size(); i++)
        {
            float noiseSpeed = 0.1;
            float noiseSpread = 0.06;
            
            switch (style){
                case 0:
                    cablesPositions[i].leftX = ofNoise(ofGetElapsedTimef()*noiseSpeed, 0, i*noiseSpread);
                    cablesPositions[i].leftY = ofNoise(ofGetElapsedTimef()*noiseSpeed, 34774.3, i*noiseSpread);
                    cablesPositions[i].rightX = ofNoise(ofGetElapsedTimef()*noiseSpeed, 1238.44, i*noiseSpread);
                    cablesPositions[i].rightY = ofNoise(ofGetElapsedTimef()*noiseSpeed, 283.2, i*noiseSpread);
                    break;
                    
                case 1:
                    cablesPositions[i].leftX = leftX[i];
                    cablesPositions[i].leftY = leftY[i];
                    cablesPositions[i].rightX = rightX[i];
                    cablesPositions[i].rightY = rightY[i];
                    break;
                    
                case 11:
                    cablesPositions[i].leftX = leftX[i];
                    cablesPositions[i].leftY = leftY[i];
                    cablesPositions[i].rightX = rightX[i];
                    cablesPositions[i].rightY = rightY[i];
                    break;
                    
                default:
                    style = 0;

            }
    
        }
        
        cam.begin();
        for(int i = 0; i < cablesPositions.size(); i++)
        {
            cablesPositions[i].draw();
        }
        cam.end();
    }
    
    ofEasyCam cam;
    
    vector<Cable> cablesPositions;
    
    int cables = 20;
    
    int style = 0;
    vector<float> leftX, leftY, rightX, rightY;

};



#endif /* defined(__ParkYohoKineticControl__KineticController__) */
