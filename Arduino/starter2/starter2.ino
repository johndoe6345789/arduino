// Nano R4 Minima — RGB LED “chatbot” controller over USB serial
// Open Serial Monitor @ 115200 and try:
//   rainbow | breathe | police | candle | strobe | pulse red | solid blue
//   color 255 0 120 | color orange | brightness 180 | speed 2 | off | status | help

#include <Arduino.h>
#include <math.h>
#include <stdio.h>

// ---------- State & Types ----------
enum Pattern : uint8_t {
  P_OFF,
  P_RAINBOW,
  P_BREATHE,
  P_POLICE,
  P_CANDLE,
  P_STROBE,
  P_PULSE,
  P_SOLID
};

static Pattern current = P_RAINBOW;
static uint8_t brightness = 128;   // 0..255
static float   speedMul   = 1.0f;  // 0.1..8
static uint8_t solidR=255, solidG=0, solidB=0;
static uint16_t hue = 0;
static uint32_t lastStep = 0;
static String inBuf;

// ---------- LED helpers ----------
static inline void setRGB_raw(uint8_t r, uint8_t g, uint8_t b) {
  // Common-anode: invert PWM
  analogWrite(LEDR, 255 - r);
  analogWrite(LEDG, 255 - g);
  analogWrite(LEDB, 255 - b);
}

static uint8_t clampU8(int v) { return (v<0)?0:((v>255)?255:(uint8_t)v); }

static void hsv2rgb(uint16_t h, uint8_t s, uint8_t v,
                    uint8_t &r, uint8_t &g, uint8_t &b) {
  uint8_t c = (uint16_t)v * s / 255;
  uint8_t x = (uint16_t)c * (60 - abs((h % 120) - 60)) / 60;
  uint8_t m = v - c;
  uint8_t rp=0,gp=0,bp=0;
  if (h < 60)       { rp=c; gp=x; bp=0; }
  else if (h < 120) { rp=x; gp=c; bp=0; }
  else if (h < 180) { rp=0; gp=c; bp=x; }
  else if (h < 240) { rp=0; gp=x; bp=c; }
  else if (h < 300) { rp=x; gp=0; bp=c; }
  else              { rp=c; gp=0; bp=x; }
  r = rp + m; g = gp + m; b = bp + m;
}

static void outRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t br) {
  uint16_t R = (uint16_t)r * br / 255;
  uint16_t G = (uint16_t)g * br / 255;
  uint16_t B = (uint16_t)b * br / 255;
  setRGB_raw((uint8_t)R, (uint8_t)G, (uint8_t)B);
}

// ---------- Bot helpers ----------
static void botSay(const String& s){ Serial.print(F("[bot] ")); Serial.println(s); }

static void setPattern(Pattern p) {
  current = p;
  lastStep = millis();
}

static void showStatus() {
  const char* pname =
    (current==P_OFF)?"off":
    (current==P_RAINBOW)?"rainbow":
    (current==P_BREATHE)?"breathe":
    (current==P_POLICE)?"police":
    (current==P_CANDLE)?"candle":
    (current==P_STROBE)?"strobe":
    (current==P_PULSE)?"pulse":"solid";
  Serial.print(F("[bot] pattern=")); Serial.print(pname);
  Serial.print(F(" brightness=")); Serial.print(brightness);
  Serial.print(F(" speed=")); Serial.print(speedMul,2);
  Serial.print(F(" color=(")); Serial.print(solidR); Serial.print(',');
  Serial.print(solidG); Serial.print(','); Serial.print(solidB); Serial.println(')');
}

static void showHelp() {
  botSay(F("Commands:"));
  Serial.println(F("  rainbow | breathe | police | candle | strobe | pulse <color> | solid <color> | off"));
  Serial.println(F("  color <r 0-255> <g 0-255> <b 0-255>  OR  color <name>"));
  Serial.println(F("  names: red, green, blue, white, purple, pink, yellow, orange, cyan, magenta, amber"));
  Serial.println(F("  brightness <0-255>"));
  Serial.println(F("  speed <float e.g. 0.5, 1, 2>"));
  Serial.println(F("  status | help"));
}

static bool parseColorName(const String& name, uint8_t &r, uint8_t &g, uint8_t &b) {
  String n = name; n.trim(); n.toLowerCase();
  if (n=="red")    { r=255; g=0;   b=0;   return true; }
  if (n=="green")  { r=0;   g=255; b=0;   return true; }
  if (n=="blue")   { r=0;   g=0;   b=255; return true; }
  if (n=="white")  { r=255; g=255; b=255; return true; }
  if (n=="purple") { r=180; g=0;   b=255; return true; }
  if (n=="pink")   { r=255; g=80;  b=160; return true; }
  if (n=="yellow") { r=255; g=255; b=0;   return true; }
  if (n=="orange") { r=255; g=140; b=0;   return true; }
  if (n=="cyan")   { r=0;   g=255; b=255; return true; }
  if (n=="magenta"){ r=255; g=0;   b=255; return true; }
  if (n=="amber")  { r=255; g=191; b=0;   return true; }
  return false;
}

// ---------- Patterns (non-blocking) ----------
static void stepRainbow() {
  const uint32_t stepMsBase = 10;
  if (millis() - lastStep < (uint32_t)(stepMsBase / speedMul)) return;
  lastStep += (uint32_t)(stepMsBase / speedMul);

  hue = (hue + 1) % 360;
  uint8_t r,g,b; hsv2rgb(hue, 255, 255, r, g, b);
  outRGB(r,g,b, brightness);
}

static void stepBreathe() {
  const uint32_t stepMsBase = 10;
  static uint16_t t = 0; // 0..1023
  if (millis() - lastStep < (uint32_t)(stepMsBase / speedMul)) return;
  lastStep += (uint32_t)(stepMsBase / speedMul);

  t = (t + 2) & 1023;
  float phase = (float)t * 6.2831853f / 1024.0f;
  float f = 0.55f + 0.45f * (0.5f * (1.0f + cosf(phase))); // 0.55..1.0
  uint8_t r,g,b; hsv2rgb(hue, 20, 255, r, g, b);
  outRGB((uint8_t)(r*f), (uint8_t)(g*f), (uint8_t)(b*f), brightness);
}

static void stepPolice() {
  static bool redPhase = true;
  static enum { S_ON, S_OFF } st = S_ON;
  static uint32_t phaseStart = 0;
  uint32_t now = millis();
  uint32_t onMs  = (uint32_t)(120 / speedMul);
  uint32_t offMs = (uint32_t)(80  / speedMul);

  if (st == S_ON) {
    if (redPhase) outRGB(255,0,0, brightness); else outRGB(0,0,255, brightness);
    if (now - phaseStart >= onMs) { st = S_OFF; phaseStart = now; }
  } else {
    outRGB(0,0,0, 0);
    if (now - phaseStart >= offMs) { st = S_ON; phaseStart = now; redPhase = !redPhase; }
  }
}

static inline int16_t irand(int16_t a, int16_t b) { return a + (int16_t)(random(b - a + 1)); }

static void stepCandle() {
  const uint32_t stepMs = (uint32_t)(40 / speedMul);
  if (millis() - lastStep < stepMs) return;
  lastStep += stepMs;

  int base = 200;
  int jitter = irand(-50, 25);
  int v = clampU8(base + jitter);
  uint8_t r = v, g = (uint8_t)(v * 160 / 255), b = (uint8_t)(v * 20 / 255);
  outRGB(r,g,b, brightness);
}

static void stepStrobe() {
  static bool on = true;
  static uint32_t phaseStart = 0;
  uint32_t now = millis();
  uint32_t onMs  = (uint32_t)(30  / speedMul);
  uint32_t offMs = (uint32_t)(220 / speedMul);

  if (on) {
    outRGB(255,255,255, brightness);
    if (now - phaseStart >= onMs) { on = false; phaseStart = now; }
  } else {
    outRGB(0,0,0, 0);
    if (now - phaseStart >= offMs) { on = true; phaseStart = now; }
  }
}

static void stepPulse() {
  const uint32_t stepMs = (uint32_t)(12 / speedMul);
  static int16_t lvl = 0;
  static int8_t dir = +3;
  if (millis() - lastStep < stepMs) return;
  lastStep += stepMs;

  lvl += dir;
  if (lvl >= 255) { lvl = 255; dir = -3; }
  if (lvl <= 0)   { lvl = 0;   dir = +3; }

  uint8_t r = (uint16_t)solidR * lvl / 255;
  uint8_t g = (uint16_t)solidG * lvl / 255;
  uint8_t b = (uint16_t)solidB * lvl / 255;
  outRGB(r,g,b, brightness);
}

static void stepSolid() { outRGB(solidR, solidG, solidB, brightness); }
static void stepOff()   { outRGB(0,0,0, 0); }

// ---------- Command parsing ----------
static void handleLine(String line) {
  String raw = line;
  line.trim(); line.toLowerCase();

  if (line == "help" || line == "?") { showHelp(); return; }
  if (line == "status")              { showStatus(); return; }

  if (line.startsWith("rainbow")) { setPattern(P_RAINBOW); botSay(F("Rainbow engaged.")); return; }
  if (line.startsWith("breathe")) { setPattern(P_BREATHE); botSay(F("Breathe engaged.")); return; }
  if (line.startsWith("police"))  { setPattern(P_POLICE);  botSay(F("Police flash engaged.")); return; }
  if (line.startsWith("candle"))  { setPattern(P_CANDLE);  botSay(F("Candle flicker engaged.")); return; }
  if (line.startsWith("strobe"))  { setPattern(P_STROBE);  botSay(F("Strobe engaged.")); return; }
  if (line == "off")              { setPattern(P_OFF);     botSay(F("LED off.")); return; }

  if (line.startsWith("pulse")) {
    setPattern(P_PULSE);
    String arg = line.substring(5); arg.trim();
    if (arg.length()) { uint8_t r,g,b; if (parseColorName(arg,r,g,b)) { solidR=r; solidG=g; solidB=b; } }
    botSay(F("Pulsing.")); return;
  }
  if (line.startsWith("solid")) {
    setPattern(P_SOLID);
    String arg = line.substring(5); arg.trim();
    if (arg.length()) { uint8_t r,g,b; if (parseColorName(arg,r,g,b)) { solidR=r; solidG=g; solidB=b; } }
    botSay(F("Solid color.")); return;
  }

  if (line.startsWith("color ")) {
    String rest = line.substring(6); rest.trim();
    uint8_t r,g,b;
    if (parseColorName(rest, r,g,b)) {
      solidR=r; solidG=g; solidB=b; botSay(F("Color set (named).")); return;
    } else {
      int rr=0, gg=0, bb=0;
      int scanned = sscanf(raw.c_str(), "%*s %d %d %d", &rr, &gg, &bb);
      if (scanned == 3) {
        solidR = clampU8(rr); solidG = clampU8(gg); solidB = clampU8(bb);
        botSay(F("Color set (RGB).")); return;
      }
      botSay(F("Usage: color <r> <g> <b> OR color <name>")); return;
    }
  }

  if (line.startsWith("brightness ")) {
    int val = brightness;
    int scanned = sscanf(line.c_str(), "%*s %d", &val);
    if (scanned == 1) { brightness = clampU8(val); botSay(F("Brightness updated.")); return; }
    botSay(F("Usage: brightness 0..255")); return;
  }

  if (line.startsWith("speed ")) {
    float s = speedMul;
    int matched = sscanf(line.c_str(), "%*s %f", &s);
    if (matched == 1) {
      if (s < 0.1f) s = 0.1f; if (s > 8.0f) s = 8.0f;
      speedMul = s; botSay(F("Speed updated.")); return;
    }
    botSay(F("Usage: speed <float>, e.g. 0.5, 1, 2")); return;
  }

  botSay(F("Sorry, I didn’t catch that. Try: help"));
}

// ---------- Arduino lifecycle ----------
void setup() {
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // ensure off at start
  analogWrite(LEDR, 255);
  analogWrite(LEDG, 255);
  analogWrite(LEDB, 255);

  Serial.begin(115200);
  while (!Serial) {}
  botSay(F("Hello! Type 'help' for commands."));
  showStatus();

  setPattern(P_RAINBOW);
  lastStep = millis();
}

void loop() {
  // serial input
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\r') continue;
    if (c == '\n') { handleLine(inBuf); inBuf = ""; }
    else if (inBuf.length() < 120) inBuf += c;
  }

  // run pattern
  switch (current) {
    case P_RAINBOW: stepRainbow(); break;
    case P_BREATHE: stepBreathe(); break;
    case P_POLICE:  stepPolice();  break;
    case P_CANDLE:  stepCandle();  break;
    case P_STROBE:  stepStrobe();  break;
    case P_PULSE:   stepPulse();   break;
    case P_SOLID:   stepSolid();   break;
    default:        stepOff();     break;
  }
}
