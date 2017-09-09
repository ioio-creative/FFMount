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

void ofApp::serialWrite(int arduinoID, string sw){
    
    
    
#ifdef USEOSC
    if(arduinoID == -1){
        
        for(int i=0; i< NUM_OF_WINGS; i++){
            sendOSC(arduino[i], sw);
        }
    }else if (arduinoID >= 0 && working_cable[arduinoID] && arduinoID<arduino.size()){
        sendOSC(arduino[arduinoID], sw);
    }
    
    
#else
    
    if(arduinoID == -1){
        
        int arraySize = 0;
        if(arduino.size() <= NUM_OF_WINGS){
            arraySize = arduino.size();
        }else{
            arraySize = NUM_OF_WINGS;
        }
        for(int i=0; i< arraySize; i++){
            //    for(int i=0; i< arduino.size(); i++){
            if(working_cable[i]){
                // The serial device can throw exceptions.
                try
                {
                    std::string text = sw;
                    
                    
                    
                    
                    ofx::IO::ByteBuffer textBuffer(text);
                    
                    arduino[i].writeBytes(textBuffer);
                    arduino[i].writeByte('\n');
                    
                }
                catch (const std::exception& exc)
                {
                    ofLogError("ofApp::update") << exc.what();
                }
            }
        }
        
    }
    else if (isArduinoConnected[arduinoID]==TRUE && arduinoID >= 0 && working_cable[arduinoID])
    {
        
        // The serial device can throw exeptions.
        try
        {
            std::string text = sw;
            
            ofx::IO::ByteBuffer textBuffer(text);
            
            arduino[arduinoID].writeBytes(textBuffer);
            arduino[arduinoID].writeByte('\n');
            
        }
        catch (const std::exception& exc)
        {
            ofLogError("ofApp::update") << exc.what();
        }
        
        
    }
    else{ ofLog() << "Arduino: " <<arduinoID << "not connected";} // todo put in gui
    
    ofLog() << "Arduino Write: " <<arduinoID <<  ":  "<<sw;
#endif
}

string ofApp::serialRead(int a){
    
    string combinedStr = "";
    // for(int i=0; i< arduino.size(); i++){
    
    // The serial device can throw exeptions.
#ifdef USEOSC
    
    
    
#else
    try
    {
        // Read all bytes from the device;
        uint8_t buffer[1024];
        vector<uint8_t> finalBuffer;
        finalBuffer.clear();
        while (arduino[a].available() > 0)
        {
            std::size_t sz = arduino[a].readBytes(buffer, 1024);
            ofLog() << "buffer size: " << sz;
            for (std::size_t j = 0; j < sz; ++j)
            {
                // std::cout << buffer[j];
                //ofLog() << "buf: " << buffer[j];
                if(isalnum(buffer[j]) || buffer[j] == '|' || buffer[j] == '-' ){
                    finalBuffer.push_back(buffer[j]);
                }
                
                
            }
            for(int i = 0; i< finalBuffer.size();i++){
                ofLog() << "New Buf : " << finalBuffer[i];
                combinedStr += ofToString(finalBuffer[i]);
            }
        }
        
    }
    catch (const std::exception& exc)
    {
        ofLogError("ofApp::update") << exc.what();
    }
    //  }
    
#endif
    return combinedStr;
}

//--------------------------------------------------------------
vector<int> ofApp::stringDecode(string s){
    
    vector<int> sToIntArray;
    
    
    for (int i=0; i<s.length(); i++)
    {
        if (s[i] == '-')
            s[i] = ' ';
    }
    
    vector<string> seglist;
    stringstream ss(s);
    string temp;
    while (ss >> temp)
        seglist.push_back(temp);
    
    //ofLog() << "seglist.size() " << seglist.size();
    /* for(int i=0; i<seglist.size(); i++){
     seglist[i].erase(seglist[i].find_last_not_of(" \n\r\t")+1);
     
     //  ofLog() << "seglist[i] : " << seglist[i];
     
     }*/
    bool isContainParameter = false;
    
    for(int i=0; i < seglist.size(); i++){
        for(int j=0; j < SERIAL_PARAMETERES.size(); j++){
            if(seglist[i] == SERIAL_PARAMETERES[j]){ //check if anything match with SERIAL_PARAMETERS
                ofLog() << "SERIAL_PARAMETERES[j]" << SERIAL_PARAMETERES[j];
                sToIntArray.push_back(j);
                isContainParameter= true;
            }
        }
        if(isContainParameter){
            if(i!=0 && is_number(seglist[i])){
                sToIntArray.push_back(std::stoi( seglist[i] ));
            }
        }
    }
    //LOAD
    for(int i=0; i < sToIntArray.size(); i++){
        ofLog() << "sToIntArray " << i << " : " << sToIntArray[i];
    }
    if(sToIntArray.size() == EEPROM.size()+1){
        //currentdisplayLog = ofToString(currentDebugArduinoID) +" EEPROM LOADED";
        return sToIntArray;
    }
    else{
        // vector<int> sToIntArray;
        return sToIntArray;
    }
    
}

//--------------------------------------------------------------

void ofApp::onSerialBuffer(const ofx::IO::SerialBufferEventArgs& args)
{
    // Buffers will show up here when the marker character is found.
    SerialMessage message(args.getBuffer().toString(), "", 500);
    serialMessages.push_back(message);
    ofLog() << "SERIALLLLLLLL : " << message.message;
}

//--------------------------------------------------------------

void ofApp::onSerialError(const ofx::IO::SerialBufferErrorEventArgs& args)
{
    // Errors and their corresponding buffer (if any) will show up here.
    SerialMessage message(args.getBuffer().toString(),
                          args.getException().displayText(),
                          500);
    serialMessages.push_back(message);
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
