/*!BM
   ------------------------------------------------------------------------------
   *
   * Module      : boutons.c
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
   *                    Variables
   *                    ---------
   *            Document CurrentDoc, DocPalette
   *            int GrilleX, GrilleY
   *            Element ElementAModifier, CurrentSelect
   *
   *                    Functions
   *                    ---------
   *
   *            CallbackDialogueDraw3
   *            AttacherIcones
   *            DetacherIcones
   *	DrawShowTools
   *	DrawHideTools
   *	DrawCloseDoc
   *            CreerFormeRectangle
   *            CreerFormeEllipse
   *            CreerFormeTrait
   *            CreerFormeFleche
   *            CreerFormeTexte
   *            CreerFormeImage
   *            CreerFormeRenvoi
   *            CreerRenvoi
   *            CreerFormeAutre
   *            CreerFormeLien
   *            CreerFormePolygone
   *
   ------------------------------------------------------------------------------
   !EM */

#define Xboutons

/****************************************************************************/
/*--------------------------- SYSTEM INCLUDES ------------------------------*/
/****************************************************************************/

#include "thot_gui.h"
#include "thot_sys.h"

/****************************************************************************/
/*------------------------ APPLICATION  INCLUDES ---------------------------*/
/****************************************************************************/

#include "boutons.h"
#include "application.h"
#include "document.h"
#include "attribute.h"
#include "language.h"
#include "content.h"
#include "reference.h"
#include "tree.h"
#include "app.h"
#include "view.h"
#include "interface.h"
#include "appaction.h"
#include "dialog.h"
#include "selection.h"
#include "message.h"
#include "Draw3.h"

#include "menus.h"
#include "paloutils.h"
#include "utilitaires.h"
#include "Draw3actions.h"
#include "edition.h"
#include "drawmsg.h"

#include "Icons/rectangle.xpm"
#include "Icons/ellipse.xpm"
#include "Icons/polygone.xpm"
#include "Icons/trait.xpm"
#include "Icons/fleche.xpm"
#include "Icons/texte.xpm"
#include "Icons/image.xpm"
#include "Icons/liens.xpm"
#include "Icons/renvoi.xpm"
#include "Icons/autre.xpm"
#include "Icons/inactrectangle.xpm"
#include "Icons/inactellipse.xpm"
#include "Icons/inactpolygone.xpm"
#include "Icons/inacttrait.xpm"
#include "Icons/inactfleche.xpm"
#include "Icons/inacttexte.xpm"
#include "Icons/inactimage.xpm"
#include "Icons/inactliens.xpm"
#include "Icons/inactrenvoi.xpm"
#include "Icons/inactautre.xpm"
/****************************************************************************/
/*----------------------- LOCAL DEFINES AND TYPES --------------------------*/
/****************************************************************************/
#define DRAW_MAX_DOC 10

/****************************************************************************/
/*--------------------------- LOCAL VARIABLES ------------------------------*/
/****************************************************************************/
static int DrawTabIconeDoc[DRAW_MAX_DOC];
static Pixmap IRec, IEll, IPoly, ITrait, IArr, IText, IImage, ILink, IRenvoi,  IAutre,IRecI, IEllI, IPolyI, ITraitI, IArrI, ITextI, IImageI, ILinkI, IRenvoiI, IAutreI;


/****************************************************************************/
/*-------------------------- GLOBAL VARIABLES ------------------------------*/
/****************************************************************************/

Document CurrentDoc, DocPalette;
int GrilleX = 10, GrilleY = 10;
Element ElementAModifier, CurrentSelect;
int BaseDialogDraw3,DrawMsgTable;
boolean DrawTabIDoc[DRAW_MAX_DOC];

/****************************************************************************/
/*-------------------------- FORWARD FUNCTIONS -----------------------------*/
/****************************************************************************/

#ifdef __STDC__

/****************************************************************************/
/*----------------------------- C++ INTERFACE ------------------------------*/
/****************************************************************************/

void CallbackDialogueDraw3 (int ref, int typedata, char *data);
void InitButtons ();
static void AttacherIcones (Document doc, View view);
static void DetacherIcones ( Document doc, View view );
void DrawShowTools ( Document document );
void DrawHideTools ( Document document );
void DrawCloseDoc ( Document doc );
void CreerFormeRectangle (Document doc, View view);
void CreerFormeEllipse (Document doc, View view);
void CreerFormeTrait (Document doc, View view);
void CreerFormeFleche (Document doc, View view);
void CreerFormeTexte (Document doc, View view);
void CreerFormeImage (Document doc, View view);
void CreerFormeRenvoi (Document doc, View view);
void CreerRenvoi (Document doc, View view, int TypeRenvoi);
void CreerFormeAutre (Document doc, View view);
void CreerFormeLien (Document doc, View view);
void CreerFormePolygone (Document doc, View view);

#else

/****************************************************************************/
/*-------------------------- SIMPLE  C INTERFACE ---------------------------*/
/****************************************************************************/

void CallbackDialogueDraw3 ( /* int ref, int typedata, char *data */ );
void InitButtons ();
static void AttacherIcones ( /* Document doc, View view */ );
static void DetacherIcones (/* Document doc, View view */);
void DrawShowTools (/* Document document */);
void DrawHideTools (/* Document document */);
void DrawCloseDoc (/* Document doc */);
void CreerFormeRectangle ( /* Document doc, View view */ );
void CreerFormeEllipse ( /* Document doc, View view */ );
void CreerFormeTrait ( /* Document doc, View view */ );
void CreerFormeFleche ( /* Document doc, View view */ );
void CreerFormeTexte ( /* Document doc, View view */ );
void CreerFormeImage ( /* Document doc, View view */ );
void CreerFormeRenvoi ( /* Document doc, View view */ );
void CreerRenvoi ( /* Document doc, View view, int TypeRenvoi */ );
void CreerFormeAutre ( /* Document doc, View view */ );
void CreerFormeLien ( /* Document doc, View view */ );
void CreerFormePolygone ( /* Document doc, View view */ );

#endif /* __STDC__ */

/****************************************************************************/
/*--------------------------- LOCAL FUNCTIONS ------------------------------*/
/****************************************************************************/


/****************************************************************************/
/*------------------------- EXPORTED  FUNCTIONS ----------------------------*/
/****************************************************************************/

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CallbackDialogueDraw3
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * ref                int             Numero du callback
   * typedata   int             Type de donnee associee
   * data               char*           Donnee associee
   *
   * Functionality : Traite les evenements
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void CallbackDialogueDraw3 (int ref, int typedata, char *data)
#else /* __STDC__ */
void CallbackDialogueDraw3 (ref, typedata, data)
     int ref;
     int typedata;
     char *data;
#endif /* __STDC__ */

{
  int ret;

  TtaHandlePendingEvents();
  ret = (int) data;
  switch (ref - BaseDialogDraw3)
    {
    case OFFSET_TOOLS_PREMIERE_COLONNE:
    case OFFSET_TOOLS_DEUXIEME_COLONNE:
    case OFFSET_TOOLS_TROISIEME_COLONNE:
    case OFFSET_TOOLS_QUATRIEME_COLONNE:
    case OFFSET_TOOLS_CINQUIEME_COLONNE:
      TraiterEvtPaletteOutils (ref - BaseDialogDraw3,
			       typedata, data);
      break;
    case OFFSET_PALETTE_LIENS:
      break;
    case OFFSET_LIEN_PREMIERE_LIGNE:
      TraiterEvtPaletteLien (ref - BaseDialogDraw3,
			     typedata, data);
      break;
    case OFFSET_FEUILLE_RECTANGLE:
      AppliquerFeuilleRectangle (ref - BaseDialogDraw3,
				 typedata, data);
      break;
    case OFFSET_FEUILLE_TRAIT:
      AppliquerFeuilleTrait (ref - BaseDialogDraw3,
			     typedata, data);
      break;
    case OFFSET_RECTANGLE_EPAISSEUR:
    case OFFSET_RECTANGLE_EP_TRAIT:
    case OFFSET_RECTANGLE_OMBRE_1:
    case OFFSET_RECTANGLE_OMBRE_2:
    case OFFSET_RECTANGLE_OMBRE_3:
    case OFFSET_RECTANGLE_COULEUR:
    case OFFSET_POSITION_LABEL:
    case OFFSET_RECTANGLE_ANGLES:
    case OFFSET_RECTANGLE_COULEURO:
    case OFFSET_RECTANGLE_STYLE_TRAIT:
      if (TtaGetElementType (ElementAModifier).ElTypeNum ==
	  Draw3_EL_Trait)
	TraiterEvtPaletteFormeTrait
	  (ref - BaseDialogDraw3, typedata, data);
      else
	TraiterEvtPaletteFormeRectangle
	  (ref - BaseDialogDraw3, typedata, data);
      break;
    case OFFSET_MENU_RENVOI:
      CreerRenvoi (CurrentDoc, 1, (int) data);
      TtaAbortShowDialogue ();
      break;
    }
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : InitButtons
   * Result        : void
   *
   * Functionality : Charge les pixmap associees aux boutons de la main view
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
InitButtons ()

#else /* __STDC__ */

void 
InitButtons ()

#endif				/* __STDC__ */
{
  int i;

  IRec = TtaCreatePixmapLogo (rectangle_xpm);
  IEll = TtaCreatePixmapLogo (ellipse_xpm);
  IPoly = TtaCreatePixmapLogo (polygone_xpm);
  ITrait = TtaCreatePixmapLogo (trait_xpm);
  IArr = TtaCreatePixmapLogo (fleche_xpm);
  IText = TtaCreatePixmapLogo (texte_xpm);
  IImage = TtaCreatePixmapLogo (image_xpm);
  ILink = TtaCreatePixmapLogo (liens_xpm);
  IRenvoi = TtaCreatePixmapLogo (renvoi_xpm);
  IAutre = TtaCreatePixmapLogo (autre_xpm);
  IRecI = TtaCreatePixmapLogo (inactrectangle_xpm);
  IEllI = TtaCreatePixmapLogo (inactellipse_xpm);
  IPolyI = TtaCreatePixmapLogo (inactpolygone_xpm);
  ITraitI = TtaCreatePixmapLogo (inacttrait_xpm);
  IArrI = TtaCreatePixmapLogo (inactfleche_xpm);
  ITextI = TtaCreatePixmapLogo (inacttexte_xpm);
  IImageI = TtaCreatePixmapLogo (inactimage_xpm);
  ILinkI = TtaCreatePixmapLogo (inactliens_xpm);
  IRenvoiI = TtaCreatePixmapLogo (inactrenvoi_xpm);
  IAutreI = TtaCreatePixmapLogo (inactautre_xpm);  
  for (i=0; i < DRAW_MAX_DOC;i++) {
    DrawTabIDoc[i]=FALSE;
    DrawTabIconeDoc[i]=-1;
  }
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : AttacherIcones
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document duquel on veut modifier la fenetre
   * view               View            Vue concernee
   *
   * Functionality : Place les boutons sur la vue principale.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
static void AttacherIcones (Document doc, View view)
#else /* __STDC__ */
static void AttacherIcones (doc, view)
     Document doc;
     View view;
#endif /* __STDC__ */
{
  DrawTabIDoc[(int)doc]=TRUE;
  TtaSetDisplayMode (doc, DeferredDisplay);
  if(DrawTabIconeDoc[(int)doc]==-1)
    {
      DrawTabIconeDoc[(int)doc]=TtaAddButton (doc, 1, IRec, CreerFormeRectangle);
      TtaAddButton (doc, 1, IEll, CreerFormeEllipse);
      TtaAddButton (doc, 1, IPoly, CreerFormePolygone);
      TtaAddButton (doc, 1, ITrait, CreerFormeTrait);
      TtaAddButton (doc, 1, IArr, CreerFormeFleche);
      TtaAddButton (doc, 1, IText, CreerFormeTexte);
      TtaAddButton (doc, 1, IImage, CreerFormeImage);
      TtaAddButton (doc, 1, ILink, CreerFormeLien);
      TtaAddButton (doc, 1, IRenvoi, CreerFormeRenvoi);
      TtaAddButton (doc, 1, IAutre, CreerFormeAutre);
      CreerPaletteOutils();
      TtaShowDialogue (PALETTE_OUTILS, TRUE);
    }
  else
    {
      TtaChangeButton(doc, view, DrawTabIconeDoc[(int)doc],IRec);
      TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+1,IEll);
      TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+2,IPoly);
      TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+3,ITrait);
      TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+4,IArr);
      TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+5,IText);
      TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+6,IImage);
      TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+7,ILink);
      TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+8,IRenvoi);
      TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+9,IAutre);
      }
  TtaSetDisplayMode (doc, DisplayImmediately);
}
  
/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : DetacherIcones
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document duquel on veut modifier la fenetre
   * view               View            Vue concernee
   *
   * Functionality : DeleteElement les boutons sur la vue principale.
   *
   ------------------------------------------------------------------------------
   !EF */
#ifdef __STDC__
static void DetacherIcones (Document doc, View view)
#else /* __STDC__ */
static void DetacherIcones (doc, view)
     Document doc;
     View view;
#endif /* __STDC__ */
{
  DrawTabIDoc[(int)doc]=FALSE;
  TtaSetDisplayMode (doc, DeferredDisplay);
  TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc],IRecI);
  TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+1,IEllI);
  TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+2,IPolyI);
  TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+3,ITraitI);
  TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+4,IArrI);
  TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+5,ITextI);
  TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+6,IImageI);
  TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+7,ILinkI);
  TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+8,IRenvoiI);
  TtaChangeButton (doc, view, DrawTabIconeDoc[(int)doc]+9,IAutreI);
  TtaSetDisplayMode (doc, DisplayImmediately); 
}


/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : DrawShowTools
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * document  Document	    
   *
   * Functionality : Rajoute les boutons a la vue principale du document et ouvre la palette
   * d'outils dessin
   *
   ------------------------------------------------------------------------------
   !EF */
#ifdef __STDC__
void DrawShowTools (Document document)
#else /* __STDC__ */
void DrawShowTools (document )
Document document;
#endif /*__STDC__*/
{
  View vue;
 
  if (!DrawTabIDoc[(int)document] &&
      document != DocPalette)
    {
      vue = (View)1;
      AttacherIcones(document, vue);
/*       TtaShowDialogue (PALETTE_OUTILS, TRUE); */
    }
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : DrawHideTools
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * document  Document	    
   *
   * Functionality : Enleve les boutons a la vue principale du document et ferme la palette
   * d'outils dessin si le document etait le dernier a parter les boutons
   *
   ------------------------------------------------------------------------------
   !EF */
#ifdef __STDC__
void DrawHideTools (Document document)
#else /* __STDC__ */
void DrawHideTools (document)
Document document;
#endif /*__STDC__*/
{
  View vue;

 
  if (DrawTabIDoc[(int)document])
    {
      vue = (View)1;
     
      DetacherIcones(document, vue);    
      TtaDestroyDialogue(PALETTE_LIENS);
      TtaDestroyDialogue(FEUILLE_RECTANGLE);
      TtaDestroyDialogue(FEUILLE_TRAIT);
    }
}

#ifdef __STDC__
void  DrawCloseDoc(Document doc)
#else /* __STDC__ */
void  DrawCloseDoc(doc)
     Document doc;
#endif /* __STDC__ */
{
  int i;
  if(doc==DocPalette)
    DocPalette = 0;
  DrawHideTools(doc);
  for(i=1;i<DRAW_MAX_DOC && (i==doc || !DrawTabIDoc[i]);i++);
  if (i == DRAW_MAX_DOC)
    TtaDestroyDialogue (PALETTE_OUTILS);
  
  DrawTabIconeDoc[(int)doc]=-1;
}


/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerFormeRectangle
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On cree l'objet associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerFormeRectangle (Document doc, View view)

#else /* __STDC__ */

void 
CreerFormeRectangle (doc, view)
     Document doc;
     View view;

#endif /* __STDC__ */

{
  Element Nouveau, Frere;
  ElementType NouveauType;
  NotifyElement Event;
  boolean inside;

  if (!DrawTabIDoc[(int)doc] || CurrentDoc != doc)
    return;
  Frere = ChercherContexteInsert (CurrentDoc,&inside);
  if (Frere == NULL)
    return;
  
  TtaAskFirstCreation (); 
  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  NouveauType.ElTypeNum = Draw3_EL_Rectangle;
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  if(inside)
    TtaInsertFirstChild(&Nouveau, Frere, CurrentDoc);
  else
    TtaInsertSibling (Nouveau, Frere, FALSE, CurrentDoc);
  Event.document = CurrentDoc;
  Event.element = Nouveau;
  TtaSelectElement (CurrentDoc, TtaGetFirstChild (Nouveau));
  CreerUneForme (&Event);
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerFormeEllipse
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On cree l'objet associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerFormeEllipse (Document doc, View view)

#else /* __STDC__ */

void 
CreerFormeEllipse (doc, view)
     Document doc;
     View view;

#endif /* __STDC__ */

{
  Element Nouveau, Frere;
  ElementType NouveauType;
  NotifyElement Event;
  boolean inside;


  if (!DrawTabIDoc[(int)doc] || CurrentDoc!= doc)
    return;
  Frere = ChercherContexteInsert (CurrentDoc,&inside);
  if (Frere == NULL)
    return;
  
  TtaAskFirstCreation (); 
  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  NouveauType.ElTypeNum = Draw3_EL_Ellipse;
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  if(inside)
    TtaInsertFirstChild(&Nouveau, Frere, CurrentDoc);
  else
    TtaInsertSibling (Nouveau, Frere, FALSE, CurrentDoc);
  Event.document = CurrentDoc;
  Event.element = Nouveau;
  TtaSelectElement (CurrentDoc, TtaGetFirstChild (Nouveau));
  CreerUneForme (&Event);
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerFormeTrait
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On cree l'objet associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerFormeTrait (Document doc, View view)

#else /* __STDC__ */

void 
CreerFormeTrait (doc, view)
     Document doc;
     View view;

#endif /* __STDC__ */

{
  Element Nouveau, Frere;
  ElementType NouveauType;
  NotifyElement Event;
  boolean inside;

  if (!DrawTabIDoc[(int)doc] || CurrentDoc!= doc)
    return;
  Frere = ChercherContexteInsert (CurrentDoc,&inside);
  if (Frere == NULL)
    return;
 
  TtaAskFirstCreation ();
  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  NouveauType.ElTypeNum = Draw3_EL_Trait;
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  if(inside)
    TtaInsertFirstChild(&Nouveau, Frere, CurrentDoc);
  else
    TtaInsertSibling (Nouveau, Frere, FALSE, CurrentDoc);
  Event.document = CurrentDoc;
  Event.element = Nouveau;
  TtaSelectElement (CurrentDoc, TtaGetFirstChild (Nouveau));
  CreerUneForme (&Event);
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerFormeFleche
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On cree l'objet associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerFormeFleche (Document doc, View view)

#else /* __STDC__ */

void 
CreerFormeFleche (doc, view)
     Document doc;
     View view;

#endif /* __STDC__ */

{
  Element Nouveau, Frere;
  ElementType NouveauType;
  NotifyElement Event;
  boolean inside;

  if (!DrawTabIDoc[(int)doc] || CurrentDoc!= doc)
    return;
  Frere = ChercherContexteInsert (CurrentDoc,&inside);
  if (Frere == NULL)
    return;
  TtaAskFirstCreation ();
  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  NouveauType.ElTypeNum = Draw3_EL_Fleche;
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  if(inside)
    TtaInsertFirstChild(&Nouveau, Frere, CurrentDoc);
  else
    TtaInsertSibling (Nouveau, Frere, FALSE, CurrentDoc);
  Event.document = CurrentDoc;
  Event.element = Nouveau;
  TtaSelectElement (CurrentDoc, TtaGetFirstChild (Nouveau));
  CreerUneForme (&Event);
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerFormeTexte
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On cree l'objet associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerFormeTexte (Document doc, View view)

#else /* __STDC__ */

void 
CreerFormeTexte (doc, view)
     Document doc;
     View view;

#endif /* __STDC__ */

{
  Element Nouveau, Frere;
  ElementType NouveauType;
  NotifyElement Event;
  boolean inside;

  if (!DrawTabIDoc[(int)doc] || CurrentDoc!= doc)
    return;
  Frere = ChercherContexteInsert (CurrentDoc,&inside);
  if (Frere == NULL)
    return;

  TtaAskFirstCreation ();
  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  NouveauType.ElTypeNum = Draw3_EL_Texte;
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  if(inside)
    TtaInsertFirstChild(&Nouveau, Frere, CurrentDoc);
  else
    TtaInsertSibling (Nouveau, Frere, FALSE, CurrentDoc);

  Event.document = CurrentDoc;
  Event.element = Nouveau;
  TtaSelectElement (CurrentDoc, TtaGetFirstChild (Nouveau));
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerFormeImage
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On cree l'objet associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerFormeImage (Document doc, View view)

#else /* __STDC__ */

void 
CreerFormeImage (doc, view)
     Document doc;
     View view;

#endif /* __STDC__ */

{
  Element Nouveau, Frere;
  ElementType NouveauType;
  boolean inside;

  if (!DrawTabIDoc[(int)doc] || CurrentDoc!= doc)
    return;
  Frere = ChercherContexteInsert (CurrentDoc,&inside);
  if (Frere == NULL)
    return;

  TtaAskFirstCreation ();
  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  NouveauType.ElTypeNum = Draw3_EL_Image;
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  if(inside)
    TtaInsertFirstChild(&Nouveau, Frere, CurrentDoc);
  else
    TtaInsertSibling (Nouveau, Frere, FALSE, CurrentDoc);
  TtaSelectElement (CurrentDoc, Nouveau);
  /* ici appeler le formulaire de creation de l'image */
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerFormeRenvoi
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On affiche le menu pour le choix du renvoi.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerFormeRenvoi (Document doc, View view)

#else /* __STDC__ */

void 
CreerFormeRenvoi (doc, view)
     Document doc;
     View view;

#endif /* __STDC__ */

{
  char BufMenu[100];
  int pos;

  if (!DrawTabIDoc[(int)doc]|| CurrentDoc!= doc)
    return;
  sprintf(BufMenu,"B%s",TtaGetMessage(DrawMsgTable,DRW_CR_ORIGIN));
  pos=strlen(BufMenu);
  sprintf(&BufMenu[pos+1],"B%s",TtaGetMessage(DrawMsgTable,DRW_CR_TARGET));
  
  TtaSetDialoguePosition ();
  TtaNewPopup (MENU_RENVOI, 0, NULL, 2,
	       BufMenu,
	       NULL, 'L');
  TtaShowDialogue (MENU_RENVOI, FALSE);
  TtaWaitShowDialogue ();
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerRenvoi
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   * TypeRenvoi int             Renvoi source ou destination
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On cree l'objet associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerRenvoi (Document doc, View view, int TypeRenvoi)

#else /* __STDC__ */

void 
CreerRenvoi (doc, view, TypeRenvoi)
     Document doc;
     View view;
     int TypeRenvoi;

#endif /* __STDC__ */

{
  Element Nouveau, Frere;
  ElementType NouveauType;
  boolean inside;

  if (!DrawTabIDoc[(int)doc] || CurrentDoc!= doc)
    return;
  Frere = ChercherContexteInsert (CurrentDoc,&inside);
  if (Frere == NULL)
    return;
  
  
  TtaAskFirstCreation ();
  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  if (TypeRenvoi)
    NouveauType.ElTypeNum = Draw3_EL_DestinationRenvoi;
  else
    NouveauType.ElTypeNum = Draw3_EL_OrigineRenvoi;
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  if(inside)
    TtaInsertFirstChild(&Nouveau, Frere, CurrentDoc);
  else
    TtaInsertSibling (Nouveau, Frere, FALSE, CurrentDoc);
  TtaSelectElement (CurrentDoc, Nouveau);
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerFormeAutre
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On cree l'objet associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerFormeAutre (Document doc, View view)

#else /* __STDC__ */

void 
CreerFormeAutre (doc, view)
     Document doc;
     View view;

#endif /* __STDC__ */

{
  Element Nouveau, Frere;
  ElementType NouveauType;
  NotifyElement Event;
  boolean inside;

  if (!DrawTabIDoc[(int)doc] || CurrentDoc!= doc)
    return;
  Frere = ChercherContexteInsert (CurrentDoc,&inside);
  if (Frere == NULL)
    return;
  
  TtaAskFirstCreation ();
  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  NouveauType.ElTypeNum = Draw3_EL_Autre_forme;
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  if(inside)
    TtaInsertFirstChild(&Nouveau, Frere, CurrentDoc);
  else
    TtaInsertSibling (Nouveau, Frere, FALSE, CurrentDoc);
  Event.document = CurrentDoc;
  Event.element = Nouveau;
  TtaSelectElement (CurrentDoc, TtaGetFirstChild (Nouveau));
  TtcDisplayGraphicsKeyboard (doc, view);
  CreerUneForme (&Event);
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerFormeLien
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On affiche le formulaire pour le choix du lien.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerFormeLien (Document doc, View view)

#else /* __STDC__ */

void 
CreerFormeLien (doc, view)
     Document doc;
     View view;

#endif /* __STDC__ */

{
  if (!DrawTabIDoc[(int)doc])
    return;
  CreerPaletteLien(PALETTE_LIENS,TtaGetViewFrame(doc, view),0);
  TtaSetDialoguePosition ();
  TtaShowDialogue (PALETTE_LIENS, FALSE);
  return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : CreerFormePolygone
   * Result        : void
   * Parameters
   * Name          Type                 Usage
   * ----          ----                 -----
   * doc                Document        Document dans lequel on veut creer l'objet
   * view               View            Vue associee (toujours la main view)
   *
   * Functionality : Fonction appelee lors d'un clic sur un bouton de la vue
   * principale. On cree l'objet associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerFormePolygone (Document doc, View view)

#else /* __STDC__ */

void 
CreerFormePolygone (doc, view)
     Document doc;
     View view;

#endif /* __STDC__ */

{
  Element Nouveau, Frere;
  ElementType NouveauType;
  NotifyElement Event;
  boolean inside;

  if (!DrawTabIDoc[(int)doc] || CurrentDoc!= doc)
    return;
  Frere = ChercherContexteInsert (CurrentDoc,&inside);
  if (Frere == NULL)
    return;
  
  TtaAskFirstCreation ();
  NouveauType.ElSSchema = TtaGetSSchema ("Draw3", CurrentDoc);
  NouveauType.ElTypeNum = Draw3_EL_Polygone;
  Nouveau = TtaNewTree (CurrentDoc, NouveauType, "");
  if(inside)
    TtaInsertFirstChild(&Nouveau, Frere, CurrentDoc);
  else
    TtaInsertSibling (Nouveau, Frere, FALSE, CurrentDoc);
  Event.document = CurrentDoc;
  Event.element = Nouveau;
  TtaSelectElement (CurrentDoc, TtaGetFirstChild (Nouveau));
 /*  TtaSetDisplayMode (CurrentDoc, DisplayImmediately); */
/*   TtcInsertChar (CurrentDoc, view, 'p'); */
/*   TtaSetDisplayMode (CurrentDoc, DeferredDisplay); */

  CreerUneForme (&Event);
  TtaSetDisplayMode (CurrentDoc, DisplayImmediately);
}
