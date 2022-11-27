#include <Encoder.h>               // From Arduino IDE
#include <Adafruit_NeoPixel.h>     // From Arduino IDE
#include <LiquidCrystal_I2C.h>     // From https://lastminuteengineers.com/i2c-lcd-arduino-tutorial/


const int NUM_LEDS = 24;           // Number of LEDs on strip
const int LED_PIN = 5;             // Pin for LED strip
const int ENCODER_KNOB = 2;
const int ENCODER_DT = 3;
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
LiquidCrystal_I2C lcd(0x27, 16, 2);
Encoder encoder(ENCODER_KNOB, ENCODER_DT);

void initializeToBlack() {
// Turn all LEDs off.
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, 0);
    }
}

void setScale(char* scale, int color) {
// This function sets which LEDs to be lit up on the strip. Pass a string like
// "abc" to set LEDs 0, 1, and 2. You still need to strip.show() them to make
// them light up, but I've already done that in the switch statements. Each
// letter of the alphabet corresponds to a position on the LED strip in respect
// to that letter's position in the alphabet. A string of "xwjab" passed into
// this function would set LEDs 23, 22, 9, 0, and 1. The second parameter,
// "color", accepts values of 0, 1, 2 which correspond to lighting up all the
// LEDs either Red, Green, or Blue respectively. I used the alphabet because I
// needed 24 unique characters to represent each available LED to make the
// function easy to use.
    initializeToBlack();
    switch (color) {
        // Red
        case 0:
            for (int i = 0; scale[i] != '\0'; i++) {
                strip.setPixelColor(scale[i] - 'a', 0, 255, 0);
            }
            break;

        // Green
        case 1:
            for (int i = 0; scale[i] != '\0'; i++) {
                strip.setPixelColor(scale[i] - 'a', 255, 0, 0);
            }
            break;

        // Blue
        case 2:
            for (int i = 0; scale[i] != '\0'; i++) {
                strip.setPixelColor(scale[i] - 'a', 0, 0, 255);
            }
            break;
    }
}

// This function is used to print onto the LCD how we want to when we change
// states via a turn of the knob.
void turnPrint(char* text) {
    lcd.setCursor(1, 1);
    lcd.print("                ");
    lcd.setCursor(1, 1);
    lcd.print(text);
}

// This function is used to print onto the LCD how we want to when we change
// states via a press of the encoder.
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
    // from the hardware and then checks if that true position is different from
    // the "currentEncoderPosition" variable that was set in the last loop
    // before updating "currentEncoderPosition". This allows us to know when the
    // encoder changes positions and update our switch statement based on a
    // software variable, instead of the actual hardware encoder position, which
    // removes unused encoder positions that the user would have to cycle
    // through that do nothing. Now each change in the encoder knob position
    // will always result in a new state.
    knobValue = encoder.read() / 2;
    encoderPosition = normalize(knobValue, NUM_LEDS);

    if (encoderPosition != currentEncoderPosition) {
        if (encoderPosition > currentEncoderPosition) {
            encoderLoopPosition++;
            // Loop around.
            if (encoderLoopPosition > 2) encoderLoopPosition = 0;
        } else {
            encoderLoopPosition--;
            // Loop around.
            if (encoderLoopPosition < 0) encoderLoopPosition = 2;
        }
    }
    currentEncoderPosition = encoderPosition;

    // Implement button push loop-around. The encoder button produces a unique
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

    // Could use interrupts for both the button and encoderPosition but this
    // this works too, and the pin configuration for the Rev3 isn't clear which
    // pins are compatible with interrupts.

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
                        setScale("acefhjlmoqrtvx", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("acdfhikmmoprtuw", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("acdfhilmmoprtux", 1);
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
                        setScale("bdfgikmnprsuw", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("bdegijlnpqsuvx", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("bdegijmnpqsuv", 1);
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
                        setScale("ceghjlnoqstvx", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("coeqfrhtjvkmx", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("coeqfrhtjvknx", 1);
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
                        setScale("d", 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("d", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("d", 2);
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
                        setScale("e", 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("e", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("e", 2);
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
                        setScale("f", 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("f", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("f", 2);
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
                        setScale("g", 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("g", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("g", 2);
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
                        setScale("h", 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("h", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("h", 2);
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
                        setScale("i", 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("i", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("i", 2);
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
                        setScale("j", 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("j", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("j", 2);
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
                        setScale("k", 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("k", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("k", 2);
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
                        setScale("l", 0);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 1:
                    currentLoopPosition = 2;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Natural Minor");
                        setScale("l", 1);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;

                // Natural Minor
                case 2:
                    currentLoopPosition = 3;
                    if (currentLoopPosition != oldLoopPosition || buttonPushed) {
                        turnPrint("Harmonic Minor");
                        setScale("l", 2);
                        oldLoopPosition = currentLoopPosition;
                        buttonPushed = 0;
                    }
                    break;
            }
            strip.show();
            break;
    }
}
