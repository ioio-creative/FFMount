#include <Metro.h> // Include the Metro library

Metro serialMetro = Metro(4);  // Instantiate an instance
const int numReadings = 30;
int readIndex = 0;              // the index of the current reading
float total = 0;                  // the running total
float average = 0;                // the average
float readings[numReadings];      // the readings from the analog input

//SoftwareSerial mySerial(10, 11); // RX, TX

float receivedAngle = 0;

bool needUpdate = false;


//Stepper
#include <AccelStepper.h>

bool isCal[4]; //L1//L2//R1//R2
int stepSpeed[4];
int stepAccel[4];
int stepBtn[4];

bool isCalL1 = true;
int speedL1 = 3300;
int accelL1 = 1500;
AccelStepper stepperL1(AccelStepper::DRIVER, 6, 7);
int btnL1 = 4;

bool isCalL2 = true;
int speedL2 = 3300;
int accelL2 = 1500;
AccelStepper stepperL2(AccelStepper::DRIVER,8, 9);
int btnL2 = 7;


bool isCalR1 = true;
int speedR1 = 3300;
int accelR1 = 1500;
AccelStepper stepperR1(AccelStepper::DRIVER, 38, 39);
int btnR1 = 40;

bool isCalR2 = true;
int speedR2 = 3300;
int accelR2 = 1500;
AccelStepper stepperR2(AccelStepper::DRIVER, 41, 42);
int btnR2 = 43;

//MidMotor
AccelStepper stepperMid(AccelStepper::DRIVER, 22, 23);
int stepLowerValMid = 180;
int stepHigherValMid = 180;

int delayLowerValMid = 1000;
int delayHigherValMid = 3000;
long intervalMid = 1000;

unsigned long previousMillisMid = 0;        // will store last time LED was updated
// constants won't change

bool isCalMid = true;

bool clockwiseMid = true;



//Rotary
#include <EEPROM.h>

int addr = 0;

const int analogInPin = A5;  // Analog input pin that the potentiometer is attached to

int sensorVal = 0;        // value read from the pot
int rotaryVal = 0;        // value output to the PWM (analog out)
int prevrotaryVal = 0;
bool rotaryChange = false;
int flashVal;

int posL1 = 0;
int posL2 = 0;
int posR1 = 0;
int posR2 = 0;

float myfloat;


//Rad Mode
bool isHighPos[4] = {false,false,false,false};

//Pos Mode
Metro posMetro = Metro(5000);  // Instantiate an instance
Metro modeMetro = Metro(60000);  // Instantiate an instance
int posMetroMin = 600;
int posMetroMax = 5000;

bool posMode = true;
bool debug = false;
int pos = 0;

const int numOfPos = 9;
int currPos = 0;


int motorPosL1[numOfPos] = {100, 300, 500, 400, 2000, 4000,5000,3000,4000};
int motorPosL2[numOfPos] = {5000, 5000, 3000, 1000, 4000,1000,5000,5000,3000};
int motorPosR1[numOfPos] = {500, 100, 3000, 3000, 1800, 1000,5000,3000,4000};
int motorPosR2[numOfPos] = {500, 100, 3000, 2000, 1300, 4000,5000,5000,300};
int motorPosMid[numOfPos] = {0, -100, 150, 180, -180, 180, -100, 130,-130};


int motorPosL1_L[numOfPos] = {100, 100, 500};
int motorPosL2_L[numOfPos] = {5000, 5000, 500};
int motorPosR1_L[numOfPos] = {500, 100, 500};
int motorPosR2_L[numOfPos] = {500, 100, 500};
int motorPosMid_L[numOfPos] = {0, -100, 150};
//L


int motorPosL1_R[numOfPos] = {100, 100, 100};
int motorPosL2_R[numOfPos] = {100, 100, 100};
int motorPosR1_R[numOfPos] = {5000, 3000, 4000};
int motorPosR2_R[numOfPos] = {5000, 100, 1000};
int motorPosMid_R[numOfPos] = { -10, -100, 150};

int motorPosL1_M[numOfPos] = {100, 800, 500};
int motorPosL2_M[numOfPos] = {5000, 800, 500};
int motorPosR1_M[numOfPos] = {500, 800, 500};
int motorPosR2_M[numOfPos] = {1000, 800, 500};
int motorPosMid_M[numOfPos] = {0, 0, 0};

//int motorPos1[5] = {100, 5000, 500, 500, -100}; //L1 //L2 //R1 //R2 //M

//M
//int motorPos0[5] = {100, 5000, 100, 5000, 0}; //L1 //L2 //R1 //R2 //M


int centerRot = 81; //>81 R Low < 81 L Low
int currentRot;


void setup()
{


  pinMode(btnL1, INPUT);
  pinMode(btnL2, INPUT);
  pinMode(btnR1, INPUT);
  pinMode(btnR2, INPUT);
  stepperL1.setMaxSpeed(1000);
  stepperL2.setMaxSpeed(1000);
  stepperR1.setMaxSpeed(1000);
  stepperR2.setMaxSpeed(1000);


  stepperMid.setCurrentPosition(0);
  stepperMid.setMaxSpeed(330);
  stepperMid.setAcceleration(190);

  //Serial

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial2.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  while (!Serial2) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  receivedAngle = 0;

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop()
{


  if (digitalRead(btnL1) == LOW) {
    // stepperL1.setCurrentPosition(0);
    Serial.println("stepperL1 pos 0");
  }
  if (digitalRead(btnL2) == LOW) {
    // stepperL2.setCurrentPosition(0);
    Serial.println("stepperL2 pos 0");
  }
  if (digitalRead(btnR1) == LOW) {
     //stepperR1.setCurrentPosition(0);
    Serial.println("stepperR1 pos 0");
  }
  if (digitalRead(btnR2) == LOW) {
    // stepperR2.setCurrentPosition(0);
    Serial.println("stepperR2 pos 0");
  }

  if (isCalL1) {
    calL1();
  } else {
    if (stepperL1.distanceToGo() == 0) {
      if (posMode) {
        if (pos == 0) {
          stepperL1.moveTo(-motorPosL1[currPos]);
        }
      } else {

        if(isHighPos[0]){
           stepperL1.moveTo(-(random(4000, 5000)));
          }
          else{
            stepperL1.moveTo(-(random(1000, 2500)));
          }
          isHighPos[0] = !isHighPos[0];


        
        //stepperL1.moveTo(-random(1000, 5000));
      }
    }

    stepperL1.run();
  }

  if (isCalL2) {
    calL2();
  } else {
    if (stepperL2.distanceToGo() == 0) {
      if (posMode) {
        if (pos == 0) {
          stepperL2.moveTo(motorPosL2[currPos]);
        }
      } else {
        if(isHighPos[1]){
           stepperL2.moveTo((random(4000, 5000)));
          }
          else{
            stepperL2.moveTo((random(1000, 2500)));
          }
          isHighPos[1] = !isHighPos[1];
      }
    }

    stepperL2.run();
  }


  if (isCalR1) {
    calR1();
  } else {
    if (stepperR1.distanceToGo() == 0) {
      if (posMode) {
        if (pos == 0) {
          stepperR1.moveTo(-motorPosR1[currPos]);
        }
      } else {
        if(isHighPos[2]){
           stepperR1.moveTo(-(random(4000, 5000)));
          }
          else{
            stepperR1.moveTo(-(random(1000, 2500)));
          }
          isHighPos[2] = !isHighPos[2];
        //stepperR1.moveTo(-(random(1000, 5000)));
      }
    }

    stepperR1.run();
  }

  if (isCalR2) {
    calR2();
  } else {
    if (stepperR2.distanceToGo() == 0) {
      if (posMode) {
        if (pos == 0) {
          stepperR2.moveTo(-motorPosR2[currPos]);
        }
      } else {
        if(isHighPos[3]){
           stepperR2.moveTo(-(random(4000, 5000)));
          }
          else{
            stepperR2.moveTo(-(random(1000, 2500)));
          }
          isHighPos[3] = !isHighPos[3];


        
     //   stepperR2.moveTo(-(random(1000, 5000)));
      }
    }

    stepperR2.run();
  }

  //MidMotor

  if (isCalMid) {
    calMid();
  } else {

    unsigned long currentMillis = millis();
    if (stepperMid.distanceToGo() == 0) {
      if (posMode) {
        if (pos == 0) {
          stepperMid.moveTo(motorPosMid[currPos]);
        }
      } else {
        if (currentMillis - previousMillisMid >= intervalMid) {
          int a;



          if (clockwiseMid) {
            a = random(stepLowerValMid, stepHigherValMid);
            clockwiseMid = !clockwiseMid;
          } else {
            a = -1 * random(stepLowerValMid, stepHigherValMid);
            clockwiseMid = !clockwiseMid;
          }

          stepperMid.moveTo(a);
          intervalMid = random(delayLowerValMid, delayHigherValMid);
        }

      }
    }
    stepperMid.run();

  }

  if (serialMetro.check() == 1) { // check if the metro has passed it's interval .
    // if (needUpdate) {
    receiveSerial();
    if (receivedAngle > 90) {
      receivedAngle = 90;
      Serial.println("TOO BIG");
    } else if (receivedAngle < -90) {
      receivedAngle = -90;
      Serial.println("TOO SMALL");
    }
    //  Serial.println(receivedAngle);
    posL1 = (int)map(average, -50, 50, 1000, 5000);
    posL2 = (int)map(average, -50, 50, 1000, 5000);
    posR1 = (int)map(average, -50, 50, 1000, 5000);
    posR2 = (int)map(average, -50, 50, 1000, 5000);
    //   }
    //Rotary
    sensorVal = analogRead(analogInPin);
    // map it to the range of the analog out:
    rotaryVal = map(sensorVal, 0, 1023, 0, 360);
    if(debug){
Serial.print("Rotary Val : ");
Serial.println(rotaryVal);
    }
    // print the results to the serial monitor:
    // Serial.print("sensor = ");
    // Serial.print(sensorVal);
    // Serial.print("\t output = ");
    //Serial.println(rotaryVal);




  }
  needUpdate = false;


  if (modeMetro.check() == 1) {

    posMode = !posMode;
  }
  if (posMetro.check() == 1) {
    currPos = random(0, numOfPos);
    posMetro.interval(random(posMetroMin, posMetroMax));
  }
}


void calL1() {

  int currSpeed;
  if (digitalRead(btnL1) == HIGH) {
    currSpeed = 400; //1000
  } else {
    currSpeed = 0;
    stepperL1.setCurrentPosition(0);
    stepperL1.setMaxSpeed(speedL1);
    stepperL1.setAcceleration(accelL1);

    isCalL1 = false;
  }

  stepperL1.setSpeed(currSpeed);
  stepperL1.runSpeed();
}


void calL2() {

  int currSpeed;
  if (digitalRead(btnL2) == HIGH) {
    currSpeed = -400; //1000
  } else {
    currSpeed = 0;
    stepperL2.setCurrentPosition(0);
    stepperL2.setMaxSpeed(speedL2);
    stepperL2.setAcceleration(accelL2);

    isCalL2 = false;
  }

  stepperL2.setSpeed(currSpeed);
  stepperL2.runSpeed();
}


void calR1() {

  int currSpeed;
  if (digitalRead(btnR1) == HIGH) {
    currSpeed = 400; //1000
  } else {
    currSpeed = 0;
    stepperR1.setCurrentPosition(0);
    stepperR1.setMaxSpeed(speedR1);
    stepperR1.setAcceleration(accelR1);

    isCalR1 = false;
  }

  stepperR1.setSpeed(currSpeed);
  stepperR1.runSpeed();
}




void calR2() {

  int currSpeed;
  if (digitalRead(btnR2) == HIGH) {
    currSpeed = 400; //1000
  } else {
    currSpeed = 0;
    stepperR2.setCurrentPosition(0);
    stepperR2.setMaxSpeed(speedR2);
    stepperR2.setAcceleration(accelR2);

    isCalR2 = false;
  }

  stepperR2.setSpeed(currSpeed);
  stepperR2.runSpeed();
}


void calMid() {

  int currSpeed;
  if (rotaryVal == 40) {
    currSpeed = 0;
    stepperMid.setCurrentPosition(0);
    stepperMid.setMaxSpeed(90);
    stepperMid.setAcceleration(50);
    isCalMid = false;
    Serial.println("CalDone");
  } else if (rotaryVal > 40 && rotaryVal < 220) {
    currSpeed = -10; //1000
    Serial.println("R");
  } else if ((rotaryVal < 40 && rotaryVal >= 0) || (rotaryVal >= 220 && rotaryVal >= 360)) {
    currSpeed = 10; //1000
    Serial.println("L");
  } else {
    Serial.println("Nocase");
  }


  stepperMid.setSpeed(currSpeed);
  stepperMid.runSpeed();
}



void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
  asm volatile ("  jmp 0");
}
