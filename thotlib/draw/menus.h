/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

#ifndef Emenus
#define Emenus


/*----------------------------------------------------------------------
---------------------------- NESTED INCLUDES -----------------------------
  ----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
------------------------- DEFINE AND TYPEDEF AREA ------------------------
  ----------------------------------------------------------------------*/


#define OFFSET_FEUILLE_TRAIT		0
#define OFFSET_RECTANGLE_STYLE_TRAIT	1
#define OFFSET_LABEL_FORME_OMBRE	2
#define OFFSET_LABEL_STYLE_TRAIT	3

#define OFFSET_PALETTE_LIENS		4
#define OFFSET_LIEN_PREMIERE_LIGNE	5

#define OFFSET_PALETTE_OUTILS		6
#define OFFSET_TOOLS_PREMIERE_COLONNE	7
#define OFFSET_TOOLS_DEUXIEME_COLONNE	8
#define OFFSET_TOOLS_TROISIEME_COLONNE	9
#define OFFSET_TOOLS_QUATRIEME_COLONNE	10
#define OFFSET_TOOLS_CINQUIEME_COLONNE	11

#define OFFSET_FEUILLE_RECTANGLE	12
#define OFFSET_RECTANGLE_EPAISSEUR	13
#define OFFSET_RECTANGLE_EP_TRAIT	14
#define OFFSET_RECTANGLE_OMBRE_1	15
#define OFFSET_RECTANGLE_OMBRE_2	16
#define OFFSET_RECTANGLE_OMBRE_3	17
#define OFFSET_RECTANGLE_COULEURO	18
#define OFFSET_RECTANGLE_COULEUR	19
#define OFFSET_RECTANGLE_ANGLES		20
#define OFFSET_LABEL_OMBRE		21
#define OFFSET_LABEL_LABEL		22
#define OFFSET_POSITION_LABEL		23
#define OFFSET_LABEL_A_VAL		24
#define OFFSET_LABEL_O_VAL		25

#define OFFSET_MENU_RENVOI		26

#define MAX_MENU			27

#define FEUILLE_TRAIT			(MenuBase+OFFSET_FEUILLE_TRAIT)
#define RECTANGLE_STYLE_TRAIT		(MenuBase+OFFSET_RECTANGLE_STYLE_TRAIT)
#define LABEL_FORME_OMBRE		(MenuBase+OFFSET_LABEL_FORME_OMBRE)
#define LABEL_STYLE_TRAIT		(MenuBase+OFFSET_LABEL_STYLE_TRAIT)

#define PALETTE_LIENS			(MenuBase+OFFSET_PALETTE_LIENS)
#define LIEN_PREMIERE_LIGNE		(MenuBase+OFFSET_LIEN_PREMIERE_LIGNE)

#define PALETTE_OUTILS			(MenuBase+OFFSET_PALETTE_OUTILS)
#define TOOLS_PREMIERE_COLONNE	        (MenuBase+OFFSET_TOOLS_PREMIERE_COLONNE)
#define TOOLS_DEUXIEME_COLONNE	        (MenuBase+OFFSET_TOOLS_DEUXIEME_COLONNE)
#define TOOLS_TROISIEME_COLONNE	        (MenuBase+OFFSET_TOOLS_TROISIEME_COLONNE)
#define TOOLS_QUATRIEME_COLONNE	        (MenuBase+OFFSET_TOOLS_QUATRIEME_COLONNE)
#define TOOLS_CINQUIEME_COLONNE	        (MenuBase+OFFSET_TOOLS_CINQUIEME_COLONNE)
#define FEUILLE_RECTANGLE		(MenuBase+OFFSET_FEUILLE_RECTANGLE)
#define RECTANGLE_EPAISSEUR		(MenuBase+OFFSET_RECTANGLE_EPAISSEUR)
#define RECTANGLE_EP_TRAIT		(MenuBase+OFFSET_RECTANGLE_EP_TRAIT)
#define RECTANGLE_OMBRE_1		(MenuBase+OFFSET_RECTANGLE_OMBRE_1)
#define RECTANGLE_OMBRE_2		(MenuBase+OFFSET_RECTANGLE_OMBRE_2)
#define RECTANGLE_OMBRE_3		(MenuBase+OFFSET_RECTANGLE_OMBRE_3)
#define RECTANGLE_COULEURO		(MenuBase+OFFSET_RECTANGLE_COULEURO)
#define RECTANGLE_COULEUR		(MenuBase+OFFSET_RECTANGLE_COULEUR)
#define RECTANGLE_ANGLES		(MenuBase+OFFSET_RECTANGLE_ANGLES)
#define LABEL_OMBRE			(MenuBase+OFFSET_LABEL_OMBRE)
#define LABEL_LABEL			(MenuBase+OFFSET_LABEL_LABEL)
#define POSITION_LABEL			(MenuBase+OFFSET_POSITION_LABEL)
#define LABEL_A_VAL			(MenuBase+OFFSET_LABEL_A_VAL)
#define LABEL_O_VAL			(MenuBase+OFFSET_LABEL_O_VAL)

#define MENU_RENVOI			(MenuBase+OFFSET_MENU_RENVOI)

#define F_REC_TRAIT_LIE			"BRECTANGLE\0BTRAIT\0BLIENS"
#define F_ELL_FLECH_IMA			"BELLIPSE\0BFLECHE\0BIMAGE"
#define F_POLY_TEXT_GRP			"BPOLYGONE\0BTEXTE\0BGROUPE"

#define MAX_TXT_LEN 1024

#ifndef Xmenus
/*----------------------------------------------------------------------
-------------------------- EXTERNAL  VARIABLES ---------------------------
  ----------------------------------------------------------------------*/

extern int MenuBase;
extern int MenuPaletteGenerale, MenuPaletteLien;

/*----------------------------------------------------------------------
-------------------------- EXTERNAL  FUNCTIONS ---------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
/*----------------------------------------------------------------------
----------------------------- C++ INTERFACE ------------------------------
  ----------------------------------------------------------------------*/

extern void CreerPaletteFormeRectangle (int MenuBase, ThotWidget Pere, int Langue);
extern void CreerPaletteForme     (int MenuBase, ThotWidget Pere, int Langue);
extern void CreerPaletteTexte	  (int MenuBase, ThotWidget Pere, int Langue);
extern void CreerPaletteTrait     (int MenuBase, ThotWidget Pere, int Langue);
extern void CreerPaletteFleche    (int MenuBase, ThotWidget Pere, int Langue);
extern void CreerPaletteLien 	  (int MenuBase, ThotWidget Pere, int Langue);

extern void TraiterEvtPaletteFormeRectangle(int ref, int typedata, char *data);
extern void TraiterEvtPaletteForme   (int ref, int typedata, char *data);
extern void TraiterEvtPaletteTexte   (int ref, int typedata, char *data);
extern void TraiterEvtPaletteTrait   (int ref, int typedata, char *data);
extern void TraiterEvtPaletteFormeTrait (int ref, int typedata, char* data);
extern void TraiterEvtPaletteFleche  (int ref, int typedata, char *data);
extern void TraiterEvtPaletteLien    (int ref, int typedata, char *data);

extern void AppliquerFeuilleRectangle 	(int ref, int typedata, char *data);
extern void AppliquerFeuilleTrait 	(int ref, int typedata, char *data);


#else
/*----------------------------------------------------------------------
-------------------------- SIMPLE  C INTERFACE ---------------------------
  ----------------------------------------------------------------------*/

extern void CreerPaletteFormeRectangle 
				 (/* int MenuBase, ThotWidget Pere, int Langue */);
extern void CreerPaletteForme     (/*int MenuBase, ThotWidget Pere, int Langue*/);
extern void CreerPaletteTexte	  (/*int MenuBase, ThotWidget Pere, int Langue*/);
extern void CreerPaletteTrait     (/*int MenuBase, ThotWidget Pere, int Langue*/);
extern void CreerPaletteFleche    (/*int MenuBase, ThotWidget Pere, int Langue*/);
extern void CreerPaletteLien      (/*int MenuBase, ThotWidget Pere, int Langue*/);

extern void TraiterEvtPaletteFormeRectangle
				 (/* int MenuBase, ThotWidget Pere, int Langue */);
extern void TraiterEvtPaletteForme   (/* int ref, int typedata, char *data */);
extern void TraiterEvtPaletteTexte   (/* int ref, int typedata, char *data */);
extern void TraiterEvtPaletteTrait   (/* int ref, int typedata, char *data */);
extern void TraiterEvtPaletteFormeTrait(/* int ref,int typedata,char* data */);
extern void TraiterEvtPaletteFleche  (/* int ref, int typedata, char *data */);
extern void TraiterEvtPaletteLien    (/* int ref, int typedata, char *data */);


extern void AppliquerFeuilleRectangle(/* int ref, int typedata, char *data */);
extern void AppliquerFeuilleTrait    (/* int ref, int typedata, char *data */);

#endif /* __STDC__ */
#endif /* Xmenus */
#endif /* Emenus */
