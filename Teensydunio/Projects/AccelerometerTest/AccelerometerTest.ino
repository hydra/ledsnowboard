/*  OctoWS2811 BasicTest.ino - Basic RGB LED Test
    http://www.pjrc.com/teensy/td_libs_OctoWS2811.html
    Copyright (c) 2013 Paul Stoffregen, PJRC.COM, LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

  Required Connections
  --------------------
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4 - Do not use
    pin 3 - Do not use as PWM.  Normal use is ok.

  This test is useful for checking if your LED strips work, and which
  color config (WS2811_RGB, WS2811_GRB, etc) they require.
*/

#include <OctoWS2811.h>

#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelgyro;

const int ledsPerStrip = 30;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);


#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define YELLOW 0xFFFF00
#define PINK   0xFF1088
#define ORANGE 0xE05800
#define WHITE  0xFFFFFF
#define BLACK  0x000000

bool blinkState = false;
#define LED_PIN 13

void setup() {
  pinMode(LED_PIN, OUTPUT);
  
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

// initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(115200);

    digitalWrite(LED_PIN, false);

    // initialize device
    Serial.println("Initializing I2C devices...");
    while (true) {
      accelgyro.initialize();
  
      // verify connection
      Serial.println("Testing device connections...");
      if (accelgyro.testConnection()) {
        Serial.println("MPU6050 connection successful");
        break;
      } else {
        Serial.println("MPU6050 connection failed");
        int togglesLeft = 5;
        while(togglesLeft--) {
              blinkState = !blinkState;
              digitalWrite(LED_PIN, blinkState);
              delay(500);
        }
      }
    }
    digitalWrite(LED_PIN, true);
    
  leds.begin();
  ledFill(BLACK);
  leds.show();
}

int16_t ax, ay, az;
int16_t gx, gy, gz;

int16_t minAx = 0, maxAx = 0;


void loop() {
  
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
/*
  Serial.print("a/g:\t");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz);
*/
  autoCalibrate();

  int16_t range = maxAx - minAx;
  int16_t rangeOffset = range - ax + minAx;
  int rangePercent = (100L * (int32_t)rangeOffset) / range;

/*  
  Serial.print("minAx: "); Serial.print(minAx); Serial.print("\t");
  Serial.print("maxAx: "); Serial.print(maxAx); Serial.print("\t");
  Serial.print("range: "); Serial.print(range); Serial.print("\t");
  Serial.print("rangeOffset: "); Serial.print(rangeOffset); Serial.print("\t");
  Serial.print("rangePercent: "); Serial.print(rangePercent); Serial.print("\t");
*/
  int xPos = ((ledsPerStrip-1) * (int32_t)rangeOffset) / range;
  if (xPos > ledsPerStrip-1) {
    xPos = ledsPerStrip-1;
  }
  xPos = (ledsPerStrip-1) - xPos;
  //Serial.print("xPos: "); Serial.print(xPos); Serial.print("\t");

  ledFill(BLACK);
  ledVerticalLine(WHITE, xPos);
  
  //Serial.println();


  
  // blink LED to indicate activity
  //blinkState = !blinkState;
  //digitalWrite(LED_PIN, blinkState);
  
  
  leds.show();
  delayMicroseconds(1000);
}

void autoCalibrate() {
  if (ax < minAx) {
    minAx = ax;
  }
  if (ax > maxAx) {
    maxAx = ax;
  }
}

void ledFill(int color)
{
  for (int i=0; i < leds.numPixels(); i++) {
    leds.setPixel(i, color);
  }
}

void ledVerticalLine(int color, int x)
{
  for (int i = x; i < leds.numPixels(); i+= ledsPerStrip) {
    leds.setPixel(i, color);
  }
}
