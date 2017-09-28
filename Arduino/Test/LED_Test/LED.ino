#include <FastLED.h>

// ============ LED ================

#define LED_ly 6
#define LED_ry 7

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
