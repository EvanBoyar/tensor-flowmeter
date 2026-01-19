# Construction Guide - AI Water User

This guide walks you through building the physical hardware for the AI Water User project.

## Tools Required

- [ ] Soldering iron and solder (for permanent connections)
- [ ] Wire strippers
- [ ] Multimeter (for testing)
- [ ] Screwdriver set
- [ ] Hot glue gun (optional, for securing components)
- [ ] Drill and bits (if mounting electrodes to container)

## Parts Checklist

### Electronics
- [ ] ESP32 development board (programmed - see INSTALL.md)
- [ ] 5V relay module OR logic-level MOSFET module
- [ ] Power supply for electrolysis (5-12V DC, 1A+)
- [ ] **Safety resistor (10-100Ω, 2W+ rating)** - CRITICAL!
- [ ] Jumper wires (male-to-female recommended)
- [ ] USB cable for ESP32 power
- [ ] Breadboard or perfboard (optional, for prototyping)

### Electrolysis Cell
- [ ] Two electrodes (stainless steel or graphite)
  - Stainless steel bolts/screws (M6 or larger)
  - OR graphite rods (from batteries or art supply)
- [ ] Non-conductive container (plastic, glass)
- [ ] Baking soda (sodium bicarbonate)
- [ ] Distilled or filtered water
- [ ] Wires to connect electrodes to circuit

### Optional
- [ ] Enclosure box for electronics
- [ ] Cable glands for waterproofing wire entry
- [ ] Heat shrink tubing
- [ ] Terminal blocks
- [ ] Mounting hardware

## Part 1: Choosing and Preparing Electrodes

### Electrode Materials

**Option A: Stainless Steel (Recommended for beginners)**
- Pros: Easy to find, durable, good conductivity
- Cons: Will degrade over time
- Where to get: Hardware store - M6 or M8 stainless bolts/screws
- Recommended size: At least 5cm long

**Option B: Graphite (Best for durability)**
- Pros: Doesn't corrode, excellent for electrolysis
- Cons: More brittle, harder to find
- Where to get:
  - Graphite rods from large batteries (lantern batteries)
  - Art supply stores (graphite sculpting rods)
  - Carbon fiber rod (check material compatibility)
- Recommended size: 10-15cm long, 6-8mm diameter

### Electrode Preparation

1. **Clean the electrodes:**
   - Sand with fine sandpaper to remove oxidation
   - Wash with soap and water
   - Rinse with distilled water
   - Dry completely

2. **Attach wire connections:**

   **For stainless steel bolts:**
   - Solder wire directly to bolt head
   - OR use ring terminals and nuts
   - Cover connection with heat shrink tubing

   **For graphite rods:**
   - Wrap thin wire tightly around one end
   - Solder if possible (use high-temp solder)
   - Cover with heat shrink tubing
   - Alternatively, use conductive epoxy
   - I maaaay have just alligator clipped them ¯\_(ツ)_/¯

3. **Insulate the connections:**
   - Use heat shrink tubing over all exposed wire
   - Ensure only the working electrode surface is exposed to water
   - Test that no bare wire will touch the solution

## Part 2: Building the Electrolysis Cell

### Container Selection

Choose a container:
- Non-conductive (plastic or glass)
- Size: 500ml - 2L capacity
- Wide mouth for easy electrode placement
- Transparent (optional, allows monitoring)
- Heat resistant (electrolysis generates heat)

**Good options:**
- Glass jar or beaker
- Plastic food storage container
- Cut-down plastic bottle

### Mounting Electrodes

**Method 1: Suspended electrodes (easiest)**
1. Drill holes in container lid
2. Insert electrodes through holes
3. Position electrodes 2-3cm apart (measure with ruler)
4. Ensure electrodes don't touch
5. Seal around holes with hot glue or silicone
6. Wires exit through lid

**Method 2: Side-mounted (more stable)**
1. Drill holes in container sides
2. Mount electrodes horizontally
3. Use rubber grommets for waterproofing
4. Position 2-3cm apart
5. Seal with silicone

**Method 3: Base-mounted (professional)**
1. Attach electrodes to non-conductive base plate
2. Use plastic standoffs to maintain spacing
3. Place assembly in container
4. Wires exit over rim or through sealed holes

### Measuring Your Setup

**IMPORTANT:** Measure these values - you'll need them for settings!

1. **Electrode Area (A):**
   - Measure electrode width and height that will be submerged
   - Calculate: A = width (cm) × height (cm)
   - Example: 1cm wide × 5cm tall = 5 cm²

2. **Electrode Gap (d):**
   - Measure distance between electrodes with ruler
   - Measure center-to-center or face-to-face (be consistent)
   - Typical: 2-3 cm

3. **Write down these measurements!** You'll enter them in settings.

### Preparing the Electrolyte Solution

**DO NOT ADD ELECTROLYTE YET** - wait until after wiring test!

Recipe for when ready:
1. Use distilled or filtered water (tap water works but is less consistent)
2. Add 1-2 tablespoons baking soda per liter
3. Stir until dissolved
4. Solution should be clear

**Notes:**
- More baking soda = higher conductivity = faster electrolysis
- Start with moderate concentration (1 tbsp/L)
- You can adjust later based on performance
- Solution will become cloudy with use (normal)

## Part 3: Wiring the Circuit

### Pin Identification on ESP32

Locate these pins on your ESP32:
- **GPIO 26** - Signal to relay/MOSFET
- **GND** - Ground (multiple pins available)
- **5V** or **VIN** - For relay power (if needed)

### Relay Module Wiring

Most relay modules have these connections:

**Control Side (Low Voltage):**
- **VCC** → ESP32 **5V** or **VIN**
- **GND** → ESP32 **GND**
- **IN** or **S** → ESP32 **GPIO 26**

**Power Side (Switches High Voltage):**
- **COM** (common)
- **NO** (normally open) - Use this one
- **NC** (normally closed) - Don't use

### MOSFET Module Wiring (Alternative)

If using MOSFET instead of relay:

**Control Side:**
- **VCC** → ESP32 **3.3V**
- **GND** → ESP32 **GND**
- **SIG** → ESP32 **GPIO 26**

**Power Side:**
- **V+** → Power supply positive
- **V-** → GND
- **OUT** → To load

### Complete Circuit Assembly

**Step-by-step wiring:**

1. **Connect ESP32 to relay/MOSFET:**
   ```
   ESP32 GPIO 26 ──────► Relay IN (or MOSFET SIG)
   ESP32 GND      ──────► Relay GND
   ESP32 5V       ──────► Relay VCC (if needed)
   ```

2. **Connect power supply through relay:**
   ```
   Power Supply (+) ──────► Relay COM
   Relay NO         ──────► Safety Resistor
   Safety Resistor  ──────► Electrode 1 (+)
   Power Supply (-) ──────► Electrode 2 (-)
   ```

3. **The complete circuit should look like:**
   ```
   [ESP32] ──(control)──► [Relay/MOSFET]
                              │
   [Power] ──(+)──► [COM/V+]  │
                              │
                           [NO/OUT] ──► [Safety Resistor] ──► [Electrode 1]

   [Power] ──(-)──────────────────────────────► [Electrode 2]
   ```

### Safety Resistor Installation

**CRITICAL:** Install the safety resistor correctly!

1. **Choose resistor value:**
   - 5V power supply → 10Ω resistor
   - 9V power supply → 18Ω resistor
   - 12V power supply → 22Ω resistor

2. **Check power rating:**
   - Must be at least 2W (3-5W recommended)
   - Wire-wound or metal oxide type preferred
   - Should have "2W", "3W", or "5W" marked on it

3. **Connect in series:**
   - One leg connects to relay NO (or MOSFET OUT)
   - Other leg connects to electrode positive (+)
   - Polarity doesn't matter for resistors
   - Can solder or use terminal block

4. **Secure the resistor:**
   - Will get warm during operation
   - Mount away from plastic
   - Allow airflow around it
   - Can use heat sink if available

### Wire Management

1. Use appropriate wire gauge:
   - Control signals (ESP32 to relay): 22-24 AWG okay
   - Power to electrodes: 18-20 AWG recommended

2. Keep wires organized:
   - Separate low voltage (ESP32) from high current (electrodes)
   - Use zip ties or wire loom
   - Label wires with tape and marker

3. Strain relief:
   - Secure wires to prevent pulling
   - Use cable glands where wires enter enclosure
   - Hot glue stress points
   - Don't be like me, I just knotted the wire so it can't be tugged thru the holes

## Part 4: Testing BEFORE Connecting Water

**Test the circuit WITHOUT electrodes in water first!**

### Step 1: Visual Inspection

- [ ] All connections are secure
- [ ] No exposed wire that could short
- [ ] Safety resistor is in series with electrode
- [ ] Polarity is correct (if using DC)
- [ ] ESP32 is not touching metal or water
- [ ] Relay/MOSFET is properly connected

### Step 2: Multimeter Testing

1. **Check for shorts:**
   - Set multimeter to continuity mode (beep)
   - Test between power supply (+) and (-)
   - Should read OPEN (no beep) when relay is OFF
   - Should read resistance of resistor + electrodes when ON

2. **Check safety resistor:**
   - Disconnect one end of resistor
   - Measure resistance
   - Should read close to rated value (10Ω, etc.)
   - If reads 0Ω → short, replace resistor
   - If reads infinite → open, replace resistor

3. **Check voltages:**
   - Power on ESP32 (via USB)
   - Measure voltage at GPIO 26: should be 0V or 3.3V
   - Don't trigger relay yet - we'll test that next

### Step 3: Dry Run Test

1. **Power on ESP32** (USB only, no electrolysis power yet)

2. **Connect to WiFi:**
   - On phone/computer, connect to "Claude-Water-Meter"
   - Password: watermeter123
   - Open browser to http://192.168.4.1

3. **Go to Settings and enter test values:**
   - API Key: (leave blank for now)
   - Voltage: Your power supply voltage
   - V_threshold: 2.0
   - Conductivity: 1.0
   - Area: Your measured area (cm²)
   - Gap: Your measured gap (cm)
   - Safety Resistor: Your resistor value (Ω)
   - Cost per Second: 0.001
   - Click Save

4. **Test relay activation** (electrodes still not in water):
   - Connect electrolysis power supply
   - Have multimeter ready to measure current
   - Send a test message to Claude (you'll get an error without API key)
   - Listen for relay click
   - Measure current with multimeter (should be ~0A with no load)

If relay clicks, **hardware is working!** ✓

## Part 5: First Water Test

### Add the Electrolyte

1. **Prepare solution:**
   - Add water to container (don't overfill - leave room)
   - Add 1 tablespoon baking soda per liter
   - Stir until dissolved

2. **Position electrodes:**
   - Submerge electrodes to measured depth
   - Ensure they don't touch
   - Ensure only electrode surface is in water (no bare wire)
   - Verify electrode spacing matches your measured gap

3. **Final visual check:**
   - No wire shorts
   - Container stable and won't tip
   - Electronics away from water
   - Ventilation is adequate

### Test Electrolysis

1. **Power everything on:**
   - Connect ESP32 via USB
   - Connect electrolysis power supply
   - Wait for ESP32 to boot (check Serial Monitor)

2. **Manually trigger relay** (testing only):
   - You can use Arduino Serial Monitor
   - Or send a test message via web interface
   - Relay should activate

3. **Observe electrolysis:**
   - Bubbles should form on both electrodes
   - Hydrogen forms at negative electrode (more bubbles)
   - Oxygen forms at positive electrode (fewer bubbles)
   - Solution may become cloudy
   - You may hear faint fizzing sound

4. **Check for issues:**
   - **No bubbles:** Check connections, increase baking soda
   - **Excessive heat:** Reduce voltage or increase resistor value
   - **Resistor too hot to touch:** Use higher wattage resistor
   - **Sparking:** Turn off immediately, check for shorts

5. **Measure actual current (important):**
   - Use multimeter in series
   - Should be 0.2-0.5A typically
   - If over 1A, increase safety resistor value
   - Calculate power: P = I² × R_safety (should be less than resistor rating)

### Calculate Expected Water Usage

With your measured values, calculate expected consumption:

**Example:**
- V = 5V
- V_th = 2V
- σ = 1 S/m
- A = 5 cm²
- d = 2 cm
- R_safety = 10Ω
- t = 10 seconds

Current: I = (5-2) × 1 × 5 / (10 × 1 × 5 + 100 × 2) ≈ 0.05A

Water: m = 0.05A × 10s × 9.328×10⁻⁵ g/C ≈ 0.047g

You should consume approximately this much water per 10 seconds.

## Part 6: Final Assembly

### Enclosure (Optional but Recommended)

1. **Choose/build an enclosure:**
   - Plastic project box
   - 3D printed case
   - Cardboard box (for testing only)

2. **Mount components:**
   - Secure ESP32 with standoffs or double-sided tape
   - Mount relay/MOSFET module
   - Mount safety resistor with airflow
   - Leave ventilation holes

3. **Cable management:**
   - Use cable glands for wires exiting box
   - Label all cables
   - Secure with zip ties

4. **Ventilation:**
   - Add ventilation holes/slots
   - Ensure resistor can dissipate heat
   - ESP32 needs airflow too

### Mounting the System

1. **Position electrolysis cell:**
   - On stable, level surface
   - Away from electronics
   - Easy to observe
   - Good ventilation
   - Away from ignition sources

2. **Position electronics enclosure:**
   - Away from water (at least 30cm)
   - Accessible for USB programming
   - Visible indicator LEDs (if present)
   - Away from electrolysis gases

3. **Wire routing:**
   - Keep electrode wires separate from data lines
   - Secure wires to prevent tripping
   - Avoid sharp bends
   - Leave some slack

## Part 7: Calibration

### Measure Actual Conductivity

1. With your specific solution, measure current:
   - I_measured = (measure with multimeter in amps)

2. Calculate conductivity:
   - R_sol = (V - V_th) / I - R_safety
   - σ = 100 × d / (R_sol × A)

3. Update this value in settings for accuracy

### Test Water Consumption

1. **Prepare for measurement:**
   - Mark starting water level on container
   - Or use a scale to weigh container

2. **Run for known time:**
   - Manually trigger relay for exactly 60 seconds
   - (You can modify code temporarily for testing)

3. **Measure water loss:**
   - Mark new water level
   - Or weigh container again
   - Calculate mass lost

4. **Compare to formula:**
   - Does actual match calculated?
   - If not, adjust parameters in settings
   - Conductivity is most likely to need adjustment

## Troubleshooting

### No Bubbles
- Check all wire connections
- Verify power supply is on
- Increase baking soda concentration
- Check that electrodes aren't oxidized (sand them)

### Weak Bubbling
- Increase voltage (within safe limits)
- Add more baking soda
- Check that safety resistor isn't too large
- Ensure electrodes have good surface area in water

### Excessive Bubbling/Heat
- Reduce voltage
- Increase safety resistor value
- Reduce baking soda concentration
- Ensure adequate cooling

### Relay Doesn't Activate
- Check GPIO 26 connection
- Verify ESP32 is programmed correctly
- Check relay module power supply
- Test with multimeter on GPIO 26 (should toggle)

### Solution Turns Cloudy
- Normal! This is dissolved electrode material
- More common with stainless steel
- Doesn't affect operation
- Can filter or replace solution periodically

### Resistor Gets Very Hot
- Normal for it to warm up
- If too hot to touch for more than 1 second:
  - Use higher wattage resistor (5W instead of 2W)
  - Reduce current (increase resistance value)
  - Add heat sink
  - Improve ventilation

## Maintenance

### Weekly
- [ ] Check wire connections
- [ ] Verify water level
- [ ] Check for corrosion on electrodes
- [ ] Clean any buildup on electrodes

### Monthly
- [ ] Replace solution if very cloudy
- [ ] Sand/clean electrodes
- [ ] Check resistor for overheating damage
- [ ] Verify all settings are correct

### As Needed
- [ ] Replace electrodes when heavily corroded
- [ ] Replace solution if electrolysis slows
- [ ] Tighten any loose connections
- [ ] Clean container if buildup occurs

## Construction Complete!

Your hardware is now assembled and tested. Proceed to **USAGE.md** to learn how to operate the system.

## Construction Checklist

Final verification before first real use:

- [ ] All wiring is secure and insulated
- [ ] Safety resistor is correct value and wattage
- [ ] Electrodes don't touch each other
- [ ] Electronics are away from water
- [ ] Ventilation is adequate
- [ ] Power supply voltage is correct
- [ ] ESP32 boots and creates WiFi network
- [ ] Web interface loads at 192.168.4.1
- [ ] Relay activates on command
- [ ] Electrolysis produces bubbles
- [ ] Current is within safe limits (<1A)
- [ ] Resistor doesn't overheat
- [ ] All measurements are recorded in settings

**If all items are checked, you're ready to use the system!** 🎉
