#include "thot_gui.h"
#include "thot_sys.h"
#include "tree.h"
#include "application.h"
#include "view.h"
#include "selection.h"
#include "conststr.h"
#include "typestr.h"
#define THOT_EXPORT extern
#include "constres.h"


/*----------------------------------------------------------------------  
  RestCoupler
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean RestCoupler (Restruct restr, int indSrc, int indDst)
#else  /* __STDC__ */
static boolean RestCoupler (restr, indSrc, indDst)
Restruct restr;
int indSrc;
int indDst;
#endif  /* __STDC__ */
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
      restr->RCoupledNodes[indSrc] = restr->RDestNodes[indDst];
      return TRUE;
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------  
  RestDeCoupler
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean RestDeCoupler (Restruct resctx, int indSrc)
#else  /* __STDC__ */
static boolean RestDeCoupler (resctx, indSrc)
Restruct resctx;
int indSrc;
#endif  /* __STDC__ */
{
#ifdef DEBUG
  fprintf (stdout, "      Decoupler source : %d %c\n",indSrc, resctx->RSrcPrint->SPrint[indSrc]);
#endif
  if (indSrc < 0 || resctx->RCoupledNodes[indSrc] == NULL)
    {
#ifdef DEBUG
      fprintf(stderr,"      erreur indice decoupler\n");
#endif
      return FALSE;
    }
  resctx->RCoupledNodes[indSrc] = NULL;
  return TRUE;
}  


/*----------------------------------------------------------------------  
  RestEraseCouple
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean RestEraseCouple (Restruct resctx)
#else  /* __STDC__ */
static boolean RestEraseCouple (resctx)
Restruct resctx;
#endif  /* __STDC__ */
{
  int i;
  
#ifdef DEBUG
  fprintf (stdout, "      RAZ couples \n");
#endif
  for (i=0; i<SIZEPRINT; i++)
    resctx->RCoupledNodes[i] = NULL;
  return TRUE;
} 

/*----------------------------------------------------------------------  
  RestMatchSymb
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static TyRelation RestMatchSymb (Restruct resctx, int ISrc, int IDst)
#else  /* __STDC__ */
static TyRelation RestMatchSymb (Restruct resctx, ISrc, IDst)
Restruct resctx;
int ISrc;
int IDst;
#endif  /* __STDC__ */
{
   TyRelation result;
   char CSrc, CDst;
   char *PSrc;
   char *PDst;

   PSrc = resctx->RSrcPrint->SPrint;
   PDst = resctx->RDestPrint;
   result = NONE;
   if (TtaSameSSchemas (RContext->CSrcSchema, (resctx->RDestType).ElSSchema) &&
       resctx->RSrcPrint->SNodes[ISrc] != NULL &&
       resctx->RDestNodes[IDst] != NULL &&
       resctx->RSrcPrint->SNodes[ISrc]->TypeNum == resctx->RDestNodes[IDst]->TypeNum &&
       resctx->RSrcPrint->SPrint[ISrc] == resctx->RDestPrint[IDst])
       result = IDENTITE;
   if (result == NONE)
     {
       CSrc = PSrc[ISrc];
       CDst = PDst[IDst];
       if (CDst != CSrc)
	 {
	   if (CDst == 'U' && 
	       (CSrc == 'T' || CSrc == 'G' || CSrc == 'S' || CSrc == 'R' || CSrc == 'P'))
	     result = EQUIVALENCE;
	 }
       else
	 {
	   if (CSrc == 'N') /* deux natures */
	     ;
	   if (CSrc == 'R') /* deux references */
	     ;
	   result = EQUIVALENCE;
	 }
     }
   return result;
}

/*----------------------------------------------------------------------  
  RestCalcShift
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int *RestCalcShift (char *pattern)
#else  /* __STDC__ */
static int *RestCalcShift (pattern)
char *pattern;
#endif  /* __STDC__ */
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
#ifdef __STDC__
static void RestFacteur (Restruct resctx)
#else  /* __STDC__ */
static void RestFacteur (resctx)
Restruct resctx;
#endif  /* __STDC__ */
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
	RestCoupler (resctx,i,j);
#ifdef DEBUG
      fprintf (stdout,"Factor found \n");
#endif
    }
}

/*----------------------------------------------------------------------  
  RestEquivalence
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RestEquivalence (Restruct resctx)
#else  /* __STDC__ */
static void RestEquivalence (resctx)
Restruct resctx;
#endif  /* __STDC__ */
{
    int                 i, j, l;
    char *SEmp;
    char *DEmp;

   DEmp = resctx->RDestPrint;
   SEmp = resctx->RSrcPrint->SPrint;
   /* si les empreintes n'ont pas la meme longueur, ce n'est pas la peine */
   l = strlen (SEmp);
   if (l != strlen (DEmp))
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
     RestCoupler (resctx, i, j); 
}

/*----------------------------------------------------------------------  
  RestCluster
  recherche d'une relation de massif d'apres l'algo de Frilley. 

  modif 07/97 : RestMatchSymb prend en compte les types reels des noeuds
  coorespondant au symboles compares et renvoie IDENTITE si ils sont 
  egaux, on continue la comparaisons sur les symboles de fin des noeuds
  consideres
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void RestCluster (Restruct resctx)
#else  /* __STDC__ */
static void RestCluster (resctx)
Restruct resctx;
#endif  /* __STDC__ */
{
  char               *V, *CSrc, *CDst; /* empreintes */
  int                 LgU, LgV, LgMax;
  int		      i, j;
  int                 TopMS;   /* sommet de la pile de massif */
  TyClusterStack       *ClusterStack;      /* Pile de massif */
  TyRelation	      rel;
  boolean             stop;
  int                 level;

  CSrc = resctx->RSrcPrint->SPrint;
  CDst = resctx->RDestPrint;
  V = &CDst[-1];
  LgU = strlen (CSrc);
  LgV = strlen (CDst);
  if (LgU > LgV)
     LgMax = LgU;
  else
     LgMax = LgV;
  /* creer la pile de massif a vide */
  TopMS = 0;
  ClusterStack = (TyClusterStack *) TtaGetMemory((LgMax + 1) * sizeof (TyClusterStack));
  ClusterStack[0].Emp = ' ';
  ClusterStack[0].Inverse = ' ';
  ClusterStack[0].Retour = -1;
  ClusterStack[0].Compteur = -1;

  i = 1;
  j = 1;
  while ((i <= LgU) && (j <= LgV))
    {
     rel = RestMatchSymb (resctx, i - 1, j - 1);
     if (rel !=NONE) /*== EQUIVALENCE)*/
        {
          if (V[j] == ClusterStack[TopMS].Inverse)
            if (ClusterStack[TopMS].Compteur == 0)    /*180794 */
              {
		RestCoupler (resctx, i - 1, j - 1);
		TopMS -= 1;        /* Depiler */
		i += 1;
              }
            else            /*180794 */
              ClusterStack[TopMS].Compteur -= 1;     /*180794 */
          else if ((V[j] == '{') || (V[j] == '(') || (V[j] == '['))
            {                /* on n'empile que les constructeurs */
              TopMS += 1;   /* empiler */
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
	      RestCoupler (resctx, i - 1, j - 1);
	      if (rel == IDENTITE)
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
	      i += 1;
	   
            }
          else
            { 
	      RestCoupler (resctx, i - 1, j - 1);
              i += 1;
            }
        }
      else
        {
          if (V[j] == ClusterStack[TopMS].Inverse)
            if (ClusterStack[TopMS].Compteur > 0)
              ClusterStack[TopMS].Compteur -= 1;
            else
              {
                int                k;
                /* retour en arriere, supprimer les couplages errones */
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
          if (V[j] == ClusterStack[TopMS].Emp)         /*180794 */
            ClusterStack[TopMS].Compteur += 1;
        }
      j += 1;
    }
  TtaFreeMemory ((char *)ClusterStack);

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

#ifdef __STDC__
void RestMatchPrints (Restruct resctx)
#else  /* __STDC__ */
void RestMatchPrints (resctx)
Restruct resctx;
int indDest;
boolean effective;
boolean avecCouplage;
#endif  /* __STDC__ */
{
  RestEquivalence (resctx);
  if (resctx->RRelation == NONE)
     RestFacteur (resctx);
  if (resctx->RRelation == NONE)
     RestCluster (resctx);
}
 
