/***
 *** Copyright (c) 1996 INRIA, All rights reserved
 ***/

/* Included headerfiles */
#include "amaya.h"

#include "content.h"
#include "tree.h"
#include "browser.h"
#include "interface.h"
#include "selection.h"
#include "presentation.h"
#include "dialog.h"
#include "app.h"
#include "message.h"
#include "HTMLstyle.h"


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
