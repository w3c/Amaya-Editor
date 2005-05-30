/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef THOT_DIALOGAPI_H
#define THOT_DIALOGAPI_H

/* Catalogues structures */
#define CAT_INT        0
#define CAT_TEXT       1
#define CAT_FORM       2
#define CAT_MENU       3
#define CAT_POPUP      4
#define CAT_FMENU      5
#define CAT_SHEET      6
#define CAT_SELECT     7
#define CAT_TMENU      8
#define CAT_LABEL      9
#define CAT_DIALOG    10
#define CAT_PULL      11
#define CAT_ICON      12
#define CAT_SCRPOPUP  13
#define CAT_TREE      14
#define CAT_COMBOBOX  15

#define MAX_CAT       20
#define C_NUMBER      15
#define INTEGER_DATA   1
#define STRING_DATA    2
#define MAX_TXTMSG   120
#define MAX_ARGS      20

struct E_List
  {
     struct     E_List* E_Next;         /* CS_List d'entrees suivante         */
     char     E_Free[C_NUMBER];       /* Disponibilite des entrees         */
     char     E_Type[C_NUMBER];       /* CsList des types des entrees      */
     ThotWidget E_ThotWidget[C_NUMBER]; /* ThotWidgets associes aux entrees  */
  };

struct Cat_Context
  {
    int                 Cat_Ref;	/* CsReference appli du catalogue    */
    unsigned char       Cat_Type;	/* Type du catalogue                 */
    unsigned char       Cat_Button;	/* Le bouton qui active              */
    unsigned char       Cat_Default;    /* Defaulft return                   */
    union
    {
      int                Catu_Data;        /* Valeur de retour                  */
      ThotWidget	 Catu_XtWParent;
    } Cat_Union1;
    union
    {
      int                Catu_in_lines;	/* Orientation des formulaires       */
      ThotWidget	 Catu_SelectLabel;
    } Cat_Union2;
    ThotWidget          Cat_Widget;	/* Le widget associe au catalogue    */
    ThotWidget          Cat_Title;	/* Le widget du titre                */
    ThotWidget          Cat_ParentWidget; /* pointer to the widget parent */
    struct Cat_Context *Cat_PtParent;	/* Adresse du catalogue pere         */
    int                 Cat_EntryParent; /* Entree du menu parent            */
    ThotBool            Cat_React;	/* Indicateur reaction immediate     */
    ThotBool            Cat_SelectList; /* Indicateur selecteur = liste      */
    struct E_List      *Cat_Entries;	/* CsList des entrees d'un menu      */
                                        /* ou widget de saisie de texte      */
  };

/* Redefiniton de champs de catalogues dans certains cas */
#define Cat_ListLength  Cat_Union1.Catu_Data
#define Cat_FormPack    Cat_Union1.Catu_Data
#define Cat_Data	Cat_Union1.Catu_Data
#define Cat_XtWParent   Cat_Union1.Catu_XtWParent
#define Cat_in_lines	Cat_Union2.Catu_in_lines
#define Cat_SelectLabel Cat_Union2.Catu_SelectLabel
#define Cat_Focus       Cat_React

struct Cat_List
  {
     struct Cat_List    *Cat_Next;
     struct Cat_Context  Cat_Table[MAX_CAT];
  };

#ifdef _WINGUI

#define MAX_FRAMECAT 50
typedef struct FrCatalogue {
  struct Cat_Context * Cat_Table[MAX_FRAMECAT];
} FrCatalogue;

typedef struct WIN_Form
{
  HWND Buttons [10]; /* Dialog Button      */
  int  x  [10];      /* Initial x position */
  int  cx [10];      /* Button width       */
} WIN_Form;

extern LRESULT CALLBACK WIN_ScrPopupProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern struct Cat_Context *CatEntry (int ref);

#endif /* _WINGUI */

#endif /* #define THOT_DIALOGAPI_H */
