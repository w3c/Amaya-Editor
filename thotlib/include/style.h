/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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

/* two different contexts for generic and specific presentation */
typedef struct struct_GenericContext
  {
     Document              doc;	     /* document number */
     SSchema               schema;   /* associated structure */
     unsigned int          type;     /* type of element */
     ThotBool              destroy;  /* destructive mode ? */
     /*
      * below is the context description.
      */
     int                   box;	     /* specific presentation box if any */
     int                   name[MAX_ANCESTORS]; /* ancestors type */
     int                   names_nb[MAX_ANCESTORS]; /* number of occurences */
     int                   attrType[MAX_ANCESTORS]; /* type of the attribute */
     CHAR_T*               attrText[MAX_ANCESTORS]; /* attr. or box name */
  }
GenericContextBlock, *GenericContext;

typedef struct struct_SpecificContext
  {
     Document              doc;	     /* document number */
     SSchema               schema;   /* associated structure */
     int                   type;     /* type of element */
     ThotBool              destroy;  /* destructive mode ? */

     /*
      * The end of the block is to be filled with other kind
      * of informations needed to specify the conditions
      * influencing rendering.
      */
  }
PresentationContextBlock, *PresentationContext;

/* Units */
#define STYLE_UNIT_INVALID	0	/* invalid unit : for parsing   */
#define STYLE_UNIT_REL		1	/* relative : e.g. to font size */
#define STYLE_UNIT_PT		2	/* typo points : 1/72 inches    */
#define STYLE_UNIT_PC		3	/* typo points : 1/6 inches     */
#define STYLE_UNIT_IN		4	/* inches                       */
#define STYLE_UNIT_CM		5	/* centimeters                  */
#define STYLE_UNIT_MM		6	/* millimeters                  */
#define STYLE_UNIT_EM		7	/* defined / size of M char     */
#define STYLE_UNIT_PX		8	/* defined in pixel, bad :-(    */
#define STYLE_UNIT_PERCENT	9	/* a relative size in percent   */
#define STYLE_UNIT_XHEIGHT	10	/* defined / size of x char     */
#define STYLE_UNIT_BOX	11	/* this is a box number         */

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

/*
 * A bunch of constants describing the most common values for presentation
 * attributes, like font families, styles, etc ...
 */

#define STYLE_FONT_TIMES		FontTimes
#define STYLE_FONT_HELVETICA		FontHelvetica
#define STYLE_FONT_COURIER		FontCourier

#define STYLE_FONT_ROMAN		StyleRoman
#define STYLE_FONT_ITALICS		StyleItalics
#define STYLE_FONT_OBLIQUE		StyleOblique

#define STYLE_WEIGHT_BOLD		WeightBold
#define STYLE_WEIGHT_NORMAL		WeightNormal

#define STYLE_NOUNDERLINE		1
#define STYLE_UNDERLINE			2
#define STYLE_OVERLINE			3
#define STYLE_CROSSOUT			4

#define STYLE_THINUNDERLINE		1
#define STYLE_THICKUNDERLINE		2

#define STYLE_LINESOLID			1
#define STYLE_LINEDASHED		2
#define STYLE_LINEDOTTED		3

#define STYLE_BORDERNONE		1
#define STYLE_BORDERHIDDEN		2
#define STYLE_BORDERDOTTED		3
#define STYLE_BORDERDASHED		4
#define STYLE_BORDERSOLID		5
#define STYLE_BORDERDOUBLE		6
#define STYLE_BORDERGROOVE		7
#define STYLE_BORDERRIDGE		8
#define STYLE_BORDERINSET		9
#define STYLE_BORDEROUTSET		10

#define STYLE_JUSTIFIED			1
#define STYLE_NOTJUSTIFIED		2

#define STYLE_HYPHENATE			1
#define STYLE_NOHYPHENATE		2

#define STYLE_ADJUSTLEFT		1
#define STYLE_ADJUSTRIGHT		2
#define STYLE_ADJUSTCENTERED		3
#define STYLE_ADJUSTLEFTWITHDOTS	4

#define STYLE_INLINE			1
#define STYLE_NOTINLINE			2

#define STYLE_HIDE			0
#define STYLE_DISPLAY			5

#define STYLE_REALSIZE			1
#define STYLE_SCALE			2
#define STYLE_REPEAT			3
#define STYLE_VREPEAT			4
#define STYLE_HREPEAT			5

/* keep this in sync with $THOTDIR/config/thot.pattern */
#define STYLE_PATTERN_NONE		0
#define STYLE_PATTERN_FOREGROUND	1
#define STYLE_PATTERN_BACKGROUND	2
#define STYLE_PATTERN_GRAY0		3
#define STYLE_PATTERN_GRAY1		4
#define STYLE_PATTERN_GRAY2		5
#define STYLE_PATTERN_GRAY3		6
#define STYLE_PATTERN_GRAY4		7
#define STYLE_PATTERN_GRAY5		8
#define STYLE_PATTERN_GRAY6		9
#define STYLE_PATTERN_GRAY7		10
#define STYLE_PATTERN_GRAY8		11
#define STYLE_PATTERN_HORIZ1		12
#define STYLE_PATTERN_HORIZ2		13
#define STYLE_PATTERN_HORIZ3		14
#define STYLE_PATTERN_VERT1		15
#define STYLE_PATTERN_VERT2		16
#define STYLE_PATTERN_VERT3		17
#define STYLE_PATTERN_LEFT1		18
#define STYLE_PATTERN_LEFT2		19
#define STYLE_PATTERN_LEFT3		20
#define STYLE_PATTERN_RIGHT1		21
#define STYLE_PATTERN_RIGHT2		22
#define STYLE_PATTERN_RIGHT3		23
#define STYLE_PATTERN_SQUARE1		24
#define STYLE_PATTERN_SQUARE2		25
#define STYLE_PATTERN_SQUARE3		26
#define STYLE_PATTERN_LOZENGE		27
#define STYLE_PATTERN_BRICK		28
#define STYLE_PATTERN_TILE		29
#define STYLE_PATTERN_SEA		30

typedef void (*SettingsApplyHandler) (Element element,
				      Document doc,
				      PresentationSetting setting,
				      void *param);


#ifdef __STDC__
/*----------------------------------------------------------------------
  GetGenericStyleContext : user level function needed to allocate and
  initialize a GenericContext.
  ----------------------------------------------------------------------*/
extern GenericContext TtaGetGenericStyleContext (Document doc);

/*----------------------------------------------------------------------
  GetSpecificStyleContext : user level function needed to allocate and
  initialize a SpecificContext.
  ----------------------------------------------------------------------*/
extern PresentationContext TtaGetSpecificStyleContext (Document doc);

/*----------------------------------------------------------------------
  TtaSetStylePresentation attachs a style rule to an element or to an
  extended presentation schema.
  ----------------------------------------------------------------------*/
extern int TtaSetStylePresentation (unsigned int type, Element el, PSchema tsch, PresentationContext c, PresentationValue v);

/*----------------------------------------------------------------------
  TtaGetStylePresentation returns the style rule attached to an element
  or to an extended presentation schema.
  ----------------------------------------------------------------------*/
extern int TtaGetStylePresentation (unsigned int type, Element el, PSchema tsch, PresentationContext c, PresentationValue *v);

/*----------------------------------------------------------------------
  ApplyAllSpecificSettings browses all the PRules structures,
  associated to the corresponding Specific Context 
  structure, and calls the given handler for each one.
  ----------------------------------------------------------------------*/
extern void TtaApplyAllSpecificSettings (Element element, Document doc, SettingsApplyHandler handler, void *param);

/*----------------------------------------------------------------------
  Function used to remove all presentation for a given element or an
  extended presentation schema
  ----------------------------------------------------------------------*/
extern void TtaCleanStylePresentation (Element el, PSchema tsch, Document doc );

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

#else  /* __STDC__ */
extern GenericContext TtaGetGenericStyleContext (/* doc */)
extern PresentationContext TTaGetSpecificStyleContext (/* doc */)
extern int TtaSetStylePresentation (/* unsigned int type, Element el, PSchema tsch, PresentationContext c, PresentationValue v */);
extern int TtaGetStylePresentation (/* unsigned int type, Element el, PSchema tsch, PresentationContext c, PresentationValue *v */);
extern void TtaApplyAllSpecificSettings (/* Element element, Document doc, SettingsApplyHandler handler, void *param */);
extern void TtaCleanStylePresentation (/* Element el, PSchema tsch, Document doc */);
extern void TtaListStyleSchemas (/* Document document, FILE * fileDescriptor */);
#endif /* __STDC__ */

#endif /* __STYLE_H__ */
