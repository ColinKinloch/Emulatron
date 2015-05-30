#pragma once

#include <iostream>
#include <glibmm/module.h>
#include "libretro-arb/libretro.h"

class LibRetroCore: public Glib::Module
{
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

public:
  LibRetroCore(std::string path);

  void loadSymbols();

  void init();

  retro_system_info getSystemInfo();
  retro_system_av_info getSystemAVInfo();

  void setEnvironment(retro_environment_t);
  void setVideoRefresh(retro_video_refresh_t);
  void setAudioSample(retro_audio_sample_t);
  void setAudioSampleBatch(retro_audio_sample_batch_t);
  void setInputPoll(retro_input_poll_t);
  void setInputState(retro_input_state_t);

  void setControllerPortDevice(unsigned, unsigned);

  unsigned apiVersion();

  bool loadGame(const retro_game_info* game);
};
