#pragma once


//======== DEFINITION ===========

//#define USEOSC
//#define RECEIVER_IS_WINDOWS         // lattePanda / Win10

#define NUM_OF_WINGS 6 //Todo Transfer definition /variables to xml
#define NUM_OF_SERIAL_TO_INIT 8  //Com port device size
#define BAUD 57600 //Todo Transfer definition /variables to xml

#define MAX_Y_POS 5000
#define MAX_Y_SPEED 2000
#define MAX_Y_ACCEL 2000

#define MAX_Y_DUR 300 // 30.0 s

//======== INCLUDES ===========

#include "ofMain.h"

#include "ofxSerial.h"
#include "ofxGui.h"
#include "TimelinePlayer.h"
#include "Simulation.h"

//======== OSC COMMUNICATION ===========
//OSC is essential for final kinetic work connection. keep here

#ifdef USEOSC
#include "ofxOsc.h"
#define R_PORT 12346

#define S_HOST "192.168.1.101"
//#define S_HOST "localhost"
#define S_PORT 12345

#else

#endif

/*
 
 TIMELINE
 
 
 */


class SerialMessage
{
public:
    SerialMessage(): fade(0)
    {
    }
    
    SerialMessage(const std::string& _message,
                  const std::string& _exception,
                  int _fade):
    message(_message),
    exception(_exception),
    fade(_fade)
    {
    }
    
    std::string message;
    std::string exception;
    int fade;
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    
    
    //================== Serial ==================
    
    
    vector<string> SERIAL_PARAMETERES;
    
    vector<bool> serialSetup();
    
    string serialRead(int a);
    vector<int> stringDecode(string s);
    
    void serialWrite(int arduinoID, string sw);

    vector<bool> isArduinoConnected;
    vector<bool> isArduinoConnectedBySerial;

    void checkArduinoIsConnected();
    bool initOnUpdate; //to check for arduino in update() after setup()
    long checkArduinoMillis; //todo
    
#ifdef USEOSC
    vector<bool> isArduioPort;
    vector<int> arduino;
#else
    vector<ofx::IO::BufferedSerialDevice> comPort;
    vector<ofx::IO::BufferedSerialDevice> arduino;
#endif
    
    
    //unused
    //void sendChar();
    void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
    void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);
    
    vector<SerialMessage> serialMessages;
    
    string receivedMsg;
    vector<string> prevReceivedString;
    vector<string> receivedStringBuffer;
    vector<string> receivedString;
    vector<int> updateColor;
    
    //function
    void removeSubstrs(string& s, string& p);


#ifdef USEOSC
    //================== OSC ==================
    //send
    ofxOscSender sender;
    void sendOSC(int arduino, string s);
    //read
    vector<string> readOSC();
    ofxOscReceiver receiver;
#else
    
#endif
    
    
    
    //================== GUI ==================
    

    void guiSetup();
    void guiDraw();
    
    bool debugMode;
    ofxPanel guiDebug;
    ofxPanel guiDebug2;
    ofParameterGroup parametersDebug;
    ofXml settings;

    ofxButton style_Btn;
    ofxButton style_Btn_all_same;
    ofxButton style_Btn_all;
    ofxButton reset_Btn;
    ofxButton home_Btn;
    ofxToggle all_Tog;
    vector<ofParameter<bool>> working_cable;
    vector<ofParameter<bool>> input_pts;
    vector<ofParameter<bool>> output_pts;
    
    //EEPROM
    
    vector<ofParameter<int>> EEPROM;
    vector<ofxButton> EEPROM_btn;
    
    ofxButton EEPROM_saveBtn;
    ofxButton EEPROM_loadBtn;
    
    void loadButtonPressed();

    
    
    //Cable Pos
    ofxPanel guiCablePosLy;
    ofxPanel guiCablePosRy;
    ofParameterGroup parametersCablePos;

    vector<ofParameter<int>> cablePosLy;
    vector<ofParameter<int>> cablePosRy;
    
    //Cable Duration
    ofxPanel guiCableDurLy;
    ofxPanel guiCableDurRy;
    ofParameterGroup parametersCableDur;
    
    vector<ofParameter<int>> cableDurLy;
    vector<ofParameter<int>> cableDurRy;
    
    //Cable Pos Offset
    ofxPanel guiCablePosLyOffset;
    ofxPanel guiCablePosRyOffset;
    ofParameterGroup parametersCablePosOffset;
    vector<ofParameter<int>> cablePosLyOffset;
    vector<ofParameter<int>> cablePosRyOffset;
    
    bool showOffset;
    
    
    //Cable Accel
    ofxPanel guiCableAccelLy;
    ofxPanel guiCableAccelRy;
    ofParameterGroup parametersCableAccel;
    vector<ofParameter<int>> cableAccelLy;
    vector<ofParameter<int>> cableAccelRy;
    
    //Cable Speed
    ofxPanel guiCableSpeedLy;
    ofxPanel guiCableSpeedRy;
    ofParameterGroup parametersCableSpeed;
    vector<ofParameter<int>> cableSpeedLy;
    vector<ofParameter<int>> cableSpeedRy;
    

    
    //in Draw
    int page;
    int numOfPages;

    //================== Timeline player ==================
    
    TimelinePlayer timelinePlayer;
    void onKeyframe(Keyframe &kf);
    
    
    //================== Simulation ==================
    
    Simulation wing;
    
    
    //================== MISC ==================
    bool serialTrigger; //TO avoid ofxButton cause multiple click and send mutiple serial command;
    long prevSerialTriggerMillis; //TO avoid ofxButton cause multiple click and send mutiple serial command;
    long currMillis;
    
    bool isEmergencyStop;

    
    //================Parameteres================

    ofParameter<int> currentScene;
    ofParameter<int> currentArduinoID;
    ofParameter<int> currentMotor;
    
    //================ LED ====================
    ofParameter<int> LEDStyle;
    ofParameter<int> LEDParameter0;
    ofParameter<int> LEDParameter1;
    ofParameter<int> LEDParameter2;
    ofParameter<int> LEDParameter3;
    
    
    //================Style ====================
    ofParameter<int> currentStyle;
    void writeStyle(int s);
    void writeLEDStyle(int s, int ss);

    
    //================Other Functions ====================
    bool is_number(const std::string& s);
    
    //================ Video ====================
    ofVideoPlayer 		ffMovie;
    
};
