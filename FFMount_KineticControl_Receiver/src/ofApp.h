#pragma once

#include "ofMain.h"

#include "ofxOsc.h"
#include "ofxSerial.h"

#define R_PORT 12345

#define S_HOST "localhost"
#define S_PORT 12346

#define NUM_MSG_STRINGS 8


#define NUM_OF_CABLES 8 //Todo Transfer definition /
#define BAUD 57600 //Todo Transfer definition /variables to xml

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
        void exit();
    
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

    
		ofxOscReceiver receiver;
    
        string msg_strings[NUM_MSG_STRINGS];
		
    vector<string> receivedOSCString;
    int currentArduinoID;
    
    
    //======= sender =========
    ofxOscSender sender;
    void sendOSC(int arduino, string s);
    void readOSC();
    
    //================== Serial ==================
    
    vector<bool> serialSetup();
    string serialRead(int a);
    //vector<int> stringDecode(string s);
    bool is_number(const std::string& s);
    vector<bool> isArduinoConnected;
    vector<bool> isArduinoConnectedBySerial;
    void serialWrite(int arduinoID, string sw);
    void checkArduinoIsConnected();
    bool initOnUpdate;
    long checkArduinoMillis; //todo
    
    vector<ofx::IO::BufferedSerialDevice> arduino;
    
        vector<ofParameter<bool>> working_cable;

    
    
    void onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args);
    void onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args);
    
    vector<SerialMessage> serialMessages;
    
    string receivedMsg;
    vector<bool> isReceivedString;
    vector<string> receivedStringBuffer;
    vector<string> receivedString;
    vector<int> updateColor;
    vector<string> displayText;

};
