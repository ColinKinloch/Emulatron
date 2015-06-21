
#include "libretro-core.hh"
#include <gtkmm.h>
#include <functional>

#undef SYM
#define SYM(x) do { \
  get_symbol(#x, (void *&)p##x); \
  if(p##x == nullptr) {std::cerr<<"Failed to load symbol: "<<#x<<std::endl;} \
  else{std::cout<<"Loaded symbol: "<<#x<<std::endl;} \
} while(0)


LibRetroCore::LibRetroCore(std::string p):
  Glib::Module(p, Glib::MODULE_BIND_LOCAL|Glib::MODULE_BIND_LAZY)
{
  path = p;
  retro_system_info info = getSystemInfo();
  path = p;
  name = info.library_name;
  version = info.library_version;
  extensions = info.valid_extensions;
  std::cout<<"libRetro v"<<apiVersion()<<
  " Core: "<<name<<" "<<version<<" "<<extensions<<std::endl;
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
void LibRetroCore::deinit()
{
  pretro_deinit();
}

unsigned LibRetroCore::apiVersion()
{
  unsigned (*func)() = nullptr;
  get_symbol("retro_api_version", (void *&)func);
  return func();
}

retro_system_info LibRetroCore::getSystemInfo()
{
  void (*func)(retro_system_info*) = nullptr;
  get_symbol("retro_get_system_info", (void *&)func);
  retro_system_info info;
  func(&info);
  return info;
}
retro_system_av_info LibRetroCore::getSystemAVInfo()
{
  if(pretro_get_system_av_info == nullptr) {
    void (*func)(retro_system_av_info*) = nullptr;
    get_symbol("retro_get_system_av_info", (void *&)func);
    retro_system_av_info info;
    func(&info);
    return info;
  }
  else {
    retro_system_av_info info;
    pretro_get_system_av_info(&info);
    return info;
  }
}

void LibRetroCore::setEnvironment(retro_environment_t cb)
{
  pretro_set_environment(cb);
}
void LibRetroCore::setVideoRefresh(retro_video_refresh_t cb)
{
  pretro_set_video_refresh(cb);
}
void LibRetroCore::setAudioSample(retro_audio_sample_t cb)
{
  pretro_set_audio_sample(cb);
}
void LibRetroCore::setAudioSampleBatch(retro_audio_sample_batch_t cb)
{
  pretro_set_audio_sample_batch(cb);
}
void LibRetroCore::setInputPoll(retro_input_poll_t cb)
{
  pretro_set_input_poll(cb);
}
void LibRetroCore::setInputState(retro_input_state_t cb)
{
  pretro_set_input_state(cb);
}

void LibRetroCore::setControllerPortDevice(unsigned port, unsigned device)
{
  pretro_set_controller_port_device(port, device);
}

void LibRetroCore::reset()
{
  pretro_reset();
}
void LibRetroCore::run()
{
  pretro_run();
}

bool LibRetroCore::loadGame(const retro_game_info* game)
{
  return pretro_load_game(game);
}
bool LibRetroCore::loadGame(Glib::RefPtr<Gio::File> file)
{
  char* contents;
  size_t size;
  retro_game_info game;
  game.path = file->get_path().c_str();
  try {
    file->load_contents(contents, size);
  }
  catch(Gio::Error e)
  {
    std::cerr<<"Loading game: "<<file->get_basename()<<e.what()<<std::endl;
  }
  game.data = contents;
  game.size = size;
  std::cout<<"Ready"<<std::endl;
  return loadGame(&game);
}
