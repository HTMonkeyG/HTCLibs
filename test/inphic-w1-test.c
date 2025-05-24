#include "../devices/inphic-mouse-w1.h"

InphicW1 mouse;

int main() {
  inphicW1_init(&mouse);

  inphicW1_setDpiPreset(&mouse, INPHICW1_PRESET_2, 1000);
  inphicW1_setLightMode(&mouse, INPHICW1_LIGHT_DPI, 5);

  Sleep(3000);

  inphicW1_setDpiPreset(&mouse, INPHICW1_PRESET_3, 1000);
  inphicW1_setLightMode(&mouse, INPHICW1_LIGHT_DPI, 0);

  return 0;
}