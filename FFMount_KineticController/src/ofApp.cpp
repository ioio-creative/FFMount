#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(25);
    
    //================== Serial ==================
    
    isArduinoConnected = serialSetup();
    
    SERIAL_PARAMETERES = {"sa", "lo","on", "allhm"}; //save, load, online

    for(int i=0; i < NUM_OF_WINGS; i++){
        isArduinoConnectedBySerial.push_back(false);
    }
    initOnUpdate = true;

#ifdef USEOSC
    //================== OSC ==================
    // listen on the given port
    cout << "listening for osc messages on port " << R_PORT << "\n";
    receiver.setup(R_PORT);
    // open an outgoing connection to HOST:PORT
    sender.setup(S_HOST, S_PORT);
#else
    
#endif
    
    //================== MISC ==================
    serialTrigger = true;
    prevSerialTriggerMillis = ofGetElapsedTimeMillis();
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
//-----------------SERIAL COMMUNICATION ------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::checkArduinoIsConnected(){
    
    if(ofGetFrameNum() < 200){
        
        if(ofGetFrameNum() %30 == 0){
            // for(int i=0; i< arduino.size(); i++){
            // serialWrite(i, "C");
            //      ofSleepMillis(100);
            //  }
            serialWrite(-1, "C");
            ofLog() << "hello";
        }
    }
}


//--------------------------------------------------------------
vector<bool> ofApp::serialSetup(){ //int give the connection status of each cables
    
    
    
    vector<bool> connectionStatus;
    
    
    
    for(int i=0; i< NUM_OF_WINGS; i++){
        connectionStatus.push_back(0);
    }
#ifdef USEOSC
    
#ifdef RECEIVER_IS_WINDOWS
    //USING 32 port and Windows, To connect arduino in sequence, we have to do it one by one
    
    vector<int> allComPort;
    
    for(int i=0; i<NUM_OF_SERIAL_TO_INIT; i++){
        allComPort.push_back(i);
    }
    arduino.push_back(allComPort[17]); //1
    arduino.push_back(allComPort[21]); //2
    arduino.push_back(allComPort[2]); //3
    arduino.push_back(allComPort[3]); //4
    arduino.push_back(allComPort[5]); //5
    arduino.push_back(allComPort[20]); //6
    arduino.push_back(allComPort[27]); //7
    arduino.push_back(allComPort[6]); //8
    arduino.push_back(allComPort[15]); //9
    arduino.push_back(allComPort[16]); //10
    arduino.push_back(allComPort[19]); //11
    arduino.push_back(allComPort[25]); //12
    arduino.push_back(allComPort[11]); //13
    arduino.push_back(allComPort[12]); //14
    arduino.push_back(allComPort[13]); //15
    arduino.push_back(allComPort[14]); //16
    arduino.push_back(allComPort[31]); //17
    arduino.push_back(allComPort[1]); //18
    arduino.push_back(allComPort[4]); //19
    arduino.push_back(allComPort[18]); //20
    for(int i = 20; i < NUM_OF_SERIAL_TO_INIT; i++){
        arduino.push_back(300+i);
    }
    
    
    for(int i=0; i< NUM_OF_WINGS; i++){
        ofLog() <<arduino[i];
        isArduinoConnected.push_back(true);
    }
#else
    
    for(int i=0; i< NUM_OF_WINGS; i++){
        arduino.push_back(true);
        isArduinoConnected.push_back(true);
    }
    
#endif
    
#else
    
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    ofLogNotice("ofApp::setup") << "Connected Devices: ";
    for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    {
        ofLogNotice("ofApp::setup") << "\t" << devicesInfo[i];
    }
    int a=0;
    if (!devicesInfo.empty())
    {
        
        for (std::size_t i = 0; i < devicesInfo.size(); ++i)
        {
            string portDesc = devicesInfo[i].getDescription();
            ofLog() << "devicesInfo[i].getDescription() : " << devicesInfo[i].getDescription();
            
            if(portDesc.find("USB") != std::string::npos || portDesc.find("Arduino") != std::string::npos )
            {
                // Connect to the first matching device.
                ofx::IO::BufferedSerialDevice device;
                
                arduino.push_back(device);
                
                bool success = arduino[a].setup(devicesInfo[i], BAUD);
                
                if(success)
                {
                    connectionStatus[a] = true;
                    arduino[a].unregisterAllEvents(this);
                    arduino[a].registerAllEvents(this);
                    
                    ofLogNotice("ofApp::setup") << "Successfully setup " << devicesInfo[i];
                    a++;
                    ofSleepMillis(100);
                }
                else
                {
                    ofLogNotice("ofApp::setup") << "Unable to setup " << devicesInfo[i];
                }
            }
            
        }
    }
    else
    {
        ofLogNotice("ofApp::setup") << "No devices connected.";
    }
    
    ofSleepMillis(1000); //Keep it - Arduino Mega needs time to initiate (Autoreset issue)
    
    
    
#endif
    return connectionStatus;
    
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
