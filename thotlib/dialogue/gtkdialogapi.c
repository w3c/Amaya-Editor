/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * Dialogue API routines
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */
#include <gtk/gtk.h>


#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "copyright.xbm"
//#include "dialog.h"
#include "gtkdialog.h"

#include "application.h"
#include "interface.h"
#include "appdialogue.h"
#include "appli_f.h"
#include "message.h"

#ifdef _WINDOWS
#include "winsys.h"
#include "wininclude.h"
#endif /* _WINDOWS */

#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "thotcolor_tv.h"
#include "frame_tv.h"
#include "font_tv.h"

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

#define MAX_CAT       20
#define C_NUMBER      15
#define INTEGER_DATA   1
#define STRING_DATA    2
#define MAX_TXTMSG   120
#define MAX_ARGS      20

struct E_List
  {
     struct     E_List* E_Next;         /* CsList d'entrees suivante         */
     ThotWidget E_ThotWidget[C_NUMBER]; /* ThotWidgets associes aux entrees  */
  };

struct Cat_Context
  {
    int                 Cat_Ref;	         /* CsReference appli du catalogue    */
    UCHAR_T             Cat_Type;	         /* Type du catalogue                 */
   
    ThotWidget          Cat_Widget;	         /* Le widget associe au catalogue    */
    ThotBool            Cat_React;	         /* Indicateur reaction immediate     */
    struct Cat_Context *Cat_PtParent;
    ThotBool            Cat_SelectList;	 /* Indicateur selecteur = liste      */
    struct E_List      *Cat_Entries;	         /* CsList des entrees d'un menu      */
                                                 /* ou widget de saisie de texte      */
  };



struct Cat_List
  {
     struct Cat_List    *Cat_Next;
     struct Cat_Context  Cat_Table[MAX_CAT];
  };

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "appdialogue_tv.h"

#ifdef _GTK
extern int          appArgc; 
extern STRING       *appArgv; 
#endif /*_GTK */  

/* Declarations des options de dialogue */
ThotBool            WithMessages = TRUE;


static int          FirstFreeRef;	/* First free reference */
/* Declarations des variables globales */
static struct Cat_List*    PtrCatalogue;	/* Le pointeur su les catalogues  */
static int                 NbOccCat;
static int                 NbLibCat;
static struct E_List*      PtrFreeE_List;
static int                 NbOccE_List;
static int                 NbLibE_List;

static int                 CurrentWait;
static int                 ShowReturn;
static int                 ShowX, ShowY;
static struct Cat_Context* ShowCat = NULL;
static ThotTranslations    TextTranslations;
static ThotWidget          MainShell, PopShell;



static XmFontList   formFONT;
static ThotAppContext Def_AppCont;
static Display*       GDp;


#include "gtkappdialogue_f.h"
#include "memory_f.h"
#include "thotmsg_f.h"



/*----------------------------------------------------------------------
   GetFrameNumber :  returns the Thot window number associated to an         
   X-Window window.                                            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int GetFrameNumber (GtkWindow *win)
#else  /* !__STDC__ */
int GetFrameNumber (win)
GtkWindow win;
#endif /* __STDC__ */
{
#ifndef _GTK
   int frame;

   for (frame = 0; frame <= MAX_FRAME; frame++)
       if (FrRef[frame] == win)
	  return (frame);

   fprintf (stderr, "Could not get X-Window number for %X\n", (unsigned int)win);
   return (-1);
#else /* _GTK */
return 0;
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   Procedure de retour par defaut.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CallbackError (int ref, int typedata, STRING data)

#else  /* __STDC__ */
static void         CallbackError (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif /* __STDC__ */
{
   printf ("Toolkit error : No callback procedure ...\n");
}

static void         (*CallbackDialogue) () = CallbackError;

/*----------------------------------------------------------------------
   NewCatList Creates a new catalogue list.                           
  ----------------------------------------------------------------------*/
static struct Cat_List *NewCatList ()
{
   register int        i;
   struct Cat_List    *adlist;

   adlist = (struct Cat_List *) TtaGetMemory (sizeof (struct Cat_List));

   adlist->Cat_Next = NULL;

   /* Updates thenumber of available and free catalogues */
   NbLibCat += MAX_CAT;
   for (i = 0; i < MAX_CAT; i++)
     {
       adlist->Cat_Table[i].Cat_Widget = 0;
       adlist->Cat_Table[i].Cat_Entries = NULL;
     }
   return (adlist);
}


/*----------------------------------------------------------------------
   NewEList: creates a new block of elements.                         
  ----------------------------------------------------------------------*/
static struct E_List *NewEList ()
{
   register int        i;
   struct E_List      *adbloc;

   /* verifies if there is a free block */
   if (PtrFreeE_List == NULL)
      adbloc = (struct E_List *) TtaGetMemory (sizeof (struct E_List));
   else
     {
	adbloc = PtrFreeE_List;
	PtrFreeE_List = adbloc->E_Next;
	NbLibE_List--;
     }

   /* Updates the number of blocks of allocated E_List */
   NbOccE_List++;
   adbloc->E_Next = NULL;
   for (i = 0; i < C_NUMBER; i++)
     adbloc->E_ThotWidget[i] = 0;
 
   return (adbloc);
}


/*----------------------------------------------------------------------
   FreeEList: Releases all blocks of elements.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreeEList (struct E_List *adbloc)
#else  /* __STDC__ */
static void         FreeEList (adbloc)
struct E_List      *adbloc;
#endif /* __STDC__ */
{
   struct E_List      *cebloc;

   cebloc = adbloc;
   while (cebloc != NULL)
     {
	NbLibE_List++;
	if (cebloc->E_Next == NULL)
	  {
	     cebloc->E_Next = PtrFreeE_List;
	     PtrFreeE_List = adbloc;
	     cebloc = NULL;
	  }
	else
	   cebloc = cebloc->E_Next;
     }
}


/*----------------------------------------------------------------------
   Callback for closing a menu                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         UnmapMenu (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
#else  /* __STDC__ */
static void         UnmapMenu (w, catalogue, call_d)
ThotWidget          w;
struct Cat_Context *catalogue;
caddr_t             call_d;
#endif /* __STDC__ */
{
#ifndef _GTK
   struct Cat_Context *icatal;

   /* Une attende est peut etre debloquee */
   icatal = catalogue;
   while (icatal->Cat_PtParent != NULL)
      icatal = icatal->Cat_PtParent;

   if (icatal == ShowCat && ShowReturn == 1)
      ShowReturn = 0;
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   Callback for a menu button                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CallMenu (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
#else  /* __STDC__ */
static void         CallMenu (w, catalogue, call_d)
ThotWidget          w;
struct Cat_Context *catalogue;
caddr_t             call_d;
#endif /* __STDC__ */
{
   register int        i;
   register int        index;
   register int        entry;
   struct E_List      *adbloc;
   struct Cat_Context *icatal;

   /* Une attende est peut etre debloquee */
   icatal = catalogue;
#ifndef _GTK
   while (icatal->Cat_PtParent != NULL)
      icatal = icatal->Cat_PtParent;
#endif /*_GTK */
   if (icatal == ShowCat && ShowReturn == 1)
      ShowReturn = 0;

   /* A menu entry is selected */
   if (catalogue->Cat_Widget != 0)
      if ((int) catalogue->Cat_Widget == -1)
	/*** back to a simple button ***/
	(*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, 0);
      else
	{
	   adbloc = catalogue->Cat_Entries;
	   entry = -1;
	   index = 0;
	   i = 2;		/* decalage de 2 pour le widget titre */
	   while ((entry == -1) && (adbloc != NULL))
	     {
		while ((entry == -1) && (i < C_NUMBER))
		  {

		     if (adbloc->E_ThotWidget[i] == w)
                entry = index;
		     i++;
		     index++;
		  }
		/* Passe au bloc suivant */
		adbloc = adbloc->E_Next;
		i = 0;
	     }

	   /*** Retour de l'entree du menu choisie vers l'application ***/
	   (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, entry);
	}
}


/*----------------------------------------------------------------------
   Callback pour un bouton du sous-menu de formulaire                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CallRadio (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
#else  /* __STDC__ */
static void         CallRadio (w, catalogue, call_d)
ThotWidget          w;
struct Cat_Context *catalogue;
caddr_t             call_d;
#endif /* __STDC__ */
{
   register int        i;
   register int        index;
   register int        entry;
   struct E_List      *adbloc;

   /* Enregistre la selection d'un toggle button */
   if (catalogue->Cat_Widget != 0)
     {
	adbloc = catalogue->Cat_Entries;
	entry = -1;
	index = 0;
	i = 2;			/* decalage de 2 pour le widget titre */
	while ((entry == -1) && (adbloc != NULL))
	  {
	     while ((entry == -1) && (i < C_NUMBER))
	       {
		  if (adbloc->E_ThotWidget[i] == w)
		     entry = index;
		  i++;
		  index++;
	       }
	     /* Passe au bloc suivant */
	     adbloc = adbloc->E_Next;
	     i = 0;
	  }

	/*** Sauve la valeur de la derniere selection ***/
#ifndef _GTK
	catalogue->Cat_Data = entry;
#endif /* _GTK */
	/* retourne la valeur si le menu est reactif */
	if (catalogue->Cat_React)
	   (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, entry);
     }
}


/*----------------------------------------------------------------------
   Callback pour un bouton du toggle-menu                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CallToggle (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
#else  /* __STDC__ */
static void         CallToggle (w, catalogue, call_d)
ThotWidget          w;
struct Cat_Context *catalogue;
caddr_t             call_d;
#endif /* __STDC__ */
{
   register int        i;
   int                 entry;
   int                 ent;
   int                 max;
   struct E_List      *adbloc;

   /* Enregistre la selection d'un toggle button */
   if (catalogue->Cat_Widget != 0)
     {
	adbloc = catalogue->Cat_Entries;
	entry = -1;
	ent = 0;
#ifndef _GTK
	max = catalogue->Cat_Data;	/* nombre d'entrees definies */
#endif /* _GTK */
	i = 2;			/* decalage de 2 pour le widget titre */
	while (entry == -1 && adbloc != NULL && ent <= max)
	  {
	     while (entry == -1 && i < C_NUMBER && ent <= max)
	       {
		  if (adbloc->E_ThotWidget[i] == w)
		    {
		       entry = 0;
		       /* Bascule la valeur du bouton correspondant a l'entree */
#ifndef _GTK
		       adbloc->E_Free[i] = TEXT('Y');
#endif /* _GTK */
		       /* signale que l'entree est basculee si le menu est reactif */
		       if (catalogue->Cat_React)
			  (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, ent);
		    }
		  i++;
		  ent++;
	       }
	     /* Passe au bloc suivant */
	     adbloc = adbloc->E_Next;
	     i = 0;
	  }			/*while */
     }				/*if */
}				/*CallToggle */


/*----------------------------------------------------------------------
   ReturnTogglevalues retourne les entre'es bascule'es du             
   toggle-menu catalogue.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReturnTogglevalues (struct Cat_Context *catalogue)
#else  /* __STDC__ */
static void         ReturnTogglevalues (catalogue)
struct Cat_Context *catalogue;
#endif /* __STDC__ */
{
#ifndef _GTK
   register int        i;
   register int        index;
   int                 ent;
   int                 max;
   struct E_List      *adbloc;

   /* Enregistre la selection d'un toggle button */
   if (catalogue->Cat_Widget != 0)
     {
	adbloc = catalogue->Cat_Entries;
	index = 0;
	ent = 0;
#ifndef _GTK
	max = catalogue->Cat_Data;	/* nombre d'entrees definies */
#endif /* _GTK */
	i = 2;			/* decalage de 2 pour le widget titre */
	while (adbloc != NULL)
	  {
	     while (i < C_NUMBER && ent < max)
	       {
		 /*** Retour les entrees selectionnees vers l'application ***/

		  if (adbloc->E_Free[i] == TEXT('Y'))
		    {
		       (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, index);
		       adbloc->E_Free[i] = TEXT('N');
		    }
		  i++;
		  ent++;
		  index++;
	       }

	     /* Passe au bloc suivant */
	     adbloc = adbloc->E_Next;
	     i = 0;
	  }
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   Callback d'initialisation d'un formulaire.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         INITform (ThotWidget w, struct Cat_Context *parentCatalogue, caddr_t call_d)

#else  /* __STDC__ */
static void         INITform (w, parentCatalogue, call_d)
ThotWidget          w;
struct Cat_Context *parentCatalogue;
caddr_t             call_d;

#endif /* __STDC__ */
{
#ifndef _GTK   
  register int        n;
   int                 ent;
   struct E_List      *adbloc;
#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
#  endif /* _WINDOWS */
   struct Cat_Context *catalogue;

   /* Affiche le formulaire */
   if (parentCatalogue->Cat_Widget != 0)
     {
        /*** Allume les sous-widgets du formulaire ***/
	adbloc = parentCatalogue->Cat_Entries;
	/* Le premier bloc contient les boutons de la feuille de saisie */
	adbloc = adbloc->E_Next;

	ent = 1;
	while (adbloc->E_ThotWidget[ent] != 0)
	  {
	     /* Il faut sauter les widgets des RowColumns */
	     if (adbloc->E_Free[ent] == TEXT('N'))
	       {
		  catalogue = (struct Cat_Context *) adbloc->E_ThotWidget[ent];
#                 ifndef _WINDOWS
		  if (catalogue->Cat_Widget != 0)
		     XtManageChild (catalogue->Cat_Widget);
#                 else  /* _WINDOWS */
                  ShowWindow (catalogue->Cat_Widget, SW_SHOWNORMAL);
		  UpdateWindow (catalogue->Cat_Widget);
#                 endif /* _WINDOWS */
	       }

	     /* Faut-il passer au bloc suivant ? */
	     ent++;
	     if (ent >= C_NUMBER)
	       {
		  ent = 0;
		  if (adbloc->E_Next == NULL)
		     break;
		  else
		     adbloc = adbloc->E_Next;
	       }
	  }			/*while */

	/*** Positionne le formulaire a la position courante du show ***/
	n = 0;
	w = parentCatalogue->Cat_Widget;
	n = 0;
#       ifndef _WINDOWS
	XtSetArg (args[n], XmNx, (Position) ShowX);
	n++;
	XtSetArg (args[n], XmNy, (Position) ShowY);
	n++;
	XtSetValues (XtParent (w), args, n);

	XtManageChild (w);
	XtManageChild (XtParent (w));
	if (PopShell != 0)
	  {
	    XtSetValues (PopShell, args, n);
	    XtPopup (PopShell, XtGrabNonexclusive);
	  }
#       else  /* _WINDOWS */
        ShowWindow (w, SW_SHOWNORMAL);
        UpdateWindow (w);
        ShowWindow (GetParent (w), SW_SHOWNORMAL);
        UpdateWindow (GetParent (w));
#       endif /* _WINDOWS */
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   Callback d'initialisation d'un formulaire avec positionnement.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         INITetPOSform (ThotWidget w, struct Cat_Context *parentCatalogue, caddr_t call_d)

#else  /* __STDC__ */
static void         INITetPOSform (w, parentCatalogue, call_d)
ThotWidget          w;
struct Cat_Context *parentCatalogue;
caddr_t             call_d;

#endif /* __STDC__ */
{
#ifndef _GTK
   TtaSetDialoguePosition ();
   INITform (w, parentCatalogue, call_d);
#endif /*_GTK */
}


/*----------------------------------------------------------------------
   Destruction de feuillet.                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         formKill (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)

#else  /* __STDC__ */
static void         formKill (w, parentCatalogue, call_d)
ThotWidget          w;
struct Cat_Context *parentCatalogue;
caddr_t             call_d;

#endif /* __STDC__ */
{
   /* Le widget est detruit */
  if ((catalogue->Cat_Type == CAT_FORM)
      || (catalogue->Cat_Type == CAT_SHEET)
      || (catalogue->Cat_Type == CAT_DIALOG))
    TtaDestroyDialogue (catalogue->Cat_Ref);
}
#ifndef _GTK
#ifndef _WINDOWS
/*----------------------------------------------------------------------
   Callback de saisie de valeur.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CallValueSet (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
#else  /* __STDC__ */
static void         CallValueSet (w, catalogue, call_d)
ThotWidget          w;
struct Cat_Context *catalogue;
caddr_t             call_d;

#endif /* __STDC__ */
{

  int                 val, val1;
  CHAR_T              text[11];
  ThotWidget          wtext;

  /* Indication de valeur */
  if (catalogue->Cat_Widget != 0)
    if (catalogue->Cat_Type == CAT_INT)
      {
	catalogue->Cat_Data = 0;
	wtext = catalogue->Cat_Entries->E_ThotWidget[1];
	
	ustrncpy (text, XmTextGetString (wtext), 10);
	text[10] = EOS;
	if (text[0] != EOS)
	  {
	    if ((text[0] == '-') && (text[1] == EOS))
	      /* cas ou le caractere - a ete tape, on met val a 0 */
	      val = 0;
	    else
	      sscanf (text, "%d", &val);
	    
	    /* Est-ce une valeur valide ? */
	    if (val < (int) catalogue->Cat_Entries->E_ThotWidget[2])
	      val1 = (int) catalogue->Cat_Entries->E_ThotWidget[2];
	    else if (val > (int) catalogue->Cat_Entries->E_ThotWidget[3])
	      val1 = (int) catalogue->Cat_Entries->E_ThotWidget[3];
	    else
	      val1 = val;	/* valeur inchangee */
	    
	    /* Est-ce qu'il faut changer le contenu du widget ? */
	    if (val != val1)
	      {
		usprintf (text, "%d", val1);
		/* Desactive la procedure de Callback */
		if (catalogue->Cat_React)
		  XtRemoveCallback (wtext, XmNvalueChangedCallback, (XtCallbackProc) CallValueSet, catalogue);
		
		XmTextSetString (wtext, text);
		val = ustrlen (text);
		XmTextSetSelection (wtext, val, val, 500);
		
		/* Reactive la procedure de Callback */
		if (catalogue->Cat_React)
		  XtAddCallback (wtext, XmNvalueChangedCallback, (XtCallbackProc) CallValueSet, catalogue);
	      }

	    /* retourne la valeur saisie si la feuille de saisie est reactive */
	    if (catalogue->Cat_React)
	      (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, val);
	  }
      }

}
#endif /* !WINDOWS */
#endif /* _GTK */
/*----------------------------------------------------------------------
   Callback de feuillet.                                              
  ----------------------------------------------------------------------*/
#ifndef _WINDOWS
#ifdef __STDC__
static void         CallSheet (ThotWidget w, struct Cat_Context *parentCatalogue, caddr_t call_d)

#else  /* __STDC__ */
static void         CallSheet (w, parentCatalogue, call_d)
ThotWidget          w;
struct Cat_Context *parentCatalogue;
caddr_t             call_d;

#endif /* __STDC__ */
{
#ifndef _GTK
  register int        n;
  int                 i;
  int                 ent;
  int                 entry;
  CHAR_T              text[100];
  STRING              ptr;
  Arg                 args[MAX_ARGS];
  XmStringTable       strings;
  struct E_List      *adbloc;
  struct Cat_Context *catalogue;
  ThotWidget          wtext;

  /* On a selectionne une entree du menu */
  if (parentCatalogue->Cat_Widget != 0)
    {
      adbloc = parentCatalogue->Cat_Entries;
      entry = -1;
      i = 0;
      while ((entry == -1) && (i < C_NUMBER))
	{
	  if (adbloc->E_ThotWidget[i] == w)
	    entry = i;
	  i++;
	}

      /* Si la feuille de dialogue est detruite cela force l'abandon */
      if (entry == -1)
	if (parentCatalogue->Cat_Type == CAT_SHEET)
	  entry = 0;
	else
	  return;

      /*** Retour vers l'application ***/
      /*** Eteins les sous-widgets du feuillet si on quitte ***/
      /*** Recupere les retours des sous-catalogues         ***/
      adbloc = adbloc->E_Next;
      ent = 1;
      while (adbloc->E_ThotWidget[ent] != 0)
	{
	  /* Il faut sauter les widgets des RowColumns */
	  if (adbloc->E_Free[ent] == 'N')
	    {
	      catalogue = (struct Cat_Context *) adbloc->E_ThotWidget[ent];
	      if (catalogue->Cat_Widget != 0)
		{
		  if (entry == 0)
		    XtUnmanageChild (catalogue->Cat_Widget);
		  
		  /* Sinon il faut retourner la valeur du sous-catalogue */
		  else
		    {
		      if (catalogue->Cat_React)
			;	/* La valeur est deja transmise */
		      /*________________________________________________ Un sous-menu __*/
		      else if (catalogue->Cat_Type == CAT_FMENU)
			{
			  i = catalogue->Cat_Data;
			  (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, i);
			}
		      /*______________________________________________ Un toggle-menu __*/
		      else if (catalogue->Cat_Type == CAT_TMENU)
			ReturnTogglevalues (catalogue);
		      /*______________________________ Une feuille de saisie d'entier __*/
		      else if (catalogue->Cat_Type == CAT_INT)
			{
			  CallValueSet (catalogue->Cat_Entries->E_ThotWidget[1], catalogue, NULL);
			  ustrncpy (text, XmTextGetString (catalogue->Cat_Entries->E_ThotWidget[1]), 10);
			  
			  text[10] = EOS;
			  if (text[0] != EOS)
			    sscanf (text, "%d", &i);
			  else
			    i = 0;
			  (*CallbackDialogue) (catalogue->Cat_Ref, INTEGER_DATA, i);
			}
		      /*______________________________ Une feuille de saisie de texte __*/
		      else if (catalogue->Cat_Type == CAT_TEXT)
			{
			  (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA,
					       XmTextGetString ((ThotWidget) catalogue->Cat_Entries));
			}
		      /*_______________________________________________ Un selecteur __*/
		      else if (catalogue->Cat_Type == CAT_SELECT)
			{
			  if (catalogue->Cat_SelectList)
			    {
			      text[0] = EOS;
			      n = 0;
			      XtSetArg (args[n], XmNselectedItems, &strings);
			      n++;
			      XtGetValues ((ThotWidget) catalogue->Cat_Entries, args, n);
			      ptr = text;
			      if (strings != NULL)
				XmStringGetLtoR (strings[0], XmSTRING_DEFAULT_CHARSET, &ptr);
			      (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, ptr);
			    }
			  else
			    {
			      wtext = XmSelectionBoxGetChild ((ThotWidget) catalogue->Cat_Entries, XmDIALOG_TEXT);
			      /* Retourne la valeur dans tous les cas */
			      (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, XmTextGetString (wtext));
			    }
			}
		    }
		}
	    }

	  /* Faut-il passer au bloc suivant ? */
	  ent++;
	  if (ent >= C_NUMBER)
	    {
	      ent = 0;
	      if (adbloc->E_Next == NULL)
		break;
	      else
		adbloc = adbloc->E_Next;
	    }
	}

      /*** On fait disparaitre le formulaire ***/
      if (entry == 0 || parentCatalogue->Cat_Type == CAT_DIALOG || parentCatalogue->Cat_Type == CAT_FORM)
	{
	  XtUnmanageChild (parentCatalogue->Cat_Widget);
	  XtUnmanageChild (XtParent (parentCatalogue->Cat_Widget));
	  
	  /* Si on en a fini avec la feuille de dialogue */
	  catalogue = parentCatalogue;
	  while (catalogue->Cat_PtParent != NULL)
	    catalogue = catalogue->Cat_PtParent;
	  
	  if (catalogue == ShowCat && ShowReturn == 1)
	    ShowReturn = 0;
	}

      (*CallbackDialogue) (parentCatalogue->Cat_Ref, INTEGER_DATA, entry);
    }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   Callback de selection dans une liste.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CallList (ThotWidget w, struct Cat_Context *catalogue, XmListCallbackStruct * infos)
#else  /* __STDC__ */
static void         CallList (w, catalogue, infos)
ThotWidget          w;
struct Cat_Context *catalogue;
XmListCallbackStruct *infos;

#endif /* __STDC__ */

{
#ifndef _GTK
   STRING              text;
   ThotBool            ok;

   if (catalogue->Cat_Widget != 0)
      if (catalogue->Cat_Type == CAT_SELECT)
	{
	   ok = XmStringGetLtoR (infos->item, XmSTRING_DEFAULT_CHARSET, &text);
	   /* retourne l'entree choisie */
	   if (ok && text != NULL)
	      if (text[0] != EOS && text[0] != SPACE)
		 (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, text);
	}
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   Callback de saisie de texte.                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CallTextChange (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)

#else  /* __STDC__ */
static void         CallTextChange (w, catalogue, call_d)
ThotWidget          w;
struct Cat_Context *catalogue;
caddr_t             call_d;

#endif /* __STDC__ */

{
#ifndef _GTK   
  ThotWidget          wtext;

   if (catalogue->Cat_Widget != 0)
      if (catalogue->Cat_Type == CAT_TEXT)
	 /* retourne la valeur saisie si la feuille de saisie est reactive */
	 (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA,
		     XmTextGetString ((ThotWidget) catalogue->Cat_Entries));
      else if (catalogue->Cat_Type == CAT_SELECT)
	{
	   wtext = XmSelectionBoxGetChild ((ThotWidget) catalogue->Cat_Entries, XmDIALOG_TEXT);
	   /* retourne la valeur saisie si la feuille de saisie est reactive */
	   (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, XmTextGetString (wtext));
	}
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   Callback pour un bouton du label de selecteur                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CallLabel (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)

#else  /* __STDC__ */
static void         CallLabel (w, catalogue, call_d)
ThotWidget          w;
struct Cat_Context *catalogue;
caddr_t             call_d;

#endif /* __STDC__ */
{
#ifndef _GTK
   Arg                 args[MAX_ARGS];
   XmString            text;
   STRING              str;

   if (catalogue->Cat_Widget != 0)
     {
	/* Recupere le texte du label */
	XtSetArg (args[0], XmNlabelString, &text);
	XtGetValues (w, args, 1);
	/* Initialise le champ texte */
	XtSetArg (args[0], XmNtextString, text);

	if (catalogue->Cat_SelectList)
	  {
	     /* retourne la valeur du label */
	     XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &str);
	     (*CallbackDialogue) (catalogue->Cat_Ref, STRING_DATA, str);
	  }
	else
	   XtSetValues ((ThotWidget) catalogue->Cat_Entries, args, 1);
     }
#endif /* _GTK */
}
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   warning handler                                                    
  ----------------------------------------------------------------------*/
void                MyWarningHandler ()
{
}
#ifndef _GTK
#ifndef _WINDOWS
/*----------------------------------------------------------------------
  Procedure which controls Motif dialogue colors
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void             ThotXmColorProc (ThotColorStruct *bg, ThotColorStruct *fg, ThotColorStruct *sel, ThotColorStruct *top, ThotColorStruct *bottom)
#else  /* !__STDC__ */
void             ThotXmColorProc (bg, fg, sel, top, bottom)
ThotColorStruct *bg;
ThotColorStruct *fg;
ThotColorStruct *sel;
ThotColorStruct *top;
ThotColorStruct *bottom;
#endif /* __STDC__ */
{
   top->red = RGB_Table[3].red *256;
   top->green = RGB_Table[3].green *256;
   top->blue = RGB_Table[3].blue *256;
   bottom->red = RGB_Table[6].red *256;
   bottom->green = RGB_Table[6].green *256;
   bottom->blue = RGB_Table[6].blue *256;
   fg->red = RGB_Table[7].red *256;
   fg->green = RGB_Table[7].green *256;
   fg->blue = RGB_Table[7].blue *256;
   sel->red = RGB_Table[5].red *256;
   sel->green = RGB_Table[5].green *256;
   sel->blue = RGB_Table[5].blue *256;
}
#endif /* _WINDOWS */
#endif /* _GTK */
/*----------------------------------------------------------------------
   TtaInitDialogue

   Initialise la connexion au serveur X  et le contexte du dialogue.

   Parameters:
   server: nom du serveur X.
   X-Specific stuff :
   app_context: contient au retour l'identification du contexte d'application.
   display:  contient au retour l'identification de l'e'cran.
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
#ifdef __STDC__
BOOL             WIN_TtaInitDialogue (STRING server)
#else  /* !__STDC__ */
BOOL             WIN_TtaInitDialogue (server)
STRING server; 
#endif /* __STDC__ */
#else  /* _WINDOWS */
#ifdef __STDC__
void             TtaInitDialogue (STRING server, ThotAppContext * app_context, Display ** Dp)

#else  /* __STDC__ */
void             TtaInitDialogue (server, app_context, Dp)
STRING              server;
ThotAppContext     *app_context;
Display           **Dp;

#endif /* __STDC__ */
#endif /* _WINDOWS */
{
#  ifndef _WINDOWS
   int                 n;
#  endif /* !_WINDOWS */

#ifdef _GTK
   gtk_init(&appArgc,&appArgv);
#endif /* _GTK */

#  ifndef _WINDOWS
   /* Ouverture de l'application pour le serveur X-ThotWindow */
   RootShell = 0;
   XtToolkitInitialize ();
   n = 0;
   Def_AppCont = XtCreateApplicationContext ();
   GDp = XtOpenDisplay (Def_AppCont, server, "appli", "Dialogue", NULL, 0, &n, NULL);
   if (!GDp)
      /* Connexion au serveur X impossible */
      return;
   *app_context = Def_AppCont;
   *Dp = GDp;
   RootShell = XtAppCreateShell ("", "Dialogue", applicationShellWidgetClass, GDp, NULL, 0);

   /* redirige le handler de warnings vers une fonction vide pour eviter */
   /* les delires de motif */
   XtAppSetWarningHandler (*app_context, MyWarningHandler);

   /* Initialisation des options de dialogue */
   DefaultFont = XmFontListCreate (XLoadQueryFont (GDp, "fixed"), XmSTRING_DEFAULT_CHARSET);
   formFONT = XmFontListCreate (XLoadQueryFont (GDp, "fixed"), XmSTRING_DEFAULT_CHARSET);
#ifndef _GTK
   XmSetColorCalculation ((XmColorProc) ThotXmColorProc);
#endif /* _GTK */
#  endif /* _WINDOWS */

   CurrentWait = 0;
   ShowReturn = 0;
   ShowX = 100;
   ShowY = 100;

   /* Initialisation des catalogues */
   NbOccCat = 0;
   NbLibCat = 0;
   PtrCatalogue = NewCatList ();
   NbOccE_List = 0;
   NbLibE_List = 0;
   PtrFreeE_List = NULL;
   /* Initialisation des couleurs et des translations */
#  ifndef _WINDOWS
   TextTranslations = NULL;
#  endif /* _WINDOWS */
#ifndef _GTK
   MainShell = 0;
   PopShell = 0;
#endif /* _GTK */
   /* Pas encore de reference attribuee */
   FirstFreeRef = 0;
#  ifdef _WINDOWS
   return TRUE;
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtaInitDialogueTranslations initialise les translations du         
   dialogue. Ce sont tous les racoursis claviers.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaInitDialogueTranslations (ThotTranslations translations)
#else  /* __STDC__ */
void                TtaInitDialogueTranslations (translations)
ThotTranslations      translations;

#endif /* __STDC__ */
{
   TextTranslations = translations;
}


/*----------------------------------------------------------------------
   TtaChangeDialogueFonts change les polices de caracteres du dialogue.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaChangeDialogueFonts (STRING menufont, STRING formfont)
#else  /* __STDC__ */
void                TtaChangeDialogueFonts (menufont, formfont)
STRING              menufont;
STRING              formfont;
#endif /* __STDC__ */
{
#  ifdef _WINDOWS
   /* see code/chap04/ezfont.c */
#  endif
# ifndef _GTK
#  ifndef _WINDOWS
   if (menufont != NULL)
     {
       XmFontListFree (DefaultFont);
       DefaultFont = XmFontListCreate (XLoadQueryFont (GDp, menufont), XmSTRING_DEFAULT_CHARSET);
     }
   if (menufont != NULL)
     {
       XmFontListFree (formFONT);
       formFONT = XmFontListCreate (XLoadQueryFont (GDp, formfont), XmSTRING_DEFAULT_CHARSET);
     }
#  endif /* _WINDOWS */
#endif _GTK
}


/*----------------------------------------------------------------------
   TtaGetReferencesBase re'serve number re'fe'rences pour          
   l'application a` partir de la base courante.            
   La fonction retourne la base courante.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetReferencesBase (int number)
#else  /* __STDC__ */
int                 TtaGetReferencesBase (number)
int                 number;

#endif /* __STDC__ */
{
   int                 base;

   base = FirstFreeRef;
   if (number > 0)
      FirstFreeRef += number;
   return (base);
}


/*----------------------------------------------------------------------
   TtaInitDialogueWindow Cre'ation et initialisation de la fenetree^tre    
   principale d'une application.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaInitDialogueWindow (STRING name, STRING geometry, Pixmap logo, Pixmap icon, int number, STRING textmenu)
#else  /* __STDC__ */
void                TtaInitDialogueWindow (name, geometry, logo, icon, number, textmenu)
STRING              name;
STRING              geometry;
Pixmap              logo;
Pixmap              icon;
int                 number;
STRING              textmenu;

#endif /* __STDC__ */
{
#ifndef _GTK
#ifndef _WINDOWS
   Arg                 args[10];
   ThotWidget          frame;
   int                 index;
   ThotWidget          row, row1;
   ThotWidget          w;
   ThotWidget          Main_Wd;
   int                 k;
   ThotWidget          menu_bar;
   ThotWindow          wind;
#endif /* !_WINDOWS */

   int                 n;
   STRING              value;

#  ifndef _WINDOWS
   Pixmap              lthot;
#  endif /* _WINDOWS */

   FrRef[0] = 0;
#  ifdef _WINDOWS
   FrMainRef[0] = 0;
#  endif /* _WINDOWS */
   FrameTable[0].WdStatus = 0;
   MainShell = 0;
   PopShell = 0;
   FrameTable[0].WdFrame = 0;	/* widget frame */
   n = 0;
   value = TtaGetEnvString (TEXT("geometry"));

#  ifndef _WINDOWS
   if (value != NULL)
     {
	XtSetArg (args[n], XmNgeometry, value);
	n++;
     }
   else if (geometry != NULL)
     {
	XtSetArg (args[n], XmNgeometry, geometry);
	n++;
     }
#  endif /* _WINDOWS */

   if (number == 0 && logo == 0 && !WithMessages)
      return;

#  ifndef _WINDOWS
   /* Icone de la fenetre d'application */
   XtSetArg (args[n], XmNiconPixmap, icon);
   n++;
   XtSetArg (args[n], XmNmwmDecorations, MWM_DECOR_MENU);
   n++;
   MainShell = XtCreatePopupShell (name, applicationShellWidgetClass, RootShell, args, n);
   if (!MainShell)
      /* La frame de dialogue ne peut etre ouverte */
      TtaError (ERR_cannot_open_main_window);
   else
     {
	/* Creation la fenetre appli */
	n = 0;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNspacing, 0);
	n++;
	Main_Wd = XmCreateMainWindow (MainShell, "Thot", args, n);
	XtManageChild (Main_Wd);

	if (number > 0)
	  {
	    /*** La barre des menus ***/
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     menu_bar = XmCreateMenuBar (Main_Wd, "Barre_menu", args, n);
	     XtManageChild (menu_bar);

	     /*** Creation des menus ***/
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;

	     /* Creation des boutons du menu */
	     index = 0;
	     for (k = 0; k < number; k++)
	       {
		  w = XmCreateCascadeButton (menu_bar, &textmenu[index], args, n);
		  XtManageChild (w);
		  FrameTable[0].WdMenus[k] = w;
		  index += ustrlen (&textmenu[index]) + 1;
	       }
	  }
	else
	   menu_bar = 0;
	frame = 0;
	n = 0;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	frame = XmCreateFrame (Main_Wd, "Frame", args, n);
	XtManageChild (frame);

	/*** Creation de la zone messages ***/
	if (WithMessages || logo != 0)
	  {

	     /* Un row-column pour afficher le logo a gauche des messages */
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNadjustLast, TRUE);
	     n++;
	     row = XmCreateRowColumn (frame, "", args, n);
	     XtManageChild (row);
	     /* Un row-column pour afficher le logo et copyright */
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNorientation, XmVERTICAL);
	     n++;
	     /*XtSetArg(args[n], XmNadjustLast, FALSE); n++; */
	     XtSetArg (args[n], XmNentryAlignment, XmALIGNMENT_CENTER);
	     n++;
	     row1 = XmCreateRowColumn (row, "", args, n);
	     XtManageChild (row1);

	     /* Traitement du logo de l'application */
	     if (logo != 0)
	       {
		  n = 0;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNforeground, FgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNlabelType, XmPIXMAP);
		  n++;
		  XtSetArg (args[n], XmNlabelPixmap, logo);
		  n++;
		  w = XmCreateLabel (row1, "Logo", args, n);
		  XtManageChild (w);
	       }

	     /* Recuperation de la couleur de trace */
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     wind = RootWindow (GDp, DefaultScreen (GDp));
	     lthot = XCreatePixmapFromBitmapData (GDp, wind,
			  copyright_bits, copyright_width, copyright_height,
						  FgMenu_Color, BgMenu_Color,
						  DefaultDepth (GDp, 0));
	     XtSetArg (args[n], XmNlabelType, XmPIXMAP);
	     n++;
	     XtSetArg (args[n], XmNlabelPixmap, lthot);
	     n++;
	     w = XmCreateLabel (row1, "Logo", args, n);
	     XtManageChild (w);

	     if (WithMessages)
	       {
		  /* Le scrolled text */
		  n = 0;
		  XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);
		  n++;
		  XtSetArg (args[n], XmNeditable, FALSE);
		  n++;
		  XtSetArg (args[n], XmNrows, 3);
		  n++;
		  XtSetArg (args[n], XmNcolumns, 50);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNfontList, DefaultFont);
		  n++;
		  FrameTable[0].WdStatus = XmCreateScrolledText (row, "Thot_MSG", args, n);
		  XtManageChild (FrameTable[0].WdStatus);
		  XmTextSetAddMode (FrameTable[0].WdStatus, TRUE);
	       }
	  }

	/*** Creation d'un frame pour attacher le dialogue ***/
	n = 0;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	w = XmCreateFrame (Main_Wd, "Frame", args, n);
	XtManageChild (w);
	FrameTable[0].WdFrame = Main_Wd;	/* widget frame */

	/*** Realisation de la fenetre de dialogue ***/
	if (menu_bar != 0 || frame != 0)
	   XmMainWindowSetAreas (Main_Wd, menu_bar, w, NULL, NULL, frame);
	XtPopup (MainShell, XtGrabNonexclusive);
	FrRef[0] = XtWindowOfObject (w);
     }
#  endif /* _WINDOWS */
#endif /* _GTK */
}


#ifndef _WINDOWS
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ConfirmMessage (Widget w, Widget MsgBox, caddr_t call_d)
#else  /* __STDC__ */
static void         ConfirmMessage (w, MsgBox, call_d)
Widget              w;
Widget              MsgBox;
caddr_t             call_d;

#endif /* __STDC__ */
{
   XtPopdown (MsgBox);
}
#endif /* !_WINDOWS */

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   DisplayConfirmMessage displays the given message (text).        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayConfirmMessage (STRING text)
#else  /* __STDC__ */
void                DisplayConfirmMessage (text)
STRING              text;

#endif /* __STDC__ */
{
#ifndef _GTK
#  ifndef _WINDOWS
   XmString            title_string, OK_string;
   Arg                 args[MAX_ARGS];
   ThotWidget          row, w;
   ThotWidget          msgbox;
   int                 n;

   /* get current position */
   TtaSetDialoguePosition ();

   /* Create the window message */
   title_string = XmStringCreateSimple (text);
   OK_string = XmStringCreateSimple (TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   n = 0;
   XtSetArg (args[n], XmNx, (Position) ShowX);
   n++;
   XtSetArg (args[n], XmNy, (Position) ShowY);
   n++;
   XtSetArg (args[n], XmNallowShellResize, TRUE);
   n++;
   XtSetArg (args[n], XmNuseAsyncGeometry, TRUE);
   n++;
   msgbox = XtCreatePopupShell (TtaGetMessage (LIB, TMSG_LIB_CONFIRM), applicationShellWidgetClass,
				RootShell, args, n);
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNfontList, DefaultFont);
   n++;
   XtSetArg (args[n], XmNdialogTitle, OK_string);
   n++;
   XtSetArg (args[n], XmNautoUnmanage, FALSE);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   w = XmCreateBulletinBoard (msgbox, "Dialogue", args, n);
   XtManageChild (w);

/*** Create a Row-Column to add the label and OK button ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNadjustLast, FALSE);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 0);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   XtSetArg (args[n], XmNspacing, 0);
   n++;
   XtSetArg (args[n], XmNpacking, XmPACK_TIGHT);
   n++;
   XtSetArg (args[n], XmNorientation, XmVERTICAL);
   n++;
   XtSetArg (args[n], XmNresizeHeight, TRUE);
   n++;
   row = XmCreateRowColumn (w, "Dialogue", args, n);
   XtManageChild (row);

   /* the label */
   n = 0;
   XtSetArg (args[n], XmNfontList, DefaultFont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, FgMenu_Color);
   n++;
   XtSetArg (args[n], XmNlabelString, title_string);
   n++;
   w = XmCreateLabel (row, "Thot_MSG", args, n);
   XtManageChild (w);

   /*** Create the Row-Column that includes OK button ***/
   n = 0;
   XtSetArg (args[n], XmNfontList, DefaultFont);
   n++;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
   n++;
   XtSetArg (args[n], XmNmarginWidth, 100);
   n++;
   XtSetArg (args[n], XmNmarginHeight, 0);
   n++;
   row = XmCreateRowColumn (row, "Dialogue", args, n);
   XtManageChild (row);

   /*** Create the OK button ***/
   n = 0;
   XtSetArg (args[n], XmNbackground, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNforeground, FgMenu_Color);
   n++;
   XtSetArg (args[n], XmNbottomShadowColor, BgMenu_Color);
   n++;
   XtSetArg (args[n], XmNfontList, DefaultFont);
   n++;
   w = XmCreatePushButton (row, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), args, n);
   XtManageChild (w);
   XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) ConfirmMessage, msgbox);

   /* display the message */
   XtPopup (msgbox, XtGrabNonexclusive);

   XmStringFree (title_string);
   XmStringFree (OK_string);
#  endif /* _WINDOWS */
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   DisplayMessage display the given messge (text) in main window   
   according to is msgType.		                
   - INFO : bellow the previous message.                   
   - OVERHEAD : instead of the previous message.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayMessage (STRING text, int msgType)
#else  /* __STDC__ */
void                DisplayMessage (text, msgType)
STRING              text;
int                 msgType;

#endif /* __STDC__ */
{
#ifndef _GTK
#  ifndef _WINDOWS
   int                 lg;
   int                 n;
   CHAR_T                buff[500 + 1];
   STRING              pointer;

   /* Is the initialisation done ? */
   lg = ustrlen (text);
   if (MainShell != 0 && WithMessages && lg > 0)
     {
	/* take current messages */
	ustrncpy (buff, XmTextGetString (FrameTable[0].WdStatus), 500);
	n = ustrlen (buff);

	if (msgType == INFO)
	  {
	     /* is it necessary to suppress one or more messages ? */
	     if (n + lg + 1 >= 500)
	       {
		  /* suppress messages */
		  /* kill until we have 50 free characters */
		  while (n + lg + 1 >= 450)
		    {
		       /* search next New Line */
		       pointer = ustrchr (buff, TEXT('\n'));
		       if (pointer == NULL)
			  n = 0;
		       else
			 {
			    ustrcpy (buff, &pointer[1]);
			    n = ustrlen (buff);
			 }
		    }

		  /* add message */
		  if (n > 0)
		     ustrcpy (&buff[n++], "\n");
		  ustrncpy (&buff[n], text, 500 - n);
		  lg += n;

		  /* copy text */
		  XmTextSetString (FrameTable[0].WdStatus, buff);
	       }
	     else
	       {
		  /* enough space, just add new message at the end */
		  ustrcpy (buff, "\n");
		  ustrcat (buff, text);
		  XmTextInsert (FrameTable[0].WdStatus, n, buff);
		  lg += n;
	       }
	     /* select the message end to force scroll down */
	     XmTextSetSelection (FrameTable[0].WdStatus, lg, lg, 500);
	  }
	else if (msgType == OVERHEAD)
	  {
	     /* search last New Line */
	     while (buff[n] != EOL && n >= 0)
		n--;
	     /* replace last message by the new one */
	     XmTextReplace (FrameTable[0].WdStatus, n + 1, ustrlen (buff), text);
	  }
	XFlush (GDp);
     }
#  endif /* _WINDOWS */
# endif /* _GTK */
}

/*----------------------------------------------------------------------
   DefineCallbackDialog de'finit la proce'dure de traitement des      
   retoursde catalogues dans l'application.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaDefineDialogueCallback (void (*procedure) ())
#else  /* __STDC__ */
void                TtaDefineDialogueCallback (procedure)
void                (*procedure) ();

#endif /* __STDC__ */
{
   CallbackDialogue = procedure;
}



/*----------------------------------------------------------------------
   ClearChildren nettoie tous les catalalogues fils du catalogue.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ClearChildren (struct Cat_Context *parentCatalogue)
#else  /* __STDC__ */
static void         ClearChildren (parentCatalogue)
struct Cat_Context *parentCatalogue;

#endif /* __STDC__ */
{
#ifndef _GTK
   register int        icat;
   struct Cat_Context *catalogue;
   struct Cat_List    *adlist;

   /* Tous les sous-menus ou sous-formulaires sont detruits par MOTIF */
   /* Recherche les catalogues qui sont les fils de parentCatalogue         */
   adlist = PtrCatalogue;
   while (adlist != NULL)
     {
	icat = 0;
	while (icat < MAX_CAT)
	  {
	     catalogue = &adlist->Cat_Table[icat];
	     if ((catalogue->Cat_Widget != 0)
		 && (catalogue->Cat_PtParent == parentCatalogue))
	       {
		  /* Libere les blocs des entrees */
		  if ((catalogue->Cat_Type != CAT_TEXT)
		      && (catalogue->Cat_Type != CAT_SELECT)
		      && (catalogue->Cat_Type != CAT_LABEL))
		    {
		       FreeEList (catalogue->Cat_Entries);
		       catalogue->Cat_Entries = NULL;
		    }		/*if */

		  /* Libere le catalogue */
		  catalogue->Cat_Widget = 0;
		  NbLibCat++;
		  NbOccCat--;

		  if ((catalogue->Cat_Type == CAT_POPUP)
		      || (catalogue->Cat_Type == CAT_PULL)
		      || (catalogue->Cat_Type == CAT_MENU)
		      || (catalogue->Cat_Type == CAT_FORM)
		      || (catalogue->Cat_Type == CAT_SHEET)
		      || (catalogue->Cat_Type == CAT_DIALOG))
		     ClearChildren (catalogue);
	       }		/*if */
	     icat++;
	  }			/*while */
	/* On passe au bloc suivant */
	adlist = adlist->Cat_Next;
     }				/*while */
#endif /* _GTK */
}				/*ClearChildren */

/*----------------------------------------------------------------------
   CatEntry recherche si le catalogue de'signe' par sa re'fe'rence   
   existe de'ja` ou une entre'e libre dans la table des catalogues.   
   Retourne l'adresse du catalogue cre'e' ou NULL.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static struct Cat_Context *CatEntry (int ref)
#else  /* __STDC__ */
static struct Cat_Context *CatEntry (ref)
int                 ref;

#endif /* __STDC__ */
{
   register int        icat;
   struct Cat_Context *catlib;
   struct Cat_Context *catval;
   struct Cat_Context *catalogue;
   struct Cat_List    *adlist;

   /* Si la reference depasse la borne declaree */
   if (ref >= FirstFreeRef)
      return (NULL);

   /* Une entree de catalogue libre */
   catlib = NULL;
   /* L'entree qui porte la reference */
   catval = NULL;

   /* Parcours toutes les entrees existantes */
   adlist = PtrCatalogue;
   while (adlist != NULL && catval == NULL)
     {
	icat = 0;
	while (icat < MAX_CAT && catval == NULL)
	  {
	     catalogue = &adlist->Cat_Table[icat];
	     /*===============> C'est la premiere entree libre */
	     if (catalogue->Cat_Widget == 0)
	       {
		  if (catlib == NULL)
		     catlib = catalogue;
	       }
	     /*===============> Le catalogue existe deja */
	     else if (catalogue->Cat_Ref == ref)
		catval = catalogue;

	     icat++;
	  }

	/* On passe au bloc suivant */
	if (adlist->Cat_Next == NULL && catval == NULL && catlib == NULL)
	  {
	     /* Cree une nouvelle liste de catalogues */
	     adlist->Cat_Next = NewCatList ();
	  }
	adlist = adlist->Cat_Next;
     }

   /* Si le catalogue n'existe pas encore */
   if (catval == NULL && catlib != NULL)
     {
#ifndef _GTK
	catlib->Cat_PtParent = NULL;
#endif /* _GTK */
	NbOccCat++;
	NbLibCat--;
	return (catlib);
     }
   else
      return (catval);
}



/*----------------------------------------------------------------------
   DestContenuMenu de'truit les entre'es du catalogue de'signe' par   
   son index.                                                         
   Retourne un code d'erreur.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          DestContenuMenu (struct Cat_Context *catalogue)

#else  /* __STDC__ */
static int          DestContenuMenu (catalogue)
struct Cat_Context *catalogue;

#endif /* __STDC__ */
{
#ifndef _GTK
   register int        ent;
   struct E_List      *adbloc;
   ThotWidget          w;

   if (catalogue == NULL)
      return (1);
   else if (catalogue->Cat_Widget == 0)
      return (1);
   else
     {
	if (catalogue->Cat_Type == CAT_LABEL)
	  {
	     /* Recupere le widget parent du widget detruit */
#            ifndef _WINDOWS
	     w = XtParent (catalogue->Cat_Widget);
	     XtDestroyWidget (catalogue->Cat_Widget);
#            else  /* _WINDOWS */
	     w = GetParent (catalogue->Cat_Widget);
	     DestroyWindow (w);
#            endif /* _WINDOWS */
	     catalogue->Cat_Widget = 0;
	  }
	else
	  {
	     /* Ce sont des menus */
	     adbloc = catalogue->Cat_Entries;
	     /* On saute les entrees 0 et 1 */
	     ent = 2;
	     w = 0;
	     if (adbloc != NULL)
		/* Liberation de toutes les entrees du menu */
		while (adbloc->E_ThotWidget[ent] != 0)
		  {
		     /* Recuperation du widget parent en sautant le widget titre */
#                    ifdef _WINDOWS
		     if (w == 0 && ent != 0)
			w = GetParent (adbloc->E_ThotWidget[ent]);

		     DestroyWindow (adbloc->E_ThotWidget[ent]);
#                    else  /* _WINDOWS */
		     if (w == 0 && ent != 0)
			w = XtParent (adbloc->E_ThotWidget[ent]);

		     /* Libere les widgets */
		     XtUnmanageChild (adbloc->E_ThotWidget[ent]);
		     XtDestroyWidget (adbloc->E_ThotWidget[ent]);
#                    endif /* _WINDOWS */
		     adbloc->E_ThotWidget[ent] = (ThotWidget) 0;

		     /* Faut-il changer de bloc d'entrees ? */
		     ent++;
		     if (ent >= C_NUMBER)
		       {
			  if (adbloc->E_Next != NULL)
			     adbloc = adbloc->E_Next;
			  ent = 0;
		       }	/*if */
		  }		/*while */

	     /* Note que tous les fils sont detruits par MOTIF */
	     if ((catalogue->Cat_Type == CAT_POPUP)
		 || (catalogue->Cat_Type == CAT_PULL)
		 || (catalogue->Cat_Type == CAT_MENU))
		ClearChildren (catalogue);

	     /* Libere les blocs des entrees */
	     adbloc = catalogue->Cat_Entries->E_Next;
	     FreeEList (adbloc);
	     catalogue->Cat_Entries->E_Next = NULL;
	  }			/*else */

	/* On memorise le widget parent des entrees a recreer */
	catalogue->Cat_XtWParent = w;
	return (0);
     }
#endif /* _GTK */
#ifdef _GTK
return 0; /*rajouté pour tester gtk */
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaNewPulldown cre'e un pull-down menu :                           
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre parent identifie le widget pe`re du pull-down menu.  
   Le parame`tre title donne le titre du catalogue.                   
   Le parame`tre number indique le nombre d'entre'es dans le menu.    
   Le parame`tre text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre ] donne les acce'le'rateurs  
   des entre'es du menu.                                              
   Retourne un code d'erreur.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewPulldown (int ref, ThotWidget *parent, STRING title, int number, STRING text, STRING equiv)
#else  /* __STDC__ */
void                TtaNewPulldown (ref, parent, title, number, text, equiv)
int                 ref;
ThotWidget           *parent;
STRING              title;
int                 number;
STRING              text;
STRING              equiv;

#endif /* __STDC__ */
{
   register int        count;
   register int        index;
   register int        ent;
   int                 eindex;
   register int        i;
   int                 n;
   ThotBool            rebuilded;
   struct Cat_Context *catalogue;
   struct E_List      *adbloc;

   ThotWidget          menu;
   ThotWidget          w;
   CHAR_T              heading[200];




   Arg                 args[MAX_ARGS];
   XmString            title_string;




   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);


   title_string = 0;


   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else
     {
	/* Est-ce que le catalogue existe deja ? */
	rebuilded = FALSE;
	if (catalogue->Cat_Widget != 0)
	  {
	     if (catalogue->Cat_Type == CAT_PULL)
	       {
		  DestContenuMenu (catalogue);	/* Modification du catalogue */
		  rebuilded = TRUE;
	       }
	     else
		TtaDestroyDialogue (ref);
	  }			/*if */


	if (parent == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (number == 0)
	  // menu = (ThotMenu) - 1;	/* pas de pull-down */
	  ; /* je ne sais pas */
	else if (!rebuilded)
	  {
	     menu = parent;

	  }
	else
	  {
        menu = (ThotWidget) catalogue->Cat_Widget;
	  }

	catalogue->Cat_Ref = ref;
	catalogue->Cat_Type = CAT_PULL;

	catalogue->Cat_Widget = (ThotWidget) menu;
	adbloc = catalogue->Cat_Entries;
	if (number == 0)
	  {
	    /* c'est un bouton et non un pull-down */
	    if (parent != 0)
	      {
		gtk_widget_show (parent);
		gtk_signal_connect (GTK_OBJECT (parent), "activate",
				    GTK_SIGNAL_FUNC (CallMenu),(gpointer) catalogue);
	      }
	    return;
	  }

	/*** Cree le titre du menu ***/
	if (title != NULL)
	  {
	    ; /* je ne sais pas ce qu'il faut mettre */
	  }
	else if (!rebuilded)
	  {
	     adbloc = NewEList ();
	     catalogue->Cat_Entries = adbloc;
	  }

	if (equiv != NULL)
	   n++;
	i = 0;
	index = 0;
	eindex = 0;
	ent = 2;

	if (text != NULL)
	   while (i < number)
	     {
		count = ustrlen (&text[index]);	/* Longueur de l'intitule */
		/* S'il n'y a plus d'intitule -> on arrete */
		if (count == 0)
		  {
		     i = number;
		     TtaError (ERR_invalid_parameter);
		     return;
		  }
		else
		  {

		     /* Faut-il changer de bloc d'entrees ? */
		     if (ent >= C_NUMBER)
		       {
			  adbloc->E_Next = NewEList ();
			  adbloc = adbloc->E_Next;
			  ent = 0;
		       }
#ifndef _GTK
		     /* Recupere le type de l'entree */
		     adbloc->E_Type[ent] = text[index];
		     adbloc->E_Free[ent] = TEXT('Y');
#endif /* _GTK */
		     /* Note l'accelerateur */
		     if (equiv != NULL)
		       {
			 ; /* je ne sais pas ce qu'il faut faire */
		       }

		     if (text[index] == TEXT('B'))
		       /*__________________________________________ Creation d'un bouton __*/
		       {
			 w = gtk_menu_item_new_with_label ( &text[index + 1]);
			 gtk_widget_show (w);
			 gtk_menu_append (GTK_MENU (parent),w);
			 gtk_signal_connect(GTK_OBJECT (w), "activate",
						   GTK_SIGNAL_FUNC (CallMenu),(gpointer)catalogue);
			 adbloc->E_ThotWidget[ent] = w;
		       }
		     else if (text[index] == TEXT('T'))
		       /*__________________________________________ Creation d'un toggle __*/
		       {
			  w = gtk_check_menu_item_new_with_label (&text[index + 1]);
			  gtk_widget_show (w);
			  gtk_menu_append (GTK_MENU (parent),w);
			  gtk_check_menu_item_set_state (GTK_CHECK_MENU_ITEM (w), TRUE);
			  gtk_signal_connect (GTK_OBJECT (w), "activate",
					      GTK_SIGNAL_FUNC (CallMenu),(gpointer)catalogue);
			 
			  gtk_check_menu_item_set_show_toggle (GTK_CHECK_MENU_ITEM (w), TRUE);
			  adbloc->E_ThotWidget[ent] = w;
			 
		       }
		     else if (text[index] == TEXT('M'))
		       /*_______________________________________ Creation d'un sous-menu __*/
		       {
			 w =  gtk_menu_item_new_with_label (&text[index + 1]);
			 gtk_widget_show (w);
			 gtk_menu_append (GTK_MENU (parent),w);
			 adbloc->E_ThotWidget[ent] = w;						 
		       }
		     else if (text[index] == TEXT('F'))
		       /*_________________________________ Creation d'un sous-formulaire __*/
		       {
			 w = gtk_menu_item_new_with_label ( &text[index + 1]);
			 gtk_menu_append (GTK_MENU (parent),w);
			 gtk_widget_show (w);
			 gtk_signal_connect (GTK_OBJECT (w), "activate",
						   GTK_SIGNAL_FUNC (CallMenu),(gpointer) catalogue);
			 adbloc->E_ThotWidget[ent] = w;
		       }
		     else if (text[index] == TEXT('D'))
		       /*_________________________________ Creation d'un menu dynamique__*/
		       {
			 w = gtk_menu_item_new_with_label ( &text[index + 1]);
			 gtk_menu_append (GTK_MENU (parent),w);
			 gtk_widget_show (w);
			 gtk_signal_connect (GTK_OBJECT (w), "activate",
						   GTK_SIGNAL_FUNC (CallMenu),(gpointer) catalogue);
			 adbloc->E_ThotWidget[ent] = w;
		       }
		     else if (text[index] == TEXT('S'))
		       /*_________________________________ Creation d'un separateur __*/
		       {
			 w = gtk_menu_item_new ();
			 gtk_widget_show (w);
			 gtk_menu_append (GTK_MENU (parent),w);

			  adbloc->E_ThotWidget[ent] = w;
                        
		       }
		     else
		       /*____________________________________ Une erreur de construction __*/
		       {
			  TtaDestroyDialogue (ref);
			  /* Type d'entree non defini */
			  TtaError (ERR_invalid_parameter);
			  return;
		       }


		     /* liberation de la string */
		 //    if (equiv != NULL && !title_string)
		//	XmStringFree (title_string);
		        /* je ne sais ce qu'il faut faire  faire */
		     i++;
		     ent++;
		     index += count + 1;
		  }
	     }

	/* Attache le pull-down menu au widget passe en parametre */
	if (parent != 0 && !rebuilded)
	  {
	    ; /* je ne sais ce qu'il faut faire  faire */
	  }
     }

}


/*----------------------------------------------------------------------
   TtaSetPulldownOff suspend le pulldown                           
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
#ifdef __STDC__
void                WIN_TtaSetPulldownOff (int ref, ThotMenu parent, HWND owner)
#else  /* __STDC__ */
void                WIN_TtaSetPulldownOff (ref, parent, owner)
int                 ref;
ThotMenu            parent;
HWND                owner;
#endif /* __STDC__ */
#else  /* !_WINDOWS */
#ifdef __STDC__
void                TtaSetPulldownOff (int ref, ThotWidget parent)
#else  /* __STDC__ */
void                TtaSetPulldownOff (ref, parent)
int                 ref;
ThotWidget          parent;
#endif /* __STDC__ */
#endif /* _WINDOWS */
{
#ifndef _GTK  
 struct Cat_Context *catalogue;

#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
#  else  /* _WINDOWS */
   int frame;
#  endif /* _WINDOWS */

   if (ref == 0)
      TtaError (ERR_invalid_reference);
   else if (parent == 0)
      TtaError (ERR_invalid_parent_dialogue);
   else
     {
	catalogue = CatEntry (ref);
	if (catalogue == NULL)
	   TtaError (ERR_invalid_reference);
#   ifndef _WINDOWS
	else if (catalogue->Cat_Widget != 0)
	  {
             XtSetArg (args[0], XmNsubMenuId, NULL);
             XtSetValues (parent, args, 1);
             XtManageChild (parent);
	  }
#   else  /* _WINDOWS */
	frame = GetMainFrameNumber (owner);
    EnableMenuItem ((HMENU)WinMenus[frame], (UINT)parent, MF_GRAYED);
	DrawMenuBar (FrMainRef[frame]); 
#            endif /* _WINDOWS */
     }
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   TtaSetPulldownOn reactive le pulldown                           
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS
#ifdef __STDC__
void                WIN_TtaSetPulldownOn (int ref, ThotMenu parent, HWND owner)
#else  /* __STDC__ */
void                WIN_TtaSetPulldownOn (ref, parent, owner)
int                 ref;
ThotMenu            parent;
HWND                owner;
#endif /* __STDC__ */
#else  /* !_WINDOWS */
#ifdef __STDC__
void                TtaSetPulldownOn (int ref, ThotWidget parent)
#else  /* __STDC__ */
void                TtaSetPulldownOn (ref, parent)
int                 ref;
ThotWidget          parent;
#endif /* __STDC__ */
#endif /* _WINDOWS */
{
#ifndef _GTK
   struct Cat_Context *catalogue;
   ThotWidget          menu;

#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
#  else  /* _WINDOWS */
   int frame;
#  endif /* _WINDOWS */

   if (ref == 0)
      TtaError (ERR_invalid_reference);
   else if (parent == 0)
      TtaError (ERR_invalid_parent_dialogue);
   else
     {
	catalogue = CatEntry (ref);
	if (catalogue == NULL)
	   TtaError (ERR_invalid_reference);
	else if (catalogue->Cat_Widget != 0)
	  {
	     menu = catalogue->Cat_Widget;
#            ifndef _WINDOWS
             XtSetArg (args[0], XmNsubMenuId, menu);
             XtSetValues (parent, args, 1);
             XtManageChild (parent);
#            else  /* _WINDOWS */
			 frame = GetMainFrameNumber (owner);
             EnableMenuItem ((HMENU)WinMenus[frame], (UINT)parent, MF_ENABLED);
			 DrawMenuBar (FrMainRef[frame]); 
#            endif /* _WINDOWS */
	  }
     }
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   TtaNewPopup cre'e un pop-up menu :                                 
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre title donne le titre du catalogue.                   
   Le parame`tre number indique le nombre d'entre'es dans le menu.    
   Le parame`tre text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   Le parame`tre button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewPopup (int ref, ThotWidget parent, STRING title, int number, STRING text, STRING equiv, CHAR_T button)
#else  /* __STDC__ */
void                TtaNewPopup (ref, parent, title, number, text, equiv, button)
int                 ref;
ThotWidget          parent;
STRING              title;
int                 number;
STRING              text;
STRING              equiv;
CHAR_T                button;

#endif /* __STDC__ */
{
#ifndef _GTK
   register int        count;
   register int        index;
   register int        ent;
   register int        i;
   int                 eindex;
   ThotBool            rebuilded;
   struct Cat_Context *catalogue;
   struct E_List      *adbloc;
   CHAR_T                heading[200];

#  ifdef _WINDOWS
   HMENU               menu;
   HMENU               w;
   int                 nbOldItems, ndx;
   struct Cat_Context *copyCat;
#  else  /* _WINDOWS */
   Arg                 args[MAX_ARGS];
   ThotWidget          menu;
   XmString            title_string;
   ThotWidget          w;
   int                 n;
#  endif /* !_WINDOWS */

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   catalogue = CatEntry (ref);
#  ifdef _WINDOWS
#  ifdef AMAYA_DEBUG
   fprintf (stderr, "TtaNewPopup(ref %d, parent %X, title %s, number %d,\n text %s, equiv %s, button %d)\n",
	    ref, parent, title, number, text, equiv, button);
   fprintf (stderr, "catalogue : %X\n", catalogue);
#  endif AMAYA_DEBUG
#  else  /* _WINDOWS */
   title_string = 0;
#  endif /* !_WINDOWS */
   menu = 0;
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else
     {
	/* Est-ce que le catalogue existe deja ? */
	rebuilded = FALSE;
	if (catalogue->Cat_Widget != 0)
	  {
	     if (catalogue->Cat_Type == CAT_POPUP)
	       {
		 /* Modification du catalogue */
		  DestContenuMenu (catalogue);
		  rebuilded = TRUE;
	       }
	     else
		TtaDestroyDialogue (ref);
	  }

	if (!rebuilded)
	  {
	     /* Creation du Popup Shell pour contenir le menu */
#            ifdef _WINDOWS
         /* menu = parent; */
#            else  /* _WINDOWS */
	     n = 0;
	     /*XtSetArg(args[n], XmNallowShellResize, TRUE); n++; */
	     XtSetArg (args[n], XmNheight, (Dimension) 10);
	     n++;
	     XtSetArg (args[n], XmNwidth, (Dimension) 10);
	     n++;
	     menu = XtCreatePopupShell ("Dialogue", xmMenuShellWidgetClass, RootShell, args, n);
#            endif /* !_WINDOWS */
	  }

	/* Cree le menu correspondant */
#       ifndef _WINDOWS
	if (button == TEXT('R'))
	   XtSetArg (args[0], XmNwhichButton, Button3);
	else if (button == TEXT('M'))
	   XtSetArg (args[0], XmNwhichButton, Button2);
	else
	  {
	     button = TEXT('L');
	     XtSetArg (args[0], XmNwhichButton, Button1);
	  }
#       endif /* !_WINDOWS */

	if (!rebuilded)
	  {
#            ifndef _WINDOWS
	     n = 1;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNrowColumnType, XmMENU_POPUP);
	     n++;
	     menu = XmCreateRowColumn (menu, "Dialogue", args, n);
	     XtAddCallback (XtParent (menu), XmNpopdownCallback, (XtCallbackProc) UnmapMenu, catalogue);
#            else  /* _WINDOWS */
		 menu = CreatePopupMenu ();
#            endif /* !_WINDOWS */
	     catalogue->Cat_Widget = menu;
	     catalogue->Cat_Ref = ref;
	     catalogue->Cat_Type = CAT_POPUP;
	     catalogue->Cat_Button = button;
	     /* Initialisation de la liste des widgets fils */
	     adbloc = NewEList ();
	     catalogue->Cat_Entries = adbloc;
	  }
	else
	  {
	     /* Mise a jour du menu existant */
	     menu = catalogue->Cat_Widget;
	     adbloc = catalogue->Cat_Entries;
	     /* Si on a change de bouton on met a jour le widget avec args[0] */
	     if (catalogue->Cat_Button != (UCHAR_T) button)
	       {
#                 ifndef _WINDOWS
		  XtSetValues (menu, args, 1);
#                 endif /* !_WINDOWS */
		  catalogue->Cat_Button = button;
	       }
	     else
		button = catalogue->Cat_Button;
	  }

	catalogue->Cat_Data = -1;

#   ifdef _WINDOWS
    if (currentParent != 0) {
       copyCat = catalogue;
       WIN_AddFrameCatalogue (currentParent, copyCat) ;
    }
#   endif /* _WINDOWS */

/*** Cree le titre du menu ***/
	if (title != NULL)
	  {
#            ifndef _WINDOWS
	     n = 0;
	     title_string = XmStringCreateSimple (title);
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
#            endif /* !_WINDOWS */
	     if (!rebuilded)
	       {
#                 ifdef _WINDOWS
		  adbloc->E_ThotWidget[0] = (ThotWidget) 0;
		  adbloc->E_ThotWidget[1] = (ThotWidget) 0;
#                 else  /* _WINDOWS */
		  XtSetArg (args[n], XmNfontList, DefaultFont);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNforeground, FgMenu_Color);
		  n++;
		  w = XmCreateLabel (menu, "Dialogue", args, n);
		  XtManageChild (w);
		  adbloc->E_ThotWidget[0] = w;
		  n = 0;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
		  n++;
		  w = XmCreateSeparator (menu, "Dialogue", args, n);
		  XtManageChild (w);
		  adbloc->E_ThotWidget[1] = w;
#                 endif /* !_WINDOWS */
	       }
#            ifndef _WINDOWS
	     else if (adbloc->E_ThotWidget[0] != 0)
		XtSetValues (adbloc->E_ThotWidget[0], args, n);
	     XmStringFree (title_string);
#            endif /* !_WINDOWS */
	  }

	/* Cree les differentes entrees du menu */
#       ifndef _WINDOWS
	n = 0;
	XtSetArg (args[n], XmNfontList, DefaultFont);
	n++;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNforeground, FgMenu_Color);
	n++;
	if (equiv != NULL)
	   n++;
#       endif /* !_WINDOWS */
#   ifdef _WINDOWS
    nbOldItems = GetMenuItemCount (menu);
    for (ndx = 0; ndx < nbOldItems; ndx ++)
        if (!DeleteMenu (menu, ref + ndx, MF_BYCOMMAND))
           DeleteMenu (menu, ndx, MF_BYPOSITION);
        /* RemoveMenu (menu, ref + ndx, MF_BYCOMMAND) ; */
#   endif /* _WINDOWS */

	i = 0;
	index = 0;
	eindex = 0;
	ent = 2;

	if (text != NULL)
	   while (i < number)
	     {
		count = ustrlen (&text[index]);	/* Longueur de l'intitule */
		/* S'il n'y a plus d'intitule -> on arrete */
		if (count == 0)
		  {
		     i = number;
		     TtaError (ERR_invalid_parameter);
		     return;
		  }
		else
		  {

		     /* Faut-il changer de bloc d'entrees ? */
		     if (ent >= C_NUMBER)
		       {
			  adbloc->E_Next = NewEList ();
			  adbloc = adbloc->E_Next;
			  ent = 0;
		       }	/*if */

		     /* Recupere le type de l'entree */
		     adbloc->E_Type[ent] = text[index];
		     adbloc->E_Free[ent] = TEXT('Y');

		     /* Note l'accelerateur */
		     if (equiv != NULL)
		       {
#                         ifdef _WINDOWS
                          if (&equiv[eindex] != EOS) {
                             if (parseAccelerator (&equiv[eindex], &fVirt, &key))
                                addAccelerator (1, fVirt, key, ref + i);
                          }
                          eindex += ustrlen (&equiv[eindex]) + 1;
#                         else  /* !_WINDOWS */
			  title_string = XmStringCreate (&equiv[eindex], XmSTRING_DEFAULT_CHARSET);
			  eindex += ustrlen (&equiv[eindex]) + 1;
			  XtSetArg (args[n - 1], XmNacceleratorText, title_string);
#                         endif /* !_WINDOWS */
		       }

		     if (text[index] == TEXT('B'))
		       /*__________________________________________ Creation d'un bouton __*/
		       {
#                         ifdef _WINDOWS
			  AppendMenu (menu, MF_STRING, ref + i, &text[index + 1]);
			  adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#                         else  /* _WINDOWS */
			  w = XmCreatePushButton (menu, &text[index + 1], args, n);
			  XtManageChild (w);
			  adbloc->E_ThotWidget[ent] = w;
			  XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallMenu, catalogue);
#                         endif /* !_WINDOWS */
		       }
		     else if (text[index] == TEXT('T'))
		       /*__________________________________________ Creation d'un toggle __*/
		       {
#                         ifdef _WINDOWS
			  AppendMenu (menu, MF_STRING | MF_UNCHECKED, ref + i, &text[index + 1]);
			  adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#                         else  /* _WINDOWS */
			  XtSetArg (args[n], XmNvisibleWhenOff, TRUE);
			  XtSetArg (args[n + 1], XmNselectColor, BgMenu_Color);
			  w = XmCreateToggleButton (menu, &text[index + 1], args, n + 2);
			  XtManageChild (w);
			  adbloc->E_ThotWidget[ent] = w;
			  XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallMenu, catalogue);
#                         endif /* !_WINDOWS */
		       }
		     else if (text[index] == TEXT('M'))
		       /*_______________________________________ Creation d'un sous-menu __*/
		       {
			  /* En attendant le sous-menu on cree un bouton */
#                         ifdef _WINDOWS
			  w = (HMENU) CreateMenu ();
              /* InsertMenu (menu, i, MF_POPUP, (UINT) w, (LPCTSTR) (&heading)); */
			  AppendMenu (menu, MF_POPUP, (UINT) w, (LPCTSTR) (&text[index + 1]));
			  /* AppendMenu (menu, MF_STRING, ref + i, &text[index + 1]); */
			  adbloc->E_ThotWidget[ent] = (ThotWidget) w;
#                         else  /* _WINDOWS */
			  w = XmCreateCascadeButton (menu, &text[index + 1], args, n);
			  adbloc->E_ThotWidget[ent] = w;
#                         endif /* !_WINDOWS */
		       }
		     else if (text[index] == TEXT('F'))
		       /*_________________________________ Creation d'un sous-formulaire __*/
		       {
			  ustrcpy (heading, &text[index + 1]);
			  ustrcat (heading, TEXT("..."));
#                         ifdef _WINDOWS
			  AppendMenu (menu, MF_STRING, (UINT) (ref + i), (LPCTSTR) (&heading));
			  adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#                         else  /* _WINDOWS */
			  w = XmCreatePushButton (menu, heading, args, n);
			  XtManageChild (w);
			  adbloc->E_ThotWidget[ent] = w;
#                         endif /* !_WINDOWS */
		       }
		     else if (text[index] == TEXT('S'))
		       /*_________________________________ Creation d'un separateur __*/
		       {
#                         ifdef _WINDOWS
			  AppendMenu (menu, MF_SEPARATOR, 0, NULL);
			  adbloc->E_ThotWidget[ent] = (ThotWidget) i;
#                         else  /* _WINDOWS */
			  XtSetArg (args[n], XmNseparatorType, XmSINGLE_DASHED_LINE);
			  w = XmCreateSeparator (menu, "Dialogue", args, n + 1);
			  XtManageChild (w);
			  adbloc->E_ThotWidget[ent] = w;
#                         endif /* !_WINDOWS */
		       }
		     else
		       /*____________________________________ Une erreur de construction __*/
		       {
			  TtaDestroyDialogue (ref);
			  TtaError (ERR_invalid_parameter);	/* Type d'entree non defini */
			  return;
		       }

#                    ifndef _WINDOWS
		     /* liberation de la string */
		     if (equiv != NULL)
			XmStringFree (title_string);
#                    endif /* !_WINDOWS */

		     i++;
		     ent++;
		     index += count + 1;
		  }		/*else */
	     }

     }
#endif /* _GTK */

}


/*----------------------------------------------------------------------
   AddInFormulary recherche une entree libre dans le formulaire  
   et cre'e e'ventuellement un nouveau bloc d'entre'es et un nouveau  
   Row-Column.                                                        
   Retourne :                                                         
   + index -> le nume'ro d'entre'e sur l'ensemble des blocs.          
   + entry -> l'index dans le bloc des entre'es concerne'.            
   + adbloc -> le bloc des entre'es concerne'.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotWidget  * AddInFormulary (struct Cat_Context *catalogue, int *index, int *entry, struct E_List **adbloc)
#else  /* __STDC__ */
static ThotWidget   *AddInFormulary (catalogue, index, entry, adbloc)
struct Cat_Context *catalogue;
int                *index;
int                *entry;
struct E_List     **adbloc;

#endif /* __STDC__ */
{
#ifndef _GTK
   ThotWidget          row;
   ThotWidget          w;

#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
   int                 n;
#  endif /* _WINDOWS */

   /* Il faut sauter la 1ere entree allouee a un Row-Column */
   *entry = 1;
   *index = 1;
   /* Le 1er bloc sert aux boutons du feuillet */
   *adbloc = catalogue->Cat_Entries->E_Next;

   /* Recupere le Row-Column racine du formulaire */
   w = (*adbloc)->E_ThotWidget[0];
#  ifdef _WINDOWS
   row = GetParent (w);
#  else  /* _WINDOWS */
   row = XtParent (w);
#  endif /* !_WINDOWS */

   /*** Recherche une entree libre dans le formulaire ***/
   while ((*adbloc)->E_ThotWidget[*entry] != 0)
     {
	/* Est-ce un widget de Row-Column (n fois le facteur de blocage) ? */
	if (*index % catalogue->Cat_FormPack == 0)
	   w = (*adbloc)->E_ThotWidget[*entry];

	(*entry)++;
	(*index)++;
	if (*entry >= C_NUMBER)
	  {
	     /* Il faut changer de bloc et enventuellement en creer un */
	     if ((*adbloc)->E_Next == NULL)
		(*adbloc)->E_Next = NewEList ();
	     *adbloc = (*adbloc)->E_Next;
	     *entry = 0;
	  }
     }

   /* Faut-il ajouter un nouveau Row-Column ? */
   if (*index % catalogue->Cat_FormPack == 0)
     {
#       ifdef _WINDOWS
#       else  /* _WINDOWS */
	n = 0;
	XtSetArg (args[n], XmNadjustLast, FALSE);
	n++;
	if (catalogue->Cat_in_lines)
	  {
	   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	   n++;
	   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);
	   n++;
	  }
	else
	  {
	   XtSetArg (args[n], XmNorientation, XmVERTICAL);
	   n++;
	   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);
	   n++;
	  }
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	w = XmCreateRowColumn (row, "Dialogue", args, n);
	XtManageChild (w);
#       endif /* _WINDOWS */
	(*adbloc)->E_ThotWidget[*entry] = w;
	(*adbloc)->E_Free[*entry] = TEXT('X');

	(*index)++;
	(*entry)++;
	if (*entry >= C_NUMBER)
	  {
	     /* Il faut changer de bloc et enventuellement en creer un */
	     if ((*adbloc)->E_Next == NULL)
		(*adbloc)->E_Next = NewEList ();
	     *adbloc = (*adbloc)->E_Next;
	     *entry = 0;
	  }
     }				/*if */

   /* il faut mettre a jour l'index */
   *index += C_NUMBER;
   return (w);
#endif /* _GTK */
#ifdef _GTK 
return (NULL);
#endif _GTK
}


/*----------------------------------------------------------------------
   TtaNewIconMenu cre'e un sous-menu :                                        
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre ref_parent identifie le formulaire pe`re.            
   Le parametre entry de'signe l'entre'e correspondante dans le menu  
   pe`re. Le parame`tre title donne le titre du catalogue.            
   Le parame`tre number indique le nombre d'entre'es dans le menu.    
   Le parame`tre icons contient la liste des icones du catalogue.     
   Tout changement de se'lection dans le sous-menu est imme'diatement 
   signale' a` l'application.                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewIconMenu (int ref, int ref_parent, int entry, STRING title, int number, Pixmap * icons, ThotBool horizontal)
#else  /* __STDC__ */
void                TtaNewIconMenu (ref, ref_parent, entry, title, number, icons, horizontal)
int                 ref;
int                 ref_parent;
int                 entry;
STRING              title;
int                 number;
Pixmap             *icons;
ThotBool            horizontal;

#endif /* __STDC__ */
{
#ifndef _GTK
#  ifndef _WINDOWS
   int                 i;
   int                 ent;
   int                 n;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   struct E_List      *adbloc;
   Arg                 args[MAX_ARGS];
   ThotWidget          menu;
   ThotWidget          w;
   ThotWidget          row;
   XmString            title_string;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   menu = 0;
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else if (catalogue->Cat_Widget != 0)
      TtaError (ERR_invalid_reference);
   else
     {
	catalogue->Cat_React = TRUE;
	title_string = 0;
	parentCatalogue = CatEntry (ref_parent);
	/*__________________________________ Le catalogue parent n'existe pas __*/
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (parentCatalogue->Cat_Widget == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_________________________________________ Sous-menu d'un formulaire __*/
	else if (parentCatalogue->Cat_Type == CAT_FORM
		 || parentCatalogue->Cat_Type == CAT_SHEET
		 || parentCatalogue->Cat_Type == CAT_DIALOG)
	  {
	     w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);

	     /*** Cree un sous-menu d'un formulaire ***/
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     menu = XmCreateRowColumn (w, "Dialogue", args, n);

	     catalogue->Cat_Ref = ref;
	     catalogue->Cat_Type = CAT_FMENU;
	     catalogue->Cat_Data = -1;
	     catalogue->Cat_Widget = menu;
	     catalogue->Cat_PtParent = parentCatalogue;
	     adbloc->E_ThotWidget[ent] = (ThotWidget) (catalogue);
	     adbloc->E_Free[ent] = TEXT('N');
	     catalogue->Cat_EntryParent = i;
	     adbloc = NewEList ();
	     catalogue->Cat_Entries = adbloc;
	  }

	/*** Cree le titre du sous-menu ***/
	if (title != NULL)
	  {
	     n = 0;
	     title_string = XmStringCreateSimple (title);
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     w = XmCreateLabel (menu, "Dialogue", args, n);
	     XtManageChild (w);
	     adbloc->E_ThotWidget[0] = w;
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
	     n++;
	     w = XmCreateSeparator (menu, "Dialogue", args, n);
	     XtManageChild (w);
	     adbloc->E_ThotWidget[1] = w;
	     XmStringFree (title_string);
	  }

	/* Cree un Row-Column d'icone dans le Row-Column du formulaire */
	n = 0;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNspacing, 0);
	n++;
	XtSetArg (args[n], XmNrowColumnType, XmMENU_BAR);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	if (horizontal)
	   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	else
	   XtSetArg (args[n], XmNorientation, XmVERTICAL);
	n++;
	row = XmCreateRowColumn (menu, "Dialogue", args, n);
	XtManageChild (row);

	/*** Cree les differentes entrees du sous-menu ***/
	n = 0;
	XtSetArg (args[n], XmNfontList, DefaultFont);
	n++;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNlabelType, XmPIXMAP);
	n++;
	i = 0;
	ent = 2;
	while (i < number)
	  {
	     /* Faut-il changer de bloc d'entrees ? */
	     if (ent >= C_NUMBER)
	       {
		  adbloc->E_Next = NewEList ();
		  adbloc = adbloc->E_Next;
		  ent = 0;
	       }

	     /* On ne traite que des entrees de type bouton */
	     adbloc->E_Type[ent] = TEXT('B');
	     adbloc->E_Free[ent] = TEXT('Y');
	     XtSetArg (args[n], XmNlabelPixmap, icons[i]);
	     w = XmCreateCascadeButton (row, "dialogue", args, n + 1);
	     XtManageChild (w);
	     adbloc->E_ThotWidget[ent] = w;
	     XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallRadio, catalogue);
	     i++;
	     ent++;
	  }
     }
#  endif /* _WINDOWS */
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   TtaNewSubmenu cre'e un sous-menu :                                 
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre ref_parent identifie le formulaire pe`re.            
   Le parametre entry de'signe l'entre'e correspondante dans le menu  
   pe`re. Le parame`tre title donne le titre du catalogue.            
   Le parame`tre number indique le nombre d'entre'es dans le menu.    
   Le parame`tre text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   Quand le parame`tre react est vrai, tout changement de se'lection  
   dans le sous-menu est imme'diatement signale' a` l'application.    
   retourne un pointeur sur le sous_menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               TtaNewSubmenu (int ref, int ref_parent, int entry, STRING title, int number, STRING text, STRING equiv, ThotBool react)
#else  /* __STDC__ */
void              TtaNewSubmenu (ref, ref_parent, entry, title, number, text, equiv, react)
int                 ref;
int                 ref_parent;
int                 entry;
STRING              title;
int                 number;
STRING              text;
STRING              equiv;
ThotBool            react;

#endif /* __STDC__ */
{

   register int        count;
   register int        index;
   int                 eindex;
   int                 i;
   int                 ent;
   ThotBool            rebuilded;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   struct E_List      *adbloc;


 
   XmString            title_string;
   ThotWidget          sub_menu;
   CHAR_T              heading[200];
   int                 n;


   ThotWidget          w;
   ThotWidget          row;
   CHAR_T              button;



   if (ref == 0) {
      TtaError (ERR_invalid_reference);
      return;
   } 

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else {
        catalogue->Cat_React = react;
        title_string = 0;

        /* Faut-il detruire le catalogue precedent ? */
        rebuilded = FALSE;
        if (catalogue->Cat_Widget != 0)
           if (catalogue->Cat_Type == CAT_MENU || catalogue->Cat_Type == CAT_FMENU) {
              DestContenuMenu (catalogue);	/* Modification du catalogue */
              rebuilded = TRUE;
		   } else /* Modification du catalogue */
                  TtaDestroyDialogue (ref);

        /*======================================> Recherche le catalogue parent */
        parentCatalogue = CatEntry (ref_parent);
        /*__________________________________ Le catalogue parent n'existe pas __*/
        if (parentCatalogue == NULL) {
           TtaError (ERR_invalid_parent_dialogue);
           return;
		} else if (parentCatalogue->Cat_Widget == 0) {
               TtaError (ERR_invalid_parent_dialogue);
               return;
		} 
	else if (parentCatalogue->Cat_Type == CAT_FORM  || 
                   parentCatalogue->Cat_Type == CAT_SHEET || 
                   parentCatalogue->Cat_Type == CAT_DIALOG) {
	  ; /*je ne sais pas ce qu'il faut faire */
		}
	else { 

               /*_______________________________________________ Sous-menu d'un menu __*/
               if (parentCatalogue->Cat_Type == CAT_POPUP || 
                   parentCatalogue->Cat_Type == CAT_PULL  || 
                   parentCatalogue->Cat_Type == CAT_MENU) {

                   /* Faut-il reconstruire entierement le menu */
                   if (!rebuilded) {
                      /*=========> Recherche l'entree du menu parent corespondante */
                      adbloc = parentCatalogue->Cat_Entries;
                      ent = entry + 2;		/* decalage de 2 pour le widget titre */
                      while (ent >= C_NUMBER) {
                            if (adbloc->E_Next == NULL) {
                               TtaError (ERR_invalid_parent_dialogue);
                               return;
							} else
                                  adbloc = adbloc->E_Next;
                            ent -= C_NUMBER;
					  } /*while */

                   
                         /* Cree un sous-menu d'un menu */
                         w = adbloc->E_ThotWidget[ent];
			 sub_menu = gtk_menu_new ();
			 gtk_menu_item_set_submenu (GTK_MENU_ITEM (w), sub_menu);

                         catalogue->Cat_Ref = ref;
                         catalogue->Cat_Type = CAT_MENU; 
			 catalogue->Cat_PtParent = parentCatalogue;
                         catalogue->Cat_Widget = sub_menu;

                         /*** Relie le sous-menu au bouton du menu ***/
                         w = adbloc->E_ThotWidget[ent];
                         adbloc = NewEList ();
                         catalogue->Cat_Entries = adbloc;
		   
		 
                          //   TtaError (ERR_invalid_parameter);
                          //   return;
		   } 
		   else { /* On recupere le widget du menu */
                          sub_menu = catalogue->Cat_Widget;
                          adbloc = catalogue->Cat_Entries;
		   }
	       } 
	       else {
		 /*____________________________________________ Sous-menu non valide __*/
		 TtaError (ERR_invalid_parameter);
		 return;
	       } 
	       /*** Cree le titre du sous-menu ***/
	       if (title != NULL) {
	//	 adbloc->E_ThotWidget[0] = w;
	//	 adbloc->E_ThotWidget[1] = w;
	       }
	


               /* Cree les differentes entrees du sous-menu */
               i = 0;
               index = 0;
               eindex = 0;
               ent = 2;
               while (i < number) {
                     count = ustrlen (&text[index]);	/* Longueur de l'intitule */
                     /* S'il n'y a plus d'intitule -> on arrete */
                     if (count == 0) {
                        i = number;
                        TtaError (ERR_invalid_parameter);
                        return;
					 } else { /* Faut-il changer de bloc d'entrees ? */
                            if (ent >= C_NUMBER) {
                               adbloc->E_Next = NewEList ();
                               adbloc = adbloc->E_Next;
                               ent = 0;
							}	/*if */

                            /* Recupere le type de l'entree */
                           // adbloc->E_Type[ent] = text[index];
                           // adbloc->E_Free[ent] = TEXT('Y');

                            /* Note l'accelerateur */
                            if (equiv != NULL) {
			      ; /* je ne sais pas ce qu'il faut faire */

							} 

                            if (text[index] == TEXT('B')) {
                               /*________________________________________ Creation d'un bouton __*/

			      w  = gtk_menu_item_new_with_label (&text[index + 1]);
			      gtk_widget_show (w);
			      gtk_menu_append (GTK_MENU (sub_menu),w);
			      gtk_signal_connect(GTK_OBJECT (w), "activate",
							GTK_SIGNAL_FUNC (CallMenu),(gpointer)catalogue);
			      adbloc->E_ThotWidget[ent] = w;

							} 
			    else if (text[index] == TEXT('T')) {
                                   /*________________________________________ Creation d'un toggle __*/
			      w = gtk_check_menu_item_new_with_label (&text[index + 1]);
			      gtk_widget_show (w);
			      gtk_menu_append (GTK_MENU (sub_menu),w);
			      gtk_signal_connect(GTK_OBJECT (w), "activate",
							GTK_SIGNAL_FUNC (CallMenu),(gpointer) catalogue);
			      adbloc->E_ThotWidget[ent] = w;

			    } 
			    else if (text[index] == TEXT('M')) {
                                   /*________________________________________ Appel d'un sous-menu __*/
			       w  = gtk_menu_item_new_with_label (&text[index + 1]);
			       adbloc->E_ThotWidget[ent] = w;

			    }
			    else if (text[index] == TEXT('F')) {
                                   /*__________________________________ Appel d'un sous-formulaire __*/

                                   ustrcpy (heading, &text[index + 1]);
                                   ustrcat (heading, "...");
                                   w = gtk_menu_item_new_with_label (&text[index + 1]);
				   gtk_widget_show (w);
                                   adbloc->E_ThotWidget[ent] = w;
			    } 
			    else if (text[index] == TEXT('S')) {
			      /*_________________________________ Creation d'un separateur __*/
			      w =  gtk_menu_item_new ();
			      gtk_widget_show (w);
			      gtk_menu_append (GTK_MENU (sub_menu),w);
			      adbloc->E_ThotWidget[ent] = w;			    
			    } 
			    else {
                                   /*__________________________________ Une erreur de construction __*/
                                   TtaDestroyDialogue (ref);
                                   TtaError (ERR_invalid_parameter);	/* Type d'entree non defini */
                                   return;
							} 

                            /* liberation de la string */
                            if (equiv != NULL){ ;} /*je ne sais pas ce qu'il faut faire */
			    
			   
                            i++;
                            index += count + 1;
                            ent++;

			 }  
	       }		/*while */
	      

	}
     
   }  
}


/*----------------------------------------------------------------------
   TtaSetMenuForm fixe la selection dans un sous-menu de formulaire : 
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre val de'signe l'entre'e se'lectionne'e.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetMenuForm (int ref, int val)
#else  /* __STDC__ */
void                TtaSetMenuForm (ref, val)
int                 ref;
int                 val;

#endif /* __STDC__ */
{
#ifndef _GTK
#  ifndef _WINDOWS
   register int        i, n;
   register int        ent;
   ThotBool            visible;
   struct E_List      *adbloc;
   Arg                 args[MAX_ARGS];
#  endif /* !_WINDOWS */
   struct Cat_Context *catalogue;

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else
     {
	/* Si ce n'est pas un sous-menu de formulaire */
	if (catalogue->Cat_Type != CAT_FMENU)
	  {
	     TtaError (ERR_invalid_reference);
	     return;
	  }

	/* Est-ce que le sous-menu est actuellement affiche */
#       ifndef _WINDOWS
	if (XtIsManaged (catalogue->Cat_Widget))
	   visible = TRUE;
	else
	  {
	     visible = FALSE;
	     XtManageChild (catalogue->Cat_Widget);
	  }

	/* Positionnement de la valeur de chaque entree */
	adbloc = catalogue->Cat_Entries;
	ent = 0;
	i = 2;			/* decalage de 2 pour le widget titre */
	while (adbloc != NULL)
	  {
	     while (i < C_NUMBER)
	       {
		  if (adbloc->E_ThotWidget[i] == 0)
		     i = C_NUMBER;
		  else if (ent == val)
		    {
		       n = 0;
		       XtSetArg (args[n], XmNset, TRUE);
		       n++;
		       XtSetValues (adbloc->E_ThotWidget[i], args, n);
		    }
		  else
		    {
		       n = 0;
		       XtSetArg (args[n], XmNset, FALSE);
		       n++;
		       XtSetValues (adbloc->E_ThotWidget[i], args, n);
		    }
		  i++;
		  ent++;
	       }
	     /* Passe au bloc suivant */
	     adbloc = adbloc->E_Next;
	     i = 0;
	  }			/*while */

	if (!visible)
	   XtUnmanageChild (catalogue->Cat_Widget);
#       endif /* _WINDOWS */
	/* La selection de l'utilisateur est desactivee */
	catalogue->Cat_Data = val;
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaNewToggleMenu cre'e un sous-menu a` choix multiples :           
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre ref_parent identifie le formulaire pe`re.            
   Le parame`tre title donne le titre du catalogue.                   
   Le parame`tre number indique le nombre d'entre'es dans le menu.    
   Le parame`tre text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   Quand le parame`tre react est vrai, tout changement de se'lection  
   dans le sous-menu est imme'diatement signale' a` l'application.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewToggleMenu (int ref, int ref_parent, STRING title, int number, STRING text, STRING equiv, ThotBool react)
#else  /* __STDC__ */
void                TtaNewToggleMenu (ref, ref_parent, title, number, text, equiv, react)
int                 ref;
int                 ref_parent;
STRING              title;
int                 number;
STRING              text;
STRING              equiv;
ThotBool            react;

#endif /* __STDC__ */
{
#ifndef _GTK
#  ifndef _WINDOWS
   register int        count;
   register int        index;
   int                 eindex;
   int                 i;
   int                 ent;
   int                 n;
   ThotBool            rebuilded;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   struct E_List      *adbloc;
   Arg                 args[MAX_ARGS];
   ThotWidget          menu;
   ThotWidget          w;
   ThotWidget          row;
   XmString            title_string;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else
     {
	catalogue->Cat_React = react;
	title_string = 0;

	/* Faut-il detruire le catalogue precedent ? */
	rebuilded = FALSE;
	if (catalogue->Cat_Widget != 0)
	   if (catalogue->Cat_Type == CAT_TMENU)
	     {
		DestContenuMenu (catalogue);	/* Modification du catalogue */
		rebuilded = TRUE;
	     }
	   else
	      /* Modification du catalogue */
	      TtaDestroyDialogue (ref);

	/*======================================> Recherche le catalogue parent */
	if (!rebuilded)
	   parentCatalogue = CatEntry (ref_parent);
	else
	   parentCatalogue = catalogue->Cat_PtParent;

	/*__________________________________ Le catalogue parent n'existe pas __*/
	if (parentCatalogue == NULL)
	   TtaError (ERR_invalid_parent_dialogue);
	else if (parentCatalogue->Cat_Widget == 0)
	   TtaError (ERR_invalid_parent_dialogue);
	/*_________________________________________ Sous-menu d'un formulaire __*/
	else if (parentCatalogue->Cat_Type == CAT_FORM
		 || parentCatalogue->Cat_Type == CAT_SHEET
		 || parentCatalogue->Cat_Type == CAT_DIALOG)
	  {
	     if (!rebuilded)
	       {
		  w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);

		  /* Cree un sous-menu d'un formulaire */
		  n = 0;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNmarginWidth, 0);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNspacing, 0);
		  n++;
		  menu = XmCreateRowColumn (w, "Dialogue", args, n);

		  catalogue->Cat_Ref = ref;
		  catalogue->Cat_Type = CAT_TMENU;
		  catalogue->Cat_Widget = menu;
		  catalogue->Cat_PtParent = parentCatalogue;
		  adbloc->E_ThotWidget[ent] = (ThotWidget) catalogue;
		  adbloc->E_Free[ent] = TEXT('N');
		  catalogue->Cat_EntryParent = i;
		  /* prepare le nouveau catalogue */
		  adbloc = NewEList ();
		  catalogue->Cat_Entries = adbloc;
	       }
	     else
	       {
		  menu = catalogue->Cat_Widget;
		  adbloc = catalogue->Cat_Entries;
	       }

	     /*** Cree le titre du sous-menu ***/
	     if (title != NULL)
	       {
		  n = 0;
		  title_string = XmStringCreateSimple (title);
		  XtSetArg (args[n], XmNlabelString, title_string);
		  n++;
		  if (!rebuilded)
		    {
		       XtSetArg (args[n], XmNfontList, DefaultFont);
		       n++;
		       XtSetArg (args[n], XmNmarginHeight, 0);
		       n++;
		       XtSetArg (args[n], XmNbackground, BgMenu_Color);
		       n++;
		       XtSetArg (args[n], XmNforeground, FgMenu_Color);
		       n++;
		       w = XmCreateLabel (menu, "Dialogue", args, n);
		       XtManageChild (w);
		       adbloc->E_ThotWidget[0] = w;
		       n = 0;
		       XtSetArg (args[n], XmNbackground, BgMenu_Color);
		       n++;
		       XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
		       n++;
		       w = XmCreateSeparator (menu, "Dialogue", args, n);
		       XtManageChild (w);
		       adbloc->E_ThotWidget[1] = w;
		    }
		  else if (adbloc->E_ThotWidget[0] != 0)
		     XtSetValues (adbloc->E_ThotWidget[0], args, n);
		  XmStringFree (title_string);
	       }

	     if (!rebuilded)
	       {
		  /* Cree un Row-Column de Toggle dans le Row-Column du formulaire */
		  n = 0;
		  XtSetArg (args[n], XmNradioAlwaysOne, FALSE);
		  n++;
		  XtSetArg (args[n], XmNmarginWidth, 0);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNspacing, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  row = XmCreateRowColumn (menu, "Dialogue", args, n);
		  XtManageChild (row);
	       }
	     else
		/* Sinon on recupere le widget parent des entrees */
		row = catalogue->Cat_XtWParent;

	     /* note le nombre d'entrees du toggle */
	     catalogue->Cat_Data = number;	/* recouvre Cat_XtWParent */

	     /* Cree les differentes entrees du sous-menu */
	     n = 0;
	     XtSetArg (args[n], XmNindicatorType, XmN_OF_MANY);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     if (equiv != NULL)
	       {
		  XtSetArg (args[n + 1], XmNacceleratorText, title_string);
		  n++;
	       }
	     i = 0;
	     index = 0;
	     eindex = 0;
	     ent = 2;		/* decalage de 2 pour le widget titre */
	     while (i < number)
	       {
		  count = ustrlen (&text[index]);	/* Longueur de l'intitule */
		  /* S'il n'y a plus d'intitule -> on arrete */
		  if (count == 0)
		    {
		       number = i;

		       /* note le nombre d'entrees du toggle */
		       catalogue->Cat_Data = number;
		       TtaError (ERR_invalid_parameter);
		       return;
		    }
		  else
		    {

		       /* Faut-il changer de bloc d'entrees ? */
		       if (ent >= C_NUMBER)
			 {
			    adbloc->E_Next = NewEList ();
			    adbloc = adbloc->E_Next;
			    ent = 0;
			 }

		       /* E_Free est utilise pour marquer les entrees selectionnes */
		       /* Intialement les entrees sont non selectionnees */
		       adbloc->E_Free[ent] = TEXT('N');

		       /* Note l'accelerateur */
		       if (equiv != NULL)
			 {
			    title_string = XmStringCreate (&equiv[eindex], XmSTRING_DEFAULT_CHARSET);
			    eindex += ustrlen (&equiv[eindex]) + 1;
			 }
		       /* On accepte toggles, boutons et separateurs */
		       if (text[index] == TEXT('B') || text[index] == TEXT('T'))
			 /*________________________________________ Creation d'un bouton __*/
			 {
			   adbloc->E_Type[ent] = TEXT('B');
			   w = XmCreateToggleButton (row, &text[index + 1], args, n);
		       
			   XtManageChild (w);
			   adbloc->E_ThotWidget[ent] = w;
			   XtAddCallback (w, XmNarmCallback, (XtCallbackProc) CallToggle, catalogue);
			 }
		       else if (text[index] == TEXT('S'))
			 /*_________________________________ Creation d'un separateur __*/
			 {
			    XtSetArg (args[n], XmNseparatorType, XmSINGLE_DASHED_LINE);
			    w = XmCreateSeparator (row, "Dialogue", args, n+1);
			    XtManageChild (w);
			    adbloc->E_ThotWidget[ent] = w;
			 }
		       /* liberation de la string */
		       if (equiv != NULL)
			  XmStringFree (title_string);
		       i++;
		       index += count + 1;
		       ent++;
		    }
	       }
	  }
	else
	   TtaError (ERR_invalid_parameter);
     }
#  endif /* _WINDOWS */
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaSetToggleMenu fixe la selection dans un toggle-menu :           
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre val de'signe l'entre'e se'lectionne'e (-1 pour       
   toutes les entre'es). Le parame`tre on indique que le bouton       
   correspondant doit e^tre allume' (on positif) ou e'teint (on nul). 
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS 
#ifdef __STDC__
void                WIN_TtaSetToggleMenu (int ref, int val, ThotBool on, HWND owner)
#else  /* __STDC__ */
void                WIN_TtaSetToggleMenu (ref, val, on, owner)
int                 ref;
int                 val;
ThotBool            on;
HWND                owner;
#endif /* __STDC__ */
#else  /* !_WINDOWS */
#ifdef __STDC__
void                TtaSetToggleMenu (int ref, int val, ThotBool on)
#else  /* __STDC__ */
void                TtaSetToggleMenu (ref, val, on)
int                 ref;
int                 val;
ThotBool            on;

#endif /* __STDC__ */
#endif /* _WINDOWS */
{
#ifndef _GTK
#  ifdef _WINDOWS 
   struct Cat_Context *catalogue;
   HMENU              hMenu;
   struct E_List      *adbloc;
   int                ent = 2;

   catalogue = CatEntry (ref);
   adbloc    = catalogue->Cat_Entries;

   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else {
        hMenu = catalogue->Cat_Widget;
        if (on) {
			if (IsMenu (adbloc->E_ThotWidget[ent + val])) {
              if (CheckMenuItem (adbloc->E_ThotWidget[ent], ref + val + 1, MF_CHECKED) == 0xFFFFFFFF) 
				  WinErrorBox (NULL);
			} else if (CheckMenuItem (hMenu, ref + val, MF_CHECKED) == 0xFFFFFFFF) {
                   hMenu = GetMenu (owner);
                   if (CheckMenuItem (hMenu, ref + val, MF_CHECKED) == 0xFFFFFFFF) 
                      WinErrorBox (NULL);
			}
        } else {
			if (CheckMenuItem (hMenu, ref + val, MF_UNCHECKED) == 0xFFFFFFFF) {
               hMenu = GetMenu (owner);
               if (CheckMenuItem (hMenu, ref + val, MF_UNCHECKED) == 0xFFFFFFFF) {
                  if (IsMenu (adbloc->E_ThotWidget[ent + val]))
                     if (CheckMenuItem (adbloc->E_ThotWidget[ent], ref + val + 1, MF_UNCHECKED) == 0xFFFFFFFF) 
                         WinErrorBox (NULL);
			   }
			}
		}
   }
#  else  /* !_WINDOWS  */
   ThotWidget          w;
   Arg                 args[MAX_ARGS];
   register int        i, n;
   register int        ent;
   ThotBool            visible;
   struct E_List      *adbloc;
   struct Cat_Context *catalogue;

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else
     {
	/* Si ce n'est pas un menu sous-menu */
	if (catalogue->Cat_Type != CAT_TMENU
	    && catalogue->Cat_Type != CAT_MENU
	    && catalogue->Cat_Type != CAT_PULL
	    && catalogue->Cat_Type != CAT_POPUP)
	  {
	     TtaError (ERR_invalid_reference);
	     return;
	  }
	else if (catalogue->Cat_Type == CAT_TMENU && val > catalogue->Cat_Data)
	  {
	     /* ce n'est pas une entree du toggle */
	     TtaError (ERR_invalid_reference);
	     return;
	  }
	/* Est-ce que le sous-menu est actuellement affiche */
	else if (XtIsManaged (catalogue->Cat_Widget))
	   visible = TRUE;
	else
	  {
	     visible = FALSE;
	     XtManageChild (catalogue->Cat_Widget);
	  }

	/* Positionnement de la valeur de chaque entree */
	adbloc = catalogue->Cat_Entries;
	ent = 0;
	i = 2;			/* decalage de 2 pour le widget titre */
	while (adbloc != NULL)
	  {
	     while (i < C_NUMBER)
	       {
		  /* S'il n'y a pas de bouton  */
		  if (adbloc->E_ThotWidget[i] == 0)
		     i = C_NUMBER;
		  /* C'est une entree qu'il faut initialiser */
		  else if ((ent == val) || (val == -1))
		    {
		       if (catalogue->Cat_Type != CAT_TMENU && adbloc->E_Type[i] != TEXT('T'))
			 {
			    /* ce n'est pas une entree du toggle */
			    TtaError (ERR_invalid_reference);
			    return;
			 }
		       else
			 {
			    w = adbloc->E_ThotWidget[i];
			    /* retire les callbacks */
			    if (catalogue->Cat_Type == CAT_TMENU)
			       XtRemoveCallback (w, XmNarmCallback, (XtCallbackProc) CallToggle, catalogue);
			    else
			       XtRemoveCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallMenu, catalogue);
			    if (on)
			       /* Bouton allume */
			      {
				 n = 0;
				 XtSetArg (args[n], XmNset, TRUE);
				 n++;
				 XtSetValues (w, args, n);
			      }
			    else
			       /* Etat initial du bouton : eteint */
			      {
				 n = 0;
				 XtSetArg (args[n], XmNset, FALSE);
				 n++;
				 XtSetValues (w, args, n);
			      }

			    /* retablit les callbacks */
			    if (catalogue->Cat_Type == CAT_TMENU)
			       XtAddCallback (w, XmNarmCallback, (XtCallbackProc) CallToggle, catalogue);
			    else
			       XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallMenu, catalogue);
			 }
		       adbloc->E_Free[i] = TEXT('N');		/* La valeur est la valeur initiale */
		    }
		  /* Sinon l'entree garde son etat precedent  */
		  i++;
		  ent++;
	       }		/*while */
	     /* Passe au bloc suivant */
	     adbloc = adbloc->E_Next;
	     i = 0;
	  }			/*while */

	if (!visible)
	   XtUnmanageChild (catalogue->Cat_Widget);
     }
#  endif /* _WINDOWS */
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaChangeMenuEntry modifie l'intitule' texte de l`entre'e entry    
   du menu de'signe' par sa re'fe'rence ref.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaChangeMenuEntry (int ref, int entry, STRING text)
#else  /* __STDC__ */
void                TtaChangeMenuEntry (ref, entry, text)
int                 ref;
int                 entry;
STRING              text;

#endif /* __STDC__ */
{
#ifndef _GTK
   ThotWidget          w;
   struct Cat_Context *catalogue;
   struct E_List      *adbloc;
   int                 ent;

#  ifndef _WINDOWS
   int                 n;
   Arg                 args[MAX_ARGS];
   XmString            title_string;
#  endif /* _WINDOWS */

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   /* Est-ce qu'il s'agit bien d'un menu ou d'un sous-menu ? */
   else if (catalogue->Cat_Widget == 0
	    || (catalogue->Cat_Type != CAT_MENU
		&& catalogue->Cat_Type != CAT_POPUP
		&& catalogue->Cat_Type != CAT_PULL
		&& catalogue->Cat_Type != CAT_TMENU
		&& catalogue->Cat_Type != CAT_FMENU))
      TtaError (ERR_invalid_reference);
   else
     {
	/* Recherche l'entree dans le menu ou sous-menu */
	adbloc = catalogue->Cat_Entries;
	ent = entry + 2;	/* decalage de 2 pour le widget titre */
	while (ent >= C_NUMBER)
	  {
	     if (adbloc->E_Next == NULL)
	       {
		  TtaError (ERR_invalid_parameter);
		  return;
	       }
	     else
		adbloc = adbloc->E_Next;
	     ent -= C_NUMBER;
	  }			/*while */

	if (adbloc->E_ThotWidget[ent] == 0)
	   TtaError (ERR_invalid_parameter);
	else
	  {
	     w = adbloc->E_ThotWidget[ent];
#            ifndef _WINDOWS
	     title_string = XmStringCreateSimple (text);
	     n = 0;
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetValues (w, args, n);
	     XtManageChild (w);
	     XmStringFree (title_string);
#            endif /* _WINDOWS */
	  }
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaRedrawMenuEntry modifie la couleur et/ou la police de l'entre'e 
   entry du menu de'signe' par sa re'fe'rence ref.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaRedrawMenuEntry (int ref, int entry, STRING fontname, Pixel color, int activate)
#else  /* __STDC__ */
void                TtaRedrawMenuEntry (ref, entry, fontname, color, activate)
int                 ref;
int                 entry;
STRING              fontname;
Pixel               color;
int                 activate;

#endif /* __STDC__ */
{
#ifndef _GTK
   ThotWidget          w;
   struct Cat_Context *catalogue;
   struct E_List      *adbloc;
   int                 ent;

#  ifndef _WINDOWS
   int                 n;
   Arg                 args[MAX_ARGS];
   XmFontList          font;
#  endif /* _WINDOWS */

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);

   /* Est-ce qu'il s'agit bien d'un menu ou d'un sous-menu ? */
   else if (catalogue->Cat_Widget == 0
	    || (catalogue->Cat_Type != CAT_MENU
		&& catalogue->Cat_Type != CAT_POPUP
		&& catalogue->Cat_Type != CAT_PULL
		&& catalogue->Cat_Type != CAT_TMENU
		&& catalogue->Cat_Type != CAT_FMENU))
      TtaError (ERR_invalid_parameter);
   else
     {
	/* Recherche l'entree dans le menu ou sous-menu */
	adbloc = catalogue->Cat_Entries;
	ent = entry + 2;	/* decalage de 2 pour le widget titre */
	while (ent >= C_NUMBER)
	  {
	     if (adbloc->E_Next == NULL)
	       {
		  TtaError (ERR_invalid_parameter);
		  return;
	       }
	     else
		adbloc = adbloc->E_Next;
	     ent -= C_NUMBER;
	  }			/*while */

	if (adbloc->E_ThotWidget[ent] == 0)
	   TtaError (ERR_invalid_parameter);
	else
	  {
	     w = adbloc->E_ThotWidget[ent];
#        ifndef _WINDOWS
	     /* Recupere si necessaire la couleur par defaut */
	     n = 0;
	     /* Faut-il changer la police de caracteres ? */
	     if (fontname != NULL)
            font = XmFontListCreate (XLoadQueryFont (GDp, fontname), XmSTRING_DEFAULT_CHARSET);
	     else
            font = DefaultFont;
	     XtSetArg (args[n], XmNfontList, font);
	     n++;

	     if (color != -1)
	       {
		  /* La couleur imposee par l'application */
		  XtSetArg (args[n], XmNforeground, color);
		  n++;
	       }
	     else
	       {
		  /* La couleur par defaut */
		  XtSetArg (args[n], XmNforeground, FgMenu_Color);
		  n++;
	       }

	     /* Faut-il activer ou desactiver le Callback */
	     if (activate != -1)
		if (catalogue->Cat_Type == CAT_POPUP
		    || catalogue->Cat_Type == CAT_PULL
		    || catalogue->Cat_Type == CAT_MENU)
		  {
		     XtRemoveCallback (w, XmNactivateCallback, (XtCallbackProc) CallMenu, catalogue);
		     if (activate)
			XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallMenu, catalogue);
		  }
		else
		   /*CAT_FMENU et CAT_TMENU */ if (activate)
		  {
		     XtSetArg (args[n], XmNsensitive, TRUE);
		     n++;
		  }
		else
		  {
		     XtSetArg (args[n], XmNsensitive, FALSE);
		     n++;
		  }

	     XtSetValues (w, args, n);
	     XtManageChild (w);

	     if (fontname != NULL)
		XmFontListFree (font);
#            endif /* _WINDOWS */
	  }
     }
#endif /* _GTK *//
}


/*----------------------------------------------------------------------
   DestForm de'truit un formulaire ou une feuille de saisie:          
   Le parame`tre ref donne la re'fe'rence du catalogue.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          DestForm (int ref)
#else  /* __STDC__ */
static int          DestForm (ref)
int                 ref;

#endif /* __STDC__ */
{
#ifndef _GTK 
   register int        entry;
   ThotWidget          w;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   struct E_List      *adbloc;

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      return (1);
   else if (catalogue->Cat_Widget == 0)
      return (1);
   else
     {
       /*=====================> Detruit une feuille de saisie ou un formulaire*/
	if (catalogue->Cat_Type != CAT_INT
	    && catalogue->Cat_Type != CAT_TEXT
	    && catalogue->Cat_Type != CAT_FORM
	    && catalogue->Cat_Type != CAT_SELECT
	    && catalogue->Cat_Type != CAT_SHEET
	    && catalogue->Cat_Type != CAT_DIALOG)
	   return (1);		/* Ce n'est pas le cas : On ne fait rien */
	else
	  {
	     /* Il faut liberer l'entree du menu pere qui le reference */
	     parentCatalogue = catalogue->Cat_PtParent;
	     entry = catalogue->Cat_EntryParent;
	     catalogue->Cat_PtParent = NULL;
	     catalogue->Cat_EntryParent = 0;

	     if (parentCatalogue != NULL)
	       {
		  /* Recherche l'entree du menu qui lui correspond */
		  adbloc = parentCatalogue->Cat_Entries;
		  while (entry >= C_NUMBER)
		    {
		       entry -= C_NUMBER;
		       if (adbloc->E_Next == NULL)
			  entry = -1;
		       else
			  adbloc = adbloc->E_Next;
		    }		/*while */

		  if (entry >= 0)
		    {
		       if (parentCatalogue->Cat_Type == CAT_FORM
			   || parentCatalogue->Cat_Type == CAT_SHEET
			   || parentCatalogue->Cat_Type == CAT_DIALOG)
			 /*__________________________________________ Dans un formulaire __*/
			 {
#                           ifndef _WINDOWS
			    XtUnmanageChild (catalogue->Cat_Widget);
#                           endif /* _WINDOWS */
			    adbloc->E_ThotWidget[entry] = (ThotWidget) 0;
			    adbloc->E_Free[entry] = TEXT('Y');
			 }
		       else if ((parentCatalogue->Cat_Type == CAT_POPUP)
				|| (parentCatalogue->Cat_Type == CAT_PULL)
				|| (parentCatalogue->Cat_Type == CAT_MENU))
			 /*________________________________ Formulaire attache a un menu __*/
			 {
			    if ((adbloc->E_Type[entry] == TEXT('F'))
				&& (adbloc->E_Free[entry] == TEXT('N')))
			      {
				 /* Detache le formulaire du bouton du menu */
				 w = adbloc->E_ThotWidget[entry];
#                                ifndef _WINDOWS
				 XtRemoveCallback (w, XmNactivateCallback, (XtCallbackProc) INITetPOSform, catalogue);
#                                endif /* _WINDOWS */
				 adbloc->E_Free[entry] = TEXT('Y');
			      }
			 }
		    }
	       }

	     /* Libere le bloc des entrees alloue */
	     if ((catalogue->Cat_Type == CAT_FORM)
		 || (catalogue->Cat_Type == CAT_INT)
		 || (catalogue->Cat_Type == CAT_SHEET)
		 || (catalogue->Cat_Type == CAT_DIALOG))
	       {
		  FreeEList (catalogue->Cat_Entries);
		  catalogue->Cat_Entries = NULL;
		  /* Note que tous les fils sont detruits */
		  if (catalogue->Cat_Type != CAT_INT)
		    {
		      ClearChildren (catalogue);
#ifndef _WINDOWS
		      XtRemoveCallback (catalogue->Cat_Widget, XmNdestroyCallback, (XtCallbackProc) formKill, catalogue);
#endif /* _WINDOWS */
		    }
	       }

#ifndef _WINDOWS
	     XtDestroyWidget (catalogue->Cat_Widget);
#endif /* _WINDOWS */
	     /* Libere le catalogue */
	     catalogue->Cat_Widget = 0;
	     NbLibCat++;
	     NbOccCat--;
	     return (0);
	  }
     }
#endif  /* _GTK */
}


/*----------------------------------------------------------------------
   TtaUnmapDialogue desactive le dialogue s'il est actif.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaUnmapDialogue (int ref)

#else  /* __STDC__ */
void                TtaUnmapDialogue (ref)
int                 ref;

#endif /* __STDC__ */
{
#ifndef _GTK
   struct Cat_Context *catalogue;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      return;
   else if (catalogue->Cat_Widget == 0)
      return;
#  ifndef _WINDOWS
   else if (XtIsManaged (catalogue->Cat_Widget))
     {
	/* Traitement particulier des formulaires */
	if (catalogue->Cat_Type == CAT_FORM
	    || catalogue->Cat_Type == CAT_SHEET
	    || catalogue->Cat_Type == CAT_DIALOG)
	   XtUnmanageChild (XtParent (catalogue->Cat_Widget));
	XtUnmanageChild (catalogue->Cat_Widget);
     }
#  endif /* _WINDOWS */

   /* Si le catalogue correspond au dernier TtaShowDialogue */
   if (ShowCat != NULL)
      if (ref == ShowCat->Cat_Ref)
	{
	   /* Debloque eventuellement une attente active sur le catalogue */
	   ShowCat = NULL;
	   ShowReturn = 0;
	}
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaDestroyDialogue de'truit le catalogue de'signe' par ref.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaDestroyDialogue (int ref)
#else  /* __STDC__ */
void                TtaDestroyDialogue (ref)
int                 ref;

#endif /* __STDC__ */
{
#ifndef _GTK
   register int        entry;
   ThotWidget          w;
   struct E_List      *adbloc;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   int                 n;

#  ifdef _WINDOWS
   int nbMenuItems, itNdx;
#  else  /* _WINDOWS */
   Arg                 args[MAX_ARGS];
#  endif /* _WINDOWS */

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      return;
   else if (catalogue->Cat_Widget == 0)
      return;
   else
     {
	TtaUnmapDialogue (ref);
	/*===============================================> Detruit un sous-menu */
	if ((catalogue->Cat_Type == CAT_MENU)
	    || (catalogue->Cat_Type == CAT_TMENU)
	    || (catalogue->Cat_Type == CAT_FMENU))
	  {
	     /* Il faut liberer l'entree du menu pere qui le reference */
	     parentCatalogue = catalogue->Cat_PtParent;
	     entry = catalogue->Cat_EntryParent;
	     catalogue->Cat_PtParent = NULL;
	     catalogue->Cat_EntryParent = 0;

	     if (parentCatalogue != NULL)
	       {
		  /* Recherche l'entree du menu qui lui correspond */
		  adbloc = parentCatalogue->Cat_Entries;
		  while (entry >= C_NUMBER)
		    {
		       entry -= C_NUMBER;
		       if (adbloc->E_Next == NULL)
			  entry = -1;
		       else
			  adbloc = adbloc->E_Next;
		    }		/*while */

		  if (entry >= 0)
		    {
		       if (catalogue->Cat_Type == CAT_FMENU
			   || catalogue->Cat_Type == CAT_TMENU)
			 /*___________________________________ Sous-menu d'un formulaire __*/
			 {
			    /* Libere l'entree du sous-menu dans le formulaire */
			    adbloc->E_ThotWidget[entry] = (ThotWidget) 0;
			    adbloc->E_Free[entry] = TEXT('Y');

			 }
		       else
			 /*_________________________________________ Sous-menu d'un menu __*/
			 {
			    if ((adbloc->E_Type[entry] == TEXT('M'))
				&& (adbloc->E_Free[entry] == TEXT('N')))
			      {
/*** Delie le sous-menu du bouton du menu ***/
				 w = adbloc->E_ThotWidget[entry];
#                ifdef _WINDOWS
                 nbMenuItems = GetMenuItemCount (w);
                 for (itNdx = 0; itNdx < nbMenuItems; itNdx ++) 
                     if (!DeleteMenu (w, ref + itNdx, MF_BYCOMMAND))
                        if (!DeleteMenu (w, ref + itNdx, MF_BYPOSITION))
                           WinErrorBox (NULL);;
                     /* RemoveMenu (w, ref + itNdx, MF_BYCOMMAND); */
                 DestroyMenu (w);
                 subMenuID [currentFrame] = (UINT)w;
                 /* CHECK  CHECK  CHECK  CHECK  CHECK  CHECK  CHECK */
#                endif /* _WINDOWS */
				 n = 0;
#                                ifndef _WINDOWS
				 XtSetArg (args[n], XmNsubMenuId, 0);
				 n++;
				 XtSetValues (w, args, n);
				 XtManageChild (w);
#                                endif /* _WINDOWS */
				 adbloc->E_Free[entry] = TEXT('Y');
			      }
			 }
		    }		/*if entry */
	       }
	  }
	/*=================================================> Un autre catalogue */
	else if ((catalogue->Cat_Type != CAT_POPUP)
		 && (catalogue->Cat_Type != CAT_PULL)
		 && (catalogue->Cat_Type != CAT_LABEL))
	  {
	     /* C'est surement une destruction de formulaire */
	     if (DestForm (ref) != 0)
		TtaError (ERR_invalid_reference);
#            ifndef _WINDOWS
	     if (PopShell != 0)
	       {
		 XtDestroyWidget (PopShell);
		 PopShell = 0;
	       }
#            endif /* _WINDOWS */
	     return;
	  }

	/* Note que tous les fils sont detruits */
	if ((catalogue->Cat_Type == CAT_POPUP)
	    || (catalogue->Cat_Type == CAT_PULL)
	    || (catalogue->Cat_Type == CAT_MENU))
	   ClearChildren (catalogue);

	/* Libere les blocs des entrees */
	FreeEList (catalogue->Cat_Entries);
	catalogue->Cat_Entries = NULL;

#       ifndef _WINDOWS
	if (catalogue->Cat_Type != CAT_PULL)
	   XtDestroyWidget (catalogue->Cat_Widget);
#       endif /* _WINDOWS */
	/* Libere le catalogue */
	catalogue->Cat_Widget = 0;
	NbLibCat++;
	NbOccCat--;
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaChangeFormTitle change le titre d'un formulaire ou d'une feuille        
   de dialogue :                                                   
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame'tre title donne le titre du catalogue.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaChangeFormTitle (int ref, STRING title)
#else  /* __STDC__ */
void                TtaChangeFormTitle (ref, title)
int                 ref;
STRING              title;

#endif /* __STDC__ */
{
#ifndef _GTK 
   struct Cat_Context *catalogue;

#  ifndef _WINDOWS
   int                 n;
   Arg                 args[MAX_ARGS];
   XmString            title_string;
#  endif /* _WINDOWS */

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   /* Est-ce que le catalogue existe deja ? */
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else
     {
#       ifndef _WINDOWS
	title_string = XmStringCreateSimple (title);
	n = 0;
	XtSetArg (args[n], XmNdialogTitle, title_string);
	n++;
	XtSetValues (catalogue->Cat_Widget, args, n);
#       endif /* _WINDOWS */
     }
#endif  /* _GTK */
}

/*----------------------------------------------------------------------
  NewSheet
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NewSheet (int ref, ThotWidget parent, STRING title, int number, STRING text, ThotBool horizontal, int package, CHAR_T button, int dbutton, int cattype)
#else  /* __STDC__ */
static void         NewSheet (ref, parent, title, number, text, horizontal, package, button, dbutton, cattype)
int                 ref;
ThotWidget          parent;
STRING              title;
int                 number;
STRING              text;
ThotBool            horizontal;
int                 package;
CHAR_T                button;
int                 dbutton;
int                 cattype;

#endif /* __STDC__ */
{
#ifndef _GTK
   int                 ent;
   int                 n;
   int                 index;
   int                 count;
   struct Cat_Context* catalogue;
   struct E_List*      adbloc;
   ThotWidget          form;
   ThotWidget          w;
   STRING              ptr = NULL;

#  ifdef _WINDOWS
   struct Cat_Context* copyCat;
   int                 strSize;
   int                 charWidth;
   TEXTMETRIC          tm;
   WIN_GetDeviceContext (-1);
   GetTextMetrics (TtDisplay, &tm);
   charWidth = tm.tmAveCharWidth;
#  else /* _WINDOWS */
   Arg                 args[MAX_ARGS];
   Arg                 argform[1];
   XmString            title_string, OK_string;
   ThotWidget          row;
#  endif /* _WINDOWS */

#  ifdef _WINDOWS
   cyValue = 10;
#  endif /* _WINDOWS */
   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else
     {

	/* Est-ce que le catalogue existe deja ? */
	if (catalogue->Cat_Widget != 0)
	   TtaDestroyDialogue (ref);	/* Reconstruction du catalogue */

	/* Recherche le widget parent */
	if (MainShell == 0 && parent == 0)
	  {
#       ifndef _WINDOWS
	    OK_string = XmStringCreateSimple (TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
	    n = 0;
	    XtSetArg (args[n], XmNx, (Position) ShowX);
	    n++;
	    XtSetArg (args[n], XmNy, (Position) ShowY);
	    n++;
	    XtSetArg (args[n], XmNallowShellResize, TRUE);
	    n++;
	    XtSetArg (args[n], XmNuseAsyncGeometry, TRUE);
	    n++;
	    PopShell = XtCreatePopupShell ("", applicationShellWidgetClass, RootShell, args, 0);
#       endif /* _WINDOWS */
	  }
	/*________________________________________________ Feuillet principal __*/
	else
	  {
	     /* Il n'y a pas de menu parent */
	     catalogue->Cat_PtParent = NULL;
	     catalogue->Cat_EntryParent = 0;
	  }			/*else */

	/* Cree la fenetre du formulaire */
	if (parent != 0)
	   w = parent;
	else if (MainShell == 0)
	   w = PopShell;
	else
	   w = MainShell;

	n = 0;
#       ifndef _WINDOWS
	XtSetArg (args[n], XmNfontList, DefaultFont);
	n++;
	if (w == PopShell)
	  form = PopShell;
	else
	  form = XtCreateWidget (title, topLevelShellWidgetClass, w, args, n);
	XtAddCallback (form, XmNdestroyCallback, (XtCallbackProc) formKill, catalogue);
#       endif /* !_WINDOWS */

#       ifndef _WINDOWS
/*** Cree le formulaire dans sa frame ***/
	title_string = XmStringCreateSimple (title);
	XtSetArg (args[n], XmNdialogTitle, title_string);
	n++;
	XtSetArg (args[n], XmNautoUnmanage, FALSE);
	n++;
	XtSetArg (args[n], XmNdefaultPosition, FALSE);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	form = XmCreateBulletinBoard (form, "", args, n);
	XtAddCallback (XtParent (form), XmNpopdownCallback, (XtCallbackProc) CallSheet, catalogue);

	XmStringFree (title_string);
#       endif /* _WINDOWS */
	catalogue->Cat_Ref = ref;
	catalogue->Cat_Type = cattype;
	catalogue->Cat_Button = button;
	catalogue->Cat_FormPack = package + 1;
	catalogue->Cat_Widget = form;

	/* Cree le contenu initial du feuillet */
	adbloc = NewEList ();	/* Un bloc supplementaire pour les boutons */
	catalogue->Cat_Entries = adbloc;
	adbloc->E_Next = NewEList ();
	adbloc = adbloc->E_Next;

#       ifndef _WINDOWS
	/*** Cree un Row-Column pour mettre les boutons QUIT/... ***/
	/*** en dessous des sous-menus et sous-formulaires.    ***/
	n = 0;
	XtSetArg (args[n], XmNadjustLast, FALSE);
	n++;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNspacing, 0);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	row = XmCreateRowColumn (form, "Dialogue", args, n);
	XtManageChild (row);

	/*** Cree un Row-Column pour contenir les lignes ou colonnes ***/
	/*** de sous-menus et sous-formulaires.                      ***/
	n = 0;
	XtSetArg (args[n], XmNadjustLast, FALSE);
	n++;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNspacing, 2);
	n++;
	if (horizontal)
	   XtSetArg (args[n], XmNorientation, XmVERTICAL);
	else
	   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	w = XmCreateRowColumn (row, "Dialogue", args, n);
	XtManageChild (w);

	/*** Cree un 1er Row-Column pour contenir les sous-menus/formulaires ***/
	n = 0;
	XtSetArg (args[n], XmNadjustLast, FALSE);
	n++;
	catalogue->Cat_in_lines = (int) horizontal;
	if (horizontal)
	   XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	else
	   XtSetArg (args[n], XmNorientation, XmVERTICAL);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	w = XmCreateRowColumn (w, "Dialogue", args, n);
	XtManageChild (w);
	adbloc->E_ThotWidget[0] = w;
	adbloc->E_Free[0] = TEXT('X');

	if (number < 0)
	   /* il n'y a pas de boutons a engendrer */
	   return;

	/*** Cree un Row-Column pour contenir les boutons QUIT/... ***/
	n = 0;
	XtSetArg (args[n], XmNadjustLast, FALSE);
	n++;
	XtSetArg (args[n], XmNorientation, XmHORIZONTAL);
	n++;
	XtSetArg (args[n], XmNpacking, XmPACK_TIGHT/*XmPACK_COLUMN*/);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNforeground, FgMenu_Color);
	n++;
	XtSetArg (args[n], XmNspacing, 5);
	n++;
	XtSetArg (args[n], XmNnumColumns, /*(number + 4) / 4*/number);
	n++;
	row = XmCreateRowColumn (row, "Dialogue", args, n);
	XtManageChild (row);

	/*** Cree les boutons ***/
	n = 0;
	XtSetArg (args[n], XmNfontList, formFONT);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNforeground, FgMenu_Color);
	n++;
#       endif /* !_WINDOWS */
	adbloc = catalogue->Cat_Entries;

	if (cattype == CAT_SHEET)
	   ent = 1;
	else if (cattype == CAT_FORM)
	  {
	    /*** Cree le bouton de confirmation du formulaire ***/
	     ent = 1;
#            ifndef _WINDOWS 
	     w = XmCreatePushButton (row, TtaGetMessage (LIB, TMSG_LIB_CONFIRM), args, n);
	     XtManageChild (w);
	     XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallSheet, catalogue);
	     /* Definit le bouton de confirmation comme bouton par defaut */
	     XtSetArg (argform[0], XmNdefaultButton, w);
	     XtSetValues (form, argform, 1);
	     adbloc->E_ThotWidget[1] = w;
	     /* Definit le bouton de confirmation comme bouton par defaut */
	     XtSetArg (argform[0], XmNdefaultButton, w);
	     XtSetValues (form, argform, 1);
#            else  /* _WINDOWS */
             strSize = ustrlen (TtaGetMessage (LIB, TMSG_LIB_CONFIRM)) * charWidth + 20;
             formulary.Buttons[bIndex] = CreateWindow (TEXT("BUTTON"), TtaGetMessage (LIB, TMSG_LIB_CONFIRM), 
                                                       WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                                       bAbsBase, 300, strSize, 20, parent, 
                                                       (HMENU) IDCANCEL, hInstance, NULL) ;
             bAbsBase += (strSize + 20);
             bIndex++ ;
	     adbloc->E_ThotWidget[1] = (ThotWidget) IDCANCEL;
#            endif /* _WINDOWS */
	  }
	else
	   ent = 0;

	/*** Cree les autres boutons du feuillet ***/
	index = 0;
	while (ent < C_NUMBER && ent <= number && text != NULL)
	  {
	     count = ustrlen (&text[index]);	/* Longueur de l'intitule */
	     /* S'il n'y a plus d'intitule -> on arrete */
	     if (count == 0)
		ent = number;
	     else
	       {
		  if (ent == 0)
		     /* Le premier bouton remplace le bouton QUIT */
		     ptr = &text[index];
		  else
		    {
#                      ifndef _WINDOWS
		       w = XmCreatePushButton (row, &text[index], args, n);
		       XtManageChild (w);
		       XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallSheet, catalogue);
		       adbloc->E_ThotWidget[ent] = w;
#                      else  /* _WINDOWS */
                       strSize = ustrlen (&text[index]) * charWidth + 10;
                       formulary.Buttons[bIndex] = CreateWindow ((LPCTSTR)"BUTTON", (LPCTSTR)(&text[index]), 
                                                                 WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                                                 bAbsBase, 300, strSize, 20, (HWND)parent, 
                                                                 (HMENU)(ref + bIndex), (HINSTANCE)hInstance, (LPVOID)NULL) ;
                       bAbsBase += (strSize + 10);
                       bIndex++ ;
	               adbloc->E_ThotWidget[ent] = (ThotWidget) bIndex;
#                      endif /* _WINDOWS */
		    }
#                 ifndef _WINDOWS
		  /* Definit le bouton de confirmation comme bouton par defaut */
		  if (index == 0)
		     XtSetArg (argform[0], XmNdefaultButton, w);
		  XtSetValues (form, argform, 1);
#                 endif  /* !_WINDOWS */
	       }
	     index += count + 1;
	     ent++;
	  }

	if (cattype == CAT_SHEET || cattype == CAT_FORM)
	  /*** Cree le bouton QUIT ***/
	   switch (dbutton)
		 {
		    case D_CANCEL:
#                      ifndef _WINDOWS
		       w = XmCreatePushButton (row, TtaGetMessage (LIB, TMSG_CANCEL), args, n);
#                      else  /* _WINDOWS */
                       strSize = ustrlen (TtaGetMessage (LIB, TMSG_CANCEL)) * charWidth + 10;
                       formulary.Buttons[bIndex] = CreateWindow (TEXT("BUTTON"), TtaGetMessage (LIB, TMSG_CANCEL), 
                                                                 WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                                                 bAbsBase, 300, strSize, 20, parent, 
                                                                 (HMENU) IDCANCEL, hInstance, NULL) ;
	               adbloc->E_ThotWidget[ent] = (ThotWidget) IDCANCEL;
                       bAbsBase += (strSize + 10);
#                      endif /* _WINDOWS */
		       break;
		    case D_DONE:
#                      ifndef _WINDOWS
		       w = XmCreatePushButton (row, TtaGetMessage (LIB, TMSG_DONE), args, n);
#                      else  /* _WINDOWS */
                       strSize = ustrlen (TtaGetMessage (LIB, TMSG_DONE)) * charWidth + 10;
                       formulary.Buttons[bIndex] = CreateWindow (TEXT("BUTTON"), TtaGetMessage (LIB, TMSG_DONE), 
                                                                 WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                                                 bAbsBase, 300, strSize, 20, parent, 
                                                                 (HMENU) IDCANCEL, hInstance, NULL) ;
	               adbloc->E_ThotWidget[ent] = (ThotWidget) IDCANCEL;
                       bAbsBase += (strSize + 10);
#                      endif /* _WINDOWS */
		       break;
		 }
	else
#          ifndef _WINDOWS
	   w = XmCreatePushButton (row, ptr, args, n);
#          else  /* _WINDOWS */
	  {
             strSize = ustrlen (ptr) * charWidth + 10;
             formulary.Buttons[bIndex] = CreateWindow (TEXT("BUTTON"), ptr, 
                                                       WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
                                                       bAbsBase, 300, strSize, 20, parent, 
                                                       (HMENU) (ref + bIndex), hInstance, NULL) ;
             bAbsBase += (strSize + 10);
	     adbloc->E_ThotWidget[ent] = (ThotWidget) bIndex;
	  } 
          bIndex++ ;
#          endif /* _WINDOWS */

#       ifndef _WINDOWS
	XtManageChild (w);
	XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallSheet, catalogue);
#       endif /* !_WINDOWS */
	/* Range le bouton dans le 1er bloc de widgets */
	adbloc->E_ThotWidget[0] = w;
#       ifdef _WINDOWS
        form = CreateWindow (_WNDIALOGBOXCST_, title, DS_MODALFRAME | WS_POPUP | 
                             WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
	  		     CW_USEDEFAULT, CW_USEDEFAULT, bAbsBase + 20, 400,
			     w, 0, hInstance, 0); 

#   ifdef AMAYA_DEBUG
	fprintf (stderr, "Created ComboBox %X\n", form);
#   endif /* AMAYA_DEBUG */

        catalogue->Cat_Widget = form;
        copyCat = catalogue;
        WIN_AddFrameCatalogue (parent, copyCat) ;
        bIndex   =  0;
        bAbsBase = 60;
#       endif /* _WINDOWS */
     }
#endif /* _GTK */
}



/*----------------------------------------------------------------------
   TtaNewForm cre'e un formulaire :                                   
   Le parame`tre ref donne la reference et parent le ThotWidget pe're 
   Si parent est nul, le menu est attache a la fenetre MainWindow.          
   Le parame'tre title donne le titre du catalogue.            
   Le parame`tre horizontal indique que le formulaire est compose' en 
   lignes (TRUE) ou en colonnes (FALSE).                              
   Le parame`tre package donne le facteur de blocage du formulaire    
   (nombre d'entre'es par ligne ou par colonne).                      
   Le parame'tre button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewForm (int ref, ThotWidget *parent, STRING title, ThotBool horizontal, int package, CHAR_T button, int dbutton)
#else  /* __STDC__ */
void                TtaNewForm (ref, parent, title, horizontal, package, button, dbutton)
int                 ref;
ThotWidget         * parent;
STRING              title;
ThotBool            horizontal;
int                 package;
CHAR_T                button;
int                 dbutton;

#endif /* __STDC__ */
{
#ifndef _GTK
   NewSheet (ref, parent, title, 0, NULL, horizontal, package, button, dbutton, CAT_FORM);
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaNewSheet cre'e un feuillet de commande :                        
   Le parame`tre ref donne la reference et parent le ThotWidget pe're 
   Si parent est nul, le menu est attache a la fenetre MainWindow.          
   Le parame`tre title donne le titre du catalogue.            
   Le parame`tre number indique le nombre de boutons ajoute's au      
   bouton 'QUIT' mis par de'faut.                                     
   Le parame`tre text contient la liste des intitule's des boutons    
   ajoute's au bouton 'QUIT'.                                         
   Chaque intitule' se termine par le caracte`re de fin de chai^ne \0.
   Le parame`tre horizontal indique que le formulaire est compose' en 
   lignes (TRUE) ou en colonnes (FALSE).                              
   Le parame`tre package donne le facteur de blocage du formulaire    
   (nombre d'entre'es par ligne ou par colonne).                      
   Le parame`tre button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewSheet (int ref, ThotWidget parent, STRING title, int number, STRING text, ThotBool horizontal, int package, CHAR_T button, int dbutton)
#else  /* __STDC__ */
void                TtaNewSheet (ref, parent, title, number, text, horizontal, package, button, dbutton)
int                 ref;
ThotWidget          parent;
STRING              title;
int                 number;
STRING              text;
ThotBool            horizontal;
int                 package;
CHAR_T                button;
int                 dbutton;

#endif /* __STDC__ */
{
#ifndef _GTK
   NewSheet (ref, parent, title, number, text, horizontal, package, button, dbutton, CAT_SHEET);
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaNewDialogSheet cre'e un feuillet de dialogue :                  
   Le parame`tre ref donne la reference et parent le ThotWidget pe're 
   Si parent est nul, le menu est attache a la fenetre MainWindow.          
   Le parame`tre title donne le titre du catalogue.            
   Le parame`tre number indique le nombre de boutons ajoute's au      
   bouton 'QUIT' mis par de'faut.                                     
   Le parame`tre text contient la liste des intitule's des boutons    
   ajoute's au bouton 'QUIT'.                                         
   Chaque intitule' se termine par le caracte`re de fin de chai^ne \0.
   Le parame`tre horizontal indique que le formulaire est compose' en 
   lignes (TRUE) ou en colonnes (FALSE).                              
   Le parame`tre package donne le facteur de blocage du formulaire    
   (nombre d'entre'es par ligne ou par colonne).                      
   Le parame`tre button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewDialogSheet (int ref, ThotWidget parent, STRING title, int number, STRING text, ThotBool horizontal, int package, CHAR_T button)

#else  /* __STDC__ */
void                TtaNewDialogSheet (ref, parent, title, number, text, horizontal, package, button)
int                 ref;
ThotWidget          parent;
STRING              title;
int                 number;
STRING              text;
ThotBool            horizontal;
int                 package;
CHAR_T                button;

#endif /* __STDC__ */
{
#ifndef _GTK
   NewSheet (ref, parent, title, number - 1, text, horizontal, package, button, D_DONE, CAT_DIALOG);
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaAttachForm attache le catalogue au formulaire ou au feuillet    
   dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaAttachForm (int ref)
#else  /* __STDC__ */
void                TtaAttachForm (ref)
int                 ref;

#endif /* __STDC__ */
{
#ifndef _GTK
   int                 entry;
   struct E_List      *adbloc;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else if ((catalogue->Cat_Type != CAT_FMENU)
	    && (catalogue->Cat_Type != CAT_TMENU)
	    && (catalogue->Cat_Type != CAT_TEXT)
	    && (catalogue->Cat_Type != CAT_LABEL)
	    && (catalogue->Cat_Type != CAT_INT)
	    && (catalogue->Cat_Type != CAT_SELECT))
      TtaError (ERR_invalid_reference);
   else
     {
	/* Attache le catalogue au formulaire designe */
	parentCatalogue = catalogue->Cat_PtParent;
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }

	/* Recherche l'entree du menu qui lui correspond */
	adbloc = parentCatalogue->Cat_Entries;
	entry = catalogue->Cat_EntryParent;

	/* Saute au bloc qui contient l'entree recherchee */
	while (entry >= C_NUMBER)
	  {
	     if (adbloc->E_Next == NULL)
	       {
		  TtaError (ERR_invalid_parameter);
		  return;
	       }
	     else
		adbloc = adbloc->E_Next;
	     entry -= C_NUMBER;
	  }

	if (adbloc->E_Free[entry] == TEXT('Y'))
	  {
	     /* marque que le sous-menu est attache */
	     adbloc->E_Free[entry] = TEXT('N');
	     /* affiche le widget sur l'ecran */
#            ifndef _WINDOWS
	     if (XtIsManaged (parentCatalogue->Cat_Widget))
		XtManageChild (catalogue->Cat_Widget);
#            endif /* _WINDOWS */
	  }
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaDetachForm detache le catalogue au formulaire ou au feuillet    
   dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaDetachForm (int ref)
#else  /* __STDC__ */
void                TtaDetachForm (ref)
int                 ref;

#endif /* __STDC__ */
{
#ifndef _GTK
   int                 entry;
   struct E_List      *adbloc;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else if ((catalogue->Cat_Type != CAT_FMENU)
	    && (catalogue->Cat_Type != CAT_TMENU)
	    && (catalogue->Cat_Type != CAT_TEXT)
	    && (catalogue->Cat_Type != CAT_LABEL)
	    && (catalogue->Cat_Type != CAT_INT)
	    && (catalogue->Cat_Type != CAT_SELECT))
      TtaError (ERR_invalid_reference);
   else
     {
	/* Detache le catalogue du formulaire designe */
	parentCatalogue = catalogue->Cat_PtParent;
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }

	/* Recherche l'entree du menu qui lui correspond */
	adbloc = parentCatalogue->Cat_Entries;
	entry = catalogue->Cat_EntryParent;

	/* Saute au bloc qui contient l'entree recherchee */
	while (entry >= C_NUMBER)
	  {
	     if (adbloc->E_Next == NULL)
	       {
		  TtaError (ERR_invalid_parameter);
		  return;
	       }
	     else
		adbloc = adbloc->E_Next;
	     entry -= C_NUMBER;
	  }

	if (adbloc->E_Free[entry] == TEXT('N'))
	  {
	     /* marque que le sous-menu est detache */
	     adbloc->E_Free[entry] = TEXT('Y');
	     /* retire le widget de l'ecran */
#            ifndef _WINDOWS
	     if (XtIsManaged (catalogue->Cat_Widget))
		XtUnmanageChild (catalogue->Cat_Widget);
#            endif /* _WINDOWS */
	  }

     }
#endif _GTK
}


/*----------------------------------------------------------------------
   TtaNewSelector cre'e un se'lecteur dans un formulaire :            
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre title donne le titre du catalogue.                   
   Le parame`tre number donne le nombre d'intitule's.                 
   Le parame`tre text contient la liste des intitule's.               
   Le parame`tre height donne le nombre d'intitule's visibles a` la   
   fois (hauteur de la fenetree^tre de visualisation).                        
   Le parame`tre label ajoute un choix supple'mentaire a` la liste    
   des choix possibles. Ce choix est affiche' se'pare'ment et donc    
   mis en e'vidence.                                                  
   Le parame`tre withText indique s'il faut creer une zone texte.     
   Quand le parame`tre react est vrai, tout changement de se'lection  
   dans le se'lecteur est imme'diatement signale' a` l'application.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewSelector (int ref, int ref_parent, STRING title, int number, STRING text, int height, STRING label, ThotBool withText, ThotBool react)
#else  /* __STDC__ */
void                TtaNewSelector (ref, ref_parent, title, number, text, height, label, withText, react)
int                 ref;
int                 ref_parent;
STRING              title;
int                 number;
STRING              text;
int                 height;
STRING              label;
ThotBool            withText;
ThotBool            react;

#endif /* __STDC__ */
{
#ifndef _GTK
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;

#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
   ThotWidget          wt;
   int                 ent;
   int                 index;
   int                 i;
   int                 n;
   ThotWidget          row;
   ThotWidget          w;
   struct E_List      *adbloc;
   XmString            title_string;
   XmString           *item;
#  endif /* _WINDOWS */

   ThotBool            rebuilded;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   rebuilded = FALSE;
   if (catalogue == NULL)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   /* Faut-il detruire le catalogue precedent ? */
   else if (catalogue->Cat_Widget != 0)
     {
	/* Si c'est une mise a jour du selecteur */
	if (catalogue->Cat_Type == CAT_SELECT && (withText != catalogue->Cat_SelectList))
	   rebuilded = TRUE;
	else
	   TtaDestroyDialogue (ref);	/* Modification du catalogue */
     }

   /*______________________________ Regarde si le catalogue parent existe __*/
   if (!rebuilded)
     {
	parentCatalogue = CatEntry (ref_parent);
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (parentCatalogue->Cat_Widget == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if ((parentCatalogue->Cat_Type != CAT_FORM)
		 && (parentCatalogue->Cat_Type != CAT_SHEET)
		 && (parentCatalogue->Cat_Type != CAT_DIALOG))
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	catalogue->Cat_PtParent = parentCatalogue;
     }
   else
      parentCatalogue = catalogue->Cat_PtParent;

   /* Avec ou sans zone texte */
   catalogue->Cat_SelectList = !withText;
   catalogue->Cat_React = react;

#  ifndef _WINDOWS
   if (number == 0)
     {
	/* Cree un selecteur avec une entree a blanc */
	item = (XmString *) TtaGetMemory (sizeof (XmString) * 2);
	item[0] = XmStringCreateLtoR (" ", XmSTRING_DEFAULT_CHARSET);
	number = 1;
     }
   else
     {
	/* Cree les differentes entrees du selecteur */
	item = (XmString *) TtaGetMemory (sizeof (XmString) * (number + 1));
	i = 0;
	index = 0;
	while (i < number && text[index] != EOS)
	  {
	     item[i++] = XmStringCreateLtoR (&text[index], XmSTRING_DEFAULT_CHARSET);
	     index += ustrlen (&text[index]) + 1;	/* Longueur de l'intitule */
	  }
	number = i;
     }
   item[number] = NULL;

   /* Faut-il simplement mettre a jour le selecteur ? */
   if (rebuilded)
     {

	/* On met a jour le titre du selecteur */
	if (catalogue->Cat_Title != 0 && title != NULL)
	  {
	     n = 0;
	     title_string = XmStringCreateSimple (title);
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetValues (catalogue->Cat_Title, args, n);
	     XtManageChild (catalogue->Cat_Title);
	     XmStringFree (title_string);
	  }

	/* On met a jour le label attache au selecteur */
	if (catalogue->Cat_SelectLabel != 0 && label != NULL)
	  {
	     n = 0;
	     title_string = XmStringCreateSimple (label);
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetValues (catalogue->Cat_SelectLabel, args, n);
	     XtManageChild (catalogue->Cat_SelectLabel);
	     XmStringFree (title_string);
	  }

	/* On met a jour le selecteur (catalogue->Cat_Entries) */
	catalogue->Cat_ListLength = number;
	n = 0;
	w = (ThotWidget) catalogue->Cat_Entries;
	if (catalogue->Cat_SelectList)
	  {
	     /* Une simple liste */
	     XtSetArg (args[n], XmNitems, item);
	     n++;
	     XtSetArg (args[n], XmNitemCount, number);
	     n++;

	     /* Detruit l'ancienne liste */
	     XmListDeleteAllItems (w);
	     /* Remplace par la nouvelle liste */
	     XtSetValues (w, args, n);
	  }
	else
	  {
	     /* Un selecteur */
	     XtSetArg (args[n], XmNlistItems, item);
	     n++;
	     XtSetArg (args[n], XmNlistItemCount, number);
	     n++;
	     /*XtSetArg(args[n], XmNlistVisibleItemCount, height); n++; */
	     title_string = XmStringCreateSimple ("");
	     XtSetArg (args[n], XmNselectionLabelString, title_string);
	     n++;

	     /* Detruit l'ancienne liste */
	     wt = XmSelectionBoxGetChild (w, XmDIALOG_LIST);
	     XmListDeleteAllItems (wt);
	     /* Remplace par la nouvelle liste */
	     XtSetValues (w, args, n);
	     XmStringFree (title_string);
	  }
	XtManageChild (w);
     }
   else
     {
       /*_______________________________________ C'est un nouveau formulaire __*/
	w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);

	/* Cree un sous-menu d'un formulaire */
	/*** Cree un Row-Column dans le Row-Column du formulaire ***/
	n = 0;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNspacing, 0);
	n++;
	row = XmCreateRowColumn (w, "Dialogue", args, n);

	catalogue->Cat_Ref = ref;
	catalogue->Cat_Type = CAT_SELECT;
	catalogue->Cat_ListLength = number;
	catalogue->Cat_Widget = row;
	adbloc->E_ThotWidget[ent] = (ThotWidget) catalogue;
	adbloc->E_Free[ent] = TEXT('N');
	catalogue->Cat_EntryParent = i;
	catalogue->Cat_Title = 0;

	/*** Cree le titre du selecteur ***/
	if (title != NULL)
	  {
	     n = 0;
	     title_string = XmStringCreateSimple (title);
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     w = XmCreateLabel (row, "Dialogue", args, n);
	     XtManageChild (w);
	     catalogue->Cat_Title = w;
	     XmStringFree (title_string);
	  }

	/*** Cree le label attache au selecteur ***/
	if (label != NULL)
	  {
	     n = 0;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     w = XmCreatePushButton (row, label, args, n);
	     XtManageChild (w);
	     catalogue->Cat_SelectLabel = w;
	     XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) CallLabel, catalogue);
	  }
	else
	   catalogue->Cat_SelectLabel = 0;

	n = 0;
	XtSetArg (args[n], XmNmarginWidth, 0);
	n++;
	XtSetArg (args[n], XmNmarginHeight, 0);
	n++;
	XtSetArg (args[n], XmNspacing, 0);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNforeground, FgMenu_Color);
	n++;
	XtSetArg (args[n], XmNfontList, DefaultFont);
	n++;
	if (catalogue->Cat_SelectList)
	  {
	     /* Une simple liste */
	     XtSetArg (args[n], XmNselectionPolicy, XmSINGLE_SELECT);
	     n++;
	     XtSetArg (args[n], XmNlistSizePolicy, XmVARIABLE);
	     n++;
	     XtSetArg (args[n], XmNitems, item);
	     n++;
	     XtSetArg (args[n], XmNitemCount, number);
	     n++;
	     XtSetArg (args[n], XmNvisibleItemCount, height);
	     n++;
	     w = XmCreateScrolledList (row, title, args, n);
	     XtManageChild (w);
	     /* Si le selecteur est reactif */
	     if (react)
		XtAddCallback (w, XmNsingleSelectionCallback, (XtCallbackProc) CallList, catalogue);
	  }
	else
	  {
	     /* Un selecteur */
	     XtSetArg (args[n], XmNlistItems, item);
	     n++;
	     XtSetArg (args[n], XmNlistItemCount, number);
	     n++;
	     XtSetArg (args[n], XmNlistVisibleItemCount, height);
	     n++;
	     XtSetArg (args[n], XmNdialogType, XmDIALOG_COMMAND);
	     n++;
	     if (TextTranslations != NULL)
	       {
		  XtSetArg (args[n], XmNtextTranslations, TextTranslations);
		  n++;
	       }
	     title_string = XmStringCreateSimple ("");
	     XtSetArg (args[n], XmNselectionLabelString, title_string);
	     n++;
	     w = XmCreateSelectionBox (row, title, args, n);
	     XtManageChild (w);
	     XmStringFree (title_string);

	     /* Controle la couleur des widgets fils */
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNtroughColor, BgMenu_Color);
	     n++;
	     wt = XmSelectionBoxGetChild (w, XmDIALOG_LIST);
	     XtSetValues (wt, args, n);
	     wt = XmSelectionBoxGetChild (w, XmDIALOG_TEXT);
	     XtSetValues (wt, args, n - 1);
	     /* Si le selecteur est reactif */
	     if (react)
		XtAddCallback (wt, XmNvalueChangedCallback, (XtCallbackProc) CallTextChange, catalogue);
	  }

	/* Conserve le widget du selecteur dans l'entree Cat_Entries */
	catalogue->Cat_Entries = (struct E_List *) w;
     }

   /* Libere les XmString allouees */
   i = 0;
   while (item[i] != NULL)
     {
	XmStringFree (item[i]);
	i++;
     }
   TtaFreeMemory ( item);
#  endif /* _WINDOWS */
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaActiveSelector rend actif le  se'lecteur.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaActiveSelector (int ref)
#else  /* __STDC__ */
void                TtaActiveSelector (ref)
int                 ref;

#endif /* __STDC__ */
{
#ifndef _GTK
   ThotWidget          w;
   struct Cat_Context *catalogue;

#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
#  endif /* _WINDOWS */

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Type != CAT_SELECT)
      TtaError (ERR_invalid_reference);
   else
     {
	/* Recupere le widget du selecteur */
	w = (ThotWidget) catalogue->Cat_Entries;
#       ifndef _WINDOWS
	XtSetArg (args[0], XmNsensitive, TRUE);
	XtSetValues (w, args, 1);
	XtManageChild (w);
#       endif /* _WINDOWS */
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaDesactiveSelector rend non actif le  se'lecteur.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaDesactiveSelector (int ref)
#else  /* __STDC__ */
void                TtaDesactiveSelector (ref)
int                 ref;

#endif /* __STDC__ */
{
#ifndef _GTK 
   ThotWidget          w;
   struct Cat_Context *catalogue;

#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
#  endif /* _WINDOWS */


   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Type != CAT_SELECT)
      TtaError (ERR_invalid_reference);
   else
     {
	/* Recupere le widget du selecteur */
	w = (ThotWidget) catalogue->Cat_Entries;
#       ifndef _WINDOWS
	XtSetArg (args[0], XmNsensitive, FALSE);
	XtSetValues (w, args, 1);
	XtManageChild (w);
#       endif /* _WINDOWS */
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaSetSelector initialise l'entre'e et/ou le texte du se'lecteur : 
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre entry positif ou nul donne l'index de l'entre'e      
   se'lectionne'e.                                                    
   Le parame`tre text donne le texte si entry vaut -1.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetSelector (int ref, int entry, STRING text)
#else  /* __STDC__ */
void                TtaSetSelector (ref, entry, text)
int                 ref;
int                 entry;
STRING              text;

#endif /* __STDC__ */
{
#ifndef _GTK
#  ifndef _WINDOWS
   ThotWidget          w;
#  endif /* !_WINDOWS */
   ThotWidget          wt;
   ThotWidget          select;
   struct Cat_Context *catalogue;

   catalogue = CatEntry (ref);
   wt = 0;
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget != 0)
     {
	/* Recupere le widget du selecteur */
	select = (ThotWidget) catalogue->Cat_Entries;
	if (catalogue->Cat_Type != CAT_SELECT)
	  {
	     TtaError (ERR_invalid_reference);
	     return;
	  }

#       ifndef _WINDOWS
	if (!catalogue->Cat_SelectList)
	  {
	     wt = XmSelectionBoxGetChild (select, XmDIALOG_TEXT);
	     /* Si le selecteur est reactif */
	     if (catalogue->Cat_React)
		XtRemoveCallback (wt, XmNvalueChangedCallback, (XtCallbackProc) CallTextChange, catalogue);
	  }
	else if (catalogue->Cat_React)
	   XtRemoveCallback (select, XmNsingleSelectionCallback, (XtCallbackProc) CallList, catalogue);

	if (entry >= 0 && entry < catalogue->Cat_ListLength)
	  {
	     /* Initialise l'entree de la liste */
	     if (catalogue->Cat_SelectList)
		XmListSelectPos (select, entry + 1, TRUE);
	     else
	       {
		  w = XmSelectionBoxGetChild (select, XmDIALOG_LIST);
		  XmListSelectPos (w, entry + 1, TRUE);
	       }
	  }
	else if (catalogue->Cat_SelectList)
	   XmListDeselectAllItems (select);
	else
	  {
	     /* Initialise le champ texte */
	     if (catalogue->Cat_ListLength != 0)
	       {
		  /* Deselectionne dans la liste */
		  w = XmSelectionBoxGetChild (select, XmDIALOG_LIST);
		  XmListDeselectAllItems (w);
	       }

	     XmTextSetString (wt, text);
      /*----------------------------------------------------------------------
   lg = ustrlen(text);
   XmTextSetSelection(wt, lg, lg, 500);
  ----------------------------------------------------------------------*/
	  }

	/* Si le selecteur est reactif */
	if (catalogue->Cat_React)
	   if (catalogue->Cat_SelectList)
	      XtAddCallback (select, XmNsingleSelectionCallback, (XtCallbackProc) CallList, catalogue);
	   else
	      XtAddCallback (wt, XmNvalueChangedCallback, (XtCallbackProc) CallTextChange, catalogue);
#       endif /* _WINDOWS */
     }
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   TtaNewLabel cre'e un intitule' constant dans un formulaire :       
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre text donne l'intitule'.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewLabel (int ref, int ref_parent, STRING text)
#else  /* __STDC__ */
void                TtaNewLabel (ref, ref_parent, text)
int                 ref;
int                 ref_parent;
STRING              text;

#endif /* __STDC__ */
{
#ifndef _GTK
#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
   XmString            title_string;
   int                 n;
#  else  /* _WINDOWS */
   HDC         hdc ;
   RECT        rect ;
   PAINTSTRUCT ps ;
#  endif /* _WINDOWS */
   int                 i;
   int                 ent;
   int                 rebuilded;
   struct E_List      *adbloc;
   ThotWidget          w;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
#  ifndef _WINDOWS
   title_string = 0;
#  endif /* _WINDOWS */
   catalogue = CatEntry (ref);
   rebuilded = 0;
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else if (catalogue->Cat_Widget != 0 && catalogue->Cat_Type == CAT_LABEL)
     {
	/* Modification du catalogue */
	w = catalogue->Cat_Widget;
	/* Regarde si le widget est affiche */
#       ifndef _WINDOWS
	if (XtIsManaged (w))
#       else  /* _WINDOWS */
        if (IsWindowVisible (w))
#       endif /* WINDOWS */
	   rebuilded = 2;
	else
	   rebuilded = 1;
#       ifndef _WINDOWS
	n = 0;
	title_string = XmStringCreateSimple (text);
	XtSetArg (args[n], XmNlabelString, title_string);
	n++;
	XtSetValues (w, args, n);
#       else   /* _WINDOWS */
        hdc = BeginPaint (w, &ps);
        GetClientRect (w, &rect);
        DrawText (hdc, text, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        EndPaint (w, &ps);
#       endif 
	/* Faut-il reafficher le widget ? */
	if (rebuilded == 2)
#          ifndef _WINDOWS
	   XtManageChild (w);
#          else  /* _WINDOWS */
           UpdateWindow (w);
#          endif /* _WINDOWS */   
     }
   else
     {
	if (catalogue->Cat_Widget != 0)
	   /* Le catalogue est a reconstruire completement */
	   TtaDestroyDialogue (ref);

/*======================================> Recherche le catalogue parent */
	parentCatalogue = CatEntry (ref_parent);
/*__________________________________ Le catalogue parent n'existe pas __*/
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (parentCatalogue->Cat_Widget == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
/*_________________________________________ Sous-menu d'un formulaire __*/
	else if ((parentCatalogue->Cat_Type != CAT_FORM)
		 && (parentCatalogue->Cat_Type != CAT_SHEET)
		 && (parentCatalogue->Cat_Type != CAT_DIALOG))
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (text == NULL)
	  {
	     TtaError (ERR_invalid_parameter);
	     return;
	  }

	/* Recupere le widget parent */
	w = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);

/*** Cree l'intitule ***/
#       ifndef _WINDOWS
	n = 0;
	title_string = XmStringCreateSimple (text);
	XtSetArg (args[n], XmNfontList, DefaultFont);
	n++;
	XtSetArg (args[n], XmNlabelString, title_string);
	n++;
	XtSetArg (args[n], XmNbackground, BgMenu_Color);
	n++;
	XtSetArg (args[n], XmNforeground, FgMenu_Color);
	n++;
	/*XtSetArg (args[n], XmNborderColor, BgMenu_Color);
	n++;*/
	w = XmCreateLabel (w, "Dialogue", args, n);
#       else  /* _WINDOWS */
	if (!isOnlyBlank (text)) {
	   w = CreateWindow (TEXT("STATIC"), text, WS_CHILD | WS_VISIBLE | SS_LEFT, 10, cyValue, 100, 30, 
			     parentCatalogue->Cat_Widget, (HMENU) ref, hInstance, NULL);
	   cyValue += 40;
	}
#       endif /* _WINDOWS */

	catalogue->Cat_Widget = w;
	catalogue->Cat_Ref = ref;
	catalogue->Cat_Type = CAT_LABEL;
	catalogue->Cat_PtParent = parentCatalogue;
	adbloc->E_ThotWidget[ent] = (ThotWidget) (catalogue);
	adbloc->E_Free[ent] = TEXT('N');
	catalogue->Cat_EntryParent = i;
	catalogue->Cat_Entries = NULL;
     }
#  ifndef _WINDOWS
   XmStringFree (title_string);
#  endif /* _WINDOWS */
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   TtaNewTextForm cre'e une feuille de saisie de texte :              
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre title donne le titre du catalogue.                   
   Les parame`tres width et height indiquent la taille en caracte`ree 
   de la feuille de saisie de texte.                                  
   Quand le parame`tre react est vrai, tout changement dans la        
   feuille de saisie est imme'diatement signale' a` l'application.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewTextForm (int ref, int ref_parent, STRING title, int width, int height, ThotBool react)
#else  /* __STDC__ */
void                TtaNewTextForm (ref, ref_parent, title, width, height, react)
int                 ref;
int                 ref_parent;
STRING              title;
int                 width;
int                 height;
ThotBool            react;

#endif /* __STDC__ */
{
#ifndef _GTK
   int                 ent;
   int                 i;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   struct E_List      *adbloc;
   ThotWidget          w;
   ThotWidget          row;
#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
   XmString            title_string;
   int                 n;
#  endif  /* !_WINDOWS */

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else
     {
	catalogue->Cat_React = react;

	/* Faut-il detruire le catalogue precedent ? */
	if (catalogue->Cat_Widget != 0)
	   DestForm (ref);	/* Modification du catalogue */

	/*======================================> Recherche le catalogue parent */
	parentCatalogue = CatEntry (ref_parent);
	/*__________________________________ Le catalogue parent n'existe pas __*/
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (parentCatalogue->Cat_Widget == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_______________________ Le catalogue parent n'est pas un formulaire __*/
	else if ((parentCatalogue->Cat_Type != CAT_FORM)
		 && (parentCatalogue->Cat_Type != CAT_SHEET)
		 && (parentCatalogue->Cat_Type != CAT_DIALOG))
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_____________________________________________________________ Sinon __*/
	else
	  {
	     /* Cree a l'interieur Row-Column du formulaire */
	     row = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
#            ifndef _WINDOWS
	     row = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     row = XmCreateRowColumn (row, "Dialogue", args, n);

	     /*** Cree le titre du sous-menu ***/
	     if (title != NULL)
	       {
		  n = 0;
		  title_string = XmStringCreateSimple (title);
		  XtSetArg (args[n], XmNfontList, DefaultFont);
		  n++;
		  XtSetArg (args[n], XmNlabelString, title_string);
		  n++;
		  XtSetArg (args[n], XmNmarginWidth, 0);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNforeground, FgMenu_Color);
		  n++;
		  w = XmCreateLabel (row, "Dialogue", args, n);
		  XtManageChild (w);
		  XmStringFree (title_string);
	       }		/*if */

	     /* Cree une feuille de saisie de texte */
	     n = 0;
	     if (width == 0)
		XtSetArg (args[n], XmNcolumns, 10);
	     else
		XtSetArg (args[n], XmNcolumns, width);
	     n++;

	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     if (TextTranslations != NULL)
		XtSetArg (args[n], XmNtranslations, TextTranslations);
	     n++;
#            endif /* !_WINDOWS */

	     if (height < 2)
	       {
#                 ifndef _WINDOWS 
		  XtSetArg (args[n], XmNeditMode, XmSINGLE_LINE_EDIT);
		  n++;
		  /*XtSetArg(args[n], XmNscrollVertical, FALSE); n++; */
		  w = XmCreateText (row, "Dialogue", args, n);
#                 else  /* _WINDOWS */
		  w = CreateWindow (TEXT("EDIT"), _EMPTYSTR_, WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER, 
				    10, cyValue, 200, 30, parentCatalogue->Cat_Widget, (HMENU) ref, hInstance, NULL);
		  cyValue += 40;
#                 endif /* _WINDOWS */
	       }
	     else
	       {
#                 ifndef _WINDOWS 
		  XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);
		  n++;
		  XtSetArg (args[n], XmNrows, (short) height);
		  n++;
		  w = XmCreateScrolledText (row, "Dialogue", args, n);
#                 else  /* _WINDOWS */
		  /* Create a multi-line edit windows *
		   * TODO TODO TODO TODO TODO TODO    */
#                 endif /* !_WINDOWS */
	       }
#            ifndef _WINDOWS 
	     XtManageChild (w);
	     /* Si la feuille de saisie est reactive */
	     if (react)
		XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallTextChange, catalogue);
#            else  /* _WINDOWS */
	     ShowWindow (w, SW_SHOWNORMAL);
	     UpdateWindow (w);
#            endif /* _WINDOWS */
	     catalogue->Cat_Ref = ref;
	     catalogue->Cat_Type = CAT_TEXT;
	     /* L'entree Cat_Entries contient le numero du widget texte */
	     catalogue->Cat_Entries = (struct E_List *) w;
#            ifndef _WINDOWS
	     catalogue->Cat_Widget = row;
#            else  /* _WINDOWS */
	     catalogue->Cat_Widget = w;
#            endif /* _WINDOWS */
	     catalogue->Cat_PtParent = parentCatalogue;
	     adbloc->E_ThotWidget[ent] = (ThotWidget) catalogue;
	     adbloc->E_Free[ent] = TEXT('N');
	     catalogue->Cat_EntryParent = i;
	  }
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaSetTextForm initialise une feuille de saisie de texte :         
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre text donne la valeur initiale.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetTextForm (int ref, STRING text)
#else  /* __STDC__ */
void                TtaSetTextForm (ref, text)
int                 ref;
STRING              text;

#endif /* __STDC__ */
{
#ifndef _GTK
#  ifndef _WINDOWS
   int                 lg;
#  endif  /* !_WINDOWS */
   struct Cat_Context *catalogue;
   ThotWidget          w;

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else if (text == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Type != CAT_TEXT)
      TtaError (ERR_invalid_reference);
   else
     {
	w = (ThotWidget) catalogue->Cat_Entries;
	/* Si la feuille de saisie est reactive */
#       ifndef _WINDOWS
	if (catalogue->Cat_React)
	   XtRemoveCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallTextChange, catalogue);

	XmTextSetString (w, text);
	lg = ustrlen (text);
	XmTextSetSelection (w, lg, lg, 500);

	/* Si la feuille de saisie est reactive */
	if (catalogue->Cat_React)
	   XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallTextChange, catalogue);
#       else  /* _WINDOWS */
	SetWindowText (w, text) ;
#       endif /* _WINDOWS */
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaNewNumberForm cre'e une feuille de saisie de nombre :           
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre ref_parent identifie le formulaire pe`re.            
   Le parame`tre title donne le titre du catalogue.                   
   Les parame`tres min et max fixent les bornes valides du nombre.    
   Quand le parame`tre react est vrai, tout changement dans           
   la feuille de saisie est imme'diatement signale' a` l'application. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaNewNumberForm (int ref, int ref_parent, STRING title, int min, int max, ThotBool react)
#else  /* __STDC__ */
void                TtaNewNumberForm (ref, ref_parent, title, min, max, react)
int                 ref;
int                 ref_parent;
STRING              title;
int                 min;
int                 max;
ThotBool            react;

#endif /* __STDC__ */
{
#ifndef _GTK
#  ifndef _WINDOWS
   int                 ent;
   int                 n;
   int                 i;
   struct Cat_Context *catalogue;
   struct Cat_Context *parentCatalogue;
   struct E_List      *adbloc;
   Arg                 args[MAX_ARGS];
   ThotWidget          w;
   ThotWidget          row;
   XmString            title_string;
   CHAR_T                bounds[100];

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_cannot_create_dialogue);
   else
     {
	catalogue->Cat_React = react;

	/* Faut-il detruire le catalogue precedent ? */
	if (catalogue->Cat_Widget != 0)
	   DestForm (ref);	/* Modification du catalogue */

	/*======================================> Recherche le catalogue parent */
	parentCatalogue = CatEntry (ref_parent);
	/*__________________________________ Le catalogue parent n'existe pas __*/
	if (parentCatalogue == NULL)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	else if (parentCatalogue->Cat_Widget == 0)
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_______________________ Le catalogue parent n'est pas un formulaire __*/
	else if ((parentCatalogue->Cat_Type != CAT_FORM)
		 && (parentCatalogue->Cat_Type != CAT_SHEET)
		 && (parentCatalogue->Cat_Type != CAT_DIALOG))
	  {
	     TtaError (ERR_invalid_parent_dialogue);
	     return;
	  }
	/*_____________________________________________________________ Sinon __*/
	else
	  {
	     row = AddInFormulary (parentCatalogue, &i, &ent, &adbloc);

	     /* Cree a l'interieur Row-Column du formulaire */
	     n = 0;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     /*XtSetArg(args[n], XmNpacking, XmPACK_NONE); n++; */
	     XtSetArg (args[n], XmNadjustLast, FALSE);
	     n++;
	     XtSetArg (args[n], XmNmarginWidth, 0);
	     n++;
	     XtSetArg (args[n], XmNmarginHeight, 0);
	     n++;
	     XtSetArg (args[n], XmNspacing, 0);
	     n++;
	     row = XmCreateRowColumn (row, "Dialogue", args, n);
	     catalogue->Cat_Widget = row;
	     catalogue->Cat_PtParent = parentCatalogue;
	     adbloc->E_ThotWidget[ent] = (ThotWidget) catalogue;
	     adbloc->E_Free[ent] = TEXT('N');
	     catalogue->Cat_EntryParent = i;
	     catalogue->Cat_Ref = ref;
	     catalogue->Cat_Type = CAT_INT;

	     /* Alloue un bloc pour ranger les widgets scale et texte */
	     adbloc = NewEList ();
	     catalogue->Cat_Entries = adbloc;

	     /*** Cree le titre du sous-menu ***/
	     if (title != NULL)
	       {
		  n = 0;
		  title_string = XmStringCreateSimple (title);
		  XtSetArg (args[n], XmNlabelString, title_string);
		  n++;
		  XtSetArg (args[n], XmNfontList, DefaultFont);
		  n++;
		  XtSetArg (args[n], XmNmarginWidth, 0);
		  n++;
		  XtSetArg (args[n], XmNmarginHeight, 0);
		  n++;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNforeground, FgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNadjustLast, FALSE);
		  n++;
		  w = XmCreateLabel (row, "Dialogue", args, n);
		  XtManageChild (w);
		  n = 0;
		  XtSetArg (args[n], XmNbackground, BgMenu_Color);
		  n++;
		  XtSetArg (args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);
		  n++;
		  w = XmCreateSeparator (row, "Dialogue", args, n);
		  XtManageChild (w);
		  XmStringFree (title_string);
	       }		/*if */

	     /* Cree une feuille de saisie d'un nombre */
	     if (min < max)
	       {
		  /* Note les bornes de l'echelle */
		  usprintf (bounds, "%d", min);
		  ustrcat (&bounds[ustrlen (bounds)], "..");
		  usprintf (&bounds[ustrlen (bounds)], "%d", max);
		  catalogue->Cat_Entries->E_ThotWidget[2] = (ThotWidget) min;
		  catalogue->Cat_Entries->E_ThotWidget[3] = (ThotWidget) max;
		  ent = max;
	       }
	     else
	       {
		  /* Note les bornes de l'echelle */
		  usprintf (bounds, "%d", max);
		  ustrcat (&bounds[ustrlen (bounds)], "..");
		  usprintf (&bounds[ustrlen (bounds)], "%d", min);
		  catalogue->Cat_Entries->E_ThotWidget[2] = (ThotWidget) max;
		  catalogue->Cat_Entries->E_ThotWidget[3] = (ThotWidget) min;
		  ent = min;
	       }
	     title_string = XmStringCreateSimple (bounds);
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     XtSetArg (args[n], XmNlabelString, title_string);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     XtSetArg (args[n], XmNforeground, FgMenu_Color);
	     n++;
	     w = XmCreateLabel (row, "Dialogue", args, n);
	     XtManageChild (w);
	     XmStringFree (title_string);

	     /* Cree une feuille de saisie annexe */
	     n = 0;
	     XtSetArg (args[n], XmNeditMode, XmSINGLE_LINE_EDIT);
	     n++;
	     XtSetArg (args[n], XmNbackground, BgMenu_Color);
	     n++;
	     usprintf (bounds, "%d", min);
	     XtSetArg (args[n], XmNvalue, bounds);
	     n++;
	     if (min < 0)
		i = 1;
	     else
		i = 0;
	     do
	       {
		  ent = ent / 10;
		  i++;
	       }
	     while (ent > 0);
	     XtSetArg (args[n], XmNcolumns, i);
	     n++;
	     XtSetArg (args[n], XmNmaxLength, i + 1);
	     n++;
	     XtSetArg (args[n], XmNfontList, DefaultFont);
	     n++;
	     w = XmCreateText (row, "Dialogue", args, n);
	     XtManageChild (w);
	     if (catalogue->Cat_React)
	       XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) CallValueSet, catalogue);
	     catalogue->Cat_Entries->E_ThotWidget[1] = w;
	  }
     }
#  endif /* _WINDOWS */
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaSetNumberForm fixe le contenu de la feuille de saisie de texte :        
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre val donne la valeur initiale.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetNumberForm (int ref, int val)
#else  /* __STDC__ */
void                TtaSetNumberForm (ref, val)
int                 ref;
int                 val;

#endif /* __STDC__ */
{
#ifndef _GTK
#  ifndef _WINDOWS 
   CHAR_T              text[10];
   int                 lg;
#  endif /* !_WINDOWS */
   ThotWidget          wtext;
   struct Cat_Context *catalogue;

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
      TtaError (ERR_invalid_reference);
   else if (catalogue->Cat_Widget == 0)
      TtaError (ERR_invalid_reference);
   else
     {
	if (catalogue->Cat_Type != CAT_INT)
	  {
	     TtaError (ERR_invalid_reference);
	     return;
	  }

	/* Est-ce une valeur valide ? */
	if ((val < (int) catalogue->Cat_Entries->E_ThotWidget[2])
	    || (val > (int) catalogue->Cat_Entries->E_ThotWidget[3]))
	  {
	     TtaError (ERR_invalid_reference);
	     return;
	  }

	wtext = catalogue->Cat_Entries->E_ThotWidget[1];

	/* Desactive la procedure de Callback */
#       ifndef _WINDOWS
	if (catalogue->Cat_React)
	  XtRemoveCallback (wtext, XmNvalueChangedCallback, (XtCallbackProc) CallValueSet, catalogue);

	usprintf (text, "%d", val);
	XmTextSetString (wtext, text);
	lg = ustrlen (text);
	XmTextSetSelection (wtext, lg, lg, 500);

	/* Reactive la procedure de Callback */
	if (catalogue->Cat_React)
	  XtAddCallback (wtext, XmNvalueChangedCallback, (XtCallbackProc) CallValueSet, catalogue);
#       endif /* _WINDOWS */
     }
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaSetDialoguePosition me'morise la position actuelle de la souris 
   comme position d'affichage des TtaShowDialogue().               
  ----------------------------------------------------------------------*/
void                TtaSetDialoguePosition ()
{
#ifndef _GTK
#  ifndef _WINDOWS
   ThotWindow          wdum;
   int                 xdum;
   int                 ydum;

   /* Enregistre la position courante du curseur pour les futurs show */
   wdum = RootWindow (GDp, DefaultScreen (GDp));
   XQueryPointer (GDp, wdum, &wdum, &wdum, &xdum, &ydum, &ShowX, &ShowY, &xdum);
#  endif /* _WINDOWS */
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaShowDialogue active le catalogue de'signe.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaShowDialogue (int ref, ThotBool remanent)
#else  /* __STDC__ */
void                TtaShowDialogue (ref, remanent)
int                 ref;
ThotBool            remanent;

#endif /* __STDC__ */
{
#ifndef _GTK
   int                 n;
#  ifndef _WINDOWS
   Arg                 args[MAX_ARGS];
#  else  /* _WINDOWS */
   POINT               curPoint;
#  endif /* _WINDOWS */

   ThotWidget          w;
   struct Cat_Context *catalogue;

   if (ref == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }

   catalogue = CatEntry (ref);
   if (catalogue == NULL)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
   else
      w = catalogue->Cat_Widget;

   if (w == 0)
     {
	TtaError (ERR_invalid_reference);
	return;
     }
#  ifndef _WINDOWS
   else if (XtIsManaged (w))
      XMapRaised (GDp, XtWindowOfObject (XtParent (w)));
#  endif /* _WINDOWS */
   /*===========> Active un pop-up menu */
   else if (catalogue->Cat_Type == CAT_POPUP || catalogue->Cat_Type == CAT_PULL)
     {
	/* Faut-il invalider un TtaShowDialogue precedent */
	TtaAbortShowDialogue ();

	/* Memorise qu'un retour sur le catalogue est attendu et */
	/* qu'il peut etre aborte' si et seulement s'il n'est pas remanent */
	if (!remanent)
	  {
	     ShowReturn = 1;
	     ShowCat = catalogue;
	  }

	/*** Positionne le pop-up a la position courante du show ***/
	n = 0;
#       ifndef _WINDOWS
	XtSetArg (args[n], XmNx, (Position) ShowX);
	n++;
	XtSetArg (args[n], XmNy, (Position) ShowY);
	n++;
	XtSetValues (w, args, n);
	XtManageChild (w);
#       else  /* _WINDOWS */
	if (catalogue->Cat_Type == CAT_POPUP) {
       GetCursorPos (&curPoint);
	   if (!TrackPopupMenu (w,  TPM_LEFTALIGN, curPoint.x, curPoint.y, 0, currentParent, NULL))
		   WinErrorBox (WIN_Main_Wd);
	} else {
          ShowWindow (w, SW_SHOWNORMAL);
          UpdateWindow (w);
    }
#       endif /* _WINDOWS */
     }
   /*===========> Active un formulaire */
   else if (((catalogue->Cat_Type == CAT_FORM)
	     || (catalogue->Cat_Type == CAT_SHEET)
	     || (catalogue->Cat_Type == CAT_DIALOG))
	    && (catalogue->Cat_PtParent == NULL))
     {
	/* Faut-il invalider un TtaShowDialogue precedent */
	TtaAbortShowDialogue ();

	/* Memorise qu'un retour sur le catalogue est attendu et */
	/* qu'il peut etre aborter si et seulement s'il n'est pas remanent */
	if (!remanent)
	  {
	     ShowReturn = 1;
	     ShowCat = catalogue;
	  }

	/* Pour les feuilles de dialogue force le bouton par defaut */
	if (catalogue->Cat_Type == CAT_SHEET
	    || catalogue->Cat_Type == CAT_DIALOG
	    || catalogue->Cat_Type == CAT_FORM)
	  {
#            ifndef _WINDOWS
	     XtSetArg (args[0], XmNdefaultButton, catalogue->Cat_Entries->E_ThotWidget[1]);
	     XtSetValues (w, args, 1);
#            endif /* _WINDOWS */
	  }
	INITform (w, catalogue, NULL);
     }
   else
      TtaError (ERR_invalid_reference);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   TtaWaitShowDialogue attends le retour du catalogue affiche par     
   TtaShowDialogue.                                                   
  ----------------------------------------------------------------------*/
void                TtaWaitShowDialogue ()
{
#ifndef _GTK
   ThotEvent              event;
#  ifdef _WINDOWS
   int frame;
#  endif /* _WINDOWS */

   /* Un TtaWaitShowDialogue en cours */
   CurrentWait = 1;

#  ifdef _WINDOWS
   while (ShowReturn == 1) {
         if (GetMessage (&event, NULL, 0, 0)) {			
            frame = GetFrameNumber (event.hwnd);
            if (frame != -1) {
               if (!hAccel[frame] || !TranslateAccelerator (FrMainRef[frame], hAccel[frame], &event))
                  TtaHandleOneWindowEvent (&event);
			} else
                   TtaHandleOneWindowEvent (&event);
	}
   }
#  else  /* !_WINDOWS */
   TtaLockMainLoop();
   while (ShowReturn == 1)
     {
	TtaFetchOneEvent (&event);
	TtaHandleOneEvent (&event);
     }
   TtaUnlockMainLoop();
#  endif /* _WINDOWS */

   /* Fin de l'attente */
   CurrentWait = 0;
#endif /* _GTK */
}


/*----------------------------------------------------------------------
   TtaTestWaitShowDialogue retourne Vrai (1) si un TtaWaitShowDialogue        
   est en cours, sinon Faux (0).                                      
  ----------------------------------------------------------------------*/
ThotBool            TtaTestWaitShowDialogue ()
{
   return (CurrentWait);
}


/*----------------------------------------------------------------------
   TtaAbortShowDialogue abandonne le TtaShowDialogue.                 
  ----------------------------------------------------------------------*/
void                TtaAbortShowDialogue ()
{
#ifndef _GTK

   if (ShowReturn == 1)
     {
	/* Debloque l'attente courante */
	ShowReturn = 0;

	/* Invalide le menu ou formulaire courant */
	if (ShowCat != NULL)
	  {
	     if (ShowCat->Cat_Widget != 0)
	       {
#                 ifndef _WINDOWS
		  if (XtIsManaged (ShowCat->Cat_Widget))
		    {
		       /* Traitement particulier des formulaires */
		       if (ShowCat->Cat_Type == CAT_FORM
			   || ShowCat->Cat_Type == CAT_SHEET
			   || ShowCat->Cat_Type == CAT_DIALOG)
			 {
			    XtUnmanageChild (XtParent (ShowCat->Cat_Widget));
			    CallSheet (None, ShowCat, NULL);
			 }
		       else
			  XtUnmanageChild (ShowCat->Cat_Widget);
		    }
#                 endif /* _WINDOWS */
	       }

	  }
     }
#endif /* _GTK */
}
#endif /* _WIN_PRINT */
