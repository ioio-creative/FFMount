int curLEDStyle;

const int numOfStepper = 1;
long stepperPos[numOfStepper]  = {0};
long prevStepperPos[numOfStepper]  = {0};
long newStepperPos[numOfStepper]  = {0};

int inByte;

void setup() {
  for (int i = 0; i < numOfStepper; i++) {
    stepperPos[i] = random(0, 5000);
  }
  setupLED();
  Serial.begin(57600);
}
float t;
void loop() {
  for (int i = 0; i < numOfStepper; i++) {
    float dx = newStepperPos[i] - stepperPos[i] ;//swap X and Y for orientation
    //if (abs(dx) > 20) {
    if (t < 1) {
      stepperPos[i] += dx * easeInOut(t += 0.0002);
    } else {
      if (newStepperPos[i] >= 2500) {
        newStepperPos[i] = random(0, 2000);
      } else {

        newStepperPos[i] = random(3000, 5000);
      }

      prevStepperPos[i] = stepperPos[i];

      t = 0;
    }
   /* Serial.print(i);
    Serial.print(" : ");
    Serial.println(newStepperPos[i]);
    Serial.print(" : ");
    Serial.println(prevStepperPos[i]);
*/
  }
  loopLED();

 if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.parseInt();
    char inByte2 = Serial.read();
    Serial.println(inByte);
    curLEDStyle = inByte ;
  }
}


float easeInOut(float t) {
  return t > 0.5 ? 4 * pow((t - 1), 3) + 1 : 4 * pow(t, 3);
}

