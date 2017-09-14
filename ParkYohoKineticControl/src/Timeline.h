//A class for timeline that contain keyframes and calculate their tween values based on X position
//call getValueAtPos to get the tween value of keyframes
#ifndef __ParkYohoTimeline__
#define __ParkYohoTimeline__


#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"
#include "Keyframe.h"
#include "ofEvents.h"


class Timeline
{
public:
	ofEvent<Keyframe> keyframeSelectedEvent; //a event for other class to listen to keyframe selected
	ofEvent<int> keyframeDeselectedEvent; //a event for other class to listen to keyframe deselected

	int NULL_FRAME;
	vector <Keyframe> frames; //all the keyframes
	ofRectangle rect;
	Keyframe selectedKeyframe; //the selected keyframes (select keyframe for editing its value)

	float xPos;
	float yPos;
	float width;
	float scroll; //scroll value of the timeline
	int id; //id of this timeline
	bool doAddKeyframeOnClick;
	bool doRemoveKeyframeOnClick;	

	void setPos(float x, float y, float w, float scroll, int id); //set the position of timeline, scroll is the scroll amount of the graph
	void addKeyframeOnClick(); //enable add keyframe mode
	void addKeyframeByVal(float val, float x); //enable add keyframe mode
	void removeKeyframeOnClick(); //enable remove keyframe mode
	void selectKeyframeOnClick(); //disable add or remove keyframe mode
	float getValueAtPos(float posX); //get the tween value at X position
	void deselectKeyframes(); //deselect keyframes
								   
	//-----------  To do  --------------
	void loadKeyframes();//load keyframes data to init the frames array
	void saveKeyframes();//save keyframes data to file
	void reset();//remove all keyframes and ready for load keyframes to add data

	void setup();
	void update();
	void draw();
	void mousePressed(int x, int y, int button);

private:
	void sortKeyframes();
	Keyframe nullKeyframe;
};

#endif