/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * presentdriver.h : presentation driver, defines the presentation context
 *      used while parsing style sheets and the associated set of routines.
 *      This header file doesn't reflect the exact interface of one
 *      presentation driver but sumarize all the canonical properties :
 *
 *   - Element or PSchema of a presentation manipulation.
 *   - PresentationContext : the context associated to the presentation
 *              informations. At least the common fields and how to browse them.
 *   - PresentationValue : the internal unit used to describe a value.
 *   - PresentationSetting : a presentation rule.
 */

#ifndef __PRESENT_DRIVER_H__
#define __PRESENT_DRIVER_H__
#define MAX_ANCESTORS 10

/* two different context for generic and specific presentation */
typedef struct struct_GenericContext
  {
     Document              doc;	     /* document number */
     SSchema               schema;   /* associated structure */
     unsigned int          type;     /* type of element */
     boolean               destroy;  /* destructive mode ? */
     /*
      * below is the context description.
      */
     int                   box;	     /* specific presentation box if any */
     int                   attr;     /* or attribute */
     int                   attrval;  /* and the corresponding value */
     char                 *class;    /* class or box name */
     int                   classattr;/* class attribute */
     int                   attrelem; /* elem porting the attribute */
     int                   ancestors[MAX_ANCESTORS];	/* ancestors type */
     int                   ancestors_nb[MAX_ANCESTORS];	/* number for each */
  }
GenericContextBlock, *GenericContext;

/* PresentationContext : conditions influencing a presentation modification */
typedef struct struct_PresentationStrategy *PtrPresentationStrategy;
typedef struct struct_PresentationContext
  {
     Document                doc;	/* document number */
     SSchema                 schema;	/* associated structure */
     int                     type;      /* type of element */
     boolean                 destroy;   /* destructive mode ? */

     /*
      * The end of the block is to be filled with other kind
      * of informations needed to specify the conditions
      * influencing rendering.
      */
  }
PresentationContextBlock, *PresentationContext;

/* PresentationValue : kind of value for a presentation attribute */
#define DP_FLOAT(f) ((int) ((float) (f)) * 1000)

#define DRIVERP_UNIT_INVALID	0	/* invalid unit : for parsing   */
#define DRIVERP_UNIT_REL	1	/* relative : e.g. to font size */
#define DRIVERP_UNIT_PT		2	/* typo points : 1/72 inches    */
#define DRIVERP_UNIT_PC		3	/* typo points : 1/6 inches     */
#define DRIVERP_UNIT_IN		4	/* inches                       */
#define DRIVERP_UNIT_CM		5	/* centimeters                  */
#define DRIVERP_UNIT_MM		6	/* millimeters                  */
#define DRIVERP_UNIT_EM		7	/* defined / size of M char     */
#define DRIVERP_UNIT_PX		8	/* defined in pixel, bad :-(    */
#define DRIVERP_UNIT_PERCENT	9	/* a relative size in percent   */
#define DRIVERP_UNIT_XHEIGHT	10	/* defined / size of x char     */
#define DRIVERP_UNIT_BOX	11	/* this is a box number         */

typedef union _PresentationValue
 {
   int                 data;          /* some data without unit */
   struct
   {
     int               value;         /* the value */
     int               unit;          /* the unit */
     boolean           real;          /* the value is float like 115.5 */
   } typed_data;
   void               *pointer;       /* A pointer */
}
PresentationValue;


/*
 * PresentationSettings : structure returned by GetNextPresentationSettings
 *      indicating a presentation currently defined in a given context.
 */
typedef enum
  {
     DRIVERP_NONE,
     DRIVERP_FOREGROUND_COLOR,
     DRIVERP_BACKGROUND_COLOR,
     DRIVERP_FONT_SIZE,
     DRIVERP_FONT_STYLE,
     DRIVERP_FONT_FAMILY,
     DRIVERP_TEXT_UNDERLINING,
     DRIVERP_ALIGNMENT,
     DRIVERP_LINE_SPACING,
     DRIVERP_INDENT,
     DRIVERP_JUSTIFICATION,
     DRIVERP_HYPHENATION,
     DRIVERP_FILL_PATTERN,
     DRIVERP_VERTICAL_POSITION,
     DRIVERP_HORIZONTAL_POSITION,
     DRIVERP_HEIGHT,
     DRIVERP_RELATIVE_HEIGHT,
     DRIVERP_WIDTH,
     DRIVERP_RELATIVE_WIDTH,
     DRIVERP_IN_LINE,
     DRIVERP_SHOW,
     DRIVERP_BOX,
     DRIVERP_SHOWBOX,
     DRIVERP_BGIMAGE,
     DRIVERP_PICTUREMODE
  }
PresentationType;

typedef struct _PresentationSetting
  {
     PresentationType    type;	/* type of the presentation rule returned */
     PresentationValue   value;	/* the presentation value (type + value)  */
  }
                   *PresentationSetting, PresentationSettingBlock;

/*
 * A bunch of constants describing the most common values for presentation
 * attributes, like font families, styles, etc ...
 */

#define DRIVERP_FONT_ROMAN		StyleRoman
#define DRIVERP_FONT_BOLD		StyleBold
#define DRIVERP_FONT_ITALICS		StyleItalics
#define DRIVERP_FONT_OBLIQUE		StyleOblique
#define DRIVERP_FONT_BOLDITALICS	StyleBoldItalics
#define DRIVERP_FONT_BOLDOBLIQUE	StyleBoldOblique

#define DRIVERP_FONT_TIMES		FontTimes
#define DRIVERP_FONT_HELVETICA		FontHelvetica
#define DRIVERP_FONT_COURIER		FontCourier

#define DRIVERP_NOUNDERLINE		1
#define DRIVERP_UNDERLINE		2
#define DRIVERP_OVERLINE		3
#define DRIVERP_CROSSOUT		4

#define DRIVERP_THINUNDERLINE		1
#define DRIVERP_THICKUNDERLINE		2

#define DRIVERP_ADJUSTLEFT		1
#define DRIVERP_ADJUSTRIGHT		2
#define DRIVERP_ADJUSTCENTERED		3
#define DRIVERP_ADJUSTLEFTWITHDOTS	4

#define DRIVERP_JUSTIFIED		1
#define DRIVERP_NOTJUSTIFIED		2

#define DRIVERP_LINESOLID		1
#define DRIVERP_LINEDASHED		2
#define DRIVERP_LINEDOTTED		3

#define DRIVERP_HYPHENATE		1
#define DRIVERP_NOHYPHENATE		2

#define DRIVERP_HIDE			0
#define DRIVERP_DISPLAY			5

#define DRIVERP_INLINE			1
#define DRIVERP_NOTINLINE		2

#define DRIVERP_REALSIZE		1
#define DRIVERP_SCALE			2
#define DRIVERP_REPEAT			3
#define DRIVERP_VREPEAT			4
#define DRIVERP_HREPEAT			5

/* keep this in sync with $THOTDIR/config/thot.pattern */
#define DRIVERP_PATTERN_NONE		0
#define DRIVERP_PATTERN_FOREGROUND	1
#define DRIVERP_PATTERN_BACKGROUND	2
#define DRIVERP_PATTERN_GRAY0		3
#define DRIVERP_PATTERN_GRAY1		4
#define DRIVERP_PATTERN_GRAY2		5
#define DRIVERP_PATTERN_GRAY3		6
#define DRIVERP_PATTERN_GRAY4		7
#define DRIVERP_PATTERN_GRAY5		8
#define DRIVERP_PATTERN_GRAY6		9
#define DRIVERP_PATTERN_GRAY7		10
#define DRIVERP_PATTERN_GRAY8		11
#define DRIVERP_PATTERN_HORIZ1		12
#define DRIVERP_PATTERN_HORIZ2		13
#define DRIVERP_PATTERN_HORIZ3		14
#define DRIVERP_PATTERN_VERT1		15
#define DRIVERP_PATTERN_VERT2		16
#define DRIVERP_PATTERN_VERT3		17
#define DRIVERP_PATTERN_LEFT1		18
#define DRIVERP_PATTERN_LEFT2		19
#define DRIVERP_PATTERN_LEFT3		20
#define DRIVERP_PATTERN_RIGHT1		21
#define DRIVERP_PATTERN_RIGHT2		22
#define DRIVERP_PATTERN_RIGHT3		23
#define DRIVERP_PATTERN_SQUARE1		24
#define DRIVERP_PATTERN_SQUARE2		25
#define DRIVERP_PATTERN_SQUARE3		26
#define DRIVERP_PATTERN_LOZENGE		27
#define DRIVERP_PATTERN_BRICK		28
#define DRIVERP_PATTERN_TILE		29
#define DRIVERP_PATTERN_SEA		30

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
  Function used to update the drawing after styling an element or a
  generic type.
  ----------------------------------------------------------------------*/
extern void TtaUpdateStylePresentation (Element el, PSchema tsch, PresentationContext c);

#else  /* __STDC__ */
extern GenericContext TtaGetGenericStyleContext (/* doc */)
extern PresentationContext TTaGetSpecificStyleContext (/* doc */)
extern int TtaSetStylePresentation (/* unsigned int type, Element el, PSchema tsch, PresentationContext c, PresentationValue v */);
extern int TtaGetStylePresentation (/* unsigned int type, Element el, PSchema tsch, PresentationContext c, PresentationValue *v */);
extern void TtaApplyAllSpecificSettings (/* Element element, Document doc, SettingsApplyHandler handler, void *param */);
extern void TtaCleanStylePresentation (/* Element el, PSchema tsch, Document doc */);
extern void TtaUpdateStylePresentation (/* Element el, PSchema tsch, PresentationContext c */);

#endif /* __STDC__ */

#endif /* __PRESENT_DRIVER_H__ */
