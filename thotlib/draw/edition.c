/*!BM
   ------------------------------------------------------------------------------
   *
   * Module      : edition.c
   * Language    : C
   * Authors     : Christian Lenne
   * Version     : %I%
   * Creation    : %E% %U%
   * Description : 
   *
   * Defined variables and functions :
   *
   *           <---------  L O C A L  --------->
   *                    AlignerElement
   *                    InsererLienGroupe
   *
   *           <------- E X P O R T E D ------->
   *                    ObjetsAligner
   *                    ObjetsGrouper
   *                    ObjetsDegrouper
   *                    ObjetsHorsGroupe
   *                    ObjetsMemeGroupe
   *                    ObjetsMemeTaille
   *                    ObjetsDupliquer
   *                    ObjetsEquidistants
   *                    ObjetsModifierPlan
   *                    CopieVersPalette
   *
   ------------------------------------------------------------------------------
   !EM */

#define Xedition

/****************************************************************************/
/*--------------------------- SYSTEM INCLUDES ------------------------------*/
/****************************************************************************/

#include "thot_gui.h"
#include "thot_sys.h"
/****************************************************************************/
/*------------------------ APPLICATION  INCLUDES ---------------------------*/
/****************************************************************************/

#include "application.h"
#include "document.h"
#include "attribute.h"
#include "language.h"
#include "content.h"
#include "reference.h"
#include "tree.h"
#include "app.h"
#include "selection.h"
#include "typebase.h"
#include "message.h"
#include "libmsg.h"

#include "edition.h"
#include "Draw3.h"
#include "Paletteactions.h"
#include "utilitaires.h"
#include "objets.h"
#include "liens.h"
#include "boutons.h"
#include "drawmsg.h"

/****************************************************************************/
/*----------------------- LOCAL DEFINES AND TYPES --------------------------*/
/****************************************************************************/

#define PREMIER_PLAN		2
#define ARRIERE_PLAN		250
#define MAXREF			40+1

/****************************************************************************/
/*--------------------------- LOCAL VARIABLES ------------------------------*/
/****************************************************************************/


/****************************************************************************/
/*-------------------------- GLOBAL VARIABLES ------------------------------*/
/****************************************************************************/
extern int LargeurObjetPalette;
extern int HauteurObjetPalette;
extern int XObjetPalette, YObjetPalette;


/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : AlignerElement
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Courant    Element         Element a aligner
   * Plan               char            Plan d'alignement (H ou V)
   * Axe                char            Axe d'alignement
   * XRef               int             X de reference
   * YRef               int             Y de reference
   * HRef               int             Hauteur de reference
   * LRef               int             Largeur de reference
   * Doc                Document        Document contenant l'element
   *
   * Functionality : Positionne l'element courant a sa nouvelle position qui sera
   * calculee en fonction de la position du premier
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

static void 
AlignerElement (Element Courant, char Plan, char Axe,
		int XRef, int YRef,
		int HRef, int LRef, Document Doc)


#else /* __STDC__ */

static void 
AlignerElement (Courant, Plan, Axe, XRef, YRef, HRef, LRef, Doc)
     Element Courant;
     char Plan, Axe;
     int XRef, YRef, HRef, LRef;
     Document Doc;

#endif /* __STDC__ */

{
  Document doc;
  int X, Y, H, L;
  View VuePrincipale;

  TtaGiveActiveView(&doc,&VuePrincipale);
  TtaGiveBoxPosition (Courant, Doc, VuePrincipale, UnPixel, &X, &Y);
  TtaGiveBoxSize (Courant, Doc, VuePrincipale, UnPixel, &L, &H);
#ifdef TRACE
  printf("AlignerElement : Pos Courant: %d, %d \n",X,Y);
  printf("AlignerElement : Size Courant: %d, %d \n",L,H);
#endif
  if (Plan == 'V')
    switch (Axe)
      {
      case 'G':
	Y = 0;
	X = XRef - X;
	break;
      case 'C':
	Y = 0;
	X = (XRef + (LRef / 2)) - (X + (L / 2));
	break;
      case 'D':
	Y = 0;
	X = (XRef + LRef) - (X + L);
	break;
      }
  else
    switch (Axe)
      {
      case 'H':
	X = 0;
	Y = YRef - Y;
	break;
      case 'C':
	X = 0;
	Y = (YRef + (HRef / 2)) - (Y + (H / 2));
	break;
      case 'B':
	X = 0;
	Y = (YRef + HRef) - (Y + H);
	break;
      }
#ifdef TRACE
  printf("AlignerElement : new pos Courant: %d, %d \n",X,Y);
  printf("\n");
#endif
  TtaChangeBoxPosition (Courant, Doc, VuePrincipale, X, Y, UnPixel);
  AjusterBoite (Courant, Doc);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : InsererLienGroupe
   * Result        : Boolean
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Lien               Element         Lien a inserer
   * Groupe     Element         Groupe dans lequel on insere le lien
   * Doc                Doocument       Document contenant les objets
   *
   * Functionality : Insere un lien dans un groupe si les deux extremites du
   * lien sont dans le groupe designe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

static Boolean 
InsererLienGroupe (Element Lien, Element Groupe, Document Doc)

#else /* __STDC__ */

static Boolean 
InsererLienGroupe (Lien, Groupe, Doc)
     Element Lien, Element Groupe;
     Document Doc;

#endif /* __STDC__ */

{ 
  Element Origine, Destination;
  Document Document;
  char DocName[1];
  AttributeType TypeAttOrigine, TypeAttDestination;
  Attribute AttO, AttD;
 
  Document = 0;
  DocName[0] = '\0';
  TypeAttOrigine.AttrSSchema =
    TypeAttDestination.AttrSSchema =
    TtaGetSSchema ("Draw3", Doc);
  if (TtaGetElementType (Lien).ElTypeNum !=
      Draw3_EL_PartieDestinationLien)
    TypeAttOrigine.AttrTypeNum = Draw3_ATTR_Origine_lien;
  else
    TypeAttOrigine.AttrTypeNum = Draw3_ATTR_Origine;
  if (TtaGetElementType (Lien).ElTypeNum !=
      Draw3_EL_PartieOrigineLien)
    TypeAttDestination.AttrTypeNum = Draw3_ATTR_Terminaison_lien;
  else
    TypeAttDestination.AttrTypeNum = Draw3_ATTR_Terminaison;
  AttO = TtaGetAttribute (Lien, TypeAttOrigine);
  AttD = TtaGetAttribute (Lien, TypeAttDestination);
  Document = 0;
  DocName[0] = '\0';
  if (AttO)
    TtaGiveReferenceAttributeValue (AttO, &Origine,
				    DocName, &Document);
  else
    Origine = NULL;

  Document = 0;
  DocName[0] = '\0';
  if (AttD)
    TtaGiveReferenceAttributeValue (AttD, &Destination,
				    DocName, &Document);
  else
    Destination = NULL;

  if ((Origine != NULL) && (Destination != NULL) &&
      (TtaGetParent (Origine) == Groupe) &&
      (TtaGetParent (Destination) == Groupe))
    return (True);
  else
    return (False);

}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : RetaillerElement
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Courant    Element         Element a aligner
   * HRef               int             Hauteur de reference
   * LRef               int             Largeur de reference
   * Doc                Document        Document contenant l'element
   *
   * Functionality : Retaille l'element courant a sa nouvelle taille qui sera
   * calculee en fonction des la position du premier
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

static void 
RetaillerElement (Element Courant, int HRef, int LRef,
		  Document Doc)


#else /* __STDC__ */

static void 
RetaillerElement (Courant, HRef, LRef, Doc)
     Element Courant;
     int HRef, LRef;
     Document Doc;

#endif /* __STDC__ */

{
  Document doc;
  int H, L;
  View VuePrincipale;

  TtaGiveActiveView(&doc,&VuePrincipale);
  TtaGiveBoxSize (Courant, Doc, VuePrincipale, UnPixel, &L, &H);
  TtaChangeBoxSize (Courant, Doc, VuePrincipale, (LRef - L), (HRef - H), UnPixel);
  AjusterBoite (Courant, Doc);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : BougerElement
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * Courant    Element         Element a aligner
   * XRef               int*            X du coin droit precedent
   * YRef               int*            Y du coin droit precedent
   * LRef               int             Hauteur de reference
   * HRef               int             Largeur de reference
   * Doc                Document        Document contenant l'element
   *
   * Functionality : Retaille l'element courant a sa nouvelle taille qui sera
   * calculee en fonction de la position du premier
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

static void 
BougerElement (Element Courant, int *XRef, int *YRef,
	       int LRef, int HRef, Document Doc)


#else /* __STDC__ */

static void 
BougerElement (Courant, XRef, YRef, LRef, HRef, Doc)
     Element Courant;
     int *XRef, *YRef, LRef, HRef;
     Document Doc;

#endif /* __STDC__ */

{
  Document doc;
  int X, Y, H, L;
  View VuePrincipale;

  TtaGiveActiveView(&doc,&VuePrincipale);
  TtaGiveBoxSize (Courant, Doc, VuePrincipale, UnPixel, &L, &H);
  TtaGiveBoxPosition (Courant, Doc, VuePrincipale, UnPixel, &X, &Y);
  TtaChangeBoxPosition (Courant, Doc, VuePrincipale, (*XRef + LRef - X),
			(*YRef + HRef - Y), UnPixel);
  *XRef = *XRef + LRef;
  *YRef = *YRef + H + HRef;
  AjusterBoite (Courant, Doc);
  return;
}

/****************************************************************************/
/*------------------------- EXPORTED  FUNCTIONS ----------------------------*/
/****************************************************************************/

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ObjetsAligner
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                  -----
   * Plan               char            'H' pour horizontal ou 'V' pour Vertical
   * Axe                char            'C' pour Centre, 'H' pour Haut, 'B' pour Bas
   *                            'G' pour Gauche ou 'D' pour Droite
   * Doc                Document        Document d'appartenance
   *
   * Functionality : Aligner les objets de la selection.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void ObjetsAligner (char Plan, char Axe, Document Doc)
#else /* __STDC__ */
void ObjetsAligner (Plan, Axe, Doc)
     char Plan;
     char Axe;
     Document Doc;
#endif /* __STDC__ */
{
  Document doc;
  Element PcFirst, Courant, Selected;
  int PosXPremier, PosYPremier, HauteurPremier, LargeurPremier;
  int PosX, PosY;
  View VuePrincipale;

  TtaGiveActiveView(&doc,&VuePrincipale);
  TtaGiveFirstSelectedElement (Doc, &Selected, &PosX, &PosY);
  PcFirst = Selected;
  if (PcFirst == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale, 
		    TtaGetMessage(LIB,LIB_DEBUG_NO_SELECTION), NULL);
      return;
    }
  while (ExistAttribute (PcFirst, Draw3_ATTR_PositionH, Doc) == NULL)
    if ((PcFirst = TtaGetParent (PcFirst)) == NULL)
      return;
  GetValAttrTaille (PcFirst, &HauteurPremier, &LargeurPremier, Doc);
  GetValAttrPosition (PcFirst, &PosXPremier, &PosYPremier, Doc);
#ifdef TRACE
  printf ("ObjetsAligner : AttrTaille PcFirst %d, %d \n",HauteurPremier,LargeurPremier);
  printf ("ObjetsAligner : AtPosition PcFirst %d, %d \n",PosXPremier,PosYPremier);
#endif
  TtaGiveNextSelectedElement (Doc, &Selected, &PosX, &PosY);
  Courant = Selected;
  while (Courant != NULL)
    {
      TtaGiveNextSelectedElement (Doc, &Selected, &PosX, &PosY);
      while (ExistAttribute (Courant, Draw3_ATTR_PositionH, Doc)
	     == NULL)
	if ((Courant = TtaGetParent (Courant)) == NULL)
	  continue;
      AlignerElement (Courant, Plan, Axe, PosXPremier, PosYPremier,
		      HauteurPremier, LargeurPremier, Doc);
      Courant = Selected;
    }
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ObjetsGrouper
   * Result        : void
   *
   * Functionality : Grouper les objets de la selection. On separe les formes des
   * liens. Le groupage se fait en 3 passes.
   *     1) On met dans le groupe les objets.
   *     2) On rajoute les liens selectionnes, seulement si les objets
   *     references sont tous les deux dans le groupe.
   *     3) On rajoute les liens entre objets du groupe, meme si ils ne sont
   *     pas selectionnes.
   *
   ------------------------------------------------------------------------------
   !EF */
#ifdef __STDC__
void ObjetsGrouper (Document Doc)
#else /* __STDC__ */
void ObjetsGrouper (Doc)
     Document Doc;
#endif /* __STDC__ */
{
  Element Courant, Selected, Groupe, Fleche, LeGroupe;
  int X, Y, XMin, YMin, Plan, PlanMax;
  Document Document;
  Attribute Att;
  View VuePrincipale;
  NotifyElement Target;
  Element TabSelectObjet[MAXREF], TabSelectLien[MAXREF];
  int i, j, MaxRefObjet, MaxRefLien;

  TtaGiveActiveView(&Document,&VuePrincipale);

  TtaGiveFirstSelectedElement (Doc, &Selected, &X, &Y);
  
  Courant = Selected;
  if (Courant == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale,
		    TtaGetMessage(LIB,LIB_DEBUG_NO_SELECTION), NULL);
      return;
    }
  TabSelectObjet[0] = TabSelectLien[0] = NULL;

  for (i = j = 1; (i < MAXREF) && (j < MAXREF) && (Selected != NULL);)
    {
      while ((Selected !=NULL) &&
	     (ExistAttribute (Selected, Draw3_ATTR_PositionH, Doc) == NULL))
	Selected = TtaGetParent (Selected);
      if (Selected == NULL)
	continue;

      /* On ventile suivant le critere objet ou lien */

      switch (TtaGetElementType (Selected).ElTypeNum)
	{
	case Draw3_EL_LienSimple:
	case Draw3_EL_FlecheCreuse:
	case Draw3_EL_PartieOrigineLien:
	case Draw3_EL_PartieDestinationLien:
	  TabSelectLien[j] = Selected;
	  j++;
	  break;
	default:
	  TabSelectObjet[i] = Selected;
	  i++;
	  break;
	}
      TtaGiveNextSelectedElement (Doc, &Courant, &X, &Y);
      Selected = Courant;
    }

  MaxRefObjet = --i;
  MaxRefLien = --j;

  TtaSelectElement (Doc, NULL);
  TtaSetDisplayMode (Doc, DeferredDisplay);

  Courant = Selected = TabSelectObjet[i--];

  AjouterUnNoeud (Selected, Doc, &Groupe, Draw3_EL_Groupe);
  LeGroupe = Groupe;
  XMin = YMin = 32000;
  Plan = 0;
  while (Courant != NULL)
    {
      Selected = TabSelectObjet[i--];
      AjusterBoite (Courant, Doc);
      GetValAttrPosition (Courant, &X, &Y, Doc);
      if (X < XMin)
	XMin = X;
      if (Y < YMin)
	YMin = Y;
      Plan = GetValAttrEntier (Courant, Draw3_ATTR_Plan, Doc);
      if (Plan > PlanMax)
	PlanMax = Plan;
      Courant = Selected;
    }

  Courant = Selected = TabSelectLien[j--];

  SetAttrPosition (Groupe, XMin, YMin, Doc);
  SetAttrEntier (Groupe, Draw3_ATTR_Plan, Plan + 5, Doc);
  Groupe = TtaGetFirstChild (Groupe);
  i = MaxRefObjet;
  Selected = TabSelectObjet[i--];
  Courant = Selected;

/* On groupe la liste des objets */

  while (Courant != NULL)
    {
      Selected = TabSelectObjet[i--];
      AjusterBoite (Courant, Doc);
      GetValAttrPosition (Courant, &X, &Y, Doc);
      TtaRemoveTree (Courant, Doc);
      TtaInsertSibling (Courant, Groupe, True, Doc);
      SetAttrPosition (Courant, X - XMin, Y - YMin, Doc);
      Courant = Selected;
    }

  i = MaxRefLien;
  Selected = TabSelectLien[i--];
  Courant = Selected;

/* On groupe les liens selectionnes */

  while (Courant != NULL)
    {
      Selected = TabSelectLien[i--];
      if (InsererLienGroupe (Courant, LeGroupe, Doc) == True)
	{
	  AjusterBoite (Courant, Doc);
	  GetValAttrPosition (Courant, &X, &Y, Doc);
	  TtaRemoveTree (Courant, Doc);
	  TtaInsertSibling (Courant, Groupe, True, Doc);
	  SetAttrPosition (Courant, X - XMin, Y - YMin, Doc);
	  Target.element = Courant;
	  Target.document = Doc;
	  RePositionnerLien (&Target);
	}
      Courant = Selected;
    }

  i = MaxRefObjet;
  Courant = TabSelectObjet[i--];

/* On rajoute les liens non selectionnes entre les objets du groupe */

  while (Courant != NULL)
    {
      if (TtaIsElementReferred (Courant))
	{
	  Document = 0;
	  Att = NULL;
	  Fleche = NULL;
	  TtaNextLoadedReference (Courant, Doc, &Fleche, &Att,
				  &Document);
	  while (Fleche != NULL)
	    {
	      /* On s'assure que le lien n'etait pas deja
	         selectionne */
	      if (LeGroupe != TtaGetParent (Fleche))
		{
		  if (InsererLienGroupe (Fleche, LeGroupe, Doc)
		      == True)
		    {
/* 
   AjusterBoite (Fleche, Doc);
   GetValAttrPosition (Fleche, &X, &Y,Doc);
   SetAttrPosition (Fleche, X - XMin, Y - YMin, Doc);
   */
		      TtaRemoveTree (Fleche, Doc);
		      TtaInsertSibling (Fleche, Groupe, True,
					Doc);
		      Target.element = Fleche;
		      Target.document = Doc;
		      RePositionnerLien (&Target);
		    }
		}
	      TtaNextLoadedReference (Courant, Doc, &Fleche,
				      &Att, &Document);
	    }
	}
      Courant = TabSelectObjet[i--];
    }
  TtaDeleteTree (Groupe, Doc);
  TtaSetDisplayMode (Doc, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ObjetsDegrouper
   * Result        : void
   *
   * Functionality : Degrouper les objets de la selection.
   *
   ------------------------------------------------------------------------------
   !EF */
#ifdef __STDC__
void ObjetsDegrouper (Document Doc)
#else /* __STDC__ */
void ObjetsDegrouper (Doc)
     Document Doc;
#endif /* __STDC__ */
{
  Element Courant, Selected, Groupe, Fleche;
  int X, Y, XMin, YMin, First;
  Document Document;
  Attribute Att;
  View VuePrincipale;
  NotifyElement Target;

  TtaGiveActiveView(&Document,&VuePrincipale);

  TtaGiveFirstSelectedElement (Doc, &Selected, &X, &Y);
  Courant = Selected;
  if (Courant == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale, 
		    TtaGetMessage(LIB,LIB_DEBUG_NO_SELECTION), NULL);
      return;
    }

  while ((Selected!=NULL) &&
	 ExistAttribute (Selected, Draw3_ATTR_PositionH, Doc) == NULL)
    Selected = TtaGetParent (Selected);
  if (Selected == NULL)
      return;

  if (TtaGetElementType (Selected).ElTypeNum != Draw3_EL_Groupe)
    Groupe = TtaGetParent (Selected);
  else
    Groupe = Selected;

  if (TtaGetElementType (Groupe).ElTypeNum != Draw3_EL_Groupe)
    {
      TtaSetStatus (Doc, VuePrincipale,
		   TtaGetMessage(DrawMsgTable,DRW_SELECT_GROUP_FIRST),
		    NULL);
      return;
    }

  GetValAttrPosition (Groupe, &XMin, &YMin, Doc);

  Courant = TtaGetFirstChild (Groupe);
  TtaSelectElement (Doc, NULL);
  First = 1;
  while (Courant != NULL)
    {
      GetValAttrPosition (Courant, &X, &Y, Doc);
      TtaRemoveTree (Courant, Doc);
      TtaInsertSibling (Courant, Groupe, True, Doc);
      SetAttrPosition (Courant, X + XMin, Y + YMin, Doc);
      if (TtaIsElementReferred (Courant))
	{
	  Document = 0;
	  Att = NULL;
	  Fleche = NULL;
	  TtaNextLoadedReference (Courant, Doc, &Fleche, &Att, &Document);
	  while (Fleche != NULL)
	    {
	      Target.element = Fleche;
	      Target.document = Doc;
	      if (TtaGetParent (Fleche) != Groupe)
		{
		  TtaRemoveTree (Fleche, Doc);
		  TtaInsertSibling (Fleche, Groupe, True, Doc);
		  RePositionnerLien (&Target);
		}
	      else
		RePositionnerLien (&Target);
	      TtaNextLoadedReference (Courant, Doc, &Fleche, &Att,
				      &Document);
	    }
	}
      Courant = TtaGetFirstChild (Groupe);
    }

  TtaDeleteTree (Groupe, Doc);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ObjetsHorsGroupe
   * Result        : void
   *
   * Functionality : HorsGrouper les objets de la selection.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void ObjetsHorsGroupe (Document Doc)
#else /* __STDC__ */
void ObjetsHorsGroupe (Doc)
     Document Doc;
#endif /* __STDC__ */

{
  Element Courant, Selected, Groupe, Fleche;
  int X, Y, XMin, YMin;
  Document Document;
  Attribute Att;
  View VuePrincipale;
  NotifyElement Target;

  TtaGiveActiveView(&Document,&VuePrincipale);

  TtaGiveFirstSelectedElement (Doc, &Selected, &X, &Y);
  Courant = Selected;
  if (Courant == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale, TtaGetMessage(LIB,LIB_DEBUG_NO_SELECTION), NULL);
      return;
    }

  while (Courant != NULL)
    {
      TtaGiveNextSelectedElement (Doc, &Selected, &X, &Y);
      while ((Courant!=NULL) &&
	     ExistAttribute (Courant, Draw3_ATTR_PositionH, Doc) == NULL)
	Courant = TtaGetParent (Courant);
      if(Courant == NULL)
	return;
      Groupe = TtaGetParent (Courant);
      if (TtaGetElementType (Groupe).ElTypeNum == Draw3_EL_Groupe)
	{
	  GetValAttrPosition (Groupe, &XMin, &YMin, Doc);
	  AjusterBoite (Courant, Doc);
	  GetValAttrPosition (Courant, &X, &Y, Doc);
	  TtaRemoveTree (Courant, Doc);
	  TtaInsertSibling (Courant, Groupe, True, Doc);
	  SetAttrPosition (Courant, X + XMin, Y + YMin, Doc);
	  if (TtaIsElementReferred (Courant))
	    {
	      Document = 0;
	      Att = NULL;
	      Fleche = NULL;
	      TtaNextLoadedReference (Courant, Doc,
				      &Fleche, &Att, &Document);
	      while (Fleche != NULL)
		{
		  if (TtaGetParent (Fleche) != Groupe)
		    {
		      TtaRemoveTree (Fleche, Doc);
		      TtaInsertSibling (Fleche, Groupe,
					True, Doc);
		      Target.element = Fleche;
		      Target.document = Doc;
		      RePositionnerLien (&Target);
		    }
		  else
		    RePositionnerLien (&Target);
		  TtaNextLoadedReference (Courant, Doc, &Fleche,
					  &Att, &Document);
		}
	    }
	}
      Courant = Selected;
    }
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ObjetsMemeGroupe
   * Result        : void
   *
   * Functionality : MemeGrouper les objets de la selection.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
ObjetsMemeGroupe (Document Doc)

#else /* __STDC__ */

void 
ObjetsMemeGroupe (Doc)
     Document Doc;

#endif /* __STDC__ */

{
  Element Courant, Selected, Groupe, Fleche;
  int X, Y, XMin, YMin;
  Document Document;
  Attribute Att;
  View VuePrincipale;
  NotifyElement Target;

  TtaGiveActiveView(&Document,&VuePrincipale);

  TtaGiveFirstSelectedElement (Doc, &Selected, &X, &Y);
  Courant = Selected;
  if (Courant == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale,
		    TtaGetMessage(LIB,LIB_DEBUG_NO_SELECTION), NULL);
      return;
    }

  while (ExistAttribute (Selected, Draw3_ATTR_PositionH, Doc) == NULL)
    if ((Selected = TtaGetParent (Selected)) == NULL)
      return;

  if (TtaGetElementType (Selected).ElTypeNum != Draw3_EL_Groupe)
    Groupe = TtaGetParent (Selected);
  else
    Groupe = Selected;

  if (TtaGetElementType (Groupe).ElTypeNum != Draw3_EL_Groupe)
    {
      TtaSetStatus (Doc, VuePrincipale,
		    TtaGetMessage(DrawMsgTable,DRW_SELECT_GROUP_FIRST),
		    NULL);
      return;
    }

  GetValAttrPosition (Groupe, &XMin, &YMin, Doc);

  TtaGiveNextSelectedElement (Doc, &Selected, &X, &Y);
  Courant = Selected;

  Groupe = TtaGetFirstChild (Groupe);

  while (Courant != NULL)
    {
      TtaGiveNextSelectedElement (Doc, &Selected, &X, &Y);
      while ((Courant = NULL) && 
	     (ExistAttribute (Courant, Draw3_ATTR_PositionH, Doc) == NULL))
	Courant = TtaGetParent(Courant);
      if (Courant == NULL) {
	Courant = Selected;
	continue;
      }
      AjusterBoite (Courant, Doc);
      GetValAttrPosition (Courant, &X, &Y, Doc);
      TtaRemoveTree (Courant, Doc);
      SetAttrPosition (Courant, X - XMin, Y - YMin, Doc);
      TtaInsertSibling (Courant, Groupe, True, Doc);
      if (TtaIsElementReferred (Courant))
	{
	  Document = 0;
	  Att = NULL;
	  Fleche = NULL;
	  TtaNextLoadedReference (Courant, Doc, &Fleche, &Att, &Document);
	  while (Fleche != NULL)
	    {
	      Target.element = Fleche;
	      Target.document = Doc;
	      if (TtaGetParent (Fleche) != TtaGetParent (Courant))
		{
		  TtaRemoveTree (Fleche, Doc);
		  TtaInsertSibling (Fleche, Groupe, True, Doc);
		  RePositionnerLien (&Target);
		}
	      else
		RePositionnerLien (&Target);
	      TtaNextLoadedReference (Courant, Doc, &Fleche, &Att,
				      &Document);
	    }
	}
      Courant = Selected;
    }
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ObjetsMemeTaille
   * Result        : void
   *
   * Functionality : Affecte la meme taille que celle du premier aux objets de
   * la selection.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
ObjetsMemeTaille (Document Doc)

#else /* __STDC__ */

void 
ObjetsMemeTaille (Doc)
     Document Doc;
#endif /* __STDC__ */

{
  Element PcFirst, Courant, Selected;
  int PosX, PosY, HauteurPremier, LargeurPremier;
  View VuePrincipale;
  Document doc;

  TtaGiveActiveView(&doc,&VuePrincipale);
  TtaGiveFirstSelectedElement (Doc, &Selected, &PosX, &PosY);
  PcFirst = Selected;
  if (PcFirst == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale, TtaGetMessage(LIB,LIB_DEBUG_NO_SELECTION), NULL);
      return;
    }
  while (ExistAttribute (PcFirst, Draw3_ATTR_PositionH, Doc) == NULL)
    if ((PcFirst = TtaGetParent (PcFirst)) == NULL)
      return;
  AjusterBoite (PcFirst, Doc);
  GetValAttrTaille (PcFirst, &HauteurPremier, &LargeurPremier, Doc);

  TtaGiveNextSelectedElement (Doc, &Selected, &PosX, &PosY);
  Courant = Selected;
  while (Courant != NULL)
    {
      TtaGiveNextSelectedElement (Doc, &Selected, &PosX, &PosY);
      while ((Courant != NULL) &&
	     (ExistAttribute (Courant, Draw3_ATTR_PositionH, Doc)== NULL))
	Courant = TtaGetParent (Courant);
      if (Courant != NULL) 
	RetaillerElement (Courant, HauteurPremier, LargeurPremier, Doc);
      Courant = Selected;
    }

}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ObjetsDupliquer
   * Result        : void
   *
   * Functionality : Duplique les objets de la selection et les cree legerement
   * decale.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void ObjetsDupliquer (Document Doc)
#else /* __STDC__ */
void ObjetsDupliquer (Doc)
     Document Doc;
#endif /* __STDC__ */
{
  Element FnCopy, Courant, Selected;
  int X, Y;
  View VuePrincipale;
  Document doc;

  TtaGiveActiveView(&doc,&VuePrincipale);

  TtaGiveFirstSelectedElement (Doc, &Selected, &X, &Y);
  Courant = Selected;
  if (Courant == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale, TtaGetMessage(LIB,LIB_DEBUG_NO_SELECTION), NULL);
      return;
    }

  while (Courant != NULL)
    {
      TtaGiveNextSelectedElement (Doc, &Selected, &X, &Y);
      while ((Courant !=NULL)&&
	     (ExistAttribute (Courant, Draw3_ATTR_PositionH, Doc) == NULL))
	Courant = TtaGetParent (Courant);
      if (Courant != NULL) {
	AjusterBoite (Courant, Doc);
	GetValAttrPosition (Courant, &X, &Y, Doc);
	FnCopy = TtaCopyTree (Courant, Doc, Doc, TtaGetParent (Courant));
	TtaInsertSibling (FnCopy, Courant, False, Doc);
	TtaChangeBoxPosition (FnCopy, Doc, VuePrincipale, 15, 15, UnPixel);
	AjusterBoite (FnCopy, Doc);
      }
      Courant = Selected;
    }

}

/*!BF
   ------------------------------------------------------------------------------
   *
   * function      : ObjetsEquidistants
   * Result        : void
   *
   * Functionality : Placce les objets a un intervalle regulier fixe par les 2
   * premiers elements de la selection.
   *
   ------------------------------------------------------------------------------
   !EF */
#ifdef __STDC__
void ObjetsEquidistants (Document Doc)
#else /* __STDC__ */
void ObjetsEquidistants (Doc)
     Document Doc;
#endif /* __STDC__ */
{
  Element PcFirst, Second, Courant, Selected;
  int X1, Y1, H1, L1, X2, Y2, H2, L2;
  int X, Y;
  View VuePrincipale;
  Document doc;

  TtaGiveActiveView(&doc,&VuePrincipale);
  TtaGiveFirstSelectedElement (Doc, &Selected, &X, &Y);
  PcFirst = Selected;
  if (PcFirst == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale,
		    TtaGetMessage(LIB,LIB_DEBUG_NO_SELECTION), NULL);
      return;
    }
  while (ExistAttribute (PcFirst, Draw3_ATTR_PositionH, Doc) == NULL)
    if ((PcFirst = TtaGetParent (PcFirst)) == NULL)
      return;
  AjusterBoite (PcFirst, Doc);
  GetValAttrTaille (PcFirst, &H1, &L1, Doc);
  GetValAttrPosition (PcFirst, &X1, &Y1, Doc);

  TtaGiveNextSelectedElement (Doc, &Selected, &X, &Y);
  Second = Selected;
  if (Second == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale, 
		    TtaGetMessage(DrawMsgTable,DRW_SELECT_2),NULL);
      return;
    }
  while (ExistAttribute (Second, Draw3_ATTR_PositionH, Doc) == NULL)
    if ((Second = TtaGetParent (Second)) == NULL)
      return;

  AjusterBoite (Second, Doc);
  GetValAttrTaille (Second, &H2, &L2, Doc);
  GetValAttrPosition (Second, &X2, &Y2, Doc);

  L1 = X2 - X1;
  H1 = Y2 - (Y1 + H1);
  Y2 = Y2 + H2;

  TtaGiveNextSelectedElement (Doc, &Selected, &X, &Y);
  Courant = Selected;
  while (Courant != NULL)
    {
      TtaGiveNextSelectedElement (Doc, &Selected, &X, &Y);
      while ((Courant != NULL) && 
	     (ExistAttribute (Courant, Draw3_ATTR_PositionH, Doc) == NULL))
	Courant = TtaGetParent (Courant);
      if (Courant != NULL) {
	AjusterBoite (Courant, Doc);
	BougerElement (Courant, &X2, &Y2, L1, H1, Doc);
      }
      Courant = Selected;
    }
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ObjetsModifierPlan
   * Result        : void
   *
   * Functionality : Duplique les objets de la selection et les cree legerement
   * decale.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void ObjetsModifierPlan (char Plan, Document Doc)
#else /* __STDC__ */
void ObjetsModifierPlan (Plan, Doc)
     char Plan;
     Document Doc;
#endif /* __STDC__ */

{
  Element Courant, Selected;
  int Profondeur, X, Y;
  View VuePrincipale;
  Document doc;

  TtaGiveActiveView(&doc,&VuePrincipale);
  TtaGiveFirstSelectedElement (Doc, &Selected, &X, &Y);
  Courant = Selected;
  if (Courant == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale,
		    TtaGetMessage(LIB,LIB_DEBUG_NO_SELECTION), NULL);
      return;
    }

  while (Courant != NULL)
    {
      TtaGiveNextSelectedElement (Doc, &Selected, &X, &Y);

      while ((Courant != NULL) &&
	     (ExistAttribute (Courant, Draw3_ATTR_PositionH, Doc) == NULL))
	Courant= TtaGetParent (Courant);
      if (Courant == NULL) {
	Courant = Selected;
	continue;
      }
      if (ExistAttribute (Courant, Draw3_ATTR_Plan, Doc) == NULL)
	{
	  CreateAnAttribute (Courant, Draw3_ATTR_Plan, 0, Doc);
	  Profondeur = 0;
	}
      else
	Profondeur = GetValAttrEntier (Courant, Draw3_ATTR_Plan,
				       Doc);
      switch (Plan)
	{
	case 'P':
	  SetAttrEntier (Courant, Draw3_ATTR_Plan,
			 PREMIER_PLAN, Doc);
	  break;
	case 'D':
	  SetAttrEntier (Courant, Draw3_ATTR_Plan,
			 ARRIERE_PLAN, Doc);
	  break;
	case 'E':
	  SetAttrEntier (Courant, Draw3_ATTR_Plan,
			 Profondeur + 5, Doc);
	  break;
	case 'R':
	  Profondeur = Profondeur - 5;
	  if (Profondeur < 0)
	    Profondeur = PREMIER_PLAN;
	  SetAttrEntier (Courant, Draw3_ATTR_Plan,
			 Profondeur, Doc);
	  break;
	}
      Courant = Selected;
    }

}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CopieVersPalette
   * Result        : void
   *
   * Functionality : Duplique les objets de la selection et les cree legerement
   * decale.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CopieVersPalette (Document Doc)

#else /* __STDC__ */

void 
CopieVersPalette (Doc)
     Document Doc;

#endif /* __STDC__ */

{
  Element FnCopy, Courant, Selected, PerePalette, FrerePalette;
  int X, Y;
  View VuePrincipale;
  DisplayMode etat, etatSource;
  Document doc;

  TtaGiveActiveView(&doc,&VuePrincipale);
  TtaGiveFirstSelectedElement (Doc, &Selected, &X, &Y);
  Courant = Selected;
  if (Courant == NULL)
    {
      TtaSetStatus (Doc, VuePrincipale,
		    TtaGetMessage(LIB,LIB_DEBUG_NO_SELECTION), NULL);
      return;
    }

  if (DocPalette == 0)
    if(!ChargerPalette ())
      {
	TtaSetStatus (Doc, VuePrincipale,
		      TtaGetMessage(DrawMsgTable,DRW_CANNOT_LOAD_PAL),
		      NULL);
	return;
      }
  PerePalette = TtaGetFirstChild(TtaGetMainRoot (DocPalette));
  FrerePalette = TtaGetLastChild (PerePalette);
  etat = TtaGetDisplayMode (DocPalette);
  etatSource = TtaGetDisplayMode (Doc);
  TtaSetDisplayMode (DocPalette, DeferredDisplay);
  TtaSetDisplayMode (Doc, DeferredDisplay);

  while (Courant != NULL)
    {
      TtaGiveNextSelectedElement (Doc, &Selected, &X, &Y);
      while ((Courant!= NULL) &&
	     (ExistAttribute (Courant, Draw3_ATTR_PositionH, Doc) == NULL))
	Courant = TtaGetParent (Courant);
      if (Courant != NULL) {
	AjusterBoite (Courant, Doc);
	GetValAttrPosition (Courant, &X, &Y, Doc);
	FnCopy = TtaCopyTree (Courant, Doc, DocPalette, PerePalette);
	TtaInsertSibling (FnCopy, FrerePalette, True, DocPalette);
	if (TtaGetElementType (FnCopy).ElTypeNum == Draw3_EL_Groupe)
	  {
	    GetValAttrTaille (Courant, &X, &Y, Doc);
	    RetaillerGroupe (FnCopy, X, HauteurObjetPalette,
			     Y, LargeurObjetPalette,
			     DocPalette, 1);
	  }
	SetAttrTaille (FnCopy, HauteurObjetPalette, LargeurObjetPalette,
		       DocPalette);
	/*   SetAttrPosition (FnCopy, XObjetPalette, YObjetPalette, DocPalette); */
	/*       if (XObjetPalette == 10) */
	/* 	XObjetPalette = XObjetPalette + LargeurObjetPalette + 5; */
	/*       else */
	/* 	{ */
	/* 	  XObjetPalette = 10; */
	/* 	  YObjetPalette = YObjetPalette + HauteurObjetPalette + 5; */
	/* 	} */
      }
      Courant = Selected;
    }
  TtaSetDisplayMode (DocPalette, DisplayImmediately);
  TtaSetDisplayMode (DocPalette, etat);
  TtaSetDisplayMode (Doc, etatSource);
  return;
}

void ResizeGroup (Document doc, View view)
{
  int ah, nh, al, nl;
  Element Selected;
  
  TtaGiveFirstSelectedElement (doc, &Selected, &ah, &nh);
  GetValAttrTaille (Selected, &ah, &al, doc);
  TtaGiveBoxSize (Selected, doc, view, UnPixel, &nl, &nh);
  RetaillerGroupe (Selected, ah, nh, al, nl, doc, view);
}

