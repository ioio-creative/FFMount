void alarm_check() {

  //Servo Alarm
  if (!DO5_ALRM_ly || !DO5_ALRM_ry) {
    Serial.print("alarm");
    if (!DO5_ALRM_ly) {
      Serial.println(" ly ");
      digitalWrite(DI2_ALARM_RESET_ly, HIGH);
      delay(500);
      digitalWrite(DI2_ALARM_RESET_ly, LOW);
    }

    if (!DO5_ALRM_ry) {
      Serial.println(" ry ");
      digitalWrite(DI2_ALARM_RESET_ry, HIGH);
      delay(500);
      digitalWrite(DI2_ALARM_RESET_ry, LOW);
    }

  }

}
