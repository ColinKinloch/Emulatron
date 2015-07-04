#include <pulse/pulseaudio.h>
#include <iostream>
#include <sigc++/sigc++.h>
#include <algorithm>
#include <gtkmm.h>

#define AUDIO_CHUNK_SIZE_BLOCKING 512
/* So we don't get complete line-noise when fast-forwarding audio. */
#define AUDIO_CHUNK_SIZE_NONBLOCKING 2048

typedef struct
{
  pa_threaded_mainloop *mainloop;
  pa_context *context;
  pa_stream *stream;
  pa_cvolume cvolume;
  size_t buffer_size;
  bool nonblock;
  bool success;
  bool is_paused;
} pa_t;


class Audio
{
  pa_t *pa;
  pa_buffer_attr buffer_attr;
public:
  Audio(Glib::RefPtr<Gio::Settings> set);
  ~Audio();

  Glib::RefPtr<Gio::Settings> settings;

  size_t write(const void *buffer, size_t size);
  void setVolume(double volume);
  bool stop();
  bool alive();
  bool start();
};
