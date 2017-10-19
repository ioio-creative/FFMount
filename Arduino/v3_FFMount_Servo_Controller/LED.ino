#include <FastLED.h>

// ============ LED ================

#define COLOR_ORDER BRG
#define CHIPSET     WS2811
#define NUM_LEDS    34

#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 30

//CRGB leds_ly[NUM_LEDS];
//CRGB leds_ry[NUM_LEDS];


CRGBArray<NUM_LEDS> leds_ly;
CRGBArray<NUM_LEDS> leds_ry;

int currLED;

bool gReverseDirection = false;


// ============ LED Style Setup and Loop================
//LED
int curLEDStyle = 1;

long previousMillis1 = 0;
long previousMillis2 = 0;

bool LEDincrement_ly = true;
bool LEDincrement_ry = true;

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

  led_style(curLEDStyle, LEDFactor0, LEDFactor1, LEDFactor2, LEDFactor3);

  if (currentMillis - previousMillis1 > (1000 / FRAMES_PER_SECOND)) {
    previousMillis1 = currentMillis;
    FastLED.show(); // display this frame
  }

}

//FORMAT::: intervalTime(30) - fade(30) - MinRange / multiplyRate - MaxRange
//1 - intervalTime(30) - fade(30) ||| fade out
//2 - intervalTime(30) ||| fade in
//3 - empty - empty - rangeMin - rangeMax(5000) ||| brightness according to motor val
//4 - empty - fade(30) - rangeMin - rangeMax(5000)||| brightness according to motor val, furthest LED brightest
//5 - intervalTime(30) - fade(10) ||| LED trail shooting out
//6 - intervalTime(30) - fade(50) - rangeMin - rangeMax(5000) ||| one glowing spot, pos according to motor
//7 - intervalTime(30) - fade(25) - multiplyRate (15) ||| many glowing spot, pos according to motor


void led_style(int s, long f0, long f1, long f2, long f3)
{
  if (s == 1) { //fade out - 21 - intervalTime(30) - fade(30)
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
  } else if (s == 2) { //fade in - 22 - intervalTime(30)

    int intervalTime = f0;
    int minVal = f1;
    if (minVal == 0) {
      minVal = 0;
    }

    if (intervalTime == 0) {
      intervalTime = 30;
    }

    if (currentMillis - previousMillis2 > intervalTime) {
      previousMillis2 = currentMillis;
      for (int i = 0; i < NUM_LEDS; i++) {
        int ll;
        int rr;
        if ( leds_ly[i].r >= maxR && leds_ly[i].g >= maxG && leds_ly[i].b >= maxB) {
          LEDincrement_ly = false;
        } else if ( leds_ly[i].r <= minVal && leds_ly[i].g <= minVal && leds_ly[i].b <= minVal) {
          LEDincrement_ly = true;
        }
        if (LEDincrement_ly) {
          ll = 1;
        } else {
          ll = -1;
        }

        if ( leds_ry[i].r >= maxR && leds_ry[i].g >= maxG && leds_ry[i].b >= maxB) {
          LEDincrement_ry = false;
        } else if ( leds_ry[i].r <= minVal && leds_ry[i].g <= minVal && leds_ry[i].b <= minVal) {
          LEDincrement_ry = true;
        }
        if (LEDincrement_ry) {
          rr = 1;
        } else {
          rr = -1;
        }

        leds_ly[i].r += ll;
        leds_ly[i].g += ll;
        leds_ly[i].b += ll;

        leds_ry[i].r += rr;
        leds_ry[i].g += rr;
        leds_ry[i].b += rr;

      }
    }
  }
  else if (s == 3) { //brightness according to motor val

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
  else if (s == 4) {//brightness according to motor val, furthest LED brightest
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

  } else if (s == 5) {//LED trail shooting out

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
  } else if (s == 6) { //one glowing spot, pos according to motor

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
  else if (s == 7) {  //many glowing spot, pos according to motor


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

