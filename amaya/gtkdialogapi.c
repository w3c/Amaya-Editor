#ifdef _GTK
/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#include "gtkdialogapi.h"
#include "gtkbrowser.h"

/*
 * Global variables 
 */

gchar *selected_filename;
GtkWidget *file_selector;









void store_filename(GtkFileSelection *selector, gpointer user_data)
{
  selected_filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION(file_selector));
}

/*----------------------------------------------------------------
  Function that creates a "New HTML document" or a "New CSS style
  sheet " dialog box. 
  Callback function attached to widget of this dialog box are
  after this function
  ----------------------------------------------------------------*/
void CreateOpenDocDlgGTK (gchar *title)
{
    /* Create the selector */    
    file_selector = gtk_file_selection_new("Please select a file for editing.");
    
    gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->ok_button),
                            "clicked", GTK_SIGNAL_FUNC (store_filename), NULL);
                            
    /* Ensure that the dialog box is destroyed when the user clicks a button. */
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->ok_button),
                                           "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy),
                                           (gpointer) file_selector);
    gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->cancel_button),
                                           "clicked", GTK_SIGNAL_FUNC (gtk_widget_destroy),
                                           (gpointer) file_selector);
    
    /* Display that dialog */    
    gtk_widget_show (file_selector);

    return;


    /*
  GtkWidget *dialog_new;
  GtkWidget *dialog_vbox_new;
  GtkWidget *file_selection_frame;

  dialog_new = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog_new),title );
  gtk_window_set_policy (GTK_WINDOW (dialog_new), FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog_new), "destroy",
		     GTK_SIGNAL_FUNC (gtk_widget_destroy), NULL);
  dialog_vbox_new = GTK_DIALOG (dialog_new)->vbox;
  gtk_widget_show (dialog_vbox_new);
 
  dialog_type = NEW;
  dialog_box = dialog_new;
  file_selection_frame = gtk_my_file_selection_new ("");
  gtk_box_pack_start (GTK_BOX ( dialog_vbox_new), file_selection_frame, TRUE, TRUE, 0);
  gtk_widget_show (file_selection_frame);

  gtk_widget_show (dialog_new);

    */
}

















#endif
