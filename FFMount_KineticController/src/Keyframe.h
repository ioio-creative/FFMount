//A class for storing keyframe data. X position, value and selected state

#ifndef __ParkYohoKeyframe__
#define __ParkYohoKeyframe__


class Keyframe
{
public:
	Keyframe();

	bool selected; //a state whethter this keyframe is selected by user 
	float val; //value of this keyframe
	float x; //x position of the keyframe

};


#endif