#ifndef __INPHIC_W1_H__
#define __INPHIC_W1_H__

#include <windows.h>
#include "../utilities/aliases.h"

#define INPHICW1_PRESET_1 (0x00)
#define INPHICW1_PRESET_2 (0x01)
#define INPHICW1_PRESET_3 (0x02)
#define INPHICW1_PRESET_4 (0x03)

#define INPHICW1_LIGHT_DPI (0x00)
#define INPHICW1_LIGHT_LOOP (0x01)
#define INPHICW1_LIGHT_ALWAYS (0x02)
#define INPHICW1_LIGHT_OFF (0x03)

const GUID DECLSPEC_SELECTANY GUID_InphicMouseW1 = {
  0x4D1E55B2,
  0xF16F,
  0x11CF,
  { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 }
};

typedef struct {
  HANDLE hDevice;
} InphicW1;

i08 inphicW1_init(InphicW1 *device);
i08 inphicW1_setDpiPreset(InphicW1 *device, u08 preset, u16 dpi);
i08 inphicW1_setLightMode(InphicW1 *device, u08 lightMode, u08 freq);

#endif
