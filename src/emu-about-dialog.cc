#include "emu-about-dialog.hh"

EmuAboutDialog::EmuAboutDialog()
{
  
}
EmuAboutDialog::EmuAboutDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::AboutDialog(cobject)
{
  const std::vector<Glib::ustring> authors = {
    "Colin Kinloch <colin@kinlo.ch>"
  };
  //const std::vector<Glib::ustring> artists = {};
  set_authors(authors);
  //aboutDialog->set_artists(artists);
  set_logo(Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/img/joy-angle-256.png"));
  set_copyright("© 2014 Colin Kinloch");
  set_version("0.0.0");//VERSION
}
EmuAboutDialog::~EmuAboutDialog()
{
  
}
