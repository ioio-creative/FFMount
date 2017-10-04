#include "Timeline.h"

//init timeline values
void Timeline::setup() {
    selectedKeyframe = nullKeyframe;
    xPos = 0.0f;
    yPos = 0.0f;
    width = 0.0f;
    scroll = 0.0f;
    NULL_FRAME = 0;
    doAddKeyframeOnClick = false;
    doRemoveKeyframeOnClick = false;
    scale = 1;
}

void Timeline::update() {
    
}

void Timeline::draw() {
    ofPushStyle();
    ofPushMatrix();
    
    //draw background
    ofFill();
    ofEnableAlphaBlending();
    if (doAddKeyframeOnClick) {
        ofSetColor(200, 225, 200, 200);
    }
    else if (doRemoveKeyframeOnClick) {
        ofSetColor(225, 200, 200, 200);
    }
    else {
        ofSetColor(200, 200, 200, 200);
    }
    ofDrawRectangle(xPos, yPos, width, 20);
    ofDisableAlphaBlending();
    ofNoFill();
    ofSetColor(10, 10, 10);
    ofDrawRectangle(xPos, yPos, width, 20);
    
    //draw the id of the timeline
    ofSetColor(50, 50, 50);
    string str;
    if (id % 2) {
        if (id < 12){
            str = "Lx" + ofToString(id/2) +" Time";
        }else if(id >= 12 && id < 24){
            str = "Rx" + ofToString((id - 12)/2 ) +" Time";
        }else if(id >= 24 && id < 26){
            str = "FAN";
        }else{
            str = "LED";

        }
        //str = ofToString(id/2) + d + "Time";
    }
    else {
        if (id < 12){
            str = "Lx" + ofToString(id/2) +" MoveTo";
        }else if(id >= 12 && id < 24){
            str = "Rx" + ofToString((id - 12)/2 ) +" MoveTo";
        }else if(id >= 24 && id < 26){
            str = "FAN";
        }else{
            str = "LED";

        }
        //str = ofToString(id/2) + "MoveTo";
    }
    if(id == 26){ str+= " Style";}
    if(id == 27){ str+= " Interval";}
    if(id == 28){ str+= " Fade";}
    if(id == 29){ str+= " Min Range / Multiply";}
    if(id == 30){ str+= " Max Range";}
    
    ofDrawBitmapString(str, xPos, yPos + 15.0f);
    
    //draw keyframes
    ofFill();
    ofSetColor(225, 100, 100);
    for (int i = 0; i < frames.size(); i++) {
        Keyframe kf = frames[i];
        if (kf.selected) {
            ofSetColor(100, 225, 100);
        }
        else {
            ofSetColor(225, 100, 100);
        }
        ofDrawCircle(kf.x*scale, yPos+10.0f, 10.0f);
        
        ofSetColor(10, 10, 10);
        string str = ofToString(kf.val);
        ofDrawBitmapString(str, kf.x*scale -10.0f, yPos + 15.0f);
    }
    
    ofPopMatrix();
    ofPopStyle();
}

//function that detect user's click in the timeline/keyframes
void Timeline::mousePressed(int x, int y, int button) {
    //check if the click is inside the timeline
    if (y > yPos && y < yPos + 20 && x > xPos && x < xPos + width) {
        //deselect all keyframes
        deselectKeyframes();
        
        int i;
        Keyframe kf;
        //check if any keyframes is selected and choose the nearest to mousePos
        bool isSelected = false;
        int selectedKeyframeIndex;
        float minDist = 10.0f;
        for (i = 0; i < frames.size(); i++) {
            kf = frames[i];
            float dist = (kf.x*scale - scroll) - x;
            dist = std::abs(dist);
            if (dist < 10.0f && dist<minDist) {
                minDist = dist;
                isSelected = true;
                selectedKeyframeIndex = i;
            }
        }
        
        if (isSelected) {
            //if a keyframe is clicked, select it or remove it
            if (doRemoveKeyframeOnClick) {
                //check if the frame is currently selected
                if (frames[selectedKeyframeIndex].x == selectedKeyframe.x) {
                    //deselect it if the same
                    selectedKeyframe = nullKeyframe;
                    keyframeDeselectedEvent.notify(this, NULL_FRAME);
                }
                
                frames.erase(frames.begin() + selectedKeyframeIndex);
                //doRemoveKeyframeOnClick = false;
            }else {
                frames[selectedKeyframeIndex].selected = true;
                selectedKeyframe = frames[selectedKeyframeIndex];
                keyframeSelectedEvent.notify(this, frames[selectedKeyframeIndex]);
            }
        }else if (doAddKeyframeOnClick) {
            //deselect the keyframe if there is a selected
            if (nullKeyframe.x != selectedKeyframe.x) {
                selectedKeyframe = nullKeyframe;
                keyframeDeselectedEvent.notify(this, NULL_FRAME);
            }
            
            //if not clicking on any keyframe, add a new keyframe
            Keyframe newKf;
            newKf.timelineId = id;
            newKf.x = (scroll + x) / scale;
            //newKf.selected = true;
            frames.push_back(newKf);
            sortKeyframes();
            //selectedKeyframe = newKf;
            //keyframeSelectedEvent.notify(this, newKf);
            //doAddKeyframeOnClick = false;
            
            //odd number of timeline add keyframes will add to even number of timeline
            if (id % 2 == 0) {
                keyframeAddedEvent.notify(this, newKf);
            }
        }else {
            //deselect the keyframe if there is a selected
            if (nullKeyframe.x != selectedKeyframe.x) {
                selectedKeyframe = nullKeyframe;
                keyframeDeselectedEvent.notify(this, NULL_FRAME);
            }
            
        }
    }
}

void Timeline::addKeyframeByVal(float val, float x) {
    //deselect the keyframe if there is a selected
    if (nullKeyframe.x != selectedKeyframe.x) {
        selectedKeyframe = nullKeyframe;
        keyframeDeselectedEvent.notify(this, NULL_FRAME);
    }
    //if not clicking on any keyframe, add a new keyframe
    Keyframe newKf;
    newKf.x = x;
    newKf.val = val;
    newKf.timelineId = id;
    //newKf.selected = true;
    frames.push_back(newKf);
    sortKeyframes();
}

//deselect all keyframes
void Timeline::deselectKeyframes() {
    int i;
    Keyframe kf;
    for (i = 0; i < frames.size(); i++) {
        frames[i].selected = false;
    }
}

//set the position of the timeline
void Timeline::setPos(float x, float y, float w, float s, int i, float sc) {
    xPos = x;
    yPos = y;
    width = w;
    scroll = s;
    id = i;
    scale = sc;
}

//call this to enable Add key frame mode
void Timeline::addKeyframeOnClick() {
    doAddKeyframeOnClick = true;
    doRemoveKeyframeOnClick = false;
}

//call this to enable remove key frame mode
void Timeline::removeKeyframeOnClick() {
    doRemoveKeyframeOnClick = true;
    doAddKeyframeOnClick = false;
}

//call this to enable select key frame mode
void Timeline::selectKeyframeOnClick() {
    doRemoveKeyframeOnClick = false;
    doAddKeyframeOnClick = false;
}

//get the tween value of the keyframes at Position X
float Timeline::getValueAtPos(float posX) {
    
    int size = frames.size();
    //handle special cases of keyframes
    if (size == 0) {
        //if no keyframes
        return 0.0f;
    } else {
        //values before the first keyframe is set to same as first keyframe
        if (posX <= frames[0].x*scale) {
            return frames[0].val;
        }
        //values after the last keyframe is set to same as last keyframe
        if (posX >= frames[size-1].x*scale) {
            return frames[size-1].val;
        }
    }
    
    //values is in-between the keyframes
    float val = 0.0f;
    Keyframe k1; //nearest smaller keyframe
    Keyframe k2; //nearset larger keyframe
    float maxX = 100000000.0f;
    float minX = 0.0f;
    for (int i = 0; i < size; i++) {
        if (frames[i].x*scale <= posX && frames[i].x*scale >=minX) {
            k1 = frames[i];
            minX = k1.x*scale;
        }
        if (frames[i].x*scale >= posX && frames[i].x*scale <=maxX) {
            k2 = frames[i];
            maxX = k1.x*scale;
        }
    }
    //Tween equation
    float t = posX - k1.x*scale;
    float b = k1.val;
    float c = k2.val - k1.val;
    float d = k2.x*scale - k1.x*scale;
    
    //if duration is 0
    if (d == 0.0f) {
        return b;
    }
    
    if (k2.val > k1.val) {
        //ease in when value is going up
        val = -c * cos(t / d * (PI / 2)) + c + b; //easeInSine
    }
    else if (k2.val == k1.val) {
        //val = c * t / d + b;//linear
        val = k1.val;
    }else{
        //ease out when value is going down
        val = c * sin(t / d * (PI / 2)) + b; //easeOutSine
    }
    
    /*REFERENCE EQUATIONS from http://gizma.com/easing/#l
     t= current time
     b= start value
     c= end value
     d= duration
     
     //linear
     val = c * t / d + b;
     
     //easeInSine
     val = -c * cos(t / d * (PI / 2)) + c + b;
     
     //easeOutSine
     val = c * sin(t / d * (PI / 2)) + b;
     
     //easeInoutSine
     
     t /= d/2;
     if (t < 1) return c/2 * pow( 2, 10 * (t - 1) ) + b;
     t--;
     val = c/2 * ( -pow( 2, -10 * t) + 2 ) + b;
     */
    return val;
    
}

//TO DO: write a loop to read the saved data and populate the frames array
void Timeline::loadKeyframes() {
    
    Keyframe newKf;
    //newKf.x = XXXX;
    frames.push_back(newKf);
    
    sortKeyframes();
}

//TO DO: save keyframes data to file
void Timeline::saveKeyframes() {
    
}

//clear all the key frames and reset states
void Timeline::reset() {
    frames.clear();
    selectKeyframeOnClick();
    if (nullKeyframe.x != selectedKeyframe.x) {
        selectedKeyframe = nullKeyframe;
        keyframeDeselectedEvent.notify(this, NULL_FRAME);
    }
}

//Compare the keyframes's X position for sorting
struct keyframe_comparer
{
    bool operator()(const Keyframe& lhs, const Keyframe& rhs)
    {
        return lhs.x < rhs.x;
    }
};

//sort the frames using the compare function
void Timeline::sortKeyframes() {
    //sort the keyframes
    std::sort(frames.begin(), frames.end(), keyframe_comparer());
}
