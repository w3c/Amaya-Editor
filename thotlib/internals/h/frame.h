/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

#ifndef _FRAME_H
#define _FRAME_H

#ifndef NODISPLAY

#include "constmenu.h"

#define MAX_FONT    60
#define MAX_FONTNAME   10
#define MAX_BUTTON  30
#define MAX_TEXTZONE 3

typedef struct _Frame_Ctl
{
  short         FrTopMargin;    /* Marge au sommet de la fenetre	  */
  short         FrLeftMargin;   /* Marge a bas de la fenetre		  */
  short 	FrWidth;	/* Largeur de la fenetre                  */
  short 	FrHeight;      	/* Hauteur de la fenetre                  */
  int		FrDoc;		/* Ident du document affiche      	  */
  int		FrView;		/* Vue du document affichee		  */
  ThotWidget	WdScrollV;	/* Le widget Scroll Vertical              */
  ThotWidget 	WdScrollH;	/* Le widget Scroll Horizontal            */
  ThotWidget 	WdFrame;	/* Le widget de la fenetre                */
  ThotWidget	WdStatus;	/* Le widget de status courant            */
  struct _Menu_Ctl *FrMenus;	/* Contexte du premier menu		  */
  int		MenuAttr;	/* Identification menu attributs ou 0	  */
  int		MenuSelect;	/* Identification menu selection ou 0	  */
  ThotWidget	WdMenus[MAX_MENU];	/* Widget du menu      		  */
  boolean	ActifMenus[MAX_MENU];	/* Menus actifs      		  */
#ifndef _WINDOWS
  ThotWidget	Button[MAX_BUTTON];	/* Widget des boutons		  */
#else  /* _WINDOWS */
  TBBUTTON*	Button[MAX_BUTTON];	/* Widget des boutons		  */
#endif /* _WINDOWS */
  Proc  	Call_Button[MAX_BUTTON];/* Callback des boutons		  */
  ThotWidget	Text_Zone[MAX_TEXTZONE];/* Widget des boutons		  */
#ifdef _WINDOWS
  ThotWidget    Label[MAX_TEXTZONE];  /* Labels of text zones             */
  /* boolean       showLogo; */
#endif /* _WINDOWS */
  Proc  	Call_Text[MAX_TEXTZONE];/* Callback des boutons		  */
}Frame_Ctl;
#endif /* !NODISPLAY */

#endif
