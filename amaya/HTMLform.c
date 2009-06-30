/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * HTMLform.c: This module contains all the functions used to handle
 * forms. These are of three types: functions for handling user actions
 * on the elements of the form (e.g., clicking on a radio button), 
 * functions for resetting a form to its default value, and, finally,
 * functions for parsing and for submitting a form.
 * If you compile this module with -DFORM_DEBUG, then each time you press on
 * a Submit or Resetbutton, the subtree of the form will be printed
 * out to /tmp/FormTree.dbg.
 *
 * Authors: J. Kahan, I. Vatton
 *
 */
 
/**
   #define FORM_DEBUG
**/

#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#define PARAM_INCREMENT 50
#define MAX_OPTIONS 2000
#define MAX_SUBOPTIONS 20
#define MAX_LABEL_LENGTH 50

#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */

#include "init_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "HTMLform_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "AHTURLTools_f.h"

char        *FormBuf;    /* temporary buffer used to build the query string */
int          FormLength;  /* size of the temporary buffer */
int          FormBufIndex; /* gives the index of the last char + 1 added to
                              the buffer (only used in AddBufferWithEos) */
static ThotBool     Document_state;
static Element      Option [MAX_OPTIONS];

#ifdef _WINGUI 
extern HWND         FrMainRef [12];
extern int          ActiveFrame;
Document            opDoc;
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/ 
ThotBool SaveDocumentStatus (NotifyOnTarget *event)
{
  /* save the document status */
  Document_state = TtaIsDocumentModified (event->document);
  return FALSE; /* let Thot perform normal operation */
}
 
/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/ 
void RestoreDocumentStatus (NotifyOnTarget *event)
{
  if (!Document_state)
    {
      TtaSetDocumentUnmodified (event->document);
      /* switch Amaya buttons and menus */
      DocStatusUpdate (event->document, Document_state);
    }
}
 
 
/*----------------------------------------------------------------------
  AddToBuffer
  reallocates memory and concatenates a string into buffer	
  ----------------------------------------------------------------------*/
void AddToBuffer (const char *orig)
{
  void               *status;
  int                 lg;

  lg = strlen (orig) + 1;
  if ((int)strlen (FormBuf) + lg > FormLength)
    {
      /* it is necessary to extend the FormBuf */
      if (lg < PARAM_INCREMENT)
        lg = PARAM_INCREMENT;
      status = TtaRealloc (FormBuf, sizeof (char) * (FormLength + lg));      
      if (status)
        {
          // the buffer is now extended
          FormLength += lg;
          FormBuf = (char *)status;
          strcat (FormBuf, orig);
        }
    }
  else
    strcat (FormBuf, orig);
}

/*----------------------------------------------------------------------
  AddToBufferWithEos
  reallocates memory and concatenates a string into FormBuf. Skips the
  last EOS char.
  ----------------------------------------------------------------------*/
static void AddToBufferWithEOS (char *orig)
{
  void               *status;
  int                 lg;
  int                 i;

  lg = strlen (orig) + 2;
  if ((int)(&FormBuf[FormBufIndex] - FormBuf) + lg > FormLength)
    {
      /* it is necessary to extend the FormBuf */
      if (lg < PARAM_INCREMENT)
        i = PARAM_INCREMENT;
      else i = lg;

      status = TtaRealloc (FormBuf, sizeof (char) * (FormLength + i));

      if (status != NULL)
        {
          FormBuf = (char *)status;
          FormLength += i;
          strcpy (&FormBuf[FormBufIndex], orig);
          FormBufIndex = FormBufIndex + lg - 1;
        }
    }
  else
    {
      strcpy (&FormBuf[FormBufIndex], orig);
      FormBufIndex = FormBufIndex + lg - 1;
    }
}


/*----------------------------------------------------------------------
  AddElement
  add a string into the query FormBuf				
  ----------------------------------------------------------------------*/
static void AddElement (const unsigned char *element, CHARSET charset)
{
  CHAR_T           wc;
  char            tmp[4];
  char            tmp2[2];

  strcpy (tmp, "%");
  strcpy (tmp2, "a");
  if (FormBuf == NULL)
    {
      FormBuf = (char *)(char *)TtaGetMemory (PARAM_INCREMENT);
      FormLength = PARAM_INCREMENT;
      FormBuf[0] = EOS;
    }
  while (*element)
    {
      /* for valid standard ASCII chars */
      if (*element >= 0x20 && *element <= 0x7e)
        {
          /* verify whether the char must be escaped, according to  the
             URL BNF document */
          switch (*element)
            {
            case ';':
            case '/':
            case '#':
            case '?':
            case ':':
            case '+':
            case '&':
            case '>':
            case '<':
            case '=':
            case '%':
            case '@':
              EscapeChar (&tmp[1], *element);
              AddToBuffer (tmp);
              break;
            case SPACE:
              tmp2[0] = '+';
              AddToBuffer (tmp2);
              break;
            default:
              tmp2[0] = *element;
              AddToBuffer (tmp2);
              break;
            }
        }
      else
        {
          /* for all other characters */
          if (charset != UTF_8)
            {
              TtaMBstringToWC (&element, &wc);
              tmp2[0] = TtaGetCharFromWC (wc, charset);
              element--; /* it will be incremented after */
            }
          else
            tmp2[0] = *element;
          if (tmp2[0] == '\n')
            {
              EscapeChar (&tmp[1], __CR__);
              AddToBuffer (&tmp[0]);
              EscapeChar (&tmp[1], EOL);
              AddToBuffer (&tmp[0]);
            }
          else
            {
              EscapeChar (&tmp[1], tmp2[0]);
              AddToBuffer (tmp);
            }
        }
      element++;
    }
}

/*----------------------------------------------------------------------
  TrimSpaces
  Removes beginning and ending spaces in a char string
  ----------------------------------------------------------------------*/
static void TrimSpaces (char *string)
{
  char *start;
  char *end;
  char *ptr;

  if (!string || *string == EOS)
    return;

  start = string;

  while (*start && *start == ' ')
    start++;

  end = &string[strlen (string) - 1];

  while (end > start && *end == ' ')
    end--;

  for (ptr = string; start < end; start++, ptr++)
    *ptr = *start;
  *ptr = *start;

  if (*ptr++)
    *ptr = EOS;
}

/*----------------------------------------------------------------------
  AddNameValue
  add a name=value pair, and a trailling & into the query FormBuf	
  ----------------------------------------------------------------------*/
static void AddNameValue (const char *name, const char *value, CHARSET charset)
{
  AddElement ((unsigned char *)name, charset);
  AddToBuffer ("=");
  if (value)
    AddElement ((unsigned char *)value, charset);
  AddToBuffer ("&");
}

/*----------------------------------------------------------------------
  SubmitOption
  
  ----------------------------------------------------------------------*/
static void SubmitOption (Element option, char *name, Document doc)
{
  Element             elText;
  Attribute           attr;
  AttributeType       attrType;
  CHARSET             charset;
  Language            lang;
  int                 length;
  char             *value;

  /* check if element is selected */
  charset = TtaGetDocumentCharset (doc);
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_Selected;
  attr = TtaGetAttribute (option, attrType);
  if (attr && TtaGetAttributeValue (attr) == HTML_ATTR_Selected_VAL_Yes_)
    {
      value = NULL;
      attrType.AttrTypeNum = HTML_ATTR_Value_;
      attr = TtaGetAttribute (option, attrType);
      if (attr != NULL)
        {
          /* there's an explicit value */
          length = TtaGetTextAttributeLength (attr) + 1;
          value = (char *)TtaGetMemory (length);
          TtaGiveTextAttributeValue (attr, value, &length);
        }
      else
        {
          /* use the attached text as an implicit value */
          elText = TtaGetFirstChild(option);
          if (elText)
            {
              length = TtaGetTextLength (elText) + 1;
              value = (char *)TtaGetMemory (length);
              TtaGiveTextContent (elText, (unsigned char *)value, &length, &lang);
            }
        }
      /* remove extra spaces */
      TrimSpaces (name);
      TrimSpaces (value);
      /* save the name/value pair of the element */
      AddNameValue (name, value, charset);
      if (value)
        TtaFreeMemory (value);
    }
}


/*----------------------------------------------------------------------
  SubmitOptionMenu
  
  ----------------------------------------------------------------------*/
static void SubmitOptionMenu (Element menu, Attribute nameAttr, Document doc)
{
  ElementType         elType, opType;
  Element             option, child;
  int                 length;
  char                name[MAX_LENGTH];
  
  /* get the name of the Option Menu */
  length = MAX_LENGTH - 1;
  TtaGiveTextAttributeValue (nameAttr, name, &length);
  if (name[0] != EOS)
    {
      /* there was a value for the NAME attribute. Now, process the
         selected option elements */
      option = TtaGetFirstChild (menu);
      opType = TtaGetElementType (menu);
      opType.ElTypeNum = HTML_EL_Option;
      while (option)
        {
        elType = TtaGetElementType (option);
        if (elType.ElTypeNum == HTML_EL_Option)
           SubmitOption (option, name, doc);
        else
          {
            child = TtaSearchTypedElement (opType, SearchInTree, option);
            while (child)
              {
                SubmitOption (child, name, doc);
                // look for the next option
                child = TtaSearchTypedElementInTree (opType, SearchForward, option, child);
              }
          }
        TtaNextSibling (&option);
      }
    }
}


/*----------------------------------------------------------------------
  ResetOption
  
  ----------------------------------------------------------------------*/
static void ResetOption (Element option, ThotBool multipleSelects,
                         ThotBool *defaultSelected, Document doc)
{
  Attribute           attr, def;
  AttributeType       attrType;

  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_DefaultSelected;
  def = TtaGetAttribute (option, attrType);
  attrType.AttrTypeNum = HTML_ATTR_Selected;
  attr = TtaGetAttribute (option, attrType);
  if (!def) 
    {
      /* not a default option, so remove attribute Selected */
      if (attr)
        TtaRemoveAttribute (option, attr, doc);
    }
  else if (!multipleSelects && defaultSelected) 
    {
      /* a default option, but multiple default options are not allowed and
         one other option has already been selected */
      if (attr)
        TtaRemoveAttribute (option, attr, doc);
    }
  else if (multipleSelects || (!multipleSelects && !defaultSelected))
    {
      /* a default option and it may be selected */
      if (!attr)
        {
          /* create a new selected attribute */
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (option, attr, doc);
          TtaSetAttributeValue (attr, HTML_ATTR_Selected_VAL_Yes_, option, doc);
        }
      *defaultSelected = TRUE;
    }
}

/*----------------------------------------------------------------------
  ResetOptionMenu
  
  ----------------------------------------------------------------------*/
static void ResetOptionMenu (Element menu, Document doc)
{
  ElementType	       elType, opType;
  Element             option, firstOption, child;
  Attribute           attr;
  AttributeType       attrType;
  ThotBool            multipleSelects, defaultSelected;

  /* reset according to the default attribute */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_Multiple;
  attr = TtaGetAttribute (menu, attrType);
  if (attr && TtaGetAttributeValue (attr) == HTML_ATTR_Multiple_VAL_Yes_)
    /* it's a multiple selects menu */
    multipleSelects = TRUE;
  else
    multipleSelects = FALSE;
  firstOption = NULL;
  /* reset/set each option of the menu */
  defaultSelected = FALSE;
  /* menu is the root of the SELECT subtree */
  option = TtaGetFirstChild (menu);
  opType = TtaGetElementType (menu);
  opType.ElTypeNum = HTML_EL_Option;
  while (option)
    {
      elType = TtaGetElementType (option);
      if (elType.ElTypeNum == HTML_EL_Option)
        {
          if (!firstOption)
            firstOption = option;
          ResetOption (option, multipleSelects, &defaultSelected, doc);
        }
      else
        {
          child = TtaSearchTypedElement (opType, SearchInTree, option);
          while (child)
            {
              if (!firstOption)
                firstOption = child;
              ResetOption (child, multipleSelects, &defaultSelected, doc);
              // look for the next option
              child = TtaSearchTypedElementInTree (opType, SearchForward, option, child);
            }
        }
      TtaNextSibling (&option);
    }

  if (defaultSelected == FALSE && firstOption)
    {
      /* there's no explicit default option, so select the first option of
         the menu */
      /* select the option if it's not already selected */
      attrType.AttrTypeNum = HTML_ATTR_Selected;
      attr = TtaGetAttribute (firstOption, attrType);
      if (attr == NULL)
        {
          /* create a new selected attribute */
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (firstOption, attr, doc);
          TtaSetAttributeValue (attr, HTML_ATTR_Selected_VAL_Yes_, firstOption, doc);
        }
    }

  if (!multipleSelects)
    {
      /* call the parser to check the default selections menu ? */
      attrType.AttrTypeNum = HTML_ATTR_DefaultSelected;
      TtaSearchAttribute (attrType, SearchInTree, menu, &option, &attr);
      if (option)
        /* Reset according to the default attribute */
        OnlyOneOptionSelected (option, doc, FALSE);
    }
}

/*----------------------------------------------------------------------
  ParseForm
  traverses the tree of element, applying the parse_input 
  function to each element with an attribute NAME                    
  ----------------------------------------------------------------------*/
static void ParseForm (Document doc, Element ancestor, Element el, int mode)
{
  ElementType         elType;
  Element             elForm;
  Attribute           attr, attrS, def;
  AttributeType       attrType, attrTypeS;
  Language            lang;
  /* we initialiwe dispMode so that gcc feels happy */
  DisplayMode         dispMode = DisplayImmediately;
  CHARSET             charset;
  char                name[MAX_LENGTH];
  char               *value = NULL;
  char               *text;
  int                 length;
  ThotBool            modified = FALSE;

  if (!el || !ancestor)
    return;

  if (mode == HTML_EL_Reset_Input)
    {
      /* save current status of the document */
      modified = TtaIsDocumentModified (doc);
      /* change display mode to avoid flicker due to callbacks executed 
         when resetting the elements */
      dispMode = TtaGetDisplayMode (doc);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
    }
  
  charset = TtaGetDocumentCharset (doc);
  lang = TtaGetDefaultLanguage ();      
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_NAME;
  attrTypeS.AttrSSchema = attrType.AttrSSchema;
  TtaSearchAttribute (attrType, SearchForward, ancestor, &el, &attr);
  while (el && TtaIsAncestor(el, ancestor))
    {
      if (attr)
        {
          elType = TtaGetElementType (el);
          switch (elType.ElTypeNum)
            {
            case HTML_EL_Option_Menu:
              if (mode == HTML_EL_Submit_Input)
                SubmitOptionMenu (el, attr, doc);
              else if (mode == HTML_EL_Reset_Input)
                ResetOptionMenu (el, doc);
              break;
	      
            case HTML_EL_Checkbox_Input:
              if (mode == HTML_EL_Submit_Input)
                {
                  /* Get the element's current status */
                  attrTypeS.AttrTypeNum = HTML_ATTR_Checked;
                  attrS = TtaGetAttribute (el, attrTypeS);
                  if (attrS != NULL &&
                      TtaGetAttributeValue (attrS) == HTML_ATTR_Checked_VAL_Yes_)
                    {
                      /* save the NAME attribute of the element el */
                      length = MAX_LENGTH - 1;
                      TtaGiveTextAttributeValue (attr, name, &length);
                      /* get the "value" attribute */
                      attrTypeS.AttrTypeNum = HTML_ATTR_Value_;
                      attrS = TtaGetAttribute (el, attrTypeS);
                      if (attrS != NULL)
                        {
                          /* save the Value attribute of the element el */
                          length = TtaGetTextAttributeLength (attrS) + 1;
                          value = (char *)TtaGetMemory (length);
                          TtaGiveTextAttributeValue (attrS, value, &length);
                          AddNameValue (name, value, charset);
                          TtaFreeMemory (value);
                          value = NULL;
                        }
                      else
                        /* give a default checkbox value (On) */
                        AddNameValue (name, "on", charset);
                    }
                }
              else if (mode == HTML_EL_Reset_Input)
                {
                  /* Reset according to the default attribute */
                  attrTypeS.AttrTypeNum = HTML_ATTR_DefaultChecked;
                  def = TtaGetAttribute (el, attrTypeS);
                  /* remove previous checked attribute */
                  attrTypeS.AttrTypeNum = HTML_ATTR_Checked;
                  attrS = TtaGetAttribute (el, attrTypeS);
                  if (attrS != NULL)
                    TtaRemoveAttribute (el, attrS, doc);
                  /* create a new checked attribute */
                  attrS = TtaNewAttribute (attrTypeS);
                  TtaAttachAttribute (el, attrS, doc);
                  if (def != NULL)
                    TtaSetAttributeValue (attrS, HTML_ATTR_Checked_VAL_Yes_, el, doc);
                  else
                    TtaSetAttributeValue (attrS, HTML_ATTR_Checked_VAL_No_, el, doc);
                }
              break;
	      
            case HTML_EL_Radio_Input:
              if (mode == HTML_EL_Submit_Input)
                {
                  /* Get the element's current status */
                  attrTypeS.AttrTypeNum = HTML_ATTR_Checked;
                  attrS = TtaGetAttribute (el, attrTypeS);
                  if (attrS != NULL &&
                      TtaGetAttributeValue (attrS) == HTML_ATTR_Checked_VAL_Yes_)
                    {
                      /* get the Value attribute */
                      attrTypeS.AttrTypeNum = HTML_ATTR_Value_;
                      attrS = TtaGetAttribute (el, attrTypeS);
                      if (attrS != NULL)
                        {
                          /* save the NAME attribute of the element el */
                          length = MAX_LENGTH - 1;
                          TtaGiveTextAttributeValue (attr, name, &length);
                          /* save the Value attribute of the element el */
                          length = TtaGetTextAttributeLength (attrS) + 1;
                          value = (char *)TtaGetMemory (length);
                          TtaGiveTextAttributeValue (attrS, value, &length);
                          AddNameValue (name, value, charset);
                          TtaFreeMemory (value);
                          value = NULL;
                        }
                    }
                }
              else if (mode == HTML_EL_Reset_Input)
                {
                  /* Reset according to the default attribute */
                  attrTypeS.AttrTypeNum = HTML_ATTR_DefaultChecked;
                  def = TtaGetAttribute (el, attrTypeS);
                  /* remove previous checked attribute */
                  attrTypeS.AttrTypeNum = HTML_ATTR_Checked;
                  attrS = TtaGetAttribute (el, attrTypeS);
                  if (attrS != NULL)
                    TtaRemoveAttribute (el, attrS, doc);
                  /* create a new checked attribute */
                  attrS = TtaNewAttribute (attrTypeS);
                  TtaAttachAttribute (el, attrS, doc);
                  if (def != NULL)
                    TtaSetAttributeValue (attrS, HTML_ATTR_Checked_VAL_Yes_, el, doc);
                  else
                    TtaSetAttributeValue (attrS, HTML_ATTR_Checked_VAL_No_, el, doc);
                }
              break;
	      
            case HTML_EL_Text_Area:
            case HTML_EL_Text_Input:
            case HTML_EL_File_Input:
            case HTML_EL_Password_Input:
              if (mode == HTML_EL_Submit_Input)
                {
                  /* search the value in the text element */
                  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                  elForm = TtaSearchTypedElement (elType, SearchInTree, el);
                  /* save the NAME attribute of the element el */
                  length = MAX_LENGTH - 1;
                  TtaGiveTextAttributeValue (attr, name, &length);
                  AddElement ((unsigned char *)name, charset);
                  AddToBuffer ("=");
                  while (elForm)
                    {
                      length = TtaGetTextLength (elForm) + 1;
                      text = (char *)TtaGetMemory (length);
                      TtaGiveTextContent (elForm, (unsigned char *)text, &length, &lang);
                      AddElement ((unsigned char *)text, charset);
                      TtaFreeMemory (text);
                      elForm = TtaSearchTypedElementInTree (elType, SearchForward, el, elForm);
                    }
                  AddToBuffer ("&");
                }
              else if (mode == HTML_EL_Reset_Input)
                {
                  /* Reset according to the default attribute*/
                  /* gets the default value */
                  attrTypeS.AttrTypeNum = HTML_ATTR_Default_Value;
                  def = TtaGetAttribute (el, attrTypeS);
                  if (def != NULL)
                    {
                      length = TtaGetTextAttributeLength (def) + 1;
                      value = (char *)TtaGetMemory (length);
                      TtaGiveTextAttributeValue (def, value, &length);
                    }
                  else
                    {
                      /* there's no default value */
                      value = (char *)TtaGetMemory (1);
                      value[0] = EOS;
                    }
                  /* search the value in the text element */
                  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                  elForm = TtaSearchTypedElement (elType, SearchInTree, el);
                  /* reset the value of the element */
                  if (elForm != NULL) 
                    TtaSetTextContent (elForm, (unsigned char *)value, lang, doc);
                  TtaFreeMemory (value);
                  value = NULL;
                }
              break;
	      
            case HTML_EL_Hidden_Input:
              if (mode == HTML_EL_Submit_Input)
                {
                  /* the value is in the default value attribute */
                  attrTypeS.AttrTypeNum = HTML_ATTR_Value_;
                  attrS = TtaGetAttribute (el, attrType);
                  def = TtaGetAttribute (el, attrTypeS);
                  if (def != NULL)
                    {
                      /* save the NAME attribute of the element el */
                      length = MAX_LENGTH - 1;
                      TtaGiveTextAttributeValue (attr, name, &length);
                      /* save of the element content */
                      length = TtaGetTextAttributeLength (def) + 1;
                      value = (char *)TtaGetMemory (length);
                      TtaGiveTextAttributeValue (def, value, &length);
                      AddNameValue (name, value, charset);
                      TtaFreeMemory (value);
                      value = NULL;
                    }
                }
              break;
	      
            default:
              break;
            }
        }
      TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
    }
  
  if (mode == HTML_EL_Reset_Input)
    {
      /* restore original display mode */
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
      
      /* restore status of the document */
      if (!modified)
        {
          TtaSetDocumentUnmodified (doc);
          /* switch Amaya buttons and menus */
          DocStatusUpdate (doc, modified);
        }
    }
}

/*----------------------------------------------------------------------
  DoSubmit
  submits a form : builds the query string and sends the request
  action is a utf-8 string.
  ----------------------------------------------------------------------*/
static void DoSubmit (Document doc, int method, char *action)
{
  char               *urlName, *ptr, *param;
  int                 evt;
  int                 lg;

  /* clear the selection */
  if (TtaGetSelectedDocument () == doc)
    TtaUnselect (doc);

  /* Reencode the URI */
  switch (method)
    {
    case HTML_ATTR_METHOD_VAL_Get_:
    case HTML_ATTR_METHOD_VAL_Post_:
      if (method == HTML_ATTR_METHOD_VAL_Get_)
        evt = CE_FORM_GET;
      else
        evt = CE_FORM_POST;
      urlName = (char *)TtaConvertMbsToByte ((unsigned char *)action,
                                             TtaGetDefaultCharset ());
      if (urlName)
        {
          if (FormBuf)
            {
              lg = strlen (FormBuf);
              if (lg && FormBuf[lg - 1] == '&')
                /* remove any trailing & */
                FormBuf[lg - 1] = EOS;
              ptr = FormBuf;
            }
          else
            ptr = NULL;
          if (ptr)
            param = (char *)TtaConvertMbsToByte ((unsigned char *)ptr,
                                                 TtaGetDefaultCharset ());
          else
            param = NULL;
          GetAmayaDoc (urlName, param, doc, doc, evt, TRUE, NULL, NULL);
          if (ptr)
            TtaFreeMemory (param);
          TtaFreeMemory (urlName);
        }
      break;
    default:
      break;
    }
}



/*----------------------------------------------------------------------
  SubmitForm
  callback handler that launches the parsing of the form containing 
  the element and sends	the query to the server
  ----------------------------------------------------------------------*/
void SubmitForm (Document doc, Element element)
{
  Element             elForm;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  CHARSET             charset;
  char               *action, *name, *value, *info;
  int                 i, length, button_type;
  int                 method;
  ThotBool	       found, withinForm;

  FormBuf = NULL;
  action = NULL;
  /* find out the characteristics of the button which was pressed */
  found = FALSE;
  while (!found && element)
    {
      elType = TtaGetElementType (element);
      if (elType.ElTypeNum == HTML_EL_Reset_Input ||
          elType.ElTypeNum == HTML_EL_Submit_Input ||
          elType.ElTypeNum == HTML_EL_BUTTON_ ||
          elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
        found = TRUE;
      else
        element = TtaGetParent (element);
    }
  if (!found)
    return;

  charset = TtaGetDocumentCharset (doc);
  if (charset == UNDEFINED_CHARSET)
    {
      if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat)
        charset = UTF_8;
      else
        charset = ISO_8859_1;
    }
  button_type = 0;
  attrType.AttrSSchema = elType.ElSSchema;
  switch (elType.ElTypeNum)
    {
    case HTML_EL_Reset_Input:
      button_type = HTML_EL_Reset_Input;
      break;
    case HTML_EL_BUTTON_:
      attrType.AttrTypeNum = HTML_ATTR_Button_type;
      attr = TtaGetAttribute (element, attrType);
      if (!attr)
        /* default value of attribute type is submit */
        button_type = HTML_EL_Submit_Input;
      else
        {
          i = TtaGetAttributeValue (attr);
          if (i == HTML_ATTR_Button_type_VAL_submit)
            button_type = HTML_EL_Submit_Input;
          else if (i == HTML_ATTR_Button_type_VAL_reset)
            button_type = HTML_EL_Reset_Input;
        }
      break;
    case HTML_EL_Submit_Input:
      button_type = HTML_EL_Submit_Input;
      break;
    case HTML_EL_PICTURE_UNIT:
      button_type = HTML_EL_Submit_Input;
      /* get the button's name, if it exists */
      attrType.AttrTypeNum = HTML_ATTR_NAME;
      attr = TtaGetAttribute (element, attrType);
      if (attr != NULL)
        {
          length = TtaGetTextAttributeLength (attr);
          name = (char *)TtaGetMemory (length + 3);
          TtaGiveTextAttributeValue (attr, name, &length);
          strcat (name, ". ");
          length ++;
          /* get the x and y coordinates */
          info = GetActiveImageInfo (doc, element);
          if (info != NULL) 
            {
              /* create the x name-value pair */
              name [length] = 'x';
              for (i = 0; info[i] != ','; i++);
              info[i] = EOS;
              /* skip the ? char */
              value = &info[1];
              AddNameValue (name, value, charset);
              /* create the y name-value pair */
              name [length] = 'y';
              value = &info[i+1];
              AddNameValue (name, value, charset);
              TtaFreeMemory (info);
            }
          if (name)
            TtaFreeMemory (name);
        }
      break;
    }
  
  if (button_type == 0)
    return;

  if (elType.ElTypeNum == HTML_EL_Submit_Input ||
      (elType.ElTypeNum == HTML_EL_BUTTON_ &&
       button_type == HTML_EL_Submit_Input))
    {
      /* get the button's value and name, if they exist */
      attrType.AttrTypeNum = HTML_ATTR_NAME;
      attr = TtaGetAttribute (element, attrType);
      if (attr != NULL)
        {
          value = NULL;
          length = TtaGetTextAttributeLength (attr);
          name = (char *)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (attr, name, &length);
          attrType.AttrTypeNum = HTML_ATTR_Value_;
          attr = TtaGetAttribute (element, attrType);
          if (attr != NULL)
            {
              length = TtaGetTextAttributeLength (attr);
              value = (char *)TtaGetMemory (length + 1);
              TtaGiveTextAttributeValue (attr, value, &length);
              AddNameValue (name, value, charset);
            }
          if (name)
            {
              TtaFreeMemory (name);
              if (value)
                TtaFreeMemory (value);
            }
        }
    }

  /* find the parent form node */
  elType.ElTypeNum = HTML_EL_Form;
  elForm = TtaGetTypedAncestor (element, elType);
  withinForm = (elForm != NULL);
  if (!withinForm)
    {
      /* could not find an ancestor form -> check a previous one */
      elForm = TtaSearchTypedElement (elType, SearchBackward, element);
      if (!elForm)
        {
          /* could not find a form before that element */
          TtaFreeMemory (FormBuf);
          return;
        }
    }

  /* get the  ACTION attribute value */
  if (button_type == HTML_EL_Submit_Input)
    {
      attrType.AttrTypeNum = HTML_ATTR_Script_URL;
      attr = TtaGetAttribute (elForm, attrType);
      if (attr != NULL)
        {
          length = TtaGetTextAttributeLength (attr);
          if (length)
            {
              action = (char *)TtaGetMemory (length + 1);
              TtaGiveTextAttributeValue (attr, action, &length);
            }
        }
      else
        action = NULL;

      /* get the  METHOD attribute value */
      attrType.AttrTypeNum = HTML_ATTR_METHOD;
      attr = TtaGetAttribute (elForm, attrType);
      if (attr == NULL)
        method = HTML_ATTR_METHOD_VAL_Get_;
      else
        method = TtaGetAttributeValue (attr);
    }
  else
    method = HTML_ATTR_METHOD_VAL_Get_;

  /* search the subtree for the form elements */
  element  = TtaGetFirstChild(elForm);
  /* process the form */
  if (button_type == HTML_EL_Submit_Input)
    {
      if (action)
        {
          if (CanReplaceCurrentDocument (doc, 1))
            {
              ParseForm (doc, elForm, element, button_type);
              DoSubmit (doc, method, action);
            }
        }
      else
        InitConfirm3L (doc, 1, "No action", NULL, NULL, NO);
    }
  else
    ParseForm (doc, elForm, element, button_type);
   
  if (action)
    TtaFreeMemory (action);
  TtaFreeMemory (FormBuf);
}


/*----------------------------------------------------------------------
  HandleReturn submits the enclosing form
  -----------------------------------------------------------------------*/
ThotBool HandleReturn (NotifyOnTarget *event)
{
  Element             elForm;
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  char               *action = NULL;
  int                 method, length;

  /* find the parent form element */
  elType = TtaGetElementType (event->element);
  elType.ElTypeNum = HTML_EL_Form;
  attrType.AttrSSchema = elType.ElSSchema;
  elForm = TtaGetTypedAncestor (event->element, elType);
  if (elForm)
    {
      /* get the  METHOD attribute value */
      attrType.AttrTypeNum = HTML_ATTR_METHOD;
      attr = TtaGetAttribute (elForm, attrType);
      if (attr == NULL)
        method = HTML_ATTR_METHOD_VAL_Get_;
      else
        method = TtaGetAttributeValue (attr);
      /* get the  ACTION attribute value */
      attrType.AttrTypeNum = HTML_ATTR_Script_URL;
      attr = TtaGetAttribute (elForm, attrType);
      if (attr != NULL)
        {
          length = TtaGetTextAttributeLength (attr);
          if (length)
            {
              action = (char *)TtaGetMemory (length + 1);
              TtaGiveTextAttributeValue (attr, action, &length);
              FormBuf = NULL;
              ParseForm (event->document, elForm, elForm, 
                         HTML_EL_Submit_Input);
              DoSubmit (event->document, method, action);
              TtaFreeMemory (action);
              if (FormBuf && *FormBuf != EOS)
                TtaFreeMemory (FormBuf);
            }
        }
      else
        action = NULL;
    }
  return TRUE; /* don't let Thot perform normal operation */
}


/*----------------------------------------------------------------------
  ActivateFileInput
  The user has triggered the Browse button associated with a File_Input
  element.  Display a file selector.
  ----------------------------------------------------------------------*/
void ActivateFileInput (Document doc, Element el)
{
	/******* TO DO *******/
}


/*----------------------------------------------------------------------
  SelectCheckbox
  selects a Checkbox input				
  ----------------------------------------------------------------------*/
void SelectCheckbox (Document doc, Element el)
{
  ElementType         elType;
  Attribute           attr;
  AttributeType       attrType;
  ThotBool            modified = FALSE;

  if (el == NULL)
    return;
  elType = TtaGetElementType (el);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_Checked;
  if (elType.ElTypeNum == HTML_EL_Checkbox_Input)
    {
      modified = TtaIsDocumentModified (doc);
      /* change the checked attribute of this checkbox */
      attr = TtaGetAttribute (el, attrType);
      if (attr == NULL)
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
          TtaSetAttributeValue (attr, HTML_ATTR_Checked_VAL_Yes_, el, doc);
        }
      else if (TtaGetAttributeValue (attr) == HTML_ATTR_Checked_VAL_Yes_)
        {
          TtaRemoveAttribute (el, attr, doc);
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
          TtaSetAttributeValue (attr, HTML_ATTR_Checked_VAL_No_, el, doc);
        }
      else
        {
          TtaRemoveAttribute (el, attr, doc);
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
          TtaSetAttributeValue (attr, HTML_ATTR_Checked_VAL_Yes_, el, doc);
        }
      if (!modified)
        {
          TtaSetDocumentUnmodified (doc);
          /* switch Amaya buttons and menus */
          DocStatusUpdate (doc, modified);
        }
    }
}

/*----------------------------------------------------------------------
  SelectOneRadio
  selects one Radio input				
  ----------------------------------------------------------------------*/
void SelectOneRadio (Document doc, Element el)
{
  ElementType         elType;
  Element             elForm, elFound;
  Attribute           attr, attrN;
  AttributeType       attrType, attrTypeN;
  ThotBool            modified = FALSE;
  int                 length;
  char                name[MAX_LENGTH], *buffer = NULL;

  if (el == NULL)
    return;
  elType = TtaGetElementType (el);

  /* extract the NAME attribute */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_Checked;
  attrTypeN.AttrSSchema = attrType.AttrSSchema;
  attrTypeN.AttrTypeNum = HTML_ATTR_NAME;
  attrN = TtaGetAttribute (el, attrTypeN);
  if (attrN != NULL)
    {
      /* save the NAME attribute of the element */
      length = MAX_LENGTH - 1;
      TtaGiveTextAttributeValue (attrN, name, &length);
    }
  else
    return;

  if (elType.ElTypeNum == HTML_EL_Radio_Input)
    {
      attr = TtaGetAttribute (el, attrType);
      if (attr != NULL && TtaGetAttributeValue (attr) == HTML_ATTR_Checked_VAL_Yes_)
        /* nothing to do */
        return;
      else
        {
          modified = TtaIsDocumentModified (doc);
          /* set the checked attribute of this radio input */
          if (attr != NULL)
            TtaRemoveAttribute (el, attr, doc);
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
          TtaSetAttributeValue (attr, HTML_ATTR_Checked_VAL_Yes_, el, doc);
	  
          /* Remove other checked radio input with the same NAME */
          elForm = TtaGetParent (el);
          while (elType.ElTypeNum != HTML_EL_BODY && elType.ElTypeNum != HTML_EL_Form
                 && elForm != NULL)
            {
              elForm = TtaGetParent (elForm);
              elType = TtaGetElementType (elForm);
            }

          if (elForm)
            {
              /* search the first radio input */
              elType.ElTypeNum = HTML_EL_Radio_Input;
              elFound = TtaSearchTypedElement (elType, SearchInTree, elForm);
              while (elFound)
                {
                  if (elFound != el)
                    {
                      /* compare its NAME attribute */
                      attrN = TtaGetAttribute (elFound, attrTypeN);
                      if (attrN != NULL)
                        {
                          length = TtaGetTextAttributeLength (attrN) + 1;
                          buffer = (char *)TtaGetMemory (length);
                          TtaGiveTextAttributeValue (attrN, buffer, &length);
                          if (!strcmp (name, buffer))
                            {
                              /* same NAME: set the checked attribute to NO */
                              attr = TtaGetAttribute (elFound, attrType);
                              if (attr != NULL
                                  && TtaGetAttributeValue (attr) == HTML_ATTR_Checked_VAL_Yes_)
                                {
                                  TtaRemoveAttribute (elFound, attr, doc);
                                  attr = TtaNewAttribute (attrType);
                                  TtaAttachAttribute (elFound, attr, doc);
                                  TtaSetAttributeValue (attr, HTML_ATTR_Checked_VAL_No_, elFound, doc);
                                }
                            }
                          TtaFreeMemory (buffer);
                          buffer = NULL;
                        }
                    }
                  /* search the next radio input */
                  elFound = TtaSearchTypedElementInTree (elType, SearchForward, elForm, elFound);
                }
            }
          if (!modified)
            {
              TtaSetDocumentUnmodified (doc);
              /* switch Amaya buttons and menus */
              DocStatusUpdate (doc, modified);
            }
        }
    }
}


/*----------------------------------------------------------------------
  SelectOneOption
  selects an option in option menu			
  ----------------------------------------------------------------------*/
void SelectOneOption (Document doc, Element el)
{
#ifdef _WINGUI
  int                 nbOldEntries = 20;
#endif /* _WINGUI */
#if defined (_WINGUI) || defined (_GTK)
  int                 i;
#endif /* _WINGUI || _GTK */
  ElementType         elType;
  Element	            elText, menuEl, parent, other;
  ThotBool	          selected[MAX_OPTIONS];
  AttributeType       attrType;
  Attribute	          attr;
  SSchema	            htmlSch;
  char                text[MAX_LABEL_LENGTH + 1];
  char               *tmp;
  Language            lang;
  int                 length, nbitems;
  ThotBool            modified = FALSE;
  ThotBool     	      multipleOptions, sel;

  if (el == NULL)
    return;

#ifdef _WINGUI
  opDoc = doc;
#endif /* _WINGUI */
  htmlSch = TtaGetSSchema ("HTML", doc);
  /* search the enclosing option element */
  do
    {
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum != HTML_EL_Option || elType.ElSSchema != htmlSch)
        el = TtaGetParent (el);
    }
  while (el &&
         (elType.ElTypeNum != HTML_EL_Option || elType.ElSSchema != htmlSch));

  if (elType.ElTypeNum == HTML_EL_Option && elType.ElSSchema == htmlSch)
    {
      /* create the option menu */
      nbitems = 0;
      parent = NULL;
      elType.ElTypeNum = HTML_EL_Option_Menu;
      menuEl = TtaGetTypedAncestor (el, elType);
      if (menuEl)
        {
          attrType.AttrSSchema = htmlSch;
          attrType.AttrTypeNum = HTML_ATTR_Multiple;
          attr = TtaGetAttribute (menuEl, attrType);
          if (attr)
            /* multiple options are allowed */
            multipleOptions = TRUE;
          else
            multipleOptions = FALSE;
	   
          attrType.AttrTypeNum = HTML_ATTR_Selected;
          el = TtaGetFirstChild (menuEl);

          /* use the global allocation buffer to store the entries */
          FormBuf = (char *)TtaGetMemory (PARAM_INCREMENT);
          FormLength = PARAM_INCREMENT;
          FormBuf[0] = EOS;
          FormBufIndex = 0;
          other = NULL;
          while (el && nbitems < MAX_OPTIONS)
            {
              elType = TtaGetElementType (el);
              if ((elType.ElTypeNum != HTML_EL_Option &&
                   elType.ElTypeNum != HTML_EL_OptGroup) ||
                  elType.ElSSchema != htmlSch)
                {
                  // another element: look for an included option
                  other = el;
                  elType.ElTypeNum = HTML_EL_Option;
                  el = TtaSearchTypedElement (elType, SearchInTree, other);
                  elType = TtaGetElementType (el);
                }
              if (el && elType.ElSSchema == htmlSch &&
                  (elType.ElTypeNum == HTML_EL_Option ||
                   elType.ElTypeNum == HTML_EL_OptGroup))
                {
                  Option[nbitems] = el;
                  if (multipleOptions)
                    {
                      attrType.AttrTypeNum = HTML_ATTR_Selected;
                      selected[nbitems] = (TtaGetAttribute (el, attrType) != NULL);
                    }
                  /* get the menu item label */
                  /* is there a label attribute? */
                  attrType.AttrTypeNum = HTML_ATTR_label;
                  attr = TtaGetAttribute (el, attrType);
                  length = MAX_LABEL_LENGTH;
                  text[1] = EOS;
                  if (attr)
                    TtaGiveTextAttributeValue (attr, text + 1, &length);
                  else if (elType.ElTypeNum == HTML_EL_Option)
                    /* there is no label attribute, but it's an Option
                       Take its content as the item label */
                    {
                      elText = TtaGetFirstChild (el);
                      if (elText)
                        TtaGiveTextContent (elText, (unsigned char *)(text + 1),
                                            &length, &lang);
                      else
                        length = 1;
                    }
                  else
                    length = 1;
                  /* count the EOS character */
                  text[length + 1] = EOS;
                  /* add an item */
                  /* we have to add the 'B', 'T' or 'M' character */
                  if (elType.ElTypeNum == HTML_EL_OptGroup)
                    {
                      /* add the sub-list items */
                      parent = el;
                      el = TtaGetFirstChild (parent);
                      text[0] = 'M';
                    }
                  else
                    {
                      /* get next element in the current list */
                      TtaNextSibling (&el);
                      if (multipleOptions)
                        text[0] = 'T';
                      else
                        text[0] = 'B';
                    }
                  /* convert the UTF-8 string */
                  tmp = (char *)TtaConvertMbsToByte ((unsigned char *)text,
                                                     TtaGetDefaultCharset ());
                  if (tmp)
                    {
                      AddToBufferWithEOS (tmp);
                      TtaFreeMemory (tmp);
                      nbitems++;
                    }
                }
              else
                TtaNextSibling (&el);
              if (el == NULL && parent)
                {
                  /* continue with the sibling of the parent */
                  el = parent;
                  parent = NULL;
                  TtaNextSibling (&el);
                }
              if (el == NULL && other)
                {
                  /* continue with the sibling of the parent */
                  el = other;
                  other = NULL;
                  TtaNextSibling (&el);
                }
            }

          if (nbitems == 0)
            TtaFreeMemory (FormBuf);
          else
            {
              /* create the main menu */
#if defined (_WINGUI) || defined (_GTK) || defined(_WX)
              TtaNewScrollPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1),
                                 NULL, nbitems, FormBuf, NULL, multipleOptions, 'L');
#endif /* _WINGUI || _GTK || _WX */
              TtaFreeMemory (FormBuf);
#ifdef _WINGUI
              if (multipleOptions)
                for (i = 0; i < nbitems; i++)
                  if (selected[i])         
                    WIN_TtaSetToggleMenu (BaseDialog + OptionMenu,
                                          i, TRUE, FrMainRef [ActiveFrame]);
#endif /* _WINGUI */
#if defined(_GTK)
              if (multipleOptions)
                for (i = 0; i < nbitems; i++)
                  if (selected[i])         
                    TtaSetToggleMenu (BaseDialog + OptionMenu, i, TRUE);
#endif /* #if defined(_GTK) */

#ifdef _WX
              wxASSERT_MSG( !multipleOptions, _T("TODO: multipleOptions") );
#endif /* _WX */
	       
              /* activate the menu that has just been created */
              ReturnOption = -1;
#if defined(_GTK)  || defined(_WX)
              TtaSetDialoguePosition ();
#endif /* #if defined(_GTK) || _WX */
              TtaShowDialogue (BaseDialog + OptionMenu, FALSE, TRUE);
              /* wait for an answer from the user */
              TtaWaitShowProcDialogue ();
              /* destroy the dialogue */
              TtaDestroyDialogue (BaseDialog + OptionMenu);
              if (ReturnOption >= 0)
                {
                  /* make the returned option selected */
                  el = Option[ReturnOption];
                  sel = selected[ReturnOption];
                  modified = TtaIsDocumentModified (doc);	  
                  if (!multipleOptions)
                    OnlyOneOptionSelected (el, doc, FALSE);
                  else
                    {
                      attrType.AttrTypeNum = HTML_ATTR_Selected;
                      attr = TtaGetAttribute (el, attrType);
                      if (sel)
                        TtaRemoveAttribute (el, attr, doc);
                      else
                        {
                          if (!attr)
                            attr = TtaNewAttribute (attrType);
                          TtaAttachAttribute (el, attr, doc);
                          TtaSetAttributeValue (attr, HTML_ATTR_Selected_VAL_Yes_, el, doc);
                        }
                    }
                  if (!modified)
                    {
                      TtaSetDocumentUnmodified (doc);
                      /* switch Amaya buttons and menus */
                      DocStatusUpdate (doc, modified);
                    }
                  /* clear the selection (which may happen from clicking on the dialogue) */
                  TtaUnselect (doc);
                }
            }
        }
    }
}
 
