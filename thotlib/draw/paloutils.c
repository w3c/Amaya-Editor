/*!BM
   ------------------------------------------------------------------------------
   *
   * Module      : paloutils.c
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
   *            CreerPaletteOutils
   *            TraiterEvtPaletteOutils
   *
   ------------------------------------------------------------------------------
   !EM */

#define Xpaloutils

/****************************************************************************/
/*--------------------------- SYSTEM INCLUDES ------------------------------*/
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <unistd.h>
#include <X11/Intrinsic.h>

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
#include "tree.h"
#include "app.h"
#include "dialog.h"
#include "selection.h"
#include "message.h"

#include "menus.h"
#include "objets.h"
#include "edition.h"
#include "boutons.h"
#include "drawmsg.h"

#include "Icons/AligneB.xpm"
#include "Icons/AligneC.xpm"
#include "Icons/AligneD.xpm"
#include "Icons/AligneG.xpm"
#include "Icons/AligneH.xpm"
#include "Icons/AligneM.xpm"
#include "Icons/Dupliquer.xpm"
#include "Icons/Equidistant.xpm"
#include "Icons/Loupe.xpm"
#include "Icons/MemeDimension.xpm"
#include "Icons/Groupe.xpm"
#include "Icons/Degroupe.xpm"
#include "Icons/HorsGroupe.xpm"
#include "Icons/MemeGroupe.xpm"
#include "Icons/Libre.xpm"
#include "Icons/VersPalette.xpm"
#include "Icons/PremierPlan.xpm"
#include "Icons/ArrierePlan.xpm"
#include "Icons/Eloigner.xpm"
#include "Icons/Rapprocher.xpm"

#include "edition_f.h"

/****************************************************************************/
/*----------------------- LOCAL DEFINES AND TYPES --------------------------*/
/****************************************************************************/

/****************************************************************************/
/*--------------------------- LOCAL VARIABLES ------------------------------*/
/****************************************************************************/


/****************************************************************************/
/*-------------------------- GLOBAL VARIABLES ------------------------------*/
/****************************************************************************/


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
   * Function      : CreerPaletteOutils
   * Result        : void
   * Parameters
   * Name          Type         Usage
   * ----          ----                 -----
   * MenuBase   int             Idf du menu
   * Pere               Widget          Widget de rattachement
   * Langue     int             Langue pour l'affichage de messaage
   *
   * Functionality : Cree le formulaire associe.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__

void 
CreerPaletteOutils ()

#else /* __STDC__ */

void 
CreerPaletteOutils ()
#endif /* __STDC__ */

{
  static Pixmap TabFormes1[4] =
  {0, 0, 0, 0};
  static Pixmap TabFormes2[4] =
  {0, 0, 0, 0};
  static Pixmap TabFormes3[4] =
  {0, 0, 0, 0};
  static Pixmap TabFormes4[4] =
  {0, 0, 0, 0};
  static Pixmap TabFormes5[4] =
  {0, 0, 0, 0};

  TtaNewDialogSheet (PALETTE_OUTILS, 0 /* TtaGetViewFrame(0,0) */, 0, 0,
		     TtaGetMessage(DrawMsgTable,DRW_TOOLS),
		     0, "", TRUE, 5, 'L', D_DONE);
  if (TabFormes1[0] == 0)
    TabFormes1[0] = TtaCreatePixmapLogo (AligneG_xpm);
  if (TabFormes1[1] == 0)
    TabFormes1[1] = TtaCreatePixmapLogo (AligneC_xpm);
  if (TabFormes1[2] == 0)
    TabFormes1[2] = TtaCreatePixmapLogo (AligneD_xpm);
  if (TabFormes1[3] == 0)
    TabFormes1[3] = TtaCreatePixmapLogo (Equidistant_xpm);

  if (TabFormes2[0] == 0)
    TabFormes2[0] = TtaCreatePixmapLogo (AligneH_xpm);
  if (TabFormes2[1] == 0)
    TabFormes2[1] = TtaCreatePixmapLogo (AligneM_xpm);
  if (TabFormes2[2] == 0)
    TabFormes2[2] = TtaCreatePixmapLogo (AligneB_xpm);
  if (TabFormes2[3] == 0)
    TabFormes2[3] = TtaCreatePixmapLogo (MemeDimension_xpm);

  if (TabFormes3[0] == 0)
    TabFormes3[0] = TtaCreatePixmapLogo (Dupliquer_xpm);
  if (TabFormes3[1] == 0)
    TabFormes3[1] = TtaCreatePixmapLogo (Loupe_xpm);
  if (TabFormes3[2] == 0)
    TabFormes3[2] = TtaCreatePixmapLogo (VersPalette_xpm);
  if (TabFormes3[3] == 0)
    TabFormes3[3] = TtaCreatePixmapLogo (Libre_xpm);

  if (TabFormes4[0] == 0)
    TabFormes4[0] = TtaCreatePixmapLogo (Groupe_xpm);
  if (TabFormes4[1] == 0)
    TabFormes4[1] = TtaCreatePixmapLogo (Degroupe_xpm);
  if (TabFormes4[2] == 0)
    TabFormes4[2] = TtaCreatePixmapLogo (MemeGroupe_xpm);
  if (TabFormes4[3] == 0)
    TabFormes4[3] = TtaCreatePixmapLogo (HorsGroupe_xpm);

  if (TabFormes5[0] == 0)
    TabFormes5[0] = TtaCreatePixmapLogo (PremierPlan_xpm);
  if (TabFormes5[1] == 0)
    TabFormes5[1] = TtaCreatePixmapLogo (ArrierePlan_xpm);
  if (TabFormes5[2] == 0)
    TabFormes5[2] = TtaCreatePixmapLogo (Rapprocher_xpm);
  if (TabFormes5[3] == 0)
    TabFormes5[3] = TtaCreatePixmapLogo (Eloigner_xpm);

  TtaNewIconMenu (TOOLS_PREMIERE_COLONNE, PALETTE_OUTILS, 0, NULL, 4, &TabFormes1[0], FALSE);
  TtaNewIconMenu (TOOLS_DEUXIEME_COLONNE, PALETTE_OUTILS, 0, NULL, 4, &TabFormes2[0], FALSE);
  TtaNewIconMenu (TOOLS_TROISIEME_COLONNE, PALETTE_OUTILS, 0, NULL, 4, &TabFormes3[0], FALSE);
  TtaNewIconMenu (TOOLS_QUATRIEME_COLONNE, PALETTE_OUTILS, 0, NULL, 4, &TabFormes4[0], FALSE);
  TtaNewIconMenu (TOOLS_CINQUIEME_COLONNE, PALETTE_OUTILS, 0, NULL, 4, &TabFormes5[0], FALSE);

   return;
}

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : TraiterEvtPaletteOutils
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
TraiterEvtPaletteOutils (int ref, int typedata, char *data)

#else /* __STDC__ */

void 
TraiterEvtPaletteOutils (ref, typedata, data)
     int ref, typedata;
     char *data;

#endif /* __STDC__ */

{
  int Action;
  Document Doc;
  View VuePrincipale;

  Action = (int) data;
  TtaGiveActiveView(&Doc,&VuePrincipale);
  if(Doc!=CurrentDoc) 
    return;
  switch (ref)
    {
    case OFFSET_TOOLS_PREMIERE_COLONNE:
      switch (Action)
	{
	case 0:
	  ObjetsAligner ('V', 'G', Doc);
	  break;
	case 1:
	  ObjetsAligner ('V', 'C', Doc);
	  break;
	case 2:
	  ObjetsAligner ('V', 'D', Doc);
	  break;
	case 3:
	  ObjetsEquidistants (Doc);
	  break;
	}
      break;
    case OFFSET_TOOLS_DEUXIEME_COLONNE:
      switch (Action)
	{
	case 0:
	  ObjetsAligner ('H', 'H', Doc);
	  break;
	case 1:
	  ObjetsAligner ('H', 'C', Doc);
	  break;
	case 2:
	  ObjetsAligner ('H', 'B', Doc);
	  break;
	case 3:
	  ObjetsMemeTaille (Doc);
	  break;
	}
      break;
    case OFFSET_TOOLS_TROISIEME_COLONNE:
      switch (Action)
	{
	case 0:
	  ObjetsDupliquer (Doc);
	  break;
	case 1:
	  /* TtaSetNumberForm (ZOOM, TtaGetZoom (Doc,VuePrincipale)); */
	  /* PopUpOuvert = FEUILLE_ZOOM; */
	  /* TtaShowDialogue (FEUILLE_ZOOM, FALSE);*/
	  TtcSetZoomView(Doc,VuePrincipale);
	  break;
	case 2:
	  CopieVersPalette (Doc);
	  break;
	case 3:
	  ResizeGroup(Doc,VuePrincipale);
/* 	  ObjetsModifierPlan ('E', Doc); */
	  break;
	}
      break;
    case OFFSET_TOOLS_QUATRIEME_COLONNE:
      switch (Action)
	{
	case 0:
	  ObjetsGrouper (Doc);
	  break;
	case 1:
	  ObjetsDegrouper (Doc);
	  break;
	case 2:
	  ObjetsMemeGroupe (Doc);
	  break;
	case 3:
	  ObjetsHorsGroupe (Doc);
	  break;
	}
      break;
    case OFFSET_TOOLS_CINQUIEME_COLONNE:
      switch (Action)
	{
	case 0:
	  ObjetsModifierPlan ('P', Doc);
	  break;
	case 1:
	  ObjetsModifierPlan ('D', Doc);
	  break;
	case 2:
	  ObjetsModifierPlan ('R', Doc);
	  break;
	case 3:
	  ObjetsModifierPlan ('E', Doc);
	  break;
	}
      break;
    }
}
/*!BF
------------------------------------------------------------------------------
*
* Function      : TraiterEvtZoom
* Result        : void
* Parameters
* Name          Type            Usage
* ----          ----            -----
* ref           int             Evenement
* typedata      int             Type du parametre
* data          char *          Parametre
*
* Functionality : Traitement d'un callback
*
------------------------------------------------------------------------------
!EF*/

#ifdef __STDC__

void TraiterEvtZoom (int ref, int typedata, char* data)

#else /* __STDC__ */

void TraiterEvtZoom (ref, typedata, data)
int ref, typedata;
char* data;

#endif /* __STDC__ */

{
  View VuePrincipale;
  int val;
  Document doc;
  
  TtaGiveActiveView(&doc,&VuePrincipale);
  
  val = TtaGetZoom (doc, VuePrincipale);
  val = (int) data - val;
  if (val)
    TtaSetZoom (doc, VuePrincipale, val);
}
