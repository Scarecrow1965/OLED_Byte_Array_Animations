// +-------------------------------------------------------------
//
// Equipment:
// ESP32, OLED SSD1306
//
// File: main.cpp
//
// Description:
//
// main file to engage all byte array graphics, whether they be
// non-animated or animated on the ESP 32 platform
//
// History:     1-Nov-2023     Scarecrow1965   Created
//
// +-------------------------------------------------------------

// CONFIRMED: WORKS ON ESP32 AND OLED SCREEN

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

// #define LED_BUILTIN 2 // pin for onboard LED or use LED_BUILTIN as the default location
bool bLED = LOW;

// This will enable for the OLED screen to display information
// definition of OLED display SSD1306 for Arduino Mega SCA & SDL
#define OLED_CLOCK 17 // SCA pin on Display = SCL on ESP32
#define OLED_DATA 20  // SDL pin on display = SDA on ESP32
// U8G2 SSD1306 Driver here to run OLED Screen
// built constructor for the OLED function
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLED_CLOCK, OLED_DATA, U8X8_PIN_NONE); // This works but according to the function, it shouldn't
// static uint8_t oled_LineH = 0;

// ADAFRUIT SSD1306 Driver here to run animation
// NOTE: confirmed by I2C Scanner address for SSD1306 is 0x3c
// NOTE: const int SSD1306_addr = 0x3c; // to initialilze the address of the OLED display
#define SCREEN_I2C_ADDR 0x3C // or 0x3C
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RST_PIN -1      // Reset pin (-1 if not available)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST_PIN);

// adding the SD card reader
#define SCK 17
#define MISO 19
#define MOSI 23
#define CS 5

#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include "animations.h" // this is the header file for the animations

SPIClass spi = SPIClass(VSPI);
File file;

// variables for calling specific files
const char* file0 = "/";
// const char* file1 = "/byteArrayAnim_Meteo.cpp";
// const char* file2 = "/byteArrayAnim_Position.cpp";
// const char* file3 = "/byteArrayAnim_Battery.cpp";
// const char* file4 = "/byteArrayAnim_System.cpp";
// const char* file5 = "/byteArrayAnim_Icons.cpp";
// const char* path6 = "/byteArrayAnim.h";

void loadAnimation(const char *fileName, uint8_t **data, uint8_t *dataSize)
{
    if (!SD.begin(5))
    {
        Serial.println("SD card initialization failed!");
        return;
    }

    // Open the file
    File file = SD.open(fileName);
    if (!file)
    {
        Serial.println("Failed to open file");
        return;
    }

    // Get the size of the file
    *dataSize = file.size() / 288;

    // Allocate memory for the data
    *data = new uint8_t [*dataSize * 288];
    // for (int i = 0; i < *dataSize; i++)
    // {
    //     (*data)[i] = new uint8_t[288]; // Assuming each frame is 288 bytes
    // }

    // Read the data from the file
    for (int i = 0; i < *dataSize; i++)
    {
        file.read(&(*data)[i * 288], 288);
    }

    // Close the file
    file.close();
};

#include "byteArrayAnim_Battery.h"
#include "byteArrayAnim_Icons.h"
#include "byteArrayAnim_Meteo.h"
#include "byteArrayAnim_Position.h"
#include "byteArrayAnim_System.h"

// including functions for SD card reader
void listDir(fs::FS & fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}; // end listDir function

// ==================================
// ONE TIME MANDATORY FUNCTION - DO NOT REMOVE
// ==================================
void setup(void)
{
    delay(1000); // adding delay for powering up the ESP32
    pinMode(LED_BUILTIN, OUTPUT); // relying on GPIO2 LED to light up on MB

    Serial.begin(115200);
    Serial.println("Starting setup");

    // for U8G2 library setup
    u8g2.begin();
    u8g2.clear();
    u8g2.setFont(u8g2_font_profont10_tf);
    oled_LineH = u8g2.getFontAscent() + u8g2.getFontAscent();

    // for Adafruit library setup
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDR);
    display.clearDisplay();

    // for SD card setup
    if (!SD.begin(5))
    {
        Serial.println("Card Mount Failed");
        return;
    }
    spi.begin(SCK, MISO, MOSI, CS);
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    Serial.println("Setup complete");
}; // end setup function

// ==================================
// REPETITIVE MANDATORY FUNCTION - DO NOT REMOVE
// ==================================
void loop(void)
{
    Serial.println("looking for animation files");
    // Commands for SD card reader
    // listDir(SD, "/", 0);
    // createDir(SD, "/mydir");
    // removeDir(SD, "/mydir");
    // writeFile(SD, "/hello.txt", "Hello ");
    // appendFile(SD, "/hello.txt", "World!\n");
    // readFile(SD, "/hello.txt");
    // deleteFile(SD, "/foo.txt");
    // renameFile(SD, "/hello.txt", "/foo.txt");
    // readFile(SD, "/foo.txt");
    // testFileIO(SD, "/test.txt");
    listDir(SD, file0, 0);

    Serial.println("Starting Byte Array Animation loop");

    // byteArray_Anim(); // call the function to run the animation not in class
    // Calling individual functions when dealing with separated .h files
    // byteArrayAnimation.byteArray_Anim(); // call the function to run the animation in class
    // byteArrayAnimation_Meteo.byteArrayMeteo_Anim(); // call the function to run the animation in class
    // byteArrayAnimation_Position.byteArrayPosition_Anim(); // call the function to run the animation in class
    // byteArrayAnimation_System.byteArraySystem_Anim(); // call the function to run the animation in class
    // byteArrayAnimation_Icons.byteArrayIcons_Anim(); // call the function to run the animation in class
    // byteArrayAnimation_Battery.byteArrayBattery_Anim(); // call the function to run the animation in class

    // Calling functions to display all byte array animatyions of each groupings
    bLED = !bLED; // toggle LED State
    digitalWrite(LED_BUILTIN, bLED);
    Serial.println("Meteo Animation starting");
    byteArrayMeteo_Anim(); // call the function to run the animation

    bLED = !bLED; // toggle LED State
    digitalWrite(LED_BUILTIN, bLED);
    Serial.println("Position Animation starting");
    byteArrayPosition_Anim(); // call the function to run the animation

    bLED = !bLED; // toggle LED State
    digitalWrite(LED_BUILTIN, bLED);
    Serial.println("Battery Animation starting");
    byteArrayBattery_Anim(); // call the function to run the animation

    bLED = !bLED; // toggle LED State
    digitalWrite(LED_BUILTIN, bLED);
    Serial.println("System Animation starting");
    byteArraySystem_Anim(); // call the function to run the animation

    bLED = !bLED; // toggle LED State
    digitalWrite(LED_BUILTIN, bLED);
    Serial.println("Icons Animation starting");
    byteArrayIcons_Anim(); // call the function to run the animation

    // calling an individual animation from each groupings
    bLED = !bLED; // toggle LED State
    digitalWrite(LED_BUILTIN, bLED);
    Serial.println("Meteo Lightning Bolt Weather Animation starting");
    Meteo_lightningboltWeather(); // call the function to run the lightning bolt weather animation
    Serial.println("Position Down Arrow Animation starting");
    Position_Down(); // call the function to run the down arrow animation
    Serial.println("Battery Low Level Animation starting");
    Battery_isLow(); // call the function to run the low battery level animation
    Serial.println("System Sound Animation starting");
    System_sound(); // call the function to run the sound animation
    Serial.println("Icons Heartbeat Animation starting");
    Icons_heartbeat(); // call the function to run the heartbeat animation

    Serial.println("ending loop");
}; // end loop function

// creating objects for individual .h files
// byteArrayAnim byteArrayAnimation; // create an instance of the class for byteArrayAnim.h
// byteArrayAnim_Battery byteArrayAnimation_Battery; // create an instance of the class for byteArrayAnim_Battery.h
// byteArrayAnim_Meteo byteArrayAnimation_Meteo; // create an instance of the class for byteArrayAnim_Meteo.h
// byteArrayAnim_Position byteArrayAnimation_Position; // create an instance of the class for byteArrayAnim_Position.h
// byteArrayAnim_System byteArrayAnimation_System; // create an instance of the class for byteArrayAnim_System.h
// byteArrayAnim_Icons byteArrayAnimation_Icons; // create an instance of the class for byteArrayAnim_Icons.h

// byteArrayAnim byteArrayAnimation; // create an instance of the class
// byteArrayAnim_Battery byteArrayAnimation_Battery; // create an instance of the class
// byteArrayAnim_Meteo byteArrayAnimation_Meteo; // create an instance of the class
// byteArrayAnim_Position byteArrayAnimation_Position; // create an instance of the class
// byteArrayAnim_System byteArrayAnimation_System; // create an instance of the class
// byteArrayAnim_Icons byteArrayAnimation_Icons; // create an instance of the class

// FileFunction fileRegistry1[] = {
//     {"byteArrayAnim_Meteo.cpp", byteArrayMeteo_Anim}, // "byteArrayMeteo_Anim"
//     {"byteArrayAnim_Meteo.cpp", "Meteo_cloudyWeather"},
//     {"byteArrayAnim_Meteo.cpp", "Meteo_lightSnowWeather"},
//     {"byteArrayAnim_Meteo.cpp", "Meteo_lightningWeather"},
//     {"byteArrayAnim_Meteo.cpp", "Meteo_lightningboltWeather"},
//     {"byteArrayAnim_Meteo.cpp", "Meteo_rainyWeather"},
//     {"byteArrayAnim_Meteo.cpp", "Meteo_snowStormWeather"},
//     {"byteArrayAnim_Meteo.cpp", "Meteo_stormyWeather"},
//     {"byteArrayAnim_Meteo.cpp", "Meteo_sunWeather"},
//     {"byteArrayAnim_Meteo.cpp", "Meteo_temperatureWeather"},
//     {"byteArrayAnim_Meteo.cpp", "Meteo_torrentialRainWeather"},
//     {"byteArrayAnim_Meteo.cpp", "Meteo_windyWeather"},
// };
// FileFunction fileRegistry2[] = {
//     {"byteArrayAnim_Position.cpp", "byteArrayPosition_Anim"},
//     {"byteArrayAnim_Position.cpp", "UninstallingUpdatesArrow"},
//     {"byteArrayAnim_Position.cpp", "InstallingUpdatesArrow"},
//     {"byteArrayAnim_Position.cpp", "UploadArrow"},
//     {"byteArrayAnim_Position.cpp", "DownloadArrow"},
//     {"byteArrayAnim_Position.cpp", "DownArrow"},
// };
// FileFunction fileRegistry3[] = {
//     {"byteArrayAnim_Battery.cpp", "byteArrayBattery_Anim"},
//     {"byteArrayAnim_Battery.cpp", "isbatteryLevel"},
//     {"byteArrayAnim_Battery.cpp", "isbatteryCharging"},
//     {"byteArrayAnim_Battery.cpp", "isbatteryCharged"},
//     {"byteArrayAnim_Battery.cpp", "isbatteryLow"},
// };
// FileFunction fileRegistry4[] = {
//     {"byteArrayAnim_System.cpp", "byteArraySystem_Anim"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_bell"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_checkmarkOK"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_clockspin"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_globe"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_home"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_hourglass"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_noConnection"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_sound"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_wifisearch"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_gear"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_gears"},
//     {"byteArrayAnim_System.cpp", "byteArraySystem_settings"},
// };
// FileFunction fileRegistry5[] = {
//     {"byteArrayAnim_Icons.cpp", "byteArrayIcons_Anim"},
//     {"byteArrayAnim_Icons.cpp", "heartbeatDisplay"},
//     {"byteArrayAnim_Icons.cpp", "aircraftDisplay"},
//     {"byteArrayAnim_Icons.cpp", "eventDisplay"},
//     {"byteArrayAnim_Icons.cpp", "plotDisplay"},
//     {"byteArrayAnim_Icons.cpp", "toggleDisplay"},
//     {"byteArrayAnim_Icons.cpp", "openLetterDisplay"},
//     {"byteArrayAnim_Icons.cpp", "phoneRingingDisplay"},
// };

// /**************************************************************************/
// /*!
//    @brief      Draw a PROGMEM-resident 1-bit image at the specified (x,y)
//    position, using the specified foreground color (unset bits are transparent).
//     @param    x   Top left corner x coordinate
//     @param    y   Top left corner y coordinate
//     @param    bitmap  byte array with monochrome bitmap
//     @param    w   Width of bitmap in pixels
//     @param    h   Height of bitmap in pixels
//     @param    color 16-bit 5-6-5 Color to draw with
// */
// /**************************************************************************/
// void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
//                               int16_t w, int16_t h, uint16_t color)
// {
// 
//   int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
//   uint8_t b = 0;
// 
//   startWrite();
//   for (int16_t j = 0; j < h; j++, y++)
//   {
//     for (int16_t i = 0; i < w; i++)
//     {
//       if (i & 7)
//         b <<= 1;
//       else
//         b = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
//       if (b & 0x80)
//         writePixel(x + i, y, color);
//     }
//   }
//   endWrite();
// }

// /**************************************************************************/
// /*!
//    @brief    Start a display-writing routine, overwrite in subclasses.
// */
// /**************************************************************************/
// void Adafruit_GFX::startWrite() {}
// 
// #ifndef pgm_read_byte
// #define pgm_read_byte(addr) (*(const unsigned char *)(addr))

// /**************************************************************************/
// /*!
//    @brief    Write a pixel, overwrite in subclasses if startWrite is defined!
//     @param   x   x coordinate
//     @param   y   y coordinate
//    @param    color 16-bit 5-6-5 Color to fill with
// */
// /**************************************************************************/
// void Adafruit_GFX::writePixel(int16_t x, int16_t y, uint16_t color)
// {
//   drawPixel(x, y, color);
// }

// /**************************************************************************/
// /*!
//    @brief    Write a pixel, overwrite in subclasses if startWrite is defined!
//     @param   x   x coordinate
//     @param   y   y coordinate
//    @param    color 16-bit 5-6-5 Color to fill with
// */
// /**************************************************************************/
// void Adafruit_GFX::writePixel(int16_t x, int16_t y, uint16_t color) {
//   drawPixel(x, y, color);
// }

// template for the following byte PROGMEN [512] animation
// #define FRAME_WIDTH (64)
// #define FRAME_HEIGHT (64)
// #define FRAME_COUNT (sizeof(frames) / sizeof(frames[0]))
// void setup()
// {
//     display.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDR);
// }
// int frame = 0;
// void loop()
// {
//     display.clearDisplay();
//     display.drawBitmap(32, 0, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
//     display.display();
//     frame = (frame + 1) % FRAME_COUNT;
// }
// const byte PROGMEM frames[][512] = {};

// =========================
// TEMPLATES FOR SD CARD USE
// =========================
// void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
// {
//     Serial.printf("Listing directory: %s\n", dirname);
// 
//     File root = fs.open(dirname);
//     if (!root)
//     {
//         Serial.println("Failed to open directory");
//         return;
//     }
//     if (!root.isDirectory())
//     {
//         Serial.println("Not a directory");
//         return;
//     }
// 
//     File file = root.openNextFile();
//     while (file)
//     {
//         if (file.isDirectory())
//         {
//             Serial.print("  DIR : ");
//             Serial.println(file.name());
//             if (levels)
//             {
//                 listDir(fs, file.name(), levels - 1);
//             }
//         }
//         else
//         {
//             Serial.print("  FILE: ");
//             Serial.print(file.name());
//             Serial.print("  SIZE: ");
//             Serial.println(file.size());
//         }
//         file = root.openNextFile();
//     }
// }; // end listDir function

// void createDir(fs::FS &fs, const char *path)
// {
//     Serial.printf("Creating Dir: %s\n", path);
//     if (fs.mkdir(path))
//     {
//         Serial.println("Dir created");
//     }
//     else
//     {
//         Serial.println("mkdir failed");
//     }
// }; // end createDir function

// void removeDir(fs::FS &fs, const char *path)
// {
//     Serial.printf("Removing Dir: %s\n", path);
//     if (fs.rmdir(path))
//     {
//         Serial.println("Dir removed");
//     }
//     else
//     {
//         Serial.println("rmdir failed");
//     }
// }; // end removeDir function

// void readFile(fs::FS &fs, const char *path)
// {
//     Serial.printf("Reading file: %s\n", path);
// 
//     File file = fs.open(path);
//     if (!file)
//     {
//         Serial.println("Failed to open file for reading");
//         return;
//     }
// 
//     Serial.print("Read from file: ");
//     while (file.available())
//     {
//         Serial.write(file.read());
//     }
//     file.close();
// }; // end reading File content and displaying it function

// void writeFile(fs::FS &fs, const char *path, const char *message)
// {
//     Serial.printf("Writing file: %s\n", path);
// 
//     File file = fs.open(path, FILE_WRITE);
//     if (!file)
//     {
//         Serial.println("Failed to open file for writing");
//         return;
//     }
//     if (file.print(message))
//     {
//         Serial.println("File written");
//     }
//     else
//     {
//         Serial.println("Write failed");
//     }
//     file.close();
// }; // end writing File content amd then displaying it function

// void appendFile(fs::FS &fs, const char *path, const char *message)
// {
//     Serial.printf("Appending to file: %s\n", path);
// 
//     File file = fs.open(path, FILE_APPEND);
//     if (!file)
//     {
//         Serial.println("Failed to open file for appending");
//         return;
//     }
//     if (file.print(message))
//     {
//         Serial.println("Message appended");
//     }
//     else
//     {
//         Serial.println("Append failed");
//     }
//     file.close();
// }; // end appending File content and then seeing if it saved properly function

// void renameFile(fs::FS &fs, const char *path1, const char *path2)
// {
//     Serial.printf("Renaming file %s to %s\n", path1, path2);
//     if (fs.rename(path1, path2))
//     {
//         Serial.println("File renamed");
//     }
//     else
//     {
//         Serial.println("Rename failed");
//     }
// }; // end renameFile function

// void deleteFile(fs::FS &fs, const char *path)
// {
//     Serial.printf("Deleting file: %s\n", path);
//     if (fs.remove(path))
//     {
//         Serial.println("File deleted");
//     }
//     else
//     {
//         Serial.println("Delete failed");
//     }
// }; // end deleteFile function

// shows how long it takes to read the content of a file
// void testFileIO(fs::FS &fs, const char *path)
// {
//     File file = fs.open(path);
//     static uint8_t buf[512];
//     size_t len = 0;
//     uint32_t start = millis();
//     uint32_t end = start;
//     if (file)
//     {
//         len = file.size();
//         size_t flen = len;
//         start = millis();
//         while (len)
//         {
//             size_t toRead = len;
//             if (toRead > 512)
//             {
//                 toRead = 512;
//             }
//             file.read(buf, toRead);
//             len -= toRead;
//         }
//         end = millis() - start;
//         Serial.printf("%u bytes read for %u ms\n", flen, end);
//         file.close();
//     }
//     else
//     {
//         Serial.println("Failed to open file for reading");
//     }
// 
//     file = fs.open(path, FILE_WRITE);
//     if (!file)
//     {
//         Serial.println("Failed to open file for writing");
//         return;
//     }
// 
//     size_t i;
//     start = millis();
//     for (i = 0; i < 2048; i++)
//     {
//         file.write(buf, 512);
//     }
//     end = millis() - start;
//     Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
//     file.close();
// }; // end testFileIO function

    // LOOP USED TO VIEW ALL BYTE ARRAY ANIMATIONS BEFORE WE TRANSFERED THEM TO INDIVIDUAL .h FILES
    // for (int i = 0; i < totalarrays; i++)
    // {
    //     int frame = 0;
    //     int effecttime = 30;
    // 
    //     bLED = !bLED; // toggle LED State
    //     digitalWrite(LED_BUILTIN, bLED);
    // 
    //     u8g2.clearBuffer();
    // 
    //     while (effecttime > 0)
    //     {
    //         u8g2.home();
    //         u8g2.setCursor(3, oled_LineH * 1 + 2);
    //         u8g2.print(AnimationArrays[i].name);
    //         u8g2.sendBuffer();
    // 
    //         display.drawBitmap(0, 15, AnimationArrays[i].data[frame], framewidth, frameheight, 1); // used for animation with text
    //         display.display();
    //         frame = (frame + 1) % AnimationArrays[i].frameCount;
    //         effecttime--;
    //         display.clearDisplay();
    //     }
    // }

// =========================================================
// END OF PROGRAM
// =========================================================