#ifndef _DIALOG_BOX_H_
#define _DIALOG_BOX_H_

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>
#include <stdio.h>
#include <X11/Xlib.h>

/*#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "modif.h"
#include "appdialogue.h"
#include "dialog.h"
#include "logowindow.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "LiteClue.h"*/
#define THOT_EXPORT extern
#include "amaya.h"


#define MAX_ARGS	20
#define MAX_USER_ACTION	100

/* Stucture containing print parameters for the "Set up and print" dialog box */
typedef struct _Print_form 
{
  ThotBool Printer;  /* Printer not PostScript file */
  ThotBool A4_not_US;
  ThotBool Manual_feed;
  ThotBool Table_of_contents;
  ThotBool Table_of_links;
  ThotBool Print_URL;
  STRING  output;
}Print_form;


/* Stucture containing characters parameters of the "style" dialog box */
typedef struct _Char_style 
{
  STRING font_family;
  STRING character;
  STRING underline;
  int body_size;
}Char_style;
/* Stuctures containing color parameters  of the "style" dialog box */
typedef struct _Color_previous
{
  GtkWidget *label_f;
  GtkWidget *label_b; 
  int indice_f;   /* index of the foreground color */
  int indice_b; /* index of the background color */
}Color_previous;

typedef struct _Color_form
{ 
  int indice;
  GtkWidget *label;
}Color_form;
/* Stucture containing format parameters  of the "style" dialog box */
typedef struct _Format_style 
{
  int align; /* align =1 if "left", =2 if "right", =3 if "center" =4 if "default" */
  int justification; /* =1 if "yes", =2 if "no", =3 if "default"*/
  int line_spcg; /* =1 if "small", =2 if "medium", =3 if "large" =4 if "default" **/
  int indent; /* =1 if "indent1", =2 if "indent2", =3 if "default" */
  int line_spcg_pt;
  int indent_pt;
}Format_style;
/* Definition of the structure containing general preferences parameters */
typedef struct _General
{
    STRING home_page;
    ThotBool multi_key;
    ThotBool show_bckgd_image;
    ThotBool dbl_click_activate;
    int dbl_click_delay;
    int zoom;
    int menu_font_size;
    STRING dlg_lang;
 
}General;

/* Definition of the structure containing publishing preferences parameters */
typedef struct _Publish
{
  ThotBool use_etags;
  ThotBool verify_puts;
  STRING default_url;
}Publish;

/* Definition of the structure containing cache preferences parameters */
typedef struct _Cache
{
  STRING directory;
  ThotBool enable;
  ThotBool protected_document;
  ThotBool disconnected;
  ThotBool ignore_expires;
  int size;
  int limit_entry;
}Cache;

/* Definition of the structure containing proxy preferences parameters */
typedef struct _Proxy
{
  STRING http_proxy;
  STRING no_proxy_domains;
}Proxy;

/* Definition of the structure containing color preferences parameters */
typedef struct _Color
{
  STRING doc_forgrd;
  STRING doc_backgrd;
  STRING menu_forgrd;
  STRING menu_backgrd;
}Color;
/* Stucture containing search  parameters */
typedef struct _Search_form 
{
  int  replace;  /* replace = 1 if No replace, =2 if replace on request */
                 /* = 3 if Automatic replace */
  int selection; /* selection = 1 if before selection, = 2 if Within selection*/ 
		    /* =3 if After selection, = 4if in the whole document */
  ThotBool uppercase; 
  STRING  search_for;
  STRING replace_by;
}Search_form;
/* Stucture containing spell checking  parameters */
typedef struct _Spell_form 
{
  int checking; /* checking = 1 if before selction, = 2 if Within selection*/ 
		    /* =3 if After selection, = 4 if in the whole document */
  ThotBool caps; 
  ThotBool digits;
  ThotBool numerals;
  ThotBool containing;
  int  nb_proposals;
  STRING replaced_word;
  STRING ignored_word;
  STRING proposed_word;
  
}Spell_form;
/* Definition of the structure containing table parameters */
typedef struct _Table_form
{
  int nb_columns;
  int nb_rows;
  int border_size;
}Table_form;

/*-----------------------------------------------------------------------
global variables
-----------------------------------------------------------------------*/
/* variables containing print  parameters */
Print_form *print_form; 
/* variables containing style parameters */
Char_style *char_style;
Format_style *format_style;
Color_form *color_form[152];
/* variables containing preferences  parameters */
General *general;
Publish *publish;
Cache *cache;
Proxy *proxy;
Color *color;
STRING *list_language;
/* variables containing search  parameters */
Search_form *search_form;
/* variables containing spell  parameters */
Spell_form *spell_form;
/* variables containing table  parameters */
Table_form *table;

/*------------------------------------------------------------------------
external functions
--------------------------------------------------------------------------*/

/*Function that creates a "New HTML document" or "New CSS.." dialog box  */
void CreateOpenDocDlgGTK (gchar *title);







#endif /*_DIALOG_BOX_H_*/

