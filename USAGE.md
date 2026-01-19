# Usage Guide - AI Water User

This guide explains how to use the AI Water User system for daily operation.

## Prerequisites

Before using the system:
- [ ] Software is installed (see INSTALL.md)
- [ ] Hardware is constructed and tested (see CONSTRUCTION.md)
- [ ] You have an Anthropic API key
- [ ] System is powered on and creating WiFi network

## Quick Start

### 1. Power On

1. Connect ESP32 to USB power (5V, 1A minimum)
2. Connect electrolysis power supply
3. Wait 10-15 seconds for ESP32 to boot
4. Look for relay click or LED indicator

### 2. Connect to the Device

**On your phone, laptop, or tablet:**

1. Open WiFi settings
2. Look for network: **AI-Water-User**
3. Connect using password: **watermeter123**
4. Wait for connection to establish
5. If it asks "Sign in to network" or "Log in required" - dismiss or cancel
6. Open web browser
7. Navigate to: **http://192.168.4.1**

**Expected result:** You should see the AI Water User interface with:
- "AI Water User" logo with "powered by Claude" subtitle
- Tensor Flow Meter showing water usage (0.000 g)
- Message input box
- Settings button (⚙️)

### 3. Configure Settings (First Time Only)

**First boot - Access Point mode:**
1. Device creates WiFi network "AI-Water-User" (password: watermeter123)
2. Connect to "AI-Water-User" from your device
3. Open browser to http://192.168.4.1
4. Click the **Settings button (⚙️)**
5. Configure:
   - **WiFi Network Name (SSID)** - Your home WiFi
   - **WiFi Password** - Your home WiFi password
   - **Anthropic API Key** - Get from [console.anthropic.com](https://console.anthropic.com)
6. Click **Save Settings**
7. Device will reboot and connect to your WiFi

**After first boot - Station mode:**
1. Device connects to your home WiFi automatically
2. Check serial monitor for IP address (e.g., "Access web interface at: http://192.168.1.227")
3. Connect your device to the same WiFi network
4. Open browser to that IP address
5. Start chatting!

**Note:** If WiFi connection fails, device automatically falls back to Access Point mode for reconfiguration.

### 4. Start Chatting

1. Type a message in the input box
2. Press **Enter** or click the **send button (→)**
3. Wait for Claude to respond
4. Watch the water usage increase in real-time!

**What happens:**
- Your message is sent to Anthropic's API
- Claude generates a response
- The system calculates the API cost
- Electrolysis runs proportionally to that cost
- Water usage updates every 500ms while active

## Understanding the Interface

### Main Screen

```
┌─────────────────────────────────────────────────────────────┐
│ AI Water User   [New Session]   💧 TENSOR FLOW METER        │
│ powered by Claude                   Session Water Usage ⚙️   │
│                                     [0.123 g]                │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  [Chat messages appear here]                                 │
│                                                               │
│                                                               │
│                                                               │
├─────────────────────────────────────────────────────────────┤
│ [Type message here...]                                   [→] │
│ Status: Ready                                                │
└─────────────────────────────────────────────────────────────┘
```

### Header Elements

**AI Water User Logo** - Main branding with "powered by Claude" subtitle

**New Session Button** - Clears conversation and resets water counter

**Tensor Flow Meter** - Displays real-time water consumption tracking
- Shows "TENSOR FLOW METER" as main label
- "Session Water Usage" as sublabel
- Water amount in grams (updates every 500ms)
- Glows/pulses when electrolysis is active

**Settings Button (⚙️)** - Opens configuration panel

### Chat Area

**Welcome Message** - Initial screen before first message

**User Messages** - Your messages appear with light background

**Assistant Messages** - Claude's responses with metadata showing:
- API cost in dollars
- Input token count
- Output token count

**Error Messages** - Red text if something goes wrong

### Input Area

**Message Box** - Type your prompts here
- Auto-resizes as you type
- Shift+Enter for new line
- Enter to send

**Send Button (→)** - Click to send message

**Status Text** - Shows current system state:
- "Ready" - System idle, ready for input
- "Sending..." - Message being sent to API
- "Error: ..." - Problem occurred
- Other messages based on activity

## Settings Panel

### Opening Settings

Click the **⚙️ button** to open the settings panel. It slides in from the right.

### API Configuration

**Anthropic API Key**
- Required for system to work
- Type or paste your key (starts with `sk-ant-`)
- Stored securely on the ESP32
- Never sent anywhere except Anthropic's API
- Shows as dots (password field) for security

**Getting an API Key:**
1. Go to [console.anthropic.com](https://console.anthropic.com)
2. Create account or sign in
3. Go to API Keys section
4. Click "Create Key"
5. Copy the key immediately (can't view again)
6. Paste into settings

**API Key Security:**
- Treat like a password - don't share
- Generates charges on your Anthropic account
- Can set usage limits in Anthropic console
- Regenerate if compromised

**Claude Model**
- Specifies which Claude model to use
- Default: claude-3-haiku-20240307 (fastest and cheapest)
- Click "Fetch Available Models" to see what's available with your API key
- Models appear as clickable links - click to select
- Common models:
  - claude-3-haiku-20240307 (fastest, cheapest)
  - claude-3-5-sonnet-20241022 (balanced, recommended)
  - claude-3-opus-20240229 (most capable, expensive)

**Fetching Available Models:**
1. Click "Fetch Available Models" button
2. Wait 2-3 seconds while system queries API
3. Available models appear as clickable list
4. Click desired model name to select it
5. Model name is automatically filled in
6. Click "Save Settings" to apply

**Note:** The model list shows only models available to your API key. If you don't see a particular model, check your Anthropic account tier.

### Electrolysis Parameters

**Applied Voltage (V)**
- Voltage of your power supply
- Typical: 5V, 9V, or 12V
- Should match your actual supply
- Higher voltage = more current = more water consumption

**Threshold Voltage (V_th)**
- Minimum voltage needed for electrolysis
- Default: 2.0V (standard for water)
- Usually don't need to change
- If bubbles at low voltage, decrease slightly

**Conductivity (σ)**
- Measured in S/m (Siemens per meter)
- Depends on baking soda concentration
- Default: 1.0 for moderate solution
- Higher number = more conductive = more current
- Adjust based on calibration measurements

**Electrode Area (A)**
- Surface area of ONE electrode in cm²
- Measure width × height of submerged portion
- Example: 1cm wide × 5cm tall = 5 cm²
- Larger area = more current = more water consumption

**Electrode Gap (d)**
- Distance between electrodes in cm
- Measure center-to-center or face-to-face
- Typical: 2-3 cm
- Smaller gap = less resistance = more current

**Safety Resistor (R)**
- Value in ohms (Ω)
- Should match your physical resistor
- Default: 10Ω
- Never set to 0 if you have a resistor!
- Limits short-circuit current

### Cost-to-Time Scaling

**Cost per Second ($/s)**
- How many dollars of API cost = 1 second of electrolysis
- Default: 0.001 (one-tenth of a cent per second)
- Lower value = less water used per response
- Higher value = more water used per response

**Example calculations:**
- At 0.001 $/s: A $0.005 response runs for 5 seconds
- At 0.0001 $/s: A $0.005 response runs for 50 seconds
- At 0.01 $/s: A $0.005 response runs for 0.5 seconds

**Choosing a value:**
- Start with default (0.001)
- If water consumption too low: increase value
- If water consumption too high: decrease value
- Consider your water supply and cleanup

### Safety Limits

**Max Single Duration (s)**
- Maximum time for one electrolysis event
- Default: 300 seconds (5 minutes)
- Prevents runaway operation
- Limits heat buildup

**Max Water Per Session (g)**
- Maximum total water consumption per session
- Default: 100 grams
- Prevents depleting entire container
- Session resets with "New Session" button

**How limits work:**
- System checks before each activation
- If limit would be exceeded, electrolysis is skipped
- Warning shown in Serial Monitor
- Can continue chatting (just no more electrolysis)

### Saving Settings

1. Make your changes
2. Click **Save Settings** button at bottom
3. Wait for "Settings saved" confirmation
4. Close panel with **×** button

**Important: WiFi Credential Changes**
- If you changed WiFi SSID or password, you must **manually restart the ESP32**
- Simply press the RESET button or power cycle the device
- The new WiFi settings will apply on next boot
- This prevents network interruptions during the save operation

**Settings are persistent** - they survive:
- Page refreshes
- Browser closure
- Device reboots
- Power cycles

Settings are stored in ESP32's non-volatile storage (NVS).

## Using the System

### Starting a Conversation

1. Type your message in the input box
2. Examples:
   - "What is the capital of France?"
   - "Explain quantum computing in simple terms"
   - "Write a haiku about water"
3. Press Enter or click Send
4. Wait for response (usually 2-10 seconds)

### During a Response

Watch the interface:
- Status changes to "Sending..."
- Claude's response appears
- Water usage starts increasing
- You may hear relay click
- Electrolysis bubbles increase
- Water usage updates in real-time

### After Each Response

The interface shows:
- Claude's complete response
- Metadata: cost, input tokens, output tokens
- Updated total water usage

**Typical costs:**
- Simple question: $0.001 - 0.003
- Moderate response: $0.003 - 0.010
- Long response: $0.010 - 0.050

**Typical water consumption** (with default settings):
- Simple question: 0.01 - 0.03 grams
- Moderate response: 0.03 - 0.10 grams
- Long response: 0.10 - 0.50 grams

### Continuing the Conversation

Just keep typing! The system maintains context:
- Previous messages are remembered
- Claude can reference earlier conversation
- Water usage accumulates
- Conversation history stored in ESP32 memory

**Memory limits:**
- ESP32 has limited RAM
- Very long conversations may fail
- Start new session if experiencing errors
- Typical limit: 20-50 exchanges depending on length

### Starting a New Session

Click **New Session** button when:
- Want to start fresh conversation
- Water usage approaching limit
- System seems slow or unstable
- Want to reset counter

**What happens:**
- Conversation history cleared
- Water usage counter resets to 0
- Any active electrolysis stops
- Ready for new conversation

Confirmation dialog asks if you're sure.

## Monitoring the System

### Water Usage Display

The display shows cumulative grams for the session:
- **0.000 g** - No water used yet
- **0.123 g** - Small amount (few responses)
- **12.345 g** - Moderate use (good conversation)
- **99.999 g** - Approaching default limit (100g)

**When electrolysis is active:**
- Display pulses or glows
- Updates every 500ms
- You can watch it increment in real-time

### Physical Indicators

**What to observe:**
- **Bubbles** - Should see steady stream during activation
- **Relay click** - Audible when electrolysis starts/stops
- **LED indicators** - ESP32 and relay module LEDs
- **Water level** - Gradually decreases over many uses
- **Temperature** - Components should be warm, not hot

### Status Messages

Bottom of screen shows current state:
- **"Ready"** - Waiting for input
- **"Sending..."** - Communication with API
- **"Settings saved"** - Confirmation message
- **"New session started"** - After reset
- **"Error: ..."** - Problem description

### Common Status Messages

**"Error: API key not configured"**
- Go to settings and enter your API key
- Make sure it starts with `sk-ant-`
- Click Save Settings

**"Error: API request failed with code: XXX"**
- 401: Invalid API key
- 403: Rate limit or account issue
- 500: Anthropic server error
- Check internet connection

**"Max water per session reached"**
- Hit the safety limit (default 100g)
- Start a new session to continue
- Or increase limit in settings

## Advanced Usage

### Adjusting Water Consumption Rate

If water consumption is too slow or too fast:

1. **Option 1: Change Cost per Second**
   - Settings → Cost per Second
   - Increase for more water usage
   - Decrease for less water usage
   - This is the easiest adjustment

2. **Option 2: Change Voltage**
   - Higher voltage = more current = faster
   - But requires hardware change
   - More heat and safety concerns

3. **Option 3: Adjust Conductivity**
   - Add more baking soda = higher conductivity
   - Update conductivity value in settings
   - More immediate than voltage change

### Calibrating the System

For accurate water consumption calculations:

1. **Measure actual current:**
   - Use multimeter in series
   - During active electrolysis
   - Record current in amps

2. **Calculate actual conductivity:**
   ```
   R_solution = [(V - V_th) / I_measured] - R_safety
   σ_actual = (100 × d) / (R_solution × A)
   ```

3. **Update settings:**
   - Enter calculated σ_actual as conductivity
   - System now matches physical reality

4. **Verify:**
   - Manually time electrolysis for 60 seconds
   - Measure water loss by weight or volume
   - Compare to formula prediction

### Batch Testing

For research or demonstration:

1. Prepare multiple test prompts in advance
2. Send them one at a time
3. Record for each:
   - API cost
   - Water consumed
   - Token counts
   - Time taken

4. Analyze relationships:
   - Cost vs water (should be linear)
   - Tokens vs cost (5:1 output:input ratio)
   - Complexity vs resource usage

### Remote Access Setup

To access from internet (not just local WiFi):

**Option 1: Port Forwarding**
1. Give ESP32 static IP in your router
2. Forward port 80 to ESP32's IP
3. Access via your public IP
4. Security risk - not recommended without authentication

**Option 2: VPN**
1. Set up VPN server on your network
2. Connect to VPN from outside
3. Access ESP32 normally via 192.168.4.1
4. Much more secure

**Option 3: Ngrok or similar**
1. Run ngrok on a computer on the same network
2. Tunnel to ESP32
3. Access via ngrok URL
4. Free tier available

**Note:** The code currently uses WiFi AP mode (creates network). To connect to existing WiFi, you'll need to modify the `.ino` file to use `WiFi.begin(ssid, password)` instead of `WiFi.softAP()`.

## Troubleshooting

### "No response from Claude"

Check:
1. API key is entered correctly
2. ESP32 has internet connection (if not AP mode, connect it to WiFi)
3. Anthropic API is operational (check status.anthropic.com)
4. You have API credits remaining

### Water usage not increasing

Check:
1. Electrolysis power supply is connected
2. Relay is clicking (listen carefully)
3. Bubbles are forming in solution
4. Safety limits not reached
5. Cost per second isn't too small

### Electrolysis but no bubbles

Check:
1. Electrodes are submerged
2. Solution has baking soda
3. Voltage is sufficient (>3V)
4. Electrodes aren't corroded (clean them)
5. Current is flowing (measure with multimeter)

### Very slow response times

Possible causes:
1. Poor WiFi signal - move closer to ESP32
2. Internet connection slow
3. Anthropic API experiencing delays
4. ESP32 memory nearly full (start new session)

### Connection drops frequently

1. Check USB power supply (needs 1A minimum)
2. Move device away from interference
3. Reduce number of connected devices
4. Restart ESP32
5. Check Serial Monitor for error messages

### Resistor overheating

1. Increase resistor wattage (5W instead of 2W)
2. Increase resistor value (20Ω instead of 10Ω)
3. Improve ventilation
4. Reduce voltage
5. Add heat sink

### "Settings saved" but changes don't apply

For WiFi credential changes:
1. This is expected behavior
2. Press the RESET button on ESP32
3. Or power cycle the device
4. New WiFi settings apply on next boot
5. Check Serial Monitor for new IP address

### Long responses not appearing or cut off

Fixed in latest version:
1. Increased buffer sizes to handle longer responses
2. Max response tokens increased to 2048
3. If still experiencing issues, check Serial Monitor for errors
4. Try shorter prompts or start a new session

### Model selection not working

Check:
1. API key is entered and valid
2. Internet connection is working
3. Click "Fetch Available Models" and wait 2-3 seconds
4. Check Serial Monitor for API errors
5. Verify API key has access to requested model tier

### Checking SPIFFS filesystem contents

To verify files were uploaded correctly:
1. Navigate to: http://[your-esp32-ip]/debug/files
2. Should see list of files: index.html, script.js, styles.css, favicon.svg, etc.
3. If files missing, re-upload filesystem image using PlatformIO or Arduino IDE
4. Check file sizes match expected values

## Best Practices

### For Reliable Operation

- **Keep solution topped up** - Electrolysis evaporates water
- **Clean electrodes monthly** - Buildup reduces efficiency
- **Monitor first few uses** - Ensure everything works correctly
- **Don't leave unattended** - Safety first
- **Ventilate well** - Hydrogen + oxygen = explosive
- **Check connections** - Loose wires cause problems

### For Accurate Results

- **Calibrate conductivity** - Measure and adjust
- **Consistent solution** - Same concentration each time
- **Stable temperature** - Temperature affects conductivity
- **Clean electrodes** - Corrosion affects measurements
- **Record data** - Track usage over time

### For Safety

- **Never exceed 12V DC** - Higher voltage is dangerous
- **Always use safety resistor** - Protects against shorts
- **Keep water away from electronics** - Obvious but critical
- **Watch for overheating** - Hot components indicate problems
- **Adequate ventilation** - Gas buildup is dangerous
- **Supervise minors** - Not a toy despite appearance

### For Demonstrations

- **Test thoroughly first** - Don't demo broken equipment
- **Have backup power** - Extra batteries/power supply
- **Prepare talking points** - Explain the concept clearly
- **Show water consumption** - Make it visible and tangible
- **Discuss implications** - Resource usage of AI
- **Safety brief** - Explain what could go wrong

## Maintenance Schedule

### Before Each Use
- [ ] Check water level (top up if needed)
- [ ] Verify power connections
- [ ] Test that WiFi network appears
- [ ] Open web interface to confirm functionality

### After Each Use
- [ ] Review total water consumption
- [ ] Check for unusual observations
- [ ] Record data if tracking
- [ ] Turn off electrolysis power (optional)

### Weekly
- [ ] Clean any residue from container
- [ ] Check electrode condition
- [ ] Verify all connections secure
- [ ] Test relay operation

### Monthly
- [ ] Replace electrolyte solution
- [ ] Clean electrodes (sand if needed)
- [ ] Check safety resistor for damage
- [ ] Verify settings are correct
- [ ] Update firmware if available

### As Needed
- [ ] Replace corroded electrodes
- [ ] Replace worn safety resistor
- [ ] Recalibrate conductivity
- [ ] Clean container thoroughly

## Understanding the Data

### What the Numbers Mean

**Input Tokens:**
- Your message converted to tokens
- ~4 characters per token typically
- Short messages: 10-50 tokens
- Long messages: 100-500 tokens

**Output Tokens:**
- Claude's response in tokens
- Varies by response length
- Short: 50-200 tokens
- Medium: 200-500 tokens
- Long: 500-1000+ tokens

**Cost:**
- Input: $3 per 1M tokens
- Output: $15 per 1M tokens
- Output costs 5× more (more computation)
- Total = (input × 0.000003) + (output × 0.000015)

**Water Consumed:**
- Based on actual API cost
- Scaled by your "cost per second" setting
- Calculated via electrochemical formula
- Cumulative over session

### Example Analysis

**Simple Question:**
```
User: "What is 2+2?"
Input tokens: 12
Output tokens: 45
Cost: $0.000711
Water: 0.037 g (with defaults)
Time: ~3 seconds
```

**Complex Response:**
```
User: "Explain machine learning"
Input tokens: 20
Output tokens: 450
Cost: $0.006810
Water: 0.355 g (with defaults)
Time: ~7 seconds
```

### Comparing to Real-World Usage

**Put it in perspective:**

A typical ChatGPT/Claude user might:
- Send 50 messages per day
- Average 200 output tokens each
- Cost: ~$0.15 per day
- With this device: ~7.8 grams of water per day

That's about **2.8 liters per year** of water destroyed.

Meanwhile, a single data center might use **millions of liters per year** for cooling. This project makes a tiny fraction of that usage tangible and visible.

## Advanced Configuration

### Modifying the Code

The `.ino` file can be edited to:
- Change WiFi credentials (SSID/password)
- Modify GPIO pin assignments
- Adjust default settings
- Add new features
- Change API model used

After modifications:
1. Compile in Arduino IDE
2. Upload to ESP32
3. Test thoroughly

### Changing WiFi Mode

To connect to existing WiFi instead of creating AP:

In `claude-water-meter.ino`, replace:
```cpp
WiFi.softAP(ap_ssid, ap_password);
```

With:
```cpp
WiFi.begin("YourWiFiName", "YourPassword");
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}
Serial.println(WiFi.localIP());
```

Then access via the printed IP address instead of 192.168.4.1.

### Using Different Claude Models

In `claude-water-meter.ino`, find:
```cpp
requestDoc["model"] = "claude-3-5-sonnet-20241022";
```

Change to:
- `"claude-3-opus-20240229"` (most capable, expensive)
- `"claude-3-sonnet-20240229"` (balanced)
- `"claude-3-haiku-20240307"` (fast, cheap)

Update pricing constants accordingly.

## Getting Support

If you experience issues:

1. **Check Serial Monitor:**
   - Connect via USB
   - Open Arduino IDE → Tools → Serial Monitor
   - Set to 115200 baud
   - Look for error messages

2. **Review this guide:**
   - Troubleshooting section
   - Construction guide for hardware issues
   - Install guide for software issues

3. **Test systematically:**
   - Isolate the problem (hardware vs software?)
   - Test components individually
   - Check connections and settings

4. **Document the issue:**
   - What you were trying to do
   - What you expected to happen
   - What actually happened
   - Any error messages
   - Serial Monitor output

## Conclusion

The Claude Water Meter makes AI resource consumption tangible. Each interaction has a cost - in electricity, in water, in resources. This project transforms abstract computational cost into physical, visible destruction of water.

Use it to:
- Demonstrate AI resource consumption
- Educate about hidden costs
- Provoke thought about sustainability
- Make abstract concepts concrete
- Start conversations about technology's impact

**Operate safely and thoughtfully.** 💧🤖

---

**Quick Reference Card**

```
┌─────────────────────────────────────────┐
│ QUICK REFERENCE                         │
├─────────────────────────────────────────┤
│ WiFi Network: Claude-Water-Meter        │
│ Password: watermeter123                 │
│ Web Interface: http://192.168.4.1      │
│                                         │
│ Default Settings:                       │
│ • Voltage: 5V                          │
│ • Safety Resistor: 10Ω                 │
│ • Cost/Second: $0.001                  │
│ • Max Duration: 300s                   │
│ • Max Water: 100g                      │
│                                         │
│ Emergency:                              │
│ • Unplug USB power                     │
│ • Disconnect electrolysis power        │
│ • Remove from water                    │
└─────────────────────────────────────────┘
```

Print this and keep it near your device!
