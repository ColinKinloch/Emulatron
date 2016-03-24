#include "core.hh"
#include <iostream>

#undef SYM
#define SYM(x) do { \
  get_symbol(#x, (void *&)p##x); \
  if(p##x == nullptr) {std::cerr<<"Failed to load symbol: "<<#x<<std::endl;} \
  else{std::cout<<"Loaded symbol: "<<#x<<std::endl;} \
} while(0)

Retro::Core* core;
static bool envc(unsigned cmd, void *data)
{
  return core->on_environment(cmd, data);
}
static void vrc(const void *data, unsigned width, unsigned height, size_t pitch){
  core->on_video_refresh(data, width, height, pitch);
}
static void asc(int16_t left, int16_t right)
{
  core->on_audio_sample(left, right);
}
size_t asbc(const int16_t *data, size_t frames)
{
  return core->on_audio_sample_batch(data, frames);
}
void ipc()
{
  core->on_input_poll();
}
int16_t isc(unsigned port, unsigned device, unsigned index, unsigned id)
{
  return core->on_input_state(port, device, index, id);
}

namespace Retro
{
  Core::Core(std::string p):
    Glib::Module(p, Glib::MODULE_BIND_LOCAL|Glib::MODULE_BIND_LAZY)
  {
    file = Gio::File::create_for_path(p);
    retro_system_info info = getSystemInfo();
    name = info.library_name;
    version = info.library_version;
    if(info.valid_extensions)
      extensions = info.valid_extensions;
    std::cout<<"libRetro v"<<apiVersion()<<
    " Core: "<<name<<" "<<version<<" "<<extensions<<std::endl;
  }
  void Core::loadSymbols()
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

  void Core::init()
  {
    core = this;
    pretro_set_environment(&envc);
    pretro_set_video_refresh(&vrc);
    pretro_set_audio_sample(&asc);
    pretro_set_audio_sample_batch(&asbc);
    pretro_set_input_poll(ipc);
    pretro_set_input_state(isc);
    pretro_init();
  }
  void Core::deinit()
  {
    pretro_deinit();
  }

  unsigned Core::apiVersion()
  {
    unsigned (*func)() = nullptr;
    get_symbol("retro_api_version", (void *&)func);
    return func();
  }

  retro_system_info Core::getSystemInfo()
  {
    void (*func)(retro_system_info*) = nullptr;
    get_symbol("retro_get_system_info", (void *&)func);
    retro_system_info info;
    func(&info);
    return info;
  }
  retro_system_av_info Core::getSystemAVInfo()
  {
    /*
    if(pretro_get_system_av_info == nullptr) {
      void (*func)(retro_system_av_info*) = nullptr;
      get_symbol("retro_get_system_av_info", (void *&)func);
      retro_system_av_info info;
      func(&info);
      return info;
    }
    else {
    */
    retro_system_av_info info;
    pretro_get_system_av_info(&info);
    return info;
  }

  bool Core::on_environment(unsigned cmd, void *data)
  {
    return m_signal_environment.emit(cmd, data);
  }
  void Core::on_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
  {
    m_signal_video_refresh.emit(data, width, height, pitch);
  }
  void Core::on_audio_sample(int16_t left, int16_t right)
  {
    m_signal_audio_sample.emit(left, right);
  }
  size_t Core::on_audio_sample_batch(const int16_t *data, size_t frames)
  {
    return m_signal_audio_sample_batch.emit(data, frames);
  }
  void Core::on_input_poll()
  {
    return m_signal_input_poll();
  }
  int16_t Core::on_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
  {
    return m_signal_input_state(port, device, index, id);
  }

  Core::type_signal_environment Core::signal_environment()
  {
    return m_signal_environment;
  }
  Core::type_signal_video_refresh Core::signal_video_refresh()
  {
    return m_signal_video_refresh;
  }
  Core::type_signal_audio_sample Core::signal_audio_sample()
  {
    return m_signal_audio_sample;
  }
  Core::type_signal_audio_sample_batch Core::signal_audio_sample_batch()
  {
    return m_signal_audio_sample_batch;
  }
  Core::type_signal_input_poll Core::signal_input_poll()
  {
    return m_signal_input_poll;
  }
  Core::type_signal_input_state Core::signal_input_state()
  {
    return m_signal_input_state;
  }

  void Core::setControllerPortDevice(unsigned port, unsigned device)
  {
    pretro_set_controller_port_device(port, device);
  }

  void Core::reset()
  {
    pretro_reset();
  }
  void Core::run()
  {
    pretro_run();
  }

  bool Core::loadGame(const retro_game_info* game)
  {
    return pretro_load_game(game);
  }
  void Core::unloadGame()
  {
    pretro_unload_game();
  }
  unsigned Core::getRegion() {
    return pretro_get_region();
  }
};
