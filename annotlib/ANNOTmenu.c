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

/* common local variables */
CHAR_T  s[MAX_LENGTH]; /* general purpose buffer */

static int      CustomQueryBase;
static int      AnnotShowHideBase;

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

   AnnotShowHide (document, view);
   return;

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

/*----------------------------------------------------------------------
  Annot_ShowHideAnnotations
  Shows or hides the annotations in a given document
  @@ we need a table of the show/hide status per document, so that
  @@ we can reset it when closing that document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void ANNOT_ShowHideAnnotations (Document document, View view)
#else
void ANNOT_ShowHideAnnotations (document, view)
Document document;
View view;
#endif /* __STDC__ */
{
  static ThotBool show = TRUE;
  ElementType elType;
  Element     el;
  Attribute           attr;
  AttributeType       attrType;

  /* toggle the last status */
  show = !show;
 
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

/*----------------------------------------------------------------------
   callback of the AnnotShowHide menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AnnotShowHideCallbackDialog (int ref, int typedata, CHAR_T * data)
#else
static void         AnnotShowHideCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
CHAR_T             *data;
#endif /* __STDC__ */
{
  int val;

  if (ref == -1)
    {
      /* removes the AnnotShowHide conf menu */
      TtaDestroyDialogue (AnnotShowHideBase + AnnotShowHideMenu);
    }
  else
    {
      /* has the user changed the options? */
      val = (int) data;
      switch (ref - AnnotShowHideBase)
	{
	case AnnotShowHideMenu:
	  switch (val) 
	    {
	    case 0:
	      TtaDestroyDialogue (ref);
	      break;
	    case 1:
	      TtaDestroyDialogue (ref);
	      break;
	    case 2:
	      break;
	    default:
	      break;
	    }
	  break;

	case mAnnotShowHideSelector:
	  /* copy what was selected */
#if 0 
	  if (data)
	    ustrcpy (TemplatesUrl, data);
	  else
	    TemplatesUrl [0] = EOS;
#endif
	  break;
	  
	default:
	  break;
	}
    }
}

/*---------------------------------------------------------------
  BuildAnnotShowHideSelector builds the list allowing to select a profile
------------------------------------------------------------------*/
#ifdef __STDC__
static void BuildAnnotShowHideSelector (Document doc)
#else
static void BuildAnnotShowHideSelector (doc)
Document doc;
#endif /* __STDC__ */
{
  int                   nb_entries;
  int                   i;
  CHAR_T                *entry;
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
       if (annot->author)
	 entry = annot->author;
       else
	 continue;
       ustrcpy (&s[i], entry);
       i += ustrlen (&s[i]) + 1;
       nb_entries++;
       item = item->next;
     }

   /* Fill in the form  */
   TtaNewSelector (AnnotShowHideBase + mAnnotShowHideSelector, 
		   AnnotShowHideBase + AnnotShowHideMenu,
		   "",
		   nb_entries,
		   s,
		   (nb_entries < 5) ? nb_entries : 5,
		   NULL,
		   FALSE, 
		   FALSE);
}

/*----------------------------------------------------------------------
  AnnotShowHide
  Build and display the AnnotShowHide Menu dialog box and prepare for input.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         AnnotShowHide (Document document, View view)
#else
void         AnnotShowHide (document, view)
Document            document;
View                view;
#endif /* __STDC__*/
{
  int              i;

  /* initialize the base if it hasn't yet been done */
  if (AnnotShowHideBase == 0)
    AnnotShowHideBase =  TtaSetCallback (AnnotShowHideCallbackDialog,
					 MAX_ANNOTSHOWHIDE_DLG);
  /* Create the dialogue form */
  i = 0;
  strcpy (&s[i], TEXT("Show/Hide selection"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("Select all"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("Show all"));
  i += ustrlen (&s[i]) + 1;
  strcpy (&s[i], TEXT("Hide all"));

  TtaNewSheet (AnnotShowHideBase + AnnotShowHideMenu, 
	       TtaGetViewFrame (document, view),
	       TEXT("Show/Hide annotations"), 2, s, TRUE, 1, 'L', 
	       D_DONE);
  BuildAnnotShowHideSelector (document);

  /* display the menu */
  TtaSetDialoguePosition ();
  TtaShowDialogue (AnnotShowHideBase + AnnotShowHideMenu, TRUE);
}







