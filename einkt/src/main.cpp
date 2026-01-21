#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2.h>
#include <Fonts/FreeMonoBold12pt7b.h>

// 2.9" WeActStudio (SSD1680-compatible)
#define EPD_CS   5
#define EPD_DC   15
#define EPD_RST  13
#define EPD_BUSY 4
#define CS_PIN 5
#define DC_PIN 15
#define RES_PIN 13
#define BUSY_PIN 4
// GxEPD2_BW<GxEPD2_290, GxEPD2_290::HEIGHT>
//   display(GxEPD2_290(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

#define MAX_DISPLAY_BUFFER_SIZE 500ul // ~15k is a good compromise
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))

GxEPD2_BW<GxEPD2_290_BS,  MAX_HEIGHT(GxEPD2_290_BS)> display(GxEPD2_290_BS(/*CS=5*/ CS_PIN, /*DC=*/ DC_PIN, /*RES=*/ RES_PIN, /*BUSY=*/ BUSY_PIN)); // DEPG0290BS 128x296, SSD1680

void setup()
{
    Serial.begin(115200);
    delay(2000);

    display.init(115200, true, 2, false);
    display.setRotation(3);

    Serial.println("Starting full refresh with timing…");

    uint32_t totalStart = millis();

    display.firstPage();
    int page = 1;

    do {
        uint32_t t0 = micros();     // start timing

        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(10, 40);
        display.setFont(&FreeMonoBold12pt7b);
        display.println("WeActStudio 2.9\"");
        display.setCursor(10, 80);
        display.println("ESP32 + GxEPD2");

        uint32_t t1 = micros();     // end timing
        Serial.printf("Page %d took %u us (%.2f ms)\n",
                      page++, t1 - t0, (t1 - t0) / 1000.0);

    } while (display.nextPage());

    uint32_t total = millis() - totalStart;

    Serial.printf("Full refresh finished in %u ms\n", total);
}



void loop()
{
    // Nothing – ePaper keeps image without power
    delay(2000);
}
