
#include "WProgram.h"

PROGMEM prog_uchar animation1[] = { 86,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,1,0,255,255,255,255,0,0,0,0,0,0,1,0,255,255,0,0,0,0,255,255,0,0,1,0,
        156,0,0,9,100,1,0,0,0,1,156,0,255,1,0,1,2,34,1,1,3,1,2,34,2,34,1,1,255,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,
        0,0,1,1,255,0,0,1,1,255,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,255,0,0,69 };

#include <OctoWS2811.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelGyro;

#define LEDS_PER_STRIP 2

DMAMEM int displayMemory[LEDS_PER_STRIP * 6];
int drawingMemory[LEDS_PER_STRIP * 6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds( LEDS_PER_STRIP, displayMemory, drawingMemory, config);
bool blinkState = false;

struct valueAxis {
    int lowValue;
    int highValue;
    int zeroValue;
}typedef ValueAxis;

// Frame Types
#define FT_COLOUR 1
#define FT_FUNCTION 2
#define FT_LINKED 3

#define INITIAL_LED 1

#define HEADER_BYTE 0x56
#define TERMINATING_BYTE 0x45
#define ESCAPE_BYTE 0x02
#define XOR_BYTE 0x20

#define MILLISECONDS_IN_A_SECOND 1000
#define MICROSECONDS_IN_A_MILLISECOND 1000
#define MICROSECONDS_IN_A_SECOND MILLISECONDS_IN_A_SECOND * MICROSECONDS_IN_A_MILLISECOND

unsigned char iValueAxisData[16][201];

signed int iFunctions[10][3];

#define LED_PIN 13

int iCounter;
int iTimeAxisNum;
int iNumValueAxes;
int numLeds;

int iTopOfTimeAxis;

int iNumFunctions;

signed char iValueAxisLowValue;
signed char iValueAxisHighValue;
signed char iValueAxisZeroValue;

int iValueAxisOffset;

int iTimeAxisLowValue;
int iTimeAxisHighValue;
int iTimeAxisSpeed;

bool iBackgroundColour;
unsigned char iBackgroundColourRed;
unsigned char iBackgroundColourGreen;
unsigned char iBackgroundColourBlue;

int16_t iMinAx = 0, iMaxAx = 0, iZeroAx = 0;

void readAnimationDetails();
unsigned char readByteUnsignedChar(int* aPosition);
signed char readByteSignedChar(int* aPosition);
void readFunctionData(int num);
void readAxis(int number);
void readAxisData(int number);

void setup() {
     pinMode(LED_PIN, OUTPUT);
      
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
    #endif
      
    Serial.begin(115200);
    delayMicroseconds(MICROSECONDS_IN_A_SECOND * 5);
    
    digitalWrite(LED_PIN, false);

    // initialize device
    Serial.println("Initializing I2C devices...");
    while (true) {
        accelGyro.initialize();
  
      // verify connection
      Serial.println("Testing device connections...");
      if (accelGyro.testConnection()) {
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
    
    readAnimationDetails();
}

void readAnimationDetails() {
    iTimeAxisNum = -1;
    
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 201; j++) {
            iValueAxisData[i][j] = 0;
        }
    }

    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 3; j++) {
            iFunctions[i][j] = 0;
        }
    }

    if (readByteUnsignedChar(&iCounter) != HEADER_BYTE) {
        //throw new InvalidAnimationException("No header byte");
    }

    unsigned char numLedsHigh = readByteUnsignedChar(&iCounter);
    unsigned char numLedsLow = readByteUnsignedChar(&iCounter);

    numLeds = numLedsHigh |= numLedsLow << 8;
    Serial.print("led count is ");
    Serial.print(numLeds, DEC);
    Serial.print("\n");

    iNumFunctions = readByteUnsignedChar(&iCounter);
    Serial.print("function count is ");
    Serial.print(iNumFunctions, DEC);
    Serial.print("\n");
    for (int i = 0; i < iNumFunctions; i++) {
        readFunctionData(i);
    }

    iNumValueAxes = readByteUnsignedChar(&iCounter);
    Serial.print("value axis count is ");
    Serial.print(iNumValueAxes, DEC);
    Serial.print("\n");

    readAxis(iTimeAxisNum);

    for (int i = 0; i < iNumValueAxes; i++) {
        readAxis(i);
    }

    leds.begin();

    Serial.print("FINISHED SETUP");
    Serial.print("\n");
}

void loop() {
    Serial.print("woo");
    Serial.print("\n");
    readAxisData(iTimeAxisNum);

    // if (readByteUnsignedChar(&iCounter) != TERMINATING_BYTE) {
    //throw new InvalidAnimationException("No terminating byte");
    // }
}

void readFunctionData(int num) {
    unsigned char red1 = readByteUnsignedChar(&iCounter);
    unsigned char red2 = readByteUnsignedChar(&iCounter);
    unsigned char red3 = readByteUnsignedChar(&iCounter);
    unsigned char red4 = readByteUnsignedChar(&iCounter);

    unsigned int r = red1;
    r |= red2 << 8;
    r |= red3 << 16;
    r |= red4 << 24;

    signed int redIncrement = r;

    unsigned char green1 = readByteUnsignedChar(&iCounter);
    unsigned char green2 = readByteUnsignedChar(&iCounter);
    unsigned char green3 = readByteUnsignedChar(&iCounter);
    unsigned char green4 = readByteUnsignedChar(&iCounter);

    unsigned int g = green1;
    g |= green2 << 8;
    g |= green3 << 16;
    g |= green4 << 24;
    signed int greenIncrement = g;

    //green1 |= green2 << 8;
    //greenIncrement = greenIncrement |= green3 << 16;
    //greenIncrement = greenIncrement |= green4 << 24;

    unsigned char blue1 = readByteUnsignedChar(&iCounter);
    unsigned char blue2 = readByteUnsignedChar(&iCounter);
    unsigned char blue3 = readByteUnsignedChar(&iCounter);
    unsigned char blue4 = readByteUnsignedChar(&iCounter);

    unsigned int b = blue1;
    b |= blue2 << 8;
    b |= blue3 << 16;
    b |= blue4 << 24;

    signed int blueIncrement = b;

    Serial.print(redIncrement, DEC);
    Serial.print(" ");
    Serial.print(greenIncrement, DEC);
    Serial.print(" ");
    Serial.print(blueIncrement, DEC);
    Serial.print("\n");

    iFunctions[num][0] = redIncrement;
    iFunctions[num][1] = greenIncrement;
    iFunctions[num][2] = blueIncrement;
}

int getNormalisedAccelerometerValue() {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    accelGyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    if(iZeroAx == 0) {
        Serial.print("new zero :");
        Serial.print(ax, DEC);
        Serial.print("\n");
        iZeroAx = ax;
    }
    
    
     if (ax < iMinAx && ax < iZeroAx - 200) {
        iMinAx = ax;
        Serial.print("new min :");
        Serial.print(ax, DEC);
        Serial.print("\n");
      }
      if (ax > iMaxAx && ax > iZeroAx + 200) {
        iMaxAx = ax;
        Serial.print("new max :");
        Serial.print(ax, DEC);
        Serial.print("\n");
      }
    
    if(ax > iZeroAx +200) {
        return 1;
    } else if(ax < iZeroAx - 200) {
        return -1;
    } else {
        return 0;
    }
}

void readAndSetColour(int ledNum) {
    Serial.print(ledNum, DEC);
    Serial.print(" : ");

    unsigned char red = readByteUnsignedChar(&iCounter);
    unsigned char green = readByteUnsignedChar(&iCounter);
    unsigned char blue = readByteUnsignedChar(&iCounter);
    
    Serial.print(red, HEX);
    Serial.print(" ");
    Serial.print(green, HEX);
    Serial.print(" ");
    Serial.print(blue, HEX);
    Serial.print("\n");
    
    if(iBackgroundColour &&
            red != iBackgroundColourRed ||
            green != iBackgroundColourGreen ||
            blue != iBackgroundColourBlue) {

        signed char accelerometerValue = getNormalisedAccelerometerValue();
    
        signed int redIncrement = 0;
        signed int greenIncrement = 0;
        signed int blueIncrement = 0;
    
        int initialValue = 0;
        int highValue = 0;
        if(accelerometerValue < 0) {
            initialValue = accelerometerValue;
            highValue = iValueAxisZeroValue;
        } else if(accelerometerValue > 0) {
            initialValue = iValueAxisZeroValue + 1;
            highValue = accelerometerValue + 1;
        }
        
        for(int frame = initialValue; frame < highValue; frame++) {
            int functionNum = iValueAxisData[ledNum][frame + iValueAxisOffset];
            //Serial.print(functionNum, DEC);
            //Serial.print(":");
            redIncrement += iFunctions[functionNum][0];
            greenIncrement += iFunctions[functionNum][1];
            blueIncrement += iFunctions[functionNum][2];
        }
    
        /*Serial.print(redIncrement, DEC);
        Serial.print(" ");
        Serial.print(greenIncrement, DEC);
        Serial.print(" ");
        Serial.print(blueIncrement, DEC);
        Serial.print("\n");*/
    
        if(redIncrement < -65536) {
            redIncrement = 0;
        }
    
        if(redIncrement > 65536) {
            redIncrement = 65536;
        }
    
        if(greenIncrement < -65536) {
            greenIncrement = 0;
        }
    
        if(greenIncrement > 65536) {
            greenIncrement = 65536;
        }
    
        if(blueIncrement < -65536) {
            blueIncrement = 0;
        }
    
        if(blueIncrement > 65536) {
            blueIncrement = 65536;
        }
    
    
        int redBig = red * 256;
        int greenBig = green * 256;
        int blueBig = blue * 256;
    
        redBig += redIncrement;
        if(redBig < 0) {
            redBig = 0;
        }
    
        if(redBig > 65536) {
            redBig = 65536;
        }
    
        greenBig += greenIncrement;
        if(greenBig < 0) {
            greenBig = 0;
        }
    
        if(greenBig > 65536) {
            greenBig = 65536;
        }
    
        blueBig += blueIncrement;
        if(greenBig < 0) {
            greenBig = 0;
        }
    
        if(greenBig > 65536) {
            greenBig = 65536;
        }
    
        redBig = redBig/256;
        greenBig = greenBig/256;
        blueBig = blueBig/256;
    
        red = redBig;
        green = greenBig;
        blue = blueBig;
    }

    leds.setPixel(ledNum, red, green, blue);
}

void readAxis(int number) {
    int axisType = readByteUnsignedChar(&iCounter);
    int priority = readByteUnsignedChar(&iCounter);
    boolean opaque = (boolean) readByteUnsignedChar(&iCounter);

    if (number == iTimeAxisNum) {
        iTimeAxisLowValue = readByteSignedChar(&iCounter);
        Serial.print("time axis low value : ");
        Serial.print(iTimeAxisLowValue, DEC);
        Serial.print("\n");
        iTimeAxisHighValue = readByteSignedChar(&iCounter);
        Serial.print("time axis high value : ");
        Serial.print(iTimeAxisHighValue, DEC);
        Serial.print("\n");
        iTimeAxisSpeed = readByteSignedChar(&iCounter);
        Serial.print("time axis speed : ");
        Serial.print(iTimeAxisSpeed, DEC);
        Serial.print("\n");
        
        iBackgroundColour = readByteUnsignedChar(&iCounter);
        if(iBackgroundColour) {
            Serial.print("background colour : ");
            iBackgroundColourRed = readByteUnsignedChar(&iCounter);
            iBackgroundColourGreen = readByteUnsignedChar(&iCounter);
            iBackgroundColourBlue = readByteUnsignedChar(&iCounter);
            
            Serial.print(iBackgroundColourRed, HEX);
            Serial.print(" ");
            Serial.print(iBackgroundColourGreen, HEX);
            Serial.print(" ");
            Serial.print(iBackgroundColourBlue, HEX);
            Serial.print("\n");
        }
    } else {
        iValueAxisLowValue = readByteSignedChar(&iCounter);
        Serial.print("value axis low value : ");
        Serial.print(iValueAxisLowValue, DEC);
        Serial.print("\n");
        iValueAxisHighValue = readByteSignedChar(&iCounter);
        Serial.print("value axis high value : ");
        Serial.print(iValueAxisHighValue, DEC);
        Serial.print("\n");
        iValueAxisZeroValue = readByteSignedChar(&iCounter);
        Serial.print("value axis zero value : ");
        Serial.print(iValueAxisZeroValue, DEC);
        Serial.print("\n");

        iValueAxisOffset = -iValueAxisLowValue;

        for (int frame = iValueAxisLowValue; frame <= iValueAxisHighValue;
                frame++) {
            //  Serial.print(frame, DEC);
            //   Serial.print("\n");
            int ledNum = INITIAL_LED;
            for (int i = 0; i < numLeds; i++) {
                char ledNum = readByteUnsignedChar(&iCounter); // led number
                //   Serial.print(ledNum, DEC);

                unsigned char frameType = readByteUnsignedChar(&iCounter);

                switch (frameType) {
                case FT_FUNCTION:
                    iValueAxisData[i][iValueAxisOffset + frame] =
                            readByteUnsignedChar(&iCounter);
                    //if(iValueAxisData[i][iValueAxisOffset + frame] != 0) {
                    //  Serial.print("function is ");
                    //  Serial.print(iValueAxisData[i][iValueAxisOffset + frame], DEC);
                    //  Serial.print("\n");
                    // }
                    break;
                case FT_LINKED:
                    iValueAxisData[i][iValueAxisOffset + frame] = 255;
                    break;
                }
            }
        }
    }
}

void readAxisData(int number) {
    Serial.print("readAxisData");
    Serial.print("\n");

    iTopOfTimeAxis = iCounter;
    for (int frame = iTimeAxisLowValue; frame <= iTimeAxisHighValue; frame++) {
        // Serial.print(frame, DEC);
        // Serial.print("\n");
        //int ledNum = kInitialLed;
        for (int i = 0; i < numLeds; i++) {
            int ledNum = readByteUnsignedChar(&iCounter); // led number
            //Serial.print(ledNum, DEC);
            //Serial.print("\n");

            unsigned char frameType = readByteUnsignedChar(&iCounter);

            switch (frameType) {
            case FT_FUNCTION:
                //readFunctionAndSetColour();
                break;
            case FT_COLOUR:
                readAndSetColour(i);
                break;
            }
        }

        //Serial.print("SHOW!");
        leds.show();
        //Serial.print("DONE!");

        delayMicroseconds(iTimeAxisSpeed * MICROSECONDS_IN_A_MILLISECOND);
    }

    iCounter = iTopOfTimeAxis;
}

unsigned char readByteUnsignedChar(int* aPosition) {
    unsigned char readByte = pgm_read_byte_near(animation1 + (*(aPosition))++);
    if (readByte == ESCAPE_BYTE) {
        readByte = pgm_read_byte_near(animation1 + (*(aPosition))++);
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}

signed char readByteSignedChar(int* aPosition) {
    signed char readByte = pgm_read_byte_near(animation1 + (*(aPosition))++);
    if (readByte == ESCAPE_BYTE) {
        readByte = pgm_read_byte_near(animation1 + (*(aPosition))++);
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}
