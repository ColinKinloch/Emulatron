#include "openvgdb.hh"
#include <iostream>

using namespace Gnome;

OpenVGDB::OpenVGDB(Glib::RefPtr<Gio::File> f)
{
  file = f;
  Glib::ustring filestr = "DB_DIR="+f->get_parent()->get_path()+";DB_NAME="+f->get_basename();
  std::cout<<filestr<<std::endl;
  connection = Gda::Connection::open_from_string("SQLite",
    filestr,//"DB_DIR="+f->get_path()+";DB_NAME="+f->get_basename(),
    "",
    Gda::CONNECTION_OPTIONS_READ_ONLY
  );
  std::cout<<"hello"<<std::endl;

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
OpenVGDB::OpenVGDB(Glib::ustring path)
{
  OpenVGDB(Gio::File::create_for_uri(path));
}
