/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
   Ce programme effectue le chargement d'un schema de presentation et
   du schema de structure associe' et liste le contenu du schema de
   presentation.        
 *
 * Author: D. Veillard (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "prsmsg.h"
#include "fileaccess.h"
#include "message.h"
#include "pschema.h"

#define THOT_EXPORT
#include "platform_tv.h"

#include "readprs_f.h"
#include "memory_f.h"
#include "fileaccess_f.h"

FILE               *output = NULL;
static PtrSSchema   pSchemaStr;
static PtrPSchema   pSchemaPrs;

#ifdef STANDALONE
static char         filename[1000];

#endif
static int          i, j, El, GetAttributeOfElement, Val;

/* Identification des messages */
static int          PRS;
PtrPSchema          pSc1;
Counter            *pCo1;
CntrItem           *pCp1;
PresConstant       *pPr1;
PresVariable       *pPres1;
PresVarItem        *pVa1;
PresentationBox    *pBo1;
TtAttribute        *pAt1;
AttributePres      *pRP1;
NumAttrCase        *pCa1;
ThotBool            Transm;


#ifdef putchar
#undef putchar
#define putchar(c) putc(c, output)
#else
#define putchar(c) putc(c, output)
#endif

#ifdef __STDC__
extern char        *TtaGetEnvString (char *);

#else  /* __STDC__ */
extern char        *TtaGetEnvString ();

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   ReadEnv lit la variable THOTSCH dans SchemaPath.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReadEnv ()
#else  /* __STDC__ */
static void         ReadEnv ()
#endif				/* __STDC__ */
{
   char               *pT;

   /* lit la variable d'environnement THOTSCH */
   pT = (char *) TtaGetEnvString ("THOTSCH");
   if (pT == NULL)
      /* la variable d'environnement THOTSCH n'existe pas */
      SchemaPath[0] = EOS;
   else
      /* on copie la valeur de la variable THOTSCH */
      strncpy (SchemaPath, pT, MAX_TXT_LEN);
}


/*----------------------------------------------------------------------
   wrnb ecrit au terminal l'entier nb.                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrnb (int nb)

#else  /* __STDC__ */
static void         wrnb (nb)
int                 nb;

#endif /* __STDC__ */

{
   if (nb < 0)
     {
	fprintf (output, "-");
	nb = -nb;
     }
   if (nb < 10)
      fprintf (output, "%1d", nb);
   else if (nb < 100)
      fprintf (output, "%2d", nb);
   else if (nb < 1000)
      fprintf (output, "%3d", nb);
   else if (nb < 10000)
      fprintf (output, "%4d", nb);
   else if (nb < 100000)
      fprintf (output, "%5d", nb);
   else
      fprintf (output, "999999999");
}


/*----------------------------------------------------------------------
   wrniveau ecrit au terminal le niveau relatif n.                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrniveau (Level n)

#else  /* __STDC__ */
static void         wrniveau (n)
Level               n;

#endif /* __STDC__ */

{
   switch (n)
	 {
	    case RlEnclosing:
	       fprintf (output, "Enclosing");
	       break;
	    case RlSameLevel:

	       break;
	    case RlEnclosed:
	       fprintf (output, "Enclosed");
	       break;
	    case RlPrevious:
	       fprintf (output, "Previous");
	       break;
	    case RlNext:
	       fprintf (output, "Next");
	       break;
	    case RlSelf:
	       fprintf (output, "*");
	       break;
	    case RlContainsRef:
	       fprintf (output, "Refering");
	       break;
	    case RlRoot:
	       fprintf (output, "Root");
	       break;
	    case RlReferred:
	       fprintf (output, "Referred");
	       break;
	    case RlCreator:
	       fprintf (output, "Creator");
	       break;
	 }
}


/*----------------------------------------------------------------------
   wrrepere ecrit au terminal le repere de boite r.                
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrrepere (BoxEdge r)

#else  /* __STDC__ */
static void         wrrepere (r)
BoxEdge             r;

#endif /* __STDC__ */

{
   switch (r)
	 {
	    case Top:
	       fprintf (output, "Top");
	       break;
	    case Bottom:
	       fprintf (output, "Bottom");
	       break;
	    case Left:
	       fprintf (output, "Left");
	       break;
	    case Right:
	       fprintf (output, "Right");
	       break;
	    case HorizRef:
	       fprintf (output, "HRef");
	       break;
	    case VertRef:
	       fprintf (output, "VRef");
	       break;
	    case HorizMiddle:
	       fprintf (output, "HMiddle");
	       break;
	    case VertMiddle:
	       fprintf (output, "VMiddle");
	       break;
	    case NoEdge:
	       fprintf (output, "NULL");
	       break;
	 }
}


/*----------------------------------------------------------------------
   wrnom ecrit au terminal le nom n.                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrnom (Name n)

#else  /* __STDC__ */
static void         wrnom (n)
Name                n;

#endif /* __STDC__ */

{
   int                 i;

   i = 1;
   while (n[i - 1] != EOS)
     {
	if (n[i - 1] < SPACE || n[i - 1] > TEXT('~'))
	   /* caractere non imprimable, ecrit son code */
	  {
	     fprintf (output, "\\%o", (unsigned char) n[i - 1]);
	  }
	else
	  {
	     /* caractere imprimable, ecrit le  caractere */
	     putchar (n[i - 1]);
	  }
	i++;
     }
}


/*----------------------------------------------------------------------
   wrnomregle ecrit au terminal le nom de la regle de numero r.    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrnomregle (int r)

#else  /* __STDC__ */
static void         wrnomregle (r)
int                 r;

#endif /* __STDC__ */

{
   if (r > 0)
      wrnom (pSchemaStr->SsRule[r - 1].SrName);
}


/*----------------------------------------------------------------------
   wrnomattr ecrit au terminal le nom de l'attribut de numero a.   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrnomattr (int a)

#else  /* __STDC__ */
static void         wrnomattr (a)
int                 a;

#endif /* __STDC__ */

{
   if (a != 0)
      wrnom (pSchemaStr->SsAttribute[abs (a) - 1].AttrName);
}


/*----------------------------------------------------------------------
   wrnomboite ecrit au terminal le nom de la boite de presentation 
   de numero b.                                            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrnomboite (int b)

#else  /* __STDC__ */
static void         wrnomboite (b)
int                 b;

#endif /* __STDC__ */

{
   if (b > 0)
      wrnom (pSchemaPrs->PsPresentBox[b - 1].PbName);
}


/*----------------------------------------------------------------------
   wrModeHerit ecrit au terminal un mode d'heritage.               
  ----------------------------------------------------------------------*/


#ifdef __STDC__
static void         wrModeHerit (InheritMode M)

#else  /* __STDC__ */
static void         wrModeHerit (M)
InheritMode         M;

#endif /* __STDC__ */

{
   switch (M)
	 {
	    case InheritParent:
	       fprintf (output, "Enclosing");
	       break;
	    case InheritPrevious:
	       fprintf (output, "Previous");
	       break;
	    case InheritChild:
	       fprintf (output, "Enclosed");
	       break;
	    case InheritCreator:
	       fprintf (output, "Creator");
	       break;
	    case InheritGrandFather:
	       fprintf (output, "GrandFather");
	       break;
	 }
}


/*----------------------------------------------------------------------
   wrsize ecrit au terminal la regle de taille pointee par pR.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrsize (PtrPRule pR)

#else  /* __STDC__ */
static void         wrsize (pR)
PtrPRule            pR;

#endif /* __STDC__ */

{
   PtrPRule            pRe1;

   pRe1 = pR;
   if (pRe1->PrPresMode == PresInherit)
     {
	wrModeHerit (pRe1->PrInheritMode);
        if (pRe1->PrInhPercent)
          {
          fprintf (output, " * ");
          if (pRe1->PrInhAttr)
             wrnomattr (pRe1->PrInhDelta);
          else
             wrnb (pRe1->PrInhDelta);
          fprintf (output, " %");
          }
        else
	  if (pRe1->PrInhDelta == 0)
	     fprintf (output, " =");
	  else
	     {
	     if (pRe1->PrInhDelta > 0)
		fprintf (output, "+");
	     if (pRe1->PrInhAttr)
		wrnomattr (pRe1->PrInhDelta);
	     else
		wrnb (pRe1->PrInhDelta);
	     if (pRe1->PrInhUnit == UnPoint)
	        fprintf (output, " pt");
	     }
	if (pRe1->PrInhMinOrMax > 0)
	  {
	     if (pRe1->PrInhDelta >= 0)
		fprintf (output, " max ");
	     else
		fprintf (output, " min ");
	     if (pRe1->PrMinMaxAttr)
		wrnomattr (pRe1->PrInhMinOrMax);
	     else
		wrnb (pRe1->PrInhMinOrMax);
	  }
     }
   else if (pRe1->PrPresMode == PresImmediate)
     {
	if (pRe1->PrMinAttr)
	   wrnomattr (pRe1->PrMinValue);
	else
	   wrnb (pRe1->PrMinValue);
	if (pRe1->PrMinUnit == UnPoint)
	   fprintf (output, " pt");
     }
   else
      fprintf (output, "??????");
   fprintf (output, ";");
}


/*----------------------------------------------------------------------
   wrfontstyle ecrit au terminal la regle d'heritage ou la valeur  
   entiere de la regle pointee par pR.                     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrfontstyle (PtrPRule pR)

#else  /* __STDC__ */
static void         wrfontstyle (pR)
PtrPRule            pR;

#endif /* __STDC__ */

{
   PtrPRule            pRe1;

   pRe1 = pR;
   if (pRe1->PrPresMode == PresInherit)
     {
	wrModeHerit (pRe1->PrInheritMode);
	if (pRe1->PrInhDelta == 0 && !pRe1->PrInhPercent)
	   fprintf (output, " =");
	else
	   fprintf (output, "??????");
     }
   else if (pRe1->PrPresMode == PresImmediate)
      if (pRe1->PrType == PtFont)
	 switch (pRe1->PrChrValue)
	       {
		  case 'C':
		     fprintf (output, "Courrier");
		     break;
		  case 'H':
		     fprintf (output, "Helvetica");
		     break;
		  case 'T':
		     fprintf (output, "Times");
		     break;
		  case 'c':
		     fprintf (output, "courrier");
		     break;
		  case 'h':
		     fprintf (output, "helvetica");
		     break;
		  case 't':
		     fprintf (output, "times");
		     break;
		  default:
		     putchar (pRe1->PrChrValue);
		     break;
	       }
      else if (pRe1->PrType == PtStyle)
	 switch (pRe1->PrChrValue)
	       {
		  case 'I':
		     fprintf (output, "Italics");
		     break;
		  case 'R':
		     fprintf (output, "Roman");
		     break;
		  case 'O':
		     fprintf (output, "Oblique");
		     break;
		  default:
		     putchar (pRe1->PrChrValue);
		     break;
	       }
      else if (pRe1->PrType == PtWeight)
	 switch (pRe1->PrChrValue)
	       {
		  case 'B':
		     fprintf (output, "Bold");
		     break;
		  case 'N':
		     fprintf (output, "Normal");
		     break;
		  default:
		     putchar (pRe1->PrChrValue);
		     break;
	       }
      else if (pRe1->PrType == PtUnderline)
	 switch (pRe1->PrChrValue)
	       {
		  case 'N':
		     fprintf (output, "NoUnderline");
		     break;
		  case 'U':
		     fprintf (output, "Underlined");
		     break;
		  case 'O':
		     fprintf (output, "Overlined");
		     break;
		  case 'C':
		     fprintf (output, "CrossedOut");
		     break;
		  default:
		     putchar (pRe1->PrChrValue);
		     break;
	       }
      else if (pRe1->PrType == PtThickness)
	 switch (pRe1->PrChrValue)
	       {
		  case 'T':
		     fprintf (output, "Thick");
		     break;
		  case 'N':
		     fprintf (output, "Thin");
		     break;
		  default:
		     putchar (pRe1->PrChrValue);
		     break;
	       }
      else if (pRe1->PrType == PtLineStyle)
	 switch (pRe1->PrChrValue)
	       {
		  case 'S':
		     fprintf (output, "Solid");
		     break;
		  case '-':
		     fprintf (output, "Dashed");
		     break;
		  case '.':
		     fprintf (output, "Dotted");
		     break;
	       }
      else
	 putchar (pRe1->PrChrValue);
   else
      fprintf (output, "??????");
   fprintf (output, ";");
}


/*----------------------------------------------------------------------
   wrnbherit ecrit au terminal la regle d'heritage ou la valeur    
   entiere de la regle pointee par pR.                     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrnbherit (PtrPRule pR)

#else  /* __STDC__ */
static void         wrnbherit (pR)
PtrPRule            pR;

#endif /* __STDC__ */

{
   PtrPRule            pRe1;


   pRe1 = pR;
   if (pRe1->PrPresMode == PresInherit)
     if (pRe1->PrInhPercent)
        fprintf (output, "??????");
     else
        {
	wrModeHerit (pRe1->PrInheritMode);
	if (pRe1->PrInhDelta == 0)
	   fprintf (output, " =");
	else
	  {
	     if (pRe1->PrInhDelta > 0)
		fprintf (output, "+");
	     if (pRe1->PrInhAttr)
		wrnomattr (pRe1->PrInhDelta);
	     else
		wrnb (pRe1->PrInhDelta);
	  }
	if (pRe1->PrInhMinOrMax > 0)
	  {
	     if (pRe1->PrInhDelta >= 0)
		fprintf (output, " max ");
	     else
		fprintf (output, " min ");
	     if (pRe1->PrMinMaxAttr)
		wrnomattr (pRe1->PrInhMinOrMax);
	     else
		wrnb (pRe1->PrInhMinOrMax);
	  }
        }
   else if (pRe1->PrPresMode == PresImmediate)
      if (pRe1->PrAttrValue)
	 wrnomattr (pRe1->PrIntValue);
      else
	 wrnb (pRe1->PrIntValue);
   else
      fprintf (output, "??????");
   fprintf (output, ";");
}


/*----------------------------------------------------------------------
   wrminind ecrit au terminal la regle 'NoBreak1, NoBreak2 ou      
   Indent pointee par pR.                                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrminind (PtrPRule pR)

#else  /* __STDC__ */
static void         wrminind (pR)
PtrPRule            pR;

#endif /* __STDC__ */

{
   PtrPRule            pRe1;

   pRe1 = pR;
   if (pRe1->PrPresMode == PresInherit)
      wrnbherit (pR);
   else
     {
	if (pRe1->PrPresMode == PresImmediate)
	  {
	     if (pRe1->PrMinAttr)
		wrnomattr (pRe1->PrMinValue);
	     else
		wrnb (pRe1->PrMinValue);
	     if (pRe1->PrMinUnit == UnPoint)
		fprintf (output, " pt");
	  }
	else
	   fprintf (output, "??????");
	fprintf (output, ";");
     }
}


/*----------------------------------------------------------------------
   wrpos ecrit au terminal la position pos.                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrpos (PosRule pos, ThotBool Def)

#else  /* __STDC__ */
static void         wrpos (pos, Def)
PosRule             pos;
ThotBool            Def;

#endif /* __STDC__ */

{
   PosRule            *pRe1;

   pRe1 = &pos;
   if (Def)
      if (pRe1->PoPosDef == NoEdge)
	 fprintf (output, " NULL");
      else
	{
	   wrrepere (pRe1->PoPosDef);
	   fprintf (output, " = ");
	}
   if (!Def || pRe1->PoPosDef != NoEdge)
     {
	wrniveau (pRe1->PoRelation);
	if (pRe1->PoNotRel)
	   fprintf (output, " NOT");
	fprintf (output, " ");
	if (pRe1->PoRefKind == RkElType)
	   wrnomregle (pRe1->PoRefIdent);
	else (pRe1->PoRefKind == RkPresBox)
	   wrnomboite (pRe1->PoRefIdent);
	else (pRe1->PoRefKind == RkAttr)
	   wrnomattr (pRe1->PoRefIdent);
	fprintf (output, ". ");
	wrrepere (pRe1->PoPosRef);
	if (pRe1->PoDistance != 0)
	  {
	     if (pRe1->PoDistance > 0)
		fprintf (output, "+");
	     else
		fprintf (output, "-");
	     if (pRe1->PoDistAttr)
		wrnomattr (abs (pRe1->PoDistance));
	     else
		wrnb (abs (pRe1->PoDistance));
	     if (pRe1->PoDistUnit == UnPoint)
		fprintf (output, " pt");
	  }
	if (pRe1->PoUserSpecified)
	   fprintf (output, " UserSpecified");
     }
   fprintf (output, ";");
}


/*----------------------------------------------------------------------
   wrdimens ecrit au terminal la dimension Dim.                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrdimens (DimensionRule Dim, ThotBool Hauteur)

#else  /* __STDC__ */
static void         wrdimens (Dim, Hauteur)
DimensionRule       Dim;
ThotBool            Hauteur;

#endif /* __STDC__ */

{
   DimensionRule      *pRe1;

   pRe1 = &Dim;
   if (pRe1->DrPosition)
      wrpos (pRe1->DrPosRule, TRUE);
   else
     {
	if (pRe1->DrAbsolute)
	  {
	     if (pRe1->DrAttr)
		wrnomattr (pRe1->DrValue);
	     else
		wrnb (pRe1->DrValue);
	     if (pRe1->DrValue != 0)
		if (pRe1->DrUnit == UnPoint)
		   fprintf (output, " pt");
	     if (pRe1->DrUserSpecified)
		fprintf (output, " UserSpecified");
	     if (pRe1->DrMin)
		fprintf (output, " Min");
	  }
	else
	  {
	     wrniveau (pRe1->DrRelation);
	     fprintf (output, " ");
	     if (pRe1->DrNotRelat)
		fprintf (output, "not ");
	     if (pRe1->DrRefKind == RkElType)
		wrnomregle (pRe1->DrRefIdent);
	     else if (pRe1->DrRefKind == RkPresBox)
		wrnomboite (pRe1->DrRefIdent);
	     else if (pRe1->DrRefKind == RkAttr)
		wrnomattr (pRe1->DrRefIdent);
	     fprintf (output, ". ");
	     if ((pRe1->DrSameDimens && Hauteur) || (!pRe1->DrSameDimens && !Hauteur))
		fprintf (output, "Height");
	     else
		fprintf (output, "Width");
	     if (pRe1->DrUnit == UnPercent)
	       {
		  fprintf (output, "*");
		  if (pRe1->DrValue < 0)
		     fprintf (output, "-");
		  if (pRe1->DrAttr)
		     wrnomattr (abs (pRe1->DrValue));
		  else
		     wrnb (abs (pRe1->DrValue));
		  fprintf (output, "%%");
	       }
	     else
	       {
		  if (pRe1->DrValue < 0)
		     fprintf (output, "-");
		  if (pRe1->DrValue > 0)
		     fprintf (output, "+");
		  if (pRe1->DrValue != 0)
		    {
		       if (pRe1->DrAttr)
			  wrnomattr (abs (pRe1->DrValue));
		       else
			  wrnb (abs (pRe1->DrValue));
		       if (pRe1->DrUnit == UnPoint)
			  fprintf (output, " pt");
		    }
	       }
	     if (pRe1->DrMin)
		fprintf (output, " Min");
	  }
	fprintf (output, ";");
     }
}


/*----------------------------------------------------------------------
   wrCondition                                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrCondition (PtrCondition pCond)

#else  /* __STDC__ */
static void         wrCondition (pCond)
PtrCondition        pCond;

#endif /* __STDC__ */

{
   if (!pCond->CoNotNegative)
      fprintf (output, "NOT ");
   switch (pCond->CoCondition)
	 {
	    case PcFirst:
	       fprintf (output, "First ");
	       break;
	    case PcLast:
	       fprintf (output, "Last ");
	       break;
	    case PcReferred:
	       fprintf (output, "Referred ");
	       break;
	    case PcFirstRef:
	       fprintf (output, "FirstRef ");
	       break;
	    case PcLastRef:
	       fprintf (output, "LastRef ");
	       break;
	    case PcExternalRef:
	       fprintf (output, "ExternalRef ");
	       break;
	    case PcInternalRef:
	       fprintf (output, "InternalRef ");
	       break;
	    case PcCopyRef:
	       fprintf (output, "PcCopyRef ");
	       break;
	    case PcAnyAttributes:
	       fprintf (output, "AnyAttributes ");
	       break;
	    case PcFirstAttr:
	       fprintf (output, "FirstAttr ");
	       break;
	    case PcLastAttr:
	       fprintf (output, "LastAttr ");
	       break;
	    case PcUserPage:
	       fprintf (output, "UserPage ");
	       break;
	    case PcStartPage:
	       fprintf (output, "StartPage ");
	       break;
	    case PcComputedPage:
	       fprintf (output, "ComputedPage ");
	       break;
	    case PcEmpty:
	       fprintf (output, "PcEmpty ");
	       break;
	    case PcEven:
	       fprintf (output, "Even");
	       break;
	    case PcOdd:
	       fprintf (output, "Odd");
	       break;
	    case PcOne:
	       fprintf (output, "One");
	       break;
	    case PcWithin:
	       if (pCond->CoImmediate)
		  fprintf (output, "Immediately ");
	       fprintf (output, "Within ");
	       if (pCond->CoAncestorRel == CondGreater &&
		   pCond->CoRelation != 0)
		  fprintf (output, ">");
	       else if (pCond->CoAncestorRel == CondLess)
		  fprintf (output, "<");
	       if (pCond->CoRelation > 0 ||
		   pCond->CoAncestorRel == CondEquals)
		 {
		    wrnb (pCond->CoRelation);
		    fprintf (output, " ");
		 }
	       if (pCond->CoTypeAncestor == 0)
		 {
		    wrnom (pCond->CoAncestorName);
		    fprintf (output, "(");
		    wrnom (pCond->CoSSchemaName);
		    fprintf (output, ")");
		 }
	       else
		  wrnomregle (pCond->CoTypeAncestor);
	       fprintf (output, " ");
	       break;
	    case PcInterval:
	       if (pCond->CoCounter > 0)
		 {
		    fprintf (output, "(");
		    if (pCond->CoValCounter == CntMaxVal)
		       fprintf (output, "MaxRangeVal ");
		    else if (pCond->CoValCounter == CntMinVal)
		       fprintf (output, "MinRangeVal ");
		    fprintf (output, "Cpt");
		    wrnb (pCond->CoCounter);
		    fprintf (output, ") ");
		 }
	       break;
	    case PcElemType:
	       wrnomregle (pCond->CoTypeElAttr);
	       fprintf (output, " ");
	       break;
	    case PcAttribute:
	       wrnomattr (pCond->CoTypeElAttr);
	       fprintf (output, " ");
	       break;
	    default:
	       break;
	 }
   if (pCond->CoCondition == PcEven || pCond->CoCondition == PcOdd ||
       pCond->CoCondition == PcOne)
      if (pCond->CoCounter > 0)
	{
	   fprintf (output, "(Cpt");
	   wrnb (pCond->CoCounter);
	   fprintf (output, ") ");
	}
}

/*----------------------------------------------------------------------
   wrFonctPres ecrit au terminal la fonction de presentation       
   contenue dans la regle pointee par pR.                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrFonctPres (PtrPRule pR)

#else  /* __STDC__ */
static void         wrFonctPres (pR)
PtrPRule            pR;

#endif /* __STDC__ */

{
   int                 i;
   PtrPRule            pRe1;

   pRe1 = pR;
   switch (pRe1->PrPresFunction)
	 {
	    case FnLine:
	       fprintf (output, "Line");
	       break;
	    case FnNoLine:
	       fprintf (output, "NoLine");
	       break;
	    case FnCreateBefore:
	       fprintf (output, "CreateBefore");
	       break;
	    case FnCreateWith:
	       fprintf (output, "CreateWith");
	       break;
	    case FnCreateFirst:
	       fprintf (output, "CreateFirst");
	       break;
	    case FnCreateLast:
	       fprintf (output, "CreateLast");
	       break;
	    case FnCreateAfter:
	       fprintf (output, "CreateAfter");
	       break;
	    case FnCreateEnclosing:
	       fprintf (output, "CreateEnclosing");
	       break;
	    case FnPage:
	       fprintf (output, "Page");
	       break;
	    case FnColumn:
	       fprintf (output, "Column");
	       break;
	    case FnSubColumn:
	       fprintf (output, "Included Column");
	       break;
	    case FnCopy:
	       fprintf (output, "Copy");
	       break;
	    case FnContentRef:
	       fprintf (output, "Content: Cste");
	       wrnb (pRe1->PrPresBox[0]);
	       break;
	    case FnShowBox:
	       fprintf (output, "ShowBox");
	       break;
	    case FnBackgroundPicture:
	       fprintf (output, "BackgroundPicture: Cste");
	       wrnb (pRe1->PrPresBox[0]);
	       break;
            case FnPictureMode:
               fprintf (output, "PictureMode: ");
               switch (pRe1->PrPresBox[0])
                 {
                 case RealSize:
                    fprintf (output, "NormalSize");
                    break;
                 case ReScale:
                    fprintf (output, "Scale");
                    break;
                 case FillFrame:
                    fprintf (output, "RepeatXY");
                    break;
                 case XRepeat:
                    fprintf (output, "RepeatX");
                    break;
                 case YRepeat:
                    fprintf (output, "RepeatY");
                    break;
                 default:
                    fprintf (output, "??????");
                    break;
                 }
               break;
	    case FnNotInLine:
	       fprintf (output, "InLine: No");
	       break;
	 }
   if (pRe1->PrPresFunction != FnLine && pRe1->PrPresFunction != FnContentRef
       && pRe1->PrPresFunction != FnNoLine && pRe1->PrPresFunction != ShowBox
       && pRe1->PrPresFunction != FnPictureMode
       && pRe1->PrPresFunction != FnBackgroundPicture
       && pRe1->PrPresFunction != FnNotInLine)
     {
	fprintf (output, "(");
	if (pRe1->PrNPresBoxes == 0)
	  {
	     wrnom (pRe1->PrPresBoxName);
	     if (pRe1->PrExternal || !pRe1->PrElement)
		fprintf (output, "(****)");
	  }
	else
	   for (i = 1; i <= pRe1->PrNPresBoxes; i++)
	     {
		if (i > 1)
		   fprintf (output, ", ");
		if (pRe1->PrElement)
		   wrnomregle (pRe1->PrPresBox[i - 1]);
		else
		   wrnomboite (pRe1->PrPresBox[i - 1]);
	     }
	fprintf (output, ")");
     }
   fprintf (output, ";");
}


/*----------------------------------------------------------------------
   wrajust ecrit au terminal la regle d'ajustement des lignes.     
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrajust (PtrPRule pR)

#else  /* __STDC__ */
static void         wrajust (pR)
PtrPRule            pR;

#endif /* __STDC__ */

{
   PtrPRule            pRe1;

   pRe1 = pR;
   if (pRe1->PrPresMode == PresInherit)
      wrnbherit (pR);
   if (pRe1->PrPresMode == PresImmediate)
      switch (pRe1->PrAdjust)
	    {
	       case AlignLeft:
		  fprintf (output, "Left;");
		  break;
	       case AlignRight:
		  fprintf (output, "Right;");
		  break;
	       case AlignCenter:
		  fprintf (output, "VMiddle;");
		  break;
	       case AlignLeftDots:
		  fprintf (output, "LeftWithDots;");
		  break;
	    }
}


/*----------------------------------------------------------------------
   wrjustif ecrit la regle de justification ou hyphenation pointee 
   par pR.                                                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrjustif (PtrPRule pR)

#else  /* __STDC__ */
static void         wrjustif (pR)
PtrPRule            pR;

#endif /* __STDC__ */

{
   PtrPRule            pRe1;

   pRe1 = pR;
   if (pRe1->PrPresMode == PresInherit)
      wrnbherit (pR);
   if (pRe1->PrPresMode == PresImmediate)
      if (pRe1->PrJustify)
	 fprintf (output, "Yes;");
      else
	 fprintf (output, "No;");
}


/*----------------------------------------------------------------------
   WriteCounterStyle ecrit au terminal un style de compteur.            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         WriteCounterStyle (CounterStyle St)

#else  /* __STDC__ */
static void         WriteCounterStyle (St)
CounterStyle        St;

#endif /* __STDC__ */

{
   switch (St)
	 {
	    case CntArabic:
	       fprintf (output, ",Arabic)");
	       break;
	    case CntURoman:
	       fprintf (output, ",URoman)");
	       break;
	    case CntLRoman:
	       fprintf (output, ",LRoman)");
	       break;
	    case CntUppercase:
	       fprintf (output, ",Uppercase)");
	       break;
	    case CntLowercase:
	       fprintf (output, ",Lowercase)");
	       break;
	 }
}


/*----------------------------------------------------------------------
   wrsuiteregles ecrit au terminal la suite de regles chainees dont 
   RP pointe sur la regle de tete.                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrsuiteregles (PtrPRule RP)

#else  /* __STDC__ */
static void         wrsuiteregles (RP)
PtrPRule            RP;

#endif /* __STDC__ */

{
   PtrPRule            pRe1;
   PtrCondition        pCond;

   while (RP != NULL)
      /* ecrit une regle de presentation */
     {
	pRe1 = RP;
	fprintf (output, "   ");
	if (pRe1->PrViewNum > 1)
	  {
	     fprintf (output, "IN ");
	     wrnom (pSchemaPrs->PsView[pRe1->PrViewNum - 1]);
	     fprintf (output, " ");
	  }
	if (pRe1->PrCond != NULL)
	  {
	     pCond = pRe1->PrCond;
	     if (pCond->CoCondition == PcDefaultCond)
		fprintf (output, "OTHERWISE ");
	     else
	       {
		  fprintf (output, "IF ");
		  wrCondition (pCond);
	       }
	     pCond = pCond->CoNextCondition;
	     while (pCond != NULL)
	       {
		  fprintf (output, "AND ");
		  wrCondition (pCond);
		  pCond = pCond->CoNextCondition;
	       }
	  }
	switch (pRe1->PrType)
	      {
		 case PtVisibility:
		    fprintf (output, "Visibility: ");
		    wrnbherit (RP);
		    break;
		 case PtFunction:
		    wrFonctPres (RP);
		    break;
		 case PtVertRef:
		    fprintf (output, "VertRef: ");
		    wrpos (pRe1->PrPosRule, FALSE);
		    break;
		 case PtHorizRef:
		    fprintf (output, "HorizRef: ");
		    wrpos (pRe1->PrPosRule, FALSE);
		    break;
		 case PtHeight:
		    fprintf (output, "Height: ");
		    wrdimens (pRe1->PrDimRule, TRUE);
		    break;
		 case PtWidth:
		    fprintf (output, "Width: ");
		    wrdimens (pRe1->PrDimRule, FALSE);
		    break;
		 case PtVertPos:
		    fprintf (output, "VertPos: ");
		    wrpos (pRe1->PrPosRule, TRUE);
		    break;
		 case PtHorizPos:
		    fprintf (output, "HorizPos: ");
		    wrpos (pRe1->PrPosRule, TRUE);
		    break;
		 case PtFont:
		    fprintf (output, "Font: ");
		    wrfontstyle (RP);
		    break;
		 case PtStyle:
		    fprintf (output, "Style: ");
		    wrfontstyle (RP);
		    break;
		 case PtWeight:
		    fprintf (output, "Weight: ");
		    wrfontstyle (RP);
		    break;
		 case PtUnderline:
		    fprintf (output, "Underline: ");
		    wrfontstyle (RP);
		    break;
		 case PtThickness:
		    fprintf (output, "Thickness: ");
		    wrfontstyle (RP);
		    break;
		 case PtSize:
		    fprintf (output, "Size: ");
		    wrsize (RP);
		    break;
		 case PtIndent:
		    fprintf (output, "Indent: ");
		    wrminind (RP);
		    break;
		 case PtBreak1:
		    fprintf (output, "NoBreak1: ");
		    wrminind (RP);
		    break;
		 case PtBreak2:
		    fprintf (output, "NoBreak2: ");
		    wrminind (RP);
		    break;
		 case PtLineSpacing:
		    fprintf (output, "LineSpacing: ");
		    wrminind (RP);
		    break;
		 case PtAdjust:
		    fprintf (output, "Adjust: ");
		    wrajust (RP);
		    break;
		 case PtJustify:
		    fprintf (output, "Justify: ");
		    wrjustif (RP);
		    break;
		 case PtHyphenate:
		    fprintf (output, "Hyphenate: ");
		    wrjustif (RP);
		    break;
		 case PtHorizOverflow:
		    fprintf (output, "HorizOverflow: ");
		    wrjustif (RP);
		    break;
		 case PtVertOverflow:
		    fprintf (output, "VertOverflow: ");
		    wrjustif (RP);
		    break;
		 case PtDepth:
		    fprintf (output, "Depth: ");
		    wrnbherit (RP);
		    break;
		 case PtLineStyle:
		    fprintf (output, "LineStyle: ");
		    wrfontstyle (RP);
		    break;
		 case PtLineWeight:
		    fprintf (output, "LineWeight: ");
		    wrminind (RP);
		    break;
		 case PtFillPattern:
		    fprintf (output, "FillPattern: ");
		    wrnbherit (RP);
		    break;
		 case PtBackground:
		    fprintf (output, "Background: ");
		    wrnbherit (RP);
		    break;
		 case PtForeground:
		    fprintf (output, "Foreground: ");
		    wrnbherit (RP);
		    break;
		 case PtPictInfo:
		    break;
	      }
	fprintf (output, "\n");	/* passe a la regle suivante */
	RP = pRe1->PrNextPRule;
     }
}


#ifdef STANDALONE
/*----------------------------------------------------------------------
   main                                                            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 main (int argc, char **argv)

#else  /* __STDC__ */
int                 main (argc, argv)
int                 argc;
char              **argv;

#endif /* __STDC__ */
{

   PRS = TtaGetMessageTable ("libdialogue", MSG_MAX);
   PRS = TtaGetMessageTable ("prsdialogue", PRS_MSG_MAX);
   /* recupere d'abord le nom du schema a lister */
   filename[0] = EOS;
   if (argc != 2)
      goto Usage;
   argv++;
   strcpy (filename, *argv);
   ReadEnv ();			/* on utilise le directory courant */
   /* indique que le chargement du schema de presentation doit commencer par */
   /* le chargement du schema de structure correspondant */
   GetSchStruct (&pSchemaStr);
   pSchemaStr->SsRootElem = 0;
   /* lit le schema de presentation et le schema de structure correspondant */
   pSchemaPrs = ReadPresentationSchema (filename, pSchemaStr);
   if (pSchemaPrs == NULL)
      TtaDisplaySimpleMessage (FATAL, PRS, UNKNOWN_FILE);
#else  /* !STANDALONE */
/*----------------------------------------------------------------------
   DebugPresent                                                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 DebugPresent (Document doc, PSchema gPres, char *output_file)

#else  /* __STDC__ */
int                 DebugPresent (doc, gPres, output_file)
Document            doc;
PSchema             gPres;
char               *output_file;

#endif /* __STDC__ */
{
   /* on ouvre d'abord le fichier de sortie */
   output = fopen (output_file, "w");
   if (output == NULL)
     output = stdout;
   ReadEnv ();			/* on utilise le directory courant */

   /* get the pointer to the presentation structure */
   pSchemaStr = (PtrSSchema) TtaGetDocumentSSchema (doc);
   pSchemaPrs = (PtrPSchema) gPres;
   if (pSchemaPrs == NULL)
      TtaDisplaySimpleMessage (FATAL, PRS, UNKNOWN_FILE);
#endif
   else
      /* les schemas ont ete lus correctement */
     {
	pSc1 = pSchemaPrs;
	fprintf (output, "\n");
	fprintf (output, "{ In each block, rules are listed in the same order as they\n");
	fprintf (output, "are processed by the editor. }\n");
	/* ecrit au terminal le nom du schema  de structure */
	fprintf (output, "\n");
	fprintf (output, "PRESENTATION ");
	wrnom (pSchemaStr->SsName);
	fprintf (output, ";\n");
	/* ecrit au terminal les noms des  vues */
	if (pSc1->PsNViews > 0 && pSc1->PsView[0][0] != SPACE)
	  {
	     fprintf (output, "\n");
	     fprintf (output, "VIEWS\n");
	     fprintf (output, "   ");
	     for (i = 1; i <= pSc1->PsNViews; i++)
	       {
		  wrnom (pSc1->PsView[i - 1]);
		  if (pSc1->PsExportView[i - 1])
		     fprintf (output, " THOT_EXPORT");
		  if (pSc1->PsPaginatedView[i - 1])
		     fprintf (output, " {with pages}");
		  if (i < pSc1->PsNViews)
		     fprintf (output, ", ");
		  else
		     fprintf (output, ";\n");
	       }
	  }
	/* ecrit au terminal les compteurs declares */
	if (pSc1->PsNCounters > 0)
	  {
	     fprintf (output, "\n");
	     fprintf (output, "COUNTERS\n");
	     for (i = 1; i <= pSc1->PsNCounters; i++)
		/* ecrit un compteur */
	       {
		  pCo1 = &pSc1->PsCounter[i - 1];
		  fprintf (output, "   Cpt");
		  wrnb (i);
		  fprintf (output, ": ");
		  for (j = 1; j <= pCo1->CnNItems; j++)
		    {
		       pCp1 = &pCo1->CnItem[j - 1];
		       switch (pCp1->CiCntrOp)
			     {
				case CntrSet:
				   fprintf (output, " SET ");
				   wrnb (pCp1->CiParamValue);
				   fprintf (output, " ON ");
				   break;
				case CntrAdd:
				   fprintf (output, " ADD ");
				   wrnb (pCp1->CiParamValue);
				   fprintf (output, " ON ");
				   break;
				case CntrRank:
				   fprintf (output, " RANK of ");
				   break;
				case CntrRLevel:
				   fprintf (output, " RLEVEL of ");
				   break;
			     }

		       wrnomregle (pCp1->CiElemType);
		       if (pCp1->CiViewNum > 0)
			 {
			    fprintf (output, "(");
			    wrnom (pSc1->PsView[pCp1->CiViewNum - 1]);
			    fprintf (output, ")");
			 }
		       if (pCp1->CiCondAttr > 0)
			 {
			    if (pCp1->CiCondAttrPresent)
			       printf (" with ");
			    else
			       printf (" without ");
			    wrnomattr (pCp1->CiCondAttr);
			 }
		    }
		  if (pCo1->CnItem[0].CiAscendLevel != 0)
		     fprintf (output, " %d ", pCo1->CnItem[0].CiAscendLevel);

		  if (pCo1->CnItem[0].CiInitAttr > 0)
		    {
		       fprintf (output, " INIT ");
		       wrnomattr (pCo1->CnItem[0].CiInitAttr);
		    }
		  if (pCo1->CnItem[0].CiReinitAttr > 0)
		    {
		       fprintf (output, " REINIT ");
		       wrnomattr (pCo1->CnItem[0].CiReinitAttr);
		    }
		  fprintf (output, ";\n");
		  /* ecrit les noms des boites qui utilisent le compteur */
		  if (pCo1->CnNPresBoxes > 0)
		    {
		       fprintf (output, "       { Value used in ");
		       for (j = 1; j <= pCo1->CnNPresBoxes; j++)
			 {
			    wrnomboite (pCo1->CnPresBox[j - 1]);
			    if (j < pCo1->CnNPresBoxes)
			       fprintf (output, ", ");
			    else
			       fprintf (output, ". }\n");
			 }
		    }
		  if (pCo1->CnNCreatedBoxes > 0)
		    {
		       fprintf (output, "       { Controls creation of ");
		       for (j = 1; j <= pCo1->CnNCreatedBoxes; j++)
			 {
			    wrnomboite (pCo1->CnCreatedBox[j - 1]);
			    if (j < pCo1->CnNCreatedBoxes)
			       fprintf (output, ", ");
			    else
			       fprintf (output, ". }\n");
			 }
		    }
		  if (pCo1->CnNCreators > 0)
		    {
		       fprintf (output, "       { Controls creation by ");
		       for (j = 1; j <= pCo1->CnNCreators; j++)
			 {
			    if (pCo1->CnPresBoxCreator[j - 1])
			       wrnomboite (pCo1->CnCreator[j - 1]);
			    else
			       wrnomregle (pCo1->CnCreator[j - 1]);
			    if (j < pCo1->CnNCreators)
			       fprintf (output, ", ");
			    else
			       fprintf (output, ". ");
			 }
		       fprintf (output, "}\n");
		    }
		  if (pCo1->CnPageFooter)
		     fprintf (output, "       { Page footer }\n");
	       }
	  }
	/* ecrit au terminal les constantes */
	if (pSc1->PsNConstants > 0)
	  {
	     fprintf (output, "\n");
	     fprintf (output, "CONST\n");
	     for (i = 1; i <= pSc1->PsNConstants; i++)
	       {
		  pPr1 = &pSc1->PsConstant[i - 1];
		  /* donne un nom a chaque constante */
		  fprintf (output, "   Cste");
		  wrnb (i);
		  fprintf (output, "= ");
		  /* ecrit le type de la constante */
		  switch (pPr1->PdType)
			{
			   case CharString:
			      fprintf (output, "Text ");
			      putchar (pPr1->PdAlphabet);
			      break;
			   case GraphicElem:
			      fprintf (output, "Graphics ");
			      break;
			   case Symbol:
			      fprintf (output, "Symbol ");
			      break;
			   case Picture:
			      fprintf (output, "Picture ");
			      /* ecrit la valeur de la constante */
			      break;
			   default:
			      break;
			}

		  fprintf (output, " \'");
		  j = 1;
		  while (pPr1->PdString[j - 1] != EOS)
		    {
		       if (pPr1->PdString[j - 1] < SPACE)
			  fprintf (output, "\\%3d", pPr1->PdString[j - 1]);
		       else
			  putchar (pPr1->PdString[j - 1]);
		       j++;
		    }
		  fprintf (output, "\';\n");
	       }
	  }
	/* ecrit au terminal les variables */
	if (pSc1->PsNVariables > 0)
	  {
	     fprintf (output, "\n");
	     fprintf (output, "VAR\n");
	     for (i = 1; i <= pSc1->PsNVariables; i++)
	       {
		  pPres1 = &pSc1->PsVariable[i - 1];
		  /* donne un nom a chaque variable */
		  fprintf (output, "   Var");
		  wrnb (i);
		  fprintf (output, ":");
		  for (j = 1; j <= pPres1->PvNItems; j++)
		    {
		       pVa1 = &pPres1->PvItem[j - 1];
		       switch (pVa1->ViType)
			     {
				case VarText:
				   fprintf (output, " Cste");
				   wrnb (pVa1->ViConstant);
				   break;
				case VarCounter:
				   fprintf (output, " VALUE(");
				   if (pVa1->ViCounterVal == CntMaxVal)
				      fprintf (output, "MaxRangeVal ");
				   else if (pVa1->ViCounterVal == CntMinVal)
				      fprintf (output, "MinRangeVal ");
				   fprintf (output, "Cpt");
				   wrnb (pVa1->ViCounter);
				   WriteCounterStyle (pVa1->ViStyle);
				   break;
				case VarAttrValue:
				   fprintf (output, " VALUE(");
				   wrnomattr (pVa1->ViAttr);
				   WriteCounterStyle (pVa1->ViStyle);
				   break;
				case VarDate:
				   fprintf (output, " VarDate");
				   break;
				case VarFDate:
				   fprintf (output, " VarFDate");
				   break;
				case VarDirName:
				   fprintf (output, " DirName");
				   break;
				case VarDocName:
				   fprintf (output, " DocName");
				   break;
				case VarElemName:
				   fprintf (output, " ElemName");
				   break;
				case VarAttrName:
				   fprintf (output, " VarAttrName");
				   break;
				case VarPageNumber:
				   fprintf (output, " VALUE(PageNumber(");
				   wrnom (pSc1->PsView[pVa1->ViView - 1]);
				   fprintf (output, ")");
				   WriteCounterStyle (pVa1->ViStyle);
				   break;
			     }

		    }
		  fprintf (output, ";\n");
	       }
	  }
	/* ecrit au terminal les regles de presentation par defaut */
	fprintf (output, "\n");
	fprintf (output, "DEFAULT\n");
	fprintf (output, "\n");
	fprintf (output, "   BEGIN\n");
	wrsuiteregles (pSc1->PsFirstDefaultPRule);
	fprintf (output, "   END;\n");
	fprintf (output, "\n");
	if (pSc1->PsNPresentBoxes > 0)
	   /* ecrit au terminal les boites de presentation et de mise en page */
	  {
	     fprintf (output, "\n");
	     fprintf (output, "BOXES\n");
	     fprintf (output, "\n");
	     for (i = 1; i <= pSc1->PsNPresentBoxes; i++)
	       {
		  pBo1 = &pSc1->PsPresentBox[i - 1];
		  wrnom (pBo1->PbName);
		  fprintf (output, ":	{%3d }\n", i);
		  if (pBo1->PbFirstPRule != NULL)
		     fprintf (output, "   BEGIN\n");
		  wrsuiteregles (pBo1->PbFirstPRule);
		  fprintf (output, "   PageBreak: ");
		  if (pBo1->PbAcceptPageBreak)
		     fprintf (output, "Yes;\n");
		  else
		     fprintf (output, "No;\n");
		  fprintf (output, "   LineBreak: ");
		  if (pBo1->PbAcceptLineBreak)
		     fprintf (output, "Yes;\n");
		  else
		     fprintf (output, "No;\n");
		  fprintf (output, "   Gather: ");
		  if (pBo1->PbBuildAll)
		     fprintf (output, "Yes;\n");
		  else
		     fprintf (output, "No;\n");
		  /* ecrit le contenu de la boite */
		  if (pBo1->PbContent != FreeContent)
		    {
		       fprintf (output, "   Content: ");
		       switch (pBo1->PbContent)
			     {
				case ContVariable:
				   fprintf (output, "Var");
				   wrnb (pBo1->PbContVariable);
				   break;
				case ContConst:
				   fprintf (output, "Cste");
				   wrnb (pBo1->PbContConstant);
				   break;
				case ContElement:
				   wrnomregle (pBo1->PbContElem);
				   fprintf (output, " {referenced by ");
				   wrnomregle (pBo1->PbContRefElem);
				   fprintf (output, "}");
				   break;
				case FreeContent:

				   break;
			     }

		       fprintf (output, ";\n");
		    }
		  if (pBo1->PbPageBox)
		    {
		       fprintf (output, "   { Page box, footer=%5d pt, ", pBo1->PbFooterHeight);
		       if (pBo1->PbPageCounter == 0)
			  fprintf (output, "no page number }\n");
		       else
			 {
			    fprintf (output, "page number=Cpt");
			    wrnb (pBo1->PbPageCounter);
			    fprintf (output, " }\n");
			 }
		    }
		  if (pBo1->PbPageFooter)
		     fprintf (output, "   { Bottom of page }\n");
		  if (pBo1->PbPageHeader)
		     fprintf (output, "   { Top of page }\n");
		  if (pBo1->PbFirstPRule != NULL)
		     fprintf (output, "   END;\n");
		  fprintf (output, "\n");
	       }
	  }
	/* ecrit au terminal les regles de presentation des elements structure's */
	fprintf (output, "\nRULES\n\n");
	for (El = 1; El <= pSchemaStr->SsNRules; El++)
	  {
	     if (pSchemaStr->SsRule[El - 1].SrConstruct == CsPairedElement)
		if (pSchemaStr->SsRule[El - 1].SrFirstOfPair)
		   fprintf (output, "First ");
		else
		   fprintf (output, "Second ");
	     wrnomregle (El);
	     fprintf (output, ":\n");
	     if (pSc1->PsElemPRule[El - 1] != NULL)
		fprintf (output, "   BEGIN\n");
	     wrsuiteregles (pSc1->PsElemPRule[El - 1]);
	     fprintf (output, "   PageBreak: ");
	     if (pSc1->PsAcceptPageBreak[El - 1])
		fprintf (output, "Yes;\n");
	     else
		fprintf (output, "No;\n");
	     fprintf (output, "   LineBreak: ");
	     if (pSc1->PsAcceptLineBreak[El - 1])
		fprintf (output, "Yes;\n");
	     else
		fprintf (output, "No;\n");
	     fprintf (output, "   Gather: ");
	     if (pSc1->PsBuildAll[El - 1])
		fprintf (output, "Yes;\n");
	     else
		fprintf (output, "No;\n");
	     if (pSc1->PsInPageHeaderOrFooter[El - 1])
		fprintf (output, "   { displayed in top or bottom of page }\n");
	     if (pSc1->PsAssocPaginated[El - 1])
		fprintf (output, "   { with pages }\n");
	     if (pSc1->PsElemPRule[El - 1] != NULL)
		fprintf (output, "   END;\n");
	     fprintf (output, "\n");
	  }
	/* ecrit au terminal les regles de presentation des attributs */
	if (pSchemaStr->SsNAttributes > 0)
	  {
	     fprintf (output, "\n");
	     fprintf (output, "ATTRIBUTES\n");
	     fprintf (output, "\n");
	     for (GetAttributeOfElement = 1; GetAttributeOfElement <= pSchemaStr->SsNAttributes; GetAttributeOfElement++)
	       {
		  pAt1 = &pSchemaStr->SsAttribute[GetAttributeOfElement - 1];
		  pRP1 = pSc1->PsAttrPRule[GetAttributeOfElement - 1];
		  if (pRP1 != NULL)
		    {
		       /* si cette attribut a une presentation */
		       switch (pAt1->AttrType)
			     {
				case AtNumAttr:
				   for (i = 1; i <= pRP1->ApNCases; i++)
				     {
					pCa1 = &pRP1->ApCase[i - 1];
					wrnom (pAt1->AttrName);
					if (pRP1->ApElemType > 0)
					  {
					     fprintf (output, "(");
					     wrnomregle (pRP1->ApElemType);
					     fprintf (output, ")");
					  }
					if (pCa1->CaLowerBound == pCa1->CaUpperBound)
					  {
					     fprintf (output, "=");
					     wrnb (pCa1->CaLowerBound);
					  }
					else if (pCa1->CaLowerBound != -MAX_INT_ATTR_VAL - 1
						 && pCa1->CaUpperBound != MAX_INT_ATTR_VAL + 1)
					  {
					     fprintf (output, " IN [");
					     wrnb (pCa1->CaLowerBound);
					     fprintf (output, "..");
					     wrnb (pCa1->CaUpperBound);
					     fprintf (output, "] ");
					  }
					else if (pCa1->CaLowerBound != -MAX_INT_ATTR_VAL - 1
						 || pCa1->CaUpperBound != MAX_INT_ATTR_VAL + 1)
					   if (pCa1->CaLowerBound != -MAX_INT_ATTR_VAL - 1)
					     {
						fprintf (output, ">");
						wrnb (pCa1->CaLowerBound - 1);
					     }
					   else if (pCa1->CaUpperBound != MAX_INT_ATTR_VAL + 1)
					     {
						fprintf (output, "<");
						wrnb (pCa1->CaUpperBound + 1);
					     }
					fprintf (output, ":\n");
					if (pCa1->CaFirstPRule == NULL)
					   fprintf (output, "   BEGIN END;\n");
					else
					  {
					     if (pCa1->CaFirstPRule->PrNextPRule != NULL)
						fprintf (output, "   BEGIN\n");
					     wrsuiteregles (pCa1->CaFirstPRule);
					     if (pCa1->CaFirstPRule->PrNextPRule != NULL)
						fprintf (output, "   END;\n");
					  }
					fprintf (output, "\n");
				     }
				   break;
				case AtTextAttr:	/* !!!!!!!!!!!! */
				   while (pRP1 != NULL)
				     {
					if (pRP1->ApTextFirstPRule != NULL)
					  {
					     wrnom (pAt1->AttrName);
					     if (pRP1->ApElemType > 0)
					       {
						  fprintf (output, "(");
						  wrnomregle (pRP1->ApElemType);
						  fprintf (output, ")");
					       }
					     if (pRP1->ApString[0] != EOS)
					       {
						  fprintf (output, "=\'");
						  wrnom (pRP1->ApString);
						  fprintf (output, "\'");
					       }
					     fprintf (output, ":\n");
					     if (pRP1->ApTextFirstPRule->PrNextPRule != NULL)
						fprintf (output, "   BEGIN\n");
					     wrsuiteregles (pRP1->ApTextFirstPRule);
					     if (pRP1->ApTextFirstPRule->PrNextPRule != NULL)
						fprintf (output, "   END;\n");
					     fprintf (output, "\n");
					  }
					pRP1 = pRP1->ApNextAttrPres;
				     }
				   break;
				case AtReferenceAttr:
				   if (pRP1->ApRefFirstPRule != NULL)
				     {
					wrnom (pAt1->AttrName);
					if (pRP1->ApElemType > 0)
					  {
					     fprintf (output, "(");
					     wrnomregle (pRP1->ApElemType);
					     fprintf (output, ")");
					  }
					fprintf (output, ":\n");
					if (pRP1->ApRefFirstPRule->PrNextPRule != NULL)
					   fprintf (output, "   BEGIN\n");
					wrsuiteregles (pRP1->ApRefFirstPRule);
					if (pRP1->ApRefFirstPRule->PrNextPRule != NULL)
					   fprintf (output, "   END;\n");
					fprintf (output, "\n");
				     }
				   break;
				case AtEnumAttr:
				   for (Val = 0; Val <= pAt1->AttrNEnumValues; Val++)
				      if (pRP1->ApEnumFirstPRule[Val] != NULL)
					{
					   wrnom (pAt1->AttrName);
					   if (pRP1->ApElemType > 0)
					     {
						fprintf (output, "(");
						wrnomregle (pRP1->ApElemType);
						fprintf (output, ")");
					     }
					   if (Val > 0)
					     {
						fprintf (output, "=");
						wrnom (pAt1->AttrEnumValue[Val - 1]);
					     }
					   fprintf (output, ":\n");
					   if (pRP1->ApEnumFirstPRule[Val]->PrNextPRule
					       != NULL)
					      fprintf (output, "   BEGIN\n");
					   wrsuiteregles (pRP1->ApEnumFirstPRule[Val]);
					   if (pRP1->ApEnumFirstPRule[Val]->PrNextPRule
					       != NULL)
					      fprintf (output, "   END;\n");
					   fprintf (output, "\n");
					}
				   break;
				default:
				   break;
			     }

		    }
	       }
	  }
	/* ecrit au terminal les regles de transmission de valeurs aux */
	/* attributs des documents inclus */
	Transm = FALSE;
	/* ecrit d'abord les regles de transmission des compteurs */
	for (i = 1; i <= pSc1->PsNCounters; i++)
	  {
	     pCo1 = &pSc1->PsCounter[i - 1];
	     for (j = 1; j <= pCo1->CnNTransmAttrs; j++)
	       {
		  if (!Transm)
		    {
		       fprintf (output, "\nTRANSMIT\n");
		       Transm = TRUE;
		    }
		  fprintf (output, "   Cpt");
		  wrnb (i);
		  fprintf (output, " To ");
		  wrnom (pCo1->CnTransmAttr[j - 1]);
		  fprintf (output, "(");
		  wrnomregle (pCo1->CnTransmSSchemaAttr[j - 1]);
		  fprintf (output, ");\n");
	       }
	  }
	/* ecrit les regles de transmission de valeurs d'elements */
	if (pSc1->PsNTransmElems > 0)
	   for (i = 1; i <= pSchemaStr->SsNRules; i++)
	      if (pSc1->PsElemTransmit[i - 1] > 0)
		{
		   if (!Transm)
		     {
			fprintf (output, "\nTRANSMIT\n");
			Transm = TRUE;
		     }
		   fprintf (output, "   ");
		   wrnomregle (i);
		   fprintf (output, " To ");
		   wrnom (pSc1->PsTransmElem[pSc1->PsElemTransmit[i - 1] - 1].TeTargetAttr);
		   fprintf (output, "(");
		   wrnomregle (pSc1->PsTransmElem[pSc1->PsElemTransmit[i - 1] - 1].TeTargetDoc);
		   fprintf (output, ");\n");
		}

	fprintf (output, "END\n");
     }
#ifdef STANDALONE
   exit (0);

 Usage:
   fprintf (stderr, "usage : %s <input-file>\n", argv[0]);
   exit (1);
#else
   fclose (output);
   return (0);
#endif
}
