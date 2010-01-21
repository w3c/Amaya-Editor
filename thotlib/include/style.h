/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * style.h : defines the presentation context and function used to
 * style Thot documents. Style could be specific or generic.
 * Specific style is attached to an Element and generic style is attached
 * to an element type, an attribute within a PSchema.
 *
 *   - PresentationType: the style property
 *   - Element or PSchema of a presentation manipulation.
 *   - PresentationContext: the context gives associated information
 *   - PresentationValue: the value and its unit.
 */

#ifndef __STYLE_H__
#define __STYLE_H__
#define MAX_ANCESTORS 15

struct unit_def
{
   const char         *sign;
   unsigned int        unit;
};

extern struct unit_def CSSUnitNames[];

typedef enum
{
  Txtmatch,
  Txtsubstring,
  Txtword
} AttrMatch;

typedef enum
{
  RelAncestor,
  RelParent,
  RelPrevious,
  RelVoid
} ElemRel;

typedef enum
{
  PbNone,
  PbBefore,
  PbAfter
} PseudoBox;

/* two different contexts for generic and specific presentation */
typedef struct struct_GenericContext
  {
    Document      doc;	          /* document number */
    SSchema       schema;         /* associated structure */
    unsigned int  type;           /* type of element */
    int           cssSpecificity; /* specificity according to selector */
    int           uses;           /* number of current uses */
    int           cssLine;        /* the line number of the CSS rule */
    char         *cssURL;         /* points to the current CSS URL */
    ThotBool      important;      /* important rule */
    ThotBool      destroy;        /* destructive mode ? */
    /*
     * below is the context description.
     */
    int           box;	                 /* specific presentation box if any */
    int           var;                      /* variable number for pseudo box*/
    PseudoBox     pseudo;                   /* pseudo box in selector    */
    int           nbElem;                   /* number of elem. in name[] */
    int           name[MAX_ANCESTORS];      /* ancestors type            */
    ThotBool      firstChild[MAX_ANCESTORS];/* first:child pseudo-class  */
    int           attrType[MAX_ANCESTORS];  /* type of the attribute     */
    int           attrLevel[MAX_ANCESTORS]; /* level of the attribute    */
    char         *attrText[MAX_ANCESTORS];  /* attr. or box name         */
    AttrMatch     attrMatch[MAX_ANCESTORS]; /* how attr. values match    */
    ElemRel       rel[MAX_ANCESTORS];       /* element relation          */
    char          sel[MAX_ANCESTORS * 50];  /* include selected values   */
  }
GenericContextBlock, *GenericContext;

typedef struct struct_SpecificContext
  {
    Document      doc;	          /* document number */
    SSchema       schema;         /* associated structure */
    int           type;           /* type of element */
    int           cssSpecificity; /* For specific rules: > 0 when the rule
                                     translates a CSS style rule */
    int           uses;           /* number of current uses */
    int           cssLine;        /* the line number of the CSS rule */
    char         *cssURL;         /* points to the current CSS URL */
    ThotBool      important;      /* important rule */
    ThotBool      destroy;        /* destructive mode ? */
    /*
     * The end of the block is to be filled with other kind
     * of informations needed to specify the conditions
     * influencing rendering.
     */
  }
PresentationContextBlock, *PresentationContext;

typedef struct _PresentationValue
 {
   int                 data;          /* some data without unit */
   struct
   {
     int               value;         /* the value */
     int               unit;          /* the unit */
     ThotBool          real;          /* the value is float like 115.5 */
     ThotBool          mainValue;     /* FALSE: value is an increment (only
					 for position rules) */
   } typed_data;
   void               *pointer;       /* A pointer */
}
PresentationValue;


/*
 * PresentationSettings : structure returned by GetNextPresentationSettings
 *      indicating a presentation currently defined in a given context.
 */
typedef struct _PresentationSetting
  {
     unsigned int        type;	/* type of the presentation rule returned */
     PresentationValue   value;	/* the presentation value (type + value)  */
  }
*PresentationSetting, PresentationSettingBlock;

typedef void (*SettingsApplyHandler) (Element element,
				      Document doc,
				      PresentationSetting setting,
				      void *param);


/*----------------------------------------------------------------------
  GetGenericStyleContext : user level function needed to allocate and
  initialize a GenericContext.
  ----------------------------------------------------------------------*/
extern GenericContext TtaGetGenericStyleContext (Document doc);

/*----------------------------------------------------------------------
  GetSpecificStyleContext : user level function needed to allocate and
  initialize a SpecificContext.
  ---------------------------------------------------------------------*/
extern PresentationContext TtaGetSpecificStyleContext (Document doc);

/*----------------------------------------------------------------------
  TtaSetStylePresentation attachs a style rule to an element or to an
  extended presentation schema.
  ----------------------------------------------------------------------*/
extern int TtaSetStylePresentation (unsigned int type, Element el, PSchema tsch,
				    PresentationContext c, PresentationValue v);

/*----------------------------------------------------------------------
  TtaGetStylePresentation returns the style rule attached to an element
  or to an extended presentation schema.
  ----------------------------------------------------------------------*/
extern int TtaGetStylePresentation (unsigned int type, Element el, PSchema tsch,
				    PresentationContext c, PresentationValue *v);

/*----------------------------------------------------------------------
  ApplyAllSpecificSettings browses all the PRules structures,
  associated to the corresponding Specific Context 
  structure, and calls the given handler for each one.
  ----------------------------------------------------------------------*/
extern void TtaApplyAllSpecificSettings (Element element, Document doc,
					 SettingsApplyHandler handler, void *param);

/*----------------------------------------------------------------------
  TtaCleanElementPresentation
  Remove all specific presentation rules attached to element el
  in document doc
  ----------------------------------------------------------------------*/
extern void TtaCleanElementPresentation (Element el, Document doc);

/*----------------------------------------------------------------------
  TtaGetStyledAttributeValues returns the list of values of the attribute
  attrType which have an entry in the presentation schema tsch.
  ----------------------------------------------------------------------*/
extern char *TtaGetStyledAttributeValues (PSchema tsch, int attrType);

/*----------------------------------------------------------------------
 TtaPToCss:  translate a PresentationSetting to the
     equivalent CSS string, and add it to the buffer given as the
     argument. It is used when extracting the CSS string from actual
     presentation.
     el is the element for which the style rule is generated
 
  All the possible values returned by the presentation drivers are
  described in thotlib/include/presentation.h
 -----------------------------------------------------------------------*/
extern void TtaPToCss (PresentationSetting settings, char *buffer, int len,
		       Element el, void* pSchP);

/*----------------------------------------------------------------------
   TtaListStyleSchemas
   Produces in a file a human-readable form of style schemas applied to 
   the current document.
   Parameters:
   document: the document.
   fileDescriptor: file descriptor of the file that will contain the list.
   This file must be open when calling the function.
  ----------------------------------------------------------------------*/
extern void TtaListStyleSchemas (Document document, FILE * fileDescriptor);

/*----------------------------------------------------------------------
   TtaListStyleOfCurrentElement
   Produces in a file a human-readable form of style rules applied to 
   the first selected element.
   Parameters:
   document: the document.
   el: the element.
   fileDescriptor: file descriptor of the file that will contain the list.
   This file must be open when calling the function.
   Returns the number of rules generated.
  ----------------------------------------------------------------------*/
extern int TtaListStyleOfCurrentElement (Document document, FILE *fileDescriptor);

#endif /* __STYLE_H__ */
