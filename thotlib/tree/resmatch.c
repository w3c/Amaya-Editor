
#ifndef STANDALONE
#include "thot_gui.h"
#include "thot_sys.h"
#include "tree.h"
#include "application.h"
#include "view.h"
#include "selection.h"
#include "conststr.h"
#include "typestr.h"
#endif /* STANDALONE */

#define THOT_EXPORT extern
#include "constres.h"


/*----------------------------------------------------------------------  
  RestCoupler
  ----------------------------------------------------------------------*/
static ThotBool RestCoupler (Restruct restr, int indSrc, int indDst,
			     int indRecFrom, int indRecTo)
{
#ifdef DEBUG
  fprintf (stdout, "      Coupler  : %d %c - %d %c\n",
	   indSrc, 
	   restr->RSrcPrint->SPrint[indSrc],
	   indDst, 
	   restr->RDestPrint[indDst]);
#endif
  if (restr->RSrcPrint->SNodes[indSrc] != NULL && 
      restr->RDestNodes[indDst] != NULL)
    {
      restr->RCoupledNodes[indSrc] = (StrRestCouple*)TtaGetMemory (sizeof (StrRestCouple));
      restr->RCoupledNodes[indSrc]->CDstNode = restr->RDestNodes[indDst];
      if (indRecFrom == 0 && indRecTo == 0)
	{
	  restr->RCoupledNodes[indSrc]->CRecFrom = NULL;
	  restr->RCoupledNodes[indSrc]->CRecTo = NULL;
	}
      else
	{
	  restr->RCoupledNodes[indSrc]->CRecFrom = restr->RDestNodes[indRecFrom];
	  restr->RCoupledNodes[indSrc]->CRecTo = restr->RDestNodes[indRecTo];
	}
      return TRUE;
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------  
  RestDeCoupler
  ----------------------------------------------------------------------*/
static ThotBool RestDeCoupler (Restruct resctx, int indSrc)
{
#ifdef DEBUG
  fprintf (stdout, "      Decoupler source : %d %c\n",indSrc,
	   resctx->RSrcPrint->SPrint[indSrc]);
#endif
  if (indSrc < 0 || resctx->RCoupledNodes[indSrc] == NULL)
    {
#ifdef DEBUG
      fprintf(stderr,"      erreur indice decoupler\n");
#endif
      return FALSE;
    }
  TtaFreeMemory (resctx->RCoupledNodes[indSrc]);
  resctx->RCoupledNodes[indSrc] = NULL;
  return TRUE;
}  


/*----------------------------------------------------------------------  
  RestEraseCouple
  ----------------------------------------------------------------------*/
static ThotBool RestEraseCouple (Restruct resctx)
{
  int i;
  
#ifdef DEBUG
  fprintf (stdout, "      RAZ couples \n");
#endif
  for (i=0; i<SIZEPRINT; i++)
    {
      if (resctx->RCoupledNodes[i] != NULL)
	{
	  TtaFreeMemory (resctx->RCoupledNodes[i]);
	  resctx->RCoupledNodes[i] = NULL;
	}
    }
  return TRUE;
} 

/*----------------------------------------------------------------------  
  RestMatchSymb
  ----------------------------------------------------------------------*/
static TyRelation RestMatchSymb (Restruct resctx, int ISrc, int IDst)
{
  TyRelation result;
  char       CSrc, CDst;
  char      *PSrc;
  char      *PDst;

  PSrc = resctx->RSrcPrint->SPrint;
  PDst = resctx->RDestPrint;
  result = NONE;
  /* si l'un des deux symboles (source ou dest) est @, recherche s'ils */
  /* correspondent au meme type d'element */
  if (PSrc[ISrc] == '@' || PDst[IDst] == '@')
    {
      if (TtaSameSSchemas (RContext->CSrcSchema, (resctx->RDestType).ElSSchema) &&
	  resctx->RSrcPrint->SNodes[ISrc] != NULL &&
	  resctx->RDestNodes[IDst] != NULL &&
	  resctx->RSrcPrint->SNodes[ISrc]->TypeNum == resctx->RDestNodes[IDst]->TypeNum && 
	  (PSrc[ISrc] == '{' || PSrc[ISrc] == '(' || PSrc[ISrc] == '[' || 
	   PDst[IDst] == '{' || PDst[IDst] == '(' || PDst[IDst] == '['))
	result = IDENTITE;
      else if (PDst[IDst] == '@' && PSrc[ISrc] != '}' && PSrc[ISrc] != ')' &&
	       PSrc[ISrc] != ']')
	result = REC_MASK;
    }

  if (result == NONE)
    {
      CSrc = PSrc[ISrc];
      CDst = PDst[IDst];
      if (CDst != CSrc)
	{ /* les sources  texte, graphique, symbole, reference, image */
	  /* sont compatibles avec la destination unite */
	  if (CDst == 'U' && 
	      (CSrc == 'T' || CSrc == 'G' || CSrc == 'S' || CSrc == 'R' || CSrc == 'P'))
	    result = EQUIVALENCE;
	}
      else
	{
	  /****
	       if (CSrc == 'N') ** deux natures **
	       ;
	       if (CSrc == 'R') ** deux references **
	       ;
	  ****/
	  result = EQUIVALENCE;
	}
    }
  return result;
}

/*----------------------------------------------------------------------  
  RestCalcShift
  ----------------------------------------------------------------------*/
static int *RestCalcShift (char *pattern)
{
  int  j, k, Lgpattern;
  int *Shift;
 
  j = 0;
  k = -1;
  Lgpattern = strlen (pattern);
  Shift = (int *) TtaGetMemory (Lgpattern * sizeof (int));

  Shift[0] = -1;
  while (j < (Lgpattern - 1))
    {
      while (k >= 0 && pattern[j] != pattern[k])
        k = Shift[k];
      j += 1;
      k += 1;
      if (pattern[j] == pattern[k])
        Shift[j] = Shift[k];
      else
        Shift[j] = k;
    }
  return Shift;
} 


/*----------------------------------------------------------------------  
  RestFacteur
  ----------------------------------------------------------------------*/
static void RestFacteur (Restruct resctx)
{
  int                *ShiftTab;        /* tableau de decalages */
  int                 IndSrc, IndDst;
  int                 LgSrc, LgDst, SubStrBegin;
  int                 i, j;
  char               *CSrc, *CDst;


  CSrc = resctx->RSrcPrint->SPrint;
  CDst = resctx->RDestPrint;
  ShiftTab = RestCalcShift (CDst);
  LgSrc = strlen (CSrc);
  LgDst = strlen (CDst);
  SubStrBegin = -1;
  IndSrc = 0;
  IndDst = 0;
  while (IndDst < LgDst && IndSrc < LgSrc)
    {
      while (IndDst >= 0 && 
             RestMatchSymb (resctx, IndDst, IndSrc) == NONE)
        IndDst = ShiftTab[IndDst];
      IndSrc += 1;
      IndDst += 1;
    }
  TtaFreeMemory (ShiftTab);
  if (IndDst == LgDst)
    {
      resctx->RRelation = FACTOR;
      SubStrBegin = IndSrc - IndDst;
      j = SubStrBegin;
      for (i = 0; i < LgDst; i++, j++)
	RestCoupler (resctx,i,j, 0, 0);
#ifdef DEBUG
      fprintf (stdout,"Factor found \n");
#endif
    }
}

/*----------------------------------------------------------------------  
  RestEquivalence
  ----------------------------------------------------------------------*/
static void RestEquivalence (Restruct resctx)
{
    int                 i, j, l;
    char *SEmp;
    char *DEmp;

   DEmp = resctx->RDestPrint;
   SEmp = resctx->RSrcPrint->SPrint;
   /* si les empreintes n'ont pas la meme longueur, ce n'est pas la peine */
   l = strlen (SEmp);
   if ((size_t)l != strlen (DEmp))
     {
       resctx->RRelation = NONE;
       return;
     }
   /* compare chcun des symboles */
   for (i = 0, j = 0; i < l; i++, j++)
     if (RestMatchSymb (resctx, i, j) == NONE)
       {
         resctx->RRelation = NONE;
         return;
       }
   resctx->RRelation = EQUIVALENCE;
#ifdef DEBUG
   fprintf (stdout,"Equiv found \n");
#endif
   for (i = 0, j = 0; i < l; i++, j++)
     RestCoupler (resctx, i, j, 0, 0); 
}
/*----------------------------------------------------------------------  
  RestCalcIndRec
  calcule la table d'indice des noeuds récursifs
  ----------------------------------------------------------------------*/
static void RestCalcIndRec (int **tabIndices, char *emp, int len, Restruct resctx)
{
  int j, k;

  for (j = 0; j < len ;j++)
    {
      if (emp[j] != '@')
	(*tabIndices)[j] = 0;
      else
	for (k = 0; k < j; k++)
	  {
	    if (resctx->RDestNodes[k] == resctx->RDestNodes[j]->TRecursive)
	      {
		(*tabIndices)[j] = k;
		k = j;
	      }
	  }
    }
}

/*----------------------------------------------------------------------  
  RestCluster
  recherche d'une relation de massif d'apres l'algo de Frilley. 

  modif 07/97 : RestMatchSymb prend en compte les types reels des noeuds
  coorespondant au symboles compares et renvoie IDENTITE si ils sont 
  egaux, on continue la comparaisons sur les symboles de fin des noeuds
  consideres
  ----------------------------------------------------------------------*/
static void RestCluster (Restruct resctx)
{
  char               *V, *CSrc, *CDst; /* empreintes */
  int                 LgU, LgV, LgMax;
  int		      i, j;
  int                 TopMS;   /* sommet de la pile de massif */
  TyClusterStack       *ClusterStack;      /* Pile de massif */
  TyRelation	      rel;
  ThotBool            stop;
  int                 level;
  int		      *indrec;
  int                 RecFirstIndex = 0;

  CSrc = resctx->RSrcPrint->SPrint;
  CDst = resctx->RDestPrint;
  V = &CDst[-1];
  LgU = strlen (CSrc);
  LgV = strlen (CDst);
  indrec = (int*)TtaGetMemory (LgV * sizeof(int));
  RestCalcIndRec (&indrec, CDst, LgV, resctx);
  if (LgU > LgV)
    LgMax = LgU;
  else
    LgMax = LgV;
  /* creer la pile de massif a vide */
  TopMS = 0;
  ClusterStack = (TyClusterStack *) TtaGetMemory((LgMax + 1) * sizeof (TyClusterStack));
  ClusterStack[0].Emp = SPACE;
  ClusterStack[0].Inverse = SPACE;
  ClusterStack[0].Retour = -1;
  ClusterStack[0].Compteur = -1;

  i = 1;
  j = 1;
  while (((i <= LgU) && (j <= LgV)) || 
	 (j == LgV+1 && 
	  ClusterStack[TopMS].Compteur == 0 && 
	  ClusterStack[TopMS].Emp == '@'))
    {
      if (ClusterStack[TopMS].Compteur == 0 && 
	  ClusterStack[TopMS].Emp == '@' &&
	  j != indrec[ClusterStack[TopMS].Retour - 1] + 1)
	{
#ifdef DEBUG
	  printf (" <<<<  %d -> %d \n",j-1,ClusterStack[TopMS].Retour);
#endif
	  j = ClusterStack[TopMS].Retour + 1;
	  TopMS -= 1;        /* Depiler */
	}
      else
	{
	  rel = RestMatchSymb (resctx, i - 1, j - 1);
	  if (rel == REC_MASK)
	    { /* on a rencontre un symbole de recursion dans la cible */
	      if (ClusterStack[TopMS].Emp != '@' && i > 1)
		/* on a pas de symbole de recursion sur le haut de la pile */
		{ /* on developpe la recursion trans m6 */
#ifdef DEBUG
		  printf (" >>>>  %d -> %d \n",j-1,indrec [j - 1] -1);
#endif
		  TopMS += 1;    
		  ClusterStack[TopMS].Emp = '@';
		  ClusterStack[TopMS].Retour = j;
		  ClusterStack[TopMS].Compteur = 0;
		  RecFirstIndex = j;
		  j = indrec [j - 1];
		}
	      /* sinon trans m7*/
	    }

	  /* u[i] = v[j] et v[j]=^.Inv et ^.Emp!=@ : trans m1 */
	  else if (rel != NONE && V[j] == ClusterStack[TopMS].Inverse && ClusterStack[TopMS].Emp != '@')
	    if (ClusterStack[TopMS].Compteur == 0)    /*180794 */
	      {
		RestCoupler (resctx, i - 1, j - 1, 0, 0);
		TopMS -= 1;        /* Depiler */
		i += 1;
	      }
	    else            /*180794 */
	      ClusterStack[TopMS].Compteur -= 1;     /*180794 */

	  /* u[i] = v[j] et v[j] in S  : trans m'1 */
	  else if ((rel != NONE) && ((V[j] == '{') || (V[j] == '(') || (V[j] == '[')))
	    {                /* on n'empile que les constructeurs */
	      TopMS += 1;    /* empiler */
	      ClusterStack[TopMS].Emp = V[j];
	      switch (V[j])
		{
		case '{':
		  ClusterStack[TopMS].Inverse = '}';
		  break;
		case '(':
		  ClusterStack[TopMS].Inverse = ')';
		  break;
		case '[':
		  ClusterStack[TopMS].Inverse = ']';
		  break;
		default:
		  ClusterStack[TopMS].Inverse = V[j];
		} 
	      ClusterStack[TopMS].Retour = i;
	      ClusterStack[TopMS].Compteur = 0;
	      if (TopMS > 1 && ClusterStack[TopMS-1].Emp == '@')
		{
		  RestCoupler (resctx, i - 1, j - 1, RecFirstIndex, ClusterStack[TopMS-1].Retour);
		}
	      else
		RestCoupler (resctx, i - 1, j - 1, 0, 0);
	      if (rel == IDENTITE)
		{
		  if (CSrc[i - 1] != '@')
		    {
		      /* recherche la parenthese correspondante dans la source */
		      level = -1;
		      stop = FALSE;
		      while (!stop)
			{
			  stop = (CSrc[i - 1] == ClusterStack[TopMS].Inverse &&
				  level == 0);
			  if (!stop)
			    {
			      if (CSrc[i - 1] == ClusterStack[TopMS].Inverse)
				level--;
			      if (CSrc[i - 1] == ClusterStack[TopMS].Emp)
				level++;
			      i++;
			    } 
			}
		    }
		  if (V[j] != '@')
		    {
		      /* recherche la parenthese correspondante dans la destination */
		      level = -1;
		      stop = FALSE;
		      while (!stop)
			{
			  stop = (V[j] == ClusterStack[TopMS].Inverse &&
				  level == 0);
			  if (!stop)
			    {
			      if (V[j] == ClusterStack[TopMS].Inverse)
				level--;
			      if (V[j] == ClusterStack[TopMS].Emp)
				level++;
			      j++;
			    }
			}
		    }
		}
	      i += 1;
	    }
	 
	  
	  /* u[i]!=v[j] et ^.Emp = @ : change le compteur de top de pile */
	  else if (ClusterStack[TopMS].Emp == '@')
	    {
	      /*  transition m9 */
	      if ((V[j] == '{') || (V[j] == '(') || (V[j] == '['))
		ClusterStack[TopMS].Compteur += 1;
	      /* transition m8 */
	      if ((V[j] == '}') || (V[j] == ')') || (V[j] == ']'))
		ClusterStack[TopMS].Compteur -= 1;
	    }
	      
	  else if (rel != NONE)
	    { /* on couple les types de base sans empiler*/
	      RestCoupler (resctx, i - 1, j - 1, 0, 0);
	      i += 1;
	    }
	      
	  else if (rel == NONE && V[j] == ClusterStack[TopMS].Inverse)
	    if (ClusterStack[TopMS].Compteur > 0)
	      /* transition m2 */
	      ClusterStack[TopMS].Compteur -= 1;
	    else
	      {
		int                k;
		/* trans m3 : retour dans u, supprimer les couplages errones */
		for (k = i - 1; k >= ClusterStack[TopMS].Retour; k--)
		  if (!RestDeCoupler (resctx, k - 1))
		    {
		      RestEraseCouple (resctx);
		      resctx->RRelation = NONE;
		      return;
		    }
		i = ClusterStack[TopMS].Retour;
		TopMS -= 1;        /* Depiler */
	      }

	  /* transition m4 */
	  else if (rel == NONE && V[j] == ClusterStack[TopMS].Emp) /*180794 */
	    ClusterStack[TopMS].Compteur += 1;
	  
	  /* par defaut : transition m5 */
	  j += 1;
	}
    }
  TtaFreeMemory (ClusterStack);
  
  if (i > LgU)
    {
#ifdef DEBUG
      fprintf (stderr, "Relation Massif trouvee\n");
#endif
      resctx->RRelation = CLUSTER;
      return;
    }
  else
    {
      RestEraseCouple (resctx);
      resctx->RRelation = NONE;
      return;
    }
}     

/*----------------------------------------------------------------------  
  RestMatchPrints
  ----------------------------------------------------------------------*/
void RestMatchPrints (Restruct resctx)
{
  RestEquivalence (resctx);
  if (resctx->RRelation == NONE)
     RestFacteur (resctx);
  if (resctx->RRelation == NONE)
     RestCluster (resctx);
}
 
