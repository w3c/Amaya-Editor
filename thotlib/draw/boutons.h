#ifndef Eboutons
#define Eboutons

/****************************************************************************/
/*---------------------------- NESTED INCLUDES -----------------------------*/
/****************************************************************************/


/****************************************************************************/
/*------------------------- DEFINE AND TYPEDEF AREA ------------------------*/
/****************************************************************************/


#ifndef Xboutons
/****************************************************************************/
/*-------------------------- EXTERNAL  VARIABLES ---------------------------*/
/****************************************************************************/

extern int GrilleX, GrilleY, BaseDialogDraw3;
extern Document CurrentDoc, DocPalette;
extern Element ElementAModifier, CurrentSelect;
extern boolean DrawTabIDoc[];
extern int DrawMsgTable;
/****************************************************************************/
/*-------------------------- EXTERNAL  FUNCTIONS ---------------------------*/
/****************************************************************************/

#ifdef __STDC__
/****************************************************************************/
/*----------------------------- C++ INTERFACE ------------------------------*/
/****************************************************************************/

extern void CallbackDialogueDraw3 (int ref, int typedata, char *data);
extern void InitPaletteGen	();
extern void DetacherIcones 	(Document doc, View view);
extern void AttacherIcones 	(Document doc, View view);
extern void OuvrirPalette 	(Document doc, View view);
extern void FermerPalette 	(Document doc, View view);
extern void IdentifieDocument 	(NotifyDialog *event);
extern void CompleterWindow 	(Document doc, View view);
extern void CreerFormeRectangle (Document doc, View view);
extern void CreerFormeEllipse 	(Document doc, View view);
extern void CreerFormeTrait 	(Document doc, View view);
extern void CreerFormeFleche 	(Document doc, View view);
extern void CreerFormeTexte 	(Document doc, View view);
extern void CreerFormeImage 	(Document doc, View view);
extern void CreerFormeRenvoi 	(Document doc, View view);
extern void CreerRenvoi 	(Document doc, View view, int TypeRenvoi);
extern void CreerFormeAutre 	(Document doc, View view);
extern void CreerFormeLien 	(Document doc, View view);
extern void CreerFormePolygone 	(Document doc, View view);

#else

/****************************************************************************/
/*-------------------------- SIMPLE  C INTERFACE ---------------------------*/
/****************************************************************************/

extern void CallbackDialogueDraw3(/* int ref, int typedata, char *data */);
extern void InitPaletteGen 	();
extern void DetacherIcones 	(/* Document doc, View view */);
extern void AttacherIcones 	(/* Document doc, View view */);
extern void OuvrirPalette 	(/* Document doc, View view */);
extern void FermerPalette 	(/* Document doc, View view */);
extern void IdentifieDocument 	(/* NotifyDialog *event */);
extern void CompleterWindow 	(/* Document doc, View view */);
extern void CreerFormeRectangle (/* Document doc, View view */);
extern void CreerFormeEllipse 	(/* Document doc, View view */);
extern void CreerFormeTrait 	(/* Document doc, View view */);
extern void CreerFormeFleche 	(/* Document doc, View view */);
extern void CreerFormeTexte 	(/* Document doc, View view */);
extern void CreerFormeImage 	(/* Document doc, View view */);
extern void CreerFormeRenvoi 	(/* Document doc, View view */);
extern void CreerRenvoi 	(/* Document doc, View view, int TypeRenvoi*/);
extern void CreerFormeAutre 	(/* Document doc, View view */);
extern void CreerFormeLien 	(/* Document doc, View view */);
extern void CreerFormePolygone 	(/* Document doc, View view */);

#endif /* __STDC__ */
#endif /* Xboutons */
#endif /* Eboutons */
