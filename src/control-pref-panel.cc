#include "control-pref-panel.hh"
#include <iostream>
#include <gtkmm/cssprovider.h>

ControlPrefPanel::ControlPrefPanel()
{
  
}
ControlPrefPanel::ControlPrefPanel(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder):
  Gtk::Box(cobject)
{
  ControlPrefPanel();
  
  consoles = ConsoleStore::create();
  controllers = ControllerStore::create();
  players = PlayerStore::create(4);
  
  refBuilder->get_widget("control-image", image);
  refBuilder->get_widget("control-console-combo", consoleCombo);
  refBuilder->get_widget("control-player-combo", playerCombo);
  refBuilder->get_widget("control-controller-combo", controllerCombo);
  
  consoleCombo->set_model(consoles);
  consoleCombo->set_id_column(consoles->col.id.index());
  consoleCombo->set_active(0);
  
  playerCombo->set_model(players);
  playerCombo->set_active(0);
  
  controllerCombo->set_model(controllers);
  controllerCombo->set_active(0);
  
  consoleCombo->signal_changed().connect(sigc::mem_fun(this, &ControlPrefPanel::on_console_changed));
}
ControlPrefPanel::~ControlPrefPanel()
{
  
}

void ControlPrefPanel::on_console_changed()
{
  Gtk::TreeModel::Row row = *(consoleCombo->get_active());
  Glib::ustring imagePath = "/org/colinkinloch/emulatron/systems/"+row[consoles->col.shortName]+"/controller.png";
  try
  {
    image->set(Gdk::Pixbuf::create_from_resource(imagePath));
  }
  catch(Glib::Error& err)
  {
    std::cerr<<"Controller image error: "<<err.what()<<std::endl;
    image->set(Gdk::Pixbuf::create_from_resource("/org/colinkinloch/emulatron/systems/Default/controller.png"));
  }
}
