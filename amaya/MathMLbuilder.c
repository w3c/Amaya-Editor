/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *
 * MathMLbuilder
 *
 * Author: V. Quint
 */


#define THOT_EXPORT extern
#include "amaya.h"

#include "Mathedit_f.h"
#include "XMLparser_f.h"

#include "MathML.h"
#include "parser.h"

#define EOS     '\0'
#define SPACE    ' '

typedef unsigned char MathEntityName[20];
typedef struct _MathEntity
  {			 /* a Math entity representing an operator char */
     MathEntityName      MentityName;	/* entity name */
     int                 charCode;	/* decimal code of char */
     char		 alphabet;	/* 'L' = ISO-Latin-1, 'G' = Symbol */
  }
MathEntity;

static MathEntity        MathEntityTable[] =
{
   /* This table MUST be in alphabetical order */
   /* This table contains characters from the Symbol font plus some
      specific MathML entities */
   {"Agr", 65, 'G'},
   {"ApplyFunction", 32, 'L'},  /* render as white space */
   {"Bgr", 66, 'G'},
   {"Cap", 199, 'G'},
   {"Cup", 200, 'G'},
   {"Delta", 68, 'G'},
   {"EEgr", 72, 'G'},
   {"Egr", 69, 'G'},
   {"Gamma", 71, 'G'},
   {"Igr", 73, 'G'},
   {"Integral", 242, 'G'},
   {"InvisibleTimes", 0, SPACE},
   {"Kgr", 75, 'G'},
   {"KHgr", 67, 'G'},
   {"Lambda", 76, 'G'},
   {"LeftArrow", 172, 'G'},
   {"Mgr", 77, 'G'},
   {"Ngr", 78, 'G'},
   {"Ogr", 79, 'G'},
   {"Omega", 87, 'G'},
   {"PI", 213, 'G'},
   {"Phi", 70, 'G'},
   {"Pi", 80, 'G'},
   {"PlusMinus", 177, 'G'},
   {"Psi", 89, 'G'},
   {"Rgr", 82, 'G'},
   {"RightArrow", 174, 'G'},
   {"Sigma", 83, 'G'},
   {"Sol", 164, 'G'},
   {"Sum", 229, 'G'},
   {"Tgr", 84, 'G'},
   {"Theta", 81, 'G'},
   {"Upsi", 85, 'G'},
   {"Upsi1", 161, 'G'},
   {"Verbar", 189, 'G'},
   {"Xi", 88, 'G'},
   {"Zgr", 90, 'G'},
   {"af", 32, 'L'},             /* render as white space */
   {"aleph", 192, 'G'},
   {"alpha", 97, 'G'},
   {"and", 217, 'G'},
   {"angle", 208, 'G'},
   {"ap", 187, 'G'},
   {"beta", 98, 'G'},
   {"bottom", 94, 'G'},
   {"bull", 183, 'G'},
   {"cap", 199, 'G'},
   {"chi", 99, 'G'},
   {"clubs", 167, 'G'},
   {"cong", 64, 'G'},
   {"copysf", 211, 'G'},
   {"copyssf", 227, 'G'},
   {"cr", 191, 'G'},
   {"cup", 200, 'G'},
   {"darr", 175, 'G'},
   {"dArr", 223, 'G'},
   {"dd", 100, 'L'},
   {"deg", 176, 'G'},
   {"delta", 100, 'G'},
   {"diams", 168, 'G'},
   {"divide", 184, 'G'},
   {"dtri", 209, 'G'},
   {"ee", 101, 'L'},
   {"empty", 198, 'G'},
   {"emsp", 32, 'G'},
   {"epsiv", 101, 'G'},
   {"equiv", 186, 'G'},
   {"eta", 104, 'G'},
   {"exist", 36, 'G'},
   {"florin", 166, 'G'},
   {"forall", 34, 'G'},
   {"gamma", 103, 'G'},
   {"ge", 179, 'G'},
   {"gt", 62, 'L'},
   {"hearts", 169, 'G'},
   {"horbar", 190, 'G'},
   {"ifraktur", 225, 'G'},
   {"infin", 165, 'G'},
   {"int", 242, 'G'},
   {"iota", 105, 'G'},
   {"isin", 206, 'G'},
   {"it", 242, 'G'},
   {"kappa", 107, 'G'},
   {"lambda", 108, 'G'},
   {"lang", 225, 'G'},
   {"larr", 172, 'G'},
   {"lArr", 220, 'G'},
   {"le", 163, 'G'},
   {"lowbar", 95, 'G'},
   {"loz", 224, 'G'},
   {"lrarr", 171, 'G'},
   {"lrArr", 219, 'G'},
   {"lsqb", 91, 'G'},
   {"lt", 62, 'L'},
   {"middot", 215, 'G'},
   {"mldr", 188, 'G'},
   {"mu", 109, 'G'},
   {"ne", 185, 'G'},
   {"not", 216, 'G'},
   {"notin", 207, 'G'},
   {"nu", 107, 'G'},
   {"ogr", 111, 'G'},
   {"omega", 119, 'G'},
   {"oplus", 197, 'G'},
   {"or", 218, 'G'},
   {"otimes", 196, 'G'},
   {"part", 182, 'G'},
   {"phi", 102, 'G'},
   {"phiv", 106, 'G'},
   {"pi", 112, 'G'},
   {"piv", 118, 'G'},
   {"prop", 181, 'G'},
   {"psi", 121, 'G'},
   {"radic", 214, 'G'},
   {"rarr", 174, 'G'},
   {"rArr", 222, 'G'},
   {"rdquo", 178, 'G'},
   {"regsf", 210, 'G'},
   {"regssf", 226, 'G'},
   {"rfraktur", 194, 'G'},
   {"rho", 114, 'G'},
   {"rsqb", 93, 'G'},
   {"sigma", 115, 'G'},
   {"sigmav", 86, 'G'},
   {"spades", 170, 'G'},
   {"sub", 204, 'G'},
   {"sube", 205, 'G'},
   {"subne", 203, 'G'},
   {"sum", 229, 'G'},
   {"sup", 201, 'G'},
   {"supe", 202, 'G'},
   {"tau", 116, 'G'},
   {"there4", 92, 'G'},
   {"theta", 113, 'G'},
   {"thetav", 74, 'G'},
   {"thickspace", 32, 'L'},
   {"times", 180, 'G'},
   {"trade", 212, 'G'},
   {"tradesf", 212, 'G'},
   {"tradessf", 228, 'G'},
   {"uarr", 173, 'G'},
   {"uArr", 221, 'G'},
   {"upsi", 117, 'G'},
   {"weierp", 195, 'G'},
   {"xi", 120, 'G'},
   {"zeta", 122, 'G'},
   {"zzzz", -1, SPACE}		/* this last entry is required */
};

/* mapping table of MathML elements */

static ElemMapping    MathMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {"XMLcomment", SPACE, MathML_EL_XMLcomment},
   {"XMLcomment_line", SPACE, MathML_EL_XMLcomment_line},
   {"maligngroup", 'E', MathML_EL_MALIGNGROUP},
   {"malignmark", 'E', MathML_EL_MALIGNMARK},
   {"merror", SPACE, MathML_EL_MERROR},
   {"mf", SPACE, MathML_EL_MF},  /* for compatibility with an old version of
				    MathML: WD-math-970704 */
   {"mfenced", SPACE, MathML_EL_MFENCED},
   {"mfrac", SPACE, MathML_EL_MFRAC},
   {"mi", SPACE, MathML_EL_MI},
   {"mmultiscripts", SPACE, MathML_EL_MMULTISCRIPTS},
   {"mn", SPACE, MathML_EL_MN},
   {"mo", SPACE, MathML_EL_MO},
   {"mover", SPACE, MathML_EL_MOVER},
   {"mpadded", SPACE, MathML_EL_MPADDED},
   {"mphantom", SPACE, MathML_EL_MPHANTOM},
   {"mprescripts", SPACE, MathML_EL_PrescriptPairs},
   {"mroot", SPACE, MathML_EL_MROOT},
   {"mrow", SPACE, MathML_EL_MROW},
   {"ms", SPACE, MathML_EL_MS},
   {"mspace", 'E', MathML_EL_MSPACE},
   {"msqrt", SPACE, MathML_EL_MSQRT},
   {"mstyle", SPACE, MathML_EL_MSTYLE},
   {"msub", SPACE, MathML_EL_MSUB},
   {"msubsup", SPACE, MathML_EL_MSUBSUP},
   {"msup", SPACE, MathML_EL_MSUP},
   {"mtable", SPACE, MathML_EL_MTABLE},
   {"mtd", SPACE, MathML_EL_MTD},
   {"mtext", SPACE, MathML_EL_MTEXT},
   {"mtr", SPACE, MathML_EL_MTR},
   {"munder", SPACE, MathML_EL_MUNDER},
   {"munderover", SPACE, MathML_EL_MUNDEROVER},
   {"none", SPACE, MathML_EL_Construct},
   {"sep", 'E', MathML_EL_SEP},
   {"", SPACE, 0}	/* Last entry. Mandatory */
};

static AttributeMapping MathMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', MathML_ATTR_Invalid_attribute},
   {"ZZGHOST", "", 'A', MathML_ATTR_Ghost_restruct},

   {"close", "mfenced", 'A', MathML_ATTR_close},
   {"fence", "mo", 'A', MathML_ATTR_fence},
   {"fontstyle", "mi", 'A', MathML_ATTR_fontstyle},
   {"open", "mfenced", 'A', MathML_ATTR_open},
   {"separators", "mfenced", 'A', MathML_ATTR_separators},

   {"", "", EOS, 0}		/* Last entry. Mandatory */
};

/* mapping table of attribute values */

static AttrValueMapping MathMLAttrValueMappingTable[] =
{
   {MathML_ATTR_fence, "true", MathML_ATTR_fence_VAL_true},
   {MathML_ATTR_fence, "false", MathML_ATTR_fence_VAL_false},
   {MathML_ATTR_fontstyle, "italic", MathML_ATTR_fontstyle_VAL_italic},
   {MathML_ATTR_fontstyle, "normal", MathML_ATTR_fontstyle_VAL_normal},

   {0, "", 0}			/* Last entry. Mandatory */
};

#define MaxMsgLength 200

/*----------------------------------------------------------------------
   GetMathMLSSchema returns the MathML Thot schema for document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
SSchema            GetMathMLSSchema (Document doc)
#else
SSchema            GetMathMLSSchema (doc)
Document	   doc;

#endif
{
  SSchema	MathMLSSchema;

   MathMLSSchema = TtaGetSSchema ("MathML", doc);
   if (MathMLSSchema == NULL)
      MathMLSSchema = TtaNewNature(TtaGetDocumentSSchema(doc), "MathML", "MathMLP");
   return (MathMLSSchema);
}


/*----------------------------------------------------------------------
   MapMathMLElementType
   search in the mapping tables the entry for the element type of
   name XMLname and returns the corresponding Thot element type.
   Returns -1 and schema = NULL if not found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               MapMathMLElementType (char *XMLname, ElementType *elType, char** mappedName, char* content, Document doc)
#else
void               MapMathMLElementType (XMLname, elType, mappedName, content, doc)
char               *XMLname;
ElementType	   *elType;
unsigned char	   **mappedName;
char		   *content;
Document            doc;
#endif
{
   int                 i;

   elType->ElTypeNum = 0;
   /* search in MathMLElemMappingTable */
   i = 0;
   do
       if (strcasecmp (MathMLElemMappingTable[i].XMLname, XMLname))
	  i++;
       else
	  {
	  elType->ElTypeNum = MathMLElemMappingTable[i].ThotType;
	  if (elType->ElSSchema == NULL)
	    elType->ElSSchema = GetMathMLSSchema (doc);
	  *mappedName = MathMLElemMappingTable[i].XMLname;
	  *content = MathMLElemMappingTable[i].XMLcontents;
	  }
   while (elType->ElTypeNum <= 0 && MathMLElemMappingTable[i].XMLname[0] != EOS);
}

/*----------------------------------------------------------------------
   GetMathMLElementName
   search in the mapping tables the XML name for a given Thot type
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void               GetMathMLElementName (ElementType elType, char **buffer)
#else
void               GetMathMLElementName (elType, buffer)
ElementType elType;
char* buffer;

#endif
{
   int                 i;

   if (elType.ElTypeNum > 0)
     {
	i = 0;
	if (strcmp ("MathML", TtaGetSSchemaName (elType.ElSSchema)) == 0)
	  do
	    {
	     if (MathMLElemMappingTable[i].ThotType == elType.ElTypeNum)
		{
		*buffer = (char *) MathMLElemMappingTable[i].XMLname;
		return;
		}
	     i++;
	    }
	  while (MathMLElemMappingTable[i].XMLname[0] != EOS);	  
     }
   *buffer = "???";
   return;
}

/*----------------------------------------------------------------------
   MapMathMLAttribute
   Search in the Attribute Mapping Table the entry for the
   attribute of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void          MapMathMLAttribute (char *Attr, AttributeType *attrType, char* elementName, Document doc)
#else
void          MapMathMLAttribute (Attr, attrType, elementName, doc)
char               *Attr;
AttributeType      *attrType;
char 		   *elementName;
Document            doc;
#endif
{
   int                 i;

   attrType->AttrTypeNum = 0;
   attrType->AttrSSchema = NULL;
   i = 0;
   do
      if (strcasecmp (MathMLAttributeMappingTable[i].XMLattribute, Attr))
	 i++;
      else
	 if (MathMLAttributeMappingTable[i].XMLelement[0] == EOS)
	       {
	       attrType->AttrTypeNum = MathMLAttributeMappingTable[i].ThotAttribute;
	       attrType->AttrSSchema = GetMathMLSSchema (doc);
	       }
	 else if (!strcasecmp (MathMLAttributeMappingTable[i].XMLelement,
			       elementName))
	       {
	       attrType->AttrTypeNum = MathMLAttributeMappingTable[i].ThotAttribute;
	       attrType->AttrSSchema = GetMathMLSSchema (doc);
	       }
	 else
	       i++;
   while (attrType->AttrTypeNum <= 0 && MathMLAttributeMappingTable[i].AttrOrContent != EOS);
}

/*----------------------------------------------------------------------
   MapMathMLAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value AttrVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MapMathMLAttributeValue (char *AttrVal, AttributeType attrType, int *value)
#else
void                MapMathMLAttributeValue (AttrVal, attrType, value)
char               *AttrVal;
AttributeType       attrType;
int		   *value;
#endif
{
   int                 i;

   *value = 0;
   i = 0;
   while (MathMLAttrValueMappingTable[i].ThotAttr != attrType.AttrTypeNum &&
	  MathMLAttrValueMappingTable[i].ThotAttr != 0)
       i++;
   if (MathMLAttrValueMappingTable[i].ThotAttr == attrType.AttrTypeNum)
       do
	   if (!strcasecmp (MathMLAttrValueMappingTable[i].XMLattrValue, AttrVal))
	       *value = MathMLAttrValueMappingTable[i].ThotAttrValue;
	   else
	       i++;
       while (*value <= 0 && MathMLAttrValueMappingTable[i].ThotAttr != 0);
}

/*----------------------------------------------------------------------
   MapMathMLEntity
   Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void	MapMathMLEntity (char *entityName, char *entityValue, int valueLength, char *alphabet)
#else
void	MapMathMLEntity (entityName, entityValue, valueLength, alphabet)
char *entityName;
char *entityValue;
int valueLength;
char *alphabet;

#endif

{
   int	i;

   for (i = 0; MathEntityTable[i].charCode >= 0 &&
	       strcmp (MathEntityTable[i].MentityName, entityName);
	       i++);
   if (!strcmp (MathEntityTable[i].MentityName, entityName))
      /* entity found */
      {
      entityValue[0] = (unsigned char) MathEntityTable[i].charCode;
      entityValue[1] = EOS;
      *alphabet = MathEntityTable[i].alphabet;
      }
   else
      {
      entityValue[0] = EOS;
      *alphabet = EOS;
      }
}

/*----------------------------------------------------------------------
   MathMLEntityCreated
   A MathML entity has been created by the XML parser.
   Create a text element containing the entity name.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        MathMLEntityCreated (unsigned char *entityValue, char *entityName, Document doc)
#else
void        MathMLEntityCreated (entityValue, entityName, doc)
unsigned char *entityValue;
char *entityName;
Document doc;

#endif
{
   ElementType	 elType;
   Element	 elText;
   AttributeType attrType;
   Attribute	 attr;
   Language	 lang;
   int		 len;
#define MAX_ENTITY_LENGTH 80
   char		 buffer[MAX_ENTITY_LENGTH];

   if (strlen (entityValue) <= 1)
     if (entityValue[0] == EOS || entityValue[0] == SPACE ||
        ((int)entityValue[0]) == 129 ||	/* thin space */
        ((int)entityValue[0]) == 130 ||	/* en space */
        ((int)entityValue[0]) == 160)	/* sticky space */
        /* null character or space */
        /* create a text element containing the entity name with an
           attribute entity */
	{
	XMLTextToDocument ();
	len = strlen (entityName);
	if (len > MAX_ENTITY_LENGTH -3)
	   len = MAX_ENTITY_LENGTH -3;
	buffer[0] = '&';
	strncpy (&buffer[1], entityName, len);
	buffer[len+1] = ';';
	buffer[len+2] = EOS;
	elType.ElTypeNum = MathML_EL_TEXT_UNIT;
	elType.ElSSchema = GetMathMLSSchema (doc);
	elText = TtaNewElement (doc, elType);
	XMLInsertElement (elText);
	lang = TtaGetLanguageIdFromAlphabet('L');
	TtaSetTextContent (elText, buffer, lang, doc);
	attrType.AttrSSchema = GetMathMLSSchema (doc);
	attrType.AttrTypeNum = MathML_ATTR_entity;
        attr = TtaNewAttribute (attrType);
        TtaAttachAttribute (elText, attr, doc);
	TtaSetAttributeValue (attr, MathML_ATTR_entity_VAL_yes_, elText, doc);
	}
}

/*----------------------------------------------------------------------
  CheckTextElement  Put the content of input buffer into the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	CheckTextElement (Element *el, Document doc)
#else
static void     CheckTextElement (el, doc)
Element *el;
Document doc;

#endif
{
   ElementType	parentType, elType;
   Element	parent, new;
   int		len;
   Language	lang;
   char		alphabet;
   char		text[4];

   len = TtaGetTextLength (*el);
   if (len == 1)
      {
      len = 2;
      TtaGiveTextContent (*el, text, &len, &lang);
      alphabet = TtaGetAlphabet (lang);
      parent = TtaGetParent (*el);
      if (text[0] != EOS)
	  {
	    parentType = TtaGetElementType (parent);
	    elType = parentType;
	    if (parentType.ElTypeNum == MathML_EL_MF &&
		(text[0] == '(' ||
		 text[0] == ')' ||
		 text[0] == '[' ||
		 text[0] == ']' ||
		 text[0] == '{' ||
		 text[0] == '}'))
	       /* Transform the text element into a Thot SYMBOL */
	       elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
	    else if (parentType.ElTypeNum == MathML_EL_MF &&
		     text[0] == '|')
	       /* Transform the text element into a Thot GRAPHIC */
	       {
	       elType.ElTypeNum = MathML_EL_GRAPHICS_UNIT;
	       text[0] = 'v';
	       }
	    else
	       /* a TEXT element is OK */
	       elType.ElTypeNum = MathML_EL_TEXT_UNIT;
	    if (elType.ElTypeNum != MathML_EL_TEXT_UNIT)
	       {
	       new = TtaNewElement (doc, elType);
	       TtaInsertSibling (new, *el, FALSE, doc);
	       TtaDeleteTree (*el, doc);
	       *el = new;
	       TtaSetGraphicsShape (new, text[0], doc);
	       }
	  }
      }
}

/*----------------------------------------------------------------------
  ElementNeedsPlaceholder
  returns TRUE if element el needs a sibling placeholder.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean      ElementNeedsPlaceholder (Element el)
#else
boolean      ElementNeedsPlaceholder (el)
Element el;
 
#endif
{
  ElementType   elType;
  Element	child, parent;
  boolean	ret;
 
  ret = FALSE;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_MROW ||
      elType.ElTypeNum == MathML_EL_MF ||
      elType.ElTypeNum == MathML_EL_MFENCED ||
      elType.ElTypeNum == MathML_EL_MROOT ||
      elType.ElTypeNum == MathML_EL_MSQRT ||
      elType.ElTypeNum == MathML_EL_MFRAC ||
      elType.ElTypeNum == MathML_EL_MSUBSUP ||
      elType.ElTypeNum == MathML_EL_MSUB ||
      elType.ElTypeNum == MathML_EL_MSUP ||
      elType.ElTypeNum == MathML_EL_MUNDER ||
      elType.ElTypeNum == MathML_EL_MOVER ||
      elType.ElTypeNum == MathML_EL_MUNDEROVER ||
      elType.ElTypeNum == MathML_EL_MMULTISCRIPTS)
     ret = TRUE;
  else
     if (elType.ElTypeNum == MathML_EL_MO)
	/* an operator that contains a single Symbol needs a placeholder,
	   except when it is in a Base or UnderOverBase */
	{
	child = TtaGetFirstChild (el);
	if (child != NULL)
	   {
	   elType = TtaGetElementType (child);
	   if (elType.ElTypeNum == MathML_EL_SYMBOL_UNIT)
	      {
	      ret = TRUE;
	      parent = TtaGetParent (el);
	      if (parent != NULL)
		{
		elType = TtaGetElementType (parent);
		if (elType.ElTypeNum == MathML_EL_Base ||
		    elType.ElTypeNum == MathML_EL_UnderOverBase)
		   ret = FALSE;
		}
	      }
	   }
	}
  return ret;
}
 
/*----------------------------------------------------------------------
  CreatePlaceholders
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	CreatePlaceholders (Element el, Document doc)
#else
static void	CreatePlaceholders (el, doc)
   Element	el;
   Document	doc;
#endif
{
   Element	sibling, prev, constr, child;
   Attribute	attr;
   ElementType	elType;
   AttributeType	attrType;
   boolean	create;

   elType.ElSSchema = GetMathMLSSchema (doc);
   prev = NULL;
   create = TRUE;
   sibling = el;
   while (sibling != NULL)
      {
      if (!ElementNeedsPlaceholder (sibling))
	 create = FALSE;
      else
	 {
	 if (sibling == el)
	    /* first element */
	    {
	    elType = TtaGetElementType (sibling);
	    if (elType.ElTypeNum == MathML_EL_MF)
	       /* the first element is a MF. Don't create a placeholder
		  before */
	       create = FALSE;
	    else if (elType.ElTypeNum == MathML_EL_MROW)
	       /* the first element is a MROW */
	       {
	       child = TtaGetFirstChild (sibling);
	       if (child != NULL)
		  {
	          elType = TtaGetElementType (child);
	          if (elType.ElTypeNum != MathML_EL_MF)
		     /* the first child of the MROW element is not a MF */
		     /* Don't create a placeholder before */
	             create = FALSE;
		  }
	       }
	    }
	 if (create)
	    {
            elType.ElTypeNum = MathML_EL_Construct;
	    constr = TtaNewElement (doc, elType);
	    TtaInsertSibling (constr, sibling, TRUE, doc);
	    attrType.AttrSSchema = elType.ElSSchema;
	    attrType.AttrTypeNum = MathML_ATTR_placeholder;
	    attr = TtaNewAttribute (attrType);
	    TtaAttachAttribute (constr, attr, doc);
	    TtaSetAttributeValue (attr, MathML_ATTR_placeholder_VAL_yes_, constr, doc);
	    }
	 create = TRUE;
	 }
      prev = sibling;
      TtaNextSibling (&sibling);
      }
   if (prev != NULL && create)
      {
	elType = TtaGetElementType (prev);
	/* don't insert a placeholder after the last element if it's a MF
	   or a SEP */
	if (elType.ElTypeNum == MathML_EL_MF ||
	    elType.ElTypeNum == MathML_EL_SEP)
	   create = FALSE;
	else if (elType.ElTypeNum == MathML_EL_MROW)
	   /* the last element is a MROW */
	   {
	   child = TtaGetLastChild (prev);
	   if (child != NULL)
	      {
	      elType = TtaGetElementType (child);
	      if (elType.ElTypeNum != MathML_EL_MF)
		 /* the last child of the MROW element is not a MF */
		 /* Don't create a placeholder before */
	         create = FALSE;
	      }
	   }
	if (create)
	   {
           elType.ElTypeNum = MathML_EL_Construct;
	   constr = TtaNewElement (doc, elType);
	   TtaInsertSibling (constr, prev, FALSE, doc);
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = MathML_ATTR_placeholder;
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (constr, attr, doc);
	   TtaSetAttributeValue (attr, MathML_ATTR_placeholder_VAL_yes_, constr, doc);
	   } 
      }
}

/*----------------------------------------------------------------------
  NextNotSep
  Return the next sibling of element el that is not a SEP element
  Return el itself if it's not a SEP
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	NextNotSep (Element* el, Element* prev)
#else
static void	NextNotSep (el, prev)
   Element	*el;
#endif
{
   ElementType	elType;

   if (*el == NULL)
      return;
   elType = TtaGetElementType (*el);
   while (*el != NULL && elType.ElTypeNum == MathML_EL_SEP)
      {
      *prev = *el;
      TtaNextSibling (el);
      if (*el != NULL)
	elType = TtaGetElementType (*el);
      }
}

/*----------------------------------------------------------------------
  CheckMathSubExpressions
  Children of element el should be of type type1, type2, and type3.
  Create an element of that type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void	CheckMathSubExpressions (Element el, int type1, int type2, int type3, Document doc)
#else
static void	CheckMathSubExpressions (el, type1, type2, type3, doc)
   Element	el;
   int		type1;
   int		type2;
   int		type3;
   Document	doc;
#endif
{
  Element	child, new, prev;
  ElementType	elType, childType;

  elType.ElSSchema = GetMathMLSSchema (doc);
  child = TtaGetFirstChild (el);
  prev = NULL;
  NextNotSep (&child, &prev);
  if (child != NULL && type1 != 0)
    {
      elType.ElTypeNum = type1;
      childType = TtaGetElementType (child);
      if (TtaSameTypes (childType, elType) == 0)
	{
	  TtaRemoveTree (child, doc);	
	  new = TtaNewElement (doc, elType);
	  if (prev == NULL)
	    TtaInsertFirstChild (&new, el, doc);
	  else
	    TtaInsertSibling (new, prev, FALSE, doc);
	  TtaInsertFirstChild (&child, new, doc);
	  CreatePlaceholders (child, doc);
	  child = new;
	}
      if (type2 != 0)
	{
	  prev = child;
	  TtaNextSibling (&child);
	  NextNotSep (&child, &prev);
	  if (child != NULL)
	    {
	      elType.ElTypeNum = type2;
	      childType = TtaGetElementType (child);
	      if (TtaSameTypes (childType, elType) == 0)
		{
		  TtaRemoveTree (child, doc);
		  new = TtaNewElement (doc, elType);
		  TtaInsertSibling (new, prev, FALSE, doc);
		  TtaInsertFirstChild (&child, new, doc);
		  CreatePlaceholders (child, doc);
		  child = new;
		}
	      if (type3 != 0)
		{
		  prev = child;
		  TtaNextSibling (&child);
		  NextNotSep (&child, &prev);
		  if (child != NULL)
		    {
		      elType.ElTypeNum = type3;
		      childType = TtaGetElementType (child);
		      if (TtaSameTypes (childType, elType) == 0)
			{
			  TtaRemoveTree (child, doc);
			  new = TtaNewElement (doc, elType);
			  TtaInsertSibling (new, prev, FALSE, doc);
			  TtaInsertFirstChild (&child, new, doc);
			  CreatePlaceholders (child, doc);
			}
		    }
		}
	    }
        }
    }
}


/*----------------------------------------------------------------------
   SetSingleHorizStretchAttr

   Put a horizstretch attribute on element el if it contains only
   a MO element that is a stretchable symbol.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetSingleHorizStretchAttr (Element el, Document doc, Element* selEl)
#else /* __STDC__*/
void SetSingleHorizStretchAttr (el, doc, selEl)
  Element	el;
  Document	doc;
  Element*	selEl;
#endif /* __STDC__*/
{
  Element	child, sibling, textEl, symbolEl;
  ElementType	elType;
  Attribute	attr;
  AttributeType	attrType;
  int		len;
  Language	lang;
  char		alphabet;
  unsigned char	text[2], c;

  if (el == NULL)
     return;
  child = TtaGetFirstChild (el);
  if (child != NULL)
     {
     elType = TtaGetElementType (child);
     if (elType.ElTypeNum == MathML_EL_MO)
	/* the first child is a MO */
        {
        sibling = child;
        TtaNextSibling (&sibling);
	if (sibling == NULL)
	   /* there is no other child */
	   {
	   textEl = TtaGetFirstChild (child);
	   elType = TtaGetElementType (textEl);
	   if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	      {
	      len = TtaGetTextLength (textEl);
	      if (len == 1)
		{
		len = 2;
		TtaGiveTextContent (textEl, text, &len, &lang);
		alphabet = TtaGetAlphabet (lang);
		if (len == 1)
		   if (alphabet == 'G')
		     /* a single Symbol character */
		     if ((int)text[0] == 172 || (int)text[0] == 174)
			/* horizontal arrow */
			{
			/* attach a horizstretch attribute */
			attrType.AttrSSchema = elType.ElSSchema;
			attrType.AttrTypeNum = MathML_ATTR_horizstretch;
			attr = TtaNewAttribute (attrType);
			TtaAttachAttribute (el, attr, doc);
			TtaSetAttributeValue (attr, MathML_ATTR_horizstretch_VAL_yes_, el, doc);
			/* replace the TEXT element by a Thot SYMBOL element */
			elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
			symbolEl = TtaNewElement (doc, elType);
			TtaInsertSibling (symbolEl, textEl, FALSE, doc);
			if (selEl != NULL)
			   if (*selEl == textEl)
			      *selEl = symbolEl;
			TtaDeleteTree (textEl, doc);
			if ((int)text[0] == 172)
			   c = '<';
			if ((int)text[0] == 174)
			   c = '>';
			TtaSetGraphicsShape (symbolEl, c, doc);
			}
		}
	      }
	   }
	}
     }
}

/*----------------------------------------------------------------------
   SetHorizStretchAttr

   Put a horizstretch attribute on all children of element el which
   contain only a MO element that is a stretchable symbol.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetHorizStretchAttr (Element el, Document doc)
#else /* __STDC__*/
static void SetHorizStretchAttr (el, doc)
  Element	el;
  Document	doc;
#endif /* __STDC__*/
{
  Element	child;

  if (el == NULL)
     return;
  child = TtaGetFirstChild (el);
  while (child != NULL)
     {
     SetSingleHorizStretchAttr (child, doc, NULL);
     TtaNextSibling (&child);
     }
}

/*----------------------------------------------------------------------
   SetVertStretchAttr

   Put a vertstretch attribute on element el if its base element
   (Base for a MSUBSUP, MSUP or MSUB; UnderOverBase for a MUNDEROVER,
   a MUNDER of a MOVER) contains only a MO element that is a vertically
   stretchable symbol.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetVertStretchAttr (Element el, Document doc, int base, Element* selEl)
#else /* __STDC__*/
void SetVertStretchAttr (el, doc, base, selEl)
  Element	el;
  Document	doc;
  int		base;
  Element*	selEl;
#endif /* __STDC__*/
{
  Element	child, sibling, textEl, symbolEl, parent, operator;
  ElementType	elType;
  Attribute	attr;
  AttributeType	attrType;
  int		len;
  Language	lang;
  char		alphabet;
  unsigned char	text[2], c;

  if (el == NULL)
     return;
  operator = NULL;
  if (base == 0)
     /* it's a MO */
     {
     parent = TtaGetParent (el);
     if (parent != NULL)
	{
	elType = TtaGetElementType (parent);
	if (elType.ElTypeNum != MathML_EL_Base &&
	    elType.ElTypeNum != MathML_EL_UnderOverBase &&
	    elType.ElTypeNum != MathML_EL_MSUBSUP &&
	    elType.ElTypeNum != MathML_EL_MSUB &&
	    elType.ElTypeNum != MathML_EL_MSUP &&
	    elType.ElTypeNum != MathML_EL_MUNDEROVER &&
	    elType.ElTypeNum != MathML_EL_MUNDER &&
	    elType.ElTypeNum != MathML_EL_MUNDEROVER)
	   operator = el;
        }
     }
  else
     /* it's not a MO */
     {
     /* search the Base or UnderOverBase child */
     child = TtaGetFirstChild (el);
     if (child != NULL)
        {
        elType = TtaGetElementType (child);
        if (elType.ElTypeNum == base)
	   /* the first child is a Base or UnderOverBase */
           {
	   child = TtaGetFirstChild (child);
	   if (child != NULL)
	      {
	      elType = TtaGetElementType (child);
              if (elType.ElTypeNum == MathML_EL_MO)
	         /* its first child is a MO */
                 {
                 sibling = child;
                 TtaNextSibling (&sibling);
	         if (sibling == NULL)
	            /* there is no other child */
	            operator = child;
		 }
	      }
	   }
	}
     }
  if (operator != NULL)
     {
	   textEl = TtaGetFirstChild (operator);
	   if (textEl != NULL)
	      {
	      elType = TtaGetElementType (textEl);
	      if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	         {
	         len = TtaGetTextLength (textEl);
	         if (len == 1)
		   {
		   len = 2;
		   TtaGiveTextContent (textEl, text, &len, &lang);
		   alphabet = TtaGetAlphabet (lang);
		   if (len == 1)
		     if (alphabet == 'G')
		       /* a single Symbol character */
		       if ((int)text[0] == 242)
			 /* Integral */
			 {
			 /* attach a vertstretch attribute */
			 attrType.AttrSSchema = elType.ElSSchema;
			 attrType.AttrTypeNum = MathML_ATTR_vertstretch;
			 attr = TtaNewAttribute (attrType);
			 TtaAttachAttribute (el, attr, doc);
			 TtaSetAttributeValue (attr, MathML_ATTR_vertstretch_VAL_yes_, el, doc);
			 /* replace the TEXT element by a Thot SYMBOL element*/
			 elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
			 symbolEl = TtaNewElement (doc, elType);
			 TtaInsertSibling (symbolEl, textEl, FALSE, doc);
			 if (selEl != NULL)
			   if (*selEl == textEl)
			      *selEl = symbolEl;
			 TtaDeleteTree (textEl, doc);
			 c = 'i';
			 TtaSetGraphicsShape (symbolEl, c, doc);
			 }
		   }
	         }
	      }
     }
}

/*----------------------------------------------------------------------
   SetPlaceholderAttr

   Put a placeholder attribute on all Construct elements in the
   subtree of root el.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void SetPlaceholderAttr (Element el, Document doc)
#else /* __STDC__*/
static void SetPlaceholderAttr (el, doc)
  Element	el;
  Document	doc;
#endif /* __STDC__*/
{
  Element	child;
  ElementType	elType;
  Attribute	attr;
  AttributeType	attrType;

  if (el == NULL)
     return;
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == MathML_EL_Construct &&
      elType.ElSSchema == GetMathMLSSchema (doc))
     {
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_placeholder;
     attr = TtaNewAttribute (attrType);
     TtaAttachAttribute (el, attr, doc);
     TtaSetAttributeValue (attr, MathML_ATTR_placeholder_VAL_yes_, el, doc);
     }
  else
     {
     child = TtaGetFirstChild (el);
     while (child != NULL)
        {
        SetPlaceholderAttr (child, doc);
        TtaNextSibling (&child);
        }
     }
}

/*----------------------------------------------------------------------
   BuildMultiscript

   The content of a MMULTISCRIPT element has been created following
   the original MathML structure.  Create all Thot elements defined
   in the MathML S schema.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void BuildMultiscript (Element elMMULTISCRIPT, Document doc)
#else /* __STDC__*/
static void BuildMultiscript (elMMULTISCRIPT, doc)
  Element	elMMULTISCRIPT;
  Document	doc;
#endif /* __STDC__*/
{
  Element	elem, base, next, group, pair, script, prevPair, prevScript;
  ElementType	elType, elTypeGroup, elTypePair, elTypeScript;
  SSchema       MathMLSSchema;
  base = NULL;
  group = NULL;
  prevPair = NULL;
  prevScript = NULL;

  MathMLSSchema = GetMathMLSSchema (doc);
  elTypeGroup.ElSSchema = MathMLSSchema;
  elTypePair.ElSSchema = MathMLSSchema;
  elTypeScript.ElSSchema = MathMLSSchema;

  /* process all children of the MMULTISCRIPT element */
  elem = TtaGetFirstChild (elMMULTISCRIPT);
  while (elem != NULL)
    {
      /* remember the element to be processed after the current one */
      next = elem;
      TtaNextSibling (&next);

      /* remove the current element from the tree */
      TtaRemoveTree (elem, doc);

      if (base == NULL)
	/* the current element is the first child of the MMULTISCRIPT
	   element */
	{
	  /* Create a MultiscriptBase element as the first child of
	     MMULTISCRIPT and move the current element as the first child
	     of the MultiscriptBase element */
	  elTypeGroup.ElTypeNum = MathML_EL_MultiscriptBase;
	  base = TtaNewElement (doc, elTypeGroup);
	  TtaInsertFirstChild (&base, elMMULTISCRIPT, doc);
	  TtaInsertFirstChild (&elem, base, doc);
	}
      else
	/* the current element is a subscript or a superscript */
	{
	  if (group == NULL)
	    /* there is no PostscriptPairs element. Create one */
	    {
	      elTypeGroup.ElTypeNum = MathML_EL_PostscriptPairs;
	      group = TtaNewElement (doc, elTypeGroup);
	      TtaInsertSibling (group, base, FALSE, doc);
	      elTypePair.ElTypeNum = MathML_EL_PostscriptPair;
	      /* create a first and a last PostscriptPair as placeholders */
	      pair = TtaNewTree (doc, elTypePair, "");
	      TtaInsertFirstChild (&pair, group, doc);
	      SetPlaceholderAttr (pair, doc);
	      prevPair = pair;
	      pair = TtaNewTree (doc, elTypePair, "");
	      TtaInsertSibling (pair, prevPair, FALSE, doc);
	      SetPlaceholderAttr (pair, doc);
	      prevScript = NULL;
	    }
	  if (prevScript == NULL)
	    /* the current element is the first subscript or superscript
	       in a pair */
	    {
	      /* create a PostscriptPair or PrescriptPair element */
	      pair = TtaNewElement (doc, elTypePair);
	      if (prevPair == NULL)
		TtaInsertFirstChild (&pair, group, doc);
	      else
		TtaInsertSibling (pair, prevPair, FALSE, doc);
	      prevPair = pair;
	      /* create a MSubscript element */
	      elTypeScript.ElTypeNum = MathML_EL_MSubscript;
	      script = TtaNewElement (doc, elTypeScript);
	      TtaInsertFirstChild (&script, pair, doc);
	      prevScript = script;	  
	    }
	  else
	    /* the current element is a superscript in a pair */
	    {
	      /* create a MSuperscript element */
	      elTypeScript.ElTypeNum = MathML_EL_MSuperscript;
	      script = TtaNewElement (doc, elTypeScript);
	      /* insert it as a sibling of the previous MSubscript element */
	      TtaInsertSibling (script, prevScript, FALSE, doc);
	      prevScript = NULL;	  
	    }
	  /* insert the current element as a child of the new MSuperscript or
	     MSubscript element */
	  TtaInsertFirstChild (&elem, script, doc);
	  SetPlaceholderAttr (elem, doc);
	}

      CreatePlaceholders (elem, doc);

      /* get next child of the MMULTISCRIPT element */
      elem = next;
      if (elem != NULL)
	{
	  elType = TtaGetElementType (elem);
	  if (elType.ElSSchema == MathMLSSchema &&
	      elType.ElTypeNum == MathML_EL_PrescriptPairs)
	    /* the next element is a PrescriptPairs */
	    {
	      /* if there there is no PostscriptPairs element, create one as a
		 placeholder */
	      if (elTypeGroup.ElTypeNum != MathML_EL_PostscriptPairs)
		{
		  elTypeGroup.ElTypeNum = MathML_EL_PostscriptPairs;
		  group = TtaNewTree (doc, elTypeGroup, "");
		  TtaInsertSibling (group, elem, TRUE, doc);
		  SetPlaceholderAttr (group, doc);
		}
	      /* the following elements will be interpreted as sub- superscripts
		 in PrescriptPair elements, wich will be children of this
		 PrescriptPairs element */
	      elTypeGroup.ElTypeNum = MathML_EL_PrescriptPairs;
	      elTypePair.ElTypeNum = MathML_EL_PrescriptPair;
	      group = elem;
	      /* create a first and a last PostscriptPair as placeholders */
	      pair = TtaNewTree (doc, elTypePair, "");
	      TtaInsertFirstChild (&pair, group, doc);
	      SetPlaceholderAttr (pair, doc);
	      prevPair = pair;
	      pair = TtaNewTree (doc, elTypePair, "");
	      TtaInsertSibling (pair, prevPair, FALSE, doc);
	      SetPlaceholderAttr (pair, doc);
	      prevScript = NULL;
	      TtaNextSibling (&elem);
	    }
	}
    }
  /* all children of element MMULTISCRIPTS have been processed */
  /* if the last group processed is not a PrescriptPairs element,
     create one as a placeholder */
  if (elTypeGroup.ElTypeNum != MathML_EL_PrescriptPairs && base != NULL)
    {
      elTypeGroup.ElTypeNum = MathML_EL_PrescriptPairs;
      elem = TtaNewTree (doc, elTypeGroup, "");
      if (group == NULL)
	group = base;
      TtaInsertSibling (elem, group, TRUE, doc);
      SetPlaceholderAttr (elem, doc);
    }
}

/*----------------------------------------------------------------------
   LinkMathCellsWithColumnHeads

 -----------------------------------------------------------------------*/
#ifdef __STDC__
void LinkMathCellsWithColumnHeads (Element elMTABLE, Document doc)
#else /* __STDC__*/
void LinkMathCellsWithColumnHeads (elMTABLE, doc)
  Element	elMTABLE;
  Document	doc;
#endif /* __STDC__*/
{
  ElementType	elType;
  Element	firstColHead, head, prevHead, nextHead, row, cell, prevCell,
		MTableHead, MTableBody;
  SSchema	MathMLSSchema;
  AttributeType	attrType;
  Attribute	attr;
  int		nCol, nCell, nHead;

  MathMLSSchema = GetMathMLSSchema (doc);
  MTableHead = TtaGetFirstChild (elMTABLE);
  MTableBody = MTableHead;
  TtaNextSibling (&MTableBody);
  firstColHead = TtaGetFirstChild (MTableHead);

  /* first, create a MColumn_head for each column in the table and
     count columns */
  row = TtaGetFirstChild (MTableBody);
  nCol = 0;
  while (row)
    {
    elType = TtaGetElementType (row);
    if (elType.ElTypeNum == MathML_EL_MTR &&
	TtaSameSSchemas (elType.ElSSchema, MathMLSSchema))
      {
      head = firstColHead;
      prevHead = NULL;
      cell = TtaGetFirstChild (row);
      nCell = 0;
      while (cell)
	{
	elType = TtaGetElementType (cell);
	if (elType.ElTypeNum == MathML_EL_MTD &&
	    TtaSameSSchemas (elType.ElSSchema, MathMLSSchema))
	   {
	   if (!head)
	      {
	      elType.ElTypeNum = MathML_EL_MColumn_head;
	      head = TtaNewTree (doc, elType, "");
	      TtaInsertSibling (head, prevHead, FALSE, doc);
	      }
	   nCell++;
	   prevHead = head;
	   TtaNextSibling (&head);
	   }
	TtaNextSibling (&cell);
	}
      }
    if (nCell > nCol)
       nCol = nCell;
    TtaNextSibling (&row);
    }

  /* delete the last MColumn_heads if there are more MColumn_heads than
     columns */
  head = firstColHead;
  nHead = 0;
  while (head)
    {
    nHead++;
    nextHead = head;
    TtaNextSibling (&nextHead);
    if (nHead > nCol)
       TtaDeleteTree (head, doc);
    head = nextHead;
    }

  /* relate each cell with the corresponding MColumn_head and create
     additional cells in incomplete rows */
  attrType.AttrSSchema = MathMLSSchema;
  attrType.AttrTypeNum = MathML_ATTR_MRef_column;
  row = TtaGetFirstChild (MTableBody);
  while (row)
    {
    elType = TtaGetElementType (row);
    if (elType.ElTypeNum == MathML_EL_MTR &&
	TtaSameSSchemas (elType.ElSSchema, MathMLSSchema))
      {
      head = firstColHead;
      prevCell = NULL;
      cell = TtaGetFirstChild (row);
      while (head)
	{
	if (cell)
	   elType = TtaGetElementType (cell);
	while (cell &&
	       (elType.ElTypeNum != MathML_EL_MTD ||
	        !TtaSameSSchemas (elType.ElSSchema, MathMLSSchema)))
	   {
	   TtaNextSibling (&cell);
	   if (cell)
	      elType = TtaGetElementType (cell);
	   }
	if (!cell)
	   {
	   elType.ElTypeNum = MathML_EL_MTD;
	   cell = TtaNewTree (doc, elType, "");
	   if (prevCell)
	      TtaInsertSibling (cell, prevCell, FALSE, doc);
	   else
	      TtaInsertFirstChild (&cell, row, doc);
	   }
	attr = TtaGetAttribute (cell, attrType);
	if (!attr)
	   {
	   attr = TtaNewAttribute (attrType);
	   TtaAttachAttribute (cell, attr, doc);
	   }
	TtaSetAttributeReference (attr, cell, doc, head, doc);
	prevCell = cell;
	TtaNextSibling (&cell);
	TtaNextSibling (&head);
	}
      }
    TtaNextSibling (&row);
    }
}

/*----------------------------------------------------------------------
   CheckMTable

   The content of a MTABLE element has been created following
   the original MathML structure.  Create all Thot elements defined
   in the MathML S schema.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
static void CheckMTable (Element elMTABLE, Document doc)
#else /* __STDC__*/
static void CheckMTable (elMTABLE, doc)
  Element	elMTABLE;
  Document	doc;
#endif /* __STDC__*/
{
  ElementType	elType;
  Element	MTableHead, MTableBody, row, nextRow, el, prevRow, cell,
		nextCell, newMTD, firstColHead, child, prevChild, nextChild,
		wrapper;
  SSchema	MathMLSSchema;

  MathMLSSchema = GetMathMLSSchema (doc);
  row = TtaGetFirstChild (elMTABLE);

  /* create a MTable_head as the first child of element MTABLE */
  elType.ElSSchema = MathMLSSchema;
  elType.ElTypeNum = MathML_EL_MTable_head;
  MTableHead = TtaNewElement (doc, elType);
  TtaInsertFirstChild (&MTableHead, elMTABLE, doc);
  elType.ElTypeNum = MathML_EL_MColumn_head;
  firstColHead = TtaNewTree (doc, elType, "");
  TtaInsertFirstChild (&firstColHead, MTableHead, doc);

  /* create a MTable_body */
  elType.ElSSchema = MathMLSSchema;
  elType.ElTypeNum = MathML_EL_MTable_body;
  MTableBody = TtaNewElement (doc, elType);
  TtaInsertSibling (MTableBody, MTableHead, FALSE, doc);

  /* move all children of element MTABLE into the new MTable_body element
     and wrap each non-MTR element with a MTR */
  prevRow = NULL;
  while (row)
    {
    nextRow = row;
    TtaNextSibling (&nextRow);
    elType = TtaGetElementType (row);
    TtaRemoveTree (row, doc);
    if (TtaSameSSchemas (elType.ElSSchema, MathMLSSchema) &&
	(elType.ElTypeNum == MathML_EL_XMLcomment ||
	 elType.ElTypeNum == MathML_EL_MTR))
       {
       if (prevRow == NULL)
	  TtaInsertFirstChild (&row, MTableBody, doc);
       else
	  TtaInsertSibling (row, prevRow, FALSE, doc);
       prevRow = row;
       if (elType.ElTypeNum == MathML_EL_MTR)
          cell = TtaGetFirstChild (row);
       else
	  cell = NULL;
       }
    else
       /* this child is not a MTR nor a comment, create a MTR element */
       {
       elType.ElSSchema = MathMLSSchema;
       elType.ElTypeNum = MathML_EL_MTR;
       el = TtaNewElement (doc, elType);
       if (prevRow == NULL)
	  TtaInsertFirstChild (&el, MTableBody, doc);
       else
	  TtaInsertSibling (el, prevRow, FALSE, doc);
       TtaInsertFirstChild (&row, el, doc);
       cell = row;
       prevRow = el;
       }
    while (cell)
      /* check all children of the current MTR element */
      {
      nextCell = cell;
      TtaNextSibling (&nextCell);
      elType = TtaGetElementType (cell);
      if (!TtaSameSSchemas (elType.ElSSchema, MathMLSSchema) ||
          (elType.ElTypeNum != MathML_EL_XMLcomment &&
           elType.ElTypeNum != MathML_EL_MTD))
	 /* this is not a MTD nor a comment, create a wrapping MTD */
         {
	 elType.ElSSchema = MathMLSSchema;
	 elType.ElTypeNum = MathML_EL_MTD;
	 newMTD = TtaNewElement (doc, elType);
	 TtaInsertSibling (newMTD, cell, TRUE, doc);
	 TtaRemoveTree (cell, doc);
	 TtaInsertFirstChild (&cell, newMTD, doc);
	 cell = newMTD;
	 }
      if (elType.ElTypeNum == MathML_EL_MTD)
	 /* This is a MTD element. Wrap its contents with a CellWrapper */
	 {
	 child = TtaGetFirstChild (cell);
	 elType.ElSSchema = MathMLSSchema;
	 elType.ElTypeNum = MathML_EL_CellWrapper;
	 wrapper = TtaNewElement (doc, elType);
	 TtaInsertFirstChild (&wrapper, cell, doc);
	 prevChild = NULL;
	 while (child)
	      {
	      nextChild = child;
	      TtaNextSibling (&nextChild);
	      TtaRemoveTree (child, doc);
	      if (prevChild == NULL)
		 TtaInsertFirstChild (&child, wrapper, doc);
	      else
		 TtaInsertSibling (child, prevChild, FALSE, doc);
	      prevChild = child;
	      child = nextChild;
	      }
	 }
      cell = nextCell;
      }
    row = nextRow;
    }
  LinkMathCellsWithColumnHeads (elMTABLE, doc);
}

/*----------------------------------------------------------------------
   SetFontstyleAttr
   The content of a MI element has been created or modified.
   Create or change attribute IntFontstyle for that element accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetFontstyleAttr (Element el, Document doc)
#else /* __STDC__*/
void SetFontstyleAttr (el, doc)
  Element	el;
  Document	doc;
#endif /* __STDC__*/
{
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr, IntAttr;
  int		len;

  if (el != NULL)
     {
     /* search the fontstyle attribute */
     elType = TtaGetElementType (el);
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_fontstyle;
     attr = TtaGetAttribute (el, attrType);
     attrType.AttrTypeNum = MathML_ATTR_IntFontstyle;
     IntAttr = TtaGetAttribute (el, attrType);
     if (attr != NULL)
	/* there is a fontstyle attribute. Remove the corresponding
	   internal attribute that is not needed */
	{
	if (IntAttr != NULL)
	TtaRemoveAttribute (el, IntAttr, doc);
	}
     else
	/* there is no fontstyle attribute. Create an internal attribute
	   IntFontstyle with a value that depends on the content of the MI */
	{
        /* get content length */
        len = TtaGetElementVolume (el);
        if (len > 1)
           /* put an attribute IntFontstyle = IntNormal */
	   {
	   if (IntAttr == NULL)
	      {
	      IntAttr = TtaNewAttribute (attrType);
	      TtaAttachAttribute (el, IntAttr, doc);
	      }
	   TtaSetAttributeValue (IntAttr, MathML_ATTR_IntFontstyle_VAL_IntNormal,
				 el, doc);
	   }
        else
	   /* MI contains a single character. Remove attribute IntFontstyle
	      if it exists */
	   {
	   if (IntAttr != NULL)
	      TtaRemoveAttribute (el, IntAttr, doc);
	   }
        }
     }
}

/*----------------------------------------------------------------------
   SetAddspaceAttr
   The content of a MO element has been created or modified.
   Create or change attribute addspace for that element accordingly.
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void SetAddspaceAttr (Element el, Document doc)
#else /* __STDC__*/
void SetAddspaceAttr (el, doc)
  Element	el;
  Document	doc;
#endif /* __STDC__*/
{
  Element	textEl, previous;
  ElementType	elType;
  AttributeType	attrType;
  Attribute	attr;
  int		len, val;
#define BUFLEN 10
  unsigned char	text[BUFLEN];
  Language	lang;
  char		alphabet;

  textEl = TtaGetFirstChild (el);
  if (textEl != NULL)
     {
     /* search the addspace attribute */
     elType = TtaGetElementType (el);
     attrType.AttrSSchema = elType.ElSSchema;
     attrType.AttrTypeNum = MathML_ATTR_addspace;
     attr = TtaGetAttribute (el, attrType);
     if (attr == NULL)
	{
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
	}
     val = MathML_ATTR_addspace_VAL_nospace;
     len = TtaGetTextLength (textEl);
     if (len > 0 && len < BUFLEN)
	{
	len = BUFLEN;
	TtaGiveTextContent (textEl, text, &len, &lang);
	alphabet = TtaGetAlphabet (lang);
	if (len == 1)
	   if (alphabet == 'L')
	     /* ISO-Latin 1 character */
	     {
	     if (text[0] == '-')
		/* unary or binary operator? */
		{
		previous = el;
		TtaPreviousSibling (&previous);
		if (previous == NULL)
		   /* no previous sibling => unary operator */
		   val = MathML_ATTR_addspace_VAL_nospace;
		else
		   {
		   elType = TtaGetElementType (previous);
		   if (elType.ElTypeNum == MathML_EL_MO)
		      /* after an operator => unary operator */
		      val = MathML_ATTR_addspace_VAL_nospace;
		   else
		      /* binary operator */
		      val = MathML_ATTR_addspace_VAL_both;
		   }
		}
	     else if (text[0] == '+' ||
	         text[0] == '&' ||
	         text[0] == '*' ||
	         text[0] == '<' ||
	         text[0] == '=' ||
	         text[0] == '>' ||
	         text[0] == '^')
		 /* binary operator */
	         val = MathML_ATTR_addspace_VAL_both;
	     else if (text[0] == ',' ||
		      text[0] == ';')
	         val = MathML_ATTR_addspace_VAL_spaceafter;
	     }
	   else if (alphabet == 'G')
	     /* Symbol character set */
	     if ((int)text[0] == 163 || /* less or equal */
		 (int)text[0] == 177 ||	/* plus or minus */
		 (int)text[0] == 179 || /* greater or equal */
		 (int)text[0] == 180 || /* times */
		 (int)text[0] == 184 || /* divide */
		 (int)text[0] == 185 || /* not equal */
		 (int)text[0] == 186 || /* identical */
		 (int)text[0] == 187 || /* equivalent */
		 (int)text[0] == 196 || /* circle times */
		 (int)text[0] == 197 || /* circle plus */
		 ((int)text[0] >= 199 && (int)text[0] <= 209) || /*  */
		 (int)text[0] == 217 || /* and */
		 (int)text[0] == 218 )  /* or */
		val = MathML_ATTR_addspace_VAL_both;
	}
     TtaSetAttributeValue (attr, val, el, doc);
     }
}


/*----------------------------------------------------------------------
   ChangeTypeOfElement
   Change the type of element elem into newTypeNum
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void ChangeTypeOfElement (Element elem, Document doc, int newTypeNum)
#else /* __STDC__*/
void ChangeTypeOfElement (elem, doc, newTypeNum)
     Element elem;
     Document doc;
     int newTypeNum;
#endif /* __STDC__*/
 
{
     Element    prev, next, parent;
 
     prev = elem;
     TtaPreviousSibling (&prev);
     if (prev == NULL)
        {
        next = elem;
        TtaNextSibling (&next);
        if (next == NULL)
           parent = TtaGetParent (elem);
        }
     TtaRemoveTree (elem, doc);
     ChangeElementType (elem, newTypeNum);
     if (prev != NULL)
        TtaInsertSibling (elem, prev, FALSE, doc);
     else if (next != NULL)
        TtaInsertSibling (elem, next, TRUE, doc);
     else
        TtaInsertFirstChild (&elem, parent, doc);
}


/*----------------------------------------------------------------------
   CheckFence
   If el is a MO element that contains a single fence character,
   transform the MO into a MF and the character into a Thot symbol.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      CheckFence (Element el, Document doc)
#else
void      CheckFence (el, doc)
Element                 el;
Document		doc;

#endif
{
   ElementType	elType;
   Element	content;
   AttributeType attrType;
   Attribute	attr;
   int		len;
   Language	lang;
   char		alphabet;
   unsigned char	text[2], c;

   elType = TtaGetElementType (el);
   if (elType.ElTypeNum == MathML_EL_MO)
      {
      content = TtaGetFirstChild (el);
      if (content != NULL)
	{
	elType = TtaGetElementType (content);
	if (elType.ElTypeNum == MathML_EL_TEXT_UNIT)
	   {
	   len = TtaGetTextLength (content);
	   if (len == 1)
	      {
	      len = 2;
	      TtaGiveTextContent (content, text, &len, &lang);
	      alphabet = TtaGetAlphabet (lang);
	      if (len == 1)
		if (alphabet == 'L')
		   /* a single character */
		   if (text[0] == '(' || text[0] == ')' ||
		       text[0] == '[' || text[0] == ']' ||
		       text[0] == '{' || text[0] == '}' ||
		       text[0] == '|' )
		      {
		      /* remove the content of the MO element */
		      TtaDeleteTree (content, doc);
		      /* change the MO element into a MF element */
		      ChangeTypeOfElement (el, doc, MathML_EL_MF);
		      /* attach a vertstretch attribute to the MF element */
		      attrType.AttrSSchema = elType.ElSSchema;
		      attrType.AttrTypeNum = MathML_ATTR_vertstretch;
		      attr = TtaNewAttribute (attrType);
		      TtaAttachAttribute (el, attr, doc);
		      TtaSetAttributeValue (attr, MathML_ATTR_vertstretch_VAL_yes_, el, doc);
		      /* create a new content for the MF element */
		      if (text[0] == '|')
			 {
			 elType.ElTypeNum = MathML_EL_GRAPHICS_UNIT;
			 c = 'v';
			 }
		      else
			 {
		         elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
		         c = text[0];
			 }
		      content = TtaNewElement (doc, elType);
		      TtaInsertFirstChild (&content, el, doc);
		      TtaSetGraphicsShape (content, c, doc);
		      }
	      }
	   }
	}
      }
}

/*----------------------------------------------------------------------
   CreateFencedSeparators
   Create FencedSeparator elements within the fencedExpression
   according to attribute separators of the MFENCED element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      CreateFencedSeparators (Element fencedExpression, Document doc)
#else
void      CreateFencedSeparators (fencedExpression, doc)
Element		fencedExpression;
Document	doc;

#endif
{
   ElementType	 elType;
   Element	 child, separator, leaf, next, prev, mfenced;
   AttributeType attrType;
   Attribute     attr;
   int		 length, sep, i;
   Language	 lang;
   char		 text[32], sepValue[4];

   /* get the separators attribute */
   mfenced = TtaGetParent (fencedExpression);
   elType = TtaGetElementType (fencedExpression);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = MathML_ATTR_separators;
   text[0] = ',';	/* default value is  sparators=","  */
   text[1] = EOS;
   length = 1;
   attr = TtaGetAttribute (mfenced, attrType);
   if (attr != NULL)
      {
      length = 31;
      TtaGiveTextAttributeValue (attr, text, &length);
      }

   /* create FencedSeparator elements in the FencedExpression */
   prev = NULL;
   sep = 0;
   /* skip leading spaces in attribute separators */
   while (text[sep] <= SPACE && text[sep] != EOS)
      sep++;
   /* if attribute separators is empty or contains only spaces, do not
      insert any separator element */
   if (text[sep] != EOS)
     {
     child = TtaGetFirstChild (fencedExpression);
     while (child != NULL)
       {
       next = child;
       TtaNextSibling (&next);
       elType = TtaGetElementType (child);
       if (elType.ElTypeNum != MathML_EL_Construct)
         {
         if (prev != NULL)
           {
           elType.ElTypeNum = MathML_EL_FencedSeparator;
           separator = TtaNewElement (doc, elType);
           TtaInsertSibling (separator, prev, FALSE, doc);
           elType.ElTypeNum = MathML_EL_TEXT_UNIT;
           leaf = TtaNewElement (doc, elType);
           TtaInsertFirstChild (&leaf, separator, doc);
           sepValue[0] = text[sep];
           sepValue[1] = SPACE;
           sepValue[2] = EOS;
	   lang = TtaGetLanguageIdFromAlphabet('L');
           TtaSetTextContent (leaf, sepValue, lang, doc);
	   /* is there a following non-space character in separators? */
	   i = sep + 1;
	   while (text[i] <= SPACE && text[i] != EOS)
	      i++;
           if (text[i] > SPACE && text[i] != EOS)
              sep = i;
           }
         prev = child;
         }
       child = next;
       }
     }
}


/*----------------------------------------------------------------------
   TransformMFENCED
   Transform the content of a MFENCED element: create elements
   OpeningFence, FencedExpression, ClosingFence and FencedSeparator.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      TransformMFENCED (Element el, Document doc)
#else
void      TransformMFENCED (el, doc)
Element		el;
Document	doc;

#endif
{
   ElementType	 elType;
   Element	 child, fencedExpression, leaf, fence, next, prev,
		 firstChild;
   AttributeType attrType;
   Attribute     attr;
   int		 length;
   char		 text[32], c;

   child = TtaGetFirstChild (el);
   if (child != NULL)
        elType = TtaGetElementType (child);
   if (child != NULL && elType.ElTypeNum == MathML_EL_OpeningFence)
      /* The first child of this MFENCED element is an OpeningFence.
	 This MFENCED expression has already been transformed, possibly
	 by the Transform command */
      {
      TtaNextSibling (&child);
      fencedExpression = child;
      if (fencedExpression != NULL)
	 elType = TtaGetElementType (fencedExpression);
      if (elType.ElTypeNum == MathML_EL_FencedExpression)
	 /* the second child is a FencedExpression. OK.
	    Remove all existing FencedSeparator elements */
	 {
	 child = TtaGetFirstChild (fencedExpression);
	 prev = NULL;
	 while (child != NULL)
	    {
	    elType = TtaGetElementType (child);
	    next = child;
	    TtaNextSibling (&next);
	    if (elType.ElTypeNum == MathML_EL_FencedSeparator)
		/* Remove this separator */
		TtaDeleteTree (child, doc);
	    child = next;
	    }
	 /* create FencedSeparator elements in the FencedExpression */
	 CreateFencedSeparators (fencedExpression, doc);
	 }
      }
   else
      /* this MFENCED element must be transformed */
      {
      /* create a FencedExpression element as a child of the MFENCED elem. */
      elType = TtaGetElementType (el);
      elType.ElTypeNum = MathML_EL_FencedExpression;
      fencedExpression = TtaNewElement (doc, elType);
      TtaInsertFirstChild (&fencedExpression, el, doc);
      if (child == NULL)
	/* empty MFENCED element */
	{
        elType.ElTypeNum = MathML_EL_Construct;
	child = TtaNewElement (doc, elType);
	TtaInsertFirstChild (&child, fencedExpression, doc);
	SetPlaceholderAttr (child, doc);
	}
      else
	{
        /* move the content of the MFENCED element within the new
	   FencedExpression element */
        prev = NULL;
	firstChild = NULL;
        while (child != NULL)
	  {
	  next = child;
	  TtaNextSibling (&next);
	  TtaRemoveTree (child, doc);
	  if (prev == NULL)
	    {
	    TtaInsertFirstChild (&child, fencedExpression, doc);
	    firstChild = child;
	    }
	  else
	    TtaInsertSibling (child, prev, FALSE, doc);
	  prev = child;
	  child = next;
	  }

	/* create FencedSeparator elements in the FencedExpression */
	CreateFencedSeparators (fencedExpression, doc);

        /* Create placeholders within the FencedExpression element */
        CreatePlaceholders (firstChild, doc);
	}

      /* create the OpeningFence element according to the open attribute */
      c = '(';
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = MathML_ATTR_open;
      attr = TtaGetAttribute (el, attrType);
      if (attr != NULL)
	{
        length = 7;
        TtaGiveTextAttributeValue (attr, text, &length);
	c = text[0];
	}
      elType.ElTypeNum = MathML_EL_OpeningFence;
      fence = TtaNewElement (doc, elType);
      TtaInsertSibling (fence, fencedExpression, TRUE, doc);
      elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
      leaf = TtaNewElement (doc, elType);
      TtaInsertFirstChild (&leaf, fence, doc);
      TtaSetGraphicsShape (leaf, c, doc);

      /* create the ClosingFence element according to close attribute */
      c = ')';
      attrType.AttrTypeNum = MathML_ATTR_close;
      attr = TtaGetAttribute (el, attrType);
      if (attr != NULL)
	{
        length = 7;
        TtaGiveTextAttributeValue (attr, text, &length);
	c = text[0];
	}
      elType.ElTypeNum = MathML_EL_ClosingFence;
      fence = TtaNewElement (doc, elType);
      TtaInsertSibling (fence, fencedExpression, FALSE, doc);
      elType.ElTypeNum = MathML_EL_SYMBOL_UNIT;
      leaf = TtaNewElement (doc, elType);
      TtaInsertFirstChild (&leaf, fence, doc);
      TtaSetGraphicsShape (leaf, c, doc);
      }
}

/*----------------------------------------------------------------------
   MathMLElementComplete
   Check the Thot structure of the MathML element el.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      MathMLElementComplete (Element el, Document doc)
#else
void      MathMLElementComplete (el, doc)
Element		el;
Document	doc;

#endif
{
   ElementType		elType, parentType;
   Element		child, parent, new, prev, next;
   AttributeType	attrType;
   Attribute		attr;
   SSchema       MathMLSSchema;

   elType = TtaGetElementType (el);
   MathMLSSchema = GetMathMLSSchema (doc);

   if (elType.ElSSchema != MathMLSSchema)
     /* this is not a MathML element. It's the HTML element <math>, or
	any other element containing a MathML expression */
     {
     if (TtaGetFirstChild (el) == NULL && !TtaIsLeaf (elType))
	/* this element is empty. Create a MathML element as it's child */
	{
	elType.ElSSchema = MathMLSSchema;
	elType.ElTypeNum = MathML_EL_MathML;
	new = TtaNewElement (doc, elType);
	TtaInsertFirstChild (&new, el, doc);
	/* Create a placeholder within the MathML element */
        elType.ElTypeNum = MathML_EL_Construct;
	child = TtaNewElement (doc, elType);
	TtaInsertFirstChild (&child, new, doc);
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = MathML_ATTR_placeholder;
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (child, attr, doc);
	TtaSetAttributeValue (attr, MathML_ATTR_placeholder_VAL_yes_, child, doc);
	}
     }
   else
     {
     switch (elType.ElTypeNum)
       {
       case MathML_EL_TEXT_UNIT:
	  CheckTextElement (&el, doc);
	  break;
       case MathML_EL_MI:
	  SetFontstyleAttr (el, doc);
	  break;
       case MathML_EL_MO:
	  SetAddspaceAttr (el, doc);
	  SetVertStretchAttr (el, doc, 0, NULL);
	  break;
       case MathML_EL_MROOT:
	  /* end of a Root. Create a RootBase and an Index */
	  CheckMathSubExpressions (el, MathML_EL_RootBase, MathML_EL_Index,
				   0, doc);
	  break;
       case MathML_EL_MSQRT:
	  /* end od a Square Root. Create a RootBase */
	  CheckMathSubExpressions (el, MathML_EL_RootBase, 0, 0, doc);
	  break;
       case MathML_EL_MFRAC:
	  /* end of a fraction. Create a Numerator and a Denominator */
	  CheckMathSubExpressions (el, MathML_EL_Numerator,
				 MathML_EL_Denominator, 0, doc);
	  break;
       case MathML_EL_MFENCED:
	  TransformMFENCED (el, doc);
	  break;
       case MathML_EL_MSUBSUP:
	  /* end of a MSUBSUP. Create Base, Subscript, and Superscript */
	  CheckMathSubExpressions (el, MathML_EL_Base, MathML_EL_Subscript,
				   MathML_EL_Superscript, doc);
	  SetVertStretchAttr (el, doc, MathML_EL_Base, NULL);
	  break;
       case MathML_EL_MSUB:
	  /* end of a MSUB. Create Base and Subscript */
	  CheckMathSubExpressions (el, MathML_EL_Base, MathML_EL_Subscript,
				   0, doc);
	  SetVertStretchAttr (el, doc, MathML_EL_Base, NULL);
	  break;
       case MathML_EL_MSUP:
	  /* end of a MSUP. Create Base and Superscript */
	  CheckMathSubExpressions (el, MathML_EL_Base, MathML_EL_Superscript,
				   0, doc);
	  SetVertStretchAttr (el, doc, MathML_EL_Base, NULL);
	  break;
       case MathML_EL_MUNDEROVER:
	  /* end of a MUNDEROVER. Create UnderOverBase, Underscript, and
	     Overscript */
	  CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
			  MathML_EL_Underscript, MathML_EL_Overscript, doc);
	  SetHorizStretchAttr (el, doc);
	  SetVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
	  break;
       case MathML_EL_MUNDER:
	  /* end of a MUNDER. Create UnderOverBase, and Underscript */
	  CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
				   MathML_EL_Underscript, 0, doc);
	  SetHorizStretchAttr (el, doc);
	  SetVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
	  break;
       case MathML_EL_MOVER:
	  /* end of a MOVER. Create UnderOverBase, and Overscript */
	  CheckMathSubExpressions (el, MathML_EL_UnderOverBase,
				   MathML_EL_Overscript, 0, doc);
	  SetHorizStretchAttr (el, doc);
	  SetVertStretchAttr (el, doc, MathML_EL_UnderOverBase, NULL);
	  break;
       case MathML_EL_MMULTISCRIPTS:
	  /* end of a MMULTISCRIPTS. Create all elements defined in the
	     MathML S schema */
	  BuildMultiscript (el, doc);
	  break;
       case MathML_EL_MTABLE:
	  /* end of a MTABLE. Create all elements defined in the MathML S
             schema */
	  CheckMTable (el, doc);
	  break;
       case MathML_EL_MROW:
	  /* end of MROW */
	  /*if the first and the last child are MO containing a fence character
	     transform the MO into a MF and the character into a Thot SYMBOL */
	  child = TtaGetFirstChild (el);
	  if (child != NULL)
	    {
	    CheckFence (child, doc);
	    child = TtaGetLastChild (el);
	    if (child != NULL)
	      CheckFence (child, doc);
	    /* Create placeholders within the MROW */
            CreatePlaceholders (TtaGetFirstChild (el), doc);
	    }
	  break;
       default:
	  break;
       }
     parent = TtaGetParent (el);
     parentType = TtaGetElementType (parent);
     if (parentType.ElSSchema != elType.ElSSchema)
        /* root of a MathML tree, Create a MathML element if there is no */
        if (elType.ElTypeNum != MathML_EL_MathML)
	  {
	  elType.ElSSchema = MathMLSSchema;
	  elType.ElTypeNum = MathML_EL_MathML;
	  new = TtaNewElement (doc, elType);
	  TtaInsertSibling (new, el, TRUE, doc);
	  next = el;
	  TtaNextSibling (&next);
	  TtaRemoveTree (el, doc);
	  TtaInsertFirstChild (&el, new, doc);
	  prev = el;
	  while (next != NULL)
	    {
	    child = next;
	    TtaNextSibling (&next);
	    TtaRemoveTree (child, doc);
	    TtaInsertSibling (child, prev, FALSE, doc);
	    prev = child;
	    }
	  /* Create placeholders within the MathML element */
	  CreatePlaceholders (el, doc);
	  }
     }
}

/*----------------------------------------------------------------------
   MathMLAttributeComplete
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      MathMLAttributeComplete (Attribute attr, Element el, Document doc)
#else
void      MathMLAttributeComplete (attr, el, doc)
Attribute	attr;
Element		el;
Document	doc;

#endif
{
}

/*----------------------------------------------------------------------
   MathMLGetDTDName
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void      MathMLGetDTDName (char* DTDname, char *elementName)
#else
void      MathMLGetDTDName (DTDname, elementName)
char* DTDname;
char *elementName;

#endif
{
   /* no other DTD allowed within MathML elements */
   strcpy (DTDname, "");
}

/* end of module */
