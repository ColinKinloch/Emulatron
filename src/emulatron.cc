#include "config.h"

#include <algorithm>
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

Audio* aud;
Gtk::DrawingArea* area;
Gtk::Allocation alloc;
Glib::RefPtr<Gdk::Pixbuf> pix;

retro_pixel_format pixFormat = RETRO_PIXEL_FORMAT_0RGB1555;

static gboolean
render (GtkGLArea *area, GdkGLContext *context)
{
  glClearColor (0,0,0,0);
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

static int latency = 64;//pow(2, 6);// start latency in micro seconds
static int sampleoffs = 0;
static int16_t sampledata[20000];

static bool environment_cb(unsigned cmd, void *data)
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
      //std::cout<<"get variable update"<<std::endl;
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
  int sw = width;
  int sh = height;
  float sr = (float)sw/sh;
  int dw = alloc.get_width();
  int dh = alloc.get_height();
  float dr = (float)dw/dh;
  float s = 0;
  if(dr<sr)
  {
    s = (float)dw/sw;
  }
  else
  {
    std::cout<<"this happens?"<<std::endl;
    s = (float)dh/sh;
  }
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
  pix = oPix->scale_simple(s*width, s*height, Gdk::INTERP_NEAREST);
  area->queue_draw();
  //area->set(pix);
  delete p;
  //std::cout<<"video refresh: "<<width<<"x"<<height<<":"<<padding<<std::endl;
}
static bool draw_cairo(const Cairo::RefPtr<Cairo::Context>& cr)
{
  if(!pix)
    return false;
  Gdk::Cairo::set_source_pixbuf(cr, pix, 0, 0);
  cr->paint();
  return true;
}
static void resize_cairo(Gtk::Allocation a)
{
  alloc = a;
}
static void audio_sample(int16_t left, int16_t right)
{
  //int16_t *buffer[2];
  //buffer[0] = left;
  //buffer[1] = right;
  //ao_play(dev, buffer, sizeof(buffer));
  std::cout<<"audio sample"<<std::endl;
}
bool audio_driver_flush(const int16_t *data, size_t samples)
{

  //convert samples to avInfo.timing.sample_rate
  //audioSettings->get_uint("rate")
  //avInfo.timing.sample_rate
  float dif = avInfo.timing.sample_rate/aud->settings->get_uint("rate");
  /*for(int i=0; i<aud->settings->get_uint("rate"); ++i)
  {
  }*/
  //aud->write(data, samples);
  return true;
}
static size_t audio_sample_batch(const int16_t *data, size_t frames)
{
  if (frames > (AUDIO_CHUNK_SIZE_NONBLOCKING >> 1))
    frames = AUDIO_CHUNK_SIZE_NONBLOCKING >> 1;

  audio_driver_flush(data, frames << 1);

  return frames;
}
static void input_poll()
{
  //std::cout<<"input poll"<<std::endl;
}
static int16_t input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
  //std::cout<<"input state:"<<port<<":"<<device<<":"<<index<<":"<<id<<std::endl;
  return 0;
}



Emulatron::Emulatron(int& argc, char**& argv):
  Gtk::Application(argc, argv, "org.colinkinloch.emulatron", Gio::APPLICATION_FLAGS_NONE)
{

  Glib::init();
  Gio::init();

  Gtk::Window::set_default_icon_list({
    Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png"),
    Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/icon32.png"),
    Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/joy-angle-256.png")
  });

  settings = Gio::Settings::create("org.colinkinloch.emulatron");

  LibRetroCore* dinothwar = new LibRetroCore("./src/libretro-cores/Dinothawr/dinothawr_libretro");
  LibRetroCore* bsnes = new LibRetroCore("./src/libretro-cores/bsnes-libretro/out/bsnes_accuracy_libretro");
  LibRetroCore* vbaNext = new LibRetroCore("./src/libretro-cores/vba-next/vba_next_libretro");
  LibRetroCore* snes9x = new LibRetroCore("./src/libretro-cores/snes9x/libretro/snes9x_libretro");
  //LibRetroCore* mupen = new LibRetroCore("./src/libretro-cores/mupen64plus-libretro/mupen64plus_libretro");

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

  Glib::RefPtr<Gio::Settings> audioSettings = settings->get_child("audio");
  audio = aud = new Audio(audioSettings);

  Glib::RefPtr<Gio::File> openVGDBFile = Gio::File::create_for_path(settings->get_string("openvgdb-path"));
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

  uiSettings = Gtk::Settings::get_default();
  uiSettings->property_gtk_application_prefer_dark_theme() = true;

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
    refBuilder->get_widget("game_cairo_area", gameCairoArea);
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

  area = gameCairoArea;

  GtkWidget* glAreaWidget = gameArea->gobj();
  GtkGLArea* glArea = GTK_GL_AREA(glAreaWidget);
  gtk_gl_area_make_current(glArea);

  glClearColor(0,1,0,0);
  glClear(GL_COLOR_BUFFER_BIT);

  g_signal_connect(glArea, "render", G_CALLBACK(render), NULL);
  g_signal_connect(glArea, "resize", G_CALLBACK(resize), NULL);

  //g_signal_connect(gameCairoArea, "resize", G_CALLBACK(draw_cairo), NULL);
  //g_signal_connect(gameCairoArea, "draw", G_CALLBACK(draw_cairo), NULL);
  gameCairoArea->signal_size_allocate().connect(&resize_cairo);
  gameCairoArea->signal_draw().connect(&draw_cairo);

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
    std::cout<<avInfo.timing.fps<<std::endl;
    Glib::signal_timeout().connect(sigc::mem_fun(this, &Emulatron::stepGame), 1000/avInfo.timing.fps, Glib::PRIORITY_HIGH);
  }
}
bool Emulatron::stepGame()
{
  core->run();
  return true;
}
bool Emulatron::stepSound()
{
  //pa_mainloop_iterate(pa_ml, false, nullptr);
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
