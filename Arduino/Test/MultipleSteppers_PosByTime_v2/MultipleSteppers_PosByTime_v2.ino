#include <MyStepper.h>
#include "PinAssignment.h" //Pin Config

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int buttonState2;

int prevReading0 = 0;
int prevReading1 = 0;
int prevReading2 = 0;
int prevReading3 = 0;

int minPos = 000;
int maxPos = 5000;

int stageLMotor = 0;
int stageRMotor = 0;
//========= STEPPER ========
//#include <AccelStepper.h>

MyStepper stepper1 (MyStepper::DRIVER, lyStep, lyDir);
AccelStepper stepper2 (AccelStepper::DRIVER, ryStep, ryDir);
//AccelStepper stepper (AccelStepper::DRIVER, st, dir);

// L == up
bool goHome = true;
bool goHome2 = true;


long homeSpeed = -250;
long homeMaxSpeed = 250;

long motorSpeed = 500 * 5000;
long motorAccel = 80 * 10;

//long motorSpeed = 500 * 10;
//long motorAccel = 800 * 10;

bool triggerState = true;
bool finish = false;

bool manualMode = true;
bool isLeft = true;
int moveToPos = 1000;
int preMoveToPos = 0;

int timeToMove = 1000;
bool isNew = false;
int newTwoTimes = 0;



long currentTime;
long prevTime;


const long MAX_SPEED = 200000;

bool isStepper1Stopped = false;
bool isStepper2Stopped = false;


const int timeStep = 1000;  // ms


void setup() {

  // ============ SERIAL ================
  Serial.begin(BAUD);
  Serial1.begin(BAUD);
  pinMode(RX1, INPUT_PULLUP);

  pinMode(LimitSwitchLy, INPUT);
  pinMode(LimitSwitchRy, INPUT);

  pinMode(DI1_SERVO_ON_ly, OUTPUT);
  pinMode(DI1_SERVO_ON_ry, OUTPUT);

  digitalWrite(DI1_SERVO_ON_ly, HIGH);
  digitalWrite(DI1_SERVO_ON_ry, HIGH);

  /*  digitalWrite(BrakeLx, LOW);
    digitalWrite(BrakeLy, LOW);
    digitalWrite(BrakeRx, LOW);
    digitalWrite(BrakeRy, LOW);
  */

  digitalWrite(DI2_ALARM_RESET_ly, HIGH);
  digitalWrite(DI2_ALARM_RESET_ry, HIGH);

  delay(500);

  digitalWrite(DI2_ALARM_RESET_ly, LOW);
  digitalWrite(DI2_ALARM_RESET_ry, LOW);

  //========
  Serial.println("=== UReady to go home === ");

  /*
      digitalWrite(BrakeLx, HIGH);
      digitalWrite(BrakeLy, HIGH);
      digitalWrite(BrakeRx, HIGH);
      digitalWrite(BrakeRy, HIGH);
  */
  digitalWrite(DI1_SERVO_ON_ly, HIGH);
  digitalWrite(DI1_SERVO_ON_ry, HIGH);

  alarm_check();

}

void loop() {
  currentTime = millis();

  if (stepper1.isCompleteTotDist() && isNew) {
    stepper1.reset(moveToPos, timeToMove);
    stepper1.setTimeStepInMillis(timeStep);
    stepper1.setIsPrintTimeStepToSerial(true);
    newTwoTimes++;
    if (newTwoTimes >= 2) {
      long timeTravelled = currentTime - prevTime;

      Serial.print("timeTravelled : ");
      Serial.println(timeTravelled);
      newTwoTimes = 0;
      isNew = false;
    }
  }

  if (!stepper1.isCompleteTotDist())
  {
    stepper1.myRun();
  }

  if (stepper2.distanceToGo() == 0) {
    // stepper2.setMaxSpeed(MAX_SPEED);
    // stepper2.setAcceleration(computeAcceleration(abs(stepper2.currentPosition() - moveToPos), timeToMove));
    // stepper2.moveTo(moveToPos);
  }
  //stepper1.run();
  //stepper2.run();


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
    digitalWrite(DI1_SERVO_ON_ly, HIGH);
    digitalWrite(DI1_SERVO_ON_ry, HIGH);

    if (inChar == 'l') {
      isLeft = true;
      Serial.print("Left : ");
      isNew = true;
      prevTime = currentTime;
    }
    if (inChar == 'r') {
      isLeft = false;
      isNew = true;
      prevTime = currentTime;
      Serial.print("Right : ");
    }
    if (inChar == 't') {
      //isLeft = false;
      Serial.print("Right : ");
      timeToMove = inChar2;
      Serial.println(timeToMove);
    }
    if (inChar == 'p') {
      // isLeft = false;
      Serial.print("moveToPos : ");
      moveToPos = inChar2;
      Serial.println(moveToPos);
    }
    if (inChar == 'm') {
      manualMode = !manualMode;
      Serial.print("Manual Mode : ");
      Serial.println(manualMode);
      stepper1.setCurrentPosition(0);
      moveToPos = 0;
    }
  }
}
