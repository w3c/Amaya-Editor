/* Included headerfiles */
#include "thot_gui.h"
#include "thot_sys.h"
#include "application.h"
#include "app.h"
#include "interface.h"
#include "Palette.h"

/* ---------------------------------------------------------------------- */
void PaletteApplicationInitialise ()
{
 PtrEventsSet appliActions;

  /* Create the new application context*/
  appliActions = TteNewEventsSet (60906, "Palette");
   /* Generate event/action entries */
  TteAddActionEvent (appliActions, 8, TteElemNew, 0, "PalNew");
  TteAddActionEvent (appliActions, 8, TteElemRead, 0, "PalNew");
  TteAddActionEvent (appliActions, 0, TteElemSelect, 1, "PalSelectPre");
  TteAddActionEvent (appliActions, 0, TteElemActivate, 0, "PalActivate");

}

/*########## Init Appplication action-list #################*/
#ifdef __STDC__
extern boolean PalSelectPre (NotifyElement *event);
extern void PalActivate (NotifyElement *event);
extern void PalNew (NotifyElement *event);
#else /* __STDC__*/
extern boolean PalSelectPre (/* NotifyElement *event */);
extern void PalActivate (/* NotifyElement *event */);
extern void PalNew (/* NotifyElement *event */);
#endif /* __STDC__*/

void PaletteActionListInit ()
{
  InsertAction ("PalSelectPre", (Proc)PalSelectPre);
  InsertAction ("PalActivate", (Proc)PalActivate);
  InsertAction ("PalNew", (Proc)PalNew);
}

void PaletteLoadResources ()
{
  PaletteActionListInit ();
  PaletteApplicationInitialise ();
}

