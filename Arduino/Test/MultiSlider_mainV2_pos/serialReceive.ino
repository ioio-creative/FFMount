void receiveSerial() {
  if (Serial2.available()) {

    if (Serial2.read() == 'n') {

      Serial2.readBytes((char *)&myfloat, sizeof(float));
      //Serial.println(myfloat);

      if (myfloat > -90 && myfloat < 90 && (myfloat >= 0.01  || myfloat <= -0.01)) {
        receivedAngle = myfloat;
        //Serial.println(receivedAngle);
        // subtract the last reading:
        total = total - readings[readIndex];
        // read from the sensor:
        readings[readIndex] = receivedAngle;
        // add the reading to the total:
        total = total + readings[readIndex];
        // advance to the next position in the array:
        readIndex = readIndex + 1;

        // if we're at the end of the array...
        if (readIndex >= numReadings) {
          // ...wrap around to the beginning:
          readIndex = 0;
        }

        // calculate the average:
        average = total / numReadings;
        // send it to the computer as ASCII digits
        //   Serial.println(average);

        currentRot = (int)map(average, -20, 20, 0, 180);
        // Serial.println(currentRot);
        if (debug) {

        } else {
          Serial.write(currentRot);
        }


      }

    }


    Serial2.flush();

  }
}
