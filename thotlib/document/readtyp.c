/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
   rdschtyp.c : chargement d'un schema de typographie
 */
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "typetyp.h"
#include "libmsg.h"
#define THOT_EXPORT extern
#include "platform_tv.h"

#include "memory_f.h"
#include "fileaccess.h"
#include "message.h"

static ThotBool     erreurSchema;

/*----------------------------------------------------------------------
   erreurTypo positionne erreurSchema dans le cas d'une erreur     
   de lecture.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                erreurTypo (int err)

#else  /* __STDC__ */
void                erreurTypo (err)
int                 err;

#endif /* __STDC__ */
{
   erreurSchema = TRUE;
}

/*----------------------------------------------------------------------
   RdPtrBlocTypo retourne un pointeur sur le bloc de regles suivant   
   ou NULL s'il n'y a pas de bloc suivant.                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrRegleTypo RdPtrBlocTypo (BinFile fich, PtrRegleTypo * nextr)

#else  /* __STDC__ */
static PtrRegleTypo RdPtrBlocTypo (fich, nextr)
BinFile             fich;
PtrRegleTypo       *nextr;

#endif /* __STDC__ */

{
   char                c;
   PtrRegleTypo        bloc;

   TtaReadByte (fich, &c);
   if (c == EOS)
      bloc = NULL;
   else
      bloc = *nextr;
   return bloc;
}


/*----------------------------------------------------------------------
   RdPtrAttrTypo retourne un pointeur sur le bloc attribut suivant    
   ou NULL s'il n'y a pas de bloc attribut suivant.        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrRTypoAttribut RdPtrAttrTypo (BinFile fich, PtrRTypoAttribut * nexta)

#else  /* __STDC__ */
static PtrRTypoAttribut RdPtrAttrTypo (fich, nexta)
BinFile             fich;
PtrRTypoAttribut   *nexta;

#endif /* __STDC__ */

{
   char                c;
   PtrRTypoAttribut    attrtyp;

   TtaReadByte (fich, &c);
   if (c == EOS)
      attrtyp = NULL;
   else
     {
	attrtyp = *nexta;
	/* acquiert un buffer pour l'attribut suivant */
	if ((*nexta = (PtrRTypoAttribut) TtaGetMemory (sizeof (RTypoAttribut))) == NULL)
	   erreurTypo (1);
     }
   return attrtyp;
}


/*----------------------------------------------------------------------
   RdPtrRegle retourne un pointeur sur la regle suivante ou        
   NULL s'il n'y a pas de regle suivante.                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrRegleTypo RdPtrRegle (BinFile fich, PtrRegleTypo * nextr)

#else  /* __STDC__ */
static PtrRegleTypo RdPtrRegle (fich, nextr)
BinFile             fich;
PtrRegleTypo       *nextr;

#endif /* __STDC__ */

{
   char                c;
   PtrRegleTypo        regtyp;

   TtaReadByte (fich, &c);
   if (c == EOS)
      regtyp = NULL;
   else
      regtyp = *nextr;
   return regtyp;
}

/*----------------------------------------------------------------------
   RdPtrFunction retourne un pointeur sur la fonction suivante ou  
   NULL s'il n'y a pas de fonction suivante.               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static PtrTypoFunction RdPtrFunction (BinFile fich, PtrTypoFunction * nextf)

#else  /* __STDC__ */
static PtrTypoFunction RdPtrFunction (fich, nextf)
BinFile             fich;
PtrTypoFunction    *nextf;

#endif /* __STDC__ */

{
   char                c;
   PtrTypoFunction     fcttyp;

   TtaReadByte (fich, &c);
   if (c == EOS)
      fcttyp = NULL;
   else
      fcttyp = *nextf;
   return fcttyp;
}

/*----------------------------------------------------------------------
   RdTypeCondTypo lit un type de condition de regle de typographie.   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static TypeCondTypo RdTypeCondTypo (BinFile fich)

#else  /* __STDC__ */
static TypeCondTypo RdTypeCondTypo (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   TypeCondTypo        type;

   if (!TtaReadByte (fich, &c))
      erreurTypo (1);
   else
      switch (c)
	    {
	       case MTyPremier:
		  type = TyPremier;
		  break;
	       case MTyDernier:
		  type = TyDernier;
		  break;
	       case MTyDansType:
		  type = TyDansType;
		  break;
	       case MTyApresType:
		  type = TyApresType;
		  break;
	       case MTyAvantType:
		  type = TyAvantType;
		  break;
	       case MTyLangue:
		  type = TyLangue;
		  break;
	       case MTyFonction:
		  type = TyFonction;
		  break;
	       default:
		  erreurTypo (1);	/* erreur de codage */
		  type = TyPremier;
		  break;
	    }
   return type;
}


/*----------------------------------------------------------------------
   RdTypeRegleTypo lit un type de regle de typographie.            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static TypeFunct    RdTypeRegleTypo (BinFile fich)

#else  /* __STDC__ */
static TypeFunct    RdTypeRegleTypo (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   TypeFunct           type;

   if (!TtaReadByte (fich, &c))
      erreurTypo (1);
   else
      switch (c)
	    {
	       case MTyCapital:
		  type = TyCapital;
		  break;
	       case MTyWord:
		  type = TyWord;
		  break;
	       case MTySpace:
		  type = TySpace;
		  break;
	       case MTyPunct:
		  type = TyPunct;
		  break;
	       case MTyPair:
		  type = TyPair;
		  break;
	       case MTyDistance:
		  type = TyDistance;
		  break;
	       case MTyExponent:
		  type = TyExponent;
		  break;
	       case MTyAbbrev:
		  type = TyAbbrev;
		  break;
	       case MTyAttribute:
		  type = TyAttribute;
		  break;
	       default:
		  erreurTypo (1);	/* erreur de codage */
		  type = TyCapital;
		  break;
	    }

   return type;
}


/*----------------------------------------------------------------------
   ReadTypoFunctions  lit une suite de focntions chainees             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ReadTypoFunctions (BinFile fich, PtrTypoFunction * pF, PtrTypoFunction * nextf)

#else  /* __STDC__ */
static void         ReadTypoFunctions (fich, pF, nextf)
BinFile             fich;
PtrTypoFunction    *pF;
PtrTypoFunction    *nextf;

#endif /* __STDC__ */

{
   PtrTypoFunction     f;	/* pointeur sur la premiere fct qui va etre lue */
   PtrTypoFunction     pFc1;

   *pF = *nextf;
   if (!erreurSchema)
      do
	{
	   f = *nextf;		/* acquiert un buffer pour la fct suivante */
	   if ((*nextf = (PtrTypoFunction) TtaGetMemory (sizeof (TypoFunction))) == NULL)
	      erreurTypo (1);
	   /* lit une fonction */
	   pFc1 = f;
	   pFc1->TFSuiv = RdPtrFunction (fich, nextf);
	   pFc1->TFTypeFunct = RdTypeRegleTypo (fich);
	   if (!erreurSchema)
	     {
		TtaReadName (fich, pFc1->TFIdent);
		/* passe a la fonction suivante */
		if (pFc1->TFSuiv != NULL)
		   pFc1->TFSuiv = *nextf;
	     }
	}
      while (!(f->TFSuiv == NULL || erreurSchema));
}


/*----------------------------------------------------------------------
   ReadReglesTypo  lit une suite de regles chainees                   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ReadReglesTypo (BinFile fich, PtrRegleTypo * pR, PtrRegleTypo * nextr)

#else  /* __STDC__ */
static void         ReadReglesTypo (fich, pR, nextr)
BinFile             fich;
PtrRegleTypo       *pR;
PtrRegleTypo       *nextr;

#endif /* __STDC__ */

{
   PtrRegleTypo        r;	/* pointeur sur la premiere regle qui va etre lue */
   PtrRegleTypo        pRe1;
   int                 nbcond;
   TypoCondition      *TyCond;

   *pR = *nextr;
   if (!erreurSchema)
      do
	{
	   r = *nextr;		/* acquiert un buffer pour la regle suivante */
	   if ((*nextr = (PtrRegleTypo) TtaGetMemory (sizeof (RegleTypo))) == NULL)
	      erreurTypo (1);
	   /* lit une regle */
	   pRe1 = r;
	   pRe1->RTyRegleSuiv = RdPtrRegle (fich, nextr);
	   pRe1->RTyTypeFunct = RdTypeRegleTypo (fich);
	   if (!erreurSchema)
	     {
		TtaReadName (fich, pRe1->RTyIdentFunct);
		TtaReadShort (fich, &pRe1->RTyNbCond);

		for (nbcond = 1; nbcond <= pRe1->RTyNbCond; nbcond++)
		  {
		     TyCond = &pRe1->RTyCondition[nbcond - 1];
		     TyCond->CondTypeCond = RdTypeCondTypo (fich);
		     TtaReadBool (fich, &TyCond->CondNegative);
		     TtaReadShort (fich, &TyCond->CondTypeElem);
		     switch (TyCond->CondTypeCond)
			   {
			      case TyAvantType:
			      case TyApresType:
				 TtaReadBool (fich, &TyCond->CondOpTypeIn);
				 break;
			      case TyLangue:
			      case TyFonction:
				 TtaReadName (fich, TyCond->CondNom);
				 break;
			      default:;
				 break;
			   }
		  }
		/* passe a la regle suivante */
		if (pRe1->RTyRegleSuiv != NULL)
		   pRe1->RTyRegleSuiv = *nextr;
	     }
	}
      while (!(r->RTyRegleSuiv == NULL || erreurSchema));
}


/*----------------------------------------------------------------------
   ReadReglesAttr lit les regles de typographie de l'attribut de   
   numero att appartenant au schema de structure SS.       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ReadReglesAttr (BinFile fich, int att, PtrRTypoAttribut * pA, PtrRTypoAttribut * nextA, PtrRegleTypo * nextR, PtrSSchema * SS)

#else  /* __STDC__ */
static void         ReadReglesAttr (fich, att, pA, nextA, nextR, SS)
BinFile             fich;
int                 att;
PtrRTypoAttribut   *pA;
PtrRTypoAttribut   *nextA;
PtrRegleTypo       *nextR;
PtrSSchema         *SS;

#endif /* __STDC__ */

{
   int                 j;
   PtrRTypoAttribut    pRTy1;
   TyCasAttrNum       *pTyC1;

   pRTy1 = *pA;
   if (!erreurSchema)
     {
	/* lit un attribut */
	TtaReadShort (fich, &pRTy1->RTyATypeElem);
	switch ((*SS)->SsAttribute[att - 1].AttrType)
	      {
		 case AtNumAttr:
		    TtaReadShort (fich, &pRTy1->RTyANbCas);
		    if (!erreurSchema)
		       for (j = 1; j <= pRTy1->RTyANbCas; j++)
			  pRTy1->RTyACas[j - 1].TyANBlocRegles = RdPtrBlocTypo (fich, nextR);
		    for (j = 1; j <= pRTy1->RTyANbCas; j++)
		       if (!erreurSchema)
			 {
			    pTyC1 = &pRTy1->RTyACas[j - 1];
			    TtaReadSignedShort (fich, &pTyC1->TyANBorneInf);
			    TtaReadSignedShort (fich, &pTyC1->TyANBorneSup);
			    ReadReglesTypo (fich, &pTyC1->TyANBlocRegles, nextR);
			 }
		    break;
		 case AtTextAttr:
		    TtaReadName (fich, pRTy1->RTyATxtVal);
		    pRTy1->RTyATxt = RdPtrBlocTypo (fich, nextR);
		    ReadReglesTypo (fich, &pRTy1->RTyATxt, nextR);
		    break;
		 case AtReferenceAttr:
		    pRTy1->RTyARefPremRegle = RdPtrBlocTypo (fich, nextR);
		    ReadReglesTypo (fich, &pRTy1->RTyARefPremRegle, nextR);
		    break;
		 case AtEnumAttr:
		    for (j = 0; j <= (*SS)->SsAttribute[att - 1].AttrNEnumValues; j++)
		       pRTy1->RTyAValEnum[j] = RdPtrBlocTypo (fich, nextR);
		    for (j = 0; j <= (*SS)->SsAttribute[att - 1].AttrNEnumValues; j++)
		       if (pRTy1->RTyAValEnum[j] != NULL)
			  ReadReglesTypo (fich, &pRTy1->RTyAValEnum[j], nextR);
		    break;
		 default:;
		    break;
	      }
     }
}


/*----------------------------------------------------------------------
   RdSchTyp lit un fichier contenant un schema de typographie et   
   le charge en memoire. Retourne un pointeur sur le       
   schema de typographie en memoire si chargement reussi,  
   NULL si echec.                                          
   - fname: nom du fichier a lire, sans le suffixe .TYP    
   - SS: schema de structure correspondant, deja rempli    
   si SsRootElem != 0. Charge le schema de structure 
   si SsRootElem == 0.                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrSchTypo          RdSchTypo (Name fname, PtrSSchema SS)

#else  /* __STDC__ */
PtrSchTypo          RdSchTypo (fname, SS)
Name                fname;
PtrSSchema          SS;

#endif /* __STDC__ */

{
   int                 i;
   ThotBool            ret;
   PtrSchTypo          pSchT;
   PathBuffer          DirBuffer;
   BinFile             fich;
   char                texte[MAX_TXT_LEN];
   PtrTypoFunction     nextf;
   PtrRegleTypo        nextr;
   PtrRTypoAttribut    nexta;
   int                 NbElemStructInitial;

   ret = TRUE;
   erreurSchema = FALSE;
   pSchT = NULL;
   /* compose le nom du fichier a ouvrir avec le nom du directory */
   /* des schemas... */
   strncpy (DirBuffer, SchemaPath, MAX_PATH);
   MakeCompleteName (fname, TEXT("TYP"), DirBuffer, texte, &i);

   /* ouvre le fichier */
   fich = TtaReadOpen (texte);
   if (fich == 0)
     {
	strncpy (texte, fname, MAX_PATH);
	strcat (texte, ".TYP");
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_MISSING_FILE), texte);
     }
   else
     {
	if ((pSchT = (PtrSchTypo) TtaGetMemory (sizeof (SchTypo))) == NULL)
	   erreurTypo (1);
	if ((nextf = (PtrTypoFunction) TtaGetMemory (sizeof (TypoFunction))) == NULL)
	   erreurTypo (1);
	if ((nextr = (PtrRegleTypo) TtaGetMemory (sizeof (RegleTypo))) == NULL)
	   erreurTypo (1);
	if ((nexta = (PtrRTypoAttribut) TtaGetMemory (sizeof (RTypoAttribut))) == NULL)
	   erreurTypo (1);

	/* lit la partie fixe du schema de typographie */
	TtaReadName (fich, &pSchT->STyNomStruct);
	TtaReadShort (fich, &pSchT->STyStructCode);
	pSchT->STySuivant = NULL;

	if (SS->SsRootElem == 0 && !erreurSchema)
	   /* lit le schema de structure */
	   ret = ReadStructureSchema (pSchT->STyNomStruct, SS);
	if (!ret || pSchT->STyStructCode != SS->SsCode)
	  {
	     TtaFreeMemory (pSchT);
	     /* Schemas incompatibles */
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_OLD_SCH_TO_RECOMP), texte);
	     erreurTypo (1);
	  }
	else
	  {
	     if (SS->SsFirstDynNature == 0)
		NbElemStructInitial = SS->SsNRules;
	     else
		NbElemStructInitial = SS->SsFirstDynNature - 1;

	     if (!erreurSchema)
		for (i = 1; i <= NbElemStructInitial; i++)
		   TtaReadBool (fich, &pSchT->STyElemAlinea[i - 1]);
	     if (!erreurSchema)
		/* lit la chaine des fonctions */
		ReadTypoFunctions (fich, &pSchT->STyFunction, &nextf);
	     if (!erreurSchema)
		for (i = 1; i <= NbElemStructInitial; i++)
		   pSchT->STyRegleElem[i - 1] = RdPtrBlocTypo (fich, &nextr);
	     if (!erreurSchema)
		for (i = 1; i <= NbElemStructInitial; i++)
		   TtaReadBool (fich, &pSchT->STyElemHeritAttr[i - 1]);
	     if (!erreurSchema)
		for (i = 1; i <= SS->SsNAttributes; i++)
		   pSchT->STyAttribSem[i - 1] = RdPtrAttrTypo (fich, &nexta);

	     /* lit la chaine des regles des elements structures */
	     for (i = 1; (i <= NbElemStructInitial && !erreurSchema); i++)
		if (pSchT->STyRegleElem[i - 1] != NULL)
		   ReadReglesTypo (fich, &pSchT->STyRegleElem[i - 1], &nextr);

	     /* lit les blocs de regles des attributs */
	     for (i = 1; (i <= SS->SsNAttributes && !erreurSchema); i++)
		if (pSchT->STyAttribSem[i - 1] != NULL)
		   ReadReglesAttr (fich, i, &pSchT->STyAttribSem[i - 1], &nexta, &nextr, &SS);

	     free (nextf);	/* on avait anticipe... */
	     free (nextr);	/* on avait anticipe... */
	     free (nexta);	/* on avait anticipe... */
	  }
	/* ferme le fichier */
	TtaReadClose (fich);
     }
   if (erreurSchema)
     {
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_SCHEMA_NOT_FIND), fname);
	return NULL;
     }
   else
      return pSchT;		/* rend la valeur de retour */
}

/* End Of Module rdschtyp */
