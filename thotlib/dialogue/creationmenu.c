/*
 *
 *  (C) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * This module handles the creation commands
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "message.h"
#include "dialog.h"
#include "constmedia.h"
#include"constmenu.h"
#include "typemedia.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "modif_tv.h"
#include "select_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"

#include "config_f.h"
#include "structcreation_f.h"

/*----------------------------------------------------------------------
  BuildChoiceMenu
  ----------------------------------------------------------------------*/
void BuildChoiceMenu (char *bufMenu, Name menuTitle, int nbEntries,
		      ThotBool natureChoice)
{
#ifdef IV
   int                 menu;
   char                bufMenuB[MAX_TXT_LEN];
   char               *src, *dest;
   int                 k, l, nbitem, length;

   if (natureChoice)
     {
	menu = NumFormNature;
#ifndef _WINDOWS
	/* selector stating the nature of the element to create (or of the capture zone
	   if the configuration files don't define any natures */
	TtaNewForm (NumFormNature, 0, TtaGetMessage (LIB, TMSG_OBJECT_TYPE), TRUE, 1, 'L', D_CANCEL);
#endif /* !_WINDOWS */
	nbitem = ConfigMakeDocTypeMenu (bufMenuB, &length, FALSE);
	if (nbitem > 0)
	   /* the Start Up file defines the natures */
	  {
	     /* computes the height of the menu part of the selector */
	     if (nbitem < 5)
		length = nbitem;
	     else
		length = 5;
	     /* creates the selector */
#ifndef _WINDOWS
	     TtaNewSelector (NumSelectNatureName, NumFormNature,
			     TtaGetMessage (LIB, TMSG_OBJECT_TYPE), nbitem, bufMenuB, length, NULL, TRUE, FALSE);
	     /* sets the selector on its first entry */
	     TtaSetSelector (NumSelectNatureName, 0, "");
#endif /* !_WINDOWS */
	  }
	else
	   /* we did not create a selector, we create a capture zone having
	      the nature of the element to create */
#ifndef _WINDOWS
	   TtaNewTextForm (NumSelectNatureName, NumFormNature, TtaGetMessage (LIB, TMSG_OBJECT_TYPE), 30, 1, FALSE)
#endif /* !_WINDOWS */
	   ;
     }
   else
     {
	menu = NumMenuElChoice;
	/* adds 'B' to the beginning of each entry of the menu */
	dest = &bufMenuB[0];
	src = &bufMenu[0];
	for (k = 1; k <= nbEntries; k++)
	  {
	     strcpy (dest, "B");
	     dest++;
	     l = strlen (src);
	     strcpy (dest, src);
	     dest += l + 1;
	     src += l + 1;
	  }
	TtaNewPopup (NumMenuElChoice, 0, menuTitle, nbEntries, bufMenuB, NULL, 'L');
     }
#ifndef _WINDOWS
   TtaShowDialogue (menu, FALSE);
   /* waits until the user has answered to the menu and that the 
      mediator has called ChoiceMenuCallback */
   TtaWaitShowDialogue ();
#endif /* !_WINDOWS */
#endif /* IV */
}


/*----------------------------------------------------------------------
   InsertSeparatorInMenu 
   Inserts a separator in the menu Insert/Paste/Include
  ----------------------------------------------------------------------*/
void InsertSeparatorInMenu (int *prevMenuInd, int *nbEntries, int *menuInd,
			    char *bufMenu)
{
   *prevMenuInd = *menuInd;
   /* indicates if it's a separator */
   bufMenu[*menuInd] = 'S';
   (*menuInd)++;
   bufMenu[*menuInd] = EOS;
   (*menuInd)++;
   (*nbEntries)++;
}

/*----------------------------------------------------------------------
  BuildPasteMenu
  ----------------------------------------------------------------------*/
void BuildPasteMenu (int RefMenu, char *bufMenu, Name title,
		     int nbEntries, char button)
{
   char                bufMenuB[MAX_TXT_LEN];
   char               *src, *dest;
   int                 k, l;

   /* adds 'B' to the beginning of each entry of the menu */
   dest = &bufMenuB[0];
   src = &bufMenu[0];
   for (k = 1; k <= nbEntries; k++)
     {
	l = strlen (src);
	/* don't add 'B' to the beginning of separators */
	if (*src != 'S' || l != 1)
	  {
	     strcpy (dest, "B");
	     dest++;
	  }
	strcpy (dest, src);
	dest += l + 1;
	src += l + 1;
     }
   TtaNewPopup (RefMenu, 0, title, nbEntries, bufMenuB, NULL, button);
#ifndef _WINDOWS
   TtaShowDialogue (RefMenu, FALSE);
   /* waits for the user's answer */
   TtaWaitShowDialogue ();
#endif /* !_WINDOWS */
}










