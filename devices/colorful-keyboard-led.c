#include <windows.h>
#include <cfgmgr32.h>
#include <stdlib.h>

#include "../utilities/aliases.h"
#include "colorful-keyboard-led.h"

typedef struct {
  char c1[16];
  int c2;
  int c3;
  int opCode;
  short c4;
  short c5;
  char buffer[1024];
} KeyboardData;

static const u08 xmmword[16] = {
  0xE4, 0x24, 0xF2, 0x93, 0xDC, 0xFB, 0xBF, 0x4B,
  0xAD, 0xD6, 0xDB, 0x71, 0xBD, 0xC0, 0xAF, 0xAD
};

// Get device path through GUID.
static void getDevicePath(wchar_t *path) {
  ULONG pulLen;
  wchar_t *buffer;

  while (
    CM_Get_Device_Interface_List_SizeW(
      &pulLen, (LPGUID)&GUID_ColorfulKeyboard, NULL, 0
    ) == CR_SUCCESS
  ) {
    buffer = malloc(pulLen * sizeof(wchar_t));

    if (!buffer)
      break;

    if (
      CM_Get_Device_Interface_ListW(
        (LPGUID)&GUID_ColorfulKeyboard, NULL, buffer, pulLen, 0
      ) == CR_SUCCESS
    ) {
      wcscpy(path, buffer);
      free(buffer);
      break;
    }

    free(buffer);
  }
}

// Send data to hardware driver.
static i08 sendData(ColorfulKeyboardLED *kbled, i32 opCode, u08 *data, i32 size) {
  KeyboardData dataSent;
  char *lpOutBuffer;
  DWORD BytesReturned[4];
  i08 result = 1;

  if (!data || !kbled)
    return 0;

  dataSent.c3 = 1297302623;
  dataSent.opCode = opCode;
  memcpy(dataSent.c1, xmmword, 16);
  dataSent.c2 = 0;
  dataSent.c4 = 260;
  memset(&dataSent.c5, 0, 0x400);
  if (size > 0)
    memcpy(dataSent.buffer + 2, data, size);
  dataSent.c5 = 0x0002;
  dataSent.buffer[0] = 0x00;
  dataSent.buffer[1] = 0x01;

  lpOutBuffer = malloc(0x40C);
  if (!DeviceIoControl(
    kbled->hDevice,
    IOCTL_SET_COLORFUL_KEYBOARD,
    &dataSent, 0x420u,
    lpOutBuffer, 0x40Cu,
    BytesReturned,
    NULL
  ))
    result = 0;

  free(lpOutBuffer);

  return result;
}

/**
 * Initialize keyboard led handle.
 * @param kbled Pointer to handle struct.
 * @returns 0 if failed.
 */
i08 kbled_init(ColorfulKeyboardLED *kbled) {
  if (!kbled)
    return 0;

  getDevicePath(kbled->devicePath);
  kbled->hDevice = CreateFileW(
    kbled->devicePath,
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  if (kbled->hDevice == INVALID_HANDLE_VALUE)
    return 0;
  return 1;
}

/**
 * Set keyboard color.
 * @param kbled Pointer to handle struct.
 * @param color 8 bit color data in GRB format.
 * @returns 0 if failed.
 */
i08 kbled_setColor(ColorfulKeyboardLED *kbled, COLORREF color) {
  unsigned int result = 0
    , data;

  data = 0xF0000000 | (color & 0xFFFFFF);
  result &= sendData(kbled, 0x67, (char *)&data, 4);
  //data = 0xF1000000 | packed;
  //result &= sendData(103, &data, 4);
  //data = 0xF2000000 | packed;
  //result &= sendData(103, &data, 4);

  return result;
}

/**
 * Set keyboard led sleep time.
 * @param kbled Pointer to handle struct.
 * @param color Sleep time in seconds. Disable keyboard sleep by set to 0.
 * @returns 0 if failed.
 */
i08 kbled_setSleepTime(ColorfulKeyboardLED *kbled, u16 sleepTime) {
  u32 data = 0x180000FF | (sleepTime << 8);
  return sendData(kbled, 0x79, (char *)&data, 4);
}

/**
 * Set keyboard led brightness.
 * @param kbled Pointer to handle struct.
 * @param color Brightness.
 * @returns 0 if failed.
 */
i08 kbled_setBrightness(ColorfulKeyboardLED *kbled, u08 brightness) {
  u32 data = 0xF4000000 | brightness;
  return sendData(kbled, 0x67, (char *)&data, 4);
}
