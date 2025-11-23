#pragma once

#include <Arduino.h>

typedef void (*HraCommandHandler)(uint8_t argc, const char **argv);

struct HraCommandDef {
    const char        *name;
    HraCommandHandler  handler;
};

class HumanReadableApi {
public:
    HumanReadableApi(
        Stream        &io,
        HraCommandDef *commands,
        uint8_t        commandCount,
        char          *lineBuffer,
        uint8_t        lineBufferSize
    );

    // Call this regularly from loop(); it reads characters and fires handlers
    void poll();

    // Optional: directly handle a full, null-terminated line
    void handleLine(char *line);

    // Helper: parse "key=value" pairs in a line and call a callback
    typedef void (*KeyValueHandler)(const char *key, const char *value);
    static void parseKeyValuePairs(char *line, KeyValueHandler cb);

private:
    Stream        &_io;
    HraCommandDef *_commands;
    uint8_t        _commandCount;

    char          *_buffer;
    uint8_t        _bufferSize;
    uint8_t        _len;

    void          dispatchTokens(uint8_t argc, const char **argv);
};
