#include <AccelStepper.h>

int speedL1 = 3500;
int accelL1 = 1800;
AccelStepper stepperL1(AccelStepper::DRIVER, 7, 6);

int speedL2 = 3500;
int accelL2 = 1800;
AccelStepper stepperL2(AccelStepper::DRIVER, 9, 8);



void setup()
{
  /* stepper1.setMaxSpeed(200.0);
    stepper1.setAcceleration(100.0);
    stepper1.moveTo(24);

    stepper2.setMaxSpeed(300.0);
    stepper2.setAcceleration(100.0);
    stepper2.moveTo(1000000);*/

  Serial.begin(9600);


 // delay(5000);
  stepperL1.setMaxSpeed(speedL1);
  stepperL1.setAcceleration(accelL1);

  stepperL2.setMaxSpeed(speedL2);
  stepperL2.setAcceleration(accelL2);

}

void loop()
{
 
    if (stepperL1.distanceToGo() == 0) {
      stepperL1.moveTo(random(1000, 5000));
    }

    stepperL1.run();


    if (stepperL2.distanceToGo() == 0) {
      //stepperL2.moveTo(random(-1000, -5000));
      stepperL2.moveTo((random(1000, 5000)));
    }

    stepperL2.run();

}


