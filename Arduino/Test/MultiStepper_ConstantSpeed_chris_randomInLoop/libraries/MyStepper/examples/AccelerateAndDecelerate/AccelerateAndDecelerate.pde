#include <MyStepper.h>

MyStepper stepper1(MyStepper::DRIVER, 7, 6);

const int timeStep = 100;  // ms

// about 1600 = 1 revolution for stepper 1

// stepper 1 parameters
const long stepper1PositionToGo = 5000;
const float stepper1TimeToComplete = 10;  // s
bool isPrintTimeStepToSerial = false;
const float minReturnSpeed = 0;

void setup()
{  
  Serial.begin(9600);

  stepper1.setMinReturnSpeed(minReturnSpeed);  // should be positive
  stepper1.setTimeStepInMillis(timeStep);
  stepper1.setIsPrintTimeStepToSerial(isPrintTimeStepToSerial);
  stepper1.reset(stepper1PositionToGo, stepper1TimeToComplete);
}

void loop()
{
  if (!stepper1.isCompleteTotDist())
  {
    stepper1.myRun();
  }
  else
  {
    stepper1.printStatusToSerial();
    Serial.println("Stopped");
    stepper1.reset(stepper1.currentPosition() == 0 ? stepper1PositionToGo : 0, stepper1TimeToComplete);    
  }
}