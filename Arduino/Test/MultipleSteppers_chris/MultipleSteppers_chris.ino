#include <AccelStepper.h>

const long MAX_SPEED = 200000;

bool isStepper1Stopped = false;
bool isStepper2Stopped = false;

AccelStepper stepper1(AccelStepper::DRIVER, 7, 6);
AccelStepper stepper2(AccelStepper::DRIVER, 9, 8);

void setup() {
  Serial.begin(9600);
  
  stepper1.setMaxSpeed(MAX_SPEED);
  stepper1.setAcceleration(computeAcceleration(20000, 20000));
  stepper1.moveTo(15000);
  
  stepper2.setMaxSpeed(MAX_SPEED);
  stepper2.setAcceleration(computeAcceleration(10000, 20000));
  stepper2.moveTo(10000);

//  stepper1.runToPosition();
//  Serial.println("Stepper 1 stopped at " + String(millis(), DEC));
//  stepper2.runToPosition();
//  Serial.println("Stepper 2 stopped at " + String(millis(), DEC));
}

void loop() {
  if (stepper1.distanceToGo() == 0 && !isStepper1Stopped)
  {
    Serial.println("Stepper 1 stopped at " + String(millis(), DEC));    
    isStepper1Stopped = true;
  }

  if (stepper2.distanceToGo() == 0 && !isStepper2Stopped)
  {
    Serial.println("Stepper 2 stopped at " + String(millis(), DEC));
    isStepper2Stopped = true;
  }
  
  stepper1.run();
  stepper2.run();
}

float computeAcceleration(long distanceToMove, long timeToMove)
{
  return pow(4.179267E-07 * (timeToMove - (0.24561 * distanceToMove)), -1.0793);
}

