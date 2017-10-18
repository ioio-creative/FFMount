/*  S200-200-200 for initi EEPROM

     AccelStepper Library
   http://www.airspayce.com/mikem/arduino/AccelStepper/
   http://www.airspayce.com/mikem/arduino/AccelStepper/AccelStepper-1.56.zip


   x -> Horizontal Movement
   y -> Vertical Movement

    __________________
   | 1  2  ....... 6  |
   |                  |
   | Ly Ly ....... Ly |
   | Ry Ry ....... Ry |
   |                  |
   |__________________|
            |
            |
    ________|_________
   |                  |
   |  Control Room    |
   |__________________|



  //Todo
  -Test Actual code with 4 motor and switches

*/
const int timeStep = 100;  // ms
const float minReturnSpeed = 100;



//#define ISMEGA
//#include "SoftReset.h" //SoftReset function
//================ Config ================
#include "PinAssignment.h" //Pin Config
#include "config.h" //Pin Config
//================ Serial ================

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

long input_value[Input_size];

// ============ STEPPER ================
//#include <AccelStepper.h>
#include <MyStepper.h>

long stepperSpeed[numOfStepper]  = {0, 0};
long stepperAccel[numOfStepper]  = {0, 0};
long stepperTime[numOfStepper]  = {0, 0};  // Travel Time to POS
long stepperPos[numOfStepper]  = {0, 0};


MyStepper stepperLy (AccelStepper::DRIVER, lyStep, lyDir);
MyStepper stepperRy (AccelStepper::DRIVER, ryStep, ryDir);

MyStepper* steppers[numOfStepper] = { &stepperLy, &stepperRy };

bool isEmergencyStop = false;

//variables
long inverseDir[numOfStepper]  = {0, 0};
long maxPos[numOfStepper]  = {0, 0 };
long maxSpeed[numOfStepper] = {0, 0};
long maxAccel[numOfStepper] = {0, 0};



// ============ ENCODER ================
#include <Encoder.h>

Encoder encoderLy(encoderLyA, encoderLyB);
Encoder encoderRy(encoderRyA, encoderRyB);

Encoder* encoder[numOfStepper] = {&encoderLy, &encoderRy};

// ============ LIMIT SWITCH ================
const byte limitSwitch[numOfStepper]  = {LimitSwitchLy, LimitSwitchRy};
bool homeDone[numOfStepper]  = {false, false};
int home_speed = 200;
int home_accel = 200;
//================ Style ================
//Motor
int style = 0; //0 command to go
//LED
int LEDFactor0;
int LEDFactor1;
int LEDFactor2;
int LEDFactor3;

//style 2
long stepperPos1[numOfStepper];
long stepperPos2[numOfStepper];
bool abPos[numOfStepper];

// ============ ============ ================
// ============ ====SETUP=== ================
// ============ ============ ================

void setup() {


  // ============ STEPPER ================

  for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
    steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
    steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
    steppers[stepperNumber]->moveTo(stepperPos[stepperNumber]);

    steppers[stepperNumber]->setMinReturnSpeed(minReturnSpeed);  // should be positive
    steppers[stepperNumber]->setTimeStepInMillis(timeStep);

    //steppers[stepperNumber]->setPinsInverted(true, true, true); //(directionInvert,stepInvert,enableInvert)
#ifdef ISMEGA
    Load_Flash();
    Load_To_Variables();
#else
#endif

    // ============ LIMIT SWITCH ================
    pinMode(limitSwitch[stepperNumber], INPUT);

  }

  // ============ LED ================
  setupLED();

  // ============ SERIAL ================
  Serial.begin(BAUD);
  Serial1.begin(BAUD);
  pinMode(RX1, INPUT_PULLUP);
  // ============  ================

  pinMode(BrakeLy, OUTPUT);
  pinMode(BrakeRy, OUTPUT);

  digitalWrite(BrakeLy, LOW);
  digitalWrite(BrakeRy, LOW);

  pinMode(DI1_SERVO_ON_ly, OUTPUT);
  pinMode(DI1_SERVO_ON_ry, OUTPUT);

  digitalWrite(DI1_SERVO_ON_ly, HIGH);
  digitalWrite(DI1_SERVO_ON_ry, HIGH);

  digitalWrite(DI2_ALARM_RESET_ly, HIGH);
  digitalWrite(DI2_ALARM_RESET_ry, HIGH);

  delay(500);

  digitalWrite(DI2_ALARM_RESET_ly, LOW);
  digitalWrite(DI2_ALARM_RESET_ry, LOW);
  /*
    #define DI2_ALARM_RESET_lx 50
    #define DI2_ALARM_RESET_ly 51
    #define DI2_ALARM_RESET_rx 52
    #define DI2_ALARM_RESET_ry 53*/

  pinMode(DO5_ALRM_ly, INPUT);
  pinMode(DO5_ALRM_ry, INPUT);
}

// ============ ============ ================
// ============ ====LOOP==== ================
// ============ ============ ================

void loop() {
  // == == == == == == SERIAL == == == == == == == ==
  serial_decode();
  check_update();
  alarm_check();
  if (isEmergencyStop) {
    //TODO -> check brake
  }
  else if (GO_HOME) { //TODO

    digitalWrite(BrakeLy, HIGH);
    digitalWrite(BrakeRy, HIGH);

    digitalWrite(DI1_SERVO_ON_ly, HIGH);
    digitalWrite(DI1_SERVO_ON_ry, HIGH);

    int limitSwitchReading[numOfStepper]  = {0, 0};

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      limitSwitchReading[stepperNumber] = analogRead(limitSwitch[stepperNumber]);
      if (limitSwitchReading[stepperNumber] > 500 && !homeDone[stepperNumber]) {
        steppers[stepperNumber]->setSpeed(0);
        steppers[stepperNumber]->setCurrentPosition(0);
        homeDone[stepperNumber] = true;
        Serial.print("hm-");
        Serial.print(stepperNumber);
        Serial.print("-");
      }
      if (homeDone[stepperNumber]) {
        steppers[stepperNumber]->setMaxSpeed(0);
        steppers[stepperNumber]->setSpeed(0);
      } else {
        steppers[stepperNumber]->setMaxSpeed(1 * home_speed);
        steppers[stepperNumber]->setSpeed(1 * home_speed);
      }

      steppers[stepperNumber]->runSpeed();
    }

    if (homeDone[0] && homeDone[1]) {
      GO_HOME = false; //done with go home
      Serial.print("allhm");
      Serial.println("|");
    }

  } else {

    // ============ LED ================
    loopLED();

    // ============ STEPPER ================

    stepper_style();

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (style != 4) {
        steppers[stepperNumber]->run();
      } else {
        if (!steppers[stepperNumber]->isCompleteTotDist()) {
          steppers[stepperNumber]->myRun();
        }

      }
    }
    // ============ ENCODER ================

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      encoder[stepperNumber]->read(); //TODO, read to sth?
    }
  }
}

