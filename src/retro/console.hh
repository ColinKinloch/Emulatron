#pragma once

#include "core.hh"
#include "../controller.hh"
#include "../mouse.hh"
#include "../audio.hh"

#include <glibmm/dispatcher.h>
#include <thread>
#include <mutex>
#include <glibmm/timer.h>
#include <cairomm/matrix.h>
#include <cairomm/surface.h>
namespace Retro
{
  class Console
  {
  public:
    Console(std::string path);

    void start();
    void stop();
    void play();
    void pause();
    void togglePlaying();
    void reset();

    bool loadGame(Glib::RefPtr<Gio::File> file);

    retro_system_info info;
    Glib::Timer frameTimer;

    Cairo::Format vFormat;
    Cairo::RefPtr<Cairo::ImageSurface> video;

    int16_t *audioBuffer;
    size_t audioFrames;

    Glib::Dispatcher m_signal_draw;
    Glib::Dispatcher m_signal_audio;
    Glib::Dispatcher m_signal_input_poll;

    std::mutex video_lock;
    std::mutex audio_lock;

    Mouse* mouse;

    Audio* audio;

  protected:
    bool set_environment(unsigned cmd, void *data);
    void set_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch);
    void set_audio_sample(int16_t left, int16_t right);
    size_t set_audio_sample_batch(const int16_t *data, size_t frames);
    void set_input_poll();
    int16_t set_input_state(unsigned port, unsigned device, unsigned index, unsigned id);

    bool audio_driver_flush(const int16_t *data, size_t samples);

  private:
    Retro::Core* core;
    std::thread gameThread;

    void run();
    bool running;
    bool playing;

  };
};
