#include "game-store.hh"
#include <iostream>

using namespace Gnome;

GameStore::GameStore(const GameModel::ColumnRecord& columns): Gtk::TreeStore()
{
  set_column_types(columns);
  openVGDB = Gda::Connection::open_from_string(
   "SQLite",
   "DB_DIR=.;DB_NAME=openvgdb.sqlite",
   "",
   Gda::CONNECTION_OPTIONS_READ_ONLY);
}
GameStore::GameStore(): GameStore::GameStore(col)
{}
GameStore::~GameStore()
{
}

bool GameStore::add(std::string uri)
{
  GameModel::Row row = *append();
  
  row[col.filename] = uri;
  row[col.title] = "Loading...";
  row[col.cover] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png");
  row[col.thumbnail] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png", 200, 200);
  
  Glib::RefPtr<Gio::File> file = Gio::File::create_for_uri(uri);
  file->load_contents_async(sigc::bind<GameModel::Row, Glib::RefPtr<Gio::File>>(sigc::mem_fun(this, &GameStore::on_rom_lookup), row, file));
  
  /*
  std::string romTitle;
  std::string romCoverUrl;
  
  Glib::RefPtr<Gio::File> file = Gio::File::create_for_uri(uri);
  std::string contents;
  try
  {
    contents = Glib::file_get_contents(file->get_path());
  }
  catch(Glib::Error &err)
  {
    std::cerr<<"File error: "<<err.what()<<std::endl;
    return false;
  }
  std::string sha1 = Glib::Checksum::compute_checksum(Glib::Checksum::CHECKSUM_SHA1, contents);
  std::string md5 = Glib::Checksum::compute_checksum(Glib::Checksum::CHECKSUM_MD5, contents);
  
  std::string query = "SELECT ROMs.romID, SYSTEMS.systemID, RELEASES.releaseTitleName, RELEASES.releaseCoverFront FROM SYSTEMS, RELEASES, ROMs WHERE SYSTEMS.systemID=ROMs.systemID AND RELEASES.RomID=ROMs.RomID AND (LOWER(ROMs.romHashMD5)='"+md5+"' OR LOWER(ROMs.romHashSHA1)='"+sha1+"');";
  
  Glib::RefPtr<Gda::DataModel> data;
  
  try
  {
    data = openVGDB->statement_execute_select(query);
  }
  catch(const Glib::Error& err)
  {
    std::cerr<<"Select error: "<<err.what()<<std::endl;
    return false;
  }
  
  try
  {
    if(data->get_n_rows())
    {
      romTitle = data->get_value_at(2, 0).get_string();
      romCoverUrl = data->get_value_at(3, 0).get_string();
    }
    else
    {
      romTitle = "unknown";
      return false;
    }
  }
  catch(const Glib::Error& err)
  {
    std::cerr<<"Select error: "<<err.what()<<std::endl;
    return false;
  }
  
  if(romTitle.size() == 0)
  {
    romTitle = file->get_basename();
  }
  
  Glib::RefPtr<Gdk::Pixbuf> cover;
  try
  {
    //read_async
    //Glib::RefPtr<Gio::File> file = Gio::File::create_for_uri(romCoverUrl);
    //file->read_async(sigc::mem_fun(*this, &GameStore::thumb_callback));
    cover = Gdk::Pixbuf::create_from_stream(Gio::File::create_for_uri(romCoverUrl)->read());
  }
  catch(const Glib::Error& err)
  {
    std::cerr<<"Thumbnail from URL error: "<<err.what()<<std::endl;
    cover = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png");
  }
  int cw = cover->get_width();
  int ch = cover->get_height();
  int mw = 200;
  int mh = mw*1.25;
  float s;
  if(cw>ch)
  {
    s = (float)mw/cw;
  }
  else
  {
    s = (float)mh/ch;
  }
  
  Glib::RefPtr<Gdk::Pixbuf> cover200 = cover->scale_simple(cw*s, ch*s, Gdk::INTERP_BILINEAR);
  
  row[col.filename] = uri;
  row[col.title] = romTitle;
  row[col.cover] = cover;
  row[col.thumbnail] = cover200;*/
  return true;
}

Glib::RefPtr<GameStore> GameStore::create(const GameModel::ColumnRecord& columns)
{
  return Glib::RefPtr<GameStore>(new GameStore(columns));
}
Glib::RefPtr<GameStore> GameStore::create()
{
  return Glib::RefPtr<GameStore>(new GameStore());
}

void GameStore::on_rom_lookup(const Glib::RefPtr<Gio::AsyncResult>& result, 
 GameModel::Row row, Glib::RefPtr<Gio::File> file)
{
  Glib::ustring md5;
  try
  {
    //Glib::RefPtr<Gio::InputStream> stream = file->read_finish(result);
    char* contents;
    gsize size;
    file->load_contents_finish(result, contents, size);
    md5 = Glib::Checksum::compute_checksum(Glib::Checksum::CHECKSUM_MD5, (guchar*)contents, size);
    row[col.md5] = md5;
  }
  catch(Glib::Error &err)
  {
    std::cerr<<"Stream Callback fail: "<<err.what()<<std::endl;
  }
  
  const std::string query = "SELECT ROMs.romID, SYSTEMS.systemID, RELEASES.releaseTitleName, RELEASES.releaseCoverFront FROM SYSTEMS, RELEASES, ROMs WHERE SYSTEMS.systemID=ROMs.systemID AND RELEASES.RomID=ROMs.RomID AND LOWER(ROMs.romHashMD5)='"+md5+"';";
  
  Glib::RefPtr<Gda::DataModel> data;
  try
  {
    data = openVGDB->statement_execute_select(query);
  }
  catch(const Glib::Error& err)
  {
    std::cerr<<"Select error: "<<err.what()<<std::endl;
  }
  
  Glib::ustring romTitle;
  Glib::ustring romCoverUrl;
  try
  {
    if(data->get_n_rows())
    {
      romTitle = data->get_value_at(2, 0).get_string();
      romCoverUrl = data->get_value_at(3, 0).get_string();
    }
    else
    {
      romTitle = file->get_basename();
    }
  }
  catch(const Glib::Error& err)
  {
    std::cerr<<"Select error: "<<err.what()<<std::endl;
  }
  row[col.title] = romTitle;
  
  Glib::RefPtr<Gio::File> image = Gio::File::create_for_uri(romCoverUrl);
  image->read_async(
   sigc::bind<GameModel::Row, Glib::RefPtr<Gio::File>>(
    sigc::mem_fun(this, &GameStore::on_image_ready), row, image));
}
void GameStore::on_image_ready(const Glib::RefPtr<Gio::AsyncResult>& result,
 GameModel::Row row, Glib::RefPtr<Gio::File> file)
{
  Glib::RefPtr<Gdk::Pixbuf> cover;
  try
  {
    //Glib::RefPtr<Gio::InputStream> stream = file->read_finish(result);
    cover = Gdk::Pixbuf::create_from_stream(file->read_finish(result));
  }
  catch(const Glib::Error& err)
  {
    std::cerr<<"Thumbnail from URL error: "<<err.what()<<std::endl;
    cover = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/dino_down.png");
  }
  int cw = cover->get_width();
  int ch = cover->get_height();
  int mw = 200;
  int mh = mw*1.25;
  float s;
  if(cw>ch)
  {
    s = (float)mw/cw;
  }
  else
  {
    s = (float)mh/ch;
  }
  
  Glib::RefPtr<Gdk::Pixbuf> cover200 = cover->scale_simple(cw*s, ch*s, Gdk::INTERP_BILINEAR);
  
  row[col.cover] = cover;
  row[col.thumbnail] = cover200;
}
