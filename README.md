# README #

## EPAPER for Microblocks ##

This repository demonstrates how to integrate e-paper displays into MicroBlocks. The integration uses the [GxEPD2 library](https://github.com/ZinggJM/GxEPD2).

## Main integration

The standard TFT driver is replaced with a GxEPD2 e-ink driver. All drawing primitives continue to write into the display buffer as before, but the slow e-paper panel refresh has been moved to `tftServiceEPD()`.

This service function is called from the VM loop and performs the refresh in small partial steps, ensuring that the interpreter never blocks on a full e-ink update.

## Connecting display
The current integration targets an ESP32-WROOM microcontroller and uses the default VSPI pins:

```
		// SDA 	-- GPIO23 (MOSI)
		//      -- GPIO19 (MISO) not used
        // SCL 	-- GPIO18 (SCLK)
        
		// CS  	-- GPIO5
		// DC 	-- GPIO15
		// RES	-- GPIO13
		// BUSY	-- GPIO4
		// VCC	-- 3V3
		// GND	-- GND
```
## Using the epaper
You can use the existing TFT library APIs.

Use white (255, 255, 255) for the background

Use black (0, 0, 0) for drawing objects

The LED Display can also be used, but make sure to set the display color to black.
The same applies to turtle graphics: set the line color to black.

## Changes made for this integration

All changes are marked with `// epaper` in the following files:

- `persist.c`
- `tftPrims.cpp`

### Display backend
- Added a new display backend in `tftPrims.cpp`.
- Added a new board section defining a global tft object. This object is derived from Arduino_GFX and is therefore compatible with all existing TFT methods used in `tftPrims.cpp`.
- Overrode the following methods:
	- `drawRGBBitmap`
	- `draw16bitRGBBitmap`
	- `pushImage`

### Non-blocking display updates
The UPDATE_DISPLAY() macro was overridden to be non-blocking.

Originally, this macro called taskSleep(-1) after slow operations. All existing drawing primitives (setPixel, rect, text, drawBuffer, etc.) remain unchanged and still call UPDATE_DISPLAY(), which now only sets a flag instead of blocking.

### E-paper refresh service
-A new refresh service function, `ftServiceEPD()`, was added. Because it is called from `persist.c`, it uses a C linkage wrapper:

```cpp
extern "C" void tftServiceEPD(void) {
    // state machine:
    //  - if epdDirty, refresh in small stripes using displayWindow()
    //  - spread work over multiple calls
}
```

### VM integration

`tftServiceEPD()` is called from the VM loop in `interp.c`, allowing the refresh process to run incrementally without blocking the interpreter.

## Summary of integration

```
+----------------------+
| Drawing Primitive    |
| (setPixel, rect, ...)|
+----------+-----------+
           |
           v
+----------------------+
| UPDATE_DISPLAY()     |
| sets epdDirty = true |
+----------+-----------+
           |
           v
+----------------------+
| VM Loop              |
| calls tftServiceEPD()|
+----------+-----------+
           |
           v
+---------------------------+
| tftServiceEPD()           |
| - check epdDirty          |
| - refresh small stripe    |
| - spread over multiple    |
|   calls                   |
+----------+----------------+
           |
           v
+----------------------+
| Full display updated |
| epdDirty = false     |
+----------------------+
```

## Support for other epaper screen
To support additional e-paper panels, refer to:

`https://github.com/ZinggJM/GxEPD2/blob/master/examples/GxEPD2_Example/GxEPD2_display_selection.h`

Select the appropriate display configuration and update `tftPrims.cpp` accordingly.
