/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
   module:  parsexml.c
   Authors: Monte Regis
            Bonhomme Stephane

   Comments: this module handle specific thot elements and attributes
             (namespace thot) during xml parsing

   Compilation directives: -DXML_DEBUG for Debuging

   
*/
#include "ustring.h"
#include "uconvert.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "tree.h"
#include "content.h"
#include "view.h"
#include "language.h"
#include "attribute.h"
#include "document.h"
#include "application.h"
#include "appstruct.h"
#include "appdialogue.h"
#include "constxml.h"
#include "typexml.h"


#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "edit_tv.h"


typedef struct _XmlPresentationType{
  Element  El;
  STRING   ViewName;
  int      PRuleNum;
  int      PRuleValue;
  TypeUnit PRuleUnit;
  ThotBool PRuleAbs;
  struct  _XmlPresentationType *Next;
}XmlPresentationType;

static CHAR_T     PPrefixName[9] = _EMPTYSTR_;
static CHAR_T     PSchemaName[30] = _EMPTYSTR_;
static int        XmlMaxID = 0;
/*static PrefixType     *ParserPrefixs;*/
static XmlPresentationType *XmlPresentation = NULL;
static int PBnumber = 0;
static int PBview = 0;
static int PBtype = 0;

#include "parsexml_f.h"
#include "xmlmodule_f.h"
#include "changeabsbox_f.h"

/*----------------------------------------------------------------------
  XmlSetPageNum: sets the next pagebreak number
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void XmlSetPageNum (Document doc, Element el, STRING value)
#else
static void XmlSetPageNum (doc, el, value)
Document doc;
Element  el;
STRING   value;
#endif
{
  PBnumber = uctoi (value);
}

/*----------------------------------------------------------------------
  XmlSetPageView : sets the next pagebreak View number
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void XmlSetPageView (Document doc, Element el, STRING value)
#else
static void XmlSetPageView (doc, el, value)
Document doc;
Element  el;
STRING   value;
#endif
{
  PBview = uctoi (value);
}

/*----------------------------------------------------------------------
  XmlSetPageType : sets the next pagebreak Type number
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void XmlSetPageType (Document doc, Element el, STRING value)
#else
static void XmlSetPageType (doc, el, value)
Document doc;
Element  el;
STRING   value;
#endif
{
  PBtype = uctoi (value);
}

/*----------------------------------------------------------------------
  XmlSetPageBreakProperties
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void XmlSetPageBreakProperties (Element el)
#else
void XmlSetPageBreakProperties (el)
Element el;
#endif
{
  if (el != NULL)
    if (TtaGetElementType (el).ElTypeNum == 6) /*element is a Page Break */
      {
	if (PBnumber != 0)
	  {
	    ((PtrElement)el)->ElPageNumber = PBnumber;
	    PBnumber = 0;
	  }
	if (PBview != 0)
	  {
	    ((PtrElement)el)->ElViewPSchema = PBview;
	    PBview = 0;
	  }
	if (PBtype != 0)
	  {
	    ((PtrElement)el)->ElPageType = PBtype;
	    PBtype = 0;
	  }
      }
}



/*----------------------------------------------------------------------
   XmlSetPPrefix
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void XmlSetPPrefix (Document doc, Element el, STRING value)
#else
static void XmlSetPPrefix (doc, el, value)
Document doc;
Element el;
STRING value;
#endif
{
  ustrcpy (PPrefixName, value);
}

/*----------------------------------------------------------------------
   XmlSetPSchema
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void XmlSetPSchema (Document doc, Element el, STRING value)
#else
static void XmlSetPSchema (doc, el, value)
Document doc;
Element el;
STRING value;
#endif
{
  if (PPrefixName[0] != EOS)
    if (!ustrcmp (PPrefixName, DEFAULT_VALUE))
      XmlAddNSPresentation (doc, _EMPTYSTR_, value);
  else
    XmlAddNSPresentation (doc, PPrefixName, value);
  PSchemaName[0] = EOS;
  PPrefixName[0] = EOS;
}

/*----------------------------------------------------------------------
  XmlSetHolophraste 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void XmlSetHolophraste (Document doc, Element el, STRING value)
#else
static void XmlSetHolophraste (doc, el, value)
Document doc;
Element el;
STRING value;
#endif
{
  if ( el != NULL && !ustrcmp (value, TRUE_VALUE))
    TtaHolophrastElement (el, TRUE, doc);
}

/*----------------------------------------------------------------------
   XmlHoldPresentation
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  XmlHoldPresentation (Document doc, Element el, STRING value)
#else
static void  XmlHoldPresentation (doc, el, value)
Document doc;
Element el;
STRING value;
#endif
{

  XmlPresentationType  *newPres;
  int                   begin  = 0;
  int                   end    = 0;
  int                   length = 0;
  ThotBool              sign   = FALSE;

  if (el != NULL)
    {
      length = ustrlen (value) - 1;
      while (begin < length)
	{
	  newPres = (XmlPresentationType *) TtaGetMemory (sizeof (XmlPresentationType));
	  newPres->Next = XmlPresentation;
	  XmlPresentation = newPres;
	  
	  XmlPresentation->El = el;
	  /* reading view name */
	  while (end < length && value[end] != ':') end++;
	  value[end] = EOS;
	  newPres->ViewName = TtaStrdup (&value[begin]);
	  begin = end + 1;
	  /* reading Prule type */
	  while (end<length && value[end] != ':') end++;
	  value[end] = EOS;
	  newPres->PRuleNum = uctoi (&value[begin]);
	  begin = end + 1;

	  if (newPres->PRuleNum == PtHeight ||
	      newPres->PRuleNum == PtWidth ||
	      newPres->PRuleNum == PtVertPos ||
	      newPres->PRuleNum == PtHorizPos || 
	      newPres->PRuleNum == PtBreak1 || 
	      newPres->PRuleNum == PtBreak2 || 
	      newPres->PRuleNum == PtIndent || 
	      newPres->PRuleNum == PtSize || 
	      newPres->PRuleNum == PtLineSpacing || 
	      newPres->PRuleNum == PtLineWeight)
	    {
	      /* reading unit */
	      while(end < length && value[end] != ':') end++;
	      value[end] = EOS;
	      newPres->PRuleUnit = (TypeUnit) uctoi (&value[begin]);
	      begin = end + 1;
	    }

	  if (newPres->PRuleNum == PtHeight ||
	      newPres->PRuleNum == PtWidth) 
	    {
	      /* reading the 'R' for relative or 'A' for absolute */
	      newPres->PRuleAbs = ((value[begin] == 'R')? FALSE : TRUE);
	      begin = begin + 2;
	    }

	  /* reading the sign */
	  if (newPres->PRuleNum == PtHeight ||
	      newPres->PRuleNum == PtWidth ||
	      newPres->PRuleNum == PtVertPos ||
	      newPres->PRuleNum == PtHorizPos ||
	      newPres->PRuleNum == PtIndent)
	    {
	      if (value[begin] == '+') 
		{
		  sign = FALSE;
		  begin++;
		}
	      if (value[begin] == '-') 
		{
		  sign = TRUE;
		  begin++;
		}
	    }
	  /* reading the absolute value */
	  while(end < length && value[end] != ';') end++;
	  value[end] = EOS;
	  newPres->PRuleValue = uctoi (&value[begin]);
	  begin = end + 1;

	  /* signing the value */
	  if (sign)
	    newPres->PRuleValue = -newPres->PRuleValue;
	}
    }
}


/*----------------------------------------------------------------------
   XmlFreePresentationList
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  XmlFreePresentationList ()
#else
void  XmlFreePresentationList ()
#endif
{
  XmlPresentationType  *newPres;
  while (XmlPresentation != NULL)
    {
      newPres = XmlPresentation;
      XmlPresentation = XmlPresentation->Next;
      TtaFreeMemory (newPres->ViewName);
      TtaFreeMemory (newPres);
    }
}
    
/*----------------------------------------------------------------------
   XmlSetPresentation
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  XmlSetPresentation (Document doc)
#else
void  XmlSetPresentation (doc)
Document doc;
#endif
{
  PRule                newPRule;
  XmlPresentationType *inter;
  View                 view;
  DimensionRule       *pDimRule;
  PosRule             *pPosRule;
  PtrPRule             pR1;
  PtrPSchema           pSPR;
  PtrSSchema           pSSR;
  PtrAttribute         pAttr;

  while (XmlPresentation != NULL)
    {
      newPRule = TtaGetPRule (XmlPresentation->El,
			      XmlPresentation->PRuleNum);
      if (newPRule == NULL)
	/* PRule doesn't exist yet */
	{
	  newPRule = TtaNewPRule (XmlPresentation->PRuleNum, TtaGetViewFromName (doc, XmlPresentation->ViewName), doc);

	}
      switch (XmlPresentation->PRuleNum)
	/* Manage the different PRule types */
	{
	case 4: /* case heigh */
	case 5: /* case width */
	  view = TtaGetViewFromName (doc, XmlPresentation->ViewName);
	  pDimRule = &(((PtrPRule)newPRule)->PrDimRule);
	  pDimRule->DrAbsolute = XmlPresentation->PRuleAbs;
	  pDimRule->DrPosition = FALSE;
	  if (!pDimRule->DrAbsolute)
	    {
	      pR1 = GlobalSearchRulepEl ((PtrElement)XmlPresentation->El, 
					 &pSPR, &pSSR, 
					 0, NULL, view,
					 XmlPresentation->PRuleNum, FnAny, 
					 FALSE, TRUE, &pAttr);
	      if (pR1 != NULL)
		{
		  *((PtrPRule)newPRule) = *pR1;
		  ((PtrPRule)newPRule)->PrViewNum = view;
		  ((PtrPRule)newPRule)->PrNextPRule = NULL;
		  ((PtrPRule)newPRule)->PrCond = NULL;
		}
	    }
	  pDimRule->DrAttr = FALSE;
	  pDimRule->DrValue = XmlPresentation->PRuleValue;
	  pDimRule->DrUnit = XmlPresentation->PRuleUnit;
	  pDimRule->DrMin = FALSE;
	  TtaAttachPRule (XmlPresentation->El,
			  newPRule,
			  doc);
	  break;
	case 6: /* case y offset */
	case 7: /* case x offset */
	  view = TtaGetViewFromName (doc, XmlPresentation->ViewName);
	  pPosRule = &(((PtrPRule)newPRule)->PrPosRule);
	  pR1 = GlobalSearchRulepEl ((PtrElement)XmlPresentation->El, 
				     &pSPR, &pSSR, 0, NULL, view,
				     XmlPresentation->PRuleNum, 
				     FnAny, FALSE, TRUE, &pAttr);
	  if (pR1 != NULL)
	    *((PtrPRule)newPRule) = *pR1;
	  ((PtrPRule)newPRule)->PrViewNum = view;
	  ((PtrPRule)newPRule)->PrNextPRule = NULL;
	  ((PtrPRule)newPRule)->PrCond = NULL;
	  pPosRule->PoDistAttr = FALSE;
	  pPosRule->PoDistance = XmlPresentation->PRuleValue;
	  pPosRule->PoDistUnit = XmlPresentation->PRuleUnit;	 
	  TtaAttachPRule (XmlPresentation->El,
			  newPRule,
			  doc);
	  break;
	case PtBreak1:
	case PtBreak2:
	case PtIndent:
	case PtSize:
	case PtLineSpacing:
	case PtLineWeight:
	  ((PtrPRule) newPRule)->PrMinAttr = FALSE;
	  ((PtrPRule) newPRule)->PrMinValue = XmlPresentation->PRuleValue;
	  ((PtrPRule) newPRule)->PrMinUnit = XmlPresentation->PRuleUnit;
	  TtaAttachPRule (XmlPresentation->El,
			  newPRule,
			  doc);
	  break;
	default: /* Other PRules */
	  TtaAttachPRule (XmlPresentation->El,
			  newPRule,
			  doc);
	  TtaSetPRuleValue (XmlPresentation->El,
			    newPRule,
			    XmlPresentation->PRuleValue,
			    doc);
	}
      inter = XmlPresentation->Next;
      TtaFreeMemory (XmlPresentation->ViewName);
      TtaFreeMemory (XmlPresentation);
      XmlPresentation = inter;
    }
  XmlFreePresentationList ();

}

/*----------------------------------------------------------------------
   XmlSetGraphicsShape
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  XmlSetGraphicsShape (Document doc, Element el, STRING value)
#else
static void  XmlSetGraphicsShape (doc, el, value)
Document doc;
Element el;
STRING value;
#endif
{
  if (el != NULL)
    TtaSetGraphicsShape (el, value[0], doc);
}

/*----------------------------------------------------------------------
   XmlSetPairedPosition
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  XmlSetPairedPosition(Document doc, Element el, STRING value)
#else
static void  XmlSetPairedPosition (doc, el, value)
Document doc;
Element el;
STRING value;
#endif
{}

/*----------------------------------------------------------------------
   XmlSetLinePoints
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  XmlSetLinePoints(Document doc, Element el, STRING value)
#else
static void  XmlSetLinePoints (doc, el, value)
Document doc;
Element el;
STRING value;
#endif
{
  int     begin=0;
  int     end=0;
  int     length=0;
  int     x;
  int     y;
  int     rank=1;

  if (el !=NULL)
    {
      length = ustrlen (value)-1;

      while(begin<length)
	{
	  while(end<length&&value[end]!=',') end++;
	  value[end]=EOS;
	  x = uctoi(&value[begin]);
	  begin = end + 1;

	  while(end<length&&value[end]!=';') end++;
	  value[end]=EOS;
	  y = uctoi(&value[begin]);
	  begin = end + 1;

	  TtaAddPointInPolyline(el,rank,UnPoint,x,y,doc);
	  rank++;
	}
    }
}

/*----------------------------------------------------------------------
   XmlSetSrc
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  XmlSetSrc (Document doc, Element el, STRING value)
#else
static void  XmlSetSrc (doc, el, value)
Document doc;
Element el;
STRING value;
#endif
{
  if (el != NULL)
    TtaSetTextContent(el, value, SPACE, doc);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  XmlSetMaxId (Document doc, Element el, STRING value)
#else
static void  XmlSetMaxId (doc, el, value)
Document doc;
Element el;
STRING value;
#endif
{
  if (value != 0)
    XmlMaxID = uctoi (value);
  else
    {
      ((PtrDocument) LoadedDocument[doc - 1])-> DocLabelExpMax = XmlMaxID;
      XmlMaxID = 0;
    }
}


static XmlAttrEntry ThotAttr[] = 
{
  {P_PREFIX_ATTR, (Proc) XmlSetPPrefix},
  {P_SCHEMA_ATTR, (Proc) XmlSetPSchema},
  {HOLOPHRASTE_ATTR, (Proc) XmlSetHolophraste},
  {STYLE_ATTR, (Proc) XmlHoldPresentation},
  {GRAPH_CODE_ATTR, (Proc) XmlSetGraphicsShape},
  {PAIRED_ATTR, (Proc) XmlSetPairedPosition},
  {LINE_POINTS_ATTR, (Proc) XmlSetLinePoints},
  {SRC_ATTR, (Proc) XmlSetSrc},
  {NLABEL_ATTR, (Proc) XmlSetMaxId},
  {PG_NUM_ATTR, (Proc) XmlSetPageNum},
  {PG_VIEW_ATTR, (Proc) XmlSetPageView},
  {PG_TYPE_ATTR, (Proc) XmlSetPageType},
  {_EMPTYSTR_, (Proc) NULL},
};

/*----------------------------------------------------------------------
  ParseThotAttribute: parse  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool ParseThotAttribute (Document doc, Element el, STRING attrName, STRING value)
#else /* __STDC__ */
ThotBool ParseThotAttribute (Document doc, Element el, STRING attrName, STRING value)
Document doc;
Element el;
STRING attrName;
STRING  value;
#endif /* __STDC__ */

{
  int i=0;
  while (ThotAttr[i].AttrAction != (Proc)NULL && 
	 ustrcmp(attrName,ThotAttr[i].AttrName)) 
    i++;

  if (ThotAttr[i].AttrAction != (Proc)NULL)
    {
      (*(ThotAttr[i].AttrAction))(doc,el,value);
    }
  else 
    {
      return (FALSE);
    }
  return (TRUE);
}

/* y'avait un truc sur les exceptions des tables */

static Proc tabTableActions[20];
/*----------------------------------------------------------------------
  StoreTableActions
  temporarely set table actions off 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void StoreTableActions ()
#else /* __STDC__ */
void StoreTableActions ()
#endif /* __STDC__ */
{
  int i;

  for (i = 0; i < T_deletepageab - T_createtable; i ++)
    {
      tabTableActions[i] = ThotLocalActions[i + T_createtable];
      TteConnectAction (i + T_createtable, (Proc) NULL);
    }
}

/*----------------------------------------------------------------------
  RestoreTableActions
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void RestoreTableActions ()
#else /* __STDC__ */
void RestoreTableActions ()
#endif /* __STDC__ */
{
  int i;

  for (i = 0; i < T_deletepageab - T_createtable; i ++)
    {
      TteConnectAction (i + T_createtable, tabTableActions[i]);
    }
}
