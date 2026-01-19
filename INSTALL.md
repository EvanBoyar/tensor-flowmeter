# Installation Guide - AI Water User

This guide walks you through installing the software on your ESP32.

## Prerequisites

Before you begin, ensure you have:
- [ ] ESP32 development board
- [ ] USB cable (data capable, not charge-only)
- [ ] Computer running Windows, macOS, or Linux
- [ ] Stable internet connection

## Step 1: Install Arduino IDE

### Windows
1. Download Arduino IDE from [arduino.cc/en/software](https://www.arduino.cc/en/software)
2. Run the installer (.exe file)
3. Follow the installation wizard
4. Launch Arduino IDE

### macOS
1. Download Arduino IDE from [arduino.cc/en/software](https://www.arduino.cc/en/software)
2. Open the .dmg file
3. Drag Arduino to Applications folder
4. Launch Arduino IDE from Applications

### Linux
```bash
# Download the latest version
wget https://downloads.arduino.cc/arduino-ide/arduino-ide_latest_Linux_64bit.AppImage

# Make it executable
chmod +x arduino-ide_*_Linux_64bit.AppImage

# Run it
./arduino-ide_*_Linux_64bit.AppImage
```

## Step 2: Install ESP32 Board Support

1. Open Arduino IDE
2. Go to **File → Preferences** (or **Arduino IDE → Settings** on macOS)
3. Find "Additional Board Manager URLs" field
4. Paste the following URL:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
5. Click **OK**

6. Go to **Tools → Board → Boards Manager**
7. Wait for the list to load
8. Search for "esp32"
9. Find "esp32 by Espressif Systems"
10. Click **Install**
11. Wait for installation to complete (may take several minutes)
12. Close Boards Manager

## Step 3: Install Required Libraries

1. Go to **Tools → Manage Libraries** (or **Sketch → Include Library → Manage Libraries**)
2. Wait for library list to load

### Install ArduinoJson
1. Search for "ArduinoJson"
2. Find "ArduinoJson by Benoit Blanchon"
3. Select version **6.x** (not 7.x)
4. Click **Install**

That's it! The other required libraries (WiFi, WebServer, Preferences, HTTPClient, SPIFFS) are included with ESP32 board support.

## Step 4: Download the Project Files

### Option A: Git Clone (if you have git)
```bash
cd ~/Documents
git clone <repository-url>
cd claude-water-meter
```

### Option B: Manual Download
1. Download the project as a ZIP file
2. Extract it to a location you can find (e.g., Documents folder)
3. Note the location of the `claude-water-meter` folder

## Step 5: Connect Your ESP32

1. Connect ESP32 to your computer via USB cable
2. Wait for drivers to install (Windows may need a moment)

### Select the Board
1. Go to **Tools → Board → esp32**
2. Select **ESP32 Dev Module** (or your specific board model)

### Select the Port
1. Go to **Tools → Port**
2. Select the port with your ESP32:
   - **Windows**: Usually `COM3`, `COM4`, etc.
   - **macOS**: Usually `/dev/cu.usbserial-*` or `/dev/cu.SLAB_USBtoUART`
   - **Linux**: Usually `/dev/ttyUSB0` or `/dev/ttyACM0`

**Troubleshooting Port Selection:**
- If no port appears, disconnect and reconnect the ESP32
- On Windows, you may need to install [CP210x drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- On Linux, you may need to add yourself to the `dialout` group:
  ```bash
  sudo usermod -a -G dialout $USER
  # Log out and back in for this to take effect
  ```

### Configure Upload Settings
Go to **Tools** menu and configure:
- **Upload Speed**: 921600
- **Flash Frequency**: 80MHz
- **Flash Mode**: QIO
- **Flash Size**: 4MB (32Mb)
- **Partition Scheme**: Default 4MB with spiffs
- **Core Debug Level**: None (or Info for debugging)

## Step 6: Install ESP32 Sketch Data Upload Plugin

This plugin allows you to upload the web interface files to ESP32's filesystem.

### For Arduino IDE 2.x (Newer Version)

1. Download the plugin from [GitHub](https://github.com/me-no-dev/arduino-esp32fs-plugin/releases)
2. Download the file: `esp32fs-X.X.X.vsix`
3. In Arduino IDE, go to **File → Preferences**
4. Note the "Sketchbook location" path
5. Close Arduino IDE
6. Create this directory structure:
   ```
   <sketchbook>/tools/
   ```
7. Extract the plugin ZIP into the tools folder
8. Restart Arduino IDE

### For Arduino IDE 1.x (Older Version)

1. Download the plugin from [GitHub](https://github.com/me-no-dev/arduino-esp32fs-plugin/releases)
2. Download `ESP32FS-X.X.X.zip`
3. In Arduino IDE, go to **File → Preferences**
4. Note the "Sketchbook location" path
5. Close Arduino IDE
6. Navigate to sketchbook location
7. Create `tools` folder if it doesn't exist
8. Extract the ZIP file into tools folder so you have:
   ```
   <sketchbook>/tools/ESP32FS/tool/esp32fs.jar
   ```
9. Restart Arduino IDE

**Verify Installation:**
- Open Arduino IDE
- You should see **Tools → ESP32 Sketch Data Upload** in the menu
- If not, review the steps above or check plugin documentation

## Step 7: Open the Project

1. In Arduino IDE, go to **File → Open**
2. Navigate to the `claude-water-meter` folder
3. Open `claude-water-meter.ino`
4. The project should open with all files visible in tabs

## Step 8: Upload the Filesystem (SPIFFS)

**Important:** Do this BEFORE uploading the sketch!

1. Make sure ESP32 is connected and port is selected
2. Close Serial Monitor if it's open (important!)
3. Go to **Tools → ESP32 Sketch Data Upload**
4. Wait for upload to complete (takes 1-2 minutes)
5. You should see "SPIFFS Image Uploaded" when done

**Troubleshooting:**
- **Error: "SPIFFS Image Upload Failed"** - Close Serial Monitor and try again
- **Error: "esptool not found"** - Reinstall ESP32 board support
- **Timeout errors** - Try lowering upload speed in Tools menu

## Step 9: Upload the Sketch

1. Click the **Upload** button (→ arrow icon) or press **Ctrl+U** (Cmd+U on macOS)
2. Wait for compilation (first time will take a few minutes)
3. Watch the output window for progress
4. You should see:
   ```
   Connecting........____.....
   Chip is ESP32-D0WDQ6 (revision 1)
   ...
   Writing at 0x000xxxxx... (100%)
   Leaving...
   Hard resetting via RTS pin...
   ```
5. Upload complete!

**Troubleshooting:**
- **Error: "A fatal error occurred: Failed to connect"**
  - Hold the BOOT button on ESP32 while clicking Upload
  - Release BOOT button when "Connecting..." appears
  - Try a different USB cable
  - Lower upload speed to 115200

- **Compilation errors:**
  - Verify ArduinoJson library is version 6.x (not 7.x)
  - Check that all files are in the sketch folder
  - Try restarting Arduino IDE

## Step 10: Verify Installation

1. Open Serial Monitor: **Tools → Serial Monitor**
2. Set baud rate to **115200**
3. Press the **RESET** button on your ESP32
4. You should see output like:
   ```
   AI Water User Starting...
   SPIFFS Mount Success
   Settings loaded from NVS
   AP IP address: 192.168.4.1
   HTTP server started
   ```

If you see this output, **installation is complete!** 🎉

## Next Steps

- Proceed to **CONSTRUCTION.md** to build the hardware
- Or jump to **USAGE.md** if hardware is already assembled

## Common Issues

### ESP32 Not Detected
- Try different USB cable (must be data cable, not charge-only)
- Install CP210x or CH340 drivers for your operating system
- On Linux: Add user to dialout group and reboot

### Upload Fails at 100%
- This sometimes happens but is successful - check Serial Monitor
- Press RESET button and verify device starts correctly

### SPIFFS Upload Not Available
- Reinstall ESP32 Sketch Data Upload plugin
- Ensure you extracted to correct location
- Restart Arduino IDE

### Out of Memory Errors
- Select correct partition scheme: "Default 4MB with spiffs"
- Verify Flash Size is set correctly (usually 4MB)

### Library Not Found
- Use Library Manager, not manual installation
- Ensure ArduinoJson is version 6.x
- Restart Arduino IDE after installing libraries

## Getting Help

If you encounter issues:
1. Check Serial Monitor output for error messages
2. Verify all settings in Tools menu
3. Try the troubleshooting steps above
4. Search for the error message online
5. Check ESP32 Arduino GitHub issues

## System Requirements

**Minimum:**
- 2GB RAM
- 500MB free disk space
- USB 2.0 port

**Recommended:**
- 4GB+ RAM
- 1GB free disk space
- USB 3.0 port
- Stable USB connection (not through hub if possible)

## Installation Complete!

Your ESP32 is now programmed and ready. Proceed to the Construction Guide to build the hardware assembly.
