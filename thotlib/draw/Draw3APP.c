/* Included headerfiles */
#include "thot_gui.h"
#include "thot_sys.h"
#include "application.h"
#include "app.h"
#include "interface.h"
#include "Draw3.h"

/* ---------------------------------------------------------------------- */
void Draw3ApplicationInitialise ()
{
 PtrEventsSet appliActions;

  /* Create the new application context*/
  appliActions = TteNewEventsSet (60898, "Draw3");
   /* Generate event/action entries */
  TteAddActionEvent (appliActions, 10, TteAttrModify, 1, "ModifyAttribute");
  TteAddActionEvent (appliActions, 10, TteAttrModify, 0, "ModifyAttributeDim");
  TteAddActionEvent (appliActions, 11, TteAttrModify, 1, "ModifyAttribute");
  TteAddActionEvent (appliActions, 11, TteAttrModify, 0, "ModifyAttributeDim");
  TteAddActionEvent (appliActions, 8, TteAttrModify, 1, "ModifyAttribute");
  TteAddActionEvent (appliActions, 8, TteAttrModify, 0, "ModifyAttributePos");
  TteAddActionEvent (appliActions, 9, TteAttrModify, 1, "ModifyAttribute");
  TteAddActionEvent (appliActions, 9, TteAttrModify, 0, "ModifyAttributePos");
  TteAddActionEvent (appliActions, 0, TteElemNew, 0, "CreateAttributes");
  TteAddActionEvent (appliActions, 9, TteElemNew, 0, "NewDraw");
  TteAddActionEvent (appliActions, 2, TteElemNew, 1, "CreerUneForme");
  TteAddActionEvent (appliActions, 4, TteElemNew, 1, "CreerUneForme");
  TteAddActionEvent (appliActions, 22, TteElemNew, 0, "CreerUneForme");
  TteAddActionEvent (appliActions, 25, TteElemNew, 0, "CreerUneForme");
  TteAddActionEvent (appliActions, 27, TteElemNew, 0, "CreerUneForme");
  TteAddActionEvent (appliActions, 26, TteElemNew, 0, "CreerUneForme");
  TteAddActionEvent (appliActions, 10, TteElemNew, 0, "CreerUnLien");
  TteAddActionEvent (appliActions, 14, TteElemNew, 0, "CreerUnLien");
  TteAddActionEvent (appliActions, 16, TteElemNew, 0, "CreerUnLien");
  TteAddActionEvent (appliActions, 17, TteElemNew, 0, "CreerUnLien");
  TteAddActionEvent (appliActions, 15, TteElemNew, 0, "CreerUneForme");
  TteAddActionEvent (appliActions, 30, TteElemNew, 0, "CreerUneForme");
  TteAddActionEvent (appliActions, 0, TteElemDelete, 1, "DeleteAnElement");
  TteAddActionEvent (appliActions, 9, TteElemDelete, 0, "DeleteDraw");
  TteAddActionEvent (appliActions, 10, TteElemDelete, 1, "DeleteALien");
  TteAddActionEvent (appliActions, 16, TteElemDelete, 1, "DeleteALien");
  TteAddActionEvent (appliActions, 17, TteElemDelete, 1, "DeleteALien");
  TteAddActionEvent (appliActions, 0, TteElemSelect, 1, "DrawSelectPre");
  TteAddActionEvent (appliActions, 2, TteElemActivate, 0, "ModifAttributsRectangle");
  TteAddActionEvent (appliActions, 4, TteElemActivate, 0, "ModifAttributsRectangle");
  TteAddActionEvent (appliActions, 29, TteElemActivate, 0, "ModifAttributsRectangle");
  TteAddActionEvent (appliActions, 2, TteElemGraphModify, 1, "GraphModify");
  TteAddActionEvent (appliActions, 4, TteElemGraphModify, 1, "GraphModify");
  TteAddActionEvent (appliActions, 28, TtePRuleModify, 0, "ModifyPRule");

}

/*########## Init Appplication action-list #################*/
#ifdef __STDC__
extern boolean DrawSelectPre (NotifyElement *event);
extern void CreateAttributes (NotifyElement *event);
extern boolean DeleteAnElement (NotifyElement *event);
extern void NewDraw (NotifyElement *event);
extern void DeleteDraw (NotifyElement *event);
extern boolean CreerUneForme (NotifyElement *event);
extern boolean GraphModify (NotifyOnValue *event);
extern void ModifAttributsRectangle (NotifyElement *event);
extern void ModifyPRule (NotifyPresentation *event);
extern void CreerUnLien (NotifyElement *event);
extern boolean DeleteALien (NotifyElement *event);
extern boolean ModifyAttribute (NotifyAttribute *event);
extern void ModifyAttributeDim (NotifyAttribute *event);
extern void ModifyAttributePos (NotifyAttribute *event);
#else /* __STDC__*/
extern boolean DrawSelectPre (/* NotifyElement *event */);
extern void CreateAttributes (/* NotifyElement *event */);
extern boolean DeleteAnElement (/* NotifyElement *event */);
extern void NewDraw (/* NotifyElement *event */);
extern void DeleteDraw (/* NotifyElement *event */);
extern boolean CreerUneForme (/* NotifyElement *event */);
extern boolean GraphModify (/* NotifyOnValue *event */);
extern void ModifAttributsRectangle (/* NotifyElement *event */);
extern void ModifyPRule (/* NotifyPresentation *event */);
extern void CreerUnLien (/* NotifyElement *event */);
extern boolean DeleteALien (/* NotifyElement *event */);
extern boolean ModifyAttribute (/* NotifyAttribute *event */);
extern void ModifyAttributeDim (/* NotifyAttribute *event */);
extern void ModifyAttributePos (/* NotifyAttribute *event */);
#endif /* __STDC__*/

void Draw3ActionListInit ()
{
  InsertAction ("DrawSelectPre", (Proc)DrawSelectPre);
  InsertAction ("CreateAttributes", (Proc)CreateAttributes);
  InsertAction ("DeleteAnElement", (Proc)DeleteAnElement);
  InsertAction ("NewDraw", (Proc)NewDraw);
  InsertAction ("DeleteDraw", (Proc)DeleteDraw);
  InsertAction ("CreerUneForme", (Proc)CreerUneForme);
  InsertAction ("GraphModify", (Proc)GraphModify);
  InsertAction ("ModifAttributsRectangle", (Proc)ModifAttributsRectangle);
  InsertAction ("ModifyPRule", (Proc)ModifyPRule);
  InsertAction ("CreerUnLien", (Proc)CreerUnLien);
  InsertAction ("DeleteALien", (Proc)DeleteALien);
  InsertAction ("ModifyAttribute", (Proc)ModifyAttribute);
  InsertAction ("ModifyAttributeDim", (Proc)ModifyAttributeDim);
  InsertAction ("ModifyAttributePos", (Proc)ModifyAttributePos);
}

void Draw3LoadResources ()
{
  Draw3ActionListInit ();
  Draw3ApplicationInitialise ();
}

