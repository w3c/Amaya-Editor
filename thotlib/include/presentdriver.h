/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * presentdriver.h : presentation driver, defines the presentation context
 *      used while parsing style sheets and the associated set of routines.
 *      This header file doesn't reflect the exact interface of one
 *      presentation driver but sumarize all the canonical properties :
 *
 *   - PresentationTarget : the object of a presentation manipulation.
 *   - PresentationContext : the context associated to the presentation
 *              informations. At least the common fields and how to browse them.
 *   - PresentationValue : the internal unit used to describe a value.
 *   - PresentationSetting : a presentation rule.
 */

#ifndef __PRESENT_DRIVER_H__
#define __PRESENT_DRIVER_H__

/*
 * PresentationTarget : Target of a presentation modification.
 */

typedef void       *PresentationTarget;		/* could be an union of typed pointers */

/*
 * PresentationContext : conditions influencing a presentation modification.
 */

typedef struct struct_PresentationStrategy *PtrPresentationStrategy;

typedef struct struct_PresentationContext
  {
     /*
      * MANDATORY : the first element must be a pointer
      *    to a strategy block holding the specific routines
      *    a descriptor for the current document and the
      *    associated schema structure.
      *    destroy indicate whether the parser will build the
      *    associated presentation or remove it.
      */
     PtrPresentationStrategy drv;
     Document            doc;	/* document number */
     SSchema             schema;	/* associated structure */
     int                 destroy;/* destructive mode ? */

     /*
      * The end of the block is to be filled with other kind
      * of informations needed to specify the conditions
      * influencing rendering.
      */
  }
                   *PresentationContext, PresentationContextBlock;


/*
 * PresentationValue : kind of value for a presentation attribute.
 */

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
#define DRIVERP_UNIT_FLOAT      (1 << 6)	/* Value is encoded via DP_FLOAT */

#define DRIVERP_UNIT_IS_FLOAT(u) (((u) & DRIVERP_UNIT_FLOAT) != 0)
#define DRIVERP_UNIT_SET_FLOAT(u) u = ((u) | DRIVERP_UNIT_FLOAT)
#define DRIVERP_UNIT_UNSET_FLOAT(u) u = ((u) & (~((int) DRIVERP_UNIT_FLOAT)))

typedef union _PresentationValue {
    int data;				/* some data without unit */
    struct {
        int                 value:24;	/* the value coded on 24 bits */
        int                 unit:8;	/* the unit  coded on 8 bits */
    } typed_data;
    void *pointer;			/* A pointer */
} PresentationValue;

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

#define DRIVERP_HIDE			1
#define DRIVERP_DISPLAY			2

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

/*
 * ApplyAllPresentationContext : function used to browse all the
 *      PresentationContext set up for a PresentationTarget. The
 *      handler is called on for all presentation contexts found
 *      on the target. Param is passed to the handler.
 */

typedef void        (*PresentationContextApplyHandler)
                    (PresentationTarget target, PresentationContext cond, void *param);

extern void         ApplyAllPresentationContext (PresentationTarget target,
		      PresentationContextApplyHandler handler, void *param);

/*
 * ApplyAllPresentationSettings : function used to browse all the
 *      PresentationSetting set up for a PresentationContext. The
 *      handler is called on for all presentation settings found
 *      on the target, within the given presentation context.
 */

typedef void        (*PresentationSettingsApplyHandler)
                    (PresentationTarget target, PresentationContext cond,
		     PresentationSetting setting, void *param);

extern void         ApplyAllPresentationSettings (PresentationTarget target,
	 PresentationContext cond, PresentationSettingsApplyHandler handler,
						  void *param);

/*
 * PresentationSetFunction : routine used to modify one kind of
 *           presentation attribute, eg foreground color.
 */

typedef int         (*PresentationSetFunction) (PresentationTarget target,
			   PresentationContext cond, PresentationValue val);

typedef int         (*PresentationSet2Function) (PresentationTarget target,
						 PresentationContext cond,
			    PresentationValue val1, PresentationValue val2);

/*
 * PresentationGetFunction : routine used to read one kind of
 *           presentation attribute, eg foreground color.
 */

typedef int         (*PresentationGetFunction) (PresentationTarget target,
			 PresentationContext cond, PresentationValue * val);

typedef int         (*PresentationGet2Function) (PresentationTarget target,
						 PresentationContext cond,
			PresentationValue * val1, PresentationValue * val2);

/*
 * Functions used to convert between external and internal presentation
 * representation, i.e PresentationValue's and PRule's .
 */

#ifdef __STDC__
void                PresentationValueToPRule (PresentationValue val,
                                              int type, PRule pRule,
					      int specific);
PresentationValue   PRuleToPresentationValue (PRule pRule);
void                PRuleToPresentationSetting (PRule pRule,
                                                PresentationSetting setting,
						int extra);

#else
void                PresentationValueToPRule (	/* PresentationValue val,
                                                   int type, PRule pRule,
						   int specific */ );
PresentationValue   PRuleToPresentationValue ( /* PRule pRule */ );
void                PRuleToPresentationSetting ( /* PRule pRule,
                                                    PresentationSetting setting,
						    int extra */ );

#endif

/*
 * PresentationStrategy : set of routines offered in standard
 *          by a presentation driver. This is a list of reading and
 *          modifying routines for each kind of attributes.
 */

typedef struct struct_PresentationStrategy
  {
     PresentationSetFunction CleanPresentation;
     PresentationSetFunction UpdatePresentation;

     PresentationGetFunction GetForegroundColor;
     PresentationSetFunction SetForegroundColor;

     PresentationGetFunction GetBackgroundColor;
     PresentationSetFunction SetBackgroundColor;

     PresentationGetFunction GetFontSize;
     PresentationSetFunction SetFontSize;

     PresentationGetFunction GetFontStyle;
     PresentationSetFunction SetFontStyle;

     PresentationGetFunction GetFontFamily;
     PresentationSetFunction SetFontFamily;

     PresentationGetFunction GetTextUnderlining;
     PresentationSetFunction SetTextUnderlining;

     PresentationGetFunction GetAlignment;
     PresentationSetFunction SetAlignment;

     PresentationGetFunction GetLineSpacing;
     PresentationSetFunction SetLineSpacing;

     PresentationGetFunction GetIndent;
     PresentationSetFunction SetIndent;

     PresentationGetFunction GetJustification;
     PresentationSetFunction SetJustification;

     PresentationGetFunction GetHyphenation;
     PresentationSetFunction SetHyphenation;

     PresentationGetFunction GetFillPattern;
     PresentationSetFunction SetFillPattern;

     PresentationGetFunction GetVPos;
     PresentationSetFunction SetVPos;

     PresentationGetFunction GetHPos;
     PresentationSetFunction SetHPos;

     PresentationGetFunction GetHeight;
     PresentationSetFunction SetHeight;

     PresentationGetFunction GetRelHeight;
     PresentationSetFunction SetRelHeight;

     PresentationGetFunction GetWidth;
     PresentationSetFunction SetWidth;

     PresentationGetFunction GetRelWidth;
     PresentationSetFunction SetRelWidth;

     PresentationGetFunction GetInLine;
     PresentationSetFunction SetInLine;

     PresentationGetFunction GetShow;
     PresentationSetFunction SetShow;

     PresentationGetFunction GetBox;
     PresentationSetFunction SetBox;

     PresentationGetFunction GetShowBox;
     PresentationSetFunction SetShowBox;

     PresentationGetFunction GetBgImage;
     PresentationSetFunction SetBgImage;

     PresentationGetFunction GetPictureMode;
     PresentationSetFunction SetPictureMode;
  }
PresentationStrategy;

#endif /* __PRESENT_DRIVER_H__ */
