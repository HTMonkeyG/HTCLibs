#include <stdio.h>
#include <math.h>

#include "../devices/colorful-keyboard-led.h"

ColorfulKeyboardLED kbled;

void HSLtoRGB(float h, float s, float l, float *r, float *g, float *b) {
  float c = (1 - fabsf(2 * l - 1)) * s; // Chroma
  float h_ = fmodf(h / 60.0, 6); // Sector 0 to 5
  float x = c * (1 - fabsf(fmodf(h_, 2) - 1)); // Intermediate value of R, G, B
  float m = l - c/2; // Lightness adjustment
 
  if (h_ >= 0 && h_ < 1)
    *r = c, *g = x, *b = 0;
  else if (h_ >= 1 && h_ < 2)
    *r = x, *g = c, *b = 0;
  else if (h_ >= 2 && h_ < 3)
    *r = 0, *g = c, *b = x;
  else if (h_ >= 3 && h_ < 4)
    *r = 0, *g = x, *b = c;
  else if (h_ >= 4 && h_ < 5)
    *r = x, *g = 0, *b = c;
  else
    *r = c, *g = 0, *b = x;
 
  *r = (*r + m) * 255; // Convert to [0,255] range
  *g = (*g + m) * 255;
  *b = (*b + m) * 255;
}

int main() {
  f32 i, r, g, b;
  u32 color;

  kbled_init(&kbled);
  wprintf(L"%s\n", kbled.devicePath);

  i = 0;
  while (1) {
    HSLtoRGB(i, 1, 0.5, &r, &g, &b);
    color = (u8)g | ((u8)r << 8) | ((u8)b << 8);
    kbled_setColor(&kbled, color);
    Sleep(20);
    i += 1;
    if (i >= 360)
      i = 0;
  }

  return 0;
}