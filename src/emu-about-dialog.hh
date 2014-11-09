#pragma once

#include <gtkmm/aboutdialog.h>
#include <gtkmm/builder.h>

class EmuAboutDialog: public Gtk::AboutDialog
{
public:
  EmuAboutDialog();
  EmuAboutDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  virtual ~EmuAboutDialog();
  
protected:
};
