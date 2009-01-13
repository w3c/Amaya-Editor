/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Authors: V. Quint, I. Vatton (INRIA)
 */
#include "thot_gui.h"
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
#include "appdialogue_wx_f.h"
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
#include "unstructlocate_f.h"
#include "views_f.h"
#include "viewapi_f.h"

#include "AmayaParams.h"
#include "appdialogue_wx_f.h"

static char           nameBuffer[MAX_NAME_LENGTH];

/*----------------------------------------------------------------------
  TtaOpenMainView

  Opens the main view of a document. This document must have a PSchema
  (see TtaSetPSchema).
  Parameters:
  document: the document for which a window must be open.
  doctypename: the document type name used later to configure WX interface (icon tabs)
  x, y: coordinate (in millimeters) of the upper left corner of the
  window that will display the view.
  w, h: width and height (in millimeters) of the upper left corner of the
  window that will display the view.
  withMenu is TRUE when menus are displayed
  withButton is TRUE when buttons are displayed
  Return value:
  the view opened or 0 if the view cannot be opened.
  ----------------------------------------------------------------------*/
View TtaOpenMainView ( Document document, const char * doctypename,
                       int x, int y, int w, int h,
                       ThotBool withMenu, ThotBool withButton,
                       int window_id, int page_id, int page_position )
{
  PtrDocument         pDoc;
  PtrPSchema          pPS;
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
        {
          pPS = PresentationSchema (pDoc->DocSSchema, pDoc);
          if (pPS == NULL)
            TtaError (ERR_no_presentation_schema);
          else
            {
              /* store the document type name */
              pDoc->DocTypeName = TtaStrdup(doctypename);
              /* Add a pagebreak probably missed at the end of the document */
              if (pPS->PsPaginatedView[0])
                AddLastPageBreak (pDoc->DocDocElement, 1, pDoc, FALSE);
              nView = CreateAbstractImage (pDoc, 1, pDoc->DocSSchema, 1,
                                           TRUE, NULL);
              OpenCreatedView (pDoc, nView, x, y, w, h, withMenu, withButton,
                               window_id, page_id, page_position, "Formatted_view");
              view = nView;
            }
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
static View OpenView (Document document, const char *viewName,
                      int x, int y, int w, int h,
                      Element subtree)
{
  int                 nView;
  int                 nbViews;
  int                 i;
  int                 v;
  PtrDocument         pDoc;
  PtrPSchema          pPS;
  AvailableView       allViews;
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
    {
      pPS = PresentationSchema (LoadedDocument[document - 1]->DocSSchema,
                                LoadedDocument[document - 1]);
      if (pPS == NULL)
        TtaError (ERR_no_presentation_schema);
      else
        {
          pDoc = LoadedDocument[document - 1];
          nView = 0;
          /* Enumerate the list of all pssible views for this document */
          nbViews = BuildDocumentViewList (pDoc, allViews);
          /* Looks in the list for the name of the view to be opened */
          found = FALSE;
          for (i = 0; i < nbViews && !found; i++)
            {
              found = strcmp (viewName, allViews[i].VdViewName) == 0;
              if (found)
                v = i;
            }
          if (found)
            {
              viewHasBeenOpen = TRUE;
	    
              /* Open the view */
              /* Add a page break probably missed at the end */
              if (pPS->PsPaginatedView[allViews[v].VdView-1])
                AddLastPageBreak (pDoc->DocDocElement, allViews[v].VdView,
                                  pDoc, FALSE);
              nView = CreateAbstractImage (pDoc, allViews[v].VdView,
                                           allViews[v].VdSSchema, 1, FALSE,
                                           (PtrElement) subtree);
              if (nView == 0)
                TtaError (ERR_cannot_open_view);
              else
                {
                  if (viewHasBeenOpen)
                    {
                      /* look for the current windows, current page, and current page position (top/bottom)*/
                      int window_id = TtaGetDocumentWindowId (document, -1);
                      int page_id;
                      int page_position;
                      TtaGetDocumentPageId (document, -1, &page_id, &page_position);
		    
                      /* force to see the view in the second frame */
                      page_position = 2;
                      OpenCreatedView (pDoc, nView, x, y, w, h, TRUE, TRUE,
                                       window_id, page_id, page_position, viewName);
                      view = nView;
                    }
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
View TtaOpenView (Document document, const char *viewName, int x, int y, int w, int h)
{
  return OpenView (document, viewName, x, y, w, h, NULL);
}

/*----------------------------------------------------------------------
  TtaOpenSubView

  Opens a view that shows only a subtree. This document must have a
  PSchema (see TtaSetPSchema).
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
View TtaOpenSubView (Document document, char *viewName, int x, int y,
                     int w, int h, Element subtree)
{
  return OpenView (document, viewName, x, y, w, h, subtree);
}

/*----------------------------------------------------------------------
  TtcCloseView

  Closes a view.
  Parameters:
  document: the document for which a view must be closed.
  view: the view to be closed.
  ----------------------------------------------------------------------*/
void TtaCloseView (Document document, View view)
{
  PtrDocument         pDoc;

  /* Checks the parameter document */
  if (document >= 1 && document <= MAX_DOCUMENTS &&
      LoadedDocument[document - 1])
    {
      pDoc = LoadedDocument[document - 1];
      DestroyFrame (pDoc->DocViewFrame[view - 1]);
      CloseDocumentView (pDoc, view, FALSE);
    }
}

/*----------------------------------------------------------------------
  TtaChangeWindowTitle

  Changes the title of a view.
  if view == 0, changes the title of all windows of document
  otherwise change the window title of the specified view.
  Parameters:
  document: the document.
  view: the view.
  title: the new title.
  encoding: the title encoding;
  ----------------------------------------------------------------------*/
void TtaChangeWindowTitle (Document document, View view, char *title,
                           CHARSET encoding)
{
  int          idwindow, v;
  PtrDocument  pDoc;
  
  if (document < 1 || document > MAX_DOCUMENTS)
    return;
  if (view > 0)
    {
      idwindow = GetWindowNumber (document, view);
      if (idwindow > 0) 
        ChangeFrameTitle (idwindow, (unsigned char *)title, encoding);
    }
  else
    {
      pDoc = LoadedDocument[document - 1];
      /* traite les vues du document */
      for (v = 0; v < MAX_VIEW_DOC; v++)
        if (pDoc->DocView[v].DvPSchemaView > 0)
          ChangeFrameTitle (pDoc->DocViewFrame[v], (unsigned char *)title, encoding);
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
void TtaSetSensibility (Document document, View view, int value)
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
  view: the view. (-1 to set all views)
  value: new value of the zoom.
  ----------------------------------------------------------------------*/
void TtaSetZoom (Document document, View view, int value)
{
  int                 frame;
  int                 valvisib, valzoom;

  /* if view == -1, loop on every views */
  if (view == -1)
    {
      view = 1;
      while( view <= MAX_VIEW_DOC )
        {
          TtaSetZoom(document, view, value);
          view++;
        }
      return;
    }
  
  UserErrorCode = 0;
  frame = GetWindowNumber (document, view);
  if (frame != 0)
    {
      GetFrameParams (frame, &valvisib, &valzoom);
      if (valzoom < -10 || valzoom > 10)
        TtaError (ERR_invalid_parameter);
      else if (valzoom != value)
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
Element TtaGetFirstElementShown (Document document, View view, int *position)
{
  ViewFrame          *pFrame;
  PtrElement          pEl;
  PtrAbstractBox      pRootAb;
  PtrBox              pBox;
  PtrFlow             pFlow = NULL;
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
#ifndef _GL
      x = pFrame->FrXOrg;
      y = pFrame->FrYOrg;
#else /* _GL */
      x = y = 0;
#endif /* _GL */
      pRootAb = pFrame->FrAbstractBox;
      if (pRootAb != NULL && pRootAb->AbBox != NULL)
        {
          charsNumber = 0;
          /* recupere la boite selectionnee */
          GetClickedBox (&pBox, &pFlow, pRootAb, frame,
                         x, y, 1, &charsNumber);
          if (pBox && pBox->BxAbstractBox)
            {
              if (pBox->BxType == BoPiece ||
                  pBox->BxType == BoScript ||
                  pBox->BxType == BoMulScript ||
                  pBox->BxType == BoSplit)
                pBox = pBox->BxAbstractBox->AbBox->BxNexChild;
              pEl = pBox->BxAbstractBox->AbElement;
              /* width and height of the frame */
              GetSizesFrame (frame, &width, &height);
              /* position of the box top in the frame in % */
              y = pBox->BxYOrg - pFrame->FrYOrg;
              if (pFlow)
                y += pFlow->FlYStart;
              *position = (y) * 100 / height;
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
void TtaShowElement (Document document, View view, Element element, int position)
{
  PtrElement          pEl;
  int                 frame;
  int                 aView;
  
  UserErrorCode = 0;
  frame = GetWindowNumber (document, view);
  if (frame != 0)
    {
      if (element == NULL)
        TtaError (ERR_invalid_parameter);
      else
        {
          if (view < 100)
            aView = view;
          else
            aView = 1;
          pEl = (PtrElement) element;
          /* If the first abstract box of the element is incomplete,
             it is suppressed */
          if (pEl->ElAbstractBox[aView - 1] != NULL &&
              pEl->ElAbstractBox[aView - 1]->AbLeafType == LtCompound &&
              pEl->ElAbstractBox[aView - 1]->AbTruncatedHead)
            /* don't destroy the root box */
            if (pEl->ElParent)
              {
                /* Destroying the abstract box of the element in this view */
                DestroyAbsBoxesView (pEl, LoadedDocument[document - 1], FALSE,
                                     aView);
                /* reevalue l'image de toutes les vues */
                AbstractImageUpdated (LoadedDocument[document - 1]);
              }
          /* and CheckAbsBox will rebuild it at the beginning of the element */
          CheckAbsBox (pEl, aView, LoadedDocument[document - 1], FALSE, TRUE);
          if (pEl->ElAbstractBox[aView - 1])
            ShowBox (frame, pEl->ElAbstractBox[aView - 1]->AbBox, 0, position, TRUE);
          else 
            {
              pEl = pEl->ElParent;
              while (pEl)
                {
                  if (pEl->ElAbstractBox[aView - 1])
                    {
                      ShowBox (frame, pEl->ElAbstractBox[aView - 1]->AbBox,
                               0, position, TRUE);
                      pEl = NULL;
                    }
                  else
                    pEl = pEl->ElParent;
                }
            }
        }
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
int TtaGetSensibility (Document document, View view)
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
}


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
int TtaGetZoom (Document document, View view)
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
}


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
int TtaIsPSchemaValid (char *structureName, char *presentationName)
{
  PathBuffer          DirBuffer;
  BinFile             file;
  char                  text[MAX_TXT_LEN];
  int                 i;
  Name                gotStructName;
  int                 result;

  UserErrorCode = 0;
  result = 0;
  /* Arrange the name of the file to be opened with the schema directory
     name */
  strncpy (DirBuffer, SchemaPath, MAX_PATH);
  MakeCompleteName (presentationName, "PRS", DirBuffer, text, &i);
  /* Checks if the file exists */
  file = TtaReadOpen (text);
  if (file == 0)
    /* presentation schema inaccessible */
    TtaError (ERR_cannot_load_pschema);
  else
    {
      /* Gets the corresponding structure schema name */
      TtaReadName (file, (unsigned char *)gotStructName);
      if (strcmp (structureName, gotStructName) == 0)
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
void TtaGiveViewsToOpen (Document document, char *buffer, int *nbViews)
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
char *TtaGetViewName (Document document, View view)
{
  PtrDocument         pDoc;
  PtrPSchema          pPS;
  DocViewDescr        dView;

  UserErrorCode = 0;
  nameBuffer[0] = EOS;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (view < 1 || view > MAX_VIEW_DOC)
    TtaError (ERR_invalid_parameter);
  else
    {
      pDoc = LoadedDocument[document - 1];
      dView = pDoc->DocView[view - 1];
      if (dView.DvSSchema != NULL || dView.DvPSchemaView != 0)
        {
          pPS = PresentationSchema (dView.DvSSchema, pDoc);
          strncpy (nameBuffer, pPS->PsView[dView.DvPSchemaView - 1],
                   MAX_NAME_LENGTH);
        }
    }
  return nameBuffer;
}


/*----------------------------------------------------------------------
  TtaIsViewOpen

  Returns TRUE for a open view.
  Parameters:
  document: the document to which the view belongs.
  view: the view.
  Return value:
  TRUE or FALSE.
  ----------------------------------------------------------------------*/
ThotBool TtaIsViewOpen (Document document, View view)
{
  DocViewDescr        dView;
  ThotBool            opened;

  UserErrorCode = 0;
  opened = FALSE;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (view < 1 || view > MAX_VIEW_DOC)
    TtaError (ERR_invalid_parameter);
  else
    {
      dView = LoadedDocument[document - 1]->DocView[view - 1];
      if (dView.DvSSchema != NULL || dView.DvPSchemaView != 0)
        opened = TRUE;
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
View TtaGetViewFromName (Document document, const char *viewName)
{
  View                view;
  PtrDocument         pDoc;
  PtrPSchema          pPS;
  DocViewDescr        dView;
  int                 aView;

  UserErrorCode = 0;
  view = 0;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is ok */
      pDoc = LoadedDocument[document - 1];
      /* search in the opened views of the main tree */
      for (aView = 1; aView <= MAX_VIEW_DOC && view == 0; aView++)
        {
          dView = pDoc->DocView[aView - 1];
          if (dView.DvSSchema != NULL && dView.DvPSchemaView != 0)
            {
              pPS = PresentationSchema (dView.DvSSchema, pDoc);
              if (pPS && !strcmp (viewName, pPS->PsView[dView.DvPSchemaView - 1]))
                view = aView;
            }
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
void TtaGiveActiveView (Document * document, View * view)
{
  PtrDocument         pDoc;
  int                 aView;

  UserErrorCode = 0;
  *document = 0;
  *view = 0;
  if (ActiveFrame != 0)
    {
      GetDocAndView (ActiveFrame, &pDoc, &aView);
      if (pDoc != NULL)
        {
          *document = IdentDocument (pDoc);
          *view = aView;
        }
    }
}

/*----------------------------------------------------------------------
  TtaGiveActiveFrame
  Returns the active frame.
  ----------------------------------------------------------------------*/
int TtaGiveActiveFrame ()
{
  return ActiveFrame;
}

/*----------------------------------------------------------------------
  DisplayHolophrasted reaffiche un element sous forme holphrastee 
  ----------------------------------------------------------------------*/
void DisplayHolophrasted (PtrElement pEl, Document document)
{
  PtrDocument         pDoc;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
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
void RedisplayReference (PtrElement element, Document document)
{
  PtrDocument         pDoc;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
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
void RedisplayLeaf (PtrElement element, Document document, int delta)
{
  PtrDocument         pDoc;
  int                 view, h;
  PtrAbstractBox      pAbb;
  PtrAbstractBox      pAbbox1;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
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
        while (pAbbox1->AbElement == element &&
               pAbbox1->AbPresentationBox &&
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
            pAbbox1->AbLang = element->ElLanguage;
            break;
          case LtPolyLine:
            pAbbox1->AbLeafType = LtPolyLine;
            pAbbox1->AbPolyLineBuffer = element->ElPolyLineBuffer;
            pAbbox1->AbPolyLineShape = element->ElPolyLineType;
            pAbbox1->AbVolume = element->ElNPoints;
            break;
          case LtPath:
            pAbbox1->AbLeafType = LtPath;
            pAbbox1->AbFirstPathSeg = element->ElFirstPathSeg;
            pAbbox1->AbVolume = element->ElVolume;
            break;
          case LtSymbol:
          case LtGraphics:
            pAbbox1->AbLeafType = element->ElLeafType;
            pAbbox1->AbShape = element->ElGraph;
            pAbbox1->AbGraphScript = 'G';
            if (element->ElLeafType == LtGraphics)
              {
                if (element->ElGraph == 'a' &&
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
                else if (element->ElGraph == 1 || element->ElGraph == 'C')
                  /* a rectangle with rounded corners */
                  {
                    pAbbox1->AbRx = 0;
                    pAbbox1->AbRy = 0;
                  }
              }
            break;
          default:
            break;
          }
        /* un pave correspondant existe dans la vue view */
        if (pAbbox1 != NULL)
          {
            /* on ne s'occupe pas de la hauteur de page */
            h = 0;
            ChangeConcreteImage (pDoc->DocViewFrame[view], &h, pAbbox1);
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
void RedisplaySplittedText (PtrElement element, int position,
                            PtrElement pNewEl, PtrElement  pNextEl,
                            Document document)
{
  PtrElement          pEl;
  PtrDocument         pDoc;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
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
void RedisplayMergedText (PtrElement element, Document document)
{
  PtrElement          pEl;
  PtrDocument         pDoc;
  int                 view, dvol, h;
  PtrAbstractBox      pAb;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
    return;
  pEl = element;
  /* si le document est en mode de non calcul de l'image, on ne fait rien */
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  /* teste si pEl est le dernier fils de son pere, */
  /* abstraction faite des marques de page et autres elements a ignorer */
  if (SiblingElement (pEl, FALSE) == NULL)
    /* pEl devient le dernier fils de son pere */
    ChangeFirstLast (pEl, pDoc, FALSE, FALSE);

  /* met a jour le volume des paves correspondants */
  for (view = 0; view < MAX_VIEW_DOC; view++)
    {
      pAb = pEl->ElAbstractBox[view];
      if (pAb != NULL)
        {
          dvol = pEl->ElTextLength - pAb->AbVolume;
          pAb->AbVolume += dvol;
          pAb->AbChange = TRUE;
          pDoc->DocViewModifiedAb[view] = Enclosing (pAb,
                                                     pDoc->DocViewModifiedAb[view]);
          h = 0;		/* on ne s'occupe pas de la hauteur de page */
          ChangeConcreteImage (pDoc->DocViewFrame[view], &h, pAb->AbEnclosing);
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
  UndisplayInheritedAttributes supprime sur l'element pEl et son
  sous-arbre la presentation liee a l'attribut decrit par pAttr
  ou a l'heritage d'un tel attribut.    
  ----------------------------------------------------------------------*/
void UndisplayInheritedAttributes (PtrElement pEl, PtrAttribute pAttr,
                                   Document document, ThotBool suppression)
{
  ThotBool            inheritance, comparison;
  PtrHandlePSchema    pHd;   
  PtrAttribute        pAttrAsc;
  PtrAttribute        pOldAttr;
  PtrElement          pElChild, pElAttr;
  PtrDocument         pDoc;
  PtrPSchema          pPS;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
    return;
  /* si le document est en mode de non calcul de l'image, on ne fait rien */
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  if (suppression)
    pOldAttr = pAttr;
  else
    /* l'element porte-t-il deja un attribut du meme type ? */
    pOldAttr = AttributeValue (pEl, pAttr);
  pPS = PresentationSchema (pAttr->AeAttrSSchema, pDoc); 
  /* si l'attribut n'a pas de schema de presentation pour ce document,
     on ne fait rien */
  if (pPS == NULL)
    return;
  /* doit-on se preoccuper des heritages et comparaisons d'attributs? */
  inheritance = FALSE;
  comparison = FALSE;
  pHd = NULL;
  while (pPS)
    {
      inheritance = inheritance ||
        (pPS->PsNHeirElems->Num[pAttr->AeAttrNum - 1] > 0);
      comparison = comparison ||
        (pPS->PsNComparAttrs->Num[pAttr->AeAttrNum - 1] > 0);
      /* next P schema */
      if (pHd)
        /* get the next extension schema */
        pHd = pHd->HdNextPSchema;
      else if (CanApplyCSSToElement (pEl))
        /* extension schemas have not been checked yet */
        /* get the first extension schema */
        pHd = FirstPSchemaExtension (pAttr->AeAttrSSchema, pDoc, pEl);

      if (pHd == NULL)
        /* no more extension schemas. Stop */
        pPS = NULL;
      else
        pPS = pHd->HdPSchema;
    }

  if (inheritance || comparison)
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
      if (!pEl->ElTerminal && comparison)
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
      if (comparison)
        RemoveComparAttrPresent (pEl, LoadedDocument[document-1], pAttrAsc);
    }
}

/*----------------------------------------------------------------------
  UndisplayElement "desaffiche" un element qui va etre retire'    
  de son arbre abstrait.                                          
  ----------------------------------------------------------------------*/
void UndisplayElement (PtrElement pEl, Document document)
{
  PtrElement          pNext, pPrevious, pFather, pE, pSS, pSib;
  ThotBool            stop;
  PtrDocument         pDoc;
  int                 savePageHeight;
  ThotBool            immediat, first;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* si le document n'a pas de schema de presentation, */
  /* on retire simplement l'element de l'arbre abstrait */
  if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
    {
      RemoveElement (pEl);
      return;
    }
  /* si le document est en mode de non calcul de l'image, */
  /* on retire simplement l'element de l'arbre abstrait */
  /* sauf si c'est la racine car son pave n'avait pas ete detruit */
  /* il faut donc executer la suite de la procedure */
  immediat = documentDisplayMode[document - 1] == DisplayImmediately;
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    {
      if (pEl->ElParent == NULL)
        {
          DestroyAbsBoxes (pEl, pDoc, TRUE);
          RemoveElement (pEl);
          if (pEl != pDoc->DocDocElement)
            /* Ce n'est pas la racine du document ! */
            return;
        }
      else
        {
          RemoveElement (pEl);
          return;
        }
    }
  pPrevious = pEl->ElPrevious;
  /* saute les marques de page */
  stop = FALSE;
  do
    if (pPrevious == NULL)
      stop = TRUE;
    else if (!pPrevious->ElTerminal ||
             pPrevious->ElLeafType != LtPageColBreak)
      stop = TRUE;
    else
      pPrevious = pPrevious->ElPrevious;
  while (!(stop));
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

  pSib = NULL;
  first = FALSE;
  if (!ElemDoesNotCount (pEl, TRUE))
    /* the element we are removing is a significant element when it comes to
       decide whether an element is the first (resp. last) child of its parent */
    {
      if (!SiblingElement (pEl, FALSE))
        /* we are removing a last child. Get the previous sibling: it will
           become the last child */
        {
          first = FALSE;
          pSib = SiblingElement (pEl, TRUE);
        }
      else if (!SiblingElement (pEl, TRUE))
        /* we are removing a first child. Get the next sibling: it will
           become the first child */
        {
          first = TRUE;
          pSib = SiblingElement (pEl, FALSE);
        }
    }

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
          UpdateNumbers (pNext, pE, pDoc, immediat);
          pE = pE->ElNext;
        }
      PageHeight = savePageHeight;
    }

  if (pSib)
    /* element pSib becomes first or last */
    ChangeFirstLast (pSib, pDoc, first, FALSE);

  /* reevalue l'image de toutes les vues */
  AbstractImageUpdated (pDoc);
  RedisplayCommand (document);
  if (pEl != NULL)
    {
      /* reaffiche les paves qui copient les elements detruits */
      RedisplayCopies (pEl, pDoc, immediat);
      /* la renumerotation est faite plus haut */
      /* reaffiche les references aux elements detruits */
      /* et enregistre les references sortantes coupees */
      /* ainsi que les elements coupe's qui sont reference's par */
      /* d'autres documents */
      RedisplayEmptyReferences (pEl, &pDoc, immediat);
      /* Retransmet les valeurs des compteurs et attributs TRANSMIT */
      /* s'il y a des elements apres */
      if (pEl->ElStructSchema)
        if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrRefImportedDoc)
          RepApplyTransmitRules (pEl, pSS, pDoc);
    }
}

/*----------------------------------------------------------------------
  RedispNewGeometry                                               
  ----------------------------------------------------------------------*/
void RedispNewGeometry (Document document, PtrElement pEl)
{
  PtrDocument         pDoc;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
    return;
  /* si le document est en mode de non calcul de l'image, on ne fait rien */
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  AbstractImageUpdated (pDoc);
  RedisplayCommand (document);
  /* la nouvelle regle de presentation doit etre prise en compte dans */
  /* les copies-inclusions de l'element */
  RedisplayCopies (pEl, pDoc, (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
}
