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
 * a Submit or Reset button, the subtree of the form will be printed
 * out to /tmp/FormTree.dbg.
 *
 * Authors: J. Kahan, I. Vatton
 *
 */

/*
#define FORM_DEBUG
*/

/* Included headerfiles */
#define THOT_EXPORT extern

#include "amaya.h"

#define PARAM_INCREMENT 50

#include "init_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "HTMLform_f.h"

static char        *buffer;    /* temporary buffer used to build the query
				  string */
static int          lgbuffer;  /* size of the temporary buffer */
static int          documentStatus;
 
extern char               *GetActiveImageInfo (Document document, Element element);

#ifdef _WINDOWS 
Document  opDoc;
Element   opOption [200];
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/ 
#ifdef __STDC__
boolean SaveDocumentStatus (NotifyOnTarget *event)
#else /* __STDC__*/
boolean SaveDocumentStatus (event)
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
}
 
 
/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/ 
#ifdef __STDC__
boolean DeleteText (NotifyElement *event)
#else /* __STDC__*/
boolean DeleteText (event)
     NotifyElement *event;
#endif /* __STDC__*/
{
  ElementType         elType;
  Element             parent;
  boolean             done;

  parent = TtaGetParent (event->element);
  /* let Thot perform normal operation */
  done = FALSE;
  if (parent != NULL)
    {
      elType = TtaGetElementType (parent);
      if (elType.ElTypeNum == HTML_EL_Text_Input
	  || elType.ElTypeNum == HTML_EL_Text_Area)
	{
	  TtaDeleteTree (parent, event->document);
	  /* don't let Thot perform normal operation */
	  done = FALSE;
	}
      else
	{
	  parent = TtaGetParent (parent);
	  if (parent != NULL)
	    {
	      elType = TtaGetElementType (parent);
	      if (elType.ElTypeNum == HTML_EL_Text_Input
		  || elType.ElTypeNum == HTML_EL_Text_Area)
		{
		  TtaDeleteTree (parent, event->document);
		  /* don't let Thot perform normal operation */
		  done = FALSE;
		}
	    }
	}
    }
  return (done);
}

/*----------------------------------------------------------------------
  EscapeChar
  writes the equivalent escape code of a car in a string		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EscapeChar (char *string, unsigned char c)
#else
static void         EscapeChar (string, c)
char               *string;
unsigned char       c;

#endif
{
   c &= 0xFF;			/* strange behavior under solaris? */
   sprintf (string, "%02x", (unsigned int) c);
}

/*----------------------------------------------------------------------
  AddToBuffer
  reallocates memory and concatenates a string into buffer	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddToBuffer (char *orig)
#else
static void         AddToBuffer (orig)
char               *orig;

#endif
{
   void               *status;
   int                 lg;

   lg = strlen (orig) + 1;
   if (strlen (buffer) + lg > lgbuffer)
     {
	/* it is necessary to extend the buffer */
	if (lg < PARAM_INCREMENT)
	   lg = PARAM_INCREMENT;
	status = TtaRealloc (buffer, sizeof (char) * (lgbuffer + lg));

	if (status != NULL)
	  {
	     buffer = status;
	     lgbuffer += lg;
	     strcat (buffer, orig);
	  }
     }
   else
      strcat (buffer, orig);
}


/*----------------------------------------------------------------------
  AddElement
  add a string into the query buffer				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddElement (unsigned char *element)
#else
static void         AddElement (element)
unsigned char      *element;

#endif
{
   char                tmp[4] = "%";
   char                tmp2[2] = "a";

   if (buffer == (char *) NULL)
     {
	buffer = (char *) TtaGetMemory (PARAM_INCREMENT);
	lgbuffer = PARAM_INCREMENT;
	buffer[0] = EOS;
     }
   while (*element)
     {
	/* for valid standard ASCII chars */
	if (*element >= 0x20 && *element <= 0x7e)
	  {
	     /* verify whether the char must be escaped */
	     switch (*element)
		   {
		      case SPACE:
		      case '+':
		      case '&':
                      case '>':
                      case '<':
			 EscapeChar (&tmp[1], *element);
			 AddToBuffer (tmp);
			 break;

		      default:
			 tmp2[0] = *element;
			 AddToBuffer (tmp2);
			 break;
		   }
	  }
	/* for all other characters */
	else
	  {
	     EscapeChar (&tmp[1], *element);
	     AddToBuffer (tmp);
	  }

	element++;
     }
}

/*----------------------------------------------------------------------
  AddNameValue
  add a name=value pair, and a trailling & into the query buffer	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddNameValue (char *name, char *value)
#else
static void         AddNameValue (name, value)
char               *name, *value,
#endif
{
   AddElement (name);
   AddToBuffer ("=");
   if (value)
      AddElement (value);
   AddToBuffer ("&");
}

/*----------------------------------------------------------------------
   ParseForm
   traverses the tree of element, applying the parse_input 
   function to each element with an attribute NAME                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ParseForm (Document doc, Element ancestor, Element el, int mode)
#else
static void         ParseForm (doc, ancestor el, mode)
Document            doc;
Element             ancestor;
Element             el;
int                 mode;

#endif
{
   ElementType         elType;
   Element             elForm;
   Attribute           attr, attrS, def;
   AttributeType       attrType, attrTypeS;
   int                 length;
   char                name[MAX_LENGTH], value[MAX_LENGTH];
   boolean             multipleSelects, defaultSelected;
   int                 modified = FALSE;
   Language            lang;

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
	while (el != NULL && TtaIsAncestor(el, ancestor))
	  {
	     if (attr != NULL)
	       {
		  elType = TtaGetElementType (el);
		  switch (elType.ElTypeNum)
			{
			   case HTML_EL_Option_Menu:
			      if (mode == HTML_EL_Submit_Input)
				{
				  /* get the name of the Option Menu */
				  length = MAX_LENGTH - 1;
				  TtaGiveTextAttributeValue (attr, name, &length);
				  if (name[0] != '\0')
				    {
				      /* there was a value for the NAME attribute. Now, process the
					 selected option elements */
				      elForm = TtaGetFirstChild (el);
				      while (elForm) {
					/* check if element is selected */
					attrTypeS.AttrTypeNum = HTML_ATTR_Selected;
					attrS = TtaGetAttribute (elForm, attrTypeS);					     
					if (attrS != NULL && TtaGetAttributeValue (attrS) == HTML_ATTR_Selected_VAL_Yes_)
					  {
					    attrTypeS.AttrTypeNum = HTML_ATTR_Value_;
					    attrS = TtaGetAttribute (elForm, attrTypeS);
					    if (attrS != NULL)
					      {
						/* there's an explicit value */
						length = MAX_LENGTH - 1;
						TtaGiveTextAttributeValue (attrS, value, &length);
					      }
					    else
					      {
						/* use the attached text as an implicit value */
						elForm  = TtaGetFirstChild(elForm);
						length = MAX_LENGTH - 1;
						TtaGiveTextContent (elForm, value, &length, &lang);
						elForm = TtaGetParent (elForm);
					      }
					    /* save the name/value pair of the element */
					    AddNameValue (name, value);
					  }
					TtaNextSibling (&elForm);
				      }
				    }
				}

			      else if (mode == HTML_EL_Reset_Input)
				{
				  /* reset according to the default attribute */
				  attrTypeS.AttrTypeNum = HTML_ATTR_Multiple;
				  attrS = TtaGetAttribute (el, attrTypeS);
				  if (attrS != NULL &&
				      TtaGetAttributeValue (attrS) == HTML_ATTR_Multiple_VAL_Yes_)
				    /* it's a multiple selects menu */
				    multipleSelects = TRUE;
				  else
				    multipleSelects = FALSE;

				  /* reset/set each option of the menu */
				  defaultSelected = FALSE;
				  elForm = TtaGetFirstChild (el);
				  /* el points to the rootf of the SELECT subtree */
				  while (elForm)
				    {
				      attrTypeS.AttrTypeNum = HTML_ATTR_DefaultSelected;
				      def = TtaGetAttribute (elForm, attrTypeS);
				      attrTypeS.AttrTypeNum = HTML_ATTR_Selected;
				      attrS = TtaGetAttribute (elForm, attrTypeS);
				      if (def == NULL) 
					{
					  /* not a default option, so remove it */
					  if(attrS != NULL)
					    TtaRemoveAttribute (elForm, attrS, doc);
					}
				      else if (def != NULL && !multipleSelects && defaultSelected) 
					  {
					  /* a default option, but multiple default options are not allowed and
					     one other option has already been selected */
					    if(attrS != NULL)
					      TtaRemoveAttribute (elForm, attrS, doc);
					  }
				      else if (def != NULL && (multipleSelects || (!multipleSelects && !defaultSelected)))
					{
					  /* a default option and it may be selected */
					  if (attrS == NULL)
					    {
					      /* create a new selected attribute */
					      attrS = TtaNewAttribute (attrTypeS);
					      TtaAttachAttribute (elForm, attrS, doc);
					      TtaSetAttributeValue (attrS, HTML_ATTR_Selected_VAL_Yes_, elForm, doc);
					    }
					  defaultSelected = TRUE;
					}
				      TtaNextSibling (&elForm);
				    }

				  if (defaultSelected == FALSE)
				    {
				      /* there's no explicit default option, so select the first option of the menu */
				      elForm = TtaGetFirstChild (el);
				      if (elForm)
					{
					  /* select the option if it's not already selected */
					  attrTypeS.AttrTypeNum = HTML_ATTR_Selected;
					  attrS = TtaGetAttribute (elForm, attrTypeS);
					  if (attrS == NULL)
					    {
					      /* create a new selected attribute */
					      attrS = TtaNewAttribute (attrTypeS);
					      TtaAttachAttribute (elForm, attrS, doc);
					      TtaSetAttributeValue (attrS, HTML_ATTR_Selected_VAL_Yes_, elForm, doc);
					    }
					}
				    }

				  if (!multipleSelects) {
				    /* call the parser to check the default selections menu ? */
				    attrTypeS.AttrTypeNum = HTML_ATTR_DefaultSelected;
				    TtaSearchAttribute (attrTypeS, SearchInTree, el, &elForm, &attrS);
				    if (elForm != NULL)
				      /* Reset according to the default attribute */
				      OnlyOneOptionSelected (elForm, doc, FALSE);
				  }
				}
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
					     length = MAX_LENGTH - 1;
					     TtaGiveTextAttributeValue (attrS, value, &length);
					     AddNameValue (name, value);
					  }
					else
					  /* give a default checkbox value (On) */
					  AddNameValue (name, "on");
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
					     length = MAX_LENGTH - 1;
					     TtaGiveTextAttributeValue (attrS, value, &length);
					     AddNameValue (name, value);
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
			   case HTML_EL_Password_Input:
			      if (mode == HTML_EL_Submit_Input)
				{
				  /* search the value in the Text_With_Frame element */
				  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
				  elForm = TtaSearchTypedElement (elType, SearchInTree, el);
				  if (elForm)
				     {
					/* save the NAME attribute of the element el */
					length = MAX_LENGTH - 1;
					TtaGiveTextAttributeValue (attr, name, &length);
					/* save the VALUE attribute of the element el */
					length = MAX_LENGTH - 1;
					TtaGiveTextContent (elForm, value, &length, &lang);
					AddNameValue (name, value);
				     }
				}
			      else if (mode == HTML_EL_Reset_Input)
				{
				   /* Reset according to the default attribute */
				  /* gets the default value */
				   attrTypeS.AttrTypeNum = HTML_ATTR_Default_Value;
				   def = TtaGetAttribute (el, attrTypeS);
				   if (def != NULL)
				     {
					length = MAX_LENGTH - 1;
					TtaGiveTextAttributeValue (def, value, &length);
				     }
				   else
				     /* there's no default value */
				     value[0] = EOS;
				   /* search the value in the Text_With_Frame element */
				   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
				   elForm = TtaSearchTypedElement (elType, SearchInTree, el);
				   /* reset the value of the element */
				   if (elForm != NULL) 
				     TtaSetTextContent (elForm, value, lang, doc);
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
					length = MAX_LENGTH - 1;
					TtaGiveTextAttributeValue (def, value, &length);
					AddNameValue (name, value);
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
	      TtaSetDocumentUnmodified (doc);
     }
}

/*----------------------------------------------------------------------
  DoSubmit
  submits a form : builds the query string and sends the request	       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DoSubmit (Document doc, int method, char *action)
#else
static void         DoSubmit (doc, method, action)
Document            doc;
int                 method;
char               *action;

#endif
{
   int                 buffer_size;
   int                 i;
   char               *urlName;

   /* remove any trailing & */
   if (buffer)
      buffer_size = strlen (buffer);
   else
     {
       buffer_size = 0;
       buffer = "";
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
			}	/* switch */
	       break;		/* case INDEX */
	    case HTML_ATTR_METHOD_VAL_Get_:
	       urlName = TtaGetMemory (strlen (action) + buffer_size + 2);
	       if (urlName != (char *) NULL)
		 {
		    strcpy (urlName, action);
		    strcat (urlName, "?");
		    if (buffer_size)
		      strcat (urlName, buffer);
		    GetHTMLDocument (urlName, NULL, doc, doc, CE_FORM_GET, TRUE, FALSE, FALSE);
		    TtaFreeMemory (urlName);
		 }
	       break;
	    case HTML_ATTR_METHOD_VAL_Post_:
	       GetHTMLDocument (action, buffer, doc, doc, CE_FORM_POST, TRUE, NULL, NULL);
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
   int                 i, length, button_type;
   char               *action, *name, *value, *info;
   int                 method;

   buffer = (char *) NULL;
   action = (char *) NULL;

   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);

   /* find out the characteristics of the button which was pressed */
   button_type = 0;
   elForm = element;
   while (!button_type)
     {
	elType = TtaGetElementType (elForm);
	switch (elType.ElTypeNum)
	      {
		 case HTML_EL_Reset_Input:
		    button_type = HTML_EL_Reset_Input;
		    break;

		 case HTML_EL_Submit_Input:
		    button_type = HTML_EL_Submit_Input;

		    /* get the button's value and name, if they exist */
		    attrType.AttrTypeNum = HTML_ATTR_NAME;
		    attr = TtaGetAttribute (elForm, attrType);
		    if (attr != NULL)
		      {
			value = NULL;
			length = TtaGetTextAttributeLength (attr);
			name = TtaGetMemory (length + 1);
			TtaGiveTextAttributeValue (attr, name, &length);
			attrType.AttrTypeNum = HTML_ATTR_Value_;
			attr = TtaGetAttribute (elForm, attrType);
			if (attr != NULL) {
			  length = TtaGetTextAttributeLength (attr);
			  value = TtaGetMemory (length + 1);
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
		    break;

		 case HTML_EL_File_Input:
		    /* not supported for the moment */
		    button_type = HTML_EL_File_Input;
		    return;

	      case HTML_EL_PICTURE_UNIT:
		button_type = HTML_EL_Submit_Input;
		    /* get the button'spair of name and values, if they
		       exist */
		    attrType.AttrTypeNum = HTML_ATTR_NAME;
		    attr = TtaGetAttribute (elForm, attrType);
		    if (attr != NULL)
		      {
			length = TtaGetTextAttributeLength (attr);
			name = TtaGetMemory (length + 3);
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
		 default:
		    elForm = TtaGetParent (elForm);
		    break;
	      }
     }

   /* find the parent form node */
   elType.ElTypeNum = HTML_EL_Form;
   elType.ElSSchema = TtaGetDocumentSSchema (doc);
   elForm = TtaGetTypedAncestor (element, elType);
   if (elForm == NULL) {
     if (buffer)
       TtaFreeMemory (buffer);
	/* could not find a form ancestor */
	return;
   }
   else
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
	length = TtaGetTextAttributeLength (attr);
	if (length) {
	  action = TtaGetMemory (length + 1);
	  TtaGiveTextAttributeValue (attr, action, &length);
	}

	/* get the  METHOD attribute value */
	attrType.AttrTypeNum = HTML_ATTR_METHOD;
	attr = TtaGetAttribute (elForm, attrType);
	if (attr == NULL)
	   method = HTML_ATTR_METHOD_VAL_Get_;
	else
	   method = TtaGetAttributeValue (attr);
     }

   /* search the subtree for the form elements */
   elForm  = TtaGetFirstChild(elForm);

   /* process the form */
   if (button_type == HTML_EL_Submit_Input) {
     if (action) {
       ParseForm (doc, ancestor, elForm, button_type);   
       DoSubmit (doc, method, action);
     }
   } else
     ParseForm (doc, ancestor, elForm, button_type);   
   
   if (action)
     TtaFreeMemory (action);
   if (buffer)
     TtaFreeMemory (buffer);
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
	   TtaSetDocumentUnmodified (doc);
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
   char                name[MAX_LENGTH], buffer[MAX_LENGTH];

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
				 length = MAX_LENGTH - 1;
				 TtaGiveTextAttributeValue (attrN, buffer, &length);
				 if (!strcmp (name, buffer))
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
			      }
			 }
		       /* search the next radio input */
		       elForm = TtaSearchTypedElement (elType, SearchForward, elForm);
		    }
	       }
	     if (!modified)
		TtaSetDocumentUnmodified (doc);
	  }
     }
}


/*----------------------------------------------------------------------
  SelectIncludedText
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             SelectIncludedText (NotifyElement * event)
#else  /* __STDC__ */
boolean             SelectIncludedText (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el;
   int		       length;

   /* search the first text leaf */
   elType = TtaGetElementType (event->element);
   elType.ElTypeNum = HTML_EL_TEXT_UNIT;
   el = TtaSearchTypedElement (elType, SearchForward, event->element);
   if (el != NULL && TtaIsAncestor(el, event->element))
     {
	length = TtaGetTextLength (el);
        TtaSelectString (event->document, el, length+1, length);
	/* refuse to select the clicked element */
        return TRUE;
     }
   else
	return FALSE;
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
   ElementType         elType;
   Element             option[200], elText;
   int                 length, nbitems, lgmenu;
   char                text[200];
   char                buffmenu[MAX_LENGTH];
   Language            lang;
   int                 modified;

   if (el == NULL)
      return;

#  ifdef _WINDOWS  
   opDoc = doc;
#  endif /* _WINDOWS */

   /* search the option element */
   elType = TtaGetElementType (el);
   while (elType.ElTypeNum != HTML_EL_BODY && elType.ElTypeNum != HTML_EL_Option)
     {
	el = TtaGetParent (el);
	elType = TtaGetElementType (el);
     }

   if (elType.ElTypeNum == HTML_EL_Option)
     {
	/* create the option menu */
	lgmenu = 0;
	nbitems = 0;
	el = TtaGetParent (el);
	if (el != NULL)
	  {
	     el = TtaGetFirstChild (el);
	     while (nbitems < 200 && el != NULL)
	       {
		  elType = TtaGetElementType (el);
		  if (elType.ElTypeNum == HTML_EL_Option)
		    {
		       option[nbitems] = el;
#              ifdef _WINDOWS 
               opOption[nbitems] = el;
#              endif /* _WINDOWS */
		       length = 200;
		       elText = TtaGetFirstChild (el);
		       TtaGiveTextContent (elText, text, &length, &lang);
		       if (length >= 50)
			 {
			    /* CHKR_LIMIT on entry name to 50 characters */
			    length = 50;
			    text[length - 1] = EOS;
			 }
		       else
			  length++;
		       length++;	/* we have to add the 'B' character */
		       if (lgmenu + length < MAX_LENGTH)
			 {
			    /* add an item */
			    sprintf (&buffmenu[lgmenu], "B%s", text);
			    nbitems++;
			 }
		       lgmenu += length;
		    }
		  TtaNextSibling (&el);
	       }
	     if (nbitems > 0)
	       {
		  ReturnOption = -1;
		  TtaNewPopup (BaseDialog + OptionMenu, TtaGetViewFrame (doc, 1),
			       "", nbitems, buffmenu, NULL, 'L');
		  TtaSetDialoguePosition ();
		  TtaShowDialogue (BaseDialog + OptionMenu, FALSE);
		  /* wait for an answer */
		  TtaWaitShowDialogue ();
		  if (ReturnOption >= 0)
		    {
		       /* make the returned option selected */
		       el = option[ReturnOption];
		       modified = TtaIsDocumentModified (doc);
		       OnlyOneOptionSelected (el, doc, FALSE);
		       if (!modified)
			  TtaSetDocumentUnmodified (doc);
		    }
	       }
	  }
     }
}










