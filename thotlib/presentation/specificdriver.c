/**************************************************************************
 * specificdriver.c : presentation driver used to manipulate presentation *
 *           using presentation specific attributes.			  *
 **************************************************************************
 *  This kind of routines are used only when changing presentation        *
 *  attributes of A SINGLE ELEMENT. Each element correspond to a node     *
 *  in the tree composing a Thot document and these routine puts special  *
 *  markers "presentation attributes" on the node correponding to the     *
 *  the element whose representation has to be modified. The Thot runtime *
 *  interprets these markers and changes the graphical output on the main *
 *  view of the document accordingly.					  *
 *  The spirit of these kind of modifications are completely opposed to   *
 *  the usual way of representing objects which consists of following the *
 *  presentation scheme associated to this kind of document and described *
 * in the P language.                                                     *
 **************************************************************************
 */

#include "thot_sys.h"
#include "message.h"
#include "conststr.h"
#include "typestr.h"
#include "constprs.h"
#include "typeprs.h"
#include "application.h"

#include "specificdriver.h"

/*
 * GetSpecificContext : user level function needed to allocate and
 *        initialize a SpecificContext.
 */

#ifdef __STDC__
SpecificContext     GetSpecificContext (Document doc)
#else  /* __STDC__ */
SpecificContext     GetSpecificContext (doc)
Document            doc;

#endif /* __STDC__ */
{
   SpecificContext     ctxt;

   ctxt = (SpecificContext) TtaGetMemory (sizeof (SpecificContextBlock));
   if (ctxt == NULL)
      return (NULL);
   ctxt->drv = &SpecificStrategy;
   ctxt->doc = doc;
   ctxt->schema = TtaGetDocumentSSchema (doc);
   return (ctxt);
}

/*
 * FreeSpecificContext : user level function needed to deallocate
 *        a SpecificContext.
 */

#ifdef __STDC__
void                FreeSpecificContext (SpecificContext ctxt)
#else  /* __STDC__ */
void                FreeSpecificContext (ctxt)
SpecificContext     ctxt;

#endif /* __STDC__ */
{
   if (ctxt == NULL)
      return;
   if (ctxt->drv != &SpecificStrategy)
      return;
   TtaFreeMemory ((char *) ctxt);
}

/************************************************************************
 *									*
 *	Function used to remove all specific presentation for a given	*
 *	element.							*
 *									*
 ************************************************************************/

#ifdef __STDC__
int                 SpecificCleanPresentation (PresentationTarget t, PresentationContext c,
					       PresentationValue v)
#else
int                 GenericSetWidth (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;

#endif
{
   PRule               rule;
   Element             elem = (Element) t;

   /*
    * remove all the presentation specific rules applied to the element.
    */
   do
     {
	rule = NULL;
	TtaNextPRule (elem, &rule);
	if (rule)
	   TtaRemovePRule (elem, rule, c->doc);
     }
   while (rule != NULL);
   return (0);
}

/************************************************************************
 *									*
 *	Function used to translate various parameters between external  *
 *	and internal representation of presentation attributes.		*
 *      These function also handle setting or fetching these values     *
 *      from the internal memory representation of presentation rules.  *
 *									*
 ************************************************************************/

#ifdef __STDC__
static int          etoi_convert (SpecificTarget el, int type,
			   PresentationValue val, PRule pRule, Document doc)
#else
static int          etoi_convert (el, type, val, pRule, doc)
SpecificTarget      el;
int                 type;
PresentationValue   val;
PRule               pRule;
Document            doc;

#endif
{
   PtrPRule        rule = (PtrPRule) pRule;

   PresentationValueToPRule (val, rule->PrType, pRule, 0);
}

/* itoe_convert : the dual function, interternal to external              */

#ifdef __STDC__
static int          itoe_convert (SpecificTarget el, int type,
			 PresentationValue * val, PRule pRule, Document doc)
#else
static int          itoe_convert (el, type, val, pRule, doc)
SpecificTarget      el;
int                 type;
PresentationValue  *val;
PRule               pRule;
Document            doc;

#endif
{
   *val = PRuleToPresentationValue (pRule);
   return (0);
}

/************************************************************************
 *									*
 *	Macro's used to generate presentations routines			*
 *      These heavily rely on the token-pasting mechanism provided by   *
 *      the C preprocessor. The string a##b is replaced by the string   *
 *      "ab", but this is done after the macro is expanded.             *
 *      This mecanism allows to avoid a lot of typing, errors and keep  *
 *      the code compact at the price of a loss of readability.         *
 *      On old fashionned preprocessor (pre-Ansi) the token pasting was *
 *      a side effect of the preprocessor implementation on empty       *
 *      comments. In this case we use a+slash+star+star+slash+b to      *
 *      produce the same string "ab".					*
 *									*
 ************************************************************************/

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)

#define SPECIFIC_FUNCS(type,name)					\
									\
int SpecificSet##name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue v)				\
{									\
    PRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue val = /* (SpecificValue) - EGP */ v;			\
									\
    prule = TtaGetPRule(el,PR##type);					\
    if (prule == NULL) {						\
        prule = TtaNewPRuleForView(PR##type,1,				\
                                       cont->doc);			\
        TtaAttachPRule(el,prule,cont->doc);				\
    }									\
    etoi_convert(el,PR##type,val,prule,cont->doc);			\
    return(0);								\
}									\
									\
int SpecificGet##name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue *v)				\
{									\
    PRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue *val = /* (SpecificValue *) - EGP */ v;		\
									\
    prule = TtaGetPRule(el,PR##type);					\
    if (prule == NULL) return(-1);					\
    itoe_convert(el,PR##type,val,prule,cont->doc);			\
    return(0);								\
}									\

#else  /* ! __STDC__ i.e. token-pasting is made the old way ! */

#define SPECIFIC_FUNCS(type,name)					\
									\
int SpecificSet/**/name(t,c,v)						\
    PresentationTarget t;						\
    PresentationContext c;						\
    PresentationValue v;						\
{									\
    PRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue val = /* (SpecificValue) - EGP */ v;			\
									\
    prule = TtaGetPRule(el,PR/**/type);					\
    if (prule == NULL) {						\
        prule = TtaNewPRuleForView(PR/**/type,1,	\
                                       cont->doc);			\
        TtaAttachPRule(el,prule,cont->doc);				\
    }									\
    etoi_convert(el,PR/**/type,val,prule,cont->doc);			\
    return(0);								\
}									\
									\
int SpecificGet/**/name(t,c,v)						\
    PresentationTarget t;						\
    PresentationContext c;						\
    PresentationValue *v;						\
{									\
    PRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue *val = /* (SpecificValue *) - EGP */ v;		\
									\
    prule = TtaGetPRule(el,PR/**/type);					\
    if (prule == NULL) return(-1);					\
    itoe_convert(el,PR/**/type,val,prule,cont->doc);			\
    return(0);								\
}									\

#endif /* __STDC__ */

/************************************************************************
 *									*
 *	generation of most common presentations routines		*
 *									*
 ************************************************************************/

SPECIFIC_FUNCS (Foreground, ForegroundColor) \
SPECIFIC_FUNCS (Background, BackgroundColor) \
SPECIFIC_FUNCS (Size, FontSize)
SPECIFIC_FUNCS (Style, FontStyle)
SPECIFIC_FUNCS (Adjust, Alignment)
SPECIFIC_FUNCS (Indent, Indent)
SPECIFIC_FUNCS (Justify, Justification)
SPECIFIC_FUNCS (Hyphenate, Hyphenation)
SPECIFIC_FUNCS (Underline, TextUnderlining)
SPECIFIC_FUNCS (FillPattern, FillPattern)
SPECIFIC_FUNCS (Font, FontFamily)
SPECIFIC_FUNCS (LineSpacing, LineSpacing)

/************************************************************************
 *									*
 *	a few presentations routines still need to be hand-coded	*
 *									*
 ************************************************************************/

#ifdef __STDC__
int                 SpecificSetWidth (PresentationTarget t, PresentationContext c,
				      PresentationValue v)
#else
int                 SpecificSetWidth (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;

#endif
{
   SpecificTarget      el = (SpecificTarget) t;
   SpecificContext     cont = (SpecificContext) c;
   int                 x, y;
   int                 delta = v.value;

   TtaGiveBoxSize (el, cont->doc, 1, UnPoint, &x, &y);
   delta -= x;
   TtaChangeBoxSize (el, cont->doc, 1, delta, 0, UnPoint);

   return (0);
}

#ifdef __STDC__
int                 SpecificGetWidth (PresentationTarget t, PresentationContext c,
				      PresentationValue * v)
#else
int                 SpecificGetWidth (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue  *v;

#endif
{
   PRule               prule;
   SpecificTarget      el = (SpecificTarget) t;
   SpecificContext     cont = (SpecificContext) c;
   SpecificValue      *val = (SpecificValue *) v;
   int                 width = -1;
   int                 type;

   prule = TtaGetPRule (el, PRWidth);
   prule = NULL;
   do
     {
	TtaNextPRule (el, &prule);
	if (prule == NULL)
	   break;
	type = TtaGetPRuleType (prule);
	if (type == PRWidth)
	  {
	     width = TtaGetPRuleValue (prule);
	     break;
	  }
     }
   while (prule != NULL);
   if (width < 0)
      return (-1);
   itoe_convert (el, PRWidth, val, prule, cont->doc);
   return (0);
}

/************************************************************************
 *									*
 *	the strategy block for the specific presentation driver         *
 *									*
 ************************************************************************/

PresentationStrategy SpecificStrategy =
{
   (PresentationSetFunction) SpecificCleanPresentation,

   (PresentationGetFunction) SpecificGetForegroundColor,
   (PresentationSetFunction) SpecificSetForegroundColor,

   (PresentationGetFunction) SpecificGetBackgroundColor,
   (PresentationSetFunction) SpecificSetBackgroundColor,

   (PresentationGetFunction) SpecificGetFontSize,
   (PresentationSetFunction) SpecificSetFontSize,

   (PresentationGetFunction) SpecificGetFontStyle,
   (PresentationSetFunction) SpecificSetFontStyle,

   (PresentationGetFunction) SpecificGetFontFamily,
   (PresentationSetFunction) SpecificSetFontFamily,

   (PresentationGetFunction) SpecificGetTextUnderlining,
   (PresentationSetFunction) SpecificSetTextUnderlining,

   (PresentationGetFunction) SpecificGetAlignment,
   (PresentationSetFunction) SpecificSetAlignment,

   (PresentationGetFunction) SpecificGetLineSpacing,
   (PresentationSetFunction) SpecificSetLineSpacing,

   (PresentationGetFunction) SpecificGetIndent,
   (PresentationSetFunction) SpecificSetIndent,

   (PresentationGetFunction) SpecificGetJustification,
   (PresentationSetFunction) SpecificSetJustification,

   (PresentationGetFunction) SpecificGetHyphenation,
   (PresentationSetFunction) SpecificSetHyphenation,

   (PresentationGetFunction) SpecificGetFillPattern,
   (PresentationSetFunction) SpecificSetFillPattern,

/*************************************************************
    (PresentationGetFunction) SpecificGetVPos,
    (PresentationSetFunction) SpecificSetVPos,

    (PresentationGetFunction) SpecificGetHPos,
    (PresentationSetFunction) SpecificSetHPos,

    (PresentationGetFunction) SpecificGetHeight,
    (PresentationSetFunction) SpecificSetHeight,

    (PresentationGetFunction) SpecificGetRelHeight,
    (PresentationSetFunction) SpecificSetRelHeight,
 *************************************************************/
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

/*************************************************************
    (PresentationGetFunction) SpecificGetWidth,
    (PresentationSetFunction) SpecificSetWidth,
 *************************************************************/
   NULL, NULL,

/*************************************************************
    (PresentationGetFunction) SpecificGetRelWidth,
    (PresentationSetFunction) SpecificSetRelWidth,

    (PresentationGetFunction) SpecificGetInLine,
    (PresentationSetFunction) SpecificSetInLine,

    (PresentationGetFunction) SpecificGetShow,
    (PresentationSetFunction) SpecificSetShow,

    (PresentationGetFunction) SpecificGetBox,
    (PresentationSetFunction) SpecificSetBox,
 *************************************************************/
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};
