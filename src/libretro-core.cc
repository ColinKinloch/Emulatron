
#include "libretro-core.hh"

#undef SYM
#define SYM(x) do { \
  get_symbol(#x, (void *&)p##x); \
  if(p##x == nullptr) {std::cerr<<"Failed to load symbol: "<<#x<<std::endl;} \
  else{std::cout<<"Loaded symbol: "<<#x<<std::endl;} \
} while(0)

bool environment_cb(unsigned cmd, void *data)
{
  return false;
}

LibRetroCore::LibRetroCore(std::string path):
  Glib::Module(path, Glib::MODULE_BIND_LOCAL)
{
  loadSymbols();
  setEnvironment(&environment_cb);
  retro_system_info info;
  pretro_get_system_info(&info);
  std::cout<<"Retro Core: "<<info.library_name<<" "<<info.library_version<<std::endl;
}

void LibRetroCore::loadSymbols()
{
  SYM(retro_init);
  SYM(retro_deinit);

  SYM(retro_api_version);
  SYM(retro_get_system_info);
  SYM(retro_get_system_av_info);

  SYM(retro_set_environment);
  SYM(retro_set_video_refresh);
  SYM(retro_set_audio_sample);
  SYM(retro_set_audio_sample_batch);
  SYM(retro_set_input_poll);
  SYM(retro_set_input_state);

  SYM(retro_set_controller_port_device);

  SYM(retro_reset);
  SYM(retro_run);

  SYM(retro_serialize_size);
  SYM(retro_serialize);
  SYM(retro_unserialize);

  SYM(retro_cheat_reset);
  SYM(retro_cheat_set);

  SYM(retro_load_game);
  SYM(retro_load_game_special);

  SYM(retro_unload_game);
  SYM(retro_get_region);
  SYM(retro_get_memory_data);
  SYM(retro_get_memory_size);
}

void LibRetroCore::init()
{
  pretro_init();
}

unsigned LibRetroCore::apiVersion()
{
  if(pretro_api_version == nullptr) {
    unsigned (*func)() = nullptr;
    get_symbol("retro_api_version", (void *&)func);
    return func();
  }
  else {
    return pretro_api_version();
  }
}

retro_system_info LibRetroCore::getSystemInfo()
{
  if(pretro_get_system_info == nullptr) {
    void (*func)(retro_system_info*) = nullptr;
    get_symbol("retro_get_system_info", (void *&)func);
    retro_system_info info;
    func(&info);
    return info;
  }
  else {
    retro_system_info info;
    pretro_get_system_info(&info);
    return info;
  }
}

void LibRetroCore::setEnvironment(retro_environment_t cb)
{
  pretro_set_environment(cb);
}

bool LibRetroCore::loadGame(const retro_game_info* game)
{
  return pretro_load_game(game);
}
