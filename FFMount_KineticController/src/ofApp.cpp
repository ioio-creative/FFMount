#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(20);
    //================== GUI ==================
    guiSetup();
    //================== Serial ==================
    
    isArduinoConnected = serialSetup();
    
    SERIAL_PARAMETERES = {"sa", "lo", "on", "allhm", "mdone"}; //save, load, online
    
    for(int i=0; i < NUM_OF_WINGS; i++){
        isArduinoConnectedBySerial.push_back(false);
    }
    initOnUpdate = true;
    
    /*
     //================== Video ==================
     ffMovie.load("ffmount.mp4");
     ffMovie.setLoopState(OF_LOOP_NORMAL);
     ffMovie.stop();
     */
    //================== Timeline Player ==================
    timelinePlayer.setup();
    ofAddListener(timelinePlayer.onKeyFrameEntered, this, &ofApp::onKeyframe);
    timelinePlayer.loadButtonPressed();
    
    isExhibitionMode = true;
    timelinePlayer.exhibitionMode(isExhibitionMode);
    
    // ofLog() << "ffMovie.getDuration() : " << ffmovie.getDuration();
    // timelinePlayer.setDuration(ffMovie.getDuration()*1000);
    
    //================== Simulation ==================
    wing.setup(NUM_OF_WINGS);
    
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
    
    page = 0;
    numOfPages = 2;
    for(int i=0; i < NUM_OF_SERIAL_TO_INIT; i++){
        receivedString.push_back("");
        receivedStringBuffer.push_back("");
        prevReceivedString.push_back("");
        updateColor.push_back(255);
    }
    
    showBeginTrigger = false;
    isStyleChanged = false;
    
    prevStyle = currentStyle;
    
}

void ofApp::onKeyframe(Keyframe &kf){
    if(kf.timelineId < NUM_OF_WINGS *2  && kf.timelineId%2==0){ //LY
        ofLog() << "LY HAS KEYFRAME : " << kf.timelineId << " " << kf.val << " " << kf.x;
        currentArduinoID = kf.timelineId /2 ;
        cableDurLy[currentArduinoID] = timelinePlayer.getTimelineValue(kf.timelineId + 1, kf.x);
        cableDurRy[currentArduinoID] = 0; //means no input
        currentMotor = 0;
        writeStyleMode(2);
        
    }else if(kf.timelineId >= NUM_OF_WINGS *2  && kf.timelineId%2==0 && kf.timelineId < NUM_OF_WINGS *4){ //RY
        currentArduinoID = kf.timelineId /2 - NUM_OF_WINGS;
        currentMotor = 1;
        cableDurLy[currentArduinoID] = 0; //means no input
        cableDurRy[currentArduinoID] = timelinePlayer.getTimelineValue(kf.timelineId + 1, kf.x);
        writeStyleMode(2);
        ofLog() << "RY HAS KEYFRAME : " << kf.timelineId << " " << kf.val << " " << kf.x;
    }else if(kf.timelineId == 24){ //FAN
        ofLog() << "FAN " << timelinePlayer.getTimelineValue(kf.timelineId, kf.x);
        if(timelinePlayer.getTimelineValue(kf.timelineId, kf.x) > 100){
            serialWrite(FAN_SERIAL, "o");  // 'o' is on, 'f' is off
        }else{
            serialWrite(FAN_SERIAL, "f");  // 'o' is on, 'f' is off
        }
    }else{
        
        // currentArduinoID = kf.timelineId /2 - NUM_OF_WINGS;
        //currentMotor = 1;
        currentArduinoID = 0;
        cableDurLy[currentArduinoID] = 0; //means no input
        cableDurRy[currentArduinoID] = 0;//means no input
        writeStyleMode(0);
        ofLog() << "LED HAS KEYFRAME : " << kf.timelineId << " " << kf.val << " " << kf.x;
        //ofLog() << "LED " << LEDStyle;
        // writeStyleMode(2);
    }
    
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
    
    vector<string> EEPROM_names = {"HOME_MAXSPEED","MAX_SPEED_Y","MAX_ACCELERATION_Y","INVERT_DIR_Y"};
    
    vector<int> EEPROM_min = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    vector<int> EEPROM_max = {500, 1000, 5000, 1000,5000, 1000, 1000, 1000, 1000, 1, 1, 1, 1}; //Todo Transfer definition /variables to xml
    
    guiDebug.add(currentArduinoID.set("Wing No + 1",0,0,NUM_OF_WINGS-1));
    guiDebug.add(currentMotor.set("Motor No + 1",0,0,2));
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
    guiDebug.add(currentStyle.set("Style",4,0,NUM_OF_WINGS)); //TODO
    guiDebug.add(LEDStyle.set("LED Style",0,0,8));
    guiDebug.add(LEDParameter0.set("Interval",0,0,500));
    guiDebug.add(LEDParameter1.set("Fade",0,0,500));
    guiDebug.add(LEDParameter2.set("MinRange / Multiply",0,0,MAX_Y_POS));
    guiDebug.add(LEDParameter3.set("Max Range",0,0,MAX_Y_POS));
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
            c+= "Ly";
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
        a.set("WING Port " + ofToString(i),true);
        // }
        working_cable.push_back(a);
        guiDebug2.add(working_cable[i]);
    }
    
    
    // guiDebug.add(all_Tog.setup("SET ALL:"));
    
    //textField.addListener(this,&ofApp::serialTextInput);
    //--- Cable Position Control ---
    
    parametersCablePos.setName("cablePosition");
    guiCablePosLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 0);
    guiCablePosRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 0);
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
    
    //--- Cable Duration Control ---
    
    parametersCableDur.setName("cableDuration");
    guiCableDurLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 170);
    guiCableDurRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 170);
    for(int i=0; i< NUM_OF_WINGS; i++){
        ofParameter<int> a;
        a.set("Dur Ly" + ofToString(i),0,0,MAX_Y_DUR);
        ofParameter<int> b;
        b.set("Dur Ry" + ofToString(i),0,0,MAX_Y_DUR);
        cableDurLy.push_back(a);
        cableDurRy.push_back(b);
        guiCableDurLy.add(cableDurLy[i]);
        guiCableDurRy.add(cableDurRy[i]);
    }
    
    //--- Style 5 ----
    
    //--- Cable Position Control ---
    
    parametersCablePos2.setName("cablePosition 2");
    guiCablePosLy2.setup("guiCablePosLy2", "guiCablePosLy2.xml", ofGetWidth() - 200, 370);
    guiCablePosRy2.setup("guiCablePosRy2", "guiCablePosRy2.xml", ofGetWidth() - 100, 370);
    for(int i=0; i< NUM_OF_WINGS; i++){
        ofParameter<int> a;
        a.set("P2 Ly " + ofToString(i),0,0,MAX_Y_POS);
        ofParameter<int> b;
        b.set("P2 Ry " + ofToString(i),0,0,MAX_Y_POS);
        cablePosLy2.push_back(a);
        cablePosRy2.push_back(b);
        guiCablePosLy2.add(cablePosLy2[i]);
        guiCablePosRy2.add(cablePosRy2[i]);
    }
    
    //--- Cable Duration Control ---
    
    parametersCableDur2.setName("cableDuration 2");
    guiCableDurLy2.setup("guiCableDurLy2", "guiCableDurLy2.xml", ofGetWidth() - 200, 570);
    guiCableDurRy2.setup("guiCableDurRy2", "guiCableDurRy2.xml", ofGetWidth() - 100, 570);
    for(int i=0; i< NUM_OF_WINGS; i++){
        ofParameter<int> a;
        a.set("Dur2 Ly " + ofToString(i),0,0,MAX_Y_DUR);
        ofParameter<int> b;
        b.set("Dur2 Ry " + ofToString(i),0,0,MAX_Y_DUR);
        cableDurLy2.push_back(a);
        cableDurRy2.push_back(b);
        guiCableDurLy2.add(cableDurLy2[i]);
        guiCableDurRy2.add(cableDurRy2[i]);
    }
    
    loadStyle5();
    
    //--- Style 5 END----
    
    //--- Cable Position Offset Control ---  //not used??
    showOffset = false;
    
    parametersCablePosOffset.setName("cablePositionOffset");
    guiCablePosLyOffset.setup("EEPROMReadWrite", "settings.xml",  100, 0);
    guiCablePosRyOffset.setup("EEPROMReadWrite", "settings.xml",  200, 0);
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
    guiCableAccelLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 310, 0);
    guiCableAccelRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 260, 0);
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
    guiCableSpeedLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 420, 0);
    guiCableSpeedRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 370, 0);
    for(int i=0; i< NUM_OF_WINGS; i++){
        ofParameter<int> a;
        a.set("S" + ofToString(i),500,0,MAX_Y_SPEED);
        ofParameter<int> b;
        b.set("S" + ofToString(i),500,0,MAX_Y_SPEED);
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
    
    guiCableDurLy.setSize(guiPosCableW, guiCableH);
    guiCableDurLy.setWidthElements(guiPosCableW);
    guiCableDurRy.setSize(guiPosCableW, guiCableH);
    guiCableDurRy.setWidthElements(guiPosCableW);
    
    
    //---style 5
    guiCablePosLy2.setSize(guiPosCableW, guiCableH);
    guiCablePosLy2.setWidthElements(guiPosCableW);
    guiCablePosRy2.setSize(guiPosCableW, guiCableH);
    guiCablePosRy2.setWidthElements(guiPosCableW);
    
    guiCableDurLy2.setSize(guiPosCableW, guiCableH);
    guiCableDurLy2.setWidthElements(guiPosCableW);
    guiCableDurRy2.setSize(guiPosCableW, guiCableH);
    guiCableDurRy2.setWidthElements(guiPosCableW);
    
    // ---
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
    
    //====app init only ===
    
    if(initOnUpdate){
        checkArduinoIsConnected();
    }
    //====MISC ===
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    currMillis = ofGetElapsedTimeMillis();
    
    if(currMillis - prevShowBeginMillis > SHOW_DELAY_TIME && showBeginTrigger){
        timelinePlayer.loadButtonPressed();
        timelinePlayer.playButtonPressed();
        showBeginTrigger =false;
    }
    
    if(prevStyle != currentStyle){
        isStyleChanged = true;
        prevStyle = currentStyle;
    }
    if(isStyleChanged && currentStyle == 5){
        loadStyle5();
        isStyleChanged = false;
    }
    
    
    //------ Control Room -------
    string sTemp = serialReadCtrlrm();
    if(sTemp.find("0") != std::string::npos || sTemp.find("1") != std::string::npos){
        if(sTemp.find("0") != std::string::npos && currMillis - showDoneMillis > showDoneDur ){
            ofLog() << "Show Done, Reset";
            showDoneMillis = currMillis;
            isShowBegin(false);
            
        }
        if(sTemp.find("1") != std::string::npos && currMillis - showDoneMillis > showDoneDur ){
            ofLog() << "Show Begin";
            showDoneMillis = currMillis;
            isShowBegin(true);
        }
        /*
         ctrlrmReceivedString+= ofTrim(sTemp);
         ofLog() << " ctrlrmReceivedString.size() : "<< sTemp.size();
         ofLog() << " serialReadCtrlrm AA : " << ofTrim(sTemp);
         ofLog() << " serialReadCtrlrm VV : " << ctrlrmReceivedString;
         if(ctrlrmReceivedString.size() >= 4){
         if(ctrlrmReceivedString == "0000" || ctrlrmReceivedString == "1111"){
         ofLog() << "ctrlrmReceivedString Done : ";
         ctrlrmReceivedString = "";
         }else{
         ofLog() << "ctrlrmReceivedString NOT Done : ";
         
         }
         }
         */
    }
    
    
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
    
    for(int i=0; i< receivedString.size(); i++){
        if(stringDecode(receivedString[i]).size()>=1){
            if(stringDecode(receivedString[i])[0] == 1){ //load
                for(int j=0; j< stringDecode(receivedString[i]).size(); j++){
                    if(j< EEPROM.size()){
                        EEPROM[j] = stringDecode(receivedString[i])[j+1];
                    }
                }
            }
            
            if(stringDecode(receivedString[i])[0] == 2){ //online
                ofLog() << "device : " << i << " online";
                isArduinoConnectedBySerial[i] = true;
            }
            
            if(stringDecode(receivedString[i])[0] == 3){ //all home
                ofLog() << "device : " << i << " ALL HOME";
            }
            
            if(stringDecode(receivedString[i])[0] == 4){ //Motor Done Movement
                ofLog() << "device : " << i << " ALL HOME";
            }
            
        }
    }
    
    for(int i=0; i< NUM_OF_SERIAL_TO_INIT; i++){
        // ofLog() << "receivedString : "<< i << " : "<<receivedString[i];
        if(receivedString[i].size() > 0){
            prevReceivedString[i] = receivedString[i];
            receivedString[i]="";
            updateColor[i] = 255;
        }
        if(updateColor[i]>0){
            updateColor[i]--;
        }
        // ofLog() << "prevReceivedString : "<< i << " : "<<prevReceivedString[i];
        //ofLog() << "updateColor : "<< i << " : "<<updateColor[i];
        
    }
    
    
    //================== Timeline Player ==================
    if(currentStyle == 4){
        timelinePlayer.update();
        vector<float> a;
        a = timelinePlayer.getTimelineTweenValues();
        
        for(int i = 0; i < NUM_OF_WINGS; i++){
            cablePosLy[i] = a[i*2];
            cablePosRy[i] = a[i*2+(2*NUM_OF_WINGS)];
        }
        if(a.size() >=31){
            LEDStyle =(int)a[26]/100;
            LEDParameter0 =a[27]/10;
            LEDParameter1 =a[28]/10;
            LEDParameter2 =a[29];
            LEDParameter3 =a[30];
        }
    }
    //================== Simulation ==================
    wing.update();
    
    /*
     //================== Video ==================
     ffMovie.setPosition(timelinePlayer.getCurrentTime()/(ffMovie.getDuration()*1000));
     ffMovie.update();
     
     */
}

void ofApp::guiDraw(){
    
    //buttons
    
    if(serialTrigger){
        /*  if(EEPROM_saveBtn){
         
         string toWrite = "";
         
         for(int i=0; i< EEPROM.size(); i++)
         {
         toWrite+= ofToString(EEPROM[i]);
         if(i!=EEPROM.size()-1){
         toWrite+= "-";
         }
         }
         ofLog() << "toWrite : "<< toWrite ;
         serialWrite(currentArduinoID, toWrite);
         serialWrite(currentArduinoID, "S");
         
         currentdisplayLog = ofToString(currentArduinoID) +" EEPROM SAVED";
         serialTrigger = false;
         prevSerialTriggerMillis = currMillis;
         
         
         }
         if(EEPROM_loadBtn){
         
         }
         if(reset_Btn){
         serialWrite(currentArduinoID, "Q");
         }
         */
        if(home_Btn){
            if(all_Tog){
                serialWrite(-1, "H");
            }else{
                serialWrite(currentArduinoID, "H");
            }
            //manual home "G"
            //serialWrite(currentArduinoID, 1-1-1-1);
            //serialWrite(currentArduinoID, "G");
        }
        
        if(style_Btn_all_same){
            writeStyleMode(0);//all same = 0, all diff = 1, specific = 2
            
        }
        
        if(style_Btn_all){
            writeStyleMode(1);//all same = 0, all diff = 1, specific = 2
        }
        
        if(style_Btn){
            writeStyleMode(2);//all same = 0, all diff = 1, specific = 2
            
        }
        
    }
    if(currMillis -  prevSerialTriggerMillis > 200){
        serialTrigger = true;
    }
    
    //draw
    
    
    //if(drawDebugGui){
    guiDebug.draw();
    guiDebug2.draw();
    // }
    
    guiCablePosLy.draw();
    guiCablePosRy.draw();
    
    guiCableDurLy.draw();
    guiCableDurRy.draw();
    
    //----style 5---
    guiCablePosLy2.draw();
    guiCablePosRy2.draw();
    
    guiCableDurLy2.draw();
    guiCableDurRy2.draw();
    
    //--
    
    
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
        ofBackground(100, 0, 150);
        
        ofSetColor(255);
        
        //================== Debug Gui ==================
        
        std::stringstream ss;
        ss << "debugMode : "<< debugMode << endl;
        ss << "FrameRate : "<< ofGetFrameRate() << endl;
        ss << "Page : "<< page << endl;
        
        ss << "Num of Connected Devices: " << arduino.size() << " / " << NUM_OF_WINGS << endl;
        //ss << "Style: " << currentStyle << endl;
        
        ofDrawBitmapString(ss.str(), ofVec2f(20, 20));
        
        
        for(int i=0; i < NUM_OF_WINGS; i++){
            std::stringstream ss2;
            // if(isArduinoConnected[i]){
            if(isArduinoConnectedBySerial[i]){
                ofSetColor(0,updateColor[i],updateColor[i]);
#ifdef USEOSC
                ss2 << "Connected Devices (" << i+1 << ") : " << prevReceivedString[i] << endl;
                
#else
                ss2 << "Connected Devices (" << i+1 << ") : " << arduino[i].getPortName() << " : " << prevReceivedString[i] << endl;
#endif
            }else{
                ofSetColor(0,0,0);
                ss2 << "Connected Devices (" << i+1 << ") : " << "Disconnected" << endl;
            }
            ofDrawBitmapString(ss2.str(), ofVec2f(20, 25*i + 100));
            
            
        }
        
        
        
        for(int i=0; i< NUM_OF_WINGS; i++){
            
            wing.setRotate(i*2,ofMap(cablePosLy[i],0,MAX_Y_POS,30,-30));
            wing.setRotate(i*2+1,ofMap(cablePosRy[i],0,MAX_Y_POS,30,-30));
            
        }
        
        if(page == 0){
            //======== GUI =========
            guiDraw();
            //================== Simulation ==================
            
            wing.setMouseControllable(true);
            wing.draw(0,0,800,800);
            
        }else{
            
            //======== GUI =========
            guiDraw();
            //================== Timeline Player ==================
            timelinePlayer.draw();
            
            //================== Simulation ==================
            //wing.setRotate(1,mouseX);
            wing.setMouseControllable(false);
            wing.draw(400,-100,400,400);
            
            /*  //================== Video ==================
             ofSetColor(255);
             ffMovie.draw(400,0,320,180);
             */
            
        }
    }
}

//--------------------------------------------------------------
//--------------------------GUI EVENT -----------------------------
//--------------------------------------------------------------
void ofApp::loadButtonPressed(){
    serialWrite(currentArduinoID, "L");
    serialTrigger = false;
}

//--------------------------------------------------------------
//-----------------SERIAL COMMUNICATION ------------------------
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
    
    
    //=== Control ROOM ===
    ctrlrmReceivedString = "";
    showDoneMillis = currMillis;
    showDoneDur = 1000;
    
    
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
            string portDesc = devicesInfo[i].getPort();
            
            ofLog() << "devicesInfo[i].getPort() : " << devicesInfo[i].getPort();
            ofLog() << "devicesInfo[i].getHardwareId() : " << devicesInfo[i].getHardwareId();
            ofLog() << "devicesInfo[i].getDescription() : " << devicesInfo[i].getDescription();
            
            if(portDesc.find("FTA6PDIA") != std::string::npos)
            {
                
                
                
                // Connect to the first matching device.
                ofx::IO::BufferedSerialDevice device;
                
                // ctrlrm.push_back(device);
                
                bool success = ctrlrm.setup(devicesInfo[i], BAUD_CTRLRM);
                
                if(success)
                {
                    connectionStatus[a] = true;
                    ctrlrm.unregisterAllEvents(this);
                    ctrlrm.registerAllEvents(this);
                    
                    ofLogNotice("ofApp::setup") << "Control Room Signal Successfully setup " << devicesInfo[i];
                    a++;
                    ofSleepMillis(100);
                }
                else
                {
                    ofLogNotice("ofApp::setup") << "Control Room Signal Unable to setup " << devicesInfo[i];
                }
                
                
                
            }
            
        }
    }
    else
    {
        ofLogNotice("ofApp::setup") << "No devices connected.";
    }
    
    ofSleepMillis(1000); //Keep it - Arduino Mega needs time to initiate (Autoreset issue)
    
    
    
    
    //=== Control ROOM END ===
    
    
#ifdef USEOSC
    
#ifdef RECEIVER_IS_WINDOWS
    //USING 32 port and Windows, To connect arduino in sequence, we have to do it one by one
    
    vector<int> allComPort;
    
    for(int i=0; i<NUM_OF_SERIAL_TO_INIT; i++){
        allComPort.push_back(i);
    }
    arduino.push_back(allComPort[22]); //1
    arduino.push_back(allComPort[23]); //2
    arduino.push_back(allComPort[24]); //3
    arduino.push_back(allComPort[25]); //4
    arduino.push_back(allComPort[26]); //5
    arduino.push_back(allComPort[27]); //6
    arduino.push_back(allComPort[28]); //7
    arduino.push_back(allComPort[29]); //8
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
        
        arduino.push_back(i);
        isArduinoConnected.push_back(true);
        
    }
    
    
#endif
    
#ifdef COMPORT_SWAPPED
    int tmpArduino;
    
    tmpArduino = arduino[4];
    arduino[4] = arduino[3];
    arduino[3] = tmpArduino;
    //isArduinoConnected.push_back(true);
    
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
            
            if(portDesc.find("USB") != std::string::npos || portDesc.find("Arduino") != std::string::npos || portDesc.find("FTDI") != std::string::npos)
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
        // }else if (arduinoID >= 0 && working_cable[arduinoID] && arduinoID<arduino.size()){
    }else if (arduinoID >= 0 && arduinoID<arduino.size()){
        //  }else if (arduinoID >= 0 && arduinoID<NUM_OF_SERIAL_TO_INIT){
        
        
        sendOSC(arduino[arduinoID], sw);
        ofLog() << "WING sendOSC(arduinoID, sw);" << arduinoID;
        
    }else{
        sendOSC(arduinoID, sw);
        ofLog() << "FAN sendOSC(arduinoID, sw);" << arduinoID;
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

string ofApp::serialReadCtrlrm(){
    
    string combinedStr = "";
    // for(int i=0; i< arduino.size(); i++){
    
    // The serial device can throw exeptions.
    
    try
    {
        // Read all bytes from the device;
        uint8_t buffer[1024];
        vector<uint8_t> finalBuffer;
        finalBuffer.clear();
        while (ctrlrm.available() > 0)
        {
            std::size_t sz = ctrlrm.readBytes(buffer, 1024);
            // ofLog() << "CTRL buffer size: " << sz;
            for (std::size_t j = 0; j < sz; ++j)
            {
                std::cout << buffer[j];
                //  ofLog() << "CTRL buf: " << buffer[j];
                if(buffer[j] == '1' || buffer[j] == '0'){
                    finalBuffer.push_back(buffer[j]);
                }
                /*   if(isalnum(buffer[j]) || buffer[j] == '|' || buffer[j] == '-' ){
                 finalBuffer.push_back(buffer[j]);
                 }
                 */
                
            }
            for(int i = 0; i< finalBuffer.size();i++){
                //  ofLog() << "CTRL New Buf : " << finalBuffer[i];
                //  ofLog() << "CTRL New Buf SIZE : " << finalBuffer.size();
                combinedStr += ofToString(finalBuffer[i]);
            }
        }
        
    }
    catch (const std::exception& exc)
    {
        ofLogError("ofApp::update") << exc.what();
    }
    //  }
    
    return combinedStr;
    
    
    
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
        //currentdisplayLog = ofToString(currentArduinoID) +" EEPROM LOADED";
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
    switch (key){
            
        case '1':
            isExhibitionMode = !isExhibitionMode;
            timelinePlayer.exhibitionMode(isExhibitionMode);
            break;
        case '4':
            currentStyle = 4;
            break;
        case '5':
            currentStyle = 5;
            break;
            /*
             
             case 's':
             currentStyle++;
             if(currentStyle >NUM_OF_CABLES){ //todo
             currentStyle=0;
             }
             break;
             
             case 'p': //dialog for serial writing
             commandPrompt();
             break;
             */
            
        case 'a':
            ofLog() << "Hello";
#ifdef USEOSC
            sendOSC(currentArduinoID, "Testing");
#else
#endif
            break;
        case 'l':
            writeLEDStyle(tempLEDStyle++, 0);
            if(tempLEDStyle > 8){
                tempLEDStyle = 0;
            }
            break;
            
            
        case 'p':
            isShowBegin(true);
            break;
            
        case 'o':
            isShowBegin(false);
        
            break;
            
        case 'd':
            debugMode = !debugMode;
            break;
            
        case 'c': //check if arduino online
            serialWrite(-1, "C");
            break;
            
        case ' ': //emergency Stop
            serialWrite(-1, "E");
            isEmergencyStop = true;
            break;
            
        case 'r': //emergency Stop release
            serialWrite(-1, "R");
            isEmergencyStop = false;
            break;
            
        case 'q': //reset Arduino
            if(isEmergencyStop){
                serialWrite(-1, "Q");
                isEmergencyStop = false;
            }
            break;
            
        case 358: //rightArrow;
            page++;
            if(page >= numOfPages ){
                page = 0;
            }
            ofLog() << "page " << page;
            break;
            
        case 356: //leftArrow;
            page--;
            if(page < 0 ){
                page = numOfPages-1;
            }
            ofLog() << "page " << page;
            break;
            
        case 'j':
            //save style5 xml
            saveStyle5();
            break;
            
        case 'k':
            // load style5 xml
            loadStyle5();
            
            break;
            
        case '.': //fan
            
            serialWrite(FAN_SERIAL, "o");
            break;
        case ',': //fan
            
            serialWrite(FAN_SERIAL, "f");
            break;
        default:
            break;
            
    }
}

// =========== Style ================
void ofApp::writeLEDStyle(int s, int ss){
    //ofLog() << "LED Style " << s  << " current arduino " << currentArduinoID;
    // for(int i=0; i< NUM_OF_WINGS; i++){
    string writeInTotal = "LED : ";
    
    string toWrite = "";
    
    
    toWrite+= ofToString((int)s+21);
    toWrite+= "-";
    toWrite+= ofToString((int)ss);
    toWrite+= "-";
    
    writeInTotal=toWrite;
    
    serialWrite(-1, toWrite);
    //  }
}

void ofApp::writeStyleMode(int s){
    ofLog() << "write Style " << s  << " current arduino " << currentArduinoID;
    
    if (s == 0){
        
        
        if(currentStyle == 5){
            
            //STYLE - POS_1 - TIME_1 - POS_2 - TIME_2 - - POS_1 - TIME_1 - POS_2 - TIME_2 - LED_STYLE - VAL1 - VAL2 - VAL3 - VAL4
            
            
            string writeInTotal = " : ";
            string toWrite = "";
            
            toWrite+= ofToString(currentStyle);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cablePosLy[currentArduinoID]);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cableDurLy[currentArduinoID]*100);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cablePosLy2[currentArduinoID]);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cableDurLy2[currentArduinoID]*100);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cablePosRy[currentArduinoID]);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cableDurRy[currentArduinoID]*100);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cablePosRy2[currentArduinoID]);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cableDurRy2[currentArduinoID]*100);
            toWrite+= "-";
            
            //LED
            
            toWrite+= ofToString((int)LEDStyle);
            toWrite+= "-";
            
            
            writeInTotal=toWrite;
            
            serialWrite(-1, toWrite);
            // currentdisplayLog = writeInTotal;
            
            
        }
        
        if(currentStyle == 4){
            
            //STYLE - POS - TIME - POS - TIME - LED_STYLE - VAL1 - VAL2 - VAL3 - VAL4
            
            string writeInTotal = " : ";
            string toWrite = "";
            
            toWrite+= ofToString(currentStyle);
            toWrite+= "-";
            
            // if(currentMotor == 0){
            toWrite+= ofToString((int)cablePosLy[currentArduinoID]);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cableDurLy[currentArduinoID]*100);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cablePosRy[currentArduinoID]);
            toWrite+= "-";
            
            //  }else{
            toWrite+= ofToString((int)cableDurRy[currentArduinoID]*100);
            toWrite+= "-";
            
            
            //   }
            
            //LED
            
            toWrite+= ofToString((int)LEDStyle);
            toWrite+= "-";
            toWrite+= ofToString((int)LEDParameter0);
            toWrite+= "-";
            toWrite+= ofToString((int)LEDParameter1);
            toWrite+= "-";
            toWrite+= ofToString((int)LEDParameter2);
            toWrite+= "-";
            toWrite+= ofToString((int)LEDParameter3);
            toWrite+= "-";
            
            
            writeInTotal=toWrite;
            
            serialWrite(-1, toWrite);
            
        }
        
        
        if(currentStyle == 3){
            
            string writeInTotal = "LY : ";
            
            string toWrite = "";
            
            
            toWrite+= ofToString((int)cableSpeedLy[currentArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelLy[currentArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLy[currentArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RY: ";;
            
            toWrite+= ofToString((int)cableSpeedRy[currentArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelRy[currentArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRy[currentArduinoID]);
            
            writeInTotal=toWrite;
            
            serialWrite(-1, toWrite);
            // currentdisplayLog = writeInTotal;
            
            //MovementController
            
            //MovementController.getPoints();
            
        }
        
        if(currentStyle == 2){
            /*      string writeInTotal = "SPEED - ACCEL - : ";
             
             string toWrite = "";
             
             toWrite+= ofToString(currentStyle);
             toWrite+= "-";
             
             toWrite+= ofToString((int)cableSpeedX);
             toWrite+= "-";
             toWrite+= ofToString((int)cableAccelX);
             toWrite+= "-";
             toWrite+= ofToString((int)cableSpeedY);
             toWrite+= "-";
             toWrite+= ofToString((int)cableAccelY);
             toWrite+= "-";
             
             writeInTotal=toWrite + " LX:";
             
             toWrite+= ofToString((int)cablePosLx[currentArduinoID]);
             toWrite+= "-";
             toWrite+= ofToString((int)cablePosLx2[currentArduinoID]);
             toWrite+= "-";
             
             writeInTotal=toWrite + " LY:";
             
             toWrite+= ofToString((int)cablePosLy[currentArduinoID]);
             toWrite+= "-";
             toWrite+= ofToString((int)cablePosLy2[currentArduinoID]);
             toWrite+= "-";
             
             writeInTotal=toWrite +" RX: ";
             
             toWrite+= ofToString((int)cablePosRx[currentArduinoID]);
             toWrite+= "-";
             toWrite+= ofToString((int)cablePosRx2[currentArduinoID]);
             toWrite+= "-";
             
             writeInTotal=toWrite +" RY: ";;
             
             toWrite+= ofToString((int)cablePosRy[currentArduinoID]);
             toWrite+= "-";
             toWrite+= ofToString((int)cablePosRy2[currentArduinoID]);
             
             writeInTotal=toWrite;
             
             serialWrite(-1, toWrite);
             currentdisplayLog = writeInTotal;
             */
        }
        
    }
    else if (s == 1){
        for(int i=0; i< NUM_OF_WINGS; i++){
            
            if(currentStyle == 5){
                
                //STYLE - POS_1 - TIME_1 - POS_2 - TIME_2 - - POS_1 - TIME_1 - POS_2 - TIME_2 - LED_STYLE - VAL1 - VAL2 - VAL3 - VAL4
                
                
                string writeInTotal = " : ";
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosLy[i]);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableDurLy[i]*100);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosLy2[i]);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableDurLy2[i]*100);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosRy[i]);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableDurRy[i]*100);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosRy2[i]);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableDurRy2[i]*100);
                toWrite+= "-";
                
                //LED
                
                toWrite+= ofToString((int)LEDStyle);
                toWrite+= "-";
                
                
                writeInTotal=toWrite;
                
                serialWrite(i, toWrite);
                // currentdisplayLog = writeInTotal;
                
                
            }
            
            
            
            
            
            if(currentStyle == 4){
                
                //STYLE - POS - TIME - POS - TIME - LED_STYLE - VAL1 - VAL2 - VAL3 - VAL4
                
                string writeInTotal = " : ";
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
                toWrite+= "-";
                
                // if(currentMotor == 0){
                toWrite+= ofToString((int)cablePosLy[i]);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableDurLy[i]*100);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosRy[i]);
                toWrite+= "-";
                
                //  }else{
                toWrite+= ofToString((int)cableDurRy[i]*100);
                toWrite+= "-";
                
                
                //   }
                
                //LED
                
                toWrite+= ofToString((int)LEDStyle);
                toWrite+= "-";
                toWrite+= ofToString((int)LEDParameter0);
                toWrite+= "-";
                toWrite+= ofToString((int)LEDParameter1);
                toWrite+= "-";
                toWrite+= ofToString((int)LEDParameter2);
                toWrite+= "-";
                toWrite+= ofToString((int)LEDParameter3);
                toWrite+= "-";
                
                
                writeInTotal=toWrite;
                
                serialWrite(i, toWrite);
                
            }
            
            
            
            if(currentStyle == 3){
                
                string writeInTotal = "LY : ";
                
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableSpeedLy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelLy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLy[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RY: ";;
                
                toWrite+= ofToString((int)cableSpeedRy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelRy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRy[i]);
                
                writeInTotal=toWrite;
                
                serialWrite(i, toWrite);
                //  currentdisplayLog = writeInTotal;
                
                //MovementController
                
                //MovementController.getPoints();
                
            }
            
            if(currentStyle == 2){
                
                /*
                 string writeInTotal = "SPEED - ACCEL - : ";
                 
                 string toWrite = "";
                 
                 toWrite+= ofToString(currentStyle);
                 toWrite+= "-";
                 
                 toWrite+= ofToString((int)cableSpeedX);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cableAccelX);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cableSpeedY);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cableAccelY);
                 toWrite+= "-";
                 
                 writeInTotal=toWrite + " LX:";
                 
                 toWrite+= ofToString((int)cablePosLx[i]);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cablePosLx2[i]);
                 toWrite+= "-";
                 
                 writeInTotal=toWrite + " LY:";
                 
                 toWrite+= ofToString((int)cablePosLy[i]);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cablePosLy2[i]);
                 toWrite+= "-";
                 
                 writeInTotal=toWrite +" RX: ";
                 
                 toWrite+= ofToString((int)cablePosRx[i]);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cablePosRx2[i]);
                 toWrite+= "-";
                 
                 writeInTotal=toWrite +" RY: ";;
                 
                 toWrite+= ofToString((int)cablePosRy[i]);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cablePosRy2[i]);
                 
                 writeInTotal=toWrite;
                 
                 serialWrite(i, toWrite);
                 currentdisplayLog = writeInTotal;
                 */
            }
        }
        
    }else if (s == 2){
        if(currentArduinoID <= NUM_OF_WINGS-1 && currentArduinoID >= 0){
            
            
            if(currentStyle == 5){
                
                //STYLE - POS_1 - TIME_1 - POS_2 - TIME_2 - - POS_1 - TIME_1 - POS_2 - TIME_2 - LED_STYLE - VAL1 - VAL2 - VAL3 - VAL4
                
                
                string writeInTotal = " : ";
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosLy[currentArduinoID]);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableDurLy[currentArduinoID]*100);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosLy2[currentArduinoID]);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableDurLy2[currentArduinoID]*100);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosRy[currentArduinoID]);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableDurRy[currentArduinoID]*100);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosRy2[currentArduinoID]);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableDurRy2[currentArduinoID]*100);
                toWrite+= "-";
                
                //LED
                
                toWrite+= ofToString((int)LEDStyle);
                toWrite+= "-";
                
                
                writeInTotal=toWrite;
                
                serialWrite(currentArduinoID, toWrite);
                // currentdisplayLog = writeInTotal;
                
                
            }
            
            
            
            
            if(currentStyle == 4){
                
                //STYLE - POS - TIME - POS - TIME - LED_STYLE - VAL1 - VAL2 - VAL3 - VAL4
                
                string writeInTotal = " : ";
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
                toWrite+= "-";
                
                // if(currentMotor == 0){
                toWrite+= ofToString((int)cablePosLy[currentArduinoID]);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableDurLy[currentArduinoID]*100);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cablePosRy[currentArduinoID]);
                toWrite+= "-";
                
                //  }else{
                toWrite+= ofToString((int)cableDurRy[currentArduinoID]*100);
                toWrite+= "-";
                
                
                //   }
                
                //LED
                
                toWrite+= ofToString((int)LEDStyle);
                toWrite+= "-";
                toWrite+= ofToString((int)LEDParameter0);
                toWrite+= "-";
                toWrite+= ofToString((int)LEDParameter1);
                toWrite+= "-";
                toWrite+= ofToString((int)LEDParameter2);
                toWrite+= "-";
                toWrite+= ofToString((int)LEDParameter3);
                toWrite+= "-";
                
                
                writeInTotal=toWrite;
                
                serialWrite(currentArduinoID, toWrite);
                
            }
            
            if(currentStyle == 3){
                
                string writeInTotal = "LY : ";
                
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableSpeedLy[currentArduinoID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelLy[currentArduinoID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLy[currentArduinoID]);
                toWrite+= "-";
                
                
                writeInTotal=toWrite +" RY: ";;
                
                toWrite+= ofToString((int)cableSpeedRy[currentArduinoID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelRy[currentArduinoID]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRy[currentArduinoID]);
                
                writeInTotal=toWrite;
                
                serialWrite(currentArduinoID, toWrite);
                // currentdisplayLog = writeInTotal;
                
                //MovementController
                
                //MovementController.getPoints();
                
            }
            
            if(currentStyle == 2){
                
                /*       string writeInTotal = "SPEED - ACCEL - : ";
                 
                 string toWrite = "";
                 
                 toWrite+= ofToString(currentStyle);
                 toWrite+= "-";
                 
                 toWrite+= ofToString((int)cableSpeedX);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cableAccelX);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cableSpeedY);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cableAccelY);
                 toWrite+= "-";
                 
                 writeInTotal=toWrite + " LX:";
                 
                 toWrite+= ofToString((int)cablePosLx[currentArduinoID]);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cablePosLx2[currentArduinoID]);
                 toWrite+= "-";
                 
                 writeInTotal=toWrite + " LY:";
                 
                 toWrite+= ofToString((int)cablePosLy[currentArduinoID]);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cablePosLy2[currentArduinoID]);
                 toWrite+= "-";
                 
                 writeInTotal=toWrite +" RX: ";
                 
                 toWrite+= ofToString((int)cablePosRx[currentArduinoID]);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cablePosRx2[currentArduinoID]);
                 toWrite+= "-";
                 
                 writeInTotal=toWrite +" RY: ";;
                 
                 toWrite+= ofToString((int)cablePosRy[currentArduinoID]);
                 toWrite+= "-";
                 toWrite+= ofToString((int)cablePosRy2[currentArduinoID]);
                 
                 writeInTotal=toWrite;
                 
                 serialWrite(currentArduinoID, toWrite);
                 currentdisplayLog = writeInTotal;
                 
                 */
            }
        }
    }
    
}



//--------------------------------------------------------------
//-------------------------- OSC -------------------------------
//--------------------------------------------------------------
#ifdef USEOSC

//--------------------------------------------------------------
void ofApp::sendOSC(int ar, string s){
    ofxOscMessage m;
    m.setAddress("/serial/");
    m.addIntArg(ar);
    m.addStringArg(s);
    sender.sendMessage(m, false);
    // ofLog() << "sending OSC : " << ar << " : " << s;
}

vector<string> ofApp::readOSC(){
    
    vector<string> read;
    
    for(int i=0; i< NUM_OF_SERIAL_TO_INIT; i++){
        read.push_back("");
    }
    
    int currentArduinoID = -1;
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        // check for mouse moved message
        if(m.getAddress() == "/serial/in"){
            // the single argument is a string
            string mouseButtonState = m.getArgAsString(0);
            ofLog() << mouseButtonState;
            
            receivedString[0] = mouseButtonState;
            
        }
        // check for an image being sent (note: the size of the image depends greatly on your network buffer sizes - if an image is too big the message won't come through )
        else{
            // unrecognized message: display on the bottom of the screen
            string msg_string;
            string getString = "";
            msg_string = m.getAddress();
            msg_string += ": ";
            for(int i = 0; i < m.getNumArgs(); i++){
                // get the argument type
                msg_string += m.getArgTypeName(i);
                msg_string += ":";
                // display the argument - make sure we get the right type
                if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                    //msg_string += ofToString(m.getArgAsInt32(i));
                    currentArduinoID = m.getArgAsInt32(i);
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                    msg_string += ofToString(m.getArgAsFloat(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                    msg_string += m.getArgAsString(i);
                    getString+= m.getArgAsString(i);
                }
                else{
                    msg_string += "unknown";
                }
                
            }
            //  ofLog() << " currentArduinoID:  "<< currentArduinoID << " String: " << getString;
            for(int i=0; i< NUM_OF_SERIAL_TO_INIT; i++){
                if(arduino[i] == currentArduinoID){
                    read[i] = getString;
                }
            }
            
            
        }
        
    }
    return read;
    
}
#else
#endif

//================Other Functions ====================

bool ofApp::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    timelinePlayer.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    timelinePlayer.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    timelinePlayer.mouseReleased(x, y, button);
}
//================ Show Control ====================
//--------------------------------------------------------------
void ofApp::isShowBegin(bool sb){
    if(sb){ //begin
        prevShowBeginMillis = currMillis;
        showBeginTrigger = true;
        page = 1;
        currentStyle = 4;
    }
    else{ //reset
        timelinePlayer.stopButtonPressed();
        serialWrite(-1, "Q"); //Reset and Home All
    }
    
}

//==================== Style 5 ======================
//--------------------------------------------------------------
void ofApp::saveStyle5(){
    
    if(currentStyle == 5){
        guiCablePosLy.saveToFile("guiCablePosLy.xml");
        guiCablePosRy.saveToFile("guiCablePosRy.xml");
        
        guiCableDurLy.saveToFile("guiCableDurLy.xml");
        guiCableDurRy.saveToFile("guiCableDurRy.xml");
        
        
        guiCablePosLy2.saveToFile("guiCablePosLy2.xml");
        guiCablePosRy2.saveToFile("guiCablePosRy2.xml");
        
        guiCableDurLy2.saveToFile("guiCableDurLy2.xml");
        guiCableDurRy2.saveToFile("guiCableDurRy2.xml");
    }
    
}

//--------------------------------------------------------------
void ofApp::loadStyle5(){
    if(currentStyle == 5){
        guiCablePosLy.loadFromFile("guiCablePosLy.xml");
        guiCablePosRy.loadFromFile("guiCablePosRy.xml");
        
        guiCableDurLy.loadFromFile("guiCableDurLy.xml");
        guiCableDurRy.loadFromFile("guiCableDurRy.xml");
        
        guiCablePosLy2.loadFromFile("guiCablePosLy2.xml");
        guiCablePosRy2.loadFromFile("guiCablePosRy2.xml");
        
        guiCableDurLy2.loadFromFile("guiCableDurLy2.xml");
        guiCableDurRy2.loadFromFile("guiCableDurRy2.xml");
    }
    
}


void ofApp::style3(){
    
}

void ofApp::style4(){
    
}

void ofApp::style5(){
    
}



//==================== Unused ======================

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
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
