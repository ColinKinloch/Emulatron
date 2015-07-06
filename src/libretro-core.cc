
#include "libretro-core.hh"
#include <gtkmm.h>
#include <functional>

#undef SYM
#define SYM(x) do { \
  get_symbol(#x, (void *&)p##x); \
  if(p##x == nullptr) {std::cerr<<"Failed to load symbol: "<<#x<<std::endl;} \
  else{std::cout<<"Loaded symbol: "<<#x<<std::endl;} \
} while(0)


LibRetroCore* core;

LibRetroCore::LibRetroCore(std::string p):
  Glib::Module(p, Glib::MODULE_BIND_LOCAL|Glib::MODULE_BIND_LAZY)
{
  file = Gio::File::create_for_path(p);
  retro_system_info info = getSystemInfo();
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
static bool env(unsigned cmd, void *data)
{
  return core->on_environment(cmd, data);
}
static void vr(const void *data, unsigned width, unsigned height, size_t pitch){
  core->on_video_refresh(data, width, height, pitch);
}
static void as(int16_t left, int16_t right)
{
  core->on_audio_sample(left, right);
}
size_t asb(const int16_t *data, size_t frames)
{
  return core->on_audio_sample_batch(data, frames);
}
void ip()
{
  core->on_input_poll();
}
int16_t is(unsigned port, unsigned device, unsigned index, unsigned id)
{
  return core->on_input_state(port, device, index, id);
}
void LibRetroCore::init()
{
  core = this;
  pretro_set_environment(&env);
  pretro_set_video_refresh(&vr);
  pretro_set_audio_sample(&as);
  pretro_set_audio_sample_batch(&asb);
  pretro_set_input_poll(ip);
  pretro_set_input_state(is);
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

bool LibRetroCore::on_environment(unsigned cmd, void *data)
{
  return m_signal_environment.emit(cmd, data);
}
void LibRetroCore::on_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
{
  m_signal_video_refresh.emit(data, width, height, pitch);
}
void LibRetroCore::on_audio_sample(int16_t left, int16_t right)
{
  m_signal_audio_sample.emit(left, right);
}
size_t LibRetroCore::on_audio_sample_batch(const int16_t *data, size_t frames)
{
  return m_signal_audio_sample_batch.emit(data, frames);
}
void LibRetroCore::on_input_poll()
{
  return m_signal_input_poll();
}
int16_t LibRetroCore::on_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
  return m_signal_input_state(port, device, index, id);
}

LibRetroCore::type_signal_environment LibRetroCore::signal_environment()
{
  return m_signal_environment;
}
LibRetroCore::type_signal_video_refresh LibRetroCore::signal_video_refresh()
{
  return m_signal_video_refresh;
}
LibRetroCore::type_signal_audio_sample LibRetroCore::signal_audio_sample()
{
  return m_signal_audio_sample;
}
LibRetroCore::type_signal_audio_sample_batch LibRetroCore::signal_audio_sample_batch()
{
  return m_signal_audio_sample_batch;
}
LibRetroCore::type_signal_input_poll LibRetroCore::signal_input_poll()
{
  return m_signal_input_poll;
}
LibRetroCore::type_signal_input_state LibRetroCore::signal_input_state()
{
  return m_signal_input_state;
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
void LibRetroCore::unloadGame()
{
  pretro_unload_game();
}
