/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2003
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
#define MAX_ANCESTORS 5

typedef enum
{
  Txtmatch,
  Txtsubstring,
  Txtword
} AttrMatch;

/* two different contexts for generic and specific presentation */
typedef struct struct_GenericContext
  {
    Document      doc;	          /* document number */
    SSchema       schema;         /* associated structure */
    unsigned int  type;           /* type of element */
    int           cssSpecificity; /* specificity according to selector */
    int           uses;           /* number of current uses */
    ThotBool      important;      /* important rule */
    ThotBool      destroy;        /* destructive mode ? */
    /*
     * below is the context description.
     */
    int           box;	                 /* specific presentation box if any */
    int           name[MAX_ANCESTORS];      /* ancestors type */
    int           names_nb[MAX_ANCESTORS];  /* number of occurences */
    int           attrType[MAX_ANCESTORS];  /* type of the attribute */
    int           attrLevel[MAX_ANCESTORS]; /* level of the attribute */
    char         *attrText[MAX_ANCESTORS];  /* attr. or box name */
    AttrMatch     attrMatch[MAX_ANCESTORS]; /* how attr. values match */
    char          sel[MAX_ANCESTORS * 50];  /* include selected values */
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
    ThotBool      important;      /* important rule */
    ThotBool      destroy;        /* destructive mode ? */
    /*
     * The end of the block is to be filled with other kind
     * of informations needed to specify the conditions
     * influencing rendering.
     */
  }
PresentationContextBlock, *PresentationContext;

typedef union _PresentationValue
 {
   int                 data;          /* some data without unit */
   struct
   {
     int               value;         /* the value */
     int               unit;          /* the unit */
     ThotBool          real;          /* the value is float like 115.5 */
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
  TtaCleanStylePresentation
  Remove all presentation for the extended presentation schema tsch associated
  with structure schema sSch in document doc
  ----------------------------------------------------------------------*/
extern void TtaCleanStylePresentation (PSchema tsch, Document doc, SSchema sSch);

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

#endif /* __STYLE_H__ */
