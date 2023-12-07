#ifndef ANIMATION_H
#define ANIMATION_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

static const uint8_t framewidth = 48;
static const uint8_t frameheight = 48;
static const uint8_t framecount = 28;

static uint8_t oled_LineH = 0;

struct Frame
{
    const char *fileName;
    uint8_t frameCounts;
    const String name;
};

void loadAnimation(const char *fileName, uint8_t **data, uint8_t *dataSize);

#endif // ANIMATION_H