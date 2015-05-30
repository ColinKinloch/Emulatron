#pragma once

#include <glibmm/module.h>
#include "libretro-arb/libretro.h"

class LibRetroCore: public Glib::Module
{
public:
  LibRetroCore(std::string path);

  retro_system_info getSystemInfo();
};
