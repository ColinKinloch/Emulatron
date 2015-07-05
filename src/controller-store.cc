#include "controller-store.hh"
#include <iostream>

ControllerStore::ControllerStore(const ControllerModel::ColumnRecord& columns):
  Gtk::TreeStore()
{
  set_column_types(columns);
  
  int n = SDL_NumJoysticks();
  for(int i = 0; i<n; i++)
  {
    new Controller(i);
  }
  std::cout<<"Controllers!"<<std::endl;
  for(auto& it : Controller::controllers)
  {
    Gtk::TreeModel::Row row = *(append());
    try
    {
      const char* cname = it.second->getName();
      Glib::ustring name = Glib::ustring(cname?cname:"Unknown");
      row[col.cont] = it.second;
      row[col.name] = name;
      row[col.attached] = SDL_GameControllerGetAttached(it.second->getSDLGameController());
      std::cout<<"Controller:"<<name<<std::endl;
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
