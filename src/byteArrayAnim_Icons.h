// +-------------------------------------------------------------
//
// Equipment:
// ESP32, OLED SSD1306
//
// File: byteArrayAnim_Icons.h
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
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
extern Adafruit_SSD1306 display;

const uint8_t totalarrays_Icons = 7; // ensure this is the same as the number of arrays in the IconsArray array

Frame IconsArray[7] {
    {"/by_hrtbt.bin", 28, "Heartbeat"},
    {"/by_acft.bin", 28, "Aircraft"},
    {"/by_event.bin", 28, "Event"},
    {"/by_plot.bin", 28, "Plot"},
    {"/by_toggl.bin", 28, "Toggle"},
    {"/by_opLet.bin", 28, "Open Letter"},
    {"/by_phrng.bin", 28, "Phone Ringing"},
};

void byteArrayIcons_Anim(void)
{
    Serial.println("Starting Meteo byte Array loop");

    for (uint8_t i = 0; i < totalarrays_Icons; i++)
    {
        uint8_t *data;
        uint8_t dataSize;

        loadAnimation(IconsArray[i].fileName, &data, &dataSize);

        uint8_t frame = 0;
        uint8_t effecttime = 30;

        u8g2.clearBuffer();

        while (effecttime > 0)
        {
            u8g2.home();
            u8g2.setCursor(3, oled_LineH * 1 + 2);
            u8g2.print(IconsArray[i].name);
            u8g2.sendBuffer();

            // Serial.print("Displaying frame= "); // used for testing purposes only
            // Serial.println(frame); // used for testing purposes only

            display.drawBitmap(0, 15, &data[frame * 288], framewidth, frameheight, 1);
            display.display();
            frame = (frame + 1) % IconsArray[i].frameCounts;
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

void byteArrayIcons_Display(uint8_t i)
{
    uint8_t *data;
    uint8_t dataSize;

    loadAnimation(IconsArray[i].fileName, &data, &dataSize);

    uint8_t frame = 0;
    for (uint8_t j = 0; j < framecount; j++)
    {
        display.drawBitmap(0, 15, &data[frame * 288], framewidth, frameheight, 1);
        display.display();
        frame = (frame + 1) % IconsArray[i].frameCounts;
        display.clearDisplay();
    }

    // Don't forget to free the allocated memory when you're done with it
    delete[] data;

}; // end byte Array Animation Display function

// {heartbeat, (sizeof(heartbeat) / sizeof(heartbeat[0])), "Heartbeat"},
void Icons_heartbeat(void)
{
    uint8_t i = 0;
    byteArrayIcons_Display(i);
}; // end heartbeatDisplay function

// {aircraft, (sizeof(aircraft) / sizeof(aircraft[0])), "Aircraft"},
void Icons_aircraft(void)
{
    uint8_t i = 1;
    byteArrayIcons_Display(i);
}; // end aircraftDisplay function

// {event, (sizeof(event) / sizeof(event[0])), "Event"},
void Icons_event(void)
{
    uint8_t i = 2;
    byteArrayIcons_Display(i);
}; // end eventDisplay function

// {plot, (sizeof(plot) / sizeof(plot[0])), "Plot"},
void Icons_plot(void)
{
    uint8_t i = 3;
    byteArrayIcons_Display(i);
}; // end plotDisplay function

// {toggle, (sizeof(toggle) / sizeof(toggle[0])), "Toggle"},
void Icons_toggle(void)
{
    uint8_t i = 4;
    byteArrayIcons_Display(i);
}; // end toggleDisplay function

// {openLetter, (sizeof(openLetter) / sizeof(openLetter[0])), "Open Letter"},
void Icons_openLetter(void)
{
    uint8_t i = 5;
    byteArrayIcons_Display(i);
}; // end openLetterDisplay function

// {phoneringing, (sizeof(phoneringing) / sizeof(phoneringing[0])), "Phone Ringing"},
void Icons_phoneRinging(void)
{
    uint8_t i = 6;
    byteArrayIcons_Display(i);
}; // end phoneringingDisplay function

// =========================================================
// END OF PROGRAM
// =========================================================
