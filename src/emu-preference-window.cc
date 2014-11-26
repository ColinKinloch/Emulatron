#include "emu-preference-window.hh"

EmuPreferenceWindow::EmuPreferenceWindow()
{
  set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
}
EmuPreferenceWindow::EmuPreferenceWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder):
  Gtk::Window(cobject)
{
  EmuPreferenceWindow();
  
  refBuilder->get_widget("pref-library-content", libraryPanel);
  refBuilder->get_widget("pref-gameplay-content", gameplayPanel);
  refBuilder->get_widget_derived("pref-control-content", controlPanel);
  refBuilder->get_widget("pref-core-content", corePanel);
}
EmuPreferenceWindow::~EmuPreferenceWindow()
{
  
}
