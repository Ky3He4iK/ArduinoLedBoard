#include <FastLED.h>
//#include <SoftwareSerial.h>

// config
#define BT_RX 2 // rx _ON_BT_PLATE_?
#define BT_TX 3 // tx _ON_BT_PLATE_?
#define LED_PIN     11
#define MAX_NUM_LEDS    220
#define BRIGHTNESS  48
#define UPDATES_PER_SECOND 60

// other constants
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

//variables
CRGB leds[MAX_NUM_LEDS];
//SoftwareSerial SerialBT(BT_RX, BT_TX);

struct LedGroup {
    uint16_t startIndex = 0;
    uint8_t series_len = 1; // each position will be shown N ticks
    uint8_t step_size = 1; // each position has shift N
    uint8_t num_leds;
    uint8_t first_led;
    bool is_paused = false;
    CRGBPalette16 palette = RainbowColors_p;

    LedGroup(int num, int first) {
        num_leds = num;
        first_led = first;
    }
};

LedGroup mainGroup(120, 0);
LedGroup headGroup(100, 120);
uint16_t last_time = 0, c_time, frame_time;
uint16_t update_time_ms = 1000 / UPDATES_PER_SECOND;
uint16_t i; // for indexing
uint16_t brightness = BRIGHTNESS;

void setup() {
    delay(1000); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, mainGroup.num_leds + headGroup.num_leds).setCorrection(
            TypicalLEDStrip);
    FastLED.setBrightness(brightness);
    headGroup.palette = CRGBPalette16(
            CRGB(0, 0, 0), CRGB(85, 85, 85), CRGB(171, 171, 171), CRGB(255, 255, 255),
            CRGB(255, 255, 255), CRGB(171, 171, 171), CRGB(85, 85, 85), CRGB(0, 0, 0),
            CRGB(0, 0, 0), CRGB(85, 85, 85), CRGB(171, 171, 171), CRGB(255, 255, 255),
            CRGB(255, 255, 255), CRGB(171, 171, 171), CRGB(85, 85, 85), CRGB(0, 0, 0)
    );

    Serial.begin(9600);
}


void loop() {
    if (Serial.available() > 0) {
        serial_code();
    } else
        graphic_code();
    sleepControl();
}

void serial_code() {
    uint8_t cmd = Serial.read();
    if (String("fpbcsiaotnFPBCSIAOTN").indexOf(cmd) == -1)
        return;
    Serial.println("O");

    for (i = 0; Serial.available() == 0 && i < 60; ++i)
        delay(50);
    if (Serial.available() == 0)
        return;

    uint8_t data_val = Serial.read(), r, g, b;
    switch (cmd) {
        case 'F':
        case 'f': // set framerate
            data_val -= 'A';
            if (data_val <= 0)
                data_val = 1;
            Serial.print("S F ");
            Serial.println(data_val, DEC);
            update_time_ms = 1000 / data_val;
            break;

        case 'p': // set palette
            Serial.print("S P ");
            data_val -= 'A';
            if (data_val < 16) {
                switch (data_val) {
                    case 0:
                        mainGroup.palette = RainbowColors_p;
                        break;
                    case 1:
                        mainGroup.palette = RainbowStripeColors_p;
                        break;
                    case 2:
                        mainGroup.palette = OceanColors_p;
                        break;
                    case 3:
                        mainGroup.palette = CloudColors_p;
                        break;
                    case 4:
                        mainGroup.palette = LavaColors_p;
                        break;
                    case 5:
                        mainGroup.palette = ForestColors_p;
                        break;
                    case 6:
                        mainGroup.palette = PartyColors_p;
                        break;
                    case 7:
                        mainGroup.palette = CRGBPalette16( // 7 BW
                                CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black,
                                CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black,
                                CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black,
                                CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black
                        );
                        break;
                    case 8:
                        mainGroup.palette = CRGBPalette16( // 8 purple and green
                                CRGB::Green, CRGB::Green, CRGB::Black, CRGB::Black,
                                CRGB::Purple, CRGB::Purple, CRGB::Black, CRGB::Black,
                                CRGB::Green, CRGB::Green, CRGB::Black, CRGB::Black,
                                CRGB::Purple, CRGB::Purple, CRGB::Black, CRGB::Black
                        );
                        break;
                    case 9:
                        mainGroup.palette = CRGBPalette16( // 9 rgb with white
                                CRGB::White, CRGB::Red, CRGB::Green, CRGB::Blue,
                                CRGB::Black, CRGB::Blue, CRGB::Green, CRGB::Red,
                                CRGB::White, CRGB::Red, CRGB::Green, CRGB::Blue,
                                CRGB::Black, CRGB::Blue, CRGB::Green, CRGB::Red
                        );
                        break;
                    case 10:
                        mainGroup.palette = CRGBPalette16( // 10 rainbow with white
                                CRGB::White, CRGB::Red, CRGB::Orange, CRGB::Yellow,
                                CRGB::Green, CRGB::Aqua, CRGB::Blue, CRGB::Purple,
                                CRGB::White, CRGB::Red, CRGB::Green, CRGB::Blue,
                                CRGB::Black, CRGB::Blue, CRGB::Green, CRGB::Red
                        );
                        break;
                    case 11:
                        mainGroup.palette = CRGBPalette16( // 11 strange palette
                                CRGB(255, 255, 255), CRGB(213, 42, 0), CRGB(171, 85, 0), CRGB(171, 127, 0),
                                CRGB(171, 171, 0), CRGB(86, 213, 0), CRGB(0, 255, 0), CRGB(0, 213, 42),
                                CRGB(0, 171, 85), CRGB(0, 86, 170), CRGB(0, 0, 255), CRGB(42, 0, 213),
                                CRGB(85, 0, 171), CRGB(127, 0, 129), CRGB(171, 0, 85), CRGB(213, 0, 43)
                        );
                        break;
                    case 12:
                        mainGroup.palette = CRGBPalette16( // 12 Red white blue
                                CRGB::Red, CRGB::Gray, CRGB::Blue, CRGB::Black,
                                CRGB::Red, CRGB::Gray, CRGB::Blue, CRGB::Black,
                                CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
                                CRGB::Blue, CRGB::Blue, CRGB::Black, CRGB::Black
                        );
                        break;
                    case 13:
                        mainGroup.palette = CRGBPalette16( // 13
                                CRGB::White, CRGB::Gray, CRGB::Black, CRGB::Black,
                                CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Gray,
                                CRGB::White, CRGB::Gray, CRGB::Black, CRGB::Black,
                                CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Gray
                        );
                        break;
                    case 14:
                        mainGroup.palette = CRGBPalette16( // 14
                                CRGB::Red, CRGB::White, CRGB::Blue, CRGB::Black,
                                CRGB::Green, CRGB::White, CRGB::Aqua, CRGB::Black,
                                CRGB::Red, CRGB::White, CRGB::Blue, CRGB::Gray,
                                CRGB::Green, CRGB::White, CRGB::Purple, CRGB::Black
                        );
                        break;
                    case 15:
                        mainGroup.palette = CRGBPalette16( // 15
                                CRGB(0, 0, 0), CRGB(85, 85, 85), CRGB(171, 171, 171), CRGB(255, 255, 255),
                                CRGB(255, 255, 255), CRGB(171, 171, 171), CRGB(85, 85, 85), CRGB(0, 0, 0),
                                CRGB(0, 0, 0), CRGB(85, 85, 85), CRGB(171, 171, 171), CRGB(255, 255, 255),
                                CRGB(255, 255, 255), CRGB(171, 171, 171), CRGB(85, 85, 85), CRGB(0, 0, 0)
                        );
                        break;
                }
                Serial.print("S P PR ");
                Serial.println(data_val, DEC);
            } else {
                switch (data_val) {
                    case 32: // random
                        for (int i = 0; i < 16; i++)
                            mainGroup.palette[i] = CHSV(random8(), 255, random8());
                        Serial.println("S P RNG");
                        break;
                    case 33: // custom palette
                        for (i = 0; Serial.available() < 2 && i < 10; ++i)
                            delay(50);
                        mainGroup.palette[0] = CRGB(data_val + 'A', Serial.read(), Serial.read());
                        Serial.write(0);
                        for (uint8_t i = 1; i < 16; ++i) {
                            for (uint8_t j = 0; Serial.available() < 3 && j < 10; ++j)
                                delay(50);
                            mainGroup.palette[i] = CRGB(Serial.read(), Serial.read(), Serial.read());
                            Serial.write(i);
                        }
                        Serial.print("S P CUSTOM");
                        for (i = 0; i < (16 << 2); ++i) {
                            if ((i & 3) == 3)
                                continue;
                            Serial.print(mainGroup.palette[(uint8_t)(i >> 2)][i & 3]);
                            Serial.print(' ');
                        }
                        Serial.println();
                        break;
                }
            }
            break;
        case 'b':
        case 'B': // set brigthness
            brightness = data_val;
            FastLED.setBrightness(brightness);
            Serial.print("S B ");
            Serial.println(brightness, DEC);
            break;
        case 'c': // set all to solid color
            r = data_val;
            g = Serial.read();
            b = Serial.read();
            Serial.print("S C ");
            Serial.print(r, DEC);
            Serial.print(' ');
            Serial.print(g, DEC);
            Serial.print(' ');
            Serial.println(b, DEC);

            for (i = 0; i < 16; ++i)
                mainGroup.palette[(uint8_t) i] = CRGB(r, g, b);
            break;
        case 's': // set series
            Serial.print("S S ");
            data_val -= 'A';
            if (data_val <= 0)
                data_val = 1;
            mainGroup.series_len = data_val;
            Serial.println(data_val, DEC);
            break;
        case 'i': // get variables info:
            Serial.print(brightness, DEC);
            Serial.print(' ');
            Serial.print(mainGroup.num_leds, DEC);
            Serial.print(' ');
            Serial.print(mainGroup.series_len, DEC);
            Serial.print(' ');
            Serial.print(mainGroup.step_size, DEC);
            Serial.print(' ');
            Serial.print(last_time, DEC);
            Serial.print(' ');
            Serial.print(mainGroup.startIndex, DEC);
            Serial.print(' ');
            Serial.print(update_time_ms, DEC);
            Serial.print(' ');
            for (i = 0; i < 16 * 4; ++i) {
                if ((i & 3) == 3)
                    continue;
                Serial.print(mainGroup.palette[(uint8_t)(i >> 2)][i & 3], DEC);
                Serial.print(' ');
            }
            Serial.println();
            break;
        case 'a': // pause/continue
            mainGroup.is_paused = !mainGroup.is_paused;
            FastLED.setBrightness(brightness);
            Serial.print("PAUSE ");
            Serial.println(mainGroup.is_paused, DEC);
            break;
        case 'o':
        case 'O': // turn off
//            fill_solid(mainGroup.palette, 16, CRGB::Black);
            mainGroup.is_paused = true;
            FastLED.setBrightness(0);
//            FastLED.show();
            Serial.println("OFF");
            break;
        case 't': // step_size
            data_val -= 'A';
            if (data_val <= 0)
                data_val = 1;
            mainGroup.step_size = data_val;
            Serial.print("S STEP ");
            Serial.println(mainGroup.step_size, DEC);
            break;
        case 'n': // particular color in palette
            for (i = 0; Serial.available() < 3 && i < 60; ++i)
                delay(50);
            mainGroup.palette[data_val] = CRGB(Serial.read(), Serial.read(), Serial.read());
            break;


        case 'P': // set palette
            Serial.print("S2 P ");
            data_val -= 'A';
            if (data_val < 16) {
                switch (data_val) {
                    case 0:
                        headGroup.palette = RainbowColors_p;
                        break;
                    case 1:
                        headGroup.palette = RainbowStripeColors_p;
                        break;
                    case 2:
                        headGroup.palette = OceanColors_p;
                        break;
                    case 3:
                        headGroup.palette = CloudColors_p;
                        break;
                    case 4:
                        headGroup.palette = LavaColors_p;
                        break;
                    case 5:
                        headGroup.palette = ForestColors_p;
                        break;
                    case 6:
                        headGroup.palette = PartyColors_p;
                        break;
                    case 7:
                        headGroup.palette = CRGBPalette16( // 7 BW
                                CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black,
                                CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black,
                                CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black,
                                CRGB::White, CRGB::Black, CRGB::Black, CRGB::Black
                        );
                        break;
                    case 8:
                        headGroup.palette = CRGBPalette16( // 8 purple and green
                                CRGB::Green, CRGB::Green, CRGB::Black, CRGB::Black,
                                CRGB::Purple, CRGB::Purple, CRGB::Black, CRGB::Black,
                                CRGB::Green, CRGB::Green, CRGB::Black, CRGB::Black,
                                CRGB::Purple, CRGB::Purple, CRGB::Black, CRGB::Black
                        );
                        break;
                    case 9:
                        headGroup.palette = CRGBPalette16( // 9 rgb with white
                                CRGB::White, CRGB::Red, CRGB::Green, CRGB::Blue,
                                CRGB::Black, CRGB::Blue, CRGB::Green, CRGB::Red,
                                CRGB::White, CRGB::Red, CRGB::Green, CRGB::Blue,
                                CRGB::Black, CRGB::Blue, CRGB::Green, CRGB::Red
                        );
                        break;
                    case 10:
                        headGroup.palette = CRGBPalette16( // 10 rainbow with white
                                CRGB::White, CRGB::Red, CRGB::Orange, CRGB::Yellow,
                                CRGB::Green, CRGB::Aqua, CRGB::Blue, CRGB::Purple,
                                CRGB::White, CRGB::Red, CRGB::Green, CRGB::Blue,
                                CRGB::Black, CRGB::Blue, CRGB::Green, CRGB::Red
                        );
                        break;
                    case 11:
                        headGroup.palette = CRGBPalette16( // 11 strange palette
                                CRGB(255, 255, 255), CRGB(213, 42, 0), CRGB(171, 85, 0), CRGB(171, 127, 0),
                                CRGB(171, 171, 0), CRGB(86, 213, 0), CRGB(0, 255, 0), CRGB(0, 213, 42),
                                CRGB(0, 171, 85), CRGB(0, 86, 170), CRGB(0, 0, 255), CRGB(42, 0, 213),
                                CRGB(85, 0, 171), CRGB(127, 0, 129), CRGB(171, 0, 85), CRGB(213, 0, 43)
                        );
                        break;
                    case 12:
                        headGroup.palette = CRGBPalette16( // 12 Red white blue
                                CRGB::Red, CRGB::Gray, CRGB::Blue, CRGB::Black,
                                CRGB::Red, CRGB::Gray, CRGB::Blue, CRGB::Black,
                                CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
                                CRGB::Blue, CRGB::Blue, CRGB::Black, CRGB::Black
                        );
                        break;
                    case 13:
                        headGroup.palette = CRGBPalette16( // 13
                                CRGB::White, CRGB::Gray, CRGB::Black, CRGB::Black,
                                CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Gray,
                                CRGB::White, CRGB::Gray, CRGB::Black, CRGB::Black,
                                CRGB::Black, CRGB::Black, CRGB::Black, CRGB::Gray
                        );
                        break;
                    case 14:
                        headGroup.palette = CRGBPalette16( // 14
                                CRGB::Red, CRGB::White, CRGB::Blue, CRGB::Black,
                                CRGB::Green, CRGB::White, CRGB::Aqua, CRGB::Black,
                                CRGB::Red, CRGB::White, CRGB::Blue, CRGB::Gray,
                                CRGB::Green, CRGB::White, CRGB::Purple, CRGB::Black
                        );
                        break;
                    case 15:
                        headGroup.palette = CRGBPalette16( // 15
                                CRGB(0, 0, 0), CRGB(85, 85, 85), CRGB(171, 171, 171), CRGB(255, 255, 255),
                                CRGB(255, 255, 255), CRGB(171, 171, 171), CRGB(85, 85, 85), CRGB(0, 0, 0),
                                CRGB(0, 0, 0), CRGB(85, 85, 85), CRGB(171, 171, 171), CRGB(255, 255, 255),
                                CRGB(255, 255, 255), CRGB(171, 171, 171), CRGB(85, 85, 85), CRGB(0, 0, 0)
                        );
                        break;
                }
                Serial.print("S2 P PR ");
                Serial.println(data_val, DEC);
            } else {
                switch (data_val) {
                    case 32: // random
                        for (int i = 0; i < 16; i++)
                            headGroup.palette[i] = CHSV(random8(), 255, random8());
                        Serial.println("S P RNG");
                        break;
                    case 33: // custom palette
                        for (i = 0; Serial.available() < 2 && i < 10; ++i)
                            delay(50);
                        headGroup.palette[0] = CRGB(data_val + 'A', Serial.read(), Serial.read());
                        Serial.write(0);
                        for (uint8_t i = 1; i < 16; ++i) {
                            for (uint8_t j = 0; Serial.available() < 3 && j < 10; ++j)
                                delay(50);
                            headGroup.palette[i] = CRGB(Serial.read(), Serial.read(), Serial.read());
                            Serial.write(i);
                        }
                        Serial.print("S2 P CUSTOM");
                        for (i = 0; i < (16 << 2); ++i) {
                            if ((i & 3) == 3)
                                continue;
                            Serial.print(headGroup.palette[(uint8_t)(i >> 2)][i & 3], DEC);
                            Serial.print(' ');
                        }
                        Serial.println();
                        break;
                }
            }
            break;
        case 'C': // set all to solid color
            r = data_val;
            g = Serial.read();
            b = Serial.read();
            Serial.print("S2 C ");
            Serial.print(r, DEC);
            Serial.print(' ');
            Serial.print(g, DEC);
            Serial.print(' ');
            Serial.println(b, DEC);

            for (i = 0; i < 16; ++i)
                headGroup.palette[(uint8_t) i] = CRGB(r, g, b);
            break;
        case 'S': // set series
            Serial.print("S2 S ");
            data_val -= 'A';
            if (data_val <= 0)
                data_val = 1;
            headGroup.series_len = data_val;
            Serial.println(data_val, DEC);
            break;
        case 'I': // get variables info:
            Serial.print(brightness, DEC);
            Serial.print(' ');
            Serial.print(headGroup.num_leds, DEC);
            Serial.print(' ');
            Serial.print(headGroup.series_len, DEC);
            Serial.print(' ');
            Serial.print(headGroup.step_size, DEC);
            Serial.print(' ');
            Serial.print(last_time, DEC);
            Serial.print(' ');
            Serial.print(headGroup.startIndex, DEC);
            Serial.print(' ');
            Serial.print(update_time_ms, DEC);
            Serial.print(' ');
            for (i = 0; i < 16 * 4; ++i) {
                if ((i & 3) == 3)
                    continue;
                Serial.print(headGroup.palette[(uint8_t)(i >> 2)][i & 3], DEC);
                Serial.print(' ');
            }
            Serial.println();
            break;
        case 'A': // pause/continue
            headGroup.is_paused = !headGroup.is_paused;
            FastLED.setBrightness(brightness);
            Serial.print("PAUSE ");
            Serial.println(headGroup.is_paused, DEC);
            break;
        case 'T': // step_size
            data_val -= 'A';
            if (data_val <= 0)
                data_val = 1;
            headGroup.step_size = data_val;
            Serial.print("S2 STEP ");
            Serial.println(headGroup.step_size, DEC);
            break;
        case 'N': // particular color in palette
            for (i = 0; Serial.available() < 3 && i < 60; ++i)
                delay(50);
            headGroup.palette[data_val] = CRGB(Serial.read(), Serial.read(), Serial.read());
            break;
    }
}

void graphic_code() {
    bool hasShow = false;
    if (!mainGroup.is_paused) {
        hasShow = true;
        mainGroup.startIndex += mainGroup.step_size; /* motion speed */

        uint16_t colorIndex = mainGroup.startIndex / mainGroup.series_len;

        for (i = 0; i < mainGroup.num_leds; i++) {
            leds[mainGroup.first_led + i] = ColorFromPalette(mainGroup.palette, colorIndex, brightness, LINEARBLEND);
            colorIndex += 3;
        }
    }
    if (!headGroup.is_paused) {
        hasShow = true;
        headGroup.startIndex += headGroup.step_size; /* motion speed */

        uint16_t colorIndex = headGroup.startIndex / headGroup.series_len;

        for (i = 0; i < headGroup.num_leds; i++) {
            leds[headGroup.first_led + i] = ColorFromPalette(headGroup.palette, colorIndex, brightness, LINEARBLEND);
            colorIndex += 3;
        }
    }
    if (hasShow)
        FastLED.show();
}

inline void sleepControl() {
    c_time = millis() & 0xFFFFFFFF;
    frame_time = c_time - last_time;
    if (frame_time < update_time_ms)
        FastLED.delay(update_time_ms - frame_time);
    last_time = c_time;
}
