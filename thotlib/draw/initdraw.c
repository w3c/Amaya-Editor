/*!BM
   ------------------------------------------------------------------------------
   *
   * Module      : initdraw.c
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
   *            InitDraw
   *
   ------------------------------------------------------------------------------
   !EM */

#define Xinitdraw

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
#include "tree.h"
#include "view.h"
#include "app.h"
#include "selection.h"
#include "dialog.h"

#include "initdraw.h"
#include "menus.h"
#include "boutons.h"
#include "paloutils.h"
#include "objets.h"
#include "utilitaires.h"
#include "drawmsg.h"

/*!BF
   ------------------------------------------------------------------------------
   *
   * Function      : InitDraw
   * Result        : void
   *
   * Functionality : Initialise tous ce qui est necessaire pour faire des 
   * graphiques.
   *
   ------------------------------------------------------------------------------
   !EF */

#ifdef __STDC__
void InitDraw ()
#else /* __STDC__ */
void InitDraw ()
#endif/* __STDC__ */
{
  static int ThotDrawInitialized = 0;

  if (!ThotDrawInitialized)
    {
      ThotDrawInitialized = 1;

      MenuBase = BaseDialogDraw3 =
	TtaSetCallback (CallbackDialogueDraw3, MAX_MENU);
      DrawMsgTable=TtaGetMessageTable("drawdialogue",DRW_MSG_MAX);
      CurrentDoc = DocPalette = 0;
      CurrentSelect = NULL;
      InitButtons ();
/*       CreerPaletteOutils (PALETTE_OUTILS, 0, 1); */
/*       CreerPaletteLien (PALETTE_LIENS, 0, 1); */
     /*  TtaNewPopup (MENU_RENVOI, 0, NULL, 2, */
/* 		   "BOrigine du renvoi\0BDestination du renvoi", */
/* 		   NULL, 'L'); */
    } 
  return;
}
