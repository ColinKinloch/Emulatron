#pragma once

#include <gtkmm.h>

class GameArea:
    public Gtk::GLArea
{
public:
  GameArea(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  virtual ~GameArea();
};
