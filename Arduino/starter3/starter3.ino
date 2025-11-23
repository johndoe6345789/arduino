#include <HumanReadableApi.h>

static char lineBuffer[64];

// Built-in RGB LED on Arduino Nano R4
static const int LED_R_PIN = LEDR;
static const int LED_G_PIN = LEDG;
static const int LED_B_PIN = LEDB;

// ----------------------
// Forward declarations
// ----------------------

void handlePattern(uint8_t argc, const char **argv);
void handleColor(uint8_t argc, const char **argv);
void handleBrightness(uint8_t argc, const char **argv);
void handleSpeed(uint8_t argc, const char **argv);
void handleStatus(uint8_t argc, const char **argv);
void handleHelp(uint8_t argc, const char **argv);

// Helper forward declarations
static void setPattern(const char *pattern);
static bool parseColorName(const char *name,
                           uint8_t &r, uint8_t &g, uint8_t &b);
static bool parseColorArgs(uint8_t argc, const char **argv,
                           uint8_t &r, uint8_t &g, uint8_t &b);
static void applyColor(uint8_t r, uint8_t g, uint8_t b);
static void printStatus();
static void updateLedFromState();

// ----------------------
// Color registry
// ----------------------

struct ColorMeta {
    const char *name;
    uint8_t     r;
    uint8_t     g;
    uint8_t     b;
};

static const ColorMeta COLOR_TABLE[] = {
    { "red",     255, 0,   0   },
    { "green",   0,   255, 0   },
    { "blue",    0,   0,   255 },
    { "white",   255, 255, 255 },
    { "purple",  128, 0,   128 },
    { "pink",    255, 105, 180 },
    { "yellow",  255, 255, 0   },
    { "orange",  255, 165, 0   },
    { "cyan",    0,   255, 255 },
    { "magenta", 255, 0,   255 },
    { "amber",   255, 191, 0   },
};

// ----------------------
// Unified command registry
// ----------------------

struct CommandMeta {
    const char       *name;
    HraCommandHandler handler;
    const char       *usage;
    const char       *description;
};

// Single source of truth for commands
static const CommandMeta COMMANDS_META[] = {
    {
        "rainbow",
        handlePattern,
        "rainbow",
        "Animated rainbow pattern"
    },
    {
        "breathe",
        handlePattern,
        "breathe",
        "Smooth breathing effect using current color"
    },
    {
        "police",
        handlePattern,
        "police",
        "Red/blue flashing pattern"
    },
    {
        "candle",
        handlePattern,
        "candle",
        "Warm flickering candle effect"
    },
    {
        "strobe",
        handlePattern,
        "strobe",
        "Fast strobe using current color"
    },
    {
        "pulse",
        handlePattern,
        "pulse <color>",
        "Pulse pattern with given color (name or r g b)"
    },
    {
        "solid",
        handlePattern,
        "solid <color>",
        "Solid color (name or r g b)"
    },
    {
        "off",
        handlePattern,
        "off",
        "Turn LEDs off"
    },
    {
        "color",
        handleColor,
        "color <r 0-255> <g 0-255> <b 0-255> | color <name>",
        "Set color by RGB or name"
    },
    {
        "brightness",
        handleBrightness,
        "brightness <0-255>",
        "Set brightness (0 = dark, 255 = max)"
    },
    {
        "speed",
        handleSpeed,
        "speed <float>",
        "Set pattern speed (e.g. 0.5, 1, 2)"
    },
    {
        "status",
        handleStatus,
        "status",
        "Show current pattern, color, brightness, speed"
    },
    {
        "help",
        handleHelp,
        "help",
        "Show this help"
    },
};

// Array used by HumanReadableApi, filled from COMMANDS_META
static HraCommandDef commands[sizeof(COMMANDS_META) /
                              sizeof(COMMANDS_META[0])];

static void initCommandsFromMeta() {
    const size_t count = sizeof(COMMANDS_META) / sizeof(COMMANDS_META[0]);
    for (size_t i = 0; i < count; ++i) {
        commands[i].name    = COMMANDS_META[i].name;
        commands[i].handler = COMMANDS_META[i].handler;
    }
}

// ----------------------
// HumanReadableApi instance
// ----------------------

HumanReadableApi api(
    Serial,
    commands,
    sizeof(commands) / sizeof(commands[0]),
    lineBuffer,
    sizeof(lineBuffer)
);

// ----------------------
// Current state
// ----------------------

// Sensible defaults: solid white, medium brightness, normal speed
static const char *g_pattern    = "solid";
static uint8_t     g_r          = 255;
static uint8_t     g_g          = 255;
static uint8_t     g_b          = 255;
static uint8_t     g_brightness = 128;
static float       g_speed      = 1.0f;

// ----------------------
// Arduino lifecycle
// ----------------------

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for USB on boards that need it
    }

    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT);

    // Initialize the dispatch table from the unified meta table
    initCommandsFromMeta();

    Serial.println(F("[bot] Hello! Type 'help' for commands."));
    printStatus();
    updateLedFromState();
}

void loop() {
    api.poll();
    updateLedFromState();
}

// ----------------------
// Command handlers
// ----------------------

void handlePattern(uint8_t argc, const char **argv) {
    if (argc < 1) {
        Serial.println(
            F("[bot] Usage: rainbow|breathe|police|candle|strobe|"
              "pulse [color]|solid [color]|off")
        );
        return;
    }

    const char *cmd = argv[0];

    if (strcasecmp(cmd, "rainbow") == 0) {
        setPattern("rainbow");
        printStatus();
        return;
    }

    if (strcasecmp(cmd, "breathe") == 0) {
        setPattern("breathe");
        printStatus();
        return;
    }

    if (strcasecmp(cmd, "police") == 0) {
        setPattern("police");
        printStatus();
        return;
    }

    if (strcasecmp(cmd, "candle") == 0) {
        setPattern("candle");
        printStatus();
        return;
    }

    if (strcasecmp(cmd, "strobe") == 0) {
        setPattern("strobe");
        printStatus();
        return;
    }

    if (strcasecmp(cmd, "pulse") == 0) {
        setPattern("pulse");

        // Optional: pulse <color>
        if (argc > 1) {
            uint8_t r, g, b;
            if (!parseColorArgs(argc - 1, &argv[1], r, g, b)) {
                Serial.println(
                    F("[bot] Usage: pulse <r g b> OR pulse <name>")
                );
                return;
            }
            applyColor(r, g, b);
        }

        printStatus();
        return;
    }

    if (strcasecmp(cmd, "solid") == 0) {
        setPattern("solid");

        // Optional: solid <color>
        if (argc > 1) {
            uint8_t r, g, b;
            if (!parseColorArgs(argc - 1, &argv[1], r, g, b)) {
                Serial.println(
                    F("[bot] Usage: solid <r g b> OR solid <name>")
                );
                return;
            }
            applyColor(r, g, b);
        }

        printStatus();
        return;
    }

    if (strcasecmp(cmd, "off") == 0) {
        setPattern("off");
        applyColor(0, 0, 0);
        printStatus();
        return;
    }

    Serial.println(F("[bot] Unknown pattern"));
}

void handleColor(uint8_t argc, const char **argv) {
    if (argc < 2) {
        Serial.println(
            F("[bot] Usage: color <r> <g> <b> OR color <name>")
        );
        return;
    }

    uint8_t r, g, b;
    if (!parseColorArgs(argc - 1, &argv[1], r, g, b)) {
        Serial.println(
            F("[bot] Usage: color <r> <g> <b> OR color <name>")
        );
        return;
    }

    applyColor(r, g, b);

    // Sensible behaviour: if we were off, go to solid
    if (strcasecmp(g_pattern, "off") == 0) {
        setPattern("solid");
    }

    printStatus();
}

void handleBrightness(uint8_t argc, const char **argv) {
    if (argc < 2) {
        Serial.println(F("[bot] Usage: brightness <0-255>"));
        return;
    }

    int val = atoi(argv[1]);
    if (val < 0 || val > 255) {
        Serial.println(F("[bot] Brightness must be 0-255"));
        return;
    }

    g_brightness = static_cast<uint8_t>(val);

    Serial.print(F("[bot] brightness="));
    Serial.println(g_brightness);

    printStatus();
}

void handleSpeed(uint8_t argc, const char **argv) {
    if (argc < 2) {
        Serial.println(F("[bot] Usage: speed <float e.g. 0.5, 1, 2>"));
        return;
    }

    float val = atof(argv[1]);
    if (val <= 0.0f) {
        Serial.println(F("[bot] Speed must be > 0"));
        return;
    }

    g_speed = val;

    Serial.print(F("[bot] speed="));
    Serial.println(g_speed, 2);

    printStatus();
}

void handleStatus(uint8_t, const char **) {
    printStatus();
}

void handleHelp(uint8_t, const char **) {
    Serial.println(F("[bot] Commands:"));

    const size_t cmdCount = sizeof(COMMANDS_META) / sizeof(COMMANDS_META[0]);
    const size_t colorCount = sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0]);

    for (size_t i = 0; i < cmdCount; ++i) {
        const CommandMeta &m = COMMANDS_META[i];

        Serial.print(F("  "));
        Serial.print(m.usage);
        if (m.description && m.description[0] != '\0') {
            Serial.print(F("  - "));
            Serial.print(m.description);
        }
        Serial.println();

        // For the color command, also print the available names
        if (strcmp(m.name, "color") == 0) {
            Serial.print(F("    names: "));
            for (size_t j = 0; j < colorCount; ++j) {
                Serial.print(COLOR_TABLE[j].name);
                if (j + 1 < colorCount) {
                    Serial.print(F(", "));
                }
            }
            Serial.println();
        }
    }
}

// ----------------------
// Helpers
// ----------------------

static void setPattern(const char *pattern) {
    g_pattern = pattern;

    Serial.print(F("[bot] pattern="));
    Serial.println(g_pattern);
}

static bool parseColorName(const char *name,
                           uint8_t &r, uint8_t &g, uint8_t &b) {
    const size_t count = sizeof(COLOR_TABLE) / sizeof(COLOR_TABLE[0]);
    for (size_t i = 0; i < count; ++i) {
        if (strcasecmp(name, COLOR_TABLE[i].name) == 0) {
            r = COLOR_TABLE[i].r;
            g = COLOR_TABLE[i].g;
            b = COLOR_TABLE[i].b;
            return true;
        }
    }
    return false;
}

static bool parseColorArgs(uint8_t argc, const char **argv,
                           uint8_t &r, uint8_t &g, uint8_t &b) {
    // argc/argv here do NOT include the command word.
    if (argc == 1) {
        // Single name
        return parseColorName(argv[0], r, g, b);
    }

    if (argc == 3) {
        int ri = atoi(argv[0]);
        int gi = atoi(argv[1]);
        int bi = atoi(argv[2]);

        if (ri < 0 || ri > 255 ||
            gi < 0 || gi > 255 ||
            bi < 0 || bi > 255) {
            return false;
        }

        r = static_cast<uint8_t>(ri);
        g = static_cast<uint8_t>(gi);
        b = static_cast<uint8_t>(bi);
        return true;
    }

    return false;
}

static void applyColor(uint8_t r, uint8_t g, uint8_t b) {
    g_r = r;
    g_g = g;
    g_b = b;

    Serial.print(F("[bot] color=("));
    Serial.print(g_r);
    Serial.print(',');
    Serial.print(g_g);
    Serial.print(',');
    Serial.print(g_b);
    Serial.println(')');
}

static void printStatus() {
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

static void updateLedFromState() {
    uint8_t br = g_brightness;

    uint8_t r = static_cast<uint8_t>(
        (static_cast<uint16_t>(g_r) * br) / 255
    );
    uint8_t g = static_cast<uint8_t>(
        (static_cast<uint16_t>(g_g) * br) / 255
    );
    uint8_t b = static_cast<uint8_t>(
        (static_cast<uint16_t>(g_b) * br) / 255
    );

    // Nano R4 onboard RGB is common-anode: 0 = ON, 255 = OFF
    analogWrite(LED_R_PIN, 255 - r);
    analogWrite(LED_G_PIN, 255 - g);
    analogWrite(LED_B_PIN, 255 - b);
}
