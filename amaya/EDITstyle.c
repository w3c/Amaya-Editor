/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: D. Veillard
 *
 */


#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
 
/*----------------------------------------------------------------------
   
   CLASS MODIFICATION : USER INTERFACE AND INNER FUNCTIONS           
   
  ----------------------------------------------------------------------*/

#include "css_f.h"
#include "html2thot_f.h"
#include "HTMLstyle_f.h"
#include "UIcss_f.h"

/* CSSLEVEL2 adding new features to the standard */
/* DEBUG_STYLES verbose output of style actions */
/* DEBUG_CLASS_INTERF verbose output on class interface actions */

/*
 * specific data :
 *   The Class List contains the list of classe names needed for selection
 *                  by the user.
 *   NbClass is the corresponding number of elements found.
 *   CurrentClass contains the Class name selected by the user.
 *   ClassReference is the selected element used to update the class properties.
 *   DocReference is the selected document.
 */

static char         ClassList[50 * 80];
static int          NbClass = 0;

static char         CurrentClass[80];
static Element      ClassReference;
static Document     DocReference;

static char         AClassList[50 * 80];
static int          NbAClass = 0;

static char         CurrentAClass[80];
static Element      AClassFirstReference;
static Element      AClassLastReference;
static Document     ADocReference;


/*----------------------------------------------------------------------
   ApplyClassChange : Change all the presentation attributes of    
   the selected elements to reflect their new class                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyClassChange (Document doc)
#else  /* __STDC__ */
void                ApplyClassChange (doc)
Document            doc;

#endif /* __STDC__ */
{
   Element             cour;
   Attribute           at;
   AttributeType       atType;
   char               *class = CurrentAClass;

#ifdef DEBUG_STYLES
   fprintf (stderr, "ApplyClassChange(%d,%s)\n", doc, CurrentAClass);
#endif

   if (!class)
      return;

   /* remove any leading dot in a class definition. */
   if (*class == '.')
      class++;
   if (*class == 0)
      return;

   /* class default : suppress all specific presentation. */
   if (!strcmp (CurrentAClass, "default"))
     {
	cour = AClassFirstReference;
	while (cour != NULL)
	  {
	     /* remove any style attribute and update the presentation. */
	     RemoveStyle (cour, doc);

	     /* jump on next element until last one is reached. */
	     if (cour == AClassLastReference)
		break;
	     TtaNextSibling (&cour);
	  }
	/*
	RedisplayDocument (doc);
	 */
	return;
     }
   /* loop on each selected element. */
   cour = AClassFirstReference;
   while (cour != NULL)
     {
	/* remove any Style attribute left */
	RemoveStyle (cour, doc);

	/* set the Class attribute of the element. */

	atType.AttrSSchema = TtaGetDocumentSSchema (doc);
	if (!IsImplicitClassName (CurrentAClass, doc))
	  {
	     atType.AttrTypeNum = HTML_ATTR_Class;
	     at = TtaGetAttribute (cour, atType);
	     if (!at)
	       {
		  at = TtaNewAttribute (atType);
		  TtaAttachAttribute (cour, at, doc);
	       }
	     TtaSetAttributeText (at, CurrentAClass, cour, doc);
	  }
	/* jump on next element until last one is reached. */
	if (cour == AClassLastReference)
	   break;
	TtaNextSibling (&cour);
     }

   /*last, update the display.
   RedisplayDocument (doc);
    */
}

/*----------------------------------------------------------------------
   UpdateClass : Change a class to reflect the presentation        
   attributes of the selected elements                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateClass (Document doc)
#else  /* __STDC__ */
void                UpdateClass (doc)
Document            doc;

#endif /* __STDC__ */
{
   Attribute           at;
   AttributeType       atType;
   int                 len, base;
   char                stylestring[1000];
   char               *class;

   /* create a string containing the new CSS definition. */
   strcpy (&stylestring[0], CurrentClass);
   strcat (stylestring, " { ");
   base = len = strlen (stylestring);
   len = sizeof (stylestring) - len;
   len -= 4;
   GetHTML3StyleString (ClassReference, doc, &stylestring[base], &len);
   strcat (stylestring, "}");

   /* change the selected element to be of the new class. */
   RemoveStyle (ClassReference, doc);

   atType.AttrSSchema = TtaGetDocumentSSchema (doc);
   if (!IsImplicitClassName (CurrentClass, doc))
     {
	class = &CurrentClass[0];
	if (*class == '.')
	   class++;
	atType.AttrTypeNum = HTML_ATTR_Class;
	at = TtaGetAttribute (ClassReference, atType);
	if (!at)
	  {
	     at = TtaNewAttribute (atType);
	     TtaAttachAttribute (ClassReference, at, doc);
	  }
	TtaSetAttributeText (at, class, ClassReference, doc);
     }
   /* parse and apply this new CSS to the current document. */
   ParseHTMLStyleHeader (NULL, &stylestring[0], doc, TRUE);

   /* last, update the display.
   RedisplayDocument (doc);
    */
}

/*----------------------------------------------------------------------
   BuildClassList : Build the whole list of HTML class names in use  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 BuildClassList (Document doc, char *buf, int size, char *first)
#else  /* __STDC__ */
int                 BuildClassList (doc, buf, size, first)
Document            doc;
char               *buf;
int                 size;
char               *first;

#endif /* __STDC__ */
{
   ElementType         elType;
   Element             el;
   Attribute           at;
   AttributeType       atType;
   int                 free = size;
   int                 len;
   int                 nb = 0;
   int                 index = 0;
   char                val[100];
   int                 valen;
   int                 type;
   char               *ptr;

   /* ad the first element if specified. */
   buf[0] = 0;
   if (first)
     {
	strcpy (&buf[index], first);
	len = strlen (first);
	len++;
	free -= len;
	index += len;
	nb++;
     }
   elType.ElSSchema = TtaGetDocumentSSchema (doc);
   elType.ElTypeNum = HTML_EL_StyleRule;
   el = TtaSearchTypedElement (elType, SearchInTree, TtaGetMainRoot (doc));

   while (el != NULL)
     {
	atType.AttrSSchema = TtaGetDocumentSSchema (doc);
	atType.AttrTypeNum = HTML_ATTR_Selector;

	at = TtaGetAttribute (el, atType);
	if (at)
	  {
	     valen = 100;
	     TtaGiveTextAttributeValue (at, &val[0], &valen);

             /*
	      * if the selector uses # this is an ID so don't show it
	      * in the list. if there is a blank in the name, it's probably
	      * not a class name.
	      */
	     ptr = &val[0];
	     while ((*ptr != '\0') && (*ptr != '#') && (*ptr != ' ')) ptr++;
	     if (*ptr == '\0') {
		 /*
		  * Type name are not class names, remove them.
		  * Don't list the first field twice, too.
		  */
		 GIType(val, &type);
		 if ((type == 0) && (strcmp (val, first)))
		   {
		      len = free;
		      TtaGiveTextAttributeValue (at, &buf[index], &len);
		      len++;
		      free -= len;
		      index += len;
		      nb++;
		   }
	     }
	     
	  }
	/* get next StyleRule */
	TtaNextSibling (&el);
     }
#ifdef DEBUG_CLASS_INTERF
   fprintf (stderr, "BuildClassList : found %d class\n", nb);
#endif
   return (nb);
}

/*----------------------------------------------------------------------
   ChangeClass : Change a class to reflect the presentation        
   attributes of the selected element                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeClass (Document doc, View view)
#else  /* __STDC__ */
void                ChangeClass (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   Attribute           at;
   AttributeType       atType;
   int                 firstSelectedChar, lastSelectedChar, i, j;
   char                class[50];
   int                 len;
   char               *elHtmlName;
   Element             last_elem;

   DocReference = doc;
   CurrentClass[0] = 0;
   ClassReference = NULL;
   TtaGiveFirstSelectedElement (doc, &ClassReference,
				&firstSelectedChar, &lastSelectedChar);
   TtaGiveLastSelectedElement (doc, &last_elem, &i, &j);

   /* one can only define a style from one element at a time. */
   if (last_elem != ClassReference)
     {
#ifdef DEBUG_CLASS_INTERF
	fprintf (stderr, "first selected != last selected, first char %d, last %d\n",
		 firstSelectedChar, lastSelectedChar);
#endif
	return;
     }
   if (ClassReference == NULL)
      return;

   /* updating the class name selector. */
   elHtmlName = GetHTML3Name (ClassReference, doc);

   TtaNewForm (BaseDialog + ClassForm, TtaGetViewFrame (doc, 1), 
	       TtaGetMessage (AMAYA, AM_DEF_CLASS), FALSE, 2, 'L', D_DONE);
   NbClass = BuildClassList (doc, ClassList, sizeof (ClassList), elHtmlName);
   TtaNewSelector (BaseDialog + ClassSelect, BaseDialog + ClassForm,
		   TtaGetMessage (AMAYA, AM_SEL_CLASS),
		   NbClass, ClassList, 5, NULL, TRUE, TRUE);

   /* preselect the entry corresponding to the class of the element. */
   atType.AttrSSchema = TtaGetDocumentSSchema (doc);
   atType.AttrTypeNum = HTML_ATTR_Class;

   at = TtaGetAttribute (ClassReference, atType);
   if (at)
     {
	len = 50;
	TtaGiveTextAttributeValue (at, class, &len);
	TtaSetSelector (BaseDialog + ClassSelect, -1, class);
	strcpy (CurrentClass, class);
     }
   else
     {
	TtaSetSelector (BaseDialog + ClassSelect, 0, NULL);
	strcpy (CurrentClass, elHtmlName);
     }

   /* pop-up the dialogue box. */
   TtaShowDialogue (BaseDialog + ClassForm, TRUE);
}

/*----------------------------------------------------------------------
   ApplyClass : Use a class to change the presentation             
   attributes of the selected elements                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyClass (Document doc, View view)
#else  /* __STDC__ */
void                ApplyClass (doc, view)
Document            doc;
View                view;

#endif /* __STDC__ */
{
   Attribute           at;
   AttributeType       atType;
   int                 firstSelectedChar, lastSelectedChar, i, j;
   Element             cour, parent;
   char                class[50];
   int                 len;

   ADocReference = doc;
   CurrentAClass[0] = 0;
   AClassFirstReference = NULL;
   AClassLastReference = NULL;

   /* a class can be applied to many elements. */
   TtaGiveFirstSelectedElement (doc, &AClassFirstReference,
				&firstSelectedChar, &lastSelectedChar);

   if (AClassFirstReference == NULL)
      return;
   cour = AClassLastReference = AClassFirstReference;
   do
     {
	TtaGiveNextSelectedElement (doc, &cour, &i, &j);
	if (cour != NULL)
	   AClassLastReference = cour;
     }
   while (cour != NULL);


   /* Case of a substring : need to split the original text. */
   if ((AClassFirstReference == AClassLastReference) &&
       (firstSelectedChar != 0))
     {
	int                 len = TtaGetTextLength (AClassFirstReference);

	if (len <= 0)
	   return;
	if (lastSelectedChar < len)
	   TtaSplitText (AClassFirstReference, lastSelectedChar, doc);
	if (firstSelectedChar > 1)
	  {
	     firstSelectedChar--;
	     TtaSplitText (AClassFirstReference, firstSelectedChar, doc);
	     TtaNextSibling (&AClassFirstReference);
	     AClassLastReference = AClassFirstReference;
	  }
     }
   if (AClassFirstReference == NULL)
      return;

   /* Case of all child of an element are selected, select the parent instead */
   parent = TtaGetParent(AClassFirstReference);
   if ((parent == TtaGetParent(AClassLastReference)) &&
       (AClassFirstReference == TtaGetFirstChild(parent)) &&
       (AClassLastReference == TtaGetLastChild(parent))) { 
       int select_parent = TRUE;
       ElementType elType;
       int len;

       elType = TtaGetElementType(AClassFirstReference);
       if (elType.ElTypeNum == HTML_EL_TEXT_UNIT) {
          if (firstSelectedChar > 1) select_parent = FALSE;
       }

       elType = TtaGetElementType(AClassLastReference);
       if (elType.ElTypeNum == HTML_EL_TEXT_UNIT) {
	  len = TtaGetTextLength (AClassLastReference);
          if (lastSelectedChar < len) select_parent = FALSE;
       }

       if (select_parent) {
           AClassFirstReference = AClassLastReference = parent;
	   firstSelectedChar = lastSelectedChar = 0;
       }
   }

   /* updating the class name selector. */
   TtaNewForm (BaseDialog + AClassForm, TtaGetViewFrame (doc, 1), 
	       TtaGetMessage (AMAYA, AM_APPLY_CLASS), TRUE, 2, 'L', D_DONE);
   NbAClass = BuildClassList (doc, AClassList, sizeof (AClassList), "default");
   TtaNewSelector (BaseDialog + AClassSelect, BaseDialog + AClassForm,
		   TtaGetMessage (AMAYA, AM_SEL_CLASS),
		   NbAClass, AClassList, 5, NULL, FALSE, TRUE);

   /* preselect the entry corresponding to the class of the element. */
   atType.AttrSSchema = TtaGetDocumentSSchema (doc);
   atType.AttrTypeNum = HTML_ATTR_Class;

   at = TtaGetAttribute (AClassFirstReference, atType);
   if (at)
     {
	len = 50;
	TtaGiveTextAttributeValue (at, class, &len);
	TtaSetSelector (BaseDialog + AClassSelect, -1, class);
	strcpy (CurrentAClass, class);
     }
   else
     {
	TtaSetSelector (BaseDialog + AClassSelect, 0, NULL);
	strcpy (CurrentAClass, "default");
     }

   /* pop-up the dialogue box. */
   TtaShowDialogue (BaseDialog + AClassForm, TRUE);
}

/*----------------------------------------------------------------------
   StyleCallbackDialogue : procedure for style dialogue events        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                StyleCallbackDialogue (int ref, int typedata, char *data)
#else  /* __STDC__ */
void                StyleCallbackDialogue (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
  int               val;

  val = (int) data;
#ifdef DEBUG_CLASS_INTERF
  if (typedata == INTEGER_DATA)
    fprintf (stderr, "StyleCallbackDialogue(%d,%d) \n", ref, (int) data);
  else if (typedata == STRING_DATA)
    fprintf (stderr, "StyleCallbackDialogue(%d,\"%s\") \n", ref, (char *) data);
#endif

  switch (ref - BaseDialog)
    {
    case ClassForm:
      if (typedata == INTEGER_DATA && val == 1)
	UpdateClass (DocReference);
      TtaDestroyDialogue (BaseDialog + ClassForm);
      break;
    case ClassSelect:
      if (typedata == STRING_DATA)
	strcpy (CurrentClass, data);
      break;
    case AClassForm:
      if (typedata == INTEGER_DATA && val == 1)
	ApplyClassChange (ADocReference);
      TtaDestroyDialogue (BaseDialog + AClassForm);
      break;
    case AClassSelect:
      if (typedata == STRING_DATA)
	strcpy (CurrentAClass, data);
      break;
    default:
      break;
    }
}
