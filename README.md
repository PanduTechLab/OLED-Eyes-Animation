# OLED-Eyes-Animation
Animated robot eyes on an  OLED display using ESP8266
This project brings expressive, smoothly animated robot eyes to a 128x64 SSD1306 OLED display using an ESP8266 (NodeMCU / Wemos D1 Mini). It is a full port and merge of the FluxGarage RoboEyes library, adapted to work with the ESP8266 toolchain and covering all available expressions and animation sequences in a single optimized sketch.
Features:

Moods: Default, Happy, Tired, Angry
Animations: Blinking, Laughing, Confused, Idle gaze, Sweat drops
Gaze positions: 8 directions (N, NE, E, SE, S, SW, W, NW)
Special modes: Cyclops (single eye), Curious (eye grows when looking sideways)
Flicker effects: horizontal and vertical
Smooth tweening on all transitions
Watchdog-safe for ESP8266 (yield() handled internally)

Hardware:

ESP8266 NodeMCU or Wemos D1 Mini
SSD1306 128x64 OLED display (I2C)
SDA → D2 (GPIO4), SCL → D1 (GPIO5)

Libraries required:

Adafruit SSD1306
Adafruit GFX Library

Based on the original FluxGarage RoboEyes library by Dennis Hoelscher.

