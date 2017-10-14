#include <MyStepper.h>

MyStepper stepper1(MyStepper::DRIVER, 3, 2);
MyStepper stepper2(MyStepper::DRIVER, 5, 4);

#define LimitSwitchLy A0 
#define LimitSwitchRy A1
long homeMaxSpeed = 250;
long homeSpeed = -250;

int buttonState;             // the current reading from the input pin
int buttonState2;



//MyStepper stepper1(MyStepper::DRIVER, 7, 6);
//MyStepper stepper2(MyStepper::DRIVER, 9, 8);

const int timeStep = 100;  // ms
const float minReturnSpeed = 50;

// about 1600 = 1 revolution for stepper 1

// stepper 1 parameters
const long stepper1DistanceToMove = -3000;
const long stepper1TimeToComplete = 5;  // s
bool isStepper1PrintTimeStepToSerial = true;
bool isStepper1Stopped = false;

// stepper 2 parameters
const long stepper2DistanceToMove = -2000;
const long stepper2TimeToComplete = 5;  // s
bool isStepper2PrintTimeStepToSerial = false;
bool isStepper2Stopped = false;

void setup()
{
  Serial.begin(9600);

  stepper1.setMinReturnSpeed(minReturnSpeed);  // should be positive
  stepper1.reset(stepper1DistanceToMove, stepper1TimeToComplete);
  stepper1.setTimeStepInMillis(timeStep);
  stepper1.setIsPrintTimeStepToSerial(isStepper1PrintTimeStepToSerial);

  stepper2.setMinReturnSpeed(minReturnSpeed);  // should be positive
  stepper2.reset(stepper2DistanceToMove, stepper2TimeToComplete);
  stepper2.setTimeStepInMillis(timeStep);
  stepper2.setIsPrintTimeStepToSerial(isStepper2PrintTimeStepToSerial);

  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);


    pinMode(LimitSwitchLy, INPUT);
  pinMode(LimitSwitchRy, INPUT);

}

long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)

bool goHome = true;
bool goHome2 = true;

void loop()
{   
  bool isStepper1Complete = stepper1.isCompleteTotDist();
  bool isStepper2Complete = stepper2.isCompleteTotDist();
  
  if (!isStepper1Complete)
  {
    stepper1.myRun();
  }

  if (!isStepper2Complete)
  {
    stepper2.myRun();
  }

  if (isStepper1Complete && isStepper2Complete)
  {
    int nextStepper1Position = random(1000,4000);
    int nextStepper2Position = random(1000,4000);
    float nextTimespan = random(5, 10);
    Serial.println("Next Timespan: " + String(nextTimespan, DEC));
    Serial.println("Next Pos 1: " + String(nextStepper1Position, DEC));
    Serial.println("Next Pos 2: " + String(nextStepper2Position, DEC));
    stepper1.reset(-nextStepper1Position, nextTimespan);
    stepper2.reset(-nextStepper2Position, nextTimespan);
  }




  //HOME
  
    if (goHome) {
      int reading = digitalRead(LimitSwitchLy);
      buttonState = reading;
      //   digitalWrite(ledPin, buttonState);
      stepper1.setMaxSpeed(homeMaxSpeed);
        stepper1.setSpeed(-homeSpeed);
     
      if (buttonState) {

        Serial.println("Reached Home L MOTOR");
        stepper1.setSpeed(0);
        stepper1.setCurrentPosition(0);
        goHome = false;
      }
      stepper1.runSpeed();
    }
    if (goHome2) {
      int reading = digitalRead(LimitSwitchRy);
      buttonState2 = reading;
      //   digitalWrite(ledPin, buttonState);
      stepper2.setMaxSpeed(homeMaxSpeed);
        stepper2.setSpeed(-homeSpeed);
   
      if (buttonState2) {

        Serial.println("Reached Home R MOTOR");
        stepper2.setSpeed(0);
        stepper2.setCurrentPosition(0);
        goHome2 = false;
      }
      stepper2.runSpeed();
    }

}
