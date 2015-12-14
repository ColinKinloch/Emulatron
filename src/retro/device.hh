#pragma once

#include "../libretro-arb/libretro.h"

namespace Retro
{
  enum DeviceType
  {
    NONE=RETRO_DEVICE_NONE,
    JOYPAD=RETRO_DEVICE_JOYPAD,
    MOUSE=RETRO_DEVICE_MOUSE,
    KEYBOARD=RETRO_DEVICE_KEYBOARD,
    LIGHTGUN=RETRO_DEVICE_LIGHTGUN,
    ANALOG=RETRO_DEVICE_ANALOG,
    POINTER=RETRO_DEVICE_POINTER
  };
  enum DeviceID
  {
    JOYPAD_B=RETRO_DEVICE_ID_JOYPAD_B,
    JOYPAD_Y=RETRO_DEVICE_ID_JOYPAD_Y,
    JOYPAD_SELECT=RETRO_DEVICE_ID_JOYPAD_SELECT,
    JOYPAD_START=RETRO_DEVICE_ID_JOYPAD_START,
    JOYPAD_UP=RETRO_DEVICE_ID_JOYPAD_UP,
    JOYPAD_DOWN=RETRO_DEVICE_ID_JOYPAD_DOWN,
    JOYPAD_LEFT=RETRO_DEVICE_ID_JOYPAD_LEFT,
    JOYPAD_RIGHT=RETRO_DEVICE_ID_JOYPAD_RIGHT,
    JOYPAD_A=RETRO_DEVICE_ID_JOYPAD_A,
    JOYPAD_X=RETRO_DEVICE_ID_JOYPAD_X,
    JOYPAD_L=RETRO_DEVICE_ID_JOYPAD_L,
    JOYPAD_R=RETRO_DEVICE_ID_JOYPAD_R,
    JOYPAD_L2=RETRO_DEVICE_ID_JOYPAD_L2,
    JOYPAD_R2=RETRO_DEVICE_ID_JOYPAD_R2,
    JOYPAD_L3=RETRO_DEVICE_ID_JOYPAD_L3,
    JOYPAD_R3=RETRO_DEVICE_ID_JOYPAD_R3,

    ANALOG_X=RETRO_DEVICE_ID_ANALOG_X,
    ANALOG_Y=RETRO_DEVICE_ID_ANALOG_Y,

    MOUSE_X=RETRO_DEVICE_ID_MOUSE_X,
    MOUSE_Y=RETRO_DEVICE_ID_MOUSE_Y,
    MOUSE_LEFT=RETRO_DEVICE_ID_MOUSE_LEFT,
    MOUSE_RIGHT=RETRO_DEVICE_ID_MOUSE_RIGHT,
    MOUSE_WHEELUP=RETRO_DEVICE_ID_MOUSE_WHEELUP,
    MOUSE_WHEELDOWN=RETRO_DEVICE_ID_MOUSE_WHEELDOWN,
    MOUSE_MIDDLE=RETRO_DEVICE_ID_MOUSE_MIDDLE,
    MOUSE_HORIZ_WHEELUP=RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELUP,
    MOUSE_HORIZ_WHEELDOWN=RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELDOWN,

    LIGHTGUN_X=RETRO_DEVICE_ID_LIGHTGUN_X,
    LIGHTGUN_Y=RETRO_DEVICE_ID_LIGHTGUN_Y,
    LIGHTGUN_TRIGGER=RETRO_DEVICE_ID_LIGHTGUN_TRIGGER,
    LIGHTGUN_CURSOR=RETRO_DEVICE_ID_LIGHTGUN_CURSOR,
    LIGHTGUN_TURBO=RETRO_DEVICE_ID_LIGHTGUN_TURBO,
    LIGHTGUN_PAUSE=RETRO_DEVICE_ID_LIGHTGUN_PAUSE,
    LIGHTGUN_START=RETRO_DEVICE_ID_LIGHTGUN_START,

    POINTER_X=RETRO_DEVICE_ID_POINTER_X,
    POINTER_Y=RETRO_DEVICE_ID_POINTER_Y,
    POINTER_PRESSED=RETRO_DEVICE_ID_POINTER_PRESSED,
  };
  enum DeviceIndex
  {
    ANALOG_LEFT=RETRO_DEVICE_INDEX_ANALOG_LEFT,
    ANALOG_RIGHT=RETRO_DEVICE_INDEX_ANALOG_RIGHT
  };
  class Device
  {
  public:
    Device();

    int16_t getState(unsigned index, unsigned id);
  };
};