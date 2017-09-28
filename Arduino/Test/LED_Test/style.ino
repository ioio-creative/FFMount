



long previousMillis1 = 0;
long previousMillis2 = 0;

unsigned long currentMillis = millis();

void setupLED() {

  delay(3000); // sanity delay
  FastLED.addLeds<CHIPSET, LED_ly, COLOR_ORDER>(leds_ly, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<CHIPSET, LED_ry, COLOR_ORDER>(leds_ry, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  for (int i = 0; i < NUM_LEDS; i++) {
    leds_ly[i] = 0xFF0000;
  }
}

void loopLED() {
  currentMillis = millis();

  led_style(currentStyle);

  if (currentMillis - previousMillis1 > (1000 / FRAMES_PER_SECOND)) {
    previousMillis1 = currentMillis;
    FastLED.show(); // display this frame
  }

}

void led_style(int s)
{
  if (s == 0) { //fade out
    if (currentMillis - previousMillis2 > 30) {
      previousMillis2 = currentMillis;
      for (int i = 0; i < NUM_LEDS; i++) {
        leds_ly[i].fadeToBlackBy( 40 );
      }
    }
  } else if (s == 1) { //fade in

  }
  else if (s == 2) { //brightness according to motor val
    int m;
    for (int i = 0; i < NUM_LEDS; i++) {
      m  = map(stepperPos[0], 0, 5000, 0, 255);
      leds_ly[i] = CRGB(m, m, m);
    }
  }
  else if (s == 3) {//brightness according to motor val, furthest LED brightest
    int m;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds_ly.fadeToBlackBy(4);
      m  = map(stepperPos[0], 0, 5000, 0, 255);
      leds_ly[i] = CRGB(m * ((float)i / NUM_LEDS), m * ((float)i / NUM_LEDS), m * ((float)i / NUM_LEDS));
    }

  } else if (s == 4) {//LED trail shooting out
    if (currLED > NUM_LEDS - 1) {
      currLED = 0;
    }
    if (currentMillis - previousMillis2 > 30) {
      previousMillis2 = currentMillis;

      leds_ly.fadeToBlackBy(10);
      leds_ly[currLED++] = CRGB(255, 255, 255);
    }
  } else if (s == 5) { //one glowing spot, pos according to motor
    if (currentMillis - previousMillis2 > 30) {
      previousMillis2 = currentMillis;

      leds_ly.fadeToBlackBy(10);
      leds_ly[map(stepperPos[0],0,5000,0,NUM_LEDS)] = CRGB(255, 255, 255);
    }
  }
  else if (s == 6) {  //one glowing spot, pos according to motor

  }

}

