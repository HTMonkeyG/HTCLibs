#include "inphic-mouse-w1.h"

#include <setupapi.h>
#include <hidsdi.h>
#include <hidpi.h>

#define TARGET_VENDOR 0x30FA
#define TARGET_PRODUCT 0x1701
#define USAGE_PAGE_INPHIC 0xFF01

// Get the identifier enum from given dpi number.
static u08 getDpiIdFrom(u16 dpi) {
  u8 result;

  if (dpi < 200)
    dpi = 200;
  if (dpi > 8800)
    dpi = 8800;

  if (200 <= dpi && dpi < 2000)
    result = dpi / 200;
  else if (2000 <= dpi && dpi < 2400)
    result = 0x0A;
  else if (2400 <= dpi && dpi <= 8800)
    result = dpi / 800 + 8;

  return result;
}

/**
 * Initialize the device handle.
 * @param device Pointer to the handle.
 */
i08 inphicW1_init(InphicW1 *device) {
  i08 r = 0;
  HDEVINFO deviceInfoSet;
  SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
  DWORD requiredSize = 0
    , size;
  PSP_DEVICE_INTERFACE_DETAIL_DATA_W block = NULL;
  HANDLE hDeviceTemp;
  HIDD_ATTRIBUTES attributes;
  wchar_t buffer[100];
  PHIDP_PREPARSED_DATA preparsedData;
  HIDP_CAPS caps;

  deviceInfoSet = SetupDiGetClassDevsW(
    &GUID_InphicMouseW1,
    NULL,
    NULL,
    DIGCF_DEVICEINTERFACE | DIGCF_PRESENT
  );
  deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

  for (i32 i = 0; ; i++) {
    if (!SetupDiEnumDeviceInterfaces(
      deviceInfoSet,
      NULL,
      &GUID_InphicMouseW1,
      i,
      &deviceInterfaceData
    )) {
      if (GetLastError() == ERROR_NO_MORE_ITEMS)
        break;
      continue;
    }

    SetupDiGetDeviceInterfaceDetailW(
      deviceInfoSet,
      &deviceInterfaceData,
      NULL, 0,
      &requiredSize,
      NULL
    );

    free(block);
    block = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)malloc(requiredSize);
    block->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
    SetupDiGetDeviceInterfaceDetailW(
      deviceInfoSet,
      &deviceInterfaceData,
      block, requiredSize,
      &size,
      NULL
    );

    hDeviceTemp = CreateFileW(
      block->DevicePath,
      0,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      0,
      NULL
    );
    attributes.Size = sizeof(HIDD_ATTRIBUTES);
    HidD_GetAttributes(hDeviceTemp, &attributes);

    if (
      attributes.ProductID != TARGET_PRODUCT
      || attributes.VendorID != TARGET_VENDOR
    ) 
      continue;
    if (!HidD_GetPreparsedData(hDeviceTemp, &preparsedData))
      continue;

    HidP_GetCaps(preparsedData, &caps);
    if (
      caps.Usage != HID_USAGE_GENERIC_POINTER
      || caps.UsagePage != USAGE_PAGE_INPHIC
    )
      continue;

    device->hDevice = CreateFileW(
      block->DevicePath,
      GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      0,
      NULL
    );
    r = 1;
    HidD_FreePreparsedData(preparsedData);
    break;
  }

  free(block);
  SetupDiDestroyDeviceInfoList(deviceInfoSet);

  return r;
}

/**
 * Set the DPI of specified preset, and use this preset.
 * @param device Pointer to the handle.
 * @param preset The preset number.
 * @param dpi DPI value. Must in 200~8800.
 */
i08 inphicW1_setDpiPreset(InphicW1 *device, u08 preset, u16 dpi) {
  u08 data[8]
    , dpiId;

  preset &= 0x03;
  dpiId = getDpiIdFrom(dpi);

  memset(data, 0, 8);

  data[0] = 0x07;
  data[1] = 0x09;
  data[4] = 0x0F;

  // The `preset` here indicates the preset to be used.
  data[2] = preset | ((dpiId & 0xF0) << 2);
  // The `preset` here indicates the preset to be modified.
  data[3] = (preset + 8) | ((dpiId << 4) & 0xF0);

  return HidD_SetFeature(device->hDevice, data, 8);
}

/**
 * Set light mode.
 * @param device Pointer to the handle.
 * @param lightMode Light mode.
 * @param freq Blinking frequency of the light. Must be 0~5.
 */
i08 inphicW1_setLightMode(InphicW1 *device, u08 lightMode, u08 freq) {
  u08 data[8] = {
    0x07, 0x13, 0x7F, 0x00, 0x00, 0x00, 0x20, 0x07
  };

  lightMode &= 0x03;
  freq = freq > 0x05 ? 0x05 : freq;

  if (lightMode == INPHICW1_LIGHT_OFF)
    data[3] = 0x07;
  else if (lightMode == INPHICW1_LIGHT_ALWAYS)
    data[3] = 0x26;
  else
    data[3] = (lightMode << 8) | (freq & 0x0F);

  return HidD_SetFeature(device->hDevice, data, 8);
}
