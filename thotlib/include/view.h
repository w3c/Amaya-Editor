/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _VIEW_H_
#define _VIEW_H_
#include "tree.h"

/* view identifier */
typedef int         View;

/* display mode */
/* DisplayImmediately :  calcul de l'image et affichage */
/* DeferredDisplay : calcul de l'image mais pas d'affichage */
/* NoComputedDisplay : pas de calcul de l'image ni d'affichage */
typedef enum _DisplayMode
  {
     DisplayImmediately, DeferredDisplay, NoComputedDisplay
  }
DisplayMode;


#ifdef __STDC__

extern View         TtaOpenMainView (Document document, int x, int y, int w, int h);
extern void         TtaGetViewGeometry (Document document, char *name, int *x, int *y, int *width, int *height);
extern View         TtaOpenView (Document document, char *viewName, int x, int y, int w, int h);
extern View         TtaOpenSubView (Document document, char *viewName, int x, int y, int w, int h, Element subtree);
extern void         TtaChangeViewTitle (Document document, View view, char *title);
extern void         TtaCloseView (Document document, View view);
extern void         TtaFreeView (Document document, View view);
extern void         TtaSetSensibility (Document document, View view, int value);
extern void         TtaSetZoom (Document document, View view, int value);
extern void         TtaShowElement (Document document, View view, Element element, int position);
extern int          TtaGetSensibility (Document document, View view);
extern int          TtaGetZoom (Document document, View view);
extern int          TtaIsPSchemaValid (char *structureName, char *presentationName);
extern void         TtaGiveViewsToOpen (Document document, char *buffer, int *nbViews);
extern char        *TtaGetViewName (Document document, View view);
extern boolean      TtaIsViewOpened (Document document, View view);
extern View         TtaGetViewFromName (Document document, char *viewName);
extern void         TtaRaiseView (Document document, View view);
extern void         TtaGiveActiveView (Document * document, View * view);
extern void         TtaSetDisplayMode (Document document, DisplayMode newDisplayMode);
extern DisplayMode  TtaGetDisplayMode (Document document);
extern void         TtaListView (Document document, View view, FILE * fileDescriptor);
extern void         TtaResetViewBackgroundColor (Document doc, View view);
extern void         TtaSetViewBackgroundColor (Document doc, View view, int color);
extern int          TtaGetViewBackgroundColor (Document doc, View view);
extern int          TtaGetThotColor (unsigned short red, unsigned short green, unsigned short blue);

#else  /* __STDC__ */

extern void         TtaGetViewGeometry (/*Document document, char *name, int *x, int *y, int *width, int *height*/);
extern View         TtaOpenMainView ( /* Document document, int x, int y, int w, int h */ );
extern View         TtaOpenSubView ( /* Document document, char *viewName, int x, int y, int w, int h, Element subtree */ );
extern void         TtaChangeViewTitle ( /*Document document, View view, char *title */ );
extern void         TtaCloseView ( /* Document document, View view */ );
extern void         TtaFreeView ( /*Document document, View view */ );
extern void         TtaSetSensibility ( /* Document document, View view, int value */ );
extern void         TtaSetZoom ( /* Document document, View view, int value */ );
extern void         TtaShowElement ( /* Document document, View view, Element element, int position */ );
extern int          TtaGetSensibility ( /* Document document, View view */ );
extern int          TtaGetZoom ( /* Document document, View view */ );
extern int          TtaIsPSchemaValid ( /* char *structureName, char *presentationName */ );
extern void         TtaGiveViewsToOpen ( /* Document document, char *buffer, int *nbViews */ );
extern char        *TtaGetViewName ( /* Document document, View view */ );
extern boolean      TtaIsViewOpened ( /*Document document, View view */ );
extern View         TtaGetViewFromName ( /* Document document, char *viewName */ );
extern void         TtaRaiseView ( /*Document document, View view */ );
extern void         TtaGiveActiveView ( /* Document *document, View *view */ );
extern void         TtaSetDisplayMode ( /* Document document, DisplayMode newDisplayMode */ );
extern DisplayMode  TtaGetDisplayMode ( /* Document document */ );
extern void         TtaListView ( /*Document document, View view, FILE *fileDescriptor */ );
extern void         TtaResetViewBackgroundColor ( /*Document doc, View view */ );
extern void         TtaSetViewBackgroundColor ( /*Document doc, View view, int color */ );
extern int          TtaGetViewBackgroundColor ( /*Document doc, View view */ );
extern int          TtaGetThotColor ( /*unsigned short red, unsigned short green, unsigned short blue */ );

#endif /* __STDC__ */
#endif
