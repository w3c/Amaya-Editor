/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: V. Quint (INRIA)
 *
 */
 
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "presentation.h"
#include "tree.h"
#include "typecorr.h"

#undef THOT_EXPORT
#define THOT_EXPORT
#include "edit_tv.h"

#include "appli_f.h"
#include "compilmsg_f.h"
#include "font_f.h"
#include "memory_f.h"
#include "changepresent_f.h"
#include "boxpositions_f.h"
#include "viewapi_f.h"

extern int          UserErrorCode;


/*----------------------------------------------------------------------
   TtaNewPRule

   Creates a new presentation rule of a given type for a given view of a given
   document.

   Parameter:
   presentationType: type of the presentation rule to be created. Available
   values are PRSize, PRStyle, PRFont, PRUnderline, PRThickness, PRIndent,
   PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle, PRLineWeight,
   PRFillPattern, PRBackground, PRForeground, PRHyphenate.
   view: the view (this view must be open).
   document: the document.

   Return value:
   new presentation rule.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRule               TtaNewPRule (int presentationType, View view, Document document)
#else  /* __STDC__ */
PRule               TtaNewPRule (presentationType, view, document)
int                 presentationType;
View                view;
Document            document;
#endif /* __STDC__ */
{
   PtrPRule            pPres;
   int                 v;

   UserErrorCode = 0;
   pPres = NULL;
   if (presentationType < 0 || presentationType > PRHyphenate)
     TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   else
     /* parameter document is correct */
     {
       v = 0;
       if (view < 100)
	 /* View of the main tree */
	 if (view < 1 || view > MAX_VIEW_DOC)
	   TtaError (ERR_invalid_parameter);
	 else if (LoadedDocument[document - 1]->DocView[view - 1].DvPSchemaView == 0)
	   /* this view is not open */
	   TtaError (ERR_invalid_parameter);
	 else
	   v = LoadedDocument[document - 1]->DocView[view - 1].DvPSchemaView;
       else
	 /* View of associated elements */
	 if (view - 100 < 1 || view - 100 > MAX_ASSOC_DOC)
	   TtaError (ERR_invalid_parameter);
	 else if (LoadedDocument[document - 1]->DocAssocFrame[view - 101] == 0)
	   /* this view is not open */
	   TtaError (ERR_invalid_parameter);
	 else
	   v = 1;
       if (v > 0)
	 {
	   GetPresentRule (&pPres);
	   pPres->PrType = (PRuleType) presentationType;
	   pPres->PrNextPRule = NULL;
	   pPres->PrViewNum = v;
	 }
     }
   return ((PRule) pPres);
}

/*----------------------------------------------------------------------
   TtaNewPRuleForView

   Creates a new presentation rule of a given type for a given view of a given
   document. The view is identified by its number.

   Parameter:
   presentationType: type of the presentation rule to be created. Available
   values are PRSize, PRStyle, PRFont, PRUnderline, PRThickness, PRIndent,
   PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle, PRLineWeight,
   PRFillPattern, PRBackground, PRForeground, PRHyphenate, PRShowBox,
   PRNotInLine.
   viewName: the name of the view (this view does not need to be open).
   document: the document.

   Return value:
   new presentation rule.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRule               TtaNewPRuleForView (int presentationType, int view, Document document)
#else  /* __STDC__ */
PRule               TtaNewPRuleForView (presentationType, view, document)
int                 presentationType;
int                 view;
Document            document;
#endif /* __STDC__ */
{
   PtrPRule            pPres;

   UserErrorCode = 0;
   pPres = NULL;
   if (presentationType != PRNotInLine &&
       presentationType != PRShowBox &&
       (presentationType < 0 || presentationType > PRHyphenate))
     TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   else if ((view < 1) || (view > MAX_VIEW))
     TtaError (ERR_invalid_document_parameter);
   else
     /* parameters document and view are correct */
     {
       GetPresentRule (&pPres);
       pPres->PrNextPRule = NULL;
       pPres->PrViewNum = view;
       if (presentationType == PRShowBox)
	 {
	   pPres->PrType = PtFunction;
	   pPres->PrPresFunction = FnShowBox;
	 }
       else if (presentationType == PRNotInLine)
	 {
	   pPres->PrType = PtFunction;
	   pPres->PrPresFunction = FnNotInLine;
	 }
       else
	 pPres->PrType = (PRuleType) presentationType;
     }
   return ((PRule) pPres);
}

/*----------------------------------------------------------------------
   TtaNewPRuleForNamedView

   Creates a new presentation rule of a given type for a given view of a given
   document. The view is identified by its name.

   Parameter:
   presentationType: type of the presentation rule to be created. Available
   values are PRSize, PRStyle, PRFont, PRUnderline, PRThickness, PRIndent,
   PRLineSpacing, PRDepth, PRAdjust, PRJustify, PRLineStyle, PRLineWeight,
   PRFillPattern, PRBackground, PRForeground, PRHyphenate, PRShowBox,
   PRNotInLine.
   viewName: the name of the view (this view does not need to be open).
   document: the document.

   Return value:
   new presentation rule.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRule               TtaNewPRuleForNamedView (int presentationType, char *viewName, Document document)
#else  /* __STDC__ */
PRule               TtaNewPRuleForNamedView (presentationType, viewName, document)
int                 presentationType;
char               *viewName;
Document            document;
#endif /* __STDC__ */
{
   PtrPRule            pPres;
   PtrDocument         pDoc;
   PtrElement          pEl;
   int                 vue;
   int                 v;

   UserErrorCode = 0;
   pPres = NULL;
   if (presentationType != PRShowBox &&
       presentationType != PRNotInLine &&
       (presentationType < 0 || presentationType > PRHyphenate))
     TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   else
     /* parameter document is correct */
     {
       pDoc = LoadedDocument[document - 1];
       vue = 0;
       /* Searching into the main tree views */
       if (pDoc->DocSSchema->SsPSchema != NULL)
	 for (v = 1; v <= MAX_VIEW && vue == 0; v++)
	   if (strcmp (pDoc->DocSSchema->SsPSchema->PsView[v - 1], viewName) == 0)
	     vue = v;
       /* If not found one search into associated elements */
       if (vue == 0)
	 for (v = 1; v <= MAX_ASSOC_DOC && vue == 0; v++)
	   {
	     pEl = pDoc->DocAssocRoot[v - 1];
	     if (pEl != NULL)
	       if (strcmp (viewName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName) == 0)
		 vue = 1;
	   }
       if (vue == 0)
	 TtaError (ERR_invalid_parameter);
       else
	 {
	   GetPresentRule (&pPres);
	   pPres->PrNextPRule = NULL;
	   pPres->PrViewNum = vue;
	   if (presentationType == PRShowBox)
	     {
	       pPres->PrType = PtFunction;
	       pPres->PrPresFunction = FnShowBox;
	     }
	   else	if (presentationType == PRNotInLine)
	     {
	       pPres->PrType = PtFunction;
	       pPres->PrPresFunction = FnNotInLine;
	     }
	   else
	     pPres->PrType = (PRuleType) presentationType;
	 }
     }
   return ((PRule) pPres);
}

/*----------------------------------------------------------------------
   TtaCopyPRule

   Creates a new presentation rule and initializes it with a copy of an existing
   presentation rule.

   Parameter:
   pRule: presentation rule to be copied.

   Return value:
   new presentation rule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRule               TtaCopyPRule (PRule pRule)
#else  /* __STDC__ */
PRule               TtaCopyPRule (pRule)
PRule               pRule;
#endif /* __STDC__ */
{
   PtrPRule            copy;

   UserErrorCode = 0;
   copy = NULL;
   if (pRule == NULL)
     TtaError (ERR_invalid_parameter);
   else
     {
       GetPresentRule (&copy);
       *copy = *((PtrPRule) pRule);
       copy->PrNextPRule = NULL;
     }
   return ((PRule) copy);
}

/*----------------------------------------------------------------------
   TtaAttachPRule

   Attaches a presentation rule to an element.

   Parameters:
   element: the element to which the presentation rule
   has to be attached.
   pRule: the presentation rule to be attached.
   document: the document to which the element belongs.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaAttachPRule (Element element, PRule pRule, Document document)
#else  /* __STDC__ */
void                TtaAttachPRule (element, pRule, document)
Element             element;
PRule               pRule;
Document            document;
#endif /* __STDC__ */
{
   PtrPRule            pPres;
   boolean             stop;
#ifndef NODISPLAY
   boolean             ok;

#endif
   UserErrorCode = 0;
   if (element == NULL || pRule == NULL)
     TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is correct */
     {
#ifndef NODISPLAY
	ok = TRUE;
#endif
	if (((PtrElement) element)->ElFirstPRule == NULL)
	  ((PtrElement) element)->ElFirstPRule = (PtrPRule) pRule;
	else
	  {
	    pPres = ((PtrElement) element)->ElFirstPRule;
	    stop = FALSE;
	    do
	      if (pPres->PrType == ((PtrPRule) pRule)->PrType &&
		  pPres->PrViewNum == ((PtrPRule) pRule)->PrViewNum)
		{
		  TtaError (ERR_duplicate_presentation_rule);
#ifndef NODISPLAY
		  ok = FALSE;
#endif
		  stop = TRUE;
		}
	      else if (pPres->PrNextPRule == NULL)
		{
		  pPres->PrNextPRule = (PtrPRule) pRule;
		  ((PtrPRule) pRule)->PrNextPRule = NULL;
		  stop = TRUE;
		}
	      else
		pPres = pPres->PrNextPRule;
	    while (!stop);
	  }
#ifndef NODISPLAY
	if (ok)
	  RedisplayNewPRule (document, (PtrElement) element, (PtrPRule) pRule);
#endif
     }
}

/*----------------------------------------------------------------------
   TtaRemovePRule

   Removes a presentation rule from an element and release that rule.

   Parameters:
   element: the element with which the presentation rule is associated.
   pRule: the presentation rule to be removed.
   document: the document to which the element belongs.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaRemovePRule (Element element, PRule pRule, Document document)
#else  /* __STDC__ */
void                TtaRemovePRule (element, pRule, document)
Element             element;
PRule               pRule;
Document            document;
#endif /* __STDC__ */
{
   PtrPRule            pPres, pPreviousPres;

   UserErrorCode = 0;
   if (element == NULL || pRule == NULL)
     TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   else
     /* parameter document is correct */
     {
       pPres = ((PtrElement) element)->ElFirstPRule;
       pPreviousPres = NULL;
       while (pPres != NULL && pPres != (PtrPRule) pRule)
	 {
	   pPreviousPres = pPres;
	   pPres = pPres->PrNextPRule;
	 }
       if (pPres != (PtrPRule) pRule)
	 /* This element does not own a presentation rule */
	 TtaError (ERR_invalid_parameter);
       else
	 {
	   if (pPreviousPres == NULL)
	     ((PtrElement) element)->ElFirstPRule = pPres->PrNextPRule;
	   else
	     pPreviousPres->PrNextPRule = pPres->PrNextPRule;
#ifndef NODISPLAY
	   RedisplayDefaultPresentation (document, (PtrElement) element, pPres->PrType,
					 pPres->PrPresFunction,pPres->PrViewNum);
#endif
	   FreePresentRule (pPres);
	 }
     }
}


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
   RSize: an integer between 6 and 72 (body size in points).
   PtStyle: StyleRoman, StyleBold, StyleItalics, StyleOblique,
   StyleBoldItalics, StyleBoldOblique.
   RFont: FontTimes, FontHelvetica, FontCourier.
   RUnderline: NoUnderline, Underline, Overline, CrossOut.
   RThickness: ThinUnderline, ThickUnderline.
   PtIndent: a positive, null or negative integer (indentation in points).
   RLineSpacing: a positive integer (line spacing in points).
   RDepth: a positive integer (depth of the element).
   RAdjust: AdjustLeft, AdjustRight, Centered, LeftWithDots.
   RJustify: Justified, NotJustified.
   RHyphenate: Hyphenation, NoHyphenation.
   RLineStyle: SolidLine, DashedLine, DottedLine.
   RLineWeight: a positive or null integer (stroke width for graphics).
   RFillPattern: rank of the pattern in the file thot.pattern.
   RBackground: rank of the background color in the file thot.color.
   RForeground: rank of the foreground color in the file thot.color.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetPRuleValue (Element element, PRule pRule, int value, Document document)
#else  /* __STDC__ */
void                TtaSetPRuleValue (element, pRule, value, document)
Element             element;
PRule               pRule;
int                 value;
Document            document;
#endif /* __STDC__ */
{
#ifndef NODISPLAY
   boolean             done;
#endif

   UserErrorCode = 0;
   if (element == NULL || pRule == NULL)
     TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
     /* verifies the parameter document */
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   else
     /* parameter document is correct */
     {
#ifndef NODISPLAY
       done = TRUE;
#endif
       switch (((PtrPRule) pRule)->PrType)
	 {
	 case PtSize:
	   /* Body-size in typographic points */
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   ((PtrPRule) pRule)->PrMinUnit = UnPoint;
	   ((PtrPRule) pRule)->PrMinAttr = FALSE;
	   ((PtrPRule) pRule)->PrMinValue = value;
	   break;
	 case PtStyle:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   switch (value)
	     {
	     case StyleRoman:
	       ((PtrPRule) pRule)->PrChrValue = 'R';
	       break;
	     case StyleBold:
	       ((PtrPRule) pRule)->PrChrValue = 'B';
	       break;
	     case StyleItalics:
	       ((PtrPRule) pRule)->PrChrValue = 'I';
	       break;
	     case StyleOblique:
	       ((PtrPRule) pRule)->PrChrValue = 'O';
	       break;
	     case StyleBoldItalics:
	       ((PtrPRule) pRule)->PrChrValue = 'G';
	       break;
	     case StyleBoldOblique:
	       ((PtrPRule) pRule)->PrChrValue = 'Q';
	       break;
	     default:
#ifndef NODISPLAY
	       done = FALSE;
#endif
	       TtaError (ERR_invalid_parameter);
	       break;
	     }
	   break;
	 case PtFont:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   switch (value)
	     {
	     case FontTimes:
	       ((PtrPRule) pRule)->PrChrValue = 'T';
	       break;
	     case FontHelvetica:
	       ((PtrPRule) pRule)->PrChrValue = 'H';
	       break;
	     case FontCourier:
	       ((PtrPRule) pRule)->PrChrValue = 'C';
	       break;
	     default:
#ifndef NODISPLAY
	       done = FALSE;
#endif
	       TtaError (ERR_invalid_parameter);
	       break;
	     }
	   break;
	 case PtUnderline:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   switch (value)
	     {
	     case NoUnderline:
	       ((PtrPRule) pRule)->PrChrValue = 'N';
	       break;
	     case Underline:
	       ((PtrPRule) pRule)->PrChrValue = 'U';
	       break;
	     case Overline:
	       ((PtrPRule) pRule)->PrChrValue = 'O';
	       break;
	     case CrossOut:
	       ((PtrPRule) pRule)->PrChrValue = 'C';
	       break;
	     default:
#ifndef NODISPLAY
	       done = FALSE;
#endif
	       TtaError (ERR_invalid_parameter);
	       break;
	     }
	   break;
	 case PtThickness:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   switch (value)
	     {
	     case ThinUnderline:
	       ((PtrPRule) pRule)->PrChrValue = 'N';
	       break;
	     case ThickUnderline:
	       ((PtrPRule) pRule)->PrChrValue = 'T';
	       break;
	     default:
#ifndef NODISPLAY
	       done = FALSE;
#endif
	       TtaError (ERR_invalid_parameter);
	       break;
	     }
	   break;
	 case PtIndent:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   ((PtrPRule) pRule)->PrMinUnit = UnPoint;
	   ((PtrPRule) pRule)->PrMinAttr = FALSE;
	   ((PtrPRule) pRule)->PrMinValue = value;
	   break;
	 case PtLineSpacing:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   ((PtrPRule) pRule)->PrMinUnit = UnPoint;
	   ((PtrPRule) pRule)->PrMinAttr = FALSE;
	   ((PtrPRule) pRule)->PrMinValue = value;
	   break;
	 case PtDepth:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   ((PtrPRule) pRule)->PrIntValue = value;
	   ((PtrPRule) pRule)->PrAttrValue = FALSE;
	   break;
	 case PtAdjust:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   switch (value)
	     {
	     case AdjustLeft:
	       ((PtrPRule) pRule)->PrAdjust = AlignLeft;
	       break;
	     case AdjustRight:
	       ((PtrPRule) pRule)->PrAdjust = AlignRight;
	       break;
	     case Centered:
	       ((PtrPRule) pRule)->PrAdjust = AlignCenter;
	       break;
	     case LeftWithDots:
	       ((PtrPRule) pRule)->PrAdjust = AlignLeftDots;
	       break;
	     default:
#ifndef NODISPLAY
	       done = FALSE;
#endif
	       TtaError (ERR_invalid_parameter);
	       break;
	     }
	   break;
	 case PtJustify:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   switch (value)
	     {
	     case Justified:
	       ((PtrPRule) pRule)->PrJustify = TRUE;
	       break;
	     case NotJustified:
	       ((PtrPRule) pRule)->PrJustify = FALSE;
	       break;
	     default:
#ifndef NODISPLAY
	       done = FALSE;
#endif
	       TtaError (ERR_invalid_parameter);
	       break;
	     }
	   break;
	 case PtHyphenate:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   switch (value)
	     {
	     case Hyphenation:
	       ((PtrPRule) pRule)->PrJustify = TRUE;
	       break;
	     case NoHyphenation:
	       ((PtrPRule) pRule)->PrJustify = FALSE;
	       break;
	     default:
#ifndef NODISPLAY
	       done = FALSE;
#endif
	       TtaError (ERR_invalid_parameter);
	       break;
	     }
	   break;
	 case PtLineStyle:
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   switch (value)
	     {
	     case SolidLine:
	       ((PtrPRule) pRule)->PrChrValue = 'S';
	       break;
	     case DashedLine:
	       ((PtrPRule) pRule)->PrChrValue = '-';
	       break;
	     case DottedLine:
	       ((PtrPRule) pRule)->PrChrValue = '.';
	       break;
	     default:
#ifndef NODISPLAY
	       done = FALSE;
#endif
	       TtaError (ERR_invalid_parameter);
	       break;
	     }
	   break;
	 case PtLineWeight:
	   /* value = thickness of the line in typo points. */
	   ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	   ((PtrPRule) pRule)->PrMinUnit = UnPoint;
	   ((PtrPRule) pRule)->PrMinAttr = FALSE;
	   ((PtrPRule) pRule)->PrMinValue = value;
	   break;
	 case PtFillPattern:
	   if (value < 0)
	     TtaError (ERR_invalid_parameter);
	   else
	     {
	       ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	       ((PtrPRule) pRule)->PrIntValue = value;
	       ((PtrPRule) pRule)->PrAttrValue = FALSE;
	     }
	   break;
	 case PtBackground:
	 case PtForeground:
	   if (value < 0)
	     TtaError (ERR_invalid_parameter);
	   else
	     {
	       ((PtrPRule) pRule)->PrPresMode = PresImmediate;
	       ((PtrPRule) pRule)->PrIntValue = value;
	       ((PtrPRule) pRule)->PrAttrValue = FALSE;
	     }
	   break;
	 case PtFunction:
	   if (((PtrPRule) pRule)->PrPresFunction == FnBackgroundPicture)
	     ((PtrPRule) pRule)->PrPresBox[0] = value;
	   else if (((PtrPRule) pRule)->PrPresFunction == FnPictureMode)
	     ((PtrPRule) pRule)->PrPresBox[0] = value;
	   break;
	 default:
	   TtaError (ERR_invalid_parameter);
#ifndef NODISPLAY
	   done = FALSE;
#endif
	   break;
	 }
#ifndef NODISPLAY
       if (done)
	 RedisplayNewPRule (document, (PtrElement) element, (PtrPRule) pRule);
#endif
     }
}


/*----------------------------------------------------------------------
   TtaSetPRuleView

   Sets the view to which a presentation rule applies. The presentation rule
   must not be attached yet to an element.

   Parameters:
   pRule: the presentation rule to be changed.
   view: the value to be set.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetPRuleView (PRule pRule, int view)
#else  /* __STDC__ */
void                TtaSetPRuleView (pRule, view)
PRule               pRule;
int                 view;
#endif /* __STDC__ */
{
   UserErrorCode = 0;
   if (pRule == NULL)
      TtaError (ERR_invalid_parameter);
   else if (view < 1 || view > MAX_VIEW)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameters are OK */
     ((PtrPRule) pRule)->PrViewNum = view;
}


#ifndef NODISPLAY
/*----------------------------------------------------------------------
   TtaChangeBoxSize

   Changes the height and width of the box corresponding to an element in
   a given view.

   Parameters:
   element: the element of interest.
   document: the document to which the element belongs.
   view: the view.
   deltaX: width increment in points (positive, negative or zero).
   deltaY: height increment in points (positive, negative or zero).
   unit: the unit used for the values.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaChangeBoxSize (Element element, Document document, View view, int deltaX, int deltaY, TypeUnit unit)
#else  /* __STDC__ */
void                TtaChangeBoxSize (element, document, view, deltaX, deltaY, unit)
Element             element;
View                view;
int                 deltaX;
int                 deltaY;
Document            document;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   int                 v, frame;
   int                 x, y;

   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   /* verifies the parameter document */
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is correct */
     {
	frame = GetWindowNumber (document, view);
	if (frame != 0)
	  {
	     if (view < 100)
		/* View of the main tree */
		v = view;
	     else
		/* View of associated elements */
		v = 1;
	     pAb = AbsBoxOfEl ((PtrElement) element, v);
	     if (pAb == NULL)
		TtaError (ERR_element_has_no_box);
	     else
	       {
		  /* Convert values to pixels */
		  if (unit == UnPercent)
		    {
		       if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
			  GetSizesFrame (frame, &x, &y);
		       else
			 {
			    x = pAb->AbEnclosing->AbBox->BxWidth;
			    y = pAb->AbEnclosing->AbBox->BxHeight;
			 }
		       deltaX = PixelValue (deltaX, UnPercent, (PtrAbstractBox) x);
		       deltaY = PixelValue (deltaY, UnPercent, (PtrAbstractBox) y);
		    }
		  else if (unit != UnPixel)
		    {
		       deltaX = PixelValue (deltaX, unit, pAb);
		       deltaY = PixelValue (deltaY, unit, pAb);
		    }
		  NewDimension (pAb, deltaX+pAb->AbBox->BxWidth, deltaY+pAb->AbBox->BxHeight, frame, FALSE);
		  RedispNewGeometry (document, (PtrElement) element);
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   TtaChangeBoxPosition

   Moves the box of an element in a given view.

   Parameters:
   element: the element of interest.
   document: the document to which the element belongs.
   view: the view.
   deltaX: horizontal shift in points (positive, negative or zero).
   deltaY: vertical shift in points (positive, negative or zero).
   unit: the unit used for the values.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaChangeBoxPosition (Element element, Document document, View view, int deltaX, int deltaY, TypeUnit unit)
#else  /* __STDC__ */
void                TtaChangeBoxPosition (element, document, view, deltaX, deltaY, unit)
Element             element;
Document            document;
View                view;
int                 deltaX;
int                 deltaY;
TypeUnit            unit;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   int                 v, frame;
   int                 x, y;

   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   /* verifies the parameter document */
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is correct */
     {
	frame = GetWindowNumber (document, view);
	if (frame != 0)
	  {
	     if (view < 100)
		/* View of the main tree */
		v = view;
	     else
		/* View of associated elements */
		v = 1;
	     pAb = AbsBoxOfEl ((PtrElement) element, v);
	     if (pAb == NULL)
		TtaError (ERR_element_has_no_box);
	     else
	       {
		  /* Convert values to pixels */
		  if (unit == UnPercent)
		    {
		       if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
			  GetSizesFrame (frame, &x, &y);
		       else
			 {
			    x = pAb->AbEnclosing->AbBox->BxWidth;
			    y = pAb->AbEnclosing->AbBox->BxHeight;
			 }
		       deltaX = PixelValue (deltaX, UnPercent, (PtrAbstractBox) x);
		       deltaY = PixelValue (deltaY, UnPercent, (PtrAbstractBox) y);
		    }
		  else if (unit != UnPixel)
		    {
		       deltaX = PixelValue (deltaX, unit, pAb);
		       deltaY = PixelValue (deltaY, unit, pAb);
		    }
		  NewPosition (pAb, deltaX, deltaY, frame, FALSE);
		  RedispNewGeometry (document, (PtrElement) element);
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   TtaGiveBoxSize

   Returns the height and width of the box corresponding to an element in
   a given view.

   Parameters:
   element: the element of interest.
   view: the view.
   unit: the unit used for the values.

   Return parameters:
   width: box width in points.
   height: box height in points.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGiveBoxSize (Element element, Document document, View view, TypeUnit unit, int *width, int *height)
#else  /* __STDC__ */
void                TtaGiveBoxSize (element, document, view, unit, width, height)
Element             element;
Document            document;
View                view;
TypeUnit            unit;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   PtrAbstractBox      pAb;
   int                 v, frame;
   int                 x, y;

   UserErrorCode = 0;
   *width = 0;
   *height = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   /* verifies the parameter document */
   else if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is correct */
     {
	frame = GetWindowNumber (document, view);
	if (frame != 0)
	  {
	     if (view < 100)
		/* View of the main tree */
		v = view;
	     else
		/* View of associated elements */
		v = 1;
	     pAb = AbsBoxOfEl ((PtrElement) element, v);
	     if (pAb == NULL)
		TtaError (ERR_element_has_no_box);
	     else
	       {
		  *width = pAb->AbBox->BxWidth;
		  *height = pAb->AbBox->BxHeight;

		  /* Convert values to pixels */
		  if (unit == UnPercent)
		    {
		       if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
			  GetSizesFrame (frame, &x, &y);
		       else
			 {
			    x = pAb->AbEnclosing->AbBox->BxWidth;
			    y = pAb->AbEnclosing->AbBox->BxHeight;
			 }
		       *width = PixelValue (*width, UnPercent, (PtrAbstractBox) x);
		       *height = PixelValue (*height, UnPercent, (PtrAbstractBox) y);
		    }
		  else if (unit != UnPixel)
		    {
		       *width = PixelValue (*width, unit, pAb);
		       *height = PixelValue (*height, unit, pAb);
		    }
	       }
	  }
     }
}

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
   edge of the box, in points.
   yCoord:  distance from the upper edge of the parent box to the upper
   edge of the box, in points.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGiveBoxPosition (Element element, Document document, View view, TypeUnit unit, int *xCoord, int *yCoord)
#else  /* __STDC__ */
void                TtaGiveBoxPosition (element, Document document, view, unit, xCoord, yCoord)
Element             element;
Document            document;
View                view;
TypeUnit            unit;
int                *xCoord;
int                *yCoord;

#endif /* __STDC__ */
{
  PtrAbstractBox      pAb;
  PtrBox              pBox;
  int                 v, frame;
  int                 x, y;

  UserErrorCode = 0;
  *xCoord = 0;
  *yCoord = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  /* verifies the parameter document */
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      frame = GetWindowNumber (document, view);
      if (frame != 0)
	{
	  if (view < 100)
	    /* View of the main tree */
	    v = view;
	  else
	    /* View of associated elements */
	    v = 1;
	  pAb = AbsBoxOfEl ((PtrElement) element, v);
	  if (pAb == NULL)
	    TtaError (ERR_element_has_no_box);
	  else
	    {
	      pBox = pAb->AbBox;
	      if (pBox->BxType == BoSplit && pBox->BxNexChild != NULL)
		pBox = pBox->BxNexChild;
	      if (pAb->AbEnclosing == NULL || pAb->AbEnclosing->AbBox == NULL)
		{
		  GetSizesFrame (frame, &x, &y);
		  *xCoord = pBox->BxXOrg;
		  *yCoord = pBox->BxYOrg;
		}
	      else
		{
		  x = pAb->AbEnclosing->AbBox->BxWidth;
		  y = pAb->AbEnclosing->AbBox->BxHeight;
		  *xCoord = pBox->BxXOrg - pAb->AbEnclosing->AbBox->BxXOrg;
		  *yCoord = pBox->BxYOrg - pAb->AbEnclosing->AbBox->BxYOrg;
		}
	      
	      /* Convert values to pixels */
	      if (unit == UnPercent)
		{
		  *xCoord = PixelValue (*xCoord, UnPercent, (PtrAbstractBox) x);
		  *yCoord = PixelValue (*yCoord, UnPercent, (PtrAbstractBox) y);
		}
	      else if (unit != UnPixel)
		{
		  *xCoord = PixelValue (*xCoord, unit, pAb);
		  *yCoord = PixelValue (*yCoord, unit, pAb);
		}
	    }
	}
    }
}
#endif

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

#ifdef __STDC__
void                TtaNextPRule (Element element, PRule * pRule)

#else  /* __STDC__ */
void                TtaNextPRule (element, pRule)
Element             element;
PRule              *pRule;

#endif /* __STDC__ */

{
   PtrPRule            nextPRule;

   UserErrorCode = 0;
   nextPRule = NULL;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	if (*pRule == NULL)
	   nextPRule = ((PtrElement) element)->ElFirstPRule;
	else
	   nextPRule = ((PtrPRule) (*pRule))->PrNextPRule;
     }
   *pRule = (PRule) nextPRule;
}

/*----------------------------------------------------------------------
   TtaGetPRule

   Returns a presentation rule of a given type associated with a given element.

   Parameters:
   element: the element of interest.
   presentationType: type of the desired presentation rule. Available
   values are RSize, PtStyle, RFont, RUnderline, RThickness, PtIndent,
   RLineSpacing, RDepth, RAdjust, RJustify, RLineStyle, RLineWeight,
   RFillPattern, RBackground, RForeground, RHyphenate, PRShowBox,
   PRNotInLine.

   Return value:
   the presentation rule found, or NULL if the element
   does not have this type of presentation rule.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
PRule               TtaGetPRule (Element element, int presentationType)
#else  /* __STDC__ */
PRule               TtaGetPRule (element, presentationType)
Element             element;
int                 presentationType;
#endif /* __STDC__ */

{
  PtrPRule          pRule;
  PtrPRule          pPres;
  int               func;
  boolean           found;

  UserErrorCode = 0;
  pRule = NULL;
  if (element == NULL || presentationType < 0)
    TtaError (ERR_invalid_parameter);
  else
    {
      if (presentationType == PRShowBox)
	func = FnShowBox;
      else if (presentationType == PRNotInLine)
	func = FnNotInLine;
      else
	func = -1;

      pPres = ((PtrElement) element)->ElFirstPRule;
      found = FALSE;
      while (pPres != NULL && !found)
	if (pPres->PrType == presentationType
	    || (pPres->PrType == PtFunction && pPres->PrPresFunction == func))
	  {
	    pRule = pPres;
	    found = TRUE;
	  }
	else
	  pPres = pPres->PrNextPRule;
    }
  return ((PRule) pRule);
}

/*----------------------------------------------------------------------
   TtaGetPRuleType

   Returns the type of a presentation rule.

   Parameters:
   pRule: the presentation rule of interest.

   Return value:
   type of that presentation rule. Available values are RSize, PtStyle,
   RFont, RUnderline, RThickness, PtIndent, RLineSpacing, RDepth, RAdjust,
   RJustify, RLineStyle, RLineWeight, RFillPattern, RBackground,
   RForeground, RHyphenate, PRShowBox, PRNotInLine.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetPRuleType (PRule pRule)
#else  /* __STDC__ */
int                 TtaGetPRuleType (pRule)
PRule               pRule;
#endif /* __STDC__ */
{
  int               presentationType;

  UserErrorCode = 0;
  presentationType = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      presentationType = ((PtrPRule) pRule)->PrType;
      if (presentationType == PtFunction && ((PtrPRule) pRule)->PrPresFunction == FnShowBox)
	presentationType = PRShowBox;
      else if (presentationType == PtFunction && ((PtrPRule) pRule)->PrPresFunction == FnNotInLine)
	presentationType = PRNotInLine;
    }
  return presentationType;
}

/*----------------------------------------------------------------------
   TtaGetPRuleValue

   Returns the value of a presentation rule.

   Parameters:
   pRule: the presentation rule of interest.

   Return values according to rule type:
   RSize: an integer between 6 and 72 (body size in points).
   PtStyle: StyleRoman, StyleBold, StyleItalics, StyleOblique,
   StyleBoldItalics, StyleBoldOblique.
   RFont: FontTimes, FontHelvetica, FontCourier.
   RUnderline: NoUnderline, Underline, Overline, CrossOut.
   RThickness: ThinUnderline, ThickUnderline.
   PtIndent: a positive, null or negative integer (indentation in points).
   RLineSpacing: a positive integer (line spacing in points).
   RDepth: a positive integer (depth of the element).
   RAdjust: AdjustLeft, AdjustRight, Centered, LeftWithDots.
   RJustify: Justified, NotJustified.
   RHyphenate: Hyphenation, NoHyphenation.
   RLineStyle: SolidLine, DashedLine, DottedLine.
   RLineWeight: a positive or null integer (stroke width for graphics).
   RFillPattern: rank of the pattern in the file thot.pattern.
   RBackground: rank of the background color in the file thot.color.
   RForeground: rank of the foreground color in the file thot.color.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetPRuleValue (PRule pRule)
#else  /* __STDC__ */
int                 TtaGetPRuleValue (pRule)
PRule               pRule;
#endif /* __STDC__ */
{
  int                 value;

  UserErrorCode = 0;
  value = 0;
  if (pRule == NULL)
    TtaError (ERR_invalid_parameter);
  else
    switch (((PtrPRule) pRule)->PrType)
      {
      case PtSize:
	/* Body-size in typographic points */
	value = ((PtrPRule) pRule)->PrMinValue;
	break;
      case PtStyle:
	switch (((PtrPRule) pRule)->PrChrValue)
	  {
	  case 'R':
	    value = StyleRoman;
	    break;
	  case 'B':
	    value = StyleBold;
	    break;
	  case 'I':
	    value = StyleItalics;
	    break;
	  case 'O':
	    value = StyleOblique;
	    break;
	  case 'G':
	    value = StyleBoldItalics;
	    break;
	  case 'Q':
	    value = StyleBoldOblique;
	    break;
	  default:
	    TtaError (ERR_invalid_parameter);
	    break;
	  }
	break;
      case PtFont:
	switch (((PtrPRule) pRule)->PrChrValue)
	  {
	  case 'T':
	    value = FontTimes;
	    break;
	  case 'H':
	    value = FontHelvetica;
	    break;
	  case 'C':
	    value = FontCourier;
	    break;
	  default:
	    TtaError (ERR_invalid_parameter);
	    break;
	  }
	break;
      case PtUnderline:
	switch (((PtrPRule) pRule)->PrChrValue)
	  {
	  case 'N':
	    value = NoUnderline;
	    break;
	  case 'U':
	    value = Underline;
	    break;
	  case 'O':
	    value = Overline;
	    break;
	  case 'C':
	    value = CrossOut;
	    break;
	  default:
	    TtaError (ERR_invalid_parameter);
	    break;
	  }
	break;
      case PtThickness:
	switch (((PtrPRule) pRule)->PrChrValue)
	  {
	  case 'N':
	    value = ThinUnderline;
	    break;
	  case 'T':
	    value = ThickUnderline;
	    break;
	  default:
	    TtaError (ERR_invalid_parameter);
	    break;
	  }
	break;
      case PtIndent:
	value = ((PtrPRule) pRule)->PrMinValue;
	break;
      case PtLineSpacing:
	value = ((PtrPRule) pRule)->PrMinValue;
	break;
      case PtDepth:
	value = ((PtrPRule) pRule)->PrIntValue;
	break;
      case PtAdjust:
	switch (((PtrPRule) pRule)->PrAdjust)
	  {
	  case AlignLeft:
	    value = AdjustLeft;
	    break;
	  case AlignRight:
	    value = AdjustRight;
	    break;
	  case AlignCenter:
	    value = Centered;
	    break;
	  case AlignLeftDots:
	    value = LeftWithDots;
	    break;
	  default:
	    TtaError (ERR_invalid_parameter);
	    break;
	  }
	break;
      case PtJustify:
	if (((PtrPRule) pRule)->PrJustify)
	  value = Justified;
	else
	  value = NotJustified;
	break;
      case PtHyphenate:
	if (((PtrPRule) pRule)->PrJustify)
	  value = Hyphenation;
	else
	  value = NoHyphenation;
	break;
      case PtLineStyle:
	switch (((PtrPRule) pRule)->PrChrValue)
	  {
	  case 'S':
	    value = SolidLine;
	    break;
	  case '-':
	    value = DashedLine;
	    break;
	  case '.':
	    value = DottedLine;
	    break;
	  default:
	    TtaError (ERR_invalid_parameter);
	    break;
	  }
	break;
      case PtLineWeight:
	/* value = thickness of the line in typographic points */
	value = ((PtrPRule) pRule)->PrMinValue;
	break;
      case PtFillPattern:
      case PtBackground:
      case PtForeground:
	value = ((PtrPRule) pRule)->PrIntValue;
	break;
      case PtWidth:
      case PtHeight:
	value = ((PtrPRule) pRule)->PrDimRule.DrValue;
	break;
      default:
	TtaError (ERR_invalid_parameter);
	break;
      }
  return value;
}

/*----------------------------------------------------------------------
   TtaGetPRuleView

   Returns the number of the view to which a presentation rule applies.

   Parameters:
       pRule: the presentation rule of interest.

   Return value:
       number of the view to which the presentation rule applies.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetPRuleView (PRule pRule)
#else  /* __STDC__ */
int                 TtaGetPRuleView (pRule)
PRule               pRule;
#endif /* __STDC__ */
{
   int                 view;

   UserErrorCode = 0;
   view = 0;
   if (pRule == NULL)
     TtaError (ERR_invalid_parameter);
   else
     view = ((PtrPRule) pRule)->PrViewNum;
   return view;
}

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
#ifdef __STDC__
int                 TtaSamePRules (PRule pRule1, PRule pRule2)
#else  /* __STDC__ */
int                 TtaSamePRules (pRule1, pRule2)
PRule               pRule1;
PRule               pRule2;
#endif /* __STDC__ */
{
   int                 result;
   PtrPRule            pR1, pR2;

   UserErrorCode = 0;
   result = 0;
   if (pRule1 == NULL || pRule2 == NULL)
     TtaError (ERR_invalid_parameter);
   else
     {
	pR1 = (PtrPRule) pRule1;
	pR2 = (PtrPRule) pRule2;
	if (pR1->PrType == pR2->PrType)
	   if (pR1->PrViewNum == pR2->PrViewNum)
	      if (pR1->PrSpecifAttr == pR2->PrSpecifAttr)
		 if (pR1->PrSpecifAttrSSchema == pR2->PrSpecifAttrSSchema)
		    if (pR1->PrPresMode == pR2->PrPresMode)
		       if (pR1->PrPresMode == PresImmediate)
			  switch (pR1->PrType)
				{
				   case PtVisibility:
				   case PtDepth:
				   case PtFillPattern:
				   case PtBackground:
				   case PtForeground:
				      if (pR1->PrAttrValue == pR2->PrAttrValue)
					 if (pR1->PrIntValue == pR2->PrIntValue)
					    result = 1;
				      break;
				   case PtFont:
				   case PtStyle:
				   case PtUnderline:
				   case PtThickness:
				   case PtLineStyle:
				      if (pR1->PrChrValue == pR2->PrChrValue)
					 result = 1;
				      break;
				   case PtBreak1:
				   case PtBreak2:
				   case PtIndent:
				   case PtSize:
				   case PtLineSpacing:
				   case PtLineWeight:
				      if (pR1->PrMinUnit == pR2->PrMinUnit)
					 if (pR1->PrMinAttr == pR2->PrMinAttr)
					    if (pR1->PrMinValue == pR2->PrMinValue)
					       result = 1;
				      break;
				   case PtVertRef:
				   case PtHorizRef:
				   case PtVertPos:
				   case PtHorizPos:
				      if (pR1->PrPosRule.PoDistUnit == pR2->PrPosRule.PoDistUnit)
					 if (pR1->PrPosRule.PoDistance == pR2->PrPosRule.PoDistance)
					    result = 1;
				      break;
				   case PtHeight:
				   case PtWidth:
				      if (pR1->PrDimRule.DrUnit == pR2->PrDimRule.DrUnit)
					 if (pR1->PrDimRule.DrValue == pR2->PrDimRule.DrValue)
					    result = 1;
				      break;
				   case PtJustify:
				   case PtHyphenate:
				      if (pR1->PrJustify == pR2->PrJustify)
					 result = 1;
				      break;
				   case PtAdjust:
				      if (pR1->PrAdjust == pR2->PrAdjust)
					 result = 1;
				      break;
				   default:
				      break;
				}
     }
   return result;
}
