#ifdef _GTK

#include "gtkdialogue_box.h"
#include "gtkbrowser.h"

/* N'arrivant pas à inclure "thotcolor.h" j'ai redéfini _RGBstruct */
typedef struct _RGBstruct{
  unsigned short red;
  unsigned short green;
  unsigned short blue;
  }RGBstruct;

extern RGBstruct *RGB_Table;
/*-----------------------------------------------------------------
Local variables 
------------------------------------------------------------------*/
/*  of the "Set up and print" dialog box */
static  GtkWidget *entry_print1; 
/*  of the "Style" dialog box */
static GtkWidget *combo_char;
static GtkWidget *combo_font;
static GtkWidget *combo_underline;
static GtkWidget *combo_body_size;
static GtkWidget *entry_format1;
static GtkWidget *entry_format2;
static GdkColor white = {1,0xffff,0xffff, 0xffff};
static GtkWidget *palette_label2;
static Color_previous *color_previous;
/* of the "greek alphabet" dialog box */
static int *greek_index_value = NULL; /* The index value (in "Items_Grec") of the chosen letter */
static int *greek_index[160];
/* of the "Preferences" dialog box */
static GtkWidget *general_entry1;
static GtkWidget *general_entry2;
static GtkWidget *general_entry3;
static GtkWidget *general_entry4;
static GtkWidget *general_entry5;
static GtkWidget *general_checkbutton1;
static GtkWidget *general_checkbutton2;
static GtkWidget *general_checkbutton3;

static GtkWidget *publish_entry1;
static GtkWidget *publish_checkbutton1;
static GtkWidget *publish_checkbutton2;

static GtkWidget *cache_entry1;
static GtkWidget *cache_entry2;
static GtkWidget *cache_entry3;
static GtkWidget *cache_checkbutton1;
static GtkWidget *cache_checkbutton2;
static GtkWidget *cache_checkbutton3;
static GtkWidget *cache_checkbutton4;

static GtkWidget *proxy_entry1;
static GtkWidget *proxy_entry2;

static GtkWidget *color_entry1;
static GtkWidget *color_entry2;
static GtkWidget *color_entry3;
static GtkWidget *color_entry4;

static GtkWidget *language_entry1;

/* of the search dialog box */
static  GtkWidget *entry_search1;
static  GtkWidget *entry_search2;

/* of the spell dialog box */
static GtkWidget *entry_spell1;
static GtkWidget *entry_spell2;
static GtkWidget *entry_spell3;
static GtkWidget *label_spell1;
static GtkWidget *clist_spell1;

/* of the table  dialog box */
static GtkWidget *table_entry1;
static GtkWidget *table_entry2;
static GtkWidget *table_entry3;

/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Set up and print" dialog box 
----------------------------------------------------------------*/
static  void Initialisation_print ()
{
  
  print_form = (Print_form *)TtaGetMemory (sizeof (Print_form));
  print_form->Printer = TRUE;
  print_form->A4_not_US = TRUE;
  print_form->output = "lpr";
  print_form->Manual_feed = FALSE;
  print_form->Table_of_contents = FALSE;
  print_form->Table_of_links = FALSE;
  print_form->Print_URL = TRUE;
}

/*----------------------- Callback functions -----------------------------**/
/*--------------------------------------------------------------------------
 Callback function called by the "Printer" radio_button of the "Set up and print" dialog box
------------------------------------------------------------------------- */
static void printer_func (GtkWidget *widget, gpointer data )
{
  /* Save the selection of a radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
     
      printf ("%s\n" , print_form->output); 
      print_form->Printer = TRUE;
      print_form->output = "lpr";
      printf ("%s\n" , print_form->output); 
      gtk_entry_set_text (GTK_ENTRY (entry_print1), "lpr");

    }
}
/*--------------------------------------------------------------------------
 Callback function called by the "PostScript file" radio_button of the "Set up and print" dialog box
------------------------------------------------------------------------- */
static void ps_func (GtkWidget *widget, gpointer data)
{
  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      print_form->Printer = FALSE;
      print_form->output = "noname.ps";
      gtk_entry_set_text (GTK_ENTRY (entry_print1), "noname.ps");

    }
}    
/*--------------------------------------------------------------------------
 Callback function called by the "US" radio_button of the "Set up and print" dialog box
------------------------------------------------------------------------- */
static void us_func (GtkWidget *widget, gpointer data)
{
  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      print_form->A4_not_US = FALSE;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "PostScript file" radio_button of the "Set up and print" dialog box
------------------------------------------------------------------------- */
static void a4_func (GtkWidget *widget, gpointer data)
{
  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      print_form->A4_not_US = TRUE;
    }
}       
/*--------------------------------------------------------------------------
 Callback function called by the "Output" text _entry of the "Set up and print" dialog box
------------------------------------------------------------------------- */
static void  entry_func (GtkWidget *widget, gpointer data) 
{
   print_form->output = gtk_entry_get_text (GTK_ENTRY (entry_print1));
   printf ("%s\n" , print_form->output); 
}
/*--------------------------------------------------------------------------
 Callback function called by the "Manual feed" checkbutton of the "Set up and print" dialog box
------------------------------------------------------------------------- */
static void mf_func (GtkWidget *widget, gpointer data) 
{
  if (GTK_TOGGLE_BUTTON (widget)->active) 
    print_form->Manual_feed = TRUE;
  else
    print_form->Manual_feed = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "Table of contents" checkbutton of the "Set up and print" dialog box
------------------------------------------------------------------------- */
static void tc_func (GtkWidget *widget, gpointer data) 
{
   if (GTK_TOGGLE_BUTTON (widget)->active) 
    print_form->Table_of_contents = TRUE;
  else
    print_form->Table_of_contents = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "Table of links" checkbutton of the "Set up and print" dialog box
------------------------------------------------------------------------- */
static void tl_func (GtkWidget *widget, gpointer data) 
{
  if (GTK_TOGGLE_BUTTON (widget)->active) 
    print_form->Table_of_links = TRUE;
  else
    print_form->Table_of_links = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "Print URL" checkbutton of the "Set up and print" dialog box
------------------------------------------------------------------------- */
static void pu_func (GtkWidget *widget, gpointer data) 
{
 if (GTK_TOGGLE_BUTTON (widget)->active) 
    print_form->Print_URL = TRUE;
  else
    print_form->Print_URL = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "Print" button of the "Set up and print" dialog box
------------------------------------------------------------------------- */
static void print_func (GtkWidget *widget, gpointer data) 
{
  printf ("%s\t",  print_form->output );
  printf ("A4_not_U : %d\t",  print_form->A4_not_US);
  printf ("Printer : %d\t", print_form->Printer);
  printf ("Manual_feed :  %d\t",print_form->Manual_feed);
  printf ("Table_of_contents = %d\t",  print_form->Table_of_contents);
  printf ("Table_of_links = %d\t",print_form->Table_of_links);
  printf ("Print_URL = %d\n", print_form->Print_URL);
}
/*--------------------------------------------------------------------------
 Callback function called by the "Cancel" button of the "Set up and print" dialog box
------------------------------------------------------------------------- */
 void cancel_func (GtkWidget *widget, gpointer data)
{
  gtk_widget_destroy (GTK_WIDGET (data));

}

/*---------------------------------------------------------------------------
 Function that creates the "setup and print" dialog 
---------------------------------------------------------------------------*/
GtkWidget*
create_dialog_print ()
{
  GtkWidget *dialog_print1;
  GtkWidget *dialog_vbox_print1;
  GtkWidget *vbox_print1;
  GtkWidget *hbox_print1;
  GtkWidget *label_print1;
  GSList *hbox_print1_group = NULL;
  GtkWidget *radiobutton_print1;
  GtkWidget *radiobutton_print2;
 
 
  GtkWidget *hbox_print2;
  GtkWidget *vbox_print2;
  GtkWidget *frame_print1;
  GtkWidget *frame_print2;
  GtkWidget *label_print2;
  GtkWidget *checkbutton_print1;
  GtkWidget *checkbutton_print2;
  GtkWidget *checkbutton_print3;
  GtkWidget *checkbutton_print4;
  GtkWidget *vbox_print3;
  GtkWidget *label_print3;
  GSList *vbox_print3_group = NULL;
  GtkWidget *radiobutton_print3;
  GtkWidget *radiobutton_print4;
  GtkWidget *dialog_action_area_print1;
  GtkWidget *hbuttonbox_print1;
  GtkWidget *button_print1;
  GtkWidget *button_print2;
  GtkWidget *vbox_print4;
  GtkWidget *label_print4;
 
  Initialisation_print ();
  /* Creation of the dialog box */
  dialog_print1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog_print1), "Print ");
  gtk_window_set_policy (GTK_WINDOW (dialog_print1), FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog_print1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy),GTK_OBJECT (dialog_print1));

  dialog_vbox_print1 = GTK_DIALOG (dialog_print1)->vbox;
  gtk_widget_show (dialog_vbox_print1);

  vbox_print1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_print1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox_print1), vbox_print1, FALSE, FALSE, 0);

  hbox_print1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_print1);
  gtk_box_pack_start (GTK_BOX (vbox_print1), hbox_print1, FALSE, FALSE, 0);
  gtk_container_border_width (GTK_CONTAINER (hbox_print1), 5);

  /* The label of "Output" message */
  label_print1 = gtk_label_new ("Output");
  gtk_misc_set_alignment (GTK_MISC (label_print1), 0.1, 0.5);
  gtk_widget_show (label_print1);
  gtk_box_pack_start (GTK_BOX (hbox_print1), label_print1, FALSE, FALSE, 5);

  /* The "Printer" radiobutton */
  radiobutton_print1 = gtk_radio_button_new_with_label (hbox_print1_group, "Printer");
  hbox_print1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_print1));
  gtk_widget_show (radiobutton_print1);
  gtk_box_pack_start (GTK_BOX (hbox_print1), radiobutton_print1, FALSE, FALSE, 2);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton_print1), TRUE);
  gtk_signal_connect (GTK_OBJECT (radiobutton_print1), "clicked", 
		      GTK_SIGNAL_FUNC (printer_func), NULL);

  /* The "PostScript file" radiobutton */
  radiobutton_print2 = gtk_radio_button_new_with_label (hbox_print1_group, "PostScript file");
  hbox_print1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_print2));
  gtk_widget_show (radiobutton_print2);
  gtk_box_pack_start (GTK_BOX (hbox_print1), radiobutton_print2, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (radiobutton_print2), "clicked", 
		      GTK_SIGNAL_FUNC (ps_func), NULL);

  /* The print command entry */
  entry_print1 = gtk_entry_new ();
  gtk_widget_show (entry_print1);
  gtk_box_pack_start (GTK_BOX (vbox_print1), entry_print1, TRUE, TRUE, 0);
  gtk_entry_set_text (GTK_ENTRY (entry_print1), "lpr");
  gtk_entry_set_position (GTK_ENTRY (entry_print1), 4);
  gtk_signal_connect (GTK_OBJECT (entry_print1), "changed",
		      GTK_SIGNAL_FUNC (entry_func), NULL);

  hbox_print2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_print2);
  gtk_box_pack_start (GTK_BOX (vbox_print1), hbox_print2, FALSE, FALSE, 0);

  /* The "Options" frame */
  frame_print1 = gtk_frame_new ("Options");
  gtk_widget_show (frame_print1);
  gtk_box_pack_start (GTK_BOX (hbox_print2), frame_print1, FALSE, FALSE, 0);

  vbox_print2 = gtk_vbox_new (TRUE,0);
  gtk_widget_show (vbox_print2);
  gtk_container_add (GTK_CONTAINER (frame_print1), vbox_print2);


  /* The "manual feed" checkbutton */
  checkbutton_print1 = gtk_check_button_new_with_label ("Manual feed");
  gtk_widget_show (checkbutton_print1);
  gtk_box_pack_start (GTK_BOX (vbox_print2), checkbutton_print1, FALSE, FALSE, 0);
  gtk_signal_connect (GTK_OBJECT (checkbutton_print1), "clicked",
		      GTK_SIGNAL_FUNC (mf_func),NULL);

  /* The "Table of contents" checkbutton */
  checkbutton_print2 = gtk_check_button_new_with_label ("Table of contents");
  gtk_widget_show (checkbutton_print2);
  gtk_box_pack_start (GTK_BOX (vbox_print2), checkbutton_print2, FALSE, FALSE, 0);
  gtk_signal_connect (GTK_OBJECT (checkbutton_print2), "clicked",
		      GTK_SIGNAL_FUNC (tc_func),NULL);

  /* The "Table of links" checkbutton */
  checkbutton_print3 = gtk_check_button_new_with_label ("Table of links");
  gtk_widget_show (checkbutton_print3);
  gtk_box_pack_start (GTK_BOX (vbox_print2), checkbutton_print3, FALSE, FALSE, 0);
  gtk_signal_connect (GTK_OBJECT (checkbutton_print3), "clicked",
		      GTK_SIGNAL_FUNC (tl_func),NULL);

  /* The "Print URL" checkbutton */
  checkbutton_print4 = gtk_check_button_new_with_label ("Print URL");
  gtk_widget_show (checkbutton_print4);
  gtk_box_pack_start (GTK_BOX (vbox_print2), checkbutton_print4, FALSE, FALSE, 0);
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (checkbutton_print4), TRUE);
  gtk_signal_connect (GTK_OBJECT (checkbutton_print4), "clicked",
		      GTK_SIGNAL_FUNC (pu_func),NULL);


  label_print3 = gtk_label_new ("");
  gtk_widget_show (label_print3);
  gtk_box_pack_start (GTK_BOX (hbox_print2), label_print3, TRUE, TRUE, 0);

  vbox_print4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_print4);
  gtk_box_pack_start (GTK_BOX (hbox_print2), vbox_print4, FALSE, FALSE, 0);
  
  label_print4 = gtk_label_new ("");
  gtk_widget_show (label_print4);
  gtk_box_pack_start (GTK_BOX (vbox_print4), label_print4, TRUE, TRUE, 0);

  /* The "Paper format" frame */
  frame_print2 = gtk_frame_new ("Paper format");
  gtk_widget_show (frame_print2);
  gtk_box_pack_start (GTK_BOX (vbox_print4), frame_print2, FALSE, FALSE, 0);

  vbox_print3 = gtk_vbox_new (TRUE, 0);
  gtk_widget_show (vbox_print3);
  gtk_container_add (GTK_CONTAINER (frame_print2), vbox_print3);

  /* The "A_print4" radiobutton */
  radiobutton_print3 = gtk_radio_button_new_with_label (vbox_print3_group, "A_print4");
  vbox_print3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_print3));
  gtk_widget_show (radiobutton_print3);
  gtk_box_pack_start (GTK_BOX (vbox_print3), radiobutton_print3, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radiobutton_print3), TRUE);
  gtk_signal_connect (GTK_OBJECT (radiobutton_print3), "clicked",
		      GTK_SIGNAL_FUNC (a4_func), NULL);

  /* The "US" radiobutton */
  radiobutton_print4 = gtk_radio_button_new_with_label (vbox_print3_group, "US");
  vbox_print3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_print4));
  gtk_widget_show (radiobutton_print4);
  gtk_box_pack_start (GTK_BOX (vbox_print3), radiobutton_print4, FALSE, FALSE, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_print4), "clicked",
		      GTK_SIGNAL_FUNC (us_func), NULL);


  label_print2 = gtk_label_new ("");
  gtk_widget_show (label_print2);
  gtk_box_pack_start (GTK_BOX (vbox_print4), label_print2, TRUE, TRUE, 0);
 
 

  dialog_action_area_print1 = GTK_DIALOG (dialog_print1)->action_area;
  gtk_widget_show (dialog_action_area_print1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area_print1), 10);

  /* The Printer button box */
  hbuttonbox_print1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox_print1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area_print1), hbuttonbox_print1, TRUE, TRUE, 0);

  /* The "print" button */
  button_print1 = gtk_button_new_with_label ("Print");
  GTK_WIDGET_SET_FLAGS (button_print1, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_print1), button_print1);
  gtk_widget_show (button_print1);
  gtk_widget_grab_default (button_print1);
  gtk_signal_connect (GTK_OBJECT (button_print1), "clicked",
		      GTK_SIGNAL_FUNC (print_func), NULL);
 
 /* The "Cancel" button */
  button_print2 = gtk_button_new_with_label ("Cancel");
  GTK_WIDGET_SET_FLAGS (button_print2, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_print1), button_print2);
  gtk_widget_show (button_print2);
  gtk_signal_connect (GTK_OBJECT (button_print2), "clicked",
		      GTK_SIGNAL_FUNC (cancel_func), dialog_print1);

  return dialog_print1;
}
/*********************************************************************************/
/* End of "Set up and print functions */
/*******************************************************************************/
/*----------------------------------------------------------------
Function that creates a "New HTML document" or a "New CSS style sheet " dialog box. 
Callback function attached to widget of this dialog box are in new_file_selection.c
----------------------------------------------------------------*/
GtkWidget *create_dialog_new (gchar *title)
{
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
 return (dialog_new);
}
/*----------------------------------------------------------------
Function that creates a "Save as" dialog box. 
Callback function attached to widget of this dialog box are in new_file_selection.c
----------------------------------------------------------------*/
GtkWidget *create_dialog_save (gchar * title)
{
  GtkWidget *dialog_save;
  GtkWidget *dialog_vbox_save;
  GtkWidget *file_selection_frame;
 
  dialog_save = gtk_dialog_new ();
 gtk_window_set_title (GTK_WINDOW (dialog_save), title );
 gtk_window_set_policy (GTK_WINDOW (dialog_save), FALSE, TRUE, FALSE);
 gtk_signal_connect (GTK_OBJECT (dialog_save), "destroy",
		     GTK_SIGNAL_FUNC (gtk_widget_destroy), NULL);
 
 dialog_vbox_save = GTK_DIALOG (dialog_save)->vbox;
 gtk_widget_show (dialog_vbox_save);
 
 dialog_type = SAVE;
 dialog_box = dialog_save;
 file_selection_frame = gtk_my_file_selection_new ("");
 gtk_box_pack_start (GTK_BOX ( dialog_vbox_save), file_selection_frame, TRUE, TRUE, 0);
 
 gtk_widget_show (file_selection_frame);
 return (dialog_save);
}
/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Style" dialog box 
----------------------------------------------------------------*/
/* This is an internally used function to set notebook tab widgets. */
void set_notebook_tab (GtkWidget *notebook, gint page_num, GtkWidget *widget)
{
  GtkNotebookPage *page;
  GtkWidget *notebook_page;

  page = (GtkNotebookPage*) g_list_nth (GTK_NOTEBOOK (notebook)->children, page_num)->data;
  notebook_page = page->child;
  gtk_widget_ref (notebook_page);
  gtk_notebook_remove_page (GTK_NOTEBOOK (notebook), page_num);
  gtk_notebook_insert_page (GTK_NOTEBOOK (notebook), notebook_page,
                            widget, page_num);
  gtk_widget_unref (notebook_page);
}
static void Initialisation_style ()/* il faudra faire un free */
{
  STRING temp;
  char_style = (Char_style *)TtaGetMemory(sizeof (Char_style));
  char_style->font_family = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (combo_font)->entry));
  char_style->character = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (combo_char)->entry));
  char_style->underline = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (combo_underline)->entry));
  temp = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (combo_body_size)->entry));
  char_style->body_size = atoi(temp);
 
  format_style = (Format_style *)TtaGetMemory (sizeof (Format_style));
  format_style->align = 1;
  format_style->justification = 2;
  format_style->line_spcg = 2;

  format_style->indent = 1;
  temp = gtk_entry_get_text (GTK_ENTRY (entry_format1));
  format_style->line_spcg_pt = atoi(temp);
  temp = gtk_entry_get_text (GTK_ENTRY (entry_format1));
  format_style->indent_pt = atoi (temp);

  
}  
/* Callback function of the "font family" combo box  of the "character" frame */
void
combo_font_func                          (GtkWidget       *widget,
                                        gpointer         user_data)
{
char_style->font_family = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (combo_font)->entry));
printf ("%s\n",char_style->font_family);
}
/* Callback function of the "character style" combo box  of the "character" frame */
void
combo_char_func                          (GtkWidget       *widget,
                                        gpointer         user_data)
{
  char_style->character = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (combo_char)->entry));
  printf ("%s\n",char_style->character);
}
/* Callback function of the "underline" combo box  of the "character" frame */
void
combo_underline_func                          (GtkWidget       *widget,
                                        gpointer         user_data)
{
 char_style->underline = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (combo_underline)->entry));
 printf ("%s\n",char_style->underline);
}
/* Callback function of the "body size" combo box of the "character" frame*/
void
combo_body_size_func                          (GtkWidget       *widget,
                                        gpointer         user_data)
{
  STRING temp;
  temp = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (combo_body_size)->entry));
  char_style->body_size = atoi(temp);
  printf ("%d\n",char_style->body_size);
}
/* Callback function of the "apply" button of the "character" frame */
void
ch_apply_func                          (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("apply\n");
}
/* Callback function of the "done" button of the "character" frame */

void
ch_done_func                           (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("done\n");
}
/* Callback function called by a colored  button */
void button_press_func (GtkButton       *button,
			GdkEventButton *event,
			gpointer         data)
{
  Color_form *color_inter = (Color_form *)data;
  gchar *str1;
  gchar *str2;
  gchar *str3;

 

  gtk_label_get (GTK_LABEL (color_inter->label), &str1);
 
  
  /* if the left button is pressed then the index of the foreground color is modified */
  if (event->button== 1){
    if (color_previous->label_f !=NULL) {
      gtk_label_get (GTK_LABEL (color_previous->label_f), &str2);
      if (strcmp(str2,"F/B")==0){ 
	gtk_label_set_text (GTK_LABEL (color_previous->label_f), "B");

     }
     else {
	gtk_label_set_text (GTK_LABEL (color_previous->label_f), "");
      }
    }
    if ((strcmp(str1,"F")==0)||(strcmp(str1,"")==0)){ 
	gtk_label_set_text (GTK_LABEL (color_inter->label), "F");
    }
    else if ((strcmp(str1,"B")==0)||(strcmp(str1,"F/B")==0)){ 
      gtk_label_set_text (GTK_LABEL (color_inter->label), "F/B");
    }
    
    color_previous->indice_f = color_inter->indice;
    color_previous->label_f = color_inter->label;
  }

  /* if the middle or right button is pressed then the index of the foreground color is modified */
  if ((event->button== 2)||(event->button ==3))
    {	
      if (color_previous->label_b !=NULL) {
	gtk_label_get (GTK_LABEL (color_previous->label_b), &str3);
	if (strcmp(str3,"F/B")==0){ 
	  gtk_label_set_text (GTK_LABEL (color_previous->label_b), "F");
	}
	else {
	  gtk_label_set_text (GTK_LABEL (color_previous->label_b), "");
	}
      }	

      if ((strcmp(str1,"B")==0)||(strcmp(str1,"")==0)){
	  gtk_label_set_text (GTK_LABEL (color_inter->label), "B");
	}
	else if ((strcmp(str1,"F")==0)||(strcmp(str1,"F/B")==0)){ 
	  gtk_label_set_text (GTK_LABEL (color_inter->label), "F/B");
	}
	
	color_previous->indice_b = color_inter->indice;
	color_previous->label_b  = color_inter->label;	
      }

}
/* Callback function of the "apply" button of the "character" frame */
void
pal_apply_func                          (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("apply\n");
}
/* Callback function of the "done" button of the "character" frame */

void
pal_done_func                           (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("done\n");
}

/* Callback function of the left align radiobutton of the "format" frame */
void
left_align_func                          (GtkWidget       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->align = 1;
}
/* Callback function of the right align radiobutton of the "format" frame */
void
right_align_func                          (GtkWidget       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->align = 2;
}
/* Callback function of the center align radiobutton of the "format" frame */
void
center_align_func                          (GtkWidget       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->align = 3;

}
/* Callback function of the "default" align radiobutton of the "format" frame */
void
default_align_func                          (GtkWidget       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->align = 4;
}
/* Callback of the "Yes" radiobutton  of the "format" frame */
void
yes_jus_func                           (GtkButton       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->justification = 1;
}

/* Callback of the "No" radiobutton of the "format" frame */
void
no_jus_func                            (GtkButton       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->justification = 2;
}

/* Callback of the "Default" radiobutton of the "format" frame */
void
default_jus_func                      (GtkButton       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->justification = 3;
}
/* Callback of the "indent1" radiobutton of the "format" frame */
void
indent1_func                      (GtkButton       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->indent = 1;
}
/* Callback of the "indent2" radiobutton of the "format" frame */
void
indent2_func                      (GtkButton       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->indent = 2;
}
/* Callback of the "Default" radiobutton of the "format" frame */
void
default_indent_func                      (GtkButton       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->indent = 3;
}
/* Callback of the "Indent (pt)" entry  of the "format" frame */
void
indent_pt_func                         (GtkEditable     *editable,
                                        gpointer         user_data)
{
  STRING temp; 
  
  temp = gtk_entry_get_text (GTK_ENTRY (entry_format1));
  format_style->indent_pt = atoi (temp);
}
/* Callback of the "small" radiobutton of the "format" frame */
void
small_line_spcg_func                      (GtkButton       *widget,
                                        gpointer         user_data)
{
 if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->line_spcg = 1;
}
/* Callback of the "medium" radiobutton of the "format" frame */
void
medium_line_spcg_func                      (GtkButton       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->line_spcg = 2;
}
/* Callback of the "large" radiobutton of the "format" frame */
void
large_line_spcg_func                      (GtkButton       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->line_spcg = 3;
}
/* Callback of the "Default" radiobutton of the "format" frame */
void
default_line_spcg_func                      (GtkButton       *widget,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (widget)->active)
    format_style->line_spcg = 4;
}
/* Callback of the "line spacing (pt)" entry  of the "format" frame */
void
line_spcg_pt_func                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
  STRING temp;

  temp = gtk_entry_get_text (GTK_ENTRY (entry_format1));
  format_style->line_spcg_pt = atoi(temp);
}

/* Callback of the "Apply" button of the "format" frame */
void
fo_apply_func                          (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("apply\n");
  printf("%d\n",format_style->align);
  printf("%d\n",format_style->justification);
  printf("%d\n",format_style->line_spcg);
  printf("%d\n",format_style->indent);
  printf("%d\n",format_style->line_spcg_pt);
  printf("%d\n",format_style->indent_pt);

}
/* Callback of the "Done" button of the "format" frame */

void
fo_done_func                           (GtkButton       *button,
                                        gpointer         user_data)
{
  printf ("done\n");
}


GtkWidget *create_dialog_style()
{
  GtkWidget *dialog1;     /* Container widgets */
  GtkWidget *dialog_vbox1;
  GtkWidget *notebook1;
  GtkWidget *vbox1;
  GtkWidget *vbox3;
  GtkWidget *label1; 
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *dialog_action_area1;
 
 
  GtkWidget *label_char1;    /* Widget of the "character style" frame */
  GtkWidget *label_char2;
  GtkWidget *label_char3;
  GtkWidget *label_char4;
  GtkWidget *label_char5;
  GtkWidget *hseparator_char1;
  GtkWidget *hbuttonbox_char1;
  GtkWidget *button_char1;
  GtkWidget *button_char2;
  GList *char_list;
  GList *font_list;
  GList *underline_list;
  GList *body_size_list;
  
  GtkWidget *hbox_char1;
  GtkWidget *hbox_char2;
  GtkWidget *hbox_char3;
  GtkWidget *hbox_char4;

           
 
  GtkWidget *palette_frame;     /* Widget of the "color"  frame */
  GtkWidget *palette_table1;
  GtkWidget *palette_vbox1;
  GtkWidget *palette_label1;
  GtkWidget *palette_button1;
  GtkWidget *palette_button2;
 GtkWidget *palette_button3;
 GtkWidget *palette_hbuttonbox;
 GtkStyle *defstyle;
 GtkStyle *button_style;
 GdkColor button_color;
 int i, j, k, indice;

  GtkWidget *hbox_format1;   /* Widget of the "format"  frame */
  GtkWidget *frame_format1;
  GtkWidget *table_format1;
  GdkPixmap *glade_pixmap;
  GdkBitmap *glade_mask;
  GtkWidget *pixmap_format1;
  GtkWidget *pixmap_format2;
  GtkWidget *pixmap_format3;
  GtkWidget *label_format1;
  GSList *table_format1_group = NULL;
  GtkWidget *radiobutton_format1;
  GtkWidget *radiobutton_format2;
  GtkWidget *radiobutton_format3;
  GtkWidget *radiobutton_format4;
  GtkWidget *frame_format2;
  GtkWidget *table_format2;
  GtkWidget *label_format2;
  GtkWidget *label_format3;
  GtkWidget *label_format4;
  GSList *table_format2_group = NULL;
  GtkWidget *radiobutton_format5;
  GtkWidget *radiobutton_format6;
  GtkWidget *radiobutton_format7;
  GtkWidget *hbox_format2;
  GtkWidget *frame_format4;
  GtkWidget *vbox_format3;
  GtkWidget *table_format3;
  GtkWidget *pixmap_format5;
  GtkWidget *pixmap_format6;
  GtkWidget *pixmap_format7;
  GtkWidget *label_format6;
  GSList *table_format3_group = NULL;
  GtkWidget *radiobutton_format8;
  GtkWidget *radiobutton_format9;
  GtkWidget *radiobutton_format10;
  GtkWidget *radiobutton_format11;
  GtkWidget *hbox_format3;
  GtkWidget *label_format7;
  
  GtkWidget *frame_format5;
  GtkWidget *vbox_format4;
  GtkWidget *table_format4;
  GtkWidget *pixmap_format8;
  GtkWidget *pixmap_format9;
  GtkWidget *label_format8;
  GSList *table_format4_group = NULL;
  GtkWidget *radiobutton_format12;
  GtkWidget *radiobutton_format13;
  GtkWidget *radiobutton_format14;
  GtkWidget *hbox_format4;
  GtkWidget *label_format9;

  GtkWidget *label_format10;
  GtkWidget *hseparator_format1;
  GtkWidget *hbuttonbox_format1;
  GtkWidget *button_format1;
  GtkWidget *button_format2;
 
  GtkWidget *maboite;

  

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), "Document style");
  gtk_window_set_policy (GTK_WINDOW (dialog1), FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy),NULL);

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, TRUE, TRUE, 0);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox1);

  hbox_char1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_char1);
  gtk_container_add (GTK_CONTAINER (vbox1), hbox_char1);

  label_char1 = gtk_label_new ("Font family");
  gtk_widget_show (label_char1);
  gtk_box_pack_start (GTK_BOX (hbox_char1), label_char1, TRUE, TRUE, 2);
  gtk_misc_set_alignment (GTK_MISC (label_char1), 0.1, 0.5);

  font_list = NULL;
  font_list = g_list_append (font_list, "Times");
  font_list = g_list_append (font_list, "Helvetica");
  font_list = g_list_append (font_list, "Courier");
  font_list = g_list_append (font_list, "Default");
 
  combo_font = gtk_combo_new ();
  gtk_widget_set_usize (combo_font, 150, 22);
  gtk_combo_set_popdown_strings (GTK_COMBO (combo_font), font_list);
  gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo_font)->entry), FALSE);
  gtk_combo_set_use_arrows_always( GTK_COMBO (combo_font), 1);
  gtk_signal_connect (GTK_OBJECT (GTK_COMBO (combo_font)->entry),"changed",
		      GTK_SIGNAL_FUNC (combo_font_func), NULL);
  gtk_box_pack_start (GTK_BOX (hbox_char1), combo_font, FALSE, FALSE, 2);
  gtk_widget_show (combo_font);

  hbox_char2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_char2);
  gtk_container_add (GTK_CONTAINER (vbox1), hbox_char2);

  label_char2 = gtk_label_new ("Character style");
  gtk_widget_show (label_char2);
  gtk_box_pack_start (GTK_BOX (hbox_char2), label_char2, TRUE, TRUE, 2);
  gtk_misc_set_alignment (GTK_MISC (label_char2), 0.1, 0.5);

  char_list = NULL;
  char_list = g_list_append (char_list, "Roman");
  char_list = g_list_append (char_list, "Bold");
  char_list = g_list_append (char_list, "Italic");
  char_list = g_list_append (char_list, "Oblique");
  char_list = g_list_append (char_list, "Bold italic");
  char_list = g_list_append (char_list, "Bold oblique");
  char_list = g_list_append (char_list, "Default");

 
  combo_char = gtk_combo_new ();
  gtk_widget_set_usize (combo_char, 150, 22);
  gtk_combo_set_popdown_strings (GTK_COMBO (combo_char), char_list);
  gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo_char)->entry), FALSE);
  gtk_combo_set_use_arrows_always( GTK_COMBO (combo_char), 1);
  gtk_signal_connect (GTK_OBJECT (GTK_COMBO (combo_char)->entry),"changed",
		      GTK_SIGNAL_FUNC (combo_char_func), NULL);
  gtk_box_pack_start (GTK_BOX (hbox_char2), combo_char, FALSE, FALSE, 2);
  gtk_widget_show (combo_char);


  hbox_char3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_char3);
  gtk_container_add (GTK_CONTAINER (vbox1), hbox_char3);

  label_char3 = gtk_label_new ("Underline");
  gtk_widget_show (label_char3);
  gtk_box_pack_start (GTK_BOX (hbox_char3), label_char3, TRUE, TRUE, 2);
  gtk_misc_set_alignment (GTK_MISC (label_char3), 0.1, 0.5);

  underline_list = NULL;
  underline_list = g_list_append (underline_list, "Normal");
  underline_list = g_list_append (underline_list, "Underline");
  underline_list = g_list_append (underline_list, "Overline");
  underline_list = g_list_append (underline_list, "Cross out");
  underline_list = g_list_append (underline_list, "Default");
 
  combo_underline = gtk_combo_new ();
  gtk_widget_set_usize (combo_underline, 150, 22);
  gtk_combo_set_popdown_strings (GTK_COMBO (combo_underline), underline_list);
  gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo_underline)->entry), FALSE);
  gtk_combo_set_use_arrows_always( GTK_COMBO (combo_underline), 1);
  gtk_signal_connect (GTK_OBJECT (GTK_COMBO (combo_underline)->entry),"changed",
		      GTK_SIGNAL_FUNC (combo_underline_func), NULL);
  gtk_box_pack_start (GTK_BOX (hbox_char3), combo_underline, FALSE, FALSE, 2);
  gtk_widget_show (combo_underline);

  hbox_char4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_char4);
  gtk_container_add (GTK_CONTAINER (vbox1), hbox_char4);

  label_char4 = gtk_label_new ("Body size (pt)");
  gtk_widget_show (label_char4);
  gtk_box_pack_start (GTK_BOX (hbox_char4), label_char4, TRUE, TRUE, 2);
  gtk_misc_set_alignment (GTK_MISC (label_char4), 0.1, 0.5);

  body_size_list = NULL;
  body_size_list = g_list_append (body_size_list, "6");
  body_size_list = g_list_append (body_size_list, "8");
  body_size_list = g_list_append (body_size_list, "10");
  body_size_list = g_list_append (body_size_list, "12");
  body_size_list = g_list_append (body_size_list, "14");
  body_size_list = g_list_append (body_size_list, "16");
  body_size_list = g_list_append (body_size_list, "20");
  body_size_list = g_list_append (body_size_list, "20");
  body_size_list = g_list_append (body_size_list, "24");
  body_size_list = g_list_append (body_size_list, "30");
  body_size_list = g_list_append (body_size_list, "40");
  body_size_list = g_list_append (body_size_list, "60");
  body_size_list = g_list_append (body_size_list, "Default");

 
  combo_body_size = gtk_combo_new ();
  gtk_widget_set_usize (combo_body_size, 150, 22);
  gtk_combo_set_popdown_strings (GTK_COMBO (combo_body_size), body_size_list);
  gtk_entry_set_editable (GTK_ENTRY (GTK_COMBO (combo_body_size)->entry), FALSE);
  gtk_combo_set_use_arrows_always( GTK_COMBO (combo_body_size), 1);
  gtk_signal_connect (GTK_OBJECT (GTK_COMBO (combo_body_size)->entry),"changed",
		      GTK_SIGNAL_FUNC (combo_body_size_func), NULL);
  gtk_box_pack_start (GTK_BOX (hbox_char4), combo_body_size, FALSE, FALSE, 2);
  gtk_widget_show (combo_body_size);

  label_char5 = gtk_label_new ("");
  gtk_widget_show (label_char5);
  gtk_box_pack_start (GTK_BOX (vbox1), label_char5, TRUE, TRUE, 0);

  hseparator_char1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator_char1);
  gtk_box_pack_start (GTK_BOX (vbox1), hseparator_char1, FALSE, FALSE, 5);

  hbuttonbox_char1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox_char1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox_char1, FALSE, FALSE, 5);

  button_char1 = gtk_button_new_with_label ("Apply");
  gtk_widget_show (button_char1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_char1), button_char1);
  gtk_signal_connect (GTK_OBJECT (button_char1), "clicked",
                      GTK_SIGNAL_FUNC (ch_apply_func),
                      NULL);

  button_char2 = gtk_button_new_with_label ("Done");
  gtk_widget_show (button_char2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_char1), button_char2);
  gtk_signal_connect (GTK_OBJECT (button_char2), "clicked",
                      GTK_SIGNAL_FUNC (ch_done_func),
                      NULL);
                   /************************************************/

 /* Create the "color" frame */
/* Initialisations */
  defstyle = gtk_widget_get_default_style ();
  for (i = 0; i < 153; i++){
    color_form[i] = (Color_form *)TtaGetMemory (sizeof(Color_form)); 
  }
  color_previous = (Color_previous *)TtaGetMemory (sizeof (Color_previous));
  color_previous->label_f = NULL;
  color_previous->label_b = NULL;

  palette_frame = gtk_frame_new ("Colors");
  gtk_container_add (GTK_CONTAINER (notebook1),palette_frame);
  gtk_widget_show (palette_frame);

  palette_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (palette_frame), palette_vbox1);
  gtk_widget_show (palette_vbox1);
  
  palette_label1 = gtk_label_new ("Left button for the foreground \n Right button for the background");
  gtk_misc_set_alignment (GTK_MISC (palette_label1), 0.1, 0.5);
  gtk_widget_show (palette_label1);
  gtk_box_pack_start (GTK_BOX (palette_vbox1), palette_label1, FALSE, FALSE,0);

  palette_table1 = gtk_table_new (19, 8, TRUE);
  gtk_widget_show (palette_table1);
  gtk_box_pack_start (GTK_BOX (palette_vbox1), palette_table1, TRUE, TRUE, 0);
  
  indice = 0;

  for (j=0; j<19; j++){
    for (i=0;i<8; i++){

      button_color.red = RGB_Table[indice].red*256;
      button_color.green = RGB_Table[indice].green*256;
      button_color.blue = RGB_Table[indice].blue*256;
      button_color.pixel = 0;
      button_style = gtk_style_copy (defstyle);
      for (k=0; k<5; k++){
	button_style->bg[k]= button_color;
	if (indice == 1||indice == 7 ||indice ==15||indice ==23||indice ==31
	    ||indice ==39||indice ==47||indice ==55||indice ==63||indice ==71
	    ||indice ==79||indice ==87||indice ==95||indice ==103||indice ==111
	    ||indice ==119||indice ==127||indice ==135||indice ==143||indice ==151
	    ||indice ==110||indice ==118||indice ==126||indice ==134||indice ==142
	    ||indice ==150||indice ==116||indice ==117||indice ==124||indice ==125)
	    	button_style->fg[k]= white;

      }
      
      palette_button1 = gtk_button_new ();
      gtk_widget_set_style (palette_button1, button_style);
      gtk_table_attach (GTK_TABLE (palette_table1), palette_button1, i, i+1, j, j+1,
			(GtkAttachOptions) GTK_EXPAND | GTK_FILL, 
			(GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);
      gtk_signal_connect (GTK_OBJECT (palette_button1), "button_press_event",
			  GTK_SIGNAL_FUNC (button_press_func),
			  (gpointer)color_form[indice]);
      gtk_widget_set_events (dialog1, GDK_BUTTON_PRESS_MASK);
      gtk_widget_show (palette_button1);
      
      palette_label2= color_form[indice]->label = gtk_label_new ("");
      gtk_widget_set_style (palette_label2, button_style);
      gtk_widget_show (palette_label2);
      gtk_container_add (GTK_CONTAINER (palette_button1), palette_label2);
      color_form[indice]->indice = indice;
         
      indice++;
    }
}
  gtk_widget_show (palette_table1);


  palette_hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (palette_hbuttonbox);
  gtk_box_pack_start (GTK_BOX (palette_vbox1), palette_hbuttonbox, FALSE, FALSE, 5);

  palette_button2 = gtk_button_new_with_label ("Apply");
  gtk_widget_show (palette_button2);
  gtk_container_add (GTK_CONTAINER (palette_hbuttonbox),palette_button2);
  gtk_signal_connect (GTK_OBJECT (palette_button2), "clicked",
                      GTK_SIGNAL_FUNC (pal_apply_func),
                      NULL);

  palette_button3 = gtk_button_new_with_label ("Done");
  gtk_widget_show (palette_button3);
  gtk_container_add (GTK_CONTAINER (palette_hbuttonbox),palette_button3);
  gtk_signal_connect (GTK_OBJECT (palette_button3), "clicked",
                      GTK_SIGNAL_FUNC (pal_done_func),
                      NULL);
 
                   /************************************************/
  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox3);
  gtk_widget_realize (vbox3);

  hbox_format1 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox_format1);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox_format1, TRUE, TRUE, 0);

  frame_format1 = gtk_frame_new ("Align");
  gtk_widget_show (frame_format1);
  gtk_box_pack_start (GTK_BOX (hbox_format1), frame_format1, FALSE, FALSE, 2);
  gtk_widget_set_usize (frame_format1, 160, 120);

  table_format1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table_format1);
  gtk_container_add (GTK_CONTAINER (frame_format1), table_format1);

  glade_pixmap = gdk_pixmap_create_from_xpm (dialog1->window, &glade_mask,
                                             &dialog1->style->bg[GTK_STATE_NORMAL],
                                             "/home/tobago/riche/dialogue/myxpm/bleft.xpm");
  pixmap_format1 = gtk_pixmap_new (glade_pixmap, glade_mask);
  gdk_pixmap_unref (glade_pixmap);
  gdk_bitmap_unref (glade_mask);
  gtk_widget_show (pixmap_format1);
  gtk_table_attach (GTK_TABLE (table_format1), pixmap_format1, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  glade_pixmap = gdk_pixmap_create_from_xpm (dialog1->window, &glade_mask,
                                             &dialog1->style->bg[GTK_STATE_NORMAL],
                                             "/home/tobago/riche/dialogue/myxpm/bright.xpm");
  pixmap_format2 = gtk_pixmap_new (glade_pixmap, glade_mask);
  gdk_pixmap_unref (glade_pixmap);
  gdk_bitmap_unref (glade_mask);
  gtk_widget_show (pixmap_format2);
  gtk_table_attach (GTK_TABLE (table_format1), pixmap_format2, 1, 2, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  glade_pixmap = gdk_pixmap_create_from_xpm (dialog1->window, &glade_mask,
                                             &dialog1->style->bg[GTK_STATE_NORMAL],
                                             "/home/tobago/riche/dialogue/myxpm/bcenter.xpm");
  pixmap_format3 = gtk_pixmap_new (glade_pixmap, glade_mask);
  gdk_pixmap_unref (glade_pixmap);
  gdk_bitmap_unref (glade_mask);
  gtk_widget_show (pixmap_format3);
  gtk_table_attach (GTK_TABLE (table_format1), pixmap_format3, 1, 2, 2, 3,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  label_format1 = gtk_label_new ("Default");
  gtk_widget_show (label_format1);
  gtk_table_attach (GTK_TABLE (table_format1), label_format1, 1, 2, 3, 4,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  radiobutton_format1 = gtk_radio_button_new_with_label (table_format1_group, "");
  table_format1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format1));
  gtk_widget_show (radiobutton_format1);
  gtk_table_attach (GTK_TABLE (table_format1), radiobutton_format1, 0, 1, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (radiobutton_format1), TRUE);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format1), "clicked",
                      GTK_SIGNAL_FUNC (left_align_func),
                      NULL);

  radiobutton_format2 = gtk_radio_button_new_with_label (table_format1_group, "");
  table_format1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format2));
  gtk_widget_show (radiobutton_format2);
  gtk_table_attach (GTK_TABLE (table_format1), radiobutton_format2, 0, 1, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format2), "clicked",
                      GTK_SIGNAL_FUNC (right_align_func),
                      NULL);

  radiobutton_format3 = gtk_radio_button_new_with_label (table_format1_group, "");
  table_format1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format3));
  gtk_widget_show (radiobutton_format3);
  gtk_table_attach (GTK_TABLE (table_format1), radiobutton_format3, 0, 1, 2, 3,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format3), "clicked",
                      GTK_SIGNAL_FUNC (center_align_func),
                      NULL);

  radiobutton_format4 = gtk_radio_button_new_with_label (table_format1_group, "");
  table_format1_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format4));
  gtk_widget_show (radiobutton_format4);
  gtk_table_attach (GTK_TABLE (table_format1), radiobutton_format4, 0, 1, 3, 4,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format4), "clicked",
		     GTK_SIGNAL_FUNC (default_align_func),
                      NULL);

  frame_format2 = gtk_frame_new ("Justification");
  gtk_widget_show (frame_format2);
  gtk_box_pack_start (GTK_BOX (hbox_format1), frame_format2, FALSE, FALSE, 2);
  gtk_widget_set_usize (frame_format2, 160, 120);

  table_format2 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table_format2);
  gtk_container_add (GTK_CONTAINER (frame_format2), table_format2);

  label_format2 = gtk_label_new ("Yes");
  gtk_widget_show (label_format2);
  gtk_table_attach (GTK_TABLE (table_format2), label_format2, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  label_format3 = gtk_label_new ("No");
  gtk_widget_show (label_format3);
  gtk_table_attach (GTK_TABLE (table_format2), label_format3, 1, 2, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  label_format4 = gtk_label_new ("Default");
  gtk_widget_show (label_format4);
  gtk_table_attach (GTK_TABLE (table_format2), label_format4, 1, 2, 2, 3,
                    (GtkAttachOptions) GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  radiobutton_format5 = gtk_radio_button_new_with_label (table_format2_group, "");
  table_format2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format5));
  gtk_widget_show (radiobutton_format5);
  gtk_table_attach (GTK_TABLE (table_format2), radiobutton_format5, 0, 1, 0, 1,
                    (GtkAttachOptions) GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format5), "clicked",
                      GTK_SIGNAL_FUNC (yes_jus_func),
                      NULL);

  radiobutton_format6 = gtk_radio_button_new_with_label (table_format2_group, "");
  table_format2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format6));
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (radiobutton_format6), TRUE);
  gtk_widget_show (radiobutton_format6);
  gtk_table_attach (GTK_TABLE (table_format2), radiobutton_format6, 0, 1, 1, 2,
                    (GtkAttachOptions) GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format6), "clicked",
                      GTK_SIGNAL_FUNC (no_jus_func),
                      NULL);

  radiobutton_format7 = gtk_radio_button_new_with_label (table_format2_group, "");
  table_format2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format7));
  gtk_widget_show (radiobutton_format7);
  gtk_table_attach (GTK_TABLE (table_format2), radiobutton_format7, 0, 1, 2, 3,
                    (GtkAttachOptions) GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format7), "clicked",
                      GTK_SIGNAL_FUNC (default_jus_func),
                      NULL);

  hbox_format2 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox_format2);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox_format2, TRUE, TRUE, 0);

  frame_format4 = gtk_frame_new ("Line spacing");
  gtk_widget_show (frame_format4);
  gtk_box_pack_start (GTK_BOX (hbox_format2), frame_format4, FALSE, FALSE, 2);
  gtk_widget_set_usize (frame_format4, 160, -1);

  vbox_format3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_format3);
  gtk_container_add (GTK_CONTAINER (frame_format4), vbox_format3);

  table_format3 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table_format3);
  gtk_box_pack_start (GTK_BOX (vbox_format3), table_format3, TRUE, TRUE, 0);


  glade_pixmap = gdk_pixmap_create_from_xpm (dialog1->window, &glade_mask,
                                             &dialog1->style->bg[GTK_STATE_NORMAL],
                                             "/home/tobago/riche/dialogue/myxpm/bssmall.xpm");
  pixmap_format5 = gtk_pixmap_new (glade_pixmap, glade_mask);
  gdk_pixmap_unref (glade_pixmap);
  gdk_bitmap_unref (glade_mask);
  gtk_widget_show (pixmap_format5);
  gtk_table_attach (GTK_TABLE (table_format3), pixmap_format5, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

 
  glade_pixmap = gdk_pixmap_create_from_xpm (dialog1->window, &glade_mask,
                                             &dialog1->style->bg[GTK_STATE_NORMAL],
                                             "/home/tobago/riche/dialogue/myxpm/bsmedium.xpm");
  pixmap_format6 = gtk_pixmap_new (glade_pixmap, glade_mask);
  gdk_pixmap_unref (glade_pixmap);
  gdk_bitmap_unref (glade_mask);
  gtk_widget_show (pixmap_format6);
  gtk_table_attach (GTK_TABLE (table_format3), pixmap_format6, 1, 2, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

 
  glade_pixmap = gdk_pixmap_create_from_xpm (dialog1->window, &glade_mask,
                                             &dialog1->style->bg[GTK_STATE_NORMAL],
                                             "/home/tobago/riche/dialogue/myxpm/bssmall.xpm");
  pixmap_format7 = gtk_pixmap_new (glade_pixmap, glade_mask);
  gdk_pixmap_unref (glade_pixmap);
  gdk_bitmap_unref (glade_mask);
  gtk_widget_show (pixmap_format7);
  gtk_table_attach (GTK_TABLE (table_format3), pixmap_format7, 1, 2, 2, 3,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  label_format6 = gtk_label_new ("Default");
  gtk_widget_show (label_format6);
  gtk_table_attach (GTK_TABLE (table_format3), label_format6, 1, 2, 3, 4,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  radiobutton_format8 = gtk_radio_button_new_with_label (table_format3_group, "");
  table_format3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format8));
  gtk_widget_show (radiobutton_format8);
  gtk_table_attach (GTK_TABLE (table_format3), radiobutton_format8, 0, 1, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format8), "clicked",
                      GTK_SIGNAL_FUNC (small_line_spcg_func),
		      NULL);

  radiobutton_format9 = gtk_radio_button_new_with_label (table_format3_group, "");
  table_format3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format9));
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (radiobutton_format9), TRUE);
  gtk_widget_show (radiobutton_format9);
  gtk_table_attach (GTK_TABLE (table_format3), radiobutton_format9, 0, 1, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format9), "clicked",
                      GTK_SIGNAL_FUNC (medium_line_spcg_func),
                      NULL);

  radiobutton_format10 = gtk_radio_button_new_with_label (table_format3_group, "");
  table_format3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format10));
  gtk_widget_show (radiobutton_format10);
  gtk_table_attach (GTK_TABLE (table_format3), radiobutton_format10, 0, 1, 2, 3,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format10), "clicked",
                      GTK_SIGNAL_FUNC (large_line_spcg_func),
                      NULL);

  radiobutton_format11 = gtk_radio_button_new_with_label (table_format3_group, "");
  table_format3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format11));
  gtk_widget_show (radiobutton_format11);
  gtk_table_attach (GTK_TABLE (table_format3), radiobutton_format11, 0, 1, 3, 4,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format11), "clicked",
                      GTK_SIGNAL_FUNC (default_line_spcg_func),
                      NULL);

  hbox_format3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_format3);
  gtk_box_pack_start (GTK_BOX (vbox_format3), hbox_format3, TRUE, TRUE, 0);

  label_format7 = gtk_label_new ("Line spacing (pt)\n1..200");
  gtk_widget_show (label_format7);
  gtk_box_pack_start (GTK_BOX (hbox_format3), label_format7, TRUE, FALSE, 2);
  gtk_widget_set_usize (label_format7, -1, 35);
  gtk_label_set_justify (GTK_LABEL (label_format7), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_format7), 0.1, 0.5);

  entry_format1 = gtk_entry_new ();
  gtk_widget_show (entry_format1);
  gtk_box_pack_start (GTK_BOX (hbox_format3), entry_format1, FALSE, FALSE, 2);
  gtk_widget_set_usize (entry_format1, 50, -1);
  gtk_signal_connect (GTK_OBJECT (entry_format1), "changed",
                      GTK_SIGNAL_FUNC (line_spcg_pt_func),
                      NULL);

  frame_format5 = gtk_frame_new ("Indent");
  gtk_widget_show (frame_format5);
  gtk_box_pack_start (GTK_BOX (hbox_format2), frame_format5, FALSE, FALSE, 0);
  gtk_widget_set_usize (frame_format5, 160, 120);

  vbox_format4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_format4);
  gtk_container_add (GTK_CONTAINER (frame_format5), vbox_format4);

  table_format4 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table_format4);
  gtk_box_pack_start (GTK_BOX (vbox_format4), table_format4, TRUE, TRUE, 0);

 
  glade_pixmap = gdk_pixmap_create_from_xpm (dialog1->window, &glade_mask,
                                             &dialog1->style->bg[GTK_STATE_NORMAL],
                                             "/home/tobago/riche/dialogue/myxpm/bindent1.xpm");
  pixmap_format8 = gtk_pixmap_new (glade_pixmap, glade_mask);
  gdk_pixmap_unref (glade_pixmap);
  gdk_bitmap_unref (glade_mask);
  gtk_widget_show (pixmap_format8);
  gtk_table_attach (GTK_TABLE (table_format4), pixmap_format8, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

 
  glade_pixmap = gdk_pixmap_create_from_xpm (dialog1->window, &glade_mask,
                                             &dialog1->style->bg[GTK_STATE_NORMAL],
                                             "/home/tobago/riche/dialogue/myxpm/bindent2.xpm");
  pixmap_format9 = gtk_pixmap_new (glade_pixmap, glade_mask);
  gdk_pixmap_unref (glade_pixmap);
  gdk_bitmap_unref (glade_mask);
  gtk_widget_show (pixmap_format9);
  gtk_table_attach (GTK_TABLE (table_format4), pixmap_format9, 1, 2, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  label_format8 = gtk_label_new ("Default");
  gtk_widget_show (label_format8);
  gtk_table_attach (GTK_TABLE (table_format4), label_format8, 1, 2, 2, 3,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  radiobutton_format12 = gtk_radio_button_new_with_label (table_format4_group, "");
  table_format4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format12));
  gtk_widget_show (radiobutton_format12);
  gtk_table_attach (GTK_TABLE (table_format4), radiobutton_format12, 0, 1, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (radiobutton_format12), TRUE);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format12), "clicked",
                      GTK_SIGNAL_FUNC (indent1_func),
                      NULL);

  radiobutton_format13 = gtk_radio_button_new_with_label (table_format4_group, "");
  table_format4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format13));
  gtk_widget_show (radiobutton_format13);
  gtk_table_attach (GTK_TABLE (table_format4), radiobutton_format13, 0, 1, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format13), "clicked",
                      GTK_SIGNAL_FUNC (indent2_func),
                      NULL);

  radiobutton_format14 = gtk_radio_button_new_with_label (table_format4_group, "");
  table_format4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_format14));
  gtk_widget_show (radiobutton_format14);
  gtk_table_attach (GTK_TABLE (table_format4), radiobutton_format14, 0, 1, 2, 3,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 1);
  gtk_signal_connect (GTK_OBJECT (radiobutton_format14), "clicked",
                      GTK_SIGNAL_FUNC (default_indent_func),
                      NULL);

  hbox_format4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_format4);
  gtk_box_pack_start (GTK_BOX (vbox_format4), hbox_format4, TRUE, TRUE, 0);

  label_format9 = gtk_label_new ("Indent (pt)\n0..3000");
  gtk_widget_show (label_format9);
  gtk_box_pack_start (GTK_BOX (hbox_format4), label_format9, TRUE, FALSE, 2);
  gtk_widget_set_usize (label_format9, -1, 35);
  gtk_label_set_justify (GTK_LABEL (label_format9), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_format9), 0.1, 0.5);

  entry_format2 = gtk_entry_new ();
  gtk_widget_show (entry_format2);
  gtk_box_pack_start (GTK_BOX (hbox_format4), entry_format2, FALSE, FALSE, 2);
  gtk_widget_set_usize (entry_format2, 50, -1);
  gtk_signal_connect (GTK_OBJECT (entry_format2), "changed",
                      GTK_SIGNAL_FUNC (indent_pt_func),
                      NULL);

  label_format10 = gtk_label_new ("");
  gtk_widget_show (label_format10);
  gtk_box_pack_start (GTK_BOX (vbox3), label_format10, TRUE, TRUE, 0);

  hseparator_format1 = gtk_hseparator_new ();
  gtk_widget_show (hseparator_format1);
  gtk_box_pack_start (GTK_BOX (vbox3), hseparator_format1, FALSE, FALSE, 2);

  hbuttonbox_format1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox_format1);
  gtk_box_pack_start (GTK_BOX (vbox3), hbuttonbox_format1, FALSE, FALSE, 2);

  button_format1 = gtk_button_new_with_label ("Apply");
  GTK_WIDGET_SET_FLAGS (button_format1, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (hbuttonbox_format1), button_format1, TRUE, TRUE,0);
  gtk_widget_grab_default (button_format1);
  gtk_widget_show (button_format1);
  gtk_signal_connect (GTK_OBJECT (button_format1), "clicked",
		      GTK_SIGNAL_FUNC (fo_apply_func), NULL);

  button_format2 = gtk_button_new_with_label ("Done");
  GTK_WIDGET_SET_FLAGS (button_format2, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (hbuttonbox_format1), button_format2, TRUE, TRUE,0);
  gtk_widget_show (button_format2);
  gtk_signal_connect (GTK_OBJECT (button_format2), "clicked",
		      GTK_SIGNAL_FUNC (fo_done_func), NULL);


                        /***********************************/

  dialog_type = BACKGROUND_IMAGE;
  dialog_box = dialog1;
  maboite = gtk_my_file_selection_new ("Background image ");
  gtk_my_file_selection_hide_fileop_buttons (GTK_MY_FILE_SELECTION (maboite));
  gtk_widget_show (maboite);
  gtk_container_add (GTK_CONTAINER (notebook1), maboite);
  
 
  label1 = gtk_label_new ("Characters");
  gtk_widget_show (label1);
  set_notebook_tab (notebook1, 0, label1);

  label2 = gtk_label_new ("Colors");
  gtk_widget_show (label2);
  set_notebook_tab (notebook1, 1, label2);

  label3 = gtk_label_new ("Format");
  gtk_widget_show (label3);
  set_notebook_tab (notebook1, 2, label3);

  label4 = gtk_label_new ("Background  image");
  gtk_widget_show (label4);
  set_notebook_tab (notebook1, 3, label4);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area1), 10);

 
 
  
  gtk_widget_show (dialog1);

  Initialisation_style ();
  return (dialog1);
}
/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Apply Class" dialog box 
----------------------------------------------------------------*/
/* Callback function called by the "Select a class" text widget */
void
class_text_func                        (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

/* Callback function called by the "Confirm" button widget */
void
class_confirm_func                     (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "Done" button widget */
void
class_done_func                        (GtkButton       *button,
                                        gpointer         user_data)
{

}
/* Function that creates an "Apply class" dialog box */
GtkWidget*
create_dialog_apply_class ()
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *class_label1;
  GtkWidget *class_text1;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbuttonbox1;
  GtkWidget *class_button1;
  GtkWidget *class_button2;

  /* Create the "apply class" dialog box */
  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), "Apply class");
  gtk_window_set_policy (GTK_WINDOW (dialog1), FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy), dialog1);
 


  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (vbox1), 20);

  /* Create the "Select a class" label */
  class_label1 = gtk_label_new ("Select a class");
  gtk_widget_show (class_label1);
  gtk_box_pack_start (GTK_BOX (vbox1), class_label1, FALSE, FALSE, 2);
  gtk_label_set_justify (GTK_LABEL (class_label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (class_label1), 0.1, 0.5);

  /* Create the "Select a class" text widget */
  class_text1 = gtk_text_new (NULL, NULL);
  gtk_widget_show (class_text1);
  gtk_box_pack_start (GTK_BOX (vbox1), class_text1, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (class_text1), "changed",
                      GTK_SIGNAL_FUNC (class_text_func),
                      NULL);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  /* Create the "Apply class" hbuttonbox */
  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbuttonbox1, TRUE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_EDGE);

  /* Create the "Confirm" button */
  class_button1 = gtk_button_new_with_label ("Confirm");
  gtk_widget_show (class_button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), class_button1);
  GTK_WIDGET_SET_FLAGS (class_button1, GTK_CAN_DEFAULT);
  gtk_signal_connect (GTK_OBJECT (class_button1), "clicked",
                      GTK_SIGNAL_FUNC (class_confirm_func),
                      NULL);

  /* Create the "Done" button */
  class_button2 = gtk_button_new_with_label ("Done");
  gtk_widget_show (class_button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), class_button2);
  GTK_WIDGET_SET_FLAGS (class_button2, GTK_CAN_DEFAULT);
  gtk_signal_connect (GTK_OBJECT (class_button2), "clicked",
                      GTK_SIGNAL_FUNC (class_done_func),
                      NULL);

  return dialog1;
}

/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Language attributes" dialog box 
----------------------------------------------------------------*/
/* Callback function called by the entry widget*/
void
attr_lang_entry_func                    (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

/* Callback function called by the text widget*/
void
attr_lang_text_func                    (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

/* Callback function called by the "Apply" button */
void
attr_lang_apply_func                   (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "Remove attribute" button */
void
attr_lang_remove_func                  (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "Done" button */
void
attr_lang_done_func                    (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Create an "Attribute" dialog box*/
GtkWidget*
create_dialog_attr_lang ()
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *attr_lang_table1;
  GtkWidget *attr_lang_label2;
  GtkWidget *attr_lang_label3;
  GtkWidget *attr_lang_label4;
  GtkWidget *attr_lang_label5;
  GtkWidget *attr_lang_label6;
  GtkWidget *attr_lang_entry1;
  GtkWidget *attr_lang_label1;
  GtkWidget *attr_lang_scrolledwindow1;
  GtkWidget *attr_lang_text1;
  GtkWidget *dialog_action_area1;
  GtkWidget *attr_lang_hbuttonbox1;
  GtkWidget *attr_lang_button1;
  GtkWidget *attr_lang_button2;
  GtkWidget *attr_lang_button3;

  /* Create the "attribute" dialog box*/
  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), "dialog1");
  gtk_window_set_policy (GTK_WINDOW (dialog1), FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy), dialog1);
 

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  attr_lang_table1 = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (attr_lang_table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), attr_lang_table1, TRUE, TRUE, 0);

  /* Create the "Inherited language : English" label */
  attr_lang_label2 = gtk_label_new ("Inherited language : English");
  gtk_widget_show (attr_lang_label2);
  gtk_table_attach (GTK_TABLE (attr_lang_table1), attr_lang_label2, 0, 1, 3, 4,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_label_set_justify (GTK_LABEL (attr_lang_label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (attr_lang_label2), 0.1, 0.5);

  attr_lang_label3 = gtk_label_new ("");
  gtk_widget_show (attr_lang_label3);
  gtk_table_attach (GTK_TABLE (attr_lang_table1), attr_lang_label3, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  attr_lang_label4 = gtk_label_new ("");
  gtk_widget_show (attr_lang_label4);
  gtk_table_attach (GTK_TABLE (attr_lang_table1), attr_lang_label4, 1, 2, 1, 2,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  attr_lang_label5 = gtk_label_new ("");
  gtk_widget_show (attr_lang_label5);
  gtk_table_attach (GTK_TABLE (attr_lang_table1), attr_lang_label5, 1, 2, 2, 3,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  attr_lang_label6 = gtk_label_new ("");
  gtk_widget_show (attr_lang_label6);
  gtk_table_attach (GTK_TABLE (attr_lang_table1), attr_lang_label6, 1, 2, 3, 4,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  /*Create the entry widget */
  attr_lang_entry1 = gtk_entry_new ();
  gtk_widget_show (attr_lang_entry1);
  gtk_table_attach (GTK_TABLE (attr_lang_table1), attr_lang_entry1, 0, 1, 2, 3,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_signal_connect (GTK_OBJECT (attr_lang_entry1), "changed",
                      GTK_SIGNAL_FUNC (attr_lang_entry_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (attr_lang_entry1), "e");

  attr_lang_label1 = gtk_label_new ("Language of element");
  gtk_widget_show (attr_lang_label1);
  gtk_table_attach (GTK_TABLE (attr_lang_table1), attr_lang_label1, 0, 1, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 0, 0);
  gtk_label_set_justify (GTK_LABEL (attr_lang_label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (attr_lang_label1), 0.1, 0.5);

  attr_lang_scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (attr_lang_scrolledwindow1);
  gtk_table_attach (GTK_TABLE (attr_lang_table1), attr_lang_scrolledwindow1, 0, 1, 1, 2,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (attr_lang_scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  
  /*Create the text widget */
  attr_lang_text1 = gtk_text_new (NULL, NULL);
  gtk_widget_show (attr_lang_text1);
  gtk_container_add (GTK_CONTAINER (attr_lang_scrolledwindow1), attr_lang_text1);
  gtk_signal_connect (GTK_OBJECT (attr_lang_text1), "activate",
                      GTK_SIGNAL_FUNC (attr_lang_text_func),
                      NULL);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  /* Create the "attribute" hbuttonbox */
  attr_lang_hbuttonbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (attr_lang_hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), attr_lang_hbuttonbox1, TRUE, TRUE, 0);

  /* Create the "Apply" button */
  attr_lang_button1 = gtk_button_new_with_label ("Apply");
  GTK_WIDGET_SET_FLAGS (attr_lang_button1, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (attr_lang_hbuttonbox1), attr_lang_button1);
  gtk_widget_grab_default (attr_lang_button1);
  gtk_widget_show (attr_lang_button1);
  gtk_signal_connect (GTK_OBJECT (attr_lang_button1), "clicked",
                      GTK_SIGNAL_FUNC (attr_lang_apply_func),
                      NULL);

   /* Create the "Remove attribute" button */
  attr_lang_button2 = gtk_button_new_with_label ("Remove attribute");
  GTK_WIDGET_SET_FLAGS (attr_lang_button2, GTK_CAN_DEFAULT);
  gtk_widget_show (attr_lang_button2);
  gtk_container_add (GTK_CONTAINER (attr_lang_hbuttonbox1), attr_lang_button2);
  gtk_signal_connect (GTK_OBJECT (attr_lang_button2), "clicked",
                      GTK_SIGNAL_FUNC (attr_lang_remove_func),
                      NULL);
  /* Create the "Done" button */
  attr_lang_button3 = gtk_button_new_with_label ("Done");
  GTK_WIDGET_SET_FLAGS (attr_lang_button3, GTK_CAN_DEFAULT);
  gtk_widget_show (attr_lang_button3);
  gtk_container_add (GTK_CONTAINER (attr_lang_hbuttonbox1), attr_lang_button3);
  gtk_signal_connect (GTK_OBJECT (attr_lang_button3), "clicked",
                      GTK_SIGNAL_FUNC (attr_lang_done_func),
                      NULL);

  return dialog1;
}
/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Close" dialog box 
----------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 Callback function called by the "Save document" button
------------------------------------------------------------------------- */
static  void sd_func (GtkWidget *widget, gpointer data) 
{
 
}

/*--------------------------------------------------------------------------
 Callback function called by the "Close don't save" button
------------------------------------------------------------------------- */
static  void cds_func (GtkWidget *widget, gpointer data) 
{
 printf("Close, don't save \n");
}

/*--------------------------------------------------------------------------
 Callback function called by the "Cancel" button
------------------------------------------------------------------------- */
static  void cancel_close_func (GtkWidget *widget, gpointer data) 
{
 gtk_widget_destroy (GTK_WIDGET (data));
}
GtkWidget*
create_dialog_close ()
{
  GtkWidget *dialog_close1;
  GtkWidget *dialog_vbox_close1;
  GtkWidget *label_close1;
  GtkWidget *dialog_action_area_close1;
  GtkWidget *hbuttonbox_close1;
  GtkWidget *button_close1;
  GtkWidget *button_close2;
  GtkWidget *button_close3;

  /* Creation of the dialog box */
  dialog_close1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog_close1), "dialog_close1");
  gtk_window_set_policy (GTK_WINDOW (dialog_close1), FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog_close1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy), NULL);
 

  dialog_vbox_close1 = GTK_DIALOG (dialog_close1)->vbox;
  gtk_widget_show (dialog_vbox_close1);

  /* Label of the "warning" message */
  label_close1 = gtk_label_new ("label_close1");
  gtk_widget_show (label_close1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox_close1), label_close1, FALSE, FALSE, 25);

  dialog_action_area_close1 = GTK_DIALOG (dialog_close1)->action_area;
  gtk_widget_show (dialog_action_area_close1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area_close1), 10);

  /* Creation of the button box */
  hbuttonbox_close1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbuttonbox_close1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area_close1), hbuttonbox_close1, TRUE, TRUE, 0);

  /* The "Save document" button widget */
  button_close1 = gtk_button_new_with_label ("Save document");
  GTK_WIDGET_SET_FLAGS (button_close1, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_close1), button_close1);
  gtk_widget_grab_default (button_close1);
  gtk_widget_show (button_close1);
  gtk_signal_connect (GTK_OBJECT (button_close1), "clicked",
		      GTK_SIGNAL_FUNC (sd_func), NULL);

  /* The "Close, don't save" button widget */
  button_close2 = gtk_button_new_with_label ("Close, don't save");
  GTK_WIDGET_SET_FLAGS (button_close2, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_close1), button_close2);
  gtk_widget_show (button_close2);
  gtk_signal_connect (GTK_OBJECT (button_close2), "clicked",
		      GTK_SIGNAL_FUNC (cds_func), NULL);

  /* The "Cancel" button widget */
  button_close3 = gtk_button_new_with_label ("Cancel");
  GTK_WIDGET_SET_FLAGS (button_close3, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_close1), button_close3);
  gtk_widget_show (button_close3);
  gtk_signal_connect (GTK_OBJECT (button_close3), "clicked",
		      GTK_SIGNAL_FUNC (cancel_close_func), dialog_close1 );
 
  return dialog_close1;
}
/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Confirm" dialog box 
----------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 Callback function called by the "Confirm" button
------------------------------------------------------------------------- */
static  void confirm_func (GtkWidget *widget, gpointer data) 
{
 printf("confirmer\n");
}


/*----------------------------------------------------------------------------
 Function that creates the "Confirm" dialog 
----------------------------------------------------------------------------*/
GtkWidget*
create_dialog_confirm ()
{
  GtkWidget *dialog_confirm1;
  GtkWidget *dialog_vbox_confirm1;
  GtkWidget *label_confirm1;
  GtkWidget *dialog_action_area_confirm1;
  GtkWidget *hbuttonbox_confirm1;
  GtkWidget *button_confirm1;

  /* Creation of the dialog box */
  dialog_confirm1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog_confirm1), "dialog_confirm1");
  gtk_window_set_policy (GTK_WINDOW (dialog_confirm1), FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog_confirm1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy), dialog_confirm1);
 

  dialog_vbox_confirm1 = GTK_DIALOG (dialog_confirm1)->vbox;
  gtk_widget_show (dialog_vbox_confirm1);

  /* The label of  "confirm" message */
  label_confirm1 = gtk_label_new ("label_confirm1");
  gtk_widget_show (label_confirm1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox_confirm1), label_confirm1, TRUE, TRUE, 15);

  dialog_action_area_confirm1 = GTK_DIALOG (dialog_confirm1)->action_area;
  gtk_widget_show (dialog_action_area_confirm1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area_confirm1), 10);

  /* Creation of the button box */
  hbuttonbox_confirm1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox_confirm1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area_confirm1), hbuttonbox_confirm1, TRUE, TRUE, 0);

  /* The "confirm" button widget */
  button_confirm1 = gtk_button_new_with_label ("Confirm");
  GTK_WIDGET_SET_FLAGS (button_confirm1, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_confirm1), button_confirm1);
  gtk_widget_grab_default (button_confirm1);
  gtk_widget_show (button_confirm1);
  gtk_signal_connect (GTK_OBJECT (button_confirm1), "clicked",
		      GTK_SIGNAL_FUNC (confirm_func), NULL);
  return dialog_confirm1;
}
/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Create rule" dialog box 
----------------------------------------------------------------*/
/* Callback function called by the text widget */
void
create_rule_text_func                        (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


/* Callback function called by the entry widget */
void
create_rule_entry_func                       (GtkEditable     *editable,
                                        gpointer         user_data)
{

}

/* Callback function called by the "Confirm" button */
void
create_rule_confirm_func                     (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "Done" button */
void
create_rule_done_func                        (GtkButton       *button,
                                        gpointer         user_data)
{

}

GtkWidget*
create_dialog_create_rule ()
{
  GtkWidget *dialog_create_rule1;
  GtkWidget *dialog_vbox_create_rule1;
  GtkWidget *vbox_create_rule1;
  GtkWidget *create_rule_label_create_rule1;
  GtkWidget *create_rule_text_create_rule1;
  GtkWidget *create_rule_entry_create_rule1;
  GtkWidget *dialog_action_area_create_rule1;
  GtkWidget *hbuttonbox_create_rule1;
  GtkWidget *create_rule_button_create_rule1;
  GtkWidget *create_rule_button2;

  dialog_create_rule1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog_create_rule1), "dialog_create_rule1");
  gtk_window_set_policy (GTK_WINDOW (dialog_create_rule1), FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog_create_rule1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy), dialog_create_rule1);
 

  dialog_vbox_create_rule1 = GTK_DIALOG (dialog_create_rule1)->vbox;
  gtk_widget_show (dialog_vbox_create_rule1);

  vbox_create_rule1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_create_rule1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox_create_rule1), vbox_create_rule1, TRUE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (vbox_create_rule1), 20);

  create_rule_label_create_rule1 = gtk_label_new ("Select a class");
  gtk_widget_show (create_rule_label_create_rule1);
  gtk_box_pack_start (GTK_BOX (vbox_create_rule1), create_rule_label_create_rule1, FALSE, FALSE, 2);
  gtk_label_set_justify (GTK_LABEL (create_rule_label_create_rule1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (create_rule_label_create_rule1), 0.1, 0.5);

  create_rule_text_create_rule1 = gtk_text_new (NULL, NULL);
  gtk_widget_show (create_rule_text_create_rule1);
  gtk_box_pack_start (GTK_BOX (vbox_create_rule1), create_rule_text_create_rule1, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (create_rule_text_create_rule1), "changed",
                      GTK_SIGNAL_FUNC (create_rule_text_func),
                      NULL);
  create_rule_entry_create_rule1 = gtk_entry_new ();
  gtk_widget_show (create_rule_entry_create_rule1);
  gtk_box_pack_start (GTK_BOX (vbox_create_rule1), create_rule_entry_create_rule1, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (create_rule_entry_create_rule1), "changed",
                      GTK_SIGNAL_FUNC (create_rule_entry_func),
                      NULL);

  dialog_action_area_create_rule1 = GTK_DIALOG (dialog_create_rule1)->action_area;
  gtk_widget_show (dialog_action_area_create_rule1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area_create_rule1), 10);

  hbuttonbox_create_rule1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox_create_rule1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area_create_rule1), hbuttonbox_create_rule1, TRUE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox_create_rule1), GTK_BUTTONBOX_EDGE);

  create_rule_button_create_rule1 = gtk_button_new_with_label ("Confirm");
  gtk_widget_show (create_rule_button_create_rule1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_create_rule1), create_rule_button_create_rule1);
  GTK_WIDGET_SET_FLAGS (create_rule_button_create_rule1, GTK_CAN_DEFAULT);
  gtk_signal_connect (GTK_OBJECT (create_rule_button_create_rule1), "clicked",
                      GTK_SIGNAL_FUNC (create_rule_confirm_func),
                      NULL);

  create_rule_button2 = gtk_button_new_with_label ("Done");
  gtk_widget_show (create_rule_button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_create_rule1), create_rule_button2);
  GTK_WIDGET_SET_FLAGS (create_rule_button2, GTK_CAN_DEFAULT);
  gtk_signal_connect (GTK_OBJECT (create_rule_button2), "clicked",
                      GTK_SIGNAL_FUNC (create_rule_done_func),
                      NULL);

  return dialog_create_rule1;
}
/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Math" dialog box 
----------------------------------------------------------------*/
/* Callback function called by a button of the "greek alphabet" dialog_box */
static void math_button_press_func (GtkWidget *widget,
			gpointer         data)
{
  greek_index_value = (int *)data;
  printf ("%d\n", *greek_index_value);
}
/* Function that creates a "greek alphabet" dialogbox */
static GtkWidget *
Create_greek_dialog ()
{
#ifdef _0
  GtkWidget *greek_dialog;
  GtkWidget *greek_dialog_vbox;
  GtkWidget *greek_frame;
  GtkWidget *greek_table1;
  GtkWidget *greek_vbox1;
static  GtkWidget *greek_button1;
 
 

  int i, j,  indice;
  

/* Initialisations */

  for (i = 0; i < 160; i++){
  greek_index[i] = (int*)malloc(sizeof(int));
  } 
 
  /* Create the "greek alphabet" dialog box */
  greek_dialog = gtk_dialog_new (); 
  gtk_signal_connect (GTK_OBJECT (greek_dialog), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroy),GTK_OBJECT (greek_dialog));
  greek_dialog_vbox = GTK_DIALOG (greek_dialog)->vbox;
  gtk_widget_show (greek_dialog_vbox);

  /* Create a frame */
  greek_frame = gtk_frame_new ("");     
  gtk_container_add (GTK_CONTAINER (greek_dialog_vbox),greek_frame);
  gtk_widget_show (greek_frame);

  greek_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (greek_frame), greek_vbox1);
  gtk_widget_show (greek_vbox1);

  /* Create the table containing all the letter buttons */
  greek_table1 = gtk_table_new (16, 10, TRUE);
  gtk_widget_show (greek_table1);
  gtk_box_pack_start (GTK_BOX (greek_vbox1), greek_table1, TRUE, TRUE, 0);
  
  indice = 0;
 
  for (j=0; j<16; j++){
    for (i=0;i<10; i++){
                
      greek_button1 = gtk_button_new_with_label ((gchar *)&Items_Grec[indice].value); 
     greek_button1->style->font = gdk_font_load ("-adobe-symbol-medium-r-normal-*-16-160-*-*-p-*-adobe-fontspecific");  
   
      gtk_table_attach (GTK_TABLE (greek_table1), greek_button1, i, i+1, j, j+1,
			(GtkAttachOptions) GTK_EXPAND | GTK_FILL, 
			(GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);
      gtk_signal_connect (GTK_OBJECT (greek_button1), "clicked",
			  GTK_SIGNAL_FUNC (math_button_press_func),
			  (gpointer) greek_index[indice]);
      gtk_widget_show (greek_button1);
      *greek_index[indice] = indice; 
      indice++;
    }
}
  gtk_widget_show (greek_table1);
  
  return (greek_dialog);
#else /* _0*/
return NULL;
#endif /*_0*/
}

  
/* Function that creates a pixmap widget */
     GtkWidget *xpm_label_box (GtkWidget *parent, gchar *xpm_filename)
     {
       
         GtkWidget *pixmapwid;
         GdkPixmap *pixmap;
         GdkBitmap *mask;
         GtkStyle *style;

         style = gtk_widget_get_style(parent);
         pixmap = gdk_pixmap_create_from_xpm (parent->window, &mask,
                                              &style->bg[GTK_STATE_NORMAL],
                                              (gchar*)xpm_filename);
         pixmapwid = gtk_pixmap_new (pixmap, mask);

       
	 return (pixmapwid);
     }

/* Callback function called by the "math" button */
static void
math_func                              (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "root" button */
static void
root_func                              (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "sqrt" button */
static void
sqrt_func                              (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "frac" button */
static void
frac_func                              (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "subsup" button */
static void
subsup_func                            (GtkButton       *button,
                                        gpointer         user_data)
{

}
/* Callback function called by the "sub" button */

static void
sub_func                               (GtkButton       *button,
                                        gpointer         user_data)
{

}
/* Callback function called by the "sup" button */

 static void
sup_func                               (GtkButton       *button,
                                        gpointer         user_data)
{

}
/* Callback function called by the "overunder" button */

static void
overunder_func                         (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "under" button */
static void
under_func                             (GtkButton       *button,
                                        gpointer         user_data)
{

}
/* Callback function called by the "over" button */

static void
over_func                              (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "fence" button */
static void
fence_func                             (GtkButton       *button,
                                        gpointer         user_data)
{

}
/* Callback function called by the "mscript" button */

static void
mscript_func                           (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "matrix" button */
static void
matrix_func                            (GtkButton       *button,
                                        gpointer         user_data)
{

}

/* Callback function called by the "greek" button */
static void
greek_func                             (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *greek_dialog;
  greek_dialog = Create_greek_dialog ();
  gtk_widget_show (greek_dialog);
}

/* Callback function called by the "done" button */
static void
done_math_func                         (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy (GTK_WIDGET (user_data));
}




GtkWidget*
create_dialog_math ()
{
  GtkWidget *math_dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *math_vbox1;
  GtkWidget *math_button1;
  GtkWidget *math_button2;
  GtkWidget *math_button3;
  GtkWidget *math_button4;
  GtkWidget *math_button5;
  GtkWidget *math_button6;
  GtkWidget *math_button7;
  GtkWidget *math_button8;
  GtkWidget *math_button9;
  GtkWidget *math_button10;
  GtkWidget *math_button11;
  GtkWidget *math_button12;
  GtkWidget *math_button13;
  GtkWidget *math_button14;
  GtkWidget *dialog_action_area1;
  GtkWidget *math_hbuttonbox1;
  GtkWidget *math_button15;
  GtkWidget *math_hbox1;
  GtkWidget *math_label1;
  GtkWidget *math_label2;
  GtkWidget *math_pixmapwid;


  math_dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (math_dialog1), "math_dialog1");
  gtk_window_set_policy (GTK_WINDOW (math_dialog1), FALSE, TRUE, FALSE);
  gtk_widget_realize (math_dialog1);
  gtk_signal_connect (GTK_OBJECT (math_dialog1), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroy),GTK_OBJECT (math_dialog1));
 

  dialog_vbox1 = GTK_DIALOG (math_dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);
 

  math_hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (math_hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), math_hbox1, TRUE, TRUE, 0);

  math_label1 = gtk_label_new ("");
  gtk_widget_show (math_label1);
  gtk_box_pack_start (GTK_BOX (math_hbox1), math_label1, TRUE, TRUE, 0);

  math_vbox1 = gtk_vbox_new (TRUE, 0);
  gtk_widget_show (math_vbox1);
  gtk_box_pack_start (GTK_BOX (math_hbox1), math_vbox1, FALSE, FALSE, 0);

  math_label2 = gtk_label_new ("");
  gtk_widget_show (math_label2);
  gtk_box_pack_start (GTK_BOX (math_hbox1), math_label2, TRUE, TRUE, 0);

  math_button1 = gtk_button_new ();
  gtk_widget_show (math_button1);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button1, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button1), "clicked",
                      GTK_SIGNAL_FUNC (math_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/Bmath.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button1), math_pixmapwid);


  math_button2 = gtk_button_new ();
  gtk_widget_show (math_button2);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button2, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button2), "clicked",
                      GTK_SIGNAL_FUNC (root_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1,"xpm/root.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button2), math_pixmapwid);

  math_button3 = gtk_button_new ();
  gtk_widget_show (math_button3);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button3, FALSE, FALSE,2);
  gtk_signal_connect (GTK_OBJECT (math_button3), "clicked",
                      GTK_SIGNAL_FUNC (sqrt_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/sqrt.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button3), math_pixmapwid);


  math_button4 = gtk_button_new ();
  gtk_widget_show (math_button4);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button4, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button4), "clicked",
                      GTK_SIGNAL_FUNC (frac_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/frac.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button4), math_pixmapwid);


  math_button5 = gtk_button_new ();
  gtk_widget_show (math_button5);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button5, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button5), "clicked",
                      GTK_SIGNAL_FUNC (subsup_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/subsup.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button5), math_pixmapwid);


  math_button6 = gtk_button_new ();
  gtk_widget_show (math_button6);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button6, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button6), "clicked",
                      GTK_SIGNAL_FUNC (sub_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/sub.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button6),math_pixmapwid);


  math_button7 = gtk_button_new ();
  gtk_widget_show (math_button7);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button7, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button7), "clicked",
                      GTK_SIGNAL_FUNC (sup_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/sup.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button7), math_pixmapwid);

  math_button8 = gtk_button_new ();
  gtk_widget_show (math_button8);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button8, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button8), "clicked",
                      GTK_SIGNAL_FUNC (overunder_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/overunder.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button8), math_pixmapwid);


  math_button9 = gtk_button_new ();
  gtk_widget_show (math_button9);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button9, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button9), "clicked",
                      GTK_SIGNAL_FUNC (under_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/under.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button9), math_pixmapwid);


  math_button10 = gtk_button_new ();
  gtk_widget_show (math_button10);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button10, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button10), "clicked",
                      GTK_SIGNAL_FUNC (over_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/over.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button10), math_pixmapwid);


  math_button11 = gtk_button_new ();
  gtk_widget_show (math_button11);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button11, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button11), "clicked",
                      GTK_SIGNAL_FUNC (fence_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/fence.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button11), math_pixmapwid);


  math_button12 = gtk_button_new ();
  gtk_widget_show (math_button12);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button12, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button12), "clicked",
                      GTK_SIGNAL_FUNC (mscript_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/mscript.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button12), math_pixmapwid);


  math_button13 = gtk_button_new ();
  gtk_widget_show (math_button13);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button13, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button13), "clicked",
                      GTK_SIGNAL_FUNC (matrix_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/matrix.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button13), math_pixmapwid);


  math_button14 = gtk_button_new ();
  gtk_widget_show (math_button14);
  gtk_box_pack_start (GTK_BOX (math_vbox1), math_button14, FALSE, FALSE, 2);
  gtk_signal_connect (GTK_OBJECT (math_button14), "clicked",
                      GTK_SIGNAL_FUNC (greek_func),
                      NULL);
  math_pixmapwid = xpm_label_box(math_dialog1, "xpm/greek.xpm");
  gtk_widget_show(math_pixmapwid);
  gtk_container_add (GTK_CONTAINER (math_button14), math_pixmapwid);


  dialog_action_area1 = GTK_DIALOG (math_dialog1)->action_area;
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  /* Create the "math" hbuttonbox */
  math_hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), math_hbuttonbox1, TRUE, TRUE, 0);
  gtk_widget_show (math_hbuttonbox1);

  /* Createh the "Done button" */
  math_button15 = gtk_button_new_with_label ("Done");
  gtk_container_add (GTK_CONTAINER (math_hbuttonbox1), math_button15);
  GTK_WIDGET_SET_FLAGS (math_button15, GTK_CAN_DEFAULT);
  gtk_widget_show (math_button15);
  gtk_widget_grab_default (math_button15);
  gtk_signal_connect (GTK_OBJECT (math_button15), "clicked",
                      GTK_SIGNAL_FUNC (done_math_func),
                      math_dialog1);

  return math_dialog1;
}
/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Preferences" dialog box 
----------------------------------------------------------------*/
static void Initialisation_preferences ()/* il faudra faire un free */
{
  general = (General *)TtaGetMemory (sizeof (General)); 
  general->home_page = TEXT("www.w3c.org");
  general->multi_key = FALSE;
  general->show_bckgd_image = FALSE;
  general->dbl_click_activate = FALSE;
  general->dbl_click_delay = 10;
  general->zoom = 10;
  general->menu_font_size =10;
  general->dlg_lang = TEXT ("en");

  publish = (Publish *)TtaGetMemory (sizeof (Publish));
  publish->use_etags = TRUE;
  publish->verify_puts = FALSE;
  publish->default_url = TEXT ("www.w3c.org");

  cache = (Cache *)TtaGetMemory(sizeof (Cache));
  cache->directory = TEXT("/HOME");
  cache->enable = TRUE;
  cache->protected_document = TRUE;
  cache->disconnected = FALSE;
  cache->ignore_expires = FALSE;
  cache->size = 1;
  cache->limit_entry = 3;

  proxy = (Proxy *)TtaGetMemory (sizeof (Proxy));
  proxy->http_proxy = TEXT("f");
  proxy->no_proxy_domains = TEXT("h");

  color = (Color *)TtaGetMemory (sizeof (Color));
  color->doc_forgrd = TEXT ("a");
  color->doc_backgrd = TEXT ("b");
  color->menu_forgrd = TEXT ("c");
  color->menu_backgrd = TEXT ("d");

  list_language = (STRING *)TtaGetMemory(sizeof (STRING));
  *list_language = TEXT("en, fr");
 }

/************************************************************************/
/*--------------------------------------------------------------------------
 Callback function called by the "home page" text _entry of the general preferences frame
------------------------------------------------------------------------- */
void
home_page_func                         (GtkEditable     *editable,
                                        gpointer         user_data)
{
  general->home_page = gtk_entry_get_text (GTK_ENTRY (general_entry1)); 
}
/*--------------------------------------------------------------------------
 Callback function called by the "double click delay" text _entry of the general preferences frame
------------------------------------------------------------------------- */

void
dbl_click_delay_func                   (GtkEditable     *editable,
                                        gpointer         user_data)
{
  gchar *temp;

  temp = gtk_entry_get_text (GTK_ENTRY (general_entry2)); 
  general->dbl_click_delay = atoi(temp);
}

/*--------------------------------------------------------------------------
 Callback function called by the "zoom " text _entry of the general preferences frame
------------------------------------------------------------------------- */
void
zoom_func                              (GtkEditable     *editable,
                                        gpointer         user_data)
{
  gchar *temp;

  temp = gtk_entry_get_text (GTK_ENTRY (general_entry3)); 
  general->zoom = atoi(temp);
}

/*--------------------------------------------------------------------------
 Callback function called by the "dialogue language" text _entry of the general preferences frame
------------------------------------------------------------------------- */
void
dialogue_language_func                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
  general->dlg_lang = gtk_entry_get_text (GTK_ENTRY (general_entry4)); 
}

/*--------------------------------------------------------------------------
 Callback function called by the "menu font size" text _entry of the general preferences frame
------------------------------------------------------------------------- */
void
menu_font_size_func                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
gchar *temp;

  temp = gtk_entry_get_text (GTK_ENTRY (general_entry5)); 
  general->menu_font_size = atoi(temp);
}
/*--------------------------------------------------------------------------
 Callback function called by the "enable multi key" checkbutton of the general preferences frame
------------------------------------------------------------------------- */
void
enable_multi_key_func                  (GtkButton       *button,
                                        gpointer         user_data)
{
 if (GTK_TOGGLE_BUTTON (button)->active) 
    general->multi_key = TRUE;
  else
  general->multi_key = FALSE;
}

/*--------------------------------------------------------------------------
 Callback function called by the "Show background images" checkbutton of the general preferences frame
------------------------------------------------------------------------- */
void
show_bckgd_images_func                 (GtkButton       *button,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (button)->active) 
    general->show_bckgd_image = TRUE;
  else
    general->show_bckgd_image = FALSE;
  
}
/*--------------------------------------------------------------------------
 Callback function called by the "Double click activates link" checkbutton of the general preferences frame
------------------------------------------------------------------------- */
void
dbl_click_activates_link_func          (GtkButton       *button,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (button)->active) 
    general->dbl_click_activate = TRUE;
  else
    general->dbl_click_activate = FALSE;
}

/*--------------------------------------------------------------------------
 Callback function called by the "Apply " button of the general preferences frame
------------------------------------------------------------------------- */
void
hp_apply_func                          (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("apply general preferences\n");
}

/*--------------------------------------------------------------------------
 Callback function called by the "Defaults " button of the general preferences frame
------------------------------------------------------------------------- */
void
hp_defaults_func                       (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_entry_set_text (GTK_ENTRY (general_entry1), "www.yahoo.fr");
  gtk_entry_set_text (GTK_ENTRY (general_entry2), "500");
  gtk_entry_set_text (GTK_ENTRY (general_entry3), "10");
  gtk_entry_set_text (GTK_ENTRY (general_entry4), "en_US");
  gtk_entry_set_text (GTK_ENTRY (general_entry5), "10");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (general_checkbutton1),FALSE );
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (general_checkbutton2),TRUE );
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (general_checkbutton3),TRUE );
  general->dbl_click_activate = TRUE;
  general->show_bckgd_image = TRUE;
  general->multi_key = FALSE;
  general->home_page =
  general->dlg_lang = TEXT ("en_US");
  general->zoom = 10;
  general->menu_font_size =10;
  general->dbl_click_delay = 500;
}


/*--------------------------------------------------------------------------
 Callback function called by the "done " button of the general preferences frame
------------------------------------------------------------------------- */
void
hp_done_func                           (GtkButton       *button,
                                        gpointer         user_data)
{
printf ("dbl_click_activate : %d\n",general->dbl_click_activate);
printf ("show_bckgd_image :%d \n",general->show_bckgd_image);
printf ("multi_key  :%d \n",general->multi_key   );
printf ("home_page :%s \n",general->home_page   );
printf ("dlg_lang  :%s \n",general->dlg_lang   );
printf ("zoom  :%d \n",general->zoom   );
printf ("menu_font_size  :%d \n",general->menu_font_size   );
printf ("dbl_click_delay  :%d \n",general->dbl_click_delay  );

}

/*--------------------------------------------------------------------------
 Callback function called by the "use ETAGS and preconditions" checkbutton of the publish preferences frame
------------------------------------------------------------------------- */
void
etags_func                             (GtkButton       *button,
                                        gpointer         user_data)
{
 if (GTK_TOGGLE_BUTTON (button)->active) 
    publish->use_etags = TRUE;
  else
    publish->use_etags = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "use ETAGS and preconditions" checkbutton of the publish preferences frame
------------------------------------------------------------------------- */
void
put_with_get_func                      (GtkButton       *button,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (button)->active) 
    publish->verify_puts = TRUE;
  else
    publish->verify_puts = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "default name for url s finishing in /" entry of the publish preferences frame
------------------------------------------------------------------------- */
void
default_url_name_func                  (GtkEditable     *editable,
                                        gpointer         user_data)
{
  publish->default_url =  gtk_entry_get_text (GTK_ENTRY (publish_entry1));
}
/*--------------------------------------------------------------------------
 Callback function called by the "apply" button  of the publish preferences frame
------------------------------------------------------------------------- */
void
pu_apply_func                          (GtkButton       *button,
                                        gpointer         user_data)
{

  printf("use_etags :%d\n", publish->use_etags);
  printf("verify_puts :%d\n", publish->verify_puts);
  printf("default url :%s\n", publish->default_url);
}
/*--------------------------------------------------------------------------
 Callback function called by the "defaults" button  of the publish preferences frame
------------------------------------------------------------------------- */
void
pu_defaults_func                       (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("defaults\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "done" button  of the publish preferences frame
------------------------------------------------------------------------- */
void
pu_done_func                           (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("done\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "enable cache" checkbutton  of the cache  preferences frame
------------------------------------------------------------------------- */
void
enable_cache_func                      (GtkButton       *button,
                                        gpointer         user_data)
{
 if (GTK_TOGGLE_BUTTON (button)->active) 
   cache->enable = TRUE;
  else
    cache->enable = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "cache protected documents" checkbutton  of the cache  preferences frame
------------------------------------------------------------------------- */
void
Cache_protected_doc_func               (GtkButton       *button,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (button)->active) 
    cache->protected_document = TRUE;
  else
    cache->protected_document  = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "disconnected mode" checkbutton  of the cache  preferences frame
------------------------------------------------------------------------- */
void
disconnected_mod_func                  (GtkButton       *button,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (button)->active) 
    cache->disconnected = TRUE;
  else
    cache->disconnected = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "ignore expires : header" checkbutton  of the cache  preferences frame
------------------------------------------------------------------------- */
void
ignore_expires_func                    (GtkButton       *button,
                                        gpointer         user_data)
{
  if (GTK_TOGGLE_BUTTON (button)->active) 
 cache->ignore_expires = TRUE;
  else
    cache->ignore_expires = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "cache directory" entry  of the cache  preferences frame
------------------------------------------------------------------------- */
void
cache_direcory_func                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
 cache->directory =  gtk_entry_get_text (GTK_ENTRY (cache_entry1));

}
/*--------------------------------------------------------------------------
 Callback function called by the "cache size" entry  of the cache  preferences frame
------------------------------------------------------------------------- */
void
cache_size_func                        (GtkEditable     *editable,
                                        gpointer         user_data)
{
  gchar* temp;
  temp =  gtk_entry_get_text (GTK_ENTRY (cache_entry2));
  cache->size = atoi(temp);
}
/*--------------------------------------------------------------------------
 Callback function called by the "cache entry size limit" entry  of the cache  preferences frame
------------------------------------------------------------------------- */
void
cache_entry_func                       (GtkEditable     *editable,
                                        gpointer         user_data)
{
 gchar * temp;
  temp =  gtk_entry_get_text (GTK_ENTRY (cache_entry3));
  cache->limit_entry =  atoi(temp); 
}
/*--------------------------------------------------------------------------
 Callback function called by the "apply" button  of the cache  preferences frame
------------------------------------------------------------------------- */
void
cp_apply_func                          (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("cache directory :%s\n", cache->directory);
  printf("cache->enable:%d\n", cache->enable);
  printf("cache->protected_document:%d\n", cache->protected_document);
  printf("cache->disconnected:%d\n", cache->disconnected );
  printf(" cache->ignore_expires:%d\n", cache->ignore_expires );

  printf("cache->size:%d\n", cache->size);
  printf("cache->limit_entry:%d\n", cache->limit_entry);

}
/*--------------------------------------------------------------------------
 Callback function called by the "defaults" button  of the cache  preferences frame
------------------------------------------------------------------------- */
void
cp_defaults_func                       (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("defaults\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "flush cache" button  of the cache  preferences frame
------------------------------------------------------------------------- */
void
cp_flush_func                          (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("flush\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "done" button  of the cache  preferences frame
------------------------------------------------------------------------- */
void
cp_done_func                           (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("done\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "http proxy" entry  of the proxy  preferences frame
------------------------------------------------------------------------- */
void
no_proxy_func                          (GtkEditable     *editable,
                                        gpointer         user_data)
{
  proxy->no_proxy_domains = gtk_entry_get_text (GTK_ENTRY (proxy_entry2));
 
}

/*--------------------------------------------------------------------------
 Callback function called by the "no proxy on these domains" entry  of the proxy  preferences frame
------------------------------------------------------------------------- */
void
http_proxy_func                        (GtkEditable     *editable,
                                        gpointer         user_data)
{
  proxy->http_proxy = gtk_entry_get_text (GTK_ENTRY (proxy_entry1));
}
/*--------------------------------------------------------------------------
 Callback function called by the "apply" button  of the proxy  preferences frame
------------------------------------------------------------------------- */
void
pr_apply_func                          (GtkButton       *button,
                                        gpointer         user_data)
{
  printf ("http proxy :%s \n", proxy->http_proxy );
  printf ("no proxy domains :%s \n",proxy->no_proxy_domains );
}
/*--------------------------------------------------------------------------
 Callback function called by the "defaults" button  of the proxy  preferences frame
------------------------------------------------------------------------- */
void
pr_defaults_func                       (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("defaults\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "done" button  of the proxy  preferences frame
------------------------------------------------------------------------- */
void
pr_done_func                           (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("done\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "document foreground color" entry  of the color  preferences frame
------------------------------------------------------------------------- */
void
doc_forgrd_clr_func                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
  color->doc_forgrd =  gtk_entry_get_text (GTK_ENTRY (color_entry1));
}
/*--------------------------------------------------------------------------
 Callback function called by the "document background color" entry  of the color  preferences frame
------------------------------------------------------------------------- */
void
doc_backgrd_clr_func                   (GtkEditable     *editable,
                                        gpointer         user_data)
{
  color->doc_backgrd = gtk_entry_get_text (GTK_ENTRY (color_entry2));
}
/*--------------------------------------------------------------------------
 Callback function called by the "menu foreground color" entry  of the color  preferences frame
------------------------------------------------------------------------- */
void
menu_forgrd_clr_func                   (GtkEditable     *editable,
                                        gpointer         user_data)
{
  color->menu_forgrd = gtk_entry_get_text (GTK_ENTRY (color_entry3));
}
/*--------------------------------------------------------------------------
 Callback function called by the "menu background color" entry  of the color  preferences frame
------------------------------------------------------------------------- */
void
menu_backgrd_clr_func                  (GtkEditable     *editable,
                                        gpointer         user_data)
{
  color->menu_backgrd = gtk_entry_get_text (GTK_ENTRY (color_entry4));
}
/*--------------------------------------------------------------------------
 Callback function called by the "apply" button  of the color  preferences frame
------------------------------------------------------------------------- */
void
co_apply_func                          (GtkButton       *button,
                                        gpointer         user_data)
{
  printf ("color->doc_forgrd = %s\n", color->doc_forgrd);
  printf ("  color->doc_backgrd = %s\n", color->doc_backgrd);
  printf ("  color->menu_forgrd =%s\n",color->menu_forgrd);
  printf (" color->menu_backgrd = %s\n",color->menu_backgrd);
}
/*--------------------------------------------------------------------------
 Callback function called by the "defaults" button  of the color  preferences frame
------------------------------------------------------------------------- */
void
co_defaults_func                       (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("defaults\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "done" button  of the color  preferences frame
------------------------------------------------------------------------- */
void
co_done_func                           (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("done\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "save current geometry" button  of the geometry  preferences frame
------------------------------------------------------------------------- */
void
save_current_geo_func                  (GtkButton       *button,
                                        gpointer         user_data)
{
 printf("save current geometry\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "restore default geometry" button  of the geometry  preferences frame
------------------------------------------------------------------------- */
void
restore_default_geo_func               (GtkButton       *button,
                                        gpointer         user_data)
{
printf("restore default geometry\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "done" button  of the geometry  preferences frame
------------------------------------------------------------------------- */
void
ge_done_func                           (GtkButton       *button,
                                        gpointer         user_data)
{
printf("done\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "list of preferred languages" entry  of the language  preferences frame
------------------------------------------------------------------------- */
void
list_language_func                          (GtkButton       *button,
                                        gpointer         user_data)
{
 // gchar *temp;
 // temp = gtk_entry_get_text (GTK_ENTRY (language_entry1));
 // list_language = TEXT (temp);
   *list_language = gtk_entry_get_text (GTK_ENTRY (language_entry1));
}
/*--------------------------------------------------------------------------
 Callback function called by the "apply" button  of the language  preferences frame
------------------------------------------------------------------------- */
void
la_apply_func                          (GtkButton       *button,
                                        gpointer         user_data)
{
   printf ("list language = %s\n", *list_language);
  printf("apply\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "defaults" button  of the language  preferences frame
------------------------------------------------------------------------- */
void
la_defaults_func                       (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("defaults\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "done" button  of the language  preferences frame
------------------------------------------------------------------------- */
void
la_done_func                           (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("done\n");
}

/*************************************************************************/
GtkWidget*
create_dialog_preferences ()
{
  /* container widgets */
  GtkWidget *dialog1; 
  GtkWidget *dialog_vbox1;
  GtkWidget *notebook1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *dialog_action_area1;
  
  /* widget of the "general preferences" notebook page */
  GtkWidget *general_frame1; 
  GtkWidget *general_vbox1;
  GtkWidget *general_hbox1;
  GtkWidget *general_table1;
  GtkWidget *general_frame2;
  GtkWidget *general_vbox2; 
  GtkWidget *general_frame3;
  GtkWidget *general_vbox3;
  GtkWidget *general_frame4;
  GtkWidget *general_frame5;
  GtkWidget *general_vbox5;
  GtkWidget *general_hseparator1;
  GtkWidget *general_hbuttonbox1;
  GtkWidget *general_button1;
  GtkWidget *general_button2;
  GtkWidget *general_button3;
  GtkWidget *general_hbox2;
  GtkWidget *general_hbox3;
  GtkWidget *general_hbox4;
  GtkWidget *general_hbox5;
  GtkWidget *general_label1;
  GtkWidget *general_label2;
  GtkWidget *general_label3;
  GtkWidget *general_vbox4;
  GtkWidget *general_label4;
  GtkWidget *general_label5;
  GtkWidget *general_label6;
  GtkWidget *general_label7;
  GtkWidget *general_label8;
  GtkWidget *general_label9;
  
  /* widget of the "publish preferences" notebook page   */
  GtkWidget *publish_frame1;
  GtkWidget *publish_vbox1;
  GtkWidget *publish_hbox1;
  GtkWidget *publish_label1;
  GtkWidget *publish_hbuttonbox1;
  GtkWidget *publish_button1;
  GtkWidget *publish_button2;
  GtkWidget *publish_button3;
  GtkWidget *publish_hseparator1;
  GtkWidget *publish_vbox2;
  
  /* widget of the "cache preferences" notebook page   */
  GtkWidget *cache_frame1;
  GtkWidget *cache_vbox1;
  GtkWidget *cache_hbox1;
  GtkWidget *cache_label1;
  GtkWidget *cache_hbox2;
  GtkWidget *cache_frame2;
  GtkWidget *cache_vbox2;
  GtkWidget *cache_label2;
  GtkWidget *cache_frame3;
  GtkWidget *cache_vbox3;
  GtkWidget *cache_label4;
  GtkWidget *cache_hseparator1;
  GtkWidget *cache_hbuttonbox1;
  GtkWidget *cache_button1;
  GtkWidget *cache_button2;
  GtkWidget *cache_button3;
  GtkWidget *cache_button4;
  GtkWidget *cache_hbox3;
  GtkWidget *cache_hbox4;
  GtkWidget *cache_label3;
  GtkWidget *cache_label5;

  /* widget of the "proxy  preferences" notebook page   */
  GtkWidget *proxy_frame1;
  GtkWidget *proxy_vbox1;
  GtkWidget *proxy_table1;
  GtkWidget *proxy_label1;
  GtkWidget *proxy_label2;
  GtkWidget *proxy_label3;
  GtkWidget *proxy_label4; 
  GtkWidget *proxy_hseparator1;
  GtkWidget *proxy_hbuttonbox1;
  GtkWidget *proxy_button1;
  GtkWidget *proxy_button2;
  GtkWidget *proxy_button3;

  /* widget of the "color preferences" notebook page   */
  GtkWidget *color_frame1;
  GtkWidget *color_vbox1;
  GtkWidget *color_table1;
  GtkWidget *color_label1;
  GtkWidget *color_label2;
  GtkWidget *color_label3;
  GtkWidget *color_label4;
  GtkWidget *color_label6;
  GtkWidget *color_hseparator1;
  GtkWidget *color_hbuttonbox1;
  GtkWidget *color_button1;
  GtkWidget *color_button2;
  GtkWidget *color_button3;
  GtkWidget *color_label5;

  /* widget of the "geometry preferences" notebook page   */
  GtkWidget *geo_frame1;
  GtkWidget *geo_vbox1;
  GtkWidget *geo_label2;
  GtkWidget *geo_hseparator1;
  GtkWidget *geo_hbuttonbox1;
  GtkWidget *geo_button1;
  GtkWidget *geo_button2;
  GtkWidget *geo_button3;
  GtkWidget *geo_label1;

  /* widget of the "language preferences" notebook page   */
  GtkWidget *language_frame1;
  GtkWidget *language_vbox1;
  GtkWidget *language_vbox2;
  GtkWidget *language_label1;
  GtkWidget *language_hseparator1;
  GtkWidget *language_hbuttonbox1;
  GtkWidget *language_button1;
  GtkWidget *language_button2;
  GtkWidget *language_button3;
  GtkWidget *language_label2;
 
  /* Initialisations */
  Initialisation_preferences ();
  /***********************************************************************/
  /* Creates the preferences dialog box */
  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), "Preferences");
  gtk_window_set_policy (GTK_WINDOW (dialog1), FALSE, TRUE, FALSE);
  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);
  gtk_signal_connect (GTK_OBJECT (dialog1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy),GTK_OBJECT (dialog1));
  /* Creates the notebook */
  notebook1 = gtk_notebook_new ();
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), notebook1, TRUE, TRUE, 0);

  /***********************************************************************/
  /* First page of the notebook */
  /* Creates the  "General Preferences" frame */
  general_frame1= gtk_frame_new ("General Preferences");
  gtk_widget_show (general_frame1);
  gtk_container_add (GTK_CONTAINER (notebook1),general_frame1);
  
  general_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (general_vbox1);
  gtk_container_add (GTK_CONTAINER (general_frame1), general_vbox1);

  general_hbox1= gtk_hbox_new (FALSE, 0);
  gtk_widget_show (general_hbox1);
  gtk_box_pack_start (GTK_BOX (general_vbox1), general_hbox1, TRUE, TRUE, 0);
  gtk_widget_set_usize (general_hbox1, -1, 30);

  general_label1 = gtk_label_new ("Home Page");
  gtk_widget_show (general_label1);
  gtk_box_pack_start (GTK_BOX (general_hbox1), general_label1, FALSE, FALSE, 5);
  gtk_widget_set_usize (general_label1, -1, 22);
  gtk_misc_set_alignment (GTK_MISC (general_label1), 0.1, 0.5);
  
  /* The "home page" entry widget */
  general_entry1 = gtk_entry_new ();
  gtk_widget_show (general_entry1);
  gtk_box_pack_start (GTK_BOX (general_hbox1), general_entry1, TRUE, TRUE, 5);
  gtk_widget_set_usize (general_entry1, -1, 22);
  gtk_signal_connect (GTK_OBJECT (general_entry1), "changed",
                      GTK_SIGNAL_FUNC (home_page_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (general_entry1), "home.page");

  /* The "enable multi key" checkbutton widget */
  general_checkbutton1 = gtk_check_button_new_with_label ("Enable_multi_key");
  gtk_widget_show (general_checkbutton1);
  gtk_box_pack_start (GTK_BOX (general_vbox1), general_checkbutton1, FALSE, FALSE, 5);
  gtk_signal_connect (GTK_OBJECT (general_checkbutton1), "clicked",
                      GTK_SIGNAL_FUNC (enable_multi_key_func),
                      NULL);

  /* The "show background images" checkbutton widget */
  general_checkbutton2 = gtk_check_button_new_with_label ("Show background images");
  gtk_widget_show (general_checkbutton2);
  gtk_box_pack_start (GTK_BOX (general_vbox1), general_checkbutton2, FALSE, FALSE, 5);
  gtk_signal_connect (GTK_OBJECT (general_checkbutton2), "clicked",
                      GTK_SIGNAL_FUNC (show_bckgd_images_func),
                      NULL);

/* The "double click activates link" checkbutton widget */
  general_checkbutton3 = gtk_check_button_new_with_label ("Double click activates link");
  gtk_widget_show (general_checkbutton3);
  gtk_box_pack_start (GTK_BOX (general_vbox1), general_checkbutton3, FALSE, FALSE, 5);
  gtk_signal_connect (GTK_OBJECT (general_checkbutton3), "clicked",
                      GTK_SIGNAL_FUNC (dbl_click_activates_link_func),
                      NULL);

  
  general_table1 = gtk_table_new (2, 2, TRUE);
  gtk_widget_show (general_table1);
  gtk_box_pack_start (GTK_BOX (general_vbox1), general_table1, TRUE, TRUE, 0);
  
  /* the "double click delay" frame */
  general_frame2 = gtk_frame_new ("Double click delay (ms)");
  gtk_widget_show (general_frame2);
  gtk_table_attach (GTK_TABLE (general_table1), general_frame2, 0, 1, 0, 1,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 30, 0);
  gtk_container_border_width (GTK_CONTAINER (general_frame2), 5);

  general_vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (general_vbox2);
  gtk_container_add (GTK_CONTAINER (general_frame2), general_vbox2);

  general_label2 = gtk_label_new ("0 ..65000");
  gtk_widget_show (general_label2);
  gtk_box_pack_start (GTK_BOX (general_vbox2), general_label2, FALSE, FALSE, 5);
  gtk_misc_set_alignment (GTK_MISC (general_label2), 0.1, 0.5);

  general_hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (general_hbox2);
  gtk_box_pack_start (GTK_BOX (general_vbox2), general_hbox2, TRUE, TRUE, 0);
  
  /* the "double click delay" entry widget */
  general_entry2 = gtk_entry_new ();
  gtk_widget_show (general_entry2);
  gtk_box_pack_start (GTK_BOX (general_hbox2), general_entry2, FALSE, FALSE, 5);
  gtk_widget_set_usize (general_entry2, 50, 22);
  gtk_signal_connect (GTK_OBJECT (general_entry2), "changed",
                      GTK_SIGNAL_FUNC (dbl_click_delay_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (general_entry2), "5");

  general_label3 = gtk_label_new (" ");
  gtk_widget_show (general_label3);
  gtk_box_pack_start (GTK_BOX (general_hbox2), general_label3, TRUE, TRUE, 5);
  gtk_misc_set_alignment (GTK_MISC (general_label3), 0.1, 0.5);

  /* The "zoom" frame widget */
  general_frame3 = gtk_frame_new ("Zoom");
  gtk_widget_show (general_frame3);
  gtk_table_attach (GTK_TABLE (general_table1), general_frame3, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 30, 0);
  gtk_container_border_width (GTK_CONTAINER (general_frame3), 5);

  general_vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (general_vbox3);
  gtk_container_add (GTK_CONTAINER (general_frame3), general_vbox3);

  general_label4 = gtk_label_new ("-10 ..10");
  gtk_widget_show (general_label4);
  gtk_box_pack_start (GTK_BOX (general_vbox3), general_label4, FALSE, FALSE, 5);
  gtk_misc_set_alignment (GTK_MISC (general_label4), 0.1, 0.5);

  general_hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (general_hbox3);
  gtk_box_pack_start (GTK_BOX (general_vbox3), general_hbox3, TRUE, TRUE, 0);
  
  /* the "zoom" entry widget */
  general_entry3 = gtk_entry_new ();
  gtk_widget_show (general_entry3);
  gtk_box_pack_start (GTK_BOX (general_hbox3), general_entry3, FALSE, FALSE, 5);
  gtk_widget_set_usize (general_entry3, 50, 22);
  gtk_signal_connect (GTK_OBJECT (general_entry3), "changed",
                      GTK_SIGNAL_FUNC (zoom_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (general_entry3), "0");

  general_label6 = gtk_label_new (" ");
  gtk_widget_show (general_label6);
  gtk_box_pack_start (GTK_BOX (general_hbox3), general_label6, TRUE, TRUE, 5);
  gtk_misc_set_alignment (GTK_MISC (general_label6), 0.1, 0.5);

  /* The "dialogue language" frame */
  general_frame4 = gtk_frame_new ("Dialogue language");
  gtk_widget_show (general_frame4);
  gtk_table_attach (GTK_TABLE (general_table1), general_frame4, 0, 1, 1, 2,
                    (GtkAttachOptions)GTK_EXPAND | GTK_FILL , (GtkAttachOptions)GTK_EXPAND | GTK_FILL, 30, 0);
  gtk_container_border_width (GTK_CONTAINER (general_frame4), 5);


  general_vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (general_vbox4);
  gtk_container_add (GTK_CONTAINER (general_frame4), general_vbox4);

  general_label5 = gtk_label_new (" ");
  gtk_widget_show (general_label5);
  gtk_box_pack_start (GTK_BOX (general_vbox4), general_label5, TRUE, TRUE, 5);
  gtk_misc_set_alignment (GTK_MISC (general_label5), 0.1, 0.5);

  general_hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (general_hbox4);
  gtk_box_pack_start (GTK_BOX (general_vbox4), general_hbox4, TRUE, TRUE, 5);

  /* The "dialogue language" entry widget */
  general_entry4 = gtk_entry_new ();
  gtk_widget_show (general_entry4);
  gtk_box_pack_start (GTK_BOX (general_hbox4), general_entry4, FALSE, FALSE, 5);
  gtk_widget_set_usize (general_entry4, 50, 22);
  gtk_signal_connect (GTK_OBJECT (general_entry4), "changed",
                      GTK_SIGNAL_FUNC (dialogue_language_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (general_entry4), "en");

  general_label7 = gtk_label_new ("");
  gtk_widget_show (general_label7);
  gtk_box_pack_start (GTK_BOX (general_hbox4), general_label7, TRUE, TRUE, 5);
  gtk_misc_set_alignment (GTK_MISC (general_label7), 0.1, 0.5);

  /* The "menu font size" frame */
  general_frame5 = gtk_frame_new ("Menu font size");
  gtk_widget_show (general_frame5);
  gtk_table_attach (GTK_TABLE (general_table1), general_frame5, 1, 2, 1, 2,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 30, 0);
  gtk_container_border_width (GTK_CONTAINER (general_frame5), 5);

  general_vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (general_vbox5);
  gtk_container_add (GTK_CONTAINER (general_frame5), general_vbox5);

  general_label8 = gtk_label_new ("8..20");
  gtk_widget_show (general_label8);
  gtk_box_pack_start (GTK_BOX (general_vbox5), general_label8, FALSE, FALSE, 5);
  gtk_misc_set_alignment (GTK_MISC (general_label8), 0.1, 0.5);

  general_hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (general_hbox5);
  gtk_box_pack_start (GTK_BOX (general_vbox5), general_hbox5, TRUE, TRUE, 0);

  /* The "menu font size" entry widget */
  general_entry5 = gtk_entry_new ();
  gtk_widget_show (general_entry5);
  gtk_box_pack_start (GTK_BOX (general_hbox5), general_entry5, FALSE, FALSE, 5);
  gtk_widget_set_usize (general_entry5, 50, 22);
  gtk_signal_connect (GTK_OBJECT (general_entry5), "changed",
                      GTK_SIGNAL_FUNC (menu_font_size_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (general_entry5), "10");

  general_label9 = gtk_label_new ("");
  gtk_widget_show (general_label9);
  gtk_box_pack_start (GTK_BOX (general_hbox5), general_label9, TRUE, TRUE, 5);
  gtk_misc_set_alignment (GTK_MISC (general_label9), 0.1, 0.5);

  general_hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (general_hseparator1);
  gtk_box_pack_start (GTK_BOX (general_vbox1), general_hseparator1, FALSE, FALSE, 5);

  /* The "general preferences" button box */
  general_hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (general_hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (general_vbox1), general_hbuttonbox1, TRUE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (general_hbuttonbox1), 5);

  /* The "apply" button widget */
  general_button1 = gtk_button_new_with_label ("Apply");
  gtk_widget_show (general_button1);
  gtk_container_add (GTK_CONTAINER (general_hbuttonbox1), general_button1);
  gtk_signal_connect (GTK_OBJECT (general_button1), "clicked",
                      GTK_SIGNAL_FUNC (hp_apply_func),
                      NULL);

  /* The "Defaults" button widget */
  general_button2 = gtk_button_new_with_label ("Defaults");
  gtk_widget_show (general_button2);
  gtk_container_add (GTK_CONTAINER (general_hbuttonbox1), general_button2);
  gtk_signal_connect (GTK_OBJECT (general_button2), "clicked",
                      GTK_SIGNAL_FUNC (hp_defaults_func),
                      NULL);

  /* The "Done" button widget */
  general_button3 = gtk_button_new_with_label ("Done");
  gtk_widget_show (general_button3);
  gtk_container_add (GTK_CONTAINER (general_hbuttonbox1), general_button3);
  gtk_signal_connect (GTK_OBJECT (general_button3), "clicked",
                      GTK_SIGNAL_FUNC (hp_done_func),
                      NULL);
  /********************************************************************************/
  /* Second page of the notebook */
  /* The "publishing preferences" frame*/
  publish_frame1 = gtk_frame_new ("Publishing Preferences");
  gtk_widget_show (publish_frame1);
  gtk_container_add (GTK_CONTAINER (notebook1), publish_frame1);

  publish_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (publish_vbox1);
  gtk_container_add (GTK_CONTAINER (publish_frame1), publish_vbox1);

  publish_vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (publish_vbox2);
  gtk_box_pack_start (GTK_BOX (publish_vbox1), publish_vbox2, TRUE, TRUE, 5);

  /* The "use etags and preconditions" checkbutton widget*/
  publish_checkbutton1 = gtk_check_button_new_with_label ("Use ETAGS and preconditions");
  gtk_widget_show (publish_checkbutton1);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (publish_checkbutton1), TRUE);
  gtk_box_pack_start (GTK_BOX (publish_vbox2), publish_checkbutton1, FALSE, FALSE, 5);
  gtk_signal_connect (GTK_OBJECT (publish_checkbutton1), "clicked",
                      GTK_SIGNAL_FUNC (etags_func),
                      NULL);
  /* The "verifiy each PUT with a GET" checkbutton widget*/
  publish_checkbutton2 = gtk_check_button_new_with_label ("Verify each PUT with a GET");
  gtk_widget_show (publish_checkbutton2);
  gtk_box_pack_start (GTK_BOX (publish_vbox2), publish_checkbutton2, FALSE, FALSE, 5);
  gtk_signal_connect (GTK_OBJECT (publish_checkbutton2), "clicked",
                      GTK_SIGNAL_FUNC (put_with_get_func),
                      NULL);

  publish_hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (publish_hbox1);
  gtk_box_pack_start (GTK_BOX (publish_vbox2), publish_hbox1, FALSE, TRUE, 5);


  publish_label1 = gtk_label_new ("Default name for URLs finishing in '/'");
  gtk_widget_show (publish_label1);
  gtk_box_pack_start (GTK_BOX (publish_hbox1), publish_label1, FALSE, FALSE, 5);
  gtk_label_set_justify (GTK_LABEL (publish_label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (publish_label1), 0.1, 0.5);

  /* The "default name for urls finishing in /" entry widget */
  publish_entry1 = gtk_entry_new ();
  gtk_widget_show (publish_entry1);
  gtk_box_pack_start (GTK_BOX (publish_hbox1), publish_entry1, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (publish_entry1), "changed",
                      GTK_SIGNAL_FUNC (default_url_name_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (publish_entry1), "toto");

  publish_hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (publish_hseparator1);
  gtk_box_pack_start (GTK_BOX (publish_vbox1), publish_hseparator1, FALSE, FALSE, 5);

  /* The "publishing preferences" button box */
  publish_hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (publish_hbuttonbox1);
  gtk_box_pack_end (GTK_BOX (publish_vbox1), publish_hbuttonbox1, FALSE, FALSE, 5);

  /* The "apply" button widget */
  publish_button1 = gtk_button_new_with_label ("Apply");
  gtk_widget_show (publish_button1);
  gtk_container_add (GTK_CONTAINER (publish_hbuttonbox1), publish_button1);
  gtk_signal_connect (GTK_OBJECT (publish_button1), "clicked",
                      GTK_SIGNAL_FUNC (pu_apply_func),
                      NULL);

  /* The "defaults" button widget */
  publish_button2 = gtk_button_new_with_label ("Defaults");
  gtk_widget_show (publish_button2);
  gtk_container_add (GTK_CONTAINER (publish_hbuttonbox1), publish_button2);
  gtk_signal_connect (GTK_OBJECT (publish_button2), "clicked",
                      GTK_SIGNAL_FUNC (pu_defaults_func),
                      NULL);

  /* The "done" button widget */
  publish_button3 = gtk_button_new_with_label ("Done");
  gtk_widget_show (publish_button3);
  gtk_container_add (GTK_CONTAINER (publish_hbuttonbox1), publish_button3);
  gtk_signal_connect (GTK_OBJECT (publish_button3), "clicked",
                      GTK_SIGNAL_FUNC (pu_done_func),
                      NULL);
  /********************************************************************/
  /* Third page of the notebook */
  /* The "cache preferences" frame */
  cache_frame1 = gtk_frame_new ("Cache Preferences");
  gtk_widget_show (cache_frame1);
  gtk_container_add (GTK_CONTAINER (notebook1), cache_frame1);
  gtk_container_border_width (GTK_CONTAINER (cache_frame1), 5);

  cache_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (cache_vbox1);
  gtk_container_add (GTK_CONTAINER (cache_frame1), cache_vbox1);

  cache_hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (cache_hbox1);
  gtk_box_pack_start (GTK_BOX (cache_vbox1), cache_hbox1, FALSE, FALSE, 5);
  gtk_container_border_width (GTK_CONTAINER (cache_hbox1), 5);

  cache_label1 = gtk_label_new ("Cache directory");
  gtk_widget_show (cache_label1);
  gtk_box_pack_start (GTK_BOX (cache_hbox1), cache_label1, FALSE, FALSE, 5);
  gtk_misc_set_alignment (GTK_MISC (cache_label1), 0.1, 0.5);

  /* The "cache directory" entry widget */
  cache_entry1 = gtk_entry_new ();
  gtk_widget_show (cache_entry1);
  gtk_box_pack_start (GTK_BOX (cache_hbox1), cache_entry1, TRUE, TRUE, 5);
  gtk_signal_connect (GTK_OBJECT (cache_entry1), "changed",
                      GTK_SIGNAL_FUNC (cache_direcory_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (cache_entry1), "/");

  /* The "enable cache" checkbutton widget */
  cache_checkbutton1 = gtk_check_button_new_with_label ("Enable Cache");
  gtk_widget_show (cache_checkbutton1);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cache_checkbutton1), TRUE);
  gtk_box_pack_start (GTK_BOX (cache_vbox1), cache_checkbutton1, FALSE, FALSE, 5);
  gtk_signal_connect (GTK_OBJECT (cache_checkbutton1), "clicked",
                      GTK_SIGNAL_FUNC (enable_cache_func),
                      NULL);

 /* The "cache protected document" checkbutton widget */
  cache_checkbutton2 = gtk_check_button_new_with_label ("Cache Protected documents");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cache_checkbutton2), TRUE);
  gtk_widget_show (cache_checkbutton2);
  gtk_box_pack_start (GTK_BOX (cache_vbox1), cache_checkbutton2, FALSE, FALSE, 5);
  gtk_signal_connect (GTK_OBJECT (cache_checkbutton2), "clicked",
                      GTK_SIGNAL_FUNC (Cache_protected_doc_func),
                      NULL);

  /* The "disconnected mode" checkbutton widget */
  cache_checkbutton3 = gtk_check_button_new_with_label ("Disconnected mode");
  gtk_widget_show (cache_checkbutton3);
  gtk_box_pack_start (GTK_BOX (cache_vbox1), cache_checkbutton3, FALSE, FALSE, 5);
  gtk_signal_connect (GTK_OBJECT (cache_checkbutton3), "clicked",
                      GTK_SIGNAL_FUNC (disconnected_mod_func),
                      NULL);

 /* The "ignore expires : header" checkbutton widget */
  cache_checkbutton4 = gtk_check_button_new_with_label ("Ignore Expires : header");
  gtk_widget_show (cache_checkbutton4);
  gtk_box_pack_start (GTK_BOX (cache_vbox1), cache_checkbutton4, FALSE, FALSE, 5);
  gtk_signal_connect (GTK_OBJECT (cache_checkbutton4), "clicked",
                      GTK_SIGNAL_FUNC (ignore_expires_func),
                      NULL);

 

  cache_hbox2 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (cache_hbox2);
  gtk_box_pack_start (GTK_BOX (cache_vbox1), cache_hbox2, FALSE, FALSE, 5);
  gtk_container_border_width (GTK_CONTAINER (cache_hbox2), 5);

  /* The "cache size" frame */
  cache_frame2 = gtk_frame_new ("Cache size (Mb)");
  gtk_widget_show (cache_frame2);
  gtk_box_pack_start (GTK_BOX (cache_hbox2), cache_frame2, TRUE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (cache_frame2), 5);

  cache_vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (cache_vbox2);
  gtk_container_add (GTK_CONTAINER (cache_frame2), cache_vbox2);

  cache_label2 = gtk_label_new ("1..100");
  gtk_widget_show (cache_label2);
  gtk_box_pack_start (GTK_BOX (cache_vbox2), cache_label2, FALSE, FALSE, 5);
  gtk_widget_set_usize (cache_label2, 20, -1);
  gtk_misc_set_alignment (GTK_MISC (cache_label2), 0.1, 0.5);

  cache_hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (cache_hbox3);
  gtk_box_pack_start (GTK_BOX (cache_vbox2), cache_hbox3, FALSE, TRUE, 5);

  /* The "cache size" entry widget */
  cache_entry2 = gtk_entry_new ();
  gtk_widget_show (cache_entry2);
  gtk_box_pack_start (GTK_BOX (cache_hbox3), cache_entry2, FALSE, FALSE, 0);
  gtk_widget_set_usize (cache_entry2, 50, 22);
  gtk_signal_connect (GTK_OBJECT (cache_entry2), "changed",
                      GTK_SIGNAL_FUNC (cache_size_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (cache_entry2), "1");

  cache_label3 = gtk_label_new ("");
  gtk_widget_show (cache_label3);
  gtk_box_pack_start (GTK_BOX (cache_hbox3), cache_label3, TRUE, TRUE, 5);
  gtk_widget_set_usize (cache_label3, 20, -1);
  gtk_misc_set_alignment (GTK_MISC (cache_label3), 0.1, 0.5);

  /* The "cache entry size limit" frame */
  cache_frame3 = gtk_frame_new ("Cache entry size limit (Mb)");
  gtk_widget_show (cache_frame3);
  gtk_box_pack_start (GTK_BOX (cache_hbox2), cache_frame3, TRUE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (cache_frame3), 5);

  cache_vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (cache_vbox3);
  gtk_container_add (GTK_CONTAINER (cache_frame3), cache_vbox3);

  cache_label4 = gtk_label_new ("1..5");
  gtk_widget_show (cache_label4);
  gtk_box_pack_start (GTK_BOX (cache_vbox3), cache_label4, FALSE, FALSE, 5);
  gtk_misc_set_alignment (GTK_MISC (cache_label4), 0.1, 0.5);

  cache_hbox4 = gtk_hbutton_box_new ();
  gtk_widget_show (cache_hbox4);
  gtk_box_pack_start (GTK_BOX (cache_vbox3), cache_hbox4, FALSE, TRUE, 5);

 /* The "cache entry size limit" entry widget */
  cache_entry3 = gtk_entry_new ();
  gtk_widget_show (cache_entry3);
  gtk_box_pack_start (GTK_BOX (cache_hbox4), cache_entry3, FALSE, FALSE, 5);
  gtk_widget_set_usize (cache_entry3, 50, 22);
  gtk_signal_connect (GTK_OBJECT (cache_entry3), "changed",
                      GTK_SIGNAL_FUNC (cache_entry_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (cache_entry3), "3");

  cache_label5 = gtk_label_new ("");
  gtk_widget_show (cache_label5);
  gtk_box_pack_start (GTK_BOX (cache_hbox4), cache_label5, TRUE, TRUE, 5);
  gtk_misc_set_alignment (GTK_MISC (cache_label5), 0.1, 0.5);

  cache_hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (cache_hseparator1);
  gtk_box_pack_start (GTK_BOX (cache_vbox1), cache_hseparator1, FALSE, FALSE, 5);

  /* The "cache preferences" button box */
  cache_hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (cache_hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (cache_vbox1), cache_hbuttonbox1, TRUE, TRUE, 5);

  /* The "apply" button widget */
  cache_button1 = gtk_button_new_with_label ("Apply");
  gtk_widget_show (cache_button1);
  gtk_container_add (GTK_CONTAINER (cache_hbuttonbox1), cache_button1);
  gtk_signal_connect (GTK_OBJECT (cache_button1), "clicked",
                      GTK_SIGNAL_FUNC (cp_apply_func),
                      NULL);

  /* The "defaults" button widget */
  cache_button2 = gtk_button_new_with_label ("Defaults");
  gtk_widget_show (cache_button2);
  gtk_container_add (GTK_CONTAINER (cache_hbuttonbox1), cache_button2);
  gtk_signal_connect (GTK_OBJECT (cache_button2), "clicked",
                      GTK_SIGNAL_FUNC (cp_defaults_func),
                      NULL);

  /* The "flush cache" button widget */
  cache_button3 = gtk_button_new_with_label ("Flush cache");
  gtk_widget_show (cache_button3);
  gtk_container_add (GTK_CONTAINER (cache_hbuttonbox1), cache_button3);
  gtk_signal_connect (GTK_OBJECT (cache_button3), "clicked",
                      GTK_SIGNAL_FUNC (cp_flush_func),
                      NULL);

  /* The "done" button widget */
  cache_button4 = gtk_button_new_with_label ("Done");
  gtk_widget_show (cache_button4);
  gtk_container_add (GTK_CONTAINER (cache_hbuttonbox1), cache_button4);
  gtk_signal_connect (GTK_OBJECT (cache_button4), "clicked",
                      GTK_SIGNAL_FUNC (cp_done_func),
                      NULL);
  /****************************************************************************/
  /* The fourth page of the notebook */
  /* The "proxy preferences" frame */
  proxy_frame1 = gtk_frame_new ("Proxy Preferences");
  gtk_widget_show (proxy_frame1);
  gtk_container_add (GTK_CONTAINER (notebook1), proxy_frame1);

  proxy_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (proxy_vbox1);
  gtk_container_add (GTK_CONTAINER (proxy_frame1), proxy_vbox1);

  proxy_table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (proxy_table1);
  gtk_box_pack_start (GTK_BOX (proxy_vbox1), proxy_table1, TRUE, TRUE, 5);

  proxy_label1 = gtk_label_new ("");
  gtk_widget_show (proxy_label1);
  gtk_table_attach (GTK_TABLE (proxy_table1), proxy_label1, 0, 1, 2, 3,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  proxy_label2 = gtk_label_new ("");
  gtk_widget_show (proxy_label2);
  gtk_table_attach (GTK_TABLE (proxy_table1), proxy_label2, 1, 2, 2, 3,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  proxy_label3 = gtk_label_new ("HTTP proxy");
  gtk_widget_show (proxy_label3);
  gtk_table_attach (GTK_TABLE (proxy_table1), proxy_label3, 0, 1, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 5, 5);
  gtk_label_set_justify (GTK_LABEL (proxy_label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (proxy_label3), 0.1, 0.5);

  proxy_label4 = gtk_label_new ("No proxy on these domains");
  gtk_widget_show (proxy_label4);
  gtk_table_attach (GTK_TABLE (proxy_table1), proxy_label4, 0, 1, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 5, 5);
  gtk_label_set_justify (GTK_LABEL (proxy_label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (proxy_label4), 0.1, 0.5);

  /* The "http proxy" entry widget */
  proxy_entry1 = gtk_entry_new ();
  gtk_widget_show (proxy_entry1);
  gtk_entry_set_text (GTK_ENTRY (proxy_entry1), "h");
  gtk_table_attach (GTK_TABLE (proxy_table1), proxy_entry1, 1, 2, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 5, 5);
  gtk_signal_connect (GTK_OBJECT (proxy_entry1), "changed",
                      GTK_SIGNAL_FUNC (no_proxy_func),
                      NULL);
  

 /* The "no proxy on these domains" entry widget */
  proxy_entry2 = gtk_entry_new ();
  gtk_widget_show (proxy_entry2);
  gtk_entry_set_text (GTK_ENTRY (proxy_entry2), "f");
  gtk_table_attach (GTK_TABLE (proxy_table1), proxy_entry2, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_FILL, 5, 5);
  gtk_signal_connect (GTK_OBJECT (proxy_entry2), "changed",
                      GTK_SIGNAL_FUNC (http_proxy_func),
                      NULL);
  

  proxy_hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (proxy_hseparator1);
  gtk_box_pack_start (GTK_BOX (proxy_vbox1), proxy_hseparator1, FALSE, FALSE, 5);

  /* The "Proxy preferences" button box */
  proxy_hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (proxy_hbuttonbox1);
  gtk_box_pack_end (GTK_BOX (proxy_vbox1), proxy_hbuttonbox1, FALSE, FALSE, 5);

  /* The "apply" button widget */
  proxy_button1 = gtk_button_new_with_label ("Apply");
  gtk_widget_show (proxy_button1);
  gtk_container_add (GTK_CONTAINER (proxy_hbuttonbox1), proxy_button1);
  gtk_signal_connect (GTK_OBJECT (proxy_button1), "clicked",
                      GTK_SIGNAL_FUNC (pr_apply_func),
                      NULL);

  /* The "defaults" button widget */
  proxy_button2 = gtk_button_new_with_label ("Defaults");
  gtk_widget_show (proxy_button2);
  gtk_container_add (GTK_CONTAINER (proxy_hbuttonbox1), proxy_button2);
  gtk_signal_connect (GTK_OBJECT (proxy_button2), "clicked",
                      GTK_SIGNAL_FUNC (pr_defaults_func),
                      NULL);

  /* The "done" button widget */
  proxy_button3 = gtk_button_new_with_label ("Done");
  gtk_widget_show (proxy_button3);
  gtk_container_add (GTK_CONTAINER (proxy_hbuttonbox1), proxy_button3);
  gtk_signal_connect (GTK_OBJECT (proxy_button3), "clicked",
                      GTK_SIGNAL_FUNC (pr_done_func),
                      NULL);
  /**************************************************************************/
  /* The fifth page of the notebook */
  /* The "colors preferences" frame */
  color_frame1 = gtk_frame_new ("Color Preferences");
  gtk_widget_show (color_frame1);
  gtk_container_add (GTK_CONTAINER (notebook1), color_frame1);

  color_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (color_vbox1);
  gtk_container_add (GTK_CONTAINER (color_frame1), color_vbox1);

  color_table1 = gtk_table_new (4, 2, TRUE);
  gtk_widget_show (color_table1);
  gtk_box_pack_start (GTK_BOX (color_vbox1), color_table1, FALSE, FALSE, 0);
  gtk_container_border_width (GTK_CONTAINER (color_table1), 5);

  color_label1 = gtk_label_new ("Document foreground color");
  gtk_widget_show (color_label1);
  gtk_table_attach (GTK_TABLE (color_table1), color_label1, 0, 1, 0, 1,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_misc_set_alignment (GTK_MISC (color_label1), 0.1, 0.5);

  /* The "document foreground color" entry widget */
  color_entry1 = gtk_entry_new ();
  gtk_widget_show (color_entry1);
  gtk_table_attach (GTK_TABLE (color_table1), color_entry1, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_signal_connect (GTK_OBJECT (color_entry1), "changed",
                      GTK_SIGNAL_FUNC (doc_forgrd_clr_func),
                      NULL);
 // gtk_entry_set_text (GTK_ENTRY (color_entry1), "B");

  color_label2 = gtk_label_new ("Document background color");
  gtk_widget_show (color_label2);
  gtk_table_attach (GTK_TABLE (color_table1), color_label2, 0, 1, 1, 2,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_label_set_justify (GTK_LABEL (color_label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (color_label2), 0.1, 0.5);

  /* The "document background color" entry widget */
  color_entry2 = gtk_entry_new ();
  gtk_widget_show (color_entry2);
  gtk_table_attach (GTK_TABLE (color_table1), color_entry2, 1, 2, 1, 2,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_signal_connect (GTK_OBJECT (color_entry2), "changed",
                      GTK_SIGNAL_FUNC (doc_backgrd_clr_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (color_entry2), "t");

  /* The "menu foreground color" entry widget */
  color_entry3 = gtk_entry_new ();
  gtk_widget_show (color_entry3);
  gtk_table_attach (GTK_TABLE (color_table1), color_entry3, 1, 2, 2, 3,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_signal_connect (GTK_OBJECT (color_entry3), "changed",
                      GTK_SIGNAL_FUNC (menu_forgrd_clr_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (color_entry3), "b");

  /* The "menu background color" entry widget */
  color_entry4 = gtk_entry_new ();
  gtk_widget_show (color_entry4);
  gtk_table_attach (GTK_TABLE (color_table1), color_entry4, 1, 2, 3, 4,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_signal_connect (GTK_OBJECT (color_entry4), "changed",
                      GTK_SIGNAL_FUNC (menu_backgrd_clr_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (color_entry4), "t");

  color_label3 = gtk_label_new ("Menu foreground color");
  gtk_widget_show (color_label3);
  gtk_table_attach (GTK_TABLE (color_table1), color_label3, 0, 1, 2, 3,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_label_set_justify (GTK_LABEL (color_label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (color_label3), 0.1, 0.5);

  color_label4 = gtk_label_new ("Menu background color");
  gtk_widget_show (color_label4);
  gtk_table_attach (GTK_TABLE (color_table1), color_label4, 0, 1, 3, 4,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 
		    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 5);
  gtk_label_set_justify (GTK_LABEL (color_label4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (color_label4), 0.1, 0.5);

  color_label5 = gtk_label_new (" ");
  gtk_widget_show (color_label5);
  gtk_box_pack_start(GTK_BOX (color_vbox1), color_label5, TRUE, TRUE, 5);
  gtk_label_set_justify (GTK_LABEL (color_label5), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (color_label5), 0.1, 0.5);

  color_label6 = gtk_label_new ("The change will be effective when you open a new window");
  gtk_widget_show (color_label6);
  gtk_box_pack_start(GTK_BOX (color_vbox1), color_label6, FALSE, FALSE, 5);
  gtk_label_set_justify (GTK_LABEL (color_label6), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (color_label6), 0.1, 0.5);

  color_hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (color_hseparator1);
  gtk_box_pack_start (GTK_BOX (color_vbox1), color_hseparator1, FALSE, FALSE, 5);
  
  /* The "color preferences" button box */
  color_hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (color_hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (color_vbox1), color_hbuttonbox1, FALSE, FALSE, 5);

  /* The "apply" button widget */
  color_button1 = gtk_button_new_with_label ("Apply");
  gtk_widget_show (color_button1);
  gtk_container_add (GTK_CONTAINER (color_hbuttonbox1), color_button1);
  gtk_signal_connect (GTK_OBJECT (color_button1), "clicked",
                      GTK_SIGNAL_FUNC (co_apply_func),
                      NULL);

  /* The "defaults" button widget */
  color_button2 = gtk_button_new_with_label ("Defaults");
  gtk_widget_show (color_button2);
  gtk_container_add (GTK_CONTAINER (color_hbuttonbox1), color_button2);
  gtk_signal_connect (GTK_OBJECT (color_button2), "clicked",
                      GTK_SIGNAL_FUNC (co_defaults_func),
                      NULL);

  /* The "done" button widget */
  color_button3 = gtk_button_new_with_label ("Done");
  gtk_widget_show (color_button3);
  gtk_container_add (GTK_CONTAINER (color_hbuttonbox1), color_button3);
  gtk_signal_connect (GTK_OBJECT (color_button3), "clicked",
                      GTK_SIGNAL_FUNC (co_done_func),
                      NULL);

  /***************************************************************/
  /* The sixth page of the notebook */
  /* The "geometry preferences" of the notebook */
  geo_frame1 = gtk_frame_new ("Geometry Preferences");
  gtk_widget_show (geo_frame1);
  gtk_container_add (GTK_CONTAINER (notebook1), geo_frame1);

  geo_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (geo_vbox1);
  gtk_container_add (GTK_CONTAINER (geo_frame1), geo_vbox1);

  geo_label1 = gtk_label_new (" ");
  gtk_widget_show (geo_label1);
  gtk_box_pack_start (GTK_BOX (geo_vbox1), geo_label1, TRUE, TRUE, 0);
  gtk_label_set_justify (GTK_LABEL (geo_label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (geo_label1), 0.1, 0.5);

  geo_label2 = gtk_label_new ("The change will be effective when you open a new window.");
  gtk_widget_show (geo_label2);
  gtk_box_pack_start (GTK_BOX (geo_vbox1), geo_label2, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (geo_label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (geo_label2), 0.1, 0.5);

  geo_hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (geo_hseparator1);
  gtk_box_pack_start (GTK_BOX (geo_vbox1), geo_hseparator1, FALSE, FALSE, 5);
  
  /* The "geometry preferences" button box */
  geo_hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (geo_hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (geo_vbox1), geo_hbuttonbox1, FALSE, FALSE, 5);

  /* The "save current geometry" button widet */
  geo_button1 = gtk_button_new_with_label ("Save current geometry");
  gtk_widget_show (geo_button1);
  gtk_container_add (GTK_CONTAINER (geo_hbuttonbox1), geo_button1);
  gtk_signal_connect (GTK_OBJECT (geo_button1), "clicked",
                      GTK_SIGNAL_FUNC (save_current_geo_func),
                      NULL);
  
  /* The "restore default geometry" button widget */
  geo_button2 = gtk_button_new_with_label ("Restore default geometry");
  gtk_widget_show (geo_button2);
  gtk_container_add (GTK_CONTAINER (geo_hbuttonbox1), geo_button2);
  gtk_signal_connect (GTK_OBJECT (geo_button2), "clicked",
                      GTK_SIGNAL_FUNC (restore_default_geo_func),
                      NULL);
  
  /* The "done" button widget */
  geo_button3 = gtk_button_new_with_label ("Done");
  gtk_widget_show (geo_button3);
  gtk_container_add (GTK_CONTAINER (geo_hbuttonbox1), geo_button3);
  gtk_signal_connect (GTK_OBJECT (geo_button3), "clicked",
                      GTK_SIGNAL_FUNC (ge_done_func),
                      NULL);
  /**************************************************************/
  /* The seventh page of the notebook */
  /* The "language preferences" frame */
  language_frame1 = gtk_frame_new ("Language Preferences");
  gtk_widget_show (language_frame1);
  gtk_container_add (GTK_CONTAINER (notebook1), language_frame1);

  language_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (language_vbox1);
  gtk_container_add (GTK_CONTAINER (language_frame1), language_vbox1);

  language_vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (language_vbox2);
  gtk_box_pack_start (GTK_BOX (language_vbox1), language_vbox2, TRUE, TRUE, 5);

  language_label1 = gtk_label_new ("List of preferred languages like \"en, fr\" :");
  gtk_widget_show (language_label1);
  gtk_box_pack_start (GTK_BOX (language_vbox2), language_label1, FALSE, FALSE, 5);
  gtk_misc_set_alignment (GTK_MISC (language_label1), 0.1, 0.5);

  /* The "list of preferred language" entry widget */
  language_entry1 = gtk_entry_new ();
  gtk_widget_show (language_entry1);
  gtk_box_pack_start (GTK_BOX (language_vbox2), language_entry1, FALSE, TRUE, 5);
  gtk_entry_set_text (GTK_ENTRY (language_entry1), "a");
  gtk_signal_connect (GTK_OBJECT (language_entry1), "changed",
                      GTK_SIGNAL_FUNC (list_language_func),
                      NULL);

  language_label2 = gtk_label_new (" ");
  gtk_widget_show (language_label2);
  gtk_box_pack_start (GTK_BOX (language_vbox2), language_label2, TRUE, TRUE, 5);
  gtk_misc_set_alignment (GTK_MISC (language_label2), 0.1, 0.5);


  language_hseparator1 = gtk_hseparator_new ();
  gtk_widget_show (language_hseparator1);
  gtk_box_pack_start (GTK_BOX (language_vbox1), language_hseparator1, FALSE, FALSE, 5);

  /* The "language preferences" button box widget */
  language_hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (language_hbuttonbox1);
  gtk_box_pack_end (GTK_BOX (language_vbox1), language_hbuttonbox1, FALSE, FALSE, 5);

  /* The "apply" button widget */
  language_button1 = gtk_button_new_with_label ("Apply");
  gtk_widget_show (language_button1);
  gtk_container_add (GTK_CONTAINER (language_hbuttonbox1), language_button1);
  gtk_signal_connect (GTK_OBJECT (language_button1), "clicked",
                      GTK_SIGNAL_FUNC (la_apply_func),
                      NULL);

  /* The "defaults" button widget */
  language_button2 = gtk_button_new_with_label ("Defaults");
  gtk_widget_show (language_button2);
  gtk_container_add (GTK_CONTAINER (language_hbuttonbox1), language_button2);
  gtk_signal_connect (GTK_OBJECT (language_button2), "clicked",
                      GTK_SIGNAL_FUNC (la_defaults_func),
                      NULL);

  /* The "done" button widget */
  language_button3 = gtk_button_new_with_label ("Done");
  gtk_widget_show (language_button3);
  gtk_container_add (GTK_CONTAINER (language_hbuttonbox1), language_button3);
  gtk_signal_connect (GTK_OBJECT (language_button3), "clicked",
                      GTK_SIGNAL_FUNC (la_done_func),
                      NULL);

  /*************************************************************/
  /* Labels of the notebook pages */
  label1 = gtk_label_new ("General");
  gtk_widget_show (label1);
  set_notebook_tab (notebook1, 0, label1);

  label2 = gtk_label_new ("Publishing");
  gtk_widget_show (label2);
  set_notebook_tab (notebook1, 1, label2);

  label3 = gtk_label_new ("Cache");
  gtk_widget_show (label3);
  set_notebook_tab (notebook1, 2, label3);

  label4 = gtk_label_new ("Proxy");
  gtk_widget_show (label4);
  set_notebook_tab (notebook1, 3, label4);

  label5 = gtk_label_new ("colors");
  gtk_widget_show (label5);
  set_notebook_tab (notebook1, 4, label5);

  label6 = gtk_label_new ("Geometry");
  gtk_widget_show (label6);
  set_notebook_tab (notebook1, 5, label6);

  label7 = gtk_label_new ("Language");
  gtk_widget_show (label7);
  set_notebook_tab (notebook1, 6, label7);

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  return dialog1;
}
/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Search" dialog box 
----------------------------------------------------------------*/
static void Initialisation_search ()/* il faudra faire un free */
{
  search_form = (Search_form *)TtaGetMemory(sizeof(Search_form));
  search_form->replace = 1;
  search_form->selection = 3;
  search_form->uppercase = FALSE;
  search_form->search_for = TEXT(" ");
  search_form->replace_by = TEXT(" ");
}
/*--------------------------------------------------------------------------
 Callback function called by the "Search for" text _entry 
------------------------------------------------------------------------- */
static void  sf_func (GtkWidget *widget, gpointer data) 
{
   search_form->search_for = gtk_entry_get_text (GTK_ENTRY (entry_search1)); 
}
/*--------------------------------------------------------------------------
 Callback function called by the "Replace by" text _entry 
------------------------------------------------------------------------- */
static void  rb_func (GtkWidget *widget, gpointer data) 
{
   search_form->replace_by = gtk_entry_get_text (GTK_ENTRY (entry_search2)); 
}
/*--------------------------------------------------------------------------
 Callback function called by the "Uppercase = lowercase" checkbutton
------------------------------------------------------------------------- */
static void ul_func (GtkWidget *widget, gpointer data) 
{
  if (GTK_TOGGLE_BUTTON (widget)->active) 
    search_form->uppercase = TRUE;
  else
    search_form->uppercase = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by a "No Replace" radio_button
------------------------------------------------------------------------- */
static void nr_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      search_form->replace = 1;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "Replace on request" radio_button
------------------------------------------------------------------------- */
static void ron_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      search_form->replace = 2;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "Automatic_replace" radio_button
------------------------------------------------------------------------- */
static void ar_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      search_form->replace = 3;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "Before selection" radio_button
------------------------------------------------------------------------- */
static void bs_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      search_form->selection = 1;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "Within selection" radio_button
------------------------------------------------------------------------- */
static void ws_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      search_form->selection = 2;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "After selection" radio_button
------------------------------------------------------------------------- */
static void as_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      search_form->selection = 3;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "in the whole document" radio_button
------------------------------------------------------------------------- */
static void itwd_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      search_form->selection = 4;
    }
} 

/*--------------------------------------------------------------------------
 Callback function called by the "Confirm" button
------------------------------------------------------------------------- */
static void Confirm_func (GtkWidget *widget, gpointer data) 
{
  printf("replace :%d\n",search_form->replace);
  printf("search :%d\n",search_form->selection);
  printf(" Search for : %s\n", search_form->search_for);
  printf(" Replace by : %s\n", search_form->replace_by);
  printf ("U=P : %d",search_form->uppercase);
}
/*--------------------------------------------------------------------------
 Callback function called by the "Do not replace" button
------------------------------------------------------------------------- */
static void do_not_replace_func (GtkWidget *widget, gpointer data)
{
  printf("do not replace\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "Done" button
------------------------------------------------------------------------- */
static void done_func (GtkWidget *widget, gpointer data)
{
  printf("done\n");
} 

GtkWidget*
create_dialog_search ()
{
  GtkWidget *dialog_search1;
  GtkWidget *dialog_vbox_search1;
  GtkWidget *vbox_search1;
  GtkWidget *hbox_search1;
  GtkWidget *label_search1;
 
  GtkWidget *checkbutton_search1;
  GtkWidget *hbox_search2;
  GtkWidget *label_search2;
  GtkWidget *hbox_search3;
  GtkWidget *frame_search1;
  GtkWidget *vbox_search2;
  GSList *vbox_search2_group = NULL;
  GtkWidget *radiobutton_search1;
  GtkWidget *radiobutton_search2;
  GtkWidget *radiobutton_search3;
  GtkWidget *frame_search2;
  GtkWidget *vbox_search3;
  GSList *vbox_search3_group = NULL;
  GtkWidget *radiobutton_search4;
  GtkWidget *radiobutton_search5;
  GtkWidget *radiobutton_search6;
  GtkWidget *radiobutton_search7;
  GtkWidget *dialog_action_area_search1;
  GtkWidget *hbuttonbox_search1;
  GtkWidget *button_search1;
  GtkWidget *button_search2;
  GtkWidget *button_search3;
  GtkWidget *label_search3;

  /* Initialisitions */
  Initialisation_search ();
  /* Create the "Search" dialog box */
  dialog_search1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog_search1), "dialog_search1");
  gtk_window_set_policy (GTK_WINDOW (dialog_search1),FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog_search1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT(dialog_search1));
 
  dialog_vbox_search1 = GTK_DIALOG (dialog_search1)->vbox;
  gtk_widget_show (dialog_vbox_search1);

  vbox_search1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_search1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox_search1), vbox_search1, TRUE, TRUE, 0);

  hbox_search1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_search1);
  gtk_box_pack_start (GTK_BOX (vbox_search1), hbox_search1, FALSE, FALSE, 0);
  gtk_container_border_width (GTK_CONTAINER (hbox_search1), 5);

  /* Create the "Search for" label */
  label_search1 = gtk_label_new ("Search for");
  gtk_widget_show (label_search1);
  gtk_box_pack_start (GTK_BOX (hbox_search1), label_search1, FALSE, FALSE, 10);
  gtk_label_set_justify (GTK_LABEL (label_search1), GTK_JUSTIFY_LEFT);
 
  /* Create the "Search for" entry */
  entry_search1 = gtk_entry_new ();
  gtk_widget_show (entry_search1);
  gtk_box_pack_start (GTK_BOX (hbox_search1), entry_search1, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (entry_search1), "changed",
		      GTK_SIGNAL_FUNC (sf_func), NULL);

  /* Create the "UPPERCASE = lowercase" checkbutton */
  checkbutton_search1 = gtk_check_button_new_with_label ("UPPERCASE = lowercase");
  gtk_widget_show (checkbutton_search1);
  gtk_box_pack_start (GTK_BOX (vbox_search1), checkbutton_search1, FALSE, FALSE, 0);
  gtk_container_border_width (GTK_CONTAINER (checkbutton_search1), 5);
  gtk_signal_connect (GTK_OBJECT (checkbutton_search1), "clicked",
		      GTK_SIGNAL_FUNC (ul_func), NULL);

  hbox_search2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_search2);
  gtk_box_pack_start (GTK_BOX (vbox_search1), hbox_search2, FALSE, FALSE, 0);
  gtk_container_border_width (GTK_CONTAINER (hbox_search2), 5);

  /* Create the   "Replace by" label */
  label_search2 = gtk_label_new ("Replace by");
  gtk_widget_show (label_search2);
  gtk_box_pack_start (GTK_BOX (hbox_search2), label_search2, FALSE, FALSE, 10);

  /* Create the  "Replace by" entry */
  entry_search2 = gtk_entry_new ();
  gtk_widget_show (entry_search2);
  gtk_box_pack_start (GTK_BOX (hbox_search2), entry_search2, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (entry_search2), "changed",
		      GTK_SIGNAL_FUNC (rb_func), NULL);

  hbox_search3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_search3);
  gtk_box_pack_start (GTK_BOX (vbox_search1), hbox_search3, FALSE, FALSE, 0);
 
  /* Create the "Search where" frame */
  frame_search2 = gtk_frame_new ("Search where ");
  gtk_widget_show (frame_search2);
  gtk_box_pack_start (GTK_BOX (hbox_search3), frame_search2, FALSE, FALSE, 0);

  vbox_search3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_search3);
  gtk_container_add (GTK_CONTAINER (frame_search2), vbox_search3);

  /* Create the "Before selection"  radiobutton */
  radiobutton_search4 = gtk_radio_button_new_with_label (vbox_search3_group, "Before selection");
  vbox_search3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_search4));
  gtk_widget_show (radiobutton_search4);
  gtk_box_pack_start (GTK_BOX (vbox_search3), radiobutton_search4, FALSE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_search4), "clicked", 
		      GTK_SIGNAL_FUNC (bs_func), NULL);
 
  /* Create the "Within selection"  radiobutton */
  radiobutton_search5 = gtk_radio_button_new_with_label (vbox_search3_group, "Within selection");
  vbox_search3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_search5));
  gtk_widget_show (radiobutton_search5);
  gtk_box_pack_start (GTK_BOX (vbox_search3), radiobutton_search5, FALSE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_search5), "clicked", 
		      GTK_SIGNAL_FUNC (ws_func), NULL);
 
  /* Create the "After selection"  radiobutton */
  radiobutton_search6 = gtk_radio_button_new_with_label (vbox_search3_group, "After selection");
  vbox_search3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_search6));
  gtk_widget_show (radiobutton_search6);
  gtk_box_pack_start (GTK_BOX (vbox_search3), radiobutton_search6, FALSE, TRUE, 0);
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (radiobutton_search6), TRUE);
  gtk_signal_connect (GTK_OBJECT (radiobutton_search6), "clicked", 
		      GTK_SIGNAL_FUNC (as_func), NULL);

  /* Create the "In the whole document" radiobutton */
  radiobutton_search7 = gtk_radio_button_new_with_label (vbox_search3_group, "In the whole document");
  vbox_search3_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_search7));
  gtk_widget_show (radiobutton_search7);
  gtk_box_pack_start (GTK_BOX (vbox_search3), radiobutton_search7, FALSE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_search7), "clicked", 
		      GTK_SIGNAL_FUNC (itwd_func), NULL);

 

  label_search3 = gtk_label_new ("");
  gtk_widget_show (label_search3);
  gtk_box_pack_start (GTK_BOX (hbox_search3), label_search3, TRUE, TRUE, 2);

  /* Create the "Replace" frame */
  frame_search1 = gtk_frame_new ("Replace");
  gtk_widget_show (frame_search1);
  gtk_box_pack_start (GTK_BOX (hbox_search3), frame_search1, FALSE, FALSE, 0);

  vbox_search2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_search2);
  gtk_container_add (GTK_CONTAINER (frame_search1), vbox_search2);

  /* Create the "No replace" radiobutton*/
  radiobutton_search1 = gtk_radio_button_new_with_label (vbox_search2_group, "No replace");
  vbox_search2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_search1));
  gtk_widget_show (radiobutton_search1);
  gtk_box_pack_start (GTK_BOX (vbox_search2), radiobutton_search1, FALSE, TRUE, 0);
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (radiobutton_search1), TRUE);
  gtk_signal_connect (GTK_OBJECT (radiobutton_search1), "clicked", 
		      GTK_SIGNAL_FUNC (nr_func), NULL);

  /* Create the "Replace on request" radiobutton*/
  radiobutton_search2 = gtk_radio_button_new_with_label (vbox_search2_group, "Replace on request");
  vbox_search2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_search2));
  gtk_widget_show (radiobutton_search2);
  gtk_box_pack_start (GTK_BOX (vbox_search2), radiobutton_search2, FALSE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_search2), "clicked", 
		      GTK_SIGNAL_FUNC (ron_func), NULL);

  
  /* Create the "Automatic replace" radiobutton*/
  radiobutton_search3 = gtk_radio_button_new_with_label (vbox_search2_group, "Automatic replace");
  vbox_search2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_search3));
  gtk_widget_show (radiobutton_search3);
  gtk_box_pack_start (GTK_BOX (vbox_search2), radiobutton_search3, FALSE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_search3), "clicked", 
		      GTK_SIGNAL_FUNC (ar_func), NULL);
 

  dialog_action_area_search1 = GTK_DIALOG (dialog_search1)->action_area;
  gtk_widget_show (dialog_action_area_search1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area_search1), 10);

  /* Create the "Search" hbuttonbox */
  hbuttonbox_search1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbuttonbox_search1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area_search1), hbuttonbox_search1, TRUE, TRUE, 0);

  /* Create the "Confirm" button */
  button_search1 = gtk_button_new_with_label ("Confirm");
  GTK_WIDGET_SET_FLAGS (button_search1, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_search1), button_search1);
  gtk_widget_grab_default (button_search1);
  gtk_widget_show (button_search1);
  gtk_signal_connect (GTK_OBJECT (button_search1), "clicked",
		      GTK_SIGNAL_FUNC (Confirm_func), NULL);
 
  /* Create the "Do not replace"  button */
  button_search2 = gtk_button_new_with_label ("Do not replace ");
  GTK_WIDGET_SET_FLAGS (button_search2, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_search1), button_search2);
  gtk_widget_show (button_search2);  
  gtk_signal_connect (GTK_OBJECT (button_search2), "clicked",
		      GTK_SIGNAL_FUNC (do_not_replace_func), NULL);

  /* Create the "Done"  button */
  button_search3 = gtk_button_new_with_label ("Done");
  GTK_WIDGET_SET_FLAGS (button_search3, GTK_CAN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (hbuttonbox_search1), button_search3);
  gtk_widget_show (button_search3); 
  gtk_signal_connect (GTK_OBJECT (button_search3), "clicked",
		      GTK_SIGNAL_FUNC (done_func), NULL);
  return dialog_search1;
}
/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Spell checking" dialog box 
----------------------------------------------------------------*/
static void Initialisation_spell ()/* il faudra faire un free */
{
  spell_form = (Spell_form *)TtaGetMemory(sizeof (Spell_form));
  spell_form->checking = 3;
  spell_form->caps = FALSE;
  spell_form->digits = FALSE;
  spell_form->numerals = FALSE;
  spell_form->containing = FALSE;
  spell_form->nb_proposals = 3;
  spell_form->replaced_word = TEXT(" ");
  spell_form->ignored_word = TEXT(" ");
  spell_form->proposed_word = TEXT(" ");
  
/*spell_form->
spell_form->*/
    }

/*--------------------------------------------------------------------------
 Callback function called by the "Spell checking" text _entry 
------------------------------------------------------------------------- */
static void  sp_func (GtkWidget *widget, gpointer data) 
{
   spell_form->replaced_word = gtk_entry_get_text (GTK_ENTRY (entry_spell1)); 
}
/*--------------------------------------------------------------------------
 Callback function called by the "Number of proposals" text _entry 
------------------------------------------------------------------------- */
static void  nop_func (GtkWidget *widget, gpointer data) 
{
  gchar *temp;

  temp = gtk_entry_get_text (GTK_ENTRY (entry_spell2)); 
  spell_form->nb_proposals = atoi(temp);
}

/*--------------------------------------------------------------------------
 Callback function called by the "Ignore" text _entry 
------------------------------------------------------------------------- */
static void  i_func (GtkWidget *widget, gpointer data) 
{
   spell_form->ignored_word = gtk_entry_get_text (GTK_ENTRY (entry_spell3)); 
}
/*--------------------------------------------------------------------------
 Callback function called by the "Before selection" radio_button
------------------------------------------------------------------------- */
static void spell_bs_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      spell_form->checking = 1;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "Within selection" radio_button
------------------------------------------------------------------------- */
static void spell_ws_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      spell_form->checking = 2;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "After selection" radio_button
------------------------------------------------------------------------- */
static void spell_as_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      spell_form->checking = 3;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "in the whole document" radio_button
------------------------------------------------------------------------- */
static void spell_itwd_func (GtkWidget *widget, gpointer data)
{

  /* Save the selection of the radio button */
  if (GTK_TOGGLE_BUTTON (widget)->active)
    {
      spell_form->checking = 4;
    }
} 
/*--------------------------------------------------------------------------
 Callback function called by the "Words that are all caps" checkbutton
------------------------------------------------------------------------- */
static void wtaac_func (GtkWidget *widget, gpointer data) 
{
  if (GTK_TOGGLE_BUTTON (widget)->active) 
    spell_form->caps = TRUE;
  else
    spell_form->caps = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "words with digits" checkbutton
------------------------------------------------------------------------- */
static void wwd_func (GtkWidget *widget, gpointer data) 
{
  if (GTK_TOGGLE_BUTTON (widget)->active) 
    spell_form->digits = TRUE;
  else
    spell_form->digits = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "roman numerals" checkbutton
------------------------------------------------------------------------- */
static void rn_func (GtkWidget *widget, gpointer data) 
{
  if (GTK_TOGGLE_BUTTON (widget)->active) 
    spell_form->numerals = TRUE;
  else
    spell_form->numerals = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "words containing" checkbutton
------------------------------------------------------------------------- */
static void wc_func (GtkWidget *widget, gpointer data) 
{
  if (GTK_TOGGLE_BUTTON (widget)->active) 
    spell_form->containing = TRUE;
  else
    spell_form->containing = FALSE;
}
/*--------------------------------------------------------------------------
 Callback function called by the "Search" button
------------------------------------------------------------------------- */
static void s_func (GtkButton *button, gpointer  user_data)
{
  gtk_label_set (GTK_LABEL (label_spell1), "Language : English");
  gtk_clist_set_column_title (GTK_CLIST (clist_spell1), 0, "Amaya");
}
/*--------------------------------------------------------------------------
 Callback function called by the "Skip (+dic)" button
------------------------------------------------------------------------- */
void spell_sd_func (GtkButton *button, gpointer  user_data)
{
  printf("skip\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "Replace and next" button
------------------------------------------------------------------------- */
void ran_func (GtkButton *button, gpointer  user_data)
{
  printf("Replace and next\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "Replace (+dic)" button
------------------------------------------------------------------------- */
void rd_func (GtkButton *button, gpointer  user_data)
{
  printf("Replace +dic\n");
}
/*--------------------------------------------------------------------------
 Callback function called by the "Done" button
------------------------------------------------------------------------- */
void spell_done_func (GtkButton *button, gpointer  user_data)
{
 printf ("checking : %d\n", spell_form->checking);
 printf ("caps : %d\n", spell_form->caps);
 printf ("digits : %d\n", spell_form->digits);
 printf ("numerals : %d\n",  spell_form->numerals);
 printf ("containing : %d\n", spell_form->containing);
 printf ("nb : %d\n", spell_form->nb_proposals);
 printf ("replace_word : %s\n", spell_form->replaced_word);
 printf ("ignored_word : %s\n", spell_form->ignored_word);
  
}

 

GtkWidget*
create_dialog_spell ()
{
  GtkWidget *dialog_spell1;
  GtkWidget *dialog_vbox_spell1;
  GtkWidget *table_spell1;
  GtkWidget *vbox_spell1; 
  GtkWidget *label_spell2;
  GtkWidget *label_spell3;
  GtkWidget *frame_spell2;
  GtkWidget *vbox_spell3;
  GtkWidget *label_spell4;
  GtkWidget *frame_spell1;
  GtkWidget *vbox_spell2;
  GSList *vbox_spell2_group = NULL;
  GtkWidget *radiobutton_spell1;
  GtkWidget *radiobutton_spell2;
  GtkWidget *radiobutton_spell3;
  GtkWidget *radiobutton_spell4;
  GtkWidget *frame_spell3;
  GtkWidget *vbox_spell4;
  GtkWidget *checkbutton_spell1;
  GtkWidget *checkbutton_spell2;
  GtkWidget *checkbutton_spell3;
  GtkWidget *checkbutton_spell4;
  GtkWidget *dialog_action_area_spell1;
  GtkWidget *hbox_spell1;
  GtkWidget *button_spell1;
  GtkWidget *button_spell2;
  GtkWidget *button_spell3;
  GtkWidget *button_spell4;
  GtkWidget *button_spell5;
  GtkWidget *frame_spell4;
  GtkWidget *label_spell5;
  GtkWidget *hbox_spell2;
  GtkWidget *label_spell6;
  GtkWidget *vbox_spell5;
 
  /* Initialisations */
  Initialisation_spell ();

  /* Create the spell dialog_box */
  dialog_spell1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog_spell1), "Spell checking");
  gtk_window_set_policy (GTK_WINDOW (dialog_spell1), FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog_spell1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy),GTK_OBJECT (dialog_spell1));

  dialog_vbox_spell1 = GTK_DIALOG (dialog_spell1)->vbox;
  gtk_widget_show (dialog_vbox_spell1);

  table_spell1 = gtk_table_new (2, 3, FALSE);
  gtk_widget_show (table_spell1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox_spell1), table_spell1, TRUE, TRUE, 0);
 
 /* Create the "Spell checking" frame  */
  frame_spell4 = gtk_frame_new ("Spell checking");
  gtk_widget_show (frame_spell4);
  gtk_table_attach (GTK_TABLE (table_spell1), frame_spell4, 0,1, 0,1,
                    0 , 0, 0, 0);
  
  vbox_spell1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_spell1);
  gtk_container_add (GTK_CONTAINER (frame_spell4), vbox_spell1);
  

  label_spell1 = gtk_label_new ("");
  gtk_widget_show (label_spell1);
  gtk_box_pack_start (GTK_BOX (vbox_spell1), label_spell1, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label_spell1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_spell1), 0.1, 0.5);


  clist_spell1 = gtk_clist_new (1);
  gtk_widget_show (clist_spell1);
  gtk_box_pack_start (GTK_BOX (vbox_spell1), clist_spell1, FALSE, FALSE, 0);
  gtk_widget_set_usize (clist_spell1, 50, 100);
  gtk_clist_set_column_width (GTK_CLIST (clist_spell1), 0, 80);
  gtk_clist_column_titles_show (GTK_CLIST (clist_spell1));

  label_spell3 = gtk_label_new ("");
  gtk_widget_show (label_spell3);
  gtk_clist_set_column_widget (GTK_CLIST (clist_spell1), 0, label_spell3);

  entry_spell1 = gtk_entry_new ();
  gtk_widget_show (entry_spell1);
  gtk_box_pack_start (GTK_BOX (vbox_spell1), entry_spell1, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (entry_spell1), "changed",
		      GTK_SIGNAL_FUNC (sp_func), NULL);

  label_spell2 = gtk_label_new ("");
  gtk_widget_show (label_spell2);
  gtk_table_attach (GTK_TABLE (table_spell1), label_spell2, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_EXPAND|GTK_FILL, (GtkAttachOptions) GTK_EXPAND|GTK_FILL, 0, 0);

  vbox_spell5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_spell5);
  gtk_table_attach (GTK_TABLE (table_spell1), vbox_spell5, 0, 1, 1, 2,
                    0, 0, 0, 0);
  frame_spell2 = gtk_frame_new ("Number of proposals");
  gtk_widget_show (frame_spell2);
  gtk_box_pack_start (GTK_BOX (vbox_spell5), frame_spell2, FALSE, FALSE, 0);
 

  vbox_spell3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_spell3);
  gtk_container_add (GTK_CONTAINER (frame_spell2), vbox_spell3);

  label_spell4 = gtk_label_new ("_spell1 .. _spell10");
  gtk_widget_show (label_spell4);
  gtk_box_pack_start (GTK_BOX (vbox_spell3), label_spell4, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label_spell4), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (label_spell4), 0.1, 0.5);

  hbox_spell2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_spell2);
  gtk_box_pack_start (GTK_BOX (vbox_spell3), hbox_spell2, FALSE, FALSE, 0);

  entry_spell2 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry_spell2), "_spell3");
  gtk_widget_show (entry_spell2);
  gtk_box_pack_start (GTK_BOX (hbox_spell2), entry_spell2, FALSE, FALSE, 0);
  gtk_widget_set_usize (entry_spell2, 35, -1);
  gtk_signal_connect (GTK_OBJECT (entry_spell2), "changed",
		      GTK_SIGNAL_FUNC (nop_func), NULL);

  label_spell6 = gtk_label_new ("");
  gtk_widget_show (label_spell6);
  gtk_box_pack_start (GTK_BOX (hbox_spell2), label_spell6, TRUE, TRUE, 0);

  label_spell5 = gtk_label_new ("Not found"); 
  gtk_widget_show (label_spell5);
  gtk_box_pack_start (GTK_BOX (vbox_spell5), label_spell5, TRUE, TRUE, 20);
  gtk_misc_set_alignment (GTK_MISC (label_spell5), 0.1, 0.5);

  frame_spell1 = gtk_frame_new ("Checking");
  gtk_widget_show (frame_spell1);
  gtk_table_attach (GTK_TABLE (table_spell1), frame_spell1, 2, 3, 0, 1,
                    (GtkAttachOptions)GTK_EXPAND , (GtkAttachOptions)GTK_EXPAND , 0, 0);

  vbox_spell2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_spell2);
  gtk_container_add (GTK_CONTAINER (frame_spell1), vbox_spell2);

  radiobutton_spell1 = gtk_radio_button_new_with_label (vbox_spell2_group, "Before selection");
  vbox_spell2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_spell1));
  gtk_widget_show (radiobutton_spell1);
  gtk_box_pack_start (GTK_BOX (vbox_spell2), radiobutton_spell1, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_spell1), "clicked", 
		      GTK_SIGNAL_FUNC (spell_bs_func), NULL);

  radiobutton_spell2 = gtk_radio_button_new_with_label (vbox_spell2_group, "Within selection");
  vbox_spell2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_spell2));
  gtk_widget_show (radiobutton_spell2);
  gtk_box_pack_start (GTK_BOX (vbox_spell2), radiobutton_spell2, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_spell2), "clicked", 
		      GTK_SIGNAL_FUNC (spell_ws_func), NULL);

  radiobutton_spell3 = gtk_radio_button_new_with_label (vbox_spell2_group, "After selection");
  vbox_spell2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_spell3));
  gtk_widget_show (radiobutton_spell3);
  gtk_box_pack_start (GTK_BOX (vbox_spell2), radiobutton_spell3, TRUE, TRUE, 0);
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (radiobutton_spell3), TRUE);
  gtk_signal_connect (GTK_OBJECT (radiobutton_spell3), "clicked", 
		      GTK_SIGNAL_FUNC (spell_as_func), NULL);

  radiobutton_spell4 = gtk_radio_button_new_with_label (vbox_spell2_group, "In the whole document");
  vbox_spell2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (radiobutton_spell4));
  gtk_widget_show (radiobutton_spell4);
  gtk_box_pack_start (GTK_BOX (vbox_spell2), radiobutton_spell4, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (radiobutton_spell4), "clicked", 
		      GTK_SIGNAL_FUNC (spell_itwd_func), NULL);


  frame_spell3 = gtk_frame_new ("Ignore");
  gtk_widget_show (frame_spell3);
  gtk_table_attach (GTK_TABLE (table_spell1), frame_spell3, 2, 3, 1, 2,
                    (GtkAttachOptions)GTK_EXPAND , (GtkAttachOptions)GTK_EXPAND , 0, 0);

  vbox_spell4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_spell4);
  gtk_container_add (GTK_CONTAINER (frame_spell3), vbox_spell4);

  checkbutton_spell1 = gtk_check_button_new_with_label ("words that are all caps");
  gtk_widget_show (checkbutton_spell1);
  gtk_box_pack_start (GTK_BOX (vbox_spell4), checkbutton_spell1, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (checkbutton_spell1), "clicked",
		      GTK_SIGNAL_FUNC (wtaac_func), NULL);

  checkbutton_spell2 = gtk_check_button_new_with_label ("words with digits");
  gtk_widget_show (checkbutton_spell2);
  gtk_box_pack_start (GTK_BOX (vbox_spell4), checkbutton_spell2, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (checkbutton_spell2), "clicked",
		      GTK_SIGNAL_FUNC (wwd_func), NULL);

  checkbutton_spell3 = gtk_check_button_new_with_label ("roman numerals");
  gtk_widget_show (checkbutton_spell3);
  gtk_box_pack_start (GTK_BOX (vbox_spell4), checkbutton_spell3, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (checkbutton_spell3), "clicked",
		      GTK_SIGNAL_FUNC (rn_func), NULL);

  checkbutton_spell4 = gtk_check_button_new_with_label ("words containing :");
  gtk_widget_show (checkbutton_spell4);
  gtk_box_pack_start (GTK_BOX (vbox_spell4), checkbutton_spell4, TRUE, TRUE, 0);
  gtk_signal_connect (GTK_OBJECT (checkbutton_spell4), "clicked",
		      GTK_SIGNAL_FUNC (wc_func), NULL);

  entry_spell3 = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry_spell3), "@#$&+~");
  gtk_widget_show (entry_spell3);
  gtk_box_pack_start (GTK_BOX (vbox_spell4), entry_spell3, FALSE, FALSE, 0);
  gtk_signal_connect (GTK_OBJECT (entry_spell3), "changed",
		      GTK_SIGNAL_FUNC (i_func), NULL);

  dialog_action_area_spell1 = GTK_DIALOG (dialog_spell1)->action_area;
  gtk_widget_show (dialog_action_area_spell1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area_spell1), 10);

  hbox_spell1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_spell1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area_spell1), hbox_spell1, TRUE, FALSE, 0);

  button_spell1 = gtk_button_new_with_label ("Search");
  GTK_WIDGET_SET_FLAGS (button_spell1, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (hbox_spell1), button_spell1, FALSE, FALSE, 0); 
  gtk_widget_show (button_spell1);
  gtk_widget_grab_default (button_spell1);
  gtk_signal_connect (GTK_OBJECT (button_spell1), "clicked",
                      GTK_SIGNAL_FUNC (s_func),
                      NULL);

  button_spell2 = gtk_button_new_with_label ("Skip (+disc)");
  GTK_WIDGET_SET_FLAGS (button_spell2, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (hbox_spell1), button_spell2, FALSE, FALSE, 0);
  gtk_widget_show (button_spell2);
  gtk_signal_connect (GTK_OBJECT (button_spell2), "clicked",
                      GTK_SIGNAL_FUNC (spell_sd_func),
                      NULL);

  button_spell3 = gtk_button_new_with_label ("Replace and next");
  GTK_WIDGET_SET_FLAGS (button_spell3, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (hbox_spell1), button_spell3, FALSE, FALSE, 0);
  gtk_widget_show (button_spell3);
  gtk_signal_connect (GTK_OBJECT (button_spell3), "clicked",
                      GTK_SIGNAL_FUNC (ran_func),
                      NULL);

  button_spell4 = gtk_button_new_with_label ("Replace (+dic)");
  GTK_WIDGET_SET_FLAGS (button_spell4, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (hbox_spell1), button_spell4, FALSE, FALSE, 0);
  gtk_widget_show (button_spell4);
  gtk_signal_connect (GTK_OBJECT (button_spell4), "clicked",
                      GTK_SIGNAL_FUNC (rd_func),
                      NULL);

  button_spell5 = gtk_button_new_with_label ("Done");
  GTK_WIDGET_SET_FLAGS (button_spell5, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (hbox_spell1), button_spell5, FALSE, FALSE, 0);
  gtk_widget_show (button_spell5);
  gtk_signal_connect (GTK_OBJECT (button_spell5), "clicked",
                      GTK_SIGNAL_FUNC (spell_done_func),
                      NULL);

  return dialog_spell1;
}
/****************************************************************/
/*----------------------------------------------------------------
Functions of the "Table" dialog box 
----------------------------------------------------------------*/
static void Initialisation_table ()/* il faudra faire un free */
{
  table = (Table_form *)TtaGetMemory (sizeof (Table_form));
  table->nb_columns = 2;
  table->nb_rows = 2;
  table->border_size = 1;
}


/* Callback function called by the "Border" entry widget */
void
border_size_func                       (GtkEditable     *editable,
                                        gpointer         user_data)
{ 
  gchar *temp;
  temp = gtk_entry_get_text (GTK_ENTRY (table_entry3));
  table->border_size = atoi(temp);
}

/* Callback function called by the "Number of Columns" entry widget */
void
nb_columns_func                        (GtkEditable     *editable,
                                        gpointer         user_data)
{
  gchar *temp;
  temp = gtk_entry_get_text (GTK_ENTRY (table_entry1));
  table->nb_columns = atoi(temp);
}

/* Callback function called by the "Number of Rows" entry widget */
void
nb_rows_func                           (GtkEditable     *editable,
                                        gpointer         user_data)
{
  gchar *temp;
  temp = gtk_entry_get_text (GTK_ENTRY (table_entry2));
  table->nb_rows = atoi(temp);
}
/* Callback function called by the "Confirm" button widget */
void
table_confirm_func                     (GtkButton       *button,
                                        gpointer         user_data)
{
  printf("columns :%d  \n", table->nb_columns );
 printf("rows : %d  \n",table->nb_rows  );
 printf("border :%d  \n",table->border_size  );
}
/* Callback function called by the "Cancel" button  widget */
void
table_cancel_func                      (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy (GTK_WIDGET (user_data));
}


GtkWidget*
create_dialog_table ()
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *table1;
  GtkWidget *table_label4;
  GtkWidget *table_label5;
  GtkWidget *table_label6;
  GtkWidget *table_label1;
  GtkWidget *table_label2;
  GtkWidget *table_label3;
 
  GtkWidget *dialog_action_area1;
  GtkWidget *hbuttonbox1;
  GtkWidget *table_button1;
  GtkWidget *table_button2;


  Initialisation_table ();

  dialog1 = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog1), "dialog1");
  gtk_window_set_policy (GTK_WINDOW (dialog1), FALSE, TRUE, FALSE);
  gtk_signal_connect (GTK_OBJECT (dialog1), "destroy",
		      GTK_SIGNAL_FUNC (gtk_widget_destroy),GTK_OBJECT (dialog1));

  dialog_vbox1 = GTK_DIALOG (dialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  table1 = gtk_table_new (3, 3, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), table1, FALSE, FALSE, 5);

  table_label4 = gtk_label_new ("");
  gtk_widget_show (table_label4);
  gtk_table_attach (GTK_TABLE (table1), table_label4, 2, 3, 0, 1,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  table_label5 = gtk_label_new ("");
  gtk_widget_show (table_label5);
  gtk_table_attach (GTK_TABLE (table1), table_label5, 2, 3, 1, 2,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  table_label6 = gtk_label_new ("");
  gtk_widget_show (table_label6);
  gtk_table_attach (GTK_TABLE (table1), table_label6, 2, 3, 2, 3,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

  table_label1 = gtk_label_new ("Number of Columns\n1..50");
  gtk_widget_show (table_label1);
  gtk_table_attach (GTK_TABLE (table1), table_label1, 0, 1, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 5, 5);
  gtk_label_set_justify (GTK_LABEL (table_label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (table_label1), 0.1, 0.5);

  table_label2 = gtk_label_new ("Number of Rows\n1..200");
  gtk_widget_show (table_label2);
  gtk_table_attach (GTK_TABLE (table1), table_label2, 0, 1, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 5, 5);
  gtk_label_set_justify (GTK_LABEL (table_label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (table_label2), 0.1, 0.5);

  table_label3 = gtk_label_new ("Border\n1..50\n");
  gtk_widget_show (table_label3);
  gtk_table_attach (GTK_TABLE (table1), table_label3, 0, 1, 2, 3,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 5, 5);
  gtk_label_set_justify (GTK_LABEL (table_label3), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment (GTK_MISC (table_label3), 0.1, 0.5);

  table_entry3 = gtk_entry_new ();
  gtk_widget_show (table_entry3);
  gtk_table_attach (GTK_TABLE (table1), table_entry3, 1, 2, 2, 3,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 5, 5);
  gtk_widget_set_usize (table_entry3, 50, -1);
  gtk_signal_connect (GTK_OBJECT (table_entry3), "changed",
                      GTK_SIGNAL_FUNC (border_size_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (table_entry3), "1");

  table_entry1 = gtk_entry_new ();
  gtk_widget_show (table_entry1);
  gtk_table_attach (GTK_TABLE (table1), table_entry1, 1, 2, 0, 1,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 5, 5);
  gtk_widget_set_usize (table_entry1, 20, -1);
  gtk_signal_connect (GTK_OBJECT (table_entry1), "changed",
                      GTK_SIGNAL_FUNC (nb_columns_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (table_entry1), "2");

  table_entry2 = gtk_entry_new ();
  gtk_widget_show (table_entry2);
  gtk_table_attach (GTK_TABLE (table1), table_entry2, 1, 2, 1, 2,
                    (GtkAttachOptions) GTK_FILL, (GtkAttachOptions) GTK_FILL, 5, 5);
  gtk_widget_set_usize (table_entry2, 50, -1);
  gtk_signal_connect (GTK_OBJECT (table_entry2), "changed",
                      GTK_SIGNAL_FUNC (nb_rows_func),
                      NULL);
  gtk_entry_set_text (GTK_ENTRY (table_entry2), "2");

  dialog_action_area1 = GTK_DIALOG (dialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_container_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbuttonbox1, TRUE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_EDGE);

  table_button1 = gtk_button_new_with_label ("Confirm");
  gtk_widget_show (table_button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), table_button1);
  GTK_WIDGET_SET_FLAGS (table_button1, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (table_button1);
  gtk_signal_connect (GTK_OBJECT (table_button1), "clicked",
                      GTK_SIGNAL_FUNC (table_confirm_func),
                      NULL);

  table_button2 = gtk_button_new_with_label ("Cancel");
  gtk_widget_show (table_button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), table_button2);
  GTK_WIDGET_SET_FLAGS (table_button2, GTK_CAN_DEFAULT);
  gtk_signal_connect (GTK_OBJECT (table_button2), "clicked",
                      GTK_SIGNAL_FUNC (table_cancel_func),
                      dialog1);

  return dialog1;
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void GeneralPreferences (Document doc, View view)
{
  GtkWidget *general_dialog;

  general_dialog = create_dialog_preferences ();
  gtk_widget_show (general_dialog);
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void ChangeDocumentStyle (Document doc, View view)
{
  GtkWidget *style_dialog;

  style_dialog = create_dialog_style();
  gtk_widget_show (style_dialog);
}

#endif /* _GTK */
