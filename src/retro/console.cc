#include "console.hh"
#include "device.hh"

#include <iostream>
#include <regex>
#include <cstring>
#include <chrono>

#include <gsf/gsf.h>

using namespace std;

// TODO Move callbacks
void logfun(enum retro_log_level level, const char *fmt, ...)
{
  va_list args;
  va_start( args, fmt );
  vfprintf( stderr, fmt, args );
  va_end( args );
}

retro_time_t perfGetTimeUsec() {
  return chrono::duration_cast<chrono::microseconds>(
      chrono::system_clock::now().time_since_epoch()
    ).count();
}

/*uint64_t getCpuFeatures() {
  return 0;
}*/

retro_perf_tick_t perfGetCounter() {
  return chrono::duration_cast<chrono::nanoseconds>(
      chrono::system_clock::now().time_since_epoch()
    ).count();
}

namespace Retro
{
  Console::Console(std::string path):
    frameTimeCallback(nullptr)
  {
    core = new Retro::Core(path);

    info = core->getSystemInfo();
    // avInfo = core->getSystemAVInfo();
    vFormat = Cairo::Format::FORMAT_RGB16_565;

  }

  void Console::init()
  {
    core->loadSymbols();

    core->signal_environment()
    .connect(sigc::mem_fun(this, &Console::set_environment));
    core->signal_video_refresh()
    .connect(sigc::mem_fun(this, &Console::set_video_refresh));
    core->signal_audio_sample()
    .connect(sigc::mem_fun(this, &Console::set_audio_sample));
    core->signal_audio_sample_batch()
    .connect(sigc::mem_fun(this, &Console::set_audio_sample_batch));
    core->signal_input_poll()
    .connect(sigc::mem_fun(this, &Console::set_input_poll));
    core->signal_input_state()
    .connect(sigc::mem_fun(this, &Console::set_input_state));

    core->init();

    //core->setControllerPortDevice(0, RETRO_DEVICE_MOUSE);
  }

  void Console::deinit()
  {
    core->deinit();
  }

  void Console::start()
  {
    running = true;
    playing = false;
    gameThread = std::thread(sigc::mem_fun(this, &Retro::Console::run));
  }
  void Console::run()
  {
    auto tick = std::chrono::system_clock::now();
    auto tock = tick;
    while(running)
    {
      //video_lock.lock();
      tick = std::chrono::system_clock::now();
      retro_usec_t t = (std::chrono::duration_cast<std::chrono::microseconds>(tick - tock)).count();
      if(frameTimeCallback != nullptr) {
        frameTimeCallback(t);
        cout<<t<<endl;
        }
      if(playing) core->run();
      tock = tick;
      //video_lock.unlock();
    }
  }
  void Console::stop()
  {
    running = false;
    if(gameThread.joinable()) {
      gameThread.join();
    }
  }
  void Console::play()
  {
    playing = true;
  }
  void Console::pause()
  {
    playing = false;
  }
  void Console::togglePlaying()
  {
    playing = !playing;
  }
  void Console::reset()
  {
    core->reset();
  }

  bool Retro::Console::loadGame(Glib::RefPtr<Gio::File> file)
  {
    std::cout<<file->query_info()->get_content_type()<<std::endl;
    // Test archive/zip; mimetype? magic? regexp?
    /*std::regex regZip ("*.zip$");
    GError* err = nullptr;
    // TODO gsfmm?
    GsfInput* gsfStream = gsf_input_gio_new(file->gobj(), &err);
    GsfInfile* gsfIn = gsf_infile_zip_new(gsfStream, &err);
    if (err != nullptr) std::cerr<<"Failed to unzip: "<<err->message<<std::endl;
    int nFiles = gsf_infile_num_children(gsfIn);
    GsfInput* f = nullptr;
    for (int i = 0; i < nFiles; ++i) {
      GsfInput* f = gsf_infile_child_by_index(gsfIn, i);
      if (std::regex_match(f->name, regZip)) break;
    }
    std::cout<<f.name<<std::endl;
    size_t size = f->size;
    const guint8* data = gsf_input_read(f, bytes, nullptr);
    */
    char* contents;
    size_t size;
    string path = file->get_path();
    game.path = path.c_str();
    try {
      file->load_contents(contents, size);
    }
    catch(Gio::Error e)
    {
      std::cerr<<"Loading game: "<<file->get_basename()<<e.what()<<std::endl;
    }
    game.data = contents;
    game.size = size;
    return core->loadGame(&game);
  }

  bool Console::set_environment(unsigned cmd, void *data)
  {
    switch(cmd) {
      case RETRO_ENVIRONMENT_SET_ROTATION:
        std::cout<<"set rotation"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_GET_OVERSCAN:
        std::cout<<"get overscan"<<std::endl;
        *((bool*)data) = true;
        return true;
      case RETRO_ENVIRONMENT_GET_CAN_DUPE:
        std::cerr<<"get can dupe"<<std::endl;
        *(bool*)data=true;
        return true;
      case RETRO_ENVIRONMENT_SET_MESSAGE:
        std::cout<<"set message"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_SHUTDOWN:
        std::cout<<"shutdown"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL:
      {
        const unsigned *perf = (const unsigned *)data;
        std::cout<<"set performance level:"<<*perf<<std::endl;
        break;
      }
      case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
      {
        std::cout<<"get system directory"<<std::endl;
        *(const char**)data = nullptr; // "./"; // Gio::File::create_for_path("./")->get_path().c_str();
        return true;
      }
      case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
      {
        switch(*(retro_pixel_format*)data)
        {
          case RETRO_PIXEL_FORMAT_0RGB1555:
          {
            // Unsupported
            std::cout<<"Pixel format unsupported"<<std::endl;
            break;
          }
          case RETRO_PIXEL_FORMAT_XRGB8888:
          {
            vFormat = Cairo::Format::FORMAT_ARGB32;
            return true;
          }
          case RETRO_PIXEL_FORMAT_RGB565:
          {
            vFormat = Cairo::Format::FORMAT_RGB16_565;
            return true;
          }
          case RETRO_PIXEL_FORMAT_UNKNOWN:
          {
            std::cout<<"Pixel format Unkown"<<std::endl;
            break;
          }
        }
      }
      case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
      {
        std::cout<<"set input descriptors"<<std::endl;
        retro_input_descriptor* var = (retro_input_descriptor*)data;
        std::cout<<var->description<<std::endl;
        break;
      }
      case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK:
        std::cout<<"set keyboard callback"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
        std::cout<<"set disk control interface"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_SET_HW_RENDER:
      {
        std::cout<<"set hw render"<<std::endl;
        retro_hw_render_callback* render = (retro_hw_render_callback*)data;
        switch(render->context_type) {
          case RETRO_HW_CONTEXT_NONE:
            break;
          case RETRO_HW_CONTEXT_OPENGL:
          case RETRO_HW_CONTEXT_OPENGLES2:
          case RETRO_HW_CONTEXT_OPENGL_CORE:
          case RETRO_HW_CONTEXT_OPENGLES3:
          case RETRO_HW_CONTEXT_OPENGLES_VERSION:
          case RETRO_HW_CONTEXT_VULKAN:
            break;
          case RETRO_HW_CONTEXT_DUMMY:
            break;
        }
        break;
      }
      case RETRO_ENVIRONMENT_GET_VARIABLE:
      {
        std::cout<<"get variable: "<<std::endl;
        retro_variable* var = (retro_variable*)data;
        if(var->key != nullptr) {
          std::cout<<var->key;
          // var->value = ???;
          //std::cout<<"="<<var->valfue
          std::cout<<std::endl;
        }
        break;
      }
      case RETRO_ENVIRONMENT_SET_VARIABLES:
      {
        std::cout<<"set varible: "<<std::endl;
        retro_variable* var = (retro_variable*)data;
        if(var->key != nullptr || var->key != nullptr) {
          std::cout<<var->key<<"="<<var->value<<std::endl;
          return true;
        }
        std::cout<<"null value"<<std::endl;
        break;
      }
      case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
      {
        // std::cout<<"get variable update: "<<std::endl;
        // *(bool*)data = false;
        break;
      }
      case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
        std::cout<<"set support no game"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH:
      {
        std::cout<<"get libretro path"<<std::endl;
        const char* path = core->file->get_path().c_str();
        //(const char**)data = path;
        std::memcpy(&path, data, sizeof(path));
        return true;
      }
      case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK:
        std::cout<<"set audio callback"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK:
        std::cout<<"set frame time callback"<<std::endl;
        frameTimeCallback = ((retro_frame_time_callback*) data)->callback;
        return true;
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
        ((struct retro_log_callback *) data)->log = logfun;
        return true;
      case RETRO_ENVIRONMENT_GET_PERF_INTERFACE:
      {
        std::cout<<"get perf interface"<<std::endl;
        retro_perf_callback* pcb = (retro_perf_callback*)data;
        pcb->get_time_usec = perfGetTimeUsec;
        //pcb->get_cpu_features = getCpuFeatures;
        pcb->get_perf_counter = perfGetCounter;
        break;
      }
      case RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE:
        std::cout<<"get location interface"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY:
        std::cout<<"get core assets directory"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
      {
        std::cout<<"get save directory"<<std::endl;
        /*string path = Gio::File::create_for_path("./")->get_path();
        *(const char**)data = path.c_str();*/
        *(const char**)data = nullptr; // Gio::File::create_for_path("./")->get_path().c_str();
        return true;
      }
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
          std::cout<<"-"<<contdesc.desc<<std::endl;
        }
        break;
      }
      case RETRO_ENVIRONMENT_SET_MEMORY_MAPS:
      {
        std::cout<<"set memory maps"<<std::endl;
        retro_memory_map* maps = (retro_memory_map*)data;
        for(int i=0; maps->num_descriptors; i++)
        {
          retro_memory_descriptor desc = maps->descriptors[i];
          //std::cout<<desc.addrspace<<std::cout;
        }
        break;
      }
      case RETRO_ENVIRONMENT_SET_GEOMETRY:
        std::cout<<"set geometry"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_GET_USERNAME:
      {
        std::cout<<"get username"<<std::endl;
        const char* uname = Glib::get_real_name().c_str();
        data = &uname;
        return true;
      }
      case RETRO_ENVIRONMENT_GET_LANGUAGE:
      {
        std::cout<<"get language"<<std::endl;
        const gchar * const * localeStrings = g_get_language_names();
        retro_language* language = (retro_language*)data;
        for(int i = 0; i < sizeof(localeStrings); ++i) {
          std::string locale = localeStrings[i];
          if(locale == "en") *language = RETRO_LANGUAGE_ENGLISH;
          else if(locale == "ja") *language = RETRO_LANGUAGE_JAPANESE;
          else if(locale == "fr") *language = RETRO_LANGUAGE_FRENCH;
          else if(locale == "es") *language = RETRO_LANGUAGE_SPANISH;
          else if(locale == "de") *language = RETRO_LANGUAGE_GERMAN;
          else if(locale == "it") *language = RETRO_LANGUAGE_ITALIAN;
          else if(locale == "nl") *language = RETRO_LANGUAGE_DUTCH;
          else if(locale == "pt") *language = RETRO_LANGUAGE_PORTUGUESE;
          else if(locale == "ru") *language = RETRO_LANGUAGE_RUSSIAN;
          else if(locale == "ko") *language = RETRO_LANGUAGE_KOREAN;
          else if(locale == "zh_tw") *language = RETRO_LANGUAGE_CHINESE_TRADITIONAL;
          else if(locale == "zh_cn") *language = RETRO_LANGUAGE_CHINESE_SIMPLIFIED;
          //else if(locale == "eo") *language = RETRO_LANGUAGE_ESPERANTO;
          //else if(locale == "pl") *language = RETRO_LANGUAGE_POLISH;
          return true;
        }
      }
      case RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER:
        cout<<"get Software Framebuffer"<<endl;
        break;
      case RETRO_ENVIRONMENT_GET_HW_RENDER_INTERFACE:
        cout<<"get hw render interface"<<endl;
        break;
      default:
        cerr<<"Unknown command:"<<cmd<<std::endl;
    }
    return false;
  }
  void Console::set_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
  {
    video_lock.lock();
    video = Cairo::ImageSurface::create(vFormat, width, height);
    auto ctx = Cairo::Context::create(video);

    auto source = Cairo::ImageSurface::create((uint8_t*)data, vFormat, width, height, pitch);
    ctx->set_source(source, 0, 0);
    ctx->paint();

    //Debug capture :)
    /*auto dir = Gio::File::create_for_path("./screenshots/");
    if(!dir->query_exists()) dir->make_directory();
    int i = 0;
    Glib::RefPtr<Gio::File> file;
    std::string filename;
    do {
      filename = std::to_string(++i);
      filename += ".png";
      file = dir->get_child(filename);
    } while (file->query_exists());
    video->write_to_png(file->get_path());*/

    video_lock.unlock();
    m_signal_draw();
  }
  void Console::set_audio_sample(int16_t left, int16_t right)
  {
    audioFrames = 2;
    std::lock_guard<std::mutex> lock(audio_lock);
    audioBuffer = new int16_t[audioFrames];
    audioBuffer[0] = left;
    audioBuffer[1] = right;
    m_signal_audio();
    audio_driver_flush(audioBuffer, audioFrames << 1);
  }
  size_t Console::set_audio_sample_batch(const int16_t *data, size_t frames)
  {
    if (frames > (AUDIO_CHUNK_SIZE_NONBLOCKING >> 1))
      frames = AUDIO_CHUNK_SIZE_NONBLOCKING >> 1;
    std::lock_guard<std::mutex> lock(audio_lock);
    audioFrames = frames;
    //audioBuffer = new int16_t[audioFrames];
    audioBuffer = (int16_t*)data;
    m_signal_audio();
    audio_driver_flush(data, frames << 1);
    return frames;
  }
  void Console::set_input_poll()
  {
    m_signal_input_poll();
    Controller::poll();
  }
  int16_t Console::set_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
  {
    switch(device)
    {
      case Retro::DeviceType::NONE: return 0;
      case RETRO_DEVICE_JOYPAD:
      {
        Controller* cont = Controller::controllers[port];
        if(cont)
        {
          return cont->getButton(id);
        }
      }
      /*case Retro::DeviceType::MOUSE:
      {
        switch(id)
        {
          case Retro::DeviceID::MOUSE_X:
          {
            return mouse->getdX();
          }
          case Retro::DeviceID::MOUSE_Y:
          {
            return mouse->getdY();
          }
          case Retro::DeviceID::MOUSE_LEFT:
          {
            return mouse->getLeft();
          }
          case Retro::DeviceID::MOUSE_RIGHT:
          {
            return mouse->getRight();
          }
          case Retro::DeviceID::MOUSE_MIDDLE:
          {
            return mouse->getMiddle();
          }
        }
      }
      case Retro::DeviceType::LIGHTGUN:
      {
        switch(id)
        {
          case Retro::DeviceID::LIGHTGUN_X:
          {
            return mouse->getdX();
          }
          case Retro::DeviceID::LIGHTGUN_Y:
          {
            return mouse->getdY();
          }
          case Retro::DeviceID::LIGHTGUN_TRIGGER:
          {
            return mouse->getLeft();
          }
          case Retro::DeviceID::LIGHTGUN_TURBO:
          {
            return mouse->getMiddle();
          }
          case Retro::DeviceID::LIGHTGUN_START:
          {
            return mouse->getRight();
          }
        }
      }*/
      case Retro::DeviceType::ANALOG:
      {
        Controller* cont = Controller::controllers[port];
        if(cont)
        {
          return cont->getAxis(id, index);
        }
      }
      /*case Retro::DeviceType::POINTER:
      {
        switch(id)
        {
          case RETRO_DEVICE_ID_POINTER_X:
          {
            return mouse->getX();
          }
          case RETRO_DEVICE_ID_POINTER_Y:
          {
            return mouse->getY();
          }
          case RETRO_DEVICE_ID_POINTER_PRESSED:
          {
            return mouse->getLeft();
          }
        }
      }*/
    }
    return 0;
  }
  bool Console::audio_driver_flush(const int16_t *data, size_t samples)
  {
    size_t i;
    size_t outsize = sizeof(float);
    float out[samples];
    float gain = 1.0 / 0x8000;
    for(i=0; i < samples; i++)
      out[i] = (float)data[i] * gain;
    audio->write(out, samples*outsize);
    return true;
  }
};
