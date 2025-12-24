# Arduino Lua Bot - Example Commands

This file contains example commands you can try with the Arduino Lua Bot.

## Getting Started

After uploading the sketch and opening Serial Monitor at 115200 baud:

```
help
status
```

## Basic LED Control (Plain English)

```
led red
led green
led blue
led white
led off
```

## Custom RGB Colors

```
led 255 0 0       # Pure red
led 0 255 0       # Pure green
led 0 0 255       # Pure blue
led 255 255 0     # Yellow
led 255 0 255     # Magenta
led 0 255 255     # Cyan
led 128 0 128     # Purple
led 255 165 0     # Orange
```

## Pin Control Commands

```
pin 13 OUTPUT     # Set pin 13 as output
write 13 HIGH     # Turn on pin 13
delay 1000        # Wait 1 second
write 13 LOW      # Turn off pin 13

pin 2 INPUT       # Set pin 2 as input
read 2            # Read pin 2 value

pin 3 INPUT_PULLUP  # Set pin 3 with pullup resistor
read 3
```

## Lua Variable Examples

```
lua x = 10
lua y = 20
lua z = x + y
status            # See all variables

lua a = 5
lua b = 3
lua sum = a + b
lua diff = a - b
lua prod = a * b
lua quot = a / b
status
```

## Lua Arduino Functions

### Digital I/O
```
lua pinMode(13, OUTPUT)
lua digitalWrite(13, 1)
lua delay(1000)
lua digitalWrite(13, 0)

lua pinMode(2, INPUT)
lua digitalRead(2)
```

### Analog I/O
```
lua analogWrite(9, 128)    # Set PWM to 50%
lua analogWrite(9, 255)    # Set PWM to 100%
lua analogWrite(9, 0)      # Set PWM to 0%

lua analogRead(A0)         # Read analog pin A0
lua analogRead(A1)
```

### Delays
```
lua delay(500)             # Wait 500ms
lua delay(2000)            # Wait 2 seconds
```

### Print
```
lua print("Hello World")
lua print("Temperature reading")
```

## Complex Examples

### LED Blink Pattern
```
pin 13 OUTPUT
write 13 HIGH
delay 500
write 13 LOW
delay 500
write 13 HIGH
delay 500
write 13 LOW
```

### PWM Fade (using Lua)
```
lua pinMode(9, OUTPUT)
lua brightness = 0
lua analogWrite(9, brightness)
delay 100

lua brightness = 50
lua analogWrite(9, brightness)
delay 100

lua brightness = 100
lua analogWrite(9, brightness)
delay 100

lua brightness = 150
lua analogWrite(9, brightness)
delay 100

lua brightness = 200
lua analogWrite(9, brightness)
delay 100

lua brightness = 255
lua analogWrite(9, brightness)
```

### RGB LED Cycle
```
led 255 0 0
delay 1000
led 255 128 0
delay 1000
led 255 255 0
delay 1000
led 0 255 0
delay 1000
led 0 255 255
delay 1000
led 0 0 255
delay 1000
led 255 0 255
delay 1000
led off
```

### Sensor Reading with Calculation
```
lua sensor = analogRead(A0)
lua voltage = sensor * 5.0 / 1023.0
lua print("Sensor voltage:")
status
```

### Digital Input with LED Output
```
pin 2 INPUT_PULLUP
pin 13 OUTPUT
read 2
# If button pressed (LOW), turn on LED:
write 13 HIGH
# If button released (HIGH), turn off LED:
write 13 LOW
```

### Complex Calculation
```
lua a = 10
lua b = 20
lua c = 30
lua result = a + b * c / 2
status
```

## Natural Language Commands (Experimental)

Try these natural language phrases:

```
hello
hi
what can you do
turn led red
turn led green  
turn led blue
turn led off
turn led on
```

## Debugging and Inspection

```
status            # Show all variables and memory
help              # Show all commands
help lua          # Show help for specific command
help led
reset             # Clear all Lua variables
```

## Multi-line Mode (Future Feature)

```
run
x = 10
y = 20
digitalWrite(13, 1)
delay(1000)
digitalWrite(13, 0)
end
```

## Tips

1. Commands are case-insensitive: `LED RED` = `led red`
2. Use `#` for comments: `led red # This is a comment`
3. Variables persist between commands
4. Use `reset` to clear all variables
5. Use `status` to see current state
6. Maximum 10 variables can be stored
7. Variable names limited to 15 characters

## Error Handling

If you see errors:
- `ERR: Line too long` - Command too long (max 128 chars)
- `ERR: Unknown command` - Command not recognized, try `help`
- `No more variable slots` - Used all 10 variable slots, use `reset`

## Performance Notes

- Each command executes immediately
- `delay` commands block execution
- Serial buffer is 128 bytes
- Lua buffer is 512 bytes
- Expression evaluation is simple (no parentheses support)

## Advanced Usage

### Combining Commands with Lua

```
# Set up LED pins with plain English
pin 9 OUTPUT
pin 10 OUTPUT
pin 11 OUTPUT

# Control with Lua
lua analogWrite(9, 255)
lua analogWrite(10, 128)
lua analogWrite(11, 64)
```

### Variable-based Control

```
lua ledState = 1
lua digitalWrite(13, ledState)
delay 1000

lua ledState = 0
lua digitalWrite(13, ledState)
delay 1000
```

### Sensor Calibration

```
lua min_val = analogRead(A0)
lua print("Minimum value recorded")
# ... wait and measure ...
lua max_val = analogRead(A0)
lua print("Maximum value recorded")
lua range = max_val - min_val
status
```

## Have Fun!

Experiment with different commands and combinations. The bot is designed to be forgiving and helpful. If something doesn't work, try rephrasing or use `help` to see available commands.
