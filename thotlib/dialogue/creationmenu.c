/*
 *
 *  (C) COPYRIGHT INRIA, 1996-2007
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

#include "appdialogue_wx.h"
#include "paneltypes_wx.h"

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
  int            menuInd, cur, i, k, len, len_name;
  int            nbItem, typeNum;
  ThotBool       previous;
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
            len_name = strlen (pSS->SsRule->SrElem[typeNum - 1]->SrName) + 1;
            if (len_name + menuInd < MAX_MENU_LENGTH)
              {
                /* compare that name with all element names already known */
                cur = 0;
                previous = FALSE;
                for (i = 0; i < nbItem && !previous; i++)
                  {
                    len = strlen (&menuBuf[cur]) + 1;
                    previous = (pSS->SsRule->SrElem[typeNum - 1]->SrName[0] < menuBuf[cur]);
                    if (!previous)
                      {
                        k = 0;
                        while (menuBuf[cur+k] != EOS &&
                               pSS->SsRule->SrElem[typeNum - 1]->SrName[k] == menuBuf[cur+k])
                          k++;
                        previous = (pSS->SsRule->SrElem[typeNum - 1]->SrName[k] < menuBuf[cur+k]);
                      }
                    if (!previous)
                      cur += len;
                  }
                if (previous && i <= pSS->SsNRules)
                  {
                    // move the tail of the current list
                    for (k = menuInd; k >= cur; k--)
                      menuBuf[k+len_name] = menuBuf[k];
                    /* add this new element name at the current position */
                    strcpy (&menuBuf[cur], pSS->SsRule->SrElem[typeNum - 1]->SrName);
                  }
                else
                  {
                    /* add this new element name at the end */
                    strcpy (menuBuf + menuInd, pSS->SsRule->SrElem[typeNum - 1]->SrName);
                  }
                nbItem++;
                menuInd += len_name;	
              }
          }
      }
  return nbItem;
}

/*----------------------------------------------------------------------
  UpdateXMLMenu
  Just refresh the XML element list.
  return TRUE if a HTML, or SVG, or MathML element is selected
  ----------------------------------------------------------------------*/
static ThotBool UpdateXMLMenu (Document doc, View view)
{
  PtrDocument    pSelDoc, pDoc;
  PtrElement     firstSel, lastSel;
  PtrSSchema     pSS;
  char           menuBuf[MAX_MENU_LENGTH];
  int            nbItem, height, firstChar, lastChar;
  AmayaParams    p;
  int            typeNum;

  UserErrorCode = 0;
  height = 4;
  nbItem = 1;
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
          pSelDoc == pDoc && firstSel)
        {
          if (firstSel->ElTerminal && firstSel->ElParent)
            {
              firstSel = firstSel->ElParent;
              lastSel = firstSel;
            }
          pSS = firstSel->ElStructSchema;
        }
      else
        {
          // the selection is not within this document
          pSS = pDoc->DocSSchema;
          firstSel = NULL;
        }
      if (firstSel == lastSel && pSS->SsIsXml)
        {
          menuBuf[0] = EOS;
          /* build the menu only for generic XML schemas */
          nbItem = BuildElementSelector (pDoc, pSS, menuBuf);
          
          /* generate the form with two buttons Insert and Done */
          p.param1 = nbItem;
          p.param2 = (void*)menuBuf;
          if (firstSel && nbItem)
            {
              typeNum = firstSel->ElTypeNumber;
              p.param3 = (void*)(pSS->SsRule->SrElem[typeNum - 1]->SrName);
            }
          else
            p.param3 = (void*)"";
          /* the dialog reference used to call the right callback in thotlib */
          p.param4 = (void*)(NumFormElemToBeCreated);
          TtaSendDataToPanel( WXAMAYA_PANEL_XML, p );
        }
      return (nbItem == 0);
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  TtaRefreshElementMenu
  Just refresh the XML element list.
  ----------------------------------------------------------------------*/
void TtaRefreshElementMenu (Document doc, View view)
{
  UpdateXMLMenu (doc, view); 
}

/*----------------------------------------------------------------------
  TtaShowElementMenu
  Display a dialog allowing the user to choose or enter the typename of an
  element to be created.  
  ----------------------------------------------------------------------*/
void TtaShowElementMenu (Document doc, View view)
{
  if (UpdateXMLMenu (doc, view))
    TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_NO_ELEMENT), NULL);
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
        }
      else
        /* we did not create a selector, we create a capture zone having
           the nature of the element to create */
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
}










