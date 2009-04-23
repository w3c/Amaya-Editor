/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*----------------------------------------------------------------------
  Ce programme effectue le chargement d'un schema de presentation et
  du schema de structure associe' et liste le contenu du schema de
  presentation.        
  ----------------------------------------------------------------------*/
#undef THOT_EXPORT
#define THOT_EXPORT extern

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "prsmsg.h"
#include "registry.h"
#include "zlib.h"
#include "fileaccess.h"
#include "message.h"

#include "platform_tv.h"

#include "fileaccess_f.h"
#include "memory_f.h"
#include "readprs_f.h"
#include "readstr_f.h"

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
PtrPresentationBox  pBo1;
TtAttribute        *pAt1;
AttributePres      *pRP1;
NumAttrCase        *pCa1;
ThotBool            Transm;

/*----------------------------------------------------------------------
  wrnb ecrit au terminal l'entier nb.                             
  ----------------------------------------------------------------------*/
static void         wrnb (int nb)
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
static void         wrlevel (Level n)
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
    case RlLastSibling:
      printf ("LastSibling");
      break;
    }
}

/*----------------------------------------------------------------------
  wrdistunit ecrit le nom d'une unite' de distance.               
  ----------------------------------------------------------------------*/
static void         wrdistunit (TypeUnit u)
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
    case UnUndefined:
      printf (" ???");
      break;
    }
}

/*----------------------------------------------------------------------
  wrrepere ecrit au terminal le repere de boite r.                
  ----------------------------------------------------------------------*/
static void         wrrepere (BoxEdge r)
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
static void wrnom (char *n)
{
  int                 i;

  if (n)
    {
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
}


/*----------------------------------------------------------------------
  wrnomregle ecrit au terminal le nom de la regle de numero r.    
  ----------------------------------------------------------------------*/
static void         wrnomregle (int r)
{
  if (r > 0)
    wrnom (pSchemaStr->SsRule->SrElem[r - 1]->SrName);
}

/*----------------------------------------------------------------------
  wrnomattr ecrit au terminal le nom de l'attribut de numero a.   
  ----------------------------------------------------------------------*/
static void         wrnomattr (int a)
{
  if (a != 0)
    wrnom (pSchemaStr->SsAttribute->TtAttr[abs (a) - 1]->AttrName);
}


/*----------------------------------------------------------------------
  wrnomboite ecrit au terminal le nom de la boite de presentation 
  de numero b.                                            
  ----------------------------------------------------------------------*/
static void         wrnomboite (int b)
{
  if (b > 0)
    wrnom (pSchemaPrs->PsPresentBox->PresBox[b - 1]->PbName);
}

/*----------------------------------------------------------------------
  wrModeHerit ecrit au terminal un mode d'heritage.               
  ----------------------------------------------------------------------*/
static void         wrModeHerit (InheritMode M)
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
static void         wrsize (PtrPRule pR)
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

/*----------------------------------------------------------------------
  wrfontstyle ecrit au terminal la regle d'heritage ou la valeur  
  entiere de la regle pointee par pR.                     
  ----------------------------------------------------------------------*/
static void         wrfontstyle (PtrPRule pR)
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
      else if (pR->PrType == PtVariant)
        switch (pR->PrChrValue)
          {
          case 'C':
            printf ("SmallCaps");
            break;
          case 'D':
            printf ("DoubleStruck");
            break;
          case 'F':
            printf ("Fraktur");
            break;
          case 'N':
            printf ("Normal");
            break;
          case 'S':
            printf ("Script");
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
      else if (pR->PrType == PtDirection)
        switch (pR->PrChrValue)
          {
          case 'L':
            printf ("ltr");
            break;
          case 'R':
            printf ("rtl");
            break;
          default:
            putchar (pR->PrChrValue);
            break;
          }
      else if (pR->PrType == PtUnicodeBidi)
        switch (pR->PrChrValue)
          {
          case 'N':
            printf ("Normal");
            break;
          case 'E':
            printf ("Embed");
            break;
          case 'O':
            printf ("Override");
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
      else if (pR->PrType == PtDisplay)
        switch (pR->PrChrValue)
          {
          case 'N':
            printf ("none");
            break;
          case 'I':
            printf ("inline");
            break;
          case 'B':
            printf ("block");
            break;
          case 'L':
            printf ("listItem");
            break;
          case 'R':
            printf ("runIn");
            break;
          case 'C':
            printf ("compact");
            break;
          case 'M':
            printf ("marker");
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
static void         wrnbherit (PtrPRule pR)
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
    if (pR->PrValueType == PrAttrValue)
      wrnomattr (pR->PrIntValue);
    else if (pR->PrValueType == PrNumValue)
      wrnb (pR->PrIntValue);
    else if (pR->PrValueType == PrConstStringValue)
      {
	printf ("Cste");
	wrnb (pR->PrIntValue);
      }
  else
    printf ("??????");
  printf (";");
}

/*----------------------------------------------------------------------
  wrminind ecrit au terminal une regle du type Indent, Size, Margin*, etc.  
  La regle a ecrire est pointee par pR.                                  
  ----------------------------------------------------------------------*/
static void         wrminind (PtrPRule pR)
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
static void         wrpos (PosRule pos, ThotBool Def)
{
  if (Def)
    {
      if (pos.PoPosDef == NoEdge)
        printf (" NULL");
      else
        {
          wrrepere (pos.PoPosDef);
          printf (" = ");
        }
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
      else if (pos.PoRefKind == RkAnyElem)
        printf ("AnyElem");
      else if (pos.PoRefKind == RkAnyBox)
        printf ("AnyBox");
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
static void         wrdimens (DimensionRule Dim, ThotBool Hauteur)
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
          else if (Dim.DrRefKind == RkAnyElem)
            printf ("AnyElem");
          else if (Dim.DrRefKind == RkAnyBox)
            printf ("AnyBox");
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
static void         wrCondition (PtrCondition pCond)
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
    case PcRoot:
      printf ("PcRoot ");
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
      wrnomregle (pCond->CoTypeElem);
      printf (" ");
      break;
    case PcInheritAttribute:
      printf ("Inherited ");
    case PcAttribute:
      wrnomattr (pCond->CoTypeAttr);
      if (pCond->CoTestAttrValue)
        {
          printf (" = ");
          if (pSchemaStr->SsAttribute->TtAttr[pCond->CoTypeAttr - 1]->AttrType == AtTextAttr)
            wrnom (pCond->CoAttrTextValue);
          else if (pSchemaStr->SsAttribute->TtAttr[pCond->CoTypeAttr - 1]->AttrType == AtEnumAttr)
            wrnom (pSchemaStr->SsAttribute->TtAttr[pCond->CoTypeAttr - 1]->AttrEnumValue[pCond->CoAttrValue - 1]);
          else
            wrnb (pCond->CoAttrValue);
        }
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
static void         wrFonctPres (PtrPRule pR)
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
    case FnContent:
      printf ("Content");
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
static void         wrajust (PtrPRule pR)
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
      case AlignJustify:
        printf ("Justify;");
        break;
	    }
}

/*----------------------------------------------------------------------
  wrboolean ecrit la regle booleenne pointee par pR.
  ----------------------------------------------------------------------*/
static void         wrboolean (PtrPRule pR)
{
  if (pR->PrPresMode == PresInherit)
    wrnbherit (pR);
  if (pR->PrPresMode == PresImmediate)
    {
      if (pR->PrBoolValue)
        printf ("Yes;");
      else
        printf ("No;");
    }
}

/*----------------------------------------------------------------------
  WriteCounterStyle ecrit au terminal un style de compteur.            
  ----------------------------------------------------------------------*/
static void         WriteCounterStyle (CounterStyle St)
{
  switch (St)
    {
    case CntDecimal:
      printf (",Decimal)");
      break;
    case CntZLDecimal:
      printf (",ZeroLeadingDecimal)");
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
    case CntUGreek:
      printf (",UGreek)");
      break;
    case CntLGreek:
      printf (",LGreek)");
      break;
    }
}

/*----------------------------------------------------------------------
  wrsuiteregles ecrit au terminal la suite de regles chainees dont 
  RP pointe sur la regle de tete.                         
  ----------------------------------------------------------------------*/
static void         wrsuiteregles (PtrPRule RP)
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
        case PtVisibility:
          printf ("Visibility: ");
          wrnbherit (RP);
          break;
        case PtFunction:
          wrFonctPres (RP);
          break;
        case PtListStyleType:
          printf ("ListStyleType: @@@");
          break;
        case PtListStyleImage:
          printf ("ListStyleImagee: @@@");
          break;
        case PtListStylePosition:
          printf ("ListStylePosition: @@@");
          break;
        case PtVertOverflow:
          printf ("VertOverflow: ");
          wrboolean (RP);
          break;
        case PtHorizOverflow:
          printf ("HorizOverflow: ");
          wrboolean (RP);
          break;
        case PtVertRef:
          printf ("VertRef: ");
          wrpos (RP->PrPosRule, False);
          break;
        case PtHorizRef:
          printf ("HorizRef: ");
          wrpos (RP->PrPosRule, False);
          break;
        case PtHeight:
          printf ("Height: ");
          wrdimens (RP->PrDimRule, True);
          break;
        case PtWidth:
          printf ("Width: ");
          wrdimens (RP->PrDimRule, False);
          break;
        case PtVertPos:
          printf ("VertPos: ");
          wrpos (RP->PrPosRule, True);
          break;
        case PtHorizPos:
          printf ("HorizPos: ");
          wrpos (RP->PrPosRule, True);
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
        case PtBorderTopColor:
          printf ("BorderTopColor: ");
          if (RP->PrPresMode == PresImmediate &&
              RP->PrValueType == PrNumValue && RP->PrIntValue == -2)
            printf ("transparent;");
          else if (RP->PrPresMode == PresImmediate &&
                   RP->PrValueType == PrNumValue && RP->PrIntValue == -1)
            printf ("foreground;");
          else
            wrnbherit (RP);
          break;
        case PtBorderRightColor:
          printf ("BorderRightColor: ");
          if (RP->PrPresMode == PresImmediate &&
              RP->PrValueType == PrNumValue && RP->PrIntValue == -2)
            printf ("transparent;");
          else if (RP->PrPresMode == PresImmediate &&
                   RP->PrValueType == PrNumValue && RP->PrIntValue == -1)
            printf ("foreground;");
          else
            wrnbherit (RP);
          break;
        case PtBorderBottomColor:
          printf ("BorderBottomColor: ");
          if (RP->PrPresMode == PresImmediate &&
              RP->PrValueType == PrNumValue && RP->PrIntValue == -2)
            printf ("transparent;");
          else if (RP->PrPresMode == PresImmediate &&
                   RP->PrValueType == PrNumValue && RP->PrIntValue == -1)
            printf ("foreground;");
          else
            wrnbherit (RP);
          break;
        case PtBorderLeftColor:
          printf ("BorderLeftColor: ");
          if (RP->PrPresMode == PresImmediate &&
              RP->PrValueType == PrNumValue && RP->PrIntValue == -2)
            printf ("transparent;");
          else if (RP->PrPresMode == PresImmediate &&
                   RP->PrValueType == PrNumValue && RP->PrIntValue == -1)
            printf ("foreground;");
          else
            wrnbherit (RP);
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
        case PtSize:
          printf ("Size: ");
          wrsize (RP);
          break;
        case PtStyle:
          printf ("Style: ");
          wrfontstyle (RP);
          break;
        case PtWeight:
          printf ("Weight: ");
          wrfontstyle (RP);
          break;
        case PtFont:
          printf ("Font: ");
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
        case PtIndent:
          printf ("Indent: ");
          wrminind (RP);
          break;
        case PtLineSpacing:
          printf ("LineSpacing: ");
          wrminind (RP);
          break;
        case PtDepth:
          printf ("Depth: ");
          wrnbherit (RP);
          break;
        case PtAdjust:
          printf ("Adjust: ");
          wrajust (RP);
          break;
        case PtDirection:
          printf ("Direction: ");
          wrfontstyle (RP);
          break;
        case PtUnicodeBidi:
          printf ("UnicodeBidi: ");
          wrfontstyle (RP);
          break;
        case PtLineStyle:
          printf ("LineStyle: ");
          wrfontstyle (RP);
          break;
        case PtLineWeight:
          printf ("LineWeight: ");
          wrminind (RP);
          break;
        case PtFillPattern:
          printf ("FillPattern: ");
          wrnbherit (RP);
          break;
        case PtOpacity:
          printf ("Opacity: ");
          wrnbherit (RP);
          break;
        case PtFillOpacity:
          printf ("FillOpacity: ");
          wrnbherit (RP);
          break;
        case PtStrokeOpacity:
          printf ("StrokeOpacity: ");
          wrnbherit (RP);
          break; 
        case PtStopOpacity:
          printf ("StopOpacity: ");
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
        case PtColor:
          printf ("Color: ");
          wrnbherit (RP);
          break;
        case PtStopColor:
          printf ("StopColor: ");
          wrnbherit (RP);
          break;
        case PtHyphenate:
          printf ("Hyphenate: ");
          wrboolean (RP);
          break;
        case PtPageBreak:
          printf ("PageBreak: ");
          wrboolean (RP);
          break;
        case PtLineBreak:
          printf ("LineBreak: ");
          wrboolean (RP);
          break;
        case PtGather:
          printf ("Gather: ");
          wrboolean (RP);
          break;
        case PtXRadius:
          printf ("XRadius: ");
          wrboolean (RP);
          break;
        case PtYRadius:
          printf ("YRadius: ");
          wrboolean (RP);
          break;
        case PtPosition:
          printf ("Position: @@@");
          break;
        case PtTop:
          printf ("Top: @@@");
          break;
        case PtRight:
          printf ("Right: @@@");
          break;
        case PtBottom:
          printf ("Bottom: @@@");
          break;
        case PtLeft:
          printf ("Left: @@@");
          break;
        case PtFloat:
          printf ("Float: @@@");
          break;
	      case PtClear:
          printf ("Clear: @@@");
          break;
        case PtDisplay:
          printf ("Display: ");
          wrfontstyle (RP);
          break;
        case PtVis:
          printf ("Visibility: @@@");
          break;
        case PtBreak1:
          printf ("NoBreak1: ");
          wrminind (RP);
          break;
        case PtBreak2:
          printf ("NoBreak2: ");
          wrminind (RP);
          break;
        case PtPictInfo:
          break;
	      }
      if (RP->PrDuplicate)
        printf ("  {duplicate}");
      printf ("\n");		/* passe a la regle suivante */
      RP = RP->PrNextPRule;
    }
}

/*----------------------------------------------------------------------
  main                                                            
  ----------------------------------------------------------------------*/
int                 main (int argc, char **argv)
{
  int                 i, k;
  PtrHostView         pHostView;
  char               *ptr;

  TtaInitializeAppRegistry (argv[0]);

  i = TtaGetMessageTable ("libdialogue", TMSG_LIB_MSG_MAX);
  PRS = TtaGetMessageTable ("prsdialogue", PRS_MSG_MAX);
  /* recupere d'abord le nom du schema a lister */
  filename[0] = '\0';
  if (argc != 3)
    goto Usage;
  argv++;
  strcpy (filename, *argv);
  /* chargement du schema de structure */
  GetSchStruct (&pSchemaStr);
  if (!ReadStructureSchema (filename, pSchemaStr))
    {
      TtaDisplaySimpleMessage (FATAL, PRS, UNKNOWN_FILE);
      return 1;
    }
  argv++;
  strcpy (filename, *argv);
  /* lit le schema de presentation */
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
      /* write the name of all declared views and their host views */
      if (pSc1->PsNViews > 0 && pSc1->PsView[0][0] != ' ')
        {
          printf ("\n");
          printf ("VIEWS\n");
          for (i = 1; i <= pSc1->PsNViews; i++)
            {
              printf ("   ");
              wrnom (pSc1->PsView[i - 1]);
              if (pSc1->PsExportView[i - 1])
                printf (" EXPORT");
              if (pSc1->PsPaginatedView[i - 1])
                printf (" {with pages}");
              if (pSc1->PsHostViewList[i - 1])
                {
                  printf (" MERGE With");
                  pHostView = pSc1->PsHostViewList[i - 1];
                  while (pHostView)
                    {
                      printf (" ");
                      wrnom (pHostView->HostViewName);
                      pHostView = pHostView->NextHostView;
                    } 
                }
              if (i < pSc1->PsNViews)
                printf (",\n");
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
                  putchar (pPr1->PdScript);
                  break;
                case GraphicElem:
                  printf ("Graphics ");
                  break;
                case Symbol:
                  printf ("Symbol ");
                  break;
                case tt_Picture:
                  printf ("Picture ");
                  /* ecrit la valeur de la constante */
                  break;
                default:
                  break;
                }

              printf (" \'");
              ptr = pPr1->PdString;
              if (ptr)
                {
                  j = 0;
                  while (ptr[j] != EOS)
                    {
                      if (ptr[j] < ' ')
                        printf ("\\%3d", (int) pPr1->PdString[j]);
                      else
                        putchar (ptr[j]);
                      j++;
                    }
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
                      if (pVa1->ViAttr == 0)
                        printf (" AttributeValue");
                      printf (" VALUE(");
                      wrnomattr (pVa1->ViAttr);
                      WriteCounterStyle (pVa1->ViStyle);
                      break;
                    case VarDate:
                      printf (" Date");
                      break;
                    case VarFDate:
                      printf (" FDate");
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
                      printf (" AttributeName");
                      break;
                    case VarPageNumber:
                      printf (" VALUE(PageNumber(");
                      wrnom (pSc1->PsView[pVa1->ViView - 1]);
                      printf (")");
                      WriteCounterStyle (pVa1->ViStyle);
                      break;
                    case VarNamedAttrValue:
                      printf ("  NamedAttributeValue");
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
              pBo1 = pSc1->PsPresentBox->PresBox[i - 1];
              wrnom (pBo1->PbName);
              printf (":	{%3d }\n", i);
              if (pBo1->PbFirstPRule != NULL)
                printf ("   BEGIN\n");
              wrsuiteregles (pBo1->PbFirstPRule);
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
                    case FreeContent:
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
          if (pSchemaStr->SsRule->SrElem[El - 1]->SrConstruct == CsPairedElement)
            {
              if (pSchemaStr->SsRule->SrElem[El - 1]->SrFirstOfPair)
                printf ("First ");
              else
                printf ("Second ");
            }
          wrnomregle (El);
          printf (":\n");
          if (pSc1->PsElemPRule->ElemPres[El - 1])
            printf ("   BEGIN\n");
          wrsuiteregles (pSc1->PsElemPRule->ElemPres[El - 1]);
          if (pSc1->PsElemPRule->ElemPres[El - 1])
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
              pAt1 = pSchemaStr->SsAttribute->TtAttr[Attr - 1];
              pRP1 = pSc1->PsAttrPRule->AttrPres[Attr - 1];
              for (k = pSc1->PsNAttrPRule->Num[Attr - 1]; k-- > 0;
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
                              {
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
                            if (pRP1->ApString &&
                                pRP1->ApString[0] != '\0')
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
          if (pSc1->PsElemTransmit->Num[i - 1] > 0)
            {
              if (!Transm)
                {
                  printf ("\nTRANSMIT\n");
                  Transm = True;
                }
              printf ("   ");
              wrnomregle (i);
              printf (" To ");
              wrnom (pSc1->PsTransmElem[pSc1->PsElemTransmit->Num[i - 1] - 1].TeTargetAttr);
              printf ("(");
              wrnomregle (pSc1->PsTransmElem[pSc1->PsElemTransmit->Num[i - 1] - 1].TeTargetDoc);
              printf (");\n");
            }

      printf ("END\n");
    }
  TtaSaveAppRegistry ();
  exit (0);

 Usage:
  fprintf (stderr, "usage : %s <S schema> <P schema>\n", argv[0]);
  exit (1);
}
/* End Of Module Printprs */
