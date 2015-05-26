
#include <string>
#include "libretro-core.hh"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

dylib_t dylib_load(std::string path)
{
#ifdef _WIN32
  dylib_t lib = LoadLibrary(path.c_str());
#else
  dylib_t lib = dlopen(path.c_str(), RTLD_LAZY);
#endif
  return lib;
}
void dylib_close(dylib_t lib)
{
#ifdef _WIN32
  FreeLibrary((HMODULE)lib);
#else
#ifndef NO_DLCLOSE
  dlclose(lib);
#endif
#endif
}

LibRetroCore::LibRetroCore(std::string path)
{
  lib = dylib_load(path);
}
