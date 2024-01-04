// +-------------------------------------------------------------
//
// Equipment:
// ESP32, OLED SSD1306
//
// File: byteArrayAnim_System.h
//
// Description:
//
// main file to engage all byte array graphics, whether they be
// non-animated or animated on the ESP 32 platform
//
// History:     1-Dec-2023     Scarecrow1965   Created
//
// NOTE: Initial byteArrayAnim.h File too big 1.3Mb, so I had to
//  split it into several files
//

// install ibraries
#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include "animations.h"

// This will enable for the OLED screen to display information
// definition of OLED display SSD1306 for Arduino Mega SCA & SDL
// #define OLED_CLOCK 17 // SCA pin on Display = SCL on ESP32
// #define OLED_DATA 20  // SDL pin on display = SDA on ESP32
// U8G2 SSD1306 Driver here to run OLED Screen
// built constructor for the OLED function
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLED_CLOCK, OLED_DATA, U8X8_PIN_NONE); // This works but according to the function, it shouldn't
// int oled_LineH = 0;

// ADAFRUIT SSD1306 Driver here to run animation
// NOTE: confirmed by I2C Scanner address for SSD1306 is 0x3c
// NOTE: const int SSD1306_addr = 0x3c; // to initialilze the address of the OLED display
// #define SCREEN_I2C_ADDR 0x3C // or 0x3C
// #define SCREEN_WIDTH 128     // OLED display width, in pixels
// #define SCREEN_HEIGHT 64     // OLED display height, in pixels
// #define OLED_RST_PIN -1      // Reset pin (-1 if not available)

// Adafruit_SSD1306 display(128, 64, &Wire, OLED_RST_PIN);
// #include "byteArrayAnim_System.h"
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
extern Adafruit_SSD1306 display;

static const uint8_t totalarrays_System = 12; // ensure this is the same as the number of arrays in the SyustemArray array

Frame SystemArray[12]{
    {"/by_bell.bin", 28, "Bell"},
    {"/by_chkOK.bin", 28, "Checkmark OK"},
    {"/by_clksp.bin", 28, "Spinning Clock"},
    {"/by_globe.bin", 28, "Globe"},
    {"/by_home.bin", 28, "Home"},
    {"/by_hrgl.bin", 28, "Hourglass"},
    {"/by_noCon.bin", 28, "No Connection"},
    {"/by_snd.bin", 28, "Sound"},
    {"/by_wifish.bin", 28, "WIFI Search"},
    {"/by_gear.bin", 28, "Gear"},
    {"/by_gears.bin", 28, "Gears"},
    {"/by_setng.bin", 28, "Settings"},
};

void byteArraySystem_Anim(void)
{
    Serial.println("Starting Meteo byte Array loop");

    for (uint8_t i = 0; i < totalarrays_System; i++)
    {
        uint8_t *data;
        uint8_t dataSize;

        loadAnimation(SystemArray[i].fileName, &data, &dataSize);

        uint8_t frame = 0;
        uint8_t effecttime = 30;

        u8g2.clearBuffer();

        while (effecttime > 0)
        {
            u8g2.home();
            u8g2.setCursor(3, oled_LineH * 1 + 2);
            u8g2.print(SystemArray[i].name);
            u8g2.sendBuffer();

            // Serial.print("Displaying frame= "); // used for testing purposes only
            // Serial.println(frame); // used for testing purposes only

            display.drawBitmap(0, 15, &data[frame * 288], framewidth, frameheight, 1);
            display.display();
            frame = (frame + 1) % SystemArray[i].frameCounts;
            effecttime--;
            // delay(1000); // used for testing purposes only
            display.clearDisplay();
        }
        // Reset frame to 0 for the next animation
        frame = 0;

        // Don't forget to delete the data array to free up memory
        delete[] data;
    }

    Serial.println("ending loop");
}; // end byte Array Animation Loop function

void byteArraySystem_Display(uint8_t i)
{
    uint8_t *data;
    uint8_t dataSize;

    loadAnimation(SystemArray[i].fileName, &data, &dataSize);

    uint8_t frame = 0;
    for (uint8_t j = 0; j < framecount; j++)
    {
        display.drawBitmap(0, 15, &data[frame * 288], framewidth, frameheight, 1);
        display.display();
        frame = (frame + 1) % SystemArray[i].frameCounts;
        display.clearDisplay();
    }

    // Don't forget to free the allocated memory when you're done with it
    delete[] data;

}; // end byte Array Animation Display function

// {bell, (sizeof(bell) / sizeof(bell[0])), "Bell"},
void System_bell(void)
{
    uint8_t i = 0;
    byteArraySystem_Display(i);
}; // end bell function

// {checkmarkOK, (sizeof(checkmarkOK) / sizeof(checkmarkOK[0])), "Checkmark OK"},
void System_checkmarkOK(void)
{
    uint8_t i = 1;
    byteArraySystem_Display(i);
}; // end checkmarkOK function

// {clockspin, (sizeof(clockspin) / sizeof(clockspin[0])), "Spinning Clock"},
void System_clockspin(void)
{
    uint8_t i = 2;
    byteArraySystem_Display(i);
}; // end clockspin function

// {globe, (sizeof(globe) / sizeof(globe[0])), "Globe"},
void System_globe(void)
{
    uint8_t i = 3;
    byteArraySystem_Display(i);
}; // end globe function

// {home, (sizeof(home) / sizeof(home[0])), "Home"},
void System_home(void)
{
    uint8_t i = 4;
    byteArraySystem_Display(i);
}; // end home function

// {hourglass, (sizeof(hourglass) / sizeof(hourglass[0])), "Hourglass"},
void System_hourglass(void)
{
    uint8_t i = 5;
    byteArraySystem_Display(i);
}; // end hourglass function

// {noConnection, (sizeof(noConnection) / sizeof(noConnection[0])), "No Connection"},
void System_noConnection(void)
{
    uint8_t i = 6;
    byteArraySystem_Display(i);
}; // end noConnection function

// {sound, (sizeof(sound) / sizeof(sound[0])), "Sound"},
void System_sound(void)
{
    uint8_t i = 7;
    byteArraySystem_Display(i);
}; // end sound function

// {wifisearch, (sizeof(wifisearch) / sizeof(wifisearch[0])), "WIFI Search"},
void System_wifisearch(void)
{
    uint8_t i = 8;
    byteArraySystem_Display(i);
}; // end wifisearch function

// {gear, (sizeof(gear) / sizeof(gear[0])), "Gear"},
void System_gear(void)
{
    uint8_t i = 9;
    byteArraySystem_Display(i);
}; // end gear function

// {gears, (sizeof(gears) / sizeof(gears[0])), "Gears"},
void System_gears(void)
{
    uint8_t i = 10;
    byteArraySystem_Display(i);
}; // end gears function

// {settings, (sizeof(settings) / sizeof(settings[0])), "Settings"},
void System_settings(void)
{
    uint8_t i = 11;
    byteArraySystem_Display(i);
}; // end settings function

// =========================================================
// END OF PROGRAM
// =========================================================
