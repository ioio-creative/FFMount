int led = 13;


void setup() {
  pinMode(led, OUTPUT); 
  Serial.begin(115200);  

    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
}

void loop() {




while (Serial.available() > 0)
  {
    int inChar = Serial.read();
   // int inChar2 = Serial.parseInt();
    

    if (inChar == 'o') {
digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    }
    if (inChar == 'f') {
 digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    }
  }

}
