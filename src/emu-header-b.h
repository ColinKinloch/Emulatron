#pragma once

#include <gtk/gtkbox.h>

G_BEGIN_DECLS
#define EMU_TYPE_HEADER_BAR                 (emu_header_bar_get_type ())
#define EMU_HEADER_BAR(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), EMU_TYPE_HEADER_BAR, EmuHeaderBar))
#define EMU_HEADER_BAR_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), EMU_TYPE_HEADER_BAR, EmuHeaderBarClass))
#define EMU_IS_HEADER_BAR(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EMU_TYPE_HEADER_BAR))
#define EMU_IS_HEADER_BAR_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), EMU_TYPE_HEADER_BAR))
#define EMU_HEADER_BAR_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), EMU_TYPE_HEADER_BAR, EmuHeaderBarClass))

typedef struct _EmuHeaderBar        EmuHeaderBar;
typedef struct _EmuHeaderBarPrivate EmuHeaderBarPrivate;
typedef struct _EmuHeaderBarClass   EmuHeaderBarClass;

struct _EmuHeaderBar
{
  /*< private >*/
  GtkHeaderBar parent;

  EmuHeaderBarPrivate *priv;
};

struct _EmuHeaderBarClass
{
  GtkHeaderBarClass parent_class;
};

GType           emu_header_bar_get_type              (void) G_GNUC_CONST;
GtkWidget*      emu_header_bar_new                   (void);
void            emu_header_bar_set_search_mode       (EmuHeaderBar *bar,
							  gboolean          search_mode);
gboolean        emu_header_bar_get_search_mode       (EmuHeaderBar *bar);
void            emu_header_bar_set_select_mode       (EmuHeaderBar *bar,
							  gboolean          select_mode);
gboolean        emu_header_bar_get_select_mode       (EmuHeaderBar *bar);
void            emu_header_bar_set_title             (EmuHeaderBar *bar,
							  const char       *title);
const char *    emu_header_bar_get_title             (EmuHeaderBar *bar);
void            emu_header_bar_set_subtitle          (EmuHeaderBar *bar,
							  const char       *subtitle);
const char *    emu_header_bar_get_subtitle          (EmuHeaderBar *bar);
void            emu_header_bar_set_search_string     (EmuHeaderBar *bar,
							  const char       *search_string);
const char *    emu_header_bar_get_search_string     (EmuHeaderBar *bar);
void            emu_header_bar_set_n_selected        (EmuHeaderBar *bar,
							  guint             n_selected);
guint           emu_header_bar_get_n_selected        (EmuHeaderBar *bar);
void            emu_header_bar_set_custom_title      (EmuHeaderBar *bar,
							  GtkWidget        *title_widget);
GtkWidget *     emu_header_bar_get_custom_title      (EmuHeaderBar *bar);
void            emu_header_bar_set_select_menu_model (EmuHeaderBar *bar,
							  GMenuModel       *model);
GMenuModel *    emu_header_bar_get_select_menu_model (EmuHeaderBar *bar);
void            emu_header_bar_pack_start            (EmuHeaderBar *bar,
							  GtkWidget        *child);
void            emu_header_bar_pack_end              (EmuHeaderBar *bar,
							  GtkWidget        *child);

G_END_DECLS
