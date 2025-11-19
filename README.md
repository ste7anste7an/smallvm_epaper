# README #

## EPAPER for Microblocks ##

This repository shows how to integrate epaper displays in MicroBlcoks. Fir this integration the [GxEPD2 library](https://github.com/ZinggJM/GxEPD2) is used. 

## Changes made for this integartion
All changes are marked with `// epaper`  in the files `persist.c` and `tftPrims.cpp`.

- Added a new “display backend” in tftPrims.cpp
    - a new board section is added to `tftPrimnscpp` defining a new global tft (which is derived from Arduino_GFX and thus compatible with all the tft methods already used in `tftPrims..cpp`
- Overrode UPDATE_DISPLAY() to be non-blocking

Original macro just did a taskSleep(-1) after slow operations.
All the existing primitives (setPixel, rect, text, drawBuffer, …) stay unchanged; they still call UPDATE_DISPLAY(), which now just sets a flag instead of blocking.
- Added a service function for refresh: tftServiceEPD(). Beacuse this is going to be called from `persist.c` it neds a "C" wrapper.

```cpp
extern "C" void tftServiceEPD(void) {
    // state machine:
    //  - if epdDirty, refresh in small stripes using displayWindow()
    //  - spread work over multiple calls
}
```

This function looks at epdDirty and refreshes only a stripe of the screen per call (using displayWindow()), not the entire panel at once and clears epdDirty when done.

- call `tftServiceEPD()` from the VM loop in `interp.c`