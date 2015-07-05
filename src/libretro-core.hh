#pragma once

#include <iostream>
#include <giomm/file.h>
#include <gdkmm/pixbuf.h>
#include <glibmm/module.h>
#include "libretro-arb/libretro.h"

class LibRetroCore: public Glib::Module
{
public:
  LibRetroCore(std::string path);

  sigc::signal<Glib::RefPtr<Gdk::Pixbuf> > videoSignal;

  void init();
  void deinit();

  unsigned apiVersion();

  retro_system_info getSystemInfo();
  retro_system_av_info getSystemAVInfo();

  bool on_environment(unsigned cmd, void *data);
  void on_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch);
  void on_audio_sample(int16_t left, int16_t right);
  size_t on_audio_sample_batch(const int16_t *data, size_t frames);
  void on_input_poll();
  int16_t on_input_state(unsigned port, unsigned device, unsigned index, unsigned id);

  void setControllerPortDevice(unsigned, unsigned);

  void reset();
  void run();

  size_t serializeSize();
  bool serialize(void*, size_t);
  bool unserialize(const void*, size_t);

  void cheatReset();
  void cheatSet(unsigned, bool, const char*);

  void loadSymbols();

  bool loadGame(const retro_game_info*);
  bool loadGame(Glib::RefPtr<Gio::File>);
  bool loadGameSpecial(unsigned, const struct retro_game_info*, size_t);
  void unloadGame();

  unsigned getRegion();

  void* getMemoryData(unsigned);
  size_t getMemorySize(unsigned);

  typedef sigc::signal<bool, unsigned, void*> type_signal_environment;
  typedef sigc::signal<void, const void*, unsigned, unsigned, size_t> type_signal_video_refresh;
  typedef sigc::signal<void, int16_t, int16_t> type_signal_audio_sample;
  typedef sigc::signal<size_t, const int16_t*, size_t> type_signal_audio_sample_batch;
  typedef sigc::signal<void> type_signal_input_poll;
  typedef sigc::signal<int16_t, unsigned, unsigned, unsigned, unsigned> type_signal_input_state;

  type_signal_environment signal_environment();
  type_signal_video_refresh signal_video_refresh();
  type_signal_audio_sample signal_audio_sample();
  type_signal_audio_sample_batch signal_audio_sample_batch();
  type_signal_input_poll signal_input_poll();
  type_signal_input_state signal_input_state();

private:
  void (*pretro_init)(void);
  void (*pretro_deinit)(void);

  unsigned (*pretro_api_version)(void);

  void (*pretro_get_system_info)(struct retro_system_info*);
  void (*pretro_get_system_av_info)(struct retro_system_av_info*);

  void (*pretro_set_environment)(retro_environment_t);
  void (*pretro_set_video_refresh)(retro_video_refresh_t);
  void (*pretro_set_audio_sample)(retro_audio_sample_t);
  void (*pretro_set_audio_sample_batch)(retro_audio_sample_batch_t);
  void (*pretro_set_input_poll)(retro_input_poll_t);
  void (*pretro_set_input_state)(retro_input_state_t);

  void (*pretro_set_controller_port_device)(unsigned, unsigned);

  void (*pretro_reset)(void);
  void (*pretro_run)(void);

  size_t (*pretro_serialize_size)(void);
  bool (*pretro_serialize)(void*, size_t);
  bool (*pretro_unserialize)(const void*, size_t);

  void (*pretro_cheat_reset)(void);
  void (*pretro_cheat_set)(unsigned, bool, const char*);

  bool (*pretro_load_game)(const struct retro_game_info*);
  bool (*pretro_load_game_special)(unsigned,
        const struct retro_game_info*, size_t);

  void (*pretro_unload_game)(void);

  unsigned (*pretro_get_region)(void);

  void *(*pretro_get_memory_data)(unsigned);
  size_t (*pretro_get_memory_size)(unsigned);

  bool environment_cb(unsigned cmd, void *data);

  Glib::RefPtr<Gdk::Pixbuf> frame;
  retro_pixel_format pixelFormat;

protected:
  std::string path;
  std::string name;
  std::string version;
  std::string extensions;

  type_signal_environment m_signal_environment;
  type_signal_video_refresh m_signal_video_refresh;
  type_signal_audio_sample m_signal_audio_sample;
  type_signal_audio_sample_batch m_signal_audio_sample_batch;
  type_signal_input_poll m_signal_input_poll;
  type_signal_input_state m_signal_input_state;
};
