#include <FastLED.h>       // Include module foundation
#define NUM_LEDs 70        // Specify total LEDs count
#define LEDS_PIN 6         // Specify LEDs GPIO pin
#define BUTTON_WAIT 5      // Specify bu/tton hold time before mode switch
#define BUTTON_PIN 9       // Specify button GPIO pin
CRGB LEDs[NUM_LEDs];       // Pre-define LEDss array
int MODE_INDEX = 0;
int MODE_COUNT = 4;

// Initialization of module and basic configurations //
// ------------------------------------------------- //

void setup() { 
  FastLED.addLeds<NEOPIXEL, LEDS_PIN>(LEDs, NUM_LEDs);
  FastLED.setBrightness(50);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(96500);
}

// ------------------------------------------------- //
//                 Main event loop                   //
// ------------------------------------------------- //

void loop() {
    mode_button();
    switch (MODE_INDEX) {
        case 0: fade_flash(); break;
        case 1: even_flash(); break;
        case 2: sin_flash(); break;
        case 3: sequence_flash(); break;
        default: return;
    } 
}

// ------------------------------------------------- //
//               On change flash mode                //
// Create a flicker effect when the mode is changed  //
// ------------------------------------------------- //

int flicker_time = 3;
int flicker_duration = 500;

void change_mode_flash() {
    for (int j=0; j<flicker_time; j++) {
        for (int i=0; i<NUM_LEDs; i++) { LEDs[i] = CRGB::Crimson; }
        FastLED.show();
        delay(flicker_duration);
        FastLED.clear(false);
        FastLED.show();
        delay(flicker_duration);
    }
}

// ------------------------------------------------- //
//               Sequence flash mode                 //
// Create a long led strip that runs to the end and  //
// change color when its tip hit a wall              //
// ------------------------------------------------- //

int sequence_index = 0;
int sequence_delta = 1;
int sequence_delay = 80;
int sequence_pallate_index = 0;

CRGB sequence_color_pallate[2][2] = {
    {CRGB::Aqua, CRGB::Crimson},
    {CRGB::Green, CRGB::Yellow},
};

CRGB primary_color = sequence_color_pallate[sequence_pallate_index][0];
CRGB secondary_color = sequence_color_pallate[sequence_pallate_index][1];
CRGB sequence_color = primary_color;

void sequence_flash() { 
    EVERY_N_MILLISECONDS(sequence_delay) {
        if (sequence_index == NUM_LEDs - 1 && sequence_delta == 1) { 
            sequence_delta = -1; 
            sequence_color = secondary_color;
        }
        if (sequence_index == 0 && sequence_delta == -1) { 
            sequence_delta = 1;
            sequence_color = primary_color;
        }
        LEDs[sequence_index] = sequence_color;
        sequence_index += sequence_delta;
        FastLED.show(10);
    }
}

void sequence_update() {
    sequence_pallate_index++;
    int sequence_mode_count = sizeof(sequence_color_pallate) / sizeof(sequence_color_pallate[0]);
    if (sequence_pallate_index > sequence_mode_count-1) { sequence_pallate_index = 0; }
    primary_color = sequence_color_pallate[sequence_pallate_index][0];
    secondary_color = sequence_color_pallate[sequence_pallate_index][1];
    sequence_color = primary_color;
}

// ------------------------------------------------- //
//                   fade flash                      //
// Turn all LEDs on and change their brightness      //
// according to a correspond sin wave                //
// ------------------------------------------------- //

int fade_color_index = 0;
CRGB fade_color_pallate[10] = { 
    CRGB::Aqua,
    CRGB::DarkBlue,
    CRGB::BlueViolet,
    CRGB::Brown,
    CRGB::Coral,
    CRGB::Crimson,
    CRGB::DarkCyan,
    CRGB::DarkGoldenrod,
    CRGB::DarkGreen,
    CRGB::Purple
};
int fade_color_count = sizeof(fade_color_pallate) / sizeof(fade_color_pallate[0]);
CRGB fade_color = fade_color_pallate[fade_color_index];

void fade_update() {
    fade_color_index++;
    if (fade_color_index > fade_color_count-1) { fade_color_index = 0; }
    fade_color = fade_color_pallate[fade_color_index];
}

void fade_flash() {
    uint8_t sinBeat = beatsin8(25, 10, 220, 0, 0);
    for (int i=0; i<NUM_LEDs; i++) { 
        LEDs[i] = fade_color;
    }
    FastLED.show(sinBeat);
}

// ------------------------------------------------- //
//                    sin flash                      //
// Creating three sin wave LEDs representation each  //
// with offset with blur effect                      //
// ------------------------------------------------- //

int sin_color_index = 0;
CRGB sin_color_pallate[3][4] = {
    {CRGB::Crimson, CRGB::Aqua, CRGB::Green, CRGB::Yellow},
    {CRGB::Crimson, CRGB::Aqua, CRGB::Green, CRGB::Yellow},
    {CRGB::Crimson, CRGB::Aqua, CRGB::Green, CRGB::Yellow},
};
int sin_color_count = sizeof(sin_color_pallate) / sizeof(sin_color_pallate[0]);
CRGB sin_color[4];
for (int i=0; i<4; i++) { sin_color[i] = sin_color_pallate[sin_color_index][i]; }

void sin_update() {
    sin_color_index++; 
    if (sin_color_index > sin_color_count - 1) { sin_color_index = 0; }
}

void sin_flash() {
    uint8_t sinBeat = beatsin8(20, 0, NUM_LEDs-1, 0, 0);
    uint8_t sinBeat2 = beatsin8(20, 0, NUM_LEDs-1, 0, 51);
    uint8_t sinBeat3 = beatsin8(20, 0, NUM_LEDs-1, 0, 102);
    uint8_t sinBeat4 = beatsin8(20, 0, NUM_LEDs-1, 0, 204);
    LEDs[sinBeat] = sin_color[0];
    LEDs[sinBeat2] = sin_color[1];
    LEDs[sinBeat3] = sin_color[2];
    LEDs[sinBeat4] = sin_color[3];
    EVERY_N_MILLISECONDS(1) { for (int i=0; i<4; i++) { blur1d(LEDs, NUM_LEDs, 50); } }
    FastLED.show();
}

// ------------------------------------------------- //
//                    even flash                     //
// Alternate between the even and odd numbered flash //
// ------------------------------------------------- //

int even_delay = 1000;
bool last_even = false;
int even_color_index = 0;
int even_color_count = 3;
//CRGB even_color_pallate[3]


void even_update() {
    last_even = false;
}

void even_flash() {
    if (last_even) { for (int i=0; i<NUM_LEDs; i++) { if (i % 2 == 0) { LEDs[i] = CRGB::BlueViolet; } } }
    else { for (int i=0; i<NUM_LEDs; i++) { if (i % 2 != 0) { LEDs[i] = CRGB::Green; } } }

    FastLED.show();
    EVERY_N_MILLISECONDS(even_delay) {
        for (int i=0; i<NUM_LEDs; i++) { LEDs[i] = CRGB::Black; } FastLED.show();
        last_even = !last_even;
    }
}

// ------------------------------------------------- //
//                    reset function                 //
// Rstart led strip and other variables              //
// ------------------------------------------------- //
void reset() {
    sequence_index = 0;
    FastLED.clear(true);
}

// ------------------------------------------------- //
//                   Mode button                     //
// + Default: change LEDs flash mode                 //
// + Alternative: change mode after 5 secs hold      //
// changing the color of LEDs instead                //
// ------------------------------------------------- //
int button_mode_duration = 3000;
unsigned long time_pressed;
bool button_pressed = false;
bool button_last = false;
bool color_mode = true;
bool pressed_passed = false;
bool mode_changed = false;

void mode_button() {  
    int state = digitalRead(BUTTON_PIN);
    if (state == HIGH) { 
        if (mode_changed) {
            mode_changed = false;
            button_pressed = false;
            return;
        }
        if (button_pressed) {
            if (color_mode) {
                switch (MODE_INDEX) {
                    case 0:
                        break;
                    case 1:
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    default: return;
                }
                reset();
                FastLED.show();
            } else { 
                MODE_INDEX++;
                if (MODE_INDEX > MODE_COUNT-1) { MODE_INDEX = 0; }
                reset();
                FastLED.show();
            }
        }
        button_pressed = false;
        pressed_passed = false;
    } else {
        if (!button_pressed) {
            time_pressed = millis();
            pressed_passed = false;
        }
        if (millis() - time_pressed > button_mode_duration && !pressed_passed) { 
            color_mode = !color_mode;
            pressed_passed = true;
            mode_changed = true;
            change_mode_flash();
            reset();
        }
        button_pressed = true;
    }
}
