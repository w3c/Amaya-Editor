/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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

/* Included headerfiles */
#define THOT_EXPORT extern

#include "amaya.h"

#define PARAM_INCREMENT 50

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */
#include "init_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "HTMLform_f.h"
#include "AHTURLTools_f.h"

static STRING       buffer;    /* temporary buffer used to build the query
				  string */
static int          lgbuffer;  /* size of the temporary buffer */
static int          documentStatus;
 
extern STRING GetActiveImageInfo (Document document, Element element);

#ifdef _WINDOWS 
extern HWND FrMainRef [12];
extern int  currentFrame;
Document  opDoc;
Element   opOption [200];
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/ 
#ifdef __STDC__
ThotBool SaveDocumentStatus (NotifyOnTarget *event)
#else /* __STDC__*/
ThotBool SaveDocumentStatus (event)
     NotifyOnTarget *event;
#endif /* __STDC__*/
{
  /* save the document status */
  documentStatus = TtaIsDocumentModified (event->document);
  return FALSE; /* let Thot perform normal operation */
}
 
 
/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/ 
#ifdef __STDC__
void RestoreDocumentStatus (NotifyOnTarget *event)
#else /* __STDC__*/
void RestoreDocumentStatus (event)
     NotifyOnTarget *event;
#endif /* __STDC__*/
{
  if (!documentStatus)
    {
      TtaSetDocumentUnmodified (event->document);
      /* switch Amaya buttons and menus */
      DocStatusUpdate (event->document, documentStatus);
    }
}
 
 
/*----------------------------------------------------------------------
  AddToBuffer
  reallocates memory and concatenates a string into buffer	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddToBuffer (STRING orig)
#else
static void         AddToBuffer (orig)
STRING              orig;

#endif
{
   void               *status;
   int                 lg;

   lg = ustrlen (orig) + 1;
   if ((int)ustrlen (buffer) + lg > lgbuffer)
     {
	/* it is necessary to extend the buffer */
	if (lg < PARAM_INCREMENT)
	   lg = PARAM_INCREMENT;
	status = TtaRealloc (buffer, sizeof (CHAR_T) * (lgbuffer + lg));

	if (status != NULL)
	  {
	     buffer = status;
	     lgbuffer += lg;
	     ustrcat (buffer, orig);
	  }
     }
   else
      ustrcat (buffer, orig);
}


/*----------------------------------------------------------------------
  AddElement
  add a string into the query buffer				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddElement (USTRING element)
#else
static void         AddElement (element)
USTRING             element;

#endif
{
   CHAR_T                tmp[4];
   CHAR_T                tmp2[2];


   usprintf (tmp, TEXT("%s"), "%");
   usprintf (tmp2, TEXT("%s"), "a");

   if (buffer == (STRING) NULL)
     {
	buffer = TtaAllocString (PARAM_INCREMENT);
	lgbuffer = PARAM_INCREMENT;
	buffer[0] = EOS;
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
	/* for all other characters */
	else 
	  if (*element == '\n')
	    {
	      EscapeChar (&tmp[1], __CR__);
	      AddToBuffer (&tmp[0]);
	      EscapeChar (&tmp[1], EOL);
	      AddToBuffer (&tmp[0]);
	    }
	  else
	    {
	      EscapeChar (&tmp[1], *element);
	      AddToBuffer (tmp);
	    }
	element++;
     }
}

/*----------------------------------------------------------------------
  TrimSpaces
  Removes beginning and ending spaces in a char string
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TrimSpaces (STRING string)
#else
static void         TrimSpaces (string)
STRING              string;
#endif
{
  STRING start;
  STRING end;
  STRING ptr;

  if (!string || *string == EOS)
    return;

  start = string;

  while (*start && *start == ' ')
    start++;

  end = &string[ustrlen (string) - 1];

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
  add a name=value pair, and a trailling & into the query buffer	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddNameValue (STRING name, STRING value)
#else
static void         AddNameValue (name, value)
STRING              name, value,
#endif
{
   AddElement (name);
   AddToBuffer (TEXT("="));
   if (value)
      AddElement (value);
   AddToBuffer (TEXT("&"));
}

/*----------------------------------------------------------------------
  SubmitOption
  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SubmitOption (Element option, STRING name, Document doc)
#else
static void         SubmitOption (option, name, doc)
Element		    option;
STRING	            name;
Document	    doc;
#endif
{
  Element             elText;
  Attribute           attr;
  AttributeType       attrType;
  int                 length;
  CHAR_T             *value;
  Language            lang;

  /* check if element is selected */
  attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
  attrType.AttrTypeNum = HTML_ATTR_Selected;
  attr = TtaGetAttribute (option, attrType);
  if (attr && TtaGetAttributeValue (attr) == HTML_ATTR_Selected_VAL_Yes_)
      {
      attrType.AttrTypeNum = HTML_ATTR_Value_;
      attr = TtaGetAttribute (option, attrType);
      if (attr != NULL)
        {
	/* there's an explicit value */
	length = TtaGetTextAttributeLength (attr) + 1;
	value = TtaAllocString (length);
	TtaGiveTextAttributeValue (attr, value, &length);
        }
      else
        {
	/* use the attached text as an implicit value */
	elText = TtaGetFirstChild(option);
	length = TtaGetTextLength (elText) + 1;
	value = TtaAllocString (length);
	TtaGiveTextContent (elText, value, &length, &lang);
        }
      /* remove extra spaces */
      TrimSpaces (name);
      TrimSpaces (value);
      /* save the name/value pair of the element */
      AddNameValue (name, value);
      TtaFreeMemory (value);
      }
}


/*----------------------------------------------------------------------
  SubmitOptionMenu
  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void        SubmitOptionMenu (Element menu, Attribute nameAttr, Document doc)
#else
static void        SubmitOptionMenu (menu, nameAttr, doc)
Element		   menu;
Attribute	   nameAttr;
Document	   doc;
#endif
{
  ElementType         elType;
  Element             option, child;
  int                 length;
  CHAR_T              name[MAX_LENGTH];
  
  /* get the name of the Option Menu */
  length = MAX_LENGTH - 1;
  TtaGiveTextAttributeValue (nameAttr, name, &length);
  if (name[0] != '\0')
    {
      /* there was a value for the NAME attribute. Now, process the
	 selected option elements */
      option = TtaGetFirstChild (menu);
      while (option)
	{
	elType = TtaGetElementType (option);
	if (elType.ElTypeNum == HTML_EL_Option)
	   SubmitOption (option, name, doc);
	else if (elType.ElTypeNum == HTML_EL_OptGroup)
	   {
	   child = TtaGetFirstChild (option);
	   while (child)
	      {
	      elType = TtaGetElementType (child);
	      if (elType.ElTypeNum == HTML_EL_Option)
	         SubmitOption (child, name, doc);
	      TtaNextSibling (&child);
	      }
	   }
	TtaNextSibling (&option);
      }
    }
}


/*----------------------------------------------------------------------
  ResetOption
  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void        ResetOption (Element option, ThotBool multipleSelects, ThotBool *defaultSelected, Document doc)
#else
static void        ResetOption (option,  multipleSelects, defaultSelected, doc)
Element		   option;
ThotBool		   multipleSelects;
ThotBool		  *defaultSelected;
Document	   doc;
#endif
{
   Attribute           attr, def;
   AttributeType       attrType;

   attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
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
#ifdef __STDC__
static void        ResetOptionMenu (Element menu, Document doc)
#else
static void        ResetOptionMenu (menu, doc)
Element		   menu;
Document	   doc;
#endif
{
   ElementType	       elType;
   Element             option, firstOption, child;
   Attribute           attr;
   AttributeType       attrType;
   ThotBool            multipleSelects, defaultSelected;

  /* reset according to the default attribute */
  attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), doc);
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
  while (option)
    {
      elType = TtaGetElementType (option);
      if (elType.ElTypeNum == HTML_EL_Option)
	 {
	 if (!firstOption)
	    firstOption = option;
         ResetOption (option, multipleSelects, &defaultSelected, doc);
	 }
      else if (elType.ElTypeNum == HTML_EL_OptGroup)
	 {
	 child = TtaGetFirstChild (option);
	 while (child)
	    {
	    elType = TtaGetElementType (child);
	    if (elType.ElTypeNum == HTML_EL_Option)
	       {
	       if (!firstOption)
		  firstOption = child;
	       ResetOption (child, multipleSelects, &defaultSelected, doc);
	       }
	    TtaNextSibling (&child);
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
#ifdef __STDC__
static void         ParseForm (Document doc, Element ancestor, Element el, int mode, ThotBool withinForm)
#else
static void         ParseForm (doc, ancestor el, mode, withinForm)
Document            doc;
Element             ancestor;
Element             el;
int                 mode;
ThotBool            withinForm;
#endif
{
  ElementType         elType;
  Element             elForm;
  Attribute           attr, attrS, def;
  AttributeType       attrType, attrTypeS;
  CHAR_T              name[MAX_LENGTH], *value = NULL;
  CHAR_T*             text;
  Language            lang;
  int                 length;
  int                 modified = FALSE;

  if (el)
    {
      if (mode == HTML_EL_Reset_Input)
	/* save current status of the document */
	modified = TtaIsDocumentModified (doc);
      
      lang = TtaGetDefaultLanguage ();      
      attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
      attrType.AttrTypeNum = HTML_ATTR_NAME;
      attrTypeS.AttrSSchema = attrType.AttrSSchema;
      TtaSearchAttribute (attrType, SearchForward, ancestor, &el, &attr);
      while (el != NULL && (!withinForm || TtaIsAncestor(el, ancestor)))
	{
	  if (attr != NULL)
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
			      value = TtaAllocString (length);
			      TtaGiveTextAttributeValue (attrS, value, &length);
			      AddNameValue (name, value);
			      TtaFreeMemory (value);
			      value = NULL;
			    }
			  else
			    /* give a default checkbox value (On) */
			    AddNameValue (name, TEXT("on"));
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
			      value = TtaAllocString (length);
			      TtaGiveTextAttributeValue (attrS, value, &length);
			      AddNameValue (name, value);
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
		      /* search the value in the Text_With_Frame element */
		      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
		      elForm = TtaSearchTypedElement (elType, SearchInTree, el);
		      /* save the NAME attribute of the element el */
		      length = MAX_LENGTH - 1;
		      TtaGiveTextAttributeValue (attr, name, &length);
		      AddElement (name);
		      AddToBuffer (TEXT("="));
		      while (elForm)
			{
			  length = TtaGetTextLength (elForm) + 1;
			  text = TtaAllocString (length);
			  TtaGiveTextContent (elForm, text, &length, &lang);
			  AddElement (text);
			  TtaFreeMemory (text);
			  elForm = TtaSearchTypedElementInTree (elType, SearchForward, el, elForm);
			}
		      AddToBuffer (TEXT("&"));
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
			  value = TtaAllocString (length);
			  TtaGiveTextAttributeValue (def, value, &length);
			}
		      else
			{
			  /* there's no default value */
			  value = TtaAllocString (1);
			  value[0] = EOS;
			}
		      /* search the value in the Text_With_Frame element */
		      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
		      elForm = TtaSearchTypedElement (elType, SearchInTree, el);
		      /* reset the value of the element */
		      if (elForm != NULL) 
			TtaSetTextContent (elForm, value, lang, doc);
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
			  value = TtaAllocString (length);
			  TtaGiveTextAttributeValue (def, value, &length);
			  AddNameValue (name, value);
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
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DoSubmit (Document doc, int method, STRING action)
#else
static void         DoSubmit (doc, method, action)
Document            doc;
int                 method;
STRING              action;

#endif
{
  int                 buffer_size;
  int                 i;
  STRING              urlName;

  /* remove any trailing & */
  if (buffer)
    buffer_size = ustrlen (buffer);
  else
    {
      buffer_size = 0;
      buffer = TEXT("");
    }
  if (buffer_size != 0  && (buffer[buffer_size - 1] == '&'))
    {
      buffer[buffer_size - 1] = EOS;
      buffer_size--;
    }

  switch (method)
    {
    case -9999:	/* index attribute, not yet supported by Amaya */
      for (i = 0; i < buffer_size; i++)
	switch (buffer[i])
	  {
	  case '&':
	  case '=':
	    buffer[i] = '+';
	    break;
	  default:
	    break;
	  }
      break;		/* case INDEX */
    case HTML_ATTR_METHOD_VAL_Get_:
      urlName = TtaAllocString (ustrlen (action) + buffer_size + 2);
      if (urlName != (STRING) NULL)
	{
	  ustrcpy (urlName, action);
	  GetHTMLDocument (urlName, buffer, doc, doc,
			   CE_FORM_GET, TRUE, NULL, NULL);
	  TtaFreeMemory (urlName);
	}
      break;
    case HTML_ATTR_METHOD_VAL_Post_:
      GetHTMLDocument (action, buffer, doc, doc,
		       CE_FORM_POST, TRUE, NULL, NULL);
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
#ifdef __STDC__
void                SubmitForm (Document doc, Element element)
#else
void                SubmitForm (doc, element)
Document            doc;
Element             element;

#endif
{
   Element             ancestor, elForm;
   ElementType         elType;
   Attribute           attr;
   AttributeType       attrType;
   STRING              action, name, value, info;
   int                 i, length, button_type;
   int                 method;
   ThotBool	       found, withinForm;

   buffer = (STRING) NULL;
   action = (STRING) NULL;

   /* find out the characteristics of the button which was pressed */
   found = FALSE;
   while (!found && element)
     {
        elType = TtaGetElementType (element);
	if (elType.ElTypeNum == HTML_EL_Reset_Input ||
	    elType.ElTypeNum == HTML_EL_Submit_Input ||
	    elType.ElTypeNum == HTML_EL_BUTTON ||
	    elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
	   found = TRUE;
	else
	   element = TtaGetParent (element);
     }
   if (!found)
     return;

   button_type = 0;
   attrType.AttrSSchema = elType.ElSSchema;
   switch (elType.ElTypeNum)
	      {
		 case HTML_EL_Reset_Input:
		    button_type = HTML_EL_Reset_Input;
		    break;

		 case HTML_EL_BUTTON:
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
			name = TtaAllocString (length + 3);
			TtaGiveTextAttributeValue (attr, name, &length);
			ustrcat (name, TEXT(". "));
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
			    AddNameValue (name, value);
			    /* create the y name-value pair */
			    name [length] = 'y';
			    value = &info[i+1];
			    AddNameValue (name, value);
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
       (elType.ElTypeNum == HTML_EL_BUTTON &&
	button_type == HTML_EL_Submit_Input))
	{
	    /* get the button's value and name, if they exist */
	    attrType.AttrTypeNum = HTML_ATTR_NAME;
	    attr = TtaGetAttribute (element, attrType);
	    if (attr != NULL)
	      {
		value = NULL;
		length = TtaGetTextAttributeLength (attr);
		name = TtaAllocString (length + 1);
		TtaGiveTextAttributeValue (attr, name, &length);
		attrType.AttrTypeNum = HTML_ATTR_Value_;
		attr = TtaGetAttribute (element, attrType);
		if (attr != NULL)
		  {
		  length = TtaGetTextAttributeLength (attr);
		  value = TtaAllocString (length + 1);
		  TtaGiveTextAttributeValue (attr, value, &length);
		  AddNameValue (name, value);
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
	   TtaFreeMemory (buffer);
	   return;
	 }
     }
   ancestor = elForm;

#ifdef FORM_DEBUG
{
  /* dump the abstract tree */
 FILE               *fp2;
 fp2 = fopen ("/tmp/FormTree.dbg", "w");
 TtaListAbstractTree(ancestor, fp2);
   fclose (fp2);
}
#endif

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
		action = TtaAllocString (length + 1);
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
   elForm  = TtaGetFirstChild(elForm);

   /* process the form */
   if (button_type == HTML_EL_Submit_Input)
     {
       if (action)
	 {
	   ParseForm (doc, ancestor, elForm, button_type, withinForm);   
	   DoSubmit (doc, method, action);
	 }
     }
   else
     ParseForm (doc, ancestor, elForm, button_type, withinForm);   
   
   if (action)
     TtaFreeMemory (action);
   if (buffer && buffer [0] != 0)
     TtaFreeMemory (buffer);
}


/*----------------------------------------------------------------------
   ActivateFileInput
   The user has triggered the Browse button associated with a File_Input
   element.  Display a file selector.
   ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ActivateFileInput (Document doc, Element el)
#else
void                ActivateFileInput (doc, el)
Document            doc;
Element             el;

#endif
{
	/******* TO DO *******/
}


/*----------------------------------------------------------------------
   SelectCheckbox
   selects a Checkbox input				
   ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectCheckbox (Document doc, Element el)
#else
void                SelectCheckbox (doc, el)
Document            doc;
Element             el;

#endif
{
   ElementType         elType;
   Attribute           attr;
   AttributeType       attrType;
   int                 modified;

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
#ifdef __STDC__
void                SelectOneRadio (Document doc, Element el)
#else
void                SelectOneRadio (doc, el)
Document            doc;
Element             el;

#endif
{
  ElementType         elType;
  Element             elForm;
  Attribute           attr, attrN;
  AttributeType       attrType, attrTypeN;
  int                 modified, length;
  CHAR_T              name[MAX_LENGTH], *buffer = NULL;

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

	  if (elForm != NULL)
	    {
	      /* search the first radio input */
	      elType.ElTypeNum = HTML_EL_Radio_Input;
	      elForm = TtaSearchTypedElement (elType, SearchInTree, elForm);
	      while (elForm != NULL)
		{
		  if (elForm != el)
		    {
		      /* compare its NAME attribute */
		      attrN = TtaGetAttribute (elForm, attrTypeN);
		      if (attrN != NULL)
			{
			  length = TtaGetTextAttributeLength (attrN) + 1;
			  buffer = TtaAllocString (length);
			  TtaGiveTextAttributeValue (attrN, buffer, &length);
			  if (!ustrcmp (name, buffer))
			    {
			      /* same NAME: set the checked attribute to NO */
			      attr = TtaGetAttribute (elForm, attrType);
			      if (attr != NULL
				  && TtaGetAttributeValue (attr) == HTML_ATTR_Checked_VAL_Yes_)
				{
				  TtaRemoveAttribute (elForm, attr, doc);
				  attr = TtaNewAttribute (attrType);
				  TtaAttachAttribute (elForm, attr, doc);
				  TtaSetAttributeValue (attr, HTML_ATTR_Checked_VAL_No_, elForm, doc);
				}
			    }
			  TtaFreeMemory (buffer);
			  buffer = NULL;
			}
		    }
		  /* search the next radio input */
		  elForm = TtaSearchTypedElement (elType, SearchForward, elForm);
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
  SelectInsertedText
  The user has clicked on a Frame element.
  if it's within a Button_Input, a Reset_Input or a Submit_Input, select
  the parent element.
  If it's within a Text_Area, a Text_Input, a File_Input or a Password_Input,
  put the caret at the end of the text element within the Inserted_Text
  element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            SelectInsertedText (NotifyElement * event)
#else  /* __STDC__ */
ThotBool            SelectInsertedText (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             textLeaf, parent;
   int		       length;
   ThotBool	       ret;

   /* search the first text leaf */
   parent = TtaGetParent(event->element);
   elType = TtaGetElementType (parent);
   if (elType.ElTypeNum == HTML_EL_Submit_Input ||
       elType.ElTypeNum == HTML_EL_Reset_Input ||
       elType.ElTypeNum == HTML_EL_Button_Input)
     {
     TtaSelectElement (event->document, parent);
     ret = TRUE;	/* prevent Thot from selecting the clicked element */
     }
   else
     {
     elType.ElTypeNum = HTML_EL_TEXT_UNIT;
     textLeaf = TtaSearchTypedElement (elType, SearchForward, parent);
     if (textLeaf != NULL && TtaIsAncestor(textLeaf, parent))
        {
	length = TtaGetTextLength (textLeaf);
        TtaSelectString (event->document, textLeaf, length+1, length);
        ret = TRUE;	/* prevent Thot from selecting the clicked element */
        }
     else
	ret = FALSE;
     }
   return ret;
}

/*----------------------------------------------------------------------
   SelectOneOption
   selects an option in option menu			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectOneOption (Document doc, Element el)
#else
void                SelectOneOption (doc, el)
Document            doc;
Element             el;
#endif
{
#ifdef _WINDOWS
  int                 nbOldEntries = 20;
#endif /* _WINDOWS */
#define MAX_OPTIONS 100
#define MAX_SUBOPTIONS 20
#define MAX_LABEL_LENGTH 50
  ElementType         elType, childType;
  Element	      elText, menuEl, child;
  Element             option[MAX_OPTIONS];
  Element	      subOptions[MAX_SUBMENUS][MAX_SUBOPTIONS];
  ThotBool	      selected[MAX_OPTIONS];
  ThotBool            subSelected[MAX_SUBMENUS][MAX_SUBOPTIONS];
  AttributeType       attrType;
  Attribute	      attr;
  SSchema	      htmlSch;
  CHAR_T              text[MAX_LABEL_LENGTH];
  CHAR_T              buffmenu[MAX_LENGTH];
  Language            lang;
  int                 length, nbitems, lgmenu, i, nbsubmenus, nbsubitems;
  int                 modified;
  ThotBool	      multipleOptions, sel;

  if (el == NULL)
    return;

#ifdef _WINDOWS  
   opDoc = doc;
#endif /* _WINDOWS */
   htmlSch = TtaGetSSchema (TEXT("HTML"), doc);
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
       lgmenu = 0;
       nbitems = 0;
       nbsubmenus = 0;
       elType.ElTypeNum = HTML_EL_Option_Menu;
       menuEl = TtaGetTypedAncestor (el, elType);
       if (menuEl != NULL)
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
	   while (nbitems < MAX_OPTIONS && el)
	     {
	       elType = TtaGetElementType (el);
	       if (elType.ElTypeNum == HTML_EL_OptGroup &&
		   elType.ElSSchema == htmlSch)
		 {
		   /* It's an OptGroup.A submenu has to be created later on */
		   if (nbsubmenus < MAX_SUBMENUS)
		     nbsubmenus++;
		   else
		     /* too many submenus. Ignore that OptGroup */
		     elType.ElTypeNum = 0;
		 }
	       if ((elType.ElTypeNum == HTML_EL_Option ||
		    elType.ElTypeNum == HTML_EL_OptGroup) &&
		   elType.ElSSchema == htmlSch)
		 {
		   option[nbitems] = el;
#ifdef _WINDOWS 
		   opOption[nbitems] = el;
#endif /* _WINDOWS */
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
		   if (attr)
		     TtaGiveTextAttributeValue (attr, text, &length);
		   else if (elType.ElTypeNum == HTML_EL_Option)
		     /* there is no label attribute, but it's an Option
			Take its content as the item label */
		     {
		       elText = TtaGetFirstChild (el);
		       if (elText)
			 TtaGiveTextContent (elText, text, &length, &lang);
		       else
			 length = 0;
		     }
		   else
		     length = 0;
		   /* count the EOS character */
		   text[length] = EOS;
		   length++;
		   /* we have to add the 'B', 'T' or 'M' character */
		   length++;
		   if (lgmenu + length < MAX_LENGTH)
		     /* add an item */
		     {
		       if (elType.ElTypeNum == HTML_EL_OptGroup)
			 usprintf (&buffmenu[lgmenu], TEXT("M%s"), text);
		       else if (multipleOptions)
			 usprintf (&buffmenu[lgmenu], TEXT("T%s"), text);
		       else
			 usprintf (&buffmenu[lgmenu], TEXT("B%s"), text);
		       nbitems++;
		     }
		   lgmenu += length;
		 }
	       TtaNextSibling (&el);
	     }
	   if (nbitems > 0)
	     {
	       /* create the main menu */
	       TtaNewPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1),
			    NULL, nbitems, buffmenu, NULL, 'L');
	       if (multipleOptions)
		 for (i = 0; i < nbitems; i++)
		   if (selected[i])
#ifdef _WINDOWS
		     WIN_TtaSetToggleMenu (BaseDialog + OptionMenu, i, TRUE, FrMainRef [currentFrame]);
#else  /* !_WINDOWS */
	       TtaSetToggleMenu (BaseDialog + OptionMenu, i, TRUE);
#endif /* _WINDOWS */
	       if (nbsubmenus > 0) {
		 /* There ia at least 1 OPTGROUP. Create submenus corresponding to OPTGROUPs */
		 nbitems = 0;	/* item number in main (SELECT) menu */
		 /* check all children of element SELECT */
		 el = TtaGetFirstChild (menuEl);
		 nbsubmenus = 0;
		 while (nbsubmenus < MAX_SUBMENUS && el) {
		   elType = TtaGetElementType (el);
		   if (elType.ElTypeNum == HTML_EL_Option && elType.ElSSchema == htmlSch)
		     /* this is an OPTION */
		     nbitems++;	/* item number in the main menu */
		   else if (elType.ElTypeNum == HTML_EL_OptGroup && elType.ElSSchema == htmlSch) {
		     /* this is an OPTGROUP.  Create the corresponding sub menu */
		     /* First, check all children of OPTGROUP */
		     child = TtaGetFirstChild (el);
		     lgmenu = 0;
		     nbsubitems = 0;
		     while (nbsubitems < MAX_SUBOPTIONS && child) {
		       childType = TtaGetElementType (child);
		       if (childType.ElTypeNum == HTML_EL_Option && childType.ElSSchema == htmlSch) {
			 /* it's an OPTION. Create a submenu item */
			 subOptions[nbsubmenus][nbsubitems] = child;
			 if (multipleOptions) {
			   attrType.AttrTypeNum = HTML_ATTR_Selected;
			   subSelected[nbsubmenus][nbsubitems] = (TtaGetAttribute (child, attrType) != NULL);
			 } 
			 /* get the item label */
			 attrType.AttrTypeNum = HTML_ATTR_label;
			 attr = TtaGetAttribute (child, attrType);
			 length = MAX_LABEL_LENGTH - 1;
			 if (attr) /* there is a label attribute. Take it */
			   TtaGiveTextAttributeValue (attr, text, &length);
			 else { /* take the element's content */
			   elText = TtaGetFirstChild (child);
			   if (elText)
			     TtaGiveTextContent (elText, text, &length, &lang);
			   else
			     length = 0;
			 } 
			 /* count the EOS character */
			 text[length] = EOS;
			 length++;
			 /* we have to add the 'B'or 'T' character */
			 length++;
			 if (lgmenu + length < MAX_LENGTH) { /* append that item to the buffer */
			   if (multipleOptions)
			     usprintf (&buffmenu[lgmenu], TEXT("T%s"), text);
			   else
			     usprintf (&buffmenu[lgmenu], TEXT("B%s"), text);
			   nbsubitems++;
			 } 
			 lgmenu += length;
		       } 
		       /* next child of OPTGROUP */
		       TtaNextSibling (&child);
		     }
		     /* All children of OPTGROUP have been checked. */
		     /* create the submenu */
#                               ifdef _WINDOWS
		     TtaNewSubmenu (BaseDialog + OptionMenu + (nbsubmenus * nbOldEntries) + 1, BaseDialog+OptionMenu, nbitems, NULL, nbsubitems, buffmenu, NULL, FALSE);
#                               else  /* !_WINDOWS */
		     TtaNewSubmenu (BaseDialog+OptionMenu+nbsubmenus+1, BaseDialog+OptionMenu, nbitems, NULL, nbsubitems, buffmenu, NULL, FALSE);
#                               endif /* _WINDOWS */
		     if (multipleOptions)
		       for (i = 0; i < nbsubitems; i++)
			 if (subSelected[nbsubmenus][i])
#                                         ifdef _WINDOWS
			   WIN_TtaSetToggleMenu (BaseDialog + OptionMenu + (nbsubmenus * nbOldEntries) + 1, i, TRUE, FrMainRef [currentFrame]);
#                                         else  /* !_WINDOWS */
		     TtaSetToggleMenu (BaseDialog+OptionMenu+nbsubmenus+1, i, TRUE);
#                                         endif /* _WINDOWS */
		     nbsubmenus++;
		     nbitems++;	/* item number in the main menu */
		   }  
		   /* Next child of SELECT */
		   TtaNextSibling (&el);
		 }
	       }
	       /* activate the menu that has just been created */
	       ReturnOption = -1;
	       ReturnOptionMenu = -1;
#                ifndef _WINDOWS
	       TtaSetDialoguePosition ();
#                endif /* !_WINDOWS */
	       TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
	       /* wait for an answer from the user */
	       TtaWaitShowDialogue ();
	       if (ReturnOption >= 0 && ReturnOptionMenu >= 0) {
		 /* make the returned option selected */
		 if (ReturnOptionMenu == 0) { /* an item in the main (SELECT) menu */
		   el = option[ReturnOption];
		   sel = selected[ReturnOption];
		 } else { /* an item in a submenu */
#                           ifdef _WINDOWS
		   /* el = subOptions[ReturnOptionMenu - nbOldEntries - 1][ReturnOption]; */
		   el = subOptions[ReturnOptionMenu / nbOldEntries ][ReturnOption];
		   sel = subSelected[ReturnOptionMenu / nbOldEntries][ReturnOption];
#                           else  /* _WINDOWS */
		   el = subOptions[ReturnOptionMenu - 1][ReturnOption];
		   sel = subSelected[ReturnOptionMenu - 1][ReturnOption];
#                           endif /* _WINDOWS */
		 }
		 modified = TtaIsDocumentModified (doc);	  
		 if (!multipleOptions)
		   OnlyOneOptionSelected (el, doc, FALSE);
		 else {
		   attrType.AttrTypeNum = HTML_ATTR_Selected;
		   attr = TtaGetAttribute (el, attrType);
		   if (sel)
		     TtaRemoveAttribute (el, attr, doc);
		   else {
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
	       } 
	     } 
	 } 
     } 
}
 
