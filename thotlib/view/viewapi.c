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

/*
 * Authors: V. Quint, I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) Unicode and Windows version
 */

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "application.h"
#include "document.h"
#include "view.h"
#include "tree.h"
#include "selection.h"
#include "typecorr.h"
#include "thotcolor.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "page_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "boxes_tv.h"
#include "platform_tv.h"
#include "thotcolor_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attrpresent_f.h"
#include "attributes_f.h"
#include "context_f.h"
#include "boxlocate_f.h"
#include "boxparams_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "createabsbox_f.h"
#include "displayview_f.h"
#include "fileaccess_f.h"
#include "frame_f.h"
#include "geom_f.h"
#include "inites_f.h"
#include "paginate_f.h"
#include "presentationapi_f.h"
#include "presrules_f.h"
#include "schemas_f.h"
#include "scroll_f.h"
#include "structcreation_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "structmodif_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "units_f.h"
#include "views_f.h"
#include "viewapi_f.h"

static CHAR_T         nameBuffer[MAX_NAME_LENGTH];


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

#ifdef __STDC__
View                TtaOpenMainView (Document document, int x, int y, int w, int h)

#else  /* __STDC__ */
View                TtaOpenMainView (document, x, y, w, h)
Document            document;
int                 x;
int                 y;
int                 w;
int                 h;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   int                 nView;
   View                view;

   UserErrorCode = 0;
   view = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (pDoc->DocSSchema != NULL)
	   if (pDoc->DocSSchema->SsPSchema == NULL)
	      TtaError (ERR_no_presentation_schema);
	   else
	     {
		/* Add a pagebreak probably missed at the end of the document */
		if (pDoc->DocSSchema->SsPSchema->PsPaginatedView[0])
		   AddLastPageBreak (pDoc->DocRootElement, 1, pDoc, FALSE);
		nView = CreateAbstractImage (pDoc, 1, 0, pDoc->DocSSchema, 1, TRUE, NULL);
		OpenCreatedView (pDoc, nView, FALSE, x, y, w, h);
		view = nView;
        /* TtaSetToggleItem (document, view, Special, BShowLogFile, FALSE); */
	     }
     }
   return view;
}


/*----------------------------------------------------------------------
   OpenView

   Opens a view for a document.

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
#ifdef __STDC__
static View         OpenView (Document document, STRING viewName, int x, int y, int w, int h, Element subtree)
#else  /* __STDC__ */
static View         OpenView (document, viewName, x, y, w, h, subtree)
Document            document;
STRING              viewName;
int                 x;
int                 y;
int                 w;
int                 h;
Element             subtree;
#endif /* __STDC__ */
{
   int                 nView;
   int                 nbViews;
   int                 i;
   int                 v;
   PtrDocument         pDoc;
   AvailableView       allViews;
   ThotBool            assoc;
   ThotBool            found;
   View                view;
   ThotBool            viewHasBeenOpen;

   UserErrorCode = 0;
   view = 0;
   v = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      TtaError (ERR_no_presentation_schema);
   else
     {
	pDoc = LoadedDocument[document - 1];
	assoc = FALSE;
	nView = 0;
	/* Enumerate the list of all pssible views for this document */
	nbViews = BuildDocumentViewList (pDoc, allViews);
	/* Looks in the list for the name of the view to be opened */
	found = FALSE;
	for (i = 0; i < nbViews && !found; i++)
	  {
	     found = ustrcmp (viewName, allViews[i].VdViewName) == 0;
	     if (found)
		v = i;
	  }
	if (found)
	  {
          viewHasBeenOpen = TRUE;

	     /* Open the view */
	     if (allViews[v].VdAssoc)
	       {
		  /* Add a page break probably missed at the end */
		  if (allViews[v].VdView > 0)
		     if (pDoc->DocSSchema->SsPSchema->PsPaginatedView[0])
			AddLastPageBreak (pDoc->DocAssocRoot[allViews[v].VdView - 1], 1, pDoc, FALSE);
		  nView = CreateAbstractImage (pDoc, 0, allViews[v].VdAssocNum,
		      allViews[v].VdSSchema, 1, TRUE, (PtrElement) subtree);
                  if (pDoc->DocAssocRoot[nView - 1] == NULL)
                    /*** Associated tree creation has been refused. ***/
                    viewHasBeenOpen = FALSE;
		  assoc = TRUE;
	       }
	     else
	       {
		  /* Add a page break probably missed at the end */
		  if (pDoc->DocSSchema->SsPSchema->PsPaginatedView[allViews[v].VdView])
		     AddLastPageBreak (pDoc->DocRootElement, allViews[v].VdView, pDoc, FALSE);
		  nView = CreateAbstractImage (pDoc, allViews[v].VdView, 0,
		     allViews[v].VdSSchema, 1, FALSE, (PtrElement) subtree);
		  assoc = FALSE;
	       }
	     if (nView == 0)
		TtaError (ERR_cannot_open_view);
	     else
	       {
                  if (viewHasBeenOpen)
                    {
		       OpenCreatedView (pDoc, nView, assoc, x, y, w, h);
		       if (assoc)
		          view = nView + 100;
		       else
		          view = nView;
                    }
	       }
	  }
     }
   return view;
}

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

#ifdef __STDC__
View                TtaOpenView (Document document, STRING viewName, int x, int y, int w, int h)
#else  /* __STDC__ */
View                TtaOpenView (document, viewName, x, y, w, h)
Document            document;
STRING              viewName;
int                 x;
int                 y;
int                 w;
int                 h;
#endif /* __STDC__ */

{
   return OpenView (document, viewName, x, y, w, h, NULL);
}

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

#ifdef __STDC__
View                TtaOpenSubView (Document document, STRING viewName, int x, int y, int w, int h, Element subtree)
#else  /* __STDC__ */
View                TtaOpenSubView (document, viewName, x, y, w, h, subtree)
Document            document;
STRING              viewName;
int                 x;
int                 y;
int                 w;
int                 h;
Element             subtree;
#endif /* __STDC__ */

{
   return OpenView (document, viewName, x, y, w, h, subtree);
}

/*----------------------------------------------------------------------
   TtaCloseView

   Closes a view.

   Parameters:
   document: the document for which a view must be closed.
   view: the view to be closed.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaCloseView (Document document, View view)
#else  /* __STDC__ */
void                TtaCloseView (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 numAssoc;

   UserErrorCode = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* View of the main tree */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		DestroyFrame (pDoc->DocViewFrame[view - 1]);
		CloseDocumentView (pDoc, view, FALSE, FALSE);
	     }
	else
	   /* View of associated elements */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
	       {
		  DestroyFrame (pDoc->DocAssocFrame[numAssoc - 1]);
		  CloseDocumentView (pDoc, numAssoc, TRUE, FALSE);
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   TtaChangeViewTitle

   Changes the title of a view.

   Parameters:
   document: the document.
   view: the view.
   title: the new title.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaChangeViewTitle (Document document, View view, STRING title)
#else  /* __STDC__ */
void                TtaChangeViewTitle (document, view, title)
Document            document;
View                view;
STRING              title;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 numAssoc;

   UserErrorCode = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* View of the main tree */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	      ChangeFrameTitle (pDoc->DocViewFrame[view - 1], title);
	else
	   /* View of associated elements */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
		ChangeFrameTitle (pDoc->DocAssocFrame[numAssoc - 1], title);
	  }
     }
}


/*----------------------------------------------------------------------
   TtaSetSensibility

   Changes the current sensibility used to display a given view of a
   given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.
   value: new value of the sensibility.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaSetSensibility (Document document, View view, int value)
#else  /* __STDC__ */
void                TtaSetSensibility (document, view, value)
Document            document;
View                view;
int                 value;
#endif /* __STDC__ */
{
   int                 frame;
   int                 valvisib, valzoom;

   UserErrorCode = 0;
   if (value < 1 || value > 10)
     TtaError (ERR_invalid_parameter);
   else
     {
       frame = GetWindowNumber (document, view);
       if (frame != 0)
	 {
	   GetFrameParams (frame, &valvisib, &valzoom);
	   /* Translation of the sensibility into threshold */
	   SetFrameParams (frame, value, valzoom);
	 }
       else
	 TtaError (ERR_invalid_parameter);
     }
}


/*----------------------------------------------------------------------
   TtaSetZoom

   Changes the current zoom used to display a given
   view of a given document.

   Parameters:
   document: the document. Cannot be 0.
   view: the view.
   value: new value of the zoom.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetZoom (Document document, View view, int value)
#else  /* __STDC__ */
void                TtaSetZoom (document, view, value)
Document            document;
View                view;
int                 value;
#endif /* __STDC__ */
{
  int                 frame;
  int                 valvisib, valzoom;

  UserErrorCode = 0;
  frame = GetWindowNumber (document, view);
  if (frame != 0)
    {
      GetFrameParams (frame, &valvisib, &valzoom);
      if (valzoom < -10 || valzoom > 10)
	TtaError (ERR_invalid_parameter);
      else
	{
	  /* Translation of the sensibility into threshold */
	  valzoom = value;
	  SetFrameParams (frame, valvisib, valzoom);
	}
    }
}


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
#ifdef __STDC__
Element             TtaGetFirstElementShown (Document document, View view, int *position)
#else  /* __STDC__ */
Element             TtaGetFirstElementShown (document, view, position)
Document            document;
View                view;
int                *position;
#endif /* __STDC__ */
{
  ViewFrame          *pFrame;
  PtrElement          pEl;
  PtrAbstractBox      pRootAb;
  PtrBox              pBox;
  int                 frame;
  int                 x, y, charsNumber;
  int                 width, height;

  UserErrorCode = 0;
  *position = 0;
  pEl = NULL;
  frame = GetWindowNumber (document, view);
  if (frame != 0)
    {
      pFrame = &ViewFrameTable[frame - 1];
      x = pFrame->FrXOrg;
      y = pFrame->FrYOrg;
      pRootAb = pFrame->FrAbstractBox;
      if (pRootAb != NULL && pRootAb->AbBox != NULL)
	{
	  charsNumber = 0;
	  /* recupere la boite selectionnee */
	  if (ThotLocalActions[T_selecbox] != NULL)
	    {
	      (*ThotLocalActions[T_selecbox]) (&pBox, pRootAb, frame, x, y, &charsNumber);
	      if (pBox != NULL && pBox->BxAbstractBox != NULL)
		{
		  if (pBox->BxType == BoPiece || pBox->BxType == BoSplit)
		    pBox = pBox->BxAbstractBox->AbBox->BxNexChild;
		  pEl = pBox->BxAbstractBox->AbElement;
		  /* width and height of the frame */
		  GetSizesFrame (frame, &width, &height);
		  /* position of the box top in the frame in % */
		  *position = (pBox->BxYOrg - pFrame->FrYOrg) * 100 / height;
		}
	    }
	}

    }
  return ((Element) pEl);
}


/*----------------------------------------------------------------------
   TtaShowElement
   Shows a given element in a given view of a given document.

   Parameters:
   document: the document to which the element to be shown belongs.
   Cannot be 0.
   view: the view where the element must be shown.
   element: the element to be shown.
   position: position of the top of the element in the window in %.
   Supposing that the y axis is oriented from the top
   of the window (coordinate 0) to the bottom (coordinate 100,
   whatever the actual height of the window), position is the
   desired y coordinate of the top of the element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaShowElement (Document document, View view, Element element, int position)
#else  /* __STDC__ */
void                TtaShowElement (document, view, element, position)
Document            document;
View                view;
Element             element;
int                 position;
#endif /* __STDC__ */
{
  int                 frame;
  int                 aView;
  PtrElement          pEl;

  UserErrorCode = 0;
  frame = GetWindowNumber (document, view);
  if (frame != 0)
    if (element == NULL)
      TtaError (ERR_invalid_parameter);
    else
      {
	if (view < 100)
	  aView = view;
	else
	  aView = 1;
	pEl = (PtrElement) element;
	/* If the first abstract box of the element is incomplete, it is suppressed */
	if (pEl->ElAbstractBox[aView - 1] != NULL)
	  if (pEl->ElAbstractBox[aView - 1]->AbLeafType == LtCompound)
	    if (pEl->ElAbstractBox[aView - 1]->AbTruncatedHead)
	      /* Destroying the abstract box of the element in this view */
	      DestroyAbsBoxesView (pEl, LoadedDocument[document - 1], FALSE, aView);
	/* and CheckAbsBox will rebuild it at the beginning of the element */
	CheckAbsBox (pEl, aView, LoadedDocument[document - 1], FALSE, FALSE);
	if (pEl->ElAbstractBox[aView - 1] != NULL)
	  ShowBox (frame, pEl->ElAbstractBox[aView - 1]->AbBox, 0, position);
      }
}


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
#ifdef __STDC__
int                 TtaGetSensibility (Document document, View view)
#else  /* __STDC__ */
int                 TtaGetSensibility (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
   int                 frame;
   int                 valzoom;
   int                 value;

   UserErrorCode = 0;
   value = 0;
   frame = GetWindowNumber (document, view);
   if (frame != 0)
     GetFrameParams (frame, &value, &valzoom);
   return value;
}				/*TtaGetSensibility */


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

#ifdef __STDC__
int                 TtaGetZoom (Document document, View view)
#else  /* __STDC__ */
int                 TtaGetZoom (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
   int                 frame;
   int                 valvisib;
   int                 value;

   UserErrorCode = 0;
   value = 0;
   frame = GetWindowNumber (document, view);
   if (frame != 0)
     GetFrameParams (frame, &valvisib, &value);
   return value;
}				/*TtaGetZoom */


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
#ifdef __STDC__
int                 TtaIsPSchemaValid (STRING structureName, STRING presentationName)
#else  /* __STDC__ */
int                 TtaIsPSchemaValid (structureName, presentationName)
STRING              structureName;
STRING              presentationName;
#endif /* __STDC__ */
{
   PathBuffer          DirBuffer;
   BinFile             file;
   CHAR_T                text[MAX_TXT_LEN];
   int                 i;
   Name                gotStructName;
   int                 result;

   UserErrorCode = 0;
   result = 0;
   /* Arrange the name of the file to be opened with the schema directory name */
   ustrncpy (DirBuffer, SchemaPath, MAX_PATH);
   MakeCompleteName (presentationName, PRS_EXT2, DirBuffer, text, &i);
   /* Checks if the file exists */
   file = TtaReadOpen (text);
   if (file == 0)
      /* presentation schema inaccessible */
      TtaError (ERR_cannot_load_pschema);
   else
     {
	/* Gets the corresponding structure schema name */
	TtaReadName (file, gotStructName);
	if (ustrcmp (structureName, gotStructName) == 0)
	   result = 1;
	TtaReadClose (file);
     }
   return result;
}


/*----------------------------------------------------------------------
   TtaGiveViewsToOpen

   Returns the names of the views that can be opened for a document.

   Parameters:
   document: the document.
   buffer: a buffer that will contain the result.

   Return parameters:
   buffer: list of view names. Each name is a character string with
   a final EOS. Names of views that are already open have a '*'
   at the       end.
   nbViews: number of names in the list, 0 if not any view can be open.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGiveViewsToOpen (Document document, STRING buffer, int *nbViews)
#else  /* __STDC__ */
void                TtaGiveViewsToOpen (document, buffer, nbViews)
Document            document;
STRING              buffer;
int                *nbViews;
#endif /* __STDC__ */
{

   UserErrorCode = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
      BuildViewList (LoadedDocument[document - 1], buffer, nbViews);
}


/*----------------------------------------------------------------------
   TtaGetViewName

   Returns the name of an open view.

   Parameters:
   document: the document to which the view belongs.
   view: the view.

   Return value:
   Name of the view. The buffer must be provided by the caller.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              TtaGetViewName (Document document, View view)
#else  /* __STDC__ */
STRING              TtaGetViewName (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrElement          pEl;
   DocViewDescr        dView;
   int                 numAssoc;

   UserErrorCode = 0;
   nameBuffer[0] = EOS;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* View of the main tree */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		dView = pDoc->DocView[view - 1];
		if (dView.DvSSchema != NULL || dView.DvPSchemaView != 0)
		   ustrncpy (nameBuffer, dView.DvSSchema->SsPSchema->PsView[dView.DvPSchemaView - 1], MAX_NAME_LENGTH);
	     }
	else
	   /* View of associated elements */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
	       {
		  pEl = pDoc->DocAssocRoot[numAssoc - 1];
		  if (pEl != NULL)
		     ustrncpy (nameBuffer, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName, MAX_NAME_LENGTH);
	       }
	  }
     }
   return nameBuffer;
}


/*----------------------------------------------------------------------
   TtaIsViewOpened

   Returns TRUE for a open view.

   Parameters:
   document: the document to which the view belongs.
   view: the view.

   Return value:
   TRUE or FALSE.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaIsViewOpened (Document document, View view)
#else  /* __STDC__ */
ThotBool            TtaIsViewOpened (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrElement          pEl;
   DocViewDescr        dView;
   int                 numAssoc;
   ThotBool            opened;

   UserErrorCode = 0;
   opened = FALSE;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* View of the main tree */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		dView = pDoc->DocView[view - 1];
		if (dView.DvSSchema != NULL || dView.DvPSchemaView != 0)
		   opened = TRUE;
	     }
	else
	   /* View of associated elements */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
	       {
		  pEl = pDoc->DocAssocRoot[numAssoc - 1];
		  if (pEl != NULL)
		     opened = TRUE;
	       }
	  }
     }
   return opened;
}

/*----------------------------------------------------------------------
   TtaGetViewFromName

   Returns the identifier of a view of a given document from its name.

   Parameters:
   document: the document to which the view belongs.
   viewName: the name of the view.

   Return value:
   the view. 0 if no view of that name is currently open for the document.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
View                TtaGetViewFromName (Document document, STRING viewName)
#else  /* __STDC__ */
View                TtaGetViewFromName (document, viewName)
Document            document;
STRING              viewName;
#endif /* __STDC__ */
{
   View                view;
   PtrDocument         pDoc;
   PtrElement          pEl;
   DocViewDescr        dView;
   int                 aView;

   UserErrorCode = 0;
   view = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (LoadedDocument[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	/* search in the opened views of the main tree */
	for (aView = 1; aView <= MAX_VIEW_DOC && view == 0; aView++)
	  {
	     dView = pDoc->DocView[aView - 1];
	     if (dView.DvSSchema != NULL && dView.DvPSchemaView != 0)
		if (ustrcmp (viewName, dView.DvSSchema->SsPSchema->PsView[dView.DvPSchemaView - 1]) == 0)
		   view = aView;
	  }
	if (view == 0)
	   /* If not found, searching in the views of associated elements */
	   for (aView = 1; aView <= MAX_ASSOC_DOC && view == 0; aView++)
	     {
		pEl = pDoc->DocAssocRoot[aView - 1];
		if (pEl != NULL)
		   if (pDoc->DocAssocFrame[aView - 1] != 0)
		      if (ustrcmp (viewName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName) == 0)
			 view = aView + 100;
	     }
     }
   return view;
}

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
#ifdef __STDC__
void                TtaGiveActiveView (Document * document, View * view)
#else  /* __STDC__ */
void                TtaGiveActiveView (document, view)
Document           *document;
View               *view;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 aView;
   ThotBool            assoc;

   UserErrorCode = 0;
   *document = 0;
   *view = 0;
   if (ActiveFrame != 0)
     {
	GetDocAndView (ActiveFrame, &pDoc, &aView, &assoc);
	if (pDoc != NULL)
	  {
	     *document = IdentDocument (pDoc);
	     if (assoc)
		*view = aView + 100;
	     else
		*view = aView;
	  }
     }
}


/*----------------------------------------------------------------------
   DisplayHolophrasted reaffiche un element sous forme holphrastee 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayHolophrasted (PtrElement pEl, Document document)
#else  /* __STDC__ */
void                DisplayHolophrasted (pEl, document)
PtrElement          pEl;
Document            document;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* Detruit les paves de l'element */
   DestroyAbsBoxes (pEl, pDoc, TRUE);
   /* reevalue l'image de toutes les vues */
   AbstractImageUpdated (pDoc);
   /* cree les paves de l'element dans la limite de la capacite' des vues
      ouvertes */
   BuildAbstractBoxes (pEl, pDoc);
   /* pas d'operation de reaffichage secondaires */
   /* reaffiche les paves construits */
   RedisplayCommand (document);
}


/*----------------------------------------------------------------------
   RedisplayReference reaffiche une reference qui vient d'etre        
   modifiee.                                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedisplayReference (PtrElement element, Document document)
#else  /* __STDC__ */
void                RedisplayReference (element, document)
PtrElement          element;
Document            document;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* supprime les anciens paves de la reference */
   DestroyAbsBoxes (element, pDoc, FALSE);
   AbstractImageUpdated (pDoc);
   ApplyTransmitRules (element, pDoc);
   RepApplyTransmitRules (element, element, pDoc);
   CreateAllAbsBoxesOfEl (element, pDoc);
   AbstractImageUpdated (pDoc);
   RedisplayCommand (document);
   /* reaffiche les paves qui copient l'element */
   RedisplayCopies (element, pDoc, (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
}

/*----------------------------------------------------------------------
   RedisplayLeaf reaffiche le contenu d'une feuille.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RedisplayLeaf (PtrElement element, Document document, int delta)
#else  /* __STDC__ */
void                RedisplayLeaf (element, document, delta)
PtrElement          element;
Document            document;
int                 delta;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 view, frame, h;
   PtrAbstractBox      pAbb;
   PtrAbstractBox      pAbbox1;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   for (view = 0; view < MAX_VIEW_DOC; view++)
      if (element->ElAbstractBox[view] != NULL
	  && !element->ElAbstractBox[view]->AbNew
	  && !element->ElAbstractBox[view]->AbDead)
	 /* un pave correspondant existe dans la vue view */
	 /* met a jour le volume dans les paves englobants */
	{
	   if (delta != 0)
	     {
		pAbb = element->ElAbstractBox[view]->AbEnclosing;
		while (pAbb != NULL)
		  {
		     pAbb->AbVolume += delta;
		     pAbb = pAbb->AbEnclosing;
		  }
	     }
	   /* met a jour le contenu et le volume et demande le */
	   /* reaffichage du pave */
	   pAbbox1 = element->ElAbstractBox[view];
	   /* saute les paves de presentation */
	   while (pAbbox1->AbElement == element && pAbbox1->AbPresentationBox &&
		  pAbbox1->AbNext != NULL)
	      pAbbox1 = pAbbox1->AbNext;
	   pAbbox1->AbVolume += delta;
	   pAbbox1->AbChange = TRUE;
	   switch (element->ElLeafType)
	     {
	     case LtPicture:
	       /* Rien a faire */
	       break;
	     case LtText:
	       pAbbox1->AbText = element->ElText;
	       pAbbox1->AbLanguage = element->ElLanguage;
	       break;
	     case LtPolyLine:
	       pAbbox1->AbLeafType = LtPolyLine;
	       pAbbox1->AbPolyLineBuffer = element->ElPolyLineBuffer;
	       pAbbox1->AbPolyLineShape = element->ElPolyLineType;
	       pAbbox1->AbVolume = element->ElNPoints;
	       break;
	     case LtSymbol:
	     case LtGraphics:
	       pAbbox1->AbLeafType = element->ElLeafType;
	       pAbbox1->AbShape = element->ElGraph;
	       pAbbox1->AbGraphAlphabet = TEXT('G');
	       if (element->ElLeafType == LtGraphics &&
		   element->ElGraph == TEXT('a') &&
		   pAbbox1->AbHeight.DimAbRef == NULL)
		 {
		   /* force the circle height to be equal to its width */
		   pAbbox1->AbHeight.DimAbRef = pAbbox1;
		   pAbbox1->AbHeight.DimSameDimension = FALSE;
		   pAbbox1->AbHeight.DimValue = 0;
		   pAbbox1->AbHeight.DimUserSpecified = FALSE;
		   if (pAbbox1->AbWidth.DimUnit == UnPoint)
		     pAbbox1->AbHeight.DimUnit = UnPoint;
		   else
		     pAbbox1->AbHeight.DimUnit = UnPixel;
		   pAbbox1->AbHeightChange = TRUE;
		 }
	       break;
	     default:
	       break;
	     }
	   /* un pave correspondant existe dans la vue view */
	   if (AssocView (element))
	     /* vue d'element associe */
	     frame = pDoc->DocAssocFrame[element->ElAssocNum - 1];
	   else
	     frame = pDoc->DocViewFrame[view];
	   
	   if (pAbbox1 != NULL)
	     {
	       h = 0;
	       /* on ne s'occupe pas de la hauteur de page */
	       ChangeConcreteImage (frame, &h, pAbbox1);
	     }
	}

   RedisplayCommand (document);
   /* si l'element modifie' appartient soit a un element copie' */
   /* dans des paves par une regle Copy, soit a un element inclus */
   /* dans d'autres, il faut reafficher ses copies */
   RedisplayCopies (element, pDoc, (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
}


/*----------------------------------------------------------------------
   RedisplaySplittedText reaffiche le contenu d'un texte divise'.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RedisplaySplittedText (PtrElement element, int position, PtrElement pNewEl, PtrElement  pNextEl, Document document)
#else  /* __STDC__ */
void                RedisplaySplittedText (element, position, pNewEl, pNextEl, document)
PtrElement          element;
int                 position;
PtrElement	    pNewEl;
PtrElement	    pNextEl;
Document            document;
#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   pEl = (PtrElement) element;
   /* check current selection */
   if (SelectedDocument == pDoc)
     {
	/* current selection is in that document */
	if (pEl == LastSelectedElement)
	   if (position < LastSelectedChar)
	     {
		LastSelectedElement = pNewEl;
		LastSelectedChar -= position;
	     }
	if ((PtrElement) element == FirstSelectedElement)
	   if (position < FirstSelectedChar)
	     {
		FirstSelectedElement = pNewEl;
		FirstSelectedChar -= position;
	     }
     }
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   BuildAbsBoxSpliText ((PtrElement) element, pNewEl, pNextEl, pDoc);
   AbstractImageUpdated (pDoc);
   RedisplayCommand (document);
}

/*----------------------------------------------------------------------
   RedisplayMergedText reaffiche le contenu d'un texte fusionne'.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RedisplayMergedText (PtrElement element, Document document)
#else  /* __STDC__ */
void                RedisplayMergedText (element, document)
PtrElement          element;
Document            document;
#endif /* __STDC__ */
{
   PtrElement          pEl, pENeighbour;
   ThotBool            stop;
   PtrDocument         pDoc;
   int                 view, dvol, h, frame;
   PtrAbstractBox      pAb;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (pDoc->DocSSchema->SsPSchema == NULL)
      return;
   pEl = element;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* teste si pEl est le dernier fils de son pere, */
   /* abstraction faite des marques de page */
   pENeighbour = pEl->ElNext;
   stop = FALSE;
   do
     if (pENeighbour == NULL)
       /* pEl devient le dernier fils de son pere */
       {
	 ChangeFirstLast (pEl, pDoc, FALSE, FALSE);
	 stop = TRUE;
       }
     else if (!pENeighbour->ElTerminal || pENeighbour->ElLeafType != LtPageColBreak)
       stop = TRUE;
     else
       pENeighbour = pENeighbour->ElNext;
   while (!(stop));

   /* met a jour le volume des paves correspondants */
   for (view = 0; view < MAX_VIEW_DOC; view++)
     {
       pAb = pEl->ElAbstractBox[view];
       if (pAb != NULL)
	 {
	   dvol = pEl->ElTextLength - pAb->AbVolume;
	   pAb->AbVolume += dvol;
	   pAb->AbChange = TRUE;
	   if (!AssocView (pEl))
	     {
	       pDoc->DocViewModifiedAb[view] = Enclosing (pAb, pDoc->DocViewModifiedAb[view]);
	       frame = pDoc->DocViewFrame[view];
	     }
	   else
	     {
	       pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] = Enclosing (pAb, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
	       frame = pDoc->DocAssocFrame[pEl->ElAssocNum - 1];
	     }
	   h = 0;		/* on ne s'occupe pas de la hauteur de page */
	   ChangeConcreteImage (frame, &h, pAb->AbEnclosing);
	   if (pAb->AbDead && pAb->AbNext != NULL)
	     pAb->AbNext->AbVolume += dvol;
	   do
	     {
	       pAb->AbVolume += dvol;
	       pAb = pAb->AbEnclosing;
	     }
	   while (!(pAb == NULL));
	 }
     }
   RedisplayCommand (document);
}

/*----------------------------------------------------------------------
   UndisplayInheritedAttributes      supprime sur l'element pEl et son       
   sous-arbre la presentation liee a l'attribut decrit par pAttr   
   ou a l'heritage d'un tel attribut.                              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                UndisplayInheritedAttributes (PtrElement pEl, PtrAttribute pAttr, Document document, ThotBool suppression)
#else  /* __STDC__ */
void                UndisplayInheritedAttributes (pEl, pAttr, document, suppression)
PtrElement          pEl;
PtrAttribute        pAttr;
Document            document;
ThotBool            suppression;
#endif /* __STDC__ */
{
   ThotBool            inheritance, comparaison;
   PtrAttribute        pAttrAsc;
   PtrAttribute        pOldAttr;
   PtrElement          pElChild, pElAttr;

   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   if (suppression)
      pOldAttr = pAttr;
   else
      /* l'element porte-t-il deja un attribut du meme type ? */
      pOldAttr = AttributeValue (pEl, pAttr);
   /* doit-on se preoccuper des heritages et comparaisons d'attributs? */
   inheritance = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNHeirElems[pAttr->AeAttrNum - 1] > 0);
   comparaison = (pAttr->AeAttrSSchema->SsPSchema->
		  PsNComparAttrs[pAttr->AeAttrNum - 1] > 0);
   if (inheritance || comparaison)
      /* cherche le premier attribut de meme type pose' sur un ascendant */
      /* de pEl */
      pAttrAsc = GetTypedAttrAncestor (pEl, pAttr->AeAttrNum,
				       pAttr->AeAttrSSchema, &pElAttr);
   else
      pAttrAsc = NULL;
   if (pOldAttr != NULL)
      /* oui, on s'en occupe */
     {
	/* on supprime d'abord les regles de presentation liees */
	/* a l'attribut sur l'element lui-meme */
	RemoveAttrPresentation (pEl, LoadedDocument[document - 1], pOldAttr,
				pEl, FALSE, NULL);
	/* puis on supprime sur pEl et sur les elements du sous arbre pEl */
	/* les regles de presentation liees a l'heritage de cet attribut */
	/* par le sous-arbre s'il existe des elements heritants de celui-ci */
	if (inheritance)
	   RemoveInheritedAttrPresent (pEl, LoadedDocument[document - 1],
				       pOldAttr, pEl);
	/* puis on supprime sur les elements du sous-arbre pEl */
	/* les regles de presentation liees a la comparaison d'un attribut */
	/* du sous-arbre avec ce type d'attribut */
	if (!pEl->ElTerminal && comparaison)
	   for (pElChild = pEl->ElFirstChild; pElChild != NULL;
		pElChild = pElChild->ElNext)
	      RemoveComparAttrPresent (pElChild, LoadedDocument[document - 1],
				       pOldAttr);
     }
   else if (pAttrAsc != NULL)
     {
	/* heritage et comparaison sont lies a un attribut porte' par un */
	/* ascendant de pEl */
	/* on supprime sur le sous arbre pEl les regles de presentation */
	/* liees a l'heritage de cet attribut par le sous-arbre s'il */
	/* existe des elements heritants de celui-ci */
	if (inheritance)
	   RemoveInheritedAttrPresent (pEl, LoadedDocument[document - 1],
				       pAttrAsc, pElAttr);
	/* puis on supprime sur le sous-arbre pEl les regles de */
	/* presentation liees a la comparaison d'un attribut */
	/* du sous-arbre avec ce type d'attribut */
	if (comparaison)
	   RemoveComparAttrPresent (pEl, LoadedDocument[document-1], pAttrAsc);
     }
}

/*----------------------------------------------------------------------
   UndisplayElement "desaffiche" un element qui va etre retire'    
   de son arbre abstrait.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UndisplayElement (PtrElement pEl, Document document)
#else  /* __STDC__ */
void                UndisplayElement (pEl, document)
PtrElement          pEl;
Document            document;
#endif /* __STDC__ */
{
   PtrElement          pNext, pPrevious, pFather, pNeighbour, pE, pSS;
   ThotBool            stop;
   PtrDocument         pDoc;
   int                 savePageHeight;
   int                 assoc;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, */
   /* on retire simplement l'element de l'arbre abstrait */
   if (pDoc->DocSSchema->SsPSchema == NULL)
     {
	RemoveElement (pEl);
	return;
     }
   /* si le document est en mode de non calcul de l'image, */
   /* on retire simplement l'element de l'arbre abstrait */
   /* sauf si c'est la racine car son pave n'avait pas ete detruit */
   /* il faut donc executer la suite de la procedure */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      if (pEl->ElParent == NULL)
	{
	   DestroyAbsBoxes (pEl, pDoc, TRUE);
	   RemoveElement (pEl);
	   if (pEl != pDoc->DocRootElement)
	     {
		for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
		   if (pDoc->DocAssocRoot[assoc] == pEl)
		      break;	/* C'est une racine associee */
		if (assoc == MAX_ASSOC_DOC)
		   /* Ce n'est pas une racine ! */
		   return;
	     }
	}
      else
	{
	   RemoveElement (pEl);
	   return;
	}
   if (ThotLocalActions[T_createhairline] != NULL)
     (*ThotLocalActions[T_checksel]) (pEl, document);
   /* cherche l'element qui precede l'element a detruire : pPrevious */
   pPrevious = pEl->ElPrevious;
   /* saute les marques de page */
   stop = FALSE;
   do
      if (pPrevious == NULL)
	 stop = TRUE;
      else if (!pPrevious->ElTerminal || pPrevious->ElLeafType != LtPageColBreak)
	 stop = TRUE;
      else
	 pPrevious = pPrevious->ElPrevious;
   while (!(stop));
   /* cherche le premier element apres l'element a detruire : pNext */
   pNext = NextElement (pEl);
   stop = FALSE;
   do
      if (pNext == NULL)
	 stop = TRUE;
      else if (pNext->ElTerminal && pNext->ElLeafType == LtPageColBreak)
	 pNext = NextElement (pNext);
      else
	 stop = TRUE;
   while (!(stop));
   if (pNext == NULL)
      pNext = NextElement (pPrevious);
   DestroyAbsBoxes (pEl, pDoc, TRUE);
   pFather = pEl->ElParent;
   RemoveElement (pEl);
   /* cherche l'element a partir duquel il faudra transmettre les */
   /* compteurs */
   if (pPrevious != NULL)
      pSS = pPrevious;
   else
      pSS = pFather;
   /* met a jour les numeros concerne's */
   pE = pEl;
   if (pNext != NULL)
     {
	/* il ne faut pas reafficher les numeros mis a jour si on est */
	/* en mode d'affichage differe'. Or, lorsque PageHeight != 0, */
	/* UpdateNumbers ne reaffiche pas les numeros qui changent. */
	savePageHeight = PageHeight;
	if (documentDisplayMode[document - 1] == DeferredDisplay)
	   PageHeight = 1;
	while (pE != NULL)
	  {
	     UpdateNumbers (pNext, pE, pDoc, (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
	     pE = pE->ElNext;
	  }
	PageHeight = savePageHeight;
     }
   if (pNext != NULL)
     {
	pNeighbour = pNext->ElPrevious;
	stop = FALSE;
	do
	   if (pNeighbour == NULL)
	      stop = TRUE;
	   else if (!pNeighbour->ElTerminal
		    || pNeighbour->ElLeafType != LtPageColBreak)
	      stop = TRUE;
	   else
	      pNeighbour = pNeighbour->ElPrevious;
	while (!(stop));
	if (pNeighbour == NULL)
	   /* l'element qui suit la partie detruite devient premier */
	   ChangeFirstLast (pNext, pDoc, TRUE, FALSE);
     }
   if (pPrevious != NULL)
     {
	pNeighbour = pPrevious->ElNext;
	stop = FALSE;
	do
	   if (pNeighbour == NULL)
	      stop = TRUE;
	   else if (!pNeighbour->ElTerminal
		    || pNeighbour->ElLeafType != LtPageColBreak)
	      stop = TRUE;
	   else
	      pNeighbour = pNeighbour->ElNext;
	while (!(stop));
	if (pNeighbour == NULL)
	   /* l'element qui precede la partie detruite devient dernier */
	   ChangeFirstLast (pPrevious, pDoc, FALSE, FALSE);
	/* traitement particulier aux tableaux */
	if (ThotLocalActions[T_createhairline] != NULL)
	   (*ThotLocalActions[T_createhairline]) (pPrevious, pEl, pDoc);
     }
   /* reevalue l'image de toutes les vues */
   AbstractImageUpdated (pDoc);
   RedisplayCommand (document);
   if (pEl != NULL)
     {
	/* reaffiche les paves qui copient les elements detruits */
	RedisplayCopies (pEl, pDoc, (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
	/* la renumerotation est faite plus haut */
	/* reaffiche les references aux elements detruits */
	/* et enregistre les references sortantes coupees */
	/* ainsi que les elements coupe's qui sont reference's par */
	/* d'autres documents */
	RedisplayEmptyReferences (pEl, &pDoc, (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
	/* Retransmet les valeurs des compteurs et attributs TRANSMIT */
	/* s'il y a des elements apres */
	if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrRefImportedDoc)
	   RepApplyTransmitRules (pEl, pSS, pDoc);
     }
}

/*----------------------------------------------------------------------
   RedispNewGeometry                                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RedispNewGeometry (Document document, PtrElement pEl)
#else  /* __STDC__ */
void                RedispNewGeometry (document, pEl)
Document            document;
PtrElement          pEl;
#endif /* __STDC__ */
{
   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (LoadedDocument[document - 1]->DocSSchema->SsPSchema == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* la nouvelle regle de presentation doit etre prise en compte dans */
   /* les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
}


/*----------------------------------------------------------------------
  TtaClipPolyline update the Polyline box to fit the polyline bounding
  box. Need to be within a draw
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaClipPolyline (Element element, Document doc, View view)
#else  /* __STDC__ */
void                TtaClipPolyline (element, doc, view)
Element             element;
Document            doc;
View                view;
#endif /* __STDC__ */
{
  PtrDocument         pDoc;
  PtrAbstractBox      pAb;
  PtrBox              pBox;
  PtrTextBuffer       Bbuffer;
  int                 xMin, yMin, xMax, yMax;
  int                 x, y, width, height;
  int                 nbpoints, i, j;
  int                 frame;

  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) element)->ElLeafType != LtPolyLine)
    TtaError (ERR_invalid_element_type);
  else if (doc < 1 || doc > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      pDoc = LoadedDocument[doc - 1];
      if (pDoc == NULL)
	TtaError (ERR_invalid_document_parameter);
      else
	{
	  /* parameter document is correct */
	  frame = GetWindowNumber (doc, view);
	  pAb = pDoc->DocViewRootAb[view - 1];
	  if (pAb != NULL && pAb->AbBox != NULL)
	    {
	      pBox = pAb->AbBox;
	      /* it's a polyline: check the box limits */
	      Bbuffer = pBox->BxBuffer;
	      xMin = PixelToPoint (pBox->BxWidth) * 1000;
	      xMax = 0;
	      yMin = PixelToPoint (pBox->BxHeight) * 1000;
	      yMax = 0;
	      nbpoints = pBox->BxNChars;
	      if (nbpoints == 0)
		return;
	      j = 0;
	      for (i = 1; i < nbpoints; i++)
		{
		  if (j >= Bbuffer->BuLength)
		    {
		      if (Bbuffer->BuNext != NULL)
			{
			  /* Next buffer */
			  Bbuffer = Bbuffer->BuNext;
			  j = 0;
			}
		    }
		  x = Bbuffer->BuPoints[j].XCoord;
		  y = Bbuffer->BuPoints[j].YCoord;
		  /* register the min and the max */
		  if (x < xMin)
		    xMin = x;
		  if (x > xMax)
		    xMax = x;
		  if (y < yMin)
		    yMin = y;
		  if (y > yMax)
		    yMax = y;
		  j++;
		}
	      x = pBox->BxXOrg;
	      y = pBox->BxYOrg;
	      /* pack the box and return the new origins */
	      SetBoundingBox (xMin, xMax, &x, yMin, yMax, &y, pBox, nbpoints);
	      if (x != pBox->BxXOrg || y != pBox->BxYOrg)
		NewPosition (pAb, x, y, frame, TRUE);
	      width = PointToPixel (pBox->BxBuffer->BuPoints[0].XCoord / 1000);
	      height = PointToPixel (pBox->BxBuffer->BuPoints[0].YCoord / 1000);
	      if (width != pBox->BxWidth || height != pBox->BxHeight)
		NewDimension (pAb, width, height, frame, TRUE);
	    }
	}
    }
}

/* End of module */
