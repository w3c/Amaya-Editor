#ifndef __MY_FILESEL_H__
#define __MY_FILESEL_H__

#include <gdk/gdk.h>
#include <gtk/gtkframe.h>

#define GTK_TYPE_MY_FILE_SELECTION            (gtk_my_file_selection_get_type ())
#define GTK_MY_FILE_SELECTION(obj)            (GTK_CHECK_CAST ((obj), GTK_TYPE_MY_FILE_SELECTION, GtkMyFileSelection))
#define GTK_MY_FILE_SELECTION_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GTK_TYPE_MY_FILE_SELECTION, GtkMyFileSelectionClass))
#define GTK_IS_MY_FILE_SELECTION(obj)         (GTK_CHECK_TYPE ((obj), GTK_TYPE_MY_FILE_SELECTION))
#define GTK_IS_MY_FILE_SELECTION_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_MY_FILE_SELECTION))
/* Constant used to know the type of the  dialog box */
#define SAVE 1
#define NEW 2
#define BACKGROUND_IMAGE 3
typedef struct _GtkMyFileSelection       GtkMyFileSelection;
typedef struct _GtkMyFileSelectionClass  GtkMyFileSelectionClass;

struct _GtkMyFileSelection
{
  /* Widgets used for all kind of file_selection dialog box */
  GtkFrame frame;


  GtkWidget *filter_label;
  GtkWidget *filter_entry;
  GtkWidget *filter_button;
  GtkWidget *clear_button;
  GtkWidget *address_label;
  GtkWidget *filter_hbox;
  GtkWidget *dir_list;
  GtkWidget *file_list;
  GtkWidget *selection_entry;
  GtkWidget *selection_text;
  GtkWidget *main_vbox;
  GtkWidget *ok_button;
  GtkWidget *cancel_button;
  GtkWidget *help_button;
  GtkWidget *history_pulldown;
  GtkWidget *history_menu;
  GList     *history_list;
  GtkWidget *fileop_dialog;
  GtkWidget *fileop_entry;
  gchar     *fileop_file;
  gpointer   cmpl_state;
  GtkWidget *fileop_c_dir;
  GtkWidget *fileop_del_file;
  GtkWidget *fileop_ren_file;
  GtkWidget *button_area;
  GtkWidget *action_area;
  /* Specific widgets used for "save as" like dialog boxes */
  GtkWidget *html_radiobutton;
  GtkWidget *xhtml_radiobutton;
  GtkWidget *text_radiobutton;
  GSList *saved_format;
  GtkWidget *copy_image_checkbutton;
  GtkWidget *transform_URL_checkbutton;
  GtkWidget *image_directory_label;
  GtkWidget *image_directory_entry;
  /* Specific widgets used for "Background image" like dialog boxes */ 
  GtkWidget *repeat_radiobutton;
  GtkWidget *repeat_x_radiobutton;
  GtkWidget *repeat_y_radiobutton;
  GtkWidget *no_repeat_radiobutton;
};

struct _GtkMyFileSelectionClass
{
  GtkFrameClass parent_class;
};

GtkType    gtk_my_file_selection_get_type            (void);
GtkWidget* gtk_my_file_selection_new                 (const gchar      *title);
void       gtk_my_file_selection_set_filename        (GtkMyFileSelection *filesel,
						   const gchar      *filename);
gchar*     gtk_my_file_selection_get_filename        (GtkMyFileSelection *filesel);
void	   gtk_my_file_selection_complete		  (GtkMyFileSelection *filesel,
						   const gchar	    *pattern);
void       gtk_my_file_selection_show_fileop_buttons (GtkMyFileSelection *filesel);
void       gtk_my_file_selection_hide_fileop_buttons (GtkMyFileSelection *filesel);

/* Variable used to know which kind of dialog box to build */
int dialog_type;
/* Variable used to identify a dialog box widget in order to destroy it */
gpointer dialog_box;

#endif /* __MY_FILESEL_H__ */







