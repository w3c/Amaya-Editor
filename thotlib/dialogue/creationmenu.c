/*
 *
 *  (C) COPYRIGHT INRIA, 1996-2004
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
#include "interface.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "modif_tv.h"
#include "select_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"

#include "applicationapi_f.h"
#include "callback_f.h"
#include "config_f.h"
#include "exceptions_f.h"
#include "schemas_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structselect_f.h"

static char     NameOfElementToBeCreated[MAX_TXT_LEN];

/*----------------------------------------------------------------------
  BuildElementSelector
  ----------------------------------------------------------------------*/
static int BuildElementSelector (PtrDocument pDoc)
{
#ifdef _GTK
  PtrDocument    pSelDoc;
  PtrElement     firstSel, lastSel;
  char           menuBuf[MAX_TXT_LEN];
  int            menuInd;
  PtrSSchema     pSS;
  int            nbItem, len, typeNum, height, firstChar, lastChar;
  NotifyElement  notifyEl;

  nbItem = 0;
  if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
			    &lastChar))
    return 0;
  if (pSelDoc != pDoc)
    return 0;
  /* make the list of all possible element types */
  menuBuf[0] = EOS;
  menuInd = 0;
  len = 0;
  pSS = firstSel->ElStructSchema;
  for (typeNum = pSS->SsRootElem + 1; typeNum <= pSS->SsNRules; typeNum++)
    if (!TypeHasException (ExcIsPlaceholder, typeNum, pSS) &&
	!TypeHasException (ExcNoCreate, typeNum, pSS) &&
	!TypeHasException (ExcHidden, typeNum, pSS) &&
	pSS->SsRule->SrElem[typeNum - 1]->SrName[0] != EOS)
      {
	/* send event ElemMenu.Pre to ask the application whether this
	   element type should appear or not */
	notifyEl.event = TteElemMenu;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = NULL;
	notifyEl.info = 0; /* not sent by undo */
	notifyEl.elementType.ElTypeNum = typeNum;
	notifyEl.elementType.ElSSchema = (SSchema) (pSS);
	notifyEl.position = 0;
	if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	  /* the application agrees */
	  {
	    len = strlen (pSS->SsRule->SrElem[typeNum - 1]->SrName) + 1;
	    if (len + menuInd < MAX_TXT_LEN)
	      {
		strcpy (menuBuf + menuInd,
			pSS->SsRule->SrElem[typeNum - 1]->SrName);
		menuInd += len;
		nbItem++;
	      }
	  }
      }

  /* build the type selector */
  if (nbItem > 0)
    {
       if (nbItem >= 4)
	 height = 4;
       else
	 height = nbItem;
       TtaNewSelector (NumSelectElemToBeCreated, NumFormElemToBeCreated,
		       TtaGetMessage (LIB, TMSG_EL_TYPE), nbItem, menuBuf,
		       height, NULL, TRUE, TRUE);
    }
  return nbItem;
#else
  return 0;
#endif /* _GTK */
}

/*----------------------------------------------------------------------
  TtaShowElementMenu
  ----------------------------------------------------------------------*/
void TtaShowElementMenu (Document doc, View view)
{
#ifdef _GTK
  char          menuBuf[MAX_TXT_LEN];
  int           nbItem;

  /* generate the form with two buttons Apply and Done */
  strcpy (menuBuf, TtaGetMessage (LIB, TMSG_APPLY));
  TtaNewSheet (NumFormElemToBeCreated, TtaGetViewFrame (doc, view),
	       TtaGetMessage(LIB, TMSG_EL_TYPE), 1, menuBuf, FALSE, 2, 'L',
	       D_DONE);
  nbItem = BuildElementSelector (LoadedDocument[doc - 1]);
  if (nbItem > 0)
    TtaShowDialogue (NumFormElemToBeCreated, TRUE);
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   CallbackElemToBeCreated
   handles the callbacks of the Element Type form.
  ----------------------------------------------------------------------*/
void CallbackElemToBeCreated (int ref, int val, char *txt)
{
  ThotBool     doit, done;
  PtrDocument  pDoc;
  PtrElement   firstSel, lastSel;
  PtrSSchema   pSS;
  char*        mappedName = NULL;
  int          firstChar, lastChar, typeNum;

  doit = FALSE;
  switch (ref)
    {
    case NumSelectElemToBeCreated:
      /* name of element to be created */
      if (txt == NULL)
	NameOfElementToBeCreated[0] = EOS;
      else
	/* save the name of the element to be created */
	strncpy (NameOfElementToBeCreated, txt, MAX_TXT_LEN);
      break;
    case NumFormElemToBeCreated:
      /* callback from the form itself */
      switch (val)
	{
	case 0:
	  /* Cancel */
	  break;
	case 1:
	  /* appliquer la nouvelle valeur */
	  doit = TRUE;
	  break;
	}
      break;
    }
  if (doit && NameOfElementToBeCreated[0] != EOS)
    {
      if (GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar,
			       &lastChar))
	{
	  done = FALSE;
	  pSS = pDoc->DocSSchema;
	  for (typeNum = pSS->SsRootElem + 1; typeNum <= pSS->SsNRules;
	       typeNum++)
	    if (!strcmp (NameOfElementToBeCreated,
			 pSS->SsRule->SrElem[typeNum - 1]->SrName))
	      {
		CreateNewElement (typeNum, pSS, pDoc, FALSE);
		done = TRUE;
	      }
	  if (!done)
	    /* this type neme is unknown in the structure schema */
	    {
	      /********* first check that the document is generic XML.
			 otherwise, do not create a new element type *******/
	      typeNum = 0;
	      AppendXmlElement (NameOfElementToBeCreated, &typeNum, pSS,
				&mappedName, pDoc);
	      if (typeNum > 0)
		{
		  CreateNewElement (typeNum, pSS, pDoc, FALSE);
		  BuildElementSelector (pDoc);
		}
	    }
	}
    }
}

/*----------------------------------------------------------------------
  BuildChoiceMenu
  ----------------------------------------------------------------------*/
void BuildChoiceMenu (char *menuBuf, char *menuTitle, int nbEntries,
		      ThotBool natureChoice)
{
#ifdef IV
   int                 menu;
   char                menuBufB[MAX_TXT_LEN];
   char               *src, *dest;
   int                 k, l, nbitem, length;

   if (natureChoice)
     {
	menu = NumFormNature;
#ifdef _GTK
	/* selector stating the nature of the element to create (or of the capture zone
	   if the configuration files don't define any natures */
	TtaNewForm (NumFormNature, 0, TtaGetMessage (LIB, TMSG_OBJECT_TYPE), TRUE, 1, 'L', D_CANCEL);
#endif /* _GTK */
	nbitem = ConfigMakeDocTypeMenu (menuBufB, &length, FALSE);
	if (nbitem > 0)
	   /* the Start Up file defines the natures */
	  {
	     /* computes the height of the menu part of the selector */
	     if (nbitem < 5)
		length = nbitem;
	     else
		length = 5;
	     /* creates the selector */
#ifdef _GTK
	     TtaNewSelector (NumSelectNatureName, NumFormNature,
			     TtaGetMessage (LIB, TMSG_OBJECT_TYPE), nbitem, menuBufB, length, NULL, TRUE, FALSE);
	     /* sets the selector on its first entry */
	     TtaSetSelector (NumSelectNatureName, 0, "");
#endif /* _GTK */
	  }
	else
	   /* we did not create a selector, we create a capture zone having
	      the nature of the element to create */
#ifdef _GTK
	   TtaNewTextForm (NumSelectNatureName, NumFormNature, TtaGetMessage (LIB, TMSG_OBJECT_TYPE), 30, 1, FALSE)
#endif /* _GTK */
	   ;
     }
   else
     {
	menu = NumMenuElChoice;
	/* adds 'B' to the beginning of each entry of the menu */
	dest = &menuBufB[0];
	src = &menuBuf[0];
	for (k = 1; k <= nbEntries; k++)
	  {
	     strcpy (dest, "B");
	     dest++;
	     l = strlen (src);
	     strcpy (dest, src);
	     dest += l + 1;
	     src += l + 1;
	  }
	TtaNewPopup (NumMenuElChoice, 0, menuTitle, nbEntries, menuBufB, NULL, 'L');
     }
#ifdef _GTK
   TtaShowDialogue (menu, FALSE);
   /* waits until the user has answered to the menu and that the 
      mediator has called ChoiceMenuCallback */
   TtaWaitShowDialogue ();
#endif /* #ifdef _GTK */

#endif /* IV */
}


/*----------------------------------------------------------------------
   InsertSeparatorInMenu 
   Inserts a separator in the menu Insert/Paste/Include
  ----------------------------------------------------------------------*/
void InsertSeparatorInMenu (int *prevMenuInd, int *nbEntries, int *menuInd,
			    char *menuBuf)
{
   *prevMenuInd = *menuInd;
   /* indicates if it's a separator */
   menuBuf[*menuInd] = 'S';
   (*menuInd)++;
   menuBuf[*menuInd] = EOS;
   (*menuInd)++;
   (*nbEntries)++;
}

/*----------------------------------------------------------------------
  BuildPasteMenu
  ----------------------------------------------------------------------*/
void BuildPasteMenu (int RefMenu, char *menuBuf, Name title,
		     int nbEntries, char button)
{
   char                menuBufB[MAX_TXT_LEN];
   char               *src, *dest;
   int                 k, l;

   /* adds 'B' to the beginning of each entry of the menu */
   dest = &menuBufB[0];
   src = &menuBuf[0];
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
   TtaNewPopup (RefMenu, 0, title, nbEntries, menuBufB, NULL, button);
#ifdef _GTK
   TtaShowDialogue (RefMenu, FALSE);
   /* waits for the user's answer */
   TtaWaitShowDialogue ();
#endif /* #ifdef _GTK */
}










