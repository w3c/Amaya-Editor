
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
/* |        Parent retourne vrai si ibox est une englobante de la boite | */
/* |                cebox.                                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             Parent (PtrBox ibox, PtrBox cebox)
#else  /* __STDC__ */
boolean             Parent (ibox, cebox)
PtrBox            ibox;
PtrBox            cebox;

#endif /* __STDC__ */
{
   PtrAbstractBox             pave;
   boolean             egal;
   boolean             parent;

   if (cebox == NULL || ibox == NULL)
      parent = False;
   else
     {
	/* Recherche dans la parente de cebox y compris elle-meme */
	pave = cebox->BxAbstractBox;
	egal = False;
	while (!egal && pave != NULL)
	  {
	     egal = pave->AbBox == ibox;
	     pave = pave->AbEnclosing;
	  }
	parent = egal;
     }
   return parent;
}

/* ---------------------------------------------------------------------- */
/* |        Soeur retourne vrai si ibox a le meme pave pere que la      | */
/* |                boite cebox et n'est pas la boite ibox.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      Soeur (PtrBox ibox, PtrBox cebox)
#else  /* __STDC__ */
static boolean      Soeur (ibox, cebox)
PtrBox            ibox;
PtrBox            cebox;

#endif /* __STDC__ */
{

   if (cebox == ibox)
      return (False);
   else if (cebox == NULL)
      return (False);
   else if (ibox == NULL)
      return (False);
   else if (cebox->BxAbstractBox->AbEnclosing == ibox->BxAbstractBox->AbEnclosing)
      return (True);
   else
      return (False);
}


/* ---------------------------------------------------------------------- */
/* |        XEnAbsolu retourne vrai si on est pas dans une cre'ation    | */
/* |            initiale ou si une boi^te englobante de la boi^te ibox  | */
/* |            est e'lastique horizontalement ou si elle a un          | */
/* |            positionnement horizontal hors-structure.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             XEnAbsolu (PtrBox ibox)
#else  /* __STDC__ */
boolean             XEnAbsolu (ibox)
PtrBox            ibox;

#endif /* __STDC__ */
{
   PtrBox            box1;
   boolean             Ok;

   box1 = ibox;
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
/* |            initiale ou si une boi^te englobante de la boi^te ibox  | */
/* |            est e'lastique verticalement ou si elle a un            | */
/* |            positionnement vertical hors-structure.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             YEnAbsolu (PtrBox ibox)
#else  /* __STDC__ */
boolean             YEnAbsolu (ibox)
PtrBox            ibox;

#endif /* __STDC__ */
{
   PtrBox            box1;
   boolean             Ok;

   box1 = ibox;
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
/* |            EnHorizontal et EnVertical pour la boi^te ibox.         | */
/* |            L'indicateur est vrai si on est pas dans une cre'ation  | */
/* |            initiale ou si une boi^te englobante de la boi^te ibox  | */
/* |            est e'lastique horizontalement (verticalement) ou si    | */
/* |            une boi^te englobante a une relation hors-structure.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                XYEnAbsolu (PtrBox ibox, boolean * EnHorizontal, boolean * EnVertical)
#else  /* __STDC__ */
void                XYEnAbsolu (ibox, EnHorizontal, EnVertical)
PtrBox            ibox;
boolean            *EnHorizontal;
boolean            *EnVertical;

#endif /* __STDC__ */
{
   PtrBox            box1;

   box1 = ibox;
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

   if (ibox->BxType == BoBlock && ibox->BxYToCompute)
      /* C'est trop tot pour placer le contenu du bloc de lignes en Y */
      *EnVertical = False;
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
static void         PolyInverse (PtrAbstractBox adpave, boolean InversionHoriz, boolean InversionVert, boolean Saisie)
#else  /* __STDC__ */
static void         PolyInverse (adpave, InversionHoriz, InversionVert, Saisie)
PtrAbstractBox             adpave;
boolean             InversionHoriz;
boolean             InversionVert;
boolean             Saisie;

#endif /* __STDC__ */
{
   PtrTextBuffer      adbuffs, adbuffd;
   PtrBox            ibox;
   int                 maxX, maxY;
   int                 val, i, j;
   float               rapportX, rapportY;
   int                 lepoint;

   ibox = adpave->AbBox;
   j = 1;
   if (Saisie)
     {
	/* maj des reperes du pave a partir de la boite */
	adbuffs = ibox->BxBuffer;
	adbuffd = adpave->AbPolyLineBuffer;
	rapportX = ibox->BxXRatio;
	rapportY = ibox->BxYRation;
     }
   else
     {
	/* maj des reperes de la boite a partir du pave */
	adbuffs = adpave->AbPolyLineBuffer;
	adbuffd = ibox->BxBuffer;
	/* annule les deformations precedentes */
	rapportX = 1;
	rapportY = 1;
	ibox->BxXRatio = 1;
	ibox->BxYRation = 1;
	adbuffd->BuPoints[0].XCoord = adbuffs->BuPoints[0].XCoord;
	adbuffd->BuPoints[0].YCoord = adbuffs->BuPoints[0].YCoord;
     }

   /* inversions eventuelles par rapport aux milieux */
   maxX = adbuffs->BuPoints[0].XCoord;
   maxY = adbuffs->BuPoints[0].YCoord;
   val = ibox->BxNChars;
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

   if (ibox->BxImageDescriptor != NULL && !Saisie)
     {
	/* le calcul des points de controle doit etre reexecute */
	free ((char *) ibox->BxImageDescriptor);
	ibox->BxImageDescriptor = NULL;
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
void                AjusteTrace (PtrAbstractBox adpave, boolean InversionHoriz, boolean InversionVert, boolean Saisie)
#else  /* __STDC__ */
void                AjusteTrace (adpave, InversionHoriz, InversionVert, Saisie)
PtrAbstractBox             adpave;
boolean             InversionHoriz;
boolean             InversionVert;
boolean             Saisie;

#endif /* __STDC__ */
{
   PtrAbstractBox             pavefils;
   char                car;

   if (adpave->AbLeafType == LtCompound && !Saisie)
     {
	/* Transmet le traitement aux paves fils */
	pavefils = adpave->AbFirstEnclosed;
	while (pavefils != NULL)
	  {
	     if (pavefils->AbLeafType == LtGraphics
		 || pavefils->AbLeafType == LtPlyLine
		 || pavefils->AbLeafType == LtCompound)
		AjusteTrace (pavefils, InversionHoriz, InversionVert, Saisie);
	     pavefils = pavefils->AbNext;
	  }
     }
   else if (adpave->AbLeafType == LtPlyLine)
      PolyInverse (adpave, InversionHoriz, InversionVert, Saisie);
   else if (adpave->AbLeafType == LtGraphics)
     {
	/* Ajuste le graphique de la boite */
	if (Saisie)
	   /* le caractere de reference est le trace reel */
	   car = adpave->AbRealShape;
	else
	   /* le caractere de reference est le trace de l'element */
	   car = adpave->AbShape;

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
	   adpave->AbShape = car;
	else
	   /* le caractere de reference est le trace de l'element */
	   adpave->AbRealShape = car;
     }
}


/* ---------------------------------------------------------------------- */
/* |        HorizInverse inverse les reperes horizontaux position et    | */
/* |            dimension de la boite elastique ibox. Si la boite est   | */
/* |            une boite graphique les dessins sont modifies.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                HorizInverse (PtrBox ibox, OpRelation op)
#else  /* __STDC__ */
void                HorizInverse (ibox, op)
PtrBox            ibox;
OpRelation          op;

#endif /* __STDC__ */
{
   PtrAbstractBox             adpave;
   PtrPosRelations      adpos;
   int                 i;
   BoxEdge         rp1;
   BoxEdge         rp2;
   BoxEdge         rd1;
   BoxEdge         rd2;
   PtrBox            boxpos;

   adpave = ibox->BxAbstractBox;

   /* On prend le repere symetrique dans le positionnement */
   rp1 = adpave->AbHorizPos.PosEdge;	/* Ancien repere de position */
   rp2 = rp1;			/* Nouveau repere de position */
   rd1 = rp1;			/* Ancien repere de dimension */
   rd2 = rp1;			/* Nouveau repere de dimension */
   if (rp1 == Left)
     {
	rp2 = Right;
	adpave->AbHorizPos.PosEdge = rp2;
	rd1 = Right;
	rd2 = Left;
	adpave->AbWidth.DimPosition.PosEdge = rd2;
     }
   else if (rp1 == Right)
     {
	rp2 = Left;
	adpave->AbHorizPos.PosEdge = rp2;
	rd1 = Left;
	rd2 = Right;
	adpave->AbWidth.DimPosition.PosEdge = rd2;
     }

   /* Mise a jour du point fixe de la boite */
   if (op == OpWidth && ibox->BxHorizEdge == rp1)
      ibox->BxHorizEdge = rp2;
   else if (op == OpHorizDep && ibox->BxHorizEdge == rd1)
      ibox->BxHorizEdge = rd2;

   /* Les reperes sont inverses */
   ibox->BxHorizInverted = !ibox->BxHorizInverted;

   /* Modifie enventuellement le dessin de la boite */
   if (rp1 != rp2)
      AjusteTrace (adpave, ibox->BxHorizInverted, ibox->BxVertInverted, False);

   /* Note la boite qui fixe la position de la boite elastique */
   if (adpave->AbHorizPos.PosAbRef == NULL)
      boxpos = NULL;
   else
      boxpos = adpave->AbHorizPos.PosAbRef->AbBox;

   /* Met a jour les relations de la boite */
   i = 1;
   adpos = ibox->BxPosRelations;
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
/* |            de la boite elastique ibox. Si la boite est une boite   | */
/* |            graphique, les dessins sont modifies.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                VertInverse (PtrBox ibox, OpRelation op)
#else  /* __STDC__ */
void                VertInverse (ibox, op)
PtrBox            ibox;
OpRelation          op;

#endif /* __STDC__ */
{
   PtrAbstractBox             adpave;
   PtrPosRelations      adpos;
   int                 i;
   BoxEdge         rp1;
   BoxEdge         rp2;
   BoxEdge         rd1;
   BoxEdge         rd2;
   PtrBox            boxpos;

   adpave = ibox->BxAbstractBox;

   /* On prend le repere symetrique dans le positionnement */
   rp1 = adpave->AbVertPos.PosEdge;
   rp2 = rp1;			/* Nouveau repere de position */
   rd1 = rp1;			/* Ancien repere de dimension */
   rd2 = rp1;			/* Nouveau repere de dimension */
   if (rp1 == Top)
     {
	rp2 = Bottom;
	adpave->AbVertPos.PosEdge = rp2;
	rd1 = Bottom;
	rd2 = Top;
	adpave->AbHeight.DimPosition.PosEdge = rd2;
     }
   else if (rp1 == Bottom)
     {
	rp2 = Top;
	adpave->AbVertPos.PosEdge = rp2;
	rd1 = Top;
	rd2 = Bottom;
	adpave->AbHeight.DimPosition.PosEdge = rd2;
     }

   /* Mise a jour du point fixe de la boite */
   if (op == OpHeight && ibox->BxVertEdge == rp1)
      ibox->BxVertEdge = rp2;
   else if (op == OpVertDep && ibox->BxVertEdge == rd1)
      ibox->BxVertEdge = rd2;

   /* Les reperes sont inverses */
   ibox->BxVertInverted = !ibox->BxVertInverted;

   /* Modifie enventuellement le dessin de la boite */
   if (rp1 != rp2)
/**PL*/ AjusteTrace (adpave, ibox->BxHorizInverted, ibox->BxVertInverted, False);

   /* Note la boite qui fixe la position de la boite elastique */
   if (adpave->AbVertPos.PosAbRef == NULL)
      boxpos = NULL;
   else
      boxpos = adpave->AbVertPos.PosAbRef->AbBox;

   /* Met a jour les relations de la boite */
   i = 1;
   adpos = ibox->BxPosRelations;
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
void                ChangeLgContenu (PtrBox ibox, PtrBox org, int large, int dbl, int frame)
#else  /* __STDC__ */
void                ChangeLgContenu (ibox, org, large, dbl, frame)
PtrBox            ibox;
PtrBox            org;
int                 large;
int                 dbl;
int                 frame;

#endif /* __STDC__ */
{
   int                 delta;

   if (ibox != NULL)
     {
	/* Regarde si la largeur reelle actuelle depend du contenu */
	if (ibox->BxContentWidth)
	  {
	     /* La largeur reelle est egale a la largeur du contenu */
	     if (large < ibox->BxRuleWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  delta = ibox->BxRuleWidth - ibox->BxWidth;
		  ibox->BxRuleWidth = large;
		  ibox->BxContentWidth = !ibox->BxContentWidth;
		  if (delta != 0)
		     ModLarg (ibox, org, NULL, delta, dbl, frame);
	       }
	     else
		/* Mise a jour de la largeur du contenu */
		ModLarg (ibox, org, NULL, large - ibox->BxWidth, dbl, frame);
	  }
	else if (!ibox->BxAbstractBox->AbWidth.DimIsPosition && ibox->BxAbstractBox->AbWidth.DimMinimum)
	  {
	     /* La largeur reelle est egale au minimum */
	     if (large > ibox->BxWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  ibox->BxRuleWidth = ibox->BxWidth;
		  ibox->BxContentWidth = !ibox->BxContentWidth;
		  ModLarg (ibox, org, NULL, large - ibox->BxWidth, dbl, frame);
	       }
	     else
		/* Mise a jour de la largeur du contenu */
		ibox->BxRuleWidth = large;
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
void                ChangeHtContenu (PtrBox ibox, PtrBox org, int haut, int frame)
#else  /* __STDC__ */
void                ChangeHtContenu (ibox, org, haut, frame)
PtrBox            ibox;
PtrBox            org;
int                 haut;
int                 frame;

#endif /* __STDC__ */
{
   int                 delta;

   if (ibox != NULL)
     {
	/* Regarde si la hauteur reelle actuelle depend du contenu */
	if (ibox->BxContentHeight)
	  {
	     /* La hauteur reelle est egale a la hauteur du contenu */
	     if (haut < ibox->BxRuleHeigth)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  delta = ibox->BxRuleHeigth - ibox->BxHeight;
		  ibox->BxRuleHeigth = haut;
		  ibox->BxContentHeight = !ibox->BxContentHeight;
		  ModHaut (ibox, org, NULL, delta, frame);
	       }
	     else
		/* Mise a jour de la hauteur du contenu */
		ModHaut (ibox, org, NULL, haut - ibox->BxHeight, frame);
	  }
	else if (!ibox->BxAbstractBox->AbHeight.DimIsPosition && ibox->BxAbstractBox->AbHeight.DimMinimum)
	  {
	     /* La hauteur reelle est egale au minimum */
	     if (haut > ibox->BxHeight)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  ibox->BxRuleHeigth = ibox->BxHeight;
		  ibox->BxContentHeight = !ibox->BxContentHeight;
		  ModHaut (ibox, org, NULL, haut - ibox->BxHeight, frame);
	       }
	     else
		/* Mise a jour de la hauteur du contenu */
		ibox->BxRuleHeigth = haut;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |        ChangeLargeur propage la modification sur la largeur de la  | */
/* |            boi^te ibox. Cette proce'dure ve'rifie la re`gle du     | */
/* |            minimum. La largeur re'elle ou l'autre largeur de la    | */
/* |            boi^te sera modifie'e.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangeLargeur (PtrBox ibox, PtrBox org, PtrBox prec, int delta, int dbl, int frame)
#else  /* __STDC__ */
void                ChangeLargeur (ibox, org, prec, delta, dbl, frame)
PtrBox            ibox;
PtrBox            org;
PtrBox            prec;
int                 delta;
int                 dbl;
int                 frame;

#endif /* __STDC__ */
{
   int                 large;

   if (ibox != NULL)
     {
	/* Regarde si la largeur reelle actuelle depend du contenu */
	if (ibox->BxContentWidth)
	  {
	     /* Il y a une regle de minimum mais ce n'est la largeur reelle */
	     large = ibox->BxRuleWidth + delta;
	     if (large > ibox->BxWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  ibox->BxRuleWidth = ibox->BxWidth;
		  ibox->BxContentWidth = !ibox->BxContentWidth;
		  ModLarg (ibox, org, prec, large - ibox->BxWidth, dbl, frame);
	       }
	     else
		/* Mise a jour de la largeur minimum */
		ibox->BxRuleWidth = large;
	  }
	else if (!ibox->BxAbstractBox->AbWidth.DimIsPosition && ibox->BxAbstractBox->AbWidth.DimMinimum)
	  {
	     /* La largeur reelle est egale au minimum */
	     large = ibox->BxWidth + delta;
	     if (large < ibox->BxRuleWidth)
	       {
		  /* Il faut echanger la largeur reelle avec l'autre largeur */
		  large = ibox->BxRuleWidth;
		  ibox->BxRuleWidth = ibox->BxWidth + delta;
		  ibox->BxContentWidth = !ibox->BxContentWidth;
		  ModLarg (ibox, org, prec, large - ibox->BxWidth, dbl, frame);
	       }
	     else
		/* Mise a jour de la largeur minimum */
		ModLarg (ibox, org, prec, delta, dbl, frame);
	  }
	else
	   ModLarg (ibox, org, prec, delta, dbl, frame);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChangeHauteur propage la modification sur la hauteur de la      | */
/* |            boi^te ibox. Cette proce'dure ve'rifie la re`gle du     | */
/* |            minimum. La hauteur re'elle ou l'autre hauteur de la    | */
/* |            boi^te sera modifie'e.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChangeHauteur (PtrBox ibox, PtrBox org, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                ChangeHauteur (ibox, org, prec, delta, frame)
PtrBox            ibox;
PtrBox            org;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   int                 haut;

   if (ibox != NULL)
     {
	/* Regarde si la hauteur reelle actuelle depend du contenu */
	if (ibox->BxContentHeight)
	  {
	     /* Il y a une regle de minimum mais ce n'est la hauteur reelle */
	     haut = ibox->BxRuleHeigth + delta;
	     if (haut > ibox->BxHeight)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  ibox->BxRuleHeigth = ibox->BxHeight;
		  ibox->BxContentHeight = !ibox->BxContentHeight;
		  ModHaut (ibox, org, prec, haut - ibox->BxHeight, frame);
	       }
	     else
		/* Mise a jour de la hauteur minimum */
		ibox->BxRuleHeigth = haut;
	  }
	else if (!ibox->BxAbstractBox->AbHeight.DimIsPosition && ibox->BxAbstractBox->AbHeight.DimMinimum)
	  {
	     /* La hauteur courante est egale au minimum */
	     haut = ibox->BxHeight + delta;
	     if (haut < ibox->BxRuleHeigth)
	       {
		  /* Il faut echanger la hauteur reelle avec l'autre hauteur */
		  haut = ibox->BxRuleHeigth;
		  ibox->BxRuleHeigth = ibox->BxHeight + delta;
		  ibox->BxContentHeight = !ibox->BxContentHeight;
		  ModHaut (ibox, org, prec, haut - ibox->BxHeight, frame);
	       }
	     else
		ModHaut (ibox, org, prec, delta, frame);
	  }
	else
	   ModHaut (ibox, org, prec, delta, frame);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChngBElast inverse les reperes et eventuellement le dessin de   | */
/* |            la boite elastique ibox quand la dimension devient      | */
/* |            negative.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChngBElast (PtrBox ibox, PtrBox org, OpRelation op, int delta, int frame, boolean EnX)
#else  /* __STDC__ */
void                ChngBElast (ibox, org, op, delta, frame, EnX)
PtrBox            ibox;
PtrBox            org;
OpRelation          op;
int                 delta;
int                 frame;
boolean             EnX;

#endif /* __STDC__ */
{
   PtrAbstractBox             adpave;
   BoxEdge         rp1;
   int                 trans;

   adpave = ibox->BxAbstractBox;
   trans = 0;
   /* Il faut verifier que l'on pas deja en train de traiter cette boite */
   if (adpave != NULL && delta != 0 && ibox->BxSpaceWidth == 0)
     {
	/* On indique que le traitement est en cours */
	ibox->BxSpaceWidth = 1;
	if (EnX)
	  {
	     /* Recherche le repere deplace et le repere fixe de la boite elastique */
	     if (op == OpWidth)
		rp1 = adpave->AbWidth.DimPosition.PosEdge;

	     else
	       {
		  rp1 = adpave->AbHorizPos.PosEdge;
		  /* Met a jour la pile des boites pour ne pas deplacer org */
		  ibox->BxMoved = org;
	       }

	     /* On calcule le changement de largeur */
	     if (rp1 == Left)
	       {
		  delta = -delta;
		  ibox->BxHorizEdge = Right;
	       }
	     else if (rp1 == Right)
		ibox->BxHorizEdge = Left;
	     else if (rp1 == VertMiddle)
	       {
		  delta = delta * 2;
		  if (op == OpHorizDep)
		     ibox->BxHorizEdge = adpave->AbWidth.DimPosition.PosEdge;
		  else
		     ibox->BxHorizEdge = adpave->AbHorizPos.PosEdge;
	       }

	     /* Faut-il echanger les reperes horizontaux ? */
	     if (delta < 0 && -delta > ibox->BxWidth)
	       {
		  /* Inversion de la boite en horizontal */
		  HorizInverse (ibox, op);

		  /* Translation de l'origine */
		  delta = -delta - 2 * ibox->BxWidth;
		  if (ibox->BxHorizEdge == Right)
		     trans = -ibox->BxWidth;
		  else if (ibox->BxHorizEdge == Left)
		     trans = ibox->BxWidth;
		  DepOrgX (ibox, org, trans, frame);
	       }
	     ModLarg (ibox, org, NULL, delta, 0, frame);

	     /* On retablit le point fixe */
	     ibox->BxHorizEdge = NoEdge;
	  }
	else
	  {
	     /* Recherche le repere deplace et le repere fixe de la boite elastique */
	     if (op == OpHeight)
		rp1 = adpave->AbHeight.DimPosition.PosEdge;

	     else
	       {
		  rp1 = adpave->AbVertPos.PosEdge;
		  /* Met a jour la pile des boites pour ne pas deplacer org */
		  ibox->BxMoved = org;
	       }

	     /* On calcule le changement de hauteur */
	     if (rp1 == Top)
	       {
		  delta = -delta;
		  ibox->BxVertEdge = Bottom;
	       }
	     else if (rp1 == Bottom)
		ibox->BxVertEdge = Top;
	     else if (rp1 == HorizMiddle)
	       {
		  delta = delta * 2;
		  if (op == OpVertDep)
		     ibox->BxVertEdge = adpave->AbHeight.DimPosition.PosEdge;
		  else
		     ibox->BxVertEdge = adpave->AbVertPos.PosEdge;
	       }

	     /* Faut-il echanger les reperes verticaux ? */
	     if (delta < 0 && -delta > ibox->BxHeight)
	       {
		  /* Inversion de la boite en vertical */
		  VertInverse (ibox, op);

		  /* Translation de l'origine */
		  delta = -delta - 2 * ibox->BxHeight;
		  if (ibox->BxVertEdge == Bottom)
		     trans = -ibox->BxHeight;
		  else if (ibox->BxVertEdge == Top)
		     trans = ibox->BxHeight;
		  DepOrgY (ibox, org, trans, frame);

	       }
	     ModHaut (ibox, org, NULL, delta, frame);
	     /* On retablit le point fixe */
	     ibox->BxVertEdge = NoEdge;
	  }
	/* On indique que le traitement est termine */
	ibox->BxSpaceWidth = 0;
	ibox->BxMoved = org;	/* retablit le chainage des deplacements */
     }
}


/* ---------------------------------------------------------------------- */
/* |    DepXContenu deplace horizontalement le contenu englobe de la    | */
/* |            boite ibox.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepXContenu (PtrBox ibox, int delta, int frame)
#else  /* __STDC__ */
void                DepXContenu (ibox, delta, frame)
PtrBox            ibox;
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

   if (ibox != NULL && (delta != 0 || ibox->BxXToCompute))
     {

	/* On enregistre la hierarchie des boites dont le */
	/* traitement de l'englobement doit etre differe  */
	SaveEnglob = Englobement;
	if (ibox->BxAbstractBox->AbEnclosing != NULL)
	   Englobement = ibox /*->BxAbstractBox->AbEnclosing->AbBox*/ ;

	/* Si Englobement est une boite mere de la boite   */
	/* precedemment designee on garde l'ancienne boite */
	if (Parent (Englobement, SaveEnglob))
	   Englobement = SaveEnglob;

	/* Si la boite est mise en lignes */
	if (ibox->BxType == BoSplit)
	  {
	     box1 = ibox->BxNexChild;	/* On decale les boites de coupure */
	     while (box1 != NULL)
	       {
		  box1->BxXOrg += delta;
		  box1 = box1->BxNexChild;
	       }
	  }

	/* On analyse les relations pour savoir comment deplacer la boite */
	else if (ibox->BxAbstractBox != NULL)
	  {
	     /* La boite est elastique et n'est pas en cours de traitement */
	     if (ibox->BxHorizFlex && (!ibox->BxAbstractBox->AbLeafType == LtCompound || ibox->BxSpaceWidth == 0))
		ChngBElast (ibox, NULL, OpHorizDep, delta, frame, True);
	     /* Dans les autres cas */
	     else
	       {
		  /* simple translation */
		  ibox->BxXOrg += delta;

		  /* Faut-il mettre a jour le rectangle de reaffichage ? */
		  if (!ibox->BxAbstractBox->AbHorizEnclosing && EvalAffich)
		    {
		       /* Prend en compte une zone de debordement des graphiques */
		       if (ibox->BxAbstractBox->AbLeafType == LtGraphics)
			  i = EXTRA_GRAPH;
		       else
			  i = 0;
		       if (delta > 0)
			  DefClip (frame, ibox->BxXOrg - delta - i, ibox->BxYOrg - i,
				   ibox->BxXOrg + ibox->BxWidth + i, ibox->BxYOrg + ibox->BxHeight + i);
		       else
			  DefClip (frame, ibox->BxXOrg - i, ibox->BxYOrg - i,
				   ibox->BxXOrg + ibox->BxWidth - delta + i, ibox->BxYOrg + ibox->BxHeight + i);
		    }

		  /* On decale les boites qui ont des relations hors-structure avec */
		  /* la boite deplacee et on met a jour les dimensions elastiques   */
		  /* des boites liees a la boite deplacee.                          */
		  adpos = ibox->BxPosRelations;
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
				     && pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef == ibox->BxAbstractBox)
				   {

				      /* On refuse de deplacer une boite englobante de ibox */
				      if (Parent (pRe1->ReBox, ibox)) ;

				      else if (pRe1->ReBox->BxHorizFlex)
					 ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, True);
				      else
					 DepOrgX (pRe1->ReBox, ibox, delta, frame);

				   }
				 /* Relation sur la largeur elastique de la boite */
				 else if (pRe1->ReOp == OpWidth)
				    ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, True);
			      }

			    i++;
			    if (i <= MAX_RELAT_POS)
			       nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
			 }
		       adpos = adpos->PosRNext;	/* Bloc suivant */
		    }

		  /* Decale des boites englobees dont l'origine depend de l'englobante */
		  pavefils = ibox->BxAbstractBox->AbFirstEnclosed;
		  /* Traite le niveau inferieur */
		  reenglobx = False;
		  /* Si la boite est en cours de deplacement -> il faut transmettre */
		  /* la valeur de l'origine plutot que la valeur du decalage        */
		  if (ibox->BxXToCompute)
		     delta = ibox->BxXOrg;

		  while (pavefils != NULL)
		    {
		       if (pavefils->AbBox != NULL)
			  if (pavefils->AbBox->BxXOutOfStruct)
			     reenglobx = True;
			  else
			    {
			       if (ibox->BxXToCompute && !pavefils->AbBox->BxHorizFlex)
				  /* Additionne le decalage de la boite */
				  pavefils->AbBox->BxXToCompute = True;
			       DepXContenu (pavefils->AbBox, delta, frame);
			    }
		       pavefils = pavefils->AbNext;
		    }
		  ibox->BxXToCompute = False;	/* le decalage eventuel est pris en compte */

		  /* Si la largeur de la boite depend du contenu et qu'une des     */
		  /* boites filles est positionnee par une relation hors-structure */
		  /* --> il faut reevaluer la largeur correspondante.              */
		  if (reenglobx && ibox->BxContentWidth)
		     DiffereEnglobement (ibox, True);
	       }
	  }
	Englobement = SaveEnglob;	/* On restaure */
     }
}


/* ---------------------------------------------------------------------- */
/* |        DepYContenu deplace verticalement tout le contenu englobe   | */
/* |               de la boite ibox.                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepYContenu (PtrBox ibox, int delta, int frame)
#else  /* __STDC__ */
void                DepYContenu (ibox, delta, frame)
PtrBox            ibox;
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

   if (ibox != NULL && (delta != 0 || ibox->BxYToCompute))
     {
	/* On enregistre la hierarchie des boites dont le */
	/* traitement de l'englobement doit etre differe  */
	SaveEnglob = Englobement;
	if (ibox->BxAbstractBox->AbEnclosing != NULL)
	   Englobement = ibox /*->BxAbstractBox->AbEnclosing->AbBox*/ ;

	/* Si Englobement est une boite mere de la boite   */
	/* precedemment designee on garde l'ancienne boite */
	if (Parent (Englobement, SaveEnglob))
	   Englobement = SaveEnglob;

	/* Si la boite est mise en lignes */
	if (ibox->BxType == BoSplit)
	  {
	     box1 = ibox->BxNexChild;	/* On decale les boites de coupure */
	     while (box1 != NULL)
	       {
		  box1->BxYOrg += delta;
		  box1 = box1->BxNexChild;
	       }

	  }

	/* On analyse les relations pour savoir comment deplacer la boite */
	else if (ibox->BxAbstractBox != NULL)
	  {
/*-> La boite est elastique et n'est pas en cours de traitement */
	     if (ibox->BxVertFlex && (!ibox->BxAbstractBox->AbLeafType == LtCompound || ibox->BxSpaceWidth == 0))
		ChngBElast (ibox, NULL, OpVertDep, delta, frame, False);
/*-> Dans les autres cas */
	     else
	       {
		  /* simple translation */
		  ibox->BxYOrg += delta;

		  /* Faut-il mettre a jour le rectangle de reaffichage ? */
		  if (!ibox->BxAbstractBox->AbVertEnclosing && EvalAffich)
		    {
		       /* Prend en compte une zone de debordement des graphiques */
		       if (ibox->BxAbstractBox->AbLeafType == LtGraphics)
			  i = EXTRA_GRAPH;
		       else
			  i = 0;
		       if (delta > 0)
			  DefClip (frame, ibox->BxXOrg - i, ibox->BxYOrg - delta - i,
				   ibox->BxXOrg + ibox->BxWidth + i, ibox->BxYOrg + ibox->BxHeight + i);
		       else
			  DefClip (frame, ibox->BxXOrg - i, ibox->BxYOrg - i,
				   ibox->BxXOrg + ibox->BxWidth + i,
				   ibox->BxYOrg + ibox->BxHeight - delta + i);
		    }

		  /* On decale les boites qui ont des relations hors-structure avec */
		  /* la boite deplacee et on met a jour les dimensions elastiques   */
		  /* des boites liees a la boite deplacee.                          */
		  adpos = ibox->BxPosRelations;
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
				     && pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef == ibox->BxAbstractBox)
				   {

				      /* On refuse de deplacer une boite englobante de ibox */
				      if (Parent (pRe1->ReBox, ibox))
					 ;
				      else if (pRe1->ReBox->BxVertFlex)
					 ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, False);
				      else
					 DepOrgY (pRe1->ReBox, ibox, delta, frame);

				   }
				 /* Relation sur la hauteur elastique de la boite */
				 else if (pRe1->ReOp == OpHeight)
				    ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, False);
			      }
			    i++;
			    if (i <= MAX_RELAT_POS)
			       nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			 }
		       adpos = adpos->PosRNext;	/* Bloc suivant */
		    }

		  /* On traite les fils englobes ou places par rapport a l'englobante */
		  pavefils = ibox->BxAbstractBox->AbFirstEnclosed;	/* Traite le niveau inferieur */
		  reengloby = False;
		  /* Si la boite est en cours de deplacement -> il faut transmettre */
		  /* la valeur de l'origine plutot que la valeur du decalage        */
		  if (ibox->BxYToCompute)
		     delta = ibox->BxYOrg;
		  while (pavefils != NULL)
		    {
		       if (pavefils->AbBox != NULL)
			 {
			    if (pavefils->AbBox->BxYOutOfStruct)
			       reengloby = True;
			    else
			      {
				 if (ibox->BxYToCompute && !pavefils->AbBox->BxVertFlex)
				    /* Additionne le decalage de la boite */
				    pavefils->AbBox->BxYToCompute = True;
				 DepYContenu (pavefils->AbBox, delta, frame);
			      }
			 }
		       pavefils = pavefils->AbNext;
		    }
		  ibox->BxYToCompute = False;	/* le decalage eventuel est pris en compte */

		  /* Si la hauteur de la boite depend du contenu et qu'une des     */
		  /* boites filles est positionnee par une relation hors-structure */
		  /* --> il faut reevaluer la hauteur correspondante.              */
		  if (reengloby && ibox->BxContentHeight)
		     DiffereEnglobement (ibox, False);
	       }
	  }
	Englobement = SaveEnglob;	/* On restaure */
     }
}


/* ---------------------------------------------------------------------- */
/* |        DepAxe deplace l'axe de reference de la boite ibox dans la  | */
/* |                frametre frame et les boites qui en dependent.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepAxe (PtrBox ibox, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                DepAxe (ibox, prec, delta, frame)
PtrBox            ibox;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   int                 i, j, k;
   PtrAbstractBox             adpave;
   PtrAbstractBox             pavebox;
   boolean             deplace;
   boolean             nonnul;
   PtrBox            box1;
   PtrBox            cebox;
   PtrPosRelations      adpos;
   BoxRelation           *pRe1;

   if (ibox != NULL && delta != 0)
     {
	if (ibox->BxAbstractBox != NULL)
	   if (!ibox->BxAbstractBox->AbDead)
	     {
		/* Verifie que le deplacement n'est pas deja traite */
		deplace = True;
		box1 = prec;
		cebox = ibox;
		while (box1 != NULL)
		   if (box1 == ibox)
		     {
			box1 = NULL;
			deplace = False;
		     }
		   else
		     {
			if (box1->BxMoved == NULL)
			   cebox = box1;	/* boite ancetre */
			box1 = box1->BxMoved;
		     }

		if (deplace)
		  {
		     ibox->BxVertRef += delta;
		     pavebox = ibox->BxAbstractBox;
		     /* On met a jour la pile des boites traitees */
		     ibox->BxMoved = prec;
		     /* On regarde si les regles de dependance sont valides */
		     if (pavebox->AbEnclosing != NULL)
			if (pavebox->AbEnclosing->AbBox != NULL)
			   deplace = pavebox->AbEnclosing->AbBox->BxType != BoGhost;

		     if (deplace)

			if (ibox->BxHorizEdge == VertRef)
			  {
			     delta = -delta;
			     deplace = True;	/* Il faut verifier l'englobement */
			     /* On evalue la partie de la fenetre a reafficher */
			     if (EvalAffich && ibox->BxType != BoSplit)
			       {
				  i = ibox->BxXOrg;
				  j = ibox->BxXOrg + ibox->BxWidth;
				  /* Prend en compte une zone de debordement des graphiques */
				  if (ibox->BxAbstractBox->AbLeafType == LtGraphics)
				     k = EXTRA_GRAPH;
				  else
				     k = 0;
				  if (delta > 0)
				     j += delta;
				  else
				     i += delta;
				  DefClip (frame, i - k, ibox->BxYOrg - k, j + k, ibox->BxYOrg + ibox->BxHeight + k);
			       }

			     if (XEnAbsolu (ibox))
			       {
				  DepXContenu (ibox, delta, frame);
				  /* On a pu detruire le chainage des boites deplacees */
				  ibox->BxMoved = prec;
			       }
			     else
				ibox->BxXOrg += delta;

			     /* On decale aussi les boites liees a la boite deplacee */
			     adpos = ibox->BxPosRelations;
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
					    if (pRe1->ReBox != ibox
					    && pRe1->ReRefEdge != VertRef)
					       if (pRe1->ReOp == OpHorizRef)
						  DepAxe (pRe1->ReBox, ibox, delta, frame);
					       else if ((pRe1->ReOp == OpHorizDep
							 && pRe1->ReBox->BxAbstractBox->AbWidth.DimIsPosition)
						    || pRe1->ReOp == OpWidth)
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, True);
					       else if (pRe1->ReOp == OpHorizDep)
						  DepOrgX (pRe1->ReBox, ibox, delta, frame);
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
			     deplace = False;
			     adpos = ibox->BxPosRelations;
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
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, True);
					       else if (pRe1->ReOp == OpHorizDep)
						 {
						    DepOrgX (pRe1->ReBox, ibox, delta, frame);
						    deplace = True;	/* Il faut verifier l'englobement */
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
			     if (XEnAbsolu (ibox) && ibox->BxType != BoBlock)
			       {
				  adpave = pavebox->AbFirstEnclosed;
				  while (adpave != NULL)
				    {
				       if (!adpave->AbDead
					   && adpave->AbHorizPos.PosAbRef == pavebox
					   && adpave->AbHorizPos.PosRefEdge == VertRef)
					 {
					    box1 = adpave->AbBox;
					    DepOrgX (box1, ibox, delta, frame);
					 }
				       adpave = adpave->AbNext;
				    }

				  /* Faut-il reevaluer l'englobement de la boite ibox ? */
				  /*et si ce n'est pas une boite fille de ibox */
				  if (box1 != NULL && Propage == ToAll)
				     Englobx (pavebox, cebox, frame);
			       }
			  }

		     /* On respecte les contraintes d'englobement */
		     adpave = pavebox->AbEnclosing;
		     if (deplace)
			if (adpave == NULL)
			  {
			     if (Propage == ToAll && ibox->BxXOrg < 0)
				DepXContenu (ibox, -ibox->BxXOrg, frame);
			  }
		     /* Verifie l'englobement des boites de la hierarchie voisine */
		     /* sauf si l'englobement des boites doit etre differe        */
			else if (!adpave->AbInLine
				 && adpave->AbBox->BxType != BoGhost
				 && !Parent (adpave->AbBox, cebox)
				 && !Parent (adpave->AbBox, Englobement))
			   Englobx (adpave, cebox, frame);
		  }
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |        DepBase deplace l'axe de reference de la boite ibox dans la | */
/* |                frame frame et les boites qui en dependent.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepBase (PtrBox ibox, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                DepBase (ibox, prec, delta, frame)
PtrBox            ibox;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   int                 i, j, k;
   PtrAbstractBox             adpave;
   PtrAbstractBox             pavebox;
   boolean             deplace;
   boolean             nonnul;
   PtrBox            box1;
   PtrBox            cebox;
   PtrPosRelations      adpos;
   BoxRelation           *pRe1;

   if (ibox != NULL && delta != 0)
     {
	if (ibox->BxAbstractBox != NULL)
	   if (!ibox->BxAbstractBox->AbDead)
	     {
		/* Verifie que le deplacement n'est pas deja traite */
		deplace = True;
		box1 = prec;
		cebox = ibox;
		while (box1 != NULL)
		   if (box1 == ibox)
		     {
			box1 = NULL;
			deplace = False;
		     }
		   else
		     {
			if (box1->BxMoved == NULL)
			   cebox = box1;	/* boite ancetre */
			box1 = box1->BxMoved;
		     }

		if (deplace)
		  {
		     ibox->BxHorizRef += delta;
		     pavebox = ibox->BxAbstractBox;
		     /* On met a jour la pile des boites traitees */
		     ibox->BxMoved = prec;
		     /* On regarde si les regles de dependance sont valides */
		     if (pavebox->AbEnclosing != NULL)
			if (pavebox->AbEnclosing->AbBox != NULL)
			   deplace = pavebox->AbEnclosing->AbBox->BxType != BoGhost;

		     /* Si le point fixe de la boite est l'axe de reference -> deplace */
		     if (deplace)

			if (ibox->BxVertEdge == HorizRef)
			  {
			     delta = -delta;
			     deplace = True;	/* Il faut verifier l'englobement */
			     /* On evalue la partie de la fenetre a reafficher */
			     if (EvalAffich && ibox->BxType != BoSplit)
			       {
				  i = ibox->BxYOrg;
				  j = ibox->BxYOrg + ibox->BxHeight;
				  /* Prend en compte une zone de debordement des graphiques */
				  if (ibox->BxAbstractBox->AbLeafType == LtGraphics)
				     k = EXTRA_GRAPH;
				  else
				     k = 0;
				  if (delta > 0)
				     j += delta;
				  else
				     i += delta;
				  DefClip (frame, ibox->BxXOrg - k, i - k, ibox->BxXOrg + ibox->BxWidth + k, j + k);
			       }

			     if (YEnAbsolu (ibox))
			       {
				  DepYContenu (ibox, delta, frame);
				  /* On a pu detruire le chainage des boites deplacees */
				  ibox->BxMoved = prec;
			       }
			     else
				ibox->BxYOrg += delta;

			     /* On decale aussi les boites liees a la boite deplacee */
			     adpos = ibox->BxPosRelations;
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
					    if (pRe1->ReBox != ibox && pRe1->ReRefEdge != HorizRef)
					       if (pRe1->ReOp == OpVertRef)
						  DepBase (pRe1->ReBox, ibox, delta, frame);
					       else if ((pRe1->ReOp == OpVertDep
							 && pRe1->ReBox->BxAbstractBox->AbHeight.DimIsPosition)
						    || pRe1->ReOp == OpHeight)
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, False);
					       else if (pRe1->ReOp == OpVertDep)
						  DepOrgY (pRe1->ReBox, ibox, delta, frame);
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
			     deplace = False;
			     adpos = ibox->BxPosRelations;
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
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, False);
					       else if (pRe1->ReOp == OpVertDep)
						 {
						    /* On deplace une voisine */
						    DepOrgY (pRe1->ReBox, ibox, delta, frame);
						    deplace = True;	/* Il faut verifier l'englobement */
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
			     if (YEnAbsolu (ibox) && ibox->BxType != BoBlock)
			       {
				  adpave = pavebox->AbFirstEnclosed;
				  while (adpave != NULL)
				    {
				       if (!adpave->AbDead
					   && adpave->AbVertPos.PosAbRef == pavebox
					   && adpave->AbVertPos.PosRefEdge == HorizRef)
					 {
					    box1 = adpave->AbBox;
					    DepOrgY (box1, ibox, delta, frame);
					 }
				       adpave = adpave->AbNext;
				    }

				  /* Faut-il reevaluer l'englobement de la boite ibox ? */
				  /* et si ce n'est pas une boite fille de ibox */
				  if (box1 != NULL && Propage == ToAll)
				     Engloby (pavebox, cebox, frame);
			       }
			  }

		     /* On respecte les contraintes d'englobement */
		     adpave = pavebox->AbEnclosing;
		     if (deplace)
			if (adpave == NULL)
			  {
			     if (Propage == ToAll && ibox->BxYOrg < 0)
				DepYContenu (ibox, -ibox->BxYOrg, frame);
			  }
			else if (adpave->AbInLine
				 || adpave->AbBox->BxType == BoGhost)
			  {
			     if (Propage == ToAll)
				EnglLigne (ibox, frame, adpave);
			  }
		     /* Verifie l'englobement des boites de la hierarchie voisine */
		     /* sauf si l'englobement des boites doit etre differe        */
			else if (!Parent (adpave->AbBox, cebox)
				 && !Parent (adpave->AbBox, Englobement))
			   Engloby (adpave, cebox, frame);
		  }
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |    ModLarg modifie la largeur de la boite ibox correspondant a` la | */
/* |        frame frame suite au changement de largeur de la boite      | */
/* |        origine org.                                                | */
/* |        On respecte les contraintes de position :                   | */
/* |        - Toute boite liee a` un des cotes deplaces est             | */
/* |          deplacee.                                                 | */
/* |        - On met a` jour la base de la boite ibox si necessaire.    | */
/* |        On respecte les contraintes de dimension :                  | */
/* |        - On met a` jour les largeurs de boites qui en              | */
/* |          dependent.                                                | */
/* |        Le parametre dbl correspond au nombre de caracteres         | */
/* |        blanc ajoutes (>0) ou retires (<0). Il n'a de sens que      | */
/* |        quand la boite texte appartient a` une ligne justifiee.     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ModLarg (PtrBox ibox, PtrBox org, PtrBox prec, int delta, int dbl, int frame)
#else  /* __STDC__ */
void                ModLarg (ibox, org, prec, delta, dbl, frame)
PtrBox            ibox;
PtrBox            org;
PtrBox            prec;
int                 delta;
int                 dbl;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox            box1;
   PtrLine            adligne;
   PtrAbstractBox             adpave;
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

   if (ibox != NULL && delta != 0)
     {
	/* Faut-il nettoyer la file des boites deplacees */
	if (org == NULL && prec == NULL)
	   ibox->BxMoved = NULL;

	if (ibox->BxAbstractBox != NULL)
	   if (!ibox->BxAbstractBox->AbDead)
	     {
		/* On verifie que la largeur d'une boite ne devient pas negative */
		if (Propage != ToSiblings && delta < 0 && -delta > ibox->BxWidth)
		   delta = -ibox->BxWidth;
		/* Valeurs limites avant deplacement */
		i = ibox->BxXOrg;
		j = i + ibox->BxWidth;
		pavebox = ibox->BxAbstractBox;
		/* Ce n'est pas une boite elastique: RAZ sur file boites deplacees */
		if (!ibox->BxHorizFlex)
		   ibox->BxMoved = NULL;

		/* Force la reevaluation des points de controle de la polyline */
		if (pavebox->AbLeafType == LtPlyLine && ibox->BxImageDescriptor != NULL)
		  {
		     /* libere les points de controle */
		     free ((char *) ibox->BxImageDescriptor);
		     ibox->BxImageDescriptor = NULL;
		  }

		/* On regarde si les regles de dependance sont valides */
		deplace = True;
		if (pavebox->AbEnclosing != NULL)
		   if (pavebox->AbEnclosing->AbBox != NULL)
		      deplace = pavebox->AbEnclosing->AbBox->BxType != BoGhost;

		/* On respecte les contraintes de position */
		/* Point fixe sur l'origine */
		if (!deplace || ibox->BxHorizEdge == Left || ibox->BxHorizEdge == VertRef)
		  {
		     /* Mise a jour du reaffichage */
		     /* Valeurs de deplacements des reperes */
		     dxo = 0;
		     dxm = (ibox->BxWidth + delta) / 2 - ibox->BxWidth / 2;
		     dxf = delta;
		  }
		/* Point fixe sur le milieu */
		else if (ibox->BxHorizEdge == VertMiddle)
		  {
		     /* L'origine de la boite et le cote inferieur sont deplaces */
		     dxo = ibox->BxWidth / 2 - (ibox->BxWidth + delta) / 2;
		     dxf = delta + dxo;
		     /* On corrige les erreurs d'arrondi */
		     dxm = 0;
		  }
		/* Point fixe sur le cote droit */
		else
		  {
		     /* L'origine de la boite et le milieu sont deplaces */
		     dxo = -delta;
		     dxm = ibox->BxWidth / 2 - (ibox->BxWidth + delta) / 2;
		     dxf = 0;
		  }
		ibox->BxWidth += delta;
		ibox->BxXOrg += dxo;

		/* On evalue la partie de la fenetre a reafficher */
		if (EvalAffich
		    && ibox->BxType != BoSplit
		/* Il ne faut pas tenir compte de la boite si elle */
		/* n'est pas encore placee dans l'image concrete   */
		    && !ibox->BxXToCompute
		    && !ibox->BxYToCompute
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
			  if (ibox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (dxo < 0)
			     i += dxo;
			  if (dxf > 0)
			     j += dxf;
		       }
		     DefClip (frame, i - k, ibox->BxYOrg - k, j + k, ibox->BxYOrg + ibox->BxHeight + k);
		  }

		/* Deplacement de boites voisines et de l'englobante ? */
		adpos = ibox->BxPosRelations;
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
				if (!ibox->BxHorizFlex
				    || pRe1->ReOp != OpHorizDep
				    || pavebox == pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
				   switch (pRe1->ReRefEdge)
					 {
					       /* cote gauche */
					    case Left:
					       if (pRe1->ReOp == OpHorizRef)
						 {
						    if (pRe1->ReBox != ibox)
						       DepAxe (pRe1->ReBox, org, dxo, frame);
						 }
					       else if (pRe1->ReOp == OpHorizInc)
						 {
						    if (!ibox->BxHorizFlex)
						       DepOrgX (ibox, NULL, -dxo, frame);
						 }
					       else if ((pRe1->ReOp == OpHorizDep && pRe1->ReBox->BxHorizFlex)
						    || pRe1->ReOp == OpWidth)
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, dxo, frame, True);
					       else if (pRe1->ReBox != org)
						  DepOrgX (pRe1->ReBox, ibox, dxo, frame);
					       break;
					       /* milieu vertical */
					    case VertMiddle:
					       if (pRe1->ReOp == OpHorizRef)
						  if (pRe1->ReBox == ibox)
						    {
						       DepAxe (ibox, NULL, delta / 2, frame);
						       ibox->BxMoved = prec;	/* restaure le chainage */
						    }
						  else
						     DepAxe (pRe1->ReBox, org, dxm, frame);
					       else if (pRe1->ReOp == OpHorizInc)
						 {
						    if (!ibox->BxHorizFlex)
						       DepOrgX (ibox, NULL, -dxm, frame);
						 }
					       else if ((pRe1->ReOp == OpHorizDep && pRe1->ReBox->BxHorizFlex)
						    || pRe1->ReOp == OpWidth)
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, dxm, frame, True);
					       else if (pRe1->ReBox != org)
						  DepOrgX (pRe1->ReBox, ibox, dxm, frame);
					       break;
					       /* cote droit */
					    case Right:
					       if (pRe1->ReOp == OpHorizRef)
						  if (pRe1->ReBox == ibox)
						    {
						       DepAxe (ibox, NULL, delta, frame);
						       ibox->BxMoved = prec;	/* restaure le chainage */
						    }
						  else
						     DepAxe (pRe1->ReBox, org, dxf, frame);
					       else if (pRe1->ReOp == OpHorizInc)
						 {
						    if (!ibox->BxHorizFlex)
						       DepOrgX (ibox, NULL, -dxf, frame);
						 }
					       else if ((pRe1->ReOp == OpHorizDep && pRe1->ReBox->BxHorizFlex)
						    || pRe1->ReOp == OpWidth)
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, dxf, frame, True);
					       else if (pRe1->ReBox != org)
						  DepOrgX (pRe1->ReBox, ibox, dxf, frame);
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
		depabsolu = XEnAbsolu (ibox);

		/* Deplacement de boites incluses ou reevaluation du bloc de lignes */
		if (depabsolu
		    || pavebox->AbWidth.DimAbRef != NULL
		    || pavebox->AbWidth.DimValue != 0)
		   /* La boite n'est pas en cours de creation */
		   /* ou elle est a l'interieur d'une boite elastique */
		   /* ou elle est a l'interieur d'une boite hors-structure */
		   /* ou elle n'herite pas de la taille de son contenu */
		   if (ibox->BxType == BoBlock)
		      ReevalBloc (pavebox, ibox->BxFirstLine, org, frame);
		   else
		     {
			adpave = pavebox->AbFirstEnclosed;
			while (adpave != NULL)
			  {
			     if (!adpave->AbDead && adpave->AbBox != NULL)
			       {
				  box1 = adpave->AbBox;
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
					       switch (adpave->AbHorizPos.PosRefEdge)
						     {
							   /* milieu vertical */
							case VertMiddle:
							   if (depabsolu)
							      j = dxm;
							   else
							      /* il faut reevaluer le centrage */
							      j = (ibox->BxWidth - box1->BxWidth) / 2 - box1->BxXOrg;
							   if (box1->BxHorizFlex)
							      ChngBElast (box1, ibox, pRe1->ReOp, j, frame, True);
							   else
							      DepOrgX (box1, ibox, j, frame);
							   break;
							   /* cote droit */
							case Right:
							   if (depabsolu)
							      j = dxf;
							   else
							      /* il faut reevaluer le cadrage */
							      j = ibox->BxWidth - box1->BxWidth - box1->BxXOrg;
							   if (box1->BxHorizFlex)
							      ChngBElast (box1, ibox, pRe1->ReOp, j, frame, True);
							   else
							      DepOrgX (box1, ibox, j, frame);
							   break;
							   /* cote gauche */
							default:
							   if (depabsolu)
							      j = dxo;
							   else
							      /* il faut reevaluer le cadrage */
							      j = 0;
							   if (box1->BxHorizFlex)
							      ChngBElast (box1, ibox, pRe1->ReOp, j, frame, True);
							   else
							      DepOrgX (box1, ibox, j, frame);
							   break;
						     }

					    i++;
					    if (i <= MAX_RELAT_POS)
					       nonnul = (adpos->PosRTable[i - 1].ReBox != NULL);
					 }
				       adpos = adpos->PosRNext;	/* Bloc suivant */
				    }
			       }
			     adpave = adpave->AbNext;
			  }
		     }

		/* On respecte les contraintes de dimension */
		addim = ibox->BxWidthRelations;
		while (addim != NULL)
		  {
		     i = 1;
		     box1 = addim->DimRTable[i - 1];
		     while (i <= MAX_RELAT_DIM && box1 != NULL)
		       {
			  adpave = box1->BxAbstractBox;
			  /* Est-ce la meme dimension ? */
			  if (addim->DimRSame[i - 1])	/* Oui => Changement de largeur */
			    {
			       if (adpave->AbWidth.DimUnit == UnPercent)
				  /* Le changement de taille est un pourcentage */
				  dxo = ibox->BxWidth * adpave->AbWidth.DimValue / 100 - box1->BxWidth;
			       else
				  dxo = delta;	/* + ou - une constante */
			       /* On evite de boucler sur l'automodification */
			       if (box1 != ibox)
				  ChangeLargeur (box1, org, ibox, dxo, dbl, frame);

			    }
			  else
			     /* Non => Changement de hauteur */
			    {
			       /* L'heritage porte sur la hauteur de la ligne? */
			       if (adpave->AbEnclosing == pavebox
				   && (pavebox->AbInLine || pavebox->AbBox->BxType == BoGhost))
				 {
				    adligne = DesLigne (box1);
				    if (adligne == NULL)
				       dxo = 0;		/* la ligne n'est pas encore construite */
				    else
				       dxo = adligne->LiHeight - box1->BxHeight;
				 }
			       /* Le changement de taille est un pourcentage */
			       else if (adpave->AbHeight.DimUnit == UnPercent)
				  dxo = ibox->BxWidth * adpave->AbHeight.DimValue / 100 - box1->BxHeight;
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
		adpave = pavebox->AbEnclosing;		/* pave pere */
		if (!deplace)
		  {
		     /* Si la boite n'est pas fille d'une boite eclatee */
		     /* --> on recherche la boite bloc de lignes        */
		     while (!adpave->AbInLine || adpave->AbBox->BxType == BoGhost)
			adpave = adpave->AbEnclosing;
		  }

		if (adpave == NULL)
		  {
		     /* C'est la racine de l'image concrete */
		     if (Propage == ToAll && ibox->BxXOrg < 0)
			DepXContenu (ibox, -ibox->BxXOrg, frame);
		  }
		/* Englobement a refaire si la boite est englobee */
		else if (pavebox->AbHorizEnclosing)
		  {
		     /* note l'origine externe ou non de la modification de largeur */
		     relexterne = !Parent (adpave->AbBox, org);

		     /* si org n'est pas une fille il faut propager */
		     if ((Propage == ToAll || relexterne)
		     /* et si l'englobement n'est pas fait par une boite soeur */
			 && !Soeur (ibox, prec)
			 && !Soeur (ibox, org))
		       {

			  /* Inclusion dans un bloc de ligne */
			  if (adpave->AbInLine)
			    {
			       pMa1 = &FntrTable[frame - 1].FrSelectionBegin;
			       if (ibox == pMa1->VsBox)
				  adligne = pMa1->VsLine;
			       else
				  adligne = DesLigne (ibox);
			       MajBloc (adpave, adligne, ibox, delta, dbl, frame);
			    }
			  /* Si l'englobement n'est pas prevu en fin de traitement */
			  else if (adpave->AbBox != Englobement
				 && !Parent (adpave->AbBox, Englobement))
			     /* On differe le traitement de l'englobement   */
			     /* quand la mise a jour a une origine externe  */
			     if (Propage != ToAll)
				DiffereEnglobement (adpave->AbBox, True);
			  /* l'englobement d'une boite ne peut etre traite */
			  /* plus de deux fois (sinon on boucle).      */
			     else if (adpave->AbBox->BxNPixels <= 1)
				Englobx (adpave, org, frame);

		       }
		     else if (!pavebox->AbNew
			      && Propage == ToSiblings
			      && pavebox->AbLeafType == LtCompound
			    && pavebox->AbInLine && !ibox->BxYToCompute)
			/* La largeur de la boite mise en lignes est donnee par une */
			/* boite suivante, il faut verifier l'englobement vertical */
			Engloby (adpave, org, frame);
		  }
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |    ModHaut modifie la hauteur de la boite ibox correspondant a` la | */
/* |         frame frame suite au changement de hauteur de la boite     | */
/* |         origine org. On respecte les contraintes de position :     | */
/* |         - Toute boite liee a` un des cotes deplaces est            | */
/* |           deplacee.                                                | */
/* |         - On met a` jour la base de la boite ibox si necessaire.   | */
/* |         On respecte les contraintes de dimension :                 | */
/* |         - On met a` jour les hauteurs des boites qui en            | */
/* |           dependent.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ModHaut (PtrBox ibox, PtrBox org, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                ModHaut (ibox, org, prec, delta, frame)
PtrBox            ibox;
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
   PtrAbstractBox             adpave;
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

   if (ibox != NULL && delta != 0)
     {

	/* Faut-il nettoyer la file des boites deplacees */
	if (org == NULL && prec == NULL)
	   ibox->BxMoved = NULL;

	if (ibox->BxAbstractBox != NULL)
	   if (!ibox->BxAbstractBox->AbDead)
	     {
		/* On verifie que la hauteur d'une boite ne devient pas negative */
		if (Propage != ToSiblings && delta < 0 && -delta > ibox->BxHeight)
		   delta = -ibox->BxHeight;
		/* Valeurs limites avant deplacement */
		i = ibox->BxYOrg;
		j = i + ibox->BxHeight;
		pavebox = ibox->BxAbstractBox;

		/* Ce n'est pas une boite elastique: RAZ sur file des boites deplacees */
		if (!ibox->BxVertFlex)
		   ibox->BxMoved = NULL;

		/* Force la reevaluation des points de controle de la polyline */
		if (pavebox->AbLeafType == LtPlyLine && ibox->BxImageDescriptor != NULL)
		  {
		     /* libere les points de controle */
		     free ((char *) ibox->BxImageDescriptor);
		     ibox->BxImageDescriptor = NULL;
		  }

		/* On regarde si les regles de dependance sont valides */
		deplace = True;
		if (pavebox->AbEnclosing != NULL)
		   if (pavebox->AbEnclosing->AbBox != NULL)
		      deplace = pavebox->AbEnclosing->AbBox->BxType != BoGhost;

		/* On respecte les contraintes de position */
/*=> Point fixe sur l'origine */
		if (!deplace || ibox->BxVertEdge == Top || ibox->BxVertEdge == HorizRef)
		  {
		     /* Valeurs de deplacement des reperes */
		     dyo = 0;
		     dym = (ibox->BxHeight + delta) / 2 - ibox->BxHeight / 2;
		     dyf = delta;
		  }
/*=> Point fixe sur le milieu */
		else if (ibox->BxVertEdge == HorizMiddle)
		  {
		     /* L'origine de la boite et le cote inferieur sont deplaces */
		     dyo = ibox->BxHeight / 2 - (ibox->BxHeight + delta) / 2;
		     dyf = delta + dyo;		/* On corrige les erreurs d'arrondi */
		     dym = 0;
		  }
/*=> Point fixe sur le cote inferieur */
		else
		  {
		     /* L'origine de la boite et le milieu sont deplaces */
		     dyo = -delta;
		     dym = ibox->BxHeight / 2 - (ibox->BxHeight + delta) / 2;
		     dyf = 0;
		  }
		ibox->BxHeight += delta;
		ibox->BxYOrg += dyo;

		/* On evalue la partie de la fenetre a reafficher */
		if (EvalAffich
		    && ibox->BxType != BoSplit
		/* Il ne faut pas tenir compte de la boite si elle */
		/* n'est pas encore placee dans l'image concrete   */
		    && !ibox->BxXToCompute
		    && !ibox->BxYToCompute
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
			  if (ibox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (dyo < 0)
			     i += dyo;
			  if (dyf > 0)
			     j += dyf;
		       }
		     DefClip (frame, ibox->BxXOrg - k, i - k, ibox->BxXOrg + ibox->BxWidth + k, j + k);
		  }

		/* Deplacement de boites voisines et de l'englobante ? */
		adpos = ibox->BxPosRelations;
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
				if (!ibox->BxVertFlex
				    || pRe1->ReOp != OpVertDep
				    || pavebox == pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
				   switch (pRe1->ReRefEdge)
					 {
					       /* cote superieur */
					    case Top:
					       if (pRe1->ReOp == OpVertRef)
						 {
						    if (pRe1->ReBox != ibox)
						       DepBase (pRe1->ReBox, org, dyo, frame);
						 }
					       else if (pRe1->ReOp == OpVertInc)
						 {
						    if (!ibox->BxVertFlex)
						       DepOrgY (ibox, NULL, -dyo, frame);
						 }
					       else if ((pRe1->ReOp == OpVertDep && pRe1->ReBox->BxVertFlex)
						    || pRe1->ReOp == OpHeight)
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, dyo, frame, False);
					       else if (pRe1->ReBox != org)
						  DepOrgY (pRe1->ReBox, ibox, dyo, frame);
					       break;
					       /* milieu horizontal */
					    case HorizMiddle:
					       if (pRe1->ReOp == OpVertRef)
						  if (pRe1->ReBox == ibox)
						    {
						       DepBase (ibox, NULL, delta / 2, frame);
						       ibox->BxMoved = prec;	/* restaure le chainage */
						    }
						  else
						     DepBase (pRe1->ReBox, org, dym, frame);
					       else if (pRe1->ReOp == OpVertInc)
						 {
						    if (!ibox->BxVertFlex)
						       DepOrgY (ibox, NULL, -dym, frame);
						 }
					       else if ((pRe1->ReOp == OpVertDep && pRe1->ReBox->BxVertFlex)
						    || pRe1->ReOp == OpHeight)
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, dym, frame, False);
					       else if (pRe1->ReBox != org)
						  DepOrgY (pRe1->ReBox, ibox, dym, frame);
					       break;
					       /* cote inferieur */
					    case Bottom:
					       if (pRe1->ReOp == OpVertRef)
						 {
						    if (pRe1->ReBox == ibox)
						      {
							 if (pavebox->AbLeafType == LtText
							     && pavebox->AbHorizRef.PosAbRef == NULL)
							    j = FontBase (ibox->BxFont) - ibox->BxHorizRef;
							 else
							    j = delta;
							 DepBase (ibox, NULL, j, frame);
							 ibox->BxMoved = prec;	/* restaure le chainage */
						      }
						    else
						       DepBase (pRe1->ReBox, org, dyf, frame);
						 }
					       else if (pRe1->ReOp == OpVertInc)
						 {
						    if (!ibox->BxVertFlex)
						       DepOrgY (ibox, NULL, -dyf, frame);
						 }
					       else if ((pRe1->ReOp == OpVertDep && pRe1->ReBox->BxVertFlex)
						    || pRe1->ReOp == OpHeight)
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, dyf, frame, False);
					       else if (pRe1->ReBox != org)
						  DepOrgY (pRe1->ReBox, ibox, dyf, frame);
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
		depabsolu = YEnAbsolu (ibox);

		/* Deplacement de boites incluses ? */
		if (depabsolu && ibox->BxType == BoBlock)
		  {
		     /* Il faut traiter les blocs de lignes elastiques */
		     /* qui ne sont pas en cours de placement          */
		     if (dyo != 0)
		       {
			  /* On decale aussi les boites mises en ligne */
			  adpave = pavebox->AbFirstEnclosed;
			  while (adpave != NULL)
			    {
			       if (!adpave->AbDead)
				  DepYContenu (adpave->AbBox, dyo, frame);
			       adpave = adpave->AbNext;
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
		     adpave = pavebox->AbFirstEnclosed;
		     while (adpave != NULL)
		       {
			  if (!adpave->AbDead && adpave->AbBox != NULL)
			    {
			       box1 = adpave->AbBox;
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
					      switch (adpave->AbVertPos.PosRefEdge)
						    {
							  /* milieu horizontal */
						       case HorizMiddle:
							  if (depabsolu)
							     j = dym;
							  else
							     /* il faut reevaluer le centrage */
							     j = (ibox->BxHeight - box1->BxHeight) / 2 - box1->BxYOrg;
							  if (box1->BxVertFlex)
							     ChngBElast (box1, ibox, pRe1->ReOp, j, frame, False);
							  else
							     DepOrgY (box1, ibox, j, frame);
							  break;
							  /* cote inferieur */
						       case Bottom:
							  if (depabsolu)
							     j = dyf;
							  else
							     /* il faut reevaluer le cadrage */
							     j = ibox->BxHeight - box1->BxHeight - box1->BxYOrg;
							  if (box1->BxVertFlex)
							     ChngBElast (box1, ibox, pRe1->ReOp, j, frame, False);
							  else
							     DepOrgY (box1, ibox, j, frame);
							  break;
							  /* cote superieur */
						       default:
							  if (depabsolu)
							     j = dyo;
							  else
							     /* il faut reevaluer le cadrage */
							     j = 0;
							  if (box1->BxVertFlex)
							     ChngBElast (box1, ibox, pRe1->ReOp, j, frame, False);
							  else
							     DepOrgY (box1, ibox, j, frame);
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
			  adpave = adpave->AbNext;
		       }
		  }

		/* On respecte les contraintes de dimension */
		addim = ibox->BxHeightRelations;
		while (addim != NULL)
		  {
		     i = 1;
		     box1 = addim->DimRTable[i - 1];
		     while (i <= MAX_RELAT_DIM && box1 != NULL)
		       {
			  adpave = box1->BxAbstractBox;

			  /* Est-ce la meme dimension ? */
			  if (addim->DimRSame[i - 1])	/* Oui => Changement de hauteur */
			    {
			       /* L'heritage porte sur la hauteur de la ligne ? */
			       if (adpave->AbEnclosing == pavebox
				   && (pavebox->AbInLine || pavebox->AbBox->BxType == BoGhost))
				 {
				    adligne = DesLigne (box1);
				    if (adligne == NULL)
				       dyo = 0;		/* la ligne n'est pas encore construite */
				    else
				       dyo = adligne->LiHeight - box1->BxHeight;
				 }
			       /* Le changement de taille est un pourcentage */
			       else if (adpave->AbHeight.DimUnit == UnPercent)
				  dyo = ibox->BxHeight * adpave->AbHeight.DimValue / 100 - box1->BxHeight;
			       else
				  dyo = delta;
			       /* On evite de boucler sur l'automodification */
			       if (box1 != ibox)
				  ChangeHauteur (box1, org, ibox, dyo, frame);
			    }
			  else
			     /* Non => Changement de largeur */
			    {
			       if (adpave->AbWidth.DimUnit == UnPercent)
				  /* Le changement de taille est un pourcentage */
				  dyo = ibox->BxHeight * adpave->AbWidth.DimValue / 100 - box1->BxWidth;
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
		adpave = pavebox->AbEnclosing;
		if (!deplace)
		  {
		     /* Si la boite n'est pas fille d'une boite eclatee */
		     /* --> on recherche la boite bloc de lignes        */
		     while (!adpave->AbInLine || adpave->AbBox->BxType == BoGhost)
			adpave = adpave->AbEnclosing;
		  }

		if (adpave == NULL)
		  {
		     /* C'est la racine de l'image concrete */
		     if (Propage == ToAll && ibox->BxYOrg < 0)
			DepYContenu (ibox, -ibox->BxYOrg, frame);
		  }
		/* Englobement a refaire si la boite est englobee */
		else if (pavebox->AbVertEnclosing)
		  {
		     /* note l'origine externe ou non de la modification de hauteur */
		     relexterne = !Parent (adpave->AbBox, org);

		     /* si org n'est pas une fille il faut propager */
		     if ((Propage == ToAll || relexterne)
		     /* et si l'englobement n'est pas fait par une boite soeur */
			 && !Soeur (ibox, prec)
			 && !Soeur (ibox, org))
		       {
			  if (adpave->AbInLine)
			     /* Inclusion dans un bloc de ligne */
			     EnglLigne (ibox, frame, adpave);
			  /* Si l'englobement n'est pas prevu en fin de traitement */
			  else if (adpave->AbBox != Englobement
				 && !Parent (adpave->AbBox, Englobement))
			     /* On differe le traitement de l'englobement   */
			     /* quand la mise a jour a une origine externe  */
			     if (Propage != ToAll)
				DiffereEnglobement (adpave->AbBox, False);
			  /* l'englobement d'une boite ne peut etre traite */
			  /* plus de deux fois (sinon on boucle).      */
			     else if (adpave->AbBox->BxSpaceWidth <= 1)
				Engloby (adpave, org, frame);
		       }
		  }
	     }
     }

   /* Traitement specifique des largeurs de symboles */
   if (ibox != NULL)
     {
	if (ibox->BxAbstractBox->AbLeafType == LtSymbol)
	  {
	     i = 0;
	     font = ibox->BxFont;
	     valeur = 1 + ((float) (ibox->BxHeight * 0.6) / (float) FontHeight (font));
	     switch (ibox->BxAbstractBox->AbShape)
		   {
		      case 'd':	/*integrale double */
			 i = CarWidth (231, font) + CarWidth (231, font) / 2;
		      case 'i':	/*integrale */
		      case 'c':	/*integrale circulaire */
			 i = (int) ((float) CarWidth (231, font) * valeur);
			 ModLarg (ibox, NULL, NULL, i - ibox->BxWidth, 0, frame);
			 break;
		      case '(':
		      case ')':
		      case '{':
		      case '}':
		      case '[':
		      case ']':
			 i = (int) ((float) CarWidth (230, font) * valeur);
			 ModLarg (ibox, NULL, NULL, i - ibox->BxWidth, 0, frame);
			 break;
		      default:
			 break;
		   }		/*switch */
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    DepOrgX deplace l'origine de la boite ibox, donnee en parametre,| */
/* |            de delta. On respecte les contraintes de position.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepOrgX (PtrBox ibox, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                DepOrgX (ibox, prec, delta, frame)
PtrBox            ibox;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox            box1;
   int                 i, j, k;
   PtrAbstractBox             adpave;
   PtrPosRelations      adpos;
   boolean             nonnul;
   boolean             deplace;
   boolean             depabsolu;
   PtrAbstractBox             pPa1;
   BoxRelation           *pRe1;
   boolean             aplacer;

   if (ibox != NULL && delta != 0)
     {
	pPa1 = ibox->BxAbstractBox;
	if (pPa1 != NULL)
	   if (!pPa1->AbDead)
	     {
		/* On verifie que la boite n'a pas ete deja deplacee */
		if (prec != NULL)
		  {
		     box1 = prec->BxMoved;
		     while (box1 != NULL)
			if (box1 == ibox)
			   return;
			else
			   box1 = box1->BxMoved;
		  }
		/* On met a jour la pile des boites traitees */
		ibox->BxMoved = prec;

		aplacer = ibox->BxXToCompute;	/* memorise que la boite doit etre placee */
		/* On evalue la partie de la fenetre a reafficher */
		if (EvalAffich)
		  {
		     if (ibox->BxType != BoSplit)
		       {
			  i = ibox->BxXOrg;
			  j = ibox->BxXOrg + ibox->BxWidth;
			  /* Prend en compte une zone de debordement des graphiques */
			  if (ibox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (delta > 0)
			     j += delta;
			  else
			     i += delta;
			  if (ibox->BxHeight > 0)
			     DefClip (frame, i - k, ibox->BxYOrg - k, j + k, ibox->BxYOrg + ibox->BxHeight + k);
		       }
		     /* Est-ce un pave non englobe ? */
		     else if (!pPa1->AbVertEnclosing)
			if (delta > 0)
			   DefClip (frame, ibox->BxXOrg - delta, ibox->BxYOrg,
				    ibox->BxXOrg + ibox->BxWidth, ibox->BxYOrg + ibox->BxHeight);
			else
			   DefClip (frame, ibox->BxXOrg, ibox->BxYOrg, ibox->BxXOrg +
				    ibox->BxWidth - delta, ibox->BxYOrg + ibox->BxHeight);
		  }

		/* On deplace seulement l'origine de la boite si elle est en cours  */
		/* de creation et que ce n'est pas une boite elastique, sinon on    */
		/*  deplace tout le contenu (boites englobees)                      */
		depabsolu = XEnAbsolu (ibox);

		/* Teste si la position de la boite est absolue ou relative */
		if (depabsolu)
		  {
		     DepXContenu (ibox, delta, frame);
		     /* On a pu detruire le chainage des boites deplacees */
		     ibox->BxMoved = prec;
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
		   ibox->BxXOrg += delta;

		/* Regarde si les regles de dependance sont valides */
		deplace = True;
		if (pPa1->AbEnclosing != NULL)
		   if (pPa1->AbEnclosing->AbBox != NULL)
		      deplace = (pPa1->AbEnclosing->AbBox->BxType != BoGhost);

		/* Decale les boites dependantes qui restent a deplacer */
		adpos = ibox->BxPosRelations;
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
				       if (pRe1->ReBox != ibox)
					 {
					    adpave = pPa1->AbEnclosing;
					    if (adpave != NULL)
					       box1 = adpave->AbBox;
					    else
					       box1 = NULL;
					    if (pRe1->ReBox != box1 || Propage == ToAll)
					       DepAxe (pRe1->ReBox, ibox, delta, frame);
					 }
				    }
				  /* Ignore la relation inverse de la boite elastique */
				  else if (ibox->BxHorizFlex
					   && pPa1 != pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
				     ;

				  /* On ne decale pas les boites qui ont des relations  */
				  /* hors-structure avec la boite deplacee et on ne met */
				  /* pas a jour les dimensions elastiques des boites    */
				  /* liees a la boite deplacee si elles ont ete         */
				  /* traitees par DepXContenu.                          */

				  else if (depabsolu)
				    {
				       if (!ibox->BxHorizFlex || aplacer)
					 {
					    /* le travail n'a pas ete fait dans DepXContenu */
					    if (pRe1->ReOp == OpHorizDep && !pRe1->ReBox->BxXOutOfStruct)
					       /* Relation conforme a la structure sur l'origine de boite */
					       if (pRe1->ReBox->BxHorizFlex
					       /* si la boite n'est pas une boite fille */
						   && pPa1 != pRe1->ReBox->BxAbstractBox->AbEnclosing
						   && pPa1 == pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef)
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, True);
					    /* Relation conforme a la structure sur la largeur de boite */
					       else
						  DepOrgX (pRe1->ReBox, ibox, delta, frame);
					 }
				    }
				  else if (pRe1->ReOp == OpHorizDep && !pRe1->ReBox->BxHorizFlex)
				     DepOrgX (pRe1->ReBox, ibox, delta, frame);
				  else if (((pRe1->ReOp == OpHorizDep
					     && pPa1 == pRe1->ReBox->BxAbstractBox->AbHorizPos.PosAbRef))
					   || pRe1->ReOp == OpWidth)
				     ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, True);
			       }

			     /* On a pu detruire le chainage des boites deplacees */
			     ibox->BxMoved = prec;
			     i++;
			     if (i <= MAX_RELAT_POS)
				nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			  }
			adpos = adpos->PosRNext;	/* Bloc suivant */
		     }

		/* Si le calcul de la largeur de la boite englobante est a refaire */
		adpave = pPa1->AbEnclosing;
		if (deplace && ibox->BxXOutOfStruct && adpave != NULL)
		   /* On ne peut traiter l'englobement d'une boite si cette boite */
		   /* est en cours de placement ou si ce traitement est differe   */
		   if (!adpave->AbBox->BxXToCompute
		       && !Parent (adpave->AbBox, Englobement)
		   /* si org est une fille il faut la propagation a toutes */
		       && (Propage == ToAll || !Parent (adpave->AbBox, prec))
		   /* et si l'englobement n'est pas fait par une boite soeur */
		       && !Soeur (ibox, prec)
		   /* et si la boite deplacee est effectivement englobee */
		       && pPa1->AbHorizEnclosing)
		      Englobx (adpave, prec, frame);
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |    DepOrgY deplace l'origine de la boite ibox, donnee en parametre,| */
/* |            de delta. On respecte les contraintes de position.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DepOrgY (PtrBox ibox, PtrBox prec, int delta, int frame)
#else  /* __STDC__ */
void                DepOrgY (ibox, prec, delta, frame)
PtrBox            ibox;
PtrBox            prec;
int                 delta;
int                 frame;

#endif /* __STDC__ */
{
   PtrBox            box1;
   int                 i, j, k;
   PtrAbstractBox             adpave;
   PtrPosRelations      adpos;
   boolean             nonnul;
   boolean             deplace;
   boolean             depabsolu;
   PtrAbstractBox             pPa1;
   BoxRelation           *pRe1;
   boolean             aplacer;

   if (ibox != NULL && delta != 0)
     {
	pPa1 = ibox->BxAbstractBox;
	if (pPa1 != NULL)
	   if (!pPa1->AbDead)
	     {
		/* On verifie que la boite n'a pas ete deja deplacee */
		if (prec != NULL)
		  {
		     box1 = prec->BxMoved;
		     while (box1 != NULL)
			if (box1 == ibox)
			   return;
			else
			   box1 = box1->BxMoved;
		  }
		/* On met a jour la pile des boites traitees */
		ibox->BxMoved = prec;

		aplacer = ibox->BxYToCompute;	/* memorise que la boite doit etre placee */
		/* On evalue la partie de la fenetre a reafficher */
		if (EvalAffich)
		  {
		     if (ibox->BxType != BoSplit)
		       {
			  i = ibox->BxYOrg;
			  j = ibox->BxYOrg + ibox->BxHeight;
			  /* Prend en compte une zone de debordement des graphiques */
			  if (ibox->BxAbstractBox->AbLeafType == LtGraphics)
			     k = EXTRA_GRAPH;
			  else
			     k = 0;
			  if (delta > 0)
			     j += delta;
			  else
			     i += delta;
			  if (ibox->BxWidth > 0 || k > 0)
			     DefClip (frame, ibox->BxXOrg - k, i - k, ibox->BxXOrg + ibox->BxWidth + k, j + k);
		       }
		     /* Est-ce un pave non englobe ? */
		     else if (!pPa1->AbHorizEnclosing)
			if (delta > 0)
			   DefClip (frame, ibox->BxXOrg, ibox->BxYOrg - delta,
				    ibox->BxXOrg + ibox->BxWidth, ibox->BxYOrg + ibox->BxHeight);
			else
			   DefClip (frame, ibox->BxXOrg, ibox->BxYOrg, ibox->BxXOrg +
				    ibox->BxWidth, ibox->BxYOrg + ibox->BxHeight - delta);
		  }

		/* On deplace seulement l'origine de la boite si elle est en cours  */
		/* de creation et que ce n'est pas une boite elastique, sinon on    */
		/*  deplace tout le contenu (boites englobees)                      */
		depabsolu = YEnAbsolu (ibox);

		/* Teste si la position de la boite est absolue ou relative */
		if (depabsolu)
		  {
		     DepYContenu (ibox, delta, frame);
		     /* On a pu detruire le chainage des boites deplacees */
		     ibox->BxMoved = prec;
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
		   ibox->BxYOrg += delta;

		/* Regarde si les regles de dependance sont valides */
		deplace = True;
		if (pPa1->AbEnclosing != NULL)
		   if (pPa1->AbEnclosing->AbBox != NULL)
		      deplace = (pPa1->AbEnclosing->AbBox->BxType != BoGhost);


		/* On decale les boites dependantes qui restent a deplacer */

		adpos = ibox->BxPosRelations;
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
				       if (pRe1->ReBox != ibox)
					 {
					    adpave = pPa1->AbEnclosing;
					    if (adpave != NULL)
					       box1 = adpave->AbBox;
					    else
					       box1 = NULL;
					    if (pRe1->ReBox != box1 || Propage == ToAll)
					       DepBase (pRe1->ReBox, ibox, delta, frame);
					 }
				    }
				  /* Ignore la relation inverse de la boite elastique */
				  else if (ibox->BxVertFlex
					   && pPa1 != pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
				     ;
				  /* On ne decale pas les boites qui ont des relations  */
				  /* hors-structure avec la boite deplacee et on ne met */
				  /* pas a jour les dimensions elastiques dess boites   */
				  /* liees a la boite deplacee si elles ont ete         */
				  /* traitees par DepYContenu.                        */

				  else if (depabsolu)
				    {
				       if (!ibox->BxVertFlex || aplacer)
					 {
					    /* le travail n'a pas ete fait dans DepYContenu */
					    /* Relation conforme a la structure sur l'origine de boite */
					    if (pRe1->ReOp == OpVertDep && !pRe1->ReBox->BxYOutOfStruct)
					       if (pRe1->ReBox->BxVertFlex
					       /* si la boite n'est pas une boite fille */
						   && pPa1 != pRe1->ReBox->BxAbstractBox->AbEnclosing
						   && pPa1 == pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
						  ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, False);
					       else
						  DepOrgY (pRe1->ReBox, ibox, delta, frame);
					 }
				    }
				  else if (pRe1->ReOp == OpVertDep && !pRe1->ReBox->BxVertFlex)
				     DepOrgY (pRe1->ReBox, ibox, delta, frame);
				  else if ((pRe1->ReOp == OpVertDep
					    && pPa1 == pRe1->ReBox->BxAbstractBox->AbVertPos.PosAbRef)
					   || pRe1->ReOp == OpHeight)
				     ChngBElast (pRe1->ReBox, ibox, pRe1->ReOp, delta, frame, False);
			       }

			     /* On a pu detruire le chainage des boites deplacees */
			     ibox->BxMoved = prec;
			     i++;
			     if (i <= MAX_RELAT_POS)
				nonnul = adpos->PosRTable[i - 1].ReBox != NULL;
			  }
			adpos = adpos->PosRNext;	/* Bloc suivant */
		     }

		/* Si le calcul de la hauteur de la boite englobante est a refaire */
		adpave = pPa1->AbEnclosing;
		if (deplace && ibox->BxYOutOfStruct && adpave != NULL)
		   /* On ne peut traiter l'englobement d'une boite si cette boite */
		   /* est en cours de placement ou si ce traitement est differe   */
		   if (!adpave->AbBox->BxYToCompute
		       && !Parent (adpave->AbBox, Englobement)
		   /* si org est une fille il faut la propagation a toutes */
		       && (Propage == ToAll || !Parent (adpave->AbBox, prec))
		   /* et si l'englobement n'est pas fait par une boite soeur */
		       && !Soeur (ibox, prec)
		   /* et si la boite deplacee est effectivement englobee */
		       && pPa1->AbVertEnclosing)
		      Engloby (adpave, prec, frame);
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |    Englobx verifie l'inclusion en largeur des boites englobees dans| */
/* |            le pave adpave suite au changement de largeur de la     | */
/* |            boite origine org.                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Englobx (PtrAbstractBox adpave, PtrBox org, int frame)
#else  /* __STDC__ */
void                Englobx (adpave, org, frame)
PtrAbstractBox             adpave;
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

   pBo1 = adpave->AbBox;
   pPavD1 = &adpave->AbWidth;
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
	nonvide = False;
	deplace = False;	/* on n'a rien deplace */

	/* Le cote gauche de la plus a gauche des boites englobees doit */
	/* etre colle au cote gauche de la boite englobante et la       */
	/* largeur de la boite englobante est delimitee par le cote     */
	/* droit le plus a droite des boites englobees.                 */

	pavefils = adpave->AbFirstEnclosed;
	while (pavefils != NULL)
	  {
	     box1 = pavefils->AbBox;
	     if (!pavefils->AbDead && box1 != NULL
		 && pavefils->AbHorizEnclosing
		 && pavefils->AbWidth.DimAbRef != adpave)
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
			       nonvide = True;
			       if (box1->BxXOrg < val)
				  val = box1->BxXOrg;	/* valeur minimum */
			       i = box1->BxXOrg + box1->BxWidth;
			    }
			  /* La position de la boite depend de la taille de l'englobante ? */
			  else if (pPa3->AbHorizPos.PosAbRef == adpave
				&& (pPa3->AbHorizPos.PosRefEdge != Left
				    || (pPa3->AbWidth.DimAbRef == adpave
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
	if (larg == x && adpave->AbVolume == 0)
	   EvalText (adpave, &larg, &x, &i);
	else
	   larg -= x;
	x = larg - pBo1->BxWidth;	/* Difference de largeur */

	/* Faut-il deplacer les boites englobees ? */
	pavefils = adpave->AbFirstEnclosed;
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
				pPavP1 = &adpave->AbVertRef;
				if (pPavP1->PosAbRef == pavefils)
				  {
				     deplace = True;
				     box1->BxMoved = NULL;
				     /* Est-ce que la relation porte sur l'axe de reference ? */
				     if (pPavP1->PosRefEdge != VertRef)
					DepAxe (pBo1, box1, val, frame);
				     /* Recalcule la position de l'axe */
				     else
				       {
					  if (pPavP1->PosUnit == UnPercent)
					     i = PixelValue (pPavP1->PosDistance, UnPercent, (PtrAbstractBox) adpave->AbBox->BxWidth);
					  else
					     i = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit, adpave);
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
	   if (adpave->AbEnclosing == NULL)
	     {
		if (pBo1->BxXOrg < 0)
		   DepXContenu (pBo1, -pBo1->BxXOrg, frame);
	     }
	   else if (!adpave->AbEnclosing->AbInLine
		    && adpave->AbEnclosing->AbBox->BxType != BoGhost)
	      Englobx (adpave->AbEnclosing, org, frame);

	/* Indique que le traitement de l'englobement horizontal est termine */
	pBo1->BxNPixels -= 1;
     }
   /* Si la boite prend la largeur minimum, il faut quand meme      */
   /* evaluer la largeur du contenu et verifier la regle du minimum */
   else if (!pPavD1->DimIsPosition && pPavD1->DimMinimum
	    && pBo1->BxType != BoGhost)
     {
	EvalComp (adpave, frame, &larg, &val);
	ChangeLgContenu (pBo1, org, larg, 0, frame);
     }
}

/* ---------------------------------------------------------------------- */
/* |        Engloby  verifie l'inclusion en hauteur des boites englobees| */
/* |           dans le pave adpave suite au changement de hauteur de la | */
/* |           boite origine org.                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                Engloby (PtrAbstractBox adpave, PtrBox org, int frame)
#else  /* __STDC__ */
void                Engloby (adpave, org, frame)
PtrAbstractBox             adpave;
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

   pBo1 = adpave->AbBox;
   pPavD1 = &adpave->AbHeight;
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
	nonvide = False;
	deplace = False;	/* on n'a rien deplace */

	/* Le cote superieur de la plus haute boite englobee doit etre  */
	/* colle au cote superieur de la boite englobante et la hauteur */
	/* de la boite englobante est delimitee par le cote inferieur   */
	/* le plus bas des boites englobees.                          */

	pavefils = adpave->AbFirstEnclosed;
	while (pavefils != NULL)
	  {
	     box1 = pavefils->AbBox;
	     if (!pavefils->AbDead
		 && box1 != NULL
		 && pavefils->AbVertEnclosing
		 && pavefils->AbHeight.DimAbRef != adpave)
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
			       nonvide = True;
			       if (box1->BxYOrg < val)
				  val = box1->BxYOrg;	/* valeur minimum */
			       i = box1->BxYOrg + box1->BxHeight;
			    }
			  /* La position de la boite depend de la taille de englobante ? */
			  else if (pPa3->AbVertPos.PosAbRef == adpave
				   && (pPa3->AbVertPos.PosRefEdge != Top
				       || (pPa3->AbHeight.DimAbRef == adpave
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
	if (haut == y && adpave->AbVolume == 0)
	   EvalText (adpave, &y, &haut, &i);
	else
	   haut -= y;
	y = haut - pBo1->BxHeight;	/* Difference de hauteur */

	/* Faut-il deplacer les boites englobees ? */
	pavefils = adpave->AbFirstEnclosed;
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
				pPavP1 = &adpave->AbHorizRef;
				if (pPavP1->PosAbRef == pavefils)
				  {
				     deplace = True;
				     box1->BxMoved = NULL;
				     /* Est-ce que la relation porte sur l'axe de reference ? */
				     if (pPavP1->PosRefEdge != HorizRef)
					DepBase (pBo1, box1, val, frame);
				     /* Recalcule la position de l'axe */
				     else
				       {
					  if (pPavP1->PosUnit == UnPercent)
					     i = PixelValue (pPavP1->PosDistance, UnPercent, (PtrAbstractBox) adpave->AbBox->BxHeight);
					  else
					     i = PixelValue (pPavP1->PosDistance, pPavP1->PosUnit, adpave);
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
	   if (adpave->AbEnclosing == NULL)
	     {
		if (pBo1->BxYOrg < 0)
		   DepYContenu (pBo1, -pBo1->BxYOrg, frame);
	     }
	   else if (adpave->AbEnclosing->AbInLine)
	      EnglLigne (pBo1, frame, adpave->AbEnclosing);
	   else if (adpave->AbEnclosing->AbBox->BxType == BoGhost)
	     {
		/* Il faut remonter au pave de mise en lignes */
		while (adpave->AbEnclosing->AbBox->BxType == BoGhost)
		   adpave = adpave->AbEnclosing;
		EnglLigne (pBo1, frame, adpave->AbEnclosing);
	     }
	   else
	      Engloby (adpave->AbEnclosing, org, frame);

	/* On indique que le traitement de l'englobement vertical est termine */
	pBo1->BxSpaceWidth -= 1;
     }
   /* Si la boite prend la hauteur minimum, il faut quand meme      */
   /* evaluer la hauteur du contenu et verifier la regle du minimum */
   else if (!pPavD1->DimIsPosition && pPavD1->DimMinimum
	    && pBo1->BxType != BoGhost)
     {
	EvalComp (adpave, frame, &val, &haut);
	ChangeHtContenu (pBo1, org, haut, frame);
     }
}
/* End Of Module dep */
