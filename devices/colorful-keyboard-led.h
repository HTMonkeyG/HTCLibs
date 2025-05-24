#ifndef __COLORFUL_KBLED_H__
#define __COLORFUL_KBLED_H__

#include <windows.h>
#include "../utilities/aliases.h"

#define IOCTL_SET_COLORFUL_KEYBOARD CTL_CODE(0x00000032, 0x0900, 0, 0)

typedef struct {
  HANDLE hDevice;
} ColorfulKeyboardLED;

const GUID DECLSPEC_SELECTANY GUID_ColorfulKeyboard = {
  0x86994C74,
  0xAD43,
  0x4812,
  { 0xB7, 0xE7, 0x0C, 0x42, 0x0B, 0x5C, 0x5F, 0xD7 }
};

i08 kbled_init(ColorfulKeyboardLED *kbled);
i08 kbled_setColor(ColorfulKeyboardLED *kbled, COLORREF color);
i08 kbled_setSleepTime(ColorfulKeyboardLED *kbled, u16 sleepTime);
i08 kbled_setBrightness(ColorfulKeyboardLED *kbled, u08 brightness);

#endif
