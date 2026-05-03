/*
 * FluxGarage RoboEyes for OLED Displays - ESP8266 Port
 * Adapted for ESP8266 + SSD1306 128x64 OLED (I2C)
 *
 * Original library by Dennis Hoelscher (FluxGarage)
 * www.fluxgarage.com
 * Ported for ESP8266 / Adafruit_SSD1306
 *
 * Required libraries (install via Arduino Library Manager):
 *   - Adafruit SSD1306
 *   - Adafruit GFX Library
 *
 * Wiring (ESP8266 NodeMCU / Wemos D1):
 *   OLED SDA  -> D2 (GPIO4)
 *   OLED SCL  -> D1 (GPIO5)
 *   OLED VCC  -> 3.3V
 *   OLED GND  -> GND
 *
 * License: GNU GPL v3 (same as original)
 */

#ifndef _FLUXGARAGE_ROBOEYES_ESP8266_H
#define _FLUXGARAGE_ROBOEYES_ESP8266_H

#include <Arduino.h>

// Display colors (monochrome SSD1306)
uint8_t BGCOLOR   = BLACK;  // 0
uint8_t MAINCOLOR = WHITE;  // 1

// Mood types — renamed to avoid clash with ESP8266 Arduino.h #define DEFAULT 1
#define MOOD_DEFAULT 0
#define MOOD_TIRED   1
#define MOOD_ANGRY   2
#define MOOD_HAPPY   3

// On / Off
#define ON  1
#define OFF 0

// Predefined gaze positions — renamed to avoid future conflicts
#define POS_DEFAULT  0  // middle center
#define POS_N        1  // north, top center
#define POS_NE       2  // north-east, top right
#define POS_E        3  // east, middle right
#define POS_SE       4  // south-east, bottom right
#define POS_S        5  // south, bottom center
#define POS_SW       6  // south-west, bottom left
#define POS_W        7  // west, middle left
#define POS_NW       8  // north-west, top left

// ─────────────────────────────────────────────────────────────────────────────
// Template class
// Example:
//   Adafruit_SSD1306 display(128, 64, &Wire, -1);
//   RoboEyes<Adafruit_SSD1306> eyes(display);
// ─────────────────────────────────────────────────────────────────────────────
template<typename AdafruitDisplay>
class RoboEyes
{
public:

  AdafruitDisplay *display;

  // Screen / frame-rate
  int screenWidth    = 128;
  int screenHeight   = 64;
  int frameInterval  = 20;
  unsigned long fpsTimer = 0;

  // Mood / expression flags
  bool tired   = false;
  bool angry   = false;
  bool happy   = false;
  bool curious = false;
  bool cyclops = false;
  bool eyeL_open = false;
  bool eyeR_open = false;

  // Eye geometry — LEFT
  int  eyeLwidthDefault  = 36;
  int  eyeLheightDefault = 36;
  int  eyeLwidthCurrent  = 36;
  int  eyeLheightCurrent = 1;
  int  eyeLwidthNext     = 36;
  int  eyeLheightNext    = 36;
  int  eyeLheightOffset  = 0;
  byte eyeLborderRadiusDefault = 8;
  byte eyeLborderRadiusCurrent = 8;
  byte eyeLborderRadiusNext    = 8;

  // Eye geometry — RIGHT
  int  eyeRwidthDefault  = 36;
  int  eyeRheightDefault = 36;
  int  eyeRwidthCurrent  = 36;
  int  eyeRheightCurrent = 1;
  int  eyeRwidthNext     = 36;
  int  eyeRheightNext    = 36;
  int  eyeRheightOffset  = 0;
  byte eyeRborderRadiusDefault = 8;
  byte eyeRborderRadiusCurrent = 8;
  byte eyeRborderRadiusNext    = 8;

  // Space between eyes
  int spaceBetweenDefault = 10;
  int spaceBetweenCurrent = 10;
  int spaceBetweenNext    = 10;

  // Eyelid heights
  byte eyelidsTiredHeight          = 0;
  byte eyelidsTiredHeightNext      = 0;
  byte eyelidsAngryHeight          = 0;
  byte eyelidsAngryHeightNext      = 0;
  byte eyelidsHappyBottomOffset    = 0;
  byte eyelidsHappyBottomOffsetNext = 0;

  // Eye coordinates
  int eyeLxDefault = 0, eyeLyDefault = 0;
  int eyeLx = 0, eyeLy = 0;
  int eyeLxNext = 0, eyeLyNext = 0;
  int eyeRxDefault = 0, eyeRyDefault = 0;
  int eyeRx = 0, eyeRy = 0;
  int eyeRxNext = 0, eyeRyNext = 0;

  // Horizontal flicker
  bool  hFlicker          = false;
  bool  hFlickerAlternate = false;
  byte  hFlickerAmplitude = 2;

  // Vertical flicker
  bool  vFlicker          = false;
  bool  vFlickerAlternate = false;
  byte  vFlickerAmplitude = 10;

  // Auto-blink
  bool          autoblinker            = false;
  int           blinkInterval          = 1;
  int           blinkIntervalVariation = 4;
  unsigned long blinktimer             = 0;

  // Idle
  bool          idle                  = false;
  int           idleInterval          = 1;
  int           idleIntervalVariation = 3;
  unsigned long idleAnimationTimer    = 0;

  // Confused
  bool          confused                  = false;
  unsigned long confusedAnimationTimer    = 0;
  int           confusedAnimationDuration = 500;
  bool          confusedToggle            = true;

  // Laugh
  bool          laugh                  = false;
  unsigned long laughAnimationTimer    = 0;
  int           laughAnimationDuration = 500;
  bool          laughToggle            = true;

  // Sweat drops
  bool  sweat             = false;
  byte  sweatBorderradius = 3;

  int   sweat1XPosInitial = 2;  int   sweat1XPos = 2;
  float sweat1YPos = 2;         int   sweat1YPosMax = 0;
  float sweat1Height = 2;       float sweat1Width = 1;

  int   sweat2XPosInitial = 2;  int   sweat2XPos = 2;
  float sweat2YPos = 2;         int   sweat2YPosMax = 0;
  float sweat2Height = 2;       float sweat2Width = 1;

  int   sweat3XPosInitial = 2;  int   sweat3XPos = 2;
  float sweat3YPos = 2;         int   sweat3YPosMax = 0;
  float sweat3Height = 2;       float sweat3Width = 1;

  // Constructor
  RoboEyes(AdafruitDisplay &disp) : display(&disp) {}

  // begin()
  void begin(int width, int height, byte frameRate) {
    screenWidth  = width;
    screenHeight = height;

    eyeLxDefault = (screenWidth - (eyeLwidthDefault + spaceBetweenDefault + eyeRwidthDefault)) / 2;
    eyeLyDefault = (screenHeight - eyeLheightDefault) / 2;
    eyeLx = eyeLxDefault;  eyeLy = eyeLyDefault;
    eyeLxNext = eyeLx;     eyeLyNext = eyeLy;

    eyeRxDefault = eyeLx + eyeLwidthCurrent + spaceBetweenDefault;
    eyeRyDefault = eyeLy;
    eyeRx = eyeRxDefault;  eyeRy = eyeRyDefault;
    eyeRxNext = eyeRx;     eyeRyNext = eyeRy;

    eyeLheightCurrent = 1;
    eyeRheightCurrent = 1;

    display->clearDisplay();
    display->display();
    setFramerate(frameRate);
  }

  // update() — call every loop()
  void update() {
    if (millis() - fpsTimer >= (unsigned long)frameInterval) {
      drawEyes();
      fpsTimer = millis();
    }
    yield();
  }

  // ── Setters ──────────────────────────────────────────────────────────────

  void setFramerate(byte fps) { frameInterval = 1000 / fps; }

  void setDisplayColors(uint8_t background, uint8_t main) {
    BGCOLOR = background; MAINCOLOR = main;
  }

  void setWidth(byte l, byte r) {
    eyeLwidthNext = l; eyeRwidthNext = r;
    eyeLwidthDefault = l; eyeRwidthDefault = r;
  }
  void setHeight(byte l, byte r) {
    eyeLheightNext = l; eyeRheightNext = r;
    eyeLheightDefault = l; eyeRheightDefault = r;
  }
  void setBorderradius(byte l, byte r) {
    eyeLborderRadiusNext = l; eyeRborderRadiusNext = r;
    eyeLborderRadiusDefault = l; eyeRborderRadiusDefault = r;
  }
  void setSpacebetween(int s) { spaceBetweenNext = s; spaceBetweenDefault = s; }

  void setMood(uint8_t mood) {
    tired = false; angry = false; happy = false;
    if (mood == MOOD_TIRED)  tired = true;
    if (mood == MOOD_ANGRY)  angry = true;
    if (mood == MOOD_HAPPY)  happy = true;
  }

  void setPosition(uint8_t position) {
    switch (position) {
      case POS_N:  eyeLxNext = getScreenConstraint_X()/2; eyeLyNext = 0; break;
      case POS_NE: eyeLxNext = getScreenConstraint_X();   eyeLyNext = 0; break;
      case POS_E:  eyeLxNext = getScreenConstraint_X();   eyeLyNext = getScreenConstraint_Y()/2; break;
      case POS_SE: eyeLxNext = getScreenConstraint_X();   eyeLyNext = getScreenConstraint_Y(); break;
      case POS_S:  eyeLxNext = getScreenConstraint_X()/2; eyeLyNext = getScreenConstraint_Y(); break;
      case POS_SW: eyeLxNext = 0;                         eyeLyNext = getScreenConstraint_Y(); break;
      case POS_W:  eyeLxNext = 0;                         eyeLyNext = getScreenConstraint_Y()/2; break;
      case POS_NW: eyeLxNext = 0;                         eyeLyNext = 0; break;
      default:     eyeLxNext = getScreenConstraint_X()/2; eyeLyNext = getScreenConstraint_Y()/2; break;
    }
  }

  void setAutoblinker(bool active, int interval, int variation) {
    autoblinker = active; blinkInterval = interval; blinkIntervalVariation = variation;
  }
  void setAutoblinker(bool active) { autoblinker = active; }

  void setIdleMode(bool active, int interval, int variation) {
    idle = active; idleInterval = interval; idleIntervalVariation = variation;
  }
  void setIdleMode(bool active) { idle = active; }

  void setCuriosity(bool b) { curious = b; }
  void setCyclops(bool b)   { cyclops = b; }

  void setHFlicker(bool b, byte amp) { hFlicker = b; hFlickerAmplitude = amp; }
  void setHFlicker(bool b)           { hFlicker = b; }
  void setVFlicker(bool b, byte amp) { vFlicker = b; vFlickerAmplitude = amp; }
  void setVFlicker(bool b)           { vFlicker = b; }

  void setSweat(bool b) { sweat = b; }

  // ── Getters ──────────────────────────────────────────────────────────────

  int getScreenConstraint_X() { return screenWidth  - eyeLwidthCurrent - spaceBetweenCurrent - eyeRwidthCurrent; }
  int getScreenConstraint_Y() { return screenHeight - eyeLheightDefault; }

  // ── Basic animations ─────────────────────────────────────────────────────

  void close() { eyeLheightNext=1; eyeL_open=false; eyeRheightNext=1; eyeR_open=false; }
  void open()  { eyeL_open=true; eyeR_open=true; }
  void blink() { close(); open(); }

  void close(bool l, bool r) {
    if(l){ eyeLheightNext=1; eyeL_open=false; }
    if(r){ eyeRheightNext=1; eyeR_open=false; }
  }
  void open(bool l, bool r) { if(l) eyeL_open=true; if(r) eyeR_open=true; }
  void blink(bool l, bool r) { close(l,r); open(l,r); }

  // ── Macro animations ─────────────────────────────────────────────────────

  void anim_confused() { confused = true; }
  void anim_laugh()    { laugh    = true; }

  // ── drawEyes() ───────────────────────────────────────────────────────────

  void drawEyes() {

    // Curious height offsets
    if (curious) {
      eyeLheightOffset = (eyeLxNext <= 10 || (eyeLxNext >= (getScreenConstraint_X()-10) && cyclops)) ? 8 : 0;
      eyeRheightOffset = (eyeRxNext >= screenWidth - eyeRwidthCurrent - 10) ? 8 : 0;
    } else {
      eyeLheightOffset = 0; eyeRheightOffset = 0;
    }

    // Smooth size tweening
    eyeLheightCurrent = (eyeLheightCurrent + eyeLheightNext + eyeLheightOffset) / 2;
    eyeLy += (eyeLheightDefault - eyeLheightCurrent) / 2;
    eyeLy -= eyeLheightOffset / 2;

    eyeRheightCurrent = (eyeRheightCurrent + eyeRheightNext + eyeRheightOffset) / 2;
    eyeRy += (eyeRheightDefault - eyeRheightCurrent) / 2;
    eyeRy -= eyeRheightOffset / 2;

    // Re-open after blink
    if (eyeL_open && eyeLheightCurrent <= 1 + eyeLheightOffset) eyeLheightNext = eyeLheightDefault;
    if (eyeR_open && eyeRheightCurrent <= 1 + eyeRheightOffset) eyeRheightNext = eyeRheightDefault;

    // Width tweening
    eyeLwidthCurrent = (eyeLwidthCurrent + eyeLwidthNext) / 2;
    eyeRwidthCurrent = (eyeRwidthCurrent + eyeRwidthNext) / 2;

    // Space tweening
    spaceBetweenCurrent = (spaceBetweenCurrent + spaceBetweenNext) / 2;

    // Position tweening
    eyeLx = (eyeLx + eyeLxNext) / 2;
    eyeLy = (eyeLy + eyeLyNext) / 2;
    eyeRxNext = eyeLxNext + eyeLwidthCurrent + spaceBetweenCurrent;
    eyeRyNext = eyeLyNext;
    eyeRx = (eyeRx + eyeRxNext) / 2;
    eyeRy = (eyeRy + eyeRyNext) / 2;

    // Border radius tweening
    eyeLborderRadiusCurrent = (eyeLborderRadiusCurrent + eyeLborderRadiusNext) / 2;
    eyeRborderRadiusCurrent = (eyeRborderRadiusCurrent + eyeRborderRadiusNext) / 2;

    // Auto-blink
    if (autoblinker && millis() >= blinktimer) {
      blink();
      blinktimer = millis() + (blinkInterval*1000UL) + (random(blinkIntervalVariation)*1000UL);
    }

    // Laugh
    if (laugh) {
      if (laughToggle) { setVFlicker(true,5); laughAnimationTimer=millis(); laughToggle=false; }
      else if (millis() >= laughAnimationTimer+laughAnimationDuration) { setVFlicker(false,0); laughToggle=true; laugh=false; }
    }

    // Confused
    if (confused) {
      if (confusedToggle) { setHFlicker(true,20); confusedAnimationTimer=millis(); confusedToggle=false; }
      else if (millis() >= confusedAnimationTimer+confusedAnimationDuration) { setHFlicker(false,0); confusedToggle=true; confused=false; }
    }

    // Idle
    if (idle && millis() >= idleAnimationTimer) {
      eyeLxNext = random(getScreenConstraint_X());
      eyeLyNext = random(getScreenConstraint_Y());
      idleAnimationTimer = millis() + (idleInterval*1000UL) + (random(idleIntervalVariation)*1000UL);
    }

    // H-flicker
    if (hFlicker) {
      int o = hFlickerAlternate ? hFlickerAmplitude : -hFlickerAmplitude;
      eyeLx += o; eyeRx += o;
      hFlickerAlternate = !hFlickerAlternate;
    }

    // V-flicker
    if (vFlicker) {
      int o = vFlickerAlternate ? vFlickerAmplitude : -vFlickerAmplitude;
      eyeLy += o; eyeRy += o;
      vFlickerAlternate = !vFlickerAlternate;
    }

    // Cyclops
    if (cyclops) { eyeRwidthCurrent=0; eyeRheightCurrent=0; spaceBetweenCurrent=0; }

    // Draw
    display->clearDisplay();
    display->fillRoundRect(eyeLx, eyeLy, eyeLwidthCurrent, eyeLheightCurrent, eyeLborderRadiusCurrent, MAINCOLOR);
    if (!cyclops)
      display->fillRoundRect(eyeRx, eyeRy, eyeRwidthCurrent, eyeRheightCurrent, eyeRborderRadiusCurrent, MAINCOLOR);

    // Eyelid transitions
    if (tired)  { eyelidsTiredHeightNext = eyeLheightCurrent/2; eyelidsAngryHeightNext = 0; }
    else        { eyelidsTiredHeightNext = 0; }
    if (angry)  { eyelidsAngryHeightNext = eyeLheightCurrent/2; eyelidsTiredHeightNext = 0; }
    else        { eyelidsAngryHeightNext = 0; }
    if (happy)  { eyelidsHappyBottomOffsetNext = eyeLheightCurrent/2; }
    else        { eyelidsHappyBottomOffsetNext = 0; }

    // Tired eyelids
    eyelidsTiredHeight = (eyelidsTiredHeight + eyelidsTiredHeightNext) / 2;
    if (!cyclops) {
      display->fillTriangle(eyeLx, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx, eyeLy+eyelidsTiredHeight-1, BGCOLOR);
      display->fillTriangle(eyeRx, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy+eyelidsTiredHeight-1, BGCOLOR);
    } else {
      display->fillTriangle(eyeLx, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx, eyeLy+eyelidsTiredHeight-1, BGCOLOR);
      display->fillTriangle(eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy+eyelidsTiredHeight-1, BGCOLOR);
    }

    // Angry eyelids
    eyelidsAngryHeight = (eyelidsAngryHeight + eyelidsAngryHeightNext) / 2;
    if (!cyclops) {
      display->fillTriangle(eyeLx, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy+eyelidsAngryHeight-1, BGCOLOR);
      display->fillTriangle(eyeRx, eyeRy-1, eyeRx+eyeRwidthCurrent, eyeRy-1, eyeRx, eyeRy+eyelidsAngryHeight-1, BGCOLOR);
    } else {
      display->fillTriangle(eyeLx, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy+eyelidsAngryHeight-1, BGCOLOR);
      display->fillTriangle(eyeLx+(eyeLwidthCurrent/2), eyeLy-1, eyeLx+eyeLwidthCurrent, eyeLy-1, eyeLx+(eyeLwidthCurrent/2), eyeLy+eyelidsAngryHeight-1, BGCOLOR);
    }

    // Happy eyelids
    eyelidsHappyBottomOffset = (eyelidsHappyBottomOffset + eyelidsHappyBottomOffsetNext) / 2;
    display->fillRoundRect(eyeLx-1, (eyeLy+eyeLheightCurrent)-eyelidsHappyBottomOffset+1, eyeLwidthCurrent+2, eyeLheightDefault, eyeLborderRadiusCurrent, BGCOLOR);
    if (!cyclops)
      display->fillRoundRect(eyeRx-1, (eyeRy+eyeRheightCurrent)-eyelidsHappyBottomOffset+1, eyeRwidthCurrent+2, eyeRheightDefault, eyeRborderRadiusCurrent, BGCOLOR);

    // Sweat drops
    if (sweat) {
      _drawSweatDrop(sweat1XPos, sweat1XPosInitial, sweat1YPos, sweat1YPosMax, sweat1Width, sweat1Height, 0, 30);
      _drawSweatDrop(sweat2XPos, sweat2XPosInitial, sweat2YPos, sweat2YPosMax, sweat2Width, sweat2Height, 30, screenWidth-60);
      _drawSweatDrop(sweat3XPos, sweat3XPosInitial, sweat3YPos, sweat3YPosMax, sweat3Width, sweat3Height, screenWidth-30, 30);
    }

    display->display();
  }

private:
  void _drawSweatDrop(int &xPos, int &xPosInitial, float &yPos, int &yPosMax,
                      float &w, float &h, int xOff, int xRange) {
    if (yPos <= yPosMax) { yPos += 0.5f; }
    else { xPosInitial=xOff+random(xRange); yPos=2; yPosMax=random(10)+10; w=1; h=2; }
    if (yPos <= yPosMax/2) { w+=0.5f; h+=0.5f; } else { w-=0.1f; h-=0.5f; }
    xPos = xPosInitial - (int)(w/2);
    display->fillRoundRect(xPos,(int)yPos,(int)w,(int)h,sweatBorderradius,MAINCOLOR);
  }

}; // end class RoboEyes

#endif
