/*
 *
 *  (C) COPYRIGHT INRIA, 1996.
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

static ThotBool     AnswerMenuAskForNew;	/* valid answer to the create/designate menu */
static ThotBool     AnswerCreateAskForNew;	/* answer AnswerCreateAskForNew to the create/designate menu */ 

#include "structcreation_f.h"
 
#ifdef __STDC__
extern int          ConfigMakeDocTypeMenu (STRING, int*, ThotBool);

#else  /* __STDC__ */
extern int          ConfigMakeDocTypeMenu ();

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   AskForNew_RemplRefer
   After setting up  a reference, this function asks the user if he wants
   to create at the same time the referenced element or if he just wants
   to designate an existing element.
   It returns FALSE if the user aborts the command. Otherwise
   generate is TRUE if the user wants to create the referenced
   element, FALSE if he only wants to designate it.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            AskForNew_RemplRefer (ThotBool * generate, Name typeName)
#else  /* __STDC__ */
ThotBool            AskForNew_RemplRefer (generate, typeName)
ThotBool           *generate;
Name                typeName;

#endif /* __STDC__ */
{
   int                 i;
   CHAR_T                bufMenu[MAX_TXT_LEN];

   /* creates and activates the menu */
   i = 0;
   usprintf (&bufMenu[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_CREATE_EL_REF));
   i += ustrlen (&bufMenu[i]) + 1;
   usprintf (&bufMenu[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_SHOW_EL_REF));
   TtaNewPopup (NumMenuCreateReferenceElem, 0, TtaGetMessage (LIB, TMSG_MODE_INSERT), 2, bufMenu, NULL, 'L');
#  ifndef _WINDOWS
   TtaShowDialogue (NumMenuCreateReferenceElem, FALSE);
   /* waits until the user replies to the menu */
   TtaWaitShowDialogue ();
#  endif /* !_WINDOWS */
   *generate = AnswerCreateAskForNew;
   return AnswerMenuAskForNew;
}

/*----------------------------------------------------------------------
   CallbackAskForNew
   handles the callback of the menu which asks the user if he wants
   to designate a referenced element or if if he prefers to create
   a new element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackAskForNew (int Val)
#else  /* __STDC__ */
void                CallbackAskForNew (Val)
int                 Val;

#endif /* __STDC__ */
{
   if (Val == 0)
     {
	AnswerMenuAskForNew = TRUE;
	AnswerCreateAskForNew = TRUE;
     }
   if (Val == 1)
     {
	AnswerMenuAskForNew = TRUE;
	AnswerCreateAskForNew = FALSE;
     }
}


/*----------------------------------------------------------------------
  BuildChoiceMenu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildChoiceMenu (STRING bufMenu, Name menuTitle, int nbEntries, ThotBool natureChoice)
#else  /* __STDC__ */
void                BuildChoiceMenu (bufMenu, menuTitle, nbEntries, natureChoice)
STRING              bufMenu;
Name                menuTitle;
int                 nbEntries;
ThotBool            natureChoice;

#endif /* __STDC__ */
{
   int                 menu;
   CHAR_T              bufMenuB[MAX_TXT_LEN];
   STRING              src;
   STRING              dest;
   int                 k, l, nbitem, length;

   if (natureChoice)
     {
	menu = NumFormNature;
#   ifndef _WINDOWS
	/* selector stating the nature of the element to create (or of the capture zone
	   if the configuration files don't define any natures */
	TtaNewForm (NumFormNature, 0, TtaGetMessage (LIB, TMSG_OBJECT_TYPE), TRUE, 1, 'L', D_CANCEL);
#   endif /* !_WINDOWS */
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
#        ifndef _WINDOWS
	     TtaNewSelector (NumSelectNatureName, NumFormNature,
			     TtaGetMessage (LIB, TMSG_OBJECT_TYPE), nbitem, bufMenuB, length, NULL, TRUE, FALSE);
	     /* sets the selector on its first entry */
	     TtaSetSelector (NumSelectNatureName, 0, TEXT(""));
#        endif /* !_WINDOWS */
	  }
	else
	   /* we did not create a selector, we create a capture zone having
	      the nature of the element to create */
#      ifndef _WINDOWS
	   TtaNewTextForm (NumSelectNatureName, NumFormNature, TtaGetMessage (LIB, TMSG_OBJECT_TYPE), 30, 1, FALSE)
#      endif /* !_WINDOWS */
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
	     ustrcpy (dest, TEXT("B"));
	     dest++;
	     l = ustrlen (src);
	     ustrcpy (dest, src);
	     dest += l + 1;
	     src += l + 1;
	  }
	TtaNewPopup (NumMenuElChoice, 0, menuTitle, nbEntries, bufMenuB, NULL, 'L');
     }
#  ifndef _WINDOWS
   TtaShowDialogue (menu, FALSE);
   /* waits until the user has answered to the menu and that the 
      mediator has called ChoiceMenuCallback */
   TtaWaitShowDialogue ();
#  endif /* !_WINDOWS */
}


/*----------------------------------------------------------------------
   InsertSeparatorInMenu 
   Inserts a separator in the menu Insert/Paste/Include
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InsertSeparatorInMenu (int *prevMenuInd, int *nbEntries, int *menuInd, STRING bufMenu)
#else  /* __STDC__ */
void                InsertSeparatorInMenu (prevMenuInd, nbEntries, menuInd, bufMenu)
int                *prevMenuInd;
int                *nbEntries;
int                *menuInd;
STRING              bufMenu;

#endif /* __STDC__ */
{
   *prevMenuInd = *menuInd;
   /* indicates if it's a separator */
   bufMenu[*menuInd] = TEXT('S');
   (*menuInd)++;
   bufMenu[*menuInd] = EOS;
   (*menuInd)++;
   (*nbEntries)++;
}

/*----------------------------------------------------------------------
  BuildPasteMenu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildPasteMenu (int RefMenu, STRING bufMenu, Name title, int nbEntries, CHAR_T button)
#else  /* __STDC__ */
void                BuildPasteMenu (RefMenu, bufMenu, title, nbEntries, button)
int                 RefMenu;
STRING              bufMenu;
Name                title;
int                 nbEntries;
CHAR_T                button;

#endif /* __STDC__ */
{
   CHAR_T                bufMenuB[MAX_TXT_LEN];
   STRING              src;
   STRING              dest;
   int                 k, l;

   /* adds 'B' to the beginning of each entry of the menu */
   dest = &bufMenuB[0];
   src = &bufMenu[0];
   for (k = 1; k <= nbEntries; k++)
     {
	l = ustrlen (src);
	/* don't add 'B' to the beginning of separators */
	if (*src != TEXT('S') || l != 1)
	  {
	     ustrcpy (dest, TEXT("B"));
	     dest++;
	  }
	ustrcpy (dest, src);
	dest += l + 1;
	src += l + 1;
     }
   TtaNewPopup (RefMenu, 0, title, nbEntries, bufMenuB, NULL, button);
#  ifndef _WINDOWS
   TtaShowDialogue (RefMenu, FALSE);
   /* waits for the user's answer */
   TtaWaitShowDialogue ();
#  endif /* !_WINDOWS */
}










