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
 
#ifndef _PRESENTATION_H_
#define _PRESENTATION_H_
#include "view.h"
/* presentation rule */
typedef int        *PRule;

/* Types of presentation rule */
/* These values must follow the order of PRuleType defined in typeprs.h */

#define PRVisibility 0
#define PRFunction 1
#define PRVertRef 2
#define PRHorizRef 3
#define PRHeight 4
#define PRWidth 5
#define PRVertPos 6
#define PRHorizPos 7
#define PRMarginTop 8
#define PRMarginRight 9
#define PRMarginBottom 10
#define PRMarginLeft 11
#define PRPaddingTop 12
#define PRPaddingRight 13
#define PRPaddingBottom 14
#define PRPaddingLeft 15
#define PRBorderTopWidth 16
#define PRBorderRightWidth 17
#define PRBorderBottomWidth 18
#define PRBorderLeftWidth 19
#define PRBorderTopColor 20
#define PRBorderRightColor 21
#define PRBorderBottomColor 22
#define PRBorderLeftColor 23
#define PRBorderTopStyle 24
#define PRBorderRightStyle 25
#define PRBorderBottomStyle 26
#define PRBorderLeftStyle 27
#define PRSize 28
#define PRStyle 29
#define PRWeight 30
#define PRFont 31
#define PRUnderline 32
#define PRThickness 33
#define PRIndent 34
#define PRLineSpacing 35
#define PRDepth 36
#define PRAdjust 37
#define PRJustify 38
#define PRLineStyle 39
#define PRLineWeight 40
#define PRFillPattern 41
#define PRBackground 42
#define PRForeground 43
#define PRHyphenate 44
#define PRVertOverflow 45
#define PRHorizOverflow 46
#define PRNoBreak1 47
#define PRNoBreak2 48
/* 49 reserved */
#define PRLine 50
#define PRCreateEnclosing 51
#define PRShowBox 52
#define PRBackgroundPicture 53
#define PRPictureMode 54
#define PRNotInLine 55
#define PRNone 56
#define LAST_PRES_RULE_TYPE 56

/* values for rule PRStyle */
#define StyleRoman 1
#define StyleItalics 2
#define StyleOblique 3

/* values for rule PRWeight */
#define WeightNormal 1
#define WeightBold 2

/* values for rule PRFont */
#define FontTimes 1
#define FontHelvetica 2
#define FontCourier 3

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
#define LeftWithDots 4

/* values for rule PRJustify */
#define Justified 1
#define NotJustified 2

/* values for rule PRLineStyle */
#define SolidLine 1
#define DashedLine 2
#define DottedLine 3


/* values for rule PRHyphenate */
#define Hyphenation 1
#define NoHyphenation 2

#ifndef __CEXTRACT__
#ifdef __STDC__


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
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle,
   PRLineWeight, PRFillPattern, PRBackground, PRForeground, PRHyphenate,
   PRWidth, PRHeight, PRVertPos, PRHorizPos.
   view: the view (this view must be open).
   document: the document.

   Return value:
   new presentation rule.

  ----------------------------------------------------------------------*/
extern PRule        TtaNewPRule (int presentationType, View view, Document document);

/*----------------------------------------------------------------------
   TtaNewPRuleForView

   Creates a new presentation rule of a given type for a given view of a given
   document. The view is identified by its number.

   Parameter:
   presentationType: type of the presentation rule to be created. Available
   values are PRSize, PRStyle, PRWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle,
   PRLineWeight, PRFillPattern, PRBackground, PRForeground, PRHyphenate,
   PRShowBox, PRNotInLine.
   viewName: the name of the view (this view does not need to be open).
   document: the document.

   Return value:
   new presentation rule.

  ----------------------------------------------------------------------*/
extern PRule        TtaNewPRuleForView (int presentationType, int view, Document document);

/*----------------------------------------------------------------------
   TtaNewPRuleForNamedView

   Creates a new presentation rule of a given type for a given view of a given
   document. The view is identified by its name.

   Parameter:
   presentationType: type of the presentation rule to be created. Available
   values are PRSize, PRStyle, PRWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle,
   PRLineWeight, PRFillPattern, PRBackground, PRForeground, PRHyphenate,
   PRShowBox, PRNotInLine.
   viewName: the name of the view (this view does not need to be open).
   document: the document.

   Return value:
   new presentation rule.

  ----------------------------------------------------------------------*/
extern PRule        TtaNewPRuleForNamedView (int presentationType, STRING viewName, Document document);

/*----------------------------------------------------------------------
   TtaCopyPRule

   Creates a new presentation rule and initializes it with a copy of an existing
   presentation rule.

   Parameter:
   pRule: presentation rule to be copied.

   Return value:
   new presentation rule.

  ----------------------------------------------------------------------*/
extern PRule        TtaCopyPRule (PRule pRule);

/*----------------------------------------------------------------------
   TtaAttachPRule

   Attaches a presentation rule to an element.

   Parameters:
   element: the element to which the presentation rule
   has to be attached.
   pRule: the presentation rule to be attached.
   document: the document to which the element belongs.

  ----------------------------------------------------------------------*/
extern void         TtaAttachPRule (Element element, PRule pRule, Document document);

/*----------------------------------------------------------------------
   TtaRemovePRule

   Removes a presentation rule from an element and release that rule.

   Parameters:
   element: the element with which the presentation rule is associated.
   pRule: the presentation rule to be removed.
   document: the document to which the element belongs.

  ----------------------------------------------------------------------*/
extern void         TtaRemovePRule (Element element, PRule pRule, Document document);

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
   PRBorderTopColor, PRBorderRightColor, PRBorderBottomColor,PRBorderLeftColor:
      if value >= 0: rank of the color in file thot.color.
      if value < 0 : -2 means transparent and -1 means same color as foreground
   PRFont: FontTimes, FontHelvetica, FontCourier.
   PRStyle: StyleRoman, StyleItalics, StyleOblique.
   PRWeight: WeightNormal, WeightBold.
   PRUnderline: NoUnderline, Underline, Overline, CrossOut.
   PRThickness: ThinUnderline, ThickUnderline.
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
   PRJustify: Justified, NotJustified.
   PRHyphenate: Hyphenation, NoHyphenation.
   PRAdjust: AdjustLeft, AdjustRight, Centered, LeftWithDots.

  ----------------------------------------------------------------------*/
extern void         TtaSetPRuleValue (Element element, PRule pRule, int value, Document document);

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
extern void         TtaSetPRuleValueWithUnit (Element element, PRule pRule, int value, TypeUnit unit, Document document);

/*----------------------------------------------------------------------
   TtaSetPRuleView

   Sets the view to which a presentation rule applies. The presentation rule
   must not be attached yet to an element.

   Parameters:
   pRule: the presentation rule to be changed.
   view: the value to be set.

  ----------------------------------------------------------------------*/
extern void         TtaSetPRuleView (PRule pRule, int view);

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
extern void         TtaChangeBoxSize (Element element, Document document, View view, int deltaX, int deltaY, TypeUnit unit);

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
extern void         TtaChangeBoxPosition (Element element, Document document, View view, int X, int Y, TypeUnit unit);

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
extern void         TtaGiveBoxSize (Element element, Document document, View view, TypeUnit unit, /*OUT*/ int *width, /*OUT*/ int *height);

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

  ----------------------------------------------------------------------*/
extern void         TtaGiveBoxPosition (Element element, Document document, View view, TypeUnit unit, /*OUT*/ int *xCoord, /*OUT*/ int *yCoord);

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
extern void         TtaGiveBoxAbsPosition (Element element, Document document, View view, TypeUnit unit, int *xCoord, int *yCoord);

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
extern void        TtaGiveWindowSize (Document document, View view, TypeUnit unit, int *width, int *height);

/*----------------------------------------------------------------------
 *   TtaGiveRGB returns the RGB of the color.
 ----------------------------------------------------------------------*/
extern CHAR_T      *TtaGiveRGB ( CHAR_T* colname, /*OUT*/ unsigned short *red, /*OUT*/ unsigned short *green, /*OUT*/ unsigned short *blue );

/*----------------------------------------------------------------------
   TtaGiveThotRGB returns the Red Green and Blue values corresponding
   to color number num.
   If the color doesn't exist the function returns the values
   for the default color.
  ----------------------------------------------------------------------*/
extern void         TtaGiveThotRGB (int num, /*OUT*/ unsigned short *red, /*OUT*/ unsigned short *green, /*OUT*/ unsigned short *blue);

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
extern void         TtaNextPRule (Element element, /*INOUT*/ PRule * pRule);

/*----------------------------------------------------------------------
   TtaGetPRule

   Returns a presentation rule of a given type associated with a given element.
 
   Parameters:
   element: the element of interest.
   presentationType: type of the desired presentation rule. Available
   values are PRSize, PtStyle, PtWeight, PRFont, PRUnderline, PRThickness,
   PRIndent, PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle,
   PRLineWeight, PRFillPattern, PRBackground, PRForeground, PRHyphenate,
   PRShowBox, PRNotInLine.
 
   Return value:
   the presentation rule found, or NULL if the element
   does not have this type of presentation rule.
 
  ----------------------------------------------------------------------*/
extern PRule        TtaGetPRule (Element element, int presentationType);

/*----------------------------------------------------------------------
   TtaGetPRuleType

   Returns the type of a presentation rule.

   Parameters:
   pRule: the presentation rule of interest.

   Return value:
   type of that presentation rule. Available values are RSize, PtStyle,
   RFont, RUnderline, RThickness, PRIndent, RLineSpacing, RDepth, RAdjust,
   RJustify, RLineStyle, RLineWeight, RFillPattern, RBackground,
   RForeground, RHyphenate, PRShowBox, PRNotInLine.

  ----------------------------------------------------------------------*/
extern int          TtaGetPRuleType (PRule pRule);

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
   PRBorderTopColor, PRBorderRightColor, PRBorderBottomColor,PRBorderLeftColor:
      if value >= 0: rank of the color in file thot.color.
      if value < 0 : -2 means transparent and -1 means same color as foreground
   PRFont: FontTimes, FontHelvetica, FontCourier.
   PRStyle: StyleRoman, StyleItalics, StyleOblique.
   PRWeight: WeightNormal, WeightBold.
   PRUnderline: NoUnderline, Underline, Overline, CrossOut.
   PRThickness: ThinUnderline, ThickUnderline.
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
   PRJustify: Justified, NotJustified.
   PRHyphenate: Hyphenation, NoHyphenation.
   PRAdjust: AdjustLeft, AdjustRight, Centered, LeftWithDots.

  ----------------------------------------------------------------------*/
extern int          TtaGetPRuleValue (PRule pRule);

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
extern int         TtaGetPRuleUnit (PRule pRule);

/*----------------------------------------------------------------------
   TtaGetPRuleView

   Returns the number of the view to which a presentation rule applies.

   Parameters:
       pRule: the presentation rule of interest.

   Return value:
       number of the view to which the presentation rule applies.

  ----------------------------------------------------------------------*/
extern int          TtaGetPRuleView (PRule pRule);

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
extern int          TtaSamePRules (PRule pRule1, PRule pRule2);

#else  /* __STDC__ */

extern void         TtaSetFontZoom (/* int zoom */);
extern PRule        TtaNewPRule ( /* int presentationType, View view, Document document */ );
extern PRule        TtaNewPRuleForView (/* int presentationType, int view, Document document */);
extern PRule        TtaNewPRuleForNamedView ( /* int presentationType, STRING viewName, Document document */ );
extern PRule        TtaCopyPRule ( /* PRule pRule */ );
extern void         TtaAttachPRule ( /* Element element, PRule pRule, Document document */ );
extern void         TtaRemovePRule ( /* Element element, PRule pRule, Document document */ );
extern void         TtaSetPRuleValue ( /* Element element, PRule pRule, int value, Document document */ );
extern void         TtaSetPRuleValueWithUnit ( /* Element element, PRule pRule, int value, TypeUnit unit, Document document */ );
extern void         TtaSetPRuleView ( /* PRule pRule, int view */ );
extern void         TtaChangeBoxSize ( /* Element element, View view, Document document, int deltaX, int deltaY, TypeUnit unit */ );
extern void         TtaChangeBoxPosition ( /* Element element, Document document, View view, int X, int Y, TypeUnit unit */ );
extern void         TtaGiveBoxSize ( /* Element element, Document document, View view, TypeUnit unit, int *width, int *height */ );
extern void         TtaGiveBoxPosition ( /* Element element, Document document, View view, TypeUnit unit, int *xCoord, int *yCoord */ );
extern void         TtaGiveBoxAbsPosition ( /* Element element, Document document, View view, TypeUnit unit, int *xCoord, int *yCoord */ );
extern void         TtaGiveWindowSize ( /* Document document, View view, TypeUnit unit, int *width, int *height */ );
extern CHAR_T      *TtaGiveRGB ( /* CHAR_T* colname, unsigned short *red, unsigned short *green, unsigned short *blue*/ );
extern void         TtaGiveThotRGB (/*int num, unsigned short *red, unsigned short *green, unsigned short *blue*/);
extern void         TtaNextPRule ( /* Element element, PRule *pRule */ );
extern PRule        TtaGetPRule ( /* Element element, int presentationType */ );
extern int          TtaGetPRuleType ( /* PRule pRule */ );
extern int          TtaGetPRuleValue ( /* PRule pRule */ );
extern int          TtaGetPRuleUnit ( /* PRule pRule */);
extern int          TtaGetPRuleView ( /* PRule pRule */ );
extern int          TtaSamePRules ( /* PRule pRule1, PRule pRule2 */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
