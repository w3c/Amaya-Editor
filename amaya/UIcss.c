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

#include "css_f.h"
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
#include "query_f.h"
#endif /* !AMAYA_JAVA  && !AMAYA_ILU */
#include "p2css_f.h"
#include "AHTURLTools_f.h"
#include "UIcss_f.h"
 
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
  char                buf[400];
  char               *ptr;
  int                 len, nb;
  int                 index, size;

  css = CSSList;
  buf[0] = 0;
  index = 0;
  nb = 0;
  size = 400;
  while (css != NULL)
    {
      if (css->category != CSS_DOCUMENT_STYLE)
	{
#  ifndef _WINDOWS
	  if (nb == 0)
	    /* create the form */
	    TtaNewForm (BaseDialog + FromCSS, TtaGetViewFrame (doc, 1), 
			TtaGetMessage (AMAYA, AM_CSS), FALSE, 2, 'L', D_DONE);
#  endif /* !_WINDOWS */
	  /* build the CSS list */
	  if (css->url == NULL)
	    ptr = css->localName;
	  else
	    ptr = css->url;
	  len = strlen (ptr);
	  len++;
	  if (size < len)
	    break;
	  strcpy (&buf[index], ptr);
	  index += len;
	  nb++;
	  size -= len;
	}	
      css = css->NextCSS;
    }

  /* display the form */
  if (nb > 0)
    {
#  ifndef _WINDOWS
      TtaNewSelector (BaseDialog + CSSSelect, BaseDialog + FromCSS,
		      TtaGetMessage (AMAYA, AM_CSS_FILE),
		      nb, buf, 5, NULL, TRUE, FALSE);
      TtaShowDialogue (BaseDialog + FromCSS, TRUE);
#  endif /* !_WINDOWS */
    }
}
