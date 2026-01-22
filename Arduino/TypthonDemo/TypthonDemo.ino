// Typthon demo sketch that uses the TypthonMini Arduino library with a typed Python-esque runtime.
//
// IMPORTANT: This sketch requires a board with C++ Standard Library support.
// Compatible boards: ESP32, ESP8266, Arduino Due, Arduino Zero, Arduino UNO R4, Raspberry Pi Pico (RP2040), STM32
// NOT compatible with: Arduino Uno (classic), Nano, Mega (AVR architecture lacks STL support)
//
#include <Arduino.h>
#include <TypthonMini.h>

using typthon::Interpreter;

const char *demoScript = R"PY(
# Typthon: Strictly-typed Python
# All variables and functions MUST have type annotations

# Typed variable declarations
x: int = 42
name: str = "Typthon"
flag: bool = True

print("Integer:", x)
print("String:", name)
print("Boolean:", flag)

# Typed function with parameter and return type annotations
def add_numbers(a: int, b: int) -> int:
    result: int = a + b
    return result

sum_val: int = add_numbers(10, 20)
print("Sum:", sum_val)

# Another typed function
def greet(name: str) -> str:
    greeting: str = "Hello, "
    message: str = greeting + name
    return message

msg: str = greet("Arduino")
print(msg)

# Type checking examples
def multiply(x: int, y: int) -> int:
    product: int = x * y
    return product

result: int = multiply(7, 6)
print("Product:", result)

# Lists with homogeneous types
numbers: list = [1, 2, 3]
print("Numbers:", numbers)

# NOTE: Advanced features like list[int], dict[str,int], and classes
# will be demonstrated once generic type parsing is complete
)PY";

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ;
    }
    Serial.println(F("Starting Typthon typed Python-like demo script..."));
    Interpreter interpreter(demoScript);
    interpreter.run();
    Serial.println(F("Script finished."));
}

void loop() {
    // Intentionally empty to keep the demo one-shot.
}
