
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */


/* 
   debug.c : Module de trace et mise au point
   V. Quint - Avril 1985        
   Major changes 
   PMA : 4/4/91       remise en forme
 */
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "imagedrvr.h"

#include "language.h"
#include "application.h"
#include "storage.h"

#undef EXPORT
#define EXPORT extern
#include "edit.var"

extern int          UserErrorCode;

#include "arbabs.f"
#include "pres.f"
#include "debug.f"
#include "thotmsg.f"
#include "storage.f"

/* ---------------------------------------------------------------------- */
/* |    wrTypeReglePres ecrit le type de la regle de presentation       | */
/* |            pointee par pRegle, dans le fichier outfile.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrTypeReglePres (PtrPRule pRegle, FILE * outfile)
#else  /* __STDC__ */
static void         wrTypeReglePres (pRegle, outfile)
PtrPRule        pRegle;
FILE               *outfile;

#endif /* __STDC__ */
{
   switch (pRegle->PrType)
	 {
	    case PtVisibility:
	       fprintf (outfile, "Visib");
	       break;
	    case PtFunction:
	       fprintf (outfile, "Line");
	       break;
	    case PtVertRef:
	       fprintf (outfile, "VRef");
	       break;
	    case PtHorizRef:
	       fprintf (outfile, "HRef");
	       break;
	    case PtHeight:
	       fprintf (outfile, "Height");
	       break;
	    case PtWidth:
	       fprintf (outfile, "Width");
	       break;
	    case PtVertPos:
	       fprintf (outfile, "VPos");
	       break;
	    case PtHorizPos:
	       fprintf (outfile, "HPos");
	       break;
	    case PtSize:
	       fprintf (outfile, "Size");
	       break;
	    case PtUnderline:
	       fprintf (outfile, "Souligne");
	       break;
	    case PtThickness:
	       fprintf (outfile, "Epaisseur");
	       break;
	    case PtStyle:
	       fprintf (outfile, "Style");
	       break;
	    case PtFont:
	       fprintf (outfile, "Font");
	       break;
	    case PtIndent:
	       fprintf (outfile, "Indent");
	       break;
	    case PtLineSpacing:
	       fprintf (outfile, "InterL");
	       break;
	    case PtDepth:
	       fprintf (outfile, "Depth");
	       break;
	    case PtAdjust:
	       fprintf (outfile, "Adjust");
	       break;
	    case PtJustify:
	       fprintf (outfile, "Justify");
	       break;
	    case PtHyphenate:
	       fprintf (outfile, "Hyphenate");
	       break;
	    case PtBreak1:
	       fprintf (outfile, "NoBr1");
	       break;
	    case PtBreak2:
	       fprintf (outfile, "NoBr2");
	       break;
	    case PtLineStyle:
	       fprintf (outfile, "LineStyle");
	       break;
	    case PtLineWeight:
	       fprintf (outfile, "LineWeight");
	       break;
	    case PtFillPattern:
	       fprintf (outfile, "FillPattern");
	       break;
	    case PtBackground:
	       fprintf (outfile, "Background");
	       break;
	    case PtForeground:
	       fprintf (outfile, "Foreground");
	       break;
	    case PtImDescr:
	       fprintf (outfile, "ImageDescriptor");
	       break;
	    default:
	       fprintf (outfile, "PrType ????");
	       break;
	 }

}

/* ---------------------------------------------------------------------- */
/* |    Wrtext ecrit dans le fichier outfile le contenu de la chaine des| */
/* |            buffers de texte commencant au buffer pointe' par pBT.  | */
/* |            Lg: longueur maximum a` ecrire.                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         Wrtext (PtrTextBuffer pBT, int Lg, FILE * outfile)
#else  /* __STDC__ */
static void         Wrtext (pBT, Lg, outfile)
PtrTextBuffer      pBT;
int                 Lg;
FILE               *outfile;

#endif /* __STDC__ */
{
   PtrTextBuffer      b;
   int                 i, l;

   l = 0;
   b = pBT;
   while (b != NULL && l < Lg)
      /* ecrit le contenu du buffer de texte */
     {
	i = 0;
	while (i < b->BuLength && b->BuContent[i] != '\0' && l < Lg)
	  {
	     putc (b->BuContent[i], outfile);
	     i++;
	     l++;
	  }
	if (l < Lg)
	  {
	     fprintf (outfile, "|");
	     b = b->BuNext;
	     /* buffer de texte suivant du meme element */
	  }
	else
	   fprintf (outfile, "...");
     }
}

#define MAXNB 200
static int          NbFils[MAXNB];
static int          NiveauFeuille[MAXNB];
static int          NbElemNiveau[MAXNB];
static int          NbFreresAscend[MAXNB];
static int          NbNoeuds;
static int          NbFeuilles;

/* ---------------------------------------------------------------------- */
/* |    CountNodes ecrit dans le fichier outfile les statistiques       | */
/* |            sur le sous-arbre de racine Noeud.                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                CountNodes (PtrElement Noeud, FILE * outfile, int level)

#else  /* __STDC__ */
void                CountNodes (Noeud, outfile, level)
PtrElement          Noeud;
FILE               *outfile;
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

   if (Noeud != NULL)
     {
	if (level == 0)
	  {
	     for (i = 0; i < MAXNB; i++)
	       {
		  NbFils[i] = 0;
		  NiveauFeuille[i] = 0;
		  NbElemNiveau[i] = 0;
		  NbFreresAscend[i] = 0;
	       }
	     NbNoeuds = 0;
	     NbFeuilles = 0;
	  }
	pEl = Noeud;
	NbNoeuds++;
	NbElemNiveau[level]++;
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
	NbFreresAscend[level] += nbf;
	if (pEl->ElTerminal)
	  {
	     NiveauFeuille[level]++;
	     NbFeuilles++;
	  }
	else
	  {
	     /* element non terminal, on traite sa descendance */
	     f = pEl->ElFirstChild;
	     nbf = 0;
	     while (f != NULL)
	       {
		  CountNodes (f, outfile, level + 1);
		  nbf++;
		  f = f->ElNext;
	       }
	     NbFils[nbf]++;
	  }

	if (level == 0)
	   /* on a parcourru tout l'arbre, on imprime les resultats */
	  {
	     fprintf (outfile, "L'arbre ");
	     /* ecrit le nom du type de l'element */
	     if (pEl->ElSructSchema != NULL)
	       {
		  pRe1 = &pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1];
		  fprintf (outfile, "%s", pRe1->SrName);
		  /* ecrit le nom du schema de structure de l'element */
		  fprintf (outfile, "(%s) ", pEl->ElSructSchema->SsName);
	       }
	     fprintf (outfile, "contient %d elements, dont %d feuilles\n\n", NbNoeuds, NbFeuilles);

	     der = MAXNB - 1;
	     while (NiveauFeuille[der] == 0 && NbElemNiveau[der] == 0)
		der--;

	     fprintf (outfile, "Profondeur\tfeuilles\telements\tnb. moyen de freres des ascend.\n");
	     Prof = 0;
	     for (i = 0; i <= der; i++)
	       {
		  fprintf (outfile, "%d\t\t%d", i, NiveauFeuille[i]);
		  Prof += i * NiveauFeuille[i];
		  fprintf (outfile, "\t\t%d", NbElemNiveau[i]);
		  Moyenne = (float) NbFreresAscend[i] / (float) NbElemNiveau[i];
		  fprintf (outfile, "\t\t%f\n", Moyenne);
	       }
	     Moyenne = (float) Prof / (float) NbFeuilles;
	     fprintf (outfile, "Profondeur moyenne : %f\n", Moyenne);
	     Moyenne = (float) NbNoeuds / (float) (der + 1);
	     fprintf (outfile, "Nombre moyen d'elements par niveau : %f\n\n", Moyenne);

	     der = MAXNB - 1;
	     while (NbFils[der] == 0)
		der--;
	     fprintf (outfile, "Nombre de fils   nombre d'elements ayant ce nombre de fils\n");
	     Fils = 0;
	     for (i = 0; i <= der; i++)
	       {
		  fprintf (outfile, "%d \t\t %d\n", i, NbFils[i]);
		  Fils += i * NbFils[i];
	       }
	     Moyenne = (float) Fils / (float) (NbNoeuds - NbFeuilles);
	     fprintf (outfile, "Nombre moyen de fils par noeud non feuille : %f\n\n", Moyenne);
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |     wrRef ecrit une reference.                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrRef (PtrReference pRef, FILE * outfile)
#else  /* __STDC__ */
static void         wrRef (pRef, outfile)
PtrReference        pRef;
FILE               *outfile;

#endif /* __STDC__ */
{
   PtrReferredDescr    pDe1;
   Name                 N;

   switch (pRef->RdTypeRef)
	 {
	    case RefFollow:
	       fprintf (outfile, "Renvoi");
	       break;
	    case RefInclusion:
	       fprintf (outfile, "Inclusion");
	       break;
	    default:
	       fprintf (outfile, "RdTypeRef ????");
	       break;
	 }

   if (pRef->RdInternalRef)
      fprintf (outfile, " interne ");
   else
      fprintf (outfile, " externe ");
   if (pRef->RdReferred == NULL)
      fprintf (outfile, "*RdReferred=NULL*");
   else
     {
	pDe1 = pRef->RdReferred;
	if (pDe1->ReExternalRef)
	  {
	     fprintf (outfile, "%s(", pDe1->ReReferredLabel);
	     GetDocName (pDe1->ReExtDocument, N);
	     fprintf (outfile, "%s)", N);
	  }
	else if (pDe1->ReReferredElem == NULL)
	   fprintf (outfile, "ReReferredElem=NULL, ReReferredLabel=%s", pDe1->ReReferredLabel);
	else
	   fprintf (outfile, "%s", pDe1->ReReferredElem->ElLabel);
     }
}

/* ---------------------------------------------------------------------- */
/* |    AffArbre ecrit dans le fichier outfile la representation        | */
/* |            textuelle indentee du sous-arbre de la representation   | */
/* |            interne commencant au noeud pointe par Noeud, avec      | */
/* |            l'indentation Indent.                                   | */
/* |            Si premierfils = true alors on ne traduit recursivement | */
/* |            que le premier fils de chaque element.                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         AffArbre (PtrElement Noeud, int Indent, FILE * outfile, boolean premierfils)
#else  /* __STDC__ */
static void         AffArbre (Noeud, Indent, outfile, premierfils)
PtrElement          Noeud;
int                 Indent;
FILE               *outfile;
boolean             premierfils;

#endif /* __STDC__ */
{
   int                 i;
   PtrElement          f;
   PtrAttribute         pAttr;
   PtrPRule        pRegle;
   SRule              *pRe1;
   PtrAttribute         pAt1;
   TtAttribute           *pAttr1;
   PtrPRule        pRegl1;

   if (Noeud != NULL)
     {
	/* ecrit les blancs de l'indentation */
	for (i = 1; i <= Indent; i++)
	   fprintf (outfile, " ");
	/* si l'element est la copie par inclusion d'un autre element, */
	/* ecrit la reference a cet autre element */
	if (Noeud->ElSource != NULL)
	  {
	     wrRef (Noeud->ElSource, outfile);
	     fprintf (outfile, "\n");
	     for (i = 1; i <= Indent; i++)
		fprintf (outfile, " ");
	  }
	i = 1;
	/* ecrit le nom du type de l'element */
	if (Noeud->ElSructSchema == NULL)
	   fprintf (outfile, "*ElSructSchema=NULL*");
	else
	  {
	     pRe1 = &Noeud->ElSructSchema->SsRule[Noeud->ElTypeNumber - 1];
	     fprintf (outfile, "%s", pRe1->SrName);
	     /* ecrit le nom du schema de structure de l'element */
	     fprintf (outfile, "(%s)", Noeud->ElSructSchema->SsName);
	  }
	fprintf (outfile, " Label=%s", Noeud->ElLabel);
	/* ecrit le volume de l'element */
	fprintf (outfile, " Vol=%d", Noeud->ElVolume);
	if (Noeud->ElAssocNum != 0)
	   fprintf (outfile, " Assoc=%d", Noeud->ElAssocNum);
	if (Noeud->ElIsCopy)
	   fprintf (outfile, " Copy");
	switch (Noeud->ElAccess)
	      {
		 case AccessReadOnly:
		    fprintf (outfile, " Right=RO");
		    break;
		 case AccessReadWrite:
		    fprintf (outfile, " Right=R/W");
		    break;
		 case AccessHidden:
		    fprintf (outfile, " Right=Hidden");
		    break;
		 default:
		    break;
	      }
	if (Noeud->ElHolophrast)
	   fprintf (outfile, " Holophrast");

	/* ecrit les attributs de l'element */
	if (Noeud->ElFirstAttr != NULL)
	  {
	     fprintf (outfile, " (ATTR ");
	     pAttr = Noeud->ElFirstAttr;
	     while (pAttr != NULL)
	       {
		  pAt1 = pAttr;
		  pAttr1 = &pAt1->AeAttrSSchema->SsAttribute[pAt1->AeAttrNum - 1];
		  fprintf (outfile, "%s=", pAttr1->AttrName);
		  switch (pAttr1->AttrType)
			{
			   case AtNumAttr:
			      fprintf (outfile, "%d", pAt1->AeAttrValue);
			      break;
			   case AtTextAttr:
			      if (pAt1->AeAttrText != NULL)
				{
				   fprintf (outfile, "%s", pAt1->AeAttrText->BuContent);
				   if (pAt1->AeAttrText->BuNext != NULL)
				      fprintf (outfile, "...");
				}
			      break;
			   case AtReferenceAttr:
			      if (pAt1->AeAttrReference == NULL)
				 fprintf (outfile, "*AeAttrReference=NULL*");
			      else if (pAt1->AeAttrReference->RdReferred == NULL)
				 fprintf (outfile, "*RdReferred=NULL*");
			      else
				 wrRef (pAt1->AeAttrReference, outfile);
			      break;
			   case AtEnumAttr:
			      fprintf (outfile, "%s", pAttr1->AttrEnumValue[pAt1->AeAttrValue - 1]);
			      break;
			   default:
			      fprintf (outfile, "AttrType ????");
			      break;
			}

		  if (pAt1->AeNext != NULL)
		     fprintf (outfile, ", ");
		  pAttr = pAt1->AeNext;
	       }
	     fprintf (outfile, ")");
	  }
	/* ecrit les regles de presentation specifiques de l'element */
	if (Noeud->ElFirstPRule != NULL)
	  {
	     fprintf (outfile, " Pres(");
	     pRegle = Noeud->ElFirstPRule;
	     while (pRegle != NULL)
	       {
		  pRegl1 = pRegle;
		  if (pRegle != Noeud->ElFirstPRule)
		     fprintf (outfile, ", ");
		  wrTypeReglePres (pRegle, outfile);
		  if (pRegl1->PrSpecifAttr > 0)
		     fprintf (outfile, "[%s]", pRegl1->PrSpecifAttrSSchema->
			    SsAttribute[pRegl1->PrSpecifAttr - 1].AttrName);
		  fprintf (outfile, " vue%d", pRegl1->PrViewNum);
		  pRegle = pRegl1->PrNextPRule;
	       }
	     fprintf (outfile, ")");
	     /* ecrit le contenu de l'element */
	  }
	if (Noeud->ElTerminal)
	   switch (Noeud->ElLeafType)
		 {
		    case LtPicture:
		       fprintf (outfile, " Lg=%d\n", Noeud->ElTextLength);
		       for (i = 1; i <= Indent; i++)
			  fprintf (outfile, " ");
		       fprintf (outfile, "\'");
		       Wrtext (Noeud->ElText, 72 - Indent, outfile);
		       fprintf (outfile, "\'\n");
		       break;
		    case LtText:
		       fprintf (outfile, " Lg=%d Language=%s\n", Noeud->ElTextLength,
				TtaGetLanguageName (Noeud->ElLanguage));
		       for (i = 1; i <= Indent; i++)
			  fprintf (outfile, " ");
		       fprintf (outfile, "\'");
		       Wrtext (Noeud->ElText, 72 - Indent, outfile);
		       fprintf (outfile, "\'\n");
		       break;
		    case LtPlyLine:
		       fprintf (outfile, " Type=%c %d points\n", Noeud->ElPolyLineType,
				Noeud->ElNPoints);
		       for (i = 1; i <= Indent; i++)
			  fprintf (outfile, " ");
		       for (i = 0; i < Noeud->ElNPoints && i < 8; i++)
			 {
			    fprintf (outfile, "%d,%d ", Noeud->ElPolyLineBuffer->BuPoints[i].XCoord,
				  Noeud->ElPolyLineBuffer->BuPoints[i].YCoord);
			 }
		       if (i < Noeud->ElNPoints)
			  fprintf (outfile, "...");
		       fprintf (outfile, "\n");
		       break;
		    case LtSymbol:
		    case LtGraphics:
		    case LtCompound:
		       fprintf (outfile, " \'%c\'\n", Noeud->ElGraph);
		       break;
		    case LtPageColBreak:
		       fprintf (outfile, " Number=%d View=%d", Noeud->ElPageNumber,
				Noeud->ElViewPSchema);
		       switch (Noeud->ElPageType)
			     {
				case PgComputed:
				   fprintf (outfile, " Computed page");
				   break;
				case PgBegin:
				   fprintf (outfile, " Begin of element");
				   break;
				case PgUser:
				   fprintf (outfile, " Page put by user");
				   break;
				case ColComputed:
				   fprintf (outfile, " Computed column");
				   break;
				case ColBegin:
				   fprintf (outfile, " First column");
				   break;
				case ColUser:
				   fprintf (outfile, " Column put by user");
				   break;
				case ColGroup:
				   fprintf (outfile, " Grouped column");
				   break;
				default:
				   break;
			     }
		       fprintf (outfile, "\n");
		       break;
		    case LtReference:
		       if (Noeud->ElReference == NULL)
			  fprintf (outfile, " *ElReference=NULL*\n");
		       else
			 {
			    fprintf (outfile, " ");
			    wrRef (Noeud->ElReference, outfile);
			    fprintf (outfile, "\n");
			 }
		       break;
		    case LtPairedElem:
		       fprintf (outfile, "(Id=%d)", Noeud->ElPairIdent);
		       if (Noeud->ElOtherPairedEl == NULL)
			  fprintf (outfile, " ElOtherPairedEl=NULL");
		       fprintf (outfile, "\n");
		       break;
		    default:
		       fprintf (outfile, "ElLeafType ????\n");
		       break;
		 }
	else
	  {
	     fprintf (outfile, "\n");
	     /* element non terminal, on ecrit sa descendance */
	     f = Noeud->ElFirstChild;
	     while (f != NULL)
	       {
		  AffArbre (f, Indent + 2, outfile, premierfils);
		  if (!premierfils)
		     f = f->ElNext;
		  else
		     f = NULL;
	       }
	  }
     }
}

/* ----------------------------------------------------------------------
   TtaListAbstractTree

   Produces in a file a human-readable form of an abstract tree.

   Parameters:
   root: the root element of the tree to be listed.
   fileDescriptor: file descriptor of the file that will contain the list.
   This file must be open when calling the function.

   ---------------------------------------------------------------------- */
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
      AffArbre ((PtrElement) root, 0, fileDescriptor, False);
}

/* ----------------------------------------------------------------------
   TtaListView

   Produces in a file a human-readable form of an abstract view.

   Parameters:
   document: the document.
   view: the view.
   fileDescriptor: file descriptor of the file that will contain the list.
   This file must be open when calling the function.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaListView (Document document, View view, FILE * fileDescriptor)
#else  /* __STDC__ */
void                TtaListView (document, view, fileDescriptor)
Document            document;
View                view;
FILE               *fileDescriptor;

#endif /* __STDC__ */
{
   PtrAbstractBox             PavRacine;
   PtrDocument         pDoc;
   int                 numAssoc;

   UserErrorCode = 0;
   /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
     {
	pDoc = TabDocuments[document - 1];
	if (view < 100)
	   /* vue de l'arbre principal */
	   if (view < 1 || view > MAX_VIEW_DOC)
	      TtaError (ERR_invalid_parameter);
	   else
	     {
		PavRacine = pDoc->DocViewRootAb[view - 1];
		NumPav (PavRacine);
		AffPaves (PavRacine, 0, fileDescriptor);
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
		  NumPav (PavRacine);
		  AffPaves (PavRacine, 0, fileDescriptor);
	       }
	  }
     }
}



/* ---------------------------------------------------------------------- */
/* |    NumeroPave numerote recursivement des paves a partir de pAb.   | */
/* |            cptpave est le compte des paves.                        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         NumeroPave (PtrAbstractBox pAb, int *cptpave)
#else  /* __STDC__ */
static void         NumeroPave (pAb, cptpave)
PtrAbstractBox             pAb;
int                *cptpave;

#endif /* __STDC__ */
{
   PtrAbstractBox             pP;

   (*cptpave)++;
   pAb->AbNum = *cptpave;
   pP = pAb->AbFirstEnclosed;
   while (pP != NULL)
     {
	NumeroPave (pP, cptpave);
	pP = pP->AbNext;
     }
}

/* ---------------------------------------------------------------------- */
/* |    NumPav numerote tous les paves du sous-arbre dont la racine est | */
/* |            pointee par pP. Appelle NumeroPave.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                NumPav (PtrAbstractBox pP)
#else  /* __STDC__ */
void                NumPav (pP)
PtrAbstractBox             pP;

#endif /* __STDC__ */
{
   int                 cptpave;	/* compteur pour numerotation des paves */

   cptpave = 0;
   if (pP != NULL)
      NumeroPave (pP, &cptpave);
}

/* ---------------------------------------------------------------------- */
/* |    wrboolean ecrit la valeur d'un booleen.                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrboolean (boolean b, FILE * outfile)
#else  /* __STDC__ */
static void         wrboolean (b, outfile)
boolean             b;
FILE               *outfile;

#endif /* __STDC__ */
{
   if (b)
      fprintf (outfile, "Y");
   else
      fprintf (outfile, "N");
}

/* ---------------------------------------------------------------------- */
/* |    wrrepere ecrit la valeur du point de reference.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrrepere (BoxEdge r, FILE * outfile)
#else  /* __STDC__ */
static void         wrrepere (r, outfile)
BoxEdge         r;
FILE               *outfile;

#endif /* __STDC__ */
{
   switch (r)
	 {
	    case Top:
	       fprintf (outfile, "Top");
	       break;
	    case Bottom:
	       fprintf (outfile, "Bottom");
	       break;
	    case Left:
	       fprintf (outfile, "Left");
	       break;
	    case Right:
	       fprintf (outfile, "Right");
	       break;
	    case HorizRef:
	       fprintf (outfile, "HRef");
	       break;
	    case VertRef:
	       fprintf (outfile, "VRef");
	       break;
	    case HorizMiddle:
	       fprintf (outfile, "HMiddle");
	       break;
	    case VertMiddle:
	       fprintf (outfile, "VMiddle");
	       break;
	    case NoEdge:
	       fprintf (outfile, "????");
	       break;
	    default:
	       fprintf (outfile, "BoxEdge ????");
	       break;
	 }

}

/* ---------------------------------------------------------------------- */
/* |    wrTypeUnit write a distance unit.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrTypeUnit (TypeUnit unit, FILE * outfile)
#else  /* __STDC__ */
static void         wrTypeUnit (unit, outfile)
TypeUnit            unit;
FILE               *outfile;

#endif /* __STDC__ */
{
   switch (unit)
	 {
	    case UnRelative:
	       break;
	    case UnXHeight:
	       fprintf (outfile, " ex");
	       break;
	    case UnPoint:
	       fprintf (outfile, " pt");
	       break;
	    case UnPixel:
	       fprintf (outfile, " px");
	       break;
	    case UnPercent:
	       fprintf (outfile, " %%");
	       break;
	    default:
	       fprintf (outfile, " ???");
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* |    wrpos ecrit la valeur d'une position.                           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrpos (AbPosition * pPos, boolean racine, FILE * outfile)
#else  /* __STDC__ */
static void         wrpos (pPos, racine, outfile)
AbPosition        *pPos;
boolean             racine;
FILE               *outfile;

#endif /* __STDC__ */
{
   fprintf (outfile, " ");
   if (pPos->PosAbRef == NULL && !racine)
      fprintf (outfile, "PosRef = NULL");
   else
     {
	wrrepere (pPos->PosEdge, outfile);
	if (racine && pPos->PosAbRef == NULL)
	   fprintf (outfile, " = ThotWindow.");
	else
	   fprintf (outfile, " = AbstractBox%d.", pPos->PosAbRef->AbNum);
	wrrepere (pPos->PosRefEdge, outfile);
	if (pPos->PosDistance != 0)
	  {
	     if (pPos->PosDistance < 0)
		fprintf (outfile, "-");
	     else
		fprintf (outfile, "+");
	     fprintf (outfile, "%d", abs (pPos->PosDistance));
	     wrTypeUnit (pPos->PosUnit, outfile);
	  }
	if (pPos->PosUserSpecified)
	   fprintf (outfile, " UserSpec");
     }
}

/* ---------------------------------------------------------------------- */
/* |    wrdim ecrit la valeur d'une dimension.                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrdim (AbDimension * pDim, boolean racine, boolean larg, FILE * outfile)
#else  /* __STDC__ */
static void         wrdim (pDim, racine, larg, outfile)
AbDimension       *pDim;
boolean             racine;
boolean             larg;
FILE               *outfile;

#endif /* __STDC__ */
{

   fprintf (outfile, " ");
   if (pDim->DimAbRef == NULL && pDim->DimValue == 0)
      fprintf (outfile, "default");
   else
     {
	if (pDim->DimAbRef == NULL && racine && pDim->DimValue <= 0)
	   /* dimension relative a la fenetre */
	   fprintf (outfile, "ThotWindow");
	if (pDim->DimAbRef != NULL)
	   /* dimension relative a un autre pave */
	   fprintf (outfile, "AbstractBox%d", pDim->DimAbRef->AbNum);
	if ((pDim->DimAbRef != NULL) ||
	    (pDim->DimAbRef == NULL && racine && pDim->DimValue <= 0))
	   /* dimension relative */
	  {
	     if (pDim->DimSameDimension && larg)
		fprintf (outfile, ".Width");
	     else
		fprintf (outfile, ".Height");
	     if (pDim->DimUnit == UnPercent)
		fprintf (outfile, "*");
	     else if (pDim->DimValue > 0)
		fprintf (outfile, "+");
	  }
	if (pDim->DimValue != 0)
	  {
	     fprintf (outfile, "%d", pDim->DimValue);
	     wrTypeUnit (pDim->DimUnit, outfile);
	  }
	if (pDim->DimUserSpecified)
	   fprintf (outfile, " UserSpec");
	if (pDim->DimMinimum)
	   fprintf (outfile, " Min");
     }
}

/* ---------------------------------------------------------------------- */
/* |    AffPaves ecrit dans le fichier outfile le sous-arbre de paves   | */
/* |            commencant au pave pointe' par pAb, et avec            | */
/* |            l'indentation Indent.                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                AffPaves (PtrAbstractBox pAb, int Indent, FILE * outfile)
#else  /* __STDC__ */
void                AffPaves (pAb, Indent, outfile)
PtrAbstractBox             pAb;
int                 Indent;
FILE               *outfile;

#endif /* __STDC__ */
{
   int                 i, j;
   PtrAbstractBox             f;
   boolean             root;
   PtrDelayedPRule     pDelPR;
   PtrAbstractBox             pPa1;
   SRule              *pRe1;
   AbDimension       *pPavDim;
   PtrAttribute         pAt1;
   ImageDescriptor    *image;

   if (pAb != NULL)
     {
	pPa1 = pAb;
	fprintf (outfile, "\n%d ", pPa1->AbNum);	/* numero du pave */
	for (i = 1; i <= Indent; i++)
	   fprintf (outfile, " ");
	pRe1 = &pPa1->AbElement->
	   ElSructSchema->SsRule[pPa1->AbElement->ElTypeNumber - 1];
	fprintf (outfile, "%s", pRe1->SrName);
	fprintf (outfile, " ");
	if (pPa1->AbElement->ElTypeNumber == ord (PageBreak) + 1)
	  {
	     fprintf (outfile, "%d", pPa1->AbElement->ElPageType);
	     fprintf (outfile, " ");
	  }
	if (pPa1->AbPresentationBox)
	   fprintf (outfile, ".%s", pPa1->AbPSchema->
		    PsPresentBox[pPa1->AbTypeNum - 1].PbName);
	fprintf (outfile, " TypeNum:%d", pPa1->AbTypeNum);
	fprintf (outfile, " El:%s", pPa1->AbElement->ElLabel);
	fprintf (outfile, " Vol:%d", pPa1->AbVolume);
	fprintf (outfile, " View%d", pPa1->AbDocView);
	fprintf (outfile, " Visib:%d", pPa1->AbVisibility);
	fprintf (outfile, " Font:%c", pPa1->AbFont);
	fprintf (outfile, " HighL:%d", pPa1->AbHighlight);
	fprintf (outfile, " Size:%d", pPa1->AbSize);
	wrTypeUnit (pPa1->AbSizeUnit, outfile);
	fprintf (outfile, " Indent:%d", pPa1->AbIndent);
	wrTypeUnit (pPa1->AbIndentUnit, outfile);
	fprintf (outfile, " Depth:%d", pPa1->AbDepth);
	fprintf (outfile, " align:");
	switch (pPa1->AbAdjust)
	      {
		 case AlignLeft:
		    fprintf (outfile, "left");
		    break;
		 case AlignRight:
		    fprintf (outfile, "right");
		    break;
		 case AlignCenter:
		    fprintf (outfile, "center");
		    break;
		 case AlignLeftDots:
		    fprintf (outfile, "leftDots");
		    break;
		 default:
		    fprintf (outfile, "AbAdjust ????");
		    break;
	      }
	fprintf (outfile, " Justif:");
	wrboolean (pPa1->AbJustify, outfile);
	fprintf (outfile, " Hyphen:");
	wrboolean (pPa1->AbHyphenate, outfile);
	if (pPa1->AbNotInLine)
	   fprintf (outfile, " NotInLine");

	if (!pPa1->AbHorizEnclosing)
	   fprintf (outfile, " Not-Horiz-Enclosed");
	if (!pPa1->AbVertEnclosing)
	   fprintf (outfile, " Not-Vert-Enclosed");

	fprintf (outfile, " Linespace:%d", pPa1->AbLineSpacing);
	wrTypeUnit (pPa1->AbLineSpacingUnit, outfile);
	fprintf (outfile, " Pattern:%d", pPa1->AbFillPattern);
	fprintf (outfile, " Background:%d", pPa1->AbBackground);
	fprintf (outfile, " Foreground:%d", pPa1->AbForeground);
	fprintf (outfile, " LineStyle:%c", pPa1->AbLineStyle);
	fprintf (outfile, " LineWeight:%d", pPa1->AbLineWeight);
	wrTypeUnit (pPa1->AbLineWeightUnit, outfile);
	fprintf (outfile, " Modif:");
	wrboolean (pPa1->AbCanBeModified, outfile);
	fprintf (outfile, " PageBreak:");
	wrboolean (pPa1->AbAcceptPageBreak, outfile);
	fprintf (outfile, " LineBreak:");
	wrboolean (pPa1->AbAcceptLineBreak, outfile);
	fprintf (outfile, " Actif:");
	wrboolean (pPa1->AbSensitive, outfile);
	fprintf (outfile, " R/O:");
	wrboolean (pPa1->AbReadOnly, outfile);
	fprintf (outfile, " AbstractBox Pres:");
	wrboolean (pPa1->AbPresentationBox, outfile);
	if (!pPa1->AbHorizEnclosing)
	   fprintf (outfile, " HorizEncl:N");
	if (!pPa1->AbVertEnclosing)
	   fprintf (outfile, " VertEncl:N");
	fprintf (outfile, " Nature:");
	switch (pPa1->AbLeafType)
	      {
		 case LtCompound:
		    fprintf (outfile, "comp");
		    break;
		 case LtPicture:
		    fprintf (outfile, "picture\n");
		    for (i = 1; i <= Indent + 6; i++)
		       fprintf (outfile, " ");
		    break;
		 case LtText:
		    fprintf (outfile, "text ");

		    break;
		 case LtPlyLine:
		    fprintf (outfile, "polyline ");
		    break;
		 case LtPageColBreak:
		    fprintf (outfile, "page");
		    break;
		 case LtSymbol:
		    fprintf (outfile, "symbol");
		    break;
		 case LtGraphics:
		    fprintf (outfile, "graphics");
		    break;
		 case LtReference:
		    fprintf (outfile, "refer");
		    break;
		 default:
		    fprintf (outfile, "AbLeafType ????");
		    break;
	      }
	switch (pPa1->AbLeafType)
	      {
		 case LtCompound:

		    break;
		 case LtPicture:
		    image = (ImageDescriptor *) pPa1->AbImageDescriptor;
		    if (image == NULL)
		       fprintf (outfile, "AbImageDescriptor = NULL");
		    else
		       fprintf (outfile, "x = %d, y = %d, w = %d, h = %d, name = %s",
			     image->xcf, image->ycf, image->wcf, image->hcf,
				image->imageFileName);
		    break;
		 case LtText:
		 case LtReference:
		    fprintf (outfile, "language=%s\n", TtaGetLanguageName (pPa1->AbLanguage));
		    for (i = 1; i <= Indent + 6; i++)
		       fprintf (outfile, " ");
		    fprintf (outfile, " \'");
		    Wrtext (pPa1->AbText, 60, outfile);
		    fprintf (outfile, "\'");
		    break;
		 case LtPlyLine:
		    fprintf (outfile, "type=%c\n", pPa1->AbPolyLineShape);
		    for (i = 1; i <= Indent + 6; i++)
		       fprintf (outfile, " ");
		    for (i = 0; i < pPa1->AbVolume && i < 8; i++)
		      {
			 fprintf (outfile, "%d,%d ",
				  pPa1->AbPolyLineBuffer->BuPoints[i].XCoord,
				  pPa1->AbPolyLineBuffer->BuPoints[i].YCoord);
		      }
		    if (i < pPa1->AbVolume)
		       fprintf (outfile, "...");
		    break;
		 /*CP */ case LtSymbol:
		 case LtGraphics:
		    fprintf (outfile, " alphabet=%c", pPa1->AbGraphAlphabet);
		    fprintf (outfile, "\'%c\'", pPa1->AbShape);
		    break;
		 default:
		    break;
	      }
	if (pPa1->AbSelected)
	   fprintf (outfile, " SELECTED");
	if (pPa1->AbNew)
	   fprintf (outfile, " NEW");
	if (pPa1->AbDead)
	   fprintf (outfile, " DEAD");
	if (pPa1->AbWidthChange)
	   fprintf (outfile, " ChngWidth");
	if (pPa1->AbHeightChange)
	   fprintf (outfile, " ChngHeight");
	if (pPa1->AbHorizPosChange)
	   fprintf (outfile, " ChngPosH");
	if (pPa1->AbVertPosChange)
	   fprintf (outfile, " ChngPosV");
	if (pPa1->AbHorizRefChange)
	   fprintf (outfile, " ChngAxisH");
	if (pPa1->AbVertRefChange)
	   fprintf (outfile, " ChngAxisV");
	if (pPa1->AbSizeChange)
	   fprintf (outfile, " ChngSize");
	if (pPa1->AbAspectChange)
	   fprintf (outfile, " ChngGraphic");
	if (pPa1->AbChange)
	   fprintf (outfile, " MODIFIED");
	if (pPa1->AbOnPageBreak)
	   fprintf (outfile, " ON PAGE BOUNDARY");
	if (pPa1->AbAfterPageBreak)
	   fprintf (outfile, " OUT OF PAGE");
	fprintf (outfile, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (outfile, " ");
	fprintf (outfile, "VertRef:");
	wrpos (&pPa1->AbVertRef, False, outfile);
	fprintf (outfile, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (outfile, " ");
	fprintf (outfile, "HorizRef:");
	wrpos (&pPa1->AbHorizRef, False, outfile);
	fprintf (outfile, "\n");
	if (pPa1->AbEnclosing == NULL)
	   root = True;
	else
	   root = False;
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (outfile, " ");
	fprintf (outfile, "VertPos:");
	wrpos (&pPa1->AbVertPos, root, outfile);
	fprintf (outfile, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (outfile, " ");
	fprintf (outfile, "HorizPos:");
	wrpos (&pPa1->AbHorizPos, root, outfile);
	fprintf (outfile, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (outfile, " ");
	fprintf (outfile, "Width:");
	pPavDim = &pPa1->AbWidth;
	if (pPavDim->DimIsPosition)
	   wrpos (&pPavDim->DimPosition, root, outfile);
	else
	   wrdim (pPavDim, root, True, outfile);
	fprintf (outfile, "\n");
	for (j = 1; j <= Indent + 6; j++)
	   fprintf (outfile, " ");
	fprintf (outfile, "Height:");
	pPavDim = &pPa1->AbHeight;
	if (pPavDim->DimIsPosition)
	   wrpos (&pPavDim->DimPosition, root, outfile);
	else
	   wrdim (pPavDim, root, False, outfile);
	fprintf (outfile, "\n");
	/* liste les regles de presentation retardees */
	if (pPa1->AbDelayedPRule != NULL)
	  {
	     pDelPR = pPa1->AbDelayedPRule;
	     do
	       {
		  for (j = 1; j <= Indent + 6; j++)
		     fprintf (outfile, " ");
		  fprintf (outfile, "Deferred rule: ");
		  wrTypeReglePres (pDelPR->DpPRule, outfile);
		  fprintf (outfile, " AbstractBox%d", pDelPR->DpAbsBox->AbNum);
		  if (pDelPR->DpAttribute != NULL)
		    {
		       pAt1 = pDelPR->DpAttribute;
		       fprintf (outfile, "[%s]", pAt1->AeAttrSSchema->
				SsAttribute[pAt1->AeAttrNum - 1].AttrName);
		    }
		  fprintf (outfile, "\n");
		  pDelPR = pDelPR->DpNext;
	       }
	     while (pDelPR != NULL);
	  }
	/* affichage du chainage des paves dupliques */
	fprintf (outfile, "\n");
	if (pPa1->AbPreviousRepeated != NULL)
	  {
	     fprintf (outfile, " AbstractBox duplique precedent : ");
	     fprintf (outfile, "%d ", pPa1->AbPreviousRepeated->AbNum);
	     fprintf (outfile, "\n");
	  }
	if (pPa1->AbNextRepeated != NULL)
	  {
	     fprintf (outfile, " AbstractBox duplique suivant  : ");
	     fprintf (outfile, "%d ", pPa1->AbNextRepeated->AbNum);
	     fprintf (outfile, "\n");
	  }
	if (pPa1->AbLeafType == LtCompound)
	  {
	     for (i = 1; i <= Indent + 6; i++)
		fprintf (outfile, " ");
	     fprintf (outfile, "line:");
	     wrboolean (pPa1->AbInLine, outfile);
	     /* on affiche systematiquement CT et CQ meme si */
	     /* c'est un pave mis en ligne */
	     fprintf (outfile, " CT:");
	     wrboolean (pPa1->AbTruncatedHead, outfile);
	     fprintf (outfile, " CQ:");
	     wrboolean (pPa1->AbTruncatedTail, outfile);

	     fprintf (outfile, "\n");
	     f = pPa1->AbFirstEnclosed;
	     while (f != NULL)
	       {
		  if (f->AbEnclosing != pAb)
		     if (f->AbEnclosing == NULL)
			fprintf (outfile,
				 "AbstractBox suivant : AbEnclosing=NULL\n");
		     else
			fprintf (outfile,
				 "AbstractBox suivant : erreur AbEnclosing\n");
		  AffPaves (f, Indent + 2, outfile);
		  f = f->AbNext;
	       }
	  }
     }
}
/* End Of Module debug */
