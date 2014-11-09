#pragma once

#include <gtkmm/window.h>
#include <gtkmm/builder.h>

class EmuPreferenceWindow: public Gtk::Window
{
public:
  EmuPreferenceWindow();
  EmuPreferenceWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  virtual ~EmuPreferenceWindow();
  
protected:
};
