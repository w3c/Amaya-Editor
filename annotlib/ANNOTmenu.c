/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * ANNOTquery.c : module for drawing a query dialog form.
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 * Acknowledgments: inspired from code written by Christophe Marjoline 
 *                  for the byzance collaborative work application
 */

/**
   w3c_algaeQuery=(ask '((?p ?s ?o)) :collect '(?p ?s ?o))
**/

/* annotlib includes */
#include "annotlib.h"
#include "ANNOTmenu.h"
#include "ANNOTschemas_f.h"
#include "ANNOTevent_f.h"
#include "ANNOTtools_f.h"
#include "init_f.h"

/* amaya includes */
#include "MENUconf.h"
#include "HTMLhistory_f.h"

/* windows includes */
#ifdef _WINGUI
#include "resource.h"
#include "wininclude.h"

#include "constmedia.h"
#include "appdialogue.h"

extern HINSTANCE hInstance;
#endif /* _WINGUI */

/* schema includes */
#include "XLink.h"

/* common local variables */
static char  s[MAX_LENGTH]; /* general purpose buffer */

/************************************************************
 ** Local custom query variables
 ************************************************************/
#ifdef _GTK
static int     CustomQueryBase;
static ThotBool CustomQueryFlag;
static char  *AlgaeText;
#endif /* _GTK */

/************************************************************
 ** Local annotation filter variables
 ************************************************************/

static int      AnnotFilterBase;
/* copies of the doc and view from which the menu was invoked */
static Document AnnotFilterDoc;
static View     AnnotFilterView;
static char   AnnotSelItem[MAX_LENGTH];
static int      AnnotSelIndex;
static SelType  AnnotSelType;

static List     *typesList;

#ifdef _WINGUI
static HWND       FilterHwnd = NULL;
#endif /* WINDOWS */

typedef struct _typeSelector
{
  char *name;
  RDFResourceP type;
} TypeSelector;


/**************************************************
 ** 
 ** AnnotFilter menu
 **
 *************************************************/

/*---------------------------------------------------------------
  WIN_AnnotFilterNewSelector
  Does the equivalent of the Thotlib selector for the Annotation
  Filter.
  ------------------------------------------------------------------*/
#ifdef _WINGUI
static void WIN_AnnotFilterNewSelector (Document doc, char *entries,
                                        int nb_entries)
{
  int index = 0;
  int i = 0;

  /* erase the text of the existing window */
  SendDlgItemMessage (FilterHwnd, IDC_FILTERSEL, LB_RESETCONTENT, 0, 0);

  while (i < nb_entries && entries[index] != '\0')
    {
      SendDlgItemMessage (FilterHwnd, IDC_FILTERSEL, LB_INSERTSTRING,
                          i, (LPARAM) &entries[index]); 
      /* @@ JK: what does this mean? Longueur de l'intitule ?? */
      index += strlen (&entries[index]) + 1;
      i++;
    }
}
#endif /* _WINGUI */

/*---------------------------------------------------------------
  BuildAnnotFilterSelector builds the list allowing to select a profile
  ------------------------------------------------------------------*/
static void BuildAnnotFilterSelector (Document doc, SelType selector)
{
  int                   nb_entries;
  int                   i;
  List                  *list_item;
  AnnotFilterData       *filter;
  char                *name;
  char                 status_char;
  int                    status;
	   

  /* count and copy the entries that we're interested in */
  switch (selector)
    {
    case BY_AUTHOR:
      list_item = AnnotMetaData[doc].authors;
      break;
    case BY_TYPE:
      list_item = AnnotMetaData[doc].types;
      break;
    case BY_SERVER:
      list_item = AnnotMetaData[doc].servers;
      break;
    default:
      list_item = NULL;
      break;
    }

  nb_entries = 0;
  strcpy (s, "");
  i = 0;
  while (list_item)
    {
      filter = (AnnotFilterData *) list_item->object;
      if (filter)
        {
          /* do we need to show it? */
          /* we have a type (or name)... now search all the
             annotations for this type and see if it must be shown */
          status = AnnotFilter_status (doc, selector, filter->object);
	   
          switch (status)
            {
            case 2: /* partial */
              status_char = '-';
              break;

            default:
            case 1: /* show */
              status_char = ' ';
              break;

            case 0: /* hide */
              status_char = '*';
              break;
            }
          if (selector == BY_TYPE)
            name = ANNOT_GetLabel (&annot_schema_list,
                                   (RDFResourceP) filter->object);
          else
            name = (char *) filter->object;
          sprintf (&s[i], "%c%s", status_char, name);
          i += strlen (&s[i]) + 1;
          nb_entries++;
        }
      list_item = list_item->next;
    }

#ifdef _WINGUI
  WIN_AnnotFilterNewSelector (doc, s, nb_entries);
#endif /* _WINGUI */
#ifdef _WX
  /* function not implemented yet */
  InitInfo ("", TtaGetMessage(LIB, TMSG_NOT_AVAILABLE));
  TtaNewScrollPopup (AnnotFilterBase + AnnotFilterMenu, TtaGetViewFrame (doc, 1),
                     NULL, nb_entries, s, NULL, FALSE, 'L');
#endif /* _WX */
}

/*---------------------------------------------------------------
  ChangeAnnotVisibility
  ------------------------------------------------------------------*/
static void ChangeAnnotVisibility (Document doc, SelType selector,
                                   char *object, ThotBool show)
{
  List               *list_item;
  AnnotFilterData    *filter;
  char             *annot_url;
  char             *name;
  AnnotMeta          *annot;
  int                 length;
  ThotBool            annot_show = TRUE;
  SSchema             XLinkSchema;

  ElementType         elType;
  Element             el;
  Attribute           attr;
  AttributeType       attrType;

  DisplayMode         dispMode;

  int		      position;
  int		      distance;

  if (AnnotSelItem[0] == EOS)
    return;

  XLinkSchema = TtaGetSSchema ("XLink", doc);
  if (!XLinkSchema)
    /* there are no xlinks in this document */
    return;

  /* change the filter metadata first */
  switch (selector)
    {
    case BY_AUTHOR:
      list_item = AnnotMetaData[doc].authors;
      break;
    case BY_TYPE:
      list_item = AnnotMetaData[doc].types;
      break;
    case BY_SERVER:
      list_item = AnnotMetaData[doc].servers;
      break;
    default: 
      list_item = NULL;
      break;
    }
  
  if (!list_item)
    return;

  while (list_item)
    {
      filter = (AnnotFilterData *) list_item->object;

      if (selector == BY_TYPE)
        name = ANNOT_GetLabel (&annot_schema_list,
                               (RDFResourceP) filter->object);
      else
        name = (char *) filter->object;

      if (filter && !strcasecmp (name, object + 1))
        {
          filter->show = show;
          break;
        }
      list_item = list_item->next;
    }

  /* 
  ** show/hide it on the document 
  */

  /* get the current position in the document */
  position = RelativePosition (doc, &distance);

  /* avoid refreshing the document while we're constructing it */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, NoComputedDisplay);

  /* initialize */
  el = TtaGetRootElement (doc);
  elType.ElSSchema = XLinkSchema;
  elType.ElTypeNum = XLink_EL_XLink;
  attrType.AttrSSchema = XLinkSchema;

  /* search and change */
  while ((el = TtaSearchTypedElement (elType, SearchForward, el)))
    {
      /* get the HREF (we will use it to search in the filters */
      attrType.AttrTypeNum = XLink_ATTR_href_;
      attr = TtaGetAttribute (el, attrType);
      if (!attr)
        /* this looks like an error! */
        continue;
      length = TtaGetTextAttributeLength (attr) + 1;
      annot_url = (char *)TtaGetMemory (length);
      TtaGiveTextAttributeValue (attr, annot_url, &length);
      
      /* now look in the filters to see if we need to hide it or not */
      annot = AnnotList_searchAnnot (AnnotMetaData[doc].annotations, 
                                     annot_url, AM_BODY_URL);
      TtaFreeMemory (annot_url);
      if (!annot || !(annot->is_visible) || annot->is_orphan)
        continue;

#ifdef ANNOT_ON_ANNOT
      /* skip the reply annotations */
      if (annot->inReplyTo)
        continue;
#endif /* ANNOT_ON_ANNOT */
      
      switch (selector) 
        {
        case BY_AUTHOR:
          if (annot->author)
            annot_show = AnnotFilter_showAuthor (AnnotMetaData[doc].authors, 
                                                 annot->author, 
                                                 annot->annot_url);
          break;
        case BY_TYPE:
          annot_show = AnnotFilter_show (AnnotMetaData[doc].types, 
                                         (void *) annot->type);
          break;
        case BY_SERVER:
          annot_show = AnnotFilter_showServer (AnnotMetaData[doc].servers, 
                                               annot->annot_url);
          break;
        }

      attrType.AttrTypeNum = XLink_ATTR_AnnotIsHidden;
      attr = TtaGetAttribute (el, attrType);
      if (annot_show)
        {
          /* erase the attribute */
          if (attr)
            {
              TtaRemoveAttribute (el, attr, doc);  
              annot->show = TRUE;
            }
        }
      else
        {
          /* add the attribute if it doesn't exist */
          if (!attr)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, doc);  
              annot->show = FALSE;
            }
        }
    }

  /* display the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);

  /* show the document at the same position as before */
  TtaResetCursor (0, 0);
  el = ElementAtPosition (doc, position);
  TtaShowElement (doc, 1, el, distance);

  /* finally, redraw the selector */
  BuildAnnotFilterSelector (doc, selector);
  /* update the selector text */
  if (show)
    AnnotSelItem[0] = ' ';
  else
    AnnotSelItem[0] = ' ';
}

/*----------------------------------------------------------------------
  DocAnnotVisibility
  Shows or hides the annotations in a given document according to the
  value of the show variable.
  ----------------------------------------------------------------------*/
static void DocAnnotVisibility (Document document, View view, ThotBool show)
{
  ElementType         elType;
  Element             el;
  Attribute           attr;
  AttributeType       attrType;
  SSchema             XLinkSchema;
  DisplayMode         dispMode;
  List                *list_item;
  int		      position;
  int		      distance;

  XLinkSchema = TtaGetSSchema ("XLink", document);
  if (!XLinkSchema)
    /* there are no xlinks in this document */
    return;

  /* change the filter's visibility status */
  AnnotFilter_toggleAll (document, BY_AUTHOR, show);
  AnnotFilter_toggleAll (document, BY_TYPE, show);
  AnnotFilter_toggleAll (document, BY_SERVER, show);

  /*
   * Change the annotations metadata for the document
   */

  list_item = AnnotMetaData[document].annotations;
  for (; list_item; list_item = list_item->next)
    {
      AnnotMeta *annot = (AnnotMeta *)list_item->object;
      if (annot && annot->is_visible && !(annot->is_orphan))
        annot->show = show;
    }

  /*
   * Do the visible change on the document
   */

  /* get the current position in the document */
  position = RelativePosition (document, &distance);
  
  /* avoid refreshing the document while we're constructing it */
  dispMode = TtaGetDisplayMode (document);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (document, NoComputedDisplay);

  /* initialize */
  el = TtaGetRootElement (document);
  elType.ElTypeNum = XLink_EL_XLink;
  elType.ElSSchema = XLinkSchema;
  attrType.AttrSSchema = XLinkSchema;
  attrType.AttrTypeNum = XLink_ATTR_AnnotIsHidden;

  /* search and change */
  while ((el = TtaSearchTypedElement (elType, SearchForward, el)))
    {
      attr = TtaGetAttribute (el, attrType);
      if (show)
        {
          /* erase the attribute */
          if (attr)
            TtaRemoveAttribute (el, attr, document);  
        }
      else
        {
          /* add the attribute if it doesn't exist */
          if (!attr)
            {
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (el, attr, document);  
            }
        }
    }

  /* display the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (document, dispMode);

  /* show the document at the same position as before */
  TtaResetCursor (0, 0);
  el = ElementAtPosition (document, position);
  TtaShowElement (document, 1, el, distance);

  /* redisplay the current selector */
  BuildAnnotFilterSelector (document, AnnotSelType);
  /* and clear the selector text */
  AnnotSelItem[0] = EOS;
}

#ifdef _WINGUI
/*-----------------------------------------------------------------------
  AnnotFilterDlgProc
  ------------------------------------------------------------------------*/
LRESULT CALLBACK WIN_AnnotFilterDlgProc (ThotWindow hwnDlg, UINT msg,
                                         WPARAM wParam, LPARAM lParam)
{
  int  index = 0;
  int itemIndex;
  UINT  i = 0; 
    
  switch (msg)
    {
    case WM_INITDIALOG:
      FilterHwnd = hwnDlg;
      /* 
      ** write the dialogue text
      */
      /* window title */
      SetWindowText (hwnDlg, TtaGetMessage (AMAYA, AM_AFILTER));
      /* buttons */
      SetWindowText (GetDlgItem (hwnDlg, ID_ANNOTSHOW), 
                     TtaGetMessage (AMAYA, AM_AFILTER_SHOW));
      SetWindowText (GetDlgItem (hwnDlg, ID_ANNOTHIDE), 
                     TtaGetMessage (AMAYA, AM_AFILTER_HIDE));
      SetWindowText (GetDlgItem (hwnDlg, ID_ANNOTSHOWALL), 
                     TtaGetMessage (AMAYA, AM_AFILTER_SHOW_ALL));
      SetWindowText (GetDlgItem (hwnDlg, ID_ANNOTHIDEALL), 
                     TtaGetMessage (AMAYA, AM_AFILTER_HIDE_ALL));
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));
      /* filter options */
      SetWindowText (GetDlgItem (hwnDlg, IDC_TFILTEROPTIONS), 
                     TtaGetMessage (AMAYA, AM_AFILTER_OPTIONS));
      SetWindowText (GetDlgItem (hwnDlg, IDC_FILTERBYAUTHOR), 
                     TtaGetMessage (AMAYA, AM_AFILTER_BYAUTHOR));
      SetWindowText (GetDlgItem (hwnDlg, IDC_FILTERBYTYPE), 
                     TtaGetMessage (AMAYA, AM_AFILTER_BYTYPE));
      SetWindowText (GetDlgItem (hwnDlg, IDC_FILTERBYSERVER), 
                     TtaGetMessage (AMAYA, AM_AFILTER_BYSERVER));
      /* help label */
      SetWindowText (GetDlgItem (hwnDlg, IDC_TFILTERHELP), 
                     TtaGetMessage (AMAYA, AM_AFILTER_HELP));

      /* display the by author items */
      BuildAnnotFilterSelector (AnnotFilterDoc, AnnotSelType);
      /* select the by author radio button */
      i = CheckRadioButton (hwnDlg, IDC_FILTERBYAUTHOR, IDC_FILTERBYAUTHOR, IDC_FILTERBYAUTHOR);
      break;
      
    case WM_CLOSE:
    case WM_DESTROY:
      FilterHwnd = NULL;
      /* the filter select window is destroyed automatically when we kill
         the parent window */
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:

      switch (LOWORD (wParam))
        {
          /* list box (filter select) */
        case IDC_FILTERSEL:
          if (HIWORD (wParam) == LBN_SELCHANGE)
            {
              /* get the index of the selected item */
              itemIndex = SendDlgItemMessage (FilterHwnd, IDC_FILTERSEL,
                                              LB_GETCURSEL, 0, 0);
              /* get the text of this item */
              SendDlgItemMessage (FilterHwnd, IDC_FILTERSEL,
                                  LB_GETTEXT, itemIndex, (LPARAM) AnnotSelItem);
              break;
            }
          break;
	
          /* radio buttons */
        case IDC_FILTERBYAUTHOR:
          if (AnnotSelType != 0)
            {
              AnnotSelType = 0;
              CheckRadioButton (hwnDlg, IDC_FILTERBYAUTHOR, IDC_FILTERBYSERVER, IDC_FILTERBYAUTHOR);
              BuildAnnotFilterSelector (AnnotFilterDoc, AnnotSelType);
            }
          break;
	
        case IDC_FILTERBYTYPE:
          if (AnnotSelType != 1)
            {
              AnnotSelType = 1;
              CheckRadioButton (hwnDlg, IDC_FILTERBYAUTHOR, IDC_FILTERBYSERVER, IDC_FILTERBYTYPE);
              BuildAnnotFilterSelector (AnnotFilterDoc, AnnotSelType);
            }
          break;
	
        case IDC_FILTERBYSERVER:
          if (AnnotSelType != 2)
            {
              AnnotSelType = 2;
              CheckRadioButton (hwnDlg, IDC_FILTERBYAUTHOR, IDC_FILTERBYSERVER, IDC_FILTERBYSERVER);
              BuildAnnotFilterSelector (AnnotFilterDoc, AnnotSelType);
            }
          break;
	
          /* action buttons */
        case ID_ANNOTSHOW:
          /* memorize the last selection */
          itemIndex = SendDlgItemMessage (FilterHwnd, IDC_FILTERSEL,
                                          LB_GETCURSEL, 0, 0);
          ChangeAnnotVisibility (AnnotFilterDoc, AnnotSelType, 
                                 AnnotSelItem, TRUE);
          /* select it again (as the selection gets deselected automatically */
          SendDlgItemMessage (FilterHwnd, IDC_FILTERSEL,
                              LB_SETCURSEL, itemIndex, 0);
          break;
	
        case ID_ANNOTHIDE:
          /* memorize the last selection */
          itemIndex = SendDlgItemMessage (FilterHwnd, IDC_FILTERSEL,
                                          LB_GETCURSEL, 0, 0);
          ChangeAnnotVisibility (AnnotFilterDoc, AnnotSelType, 
                                 AnnotSelItem, FALSE);
          /* select it again (as the selection gets deselected automatically */
          SendDlgItemMessage (FilterHwnd, IDC_FILTERSEL,
                              LB_SETCURSEL, itemIndex, 0);
          break;

        case ID_ANNOTSHOWALL:
          DocAnnotVisibility (AnnotFilterDoc, AnnotFilterView, TRUE);
          break;
	
        case ID_ANNOTHIDEALL:
          DocAnnotVisibility (AnnotFilterDoc, AnnotFilterView, FALSE);
          break;
	
        case ID_DONE:
          EndDialog (hwnDlg, ID_DONE);
          break;
        }
      break;	     
    default: return FALSE;
    }
  return TRUE;
}
#else /* _WINGUI */

/*----------------------------------------------------------------------
  callback of the AnnotFilter menu
  ----------------------------------------------------------------------*/
static void AnnotFilterCallbackDialog (int ref, int typedata, char * data)
{
  intptr_t val;

  if (ref == -1)
    {
      /* removes the AnnotFilter conf menu */
      TtaDestroyDialogue (AnnotFilterBase + AnnotFilterMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (intptr_t) data;
      switch (ref - AnnotFilterBase)
        {
        case AnnotFilterMenu:
          switch (val) 
            {
            case 0:
              TtaDestroyDialogue (ref);
              break;
            case 1:
              ChangeAnnotVisibility (AnnotFilterDoc, AnnotSelType, 
                                     AnnotSelItem, TRUE);
              /* maybe refresh the dialogue */
              break;
            case 2:
              ChangeAnnotVisibility (AnnotFilterDoc, AnnotSelType, 
                                     AnnotSelItem, FALSE);
              /* maybe refresh the dialogue */
              break;
            case 3:
              DocAnnotVisibility (AnnotFilterDoc, AnnotFilterView, TRUE);
              /* maybe refresh the dialogue and reset the selection */
              break;
            case 4:
              DocAnnotVisibility (AnnotFilterDoc, AnnotFilterView, FALSE);
              /* maybe refresh the dialogue and reset the selection */
              break;
            default:
              break;
            }
          break;

        case mFilterSelector:
          /* copy what was selected */
          if (data)
            strcpy (AnnotSelItem, data);
          else
            AnnotSelItem[0] = EOS;
          break;
	  
        case mSelectFilter:
          /* change the content of the selector */
          /* @@ here I need to have a pointer in memory to the
             annotation document... means I'll only be able to have
             one such annotation dialogue at the time */
          if (AnnotSelType != (SelType) val)
            {
              AnnotSelType = (SelType) val;
              AnnotSelItem[0] = EOS;
              BuildAnnotFilterSelector (AnnotFilterDoc, (SelType)val);
            }
          break;

        default:
          break;
        }
    }
}
#endif /* !_WINGUI */

/*----------------------------------------------------------------------
  AnnotFilter
  Build and display the AnnotFilter Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
void AnnotFilter (Document document, View view)
{
  /* build the filter structures from the downloaded 
     annotation info */
  AnnotFilter_build (document);

#ifndef _WINGUI
  /* initialize the base if it hasn't yet been done */
  if (AnnotFilterBase == 0)
    AnnotFilterBase =  TtaSetCallback ((Proc)AnnotFilterCallbackDialog,
                                       MAX_ANNOTFILTER_DLG);
#endif /* !_WINGUI */

  /* make a copy of the current document and view, so that we can
     find this info in the callback */
  AnnotFilterDoc = document;
  AnnotFilterView = view;
  AnnotSelItem[0] = EOS;
  AnnotSelIndex = -1;
  AnnotSelType = (SelType)0;

  /* Create the dialogue form */
#ifndef _WINGUI

  /* display the selectors */
  BuildAnnotFilterSelector (document, BY_AUTHOR);

  /* choose the BY_AUTHOR radio button */
#endif /* !_WINGUI */

  /* display the menu */
#ifndef _WINGUI
  TtaSetDialoguePosition ();
  TtaShowDialogue (AnnotFilterBase + AnnotFilterMenu, TRUE, TRUE);
#else /* !_WINGUI */
  if (!FilterHwnd)
    /* only activate the menu if it isn't active already */
    DialogBox (hInstance, MAKEINTRESOURCE (ANNOTFILTERMENU), NULL, (DLGPROC) WIN_AnnotFilterDlgProc);
  else
    SetFocus (FilterHwnd);
#endif /* !_WINGUI */
}

/***************************************************
 **  AnnotTypes menu
 ***************************************************/

/*---------------------------------------------------------------
  BuildAnnotTypesSelector
  builds the list showing the different annotation types.
  Returns the number of entries in the menu.
  ------------------------------------------------------------------*/
static int BuildAnnotTypesSelector (Document doc)
{
  int                   nb_entries;
  int                   i;
  RDFClassP		annotClass;

  nb_entries = 0;
  if (typesList)
    List_delAll (&typesList, List_delCharObj);
  strcpy (s, "");
  i = 0;

#ifdef ANNOT_ON_ANNOT
  if (Annot_IsReplyTo (doc))
    annotClass = THREAD_REPLY_CLASS;
  else
#endif /* ANNOT_ON_ANNOT */
    annotClass = ANNOTATION_CLASS;

  if (annotClass && annotClass->class_)
    {
      List *item;

      for (item=annotClass->class_->subClasses; item; item=item->next)
        {
          RDFClassP subType = (RDFClassP) item->object;
          TypeSelector *t = (TypeSelector *) TtaGetMemory (sizeof(TypeSelector));

          t->type = subType;
          t->name = ANNOT_GetLabel(&annot_schema_list, subType);
          List_add (&typesList, (void *) t);

          sprintf (&s[i], "B%s", t->name);
          i += strlen (&s[i]);
          s[i] = EOS;
          i++;
          nb_entries++;
        }
    }
  else
    if (annotClass)
      {
        TypeSelector *t = (TypeSelector*)TtaGetMemory (sizeof(TypeSelector));

        t->type = annotClass;
        t->name = ANNOT_GetLabel(&annot_schema_list, annotClass);
        List_add (&typesList, (void *) t);

        strcpy (s, t->name);
        i = strlen (s);
        s[i] = EOS;
        nb_entries = 1;
      }

  /* create the main menu */
#ifdef _WX
  TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1),
                     NULL, nb_entries, s, NULL, FALSE, 'L');
#else /* _WX */
  TtaNewPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1),
               NULL, nb_entries, s, NULL, 'L');
#endif /* _WX */
  
  return nb_entries;
}

/*----------------------------------------------------------------------
  AnnotTypes
  Returns the RDF Resource pointer that represents the type selection
  of the user. It is NULL if the user doesn't select a type.
  ----------------------------------------------------------------------*/
RDFResourceP AnnotTypes (Document document, View view)
{
  RDFResourceP result = NULL;
  int nb_entries;

  /* prepare the selector */
  nb_entries = BuildAnnotTypesSelector (document);
  
  /* activate the menu that has just been created */
  ReturnOption = -1;
  /* JK: 04/06/2004: this used to work when initialized to -1 */
  ReturnOptionMenu = 0;
#ifndef _WINGUI
  TtaSetDialoguePosition ();
#endif /* !_WINGUI */
  TtaShowDialogue (BaseDialog + OptionMenu, FALSE, TRUE);
  /* wait for an answer from the user */
  TtaWaitShowDialogue ();
  TtaDestroyDialogue (BaseDialog + OptionMenu);
  if (ReturnOption >= 0 && ReturnOptionMenu >= 0) 
    {
      /* make the returned option selected */
      if (ReturnOptionMenu == 0)
        { /* an item in the main (SELECT) menu */
          List *item;
          int entry;

          /* typesList is sorted in opposite order of the dialogue */
          entry = nb_entries - ReturnOption - 1;
          for (item = typesList; item && (entry-- > 0); item=item->next);
          if (item->object)
            result = ((TypeSelector *) item->object)->type;
        }
    }
  List_delAll (&typesList, List_delCharObj);
  return result;
}










