#include "config.h"

#include "emu-header-bar.hh"
#include <gdkmm/general.h>

namespace Emu {
  HeaderBar::HeaderBar():
    Glib::ObjectBase("HeaderBar"),
    Gtk::Widget()
  {
    //CppClassType* widget_class_cpp;
    //GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(CppClassType);
    //gtk_widget_class_set_template_from_resource(widget_class, "/org/colinkinloch/emulatron/emu-header-bar.ui");
    //gtk_widget_class_bind_template_child_private(widget_class, CppClassType, search_button);
    //gtk_widget_class_bind_template_child_full(widget_class, "search_button", FALSE, offsetof(HeaderBar, search_button));
  }
  HeaderBar::~HeaderBar()
  {
  }
}
