
/*!BM
   ------------------------------------------------------------------------------
   *
   * Module      : Draw3actions.c
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
   *                    DrawSelectPre
   *                    CreateAttributes
   *                    DeleteAnElement
   *                    CreerUneForme
   *                    CreerUnGraphique
   *                    GraphModify
   *                    ModifyPRule
   *                    CreerUnLien
   *                    DeleteALien
   *                    ModifyAttribute
   *                    ModifyAttributeDim
   *                    ModifyAttributePos
   *                    ModifAttributsRectangle
   *                    NewDraw
   *                    DeleteDraw
   *
   ------------------------------------------------------------------------------
   !EM */

#define XDraw3actions

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

#include "Draw3.h"
#include "Draw3actions.h"
#include "menus.h"
#include "objets.h"
#include "liens.h"
#include "utilitaires.h"
#include "boutons.h"
#include "initdraw.h"


/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : DrawSelectPre
   * Result        : boolean
   * Parameters
   * Name          Type         Usage
   * ----          ----         -----
   * event              NotifyElement*  Evenement ayant declanche ECF
   *
   * Functionality : Memorise le document englobant l'element.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
boolean DrawSelectPre (NotifyElement * event)
#else /* __STDC__ */
boolean DrawSelectPre (event)
     NotifyElement *event;
#endif /* __STDC__ */
{
  if (event->document==DocPalette) {
    PalActivate(event);
    return True;
  } else {
    CurrentDoc = event->document;
    CurrentSelect = event->element;
    return False;
  }
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreateAttributes
   * Result        : void
   * Parameters
   * Name          Type         Usage
   * ----          ----         -----
   * event              NotifyElement*  Evenement ayant declanche ECF
   *
   * Functionality : Non utile encore dans le contexte
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void CreateAttributes (NotifyElement * event)
#else /* __STDC__ */
void CreateAttributes (event)
     NotifyElement *event;
#endif /* __STDC__ */
{
  /* This function has to be written */
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : DeleteAnElement
   * Result        : boolean
   * Parameters
   * Name          Type         Usage
   * ----          ----         -----
   * event              NotifyElement*  Evenement ayant declanche ECF
   *
   * Functionality : Suppression d'un element. On regarde quel est le type de
   * l'element. Les problemes viennent des liens composites ou des objets pointes
   * par des liens.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
boolean DeleteAnElement (NotifyElement * event)
#else /* __STDC__ */
boolean DeleteAnElement (event)
     NotifyElement *event;
#endif /* __STDC__ */
{
  Document Doc;
  Attribute Att;
  Element Fleche, Courant;
  NotifyElement Target;

  TtaSetDisplayMode (event->document, DeferredDisplay);
  Courant = event->element;
  if (TtaGetElementType (Courant).ElTypeNum == Draw3_EL_GRAPHICS_UNIT)
    Courant = TtaGetParent (Courant);
  if (TtaIsElementReferred (Courant))
    {
      Doc = 0;
      Att = NULL;
      Fleche = NULL;
      TtaNextLoadedReference (Courant,
			      event->document,
			      &Fleche, &Att, &Doc);
      while (Fleche != NULL)
	{
	  switch (TtaGetElementType (Fleche).ElTypeNum)
	    {
	    case Draw3_EL_LienSimple:
	    case Draw3_EL_PartieOrigineLien:
	    case Draw3_EL_PartieDestinationLien:
	      Target.element = Fleche;
	      Target.document = Doc;
	      DeleteALien (&Target);
	      break;
	    default:
	      TtaDeleteTree (Fleche,
			     event->document);
	      break;
	    }
	  Doc = 0;
	  Att = NULL;
	  Fleche = NULL;
	  TtaNextLoadedReference(Courant,
				 event->document,
				 &Fleche, &Att, &Doc);
	}
    }
  TtaDeleteTree (Courant, event->document);
  TtaSetDisplayMode (event->document, DisplayImmediately);

  return True;			/* let Thot perform normal operation */
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerUneForme
   * Result        : boolean
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * event              NotifyElement*  Evenement ayant declanche l'ECF
   *
   * Functionality : La forme generale a ete cree, il faut maintenant lui affecter
   * une ombre et des valeurs d'attributs par defaut (position et dimension).
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
boolean CreerUneForme (NotifyElement * event)
#else /* __STDC__ */
boolean CreerUneForme (event)
     NotifyElement *event;
#endif /* __STDC__ */

{
  Element GraphicNode, Ombre;
  char shape, type;

  shape = '\0';
  TtaSetDisplayMode (event->document, DeferredDisplay);
  GraphicNode = TtaGetFirstChild (event->element);
  Ombre = TtaGetFirstChild (TtaGetLastChild (event->element));

  switch (TtaGetElementType (event->element).ElTypeNum)
    {
    case Draw3_EL_Rectangle:
      shape = 'R';
      break;
    case Draw3_EL_Ellipse:
      shape = 'c';
      break;
    case Draw3_EL_Trait:
      shape = DetermineSens (GraphicNode, 'h',
			     event->document);
      break;
    case Draw3_EL_Fleche:
      shape = 'p';
      break;
    case Draw3_EL_FlecheCreuse:
      shape = 'p';
      type = 'l';
      break;
    case Draw3_EL_Polygone:
      shape = 'p';
      type = 'W';
      break;
    }

  if (shape != '\0')
    {
      if (shape == 'p')
	switch (type)
	  {
	  case 'l': /*fleche creuse */
	    TtaSetGraphicsShape (GraphicNode, shape,
				 event->document);
	    PositionnerFleche (event);
	    break;
	  case 'W':/* polygone */
	    InsertChar ('p',1);
	    TtaSetGraphicsShape (Ombre, type,
				 event->document);
	    break;
	  default:
	    TtaSetGraphicsShape (GraphicNode, shape,
				 event->document);
	    CreerOmbre (event->element,
			event->document);
	    break;
	  }
      else
	{
	  TtaSetGraphicsShape (GraphicNode, shape,
			       event->document);
	  if (TtaGetElementType (event->element).ElTypeNum !=
	      Draw3_EL_Trait)
	    TtaSetGraphicsShape (Ombre, shape,
				 event->document);
	}
    }
  SetPRule (GraphicNode, PRLineWeight, 2, event->document);
  SetPRule (GraphicNode, PRLineStyle, SolidLine, event->document);
  if (TtaGetElementType (event->element).ElTypeNum != Draw3_EL_Trait)
    {
      SetPRule (Ombre, PRLineWeight, 2, event->document);
      SetPRule (Ombre, PRLineStyle, SolidLine, event->document);
    }

  TtaSetDisplayMode (event->document, DisplayImmediately);

  return (False);		/* let Thot perform normal operation */
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : GraphModify
   * Result        : boolean
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * event              NotifyElement*  
   *
   * Functionality : Non utile encore dans le contexte
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
boolean GraphModify (NotifyOnValue * event)
#else /* __STDC__ */
boolean GraphModify (event)
     NotifyOnValue *event;
#endif /* __STDC__ */

{
  /* This function has to be written */
  return False;			/* let Thot perform normal operation */
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ModifyPRule
   * Result        : boolean
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * event              NotifyElement*  Evenement ayant declanche l'ECF
   *
   * Functionality : Non utile encore dans le contexte
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void ModifyPRule (NotifyPresentation * event)
#else /* __STDC__ */
void ModifyPRule (event)
     NotifyPresentation *event;
#endif /* __STDC__ */
{
  /* This function has to be written */
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerUnLien
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * event              NotifyElement*  Evenement ayant declanche l'ECF
   *
   * Functionality : Cree la forme graphique du lien
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void CreerUnLien (NotifyElement * event)
#else /* __STDC__ */
void CreerUnLien (event)
     NotifyElement *event;
#endif /* __STDC__ */

{
  Document doc;
  Element GraphicNode;
  int h, l;
  View VuePrincipale;

  TtaSetDisplayMode (event->document, DeferredDisplay);
  switch (TtaGetElementType (event->element).ElTypeNum)
    {
    case Draw3_EL_LienSimple:
      PositionnerLien (event);
      break;
    case Draw3_EL_LienComposite:
      TtaSetDisplayMode (event->document,
			 DisplayImmediately);
      GraphicNode = TtaGetFirstChild (event->element);
      TtaGiveActiveView(&doc,&VuePrincipale);
      if (!VuePrincipale)
	return;
      TtaGiveBoxSize (event->element, event->document, VuePrincipale,
		      UnPixel, &l, &h);
      if (l >= h)
	TtaSetGraphicsShape (GraphicNode, 'h',
			     event->document);
      else
	TtaSetGraphicsShape (GraphicNode, 'v',
			     event->document);
      break;
    case Draw3_EL_PartieOrigineLien:
      GraphicNode = TtaGetFirstChild (event->element);
      TtaSetGraphicsShape (GraphicNode, 'S',
			   event->document);
      TracerOrigineLien (event->element,
			 event->document);
      break;
    case Draw3_EL_PartieDestinationLien:
      GraphicNode = TtaGetFirstChild (event->element);
      TtaSetGraphicsShape (GraphicNode, 'S',
			   event->document);
      TracerDestinationLien (event->element,
			     event->document);
      break;
    }
  TtaSetDisplayMode (event->document, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : DeleteALien
   * Result        : boolean
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * event              NotifyElement*  Evenement ayant declanche l'ECF
   *
   * Functionality : Supprime le lien et tout ce qui doit etre detruit sur un lien
   * composite. Lance le retaillage de ces derniers.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
boolean DeleteALien (NotifyElement * event)
#else /* __STDC__ */
boolean DeleteALien (event)
     NotifyElement *event;
#endif /* __STDC__ */
{
  Element Lien;
  Attribute AttReference;
  AttributeType AttRef;
  Document TargetDoc = 0;
  char DocName[50];
  DisplayMode mode;

  mode = TtaGetDisplayMode (event->document);
  TtaSetDisplayMode (event->document, DeferredDisplay);
  switch (TtaGetElementType (event->element).ElTypeNum)
    {
    case Draw3_EL_LienSimple:
      TtaDeleteTree (event->element, event->document);
      TtaSetDisplayMode (event->document, mode);
      return True;
    case Draw3_EL_PartieOrigineLien:
      AttRef.AttrSSchema = TtaGetSSchema ("Draw3",
					  event->document);
      AttRef.AttrTypeNum = Draw3_ATTR_Terminaison;
      AttReference = TtaGetAttribute (event->element,
				      AttRef);
      TtaGiveReferenceAttributeValue (AttReference, &Lien,
				      DocName, &TargetDoc);
      TtaDeleteTree (event->element, event->document);
      if (Lien != NULL)
	TracerLienComposite (Lien, event->document);
      TtaSetDisplayMode (event->document, mode);
      return True;
    case Draw3_EL_PartieDestinationLien:
      AttRef.AttrSSchema = TtaGetSSchema ("Draw3",
					  event->document);
      AttRef.AttrTypeNum = Draw3_ATTR_Origine;
      AttReference = TtaGetAttribute (event->element,
				      AttRef);
      TtaGiveReferenceAttributeValue (AttReference, &Lien,
				      DocName, &TargetDoc);
      TtaDeleteTree (event->element, event->document);
      if (Lien != NULL)
	TracerLienComposite (Lien, event->document);
      TtaSetDisplayMode (event->document, mode);
      return True;
    }
  return True;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ModifyAttribute
   * Result        : boolean
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * event              NotifyAttribute* Evenement ayant declanche l'ECF        
   *
   * Functionality : Sauvegarde la position et la taille de l'objet que l'on a
   * modifie.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
boolean ModifyAttribute (NotifyAttribute * event)
#else /* __STDC__ */
boolean ModifyAttribute (event)
     NotifyAttribute *event;
#endif /* __STDC__ */
{
  SauverCoordonnees (event->element, event->document);
  return False;			/* let Thot perform normal operation */
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ModifyAttributeDim
   * Result        : void
   * Parameters
   * Name          Type                         Usage
   * ----          ----                         -----
   * event              NotifyAttribute*        Evenement ayant declanche l'ECF
   *
   * Functionality : Lors de creation d'un element ou d'une action de retaillage
   * de la part de l'utilisateur, on remet a jour les attributs.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void ModifyAttributeDim (NotifyAttribute * event)
#else /* __STDC__ */
void ModifyAttributeDim (event)
     NotifyAttribute *event;
#endif /* __STDC__ */

{
  Element Fleche;
  Document Doc;
  Attribute Att;
  View VuePrincipale;
  NotifyElement Target;
  int AHO, AVO, AHD, AVD;
  int CoinX, CoinY, Hauteur, Largeur;

  Doc = event->document;

  switch (TtaGetElementType (event->element).ElTypeNum)
    {
    case Draw3_EL_LienSimple:
    case Draw3_EL_FlecheCreuse:
      GetValAttrAncre (event->element, &AHO, &AVO, &AHD,
		       &AVD, event->document);
      GetValAttrPosition (event->element, &CoinX, &CoinY,
			  Doc);
      GetValAttrTaille (event->element, &Hauteur, &Largeur,
			Doc);
      SetAttrAncre (event->element, AHO, AVO,
		    AHD + Hauteur - SaveHauteur,
		    AVD + Largeur - SaveLargeur,
		    event->document);
      break;
    case Draw3_EL_LienComposite:
      break;
    case Draw3_EL_PartieOrigineLien:
      TracerOrigineLien (event->element,
			 event->document);
      break;
    case Draw3_EL_PartieDestinationLien:
      TracerDestinationLien (event->element,
			     event->document);
      break;
    case Draw3_EL_Fleche:
      TtaGiveActiveView(&Doc,&VuePrincipale);
      if (!VuePrincipale)
	return;
      TtaGiveBoxPosition (event->element,event->document, VuePrincipale,
			  UnPixel, &CoinX, &CoinY);
      TtaGiveBoxSize (event->element,event->document, VuePrincipale,
		      UnPixel, &Largeur, &Hauteur);
      SetAttrPosition (event->element, CoinX, CoinY, Doc);
      SetAttrTaille (event->element, Hauteur, Largeur,
		     Doc);
      DessinerFleche ((NotifyElement *) event,
		      DetermineSens
		      (TtaGetFirstChild (event->element),
		       'h', event->document)
		      );
      CreerOmbre (event->element, event->document);
      break;
    }

  Doc = 0;
  Att = NULL;
  Fleche = NULL;
  TtaSetDisplayMode (event->document, DeferredDisplay);
  if ((TtaGetElementType (event->element).ElTypeNum !=
       Draw3_EL_LienComposite) &&
      TtaIsElementReferred (event->element))
    {
      TtaNextLoadedReference (event->element, event->document,
			      &Fleche, &Att, &Doc);
      while (Fleche != NULL)
	{
	  Target.element = Fleche;
	  Target.document = Doc;
	  RePositionnerLien (&Target);
	  TtaNextLoadedReference (event->element,
				  event->document,
				  &Fleche, &Att, &Doc);
	}
    }
  else
    AjusterBoiteDim (event->element, event->document);

  TtaSetDisplayMode (event->document, DisplayImmediately);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ModifyAttributePos
   * Result        : void
   * Parameters
   * Name          Type                         Usage
   * ----          ----                         -----
   * event              NotifyAttribute*        Evenement ayant declanche l'ECF
   *
   * Functionality : Lors de creation d'un element ou d'une action de deplacement
   * de la part de l'utilisateur, on remet a jour les attributs.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void ModifyAttributePos (NotifyAttribute * event)
#else /* __STDC__ */
void ModifyAttributePos (event)
     NotifyAttribute *event;
#endif /* __STDC__ */
{
  Element Fleche, GraphicNode;
  Document Doc;
  Attribute Att;
  View VuePrincipale;
  NotifyElement Target;
  int AHO, AVO, AHD, AVD;
  int CoinX, CoinY, Hauteur, Largeur;

  TtaSetDisplayMode (event->document, DeferredDisplay);

  Doc = event->document;

  switch (TtaGetElementType (event->element).ElTypeNum)
    {
    case Draw3_EL_LienSimple:
    case Draw3_EL_FlecheCreuse:
      GetValAttrAncre (event->element, &AHO, &AVO, &AHD,
		       &AVD, event->document);
      GetValAttrPosition (event->element, &CoinX, &CoinY,
			  Doc);
      GetValAttrTaille (event->element, &Hauteur, &Largeur,
			Doc);
      SetAttrAncre (event->element,
		    AHO + CoinX - SaveCoinX,
		    AVO + CoinY - SaveCoinY,
		    AHD, AVD, event->document);
      break;
    case Draw3_EL_LienComposite:
      TtaGiveActiveView(&Doc,&VuePrincipale);
      if (!VuePrincipale)
	return;
      TtaGiveBoxSize (event->element, event->document, VuePrincipale,
		      UnPixel, &Largeur, &Hauteur);
      GetValAttrPosition (event->element, &CoinX, &CoinY,
			  Doc);
      SetAttrTaille (event->element, Hauteur, Largeur,
		     event->document);
      SetAttrPosition (event->element, CoinX + 1, CoinY + 3,
		       event->document);
      break;
    case Draw3_EL_PartieOrigineLien:
      TtaGiveActiveView(&Doc,&VuePrincipale);
      if (!VuePrincipale)
	return;
      TtaGiveBoxSize (event->element, event->document, VuePrincipale,
		      UnPixel, &Largeur, &Hauteur);
      GetValAttrPosition (event->element, &CoinX, &CoinY,
			  Doc);
      SetAttrTaille (event->element, Hauteur, Largeur,
		     event->document);
      SetAttrPosition (event->element, CoinX + 1, CoinY + 3,
		       event->document);
      TracerOrigineLien (event->element,
			 event->document);
      break;
    case Draw3_EL_PartieDestinationLien:
      TtaGiveActiveView(&Doc,&VuePrincipale);
      if (!VuePrincipale)
	return;
      TtaGiveBoxSize (event->element, event->document, VuePrincipale,
		      UnPixel, &Largeur, &Hauteur);
      GetValAttrPosition (event->element, &CoinX, &CoinY, Doc);
      SetAttrTaille (event->element, Hauteur, Largeur,
		     event->document);
      SetAttrPosition (event->element, CoinX + 1, CoinY + 3,
		       event->document);
      TracerDestinationLien (event->element,
			     event->document);
      break;
    case Draw3_EL_Fleche:
      TtaGiveActiveView(&Doc,&VuePrincipale);
      if (!VuePrincipale)
	return;
      TtaGiveBoxSize (event->element, event->document, VuePrincipale,
		      UnPixel, &Largeur, &Hauteur);
      GetValAttrPosition (event->element, &CoinX, &CoinY,
			  Doc);
      SetAttrTaille (event->element, Hauteur, Largeur,
		     event->document);
      SetAttrPosition (event->element, CoinX + 1, CoinY + 3,
		       event->document);
      DessinerFleche ((NotifyElement *) event,
		      DetermineSens
		      (TtaGetFirstChild
		       (event->element),
		       'h', event->document)
	);
      CreerOmbre (event->element, event->document);
      break;
    case Draw3_EL_Polygone:
      TtaGiveActiveView(&Doc,&VuePrincipale);
      if (!VuePrincipale)
	return;
      GraphicNode = TtaGetFirstChild (event->element);
      if (TtaGetGraphicsShape (GraphicNode) == 'W')
	{
	  TtaSetGraphicsShape (GraphicNode, 'p',
			       event->document);
	  TtaShowElement (event->document,
			  VuePrincipale,
			  GraphicNode, 0);
	  TtcInsertChar (event->document, VuePrincipale,'p');
	}
    }

  Doc = 0;
  Att = NULL;
  Fleche = NULL;

  if (TtaIsElementReferred (event->element))
    {
      TtaNextLoadedReference (event->element, event->document,
			      &Fleche, &Att, &Doc);
      while (Fleche != NULL)
	{
	  Target.element = Fleche;
	  Target.document = Doc;
	  RePositionnerLien (&Target);
	  TtaNextLoadedReference (event->element,
				  event->document,
				  &Fleche, &Att, &Doc);
	}
    }
  else
    AjusterBoite (event->element, event->document);

  TtaSetDisplayMode (event->document, DisplayImmediately);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ModifAttributsRectangle
   * Result        : void
   * Parameters
   * Name          Type                         Usage
   * ----          ----                         -----
   * event              NotifyAttribute*        Evenement ayant declanche l'ECF
   *
   * Functionality : Recuperation d'un double-clic sur un objet. On filtre pour
   * afficher le bon formulaire
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void ModifAttributsRectangle (NotifyElement * event)
#else /* __STDC__ */
void ModifAttributsRectangle (event)
     NotifyElement *event;
#endif /* __STDC__ */

{
  Element Forme;
  View mainView;
  Document Doc;

  Doc = event->document;
  TtaGiveActiveView(&Doc,&mainView);
  Forme = TtaGetParent (event->element);
  if (TtaGetElementType (Forme).ElTypeNum == Draw3_EL_Ombre)
    Forme = TtaGetParent (Forme);
  ElementAModifier = Forme;

  switch (TtaGetElementType (Forme).ElTypeNum)
    {
    case Draw3_EL_Rectangle:
    case Draw3_EL_Ellipse:
    case Draw3_EL_Polygone:
    case Draw3_EL_Fleche:
    case Draw3_EL_Autre_forme:
      CreerPaletteFormeRectangle (FEUILLE_RECTANGLE,
				  TtaGetViewFrame (Doc,
						   mainView),
				  1);
      break;
    case Draw3_EL_Trait:
      CreerPaletteTrait (FEUILLE_TRAIT,
			 TtaGetViewFrame (Doc, mainView), 1);
      break;
    case Draw3_EL_Image:
/*
   MenuImage (Name, &type, &pres, event->element,
   event->document);
 */
      break;
    default:
    }
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : InitDrawCreation
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * event              NotifyElement*  Evenement ayant declanche l'action
   *
   * Functionality : Initialise le package Draw3
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void NewDraw (NotifyElement * event)
#else /* __STDC__ */
void NewDraw (event)
     NotifyElement *event;
#endif /* __STDC__ */

{
  
  DrawShowTools(event->document);
  CurrentDoc=event->document;
}


/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : EffacerDraw3
   * Result        : void
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   * event              NotifyElement*  Evenement ayant declanche l'action
   *
   * Functionality : Supprime les boutons de la fenetre principale lorsqu'il
   * n'y a plus de Draw3 dans le document.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void DeleteDraw (NotifyElement * event)
#else /* __STDC__ */
void DeleteDraw (event)
     NotifyElement *event;
#endif /* __STDC__ */
{
  if (TtaGetSSchema ("Draw3", event->document) == NULL) {
    DrawHideTools(event->document);
    CurrentDoc=0;
  }
}
