/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _PRESENTATION_H_
#define _PRESENTATION_H_
#include "view.h"
/* presentation rule */
typedef int        *PRule;

/* Types of presentation rule */
/* These values must follow the order of PRuleType defined in typeprs.h */

#define PRVisibility 0
#define PRFunction 1
#define PRListStyleType 2
#define PRListStyleImage 3
#define PRListStylePosition 4
#define PRVertOverflow 5
#define PRHorizOverflow 6
#define PRVertRef 7
#define PRHorizRef 8
#define PRHeight 9
#define PRWidth 10
#define PRVertPos 11
#define PRHorizPos 12
#define PRMarginTop 13
#define PRMarginRight 14
#define PRMarginBottom 15
#define PRMarginLeft 16
#define PRPaddingTop 17
#define PRPaddingRight 18
#define PRPaddingBottom 19
#define PRPaddingLeft 20
#define PRBorderTopWidth 21
#define PRBorderRightWidth 22
#define PRBorderBottomWidth 23
#define PRBorderLeftWidth 24
#define PRBorderTopColor 25
#define PRBorderRightColor 26
#define PRBorderBottomColor 27
#define PRBorderLeftColor 28
#define PRBorderTopStyle 29
#define PRBorderRightStyle 30
#define PRBorderBottomStyle 31
#define PRBorderLeftStyle 32
#define PRSize 33
#define PRStyle 34
#define PRWeight 35
#define PRVariant 36
#define PRFont 37
#define PRUnderline 38
#define PRThickness 39
#define PRIndent 40
#define PRLineSpacing 41
#define PRDepth 42
#define PRAdjust 43
#define PRDirection 44
#define PRUnicodeBidi 45
#define PRLineStyle 46
#define PRLineWeight 47
#define PRFillPattern 48
#define PRBackground 49
#define PRForeground 50
#define PRColor 51
#define PRStopColor 52
#define PROpacity 53
#define PRFillOpacity 54
#define PRStrokeOpacity 55
#define PRStopOpacity 56
#define PRHyphenate 57
/* PtPageBreak, PtLineBreak, PtGather */
#define PRXRadius 61
#define PRYRadius 62
#define PRPosition 63
#define PRTop 64
#define PRRight 65
#define PRBottom 66
#define PRLeft 67
#define PRFloat 68
#define PRClear 69
#define PRDisplay 70
#define PRBackgroundHorizPos 71
#define PRBackgroundVertPos 72
#define PRVis 73 // CSS visibility
#define PRNoBreak1 74
#define PRNoBreak2 75
/* PtPictInfo */
#define PRCreateEnclosing 76
#define PRShowBox 77
#define PRBackgroundPicture 78
#define PRBackgroundRepeat 79
#define PRNotInLine 80
#define PRNone 81
#define PRPageBefore 82
#define PRPageAfter 83
#define PRPageInside 84
#define PRContent 85
#define PRContentString 86
#define PRContentURL 87
#define PRContentAttr 88
#define PRContentCounter 89
#define PRContentCounterStyle 90
#define PRCounterReset 91
#define PRCounterIncrement 92
#define PRFillRule 93
#define PRMarker 94
#define PRMarkerEnd 95
#define PRMarkerMid 96
#define PRMarkerStart 97
#define LAST_PRES_RULE_TYPE 98

/*
 * A bunch of constants describing the most common values for presentation
 * attributes, like font families, styles, etc ...
 */

/* Units */
#define UNIT_INVALID	0	/* invalid unit : for parsing   */
#define UNIT_REL	1	/* relative : e.g. to font size */
#define UNIT_PT		2	/* typo points : 1/72 inches    */
#define UNIT_PC		3	/* typo points : 1/6 inches     */
#define UNIT_IN		4	/* inches                       */
#define UNIT_CM		5	/* centimeters                  */
#define UNIT_MM		6	/* millimeters                  */
#define UNIT_EM		7	/* defined / size of char m     */
#define UNIT_PX		8	/* defined in pixels            */
#define UNIT_PERCENT	9	/* a relative size in percent   */
#define UNIT_XHEIGHT	10	/* defined / size of x char     */
#define UNIT_BOX	11	/* this is a box number         */
#define VALUE_AUTO      12      /* not a unit: value = auto     */
#define VALUE_INHERIT   13      /* not a unit: value = inherit  */
#define VALUE_CURRENT   14      /* not a unit: value = currentColor  */
#define VALUE_URL       15      /* not a unit: value = url(...)  */

/* values for rules PRBorderStyle */
#define BorderStyleNone 1
#define BorderStyleHidden 2
#define BorderStyleDotted 3
#define BorderStyleDashed 4
#define BorderStyleSolid 5
#define BorderStyleDouble 6
#define BorderStyleGroove 7
#define BorderStyleRidge 8
#define BorderStyleInset 9
#define BorderStyleOutset 10

/* values for rules PRFloat */
#define FloatNone          1
#define FloatLeft          2
#define FloatRight	   3

/* values for rules PRClear */
#define ClearNone          1
#define ClearLeft          2
#define ClearRight	   3
#define ClearBoth	   4

/* values for rule PRPosition */
#define PositionStatic     1
#define PositionRelative   2
#define PositionAbsolute   3
#define PositionFixed      4

/* values for PRHorizPos and PRVertPos */
#define PositionTop 1
#define PositionBottom 2
#define PositionLeft 3
#define PositionRight 4
#define PositionHorizRef 5
#define PositionVertRef 6
#define PositionHorizMiddle 7
#define PositionVertMiddle 8

/* values for rule PRStyle */
#define StyleRoman 1
#define StyleItalics 2
#define StyleOblique 3

/* values for rule PRWeight */
#define WeightNormal 1
#define WeightBold 2

/* values for rule PRVariant */
#define VariantNormal 1
#define VariantSmallCaps 2
#define VariantDoubleStruck 3
#define VariantFraktur 4
#define VariantScript 5

/* values for rule PRFont */
#define FontTimes 1
#define FontHelvetica 2
#define FontCourier 3
#define FontOther 4

#define REALSIZE		1
#define SCALE			2
#define REPEAT			3
#define XREPEAT			4
#define YREPEAT			5

/* values for rule PRUnderline */
#define NoUnderline 1
#define Underline 2
#define Overline 3
#define CrossOut 4

/* values for rule PRThickness */
#define ThinUnderline 1
#define ThickUnderline 2

/* values for rule PRAdjust */
#define AdjustLeft 1
#define AdjustRight 2
#define Centered 3
#define Justify 4
#define LeftWithDots 5

/* values for rule PRDirection */
#define LeftToRight 1
#define RightToLeft 2

/* values for rule PRUnicodeBidi */
#define Normal 1
#define Embed 2
#define Override 3

/* values for rule PRLineStyle */
#define SolidLine 1
#define DashedLine 2
#define DottedLine 3

/* values for rule PRDisplay */
#define Undefined 1
#define DisplayNone 2
#define Inline 3
#define Block 4
#define ListItem 5
#define RunIn 6
#define InlineBlock 7

/* values for rule PRListStyleType */
#define Disc 1
#define Circle 2
#define Square 3
#define Decimal 4
#define DecimalLeadingZero 5
#define LowerRoman 6
#define UpperRoman 7
#define LowerGreek 8
#define UpperGreek 9
#define LowerLatin 10
#define UpperLatin 11
#define ListStyleTypeNone 12

/* values for rule PRListStylePosition */
#define Inside 1
#define Outside 2

/* values for rule PRHyphenate */
#define Hyphenation 1
#define NoHyphenation 2

/* values for rule PRFillRule */
#define NonZero 1
#define EvenOdd 2

/* values for PRVis (CSS visibility) */
#define VsInherit 0
#define VsVisible 1
#define VsCollapse 2
#define VsHidden 3

#define PageAuto			0
#define PageAlways			1
#define PageAvoid			2
#define PageLeft			3
#define PageRight			4
#define PageInherit			5

/* keep this in sync with $THOTDIR/config/thot.pattern */
#define PATTERN_NONE		0
#define PATTERN_BACKGROUND	2

#ifndef __CEXTRACT__


/*----------------------------------------------------------------------
  TtaSetFontZoom
  Updates the font zoom global varialbe
  ----------------------------------------------------------------------*/
extern void TtaSetFontZoom (int zoom);

/*----------------------------------------------------------------------
   TtaNewPRule

   Creates a new presentation rule of a given type for a given view of a given
   document.

   Parameter:
   presentationType: type of the presentation rule to be created. Available
   values are PRSize, PRStyle, PRWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRLineStyle, PRDirection,
   PRUnicodeBidi, PRLineWeight, PRFillPattern, PRBackground, PRForeground,
   PRColor, PRHyphenate, PRWidth, PRHeight, PRVertPos, PRHorizPos.
   view: the view (this view must be open).
   document: the document.
   Return value:
   new presentation rule.
  ----------------------------------------------------------------------*/
extern PRule TtaNewPRule (int presentationType, View view, Document document);

/*----------------------------------------------------------------------
   TtaNewPRuleForView

   Creates a new presentation rule of a given type for a given view of a given
   document. The view is identified by its number.
   Parameter:
   presentationType: type of the presentation rule to be created. Available
   values are PRSize, PRStyle, PRWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRLineStyle, PRDirection,
   PRUnicodeBidi, PRLineWeight, PRFillPattern, PRBackground, PRForeground,
   PRColor, PRHyphenate, PRShowBox, PRNotInLine.
   viewName: the name of the view (this view does not need to be open).
   document: the document.
   Return value:
   new presentation rule.
  ----------------------------------------------------------------------*/
extern PRule TtaNewPRuleForView (int presentationType, int view, Document document);

/*----------------------------------------------------------------------
   TtaNewPRuleForNamedView

   Creates a new presentation rule of a given type for a given view of a given
   document. The view is identified by its name.
   Parameter:
   presentationType: type of the presentation rule to be created. Available
   values are PRSize, PRStyle, PRWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRLineStyle, PRDirection,
   PrUnicodeBidi, PRLineWeight, PRFillPattern, PRBackground, PRForeground,
   PRColor, PRHyphenate, PRShowBox, PRNotInLine.
   viewName: the name of the view (this view does not need to be open).
   document: the document.
   Return value:
   new presentation rule.
  ----------------------------------------------------------------------*/
extern PRule TtaNewPRuleForNamedView (int presentationType, char *viewName,
				      Document document);

/*----------------------------------------------------------------------
   TtaCopyPRule

   Creates a new presentation rule and initializes it with a copy of an existing
   presentation rule.
   Parameter:
   pRule: presentation rule to be copied.
   Return value:
   new presentation rule.
  ----------------------------------------------------------------------*/
extern PRule TtaCopyPRule (PRule pRule);

/*----------------------------------------------------------------------
   TtaAttachPRule

   Attaches a presentation rule to an element.
   Parameters:
   element: the element to which the presentation rule
   has to be attached.
   pRule: the presentation rule to be attached.
   document: the document to which the element belongs.
  ----------------------------------------------------------------------*/
extern void TtaAttachPRule (Element element, PRule pRule, Document document);

/*----------------------------------------------------------------------
   TtaIsCSSPRule

   Check whether a presentation rule is associated with a CSS rule
   Parameters:
   pRule: the presentation rule to be tested.
   Return value:
   TRUE if pRule is a CSS rule
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsCSSPRule (PRule pRule);

/*----------------------------------------------------------------------
   TtaRemovePRule

   Removes a presentation rule from an element and release that rule.
   Parameters:
   element: the element with which the presentation rule is associated.
   pRule: the presentation rule to be removed.
   document: the document to which the element belongs.
  ----------------------------------------------------------------------*/
extern void TtaRemovePRule (Element element, PRule pRule, Document document);

/*----------------------------------------------------------------------
   TtaSetPRuleValue

   Changes the value of a presentation rule. The presentation rule must
   be attached to an element that is part of a document.
   Parameters:
   element: the element to which the presentation rule is attached.
   pRule: the presentation rule to be changed.
   value: the value to be set.
   document: the document to which the element belongs.
   Valid values according to rule type:
   PRDepth: an integer (z-index of the element).
   PRVisibility: an integer >= 0 (visibility level).
   PRFillPattern: rank of the pattern in file thot.pattern.
   PRBackground: rank of the background color in file thot.color.
   PRForeground: rank of the foreground color in file thot.color.
   PRColor: rank of the color in file thot.color.
   PRBorderTopColor, PRBorderRightColor, PRBorderBottomColor,PRBorderLeftColor:
      if value >= 0: rank of the color in file thot.color.
      if value < 0 : -2 means transparent, -1 means same color as foreground,
                     -3 means undefined
   PRFont: FontTimes, FontHelvetica, FontCourier.
   PRStyle: StyleRoman, StyleItalics, StyleOblique.
   PRWeight: WeightNormal, WeightBold.
   PRUnderline: NoUnderline, Underline, Overline, CrossOut.
   PRThickness: ThinUnderline, ThickUnderline.
   PRDirection: LeftToRight, RightToLeft.
   PRUnicodeBidi: Normal, Embed, Override.
   PRLineStyle, PRBorderTopStyle, PRBorderRightStyle, PRBorderBottomStyle,
   PRBorderLeftStyle: SolidLine, DashedLine, DottedLine.
   PRNoBreak1, PRNoBreak2: a positive integer (distance in points).
   PRIndent: a positive, null or negative integer (indentation in points).
   PRSize: an integer between 6 and 72 (body size in points).
   PRLineSpacing: a positive integer (line spacing in points).
   PRLineWeight: a positive or null integer (stroke width for graphics in
      points).
   PRMarginTop, PRMarginLeft, PRMarginBottom, PRMarginRight: a positive, null
      or negative integer (margin in points).
   PRPaddingTop, PRPaddingRight, PRPaddingBottom, PRPaddingLeft: an integer
      > 0 (padding in points).
   PRBorderTopWidth, PRBorderRightWidth, PRBorderBottomWidth, PRBorderLeftWidth:
      an integer > 0 (border width in points).
   PRHyphenate: Hyphenation, NoHyphenation.
   PRAdjust: AdjustLeft, AdjustRight, Centered, LeftWithDots, Justify.
  ----------------------------------------------------------------------*/
extern void TtaSetPRuleValue (Element element, PRule pRule, int value,
			      Document document);

/*----------------------------------------------------------------------
   TtaSetPRuleValueWithUnit

   Changes the value and the length unit of a presentation rule.
   The presentation rule must be attached to an element that is part of
   a document.
   Parameters:
   element: the element to which the presentation rule is attached.
   pRule: the presentation rule to be changed.
   value: the value to be set.
   unit: the length unit in which the value is expressed.
   document: the document to which the element belongs.
   Valid values according to rule type:
   PRNoBreak1, PRNoBreak2: a positive integer (distance).
   PRIndent: a positive, null or negative integer (indentation).
   PRSize: an integer between 6 and 72 (body size).
   PRLineSpacing: a positive integer (line spacing).
   PRLineWeight: a positive or null integer (stroke width for graphics).
   PRMarginTop, PRMarginLeft, PRMarginBottom, PRMarginRight: a positive, null
      or negative integer (margin).
   PRPaddingTop, PRPaddingRight, PRPaddingBottom, PRPaddingLeft: an integer
      > 0 (padding).
   PRBorderTopWidth, PRBorderRightWidth, PRBorderBottomWidth, PRBorderLeftWidth:
      an integer > 0 (border width).
  ----------------------------------------------------------------------*/
extern void TtaSetPRuleValueWithUnit (Element element, PRule pRule, int value,
				      TypeUnit unit, Document document);

/*----------------------------------------------------------------------
   TtaSetPositionPRuleDelta

   Changes the delta value and the unit of a position presentation rule.
   The presentation rule must be attached to an element that is part of
   a document.
   Parameters:
   element: the element to which the presentation rule is attached.
   pRule: the presentation rule to be changed. It must be a position rule.
   delta: the value to be set.
   unit: the unit in which the delta value is expressed.
   document: the document to which the element belongs.
  ----------------------------------------------------------------------*/
extern void TtaSetPositionPRuleDelta (Element element, PRule pRule, int delta,
				      TypeUnit unit, Document document);

/*----------------------------------------------------------------------
   TtaSetPRuleView

   Sets the view to which a presentation rule applies. The presentation rule
   must not be attached yet to an element.
   Parameters:
   pRule: the presentation rule to be changed.
   view: the value to be set.
  ----------------------------------------------------------------------*/
extern void TtaSetPRuleView (PRule pRule, int view);

/*----------------------------------------------------------------------
  TtaGetPixelValue converts a logical value into a pixel value for
  a given element.
  ----------------------------------------------------------------------*/
extern int TtaGetPixelValue (int val, int unit, Element element, Document document);

/*----------------------------------------------------------------------
  TtaGetLogicalValue converts a pixel value into a logical value for
  a given element.
  ----------------------------------------------------------------------*/
extern int TtaGetLogicalValue (int val, int unit, Element element, Document document);

/*----------------------------------------------------------------------
   TtaChangeBoxSize

   Changes the height and width of the box corresponding to an element in
   a given view.
   Parameters:
   element: the element of interest.
   document: the document to which the element belongs.
   view: the view.
   deltaX: width increment in units (positive, negative or zero).
   deltaY: height increment in units (positive, negative or zero).
   unit: the unit used for the values.
  ----------------------------------------------------------------------*/
extern void TtaChangeBoxSize (Element element, Document document, View view,
                              int deltaX, int deltaY, TypeUnit unit);

/*----------------------------------------------------------------------
   TtaChangeBoxPosition

   Moves the box of an element in a given view.
   Parameters:
   element: the element of interest.
   document: the document to which the element belongs.
   view: the view.
   deltaX: horizontal shift in units (positive, negative or zero).
   deltaY: vertical shift in units (positive, negative or zero).
   unit: the unit used for the values.
  ----------------------------------------------------------------------*/
extern void TtaChangeBoxPosition (Element element, Document document,
                                  View view, int X, int Y, TypeUnit unit);

/*----------------------------------------------------------------------
   TtaGetDepth
   Returns the depth of the box corresponding to an element in a given view.

   Parameters:
   element: the element of interest.
   document: the document of interest.
   view: the view.
  ----------------------------------------------------------------------*/
extern int TtaGetDepth (Element element, Document document, View view);


/*----------------------------------------------------------------------
  TtaGiveBoxColors
  Returns the color and background color of the box corresponding to an
  element in a given view.
  Parameters:
  element: the element of interest.
  document: the document of interest.
  view: the view.
  Return parameters:
  color the thot
  bg_color
  ----------------------------------------------------------------------*/
extern void TtaGiveBoxColors (Element element, Document document, View view,
                              int *color, int *bg_color);


/*----------------------------------------------------------------------
  TtaGiveBoxFontInfo
  Returns the font description of the box corresponding to an
  element in a given view.
  Parameters:
  element: the element of interest.
  document: the document of interest.
  view: the view.
  Return parameters:
  size the font size value
  unit the unit of the font size
  family the font family
  ----------------------------------------------------------------------*/
extern void TtaGiveBoxFontInfo (Element element, Document document, View view,
                                int *size, TypeUnit *unit, int *family);

/*----------------------------------------------------------------------
   TtaGiveBoxSize

   Returns the height and width of the box corresponding to an element in
   a given view.
   Parameters:
   element: the element of interest.
   view: the view.
   unit: the unit used for the values.
   Return parameters:
   width: box width in units.
   height: box height in units.
  ----------------------------------------------------------------------*/
extern void TtaGiveBoxSize (Element element, Document document, View view,
                            TypeUnit unit, /*OUT*/ int *width, /*OUT*/ int *height);

/*----------------------------------------------------------------------
   TtaGiveBoxPosition

   Returns the x and y coordinates of the box corresponding to an element in
   a given view. The returned coordinates indicate the distance in points
   between the upper left corner of the box and the upper left corner of its
   parent box.
   Parameters:
   element: the element of interest.
   view: the view.
   unit: the unit used for the values.
   Return parameters:
   xCoord: distance from the left edge of the parent box to the left
   edge of the box, in units.
   yCoord:  distance from the upper edge of the parent box to the upper
   edge of the box, in units.
   inner is TRUE when the inner parent origin is considered else the outside
   of the parent origin is considered.
  ----------------------------------------------------------------------*/
extern void TtaGiveBoxPosition (Element element, Document document, View view,
                                TypeUnit unit, ThotBool inner,
                                /*OUT*/ int *xCoord, /*OUT*/ int *yCoord);

/*----------------------------------------------------------------------
   TtaGiveBoxAbsPosition
 
   Returns the x and y coordinates of the box corresponding to an element in
   a given view. The returned coordinates indicate the distance
   between the upper left corner of the box and the upper left corner of its
   window.
   Parameters:
   element: the element of interest.
   document: the document of interest.
   view: the view.
   unit: the unit used for the values.
   Return parameters:
   xCoord: distance from the left edge of the window to the left
   edge of the box.
   yCoord:  distance from the upper edge of the window to the upper
   edge of the box.
  ----------------------------------------------------------------------*/
extern void TtaGiveBoxAbsPosition (Element element, Document document, View view,
                                   TypeUnit unit, int *xCoord, int *yCoord);

/*----------------------------------------------------------------------
   TtaGiveWindowSize
 
   Returns the height and width of the window corresponding to a given view.
   Parameters:
   document: the document of interest.
   view: the view.
   unit: the unit used for the values (UnPixel or UnPoint only)
   Return parameters:
   width: window width in units.
   height: window height in units.
  ----------------------------------------------------------------------*/
extern void TtaGiveWindowSize (Document document, View view, TypeUnit unit,
                               int *width, int *height);

/*----------------------------------------------------------------------
 *   TtaGiveRGB returns the RGB of the color.
 ----------------------------------------------------------------------*/
extern char *TtaGiveRGB (char *colname, /*OUT*/ unsigned short *red,
                         /*OUT*/ unsigned short *green, /*OUT*/ unsigned short *blue );

/*----------------------------------------------------------------------
   TtaGiveThotRGB returns the Red Green and Blue values corresponding
   to color number num.
   If the color doesn't exist the function returns the values
   for the default color.
  ----------------------------------------------------------------------*/
extern void TtaGiveThotRGB (int num, /*OUT*/ unsigned short *red,
                            /*OUT*/ unsigned short *green, /*OUT*/ unsigned short *blue);

/*----------------------------------------------------------------------
   TtaNextPRule

   Returns the first presentation rule associated with a given
   element (if pRule is NULL) or the presentation rule that
   follows a given rule of a given element.
   Parameters:
   element: the element of interest.
   pRule: a presentation rule of that element, or NULL
   if the first rule is asked.
   Return parameter:
   pRule: the next presentation rule, or NULL if
   pRule is the last rule of the element.
  ----------------------------------------------------------------------*/
extern void TtaNextPRule (Element element, /*INOUT*/ PRule * pRule);

/*----------------------------------------------------------------------
   TtaGetPRule

   Returns a presentation rule of a given type associated with a given element.
   Parameters:
   element: the element of interest.
   presentationType: type of the desired presentation rule. Available
   values are PRSize, PRStyle, PRWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRLineStyle, PRDirection,
   PRUnicodeBidi, PRLineWeight, PRFillPattern, PRBackground, PRForeground,
   PRColor, PRHyphenate, PRShowBox, PRNotInLine.
   Return value:
   the presentation rule found, or NULL if the element
   does not have this type of presentation rule.
  ----------------------------------------------------------------------*/
extern PRule TtaGetPRule (Element element, int presentationType);

/*----------------------------------------------------------------------
   TtaGetPRuleType

   Returns the type of a presentation rule.
   Parameters:
   pRule: the presentation rule of interest.
   Return value:
   type of that presentation rule. Available values are RSize, RStyle,
   RFont, RUnderline, RThickness, PRIndent, RLineSpacing, RDepth, RAdjust,
   RLineStyle, RDirection, RUnicodeBidi, RLineWeight, RFillPattern,
   RBackground, RForeground, RColor, RHyphenate, PRShowBox, PRNotInLine.
  ----------------------------------------------------------------------*/
extern int TtaGetPRuleType (PRule pRule);

/*----------------------------------------------------------------------
   TtaGetPRuleValue

   Returns the value of a presentation rule.
   Parameters:
   pRule: the presentation rule of interest.
   Return values according to rule type:
   PRVisibility: an integer >= 0 (visibility level).
   PRDepth: an integer (z-index of the element).
   PRFillPattern: rank of the pattern in file thot.pattern.
   PRBackground: rank of the background color in file thot.color.
   PRForeground: rank of the foreground color in file thot.color.
   PRColor: rank of the color in file thot.color.
   PRBorderTopColor, PRBorderRightColor, PRBorderBottomColor,PRBorderLeftColor:
      if value >= 0: rank of the color in file thot.color.
      if value < 0 : -2 means transparent, -1 means same color as foreground,
                     -3 means undefined
   PRFont: FontTimes, FontHelvetica, FontCourier.
   PRStyle: StyleRoman, StyleItalics, StyleOblique.
   PRWeight: WeightNormal, WeightBold.
   PRUnderline: NoUnderline, Underline, Overline, CrossOut.
   PRThickness: ThinUnderline, ThickUnderline.
   PRDirection: LeftToRight, RightToLeft.
   PRUnicodeBidi: Normal, Embed, Override.
   PRLineStyle, PRBorderTopStyle, PRBorderRightStyle, PRBorderBottomStyle,
      PRBorderLeftStyle: SolidLine, DashedLine, DottedLine.
   PRNoBreak1, PRNoBreak2: a positive integer (distance in points).
   PRIndent: a positive, null or negative integer (indentation in points).
   PRSize: an integer between 6 and 72 (body size in points).
   PRLineSpacing: a positive integer (line spacing in points).
   PRLineWeight: a positive or null integer (stroke width for graphics).
   PRMarginTop, PRMarginLeft, PRMarginBottom, PRMarginRight: a positive, null
      or negative integer (margin in points).
   PRPaddingTop, PRPaddingRight, PRPaddingBottom, PRPaddingLeft: an integer
      > 0 (padding in points).
   PRBorderTopWidth, PRBorderRightWidth, PRBorderBottomWidth, PRBorderLeftWidth:
      an integer > 0 (border width in points).
   PRHyphenate: Hyphenation, NoHyphenation.
   PRAdjust: AdjustLeft, AdjustRight, Centered, LeftWithDots, Justify.
  ----------------------------------------------------------------------*/
extern int TtaGetPRuleValue (PRule pRule);

/*----------------------------------------------------------------------
   TtaGetPositionPRuleDelta

   Returns the delta value of a position rule.

   Parameters:
   pRule: the presentation rule of interest.

   Return:
   delta value.
  ----------------------------------------------------------------------*/
extern int TtaGetPositionPRuleDelta (PRule pRule);

/*----------------------------------------------------------------------
   TtaGetPRuleUnit

   Returns the unit of a presentation rule.
   Parameters:
   pRule: the presentation rule of interest.
   Return the unit of the rule type PRSize, PRIndent, PRLineSpacing, PRLineWeight,
   PRWidth, PRHeight, PRVertPos, PRHorizPos.
   This unit could be UnRelative, UnXHeight, UnPoint, UnPixel, UnPercent.
   Return UnRelative in other cases.
  ----------------------------------------------------------------------*/
extern int TtaGetPRuleUnit (PRule pRule);

/*----------------------------------------------------------------------
   TtaGetPRuleView

   Returns the number of the view to which a presentation rule applies.
   Parameters:
       pRule: the presentation rule of interest.
   Return value:
       number of the view to which the presentation rule applies.
  ----------------------------------------------------------------------*/
extern int TtaGetPRuleView (PRule pRule);

/*----------------------------------------------------------------------
   TtaSamePRules

   Compares two presentation rules associated with the same element or with
   different elements.
   Parameters:
   pRule1: first presentation rule to be compared.
   pRule2: second presentation rule to be compared.
   Return value:
   0 if both rules are different, 1 if they are identical.
  ----------------------------------------------------------------------*/
extern int TtaSamePRules (PRule pRule1, PRule pRule2);

/*----------------------------------------------------------------------
  TtaUpdateTableWidths
  Force the rebuild of a table after presentation changes
  ----------------------------------------------------------------------*/
extern void TtaUpdateTableWidths (Element table, Document doc);

#endif /* __CEXTRACT__ */

#endif
