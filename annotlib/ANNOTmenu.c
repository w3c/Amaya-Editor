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
#include "annotlib.h"
#include "ANNOTmenu.h"

/* common local variables */
CHAR_T  s[MAX_LENGTH]; /* general purpose buffer */

static int      CustomQueryBase;


#ifndef _WINDOWS
/*----------------------------------------------------------------------
  ProxyCallbackDialog
  callback of the proxy configuration menu
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

