#pragma once

#include "libretro-arb/libretro.h"

typedef void *dylib_t;
typedef void (*function_t)(void);

class LibRetroCore
{
public:
  LibRetroCore(std::string path);
  ~LibRetroCore();
private:
  dylib_t lib;
};
