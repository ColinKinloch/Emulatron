#include "emu-preference-window.hh"

EmuPreferenceWindow::EmuPreferenceWindow()
{
  
}
EmuPreferenceWindow::EmuPreferenceWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::Window(cobject)
{
  set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
}
EmuPreferenceWindow::~EmuPreferenceWindow()
{
  
}
