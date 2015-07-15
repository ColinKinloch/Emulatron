#pragma once

#include <gtkmm/widget.h>
#include <gdkmm/devicemanager.h>
#include <cairomm/matrix.h>

class Mouse
{
public:
  Mouse(Gtk::Widget* widget);

  void update();

  int getX();
  int getY();
  int getdX();
  int getdY();

  bool getLeft();
  bool getRight();
  bool getMiddle();

  void setMatrix(Cairo::Matrix);

private:
  Glib::RefPtr<Gdk::Device> mouse;
  Gtk::Widget* widget;

  double x, y;
  double ox, oy;
  Gdk::ModifierType modifier;

  Cairo::Matrix matrix;
};
