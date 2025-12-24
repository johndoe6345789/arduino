# Quick Start Guide - LED Effects & Games

## 🚀 5-Minute Setup

### Step 1: Upload (2 minutes)
1. Open Arduino IDE
2. Open `lua_bot.ino`
3. Select your board: Tools → Board → Arduino UNO R4 WiFi (or your board)
4. Select your port: Tools → Port → COMx
5. Click Upload button (→)
6. Wait for "Done uploading"

### Step 2: Open Serial Monitor (30 seconds)
1. Click Serial Monitor button (🔍) or press Ctrl+Shift+M
2. Set baud rate to **115200** (bottom right)
3. Set line ending to "Newline" (bottom right)
4. You should see the welcome message!

### Step 3: Try Your First Effect (1 minute)
Copy and paste these lines into Serial Monitor:

```
led red
delay 500
led green
delay 500
led blue
delay 500
led off
```

Press Enter and watch your LED change colors! 🎨

## 🎨 Try More Effects (30 seconds each)

### Rainbow Cycle
```
led 255 0 0
delay 200
led 255 255 0
delay 200
led 0 255 0
delay 200
led 0 255 255
delay 200
led 0 0 255
delay 200
led 255 0 255
delay 200
led off
```

### Breathing Red
```
lua setRGB(50, 0, 0)
delay 100
lua setRGB(100, 0, 0)
delay 100
lua setRGB(200, 0, 0)
delay 100
lua setRGB(255, 0, 0)
delay 200
lua setRGB(200, 0, 0)
delay 100
lua setRGB(100, 0, 0)
delay 100
lua setRGB(50, 0, 0)
delay 100
lua setRGB(0, 0, 0)
```

### Police Lights
```
lua setRGB(255, 0, 0)
delay 200
lua setRGB(0, 0, 0)
delay 200
lua setRGB(0, 0, 255)
delay 200
lua setRGB(0, 0, 0)
delay 200
```

## 🎮 Try Your First Game (2 minutes)

### Reaction Time Challenge
```
lua print("Watch for green light...")
lua setRGB(255, 0, 0)
delay 2000
lua setRGB(0, 255, 0)
lua start = millis()
```
**Now press Enter as fast as you can!**

Then check your reaction time:
```
lua end = millis()
lua time = end - start
status
```

Look for "time" in the status output - that's your reaction time in milliseconds!
- Under 300ms = Excellent! ⚡
- 300-600ms = Good! 👍
- Over 600ms = Keep practicing! 💪

## 📚 What's Next?

### More LED Effects
Open `examples_led_effects.txt` for 12 amazing effects:
- Fire effect 🔥
- Ocean waves 🌊
- Lightning ⚡
- Sunset 🌅
- And more!

### More Games
Open `examples_games.txt` for 8 fun games:
- Number guessing game 🎯
- Simon Says memory game 🧠
- RGB color mixer 🎨
- Timing challenges ⏱️
- And more!

### Full Documentation
- **EXAMPLES_README.md** - Complete guide with tips and troubleshooting
- **README.md** - Full documentation of all features
- **EXAMPLES.md** - More example commands

## 💡 Pro Tips

1. **Copy Multiple Lines**: You can copy and paste multiple commands at once
2. **Experiment**: Change the RGB values (0-255) to create your own colors
3. **Speed Control**: Change the `delay` values to make effects faster or slower
4. **Save Favorites**: Copy your favorite effects to a text file
5. **Reset Anytime**: Type `reset` to clear variables and start fresh

## 🛠️ Essential Commands

```
help              # Show all commands
status            # Show all variables
reset             # Clear all variables
led off           # Turn LED off
lua setRGB(r,g,b) # Set LED color (0-255 for each)
delay 1000        # Wait 1 second
```

## ❓ Quick Troubleshooting

**LED not working?**
- Try: `led red` then `led off`
- Check your board has RGB LED (R4 boards do)

**Commands not responding?**
- Check baud rate is 115200
- Check line ending is "Newline"
- Try closing and reopening Serial Monitor

**Want to start over?**
- Type: `reset`
- Or re-upload the sketch

## 🎉 Have Fun!

You're all set! Start experimenting with effects and games. Don't be afraid to try different values and create your own combinations!

---
**Need help?** Check EXAMPLES_README.md for detailed instructions and troubleshooting.
