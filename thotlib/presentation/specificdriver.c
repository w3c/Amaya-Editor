/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * specificdriver.c : presentation driver used to manipulate presentation *
 *           using presentation specific attributes.			  *
 *                                                                        *
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
 *
 * Author: D. Veillard (INRIA)
 *
 */

#include "thot_sys.h"
#include "message.h"
#include "constint.h"
#include "typeint.h"
#include "constmedia.h"
#include "typemedia.h"
#include "pschema.h"
#include "application.h"

#define THOT_EXPORT extern
#include "edit_tv.h"

#include "specificdriver.h"

#include "presentationapi_f.h"
#include "changepresent_f.h"
#include "memory_f.h"
#include "presentdriver_f.h"

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
   ctxt->destroy = 0;
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
   TtaFreeMemory ( ctxt);
}

/*
 * GetDocumentMainPSchema : returns the main PSchema of a document
 */

#ifdef __STDC__
static PSchema      GetDocumentMainPSchema (Document doc)
#else  /* __STDC__ */
static PSchema      GetDocumentMainPSchema (doc)
Document            doc;

#endif /* __STDC__ */
{
    return((PSchema)LoadedDocument[doc - 1]->DocSSchema->SsPSchema);
}

/*----------------------------------------------------------------------
   *									*
   *	Function used to remove all specific presentation for a given	*
   *	element.							*
   *									*
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 SpecificCleanPresentation (PresentationTarget t, PresentationContext c,
					       PresentationValue v)
#else
int                 SpecificCleanPresentation (t, c, v)
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

/*----------------------------------------------------------------------
   *									*
   *	Function used to update the rendering for a given element.	*
   *									*
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 SpecificUpdatePresentation (PresentationTarget t, PresentationContext c,
					       PresentationValue v)
#else
int                 SpecificUpdatePresentation (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;

#endif
{
   SpecificTarget  elem = (SpecificTarget) t;
   SpecificContext ctxt = (SpecificContext) c;
   ElementType type;
   Document doc;
   PtrSSchema pSS;
   int elType = 0;
   PtrPRule pRule;

   if (ctxt == NULL)
     return(-1);

   doc = ctxt->doc;
   pSS = (PtrSSchema) TtaGetDocumentSSchema (doc);

   /*
    * The PRule list is directly associated to the element.
    */
   type = TtaGetElementType(elem);
   elType = type.ElTypeNum;
   pRule = ((PtrElement) elem)->ElFirstPRule;

   if (pRule == NULL)
      return (-1);

   ApplyPRules (doc, pSS, elType, 0, 0, pRule, ctxt->destroy);
   return(0);
}

/*----------------------------------------------------------------------
   Function used to to add a specific presentation rule
   for a given type of rule associated to an element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule  InsertElementPRule (SpecificTarget el, PRuleType type, int extra)
#else
static PtrPRule  InsertElementPRule (el, type, extra)
SpecificTarget      el;
PRuleType           type;
int                 extra;

#endif
{
    PtrPRule cur, prev, pRule;
    
    prev = NULL;
    cur = ((PtrElement) el)->ElFirstPRule;

    while (cur != NULL) {
	/* shortcut : rules are sorted by type and view number */
	if ((cur->PrType > type) ||
	    ((cur->PrType == type) && (cur->PrViewNum > 1)) ||
	    (((cur->PrType == type) && (type == PRFunction) &&
	     (cur->PrPresFunction > extra))))
	  {
	     cur = NULL;
	     break;
	  }
	
	/* check for extra specification in case of function rule */
	if ((type == PRFunction) && (cur->PrPresFunction != extra)) {
	    prev = cur;
	    cur = cur->PrNextPRule;
	    continue;
	}

	/* check this rule */
	if (type == cur->PrType)
	   break;

	/* jump to next and keep track of previous */
	prev = cur;
	cur = cur->PrNextPRule;
    }
    if (cur != NULL)
	return (cur);

    /* not found, allocate it, fill it and insert it */
    GetPresentRule (&pRule);
    if (pRule == NULL)
     {
	TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
	return (NULL);
     }
    pRule->PrType = type;
    pRule->PrCond = NULL;
    pRule->PrViewNum = 1;
    pRule->PrSpecifAttr = 0;
    pRule->PrSpecifAttrSSchema = NULL;

    /* Add the order / conditions .... */
    /* chain in the rule */
    if (prev == NULL)
     {
	pRule->PrNextPRule = ((PtrElement) el)->ElFirstPRule;
	((PtrElement) el)->ElFirstPRule = pRule;
     }
    else
     {
	pRule->PrNextPRule = prev->PrNextPRule;
	prev->PrNextPRule = pRule;
     }
    return (pRule);
}

/*----------------------------------------------------------------------
   Function used to to remove a specific presentation rule
   for a given type of rule associated to an element.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void  RemoveElementPRule (SpecificTarget el, PRuleType type, int extra)
#else
static void  RemoveElementPRule (el, type, extra)
SpecificTarget      el;
PRuleType           type;
int                 extra;

#endif
{
    PtrPRule cur, prev;
    Document doc;
    PtrSSchema pSS;
    ElementType elType;
    
    prev = NULL;
    cur = ((PtrElement) el)->ElFirstPRule;

    while (cur != NULL) {
	/* shortcut : rules are sorted by type and view number */
	if ((cur->PrType > type) ||
	    ((cur->PrType == type) && (cur->PrViewNum > 1)) ||
	    (((cur->PrType == type) && (type == PRFunction) &&
	     (cur->PrPresFunction > extra))))
	  {
	     cur = NULL;
	     break;
	  }
	
	/* check for extra specification in case of function rule */
	if ((type == PRFunction) && (cur->PrPresFunction != extra)) {
	    prev = cur;
	    cur = cur->PrNextPRule;
	    continue;
	}

	/* check this rule */
	if (type == cur->PrType)
	   break;

	/* jump to next and keep track of previous */
	prev = cur;
	cur = cur->PrNextPRule;
    }
    if (cur == NULL)
	return;

    /* remove the rule from the chain */
    if (prev == NULL)
	((PtrElement) el)->ElFirstPRule = cur->PrNextPRule;
    else
	prev->PrNextPRule = cur->PrNextPRule;
    cur->PrNextPRule = NULL;

    /* update the presentation */
    doc = TtaGetDocument(el);
    pSS = (PtrSSchema) TtaGetDocumentSSchema (doc);
    elType = TtaGetElementType(el);
    ApplyPRules (doc, pSS, elType.ElTypeNum, 0, 0, cur, TRUE);

    /* Free the PRule */
    FreePresentRule(cur);

    return;
}

/*----------------------------------------------------------------------
   Function used to to search all specific presentation rules
   for a given type of rule associated to an element.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrPRule SearchElementPRule (SpecificTarget el, int type, int extra)
#else
static PtrPRule SearchElementPRule (el, type, extra)
SpecificTarget      el;
int                 type;
int                 extra;

#endif
{
    PtrPRule cur;
    
    cur = ((PtrElement) el)->ElFirstPRule;

    while (cur != NULL) {
	/* shortcut : rules are sorted by type and view number */
	if ((cur->PrType > type) ||
	    ((cur->PrType == type) && (cur->PrViewNum > 1)) ||
	    ((cur->PrType == type) && (type == PRFunction) &&
	     (cur->PrPresFunction > extra)))
	  {
	     cur = NULL;
	     break;
	  }
	
	/* check for extra specification in case of function rule */
	if ((type == PRFunction) && (cur->PrPresFunction != extra))
	  {
	    cur = cur->PrNextPRule;
	    continue;
	  }

	/* check this rule */
	if (type == cur->PrType)
	   break;

	/* jump to next and keep track of previous */
	cur = cur->PrNextPRule;
    }
    return (cur);
}

/*----------------------------------------------------------------------
   *									*
   *	Function used to translate various parameters between external  *
   *	and internal representation of presentation attributes.		*
   *      These function also handle setting or fetching these values     *
   *      from the internal memory representation of presentation rules.  *
   *									*
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          etoi_convert (SpecificTarget el, int type,
   PresentationValue val, PRule pRule, Document doc,int specific)
#else
static int          etoi_convert (el, type, val, pRule, doc, specific)
SpecificTarget      el;
int                 type;
PresentationValue   val;
PRule               pRule;
Document            doc;
int                 specific;

#endif
{
   PtrPRule            rule = (PtrPRule) pRule;

   PresentationValueToPRule (val, rule->PrType, pRule, specific);
   /*** RedisplayNewPRule (doc, el, pRule); ***/
   return (0);
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

/*----------------------------------------------------------------------
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
  ----------------------------------------------------------------------*/

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)

#define SPECIFIC_FUNCS(type,name)					\
									\
int SpecificSet##name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue v)				\
{									\
    PtrPRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue val = /* (SpecificValue) - EGP */ v;			\
									\
    if (cont->destroy) {						\
        RemoveElementPRule(el, PR##type, 0);				\
	return(0);							\
    }									\
    prule = InsertElementPRule(el, PR##type, 0);			\
    if (prule == NULL) return(-1);					\
    etoi_convert(el, PR##type, val, (PRule)prule, cont->doc, 0);	\
    return(0);								\
}									\
									\
int SpecificGet##name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue *v)				\
{									\
    PtrPRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue *val = /* (SpecificValue *) - EGP */ v;		\
									\
    prule = SearchElementPRule(el, PR##type, 0);			\
    if (prule == NULL) return(-1);					\
    itoe_convert(el, PR##type, val, (PRule)prule, cont->doc);		\
    return(0);								\
}									\

#define SPECIFIC_FUNCS2(type,category,name)				\
									\
int SpecificSet##name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue v)				\
{									\
    PtrPRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue val = /* (SpecificValue) - EGP */ v;			\
									\
    if (cont->destroy) {						\
        RemoveElementPRule(el, PR##type, category);			\
	return(0);							\
    }									\
    prule = InsertElementPRule(el, PR##type, category);			\
    if (prule == NULL) return(-1);					\
    etoi_convert(el, PR##type, val, (PRule)prule, cont->doc, category);	\
    return(0);								\
}									\
									\
int SpecificGet##name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue *v)				\
{									\
    PtrPRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue *val = /* (SpecificValue *) - EGP */ v;		\
									\
    prule = SearchElementPRule(el,PR##type,category);			\
    if (prule == NULL) return(-1);					\
    itoe_convert(el,PR##type,val,(PRule)prule,cont->doc);		\
    return(0);								\
}									\

#else  /* ! __STDC__ i.e. token-pasting is made the old way ! */

#define SPECIFIC_FUNCS(type,name)					\
									\
int SpecificSet/**/name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue v)				\
{									\
    PtrPRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue val = /* (SpecificValue) - EGP */ v;			\
									\
    if (cont->destroy) {						\
        RemoveElementPRule(el, PR/**/type, 0);				\
	return(0);							\
    }									\
    prule = InsertElementPRule(el, PR/**/type, 0);			\
    if (prule == NULL) return(-1);					\
    etoi_convert(el, PR/**/type, val, (PRule)prule, cont->doc, 0);	\
    return(0);								\
}									\
									\
int SpecificGet/**/name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue *v)				\
{									\
    PtrPRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue *val = /* (SpecificValue *) - EGP */ v;		\
									\
    prule = SearchElementPRule(el, PR/**/type, 0);			\
    if (prule == NULL) return(-1);					\
    itoe_convert(el, PR/**/type, val, (PRule)prule, cont->doc);		\
    return(0);								\
}									\

#define SPECIFIC_FUNCS2(type,category,name)				\
									\
int SpecificSet/**/name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue v)				\
{									\
    PtrPRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue val = /* (SpecificValue) - EGP */ v;			\
									\
    if (cont->destroy) {						\
        RemoveElementPRule(el, PR/**/type, category);			\
	return(0);							\
    }									\
    prule = InsertElementPRule(el, PR/**/type, category);		\
    if (prule == NULL) return(-1);					\
    etoi_convert(el, PR/**/type, val,(PRule)prule, cont->doc, category);\
    return(0);								\
}									\
									\
int SpecificGet/**/name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue *v)				\
{									\
    PRule prule;							\
    SpecificTarget el = (SpecificTarget) t;				\
    SpecificContext cont = (SpecificContext) c;				\
    SpecificValue *val = /* (SpecificValue *) - EGP */ v;		\
									\
    prule = SearchElementPRule(el, PR/**/type, category);		\
    if (prule == NULL) return(-1);					\
    itoe_convert(el, PR/**/type, val, (PRule)prule, cont->doc, 0);	\
    return(0);								\
}									\

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   *									*
   *	generation of most common presentations routines		*
   *									*
  ----------------------------------------------------------------------*/

SPECIFIC_FUNCS (Foreground, ForegroundColor)
SPECIFIC_FUNCS (Background, BackgroundColor)
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
SPECIFIC_FUNCS (Height, Height)
SPECIFIC_FUNCS (Width, Width)
SPECIFIC_FUNCS (VertPos, VPos)
SPECIFIC_FUNCS (HorizPos, HPos)
SPECIFIC_FUNCS2 (Function, FnLine, InLine)
SPECIFIC_FUNCS2 (Function, FnShowBox, ShowBox)
SPECIFIC_FUNCS2 (Function, FnPictureMode, PictureMode)

/*----------------------------------------------------------------------
   *									*
   *	a few presentations routines still need to be hand-coded	*
   *									*
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 SpecificSetBgImage (PresentationTarget t, PresentationContext c,
				   PresentationValue v)
#else
int                 SpecificSetBgImage (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;

#endif
{
   SpecificTarget       el = (SpecificTarget) t;
   SpecificContext      ctxt = (SpecificContext) c;
   PtrPRule            rule;
   int                 cst;
   PSchema             tsch = GetDocumentMainPSchema (ctxt->doc);

   if (ctxt->destroy) {
       RemoveElementPRule (el, PtFunction, FnBackgroundPicture);
       return(0);
   }
   cst = PresConstInsert (tsch, v.pointer);
   rule = InsertElementPRule (el, PtFunction, FnBackgroundPicture);
   if (rule == NULL)
      return (-1);
   v.typed_data.unit = DRIVERP_UNIT_REL;
   v.typed_data.value = cst;
   etoi_convert (el, PRFunction, v, (PRule)rule , ctxt->doc, FnBackgroundPicture);
   return (0);
}

#ifdef __STDC__
int                 SpecificGetBgImage (PresentationTarget t, PresentationContext c,
				   PresentationValue * v)
#else
int                 SpecificGetBgImage (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue  *v;

#endif
{
   SpecificTarget      el = (SpecificTarget) t;
   PtrPSchema          pSchemaPrs;
   SpecificContext     ctxt = (SpecificContext) c;
   PtrPRule            rule;
   int                 cst;
   PresentationValue   val;
   PSchema             tsch = GetDocumentMainPSchema (ctxt->doc);

   rule = SearchElementPRule (el, PtFunction, FnBackgroundPicture);
   if (rule == NULL)
      return (-1);
   pSchemaPrs = (PtrPSchema) tsch;
   val = PRuleToPresentationValue ((PRule) rule);
   cst = val.typed_data.unit;
   v->pointer = &pSchemaPrs->PsConstant[cst-1].PdString[0];
   return (0);
}


/*----------------------------------------------------------------------
   *									*
   *	the strategy block for the specific presentation driver         *
   *									*
  ----------------------------------------------------------------------*/

PresentationStrategy SpecificStrategy =
{
   (PresentationSetFunction) SpecificCleanPresentation,
   (PresentationSetFunction) SpecificUpdatePresentation,

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

   (PresentationGetFunction) SpecificGetVPos,
   (PresentationSetFunction) SpecificSetVPos,

   (PresentationGetFunction) SpecificGetHPos,
   (PresentationSetFunction) SpecificSetHPos,

   (PresentationGetFunction) SpecificGetHeight,
   (PresentationSetFunction) SpecificSetHeight,

   NULL, /* (PresentationGetFunction) SpecificGetRelHeight, */
   NULL, /* (PresentationSetFunction) SpecificSetRelHeight, */

   (PresentationGetFunction) SpecificGetWidth,
   (PresentationSetFunction) SpecificSetWidth,

   NULL, /* (PresentationGetFunction) SpecificGetRelWidth, */
   NULL, /* (PresentationSetFunction) SpecificSetRelWidth, */

   (PresentationGetFunction) SpecificGetInLine,
   (PresentationSetFunction) SpecificSetInLine,

   NULL, /* (PresentationGetFunction) SpecificGetShow, */
   NULL, /* (PresentationSetFunction) SpecificSetShow, */

   NULL, /* (PresentationGetFunction) SpecificGetBox, */
   NULL, /* (PresentationSetFunction) SpecificSetBox, */

   (PresentationGetFunction) SpecificGetShowBox,
   (PresentationSetFunction) SpecificSetShowBox,

   (PresentationGetFunction) SpecificGetBgImage,
   (PresentationSetFunction) SpecificSetBgImage,

   (PresentationGetFunction) SpecificGetPictureMode,
   (PresentationSetFunction) SpecificSetPictureMode,
};

/*
 * ApplyAllSpecificContext : browse the PRules list the corresponding
 *      SpecificContext structure, and call the given handler for each one.
 */

#ifdef __STDC__
void                ApplyAllSpecificContext (Document doc, SpecificTarget target,
			    SpecificContextApplyHandler handler, void *param)
#else  /* __STDC__ */
void                ApplyAllSpecificContext (doc, target, handler, param)
Document            doc;
SpecificTarget       target;
SpecificSettingsApplyHandler handler;
void               *param;

#endif /* __STDC__ */
{
     SpecificContextBlock ctxt;
     
     /*
      * There is only one specific context applied to a document.
      */
     ctxt.drv = &SpecificStrategy;
     ctxt.doc = doc;
     ctxt.schema = TtaGetDocumentSSchema (doc);
     ctxt.magic1 = 0;
     ctxt.magic2 = 0;

     handler (target, &ctxt, param);
}

/*
 * ApplyAllSpecificSettings : browse all the PRules structures,
 *      associated to the corresponding SpecificContext 
 *      structure, and call the given handler for each one.
 */

#ifdef __STDC__
void                ApplyAllSpecificSettings (SpecificTarget target,
		   SpecificContext ctxt, SpecificSettingsApplyHandler handler,
					     void *param)
#else  /* __STDC__ */
void                ApplyAllSpecificSettings (target, ctxt, handler, param)
SpecificTarget       target;
SpecificContext      ctxt;
SpecificSettingsApplyHandler handler;
void               *param;

#endif /* __STDC__ */
{
  Element                  el = (Element) target;
  PtrPRule                 rule;
  PresentationSettingBlock setting;
  PtrPSchema               pSc1;
  int                      cst;
    
  if (target == NULL)
    return;
  rule = ((PtrElement) el)->ElFirstPRule;
  /*
   * for each rule corresponding to the same context i.e. identical
   * conditions, create the corresponding PresentationSetting and
   * call the user handler.
   */
  while (rule != NULL)
    {
      /* fill in the PresentationSetting and call the handler */
      if (rule->PrPresMode == PresFunction)
	PRuleToPresentationSetting ((PRule) rule, &setting, rule->PrPresFunction);
      else
	PRuleToPresentationSetting ((PRule) rule, &setting, 0);

      /* need to do some tweaking in the case of BackgroudPicture */
      if (setting.type == DRIVERP_BGIMAGE)
	{
	  cst = setting.value.typed_data.value;
	  pSc1 = (PtrPSchema) GetDocumentMainPSchema (ctxt->doc);
	  setting.value.pointer = &pSc1->PsConstant[cst-1].PdString[0];
	}

	handler (target, ctxt, &setting, param);
	rule = rule->PrNextPRule;
     }
}

