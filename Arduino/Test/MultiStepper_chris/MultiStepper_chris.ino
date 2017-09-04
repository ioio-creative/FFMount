#include <AccelStepper.h>
#include <MultiStepper.h>

AccelStepper stepper1(AccelStepper::DRIVER, 7, 6);
AccelStepper stepper2(AccelStepper::DRIVER, 9, 8);

MultiStepper steppers;

void setup() {
  Serial.begin(9600);

  // Configure each stepper
  stepper1.setMaxSpeed(500);
  stepper2.setMaxSpeed(500);

  // Then give them to MultiStepper to manage
  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);
}

void loop() {
  long positions[2]; // Array of desired stepper positions
  
  positions[0] = 10000;
  positions[1] = 5000;
  steppers.moveTo(positions);
  steppers.runSpeedToPosition(); // Blocks until all are in position
  delay(1000);
  
//  // Move to a different coordinate
//  positions[0] = -100;
//  positions[1] = 100;
//  steppers.moveTo(positions);
//  steppers.runSpeedToPosition(); // Blocks until all are in position
//  delay(1000);
}

