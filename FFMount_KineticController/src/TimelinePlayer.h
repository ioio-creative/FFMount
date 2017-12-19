#ifndef __ParkYohoMusicPlayer__
#define __ParkYohoMusicPlayer__

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxCsv.h"
#include "Timeline.h"
#include "ofxXmlSettings.h"
#include "ofEventUtils.h"

#define KEYFRAME_MIN_VALUE 0
#define KEYFRAME_MAX_VALUE 3400
#define NUM_TIMELINE 31


// A class that wrap the music player, fft graph, timeline
// set the KEYFRAME_MIN_VALUE, KEYFRAME_MAX_VALUE for the keyframe's max/min value
// "getTimelineTweenValues()" will return all the timelines's values in a vector

class
	TimelinePlayer {
    public:
        void setup();
        void update();
        void draw();
        //----------------------   CSV   ---------------------
        ofxCsv csv;
        ofxCsv csvRecorder;
        
        //-----------------------     Timeline Player      -------------------
        int duration = 3 * 60 * 1000;//the duration of the graph in ms
        int isPlaying;
        int currentTime;
        int lastMaxPlayerTime; // record the last max sound time, used for prevent redraw the graph when song looped
        int lastPausePlayerTime; //record the time when the sound is paused
        int lastFrameTime;// previous frame's time (will update event not playing)
        int previousPlayTime;// previous played frame time (update only when playing)
        int currentPlayTime;// current played frame time (update only when playing)
        int doLoop = 1;
        int resumeAfterRelease = 0;
        
        float multiplier = 1.0f;
        
        //-----------------------     Graph      -------------------
        int layerToDraw = 0;
        float graphValue; //the y value used to plot graph
        float graphScrollX; //the current page of the graph
        float graphVScale;//Verticle scale to plot the graph
        vector <float> graphHistory; //history for fftSum
        vector <float> timeHistory; //history for the time of sound
        
        
        
        //-----------------------     Timeline      -------------------
        vector <Timeline> timelines;
        ofEvent<Keyframe> onKeyFrameEntered; //event to listen for keyframe entered
        
        //-----------------------     GUI      -------------------
        ofxButton saveGUIButton;
        ofxButton loadGUIButton;
        ofxButton playButton;
        ofxButton nextButton;
        ofxButton pauseButton;
        ofxButton saveButton;
        ofxButton loadButton;
        
        ofxLabel trackFileName;
        //ofxToggle repeatAllToggle; //TODO
        ofxToggle repeatThisToggle; //TODO
        
        ofxIntSlider layerSlide;
        
        ofxFloatSlider multiSlide;
        ofxFloatSlider graphSlide;
        ofxFloatSlider graphVScaleSlide;
        ofxFloatSlider graphHScaleSlide;
        ofxButton addKeyButton;
        ofxButton removeKeyButton;
        ofxButton selectKeyButton;
        ofParameter<int> keyframeSlider;
        
        ofxPanel gui;
        
        
        void playButtonPressed();
        void nextButtonPressed();
        void saveGUIButtonPressed();
        void loadGUIButtonPressed();
        void pauseButtonPressed();
        void stopButtonPressed();
        void saveButtonPressed();
        void loadButtonPressed();
        void addKeyButtonPressed();
        void removeKeyButtonPressed();
        void selectKeyButtonPressed();
        void layerChanged(int &setLayer);
        void multiChanged(float &setMulti);
        void graphScrollChanged(float &setScroll);
        void graphVScaleChanged(float &setVScale);
        void graphHScaleChanged(float &setHScale);
        void keyframeSliderChanged(int &val);
        void keyFrameAdded(Keyframe &kf); //dispatched when a keyframe is added to odd number of timeline
        void keyFrameSelected(Keyframe &kf);
        void keyFrameDeselected(int &i);
        void OnKeyFrameEnteredEvent(Keyframe &kf);//Sample event function to handle the keyframe enter
        
        //Use this function to get the tween value of timeline
        vector<float> getTimelineTweenValues();//every first value is the distance, every second value is the velocity
        float getTimelineValue(int id, float t);//get the timeline value at speicfic time
        bool getIsKeyframe(int id, float t);//return if the speific time is a keyframe
        int getCurrentTime();
        void setDuration(int d);
        
        //Use this function to get the velocity of timeline
        //vector<float> getTimelineVelocity();
        
        void mousePressed(int x, int y, int button);
        void mouseDragged(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        
        void exhibitionMode(bool t);
        
    private:
        void resetGraph(); //reset the fft sum graph and the timelines when playing a new sound
        Keyframe *selectedKeyframe; //selected keyframe for editing its value
        Keyframe nullKeyframe; //a keyframe that is null, i.e. no selected key frame
        
       // const float KEYFRAME_MIN_VALUE = -30000.0f; //THe minimum value for a keyframe in slider
        //const float KEYFRAME_MAX_VALUE = 30000.0f; //the maximum value for a keyframe in slider
        
        //const int NUM_TIMELINE = 24;
        void reloadTimelineFromSave();//load the timeline save file
        
        float graphScale = 0.05f; //draw scale for the graph  // for 0.05f , scale = 1/20
        
        ofParameter<int> keyframeTimeSlider;
        void keyframeTimeSliderChanged(int &x);
        
        ofParameter<int> playheadSlider;
        void playheadSliderChanged(int &x);
        
        
        bool isExhibitionMode;
        ofVideoPlayer movie;
    };


#endif
