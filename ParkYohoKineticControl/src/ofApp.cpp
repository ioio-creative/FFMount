#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(25);
    
    //================== Serial ==================

    isArduinoConnected = serialSetup();
    
    SERIAL_PARAMETERES = {"sa", "lo","on", "allhm"}; //save, load, online
    serialTrigger = true;
    prevSerialTriggerMillis = ofGetElapsedTimeMillis();
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        isArduinoConnectedBySerial.push_back(false);
    }
    initOnUpdate = true;
    //================== debugMode ==================
    
    page = 0;
    numOfPages = 7;
    debugMode = true;
    isEmergencyStop = false;
    for(int i=0; i < NUM_OF_SERIAL_TO_INIT; i++){
        receivedString.push_back("");
        receivedStringBuffer.push_back("");
        prevReceivedString.push_back("");
        updateColor.push_back(255);
    }
    
    currentdisplayLog = "Park Yoho Control Software V1";
    
    guiSetup();
    
    
    //================== Movement Controls ==================
    
    int mcX = 10;
    int mcY = 200;
    int mcW = 500;
    int mcH = 200;
    int mcHg = 20;
    /*
     for(int i=0; i< 1 ; i++){
     MovementController mc;
     MovementControllers.push_back(mc);
     MovementControllers[i].setup(NUM_OF_CABLES,mcX,mcY+ (i*mcH)+ mcHg,mcW,mcH);
     }
     */
    MovementController.setup(NUM_OF_CABLES,mcX,mcY,mcW,mcH,MAX_X_POS,MAX_Y_POS);
    showMovementController = false;
    
    //================== Song 1 ==================
    currentSong = 0; //3 automa
    songStage = 0;

#ifdef USEOSC
    //================== OSC ==================
    // listen on the given port
    cout << "listening for osc messages on port " << R_PORT << "\n";
    receiver.setup(R_PORT);
    // open an outgoing connection to HOST:PORT
    sender.setup(S_HOST, S_PORT);
#else

#endif
}


//--------------------------------------------------------------
void ofApp::update(){
    
    currTime = ofGetElapsedTimeMillis();
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    
    currMillis = ofGetElapsedTimeMillis();
    
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
            
        }
    }
    
    //Online Cable Checking - TODO
    if(currMillis - prevOnlineCheckingMillis < 5000){
        num_of_online = 0;
        for (int i=0; i< NUM_OF_CABLES; i++){
            if(isArduinoConnectedBySerial[i]){
                num_of_online++;
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
    //================== Kinectic Visualisation ==================
    

    for(int i=0; i< NUM_OF_CABLES; i++){
        
        kinecticVisualisation.set(NUM_OF_CABLES ,i, currentStyle ,ofMap(cablePosLx[i],0,MAX_X_POS,0,2) ,ofMap(cablePosLy[i],0,MAX_Y_POS,0,1),ofMap(cablePosRx[i],0,MAX_X_POS,0,2), ofMap(cablePosRy[i],0,MAX_Y_POS,0,1));
        
    }
    
    
    kineticVisualizationFbo.begin();
    ofClear(255, 0, 0,0);
    kinecticVisualisation.draw();
    kineticVisualizationFbo.end();
    
    //================== Movement Controls ==================
    /*
     for(int i=0; i< MovementControllers.size(); i++){
     MovementControllers[i].update();
     }
     */
    MovementController.update();
    
    
    
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
    else if(debugMode){
        ofBackground(100, 0, 0);
        
        ofSetColor(255);
        
        if(drawKineticVisualizationFbo){
            kineticVisualizationFbo.draw(0,200,ofGetWidth()*0.7,ofGetHeight()*0.7);
        }
        
        //================== Debug Gui ==================
        
        std::stringstream ss;
        ss << "debugMode : "<< debugMode << endl;
        ss << "FrameRate : "<< ofGetFrameRate() << endl;
        ss << "Page : "<< page << endl;
        
        ss << "Num of Connected Devices: " << arduino.size() << " / " << NUM_OF_CABLES << endl;
        //ss << "Style: " << currentStyle << endl;
        
        ofDrawBitmapString(ss.str(), ofVec2f(20, 20));
        
        
        for(int i=0; i < NUM_OF_CABLES; i++){
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
        
        if(serialTrigger){
            if(EEPROM_saveBtn){
                
                string toWrite = "";
                
                for(int i=0; i< EEPROM.size(); i++)
                {
                    toWrite+= ofToString(EEPROM[i]);
                    if(i!=EEPROM.size()-1){
                        toWrite+= "-";
                    }
                }
                ofLog() << "toWrite : "<< toWrite ;
                serialWrite(currentDebugArduinoID, toWrite);
                serialWrite(currentDebugArduinoID, "S");
                
                currentdisplayLog = ofToString(currentDebugArduinoID) +" EEPROM SAVED";
                serialTrigger = false;
                prevSerialTriggerMillis =currMillis;
                
                
            }
            if(reset_Btn){
                serialWrite(currentDebugArduinoID, "Q");
            }
            
            if(home_Btn){
                if(all_Tog){
                    serialWrite(-1, "H");
                }else{
                serialWrite(currentDebugArduinoID, "H");
                }
                                //manual home "G"
                //serialWrite(currentDebugArduinoID, 1-1-1-1);
                //serialWrite(currentDebugArduinoID, "G");
            }
            if(EEPROM_loadBtn){
                
            }
            if(style_Btn_all_same){
                writeStyle(0);//all same = 0, all diff = 1, specific = 2
                
            }
            
            if(style_Btn_all){
                writeStyle(1);//all same = 0, all diff = 1, specific = 2
            }
            
            if(style_Btn){
                writeStyle(2);//all same = 0, all diff = 1, specific = 2
                
            }
            
        }
        if(currMillis -  prevSerialTriggerMillis > 200){
            serialTrigger = true;
        }
        
        
        string t = textField;
        if (t.find('+') != std::string::npos || t.find('=') != std::string::npos)
        {
            t.erase(std::remove(t.begin(), t.end(), '='), t.end());
            t.erase(std::remove(t.begin(), t.end(), '+'), t.end());
            currentdisplayLog = t;
            serialWrite(currentDebugArduinoID, t);
            textField = "";
        }
        
        if(drawDebugGui){
            guiDebug.draw();
            guiDebug2.draw();
        }
        
        guiCablePosLx.draw();
        guiCablePosLy.draw();
        guiCablePosRx.draw();
        guiCablePosRy.draw();
        guiCableAccelLx.draw();
        guiCableAccelLy.draw();
        guiCableAccelRx.draw();
        guiCableAccelRy.draw();
        guiCableSpeedLx.draw();
        guiCableSpeedLy.draw();
        guiCableSpeedRx.draw();
        guiCableSpeedRy.draw();
        
        displayLog(currentdisplayLog);
        
    }else{
        // ofBackground(100, 100, 100);
        
    }

    
    //================== Movement Controller ==================
    /*
     for(int i=0; i< MovementControllers.size(); i++){
     MovementControllers[i].draw();
     }
     */
    if(showMovementController){
        MovementController.draw();
    }
    if(showOffset){
        guiCablePosLxOffset.draw();
        guiCablePosLyOffset.draw();
        guiCablePosRxOffset.draw();
        guiCablePosRyOffset.draw();
        
    }
    
    
    song();
    
    setPoints();
    if(page == 0){
        drawDebugGui = true;
        drawKineticVisualizationFbo = true;
        showMovementController = false;
        showOffset = false;
    }else if(page == 1){
        drawDebugGui = true;
        drawKineticVisualizationFbo = false;
        showMovementController = false;
        showOffset = true;
    }else if(page == 2){
        drawDebugGui = false;
        drawKineticVisualizationFbo = false;
        showMovementController = false;
        showOffset = false;
    }else if(page == 3){
        drawDebugGui = true;
        drawKineticVisualizationFbo = true;
        showMovementController = true;
        showOffset = false;
    }else if(page == 4){
        
        drawDebugGui = true;
        drawKineticVisualizationFbo = true;
        showMovementController = true;
        showOffset = false;
        
        guiCablePosLx2.draw();
        guiCablePosLy2.draw();
        guiCablePosRx2.draw();
        guiCablePosRy2.draw();
        guiCableSpeedAccelAll.draw();
        
    }else{ //Page = 6 // ricci mode
        currentSong = 4;
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

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key){
        case 's':
            currentStyle++;
            if(currentStyle >NUM_OF_CABLES){ //todo
                currentStyle=0;
            }
            break;
            
        case 'a':
            ofLog() << "Hello";
#ifdef USEOSC
            sendOSC(currentDebugArduinoID, "Testing");
#else
#endif
            break;
            
        case 'd':
            debugMode = !debugMode;
            break;
            
        case 'c': //check if arduino online
            serialWrite(-1, "C");
            break;
            
        case 'p': //dialog for serial writing
            commandPrompt();
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
            
        case 358: //leftArrow;
            page++;
            if(page >= numOfPages ){
                page = 0;
            }
            break;
            
        case 356: //leftArrow;
            page--;
            if(page < 0 ){
                page = numOfPages-1;
            }
            break;
            
        case 'v': //Ricci Mode
            page = 6;
            break;
            
        default:
            break;
            
    }
    ofLog() << "page " << page;
}

//--------------------------------------------------------------
//-------------------------- Functions -------------------------
//--------------------------------------------------------------

bool ofApp::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void ofApp::commandPrompt(){
    string txt = ofSystemTextBoxDialog("Serial Command", txt);
    serialWrite(currentDebugArduinoID, txt);
    
}
//--------------------------------------------------------------
//-------------------------- GUI -------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
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
    
    guiDebug.add(currentDebugArduinoID.set("Cable No + 1",0,0,NUM_OF_CABLES-1));
    guiDebug.add(currentSong.set("Song",0,0,NUM_OF_CABLES));
    
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
    guiDebug.add(EEPROM_saveBtn.setup(EEPROM_saveLoad_names[0]));
    guiDebug.add(EEPROM_loadBtn.setup(EEPROM_saveLoad_names[1]));
    EEPROM_loadBtn.addListener(this, &ofApp::loadButtonPressed);
    guiDebug.add(textField.setup("Serial:", "0-0-0-0-0"));
    guiDebug.add(currentStyle.set("Style",3,0,NUM_OF_CABLES)); //TODO
    guiDebug.add(style_Btn.setup("Set Position:"));
    guiDebug.add(style_Btn_all_same.setup("Set Position ALL Same:"));
    guiDebug.add(style_Btn_all.setup("Set Position ALL:"));
    guiDebug.add(reset_Btn.setup("Reset:"));
    guiDebug.add(home_Btn.setup("Home: "));
    guiDebug.add(all_Tog.setup("ALL TOGGLE:",false));
    
    all_Tog = false;
    int numOfWaveForm = 4;
    for(int i=0; i< 8; i++){
        ofParameter<bool> a;
        a.set("input " + ofToString(i),false);
        ofParameter<bool> b;
        string c = "";
        if(i==0){
            c+= "Lx";
        }
        if(i==1){
            c+= "Ly";
        }
        if(i==2){
            c+= "Rx";
        }
        if(i==3){
            c+= "Ry";
        }
        if(i==4){
            c+= "Lx2";
        }
        if(i==5){
            c+= "Ly2";
        }
        if(i==6){
            c+= "Rx2";
        }
        if(i==7){
            c+= "Ry2";
        }
        
        b.set(c,false);
        input_pts.push_back(a);
        output_pts.push_back(b);
        guiDebug.add(input_pts[i]);
        guiDebug.add(output_pts[i]);
    }
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<bool> a;
      //  if(i == 10){
        //    a.set("input " + ofToString(i),false);
       // }else{
            a.set("input " + ofToString(i),true);
       // }
        working_cable.push_back(a);
        guiDebug2.add(working_cable[i]);
    }
    
    
   // guiDebug.add(all_Tog.setup("SET ALL:"));
    
    //textField.addListener(this,&ofApp::serialTextInput);
    //--- Cable Position Control ---
    
    
    parametersCablePos.setName("cablePosition");
    guiCablePosLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 0);
    guiCablePosLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 200, 400);
    guiCablePosRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 0);
    guiCablePosRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 100, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("P Lx" + ofToString(i),0,0,MAX_X_POS); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("P Ly" + ofToString(i),0,0,MAX_Y_POS);
        ofParameter<int> c;
        c.set("P Rx" + ofToString(i),0,0,MAX_X_POS);
        ofParameter<int> d;
        d.set("P Ry" + ofToString(i),0,0,MAX_Y_POS);
        cablePosLx.push_back(a);
        cablePosLy.push_back(b);
        cablePosRx.push_back(c);
        cablePosRy.push_back(d);
        guiCablePosLx.add(cablePosLx[i]);
        guiCablePosLy.add(cablePosLy[i]);
        guiCablePosRx.add(cablePosRx[i]);
        guiCablePosRy.add(cablePosRy[i]);
    }
    
    //--- Cable Position Offset Control ---
    showOffset = false;
    
    parametersCablePosOffset.setName("cablePositionOffset");
    guiCablePosLxOffset.setup("EEPROMReadWrite", "settings.xml",  100, 0);
    guiCablePosLyOffset.setup("EEPROMReadWrite", "settings.xml",  100, 400);
    guiCablePosRxOffset.setup("EEPROMReadWrite", "settings.xml",  200, 0);
    guiCablePosRyOffset.setup("EEPROMReadWrite", "settings.xml",  200, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("Offset Lx" + ofToString(i),0,0,MAX_X_POS*0.1); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("Offset Ly" + ofToString(i),0,0,MAX_Y_POS*0.1);
        ofParameter<int> c;
        c.set("Offset Rx" + ofToString(i),0,0,MAX_X_POS*0.1);
        ofParameter<int> d;
        d.set("Offset Ry" + ofToString(i),0,0,MAX_Y_POS*0.1);
        cablePosLxOffset.push_back(a);
        cablePosLyOffset.push_back(b);
        cablePosRxOffset.push_back(c);
        cablePosRyOffset.push_back(d);
        guiCablePosLxOffset.add(cablePosLxOffset[i]);
        guiCablePosLyOffset.add(cablePosLyOffset[i]);
        guiCablePosRxOffset.add(cablePosRxOffset[i]);
        guiCablePosRyOffset.add(cablePosRyOffset[i]);
    }
    
    
    
    
    
    //--- Cable Accel Control ---
    
    
    
    parametersCableAccel.setName("cableAccel");
    guiCableAccelLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 310, 0);
    guiCableAccelLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 310, 400);
    guiCableAccelRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 260, 0);
    guiCableAccelRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 260, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("A" + ofToString(i),50000,0,MAX_X_SPEED); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("A" + ofToString(i),50000,0,MAX_Y_SPEED);
        ofParameter<int> c;
        c.set("A" + ofToString(i),50000,0,MAX_X_SPEED);
        ofParameter<int> d;
        d.set("A" + ofToString(i),50000,0,MAX_Y_SPEED);
        cableAccelLx.push_back(a);
        cableAccelLy.push_back(b);
        cableAccelRx.push_back(c);
        cableAccelRy.push_back(d);
        guiCableAccelLx.add(cableAccelLx[i]);
        guiCableAccelLy.add(cableAccelLy[i]);
        guiCableAccelRx.add(cableAccelRx[i]);
        guiCableAccelRy.add(cableAccelRy[i]);
    }
    //--- Cable Speed Control ---
    
    parametersCableSpeed.setName("cableSpeed");
    guiCableSpeedLx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 420, 0);
    guiCableSpeedLy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 420, 400);
    guiCableSpeedRx.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 370, 0);
    guiCableSpeedRy.setup("EEPROMReadWrite", "settings.xml", ofGetWidth() - 370, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("S" + ofToString(i),50000,0,MAX_X_ACCEL); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("S" + ofToString(i),50000,0,MAX_Y_ACCEL);
        ofParameter<int> c;
        c.set("S" + ofToString(i),50000,0,MAX_X_ACCEL);
        ofParameter<int> d;
        d.set("S" + ofToString(i),50000,0,MAX_Y_ACCEL);
        cableSpeedLx.push_back(a);
        cableSpeedLy.push_back(b);
        cableSpeedRx.push_back(c);
        cableSpeedRy.push_back(d);
        guiCableSpeedLx.add(cableSpeedLx[i]);
        guiCableSpeedLy.add(cableSpeedLy[i]);
        guiCableSpeedRx.add(cableSpeedRx[i]);
        guiCableSpeedRy.add(cableSpeedRy[i]);
    }
    
    
    int guiPosCableW = 100;
    int guiCableH = 500;
    guiCablePosLx.setSize(guiPosCableW, guiCableH);
    guiCablePosLx.setWidthElements(guiPosCableW);
    guiCablePosLy.setSize(guiPosCableW, guiCableH);
    guiCablePosLy.setWidthElements(guiPosCableW);
    guiCablePosRx.setSize(guiPosCableW, guiCableH);
    guiCablePosRx.setWidthElements(guiPosCableW);
    guiCablePosRy.setSize(guiPosCableW, guiCableH);
    guiCablePosRy.setWidthElements(guiPosCableW);
    
    
    guiCablePosLxOffset.setSize(guiPosCableW, guiCableH);
    guiCablePosLxOffset.setWidthElements(guiPosCableW);
    guiCablePosLyOffset.setSize(guiPosCableW, guiCableH);
    guiCablePosLyOffset.setWidthElements(guiPosCableW);
    guiCablePosRxOffset.setSize(guiPosCableW, guiCableH);
    guiCablePosRxOffset.setWidthElements(guiPosCableW);
    guiCablePosRyOffset.setSize(guiPosCableW, guiCableH);
    guiCablePosRyOffset.setWidthElements(guiPosCableW);
    
    int guiCableW = 50;
    
    guiCableAccelLx.setSize(guiCableW, guiCableH);
    guiCableAccelLx.setWidthElements(guiCableW);
    guiCableAccelLy.setSize(guiCableW, guiCableH);
    guiCableAccelLy.setWidthElements(guiCableW);
    guiCableAccelRx.setSize(guiCableW, guiCableH);
    guiCableAccelRx.setWidthElements(guiCableW);
    guiCableAccelRy.setSize(guiCableW, guiCableH);
    guiCableAccelRy.setWidthElements(guiCableW);
    
    guiCableSpeedLx.setSize(guiCableW, guiCableH);
    guiCableSpeedLx.setWidthElements(guiCableW);
    guiCableSpeedLy.setSize(guiCableW, guiCableH);
    guiCableSpeedLy.setWidthElements(guiCableW);
    guiCableSpeedRx.setSize(guiCableW, guiCableH);
    guiCableSpeedRx.setWidthElements(guiCableW);
    guiCableSpeedRy.setSize(guiCableW, guiCableH);
    guiCableSpeedRy.setWidthElements(guiCableW);
    
    
    //ofFBO
    kineticVisualizationFbo.allocate(ofGetWidth(),ofGetHeight());
    
    
    //==== Style 2 =====
    
    guiCablePosLx2.setup("EEPROMReadWrite", "settings.xml", 100, 0);
    guiCablePosLy2.setup("EEPROMReadWrite", "settings.xml", 100, 400);
    guiCablePosRx2.setup("EEPROMReadWrite", "settings.xml", 200, 0);
    guiCablePosRy2.setup("EEPROMReadWrite", "settings.xml", 200, 400);
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofParameter<int> a;
        a.set("P2 Lx" + ofToString(i),1500,0,MAX_X_POS); //lx,ly,rx,ry
        ofParameter<int> b;
        b.set("P2 Ly" + ofToString(i),1500,0,MAX_Y_POS);
        ofParameter<int> c;
        c.set("P2 Rx" + ofToString(i),1500,0,MAX_X_POS);
        ofParameter<int> d;
        d.set("P2 Ry" + ofToString(i),1500,0,MAX_Y_POS);
        cablePosLx2.push_back(a);
        cablePosLy2.push_back(b);
        cablePosRx2.push_back(c);
        cablePosRy2.push_back(d);
        guiCablePosLx2.add(cablePosLx2[i]);
        guiCablePosLy2.add(cablePosLy2[i]);
        guiCablePosRx2.add(cablePosRx2[i]);
        guiCablePosRy2.add(cablePosRy2[i]);
    }
    
    guiCableSpeedAccelAll.setup("EEPROMReadWrite", "settings.xml", 400, 0);
    
    cableSpeedX.set("S2 X ",270,0,MAX_X_SPEED);
    cableAccelX.set("A2 X " ,140,0,MAX_X_ACCEL);
    
    cableSpeedY.set("S2 Y ",280,0,MAX_Y_SPEED);
    cableAccelY.set("A2 Y ",240,0,MAX_Y_ACCEL);
    
    guiCableSpeedAccelAll.add(cableSpeedX);
    guiCableSpeedAccelAll.add(cableAccelX);
    guiCableSpeedAccelAll.add(cableSpeedY);
    guiCableSpeedAccelAll.add(cableAccelY);
    
    guiCablePosLx2.setSize(guiPosCableW, guiCableH);
    guiCablePosLx2.setWidthElements(guiPosCableW);
    guiCablePosLy2.setSize(guiPosCableW, guiCableH);
    guiCablePosLy2.setWidthElements(guiPosCableW);
    guiCablePosRx2.setSize(guiPosCableW, guiCableH);
    guiCablePosRx2.setWidthElements(guiPosCableW);
    guiCablePosRy2.setSize(guiPosCableW, guiCableH);
    guiCablePosRy2.setWidthElements(guiPosCableW);
    
    guiCableSpeedAccelAll.setSize(guiPosCableW, guiCableH);
    guiCableSpeedAccelAll.setWidthElements(guiPosCableW);
    
}


void ofApp::displayLog(string s=""){
    if(s.size() >0){
        currentdisplayLog = s ;
    }
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("Status: " + currentdisplayLog, 10, ofGetHeight()-10);
}





//--------------------------------------------------------------
//--------------------------STYLE -----------------------------
//--------------------------------------------------------------

void ofApp::setPoints(){
    vector<ofPoint> mcPoints = MovementController.getPoints();
    
    for(int i=0; i < NUM_OF_CABLES; i++){
        for(int j=0; j < 8; j++){
            if(output_pts[j]){
                if(j==0){
                    cablePosLx[i] = mcPoints[i].y;
                    if(cablePosLx[i] >= MAX_X_POS){
                        cablePosLx[i] = MAX_X_POS;
                    }else if (cablePosLx[i]<= 0){
                        cablePosLx[i] = 0;
                    }
                }else if(j==1){
                    cablePosLy[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    
                    if(cablePosLy[i] >= MAX_Y_POS){
                        cablePosLy[i] = MAX_Y_POS;
                    }else if (cablePosLy[i]<= 0){
                        cablePosLy[i] = 0;
                    }
                }else if(j==2){
                    cablePosRx[i] = mcPoints[i].y;
                    
                    if(cablePosRx[i] >= MAX_X_POS){
                        cablePosRx[i] = MAX_X_POS;
                    }else if (cablePosRx[i]<= 0){
                        cablePosRx[i] = 0;
                    }
                }else if(j==3){
                    cablePosRy[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    
                    if(cablePosRy[i] >= MAX_Y_POS){
                        cablePosRy[i] = MAX_Y_POS;
                    }else if (cablePosRy[i]<= 0){
                        cablePosRy[i] = 0;
                    }
                }
                else  if(j==4){
                    cablePosLx2[i] = mcPoints[i].y;
                    if(cablePosLx2[i] >= MAX_X_POS){
                        cablePosLx2[i] = MAX_X_POS;
                    }else if (cablePosLx2[i]<= 0){
                        cablePosLx2[i] = 0;
                    }
                }else if(j==5){
                    cablePosLy2[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    
                    if(cablePosLy2[i] >= MAX_Y_POS){
                        cablePosLy2[i] = MAX_Y_POS;
                    }else if (cablePosLy2[i]<= 0){
                        cablePosLy2[i] = 0;
                    }
                }else if(j==6){
                    cablePosRx2[i] = mcPoints[i].y;
                    
                    if(cablePosRx2[i] >= MAX_X_POS){
                        cablePosRx2[i] = MAX_X_POS;
                    }else if (cablePosRx2[i]<= 0){
                        cablePosRx2[i] = 0;
                    }
                }else if(j==7){
                    cablePosRy2[i] = mcPoints[i].y*(MAX_Y_POS/MAX_X_POS);
                    
                    if(cablePosRy2[i] >= MAX_Y_POS){
                        cablePosRy2[i] = MAX_Y_POS;
                    }else if (cablePosRy2[i]<= 0){
                        cablePosRy2[i] = 0;
                    }
                    
                }
            }
        }
    }
    
}

//--------------------------------------------------------------

void ofApp::song(){

    if(prevSong != currentSong){
        currentDebugArduinoID =-1;
        songStage = 0;
        prevSong = currentSong;
        timeDiff = 0;
        ofLog() << "Song Reset";
    }
    if(currentSong ==1){
        //================== Song 1 ==================
        if(songStage == 0){ // all move at the same time
            if(currTime - prevTime >timeDiff){
                timeDiff = ofRandom(60000,90000);
                ofLog() << "timeDiff ; "<< timeDiff;
                prevTime = currTime;
                setPattern = true;
                
            }
            if(setPattern){
                
                int option = (int)ofRandom(4);
                if(option ==3){//TFTF , FTTF, TFTF, FTFT
                    output_pts[1] = true;
                    output_pts[3] = false;
                    
                    output_pts[0] = true;
                    output_pts[2] = false;
                }else if(option ==2)
                {
                    output_pts[1] = false;
                    output_pts[3] = true;
                    
                    output_pts[0] = true;
                    output_pts[2] = false;
                    
                }
                else if(option ==1){
                    output_pts[1] = true;
                    output_pts[3] = false;
                    
                    output_pts[0] = false;
                    output_pts[2] = true;
                }
                else {
                    output_pts[1] = false;
                    output_pts[3] = true;
                    
                    output_pts[0] = false;
                    output_pts[2] = true;
                }
                 MovementController.setPoints((int)ofRandom(30,90), ofRandom(35,37),(int)ofRandom(0,187),(int)ofRandom(0,1000));
                setPattern = false;
                ofLog() << "Set Pattern";
                
                writeStyle(1);
                
            }
            
        }
    }else if(currentSong ==2){ //one by one
        if(currTime - prevTime >timeDiff){
            
            ofLog() << "timeDiff ; "<< timeDiff;
            prevTime = currTime;
            setPattern = true;

            if(currentDebugArduinoID > NUM_OF_CABLES-1){
                timeDiff = ofRandom(60000,90000);
                ofLog() << "reach cable 19";
                 currentDebugArduinoID = -1;
            }else{
                timeDiff = 600;
            }
        }
        
        if(setPattern){
            if(songStage == 0){
                int option = (int)ofRandom(4);
                if(option ==3){//TFTF , FTTF, TFTF, FTFT
                    output_pts[1] = true;
                    output_pts[3] = false;
                    
                    output_pts[0] = true;
                    output_pts[2] = false;
                }else if(option ==2)
                {
                    output_pts[1] = false;
                    output_pts[3] = true;
                    
                    output_pts[0] = true;
                    output_pts[2] = false;
                    
                }
                else if(option ==1){
                    output_pts[1] = true;
                    output_pts[3] = false;
                    
                    output_pts[0] = false;
                    output_pts[2] = true;
                }
                else {
                    output_pts[1] = false;
                    output_pts[3] = true;
                    
                    output_pts[0] = false;
                    output_pts[2] = true;
                }
                MovementController.setPoints((int)ofRandom(30,90), ofRandom(35,37),(int)ofRandom(0,187),(int)ofRandom(0,1000));
                

                songStage++;
                
            }else if (songStage == 1){
            
                writeStyle(2);
                
                if(currentDebugArduinoID >= NUM_OF_CABLES ){
                    ofLog() << "STAGE END " << currentDebugArduinoID;
                    songStage++;
                }
                
                setPattern = false;
            }else if (songStage == 2){
                setPattern = false;
                songStage=0;
                currentDebugArduinoID = 0;
            }
            
        }
        
    }else if(currentSong == 3){ //one by one
        if(currTime - prevTime >timeDiff){
            
            //ofLog() << "timeDiff ; "<< timeDiff;
            prevTime = currTime;
            setPattern = true;

            if(currentDebugArduinoID >= NUM_OF_CABLES ){
                //timeDiff = ofRandom(60000,90000);
  
                ofLog() << "reach cable 19";
                currentDebugArduinoID = -1;
                
            }else{
                timeDiff = 600;
                currentDebugArduinoID++;
            }
             ofLog() << "songStage : " << songStage;


        }
        
        if(setPattern){
            if(songStage == 0){

                //set Y
                output_pts[0] = false;
                output_pts[2] = false;
                
                output_pts[1] = true;
                output_pts[3] = true;
                
                MovementController.setPoints((int)ofRandom(45,46), ofRandom(28,29),(int)ofRandom(1125,1126),(int)ofRandom(525,526));
                

                if(currentDebugArduinoID == NUM_OF_CABLES ){
                    songStage++;
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if(songStage == 1){
                
                //set X
                output_pts[0] = true;
                output_pts[2] = true;
                
                output_pts[1] = false;
                output_pts[3] = false;
                
                MovementController.setPoints((int)ofRandom(0,0), ofRandom(0,0),(int)ofRandom(68,69),(int)ofRandom(0,1000));
                
                if(currentDebugArduinoID == NUM_OF_CABLES ){
        
                    songStage++;
                    timeDiff = ofRandom(25000,40000);
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if(songStage == 2){
                
                //set Y
                output_pts[0] = false;
                output_pts[2] = false;
                
                output_pts[1] = true;
                output_pts[3] = true;
                
                MovementController.setPoints((int)ofRandom(45,46), ofRandom(28,29),(int)ofRandom(1125,1126),(int)ofRandom(525,526));
                
                if(currentDebugArduinoID == NUM_OF_CABLES ){
                    songStage++;
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if(songStage == 3){
                
                //set X
                output_pts[0] = true;
                output_pts[2] = true;
                
                output_pts[1] = false;
                output_pts[3] = false;
                
                MovementController.setPoints((int)ofRandom(50,51), ofRandom(25,26),(int)ofRandom(875,876),(int)ofRandom(50,51));
                
                if(currentDebugArduinoID == NUM_OF_CABLES ){
                    
                    songStage++;
                    timeDiff = ofRandom(25000,40000);
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if(songStage == 4){
                
                //set Y
                output_pts[0] = false;
                output_pts[2] = false;
                
                output_pts[1] = true;
                output_pts[3] = true;
                
                MovementController.setPoints((int)ofRandom(50,51), ofRandom(25,26),(int)ofRandom(875,876),(int)ofRandom(50,51));
                
                if(currentDebugArduinoID == NUM_OF_CABLES ){
                    songStage++;
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if(songStage == 5){
                
                //set X
                output_pts[0] = true;
                output_pts[2] = true;
                
                output_pts[1] = false;
                output_pts[3] = false;
                
                  MovementController.setPoints((int)ofRandom(45,46), ofRandom(28,29),(int)ofRandom(1125,1126),(int)ofRandom(525,526));
                
                if(currentDebugArduinoID == NUM_OF_CABLES ){
                    
                    songStage++;
                     timeDiff = ofRandom(25000,40000);
                }else{
                    writeStyle(2);
                }
                
                setPattern = false;
                
            }else if (songStage == 6){
                setPattern = false;
                songStage=0;
   
            }
        }
        
    }else if(currentSong == 4){ //ricci mode, debug one by one
        currentStyle = 3;
        
        output_pts[0] = false;
        output_pts[2] = false;
        
        output_pts[1] = false;
        output_pts[3] = false;
    }

    
    
}


void ofApp::writeStyle(int s){
    ofLog() << "write Style " << s  << " current arduino " << currentDebugArduinoID;

    if (s==0){
        
        if(currentStyle == 3){
            
            string writeInTotal = "LX : ";
            
            string toWrite = "";
            
            toWrite+= ofToString(currentStyle);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cableSpeedLx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelLx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLx[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite + " LY:";
            
            toWrite+= ofToString((int)cableSpeedLy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelLy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLy[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RX: ";
            
            toWrite+= ofToString((int)cableSpeedRx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelRx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRx[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RY: ";;
            
            toWrite+= ofToString((int)cableSpeedRy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelRy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRy[currentDebugArduinoID]);
            
            writeInTotal=toWrite;
            
            serialWrite(-1, toWrite);
            currentdisplayLog = writeInTotal;
            
            //MovementController
            
            //MovementController.getPoints();
            
        }
        
        if(currentStyle == 2){
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
            
            toWrite+= ofToString((int)cablePosLx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLx2[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite + " LY:";
            
            toWrite+= ofToString((int)cablePosLy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLy2[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RX: ";
            
            toWrite+= ofToString((int)cablePosRx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRx2[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RY: ";;
            
            toWrite+= ofToString((int)cablePosRy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRy2[currentDebugArduinoID]);
            
            writeInTotal=toWrite;
            
            serialWrite(-1, toWrite);
            currentdisplayLog = writeInTotal;
            
        }
        
    }
    else if (s ==1){
        for(int i=0; i< NUM_OF_CABLES; i++){
            if(currentStyle == 11){
                
                string writeInTotal = "LX : ";
                
                string toWrite = "";
                
                toWrite+= ofToString(currentStyle);
                toWrite+= "-";
                
                toWrite+= ofToString((int)cableSpeedLx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelLx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLx[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite + " LY:";
                
                toWrite+= ofToString((int)cableSpeedLy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelLy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosLy[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RX: ";
                
                toWrite+= ofToString((int)cableSpeedRx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelRx[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRx[i]);
                toWrite+= "-";
                
                writeInTotal=toWrite +" RY: ";;
                
                toWrite+= ofToString((int)cableSpeedRy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cableAccelRy[i]);
                toWrite+= "-";
                toWrite+= ofToString((int)cablePosRy[i]);
                
                writeInTotal=toWrite;
                
                serialWrite(i, toWrite);
                currentdisplayLog = writeInTotal;
                
                //MovementController
                
                //MovementController.getPoints();
                
            }
            
            if(currentStyle == 2){
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
                
            }
        }
        
    }else if (s ==2){
            if(currentDebugArduinoID <= NUM_OF_CABLES-1 && currentDebugArduinoID >= 0){
        if(currentStyle == 3){
            
            string writeInTotal = "LX : ";
            
            string toWrite = "";
            
            toWrite+= ofToString(currentStyle);
            toWrite+= "-";
            
            toWrite+= ofToString((int)cableSpeedLx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelLx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLx[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite + " LY:";
            
            toWrite+= ofToString((int)cableSpeedLy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelLy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLy[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RX: ";
            
            toWrite+= ofToString((int)cableSpeedRx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelRx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRx[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RY: ";;
            
            toWrite+= ofToString((int)cableSpeedRy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cableAccelRy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRy[currentDebugArduinoID]);
            
            writeInTotal=toWrite;
            
            serialWrite(currentDebugArduinoID, toWrite);
            currentdisplayLog = writeInTotal;
            
            //MovementController
            
            //MovementController.getPoints();
            
        }
        
        if(currentStyle == 2){

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
            
            toWrite+= ofToString((int)cablePosLx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLx2[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite + " LY:";
            
            toWrite+= ofToString((int)cablePosLy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosLy2[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RX: ";
            
            toWrite+= ofToString((int)cablePosRx[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRx2[currentDebugArduinoID]);
            toWrite+= "-";
            
            writeInTotal=toWrite +" RY: ";;
            
            toWrite+= ofToString((int)cablePosRy[currentDebugArduinoID]);
            toWrite+= "-";
            toWrite+= ofToString((int)cablePosRy2[currentDebugArduinoID]);
            
            writeInTotal=toWrite;
            
            serialWrite(currentDebugArduinoID, toWrite);
            currentdisplayLog = writeInTotal;
            
            
        }
            }
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
    

    
    for(int i=0; i< NUM_OF_CABLES; i++){
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
    
    
    for(int i=0; i< NUM_OF_CABLES; i++){
        ofLog() <<arduino[i];
        isArduinoConnected.push_back(true);
    }
#else
    
    for(int i=0; i< NUM_OF_CABLES; i++){
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
            
            if(portDesc.find("USB") != std::string::npos || portDesc.find("Arduino") != std::string::npos  || portDesc.find("FTDI") != std::string::npos)
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
          
          for(int i=0; i< NUM_OF_CABLES; i++){
              sendOSC(arduino[i], sw);
          }
      }else if (arduinoID >= 0 && working_cable[arduinoID] && arduinoID<arduino.size()){
          sendOSC(arduino[arduinoID], sw);
      }
    

#else
    
    if(arduinoID == -1){

        int arraySize = 0;
        if(arduino.size() <= NUM_OF_CABLES){
            arraySize = arduino.size();
        }else{
            arraySize = NUM_OF_CABLES;
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
        currentdisplayLog = ofToString(currentDebugArduinoID) +" EEPROM LOADED";
        return sToIntArray;
    }
    else{
        // vector<int> sToIntArray;
        return sToIntArray;
    }
    
}

//--------------------------------------------------------------
//---------------------- OTHER EVENTS --------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::exit()
{
    serialWrite(-1, "V");
#ifdef USEOSC
#else
    
    for(int i=0; i< arduino.size(); i++){
        arduino[i].unregisterAllEvents(this);
    }
#endif
}

//--------------------------------------------------------------
//--------------------------UNUSED -----------------------------
//--------------------------------------------------------------

//TODO: Unused for now

//--------------------------------------------------------------
void ofApp::removeSubstrs(string& s, string& p) {
    string::size_type n = p.length();
    for (string::size_type i = s.find(p);
         i != string::npos;
         i = s.find(p))
        s.erase(i, n);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
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

