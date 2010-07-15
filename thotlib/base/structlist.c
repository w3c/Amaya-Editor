/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Dump internal structures
 *
 * Authors: V. Quint, I. Vatton (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"

#include "language.h"
#include "application.h"
#include "libmsg.h"
#include "message.h"
#include "fileaccess.h"
#include "style.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "boxes_tv.h"

static PtrSSchema   pSchemaStr;
static PtrPSchema   pSc1;
static int          DisplayedRuleCounter;

#include "absboxes_f.h"
#include "boxrelations_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "exceptions_f.h"
#include "presrules_f.h"
#include "schemas_f.h"
#include "selectionapi_f.h"
#include "structlist_f.h"
#include "style_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "viewapi_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/
/*----------------------------------------------------------------------
  WrPRuleType ecrit le type de la regle de presentation       
  pointee par pRule, dans le fichier fileDescriptor.            
  ----------------------------------------------------------------------*/
static void WrPRuleType (PtrPRule pRule, FILE * fileDescriptor)
{
  switch (pRule->PrType)
    {
    case PtVisibility:
      fprintf (fileDescriptor, "Visibility");
      break;
    case PtFunction:
      switch (pRule->PrPresFunction)
        {
        case FnLine:
          fprintf (fileDescriptor, "Line");
          break;
        case FnPage:
          fprintf (fileDescriptor, "Page");
          break;
        case FnCreateBefore:
          fprintf (fileDescriptor, "CreateBefore");
          break;
        case FnCreateWith:
          fprintf (fileDescriptor, "CreateWith");
          break;
        case FnCreateFirst:
          fprintf (fileDescriptor, "CreateFirst");
          break;
        case FnCreateLast:
          fprintf (fileDescriptor, "CreateLast");
          break;
        case FnCreateAfter:
          fprintf (fileDescriptor, "CreateAfter");
          break;
        case FnColumn:
          fprintf (fileDescriptor, "Column");
          break;
        case FnCopy:
          fprintf (fileDescriptor, "Copy");
          break;
        case FnContentRef:
          fprintf (fileDescriptor, "ContentRef");
          break;
        case FnSubColumn:
          fprintf (fileDescriptor, "SubColumn");
          break;
        case FnNoLine:
          fprintf (fileDescriptor, "NoLine");
          break;
        case FnCreateEnclosing:
          fprintf (fileDescriptor, "CreateEnclosing");
          break;
        case FnShowBox:
          fprintf (fileDescriptor, "ShowBox");
          break;
        case FnBackgroundPicture:
          fprintf (fileDescriptor, "BackgroundPicture");
          break;
        case FnBackgroundRepeat:
          fprintf (fileDescriptor, "BackgroundRepeat");
          break;
        case FnNotInLine:
          fprintf (fileDescriptor, "InLine: No");
          break;
        default:
          fprintf (fileDescriptor, "PrPresFunction ????");
          break;
        }
      break;
    case PtVertOverflow:
      fprintf (fileDescriptor, "VertOverflow");
      break;
    case PtHorizOverflow:
      fprintf (fileDescriptor, "HorizOverflow");
      break;
    case PtVertRef:
      fprintf (fileDescriptor, "VRef");
      break;
    case PtHorizRef:
      fprintf (fileDescriptor, "HRef");
      break;
    case PtHeight:
      fprintf (fileDescriptor, "Height");
      break;
    case PtWidth:
      fprintf (fileDescriptor, "Width");
      break;
    case PtVertPos:
      fprintf (fileDescriptor, "VPos");
      break;
    case PtHorizPos:
      fprintf (fileDescriptor, "HPos");
      break;
    case PtMarginTop:
      fprintf (fileDescriptor, "MarginTop");
      break;
    case PtMarginRight:
      fprintf (fileDescriptor, "MarginRight");
      break;
    case PtMarginBottom:
      fprintf (fileDescriptor, "MarginBottom");
      break;
    case PtMarginLeft:
      fprintf (fileDescriptor, "MarginLeft");
      break;
    case PtPaddingTop:
      fprintf (fileDescriptor, "PaddingTop");
      break;
    case PtPaddingRight:
      fprintf (fileDescriptor, "PaddingRight");
      break;
    case PtPaddingBottom:
      fprintf (fileDescriptor, "PaddingBottom");
      break;
    case PtPaddingLeft:
      fprintf (fileDescriptor, "PaddingLeft");
      break;
    case PtBorderTopWidth:
      fprintf (fileDescriptor, "BorderTopWidth");
      break;
    case PtBorderRightWidth:
      fprintf (fileDescriptor, "BorderRightWidth");
      break;
    case PtBorderBottomWidth:
      fprintf (fileDescriptor, "BorderBottomWidth");
      break;
    case PtBorderLeftWidth:
      fprintf (fileDescriptor, "BorderLeftWidth");
      break;
    case PtBorderTopColor:
      fprintf (fileDescriptor, "BorderTopColor");
      break;
    case PtBorderRightColor:
      fprintf (fileDescriptor, "BorderRightColor");
      break;
    case PtBorderBottomColor:
      fprintf (fileDescriptor, "BorderBottomColor");
      break;
    case PtBorderLeftColor:
      fprintf (fileDescriptor, "BorderLeftColor");
      break;
    case PtBorderTopStyle:
      fprintf (fileDescriptor, "BorderTopStyle");
      break;
    case PtBorderRightStyle:
      fprintf (fileDescriptor, "BorderRightStyle");
      break;
    case PtBorderBottomStyle:
      fprintf (fileDescriptor, "BorderBottomStyle");
      break;
    case PtBorderLeftStyle:
      fprintf (fileDescriptor, "BorderLeftStyle");
      break;
    case PtSize:
      fprintf (fileDescriptor, "Size");
      break;
    case PtStyle:
      fprintf (fileDescriptor, "Style");
      break;
    case PtWeight:
      fprintf (fileDescriptor, "Weight");
      break;
    case PtVariant:
      fprintf (fileDescriptor, "Variant");
      break;
    case PtFont:
      fprintf (fileDescriptor, "Font");
      break;
    case PtUnderline:
      fprintf (fileDescriptor, "Underline");
      break;
    case PtThickness:
      fprintf (fileDescriptor, "Thickness");
      break;
    case PtIndent:
      fprintf (fileDescriptor, "Indent");
      break;
    case PtLineSpacing:
      fprintf (fileDescriptor, "InterL");
      break;
    case PtDepth:
      fprintf (fileDescriptor, "Depth");
      break;
    case PtAdjust:
      fprintf (fileDescriptor, "Adjust");
      break;
    case PtDirection:
      fprintf (fileDescriptor, "Direction");
      break;
    case PtUnicodeBidi:
      fprintf (fileDescriptor, "UnicodeBidi");
      break;
    case PtLineStyle:
      fprintf (fileDescriptor, "LineStyle");
      break;
    case PtLineWeight:
      fprintf (fileDescriptor, "LineWeight");
      break;
    case PtFillPattern:
      fprintf (fileDescriptor, "FillPattern");
      break;
    case PtOpacity:
      fprintf (fileDescriptor, "Opacity");
      break;
    case PtFillOpacity:
      fprintf (fileDescriptor, "FillOpacity");
      break;
    case PtStrokeOpacity:
      fprintf (fileDescriptor, "StrokeOpacity");
      break;
    case PtStopOpacity:
      fprintf (fileDescriptor, "StopOpacity");
      break;
    case PtMarker:
      fprintf (fileDescriptor, "Marker");
      break;
    case PtMarkerStart:
      fprintf (fileDescriptor, "MarkerStart");
      break;
    case PtMarkerMid:
      fprintf (fileDescriptor, "MarkerMid");
      break;
    case PtMarkerEnd:
      fprintf (fileDescriptor, "MarkerEnd");
      break;
    case PtFillRule:
      fprintf (fileDescriptor, "FillRule");
      break;
    case PtBackground:
      fprintf (fileDescriptor, "Background");
      break;
    case PtForeground:
      fprintf (fileDescriptor, "Foreground");
      break;
    case PtColor:
      fprintf (fileDescriptor, "Color");
      break;
    case PtStopColor:
      fprintf (fileDescriptor, "StopColor");
      break;
    case PtHyphenate:
      fprintf (fileDescriptor, "Hyphenate");
      break;
    case PtPageBreak:
      fprintf (fileDescriptor, "PageBreak");
      break;
    case PtLineBreak:
      fprintf (fileDescriptor, "LineBreak");
      break;
    case PtGather:
      fprintf (fileDescriptor, "Gather");
      break;
    case PtXRadius:
      fprintf (fileDescriptor, "XRadius");
      break;
    case PtYRadius:
      fprintf (fileDescriptor, "YRadius");
      break;
    case PtTop:
      fprintf (fileDescriptor, "Top");
      break;
    case PtRight:
      fprintf (fileDescriptor, "Right");
      break;
    case PtBottom:
      fprintf (fileDescriptor, "Bottom");
      break;
    case PtLeft:
      fprintf (fileDescriptor, "Left");
      break;
    case PtBackgroundHorizPos:
      fprintf (fileDescriptor, "BackgroundHorizPos");
      break;
    case PtBackgroundVertPos:
      fprintf (fileDescriptor, "BackgroundVertPos");
      break;
    case PtDisplay:
      fprintf (fileDescriptor, "Display");
      break;
    case PtListStyleType:
      fprintf (fileDescriptor, "ListStyleType");
      break;
    case PtListStyleImage:
      fprintf (fileDescriptor, "ListStyleImage");
      break;
    case PtListStylePosition:
      fprintf (fileDescriptor, "ListStylePosition");
      break;
    case PtFloat:
      fprintf (fileDescriptor, "Float");
      break;
    case PtClear:
      fprintf (fileDescriptor, "Clear");
      break;
    case PtPosition:
      fprintf (fileDescriptor, "Position");
      break;
    case PtBreak1:
      fprintf (fileDescriptor, "NoBr1");
      break;
    case PtBreak2:
      fprintf (fileDescriptor, "NoBr2");
      break;
    case PtPictInfo:
      fprintf (fileDescriptor, "PictInfo");
      break;
    default:
      fprintf (fileDescriptor, "PrType ????");
      break;
    }
}

/*----------------------------------------------------------------------
  WrText ecrit dans le fichier fileDescriptor le contenu de la chaine des
  buffers de texte commencant au buffer pointe' par pBT.  
  length: longueur maximum a` ecrire.                         
  ----------------------------------------------------------------------*/
static void WrText (PtrTextBuffer pBT, int ind, int length, FILE *fileDescriptor)
{
  PtrTextBuffer       b;
  int                 i, l;
  unsigned char       mbc[50];
  int                 n;

  l = 0;
  b = pBT;
  i = ind - 1;
  while (b && l < length)
    /* ecrit le contenu du buffer de texte */
    {
      while (i < b->BuLength && b->BuContent[i] != EOS && l < length)
        {
          if (b->BuContent[i] > 255)
            {
              /* display the value */
              mbc[0] = '&';
              mbc[1] = '#';
              mbc[1] = 'x';
              sprintf ((char *)&mbc[2], "%x",
                       (int)b->BuContent[i]);
              n = 0;
              while (mbc[n] != EOS)
                putc (mbc[n++], fileDescriptor);
              putc (';', fileDescriptor);
            }
          else
            putc (b->BuContent[i], fileDescriptor);
          i++;
          l++;
        }
      if (l < length)
        {
          fprintf (fileDescriptor, "|");
          b = b->BuNext;
          i = 0;
          /* buffer de texte suivant du meme element */
        }
    }
}

/*----------------------------------------------------------------------
  WrPath ecrit dans le fichier fileDescriptor le contenu de la chaine des
  segments de path commencant au segment pointe' par pPE.  
  length: longueur maximum a` ecrire.                         
  ----------------------------------------------------------------------*/
static void WrPath (PtrPathSeg pPE, int length, FILE *fileDescriptor)
{
  PtrPathSeg          b;

  b = pPE;
  if (length)
  while (b)
    /* ecrit le contenu du path */
    {
      switch (b->PaShape)
        {
        case PtLine:
          putc ('L', fileDescriptor);
          fprintf (fileDescriptor, "%d,%d %d,%d",
                   b->XStart, b->YStart,
                   b->XEnd, b->YEnd);
          break;
        case PtCubicBezier:
          putc ('C', fileDescriptor);
          fprintf (fileDescriptor, "%d,%d %d,%d %d,%d %d,%d",
                   b->XStart, b->YStart,
                   b->XEnd, b->YEnd,
                   b->XCtrlStart, b->YCtrlStart,
                   b->XCtrlEnd, b->YCtrlEnd);
          break;
        case PtQuadraticBezier:
          putc ('Q', fileDescriptor);
          fprintf (fileDescriptor, "%d,%d %d,%d %d,%d",
                   b->XStart, b->YStart,
                   b->XEnd, b->YEnd,
                   b->XCtrlStart, b->YCtrlStart);
          break;
        case PtEllipticalArc:
          putc ('A', fileDescriptor);
          fprintf (fileDescriptor, "%d,%d %d,%d %d,%d %d ",
                   b->XStart, b->YStart,
                   b->XEnd, b->YEnd,
                   b->XRadius, b->YRadius, b->XAxisRotation);
          if (b->LargeArc)
            putc ('1', fileDescriptor);
          else
            putc ('0', fileDescriptor);
          putc (' ', fileDescriptor);
          if (b->Sweep)
            putc ('1', fileDescriptor);
          else
            putc ('0', fileDescriptor);
          break;
        }
      /* next path segment */
      b = b->PaNext;
    }
}


/*----------------------------------------------------------------------
  wrRef ecrit une reference.                                     
  ----------------------------------------------------------------------*/
static void wrRef (PtrReference pRef, FILE *fileDescriptor)
{
  PtrReferredDescr pDe1;

  switch (pRef->RdTypeRef)
    {
    case RefFollow:
      fprintf (fileDescriptor, "reference");
      break;
    case RefInclusion:
      fprintf (fileDescriptor, "transclusion");
      break;
    default:
      fprintf (fileDescriptor, "RdTypeRef????");
      break;
    }
  if (pRef->RdReferred == NULL)
    fprintf (fileDescriptor, "*RdReferred=NULL*");
  else
    {
      pDe1 = pRef->RdReferred;
      if (pDe1->ReReferredElem == NULL)
        fprintf (fileDescriptor, "ReReferredElem=NULL, ReReferredLabel=%s",
                 pDe1->ReReferredLabel);
      else
        fprintf (fileDescriptor, "%s", pDe1->ReReferredElem->ElLabel);
    }
}

/*----------------------------------------------------------------------
  WrTree ecrit dans le fichier fileDescriptor la representation        
  textuelle indentee du sous-arbre de la representation   
  interne commencant au noeud pointe par pNode, avec      
  l'indentation Indent.                                   
  Si premierfils = true alors on ne traduit recursivement 
  que le premier fils de chaque element.                  
  ----------------------------------------------------------------------*/
static void WrTree (PtrElement pNode, int Indent, FILE *fileDescriptor,
                    ThotBool premierfils)
{
  PtrElement          f;
  PtrAttribute        pAttr;
  PtrPRule            pRule;
  PtrSRule            pRe1;
  PtrTtAttribute      pAttr1;
  char                text[100];
  int                 i;

  if (pNode != NULL)
    {
      /* ecrit les blancs de l'indentation */
      for (i = 1; i <= Indent; i++)
        fprintf (fileDescriptor, " ");
      /* si l'element est la copie par inclusion d'un autre element, */
      /* ecrit la reference a cet autre element */
      if (pNode->ElSource != NULL)
        {
          wrRef (pNode->ElSource, fileDescriptor);
          fprintf (fileDescriptor, "\n");
          for (i = 1; i <= Indent; i++)
            fprintf (fileDescriptor, " ");
        }
      i = 1;
      /* ecrit le nom du type de l'element */
      if (pNode->ElStructSchema == NULL)
        fprintf (fileDescriptor, "*ElStructSchema=NULL*");
      else
        {
          pRe1 = pNode->ElStructSchema->SsRule->SrElem[pNode->ElTypeNumber - 1];
          fprintf (fileDescriptor, "%s", pRe1->SrOrigName);
          /* ecrit le nom du schema de structure de l'element */
          if (pNode->ElStructSchema->SsName == NULL)
            fprintf (fileDescriptor, " Name=NULL");
          else
            fprintf (fileDescriptor, "(%s %lx)", pNode->ElStructSchema->SsName,
                     (unsigned long int)pNode->ElStructSchema);
        }
      /* ecrit l'URI associee au schema de structure */
      if (pNode->ElStructSchema->SsUriName == NULL)
        fprintf (fileDescriptor, " Uri=NULL");
      else
        fprintf (fileDescriptor, " Uri=%s", pNode->ElStructSchema->SsUriName);
      fprintf (fileDescriptor, " Label=%s", pNode->ElLabel);
      /* ecrit le volume de l'element */
      fprintf (fileDescriptor, " Vol=%d", pNode->ElVolume);
      fprintf (fileDescriptor, " Lin=%d", pNode->ElLineNb);
      if (pNode->ElFirstSchDescr)
        fprintf (fileDescriptor, " PschExt");
      if (pNode->ElIsCopy)
        fprintf (fileDescriptor, " Copy");
      switch (pNode->ElAccess)
        {
        case ReadOnly:
          fprintf (fileDescriptor, " Right=RO");
          break;
        case ReadWrite:
          fprintf (fileDescriptor, " Right=R/W");
          break;
        case Hidden:
          fprintf (fileDescriptor, " Right=Hidden");
          break;
        default:
          break;
        }
      if (pNode->ElHolophrast)
        fprintf (fileDescriptor, " Holophrast");
      if (pNode->ElGradient)
        fprintf (fileDescriptor, " Gradient");
	
      /* ecrit les attributs de l'element */
      if (pNode->ElFirstAttr != NULL)
        {
          fprintf (fileDescriptor, " (ATTR ");
          pAttr = pNode->ElFirstAttr;
          while (pAttr != NULL)
            {
              pAttr1 = pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum-1];
              fprintf (fileDescriptor, "%s=", pAttr1->AttrOrigName);
              switch (pAttr1->AttrType)
                {
                case AtNumAttr:
                  fprintf (fileDescriptor, "%d", (int)pAttr->AeAttrValue);
                  break;
                case AtTextAttr:
                  if (pAttr->AeAttrText)
                    {
                      CopyBuffer2MBs (pAttr->AeAttrText, 0, (unsigned char *)text, 99);
                      fprintf (fileDescriptor, "%s", text);
                      if (pAttr->AeAttrText->BuNext)
                        fprintf (fileDescriptor, "...");
                    }
                  break;
                case AtReferenceAttr:
                  if (pAttr->AeAttrReference == NULL)
                    fprintf (fileDescriptor, "*AeAttrReference=NULL*");
                  else if (pAttr->AeAttrReference->RdReferred == NULL)
                    fprintf (fileDescriptor, "*RdReferred=NULL*");
                  else
                    wrRef (pAttr->AeAttrReference, fileDescriptor);
                  break;
                case AtEnumAttr:
                  fprintf (fileDescriptor, "%s",
                           pAttr1->AttrEnumValue[pAttr->AeAttrValue - 1]);
                  break;
                default:
                  fprintf (fileDescriptor, "AttrType ????");
                  break;
                }
	       
              if (pAttr->AeNext != NULL)
                fprintf (fileDescriptor, ", ");
              pAttr = pAttr->AeNext;
            }
          fprintf (fileDescriptor, ")");
        }
      /* ecrit les regles de presentation specifiques de l'element */
      if (pNode->ElFirstPRule != NULL)
        {
          fprintf (fileDescriptor, " Pres(");
          pRule = pNode->ElFirstPRule;
          while (pRule != NULL)
            {
              if (pRule != pNode->ElFirstPRule)
                fprintf (fileDescriptor, ", ");
              WrPRuleType (pRule, fileDescriptor);
              if (pRule->PrSpecifAttr > 0)
                fprintf (fileDescriptor, "[%s]", pRule->PrSpecifAttrSSchema->
                         SsAttribute->TtAttr[pRule->PrSpecifAttr - 1]->AttrOrigName);
              fprintf (fileDescriptor, " view%d", pRule->PrViewNum);
              pRule = pRule->PrNextPRule;
            }
          fprintf (fileDescriptor, ")");
          /* ecrit le contenu de l'element */
        }
      if (pNode->ElTerminal)
        switch (pNode->ElLeafType)
          {
          case LtPicture:
            fprintf (fileDescriptor, " Lg=%d\n", pNode->ElTextLength);
            for (i = 1; i <= Indent; i++)
              fprintf (fileDescriptor, " ");
            fprintf (fileDescriptor, "\'");
            WrText (pNode->ElText, 1, 72 - Indent, fileDescriptor);
            fprintf (fileDescriptor, "\'\n");
            break;
          case LtText:
            fprintf (fileDescriptor, " Lg=%d Language=%s\n",
                     pNode->ElTextLength,
                     TtaGetLanguageName (pNode->ElLanguage));
            for (i = 1; i <= Indent; i++)
              fprintf (fileDescriptor, " ");
            fprintf (fileDescriptor, "\'");
            WrText (pNode->ElText, 1, 72 - Indent, fileDescriptor);
            fprintf (fileDescriptor, "\'\n");
            break;
          case LtPolyLine:
            fprintf (fileDescriptor, " Type=%c %d points\n",
                     pNode->ElPolyLineType,
                     pNode->ElNPoints);
            for (i = 1; i <= Indent; i++)
              fprintf (fileDescriptor, " ");
            for (i = 0; i < pNode->ElNPoints && i < 8; i++)
              {
                fprintf (fileDescriptor, "%d,%d ",
                         pNode->ElPolyLineBuffer->BuPoints[i].XCoord,
                         pNode->ElPolyLineBuffer->BuPoints[i].YCoord);
              }
            if (i < pNode->ElNPoints)
              fprintf (fileDescriptor, "...");
            fprintf (fileDescriptor, "\n");
            break;
          case LtPath:
            fprintf (fileDescriptor, " path:\n");
            for (i = 1; i <= Indent; i++)
              fprintf (fileDescriptor, " ");
            WrPath (pNode->ElFirstPathSeg, 72 - Indent, fileDescriptor);
            fprintf (fileDescriptor, "\n");
            break;
          case LtSymbol:
            fprintf (fileDescriptor, " \'%c\'", pNode->ElGraph);
            if (pNode->ElGraph == '?')
              fprintf (fileDescriptor, " wc=%d",
                       (int)pNode->ElWideChar);
            fprintf (fileDescriptor, "\n");
            break;
          case LtGraphics:
          case LtCompound:
            fprintf (fileDescriptor, " \'%c\'\n", pNode->ElGraph);
            break;
          case LtPageColBreak:
            fprintf (fileDescriptor, " Number=%d View=%d",
                     pNode->ElPageNumber,
                     pNode->ElViewPSchema);
            switch (pNode->ElPageType)
              {
              case PgComputed:
                fprintf (fileDescriptor, " Computed page");
                break;
              case PgBegin:
                fprintf (fileDescriptor, " Begin of element");
                break;
              case PgUser:
                fprintf (fileDescriptor, " Page put by user");
                break;
              case ColComputed:
                fprintf (fileDescriptor, " Computed column");
                break;
              case ColBegin:
                fprintf (fileDescriptor, " First column");
                break;
              case ColUser:
                fprintf (fileDescriptor, " Column put by user");
                break;
              case ColGroup:
                fprintf (fileDescriptor, " Grouped column");
                break;
              default:
                break;
              }
            fprintf (fileDescriptor, "\n");
            break;
          case LtReference:
            if (pNode->ElReference == NULL)
              fprintf (fileDescriptor, " *ElReference=NULL*\n");
            else
              {
                fprintf (fileDescriptor, " ");
                wrRef (pNode->ElReference, fileDescriptor);
                fprintf (fileDescriptor, "\n");
              }
            break;
          case LtPairedElem:
            fprintf (fileDescriptor, "(Id=%d)", pNode->ElPairIdent);
            if (pNode->ElOtherPairedEl == NULL)
              fprintf (fileDescriptor, " ElOtherPairedEl=NULL");
            fprintf (fileDescriptor, "\n");
            break;
          default:
            fprintf (fileDescriptor, "ElLeafType ????\n");
            break;
          }
      else
        {
          fprintf (fileDescriptor, "\n");
          /* element non terminal, on ecrit sa descendance */
          f = pNode->ElFirstChild;
          while (f != NULL)
            {
              WrTree (f, Indent + 2, fileDescriptor, premierfils);
              if (!premierfils)
                f = f->ElNext;
              else
                f = NULL;
            }
        }
    }
}

/*----------------------------------------------------------------------
  TtaListAbstractTree

  Produces in a file a human-readable form of an abstract tree.
  Parameters:
  root: the root element of the tree to be listed.
  fileDescriptor: file descriptor of the file that will contain the list.
  This file must be open when calling the function.
  ----------------------------------------------------------------------*/
void TtaListAbstractTree (Element root, FILE *fileDescriptor)
{
  UserErrorCode = 0;
  if (root == NULL)
    {
      TtaError (ERR_invalid_parameter);
    }
  else
    WrTree ((PtrElement) root, 0, fileDescriptor, FALSE);
}

/*----------------------------------------------------------------------
  TtaListView

  Produces in a file a human-readable form of an abstract view.
  Parameters:
  document: the document.
  view: the view.
  fileDescriptor: file descriptor of the file that will contain the list.
  This file must be open when calling the function.
  ----------------------------------------------------------------------*/
void TtaListView (Document document, View view, FILE *fileDescriptor)
{
  PtrAbstractBox      pRootAb;

  UserErrorCode = 0;
  /* verifie le parametre document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else	if (view < 1 || view > MAX_VIEW_DOC)
    TtaError (ERR_invalid_parameter);
  else
    {
      pRootAb = LoadedDocument[document - 1]->DocViewRootAb[view - 1];
      NumberAbsBoxes (pRootAb);
      ListAbsBoxes (pRootAb, 0, fileDescriptor);
    }
}

/*----------------------------------------------------------------------
  NumberOneAbsBox numerote recursivement des paves a partir de pAb.   
  cptpave est le compte des paves.                        
  ----------------------------------------------------------------------*/
static void NumberOneAbsBox (PtrAbstractBox pAb, int *cptpave)
{
  PtrAbstractBox      pP;

  (*cptpave)++;
  pAb->AbNum = *cptpave;
  pP = pAb->AbFirstEnclosed;
  while (pP != NULL)
    {
      NumberOneAbsBox (pP, cptpave);
      pP = pP->AbNext;
    }
}

/*----------------------------------------------------------------------
  NumberAbsBoxes numerote tous les paves du sous-arbre dont la racine est 
  pointee par pP. Appelle NumberOneAbsBox.                     
  ----------------------------------------------------------------------*/
void NumberAbsBoxes (PtrAbstractBox pP)
{
  int      cptpave;	/* compteur pour numerotation des paves */

  cptpave = 0;
  if (pP != NULL)
    NumberOneAbsBox (pP, &cptpave);
}

/*----------------------------------------------------------------------
  wrThotBool ecrit la valeur d'un booleen.                         
  ----------------------------------------------------------------------*/
static void wrThotBool (ThotBool b, FILE *fileDescriptor)
{
  if (b)
    fprintf (fileDescriptor, "Y");
  else
    fprintf (fileDescriptor, "N");
}

/*----------------------------------------------------------------------
  wranchor ecrit la valeur du point de reference.                 
  ----------------------------------------------------------------------*/
static void wranchor (BoxEdge r, FILE *fileDescriptor)
{
  switch (r)
    {
    case Top:
      fprintf (fileDescriptor, "Top");
      break;
    case Bottom:
      fprintf (fileDescriptor, "Bottom");
      break;
    case Left:
      fprintf (fileDescriptor, "Left");
      break;
    case Right:
      fprintf (fileDescriptor, "Right");
      break;
    case HorizRef:
      fprintf (fileDescriptor, "HRef");
      break;
    case VertRef:
      fprintf (fileDescriptor, "VRef");
      break;
    case HorizMiddle:
      fprintf (fileDescriptor, "HMiddle");
      break;
    case VertMiddle:
      fprintf (fileDescriptor, "VMiddle");
      break;
    case NoEdge:
      fprintf (fileDescriptor, "NULL");
      break;
    default:
      fprintf (fileDescriptor, "BoxEdge ????");
      break;
    }

}

/*----------------------------------------------------------------------
  wrTypeUnit write a distance unit.                               
  ----------------------------------------------------------------------*/
static void wrTypeUnit (TypeUnit unit, FILE *fileDescriptor)
{
  switch (unit)
    {
    case UnRelative:
      break;
    case UnXHeight:
      fprintf (fileDescriptor, "ex / 10");
      break;
    case UnPoint:
      fprintf (fileDescriptor, "pt");
      break;
    case UnPixel:
      fprintf (fileDescriptor, "px");
      break;
    case UnPercent:
      fprintf (fileDescriptor, "%%");
      break;
    case UnAuto:
      fprintf (fileDescriptor, "auto");
      break;
    default:
      fprintf (fileDescriptor, "???");
      break;
    }
}

/*----------------------------------------------------------------------
  wrpos ecrit la valeur d'une position.                           
  ----------------------------------------------------------------------*/
static void wrpos (AbPosition *pPos, ThotBool racine, FILE *fileDescriptor)
{
  fprintf (fileDescriptor, " ");
  if (pPos->PosAbRef == NULL && !racine)
    {
      fprintf (fileDescriptor, "PosRef = NULL ");
    }
  else
    {
      wranchor (pPos->PosEdge, fileDescriptor);
      if (racine && pPos->PosAbRef == NULL)
        fprintf (fileDescriptor, " = ThotWindow.");
      else
        fprintf (fileDescriptor, " = AbstractBox%d.", pPos->PosAbRef->AbNum);
      wranchor (pPos->PosRefEdge, fileDescriptor);
    }
  if (pPos->PosDistance != 0)
    {
      if (pPos->PosDistance < 0)
        fprintf (fileDescriptor, "-");
      else
        fprintf (fileDescriptor, "+");
      fprintf (fileDescriptor, "%d", abs (pPos->PosDistance));
      wrTypeUnit (pPos->PosUnit, fileDescriptor);
    }
  if (pPos->PosDistDelta != 0)
    {
      if (pPos->PosDistDelta < 0)
        fprintf (fileDescriptor, "-");
      else
        fprintf (fileDescriptor, "+");
      fprintf (fileDescriptor, "%d", abs (pPos->PosDistDelta));
      wrTypeUnit (pPos->PosDeltaUnit, fileDescriptor);
    }
  if (pPos->PosUserSpecified)
    fprintf (fileDescriptor, " UserSpec");
}

/*----------------------------------------------------------------------
  wrdim ecrit la valeur d'une dimension.                          
  ----------------------------------------------------------------------*/
static void wrdim (AbDimension *pDim, ThotBool racine, ThotBool larg,
                   FILE *fileDescriptor)
{

  fprintf (fileDescriptor, " ");
  if (pDim->DimUnit == UnAuto)
    fprintf (fileDescriptor, "auto {");
  if (pDim->DimAbRef == NULL && pDim->DimValue == -1)
    fprintf (fileDescriptor, "default");
  else
    {
      if (pDim->DimAbRef == NULL && racine && pDim->DimValue <= 0)
        /* dimension relative a la fenetre */
        fprintf (fileDescriptor, "ThotWindow");
      if (pDim->DimAbRef != NULL)
        /* dimension relative a un autre pave */
        fprintf (fileDescriptor, "AbstractBox%d", pDim->DimAbRef->AbNum);
      if ((pDim->DimAbRef != NULL) ||
          (pDim->DimAbRef == NULL && racine && pDim->DimValue <= 0))
        /* dimension relative */
        {
          if (pDim->DimSameDimension && larg)
            fprintf (fileDescriptor, ".Width");
          else
            fprintf (fileDescriptor, ".Height");
          if (pDim->DimUnit == UnPercent)
            fprintf (fileDescriptor, "*");
          else if (pDim->DimValue > 0)
            fprintf (fileDescriptor, "+");
        }
      if (pDim->DimValue != 0)
        {
          fprintf (fileDescriptor, "%d", pDim->DimValue);
          wrTypeUnit (pDim->DimUnit, fileDescriptor);
        }
      if (pDim->DimUserSpecified)
        fprintf (fileDescriptor, " UserSpec");
      if (pDim->DimMinimum)
        fprintf (fileDescriptor, " Min");
    }
  if (pDim->DimUnit == UnAuto)
    fprintf (fileDescriptor, "}");
}

/*----------------------------------------------------------------------
  ListAbsBoxes ecrit dans le fichier fileDescriptor le sous-arbre de paves   
  commencant au pave pointe' par pAb, et avec            
  l'indentation Indent.                                   
  ----------------------------------------------------------------------*/
void ListAbsBoxes (PtrAbstractBox pAb, int Indent, FILE *fileDescriptor)
{
  int                 i, j;
  PtrAbstractBox      f;
  PtrAttribute        pAttr;
  PtrBox              pBox;
  ThotBool            root;
  PtrDelayedPRule     pDelPR;
  PtrSRule            pRe1;
  AbDimension        *pPavDim;
  PtrAttribute        pAt1;
  ThotPictInfo       *image;
  unsigned char       buffer[100];

  if (pAb != NULL)
    {
      fprintf (fileDescriptor, "\n%d ", pAb->AbNum);	/* numero du pave */

      for (i = 1; i <= Indent; i++)
        fprintf (fileDescriptor, " ");
      pRe1 = pAb->AbElement->
        ElStructSchema->SsRule->SrElem[pAb->AbElement->ElTypeNumber - 1];
      fprintf (fileDescriptor, "%s", pRe1->SrOrigName);
      if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
        fprintf (fileDescriptor, " %d", pAb->AbElement->ElPageType);
      if (pAb->AbPresentationBox)
        {
          if (pAb->AbTypeNum == 0)
            fprintf (fileDescriptor, ".list-item-marker");
          else
            fprintf (fileDescriptor, ".%s",
                      pAb->AbPSchema->PsPresentBox->PresBox[pAb->AbTypeNum - 1]->PbName);
        }
      fprintf (fileDescriptor, " TypeNum:%d", pAb->AbTypeNum);
      if (pAb->AbPresentationBox)
        fprintf (fileDescriptor, " VarNum:%d", pAb->AbVarNum);
      fprintf (fileDescriptor, " El:%s", pAb->AbElement->ElLabel);
      fprintf (fileDescriptor, " Vol:%d", pAb->AbVolume);

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "View:%d", pAb->AbDocView);
      fprintf (fileDescriptor, " Visibility:%d", pAb->AbVisibility);
      fprintf (fileDescriptor, " CSS_visibilty:%c", pAb->AbVis);
      fprintf (fileDescriptor, " Active:");
      wrThotBool (pAb->AbSensitive, fileDescriptor);
      fprintf (fileDescriptor, " R/O:");
      wrThotBool (pAb->AbReadOnly, fileDescriptor);
      fprintf (fileDescriptor, " Modif:");
      wrThotBool (pAb->AbCanBeModified, fileDescriptor);
      fprintf (fileDescriptor, " PresBox:");
      wrThotBool (pAb->AbPresentationBox, fileDescriptor);

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "Margin Top:");
      if (pAb->AbTopMarginUnit == UnAuto)
        fprintf (fileDescriptor, "auto");
      else
        {
          fprintf (fileDescriptor, "%d", pAb->AbTopMargin);
          wrTypeUnit (pAb->AbTopMarginUnit, fileDescriptor);
        }
      fprintf (fileDescriptor, ", Right:");
      if (pAb->AbRightMarginUnit == UnAuto)
        fprintf (fileDescriptor, "auto");
      else
        {
          fprintf (fileDescriptor, "%d", pAb->AbRightMargin);
          wrTypeUnit (pAb->AbRightMarginUnit, fileDescriptor);
        }
      fprintf (fileDescriptor, ", Bottom:");
      if (pAb->AbBottomMarginUnit == UnAuto)
        fprintf (fileDescriptor, "auto");
      else
        {
          fprintf (fileDescriptor, "%d", pAb->AbBottomMargin);
          wrTypeUnit (pAb->AbBottomMarginUnit, fileDescriptor);
        }
      fprintf (fileDescriptor, ", Left:");
      if (pAb->AbLeftMarginUnit == UnAuto)
        fprintf (fileDescriptor, "auto");
      else
        {
          fprintf (fileDescriptor, "%d", pAb->AbLeftMargin);
          wrTypeUnit (pAb->AbLeftMarginUnit, fileDescriptor);
        }

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "Padding Top:%d", pAb->AbTopPadding);
      wrTypeUnit (pAb->AbTopPaddingUnit, fileDescriptor);
      fprintf (fileDescriptor, ", Right:%d", pAb->AbRightPadding);
      wrTypeUnit (pAb->AbRightPaddingUnit, fileDescriptor);
      fprintf (fileDescriptor, ", Bottom:%d", pAb->AbBottomPadding);
      wrTypeUnit (pAb->AbBottomPaddingUnit, fileDescriptor);
      fprintf (fileDescriptor, ", Left:%d", pAb->AbLeftPadding);
      wrTypeUnit (pAb->AbLeftPaddingUnit, fileDescriptor);

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "Border Top:%d", pAb->AbTopBorder);
      wrTypeUnit (pAb->AbTopBorderUnit, fileDescriptor);
      fprintf (fileDescriptor, ", Right:%d", pAb->AbRightBorder);
      wrTypeUnit (pAb->AbRightBorderUnit, fileDescriptor);
      fprintf (fileDescriptor, ", Bottom:%d", pAb->AbBottomBorder);
      wrTypeUnit (pAb->AbBottomBorderUnit, fileDescriptor);
      fprintf (fileDescriptor, ", Left:%d", pAb->AbLeftBorder);
      wrTypeUnit (pAb->AbLeftBorderUnit, fileDescriptor);

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "BorderColor Top:%d", pAb->AbTopBColor);
      if (pAb->AbTopBColor == -2)
        fprintf (fileDescriptor, "(transparent)");
      fprintf (fileDescriptor, ", Right:%d", pAb->AbRightBColor);
      if (pAb->AbRightBColor == -2)
        fprintf (fileDescriptor, "(transparent)");
      fprintf (fileDescriptor, ", Bottom:%d", pAb->AbBottomBColor);
      if (pAb->AbBottomBColor == -2)
        fprintf (fileDescriptor, "(transparent)");
      fprintf (fileDescriptor, ", Left:%d", pAb->AbLeftBColor);
      if (pAb->AbLeftBColor == -2)
        fprintf (fileDescriptor, "(transparent)");

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "BorderStyle Top:%d", pAb->AbTopStyle);
      fprintf (fileDescriptor, ", Right:%d", pAb->AbRightStyle);
      fprintf (fileDescriptor, ", Bottom:%d", pAb->AbBottomStyle);
      fprintf (fileDescriptor, ", Left:%d", pAb->AbLeftStyle);

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "Pattern:%d", pAb->AbFillPattern);
      if (pAb->AbGradientBackground)
	fprintf (fileDescriptor, " Gradient: Const%d", pAb->AbBackground);
      else
	fprintf (fileDescriptor, " Background:%d", pAb->AbBackground);
      fprintf (fileDescriptor, " Foreground:%d", pAb->AbForeground);
      fprintf (fileDescriptor, " Color:%d", pAb->AbColor);
      fprintf (fileDescriptor, " StopColor:%d", pAb->AbStopColor);
      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "Opacity:%d", pAb->AbOpacity);
      fprintf (fileDescriptor, " FillOpacity:%d", pAb->AbFillOpacity);
      fprintf (fileDescriptor, " StrokeOpacity:%d", pAb->AbStrokeOpacity);
      fprintf (fileDescriptor, " StopOpacity:%d", pAb->AbStopOpacity);
      fprintf (fileDescriptor, " FillRule:%c", pAb->AbFillRule);
      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      if (pAb->AbMarker == NULL)
        fprintf (fileDescriptor, "Marker:none");
      else
	{
	  fprintf (fileDescriptor, "Marker:");
	  pAttr = GetAttrElementWithException (ExcCssId, pAb->AbMarker);
	  if (pAttr)
	    {
	      CopyBuffer2MBs (pAttr->AeAttrText, 0, buffer, 99);
	      fprintf (fileDescriptor, "%s", buffer);
	    }
	}
      if (pAb->AbMarkerStart == NULL)
        fprintf (fileDescriptor, " MarkerStart:none");
      else
	{
	  fprintf (fileDescriptor, " MarkerStart:");
	  pAttr = GetAttrElementWithException (ExcCssId, pAb->AbMarkerStart);
	  if (pAttr)
	    {
	      CopyBuffer2MBs (pAttr->AeAttrText, 0, buffer, 99);
	      fprintf (fileDescriptor, "%s", buffer);
	    }
	}
      if (pAb->AbMarkerMid == NULL)
        fprintf (fileDescriptor, " MarkerMid:none");
      else
	{
	  fprintf (fileDescriptor, " MarkerMid:");
	  pAttr = GetAttrElementWithException (ExcCssId, pAb->AbMarkerMid);
	  if (pAttr)
	    {
	      CopyBuffer2MBs (pAttr->AeAttrText, 0, buffer, 99);
	      fprintf (fileDescriptor, "%s", buffer);
	    }
	}
      if (pAb->AbMarkerEnd == NULL)
        fprintf (fileDescriptor, " MarkerEnd:none");
      else
	{
	  fprintf (fileDescriptor, " MarkerEnd:");
	  pAttr = GetAttrElementWithException (ExcCssId, pAb->AbMarkerEnd);
	  if (pAttr)
	    {
	      CopyBuffer2MBs (pAttr->AeAttrText, 0, buffer, 99);
	      fprintf (fileDescriptor, "%s", buffer);
	    }
	}
      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "LineStyle:%c", pAb->AbLineStyle);
      fprintf (fileDescriptor, " LineWeight:%d", pAb->AbLineWeight);
      wrTypeUnit (pAb->AbLineWeightUnit, fileDescriptor);
      fprintf (fileDescriptor, " Depth:%d", pAb->AbDepth);

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "Font:%d", pAb->AbFont);
      fprintf (fileDescriptor, " Style:%d", pAb->AbFontStyle);
      fprintf (fileDescriptor, " Weight:%d", pAb->AbFontWeight);
      fprintf (fileDescriptor, " Variant:%d", pAb->AbFontVariant);
      fprintf (fileDescriptor, " Size:%d", pAb->AbSize);
      wrTypeUnit (pAb->AbSizeUnit, fileDescriptor);
      fprintf (fileDescriptor, " Underline:");
      switch (pAb->AbUnderline)
        {
        case 0:
          fprintf (fileDescriptor, "NoUnderline");
          break;
        case 1:
          fprintf (fileDescriptor, "Underlined");
          break;
        case 2:
          fprintf (fileDescriptor, "Overlined");
          break;
        case 3:
          fprintf (fileDescriptor, "CrossedOut");
          break;
        default:
          fprintf (fileDescriptor, "%c", pAb->AbUnderline);
          break;
        }
      if (!pAb->AbHorizEnclosing || !pAb->AbVertEnclosing ||
          pAb->AbNotInLine)
        {
          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 6; j++)
            fprintf (fileDescriptor, " ");
          if (!pAb->AbHorizEnclosing)
            fprintf (fileDescriptor, "HorizEncl:N ");
          if (!pAb->AbVertEnclosing)
            fprintf (fileDescriptor, "VertEncl:N ");
          if (pAb->AbNotInLine)
            fprintf (fileDescriptor, "NotInLine ");
        }

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "Dir:");
      switch (pAb->AbDirection)
        {
        case 'L':
          fprintf (fileDescriptor, "ltr");
          break;
        case 'R':
          fprintf (fileDescriptor, "rtl");
          break;
        default:
          fprintf (fileDescriptor, "%c", pAb->AbDirection);
          break;
        }
      fprintf (fileDescriptor, " Bidi:");
      switch (pAb->AbUnicodeBidi)
        {
        case 'N':
          fprintf (fileDescriptor, "normal");
          break;
        case 'E':
          fprintf (fileDescriptor, "embed");
          break;
        case 'O':
          fprintf (fileDescriptor, "override");
          break;
        default:
          fprintf (fileDescriptor, "%c", pAb->AbUnicodeBidi);
          break;
        }
      fprintf (fileDescriptor, " Indent:%d", pAb->AbIndent);
      wrTypeUnit (pAb->AbIndentUnit, fileDescriptor);
      fprintf (fileDescriptor, " Align:");
      switch (pAb->AbAdjust)
        {
        case AlignLeft:
          fprintf (fileDescriptor, "left");
          break;
        case AlignRight:
          fprintf (fileDescriptor, "right");
          break;
        case AlignCenter:
          fprintf (fileDescriptor, "center");
          break;
        case AlignLeftDots:
          fprintf (fileDescriptor, "leftDots");
          break;
        case AlignJustify:
          fprintf (fileDescriptor, "justify");
          break;
        default:
          fprintf (fileDescriptor, "AbAdjust ????");
          break;
        }
      fprintf (fileDescriptor, " Hyphen:");
      wrThotBool (pAb->AbHyphenate, fileDescriptor);

      fprintf (fileDescriptor, " LineSpace:%d", pAb->AbLineSpacing);
      wrTypeUnit (pAb->AbLineSpacingUnit, fileDescriptor);

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "VertRef:");
      wrpos (&pAb->AbVertRef, FALSE, fileDescriptor);
      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "HorizRef:");
      wrpos (&pAb->AbHorizRef, FALSE, fileDescriptor);
      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      if (pAb->AbEnclosing == NULL)
        root = TRUE;
      else
        root = FALSE;
      fprintf (fileDescriptor, "VertPos:");
      wrpos (&pAb->AbVertPos, root, fileDescriptor);
      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "HorizPos:");
      wrpos (&pAb->AbHorizPos, root, fileDescriptor);
      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "Width:");
      pPavDim = &pAb->AbWidth;
      if (pPavDim->DimIsPosition)
        wrpos (&pPavDim->DimPosition, root, fileDescriptor);
      else
        wrdim (pPavDim, root, TRUE, fileDescriptor);
      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "Height:");
      pPavDim = &pAb->AbHeight;
      if (pPavDim->DimIsPosition)
        wrpos (&pPavDim->DimPosition, root, fileDescriptor);
      else
        wrdim (pPavDim, root, FALSE, fileDescriptor);

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "Nature:");
      switch (pAb->AbLeafType)
        {
        case LtCompound:
          pBox = pAb->AbBox;
          if (pBox == NULL)
            fprintf (fileDescriptor, " NO BOX");
          else if (pBox->BxType == BoGhost)
            fprintf (fileDescriptor, " GHOST");
          else if (pBox->BxType == BoFloatGhost)
            fprintf (fileDescriptor, " FLOAT_GHOST");
          else if (pBox->BxType == BoBlock)
            fprintf (fileDescriptor, " BLOCK");
          else if (pBox->BxType == BoStructGhost)
            fprintf (fileDescriptor, " STRUCT_GHOST");
          else if (pBox->BxType == BoFloatBlock)
            fprintf (fileDescriptor, " FLOAT_BLOCK");
          else if (pBox->BxType == BoCellBlock)
            fprintf (fileDescriptor, " CELL_BLOCK");
          else if (pBox->BxType == BoTable)
            fprintf (fileDescriptor, " TABLE");
          else if (pBox->BxType == BoColumn)
            fprintf (fileDescriptor, " COLUMN");
          else if (pBox->BxType == BoRow)
            fprintf (fileDescriptor, " ROW");
          else if (pBox->BxType == BoCell)
            fprintf (fileDescriptor, " CELL");
          else
            fprintf (fileDescriptor, " COMP");
          break;
        case LtPicture:
          fprintf (fileDescriptor, "PICTURE");
          for (i = 1; i <= Indent + 6; i++)
            fprintf (fileDescriptor, " ");
          break;
        case LtText:
          fprintf (fileDescriptor, "TEXT ");
          break;
        case LtPolyLine:
          fprintf (fileDescriptor, "POLYLINE ");
          break;
        case LtPath:
          fprintf (fileDescriptor, "PATH ");
          break;
        case LtPageColBreak:
          fprintf (fileDescriptor, "PAGE");
          break;
        case LtSymbol:
          fprintf (fileDescriptor, "SYMBOL");
          break;
        case LtGraphics:
          fprintf (fileDescriptor, "GRAPHICS");
          break;
        case LtReference:
          fprintf (fileDescriptor, "REFER");
          break;
        default:
          fprintf (fileDescriptor, "AbLeafType ????");
          break;
        }

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      switch (pAb->AbLeafType)
        {
        case LtCompound:
          image = (ThotPictInfo *) pAb->AbPictBackground;
          if (image != NULL)
            {
              fprintf (fileDescriptor, "BgPicture: w=%d h=%d, name=\"",
                       image->PicWArea, image->PicHArea);
              if (image->PicFileName)
                fprintf (fileDescriptor, "%s\" ", image->PicFileName);
              else
                fprintf (fileDescriptor, "\"");
              fprintf (fileDescriptor, " Mode:");
              if (image->PicPresent == ReScale)
                fprintf (fileDescriptor, "ReScale");
              else if (image->PicPresent == FillFrame)
                fprintf (fileDescriptor, "FillFrame");
              else if (image->PicPresent == XRepeat)
                fprintf (fileDescriptor, "XRepeat");
              else if (image->PicPresent == YRepeat)
                fprintf (fileDescriptor, "YRepeat");
              else
                fprintf (fileDescriptor, "RealSize");
              fprintf (fileDescriptor, " XPos: %d", image->PicPosX);
              wrTypeUnit (image->PicXUnit, fileDescriptor);
              fprintf (fileDescriptor, " YPos: %d", image->PicPosY);
              wrTypeUnit (image->PicYUnit, fileDescriptor);
              fprintf (fileDescriptor, "\n");
              for (j = 1; j <= Indent + 6; j++)
                fprintf (fileDescriptor, " ");
            }
          image = (ThotPictInfo *) pAb->AbPictListStyle;
          if (image != NULL)
            {
              fprintf (fileDescriptor, "ListPicture: w=%d h=%d, name=\"",
                       image->PicWArea, image->PicHArea);
              if (image->PicFileName)
                fprintf (fileDescriptor, "%s\" ", image->PicFileName);
              else
                fprintf (fileDescriptor, "\"");
              fprintf (fileDescriptor, "\n");
              for (j = 1; j <= Indent + 6; j++)
                fprintf (fileDescriptor, " ");
            }
          fprintf (fileDescriptor, "ShowBox:");
          if (pAb->AbFillBox)
            fprintf (fileDescriptor, "Y");
          else
            fprintf (fileDescriptor, "N");
          break;
        case LtPicture:
          image = (ThotPictInfo *) pAb->AbPictInfo;
          if (image == NULL)
            fprintf (fileDescriptor, "AbPictInfo = NULL");
          else
            fprintf (fileDescriptor, "Picture: w=%d, h=%d, name=\"%s\"",
                     image->PicWArea,
                     image->PicHArea, image->PicFileName);
          fprintf (fileDescriptor, " Mode:");
          if (image->PicPresent == ReScale)
            fprintf (fileDescriptor, "ReScale");
          else if (image->PicPresent == FillFrame)
            fprintf (fileDescriptor, "FillFrame");
          else if (image->PicPresent == XRepeat)
            fprintf (fileDescriptor, "XRepeat");
          else if (image->PicPresent == YRepeat)
            fprintf (fileDescriptor, "YRepeat");
          else
            fprintf (fileDescriptor, "RealSize");
          break;
        case LtText:
        case LtReference:
          fprintf (fileDescriptor, "language = %s",
                   TtaGetLanguageName (pAb->AbLang));
          fprintf (fileDescriptor, "\n");
          for (i = 1; i <= Indent + 6; i++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, " \'");
          WrText (pAb->AbText, 1, 60, fileDescriptor);
          fprintf (fileDescriptor, "\'");
          break;
        case LtPolyLine:
          fprintf (fileDescriptor, "type=%c ", pAb->AbPolyLineShape);
          for (i = 0; i < pAb->AbVolume && i < 8; i++)
            fprintf (fileDescriptor, "%d,%d ",
                     pAb->AbPolyLineBuffer->BuPoints[i].XCoord,
                     pAb->AbPolyLineBuffer->BuPoints[i].YCoord);
          if (i < pAb->AbVolume)
            fprintf (fileDescriptor, "...");
          break;
        case LtPath:
          WrPath (pAb->AbFirstPathSeg, 72-Indent, fileDescriptor);
          break;
        case LtSymbol:
        case LtGraphics:
          fprintf (fileDescriptor, "script=%c \'%c\'",
                   pAb->AbGraphScript, pAb->AbShape);
          if (pAb->AbLeafType == LtGraphics &&
	      (pAb->AbShape == 1 || pAb->AbShape == 'C'))
            {
              fprintf (fileDescriptor, " rx:%d", pAb->AbRx);
              wrTypeUnit (pAb->AbRxUnit, fileDescriptor);
              fprintf (fileDescriptor, " ry:%d", pAb->AbRy);
              wrTypeUnit (pAb->AbRyUnit, fileDescriptor);
            }
          break;
        default:
          break;
        }
      if (pAb->AbSelected)
        fprintf (fileDescriptor, " SELECTED");
      if (pAb->AbNew)
        fprintf (fileDescriptor, " NEW");
      if (pAb->AbDead)
        fprintf (fileDescriptor, " DEAD");
      if (pAb->AbWidthChange)
        fprintf (fileDescriptor, " WidthChange");
      if (pAb->AbHeightChange)
        fprintf (fileDescriptor, " HeightChange");
      if (pAb->AbHorizPosChange)
        fprintf (fileDescriptor, " HorizPosChange");
      if (pAb->AbVertPosChange)
        fprintf (fileDescriptor, " VertPosChange");
      if (pAb->AbHorizRefChange)
        fprintf (fileDescriptor, " HorizRefChange");
      if (pAb->AbVertRefChange)
        fprintf (fileDescriptor, " VertRefChange");
      if (pAb->AbSizeChange)
        fprintf (fileDescriptor, " SizeChange");
      if (pAb->AbAspectChange)
        fprintf (fileDescriptor, " AspectChange");
      if (pAb->AbMBPChange)
        fprintf (fileDescriptor, " MBPChange");
      if (pAb->AbFloatChange)
        fprintf (fileDescriptor, " FloatChange");
      if (pAb->AbPositionChange)
        fprintf (fileDescriptor, " PositionChange");
      if (pAb->AbChange)
        fprintf (fileDescriptor, " OtherChange");
      if (pAb->AbOnPageBreak)
        fprintf (fileDescriptor, " ON PAGE BOUNDARY");
      if (pAb->AbAfterPageBreak)
        fprintf (fileDescriptor, " OUT OF PAGE");

      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      /* liste les regles de presentation retardees */
      if (pAb->AbDelayedPRule != NULL)
        {
          pDelPR = pAb->AbDelayedPRule;
          do
            {
              fprintf (fileDescriptor, "Deferred rule: ");
              WrPRuleType (pDelPR->DpPRule, fileDescriptor);
              fprintf (fileDescriptor, " AbstractBox%d", pDelPR->DpAbsBox->AbNum);
              if (pDelPR->DpAttribute != NULL)
                {
                  pAt1 = pDelPR->DpAttribute;
                  fprintf (fileDescriptor, "[%s]", pAt1->AeAttrSSchema->
                           SsAttribute->TtAttr[pAt1->AeAttrNum - 1]->AttrOrigName);
                }
              fprintf (fileDescriptor, "\n");
              for (j = 1; j <= Indent + 6; j++)
                fprintf (fileDescriptor, " ");
              pDelPR = pDelPR->DpNext;
            }
          while (pDelPR != NULL);
        }
      /* affichage du chainage des paves dupliques */
      if (pAb->AbPreviousRepeated != NULL)
        {
          fprintf (fileDescriptor, "AbstractBox repeats previous: ");
          fprintf (fileDescriptor, "%d ", pAb->AbPreviousRepeated->AbNum);
          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 6; j++)
            fprintf (fileDescriptor, " ");
        }
      if (pAb->AbNextRepeated != NULL)
        {
          fprintf (fileDescriptor, " AbstractBox repeats next: ");
          fprintf (fileDescriptor, "%d ", pAb->AbNextRepeated->AbNum);
          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 6; j++)
            fprintf (fileDescriptor, " ");
        }

      fprintf (fileDescriptor, "LineBreak:");
      wrThotBool (pAb->AbAcceptLineBreak, fileDescriptor);
      fprintf (fileDescriptor, " PageBreak:");
      wrThotBool (pAb->AbAcceptPageBreak, fileDescriptor);
      fprintf (fileDescriptor, " Float:%c", pAb->AbFloat);
      fprintf (fileDescriptor, " Clear:%c", pAb->AbClear);
      if (pAb->AbLeafType == LtCompound && pAb->AbPositioning)
        {
          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 6; j++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, "position: ");
          if (pAb->AbPositioning->PnAlgorithm == PnStatic)
            fprintf (fileDescriptor, "static");
          else if (pAb->AbPositioning->PnAlgorithm == PnRelative)
            fprintf (fileDescriptor, "relative");
          else if (pAb->AbPositioning->PnAlgorithm == PnAbsolute)
            fprintf (fileDescriptor, "absolute");
          else if (pAb->AbPositioning->PnAlgorithm == PnFixed)
            fprintf (fileDescriptor, "fixed");
          else
            fprintf (fileDescriptor, "???");
          if (pAb->AbPositioning->PnTopUnit != UnUndefined)
            {
              fprintf (fileDescriptor, " top:");
              if (pAb->AbPositioning->PnTopUnit == UnAuto)
                fprintf (fileDescriptor, "auto");
              else
                {
                  fprintf (fileDescriptor, "%d",
                           pAb->AbPositioning->PnTopDistance);
                  wrTypeUnit (pAb->AbPositioning->PnTopUnit,
                              fileDescriptor);
                }
            }
          if (pAb->AbPositioning->PnRightUnit != UnUndefined)
            {
              fprintf (fileDescriptor, " right:");
              if (pAb->AbPositioning->PnRightUnit == UnAuto)
                fprintf (fileDescriptor, "auto");
              else
                {
                  fprintf (fileDescriptor, "%d",
                           pAb->AbPositioning->PnRightDistance);
                  wrTypeUnit (pAb->AbPositioning->PnRightUnit,
                              fileDescriptor);
                }
            }
          if (pAb->AbPositioning->PnBottomUnit != UnUndefined)
            {
              fprintf (fileDescriptor, " bottom:");
              if (pAb->AbPositioning->PnBottomUnit == UnAuto)
                fprintf (fileDescriptor, "auto");
              else
                {
                  fprintf (fileDescriptor, "%d",
                           pAb->AbPositioning->PnBottomDistance);
                  wrTypeUnit (pAb->AbPositioning->PnBottomUnit,
                              fileDescriptor);
                }
            }
          if (pAb->AbPositioning->PnLeftUnit != UnUndefined)
            {
              fprintf (fileDescriptor, " left:");
              if (pAb->AbPositioning->PnLeftUnit == UnAuto)
                fprintf (fileDescriptor, "auto");
              else
                {
                  fprintf (fileDescriptor, "%d",
                           pAb->AbPositioning->PnLeftDistance);
                  wrTypeUnit (pAb->AbPositioning->PnLeftUnit,
                              fileDescriptor);
                }
            }
        }
      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");
      fprintf (fileDescriptor, "ListStyleType:%c", pAb->AbListStyleType);
      fprintf (fileDescriptor, " ListStyleImage:%c", pAb->AbListStyleImage);
      fprintf (fileDescriptor, " ListStylePosition:%c",
               pAb->AbListStylePosition);
      if (pAb->AbBuildAll)
        fprintf (fileDescriptor, " Gather");
      fprintf (fileDescriptor, "\n");
      for (j = 1; j <= Indent + 6; j++)
        fprintf (fileDescriptor, " ");

      if (pAb->AbLeafType == LtCompound)
        {
          fprintf (fileDescriptor, "Line:");
          wrThotBool (pAb->AbInLine, fileDescriptor);
          fprintf (fileDescriptor, " Display:%c", pAb->AbDisplay);
          /* display TruncatedHead and TruncatedTail */
          /* even if it's a lines block */
          fprintf (fileDescriptor, " TruncatedHead:");
          wrThotBool (pAb->AbTruncatedHead, fileDescriptor);
          fprintf (fileDescriptor, " TruncatedTail:");
          wrThotBool (pAb->AbTruncatedTail, fileDescriptor);

          fprintf (fileDescriptor, "\n");
          f = pAb->AbFirstEnclosed;
          while (f != NULL)
            {
              if (f->AbEnclosing != pAb)
                {
                  if (f->AbEnclosing == NULL)
                    fprintf (fileDescriptor,
                             "Next AbstractBox: AbEnclosing=NULL\n");
                  else
                    fprintf (fileDescriptor,
                             "Next AbstractBox: bad AbEnclosing\n");
                }
              ListAbsBoxes (f, Indent + 2, fileDescriptor);
              f = f->AbNext;
            }
        }
    }
}

/*----------------------------------------------------------------------
  wrnumber ecrit un entier
  ----------------------------------------------------------------------*/
static void wrnumber (int i, FILE *fileDescriptor)
{
  fprintf (fileDescriptor, "%d", i);
}

/*----------------------------------------------------------------------
  wrtext ecrit du texte
  ----------------------------------------------------------------------*/
static void wrtext (char *Text, FILE *fileDescriptor)
{
  fprintf (fileDescriptor, "%s", Text);
}

#ifdef _GL

/*----------------------------------------------------------------------
  WrTransform writes transformations
  ----------------------------------------------------------------------*/
static void WrTransform (PtrTransform trans,  FILE *fileDescriptor)
{
  while (trans)
    {
      switch (trans->TransType)
        {
        case  PtElBoxTranslate:
          fprintf (fileDescriptor, "ORIGIN Box Translation: ");
          fprintf (fileDescriptor, " X=%2e, Y=%2e ", trans->XScale, trans->YScale);
          break;

        case  PtElViewBox:
          fprintf (fileDescriptor, "viewbox: x=%2e, y=%2e, w=%2e, h=%2e ",
                   trans->VbXTranslate, trans->VbYTranslate,
                   trans->VbWidth, trans->VbHeight);
          switch (trans->VbAspectRatio)
            {
              case ArUnknown: fprintf (fileDescriptor, "? "); break;
              case ArNone: fprintf (fileDescriptor, "none "); break;
              case ArXMinYMin: fprintf (fileDescriptor, "xMinYMin "); break;
              case ArXMidYMin: fprintf (fileDescriptor, "xMidYMin "); break;
              case ArXMaxYMin: fprintf (fileDescriptor, "xMaxYMin "); break;
              case ArXMinYMid: fprintf (fileDescriptor, "xMinYMid "); break;
              case ArXMidYMid: fprintf (fileDescriptor, "xMidYMid "); break;
              case ArXMaxYMid: fprintf (fileDescriptor, "xMaxYMid "); break;
              case ArXMinYMax: fprintf (fileDescriptor, "xMinYMax "); break;
              case ArXMidYMax: fprintf (fileDescriptor, "xMidYMax "); break;
              case ArXMaxYMax: fprintf (fileDescriptor, "xMaxYMax "); break;
              default: fprintf (fileDescriptor, "??"); break;
            }
          switch (trans->VbMeetOrSlice)
            {
              case MsUnknown: fprintf (fileDescriptor, "? "); break;
              case MsMeet: fprintf (fileDescriptor, "meet "); break;
              case MsSlice: fprintf (fileDescriptor, "slice "); break;
              default: fprintf (fileDescriptor, "?? "); break;
            }
          break;

        case  PtElScale:
          fprintf (fileDescriptor, "Scale: ");
          fprintf (fileDescriptor, " X=%2e, Y=%2e ", trans->XScale, trans->YScale);
          break;

        case PtElTranslate:
          fprintf (fileDescriptor, "Translation: ");
          fprintf (fileDescriptor, " X=%2e, Y=%2e ", trans->XScale, trans->YScale);
          break;

        case PtElRotate:
          fprintf (fileDescriptor, "Rotate: ");
          fprintf (fileDescriptor, "Angle %2e X=%2e, Y=%2e ", trans->TrAngle, trans->XRotate, trans->YRotate);
          break;

        case PtElMatrix:
          fprintf (fileDescriptor, "Matrix: ");
          fprintf (fileDescriptor, "A %2e B %2e C %2e D %2e E %2e F %2e ", 
                   trans->AMatrix, trans->BMatrix, trans->CMatrix, trans->DMatrix,
                   trans->EMatrix, trans->FMatrix);
          break;
        case PtElSkewX:
          fprintf (fileDescriptor, "SkewX : ");
          fprintf (fileDescriptor, " %2e ", trans->TrFactor);
          break;

        case PtElSkewY: 
          fprintf (fileDescriptor, "SkewY : ");
          fprintf (fileDescriptor, " %2e ", trans->TrFactor);
          break;

        default:
          break;
        }
      trans = trans->Next;
    }  
}
#endif /* _GL */ 

/*----------------------------------------------------------------------
  ListBoxTree writes information about boxes associated to the abstract
  box pAb and all children.
  ----------------------------------------------------------------------*/
static void ListBoxTree (PtrAbstractBox pAb, int frame, int Indent,
                         FILE *fileDescriptor)
{
  PtrAbstractBox      pAbEnclosed;
  PtrPosRelations     pPosRel;
  PtrDimRelations     pDimRel;
  PtrBox              pBox;
  PtrBox              box1;
  ThotBool            loop;
  BoxRelation        *pRe1;
  ThotPictInfo       *image;
  int                 i, j, t, b, l, r;

  if (pAb->AbBox != NULL)
    {
      for (j = 1; j <= Indent; j++)
        fprintf (fileDescriptor, " ");
      pBox = pAb->AbBox;
      wrnumber (pBox->BxAbstractBox->AbNum, fileDescriptor);
      fprintf (fileDescriptor, " ");
      wrtext (AbsBoxType (pAb, TRUE), fileDescriptor);
      fprintf (fileDescriptor, "\n");
      if (pBox != NULL)
        {
          for (j = 1; j <= Indent + 4; j++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, "Characters:");
          wrnumber (pAb->AbVolume, fileDescriptor);
          if (pAb->AbLeafType == LtText)
            {
              fprintf (fileDescriptor, " Spaces:");
              wrnumber (pBox->BxNSpaces, fileDescriptor);
            }

          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 4; j++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, "X:");
          if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
            wrnumber (pBox->BxNexChild->BxXOrg, fileDescriptor);
          else
            wrnumber (pBox->BxXOrg, fileDescriptor);
          fprintf (fileDescriptor, " Y:");
          if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
            wrnumber (pBox->BxNexChild->BxYOrg, fileDescriptor);
          else
            wrnumber (pBox->BxYOrg, fileDescriptor);

#ifdef _GL 
          fprintf (fileDescriptor, " ClipX:");
          wrnumber (pBox->BxClipX, fileDescriptor);
          fprintf (fileDescriptor, " ClipY:");
          wrnumber (pBox->BxClipY, fileDescriptor);
          fprintf (fileDescriptor, " ClipW:");
          wrnumber (pBox->BxClipW, fileDescriptor);
          fprintf (fileDescriptor, " ClipH:");
          wrnumber (pBox->BxClipH, fileDescriptor);
          if (pAb->AbElement->ElSystemOrigin)
            {
              fprintf (fileDescriptor, " Origin System");
            } 
          if (pAb->AbElement->ElTransform)
            {
              fprintf (fileDescriptor, "\n");
              for (j = 1; j <= Indent + 4; j++)
                fprintf (fileDescriptor, " ");
              WrTransform (pAb->AbElement->ElTransform ,fileDescriptor);
            }
	     
#endif /* _GL */
#ifdef _GL
          if (pAb->AbElement->ElGradient)
            fprintf (fileDescriptor, " Holding Gradient");
#endif /* _GL */
          fprintf (fileDescriptor, " Base:");
          wrnumber (pBox->BxHorizRef, fileDescriptor);
          fprintf (fileDescriptor, " Axis:");
          wrnumber (pBox->BxVertRef, fileDescriptor);

          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 4; j++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, "Width:");
          wrnumber (pBox->BxWidth, fileDescriptor);
          fprintf (fileDescriptor, "  Inside_Width:");
          wrnumber (pBox->BxW, fileDescriptor);
          if (!pAb->AbWidth.DimIsPosition &&
              (pAb->AbWidth.DimUnit == UnAuto ||
               pAb->AbWidth.DimMinimum))
            {
              fprintf (fileDescriptor, " Rule-Width:");
              wrnumber (pBox->BxRuleWidth, fileDescriptor);
            }
          fprintf (fileDescriptor, " Max-Width:");
          wrnumber (pBox->BxMaxWidth, fileDescriptor);
          if (pBox->BxShrink)
            fprintf (fileDescriptor, " !Shrink");
 
          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 4; j++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, "Height:");
          wrnumber (pBox->BxHeight, fileDescriptor);
          fprintf (fileDescriptor, " Inside_Height:");
          wrnumber (pBox->BxH, fileDescriptor);
          if (!pAb->AbHeight.DimIsPosition &&
              pAb->AbHeight.DimMinimum)
            {
              if (pBox->BxContentHeight)
                fprintf (fileDescriptor, " Minimum-Height:");
              else
                fprintf (fileDescriptor, " Content-Height:");
              wrnumber (pBox->BxRuleHeight, fileDescriptor);
            }
          /* display extra margins */
          GetExtraMargins (pBox, frame, TRUE, &t, &b, &l, &r);
          if (t || b || l || r)
            {
              fprintf (fileDescriptor, "\n");
              for (j = 1; j <= Indent + 4; j++)
                fprintf (fileDescriptor, " ");
              fprintf (fileDescriptor, "Ghost-Margins Top:");
              wrnumber (t, fileDescriptor);
              fprintf (fileDescriptor, " Left:");
              wrnumber (l, fileDescriptor);
              fprintf (fileDescriptor, " Bottom:");
              wrnumber (b, fileDescriptor);
              fprintf (fileDescriptor, " Right:");
              wrnumber (r, fileDescriptor);
            }
          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 4; j++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, "Margins Top:");
          wrnumber (pBox->BxTMargin, fileDescriptor);
          fprintf (fileDescriptor, " Left:");
          wrnumber (pBox->BxLMargin, fileDescriptor);
          fprintf (fileDescriptor, " Bottom:");
          wrnumber (pBox->BxBMargin, fileDescriptor);
          fprintf (fileDescriptor, " Right:");
          wrnumber (pBox->BxRMargin, fileDescriptor);

          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 4; j++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, "Borders Top:");
          wrnumber (pBox->BxTBorder, fileDescriptor);
          fprintf (fileDescriptor, " Left:");
          wrnumber (pBox->BxLBorder, fileDescriptor);
          fprintf (fileDescriptor, " Bottom:");
          wrnumber (pBox->BxBBorder, fileDescriptor);
          fprintf (fileDescriptor, " Right:");
          wrnumber (pBox->BxRBorder, fileDescriptor);
          fprintf (fileDescriptor, " ");

          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 4; j++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, "Paddings Top:");
          wrnumber (pBox->BxTPadding, fileDescriptor);
          fprintf (fileDescriptor, " Left:");
          wrnumber (pBox->BxLPadding, fileDescriptor);
          fprintf (fileDescriptor, " Bottom:");
          wrnumber (pBox->BxBPadding, fileDescriptor);
          fprintf (fileDescriptor, " Right:");
          wrnumber (pBox->BxRPadding, fileDescriptor);
          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 4; j++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, "Underline:");
          switch (pBox->BxUnderline)
            {
            case 0:
              fprintf (fileDescriptor, "NoUnderline");
              break;
            case 1:
              fprintf (fileDescriptor, "Underlined");
              break;
            case 2:
              fprintf (fileDescriptor, "Overlined");
              break;
            case 3:
              fprintf (fileDescriptor, "CrossedOut");
              break;
            default:
              fprintf (fileDescriptor, "%c", pBox->BxUnderline);
              break;
            }

          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 4; j++)
            fprintf (fileDescriptor, " ");
          if (pAb->AbSelected)
            fprintf (fileDescriptor, "SELECTED ");
          if (pBox->BxDisplay)
            fprintf (fileDescriptor, "Displayed:Y ");
          if (pBox->BxShadow)
            fprintf (fileDescriptor, "Shadow:Y ");
          if (pAb->AbOnPageBreak)
            fprintf (fileDescriptor, "ON PAGE BOUNDARY ");
          if (pAb->AbAfterPageBreak)
            fprintf (fileDescriptor, "OUT OF PAGE ");
          if (!pAb->AbHorizEnclosing)
            fprintf (fileDescriptor, "Not-Horiz-Enclosed ");
          if (!pAb->AbVertEnclosing)
            fprintf (fileDescriptor, "Not-Vert-Enclosed ");
          /* On liste les relations hors-structure */
          if (pBox->BxXOutOfStruct)
            fprintf (fileDescriptor, "XRelation-Out-Enclosing ");
          if (pBox->BxYOutOfStruct)
            fprintf (fileDescriptor, "YRelation-Out-Enclosing ");
          if (pBox->BxWOutOfStruct)
            fprintf (fileDescriptor, "HDimRelation-Out-Enclosing ");
          if (pBox->BxHOutOfStruct)
            fprintf (fileDescriptor, "VDimRelation-Out-Enclosing ");

          fprintf (fileDescriptor, "\n");
          for (j = 1; j <= Indent + 4; j++)
            fprintf (fileDescriptor, " ");
          fprintf (fileDescriptor, "Nature:");
          switch (pAb->AbLeafType)
            {
            case LtCompound:
              if (pBox->BxType == BoGhost)
                fprintf (fileDescriptor, " GHOST");
              else if (pBox->BxType == BoFloatGhost)
                fprintf (fileDescriptor, " FLOAT_GHOST");
              else if (pBox->BxType == BoStructGhost)
                fprintf (fileDescriptor, " STRUCT_GHOST");
              else if (pBox->BxType == BoBlock)
                fprintf (fileDescriptor, " BLOCK");
              else if (pBox->BxType == BoFloatBlock)
                fprintf (fileDescriptor, " FLOAT_BLOCK");
              else if (pBox->BxType == BoTable)
                fprintf (fileDescriptor, " TABLE");
              else if (pBox->BxType == BoColumn)
                fprintf (fileDescriptor, " COLUMN");
              else if (pBox->BxType == BoRow)
                fprintf (fileDescriptor, " ROW");
              else if (pBox->BxType == BoCell)
                fprintf (fileDescriptor, " CELL");
              else
                fprintf (fileDescriptor, " COMP");
              if (pBox->BxType == BoBlock ||
                  pBox->BxType == BoFloatBlock ||
                  pBox->BxType == BoTable ||
                  pBox->BxType == BoColumn ||
                  pBox->BxType == BoRow)
                {
                  fprintf (fileDescriptor, "\n");
                  for (j = 1; j <= Indent + 4; j++)
                    fprintf (fileDescriptor, " ");
                  fprintf (fileDescriptor, "  Min_Width:");
                  wrnumber (pBox->BxMinWidth, fileDescriptor);
                  fprintf (fileDescriptor, " Max_Width:");
                  wrnumber (pBox->BxMaxWidth, fileDescriptor);
                }
              break;
            case LtText:
              if (pBox->BxType == BoSplit || pBox->BxType == BoMulScript)
                {
                  if (pBox->BxType == BoSplit)
                    fprintf (fileDescriptor, "SPLIT_TEXT\n");
                  else
                    fprintf (fileDescriptor, "MULTI_SCRIPT\n");
                  box1 = pBox->BxNexChild;
                  while (box1)
                    {
                      for (j = 1; j <= Indent + 6; j++)
                        fprintf (fileDescriptor, " ");
                      switch (box1->BxType)
                        {
                        case BoScript:
                          fprintf (fileDescriptor, "SCRIPT");
                          break;
                        case BoDotted:
                          fprintf (fileDescriptor, "WITH_HYPHEN");
                          break;
                        default:
                          fprintf (fileDescriptor, "PIECE");
                        }
                      fprintf (fileDescriptor, " Script:%c", box1->BxScript);
                      fprintf (fileDescriptor, " Start:");
                      wrnumber (box1->BxFirstChar, fileDescriptor);
                      fprintf (fileDescriptor, " NChars:");
                      wrnumber (box1->BxNChars, fileDescriptor);
                      fprintf (fileDescriptor, " NSpaces:");
                      wrnumber (box1->BxNSpaces, fileDescriptor);
                      fprintf (fileDescriptor, " X:");
                      wrnumber (box1->BxXOrg, fileDescriptor);
                      fprintf (fileDescriptor, " Y:");
                      wrnumber (box1->BxYOrg, fileDescriptor);
                      fprintf (fileDescriptor, " Base:");
                      wrnumber (box1->BxHorizRef, fileDescriptor);
                      fprintf (fileDescriptor, "\n");
                      for (j = 1; j <= Indent + 6; j++)
                        fprintf (fileDescriptor, " ");
                      fprintf (fileDescriptor, "Width:");
                      wrnumber (box1->BxWidth, fileDescriptor);
                      fprintf (fileDescriptor, " Index:");
                      wrnumber (box1->BxIndChar, fileDescriptor);
                      fprintf (fileDescriptor, " \'");
                      i = box1->BxNChars;
                      if (i > 20)
                        i = 20;
                      WrText (box1->BxBuffer, box1->BxIndChar + 1, i,
                              fileDescriptor);
                      fprintf (fileDescriptor, "\'");
                      fprintf (fileDescriptor, "\n");
                      box1 = box1->BxNexChild;
                    }
                  fprintf (fileDescriptor, "\n");
                }
              else
                {
                  fprintf (fileDescriptor, "TEXT");
                  fprintf (fileDescriptor, " Script:%c\n", pBox->BxScript);
                }
              for (j = 1; j <= Indent + 6; j++)
                fprintf (fileDescriptor, " ");
              fprintf (fileDescriptor, "\'");
              i = pAb->AbVolume;
              if (i > 60)
                i = 60;
              WrText (pAb->AbText, 1, i, fileDescriptor);
              fprintf (fileDescriptor, "\'");
              break;
            case LtPicture:
              image = (ThotPictInfo *) pAb->AbPictInfo;
              fprintf (fileDescriptor, "PICTURE\n");
              for (j = 1; j <= Indent + 6; j++)
                fprintf (fileDescriptor, " ");
              if (image == NULL)
                fprintf (fileDescriptor, "PictInfo = NULL");
              else
                fprintf (fileDescriptor, " w = %d, h = %d, name = %s",
                         image->PicWArea, image->PicHArea,
                         image->PicFileName);
              break;
            case LtPageColBreak:
              fprintf (fileDescriptor, "PAGE");
              break;
            case LtSymbol:
              fprintf (fileDescriptor, "SYMBOL\n");
              for (j = 1; j <= Indent + 6; j++)
                fprintf (fileDescriptor, " ");
              fprintf (fileDescriptor, "\'");
              putc (pAb->AbShape, fileDescriptor);
              fprintf (fileDescriptor, "\'");
              break;
            case LtGraphics:
              fprintf (fileDescriptor, "GRAPHICS\n");
              for (j = 1; j <= Indent + 6; j++)
                fprintf (fileDescriptor, " ");
              fprintf (fileDescriptor, "\'");
              putc (pAb->AbShape, fileDescriptor);
              fprintf (fileDescriptor, "\'");
              fprintf (fileDescriptor, " printed graphics: \'");
              putc (pAb->AbRealShape, fileDescriptor);
              fprintf (fileDescriptor, "\'");
              if (pAb->AbShape == 1 || pAb->AbShape == 'C')
                {
                  fprintf (fileDescriptor, " rx:%d", pAb->AbRx);
                  wrTypeUnit (pAb->AbRxUnit, fileDescriptor);
                  fprintf (fileDescriptor, " ry:%d", pAb->AbRy);
                  wrTypeUnit (pAb->AbRyUnit, fileDescriptor);
                }
              break;
            case LtPolyLine:
              fprintf (fileDescriptor, "POLYLINE\n");
              for (j = 1; j <= Indent + 6; j++)
                fprintf (fileDescriptor, " ");
              fprintf (fileDescriptor, "\'");
              putc (pAb->AbPolyLineShape, fileDescriptor);
              fprintf (fileDescriptor, "\' ");
              for (i = 0; i < pAb->AbVolume && i < 8; i++)
                {
                  fprintf (fileDescriptor, "%d,%d ",
                           pAb->AbPolyLineBuffer->BuPoints[i].XCoord,
                           pAb->AbPolyLineBuffer->BuPoints[i].YCoord);
                }
              if (i < pAb->AbVolume)
                fprintf (fileDescriptor, "...");
              break;
            case LtPath:
              fprintf (fileDescriptor, "PATH\n");
              for (j = 1; j <= Indent + 6; j++)
                fprintf (fileDescriptor, " ");
              WrPath (pBox->BxFirstPathSeg, 60, fileDescriptor);
              break;
            default:
              break;
            }

          if (pBox->BxHorizFlex)
            {
              fprintf (fileDescriptor, ", Horiz-ELASTIC");
              if (pBox->BxHorizInverted)
                fprintf (fileDescriptor, ", Horiz-INVERTED");
            }
          if (pBox->BxVertFlex)
            {
              fprintf (fileDescriptor, ", Vert-ELASTIC");
              if (pBox->BxVertInverted)
                fprintf (fileDescriptor, ", Vert-INVERTED");
            }

          /* liste des relations de position de la boite */
          pPosRel = pBox->BxPosRelations;
          while (pPosRel != NULL)
            {
              loop = TRUE;
              i = 1;
              while (loop)
                {
                  pRe1 = &pPosRel->PosRTable[i - 1];
                  if (pRe1->ReBox == NULL)
                    loop = FALSE;
                  else
                    {
                      fprintf (fileDescriptor, "\n");
                      for (j = 1; j <= Indent + 6; j++)
                        fprintf (fileDescriptor, " ");
                      wranchor (pRe1->ReRefEdge, fileDescriptor);
                      switch (pRe1->ReOp)
                        {
                        case OpHorizDep:
                        case OpVertDep:
                          fprintf (fileDescriptor, " bound with ");
                          break;
                        case OpHorizInc:
                        case OpVertInc:
                          fprintf (fileDescriptor, " inside ");
                          break;
                        case OpHorizRef:
                          fprintf (fileDescriptor, " moves Axis of ");
                          break;
                        case OpVertRef:
                          fprintf (fileDescriptor, " moves Base of ");
                          break;
                        case OpWidth:
                          fprintf (fileDescriptor, " extends Width of ");
                          break;
                        case OpHeight:
                          fprintf (fileDescriptor, " extends Height of ");
                          break;
                        }

                      if (pRe1->ReBox->BxAbstractBox != NULL)
                        wrnumber (pRe1->ReBox->BxAbstractBox->AbNum, fileDescriptor);
                      else
                        fprintf (fileDescriptor, "?");
                      if (i == MAX_RELAT_POS)
                        loop = FALSE;
                      else
                        i++;
                    }
                }
              pPosRel = pPosRel->PosRNext;
              /* Bloc suivant */
            }
          fprintf (fileDescriptor, "\n");
          /* liste des dependances de largeur */
          pDimRel = pBox->BxWidthRelations;
          while (pDimRel != NULL)
            {
              loop = TRUE;
              i = 1;
              while (loop)
                if (pDimRel->DimRTable[i - 1] == NULL)
                  loop = FALSE;
                else
                  {
                    fprintf (fileDescriptor, "\n");
                    for (j = 1; j <= Indent + 6; j++)
                      fprintf (fileDescriptor, " ");
                    fprintf (fileDescriptor, "Width changes ");
                    if (pDimRel->DimROp[i - 1] == OpSame)
                      fprintf (fileDescriptor, "Width of ");
                    else if (pDimRel->DimROp[i - 1] == OpReverse)
                      fprintf (fileDescriptor, "Height of ");
                    if (pDimRel->DimRTable[i - 1]->BxAbstractBox != NULL)
                      wrnumber (pDimRel->DimRTable[i - 1]->BxAbstractBox->AbNum, fileDescriptor);
                    else
                      fprintf (fileDescriptor, "?");
                    if (i == MAX_RELAT_DIM)
                      loop = FALSE;
                    else
                      i++;
                  }
              pDimRel = pDimRel->DimRNext;
            }
          /* liste des dependances de hauteur */
          pDimRel = pBox->BxHeightRelations;
          while (pDimRel != NULL)
            {
              loop = TRUE;
              i = 1;
              while (loop)
                if (pDimRel->DimRTable[i - 1] == NULL)
                  loop = FALSE;
                else
                  {
                    fprintf (fileDescriptor, "\n");
                    for (j = 1; j <= Indent + 6; j++)
                      fprintf (fileDescriptor, " ");
                    fprintf (fileDescriptor, "Height changes ");
                    if (pDimRel->DimROp[i - 1] == OpSame)
                      fprintf (fileDescriptor, "Height of ");
                    else if (pDimRel->DimROp[i - 1] == OpReverse)
                      fprintf (fileDescriptor, "Width of ");
                    if (pDimRel->DimRTable[i - 1]->BxAbstractBox != NULL)
                      wrnumber (pDimRel->DimRTable[i - 1]->BxAbstractBox->AbNum, fileDescriptor);
                    if (i == MAX_RELAT_DIM)
                      loop = FALSE;
                    else
                      i++;
                  }
              pDimRel = pDimRel->DimRNext;
              /* Bloc suivant */
            }
          fprintf (fileDescriptor, "\n");
          if (pAb->AbLeafType == LtCompound)
            {
              for (j = 1; j <= Indent + 6; j++)
                fprintf (fileDescriptor, " ");
              fprintf (fileDescriptor, "\n");
              pAbEnclosed = pAb->AbFirstEnclosed;
              while (pAbEnclosed != NULL)
                {
                  ListBoxTree (pAbEnclosed, frame, Indent + 2, fileDescriptor);
                  pAbEnclosed = pAbEnclosed->AbNext;
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  ListBoxes writes information about all boxes within the frame.           
  ----------------------------------------------------------------------*/
void ListBoxes (int frame, FILE *fileDescriptor)
{
  ViewFrame          *pFrame;

  pFrame = &ViewFrameTable[frame - 1];
  if (frame != 0 && pFrame->FrAbstractBox != NULL)
    /* Une petite cuisine pour passer le nom du fichier en parametre */
    {
      /* On numerote les paves */
      NumberAbsBoxes (pFrame->FrAbstractBox);
      /* On ecrit les informations sur les boites crees */
      ListBoxTree (pFrame->FrAbstractBox, frame, 0, fileDescriptor);
    }
}

/*----------------------------------------------------------------------
  TtaListBoxes

  Produces in a file a human-readable form of the boxes.
  Parameters:
  document: the document.
  view: the view.
  fileDescriptor: file descriptor of the file that will contain the list.
  This file must be open when calling the function.
  ----------------------------------------------------------------------*/
void TtaListBoxes (Document document, View view, FILE *fileDescriptor)
{
  UserErrorCode = 0;
  /* verifie le parametre document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else	 if (view < 1 || view > MAX_VIEW_DOC)
    TtaError (ERR_invalid_parameter);
  else
    ListBoxes (LoadedDocument[document - 1]->DocViewFrame[view - 1],
               fileDescriptor);
}

/*----------------------------------------------------------------------
  wrlevel writes the level n.                 
  ----------------------------------------------------------------------*/
static void wrlevel (Level n, FILE *fileDescriptor)
{
  switch (n)
    {
    case RlEnclosing:
      fprintf (fileDescriptor, "Enclosing");
      break;
    case RlSameLevel:
      ;
      break;
    case RlEnclosed:
      fprintf (fileDescriptor, "Enclosed");
      break;
    case RlPrevious:
      fprintf (fileDescriptor, "Previous");
      break;
    case RlNext:
      fprintf (fileDescriptor, "Next");
      break;
    case RlSelf:
      fprintf (fileDescriptor, "*");
      break;
    case RlContainsRef:
      fprintf (fileDescriptor, "Refering");
      break;
    case RlRoot:
      fprintf (fileDescriptor, "Root");
      break;
    case RlReferred:
      fprintf (fileDescriptor, "Referred");
      break;
    case RlCreator:
      fprintf (fileDescriptor, "Creator");
      break;
    case RlLastSibling:
      fprintf (fileDescriptor, "LastSibling");
      break;
    }
}

/*----------------------------------------------------------------------
  wrdistunit writes the unit u.               
  ----------------------------------------------------------------------*/
static void wrdistunit (TypeUnit u, FILE *fileDescriptor)
{
  switch (u)
    {
    case UnRelative:
    case UnUndefined:
      ;
      break;
    case UnXHeight:
      fprintf (fileDescriptor, " ex");
      break;
    case UnPoint:
      fprintf (fileDescriptor, " pt");
      break;
    case UnPixel:
      fprintf (fileDescriptor, " px");
      break;	
    case UnPercent:
      fprintf (fileDescriptor, " %%");
      break;
    case UnAuto:
      fprintf (fileDescriptor, " auto");
      break;
    case UnGradient:
      fprintf (fileDescriptor, " gradient");
      break;
    }
}

/*----------------------------------------------------------------------
  wrrulename writes the name of the rule r.
  ----------------------------------------------------------------------*/
static void wrrulename (int r, FILE *fileDescriptor)
{
  if (r > 0)
    fprintf (fileDescriptor, pSchemaStr->SsRule->SrElem[r - 1]->SrName);
}

/*----------------------------------------------------------------------
  wrattrname writes the name of the attribute a.
  ----------------------------------------------------------------------*/
static void wrattrname (int a, FILE *fileDescriptor)
{
  if (a != 0)
    fprintf (fileDescriptor,
             pSchemaStr->SsAttribute->TtAttr[abs (a) - 1]->AttrName);
}

/*----------------------------------------------------------------------
  wrboxname writes the name of the presentation box b.
  ----------------------------------------------------------------------*/
static void wrboxname (int b, FILE *fileDescriptor)
{
  if (b > 0)
    fprintf (fileDescriptor, pSc1->PsPresentBox->PresBox[b - 1]->PbName);
}

/*----------------------------------------------------------------------
  wrModeHerit writes the inheritage mode.
  ----------------------------------------------------------------------*/
static void wrModeHerit (InheritMode M, FILE *fileDescriptor)
{
  switch (M)
    {
    case InheritParent:
      fprintf (fileDescriptor, "Enclosing");
      break;
    case InheritPrevious:
      fprintf (fileDescriptor, "Previous");
      break;
    case InheritChild:
      fprintf (fileDescriptor, "Enclosed");
      break;
    case InheritCreator:
      fprintf (fileDescriptor, "Creator");
      break;
    case InheritGrandFather:
      fprintf (fileDescriptor, "GrandFather");
      break;
    }
}


/*----------------------------------------------------------------------
  wrsize ecrit au terminal la regle de taille pointee par pR.     
  ----------------------------------------------------------------------*/
static void wrsize (PtrPRule pR, FILE *fileDescriptor)
{
  if (pR->PrPresMode == PresInherit)
    {
      wrModeHerit (pR->PrInheritMode, fileDescriptor);
      if (pR->PrInhPercent)
        {
          fprintf (fileDescriptor, " * ");
          if (pR->PrInhAttr)
            wrattrname (pR->PrInhDelta, fileDescriptor);
          else
            wrnumber (pR->PrInhDelta, fileDescriptor);
          fprintf (fileDescriptor, " %%");
        }
      else
        if (pR->PrInhDelta == 0)
          fprintf (fileDescriptor, " =");
        else
          {
            if (pR->PrInhDelta > 0)
              fprintf (fileDescriptor, "+");
            if (pR->PrInhAttr)
              wrattrname (pR->PrInhDelta, fileDescriptor);
            else
              wrnumber (pR->PrInhDelta, fileDescriptor);
            wrdistunit (pR->PrInhUnit, fileDescriptor);
          }
      if (pR->PrInhMinOrMax > 0)
        {
          if (pR->PrInhDelta >= 0)
            fprintf (fileDescriptor, " max ");
          else
            fprintf (fileDescriptor, " min ");
          if (pR->PrMinMaxAttr)
            wrattrname (pR->PrInhMinOrMax, fileDescriptor);
          else
            wrnumber (pR->PrInhMinOrMax, fileDescriptor);
        }
    }
  else if (pR->PrPresMode == PresImmediate)
    {
      if (pR->PrMinAttr)
        wrattrname (pR->PrMinValue, fileDescriptor);
      else
        wrnumber (pR->PrMinValue, fileDescriptor);
      wrdistunit (pR->PrMinUnit, fileDescriptor);
    }
  else
    fprintf (fileDescriptor, "??????");
  fprintf (fileDescriptor, ";");
}


/*----------------------------------------------------------------------
  wrfontstyle ecrit au terminal la regle d'heritage ou la valeur  
  entiere de la regle pointee par pR.                     
  ----------------------------------------------------------------------*/
static void wrfontstyle (PtrPRule pR, FILE *fileDescriptor)
{
  if (pR->PrPresMode == PresInherit)
    {
      wrModeHerit (pR->PrInheritMode, fileDescriptor);
      if (pR->PrInhDelta == 0 && !pR->PrInhPercent)
        fprintf (fileDescriptor, " =");
      else
        fprintf (fileDescriptor, "??????");
    }
  else if (pR->PrPresMode == PresImmediate)
    {
      if (pR->PrType == PtFont)
        switch (pR->PrChrValue)
          {
          case 'C':
            fprintf (fileDescriptor, "Courrier");
            break;
          case 'H':
            fprintf (fileDescriptor, "Helvetica");
            break;
          case 'T':
            fprintf (fileDescriptor, "Times");
            break;
          case 'c':
            fprintf (fileDescriptor, "courrier");
            break;
          case 'h':
            fprintf (fileDescriptor, "helvetica");
            break;
          case 't':
            fprintf (fileDescriptor, "times");
            break;
          default:
            fprintf (fileDescriptor, "%c", pR->PrChrValue);
            break;
          }
      else if (pR->PrType == PtStyle)
        switch (pR->PrChrValue)
          {
          case 'I':
            fprintf (fileDescriptor, "Italics");
            break;
          case 'R':
            fprintf (fileDescriptor, "Roman");
            break;
          case 'O':
            fprintf (fileDescriptor, "Oblique");
            break;
          default:
            fprintf (fileDescriptor, "%c", pR->PrChrValue);
            break;
          }
      else if (pR->PrType == PtWeight)
        switch (pR->PrChrValue)
          {
          case 'B':
            fprintf (fileDescriptor, "Bold");
            break;
          case 'N':
            fprintf (fileDescriptor, "Normal");
            break;
          default:
            fprintf (fileDescriptor, "%c", pR->PrChrValue);
            break;
          }
      else if (pR->PrType == PtVariant)
        switch (pR->PrChrValue)
          {
          case 'C':
            fprintf (fileDescriptor, "SmallCaps");
            break;
          case 'D':
            fprintf (fileDescriptor, "DoubleStruck");
            break;
          case 'F':
            fprintf (fileDescriptor, "Fraktur");
            break;
          case 'N':
            fprintf (fileDescriptor, "Normal");
            break;
          case 'S':
            fprintf (fileDescriptor, "Script");
            break;
          default:
            fprintf (fileDescriptor, "%c", pR->PrChrValue);
            break;
          }
      else if (pR->PrType == PtUnderline)
        switch (pR->PrChrValue)
          {
          case 'N':
            fprintf (fileDescriptor, "NoUnderline");
            break;
          case 'U':
            fprintf (fileDescriptor, "Underlined");
            break;
          case 'O':
            fprintf (fileDescriptor, "Overlined");
            break;
          case 'C':
            fprintf (fileDescriptor, "CrossedOut");
            break;
          default:
            fprintf (fileDescriptor, "%c", pR->PrChrValue);
            break;
          }
      else if (pR->PrType == PtThickness)
        switch (pR->PrChrValue)
          {
          case 'T':
            fprintf (fileDescriptor, "Thick");
            break;
          case 'N':
            fprintf (fileDescriptor, "Thin");
            break;
          default:
            fprintf (fileDescriptor, "%c", pR->PrChrValue);
            break;
          }
      else if (pR->PrType == PtDirection)
        switch (pR->PrChrValue)
          {
          case 'L':
            fprintf (fileDescriptor, "ltr");
            break;
          case 'R':
            fprintf (fileDescriptor, "rtl");
            break;
          default:
            fprintf (fileDescriptor, "%c", pR->PrChrValue);
            break;
          }
      else if (pR->PrType == PtUnicodeBidi)
        switch (pR->PrChrValue)
          {
          case 'N':
            fprintf (fileDescriptor, "normal");
            break;
          case 'E':
            fprintf (fileDescriptor, "embed");
            break;
          case 'O':
            fprintf (fileDescriptor, "override");
            break;
          default:
            fprintf (fileDescriptor, "%c", pR->PrChrValue);
            break;
          }
      else if (pR->PrType == PtDisplay)
        switch (pR->PrChrValue)
          {
          case 'N':
            fprintf (fileDescriptor, "none");
            break;
          case 'I':
            fprintf (fileDescriptor, "inline");
            break;
          case 'B':
            fprintf (fileDescriptor, "block");
            break;
          case 'L':
            fprintf (fileDescriptor, "listItem");
            break;
          case 'R':
            fprintf (fileDescriptor, "runIn");
            break;
          case 'b':
            fprintf (fileDescriptor, "inlineBlock");
            break;
          }
      else if (pR->PrType == PtListStyleType)
        switch (pR->PrChrValue)
          {
          case 'D':
            fprintf (fileDescriptor, "Disc");
            break;
          case 'C':
            fprintf (fileDescriptor, "Circle");
            break;
          case 'S':
            fprintf (fileDescriptor, "Square");
            break;
          case '1':
            fprintf (fileDescriptor, "Decimal");
            break;
          case 'Z':
            fprintf (fileDescriptor, "DecimalLeadingZero");
            break;
          case 'i':
            fprintf (fileDescriptor, "LowerRoman");
            break;
          case 'I':
            fprintf (fileDescriptor, "UpperRoman");
            break;
          case 'g':
            fprintf (fileDescriptor, "LowerGreek");
            break;
          case 'G':
            fprintf (fileDescriptor, "UpperGreek");
            break;
          case 'a':
            fprintf (fileDescriptor, "LowerLatin");
            break;
          case 'A':
            fprintf (fileDescriptor, "UpperLatin");
            break;
          case 'N':
            fprintf (fileDescriptor, "None");
            break;
          }
      else if (pR->PrType == PtListStylePosition)
        switch (pR->PrChrValue)
          {
          case 'I':
            fprintf (fileDescriptor, "Inside");
            break;
          case 'O':
            fprintf (fileDescriptor, "Outside");
            break;
          }
      else if (pR->PrType == PtListStyleImage)
        {
          if (pR->PrIntValue == 0)
            fprintf (fileDescriptor, "none");
          else
            {
              fprintf (fileDescriptor, "Cste");
              wrnumber (pR->PrIntValue, fileDescriptor);
            }
        }
      else if (pR->PrType == PtFloat)
        switch (pR->PrChrValue)
          {
          case 'L':
            fprintf (fileDescriptor, "left");
            break;
          case 'R':
            fprintf (fileDescriptor, "right");
            break;
          case 'N':
            fprintf (fileDescriptor, "none");
            break;
          }
      else if (pR->PrType == PtClear)
        switch (pR->PrChrValue)
          {
          case 'L':
            fprintf (fileDescriptor, "left");
            break;
          case 'R':
            fprintf (fileDescriptor, "right");
            break;
          case 'B':
            fprintf (fileDescriptor, "both");
            break;
          case 'N':
            fprintf (fileDescriptor, "none");
            break;
          }
      else if (pR->PrType == PtPosition)
        switch (pR->PrChrValue)
          {
          case 'S':
            fprintf (fileDescriptor, "static");
            break;
          case 'R':
            fprintf (fileDescriptor, "relative");
            break;
          case 'A':
            fprintf (fileDescriptor, "absolute");
            break;
          case 'F':
            fprintf (fileDescriptor, "fixed");
            break;
          case 'I':
            fprintf (fileDescriptor, "inherit");
            break;
          }
      else if (pR->PrType == PtLineStyle ||
               pR->PrType == PtBorderTopStyle ||
               pR->PrType == PtBorderRightStyle ||
               pR->PrType == PtBorderBottomStyle ||
               pR->PrType == PtBorderLeftStyle)
        switch (pR->PrChrValue)
          {
          case '0':
            fprintf (fileDescriptor, "None");
            break;
          case 'H':
            fprintf (fileDescriptor, "Hidden");
            break;
          case '.':
            fprintf (fileDescriptor, "Dotted");
            break;
          case '-':
            fprintf (fileDescriptor, "Dashed");
            break;
          case 'S':
            fprintf (fileDescriptor, "Solid");
            break;
          case 'D':
            fprintf (fileDescriptor, "Double");
            break;
          case 'G':
            fprintf (fileDescriptor, "Groove");
            break;
          case 'R':
            fprintf (fileDescriptor, "Ridge");
            break;
          case 'I':
            fprintf (fileDescriptor, "Inset");
            break;
          case 'O':
            fprintf (fileDescriptor, "Outset");
            break;
          }
      else if (pR->PrType == PtFillRule)
	switch (pR->PrChrValue)
          {
          case 'e':
            fprintf (fileDescriptor, "EvenOdd");
            break;
          case 'n':
            fprintf (fileDescriptor, "NonZero");
            break;
          default:
            fprintf (fileDescriptor, "%c", pR->PrChrValue);
            break;
	  }
      else
        fprintf (fileDescriptor, "%c", pR->PrChrValue);
    }
  else
    fprintf (fileDescriptor, "??????");
  fprintf (fileDescriptor, ";");
}

/*----------------------------------------------------------------------
  wrnbherit ecrit au terminal la regle d'heritage ou la valeur    
  entiere de la regle pointee par pR.                     
  ----------------------------------------------------------------------*/
static void wrnbherit (PtrPRule pR, FILE *fileDescriptor)
{
  if (pR->PrPresMode == PresInherit)
    if (pR->PrInhPercent)
      fprintf (fileDescriptor, "??????");
    else
      {
        wrModeHerit (pR->PrInheritMode, fileDescriptor);
        if (pR->PrInhDelta == 0)
          fprintf (fileDescriptor, " =");
        else
          {
            if (pR->PrInhDelta > 0)
              fprintf (fileDescriptor, "+");
            if (pR->PrInhAttr)
              wrattrname (pR->PrInhDelta, fileDescriptor);
            else
              wrnumber (pR->PrInhDelta, fileDescriptor);
          }
        if (pR->PrInhMinOrMax > 0)
          {
            if (pR->PrInhDelta >= 0)
              fprintf (fileDescriptor, " max ");
            else
              fprintf (fileDescriptor, " min ");
            if (pR->PrMinMaxAttr)
              wrattrname (pR->PrInhMinOrMax, fileDescriptor);
            else
              wrnumber (pR->PrInhMinOrMax, fileDescriptor);
          }
      }
  else if (pR->PrPresMode == PresCurrentColor)
    fprintf (fileDescriptor, "currentColor");
  else if (pR->PrPresMode == PresImmediate)
    if (pR->PrValueType == PrAttrValue)
      wrattrname (pR->PrIntValue, fileDescriptor);
    else if (pR->PrValueType == PrNumValue)
      wrnumber (pR->PrIntValue, fileDescriptor);
    else if (pR->PrValueType == PrConstStringValue)
      {
	fprintf (fileDescriptor, "Cste");
	wrnumber (pR->PrIntValue, fileDescriptor);
      }
  else
    fprintf (fileDescriptor, "??????");
  fprintf (fileDescriptor, ";");
}


/*----------------------------------------------------------------------
  wrminind ecrit au terminal la regle 'NoBreak1, NoBreak2 ou      
  Indent pointee par pR.                                  
  ----------------------------------------------------------------------*/
static void wrminind (PtrPRule pR, FILE *fileDescriptor)
{
  if (pR->PrPresMode == PresInherit)
    wrnbherit (pR, fileDescriptor);
  else
    {
      if (pR->PrPresMode == PresImmediate)
        {
          if (pR->PrMinUnit == UnAuto)
            fprintf (fileDescriptor, "auto");
          else
            {
              if (pR->PrMinAttr)
                wrattrname (pR->PrMinValue, fileDescriptor);
              else
                wrnumber (pR->PrMinValue, fileDescriptor);
              wrdistunit (pR->PrMinUnit, fileDescriptor);
            }
        }
      else
        fprintf (fileDescriptor, "??????");
      fprintf (fileDescriptor, ";");
    }
}


/*----------------------------------------------------------------------
  WrPos ecrit au terminal la position pos.                        
  ----------------------------------------------------------------------*/
static void WrPos (PosRule pos, ThotBool Def, FILE *fileDescriptor)
{
  if (Def)
    {
      if (pos.PoPosDef == NoEdge)
        fprintf (fileDescriptor, " NULL");
      else
        {
          wranchor (pos.PoPosDef, fileDescriptor);
          fprintf (fileDescriptor, " = ");
        }
    }
  if (!Def || pos.PoPosDef != NoEdge)
    {
      wrlevel (pos.PoRelation, fileDescriptor);
      if (pos.PoNotRel)
        fprintf (fileDescriptor, " NOT");
      fprintf (fileDescriptor, " ");
      if (pos.PoRefKind == RkElType)
        wrrulename (pos.PoRefIdent, fileDescriptor);
      else if (pos.PoRefKind == RkPresBox)
        wrboxname (pos.PoRefIdent, fileDescriptor);
      else if (pos.PoRefKind == RkAttr)
        wrattrname (pos.PoRefIdent, fileDescriptor);
      else if (pos.PoRefKind == RkAnyElem)
        fprintf (fileDescriptor, "AnyElem");
      else if (pos.PoRefKind == RkAnyBox)
        fprintf (fileDescriptor, "AnyBox");
      fprintf (fileDescriptor, ". ");
      wranchor (pos.PoPosRef, fileDescriptor);
      if (pos.PoDistance != 0)
        {
          if (pos.PoDistance > 0)
            fprintf (fileDescriptor, "+");
          else
            fprintf (fileDescriptor, "-");
          if (pos.PoDistAttr)
            wrattrname (abs (pos.PoDistance), fileDescriptor);
          else
            wrnumber (abs (pos.PoDistance), fileDescriptor);
          wrdistunit (pos.PoDistUnit, fileDescriptor);
        }
      if (pos.PoDistDelta != 0)
        {
          if (pos.PoDistDelta > 0)
            fprintf (fileDescriptor, "+");
          else
            fprintf (fileDescriptor, "-");
          wrnumber (abs (pos.PoDistDelta), fileDescriptor);
          wrdistunit (pos.PoDeltaUnit, fileDescriptor);
	    
        }
      if (pos.PoUserSpecified)
        fprintf (fileDescriptor, " UserSpecified");
    }
  fprintf (fileDescriptor, ";");
}


/*----------------------------------------------------------------------
  wrdimens ecrit au terminal la dimension Dim.                    
  ----------------------------------------------------------------------*/
static void wrdimens (DimensionRule Dim, ThotBool Hauteur,
                      FILE *fileDescriptor)
{
  if (Dim.DrPosition)
    WrPos (Dim.DrPosRule, True, fileDescriptor);
  else
    {
      if (Dim.DrAbsolute)
        {
          if (Dim.DrAttr)
            wrattrname (Dim.DrValue, fileDescriptor);
          else
            wrnumber (Dim.DrValue, fileDescriptor);
          if (Dim.DrValue != 0)
            wrdistunit (Dim.DrUnit, fileDescriptor);
          if (Dim.DrUserSpecified)
            fprintf (fileDescriptor, " UserSpecified");
          if (Dim.DrMin)
            fprintf (fileDescriptor, " Min");
        }
      else
        {
          wrlevel (Dim.DrRelation, fileDescriptor);
          fprintf (fileDescriptor, " ");
          if (Dim.DrNotRelat)
            fprintf (fileDescriptor, "not ");
          if (Dim.DrRefKind == RkElType)
            wrrulename (Dim.DrRefIdent, fileDescriptor);
          else if (Dim.DrRefKind == RkPresBox)
            wrboxname (Dim.DrRefIdent, fileDescriptor);
          else if (Dim.DrRefKind == RkAttr)
            wrattrname (Dim.DrRefIdent, fileDescriptor);
          else if (Dim.DrRefKind == RkAnyElem)
            fprintf (fileDescriptor, "AnyElem");
          else if (Dim.DrRefKind == RkAnyBox)
            fprintf (fileDescriptor, "AnyBox");
          fprintf (fileDescriptor, ". ");
          if ((Dim.DrSameDimens && Hauteur) || (!Dim.DrSameDimens && !Hauteur))
            fprintf (fileDescriptor, "Height");
          else
            fprintf (fileDescriptor, "Width");
          if (Dim.DrUnit == UnPercent)
            {
              fprintf (fileDescriptor, "*");
              if (Dim.DrValue < 0)
                fprintf (fileDescriptor, "-");
              if (Dim.DrAttr)
                wrattrname (abs (Dim.DrValue), fileDescriptor);
              else
                wrnumber (abs (Dim.DrValue), fileDescriptor);
              fprintf (fileDescriptor, "%%");
            }
          else
            {
              if (Dim.DrValue < 0)
                fprintf (fileDescriptor, "-");
              if (Dim.DrValue > 0)
                fprintf (fileDescriptor, "+");
              if (Dim.DrValue != 0)
                {
                  if (Dim.DrAttr)
                    wrattrname (abs (Dim.DrValue), fileDescriptor);
                  else
                    wrnumber (abs (Dim.DrValue), fileDescriptor);
                  wrdistunit (Dim.DrUnit, fileDescriptor);
                }
            }
          if (Dim.DrMin)
            fprintf (fileDescriptor, " Min");
        }
      fprintf (fileDescriptor, ";");
    }
}


/*----------------------------------------------------------------------
  wrCondition                                             
  ----------------------------------------------------------------------*/
static void wrCondition (PtrCondition pCond, FILE *fileDescriptor)
{
  if (!pCond->CoNotNegative)
    fprintf (fileDescriptor, "NOT ");
  switch (pCond->CoCondition)
    {
    case PcFirst:
      fprintf (fileDescriptor, "First ");
      break;
    case PcLast:
      fprintf (fileDescriptor, "Last ");
      break;
    case PcReferred:
      fprintf (fileDescriptor, "Referred ");
      break;
    case PcFirstRef:
      fprintf (fileDescriptor, "FirstRef ");
      break;
    case PcLastRef:
      fprintf (fileDescriptor, "LastRef ");
      break;
    case PcExternalRef:
      fprintf (fileDescriptor, "ExternalRef ");
      break;
    case PcInternalRef:
      fprintf (fileDescriptor, "InternalRef ");
      break;
    case PcCopyRef:
      fprintf (fileDescriptor, "CopyRef ");
      break;
    case PcAnyAttributes:
      fprintf (fileDescriptor, "AnyAttributes ");
      break;
    case PcFirstAttr:
      fprintf (fileDescriptor, "FirstAttr ");
      break;
    case PcLastAttr:
      fprintf (fileDescriptor, "LastAttr ");
      break;
    case PcUserPage:
      fprintf (fileDescriptor, "UserPage ");
      break;
    case PcStartPage:
      fprintf (fileDescriptor, "StartPage ");
      break;
    case PcComputedPage:
      fprintf (fileDescriptor, "ComputedPage ");
      break;
    case PcEmpty:
      fprintf (fileDescriptor, "Empty ");
      break;
    case PcRoot:
      fprintf (fileDescriptor, "Root ");
      break;
    case PcEven:
      fprintf (fileDescriptor, "Even");
      break;
    case PcOdd:
      fprintf (fileDescriptor, "Odd");
      break;
    case PcOne:
      fprintf (fileDescriptor, "One");
      break;
    case PcWithin:
      if (pCond->CoImmediate)
        fprintf (fileDescriptor, "Immediately ");
      fprintf (fileDescriptor, "Within ");
      if (pCond->CoAncestorRel == CondGreater &&
          pCond->CoRelation != 0)
        fprintf (fileDescriptor, ">");
      else if (pCond->CoAncestorRel == CondLess)
        fprintf (fileDescriptor, "<");
      if (pCond->CoRelation > 0 ||
          pCond->CoAncestorRel == CondEquals)
        {
          wrnumber (pCond->CoRelation, fileDescriptor);
          fprintf (fileDescriptor, " ");
        }
      if (pCond->CoTypeAncestor == 0)
        {
          if (pCond->CoAncestorName)
            fprintf (fileDescriptor, pCond->CoAncestorName);
          fprintf (fileDescriptor, "(");
          fprintf (fileDescriptor, pCond->CoSSchemaName);
          fprintf (fileDescriptor, ")");
        }
      else
        wrrulename (pCond->CoTypeAncestor, fileDescriptor);
      fprintf (fileDescriptor, " ");
      break;
    case PcSibling:
      if (pCond->CoImmediate)
        fprintf (fileDescriptor, "Immediately ");
      fprintf (fileDescriptor, "After ");
      if (pCond->CoTypeAncestor == 0)
        {
          if (pCond->CoAncestorName)
            fprintf (fileDescriptor, pCond->CoAncestorName);
          fprintf (fileDescriptor, "(");
          fprintf (fileDescriptor, pCond->CoSSchemaName);
          fprintf (fileDescriptor, ")");
        }
      else
        wrrulename (pCond->CoTypeAncestor, fileDescriptor);
      fprintf (fileDescriptor, " ");
      break;
    case PcInterval:
      if (pCond->CoCounter > 0)
        {
          fprintf (fileDescriptor, "(");
          if (pCond->CoValCounter == CntMaxVal)
            fprintf (fileDescriptor, "MaxRangeVal ");
          else if (pCond->CoValCounter == CntMinVal)
            fprintf (fileDescriptor, "MinRangeVal ");
          fprintf (fileDescriptor, "Cpt");
          wrnumber (pCond->CoCounter, fileDescriptor);
          fprintf (fileDescriptor, ") ");
        }
      break;
    case PcElemType:
      wrrulename (pCond->CoTypeElem, fileDescriptor);
      fprintf (fileDescriptor, " ");
      break;
    case PcInheritAttribute:
      fprintf (fileDescriptor, "Inherited ");
    case PcAttribute:
      wrattrname (pCond->CoTypeAttr, fileDescriptor);
      if (pCond->CoTestAttrValue)
        {
          if (pSchemaStr->SsAttribute->TtAttr[pCond->CoTypeAttr - 1]->AttrType == AtTextAttr)
            {
              if (pCond->CoTextMatch == CoSubstring)
                fprintf (fileDescriptor, " |=");
              else if (pCond->CoTextMatch == CoWord)
                fprintf (fileDescriptor, " ~=");
              else
                fprintf (fileDescriptor, " =");
              if (pCond->CoAttrTextValue)
                fprintf (fileDescriptor, " %s", pCond->CoAttrTextValue);
            }
          else
            fprintf (fileDescriptor, " = %d", pCond->CoAttrValue);
        }
      fprintf (fileDescriptor, " ");
      break;
    default:
      break;
    }
  if (pCond->CoCondition == PcEven || pCond->CoCondition == PcOdd ||
      pCond->CoCondition == PcOne)
    if (pCond->CoCounter > 0)
      {
        fprintf (fileDescriptor, "(Cpt");
        wrnumber (pCond->CoCounter, fileDescriptor);
        fprintf (fileDescriptor, ") ");
      }
}

/*----------------------------------------------------------------------
  wrAllConditions
  ----------------------------------------------------------------------*/
static void wrAllConditions (PtrCondition pCond,  FILE *fileDescriptor)
{
  if (pCond)
    {
      if (pCond->CoCondition == PcDefaultCond)
	fprintf (fileDescriptor, "OTHERWISE ");
      else
	{
	  fprintf (fileDescriptor, "IF ");
	  wrCondition (pCond, fileDescriptor);
	}
      pCond = pCond->CoNextCondition;
      while (pCond != NULL)
	{
	  if (!pCond->CoChangeElem)
	    fprintf (fileDescriptor, "AND ");
	  wrCondition (pCond, fileDescriptor);
	  pCond = pCond->CoNextCondition;
	}
    }
}


/*----------------------------------------------------------------------
  wrFonctPres ecrit au terminal la fonction de presentation       
  contenue dans la regle pointee par pR.                  
  ----------------------------------------------------------------------*/
static void wrFonctPres (PtrPRule pR, FILE *fileDescriptor)
{
  int                 i;

  switch (pR->PrPresFunction)
    {
    case FnLine:
      fprintf (fileDescriptor, "Line");
      break;
    case FnNoLine:
      fprintf (fileDescriptor, "NoLine");
      break;
    case FnCreateBefore:
      fprintf (fileDescriptor, "CreateBefore");
      break;
    case FnCreateWith:
      fprintf (fileDescriptor, "CreateWith");
      break;
    case FnCreateFirst:
      fprintf (fileDescriptor, "CreateFirst");
      break;
    case FnCreateLast:
      fprintf (fileDescriptor, "CreateLast");
      break;
    case FnCreateAfter:
      fprintf (fileDescriptor, "CreateAfter");
      break;
    case FnContent:
      fprintf (fileDescriptor, "Content");
      break;
    case FnCreateEnclosing:
      fprintf (fileDescriptor, "CreateEnclosing");
      break;
    case FnPage:
      fprintf (fileDescriptor, "Page");
      break;
    case FnColumn:
      fprintf (fileDescriptor, "Column");
      break;
    case FnSubColumn:
      fprintf (fileDescriptor, "Included Column");
      break;
    case FnCopy:
      fprintf (fileDescriptor, "Copy");
      break;
    case FnContentRef:
      fprintf (fileDescriptor, "Content: Cste");
      wrnumber (pR->PrPresBox[0], fileDescriptor);
      break;
    case FnShowBox:
      fprintf (fileDescriptor, "ShowBox");
      break;
    case FnBackgroundPicture:
      fprintf (fileDescriptor, "BackgroundPicture: ");
      if (pR->PrPresBox[0] < 0)
        fprintf (fileDescriptor, "none");
      else
        {
          fprintf (fileDescriptor, "Cste");
          wrnumber (pR->PrPresBox[0], fileDescriptor);
        }
      break;
    case FnBackgroundRepeat:
      fprintf (fileDescriptor, "BackgroundRepeat: ");
      switch (pR->PrPresBox[0])
        {
        case RealSize:
          fprintf (fileDescriptor, "NormalSize");
          break;
        case ReScale:
          fprintf (fileDescriptor, "Scale");
          break;
        case FillFrame:
          fprintf (fileDescriptor, "RepeatXY");
          break;
        case XRepeat:
          fprintf (fileDescriptor, "RepeatX");
          break;
        case YRepeat:
          fprintf (fileDescriptor, "RepeatY");
          break;
        default:
          fprintf (fileDescriptor, "??????");
          break;
        }
      break;
    case FnNotInLine:
      fprintf (fileDescriptor, "InLine: No");
      break;
    case FnAny:
      fprintf (fileDescriptor, "??????");
      break;		    
    }
  if (pR->PrPresFunction != FnLine &&
      pR->PrPresFunction != FnContentRef &&
      pR->PrPresFunction != FnShowBox &&
      pR->PrPresFunction != FnBackgroundPicture &&
      pR->PrPresFunction != FnBackgroundRepeat &&
      pR->PrPresFunction != FnNoLine)
    {
      fprintf (fileDescriptor, "(");
      if (pR->PrPresFunction == FnContent)
        {
          fprintf (fileDescriptor, "Var");
          wrnumber (pR->PrPresBox[0], fileDescriptor);
        }
      else if (pR->PrNPresBoxes == 0)
        {
          fprintf (fileDescriptor, pR->PrPresBoxName);
          if (pR->PrExternal || !pR->PrElement)
            fprintf (fileDescriptor, "(****)");
        }
      else
        for (i = 1; i <= pR->PrNPresBoxes; i++)
          {
            if (i > 1)
              fprintf (fileDescriptor, ", ");
            if (pR->PrElement)
              wrrulename (pR->PrPresBox[i - 1], fileDescriptor);
            else
              wrboxname (pR->PrPresBox[i - 1], fileDescriptor);
          }
      fprintf (fileDescriptor, ")");
    }
  fprintf (fileDescriptor, ";");
}


/*----------------------------------------------------------------------
  wrajust ecrit au terminal la regle d'ajustement des lignes.     
  ----------------------------------------------------------------------*/
static void wrajust (PtrPRule pR, FILE *fileDescriptor)
{
  if (pR->PrPresMode == PresInherit)
    wrnbherit (pR, fileDescriptor);
  if (pR->PrPresMode == PresImmediate)
    switch (pR->PrAdjust)
	    {
      case AlignLeft:
        fprintf (fileDescriptor, "Left;");
        break;
      case AlignRight:
        fprintf (fileDescriptor, "Right;");
        break;
      case AlignCenter:
        fprintf (fileDescriptor, "VMiddle;");
        break;
      case AlignLeftDots:
        fprintf (fileDescriptor, "LeftWithDots;");
        break;
      case AlignJustify:
        fprintf (fileDescriptor, "Justify;");
        break;
	    }
}


/*----------------------------------------------------------------------
  wrjustif
  ecrit la regle booleenne pointee par pR.
  ----------------------------------------------------------------------*/
static void wrjustif (PtrPRule pR, FILE *fileDescriptor)
{
  if (pR->PrPresMode == PresInherit)
    wrnbherit (pR, fileDescriptor);
  if (pR->PrPresMode == PresImmediate)
    {
      if (pR->PrBoolValue)
        fprintf (fileDescriptor, "Yes;");
      else
        fprintf (fileDescriptor, "No;");
    }
}

/*----------------------------------------------------------------------
  wrprules displays the list of presentation rules pointed by RP.
  ----------------------------------------------------------------------*/
static void wrprules (PtrPRule RP, FILE *fileDescriptor, PtrPSchema pPSch)
{
  while (RP)
    {
      /* display a presentation rule */
      fprintf (fileDescriptor, "   ");
      if (RP->PrViewNum > 1)
        {
          fprintf (fileDescriptor, "IN ");
          fprintf (fileDescriptor, pSc1->PsView[RP->PrViewNum - 1]);
          fprintf (fileDescriptor, " ");
        }
      wrAllConditions (RP->PrCond, fileDescriptor);
      switch (RP->PrType)
        {
        case PtVisibility:
          fprintf (fileDescriptor, "Visibility: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtFunction:
          wrFonctPres (RP, fileDescriptor);
          break;
        case PtVertOverflow:
          fprintf (fileDescriptor, "VertOverflow: ");
          wrjustif (RP, fileDescriptor);
          break;
        case PtHorizOverflow:
          fprintf (fileDescriptor, "HorizOverflow: ");
          wrjustif (RP, fileDescriptor);
          break;
        case PtVertRef:
          fprintf (fileDescriptor, "VertRef: ");
          WrPos (RP->PrPosRule, False, fileDescriptor);
          break;
        case PtHorizRef:
          fprintf (fileDescriptor, "HorizRef: ");
          WrPos (RP->PrPosRule, False, fileDescriptor);
          break;
        case PtHeight:
          fprintf (fileDescriptor, "Height: ");
          wrdimens (RP->PrDimRule, True, fileDescriptor);
          break;
        case PtWidth:
          fprintf (fileDescriptor, "Width: ");
          wrdimens (RP->PrDimRule, False, fileDescriptor);
          break;
        case PtVertPos:
          fprintf (fileDescriptor, "VertPos: ");
          WrPos (RP->PrPosRule, True, fileDescriptor);
          break;
        case PtHorizPos:
          fprintf (fileDescriptor, "HorizPos: ");
          WrPos (RP->PrPosRule, True, fileDescriptor);
          break;
        case PtMarginTop:
          fprintf (fileDescriptor, "MarginTop: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtMarginRight:
          fprintf (fileDescriptor, "MarginRight: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtMarginBottom:
          fprintf (fileDescriptor, "MarginBottom: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtMarginLeft:
          fprintf (fileDescriptor, "MarginLeft: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtPaddingTop:
          fprintf (fileDescriptor, "PaddingTop: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtPaddingRight:
          fprintf (fileDescriptor, "PaddingRight: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtPaddingBottom:
          fprintf (fileDescriptor, "PaddingBottom: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtPaddingLeft:
          fprintf (fileDescriptor, "PaddingLeft: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtBorderTopWidth:
          fprintf (fileDescriptor, "BorderTopWidth: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtBorderRightWidth:
          fprintf (fileDescriptor, "BorderRightWidth: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtBorderBottomWidth:
          fprintf (fileDescriptor, "BorderBottomWidth: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtBorderLeftWidth:
          fprintf (fileDescriptor, "BorderLeftWidth: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtBorderTopColor:
          fprintf (fileDescriptor, "BorderTopColor: ");
          if (RP->PrPresMode == PresImmediate &&
              RP->PrValueType == PrNumValue && RP->PrIntValue == -2)
            fprintf (fileDescriptor, "transparent");
          else
            wrnbherit (RP, fileDescriptor);
          break;
        case PtBorderRightColor:
          fprintf (fileDescriptor, "BorderRightColor: ");
          if (RP->PrPresMode == PresImmediate &&
              RP->PrValueType == PrNumValue && RP->PrIntValue == -2)
            fprintf (fileDescriptor, "transparent");
          else
            wrnbherit (RP, fileDescriptor);
          break;
        case PtBorderBottomColor:
          fprintf (fileDescriptor, "BorderBottomColor: ");
          if (RP->PrPresMode == PresImmediate &&
              RP->PrValueType == PrNumValue && RP->PrIntValue == -2)
            fprintf (fileDescriptor, "transparent");
          else
            wrnbherit (RP, fileDescriptor);
          break;
        case PtBorderLeftColor:
          fprintf (fileDescriptor, "BorderLeftColor: ");
          if (RP->PrPresMode == PresImmediate &&
              RP->PrValueType == PrNumValue && RP->PrIntValue == -2)
            fprintf (fileDescriptor, "transparent");
          else
            wrnbherit (RP, fileDescriptor);
          break;
        case PtBorderTopStyle:
          fprintf (fileDescriptor, "BorderTopStyle: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtBorderRightStyle:
          fprintf (fileDescriptor, "BorderRightStyle: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtBorderBottomStyle:
          fprintf (fileDescriptor, "BorderBottomStyle: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtBorderLeftStyle:
          fprintf (fileDescriptor, "BorderLeftStyle: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtSize:
          fprintf (fileDescriptor, "Size: ");
          wrsize (RP, fileDescriptor);
          break;
        case PtStyle:
          fprintf (fileDescriptor, "Style: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtWeight:
          fprintf (fileDescriptor, "Weight: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtVariant:
          fprintf (fileDescriptor, "Variant: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtFont:
          fprintf (fileDescriptor, "Font: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtUnderline:
          fprintf (fileDescriptor, "Underline: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtThickness:
          fprintf (fileDescriptor, "Thickness: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtIndent:
          fprintf (fileDescriptor, "Indent: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtLineSpacing:
          fprintf (fileDescriptor, "LineSpacing: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtDepth:
          fprintf (fileDescriptor, "Depth: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtAdjust:
          fprintf (fileDescriptor, "Adjust: ");
          wrajust (RP, fileDescriptor);
          break;
        case PtDirection:
          fprintf (fileDescriptor, "Direction: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtUnicodeBidi:
          fprintf (fileDescriptor, "UnicodeBidi: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtLineStyle:
          fprintf (fileDescriptor, "LineStyle: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtLineWeight:
          fprintf (fileDescriptor, "LineWeight: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtFillPattern:
          fprintf (fileDescriptor, "FillPattern: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtOpacity:
          fprintf (fileDescriptor, "Opacity: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtFillOpacity:
          fprintf (fileDescriptor, "FillOpacity: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtStrokeOpacity:
          fprintf (fileDescriptor, "StrokeOpacity: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtStopOpacity:
          fprintf (fileDescriptor, "StopOpacity: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtMarker:
          fprintf (fileDescriptor, "Marker: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtMarkerStart:
          fprintf (fileDescriptor, "MarkerStart: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtMarkerMid:
          fprintf (fileDescriptor, "MarkerMid: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtMarkerEnd:
          fprintf (fileDescriptor, "MarkerEnd: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtFillRule:
          fprintf (fileDescriptor, "FillRule: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtBackground:
          fprintf (fileDescriptor, "Background: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtForeground:
          fprintf (fileDescriptor, "Foreground: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtColor:
          fprintf (fileDescriptor, "Color: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtStopColor:
          fprintf (fileDescriptor, "StopColor: ");
          wrnbherit (RP, fileDescriptor);
          break;
        case PtHyphenate:
          fprintf (fileDescriptor, "Hyphenate: ");
          wrjustif (RP, fileDescriptor);
          break;
        case PtPageBreak:
          fprintf (fileDescriptor, "PageBreak: ");
          wrjustif (RP, fileDescriptor);
          break;
        case PtLineBreak:
          fprintf (fileDescriptor, "LineBreak: ");
          wrjustif (RP, fileDescriptor);
          break;
        case PtGather:
          fprintf (fileDescriptor, "Gather: ");
          wrjustif (RP, fileDescriptor);
          break;
        case PtXRadius:
          fprintf (fileDescriptor, "XRadius: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtYRadius:
          fprintf (fileDescriptor, "YRadius: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtTop:
          fprintf (fileDescriptor, "Top: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtRight:
          fprintf (fileDescriptor, "Right: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtBottom:
          fprintf (fileDescriptor, "Bottom: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtLeft:
          fprintf (fileDescriptor, "Left: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtBackgroundHorizPos:
          fprintf (fileDescriptor, "BackgroundHorizPos: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtBackgroundVertPos:
          fprintf (fileDescriptor, "BackgroundVertPos: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtDisplay:
          fprintf (fileDescriptor, "Display: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtListStyleType:
          fprintf (fileDescriptor, "ListStyleType: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtListStyleImage:
          fprintf (fileDescriptor, "ListStyleImage: ");
          if (RP->PrPresMode == PresInherit)
            if (RP->PrInhPercent)
              fprintf (fileDescriptor, "??????");
            else
              {
                wrModeHerit (RP->PrInheritMode, fileDescriptor);
                if (RP->PrInhDelta != 0)
                  fprintf (fileDescriptor, "????");
                else
                  fprintf (fileDescriptor, " =");
              }
          else if (RP->PrPresMode == PresImmediate)
           {
            if (RP->PrValueType != PrNumValue)
              fprintf (fileDescriptor, "??");
            else
              if (RP->PrIntValue == 0)
                fprintf (fileDescriptor, "None");
              else if (pPSch->PsConstant[RP->PrIntValue-1].PdString)
                fprintf (fileDescriptor, pPSch->PsConstant[RP->PrIntValue-1].PdString);
           }
          break;
        case PtListStylePosition:
          fprintf (fileDescriptor, "ListStylePosition: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtFloat:
          fprintf (fileDescriptor, "Float: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtClear:
          fprintf (fileDescriptor, "Clear: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtPosition:
          fprintf (fileDescriptor, "Position: ");
          wrfontstyle (RP, fileDescriptor);
          break;
        case PtVis:
          fprintf (fileDescriptor, "Visibility: ");
          switch (RP->PrChrValue)
            {
            case 'H':
              fprintf (fileDescriptor, "hidden");
              break;
            case 'V':
              fprintf (fileDescriptor, "visible");
              break;
            case 'C':
              fprintf (fileDescriptor, "collapse");
              break;
            case 'I':
              fprintf (fileDescriptor, "inherit");
              break;
            default:
              fprintf (fileDescriptor, "??");
              break;
            }
          break;
        case PtBreak1:
          fprintf (fileDescriptor, "NoBreak1: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtBreak2:
          fprintf (fileDescriptor, "NoBreak2: ");
          wrminind (RP, fileDescriptor);
          break;
        case PtPictInfo:
          break;
        }
      fprintf (fileDescriptor, " {-- ");
      if (RP->PrBoxType == BtBefore)
        fprintf (fileDescriptor, ":before ");
      else if (RP->PrBoxType == BtAfter)
        fprintf (fileDescriptor, ":after ");
      if (RP->PrImportant)
        fprintf (fileDescriptor, "!important ");
      fprintf (fileDescriptor, "specificity=%d,", RP->PrSpecificity);
      fprintf (fileDescriptor, " line=%d --}\n", RP->PrCSSLine);
      /* next rule */
      RP = RP->PrNextPRule;
    }
}

/*----------------------------------------------------------------------
  TtaListStyleSchemas
  Produces in a file a human-readable form of style schemas applied to 
  the current document.
  Parameters:
  document: the document.
  fileDescriptor: file descriptor of the file that will contain the list.
  This file must be open when calling the function.
  ----------------------------------------------------------------------*/
void TtaListStyleSchemas (Document document, FILE *fileDescriptor)
{
  PtrDocument         pDoc;
  PtrDocSchemasDescr  pPfS;
  PtrHandlePSchema    pHd;
  PresConstant       *pPr1;
  PtrPresVariable     pVar;
  PresVarItem        *pItem;
  PtrTtAttribute      pAt1;
  AttributePres      *pRP1;
  NumAttrCase        *pCa1;
  char               *ptr, c;
  int                 i, j;
  int                 el, attr, val;
   
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parametre document correct */
    {
      pDoc = LoadedDocument[document - 1];
      pPfS = pDoc->DocFirstSchDescr;
      while (pPfS)
        {
          pSchemaStr = (PtrSSchema) pPfS->PfSSchema;
          if (pSchemaStr)
            pHd = FirstPSchemaExtension (pSchemaStr, pDoc, NULL);
          else
            pHd = NULL;
          while (pHd)
            {
              pSc1 = pHd->HdPSchema;
              if (pSc1 != NULL)
                {
                  fprintf (fileDescriptor, "{-----------------------  ");
                  switch (pSc1->PsOrigin)
                    {
                    case Agent: fprintf (fileDescriptor, "User Agent ");
                      break;
                    case User: fprintf (fileDescriptor, "User ");
                      break;
                    case Author: fprintf (fileDescriptor, "Author ");
                      break;
                    }
                  fprintf (fileDescriptor, "(%s) ", pSc1->PsPresentName);
                  fprintf (fileDescriptor, " -----------------------}\n\n");
                  fprintf (fileDescriptor, "PRESENTATION ");
                  if (pSchemaStr->SsName)
                    wrtext (pSchemaStr->SsName, fileDescriptor);
                  else
                    fprintf (fileDescriptor, "???");
                  fprintf (fileDescriptor, ";\n");

                  /* print constants */
                  if (pSc1->PsNConstants > 0)
                    {
                      fprintf (fileDescriptor, "\n");
                      fprintf (fileDescriptor, "CONST\n");
                      for (i = 1; i <= pSc1->PsNConstants; i++)
                        {
                          pPr1 = &pSc1->PsConstant[i - 1];
                          /* donne un nom a chaque constante */
                          fprintf (fileDescriptor, "   Cste");
                          wrnumber (i, fileDescriptor);
                          fprintf (fileDescriptor, "= ");
                          /* ecrit le type de la constante */
                          switch (pPr1->PdType)
                            {
                            case CharString:
                              fprintf (fileDescriptor, "Text ");
                              fprintf (fileDescriptor, "%c", pPr1->PdScript);
                              break;
                            case GraphicElem:
                              fprintf (fileDescriptor, "Graphics ");
                              break;
                            case Symbol:
                              fprintf (fileDescriptor, "Symbol ");
                              break;
                            case tt_Picture:
                              fprintf (fileDescriptor, "Picture ");
                              /* ecrit la valeur de la constante */
                              break;
                            default:
                              break;
                            }
			 
                          fprintf (fileDescriptor, " \'");
                          ptr = pPr1->PdString;
                          if (ptr)
                            {
                              while (*ptr != EOS)
                                {
                                  c = *ptr;
                                  if (c < ' ')
                                    fprintf (fileDescriptor, "\\%3d", (int)c);
                                  else
                                    fprintf (fileDescriptor, "%c", c);
                                  ptr++;
                                }
                            }
                          fprintf (fileDescriptor, "\';\n");
                        }
                    }

                  /* print variables */
                  if (pSc1->PsNVariables > 0)
                    {
                      fprintf (fileDescriptor, "\n");
                      fprintf (fileDescriptor, "\nVARIABLES\n");
                      for (i = 0; i < pSc1->PsNVariables; i++)
                        {
                          pVar = pSc1->PsVariable->PresVar[i];
                          fprintf (fileDescriptor, "   Var");
                          wrnumber (i+1, fileDescriptor);
                          fprintf (fileDescriptor, "= ");
                          for (j = 0; j < pVar->PvNItems; j++)
                            {
                              pItem = &pVar->PvItem[j];
                              switch (pItem->ViType)
                                {
                                case VarText:
                                  fprintf (fileDescriptor, " Cste");
                                  wrnumber (pItem->ViConstant, fileDescriptor);
                                  break;
                                case VarNamedAttrValue:
                                  fprintf (fileDescriptor, " {attr value}Cste");
                                  wrnumber (pItem->ViConstant, fileDescriptor);
                                  break;
                                case VarCounter:
                                  fprintf (fileDescriptor, " Cntr");
                                  wrnumber (pItem->ViCounter, fileDescriptor);
				  break;
                                case VarDate:
                                case VarFDate:
                                case VarDirName:
                                case VarDocName:
                                case VarElemName:
                                case VarAttrName:
                                case VarAttrValue:
                                case VarPageNumber:
                                  break;
                                } 
                            }
                          fprintf (fileDescriptor, ";\n");
                        }
                    }

                  /* print default presentation rules */
                  if (pSc1->PsFirstDefaultPRule)
                    {
                      fprintf (fileDescriptor, "\nDEFAULT\n\n");
                      wrprules (pSc1->PsFirstDefaultPRule, fileDescriptor,
                                pSc1);
                    }

                  /* print presentation boxes */
                  if (pSc1->PsNPresentBoxes > 0)
                    {
                      fprintf (fileDescriptor, "\nBOXES\n\n");
                      for (i = 1; i <= pSc1->PsNPresentBoxes; i++)
                        {
                          fprintf (fileDescriptor, pSc1->PsPresentBox->PresBox[i-1]->PbName);
                          fprintf (fileDescriptor, ":\n");
                          fprintf (fileDescriptor, "   BEGIN\n");
                          wrprules (pSc1->PsPresentBox->PresBox[i-1]->PbFirstPRule, fileDescriptor, pSc1);
                          fprintf (fileDescriptor, "   END;\n");
                          fprintf (fileDescriptor, "\n");
                        }
                    }

                  /* les regles de presentation des elements structure's */
                  fprintf (fileDescriptor, "\nRULES\n\n");
                  for (el = 0; el < pSchemaStr->SsNRules; el++)
                    {
                      if (pSc1->PsElemPRule->ElemPres[el])
                        {
                          if (pSchemaStr->SsRule->SrElem[el]->SrConstruct == CsPairedElement)
                            {
                              if (pSchemaStr->SsRule->SrElem[el]->SrFirstOfPair)
                                fprintf (fileDescriptor, "First ");
                              else
                                fprintf (fileDescriptor, "Second ");
                            }
                          wrrulename (el + 1, fileDescriptor);
                          fprintf (fileDescriptor, ":\n");
                          fprintf (fileDescriptor, "   BEGIN\n");
                          wrprules (pSc1->PsElemPRule->ElemPres[el], fileDescriptor, pSc1);
                          fprintf (fileDescriptor, "   END;\n");
                          fprintf (fileDescriptor, "\n");
                        }
                    }

                  /* les regles de presentation des attributs */
                  if (pSchemaStr->SsNAttributes > 0)
                    {
                      fprintf (fileDescriptor, "\nATTRIBUTES\n\n");
                      for (attr = 0; attr < pSchemaStr->SsNAttributes; attr++)
                        {
                          pAt1 = pSchemaStr->SsAttribute->TtAttr[attr];
                          if (pSc1->PsNAttrPRule->Num[attr] == 0)
                            pRP1 = NULL;
                          else
                            pRP1 = pSc1->PsAttrPRule->AttrPres[attr];
                          while (pRP1)
                            {
                              /* si cet attribut a une presentation */
                              switch (pAt1->AttrType)
                                {
                                case AtNumAttr:
                                  for (i = 1; i <= pRP1->ApNCases; i++)
                                    {
                                      pCa1 = &pRP1->ApCase[i - 1];
                                      wrtext (pAt1->AttrName, fileDescriptor);
                                      if (pRP1->ApElemType > 0)
                                        {
                                          fprintf (fileDescriptor, "(");
                                          wrrulename (pRP1->ApElemType, fileDescriptor);
                                          if (pRP1->ApElemInherits)
                                            fprintf (fileDescriptor, " *");
                                          fprintf (fileDescriptor, ")");
                                        }
                                      if (pCa1->CaLowerBound == pCa1->CaUpperBound)
                                        {
                                          fprintf (fileDescriptor, "=");
                                          wrnumber (pCa1->CaLowerBound, fileDescriptor);
                                        }
                                      else if (pCa1->CaLowerBound != -MAX_INT_ATTR_VAL - 1
                                               && pCa1->CaUpperBound != MAX_INT_ATTR_VAL + 1)
                                        {
                                          fprintf (fileDescriptor, " IN [");
                                          wrnumber (pCa1->CaLowerBound, fileDescriptor);
                                          fprintf (fileDescriptor, "..");
                                          wrnumber (pCa1->CaUpperBound, fileDescriptor);
                                          fprintf (fileDescriptor, "] ");
                                        }
                                      else if (pCa1->CaLowerBound != -MAX_INT_ATTR_VAL - 1
                                               || pCa1->CaUpperBound != MAX_INT_ATTR_VAL + 1)
                                        {
                                          if (pCa1->CaLowerBound != -MAX_INT_ATTR_VAL - 1)
                                            {
                                              fprintf (fileDescriptor, ">");
                                              wrnumber (pCa1->CaLowerBound - 1, fileDescriptor);
                                            }
                                          else if (pCa1->CaUpperBound != MAX_INT_ATTR_VAL + 1)
                                            {
                                              fprintf (fileDescriptor, "<");
                                              wrnumber (pCa1->CaUpperBound + 1, fileDescriptor);
                                            }
                                        }
                                      fprintf (fileDescriptor, ":\n");
                                      if (pCa1->CaFirstPRule == NULL)
                                        fprintf (fileDescriptor, "   BEGIN END;\n");
                                      else
                                        {
                                          if (pCa1->CaFirstPRule->PrNextPRule)
                                            fprintf (fileDescriptor, "   BEGIN\n");
                                          wrprules (pCa1->CaFirstPRule, fileDescriptor, pSc1);
                                          if (pCa1->CaFirstPRule->PrNextPRule)
                                            fprintf (fileDescriptor, "   END;\n");
                                        }
                                      fprintf (fileDescriptor, "\n");
                                    }
                                  break;
                                case AtTextAttr:
                                  if (pRP1->ApTextFirstPRule)
                                    {
                                      wrtext (pAt1->AttrName, fileDescriptor);
                                      if (pRP1->ApElemType > 0)
                                        {
                                          fprintf (fileDescriptor, "(");
                                          wrrulename (pRP1->ApElemType, fileDescriptor);
                                          if (pRP1->ApElemInherits)
                                            fprintf (fileDescriptor, " *");
                                          fprintf (fileDescriptor, ")");
                                        }
                                      if (pRP1->ApString)
                                        {
                                          if (pRP1->ApMatch == CoWord)
                                            fprintf (fileDescriptor, "~");
                                          else if (pRP1->ApMatch == CoSubstring)
                                            fprintf (fileDescriptor, "|");
                                          fprintf (fileDescriptor, "=\'");
                                          if (*pRP1->ApString != EOS)
                                            wrtext (pRP1->ApString, fileDescriptor);
                                          fprintf (fileDescriptor, "\'");
                                        }
                                      fprintf (fileDescriptor, ":\n");
                                      if (pRP1->ApTextFirstPRule->PrNextPRule)
                                        fprintf (fileDescriptor, "   BEGIN\n");
                                      wrprules (pRP1->ApTextFirstPRule, fileDescriptor, pSc1);
                                      if (pRP1->ApTextFirstPRule->PrNextPRule)
                                        fprintf (fileDescriptor, "   END;\n");
                                      fprintf (fileDescriptor, "\n");
                                    }
                                  break;
                                case AtReferenceAttr:
                                  if (pRP1->ApRefFirstPRule)
                                    {
                                      wrtext (pAt1->AttrName, fileDescriptor);
                                      if (pRP1->ApElemType > 0)
                                        {
                                          fprintf (fileDescriptor, "(");
                                          wrrulename (pRP1->ApElemType, fileDescriptor);
                                          if (pRP1->ApElemInherits)
                                            fprintf (fileDescriptor, " *");
                                          fprintf (fileDescriptor, ")");
                                        }
                                      fprintf (fileDescriptor, ":\n");
                                      if (pRP1->ApRefFirstPRule->PrNextPRule)
                                        fprintf (fileDescriptor, "   BEGIN\n");
                                      wrprules (pRP1->ApRefFirstPRule, fileDescriptor, pSc1);
                                      if (pRP1->ApRefFirstPRule->PrNextPRule)
                                        fprintf (fileDescriptor, "   END;\n");
                                      fprintf (fileDescriptor, "\n");
                                    }
                                  break;
                                case AtEnumAttr:
                                  for (val = 0; val <= pAt1->AttrNEnumValues; val++)
                                    if (pRP1->ApEnumFirstPRule[val] != NULL)
                                      {
                                        wrtext (pAt1->AttrName, fileDescriptor);
                                        if (pRP1->ApElemType > 0)
                                          {
                                            fprintf (fileDescriptor, "(");
                                            wrrulename (pRP1->ApElemType, fileDescriptor);
                                            if (pRP1->ApElemInherits)
                                              fprintf (fileDescriptor, " *");
                                            fprintf (fileDescriptor, ")");
                                          }
                                        if (val > 0 && pAt1->AttrEnumValue[val-1])
                                          {
                                            fprintf (fileDescriptor, "=");
                                            wrtext (pAt1->AttrEnumValue[val-1], fileDescriptor);
                                          }
                                        fprintf (fileDescriptor, ":\n");
                                        if (pRP1->ApEnumFirstPRule[val]->PrNextPRule)
                                          fprintf (fileDescriptor, "   BEGIN\n");
                                        wrprules (pRP1->ApEnumFirstPRule[val], fileDescriptor, pSc1);
                                        if (pRP1->ApEnumFirstPRule[val]->PrNextPRule)
                                          fprintf (fileDescriptor, "   END;\n");
                                        fprintf (fileDescriptor, "\n");
                                      }
                                  break;
                                default:;
                                  break;
                                }
                              pRP1 = pRP1->ApNextAttrPres;
                            }
                        }
                    }
                }
              pHd = pHd->HdNextPSchema;
            }
          pPfS = pPfS->PfNext;
        }
    }
}

/*----------------------------------------------------------------------
  DisplayCounterRule displays an item of a counter in the CSS format.
  ----------------------------------------------------------------------*/
void DisplayCounterRule (int counter, int item, FILE *fileDescriptor,
			 PtrElement pEl, PtrPSchema pSchP)
{
  CntrItem        *CntItem;
  char            *name;
  int              l;

  CntItem = &pSchP->PsCounter[counter].CnItem[item];
  /* if there is no number for the source line, don't do anything */
  if (CntItem->CiCSSLine == 0)
    return;
  l = 0;
  fprintf (fileDescriptor, "@");
  if (CntItem->CiCntrOp == CntrSet)
    {
      fprintf (fileDescriptor, "counter-reset: ");
      l += 15;
    }
  else if (CntItem->CiCntrOp == CntrAdd)
    {
      fprintf (fileDescriptor, "counter-increment: ");
      l += 19;
    }
  name = pSchP->PsConstant[pSchP->PsCounter[counter].CnNameIndx-1].PdString;
  fprintf (fileDescriptor, name);
  l += strlen (name);
  if ((CntItem->CiCntrOp == CntrAdd && CntItem->CiParamValue != 1) ||
      (CntItem->CiCntrOp == CntrSet && CntItem->CiParamValue != 0))
    {
      fprintf (fileDescriptor, " %d", CntItem->CiParamValue);
      l += 2;
      if (CntItem->CiParamValue > 9)
	l++;
      if (CntItem->CiParamValue > 99)
	l++;
      if (CntItem->CiParamValue > 999)
	l++;
    }
  fprintf (fileDescriptor, "; ");
  l += 2;
  while (l < 30)
    {
      fprintf (fileDescriptor, " ");
      l++;
    }

  if (CntItem->CiCSSURL)
    fprintf (fileDescriptor, "line %d, file %s\n", CntItem->CiCSSLine,
             CntItem->CiCSSURL);
  else
    fprintf (fileDescriptor, "line %d, style element\n", CntItem->CiCSSLine);
  DisplayedRuleCounter++;
}

/*----------------------------------------------------------------------
  DisplayPRule displays the presentation rule in the CSS format.
  ----------------------------------------------------------------------*/
void DisplayPRule (PtrPRule rule, FILE *fileDescriptor,
                   PtrElement pEl, PtrPSchema pSchP, int indent)
{
  PresentationSettingBlock setting;
  char                     buffer[200], *ptr;
  int                      l, i;
  PtrPresVariable          var;
  PresVarItem              *item;

  if (rule == NULL)
    return;
  if (rule->PrSpecificity == 2000)
    /* this is not really a CSS style rule, but the equivalent of an
       HTML, SVG or MathML attribute */
    return;
  /* if there is no number for the source line, don't do anything */
  if (rule->PrCSSLine == 0)
    return;

  PRuleToPresentationSetting (rule, &setting, pSchP);
  buffer[0] = EOS;
  TtaPToCss (&setting, buffer, 199, (Element) pEl, (void*)pSchP);
  if (buffer[0] == EOS)
    return;
  /* display the rule */
  fprintf (fileDescriptor, "@");
  for (i = 0; i < indent; i++)
    fprintf (fileDescriptor, " ");
  fprintf (fileDescriptor, "%s", buffer);
  l = strlen (buffer) + indent;
  DisplayedRuleCounter++;

  if (rule->PrType == PtFunction && rule->PrPresFunction == FnContent)
    /* it's a content rule. List all rules of the created box */
    {
      var = pSchP->PsVariable->PresVar[rule->PrPresBox[0] - 1];
      for (i = 0; i < var->PvNItems; i++)
        {
          item = &var->PvItem[i];
          switch (item->ViType)
            {
            case VarText:
              ptr = pSchP->PsConstant[item->ViConstant - 1].PdString;
              if (pSchP->PsConstant[item->ViConstant - 1].PdType == CharString)
                {
                  fprintf (fileDescriptor, " \"");
                  l += 3;
                  if (ptr)
                    {
                      fprintf (fileDescriptor, "%s", ptr);
                      l += strlen(ptr);
                    }
                  fprintf (fileDescriptor, "\"");
                }
              else if (pSchP->PsConstant[item->ViConstant - 1].PdType == tt_Picture)
                {
                  fprintf (fileDescriptor, " url(\"");
                  l += 7;
                  if (ptr)
                    {
                      fprintf (fileDescriptor, "%s", ptr);
                      l += strlen(ptr);
                    }
                  fprintf (fileDescriptor, "\"");
                }
              break;
            case VarCounter:
	      if (pSchP->PsCounter[item->ViCounter - 1].CnNameIndx == 0)
		{
                   fprintf (fileDescriptor, " counter(Cnt%d", item->ViCounter);
		   l += 13;
                   if (item->ViCounter > 9)
		     l++;
                   if (item->ViCounter > 99)
		     l++;
                   if (item->ViCounter > 999)
		     l++;
		}
	      else
		{
		  fprintf (fileDescriptor, " counter(%s",
			   pSchP->PsConstant[pSchP->PsCounter[item->ViCounter-1].CnNameIndx-1].PdString);
		  l+= 9;
		  l+= strlen(pSchP->PsConstant[pSchP->PsCounter[item->ViCounter-1].CnNameIndx-1].PdString);
		
		}
	      if (item->ViStyle != CntDecimal)
		{
		  fprintf (fileDescriptor, ", ");
		  l+= 2;
		  switch (item->ViStyle)
		    {
		    case CntDisc:
		      fprintf (fileDescriptor, "disc");
		      l+= 4;
		      break;
		    case CntCircle:
		      fprintf (fileDescriptor, "circle");
		      l+= 6;
		      break;
		    case CntSquare:
		      fprintf (fileDescriptor, "square");
		      l+= 6;
		      break;
		    case CntDecimal:
		      fprintf (fileDescriptor, "decimal");
		      l+= 7;
		      break;
		    case CntZLDecimal:
		      fprintf (fileDescriptor, "decimal-leading-zero");
		      l+= 20;
		      break;
		    case CntLRoman:
		      fprintf (fileDescriptor, "lower-roman");
		      l+= 11;
		      break;
		    case CntURoman:
		      fprintf (fileDescriptor, "upper-roman");
		      l+= 11;
		      break;
		    case CntLGreek:
		      fprintf (fileDescriptor, "lower-greek");
		      l+= 11;
		      break;
		    case CntUGreek:
		      fprintf (fileDescriptor, "upper-greek");
		      l+= 11;
		      break;
		    case CntLowercase:
		      fprintf (fileDescriptor, "lower-latin");
		      l+= 11;
		      break;
		    case CntUppercase:
		      fprintf (fileDescriptor, "upper-latin"); 
		      l+= 11;
		      break;
		    case CntNone:
		      fprintf (fileDescriptor, "none");
		      l+= 4;
		      break;
		    }
		}
	      fprintf (fileDescriptor, ")");
              l++;
              break;
            case VarAttrValue:
              fprintf (fileDescriptor, " attr(%s)", pSchP->PsSSchema->SsAttribute->TtAttr[item->ViAttr - 1]->AttrName);
	      l = l + 7 + strlen(pSchP->PsSSchema->SsAttribute->TtAttr[item->ViAttr - 1]->AttrName);
              break;
            case VarNamedAttrValue:
              ptr = pSchP->PsConstant[item->ViConstant - 1].PdString;
              if (ptr)
		{
		  fprintf (fileDescriptor, " attr(%s)", ptr);
		  l = l + 7 + strlen(ptr);
		}
              break;
            default:
              break;
            }
        }
    }
  if (rule->PrImportant)
    {
      fprintf (fileDescriptor, " !important");
      l += 11;
    }
  fprintf (fileDescriptor, "; ");
  l += 2;

  while (l < 30)
    {
      fprintf (fileDescriptor, " ");
      l++;
    }
  if (rule->PrSpecificity == 1000)
    fprintf (fileDescriptor, "line %d, style attribute\n", rule->PrCSSLine);
  else if (rule->PrCSSURL)
    fprintf (fileDescriptor, "line %d, file %s\n", rule->PrCSSLine,
             rule->PrCSSURL);
  else
    fprintf (fileDescriptor, "line %d, style element\n", rule->PrCSSLine);
}

/*----------------------------------------------------------------------
  TtaListStyleOfCurrentElement
  Produces in a file a human-readable form of style rules applied to 
  the first selected element.
  Parameters:
  document: the document.
  el: the element.
  fileDescriptor: file descriptor of the file that will contain the list.
  This file must be open when calling the function.
  Returns the number of rules generated.
  ----------------------------------------------------------------------*/
int TtaListStyleOfCurrentElement (Document document, FILE *fileDescriptor)
{
  PtrDocument         pDoc;
  PtrPSchema          pSchP;
  PtrSSchema          pSchS;
  Element             El;
  PtrElement          pEl;
  PtrPRule            pRDef, pRSpec;
  PtrAbstractBox      pAb, pNew;
  int                 lqueue = 0, f, l;
  int                 index, viewSch;

  /* Number of rules displayed */
  DisplayedRuleCounter = 0;
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      pDoc = LoadedDocument[document - 1];
      TtaGiveFirstSelectedElement (document, &El, &f, &l);
      pEl = (PtrElement) El;
      /* if it's a leaf, get the rules for the parent element instead */
      if (pEl && pEl->ElTerminal &&
          (pEl->ElLeafType == LtText ||
           pEl->ElLeafType == LtGraphics ||
           pEl->ElLeafType == LtPath ||
           pEl->ElLeafType == LtPolyLine ||
           pEl->ElLeafType == LtSymbol ||
           pEl->ElLeafType == LtPicture))
        pEl = pEl->ElParent;
      /* if element is hidden, get the rules for the parent element instead */
      while (pEl && TypeHasException (ExcHidden, pEl->ElTypeNumber,
                                      pEl->ElStructSchema))
        pEl = pEl->ElParent;
      if (pEl)
        {
          SearchPresSchema (pEl, &pSchP, &index, &pSchS, pDoc);
          viewSch = AppliedView (pEl, NULL, pDoc, 1);
          if (pSchP)
            {
              pRSpec = pSchP->PsElemPRule->ElemPres[index - 1];
              /* first default presentation rule */
              pRDef = pSchP->PsFirstDefaultPRule;
              pAb = pEl->ElAbstractBox[0];
              pNew = pAb;
	      /* get the presentation rules that apply to this element */
              ApplyPresRules (pEl, pDoc, 1, viewSch, pSchS, pSchP,
                              &pRSpec, &pRDef, &pAb, FALSE, &lqueue, NULL,
                              pNew, NULL, fileDescriptor, FALSE);
            }
        }
    }
  return DisplayedRuleCounter;
}
