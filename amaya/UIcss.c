/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

#define CSSForm         0
#define CSSSelect       1
#define MAX_CSS_REF     2

static int         BaseCSS;
static STRING      CSSpath;
static Document    DocCSS;

#include "AHTURLTools_f.h"
#include "EDITORactions_f.h"
#include "HTMLedit_f.h"
#include "UIcss_f.h"
#include "css_f.h"
#include "init_f.h"

/*----------------------------------------------------------------------
   Callback procedure for dialogue events.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CallbackCSS (int ref, int typedata, STRING data)
#else
static void         CallbackCSS (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;
#endif
{
  Element             el;
  ElementType         elType;
  AttributeType       attrType;
  Attribute           attr;
  CHAR                s[MAX_LENGTH];
  int                 val, length;
  boolean             found;

  val = (int) data;
  switch (ref - BaseCSS)
    {
    case CSSForm:
      TtaDestroyDialogue (ref);
      if (val == 1 && CSSpath != NULL)
	/* display the CSS file */
	GetHTMLDocument (CSSpath, NULL, 0, 0, CE_ABSOLUTE, FALSE, NULL, NULL);
      else if (val == 2)
	/* re-parse the CSS file */
	  ;
      else if (val == 3)
	{
	  /* remove the link to this file */
	  el = TtaGetMainRoot (docCSS);
	  elType = TtaGetElementType (el);
	  elType.ElTypeNum = HTML_EL_LINK;
	  /* look for the Link element */
	  el = TtaSearchTypedElement (elType, SearchInTree, el);
	  attrType.AttrSSchema = elType.ElSSchema;
	  attrType.AttrTypeNum = HTML_ATTR_HREF_;
	  found = FALSE;
	  while (el != NULL && !found)
	    {
	      attr = TtaGetAttribute (el, attrType);
	      if (attr != 0)
		{
		  length = MAX_LENGTH;
		  TtaGiveTextAttributeValue (attr, s, &length);
		}
	      if (!found)
		el = TtaSearchTypedElement (elType, SearchForward, el);
	    }
	  if (el)
	    RemoveLink (el, docCSS);
	}
      else if (val == 4)
	/* add a new link to a CSS file */
	CreateLinkInHead (DocCSS, 1);
      /* clean CSSpath */
      TtaFreeMemory (CSSpath);
      CSSpath = NULL;
      break;
    case CSSSelect:
      if (CSSpath != NULL)
	TtaFreeMemory (CSSpath);
      length = ustrlen (data);
      CSSpath = TtaGetMemory (length + 1);
      ustrcpy (CSSpath, data);      
      break;
    }
}

/*----------------------------------------------------------------------
   InitCSS                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitCSS (void)
#else
void                InitCSS ()
#endif
{
   /* initialize the dialogs */
   BaseCSS = TtaSetCallback (CallbackCSS, MAX_CSS_REF);
   CSSpath = NULL;
}

/*----------------------------------------------------------------------
   InitCSSDialog list downloaded CSS files
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitCSSDialog (Document doc, View view)
#else
void                InitCSSDialog (doc, view)
Document            doc;
View                view;
#endif
{
  CSSInfoPtr          css;
  CHAR                buf[400];
  STRING              ptr;
  CHAR                s[MAX_LENGTH];
  int                 i;
  int                 len, nb;
  int                 index, size;

  DocCSS = doc;
  css = CSSList;
  buf[0] = 0;
  index = 0;
  nb = 0;
  size = 400;
#  ifndef _WINDOWS
  /* create the form */
  i = 0;
  ustrcpy (&s[i], TtaGetMessage (LIB, TMSG_OPEN));
  i += ustrlen (&s[i]) + 1;
  ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_BROWSE));
  i += ustrlen (&s[i]) + 1;
  ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_REMOVE));
  i += ustrlen (&s[i]) + 1;
  ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_ADD));
  TtaNewSheet (BaseCSS + CSSForm, TtaGetViewFrame (doc, 1), TtaGetMessage (AMAYA, AM_CSS), 4, s, TRUE, 2, 'L', D_DONE);
#  endif /* !_WINDOWS */
  while (css != NULL)
    {
      if (css->category != CSS_DOCUMENT_STYLE && css->documents[doc])
	{
	  /* build the CSS list */
	  if (css->url == NULL)
	    ptr = css->localName;
	  else
	    ptr = css->url;
	  len = ustrlen (ptr);
	  len++;
	  if (size < len)
	    break;
	  ustrcpy (&buf[index], ptr);
	  index += len;
	  nb++;
	  size -= len;
	}	
      css = css->NextCSS;
    }

  /* display the form */
#  ifdef _WINDOWS
  CreateCSSDlgWindow (BaseCSS, CSSSelect, CSSForm, TtaGetMessage (AMAYA, AM_CSS_FILE), nb, buf);
#  else  /* !_WINDOWS */
  if (nb > 0)
    TtaNewSelector (BaseCSS + CSSSelect, BaseCSS + CSSForm,
		    TtaGetMessage (AMAYA, AM_CSS_FILE),
		    nb, buf, 5, NULL, FALSE, TRUE);
  TtaShowDialogue (BaseCSS + CSSForm, TRUE);
#  endif /* !_WINDOWS */
}
