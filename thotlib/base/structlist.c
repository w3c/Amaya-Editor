/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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

extern int          UserErrorCode;
#define MAXNB 200
static int          NbChildren[MAXNB];
static int          LeafLevel[MAXNB];
static int          NbElemLevels[MAXNB];
static int          NbAscendSiblings[MAXNB];
static int          NbNodes;
static int          NbLeaves;

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
	       fprintf (fileDescriptor, "Visib");
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
		  fprintf (fileDescriptor, "%s", pRe1->SrName);
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
   PtrReferredDescr    pDe1;
   Name                N;

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
static void         WrTree (PtrElement pNode, int Indent, FILE * fileDescriptor, boolean premierfils)
#else  /* __STDC__ */
static void         WrTree (pNode, Indent, fileDescriptor, premierfils)
PtrElement          pNode;
int                 Indent;
FILE               *fileDescriptor;
boolean             premierfils;

#endif /* __STDC__ */
{
   int                 i;
   PtrElement          f;
   PtrAttribute        pAttr;
   PtrPRule            pRule;
   SRule              *pRe1;
   PtrAttribute        pAt1;
   TtAttribute        *pAttr1;
   PtrPRule            pRegl1;

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
	     fprintf (fileDescriptor, "%s", pRe1->SrName);
	     /* ecrit le nom du schema de structure de l'element */
	     fprintf (fileDescriptor, "(%s)", pNode->ElStructSchema->SsName);
	  }
	fprintf (fileDescriptor, " Label=%s", pNode->ElLabel);
	/* ecrit le volume de l'element */
	fprintf (fileDescriptor, " Vol=%d", pNode->ElVolume);
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
		  pAt1 = pAttr;
		  pAttr1 = &pAt1->AeAttrSSchema->SsAttribute[pAt1->AeAttrNum - 1];
		  fprintf (fileDescriptor, "%s=", pAttr1->AttrName);
		  switch (pAttr1->AttrType)
			{
			   case AtNumAttr:
			      fprintf (fileDescriptor, "%d", pAt1->AeAttrValue);
			      break;
			   case AtTextAttr:
			      if (pAt1->AeAttrText != NULL)
				{
				   fprintf (fileDescriptor, "%s", pAt1->AeAttrText->BuContent);
				   if (pAt1->AeAttrText->BuNext != NULL)
				      fprintf (fileDescriptor, "...");
				}
			      break;
			   case AtReferenceAttr:
			      if (pAt1->AeAttrReference == NULL)
				 fprintf (fileDescriptor, "*AeAttrReference=NULL*");
			      else if (pAt1->AeAttrReference->RdReferred == NULL)
				 fprintf (fileDescriptor, "*RdReferred=NULL*");
			      else
				 wrRef (pAt1->AeAttrReference, fileDescriptor);
			      break;
			   case AtEnumAttr:
			      fprintf (fileDescriptor, "%s", pAttr1->AttrEnumValue[pAt1->AeAttrValue - 1]);
			      break;
			   default:
			      fprintf (fileDescriptor, "AttrType ????");
			      break;
			}

		  if (pAt1->AeNext != NULL)
		     fprintf (fileDescriptor, ", ");
		  pAttr = pAt1->AeNext;
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
		  pRegl1 = pRule;
		  if (pRule != pNode->ElFirstPRule)
		     fprintf (fileDescriptor, ", ");
		  WrPRuleType (pRule, fileDescriptor);
		  if (pRegl1->PrSpecifAttr > 0)
		     fprintf (fileDescriptor, "[%s]", pRegl1->PrSpecifAttrSSchema->
			    SsAttribute[pRegl1->PrSpecifAttr - 1].AttrName);
		  fprintf (fileDescriptor, " vue%d", pRegl1->PrViewNum);
		  pRule = pRegl1->PrNextPRule;
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
   wrboolean ecrit la valeur d'un booleen.                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrboolean (boolean b, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrboolean (b, fileDescriptor)
boolean             b;
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
	       fprintf (fileDescriptor, "????");
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
	    default:
	       fprintf (fileDescriptor, "???");
	       break;
	 }
}

/*----------------------------------------------------------------------
   wrpos ecrit la valeur d'une position.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         wrpos (AbPosition * pPos, boolean racine, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrpos (pPos, racine, fileDescriptor)
AbPosition         *pPos;
boolean             racine;
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
static void         wrdim (AbDimension * pDim, boolean racine, boolean larg, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrdim (pDim, racine, larg, fileDescriptor)
AbDimension        *pDim;
boolean             racine;
boolean             larg;
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
   boolean             root;
   PtrDelayedPRule     pDelPR;
   PtrAbstractBox      pPa1;
   SRule              *pRe1;
   AbDimension        *pPavDim;
   PtrAttribute        pAt1;
   PictInfo           *image;

   if (pAb != NULL)
     {
	pPa1 = pAb;
	fprintf (fileDescriptor, "\n%d ", pPa1->AbNum);	/* numero du pave */

	for (i = 1; i <= Indent; i++)
	   fprintf (fileDescriptor, " ");
	pRe1 = &pPa1->AbElement->
	   ElStructSchema->SsRule[pPa1->AbElement->ElTypeNumber - 1];
	fprintf (fileDescriptor, "%s", pRe1->SrName);
	fprintf (fileDescriptor, " ");
	if (pPa1->AbElement->ElTypeNumber == PageBreak + 1)
	  {
	     fprintf (fileDescriptor, "%d", pPa1->AbElement->ElPageType);
	     fprintf (fileDescriptor, " ");
	  }
	if (pPa1->AbPresentationBox)
	   fprintf (fileDescriptor, ".%s", pPa1->AbPSchema->
		    PsPresentBox[pPa1->AbTypeNum - 1].PbName);
	fprintf (fileDescriptor, " TypeNum:%d", pPa1->AbTypeNum);
	fprintf (fileDescriptor, " El:%s", pPa1->AbElement->ElLabel);
	fprintf (fileDescriptor, " Vol:%d", pPa1->AbVolume);

	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "View:%d", pPa1->AbDocView);
	fprintf (fileDescriptor, " Visib:%d", pPa1->AbVisibility);
	fprintf (fileDescriptor, " Actif:");
	wrboolean (pPa1->AbSensitive, fileDescriptor);
	fprintf (fileDescriptor, " R/O:");
	wrboolean (pPa1->AbReadOnly, fileDescriptor);
	fprintf (fileDescriptor, " Modif:");
	wrboolean (pPa1->AbCanBeModified, fileDescriptor);
	fprintf (fileDescriptor, " PresBox:");
	wrboolean (pPa1->AbPresentationBox, fileDescriptor);

	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "Pattern:%d", pPa1->AbFillPattern);
	fprintf (fileDescriptor, " Background:%d", pPa1->AbBackground);
	fprintf (fileDescriptor, " Foreground:%d", pPa1->AbForeground);
	fprintf (fileDescriptor, " LineStyle:%c", pPa1->AbLineStyle);
	fprintf (fileDescriptor, " LineWeight:%d", pPa1->AbLineWeight);
	wrTypeUnit (pPa1->AbLineWeightUnit, fileDescriptor);
	fprintf (fileDescriptor, " Depth:%d", pPa1->AbDepth);

	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "Font:%c", pPa1->AbFont);
	fprintf (fileDescriptor, " HighL:%d", pPa1->AbHighlight);
	fprintf (fileDescriptor, " Size:%d", pPa1->AbSize);
	wrTypeUnit (pPa1->AbSizeUnit, fileDescriptor);
	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	if (!pPa1->AbHorizEnclosing)
	   fprintf (fileDescriptor, " HorizEncl:N");
	if (!pPa1->AbVertEnclosing)
	   fprintf (fileDescriptor, " VertEncl:N");
	if (pPa1->AbNotInLine)
	   fprintf (fileDescriptor, " NotInLine");
	fprintf (fileDescriptor, " PageBreak:");
	wrboolean (pPa1->AbAcceptPageBreak, fileDescriptor);
	fprintf (fileDescriptor, " LineBreak:");
	wrboolean (pPa1->AbAcceptLineBreak, fileDescriptor);

	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, " Indent:%d", pPa1->AbIndent);
	wrTypeUnit (pPa1->AbIndentUnit, fileDescriptor);
	fprintf (fileDescriptor, " align:");
	switch (pPa1->AbAdjust)
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
	wrboolean (pPa1->AbJustify, fileDescriptor);
	fprintf (fileDescriptor, " Hyphen:");
	wrboolean (pPa1->AbHyphenate, fileDescriptor);

	fprintf (fileDescriptor, " Linespace:%d", pPa1->AbLineSpacing);
	wrTypeUnit (pPa1->AbLineSpacingUnit, fileDescriptor);

	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "Nature:");
	switch (pPa1->AbLeafType)
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

	fprintf (fileDescriptor, "\n\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	switch (pPa1->AbLeafType)
	      {
		 case LtCompound:
		    image = (PictInfo *) pPa1->AbPictBackground;
		    if (image != NULL)
		      {
			fprintf (fileDescriptor, "Picture: x = %d, y = %d, w = %d, h = %d, name = \"",
				 image->PicXArea, image->PicYArea, image->PicWArea, image->PicHArea);
			if (image->PicFileName)
			  fprintf (fileDescriptor, "%s\" ", image->PicFileName);
			else
			  fprintf (fileDescriptor, "\"");
		      }
		    fprintf (fileDescriptor, "ShowBox:");
		    if (pPa1->AbFillBox)
		      fprintf (fileDescriptor, "Y");
		    else
		      fprintf (fileDescriptor, "N");
		    break;
		 case LtPicture:
		    image = (PictInfo *) pPa1->AbPictInfo;
		    if (image == NULL)
		       fprintf (fileDescriptor, "AbPictInfo = NULL");
		    else
		       fprintf (fileDescriptor, "Picture: x = %d, y = %d, w = %d, h = %d, name = %s",
				image->PicXArea, image->PicYArea, image->PicWArea, image->PicHArea,
				image->PicFileName);
		    break;
		 case LtText:
		 case LtReference:
		    fprintf (fileDescriptor, "language = %s", TtaGetLanguageName (pPa1->AbLanguage));
		    fprintf (fileDescriptor, "\n");
		    for (i = 1; i <= Indent + 6; i++)
		       fprintf (fileDescriptor, " ");
		    fprintf (fileDescriptor, " \'");
		    WrText (pPa1->AbText, 60, fileDescriptor);
		    fprintf (fileDescriptor, "\'");
		    break;
		 case LtPolyLine:
		    fprintf (fileDescriptor, "type=%c", pPa1->AbPolyLineShape);
		    fprintf (fileDescriptor, "\n");
		    for (i = 1; i <= Indent + 6; i++)
		       fprintf (fileDescriptor, " ");
		    for (i = 0; i < pPa1->AbVolume && i < 8; i++)
		      {
			 fprintf (fileDescriptor, "%d,%d ",
				  pPa1->AbPolyLineBuffer->BuPoints[i].XCoord,
				pPa1->AbPolyLineBuffer->BuPoints[i].YCoord);
		      }
		    if (i < pPa1->AbVolume)
		       fprintf (fileDescriptor, "...");
		    break;
		 /*CP */ case LtSymbol:
		 case LtGraphics:
		    fprintf (fileDescriptor, " alphabet=%c", pPa1->AbGraphAlphabet);
		    fprintf (fileDescriptor, "\'%c\'", pPa1->AbShape);
		    break;
		 default:
		    break;
	      }
	if (pPa1->AbSelected)
	   fprintf (fileDescriptor, " SELECTED");
	if (pPa1->AbNew)
	   fprintf (fileDescriptor, " NEW");
	if (pPa1->AbDead)
	   fprintf (fileDescriptor, " DEAD");
	if (pPa1->AbWidthChange)
	   fprintf (fileDescriptor, " ChngWidth");
	if (pPa1->AbHeightChange)
	   fprintf (fileDescriptor, " ChngHeight");
	if (pPa1->AbHorizPosChange)
	   fprintf (fileDescriptor, " ChngPosH");
	if (pPa1->AbVertPosChange)
	   fprintf (fileDescriptor, " ChngPosV");
	if (pPa1->AbHorizRefChange)
	   fprintf (fileDescriptor, " ChngAxisH");
	if (pPa1->AbVertRefChange)
	   fprintf (fileDescriptor, " ChngAxisV");
	if (pPa1->AbSizeChange)
	   fprintf (fileDescriptor, " ChngSize");
	if (pPa1->AbAspectChange)
	   fprintf (fileDescriptor, " ChngGraphic");
	if (pPa1->AbChange)
	   fprintf (fileDescriptor, " MODIFIED");
	if (pPa1->AbOnPageBreak)
	   fprintf (fileDescriptor, " ON PAGE BOUNDARY");
	if (pPa1->AbAfterPageBreak)
	   fprintf (fileDescriptor, " OUT OF PAGE");
	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "VertRef:");
	wrpos (&pPa1->AbVertRef, FALSE, fileDescriptor);
	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "HorizRef:");
	wrpos (&pPa1->AbHorizRef, FALSE, fileDescriptor);
	fprintf (fileDescriptor, "\n");
	if (pPa1->AbEnclosing == NULL)
	   root = TRUE;
	else
	   root = FALSE;
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "VertPos:");
	wrpos (&pPa1->AbVertPos, root, fileDescriptor);
	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "HorizPos:");
	wrpos (&pPa1->AbHorizPos, root, fileDescriptor);
	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "Width:");
	pPavDim = &pPa1->AbWidth;
	if (pPavDim->DimIsPosition)
	   wrpos (&pPavDim->DimPosition, root, fileDescriptor);
	else
	   wrdim (pPavDim, root, TRUE, fileDescriptor);
	fprintf (fileDescriptor, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (fileDescriptor, " ");
	fprintf (fileDescriptor, "Height:");
	pPavDim = &pPa1->AbHeight;
	if (pPavDim->DimIsPosition)
	   wrpos (&pPavDim->DimPosition, root, fileDescriptor);
	else
	   wrdim (pPavDim, root, FALSE, fileDescriptor);
	fprintf (fileDescriptor, "\n");
	/* liste les regles de presentation retardees */
	if (pPa1->AbDelayedPRule != NULL)
	  {
	     pDelPR = pPa1->AbDelayedPRule;
	     do
	       {
		  for (j = 1; j <= Indent + 6; j++)
		     fprintf (fileDescriptor, " ");
		  fprintf (fileDescriptor, "Deferred rule: ");
		  WrPRuleType (pDelPR->DpPRule, fileDescriptor);
		  fprintf (fileDescriptor, " AbstractBox%d", pDelPR->DpAbsBox->AbNum);
		  if (pDelPR->DpAttribute != NULL)
		    {
		       pAt1 = pDelPR->DpAttribute;
		       fprintf (fileDescriptor, "[%s]", pAt1->AeAttrSSchema->
				SsAttribute[pAt1->AeAttrNum - 1].AttrName);
		    }
		  fprintf (fileDescriptor, "\n");
		  pDelPR = pDelPR->DpNext;
	       }
	     while (pDelPR != NULL);
	  }
	/* affichage du chainage des paves dupliques */
	fprintf (fileDescriptor, "\n");
	if (pPa1->AbPreviousRepeated != NULL)
	  {
	     fprintf (fileDescriptor, " AbstractBox repeats previous: ");
	     fprintf (fileDescriptor, "%d ", pPa1->AbPreviousRepeated->AbNum);
	     fprintf (fileDescriptor, "\n");
	  }
	if (pPa1->AbNextRepeated != NULL)
	  {
	     fprintf (fileDescriptor, " AbstractBox repeats next: ");
	     fprintf (fileDescriptor, "%d ", pPa1->AbNextRepeated->AbNum);
	     fprintf (fileDescriptor, "\n");
	  }
	if (pPa1->AbLeafType == LtCompound)
	  {
	     for (i = 1; i <= Indent + 6; i++)
		fprintf (fileDescriptor, " ");
	     fprintf (fileDescriptor, "line:");
	     wrboolean (pPa1->AbInLine, fileDescriptor);
	     /* display Truncated-Head and Truncated-Tail */
	     /* even if it's a lines block */
	     fprintf (fileDescriptor, " Truncated-Head:");
	     wrboolean (pPa1->AbTruncatedHead, fileDescriptor);
	     fprintf (fileDescriptor, " Truncated-Tail:");
	     wrboolean (pPa1->AbTruncatedTail, fileDescriptor);

	     fprintf (fileDescriptor, "\n");
	     f = pPa1->AbFirstEnclosed;
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
static void         wrtext (char *Text, FILE * fileDescriptor)
#else  /* __STDC__ */
static void         wrtext (Text, fileDescriptor)
char               *Text;
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
   boolean             boucle;
   PtrPosRelations     pTa1;
   BoxRelation        *pRe1;
   PtrDimRelations     pTabD1;
   PictInfo           *image;

   if (pAb->AbBox != NULL)
     {
	for (j = 1; j <= Indent; j++)
	   fprintf (fileDescriptor, " ");
	pBox = pAb->AbBox;
	wrnumber (pBox->BxAbstractBox->AbNum, fileDescriptor);
	fprintf (fileDescriptor, " ");
	wrtext (AbsBoxType (pAb), fileDescriptor);
	fprintf (fileDescriptor, "\n");
	if (pBox != NULL)
	  {
	     for (j = 1; j <= Indent; j++)
		fprintf (fileDescriptor, " ");
	     fprintf (fileDescriptor, " Characters:");
	     wrnumber (pAb->AbVolume, fileDescriptor);
	     if (pAb->AbLeafType == LtText)
	       {
		  fprintf (fileDescriptor, " Spaces:");
		  wrnumber (pBox->BxNSpaces, fileDescriptor);
	       }
	     fprintf (fileDescriptor, "\n");
	     for (j = 1; j <= Indent; j++)
		fprintf (fileDescriptor, " ");
	     fprintf (fileDescriptor, " Width:");
	     wrnumber (pBox->BxWidth, fileDescriptor);
	     fprintf (fileDescriptor, " Height:");
	     wrnumber (pBox->BxHeight, fileDescriptor);
	     if (pBox->BxContentWidth)
		fprintf (fileDescriptor, " Minimum-Width:");
	     else
		fprintf (fileDescriptor, " Content-Width:");
	     wrnumber (pBox->BxRuleWidth, fileDescriptor);
	     if (pBox->BxContentWidth)
		fprintf (fileDescriptor, " Minimum-Height:");
	     else
		fprintf (fileDescriptor, " Content-Height:");
	     wrnumber (pBox->BxRuleHeigth, fileDescriptor);
	     fprintf (fileDescriptor, " Base:");
	     wrnumber (pBox->BxHorizRef, fileDescriptor);
	     fprintf (fileDescriptor, " Axis:");
	     wrnumber (pBox->BxVertRef, fileDescriptor);
	     fprintf (fileDescriptor, " X:");
	     if (pBox->BxType == BoSplit)
		wrnumber (pBox->BxNexChild->BxXOrg, fileDescriptor);
	     else
		wrnumber (pBox->BxXOrg, fileDescriptor);
	     fprintf (fileDescriptor, " Y:");
	     if (pBox->BxType == BoSplit)
		wrnumber (pBox->BxNexChild->BxYOrg, fileDescriptor);
	     else
		wrnumber (pBox->BxYOrg, fileDescriptor);

	     fprintf (fileDescriptor, "\n");
	     for (j = 1; j <= Indent; j++)
		fprintf (fileDescriptor, " ");
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

	     fprintf (fileDescriptor, " Nature:");
	     switch (pAb->AbLeafType)
		   {
		      case LtCompound:
			 if (pBox->BxType == BoGhost)
			    fprintf (fileDescriptor, "GHOST");
			 else if (pAb->AbInLine)
			   fprintf (fileDescriptor, "LINES");
			 else if (pBox->BxType == BoTable)
			    fprintf (fileDescriptor, "TABLE");
			 else if (pBox->BxType == BoColumn)
			    fprintf (fileDescriptor, "COLUMN");
			 else if (pBox->BxType == BoRow)
			    fprintf (fileDescriptor, "ROW");
			 else
			    fprintf (fileDescriptor, "COMP");
			 if (pAb->AbInLine || pBox->BxType == BoTable ||
			     pBox->BxType == BoColumn || pBox->BxType == BoRow)
			   {
			     fprintf (fileDescriptor, "\n");
			     for (j = 1; j <= Indent; j++)
			       fprintf (fileDescriptor, " ");
			     fprintf (fileDescriptor, " Min_Width:");
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
		  pTa1 = pPosRel;
		  boucle = TRUE;
		  i = 1;
		  while (boucle)
		    {
		       pRe1 = &pTa1->PosRTable[i - 1];
		       if (pRe1->ReBox == NULL)
			  boucle = FALSE;
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
			       boucle = FALSE;
			    else
			       i++;
			 }
		    }
		  pPosRel = pTa1->PosRNext;
		  /* Bloc suivant */
	       }
	     fprintf (fileDescriptor, "\n");
	     /* liste des dependances de largeur */
	     pDimRel = pBox->BxWidthRelations;
	     while (pDimRel != NULL)
	       {
		  pTabD1 = pDimRel;
		  boucle = TRUE;
		  i = 1;
		  while (boucle)
		     if (pTabD1->DimRTable[i - 1] == NULL)
			boucle = FALSE;
		     else
		       {
			  fprintf (fileDescriptor, "\n");
			  for (j = 1; j <= Indent + 6; j++)
			     fprintf (fileDescriptor, " ");
			  fprintf (fileDescriptor, "Width changes ");
			  if (pTabD1->DimRSame[i - 1])
			     fprintf (fileDescriptor, "Width of ");
			  else
			     fprintf (fileDescriptor, "Height of ");
			  if (pTabD1->DimRTable[i - 1]->BxAbstractBox != NULL)
			     wrnumber (pTabD1->DimRTable[i - 1]->BxAbstractBox->AbNum, fileDescriptor);
			  else
			     fprintf (fileDescriptor, "?");
			  if (i == MAX_RELAT_DIM)
			     boucle = FALSE;
			  else
			     i++;
		       }
		  pDimRel = pTabD1->DimRNext;
	       }
	     /* liste des dependances de hauteur */
	     pDimRel = pBox->BxHeightRelations;
	     while (pDimRel != NULL)
	       {
		  pTabD1 = pDimRel;
		  boucle = TRUE;
		  i = 1;
		  while (boucle)
		     if (pTabD1->DimRTable[i - 1] == NULL)
			boucle = FALSE;
		     else
		       {
			  fprintf (fileDescriptor, "\n");
			  for (j = 1; j <= Indent + 6; j++)
			     fprintf (fileDescriptor, " ");
			  fprintf (fileDescriptor, "Height changes ");
			  if (pTabD1->DimRSame[i - 1])
			     fprintf (fileDescriptor, "Height of ");
			  else
			     fprintf (fileDescriptor, "Width of ");
			  if (pTabD1->DimRTable[i - 1]->BxAbstractBox != NULL)
			     wrnumber (pTabD1->DimRTable[i - 1]->BxAbstractBox->AbNum, fileDescriptor);
			  if (i == MAX_RELAT_DIM)
			     boucle = FALSE;
			  else
			     i++;
		       }
		  pDimRel = pTabD1->DimRNext;
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
