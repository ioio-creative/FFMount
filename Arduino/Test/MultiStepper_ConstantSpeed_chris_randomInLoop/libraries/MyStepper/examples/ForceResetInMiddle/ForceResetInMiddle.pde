#include <MyStepper.h>

MyStepper stepper1(MyStepper::DRIVER, 7, 6);

const int timeStep = 100;  // ms

// about 1600 = 1 revolution for stepper 1

// stepper 1 parameters
const long stepper1PositionToGo = 10000;
const float stepper1TimeToComplete = 5;  // s
bool isPrintTimeStepToSerial = true;
const float minReturnSpeed = 0;

const float timeToInterrupt = 4;
bool isProcessReset = false;
bool isProcessStopped = false;

void setup()
{  
  Serial.begin(9600);

  stepper1.setMinReturnSpeed(minReturnSpeed);  // should be positive
  stepper1.reset(stepper1PositionToGo, stepper1TimeToComplete);
  stepper1.setTimeStepInMillis(timeStep);
  stepper1.setIsPrintTimeStepToSerial(isPrintTimeStepToSerial);
}

void loop()
{
  if (!isProcessReset && stepper1.getTimeSinceLastResetInMillis() > timeToInterrupt * 1000)
  {
    stepper1.printStatusToSerial();
    Serial.println("Reset");
    stepper1.reset(-stepper1PositionToGo, stepper1TimeToComplete);
    isProcessReset = true;
  }
  else if (!stepper1.isCompleteTotDist())
  {
    stepper1.myRun();
  }
  else if (!isProcessStopped)
  {
    stepper1.printStatusToSerial();
    Serial.println("Stopped");    
    isProcessStopped = true;
  }
}