/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
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
     DisplayImmediately, DeferredDisplay, NoComputedDisplay, SuspendDisplay
  }
DisplayMode;


#ifdef __STDC__


/*----------------------------------------------------------------------
   TtaOpenMainView

   Opens the main view of a document. This document must have a PSchema
   (see TtaSetPSchema).

   Parameters:
   document: the document for which a window must be open.
   x, y: coordinate (in millimeters) of the upper left corner of the
   window that will display the view.
   w, h: width and height (in millimeters) of the upper left corner of the
   window that will display the view.

   Return value:
   the view opened or 0 if the view cannot be opened.

  ----------------------------------------------------------------------*/
extern View         TtaOpenMainView (Document document, int x, int y, int w, int h);

/*----------------------------------------------------------------------
   TtaGetViewWH 
   returns the current width and height values associated
   with the frame where a view is displayed
   Parameters: 
   doc: document number
   v: view number
   w: width of the frame
   h: height of the frame

  ----------------------------------------------------------------------*/
extern void         TtaGetViewWH (Document doc, int view, /*OUT*/ int *w, 
				  /*OUT*/ int *h);

/*----------------------------------------------------------------------
   TtaGetViewGeometryRegistry 
   returns the position (x, y) and sizes        
   (width, height) of the frame where a view is displayed. These values
   are read from the Thot registry and, if they don't exist, from a 
   .conf file.
   Parameters:    document: the document.                  
   name: the name of the view in P schema.  
   x, y, width, height: the frame's geometry

  ----------------------------------------------------------------------*/
extern void         TtaGetViewGeometryRegistry (Document document, char* name, /*OUT*/ int *x, /*OUT*/ int *y, /*OUT*/ int *width, /*OUT*/ int *height);


/*----------------------------------------------------------------------
   TtaGetViewGeometry returns the position (x, y) and sizes        
   (width, height) of the frame where a view is displayed.
   The values returned are in pixels.
   Parameters:    document: the document.                  
   name: the name of the view in P schema.  
  ----------------------------------------------------------------------*/
extern void         TtaGetViewGeometry (Document document, CHAR_T* name, /*OUT*/ int *x, /*OUT*/ int *y, /*OUT*/ int *width, /*OUT*/ int *height);

/*----------------------------------------------------------------------
   TtaGetViewGeometryMM returns the position (x, y) and sizes        
   (width, height) of the frame where a view is displayed. The values
   returned are in mm.
   Parameters:    document: the document.                  
   name: the name of the view in P schema.  
  ----------------------------------------------------------------------*/
extern void         TtaGetViewGeometryMM (Document document, CHAR_T* name, int *x, int *y, int *width, int *height);

/*----------------------------------------------------------------------
   TtaOpenView

   Opens a view for a document. This document must have a PSchema
   (see TtaSetPSchema).

   Parameters:
   document: the document.
   viewName: name of the view to be opened.
   x, y: coordinate (in millimeters) of the upper left corner of the
   window that will display the view.
   w, h: width and height (in millimeters) of the upper left corner of the
   window that will display the view.

   Return value:
   the view opened or 0 if the view cannot be opened.

  ----------------------------------------------------------------------*/
extern View         TtaOpenView (Document document, STRING viewName, int x, int y, int w, int h);

/*----------------------------------------------------------------------
   TtaOpenSubView

   Opens a view that shows only a subtree. This document must have a PSchema
   (see TtaSetPSchema).

   Parameters:
   document: the document.
   viewName: name of the view to be opened.
   x, y: coordinate (in millimeters) of the upper left corner of the
   window that will display the view.
   w, h: width and height (in millimeters) of the upper left corner of the
   window that will display the view.
   subtree: root element of the subtree to be shown in the view.

   Return value:
   the view opened or 0 if the view cannot be opened.

  ----------------------------------------------------------------------*/
extern View         TtaOpenSubView (Document document, STRING viewName, int x, int y, int w, int h, Element subtree);

/*----------------------------------------------------------------------
   TtaChangeViewTitle

   Changes the title of a view.

   Parameters:
   document: the document.
   view: the view.
   title: the new title.

  ----------------------------------------------------------------------*/
extern void         TtaChangeViewTitle (Document document, View view, STRING title);

/*----------------------------------------------------------------------
   TtaCloseView

   Closes a view.

   Parameters:
   document: the document for which a view must be closed.
   view: the view to be closed.

  ----------------------------------------------------------------------*/
extern void         TtaCloseView (Document document, View view);

/*----------------------------------------------------------------------
   TtaFreeView

   frees the view of the document. The window continues to exist but the document
   is no longer displayed in this window.

   Parameters:
   document: the document for which a view must be closed.
   view: the view to be closed.

  ----------------------------------------------------------------------*/
extern void         TtaFreeView (Document document, View view);

/*----------------------------------------------------------------------
   TtaSetSensibility

   Changes the current sensibility used to display a given view of a
   given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.
   value: new value of the sensibility.

  ----------------------------------------------------------------------*/
extern void         TtaSetSensibility (Document document, View view, int value);

/*----------------------------------------------------------------------
   TtaSetZoom

   Changes the current zoom used to display a given
   view of a given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.
   value: new value of the zoom.

  ----------------------------------------------------------------------*/
extern void         TtaSetZoom (Document document, View view, int value);

/*----------------------------------------------------------------------
   TtaGetFirstElementShown
   Returns the first element in a given view of a given document.

   Parameters:
   document: the document to which the element to be shown belongs.
   Cannot be 0.
   view: the view where the element must be shown.

   Returns:
   element: the first shown element.
   position: position of the top of the element in the window.
   Supposing that the y axis is oriented from the top
   of the window (coordinate 0) to the bottom (coordinate 100,
   whatever the actual height of the window), position is the
   desired y coordinate of the top of the element.
  ----------------------------------------------------------------------*/
extern Element        TtaGetFirstElementShown (Document document, View view, int *position);


/*----------------------------------------------------------------------
   TtaShowElement

   Shows a given element in a given view of a given document.

   Parameters:
   document: the document to which the element to be shown belongs.
   Cannot be 0.
   view: the view where the element must be shown.
   element: the element to be shown.
   position: position of the top of the element in the window.
   Supposing that the y axis is oriented from the top
   of the window (coordinate 0) to the bottom (coordinate 100,
   whatever the actual height of the window), position is the
   desired y coordinate of the top of the element.

  ----------------------------------------------------------------------*/
extern void         TtaShowElement (Document document, View view, Element element, int position);

/*----------------------------------------------------------------------
   TtaGetSensibility

   Reads the current sensibility used to display
   a given view of a given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.

   Return value:
   current value of the sensibility.

  ----------------------------------------------------------------------*/
extern int          TtaGetSensibility (Document document, View view);

/*----------------------------------------------------------------------
   TtaGetZoom

   Reads the current zoom used to display a given view
   of a given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.

   Return value:
   current value of the zoom.

  ----------------------------------------------------------------------*/
extern int          TtaGetZoom (Document document, View view);

/*----------------------------------------------------------------------
   TtaIsPSchemaValid

   Checks if a presentation schema can be applied to a document of a given
   class. No document is needed and the schemas are not loaded by this
   function.

   Parameters:
   structureName: Name of the document class.
   presentationName: Name of the presentation schema to be checked.

   Return value:
   1 if the presentation schema can be applied, 0 if it can not.

  ----------------------------------------------------------------------*/
extern int          TtaIsPSchemaValid (STRING structureName, STRING presentationName);

/*----------------------------------------------------------------------
   TtaGiveViewsToOpen

   Returns the names of the views that can be opened for a document.

   Parameters:
   document: the document.
   buffer: a buffer that will contain the result.

   Return parameters:
   buffer: list of view names. Each name is a character string with
   a final '\0'. Names of views that are already open have a '*'
   at the       end.
   nbViews: number of names in the list, 0 if not any view can be open.

  ----------------------------------------------------------------------*/
extern void         TtaGiveViewsToOpen (Document document, /*OUT*/ STRING buffer, /*OUT*/ int *nbViews);

/*----------------------------------------------------------------------
   TtaGetViewName

   Returns the name of an open view.

   Parameters:
   document: the document to which the view belongs.
   view: the view.

   Return value:
   Name of the view. The buffer must be provided by the caller.

  ----------------------------------------------------------------------*/
extern STRING       TtaGetViewName (Document document, View view);

/*----------------------------------------------------------------------
   TtaIsViewOpened

   Returns TRUE for a open view.

   Parameters:
   document: the document to which the view belongs.
   view: the view.

   Return value:
   TRUE or FALSE.

  ----------------------------------------------------------------------*/
extern ThotBool     TtaIsViewOpened (Document document, View view);

/*----------------------------------------------------------------------
   TtaGetViewFromName

   Returns the identifier of a view of a given document from its name.

   Parameters:
   document: the document to which the view belongs.
   viewName: the name of the view.

   Return value:
   the view. 0 if no view of that name is currently open for the document.

  ----------------------------------------------------------------------*/
extern View         TtaGetViewFromName (Document document, char* viewName);

/*----------------------------------------------------------------------
   Map and raise the corresponding window.                          
  ----------------------------------------------------------------------*/
extern void         TtaRaiseView (Document document, View view);

/*----------------------------------------------------------------------
   TtaGiveActiveView

   Returns the active view and the document to which that view belongs.
   The active view is the one that receives the characters typed by
   the user.

   Parameter:
   No parameter.

   Return values:
   document: the active document.
   view: the active view.

  ----------------------------------------------------------------------*/
extern void         TtaGiveActiveView (Document * document, /*OUT*/ View * view);

/*----------------------------------------------------------------------
  TtaClipPolyline update the Polyline box to fit the polyline bounding
  box. Need to be within a draw
  ----------------------------------------------------------------------*/
extern void         TtaClipPolyline (Element element, Document doc, View view);

/*----------------------------------------------------------------------
   TtaSetDisplayMode

   Changes display mode for a document. Three display modes are available.
   In the immediate mode, each modification made in the abstract tree of a
   document is immediately reflected in all opened views where the modification
   can be seen.

   In the deferred mode, the programmer can decide when the modifications are
   made visible to the user; this avoids the image of the document to blink when
   several elementary changes are made successively. Modifications are displayed
   when mode is changed to DisplayImmediately.

   In the NoComputedDisplay mode, the modifications are not displayed and they 
   are not computed inside the editor; the execution is more rapid but the current
   image is lost. When mode is changed to DisplayImmediately or DeferredMode,
   the image is completely redrawn by the editor.

   In the SuspendDisplay mode, the modifications are not displayed but stored 
   inside the editor; the execution is more and the current image is not lost.
   When mode is changed to DisplayImmediately or DeferredMode, the modifications
   are computed by the editor.

   An application that handles several documents at the same time can choose
   different modes for different documents. When a document is open or created,
   it is initially in the immediate mode.

   Parameters:
   document: the document.
   NewDisplayMode: new display mode for that document.

  ----------------------------------------------------------------------*/
extern void         TtaSetDisplayMode (Document document, DisplayMode newDisplayMode);

/*----------------------------------------------------------------------
   TtaGetDisplayMode

   Returns the current display mode for a document.

   Parameter:
   document: the document.

   Return value:
   current display mode for that document.

  ----------------------------------------------------------------------*/
extern DisplayMode  TtaGetDisplayMode (Document document);

/*----------------------------------------------------------------------
  TtaLockTableFormatting suspends all tables formatting
  ----------------------------------------------------------------------*/
extern void         TtaLockTableFormatting ();

/*----------------------------------------------------------------------
  TtaUnlockTableFormatting reformats all locked tables
  ----------------------------------------------------------------------*/
extern void         TtaUnlockTableFormatting ();
/*----------------------------------------------------------------------
  TtaGiveTableFormattingLock gives the status of the table formatting lock.
  ----------------------------------------------------------------------*/
extern void         TtaGiveTableFormattingLock (ThotBool *lock);

/*----------------------------------------------------------------------
   TtaListView

   Produces in a file a human-readable form of an abstract view.

   Parameters:
   document: the document.
   view: the view.
   fileDescriptor: file descriptor of the file that will contain the list.
   This file must be open when calling the function.

  ----------------------------------------------------------------------*/
extern void         TtaListView (Document document, View view, FILE * fileDescriptor);

/*----------------------------------------------------------------------
   TtaListBoxes

   Produces in a file a human-readable form of the boxes.

   Parameters:
   document: the document.
   view: the view.
   fileDescriptor: file descriptor of the file that will contain the list.
   This file must be open when calling the function.

  ----------------------------------------------------------------------*/
extern void         TtaListBoxes (Document document, View view, FILE * fileDescriptor);

/*----------------------------------------------------------------------
 * TtaGetThotColor returns the Thot Color.
 *            red, green, blue express the color RGB in 8 bits values
 ----------------------------------------------------------------------*/
extern int          TtaGetThotColor (unsigned short red, unsigned short green, unsigned short blue);

/*----------------------------------------------------------------------
   TtaClearViewSelections unselects and clears all current displayed
   selections.                                  
  ----------------------------------------------------------------------*/
extern void         TtaClearViewSelections ( void );

extern void         TtaChangeWindowTitle (Document, View, STRING);
#else  /* __STDC__ */

extern void         TtaGetViewWH ( /* Document doc, int view, int *w, int *h */ );
extern void         TtaGetViewGeometryRegistry ( /* Document document, char* name, int *x, int *y, int *width, int *height */ );
extern void         TtaGetViewGeometry ( /* Document document, CHAR_T* name, int *x, int *y, int *width, int *height */ );
extern void         TtaGetViewGeometryMM ( /* Document document, CHAR_T* name, int *x, int *y, int *width, int *height */ );
extern View         TtaOpenMainView ( /* Document document, int x, int y, int w, int h */ );
extern View         TtaOpenSubView ( /* Document document, STRING viewName, int x, int y, int w, int h, Element subtree */ );
extern void         TtaChangeViewTitle ( /*Document document, View view, STRING title */ );
extern void         TtaCloseView ( /* Document document, View view */ );
extern void         TtaFreeView ( /*Document document, View view */ );
extern void         TtaSetSensibility ( /* Document document, View view, int value */ );
extern void         TtaSetZoom ( /* Document document, View view, int value */ );
extern Element      TtaGetFirstElementShown (/* Document document, View view, int *position */);
extern void         TtaShowElement ( /* Document document, View view, Element element, int position */ );
extern int          TtaGetSensibility ( /* Document document, View view */ );
extern int          TtaGetZoom ( /* Document document, View view */ );
extern int          TtaIsPSchemaValid ( /* STRING structureName, STRING presentationName */ );
extern void         TtaGiveViewsToOpen ( /* Document document, STRING buffer, int *nbViews */ );
extern STRING       TtaGetViewName ( /* Document document, View view */ );
extern ThotBool     TtaIsViewOpened ( /*Document document, View view */ );
extern View         TtaGetViewFromName ( /* Document document, char* viewName */ );
extern void         TtaRaiseView ( /*Document document, View view */ );
extern void         TtaGiveActiveView ( /* Document *document, View *view */ );
extern void         TtaClipPolyline ( /* Element element, Document doc, View view */ );
extern void         TtaSetDisplayMode ( /* Document document, DisplayMode newDisplayMode */ );
extern DisplayMode  TtaGetDisplayMode ( /* Document document */ );
extern void         TtaLockTableFormatting ();
extern void         TtaUnlockTableFormatting ();
extern void         TtaGiveTableFormattingLock (/*ThotBool *lock*/);
extern void         TtaListView ( /*Document document, View view, FILE *fileDescriptor */ );
extern void         TtaListBoxes ( /*Document document, View view, FILE *fileDescriptor */ );
extern int          TtaGetThotColor ( /*unsigned short red, unsigned short green, unsigned short blue */ );
extern void         TtaClearViewSelections ( /*void*/ );
extern void         TtaChangeWindowTitle ();

#endif /* __STDC__ */
#endif
