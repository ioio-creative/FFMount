#include <MyStepper.h>

//MyStepper stepper1(MyStepper::DRIVER, 3, 2);
//MyStepper stepper2(MyStepper::DRIVER, 9, 8);

MyStepper stepper1(MyStepper::DRIVER, 3, 2);
MyStepper stepper2(MyStepper::DRIVER, 9, 8);

const int timeStep = 1000;  // ms

// about 1600 = 1 revolution for stepper 1

// stepper 1 parameters
const long stepper1DistanceToMove = 3000;
const long stepper1TimeToComplete = 5;  // s
bool isStepper1PrintTimeStepToSerial = false;
bool isStepper1Stopped = false;

// stepper 2 parameters
const long stepper2DistanceToMove = 32000;
const long stepper2TimeToComplete = 30;  // s
bool isStepper2PrintTimeStepToSerial = false;
bool isStepper2Stopped = false;

bool invertNum = true;

void setup()
{
  Serial.begin(9600);

  stepper1.reset(stepper1DistanceToMove, stepper1TimeToComplete);
  stepper1.setTimeStepInMillis(timeStep);
  stepper1.setIsPrintTimeStepToSerial(isStepper1PrintTimeStepToSerial);

  stepper2.reset(stepper2DistanceToMove, stepper2TimeToComplete);
  stepper2.setTimeStepInMillis(timeStep);
  stepper2.setIsPrintTimeStepToSerial(isStepper2PrintTimeStepToSerial);

  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);
}
long previousMillis = 0;        // will store last time LED was updated
long interval = 1000;           // interval at which to blink (milliseconds)


void loop()
{
  //  unsigned long currentMillis = millis();

  //  if (currentMillis - previousMillis > interval) {
  //    previousMillis = currentMillis;
  //
  //    Serial.print("distanceToGo : ");
  //    Serial.println(stepper1.distanceToGo());
  //  }
  int c = -1;

  if (!stepper1.isCompleteTotDist())
  {
    stepper1.myRun();
  }
  else {

    if (invertNum) {
      c = -1;
    } else {
      c = 1;
    }
    
    invertNum = !invertNum;
    
    Serial.println("Curr Pos : ");
    Serial.println(stepper1.currentPosition());
    int a = c * random(5000, 6000);
    int b = random(3, 5);
    Serial.print(" a : ");
    Serial.print(a);
    Serial.print(" b : ");
    Serial.println(b);

    stepper1.reset(a, b);
  }

  /*
    else if (!isStepper1Stopped)
    {
    stepper1.printStatusToSerial();
    Serial.println("Stepper 1 stopped");
    isStepper1Stopped = true;

    }
  */

  //  if (!stepper2.isCompleteTotDist())
  //  {
  //    stepper2.myRun();
  //  }
  //  else if (!isStepper2Stopped)
  //  {
  //    stepper2.printStatusToSerial();
  //    Serial.println("Stepper 2 stopped");
  //    isStepper2Stopped = true;
  //  }
}
