/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * User interface for attributes
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Windows version
 *
 */

#include "wx/wx.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "dialog.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "libmsg.h"
#include "message.h"
#include "constmenu.h"
#include "appdialogue.h"
#include "dialogapi.h"

#include "AmayaAttributePanel.h"
#include "AmayaWindow.h"
#include "AmayaPanel.h"
#include "appdialogue_wx.h"
#include "wxinclude.h"

#include "containers.h"
#include "attrmenu.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

/* flags to show the existence of the TtAttribute forms */
static ThotBool     AttrFormExists = FALSE;
static ThotBool     MandatoryAttrFormExists = FALSE;

#define LgMaxAttrText 500
static PtrDocument  DocCurrentAttr = NULL;
static int          NumCurrentAttr = 0;
static PtrSSchema   SchCurrentAttr = NULL;
static char         TextAttrValue[LgMaxAttrText];
static char         LangAttrValue[LgMaxAttrText];
static int          CurrentAttr;
/* return value of the input form */
static int          NumAttrValue;
/* main menu of attributes */
static PtrSSchema   AttrStruct[MAX_MENU * 2];
static int          AttrNumber[MAX_MENU * 2];
static ThotBool     AttrEvent[MAX_MENU * 2];
static int          AttrCorr[MAX_MENU * 2];
/* submenu of event attributes */
static int          AttrEventNumber[MAX_MENU];
static int          EventMenu[MAX_FRAME];

/* the context of the attribute menu that was built previously */
static PtrDocument PrevDoc = NULL;
static PtrSSchema PrevStructSchema = NULL;
static int PrevElTypeNumber = 0;
static ThotBool PrevElAttr = FALSE;

/* required attributs context */
static PtrAttribute PtrReqAttr;
static PtrDocument  PtrDocOfReqAttr;
/* External function that manages attribute changes */
static Proc2        AttributeChangeFunction = NULL;

#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "actions_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "content_f.h"
#include "displayview_f.h"
#include "exceptions_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "schemas_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "tree_f.h"
#include "uconvert_f.h"
#include "undo_f.h"
#include "dialogapi_f.h"
#include "thotmsg_f.h"


static ThotBool GetValidatedCurrentSelection (PtrDocument *pDoc,
    PtrElement *firstSel, PtrElement*lastSel, int *firstChar,int *lastChar);

/*----------------------------------------------------------------------
  TtaSetAttributeChangeFunction registers the attribute creation function
  ----------------------------------------------------------------------*/
void TtaSetAttributeChangeFunction (Proc2 procedure)
{
  AttributeChangeFunction = (Proc2)procedure;
}
 
/*----------------------------------------------------------------------
  InitFormLangue
  initializes a form for capturing the values of the Language attribute.
  ----------------------------------------------------------------------*/
static void InitFormLanguage (Document doc, View view, PtrElement firstSel,
                              PtrAttribute currAttr)
{
  Language            language;
  PtrAttribute        pHeritAttr;
  PtrElement          pElAttr;
  char               *ptr;
  char                languageCode[MAX_TXT_LEN];
  char                label[200];
  int                 defItem, nbItem;
  char                bufMenu[MAX_TXT_LEN];

  /* Initialize the language selector */
  languageCode[0] = EOS;
  if (currAttr && currAttr->AeAttrText)
    CopyBuffer2MBs (currAttr->AeAttrText, 0, (unsigned char*)languageCode,
                    MAX_TXT_LEN);
  if (languageCode[0] == EOS)
    {
      /* look for the inherited attribute value Language */
      strcpy (label, TtaGetMessage (LIB, TMSG_INHERITED_LANG));
      pHeritAttr = GetTypedAttrAncestor (firstSel, 1, NULL, &pElAttr);
      if (pHeritAttr && pHeritAttr->AeAttrText)
        {
          /* the attribute value is a RFC-1766 code. Convert it into */
          /* a language name */
          CopyBuffer2MBs (pHeritAttr->AeAttrText, 0,
                          (unsigned char*)languageCode, MAX_TXT_LEN);
          language = TtaGetLanguageIdFromName (languageCode);
          strcat (label, TtaGetLanguageName(language));
        }
    }
  else
    label[0] = EOS;

  ptr = GetListOfLanguages (bufMenu, MAX_TXT_LEN, languageCode, &nbItem, &defItem);
  AmayaParams p;
  p.param1 = (int)AmayaAttributeToolPanel::wxATTR_ACTION_SETUPLANG;
  p.param2 = NULL;
  p.param3 = (void*)ptr;
  p.param4 = (void*)label;
  p.param5 = (void*)bufMenu;
  p.param7 = nbItem;
  p.param8 = defItem;
  TtaSendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, p );
}

/*----------------------------------------------------------------------
  MenuValues
  builds the dialog box for capturing the values of the attribute defined
  by the pAttr rule.
  required specifies if it's a required attribute
  currAttr gives the current value of the attribute
  isRequiredDlg is true if the function must build a popup dlg for
  mandatory attributs (WX version)
  ----------------------------------------------------------------------*/
static void MenuValues (TtAttribute *pAttr, ThotBool required,
                        PtrAttribute currAttr, PtrElement pEl,
                        PtrDocument pDoc, int view,
                        ThotBool isRequiredDlg)
{
  Document          doc;
  ElementType       elType;
  char              full_title[MAX_TXT_LEN];
  char             *tmp;
  char              bufMenu[MAX_TXT_LEN];
  char             *title = NULL;
  int               i, lgmenu, val, buttons, begin, end;
  int               form, subform;

  doc = (Document) IdentDocument (pDoc);
  buttons = 0;
  strcpy (bufMenu, TtaGetMessage (LIB, TMSG_APPLY));
  buttons++;
  if (required)
    {
      form = NumMenuAttrRequired;
      if (MandatoryAttrFormExists)
        TtaDestroyDialogue (NumMenuAttrRequired);
      MandatoryAttrFormExists = TRUE;
    }
  else
    {
      form = NumMenuAttr;
      if (AttrFormExists)
        TtaDestroyDialogue (NumMenuAttr);
      AttrFormExists = TRUE;
      i = strlen (bufMenu) + 1;
      strcpy (&bufMenu[i], TtaGetMessage (LIB, TMSG_DEL_ATTR));
      buttons++;
    }
  title = (char *)TtaGetMemory (strlen (pAttr->AttrName) + 2);
  strcpy (title, pAttr->AttrName);
  switch (pAttr->AttrType)
    {
    case AtNumAttr: /* attribut a valeur numerique */
      {
        if (required)
          subform = NumMenuAttrNumNeeded;
        else
          subform = form + 1;
        if (currAttr == NULL)
          i = 0;
        else
          i = currAttr->AeAttrValue;
        if (SchCurrentAttr &&
            !strcmp (SchCurrentAttr->SsName, "HTML") &&
            (!strcmp (title, "rowspan") ||
             !strcmp (title, "colspan") ||
             !strcmp (title, "rows") ||
             !strcmp (title, "cols")))
          {
            if (i < 1)
              i = 1;
            begin = 1;
            end = 1000;
          }
        else
          {
            begin = 0;
            end = 1000;
          }
        if (!isRequiredDlg)
          {
            AmayaParams p;
            p.param1 = (int)AmayaAttributeToolPanel::wxATTR_ACTION_SETUPNUM;
            p.param2 = (void*)(required ? 0xFFFFFF : 0x000000);
            p.param8 = i;
            p.param9 = begin;
            p.param10 = end;
            TtaSendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, p );
          }
        else
          {
            full_title[0] = EOS;
            if (pEl)
              {
                // display element type + attribute name
                elType = TtaGetElementType(Element(pEl));
                strcpy (full_title, TtaGetElementTypeName (elType));
                strcat (full_title, " - ");
              }
            strcat (full_title, title);
            CreateNumDlgWX( NumMenuAttrRequired, NumMenuAttrNumNeeded,
                            TtaGetViewFrame(doc, view),
                            TtaGetMessage(LIB, TMSG_ATTR),
                            full_title,
                            i);
          }
      }
      break;
       
    case AtTextAttr: /* attribut a valeur textuelle */
      {
        if (required)
          subform = NumMenuAttrTextNeeded;
        else
          subform = form + 2;
        if (currAttr && currAttr->AeAttrText)
          {
            i = LgMaxAttrText - 2;
            i = CopyBuffer2MBs (currAttr->AeAttrText, 0,
                                (unsigned char*)TextAttrValue, i);
            /* convert to the dialogue encoding */
            tmp = (char *)TtaConvertMbsToByte ((unsigned char *)TextAttrValue,
                                               TtaGetDefaultCharset ());
            strcpy (TextAttrValue, tmp);
            TtaFreeMemory (tmp);
          }
        else
          TextAttrValue[0] = EOS;
        if (!isRequiredDlg)
          {
            AmayaParams p;
            p.param1 = (int)AmayaAttributeToolPanel::wxATTR_ACTION_SETUPTEXT;
            p.param2 = (void*)(required ? 0xFFFFFF : 0x000000);
            p.param3 = (void*)TextAttrValue;
            TtaSendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, p );
          }
        else
          {
            full_title[0] = EOS;
            if (pEl)
              {
                // display element type + attribute name
                elType = TtaGetElementType(Element(pEl));
                strcpy (full_title, TtaGetElementTypeName (elType));
                strcat (full_title, " - ");
              }
            strcat (full_title, title);
            /* create a dialogue to ask the mandatory attribut value (text type) */
            CreateTextDlgWX ( NumMenuAttrRequired, NumMenuAttrTextNeeded, /* references */
                              TtaGetViewFrame (doc, view), /* parent */
                              TtaGetMessage(LIB, TMSG_ATTR), /* title */
                              full_title, /* label : attribut name */
                              TextAttrValue /* initial value */ );
          }

      }
      break;
       
    case AtEnumAttr: /* attribut a valeurs enumerees */
      {
        if (required)
          subform = NumMenuAttrEnumNeeded;
        else
          subform = form + 3;
        /* cree un menu de toutes les valeurs possibles de l'attribut */
        lgmenu = 0;
        val = 0;
        /* boucle sur les valeurs possibles de l'attribut */
        while (val < pAttr->AttrNEnumValues)
          {
            i = strlen (pAttr->AttrEnumValue[val]) + 1; /* for EOS */
            if (lgmenu + i < MAX_TXT_LEN)
              {
                strcpy (&bufMenu[lgmenu], pAttr->AttrEnumValue[val]);
                val++;
              } 
	     
            lgmenu += i;
          }
        /* current value */
        i = 0;
        if (currAttr != NULL && currAttr->AeAttrValue > 0)
          i = currAttr->AeAttrValue - 1;
        if (PtrReqAttr)
          PtrReqAttr->AeAttrValue = i + 1;

        if (!isRequiredDlg)
          {
            AmayaParams p;
            p.param1 = (int)AmayaAttributeToolPanel::wxATTR_ACTION_SETUPENUM;
            p.param2 = (void*)(required ? 0xFFFFFF : 0x000000);
            p.param3 = (void*)bufMenu; /* list data */
            p.param7 = i; /* selected item */
            p.param8 = val; /* nb items */
            TtaSendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, p );
          }
        else
          {
            full_title[0] = EOS;
            if (pEl)
              {
                // display element type + attribute name
                elType = TtaGetElementType(Element(pEl));
                strcpy (full_title, TtaGetElementTypeName (elType));
                strcat (full_title, " - ");
              }
            strcat (full_title, title);
            CreateEnumListDlgWX( NumMenuAttrRequired, NumMenuAttrEnumNeeded,
                                 TtaGetViewFrame(doc, view),
                                 TtaGetMessage(LIB, TMSG_ATTR),
                                 full_title, val, bufMenu, i);
          }
      }
      break;
    default: break;
    }

  if (title != NULL)
    TtaFreeMemory (title);
}

/*----------------------------------------------------------------------
  CallbackReqAttrMenu
  handles the callback of the menu which captures the required attributes.
  ----------------------------------------------------------------------*/
void CallbackReqAttrMenu (int ref, int val, char *txt)
{
  int      length;

  switch (ref)
    {
    case NumMenuAttrRequired:
      /* retour de la feuille de dialogue elle-meme */
      /* on detruit cette feuille de dialogue sauf si c'est */
      /* un abandon */
      if (PtrReqAttr == NULL)
        {
          TtaDestroyDialogue (NumMenuAttrRequired);
          MandatoryAttrFormExists = FALSE;
          if (AttrFormExists)
            {
              ThotCallback (NumMenuAttr, INTEGER_DATA, (char *)1);
              AttrFormExists = FALSE;
            }
        }
      if (val != 0)
        {
          TtaDestroyDialogue (NumMenuAttrRequired);
          MandatoryAttrFormExists = FALSE;
          PtrReqAttr = NULL;
          PtrDocOfReqAttr = NULL;
        }
      /* on ne fait rien d'autre : tout a ete fait par les cas */
      /* suivants */
      break;
    case NumMenuAttrNumNeeded:
      /* zone de saisie de la valeur numerique de l'attribut */
      if (PtrReqAttr == NULL)
        NumAttrValue = val;
      else if (val >= -MAX_INT_ATTR_VAL || val <= MAX_INT_ATTR_VAL)
        PtrReqAttr->AeAttrValue = val;
      break;
    case NumMenuAttrTextNeeded:
      /* zone de saisie du texte de l'attribut */
      if (PtrReqAttr == NULL)
        {
          strncpy (TextAttrValue, txt, LgMaxAttrText);
        }
      else
        {
          if (PtrReqAttr->AeAttrText == NULL)
            GetTextBuffer (&PtrReqAttr->AeAttrText);
          else
            ClearText (PtrReqAttr->AeAttrText);
          CopyStringToBuffer ((unsigned char*)txt, PtrReqAttr->AeAttrText, &length);
        }
      break;
    case NumMenuAttrEnumNeeded:
      /* menu des valeurs d'un attribut a valeurs enumerees */
      val++;
      if (PtrReqAttr == NULL)
        NumAttrValue = val;
      else
        PtrReqAttr->AeAttrValue = val;
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
  BuildReqAttrMenu
  builds the form for capturing the value of the required
  attribute as defined by the pRuleAttr rule.
  ----------------------------------------------------------------------*/
void BuildReqAttrMenu (PtrAttribute pAttr, PtrDocument pDoc, PtrElement pEl)
{
  PtrTtAttribute        pRuleAttr;

  PtrReqAttr = pAttr;
  PtrDocOfReqAttr = pDoc;
  pRuleAttr = pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1];
  /* toujours lie a la vue 1 du document */
  MenuValues (pRuleAttr, TRUE, NULL, pEl, pDoc, 1, TRUE);
  TtaShowDialogue (NumMenuAttrRequired, FALSE, TRUE);
  TtaWaitShowDialogue ();
}

/*----------------------------------------------------------------------
  TteItemMenuAttrExtended
  sends the AttrMenu.Pre message which indicates that the editor
  is going to add to the Attributes menu an item for the creation
  of an attribute of type (pSS, att) for the pEl element. It 
  returns the answer from the application.
  ----------------------------------------------------------------------*/
static PtrAttrListElem TteItemMenuAttrExtended (PtrSSchema pSS, int att,
                          PtrElement pEl, PtrDocument pDoc)
{
  NotifyAttribute     notifyAttr;
  PtrAttrListElem     elem = NULL;

  notifyAttr.event = TteAttrMenu;
  notifyAttr.document = (Document) IdentDocument (pDoc);
  notifyAttr.element = (Element) pEl;
  notifyAttr.attribute = NULL;
  notifyAttr.info = 0; /* not sent by undo */
  notifyAttr.attributeType.AttrSSchema = (SSchema) pSS;
  notifyAttr.attributeType.AttrTypeNum = att;
  
  notifyAttr.restr.RestrFlags = attr_normal;
  notifyAttr.restr.RestrType  = restr_content_no_restr;
  notifyAttr.restr.RestrDefVal = NULL;
  notifyAttr.restr.RestrEnumVal = NULL;

  if (!CallEventAttribute (&notifyAttr, TRUE))
    {
      elem = (PtrAttrListElem)TtaGetMemory(sizeof(AttrListElem));
      elem->pSS      = pSS;
      elem->num      = att;
      elem->restr.RestrFlags   = notifyAttr.restr.RestrFlags;
      elem->restr.RestrType    = notifyAttr.restr.RestrType;
      elem->restr.RestrDefVal  = notifyAttr.restr.RestrDefVal;
      elem->restr.RestrEnumVal = notifyAttr.restr.RestrEnumVal;
    }

  return elem;
}


/*----------------------------------------------------------------------
  CompareAttrList
  Sorting helper for a list of PtrAttrListElem elements.
  ----------------------------------------------------------------------*/
static int CompareAttrList(PtrAttrListElem elem1, PtrAttrListElem elem2)
{
  if (elem1 && elem2)
    {
      if (elem1->categ!=elem2->categ)
        {
          return elem1->categ-elem2->categ;
        }
      else
        {
          return strcmp(AttrListElem_GetName(elem1), AttrListElem_GetName(elem2));
        }
    }
  return 0;
}


/*----------------------------------------------------------------------
  BuildAttrList
  builds an attribute list with types and values.
  ----------------------------------------------------------------------*/
static DLList BuildAttrList(PtrDocument pDoc, PtrElement firstSel)
{
  DLList          list = DLList_Create();
  ForwardIterator iter = DLList_GetForwardIterator(list);
  DLListNode      node;
  PtrSSchema      pSS, pSchExt;
  PtrSRule        pRe1;
  int             num, nbOfEntries = 0;
  PtrAttrListElem attrElem;
  PtrAttribute    pAttr;
  AttributeType   type;
  ThotBool        isNew;
  
  list->destroyElement = (Container_DestroyElementFunction)TtaFreeMemory;
  
  /* cherche les attributs globaux definis dans le schema de structure */
  /* du premier element selectionne' et dans les extrensions de ce schema*/
  pSS = firstSel->ElStructSchema;/* schema de struct de l'element courant*/
  while (pSS)
    {
      for (num=1; num<=pSS->SsNAttributes; num++)
        {
          /* skip local attributes */
          if (pSS->SsAttribute->TtAttr[num-1]->AttrGlobal &&
              /* and invisible attributes */
              !AttrHasException (ExcInvisible, num, pSS))
            /* skip the attribute Language execpt the first time */
            if (nbOfEntries == 0 || num != 1)
              {
                attrElem = TteItemMenuAttrExtended (pSS, num, firstSel, pDoc);
                if (attrElem)
                  {
                    /* conserve le schema de structure et le numero */
                    /* d'attribut de cette nouvelle entree du menu */
                    if (AttrHasException (ExcEventAttr, num, pSS))
                      attrElem->categ = attr_event;
                    else
                      attrElem->categ = attr_global;
                    type.AttrSSchema = (SSchema) pSS;
                    type.AttrTypeNum = num;
                    attrElem->val   = (PtrAttribute)TtaGetAttribute((Element)firstSel, type);
                    DLList_Append(list, attrElem);
                  }
              }
        }
      /* next extension schema */
      pSS = pSS->SsNextExtens;
    }


  /* cherche les attributs locaux du premier element selectionne' */
  pSS = firstSel->ElStructSchema;
  if (pSS != NULL)
    {
      pRe1 = pSS->SsRule->SrElem[firstSel->ElTypeNumber - 1];
      pSchExt = pDoc->DocSSchema;
      do
        {
          if (pRe1 != NULL)
            /* prend les attributs locaux definis dans cette regle */
            for (num = 0; num < pRe1->SrNLocalAttrs; num++)
              if (!pSS->SsAttribute->TtAttr[pRe1->SrLocalAttr->Num[num] - 1]->AttrGlobal)
                if (!AttrHasException (ExcInvisible,
                                       pRe1->SrLocalAttr->Num[num], pSS))
                  {
                    attrElem = TteItemMenuAttrExtended (pSS, pRe1->SrLocalAttr->Num[num],
                                     firstSel, pDoc);
                    if (attrElem)
                      {
                        /* conserve le schema de structure et le numero */
                        /* d'attribut de cette nouvelle entree du menu */
                        if (pRe1->SrRequiredAttr->Bln[num])
                          attrElem->restr.RestrFlags |= attr_mandatory;
                        if (AttrHasException (ExcEventAttr, 
                            pRe1->SrLocalAttr->Num[num], pSS))
                          attrElem->categ = attr_event;
                        else
                          attrElem->categ = attr_local;
                        type.AttrSSchema = (SSchema) pSS;
                        type.AttrTypeNum = attrElem->num;
                        attrElem->val   = (PtrAttribute)TtaGetAttribute((Element)firstSel, type);
                        DLList_Append(list, attrElem);
                      }
                  }
          /* passe a l'extension suivante du schema du document */
          pSchExt = pSchExt->SsNextExtens;
          /* cherche dans cette extension de schema la regle d'extension */
          /* pour le premier element selectionne' */
          if (pSchExt)
            {
              pSS = pSchExt;
              pRe1 = ExtensionRule (firstSel->ElStructSchema,
                                    firstSel->ElTypeNumber, pSchExt);
            }
        }
      while (pSchExt);
    }
  /* la table contient tous les attributs applicables aux elements */
  /* selectionnes */


  /* add attributes attached to the element that are not yet in
     the table */
  pAttr = firstSel->ElFirstAttr;
  while (pAttr)
    {
      isNew = TRUE;
      ITERATOR_FOREACH(iter, DLListNode, node)
      {
        attrElem = (PtrAttrListElem)node->elem;
        if (attrElem->num==pAttr->AeAttrNum &&
            attrElem->pSS==pAttr->AeAttrSSchema)
          {
            isNew = FALSE;
            break;
          }
      }
      if (isNew)
        {
          if (!AttrHasException (ExcInvisible, pAttr->AeAttrNum,
                                 pAttr->AeAttrSSchema))
            {
              attrElem = TteItemMenuAttrExtended (pAttr->AeAttrSSchema, pAttr->AeAttrNum,
                               firstSel, pDoc);
              if (attrElem)
                {
                  /* conserve le schema de structure et le numero */
                  /* d'attribut de cette nouvelle entree du menu */
                  if (AttrHasException (ExcEventAttr, 
                      pAttr->AeAttrNum, pAttr->AeAttrSSchema))
                    attrElem->categ = attr_event;
                  else
                    attrElem->categ = attr_other;
                  type.AttrSSchema = (SSchema) attrElem->pSS;
                  type.AttrTypeNum = attrElem->num;
                  attrElem->val   = (PtrAttribute)TtaGetAttribute((Element)firstSel, type);
                  DLList_Append(list, attrElem);
                }
            }
        }
      pAttr = pAttr->AeNext;
    }

  DLList_Sort(list, (Container_CompareFunction) CompareAttrList);
  
  TtaFreeMemory(iter);
  return list;
}


/*----------------------------------------------------------------------
  UpdateAttrMenu                                                       
  Updates the Attributes menu of all open frames belonging to document
  pDoc.
  The parameter force is TRUE when the update is mandatory
  ----------------------------------------------------------------------*/
void UpdateAttrMenu (PtrDocument pDoc, ThotBool force)
{
  ThotBool            selectionOK;
  PtrElement          firstSel, lastSel, parent;
  int                 firstChar, lastChar;
  int                 nbItemAttr, nbEvent;
  DLList              list;
  AmayaParams         params;

  /* Compose le menu des attributs */
  if (!pDoc)
    return;
  if (pDoc == SelectedDocument && !pDoc->DocReadOnly &&
      pDoc->DocTypeName &&
      strcmp (pDoc->DocTypeName, "source") &&
      strcmp (pDoc->DocTypeName, "css") &&
      strcmp (pDoc->DocTypeName, "text"))
    {
      selectionOK = GetCurrentSelection (&pDoc, &firstSel, &lastSel,
                                         &firstChar, &lastChar);
      if (selectionOK && firstSel == lastSel && firstSel->ElParent == NULL)
        /* the Document element is selected. It can't accept any attribute */
        selectionOK = FALSE;
      if (!selectionOK)
        {
          nbItemAttr = 0;
          nbEvent = 0;
        }
      else
        {
          /* when the PICTURE element of an IMG is selected display
             attributes of the parent element */
          parent = firstSel->ElParent;
          if (firstSel->ElTerminal && firstSel->ElLeafType == LtPicture && parent &&
              TypeHasException (ExcIsImg, parent->ElTypeNumber, parent->ElStructSchema))
            firstSel = parent;
          else if (firstSel->ElTerminal &&
                   (firstSel->ElLeafType == LtPolyLine ||
                    firstSel->ElLeafType == LtPath ||
                    firstSel->ElLeafType == LtGraphics))
            firstSel = parent;
          else
            // check if columns are selected
            MoveSelectionToCol (&firstSel, &lastSel);

          if (force || pDoc != PrevDoc ||
              firstSel->ElStructSchema != PrevStructSchema ||
              firstSel->ElTypeNumber != PrevElTypeNumber ||
              firstSel->ElFirstAttr != NULL ||
              PrevElAttr)
            {
              list = BuildAttrList(pDoc, firstSel);
              params.param1 = (int)AmayaAttributeToolPanel::wxATTR_ACTION_LISTUPDATE;
              params.param2 = (void*)list;
              params.param5 = (void*)firstSel;
              params.param6 = (void*)lastSel;
              params.param7 = firstChar;
              params.param8 = lastChar;
              
              TtaSendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, params );
              return;
            }
          else
            return;
        }
    }
  else
    {
      nbItemAttr = 0;
      nbEvent = 0;
      PrevDoc = NULL;
      PrevStructSchema = NULL;
      PrevElTypeNumber = 0;
      PrevElAttr = FALSE;
      params.param1 = (int)AmayaAttributeToolPanel::wxATTR_ACTION_LISTUPDATE;
      params.param2 = NULL;
      params.param5 = NULL;
      params.param6 = NULL;
      params.param7 = 0;
      params.param8 = 0;
      TtaSendDataToPanel( WXAMAYA_PANEL_ATTRIBUTE, params );
    }
}

/*----------------------------------------------------------------------
  SimpleUpdateAttrMenu                                                       
  Updates the Attributes menu of all open frames belonging to document
  pDoc if necessary
  ----------------------------------------------------------------------*/
static void SimpleUpdateAttrMenu (PtrDocument pDoc)
{
  UpdateAttrMenu (pDoc, FALSE);
}

/*----------------------------------------------------------------------
  TtaUpdateAttrMenu                                                       
  Updates the Attributes tool.
  ----------------------------------------------------------------------*/
void TtaUpdateAttrMenu (Document document)
{
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    UpdateAttrMenu (LoadedDocument[document - 1], FALSE);
}

/*----------------------------------------------------------------------
  AttachAttrToElem attachs the attribute to the element
  ----------------------------------------------------------------------*/
static void AttachAttrToElem (PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc)
{
  Language            lang;
  PtrAttribute        pAttrAsc;
  PtrElement          pElAttr;
  char                text[100];

  /* On ne traite pas les marques de page */
  if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
    {
      if (pAttr->AeAttrNum == 1)
        /* c'est l'attribut langue */
        {
          /* change la langue de toutes les feuilles de texte du sous-arbre */
          /* de l'element */
          if (pAttr->AeAttrText)
            {
              CopyBuffer2MBs (pAttr->AeAttrText, 0, (unsigned char*)text, 99);
              lang = TtaGetLanguageIdFromName (text);
            }
          else
            /* c'est une suppression de l'attribut Langue */
            {
              lang = TtaGetDefaultLanguage ();		/* langue par defaut */
              /* on cherche si un ascendant porte l'attribut Langue */
              if (pEl->ElParent != NULL)
                pAttrAsc = GetTypedAttrAncestor (pEl->ElParent, 1, NULL,
                                                 &pElAttr);
              else
                pAttrAsc = GetTypedAttrAncestor (pEl->ElParent, 1, NULL,
                                                 &pElAttr);
	      
              if (pAttrAsc && pAttrAsc->AeAttrText)
                {
                  /* un ascendant definit la langue, on prend cette langue */
                  CopyBuffer2MBs (pAttrAsc->AeAttrText, 0, (unsigned char*)text, 99);
                  lang = TtaGetLanguageIdFromName (text);
                }
            }
          ChangeLanguage (pDoc, pEl, lang, FALSE);
        }
      
      /* met la nouvelle valeur de l'attribut dans l'element et */
      /* applique les regles de presentation de l'attribut a l'element */
      AttachAttrWithValue (pEl, pDoc, pAttr, TRUE);
    }
}


/*----------------------------------------------------------------------
  AttachAttrToRange applique l'attribut pAttr a une partie de document
  ----------------------------------------------------------------------*/
static void AttachAttrToRange (PtrAttribute pAttr, int lastChar, 
                               int firstChar, PtrElement pLastSel, 
                               PtrElement pFirstSel, PtrDocument pDoc,
                               ThotBool reDisplay)
{
  PtrElement          pEl;
  ThotBool            isDone = FALSE;

  /* eteint d'abord la selection */
  TtaClearViewSelections ();
  /* Coupe les elements du debut et de la fin de la selection s'ils */
  /* sont partiellement selectionnes */
  IsolateSelection (pDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar, TRUE);
  /* start an operation sequence in editing history */
  OpenHistorySequence (pDoc, pFirstSel, pLastSel, NULL, firstChar, lastChar);
  /* parcourt les elements selectionnes */
  pEl = pFirstSel;
  while (pEl)
    {
      isDone = TRUE;
      if (pEl->ElStructSchema &&
          pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1] &&
          pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct == CsChoice)
        {
          /* cannot attach the attribute to that element */
          if (pEl->ElParent &&
              pEl->ElNext == NULL && pEl->ElPrevious == NULL)
            AttachAttrToElem (pAttr, pEl->ElParent, pDoc);
        }
      else
        AttachAttrToElem (pAttr, pEl, pDoc);
      /* cherche l'element a traiter ensuite */
      pEl = NextInSelection (pEl, pLastSel);
    }
  /* close the editing sequence */
  CloseHistorySequence (pDoc);
  /* parcourt a nouveau les elements selectionnes pour fusionner les */
  /* elements voisins de meme type ayant les memes attributs, reaffiche */
  /* toutes les vues et retablit la selection */
  if (reDisplay)
    {
      GetCurrentSelection (&pDoc, &pFirstSel, &pLastSel, &firstChar,
                           &lastChar);
      if (pFirstSel && pFirstSel->ElStructSchema)
        SelectRange (pDoc, pFirstSel, pLastSel, firstChar, lastChar);
    }
  if (!isDone)
    // all the selection is read only
    TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_EL_RO);
}

/*----------------------------------------------------------------------
  CallbackValAttrMenu
  handles the callback of the form which captures the attribute values.
  Applies to the selected elements the attributes chosen by the user.
  ref: reference to the dialogue element that called this function
  valmenu: selected or captured value in this dialogue element
  valtext: pointer to the captured text in this dialogue element
  ----------------------------------------------------------------------*/
void CallbackValAttrMenu (int ref, int valmenu, char *valtext)
{
  PtrDocument         pDoc;
  PtrElement          firstSel, lastSel, parent;
  PtrAttribute        pAttrNew;
  DisplayMode         dispMode = DeferredDisplay;
  Document            doc = 0;
  PtrSRule            pSRule;
  char               *tmp;
  int                 firstChar, lastChar, att;
  int                 act;
  ThotBool            lock = TRUE;
  ThotBool            isID = FALSE, isACCESS = FALSE;
  ThotBool            isCLASS = FALSE, isSpan = FALSE;

  act = 0; /* apply by default */
  switch (ref)
    {
    case NumMenuAttrNumber:
      /* valeur d'un attribut numerique */
      NumAttrValue = valmenu;
      act = 0;
      break;
    case NumMenuAttrText:
      /* valeur d'un attribut textuel */
      strncpy (TextAttrValue, valtext, LgMaxAttrText);
      act = 0;
      break;
    case NumMenuAttrEnum:
      /* numero de la valeur d'un attribut enumere' */
      NumAttrValue = valmenu + 1;
      break;
    case NumMenuAttr:
      /* retour de la feuille de dialogue elle-meme */
      if (valmenu == 0)
        {
          /* on detruit la feuille de dialogue */
          TtaDestroyDialogue (NumMenuAttr);
          AttrFormExists = FALSE;
        }
      tmp = SchCurrentAttr->SsAttribute->TtAttr[NumCurrentAttr - 1]->AttrName;
      isACCESS = (!strcmp (tmp, "accesskey") &&
                  !strcmp (SchCurrentAttr->SsName, "HTML"));
      isID = (!strcmp (tmp, "id") ||
              !strcmp (tmp, "xml:id") ||
              (!strcmp (tmp, "name") &&
               !strcmp (SchCurrentAttr->SsName, "HTML")));
      isCLASS = !strcmp (tmp, "class");
      isSpan = ((isID || isCLASS ||
                 !strcmp (tmp, "style") || !strcmp (tmp, "lang")) &&
                  !strcmp (SchCurrentAttr->SsName, "HTML"));
      if (SchCurrentAttr->SsAttribute->TtAttr[NumCurrentAttr - 1]->AttrType == AtTextAttr && TextAttrValue[0] == EOS)
        return;
      act = valmenu;
      break;
    }

  /* demande quelle est la selection courante */
  if (!GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    {
      /* no selection. quit */
      TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_NO_SELECT);
      return;
    }

  // check if columns are selected
  MoveSelectionToCol (&firstSel, &lastSel);

  /* when the PICTURE element of an IMG is selected display
     attributes of the parent element */
  parent = firstSel->ElParent;
  if (firstSel->ElTerminal && firstSel->ElLeafType == LtPicture && parent &&
      TypeHasException (ExcIsImg, parent->ElTypeNumber, parent->ElStructSchema))
    firstSel = parent;
  if (act == 2)
    /* the user wants to delete the attribute. Is it a mandatory attribute
       for the selected element ? */
    if (SchCurrentAttr == firstSel->ElStructSchema)
      {
        /* get the structure rule defining the selected element */
        pSRule = firstSel->ElStructSchema->SsRule->SrElem[firstSel->ElTypeNumber - 1];
        /* look for the attribute in the list of allowed attributes for this
           element type */
        for (att = 0; att < pSRule->SrNLocalAttrs; att++)
          if (pSRule->SrLocalAttr->Num[att] == NumCurrentAttr)
            /* this is the attribute of interest */
            {
              if (pSRule->SrRequiredAttr->Bln[att])
                /* mandatory attribute */
                act = 0;
              /* stop */
              att = pSRule->SrNLocalAttrs;
            }
      }

  if (act > 0)
    {
      /* ce n'est pas une simple fermeture de la feuille de dialogue */
      {
        /* on ne fait rien si le document ou` se trouve la selection
           n'utilise pas le schema de structure qui definit l'attribut */
        if (SchCurrentAttr &&
            GetSSchemaForDoc (SchCurrentAttr->SsName, pDoc))
          {
            /* lock tables formatting */
            TtaGiveTableFormattingLock (&lock);
            if (!lock)
              {
                doc = IdentDocument (pDoc);
                dispMode = TtaGetDisplayMode (doc);
                if (dispMode == DisplayImmediately)
                  TtaSetDisplayMode (doc, DeferredDisplay);
                /* table formatting is not locked, lock it now */
                TtaLockTableFormatting ();
              }

            GetAttribute (&pAttrNew);
            if (NumCurrentAttr == 1)
              pAttrNew->AeAttrSSchema = firstSel->ElStructSchema;
            else
              pAttrNew->AeAttrSSchema = SchCurrentAttr;
            pAttrNew->AeAttrNum = NumCurrentAttr;
            pAttrNew->AeDefAttr = FALSE;
            pAttrNew->AeAttrType = SchCurrentAttr->SsAttribute->TtAttr[NumCurrentAttr - 1]->AttrType;

            switch (pAttrNew->AeAttrType)
              {
              case AtNumAttr:
                if (act == 2)
                  /* Supprimer l'attribut */
                  pAttrNew->AeAttrValue = MAX_INT_ATTR_VAL + 1;
                else
                  /* la valeur saisie devient la valeur courante */
                  pAttrNew->AeAttrValue = NumAttrValue;
                /* applique les attributs a la partie selectionnee */
                AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
                                   firstSel, pDoc, TRUE);
                break;
		    
              case AtTextAttr:
                if (act == 2)
                  /* suppression de l'attribut */
                  pAttrNew->AeAttrText = NULL;
                else
                  {
                    /* la valeur saisie devient la valeur courante */
                    if (pAttrNew->AeAttrText == NULL)
                      GetTextBuffer (&(pAttrNew->AeAttrText));
                    else
                      ClearText (pAttrNew->AeAttrText);
                    /* special treatment for accesskey attributes */
                    if (isACCESS)
                      /* only one character is allowed */
                      TextAttrValue[1] = EOS;

                    tmp = (char *)TtaConvertByteToMbs ((unsigned char *)TextAttrValue, TtaGetDefaultCharset ());
                    CopyMBs2Buffer ((unsigned char *)tmp, pAttrNew->AeAttrText, 0, strlen (tmp));
                    TtaFreeMemory (tmp);
                    /* special treatments for id, name and accesskey attributes */
                    tmp = SchCurrentAttr->SsAttribute->TtAttr[NumCurrentAttr - 1]->AttrName;
                    if (isID)
                      TtaIsValidID ((Attribute)pAttrNew, TRUE);
                  }

                if (isSpan && firstSel != lastSel &&
                    AttributeChangeFunction)
                  {
                    // should generate a span
                    (*(Proc2)AttributeChangeFunction) ((void *)pAttrNew->AeAttrNum,
                                                       (void *)TextAttrValue);
                  }
                else
                  /* apply the attribute to each sub-element */
                  AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
                                   firstSel, pDoc, TRUE);
                if (isCLASS)
                  TtaExecuteMenuAction ("ApplyClass", doc, 1, TRUE);
                break;

              case AtReferenceAttr:
                break;

              case AtEnumAttr:
                if (act == 2)
                  /* suppression de l'attribut */
                  pAttrNew->AeAttrValue = 0;
                else
                  {
                    /* la valeur choisie devient la valeur courante */
                    pAttrNew->AeAttrValue = NumAttrValue;
                  }
                /* applique les attributs a la partie selectionnee */
                AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
                                   firstSel, pDoc, TRUE);
                break;

              default:
                break;
              }
            if (!lock)
              {
                /* unlock table formatting */
                TtaUnlockTableFormatting ();
                if (dispMode == DisplayImmediately)
                  TtaSetDisplayMode (doc, DisplayImmediately);
              }
            UpdateAttrMenu (pDoc, FALSE);
            DeleteAttribute (NULL, pAttrNew);
          }
      }
    }
}

/*----------------------------------------------------------------------
  SetAttrValueToRange
  Set a new value for the specified attr for the currently selected range.
  \param value char* is the attr is text and int if num or enum.
  ----------------------------------------------------------------------*/
void SetAttrValueToRange (PtrAttrListElem elem, intptr_t value)
{
  PtrDocument         pDoc;
  PtrElement          firstSel, lastSel;
  PtrAttribute        pAttrNew;
  DisplayMode         dispMode = DeferredDisplay;
  Document            doc = 0;
  char               *tmp;
  int                 firstChar, lastChar;
  ThotBool            lock = TRUE, isColumn;
  ThotBool            isID = FALSE, isACCESS = FALSE;
  ThotBool            isCLASS = FALSE, isSpan = FALSE;

  if (!elem || !elem->pSS || elem->num<=0)
    return;

  /* demande quelle est la selection courante */
  if (!GetValidatedCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    {
      /* no selection. quit */
      TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_NO_SELECT);
      return;
    }

  // check if columns are selected
  isColumn = MoveSelectionToCol (&firstSel, &lastSel);

  tmp = AttrListElem_GetName(elem);
  isACCESS = (!strcmp (tmp, "accesskey") &&
              !strcmp (elem->pSS->SsName, "HTML"));
  //if (!strcmp (tmp, "name") && !strcmp (elem->pSS->SsName, "HTML"))
  isID = (!strcmp (tmp, "id") ||
          !strcmp (tmp, "xml:id"));
  isCLASS = !strcmp (tmp, "class");
  isSpan = ((isID || isCLASS ||
             !strcmp (tmp, "style") || !strcmp (tmp, "lang")) &&
              !strcmp (elem->pSS->SsName, "HTML"));
  
  /* on ne fait rien si le document ou` se trouve la selection
     n'utilise pas le schema de structure qui definit l'attribut */
  if (GetSSchemaForDoc (elem->pSS->SsName, pDoc))
    {
      /* lock tables formatting */
      TtaGiveTableFormattingLock (&lock);
      if (!lock)
        {
          doc = IdentDocument (pDoc);
          dispMode = TtaGetDisplayMode (doc);
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DeferredDisplay);
          /* table formatting is not locked, lock it now */
          TtaLockTableFormatting ();
        }

      GetAttribute (&pAttrNew);
      if (elem->num == 1)
        pAttrNew->AeAttrSSchema = firstSel->ElStructSchema;
      else
        pAttrNew->AeAttrSSchema = elem->pSS;
      pAttrNew->AeAttrNum = elem->num;
      pAttrNew->AeDefAttr = FALSE;
      pAttrNew->AeAttrType = elem->pSS->SsAttribute->TtAttr[elem->num - 1]->AttrType;

      switch (pAttrNew->AeAttrType)
        {
        case AtEnumAttr:
        case AtNumAttr:
          pAttrNew->AeAttrValue = (intptr_t)value;
          /* applique les attributs a la partie selectionnee */
          AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
                             firstSel, pDoc, TRUE);
          break;
  
        case AtTextAttr:
            /* la valeur saisie devient la valeur courante */
            if (pAttrNew->AeAttrText == NULL)
              GetTextBuffer (&(pAttrNew->AeAttrText));
            else
              ClearText (pAttrNew->AeAttrText);
            /* special treatment for accesskey attributes */

            tmp = (char *)TtaConvertByteToMbs ((unsigned char *)value, TtaGetDefaultCharset ());
            CopyMBs2Buffer ((unsigned char *)tmp, pAttrNew->AeAttrText, 0, strlen (tmp));
            TtaFreeMemory (tmp);
            /* special treatments for id, name and accesskey attributes */
            tmp = elem->pSS->SsAttribute->TtAttr[elem->num - 1]->AttrName;
            if (isID)
              TtaIsValidID ((Attribute)pAttrNew, TRUE);

          if (isSpan && firstSel != lastSel && AttributeChangeFunction)
            {
              // should generate a span
              (*(Proc2)AttributeChangeFunction) ((void *)pAttrNew->AeAttrNum,
                                                 (void *)value);
            }
          else
              /* apply the attribute to each sub-element */
              AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
                               firstSel, pDoc, TRUE);
          if (isCLASS)
            TtaExecuteMenuAction ("ApplyClass", doc, 1, TRUE);
          break;

        case AtReferenceAttr:
          break;
        default:
          break;
        }

      if (!lock)
        {
          /* unlock table formatting */
          TtaUnlockTableFormatting ();
          if (dispMode == DisplayImmediately)
            TtaSetDisplayMode (doc, DisplayImmediately);
        }

      UpdateAttrMenu (pDoc, FALSE);
      DeleteAttribute (NULL, pAttrNew);
    }
}

/*----------------------------------------------------------------------
  GetValidatedCurrentSelection 
  Get the current selection but validate it with selecting the IMG element if
  element is a picture.
  ----------------------------------------------------------------------*/
static ThotBool GetValidatedCurrentSelection (PtrDocument *pDoc, PtrElement *firstSel,
                                              PtrElement*lastSel, int *firstChar,int *lastChar)
{
  PtrElement parent;
  
  if (!GetCurrentSelection (pDoc, firstSel, lastSel, firstChar, lastChar))
    return FALSE;
  else
    {
      /* when the PICTURE element of an IMG is selected display
         attributes of the parent element */
      parent = (*firstSel)->ElParent;
      if ((*firstSel)->ElTerminal && (*firstSel)->ElLeafType == LtPicture && parent &&
          TypeHasException (ExcIsImg, parent->ElTypeNumber, parent->ElStructSchema))
        *firstSel = parent;
      return TRUE;
    }
}


/*----------------------------------------------------------------------
  CallbackEditRefAttribute
  Handle the edition of reference attribute.
  ----------------------------------------------------------------------*/
void CallbackEditRefAttribute(PtrAttrListElem pAttrElem, int frame)
{
  PtrTtAttribute      pAttr;
  PtrAttribute        pAttrNew;
  PtrDocument         pDoc;
  PtrElement          firstSel, lastSel;
  PtrReference        Ref;
  Document            doc;
  View                view;
  int                 firstChar, lastChar;

  pAttr = AttrListElem_GetTtAttribute(pAttrElem);
  FrameToView (frame, &doc, &view);
  
  if (pAttr && pAttr->AttrType == AtReferenceAttr &&
      GetValidatedCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    {
      GetAttribute (&pAttrNew);
      pAttrNew->AeAttrSSchema = pAttrElem->pSS;
      pAttrNew->AeAttrNum = pAttrElem->num;
      pAttrNew->AeDefAttr = FALSE;
      pAttrNew->AeAttrType = pAttr->AttrType;

      /* attache un bloc reference a l'attribut */
      GetReference (&Ref);
      pAttrNew->AeAttrReference = Ref;
      pAttrNew->AeAttrReference->RdElement = NULL;
      pAttrNew->AeAttrReference->RdAttribute = pAttrNew;
      /* applique l'attribut a la partie selectionnee */
      AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
                         firstSel, pDoc, TRUE);
      /* re-affiche la liste des attributs.*/
      UpdateAttrMenu (pDoc, FALSE);
    }
}

/*----------------------------------------------------------------------
  CallbackEditAttribute 
  handles the callbacks of the "Attributes" panel
  choose the right subpanel for editing it.
  ----------------------------------------------------------------------*/
void CallbackEditAttribute(PtrAttrListElem pAttrElem, int frame)
{
  PtrTtAttribute      pAttr;
  PtrAttribute        pAttrNew, currAttr;
  PtrDocument         pDoc;
  PtrElement          firstSel, lastSel;
  PtrReference        Ref;
  Document            doc;
  View                view;
  int                 firstChar, lastChar;
  
  pAttr = AttrListElem_GetTtAttribute(pAttrElem);
  FrameToView (frame, &doc, &view);

  if (pAttr && GetValidatedCurrentSelection (&pDoc, &firstSel, &lastSel,
              &firstChar, &lastChar))
    {
      
      GetAttribute (&pAttrNew);
      pAttrNew->AeAttrSSchema = pAttrElem->pSS;
      pAttrNew->AeAttrNum = pAttrElem->num;
      pAttrNew->AeDefAttr = FALSE;
      pAttrNew->AeAttrType = pAttr->AttrType;

      if (pAttr->AttrType == AtReferenceAttr)
        {
          /* attache un bloc reference a l'attribut */
          GetReference (&Ref);
          pAttrNew->AeAttrReference = Ref;
          pAttrNew->AeAttrReference->RdElement = NULL;
          pAttrNew->AeAttrReference->RdAttribute = pAttrNew;
          /* applique l'attribut a la partie selectionnee */
          AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
                             firstSel, pDoc, TRUE);
          /* re-affiche la liste des attributs.*/
          UpdateAttrMenu (pDoc, FALSE);
        }
      else
        {
          /* cherche la valeur de cet attribut pour le premier element */
          /* selectionne' */
          currAttr = AttributeValue (firstSel, pAttrNew);
          if (pAttrNew->AeAttrNum == 1)
            /* that's the language attribute */
            {
              InitFormLanguage (doc, view, firstSel, currAttr);
              /* memorise l'attribut concerne' par le formulaire */
              SchCurrentAttr = pAttrNew->AeAttrSSchema;
              NumCurrentAttr = 1;
              DocCurrentAttr = LoadedDocument[doc - 1];
            }
        }
      DeleteAttribute (NULL, pAttrNew);
    }
}

/*----------------------------------------------------------------------
  CallbackAttrMenu 
  handles the callbacks of the "Attributes" menu: creates a
  form to capture the value of the chosen attribute.
  ----------------------------------------------------------------------*/
void CallbackAttrMenu (int refmenu, int attMenu, int frame)
{
  PtrTtAttribute      pAttr;
  PtrAttribute        pAttrNew, currAttr;
  PtrDocument         pDoc;
  PtrElement          firstSel, lastSel, parent;
  PtrReference        Ref;
  PtrSRule            pSRule;
  Document            doc;
  View                view;
  int                 i;
  int                 firstChar, lastChar;
  ThotBool            mandatory;
  int                 att;

  FrameToView (frame, &doc, &view);
  /* get the right entry in the attributes list */
  /* on wxWidgets, attributs is not a menu but a dialog, this dialog do not have reference */
  /* here we must simulate the default behaviour */
  if (refmenu == -1)
    {
      /* this is the events attribut menu */
      refmenu = EventMenu[frame - 1];
    }
  if (refmenu == EventMenu[frame - 1])
    att = AttrEventNumber[attMenu];
  else
    {
      /* Search the right attribute into the table */
      if (AttrCorr[attMenu] != -1)
	att = AttrCorr[attMenu];
      else
	att = attMenu; 
      i = 0;
      while (i <= att)
        {
          if (AttrEvent[i])
            att++;
          i++;
        }
    }
  if (att >= 0)
    if (GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar,
                             &lastChar))
      {
        /* when the PICTURE element of an IMG is selected display
           attributes of the parent element */
        parent = firstSel->ElParent;
        if (firstSel->ElTerminal && firstSel->ElLeafType == LtPicture && parent &&
            TypeHasException (ExcIsImg, parent->ElTypeNumber, parent->ElStructSchema))
          firstSel = parent;

        GetAttribute (&pAttrNew);
        pAttrNew->AeAttrSSchema = AttrStruct[att];
        pAttrNew->AeAttrNum = AttrNumber[att];
        pAttrNew->AeDefAttr = FALSE;
        pAttr = AttrStruct[att]->SsAttribute->TtAttr[AttrNumber[att] - 1];
        pAttrNew->AeAttrType = pAttr->AttrType;
        if (pAttr->AttrType == AtReferenceAttr)
          {
            /* attache un bloc reference a l'attribut */
            GetReference (&Ref);
            pAttrNew->AeAttrReference = Ref;
            pAttrNew->AeAttrReference->RdElement = NULL;
            pAttrNew->AeAttrReference->RdAttribute = pAttrNew;
            /* applique l'attribut a la partie selectionnee */
            AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel,
                               firstSel, pDoc, TRUE);
            UpdateAttrMenu (pDoc, FALSE);
          }
        else
          {
            /* cherche la valeur de cet attribut pour le premier element */
            /* selectionne' */
            currAttr = AttributeValue (firstSel, pAttrNew);
            if (pAttrNew->AeAttrNum == 1)
              /* that's the language attribute */
              {
                InitFormLanguage (doc, view, firstSel, currAttr);
                /* memorise l'attribut concerne' par le formulaire */
                SchCurrentAttr = pAttrNew->AeAttrSSchema;
                NumCurrentAttr = 1;
                DocCurrentAttr = LoadedDocument[doc - 1];
                /* restaure l'etat courant du toggle */
              }
            else if (pAttr->AttrType == AtEnumAttr &&
                     pAttr->AttrNEnumValues == 1)
              /* attribut enumere' a une seule valeur(attribut booleen) */
              {
                /* is this attribute mandatory ? */
                mandatory = FALSE;
                /* get the structure rule defining the selected element */
                pSRule = firstSel->ElStructSchema->SsRule->SrElem[firstSel->ElTypeNumber - 1];
                /* look for the attribute in the list of allowed attributes
                   for this element type */
                for (i = 0; i < pSRule->SrNLocalAttrs; i++)
                  if (pSRule->SrLocalAttr->Num[i] == AttrNumber[att])
                    /* this is the attribute of interest */
                    {
                      mandatory = pSRule->SrRequiredAttr->Bln[i];
                      /* stop */
                      i = pSRule->SrNLocalAttrs;
                    }

                /* construit le formulaire de saisie de la valeur de */
                /* l'attribut */
                PtrReqAttr = NULL;
                PtrDocOfReqAttr = NULL;
                if (mandatory)
                  /* the callback of required attribute should call
                     the standard callback attribute */
                  AttrFormExists = TRUE;
                /* memorise l'attribut concerne' par le formulaire */
                SchCurrentAttr = AttrStruct[att];
                NumCurrentAttr = AttrNumber[att];
                DocCurrentAttr = LoadedDocument[doc - 1];
                /* register the current attribut */
                CurrentAttr = att;
                MenuValues (pAttr, mandatory, currAttr, firstSel, pDoc, view, FALSE);
              }
            else
              {
                /* is this attribute mandatory ? */
                mandatory = FALSE;
                /* get the structure rule defining the selected element */
                pSRule = firstSel->ElStructSchema->SsRule->SrElem[firstSel->ElTypeNumber - 1];
                /* look for the attribute in the list of allowed attributes
                   for this element type */
                for (i = 0; i < pSRule->SrNLocalAttrs; i++)
                  if (pSRule->SrLocalAttr->Num[i] == AttrNumber[att])
                    /* this is the attribute of interest */
                    {
                      mandatory = pSRule->SrRequiredAttr->Bln[i];
                      /* stop */
                      i = pSRule->SrNLocalAttrs;
                    }

                /* construit le formulaire de saisie de la valeur de */
                /* l'attribut */
                PtrReqAttr = NULL;
                PtrDocOfReqAttr = NULL;
                if (mandatory)
                  /* the callback of required attribute should call
                     the standard callback attribute */
                  AttrFormExists = TRUE;
                /* memorise l'attribut concerne' par le formulaire */
                SchCurrentAttr = AttrStruct[att];
                NumCurrentAttr = AttrNumber[att];
                DocCurrentAttr = LoadedDocument[doc - 1];
                /* register the current attribut */
                CurrentAttr = att;
                MenuValues (pAttr, mandatory, currAttr, firstSel, pDoc, view, FALSE);
                /* restore the toggle state */
                /* display the form */
                if (mandatory)
                  TtaShowDialogue (NumMenuAttrRequired, TRUE, TRUE);
                else
                  TtaShowDialogue (NumMenuAttr, TRUE, TRUE);
              }
            DeleteAttribute (NULL, pAttrNew);
          }
      }
}

/*----------------------------------------------------------------------
  CallbackLanguageMenu
  handles the callbacks of the Language form.
  ----------------------------------------------------------------------*/
void CallbackLanguageMenu (int ref, int val, char *txt)
{
  ThotBool   doit;
  char       TmpTextAttrValue[LgMaxAttrText];
  int        TmpNumCurrentAttr;

  doit = FALSE;
  switch (ref)
    {
    case NumSelectLanguage:
      /* current language name */
      if (txt == NULL)
        LangAttrValue[0] = EOS;
      else
        strncpy (LangAttrValue, txt, LgMaxAttrText);
      break;
    case NumFormLanguage:
      switch (val)
        {
        case 0:
          /* cancel */
          break;
        case 1:
          /* apply the new value if it's a valid language */
          strcpy (TmpTextAttrValue, LangAttrValue);
          strcpy (LangAttrValue, TtaGetLanguageCodeFromName (TmpTextAttrValue));
          doit = LangAttrValue[0] != EOS;
          break;
        case 2:
          /* remove the current value */
          doit = TRUE;
          break;
        }
      if (doit)
        {
          /* temporary change of TextAttrValue and NumCurrentAttr */
          strcpy (TmpTextAttrValue, TextAttrValue);
          strcpy (TextAttrValue, LangAttrValue);
          TmpNumCurrentAttr = NumCurrentAttr;
          NumCurrentAttr = 1;
          CallbackValAttrMenu (NumMenuAttr, val, NULL);
          strcpy (TextAttrValue, TmpTextAttrValue);
          NumCurrentAttr = TmpNumCurrentAttr;
        }
      break;
    }
}

/*----------------------------------------------------------------------
  CloseAttributeDialogues
  Closes all dialogue boxes related with attribute input that are
  associated with document pDoc.
  ----------------------------------------------------------------------*/
void CloseAttributeDialogues (PtrDocument pDoc)
{
  if (PtrDocOfReqAttr == pDoc)
    {
      TtaUnmapDialogue (NumMenuAttrRequired);
      TtaDestroyDialogue (NumMenuAttrRequired);
      PtrReqAttr = NULL;
      PtrDocOfReqAttr = NULL;
    }
  if (DocCurrentAttr == pDoc)
    {
      if (NumCurrentAttr == 1)
        {
          TtaUnmapDialogue (NumFormLanguage);
          TtaDestroyDialogue (NumFormLanguage);
        }
      else
        {
          TtaDestroyDialogue (NumMenuAttrRequired);
          TtaDestroyDialogue (NumMenuAttr);
        }
    }
}

/*----------------------------------------------------------------------
  AttributeMenuLoadResources
  connects the local actions.
  ----------------------------------------------------------------------*/
void AttributeMenuLoadResources ()
{
  int i;

  if (ThotLocalActions[T_chattr] == NULL)
    {
      /* Connecte les actions de selection */
      TteConnectAction (T_chattr, (Proc) SimpleUpdateAttrMenu);
      TteConnectAction (T_rattr, (Proc) CallbackAttrMenu);
      TteConnectAction (T_rattrval, (Proc) CallbackValAttrMenu);
      TteConnectAction (T_rattrlang, (Proc) CallbackLanguageMenu);
      TteConnectAction (T_attrreq, (Proc) BuildReqAttrMenu);
      TteConnectAction (T_rattrreq, (Proc) CallbackReqAttrMenu);
      for (i = 0; i < MAX_FRAME; i++)
        EventMenu[i] = 0;
    }
}
