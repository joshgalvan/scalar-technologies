// Notes:
//
// On my LCD the text that was displayed was very buggy and not very clear.
// Would like to fix this in a further update but I'm not exactly sure why it's
// happening.
//

#include <Encoder.h>               // From Arduino IDE
#include <Adafruit_NeoPixel.h>     // From Arduino IDE
#include <LiquidCrystal_I2C.h>     // From https://lastminuteengineers.com/i2c-lcd-arduino-tutorial/


const int NUM_LEDS = 12;           // number of leds in strip
const int LED_PIN = 5;             // pin for led strip
const int BRIGHTNESS = 10;         // brightness of all leds
const int WHEEL_SIZE = 256;        // how many entries in the color wheel
const boolean MOVE_LIGHT = false;  // move one light around or keep all lights on
const int ENCODER_PIN_1 = 2;
const int ENCODER_PIN_2 = 3;
const int ENCODER_BUTTON = 4;
int mode = 0;
long lastPush = 0;
int autoPosition = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);
Encoder encoder(ENCODER_PIN_1, ENCODER_PIN_2);


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

    // Print a message on both lines of the LCD.
    lcd.setCursor(2,0);
    lcd.print("Test setup 1");

    lcd.setCursor(2,1);
    lcd.print("Test setup 2");
}

void loop()
{
    int button = digitalRead(ENCODER_BUTTON);
    long knobValue = encoder.read() / 2;
    long ledPosition = normalize(knobValue, NUM_LEDS);
    long colorValue = normalize(knobValue * 5, WHEEL_SIZE);
    long sleepValue = abs(knobValue) % 500;

    if (button == 0) {
        if ((millis() - lastPush) > 250) {
            lastPush = millis();
            mode++;
            // Loop around.
            if (mode > 2) mode = 0;
        }
    }

    // All off
    if (mode == 0) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Mode 0");
        initializeToBlack();
        strip.show();
    }

    // Switching Mode
    if  (mode == 1) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Mode 1");
        switch(ledPosition)
        {
            // Off
            case 0:
                lcd.setCursor(2, 1);
                // Printing blank characters is the same as erasing it. I
                // chose to print this many characters because that is the
                // maximum length of characters that would be printed on the
                // second row.
                lcd.print("        ");
                lcd.setCursor(2, 1);
                lcd.print("Off");
                initializeToBlack();
                break;

            // C Major
            case 1:
                lcd.setCursor(2, 1);
                lcd.print("        ");
                lcd.setCursor(2, 1);
                lcd.print("C Major");
                initializeToBlack();
                strip.setPixelColor(3, 0, 0, 255);
                strip.setPixelColor(2, 0, 0, 255);
                strip.setPixelColor(1, 0, 0, 255);
                break;

            // C# Major
            case 2:
                lcd.setCursor(2, 1);
                lcd.print("        ");
                lcd.setCursor(2, 1);
                lcd.print("C# Major");
                initializeToBlack();
                strip.setPixelColor(4, 0, 0, 255);
                strip.setPixelColor(12, 0, 0, 255);
                strip.setPixelColor(11, 0, 0, 255);
                break;

            default:
                lcd.setCursor(2, 1);
                lcd.print("        ");
                lcd.setCursor(2, 1);
                lcd.print("Default");
                initializeToBlack();
                strip.setPixelColor(0, 0, 0, 255);
        }
        strip.show();
    }

    // All on
    if (mode == 2) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("All on");
        for (int i = 0; i < NUM_LEDS; i++) {
            strip.setPixelColor(i, 0, 0, 255);
        }
        strip.show();
    }
}
