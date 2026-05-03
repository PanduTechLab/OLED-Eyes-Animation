/*
 * ============================================================
 *  FluxGarage RoboEyes — ESP8266 + SSD1306 OLED
 *  MERGED & OPTIMIZED — All Expressions + Animation Sequences
 * ============================================================
 *
 *  Required libraries (Arduino Library Manager):
 *    - Adafruit SSD1306
 *    - Adafruit GFX Library
 *
 *  Wiring (NodeMCU / Wemos D1 Mini — I2C):
 *    OLED SDA  ->  D2  (GPIO 4)
 *    OLED SCL  ->  D1  (GPIO 5)
 *    OLED VCC  ->  3.3V
 *    OLED GND  ->  GND
 *
 *  Put BOTH files in the same folder:
 *    RoboEyes_ESP8266_AllExpressions.ino
 *    FluxGarage_RoboEyes_ESP8266.h
 * ============================================================
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "FluxGarage_RoboEyes_ESP8266.h"

// ── Display config ────────────────────────────────────────────────────────────
#define SCREEN_W   128
#define SCREEN_H    64
#define OLED_RESET  -1
#define I2C_ADDR  0x3C   // change to 0x3D if display stays blank

#define SDA_PIN  4       // D2
#define SCL_PIN  5       // D1

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, OLED_RESET);
RoboEyes<Adafruit_SSD1306> eyes(display);

// ── Demo step descriptor ──────────────────────────────────────────────────────
struct DemoStep {
  uint8_t  mood;       // MOOD_DEFAULT / MOOD_HAPPY / MOOD_TIRED / MOOD_ANGRY
  uint8_t  position;   // POS_DEFAULT / POS_N / POS_NE … POS_NW
  bool     curious;
  bool     cyclops;
  bool     autoblink;
  bool     idle;
  bool     hFlicker;
  bool     vFlicker;
  bool     sweat;
  bool     confused;
  bool     laugh;
  uint16_t duration;   // ms to hold this step
  const char *label;
};

// ── All expressions from every merged example ─────────────────────────────────
const DemoStep DEMO[] = {
  // mood           pos          cur    cyc    blink  idle   hFl    vFl    swt    conf   lgh    dur    label
  { MOOD_DEFAULT, POS_DEFAULT,  false, false, true,  false, false, false, false, false, false, 3000,  "Default + AutoBlink"     },
  { MOOD_DEFAULT, POS_DEFAULT,  false, false, true,  true,  false, false, false, false, false, 4000,  "Default + Idle"          },
  { MOOD_DEFAULT, POS_DEFAULT,  true,  false, true,  true,  false, false, false, false, false, 4000,  "Curious + Idle"          },
  { MOOD_HAPPY,   POS_DEFAULT,  false, false, true,  false, false, false, false, false, false, 3000,  "Happy"                   },
  { MOOD_TIRED,   POS_DEFAULT,  false, false, true,  false, false, false, false, false, false, 3000,  "Tired"                   },
  { MOOD_ANGRY,   POS_DEFAULT,  false, false, true,  false, false, false, false, false, false, 3000,  "Angry"                   },
  { MOOD_HAPPY,   POS_DEFAULT,  true,  false, true,  true,  false, false, false, false, false, 4000,  "Happy + Curious + Idle"  },
  { MOOD_ANGRY,   POS_DEFAULT,  true,  false, true,  true,  false, false, false, false, false, 4000,  "Angry + Curious + Idle"  },
  // Gaze positions
  { MOOD_DEFAULT, POS_N,        false, false, false, false, false, false, false, false, false, 2000,  "Look North"              },
  { MOOD_DEFAULT, POS_NE,       false, false, false, false, false, false, false, false, false, 2000,  "Look NE"                 },
  { MOOD_DEFAULT, POS_E,        false, false, false, false, false, false, false, false, false, 2000,  "Look East"               },
  { MOOD_DEFAULT, POS_SE,       false, false, false, false, false, false, false, false, false, 2000,  "Look SE"                 },
  { MOOD_DEFAULT, POS_S,        false, false, false, false, false, false, false, false, false, 2000,  "Look South"              },
  { MOOD_DEFAULT, POS_SW,       false, false, false, false, false, false, false, false, false, 2000,  "Look SW"                 },
  { MOOD_DEFAULT, POS_W,        false, false, false, false, false, false, false, false, false, 2000,  "Look West"               },
  { MOOD_DEFAULT, POS_NW,       false, false, false, false, false, false, false, false, false, 2000,  "Look NW"                 },
  { MOOD_DEFAULT, POS_DEFAULT,  false, false, false, false, false, false, false, false, false, 2000,  "Look Center"             },
  // One-shot animations
  { MOOD_DEFAULT, POS_DEFAULT,  false, false, false, false, false, false, false, true,  false, 2500,  "Confused"                },
  { MOOD_HAPPY,   POS_DEFAULT,  false, false, false, false, false, false, false, false, true,  2500,  "Laugh"                   },
  { MOOD_DEFAULT, POS_DEFAULT,  false, false, false, false, true,  false, false, false, false, 2000,  "H-Flicker"               },
  { MOOD_DEFAULT, POS_DEFAULT,  false, false, false, false, false, true,  false, false, false, 2000,  "V-Flicker"               },
  { MOOD_DEFAULT, POS_DEFAULT,  false, false, false, false, false, false, true,  false, false, 3000,  "Sweat"                   },
  // Cyclops mode
  { MOOD_DEFAULT, POS_DEFAULT,  false, true,  true,  false, false, false, false, false, false, 3000,  "Cyclops Default"         },
  { MOOD_HAPPY,   POS_DEFAULT,  false, true,  true,  false, false, false, false, false, false, 3000,  "Cyclops Happy"           },
  { MOOD_ANGRY,   POS_DEFAULT,  false, true,  true,  false, false, false, false, false, false, 3000,  "Cyclops Angry"           },
  { MOOD_TIRED,   POS_DEFAULT,  false, true,  true,  false, false, false, false, false, false, 3000,  "Cyclops Tired"           },
  { MOOD_DEFAULT, POS_DEFAULT,  true,  true,  true,  true,  false, false, false, false, false, 4000,  "Cyclops Curious+Idle"    },
  // Combined sequences
  { MOOD_DEFAULT, POS_DEFAULT,  false, false, true,  true,  false, false, false, true,  false, 3000,  "Confused + Idle"         },
  { MOOD_HAPPY,   POS_DEFAULT,  true,  false, true,  true,  false, false, false, false, true,  3000,  "Happy Laugh + Idle"      },
  { MOOD_ANGRY,   POS_DEFAULT,  false, false, true,  false, true,  false, false, false, false, 3000,  "Angry + H-Flicker"       },
  { MOOD_TIRED,   POS_DEFAULT,  false, false, true,  true,  false, false, true,  false, false, 4000,  "Tired + Sweat + Idle"    },
};

const uint8_t DEMO_COUNT = sizeof(DEMO) / sizeof(DEMO[0]);

uint8_t       stepIndex = 0;
unsigned long stepTimer = 0;

// ── Apply one demo step ───────────────────────────────────────────────────────
void applyStep(const DemoStep &s) {
  eyes.setMood(s.mood);
  eyes.setCuriosity(s.curious);
  eyes.setCyclops(s.cyclops);
  eyes.setAutoblinker(s.autoblink, 2, 3);
  eyes.setIdleMode(s.idle, 1, 2);
  eyes.setHFlicker(s.hFlicker, 20);
  eyes.setVFlicker(s.vFlicker, 10);
  eyes.setSweat(s.sweat);
  if (s.confused) eyes.anim_confused();
  if (s.laugh)    eyes.anim_laugh();
  if (!s.idle)    eyes.setPosition(s.position);

  Serial.print(F("["));
  Serial.print(stepIndex + 1);
  Serial.print(F("/"));
  Serial.print(DEMO_COUNT);
  Serial.print(F("] "));
  Serial.println(s.label);
}

// ── Reset all toggles before applying next step ───────────────────────────────
void resetAll() {
  eyes.setHFlicker(false);
  eyes.setVFlicker(false);
  eyes.setSweat(false);
  eyes.setIdleMode(false);
  eyes.setAutoblinker(false);
  eyes.setCyclops(false);
  eyes.setCuriosity(false);
  eyes.setMood(MOOD_DEFAULT);
}

// ── setup() ──────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println(F("\n=== RoboEyes ESP8266 — All Expressions ==="));

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR)) {
    Serial.println(F("SSD1306 not found! Check wiring / I2C address (0x3C or 0x3D)."));
    while (true) yield();
  }

  eyes.begin(SCREEN_W, SCREEN_H, 50);  // 50 fps
  eyes.setWidth(36, 36);
  eyes.setHeight(36, 36);
  eyes.setBorderradius(8, 8);
  eyes.setSpacebetween(10);

  applyStep(DEMO[0]);
  stepTimer = millis();
}

// ── loop() ───────────────────────────────────────────────────────────────────
void loop() {
  eyes.update();  // draws frame + feeds watchdog via yield()

  if (millis() - stepTimer >= DEMO[stepIndex].duration) {
    resetAll();
    stepIndex = (stepIndex + 1) % DEMO_COUNT;
    applyStep(DEMO[stepIndex]);
    stepTimer = millis();
  }
}
