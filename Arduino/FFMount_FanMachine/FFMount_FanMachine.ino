int led = 13;
int fan = 12;
int fanPower = A0;


void setup() {
  pinMode(led, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(fanPower, OUTPUT);
  Serial.begin(57600);

  digitalWrite(fan, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(fanPower, HIGH);
}

void loop() {




  while (Serial.available() > 0)
  {
    int inChar = Serial.read();
    // int inChar2 = Serial.parseInt();


    if (inChar == 'o') {
      digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
      digitalWrite(fan, HIGH);   // turn the LED on (HIGH is the voltage level)
      Serial.println("o");
    }
    if (inChar == 'f') {
      digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(fan, LOW);    // turn the LED off by making the voltage LOW
      Serial.println("f");
    }
  }

}
