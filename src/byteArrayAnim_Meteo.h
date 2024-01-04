// +-------------------------------------------------------------
//
// Equipment:
// ESP32, OLED SSD1306
//
// File: byteArrayAnim_Meteo.h
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

const uint8_t totalarrays_Meteo = 11; // ensure this is the same as the number of arrays in the WeatherArray below

Frame MeteoArray[11] {
    {"/by_cldWx.bin", 28, "Cloudy Weather"},
    {"/by_lSnWx.bin", 28, "Light Snow Weather"},
    {"/by_lngWx.bin", 28, "Lightning Weather"},
    {"/by_bltWx.bin", 28, "Lightning Bolt Weather"},
    {"/by_rngWx.bin", 28, "Rainy Weather"},
    {"/by_snoWx.bin", 28, "Snowstorm Weather"},
    {"/by_stoWx.bin", 28, "Stormy Weather"},
    {"/by_sunWx.bin", 28, "Sun Weather"},
    {"/by_tmpWx.bin", 28, "Temperature Weather"},
    {"/by_tRnWx.bin", 28, "Torrential Rain Weather"},
    {"/by_wndWx.bin", 28, "Windy Weather"},
};

void byteArrayMeteo_Anim(void)
{
    Serial.println("Starting Meteo byte Array loop");

    for (uint8_t i = 0; i < totalarrays_Meteo; i++)
    {
        uint8_t *data;
        uint8_t dataSize;

        loadAnimation(MeteoArray[i].fileName, &data, &dataSize);

        uint8_t frame = 0;
        uint8_t effecttime = 30;

        u8g2.clearBuffer();

        while (effecttime > 0)
        {
            u8g2.home();
            u8g2.setCursor(3, oled_LineH * 1 + 2);
            u8g2.print(MeteoArray[i].name);
            u8g2.sendBuffer();

            // Serial.print("Displaying frame= "); // used for testing purposes only
            // Serial.println(frame); // used for testing purposes only

            display.drawBitmap(0, 15, &data[frame * 288], framewidth, frameheight, 1);
            display.display();
            frame = (frame + 1) % MeteoArray[i].frameCounts;
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

void byteArrayMeteo_Display(uint8_t i)
{
    uint8_t *data;
    uint8_t dataSize;

    loadAnimation(MeteoArray[i].fileName, &data, &dataSize);

    uint8_t frame = 0;
    for (uint8_t j = 0; j < framecount; j++)
    {
        display.drawBitmap(0, 15, &data[frame * 288], framewidth, frameheight, 1);
        display.display();
        frame = (frame + 1) % MeteoArray[i].frameCounts;
        display.clearDisplay();
    }

    // Don't forget to free the allocated memory when you're done with it
    delete[] data;

}; // end byte Array Animation Display function

// {cloudyWeather, (sizeof(cloudyWeather) / sizeof(cloudyWeather[0])), "Cloudy Weather"},
void Meteo_cloudyWeather(void)
{
    uint8_t i = 0;
    byteArrayMeteo_Display(i);
}; // end cloudyWeather Display function

// {lightSnowWeather, (sizeof(lightSnowWeather) / sizeof(lightSnowWeather[0])), "Light Snow Weather"},
void Meteo_lightSnowWeather(void)
{
    uint8_t i = 1;
    byteArrayMeteo_Display(i);
}; // end lightSnowWeather Display function

// {lightningWeather, (sizeof(lightningWeather) / sizeof(lightningWeather[0])), "Lightning Weather"},
void Meteo_lightningWeather(void)
{
    uint8_t i = 2;
    byteArrayMeteo_Display(i);
}; // end lightningWeather Display function

// {lightningboltWeather, (sizeof(lightningboltWeather) / sizeof(lightningboltWeather[0])), "Lightning Bolt Weather"},
void Meteo_lightningboltWeather(void)
{
    uint8_t i = 3;
    byteArrayMeteo_Display(i);
}; // end lightningboltWeather Display function

// {rainyWeather, (sizeof(rainyWeather) / sizeof(rainyWeather[0])), "Rainy Weather"},
void Meteo_rainyWeather(void)
{
    uint8_t i = 4;
    byteArrayMeteo_Display(i);
}; // end rainyWeather Display function

// {snowStormWeather, (sizeof(snowStormWeather) / sizeof(snowStormWeather[0])), "Snowstorm Weather"},
void Meteo_snowStormWeather(void)
{
    uint8_t i = 5;
    byteArrayMeteo_Display(i);
}; // end snowStormWeather Display function

// {stormyWeather, (sizeof(stormyWeather) / sizeof(stormyWeather[0])), "Stormy Weather"},
void Meteo_stormyWeather(void)
{
    uint8_t i = 6;
    byteArrayMeteo_Display(i);
}; // end stormyWeather Display function

// {sunWeather, (sizeof(sunWeather) / sizeof(sunWeather[0])), "Sun Weather"},
void Meteo_sunWeather(void)
{
    uint8_t i = 7;
    byteArrayMeteo_Display(i);
}; // end sunWeather Display function

// {temperatureWeather, (sizeof(temperatureWeather) / sizeof(temperatureWeather[0])), "Temperature Weather"},
void Meteo_temperatureWeather(void)
{
    uint8_t i = 8;
    byteArrayMeteo_Display(i);
}; // end temperatureWeather Display function

// {torrentialRainWeather, (sizeof(torrentialRainWeather) / sizeof(torrentialRainWeather[0])), "Torrential Rain Weather"},
void Meteo_torrentialRainWeather(void)
{
    uint8_t i = 9;
    byteArrayMeteo_Display(i);
}; // end torrentialRainWeather Display function

// {windyWeather, (sizeof(windyWeather) / sizeof(windyWeather[0])), "Windy Weather"},
void Meteo_windyWeather(void)
{
    uint8_t i = 10;
    byteArrayMeteo_Display(i);
}; // end windyWeather Display function

// =========================================================
// END OF PROGRAM
// =========================================================
