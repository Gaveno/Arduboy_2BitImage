#include "Arduboy.h"


#ifndef _ArduboyExt.h_
#define _ArduboyExt.h_

#define B2_BLACK 0
#define B2_GRAY 1
#define B2_WHITE 2
#define B2_BLANK 3


class ArduboyExt : public Arduboy
{
public:
    ArduboyExt() : Arduboy() { gray = 0; };

    void byteToBuffer(int bytePos, uint8_t byteSet)
    {
      this->sBuffer[bytePos % (WIDTH * HEIGHT)] = byteSet;
    }
    void draw2Bitmap( int x, int y, const uint8_t *sprite);
    void clearDisplayWhite();
    void cycleGray() { gray = (++gray) % 2; };

    uint8_t gray;
};

#endif