/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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
 
/* Included headerfiles */
#define EXPORT extern
#include "amaya.h"
#include "css.h"

#include "HTMLstyle_f.h"


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DeleteStyleRule (NotifyElement * event)
#else  /* __STDC__ */
boolean             DeleteStyleRule (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   RemoveStyleRule (event->element, event->document);
   return TRUE;			/* let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangePRule (NotifyPresentation * event)
#else  /* __STDC__ */
void                ChangePRule (event)
NotifyPresentation *event;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           styleAttr;
   char                style[100];
   int                 len;

   /* does the element have a Style_ attribute ? */
   attrType.AttrSSchema = TtaGetDocumentSSchema (event->document);
   attrType.AttrTypeNum = HTML_ATTR_Style_;
   styleAttr = TtaGetAttribute (event->element, attrType);
   /* keep the new style string */
   len = 100;
   GetHTML3StyleString (event->element, event->document, style, &len);
   if (len == 0)
     {
	/* delete the style attribute */
	if (styleAttr != 0)
	   TtaRemoveAttribute (event->element, styleAttr, event->document);
     }
   else
     {
	if (styleAttr == 0)
	  {
	     styleAttr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (event->element, styleAttr, event->document);
	  }
	/* copie the style string into the style attribute */
	TtaSetAttributeText (styleAttr, style, event->element, event->document);
     }
}
