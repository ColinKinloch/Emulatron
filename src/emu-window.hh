#pragma once

#include <gtkmm/applicationwindow.h>
#include <gtkmm/builder.h>

class EmuWindow: public Gtk::ApplicationWindow
{
public:
  EmuWindow();
  EmuWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  virtual ~EmuWindow();
  
protected:
};
