#include <Encoder.h>               // From Arduino IDE
#include <Adafruit_NeoPixel.h>     // From Arduino IDE
#include <LiquidCrystal_I2C.h>     // From https://lastminuteengineers.com/i2c-lcd-arduino-tutorial/


const int NUM_LEDS = 24;           // Number of LEDs on strip
const int LED_PIN = 5;             // Pin for LED strip
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

unsigned long lastPush = 0;
int buttonPushed = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Encoder encoder(ENCODER_PIN_1, ENCODER_PIN_2);

void initializeToBlack() {
// Turn all LEDs off.
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, 0);
    }
}

void setScale(char* scale, int length, int color) {
// This function sets which LEDs to be lit up on the strip. Pass a string like
// "abc" to light up leds 0, 1, 2. Each letter of the alphabet corresponds to
// a position on the LED strip in respect to that letters position in the
// alphabet. A string of "xwjab" passed into this function would set LEDs 23,
// 22, 9, 0, 1. You must pass in the length of the string you're using, so the
// string "xwjab" would have a 5 passed in for the second "lenngth" argument.
// The third parameter, "color", accepts values of 0, 1, 2 which correspond to
// lighting up all the LEDs either Red, Green, or Blue respectively.
    initializeToBlack();
    switch (color) {
        // Red
        case 0:
            for (int i = 0; i < length; i++) {
                strip.setPixelColor(scale[i] - 'a', 0, 255, 0);
            }
            break;

        // Green
        case 1:
            for (int i = 0; i < length; i++) {
                strip.setPixelColor(scale[i] - 'a', 255, 0, 0);
            }
            break;

        // Blue
        case 2:
            for (int i = 0; i < length; i++) {
                strip.setPixelColor(scale[i] - 'a', 0, 0, 255);
            }
            break;
    }
}

void turnPrint(char* text) {
    lcd.setCursor(1, 1);
    lcd.print("                ");
    lcd.setCursor(1, 1);
    lcd.print(text);
}

void pressPrint (char* text) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(text);
}

long normalize(long value, long radix) {
    long rval = value % radix;
    if (rval < 0) return radix + rval;
    else return rval;
}

void setup() {
    Serial.begin(9600);

    pinMode(ENCODER_BUTTON, INPUT);
    // Turn pullup resistor on
    digitalWrite(ENCODER_BUTTON, HIGH);

    strip.begin();
    initializeToBlack();
    strip.setBrightness(8);
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

void loop() {
    // Implement knob turn loop-around. This reads the true encoder position
    // from the hardware and then checks if that true position is different
    // from the "currentEncoderPosition" variable that was set in the last loop
    // before updating "currentEncoderPosition". This allows us to know when
    // the encoder changes positions and update our switch statement based on a
    // software variable, instead of the actual hardware encoder position,
    // which removes unused encoder positions that the user would have to cycle
    // through that do nothing. Now each change in the encoder knob position
    // will always result in a new state.
    knobValue = encoder.read() / 2;
    encoderPosition = normalize(knobValue, NUM_LEDS);

    if (encoderPosition != currentEncoderPosition) {
        if (encoderPosition > currentEncoderPosition) {
            encoderLoopPosition++;
        } else {
            encoderLoopPosition--;
        }
        // Loop around.
        if (encoderLoopPosition > 2) encoderLoopPosition = 0;
        if (encoderLoopPosition < 0) encoderLoopPosition = 2;
    }
    currentEncoderPosition = encoderPosition;

    // Implement button bush loop-around. The encoder button produces a unique
    // value when it is pressed down, making it easy to detect when the button
    // is pressed.
    int button = digitalRead(ENCODER_BUTTON);

    if (button == 0) {
        if ((millis() - lastPush) > 250) {
            lastPush = millis();
            mode++;
            // Loop around.
            if (mode > 11) mode = 0;
            buttonPushed = 1;
        }
    }

    switch (mode) {

        // C
        case 0:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("C");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("acefhjlmoqrtvx", 14, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("acdfhikmmoprtuw", 15, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("acdfhilmmoprtux", 15, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // C# and Db
        case 1:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("C# and Db");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("b", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("b", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("b", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // D
        case 2:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("D");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("c", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("c", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("c", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // D# and Eb
        case 3:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("D# and Eb");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("d", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("d", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("d", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // E
        case 4:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("E");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("e", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("e", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("e", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // F
        case 5:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("F");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("f", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("f", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("f", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // F# and Gb
        case 6:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("F# and Gb");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("g", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("g", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("g", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // G
        case 7:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("G");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("h", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("h", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("h", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // G# and Ab
        case 8:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("G# and Ab");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("i", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("i", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("i", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // A
        case 9:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("A");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("j", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("j", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("j", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // A# and Bb
        case 10:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("A# and Bb");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("k", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("k", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("k", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;

        // B
        case 11:
            currentMode = 1;
            if (currentMode != oldMode || buttonPushed) {
                pressPrint("B");
                oldMode = currentMode;
            }

            switch (encoderLoopPosition) {

                // Major
                case 0:
                    currentLoopPosition = 1;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Major");
                        setScale("l", 1, 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("l", 1, 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("l", 1, 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;
    }
}
