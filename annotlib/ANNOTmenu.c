/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000.
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

#define THOT_EXPORT extern
#include "HTML.h"
#include "annotlib.h"
#include "ANNOTmenu.h"

#ifdef _WINDOWS
#include "resource.h"
#include "wininclude.h"

#include "constmedia.h"
#include "appdialogue.h"

extern HINSTANCE hInstance;
#endif /* _WINDOWS */

/* common local variables */
static CHAR_T  s[MAX_LENGTH]; /* general purpose buffer */

/************************************************************
 ** Local custom query variables
 ************************************************************/

static int     CustomQueryBase;
static ThotBool CustomQueryFlag;
static CHAR_T  *AlgaeText;

/************************************************************
 ** Local annotation filter variables
 ************************************************************/

static int      AnnotFilterBase;
/* copies of the doc and view from which the menu was invoked */
static Document AnnotFilterDoc;
static View     AnnotFilterView;
static CHAR_T   AnnotSelItem[MAX_LENGTH];
static int      AnnotSelIndex;
static SelType  AnnotSelType;

static int      AnnotTypesBase;
static CHAR_T   AnnotTypesSelItem[MAX_LENGTH];
static List     *typesList;

#ifdef _WINDOWS
static HWND       FilterHwnd = NULL;
static ThotWindow FilterSelHwnd = NULL;
#endif /* WINDOWS */

typedef struct _typeSelector
{
  CHAR_T *name;
  RDFResourceP type;
} TypeSelector;

#ifndef _WINDOWS
/*----------------------------------------------------------------------
  CustomQueryCallbackDialog
  callback of the annot custom query menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CustomQueryCallbackDialog (int ref, int typedata, CHAR_T *data)
#else
static void         CustomQueryCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
CHAR_T              *data;

#endif /* __STDC__ */
{
  int                 val;

  if (ref == -1)
    {
      /* removes the custom query menu */
      TtaDestroyDialogue (CustomQueryBase + CustomQueryMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - CustomQueryBase)
	{
	case CustomQueryMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      /* apply */
	      SetAnnotCustomQuery (CustomQueryFlag);
	      SetAnnotAlgaeText (AlgaeText);
	      TtaFreeMemory (AlgaeText);
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      /* get default */
	      CustomQueryFlag = FALSE;
	      TtaFreeMemory (AlgaeText);
	      AlgaeText = NULL;
	      TtaSetTextForm (CustomQueryBase + mFreeText, TEXT(""));
	      TtaSetMenuForm (CustomQueryBase + mExpertMode, 0);
	      break;

	    default:
	      break;
	    }
	  break;

	case mUser1 :
	  break;
	
	case mUser2 :
	  break;

	case mBtime :
	  break;

	case mEtime :
	  break;

	case mExpertMode :
	  if (val == 0)
	    CustomQueryFlag = FALSE;
	  else
	    CustomQueryFlag = TRUE;
	  break;

	case mFreeText :
	  if (data)
	    AlgaeText = TtaStrdup (data);
	  else
	    {
	      TtaFreeMemory (AlgaeText);
	      AlgaeText = NULL;
	    }
	  break;
	  
	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  CustomQueryMenuInit
  Build and display the Query Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         CustomQueryMenuInit (Document document, View view)
#else
void         CustomQueryMenuInit (document, view)
Document     document;
View         view;
#endif /* __STDC__ */
{
#ifndef _WINDOWS
   int              i;
   CHAR_T          *ptr;

   /* initialize the base if it hasn't yet been done */
   if (!CustomQueryBase)
     CustomQueryBase = TtaSetCallback (CustomQueryCallbackDialog, 
				       MAX_QUERYCONFMENU_DLG);

   /* Create the dialogue form */
   i = 0;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_APPLY_BUTTON));
   i += ustrlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON));

   TtaNewSheet (CustomQueryBase + CustomQueryMenu,
		TtaGetViewFrame (document, view),
		TEXT("Query Customization Menu"),
		2, s, FALSE, 10, 'L', D_DONE);

   /* @@ JK: removed the following menus as they're not ready for the
      demo, and added a temporary checkbox */
#if 0
   TtaNewLabel (CustomQueryBase + mUsersGroups, 
		CustomQueryBase + CustomQueryMenu,
	       TEXT("Users and groups"));

   TtaNewTextForm (CustomQueryBase + mUser1,
		   CustomQueryBase + CustomQueryMenu,
		   TEXT("user1"),
		   20,
		   1,
		   TRUE);

   TtaNewTextForm (CustomQueryBase + mUser2,
		   CustomQueryBase + CustomQueryMenu,
		   TEXT("user2"),
		   20,
		   1,
		   TRUE);

   TtaNewLabel (CustomQueryBase + mTime, 
		CustomQueryBase + CustomQueryMenu,
		TEXT("Time"));

   TtaNewTextForm (CustomQueryBase + mBtime,
		   CustomQueryBase + CustomQueryMenu,
		   TEXT("Begin"),
		   20,
		   1,
		   TRUE);

   TtaNewTextForm (CustomQueryBase + mEtime,
		   CustomQueryBase + CustomQueryMenu,
		   TEXT("End"),
		   20,
		   1,

   TtaNewLabel (CustomQueryBase + mExpertMode,
		CustomQueryBase + CustomQueryMenu,
		TEXT("Expert mode"));
		   TRUE);
#else
  /* create the radio buttons for choosing a selector */
  i = 0;
  strcpy (&s[i], TEXT("BUse standard query"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("BUse free algae query with the following text"));

  TtaNewSubmenu (CustomQueryBase + mExpertMode,
		 CustomQueryBase + CustomQueryMenu,
		 0,
		 TEXT("Query type"),
		 2,
		 s,
		 NULL,
		 TRUE);
#endif

   TtaNewTextForm (CustomQueryBase + mFreeText,
		   CustomQueryBase + CustomQueryMenu,
		   TEXT("(%u stands for the URL of the document that's being browsed)"),
		   70,
		   5,
		   TRUE);

   /* initialize the menu */
   ptr = GetAnnotAlgaeText ();
   if (ptr)
     {
       AlgaeText = TtaStrdup (ptr);
       TtaSetTextForm (CustomQueryBase + mFreeText, AlgaeText);
     }
   else
     AlgaeText = NULL;
   
   CustomQueryFlag = GetAnnotCustomQuery ();
   TtaSetMenuForm (CustomQueryBase + mExpertMode, (CustomQueryFlag) ? 1 : 0);

   /* display the menu */
   TtaSetDialoguePosition ();
   TtaShowDialogue (CustomQueryBase + CustomQueryMenu, TRUE);

#endif /* !_WINDOWS */
}

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
#ifdef _WINDOWS
static void WIN_AnnotFilterNewSelector (Document doc, CHAR_T *entries, int nb_entries)
{
  int index = 0;
  int i = 0;

  if (!FilterSelHwnd)
    /* create a new window if it didn't exist */
    FilterSelHwnd = CreateWindow (TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
				 10, 40, 310, 200, FilterHwnd, (HMENU) 1, 
				(HINSTANCE) GetWindowLong (FilterHwnd, GWL_HINSTANCE), NULL);
  else
    /* erase the text of the existing window */
    SendMessage (FilterSelHwnd, LB_RESETCONTENT, 0, 0);

  while (i < nb_entries && entries[index] != '\0')
    {
      SendMessage (FilterSelHwnd, LB_INSERTSTRING, i, (LPARAM) &entries[index]); 
      /* @@ Longueur de l'intitule ?? */
      index += ustrlen (&entries[index]) + 1;
      i++;
    }
}
#endif /* _WINDOWS */

/*---------------------------------------------------------------
  BuildAnnotFilterSelector builds the list allowing to select a profile
------------------------------------------------------------------*/
#ifdef __STDC__
static void BuildAnnotFilterSelector (Document doc, SelType selector)
#else
static void BuildAnnotFilterSelector (doc)
Document doc;
#endif /* __STDC__ */
{
  int                   nb_entries;
  int                   i;
  List                  *list_item;
  AnnotFilterData       *filter;

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
    }
  nb_entries = 0;
  ustrcpy (s, TEXT(""));
  i = 0;
  while (list_item)
     {
       filter = (AnnotFilterData *) list_item->object;
       if (filter)
	 {
	   CHAR_T *name;
	   if (selector == BY_TYPE)
	     name = ANNOT_GetLabel (&annot_schema_list,
				    (RDFResourceP)filter->object);
	   else
	     name = (CHAR_T*)filter->object;
	   usprintf (&s[i], TEXT("%c%s"), 
		     (filter->show) ? TEXT(' ') : TEXT('*'),
		     name);
	   i += ustrlen (&s[i]) + 1;
	   nb_entries++;
	 }
       list_item = list_item->next;
     }

#ifdef _WINDOWS
  WIN_AnnotFilterNewSelector (doc, s, nb_entries);
#else /* _WINDOWS */
  /* Fill in the form  */
  TtaNewSelector (AnnotFilterBase + mFilterSelector, 
		  AnnotFilterBase + AnnotFilterMenu,
		  NULL,
		  nb_entries,
		  s,
		  5,
		  NULL,
		  TRUE,
		  TRUE);
#endif /* _WINDOWS */
}

/*---------------------------------------------------------------
  ChangeAnnotVisibility
------------------------------------------------------------------*/
#ifdef __STDC__
static void ChangeAnnotVisibility (Document doc, SelType selector, CHAR_T *object, ThotBool show)
#else
static void ChangeAnnotVisibility (doc, selector, object, show)
Document doc;
SelType selector;
CHAR_T *object;
ThotBool show;

#endif /* __STDC__ */
{
  List               *list_item;
  AnnotFilterData    *filter;
  CHAR_T             *annot_url;
  AnnotMeta          *annot;
  int                 length;
  ThotBool            annot_show;
  SSchema             XLinkSchema;

  ElementType         elType;
  Element             el;
  Attribute           attr;
  AttributeType       attrType;

  if (AnnotSelItem[0] == WC_EOS)
    return;

  XLinkSchema = TtaGetSSchema (TEXT("XLink"), doc);
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
    }
  
  while (list_item)
    {
      filter = (AnnotFilterData *) list_item->object;
      if (filter && !ustrcasecmp (filter->object, object + 1))
	{
	  filter->show = show;
	  break;
	}
      list_item = list_item->next;
    }

  /* then redraw the selector */
  BuildAnnotFilterSelector (doc, selector);
  /* update the selector text */
  if (show)
    AnnotSelItem[0] = TEXT(' ');
  else
    AnnotSelItem[0] = TEXT('*');
#ifndef _WINDOWS
  TtaSetSelector (AnnotFilterBase + mFilterSelector, -1, AnnotSelItem);
#endif /* _WINDOWS */

  /* and show/hide it on the document :) */

  /* avoid refreshing the document while we're constructing it */
  TtaSetDisplayMode (doc, NoComputedDisplay);
  /* initialize */
  el = TtaGetMainRoot (doc);
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
      annot_url = TtaAllocString (length);
      TtaGiveTextAttributeValue (attr, annot_url, &length);
      
      /* now look in the filters to see if we need to hide it or not */
      annot = AnnotList_searchAnnot (AnnotMetaData[doc].annotations, 
				     annot_url, AM_BODY_URL);
      TtaFreeMemory (annot_url);
      if (!annot)
	continue;
      
      switch (selector) 
	{
	case BY_AUTHOR:
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
	    TtaRemoveAttribute (el, attr, doc);  
	}
      else
	{
	  /* add the attribute if it doesn't exist */
	  if (!attr)
	    {
	      attr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (el, attr, doc);  
	    }
	}
    }
  /* show the document */
  TtaSetDisplayMode (doc, DisplayImmediately);
}

/*---------------------------------------------------------------
  ChangeAllAnnotVisibility
------------------------------------------------------------------*/
#ifdef __STDC__
static void ChangeAllAnnotVisibility (Document doc, SelType selector, ThotBool show)
#else
static void ChangeAllAnnotVisibility (doc, selector, show)
Document doc;
SelType selector;
ThotBool show;

#endif /* __STDC__ */
{
  List *list_item;
  AnnotFilterData *filter;

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
    }
  
  while (list_item)
    {
      filter = (AnnotFilterData *) list_item->object;
      if (filter)
	filter->show = show;
      list_item = list_item->next;
    }
}

/*----------------------------------------------------------------------
  DocAnnotVisibility
  Shows or hides the annotations in a given document according to the
  value of the show variable.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void DocAnnotVisibility (Document document, View view, ThotBool show)
#else
static void DocAnnotVisibility (document, view, show)
Document document;
View view;
ThotBool show;
#endif /* __STDC__ */
{
  ElementType elType;
  Element     el;
  Attribute           attr;
  AttributeType       attrType;
  SSchema             XLinkSchema;

  XLinkSchema = TtaGetSSchema (TEXT("XLink"), document);
  if (!XLinkSchema)
    /* there are no xlinks in this document */
    return;

  /* change the selectors */
  ChangeAllAnnotVisibility (document, BY_AUTHOR, show);
  ChangeAllAnnotVisibility (document, BY_TYPE, show);
  ChangeAllAnnotVisibility (document, BY_SERVER, show);

  /* and redisplay the current selector */
  BuildAnnotFilterSelector (document, AnnotSelType);
  /* and clear the selector text */
  AnnotSelItem[0] = WC_EOS;
#ifndef _WINDOWS
  TtaSetSelector (AnnotFilterBase + mFilterSelector, -1, TEXT(""));
#endif /* !_WINDOWS */

  /*
   * Do the visible change on the document
   */

  /* avoid refreshing the document while we're constructing it */
  TtaSetDisplayMode (document, NoComputedDisplay);

  /* initialize */
  el = TtaGetMainRoot (document);
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

  /* show the document */
  TtaSetDisplayMode (document, DisplayImmediately);
}

#ifdef _WINDOWS
/*-----------------------------------------------------------------------
 AnnotFilterDlgProc
 ------------------------------------------------------------------------*/
#ifdef __STDC__
LRESULT CALLBACK WIN_AnnotFilterDlgProc (ThotWindow hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
#else  /* !__STDC__ */
LRESULT CALLBACK WIN_AnnotFilterDlgProc (hwnDlg, msg, wParam, lParam)
ThotWindow   hwndParent;
UINT   msg;
WPARAM wParam;
LPARAM lParam;
#endif /* __STDC__ */
{
  int  index = 0;
  int itemIndex;
  UINT  i = 0; 
    
  switch (msg)
    {
    case WM_INITDIALOG:
	  FilterHwnd = hwnDlg;
      SetWindowText (hwnDlg, "Annotation Local Filter Menu");
      SetWindowText (GetDlgItem (hwnDlg, ID_ANNOTSHOW), "Show selection");
      SetWindowText (GetDlgItem (hwnDlg, ID_ANNOTHIDE), "Hide selection");
      SetWindowText (GetDlgItem (hwnDlg, ID_ANNOTSHOWALL), "Show all");
	  SetWindowText (GetDlgItem (hwnDlg, ID_ANNOTHIDEALL), "Hide all");
      SetWindowText (GetDlgItem (hwnDlg, ID_DONE), TtaGetMessage (LIB, TMSG_DONE));

      BuildAnnotFilterSelector (AnnotFilterDoc, AnnotSelType);
    break;
    
    case WM_CLOSE:
    case WM_DESTROY:
	  FilterHwnd = NULL;
	  /* the filter select window is destroyed automatically when we kill
		  the parent window */
	  FilterSelHwnd = NULL;
      EndDialog (hwnDlg, ID_DONE);
      break;

    case WM_COMMAND:
      if (LOWORD (wParam) == 1)
	{
	  if (HIWORD (wParam) == LBN_SELCHANGE)
	    {
	      itemIndex = SendMessage (FilterSelHwnd, LB_GETCURSEL, 0, 0);
	      itemIndex = SendMessage (FilterSelHwnd, LB_GETTEXT, itemIndex, (LPARAM) AnnotSelItem);
		  break;
	    }
	}

    switch (LOWORD (wParam))
	{
	case ID_ANNOTSHOW:
	  ChangeAnnotVisibility (AnnotFilterDoc, AnnotSelType, 
				 AnnotSelItem, TRUE);
	  /* maybe refresh the dialogue */
	  break;
	  
	case ID_ANNOTHIDE:
	  ChangeAnnotVisibility (AnnotFilterDoc, AnnotSelType, 
				 AnnotSelItem, FALSE);
	  /* maybe refresh the dialogue */
	  break;

	case ID_ANNOTSHOWALL:
	  DocAnnotVisibility (AnnotFilterDoc, AnnotFilterView, TRUE);
	  /* maybe refresh the dialogue and reset the selection */
	  break;

	case ID_ANNOTHIDEALL:
	  DocAnnotVisibility (AnnotFilterDoc, AnnotFilterView, FALSE);
	  /* maybe refresh the dialogue and reset the selection */
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
#else /* _WINDOWS */

/*----------------------------------------------------------------------
   callback of the AnnotFilter menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AnnotFilterCallbackDialog (int ref, int typedata, CHAR_T * data)
#else
static void         AnnotFilterCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
CHAR_T             *data;
#endif /* __STDC__ */
{
  int val;

  if (ref == -1)
    {
      /* removes the AnnotFilter conf menu */
      TtaDestroyDialogue (AnnotFilterBase + AnnotFilterMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
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
	    ustrcpy (AnnotSelItem, data);
	  else
	    AnnotSelItem[0] = WC_EOS;
	  break;
	  
	case mSelectFilter:
	  /* change the content of the selector */
	  /* @@ here I need to have a pointer in memory to the
	     annotation document... means I'll only be able to have
	     one such annotation dialogue at the time */
	  if (AnnotSelType != val)
	    {
	      AnnotSelType = val;
	      AnnotSelItem[0] = WC_EOS;
	      BuildAnnotFilterSelector (AnnotFilterDoc, val);
	      TtaSetSelector (AnnotFilterBase + mFilterSelector, -1, TEXT(""));
	    }
	  break;

	default:
	  break;
	}
    }
}
#endif /* !_WINDOWS */

/*----------------------------------------------------------------------
  AnnotFilter
  Build and display the AnnotFilter Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         AnnotFilter (Document document, View view)
#else
void         AnnotFilter (document, view)
Document            document;
View                view;
#endif /* __STDC__*/
{
	/* local variables */
#ifndef _WINDOWS
  int              i;
#endif /* !_WINDOWS */

#ifndef _WINDOWS
  /* initialize the base if it hasn't yet been done */
  if (AnnotFilterBase == 0)
    AnnotFilterBase =  TtaSetCallback (AnnotFilterCallbackDialog,
					 MAX_ANNOTFILTER_DLG);
#endif /* !_WINDOWS */

  /* make a copy of the current document and view, so that we can
     find this info in the callback */
  AnnotFilterDoc = document;
  AnnotFilterView = view;
  AnnotSelItem[0] = WC_EOS;
  AnnotSelIndex = -1;
  AnnotSelType = 0;

  /* Create the dialogue form */
#ifndef _WINDOWS
  i = 0;
  strcpy (&s[i], TEXT("Show"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("Hide"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("Show all"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("Hide all"));

  TtaNewSheet (AnnotFilterBase + AnnotFilterMenu, 
	       TtaGetViewFrame (document, view),
	       TEXT("Annotation Filter  "), 4, s, TRUE, 2, 'L', 
	       D_DONE);
  
  /* an empty text */
  TtaNewLabel (AnnotFilterBase + mAnnotFilterEmpty1,
	       AnnotFilterBase + AnnotFilterMenu,
	       TEXT("                 "));

  /* the * = filter message */
  TtaNewLabel (AnnotFilterBase + mAnnotFilterLabelStars,
	       AnnotFilterBase + AnnotFilterMenu,
	       TEXT("     a * prefix means hidden"));
	       
  /* create the radio buttons for choosing a selector */
  i = 0;
  strcpy (&s[i], TEXT("BBy author"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("BBy type"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("BBy server"));

  TtaNewSubmenu (AnnotFilterBase + mSelectFilter, 
		 AnnotFilterBase + AnnotFilterMenu,
		 0,
		 TEXT("Filter options"),
		 3,
		 s,
		 NULL,
		 TRUE);

  /* display the selectors */
  BuildAnnotFilterSelector (document, BY_AUTHOR);

  /* choose the BY_AUTHOR radio button */
  TtaSetMenuForm (AnnotFilterBase + mSelectFilter, 0);
#endif /* !_WINDOWS */

  /* display the menu */
#ifndef _WINDOWS
  TtaSetDialoguePosition ();
  TtaShowDialogue (AnnotFilterBase + AnnotFilterMenu, TRUE);
#else /* !_WINDOWS */
  if (!FilterHwnd)
    /* only activate the menu if it isn't active already */
     DialogBox (hInstance, MAKEINTRESOURCE (ANNOTFILTERMENU), NULL, (DLGPROC) WIN_AnnotFilterDlgProc);
  else
     SetFocus (FilterHwnd);
#endif /* !_WINDOWS */
}

/***************************************************
 **  AnnotTypes menu
 ***************************************************/

/*---------------------------------------------------------------
  BuildAnnotTypesSelector
  builds the list showing the different annotation types
------------------------------------------------------------------*/
#ifdef __STDC__
static void BuildAnnotTypesSelector (void)
#else
static void BuildAnnotTypesSelector ()
#endif /* __STDC__ */
{
  int                   nb_entries;
  int                   i;
  RDFClassP		annotClass;

  nb_entries = 0;
  if (&typesList)
    List_delAll (&typesList, List_delCharObj);
  ustrcpy (s, TEXT(""));
  i = 0;

  annotClass = ANNOT_FindRDFResource (&annot_schema_list,
				      ANNOTATION_PROP,
				      FALSE);

  if (annotClass && annotClass->class)
    {
      List *item;

      for (item=annotClass->class->subClasses; item; item=item->next)
	{
	  RDFClassP subType = (RDFClassP)item->object;
	  TypeSelector *t = (TypeSelector*)TtaGetMemory (sizeof(TypeSelector));

	  t->type = subType;
	  t->name = ANNOT_GetLabel(&annot_schema_list, subType);
	  List_add (&typesList, (void*)t);

	  ustrcpy (&s[i], t->name);
	  i += ustrlen (&s[i]);
	  s[i] = WC_EOS;
	  i++;
	  nb_entries++;
	}
    }
  else
    {
      /* @@ RRS use the default values */
      ustrcpy (&s[i], TEXT("positive comment"));
      i += ustrlen (&s[i]) + 1;
      ustrcpy (&s[i], TEXT("flame"));
      nb_entries = 2;
    }

#ifndef _WINDOWS
   /* Fill in the form  */
  TtaNewSelector (AnnotTypesBase + mAnnotTypesSel,
		  AnnotTypesBase + AnnotTypesMenu,
		  NULL,
		  nb_entries,
		  s,
		  (nb_entries > 5) ? 5 : nb_entries,
		  NULL,
		  TRUE,
		  TRUE);
#endif /* !_WINDOWS */
}

/*---------------------------------------------------------------
  BuildAnnotTypesSelector2
  builds the list showing the different annotation types.
  Returns the number of entries in the menu.
------------------------------------------------------------------*/
#ifdef __STDC__
static int BuildAnnotTypesSelector2 (Document doc)
#else
static	int BuildAnnotTypesSelector2 (doc)
Document doc;
#endif /* __STDC__ */
{
  int                   nb_entries;
  int                   i;
  RDFClassP		annotClass;

  nb_entries = 0;
  if (typesList)
    List_delAll (&typesList, List_delCharObj);
  ustrcpy (s, TEXT(""));
  i = 0;

  annotClass = ANNOT_FindRDFResource (&annot_schema_list,
				      ANNOTATION_PROP,
				      FALSE);

  if (annotClass && annotClass->class)
    {
      List *item;

      for (item=annotClass->class->subClasses; item; item=item->next)
	{
	  RDFClassP subType = (RDFClassP)item->object;
	  TypeSelector *t = (TypeSelector *) TtaGetMemory (sizeof(TypeSelector));

	  t->type = subType;
	  t->name = ANNOT_GetLabel(&annot_schema_list, subType);
	  List_add (&typesList, (void *) t);

	  usprintf (&s[i], "B%s", t->name);
	  i += ustrlen (&s[i]);
	  s[i] = WC_EOS;
	  i++;
	  nb_entries++;
	}
    }
  else
    {
      /* @@ RRS use the default values */
      ustrcpy (&s[i], TEXT("Bpositive comment"));
      i += ustrlen (&s[i]) + 1;
      ustrcpy (&s[i], TEXT("Bflame"));
      nb_entries = 2;
    }

  /* create the main menu */
  TtaNewPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1),
	       NULL, nb_entries, s, NULL, 'L');

  return nb_entries;
}

/*----------------------------------------------------------------------
   callback of the AnnotTypes menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AnnotTypesCallbackDialog (int ref, int typedata, CHAR_T * data)
#else
static void         AnnotTypesCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
CHAR_T             *data;
#endif /* __STDC__ */
{
  int val;

  if (ref == -1)
    {
      /* removes the AnnotTypes conf menu */
      TtaDestroyDialogue (AnnotTypesBase + AnnotTypesMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - AnnotTypesBase)
	{
	case AnnotTypesMenu:
	  switch (val) 
	    {
	      /** @@ I need to add a case for cancel */
	    case 0:
	      AnnotTypesSelItem[0] = WC_EOS;
	      List_delAll (&typesList, List_delCharObj);
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      List_delAll (&typesList, List_delCharObj);
	      TtaDestroyDialogue (ref);
	      break;
	    default:
	      break;
	    }
	  break;

	case mAnnotTypesSel:
	  /* copy what was selected */
	  if (data)
	    ustrcpy (AnnotTypesSelItem, data);
	  else
	    AnnotTypesSelItem[0] = WC_EOS;
	  break;
	  
	default:
	  break;
	}
    }
}

/*----------------------------------------------------------------------
  AnnotTypes
  Returns the RDF Resource pointer that represents the type selection
  of the user. It is NULL if the user doesn't select a type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
RDFResourceP AnnotTypes (Document document, View view)
#else
RDFResourceP AnnotTypes (document, view)
Document            document;
View                view;
#endif /* __STDC__*/
{
#ifndef _WINDOWS
  /* initialize the base if it hasn't yet been done */
  if (AnnotTypesBase == 0)
    AnnotTypesBase =  TtaSetCallback (AnnotTypesCallbackDialog,
				      MAX_ANNOTTYPES_DLG);

  /* make a copy of the current document and view, so that we can
     find this info in the callback */
  AnnotTypesSelItem[0] = WC_EOS;
  
  /* Create the dialogue form */
  TtaNewForm (AnnotTypesBase + AnnotTypesMenu, 
	      TtaGetViewFrame (document, view),
	      TEXT("Annotation Type"), TRUE, 1, 'L', D_CANCEL);
  
  /* display the selectors */
  BuildAnnotTypesSelector ();
  
  /* display the menu */
  TtaSetDialoguePosition ();
  TtaShowDialogue (AnnotTypesBase + AnnotTypesMenu, FALSE);
  TtaWaitShowDialogue ();

  {
    List *item;
    for (item = typesList; item; item=item->next)
      {
	TypeSelector *t = (TypeSelector*)item->object;
	if (!ustrcmp(t->name, AnnotTypesSelItem))
	  return t->type;
      }
  }
#endif /* _WINDOWS */
  return NULL;
}

/*----------------------------------------------------------------------
  AnnotTypes2
  Returns the RDF Resource pointer that represents the type selection
  of the user. It is NULL if the user doesn't select a type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
RDFResourceP AnnotTypes2 (Document document, View view)
#else
RDFResourceP AnnotTypes2 (document, view)
Document            document;
View                view;
#endif /* __STDC__*/
{
  RDFResourceP result = NULL;
  int nb_entries;

  /* prepare the selector */
  nb_entries = BuildAnnotTypesSelector2 (document);
  
  /* activate the menu that has just been created */
  ReturnOption = -1;
  ReturnOptionMenu = -1;
#ifndef _WINDOWS
  TtaSetDialoguePosition ();
#endif /* !_WINDOWS */
  TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
  /* wait for an answer from the user */
  TtaWaitShowDialogue ();
  if (ReturnOption >= 0 && ReturnOptionMenu >= 0) 
    {
      /* make the returned option selected */
      if (ReturnOptionMenu == 0)
	{ /* an item in the main (SELECT) menu */
	  List *item;
	  int entry;

	  entry = nb_entries - ReturnOptionMenu - 1;
	  for (item = typesList; item && (entry > 0); item=item->next, entry);
	  if (item->object)
	    result = ((TypeSelector *) item->object)->type;
	}
    }
  List_delAll (&typesList, List_delCharObj);
  return result;
}










