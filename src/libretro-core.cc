
#include "libretro-core.hh"
#include <gtkmm.h>
#include <functional>

#undef SYM
#define SYM(x) do { \
  get_symbol(#x, (void *&)p##x); \
  if(p##x == nullptr) {std::cerr<<"Failed to load symbol: "<<#x<<std::endl;} \
  else{std::cout<<"Loaded symbol: "<<#x<<std::endl;} \
} while(0)
/*
bool environment_cb(unsigned cmd, void *data)
{
  std::cout<<"environment: ";
  switch(cmd) {
    case RETRO_ENVIRONMENT_SET_ROTATION:
      std::cout<<"set rotation"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_OVERSCAN:
      std::cout<<"get overscan"<<std::endl;
      return false;
    case RETRO_ENVIRONMENT_SET_MESSAGE:
      std::cout<<"set message"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SHUTDOWN:
      std::cout<<"shutdown"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
      std::cout<<"set performance level"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
      std::cout<<"get system directory"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
    {
      retro_pixel_format format = *(retro_pixel_format*)data;
      std::cout<<"set pixel format:"<<std::endl;
      switch(format) {
        case RETRO_PIXEL_FORMAT_0RGB1555:
          std::cout<<"0RGB1555";
          break;
        case RETRO_PIXEL_FORMAT_XRGB8888:
          std::cout<<"XRGB8888";
          break;
        case RETRO_PIXEL_FORMAT_RGB565:
          std::cout<<"RGB565";
          break;
        case RETRO_PIXEL_FORMAT_UNKNOWN:
          std::cout<<"Unknown";
      }
      std::cout<<std::endl;
      return true;
    }
    case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
      std::cout<<"set input descriptors"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK:
      std::cout<<"set keyboard callback"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
      std::cout<<"set disk control interface"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_HW_RENDER:
      std::cout<<"set hw render"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_VARIABLE:
    {
      retro_variable* var = (retro_variable*)data;
      std::cout<<"get variable: "<<var->key<<std::endl;
      break;
    }
    case RETRO_ENVIRONMENT_SET_VARIABLES:
    {
        retro_variable* var = (retro_variable*)data;
        std::cout<<"set variable:"<<std::endl;
      if(var->value != nullptr) {
        std::cout<<var->key<<": "<<var->value<<std::endl;
        return true;
      }
      else {
        std::cout<<"null value"<<std::endl;
      }
      break;
    }
    case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
    {
      std::cout<<"get variable update"<<std::endl;
      return false;
    }
    case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
      std::cout<<"set support no game"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
      std::cout<<"get libretro path"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK:
      std::cout<<"set audio callback"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK:
      std::cout<<"set frame time callback"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE:
      std::cout<<"get rumble interface"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES:
      std::cout<<"get input device capabilities"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE:
      std::cout<<"get sensor interface"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE:
      std::cout<<"get camera interface"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
      std::cout<<"get log interface"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_PERF_INTERFACE:
      std::cout<<"get perf interface"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE:
      std::cout<<"get location interface"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY:
      std::cout<<"get core assets directory"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
      std::cout<<"get set directory"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
      std::cout<<"set system av info"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK:
      std::cout<<"set proc address callback"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO:
    {
      retro_subsystem_info* info = (retro_subsystem_info*)data;
      std::cout<<"set subsystem info:"<<std::endl;
      std::cout<<info->ident<<" ("<<info->desc<<")"<<std::endl;
      for(int i=0; i<info->num_roms;i++) {
        retro_subsystem_rom_info rominfo = info->roms[i];
        std::cout<<rominfo.desc<<std::endl;
      }
      break;
    }
    case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
    {
      retro_controller_info* info = (retro_controller_info*)data;
      std::cout<<"set controller info:"<<std::endl;
      for(int i=0; i < info->num_types; i++) {
        retro_controller_description contdesc = info->types[i];
        std::cout<<contdesc.desc<<std::endl;
      }
      break;
    }
    case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
      std::cout<<"set memory maps"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_SET_GEOMETRY:
      std::cout<<"set geometry"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_USERNAME:
      std::cout<<"get username"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_LANGUAGE:
      std::cout<<"get language"<<std::endl;
      break;
    case RETRO_ENVIRONMENT_GET_CAN_DUPE:
      std::cerr<<"get can dupe"<<std::endl;
      *(bool*)data=true;
      return true;
    default:
      std::cerr<<"Unknown"<<std::endl;
      return false;

  }
  return false;
}
static void video_frame(const void *data, unsigned width, unsigned height, size_t pitch)
{
  std::cout<<"video refresh: "<<width<<"x"<<height<<std::endl;
}
static void audio_sample(int16_t left, int16_t right)
{
  //int16_t *buffer[2];
  //buffer[0] = left;
  //buffer[1] = right;
  //ao_play(dev, buffer, sizeof(buffer));
  std::cout<<"audio sample"<<std::endl;
}
static size_t audio_sample_batch(const int16_t *data, size_t frames)
{
  //ao_play(dev, data, frames * sizeof(int16_t))
  return 0;
}
static void input_poll()
{
  std::cout<<"input poll"<<std::endl;
}
static int16_t input_status(unsigned port, unsigned device, unsigned index, unsigned id)
{
  std::cout<<"input status:"<<port<<":"<<device<<":"<<index<<":"<<id<<std::endl;
  return 0;
}*/


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
  //sigc::slot<bool, unsigned, void*> slot = sigc::mem_fun(*this, &LibRetroCore::environment_cb);
  //retro_environment_t env = slot;
  /*setEnvironment(environment_cb);
  setVideoRefresh(&video_frame);
  setAudioSample(&audio_sample);
  setAudioSampleBatch(&audio_sample_batch);
  setInputPoll(&input_poll);
  setInputState(&input_status);*/
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
