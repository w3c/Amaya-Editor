
/*!BM
   ------------------------------------------------------------------------------
   *
   * Module      : PaletteActions.c
   * Language    : C
   * Authors     : Stephane Bonhomme, Christian Lenne
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
   *                    PalSelectPre
   *                    PalActivate
   *
   ------------------------------------------------------------------------------
   !EM */


/****************************************************************************/
/*--------------------------- SYSTEM INCLUDES ------------------------------*/
/****************************************************************************/

#include "thot_gui.h"
#include "thot_sys.h"

/****************************************************************************/
/*------------------------ APPLICATION  INCLUDES ---------------------------*/
/****************************************************************************/
#include "app.h"
#include "tree.h"

#include "Draw3.h"
#include "Palette.h"
#include "boutons.h"

/****************************************************************************/
/*----------------------- LOCAL DEFINES AND TYPES --------------------------*/
/****************************************************************************/

#define LARGEUR_OBJET_PALETTE 	150
#define HAUTEUR_OBJET_PALETTE	100

/****************************************************************************/
/*-------------------------- GLOBAL VARIABLES ------------------------------*/
/****************************************************************************/
int LargeurObjetPalette = LARGEUR_OBJET_PALETTE;
int HauteurObjetPalette = HAUTEUR_OBJET_PALETTE;
int XObjetPalette, YObjetPalette;

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
boolean PalSelectPre (NotifyElement * event)
#else /* __STDC__ */
boolean PalSelectPre (event)
     NotifyElement *event;
#endif /* __STDC__ */
{
  /* pas de selection dans la palette */
   return True;
}
/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : PalActivate
   * Result        : boolean
   * Parameters
   * Name          Type         Usage
   * ----          ----         -----
   * event              NotifyElement*  Evenement ayant declanche ECF
   *
   * Functionality : FnCopy l'element clique dans le draw actif.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void PalActivate (NotifyElement * event)
#else /* __STDC__ */
void PalActivate (event)
     NotifyElement *event;
#endif /* __STDC__ */
{
  Element Forme, RlRoot, Copy, Frere, Nouveau;
  ElementType NouveauType;
  int AX, AY, X, Y, PX, PY;

  if (CurrentSelect == NULL)
    /* si pas d'element de draw selectionne : on sort... */
    return;
  Frere = CurrentSelect;
  while (ExistAttribute (Frere, Draw3_ATTR_PositionH,
			 CurrentDoc) == NULL)
    if ((Frere = TtaGetParent (Frere)) == NULL)
      return;
  /* repere le pere de l'element selectionne */
  RlRoot=TtaGetParent(Frere);
  /* on cherche le groupe englobant dans la palette */
  if (TtaGetElementType (event->element).ElTypeNum !=
      Draw3_EL_Groupe)
    Forme = TtaGetParent (event->element);
  else
    Forme = event->element;
  if (TtaGetElementType (Forme).ElTypeNum ==
      Draw3_EL_Ombre)
    Forme = TtaGetParent (Forme);
  /* on fait la copie de l'element */
  Copy = TtaCopyTree (Forme, event->document, CurrentDoc, RlRoot);

  /* on cree un element "fantome" de type rectangle pour ajuster les coordonees */
  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  TtaAskFirstCreation ();
  NouveauType.ElTypeNum = Draw3_EL_Rectangle;
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  TtaInsertSibling (Nouveau, Frere, True, CurrentDoc);
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  AjusterBoite (Nouveau, CurrentDoc);
  TtaGiveBoxSize (Forme, event->document, 1, UnPixel, &AY, &AX);
  TtaGiveBoxSize (Nouveau, event->document, 1, UnPixel, &Y, &X);
  TtaGiveBoxPosition (Nouveau, event->document, 1, UnPixel, &PX, &PY);
  TtaSetDisplayMode (CurrentDoc, DeferredDisplay);
  TtaRemoveTree (Nouveau, CurrentDoc);

  /* on insere la copie */
  TtaInsertSibling (Copy, Frere, True, CurrentDoc);
  /* et on la met a la position define par l'utilisateur */
  SetAttrPosition (Copy, PX + 1, PY + 3, CurrentDoc);
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  AjusterBoite (Copy, CurrentDoc);
  TtaSetDisplayMode (CurrentDoc, DeferredDisplay);
  RetaillerGroupe (Copy, AX, X, AY, Y, CurrentDoc, 1);
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : ChargerPalette
   * Result        : boolean (succes)
   * Parameters
   * Name          Type          Usage
   * ----          ----          -----
   *
   * Functionality : Charge une palette et ouvre la vue principale associee. Si
   * la palette n'existe pas, la palette est cree, Le nom du document palette est 
   * recherche dans le registry
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
boolean ChargerPalette ()
#else /* __STDC__ */
boolean ChargerPalette ()
#endif /* __STDC__ */

{
  Element RootPalette, Courant, PcLast;
  View mainView;
  int NewDocument;
  DisplayMode etat;
  char *name;
  char NomPalette[50];
  char PathDoc[500];
  
  name = TtaGetEnvString("DrawPalette");
  if (name==NULL)
    return False;
  strcpy(NomPalette,name);
  name = strrchr(NomPalette,DIR_SEP);
  if (name!=NULL)
    {
      *name = '\0';
      name++;
    }
  else
    name = NomPalette;
  TtaGetDocumentPath(PathDoc,500);
  if (name != NomPalette)
    {
      TtaSetDocumentPath(NomPalette);
    }
  DocPalette = TtaOpenDocument (name, 1);
  NewDocument = 0;
  if (DocPalette == 0)
    {
      DocPalette = TtaNewDocument ("Palette", name);
      TtaSaveDocument (DocPalette, name);
      NewDocument = 1;
    }
  TtaSetDocumentPath(PathDoc);
/* choose its presentation model */

  TtaSetPSchema (DocPalette, "PaletteP");

/* open its main view */
  mainView = TtaOpenView (DocPalette, "palette", 5, 50, 140, 200);
  TtcSwitchCommands(DocPalette,mainView);
  RootPalette = TtaGetMainRoot (DocPalette);
  Courant = TtaGetFirstChild (RootPalette);
  PcLast = TtaGetLastChild (RootPalette);
  XObjetPalette = YObjetPalette = 10;

  if (NewDocument)
    return True;

  etat = TtaGetDisplayMode (DocPalette);
  TtaSetDisplayMode (DocPalette, DeferredDisplay);

  while ((Courant != NULL) && (Courant != PcLast))
    {
      SetAttrPosition (Courant, XObjetPalette, YObjetPalette, DocPalette);
      if (XObjetPalette == 10)
	XObjetPalette = XObjetPalette + LargeurObjetPalette + 5;
      else
	{
	  XObjetPalette = 10;
	  YObjetPalette = YObjetPalette + HauteurObjetPalette + 5;
	}
      TtaNextSibling (&Courant);
    }
  TtaSetDisplayMode (DocPalette, DisplayImmediately);
  TtaSetDisplayMode (DocPalette, etat);
  return (DocPalette!=0);
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : OuvrirPalette
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document duquel on veut modifier la fenetre
   * view               View            Vue concernee
   *
   * Functionality : Ouvre la palette standard "Palette"
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void OuvrirPalette (Document doc, View view)
#else /* __STDC__ */
void OuvrirPalette (doc, view)
     Document doc;
     View view;
#endif /* __STDC__ */
{
  if (!DocPalette)
    ChargerPalette ();
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : FermerPalette
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document duquel on veut modifier la fenetre
   * view               View            Vue concernee
   *
   * Functionality : repere le doc palette
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void PalNew(NotifyElement * event)
#else /* __STDC__ */
void PalNew (event)
    NotifyElement * event;
#endif /* __STDC__ */

{
  DocPalette = event->document;
  return;
}
