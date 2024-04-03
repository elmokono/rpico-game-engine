#include "Adafruit_GFX.h"

class GFXcanvas16Opt : public GFXcanvas16 {
public:
  GFXcanvas16Opt(uint16_t w, uint16_t h);
  ~GFXcanvas16Opt(void);
  void fillBitmap(const uint16_t bitmap[]);
  void fillBitmap(const uint16_t bitmap[], uint16_t keyColor);
  void drawRGBBitmap(uint16_t x, uint16_t y, const uint16_t bitmap[], uint16_t w, uint16_t h, uint16_t keyColor);
  void drawRGBBitmap(uint16_t x, uint16_t y, const uint16_t bitmap[], uint16_t w, uint16_t h);
  void drawRGBBitmap(uint16_t x, uint16_t y, const uint16_t bitmap[], uint16_t bitmapWidth, uint16_t offset_x, uint16_t offset_y, uint16_t w, uint16_t h, uint16_t keyColor);
  void print(uint16_t x, uint16_t y, char *text, uint16_t keyColor);
};
