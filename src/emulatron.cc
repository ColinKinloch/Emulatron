#include "config.h"

#include <cmath>
#include <random>
#include <iostream>

#include <gtkmm.h>
#include <gdkmm/pixbuf.h>

#include <SDL.h>

#if __APPLE__
#include <GL/gl.h>
#else
#include <GLES3/gl3.h>
#endif

#include "emulatron.hh"
#include "emu-window.hh"

#include "libretro-core.hh"
#include "openvgdb.hh"

#include "emu-resources.h"

#include "game-model.hh"

retro_system_av_info avInfo;

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution(0,1);
auto rng = std::bind(distribution, generator);

Gtk::Image* area;
Glib::RefPtr<Gdk::Pixbuf> pix;

retro_pixel_format pixFormat = RETRO_PIXEL_FORMAT_0RGB1555;

static gboolean
render (GtkGLArea *area, GdkGLContext *context)
{
  glClearColor (rng(), rng(), rng(), 0);
  glClear (GL_COLOR_BUFFER_BIT);
  return true;
};

static void
resize (GtkGLArea *area, int width, int height)
{
  gtk_gl_area_make_current(area);
  glViewport(0,0,width,height);
};

struct {
  uint16_t *data;
  size_t size;
  unsigned offset;
} buffer;

typedef struct
{
   pa_threaded_mainloop *mainloop;
   pa_context *context;
   pa_stream *stream;
   size_t buffer_size;
   bool nonblock;
   bool success;
   bool is_paused;
} pa_t;

static pa_t* pa;

static int latency = pow(2, 7); // start latency in micro seconds
static int sampleoffs = 0;
static int16_t sampledata[20000];
static pa_buffer_attr buffer_attr;
static int underflows = 0;
static pa_sample_spec ss;
static pa_mainloop *pa_ml;
static pa_mainloop_api *pa_mlapi;
static pa_context *pa_ctx;
static pa_stream *playstream;

// This callback gets called when our context changes state.  We really only
// care about when it's ready or if it has failed
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

static bool environment_cb(unsigned cmd, void *data)
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
      pixFormat = *(retro_pixel_format*)data;
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
      if(var->key != nullptr) {
        std::cout<<var->key<<": "<<var->value<<std::endl;
        return true;
      }
      else {
        std::cout<<"null value"<<std::endl;
        return false;
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
//TODO BitFields don't work
struct RGB1555 {
  unsigned char r :5;
  unsigned char g :5;
  unsigned char b :5;
};
struct RGB565 {
  unsigned char r :5;
  unsigned char g :6;
  unsigned char b :5;
};
struct RGB8888 {
  unsigned char r :8;
  unsigned char g :8;
  unsigned char b :8;
  unsigned char a :8;
};
#define GREEN_MASK  0b0000011111100000
#define GREEN_OFFSET  5
#define RED_MASK  0b1111100000000000
#define RED_OFFSET  11
#define BLUE_MASK  0b0000000000011111
#define BLUE_OFFSET  0
RGB8888* toRGB8888(void* in, unsigned width, unsigned height, size_t pitch)
{
  int padding = (pitch - (2 * width))/2;
  size_t stride = (width+padding)*3;
  int nPixels = (width+padding)*height;
  RGB8888 *out;
  out = new RGB8888[nPixels];
  for(int p = 0; p<nPixels; ++p)
  {
    //uint16_t b = ((int*)in)[p];
    uint8_t b1 = ((char*)in)[p*2];
    uint8_t b2 = ((char*)in)[(p*2)+1];
    uint16_t b = b1|((uint16_t)b2)<<8;
    out[p].r = 255/31 * (b >> RED_OFFSET);
    out[p].g = 255/63 * (b & GREEN_MASK) >> GREEN_OFFSET;
    out[p].b = 255/31 * (b & BLUE_MASK);
    out[p].a = 255;
  }
  return out;
}

//BAD conversion
RGB8888* toRGB8888(RGB565* in, unsigned width, unsigned height, size_t pitch)
{
  int padding = (pitch - (2 * width))/2;
  size_t stride = (width+padding)*3;
  int nPixels = (width+padding)*height;
  RGB8888 *out;
  out = new RGB8888[nPixels];
  for(int p = 0; p<nPixels; ++p)
  {
    RGB565 pixel = in[p];
    out[p].b = 255/31 * pixel.r;
    out[p].g = 255/63 * pixel.g;
    out[p].r = 255/31 * pixel.b;
    out[p].a = 255;
  }
  return out;
}
RGB8888* toRGB8888(RGB1555* in, unsigned width, unsigned height, size_t pitch)
{
  int padding = (pitch - (2 * width))/2;
  size_t stride = (width+padding)*3;
  int nPixels = (width+padding)*height;
  RGB8888 *out;
  out = new RGB8888[nPixels];
  for(int p = 0; p<nPixels; ++p)
  {
    RGB1555 pixel = in[p];
    out[p].b = 255/31 * pixel.r;
    out[p].g = 255/31 * pixel.g;
    out[p].r = 255/31 * pixel.b;
    out[p].a = 255;
  }
  return out;
}
static void video_frame(const void *data, unsigned width, unsigned height, size_t pitch)
{
  int z = 4;
  int padding;
  RGB8888* p;
  switch(pixFormat)
  {
    case RETRO_PIXEL_FORMAT_RGB565:
    {
      padding = (pitch - (2 * width))/2;
      p = toRGB8888((void*)data, width, height, pitch);
      //p = toRGB8888((RGB565*)data, width, height, pitch);
      break;
    }
    case RETRO_PIXEL_FORMAT_0RGB1555:
    {
      padding = (pitch - (2 * width))/2;
      p = toRGB8888((RGB1555*)data, width, height, pitch);
      break;
    }
    case RETRO_PIXEL_FORMAT_XRGB8888:
    {
      padding = (pitch - (4 * width))/4;
      p = (RGB8888*)data;
      break;
    }
    case RETRO_PIXEL_FORMAT_UNKNOWN:
    {
      break;
    }
  }
  size_t stride = (width+padding)*4;

  auto oPix = Gdk::Pixbuf::create_from_data((guchar*)p,
  Gdk::COLORSPACE_RGB, true, 8,
  width, height, stride);
  pix = oPix->scale_simple(width*z, height*z, Gdk::INTERP_NEAREST);
  area->set(pix);
  delete p;
  std::cout<<"video refresh: "<<width<<"x"<<height<<":"<<padding<<std::endl;
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
  float gain = 1;
  float* out;
   size_t i;
   gain = gain / 0x8000;
   for (i = 0; i < frames; i++)
      out[i] = (float)data[i] * gain;
  //memcpy(sampledata, data, frames);
  //sampleoffs = 0;
  //pa_mainloop_iterate(pa_ml, false, nullptr);
  //std::cout<<"audio sample:"<<frames<<std::endl;
  //ao_play(dev, data, frames * sizeof(int16_t))
  return 0;
}
static void input_poll()
{
  std::cout<<"input poll"<<std::endl;
}
static int16_t input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
  std::cout<<"input state:"<<port<<":"<<device<<":"<<index<<":"<<id<<std::endl;
  return 0;
}



Emulatron::Emulatron(int& argc, char**& argv):
  Gtk::Application(argc, argv, "org.colinkinloch.emulatron", Gio::APPLICATION_FLAGS_NONE)
{

  Glib::init();
  Gio::init();


  LibRetroCore* dinothwar = new LibRetroCore("./src/libretro-cores/Dinothawr/dinothawr_libretro.dylib");
  LibRetroCore* bsnes = new LibRetroCore("./src/libretro-cores/bsnes-libretro/out/bsnes_accuracy_libretro.dylib");
  LibRetroCore* vbaNext = new LibRetroCore("./src/libretro-cores/vba-next/vba_next_libretro.dylib");

  core = vbaNext;

  core->loadSymbols();

  core->setEnvironment(&environment_cb);
  core->setVideoRefresh(&video_frame);
  core->setAudioSample(&audio_sample);
  core->setAudioSampleBatch(&audio_sample_batch);
  core->setInputPoll(&input_poll);
  core->setInputState(&input_state);

  avInfo = core->getSystemAVInfo();

  int r;
  int pa_ready = 0;
  int retval = 0;
  unsigned int a;
  double amp;

  for (a=0; a<sizeof(sampledata)/2; a++) {
    sampledata[a] = 0;
  }

  pa_proplist *ctxProp = pa_proplist_new();
  pa_proplist_sets(ctxProp, PA_PROP_APPLICATION_ICON_NAME, "input-gaming");

  pa = new pa_t();
  pa->mainloop = pa_threaded_mainloop_new();
  pa_ml = pa_mainloop_new();
  pa_mlapi = pa_mainloop_get_api(pa_ml);
  pa->context = pa_ctx = pa_context_new_with_proplist(pa_threaded_mainloop_get_api(pa->mainloop), "Emulatron", ctxProp);
  pa_context_set_state_callback(pa->context, context_state_cb, pa);

  pa_context_connect(pa->context, nullptr, PA_CONTEXT_NOFLAGS, NULL);
  pa_threaded_mainloop_lock(pa->mainloop);
  pa_threaded_mainloop_start(pa->mainloop);
  pa_threaded_mainloop_wait(pa->mainloop);

  pa_sample_spec spec;
  spec.format = PA_SAMPLE_FLOAT32LE;
  spec.channels = 2;
  spec.rate = avInfo.timing.sample_rate;

  pa->stream = pa_stream_new(pa->context, "audio", &spec, nullptr);

  pa_stream_set_state_callback(pa->stream, stream_state_cb, pa);
  pa_stream_set_write_callback(pa->stream, stream_request_cb, pa);
  pa_stream_set_latency_update_callback(pa->stream, stream_latency_update_cb, pa);
  pa_stream_set_underflow_callback(pa->stream, underrun_update_cb, pa);
  pa_stream_set_buffer_attr_callback(pa->stream, buffer_attr_cb, pa);

  buffer_attr.maxlength = -1;
  buffer_attr.tlength = pa_usec_to_bytes(latency * PA_USEC_PER_MSEC, &spec);
  buffer_attr.prebuf = -1;
  buffer_attr.minreq = -1;
  buffer_attr.fragsize = -1;

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
  /*
  pa_context_flags_t ctx_flags;
  pa_context_connect(pa_ctx, nullptr, (pa_context_flags_t)0, nullptr);

  pa_context_set_state_callback(pa_ctx, pa_state_cb, &pa_ready);

  while (pa_ready == 0) {
    pa_mainloop_iterate(pa_ml, 1, NULL);
  }

  ss.rate = avInfo.timing.sample_rate;
  ss.channels = 2;
  ss.format = PA_SAMPLE_FLOAT32LE;
  playstream = pa_stream_new(pa_ctx, "audio", &ss, NULL);
  if (!playstream) {
    std::cout<<"pa_stream_new failed"<<std::endl;
  }
  pa_stream_set_write_callback(playstream, stream_request_cb, NULL);
  pa_stream_set_underflow_callback(playstream, stream_underflow_cb, NULL);

  bufattr.fragsize = (uint32_t)-1;
  bufattr.maxlength = -1;//pa_usec_to_bytes(latency,&ss);
  bufattr.minreq = -1;//pa_usec_to_bytes(0,&ss);
  bufattr.prebuf = (uint32_t)-1;
  bufattr.tlength = pa_usec_to_bytes(latency*PA_USEC_PER_MSEC, &ss);
  r = pa_stream_connect_playback(playstream, NULL, &bufattr,
  (pa_stream_flags_t)(PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_ADJUST_LATENCY | PA_STREAM_AUTO_TIMING_UPDATE),
  NULL, NULL);

  if (r < 0) {
    // Old pulse audio servers don't like the ADJUST_LATENCY flag, so retry without that
    r = pa_stream_connect_playback(playstream, NULL, &bufattr,
                                   (pa_stream_flags_t)(PA_STREAM_INTERPOLATE_TIMING|
                                   PA_STREAM_AUTO_TIMING_UPDATE), NULL, NULL);
  }
  if (r < 0) {
    printf("pa_stream_connect_playback failed\n");
    retval = -1;
  }
  */
  Glib::RefPtr<Gio::File> openVGDBFile = Gio::File::create_for_path("./openvgdb.sqlite");
  OpenVGDB openVGDB = OpenVGDB(openVGDBFile);
  if(!openVGDBFile->query_exists())
  {
    std::cout<<"Downloading OpenVGDB"<<std::endl;
    //TODO Github releases api and glib-json
    //Glib::RefPtr<Gio::File> openVGDBUrl = Gio::File::create_for_uri("https://github.com/OpenVGDB/OpenVGDB/releases/latest");
    Glib::RefPtr<Gio::File> openVGDBUrl = Gio::File::create_for_uri("https://github.com/OpenVGDB/OpenVGDB/releases/download/v21.0/openvgdb.zip");
    //TODO Deal with zip
    //openVGDBFile->create_file()->splice(openVGDBUrl->read());
  }

  //retroClock = Glib::TimeoutSource::create(1000/avInfo.timing.fps);
  //retroClock = Glib::TimeoutSource::create(16);



  core->init();
  //core->loadGame(Gio::File::create_for_path("./fz.gba"));
  //core->run();
  //core.reset();
  //core->deinit();
  //retroClock->connect(sigc::mem_fun(this, &Emulatron::stepGame));
  //retroClock->connect(sigc::mem_fun(this, &Emulatron::stepGame));

  //sigc::mem_fun(core, &LibRetroCore::run));

  if(SDL_Init(SDL_INIT_HAPTIC|SDL_INIT_GAMECONTROLLER) != 0)
  {
    std::cerr<<SDL_GetError()<<std::endl;
  }

  refBuilder = Gtk::Builder::create();
  refBuilder->set_application(Glib::RefPtr<Emulatron>(this));

  settings = Gtk::Settings::get_default();
  settings->property_gtk_application_prefer_dark_theme() = true;
  
  Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
  Glib::RefPtr<Gtk::CssProvider> css = Gtk::CssProvider::create();
  
  css->load_from_file(Gio::File::create_for_uri("resource:///org/colinkinloch/emulatron/ui/emulatron.css"));
  
  Glib::RefPtr<Gtk::StyleContext> styleContext = Gtk::StyleContext::create();
  
  styleContext->add_provider_for_screen(screen,css,GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  
  try
  {
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/gtk/menus.ui");
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/ui/emu-view-actions.ui");
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/ui/emu-preference-dialog.ui");
    refBuilder->add_from_resource("/org/colinkinloch/emulatron/ui/emu-main-window.ui");
  }
  catch(const Gio::ResourceError& ex)
  {
    std::cerr<<"ResourceError: "<< ex.what() <<std::endl;
  }
  catch(const Glib::MarkupError& ex)
  {
    std::cerr<<"MarkupError: "<< ex.what() <<std::endl;
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
  
  try
  {
    refBuilder->get_widget_derived("emu_main_window", emuWindow);
    refBuilder->get_widget_derived("emu_pref_window", prefWindow);
    refBuilder->get_widget_derived("emu_about_dialog", aboutDialog);
    refBuilder->get_widget("game_area", gameArea);
    refBuilder->get_widget("game_image_area", gameImageArea);
    refBuilder->get_widget("emu_main_stack", emuMainStack);
    //add_window(*emuWindow);
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
  catch(std::runtime_error &ex)
  {
    std::cerr << "runtime_error: " << ex.what() << std::endl;
  }
  
  area = gameImageArea;

  GtkWidget* glAreaWidget = gameArea->gobj();
  GtkGLArea* glArea = GTK_GL_AREA(glAreaWidget);
  gtk_gl_area_make_current(glArea);

  glClearColor(0,1,0,0);
  glClear(GL_COLOR_BUFFER_BIT);

  g_signal_connect(glArea, "render", G_CALLBACK(render), NULL);
  g_signal_connect(glArea, "resize", G_CALLBACK(resize), NULL);

  prefWindow->set_transient_for(*emuWindow);
  aboutDialog->set_transient_for(*emuWindow);
  
  add_action("view-gl",
    sigc::mem_fun(this, &Emulatron::view_gl));

  add_action("fullscreen",
    sigc::mem_fun(emuWindow, &EmuWindow::fullscreen));

  add_action("open",
    sigc::mem_fun(this, &Emulatron::on_open));
  
  add_action("quit",
    sigc::mem_fun(this, &Emulatron::on_quit));
  
  add_action("preferences",
    sigc::mem_fun(this, &Emulatron::on_preferences));
  add_action("about",
    sigc::mem_fun(this, &Emulatron::on_about));
  
  emuWindow->signal_hide().connect(sigc::mem_fun(this, &Emulatron::on_quit));
  emuWindow->gameIconView->signal_item_activated().connect(sigc::mem_fun(this, &Emulatron::startGame));
  register_application();
  
  add_window(*emuWindow);
  add_window(*prefWindow);
  add_window(*aboutDialog);
  
  emuWindow->show();
  
  auto actions = list_actions();
  for(auto it = actions.begin(); it != actions.end() ;++it)
   std::cout<<*it<<std::endl;
}

void Emulatron::startGame(const Gtk::TreeModel::Path& path)
{
  auto store = emuWindow->gameIconView->get_model();
  auto iter = store->get_iter(path);

  if(iter)
  {
    Gtk::TreeModel::Row row = *iter;
    core->loadGame(Gio::File::create_for_uri((Glib::ustring)row[store->col.filename]));
    //retroClock->attach(Glib::MainContext::get_default());
    //Glib::signal_timeout().connect(sigc::mem_fun(this, &Emulatron::stepSound), (1000/avInfo.timing.fps));
    //retroClock->connect(sigc::mem_fun(this, &Emulatron::stepGame));
    //Glib::signal_idle().connect(sigc::mem_fun(this, &Emulatron::stepSound), Glib::PRIORITY_DEFAULT_IDLE);
    Glib::signal_timeout().connect(sigc::mem_fun(this, &Emulatron::stepGame), 16/*1000/avInfo.timing.fps*/, Glib::PRIORITY_HIGH);
  }
}
bool Emulatron::stepGame()
{
  core->run();
  return true;
}
bool Emulatron::stepSound()
{
  pa_mainloop_iterate(pa_ml, false, nullptr);
  return true;
}

void Emulatron::view_gl()
{
  std::string child = "";
  if(emuMainStack->get_visible_child_name() == "game-selector") {
    emuMainStack->set_visible_child("game-view", Gtk::STACK_TRANSITION_TYPE_OVER_LEFT);
  }
  else {
    emuMainStack->set_visible_child("game-selector", Gtk::STACK_TRANSITION_TYPE_UNDER_RIGHT);
  }
}

void Emulatron::on_open()
{
  std::cout<<"Open rom!"<<std::endl;
}

void Emulatron::on_preferences()
{
  prefWindow->show();
}

void Emulatron::on_about()
{
  aboutDialog->show();
}
void Emulatron::on_quit()
{
  std::cout << "Goodbye!" << std::endl;
  quit();
}

int main(int argc, char **argv)
{
  Glib::RefPtr<Emulatron> app = Glib::RefPtr<Emulatron>(new Emulatron(argc, argv));
  
  app->run(argc, argv);
  
  return 0;
}
