
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   dep.c : gestion des deplacements des boites
   France Logiciel numero de depot 88-39-001-00

   I. Vatton - Novembre 84
   IV : Aout 92 dimensions minimales
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#define EXPORT extern
#include "img.var"

#include "dep.f"
#include "es.f"
#include "fen.f"
#include "font.f"
#include "img.f"
#include "lig.f"
#include "memory.f"
#include "rel.f"


/* ---------------------------------------------------------------------- */
/* |        Parent retourne vrai si pBox est une englobante de la boite | */
/* |                cebox.                                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             Parent (PtrBox pBox, PtrBox cebox)
#else  /* __STDC__ */
boolean             Parent (pBox, cebox)
PtrBox            pBox;
PtrBox            cebox;

#endif /* __STDC__ */
{
   PtrAbstractBox             pave;
   boolean             egal;
   boolean             parent;

   if (cebox == NULL || pBox == NULL)
      parent = FALSE;
   else
     {
	/* Recherche dans la parente de cebox y compris elle-meme */
	pave = cebox->BxAbstractBox;
	egal = FALSE;
	while (!egal && pave != NULL)
	  {
	     egal = pave->AbBox == pBox;
	     pave = pave->AbEnclosing;
	  }
	parent = egal;
     }
   return parent;
}

/* ---------------------------------------------------------------------- */
/* |        Soeur retourne vrai si pBox a le meme pave pere que la      | */
/* |                boite cebox et n'est pas la boite pBox.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      Soeur (PtrBox pBox, PtrBox cebox)
#else  /* __STDC__ */
static boolean      Soeur (pBox, cebox)
PtrBox            pBox;
PtrBox            cebox;

#endif /* __STDC__ */
{

   if (cebox == pBox)
      return (FALSE);
   else if (cebox == NULL)
      return (FALSE);
   else if (pBox == NULL)
      return (FALSE);
   else if (cebox->BxAbstractBox->AbEnclosing == pBox->BxAbstractBox->AbEnclosing)
      return (TRUE);
   else
      return (FALSE);
}


/* ---------------------------------------------------------------------- */
/* |        XEnAbsolu retourne vrai si on est pas dans une cre'ation    | */
/* |            initiale ou si une boi^te englobante de la boi^te pBox  | */
/* |            est e'lastique horizontalement ou si elle a un          | */
/* |            positionnement horizontal hors-structure.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             XEnAbsolu (PtrBox pBox)
#else  /* __STDC__ */
boolean             XEnAbsolu (pBox)
PtrBox            pBox;

#endif /* __STDC__ */
{
   PtrBox            box1;
   boolean             Ok;

   box1 = pBox;
   Ok = (Propage != ToSiblings);
   while (!Ok && box1 != NULL)
     {
	Ok = (box1->BxHorizFlex || box1->BxXOutOfStruct);
	/* Remonte a la boite englobante */
	if (box1->BxAbstractBox->AbEnclosing == NULL)
	   box1 = NULL;
	else
	   box1 = box1->BxAbstractBox->AbEnclosing->AbBox;
     }
   return (Ok);
}


/* ---------------------------------------------------------------------- */
/* |        YEnAbsolu retourne vrai si on est pas dans une cre'ation    | */
/* |            initiale ou si une boi^te englobante de la boi^te pBox  | */
/* |            est e'lastique verticalement ou si elle a un            | */
/* |            positionnement vertical hors-structure.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             YEnAbsolu (PtrBox pBox)
#else  /* __STDC__ */
boolean             YEnAbsolu (pBox)
PtrBox            pBox;

#endif /* __STDC__ */
{
   PtrBox            box1;
   boolean             Ok;

   box1 = pBox;
   Ok = (Propage != ToSiblings);
   while (!Ok && box1 != NULL)
     {
	Ok = (box1->BxVertFlex || box1->BxYOutOfStruct);
	/* Remonte a la boite englobante */
	if (box1->BxAbstractBox->AbEnclosing == NULL)
	   box1 = NULL;
	else
	   box1 = box1->BxAbstractBox->AbEnclosing->AbBox;
     }
   return (Ok);
}


/* ---------------------------------------------------------------------- */
/* |        XYEnAbsolu retourne les indicateurs de placement absolu     | */
/* |            EnHorizontal et EnVertical pour la boi^te pBox.         | */
/* |            L'indicateur est vrai si on est pas dans une cre'ation  | */
/* |            initiale ou si une boi^te englobante de la boi^te pBox  | */
/* |            est e'lastique horizontalement (verticalement) ou si    | */
/* |            une boi^te englobante a une relation hors-structure.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                XYEnAbsolu (PtrBox pBox, boolean * EnHorizontal, boolean * EnVertical)
#else  /* __STDC__ */
void                XYEnAbsolu (pBox, EnHorizontal, EnVertical)
PtrBox            pBox;
boolean            *EnHorizontal;
boolean            *EnVertical;

#endif /* __STDC__ */
{
   PtrBox            box1;

   box1 = pBox;
   *EnVertical = (Propage != ToSiblings);
   *EnHorizontal = *EnVertical;
   while ((!*EnVertical || !*EnHorizontal) && box1 != NULL)
     {
	if (!*EnVertical)
	   *EnVertical = (box1->BxVertFlex || box1->BxYOutOfStruct);
	if (!*EnHorizontal)
	   *EnHorizontal = (box1->BxHorizFlex || box1->BxXOutOfStruct);
	/* Remonte a la boite englobante */
	if (box1->BxAbstractBox->AbEnclosing == NULL)
	   box1 = NULL;
	else
	   box1 = box1->BxAbstractBox->AbEnclosing->AbBox;
     }

   if (pBox->BxType == BoBlock && pBox->BxYToCompute)
      /* C'est trop tot pour placer le contenu du bloc de lignes en Y */
      *EnVertical = FALSE;
}


/* ---------------------------------------------------------------------- */
/* |    PolyInverse inverse les points de la polyline.                  | */
/* |            Les parame`tres InversionHoriz et InversionVert         | */
/* |            indiquent si la boi^te e'lastique a` l'origine du       | */
/* |            traitement est inverse'e dans chacun des sens.          | */
/* |            Le parame`tre Saisie est Vrai quand l'inversion doit    | */
/* |            s'appliquer aux repe`res du pave et non de la boite.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PolyInverse (PtrAbstractBox pAb, boolean InversionHoriz, boolean InversionVert, boolean Saisie)
#else  /* __STDC__ */
static void         PolyInverse (pAb, InversionHoriz, InversionVert, Saisie)
PtrAbstractBox             pAb;
boolean             InversionHoriz;
boolean             InversionVert;
boolean             Saisie;

#endif /* __STDC__ */
{
   PtrTextBuffer      adbuffs, adbuffd;
   PtrBox            pBox;
   int                 maxX, maxY;
   int                 val, i, j;
   float               rapportX, rapportY;
   int                 lepoint;

   pBox = pAb->AbBox;
   j = 1;
   if (Saisie)
     {
	/* maj des reperes du pave a partir de la boite */
	adbuffs = pBox->BxBuffer;
	adbuffd = pAb->AbPolyLineBuffer;
	rapportX = pBox->BxXRatio;
	rapportY = pBox->BxYRation;
     }
   else
     {
	/* maj des reperes de la boite a partir du pave */
	adbuffs = pAb->AbPolyLineBuffer;
	adbuffd = pBox->BxBuffer;
	/* annule les deformations precedentes */
	rapportX = 1;
	rapportY = 1;
	pBox->BxXRatio = 1;
	pBox->BxYRation = 1;
	adbuffd->BuPoints[0].XCoord = adbuffs->BuPoints[0].XCoord;
	adbuffd->BuPoints[0].YCoord = adbuffs->BuPoints[0].YCoord;
     }

   /* inversions eventuelles par rapport aux milieux */
   maxX = adbuffs->BuPoints[0].XCoord;
   maxY = adbuffs->BuPoints[0].YCoord;
   val = pBox->BxNChars;
   for (i = 1; i < val; i++)
     {
	if (j >= adbuffs->BuLength)
	  {
	     if (adbuffs->BuNext != NULL)
	       {
		  /* Changement de buffer */
		  adbuffs = adbuffs->BuNext;
		  adbuffd = adbuffd->BuNext;
		  j = 0;
	       }
	  }

	lepoint = (int) ((float) adbuffs->BuPoints[j].XCoord / rapportX);
	if (InversionHoriz)
	   /* inversion des points en x */
	   adbuffd->BuPoints[j].XCoord = maxX - lepoint;
	else
	   adbuffd->BuPoints[j].XCoord = lepoint;

	lepoint = (int) ((float) adbuffs->BuPoints[j].YCoord / rapportY);
	if (InversionVert)
	   /* inversion des points en y */
	   adbuffd->BuPoints[j].YCoord = maxY - lepoint;
	else
	   adbuffd->BuPoints[j].YCoord = lepoint;

	j++;
     }

   if (pBox->BxPictInfo != NULL && !Saisie)
     {
	/* le calcul des points de controle doit etre reexecute */
	free ((char *) pBox->BxPictInfo);
	pBox->BxPictInfo = NULL;
     }
}


/* ---------------------------------------------------------------------- */
/* |    AjusteTrace ajuste les trace's graphiques en fonction des       | */
/* |            inversions des boi^tes e'lastiques.                     | */
/* |            Si le pave' passe' en parame`tre est un pave' compose'  | */
/* |            la demande de conversion est transmise aux pave's fils. | */
/* |            Les parame`tres InversionHoriz et InversionVert         | */
/* |            indiquent si la boi^te e'lastique a` l'origine du       | */
/* |            traitement est inverse'e dans chacun des sens.          | */
/* |            Le parame`tre Saisie est Vrai quand le caracte`re de    | */
/* |            re'fe'rence correspond au caracte`re saisi (TraceReel)  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                AjusteTrace (PtrAbstractBox pAb, boolean InversionHoriz, boolean InversionVert, boolean Saisie)
#else  /* __STDC__ */
void                AjusteTrace (pAb, InversionHoriz, InversionVert, Saisie)
PtrAbstractBox             pAb;
boolean             InversionHoriz;
boolean             InversionVert;
boolean             Saisie;

#endif /* __STDC__ */
{
   PtrAbstractBox             pavefils;
   char                car;

   if (pAb->AbLeafType == LtCompound && !Saisie)
     {
	/* Transmet le traitement aux paves fils */
	pavefils = pAb->AbFirstEnclosed;
	while (pavefils != NULL)
	  {
	     if (pavefils->AbLeafType == LtGraphics
		 || pavefils->AbLeafType == LtPlyLine
		 || pavefils->AbLeafType == LtCompound)
		AjusteTrace (pavefils, InversionHoriz, InversionVert, Saisie);
	     pavefils = pavefils->AbNext;
	  }
     }
   else if (pAb->AbLeafType == LtPlyLine)
      PolyInverse (pAb, InversionHoriz, InversionVert, Saisie);
   else if (pAb->AbLeafType == LtGraphics)
     {
	/* Ajuste le graphique de la boite */
	if (Saisie)
	   /* le caractere de reference est le trace reel */
	   car = pAb->AbRealShape;
	else
	   /* le caractere de reference est le trace de l'element */
	   car = pAb->AbShape;

	if (InversionHoriz)
	   switch (car)
		 {
		    case '\\':
		       car = '/';
		       break;
		    case '/':
		       car = '\\';
		       break;
		    case '>':
		       car = '<';
		       break;
		    case '<':
		       car = '>';
		       break;
		    case 'l':
		       car = 'r';
		       break;
		    case 'r':
		       car = 'l';
		       break;
		    case 'o':
		       car = 'e';
		       break;
		    case 'e':
		       car = 'o';
		       break;
		    case 'O':
		       car = 'E';
		       break;
		    case 'E':
		       car = 'O';
		       break;
		    case 'W':
		       car = 'Z';
		       break;
		    case 'Z':
		       car = 'W';
		       break;
		    case 'X':
		       car = 'Y';
		       break;
		    case 'Y':
		       car = 'X';
		       break;
		    default:
		       break;
		 }

	if (InversionVert)
	   switch (car)
		 {
		    case '\\':
		       car = '/';
		       break;
		    case '/':
		       car = '\\';
		       break;
		    case '^':
		       car = 'V';
		       break;
		    case 'V':
		       car = '^';
		       break;
		    case 't':
		       car = 'b';
		       break;
		    case 'b':
		       car = 't';
		       break;
		    case 'o':
		       car = 'O';
		       break;
		    case 'O':
		       car = 'o';
		       break;
		    case 'e':
		       car = 'E';
		       break;
		    case 'E':
		       car = 'e';
		       break;
		    case 'W':
		       car = 'X';
		       break;
		    case 'Z':
		       car = 'Y';
		       break;
		    case 'X':
		       car = 'W';
		       break;
		    case 'Y':
		       car = 'Z';
		       break;
		    default:
		       break;
		 }

	/* Ajuste le graphique de la boite */
	if (Saisie)
	   /* le caractere de reference est le trace reel */
	   pAb->AbShape = car;
	else
	   /* le caractere de reference est le trace de l'element */
	   pAb->AbRealShape = car;
     }
}


/* ---------------------------------------------------------------------- */
/* |        HorizInverse inverse les reperes horizontaux position et    | */
/* |            dimension de la boite elastique pBox. Si la boite est   | */
/* |            une boite graphique les dessins sont modifies.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                HorizInverse (PtrBox pBox, OpRelation op)
#else  /* __STDC__ */
void                HorizInverse (pBox, op)
PtrBox            pBox;
OpRelation          op;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb;
   PtrPosRelations      adpos;
   int                 i;
   BoxEdge         rp1;
   BoxEdge         rp2;
   BoxEdge         rd1;
   BoxEdge         rd2;
   PtrBox            boxpos;

   pAb = pBox->BxAbstractBox;

   /* On prend le repere symetrique dans le positionnement */
   rp1 = pAb->AbHorizPos.PosEdge;	/* Ancien repere de position */
   rp2 = rp1;			/* Nouveau repere de position */
   rd1 = rp1;			/* Ancien repere de dimension */
   rd2 = rp1;			/* Nouveau repere de dimension */
   if (rp1 == Left)
     {
	rp2 = Right;
	pAb->AbHorizPos.PosEdge = rp2;
	rd1 = Right;
	rd2 = Left;
	pAb->AbWidth.DimPosition.PosEdge = rd2;
     }
   else if (rp1 == Right)
     {
	rp2 = Left;
	pAb->AbHorizPos.PosEdge = rp2;
	rd1 = Left;
	rd2 = Right;
	pAb->AbWidth.DimPosition.PosEdge = rd2;
     }

   /* Mise a jour du point fixe de la boite */
   if (op == OpWidth && pBox->BxHorizEdge == rp1)
      pBox->BxHorizEdge = rp2;
   else if (op == OpHorizDep && pBox->BxHorizEdge == rd1)
      pBox->BxHorizEdge = rd2;

   /* Les reperes sont inverses */
   pBox->BxHorizInverted = !pBox->BxHorizInverted;

   /* Modifie enventuellement le dessin de la boite */
   if (rp1 != rp2)
      AjusteTrace (pAb, pBox->BxHorizInverted, pBox->BxVertInverted, FALSE);

   /* Note la boite qui fixe la position de la boite elastique */
   if (pAb->AbHorizPos.PosAbRef == NULL)
      boxpos = NULL;
   else
      boxpos = pAb->AbHorizPos.PosAbRef->AbBox;

   /* Met a jour les relations de la boite */
   i = 1;
   adpos = pBox->BxPosRelations;
   while (adpos != NULL)
     {
	if (adpos->PosRTable[i - 1].ReBox == NULL)
	  {
	     adpos = adpos->PosRNext;
	     i = 0;
	  }
	/* Est-ce une relation avec le repere de position ? */
	else if (adpos->PosRTable[i - 1].ReRefEdge == rp1
		 && adpos->PosRTable[i - 1].ReBox == boxpos
		 && adpos->PosRTable[i - 1].ReOp != OpHorizRef)
	   adpos->PosRTable[i - 1].ReRefEdge = rp2;
	i++;
     }
}


/* ---------------------------------------------------------------------- */
/* |    VertInverse inverse les reperes verticaux position et dimension | */
/* |            de la boite elastique pBox. Si la boite est une boite   | */
/* |            graphique, les dessins sont modifies.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                VertInverse (PtrBox pBox, OpRelation op)
#else  /* __STDC__ */
void                VertInverse (pBox, op)
PtrBox            pBox;
OpRelation          op;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb;
   PtrPosRelations      adpos;
   int                 i;
   BoxEdge         rp1;
   BoxEdge         rp2;
   BoxEdge         rd1;
   BoxEdge         rd2;
   PtrBox            boxpos;

   pAb = pBox->BxAbstractBox;

   /* On prend le repere symetrique dans le positionnement */
   rp1 = pAb->AbVertPos.PosEdge;
   rp2 = rp1;			/* Nouveau repere de position */
   rd1 = rp1;			/* Ancien repere de dimension */
   rd2 = rp1;			/* Nouveau repere de dimension */
   if (rp1 == Top)
     {
	rp2 = Bottom;
	pAb->AbVertPos.PosEdge = rp2;
	rd1 = Bottom;
	rd2 = Top;
	pAb->AbHeight.DimPosition.PosEdge = rd2;
     }
   else if (rp1 == Bottom)
     {
	rp2 = Top;
	pAb->AbVertPos.PosEdge = rp2;
	rd1 = Top;
	rd2 = Bottom;
	pAb->AbHeight.DimPosition.PosEdge = rd2;
     }

   /* Mise a jour du point fixe de la boite */
   if (op == OpHeight && pBox->BxVertEdge == rp1)
      pBox->BxVertEdge = rp2;
   else if (op == OpVertDep && pBox->BxVertEdge == rd1)
      pBox->BxVertEdge = rd2;

   /* Les reperes sont inverses */
   pBox->BxVertInverted = !pBox->BxVertInverted;

   /* Modifie enventuellement le dessin de la boite */
   if (rp1 != rp2)
/**PL*/ AjusteTrace (pAb, pBox->BxHorizInverted, pBox->BxVertInverted, FALSE);

   /* Note la boite qui fixe la position de la boite elastique */
   if (pAb->AbVertPos.PosAbRef == NULL)
      boxpos = NULL;
   else
      boxpos = pAb->AbVertPos.PosAbRef->AbBox;

   /* Met a jour les relations de la boite */
   i = 1;
   adpos = pBox->BxPosRelations;
   while (adpos != NULL)
     {
	if (adpos->PosRTable[i - 1].ReBox == NULL)
	  {
	     adpos = adpos->PosRNext;
	     i = 0;
	  }
	/* Est-ce une relation avec le repere de position ? */
	else if (adpos->PosRTable[i - 1].ReRefEdge == rp1
		 && adpos->PosRTable[i - 1].ReBox == boxpos
		 && adpos->PosRTable[i - 1].ReOp != OpVertRef)
	   adpos->PosRTable[i - 1].ReRefEdge = rp2;
	i++;
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChangeLgContenu met a` jour la largeur du contenu de la boi^te. | */
/* |            Cette proce'dure ve'rifie que la re`gle du minimum est  | */
/* |            respecte'e. Eventuellement e'change largeur re'elle et  | */
/* |            autre largeur.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangeLgContenu (PtrBox pBox, PtrBox org, int large, int dbl, int frame)
#else  /* __STDC__ */
void                ChangeLgContenu (pBox, org, large, dbl, frame)
PtrBox            pBox;
PtrBox            org;
int                 large;
int                 dbl;
int                 frame;

#endif /* __STDC__ */
{
   int                 delta;

   if (pBox != NULL)
     {
	/* Regarde si la largeur reelle actuelle depend du contenu */
	if (pBox->BxContentWidth)
	  {
	     /* La largeur reelle est egale a la largeur du contenu */
	     if (large < pBox->BxRuleWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  delta = pBox->BxRuleWidth - pBox->BxWidth;
		  pBox->BxRuleWidth = large;
		  pBox->BxContentWidth = !pBox->BxContentWidth;
		  if (delta != 0)
		     ModLarg (pBox, org, NULL, delta, dbl, frame);
	       }
	     else
		/* Mise a jour de la largeur du contenu */
		ModLarg (pBox, org, NULL, large - pBox->BxWidth, dbl, frame);
	  }
	else if (!pBox->BxAbstractBox->AbWidth.DimIsPosition && pBox->BxAbstractBox->AbWidth.DimMinimum)
	  {
	     /* La largeur reelle est egale au minimum */
	     if (large > pBox->BxWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  pBox->BxRuleWidth = pBox->BxWidth;
		  pBox->BxContentWidth = !pBox->BxContentWidth;
		  ModLarg (pBox, org, NULL, large - pBox->BxWidth, dbl, frame);
	       }
	     else
		/* Mise a jour de la largeur du contenu */
		pBox->BxRuleWidth = large;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChangeHtContenu met a` jour la largeur du contenu de la boi^te. | */
/* |            Cette proce'dure ve'rifie que la re`gle du minimum est  | */
/* |            respecte'e. Eventuellement e'change hauteur re'elle et  | */
/* |            autre hauteur.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangeHtContenu (PtrBox pBox, PtrBox org, int haut, int frame)
#else  /* __STDC__ */
void                ChangeHtContenu (pBox, org, haut, frame)
PtrBox            pBox;
PtrBox            org;
int                 haut;
int                 frame;

#endif /* __STDC__ */
{
   int                 delta;

   if (pBox != NULL)
     {
	/* Regarde si la hauteur reelle actuelle depend du contenu */
	if (pBox->BxContentHeight)
	  {
	     /* La hauteur reelle est egale a la hauteur du contenu */
	     if (haut < pBox->BxRuleHeigth)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  delta = pBox->BxRuleHeigth - pBox->BxHeight;
		  pBox->BxRuleHeigth = haut;
		  pBox->BxContentHeight = !pBox->BxContentHeight;
		  ModHaut (pBox, org, NULL, delta, frame);
	       }
	     else
		/* Mise a jour de la hauteur du contenu */
		ModHaut (pBox, org, NULL, haut - pBox->BxHeight, frame);
	  }
	else if (!pBox->BxAbstractBox->AbHeight.DimIsPosition && pBox->BxAbstractBox->AbHeight.DimMinimum)
	  {
	     /* La hauteur reelle est egale au minimum */
	     if (haut > pBox->BxHeight)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  pBox->BxRuleHeigth = pBox->BxHeight;
		  pBox->BxContentHeight = !pBox->BxContentHeight;
		  ModHaut (pBox, org, NULL, haut - pBox->BxHeight, frame);
	       }
	     else
		/* Mise a jour de la hauteur du contenu */
		pBox->BxRuleHeigth = haut;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |        ChangeLargeur propage la modification sur la largeur de la  | */
/* |            boi^te pBox. Cette proce'dure ve'rifie la re`gle du     | */
/* |            minimum. La largeur re'elle ou l'autre largeur de la    | */
/* |            boi^te sera modifie'e.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangeLargeur (PtrBox pBox, PtrBox org, PtrBox prec, int delta, int dbl, int frame)
#else  /* __STDC__ */
void                ChangeLargeur (pBox, org, prec, delta, dbl, frame)
PtrBox            pBox;
PtrBox            org;
PtrBox            prec;
int                 delta;
int                 dbl;
int                 frame;

#endif /* __STDC__ */
{
   int                 large;

   if (pBox != NULL)
     {
	/* Regarde si la largeur reelle actuelle depend du contenu */
	if (pBox->BxContentWidth)
	  {
	     /* Il y a une regle de minimum mais ce n'est la largeur reelle */
	     large = pBox->BxRuleWidth + delta;
	     if (large > pBox->BxWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  pBox->BxRuleWidth = pBox->BxWidth;
		  pBox->BxContentWidth = !pBox->BxContentWidth;
		  ModLarg (pBox, org, prec, large - pBox->BxWidth, dbl, frame);
	       }
	     else
		/* Mise a jour de la largeur minimum */
		pBox->BxRuleWidth = large;
	  }
	else if (!pBox->BxAbstractBox->AbWidth.DimIsPosition && pBox->BxAbstractBox->AbWidth.DimMinimum)
	  {
	     /* La largeur reelle est egale au minimum */
	     large = pBox->BxWidth + delta;
	     if (large < pBox->BxRuleWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  large = pBox->BxRuleWidth;
		  pBox->BxRuleWidth = pBox->BxWidth + delta;
		  pBox->BxContentWidth = !pBox->BxContentWidth;
		  ModLarg (pBox, org, prec, large - pBox->BxWidth, dbl, frame);
	       }
	     else
		/* Mise a jour de la largeur minimum */
		ModLarg (pBox, org, prec, delta, dbl, frame);
	  }
	else
	   ModLarg (pBox, org, prec, delta, dbl, frame);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChangeHauteur propage la modification sur la hauteur de la      | */
/* |            boi^te pBox. Cette proce'dure ve'rifie la re`gle du     | */
/* |            minimum. La hauteur re'elle ou l'autre hauteur de la    | */
/* |            boi^te sera modifie'e.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangeHauteur (PtrBox pBox, PtrBox org, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                ChangeHauteur (pBox, org, prec, delta, frame)
PtrBox            pBox;
PtrBox            org;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   int                 haut;

   if (pBox != NULL)
     {
	/* Regarde si la hauteur reelle actuelle depend du contenu */
	if (pBox->BxContentHeight)
	  {
	     /* Il y a une regle de minimum mais ce n'est la hauteur reelle */
	     haut = pBox->BxRuleHeigth + delta;
	     if (haut > pBox->BxHeight)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  pBox->BxRuleHeigth = pBox->BxHeight;
		  pBox->BxContentHeight = !pBox->BxContentHeight;
		  ModHaut (pBox, org, prec, haut - pBox->BxHeight, frame);
	       }
	     else
		/* Mise a jour de la hauteur minimum */
		pBox->BxRuleHeigth = haut;
	  }
	else if (!pBox->BxAbstractBox->AbHeight.DimIsPosition && pBox->BxAbstractBox->AbHeight.DimMinimum)
	  {
	     /* La hauteur courante est egale au minimum */
	     haut = pBox->BxHeight + delta;
	     if (haut < pBox->BxRuleHeigth)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  haut = pBox->BxRuleHeigth;
		  pBox->BxRuleHeigth = pBox->BxHeight + delta;
		  pBox->BxContentHeight = !pBox->BxContentHeight;
		  ModHaut (pBox, org, prec, haut - pBox->BxHeight, frame);
	       }
	     else
		ModHaut (pBox, org, prec, delta, frame);
	  }
	else
	   ModHaut (pBox, org, prec, delta, frame);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChngBElast inverse les reperes et eventuellement le dessin de   | */
/* |            la boite elastique pBox quand la dimension devient      | */
/* |            negative.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChngBElast (PtrBox pBox, PtrBox org, OpRelation op, int delta, int frame, boolean EnX)
#else  /* __STDC__ */
void                ChngBElast (pBox, org, op, delta, frame, EnX)
PtrBox            pBox;
PtrBox            org;
OpRelation          op;
int                 delta;
int                 frame;
boolean             EnX;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAb;
   BoxEdge         rp1;
   int                 trans;

   pAb = pBox->BxAbstractBox;
   trans = 0;
   /* Il faut verifier que l'on pas deja en train de traiter cette boite */
   if (pAb != NULL && delta != 0 && pBox->BxSpaceWidth == 0)
     {
	/* On indique que le traitement est en cours */
	pBox->BxSpaceWidth = 1;
	if (EnX)
	  {
	     /* Recherche le repere deplace et le repere fixe de la boite elastique */
	     if (op == OpWidth)
		rp1 = pAb->AbWidth.DimPosition.PosEdge;

	     else
	       {
		  rp1 = pAb->AbHorizPos.PosEdge;
		  /* Met a jour la pile des boites pour ne pas deplacer org */
		  pBox->BxMoved = org;
	       }

	     /* On calcule le changement de largeur */
	     if (rp1 == Left)
	       {
		  delta = -delta;
		  pBox->BxHorizEdge = Right;
	       }
	     else if (rp1 == Right)
		pBox->BxHorizEdge = Left;
	     else if (rp1 == VertMiddle)
	       {
		  delta = delta * 2;
		  if (op == OpHorizDep)
		     pBox->BxHorizEdge = pAb->AbWidth.DimPosition.PosEdge;
		  else
		     pBox->BxHorizEdge = pAb->AbHorizPos.PosEdge;
	       }

	     /* Faut-il echanger les reperes horizontaux ? */
	     if (delta < 0 && -delta > pBox->BxWidth)
	       {
		  /* Inversion de la boite en horizontal */
		  HorizInverse (pBox, op);

		  /* Translation de l'origine */
		  delta = -delta - 2 * pBox->BxWidth;
		  if (pBox->BxHorizEdge == Right)
		     trans = -pBox->BxWidth;
		  else if (pBox->BxHorizEdge == Left)
		     trans = pBox->BxWidth;
		  DepOrgX (pBox, org, trans, frame);
	       }
	     ModLarg (pBox, org, NULL, delta, 0, frame);

	     /* On retablit le point fixe */
	     pBox->BxHorizEdge = NoEdge;
	  }
	else
	  {
	     /* Recherche le repere deplace et le repere fixe de la boite elastique */
	     if (op == OpHeight)
		rp1 = pAb->AbHeight.DimPosition.PosEdge;

	     else
	       {
		  rp1 = pAb->AbVertPos.PosEdge;
		  /* Met a jour la pile des boites pour ne pas deplacer org */
		  pBox->BxMoved = org;
	       }

	     /* On calcule le changement de hauteur */
	     if (rp1 == Top)
	       {
		  delta = -delta;
		  pBox->BxVertEdge = Bottom;
	       }
	     else if (rp1 == Bottom)
		pBox->BxVertEdge = Top;
	     else if (rp1 == HorizMiddle)
	       {
		  delta = delta * 2;
		  if (op == OpVertDep)
		     pBox->BxVertEdge = pAb->AbHeight.DimPosition.PosEdge;
		  else
		     pBox->BxVertEdge = pAb->AbVertPos.PosEdge;
	       }

	     /* Faut-il echanger les reperes verticaux ? */
	     if (delta < 0 && -delta > pBox->BxHeight)
	       {
		  /* Inversion de la boite en vertical */
		  VertInverse (pBox, op);

		  /* Translation de l'origine */
		  delta = -delta - 2 * pBox->BxHeight;
		  if (pBox->BxVertEdge == Bottom)
		     trans = -pBox->BxHeight;
		  else if (pBox->BxVertEdge == Top)
		     trans = pBox->BxHeight;
		  DepOrgY (pBox, org, trans, frame);

	       }
	     ModHaut (pBox, org, NULL, delta, frame);
	     /* On retablit le point fixe */
	     pBox->BxVertEdge = NoEdge;
	  }
	/* On indique que le traitement est termine */
	pBox->BxSpaceWidth = 0;
	pBox->BxMoved = org;	/* retablit le chainage des deplacements */
     }
}


/* ---------------------------------------------------------------------- */
/* |    DepXContenu deplace horizontalement le contenu englobe de la    | */
/* |            boite pBox.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepXContenu (PtrBox pBox, int delta, int frame)
#else  /* __STDC__ */
void                DepXContenu (pBox, delta, frame)
PtrBox            pBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrAbstractBox             pavefils;
   PtrBox            box1;
   PtrBox            SaveEnglob;
   int                 i;
   boolean             nonnul;
   PtrPosRelations      adpos;
   boolean             reenglobx;
   BoxRelation           *pRe1;

   if (pBox != NULL && (delta != 0 || pBox->BxXToCompute))
     {

	/* On enregistre la hierarchie des boites dont le */
	/* traitement de l'englobement doit etre differe  */
	SaveEnglob = Englobement;
	if (pBox->BxAbstractBox->AbEnclosing != NULL)
	   Englobement = pBox /*->BxAbstractBox->AbEnclosing->AbBox*/ ;

	/* Si Englobement est une boite mere de la boite   */
	/* precedemment designee on garde l'ancienne boite */
	if (Parent (Englobement, SaveEnglob))
	   Englobement = SaveEnglob;

	/* Si la boite est mise en lignes */
	if (pBox->BxType == BoSplit)
	  {
	     box1 = pBox->BxNexChild;	/* On decale les boites de coupure */
	     while (box1 != NULL)
	       {
		  box1->BxXOrg += delta;
		  box1 = box1->BxNexChild;
	       }
	  }

	/* On analyse les relations pour savoir comment deplacer la boite */
	else if (pBox->BxAbstractBox != NULL)
	  {
	     /* La boite est elastique et n'est pas en cours de traitement */
	     if (pBox->BxHorizFlex && (!pBox->BxAbstractBox->AbLeafType == LtCompound || pBox->BxSpaceWidth == 0))
		ChngBElast (pBox, NULL, OpHorizDep, delta, frame, TRUE);
	     /* Dans les autres cas */
	     else
	       {
		  /* simple translation */
		  pBox->BxXOrg += delta;

		  /* Faut-il mettre a jour le rectangle de reaffichage ? */
		  if (!pBox->BxAbstractBox->AbHorizEnclosing && EvalAffich)
		    {
		       /* Prend en compte une zone de debordement des graphiques */
		       if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			  i = EXTRA_GRAPH;
		       else
			  i = 0;
		       if (delta > 0)
			  DefClip (frame, pBox->BxXOrg - delta - i, pBox->BxYOrg - i,
				   pBox->BxXOrg + pBox->BxWidth + i, pBox->BxYOrg + pBox->BxHeight + i);
		       else
			  DefClip (frame, pBox->BxXOrg - i, pBox->BxYOrg - i,
				   pBox->BxXOrg + pBox->BxWidth - delta + i, pBox->BxYOrg + pBox->BxHeight + i);
		    }

		  /* On decale les boites qui ont des relations hors-structure avec */
		  /* la boite deplacee et on met a jour les dimensions elastiques   */
		  /* des boites liees a la boite deplacee.                          */
		  adpos = pBox->BxPosRelations;
		  while (adpos != NULL)
		    {
		       i = 1;
		       nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
		       while (i <= MAX_RELAT_POS && nonnul)
			 {
			    pRe1 = &adpos->PosRTable[i - 1];
			    if (pRe1->ReBox->BxAbstractBox != NULL)
			      {
				 /* Relation hors-struture sur l'origine de la boite */
				 if (pRe1->ReOp == OpHorizDep && pRe1->ReBox->BxXOutOfStruct
				     && pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef == pBox->BxAbstractBox)
				   {

				      /* On refuse de deplacer une boite englobante de pBox */
				      if (Parent (pRe1->ReBox, pBox)) ;

				      else if (pRe1->ReBox->BxHorizFlex)
					 ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, TRUE);
				      else
					 DepOrgX (pRe1->ReBox, pBox, delta, frame);

				   }
				 /* Relation sur la largeur elastique de la boite */
				 else if (pRe1->ReOp == OpWidth)
				    ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, TRUE);
			      }

			    i++;
			    if (i <= MAX_RELAT_POS)
			       nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
			 }
		       adpos = adpos->PosRNext;	/* Bloc suivant */
		    }

		  /* Decale des boites englobees dont l'origine depend de l'englobante */
		  pavefils = pBox->BxAbstractBox->AbFirstEnclosed;
		  /* Traite le niveau inferieur */
		  reenglobx = FALSE;
		  /* Si la boite est en cours de deplacement -> il faut transmettre */
		  /* la valeur de l'origine plutot que la valeur du decalage        */
		  if (pBox->BxXToCompute)
		     delta = pBox->BxXOrg;

		  while (pavefils != NULL)
		    {
		       if (pavefils->AbBox != NULL)
			  if (pavefils->AbBox->BxXOutOfStruct)
			     reenglobx = TRUE;
			  else
			    {
			       if (pBox->BxXToCompute && !pavefils->AbBox->BxHorizFlex)
				  /* Additionne le decalage de la boite */
				  pavefils->AbBox->BxXToCompute = TRUE;
			       DepXContenu (pavefils->AbBox, delta, frame);
			    }
		       pavefils = pavefils->AbNext;
		    }
		  pBox->BxXToCompute = FALSE;	/* le decalage eventuel est pris en compte */

		  /* Si la largeur de la boite depend du contenu et qu'une des     */
		  /* boites filles est positionnee par une relation hors-structure */
		  /* --> il faut reevaluer la largeur correspondante.              */
		  if (reenglobx && pBox->BxContentWidth)
		     DiffereEnglobement (pBox, TRUE);
	       }
	  }
	Englobement = SaveEnglob;	/* On restaure */
     }
}


/* ---------------------------------------------------------------------- */
/* |        DepYContenu deplace verticalement tout le contenu englobe   | */
/* |               de la boite pBox.                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepYContenu (PtrBox pBox, int delta, int frame)
#else  /* __STDC__ */
void                DepYContenu (pBox, delta, frame)
PtrBox            pBox;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrAbstractBox             pavefils;
   PtrBox            box1;
   PtrBox            SaveEnglob;
   int                 i;
   boolean             nonnul;
   PtrPosRelations      adpos;
   boolean             reengloby;
   BoxRelation           *pRe1;

   if (pBox != NULL && (delta != 0 || pBox->BxYToCompute))
     {
	/* On enregistre la hierarchie des boites dont le */
	/* traitement de l'englobement doit etre differe  */
	SaveEnglob = Englobement;
	if (pBox->BxAbstractBox->AbEnclosing != NULL)
	   Englobement = pBox /*->BxAbstractBox->AbEnclosing->AbBox*/ ;

	/* Si Englobement est une boite mere de la boite   */
	/* precedemment designee on garde l'ancienne boite */
	if (Parent (Englobement, SaveEnglob))
	   Englobement = SaveEnglob;

	/* Si la boite est mise en lignes */
	if (pBox->BxType == BoSplit)
	  {
	     box1 = pBox->BxNexChild;	/* On decale les boites de coupure */
	     while (box1 != NULL)
	       {
		  box1->BxYOrg += delta;
		  box1 = box1->BxNexChild;
	       }

	  }

	/* On analyse les relations pour savoir comment deplacer la boite */
	else if (pBox->BxAbstractBox != NULL)
	  {
/*-> La boite est elastique et n'est pas en cours de traitement */
	     if (pBox->BxVertFlex && (!pBox->BxAbstractBox->AbLeafType == LtCompound || pBox->BxSpaceWidth == 0))
		ChngBElast (pBox, NULL, OpVertDep, delta, frame, FALSE);
/*-> Dans les autres cas */
	     else
	       {
		  /* simple translation */
		  pBox->BxYOrg += delta;

		  /* Faut-il mettre a jour le rectangle de reaffichage ? */
		  if (!pBox->BxAbstractBox->AbVertEnclosing && EvalAffich)
		    {
		       /* Prend en compte une zone de debordement des graphiques */
		       if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			  i = EXTRA_GRAPH;
		       else
			  i = 0;
		       if (delta > 0)
			  DefClip (frame, pBox->BxXOrg - i, pBox->BxYOrg - delta - i,
				   pBox->BxXOrg + pBox->BxWidth + i, pBox->BxYOrg + pBox->BxHeight + i);
		       else
			  DefClip (frame, pBox->BxXOrg - i, pBox->BxYOrg - i,
				   pBox->BxXOrg + pBox->BxWidth + i,
				   pBox->BxYOrg + pBox->BxHeight - delta + i);
		    }

		  /* On decale les boites qui ont des relations hors-structure avec */
		  /* la boite deplacee et on met a jour les dimensions elastiques   */
		  /* des boites liees a la boite deplacee.                          */
		  adpos = pBox->BxPosRelations;
		  while (adpos != NULL)
		    {
		       i = 1;
		       nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
		       while (i <= MAX_RELAT_POS && nonnul)
			 {
			    pRe1 = &adpos->PosRTable[i - 1];
			    if (pRe1->ReBox->BxAbstractBox != NULL)
			      {
				 /* Relation hors-struture sur l'origine de la boite */
				 if (pRe1->ReOp == OpVertDep
				     && pRe1->ReBox->BxYOutOfStruct
				     && pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef == pBox->BxAbstractBox)
				   {

				      /* On refuse de deplacer une boite englobante de pBox */
				      if (Parent (pRe1->ReBox, pBox))
					 ;
				      else if (pRe1->ReBox->BxVertFlex)
					 ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, FALSE);
				      else
					 DepOrgY (pRe1->ReBox, pBox, delta, frame);

				   }
				 /* Relation sur la hauteur elastique de la boite */
				 else if (pRe1->ReOp == OpHeight)
				    ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, FALSE);
			      }
			    i++;
			    if (i <= MAX_RELAT_POS)
			       nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			 }
		       adpos = adpos->PosRNext;	/* Bloc suivant */
		    }

		  /* On traite les fils englobes ou places par rapport a l'englobante */
		  pavefils = pBox->BxAbstractBox->AbFirstEnclosed;	/* Traite le niveau inferieur */
		  reengloby = FALSE;
		  /* Si la boite est en cours de deplacement -> il faut transmettre */
		  /* la valeur de l'origine plutot que la valeur du decalage        */
		  if (pBox->BxYToCompute)
		     delta = pBox->BxYOrg;
		  while (pavefils != NULL)
		    {
		       if (pavefils->AbBox != NULL)
			 {
			    if (pavefils->AbBox->BxYOutOfStruct)
			       reengloby = TRUE;
			    else
			      {
				 if (pBox->BxYToCompute && !pavefils->AbBox->BxVertFlex)
				    /* Additionne le decalage de la boite */
				    pavefils->AbBox->BxYToCompute = TRUE;
				 DepYContenu (pavefils->AbBox, delta, frame);
			      }
			 }
		       pavefils = pavefils->AbNext;
		    }
		  pBox->BxYToCompute = FALSE;	/* le decalage eventuel est pris en compte */

		  /* Si la hauteur de la boite depend du contenu et qu'une des     */
		  /* boites filles est positionnee par une relation hors-structure */
		  /* --> il faut reevaluer la hauteur correspondante.              */
		  if (reengloby && pBox->BxContentHeight)
		     DiffereEnglobement (pBox, FALSE);
	       }
	  }
	Englobement = SaveEnglob;	/* On restaure */
     }
}


/* ---------------------------------------------------------------------- */
/* |        DepAxe deplace l'axe de reference de la boite pBox dans la  | */
/* |                frametre frame et les boites qui en dependent.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepAxe (PtrBox pBox, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                DepAxe (pBox, prec, delta, frame)
PtrBox            pBox;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   int                 i, j, k;
   PtrAbstractBox             pAb;
   PtrAbstractBox             pavebox;
   boolean             deplace;
   boolean             nonnul;
   PtrBox            box1;
   PtrBox            cebox;
   PtrPosRelations      adpos;
   BoxRelation           *pRe1;

   if (pBox != NULL && delta != 0)
     {
	if (pBox->BxAbstractBox != NULL)
	   if (!pBox->BxAbstractBox->AbDead)
	     {
		/* Verifie que le deplacement n'est pas deja traite */
		deplace = TRUE;
		box1 = prec;
		cebox = pBox;
		while (box1 != NULL)
		   if (box1 == pBox)
		     {
			box1 = NULL;
			deplace = FALSE;
		     }
		   else
		     {
			if (box1->BxMoved == NULL)
			   cebox = box1;	/* boite ancetre */
			box1 = box1->BxMoved;
		     }

		if (deplace)
		  {
		     pBox->BxVertRef += delta;
		     pavebox = pBox->BxAbstractBox;
		     /* On met a jour la pile des boites traitees */
		     pBox->BxMoved = prec;
		     /* On regarde si les regles de dependance sont valides */
		     if (pavebox->AbEnclosing != NULL)
			if (pavebox->AbEnclosing->AbBox != NULL)
			   deplace = pavebox->AbEnclosing->AbBox->BxType != BoGhost;

		     if (deplace)

			if (pBox->BxHorizEdge == VertRef)
			  {
			     delta = -delta;
			     deplace = TRUE;	/* Il faut verifier l'englobement */
			     /* On evalue la partie de la fenetre a reafficher */
			     if (EvalAffich && pBox->BxType != BoSplit)
			       {
				  i = pBox->BxXOrg;
				  j = pBox->BxXOrg + pBox->BxWidth;
				  /* Prend en compte une zone de debordement des graphiques */
				  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
				     k = EXTRA_GRAPH;
				  else
				     k = 0;
				  if (delta > 0)
				     j += delta;
				  else
				     i += delta;
				  DefClip (frame, i - k, pBox->BxYOrg - k, j + k, pBox->BxYOrg + pBox->BxHeight + k);
			       }

			     if (XEnAbsolu (pBox))
			       {
				  DepXContenu (pBox, delta, frame);
				  /* On a pu detruire le chainage des boites deplacees */
				  pBox->BxMoved = prec;
			       }
			     else
				pBox->BxXOrg += delta;

			     /* On decale aussi les boites liees a la boite deplacee */
			     adpos = pBox->BxPosRelations;
			     while (adpos != NULL)
			       {
				  i = 1;
				  nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
				  while (i <= MAX_RELAT_POS && nonnul)
				    {
				       pRe1 = &adpos->PosRTable[i - 1];
				       if (pRe1->ReBox->BxAbstractBox != NULL)
					 {
					    /* cote gauche */
					    /* cote droit */
					    /* milieu vertical */
					    if (pRe1->ReBox != pBox
					    && pRe1->ReRefEdge != VertRef)
					       if (pRe1->ReOp == OpHorizRef)
						  DepAxe (pRe1->ReBox, pBox, delta, frame);
					       else if ((pRe1->ReOp == OpHorizDep
							 && pRe1->ReBox->BxAbstractBox->AbWidth.DimIsPosition)
						    || pRe1->ReOp == OpWidth)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, TRUE);
					       else if (pRe1->ReOp == OpHorizDep)
						  DepOrgX (pRe1->ReBox, pBox, delta, frame);
					 }

				       i++;
				       if (i <= MAX_RELAT_POS)
					  nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
				    }
				  adpos = adpos->PosRNext;	/* Bloc suivant */
			       }
			  }

		     /* Sinon, on deplace les boites liees a l'axe de reference */
			else
			  {
			     /* Deplacement de boites voisines */
			     deplace = FALSE;
			     adpos = pBox->BxPosRelations;
			     while (adpos != NULL)
			       {
				  i = 1;
				  nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
				  while (i <= MAX_RELAT_POS && nonnul)
				    {
				       pRe1 = &adpos->PosRTable[i - 1];
				       if (pRe1->ReBox->BxAbstractBox != NULL)
					 {
					    /* reference verticale */
					    if (pRe1->ReRefEdge == VertRef)
					       if (pRe1->ReOp == OpHorizRef)
						  DepAxe (pRe1->ReBox, prec, delta, frame);
					       else if ((pRe1->ReOp == OpHorizDep
							 && pRe1->ReBox->BxAbstractBox->AbWidth.DimIsPosition)
						    || pRe1->ReOp == OpWidth)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, TRUE);
					       else if (pRe1->ReOp == OpHorizDep)
						 {
						    DepOrgX (pRe1->ReBox, pBox, delta, frame);
						    deplace = TRUE;	/* Il faut verifier l'englobement */
						 }
					 }

				       i++;
				       if (i <= MAX_RELAT_POS)
					  nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
				    }
				  adpos = adpos->PosRNext;	/* Bloc suivant */
			       }

			     /* On deplace des boites incluses */
			     box1 = NULL;
			     if (XEnAbsolu (pBox) && pBox->BxType != BoBlock)
			       {
				  pAb = pavebox->AbFirstEnclosed;
				  while (pAb != NULL)
				    {
				       if (!pAb->AbDead
					   && pAb->AbHorizPos.PosAbRef == pavebox
					   && pAb->AbHorizPos.PosRefEdge == VertRef)
					 {
					    box1 = pAb->AbBox;
					    DepOrgX (box1, pBox, delta, frame);
					 }
				       pAb = pAb->AbNext;
				    }

				  /* Faut-il reevaluer l'englobement de la boite pBox ? */
				  /*et si ce n'est pas une boite fille de pBox */
				  if (box1 != NULL && Propage == ToAll)
				     Englobx (pavebox, cebox, frame);
			       }
			  }

		     /* On respecte les contraintes d'englobement */
		     pAb = pavebox->AbEnclosing;
		     if (deplace)
			if (pAb == NULL)
			  {
			     if (Propage == ToAll && pBox->BxXOrg < 0)
				DepXContenu (pBox, -pBox->BxXOrg, frame);
			  }
		     /* Verifie l'englobement des boites de la hierarchie voisine */
		     /* sauf si l'englobement des boites doit etre differe        */
			else if (!pAb->AbInLine
				 && pAb->AbBox->BxType != BoGhost
				 && !Parent (pAb->AbBox, cebox)
				 && !Parent (pAb->AbBox, Englobement))
			   Englobx (pAb, cebox, frame);
		  }
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |        DepBase deplace l'axe de reference de la boite pBox dans la | */
/* |                frame frame et les boites qui en dependent.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepBase (PtrBox pBox, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                DepBase (pBox, prec, delta, frame)
PtrBox            pBox;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   int                 i, j, k;
   PtrAbstractBox             pAb;
   PtrAbstractBox             pavebox;
   boolean             deplace;
   boolean             nonnul;
   PtrBox            box1;
   PtrBox            cebox;
   PtrPosRelations      adpos;
   BoxRelation           *pRe1;

   if (pBox != NULL && delta != 0)
     {
	if (pBox->BxAbstractBox != NULL)
	   if (!pBox->BxAbstractBox->AbDead)
	     {
		/* Verifie que le deplacement n'est pas deja traite */
		deplace = TRUE;
		box1 = prec;
		cebox = pBox;
		while (box1 != NULL)
		   if (box1 == pBox)
		     {
			box1 = NULL;
			deplace = FALSE;
		     }
		   else
		     {
			if (box1->BxMoved == NULL)
			   cebox = box1;	/* boite ancetre */
			box1 = box1->BxMoved;
		     }

		if (deplace)
		  {
		     pBox->BxHorizRef += delta;
		     pavebox = pBox->BxAbstractBox;
		     /* On met a jour la pile des boites traitees */
		     pBox->BxMoved = prec;
		     /* On regarde si les regles de dependance sont valides */
		     if (pavebox->AbEnclosing != NULL)
			if (pavebox->AbEnclosing->AbBox != NULL)
			   deplace = pavebox->AbEnclosing->AbBox->BxType != BoGhost;

		     /* Si le point fixe de la boite est l'axe de reference -> deplace */
		     if (deplace)

			if (pBox->BxVertEdge == HorizRef)
			  {
			     delta = -delta;
			     deplace = TRUE;	/* Il faut verifier l'englobement */
			     /* On evalue la partie de la fenetre a reafficher */
			     if (EvalAffich && pBox->BxType != BoSplit)
			       {
				  i = pBox->BxYOrg;
				  j = pBox->BxYOrg + pBox->BxHeight;
				  /* Prend en compte une zone de debordement des graphiques */
				  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
				     k = EXTRA_GRAPH;
				  else
				     k = 0;
				  if (delta > 0)
				     j += delta;
				  else
				     i += delta;
				  DefClip (frame, pBox->BxXOrg - k, i - k, pBox->BxXOrg + pBox->BxWidth + k, j + k);
			       }

			     if (YEnAbsolu (pBox))
			       {
				  DepYContenu (pBox, delta, frame);
				  /* On a pu detruire le chainage des boites deplacees */
				  pBox->BxMoved = prec;
			       }
			     else
				pBox->BxYOrg += delta;

			     /* On decale aussi les boites liees a la boite deplacee */
			     adpos = pBox->BxPosRelations;
			     while (adpos != NULL)
			       {
				  i = 1;
				  nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
				  while (i <= MAX_RELAT_POS && nonnul)
				    {
				       pRe1 = &adpos->PosRTable[i - 1];
				       if (pRe1->ReBox->BxAbstractBox != NULL)
					 {
					    /* cote superieur */
					    /* cote inferieur */
					    /* milieu horizontal */
					    if (pRe1->ReBox != pBox && pRe1->ReRefEdge != HorizRef)
					       if (pRe1->ReOp == OpVertRef)
						  DepBase (pRe1->ReBox, pBox, delta, frame);
					       else if ((pRe1->ReOp == OpVertDep
							 && pRe1->ReBox->BxAbstractBox->AbHeight.DimIsPosition)
						    || pRe1->ReOp == OpHeight)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, FALSE);
					       else if (pRe1->ReOp == OpVertDep)
						  DepOrgY (pRe1->ReBox, pBox, delta, frame);
					 }

				       i++;
				       if (i <= MAX_RELAT_POS)
					  nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
				    }
				  adpos = adpos->PosRNext;
			       }
			  }

		     /* Sinon on deplace les boites liees a l'axe de reference */
			else
			  {
			     /* Deplacement de boites voisines ? */
			     deplace = FALSE;
			     adpos = pBox->BxPosRelations;
			     while (adpos != NULL)
			       {
				  i = 1;
				  nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
				  while (i <= MAX_RELAT_POS && nonnul)
				    {
				       pRe1 = &adpos->PosRTable[i - 1];
				       if (pRe1->ReBox->BxAbstractBox != NULL)
					 {
					    /* reference horizontale */
					    if (pRe1->ReRefEdge == HorizRef)
					       if (pRe1->ReOp == OpVertRef)
						  DepBase (pRe1->ReBox, prec, delta, frame);
					       else if ((pRe1->ReOp == OpVertDep
							 && pRe1->ReBox->BxAbstractBox->AbHeight.DimIsPosition)
						    || pRe1->ReOp == OpHeight)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, FALSE);
					       else if (pRe1->ReOp == OpVertDep)
						 {
						    /* On deplace une voisine */
						    DepOrgY (pRe1->ReBox, pBox, delta, frame);
						    deplace = TRUE;	/* Il faut verifier l'englobement */
						 }
					 }

				       i++;
				       if (i <= MAX_RELAT_POS)
					  nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
				    }
				  adpos = adpos->PosRNext;	/* Bloc suivant */
			       }

			     /* Deplacement de boites incluses ? */
			     box1 = NULL;
			     if (YEnAbsolu (pBox) && pBox->BxType != BoBlock)
			       {
				  pAb = pavebox->AbFirstEnclosed;
				  while (pAb != NULL)
				    {
				       if (!pAb->AbDead
					   && pAb->AbVertPos.PosAbRef == pavebox
					   && pAb->AbVertPos.PosRefEdge == HorizRef)
					 {
					    box1 = pAb->AbBox;
					    DepOrgY (box1, pBox, delta, frame);
					 }
				       pAb = pAb->AbNext;
				    }

				  /* Faut-il reevaluer l'englobement de la boite pBox ? */
				  /* et si ce n'est pas une boite fille de pBox */
				  if (box1 != NULL && Propage == ToAll)
				     Engloby (pavebox, cebox, frame);
			       }
			  }

		     /* On respecte les contraintes d'englobement */
		     pAb = pavebox->AbEnclosing;
		     if (deplace)
			if (pAb == NULL)
			  {
			     if (Propage == ToAll && pBox->BxYOrg < 0)
				DepYContenu (pBox, -pBox->BxYOrg, frame);
			  }
			else if (pAb->AbInLine
				 || pAb->AbBox->BxType == BoGhost)
			  {
			     if (Propage == ToAll)
				EnglLigne (pBox, frame, pAb);
			  }
		     /* Verifie l'englobement des boites de la hierarchie voisine */
		     /* sauf si l'englobement des boites doit etre differe        */
			else if (!Parent (pAb->AbBox, cebox)
				 && !Parent (pAb->AbBox, Englobement))
			   Engloby (pAb, cebox, frame);
		  }
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |    ModLarg modifie la largeur de la boite pBox correspondant a` la | */
/* |        frame frame suite au changement de largeur de la boite      | */
/* |        origine org.                                                | */
/* |        On respecte les contraintes de position :                   | */
/* |        - Toute boite liee a` un des cotes deplaces est             | */
/* |          deplacee.                                                 | */
/* |        - On met a` jour la base de la boite pBox si necessaire.    | */
/* |        On respecte les contraintes de dimension :                  | */
/* |        - On met a` jour les largeurs de boites qui en              | */
/* |          dependent.                                                | */
/* |        Le parametre dbl correspond au nombre de caracteres         | */
/* |        blanc ajoutes (>0) ou retires (<0). Il n'a de sens que      | */
/* |        quand la boite texte appartient a` une ligne justifiee.     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ModLarg (PtrBox pBox, PtrBox org, PtrBox prec, int delta, int dbl, int frame)
#else  /* __STDC__ */
void                ModLarg (pBox, org, prec, delta, dbl, frame)
PtrBox            pBox;
PtrBox            org;
PtrBox            prec;
int                 delta;
int                 dbl;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox            box1;
   PtrLine            adligne;
   PtrAbstractBox             pAb;
   PtrAbstractBox             pavebox;
   int                 i, j, k, dxo;
   int                 dxm, dxf;
   PtrPosRelations      adpos;
   PtrDimRelations      addim;
   boolean             nonnul;
   boolean             deplace;
   BoxRelation           *pRe1;
   ViewSelection            *pMa1;
   boolean             depabsolu;
   boolean             relexterne;

   if (pBox != NULL && delta != 0)
     {
	/* Faut-il nettoyer la file des boites deplacees */
	if (org == NULL && prec == NULL)
	   pBox->BxMoved = NULL;

	if (pBox->BxAbstractBox != NULL)
	   if (!pBox->BxAbstractBox->AbDead)
	     {
		/* On verifie que la largeur d'une boite ne devient pas negative */
		if (Propage != ToSiblings && delta < 0 && -delta > pBox->BxWidth)
		   delta = -pBox->BxWidth;
		/* Valeurs limites avant deplacement */
		i = pBox->BxXOrg;
		j = i + pBox->BxWidth;
		pavebox = pBox->BxAbstractBox;
		/* Ce n'est pas une boite elastique: RAZ sur file boites deplacees */
		if (!pBox->BxHorizFlex)
		   pBox->BxMoved = NULL;

		/* Force la reevaluation des points de controle de la polyline */
		if (pavebox->AbLeafType == LtPlyLine && pBox->BxPictInfo != NULL)
		  {
		     /* libere les points de controle */
		     free ((char *) pBox->BxPictInfo);
		     pBox->BxPictInfo = NULL;
		  }

		/* On regarde si les regles de dependance sont valides */
		deplace = TRUE;
		if (pavebox->AbEnclosing != NULL)
		   if (pavebox->AbEnclosing->AbBox != NULL)
		      deplace = pavebox->AbEnclosing->AbBox->BxType != BoGhost;

		/* On respecte les contraintes de position */
		/* Point fixe sur l'origine */
		if (!deplace || pBox->BxHorizEdge == Left || pBox->BxHorizEdge == VertRef)
		  {
		     /* Mise a jour du reaffichage */
		     /* Valeurs de deplacements des reperes */
		     dxo = 0;
		     dxm = (pBox->BxWidth + delta) / 2 - pBox->BxWidth / 2;
		     dxf = delta;
		  }
		/* Point fixe sur le milieu */
		else if (pBox->BxHorizEdge == VertMiddle)
		  {
		     /* L'origine de la boite et le cote inferieur sont deplaces */
		     dxo = pBox->BxWidth / 2 - (pBox->BxWidth + delta) / 2;
		     dxf = delta + dxo;
		     /* On corrige les erreurs d'arrondi */
		     dxm = 0;
		  }
		/* Point fixe sur le cote droit */
		else
		  {
		     /* L'origine de la boite et le milieu sont deplaces */
		     dxo = -delta;
		     dxm = pBox->BxWidth / 2 - (pBox->BxWidth + delta) / 2;
		     dxf = 0;
		  }
		pBox->BxWidth += delta;
		pBox->BxXOrg += dxo;

		/* On evalue la partie de la fenetre a reafficher */
		if (EvalAffich
		    && pBox->BxType != BoSplit
		/* Il ne faut pas tenir compte de la boite si elle */
		/* n'est pas encore placee dans l'image concrete   */
		    && !pBox->BxXToCompute
		    && !pBox->BxYToCompute
		    && (dxo != 0 || pavebox->AbFirstEnclosed == NULL))
		  {
		     if (pavebox->AbLeafType == LtText)
		       {
			  k = 0;
			  if (dxo == 0)
			     i = j;
			  else if (dxo < 0)
			     i += dxo;
			  if (dxf == 0)
			     j = i;
			  else if (dxf > 0)
			     j += dxf;
		       }
		     else
		       {
			  /* Prend en compte une zone de debordement des graphiques */
			  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (dxo < 0)
			     i += dxo;
			  if (dxf > 0)
			     j += dxf;
		       }
		     DefClip (frame, i - k, pBox->BxYOrg - k, j + k, pBox->BxYOrg + pBox->BxHeight + k);
		  }

		/* Deplacement de boites voisines et de l'englobante ? */
		adpos = pBox->BxPosRelations;
		if (deplace)
		   while (adpos != NULL)
		     {
			i = 1;
			nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			while (i <= MAX_RELAT_POS && nonnul)
			  {
			     pRe1 = &adpos->PosRTable[i - 1];
			     if (pRe1->ReBox->BxAbstractBox != NULL)
				/* Ignore la relation inverse de la boite elastique */
				if (!pBox->BxHorizFlex
				    || pRe1->ReOp != OpHorizDep
				    || pavebox == pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
				   switch (pRe1->ReRefEdge)
					 {
					       /* cote gauche */
					    case Left:
					       if (pRe1->ReOp == OpHorizRef)
						 {
						    if (pRe1->ReBox != pBox)
						       DepAxe (pRe1->ReBox, org, dxo, frame);
						 }
					       else if (pRe1->ReOp == OpHorizInc)
						 {
						    if (!pBox->BxHorizFlex)
						       DepOrgX (pBox, NULL, -dxo, frame);
						 }
					       else if ((pRe1->ReOp == OpHorizDep && pRe1->ReBox->BxHorizFlex)
						    || pRe1->ReOp == OpWidth)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, dxo, frame, TRUE);
					       else if (pRe1->ReBox != org)
						  DepOrgX (pRe1->ReBox, pBox, dxo, frame);
					       break;
					       /* milieu vertical */
					    case VertMiddle:
					       if (pRe1->ReOp == OpHorizRef)
						  if (pRe1->ReBox == pBox)
						    {
						       DepAxe (pBox, NULL, delta / 2, frame);
						       pBox->BxMoved = prec;	/* restaure le chainage */
						    }
						  else
						     DepAxe (pRe1->ReBox, org, dxm, frame);
					       else if (pRe1->ReOp == OpHorizInc)
						 {
						    if (!pBox->BxHorizFlex)
						       DepOrgX (pBox, NULL, -dxm, frame);
						 }
					       else if ((pRe1->ReOp == OpHorizDep && pRe1->ReBox->BxHorizFlex)
						    || pRe1->ReOp == OpWidth)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, dxm, frame, TRUE);
					       else if (pRe1->ReBox != org)
						  DepOrgX (pRe1->ReBox, pBox, dxm, frame);
					       break;
					       /* cote droit */
					    case Right:
					       if (pRe1->ReOp == OpHorizRef)
						  if (pRe1->ReBox == pBox)
						    {
						       DepAxe (pBox, NULL, delta, frame);
						       pBox->BxMoved = prec;	/* restaure le chainage */
						    }
						  else
						     DepAxe (pRe1->ReBox, org, dxf, frame);
					       else if (pRe1->ReOp == OpHorizInc)
						 {
						    if (!pBox->BxHorizFlex)
						       DepOrgX (pBox, NULL, -dxf, frame);
						 }
					       else if ((pRe1->ReOp == OpHorizDep && pRe1->ReBox->BxHorizFlex)
						    || pRe1->ReOp == OpWidth)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, dxf, frame, TRUE);
					       else if (pRe1->ReBox != org)
						  DepOrgX (pRe1->ReBox, pBox, dxf, frame);
					       break;
					    default:
					       break;
					 }

			     i++;
			     if (i <= MAX_RELAT_POS)
				nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
			  }
			adpos = adpos->PosRNext;	/* Bloc suivant */
		     }

		/* Note si la boite est placee en absolu ou non */
		depabsolu = XEnAbsolu (pBox);

		/* Deplacement de boites incluses ou reevaluation du bloc de lignes */
		if (depabsolu
		    || pavebox->AbWidth.DimAbRef != NULL
		    || pavebox->AbWidth.DimValue != 0)
		   /* La boite n'est pas en cours de creation */
		   /* ou elle est a l'interieur d'une boite elastique */
		   /* ou elle est a l'interieur d'une boite hors-structure */
		   /* ou elle n'herite pas de la taille de son contenu */
		   if (pBox->BxType == BoBlock)
		      ReevalBloc (pavebox, pBox->BxFirstLine, org, frame);
		   else
		     {
			pAb = pavebox->AbFirstEnclosed;
			while (pAb != NULL)
			  {
			     if (!pAb->AbDead && pAb->AbBox != NULL)
			       {
				  box1 = pAb->AbBox;
				  /* On regarde si la boite est liee a son englobante */
				  adpos = box1->BxPosRelations;
				  while (adpos != NULL)
				    {
				       i = 1;
				       nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
				       while (i <= MAX_RELAT_POS && nonnul)
					 {
					    pRe1 = &adpos->PosRTable[i - 1];
					    if (pRe1->ReOp == OpHorizInc
					    && pRe1->ReRefEdge != VertRef)
					       switch (pAb->AbHorizPos.PosRefEdge)
						     {
							   /* milieu vertical */
							case VertMiddle:
							   if (depabsolu)
							      j = dxm;
							   else
							      /* il faut reevaluer le centrage */
							      j = (pBox->BxWidth - box1->BxWidth) / 2 - box1->BxXOrg;
							   if (box1->BxHorizFlex)
							      ChngBElast (box1, pBox, pRe1->ReOp, j, frame, TRUE);
							   else
							      DepOrgX (box1, pBox, j, frame);
							   break;
							   /* cote droit */
							case Right:
							   if (depabsolu)
							      j = dxf;
							   else
							      /* il faut reevaluer le cadrage */
							      j = pBox->BxWidth - box1->BxWidth - box1->BxXOrg;
							   if (box1->BxHorizFlex)
							      ChngBElast (box1, pBox, pRe1->ReOp, j, frame, TRUE);
							   else
							      DepOrgX (box1, pBox, j, frame);
							   break;
							   /* cote gauche */
							default:
							   if (depabsolu)
							      j = dxo;
							   else
							      /* il faut reevaluer le cadrage */
							      j = 0;
							   if (box1->BxHorizFlex)
							      ChngBElast (box1, pBox, pRe1->ReOp, j, frame, TRUE);
							   else
							      DepOrgX (box1, pBox, j, frame);
							   break;
						     }

					    i++;
					    if (i <= MAX_RELAT_POS)
					       nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
					 }
				       adpos = adpos->PosRNext;	/* Bloc suivant */
				    }
			       }
			     pAb = pAb->AbNext;
			  }
		     }

		/* On respecte les contraintes de dimension */
		addim = pBox->BxWidthRelations;
		while (addim != NULL)
		  {
		     i = 1;
		     box1 = addim->DimRTable[i - 1];
		     while (i <= MAX_RELAT_DIM && box1 != NULL)
		       {
			  pAb = box1->BxAbstractBox;
			  /* Est-ce la meme dimension ? */
			  if (addim->DimRSame[i - 1])	/* Oui => Changement de largeur */
			    {
			       if (pAb->AbWidth.DimUnit == UnPercent)
				  /* Le changement de taille est un pourcentage */
				  dxo = pBox->BxWidth * pAb->AbWidth.DimValue / 100 - box1->BxWidth;
			       else
				  dxo = delta;	/* + ou - une constante */
			       /* On evite de boucler sur l'automodification */
			       if (box1 != pBox)
				  ChangeLargeur (box1, org, pBox, dxo, dbl, frame);

			    }
			  else
			     /* Non => Changement de hauteur */
			    {
			       /* L'heritage porte sur la hauteur de la ligne? */
			       if (pAb->AbEnclosing == pavebox
				   && (pavebox->AbInLine || pavebox->AbBox->BxType == BoGhost))
				 {
				    adligne = DesLigne (box1);
				    if (adligne == NULL)
				       dxo = 0;		/* la ligne n'est pas encore construite */
				    else
				       dxo = adligne->LiHeight - box1->BxHeight;
				 }
			       /* Le changement de taille est un pourcentage */
			       else if (pAb->AbHeight.DimUnit == UnPercent)
				  dxo = pBox->BxWidth * pAb->AbHeight.DimValue / 100 - box1->BxHeight;
			       else
				  dxo = delta;
			       ChangeHauteur (box1, org, NULL, dxo, frame);
			    }

			  i++;
			  if (i <= MAX_RELAT_DIM)
			     box1 = addim->DimRTable[i - 1];
		       }
		     addim = addim->DimRNext;
		  }

		/* On respecte les contraintes d'englobement */
		pAb = pavebox->AbEnclosing;		/* pave pere */
		if (!deplace)
		  {
		     /* Si la boite n'est pas fille d'une boite eclatee */
		     /* --> on recherche la boite bloc de lignes        */
		     while (!pAb->AbInLine || pAb->AbBox->BxType == BoGhost)
			pAb = pAb->AbEnclosing;
		  }

		if (pAb == NULL)
		  {
		     /* C'est la racine de l'image concrete */
		     if (Propage == ToAll && pBox->BxXOrg < 0)
			DepXContenu (pBox, -pBox->BxXOrg, frame);
		  }
		/* Englobement a refaire si la boite est englobee */
		else if (pavebox->AbHorizEnclosing)
		  {
		     /* note l'origine externe ou non de la modification de largeur */
		     relexterne = !Parent (pAb->AbBox, org);

		     /* si org n'est pas une fille il faut propager */
		     if ((Propage == ToAll || relexterne)
		     /* et si l'englobement n'est pas fait par une boite soeur */
			 && !Soeur (pBox, prec)
			 && !Soeur (pBox, org))
		       {

			  /* Inclusion dans un bloc de ligne */
			  if (pAb->AbInLine)
			    {
			       pMa1 = &FntrTable[frame - 1].FrSelectionBegin;
			       if (pBox == pMa1->VsBox)
				  adligne = pMa1->VsLine;
			       else
				  adligne = DesLigne (pBox);
			       MajBloc (pAb, adligne, pBox, delta, dbl, frame);
			    }
			  /* Si l'englobement n'est pas prevu en fin de traitement */
			  else if (pAb->AbBox != Englobement
				 && !Parent (pAb->AbBox, Englobement))
			     /* On differe le traitement de l'englobement   */
			     /* quand la mise a jour a une origine externe  */
			     if (Propage != ToAll)
				DiffereEnglobement (pAb->AbBox, TRUE);
			  /* l'englobement d'une boite ne peut etre traite */
			  /* plus de deux fois (sinon on boucle).      */
			     else if (pAb->AbBox->BxNPixels <= 1)
				Englobx (pAb, org, frame);

		       }
		     else if (!pavebox->AbNew
			      && Propage == ToSiblings
			      && pavebox->AbLeafType == LtCompound
			    && pavebox->AbInLine && !pBox->BxYToCompute)
			/* La largeur de la boite mise en lignes est donnee par une */
			/* boite suivante, il faut verifier l'englobement vertical */
			Engloby (pAb, org, frame);
		  }
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |    ModHaut modifie la hauteur de la boite pBox correspondant a` la | */
/* |         frame frame suite au changement de hauteur de la boite     | */
/* |         origine org. On respecte les contraintes de position :     | */
/* |         - Toute boite liee a` un des cotes deplaces est            | */
/* |           deplacee.                                                | */
/* |         - On met a` jour la base de la boite pBox si necessaire.   | */
/* |         On respecte les contraintes de dimension :                 | */
/* |         - On met a` jour les hauteurs des boites qui en            | */
/* |           dependent.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ModHaut (PtrBox pBox, PtrBox org, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                ModHaut (pBox, org, prec, delta, frame)
PtrBox            pBox;
PtrBox            org;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox            box1;
   int                 i, j, k, dyo;
   int                 dym, dyf;
   PtrLine            adligne;
   PtrAbstractBox             pAb;
   PtrAbstractBox             pavebox;
   PtrPosRelations      adpos;
   PtrDimRelations      addim;
   boolean             nonnul;
   boolean             deplace;
   BoxRelation           *pRe1;
   boolean             depabsolu;
   boolean             relexterne;
   ptrfont             font;
   float               valeur;

   if (pBox != NULL && delta != 0)
     {

	/* Faut-il nettoyer la file des boites deplacees */
	if (org == NULL && prec == NULL)
	   pBox->BxMoved = NULL;

	if (pBox->BxAbstractBox != NULL)
	   if (!pBox->BxAbstractBox->AbDead)
	     {
		/* On verifie que la hauteur d'une boite ne devient pas negative */
		if (Propage != ToSiblings && delta < 0 && -delta > pBox->BxHeight)
		   delta = -pBox->BxHeight;
		/* Valeurs limites avant deplacement */
		i = pBox->BxYOrg;
		j = i + pBox->BxHeight;
		pavebox = pBox->BxAbstractBox;

		/* Ce n'est pas une boite elastique: RAZ sur file des boites deplacees */
		if (!pBox->BxVertFlex)
		   pBox->BxMoved = NULL;

		/* Force la reevaluation des points de controle de la polyline */
		if (pavebox->AbLeafType == LtPlyLine && pBox->BxPictInfo != NULL)
		  {
		     /* libere les points de controle */
		     free ((char *) pBox->BxPictInfo);
		     pBox->BxPictInfo = NULL;
		  }

		/* On regarde si les regles de dependance sont valides */
		deplace = TRUE;
		if (pavebox->AbEnclosing != NULL)
		   if (pavebox->AbEnclosing->AbBox != NULL)
		      deplace = pavebox->AbEnclosing->AbBox->BxType != BoGhost;

		/* On respecte les contraintes de position */
/*=> Point fixe sur l'origine */
		if (!deplace || pBox->BxVertEdge == Top || pBox->BxVertEdge == HorizRef)
		  {
		     /* Valeurs de deplacement des reperes */
		     dyo = 0;
		     dym = (pBox->BxHeight + delta) / 2 - pBox->BxHeight / 2;
		     dyf = delta;
		  }
/*=> Point fixe sur le milieu */
		else if (pBox->BxVertEdge == HorizMiddle)
		  {
		     /* L'origine de la boite et le cote inferieur sont deplaces */
		     dyo = pBox->BxHeight / 2 - (pBox->BxHeight + delta) / 2;
		     dyf = delta + dyo;		/* On corrige les erreurs d'arrondi */
		     dym = 0;
		  }
/*=> Point fixe sur le cote inferieur */
		else
		  {
		     /* L'origine de la boite et le milieu sont deplaces */
		     dyo = -delta;
		     dym = pBox->BxHeight / 2 - (pBox->BxHeight + delta) / 2;
		     dyf = 0;
		  }
		pBox->BxHeight += delta;
		pBox->BxYOrg += dyo;

		/* On evalue la partie de la fenetre a reafficher */
		if (EvalAffich
		    && pBox->BxType != BoSplit
		/* Il ne faut pas tenir compte de la boite si elle */
		/* n'est pas encore placee dans l'image concrete   */
		    && !pBox->BxXToCompute
		    && !pBox->BxYToCompute
		    && (dyo != 0 || pavebox->AbFirstEnclosed == NULL))
		  {
		     if (pavebox->AbLeafType == LtText)
		       {
			  k = 0;
			  if (dyo == 0)
			     i = j;
			  else if (dyo < 0)
			     i += dyo;
			  if (dyf == 0)
			     j = i;
			  else if (dyf > 0)
			     j += dyf;
		       }
		     else
		       {
			  /* Prend en compte une zone de debordement des graphiques */
			  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (dyo < 0)
			     i += dyo;
			  if (dyf > 0)
			     j += dyf;
		       }
		     DefClip (frame, pBox->BxXOrg - k, i - k, pBox->BxXOrg + pBox->BxWidth + k, j + k);
		  }

		/* Deplacement de boites voisines et de l'englobante ? */
		adpos = pBox->BxPosRelations;
		if (deplace)
		   while (adpos != NULL)
		     {
			i = 1;
			nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
			while (i <= MAX_RELAT_POS && nonnul)
			  {
			     pRe1 = &adpos->PosRTable[i - 1];
			     if (pRe1->ReBox->BxAbstractBox != NULL)
				/* Ignore la relation inverse de la boite elastique */
				if (!pBox->BxVertFlex
				    || pRe1->ReOp != OpVertDep
				    || pavebox == pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
				   switch (pRe1->ReRefEdge)
					 {
					       /* cote superieur */
					    case Top:
					       if (pRe1->ReOp == OpVertRef)
						 {
						    if (pRe1->ReBox != pBox)
						       DepBase (pRe1->ReBox, org, dyo, frame);
						 }
					       else if (pRe1->ReOp == OpVertInc)
						 {
						    if (!pBox->BxVertFlex)
						       DepOrgY (pBox, NULL, -dyo, frame);
						 }
					       else if ((pRe1->ReOp == OpVertDep && pRe1->ReBox->BxVertFlex)
						    || pRe1->ReOp == OpHeight)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, dyo, frame, FALSE);
					       else if (pRe1->ReBox != org)
						  DepOrgY (pRe1->ReBox, pBox, dyo, frame);
					       break;
					       /* milieu horizontal */
					    case HorizMiddle:
					       if (pRe1->ReOp == OpVertRef)
						  if (pRe1->ReBox == pBox)
						    {
						       DepBase (pBox, NULL, delta / 2, frame);
						       pBox->BxMoved = prec;	/* restaure le chainage */
						    }
						  else
						     DepBase (pRe1->ReBox, org, dym, frame);
					       else if (pRe1->ReOp == OpVertInc)
						 {
						    if (!pBox->BxVertFlex)
						       DepOrgY (pBox, NULL, -dym, frame);
						 }
					       else if ((pRe1->ReOp == OpVertDep && pRe1->ReBox->BxVertFlex)
						    || pRe1->ReOp == OpHeight)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, dym, frame, FALSE);
					       else if (pRe1->ReBox != org)
						  DepOrgY (pRe1->ReBox, pBox, dym, frame);
					       break;
					       /* cote inferieur */
					    case Bottom:
					       if (pRe1->ReOp == OpVertRef)
						 {
						    if (pRe1->ReBox == pBox)
						      {
							 if (pavebox->AbLeafType == LtText
							     && pavebox->AbHorizRef.PosAbRef == NULL)
							    j = FontBase (pBox->BxFont) - pBox->BxHorizRef;
							 else
							    j = delta;
							 DepBase (pBox, NULL, j, frame);
							 pBox->BxMoved = prec;	/* restaure le chainage */
						      }
						    else
						       DepBase (pRe1->ReBox, org, dyf, frame);
						 }
					       else if (pRe1->ReOp == OpVertInc)
						 {
						    if (!pBox->BxVertFlex)
						       DepOrgY (pBox, NULL, -dyf, frame);
						 }
					       else if ((pRe1->ReOp == OpVertDep && pRe1->ReBox->BxVertFlex)
						    || pRe1->ReOp == OpHeight)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, dyf, frame, FALSE);
					       else if (pRe1->ReBox != org)
						  DepOrgY (pRe1->ReBox, pBox, dyf, frame);
					       break;
					    default:
					       break;
					 }
			     i++;
			     if (i <= MAX_RELAT_POS)
				nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			  }
			adpos = adpos->PosRNext;	/* Bloc suivant */
		     }

		/* Note si la boite est placee en absolu ou non */
		depabsolu = YEnAbsolu (pBox);

		/* Deplacement de boites incluses ? */
		if (depabsolu && pBox->BxType == BoBlock)
		  {
		     /* Il faut traiter les blocs de lignes elastiques */
		     /* qui ne sont pas en cours de placement          */
		     if (dyo != 0)
		       {
			  /* On decale aussi les boites mises en ligne */
			  pAb = pavebox->AbFirstEnclosed;
			  while (pAb != NULL)
			    {
			       if (!pAb->AbDead)
				  DepYContenu (pAb->AbBox, dyo, frame);
			       pAb = pAb->AbNext;
			    }
		       }
		  }
		/* Si on n'est pas en cours de creation */
		/* ou si elle est a l'interieur d'une boite elastique */
		/* ou si elle est a l'interieur d'une boite hors-structure */
		/* ou si la boite n'herite pas de la taille de son contenu */
		else if (depabsolu
			 || pavebox->AbHeight.DimAbRef != NULL
			 || pavebox->AbHeight.DimValue != 0)
		  {
		     pAb = pavebox->AbFirstEnclosed;
		     while (pAb != NULL)
		       {
			  if (!pAb->AbDead && pAb->AbBox != NULL)
			    {
			       box1 = pAb->AbBox;
			       /* On regarde si la boite est liee a son englobante */
			       adpos = box1->BxPosRelations;
			       while (adpos != NULL)
				 {
				    i = 1;
				    nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
				    while (i <= MAX_RELAT_POS && nonnul)
				      {
					 pRe1 = &adpos->PosRTable[i - 1];
					 if (pRe1->ReOp == OpVertInc
					   && pRe1->ReRefEdge != HorizRef)
					   {
					      switch (pAb->AbVertPos.PosRefEdge)
						    {
							  /* milieu horizontal */
						       case HorizMiddle:
							  if (depabsolu)
							     j = dym;
							  else
							     /* il faut reevaluer le centrage */
							     j = (pBox->BxHeight - box1->BxHeight) / 2 - box1->BxYOrg;
							  if (box1->BxVertFlex)
							     ChngBElast (box1, pBox, pRe1->ReOp, j, frame, FALSE);
							  else
							     DepOrgY (box1, pBox, j, frame);
							  break;
							  /* cote inferieur */
						       case Bottom:
							  if (depabsolu)
							     j = dyf;
							  else
							     /* il faut reevaluer le cadrage */
							     j = pBox->BxHeight - box1->BxHeight - box1->BxYOrg;
							  if (box1->BxVertFlex)
							     ChngBElast (box1, pBox, pRe1->ReOp, j, frame, FALSE);
							  else
							     DepOrgY (box1, pBox, j, frame);
							  break;
							  /* cote superieur */
						       default:
							  if (depabsolu)
							     j = dyo;
							  else
							     /* il faut reevaluer le cadrage */
							     j = 0;
							  if (box1->BxVertFlex)
							     ChngBElast (box1, pBox, pRe1->ReOp, j, frame, FALSE);
							  else
							     DepOrgY (box1, pBox, j, frame);
							  break;
						    }
					   }
					 i++;
					 if (i <= MAX_RELAT_POS)
					    nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
				      }
				    adpos = adpos->PosRNext;	/* Bloc suivant */
				 }
			    }
			  pAb = pAb->AbNext;
		       }
		  }

		/* On respecte les contraintes de dimension */
		addim = pBox->BxHeightRelations;
		while (addim != NULL)
		  {
		     i = 1;
		     box1 = addim->DimRTable[i - 1];
		     while (i <= MAX_RELAT_DIM && box1 != NULL)
		       {
			  pAb = box1->BxAbstractBox;

			  /* Est-ce la meme dimension ? */
			  if (addim->DimRSame[i - 1])	/* Oui => Changement de hauteur */
			    {
			       /* L'heritage porte sur la hauteur de la ligne ? */
			       if (pAb->AbEnclosing == pavebox
				   && (pavebox->AbInLine || pavebox->AbBox->BxType == BoGhost))
				 {
				    adligne = DesLigne (box1);
				    if (adligne == NULL)
				       dyo = 0;		/* la ligne n'est pas encore construite */
				    else
				       dyo = adligne->LiHeight - box1->BxHeight;
				 }
			       /* Le changement de taille est un pourcentage */
			       else if (pAb->AbHeight.DimUnit == UnPercent)
				  dyo = pBox->BxHeight * pAb->AbHeight.DimValue / 100 - box1->BxHeight;
			       else
				  dyo = delta;
			       /* On evite de boucler sur l'automodification */
			       if (box1 != pBox)
				  ChangeHauteur (box1, org, pBox, dyo, frame);
			    }
			  else
			     /* Non => Changement de largeur */
			    {
			       if (pAb->AbWidth.DimUnit == UnPercent)
				  /* Le changement de taille est un pourcentage */
				  dyo = pBox->BxHeight * pAb->AbWidth.DimValue / 100 - box1->BxWidth;
			       else
				  dyo = delta;	/* + ou - une constante */
			       ChangeLargeur (box1, org, NULL, dyo, 0, frame);
			    }
			  i++;
			  if (i <= MAX_RELAT_DIM)
			     box1 = addim->DimRTable[i - 1];
		       }
		     addim = addim->DimRNext;	/* Bloc suivant */
		  }

		/* On respecte les contraintes d'englobement */
		pAb = pavebox->AbEnclosing;
		if (!deplace)
		  {
		     /* Si la boite n'est pas fille d'une boite eclatee */
		     /* --> on recherche la boite bloc de lignes        */
		     while (!pAb->AbInLine || pAb->AbBox->BxType == BoGhost)
			pAb = pAb->AbEnclosing;
		  }

		if (pAb == NULL)
		  {
		     /* C'est la racine de l'image concrete */
		     if (Propage == ToAll && pBox->BxYOrg < 0)
			DepYContenu (pBox, -pBox->BxYOrg, frame);
		  }
		/* Englobement a refaire si la boite est englobee */
		else if (pavebox->AbVertEnclosing)
		  {
		     /* note l'origine externe ou non de la modification de hauteur */
		     relexterne = !Parent (pAb->AbBox, org);

		     /* si org n'est pas une fille il faut propager */
		     if ((Propage == ToAll || relexterne)
		     /* et si l'englobement n'est pas fait par une boite soeur */
			 && !Soeur (pBox, prec)
			 && !Soeur (pBox, org))
		       {
			  if (pAb->AbInLine)
			     /* Inclusion dans un bloc de ligne */
			     EnglLigne (pBox, frame, pAb);
			  /* Si l'englobement n'est pas prevu en fin de traitement */
			  else if (pAb->AbBox != Englobement
				 && !Parent (pAb->AbBox, Englobement))
			     /* On differe le traitement de l'englobement   */
			     /* quand la mise a jour a une origine externe  */
			     if (Propage != ToAll)
				DiffereEnglobement (pAb->AbBox, FALSE);
			  /* l'englobement d'une boite ne peut etre traite */
			  /* plus de deux fois (sinon on boucle).      */
			     else if (pAb->AbBox->BxSpaceWidth <= 1)
				Engloby (pAb, org, frame);
		       }
		  }
	     }
     }

   /* Traitement specifique des largeurs de symboles */
   if (pBox != NULL)
     {
	if (pBox->BxAbstractBox->AbLeafType == LtSymbol)
	  {
	     i = 0;
	     font = pBox->BxFont;
	     valeur = 1 + ((float) (pBox->BxHeight * 0.6) / (float) FontHeight (font));
	     switch (pBox->BxAbstractBox->AbShape)
		   {
		      case 'd':	/*integrale double */
			 i = CarWidth (231, font) + CarWidth (231, font) / 2;
		      case 'i':	/*integrale */
		      case 'c':	/*integrale circulaire */
			 i = (int) ((float) CarWidth (231, font) * valeur);
			 ModLarg (pBox, NULL, NULL, i - pBox->BxWidth, 0, frame);
			 break;
		      case '(':
		      case ')':
		      case '{':
		      case '}':
		      case '[':
		      case ']':
			 i = (int) ((float) CarWidth (230, font) * valeur);
			 ModLarg (pBox, NULL, NULL, i - pBox->BxWidth, 0, frame);
			 break;
		      default:
			 break;
		   }		/*switch */
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    DepOrgX deplace l'origine de la boite pBox, donnee en parametre,| */
/* |            de delta. On respecte les contraintes de position.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepOrgX (PtrBox pBox, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                DepOrgX (pBox, prec, delta, frame)
PtrBox            pBox;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox            box1;
   int                 i, j, k;
   PtrAbstractBox             pAb;
   PtrPosRelations      adpos;
   boolean             nonnul;
   boolean             deplace;
   boolean             depabsolu;
   PtrAbstractBox             pPa1;
   BoxRelation           *pRe1;
   boolean             aplacer;

   if (pBox != NULL && delta != 0)
     {
	pPa1 = pBox->BxAbstractBox;
	if (pPa1 != NULL)
	   if (!pPa1->AbDead)
	     {
		/* On verifie que la boite n'a pas ete deja deplacee */
		if (prec != NULL)
		  {
		     box1 = prec->BxMoved;
		     while (box1 != NULL)
			if (box1 == pBox)
			   return;
			else
			   box1 = box1->BxMoved;
		  }
		/* On met a jour la pile des boites traitees */
		pBox->BxMoved = prec;

		aplacer = pBox->BxXToCompute;	/* memorise que la boite doit etre placee */
		/* On evalue la partie de la fenetre a reafficher */
		if (EvalAffich)
		  {
		     if (pBox->BxType != BoSplit)
		       {
			  i = pBox->BxXOrg;
			  j = pBox->BxXOrg + pBox->BxWidth;
			  /* Prend en compte une zone de debordement des graphiques */
			  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (delta > 0)
			     j += delta;
			  else
			     i += delta;
			  if (pBox->BxHeight > 0)
			     DefClip (frame, i - k, pBox->BxYOrg - k, j + k, pBox->BxYOrg + pBox->BxHeight + k);
		       }
		     /* Est-ce un pave non englobe ? */
		     else if (!pPa1->AbVertEnclosing)
			if (delta > 0)
			   DefClip (frame, pBox->BxXOrg - delta, pBox->BxYOrg,
				    pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
			else
			   DefClip (frame, pBox->BxXOrg, pBox->BxYOrg, pBox->BxXOrg +
				    pBox->BxWidth - delta, pBox->BxYOrg + pBox->BxHeight);
		  }

		/* On deplace seulement l'origine de la boite si elle est en cours  */
		/* de creation et que ce n'est pas une boite elastique, sinon on    */
		/*  deplace tout le contenu (boites englobees)                      */
		depabsolu = XEnAbsolu (pBox);

		/* Teste si la position de la boite est absolue ou relative */
		if (depabsolu)
		  {
		     DepXContenu (pBox, delta, frame);
		     /* On a pu detruire le chainage des boites deplacees */
		     pBox->BxMoved = prec;
		     /* On evite d'introduire une boucle */
		     box1 = prec;
		     while (box1 != NULL)
			if (box1->BxMoved == prec)
			  {
			     box1->BxMoved = NULL;
			     box1 = NULL;
			  }
			else
			   box1 = box1->BxMoved;
		  }
		else
		   pBox->BxXOrg += delta;

		/* Regarde si les regles de dependance sont valides */
		deplace = TRUE;
		if (pPa1->AbEnclosing != NULL)
		   if (pPa1->AbEnclosing->AbBox != NULL)
		      deplace = (pPa1->AbEnclosing->AbBox->BxType != BoGhost);

		/* Decale les boites dependantes qui restent a deplacer */
		adpos = pBox->BxPosRelations;
		if (deplace)
		   while (adpos != NULL)
		     {
			i = 1;
			nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
			while (i <= MAX_RELAT_POS && nonnul)
			  {
			     pRe1 = &adpos->PosRTable[i - 1];
			     if (pRe1->ReBox->BxAbstractBox != NULL)
			       {
				  /* cote gauche */
				  /* cote droit */
				  /* milieu vertical */
				  /* ref. verticale */
				  if (pRe1->ReOp == OpHorizRef)
				    {
				       /* Sauf l'axe de reference de la boite elle-meme */
				       if (pRe1->ReBox != pBox)
					 {
					    pAb = pPa1->AbEnclosing;
					    if (pAb != NULL)
					       box1 = pAb->AbBox;
					    else
					       box1 = NULL;
					    if (pRe1->ReBox != box1 || Propage == ToAll)
					       DepAxe (pRe1->ReBox, pBox, delta, frame);
					 }
				    }
				  /* Ignore la relation inverse de la boite elastique */
				  else if (pBox->BxHorizFlex
					   && pPa1 != pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
				     ;

				  /* On ne decale pas les boites qui ont des relations  */
				  /* hors-structure avec la boite deplacee et on ne met */
				  /* pas a jour les dimensions elastiques des boites    */
				  /* liees a la boite deplacee si elles ont ete         */
				  /* traitees par DepXContenu.                          */

				  else if (depabsolu)
				    {
				       if (!pBox->BxHorizFlex || aplacer)
					 {
					    /* le travail n'a pas ete fait dans DepXContenu */
					    if (pRe1->ReOp == OpHorizDep && !pRe1->ReBox->BxXOutOfStruct)
					       /* Relation conforme a la structure sur l'origine de boite */
					       if (pRe1->ReBox->BxHorizFlex
					       /* si la boite n'est pas une boite fille */
						   && pPa1 != pRe1->ReBox->BxAbstractBox->AbEnclosing
						   && pPa1 == pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, TRUE);
					    /* Relation conforme a la structure sur la largeur de boite */
					       else
						  DepOrgX (pRe1->ReBox, pBox, delta, frame);
					 }
				    }
				  else if (pRe1->ReOp == OpHorizDep && !pRe1->ReBox->BxHorizFlex)
				     DepOrgX (pRe1->ReBox, pBox, delta, frame);
				  else if (((pRe1->ReOp == OpHorizDep
					     && pPa1 == pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef))
					   || pRe1->ReOp == OpWidth)
				     ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, TRUE);
			       }

			     /* On a pu detruire le chainage des boites deplacees */
			     pBox->BxMoved = prec;
			     i++;
			     if (i <= MAX_RELAT_POS)
				nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			  }
			adpos = adpos->PosRNext;	/* Bloc suivant */
		     }

		/* Si le calcul de la largeur de la boite englobante est a refaire */
		pAb = pPa1->AbEnclosing;
		if (deplace && pBox->BxXOutOfStruct && pAb != NULL)
		   /* On ne peut traiter l'englobement d'une boite si cette boite */
		   /* est en cours de placement ou si ce traitement est differe   */
		   if (!pAb->AbBox->BxXToCompute
		       && !Parent (pAb->AbBox, Englobement)
		   /* si org est une fille il faut la propagation a toutes */
		       && (Propage == ToAll || !Parent (pAb->AbBox, prec))
		   /* et si l'englobement n'est pas fait par une boite soeur */
		       && !Soeur (pBox, prec)
		   /* et si la boite deplacee est effectivement englobee */
		       && pPa1->AbHorizEnclosing)
		      Englobx (pAb, prec, frame);
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |    DepOrgY deplace l'origine de la boite pBox, donnee en parametre,| */
/* |            de delta. On respecte les contraintes de position.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepOrgY (PtrBox pBox, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                DepOrgY (pBox, prec, delta, frame)
PtrBox            pBox;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox            box1;
   int                 i, j, k;
   PtrAbstractBox             pAb;
   PtrPosRelations      adpos;
   boolean             nonnul;
   boolean             deplace;
   boolean             depabsolu;
   PtrAbstractBox             pPa1;
   BoxRelation           *pRe1;
   boolean             aplacer;

   if (pBox != NULL && delta != 0)
     {
	pPa1 = pBox->BxAbstractBox;
	if (pPa1 != NULL)
	   if (!pPa1->AbDead)
	     {
		/* On verifie que la boite n'a pas ete deja deplacee */
		if (prec != NULL)
		  {
		     box1 = prec->BxMoved;
		     while (box1 != NULL)
			if (box1 == pBox)
			   return;
			else
			   box1 = box1->BxMoved;
		  }
		/* On met a jour la pile des boites traitees */
		pBox->BxMoved = prec;

		aplacer = pBox->BxYToCompute;	/* memorise que la boite doit etre placee */
		/* On evalue la partie de la fenetre a reafficher */
		if (EvalAffich)
		  {
		     if (pBox->BxType != BoSplit)
		       {
			  i = pBox->BxYOrg;
			  j = pBox->BxYOrg + pBox->BxHeight;
			  /* Prend en compte une zone de debordement des graphiques */
			  if (pBox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (delta > 0)
			     j += delta;
			  else
			     i += delta;
			  if (pBox->BxWidth > 0 || k > 0)
			     DefClip (frame, pBox->BxXOrg - k, i - k, pBox->BxXOrg + pBox->BxWidth + k, j + k);
		       }
		     /* Est-ce un pave non englobe ? */
		     else if (!pPa1->AbHorizEnclosing)
			if (delta > 0)
			   DefClip (frame, pBox->BxXOrg, pBox->BxYOrg - delta,
				    pBox->BxXOrg + pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight);
			else
			   DefClip (frame, pBox->BxXOrg, pBox->BxYOrg, pBox->BxXOrg +
				    pBox->BxWidth, pBox->BxYOrg + pBox->BxHeight - delta);
		  }

		/* On deplace seulement l'origine de la boite si elle est en cours  */
		/* de creation et que ce n'est pas une boite elastique, sinon on    */
		/*  deplace tout le contenu (boites englobees)                      */
		depabsolu = YEnAbsolu (pBox);

		/* Teste si la position de la boite est absolue ou relative */
		if (depabsolu)
		  {
		     DepYContenu (pBox, delta, frame);
		     /* On a pu detruire le chainage des boites deplacees */
		     pBox->BxMoved = prec;
		     /* On evite d'introduire une boucle */
		     box1 = prec;
		     while (box1 != NULL)
			if (box1->BxMoved == prec)
			  {
			     box1->BxMoved = NULL;
			     box1 = NULL;
			  }
			else
			   box1 = box1->BxMoved;
		  }
		else
		   pBox->BxYOrg += delta;

		/* Regarde si les regles de dependance sont valides */
		deplace = TRUE;
		if (pPa1->AbEnclosing != NULL)
		   if (pPa1->AbEnclosing->AbBox != NULL)
		      deplace = (pPa1->AbEnclosing->AbBox->BxType != BoGhost);


		/* On decale les boites dependantes qui restent a deplacer */

		adpos = pBox->BxPosRelations;
		if (deplace)
		   while (adpos != NULL)
		     {
			i = 1;
			nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
			while (i <= MAX_RELAT_POS && nonnul)
			  {
			     pRe1 = &adpos->PosRTable[i - 1];
			     if (pRe1->ReBox->BxAbstractBox != NULL)
			       {
				  /* cote superieur */
				  /* cote inferieur */
				  /* milieu horizontal */
				  /* ref. horizontale */
				  if (pRe1->ReOp == OpVertRef)
				    {
				       /* Sauf l'axe de reference de la boite elle-meme */
				       if (pRe1->ReBox != pBox)
					 {
					    pAb = pPa1->AbEnclosing;
					    if (pAb != NULL)
					       box1 = pAb->AbBox;
					    else
					       box1 = NULL;
					    if (pRe1->ReBox != box1 || Propage == ToAll)
					       DepBase (pRe1->ReBox, pBox, delta, frame);
					 }
				    }
				  /* Ignore la relation inverse de la boite elastique */
				  else if (pBox->BxVertFlex
					   && pPa1 != pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
				     ;
				  /* On ne decale pas les boites qui ont des relations  */
				  /* hors-structure avec la boite deplacee et on ne met */
				  /* pas a jour les dimensions elastiques dess boites   */
				  /* liees a la boite deplacee si elles ont ete         */
				  /* traitees par DepYContenu.                        */

				  else if (depabsolu)
				    {
				       if (!pBox->BxVertFlex || aplacer)
					 {
					    /* le travail n'a pas ete fait dans DepYContenu */
					    /* Relation conforme a la structure sur l'origine de boite */
					    if (pRe1->ReOp == OpVertDep && !pRe1->ReBox->BxYOutOfStruct)
					       if (pRe1->ReBox->BxVertFlex
					       /* si la boite n'est pas une boite fille */
						   && pPa1 != pRe1->ReBox->BxAbstractBox->AbEnclosing
						   && pPa1 == pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
						  ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, FALSE);
					       else
						  DepOrgY (pRe1->ReBox, pBox, delta, frame);
					 }
				    }
				  else if (pRe1->ReOp == OpVertDep && !pRe1->ReBox->BxVertFlex)
				     DepOrgY (pRe1->ReBox, pBox, delta, frame);
				  else if ((pRe1->ReOp == OpVertDep
					    && pPa1 == pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
					   || pRe1->ReOp == OpHeight)
				     ChngBElast (pRe1->ReBox, pBox, pRe1->ReOp, delta, frame, FALSE);
			       }

			     /* On a pu detruire le chainage des boites deplacees */
			     pBox->BxMoved = prec;
			     i++;
			     if (i <= MAX_RELAT_POS)
				nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			  }
			adpos = adpos->PosRNext;	/* Bloc suivant */
		     }

		/* Si le calcul de la hauteur de la boite englobante est a refaire */
		pAb = pPa1->AbEnclosing;
		if (deplace && pBox->BxYOutOfStruct && pAb != NULL)
		   /* On ne peut traiter l'englobement d'une boite si cette boite */
		   /* est en cours de placement ou si ce traitement est differe   */
		   if (!pAb->AbBox->BxYToCompute
		       && !Parent (pAb->AbBox, Englobement)
		   /* si org est une fille il faut la propagation a toutes */
		       && (Propage == ToAll || !Parent (pAb->AbBox, prec))
		   /* et si l'englobement n'est pas fait par une boite soeur */
		       && !Soeur (pBox, prec)
		   /* et si la boite deplacee est effectivement englobee */
		       && pPa1->AbVertEnclosing)
		      Engloby (pAb, prec, frame);
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |    Englobx verifie l'inclusion en largeur des boites englobees dans| */
/* |            le pave pAb suite au changement de largeur de la     | */
/* |            boite origine org.                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Englobx (PtrAbstractBox pAb, PtrBox org, int frame)
#else  /* __STDC__ */
void                Englobx (pAb, org, frame)
PtrAbstractBox             pAb;
PtrBox            org;
int                 frame;

#endif /* __STDC__ */
{
   int                 val, larg;
   int                 x, i, j, k;
   PtrAbstractBox             pavefils;
   PtrBox            box1;
   PtrBox            box2;
   boolean             nonvide;
   boolean             deplace;
   boolean             depabsolu;
   PtrBox            pBo1;
   AbDimension       *pPavD1;
   PtrAbstractBox             pPa3;
   PtrAbstractBox             cepave;
   AbPosition        *pPavP1;

   /* On verifie que la largeur de la boite depend du contenu et qu'on */
   /* n'est pas deja en train de traiter l'englobement de cette boite  */
   /* ATTENTION: le champs BxNPixels a une signification differente      */
   /* dans les boites de paves composes des boites de texte */

   pBo1 = pAb->AbBox;
   pPavD1 = &pAb->AbWidth;
   if (pBo1->BxContentWidth && pBo1->BxType != BoGhost)
     {
	/* On indique que l'englobement horizontal est en cours de traitement */
	pBo1->BxNPixels += 1;

	/* On verifie l'encadrement et l'englobement */
	/* Faut-il prendre l'origine en relatif ou en absolu ? */
	depabsolu = XEnAbsolu (pBo1);
	if (depabsolu)
	   x = pBo1->BxXOrg;
	else
	   x = 0;

	/* Initialise la position extreme droite sur le cote gauche actuel */
	larg = x;
	/* Initialise la position extreme gauche sur le cote droit actuel */
	val = x + pBo1->BxWidth;
	nonvide = FALSE;
	deplace = FALSE;	/* on n'a rien deplace */

	/* Le cote gauche de la plus a gauche des boites englobees doit */
	/* etre colle au cote gauche de la boite englobante et la       */
	/* largeur de la boite englobante est delimitee par le cote     */
	/* droit le plus a droite des boites englobees.                 */

	pavefils = pAb->AbFirstEnclosed;
	while (pavefils != NULL)
	  {
	     box1 = pavefils->AbBox;
	     if (!pavefils->AbDead && box1 != NULL
		 && pavefils->AbHorizEnclosing
		 && pavefils->AbWidth.DimAbRef != pAb)
	       {
		  /* Recherche la boite dont elle depend */
		  box2 = BoiteHInclus (box1, NULL);
		  if (box2 != NULL)
		     if (box2->BxAbstractBox != NULL)
		       {
			  pPa3 = box2->BxAbstractBox;
			  /* note le pave dont depend la largeur de pavefils */
			  cepave = pavefils->AbWidth.DimAbRef;
			  if (pPa3->AbHorizPos.PosAbRef == NULL)
			    {
			       /* Box est mobile, on tient compte de son origine */
			       nonvide = TRUE;
			       if (box1->BxXOrg < val)
				  val = box1->BxXOrg;	/* valeur minimum */
			       i = box1->BxXOrg + box1->BxWidth;
			    }
			  /* La position de la boite depend de la taille de l'englobante ? */
			  else if (pPa3->AbHorizPos.PosAbRef == pAb
				&& (pPa3->AbHorizPos.PosRefEdge != Left
				    || (pPa3->AbWidth.DimAbRef == pAb
					&& !pPa3->AbWidth.DimIsPosition
			  && pavefils->AbHorizPos.PosRefEdge != Left)))
			     i = x + box1->BxWidth;
			  /* La taille de la boite depend d'une boite externe ? */
			  else if (box1->BxWOutOfStruct)
			    {
			       /* qui herite de la taille de l'englobante ? */
			       if (Parent (cepave->AbBox, pBo1)
				   && cepave->AbWidth.DimAbRef == NULL
				   && cepave->AbWidth.DimValue == 0
				&& cepave->AbBox->BxWidth == pBo1->BxWidth)
				  i = x;
			       else
				  /* On evalue l'encadrement et l'englobement */
			       if (box1->BxXOrg < x)
				  i = x + box1->BxWidth;
			       else
				  i = box1->BxXOrg + box1->BxWidth;
			    }
			  else
			     /* On evalue l'encadrement et l'englobement */
			  if (box1->BxXOrg < x)
			     i = x + box1->BxWidth;
			  else
			     i = box1->BxXOrg + box1->BxWidth;
			  if (i > larg)
			     larg = i;
		       }
	       }
	     pavefils = pavefils->AbNext;
	  }

	val = -val + x;		/* Decalage de la position extreme gauche */
	if (nonvide)
	   larg += val;		/* Nouvelle position extreme droite */
	if (larg == x && pAb->AbVolume == 0)
	   EvalText (pAb, &larg, &x, &i);
	else
	   larg -= x;
	x = larg - pBo1->BxWidth;	/* Difference de largeur */

	/* Faut-il deplacer les boites englobees ? */
	pavefils = pAb->AbFirstEnclosed;
	if (nonvide && val != 0)
	   while (pavefils != NULL)
	     {
		box1 = pavefils->AbBox;
		if (!pavefils->AbDead && box1 != NULL && pavefils->AbHorizEnclosing)
		  {

		     /* Est-ce une boite mobile a deplacer ? */
		     /* Recherche la boite dont elle depend */
		     box2 = BoiteHInclus (box1, NULL);
		     if (box2 != NULL)
			if (box2->BxAbstractBox != NULL)
			   if (box2->BxAbstractBox->AbHorizPos.PosAbRef == NULL)
			     {
				/* On met a jour la zone de reaffichage */
				if (EvalAffich)
				  {
				     /* Prend en compte une zone de debordement des graphiques */
				     if (box1->BxAbstractBox->AbLeafType == LtGraphics)
					k = EXTRA_GRAPH;
				     else
					k = 0;
				     i = box1->BxXOrg;
				     j = box1->BxXOrg + box1->BxWidth;
				     if (val > 0)
					j += val;
				     else
					i += val;
				     DefClip (frame, i - k, box1->BxYOrg - k, j + k, box1->BxYOrg + box1->BxHeight + k);
				  }

				if (XEnAbsolu (box1))
				   DepXContenu (box1, val, frame);
				else
				   box1->BxXOrg += val;

				/* Est-ce que cette boite deplace l'axe de l'englobante ? */
				pPavP1 = &pAb->AbVertRef;
				if (pPavP1->PosAbRef == pavefils)
				  {
				     deplace = TRUE;
				     box1->BxMoved = NULL;
				     /* Est-ce que la relation porte sur l'axe de reference ? */
				     if (pPavP1->PosRefEdge != VertRef)
					DepAxe (pBo1, box1, val, frame);
				     /* Recalcule la position de l'axe */
				     else
				       {
					  if (pPavP1->PosUnit == UnPercent)
					     i = PixelValue (pPavP1->PosDistance, UnPercent, (PtrAbstractBox) pAb->AbBox->BxWidth);
					  else
					     i = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit, pAb);
					  i = i + box1->BxXOrg + box1->BxVertRef - pBo1->BxXOrg;
					  DepAxe (pBo1, box1, i - pBo1->BxVertRef, frame);
				       }
				  }
			     }
		  }
		pavefils = pavefils->AbNext;
	     }

	/* Faut-il changer la largeur de la boite englobante ? */
	if (x != 0)
	   ChangeLgContenu (pBo1, org, larg, 0, frame);
	/* Faut-il verifier l'englobement au dessus ? */
	else if (deplace)
	   if (pAb->AbEnclosing == NULL)
	     {
		if (pBo1->BxXOrg < 0)
		   DepXContenu (pBo1, -pBo1->BxXOrg, frame);
	     }
	   else if (!pAb->AbEnclosing->AbInLine
		    && pAb->AbEnclosing->AbBox->BxType != BoGhost)
	      Englobx (pAb->AbEnclosing, org, frame);

	/* Indique que le traitement de l'englobement horizontal est termine */
	pBo1->BxNPixels -= 1;
     }
   /* Si la boite prend la largeur minimum, il faut quand meme      */
   /* evaluer la largeur du contenu et verifier la regle du minimum */
   else if (!pPavD1->DimIsPosition && pPavD1->DimMinimum
	    && pBo1->BxType != BoGhost)
     {
	EvalComp (pAb, frame, &larg, &val);
	ChangeLgContenu (pBo1, org, larg, 0, frame);
     }
}

/* ---------------------------------------------------------------------- */
/* |        Engloby  verifie l'inclusion en hauteur des boites englobees| */
/* |           dans le pave pAb suite au changement de hauteur de la | */
/* |           boite origine org.                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Engloby (PtrAbstractBox pAb, PtrBox org, int frame)
#else  /* __STDC__ */
void                Engloby (pAb, org, frame)
PtrAbstractBox             pAb;
PtrBox            org;
int                 frame;

#endif /* __STDC__ */
{
   int                 val, haut;
   int                 y, i, j, k;
   PtrAbstractBox             pavefils;
   PtrBox            box1;
   PtrBox            box2;
   boolean             nonvide;
   boolean             deplace;
   boolean             depabsolu;
   PtrBox            pBo1;
   AbDimension       *pPavD1;
   PtrAbstractBox             pPa3;
   PtrAbstractBox             cepave;
   AbPosition        *pPavP1;

   /* On verifie que la hauteur de la boite depend du contenu et qu'on */
   /* n'est pas deja en train de traiter l'englobement de cette boite  */
   /* ATTENTION: le champs BxSpaceWidth a une signification differente      */
   /* dans les boites de paves composes des boites de texte            */

   pBo1 = pAb->AbBox;
   pPavD1 = &pAb->AbHeight;
   if (pBo1->BxContentHeight && pBo1->BxType != BoGhost)
     {

	/* On indique que l'englobement vertical est en cours de traitement */
	pBo1->BxSpaceWidth += 1;

	/* On verifie l'encadrement et l'englobement */
	/* Faut-il prendre l'origine en relatif ou en absolu ? */
	depabsolu = YEnAbsolu (pBo1);
	if (depabsolu)
	   y = pBo1->BxYOrg;
	else
	   y = 0;

	/* Initialise la position extreme basse sur le cote superieur actuel */
	haut = y;
	/* Initialise la position extreme haute sur le cote inferieur actuel */
	val = y + pBo1->BxHeight;
	nonvide = FALSE;
	deplace = FALSE;	/* on n'a rien deplace */

	/* Le cote superieur de la plus haute boite englobee doit etre  */
	/* colle au cote superieur de la boite englobante et la hauteur */
	/* de la boite englobante est delimitee par le cote inferieur   */
	/* le plus bas des boites englobees.                          */

	pavefils = pAb->AbFirstEnclosed;
	while (pavefils != NULL)
	  {
	     box1 = pavefils->AbBox;
	     if (!pavefils->AbDead
		 && box1 != NULL
		 && pavefils->AbVertEnclosing
		 && pavefils->AbHeight.DimAbRef != pAb)
	       {
		  /* Recherche la boite dont elle depend */
		  box2 = BoiteVInclus (box1, NULL);
		  if (box2 != NULL)
		     if (box2->BxAbstractBox != NULL)
		       {
			  pPa3 = box2->BxAbstractBox;
			  /* note le pave dont depend la hauteur de pavefils */
			  cepave = pavefils->AbHeight.DimAbRef;
			  if (pPa3->AbVertPos.PosAbRef == NULL)
			    {
			       /* Box mobile, on tient compte de son origine */
			       nonvide = TRUE;
			       if (box1->BxYOrg < val)
				  val = box1->BxYOrg;	/* valeur minimum */
			       i = box1->BxYOrg + box1->BxHeight;
			    }
			  /* La position de la boite depend de la taille de englobante ? */
			  else if (pPa3->AbVertPos.PosAbRef == pAb
				   && (pPa3->AbVertPos.PosRefEdge != Top
				       || (pPa3->AbHeight.DimAbRef == pAb
					   && !pPa3->AbHeight.DimIsPosition
			     && pavefils->AbVertPos.PosRefEdge != Top)))
			     i = y + box1->BxHeight;
			  /* La taille de la boite depend d'une boite externe ? */
			  else if (box1->BxHOutOfStruct)
			    {
			       /* qui herite de la taille de l'englobante ? */
			       if (Parent (cepave->AbBox, pBo1)
				   && cepave->AbHeight.DimAbRef == NULL
				   && cepave->AbHeight.DimValue == 0
				&& cepave->AbBox->BxHeight == pBo1->BxHeight)
				  i = y;
			       else
				  /* On evalue l'encadrement et l'englobement */
			       if (box1->BxYOrg < y)
				  i = y + box1->BxHeight;
			       else
				  i = box1->BxYOrg + box1->BxHeight;
			    }
			  else
			     /* On evalue l'encadrement et l'englobement */
			  if (box1->BxYOrg < y)
			     i = y + box1->BxHeight;
			  else
			     i = box1->BxYOrg + box1->BxHeight;
			  if (i > haut)
			     haut = i;
		       }
	       }
	     pavefils = pavefils->AbNext;
	  }

	val = -val + y;		/* Decalage de la position extreme haute */
	if (nonvide)
	   haut += val;		/* Nouvelle position extreme basse */
	if (haut == y && pAb->AbVolume == 0)
	   EvalText (pAb, &y, &haut, &i);
	else
	   haut -= y;
	y = haut - pBo1->BxHeight;	/* Difference de hauteur */

	/* Faut-il deplacer les boites englobees ? */
	pavefils = pAb->AbFirstEnclosed;
	if (nonvide && val != 0)
	   while (pavefils != NULL)
	     {
		box1 = pavefils->AbBox;
		if (!pavefils->AbDead && box1 != NULL && pavefils->AbVertEnclosing)
		  {
		     /* Est-ce une boite mobile a deplacer ? */
		     /* Recherche la boite dont elle depend */
		     box2 = BoiteVInclus (box1, NULL);
		     if (box2 != NULL)
			if (box2->BxAbstractBox != NULL)
			   if (box2->BxAbstractBox->AbVertPos.PosAbRef == NULL)
			     {
				/* On met a jour la zone de reaffichage */
				if (EvalAffich)
				  {
				     i = box1->BxYOrg;
				     j = box1->BxYOrg + box1->BxHeight;
				     /* Prend en compte une zone de debordement des graphiques */
				     if (box1->BxAbstractBox->AbLeafType == LtGraphics)
					k = EXTRA_GRAPH;
				     else
					k = 0;
				     if (val > 0)
					j += val;
				     else
					i += val;
				     DefClip (frame, box1->BxXOrg - k, i - k, box1->BxXOrg
					      + box1->BxWidth + k, j + k);
				  }

				if (YEnAbsolu (box1))
				   DepYContenu (box1, val, frame);
				else
				   box1->BxYOrg += val;

				/* Est-ce que cette boite deplace la base de l'englobante ? */
				pPavP1 = &pAb->AbHorizRef;
				if (pPavP1->PosAbRef == pavefils)
				  {
				     deplace = TRUE;
				     box1->BxMoved = NULL;
				     /* Est-ce que la relation porte sur l'axe de reference ? */
				     if (pPavP1->PosRefEdge != HorizRef)
					DepBase (pBo1, box1, val, frame);
				     /* Recalcule la position de l'axe */
				     else
				       {
					  if (pPavP1->PosUnit == UnPercent)
					     i = PixelValue (pPavP1->PosDistance, UnPercent, (PtrAbstractBox) pAb->AbBox->BxHeight);
					  else
					     i = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit, pAb);
					  i = i + box1->BxYOrg + box1->BxHorizRef - pBo1->BxYOrg;
					  DepBase (pBo1, box1, i - pBo1->BxHorizRef, frame);
				       }
				  }
			     }
		  }
		pavefils = pavefils->AbNext;
	     }

	/* Faut-il changer la hauteur de la boite englobante ? */
	if (y != 0)
	   ChangeHtContenu (pBo1, org, haut, frame);
	/* Faut-il verifier l'englobement au dessus ? */
	else if (deplace)
	   if (pAb->AbEnclosing == NULL)
	     {
		if (pBo1->BxYOrg < 0)
		   DepYContenu (pBo1, -pBo1->BxYOrg, frame);
	     }
	   else if (pAb->AbEnclosing->AbInLine)
	      EnglLigne (pBo1, frame, pAb->AbEnclosing);
	   else if (pAb->AbEnclosing->AbBox->BxType == BoGhost)
	     {
		/* Il faut remonter au pave de mise en lignes */
		while (pAb->AbEnclosing->AbBox->BxType == BoGhost)
		   pAb = pAb->AbEnclosing;
		EnglLigne (pBo1, frame, pAb->AbEnclosing);
	     }
	   else
	      Engloby (pAb->AbEnclosing, org, frame);

	/* On indique que le traitement de l'englobement vertical est termine */
	pBo1->BxSpaceWidth -= 1;
     }
   /* Si la boite prend la hauteur minimum, il faut quand meme      */
   /* evaluer la hauteur du contenu et verifier la regle du minimum */
   else if (!pPavD1->DimIsPosition && pPavD1->DimMinimum
	    && pBo1->BxType != BoGhost)
     {
	EvalComp (pAb, frame, &val, &haut);
	ChangeHtContenu (pBo1, org, haut, frame);
     }
}
/* End Of Module dep */
