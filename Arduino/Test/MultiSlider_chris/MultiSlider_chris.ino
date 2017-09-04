/*
 * Arduino Ver 1.8.1
 * 
 * AccelStepper Library (1.57)
 * http://www.airspayce.com/mikem/arduino/AccelStepper/AccelStepper-1.57.zip
 */

#include <AccelStepper.h>

long speedL1 = 100000;
long accelL1 = 1000;
long targetPosL1 = 100000;
long targetPosIncL1 = 1000;
long accelIncL1 = 1000;
long lastFinishTimeL1 = 0;
AccelStepper stepperL1(AccelStepper::DRIVER, 7, 6);

int speedL2 = 3500;
int accelL2 = 4000;
int targetPosL2 = 10000;
AccelStepper stepperL2(AccelStepper::DRIVER, 9, 8);

int initialDelay = 5000;
bool isPrintL1 = false;
bool isPrintL2 = false;

void setup()
{
  Serial.begin(9600);

  stepperL1.setMaxSpeed(speedL1);
  stepperL1.setAcceleration(accelL1);  
  stepperL1.moveTo(targetPosL1);  

//  stepperL2.setMaxSpeed(speedL2);
//  stepperL2.setAcceleration(accelL2);
//  stepperL2.moveTo(targetPosL2);

  delay(initialDelay);
}

void loop()
{
  //printPositionAndSpeedWithTime();
  stepperL1.run();
  //stepperL2.run();

  // Run once
//  if (stepperL1.distanceToGo() == 0 && !isPrintL1)
//  {
//    Serial.println(String(accelL1) + ", " + String(millis() - initialDelay, DEC) + ", stepperL1 done.");
//    isPrintL1 = true;
//  }

  // Vary target distance to move
//  if (stepperL1.distanceToGo() == 0 && targetPosL1 <= 10000)
//  {
//    Serial.println(targetPosL1 + ", " + String(millis() - initialDelay, DEC) + ", stepperL1 done.");    
//    targetPosL1 += targetPosIncL1;
//    stepperL1.moveTo(stepperL1.currentPosition() + targetPosL1);
//  }

  // Vary acceleration
  if (stepperL1.distanceToGo() == 0 && accelL1 <= 10000)
  {
    long finishTime = millis() - initialDelay;
    long timeSinceLastFinishTime = finishTime - lastFinishTimeL1;
    lastFinishTimeL1 = finishTime;
    //Serial.println(String(accelL1, DEC) + ", " + String(timeSinceLastFinishTime, DEC) + ", stepperL1 done.");    
    Serial.println(String(timeSinceLastFinishTime, DEC));    
    accelL1 += accelIncL1;
    stepperL1.setAcceleration(accelL1);
    stepperL1.moveTo(stepperL1.currentPosition() + targetPosL1);
  }


//  if (stepperL2.distanceToGo() == 0 && !isPrintL2)
//  {
//    Serial.println(String(millis() - initialDelay, DEC) + ", stepperL2 done.");
//    isPrintL2 = true;
//  }
}

void printPositionAndSpeedWithTime()
{
  String strToPrint = String(millis(), DEC);
  strToPrint += ", ";   
  strToPrint += stepperL1.currentPosition();
  strToPrint += ", ";
  strToPrint += stepperL1.speed();
//  strToPrint += ", ";
//  strToPrint += stepperL2.currentPosition();
//  strToPrint += ", ";
//  strToPrint += stepperL2.speed();
  Serial.println(strToPrint);
}



//// MultiStepper.pde
//// -*- mode: C++ -*-
//// Use MultiStepper class to manage multiple steppers and make them all move to 
//// the same position at the same time for linear 2d (or 3d) motion.
//
//#include <AccelStepper.h>
//#include <MultiStepper.h>
//
//// EG X-Y position bed driven by 2 steppers
//// Alas its not possible to build an array of these with different pins for each :-(
//AccelStepper stepper1(AccelStepper::DRIVER, 7, 6);
//AccelStepper stepper2(AccelStepper::DRIVER, 9, 8);
//
//// Up to 10 steppers can be handled as a group by MultiStepper
//MultiStepper steppers;
//
//void setup() {
//  Serial.begin(9600);
//
//  // Configure each stepper
//  stepper1.setMaxSpeed(500);
//  stepper2.setMaxSpeed(500);
//
//  // Then give them to MultiStepper to manage
//  steppers.addStepper(stepper1);
//  steppers.addStepper(stepper2);
//}
//
//void loop() {
//  long positions[2]; // Array of desired stepper positions
//  
//  positions[0] = 1000;
//  positions[1] = 1000;
//  steppers.moveTo(positions);
//  steppers.runSpeedToPosition(); // Blocks until all are in position
//  delay(1000);
//
//  Serial.println("Move to position 1 done.");  
//  
//  // Move to a different coordinate
//  positions[0] = -100;
//  positions[1] = -100;
//  steppers.moveTo(positions);
//  steppers.runSpeedToPosition(); // Blocks until all are in position
//  delay(1000);
//  
//  Serial.println("Move to position 2 done.");  
//}


