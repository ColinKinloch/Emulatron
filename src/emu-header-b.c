#include "config.h"

#include <gtk/gtk.h>
#include <glib/gi18n-lib.h>

#include "emu-resources.h"
#include "emu-header-b.h"

struct _EmuHeaderBarPrivate {
  /* Template widgets */
  GtkWidget   *search_button;
  GtkWidget   *select_button;
  GtkWidget   *done_button;
  GtkWidget   *back_button;
  GtkWidget   *stack;

  /* Visibility */
  gboolean     show_search_button;
  gboolean     show_select_button;

  /* Modes */
  gboolean     search_mode;
  gboolean     select_mode;

  /* Normal title */
  GtkWidget   *title_label;
  GtkWidget   *subtitle_label;

  /* Custom title */
  GtkWidget   *custom_title;

  /* Search results */
  GtkWidget   *search_results_label;
  char        *search_string;

  /* Selection mode */
  guint        n_selected;
  GtkWidget   *selection_menu_button;
};

G_DEFINE_TYPE_WITH_CODE (EmuHeaderBar, emu_header_bar, GTK_TYPE_HEADER_BAR,
                         G_ADD_PRIVATE (EmuHeaderBar));

enum {
  PROP_0,
  PROP_TITLE,
  PROP_SUBTITLE,
  PROP_SEARCH_STRING,
  PROP_N_SELECTED,
  PROP_SEARCH_MODE,
  PROP_SHOW_SEARCH_BUTTON,
  PROP_SELECT_MODE,
  PROP_SELECT_MODE_AVAILABLE,
  PROP_SHOW_SELECT_BUTTON,
  PROP_SHOW_BACK_BUTTON,
  PROP_CUSTOM_TITLE,
  PROP_SELECT_MENU_MODEL
};

#define DEFAULT_PAGE            "title"
#define CUSTOM_TITLE_PAGE      "custom-title"
#define SEARCH_RESULTS_PAGE     "search-results"
#define SELECTION_PAGE          "select"

static void
change_class (GtkWidget  *widget,
              const char *class,
              gboolean    add)
{
  GtkStyleContext *style;

  style = gtk_widget_get_style_context (widget);
  if (add)
    gtk_style_context_add_class (style, class);
  else
    gtk_style_context_add_class (style, class);
}

static void
update_toolbar_state (EmuHeaderBar *bar)
{
  EmuHeaderBarPrivate *priv = bar->priv;

  if (priv->select_mode)
    {
      gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), SELECTION_PAGE);
      gtk_widget_hide (priv->select_button);
      gtk_widget_show (priv->done_button);

      if (priv->n_selected == 0)
        {
          gtk_button_set_label (GTK_BUTTON (priv->selection_menu_button), _("Click on items to select them"));
        }
      else
        {
          const char *format;
          char *label;

          format = g_dngettext (GETTEXT_PACKAGE, "%d selected", "%d selected", priv->n_selected);
          label = g_strdup_printf (format, priv->n_selected);
          gtk_button_set_label (GTK_BUTTON (priv->selection_menu_button), label);
          g_free (label);
        }

      change_class (GTK_WIDGET (bar), "selection-mode", TRUE);
    }
  else if (priv->search_mode)
    {
      if (!priv->search_string || *priv->search_string == '\0')
        {
          if (priv->custom_title)
            gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), CUSTOM_TITLE_PAGE);
          else
            gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), DEFAULT_PAGE);
        }
      else
        {
          char *label;

          gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), SEARCH_RESULTS_PAGE);
          label = g_strdup_printf (_("Results for “%s”"), priv->search_string);

          gtk_label_set_label (GTK_LABEL (priv->search_results_label), label);
          g_free (label);
        }

      if (priv->show_select_button)
        gtk_widget_show (priv->select_button);
      gtk_widget_hide (priv->done_button);

      change_class (GTK_WIDGET (bar), "selection-mode", FALSE);
    }
  else
    {
      if (priv->custom_title == NULL)
        gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), DEFAULT_PAGE);
      else
        gtk_stack_set_visible_child_name (GTK_STACK (priv->stack), CUSTOM_TITLE_PAGE);

      if (priv->show_select_button)
        gtk_widget_show (priv->select_button);
      gtk_widget_hide (priv->done_button);
      if (priv->show_search_button)
        gtk_widget_show (priv->search_button);

      change_class (GTK_WIDGET (bar), "selection-mode", FALSE);
    }
}

static void
done_button_clicked_cb (GtkButton        *button,
                        EmuHeaderBar *bar)
{
  emu_header_bar_set_select_mode (bar, FALSE);
}

static void
back_button_clicked_cb (GtkButton        *button,
                        EmuHeaderBar *bar)
{
  g_signal_emit_by_name (G_OBJECT (bar), "back-clicked", NULL);
}

static void
emu_header_bar_set_property (GObject         *object,
                                 guint            prop_id,
                                 const GValue    *value,
                                 GParamSpec      *pspec)
{
  EmuHeaderBar *bar = EMU_HEADER_BAR (object);
  EmuHeaderBarPrivate *priv = bar->priv;

  switch (prop_id)
    {
    case PROP_TITLE:
      emu_header_bar_set_title (bar, g_value_get_string (value));
      break;

    case PROP_SUBTITLE:
      emu_header_bar_set_subtitle (bar, g_value_get_string (value));
      break;

    case PROP_SEARCH_STRING:
      emu_header_bar_set_search_string (bar, g_value_get_string (value));
      break;

    case PROP_N_SELECTED:
      emu_header_bar_set_n_selected (bar, g_value_get_uint (value));
      break;

    case PROP_SEARCH_MODE:
      emu_header_bar_set_search_mode (bar, g_value_get_boolean (value));
      break;

    case PROP_SHOW_SEARCH_BUTTON:
      priv->show_search_button = g_value_get_boolean (value);
      gtk_widget_set_visible (priv->search_button, priv->show_search_button);
      break;

    case PROP_SELECT_MODE:
      emu_header_bar_set_select_mode (bar, g_value_get_boolean (value));
      break;

    case PROP_SHOW_SELECT_BUTTON:
      priv->show_select_button = g_value_get_boolean (value);
      gtk_widget_set_visible (priv->select_button, priv->show_select_button);
      break;

    case PROP_SHOW_BACK_BUTTON:
      gtk_widget_set_visible (priv->back_button, g_value_get_boolean (value));
      break;

    case PROP_CUSTOM_TITLE:
      emu_header_bar_set_custom_title (bar, g_value_get_object (value));
      break;

    case PROP_SELECT_MENU_MODEL:
      emu_header_bar_set_select_menu_model (bar, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
emu_header_bar_get_property (GObject         *object,
                                 guint            prop_id,
                                 GValue          *value,
                                 GParamSpec      *pspec)
{
  EmuHeaderBar *bar = EMU_HEADER_BAR (object);
  EmuHeaderBarPrivate *priv = bar->priv;

  switch (prop_id)
    {
    case PROP_TITLE:
      g_value_set_string (value, gtk_header_bar_get_title (GTK_HEADER_BAR (object)));
      break;

    case PROP_SUBTITLE:
      g_value_set_string (value, gtk_header_bar_get_subtitle (GTK_HEADER_BAR (object)));
      break;

    case PROP_SEARCH_STRING:
      g_value_set_string (value, emu_header_bar_get_search_string (bar));
      break;

    case PROP_N_SELECTED:
      g_value_set_uint (value, emu_header_bar_get_n_selected (bar));
      break;

    case PROP_SEARCH_MODE:
      g_value_set_boolean (value, emu_header_bar_get_search_mode (bar));
      break;

    case PROP_SHOW_SEARCH_BUTTON:
      g_value_set_boolean (value, priv->show_search_button);
      break;

    case PROP_SELECT_MODE:
      g_value_set_boolean (value, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->select_button)));
      break;

    case PROP_SHOW_SELECT_BUTTON:
      g_value_set_boolean (value, priv->show_select_button);
      break;

    case PROP_SHOW_BACK_BUTTON:
      g_value_set_boolean (value, gtk_widget_get_visible (priv->back_button));
      break;

    case PROP_CUSTOM_TITLE:
      g_value_set_object (value, priv->custom_title);
      break;

    case PROP_SELECT_MENU_MODEL:
      g_value_set_object (value, emu_header_bar_get_select_menu_model (bar));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
emu_header_bar_finalize (GObject *object)
{
  EmuHeaderBar *bar = EMU_HEADER_BAR (object);

  g_free (bar->priv->search_string);

  G_OBJECT_CLASS (emu_header_bar_parent_class)->finalize (object);
}

static void
emu_header_bar_class_init (EmuHeaderBarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = emu_header_bar_finalize;
  object_class->set_property = emu_header_bar_set_property;
  object_class->get_property = emu_header_bar_get_property;

  g_object_class_install_property (object_class,
                                   PROP_TITLE,
                                   g_param_spec_string ("title",
                                                        "Title",
                                                        "The title",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_SUBTITLE,
                                   g_param_spec_string ("subtitle",
                                                        "Subtitle",
                                                        "The subtitle",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_SEARCH_STRING,
                                   g_param_spec_string ("search-string",
                                                        "Search String",
                                                        "The search string used in search mode",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_N_SELECTED,
                                   g_param_spec_uint ("n-selected",
                                                      "Number of Selected Items",
                                                      "The number of selected items",
                                                      0,
                                                      G_MAXUINT,
                                                      0,
                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_SEARCH_MODE,
                                   g_param_spec_boolean ("search-mode",
                                                         "Search Mode",
                                                         "Whether the header bar is in search mode",
                                                         FALSE,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_SHOW_SEARCH_BUTTON,
                                   g_param_spec_boolean ("show-search-button",
                                                         "Show Search Button",
                                                         "Whether the search button is visible",
                                                         FALSE,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_SELECT_MODE,
                                   g_param_spec_boolean ("select-mode",
                                                         "Select Mode",
                                                         "Whether the header bar is in select mode",
                                                         FALSE,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_SHOW_SELECT_BUTTON,
                                   g_param_spec_boolean ("show-select-button",
                                                         "Show Select Button",
                                                         "Whether the select button is visible",
                                                         FALSE,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_SHOW_BACK_BUTTON,
                                   g_param_spec_boolean ("show-back-button",
                                                         "Show Back Button",
                                                         "Whether the back button is visible",
                                                         FALSE,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_CUSTOM_TITLE,
                                   g_param_spec_object ("custom-title",
                                                        "Custom Title",
                                                        "Custom title widget to display",
                                                        GTK_TYPE_WIDGET,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_SELECT_MENU_MODEL,
                                   g_param_spec_object ("select-menu-model",
                                                        "menu-model",
                                                        "The selection dropdown menu's model.",
                                                        G_TYPE_MENU_MODEL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_signal_new ("back-clicked",
                G_OBJECT_CLASS_TYPE (klass),
                0,
                0,
                NULL, NULL,
                g_cclosure_marshal_generic,
                G_TYPE_NONE, 0, G_TYPE_NONE);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/colinkinloch/emulatron/emu-header-bar.ui");
  gtk_widget_class_bind_template_child_private (widget_class, EmuHeaderBar, search_button);
  gtk_widget_class_bind_template_child_private (widget_class, EmuHeaderBar, select_button);
  gtk_widget_class_bind_template_child_private (widget_class, EmuHeaderBar, selection_menu_button);
  gtk_widget_class_bind_template_child_private (widget_class, EmuHeaderBar, done_button);
  gtk_widget_class_bind_template_child_private (widget_class, EmuHeaderBar, back_button);
  gtk_widget_class_bind_template_child_private (widget_class, EmuHeaderBar, stack);
}

static GtkWidget *
create_title_box (const char *title,
                  const char *subtitle,
                  GtkWidget **ret_title_label,
                  GtkWidget **ret_subtitle_label)
{
  GtkWidget *label_box;

  GtkWidget *title_label;
  GtkWidget *subtitle_label;
  GtkStyleContext *context;

  label_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_valign (label_box, GTK_ALIGN_CENTER);
  gtk_widget_show (label_box);

  title_label = gtk_label_new (title);
  context = gtk_widget_get_style_context (title_label);
  gtk_style_context_add_class (context, "title");
  gtk_label_set_line_wrap (GTK_LABEL (title_label), FALSE);
  gtk_label_set_single_line_mode (GTK_LABEL (title_label), TRUE);
  gtk_label_set_ellipsize (GTK_LABEL (title_label), PANGO_ELLIPSIZE_END);
  gtk_box_pack_start (GTK_BOX (label_box), title_label, FALSE, FALSE, 0);
  gtk_widget_show (title_label);

  subtitle_label = gtk_label_new (subtitle);
  context = gtk_widget_get_style_context (subtitle_label);
  gtk_style_context_add_class (context, "subtitle");
  gtk_style_context_add_class (context, "dim-label");
  gtk_label_set_line_wrap (GTK_LABEL (subtitle_label), FALSE);
  gtk_label_set_single_line_mode (GTK_LABEL (subtitle_label), TRUE);
  gtk_label_set_ellipsize (GTK_LABEL (subtitle_label), PANGO_ELLIPSIZE_END);
  gtk_box_pack_start (GTK_BOX (label_box), subtitle_label, FALSE, FALSE, 0);
  gtk_widget_set_no_show_all (subtitle_label, TRUE);

  if (ret_title_label)
    *ret_title_label = title_label;
  if (ret_subtitle_label)
    *ret_subtitle_label = subtitle_label;

  return label_box;
}

static void
emu_header_bar_init (EmuHeaderBar *bar)
{
  GtkWidget *title_widget;

  bar->priv = emu_header_bar_get_instance_private (bar);

  gtk_widget_init_template (GTK_WIDGET (bar));

  gtk_widget_set_no_show_all (bar->priv->search_button, TRUE);
  gtk_widget_set_no_show_all (bar->priv->select_button, TRUE);

  /* Back button */
  g_signal_connect (G_OBJECT (bar->priv->back_button), "clicked",
                    G_CALLBACK (back_button_clicked_cb), bar);

  /* Titles */
  title_widget = create_title_box ("", "", &bar->priv->title_label, &bar->priv->subtitle_label);
  gtk_stack_add_named (GTK_STACK (bar->priv->stack), title_widget, DEFAULT_PAGE);
  /* Custom title page will be added as needed in _set_custom_title() */

  title_widget = create_title_box ("Results", NULL, &bar->priv->search_results_label, NULL);
  gtk_stack_add_named (GTK_STACK (bar->priv->stack), title_widget, SEARCH_RESULTS_PAGE);
  /* The drop-down is added using _set_select_menu_model() */

  /* Select and Search buttons */
  g_signal_connect (G_OBJECT (bar->priv->done_button), "clicked",
                    G_CALLBACK (done_button_clicked_cb), bar);
  g_object_bind_property (bar->priv->search_button, "active",
                            bar, "search-mode", 0);
  g_object_bind_property (bar->priv->select_button, "active",
                            bar, "select-mode", 0);
};

GtkWidget *
emu_header_bar_new (void)
{
  return GTK_WIDGET (g_object_new (EMU_TYPE_HEADER_BAR, NULL));
}

void
emu_header_bar_set_search_mode (EmuHeaderBar *bar,
                                    gboolean          search_mode)
{
  g_return_if_fail (EMU_IS_HEADER_BAR (bar));

  if (bar->priv->search_mode == search_mode)
    return;

  bar->priv->search_mode = search_mode;
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (bar->priv->search_button),
                                bar->priv->search_mode);
  update_toolbar_state (bar);
  if (search_mode == FALSE)
    emu_header_bar_set_search_string (bar, "");
  g_object_notify (G_OBJECT (bar), "search-mode");
}

gboolean
emu_header_bar_get_search_mode (EmuHeaderBar *bar)
{
  g_return_val_if_fail (EMU_IS_HEADER_BAR (bar), FALSE);

  return bar->priv->search_mode;
}

void
emu_header_bar_set_select_mode (EmuHeaderBar *bar,
                                    gboolean          select_mode)
{
  g_return_if_fail (EMU_IS_HEADER_BAR (bar));

  if (bar->priv->select_mode == select_mode)
    return;

  bar->priv->select_mode = select_mode;
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (bar->priv->select_button),
                                bar->priv->select_mode);
  update_toolbar_state (bar);
  g_object_notify (G_OBJECT (bar), "select-mode");
}

gboolean
emu_header_bar_get_select_mode (EmuHeaderBar *bar)
{
  g_return_val_if_fail (EMU_IS_HEADER_BAR (bar), FALSE);

  return bar->priv->select_mode;
}

void
emu_header_bar_set_title (EmuHeaderBar *bar,
                              const char       *title)
{
  g_return_if_fail (EMU_IS_HEADER_BAR (bar));

  gtk_label_set_text (GTK_LABEL (bar->priv->title_label), title);
  gtk_header_bar_set_title (GTK_HEADER_BAR (bar), title);
}

const char *
emu_header_bar_get_title (EmuHeaderBar *bar)
{
  g_return_val_if_fail (EMU_IS_HEADER_BAR (bar), NULL);

  return gtk_header_bar_get_title (GTK_HEADER_BAR (bar));
}

void
emu_header_bar_set_subtitle (EmuHeaderBar *bar,
                                 const char       *subtitle)
{
  g_return_if_fail (EMU_IS_HEADER_BAR (bar));

  gtk_label_set_text (GTK_LABEL (bar->priv->subtitle_label), subtitle);
  gtk_header_bar_set_subtitle (GTK_HEADER_BAR (bar), subtitle);
}

const char *
emu_header_bar_get_subtitle (EmuHeaderBar *bar)
{
  g_return_val_if_fail (EMU_IS_HEADER_BAR (bar), NULL);

  return gtk_header_bar_get_subtitle (GTK_HEADER_BAR (bar));
}

void
emu_header_bar_set_search_string (EmuHeaderBar *bar,
                                      const char       *search_string)
{
  char *tmp;

  g_return_if_fail (EMU_IS_HEADER_BAR (bar));

  tmp = bar->priv->search_string;
  bar->priv->search_string = g_strdup (search_string);
  g_free (tmp);

  update_toolbar_state (bar);
  g_object_notify (G_OBJECT (bar), "search-string");
}

const char *
emu_header_bar_get_search_string (EmuHeaderBar *bar)
{
  g_return_val_if_fail (EMU_IS_HEADER_BAR (bar), NULL);

  return bar->priv->search_string;
}

void
emu_header_bar_set_n_selected (EmuHeaderBar *bar,
                                   guint             n_selected)
{
  g_return_if_fail (EMU_IS_HEADER_BAR (bar));

  if (bar->priv->n_selected == n_selected)
    return;

  bar->priv->n_selected = n_selected;

  update_toolbar_state (bar);
  g_object_notify (G_OBJECT (bar), "n-selected");
}

guint
emu_header_bar_get_n_selected (EmuHeaderBar *bar)
{
  g_return_val_if_fail (EMU_IS_HEADER_BAR (bar), 0);

  return bar->priv->n_selected;
}

void
emu_header_bar_pack_start (EmuHeaderBar *bar,
                               GtkWidget        *child)
{
  g_return_if_fail (EMU_IS_HEADER_BAR (bar));

  gtk_header_bar_pack_start (GTK_HEADER_BAR (bar), child);
}

void
emu_header_bar_pack_end (EmuHeaderBar *bar,
                           GtkWidget          *child)
{
  g_return_if_fail (EMU_IS_HEADER_BAR (bar));

  gtk_header_bar_pack_end (GTK_HEADER_BAR (bar), child);
}

void
emu_header_bar_set_select_menu_model (EmuHeaderBar *bar,
                                          GMenuModel       *model)
{
  g_return_if_fail (EMU_IS_HEADER_BAR (bar));

  gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (bar->priv->selection_menu_button), model);
}

GMenuModel *
emu_header_bar_get_select_menu_model (EmuHeaderBar *bar)
{
  g_return_val_if_fail (EMU_IS_HEADER_BAR (bar), NULL);

  return gtk_menu_button_get_menu_model (GTK_MENU_BUTTON (bar->priv->selection_menu_button));
}

void
emu_header_bar_set_custom_title (EmuHeaderBar *bar,
                                     GtkWidget        *title_widget)
{
  EmuHeaderBarPrivate *priv;

  g_return_if_fail (EMU_IS_HEADER_BAR (bar));
  if (title_widget)
    g_return_if_fail (GTK_IS_WIDGET (title_widget));

  priv = bar->priv;

  /* No need to do anything if the custom widget stays the same */
  if (priv->custom_title == title_widget)
    return;

  if (priv->custom_title)
    {
      GtkWidget *custom = priv->custom_title;

      priv->custom_title = NULL;
      gtk_container_remove (GTK_CONTAINER (bar->priv->stack), custom);
    }

  if (title_widget != NULL)
    {
      priv->custom_title = title_widget;

      gtk_stack_add_named (GTK_STACK (bar->priv->stack), title_widget, CUSTOM_TITLE_PAGE);
      gtk_widget_show (title_widget);

      update_toolbar_state (bar);
    }
  else
    {
      gtk_stack_set_visible_child_name (GTK_STACK (bar->priv->stack), DEFAULT_PAGE);
    }

  g_object_notify (G_OBJECT (bar), "custom-title");
}

GtkWidget *
emu_header_bar_get_custom_title (EmuHeaderBar *bar)
{
  g_return_val_if_fail (EMU_IS_HEADER_BAR (bar), NULL);

  return bar->priv->custom_title;
}
