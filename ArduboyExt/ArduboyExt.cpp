#include "ArduboyExt.h"

void ArduboyExt::clearDisplayWhite()
{
  // C version:
  for (int a = 0; a < (HEIGHT*WIDTH)/8; a++) sBuffer[a] = 0xFF;
}

/*-----------------------------------
 *    draw2Bitmap
 *    
 *    Draws an image stored as
 *    2-bits per pixel
 *    
 *    00: Black
 *    01: Gray
 *    10: White
 *    11: Transparent
 *    
 *    Byte0:
 *    Width
 *    Byte1:
 *    Height
 *    
 *    Byte 2+:
 *    B00 00 00 00
 *     P3 P2 P1 P0
 *    
 ------------------------------------*/
void ArduboyExt :: draw2Bitmap(int x, int y, const uint8_t *sprite)
{
  //if (x+w < 0 || x > WIDTH-1 || y+h < 0 || y > HEIGHT-1)
    //return;
  //Serial.print("--Drawing Sprite--\n");
  byte cByte[2];   //current byte read int
  byte wByte_W = 0x00;   //white byte to write into the buffer , all zereos initially
  byte wByte_B = 0xFF;  //black byte to write into the buffer , all one's initially
  
  uint8_t w = pgm_read_byte(sprite);
  uint8_t h = pgm_read_byte(sprite+1);

  if (x + w < 0 || x >= WIDTH
    || y + h < 0 || y >= HEIGHT)
      return;
  
  unsigned int bytes = (w * h) / 4;
  //Serial.print("Bytes:");
  //Serial.print(bytes);
  //Serial.print("\n");
  
  uint8_t xx = 0;
  uint8_t row = 0;
  for (unsigned int b = 2; b <= bytes; b += 2)
  {                                                 //1
    // Current byte working on
    cByte[0] = pgm_read_byte_near(sprite + b);
    cByte[1] = pgm_read_byte_near(sprite + b + 1);
    wByte_W = 0x00;
    wByte_B = 0xFF;

    for (byte i = 0; i < 2; i++)
    {                                                //2
      
      /*uint8_t pixel[] = { 0x00, 0x00, 0x00, 0x00 };     // This chunk not working yet!
      for (byte p = 0; p < 4; p++)
      {                                               //3
        // Pixel in byte
        pixel[p] = (cByte[i] & (0x03 << (i * 2))) >> (i * 2);
        if (pixel[p]!= B2_BLANK)
        {                                             //4
          if (pixel[p] == B2_GRAY)
          {                                           //5
              if (gray == 0)
                  pixel[p] = B2_BLACK;
              else
                  pixel[p] = B2_WHITE;
          }                                           //5
        
          if (pixel[p] == B2_WHITE)
            wByte_W |= _BV((i * 4) + 0);
          else
            wByte_B &= ~_BV((i * 4) + 0);
        }                                              //4
      }                                                //3
    }*/
    
    // first pixel in byte
    uint8_t pixel = cByte[i] & 0x03;
    if (pixel != B2_BLANK)
    {
      if (pixel == B2_GRAY)
      {
          if (gray == 0)
              pixel = B2_BLACK;
          else
              pixel = B2_WHITE;
      }
        
      if (pixel == B2_WHITE)
        wByte_W |= _BV((i * 4) + 0);
      else
        wByte_B &= ~_BV((i * 4) + 0);
    }
    
    // second pixel in byte
    pixel = (cByte[i] & 0x0C) >> 2;
    if (pixel != B2_BLANK)
    {
      if (pixel == B2_GRAY)
      {
          if (gray == 0)
              pixel = B2_BLACK;
          else
              pixel = B2_WHITE;
      }
        
      if (pixel == B2_WHITE)
        wByte_W |= _BV((i * 4) + 1);
      else
        wByte_B &= ~_BV((i * 4) + 1);
    }
    
    // third pixel in byte
    pixel = (cByte[i] & 0x30) >> 4;
    if (pixel != B2_BLANK)
    {
      if (pixel == B2_GRAY)
      {
          if (gray == 0)
              pixel = B2_BLACK;
          else
              pixel = B2_WHITE;
      }
        
      if (pixel == B2_WHITE)
        wByte_W |= _BV((i * 4) + 2);
      else
        wByte_B &= ~_BV((i * 4) + 2);
    }
    
    // fourth pixel in byte
    pixel = (cByte[i] & 0xC0) >> 6;
    if (pixel != B2_BLANK)
    {
      if (pixel == B2_GRAY)
      {
          if (gray == 0)
              pixel = B2_BLACK;
          else
              pixel = B2_WHITE;
      }
        
      if (pixel == B2_WHITE)
        wByte_W |= _BV((i * 4) + 3);
      else
        wByte_B &= ~_BV((i * 4) + 3);
      }
    }

    /*
     *  Combine bytes into display buffer
     */
    int8_t yoffset = abs(y) % 8;
    //if (y < 0)
      //yoffset -= 8;

    if ( (x + xx) >= 0 && (x + xx) < WIDTH && (y + (row * 8)) < HEIGHT)
    {
        //uint16_t bufferByte = max(min(x + xx + ((floor(y / 8) * 8) * WIDTH / 8) + (row * WIDTH),1023),0);
      int bufferByte = (x + xx + ((floor(y / 8) * 8) * WIDTH / 8) + (row * WIDTH));

      if (bufferByte >= 0 && bufferByte <= 1023)
      {
        if (yoffset == 0)
        { //No offset draw directly                 //6
          this->sBuffer[bufferByte] |= wByte_W;
          this->sBuffer[bufferByte] &= wByte_B;
        }                                           //6
        else
        { //Offset greater than 0                   //7
          // higher byte (lower value)  Row
            if (y >= 0)
            { 
                this->sBuffer[bufferByte] |= wByte_W << yoffset;
                this->sBuffer[bufferByte] &= ~(~wByte_B << yoffset);

                // Lower byte (higher value)  Row + 1
                if ((bufferByte + WIDTH) < 1023) {
                  this->sBuffer[bufferByte + WIDTH] |= wByte_W >> (8 - yoffset);
                  this->sBuffer[bufferByte + WIDTH] &= (wByte_B >> (8 - yoffset)) | (0xFF << yoffset);
                }
            }
            else //Partially outside screen
            {
                if ((bufferByte - WIDTH) >= 0)
                {
                  // Lower byte (higher value)  Row - 1
                  this->sBuffer[bufferByte - WIDTH] |= wByte_W << (8 - yoffset);
                  this->sBuffer[(bufferByte - WIDTH)] &= (wByte_B << (8 - yoffset)) | (0xFF >> ( yoffset));
                }
                  // Higher byte (lower value) Row
                this->sBuffer[bufferByte] |= wByte_W >> (yoffset);
                this->sBuffer[bufferByte] &= (wByte_B >> (yoffset)) | (0xFF << (8 - yoffset));
            }
         }
       }
    }

    xx++;
    if (xx >= w)
    {                                           //8
        xx = 0;
        row++;
    }                                           //8
  }                                             //2
}