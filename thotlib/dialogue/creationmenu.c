/*
 *
 *  (C) COPYRIGHT INRIA, 1996-2005
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
#include "constmenu.h"
#include "typemedia.h"
#include "interface.h"
#include "application.h"
#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */

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
#include "thotmsg_f.h"
#ifdef _WX
  #include "appdialogue_wx.h"
  #include "paneltypes_wx.h"
#endif /* _WX */

static char     NameOfElementToBeCreated[MAX_TXT_LEN];
#define MAX_MENU_LENGTH  5000

/*----------------------------------------------------------------------
  BuildElementSelector
  Prepare a selector containing all the element types defined in the
  structure schema of the first selected element.
  Return the number of entries in the selector created.
  ----------------------------------------------------------------------*/
static int BuildElementSelector (PtrDocument pDoc, PtrSSchema pSS,
				 char menuBuf[MAX_MENU_LENGTH])
{
  int            menuInd;
  int            nbItem, len, typeNum;
  NotifyElement  notifyEl;

  nbItem = 0;
  /* make the list of all possible element types */
  menuBuf[0] = EOS;
  menuInd = 0;
  len = 0;
  for (typeNum = pSS->SsRootElem; typeNum <= pSS->SsNRules; typeNum++)
    if (!TypeHasException (ExcIsPlaceholder, typeNum, pSS) &&
	!TypeHasException (ExcNoCreate, typeNum, pSS) &&
	!TypeHasException (ExcHidden, typeNum, pSS) &&
	pSS->SsRule->SrElem[typeNum - 1]->SrName[0] != EOS &&
	pSS->SsRule->SrElem[typeNum - 1]->SrConstruct != CsNatureSchema)
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
	    if (len + menuInd < MAX_MENU_LENGTH)
	      {
		strcpy (menuBuf + menuInd,
			pSS->SsRule->SrElem[typeNum - 1]->SrName);
		menuInd += len;
		nbItem++;
	      }
	  }
      }
  return nbItem;
}

/*----------------------------------------------------------------------
  TtaShowElementMenu
  Display a dialog allowing the user to choose or enter the typename of an
  element to be created.  
  ----------------------------------------------------------------------*/
void TtaShowElementMenu (Document doc, View view)
{
  PtrDocument    pSelDoc, pDoc;
  PtrElement     firstSel = NULL, lastSel = NULL;
  PtrSSchema     pSS;
  char           menuBuf[MAX_MENU_LENGTH];
  int            nbItem, height, firstChar = 0, lastChar = 0;
  ThotBool       withTextInput = FALSE;

  UserErrorCode = 0;
  height = 4;
  if (doc < 1 || doc > MAX_DOCUMENTS)
    /* Checks the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[doc - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (!LoadedDocument[doc - 1]->DocReadOnly)
    {
      nbItem = 0;
      pDoc = LoadedDocument[doc - 1];
      if (GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar) &&
	  pSelDoc == pDoc)
	  pSS = firstSel->ElStructSchema;
      else
	pSS = pDoc->DocSSchema;

      menuBuf[0] = EOS;
      withTextInput =  (strcmp (pSS->SsName, "HTML") &&
			strcmp (pSS->SsName, "SVG") &&
			strcmp (pSS->SsName, "MathML"));
      if (strcmp (pSS->SsName, "TextFile") && withTextInput)
	/* build the menu only for generic XML schemas */
	nbItem = BuildElementSelector (pDoc, pSS, menuBuf);

      /* generate the form with two buttons Isert and Done */
#ifdef _WX 
      if (nbItem == 0)
	TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_NO_ELEMENT), NULL);
      else
	{
	  AmayaParams p;
	  int         typeNum;
	  p.param1 = (void*)nbItem;
	  p.param2 = (void*)menuBuf;
	  if (firstSel)
	    {
	      if (firstSel->ElTerminal && firstSel->ElParent)
		firstSel = firstSel->ElParent;
	      typeNum = firstSel->ElTypeNumber;
	      p.param3 = (void*)(pSS->SsRule->SrElem[typeNum - 1]->SrName);
	    }
	  else
	    p.param3 = (void*)"";
	  /* the dialog reference used to call the right callback in thotlib */
	  p.param4 = (void*)(NumFormElemToBeCreated);
	  TtaSendDataToPanel( WXAMAYA_PANEL_XML, p );
	}
#endif /* _WX */
#ifdef _WINGUI
      CreateXMLDlgWindow (NULL, nbItem, menuBuf, withTextInput);
#endif /* _WINGUI */
#ifdef _GTK
      if (nbItem > 0)
	{
	  TtaNewSheet (NumFormElemToBeCreated, TtaGetViewFrame (doc, view),
		       TtaGetMessage(LIB, TMSG_EL_TYPE),
		       1, TtaGetMessage (LIB, TMSG_INSERT), FALSE, 2, 'L',
		       D_DONE);
	  if (nbItem >= 4)
	    height = 4;
	  else
	    height = nbItem;
	  /* does not allow the user to create new type names in well
	     defined XML vocabularies */
	  TtaNewSizedSelector (NumSelectElemToBeCreated, NumFormElemToBeCreated,
			       TtaGetMessage (LIB, TMSG_EL_TYPE), nbItem, menuBuf,
			       200, height, NULL, withTextInput, TRUE);
	  TtaShowDialogue (NumFormElemToBeCreated, TRUE);
	}
      else
	TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_NO_ELEMENT), NULL);
#endif /* _GTK */
    }
}

/*----------------------------------------------------------------------
  TtaRefreshElementMenu
  Just refresh the XML element list.
  ----------------------------------------------------------------------*/
void TtaRefreshElementMenu (Document doc, View view)
{
  PtrDocument    pSelDoc, pDoc;
  PtrElement     firstSel, lastSel;
  PtrSSchema     pSS;
  char           menuBuf[MAX_MENU_LENGTH];
  int            nbItem, height, firstChar, lastChar;
  ThotBool       withTextInput = FALSE;

  UserErrorCode = 0;
  height = 4;
  if (doc < 1 || doc > MAX_DOCUMENTS)
    /* Checks the parameter document */
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[doc - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (!LoadedDocument[doc - 1]->DocReadOnly)
    {
      nbItem = 0;
      pDoc = LoadedDocument[doc - 1];
      if (GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar) &&
	  pSelDoc == pDoc)
	  pSS = firstSel->ElStructSchema;
      else
	pSS = pDoc->DocSSchema;

      menuBuf[0] = EOS;
      withTextInput =  (strcmp (pSS->SsName, "HTML") &&
			strcmp (pSS->SsName, "SVG") &&
			strcmp (pSS->SsName, "MathML"));
      if (strcmp (pSS->SsName, "TextFile") && withTextInput)
	/* build the menu only for generic XML schemas */
	nbItem = BuildElementSelector (pDoc, pSS, menuBuf);

      /* generate the form with two buttons Isert and Done */
#ifdef _WX 
      if (nbItem != 0)
	{
	  AmayaParams p;
	  int         typeNum;
	  p.param1 = (void*)nbItem;
	  p.param2 = (void*)menuBuf;
	  if (firstSel)
	    {
	      if (firstSel->ElTerminal && firstSel->ElParent)
		firstSel = firstSel->ElParent;
	      typeNum = firstSel->ElTypeNumber;
	      p.param3 = (void*)(pSS->SsRule->SrElem[typeNum - 1]->SrName);
	    }
	  else
	    p.param3 = (void*)"";
	  /* the dialog reference used to call the right callback in thotlib */
	  p.param4 = (void*)(NumFormElemToBeCreated);
	  TtaSendDataToPanel( WXAMAYA_PANEL_XML, p );
	}
#endif /* _WX */
    }
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
  char         menuBuf[MAX_MENU_LENGTH];
  int          firstChar, lastChar, typeNum, height, nbItem;

  doit = FALSE;
  switch (ref)
    {
    case NumSelectElemToBeCreated:
      /* type name of element to be created */
      if (txt == NULL)
	NameOfElementToBeCreated[0] = EOS;
      else
	/* save the type name of the element to be created */
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
	  /* Do it */
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
	  /* look for this type name in the structure schema */
	  pSS = firstSel->ElStructSchema;
	  for (typeNum = pSS->SsRootElem + 1; typeNum <= pSS->SsNRules;
	       typeNum++)
	    if (!strcmp (NameOfElementToBeCreated,
			 pSS->SsRule->SrElem[typeNum - 1]->SrName))
	      /* type name found */
	      {
		/* create an element at the current position */
		CreateNewElement (typeNum, pSS, pDoc, FALSE);
		done = TRUE;
	      }
	  if (!done)
	    /* this type name is unknown in the structure schema */
	    {
	      height = 4;
	      /* does not allow the user to create new type names in well
		 defined XML vocabularies */
	      if (strcmp (pSS->SsName, "HTML") &&
		  strcmp (pSS->SsName, "SVG") &&
		  strcmp (pSS->SsName, "MathML") &&
		  strcmp (pSS->SsName, "TextFile"))
		{
		  typeNum = 0;
		  AppendXmlElement (NameOfElementToBeCreated, &typeNum, pSS,
				    &mappedName, pDoc);
		  if (typeNum > 0)
		    {
		      CreateNewElement (typeNum, pSS, pDoc, FALSE);
		      nbItem = BuildElementSelector (pDoc, pSS, menuBuf);
#ifdef _WINGUI
             CreateXMLDlgWindow (NULL, nbItem, menuBuf, TRUE);
#endif /* _WINGUI */
#ifdef _GTK
		      if (nbItem > 0)
			{
			  if (nbItem >= 4)
			    height = 4;
			  else
			    height = nbItem;
			  /* does not allow the user to create new type names in well
			     defined XML vocabularies */
			  TtaNewSizedSelector (NumSelectElemToBeCreated,
					       NumFormElemToBeCreated,
					       TtaGetMessage (LIB, TMSG_EL_TYPE),
					       nbItem, menuBuf,
					       200, height, NULL, TRUE, TRUE);
			}
#endif /* _GTK */
		    }
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










