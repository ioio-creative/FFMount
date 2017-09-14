#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(25);
    
    //================== GUI ==================
    
        guiSetup();
    
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
    
    //================== MISC INIT ==================
    serialTrigger = true;
    prevSerialTriggerMillis = ofGetElapsedTimeMillis();
    isEmergencyStop = false;
    
}

void ofApp::guiSetup(){
    //--- EEPROM ---
    parametersDebug.setName("settings");
    guiDebug.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 630, 0);
    
    guiDebug.setSize(100, 1000);
    guiDebug.setWidthElements(100);
    
    guiDebug2.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 830, 0);
    guiDebug2.setSize(100, 1000);
    guiDebug.setWidthElements(100);
    
    vector<string> EEPROM_names = {"HOME_MAXSPEED", "MAX_SPEED_X", "MAX_ACCELERATION_X", "MAX_SPEED_Y","MAX_ACCELERATION_Y","MAX_POSITION_LX","MAX_POSITION_LY","MAX_POSITION_RX","MAX_POSITION_RY","INVERT_DIR_LX","INVERT_DIR_LY","INVERT_DIR_RX","INVERT_DIR_RY"};
    
    
    vector<int> EEPROM_min = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    vector<int> EEPROM_max = {500, 1000, 5000, 1000,5000, 1000, 1000, 1000, 1000, 1, 1, 1, 1}; //Todo Transfer definition /variables to xml
    
    guiDebug.add(currentDebugArduinoID.set("Cable No + 1",0,0,NUM_OF_WINGS-1));
    guiDebug.add(currentScene.set("Song",0,0,NUM_OF_WINGS));
    
    for(int i=0; i< EEPROM_names.size(); i++){
        ofParameter<int> a;
        a.set(EEPROM_names[i],0,EEPROM_min[i],EEPROM_max[i]);
        EEPROM.push_back(a);
        guiDebug.add(EEPROM[i]);
    }
    vector<string> EEPROM_saveLoad_names = {"SAVE", "LOAD"};
    /*
     EEPROM_btn = {new ofxButton , new ofxButton}; //TODO problem with implementing ofxButton array
     for(int i=0; i< 2; i++){
     //  ofxButton a;
     //  a.setup(EEPROM_saveLoad_names[i]);
     //  EEPROM_btn.push_back(a);
     guiDebug.add(EEPROM_btn[i].setup("SAVE"+ofToString(i),50,50));
     }
     */
    
    //EEPROM
    guiDebug.add(EEPROM_saveBtn.setup(EEPROM_saveLoad_names[0]));
    guiDebug.add(EEPROM_loadBtn.setup(EEPROM_saveLoad_names[1]));
    EEPROM_loadBtn.addListener(this, &ofApp::loadButtonPressed);
    //Style
    guiDebug.add(currentStyle.set("Style",11,0,NUM_OF_WINGS)); //TODO
    guiDebug.add(style_Btn.setup("Set Position:"));
    guiDebug.add(style_Btn_all_same.setup("Set Position ALL Same:"));
    guiDebug.add(style_Btn_all.setup("Set Position ALL:"));
    guiDebug.add(reset_Btn.setup("Reset:"));
    guiDebug.add(home_Btn.setup("Home: "));
    guiDebug.add(all_Tog.setup("ALL TOGGLE:",false));
    
    all_Tog = false;
    int numOfWaveForm = 4;
    for(int i=0; i< 4; i++){
        ofParameter<bool> a;
        a.set("input " + ofToString(i),false);
        ofParameter<bool> b;
        string c = "";
        if(i==0){
            c+= "Lr";
        }
        if(i==1){
            c+= "Ry";
        }
        if(i==2){
            c+= "Ly2";
        }
        if(i==3){
            c+= "Ry2";
        }
        
        b.set(c,false);
        input_pts.push_back(a);
        output_pts.push_back(b);
        guiDebug.add(input_pts[i]);
        guiDebug.add(output_pts[i]);
    }
    
    for(int i=0; i< NUM_OF_WINGS; i++){
        ofParameter<bool> a;
        //  if(i == 10){
        //    a.set("input " + ofToString(i),false);
        // }else{
        a.set("Online WING " + ofToString(i),true);
        // }
        working_cable.push_back(a);
        guiDebug2.add(working_cable[i]);
    }
    
    
    // guiDebug.add(all_Tog.setup("SET ALL:"));
    
    //textField.addListener(this,&ofApp::serialTextInput);
    //--- Cable Position Control ---
    
    
    parametersCablePos.setName("cablePosition");
    guiCablePosLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 400);
    guiCablePosRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 400);
    for(int i=0; i< NUM_OF_WINGS; i++){
        ofParameter<int> a;
        a.set("P Ly" + ofToString(i),0,0,MAX_Y_POS);
        ofParameter<int> b;
        b.set("P Ry" + ofToString(i),0,0,MAX_Y_POS);
        cablePosLy.push_back(a);
        cablePosRy.push_back(b);
        guiCablePosLy.add(cablePosLy[i]);
        guiCablePosRy.add(cablePosRy[i]);
    }
    
    //--- Cable Position Offset Control ---
    showOffset = false;
    
    parametersCablePosOffset.setName("cablePositionOffset");
    guiCablePosLyOffset.setup("EEPROMReadWrite", "settings.xml",  100, 400);
    guiCablePosRyOffset.setup("EEPROMReadWrite", "settings.xml",  200, 400);
    for(int i=0; i< NUM_OF_WINGS; i++){
        ofParameter<int> a;
        a.set("Offset Ly" + ofToString(i),0,0,MAX_Y_POS*0.1);
        ofParameter<int> b;
        b.set("Offset Ry" + ofToString(i),0,0,MAX_Y_POS*0.1);
        cablePosLyOffset.push_back(a);
        cablePosRyOffset.push_back(b);
        guiCablePosLyOffset.add(cablePosLyOffset[i]);
        guiCablePosRyOffset.add(cablePosRyOffset[i]);
    }
    

    //--- Cable Accel Control ---

    parametersCableAccel.setName("cableAccel");
    guiCableAccelLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 310, 400);
    guiCableAccelRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 260, 400);
    for(int i=0; i< NUM_OF_WINGS; i++){
        ofParameter<int> a;
        a.set("A" + ofToString(i),500,0,MAX_Y_ACCEL);
        ofParameter<int> b;
        b.set("A" + ofToString(i),500,0,MAX_Y_ACCEL);
        cableAccelLy.push_back(a);
        cableAccelRy.push_back(b);
        guiCableAccelLy.add(cableAccelLy[i]);
        guiCableAccelRy.add(cableAccelRy[i]);
    }
    //--- Cable Speed Control ---
    
    parametersCableSpeed.setName("cableSpeed");
    guiCableSpeedLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 420, 400);
    guiCableSpeedRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 370, 400);
    for(int i=0; i< NUM_OF_WINGS; i++){
        ofParameter<int> a;
        a.set("A" + ofToString(i),500,0,MAX_Y_SPEED);
        ofParameter<int> b;
        b.set("A" + ofToString(i),500,0,MAX_Y_SPEED);
        cableSpeedLy.push_back(a);
        cableSpeedRy.push_back(b);
        guiCableSpeedLy.add(cableSpeedLy[i]);
        guiCableSpeedRy.add(cableSpeedRy[i]);
    }
    
    
    int guiPosCableW = 100;
    int guiCableH = 500;
    guiCablePosLy.setSize(guiPosCableW, guiCableH);
    guiCablePosLy.setWidthElements(guiPosCableW);
    guiCablePosRy.setSize(guiPosCableW, guiCableH);
    guiCablePosRy.setWidthElements(guiPosCableW);
    
    
    guiCablePosLyOffset.setSize(guiPosCableW, guiCableH);
    guiCablePosLyOffset.setWidthElements(guiPosCableW);
    guiCablePosRyOffset.setSize(guiPosCableW, guiCableH);
    guiCablePosRyOffset.setWidthElements(guiPosCableW);
    
    int guiCableW = 50;
    
    guiCableAccelLy.setSize(guiCableW, guiCableH);
    guiCableAccelLy.setWidthElements(guiCableW);
    guiCableAccelRy.setSize(guiCableW, guiCableH);
    guiCableAccelRy.setWidthElements(guiCableW);
    
    guiCableSpeedLy.setSize(guiCableW, guiCableH);
    guiCableSpeedLy.setWidthElements(guiCableW);
    guiCableSpeedRy.setSize(guiCableW, guiCableH);
    guiCableSpeedRy.setWidthElements(guiCableW);
    
    
}

//--------------------------------------------------------------
void ofApp::update(){

    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    //currTime = ofGetElapsedTimeMillis();

    
    //OSC
#ifdef USEOSC
    receivedString = readOSC();
#else
    
    
    //================== Serial ==================
    for(int i=0; i < arduino.size(); i++){
        receivedStringBuffer[i] += ofTrim(serialRead(i));
        
        bool reachEnd = false;
        for (int j=0; j<receivedStringBuffer[i].size(); j++)
        {
            if (receivedStringBuffer[i][j] == '|'){
                receivedStringBuffer[i][j] = ' ';
                reachEnd = true;
            }
        }
        if(reachEnd){
            vector<string> array;
            stringstream ss(receivedStringBuffer[i]);
            string temp;
            while (ss >> temp)
                array.push_back(temp);
            if(array.size()>0){
                receivedString[i] = array[0];
                ofLog() << "receivedString[i] :" <<receivedString[i];
                reachEnd = false;
                receivedStringBuffer[i] = "";
            }
        }
    }
    
#endif
    
    
    if(initOnUpdate){
        checkArduinoIsConnected();
    }

}

void ofApp::guiDraw(){
    //if(drawDebugGui){
    guiDebug.draw();
    guiDebug2.draw();
    // }
    
    guiCablePosLy.draw();
    guiCablePosRy.draw();
    
    guiCableAccelLy.draw();
    guiCableAccelRy.draw();
    
    guiCableSpeedLy.draw();
    guiCableSpeedRy.draw();
}


//--------------------------------------------------------------
void ofApp::draw(){

    if(isEmergencyStop){
        ofBackground(255, 0, 0);
        ofSetColor(255);
        
        std::stringstream ss;
        ss << "FrameRate : "<< ofGetFrameRate() << endl;
        ss << "EMERGENCY STOP - 'r' to relase: " << endl;
        ss << "EMERGENCY STOP - 'q' to reset all arduino: " << endl;
        
        ofDrawBitmapString(ss.str(), ofVec2f(20, 20));
    }
    //================== Debug Mode ==================
    else{
    ofBackground(100, 0, 0);
    
    ofSetColor(255);

    
    //======== GUI =========
    guiDraw();
    }

    
}

//--------------------------------------------------------------
//--------------------------GUI EVENT -----------------------------
//--------------------------------------------------------------
void ofApp::loadButtonPressed(){
    serialWrite(currentDebugArduinoID, "L");
    serialTrigger = false;
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
//--------------------------------------------------------------
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
