
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */
#ifndef _FRAME_H
#define _FRAME_H

#ifndef NODISPLAY

#include "constmenu.h"

#define MAX_NFONT   10
#define MAX_FONT    60

struct DisplayDesc
{
   /* Precision de l'ecran */
   int	Larg_Ecran_;		/* Largeur en pixels de l'ecran		*/
   int	Haut_Ecran_;		/* Hauteur en pixels de l'ecran 	*/
#ifndef NEW_WILLOWS
   Time	t1_;			/* VarDate du dernier click de selection  	*/
#endif /* !NEW_WILLOWS */
   /* Divers pour X */
#ifndef NEW_WILLOWS
   Display *GDp_;		/* Identification du serveur X 	       	*/
   int	   ThotScreen_; 	/* Identification de l'ecran   		*/
#endif /* !NEW_WILLOWS */
   ThotWindow  GRootW_;		/* Identification de la fenetre racine 	*/
   int     Gdepth_;		/* Nombre de plans de l'ecran  		*/
#ifdef NEW_WILLOWS
   HPALETTE cmap_;		/* Table des couleurs pour Thot		*/
   WIN_GC_BLK  GCwhite_, GCblack_, GCinvert_, GCgrey_, GCtrait_;
   WIN_GC_BLK  GCdialogue_;
   WIN_GC_BLK  graphicGC_;	/* Graphic context pour les images      */
#else /* NEW_WILLOWS */
   Colormap cmap_;		/* Table des couleurs pour Thot		*/
   ThotGC  GCwhite_, GCblack_, GCinvert_, GCgrey_, GCtrait_;
   ThotGC  GCdialogue_;
   ThotGC  graphicGC_;		/* Graphic context pour les images      */
#endif /* !NEW_WILLOWS */
   /* pour les fontes */
   char NomPolice_[MAX_FONT * MAX_NFONT];/* Name X-Window 		*/
   char NomPs_[MAX_FONT * 8];	/* Name PostScript 			*/
   ptrfont Police_[MAX_FONT];	/* fontes chargees dans le serveur 	*/
   int 	PoliceVue_[MAX_FONT];

   /* pour les curseurs */
   ThotCursor CursSel_;		/* Curseur de designation sur ecran	*/
   ThotCursor CursCmd_;		/* Curseur des commandes locales	*/
   ThotCursor CursFen_;		/* Curseur de designation de fenetres	*/
   ThotCursor CursWait_;	/* Curseur pour attendre	*/
   ThotCursor CursDepV_;	/* Curseur modification presentation 	*/
   ThotCursor CursDepH_;	/* Curseur modification presentation 	*/
   ThotCursor CursDepHV_;	/* Curseur modification presentation 	*/
   int	XCurs_, YCurs_;  	/* Coordonnes du curseurs sur l'ecran 	*/

};

/* Macros de manipulation de la structure DisplayDesc */
#define Larg_Ecran(Disp) 		OpenDisplay.Larg_Ecran_
#define Haut_Ecran(Disp) 		OpenDisplay.Haut_Ecran_
#define t1(Disp) 			OpenDisplay.t1_
#define GDp(Disp) 			OpenDisplay.GDp_
#define ThotScreen(Disp) 		OpenDisplay.ThotScreen_
#define GRootW(Disp) 			OpenDisplay.GRootW_
#define Gdepth(Disp) 			OpenDisplay.Gdepth_
#define cmap(Disp) 			OpenDisplay.cmap_
#ifdef NEW_WILLOWS
#define GCwhite(Disp) 			&OpenDisplay.GCwhite_
#define GCblack(Disp) 			&OpenDisplay.GCblack_
#define GCinvert(Disp) 			&OpenDisplay.GCinvert_
#define GCgrey(Disp) 			&OpenDisplay.GCgrey_
#define GCtrait(Disp) 			&OpenDisplay.GCtrait_
#define GCdialogue(Disp) 		&OpenDisplay.GCdialogue_
#define graphicGC(Disp) 		&OpenDisplay.graphicGC_
#else /* NEW_WILLOWS */
#define GCwhite(Disp) 			OpenDisplay.GCwhite_
#define GCblack(Disp) 			OpenDisplay.GCblack_
#define GCinvert(Disp) 			OpenDisplay.GCinvert_
#define GCgrey(Disp) 			OpenDisplay.GCgrey_
#define GCtrait(Disp) 			OpenDisplay.GCtrait_
#define GCdialogue(Disp) 		OpenDisplay.GCdialogue_
#define graphicGC(Disp) 		OpenDisplay.graphicGC_
#endif /* !NEW_WILLOWS */
#define NomPolice(Disp, n) 		OpenDisplay.NomPolice_[n]
#define NomPs(Disp, n) 			OpenDisplay.NomPs_[n]
#define Police(Disp, n) 		OpenDisplay.Police_[n]
#define PoliceVue(Disp, n) 		OpenDisplay.PoliceVue_[n]
#define CursSel(Disp)			OpenDisplay.CursSel_
#define CursCmd(Disp)			OpenDisplay.CursCmd_
#define CursFen(Disp)			OpenDisplay.CursFen_
#define CursWait(Disp)			OpenDisplay.CursWait_
#define CursDepV(Disp)			OpenDisplay.CursDepV_
#define CursDepH(Disp)			OpenDisplay.CursDepH_
#define CursDepHV(Disp)			OpenDisplay.CursDepHV_
#define XCurs(Disp)			OpenDisplay.XCurs_
#define YCurs(Disp)			OpenDisplay.YCurs_

#define MAX_BUTTON 25
#define MAX_TEXTZONE 3
typedef struct _Frame_Ctl
{
  short         FrTopMargin;    /* Marge au sommet de la fenetre	*/
  short         FrLeftMargin;   /* Marge a bas de la fenetre		*/
  short 	FrWidth;	/* Largeur de la fenetre                */
  short 	FrHeight;      	/* Hauteur de la fenetre                */
  int		FrDoc;		/* CsIdentity du document affiche		*/
  int		FrView;		/* Vue du document affichee		*/
  ThotWidget	WdScrollV;	/* Le widget Scroll Vertical            */
  ThotWidget 	WdScrollH;	/* Le widget Scroll Horizontal          */
  ThotWidget 	WdFrame;	/* Le widget de la fenetre              */
  ThotWidget	WdStatus;	/* Le widget de status courant          */
  struct _Menu_Ctl *FrMenus;	/* Contexte du premier menu		*/
  int		MenuAttr;	/* Identification menu attributs ou 0	*/
  int		MenuSelect;	/* Identification menu selection ou 0	*/
  ThotWidget	WdMenus[MAX_MENU];	/* Widget du menu      		*/
  boolean	ActifMenus[MAX_MENU];	/* Menus actifs      		*/
  ThotWidget	Button[MAX_BUTTON];	/* Widget des boutons		*/
  Proc  	Call_Button[MAX_BUTTON];/* Callback des boutons		*/
  ThotWidget	Text_Zone[MAX_TEXTZONE];/* Widget des boutons		*/
  Proc  	Call_Text[MAX_TEXTZONE];/* Callback des boutons		*/
}Frame_Ctl;
#endif /* !NODISPLAY */

#endif
