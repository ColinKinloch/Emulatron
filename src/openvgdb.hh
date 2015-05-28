#pragma once

#include <glibmm.h>
#include <giomm.h>
#include <libgdamm.h>

class OpenVGDB
{
public:
  OpenVGDB(Glib::RefPtr<Gio::File>);
  OpenVGDB(Glib::ustring);
private:
  Glib::RefPtr<Gio::File> file;
  Glib::RefPtr<Gnome::Gda::Connection> connection;
  Glib::RefPtr<Gnome::Gda::SqlBuilder> builder;
};
