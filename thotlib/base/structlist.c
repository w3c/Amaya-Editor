/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"

#include "language.h"
#include "application.h"
#include "libmsg.h"
#include "message.h"
#include "fileaccess.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "boxes_tv.h"

#define MAXNB 200
static int          NbChildren[MAXNB];
static int          LeafLevel[MAXNB];
static int          NbElemLevels[MAXNB];
static int          NbAscendSiblings[MAXNB];
static int          NbNodes;
static int          NbLeaves;
static PtrSSchema   pSchemaStr;
static PtrPSchema   pSc1;
char                mbsTmpStr[MAX_TXT_LEN];

#include "absboxes_f.h"
#include "fileaccess_f.h"
#include "presrules_f.h"
#include "structlist_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "viewapi_f.h"

/*----------------------------------------------------------------------
   WrPRuleType ecrit le type de la regle de presentation       
   pointee par pRule, dans le fichier fileDescriptor.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         WrPRuleType (PtrPRule pRule, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         WrPRuleType (pRule, fileDescriptor)
PtrPRule            pRule;
FILE               *fileDescriptor;

#endif /* __STDC__ */
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
		  case FnPictureMode:
		     fprintf (fileDescriptor, "PictureMode");
		     break;
		  case FnNotInLine:
		     fprintf (fileDescriptor, "InLine: No");
		     break;
		  default:
		     fprintf (fileDescriptor, "PrPresFunction ????");
		     break;
		  }
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
	    case PtUnderline:
	       fprintf (fileDescriptor, "Souligne");
	       break;
	    case PtThickness:
	       fprintf (fileDescriptor, "Epaisseur");
	       break;
	    case PtStyle:
	       fprintf (fileDescriptor, "Style");
	       break;
	    case PtWeight:
	       fprintf (fileDescriptor, "Weight");
	       break;
	    case PtFont:
	       fprintf (fileDescriptor, "Font");
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
	    case PtJustify:
	       fprintf (fileDescriptor, "Justify");
	       break;
	    case PtHyphenate:
	       fprintf (fileDescriptor, "Hyphenate");
	       break;
	    case PtVertOverflow:
	       fprintf (fileDescriptor, "VertOverflow");
	       break;
	    case PtHorizOverflow:
	       fprintf (fileDescriptor, "HorizOverflow");
	       break;
	    case PtBreak1:
	       fprintf (fileDescriptor, "NoBr1");
	       break;
	    case PtBreak2:
	       fprintf (fileDescriptor, "NoBr2");
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
	    case PtBackground:
	       fprintf (fileDescriptor, "Background");
	       break;
	    case PtForeground:
	       fprintf (fileDescriptor, "Foreground");
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
#ifdef __STDC__
static void         WrText (PtrTextBuffer pBT, int length, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         WrText (pBT, length, fileDescriptor)
PtrTextBuffer       pBT;
int                 length;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{
   PtrTextBuffer       b;
   int                 i, l;

   l = 0;
   b = pBT;
   while (b != NULL && l < length)
      /* ecrit le contenu du buffer de texte */
     {
	i = 0;
	while (i < b->BuLength && b->BuContent[i] != EOS && l < length)
	  {
	     putc (b->BuContent[i], fileDescriptor);
	     i++;
	     l++;
	  }
	if (l < length)
	  {
	     fprintf (fileDescriptor, "|");
	     b = b->BuNext;
	     /* buffer de texte suivant du meme element */
	  }
	else
	   fprintf (fileDescriptor, "...");
     }
}

/*----------------------------------------------------------------------
   CountNodes ecrit dans le fichier fileDescriptor les statistiques       
   sur le sous-arbre de racine pNode.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CountNodes (PtrElement pNode, FILE * fileDescriptor, int level)
#else  /* __STDC__ */
void                CountNodes (pNode, fileDescriptor, level)
PtrElement          pNode;
FILE               *fileDescriptor;
int                 level;
#endif /* __STDC__ */

{
   int                 i;
   int                 nbf;
   int                 der;
   int                 Prof, Fils;
   float               Moyenne;
   PtrElement          f;
   PtrElement          pEl;
   PtrElement          pAsc;
   SRule              *pRe1;

   if (pNode != NULL)
     {
	if (level == 0)
	  {
	     for (i = 0; i < MAXNB; i++)
	       {
		  NbChildren[i] = 0;
		  LeafLevel[i] = 0;
		  NbElemLevels[i] = 0;
		  NbAscendSiblings[i] = 0;
	       }
	     NbNodes = 0;
	     NbLeaves = 0;
	  }
	pEl = pNode;
	NbNodes++;
	NbElemLevels[level]++;
	pAsc = pEl->ElParent;
	nbf = 0;
	while (pAsc != NULL)
	  {
	     f = pAsc->ElPrevious;
	     while (f != NULL)
	       {
		  nbf++;
		  f = f->ElPrevious;
	       }
	     f = pAsc->ElNext;
	     while (f != NULL)
	       {
		  nbf++;
		  f = f->ElNext;
	       }
	     pAsc = pAsc->ElParent;
	  }
	NbAscendSiblings[level] += nbf;
	if (pEl->ElTerminal)
	  {
	     LeafLevel[level]++;
	     NbLeaves++;
	  }
	else
	  {
	     /* element non terminal, on traite sa descendance */
	     f = pEl->ElFirstChild;
	     nbf = 0;
	     while (f != NULL)
	       {
		  CountNodes (f, fileDescriptor, level + 1);
		  nbf++;
		  f = f->ElNext;
	       }
	     NbChildren[nbf]++;
	  }

	if (level == 0)
	   /* on a parcourru tout l'arbre, on imprime les resultats */
	  {
	     fprintf (fileDescriptor, "L'arbre ");
	     /* ecrit le nom du type de l'element */
	     if (pEl->ElStructSchema != NULL)
	       {
		  pRe1 = &pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1];
		  fprintf (fileDescriptor, "%s", pRe1->SrOrigName);
		  /* ecrit le nom du schema de structure de l'element */
		  fprintf (fileDescriptor, "(%s) ", pEl->ElStructSchema->SsName);
	       }
	     fprintf (fileDescriptor, "contient %d elements, dont %d feuilles\n\n", NbNodes, NbLeaves);

	     der = MAXNB - 1;
	     while (LeafLevel[der] == 0 && NbElemLevels[der] == 0)
		der--;

	     fprintf (fileDescriptor, "Profondeur\tfeuilles\telements\tnb. moyen de freres des ascend.\n");
	     Prof = 0;
	     for (i = 0; i <= der; i++)
	       {
		  fprintf (fileDescriptor, "%d\t\t%d", i, LeafLevel[i]);
		  Prof += i * LeafLevel[i];
		  fprintf (fileDescriptor, "\t\t%d", NbElemLevels[i]);
		  Moyenne = (float) NbAscendSiblings[i] / (float) NbElemLevels[i];
		  fprintf (fileDescriptor, "\t\t%f\n", Moyenne);
	       }
	     Moyenne = (float) Prof / (float) NbLeaves;
	     fprintf (fileDescriptor, "Profondeur moyenne : %f\n", Moyenne);
	     Moyenne = (float) NbNodes / (float) (der + 1);
	     fprintf (fileDescriptor, "Nombre moyen d'elements par niveau : %f\n\n", Moyenne);

	     der = MAXNB - 1;
	     while (NbChildren[der] == 0)
		der--;
	     fprintf (fileDescriptor, "Nombre de fils   nombre d'elements ayant ce nombre de fils\n");
	     Fils = 0;
	     for (i = 0; i <= der; i++)
	       {
		  fprintf (fileDescriptor, "%d \t\t %d\n", i, NbChildren[i]);
		  Fils += i * NbChildren[i];
	       }
	     Moyenne = (float) Fils / (float) (NbNodes - NbLeaves);
	     fprintf (fileDescriptor, "Nombre moyen de fils par noeud non feuille : %f\n\n", Moyenne);
	  }
     }
}

/*----------------------------------------------------------------------
   wrRef ecrit une reference.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrRef (PtrReference pRef, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrRef (pRef, fileDescriptor)
PtrReference        pRef;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{
   PtrReferredDescr pDe1;
   Name             N;

   switch (pRef->RdTypeRef)
	 {
	    case RefFollow:
	       fprintf (fileDescriptor, "Renvoi");
	       break;
	    case RefInclusion:
	       fprintf (fileDescriptor, "Inclusion");
	       break;
	    default:
	       fprintf (fileDescriptor, "RdTypeRef ????");
	       break;
	 }

   if (pRef->RdInternalRef)
      fprintf (fileDescriptor, " interne ");
   else
      fprintf (fileDescriptor, " externe ");
   if (pRef->RdReferred == NULL)
      fprintf (fileDescriptor, "*RdReferred=NULL*");
   else
     {
	pDe1 = pRef->RdReferred;
	if (pDe1->ReExternalRef)
	  {
	     fprintf (fileDescriptor, "%s(", pDe1->ReReferredLabel);
	     GetDocName (pDe1->ReExtDocument, N);
	     fprintf (fileDescriptor, "%s)", N);
	  }
	else if (pDe1->ReReferredElem == NULL)
	   fprintf (fileDescriptor, "ReReferredElem=NULL, ReReferredLabel=%s", pDe1->ReReferredLabel);
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
#ifdef __STDC__
static void         WrTree (PtrElement pNode, int Indent, FILE * fileDescriptor, ThotBool premierfils)
#else  /* __STDC__ */
static void         WrTree (pNode, Indent, fileDescriptor, premierfils)
PtrElement          pNode;
int                 Indent;
FILE               *fileDescriptor;
ThotBool            premierfils;

#endif /* __STDC__ */
{
   int                 i;
   PtrElement          f;
   PtrAttribute        pAttr;
   PtrPRule            pRule;
   SRule              *pRe1;
   TtAttribute        *pAttr1;

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
	     pRe1 = &pNode->ElStructSchema->SsRule[pNode->ElTypeNumber - 1];
	     fprintf (fileDescriptor, "%s", pRe1->SrOrigName);
	     /* ecrit le nom du schema de structure de l'element */
	     fprintf (fileDescriptor, "(%s %x)", pNode->ElStructSchema->SsName,
		                          (unsigned int)pNode->ElStructSchema);
	  }
	fprintf (fileDescriptor, " Label=%s", pNode->ElLabel);
	/* ecrit le volume de l'element */
	fprintf (fileDescriptor, " Vol=%d", pNode->ElVolume);
	fprintf (fileDescriptor, " Lin=%d", pNode->ElLineNb);
	if (pNode->ElAssocNum != 0)
	   fprintf (fileDescriptor, " Assoc=%d", pNode->ElAssocNum);
	if (pNode->ElIsCopy)
	   fprintf (fileDescriptor, " Copy");
	switch (pNode->ElAccess)
	      {
		 case AccessReadOnly:
		    fprintf (fileDescriptor, " Right=RO");
		    break;
		 case AccessReadWrite:
		    fprintf (fileDescriptor, " Right=R/W");
		    break;
		 case AccessHidden:
		    fprintf (fileDescriptor, " Right=Hidden");
		    break;
		 default:
		    break;
	      }
	if (pNode->ElHolophrast)
	   fprintf (fileDescriptor, " Holophrast");

	/* ecrit les attributs de l'element */
	if (pNode->ElFirstAttr != NULL)
	  {
	     fprintf (fileDescriptor, " (ATTR ");
	     pAttr = pNode->ElFirstAttr;
	     while (pAttr != NULL)
	       {
		  pAttr1 = &pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1];
		  fprintf (fileDescriptor, "%s=", pAttr1->AttrOrigName);
		  switch (pAttr1->AttrType)
			{
			   case AtNumAttr:
			      fprintf (fileDescriptor, "%d", pAttr->AeAttrValue);
			      break;
			   case AtTextAttr:
			      if (pAttr->AeAttrText != NULL)
				{
				   fprintf (fileDescriptor, "%s", pAttr->AeAttrText->BuContent);
				   if (pAttr->AeAttrText->BuNext != NULL)
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
			      fprintf (fileDescriptor, "%s", pAttr1->AttrEnumValue[pAttr->AeAttrValue - 1]);
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
			    SsAttribute[pRule->PrSpecifAttr - 1].AttrOrigName);
		  fprintf (fileDescriptor, " vue%d", pRule->PrViewNum);
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
		       WrText (pNode->ElText, 72 - Indent, fileDescriptor);
		       fprintf (fileDescriptor, "\'\n");
		       break;
		    case LtText:
		       fprintf (fileDescriptor, " Lg=%d Language=%s\n", pNode->ElTextLength,
				TtaGetLanguageName (pNode->ElLanguage));
		       for (i = 1; i <= Indent; i++)
			  fprintf (fileDescriptor, " ");
		       fprintf (fileDescriptor, "\'");
		       WrText (pNode->ElText, 72 - Indent, fileDescriptor);
		       fprintf (fileDescriptor, "\'\n");
		       break;
		    case LtPolyLine:
		       fprintf (fileDescriptor, " Type=%c %d points\n", pNode->ElPolyLineType,
				pNode->ElNPoints);
		       for (i = 1; i <= Indent; i++)
			  fprintf (fileDescriptor, " ");
		       for (i = 0; i < pNode->ElNPoints && i < 8; i++)
			 {
			    fprintf (fileDescriptor, "%d,%d ", pNode->ElPolyLineBuffer->BuPoints[i].XCoord,
			       pNode->ElPolyLineBuffer->BuPoints[i].YCoord);
			 }
		       if (i < pNode->ElNPoints)
			  fprintf (fileDescriptor, "...");
		       fprintf (fileDescriptor, "\n");
		       break;
		    case LtSymbol:
		    case LtGraphics:
		    case LtCompound:
		       fprintf (fileDescriptor, " \'%c\'\n", pNode->ElGraph);
		       break;
		    case LtPageColBreak:
		       fprintf (fileDescriptor, " Number=%d View=%d", pNode->ElPageNumber,
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
#ifdef __STDC__
void                TtaListAbstractTree (Element root, FILE * fileDescriptor)
#else  /* __STDC__ */
void                TtaListAbstractTree (root, fileDescriptor)
Element             root;
FILE               *fileDescriptor;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                TtaListView (Document document, View view, FILE * fileDescriptor)
#else  /* __STDC__ */
void                TtaListView (document, view, fileDescriptor)
Document            document;
View                view;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{
   PtrAbstractBox      PavRacine;
   PtrDocument         pDoc;
   int                 numAssoc;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* vue de l'arbre principal */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		PavRacine = pDoc->DocViewRootAb[view - 1];
		NumberAbsBoxes (PavRacine);
		ListAbsBoxes (PavRacine, 0, fileDescriptor);
	     }
	else
	   /* vue d'elements associes */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
	       {
		  PavRacine = pDoc->DocAssocRoot[numAssoc - 1]->ElAbstractBox[0];
		  NumberAbsBoxes (PavRacine);
		  ListAbsBoxes (PavRacine, 0, fileDescriptor);
	       }
	  }
     }
}



/*----------------------------------------------------------------------
   NumberOneAbsBox numerote recursivement des paves a partir de pAb.   
   cptpave est le compte des paves.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NumberOneAbsBox (PtrAbstractBox pAb, int *cptpave)
#else  /* __STDC__ */
static void         NumberOneAbsBox (pAb, cptpave)
PtrAbstractBox      pAb;
int                *cptpave;

#endif /* __STDC__ */
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
#ifdef __STDC__
void                NumberAbsBoxes (PtrAbstractBox pP)
#else  /* __STDC__ */
void                NumberAbsBoxes (pP)
PtrAbstractBox      pP;

#endif /* __STDC__ */
{
   int                 cptpave;	/* compteur pour numerotation des paves */

   cptpave = 0;
   if (pP != NULL)
      NumberOneAbsBox (pP, &cptpave);
}

/*----------------------------------------------------------------------
   wrThotBool ecrit la valeur d'un booleen.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrThotBool (ThotBool b, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrThotBool (b, fileDescriptor)
ThotBool            b;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{
   if (b)
      fprintf (fileDescriptor, "Y");
   else
      fprintf (fileDescriptor, "N");
}

/*----------------------------------------------------------------------
   wrrepere ecrit la valeur du point de reference.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrrepere (BoxEdge r, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrrepere (r, fileDescriptor)
BoxEdge             r;
FILE               *fileDescriptor;

#endif /* __STDC__ */
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
#ifdef __STDC__
static void         wrTypeUnit (TypeUnit unit, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrTypeUnit (unit, fileDescriptor)
TypeUnit            unit;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{
   switch (unit)
	 {
	    case UnRelative:
	       break;
	    case UnXHeight:
	       fprintf (fileDescriptor, "ex");
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
#ifdef __STDC__
static void         wrpos (AbPosition * pPos, ThotBool racine, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrpos (pPos, racine, fileDescriptor)
AbPosition         *pPos;
ThotBool            racine;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{
   fprintf (fileDescriptor, " ");
   if (pPos->PosAbRef == NULL && !racine)
      fprintf (fileDescriptor, "PosRef = NULL");
   else
     {
	wrrepere (pPos->PosEdge, fileDescriptor);
	if (racine && pPos->PosAbRef == NULL)
	   fprintf (fileDescriptor, " = ThotWindow.");
	else
	   fprintf (fileDescriptor, " = AbstractBox%d.", pPos->PosAbRef->AbNum);
	wrrepere (pPos->PosRefEdge, fileDescriptor);
	if (pPos->PosDistance != 0)
	  {
	     if (pPos->PosDistance < 0)
		fprintf (fileDescriptor, "-");
	     else
		fprintf (fileDescriptor, "+");
	     fprintf (fileDescriptor, "%d", abs (pPos->PosDistance));
	     wrTypeUnit (pPos->PosUnit, fileDescriptor);
	  }
	if (pPos->PosUserSpecified)
	   fprintf (fileDescriptor, " UserSpec");
     }
}

/*----------------------------------------------------------------------
   wrdim ecrit la valeur d'une dimension.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrdim (AbDimension * pDim, ThotBool racine, ThotBool larg, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrdim (pDim, racine, larg, fileDescriptor)
AbDimension        *pDim;
ThotBool            racine;
ThotBool            larg;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{

   fprintf (fileDescriptor, " ");
   if (pDim->DimAbRef == NULL && pDim->DimValue == 0)
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
}

/*----------------------------------------------------------------------
   ListAbsBoxes ecrit dans le fichier fileDescriptor le sous-arbre de paves   
   commencant au pave pointe' par pAb, et avec            
   l'indentation Indent.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ListAbsBoxes (PtrAbstractBox pAb, int Indent, FILE * fileDescriptor)
#else  /* __STDC__ */
void                ListAbsBoxes (pAb, Indent, fileDescriptor)
PtrAbstractBox      pAb;
int                 Indent;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{
   int                 i, j;
   PtrAbstractBox      f;
   ThotBool            root;
   PtrDelayedPRule     pDelPR;
   SRule              *pRe1;
   AbDimension        *pPavDim;
   PtrAttribute        pAt1;
   PictInfo           *image;

   if (pAb != NULL)
     {
	fprintf (fileDescriptor, "\n%d ", pAb->AbNum);	/* numero du pave */

	for (i = 1; i <= Indent; i++)
	   fprintf (fileDescriptor, " ");
	pRe1 = &pAb->AbElement->
	   ElStructSchema->SsRule[pAb->AbElement->ElTypeNumber - 1];
	fprintf (fileDescriptor, "%s", pRe1->SrOrigName);
	fprintf (fileDescriptor, " ");
	if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
	  {
	     fprintf (fileDescriptor, "%d", pAb->AbElement->ElPageType);
	     fprintf (fileDescriptor, " ");
	  }
	if (pAb->AbPresentationBox)
	   fprintf (fileDescriptor, ".%s", pAb->AbPSchema->
		    PsPresentBox[pAb->AbTypeNum - 1].PbName);
	fprintf (fileDescriptor, " TypeNum:%d", pAb->AbTypeNum);
	fprintf (fileDescriptor, " El:%s", pAb->AbElement->ElLabel);
	fprintf (fileDescriptor, " Vol:%d", pAb->AbVolume);

	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "View:%d", pAb->AbDocView);
	fprintf (fileDescriptor, " Visib:%d", pAb->AbVisibility);
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
	   fprintf (fileDescriptor, "(transp.)");
	fprintf (fileDescriptor, ", Right:%d", pAb->AbRightBColor);
	if (pAb->AbRightBColor == -2)
	   fprintf (fileDescriptor, "(transp.)");
	fprintf (fileDescriptor, ", Bottom:%d", pAb->AbBottomBColor);
	if (pAb->AbBottomBColor == -2)
	   fprintf (fileDescriptor, "(transp.)");
	fprintf (fileDescriptor, ", Left:%d", pAb->AbLeftBColor);
	if (pAb->AbLeftBColor == -2)
	   fprintf (fileDescriptor, "(transp.)");

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
	fprintf (fileDescriptor, " Background:%d", pAb->AbBackground);
	fprintf (fileDescriptor, " Foreground:%d", pAb->AbForeground);

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
	fprintf (fileDescriptor, "Font:%c", pAb->AbFont);
	fprintf (fileDescriptor, " Style:%d", pAb->AbFontStyle);
	fprintf (fileDescriptor, " Weight:%d", pAb->AbFontWeight);
	fprintf (fileDescriptor, " Size:%d", pAb->AbSize);
	wrTypeUnit (pAb->AbSizeUnit, fileDescriptor);
	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	if (!pAb->AbHorizEnclosing)
	   fprintf (fileDescriptor, "HorizEncl:N ");
	if (!pAb->AbVertEnclosing)
	   fprintf (fileDescriptor, "VertEncl:N ");
	if (pAb->AbNotInLine)
	   fprintf (fileDescriptor, "NotInLine ");
	fprintf (fileDescriptor, "PageBreak:");
	wrThotBool (pAb->AbAcceptPageBreak, fileDescriptor);
	fprintf (fileDescriptor, " LineBreak:");
	wrThotBool (pAb->AbAcceptLineBreak, fileDescriptor);

	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "Indent:%d", pAb->AbIndent);
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
		 default:
		    fprintf (fileDescriptor, "AbAdjust ????");
		    break;
	      }
	fprintf (fileDescriptor, " Justif:");
	wrThotBool (pAb->AbJustify, fileDescriptor);
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
		    fprintf (fileDescriptor, "COMP");
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
	      image = (PictInfo *) pAb->AbPictBackground;
	      if (image != NULL)
		 {
		 fprintf (fileDescriptor, "Picture: x = %d, y = %d, w = %d, h = %d, name = \"",
			  image->PicXArea, image->PicYArea, image->PicWArea,
			  image->PicHArea);
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
	      image = (PictInfo *) pAb->AbPictInfo;
	      if (image == NULL)
		 fprintf (fileDescriptor, "AbPictInfo = NULL");
	      else
		 fprintf (fileDescriptor, "Picture: x = %d, y = %d, w = %d, h = %d, name =  \"%s \"",
			  image->PicXArea, image->PicYArea, image->PicWArea,
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
		       TtaGetLanguageName (pAb->AbLanguage));
	      fprintf (fileDescriptor, "\n");
	      for (i = 1; i <= Indent + 6; i++)
		 fprintf (fileDescriptor, " ");
	      fprintf (fileDescriptor, " \'");
	      WrText (pAb->AbText, 60, fileDescriptor);
	      fprintf (fileDescriptor, "\'");
	      break;
	   case LtPolyLine:
	      fprintf (fileDescriptor, "type=%c", pAb->AbPolyLineShape);
	      fprintf (fileDescriptor, "\n");
	      for (i = 1; i <= Indent + 6; i++)
		 fprintf (fileDescriptor, " ");
	      for (i = 0; i < pAb->AbVolume && i < 8; i++)
		 fprintf (fileDescriptor, "%d,%d ",
			  pAb->AbPolyLineBuffer->BuPoints[i].XCoord,
			  pAb->AbPolyLineBuffer->BuPoints[i].YCoord);
	      if (i < pAb->AbVolume)
		 fprintf (fileDescriptor, "...");
	      break;
/*CP */    case LtSymbol:
	   case LtGraphics:
	      fprintf (fileDescriptor, " alphabet=%c", pAb->AbGraphAlphabet);
	      fprintf (fileDescriptor, "\'%c\'", pAb->AbShape);
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
			 SsAttribute[pAt1->AeAttrNum - 1].AttrOrigName);
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
	if (pAb->AbLeafType == LtCompound)
	  {
	     fprintf (fileDescriptor, "Line:");
	     wrThotBool (pAb->AbInLine, fileDescriptor);
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
		     if (f->AbEnclosing == NULL)
			fprintf (fileDescriptor,
				 "Next AbstractBox: AbEnclosing=NULL\n");
		     else
			fprintf (fileDescriptor,
			      "Next AbstractBox: bad AbEnclosing\n");
		  ListAbsBoxes (f, Indent + 2, fileDescriptor);
		  f = f->AbNext;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   wrnumber ecrit un entier
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrnumber (int i, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrnumber (i, fileDescriptor)
int                 i;
FILE               *fileDescriptor;
#endif /* __STDC__ */

{
   fprintf (fileDescriptor, "%d", i);
}

/*----------------------------------------------------------------------
   wrtext ecrit du texte
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrtext (STRING Text, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrtext (Text, fileDescriptor)
STRING              Text;
FILE               *fileDescriptor;
#endif /* __STDC__ */

{
   fprintf (fileDescriptor, "%s", Text);
}


/*----------------------------------------------------------------------
   ListBoxTree
   ecrit dans le fichier fileDescriptor les informations sur les boites
   concretes de la boite abstraite pAb et de toutes les boites
   englobees.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ListBoxTree (PtrAbstractBox pAb, int Indent, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         ListBoxTree (pAb, Indent, fileDescriptor)
PtrAbstractBox      pAb;
int                 Indent;
FILE               *fileDescriptor;
#endif /* __STDC__ */

{
   int                 i, j;
   PtrAbstractBox      pAbEnclosed;
   PtrTextBuffer       adbuff;
   PtrPosRelations     pPosRel;
   PtrDimRelations     pDimRel;
   PtrBox              pBox;
   PtrBox              box1;
   ThotBool            loop;
   BoxRelation        *pRe1;
   PictInfo           *image;

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
	     if (pBox->BxType == BoSplit)
		wrnumber (pBox->BxNexChild->BxXOrg, fileDescriptor);
	     else
		wrnumber (pBox->BxXOrg, fileDescriptor);
	     fprintf (fileDescriptor, " Y:");
	     if (pBox->BxType == BoSplit)
		wrnumber (pBox->BxNexChild->BxYOrg, fileDescriptor);
	     else
		wrnumber (pBox->BxYOrg, fileDescriptor);
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
	     if (pBox->BxContentWidth)
		fprintf (fileDescriptor, " Minimum-Width:");
	     else
		fprintf (fileDescriptor, " Content-Width:");
	     wrnumber (pBox->BxRuleWidth, fileDescriptor);

	     fprintf (fileDescriptor, "\n");
	     for (j = 1; j <= Indent + 4; j++)
		fprintf (fileDescriptor, " ");
	     fprintf (fileDescriptor, "Height:");
	     wrnumber (pBox->BxHeight, fileDescriptor);
	     fprintf (fileDescriptor, " Inside_Height:");
	     wrnumber (pBox->BxH, fileDescriptor);
	     if (pBox->BxContentHeight)
		fprintf (fileDescriptor, " Minimum-Height:");
	     else
		fprintf (fileDescriptor, " Content-Height:");
	     wrnumber (pBox->BxRuleHeigth, fileDescriptor);

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
	     fprintf (fileDescriptor, "Status:");
	     if (pBox->BxShadow)
	       fprintf (fileDescriptor, " Shadow:Y");
	     if (pAb->AbOnPageBreak)
	       fprintf (fileDescriptor, " ON PAGE BOUNDARY");
	     if (pAb->AbAfterPageBreak)
	       fprintf (fileDescriptor, " OUT OF PAGE");
	     if (!pAb->AbHorizEnclosing)
		fprintf (fileDescriptor, " Not-Horiz-Enclosed");
	     if (!pAb->AbVertEnclosing)
		fprintf (fileDescriptor, " Not-Vert-Enclosed");
	     /* On liste les relations hors-structure */
	     if (pBox->BxXOutOfStruct)
		fprintf (fileDescriptor, " XRelation-Out-Enclosing");
	     if (pBox->BxYOutOfStruct)
		fprintf (fileDescriptor, " YRelation-Out-Enclosing");
	     if (pBox->BxWOutOfStruct)
		fprintf (fileDescriptor, " HDimRelation-Out-Enclosing");
	     if (pBox->BxHOutOfStruct)
		fprintf (fileDescriptor, " VDimRelation-Out-Enclosing");

	     fprintf (fileDescriptor, "\n");
	     for (j = 1; j <= Indent + 4; j++)
		fprintf (fileDescriptor, " ");
	     fprintf (fileDescriptor, "Nature:");
	     switch (pAb->AbLeafType)
		   {
		      case LtCompound:
			 if (pBox->BxType == BoGhost)
			    fprintf (fileDescriptor, " GHOST");
			 else if (pAb->AbInLine)
			   fprintf (fileDescriptor, " LINES");
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
			 if (pAb->AbInLine || pBox->BxType == BoTable ||
			     pBox->BxType == BoColumn || pBox->BxType == BoRow)
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
			 if (pBox->BxType == BoSplit)
			   {
			      fprintf (fileDescriptor, "SPLIT_TEXT\n");
			      box1 = pBox->BxNexChild;
			      while (box1 != NULL)
				{
				   for (j = 1; j <= Indent + 6; j++)
				      fprintf (fileDescriptor, " ");
				   fprintf (fileDescriptor, "(");
				   fprintf (fileDescriptor, "Start:");
				   wrnumber (box1->BxIndChar, fileDescriptor);
				   fprintf (fileDescriptor, " Characters:");
				   wrnumber (box1->BxNChars, fileDescriptor);
				   fprintf (fileDescriptor, " Spaces:");
				   wrnumber (box1->BxNSpaces, fileDescriptor);
				   fprintf (fileDescriptor, " X:");
				   wrnumber (box1->BxXOrg, fileDescriptor);
				   fprintf (fileDescriptor, " Y:");
				   wrnumber (box1->BxYOrg, fileDescriptor);
				   fprintf (fileDescriptor, " \'");
				   adbuff = box1->BxBuffer;
				   j = box1->BxFirstChar;
				   i = box1->BxNChars;
				   if (i > 10)
				      i = 10;
				   while (i > 0)
				      /* On est en fin de buffer ? */
				     {
					while (adbuff->BuContent[j - 1] == EOS)
					  {
					     adbuff = adbuff->BuNext;
					     j = 1;
					     fprintf (fileDescriptor, "|");
					  }
					putc (adbuff->BuContent[j - 1], fileDescriptor);
					i--;
					j++;
				     }
				   fprintf (fileDescriptor, "\'");
				   fprintf (fileDescriptor, ")\n");
				   box1 = box1->BxNexChild;
				}
			      fprintf (fileDescriptor, "\n");
			   }
			 else
			    fprintf (fileDescriptor, "TEXT\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (fileDescriptor, " ");
			 fprintf (fileDescriptor, "\'");
			 adbuff = pAb->AbText;
			 j = 1;
			 i = pAb->AbVolume;
			 if (i > 60)
			    i = 60;
			 /* On est en fin de buffer ? */
			 while (i > 0)
			   {
			      while (adbuff->BuNext != NULL && adbuff->BuContent[j - 1] == EOS)
				{
				   adbuff = adbuff->BuNext;
				   j = 1;
				   fprintf (fileDescriptor, "|");
				}
			      putc (adbuff->BuContent[j - 1], fileDescriptor);
			      i--;
			      j++;
			   }
			 fprintf (fileDescriptor, "\'");
			 break;
		      case LtPicture:
			 image = (PictInfo *) pAb->AbPictInfo;
			 fprintf (fileDescriptor, "PICTURE\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (fileDescriptor, " ");
			 if (image == NULL)
			    fprintf (fileDescriptor, "PictInfo = NULL");
			 else
			    fprintf (fileDescriptor, "x = %d, y = %d, w = %d, h = %d, name = %s",
				     image->PicXArea, image->PicYArea, image->PicWArea, image->PicHArea,
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
			    wrrepere (pRe1->ReRefEdge, fileDescriptor);
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
			  if (pDimRel->DimRSame[i - 1])
			     fprintf (fileDescriptor, "Width of ");
			  else
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
			  if (pDimRel->DimRSame[i - 1])
			     fprintf (fileDescriptor, "Height of ");
			  else
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
		       ListBoxTree (pAbEnclosed, Indent + 2, fileDescriptor);
		       pAbEnclosed = pAbEnclosed->AbNext;
		    }
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   ListBoxes cree un fichier fname qui contient la liste des     
   relations entre boites dans la fenetre frame.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ListBoxes (int frame, FILE *fileDescriptor)
#else  /* __STDC__ */
void                ListBoxes (frame, fileDescriptor)
int                 frame;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{
   ViewFrame          *pFrame;

   pFrame = &ViewFrameTable[frame - 1];
   if (frame != 0 && pFrame->FrAbstractBox != NULL)
      /* Une petite cuisine pour passer le nom du fichier en parametre */
     {
       /* On numerote les paves */
       NumberAbsBoxes (pFrame->FrAbstractBox);
       /* On ecrit les informations sur les boites crees */
       ListBoxTree (pFrame->FrAbstractBox, 0, fileDescriptor);
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
#ifdef __STDC__
void                TtaListBoxes (Document document, View view, FILE *fileDescriptor)
#else  /* __STDC__ */
void                TtaListBoxes (document, view, fileDescriptor)
Document            document;
View                view;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 numAssoc;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 100)
	   /* vue de l'arbre principal */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     ListBoxes (pDoc->DocViewFrame[view - 1], fileDescriptor);
	else
	   /* vue d'elements associes */
	  {
	     numAssoc = view - 100;
	     if (numAssoc < 1 || numAssoc > MAX_ASSOC_DOC)
		TtaError (ERR_invalid_parameter);
	     else
	       ListBoxes (pDoc->DocAssocFrame[numAssoc - 1], fileDescriptor);
	  }
     }
}


/*----------------------------------------------------------------------
   wrlevel ecrit au terminal le niveau relatif n.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrlevel (Level n, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrlevel (n, fileDescriptor)
Level               n;
FILE               *fileDescriptor;
#endif /* __STDC__ */
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
	 }
}

/*----------------------------------------------------------------------
   wrdistunit ecrit le nom d'une unite' de distance.               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrdistunit (TypeUnit u, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrdistunit (u, fileDescriptor)
FILE               *fileDescriptor;
TypeUnit            u;
#endif /* __STDC__ */
{
   switch (u)
	 {
	    case UnRelative:
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
	 }
}


/*----------------------------------------------------------------------
   wrnomregle ecrit au terminal le nom de la regle de numero r.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrnomregle (int r, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrnomregle (r, fileDescriptor)
int                 r;
FILE               *fileDescriptor;
#endif /* __STDC__ */
{
	if (r > 0) {
#     ifdef _I18N_
      char mbStr [MAX_TXT_LEN];
      wcstombs (mbStr, pSchemaStr->SsRule[r - 1].SrName, MAX_TXT_LEN);
      fprintf (fileDescriptor, mbStr);
#     else  /* !_I18N_ */
      fprintf (fileDescriptor, pSchemaStr->SsRule[r - 1].SrName);
#     endif /* !_I18N_ */
	} 
}


/*----------------------------------------------------------------------
   wrnomattr ecrit au terminal le nom de l'attribut de numero a.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrnomattr (int a, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrnomattr (a, fileDescriptor)
int                 a;
FILE               *fileDescriptor;
#endif /* __STDC__ */
{
	if (a != 0) {
#     ifdef _I18N_
      char mbStr [MAX_TXT_LEN];
      wcstombs (mbStr, pSchemaStr->SsAttribute[abs (a) - 1].AttrName, MAX_TXT_LEN);
      fprintf (fileDescriptor, mbStr);
#     else  /* !_I18N_ */
      fprintf (fileDescriptor, pSchemaStr->SsAttribute[abs (a) - 1].AttrName);
#     endif /* !_I18N_ */
	} 
}


/*----------------------------------------------------------------------
   wrnomboite ecrit au terminal le nom de la boite de presentation 
   de numero b.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrnomboite (int b, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrnomboite (b, fileDescriptor)
int                 b;
FILE               *fileDescriptor;
#endif /* __STDC__ */
{
	if (b > 0) {
#     ifdef _I18N_
      char mbStr [MAX_TXT_LEN];
      wcstombs (mbStr, pSc1->PsPresentBox[b - 1].PbName, MAX_TXT_LEN);
      fprintf (fileDescriptor, mbStr);
#     else  /* !_I18N_ */
      fprintf (fileDescriptor, pSc1->PsPresentBox[b - 1].PbName);
#     endif /* !_I18N_ */
	} 
}


/*----------------------------------------------------------------------
   wrModeHerit ecrit au terminal un mode d'heritage.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrModeHerit (InheritMode M, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrModeHerit (M, fileDescriptor)
InheritMode         M;
FILE               *fileDescriptor;
#endif /* __STDC__ */
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
#ifdef __STDC__
static void         wrsize (PtrPRule pR, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrsize (pR, fileDescriptor)
PtrPRule            pR;
FILE               *fileDescriptor;
#endif /* __STDC__ */
{
   if (pR->PrPresMode == PresInherit)
     {
	wrModeHerit (pR->PrInheritMode, fileDescriptor);
	if (pR->PrInhPercent)
	  {
	  fprintf (fileDescriptor, " * ");
	  if (pR->PrInhAttr)
	     wrnomattr (pR->PrInhDelta, fileDescriptor);
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
		 wrnomattr (pR->PrInhDelta, fileDescriptor);
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
		wrnomattr (pR->PrInhMinOrMax, fileDescriptor);
	     else
		wrnumber (pR->PrInhMinOrMax, fileDescriptor);
	  }
     }
   else if (pR->PrPresMode == PresImmediate)
     {
	if (pR->PrMinAttr)
	   wrnomattr (pR->PrMinValue, fileDescriptor);
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
#ifdef __STDC__
static void         wrfontstyle (PtrPRule pR, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrfontstyle (pR, fileDescriptor)
PtrPRule            pR;
FILE               *fileDescriptor;
#endif /* __STDC__ */
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
#ifdef __STDC__
static void         wrnbherit (PtrPRule pR, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrnbherit (pR, fileDescriptor)
PtrPRule            pR;
FILE               *fileDescriptor;
#endif /* __STDC__ */
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
		wrnomattr (pR->PrInhDelta, fileDescriptor);
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
		wrnomattr (pR->PrInhMinOrMax, fileDescriptor);
	     else
		wrnumber (pR->PrInhMinOrMax, fileDescriptor);
	  }
        }
   else if (pR->PrPresMode == PresImmediate)
      if (pR->PrAttrValue)
	 wrnomattr (pR->PrIntValue, fileDescriptor);
      else
	 wrnumber (pR->PrIntValue, fileDescriptor);
   else
      fprintf (fileDescriptor, "??????");
   fprintf (fileDescriptor, ";");
}


/*----------------------------------------------------------------------
   wrminind ecrit au terminal la regle 'NoBreak1, NoBreak2 ou      
   Indent pointee par pR.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrminind (PtrPRule pR, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrminind (pR, fileDescriptor)
PtrPRule            pR;
FILE               *fileDescriptor;
#endif /* __STDC__ */

{
   if (pR->PrPresMode == PresInherit)
      wrnbherit (pR, fileDescriptor);
   else
     {
	if (pR->PrPresMode == PresImmediate)
	  {
	     if (pR->PrMinUnit == UnAuto)
	        printf ("auto");
	     else
	       {
	       if (pR->PrMinAttr)
		  wrnomattr (pR->PrMinValue, fileDescriptor);
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
#ifdef __STDC__
static void         WrPos (PosRule pos, ThotBool Def, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         WrPos (pos, Def, fileDescriptor)
PosRule             pos;
ThotBool             Def;
FILE               *fileDescriptor;
#endif /* __STDC__ */
{
   if (Def)
      if (pos.PoPosDef == NoEdge)
	 fprintf (fileDescriptor, " NULL");
      else
	{
	   wrrepere (pos.PoPosDef, fileDescriptor);
	   fprintf (fileDescriptor, " = ");
	}
   if (!Def || pos.PoPosDef != NoEdge)
     {
	wrlevel (pos.PoRelation, fileDescriptor);
	if (pos.PoNotRel)
	   fprintf (fileDescriptor, " NOT");
	fprintf (fileDescriptor, " ");
	if (pos.PoRefKind == RkElType)
	   wrnomregle (pos.PoRefIdent, fileDescriptor);
	else if (pos.PoRefKind == RkPresBox)
	   wrnomboite (pos.PoRefIdent, fileDescriptor);
	else if (pos.PoRefKind == RkAttr)
	   wrnomattr (pos.PoRefIdent, fileDescriptor);
	fprintf (fileDescriptor, ". ");
	wrrepere (pos.PoPosRef, fileDescriptor);
	if (pos.PoDistance != 0)
	  {
	     if (pos.PoDistance > 0)
		fprintf (fileDescriptor, "+");
	     else
		fprintf (fileDescriptor, "-");
	     if (pos.PoDistAttr)
		wrnomattr (abs (pos.PoDistance), fileDescriptor);
	     else
		wrnumber (abs (pos.PoDistance), fileDescriptor);
	     wrdistunit (pos.PoDistUnit, fileDescriptor);
	  }
	if (pos.PoUserSpecified)
	   fprintf (fileDescriptor, " UserSpecified");
     }
   fprintf (fileDescriptor, ";");
}


/*----------------------------------------------------------------------
   wrdimens ecrit au terminal la dimension Dim.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrdimens (DimensionRule Dim, ThotBool Hauteur, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrdimens (Dim, Hauteur, fileDescriptor)
DimensionRule       Dim;
ThotBool             Hauteur;
FILE               *fileDescriptor;
#endif /* __STDC__ */
{
   if (Dim.DrPosition)
      WrPos (Dim.DrPosRule, True, fileDescriptor);
   else
     {
	if (Dim.DrAbsolute)
	  {
	     if (Dim.DrAttr)
		wrnomattr (Dim.DrValue, fileDescriptor);
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
		wrnomregle (Dim.DrRefIdent, fileDescriptor);
	     else if (Dim.DrRefKind == RkPresBox)
		wrnomboite (Dim.DrRefIdent, fileDescriptor);
	     else if (Dim.DrRefKind == RkAttr)
		wrnomattr (Dim.DrRefIdent, fileDescriptor);
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
		     wrnomattr (abs (Dim.DrValue), fileDescriptor);
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
			  wrnomattr (abs (Dim.DrValue), fileDescriptor);
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
#ifdef __STDC__
static void         wrCondition (PtrCondition pCond, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrCondition (pCond, fileDescriptor)
PtrCondition        pCond;
FILE               *fileDescriptor;
#endif /* __STDC__ */
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
	       fprintf (fileDescriptor, "PcCopyRef ");
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
	       fprintf (fileDescriptor, "PcEmpty ");
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
#           ifdef _I18N_
            wcstombs (mbsTmpStr, pCond->CoAncestorName, MAX_TXT_LEN);
		    fprintf (fileDescriptor, mbsTmpStr);
		    fprintf (fileDescriptor, "(");
            wcstombs (mbsTmpStr, pCond->CoSSchemaName, MAX_TXT_LEN);
		    fprintf (fileDescriptor, mbsTmpStr);
		    fprintf (fileDescriptor, ")");
#           else  /* !_I18N_ */
		    fprintf (fileDescriptor, pCond->CoAncestorName);
		    fprintf (fileDescriptor, "(");
		    fprintf (fileDescriptor, pCond->CoSSchemaName);
		    fprintf (fileDescriptor, ")");
#           endif /* !_I18N_ */
		 }
	       else
		  wrnomregle (pCond->CoTypeAncestor, fileDescriptor);
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
	       wrnomregle (pCond->CoTypeElAttr, fileDescriptor);
	       fprintf (fileDescriptor, " ");
	       break;
	    case PcAttribute:
	       wrnomattr (pCond->CoTypeElAttr, fileDescriptor);
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
   wrFonctPres ecrit au terminal la fonction de presentation       
   contenue dans la regle pointee par pR.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrFonctPres (PtrPRule pR, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrFonctPres (pR, fileDescriptor)
PtrPRule            pR;
FILE               *fileDescriptor;
#endif /* __STDC__ */
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
	       fprintf (fileDescriptor, "BackgroundPicture: Cste");
	       wrnumber (pR->PrPresBox[0], fileDescriptor);
	       break;
	    case FnPictureMode:
	       fprintf (fileDescriptor, "PictureMode: ");
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
            pR->PrPresFunction != FnPictureMode &&
	    pR->PrPresFunction != FnNoLine)
     {
	fprintf (fileDescriptor, "(");
	if (pR->PrNPresBoxes == 0)
	  {
#        ifdef _I18N_
         wcstombs (mbsTmpStr, pR->PrPresBoxName, MAX_TXT_LEN);
	     fprintf (fileDescriptor, mbsTmpStr);
#        else  /* !_I18N_ */
	     fprintf (fileDescriptor, pR->PrPresBoxName);
#        endif /* !_I18N_ */
	     if (pR->PrExternal || !pR->PrElement)
		fprintf (fileDescriptor, "(****)");
	  }
	else
	   for (i = 1; i <= pR->PrNPresBoxes; i++)
	     {
		if (i > 1)
		   fprintf (fileDescriptor, ", ");
		if (pR->PrElement)
		   wrnomregle (pR->PrPresBox[i - 1], fileDescriptor);
		else
		   wrnomboite (pR->PrPresBox[i - 1], fileDescriptor);
	     }
	fprintf (fileDescriptor, ")");
     }
   fprintf (fileDescriptor, ";");
}


/*----------------------------------------------------------------------
   wrajust ecrit au terminal la regle d'ajustement des lignes.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrajust (PtrPRule pR, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrajust (pR, fileDescriptor)
PtrPRule            pR;
FILE               *fileDescriptor;
#endif /* __STDC__ */
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
	    }
}


/*----------------------------------------------------------------------
   wrjustif ecrit la regle de justification ou hyphenation pointee 
   par pR.                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrjustif (PtrPRule pR, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrjustif (pR, fileDescriptor)
PtrPRule            pR;
FILE               *fileDescriptor;
#endif /* __STDC__ */
{
   if (pR->PrPresMode == PresInherit)
      wrnbherit (pR, fileDescriptor);
   if (pR->PrPresMode == PresImmediate)
      if (pR->PrJustify)
	 fprintf (fileDescriptor, "Yes;");
      else
	 fprintf (fileDescriptor, "No;");
}


/*----------------------------------------------------------------------
   wrsuiteregles ecrit au terminal la suite de regles chainees dont 
   RP pointe sur la regle de tete.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrsuiteregles (PtrPRule RP, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrsuiteregles (RP, fileDescriptor)
PtrPRule            RP;
FILE               *fileDescriptor;
#endif /* __STDC__ */
{
   PtrCondition        pCond;

   while (RP != NULL)
      /* ecrit une regle de presentation */
     {
	fprintf (fileDescriptor, "   ");
	if (RP->PrViewNum > 1)
	  {
	     fprintf (fileDescriptor, "IN ");
#        ifdef _I18N_
         wcstombs (mbsTmpStr,  pSc1->PsView[RP->PrViewNum - 1], MAX_TXT_LEN);
	     fprintf (fileDescriptor, mbsTmpStr);
#        else  /* !_I18N_ */
	     fprintf (fileDescriptor, pSc1->PsView[RP->PrViewNum - 1]);
#        endif /* !_I18N_ */
	     fprintf (fileDescriptor, " ");
	  }
	if (RP->PrCond != NULL)
	  {
	     pCond = RP->PrCond;
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
		  fprintf (fileDescriptor, "AND ");
		  wrCondition (pCond, fileDescriptor);
		  pCond = pCond->CoNextCondition;
	       }
	  }
	switch (RP->PrType)
	      {
		 case PtVisibility:
		    fprintf (fileDescriptor, "Visibility: ");
		    wrnbherit (RP, fileDescriptor);
		    break;
		 case PtFunction:
		    wrFonctPres (RP, fileDescriptor);
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
			!RP->PrAttrValue && RP->PrIntValue == -2)
		       fprintf (fileDescriptor, "transparent");
		    else
		       wrnbherit (RP, fileDescriptor);
                    break;
                 case PtBorderRightColor:
                    fprintf (fileDescriptor, "BorderRightColor: ");
	            if (RP->PrPresMode == PresImmediate &&
			!RP->PrAttrValue && RP->PrIntValue == -2)
		       fprintf (fileDescriptor, "transparent");
		    else
		       wrnbherit (RP, fileDescriptor);
                    break;
                 case PtBorderBottomColor:
                    fprintf (fileDescriptor, "BorderBottomColor: ");
	            if (RP->PrPresMode == PresImmediate &&
			!RP->PrAttrValue && RP->PrIntValue == -2)
		       fprintf (fileDescriptor, "transparent");
		    else
		       wrnbherit (RP, fileDescriptor);
                    break;
                 case PtBorderLeftColor:
                    fprintf (fileDescriptor, "BorderLeftColor: ");
	            if (RP->PrPresMode == PresImmediate &&
			!RP->PrAttrValue && RP->PrIntValue == -2)
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
         	 case PtFont:
 		    fprintf (fileDescriptor, "Font: ");
     		    wrfontstyle (RP, fileDescriptor);
     		    break;
		 case PtStyle:
		    fprintf (fileDescriptor, "Style: ");
		    wrfontstyle (RP, fileDescriptor);
		    break;
		 case PtWeight:
		    fprintf (fileDescriptor, "Weight: ");
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
		 case PtSize:
		    fprintf (fileDescriptor, "Size: ");
		    wrsize (RP, fileDescriptor);
		    break;
		 case PtIndent:
		    fprintf (fileDescriptor, "Indent: ");
		    wrminind (RP, fileDescriptor);
		    break;
		 case PtBreak1:
		    fprintf (fileDescriptor, "NoBreak1: ");
		    wrminind (RP, fileDescriptor);
		    break;
		 case PtBreak2:
		    fprintf (fileDescriptor, "NoBreak2: ");
		    wrminind (RP, fileDescriptor);
		    break;
		 case PtLineSpacing:
		    fprintf (fileDescriptor, "LineSpacing: ");
		    wrminind (RP, fileDescriptor);
		    break;
		 case PtAdjust:
		    fprintf (fileDescriptor, "Adjust: ");
		    wrajust (RP, fileDescriptor);
		    break;
		 case PtJustify:
		    fprintf (fileDescriptor, "Justify: ");
		    wrjustif (RP, fileDescriptor);
		    break;
		 case PtHyphenate:
		    fprintf (fileDescriptor, "Hyphenate: ");
		    wrjustif (RP, fileDescriptor);
		    break;
		 case PtVertOverflow:
		    fprintf (fileDescriptor, "VertOverflow: ");
		    wrjustif (RP, fileDescriptor);
		    break;
		 case PtHorizOverflow:
		    fprintf (fileDescriptor, "HorizOverflow: ");
		    wrjustif (RP, fileDescriptor);
		    break;
		 case PtDepth:
		    fprintf (fileDescriptor, "Depth: ");
		    wrnbherit (RP, fileDescriptor);
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
		 case PtBackground:
		    fprintf (fileDescriptor, "Background: ");
		    wrnbherit (RP, fileDescriptor);
		    break;
		 case PtForeground:
		    fprintf (fileDescriptor, "Foreground: ");
		    wrnbherit (RP, fileDescriptor);
		    break;
		 case PtPictInfo:
		    break;
	      }
	fprintf (fileDescriptor, "\n");		/* passe a la regle suivante */
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
#ifdef __STDC__
void                TtaListStyleSchemas (Document document, FILE * fileDescriptor)
#else  /* __STDC__ */
void                TtaListStyleSchemas (document, fileDescriptor)
Document            document;
FILE               *fileDescriptor;
#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   PtrHandlePSchema    pHd;
   PresConstant       *pPr1;
   TtAttribute        *pAt1;
   AttributePres      *pRP1;
   NumAttrCase        *pCa1;
   int                 i, j;
   int                 El, Attr, Val;
   
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = LoadedDocument[document - 1];
	pSchemaStr = pDoc->DocSSchema;
	pHd = pSchemaStr->SsFirstPSchemaExtens;
	while (pHd != NULL)
	  {
	     pSc1 = pHd->HdPSchema;
	     if (pSc1 != NULL)
	       {
		 fprintf (fileDescriptor, "{---------------------------------------------------------------}\n\n");
		 fprintf (fileDescriptor, "PRESENTATION ");
		 wrtext (pSchemaStr->SsName, fileDescriptor);
		 fprintf (fileDescriptor, ";\n");

		 /* les constantes */
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
			     fprintf (fileDescriptor, "%c", pPr1->PdAlphabet);
			     break;
			   case GraphicElem:
			     fprintf (fileDescriptor, "Graphics ");
			     break;
			   case Symbol:
			     fprintf (fileDescriptor, "Symbol ");
			     break;
			   case Picture:
			     fprintf (fileDescriptor, "Picture ");
			     /* ecrit la valeur de la constante */
			     break;
			   default:
			     break;
			   }
			 
			 fprintf (fileDescriptor, " \'");
			 j = 0;
			 while (pPr1->PdString[j] != '\0')
			   {
			     if (pPr1->PdString[j] < ' ')
			       fprintf (fileDescriptor, "\\%3d", (int) pPr1->PdString[j]);
			     else
			       fprintf (fileDescriptor, "%c", pPr1->PdString[j]);
			     j++;
			   }
			 fprintf (fileDescriptor, "\';\n");
		       }
		   }

		 /* les regles de presentation des elements structure's */
		 fprintf (fileDescriptor, "\nRULES\n\n");
		 for (El = 1; El <= pSchemaStr->SsNRules; El++)
		   {
		     if (pSc1->PsElemPRule[El - 1])
		       {
			 if (pSchemaStr->SsRule[El - 1].SrConstruct == CsPairedElement)
			   if (pSchemaStr->SsRule[El - 1].SrFirstOfPair)
			     fprintf (fileDescriptor, "First ");
			   else
			     fprintf (fileDescriptor, "Second ");
			 wrnomregle (El, fileDescriptor);
			 fprintf (fileDescriptor, ":\n");
			 fprintf (fileDescriptor, "   BEGIN\n");
			 wrsuiteregles (pSc1->PsElemPRule[El - 1], fileDescriptor);
			 if (pSc1->PsInPageHeaderOrFooter[El - 1])
			   fprintf (fileDescriptor, "   { displayed in top or bottom of page }\n");
			 if (pSc1->PsAssocPaginated[El - 1])
			   fprintf (fileDescriptor, "   { with pages }\n");
			 fprintf (fileDescriptor, "   END;\n");
			 fprintf (fileDescriptor, "\n");
		       }
		   }

		 /* les regles de presentation des attributs */
		 if (pSchemaStr->SsNAttributes > 0)
		   {
		     fprintf (fileDescriptor, "\n");
		     fprintf (fileDescriptor, "ATTRIBUTES\n");
		     fprintf (fileDescriptor, "\n");
		     for (Attr = 1; Attr <= pSchemaStr->SsNAttributes; Attr++)
		       {
			 pAt1 = &pSchemaStr->SsAttribute[Attr - 1];
			 pRP1 = pSc1->PsAttrPRule[Attr - 1];
			 while (pRP1 != NULL)
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
					 wrnomregle (pRP1->ApElemType, fileDescriptor);
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
				     fprintf (fileDescriptor, ":\n");
				     if (pCa1->CaFirstPRule == NULL)
				       fprintf (fileDescriptor, "   BEGIN END;\n");
				     else
				       {
					 if (pCa1->CaFirstPRule->PrNextPRule != NULL)
					   fprintf (fileDescriptor, "   BEGIN\n");
					 wrsuiteregles (pCa1->CaFirstPRule, fileDescriptor);
					 if (pCa1->CaFirstPRule->PrNextPRule != NULL)
					   fprintf (fileDescriptor, "   END;\n");
				       }
				     fprintf (fileDescriptor, "\n");
				   }
				 break;
			       case AtTextAttr:
				 if (pRP1->ApTextFirstPRule != NULL)
				   {
				     wrtext (pAt1->AttrName, fileDescriptor);
				     if (pRP1->ApElemType > 0)
				       {
					 fprintf (fileDescriptor, "(");
					 wrnomregle (pRP1->ApElemType, fileDescriptor);
					 fprintf (fileDescriptor, ")");
				       }
				     if (pRP1->ApString[0] != '\0')
				       {
					 fprintf (fileDescriptor, "=\'");
					 wrtext (pRP1->ApString, fileDescriptor);
					 fprintf (fileDescriptor, "\'");
				       }
				     fprintf (fileDescriptor, ":\n");
				     if (pRP1->ApTextFirstPRule->PrNextPRule != NULL)
				       fprintf (fileDescriptor, "   BEGIN\n");
				     wrsuiteregles (pRP1->ApTextFirstPRule, fileDescriptor);
				     if (pRP1->ApTextFirstPRule->PrNextPRule != NULL)
				       fprintf (fileDescriptor, "   END;\n");
				     fprintf (fileDescriptor, "\n");
				   }
				 break;
			       case AtReferenceAttr:
				 if (pRP1->ApRefFirstPRule != NULL)
				   {
				     wrtext (pAt1->AttrName, fileDescriptor);
				     if (pRP1->ApElemType > 0)
				       {
					 fprintf (fileDescriptor, "(");
					 wrnomregle (pRP1->ApElemType, fileDescriptor);
					 fprintf (fileDescriptor, ")");
				       }
				     fprintf (fileDescriptor, ":\n");
				     if (pRP1->ApRefFirstPRule->PrNextPRule != NULL)
				       fprintf (fileDescriptor, "   BEGIN\n");
				     wrsuiteregles (pRP1->ApRefFirstPRule, fileDescriptor);
				     if (pRP1->ApRefFirstPRule->PrNextPRule != NULL)
				       fprintf (fileDescriptor, "   END;\n");
				     fprintf (fileDescriptor, "\n");
				   }
				 break;
			       case AtEnumAttr:
				 for (Val = 0; Val <= pAt1->AttrNEnumValues; Val++)
				   if (pRP1->ApEnumFirstPRule[Val] != NULL)
				     {
				       wrtext (pAt1->AttrName, fileDescriptor);
				       if (pRP1->ApElemType > 0)
					 {
					   fprintf (fileDescriptor, "(");
					   wrnomregle (pRP1->ApElemType, fileDescriptor);
					   fprintf (fileDescriptor, ")");
					 }
				       if (Val > 0)
					 {
					   fprintf (fileDescriptor, "=");
					   wrtext (pAt1->AttrEnumValue[Val - 1], fileDescriptor);
					 }
				       fprintf (fileDescriptor, ":\n");
				       if (pRP1->ApEnumFirstPRule[Val]->PrNextPRule
					   != NULL)
					 fprintf (fileDescriptor, "   BEGIN\n");
				       wrsuiteregles (pRP1->ApEnumFirstPRule[Val], fileDescriptor);
				       if (pRP1->ApEnumFirstPRule[Val]->PrNextPRule
					   != NULL)
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
     }
}
