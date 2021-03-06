



long previousMillis1 = 0;
long previousMillis2 = 0;

int maxR = 255;
int maxG = 255;
int maxB = 255;

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

  led_style(curLEDStyle, 0, 0, 0, 0);

  if (currentMillis - previousMillis1 > (1000 / FRAMES_PER_SECOND)) {
    previousMillis1 = currentMillis;
    FastLED.show(); // display this frame
  }

}
//FORMAT::: intervalTime(30) - fade(30) - MinRange / multiplyRate - MaxRange
//20 - intervalTime(30) - fade(30) ||| fade out
//21 - intervalTime(30) ||| fade in
//22 - empty - empty - rangeMin - rangeMax(5000) ||| brightness according to motor val
//23 - empty - fade(30) - rangeMin - rangeMax(5000)||| brightness according to motor val, furthest LED brightest
//24 - intervalTime(30) - fade(10) ||| LED trail shooting out
//25 - intervalTime(30) - fade(50) - rangeMin - rangeMax(5000) ||| one glowing spot, pos according to motor
//26 - intervalTime(30) - fade(25) - multiplyRate (15) ||| many glowing spot, pos according to motor

    
void led_style(int s, long f0, long f1, long f2, long f3)
{
  if (s == 0) { //fade out - 20 - intervalTime(30) - fade(30)
    int intervalTime = f0;
    int fade = f1;
    if (intervalTime == 0) {
      intervalTime = 30;
    }
    if (fade == 0) {
      fade = 30;
    }

    if (currentMillis - previousMillis2 > intervalTime) {
      previousMillis2 = currentMillis;
      for (int i = 0; i < NUM_LEDS; i++) {
        leds_ly[i].fadeToBlackBy( fade );
        leds_ry[i].fadeToBlackBy( fade );
      }
    }
  } else if (s == 1) { //fade in - 21 - intervalTime(30)

    int intervalTime = f0;
    if (intervalTime == 0) {
      intervalTime = 30;
    }

    if (currentMillis - previousMillis2 > intervalTime) {
      previousMillis2 = currentMillis;
      for (int i = 0; i < NUM_LEDS; i++) {
        if (leds_ly[i].r < maxR && leds_ly[i].g < maxG && leds_ly[i].b < maxB) {
          leds_ly[i]++;
        }
        if (leds_ry[i].r < maxR && leds_ry[i].g < maxG && leds_ry[i].b < maxB) {
          leds_ry[i]++;
        }


      }
    }
  }
  else if (s == 2) { //brightness according to motor val

    int rangeMin = f2;
    int rangeMax = f3;

    if (rangeMax == 0) {
      rangeMax = 5000;
    }

    int m, n;
    for (int i = 0; i < NUM_LEDS; i++) {
      m  = map(stepperPos[0], rangeMin, rangeMax, 0, 255);
      leds_ly[i] = CRGB(m, m, m);
      n  = map(stepperPos[1], rangeMin, rangeMax, 0, 255);
      leds_ry[i] = CRGB(n, n, n);
    }
  }
  else if (s == 3) {//brightness according to motor val, furthest LED brightest
    int fade = f1;
    int rangeMin = f2;
    int rangeMax = f3;
    

    if (rangeMax == 0) {
      rangeMax = 5000;
    }

    if (fade == 0) {
      fade = 10;
    }


    int m, n;
    for (int i = 0; i < NUM_LEDS; i++) {
      m  = map(stepperPos[0], rangeMin, rangeMax, 0, 255);
      n  = map(stepperPos[1], rangeMin, rangeMax, 0, 255);

      leds_ly.fadeToBlackBy(fade);
      leds_ry.fadeToBlackBy(fade);

      leds_ly[i] = CRGB(m * ((float)i / NUM_LEDS), m * ((float)i / NUM_LEDS), m * ((float)i / NUM_LEDS));
      leds_ry[i] = CRGB(n * ((float)i / NUM_LEDS), n * ((float)i / NUM_LEDS), n * ((float)i / NUM_LEDS));
    }

  } else if (s == 4) {//LED trail shooting out

    int intervalTime = f0;
    int fade = f1;

    if (intervalTime == 0) {
      intervalTime = 30;
    }
    if (fade == 0) {
      fade = 10;
    }

    if (currLED > NUM_LEDS - 1) {
      currLED = 0;
    }
    if (currentMillis - previousMillis2 > intervalTime) {
      previousMillis2 = currentMillis;

      leds_ly.fadeToBlackBy(fade);
      leds_ly[currLED++] = CRGB(maxR, maxG, maxB);

      leds_ry.fadeToBlackBy(fade);
      leds_ry[currLED++] = CRGB(maxR, maxG, maxB);
    }
  } else if (s == 5) { //one glowing spot, pos according to motor

    int intervalTime = f0;
    int fade = f1;
    int rangeMin = f2;
    int rangeMax = f3;

    if (intervalTime == 0) {
      intervalTime = 30;
    }
    if (fade == 0) {
      fade = 50;
    }
    if (rangeMax == 0) {
      rangeMax = 5000;
    }

    if (currentMillis - previousMillis2 > intervalTime) {
      previousMillis2 = currentMillis;

      leds_ly.fadeToBlackBy(fade); // larger is faster
      leds_ry.fadeToBlackBy(fade); // larger is faster
      leds_ly[map(stepperPos[0], rangeMin, rangeMax, 0, NUM_LEDS)] = CRGB(maxR, maxG, maxB);
      leds_ry[map(stepperPos[1], rangeMin, rangeMax, 0, NUM_LEDS)] = CRGB(maxR, maxG, maxB);
    }
  }
  else if (s == 6) {  //many glowing spot, pos according to motor


    int intervalTime = f0;
    int fade = f1;
    int multiplyRate = f2;

    if (intervalTime == 0) {
      intervalTime = 30;
    }
    if (fade == 0) {
      fade = 25;
    }
    if (multiplyRate == 0) {
      multiplyRate = 15;
    }

    if (currentMillis - previousMillis2 > intervalTime) {
      previousMillis2 = currentMillis;

      leds_ly.fadeToBlackBy(fade);
      leds_ry.fadeToBlackBy(fade);
      if (currLED > NUM_LEDS - 1) {
        currLED = 0;
      }
      leds_ly[random(0, NUM_LEDS)] *= multiplyRate;
      for (int i = 0; i < NUM_LEDS; i++) {
        if (leds_ly[i].r == 0 && leds_ly[i].g == 0 && leds_ly[i].b == 0) {
          leds_ly[i++] = CRGB(maxR / multiplyRate, maxG / multiplyRate, maxB / multiplyRate);
        }
        if (leds_ry[i].r == 0 && leds_ry[i].g == 0 && leds_ry[i].b == 0) {
          leds_ry[i++] = CRGB(maxR / multiplyRate, maxG / multiplyRate, maxB / multiplyRate);
        }
      }


    }
  }

}

