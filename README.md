# AI Water User

_MAKE_ AI resource consumption an actual issue by literally destroying water through electrolysis in proportion to the computational cost of AI interactions!

## 📚 Getting Started

**Follow these guides in order:**

1. **[INSTALL.md](INSTALL.md)** - Install software on ESP32 (30-60 minutes)
   - Install Arduino IDE and ESP32 support
   - Upload code and web interface
   - Verify ESP32 is working

2. **[CONSTRUCTION.md](CONSTRUCTION.md)** - Build the hardware (1-2 hours)
   - Wire the circuit with safety resistor
   - Prepare electrodes and electrolyte
   - Test and calibrate the system

3. **[USAGE.md](USAGE.md)** - Operate the system (start using!)
   - Connect and configure
   - Chat with Claude
   - Monitor water consumption

**Quick Overview:** This project uses an ESP32 to host a web interface where you interact with AI (powered by Claude). Each response triggers electrolysis that consumes water proportional to the API cost—making invisible computational resources tangibly visible. The "Tensor Flow Meter" displays real-time water consumption.

## Overview

This project combines an ESP32 microcontroller with a web interface to create an interactive demonstration of AI's resource footprint. When you chat with Claude through the web interface, the system triggers an electrolysis reaction that consumes water proportional to the actual API cost (which reflects electricity used by Anthropic's servers).

**Key Features:**
- Clean web interface styled after claude.ai
- Real-time water consumption tracking
- API cost-based electrolysis control
- Dynamic model selection from Anthropic API
- Support for longer responses (up to 2048 tokens)
- Persistent settings storage
- Safety limits and controls
- Smart WiFi: connects to your home network, accessible from any device on same WiFi
- Fallback Access Point mode for easy configuration
- Web-based WiFi configuration (no code changes needed)
- **Safety resistor protection** against short circuits

## Hardware Requirements

### Core Components
- **ESP32 Development Board** (ESP32-WROOM-32 or similar)
- **Relay Module or MOSFET** (5V relay or logic-level MOSFET rated for your voltage)
- **Power Supply** for electrolysis (5-12V DC recommended)
- **Safety Resistor** (10-100Ω, rated for at least 2W - **CRITICAL for safety**)
- **Electrodes** (stainless steel or graphite recommended)
- **Container** for water solution
- **Jumper wires**

### Electrolysis Setup
- Two electrodes (stainless steel screws, graphite rods, or similar)
- Baking soda solution (1-2 tablespoons per liter of water)
- Non-conductive container to hold solution

## Wiring Diagram

```
ESP32 Pin 26 ──────► Relay/MOSFET Signal Pin
ESP32 GND   ──────► Relay/MOSFET GND
ESP32 5V    ──────► Relay/MOSFET VCC (if needed)

Relay/MOSFET ──────► Safety Resistor ──────► Power Supply (+) ──────► Electrode 1
                                             Power Supply (-)        ──────► Electrode 2

Note: The relay/MOSFET controls power to the electrodes.
When GPIO 26 is HIGH, electrolysis runs. When LOW, it stops.

IMPORTANT: The safety resistor should be placed in series with the power supply.
- Recommended: 10-100 ohms, rated for at least 2W
- Purpose: Limits current if electrodes accidentally short circuit
- If electrodes touch (d→0), current is limited to: I_max = V / R_safety
```

### Pin Configuration
- **GPIO 26**: Relay/MOSFET control (configurable in code)
- Can be changed by modifying `RELAY_PIN` in the .ino file

## Choosing the Safety Resistor

The safety resistor is **critical for safety** - it limits current if the electrodes accidentally touch or short circuit.

### Sizing the Resistor

**Step 1: Determine maximum safe current**
- For low-voltage DC systems: 0.5-1A is typically safe
- For 5V supply: Use 10Ω resistor → I_max = 5V / 10Ω = 0.5A
- For 12V supply: Use 22Ω resistor → I_max = 12V / 22Ω = 0.55A

**Step 2: Calculate power rating**
- Power dissipated: P = I² × R
- For 10Ω at 0.5A: P = (0.5)² × 10 = 2.5W
- **Use at least 2W rated resistor** (3W or 5W recommended for margin)

**Step 3: Consider impact on normal operation**
- The resistor reduces water consumption rate
- With typical solution resistance of 10-100Ω, a 10Ω safety resistor is reasonable
- If you want faster electrolysis, use a smaller resistor (but never go below safe current limits)

### Recommended Values

| Supply Voltage | Resistor Value | Max Current | Min Power Rating |
|----------------|----------------|-------------|------------------|
| 5V | 10Ω | 0.5A | 2W |
| 9V | 18Ω | 0.5A | 3W |
| 12V | 22Ω | 0.55A | 5W |

### Important Notes
- **Never omit the safety resistor** - even a small value provides protection
- Use a **wire-wound or metal oxide resistor** for better heat dissipation
- The resistor will get warm during operation - this is normal
- If the resistor gets too hot to touch, use a higher wattage rating
- You can adjust the value in settings after installation to match your actual resistor

## Installation & Setup

**See detailed guides:**
- **[INSTALL.md](INSTALL.md)** - Complete software installation instructions
- **[CONSTRUCTION.md](CONSTRUCTION.md)** - Hardware building guide
- **[USAGE.md](USAGE.md)** - Operating instructions and troubleshooting

**Quick summary:**
1. Install Arduino IDE and ESP32 support
2. Upload code and web interface to ESP32
3. Wire circuit with safety resistor
4. Prepare electrodes and electrolyte
5. Connect to WiFi network "AI-Water-User"
6. Access http://192.168.4.1
7. Configure settings and start chatting

**Default Settings:**

| Parameter | Default | Description |
|-----------|---------|-------------|
| WiFi SSID | AI-Water-User | Network name |
| WiFi Password | watermeter123 | Network password |
| Web Address | 192.168.4.1 | Local access point |
| Voltage | 5.0 V | Power supply voltage |
| Safety Resistor | 10.0 Ω | Short-circuit protection |
| Cost per Second | $0.001/s | API cost to time scaling |

## Customization & Advanced Configuration

For customization options including WiFi configuration, GPIO pin changes, and internet hosting setup, see **[USAGE.md - Advanced Configuration](USAGE.md#advanced-configuration)**.

For troubleshooting common issues, see **[USAGE.md - Troubleshooting](USAGE.md#troubleshooting)**.

## Safety Warning ⚠️

**Critical safety requirements:**
- Use only LOW VOLTAGE DC (5-12V max)
- ALWAYS use a safety resistor (10-100Ω, ≥2W)
- Ensure adequate ventilation (produces H₂ + O₂)
- Keep electronics away from water
- Adult supervision required

For complete safety information, see **[CONSTRUCTION.md - Safety](CONSTRUCTION.md#safety-first-)** and **[USAGE.md - Best Practices](USAGE.md#best-practices)**.

## Theory: The Water Consumption Formula

The water consumption is calculated based on Faraday's law of electrolysis, accounting for the safety resistor:

```
m = 9.328e-5 * (V - V_th) * σ * A * t / (R_safety * σ * A + 100 * d)
```

Where:
- **m** = mass of water consumed (grams)
- **V** = applied voltage (volts)
- **V_th** = threshold voltage for electrolysis (~2V for water)
- **σ** = conductivity of solution (S/m)
- **A** = electrode area (cm²)
- **d** = electrode gap (cm)
- **R_safety** = safety resistor (ohms)
- **t** = time (seconds)

### Derivation

**Step 1: Calculate Resistance**
- Solution resistance: R_sol = (100 × d) / (σ × A) [ohms, with d in cm, A in cm²]
- Total resistance: R_total = R_safety + R_sol

**Step 2: Calculate Current**
- Current through circuit: I = (V - V_th) / R_total [amps]
- This gives: I = (V - V_th) × σ × A / (R_safety × σ × A + 100 × d)

**Step 3: Apply Faraday's Law**
For the reaction 2H₂O → 2H₂ + O₂:
- 4 electrons are needed to split 2 water molecules (36 grams)
- Charge transferred: Q = I × t [coulombs]
- Moles of electrons: n_e = Q / F where F = 96485 C/mol (Faraday constant)
- Mass of water: m = n_e × 36/4 = Q × 9/F = I × t × 9.328×10⁻⁵

**Combining:**
```
m = 9.328e-5 * (V - V_th) * σ * A * t / (R_safety * σ * A + 100 * d)
```

### Special Cases

**When R_safety = 0 (no safety resistor):**
```
m = 9.328e-7 * (V - V_th) * σ * A * t / d
```
This is the simplified formula without external resistance.

**When electrodes short-circuit (d → 0):**
```
Current limited to: I_max = (V - V_th) / R_safety
```
This demonstrates the safety resistor's protection: even if electrodes touch, current cannot exceed this limit.

### Physical Interpretation

The formula shows that:
1. **Higher voltage** → more current → more water consumed
2. **Higher conductivity** → more current → more water consumed
3. **Larger electrode area** → more current → more water consumed
4. **Safety resistor** → limits current → reduces water consumption (and provides safety)
5. **Larger gap** → higher solution resistance → less current → less water consumed

## API Cost Scaling

The system uses **actual API cost** rather than raw token counts because:
- Output tokens cost ~5x more than input tokens (computationally)
- This accurately reflects Anthropic's electricity consumption
- API pricing already accounts for computational complexity

With default settings ($0.001 per second):
- A typical response (~$0.001-0.01) runs electrolysis for 1-10 seconds
- This consumes approximately 0.5-5 milligrams of water (500-5000 µg)
- The Tensor Flow Meter displays consumption in micrograms for precision
- The ratio can be adjusted in settings

## Project Structure

```
claude-water-meter/
├── README.md                  # Project overview and technical reference
├── INSTALL.md                 # Software installation guide
├── CONSTRUCTION.md            # Hardware building guide
├── USAGE.md                   # Operating instructions
├── QUICKREF.md                # Quick reference card (printable)
├── claude-water-meter.ino     # Main ESP32 firmware
├── platformio.ini             # PlatformIO config (optional)
├── .gitignore                 # Git ignore file
└── data/                      # Web interface (upload to SPIFFS)
    ├── index.html             # Main HTML interface (production)
    ├── index-test.html        # Test HTML (for local testing without ESP32)
    ├── styles.css             # CSS styling
    ├── script.js              # Client JavaScript (production - connects to ESP32)
    ├── script-test.js         # Test JavaScript (uses localStorage, mock API)
    └── favicon.svg            # Water droplet favicon
```

## License

This is an educational/art project. Use responsibly and at your own risk.

## Acknowledgments

- Anthropic for Claude API
- ESP32 community for excellent documentation
- Arduino ecosystem

## About This Project

**AI Water User** is meant to provoke thought about AI resource consumption. The physical destruction of water through electrolysis serves as a tangible metaphor for the invisible environmental cost of AI systems. The "Tensor Flow Meter" makes these abstract computational costs visible and visceral.

**Remember:** Every interaction with AI has real-world costs in electricity, cooling, and infrastructure. This project transforms those hidden costs into something you can watch disappear in real-time.
