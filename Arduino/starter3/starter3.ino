#include <HumanReadableApi.h>

static char lineBuffer[64];

void handlePattern(uint8_t argc, const char **argv);
void handleColor(uint8_t argc, const char **argv);
void handleBrightness(uint8_t argc, const char **argv);
void handleSpeed(uint8_t argc, const char **argv);
void handleStatus(uint8_t argc, const char **argv);
void handleHelp(uint8_t argc, const char **argv);

HraCommandDef commands[] = {
    { "rainbow",    handlePattern },
    { "breathe",    handlePattern },
    { "police",     handlePattern },
    { "candle",     handlePattern },
    { "strobe",     handlePattern },
    { "pulse",      handlePattern },
    { "solid",      handlePattern },
    { "off",        handlePattern },

    { "color",      handleColor },
    { "brightness", handleBrightness },
    { "speed",      handleSpeed },
    { "status",     handleStatus },
    { "help",       handleHelp },
};

HumanReadableApi api(Serial,
                     commands,
                     sizeof(commands) / sizeof(commands[0]),
                     lineBuffer,
                     sizeof(lineBuffer));

// Your current state
static const char *g_pattern   = "off";
static uint8_t     g_r         = 0;
static uint8_t     g_g         = 0;
static uint8_t     g_b         = 0;
static uint8_t     g_brightness = 128;
static float       g_speed      = 1.0f;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for USB
    }
    Serial.println(F("[bot] Hello! Type 'help' for commands."));
}

void loop() {
    api.poll();

    // your LED update logic here, using g_pattern / g_r/g_g/g_b etc.
}


// ---- Handlers ----

void handlePattern(uint8_t argc, const char **argv) {
    const char *cmd = argv[0];

    if (strcasecmp(cmd, "off") == 0) {
        g_pattern = "off";
    } else if (strcasecmp(cmd, "rainbow") == 0) {
        g_pattern = "rainbow";
    } else if (strcasecmp(cmd, "breathe") == 0) {
        g_pattern = "breathe";
    } else if (strcasecmp(cmd, "police") == 0) {
        g_pattern = "police";
    } else if (strcasecmp(cmd, "candle") == 0) {
        g_pattern = "candle";
    } else if (strcasecmp(cmd, "strobe") == 0) {
        g_pattern = "strobe";
    } else if (strcasecmp(cmd, "pulse") == 0) {
        if (argc < 2) {
            Serial.println(F("[bot] Usage: pulse <color>"));
            return;
        }
        g_pattern = "pulse";
        // parse color name or rgb from argv[1..]
        // ...
    } else if (strcasecmp(cmd, "solid") == 0) {
        if (argc < 2) {
            Serial.println(F("[bot] Usage: solid <color>"));
            return;
        }
        g_pattern = "solid";
        // parse color
        // ...
    }

    Serial.print(F("[bot] Pattern set to "));
    Serial.println(g_pattern);
}

void handleColor(uint8_t argc, const char **argv) {
    if (argc == 4) {
        g_r = static_cast<uint8_t>(atoi(argv[1]));
        g_g = static_cast<uint8_t>(atoi(argv[2]));
        g_b = static_cast<uint8_t>(atoi(argv[3]));
    } else if (argc == 2) {
        const char *name = argv[1];
        // map names to RGB
        // e.g. if (!strcasecmp(name, "red")) { g_r=255; g_g=0; g_b=0; }
        // ...
    } else {
        Serial.println(F("[bot] Usage: color <r> <g> <b> OR color <name>"));
        return;
    }

    Serial.print(F("[bot] Color set to ("));
    Serial.print(g_r);
    Serial.print(',');
    Serial.print(g_g);
    Serial.print(',');
    Serial.print(g_b);
    Serial.println(')');
}

void handleBrightness(uint8_t argc, const char **argv) {
    if (argc < 2) {
        Serial.println(F("[bot] Usage: brightness <0-255>"));
        return;
    }

    int value = atoi(argv[1]);
    if (value < 0)   value = 0;
    if (value > 255) value = 255;

    g_brightness = static_cast<uint8_t>(value);

    Serial.print(F("[bot] Brightness set to "));
    Serial.println(g_brightness);
}

void handleSpeed(uint8_t argc, const char **argv) {
    if (argc < 2) {
        Serial.println(F("[bot] Usage: speed <float>"));
        return;
    }

    g_speed = atof(argv[1]);
    if (g_speed <= 0.0f) {
        g_speed = 0.1f;
    }

    Serial.print(F("[bot] Speed set to "));
    Serial.println(g_speed, 2);
}

void handleStatus(uint8_t, const char **) {
    Serial.print(F("[bot] pattern="));
    Serial.print(g_pattern);
    Serial.print(F(" brightness="));
    Serial.print(g_brightness);
    Serial.print(F(" speed="));
    Serial.print(g_speed, 2);
    Serial.print(F(" color=("));
    Serial.print(g_r);
    Serial.print(',');
    Serial.print(g_g);
    Serial.print(',');
    Serial.print(g_b);
    Serial.println(')');
}

void handleHelp(uint8_t, const char **) {
    Serial.println(F("[bot] Commands:"));
    Serial.println(F("  rainbow | breathe | police | candle | strobe | pulse <color> | solid <color> | off"));
    Serial.println(F("  color <r 0-255> <g 0-255> <b 0-255>  OR  color <name>"));
    Serial.println(F("  names: red, green, blue, white, purple, pink, yellow, orange, cyan, magenta, amber"));
    Serial.println(F("  brightness <0-255>"));
    Serial.println(F("  speed <float e.g. 0.5, 1, 2>"));
    Serial.println(F("  status | help"));
}
