#include "console.hh"
#include "device.hh"

#include <iostream>

namespace Retro
{
  Console::Console(std::string path)
  {
    core = new Retro::Core(path);

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

    avInfo = core->getSystemAVInfo();
    frameLength = 1000000/avInfo.timing.fps;
  }

  void Console::start()
  {
    running = true;
    playing = false;
    gameThread = Glib::Threads::Thread::create(sigc::mem_fun(this, &Retro::Console::run));
  }
  void Console::run()
  {
    while(running)
    {
      //frameTimer.start();
      if(playing)
        core->run();
      /*frameTimer.stop();
      frameTimer.elapsed(prevFrameLength);
      frameTimer.reset();
      Glib::usleep(frameLength-prevFrameLength);*/
    }
  }
  void Console::stop()
  {
    running = false;
    gameThread->join();
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

  bool Retro::Console::loadGame(Glib::RefPtr<Gio::File> file)
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
        return false;
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
        std::cout<<"get system directory"<<std::endl;
        break;
      case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
      {
        switch(*(retro_pixel_format*)data)
        {
          case RETRO_PIXEL_FORMAT_RGB565:
          {
            vFormat = Cairo::Format::FORMAT_RGB16_565;
            break;
          }
          case RETRO_PIXEL_FORMAT_0RGB1555:
          {
            break;
          }
          case RETRO_PIXEL_FORMAT_XRGB8888:
          {
            vFormat = Cairo::Format::FORMAT_ARGB32;
            break;
          }
          case RETRO_PIXEL_FORMAT_UNKNOWN:
          {
            break;
          }
        }
        return true;
      }
      case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
      {
        std::cout<<"set input descriptors"<<std::endl;
        retro_input_descriptor* var = (retro_input_descriptor*)data;
        std::cout<<var->description<<std::endl;
        break;
      }
    }
    return false;
  }
  void Console::set_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
  {
    //video_lock.writer_lock();
    video = Cairo::ImageSurface::create((unsigned char*)data, vFormat, width, height, pitch);
    //video_lock.writer_unlock();
    m_signal_draw();
  }
  void Console::set_audio_sample(int16_t left, int16_t right)
  {
    audioFrames = 2;
    //audio_lock.writer_lock();
    audioBuffer = new int16_t[audioFrames];
    audioBuffer[0] = left;
    audioBuffer[1] = right;
    //audio_lock.writer_unlock();
    //m_signal_audio();
    audio_driver_flush(audioBuffer, audioFrames << 1);
  }
  size_t Console::set_audio_sample_batch(const int16_t *data, size_t frames)
  {
    if (frames > (AUDIO_CHUNK_SIZE_NONBLOCKING >> 1))
      frames = AUDIO_CHUNK_SIZE_NONBLOCKING >> 1;
    //audio_lock.writer_lock();
    audioFrames = frames;
    //audioBuffer = new int16_t[audioFrames];
    audioBuffer = (int16_t*)data;
    //audio_lock.writer_unlock();
    //m_signal_audio();
    audio_driver_flush(data, frames << 1);
    return frames;
  }
  void Console::set_input_poll()
  {
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
      case Retro::DeviceType::MOUSE:
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
      }
      case Retro::DeviceType::ANALOG:
      {
        Controller* cont = Controller::controllers[port];
        if(cont)
        {
          return cont->getAxis(id, index);
        }
      }
      case Retro::DeviceType::POINTER:
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
      }
    }
    return 0;
  }
  bool Console::audio_driver_flush(const int16_t *data, size_t samples)
  {
    size_t i;
    size_t outsize = sizeof(float);
    unsigned int rate = audio->settings->get_uint("rate");
    float out[samples];
    float gain = 1.0 / 0x8000;
    for(i=0; i < samples; i++)
      out[i] = (float)data[i] * gain;
    audio->write(out, samples*outsize);
    return true;
  }
};
