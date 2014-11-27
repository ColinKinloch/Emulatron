#include "controller-store.hh"
#include <iostream>

ControllerStore::ControllerStore(const ControllerModel::ColumnRecord& columns):
  Gtk::TreeStore()
{
  set_column_types(columns);
  
  //get SDL controllers
  int joyNum = SDL_NumJoysticks();
  std::cout<<joyNum<<" Joysticks"<<std::endl;
  for(int i=0; i<joyNum; ++i)
  {
    Gtk::TreeModel::Row row = *(append());
    try
    {
      SDL_GameController* cont = SDL_GameControllerOpen(i);
      const char* cname = SDL_GameControllerName(cont);
      Glib::ustring name = Glib::ustring(cname?cname:"Unknown");
      row[col.sdl] = cont;
      row[col.name] = name;
      row[col.attached] = SDL_GameControllerGetAttached(cont);
      std::cout<<"Joystick "<<i<<": "<<name<<std::endl;
    }
    catch(std::logic_error& error)
    {
      std::cerr<<"Controller error"<<error.what()<<std::endl;
    }
  }
}
ControllerStore::ControllerStore():
  ControllerStore::ControllerStore(col)
{}
ControllerStore::~ControllerStore()
{}

Glib::RefPtr<ControllerStore> ControllerStore::create(const ControllerModel::ColumnRecord& columns)
{
  return Glib::RefPtr<ControllerStore>(new ControllerStore(columns));
}
Glib::RefPtr<ControllerStore> ControllerStore::create()
{
  return Glib::RefPtr<ControllerStore>(new ControllerStore());
}

void ControllerStore::update()
{
  
}
void ControllerStore::update(ControllerModel::iterator iter)
{
  
}
