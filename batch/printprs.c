/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*----------------------------------------------------------------------
   Ce programme effectue le chargement d'un schema de presentation et
   du schema de structure associe' et liste le contenu du schema de
   presentation.        
  ----------------------------------------------------------------------*/

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "prsmsg.h"
#include "registry.h"
#include "fileaccess.h"
#include "message.h"

#define THOT_EXPORT
#include "platform_tv.h"

#include "readprs_f.h"
#include "memory_f.h"
#include "fileaccess_f.h"

static PtrSSchema   pSchemaStr;
static PtrPSchema   pSchemaPrs;
static char         filename[1000];
static int          j, El, Attr, Val;
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
ThotBool             Transm;

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
	printf ("-");
	nb = -nb;
     }
   if (nb < 10)
      printf ("%1d", nb);
   else if (nb < 100)
      printf ("%2d", nb);
   else if (nb < 1000)
      printf ("%3d", nb);
   else if (nb < 10000)
      printf ("%4d", nb);
   else if (nb < 100000)
      printf ("%5d", nb);
   else
      printf ("999999999");
}


/*----------------------------------------------------------------------
   wrlevel ecrit au terminal le niveau relatif n.                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrlevel (Level n)

#else  /* __STDC__ */
static void         wrlevel (n)
Level               n;

#endif /* __STDC__ */

{
   switch (n)
	 {
	    case RlEnclosing:
	       printf ("Enclosing");
	       break;
	    case RlSameLevel:
	       ;
	       break;
	    case RlEnclosed:
	       printf ("Enclosed");
	       break;
	    case RlPrevious:
	       printf ("Previous");
	       break;
	    case RlNext:
	       printf ("Next");
	       break;
	    case RlSelf:
	       printf ("*");
	       break;
	    case RlContainsRef:
	       printf ("Refering");
	       break;
	    case RlRoot:
	       printf ("Root");
	       break;
	    case RlReferred:
	       printf ("Referred");
	       break;
	    case RlCreator:
	       printf ("Creator");
	       break;
	 }
}

/*----------------------------------------------------------------------
   wrdistunit ecrit le nom d'une unite' de distance.               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrdistunit (TypeUnit u)

#else  /* __STDC__ */
static void         wrdistunit (u)
TypeUnit            u;

#endif /* __STDC__ */

{
   switch (u)
	 {
	    case UnRelative:
	       ;
	       break;
	    case UnXHeight:
	       printf (" ex");
	       break;
	    case UnPoint:
	       printf (" pt");
	       break;
	    case UnPixel:
	       printf (" px");
	       break;
	    case UnPercent:
	       printf (" %%");
	       break;
	    case UnAuto:
	       printf (" auto");
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
	       printf ("Top");
	       break;
	    case Bottom:
	       printf ("Bottom");
	       break;
	    case Left:
	       printf ("Left");
	       break;
	    case Right:
	       printf ("Right");
	       break;
	    case HorizRef:
	       printf ("HRef");
	       break;
	    case VertRef:
	       printf ("VRef");
	       break;
	    case HorizMiddle:
	       printf ("HMiddle");
	       break;
	    case VertMiddle:
	       printf ("VMiddle");
	       break;
	    case NoEdge:
	       printf ("NULL");
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
   while (n[i - 1] != '\0')
     {
	if (n[i - 1] < ' ' || n[i - 1] > '~')
	   /* caractere non imprimable, ecrit son code */
	  {
	     printf ("\\%o", (unsigned char) n[i - 1]);
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
	       printf ("Enclosing");
	       break;
	    case InheritPrevious:
	       printf ("Previous");
	       break;
	    case InheritChild:
	       printf ("Enclosed");
	       break;
	    case InheritCreator:
	       printf ("Creator");
	       break;
	    case InheritGrandFather:
	       printf ("GrandFather");
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
   if (pR->PrPresMode == PresInherit)
     {
	wrModeHerit (pR->PrInheritMode);
	if (pR->PrInhPercent)
	  {
	  printf (" * ");
	  if (pR->PrInhAttr)
	     wrnomattr (pR->PrInhDelta);
	  else
	     wrnb (pR->PrInhDelta);
	  printf (" %%");
	  }
	else
	   if (pR->PrInhDelta == 0)
	      printf (" =");
	   else
	      {
	      if (pR->PrInhDelta > 0)
		 printf ("+");
	      if (pR->PrInhAttr)
		 wrnomattr (pR->PrInhDelta);
	      else
		 wrnb (pR->PrInhDelta);
	      wrdistunit (pR->PrInhUnit);
	      }
	if (pR->PrInhMinOrMax > 0)
	  {
	     if (pR->PrInhDelta >= 0)
		printf (" max ");
	     else
		printf (" min ");
	     if (pR->PrMinMaxAttr)
		wrnomattr (pR->PrInhMinOrMax);
	     else
		wrnb (pR->PrInhMinOrMax);
	  }
     }
   else if (pR->PrPresMode == PresImmediate)
     {
	if (pR->PrMinAttr)
	   wrnomattr (pR->PrMinValue);
	else
	   wrnb (pR->PrMinValue);
	wrdistunit (pR->PrMinUnit);
     }
   else
      printf ("??????");
   printf (";");
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
   if (pR->PrPresMode == PresInherit)
     {
	wrModeHerit (pR->PrInheritMode);
	if (pR->PrInhDelta == 0 && !pR->PrInhPercent)
	   printf (" =");
	else
	   printf ("??????");
     }
   else if (pR->PrPresMode == PresImmediate)
     {
      if (pR->PrType == PtFont)
	 switch (pR->PrChrValue)
	       {
		  case 'C':
		     printf ("Courrier");
		     break;
		  case 'H':
		     printf ("Helvetica");
		     break;
		  case 'T':
		     printf ("Times");
		     break;
		  case 'c':
		     printf ("courrier");
		     break;
		  case 'h':
		     printf ("helvetica");
		     break;
		  case 't':
		     printf ("times");
		     break;
		  default:
		     putchar (pR->PrChrValue);
		     break;
	       }
      else if (pR->PrType == PtStyle)
	 switch (pR->PrChrValue)
	       {
		  case 'I':
		     printf ("Italics");
		     break;
		  case 'R':
		     printf ("Roman");
		     break;
		  case 'O':
		     printf ("Oblique");
		     break;
		  default:
		     putchar (pR->PrChrValue);
		     break;
	       }
      else if (pR->PrType == PtWeight)
	 switch (pR->PrChrValue)
	       {
		  case 'B':
		     printf ("Bold");
		     break;
		  case 'N':
		     printf ("Normal");
		     break;
		  default:
		     putchar (pR->PrChrValue);
		     break;
	       }
      else if (pR->PrType == PtUnderline)
	 switch (pR->PrChrValue)
	       {
		  case 'N':
		     printf ("NoUnderline");
		     break;
		  case 'U':
		     printf ("Underlined");
		     break;
		  case 'O':
		     printf ("Overlined");
		     break;
		  case 'C':
		     printf ("CrossedOut");
		     break;
		  default:
		     putchar (pR->PrChrValue);
		     break;
	       }
      else if (pR->PrType == PtThickness)
	 switch (pR->PrChrValue)
	       {
		  case 'T':
		     printf ("Thick");
		     break;
		  case 'N':
		     printf ("Thin");
		     break;
		  default:
		     putchar (pR->PrChrValue);
		     break;
	       }
      else if (pR->PrType == PtLineStyle)
	 switch (pR->PrChrValue)
	       {
		  case 'S':
		     printf ("Solid");
		     break;
		  case '-':
		     printf ("Dashed");
		     break;
		  case '.':
		     printf ("Dotted");
		     break;
		  default:
		     putchar (pR->PrChrValue);
		     break;
	       }
      else if (pR->PrType == PtBorderTopStyle ||
	       pR->PrType == PtBorderRightStyle ||
	       pR->PrType == PtBorderBottomStyle ||
	       pR->PrType == PtBorderLeftStyle)
	 switch (pR->PrChrValue)
	       {
		  case '0':
		     printf ("None");
		     break;
		  case 'H':
		     printf ("Hidden");
		     break;
		  case '.':
		     printf ("Dotted");
		     break;
		  case '-':
		     printf ("Dashed");
		     break;
		  case 'S':
		     printf ("Solid");
		     break;
		  case 'D':
		     printf ("Double");
		     break;
		  case 'G':
		     printf ("Groove");
		     break;
		  case 'R':
		     printf ("Ridge");
		     break;
		  case 'I':
		     printf ("Inset");
		     break;
		  case 'O':
		     printf ("Outset");
		     break;
		  default:
		     putchar (pR->PrChrValue);
		     break;
	       }
      else
	 putchar (pR->PrChrValue);
    }
   else
      printf ("??????");
   printf (";");
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
   if (pR->PrPresMode == PresInherit)
     if (pR->PrInhPercent)
	printf ("??????");
     else
        {
	wrModeHerit (pR->PrInheritMode);
	if (pR->PrInhDelta == 0)
	   printf (" =");
	else
	  {
	     if (pR->PrInhDelta > 0)
		printf ("+");
	     if (pR->PrInhAttr)
		wrnomattr (pR->PrInhDelta);
	     else
		wrnb (pR->PrInhDelta);
	  }
	if (pR->PrInhMinOrMax > 0)
	  {
	     if (pR->PrInhDelta >= 0)
		printf (" max ");
	     else
		printf (" min ");
	     if (pR->PrMinMaxAttr)
		wrnomattr (pR->PrInhMinOrMax);
	     else
		wrnb (pR->PrInhMinOrMax);
	  }
        }
   else if (pR->PrPresMode == PresImmediate)
      if (pR->PrAttrValue)
	 wrnomattr (pR->PrIntValue);
      else
	 wrnb (pR->PrIntValue);
   else
      printf ("??????");
   printf (";");
}


/*----------------------------------------------------------------------
   wrminind ecrit au terminal une regle du type Indent, Size, Margin*, etc.  
   La regle a ecrire est pointee par pR.                                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrminind (PtrPRule pR)

#else  /* __STDC__ */
static void         wrminind (pR)
PtrPRule            pR;

#endif /* __STDC__ */

{
   if (pR->PrPresMode == PresInherit)
      wrnbherit (pR);
   else
     {
	if (pR->PrPresMode == PresImmediate)
	  {
	     if (pR->PrMinUnit == UnAuto)
	        printf ("auto");
	     else
	        {
	        if (pR->PrMinAttr)
		   wrnomattr (pR->PrMinValue);
	        else
		   wrnb (pR->PrMinValue);
	        wrdistunit (pR->PrMinUnit);
	        }
	  }
	else
	   printf ("??????");
	printf (";");
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
ThotBool             Def;

#endif /* __STDC__ */

{
   if (Def)
      if (pos.PoPosDef == NoEdge)
	 printf (" NULL");
      else
	{
	   wrrepere (pos.PoPosDef);
	   printf (" = ");
	}
   if (!Def || pos.PoPosDef != NoEdge)
     {
	wrlevel (pos.PoRelation);
	if (pos.PoNotRel)
	   printf (" NOT");
	printf (" ");
	if (pos.PoRefKind == RkElType)
	   wrnomregle (pos.PoRefIdent);
	else if (pos.PoRefKind == RkPresBox)
	   wrnomboite (pos.PoRefIdent);
	else if (pos.PoRefKind == RkAttr)
	   wrnomattr (pos.PoRefIdent);
	printf (". ");
	wrrepere (pos.PoPosRef);
	if (pos.PoDistance != 0)
	  {
	     if (pos.PoDistance > 0)
		printf ("+");
	     else
		printf ("-");
	     if (pos.PoDistAttr)
		wrnomattr (abs (pos.PoDistance));
	     else
		wrnb (abs (pos.PoDistance));
	     wrdistunit (pos.PoDistUnit);
	  }
	if (pos.PoUserSpecified)
	   printf (" UserSpecified");
     }
   printf (";");
}


/*----------------------------------------------------------------------
   wrdimens ecrit au terminal la dimension Dim.                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrdimens (DimensionRule Dim, ThotBool Hauteur)

#else  /* __STDC__ */
static void         wrdimens (Dim, Hauteur)
DimensionRule       Dim;
ThotBool             Hauteur;

#endif /* __STDC__ */

{
   if (Dim.DrPosition)
      wrpos (Dim.DrPosRule, True);
   else
     {
	if (Dim.DrAbsolute)
	  {
	     if (Dim.DrAttr)
		wrnomattr (Dim.DrValue);
	     else
		wrnb (Dim.DrValue);
	     if (Dim.DrValue != 0)
		wrdistunit (Dim.DrUnit);
	     if (Dim.DrUserSpecified)
		printf (" UserSpecified");
	     if (Dim.DrMin)
		printf (" Min");
	  }
	else
	  {
	     wrlevel (Dim.DrRelation);
	     printf (" ");
	     if (Dim.DrNotRelat)
		printf ("not ");
	     if (Dim.DrRefKind == RkElType)
		wrnomregle (Dim.DrRefIdent);
	     else if (Dim.DrRefKind == RkPresBox)
		wrnomboite (Dim.DrRefIdent);
	     else if (Dim.DrRefKind == RkAttr)
		wrnomattr (Dim.DrRefIdent);
	     printf (". ");
	     if ((Dim.DrSameDimens && Hauteur) || (!Dim.DrSameDimens && !Hauteur))
		printf ("Height");
	     else
		printf ("Width");
	     if (Dim.DrUnit == UnPercent)
	       {
		  printf ("*");
		  if (Dim.DrValue < 0)
		     printf ("-");
		  if (Dim.DrAttr)
		     wrnomattr (abs (Dim.DrValue));
		  else
		     wrnb (abs (Dim.DrValue));
		  printf ("%%");
	       }
	     else
	       {
		  if (Dim.DrValue < 0)
		     printf ("-");
		  if (Dim.DrValue > 0)
		     printf ("+");
		  if (Dim.DrValue != 0)
		    {
		       if (Dim.DrAttr)
			  wrnomattr (abs (Dim.DrValue));
		       else
			  wrnb (abs (Dim.DrValue));
		       wrdistunit (Dim.DrUnit);
		    }
	       }
	     if (Dim.DrMin)
		printf (" Min");
	  }
	printf (";");
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
      printf ("NOT ");
   switch (pCond->CoCondition)
	 {
	    case PcFirst:
	       printf ("First ");
	       break;
	    case PcLast:
	       printf ("Last ");
	       break;
	    case PcReferred:
	       printf ("Referred ");
	       break;
	    case PcFirstRef:
	       printf ("FirstRef ");
	       break;
	    case PcLastRef:
	       printf ("LastRef ");
	       break;
	    case PcExternalRef:
	       printf ("ExternalRef ");
	       break;
	    case PcInternalRef:
	       printf ("InternalRef ");
	       break;
	    case PcCopyRef:
	       printf ("PcCopyRef ");
	       break;
	    case PcAnyAttributes:
	       printf ("AnyAttributes ");
	       break;
	    case PcFirstAttr:
	       printf ("FirstAttr ");
	       break;
	    case PcLastAttr:
	       printf ("LastAttr ");
	       break;
	    case PcUserPage:
	       printf ("UserPage ");
	       break;
	    case PcStartPage:
	       printf ("StartPage ");
	       break;
	    case PcComputedPage:
	       printf ("ComputedPage ");
	       break;
	    case PcEmpty:
	       printf ("PcEmpty ");
	       break;
	    case PcEven:
	       printf ("Even");
	       break;
	    case PcOdd:
	       printf ("Odd");
	       break;
	    case PcOne:
	       printf ("One");
	       break;
	    case PcWithin:
	       if (pCond->CoImmediate)
		  printf ("Immediately ");
	       printf ("Within ");
	       if (pCond->CoAncestorRel == CondGreater &&
		   pCond->CoRelation != 0)
		  printf (">");
	       else if (pCond->CoAncestorRel == CondLess)
		  printf ("<");
	       if (pCond->CoRelation > 0 ||
		   pCond->CoAncestorRel == CondEquals)
		 {
		    wrnb (pCond->CoRelation);
		    printf (" ");
		 }
	       if (pCond->CoTypeAncestor == 0)
		 {
		    wrnom (pCond->CoAncestorName);
		    printf ("(");
		    wrnom (pCond->CoSSchemaName);
		    printf (")");
		 }
	       else
		  wrnomregle (pCond->CoTypeAncestor);
	       printf (" ");
	       break;
	    case PcInterval:
	       if (pCond->CoCounter > 0)
		 {
		    printf ("(");
		    if (pCond->CoValCounter == CntMaxVal)
		       printf ("MaxRangeVal ");
		    else if (pCond->CoValCounter == CntMinVal)
		       printf ("MinRangeVal ");
		    printf ("Cpt");
		    wrnb (pCond->CoCounter);
		    printf (") ");
		 }
	       break;
	    case PcElemType:
	       wrnomregle (pCond->CoTypeElAttr);
	       printf (" ");
	       break;
	    case PcAttribute:
	       wrnomattr (pCond->CoTypeElAttr);
	       printf (" ");
	       break;
	    default:
	       break;
	 }
   if (pCond->CoCondition == PcEven || pCond->CoCondition == PcOdd ||
       pCond->CoCondition == PcOne)
      if (pCond->CoCounter > 0)
	{
	   printf ("(Cpt");
	   wrnb (pCond->CoCounter);
	   printf (") ");
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

   switch (pR->PrPresFunction)
	 {
	    case FnLine:
	       printf ("Line");
	       break;
	    case FnNoLine:
	       printf ("NoLine");
	       break;
	    case FnCreateBefore:
	       printf ("CreateBefore");
	       break;
	    case FnCreateWith:
	       printf ("CreateWith");
	       break;
	    case FnCreateFirst:
	       printf ("CreateFirst");
	       break;
	    case FnCreateLast:
	       printf ("CreateLast");
	       break;
	    case FnCreateAfter:
	       printf ("CreateAfter");
	       break;
	    case FnCreateEnclosing:
	       printf ("CreateEnclosing");
	       break;
	    case FnPage:
	       printf ("Page");
	       break;
	    case FnColumn:
	       printf ("Column");
	       break;
	    case FnSubColumn:
	       printf ("Included Column");
	       break;
	    case FnCopy:
	       printf ("Copy");
	       break;
	    case FnContentRef:
	       printf ("Content: Cste");
	       wrnb (pR->PrPresBox[0]);
	       break;
	    case FnShowBox:
	       printf ("ShowBox");
	       break;
	    case FnBackgroundPicture:
	       printf ("BackgroundPicture: Cste");
	       wrnb (pR->PrPresBox[0]);
	       break;
	    case FnPictureMode:
	       printf ("PictureMode: ");
	       switch (pR->PrPresBox[0])
		 {
		 case RealSize:
		    printf ("NormalSize");
		    break;
		 case ReScale:
		    printf ("Scale");
		    break;
		 case FillFrame:
		    printf ("RepeatXY");
		    break;
		 case XRepeat:
		    printf ("RepeatX");
		    break;
		 case YRepeat:
		    printf ("RepeatY");
		    break;
		 default:
		    printf ("??????");
		    break;
		 }
	       break;
	    case FnNotInLine:
	       printf ("InLine: No");
	       break;
	   case FnAny:
	       printf ("??????");
	       break;		    
	 }
   if (pR->PrPresFunction != FnLine &&
            pR->PrPresFunction != FnContentRef &&
            pR->PrPresFunction != FnShowBox &&
            pR->PrPresFunction != FnBackgroundPicture &&
            pR->PrPresFunction != FnPictureMode &&
	    pR->PrPresFunction != FnNoLine)
     {
	printf ("(");
	if (pR->PrNPresBoxes == 0)
	  {
	     wrnom (pR->PrPresBoxName);
	     if (pR->PrExternal || !pR->PrElement)
		printf ("(****)");
	  }
	else
	   for (i = 1; i <= pR->PrNPresBoxes; i++)
	     {
		if (i > 1)
		   printf (", ");
		if (pR->PrElement)
		   wrnomregle (pR->PrPresBox[i - 1]);
		else
		   wrnomboite (pR->PrPresBox[i - 1]);
	     }
	printf (")");
     }
   printf (";");
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
   if (pR->PrPresMode == PresInherit)
      wrnbherit (pR);
   if (pR->PrPresMode == PresImmediate)
      switch (pR->PrAdjust)
	    {
	       case AlignLeft:
		  printf ("Left;");
		  break;
	       case AlignRight:
		  printf ("Right;");
		  break;
	       case AlignCenter:
		  printf ("VMiddle;");
		  break;
	       case AlignLeftDots:
		  printf ("LeftWithDots;");
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
   if (pR->PrPresMode == PresInherit)
      wrnbherit (pR);
   if (pR->PrPresMode == PresImmediate)
      if (pR->PrJustify)
	 printf ("Yes;");
      else
	 printf ("No;");
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
	       printf (",Arabic)");
	       break;
	    case CntURoman:
	       printf (",URoman)");
	       break;
	    case CntLRoman:
	       printf (",LRoman)");
	       break;
	    case CntUppercase:
	       printf (",Uppercase)");
	       break;
	    case CntLowercase:
	       printf (",Lowercase)");
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
   PtrCondition        pCond;

   while (RP != NULL)
      /* ecrit une regle de presentation */
     {
	printf ("   ");
	if (RP->PrViewNum > 1)
	  {
	     printf ("IN ");
	     wrnom (pSchemaPrs->PsView[RP->PrViewNum - 1]);
	     printf (" ");
	  }
	if (RP->PrCond != NULL)
	  {
	     pCond = RP->PrCond;
	     if (pCond->CoCondition == PcDefaultCond)
		printf ("OTHERWISE ");
	     else
	       {
		  printf ("IF ");
		  wrCondition (pCond);
	       }
	     pCond = pCond->CoNextCondition;
	     while (pCond != NULL)
	       {
		  printf ("AND ");
		  wrCondition (pCond);
		  pCond = pCond->CoNextCondition;
	       }
	  }
	switch (RP->PrType)
	      {
		 case PtFunction:
		    wrFonctPres (RP);
		    break;
		 case PtVisibility:
		    printf ("Visibility: ");
		    wrnbherit (RP);
		    break;
		 case PtDepth:
		    printf ("Depth: ");
		    wrnbherit (RP);
		    break;
		 case PtFillPattern:
		    printf ("FillPattern: ");
		    wrnbherit (RP);
		    break;
		 case PtBackground:
		    printf ("Background: ");
		    wrnbherit (RP);
		    break;
		 case PtForeground:
		    printf ("Foreground: ");
		    wrnbherit (RP);
		    break;
		 case PtBorderTopColor:
		    printf ("BorderTopColor: ");
	            if (RP->PrPresMode == PresImmediate &&
			!RP->PrAttrValue && RP->PrIntValue == -2)
		       printf ("transparent;");
		    else if (RP->PrPresMode == PresImmediate &&
			     !RP->PrAttrValue && RP->PrIntValue == -1)
		       printf ("foreground;");
		    else
		       wrnbherit (RP);
		    break;
		 case PtBorderRightColor:
		    printf ("BorderRightColor: ");
	            if (RP->PrPresMode == PresImmediate &&
			!RP->PrAttrValue && RP->PrIntValue == -2)
		       printf ("transparent;");
		    else if (RP->PrPresMode == PresImmediate &&
			     !RP->PrAttrValue && RP->PrIntValue == -1)
		       printf ("foreground;");
		    else
		       wrnbherit (RP);
		    break;
		 case PtBorderBottomColor:
		    printf ("BorderBottomColor: ");
	            if (RP->PrPresMode == PresImmediate &&
			!RP->PrAttrValue && RP->PrIntValue == -2)
		       printf ("transparent;");
		    else if (RP->PrPresMode == PresImmediate &&
			     !RP->PrAttrValue && RP->PrIntValue == -1)
		       printf ("foreground;");
		    else
		       wrnbherit (RP);
		    break;
		 case PtBorderLeftColor:
		    printf ("BorderLeftColor: ");
	            if (RP->PrPresMode == PresImmediate &&
			!RP->PrAttrValue && RP->PrIntValue == -2)
		       printf ("transparent;");
		    else if (RP->PrPresMode == PresImmediate &&
			     !RP->PrAttrValue && RP->PrIntValue == -1)
		       printf ("foreground;");
		    else
		       wrnbherit (RP);
		    break;
		 case PtFont:
		    printf ("Font: ");
		    wrfontstyle (RP);
		    break;
		 case PtStyle:
		    printf ("Style: ");
		    wrfontstyle (RP);
		    break;
		 case PtWeight:
		    printf ("Weight: ");
		    wrfontstyle (RP);
		    break;
		 case PtUnderline:
		    printf ("Underline: ");
		    wrfontstyle (RP);
		    break;
		 case PtThickness:
		    printf ("Thickness: ");
		    wrfontstyle (RP);
		    break;
		 case PtLineStyle:
		    printf ("LineStyle: ");
		    wrfontstyle (RP);
		    break;
		 case PtBorderTopStyle:
		    printf ("BorderTopStyle: ");
		    wrfontstyle (RP);
		    break;
		 case PtBorderRightStyle:
		    printf ("BorderRightStyle: ");
		    wrfontstyle (RP);
		    break;
		 case PtBorderBottomStyle:
		    printf ("BorderBottomStyle: ");
		    wrfontstyle (RP);
		    break;
		 case PtBorderLeftStyle:
		    printf ("BorderLeftStyle: ");
		    wrfontstyle (RP);
		    break;
		 case PtBreak1:
		    printf ("NoBreak1: ");
		    wrminind (RP);
		    break;
		 case PtBreak2:
		    printf ("NoBreak2: ");
		    wrminind (RP);
		    break;
		 case PtIndent:
		    printf ("Indent: ");
		    wrminind (RP);
		    break;
		 case PtSize:
		    printf ("Size: ");
		    wrsize (RP);
		    break;
		 case PtLineSpacing:
		    printf ("LineSpacing: ");
		    wrminind (RP);
		    break;
		 case PtLineWeight:
		    printf ("LineWeight: ");
		    wrminind (RP);
		    break;
		 case PtMarginTop:
		    printf ("MarginTop: ");
		    wrminind (RP);
		    break;
		 case PtMarginRight:
		    printf ("MarginRight: ");
		    wrminind (RP);
		    break;
		 case PtMarginBottom:
		    printf ("MarginBottom: ");
		    wrminind (RP);
		    break;
		 case PtMarginLeft:
		    printf ("MarginLeft: ");
		    wrminind (RP);
		    break;
		 case PtPaddingTop:
		    printf ("PaddingTop: ");
		    wrminind (RP);
		    break;
		 case PtPaddingRight:
		    printf ("PaddingRight: ");
		    wrminind (RP);
		    break;
		 case PtPaddingBottom:
		    printf ("PaddingBottom: ");
		    wrminind (RP);
		    break;
		 case PtPaddingLeft:
		    printf ("PaddingLeft: ");
		    wrminind (RP);
		    break;
		 case PtBorderTopWidth:
		    printf ("BorderTopWidth: ");
		    wrminind (RP);
		    break;
		 case PtBorderRightWidth:
		    printf ("BorderRightWidth: ");
		    wrminind (RP);
		    break;
		 case PtBorderBottomWidth:
		    printf ("BorderBottomWidth: ");
		    wrminind (RP);
		    break;
	         case PtBorderLeftWidth:
		    printf ("BorderLeftWidth: ");
		    wrminind (RP);
		    break;
		 case PtVertRef:
		    printf ("VertRef: ");
		    wrpos (RP->PrPosRule, False);
		    break;
		 case PtHorizRef:
		    printf ("HorizRef: ");
		    wrpos (RP->PrPosRule, False);
		    break;
		 case PtVertPos:
		    printf ("VertPos: ");
		    wrpos (RP->PrPosRule, True);
		    break;
		 case PtHorizPos:
		    printf ("HorizPos: ");
		    wrpos (RP->PrPosRule, True);
		    break;
		 case PtHeight:
		    printf ("Height: ");
		    wrdimens (RP->PrDimRule, True);
		    break;
		 case PtWidth:
		    printf ("Width: ");
		    wrdimens (RP->PrDimRule, False);
		    break;
		 case PtJustify:
		    printf ("Justify: ");
		    wrjustif (RP);
		    break;
		 case PtHyphenate:
		    printf ("Hyphenate: ");
		    wrjustif (RP);
		    break;
		 case PtVertOverflow:
		    printf ("VertOverflow: ");
		    wrjustif (RP);
		    break;
		 case PtHorizOverflow:
		    printf ("HorizOverflow: ");
		    wrjustif (RP);
		    break;
		 case PtAdjust:
		    printf ("Adjust: ");
		    wrajust (RP);
		    break;
		 case PtPictInfo:
		    break;
	      }
	printf ("\n");		/* passe a la regle suivante */
	RP = RP->PrNextPRule;
     }
}


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
   int                 i, k;

   TtaInitializeAppRegistry (argv[0]);

   i = TtaGetMessageTable (TEXT("libdialogue"), TMSG_LIB_MSG_MAX);
   PRS = TtaGetMessageTable (TEXT("prsdialogue"), PRS_MSG_MAX);
   /* recupere d'abord le nom du schema a lister */
   filename[0] = '\0';
   if (argc != 2)
      goto Usage;
   argv++;
   strcpy (filename, *argv);
   /* indique que le chargement du schema de presentation doit commencer par */
   /* le chargement du schema de structure correspondant */
   GetSchStruct (&pSchemaStr);
   pSchemaStr->SsRootElem = 0;
   /* lit le schema de presentation et le schema de structure correspondant */
   pSchemaPrs = ReadPresentationSchema (filename, pSchemaStr);
   if (pSchemaPrs == NULL)
      TtaDisplaySimpleMessage (FATAL, PRS, UNKNOWN_FILE);
   else
      /* les schemas ont ete lus correctement */
     {
	pSc1 = pSchemaPrs;
	printf ("\n");
	printf ("{ In each block, rules are listed in the same order as they\n");
	printf ("are processed by the editor. }\n");
	/* ecrit au terminal le nom du schema  de structure */
	printf ("\n");
	printf ("PRESENTATION ");
	wrnom (pSchemaStr->SsName);
	printf (";\n");
	/* ecrit au terminal les noms des  vues */
	if (pSc1->PsNViews > 0 && pSc1->PsView[0][0] != ' ')
	  {
	     printf ("\n");
	     printf ("VIEWS\n");
	     printf ("   ");
	     for (i = 1; i <= pSc1->PsNViews; i++)
	       {
		  wrnom (pSc1->PsView[i - 1]);
		  if (pSc1->PsExportView[i - 1])
		     printf (" THOT_EXPORT");
		  if (pSc1->PsPaginatedView[i - 1])
		     printf (" {with pages}");
		  if (i < pSc1->PsNViews)
		     printf (", ");
		  else
		     printf (";\n");
	       }
	  }
	/* ecrit au terminal les compteurs declares */
	if (pSc1->PsNCounters > 0)
	  {
	     printf ("\n");
	     printf ("COUNTERS\n");
	     for (i = 1; i <= pSc1->PsNCounters; i++)
		/* ecrit un compteur */
	       {
		  pCo1 = &pSc1->PsCounter[i - 1];
		  printf ("   Cpt");
		  wrnb (i);
		  printf (": ");
		  for (j = 1; j <= pCo1->CnNItems; j++)
		    {
		       pCp1 = &pCo1->CnItem[j - 1];
		       switch (pCp1->CiCntrOp)
			     {
				case CntrSet:
				   printf (" SET ");
				   wrnb (pCp1->CiParamValue);
				   printf (" ON ");
				   break;
				case CntrAdd:
				   printf (" ADD ");
				   wrnb (pCp1->CiParamValue);
				   printf (" ON ");
				   break;
				case CntrRank:
				   printf (" RANK of ");
				   break;
				case CntrRLevel:
				   printf (" RLEVEL of ");
				   break;
			     }

		       wrnomregle (pCp1->CiElemType);
		       if (pCp1->CiViewNum > 0)
			 {
			    printf ("(");
			    wrnom (pSc1->PsView[pCp1->CiViewNum - 1]);
			    printf (")");
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
		     printf (" %d ", pCo1->CnItem[0].CiAscendLevel);

		  if (pCo1->CnItem[0].CiInitAttr > 0)
		    {
		       printf (" INIT ");
		       wrnomattr (pCo1->CnItem[0].CiInitAttr);
		    }
		  if (pCo1->CnItem[0].CiReinitAttr > 0)
		    {
		       printf (" REINIT ");
		       wrnomattr (pCo1->CnItem[0].CiReinitAttr);
		    }
		  printf (";\n");
		  /* ecrit les noms des boites qui utilisent le compteur */
		  if (pCo1->CnNPresBoxes > 0)
		    {
		       printf ("       { Value used in ");
		       for (j = 1; j <= pCo1->CnNPresBoxes; j++)
			 {
			    wrnomboite (pCo1->CnPresBox[j - 1]);
			    if (j < pCo1->CnNPresBoxes)
			       printf (", ");
			    else
			       printf (". }\n");
			 }
		    }
		  if (pCo1->CnNCreatedBoxes > 0)
		    {
		       printf ("       { Controls creation of ");
		       for (j = 1; j <= pCo1->CnNCreatedBoxes; j++)
			 {
			    wrnomboite (pCo1->CnCreatedBox[j - 1]);
			    if (j < pCo1->CnNCreatedBoxes)
			       printf (", ");
			    else
			       printf (". }\n");
			 }
		    }
		  if (pCo1->CnNCreators > 0)
		    {
		       printf ("       { Controls creation by ");
		       for (j = 1; j <= pCo1->CnNCreators; j++)
			 {
			    if (pCo1->CnPresBoxCreator[j - 1])
			       wrnomboite (pCo1->CnCreator[j - 1]);
			    else
			       wrnomregle (pCo1->CnCreator[j - 1]);
			    if (j < pCo1->CnNCreators)
			       printf (", ");
			    else
			       printf (". ");
			 }
		       printf ("}\n");
		    }
		  if (pCo1->CnPageFooter)
		     printf ("       { Page footer }\n");
	       }
	  }
	/* ecrit au terminal les constantes */
	if (pSc1->PsNConstants > 0)
	  {
	     printf ("\n");
	     printf ("CONST\n");
	     for (i = 1; i <= pSc1->PsNConstants; i++)
	       {
		  pPr1 = &pSc1->PsConstant[i - 1];
		  /* donne un nom a chaque constante */
		  printf ("   Cste");
		  wrnb (i);
		  printf ("= ");
		  /* ecrit le type de la constante */
		  switch (pPr1->PdType)
			{
			   case CharString:
			      printf ("Text ");
			      putchar (pPr1->PdAlphabet);
			      break;
			   case GraphicElem:
			      printf ("Graphics ");
			      break;
			   case Symbol:
			      printf ("Symbol ");
			      break;
			   case Picture:
			      printf ("Picture ");
			      /* ecrit la valeur de la constante */
			      break;
			   default:
			      break;
			}

		  printf (" \'");
		  j = 0;
		  while (pPr1->PdString[j] != '\0')
		    {
		       if (pPr1->PdString[j] < ' ')
			  printf ("\\%3d", (int) pPr1->PdString[j]);
		       else
			  putchar (pPr1->PdString[j]);
		       j++;
		    }
		  printf ("\';\n");
	       }
	  }
	/* ecrit au terminal les variables */
	if (pSc1->PsNVariables > 0)
	  {
	     printf ("\n");
	     printf ("VAR\n");
	     for (i = 1; i <= pSc1->PsNVariables; i++)
	       {
		  pPres1 = &pSc1->PsVariable[i - 1];
		  /* donne un nom a chaque variable */
		  printf ("   Var");
		  wrnb (i);
		  printf (":");
		  for (j = 1; j <= pPres1->PvNItems; j++)
		    {
		       pVa1 = &pPres1->PvItem[j - 1];
		       switch (pVa1->ViType)
			     {
				case VarText:
				   printf (" Cste");
				   wrnb (pVa1->ViConstant);
				   break;
				case VarCounter:
				   printf (" VALUE(");
				   if (pVa1->ViCounterVal == CntMaxVal)
				      printf ("MaxRangeVal ");
				   else if (pVa1->ViCounterVal == CntMinVal)
				      printf ("MinRangeVal ");
				   printf ("Cpt");
				   wrnb (pVa1->ViCounter);
				   WriteCounterStyle (pVa1->ViStyle);
				   break;
				case VarAttrValue:
				   printf (" VALUE(");
				   wrnomattr (pVa1->ViAttr);
				   WriteCounterStyle (pVa1->ViStyle);
				   break;
				case VarDate:
				   printf (" VarDate");
				   break;
				case VarFDate:
				   printf (" VarFDate");
				   break;
				case VarDirName:
				   printf (" DirName");
				   break;
				case VarDocName:
				   printf (" DocName");
				   break;
				case VarElemName:
				   printf (" ElemName");
				   break;
				case VarAttrName:
				   printf (" VarAttrName");
				   break;
				case VarPageNumber:
				   printf (" VALUE(PageNumber(");
				   wrnom (pSc1->PsView[pVa1->ViView - 1]);
				   printf (")");
				   WriteCounterStyle (pVa1->ViStyle);
				   break;
			     }

		    }
		  printf (";\n");
	       }
	  }
	/* ecrit au terminal les regles de presentation par defaut */
	printf ("\n");
	printf ("DEFAULT\n");
	printf ("\n");
	printf ("   BEGIN\n");
	wrsuiteregles (pSc1->PsFirstDefaultPRule);
	printf ("   END;\n");
	printf ("\n");
	if (pSc1->PsNPresentBoxes > 0)
	   /* ecrit au terminal les boites de presentation et de mise en page */
	  {
	     printf ("\n");
	     printf ("BOXES\n");
	     printf ("\n");
	     for (i = 1; i <= pSc1->PsNPresentBoxes; i++)
	       {
		  pBo1 = &pSc1->PsPresentBox[i - 1];
		  wrnom (pBo1->PbName);
		  printf (":	{%3d }\n", i);
		  if (pBo1->PbFirstPRule != NULL)
		     printf ("   BEGIN\n");
		  wrsuiteregles (pBo1->PbFirstPRule);
		  printf ("   PageBreak: ");
		  if (pBo1->PbAcceptPageBreak)
		     printf ("Yes;\n");
		  else
		     printf ("No;\n");
		  printf ("   LineBreak: ");
		  if (pBo1->PbAcceptLineBreak)
		     printf ("Yes;\n");
		  else
		     printf ("No;\n");
		  printf ("   Gather: ");
		  if (pBo1->PbBuildAll)
		     printf ("Yes;\n");
		  else
		     printf ("No;\n");
		  /* ecrit le contenu de la boite */
		  if (pBo1->PbContent != FreeContent)
		    {
		       printf ("   Content: ");
		       switch (pBo1->PbContent)
			     {
				case ContVariable:
				   printf ("Var");
				   wrnb (pBo1->PbContVariable);
				   break;
				case ContConst:
				   printf ("Cste");
				   wrnb (pBo1->PbContConstant);
				   break;
				case ContElement:
				   wrnomregle (pBo1->PbContElem);
				   printf (" {referenced by ");
				   wrnomregle (pBo1->PbContRefElem);
				   printf ("}");
				   break;
				case FreeContent:
				   ;
				   break;
			     }

		       printf (";\n");
		    }
		  if (pBo1->PbPageBox)
		    {
		       printf ("   { Page box, footer=%5d pt, ", pBo1->PbFooterHeight);
		       if (pBo1->PbPageCounter == 0)
			  printf ("no page number }\n");
		       else
			 {
			    printf ("page number=Cpt");
			    wrnb (pBo1->PbPageCounter);
			    printf (" }\n");
			 }
		    }
		  if (pBo1->PbPageFooter)
		     printf ("   { Bottom of page }\n");
		  if (pBo1->PbPageHeader)
		     printf ("   { Top of page }\n");
		  if (pBo1->PbFirstPRule != NULL)
		     printf ("   END;\n");
		  printf ("\n");
	       }
	  }
	/* ecrit au terminal les regles de presentation des elements structure's */
	printf ("\nRULES\n\n");
	for (El = 1; El <= pSchemaStr->SsNRules; El++)
	  {
	     if (pSchemaStr->SsRule[El - 1].SrConstruct == CsPairedElement)
		if (pSchemaStr->SsRule[El - 1].SrFirstOfPair)
		   printf ("First ");
		else
		   printf ("Second ");
	     wrnomregle (El);
	     printf (":\n");
	     if (pSc1->PsElemPRule[El - 1] != NULL)
		printf ("   BEGIN\n");
	     wrsuiteregles (pSc1->PsElemPRule[El - 1]);
	     printf ("   PageBreak: ");
	     if (pSc1->PsAcceptPageBreak[El - 1])
		printf ("Yes;\n");
	     else
		printf ("No;\n");
	     printf ("   LineBreak: ");
	     if (pSc1->PsAcceptLineBreak[El - 1])
		printf ("Yes;\n");
	     else
		printf ("No;\n");
	     printf ("   Gather: ");
	     if (pSc1->PsBuildAll[El - 1])
		printf ("Yes;\n");
	     else
		printf ("No;\n");
	     if (pSc1->PsInPageHeaderOrFooter[El - 1])
		printf ("   { displayed in top or bottom of page }\n");
	     if (pSc1->PsAssocPaginated[El - 1])
		printf ("   { with pages }\n");
	     if (pSc1->PsElemPRule[El - 1] != NULL)
		printf ("   END;\n");
	     printf ("\n");
	  }
	/* ecrit au terminal les regles de presentation des attributs */
	if (pSchemaStr->SsNAttributes > 0)
	  {
	     printf ("\n");
	     printf ("ATTRIBUTES\n");
	     printf ("\n");
	     for (Attr = 1; Attr <= pSchemaStr->SsNAttributes; Attr++)
	       {
	       pAt1 = &pSchemaStr->SsAttribute[Attr - 1];
	       pRP1 = pSc1->PsAttrPRule[Attr - 1];
	       for (k = pSc1->PsNAttrPRule[Attr - 1]; k-- > 0;
		    pRP1 = pRP1->ApNextAttrPres)
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
					     printf ("(");
					     wrnomregle (pRP1->ApElemType);
					     printf (")");
					  }
					if (pCa1->CaLowerBound == pCa1->CaUpperBound)
					  {
					     printf ("=");
					     wrnb (pCa1->CaLowerBound);
					  }
					else if (pCa1->CaLowerBound != -MAX_INT_ATTR_VAL - 1
						 && pCa1->CaUpperBound != MAX_INT_ATTR_VAL + 1)
					  {
					     printf (" IN [");
					     wrnb (pCa1->CaLowerBound);
					     printf ("..");
					     wrnb (pCa1->CaUpperBound);
					     printf ("] ");
					  }
					else if (pCa1->CaLowerBound != -MAX_INT_ATTR_VAL - 1
						 || pCa1->CaUpperBound != MAX_INT_ATTR_VAL + 1)
					   if (pCa1->CaLowerBound != -MAX_INT_ATTR_VAL - 1)
					     {
						printf (">");
						wrnb (pCa1->CaLowerBound - 1);
					     }
					   else if (pCa1->CaUpperBound != MAX_INT_ATTR_VAL + 1)
					     {
						printf ("<");
						wrnb (pCa1->CaUpperBound + 1);
					     }
					printf (":\n");
					if (pCa1->CaFirstPRule == NULL)
					   printf ("   BEGIN END;\n");
					else
					  {
					     if (pCa1->CaFirstPRule->PrNextPRule != NULL)
						printf ("   BEGIN\n");
					     wrsuiteregles (pCa1->CaFirstPRule);
					     if (pCa1->CaFirstPRule->PrNextPRule != NULL)
						printf ("   END;\n");
					  }
					printf ("\n");
				     }
				   break;
				case AtTextAttr:
				   if (pRP1->ApTextFirstPRule != NULL)
				     {
					wrnom (pAt1->AttrName);
					if (pRP1->ApElemType > 0)
					  {
					     printf ("(");
					     wrnomregle (pRP1->ApElemType);
					     printf (")");
					  }
					if (pRP1->ApString[0] != '\0')
					  {
					     printf ("=\'");
					     wrnom (pRP1->ApString);
					     printf ("\'");
					  }
					printf (":\n");
					if (pRP1->ApTextFirstPRule->PrNextPRule != NULL)
					   printf ("   BEGIN\n");
					wrsuiteregles (pRP1->ApTextFirstPRule);
					if (pRP1->ApTextFirstPRule->PrNextPRule != NULL)
					   printf ("   END;\n");
					printf ("\n");
				     }
				   break;
				case AtReferenceAttr:
				   if (pRP1->ApRefFirstPRule != NULL)
				     {
					wrnom (pAt1->AttrName);
					if (pRP1->ApElemType > 0)
					  {
					     printf ("(");
					     wrnomregle (pRP1->ApElemType);
					     printf (")");
					  }
					printf (":\n");
					if (pRP1->ApRefFirstPRule->PrNextPRule != NULL)
					   printf ("   BEGIN\n");
					wrsuiteregles (pRP1->ApRefFirstPRule);
					if (pRP1->ApRefFirstPRule->PrNextPRule != NULL)
					   printf ("   END;\n");
					printf ("\n");
				     }
				   break;
				case AtEnumAttr:
				   for (Val = 0; Val <= pAt1->AttrNEnumValues; Val++)
				      if (pRP1->ApEnumFirstPRule[Val] != NULL)
					{
					   wrnom (pAt1->AttrName);
					   if (pRP1->ApElemType > 0)
					     {
						printf ("(");
						wrnomregle (pRP1->ApElemType);
						printf (")");
					     }
					   if (Val > 0)
					     {
						printf ("=");
						wrnom (pAt1->AttrEnumValue[Val - 1]);
					     }
					   printf (":\n");
					   if (pRP1->ApEnumFirstPRule[Val]->PrNextPRule
					       != NULL)
					      printf ("   BEGIN\n");
					   wrsuiteregles (pRP1->ApEnumFirstPRule[Val]);
					   if (pRP1->ApEnumFirstPRule[Val]->PrNextPRule
					       != NULL)
					      printf ("   END;\n");
					   printf ("\n");
					}
				   break;
				default:;
				   break;
			     }

		    }
	       }
	  }
	/* ecrit au terminal les regles de transmission de valeurs aux */
	/* attributs des documents inclus */
	Transm = False;
	/* ecrit d'abord les regles de transmission des compteurs */
	for (i = 1; i <= pSc1->PsNCounters; i++)
	  {
	     pCo1 = &pSc1->PsCounter[i - 1];
	     for (j = 1; j <= pCo1->CnNTransmAttrs; j++)
	       {
		  if (!Transm)
		    {
		       printf ("\nTRANSMIT\n");
		       Transm = True;
		    }
		  printf ("   Cpt");
		  wrnb (i);
		  printf (" To ");
		  wrnom (pCo1->CnTransmAttr[j - 1]);
		  printf ("(");
		  wrnomregle (pCo1->CnTransmSSchemaAttr[j - 1]);
		  printf (");\n");
	       }
	  }
	/* ecrit les regles de transmission de valeurs d'elements */
	if (pSc1->PsNTransmElems > 0)
	   for (i = 1; i <= pSchemaStr->SsNRules; i++)
	      if (pSc1->PsElemTransmit[i - 1] > 0)
		{
		   if (!Transm)
		     {
			printf ("\nTRANSMIT\n");
			Transm = True;
		     }
		   printf ("   ");
		   wrnomregle (i);
		   printf (" To ");
		   wrnom (pSc1->PsTransmElem[pSc1->PsElemTransmit[i - 1] - 1].TeTargetAttr);
		   printf ("(");
		   wrnomregle (pSc1->PsTransmElem[pSc1->PsElemTransmit[i - 1] - 1].TeTargetDoc);
		   printf (");\n");
		}

	printf ("END\n");
     }
   TtaSaveAppRegistry ();
   exit (0);

 Usage:
   fprintf (stderr, "usage : %s <input-file>\n", argv[0]);
   exit (1);
}
/* End Of Module Printprs */
