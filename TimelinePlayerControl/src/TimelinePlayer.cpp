#include "TimelinePlayer.h"

//--------------------------------------------------------------
void TimelinePlayer::setup() {
	//--- Player property
	isPlaying = 0;
	lastPausePlayerTime = 0;
	lastMaxPlayerTime = 0;
	currentTime = 0;
	lastFrameTime = ofGetElapsedTimef()* 1000.0f;
	previousPlayTime = -1;
	currentPlayTime = -1;
	//-----------------------     FFT      -------------------

	graphValue = 0.0f;

	//-----------------------     Graph      -------------------
	graphHistory.assign(0, 0.0);
	timeHistory.assign(0, 0.0);
	lastMaxPlayerTime = 0;
	graphScrollX = 0;
	//-----------------------     Timeline      -------------------
	selectedKeyframe = &nullKeyframe;
	for (int i = 0; i < NUM_TIMELINE; i++) {
		Timeline timeline;
		timeline.setup();
		ofAddListener(timeline.keyframeSelectedEvent, this, &TimelinePlayer::keyFrameSelected);
		ofAddListener(timeline.keyframeDeselectedEvent, this, &TimelinePlayer::keyFrameDeselected);
		timelines.push_back(timeline);
	}
	
	
	
	//-----------------------     GUI      -------------------
	playButton.addListener(this, &TimelinePlayer::playButtonPressed);
	//nextButton.addListener(this, &TimelinePlayer::nextButtonPressed);
	saveGUIButton.addListener(this, &TimelinePlayer::saveGUIButtonPressed);
	loadGUIButton.addListener(this, &TimelinePlayer::loadGUIButtonPressed);
	addKeyButton.addListener(this, &TimelinePlayer::addKeyButtonPressed);
	removeKeyButton.addListener(this, &TimelinePlayer::removeKeyButtonPressed);
	selectKeyButton.addListener(this, &TimelinePlayer::selectKeyButtonPressed);
	pauseButton.addListener(this, &TimelinePlayer::pauseButtonPressed);
	multiSlide.addListener(this, &TimelinePlayer::multiChanged);
	layerSlide.addListener(this, &TimelinePlayer::layerChanged);
	graphSlide.addListener(this, &TimelinePlayer::graphScrollChanged);
	graphVScaleSlide.addListener(this, &TimelinePlayer::graphVScaleChanged);
	graphHScaleSlide.addListener(this, &TimelinePlayer::graphHScaleChanged);
	keyframeSlider.addListener(this, &TimelinePlayer::keyframeSliderChanged);
	saveButton.addListener(this, &TimelinePlayer::saveButtonPressed);
	loadButton.addListener(this, &TimelinePlayer::loadButtonPressed);

	gui.setup("panel");
	gui.setDefaultWidth(300);
	gui.add(playButton.setup("Play From Start"));
	gui.add(pauseButton.setup("Pause/Resume"));
	gui.add(repeatThisToggle.setup("Loop", false));
	gui.add(layerSlide.setup("Layer to Draw", 0, 0, NUM_TIMELINE-1));
	gui.add(multiSlide.setup("Velocity Multiplier", 1, 0, 10));
	gui.add(graphSlide.setup("Graph ScrollX", 0, 0, 1));
	gui.add(graphVScaleSlide.setup("Graph VScale Cubic", 0.15f, 0.001f, 1));
	gui.add(graphHScaleSlide.setup("Graph HScale Cubic", 0.5f, 0.01f, 1));
	//gui.add(trackFileName.setup("Now Playing", ""));
	gui.add(saveButton.setup("Save Timeline"));
	gui.add(loadButton.setup("Load Timeline"));
	gui.add(addKeyButton.setup("Add Keyframe Mode"));
	gui.add(removeKeyButton.setup("Remove Keyframe Mode"));
	gui.add(selectKeyButton.setup("Select Keyframe Mode"));
	gui.add(keyframeSlider.set("Keyframe Value", 0, 0, 0));//the slider is disabled at start, enabled when a keyframe is selected
	gui.add(saveGUIButton.setup("Save GUI"));
	gui.add(loadGUIButton.setup("Load GUI"));

	//gui.loadFromFile("TimelinePlayerSettings.xml");

	//Sample code to Listen to Keyframe event
	ofAddListener(this->onKeyFrameEntered, this, &TimelinePlayer::OnKeyFrameEnteredEvent);
}

//--------------------------------------------------------------
void TimelinePlayer::update() {

	ofBackground(80, 20, 20);
	
	doLoop = repeatThisToggle;
	float elapsedTime = ofGetElapsedTimef() *1000.0f;


	//record the history if sound is playig. Skip drawing if the sound is looped ( where the currnetTime < lastMaxSoundTime)
	if (isPlaying) {
		//advance the currentTime
		currentTime = currentTime + (elapsedTime - lastFrameTime);
		//loop the player timeline
		if (currentTime >= duration ) {
			if (doLoop) {
				playButtonPressed();
			}
			else {
				lastPausePlayerTime = duration;
				currentTime = duration;
				isPlaying = false;
			}
		}

		float graphWidth = duration * 1.0f * graphScale;
		float playheadPos = 0.0f;
		if (isPlaying) {
			playheadPos = graphWidth*currentTime / duration;
		}
		else if (duration >0) {
			playheadPos = graphWidth*(lastPausePlayerTime*1.0f / duration);
		}

		if (currentTime >= lastMaxPlayerTime) {
			graphValue = getTimelineTweenValues()[layerToDraw];
			//graphValue = timelines[0].getValueAtPos(playheadPos);
			graphHistory.push_back(graphValue);
			timeHistory.push_back(currentTime);
			if (currentTime > lastMaxPlayerTime)
				lastMaxPlayerTime = currentTime;
		}

		//keyframe events
		if (previousPlayTime > currentPlayTime) {
			previousPlayTime = -1;
		}
		else {
			previousPlayTime = currentPlayTime;
		}
		currentPlayTime = currentTime;

		for (int n = 0; n < timelines.size(); n++) {
			Keyframe kf;
			int size = timelines[n].frames.size();
			for (int i = 0; i < size; i++) {
				kf = timelines[n].frames[i];
				if (kf.x > previousPlayTime && kf.x <= currentPlayTime) {
					//ofLog() << "onEnterKeyFrame : " << n << ", " << kf.x << ", " << kf.val;
					ofNotifyEvent(onKeyFrameEntered, kf);
				}
			}
			
		}
	}
	lastFrameTime = elapsedTime;
	

	//-----------------------     Timeline      -------------------
	for (int i = 0; i < timelines.size(); i++) {
		timelines[i].update();
	}
	
	
}

//--------------------------------------------------------------
void TimelinePlayer::draw() {

	//-----------------------     FFT      -------------------
	ofPushStyle();
	ofSetColor(255, 255, 255, 255);
	
	//calculate the data for drawing graphs
	float graphWidth = duration * 1.0f * graphScale;
	float playheadPos = 0.0f;
	if (isPlaying) {
		playheadPos = graphWidth*currentTime/duration;
	}else if (duration >0) {
		playheadPos = graphWidth*(lastPausePlayerTime*1.0f / duration);
	}

	ofPushMatrix();
	ofPoint guiPos = gui.getPosition();
	ofTranslate(-graphScrollX * graphWidth , 0, 0);//translate graph to current scroll

	//draw the value as a line graph
	int offsetY = 350;//offset Y for drawing the graph
	ofSetColor(225);
	ofSetLineWidth(1);
	ofFill();
	ofSetColor(125);
	ofDrawRectangle(0, offsetY, graphWidth, 20);
	
	//draw time indicators

	//each 1 seconds
	ofSetColor(30);
	for (int i = 1000; i < duration; i += 1000) {
		ofDrawRectangle(i*graphScale, offsetY+10.0f, 1.0f, 10.0f);
	}
	//each 5 seconds
	ofSetColor(30);
	for (int i = 5000; i < duration; i += 5000) {
		ofDrawRectangle(i*graphScale, offsetY, 1.0f, 20.0f);
	}
	//each 10 seconds
	ofSetColor(250, 0, 0);
	for (int i = 10000; i < duration; i += 10000) {
		ofDrawRectangle(i*graphScale, offsetY, 1.0f, 20.0f);
	}
	//each 60 seconds
	ofSetColor(0, 250, 250);
	for (int i = 60000; i < duration; i += 60000) {
		ofDrawRectangle(i*graphScale, offsetY, 1.0f, 20.0f);
	}
	

	ofSetColor(100, 200, 100);
	if (graphHistory.size() > 0) {
		ofBeginShape();

		for (unsigned int i = 0; i < graphHistory.size(); i++) {
			if (i == 0) ofVertex(i, offsetY);
			float t = timeHistory[i];

			ofVertex(t*graphScale, offsetY - graphHistory[i] * 10* graphVScale);

			if (i == graphHistory.size() - 1) ofVertex(t*graphScale, offsetY);
		}
		ofEndShape(false);
	}

	//draw the timeline
	for (int i = 0; i < timelines.size(); i++) {
		timelines[i].setPos(0, offsetY+20 + i * 20, graphWidth, graphScrollX * graphWidth, i, graphScale);
		timelines[i].draw();
	}

	//draw playhead
	ofSetColor(100, 255, 100);
	ofDrawRectangle(playheadPos - 1, offsetY, 2, 500);
	
	ofPopMatrix();

	//print out the useful values , timeline's value : timeline.getValueAtPos(playheadPos) or getTimelineTweenValues()[0]
	ofSetColor(255, 255, 255);
	float currentPos = 0.0f;
	if (isPlaying) {
		currentPos = currentTime;
	}else if(duration >0){
		currentPos = lastPausePlayerTime;
	}

	
	string reportString = "GraphValue: " + ofToString(graphValue) + "\ntime: " + ofToString(currentPos) + " / " + ofToString(duration) + "\nTimeline: " + ofToString(getTimelineTweenValues());
	//ofSetColor(50, 150, 50);
	//ofDrawBitmapString(reportString, 5, 360);
	ofSetColor(255, 255, 255);
	ofDrawBitmapString(reportString, 5, 865);



	//test timeline functions

	reportString = "getTimelineValue(0, 1001): " + ofToString(getTimelineValue(0, 1001)) + "\ngetIsKeyframe(0, 1001): " + ofToString(getIsKeyframe(0, 1001));
	ofDrawBitmapString(reportString, 5, 825);


	ofPopStyle();
	//-----------------------     GUI      -------------------

	gui.draw();
}



//---------------- Timeline functions ----------------

//Sample event function to handle the keyframe enter
void TimelinePlayer::OnKeyFrameEnteredEvent(Keyframe &kf) {
	ofLog() << "onEnterKeyFrame : " << kf.timelineId <<","<<kf.val <<", " << kf.x;
}


//function for other classes to get the timeline's keyframes tweening value
vector<float> TimelinePlayer::getTimelineTweenValues() {
	float graphWidth = duration * 1.0f * graphScale;
	float playheadPos = 0.0f;
	if (isPlaying) {
		playheadPos = graphWidth*currentTime / duration;
	}
	else if (duration >0) {
		playheadPos = graphWidth*(lastPausePlayerTime*1.0f / duration);
	}

	vector<float> rslt;
	for (int i = 0; i <  timelines.size(); i++) {
		float val = timelines[i].getValueAtPos(playheadPos);
		if (i % 2) {
			//every second row will store velocity, which multiply by the multiplier
			rslt.push_back(multiplier*val);
		} else {
			//every first row will store distance
			rslt.push_back(val);
		}
	}

	return rslt;
}

//function for other classes to get the timeline's keyframes tweening value
float TimelinePlayer::getTimelineValue(int id, float t) {
	float val = timelines[id].getValueAtPos(t* graphScale);
	if (id % 2) {
		//every second row will store velocity, which multiply by the multiplier
		val = multiplier*val;
	}
	return val;
}

bool TimelinePlayer::getIsKeyframe(int id, float t) {
	int size = timelines[id].frames.size();
	for (int i = 0; i < size; i++) {
		if (timelines[id].frames[i].x == t) {
			return true;
		}
	}
	return false;
}

/*
//function for other classes to get the timeline's velocity
vector<float> TimelinePlayer::getTimelineVelocity() {
	float graphWidth = duration * 1.0f * graphScale;
	float playheadPos = 0.0f;
	if (isPlaying) {
		playheadPos = graphWidth*currentTime / duration;
	}
	else if (duration >0) {
		playheadPos = graphWidth*(lastPausePlayerTime*1.0f / duration);
	}

	vector<float> rslt;
	for (int i = 0; i < timelines.size(); i++) {
		float dy = timelines[i].getValueAtPos(playheadPos) - timelines[i].getValueAtPos(playheadPos - 1);

		//velocity = delta distance/ delta time
		float val = dy;

		//acceleration = delta velocity/ delta time
		//dy = timelines[i].getValueAtPos(playheadPos+1) - timelines[i].getValueAtPos(playheadPos);
		//val = dy - val;

		rslt.push_back(val);
	}

	return rslt;
}
*/
void TimelinePlayer::keyframeSliderChanged(float &val) {
	if (selectedKeyframe->x != nullKeyframe.x) {
		selectedKeyframe->val = val;
	}
}

void TimelinePlayer::keyFrameSelected(Keyframe &kf) {
	//ofLog() << "select keyframe : " << kf.x;

	//deselect all timeline's keyframes
	for (int i = 0; i < timelines.size(); i++) {
		timelines[i].deselectKeyframes();
	}
	
	//re-select the kf
	kf.selected = true;
	selectedKeyframe = &kf;

	//ofLog() << keyframeSlider.getParameter().toString();// .setup("Keyframe Value", kf.val, 0, 20);
	keyframeSlider.setMax(KEYFRAME_MAX_VALUE);
	keyframeSlider.setMin(KEYFRAME_MIN_VALUE);
	keyframeSlider.set(kf.val);

}

void TimelinePlayer::keyFrameDeselected(int &i) {
	selectedKeyframe = &nullKeyframe;

	keyframeSlider.setMax(0);
	keyframeSlider.setMin(0);
	keyframeSlider.set(0);
}

void TimelinePlayer::addKeyButtonPressed() {
	for (int i = 0; i < timelines.size(); i++) {
		timelines[i].addKeyframeOnClick();
	}
}

void TimelinePlayer::removeKeyButtonPressed() {
	for (int i = 0; i < timelines.size(); i++) {
		timelines[i].removeKeyframeOnClick();
	}
}

void TimelinePlayer::selectKeyButtonPressed() {
	for (int i = 0; i < timelines.size(); i++) {
		timelines[i].selectKeyframeOnClick();
	}
}

//--------------------------------------------------------------
//-----------------------     GUI EVENTS     -------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
void TimelinePlayer::playButtonPressed() {
	resetGraph();
	//reloadTimelineFromSave();
	isPlaying = true;
	currentTime = 0;
}

//--------------------------------------------------------------
void TimelinePlayer::nextButtonPressed() {

}

void TimelinePlayer::saveGUIButtonPressed() {
	gui.saveToFile("TimelinePlayerSettings.xml");
}

void TimelinePlayer::loadGUIButtonPressed() {
	gui.loadFromFile("TimelinePlayerSettings.xml");
}

void TimelinePlayer::pauseButtonPressed() {
	if (isPlaying) {
		isPlaying = !isPlaying;
		lastPausePlayerTime = currentTime;
	} else {
		isPlaying = !isPlaying;
		currentTime = lastPausePlayerTime;
	}
}

void TimelinePlayer::mousePressed(int x, int y, int button) {
	//hit the timeline of the graph
	int offsetY = 350;
	if (y > offsetY && y < offsetY+20) {
		if (isPlaying) {
			resumeAfterRelease = 1;
			pauseButtonPressed();
		}
		else {
			resumeAfterRelease = 0;
		}
	}
	for (int i = 0; i < timelines.size(); i++) {
		timelines[i].mousePressed(x, y, button);
	}
}
void TimelinePlayer::mouseDragged(int x, int y, int button) {
	//hit the timeline of the graph
	int offsetY = 350;
	if (y > offsetY && y < offsetY + 20) {
		float graphWidth = duration * 1.0f * graphScale;
		float time = duration * (((x*1.0) / graphWidth) + graphScrollX);
		if (time > duration)
			time = duration;
		if (time < 0.0f)
			time = 0.0f;
		//ofLog() << "time : " << time;
		currentTime = time;
		if (!isPlaying) {
			lastPausePlayerTime = time;
		}
	}
}

void TimelinePlayer::mouseReleased(int x, int y, int button) {
	if (resumeAfterRelease) {
		pauseButtonPressed();
	}
}

void TimelinePlayer::saveButtonPressed() {
	for (int i = 0; i < timelines.size(); i++) {
		Timeline timeline = timelines[i];
		csvRecorder.clear();
		for (int j = 0; j < timeline.frames.size(); j++) {
			ofxCsvRow row;
			row.setFloat(0, timeline.frames[j].x);
			row.setFloat(1, timeline.frames[j].val);
			csvRecorder.addRow(row);
		}
		string fn = "t" + ofToString(i) + ".csv";
		csvRecorder.save(fn);
	}
	/*
	ofxXmlSettings track;
	track.addTag("track");
	track.pushTag("track");
	//volHistory is we want to save to a file
	for (int i = 0; i < timelines.size(); i++) {
		track.addTag("timeline");
		track.pushTag("timeline", i);
		Timeline timeline = timelines[i];
		for (int j = 0; j < timeline.frames.size(); j++) {
			//each position tag represents one point
			track.addTag("dat");
			track.pushTag("dat", j);
			//so set the three values in the file
			track.addValue("val", timeline.frames[j].val);
			track.addValue("t", timeline.frames[j].x / graphScale);
			track.popTag();
		}
		track.popTag();
	}
	track.popTag(); //pop position
	track.saveFile("TimelineData" + ofToString(".xml"));
	*/
}

void TimelinePlayer::loadButtonPressed() {
	reloadTimelineFromSave();
}

void TimelinePlayer::reloadTimelineFromSave() {
	//reset the timeline previous values
	for (int i = 0; i < timelines.size(); i++) {
		timelines[i].reset();
	}
	for (int n = 0; n < timelines.size(); n++) {
		string fn = "t" + ofToString(n) + ".csv";
		if (csv.load(fn)) {
			for (int i = 0; i < csv.getNumRows(); i++) {
				timelines[n].addKeyframeByVal(csv[i].getFloat(1), csv[i].getFloat(0));
			}
		}
	}
	/*
	//This is how you would load that very same file    
	ofxXmlSettings track;
	if (track.loadFile("TimelineData" + ofToString(".xml"))) {
		track.pushTag("track");
		int numTimeline = track.getNumTags("timeline");
		for (int i = 0; i < numTimeline; i++) {
			track.pushTag("timeline", i);

			int numSnd = track.getNumTags("dat");
			for (int j = 0; j < numSnd; j++) {
				track.pushTag("dat", j);
				timelines[i].addKeyframeByVal(track.getValue("val", 0.0f), track.getValue("t", 0.0f)*graphScale);
				track.popTag();
			}
			track.popTag();
		}

		track.popTag(); //pop position
	}
	else {
		ofLogError("Track Save file did not load!");
	}
	*/
}

//--------------------------------------------------------------
void TimelinePlayer::layerChanged(int &setLayer) {
	layerToDraw = setLayer;
}

void TimelinePlayer::multiChanged(float &setMulti) {
	multiplier = setMulti;
}

void TimelinePlayer::graphScrollChanged(float &setScroll) {
	graphScrollX = setScroll;
}

void TimelinePlayer::graphVScaleChanged(float &setVScale) {
	graphVScale = setVScale*setVScale*setVScale;
}

void TimelinePlayer::graphHScaleChanged(float &setHScale) {
	graphScale = setHScale*setHScale*setHScale;
}

void TimelinePlayer::resetGraph() {
	graphHistory.clear();
	timeHistory.clear();
	lastMaxPlayerTime = 0;
	lastPausePlayerTime = 0;

	//reset timeline
	//for (int i = 0; i < timelines.size(); i++) {
		//timelines[i].reset();
	//}
	selectedKeyframe = &nullKeyframe;
}