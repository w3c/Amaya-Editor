
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern PRule TtaNewPRule ( int presentationType, View view, Document document );
extern PRule TtaNewPRuleForNamedView ( int presentationType, char *viewName, Document document );
extern PRule TtaCopyPRule ( PRule pRule );
extern void TtaAttachPRule ( Element element, PRule pRule, Document document );
extern void TtaRemovePRule ( Element element, PRule pRule, Document document );
extern void TtaSetPRuleValue ( Element element, PRule pRule, int value, Document document );
extern void TtaChangeBoxSize ( Element element, View view, int deltaX, int deltaY, Document document );
extern void TtaChangeBoxPosition ( Element element, View view, int deltaX, int deltaY, Document document );
extern void TtaGiveBoxSize ( Element element, View view, int *width, int *height );
extern void TtaGiveBoxPosition ( Element element, View view, int *xCoord, int *yCoord );
extern void TtaNextPRule ( Element element, PRule *pRule );
extern PRule TtaGetPRule ( Element element, int presentationType );
extern int TtaGetPRuleType ( PRule pRule );
extern int TtaGetPRuleValue ( PRule pRule );
extern int TtaSamePRules ( PRule pRule1, PRule pRule2 );

#else /* __STDC__ */

extern PRule TtaNewPRule (/* int presentationType, View view, Document document */);
extern PRule TtaNewPRuleForNamedView (/* int presentationType, char *viewName, Document document */);
extern PRule TtaCopyPRule (/* PRule pRule */);
extern void TtaAttachPRule (/* Element element, PRule pRule, Document document */);
extern void TtaRemovePRule (/* Element element, PRule pRule, Document document */);
extern void TtaSetPRuleValue (/* Element element, PRule pRule, int value, Document document */);
extern void TtaChangeBoxSize (/* Element element, View view, int deltaX, int deltaY, Document document */);
extern void TtaChangeBoxPosition (/* Element element, View view, int deltaX, int deltaY, Document document */);
extern void TtaGiveBoxSize (/* Element element, View view, int *width, int *height */);
extern void TtaGiveBoxPosition (/* Element element, View view, int *xCoord, int *yCoord */);
extern void TtaNextPRule (/* Element element, PRule *pRule */);
extern PRule TtaGetPRule (/* Element element, int presentationType */);
extern int TtaGetPRuleType (/* PRule pRule */);
extern int TtaGetPRuleValue (/* PRule pRule */);
extern int TtaSamePRules (/* PRule pRule1, PRule pRule2 */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
