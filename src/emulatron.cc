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

#include "retro/console.hh"
#include "openvgdb.hh"

#include "emu-resources.h"

#include "game-model.hh"

#include "controller.hh"
#include "mouse.hh"

retro_system_av_info avInfo;

Audio* aud;
Cairo::RefPtr<Cairo::SurfacePattern> patt;
int px, py;

Cairo::Format pixFormat = Cairo::Format::FORMAT_RGB16_565;

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

void logfun(enum retro_log_level level, const char *fmt, ...)
{
  std::cout<<"log"<<std::endl;
}

bool Emulatron::env(unsigned cmd, void *data)
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
          pixFormat = Cairo::Format::FORMAT_RGB16_565;
          break;
        }
        case RETRO_PIXEL_FORMAT_0RGB1555:
        {
          break;
        }
        case RETRO_PIXEL_FORMAT_XRGB8888:
        {
          pixFormat = Cairo::Format::FORMAT_ARGB32;
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
      if(var->key != nullptr) {
        std::cout<<"-"<<var->key<<": "<<var->value<<std::endl;
        return true;
      }
      break;
    }
    case RETRO_ENVIRONMENT_SET_VARIABLES:
    {
        retro_variable* var = (retro_variable*)data;
        std::cout<<"set variable:"<<std::endl;
      if(var->key != nullptr) {
        std::cout<<"-"<<var->key<<": "<<var->value<<std::endl;
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
    {
      std::cout<<"get libretro path"<<std::endl;
      const char* path = core->file->get_relative_path(Gio::File::create_for_path("/")).c_str();
      data = &path;
      return true;
      break;
    }
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
      data = (void*)&logfun;

      break;//return true;
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
void Emulatron::vf(const void *data, unsigned width, unsigned height, size_t pitch)
{
  int sw = width;
  int sh = height;
  float sr = (float)sw/sh;
  int dw = alloc.get_width();
  int dh = alloc.get_height();
  float dr = (float)dw/dh;
  float s;
  int cornx, corny;
  if(dr<sr)
  {
    s = (float)dw/sw;
  }
  else
  {
    s = (float)dh/sh;
  }
  cornx = -(dw-s*sw)/2;
  corny = -(dh-s*sh)/2;
  if(!s) {
    s = 1;
  }

  surf = Cairo::ImageSurface::create((unsigned char*)data, pixFormat, sw, sh, pitch);

  mat = Cairo::identity_matrix();
  mat.scale(1/s,1/s);
  mat.translate(cornx, corny);

  gameCairoArea->queue_draw();
}
bool Emulatron::draw_cairo(const Cairo::RefPtr<Cairo::Context>& cr)
{
  if(!console->video)
    return false;
  int dw = alloc.get_width();
  int dh = alloc.get_height();
  float dr = (float)dw/dh;
  console->video_lock.reader_lock();
  Cairo::RefPtr<Cairo::ImageSurface> sur = console->video;
  patt = Cairo::SurfacePattern::create(sur);
  int sw = sur->get_width();
  int sh = sur->get_height();
  console->video_lock.reader_unlock();
  float sr = (float)sw/sh;
  float s;
  int cornx, corny;
  if(dr<sr)
  {
    s = (float)dw/sw;
  }
  else
  {
    s = (float)dh/sh;
  }
  cornx = -(dw-s*sw)/2;
  corny = -(dh-s*sh)/2;
  if(!s) {
    s = 1;
  }
  mat = Cairo::identity_matrix();
  mat.scale(1/s,1/s);
  mat.translate(cornx, corny);
  console->mouse->setMatrix(mat);
  patt->set_filter(Cairo::Filter::FILTER_NEAREST);
  patt->set_matrix(mat);
  cr->set_source(patt);
  cr->paint();

  console->mouse->update();

  return true;
}
void Emulatron::resize_cairo(Gtk::Allocation a)
{
  alloc = a;
}
bool audio_driver_flush(const int16_t *data, size_t samples)
{
  size_t i;
  size_t outsize = sizeof(float);
  unsigned int rate = aud->settings->get_uint("rate");
  float out[samples];
  float gain = 1.0 / 0x8000;
  for(i=0; i < samples; i++)
    out[i] = (float)data[i] * gain;
  aud->write(out, samples*outsize);
  return true;
}
void Emulatron::as(int16_t left, int16_t right)
{
  std::cout<<"audio sample"<<std::endl;
  int16_t out[2];
  out[0] = left;
  out[1] = right;
  audio_driver_flush(out, 2);

}
size_t Emulatron::asb(const int16_t *data, size_t frames)
{
  if (frames > (AUDIO_CHUNK_SIZE_NONBLOCKING >> 1))
    frames = AUDIO_CHUNK_SIZE_NONBLOCKING >> 1;
  audio_driver_flush(data, frames << 1);

  return frames;
}
void Emulatron::ip()
{
  Controller::poll();
}
int16_t Emulatron::is(unsigned port, unsigned device, unsigned index, unsigned id)
{
  switch(device)
  {
    case RETRO_DEVICE_NONE: return 0;
    case RETRO_DEVICE_JOYPAD:
    {
      Controller* cont = Controller::controllers[port];
      if(cont)
      {
        return cont->getButton(id);
      }
    }
    case RETRO_DEVICE_MOUSE:
    {
      switch(id)
      {
        case RETRO_DEVICE_ID_MOUSE_X:
        {
          return px;
        }
        case RETRO_DEVICE_ID_MOUSE_Y:
        {
          return py;
        }
      }
    }
    case RETRO_DEVICE_ANALOG:
    {
      Controller* cont = Controller::controllers[port];
      if(cont)
      {
        return cont->getAxis(id, index);
      }
    }
  }
  return 0;
}


void Emulatron::trigger_draw()
{
  gameCairoArea->queue_draw();
}
void Emulatron::trigger_audio()
{
  //console->audio_lock.reader_lock();
  size_t frames = console->audioFrames;
  if (frames > (AUDIO_CHUNK_SIZE_NONBLOCKING >> 1))
    frames = AUDIO_CHUNK_SIZE_NONBLOCKING >> 1;
  //This is blocking!
  //audio_driver_flush(console->audioBuffer, console->audioFrames << 1);
  //console->audio_lock.reader_unlock();
}

Emulatron::Emulatron(int& argc, char**& argv):
  Gtk::Application(argc, argv, "org.colinkinloch.emulatron", Gio::APPLICATION_FLAGS_NONE)
{

  Glib::init();
  Gio::init();

  Gtk::Window::set_default_icon_list({
    Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png"),
    Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/joy-angle-256.png")
  });

  settings = Gio::Settings::create("org.colinkinloch.emulatron");

  if(SDL_Init(SDL_INIT_HAPTIC|SDL_INIT_GAMECONTROLLER) != 0)
  {
    std::cerr<<SDL_GetError()<<std::endl;
  }

  Retro::Console* snes = new Retro::Console("./src/libretro-cores/snes9x/libretro/snes9x_libretro");

  Retro::Core* dinothwar = new Retro::Core("./src/libretro-cores/Dinothawr/dinothawr_libretro");
  Retro::Core* bsnes = new Retro::Core("./src/libretro-cores/bsnes-libretro/out/bsnes_accuracy_libretro");
  Retro::Core* vbaNext = new Retro::Core("./src/libretro-cores/vba-next/vba_next_libretro");
  Retro::Core* snes9x = new Retro::Core("./src/libretro-cores/snes9x/libretro/snes9x_libretro");
  //Retro::Core* mupen = new Retro::Core("./src/libretro-cores/mupen64plus-libretro/mupen64plus_libretro");


  //snes->loadGame(Gio::File::create_for_path("./mq.smc"));

  running = false;

  console = snes;
  //core = snes9x;

  console->m_signal_draw.connect(sigc::mem_fun(this, &Emulatron::trigger_draw));
  //console->m_signal_audio.connect(sigc::mem_fun(this, &Emulatron::trigger_audio));

  /*core->loadSymbols();

  core->signal_environment().connect(sigc::mem_fun(this, &Emulatron::env));
  core->signal_video_refresh().connect(sigc::mem_fun(this, &Emulatron::vf));
  core->signal_audio_sample().connect(sigc::mem_fun(this, &Emulatron::as));
  core->signal_audio_sample_batch().connect(sigc::mem_fun(this, &Emulatron::asb));
  core->signal_input_poll().connect(sigc::mem_fun(this, &Emulatron::ip));
  core->signal_input_state().connect(sigc::mem_fun(this, &Emulatron::is));

  avInfo = core->getSystemAVInfo();*/

  Glib::RefPtr<Gio::Settings> audioSettings = settings->get_child("audio");
  audio = aud = new Audio(audioSettings);
  console->audio = audio;

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

  //core->init();

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
    refBuilder->get_widget("volume_slider", volumeSlider);
    refBuilder->get_widget("pause_button", pauseButton);
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
  }
  catch(std::runtime_error &ex)
  {
    std::cerr << "runtime_error: " << ex.what() << std::endl;
  }

  console->mouse = new Mouse(gameCairoArea);

  GtkWidget* glAreaWidget = gameArea->gobj();
  GtkGLArea* glArea = GTK_GL_AREA(glAreaWidget);
  gtk_gl_area_make_current(glArea);

  glClearColor(0,1,0,0);
  glClear(GL_COLOR_BUFFER_BIT);

  g_signal_connect(glArea, "render", G_CALLBACK(render), NULL);
  g_signal_connect(glArea, "resize", G_CALLBACK(resize), NULL);

  gameCairoArea->signal_size_allocate().connect(sigc::mem_fun(this, &Emulatron::resize_cairo));
  gameCairoArea->signal_draw().connect(sigc::mem_fun(this, &Emulatron::draw_cairo));

  pauseButton->signal_toggled().connect(sigc::mem_fun(console, &Retro::Console::togglePlaying));
  volumeSlider->signal_value_changed().connect(sigc::mem_fun(audio, &Audio::setVolume));
  volumeSlider->set_value(audio->getVolume());

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
  for(auto it : actions)
   std::cout<<it<<std::endl;
}

void Emulatron::startGame(const Gtk::TreeModel::Path& path)
{
  auto store = emuWindow->gameIconView->get_model();
  auto iter = store->get_iter(path);

  if(iter)
  {
    Gtk::TreeModel::Row row = *iter;
    running = false;
    //if(gameThread)
    //  gameThread->join();
    //core->unloadGame();
    //core->reset();
    console->stop();
    console->loadGame(Gio::File::create_for_uri((Glib::ustring)row[store->col.filename]));
    //core->loadGame(Gio::File::create_for_uri((Glib::ustring)row[store->col.filename]));
    running = true;
    console->start();
    console->play();
    //gameThread = Glib::Threads::Thread::create(sigc::mem_fun(this, &Emulatron::stepGame));
    view_gl();
  }
}
void Emulatron::stepGame()
{
  while(running)
  {
    core->run();
  }
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

void Emulatron::setVolume(double val)
{
  std::cout<<"helo"<<val<<std::endl;
  audio->setVolume(val);
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
