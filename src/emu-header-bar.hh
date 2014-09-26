#pragma once

#include <gtkmm/widget.h>

namespace Emu {
  class HeaderBar: public Gtk::Widget
  {
  public:
    HeaderBar();
    virtual ~HeaderBar();

  protected:

  private:
    /* Template widgets */
    Gtk::Widget   *search_button;
    Gtk::Widget   *select_button;
    Gtk::Widget   *done_button;
    Gtk::Widget   *back_button;
    Gtk::Widget   *stack;

    /* Visibility */
    gboolean     show_search_button;
    gboolean     show_select_button;

    /* Modes */
    gboolean     search_mode;
    gboolean     select_mode;

    /* Normal title */
    Gtk::Widget   *title_label;
    Gtk::Widget   *subtitle_label;

    /* Custom title */
    Gtk::Widget   *custom_title;

    /* Selection mode */
    guint        n_selected;
    Gtk::Widget   *selection_menu_button;

  };
}
