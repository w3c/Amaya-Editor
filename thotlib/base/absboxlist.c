
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   liste.c : module de trace des e'tats du Mediateur.
   I. Vatton - Mai 86   
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "imagedrvr.h"
#include "libmsg.h"
#include "message.h"
#include "storage.h"

typedef int         L_Relations[30];

#define EXPORT extern
#include "img.var"
 /* Identification des messages */


#include "debug.f"
#include "imabs.f"
#include "liste.f"
#include "storage.f"

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
/* |    WrPav ecrit les informations sur la boite de pPav dans le       | */
/* |            fichier outfile.                                        | */
/**CP cette procedure n'est plus static car appelee ailleurs 	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WrPav (PtrAbstractBox pPav, int Indent, FILE * outfile)

#else  /* __STDC__ */
void                WrPav (pPav, Indent, outfile)
PtrAbstractBox             pPav;
int                 Indent;
FILE               *outfile;

#endif /* __STDC__ */

{
   int                 i, j;
   PtrAbstractBox             adpave;
   PtrTextBuffer      adbuff;
   PtrPosRelations      adpos;
   PtrDimRelations      addim;
   PtrBox            ibox;
   PtrBox            box1;
   boolean             boucle;
   PtrPosRelations      pTa1;
   BoxRelation           *pRe1;
   PtrDimRelations      pTabD1;
   ImageDescriptor    *image;

   if (pPav->AbBox != NULL)
     {
	for (j = 1; j <= Indent; j++)
	   fprintf (outfile, " ");
	ibox = pPav->AbBox;
	wrnumber (ibox->BxAbstractBox->AbNum, outfile);
	fprintf (outfile, " ");
	wrtext (TypePave (pPav), outfile);
	fprintf (outfile, "\n");
	if (ibox != NULL)
	  {
	     for (j = 1; j <= Indent; j++)
		fprintf (outfile, " ");
	     fprintf (outfile, " Characters:");
	     wrnumber (pPav->AbVolume, outfile);
	     if (!pPav->AbHorizEnclosing)
		fprintf (outfile, " Not-Horiz-Enclosed");
	     if (!pPav->AbVertEnclosing)
		fprintf (outfile, " Not-Vert-Enclosed");
	     /* On liste les relations hors-structure */
	     if (ibox->BxXOutOfStruct)
		fprintf (outfile, " XRelation-Out-Enclosing");
	     if (ibox->BxYOutOfStruct)
		fprintf (outfile, " YRelation-Out-Enclosing");
	     if (ibox->BxWOutOfStruct)
		fprintf (outfile, " HDimRelation-Out-Enclosing");
	     if (ibox->BxHOutOfStruct)
		fprintf (outfile, " VDimRelation-Out-Enclosing");

	     if (pPav->AbLeafType == LtText)
	       {
		  fprintf (outfile, " Spaces:");
		  wrnumber (ibox->BxNSpaces, outfile);
	       }
	     fprintf (outfile, "\n");
	     for (j = 1; j <= Indent; j++)
		fprintf (outfile, " ");
	     fprintf (outfile, " Width:");
	     wrnumber (ibox->BxWidth, outfile);
	     fprintf (outfile, " Height:");
	     wrnumber (ibox->BxHeight, outfile);
	     if (ibox->BxContentWidth)
		fprintf (outfile, " Minimum-Width:");
	     else
		fprintf (outfile, " Content-Width:");
	     wrnumber (ibox->BxRuleWidth, outfile);
	     if (ibox->BxContentWidth)
		fprintf (outfile, " Minimum-Height:");
	     else
		fprintf (outfile, " Content-Height:");
	     wrnumber (ibox->BxRuleHeigth, outfile);
	     fprintf (outfile, " Base:");
	     wrnumber (ibox->BxHorizRef, outfile);
	     fprintf (outfile, " Axis:");
	     wrnumber (ibox->BxVertRef, outfile);
	     fprintf (outfile, " X:");
	     if (ibox->BxType == BoSplit)
		wrnumber (ibox->BxNexChild->BxXOrg, outfile);
	     else
		wrnumber (ibox->BxXOrg, outfile);
	     fprintf (outfile, " Y:");
	     if (ibox->BxType == BoSplit)
		wrnumber (ibox->BxNexChild->BxYOrg, outfile);
	     else
		wrnumber (ibox->BxYOrg, outfile);
	     fprintf (outfile, " Nature:");
	     switch (pPav->AbLeafType)
		   {
		      case LtCompound:
			 if (ibox->BxType == BoGhost)
			    fprintf (outfile, "BREAK-COMP");
			 else if (pPav->AbInLine)
			    fprintf (outfile, "LINES");
			 else
			    fprintf (outfile, "COMP");
			 break;
		      case LtText:
			 if (ibox->BxType == BoSplit)
			   {
			      fprintf (outfile, "BREAK-TEXT\n");
			      box1 = ibox->BxNexChild;
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
			 adbuff = pPav->AbText;
			 j = 1;
			 i = pPav->AbVolume;
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
			 image = (ImageDescriptor *) pPav->AbImageDescriptor;
			 fprintf (outfile, "PICTURE\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (outfile, " ");
			 if (image == NULL)
			    fprintf (outfile, "ImageDescriptor = NULL");
			 else
			    fprintf (outfile, "x = %d, y = %d, w = %d, h = %d, name = %s",
			     image->xcf, image->ycf, image->wcf, image->hcf,
				     image->imageFileName);
			 break;
		      case LtPageColBreak:
			 fprintf (outfile, "PAGE");
			 break;
		      case LtSymbol:
			 fprintf (outfile, "SYMBOL\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (outfile, " ");
			 fprintf (outfile, "\'");
			 putc (pPav->AbShape, outfile);
			 fprintf (outfile, "\'");
			 break;
		      case LtGraphics:
			 fprintf (outfile, "GRAPHICS\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (outfile, " ");
			 fprintf (outfile, "\'");
			 putc (pPav->AbShape, outfile);
			 fprintf (outfile, "\'");
			 fprintf (outfile, " printed graphics: \'");
			 putc (pPav->AbRealShape, outfile);
			 fprintf (outfile, "\'");
			 break;
		      case LtPlyLine:
			 fprintf (outfile, "POLYLINE\n");
			 for (j = 1; j <= Indent + 6; j++)
			    fprintf (outfile, " ");
			 fprintf (outfile, "\'");
			 putc (pPav->AbPolyLineShape, outfile);
			 fprintf (outfile, "\' ");
			 for (i = 0; i < pPav->AbVolume && i < 8; i++)
			   {
			      fprintf (outfile, "%d,%d ",
				   pPav->AbPolyLineBuffer->BuPoints[i].XCoord,
				  pPav->AbPolyLineBuffer->BuPoints[i].YCoord);
			   }
			 if (i < pPav->AbVolume)
			    fprintf (outfile, "...");
			 break;
		      default:
			 break;
		   }

	     if (ibox->BxHorizFlex)
	       {
		  fprintf (outfile, ", Horiz-ELASTIC");
		  if (ibox->BxHorizInverted)
		     fprintf (outfile, ", Horiz-INVERTED");
	       }
	     if (ibox->BxVertFlex)
	       {
		  fprintf (outfile, ", Vert-ELASTIC");
		  if (ibox->BxVertInverted)
		     fprintf (outfile, ", Vert-INVERTED");
	       }

	     /* CsList des relations de position de la boite */
	     adpos = ibox->BxPosRelations;
	     while (adpos != NULL)
	       {
		  pTa1 = adpos;
		  boucle = True;
		  i = 1;
		  while (boucle)
		    {
		       pRe1 = &pTa1->PosRTable[i - 1];
		       if (pRe1->ReBox == NULL)
			  boucle = False;
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
			       boucle = False;
			    else
			       i++;
			 }
		    }
		  adpos = pTa1->PosRNext;
		  /* Bloc suivant */
	       }
	     fprintf (outfile, "\n");
	     /* CsList des dependances de largeur */
	     addim = ibox->BxWidthRelations;
	     while (addim != NULL)
	       {
		  pTabD1 = addim;
		  boucle = True;
		  i = 1;
		  while (boucle)
		     if (pTabD1->DimRTable[i - 1] == NULL)
			boucle = False;
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
			     boucle = False;
			  else
			     i++;
		       }
		  addim = pTabD1->DimRNext;
	       }
	     /* CsList des dependances de hauteur */
	     addim = ibox->BxHeightRelations;
	     while (addim != NULL)
	       {
		  pTabD1 = addim;
		  boucle = True;
		  i = 1;
		  while (boucle)
		     if (pTabD1->DimRTable[i - 1] == NULL)
			boucle = False;
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
			     boucle = False;
			  else
			     i++;
		       }
		  addim = pTabD1->DimRNext;
		  /* Bloc suivant */
	       }
	     fprintf (outfile, "\n");
	     if (pPav->AbLeafType == LtCompound)
	       {
		  for (j = 1; j <= Indent + 6; j++)
		     fprintf (outfile, " ");
		  fprintf (outfile, "\n");
		  adpave = pPav->AbFirstEnclosed;
		  while (adpave != NULL)
		    {
		       WrPav (adpave, Indent + 2, outfile);
		       adpave = adpave->AbNext;
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

   ViewFrame            *pFe1;

   pFe1 = &FntrTable[frame - 1];
   if (frame != 0 && pFe1->FrAbstractBox != NULL)
      /* Une petite cuisine pour passer le nom du fichier en parametre */
     {
	/* On teste le droit d'ecriture du fichier */
	i = OuvrEcr (fname);
	if (i != 0)
	  {
	     TtaDisplaySimpleMessageString (LIB, INFO, LIB_CANNOT_WRITE, fname);
	  }
	else if (pFe1->FrAbstractBox != NULL)
	   /* On ouvre le fichier */
	  {
	     outfile = fopen (fname, "w");
	     /* On numerote les paves */
	     NumPav (pFe1->FrAbstractBox);
	     /* On ecrit les informations sur les boites crees */
	     WrPav (pFe1->FrAbstractBox, 0, outfile);
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
static void         StPav (PtrAbstractBox pPav, L_Relations TabPos, L_Relations TabDimH, L_Relations TabDimV)

#else  /* __STDC__ */
static void         StPav (pPav, TabPos, TabDimH, TabDimV)
PtrAbstractBox             pPav;
L_Relations         TabPos;
L_Relations         TabDimH;
L_Relations         TabDimV;

#endif /* __STDC__ */

{
   PtrAbstractBox             adpave;
   int                 i, j;
   PtrPosRelations      adpos;
   PtrDimRelations      addim;
   boolean             boucle;
   PtrBox            pBo1;
   PtrPosRelations      pTa1;
   PtrDimRelations      pTabD1;

   if (pPav->AbBox != NULL)
     {
	pBo1 = pPav->AbBox;
	/* On note le nombre de relations de position de la boite */
	adpos = pBo1->BxPosRelations;
	j = 0;
	while (adpos != NULL)
	  {
	     pTa1 = adpos;
	     boucle = True;
	     i = 1;
	     while (boucle)
		if (i == MAX_RELAT_POS)
		   boucle = False;
		else if (pTa1->PosRTable[i - 1].ReBox == NULL)
		  {
		     boucle = False;
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
	     boucle = True;
	     i = 1;
	     while (boucle)
		if (i == MAX_RELAT_DIM)
		   boucle = False;
		else if (pTabD1->DimRTable[i - 1] == NULL)
		  {
		     boucle = False;
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
	     boucle = True;
	     i = 1;
	     while (boucle)
		if (i == MAX_RELAT_DIM)
		   boucle = False;
		else if (pTabD1->DimRTable[i - 1] == NULL)
		  {
		     boucle = False;
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
	adpave = pPav->AbFirstEnclosed;
	while (adpave != NULL)
	  {
	     StPav (adpave, TabPos, TabDimH, TabDimV);
	     adpave = adpave->AbNext;
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
   ViewFrame            *pFe1;


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
	pFe1 = &FntrTable[i - 1];
	if (pFe1->FrAbstractBox != NULL)
	   StPav (pFe1->FrAbstractBox, TabPos, TabDimH, TabDimV);
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
