
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   liste.c : module de trace des e'tats du Mediateur.
   I. Vatton - Mai 86   
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "libmsg.h"
#include "message.h"
#include "storage.h"

typedef int         L_Relations[30];

#define EXPORT extern
#include "img.var"
 /* Identification des messages */


#include "structlist_f.h"
#include "absboxes_f.h"
#include "absboxlist_f.h"
#include "fileaccess_f.h"

/* ---------------------------------------------------------------------- */
/* |    wrnumber ecrit un entier a` la console.                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         wrnumber (int i, FILE * outfile)

#else  /* __STDC__ */
static void         wrnumber (i, outfile)
int                 i;
FILE               *outfile;

#endif /* __STDC__ */

{
   fprintf (outfile, "%d", i);
}

/* ---------------------------------------------------------------------- */
/* |    wrtext ecrit du texte a` la console.                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         wrtext (char *Text, FILE * outfile)

#else  /* __STDC__ */
static void         wrtext (Text, outfile)
char               *Text;
FILE               *outfile;

#endif /* __STDC__ */

{
   fprintf (outfile, "%s", Text);
}

/* ---------------------------------------------------------------------- */
/* |    wrrep ecrit le type de repere a` la console.                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         wrrep (BoxEdge r, FILE * outfile)

#else  /* __STDC__ */
static void         wrrep (r, outfile)
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
	       fprintf (outfile, "Base");
	       break;
	    case VertRef:
	       fprintf (outfile, "Axis");
	       break;
	    case HorizMiddle:
	       fprintf (outfile, "HMiddle");
	       break;
	    case VertMiddle:
	       fprintf (outfile, "VMiddle");
	       break;
	    case NoEdge:
	       fprintf (outfile, "NoEdge");
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* |    WrPav ecrit les informations sur la boite de pAb dans le       | */
/* |            fichier outfile.                                        | */
/**CP cette procedure n'est plus static car appelee ailleurs 	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WrPav (PtrAbstractBox pAb, int Indent, FILE * outfile)

#else  /* __STDC__ */
void                WrPav (pAb, Indent, outfile)
PtrAbstractBox             pAb;
int                 Indent;
FILE               *outfile;

#endif /* __STDC__ */

{
   int                 i, j;
   PtrAbstractBox             pAbEnclosed;
   PtrTextBuffer      adbuff;
   PtrPosRelations      adpos;
   PtrDimRelations      addim;
   PtrBox            pBox;
   PtrBox            box1;
   boolean             boucle;
   PtrPosRelations      pTa1;
   BoxRelation           *pRe1;
   PtrDimRelations      pTabD1;
   PictInfo    *image;

   if (pAb->AbBox != NULL)
     {
	for (j = 1; j <= Indent; j++)
	   fprintf (outfile, " ");
	pBox = pAb->AbBox;
	wrnumber (pBox->BxAbstractBox->AbNum, outfile);
	fprintf (outfile, " ");
	wrtext (TypePave (pAb), outfile);
	fprintf (outfile, "\n");
	if (pBox != NULL)
	  {
	     for (j = 1; j <= Indent; j++)
		fprintf (outfile, " ");
	     fprintf (outfile, " Characters:");
	     wrnumber (pAb->AbVolume, outfile);
	     if (!pAb->AbHorizEnclosing)
		fprintf (outfile, " Not-Horiz-Enclosed");
	     if (!pAb->AbVertEnclosing)
		fprintf (outfile, " Not-Vert-Enclosed");
	     /* On liste les relations hors-structure */
	     if (pBox->BxXOutOfStruct)
		fprintf (outfile, " XRelation-Out-Enclosing");
	     if (pBox->BxYOutOfStruct)
		fprintf (outfile, " YRelation-Out-Enclosing");
	     if (pBox->BxWOutOfStruct)
		fprintf (outfile, " HDimRelation-Out-Enclosing");
	     if (pBox->BxHOutOfStruct)
		fprintf (outfile, " VDimRelation-Out-Enclosing");

	     if (pAb->AbLeafType == LtText)
	       {
		  fprintf (outfile, " Spaces:");
		  wrnumber (pBox->BxNSpaces, outfile);
	       }
	     fprintf (outfile, "\n");
	     for (j = 1; j <= Indent; j++)
		fprintf (outfile, " ");
	     fprintf (outfile, " Width:");
	     wrnumber (pBox->BxWidth, outfile);
	     fprintf (outfile, " Height:");
	     wrnumber (pBox->BxHeight, outfile);
	     if (pBox->BxContentWidth)
		fprintf (outfile, " Minimum-Width:");
	     else
		fprintf (outfile, " Content-Width:");
	     wrnumber (pBox->BxRuleWidth, outfile);
	     if (pBox->BxContentWidth)
		fprintf (outfile, " Minimum-Height:");
	     else
		fprintf (outfile, " Content-Height:");
	     wrnumber (pBox->BxRuleHeigth, outfile);
	     fprintf (outfile, " Base:");
	     wrnumber (pBox->BxHorizRef, outfile);
	     fprintf (outfile, " Axis:");
	     wrnumber (pBox->BxVertRef, outfile);
	     fprintf (outfile, " X:");
	     if (pBox->BxType == BoSplit)
		wrnumber (pBox->BxNexChild->BxXOrg, outfile);
	     else
		wrnumber (pBox->BxXOrg, outfile);
	     fprintf (outfile, " Y:");
	     if (pBox->BxType == BoSplit)
		wrnumber (pBox->BxNexChild->BxYOrg, outfile);
	     else
		wrnumber (pBox->BxYOrg, outfile);
	     fprintf (outfile, " Nature:");
	     switch (pAb->AbLeafType)
		   {
		      case LtCompound:
			 if (pBox->BxType == BoGhost)
			    fprintf (outfile, "BREAK-COMP");
			 else if (pAb->AbInLine)
			    fprintf (outfile, "LINES");
			 else
			    fprintf (outfile, "COMP");
			 break;
		      case LtText:
			 if (pBox->BxType == BoSplit)
			   {
			      fprintf (outfile, "BREAK-TEXT\n");
			      box1 = pBox->BxNexChild;
			      while (box1 != NULL)
				{
				   for (j = 1; j <= Indent + 6; j++)
				      fprintf (outfile, " ");
				   fprintf (outfile, "(");
				   fprintf (outfile, "Place:");
				   wrnumber (box1->BxIndChar, outfile);
				   fprintf (outfile, " Characters:");
				   wrnumber (box1->BxNChars, outfile);
				   fprintf (outfile, " Spaces:");
				   wrnumber (box1->BxNSpaces, outfile);
				   fprintf (outfile, " X:");
				   wrnumber (box1->BxXOrg, outfile);
				   fprintf (outfile, " Y:");
				   wrnumber (box1->BxYOrg, outfile);
				   fprintf (outfile, " \'");
				   adbuff = box1->BxBuffer;
				   j = box1->BxFirstChar;
				   i = box1->BxNChars;
				   if (i > 10)
				      i = 10;
				   while (i > 0)
				      /* On est en fin de buffer ? */
				     {
					while (adbuff->BuContent[j - 1] == '\0')
					  {
					     adbuff = adbuff->BuNext;
					     j = 1;
					     fprintf (outfile, "|");
					  }
					putc (adbuff->BuContent[j - 1], outfile);
					i--;
					j++;
				     }
				   fprintf (outfile, "\'");
				   fprintf (outfile, ")\n");
				   box1 = box1->BxNexChild;
				}
			      fprintf (outfile, "\n");
			   }
			 else
			    fprintf (outfile, "TEXT\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (outfile, " ");
			 fprintf (outfile, "\'");
			 adbuff = pAb->AbText;
			 j = 1;
			 i = pAb->AbVolume;
			 if (i > 60)
			    i = 60;
			 /* On est en fin de buffer ? */
			 while (i > 0)
			   {
			      while (adbuff->BuNext != NULL && adbuff->BuContent[j - 1] == '\0')
				{
				   adbuff = adbuff->BuNext;
				   j = 1;
				   fprintf (outfile, "|");
				}
			      putc (adbuff->BuContent[j - 1], outfile);
			      i--;
			      j++;
			   }
			 fprintf (outfile, "\'");
			 break;
		      case LtPicture:
			 image = (PictInfo *) pAb->AbPictInfo;
			 fprintf (outfile, "PICTURE\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (outfile, " ");
			 if (image == NULL)
			    fprintf (outfile, "PictInfo = NULL");
			 else
			    fprintf (outfile, "x = %d, y = %d, w = %d, h = %d, name = %s",
			     image->PicXArea, image->PicYArea, image->PicWArea, image->PicHArea,
				     image->PicFileName);
			 break;
		      case LtPageColBreak:
			 fprintf (outfile, "PAGE");
			 break;
		      case LtSymbol:
			 fprintf (outfile, "SYMBOL\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (outfile, " ");
			 fprintf (outfile, "\'");
			 putc (pAb->AbShape, outfile);
			 fprintf (outfile, "\'");
			 break;
		      case LtGraphics:
			 fprintf (outfile, "GRAPHICS\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (outfile, " ");
			 fprintf (outfile, "\'");
			 putc (pAb->AbShape, outfile);
			 fprintf (outfile, "\'");
			 fprintf (outfile, " printed graphics: \'");
			 putc (pAb->AbRealShape, outfile);
			 fprintf (outfile, "\'");
			 break;
		      case LtPlyLine:
			 fprintf (outfile, "POLYLINE\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (outfile, " ");
			 fprintf (outfile, "\'");
			 putc (pAb->AbPolyLineShape, outfile);
			 fprintf (outfile, "\' ");
			 for (i = 0; i < pAb->AbVolume && i < 8; i++)
			   {
			      fprintf (outfile, "%d,%d ",
				   pAb->AbPolyLineBuffer->BuPoints[i].XCoord,
				  pAb->AbPolyLineBuffer->BuPoints[i].YCoord);
			   }
			 if (i < pAb->AbVolume)
			    fprintf (outfile, "...");
			 break;
		      default:
			 break;
		   }

	     if (pBox->BxHorizFlex)
	       {
		  fprintf (outfile, ", Horiz-ELASTIC");
		  if (pBox->BxHorizInverted)
		     fprintf (outfile, ", Horiz-INVERTED");
	       }
	     if (pBox->BxVertFlex)
	       {
		  fprintf (outfile, ", Vert-ELASTIC");
		  if (pBox->BxVertInverted)
		     fprintf (outfile, ", Vert-INVERTED");
	       }

	     /* CsList des relations de position de la boite */
	     adpos = pBox->BxPosRelations;
	     while (adpos != NULL)
	       {
		  pTa1 = adpos;
		  boucle = TRUE;
		  i = 1;
		  while (boucle)
		    {
		       pRe1 = &pTa1->PosRTable[i - 1];
		       if (pRe1->ReBox == NULL)
			  boucle = FALSE;
		       else
			 {
			    fprintf (outfile, "\n");
			    for (j = 1; j <= Indent + 6; j++)
			       fprintf (outfile, " ");
			    wrrep (pRe1->ReRefEdge, outfile);
			    switch (pRe1->ReOp)
				  {
				     case OpHorizDep:
				     case OpVertDep:
					fprintf (outfile, " bound with ");
					break;
				     case OpHorizInc:
				     case OpVertInc:
					fprintf (outfile, " inside ");
					break;
				     case OpHorizRef:
					fprintf (outfile, " moves Axis of ");
					break;
				     case OpVertRef:
					fprintf (outfile, " moves Base of ");
					break;
				     case OpWidth:
					fprintf (outfile, " extends Width of ");
					break;
				     case OpHeight:
					fprintf (outfile, " extends Height of ");
					break;
				  }

			    if (pRe1->ReBox->BxAbstractBox != NULL)
			       wrnumber (pRe1->ReBox->BxAbstractBox->AbNum, outfile);
			    else
			       fprintf (outfile, "?");
			    if (i == MAX_RELAT_POS)
			       boucle = FALSE;
			    else
			       i++;
			 }
		    }
		  adpos = pTa1->PosRNext;
		  /* Bloc suivant */
	       }
	     fprintf (outfile, "\n");
	     /* CsList des dependances de largeur */
	     addim = pBox->BxWidthRelations;
	     while (addim != NULL)
	       {
		  pTabD1 = addim;
		  boucle = TRUE;
		  i = 1;
		  while (boucle)
		     if (pTabD1->DimRTable[i - 1] == NULL)
			boucle = FALSE;
		     else
		       {
			  fprintf (outfile, "\n");
			  for (j = 1; j <= Indent + 6; j++)
			     fprintf (outfile, " ");
			  fprintf (outfile, "Width changes ");
			  if (pTabD1->DimRSame[i - 1])
			     fprintf (outfile, "Width of ");
			  else
			     fprintf (outfile, "Height of ");
			  if (pTabD1->DimRTable[i - 1]->BxAbstractBox != NULL)
			     wrnumber (pTabD1->DimRTable[i - 1]->BxAbstractBox->AbNum, outfile);
			  else
			     fprintf (outfile, "?");
			  if (i == MAX_RELAT_DIM)
			     boucle = FALSE;
			  else
			     i++;
		       }
		  addim = pTabD1->DimRNext;
	       }
	     /* CsList des dependances de hauteur */
	     addim = pBox->BxHeightRelations;
	     while (addim != NULL)
	       {
		  pTabD1 = addim;
		  boucle = TRUE;
		  i = 1;
		  while (boucle)
		     if (pTabD1->DimRTable[i - 1] == NULL)
			boucle = FALSE;
		     else
		       {
			  fprintf (outfile, "\n");
			  for (j = 1; j <= Indent + 6; j++)
			     fprintf (outfile, " ");
			  fprintf (outfile, "Height changes ");
			  if (pTabD1->DimRSame[i - 1])
			     fprintf (outfile, "Height of ");
			  else
			     fprintf (outfile, "Width of ");
			  if (pTabD1->DimRTable[i - 1]->BxAbstractBox != NULL)
			     wrnumber (pTabD1->DimRTable[i - 1]->BxAbstractBox->AbNum, outfile);
			  if (i == MAX_RELAT_DIM)
			     boucle = FALSE;
			  else
			     i++;
		       }
		  addim = pTabD1->DimRNext;
		  /* Bloc suivant */
	       }
	     fprintf (outfile, "\n");
	     if (pAb->AbLeafType == LtCompound)
	       {
		  for (j = 1; j <= Indent + 6; j++)
		     fprintf (outfile, " ");
		  fprintf (outfile, "\n");
		  pAbEnclosed = pAb->AbFirstEnclosed;
		  while (pAbEnclosed != NULL)
		    {
		       WrPav (pAbEnclosed, Indent + 2, outfile);
		       pAbEnclosed = pAbEnclosed->AbNext;
		    }
	       }
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    ListeBoites cree un fichier fname qui contient la liste des     | */
/* |            relations entre boites dans la fenetre frame.           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ListeBoites (Name fname, int frame)
#else  /* __STDC__ */
void                ListeBoites (fname, frame)
Name                 fname;
int                 frame;
#endif /* __STDC__ */
{
   int                 i;
   FILE               *outfile;

   ViewFrame            *pFrame;

   pFrame = &FntrTable[frame - 1];
   if (frame != 0 && pFrame->FrAbstractBox != NULL)
      /* Une petite cuisine pour passer le nom du fichier en parametre */
     {
	/* On teste le droit d'ecriture du fichier */
	i = OuvrEcr (fname);
	if (i != 0)
	  {
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_CANNOT_WRITE), fname);
	  }
	else if (pFrame->FrAbstractBox != NULL)
	   /* On ouvre le fichier */
	  {
	     outfile = fopen (fname, "w");
	     /* On numerote les paves */
	     NumPav (pFrame->FrAbstractBox);
	     /* On ecrit les informations sur les boites crees */
	     WrPav (pFrame->FrAbstractBox, 0, outfile);
	     /* On ferme le fichier */
	     fclose (outfile);
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    StPav calcule le nombre d'entrees utilisees dans les blocs      | */
/* |            relation.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         StPav (PtrAbstractBox pAb, L_Relations TabPos, L_Relations TabDimH, L_Relations TabDimV)
#else  /* __STDC__ */
static void         StPav (pAb, TabPos, TabDimH, TabDimV)
PtrAbstractBox             pAb;
L_Relations         TabPos;
L_Relations         TabDimH;
L_Relations         TabDimV;
#endif /* __STDC__ */
{
   PtrAbstractBox             pAbEnclosed;
   int                 i, j;
   PtrPosRelations      adpos;
   PtrDimRelations      addim;
   boolean             boucle;
   PtrBox            pBo1;
   PtrPosRelations      pTa1;
   PtrDimRelations      pTabD1;

   if (pAb->AbBox != NULL)
     {
	pBo1 = pAb->AbBox;
	/* On note le nombre de relations de position de la boite */
	adpos = pBo1->BxPosRelations;
	j = 0;
	while (adpos != NULL)
	  {
	     pTa1 = adpos;
	     boucle = TRUE;
	     i = 1;
	     while (boucle)
		if (i == MAX_RELAT_POS)
		   boucle = FALSE;
		else if (pTa1->PosRTable[i - 1].ReBox == NULL)
		  {
		     boucle = FALSE;
		     i--;
		  }
		else
		   i++;
	     j += i;
	     adpos = pTa1->PosRNext;
	     /* Bloc suivant */
	  }
	if (j > 30)
	   j = 30;
	if (j > 0)
	   TabPos[j - 1]++;
	/* On note le nombre de relations de dimension horizontale de la boite */
	addim = pBo1->BxWidthRelations;
	j = 0;
	while (addim != NULL)
	  {
	     pTabD1 = addim;
	     boucle = TRUE;
	     i = 1;
	     while (boucle)
		if (i == MAX_RELAT_DIM)
		   boucle = FALSE;
		else if (pTabD1->DimRTable[i - 1] == NULL)
		  {
		     boucle = FALSE;
		     i--;
		  }
		else
		   i++;
	     j += i;
	     addim = pTabD1->DimRNext;
	     /* Bloc suivant */
	  }
	if (j > 30)
	   j = 30;
	if (j > 0)
	   TabDimH[j - 1]++;
	/* On note le nombre de relations de dimension verticale de la boite */
	addim = pBo1->BxHeightRelations;
	j = 0;
	while (addim != NULL)
	  {
	     pTabD1 = addim;
	     boucle = TRUE;
	     i = 1;
	     while (boucle)
		if (i == MAX_RELAT_DIM)
		   boucle = FALSE;
		else if (pTabD1->DimRTable[i - 1] == NULL)
		  {
		     boucle = FALSE;
		     i--;
		  }
		else
		   i++;
	     j += i;
	     addim = pTabD1->DimRNext;
	     /* Bloc suivant */
	  }
	if (j > 30)
	   j = 30;
	if (j > 0)
	   TabDimV[j - 1]++;
	/* On passe aux boites filles */
	pAbEnclosed = pAb->AbFirstEnclosed;
	while (pAbEnclosed != NULL)
	  {
	     StPav (pAbEnclosed, TabPos, TabDimH, TabDimV);
	     pAbEnclosed = pAbEnclosed->AbNext;
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    StatMedia liste dans le fichier outfile l'etat de l'allocation  | */
/* |            memoire.                                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                StatMedia (FILE * outfile)

#else  /* __STDC__ */
void                StatMedia (outfile)
FILE               *outfile;

#endif /* __STDC__ */

{
   int                 i, nb;
   L_Relations         TabPos;
   L_Relations         TabDimH;
   L_Relations         TabDimV;
   int                 total = 0;
   ViewFrame            *pFrame;


   /* On ecrit les informations sur les blocs de memoires utilises */
   fprintf (outfile, "\n");
   fprintf (outfile, "\n");
   fprintf (outfile, "Boites libres :");
   fprintf (outfile, "%5d\n", NbLibBox);
   fprintf (outfile, "Boites occupees :");
   fprintf (outfile, "%5d\n", NbOccBox);
   i = sizeof (Box);
   fprintf (outfile, "%5d", i);
   fprintf (outfile, " octets par boite, total =");
   i = i * (NbLibBox + NbOccBox);
   total += i;
   fprintf (outfile, "%10d\n", i);
   fprintf (outfile, "\n");
   fprintf (outfile, "Blocs de position libres :");
   fprintf (outfile, "%5d\n", NbLibBPos);
   fprintf (outfile, "Blocs de position occupes :");
   fprintf (outfile, "%5d\n", NbOccBPos);
   i = sizeof (PosRelations);
   fprintf (outfile, "%5d", i);
   fprintf (outfile, " octets par bloc de relations de position, total =");
   i = i * (NbLibBPos + NbOccBPos);
   total += i;
   fprintf (outfile, "%10d\n", i);
   fprintf (outfile, "\n");
   fprintf (outfile, "Blocs de dimension libres :");
   fprintf (outfile, "%5d\n", NbLibBDim);
   fprintf (outfile, "Blocs de dimension occupes :");
   fprintf (outfile, "%5d\n", NbOccBDim);
   i = sizeof (DimRelations);
   fprintf (outfile, "%5d", i);
   fprintf (outfile, " octets par bloc de relations de dimension, total =");
   i = i * (NbLibBPos + NbOccBPos);
   total += i;
   fprintf (outfile, "%10d\n", i);
   fprintf (outfile, "\n");
   fprintf (outfile, "Lignes libres :");
   fprintf (outfile, "%5d\n", NbLibLine);
   fprintf (outfile, "Lignes occupees :");
   fprintf (outfile, "%5d\n", NbOccLine);
   i = sizeof (Line);
   fprintf (outfile, "%5d", i);
   fprintf (outfile, " octets par ligne, total =");
   i = i * (NbLibLine + NbOccLine);
   total += i;
   fprintf (outfile, "%10d\n", i);
   /* Evaluation du nombre d'entrees des blocs utilises */
   for (i = 1; i <= 30; i++)
     {
	TabPos[i - 1] = 0;
	TabDimH[i - 1] = 0;
	TabDimV[i - 1] = 0;
     }
   for (i = 1; i <= MAX_FRAME; i++)
     {
	pFrame = &FntrTable[i - 1];
	if (pFrame->FrAbstractBox != NULL)
	   StPav (pFrame->FrAbstractBox, TabPos, TabDimH, TabDimV);
     }
   /* Evaluation des pourcentages de nombres d'entrees utilisees */
   fprintf (outfile, "\n");
   fprintf (outfile, "Pourcentage de relations de position\n");
   for (i = 1; i <= 30; i++)
      if (TabPos[i - 1] != 0)
	{
	   nb = TabPos[i - 1] * 100 / NbOccBox;
	   fprintf (outfile, "%2d->", i);
	   fprintf (outfile, "%3d%%  ", nb);
	}
   fprintf (outfile, "\n");
   fprintf (outfile, "Pourcentage de relations de largeur\n");
   for (i = 1; i <= 30; i++)
      if (TabDimH[i - 1] != 0)
	{
	   nb = TabDimH[i - 1] * 100 / NbOccBox;
	   fprintf (outfile, "%2d->", i);
	   fprintf (outfile, "%3d%%  ", nb);
	}
   fprintf (outfile, "\n");
   fprintf (outfile, "Pourcentage de relations de hauteur\n");
   for (i = 1; i <= 30; i++)
      if (TabDimV[i - 1] != 0)
	{
	   nb = TabDimV[i - 1] * 100 / NbOccBox;
	   fprintf (outfile, "%2d->", i);
	   fprintf (outfile, "%3d%%  ", nb);
	}
   fprintf (outfile, "\n");
   fprintf (outfile, "	Total pour le Mediateur =%10d", total);
   fprintf (outfile, " octets\n");
}
/* End Of Module liste */
