
#include "libretro-core.hh"


LibRetroCore::LibRetroCore(std::string path):
  Glib::Module(path, Glib::MODULE_BIND_LOCAL)
{

}

retro_system_info LibRetroCore::getSystemInfo()
{
  void (*func)(retro_system_info*) = nullptr;
  get_symbol("retro_get_system_info", (void *&)func);
  retro_system_info info;
  func(&info);
  return info;
}
