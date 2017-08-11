void stepper_style() {
  style = input_value[0];
  if (style == 1) {
    //STYLE - MOTOR - SPEED - ACCEL - POS
    int stepperNumber = input_value[1];
    stepperSpeed[stepperNumber] = input_value[2];
    stepperAccel[stepperNumber] = input_value[3];
    stepperPos[stepperNumber] = input_value[4];

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (steppers[stepperNumber]->distanceToGo() == 0) {
        steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
        steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
        steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos[stepperNumber]);
      }
    }
  }
  if (style == 2) {
    //STYLE - SPEED_X - ACCEL_X - SPEED_Y - ACCEL_Y - POS1_LX - POS2_LX - POS1_LY - POS2_LY - POS1_RX - POS2_RX - POS1_RY - POS2_RY

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (stepperNumber == 0 || stepperNumber == 2) {
        stepperSpeed[stepperNumber] = input_value[1];
        stepperAccel[stepperNumber] = input_value[2];
      } else {
        stepperSpeed[stepperNumber] = input_value[3];
        stepperAccel[stepperNumber] = input_value[4];
      }
      //abPos[stepperNumber] = true;
    }

    stepperPos1[0] = input_value[5];
    stepperPos2[0] = input_value[6];

    stepperPos1[1] = input_value[7];
    stepperPos2[1] = input_value[8];

    stepperPos1[2] = input_value[9];
    stepperPos2[2] = input_value[10];

    stepperPos1[3] = input_value[11];
    stepperPos2[3] = input_value[12];


    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (steppers[stepperNumber]->distanceToGo() == 0) {
        if (stepperPos1[stepperNumber] >= 0 && stepperPos2[stepperNumber] >= 0 && stepperAccel[stepperNumber] > 0 && stepperSpeed[stepperNumber] > 0) {
          steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
          steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
          if (abPos[stepperNumber]) {
            //   Serial.print(stepperNumber);
            //  Serial.println("A");
            steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos1[stepperNumber]);
          } else {
            //           Serial.print(stepperNumber);
            //   Serial.println("B");
            steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos2[stepperNumber]);
          }
          abPos[stepperNumber] = !abPos[stepperNumber];
        }
      }
    }
  }
  if (style == 11) {
    //STYLE - SPEED - ACCEL - POS
    stepperSpeed[0] = input_value[1];
    stepperAccel[0] = input_value[2];
    stepperPos[0] = input_value[3];

    stepperSpeed[1] = input_value[4];
    stepperAccel[1] = input_value[5];
    stepperPos[1] = input_value[6];

    stepperSpeed[2] = input_value[7];
    stepperAccel[2] = input_value[8];
    stepperPos[2] = input_value[9];

    stepperSpeed[3] = input_value[10];
    stepperAccel[3] = input_value[11];
    stepperPos[3] = input_value[12];

    for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
      if (steppers[stepperNumber]->distanceToGo() == 0) {
        if (stepperPos[stepperNumber] >= 0 && stepperAccel[stepperNumber] > 0 && stepperSpeed[stepperNumber] > 0) {
          steppers[stepperNumber]->setMaxSpeed(stepperSpeed[stepperNumber]);
          steppers[stepperNumber]->setAcceleration(stepperAccel[stepperNumber]);
          steppers[stepperNumber]->moveTo((inverseDir[stepperNumber])*stepperPos[stepperNumber]);
        }
      }
    }
  }
}
