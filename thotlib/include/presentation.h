#ifndef _PRESENTATION_H_
#define _PRESENTATION_H_
#include "view.h"
/* presentation rule */
typedef int        *PRule;

/* Types of presentation rule */

#define PRVisibility 0
#define PRFunction 1
#define PRVertRef 2
#define PRHorizRef 3
#define PRHeight 4
#define PRWidth 5
#define PRVertPos 6
#define PRHorizPos 7
#define PRSize 8
#define PRStyle 9
#define PRFont 10
#define PRUnderline 11
#define PRThickness 12
#define PRIndent 13
#define PRLineSpacing 14
#define PRDepth 15
#define PRAdjust 16
#define PRJustify 17
#define PRLineStyle 18
#define PRLineWeight 19
#define PRFillPattern 20
#define PRBackground 21
#define PRForeground 22

#define PRHyphenate 23
#define PRNoBreak1 24
#define PRNoBreak2 25

/* values for rule PRStyle */
#define StyleRoman 1
#define StyleBold 2
#define StyleItalics 3
#define StyleOblique 4
#define StyleBoldItalics 5
#define StyleBoldOblique 6

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

extern PRule        TtaNewPRule (int presentationType, View view, Document document);
extern PRule        TtaNewPRuleForNamedView (int presentationType, char *viewName, Document document);
extern PRule        TtaCopyPRule (PRule pRule);
extern void         TtaAttachPRule (Element element, PRule pRule, Document document);
extern void         TtaRemovePRule (Element element, PRule pRule, Document document);
extern void         TtaSetPRuleValue (Element element, PRule pRule, int value, Document document);
extern void         TtaChangeBoxSize (Element element, Document document, View view, int deltaX, int deltaY, TypeUnit unit);
extern void         TtaChangeBoxPosition (Element element, Document document, View view, int X, int Y, TypeUnit unit);
extern void         TtaGiveBoxSize (Element element, Document document, View view, TypeUnit unit, int *width, int *height);
extern void         TtaGiveBoxPosition (Element element, Document document, View view, TypeUnit unit, int *xCoord, int *yCoord);
extern void         TtaNextPRule (Element element, PRule * pRule);
extern PRule        TtaGetPRule (Element element, int presentationType);
extern int          TtaGetPRuleType (PRule pRule);
extern int          TtaGetPRuleValue (PRule pRule);
extern int          TtaSamePRules (PRule pRule1, PRule pRule2);

#else  /* __STDC__ */

extern PRule        TtaNewPRule ( /* int presentationType, View view, Document document */ );
extern PRule        TtaNewPRuleForNamedView ( /* int presentationType, char *viewName, Document document */ );
extern PRule        TtaCopyPRule ( /* PRule pRule */ );
extern void         TtaAttachPRule ( /* Element element, PRule pRule, Document document */ );
extern void         TtaRemovePRule ( /* Element element, PRule pRule, Document document */ );
extern void         TtaSetPRuleValue ( /* Element element, PRule pRule, int value, Document document */ );
extern void         TtaChangeBoxSize ( /* Element element, View view, Document document, int deltaX, int deltaY, TypeUnit unit */ );
extern void         TtaChangeBoxPosition ( /* Element element, Document document, View view, int X, int Y, TypeUnit unit */ );
extern void         TtaGiveBoxSize ( /* Element element, Document document, View view, TypeUnit unit, int *width, int *height */ );
extern void         TtaGiveBoxPosition ( /* Element element, Document document, View view, TypeUnit unit, int *xCoord, int *yCoord */ );
extern void         TtaNextPRule ( /* Element element, PRule *pRule */ );
extern PRule        TtaGetPRule ( /* Element element, int presentationType */ );
extern int          TtaGetPRuleType ( /* PRule pRule */ );
extern int          TtaGetPRuleValue ( /* PRule pRule */ );
extern int          TtaSamePRules ( /* PRule pRule1, PRule pRule2 */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
