


#include "PinAssignment.h" //Pin Config

/*



P1-44 Val - 15000
*/
/*
  #define DI1_SERVO_ON_lx 14
  #define DI1_SERVO_ON_ly 15
  #define DI1_SERVO_ON_rx 16
  #define DI1_SERVO_ON_ry 17

  #define DI2_ALARM_RESET_lx 50
  #define DI2_ALARM_RESET_ly 51
  #define DI2_ALARM_RESET_rx 52
  #define DI2_ALARM_RESET_ry 53

  #define DO1_SERVO_READY_lx 34
  #define DO1_SERVO_READY_ly 35
  #define DO1_SERVO_READY_rx 36
  #define DO1_SERVO_READY_ry 37

  #define BrakeLx A4
  #define BrakeLy A5
  #define BrakeRx A6
  #define BrakeRy A7


  #define DI2_ALARM_RESET_lx 50
  #define DI2_ALARM_RESET_ly 51
  #define DI2_ALARM_RESET_rx 52
  #define DI2_ALARM_RESET_ry 53

  //Servo Alarm
  #define DO5_ALRM_lx 46
  #define DO5_ALRM_ly 47
  #define DO5_ALRM_rx 48
  #define DO5_ALRM_ry 49


*/


// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin

int prevReading0 = 0;
int prevReading1 = 0;
int prevReading2 = 0;
int prevReading3 = 0;

int minPos = 0;
int maxPos = 5000;
//========= STEPPER ========
#include <AccelStepper.h>

AccelStepper stepper (AccelStepper::DRIVER, lxStep, lxDir);
AccelStepper stepper2 (AccelStepper::DRIVER, lyStep, lyDir);
//AccelStepper stepper (AccelStepper::DRIVER, st, dir);

// L == up
bool goHome = false;


long homeSpeed = -250;
long homeMaxSpeed = 250;

long motorSpeed = 500*100000;
long motorAccel = 800*10000;

//long motorSpeed = 500 * 10;
//long motorAccel = 800 * 10;

bool triggerState = true;
bool finish = false;

bool manualMode = true;
bool isLeft = true;
int moveToPos;

void setup() {
 // //========= Serial ========
 // Serial.begin(57600);

  
  // ============ SERIAL ================
  Serial.begin(BAUD);
  Serial1.begin(BAUD);
  pinMode(RX1, INPUT_PULLUP);

  pinMode(LimitSwitchLx, INPUT);
  pinMode(LimitSwitchLy, INPUT);
  pinMode(LimitSwitchRx, INPUT);
  pinMode(LimitSwitchRy, INPUT);

  //

  pinMode(DI1_SERVO_ON_lx, OUTPUT);
  pinMode(DI1_SERVO_ON_ly, OUTPUT);
  pinMode(DI1_SERVO_ON_rx, OUTPUT);
  pinMode(DI1_SERVO_ON_ry, OUTPUT);

  digitalWrite(DI1_SERVO_ON_lx, HIGH);
  digitalWrite(DI1_SERVO_ON_ly, HIGH);
  digitalWrite(DI1_SERVO_ON_rx, HIGH);
  digitalWrite(DI1_SERVO_ON_ry, HIGH);

/*  digitalWrite(BrakeLx, LOW);
  digitalWrite(BrakeLy, LOW);
  digitalWrite(BrakeRx, LOW);
  digitalWrite(BrakeRy, LOW);
*/

  digitalWrite(DI2_ALARM_RESET_lx, HIGH);
  digitalWrite(DI2_ALARM_RESET_ly, HIGH);
  digitalWrite(DI2_ALARM_RESET_rx, HIGH);
  digitalWrite(DI2_ALARM_RESET_ry, HIGH);

  delay(500);
  
  digitalWrite(DI2_ALARM_RESET_lx, LOW);
  digitalWrite(DI2_ALARM_RESET_ly, LOW);
  digitalWrite(DI2_ALARM_RESET_rx, LOW);
  digitalWrite(DI2_ALARM_RESET_ry, LOW);



  //========
  Serial.println("=== UReady to go home === ");


/*    digitalWrite(BrakeLx, HIGH);
    digitalWrite(BrakeLy, HIGH);
    digitalWrite(BrakeRx, HIGH);
    digitalWrite(BrakeRy, HIGH);
*/
    digitalWrite(DI1_SERVO_ON_lx, HIGH);
    digitalWrite(DI1_SERVO_ON_ly, HIGH);
    digitalWrite(DI1_SERVO_ON_rx, HIGH);
    digitalWrite(DI1_SERVO_ON_ry, HIGH);



  
}

void loop() {
  alarm_check();

  if (manualMode) {


    //int reading = digitalRead(buttonPin);
    int reading0 = analogRead(LimitSwitchLx);
    int reading1 = analogRead(LimitSwitchLy);
    int reading2 = analogRead(LimitSwitchRx);
    int reading3 = analogRead(LimitSwitchRy);

    /*   Serial.print("A0: ");
       Serial.print(reading0);
       Serial.print(" A1: ");
       Serial.print(reading1);
       Serial.print(" A2: ");
       Serial.print(reading2);
       Serial.print(" A3: ");
       Serial.println(reading3);*/

    if (reading0 > prevReading0) {
      Serial.print("A0: ");
      Serial.println(reading0);
      prevReading0 = reading0;
    }
    if (reading1 > prevReading1) {
      Serial.print("A1: ");
      Serial.println(reading1);
      prevReading1 = reading1;
    }
    if (reading2 > prevReading2) {
      Serial.print("A2: ");
      Serial.println(reading2);
      prevReading2 = reading2;
    }
    if (reading3 > prevReading3) {
      Serial.print("A3: ");
      Serial.println(reading3);
      prevReading3 = reading3;
    }
    // buttonState = reading;
  //  digitalWrite(ledPin, buttonState);
    if (stepper.distanceToGo() == 0) {
      moveToPos = random(minPos, maxPos);
      Serial.print("Move TO Pos : ");
      Serial.println(moveToPos);
      stepper.setMaxSpeed(motorSpeed);
      stepper.setAcceleration(motorAccel);
      if (isLeft) {
        stepper.moveTo(-moveToPos);
      } else {
        stepper.moveTo(moveToPos);
      }

    }

       if (stepper2.distanceToGo() == 0) {
      moveToPos = random(minPos, maxPos);
      Serial.print("Move TO Pos : ");
      Serial.println(moveToPos);
      stepper2.setMaxSpeed(motorSpeed);
      stepper2.setAcceleration(motorAccel);
      if (isLeft) {
        stepper2.moveTo(-moveToPos);
      } else {
        stepper2.moveTo(moveToPos);
      }

    }
    /*
      if (reading0 > 500) {
      stepper.setCurrentPosition(0);
      Serial.println(reading0);
      moveToPos = 0;
      Serial.println("Reached Home A0");

      } else
    */
  //  if (reading3 > 500) {
      //    stepper.setCurrentPosition(0);
      //  Serial.println(reading3);
      //  moveToPos = 0;
     // Serial.println("Reached Home A3");
      // stepper.run();
 //   }
  //  else {
      stepper.run();
      stepper2.run();
 //   }

  }
  else {
    if (goHome) {
      int reading = digitalRead(LimitSwitchLx);
      buttonState = reading;
   //   digitalWrite(ledPin, buttonState);
      stepper.setMaxSpeed(homeMaxSpeed);
      if (isLeft) {
        stepper.setSpeed(-homeSpeed);
      }
      else {
        stepper.setSpeed(homeSpeed);
      }
      if (buttonState) {

        Serial.println("Reached Home");
        stepper.setCurrentPosition(0);
        goHome = false;
      }
      stepper.runSpeed();
    }
    else
    {
    }
  }
  while (Serial.available() > 0)
  {
    int inChar = Serial.read();
    int inChar2 = Serial.parseInt();
/*
    digitalWrite(BrakeLx, HIGH);
    digitalWrite(BrakeLy, HIGH);
    digitalWrite(BrakeRx, HIGH);
    digitalWrite(BrakeRy, HIGH);
*/
    digitalWrite(DI1_SERVO_ON_lx, HIGH);
    digitalWrite(DI1_SERVO_ON_ly, HIGH);
    digitalWrite(DI1_SERVO_ON_rx, HIGH);
    digitalWrite(DI1_SERVO_ON_ry, HIGH);



    if (inChar == 'l') {
      isLeft = true;
      Serial.print("Left : ");
      moveToPos = inChar2;
      Serial.println(moveToPos);
    }
    if (inChar == 'r') {
      isLeft = false;
      Serial.print("Right : ");
      moveToPos = inChar2;
      Serial.println(moveToPos);
    }
    if (inChar == 'm') {
      manualMode = !manualMode;
      Serial.print("Manual Mode : ");
      Serial.println(manualMode);
      stepper.setCurrentPosition(0);
      moveToPos = 0;
    }
  }
}

