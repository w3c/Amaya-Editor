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


#define THOT_EXPORT extern
#include "HTML.h"
#include "annotlib.h"
#include "ANNOTmenu.h"

/*************
 ** Annot Filter menu
 *************/
/* the selector type used to show/hide annotations */
typedef enum _SelType {
  BY_AUTHOR = 0,
  BY_TYPE,
  BY_SERVER
} SelType;
  
/* common local variables */
CHAR_T  s[MAX_LENGTH]; /* general purpose buffer */

static int      CustomQueryBase;
static int      AnnotFilterBase;
/* copies of the doc and view from which the menu was invoked */
static Document AnnotFilterDoc;
static View     AnnotFilterView;
static CHAR_T   AnnotSelItem[MAX_LENGTH];
static SelType  AnnotSelType;
 
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
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      /* get default */
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

	case mFreeText :
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
		   TRUE);

   TtaNewLabel (CustomQueryBase + mExpertMode,
		CustomQueryBase + CustomQueryMenu,
		TEXT("Expert mode"));

   TtaNewTextForm (CustomQueryBase + mFreeText,
		   CustomQueryBase + CustomQueryMenu,
		   TEXT("Free Alguea query"),
		   40,
		   1,
		   TRUE);

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

  /* avoid refreshing the document while we're constructing it */
  TtaSetDisplayMode (document, NoComputedDisplay);

  /* initialize */
  el = TtaGetMainRoot (document);
  elType = TtaGetElementType (el);

  attrType.AttrSSchema = elType.ElSSchema;
  elType.ElTypeNum = HTML_EL_Anchor;

  while ((el = TtaSearchTypedElement (elType, SearchForward, el)))
    {
      attrType.AttrTypeNum = HTML_ATTR_Annotation;
      attr = TtaGetAttribute (el, attrType);
      if (!attr)
	/* it's not an annotation link */
	continue;
      attrType.AttrTypeNum = HTML_ATTR_AnnotationHide;
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
  CHAR_T                *sel_entry;
  List                  *item;
  AnnotMeta             *annot;

   /* count and copy the entries that we're interested in */
  item = AnnotMetaDataList[doc];
  nb_entries = 0;
  ustrcpy (s, TEXT(""));
  i = 0;
  while (item)
     {
       annot = (AnnotMeta *) item->object;
       /* @@ the idea is to use the different selection types here */
       /* @@ we need to add memory dynamically, to avoid the memory overrun */
       /* we need to check that one entry didn't exist already */
       switch (selector)
	 {
	 case BY_AUTHOR:
	   sel_entry = annot->author;
	   break;
	 case BY_TYPE:
	   sel_entry = annot->type;
	   break;
	 case BY_SERVER:
	   sel_entry = NULL;
	   /*
	   sel_entry = NormalizeURL (get server!);
	   */
	   break;
	 default:
	   sel_entry = NULL;
	   break;
	 }
       if (sel_entry == NULL)
	 {
	   item = item->next;
	   continue;
	 }
       ustrcpy (&s[i], sel_entry);
       i += ustrlen (&s[i]) + 1;
       nb_entries++;
       item = item->next;
     }

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

   /* remove the selector */
   TtaSetSelector (AnnotFilterBase + mFilterSelector, -1, TEXT(""));
}

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
#if 0
	      ToggleAnnotVisibility (AnnotSel, AnnotSelType);
#endif
	      /* maybe refresh the dialogue */
	      break;
	    case 2:
	      DocAnnotVisibility (AnnotFilterDoc, AnnotFilterView, TRUE);
	      /* maybe refresh the dialogue and reset the selection */
	      break;
	    case 3:
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
	  AnnotSelType = val;
	  BuildAnnotFilterSelector (1, val);
	  break;

	default:
	  break;
	}
    }
}

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
  int              i;

  /* initialize the base if it hasn't yet been done */
  if (AnnotFilterBase == 0)
    AnnotFilterBase =  TtaSetCallback (AnnotFilterCallbackDialog,
					 MAX_ANNOTFILTER_DLG);

  /* make a copy of the current document and view, so that we can
     find this info in the callback */
  AnnotFilterDoc = document;
  AnnotFilterView = view;

  /* Create the dialogue form */
  i = 0;
  strcpy (&s[i], TEXT("Toggle selection"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("Show all"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("Hide all"));

  TtaNewSheet (AnnotFilterBase + AnnotFilterMenu, 
	       TtaGetViewFrame (document, view),
	       TEXT("Show/Hide annotations"), 3, s, TRUE, 1, 'L', 
	       D_DONE);
  
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

  /* @@ we should save the last selector type */
  BuildAnnotFilterSelector (document, BY_AUTHOR);

  /* display the menu */
  TtaSetDialoguePosition ();
  TtaShowDialogue (AnnotFilterBase + AnnotFilterMenu, TRUE);
}








