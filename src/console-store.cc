#include "console-store.hh"
#include <iostream>

using namespace Gnome;

ConsoleStore::ConsoleStore(const ConsoleModel::ColumnRecord& columns):
  Gtk::ListStore()
{
  set_column_types(columns);
  openVGDB = Gda::Connection::open_from_string(
   "SQLite",
   "DB_DIR=.;DB_NAME=openvgdb.sqlite",
   "",
   Gda::CONNECTION_OPTIONS_READ_ONLY);
  
  const std::string query = "SELECT systemID, systemName, systemShortName FROM SYSTEMS;";
  
  Glib::RefPtr<Gda::DataModel> data;
  try
  {
    data = openVGDB->statement_execute_select(query);
  }
  catch(const Glib::Error& err)
  {
    std::cerr<<"Select error: "<<err.what()<<std::endl;
  }
  for(int i = 0; i<data->get_n_rows(); ++i)
  {
    ConsoleModel::Row row = *append();
    try
    {
      Glib::ustring shortName = data->get_value_at(2, i).get_string();
      Glib::ustring iconPath = "/org/colinkinloch/emulatron/systems/"+shortName+"/library.png";
      row[col.id] = data->get_value_at(0, i).get_int();
      row[col.name] = data->get_value_at(1, i).get_string();
      row[col.shortName] = shortName;
    }
    catch(const Glib::Error& err)
    {
      std::cerr<<"Select error: "<<err.what()<<std::endl;
    }
    try
    {
      Glib::ustring iconPath = "/org/colinkinloch/emulatron/systems/"+row[col.shortName]+"/library.png";
      row[col.icon] = Gdk::Pixbuf::create_from_resource(iconPath);
    }
    catch(const Glib::Error& err)
    {
      std::cerr<<"Icon error: "<<err.what()<<std::endl;
      try
      {
        Glib::ustring iconPath = "/org/colinkinloch/emulatron/systems/Default/library.png";
        row[col.icon] = Gdk::Pixbuf::create_from_resource(iconPath);
      }
      catch(const Glib::Error& err)
      {
        std::cerr<<"Icon error: "<<err.what()<<std::endl;
      }
    }
  }
}
ConsoleStore::ConsoleStore():
  ConsoleStore::ConsoleStore(col)
{}
ConsoleStore::~ConsoleStore()
{}

Glib::RefPtr<ConsoleStore> ConsoleStore::create(const ConsoleModel::ColumnRecord& columns)
{
  return Glib::RefPtr<ConsoleStore>(new ConsoleStore(columns));
}
Glib::RefPtr<ConsoleStore> ConsoleStore::create()
{
  return Glib::RefPtr<ConsoleStore>(new ConsoleStore());
}
