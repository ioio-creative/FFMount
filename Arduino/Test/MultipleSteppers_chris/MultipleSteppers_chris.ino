#include <MyStepper.h>

MyStepper stepper1(7, 6);
MyStepper stepper2(9, 8);

const int timeStep = 1000;  // ms

// about 1600 = 1 revolution for stepper 1

// stepper 1 parameters
const long stepper1PositionToGo = 16000;
const long stepper1TimeToComplete = 30;  // s
bool isStepper1PrintTimeStepToSerial = true;
bool isStepper1Stopped = false;

// stepper 2 parameters
const long stepper2PositionToGo = 32000;
const long stepper2TimeToComplete = 30;  // s
bool isStepper2PrintTimeStepToSerial = true;
bool isStepper2Stopped = false;

void setup()
{  
  Serial.begin(9600);

  stepper1.reset(stepper1PositionToGo, stepper1TimeToComplete);
  stepper1.setTimeStepInMillis(timeStep);
  stepper1.setIsPrintTimeStepToSerial(isStepper1PrintTimeStepToSerial);

  stepper2.reset(stepper2PositionToGo, stepper2TimeToComplete);
  stepper2.setTimeStepInMillis(timeStep);
  stepper2.setIsPrintTimeStepToSerial(isStepper2PrintTimeStepToSerial);
}

void loop()
{
  // Use compareAbs to cater for negative distances
  if (!stepper1.isCompleteTotDist())
  {
    stepper1.myRun();
  }
  else if (!isStepper1Stopped)
  {
    stepper1.printStatusToSerial();
    Serial.println("Stepper 1 stopped");
    isStepper1Stopped = true;
  }

  
  // Use compareAbs to cater for negative distances
  if (!stepper2.isCompleteTotDist())
  {
    stepper2.myRun();
  }
  else if (!isStepper2Stopped)
  {
    stepper2.printStatusToSerial();
    Serial.println("Stepper 2 stopped");
    isStepper2Stopped = true;
  }
}
