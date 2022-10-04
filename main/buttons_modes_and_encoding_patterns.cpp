#include <Encoder.h>               // From Arduino IDE
#include <Adafruit_NeoPixel.h>     // From Arduino IDE

const int NUM_LEDS = 24;           // number of leds in strip
const int LED_PIN = 5;             // pin for led strip
const int BRIGHTNESS = 10;         // brightness of all leds
const int WHEEL_SIZE = 256;        // how many entries in the color wheel
const boolean MOVE_LIGHT = false;  // move one light around or keep all lights on

const int ENCODER_PIN_1 = 2;
const int ENCODER_PIN_2 = 3;
const int ENCODER_BUTTON = 4;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);
Encoder encoder(ENCODER_PIN_1, ENCODER_PIN_2);
int mode = 0;
long lastPush = 0;
int autoPosition = 0;

// Default LED
void initializeToBlack()
{
    for (int i =0; i < NUM_LEDS; i++)
    {
     strip.setPixelColor(i, 0);
    }
}

long normalize(long value, long radix)
{
    long rval = value % radix;
    if (rval < 0) return radix + rval;
    else return rval;
}

void setup()
{
    Serial.begin(9600);
    pinMode(ENCODER_BUTTON, INPUT);
    // Turn pullup resistor on
    digitalWrite(ENCODER_BUTTON, HIGH);

    strip.begin();
    initializeToBlack();
    strip.show();
}

void loop()
{
    int button = digitalRead(ENCODER_BUTTON);

    if (button == 0)
    {
        if ((millis() - lastPush) > 250)
        {
          lastPush = millis();
          mode++;
          // 0-1-2 button states
          if (mode > 2) mode = 0;
        }
    }

    long knobValue = encoder.read() / 2;
    long ledPosition = normalize(knobValue, NUM_LEDS);
    long colorValue = normalize(knobValue * 5, WHEEL_SIZE);
    long sleepValue = abs(knobValue) % 500;

    // What if we nested the case? Switch for button press and switch for led position?
    // if mode == number

    // Mode 1
    if(mode == 0)
    {
        // Sets default values to zero
        initializeToBlack();
        strip.show();
    }

    // Mode 2
    if(mode == 1)
    {
        switch(ledPosition)
        {
            // Off
            case 0:
            // Sets default values to zero
            initializeToBlack();
            break;

            // C major
            case 1:
            initializeToBlack();
            // strip.setPixelColor(ledPosition, colorWheel(BRIGHTNESS, colorValue));
            strip.setPixelColor(3, 0, 0, 255);
            strip.setPixelColor(2, 0, 0, 255);
            strip.setPixelColor(1, 0, 0, 255);
            break;

            // C# major
            case 2:
            initializeToBlack();
            // strip.setPixelColor(ledPosition, colorWheel(BRIGHTNESS, colorValue));
            strip.setPixelColor(4, 0, 0, 255);
            strip.setPixelColor(12, 0, 0, 255);
            strip.setPixelColor(11, 0, 0, 255);
            break;
        }
    strip.show();
    }

    // Mode 3
    if(mode == 2)
    {
        // Sets default values to zero
        initializeToBlack();
        strip.setPixelColor(2, 0, 0, 255);
        strip.show();
    }

}
