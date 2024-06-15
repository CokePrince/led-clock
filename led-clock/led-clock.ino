#include <Adafruit_NeoPixel.h>
#include <TimeLib.h>  // Include the Time library

#define PIN            6  // Define the pin to which WS2812 LEDs are connected
#define NUMPIXELS      60 // Define the number of WS2812 LEDs
#define BRIGHTNESS     50 // Set the brightness

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// For demo only, shortcuts to set the colors of light and some time parameters
int secondColor = strip.Color(135, 206, 235);
int minuteColor = strip.Color(70, 130, 180);
int hourColor = strip.Color(0, 0, 128);

int breathColor = strip.Color(144, 238, 144);
int bTime = 30; // Break time, sets the interval between sets of breath, by second
int breathTimes = 3; // Sets how many breath times in a set of breathes
int breathRate = 8000; // How fast you breath, by millisecond
int animationSpeedUp = 1.5f; // A scaleFactor sets the variety of speed in nonlinear led lighting


int prevSecondLED, prevMinuteLED, prevHourLED = 0;  // Variable to store the previously lit LED index, set as 0 by default
int hours, hourLED, minuteLED, secondLED = 0; // Variable to store the time information, set as 0 by default

void setup() {

  //Serial.begin(9600); // Reserved for future functionalities impleentation, intended to receive parameters
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();  // Initialize all LEDs as off

  rainbowCycle(10);
  clearAll();
}

void loop() {
  daily(bTime, breathTimes, animationSpeedUp); // Demonstrated in daily (clock intervals eith breathes) mode
}

void clock() {

  // Save the previous LED index
  prevSecondLED = secondLED;
  prevMinuteLED = minuteLED;
  prevHourLED = hourLED;

  // Get the current time
  hours = hour();
  minuteLED = minute();
  secondLED = second();

  // Calculate the corresponding LED indices
  int hourLED = map(hours % 12, 0, 11, 0, 59);
  //int minuteLED = map(minutes, 0, 60, 0, 59);
  //int secondLED = map(seconds, 0, 60, 0, 59);

  // Turn off the previously lit LED
  strip.setPixelColor(prevSecondLED, strip.Color(0, 0, 0));
  if (minuteLED != prevMinuteLED) strip.setPixelColor(prevMinuteLED, strip.Color(0, 0, 0)); 
  if (hourLED != prevHourLED) strip.setPixelColor(prevHourLED, strip.Color(0, 0, 0));

  // Turn on the corresponding LEDs
  strip.setPixelColor(secondLED, secondColor);
  strip.setPixelColor(minuteLED, minuteColor); 
  strip.setPixelColor(hourLED, hourColor); 

  strip.show();  // Update the LED state
  delay(1000);   // Delay for 1 second

  // Store the current second LED index as the previously lit LED for the next iteration
  //prevSecondLED = secondLED;
}

// Parameter rate defines the breath rate and scaleFactor sets the variety of speed in nonlinear led lighting
void breath(int rate, float scaleFactor) {

  for (int led = 0; led <= 59; led++) {
    strip.setPixelColor(led, breathColor);
    strip.show();
    float speedFactor = 1 + sqrt(led)/scaleFactor;
    delay(rate/600 * speedFactor);
  }

  delay(rate/60);

  for (int led = 59; led >= 0; led--) {
    strip.setPixelColor(led, strip.Color(0, 0, 0));
    strip.show();
    float speedFactor = 1 + sqrt(59-led)/scaleFactor;
    // *1.2 means exhaling at lower speed
    delay(rate/600 * speedFactor * 1.2);
  }

}

// Set all leds as off
void clearAll() {
  for (int led = 0; led <= 59; led++) {
    strip.setPixelColor(led, strip.Color(0, 0, 0));
    strip.show();
  }
}

// BreakTime sets the interval between sets of breath, and breathTimes how many breath times in a set of breathes 
void daily(int breakTime, int breathTimes, float scaleFactor) {
  while(true) {
    int timer = 0;
    while(timer < breakTime) {
      clock();
      timer++;
    }
    clearAll();
    for (int i = 1; i <= breathTimes; i++) {
      breath(3500*sqrt(i), scaleFactor);
    }
    timer = 0;
  }

}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}