# Quick Reference Card - AI Water User

Print this page and keep it near your device!

---

## Connection Info

**First Time Setup (AP Mode):**
```
┌─────────────────────────────────────────┐
│ WiFi Network: AI-Water-User             │
│ Password: watermeter123                 │
│ Web Interface: http://192.168.4.1      │
└─────────────────────────────────────────┘
```

**Normal Operation (Station Mode):**
```
┌─────────────────────────────────────────┐
│ Connects to your home WiFi             │
│ Check serial monitor for IP address    │
│ Example: http://192.168.1.227          │
└─────────────────────────────────────────┘
```

## Quick Start

**First Boot:**
1. Power on ESP32 (USB) and electrolysis supply
2. Connect to WiFi network "AI-Water-User" (password: watermeter123)
3. Open browser to http://192.168.4.1
4. Configure settings:
   - Enter your WiFi network name and password
   - Enter your Anthropic API key
   - Click Save Settings
5. Device reboots and connects to your WiFi

**Normal Use:**
1. Check serial monitor for IP address
2. Connect your device to same WiFi network as ESP32
3. Open browser to ESP32's IP address
4. Start chatting and watch the Tensor Flow Meter!

## Default Settings

| Setting | Default |
|---------|---------|
| Voltage | 5V |
| V_threshold | 2V |
| Conductivity | 1.0 S/m |
| Area | 10 cm² |
| Gap | 2 cm |
| Safety Resistor | 10Ω |
| Cost per Second | $0.001 |
| Max Duration | 300 s |
| Max Water | 100 g |

## Common Operations

**Start New Session:**
Click "New Session" button → Confirm

**Open Settings:**
Click ⚙️ button → Make changes → Save Settings

**Change Claude Model:**
Click ⚙️ → "Fetch Available Models" → Click model name → Save Settings

**Check Water Usage:**
Look at Tensor Flow Meter: "💧 TENSOR FLOW METER / Session Water Usage: X.XXX g"

**Update WiFi Credentials:**
Click ⚙️ → Enter new WiFi SSID/Password → Save Settings → **Restart ESP32**

**Troubleshoot:**
Press RESET button on ESP32, reconnect to WiFi

## GPIO Connections

```
ESP32 GPIO 26 → Relay/MOSFET Signal
ESP32 GND     → Relay/MOSFET GND
ESP32 5V/VIN  → Relay/MOSFET VCC (if needed)

Power Supply:
(+) → Relay COM → NO → Safety Resistor → Electrode 1
(-) → Electrode 2
```

## Expected Water Consumption

| Response Type | Cost | Duration | Water |
|---------------|------|----------|-------|
| Simple (1 line) | $0.001 | 1 s | 0.01 g |
| Moderate (paragraph) | $0.005 | 5 s | 0.05 g |
| Long (multiple paragraphs) | $0.020 | 20 s | 0.20 g |

*With default settings: 5V, 10Ω, 1 S/m, 10 cm², 2 cm gap*

## Status Messages

| Message | Meaning |
|---------|---------|
| Ready | System idle, ready for input |
| Sending... | Communicating with API |
| Settings saved | Configuration stored (restart if WiFi changed) |
| Fetching... | Loading available models from API |
| Error: API key not configured | Go to settings, enter API key |
| Error: API request failed | Check internet/API key |
| Error: Failed to fetch models | Check API key and internet connection |
| Max water per session reached | Start new session |

## Emergency Procedures

**If something goes wrong:**

1. **Unplug USB power** (stops ESP32)
2. **Disconnect electrolysis power** (stops reaction)
3. **Remove electrodes from water** (if sparking/overheating)

**For overheating resistor:**
- Turn off immediately
- Allow to cool
- Check resistor value and wattage
- Increase wattage rating or resistance value

**For excessive bubbling:**
- Reduce voltage
- Increase safety resistor
- Decrease baking soda concentration

## Safety Reminders ⚠️

- ✅ Use only 5-12V DC (NEVER AC mains)
- ✅ Always use safety resistor (10-100Ω, 2W+)
- ✅ Ventilate well (H₂ + O₂ = explosive)
- ✅ Keep electronics away from water
- ✅ Adult supervision required
- ✅ No flames or sparks near electrolysis

## Maintenance Checklist

**Weekly:**
- [ ] Check water level, refill if needed
- [ ] Inspect wire connections
- [ ] Verify all components secure

**Monthly:**
- [ ] Clean electrodes (sand if corroded)
- [ ] Replace electrolyte solution
- [ ] Check safety resistor for damage
- [ ] Test relay operation

## Formula Reference

```
Current (I):
I = (V - V_th) × σ × A / (R_safety × σ × A + 100 × d)

Water consumed (m):
m = I × t × 9.328×10⁻⁵ grams

Where:
V = voltage (V)
V_th = threshold voltage (V)
σ = conductivity (S/m)
A = electrode area (cm²)
d = electrode gap (cm)
R_safety = safety resistor (Ω)
t = time (seconds)
```

## Useful Numbers

**Faraday's Constant:** 96485 C/mol
**Water Molar Mass:** 18 g/mol
**Electrons per H₂O:** 2 (so 4 per 2H₂O → 2H₂ + O₂)

**Current Limits:**
- 5V with 10Ω: Max 0.5A (short circuit)
- 9V with 18Ω: Max 0.5A (short circuit)
- 12V with 22Ω: Max 0.55A (short circuit)

## API Key Management

**Get API Key:**
https://console.anthropic.com → API Keys → Create Key

**Pricing (Claude 3.5 Sonnet):**
- Input: $3 per 1M tokens
- Output: $15 per 1M tokens
- Output is 5× more expensive (more computation)

**Security:**
- Treat like a password
- Don't share
- Set usage limits in Anthropic console
- Regenerate if compromised

## Serial Monitor Debugging

**Baud Rate:** 115200

**Common Messages:**
```
AI Water User Starting...
SPIFFS Mount Success
Settings loaded from NVS
AP IP address: 192.168.4.1
HTTP server started
Electrolysis started for X seconds
Electrolysis stopped
Max water per session reached
```

## Typical Current Draw

| Component | Current |
|-----------|---------|
| ESP32 | 80-160 mA |
| Relay module | 15-30 mA |
| Electrolysis (5V, 10Ω) | 100-300 mA |
| **Total** | **200-500 mA** |

Use 1A+ power supplies for margin.

## Web Interface Layout

```
┌─────────────────────────────────────────────────┐
│ Claude  [New Session]      💧 0.123 g      ⚙️  │ Header
├─────────────────────────────────────────────────┤
│                                                 │
│  [Chat messages appear here]                   │
│                                                 │
│  User: "Hello"                                 │
│  Assistant: "Hi! How can I help?"             │
│              Cost: $0.001 | Tokens: 10 in, 20 out │
│                                                 │ Chat
├─────────────────────────────────────────────────┤
│ [Type message here...]                    [→]  │ Input
│ Status: Ready                                   │
└─────────────────────────────────────────────────┘
```

## Electrolyte Recipe

**Basic Solution:**
- 1 liter distilled/filtered water
- 1-2 tablespoons baking soda (sodium bicarbonate)
- Stir until dissolved

**Adjust Conductivity:**
- More baking soda = higher σ = more current
- Less baking soda = lower σ = less current
- Start with 1 tbsp/L, adjust based on performance

## Troubleshooting Quick Guide

| Problem | Quick Fix |
|---------|-----------|
| No WiFi network | Reset ESP32, wait 15 seconds |
| Can't load webpage | Check connected to correct WiFi |
| No API response | Verify API key starts with "sk-ant-" |
| No bubbles | Add more baking soda, check wiring |
| Weak bubbles | Increase voltage or decrease resistor |
| Too much heat | Increase resistor, improve ventilation |
| Relay doesn't click | Check GPIO 26 connection |
| WiFi settings don't apply | Restart ESP32 after saving settings |
| Long responses cut off | Fixed in latest version (buffer increased) |
| Can't fetch models | Check API key and internet connection |
| Settings save error | Expected for WiFi changes - restart ESP32 |

## Support Resources

**Documentation:**
- README.md - Overview and theory
- INSTALL.md - Software setup
- CONSTRUCTION.md - Hardware building
- USAGE.md - Operating instructions

**Online:**
- Anthropic Console: https://console.anthropic.com
- ESP32 Docs: https://docs.espressif.com/
- Arduino Reference: https://www.arduino.cc/reference/

---

**Remember:** AI Water User makes AI resource consumption visible and tangible. The Tensor Flow Meter shows the real cost of every AI interaction in grams of destroyed water. Use thoughtfully! 💧🤖

---

*Version 1.0 | Cut along the line above to create a reference card*
