#include "HumanReadableApi.h"

// Implementation of HumanReadableApi

HumanReadableApi::HumanReadableApi(
    Stream &io,
    HraCommandDef *commands,
    uint8_t commandCount,
    char *lineBuffer,
    uint8_t lineBufferSize
) : _io(io),
    _commands(commands),
    _commandCount(commandCount),
    _buffer(lineBuffer),
    _bufferSize(lineBufferSize),
    _len(0) {}

void HumanReadableApi::poll() {
    while (_io.available()) {
        char c = _io.read();
        if (c == '\n' || c == '\r') {
            if (_len > 0) {
                _buffer[_len] = '\0';
                handleLine(_buffer);
                _len = 0;
            }
        } else {
            if (_len < (_bufferSize - 1)) {
                _buffer[_len++] = c;
            }
        }
    }
}

void HumanReadableApi::handleLine(char *line) {
    const char *argv[10];
    uint8_t argc = 0;

    char *p = line;
    while (*p && argc < 10) {
        while (*p == ' ') p++;
        if (!*p) break;
        argv[argc++] = p;
        while (*p && *p != ' ') p++;
        if (*p) *p++ = '\0';
    }

    dispatchTokens(argc, argv);
}

void HumanReadableApi::dispatchTokens(uint8_t argc, const char **argv) {
    if (argc == 0) return;

    for (uint8_t i = 0; i < _commandCount; i++) {
        if (strcmp(argv[0], _commands[i].name) == 0) {
            _commands[i].handler(argc, argv);
            return;
        }
    }

    _io.println("ERR: Unknown command");
}

void HumanReadableApi::parseKeyValuePairs(char *line, KeyValueHandler cb) {
    char *p = line;
    while (*p) {
        while (*p == ' ') p++;
        if (!*p) break;

        char *key = p;
        while (*p && *p != '=' && *p != ' ') p++;
        if (*p != '=') {
            while (*p && *p != ' ') p++;
            continue;
        }

        *p++ = '\0';
        char *value = p;
        while (*p && *p != ' ') p++;
        if (*p) *p++ = '\0';

        cb(key, value);
    }
}
