#include <AccelStepper.h>

AccelStepper stepper1(AccelStepper::DRIVER, 7, 6);
AccelStepper stepper2(AccelStepper::DRIVER, 9, 8);

const long maxSpeed = 100000;
const int timeStep = 1000;  // ms
long lastTimeStamp = 0;  // ms
long currentTimeStamp = 0;
int timeDiff = 0;


// about 1600 = 1 revolution for stepper 1


// stepper 1 parameters
const long stepper1InitialSpeed = 0;
const long stepper1DistanceToMove = 16000;
const long stepper1HalfDistanceToMove = stepper1DistanceToMove / 2;
const long stepper1TimeToComplete = 30;  // s
float stepper1Accl = 0;
bool isStepper1EndTimePrinted = false;
long stepper1CurrPos = 0;

// stepper2 parameters
const long stepper2InitialSpeed = 0;
const long stepper2DistanceToMove = 8000;
const long stepper2HalfDistanceToMove = stepper2DistanceToMove / 2;
const long stepper2TimeToComplete = 15;  // s
float stepper2Accl = 0;
bool isStepper2EndTimePrinted = false;
long stepper2CurrPos = 0;

void setup()
{  
  Serial.begin(9600);
  
  stepper1.setMaxSpeed(maxSpeed);
  stepper1.setSpeed(stepper1InitialSpeed);
  stepper1Accl = computeLinearAccl(stepper1DistanceToMove, stepper1TimeToComplete);
  
  stepper2.setMaxSpeed(maxSpeed);
  stepper2.setSpeed(stepper2InitialSpeed);
  stepper2Accl = computeLinearAccl(stepper2DistanceToMove, stepper2TimeToComplete);
}

void loop()
{  
  stepper1CurrPos = stepper1.currentPosition();
  stepper2CurrPos = stepper2.currentPosition();
  currentTimeStamp = millis();
  timeDiff = currentTimeStamp - lastTimeStamp;
  
  if (stepper1CurrPos < stepper1DistanceToMove)
  {
    stepper1.runSpeed();
  }
  else if (!isStepper1EndTimePrinted)
  {
    Serial.println(String(currentTimeStamp * 0.001, DEC) + ", " + String(stepper1CurrPos, DEC));
    isStepper1EndTimePrinted = true;
  }

  if (-stepper2CurrPos < stepper2DistanceToMove)
  {
    stepper2.runSpeed();
  }
  else if (!isStepper2EndTimePrinted)
  {
    Serial.println(String(currentTimeStamp * 0.001, DEC) + ", " + String(stepper2CurrPos, DEC));
    isStepper2EndTimePrinted = true;
  }

  

  

  if (timeDiff > timeStep)
  {
    /* update stepper 1 */

    if (stepper1CurrPos < stepper1HalfDistanceToMove)
    {
      stepper1.setSpeed(stepper1.speed() + stepper1Accl * timeDiff * 0.001);
    }
    else
    {
      long stepper1AnticipatedSpeed = stepper1.speed() - stepper1Accl * timeDiff * 0.001;
      if (stepper1AnticipatedSpeed > 0)
      {
        stepper1.setSpeed(stepper1AnticipatedSpeed);
      }
      else
      {
        stepper1.setSpeed(0);
      }
    }
    
    //Serial.println(String(currentTimeStamp * 0.001, DEC) + ", " + String(stepper1.currentPosition(), DEC) + ", " + String(stepper1.speed(), DEC));

    /* end of update stepper 1 */


    /* update stepper 2 */    

    if (stepper2CurrPos < stepper2HalfDistanceToMove)
    {
      stepper2.setSpeed(stepper2.speed() + stepper2Accl * timeDiff * 0.001);
    }
    else
    {
      long stepper2AnticipatedSpeed = stepper2.speed() - stepper2Accl * timeDiff * 0.001;
      if (stepper2AnticipatedSpeed > 0)
      {
        stepper2.setSpeed(stepper2AnticipatedSpeed);
      }
      else
      {
        stepper2.setSpeed(0);
      }
    }
    
    //Serial.println(String(currentTimeStamp * 0.001, DEC) + ", " + String(stepper2.currentPosition(), DEC) + ", " + String(stepper2.speed(), DEC));

    /* end of update stepper 2 */
    

    lastTimeStamp = currentTimeStamp;

    Serial.println(
      String(stepper1.currentPosition(), DEC) + ", " +
      String(stepper1.speed(), DEC) + ", " +
      String(-stepper2.currentPosition(), DEC) + ", " +
      String(-stepper2.speed(), DEC));
  }
}

float computeLinearAccl(long totDist, long totTime)
{
    return (4.0 * totDist) / (totTime * totTime);
}


