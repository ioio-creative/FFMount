void alarm_check() {

  //Servo Alarm
  if (!DO5_ALRM_lx || !DO5_ALRM_ly || !DO5_ALRM_rx || !DO5_ALRM_ry) {
    Serial.print("alarm");
    if (!DO5_ALRM_lx) {
      Serial.println(" lx ");
      digitalWrite(DI2_ALARM_RESET_lx, HIGH);
      delay(500);
      digitalWrite(DI2_ALARM_RESET_lx, LOW);
    }
    if (!DO5_ALRM_ly) {
      Serial.println(" ly ");
      digitalWrite(DI2_ALARM_RESET_ly, HIGH);
      delay(500);
      digitalWrite(DI2_ALARM_RESET_ly, LOW);
    }
    if (!DO5_ALRM_rx) {
      Serial.println(" rx ");
      digitalWrite(DI2_ALARM_RESET_rx, HIGH);
      delay(500);
      digitalWrite(DI2_ALARM_RESET_rx, LOW);
    }
    if (!DO5_ALRM_ry) {
      Serial.println(" ry ");
      digitalWrite(DI2_ALARM_RESET_ry, HIGH);
      delay(500);
      digitalWrite(DI2_ALARM_RESET_ry, LOW);
    }



  }






}
