#include "mouse.hh"

#include <gtkmm.h>
#include <iostream>

Mouse::Mouse(Gtk::Widget* wid)
{
  widget = wid;
  mouse = Gdk::Display::get_default()->get_device_manager()->get_client_pointer();
  matrix = Cairo::identity_matrix();
  x = 0;
  y = 0;
}
void Mouse::setMatrix(Cairo::Matrix mat)
{
  matrix = mat;
}
void Mouse::update()
{
  const Glib::RefPtr<Gdk::Window> win = widget->get_window();
  ox = x;
  oy = y;
  int px, py;
  //mouse->grab(win, Gdk::GrabOwnership::OWNERSHIP_WINDOW, true, Gdk::EventMask::ALL_EVENTS_MASK,/* Gdk::Cursor::create(Gdk::CursorType::BLANK_CURSOR),*/ gtk_get_current_event_time());

  //Axis Rel x/y/wheel
  /*std::vector<std::string> axis;
  axis = mouse->list_axes();
  Gdk::ModifierType mod;
  double *axes = new double[mouse->get_n_axes()];
  mouse->get_state(win, *axes, mod);*/

  /*auto devices =  mouse->list_slave_devices();
  for(auto d: devices)
  {
    std::vector<std::string> axis;
    axis = d->list_axes();
    std::cout<<d->get_name()<<std::endl;
    for(auto a: axis)
    {
      std::cout<<a<<std::endl;
    }
    std::cout<<std::endl;
  }*/

  /*double *axes = new double[mouse->get_n_axes()];
  //double* i = new double[mouse->get_n_axes()];
  double ax = 0, ay = 0, aw = 0;
  Gdk::ModifierType mod;
  mouse->get_state(win, *axes, mod);
  mouse->set_axis_use(0, Gdk::AxisUse::AXIS_X);
  mouse->set_axis_use(1, Gdk::AxisUse::AXIS_Y);
  mouse->set_axis_use(2, Gdk::AxisUse::AXIS_WHEEL);
  mouse->get_axis(*axes, Gdk::AxisUse::AXIS_X, ax);
  mouse->get_axis(*axes, Gdk::AxisUse::AXIS_Y, ay);
  mouse->get_axis(*axes, Gdk::AxisUse::AXIS_WHEEL, aw);
  std::cout<<ax<<":"<<ay<<":"<<aw<<std::endl;*/


  if(widget->get_realized())
  {
    widget->get_window()->get_device_position(mouse, px, py, modifier);
    x = px;
    y = py;
    matrix.transform_point(x, y);
  }
  else
  {
    x = ox;
    y = oy;
  }
}

int Mouse::getX()
{
  return x;
}
int Mouse::getY()
{
  return y;
}
int Mouse::getdX()
{
  return round(x) - round(ox);
}
int Mouse::getdY()
{
  return round(y) - round(oy);
}
bool Mouse::getLeft()
{
  return modifier & Gdk::BUTTON1_MASK;
}
bool Mouse::getRight()
{
  return modifier & Gdk::BUTTON2_MASK;
}
bool Mouse::getMiddle()
{
  return modifier & Gdk::BUTTON3_MASK;
}
