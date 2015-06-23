#include "audio.hh"


static void context_state_cb(pa_context *c, void *data)
{
   pa_t *pa = (pa_t*)data;

   switch (pa_context_get_state(c))
   {
      case PA_CONTEXT_READY:
      case PA_CONTEXT_TERMINATED:
      case PA_CONTEXT_FAILED:
         pa_threaded_mainloop_signal(pa->mainloop, 0);
         break;
      default:
         break;
   }
}

static void stream_state_cb(pa_stream *s, void *data)
{
   pa_t *pa = (pa_t*)data;

   switch (pa_stream_get_state(s))
   {
      case PA_STREAM_READY:
      case PA_STREAM_FAILED:
      case PA_STREAM_TERMINATED:
         pa_threaded_mainloop_signal(pa->mainloop, 0);
         break;
      default:
         break;
   }
}

static void stream_request_cb(pa_stream *s, size_t length, void *data)
{
   pa_t *pa = (pa_t*)data;

   (void)length;
   (void)s;

   pa_threaded_mainloop_signal(pa->mainloop, 0);
}

static void stream_latency_update_cb(pa_stream *s, void *data)
{
   pa_t *pa = (pa_t*)data;

   (void)s;

   pa_threaded_mainloop_signal(pa->mainloop, 0);
}

static void underrun_update_cb(pa_stream *s, void *data)
{
   pa_t *pa = (pa_t*)data;

   (void)s;

   std::cout<<"[PulseAudio]: Underrun (Buffer: "<<pa->buffer_size
   <<", Writable size:"<<pa_stream_writable_size(pa->stream)
   <<")."<<std::endl;
}

static void buffer_attr_cb(pa_stream *s, void *data)
{
   pa_t *pa = (pa_t*)data;
   const pa_buffer_attr *server_attr = pa_stream_get_buffer_attr(s);
   if (server_attr)
      pa->buffer_size = server_attr->tlength;
   std::cout<<"[PulseAudio]: Got new buffer size "<<pa->buffer_size<<"."<<std::endl;

}
static void stream_success_cb(pa_stream *s, int success, void *data)
{
   (void)s;
   pa_t *pa = (pa_t*)data;
   pa->success = success;
   pa_threaded_mainloop_signal(pa->mainloop, 0);
}
Audio::Audio(Glib::RefPtr<Gio::Settings> set)
{

  settings = set;

  unsigned rate = settings->get_uint("rate");
  unsigned latency = settings->get_uint("latency");

  pa_proplist *ctxProp = pa_proplist_new();
  pa_proplist_sets(ctxProp, PA_PROP_APPLICATION_ICON_NAME, "input-gaming");
  pa = new pa_t();
  pa->mainloop = pa_threaded_mainloop_new();
  pa->context = pa_context_new_with_proplist(pa_threaded_mainloop_get_api(pa->mainloop), "Emulatron", ctxProp);
  pa_context_set_state_callback(pa->context, context_state_cb, pa);

  pa_context_connect(pa->context, nullptr, PA_CONTEXT_NOFLAGS, NULL);
  pa_threaded_mainloop_lock(pa->mainloop);
  pa_threaded_mainloop_start(pa->mainloop);
  pa_threaded_mainloop_wait(pa->mainloop);

  pa_sample_spec spec;
  spec.format = PA_SAMPLE_S16LE;
  spec.channels = 2;
  spec.rate = rate;

  pa->stream = pa_stream_new(pa->context, "audio", &spec, nullptr);

  pa_stream_set_state_callback(pa->stream, stream_state_cb, pa);
  pa_stream_set_write_callback(pa->stream, stream_request_cb, pa);
  pa_stream_set_latency_update_callback(pa->stream, stream_latency_update_cb, pa);
  pa_stream_set_underflow_callback(pa->stream, underrun_update_cb, pa);
  pa_stream_set_buffer_attr_callback(pa->stream, buffer_attr_cb, pa);

  pa_stream_connect_playback(pa->stream, NULL, &buffer_attr, PA_STREAM_ADJUST_LATENCY, NULL, NULL);

  pa_threaded_mainloop_wait(pa->mainloop);

  const pa_buffer_attr *server_attr = nullptr;
  server_attr = pa_stream_get_buffer_attr(pa->stream);
  if (server_attr)
  {
    pa->buffer_size = server_attr->tlength;
    std::cout<<"[PulseAudio]: Requested "<<buffer_attr.tlength
    <<" bytes buffer, got "<<pa->buffer_size
    <<"."<<std::endl;
  }
  else
    pa->buffer_size = buffer_attr.tlength;

  pa_threaded_mainloop_unlock(pa->mainloop);
}
Audio::~Audio()
{
  if (!pa)
    return;

  if (pa->mainloop)
    pa_threaded_mainloop_stop(pa->mainloop);

  if (pa->stream)
  {
    pa_stream_disconnect(pa->stream);
    pa_stream_unref(pa->stream);
  }

  if (pa->context)
  {
    pa_context_disconnect(pa->context);
    pa_context_unref(pa->context);
  }

  if (pa->mainloop)
    pa_threaded_mainloop_free(pa->mainloop);
}

size_t Audio::write(const void *buffer, size_t size)
{
  const uint8_t *buf = (const uint8_t*)buffer;

  size_t written = 0;

   pa_threaded_mainloop_lock(pa->mainloop);
   while (size)
   {
      size_t writable = pa_stream_writable_size(pa->stream);

      writable = std::min(size, writable);

      if (writable)
      {
         pa_stream_write(pa->stream, buf, writable, NULL, 0, PA_SEEK_RELATIVE);
         buf += writable;
         size -= writable;
         written += writable;
      }
      else if (!pa->nonblock)
         pa_threaded_mainloop_wait(pa->mainloop);
      else
         break;
   }

   pa_threaded_mainloop_unlock(pa->mainloop);

   return written;
}
bool Audio::stop()
{
  bool ret;
  std::cout<<"[PulseAudio]: Pausing."<<std::endl;

  pa->success = true; /* In case of spurious wakeup. Not critical. */
  pa_threaded_mainloop_lock(pa->mainloop);
  pa_stream_cork(pa->stream, true, stream_success_cb, pa);
  pa_threaded_mainloop_wait(pa->mainloop);
  ret = pa->success;
  pa_threaded_mainloop_unlock(pa->mainloop);
  pa->is_paused = true;
  return ret;
}
bool Audio::alive()
{
  if(!pa)
    return false;
  return !pa->is_paused;
}
bool Audio::start()
{
  bool ret;

  std::cout<<"[PulseAudio]: Unpausing."<<std::endl;

   pa->success = true; /* In case of spurious wakeup. Not critical. */
   pa_threaded_mainloop_lock(pa->mainloop);
   pa_stream_cork(pa->stream, false, stream_success_cb, pa);
   pa_threaded_mainloop_wait(pa->mainloop);
   ret = pa->success;
   pa_threaded_mainloop_unlock(pa->mainloop);
   pa->is_paused = false;
   return ret;
}
