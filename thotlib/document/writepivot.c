/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*

This module writes in an output file the pivot representation of a
document.

*/

#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "constpiv.h"
#include "fileaccess.h"
#include "appaction.h"
#include "appstruct.h"
#include "typecorr.h"
#include "zlib.h"
#include "fileaccess.h"
#include "labelAllocator.h"
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#ifndef NODISPLAY
#include "modif_tv.h"
#endif
#include "platform_tv.h"
#include "edit_tv.h"

#include "applicationapi_f.h"
#include "attributes_f.h"
#include "callback_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "externalref_f.h"
#include "fileaccess_f.h"
#include "inites_f.h"
#include "labelalloc_f.h"
#include "language_f.h"
#include "memory_f.h"
#include "platform_f.h"
#include "readpivot_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structschema_f.h"
#include "structmodif_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "units_f.h"
#include "writepivot_f.h"

/*----------------------------------------------------------------------
  Ecrit dans le fichier le numero de version pivot courant           
  ----------------------------------------------------------------------*/
void WriteVersionNumber (BinFile pivFile)
{
  int                 version;

  TtaWriteByte (pivFile, (char) C_PIV_VERSION);
  TtaWriteByte (pivFile, (char) C_PIV_VERSION);
  /* Version courante de PIVOT: 6 */
  version = 6;
  TtaWriteByte (pivFile, (char) version);
}

/*----------------------------------------------------------------------
  PutShort   ecrit un entier court dans le fichier sur deux octets   
  ----------------------------------------------------------------------*/
static void PutShort (BinFile pivFile, int n)
{
  TtaWriteByte (pivFile, (char) (n / 256));
  TtaWriteByte (pivFile, (char) (n % 256));
}

/*----------------------------------------------------------------------
  PutInteger ecrit un entier long dans le fichier, sur 4 octets      
  ----------------------------------------------------------------------*/
static void PutInteger (BinFile pivFile, int n)
{
  PutShort (pivFile, n / 65536);
  PutShort (pivFile, n % 65536);
}

/*----------------------------------------------------------------------
  PutInteger ecrit un entier long dans le fichier, sur 4 octets      
  ----------------------------------------------------------------------*/
static void PutFloat (BinFile pivFile, float n)
{
  TtaWrite4Byte (pivFile, (unsigned char*) (&n));
}
/*----------------------------------------------------------------------
  PutDimensionType ecrit dans le fichier un type de dimension sur	
  1 octet.								
  ----------------------------------------------------------------------*/
static void PutDimensionType (BinFile pivFile, ThotBool b)
{
  if (b)
    TtaWriteByte (pivFile, C_PIV_ABSOLUTE);
  else
    TtaWriteByte (pivFile, C_PIV_RELATIVE);
}

/*----------------------------------------------------------------------
  PutUnit ecrit dans le fichier l'unite                              
  ----------------------------------------------------------------------*/
static void PutUnit (BinFile pivFile, TypeUnit unit)
{
  if (unit == UnPoint)
    TtaWriteByte (pivFile, C_PIV_PT);
  else if (unit == UnPixel)
    TtaWriteByte (pivFile, C_PIV_PX);
  else if (unit == UnAuto)
    TtaWriteByte (pivFile, C_PIV_AUTO);
  else if (unit == UnPercent)
    TtaWriteByte (pivFile, C_PIV_PERCENT);
  else
    TtaWriteByte (pivFile, C_PIV_EM);
}

/*----------------------------------------------------------------------
  PutSign   ecrit un signe dans le fichier sur un octet             
  ----------------------------------------------------------------------*/
static void PutSign (BinFile pivFile, ThotBool b)
{
  if (b)
    TtaWriteByte (pivFile, C_PIV_PLUS);
  else
    TtaWriteByte (pivFile, C_PIV_MINUS);
}

/*----------------------------------------------------------------------
  PutDimension ecrit dans le fichier le contenu de la regle de       
  dimension pointee par pRegle                            
  ----------------------------------------------------------------------*/
static void PutDimension (BinFile pivFile, PtrPRule pPRule)
{

  PutDimensionType (pivFile, pPRule->PrDimRule.DrAbsolute);
  PutShort (pivFile, abs (pPRule->PrDimRule.DrValue));
  if (pPRule->PrDimRule.DrUnit == UnPercent)
    TtaWriteByte (pivFile, C_PIV_PERCENT);
  else
    PutUnit (pivFile, pPRule->PrDimRule.DrUnit);
  PutSign (pivFile, (ThotBool) (pPRule->PrDimRule.DrValue >= 0));
}

/*----------------------------------------------------------------------
  PutBoolean ecrit un booleen dans le fichier sur un octet           
  ----------------------------------------------------------------------*/
static void PutBoolean (BinFile pivFile, ThotBool b)
{
  if (b)
    TtaWriteByte (pivFile, C_PIV_TRUE);
  else
    TtaWriteByte (pivFile, C_PIV_FALSE);
}

/*----------------------------------------------------------------------
  PutAlignment ecrit un BAlignment dans le fichier sur un octet      
  ----------------------------------------------------------------------*/
static void PutAlignment (BinFile pivFile, BAlignment c)
{
  switch (c)
    {
    case AlignLeft:
      TtaWriteByte (pivFile, C_PIV_LEFT);
      break;
    case AlignRight:
      TtaWriteByte (pivFile, C_PIV_RIGHT);
      break;
    case AlignCenter:
      TtaWriteByte (pivFile, C_PIV_CENTERED);
      break;
    case AlignLeftDots:
      TtaWriteByte (pivFile, C_PIV_LEFTDOT);
      break;
    case AlignJustify:
      TtaWriteByte (pivFile, C_PIV_JUSTIFY);
      break;
    }
}

/*----------------------------------------------------------------------
  PutPageType ecrit un type de page dans le fichier sur un octet     
  ----------------------------------------------------------------------*/
static void PutPageType (BinFile pivFile, PageType t)
{
  switch (t)
    {
    case PgComputed:
      TtaWriteByte (pivFile, C_PIV_COMPUTED_PAGE);
      break;
    case PgBegin:
      TtaWriteByte (pivFile, C_PIV_START_PAGE);
      break;
    case PgUser:
      TtaWriteByte (pivFile, C_PIV_USER_PAGE);
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
  PutReferenceType ecrit un type de reference dans le fichier sur un	
  octet. S'il s'agit d'une inclusion, expansion indique si c'est	
  une inclusion avec ou sans expansion a l'ecran.                 
  ----------------------------------------------------------------------*/
static void PutReferenceType (BinFile pivFile, ReferenceType t,
                              ThotBool expansion)
{
  switch (t)
    {
    case RefFollow:
      TtaWriteByte (pivFile, C_PIV_REF_FOLLOW);
      break;
    case RefInclusion:
      if (expansion)
        TtaWriteByte (pivFile, C_PIV_REF_INCLUS_EXP);
      else
        TtaWriteByte (pivFile, C_PIV_REF_INCLUSION);
      break;
    }
}

/*----------------------------------------------------------------------
  WriteNatureNumber ecrit une marque de nature suivie du numero du schema    
  de structure pointe par pSS.                            
  ----------------------------------------------------------------------*/
void WriteNatureNumber (PtrSSchema pSS, BinFile pivFile,
                        PtrDocument pDoc)
{
  int                 n;
  ThotBool            stop;

  TtaWriteByte (pivFile, C_PIV_NATURE);
  /* cherche le schema de structure */
  n = 0;
  stop = FALSE;
  do
    if (strcmp (pSS->SsName, pDoc->DocNatureName[n]) == 0)
      stop = TRUE;
    else if (n < pDoc->DocNNatures - 1)
      n++;
    else
      {
        n = 0;
        stop = TRUE;
      }
  while (!stop);
  PutShort (pivFile, n);
}

/*----------------------------------------------------------------------
  PutLabel   ecrit le label label dans le fichier pivFile            
  ----------------------------------------------------------------------*/
void PutLabel (BinFile pivFile, LabelString label)
{
  int              i;

  TtaWriteByte (pivFile, C_PIV_LABEL);
  i = 0;
  do
    TtaWriteByte (pivFile, label[i++]);
  while (label[i - 1] != EOS);
}

/*----------------------------------------------------------------------
  PutReference ecrit dans le fichier pivFile la reference pointee	
  par pRef.                                               
  ----------------------------------------------------------------------*/
static void PutReference (BinFile pivFile, PtrReference pRef)
{
  PtrReferredDescr    pRefD;
  LabelString         label;
  LabelString         labelNul;
  ThotBool            expansion;

  labelNul [0] = 0;
  /* ecrit le type de la reference */
  expansion = FALSE;
  if (pRef->RdElement != NULL)
    expansion = pRef->RdElement->ElSource == pRef;
  PutReferenceType (pivFile, pRef->RdTypeRef, expansion);
  if (pRef->RdReferred == NULL)
    /* la reference ne designe rien, on ecrit un label nul */
    PutLabel (pivFile, labelNul);
  else
    {
      pRefD = pRef->RdReferred;
      /* l'objet designe' est dans le meme document */
      if (pRefD->ReReferredElem == NULL)
        /* pas d'element reference' */
        label[0] = EOS;
      else
        /* cherche si l'element reference' */
        /* est dans le buffer (a la suite d'un Couper). */
        if (IsASavedElement (pRefD->ReReferredElem))
          label[0] = EOS;
        else
          /* label: label de l'element designe' */
          strncpy (label, pRefD->ReReferredElem->ElLabel, MAX_LABEL_LEN);
      /* ecrit le label de l'objet designe' */
      PutLabel (pivFile, label);
    }
}

/*----------------------------------------------------------------------
  PutAttribut ecrit dans le fichier pivFile l'attribut pointe' par  
  pAttr.                                                  
  ----------------------------------------------------------------------*/
void PutAttribut (BinFile pivFile, PtrAttribute pAttr, PtrDocument pDoc)
{
  ThotBool            attrOK;
  ThotBool            stop;
  int                 n, i;
  PtrElement          pEl;
  PtrTextBuffer       pBuf;

  attrOK = TRUE;
  if (pAttr->AeDefAttr
      && pAttr->AeAttrType != AtReferenceAttr)
    /* on n'ecrit pas les attributs fixes, sauf les references */
    attrOK = FALSE;
  else if (pAttr->AeAttrType == AtReferenceAttr)
    /* c'est un attribut reference, on n'ecrit pas */
    /* les attributs references qui pointent sur rien. */
    {
      if (pAttr->AeAttrReference == NULL)
        attrOK = FALSE;
      else if (pAttr->AeAttrReference->RdReferred == NULL)
        attrOK = FALSE;
      else
        {
          pEl = ReferredElement (pAttr->AeAttrReference);
          if (pEl == NULL)
            attrOK = FALSE;
          else if (IsASavedElement (pEl))
            attrOK = FALSE;
        }
    }
  if (attrOK)
    /* cherche le schema de structure ou est defini l'attribut */
    {
      n = 0;
      stop = FALSE;
      do
        if (strcmp (pAttr->AeAttrSSchema->SsName,
                    pDoc->DocNatureName[n]) == 0)
          stop = TRUE;
        else if (n < pDoc->DocNNatures - 1)
          n++;
        else
          {
            n = 0;
            stop = TRUE;
          }
      while (!stop);
      TtaWriteByte (pivFile, C_PIV_ATTR);
      PutShort (pivFile, n);
      /* numero de la nature de l'attribut */
      PutShort (pivFile, pAttr->AeAttrNum);
      /* numero de l'attribut */
      switch (pAttr->AeAttrType)
        {
        case AtEnumAttr:
          PutShort (pivFile, pAttr->AeAttrValue);
          /* valeur de cet attribut */
          break;
        case AtNumAttr:
          PutShort (pivFile, abs (pAttr->AeAttrValue));
          PutSign (pivFile, (ThotBool)(pAttr->AeAttrValue >= 0));
          break;
        case AtReferenceAttr:
          PutReference (pivFile, pAttr->AeAttrReference);
          break;
        case AtTextAttr:
          pBuf = pAttr->AeAttrText;
          while (pBuf != NULL)
            {
              i = 0;
              while (pBuf->BuContent[i] != EOS)
                TtaWriteWideChar (pivFile, pBuf->BuContent[i++]);
              pBuf = pBuf->BuNext;
            }
          TtaWriteByte (pivFile, EOS);
          break;
        default:
          break;
        }
    }
}

/*----------------------------------------------------------------------
  PutPresRule writes in file pivFile the specific presentation rule pPRule.
  The parameter isPicture is True if the element is a picture element.
  ----------------------------------------------------------------------*/
static void   PutPresRule (BinFile pivFile, PtrPRule pPRule)
{
  PRuleType          rType;
  unsigned short     red, green, blue;

  rType = pPRule->PrType;
  /* on ne traite que les regles de presentation directes (ni heritage */
  /* ni fonction de presentation) dont le codage pivot est defini */
  /* On traite toutefois les regles qui expriment une distance en terme
     de pourcentage du parent */
  if ((pPRule->PrPresMode == PresImmediate &&
       (rType == PtHeight || rType == PtWidth ||
        rType == PtVertPos || rType == PtHorizPos ||
        rType == PtMarginTop || rType == PtMarginRight ||
        rType == PtMarginBottom || rType == PtMarginLeft ||
        rType == PtPaddingTop || rType == PtPaddingRight ||
        rType == PtPaddingBottom || rType == PtPaddingLeft ||
        rType == PtBorderTopWidth ||
        rType == PtBorderRightWidth ||
        rType == PtBorderBottomWidth ||
        rType == PtBorderLeftWidth ||
        rType == PtXRadius || rType == PtYRadius ||
        rType == PtTop || rType == PtRight || rType == PtBottom ||
        rType == PtLeft ||
        rType == PtBackgroundHorizPos || rType ==  PtBackgroundVertPos ||
        rType == PtBorderTopColor ||
        rType == PtBorderRightColor ||
        rType == PtBorderBottomColor ||
        rType == PtBorderLeftColor ||
        rType == PtBorderTopStyle ||
        rType == PtBorderRightStyle ||
        rType == PtBorderBottomStyle ||
        rType == PtBorderLeftStyle ||
        rType == PtDisplay ||
        rType == PtListStyleType ||
        rType == PtListStyleImage ||
        rType == PtListStylePosition ||
        rType == PtFloat ||
        rType == PtClear ||
        rType == PtPosition ||
        rType == PtVisibility ||
        rType == PtVis ||
        rType == PtSize || rType == PtStyle ||
        rType == PtWeight || rType == PtVariant || rType == PtFont ||
        rType == PtUnderline || rType == PtThickness ||
        rType == PtIndent || rType == PtLineSpacing ||
        rType == PtDepth ||
        rType == PtAdjust ||
        rType == PtDirection || rType == PtUnicodeBidi ||
        rType == PtLineStyle || rType == PtLineWeight ||
        rType == PtFillPattern || rType == PtOpacity || 
        rType == PtFillOpacity || rType == PtStrokeOpacity ||
        rType == PtStopOpacity || rType == PtFillRule ||
	rType == PtMarker || rType == PtMarkerStart ||
	rType == PtMarkerMid || rType == PtMarkerEnd ||
        rType == PtBackground || rType == PtForeground ||
	rType == PtColor || rType == PtStopColor ||
        rType == PtHyphenate ||
        rType == PtBreak1 || rType == PtBreak2 ||
        rType == PtPictInfo)) ||
      (pPRule->PrPresMode == PresInherit &&
       pPRule->PrInheritMode == InheritParent &&
       pPRule->PrInhPercent && !pPRule->PrInhAttr &&
       pPRule->PrInhMinOrMax == 0 &&
       (rType == PtBreak1 || rType == PtBreak2 ||
        rType == PtIndent || rType == PtSize ||
        rType == PtLineSpacing || rType == PtLineWeight ||
        rType == PtMarginTop || rType == PtMarginRight ||
        rType == PtMarginBottom || rType == PtMarginLeft ||
        rType == PtPaddingTop || rType == PtPaddingRight ||
        rType == PtPaddingBottom || rType == PtPaddingLeft ||
        rType == PtBorderTopWidth || rType == PtBorderRightWidth ||
        rType == PtBorderBottomWidth || rType == PtBorderLeftWidth ||
        rType == PtXRadius || rType == PtYRadius ||
        rType == PtTop || rType == PtRight || rType == PtBottom ||
        rType == PtLeft ||
        rType == PtBackgroundHorizPos || rType == PtBackgroundVertPos)))
    {
      /* ecrit la marque de regle */
      TtaWriteByte (pivFile, (char) C_PIV_PRESENT);
      /* ecrit le numero de vue */
      PutShort (pivFile, pPRule->PrViewNum);
      /* ecrit la specificite et l'importance */
      PutShort (pivFile, pPRule->PrSpecificity);
      PutBoolean (pivFile, pPRule->PrImportant);
      /* ecrit le numero de la boite de presentation concernee */
      PutShort (pivFile, 0);
      /* ecrit le type de la regle */
      switch (rType)
        {
        case PtAdjust:
          /* mode de mise en ligne */
          TtaWriteByte (pivFile, C_PR_ADJUST);
          break;
        case PtHeight:
        case PtWidth:
          if (pPRule->PrDimRule.DrPosition)
            {
              if (pPRule->PrType == PtHeight)
                TtaWriteByte (pivFile, C_PR_HEIGHTPOS);
              else
                TtaWriteByte (pivFile, C_PR_WIDTHPOS);
            }
          else
            {
              if (pPRule->PrType == PtHeight)
                TtaWriteByte (pivFile, C_PR_HEIGHT);
              else
                TtaWriteByte (pivFile, C_PR_WIDTH);
            }
          break;
        case PtVertPos:
          TtaWriteByte (pivFile, C_PR_VPOS);
          break;
        case PtHorizPos:
          TtaWriteByte (pivFile, C_PR_HPOS);
          break;
        case PtFont:
          TtaWriteByte (pivFile, C_PR_FONT);
          break;
        case PtStyle:
          TtaWriteByte (pivFile, C_PR_STYLE);
          break;
        case PtWeight:
          TtaWriteByte (pivFile, C_PR_WEIGHT);
          break;
        case PtVariant:
          TtaWriteByte (pivFile, C_PR_VARIANT);
          break;
        case PtUnderline:
          TtaWriteByte (pivFile, C_PR_UNDERLINE);
          break;
        case PtThickness:
          TtaWriteByte (pivFile, C_PR_UNDER_THICK);
          break;
        case PtDirection:
          TtaWriteByte (pivFile, C_PR_DIRECTION);
          break;
        case PtUnicodeBidi:
          TtaWriteByte (pivFile, C_PR_UNICODEBIDI);
          break;
        case PtLineStyle:
          TtaWriteByte (pivFile, C_PR_LINESTYLE);
          break;
        case PtDisplay:
          TtaWriteByte (pivFile, C_PR_DISPLAY);
          break;
        case PtListStyleType:
          TtaWriteByte (pivFile, C_PR_LISTSTYLETYPE);
          break;
        case PtListStyleImage:
          TtaWriteByte (pivFile, C_PR_LISTSTYLEIMAGE);
          break;
        case PtListStylePosition:
          TtaWriteByte (pivFile, C_PR_LISTSTYLEPOSITION);
          break;
        case PtFloat:
          TtaWriteByte (pivFile, C_PR_FLOAT);
          break;
        case PtClear:
          TtaWriteByte (pivFile, C_PR_CLEAR);
          break;
        case PtPosition:
          TtaWriteByte (pivFile, C_PR_POSITION);
          break;
        case PtVisibility:
          TtaWriteByte (pivFile, C_PR_VISIBILITY);
          break;
        case PtVis:
          TtaWriteByte (pivFile, C_PR_VIS);
          break;
        case PtBorderTopStyle:
          TtaWriteByte (pivFile, C_PR_BORDERTOPSTYLE);
          break;
        case PtBorderRightStyle:
          TtaWriteByte (pivFile, C_PR_BORDERRIGHTSTYLE);
          break;
        case PtBorderBottomStyle:
          TtaWriteByte (pivFile, C_PR_BORDERBOTTOMSTYLE);
          break;
        case PtBorderLeftStyle:
          TtaWriteByte (pivFile, C_PR_BORDERLEFTSTYLE);
          break;
        case PtBreak1:
          TtaWriteByte (pivFile, C_PR_BREAK1);
          break;
        case PtBreak2:
          TtaWriteByte (pivFile, C_PR_BREAK2);
          break;
        case PtIndent:
          TtaWriteByte (pivFile, C_PR_INDENT);
          break;
        case PtSize:
          TtaWriteByte (pivFile, C_PR_SIZE);
          break;
        case PtLineSpacing:
          TtaWriteByte (pivFile, C_PR_LINESPACING);
          break;
        case PtLineWeight:
          TtaWriteByte (pivFile, C_PR_LINEWEIGHT);
          break;
        case PtMarginTop:
          TtaWriteByte (pivFile, C_PR_MARGINTOP);
          break;
        case PtMarginRight:
          TtaWriteByte (pivFile, C_PR_MARGINRIGHT);
          break;
        case PtMarginBottom:
          TtaWriteByte (pivFile, C_PR_MARGINBOTTOM);
          break;
        case PtMarginLeft:
          TtaWriteByte (pivFile, C_PR_MARGINLEFT);
          break;
        case PtPaddingTop:
          TtaWriteByte (pivFile, C_PR_PADDINGTOP);
          break;
        case PtPaddingRight:
          TtaWriteByte (pivFile, C_PR_PADDINGRIGHT);
          break;
        case PtPaddingBottom:
          TtaWriteByte (pivFile, C_PR_PADDINGBOTTOM);
          break;
        case PtPaddingLeft:
          TtaWriteByte (pivFile, C_PR_PADDINGLEFT);
          break;
        case PtBorderTopWidth:
          TtaWriteByte (pivFile, C_PR_BORDERTOPWIDTH);
          break;
        case PtBorderRightWidth:
          TtaWriteByte (pivFile, C_PR_BORDERRIGHTWIDTH);
          break;
        case PtBorderBottomWidth:
          TtaWriteByte (pivFile, C_PR_BORDERBOTTOMWIDTH);
          break;
        case PtBorderLeftWidth:
          TtaWriteByte (pivFile, C_PR_BORDERLEFTWIDTH);
          break;
        case PtXRadius:
          TtaWriteByte (pivFile, C_PR_XRADIUS);
          break;
        case PtYRadius:
          TtaWriteByte (pivFile, C_PR_YRADIUS);
          break;
        case PtTop:
          TtaWriteByte (pivFile, C_PR_TOP);
          break;
        case PtRight:
          TtaWriteByte (pivFile, C_PR_RIGHT);
          break;
        case PtBottom:
          TtaWriteByte (pivFile, C_PR_BOTTOM);
          break;
        case PtLeft:
          TtaWriteByte (pivFile, C_PR_LEFT);
          break;
        case PtBackgroundHorizPos:
          TtaWriteByte (pivFile, C_PR_BACKGROUNDHORIZPOS);
          break;
        case PtBackgroundVertPos:
          TtaWriteByte (pivFile, C_PR_BACKGROUNDVERTPOS);
          break;
        case PtHyphenate:
          TtaWriteByte (pivFile, C_PR_HYPHENATE);
          break;
        case PtDepth:
          TtaWriteByte (pivFile, C_PR_DEPTH);
          break;
        case PtFillPattern:
          TtaWriteByte (pivFile, C_PR_FILLPATTERN);
          break;
        case PtOpacity:
          TtaWriteByte (pivFile, C_PR_OPACITY);
          break;
        case PtFillOpacity:
          TtaWriteByte (pivFile, C_PR_FILL_OPACITY);
          break;
        case PtStrokeOpacity:
          TtaWriteByte (pivFile, C_PR_STROKE_OPACITY);
          break;
        case PtStopOpacity:
          TtaWriteByte (pivFile, C_PR_STOPOPACITY);
          break;
        case PtMarker:
          TtaWriteByte (pivFile, C_PR_MARKER);
          break;
        case PtMarkerStart:
          TtaWriteByte (pivFile, C_PR_MARKERSTART);
          break;
        case PtMarkerMid:
          TtaWriteByte (pivFile, C_PR_MARKERMID);
          break;
        case PtMarkerEnd:
          TtaWriteByte (pivFile, C_PR_MARKEREND);
          break;
        case PtFillRule:
          TtaWriteByte (pivFile, C_PR_FILL_RULE);
          break;
        case PtBackground:
          TtaWriteByte (pivFile, C_PR_BACKGROUND);
          break;
        case PtForeground:
          TtaWriteByte (pivFile, C_PR_FOREGROUND);
          break;
        case PtColor:
          TtaWriteByte (pivFile, C_PR_COLOR);
          break;
        case PtStopColor:
          TtaWriteByte (pivFile, C_PR_STOPCOLOR);
          break;
        case PtBorderTopColor:
          TtaWriteByte (pivFile, C_PR_BORDERTOPCOLOR);
          break;
        case PtBorderRightColor:
          TtaWriteByte (pivFile, C_PR_BORDERRIGHTCOLOR);
          break;
        case PtBorderBottomColor:
          TtaWriteByte (pivFile, C_PR_BORDERBOTTOMCOLOR);
          break;
        case PtBorderLeftColor:
          TtaWriteByte (pivFile, C_PR_BORDERLEFTCOLOR);
          break;
        default:
          printf ("\nERROR: wrong pres rule *****\n");
          break;
        }
      /* ecrit les parametres de la regle */
      switch (rType)
        {
        case PtAdjust:
          /* mode de mise en ligne */
          PutAlignment (pivFile, pPRule->PrAdjust);
          break;
        case PtHeight:
        case PtWidth:
          if (pPRule->PrDimRule.DrPosition)
            {
              PutShort (pivFile, pPRule->PrDimRule.DrPosRule.PoPosDef);
              PutShort (pivFile, pPRule->PrDimRule.DrPosRule.PoPosRef);
              PutShort (pivFile, pPRule->PrDimRule.DrPosRule.PoRelation);
              PutShort (pivFile, abs (pPRule->PrDimRule.DrPosRule.PoDistance));
              PutUnit (pivFile, pPRule->PrDimRule.DrPosRule.PoDistUnit);
              PutSign (pivFile, (ThotBool)(pPRule->PrDimRule.DrPosRule.PoDistance >= 0));
            }
          else
            PutDimension (pivFile, pPRule);
          break;
        case PtVertPos:
        case PtHorizPos:
          PutShort (pivFile, pPRule->PrPosRule.PoPosDef);
          PutShort (pivFile, pPRule->PrPosRule.PoPosRef);
          PutBoolean (pivFile, pPRule->PrPosRule.PoDistAttr);
          PutShort (pivFile, pPRule->PrPosRule.PoRelation);
          PutBoolean (pivFile, pPRule->PrPosRule.PoNotRel);
          PutShort (pivFile, pPRule->PrPosRule.PoRefKind);
          PutShort (pivFile, pPRule->PrPosRule.PoRefIdent);
          PutShort (pivFile, abs (pPRule->PrPosRule.PoDistDelta));
          PutUnit (pivFile, pPRule->PrPosRule.PoDeltaUnit);
          PutSign (pivFile, (ThotBool)(pPRule->PrPosRule.PoDistDelta >= 0));
          PutShort (pivFile, abs (pPRule->PrPosRule.PoDistance));
          PutUnit (pivFile, pPRule->PrPosRule.PoDistUnit);
          PutSign (pivFile, (ThotBool)(pPRule->PrPosRule.PoDistance >= 0));
          break;
        case PtBreak1:
        case PtBreak2:
        case PtSize:
        case PtLineSpacing:
        case PtLineWeight:
        case PtPaddingTop:
        case PtPaddingRight:
        case PtPaddingBottom:
        case PtPaddingLeft:
        case PtBorderTopWidth:
        case PtBorderRightWidth:
        case PtBorderBottomWidth:
        case PtBorderLeftWidth:
        case PtXRadius:
        case PtYRadius:
        case PtTop:
        case PtRight:
        case PtBottom:
        case PtLeft:
        case PtBackgroundHorizPos:
        case PtBackgroundVertPos:
          PutBoolean (pivFile, (pPRule->PrPresMode == PresImmediate));
          if (pPRule->PrPresMode == PresImmediate)
            {
              PutShort (pivFile, pPRule->PrMinValue);
              PutUnit (pivFile, pPRule->PrMinUnit);
            }
          else
            {
              PutShort (pivFile, pPRule->PrInhDelta);
              PutUnit (pivFile, pPRule->PrInhUnit);
            }
          break;
        case PtIndent:
        case PtMarginTop:
        case PtMarginRight:
        case PtMarginBottom:
        case PtMarginLeft:
          PutBoolean (pivFile, (pPRule->PrPresMode == PresImmediate));
          if (pPRule->PrPresMode == PresImmediate)
            {
              PutShort (pivFile, abs (pPRule->PrMinValue));
              PutUnit (pivFile, pPRule->PrMinUnit);
              PutSign (pivFile, (ThotBool)(pPRule->PrMinValue >= 0));
            }
          else
            {
              PutShort (pivFile, abs (pPRule->PrInhDelta));
              PutUnit (pivFile, pPRule->PrInhUnit);
              PutSign (pivFile, (ThotBool)(pPRule->PrInhDelta >= 0));
            }
          break;
        case PtDepth:
        case PtOpacity:
        case PtFillOpacity:
        case PtStrokeOpacity:
        case PtStopOpacity:
        case PtFillPattern:
        case PtListStyleImage:
	case PtMarker:
	case PtMarkerStart:
	case PtMarkerMid:
	case PtMarkerEnd:
          PutShort (pivFile, pPRule->PrIntValue);
          break;
        case PtBackground:
        case PtForeground:
	case PtColor:
	case PtStopColor:
        case PtBorderTopColor:
        case PtBorderRightColor:
        case PtBorderBottomColor:
        case PtBorderLeftColor:
          if (pPRule->PrIntValue >= 0)
            /* positive value */
            PutShort (pivFile, 0);
          else
            /* negative value */
            PutShort (pivFile, 1);
#ifdef NODISPLAY
          red = green = blue = 0;
#else /* NODISPLAY */
          TtaGiveThotRGB (pPRule->PrIntValue, &red, &green, &blue);
#endif /* NODISPLAY */
          PutShort (pivFile, red);
          PutShort (pivFile, green);
          PutShort (pivFile, blue);
          break;
        case PtVis:
        case PtFont:
        case PtStyle:
        case PtWeight:
        case PtVariant:
        case PtUnderline:
        case PtThickness:
        case PtDirection:
        case PtUnicodeBidi:
        case PtVisibility:
        case PtLineStyle:
        case PtDisplay:
        case PtListStyleType:
        case PtListStylePosition:
        case PtFloat:
        case PtClear:
        case PtPosition:
        case PtBorderTopStyle:
        case PtBorderRightStyle:
        case PtBorderBottomStyle:
        case PtBorderLeftStyle:
        case PtFillRule:
          TtaWriteByte (pivFile, pPRule->PrChrValue);
          break;
        case PtHyphenate:
          PutBoolean (pivFile, pPRule->PrBoolValue);
          break;
        default:
          break;
        }
    }
}

/*----------------------------------------------------------------------
  Externalise effectue la traduction sous forme pivot du sous-arbre  
  ou de l'element pointe par pEl.                                 
  subTree indique si on veut ecrire le sous-arbre ou l'element    
  seul.                                                           
  Le fichier 'pivFile' ou est ecrit la representation pivot doit  
  deja etre ouvert a l'appel et il n'est pas ferme au retour.     
  Si l'element externalise est une feuille texte suivie d'autres  
  feuilles de texte ayant les m^emes attributs, ces elements sont 
  externalise's sous la forme d'un seul element, et au retour,    
  pEl pointe sur le dernier de ces elements successifs.           
  ----------------------------------------------------------------------*/
void Externalise (BinFile pivFile, PtrElement *pEl, PtrDocument pDoc,
                  ThotBool subTree)
{
  PtrElement          pChild, pEl1;
  PtrTextBuffer       pBuf;
  PtrPathSeg          pPa;
  PtrAttribute        pAttr;
  PtrPRule            pPRule = NULL;
  PtrSSchema          pSS;
  NotifyElement       notifyEl;
  NotifyAttribute     notifyAttr;
  int                 i, c;
  ThotBool            stop;
  PtrTransform        Trans;

  pEl1 = *pEl;
  /* write the element type */
  TtaWriteByte (pivFile, (char) C_PIV_TYPE);
  /* ecrit le numero de la regle definissant le type */
  PutShort (pivFile, pEl1->ElTypeNumber);
  /* si c'est une copie d'element inclus, ecrit la reference a */
  /* l'element inclus */
  if (pEl1->ElSource != NULL)
    /* ecrit la marque d'element inclus */
    {
      TtaWriteByte (pivFile, (char) C_PIV_INCLUDED);
      PutReference (pivFile, pEl1->ElSource);
    }
  /* ecrit la marque "Element-reference'" si l'element est */
  /* effectivement reference' */
  if (pEl1->ElReferredDescr)
    if (pEl1->ElReferredDescr->ReFirstReference)
      /* l'element est effectivement reference' */
      TtaWriteByte (pivFile, (char) C_PIV_REFERRED);
  /* ecrit le label de l'element */
  PutLabel (pivFile, pEl1->ElLabel);
  
  /* Ecrit la marque d'holophraste si l'element est holophraste' */
  if (pEl1->ElHolophrast)
    TtaWriteByte (pivFile, (char) C_PIV_HOLOPHRAST);
  
  /* ecrit les attributs de l'element, mais pas les attributs imposes, */
  /* a moins qu'ils soient du type reference */
  pAttr = pEl1->ElFirstAttr;
  while (pAttr != NULL)
    {
      /* prepare et envoie l'evenement AttrSave.Pre s'il est demande' */
      notifyAttr.event = TteAttrSave;
      notifyAttr.document = (Document) IdentDocument (pDoc);
      notifyAttr.element = (Element) pEl1;
      notifyAttr.info = 0; /* not sent by undo */
      notifyAttr.attribute = (Attribute) pAttr;
      notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
      notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
      if (!CallEventAttribute (&notifyAttr, TRUE))
        /* l'application laisse l'editeur ecrire l'attribut */
        {
          /* ecrit l'attribut */
          PutAttribut (pivFile, pAttr, pDoc);
          /* prepare et envoie l'evenement AttrSave.Post s'il est demande' */
          notifyAttr.event = TteAttrSave;
          notifyAttr.document = (Document) IdentDocument (pDoc);
          notifyAttr.element = (Element) pEl1;
          notifyAttr.info = 0; /* not sent by undo */
          notifyAttr.attribute = (Attribute) pAttr;
          notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
          notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
          CallEventAttribute (&notifyAttr, FALSE);
        }
      /* passe a l'attribut suivant de l'element */
      pAttr = pAttr->AeNext;
    }

  /*****TO BE CHANGED**** */
  /* write a specific rule for each picture element */
  if (pEl1->ElTerminal && pEl1->ElLeafType == LtPicture && pEl1->ElPictInfo)
    {
      /* write the rule mark */
      TtaWriteByte (pivFile, (char) C_PIV_PRESENT);
      /* write the view number */
      PutShort (pivFile, 1);
      /* write the specificity and the importance of the rule */
      PutShort (pivFile, 0);
      PutBoolean (pivFile, TRUE);
      /* write the presentation box number */
      PutShort (pivFile, 0);
      /* write the specific rule and its parameters */
      TtaWriteByte (pivFile, C_PR_PICTURE);
      PutShort (pivFile, 0);
      PutShort (pivFile, 0);
      switch (((ThotPictInfo *)(pEl1->ElPictInfo))->PicPresent)
        {
        case RealSize:
          TtaWriteByte (pivFile, C_PIV_REALSIZE);
          break;
        case ReScale:
          TtaWriteByte (pivFile, C_PIV_RESCALE);
          break;
        case FillFrame:
          TtaWriteByte (pivFile, C_PIV_FILLFRAME);
          break;
        case XRepeat:
          TtaWriteByte (pivFile, C_PIV_XREPEAT);
          break;
        case YRepeat:
          TtaWriteByte (pivFile, C_PIV_YREPEAT);
          break;
        default:
          TtaWriteByte (pivFile, ' ');
          break;
        }
      PutShort (pivFile, ((ThotPictInfo *)(pEl1->ElPictInfo))->PicType);
    }

  /* ecrit les regles de presentation de l'element */
  pPRule = pEl1->ElFirstPRule;
  while (pPRule != NULL)
    {
      PutPresRule (pivFile, pPRule);
      pPRule = pPRule->PrNextPRule;
    }

  /* ecrit le contenu de l'element */
  pSS = pEl1->ElStructSchema;
  if (pEl1->ElSource == NULL)
    /* on n'ecrit pas le contenu d'un element inclus */
    {
      if (pEl1->ElTerminal)
        {
          /* feuille terminale: on ecrit son contenu entre C_PIV_BEGIN et
             C_PIV_END */
          if (pSS->SsRule->SrElem[pEl1->ElTypeNumber - 1]->SrConstruct != CsConstant)
            /* on n'ecrit pas le texte des constantes, puisqu'il est cree
               automatiquement */
            {
              if (pEl1->ElTypeNumber == CharString + 1)
                /* ecrit le numero de langue de la feuille de texte, si ce */
                /* n'est pas la premiere langue de la table du document */
                {
                  i = 0;
                  /* cherche le rang de la langue dans la table du document */
                  while (pDoc->DocLanguages[i] != pEl1->ElLanguage &&
                         i < pDoc->DocNLanguages)
                    i++;
                  if (i > 0)
                    {
                      TtaWriteByte (pivFile, C_PIV_LANG);
                      TtaWriteByte (pivFile, (char) i);
                    }
                }
              if (pEl1->ElLeafType != LtReference)
                TtaWriteByte (pivFile, (char) C_PIV_BEGIN);
              switch (pEl1->ElLeafType)
                {
                case LtPicture:
                case LtText:
                  /* ecrit dans le fichier le texte des buffers de l'element */
                  /* ou le nom de l'image */
                  do
                    {
                      c = 0;
                      pBuf = pEl1->ElText;
                      while (c < pEl1->ElTextLength && pBuf != NULL)
                        {
                          i = 0;
                          while (pBuf->BuContent[i] != EOS && i < pBuf->BuLength)
                            TtaWriteWideChar (pivFile, pBuf->BuContent[i++]);
                          c = c + i;
                          /* buffer suivant du meme element */
                          pBuf = pBuf->BuNext;
                        }
                      /* peut-on concatener l'element suivant ? */
                      stop = TRUE;
#ifndef NODISPLAY
                      if (pEl1->ElLeafType == LtText && pEl1->ElNext != NULL &&
                          /* c'est du texte, il y a un suivant.. */
                          pEl1->ElNext->ElTerminal &&
                          pEl1->ElNext->ElLeafType == LtText &&
                          /* qui est une feuille de text */
                          pEl1->ElNext->ElLanguage == pEl1->ElLanguage &&
                          pEl1->ElNext->ElSource == NULL &&
                          /* le suivant n'est pas une inclusion */
                          pEl1->ElStructSchema->SsRule->SrElem[pEl1->ElTypeNumber - 1]->SrConstruct != CsConstant &&
                          pEl1->ElNext->ElStructSchema->SsRule->SrElem[pEl1->ElNext->ElTypeNumber - 1]->SrConstruct != CsConstant &&
                          SameAttributes (*pEl, pEl1->ElNext))
                        /* il a les memes attributs */
                        if (BothHaveNoSpecRules (*pEl, pEl1->ElNext))
                          /* il a les memes regles de presentation specifique  */
                          /* on concatene */
                          {
                            stop = FALSE;
                            pEl1 = pEl1->ElNext;
                          }
#endif /* NODISPLAY */
                    }
                  while (!stop);
                  /* update pEl value */
                  *pEl = pEl1;
                  TtaWriteByte (pivFile, EOS);
                  break;
                case LtReference:
                  /* ecrit une marque de reference et le label de */
                  /* l'element qui est reference' */
                  TtaWriteByte (pivFile, (char) C_PIV_REFERENCE);
                  PutReference (pivFile, pEl1->ElReference);
                  break;
                case LtSymbol:
                case LtGraphics:
                  /* ecrit le code du symbole ou du graphique */
                  TtaWriteByte (pivFile, pEl1->ElGraph);
                  break;
                case LtPageColBreak:
                  /* ecrit le numero de page et le type de page */
                  PutShort (pivFile, pEl1->ElPageNumber);
                  PutShort (pivFile, pEl1->ElViewPSchema);
                  PutPageType (pivFile, pEl1->ElPageType);
                  PutBoolean (pivFile, pEl1->ElPageModified);
                  break;
                case LtPairedElem:
                  PutInteger (pivFile, pEl1->ElPairIdent);
                  break;
                case LtPolyLine:
                  /* ecrit le code representant la forme de la ligne */
                  TtaWriteByte (pivFile, pEl1->ElPolyLineType);
                  /* ecrit une marque indiquant que c'est une Polyline */
                  TtaWriteByte (pivFile, (char) C_PIV_POLYLINE);
                  /* ecrit le nombre de points de la ligne */
                  PutShort (pivFile, pEl1->ElNPoints);
                  /* ecrit tous les points */
                  c = 0;
                  pBuf = pEl1->ElPolyLineBuffer;
                  while (c < pEl1->ElNPoints && pBuf != NULL)
                    {
                      for (i = 0; i < pBuf->BuLength; i++)
                        {
                          PutInteger (pivFile, pBuf->BuPoints[i].XCoord);
                          PutInteger (pivFile, pBuf->BuPoints[i].YCoord);
                        }
                      c += pBuf->BuLength;
                      pBuf = pBuf->BuNext; /* buffer suivant du meme element */
                    }
                  break;
                case LtPath:
                  /* ecrit un caractere pour faciliter la lecture */
                  TtaWriteByte (pivFile, ' ');
                  /* ecrit une marque indiquant que c'est un Path */
                  TtaWriteByte (pivFile, (char) C_PIV_PATH);
                  pPa = pEl1->ElFirstPathSeg;
                  while (pPa)
                    {
                      switch (pPa->PaShape)
                        {
                        case PtLine:
                          TtaWriteByte (pivFile, 'L');
                          break;
                        case PtCubicBezier:
                          TtaWriteByte (pivFile, 'C');
                          break;
                        case PtQuadraticBezier:
                          TtaWriteByte (pivFile, 'Q');
                          break;
                        case PtEllipticalArc:
                          TtaWriteByte (pivFile, 'A');
                          break;
                        }
                      if (pPa->PaShape == PtLine || pPa->PaShape == PtCubicBezier ||
                          pPa->PaShape == PtQuadraticBezier || pPa->PaShape == PtEllipticalArc)
                        {
                          PutBoolean (pivFile, pPa->PaNewSubpath);
                          PutSign (pivFile, (ThotBool)(pPa->XStart >= 0));
                          PutInteger (pivFile, abs (pPa->XStart));
                          PutSign (pivFile, (ThotBool)(pPa->YStart >= 0));
                          PutInteger (pivFile, abs (pPa->YStart));
                          PutSign (pivFile, (ThotBool)(pPa->XEnd >= 0));
                          PutInteger (pivFile, abs (pPa->XEnd));
                          PutSign (pivFile, (ThotBool)(pPa->YEnd >= 0));
                          PutInteger (pivFile, abs (pPa->YEnd));
                        }
                      switch (pPa->PaShape)
                        {
                        case PtLine:
                          break;
                        case PtCubicBezier:
                          PutSign (pivFile, (ThotBool)(pPa->XCtrlStart >= 0));
                          PutInteger (pivFile, abs (pPa->XCtrlStart));
                          PutSign (pivFile, (ThotBool)(pPa->YCtrlStart >= 0));
                          PutInteger (pivFile, abs (pPa->YCtrlStart));
                          PutSign (pivFile, (ThotBool)(pPa->XCtrlEnd >= 0));
                          PutInteger (pivFile, abs (pPa->XCtrlEnd));
                          PutSign (pivFile, (ThotBool)(pPa->YCtrlEnd >= 0));
                          PutInteger (pivFile, abs (pPa->YCtrlEnd));
                          break;
                        case PtQuadraticBezier:
                          PutSign (pivFile, (ThotBool)(pPa->XCtrlStart >= 0));
                          PutInteger (pivFile, abs (pPa->XCtrlStart));
                          PutSign (pivFile, (ThotBool)(pPa->YCtrlStart >= 0));
                          PutInteger (pivFile, abs (pPa->YCtrlStart));
                          break;
                        case PtEllipticalArc:
                          PutInteger (pivFile, pPa->XRadius);
                          PutInteger (pivFile, pPa->YRadius);
                          PutSign (pivFile, (ThotBool)(pPa->XAxisRotation >= 0));
                          PutShort (pivFile, abs (pPa->XAxisRotation));
                          PutBoolean (pivFile, pPa->LargeArc);
                          PutBoolean (pivFile, pPa->Sweep);
                          break;
                        }
                      pPa = pPa->PaNext; 
                    } 
                  break;
                default:
                  break;
                }
              /*Add transformation informations in pivot file*/
              if (pEl1->ElParent && pEl1->ElParent->ElTransform)
                {
                  Trans = pEl1->ElParent->ElTransform;
                  while (Trans)
                    {
                      TtaWriteByte (pivFile, (char) C_PIV_TRANS_START);
                      PutInteger (pivFile, Trans->TransType);
                      switch (Trans->TransType)
                        {
                        case PtElBoxTranslate:
                        case PtElScale:
                        case PtElAnimTranslate:
                        case PtElTranslate:
                          PutFloat (pivFile, Trans->XScale);  
                          PutFloat (pivFile, Trans->YScale); 		
                          break;
                        case PtElAnimRotate:
                        case PtElRotate:
                          PutFloat (pivFile, Trans->XRotate);
                          PutFloat (pivFile, Trans->YRotate);
                          PutFloat (pivFile, Trans->TrAngle);	    
                          break;
                        case PtElMatrix:
                          PutFloat (pivFile, Trans->AMatrix);
                          PutFloat (pivFile, Trans->BMatrix);
                          PutFloat (pivFile, Trans->CMatrix);
                          PutFloat (pivFile, Trans->DMatrix);
                          PutFloat (pivFile, Trans->EMatrix);
                          PutFloat (pivFile, Trans->FMatrix);
                          break;
                        case PtElSkewX:
                        case PtElSkewY:
                          PutFloat (pivFile, Trans->TrFactor);
                          break;	  
                        case PtElViewBox:
                          PutFloat (pivFile, Trans->VbXTranslate);
                          PutFloat (pivFile, Trans->VbYTranslate);
                          PutFloat (pivFile, Trans->VbWidth);
                          PutFloat (pivFile, Trans->VbHeight);
                          PutInteger (pivFile, Trans->VbAspectRatio);
                          PutInteger (pivFile, Trans->VbMeetOrSlice);
                          break;
                        default:
                          break;	  
                        }
                      Trans = Trans->Next;
                    }
                  TtaWriteByte (pivFile, (char) C_PIV_TRANS_END);
                }

              if (pEl1->ElLeafType != LtReference)
                TtaWriteByte (pivFile, (char) C_PIV_END);
            }
        }
      else if (subTree && pSS && strcmp (pSS->SsName, "SVG"))
        {
          /* write the subtree of the element */
          // for the moment don't export SVG elements
          // in the future a picture should be generated

          /* ecrit une marque de debut */
          TtaWriteByte (pivFile, (char) C_PIV_BEGIN);
          pChild = pEl1->ElFirstChild;
          /* ecrit successivement la representation pivot de tous */
          /* les fils de l'element */
          while (pChild != NULL)
            {
              /* envoie l'evenement ElemSave.Pre a l'application, si */
              /* elle le demande */
              notifyEl.event = TteElemSave;
              notifyEl.document = (Document) IdentDocument (pDoc);
              notifyEl.element = (Element) pChild;
              notifyEl.info = 0; /* not sent by undo */
              notifyEl.elementType.ElTypeNum = pChild->ElTypeNumber;
              notifyEl.elementType.ElSSchema = (SSchema) (pChild->ElStructSchema);
              notifyEl.position = 0;
              if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
                /* l'application accepte que Thot sauve l'element */
                {
                  /* Ecrit d'abord le numero de la structure generique */
                  /* s'il y a changement de schema de structure par */
                  /* rapport au pere */
                  if (pEl1->ElStructSchema != pChild->ElStructSchema)
                    WriteNatureNumber (pChild->ElStructSchema, pivFile,pDoc);
                  /* Ecrit un element fils */
                  Externalise (pivFile, &pChild, pDoc, subTree);
                  /* envoie l'evenement ElemSave.Post a l'application, si*/
                  /* elle le demande */
                  notifyEl.event = TteElemSave;
                  notifyEl.document = (Document) IdentDocument (pDoc);
                  notifyEl.element = (Element) pChild;
                  notifyEl.info = 0; /* not sent by undo */
                  notifyEl.elementType.ElTypeNum = pChild->ElTypeNumber;
                  notifyEl.elementType.ElSSchema = (SSchema) (pChild->ElStructSchema);
                  notifyEl.position = 0;
                  CallEventType ((NotifyEvent *) & notifyEl, FALSE);
                }
              /* passe au fils suivant */
              pChild = pChild->ElNext;
            }
          /* ecrit une marque de fin */
          TtaWriteByte (pivFile, (char) C_PIV_END);
        }
    }
}

/*----------------------------------------------------------------------
  PutString
 ----------------------------------------------------------------------*/
static void PutString (BinFile pivFile, char* s)
{
  int                 j;

  for (j = 0; s[j] != EOS; j++)
    TtaWriteByte (pivFile, s[j]);
  TtaWriteByte (pivFile, EOS);
}

/*----------------------------------------------------------------------
  WriteSchemaNamesOfDoc ecrit dans le fichier pivFile les noms de tous 
  les schemas de structure et de presentation utilises par le     
  document pDoc.                                                  
  ----------------------------------------------------------------------*/
void WriteSchemaNamesOfDoc (BinFile pivFile, PtrDocument pDoc)
{
#ifndef NODISPLAY
  int                 nat;

  BuildDocNatureTable (pDoc);
  /* ecrit les noms des natures utilisees dans le document. */
  for (nat = 0; nat < pDoc->DocNNatures; nat++)
    {
      /* ecrit la marque de classe ou d'extension */
      if (pDoc->DocNatureSSchema[nat]->SsExtension)
        TtaWriteByte (pivFile, (char) C_PIV_SSCHEMA_EXT);
      else
        TtaWriteByte (pivFile, (char) C_PIV_NATURE);
      /* ecrit le nom de schema de structure dans le fichier */
      PutString (pivFile, pDoc->DocNatureSSchema[nat]->SsName);
      /* ecrit le code du schema de structure */
      PutShort (pivFile, pDoc->DocNatureSSchema[nat]->SsCode);
      /* ecrit le nom du schema de presentation associe' */
      PutString (pivFile, pDoc->DocNatureSSchema[nat]->SsDefaultPSchema);
    }
#endif /* NODISPLAY */
}

/*----------------------------------------------------------------------
  UpdateLanguageTable met dans la table des langues du document pDoc 
  toutes les langues utilisees dans l'arbre de racine pEl et qui  
  ne sont pas encore dans la table.				
  ----------------------------------------------------------------------*/
static void UpdateLanguageTable (PtrDocument pDoc, PtrElement pEl)
{
  int                 i;
  ThotBool            found;

  while (pEl != NULL)
    {
      pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL, NULL);
      if (pEl != NULL)
        {
          found = FALSE;
          for (i = 0; i < pDoc->DocNLanguages && !found; i++)
            if (pEl->ElLanguage == pDoc->DocLanguages[i])
              found = TRUE;
          if (!found)
            if (pDoc->DocNLanguages < MAX_LANGUAGES_DOC - 1)
              pDoc->DocLanguages[pDoc->DocNLanguages++] = pEl->ElLanguage;
        }
    }
}

/*----------------------------------------------------------------------
  WriteTableLangues	ecrit dans le fichier pivFile la table des	
  langues utilisees par le document pDoc.			
  ----------------------------------------------------------------------*/
void WriteTableLangues (BinFile pivFile, PtrDocument pDoc)
{
  int                 i;

  pDoc->DocNLanguages = 0;
  UpdateLanguageTable (pDoc, pDoc->DocDocElement);
  for (i = 0; i < pDoc->DocNLanguages; i++)
    {
      TtaWriteByte (pivFile, (char) C_PIV_LANG);
      PutString (pivFile, TtaGetLanguageCode (pDoc->DocLanguages[i]));
    }
}


/*----------------------------------------------------------------------
  WritePivotHeader ecrit l'entete d'un fichier pivot                 
  ----------------------------------------------------------------------*/
void WritePivotHeader (BinFile pivFile, PtrDocument pDoc)
{
  LabelString         label;

  /* ecrit le numero de version */
  WriteVersionNumber (pivFile);
  /* ecrit la valeur max. des labels */
  ConvertIntToLabel (GetCurrentLabel (pDoc), label);
  PutLabel (pivFile, label);
  /* ecrit la table des langues utilisees par le document */
  WriteTableLangues (pivFile, pDoc);
}


/*----------------------------------------------------------------------
  SauveDoc	sauve le document pDoc dans le fichier pivFile, sous	
  la forme pivot. Le fichier doit etre ouvert avant l'appel et est	
  toujours ouvert au retour.						
  ----------------------------------------------------------------------*/
void SauveDoc (BinFile pivFile, PtrDocument pDoc)
{
  PtrElement          pEl;
  NotifyElement       notifyEl;

  /* ecrit l'entete du fichier pivot */
  WritePivotHeader (pivFile, pDoc);
  /* ecrit les noms de tous les schemas de structure et de presentation */
  /* utilises par le document */
  WriteSchemaNamesOfDoc (pivFile, pDoc);

  /* ecrit la representation pivot de tout le corps du document */
  pEl = pDoc->DocDocElement;
  if (pEl != NULL)
    {
      /* envoie l'evenement ElemSave.Pre a l'application, si */
      /* elle le demande */
      notifyEl.event = TteElemSave;
      notifyEl.document = (Document) IdentDocument (pDoc);
      notifyEl.element = (Element) pEl;
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
      notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
      notifyEl.position = 0;
      if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
        /* l'application accepte que Thot sauve l'element */
        {
          TtaWriteByte (pivFile, (char) C_PIV_STRUCTURE);
          /* ecrit la forme pivot de tout l'arbre */
          Externalise (pivFile, &pEl, pDoc, TRUE);
          /* envoie l'evenement ElemSave.Post a l'application, si */
          /* elle le demande */
          notifyEl.event = TteElemSave;
          notifyEl.document = (Document) IdentDocument (pDoc);
          notifyEl.element = (Element) pEl;
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
          notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
          notifyEl.position = 0;
          CallEventType ((NotifyEvent *) & notifyEl, FALSE);
        }
    }
  TtaWriteByte (pivFile, (char) C_PIV_DOC_END);
}

/*----------------------------------------------------------------------
  TtaRemoveDocument

  Closes a document, releases all ressources allocated to that document,
  removes all files related to the document and updates all links connecting
  the removed document with other documents.

  Parameter:
  document: the document to be removed.
  ----------------------------------------------------------------------*/
void TtaRemoveDocument (Document document)
{
  PtrDocument         pDoc;
  int                 i;
  PathBuffer          DirectoryOrig;
  char                text[MAX_TXT_LEN];

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    {
      pDoc = LoadedDocument[document - 1];
      /* destroys files .PIV, and .BAK of the document */
      strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
      FindCompleteName (pDoc->DocDName, "PIV", DirectoryOrig, text, &i);
      TtaFileUnlink (text);
      strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
      FindCompleteName (pDoc->DocDName, "BAK", DirectoryOrig, text, &i);
      /* now close the document */
      TtaCloseDocument (document);
    }
}
