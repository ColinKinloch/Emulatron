#include "game-store.hh"
#include <iostream>
#include <thread>

using namespace Gnome;

GameStore::GameStore(const GameModel::ColumnRecord& columns): Gtk::TreeStore()
{
  set_column_types(columns);
  openVGDB = Gda::Connection::open_from_string(
   "SQLite",
   "DB_DIR=.;DB_NAME=openvgdb.sqlite",
   "",
   Gda::CONNECTION_OPTIONS_READ_ONLY);
  builder = Gda::SqlBuilder::create(Gda::SQL_STATEMENT_SELECT);

  builder->select_set_distinct(true);

  //Return values
  auto rorid = builder->select_add_field("romID", "ROMs");
  auto ssid = builder->select_add_field("systemID", "SYSTEMS");
  auto rname = builder->select_add_field("releaseTitleName", "RELEASES");
  auto rcover = builder->select_add_field("releaseCoverFront", "RELEASES");

  //Queried values
  auto rosid = builder->add_field_id("systemID", "ROMs");
  auto rrid = builder->add_field_id("romID", "RELEASES");
  auto romd5 = builder->add_field_id("romHashMD5", "ROMs");
  auto rosha1 = builder->add_field_id("romHashSHA1", "ROMs");

  //Input values
  auto pmd5 = builder->add_param("md5", G_TYPE_STRING, true);
  auto psha1 = builder->add_param("sha1", G_TYPE_STRING, true);

  builder->select_add_target_id(builder->add_id("SYSTEMS"));
  builder->select_add_target_id(builder->add_id("RELEASES"));
  builder->select_add_target_id(builder->add_id("ROMs"));

  builder->set_where(
   builder->add_cond(Gda::SQL_OPERATOR_TYPE_AND,
    builder->add_cond(Gda::SQL_OPERATOR_TYPE_EQ, rosid, ssid),
    builder->add_cond(Gda::SQL_OPERATOR_TYPE_EQ, rorid, rrid),
    builder->add_cond(Gda::SQL_OPERATOR_TYPE_OR,
     builder->add_cond(Gda::SQL_OPERATOR_TYPE_EQ, romd5, pmd5),
     builder->add_cond(Gda::SQL_OPERATOR_TYPE_EQ, rosha1, psha1)
    )
   )
  );
}
GameStore::GameStore(): GameStore::GameStore(col)
{}
GameStore::~GameStore()
{
}

bool GameStore::add(std::string uri)
{
  Glib::RefPtr<Gio::File> file = Gio::File::create_for_uri(uri);
  return add(file);
}

bool GameStore::add(Glib::RefPtr<Gio::File> file)
{
  GameModel::Row row = *append();
  row[col.filename] = file->get_uri();
  row[col.title] = file->get_basename();
  //row[col.cover] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/missing_artwork.png");
  //row[col.thumbnail] = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/missing_artwork.png");
  row[col.spinner] = true;
  row[col.pulse] = 0;

  Glib::RefPtr<Glib::TimeoutSource> spinClock = Glib::TimeoutSource::create(128);
  spinClock->attach(Glib::MainContext::get_default());
  sigc::connection spin = spinClock->connect(sigc::bind<GameModel::Row>(sigc::mem_fun(this, &GameStore::onSpin), row));

  //Glib::Threads::Thread* thread = Glib::Threads::Thread::create(sigc::bind<GameModel::Row, Glib::RefPtr<Gio::File>, sigc::connection >(sigc::mem_fun(this, &GameStore::gatherROMData), row, file, spin));
  std::thread* th = new std::thread(sigc::bind<GameModel::Row, Glib::RefPtr<Gio::File>, sigc::connection >(sigc::mem_fun(this, &GameStore::gatherROMData), row, file, spin));

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

void GameStore::gatherROMData(GameModel::Row row, Glib::RefPtr<Gio::File> file, sigc::connection spin)
{
  auto romStream = file->read();
  auto md5 = new Glib::Checksum(Glib::Checksum::CHECKSUM_MD5);
  auto sha1 = new Glib::Checksum(Glib::Checksum::CHECKSUM_SHA1);
  std::vector<uint8_t> chunk(4e+3);
  size_t size;
  while((size = romStream->read(chunk.data(), chunk.size()))) {
    md5->update(chunk.data(), size);
    sha1->update(chunk.data(), size);
  }

  Glib::ustring md5String = md5->get_string();
  Glib::ustring sha1String = sha1->get_string();

  row[col.md5] = md5String;
  row[col.sha1] = sha1String;

  auto statement = builder->get_statement();

  Glib::RefPtr<Gda::Set> params;
  statement->get_parameters(params);

  params->get_holder("md5")->set_value<Glib::ustring>(md5String.uppercase());
  params->get_holder("sha1")->set_value<Glib::ustring>(sha1String.uppercase());

  Glib::RefPtr<Gda::DataModel> data;
  try
  {
    data = openVGDB->statement_execute_select(statement, params);
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

  Glib::RefPtr<Gio::InputStream> stream;
  Glib::RefPtr<Gdk::Pixbuf> cover;
  try
  {
    stream = image->read();
    cover = Gdk::Pixbuf::create_from_stream(stream);
  }
  catch(const Glib::Error& err)
  {
    std::cerr<<"Thumbnail image error: "<<err.what()<<std::endl;
    cover = Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/missing_artwork.png");
  }

  row[col.cover] = cover;

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

  row[col.thumbnail] = cover200;

  row[col.spinner] = false;
  spin.disconnect();
}

bool GameStore::onSpin(GameModel::Row row) {
  row[col.pulse] = row[col.pulse]+1;
  return true;
}

