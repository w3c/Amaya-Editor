/*!BM
   ------------------------------------------------------------------------------
   *
   * Module      : menus.c
   * Language    : C
   * Authors     : Christian Lenne
   * Version     : %I%
   * Creation    : %E% %U%
   * Description : 
   *
   * Defined variables and functions :
   *
   *           <---------  L O C A L  --------->
   *
   *           <------- E X P O R T E D ------->
   *
   *            ElementAModifier
   *
   *            CreerPaletteFormeRectangle
   *            CreerPaletteForme
   *            CreerPaletteTexte
   *            CreerPaletteFormeTrait
   *            CreerPaletteFleche
   *            CreerPaletteLien
   *            TraiterEvtPaletteFormeRectangle
   *            TraiterEvtPaletteForme
   *            TraiterEvtPaletteTexte
   *            TraiterEvtPaletteTrait
   *            TraiterEvtPaletteFormeTrait
   *            TraiterEvtPaletteFleche
   *            TraiterEvtPaletteLien
   *            AppliquerFeuilleRectangle
   *            AppliquerFeuilleTrait
   *
   ------------------------------------------------------------------------------
   !EM */

#define Xmenus

/****************************************************************************/
/*--------------------------- SYSTEM INCLUDES ------------------------------*/
/****************************************************************************/

#include "thot_gui.h"
#include "thot_sys.h"

/****************************************************************************/
/*------------------------ APPLICATION  INCLUDES ---------------------------*/
/****************************************************************************/

#include "Draw3.h"
#include "application.h"
#include "document.h"
#include "attribute.h"
#include "language.h"
#include "content.h"
#include "reference.h"
#include "presentation.h"
#include "selection.h"
#include "tree.h"
#include "app.h"
#include "dialog.h"
#include "message.h"
#include "libmsg.h"

#include "boutons.h"
#include "menus.h"
#include "objets.h"
#include "utilitaires.h"
#include "Draw3actions.h"
#include "drawmsg.h"

#include "Icons/lien1.xpm"
#include "Icons/lien2.xpm"
#include "Icons/lien3.xpm"
#include "Icons/lien4.xpm"
#include "Icons/lien5.xpm"

#include "Icons/AngleDroit.xpm"
#include "Icons/AngleRond.xpm"
#include "Icons/LabelHG.xpm"
#include "Icons/LabelHD.xpm"
#include "Icons/LabelC.xpm"
#include "Icons/LabelBG.xpm"
#include "Icons/LabelBD.xpm"
#include "Icons/TraitPlein.xpm"
#include "Icons/TraitTiret.xpm"
#include "Icons/TraitDot.xpm"
#include "Icons/Forme.xpm"
#include "Icons/Ombre.xpm"

#include "Icons/Trait.xpm"
#include "Icons/TraitFG.xpm"
#include "Icons/TraitFD.xpm"
#include "Icons/TraitFDG.xpm"
#include "Icons/TraitO.xpm"
#include "Icons/TraitOH.xpm"
#include "Icons/TraitH.xpm"
#include "Icons/TraitV.xpm"

#include "Icons/Trait1.xpm"
#include "Icons/Trait2.xpm"
#include "Icons/Trait4.xpm"
#include "Icons/Trait6.xpm"
#include "Icons/Trait8.xpm"
#include "Icons/OmbreNO.xpm"
#include "Icons/OmbreN.xpm"
#include "Icons/OmbreNE.xpm"
#include "Icons/OmbreE.xpm"
#include "Icons/OmbreSE.xpm"
#include "Icons/OmbreS.xpm"
#include "Icons/OmbreSO.xpm"
#include "Icons/OmbreO.xpm"
#include "Icons/OmbreSans.xpm"
#include "Icons/Couleur.xpm"
#include "Icons/Texture.xpm"

/****************************************************************************/
/*----------------------- LOCAL DEFINES AND TYPES --------------------------*/
/****************************************************************************/

/****************************************************************************/
/*--------------------------- LOCAL VARIABLES ------------------------------*/
/****************************************************************************/

static int Ombre, Epaisseur, PositionLabel, TypeTrait;
static int BoutTrait;
static char Bords, ShapeTrait;
static int PereFormeRectangle, PereFormeTrait;

/****************************************************************************/
/*-------------------------- GLOBAL VARIABLES ------------------------------*/
/****************************************************************************/

int MenuBase;
Element ElementAModifier;
extern Document CurrentDoc, DocPalette;

/****************************************************************************/
/*-------------------------- FORWARD FUNCTIONS -----------------------------*/
/****************************************************************************/

/****************************************************************************/
/*--------------------------- LOCAL FUNCTIONS ------------------------------*/
/****************************************************************************/

/****************************************************************************/
/*------------------------- EXPORTED  FUNCTIONS ----------------------------*/
/****************************************************************************/

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerPaletteForme
   * Result        : void
   * Parameters
   * Name          Type         Usage
   * ----          ----                 -----
   * Menu       int             Idf du menu
   * Pere               ThotWidget          ThotWidget de rattachement
   * Langue     int             Langue pour l'affichage de messaage
   *
   * Functionality : Cree le formulaire associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerPaletteFormeRectangle (int Menu, ThotWidget Pere, int Langue)

#else /* __STDC__ */

void 
CreerPaletteFormeRectangle (Menu, Pere, Langue)
     int Menu;
     ThotWidget Pere;
     int Langue;

#endif /* __STDC__ */

{
  Element ElemForme, ElemLabel, ElemOmbre;

  static Pixmap TabEpaisseur[5] =
  {0, 0, 0, 0, 0};
  static Pixmap TabLabel[5] =
  {0, 0, 0, 0, 0};
  static Pixmap TabOmbres1[3] =
  {0, 0, 0};
  static Pixmap TabOmbres2[3] =
  {0, 0, 0};
  static Pixmap TabOmbres3[3] =
  {0, 0, 0};
  static Pixmap TabCouleur[2] =
  {0, 0};
  static Pixmap TabCouleurO[2] =
  {0, 0};
  static Pixmap TabStyleTrait[3] =
  {0, 0, 0};
  static Pixmap TabAngles[2] =
  {0, 0};

  char BufMenu[100];
  int len;

  PereFormeRectangle = Menu;
  strcpy(BufMenu,TtaGetMessage(LIB,TMSG_APPLY));
  len = strlen(BufMenu)+1;
  sprintf(&BufMenu[len],"%s",TtaGetMessage(DrawMsgTable,DRW_PREFER));
  TtaNewSheet (FEUILLE_RECTANGLE, Pere, 0, 0, 
	       TtaGetMessage(DrawMsgTable,DRW_POLY_ATTR),
	       2, BufMenu, TRUE, 2, 'L',D_DONE);

  if (TabEpaisseur[0] == 0)
    TabEpaisseur[0] = TtaCreatePixmapLogo (Trait1_xpm);
  if (TabEpaisseur[1] == 0)
    TabEpaisseur[1] = TtaCreatePixmapLogo (Trait2_xpm);
  if (TabEpaisseur[2] == 0)
    TabEpaisseur[2] = TtaCreatePixmapLogo (Trait4_xpm);
  if (TabEpaisseur[3] == 0)
    TabEpaisseur[3] = TtaCreatePixmapLogo (Trait6_xpm);
  if (TabEpaisseur[4] == 0)
    TabEpaisseur[4] = TtaCreatePixmapLogo (Trait8_xpm);

  if (TabLabel[0] == 0)
    TabLabel[0] = TtaCreatePixmapLogo (LabelHG_xpm);
  if (TabLabel[1] == 0)
    TabLabel[1] = TtaCreatePixmapLogo (LabelHD_xpm);
  if (TabLabel[2] == 0)
    TabLabel[2] = TtaCreatePixmapLogo (LabelC_xpm);
  if (TabLabel[3] == 0)
    TabLabel[3] = TtaCreatePixmapLogo (LabelBG_xpm);
  if (TabLabel[4] == 0)
    TabLabel[4] = TtaCreatePixmapLogo (LabelBD_xpm);

  if (TabOmbres1[0] == 0)
    TabOmbres1[0] = TtaCreatePixmapLogo (OmbreSE_xpm);
  if (TabOmbres1[1] == 0)
    TabOmbres1[1] = TtaCreatePixmapLogo (OmbreS_xpm);
  if (TabOmbres1[2] == 0)
    TabOmbres1[2] = TtaCreatePixmapLogo (OmbreSO_xpm);

  if (TabOmbres2[0] == 0)
    TabOmbres2[0] = TtaCreatePixmapLogo (OmbreE_xpm);
  if (TabOmbres2[1] == 0)
    TabOmbres2[1] = TtaCreatePixmapLogo (OmbreSans_xpm);
  if (TabOmbres2[2] == 0)
    TabOmbres2[2] = TtaCreatePixmapLogo (OmbreO_xpm);

  if (TabOmbres3[0] == 0)
    TabOmbres3[0] = TtaCreatePixmapLogo (OmbreNE_xpm);
  if (TabOmbres3[1] == 0)
    TabOmbres3[1] = TtaCreatePixmapLogo (OmbreN_xpm);
  if (TabOmbres3[2] == 0)
    TabOmbres3[2] = TtaCreatePixmapLogo (OmbreNO_xpm);

  if (TabCouleur[0] == 0)
    TabCouleur[0] = TtaCreatePixmapLogo (Couleur_xpm);
  if (TabCouleur[1] == 0)
    TabCouleur[1] = TtaCreatePixmapLogo (Texture_xpm);

  if (TabCouleurO[0] == 0)
    TabCouleurO[0] = TtaCreatePixmapLogo (Forme_xpm);
  if (TabCouleurO[1] == 0)
    TabCouleurO[1] = TtaCreatePixmapLogo (Ombre_xpm);

  if (TabStyleTrait[0] == 0)
    TabStyleTrait[0] = TtaCreatePixmapLogo (TraitPlein_xpm);
  if (TabStyleTrait[1] == 0)
    TabStyleTrait[1] = TtaCreatePixmapLogo (TraitTiret_xpm);
  if (TabStyleTrait[2] == 0)
    TabStyleTrait[2] = TtaCreatePixmapLogo (TraitDot_xpm);

  if (TabAngles[0] == 0)
    TabAngles[0] = TtaCreatePixmapLogo (AngleRond_xpm);
  if (TabAngles[1] == 0)
    TabAngles[1] = TtaCreatePixmapLogo (AngleDroit_xpm);

  ElemForme = TtaGetFirstChild (ElementAModifier);
  ElemLabel = ElemForme;
  TtaNextSibling (&ElemLabel);
  ElemOmbre = TtaGetLastChild (ElementAModifier);
  if (ElemLabel == ElemOmbre)
    ElemLabel = NULL;

  Ombre = GetValAttrEntier (ElemOmbre, Draw3_ATTR_SensOmbre, CurrentDoc);
  Epaisseur = GetPRule (ElemForme, PRLineWeight);
  if (ElemLabel == NULL)
    PositionLabel = Draw3_ATTR_Position_label_VAL_Centre;
  else
    PositionLabel = GetValAttrEntier (ElemLabel,
				      Draw3_ATTR_Position_label,
				      CurrentDoc);
  TypeTrait = GetPRule (ElemForme, PRLineStyle);
  Bords = TtaGetGraphicsShape (ElemForme);
  TtaNewIconMenu (RECTANGLE_COULEURO, Menu, 0, NULL, 2, &TabCouleurO[0], TRUE);
  TtaNewLabel (LABEL_FORME_OMBRE, Menu, TtaGetMessage(DrawMsgTable,DRW_SHAPE_PARAM));
  TtaNewIconMenu (RECTANGLE_STYLE_TRAIT, Menu, 0, NULL, 3, &TabStyleTrait[0],
		  TRUE);
  switch (TypeTrait)
    {
    case DashedLine:
      TtaNewLabel (LABEL_STYLE_TRAIT, Menu,
		   TtaGetMessage(DrawMsgTable,DRW_LINE_DASH));
      break;
    case DottedLine:
      TtaNewLabel (LABEL_STYLE_TRAIT, Menu,
		   TtaGetMessage(DrawMsgTable,DRW_LINE_DOTS));
      break;
    case SolidLine:
    default:
      TtaNewLabel (LABEL_STYLE_TRAIT, Menu, TtaGetMessage(DrawMsgTable,DRW_LINE_PLAIN));
      break;
    }
  TtaNewIconMenu (RECTANGLE_EPAISSEUR, Menu, 0, NULL, 5, &TabEpaisseur[0], TRUE);
  TtaNewNumberForm (RECTANGLE_EP_TRAIT, Menu, TtaGetMessage(DrawMsgTable,DRW_LINE_THICKNESS), 0, 72, FALSE);
  TtaSetNumberForm (RECTANGLE_EP_TRAIT, Epaisseur);
  TtaNewIconMenu (POSITION_LABEL, Menu, 0, NULL, 5, &TabLabel[0], TRUE);
  switch (PositionLabel)
    {
    case Draw3_ATTR_Position_label_VAL_Haut_gauche:
      TtaNewLabel (LABEL_LABEL, Menu,
		   TtaGetMessage(DrawMsgTable,DRW_LBL_UL));
      break;
    case Draw3_ATTR_Position_label_VAL_Haut_droit:
      TtaNewLabel (LABEL_LABEL, Menu,
		   TtaGetMessage(DrawMsgTable,DRW_LBL_UR));
      break;
    case Draw3_ATTR_Position_label_VAL_Bas_gauche:
      TtaNewLabel (LABEL_LABEL, Menu,
		   TtaGetMessage(DrawMsgTable,DRW_LBL_BL));
      break;
    case Draw3_ATTR_Position_label_VAL_Bas_droit:
      TtaNewLabel (LABEL_LABEL, Menu,
		   TtaGetMessage(DrawMsgTable,DRW_LBL_BL));
      break;
    case Draw3_ATTR_Position_label_VAL_Centre:
    default:
      TtaNewLabel (LABEL_LABEL, Menu,
		   TtaGetMessage(DrawMsgTable,DRW_LBL_CENTER));
      break;
    }
  TtaNewLabel (LABEL_OMBRE, Menu, TtaGetMessage(DrawMsgTable,DRW_SHADOW_POS));
  switch (Ombre)
    {
    case Draw3_ATTR_SensOmbre_VAL_NOuest:
      TtaNewLabel (LABEL_O_VAL, Menu, TtaGetMessage(DrawMsgTable,DRW_NE));
      break;
    case Draw3_ATTR_SensOmbre_VAL_Nord:
      TtaNewLabel (LABEL_O_VAL, Menu, TtaGetMessage(DrawMsgTable,DRW_N));
      break;
    case Draw3_ATTR_SensOmbre_VAL_NEst:
      TtaNewLabel (LABEL_O_VAL, Menu, TtaGetMessage(DrawMsgTable,DRW_NW));
      break;
    case Draw3_ATTR_SensOmbre_VAL_Est:
      TtaNewLabel (LABEL_O_VAL, Menu, TtaGetMessage(DrawMsgTable,DRW_E));
      break;
    case Draw3_ATTR_SensOmbre_VAL_Ouest:
      TtaNewLabel (LABEL_O_VAL, Menu, TtaGetMessage(DrawMsgTable,DRW_W));
      break;
    case Draw3_ATTR_SensOmbre_VAL_SOuest:
      TtaNewLabel (LABEL_O_VAL, Menu, TtaGetMessage(DrawMsgTable,DRW_SW));
      break;
    case Draw3_ATTR_SensOmbre_VAL_Sud:
      TtaNewLabel (LABEL_O_VAL, Menu, TtaGetMessage(DrawMsgTable,DRW_S));
      break;
    case Draw3_ATTR_SensOmbre_VAL_SEst:
      TtaNewLabel (LABEL_O_VAL, Menu, TtaGetMessage(DrawMsgTable,DRW_SE));
      break;
    case Draw3_ATTR_SensOmbre_VAL_Sans:
    default:
      TtaNewLabel (LABEL_O_VAL, Menu, TtaGetMessage(DrawMsgTable,DRW_WITHOUT));
      break;
    }
  TtaNewIconMenu (RECTANGLE_OMBRE_1, Menu, 0, NULL, 3, &TabOmbres1[0], TRUE);
  TtaNewIconMenu (RECTANGLE_COULEUR, Menu, 0, NULL, 2, &TabCouleur[0], TRUE);
  TtaNewIconMenu (RECTANGLE_OMBRE_2, Menu, 0, NULL, 3, &TabOmbres2[0], TRUE);
  TtaNewIconMenu (RECTANGLE_ANGLES, Menu, 0, NULL, 2, &TabAngles[0], TRUE);
  TtaNewIconMenu (RECTANGLE_OMBRE_3, Menu, 0, NULL, 3, &TabOmbres3[0], TRUE);
  TtaNewLabel (LABEL_A_VAL, Menu, TtaGetMessage(DrawMsgTable,DRW_RIGHT_ANGLE));

  TtaShowDialogue (Menu, FALSE);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerPaletteTrait
   * Result        : void
   * Parameters
   * Name          Type         Usage
   * ----          ----                 -----
   * Menu       int             Idf du menu
   * Pere               ThotWidget          ThotWidget de rattachement
   * Langue     int             Langue pour l'affichage de messaage
   *
   * Functionality : Cree le formulaire associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerPaletteTrait (int Menu, ThotWidget Pere, int Langue)

#else /* __STDC__ */

void 
CreerPaletteTrait (Menu, Pere, Langue)
     int Menu;
     ThotWidget Pere;
     int Langue;

#endif /* __STDC__ */

{
  Element ElemForme;

  static Pixmap TabEpaisseur[5] =
  {0, 0, 0, 0, 0};
  static Pixmap TabOmbres1[4] =
  {0, 0, 0, 0};
  static Pixmap TabOmbres2[4] =
  {0, 0, 0, 0};
  static Pixmap TabCouleur[2] =
  {0, 0};
  static Pixmap TabStyleTrait[3] =
  {0, 0, 0};

  char BufMenu[100];
  int len;

  PereFormeTrait = Menu;
 
  strcpy(BufMenu,TtaGetMessage(LIB,TMSG_APPLY));
  len = strlen(BufMenu)+1;
  sprintf(&BufMenu[len],"%s",TtaGetMessage(DrawMsgTable,DRW_PREFER));

  TtaNewSheet (FEUILLE_TRAIT, Pere, 0, 0, TtaGetMessage(DrawMsgTable,DRW_LINE_ATTR),
	       2, BufMenu, TRUE, 2, 'L',D_DONE);

  if (TabEpaisseur[0] == 0)
    TabEpaisseur[0] = TtaCreatePixmapLogo (Trait1_xpm);
  if (TabEpaisseur[1] == 0)
    TabEpaisseur[1] = TtaCreatePixmapLogo (Trait2_xpm);
  if (TabEpaisseur[2] == 0)
    TabEpaisseur[2] = TtaCreatePixmapLogo (Trait4_xpm);
  if (TabEpaisseur[3] == 0)
    TabEpaisseur[3] = TtaCreatePixmapLogo (Trait6_xpm);
  if (TabEpaisseur[4] == 0)
    TabEpaisseur[4] = TtaCreatePixmapLogo (Trait8_xpm);

  if (TabOmbres1[0] == 0)
    TabOmbres1[0] = TtaCreatePixmapLogo (Trait_xpm);
  if (TabOmbres1[1] == 0)
    TabOmbres1[1] = TtaCreatePixmapLogo (TraitFD_xpm);
  if (TabOmbres1[2] == 0)
    TabOmbres1[2] = TtaCreatePixmapLogo (TraitFG_xpm);
  if (TabOmbres1[3] == 0)
    TabOmbres1[3] = TtaCreatePixmapLogo (TraitFDG_xpm);

  if (TabOmbres2[0] == 0)
    TabOmbres2[0] = TtaCreatePixmapLogo (TraitO_xpm);
  if (TabOmbres2[1] == 0)
    TabOmbres2[1] = TtaCreatePixmapLogo (TraitH_xpm);
  if (TabOmbres2[2] == 0)
    TabOmbres2[2] = TtaCreatePixmapLogo (TraitV_xpm);
  if (TabOmbres2[3] == 0)
    TabOmbres2[3] = TtaCreatePixmapLogo (TraitOH_xpm);

  if (TabCouleur[0] == 0)
    TabCouleur[0] = TtaCreatePixmapLogo (Couleur_xpm);
  if (TabCouleur[1] == 0)
    TabCouleur[1] = TtaCreatePixmapLogo (Texture_xpm);

  if (TabStyleTrait[0] == 0)
    TabStyleTrait[0] = TtaCreatePixmapLogo (TraitPlein_xpm);
  if (TabStyleTrait[1] == 0)
    TabStyleTrait[1] = TtaCreatePixmapLogo (TraitTiret_xpm);
  if (TabStyleTrait[2] == 0)
    TabStyleTrait[2] = TtaCreatePixmapLogo (TraitDot_xpm);

  ElemForme = TtaGetFirstChild (ElementAModifier);
  Epaisseur = GetPRule (ElemForme, PRLineWeight);
  if (Epaisseur == 0)
    Epaisseur = 1;
  TypeTrait = GetPRule (ElemForme, PRLineStyle);
  ShapeTrait = TtaGetGraphicsShape (ElemForme);
  TtaNewIconMenu (RECTANGLE_STYLE_TRAIT, Menu, 0, NULL, 3, &TabStyleTrait[0],
		  TRUE);
  switch (TypeTrait)
    {
    case DashedLine:
      TtaNewLabel (LABEL_STYLE_TRAIT, Menu,
		   TtaGetMessage(DrawMsgTable,DRW_LINE_DASH));
      break;
    case DottedLine:
      TtaNewLabel (LABEL_STYLE_TRAIT, Menu,
		   TtaGetMessage(DrawMsgTable,DRW_LINE_DOTS));
      break;
    case SolidLine:
    default:
      TtaNewLabel (LABEL_STYLE_TRAIT, Menu, TtaGetMessage(DrawMsgTable,DRW_LINE_PLAIN));
      break;
    }
  TtaNewIconMenu (RECTANGLE_EPAISSEUR, Menu, 0, NULL, 5, &TabEpaisseur[0], TRUE);
  TtaNewNumberForm (RECTANGLE_EP_TRAIT, Menu, TtaGetMessage(DrawMsgTable,DRW_LINE_THICKNESS), 0, 72, FALSE);
  TtaSetNumberForm (RECTANGLE_EP_TRAIT, Epaisseur);
  TtaNewIconMenu (RECTANGLE_OMBRE_1, Menu, 0, NULL, 4, &TabOmbres1[0], TRUE);
  TtaNewIconMenu (RECTANGLE_COULEUR, Menu, 0, NULL, 2, &TabCouleur[0], TRUE);
  TtaNewIconMenu (RECTANGLE_OMBRE_2, Menu, 0, NULL, 4, &TabOmbres2[0], TRUE);

  TtaShowDialogue (Menu, FALSE);
  return;

}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerPaletteFleche
   * Result        : void
   * Parameters
   * Name          Type         Usage
   * ----          ----                 -----
   * Menu       int             Idf du menu
   * Pere               ThotWidget          ThotWidget de rattachement
   * Langue     int             Langue pour l'affichage de messaage
   *
   * Functionality : Cree le formulaire associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerPaletteFleche (int Menu, ThotWidget Pere, int Langue)

#else /* __STDC__ */

void 
CreerPaletteFleche (Menu, Pere, Langue)
     int Menu;
     ThotWidget Pere;
     int Langue;

#endif /* __STDC__ */

{

}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerPaletteLien
   * Result        : void
   * Parameters
   * Name          Type         Usage
   * ----          ----                 -----
   * Menu       int             Idf du menu
   * Pere               ThotWidget          ThotWidget de rattachement
   * Langue     int             Langue pour l'affichage de messaage
   *
   * Functionality : Cree le formulaire associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerPaletteLien (int Menu, ThotWidget Pere, int Langue)

#else /* __STDC__ */

void 
CreerPaletteLien (Menu, Pere, Langue)
     int Menu;
     ThotWidget Pere;
     int Langue;

#endif /* __STDC__ */

{
  static Pixmap TabFormes[5] =
  {0, 0, 0, 0, 0};

  TtaNewSheet (Menu, Pere, 0, 0, TtaGetMessage(DrawMsgTable,DRW_LINK_TYPES),
	       0, "", FALSE, 1, 'L',D_DONE);
  if (TabFormes[0] == 0)
    TabFormes[0] = TtaCreatePixmapLogo (lien1_xpm);
  if (TabFormes[1] == 0)
    TabFormes[1] = TtaCreatePixmapLogo (lien2_xpm);
  if (TabFormes[2] == 0)
    TabFormes[2] = TtaCreatePixmapLogo (lien3_xpm);
  if (TabFormes[3] == 0)
    TabFormes[3] = TtaCreatePixmapLogo (lien4_xpm);
  if (TabFormes[4] == 0)
    TabFormes[4] = TtaCreatePixmapLogo (lien5_xpm);
  TtaNewIconMenu (LIEN_PREMIERE_LIGNE, Menu, 0, NULL, 5, &TabFormes[0], TRUE);

}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : TraiterEvtPaletteFormeRectangle
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * ref                int             Evenement
   * typedata   int             Type du parametre
   * data               char *          Parametre
   *
   * Functionality : Traitement d'un callback
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
TraiterEvtPaletteFormeRectangle (int ref, int typedata, char *data)

#else /* __STDC__ */

void 
TraiterEvtPaletteFormeRectangle (ref, typedata, data)
     int ref, typedata;
     char *data;

#endif /* __STDC__ */

{
  int Action, X, Y;
  Document Doc;
  View VuePrincipale;
  Element Selected;

  Action = (int) data;

  TtaGiveActiveView(&Doc,&VuePrincipale);
  if (Doc!=CurrentDoc)
   { 
    return;
   }
  switch (ref)
    {
    case OFFSET_RECTANGLE_EPAISSEUR:
      switch (Action)
	{
	case 0:
	  Epaisseur = 1;
	  break;
	case 1:
	  Epaisseur = 2;
	  break;
	case 2:
	  Epaisseur = 4;
	  break;
	case 3:
	  Epaisseur = 6;
	  break;
	case 4:
	  Epaisseur = 8;
	  break;
	}
      TtaSetNumberForm (RECTANGLE_EP_TRAIT, Epaisseur);
      break;
    case OFFSET_RECTANGLE_EP_TRAIT:
      Epaisseur = Action;
      break;
    case OFFSET_POSITION_LABEL:
      switch (Action)
	{
	case 0:
	  PositionLabel =
	    Draw3_ATTR_Position_label_VAL_Haut_gauche;
	  TtaNewLabel (LABEL_LABEL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_LBL_UL));
	  break;
	case 1:
	  PositionLabel =
	    Draw3_ATTR_Position_label_VAL_Haut_droit;
	  TtaNewLabel (LABEL_LABEL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_LBL_UR));
	  break;
	case 2:
	  PositionLabel =
	    Draw3_ATTR_Position_label_VAL_Centre;
	  TtaNewLabel (LABEL_LABEL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_LBL_CENTER));
	  break;
	case 3:
	  PositionLabel =
	    Draw3_ATTR_Position_label_VAL_Bas_gauche;
	  TtaNewLabel (LABEL_LABEL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_LBL_BL));
	  break;
	case 4:
	  PositionLabel =
	    Draw3_ATTR_Position_label_VAL_Bas_droit;
	  TtaNewLabel (LABEL_LABEL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_LBL_BR));
	  break;
	}
      break;
    case OFFSET_RECTANGLE_OMBRE_1:
      switch (Action)
	{
	case 0:
	  Ombre = Draw3_ATTR_SensOmbre_VAL_NOuest;
	  TtaNewLabel (LABEL_O_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_NW));
	  break;
	case 1:
	  Ombre = Draw3_ATTR_SensOmbre_VAL_Nord;
	  TtaNewLabel (LABEL_O_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_N));
	  break;
	case 2:
	  Ombre = Draw3_ATTR_SensOmbre_VAL_NEst;
	  TtaNewLabel (LABEL_O_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_NE));
	  break;
	}
      break;
    case OFFSET_RECTANGLE_OMBRE_2:
      switch (Action)
	{
	case 0:
	  Ombre = Draw3_ATTR_SensOmbre_VAL_Ouest;
	  TtaNewLabel (LABEL_O_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_W));
	  break;
	case 1:
	  Ombre = Draw3_ATTR_SensOmbre_VAL_Sans;
	  TtaNewLabel (LABEL_O_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_WITHOUT));
	  break;
	case 2:
	  Ombre = Draw3_ATTR_SensOmbre_VAL_Est;
	  TtaNewLabel (LABEL_O_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_E));
	  break;
	}
      break;
    case OFFSET_RECTANGLE_OMBRE_3:
      switch (Action)
	{
	case 0:
	  Ombre = Draw3_ATTR_SensOmbre_VAL_SOuest;
	  TtaNewLabel (LABEL_O_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_SW));
	  break;
	case 1:
	  Ombre = Draw3_ATTR_SensOmbre_VAL_Sud;
	  TtaNewLabel (LABEL_O_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_S));
	  break;
	case 2:
	  Ombre = Draw3_ATTR_SensOmbre_VAL_SEst;
	  TtaNewLabel (LABEL_O_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_SE));
	  break;
	}
      break;
    case OFFSET_RECTANGLE_COULEUR:
      switch (Action)
	{
	case 0:
	  TtcChangeColors (CurrentDoc,VuePrincipale);
	  break;
	case 1:
	  break;
	}
      break;
    case OFFSET_RECTANGLE_ANGLES:
      switch (Action)
	{
	case 0:
	  Bords = 'c';
	  TtaNewLabel (LABEL_A_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_ROUND_ANGLE));
	  break;
	case 1:
	  Bords = 'a';
	  TtaNewLabel (LABEL_A_VAL, PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_RIGHT_ANGLE));
	  break;
	}
      break;
    case OFFSET_RECTANGLE_COULEURO:
      switch (Action)
	{
	case 0:
	  TtaNewLabel (LABEL_FORME_OMBRE,
		       PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_SHAPE_PARAM));
	  TtaGiveFirstSelectedElement (CurrentDoc,
				       &Selected, &X, &Y);
	  if (TtaGetElementType
	      (TtaGetParent (Selected)).ElTypeNum ==
	      Draw3_EL_Ombre)
	    Selected = TtaGetFirstChild
	      (TtaGetParent (
			      TtaGetParent (Selected))
	      );
	  else
	    Selected = TtaGetFirstChild (
					  TtaGetParent
					  (Selected));
	  TtaSelectElement (CurrentDoc, Selected);
	  break;
	case 1:
	  TtaNewLabel (LABEL_FORME_OMBRE,
		       PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_SHADOW_PARAM));
	  TtaGiveFirstSelectedElement (CurrentDoc,
				       &Selected, &X, &Y);
	  if (TtaGetElementType
	      (TtaGetParent (Selected)).ElTypeNum ==
	      Draw3_EL_Ombre)
	    Selected = TtaGetLastChild
	      (TtaGetParent (
			      TtaGetParent (Selected))
	      );
	  else
	    Selected = TtaGetLastChild (
					 TtaGetParent
					 (Selected));
	  TtaSelectElement (CurrentDoc, Selected);
	  break;
	}
      break;
    case OFFSET_RECTANGLE_STYLE_TRAIT:
      switch (Action)
	{
	case 0:
	  TtaNewLabel (LABEL_STYLE_TRAIT,
		       PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_LINE_PLAIN));
	  TypeTrait = SolidLine;
	  break;
	case 1:
	  TtaNewLabel (LABEL_STYLE_TRAIT,
		       PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_LINE_DASH));
	  TypeTrait = DashedLine;
	  break;
	case 2:
	  TtaNewLabel (LABEL_STYLE_TRAIT,
		       PereFormeRectangle,
		       TtaGetMessage(DrawMsgTable,DRW_LINE_DOTS));
	  TypeTrait = DottedLine;
	  break;
	}
      break;
    }
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : TraiterEvtPaletteFormeTrait
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * ref                int             Evenement
   * typedata   int             Type du parametre
   * data               char *          Parametre
   *
   * Functionality : Traitement d'un callback
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
TraiterEvtPaletteFormeTrait (int ref, int typedata, char *data)

#else /* __STDC__ */

void 
TraiterEvtPaletteFormeTrait (ref, typedata, data)
     int ref, typedata;
     char *data;

#endif /* __STDC__ */

{
  int Action;
  Document Doc;
  View VuePrincipale;

  Action = (int) data;
  
  TtaGiveActiveView(&Doc,&VuePrincipale);
  if (Doc!=CurrentDoc)
    return;
/*   VuePrincipale = TtaGetViewFromName (Doc, "The_draw"); */
  switch (ref)
    {
    case OFFSET_RECTANGLE_EPAISSEUR:
      switch (Action)
	{
	case 0:
	  Epaisseur = 1;
	  break;
	case 1:
	  Epaisseur = 2;
	  break;
	case 2:
	  Epaisseur = 4;
	  break;
	case 3:
	  Epaisseur = 6;
	  break;
	case 4:
	  Epaisseur = 8;
	  break;
	}
      TtaSetNumberForm (RECTANGLE_EP_TRAIT, Epaisseur);
      break;
    case OFFSET_RECTANGLE_EP_TRAIT:
      Epaisseur = Action;
      break;
    case OFFSET_RECTANGLE_OMBRE_1:
      switch (Action)
	{
	case 0:
	  BoutTrait = 0;
	  break;
	case 1:
	  BoutTrait = 1;
	  break;
	case 2:
	  BoutTrait = 2;
	  break;
	case 3:
	  BoutTrait = 3;
	  break;
	}
      break;
    case OFFSET_RECTANGLE_OMBRE_2:
      switch (Action)
	{
	case 0:
	  ShapeTrait = '\\';
	  break;
	case 1:
	  ShapeTrait = 'h';
	  break;
	case 2:
	  ShapeTrait = 'v';
	  break;
	case 3:
	  ShapeTrait = '/';
	  break;
	}
      break;
    case OFFSET_RECTANGLE_COULEUR:
      switch (Action)
	{
	case 0:
	  break;
	case 1:
	  break;
	}
      break;
    case OFFSET_RECTANGLE_STYLE_TRAIT:
      switch (Action)
	{
	case 0:
	  TtaNewLabel (LABEL_STYLE_TRAIT,
		       PereFormeTrait,
		       TtaGetMessage(DrawMsgTable,DRW_LINE_PLAIN));
	  TypeTrait = SolidLine;
	  break;
	case 1:
	  TtaNewLabel (LABEL_STYLE_TRAIT,
		       PereFormeTrait,
		       TtaGetMessage(DrawMsgTable,DRW_LINE_DASH));
	  TypeTrait = DashedLine;
	  break;
	case 2:
	  TtaNewLabel (LABEL_STYLE_TRAIT,
		       PereFormeTrait,
		       TtaGetMessage(DrawMsgTable,DRW_LINE_DOTS));
	  TypeTrait = DottedLine;
	  break;
	}
      break;
    }
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : TraiterEvtPaletteFleche
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * ref                int             Evenement
   * typedata   int             Type du parametre
   * data               char *          Parametre
   *
   * Functionality : Traitement d'un callback
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
TraiterEvtPaletteFleche (int ref, int typedata, char *data)

#else /* __STDC__ */

void 
TraiterEvtPaletteFleche (ref, typedata, data)
     int ref, typedata;
     char *data;

#endif /* __STDC__ */

{

}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : TraiterEvtPaletteLien
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * ref                int             Evenement
   * typedata   int             Type du parametre
   * data               char *          Parametre
   *
   * Functionality : Traitement d'un callback
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
TraiterEvtPaletteLien (int ref, int typedata, char *data)

#else /* __STDC__ */

void 
TraiterEvtPaletteLien (ref, typedata, data)
     int ref, typedata;
     char *data;

#endif /* __STDC__ */

{
  int Forme;
  Element Nouveau, Frere;
  ElementType NouveauType;
  NotifyElement Event;
  boolean inside;

  Forme = (int) data;

  Frere = ChercherContexteInsert (CurrentDoc,&inside);
  if (Frere == NULL || inside)
    return;

  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  switch (Forme)
    {
    case 0:
      NouveauType.ElTypeNum = Draw3_EL_LienSimple;
      break;
    case 1:
      NouveauType.ElTypeNum = Draw3_EL_FlecheCreuse;
      Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
      TtaInsertSibling (Nouveau, Frere, FALSE,
			CurrentDoc);
      Event.document = CurrentDoc;
      Event.element = Nouveau;
      CreerUneForme (&Event);
      return;
    case 2:
      TtaAskFirstCreation ();
      NouveauType.ElTypeNum = Draw3_EL_LienComposite;
      break;
    case 3:
      NouveauType.ElTypeNum = Draw3_EL_PartieOrigineLien;
      break;
    case 4:
      NouveauType.ElTypeNum = Draw3_EL_PartieDestinationLien;
      break;
    }

  TtaSetCursorWatch (0, 0);
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  TtaInsertSibling (Nouveau, Frere, FALSE, CurrentDoc);
  Event.document = CurrentDoc;
  Event.element = Nouveau;
  CreerUnLien (&Event);
  TtaResetCursor (0, 0);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : AppliquerFeuilleRectangle
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * ref                int             Evenement
   * typedata   int             Type du parametre
   * data               char*           parametre
   *
   * Functionality : ApplyRule les parametres a une forme polygone ou rectangle
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
AppliquerFeuilleRectangle (int ref, int typedata, char *data)

#else /* __STDC__ */

void 
AppliquerFeuilleRectangle (ref, typedata, data)

#endif				/* __STDC__ */

{
  Element Box, BoiteOmbre, Label;
  DisplayMode Mode;

  switch ((int) data)
    {
    case 0:
      ElementAModifier = NULL;
      TtaDestroyDialogue (FEUILLE_RECTANGLE);
      break;
    case 1:
      if (ElementAModifier == NULL)
	return;
      Mode = TtaGetDisplayMode (CurrentDoc);
      TtaSetDisplayMode (CurrentDoc, DeferredDisplay);
      Box = TtaGetFirstChild (ElementAModifier);
      Label = Box;
      TtaNextSibling (&Label);
      BoiteOmbre = TtaGetLastChild (ElementAModifier);
      SetAttrEntier (BoiteOmbre,
		     Draw3_ATTR_SensOmbre, Ombre, CurrentDoc);
      SetPRule (Box, PRLineStyle, TypeTrait, CurrentDoc);
      SetPRule (Box, PRLineWeight, Epaisseur, CurrentDoc);
      switch (TtaGetElementType (ElementAModifier).ElTypeNum)
	{
	case Draw3_EL_Rectangle:
	  if (Bords == 'c')
	    Bords = 'C';
	  else
	    Bords = 'R';
	  break;
	case Draw3_EL_Polygone:
	  if (Bords == 'c')
	    Bords = 'S';
	  else
	    Bords = 'p';
	  break;
	}
      TtaSetGraphicsShape (Box, Bords, CurrentDoc);
      CreerOmbre (ElementAModifier, CurrentDoc);
      if (TtaGetElementType (Label).ElTypeNum ==
	  Draw3_EL_Label)
	SetAttrEntier (Label,
		       Draw3_ATTR_Position_label, PositionLabel,
		       CurrentDoc);
      TtaSetDisplayMode (CurrentDoc, Mode);
      break;
    case 2:
      break;
    }
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : AppliquerFeuilleTrait
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * ref                int             Evenement
   * typedata   int             Type du parametre
   * data               char*           parametre
   *
   * Functionality : ApplyRule les parametres a une forme polygone ou rectangle
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
AppliquerFeuilleTrait (int ref, int typedata, char *data)

#else /* __STDC__ */

void 
AppliquerFeuilleTrait (ref, typedata, data)

#endif				/* __STDC__ */

{
  Element Box;
  int H, L;
  DisplayMode Mode;

  switch ((int) data)
    {
    case 0:
      ElementAModifier = NULL;
      TtaDestroyDialogue (FEUILLE_TRAIT);
      break;
    case 1:
      if (ElementAModifier == NULL)
	return;
      Mode = TtaGetDisplayMode (CurrentDoc);
      TtaSetDisplayMode (CurrentDoc, DeferredDisplay);
      Box = TtaGetFirstChild (ElementAModifier);
      SetPRule (Box, PRLineStyle, TypeTrait, CurrentDoc);
      SetPRule (Box, PRLineWeight, Epaisseur, CurrentDoc);
      switch (BoutTrait)
	{
	case 0:
	  TtaSetGraphicsShape (Box, ShapeTrait,
			       CurrentDoc);
	  break;
	case 1:
	  switch (ShapeTrait)
	    {
	    case '/':
	      TtaSetGraphicsShape (Box, 'E',
				   CurrentDoc);
	      break;
	    case '\\':
	      TtaSetGraphicsShape (Box, 'e',
				   CurrentDoc);
	      break;
	    case 'v':
	      TtaSetGraphicsShape (Box, 'V',
				   CurrentDoc);
	      break;
	    case 'h':
	      TtaSetGraphicsShape (Box, '>',
				   CurrentDoc);
	      break;
	    }
	  break;
	case 2:
	  switch (ShapeTrait)
	    {
	    case '/':
	      TtaSetGraphicsShape (Box, 'o',
				   CurrentDoc);
	      break;
	    case '\\':
	      TtaSetGraphicsShape (Box, 'O',
				   CurrentDoc);
	      break;
	    case 'v':
	      TtaSetGraphicsShape (Box, '^',
				   CurrentDoc);
	      break;
	    case 'h':
	      TtaSetGraphicsShape (Box, '<',
				   CurrentDoc);
	      break;
	    }
	  break;
	case 3:
	  TtaSetGraphicsShape (Box, 'h', CurrentDoc);
	  TtaSetGraphicsShape (Box, 'M', CurrentDoc);
	  GetValAttrTaille (ElementAModifier, &H, &L,
			    CurrentDoc);
	  switch (ShapeTrait)
	    {
	    case '/':
	      TtaAddPointInPolyline (Box, 1, UnPixel, 
				     0, H,
				     CurrentDoc);
	      TtaAddPointInPolyline (Box, 2, UnPixel, 
				     L, 0,
				     CurrentDoc);
	      break;
	    case '\\':
	      TtaAddPointInPolyline (Box, 1, UnPixel, 
				     0, 0,
				     CurrentDoc);
	      TtaAddPointInPolyline (Box, 2, UnPixel, 
				     L, H,
				     CurrentDoc);
	      break;
	    case 'v':
	      TtaAddPointInPolyline (Box, 1, UnPixel, 
				     L / 2, 0,
				     CurrentDoc);
	      TtaAddPointInPolyline (Box, 2, UnPixel,
				     L / 2, H,
				     CurrentDoc);
	      break;
	    case 'h':
	      TtaAddPointInPolyline (Box, 1, UnPixel, 
				     0, H / 2,
				     CurrentDoc);
	      TtaAddPointInPolyline (Box, 2, UnPixel, 
				     L, H / 2,
				     CurrentDoc);
	      break;
	    }
	  TtaChangeLimitOfPolyline (Box, UnPixel, L, H,
				    CurrentDoc);
	  break;
	}
      TtaSetDisplayMode (CurrentDoc, Mode);
      break;
    case 2:
      break;
    }
  return;
}
