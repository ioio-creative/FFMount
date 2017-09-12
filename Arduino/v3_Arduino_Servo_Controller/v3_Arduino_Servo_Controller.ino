/*  AccelStepper Library
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


#include "SoftReset.h" //SoftReset function
//================ Config ================
#include "PinAssignment.h" //Pin Config
#include "config.h" //Pin Config
//================ Serial ================

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

long input_value[Input_size];

// ============ STEPPER ================
#include <AccelStepper.h>


long stepperSpeed[numOfStepper]  = {0, 0};
long stepperAccel[numOfStepper]  = {0, 0};
long stepperPos[numOfStepper]  = {0, 0};


AccelStepper stepperLy (AccelStepper::DRIVER, lyStep, lyDir);
AccelStepper stepperRy (AccelStepper::DRIVER, ryStep, ryDir);

AccelStepper* steppers[numOfStepper] = { &stepperLy, &stepperRy };

bool isEmergencyStop = false;

//variables
long inverseDir[numOfStepper]  = {0, 0};
long maxPos[numOfStepper]  = {0, 0 };
long maxSpeed[numOfStepper] = {0, 0};
long maxAccel[numOfStepper] = {0, 0};



// ============ ENCODER ================
#include <Encoder.h>

Encoder encoderLx(encoderLxA, encoderLxB);
Encoder encoderLy(encoderLyA, encoderLyB);
Encoder encoderRx(encoderRxA, encoderRxB);
Encoder encoderRy(encoderRyA, encoderRyB);

Encoder* encoder[numOfStepper] = {&encoderLx, &encoderLy, &encoderRx, &encoderLy};

// ============ LIMIT SWITCH ================
const byte limitSwitch[numOfStepper]  = {LimitSwitchLx, LimitSwitchLy, LimitSwitchRx, LimitSwitchRy};
bool homeDone[numOfStepper]  = {false, false, false, false};
int home_speed = 100;
int home_accel = 100;
//================ Style ================
int style = 0; //0 command to go


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

    //steppers[stepperNumber]->setPinsInverted(true, true, true); //(directionInvert,stepInvert,enableInvert)
    Load_Flash();
    Load_To_Variables();
    // ============ LIMIT SWITCH ================
    // pinMode(limitSwitch[stepperNumber], INPUT_PULLUP);

  }

  // ============ SERIAL ================
  Serial.begin(BAUD);
  Serial1.begin(BAUD);
  pinMode(RX1, INPUT_PULLUP);
  // ============  ================

  pinMode(BrakeLx, OUTPUT);
  pinMode(BrakeLy, OUTPUT);
  pinMode(BrakeRx, OUTPUT);
  pinMode(BrakeRy, OUTPUT);

  digitalWrite(BrakeLx, LOW);
  digitalWrite(BrakeLy, LOW);
  digitalWrite(BrakeRx, LOW);
  digitalWrite(BrakeRy, LOW);

  pinMode(DI1_SERVO_ON_lx, OUTPUT);
  pinMode(DI1_SERVO_ON_ly, OUTPUT);
  pinMode(DI1_SERVO_ON_rx, OUTPUT);
  pinMode(DI1_SERVO_ON_ry, OUTPUT);

  digitalWrite(DI1_SERVO_ON_lx, HIGH);
  digitalWrite(DI1_SERVO_ON_ly, HIGH);
  digitalWrite(DI1_SERVO_ON_rx, HIGH);
  digitalWrite(DI1_SERVO_ON_ry, HIGH);


  digitalWrite(DI2_ALARM_RESET_lx, HIGH);
  digitalWrite(DI2_ALARM_RESET_ly, HIGH);
  digitalWrite(DI2_ALARM_RESET_rx, HIGH);
  digitalWrite(DI2_ALARM_RESET_ry, HIGH);

  delay(500);



  digitalWrite(DI2_ALARM_RESET_lx, LOW);
  digitalWrite(DI2_ALARM_RESET_ly, LOW);
  digitalWrite(DI2_ALARM_RESET_rx, LOW);
  digitalWrite(DI2_ALARM_RESET_ry, LOW);
  /*
    #define DI2_ALARM_RESET_lx 50
    #define DI2_ALARM_RESET_ly 51
    #define DI2_ALARM_RESET_rx 52
    #define DI2_ALARM_RESET_ry 53*/

  pinMode(DO5_ALRM_lx, INPUT);
  pinMode(DO5_ALRM_ly, INPUT);
  pinMode(DO5_ALRM_rx, INPUT);
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

    digitalWrite(BrakeLx, HIGH);
    digitalWrite(BrakeLy, HIGH);
    digitalWrite(BrakeRx, HIGH);
    digitalWrite(BrakeRy, HIGH);

    digitalWrite(DI1_SERVO_ON_lx, HIGH);
    digitalWrite(DI1_SERVO_ON_ly, HIGH);
    digitalWrite(DI1_SERVO_ON_rx, HIGH);
    digitalWrite(DI1_SERVO_ON_ry, HIGH);

    int limitSwitchReading[numOfStepper]  = {0, 0, 0, 0};

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      limitSwitchReading[stepperNumber] = analogRead(limitSwitch[stepperNumber]);

      if (homeDone[stepperNumber]) {
        steppers[stepperNumber]->setSpeed(0);
      } else {
        if (stepperNumber == 1 || stepperNumber == 3) {
          steppers[stepperNumber]->setMaxSpeed((inverseDir[stepperNumber])*home_speed * 2);
          steppers[stepperNumber]->setSpeed(-1 * (inverseDir[stepperNumber])*home_speed * 2);
        } else {
          steppers[stepperNumber]->setMaxSpeed((inverseDir[stepperNumber])*home_speed);
          steppers[stepperNumber]->setSpeed(-1 * (inverseDir[stepperNumber])*home_speed);
        }
      }
      if (limitSwitchReading[stepperNumber] > 500 && !homeDone[stepperNumber]) {

        steppers[stepperNumber]->setCurrentPosition(0);
        homeDone[stepperNumber] = true;
        Serial.print("hm-");
        Serial.print(stepperNumber);
        Serial.print("-");
      }
      steppers[stepperNumber]->runSpeed();
    }

    if (homeDone[0] && homeDone[1] && homeDone[2] && homeDone[3]) {
      GO_HOME = false; //done with go home
      Serial.print("allhm");
      Serial.println("|");
    }

  } else {



    // ============ STEPPER ================

    stepper_style();

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      steppers[stepperNumber]->run();
    }
    // ============ ENCODER ================


    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      encoder[stepperNumber]->read(); //TODO, read to sth?
    }

    // ============  ================
    /* unsigned long currentMillis = millis();

      if (currentMillis - previousMillis > interval) {
       // save the last time you blinked the LED
       previousMillis = currentMillis;

       // if the LED is off turn it on and vice-versa:
       if (ledState == LOW) {
         ledState = HIGH;
       }
       else {
         ledState = LOW;
         Serial.println("C");
       }
       // set the LED with the ledState of the variable:

      }
    */

  }


}

