#include "openvgdb.hh"

using namespace Gnome;

OpenVGDB::OpenVGDB(Glib::RefPtr<Gio::File> f)
{
  file = f;
  connection = Gda::Connection::open_from_string(
  "SQLite", "DB_DIR="+f->get_path()+";DB_NAME="+f->get_basename());
}
OpenVGDB::OpenVGDB(Glib::ustring path)
{
  OpenVGDB(Gio::File::create_for_uri(path));
}
