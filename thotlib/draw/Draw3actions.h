/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

#ifndef EDraw3actions
#define EDraw3actions

/*----------------------------------------------------------------------
---------------------------- NESTED INCLUDES -----------------------------
  ----------------------------------------------------------------------*/


/*----------------------------------------------------------------------
------------------------- DEFINE AND TYPEDEF AREA ------------------------
  ----------------------------------------------------------------------*/


#ifndef XDraw3actions
/*----------------------------------------------------------------------
-------------------------- EXTERNAL  VARIABLES ---------------------------
  ----------------------------------------------------------------------*/


/*----------------------------------------------------------------------
-------------------------- EXTERNAL  FUNCTIONS ---------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__

/*----------------------------------------------------------------------
----------------------------- C++ INTERFACE ------------------------------
  ----------------------------------------------------------------------*/

extern boolean 	SelectionPre 		(NotifyElement *event);
extern void 	CreateAttributes 	(NotifyElement *event);
extern boolean 	DeleteAnElement 	(NotifyElement *event);
extern boolean 	CreerUnGraphique 	(NotifyElement *event);
extern boolean 	GraphModify 		(NotifyOnValue *event);
extern void 	ModifAttributsRectangle (NotifyElement *event);
extern void 	CreerUneForme 		(NotifyElement *event);
extern void 	ModifyPRule 		(NotifyPresentation *event);
extern void 	CreerUnLien 		(NotifyElement *event);
extern boolean 	DeleteALien 		(NotifyElement *event);
extern boolean 	ModifyAttribute 	(NotifyAttribute *event);
extern void 	ModifyAttributeDim 	(NotifyAttribute *event);
extern void 	ModifyAttributePos	(NotifyAttribute *event);
extern void 	InitDrawCreation 	(NotifyElement *event);
extern void 	EffacerDraw3 		(NotifyElement *event);
extern boolean 	CompleterWindowCreation (NotifyElement *event);

#else

/*----------------------------------------------------------------------
-------------------------- SIMPLE  C INTERFACE ---------------------------
  ----------------------------------------------------------------------*/

extern boolean 	SelectionPre 		(/* NotifyElement *event */);
extern void 	CreateAttributes 	(/* NotifyElement *event */);
extern boolean 	DeleteAnElement 	(/* NotifyElement *event */);
extern boolean 	CreerUnGraphique 	(/* NotifyElement *event */);
extern boolean 	GraphModify 		(/* NotifyOnValue *event */);
extern void 	ModifAttributsRectangle (/* NotifyElement *event */);
extern void 	CreerUneForme 		(/* NotifyElement *event */);
extern void 	ModifyPRule 		(/* NotifyPresentation *event */);
extern void 	CreerUnLien 		(/* NotifyElement *event */);
extern boolean 	DeleteALien 		(/* NotifyElement *event */);
extern boolean 	ModifyAttribute 	(/* NotifyAttribute *event */);
extern void 	ModifyAttributeDim 	(/* NotifyAttribute *event */);
extern void 	ModifyAttributePos	(/* NotifyAttribute *event */);
extern void 	InitDrawCreation 	(/* NotifyElement *event */);
extern void 	EffacerDraw3 		(/* NotifyElement *event */);
extern boolean 	CompleterWindowCreation (/* NotifyElement *event */);

#endif /* __STDC__ */
#endif /* XDraw3actions */
#endif /* EDraw3actions */




