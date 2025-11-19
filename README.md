# README #

## EPAPER for Microblocks ##

This repository shows how to integrate epaper displays in MicroBlcoks. Fir this integration the [GxEPD2 library](https://github.com/ZinggJM/GxEPD2) is used. 

## Main integration

The TFT driver is replaced with a GxEPD2 e-ink driver. All drawing primitives write into its buffer as before, but the slow panel refresh has been moved into `tftServiceEPD()`, which the VM loop calls in small partial steps so the interpreter never blocks on a full e-ink update.

## Connecting display
The current integration uses an ESP32-WROOM microcontroller. It uses the default VSPI pins for the ESP32-WROOM:

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
You can use the `TFT library`. Use white (255,255,255) for background and black (0,0,0) for drawing black tft objects. `LED Display` can also be used, but don't forget to set display color to balck. Same for turtle graphics, set line volor to black.

## Changes made for this integartion
All changes are marked with `// epaper`  in the files `persist.c` and `tftPrims.cpp`.

- Added a new “display backend” in tftPrims.cpp
    - a new board section is added to `tftPrimnscpp` defining a new global tft (which is derived from Arduino_GFX and thus compatible with all the tft methods already used in `tftPrims..cpp`
    - overrode methods in tft `drawRGBBitmap`, `draw16bitRGBBitmap`, and `pushImage`
- Overrode UPDATE_DISPLAY() to be non-blocking

Original macro just did a taskSleep(-1) after slow operations.
All the existing primitives (setPixel, rect, text, drawBuffer, …) stay unchanged; they still call UPDATE_DISPLAY(), which now just sets a flag instead of blocking.
- Added a service function for refresh: tftServiceEPD(). Because this is going to be called from `persist.c` it needs a "C" wrapper.

```cpp
extern "C" void tftServiceEPD(void) {
    // state machine:
    //  - if epdDirty, refresh in small stripes using displayWindow()
    //  - spread work over multiple calls
}
```

This function looks at epdDirty and refreshes only a stripe of the screen per call (using displayWindow()), not the entire panel at once and clears epdDirty when done.

- call `tftServiceEPD()` from the VM loop in `interp.c`

## Support for other epaper screen
Look in `https://github.com/ZinggJM/GxEPD2/blob/master/examples/GxEPD2_Example/GxEPD2_display_selection.h` for the matching epaper display and make changes in `tftPrims.cpp` accordingly.
