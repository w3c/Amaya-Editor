/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: I. Vatton (INRIA)
 *         R. Guetari - Adaptation to Windows platforms.
 *
 */
 
#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "app.h"
#include "appdialogue.h"
#include "tree.h"
#include "libmsg.h"
#include "message.h"
#include "dialog.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

static PtrDocument   ViewToOpenDoc;
static int           ViewToOpenNumber;
static PtrElement    ViewToOpenSubTree;
static DocViewNumber ReferenceView;

#include "views_f.h"
#include "appdialogue_f.h"
#include "viewapi_f.h"

#ifdef _WINDOWS
extern boolean viewClosed ;
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   CallbackOpenView
   updates the openview form
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackOpenView (int ref, int dataType, char *data)
#else  /* __STDC__ */
void                CallbackOpenView (ref, dataType, data)
int                 ref;
int                 dataType;
char               *data;

#endif /* __STDC__ */
{
   if ((int) data >= 0)
      if (ViewToOpenDoc != NULL)
	 if (ViewToOpenDoc->DocSSchema != NULL)
	   {
	      ViewToOpenNumber = (int) data;
	      OpenViewByMenu (ViewToOpenDoc, ViewToOpenNumber,
			      ViewToOpenSubTree, ReferenceView);
	   }
}


/*----------------------------------------------------------------------
   TtcOpenView
   initializes the openview menu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcOpenView (Document document, View view)
#else  /* __STDC__ */
void                TtcOpenView (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 k, l, nbItems;
   char               *src;
   char               *dest;
   char                buf[MAX_TXT_LEN];
   char                bufMenu[MAX_TXT_LEN];

   PtrDocument         pDoc;

   /* initial connection */
   if (ThotLocalActions[T_openview] == NULL)
     {
	/* connects the callback actions associated to the command */
	TteConnectAction (T_openview, (Proc) CallbackOpenView);
     }

   pDoc = LoadedDocument[document - 1];
   ViewToOpenDoc = pDoc;
   ReferenceView = view;

   if (ViewToOpenDoc != NULL)
     {
	/* builds a menu of views which we can open */
	BuildViewList (pDoc, buf, &nbItems);
	if (nbItems == 0)
	   TtaDisplaySimpleMessage (INFO, LIB, TMSG_ALL_VIEWS_ALREADY_CREATED);
	else
	  {
	     ViewToOpenNumber = -1;
	     /* adds a 'B' at the beginning of each menu entry */
	     dest = &bufMenu[0];
	     src = &buf[0];
	     for (k = 1; k <= nbItems; k++)
	       {
		  strcpy (dest, "B");
		  dest++;
		  l = strlen (src);
		  strcpy (dest, src);
		  dest += l + 1;
		  src += l + 1;
	       }
	     TtaNewPopup (NumMenuViewsToOpen, 0, TtaGetMessage (LIB, TMSG_VIEWS), nbItems,
			  bufMenu, NULL, 'L');
	     TtaShowDialogue (NumMenuViewsToOpen, FALSE);
	  }
     }
}


/*----------------------------------------------------------------------
   TtcCloseView
   closes a document view.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcCloseView (Document document, View viewIn)
#else  /* __STDC__ */
void                TtcCloseView (document, viewIn)
Document            document;
View                viewIn;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 view;
   boolean             assoc;

#  ifdef _WINDOWS
   viewClosed = TRUE;
#  endif /* _WINDOWS */

   pDoc = LoadedDocument[document - 1];
   if (pDoc != NULL)
     {
	GetViewInfo (document, viewIn, &view, &assoc);
	CloseView (pDoc, view, assoc);
     }
}


/*----------------------------------------------------------------------
   TtcSynchronizeView
   Switches the synchronizaton mode of the view of a document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SwitchSynchronize (Document document, View view)
#else  /* __STDC__ */
void                SwitchSynchronize (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;

   if (document == 0 && view == 0)
      return;
   /* the views of associated elements are not concerned */
   else if (view < 100)
     {
	/* changes the synchronization mode of the view */
	pDoc = LoadedDocument[document - 1];
	pDoc->DocView[view - 1].DvSync = !pDoc->DocView[view - 1].DvSync;
     }
}








