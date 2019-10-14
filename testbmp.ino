#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <SD.h>
#include "bitmaps.h"

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define SD_CS     10

#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define TS_LEFT 889
#define TS_RT 114
#define TS_TOP 140
#define TS_BOT 889
#define YP A2
#define XM A1
#define YM 6
#define XP 7

#define BMPIMAGEOFFSET 54
#define BUFFPIXEL      50
#define PALETTEDEPTH   0

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
MCUFRIEND_kbv tft;
bool player;
int table[9], moves;

void setup() {
  initscreen();
  drawsplash();
  drawtable();
}

void loop() {
  drawid(player);
  while (showwinner(checkwin())) {
    bool played = playmove(player);
    if (played) {
      player = !player;
      drawid(player);
      moves++;
    }
  }
  initscreen();
  clearscreen();
  for (int i = 0; i <= 9; i++) {
    table[i] = 0;
  }
  moves = 0;
}

void initscreen() {
  tft.reset();
  tft.begin(0x6809);
  tft.setRotation(1);
  tft.fillScreen(WHITE);
}

void drawsplash() {
  drawBmp(0, 0, "splash");
  delay(3000);
  tft.fillScreen(WHITE);
}

void drawBmp(int x, int y, String bmpname) {
  String file;
  SD.begin(SD_CS);
  SD.open(bmpname + ".bmp");
  file = "/" + bmpname + ".bmp";
  bmpDraw((char*)file.begin(), x, y);
}

void drawtable() {
  drawBmp(40, 0, "grid");
}

void drawid(bool playerid) {
  if (playerid) {
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    tft.drawBitmap(0, 210, box, 30, 30, WHITE);
    tft.drawBitmap(0, 210, smallo, 30, 30, BLUE);
  } else {
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    tft.drawBitmap(0, 210, box, 30, 30, WHITE);
    tft.drawBitmap(0, 210, smallx, 30, 30, RED);
  }
}
void drawxo(bool type, int pos) {
  int xpos, ypos;
  switch (pos) {
    case 1:
      xpos = 40;
      ypos = 0;
      break;
    case 2:
      xpos = 120;
      ypos = 0;
      break;
    case 3:
      xpos = 200;
      ypos = 0;
      break;
    case 4:
      xpos = 40;
      ypos = 80;
      break;
    case 5:
      xpos = 120;
      ypos = 80;
      break;
    case 6:
      xpos = 200;
      ypos = 80;
      break;
    case 7:
      xpos = 40;
      ypos = 160;
      break;
    case 8:
      xpos = 120;
      ypos = 160;
      break;
    case 9:
      xpos = 200;
      ypos = 160;
      break;
  }
  switch (type) {
    case 0:
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      tft.drawBitmap(xpos, ypos, x_bitmap, 80, 80, RED);
      break;
    case 1:
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      tft.drawBitmap(xpos, ypos, o_bitmap, 80, 80, BLUE);
      break;
  }
}

bool showwinner(int result) {
  if (result != 0) {
    int playermoves, x, y, w, h;
    delay(2000);
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    tft.fillScreen(WHITE);
    //tft.setFont(&font);
    tft.setTextSize(3);
    tft.setTextColor(WHITE);
    if (result == 1)
    {
      for (int i = 0; i < 9; i++) {
        if (table[i] == 1) {
          playermoves++;
        }
      }
      tft.drawRect(60, 72, 202, 102, BLUE);
      tft.fillRect(60, 72, 202, 102, BLUE);
      tft.setCursor(100, 100);
      tft.println("O WINS");
      tft.setCursor(80, 130);
      tft.println("Moves = " + String(playermoves));
    } else if (result == 2)
    {
      for (int i = 0; i < 9; i++) {
        if (table[i] == 2) {
          playermoves++;
        }
      }
      tft.drawRect(60, 72, 202, 102, RED);
      tft.fillRect(60, 72, 202, 102, RED);
      tft.setCursor(100, 100);
      tft.print("X WINS");
      tft.setCursor(80, 130);
      tft.println("Moves = " + String(playermoves));
    } else if (result == 3) {
      //tft.getTextBounds((String)"DRAW", 110, 120, &x, &y, &w, &h);
      //tft.drawRect(x - 10, y - 10, w + 20, h + 20, GREEN);
      //tft.fillRect(x - 10, y - 10, w + 20, h + 20, GREEN);
      tft.setCursor(110, 120);
      tft.print("DRAW");
    }
    delay(5000);
    return false;
  } else {
    return true;
  }
}

void clearscreen() {
  tft.fillScreen(WHITE);
  drawtable();
}

bool playmove(bool playerid) {
  int pos, x, y;
  pinMode(XM, INPUT);
  pinMode(YP, INPUT);
  TSPoint p = ts.getPoint();
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p.y = map(p.y, TS_TOP, TS_BOT, 0, tft.width());
    p.x = map(p.x, TS_RT, TS_LEFT, 0, tft.height());
    x = p.y;
    y = p.x;
    if (x > 40 && x < 120 && y > 0 && y < 80) {
      pos = 1;
    } else if (x > 120 && x < 200 && y > 0 && y < 80) {
      pos = 2;
    } else if (x > 200 && x < 280 && y > 0 && y < 80) {
      pos = 3;
    } else if (x > 40 && x < 120 && y > 80 && y < 160) {
      pos = 4;
    } else if (x > 120 && x < 200 && y > 80 && y < 160) {
      pos = 5;
    } else if (x > 200 && x < 280 && y > 80 && y < 160) {
      pos = 6;
    } else if (x > 40 && x < 120 && y > 160 && y < 240) {
      pos = 7;
    } else if (x > 120 && x < 200 && y > 160 && y < 240) {
      pos = 8;
    } else if (x > 200 && x < 280 && y > 160 && y < 240) {
      pos = 9;
    }
    if (table[pos - 1] == 0) {
      drawxo(playerid, pos);
      delay(200);
      if (playerid) {
        table[pos - 1] = 1;
      } else {
        table[pos - 1] = 2;
      }
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

int checkwin() {
  if (moves != 0) {
    if (table[0] == 1 && table[1] == 1 && table[2] == 1) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(40, 39 + i, 280, 39 + i, MAGENTA);
      }
      return 1;
    }
    if (table[3] == 1 && table[4] == 1 && table[5] == 1) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(40, 119 + i, 280, 119 + i, MAGENTA);
      }
      return 1;
    }
    if (table[6] == 1 && table[7] == 1 && table[8] == 1) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(40, 199 + i, 280, 199 + i, MAGENTA);
      }
      return 1;
    }
    if (table[0] == 1 && table[3] == 1 && table[6] == 1) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(79 + i, 0, 79 + i, 240, MAGENTA);
      }
      return 1;
    }
    if (table[1] == 1 && table[4] == 1 && table[7] == 1) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(159 + i, 0, 159 + i, 240, MAGENTA);
      }
      return 1;
    }
    if (table[2] == 1 && table[5] == 1 && table[8] == 1) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(279 + i, 0, 279 + i, 240, MAGENTA);
      }
      return 1;
    }
    if (table[0] == 1 && table[4] == 1 && table[8] == 1) {
      return 1;
    }
    if (table[2] == 1 && table[4] == 1 && table[6] == 1) {
      return 1;
    }


    if (table[0] == 2 && table[1] == 2 && table[2] == 2) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(40, 39 + i, 280, 39 + i, MAGENTA);
      }
      return 2;
    }
    if (table[3] == 2 && table[4] == 2 && table[5] == 2) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(40, 119 + i, 280, 119 + i, MAGENTA);
      }
      return 2;
    }
    if (table[6] == 2 && table[7] == 2 && table[8] == 2) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(40, 199 + i, 280, 199 + i, MAGENTA);
      }
      return 2;
    }
    if (table[0] == 2 && table[3] == 2 && table[6] == 2) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(79 + i, 0, 79 + i, 240, MAGENTA);
      }
      return 2;
    }
    if (table[1] == 2 && table[4] == 2 && table[7] == 2) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(159 + i, 0, 159 + i, 240, MAGENTA);
      }
      return 2;
    }
    if (table[2] == 2 && table[5] == 2 && table[8] == 2) {
      for (int i = 0; i < 4; i++) {
        tft.drawLine(279 + i, 0, 279 + i, 240, MAGENTA);
      }
      return 2;
    }
    if (table[0] == 2 && table[4] == 2 && table[8] == 2) {
      return 2;
    }
    if (table[2] == 2 && table[4] == 2 && table[6] == 2) {
      return 2;
    } else if (moves == 9) {
      return 3;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
  return 0;
}


uint8_t bmpDraw(char *nm, int x, int y) {
  File bmpFile;
  int bmpWidth, bmpHeight;    // W+H in pixels
  uint8_t bmpDepth;           // Bit depth (currently must be 24, 16, 8, 4, 1)
  uint32_t bmpImageoffset;    // Start of image data in file
  uint32_t rowSize;           // Not always = bmpWidth; may have padding
  uint8_t sdbuffer[3 * BUFFPIXEL];    // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[(1 << PALETTEDEPTH) + BUFFPIXEL], *palette = NULL;
  uint8_t bitmask, bitshift;
  boolean flip = true;        // BMP is stored bottom-to-top
  int w, h, row, col, lcdbufsiz = (1 << PALETTEDEPTH) + BUFFPIXEL, buffidx;
  uint32_t pos;               // seek position
  boolean is565 = false;      //

  uint16_t bmpID;
  uint16_t n;                 // blocks read
  uint8_t ret;

  if ((x >= tft.width()) || (y >= tft.height()))
    return 1;               // off screen

  bmpFile = SD.open(nm);      // Parse BMP header
  bmpID = read16(bmpFile);    // BMP signature
  (void) read32(bmpFile);     // Read & ignore file size
  (void) read32(bmpFile);     // Read & ignore creator bytes
  bmpImageoffset = read32(bmpFile);       // Start of image data
  (void) read32(bmpFile);     // Read & ignore DIB header size
  bmpWidth = read32(bmpFile);
  bmpHeight = read32(bmpFile);
  n = read16(bmpFile);        // # planes -- must be '1'
  bmpDepth = read16(bmpFile); // bits per pixel
  pos = read32(bmpFile);      // format
  if (bmpID != 0x4D42) ret = 2; // bad ID
  else if (n != 1) ret = 3;   // too many planes
  else if (pos != 0 && pos != 3) ret = 4; // format: 0 = uncompressed, 3 = 565
  else if (bmpDepth < 16 && bmpDepth > PALETTEDEPTH) ret = 5; // palette
  else {
    bool first = true;
    is565 = (pos == 3);               // ?already in 16-bit format
    // BMP rows are padded (if needed) to 4-byte boundary
    rowSize = (bmpWidth * bmpDepth / 8 + 3) & ~3;
    if (bmpHeight < 0) {              // If negative, image is in top-down order.
      bmpHeight = -bmpHeight;
      flip = false;
    }

    w = bmpWidth;
    h = bmpHeight;
    if ((x + w) >= tft.width())       // Crop area to be loaded
      w = tft.width() - x;
    if ((y + h) >= tft.height())      //
      h = tft.height() - y;

    if (bmpDepth <= PALETTEDEPTH) {   // these modes have separate palette
      bmpFile.seek(BMPIMAGEOFFSET); //palette is always @ 54
      bitmask = 0xFF;
      if (bmpDepth < 8)
        bitmask >>= bmpDepth;
      bitshift = 8 - bmpDepth;
      n = 1 << bmpDepth;
      lcdbufsiz -= n;
      palette = lcdbuffer + lcdbufsiz;
      for (col = 0; col < n; col++) {
        pos = read32(bmpFile);    //map palette to 5-6-5
        palette[col] = ((pos & 0x0000F8) >> 3) | ((pos & 0x00FC00) >> 5) | ((pos & 0xF80000) >> 8);
      }
    }

    // Set TFT address window to clipped image bounds
    tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
    for (row = 0; row < h; row++) { // For each scanline...
      // Seek to start of scan line.  It might seem labor-
      // intensive to be doing this on every line, but this
      // method covers a lot of gritty details like cropping
      // and scanline padding.  Also, the seek only takes
      // place if the file position actually needs to change
      // (avoids a lot of cluster math in SD library).
      uint8_t r, g, b, *sdptr;
      int lcdidx, lcdleft;
      if (flip)   // Bitmap is stored bottom-to-top order (normal BMP)
        pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
      else        // Bitmap is stored top-to-bottom
        pos = bmpImageoffset + row * rowSize;
      if (bmpFile.position() != pos) { // Need seek?
        bmpFile.seek(pos);
        buffidx = sizeof(sdbuffer); // Force buffer reload
      }

      for (col = 0; col < w; ) {  //pixels in row
        lcdleft = w - col;
        if (lcdleft > lcdbufsiz) lcdleft = lcdbufsiz;
        for (lcdidx = 0; lcdidx < lcdleft; lcdidx++) { // buffer at a time
          uint16_t color;
          // Time to read more pixel data?
          if (buffidx >= sizeof(sdbuffer)) { // Indeed
            bmpFile.read(sdbuffer, sizeof(sdbuffer));
            buffidx = 0; // Set index to beginning
            r = 0;
          }
          switch (bmpDepth) {          // Convert pixel from BMP to TFT format
            case 24:
              b = sdbuffer[buffidx++];
              g = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];
              color = tft.color565(r, g, b);
              break;
            case 16:
              b = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];
              if (is565)
                color = (r << 8) | (b);
              else
                color = (r << 9) | ((b & 0xE0) << 1) | (b & 0x1F);
              break;
            case 1:
            case 4:
            case 8:
              if (r == 0)
                b = sdbuffer[buffidx++], r = 8;
              color = palette[(b >> bitshift) & bitmask];
              r -= bmpDepth;
              b <<= bmpDepth;
              break;
          }
          lcdbuffer[lcdidx] = color;

        }
        tft.pushColors(lcdbuffer, lcdidx, first);
        first = false;
        col += lcdidx;
      }           // end cols
    }               // end rows
    tft.setAddrWindow(0, 0, tft.width() - 1, tft.height() - 1); //restore full screen
    ret = 0;        // good render
  }
  bmpFile.close();
  return (ret);
}

uint16_t read16(File& f) {
  uint16_t result;         // read little-endian
  f.read((uint8_t*)&result, sizeof(result));
  return result;
}

uint32_t read32(File& f) {
  uint32_t result;
  f.read((uint8_t*)&result, sizeof(result));
  return result;
}
