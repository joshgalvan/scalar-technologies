#include <Encoder.h>               // From Arduino IDE
#include <Adafruit_NeoPixel.h>     // From Arduino IDE
#include <LiquidCrystal_I2C.h>     // From https://lastminuteengineers.com/i2c-lcd-arduino-tutorial/


const int NUM_LEDS = 24;           // number of leds in strip
const int LED_PIN = 5;             // pin for led strip
const int ENCODER_PIN_1 = 2;
const int ENCODER_PIN_2 = 3;
const int ENCODER_BUTTON = 4;

int mode = 0;
int currentMode = 0;
int oldMode = 0;

long knobValue = 0;
long encoderPosition = 0;
int currentEncoderPosition = 0;
int encoderLoopPosition = 0;
int currentLoopPosition = 0;
int oldLoopPosition = 0;

int lastPush = 0;
int buttonPushed = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Encoder encoder(ENCODER_PIN_1, ENCODER_PIN_2);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);

// Turn all LEDs off.
void initializeToBlack()
{
    for (int i = 0; i < NUM_LEDS; i++)
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

    // LCD Setup
    lcd.init();
    lcd.clear();
    // Turn backlight on.
    lcd.backlight();

    // Initialize encoder position reading.
    knobValue = encoder.read() / 2;
    encoderPosition = normalize(knobValue, NUM_LEDS);
    encoderLoopPosition = 0;
}

void loop()
{
    // Implement knob turn loop-around.
    knobValue = encoder.read() / 2;
    encoderPosition = normalize(knobValue, NUM_LEDS);

    if (encoderPosition != currentEncoderPosition) {
        encoderLoopPosition++;
        // Loop around.
        if (encoderLoopPosition > 2) encoderLoopPosition = 0;
    }
    currentEncoderPosition = encoderPosition;

    // Implement button bush loop-around.
    int button = digitalRead(ENCODER_BUTTON);

    if (button == 0) {
        if ((millis() - lastPush) > 250) {
            lastPush = millis();
            mode++;
            // Loop around.
            if (mode > 2) mode = 0;
            buttonPushed = 1;
        }
    }

    switch (mode) {

        case 0:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                lcd.clear();
                lcd.setCursor(2, 0);
                lcd.print("Mode 0");
                initializeToBlack();
                strip.show();
                oldMode = currentMode;
                buttonPushed = 0;
            }
            break;

        case 1:
            currentMode = 2;
            if (currentMode != oldMode) {
                lcd.clear();
                lcd.setCursor(2, 0);
                lcd.print("Mode 1");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        lcd.setCursor(2, 1);
                        // Printing blank characters is the same as erasing it. I
                        // chose to print this many characters because that is the
                        // maximum length of characters that would be printed on the
                        // second row.
                        lcd.print("        ");
                        lcd.setCursor(2, 1);
                        lcd.print("Off");
                        initializeToBlack();
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        lcd.setCursor(2, 1);
                        lcd.print("        ");
                        lcd.setCursor(2, 1);
                        lcd.print("C Major");
                        initializeToBlack();
                        strip.setPixelColor(3, 0, 0, 255);
                        strip.setPixelColor(2, 0, 0, 255);
                        strip.setPixelColor(1, 0, 0, 255);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        lcd.setCursor(2, 1);
                        lcd.print("        ");
                        lcd.setCursor(2, 1);
                        lcd.print("C# Major");
                        initializeToBlack();
                        strip.setPixelColor(4, 0, 0, 255);
                        strip.setPixelColor(12, 0, 0, 255);
                        strip.setPixelColor(11, 0, 0, 255);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                default:
                    currentLoopPosition = 4;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        lcd.setCursor(2, 1);
                        lcd.print("        ");
                        lcd.setCursor(2, 1);
                        lcd.print("Default");
                        initializeToBlack();
                        strip.setPixelColor(0, 0, 0, 255);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        case 2:
            currentMode = 3;
            if (currentMode != oldMode) {
                lcd.clear();
                lcd.setCursor(2, 0);
                lcd.print("All on");
                for (int i = 0; i < NUM_LEDS; i++) {
                    strip.setPixelColor(i, 0, 0, 255);
                }
                strip.show();
                oldMode = currentMode;
            }
            break;
    }
}
