#include "game-icon-view.hh"
#include <iostream>

GameIconView::GameIconView(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder)
  : Gtk::IconView(cobject)
{
  store = GameStore::create();
  
  set_model(store);
  /*set_text_column(store->col.title);
  set_pixbuf_column(store->col.thumbnail);
  */
  targets = {
    Gtk::TargetEntry("text/uri-list")
  };
  drag_dest_set(targets);
}
GameIconView::~GameIconView()
{}

void GameIconView::set_model(Glib::RefPtr<GameStore> model)
{
  store = model;
  Gtk::IconView::set_model(store);
}
Glib::RefPtr<GameStore> GameIconView::get_model()
{
  return store;
}

void GameIconView::on_item_activated(const Gtk::TreeModel::Path& path)
{
  auto iter = get_model()->get_iter(path);
  if(iter)
  {
    Gtk::TreeModel::Row row = *iter;
    std::cout<<"Begin game: "<<row[store->col.filename]<<std::endl;
    
  }
}
bool GameIconView::on_drag_motion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
  //std::cout<<"motion"<<std::endl;
  return false;
}
bool GameIconView::on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
  if(context->list_targets().size())
  {
    std::cout<<"Dropped"<<std::endl;
  }
  return false;
}
void GameIconView::on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time)
{
  auto uris = selection_data.get_uris();
  bool finished = true;
  for(auto it = uris.begin(); it != uris.end(); ++it)
  {
    std::string uri = *it;
    if(!get_model()->add(uri))
    {
      finished = false;
    }
  }
  context->drag_finish(finished, false, time);
}
