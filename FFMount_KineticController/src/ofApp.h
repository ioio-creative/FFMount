#pragma once

#define NUM_OF_WINGS 12 //Todo Transfer definition /variables to xml
#define BAUD 57600 //Todo Transfer definition /variables to xml

//INCLUDES

#include "ofMain.h"

#include "ofxSerial.h"
#include "ofxGui.h"

//OSC is essential for final kinetic work connection. keep here

#ifdef USEOSC
#include "ofxOsc.h"
#define R_PORT 12346

#define S_HOST "192.168.0.101"
//#define S_HOST "localhost"
#define S_PORT 12345

#else

#endif


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
    
    vector<bool> serialSetup();
    string serialRead(int a);
    vector<int> stringDecode(string s);
    bool is_number(const std::string& s);
    vector<bool> isArduinoConnected;
    vector<bool> isArduinoConnectedBySerial;
    void serialWrite(int arduinoID, string sw);
    void checkArduinoIsConnected();
    bool initOnUpdate;
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
    
    vector<string> SERIAL_PARAMETERES;

    


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
    
    
    //================== MISC ==================
    bool serialTrigger; //TO avoid ofxButton cause multiple click and send mutiple serial command;
    long prevSerialTriggerMillis; //TO avoid ofxButton cause multiple click and send mutiple serial command;
    long currMillis;
    
    
    //================Parameteres================
        vector<ofParameter<bool>> working_cable;
        vector<ofParameter<int>> EEPROM;
    
    
};
