/***
 *** Copyright (c) 1996 INRIA, All rights reserved
 ***/

/*----------------------------------------------------------------------
   
   Projet THOT                              
   
   Ce module sauve un schema de typographie compile'.              
   
  ----------------------------------------------------------------------*/


#include "thot_sys.h"
/* constantes */
#include "constmedia.h"
/* types */
#include "typemedia.h"
#include "typetyp.h"
#include "fileaccess.h"
#include "fileaccess_f.h"

static BinFile      outfile;

/*----------------------------------------------------------------------
   WriteShort ecrit un entier court sur deux octets dans le fichier   
   de sortie.                                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteShort (int n)
#else  /* __STDC__ */
void                WriteShort (n)
int                 n;

#endif /* __STDC__ */

{
   TtaWriteByte (outfile, (char) (n / 256));
   TtaWriteByte (outfile, (char) (n % 256));
}

/*----------------------------------------------------------------------
   WriteSignedShort  ecrit dans le fichier de sortie un entier signe' sur 
   2 octets                                            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteSignedShort (int n)

#else  /* __STDC__ */
void                WriteSignedShort (n)
int                 n;

#endif /* __STDC__ */

{

   if (n >= 0)
      WriteShort (n);
   else
      WriteShort (n + 65536);
}


/*----------------------------------------------------------------------
   wrnom   ecrit un nom dans le fichier de sortie                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                wrnom (Name N)

#else  /* __STDC__ */
void                wrnom (N)
Name                N;

#endif /* __STDC__ */

{
   int                 i;

   i = 0;
   do
     {
	TtaWriteByte (outfile, N[i]);
	i++;
     }
   while (N[i - 1] != '\0');
}


/*----------------------------------------------------------------------
   wrnomproc  ecrit un nom de procedure dans le fichier de sortie  
   c.a.d. ajoute _ devant le nom de la fonction avant d'ecrire le nom 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                wrnomproc (Name N)

#else  /* __STDC__ */
void                wrnomproc (N)
Name                N;

#endif /* __STDC__ */

{
   int                 i;

   /* le _ sera supprime pour solaris2 ... */
   TtaWriteByte (outfile, '_');
   i = 0;
   do
     {
	TtaWriteByte (outfile, N[i]);
	i++;
     }
   while (N[i - 1] != '\0');
}


/*----------------------------------------------------------------------
   wrptrregle      ecrit 1 si le pointeur existe existe,  0 sinon         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                wrptrregle (PtrRegleTypo b)

#else  /* __STDC__ */
void                wrptrregle (b)
PtrRegleTypo        b;

#endif /* __STDC__ */

{

   if (b == NULL)
      TtaWriteByte (outfile, '\0');
   else
      TtaWriteByte (outfile, '\1');
}


/*----------------------------------------------------------------------
   wrptrfonction      ecrit 1 si le pointeur existe existe,  0 sinon         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                wrptrfonction (PtrTypoFunction b)

#else  /* __STDC__ */
void                wrptrfonction (b)
PtrTypoFunction     b;

#endif /* __STDC__ */

{

   if (b == NULL)
      TtaWriteByte (outfile, '\0');
   else
      TtaWriteByte (outfile, '\1');
}


/*----------------------------------------------------------------------
   wrTypeRegleTypo    ecrit un type de regle de typographie           
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                wrTypeRegleTypo (TypeFunct T)

#else  /* __STDC__ */
void                wrTypeRegleTypo (T)
TypeFunct           T;

#endif /* __STDC__ */

{
   switch (T)
	 {
	    case TyCapital:
	       TtaWriteByte (outfile, MTyCapital);
	       break;
	    case TyWord:
	       TtaWriteByte (outfile, MTyWord);
	       break;
	    case TySpace:
	       TtaWriteByte (outfile, MTySpace);
	       break;
	    case TyPunct:
	       TtaWriteByte (outfile, MTyPunct);
	       break;
	    case TyPair:
	       TtaWriteByte (outfile, MTyPair);
	       break;
	    case TyDistance:
	       TtaWriteByte (outfile, MTyDistance);
	       break;
	    case TyExponent:
	       TtaWriteByte (outfile, MTyExponent);
	       break;
	    case TyAbbrev:
	       TtaWriteByte (outfile, MTyAbbrev);
	       break;
	    case TyAttribute:
	       TtaWriteByte (outfile, MTyAttribute);
	       break;
	 }

}


/*----------------------------------------------------------------------
   wrptrtypofunction       ecrit la liste des noms de fonctions    
   dans le fichier de sortie               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                wrlistfunction (PtrTypoFunction pFunc)

#else  /* __STDC__ */
void                wrlistfunction (pFunc)
PtrTypoFunction     pFunc;

#endif /* __STDC__ */

{
   PtrTypoFunction     pCour;

   pCour = pFunc;
   while (pCour != NULL)
     {
	wrptrfonction (pCour->TFSuiv);
	wrTypeRegleTypo (pCour->TFTypeFunct);
	wrnomproc (pCour->TFIdent);
	pCour = pCour->TFSuiv;
     }

}

/*----------------------------------------------------------------------
   wrbool       ecrit un booleen dans le fichier de sortie                
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                wrbool (boolean b)

#else  /* __STDC__ */
void                wrbool (b)
boolean             b;

#endif /* __STDC__ */

{

   if (b)
      TtaWriteByte (outfile, '\1');
   else
      TtaWriteByte (outfile, '\0');
}

/*----------------------------------------------------------------------
   wrptrbloctypo       ecrit 1 si le pointeur bloc regle existe,  0 sinon 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                wrptrbloctypo (PtrRegleTypo b)

#else  /* __STDC__ */
void                wrptrbloctypo (b)
PtrRegleTypo        b;

#endif /* __STDC__ */

{

   if (b == NULL)
      TtaWriteByte (outfile, '\0');
   else
      TtaWriteByte (outfile, '\1');
}


/*----------------------------------------------------------------------
   wrptrattrtypo      ecrit 1 si le pointeur attribut existe,  0 sinon    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                wrptrattrtypo (PtrRTypoAttribut b)

#else  /* __STDC__ */
void                wrptrattrtypo (b)
PtrRTypoAttribut    b;

#endif /* __STDC__ */

{

   if (b == NULL)
      TtaWriteByte (outfile, '\0');
   else
      TtaWriteByte (outfile, '\1');
}


/*----------------------------------------------------------------------
   wrTypeCond  ecrit un type de condition d'application de regle de   
   typographie                                            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                wrTypeCondTypo (TypeCondTypo T)

#else  /* __STDC__ */
void                wrTypeCondTypo (T)
TypeCondTypo        T;

#endif /* __STDC__ */

{

   switch (T)
	 {
	    case TyPremier:
	       TtaWriteByte (outfile, MTyPremier);
	       break;
	    case TyDernier:
	       TtaWriteByte (outfile, MTyDernier);
	       break;
	    case TyDansType:
	       TtaWriteByte (outfile, MTyDansType);
	       break;
	    case TyAvantType:
	       TtaWriteByte (outfile, MTyAvantType);
	       break;
	    case TyApresType:
	       TtaWriteByte (outfile, MTyApresType);
	       break;
	    case TyLangue:
	       TtaWriteByte (outfile, MTyLangue);
	       break;
	    case TyFonction:
	       TtaWriteByte (outfile, MTyFonction);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteReglesTypo    ecrit une suite de regles de typographie        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteReglesTypo (PtrRegleTypo pR)

#else  /* __STDC__ */
void                WriteReglesTypo (pR)
PtrRegleTypo        pR;

#endif /* __STDC__ */

{
   PtrRegleTypo        CourRegle, NextRegle;
   PtrRegleTypo        pRe1;
   int                 nbcond;
   TypoCondition      *TCond;


   CourRegle = pR;
   while (CourRegle != NULL)
     {
	pRe1 = CourRegle;
	wrptrregle (pRe1->RTyRegleSuiv);
	wrTypeRegleTypo (pRe1->RTyTypeFunct);
	wrnomproc (pRe1->RTyIdentFunct);
	WriteShort (pRe1->RTyNbCond);
	for (nbcond = 1; nbcond <= pRe1->RTyNbCond; nbcond++)
	  {
	     TCond = &pRe1->RTyCondition[nbcond - 1];
	     wrTypeCondTypo (TCond->CondTypeCond);
	     wrbool (TCond->CondNegative);
	     WriteShort (TCond->CondTypeElem);
	     switch (TCond->CondTypeCond)
		   {
		      case TyAvantType:
		      case TyApresType:
			 wrbool (TCond->CondOpTypeIn);
			 break;
		      case TyLangue:
		      case TyFonction:
			 wrnom (TCond->CondNom);
			 break;
		      default:
			 break;
		   }
	  }
	pRe1 = CourRegle;
	NextRegle = pRe1->RTyRegleSuiv;
	free (CourRegle);
	CourRegle = NextRegle;
     }
}


/*----------------------------------------------------------------------
   WriteReglesAttr    ecrit les regles de typo de l'attribut de numero 
   att appartenant au schema de structure pointe' par pSchStr 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteReglesAttr (int att, PtrRTypoAttribut pA, PtrSSchema pSchStr)

#else  /* __STDC__ */
void                WriteReglesAttr (att, pA, pSchStr)
int                 att;
PtrRTypoAttribut    pA;
PtrSSchema          pSchStr;

#endif /* __STDC__ */

{
   int                 i;
   PtrRTypoAttribut    pRTy1;
   TyCasAttrNum       *pTC1;

   if (pA != NULL)
     {
	pRTy1 = pA;
	WriteShort (pRTy1->RTyATypeElem);
	switch (pSchStr->SsAttribute[att - 1].AttrType)
	      {
		 case AtNumAttr:
		    WriteShort (pRTy1->RTyANbCas);
		    for (i = 1; i <= pRTy1->RTyANbCas; i++)
		       wrptrbloctypo (pRTy1->RTyACas[i - 1].TyANBlocRegles);
		    for (i = 1; i <= pRTy1->RTyANbCas; i++)
		      {
			 pTC1 = &pRTy1->RTyACas[i - 1];
			 WriteSignedShort (pTC1->TyANBorneInf);
			 WriteSignedShort (pTC1->TyANBorneSup);
			 WriteReglesTypo (pTC1->TyANBlocRegles);
		      }
		    break;
		 case AtTextAttr:
		    wrnom (pRTy1->RTyATxtVal);
		    wrptrbloctypo (pRTy1->RTyATxt);
		    WriteReglesTypo (pRTy1->RTyATxt);
		    break;
		 case AtReferenceAttr:
		    wrptrbloctypo (pRTy1->RTyARefPremRegle);
		    WriteReglesTypo (pRTy1->RTyARefPremRegle);
		    break;
		 case AtEnumAttr:
		    for (i = 0; i <= pSchStr->SsAttribute[att - 1].AttrNEnumValues; i++)
		       wrptrbloctypo (pRTy1->RTyAValEnum[i]);
		    for (i = 0; i <= pSchStr->SsAttribute[att - 1].AttrNEnumValues; i++)
		       WriteReglesTypo (pRTy1->RTyAValEnum[i]);
		    break;
		 default:
		    break;
	      }
	free (pRTy1);
     }
}


/*----------------------------------------------------------------------
   WrSchTyp        cree le fichier de sortie et y ecrit le schema  
   de typographie                                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WrSchTyp (Name fname, PtrSchTypo pSchTypo, PtrSSchema pSchStr)

#else  /* __STDC__ */
void                WrSchTyp (fname, pSchTypo, pSchStr)
Name                fname;
PtrSchTypo          pSchTypo;
PtrSSchema          pSchStr;

#endif /* __STDC__ */

{
   int                 i;

   /* met le suffixe .TYP a la fin du nom de fichier */
   i = 0;
   while (fname[i] != '.')
      i++;
   fname[i + 1] = 'T';
   fname[i + 2] = 'Y';
   fname[i + 3] = 'P';
   fname[i + 4] = '\0';
   /* cree le fichier */
   outfile = TtaWriteOpen (fname);

   wrnom (pSchTypo->STyNomStruct);
   WriteShort (pSchTypo->STyStructCode);
   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrbool (pSchTypo->STyElemAlinea[i - 1]);
   wrlistfunction (pSchTypo->STyFunction);
   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrptrbloctypo (pSchTypo->STyRegleElem[i - 1]);
   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrbool (pSchTypo->STyElemHeritAttr[i - 1]);
   for (i = 1; i <= pSchStr->SsNAttributes; i++)
      wrptrattrtypo (pSchTypo->STyAttribSem[i - 1]);

   for (i = 1; i <= pSchStr->SsNRules; i++)
      WriteReglesTypo (pSchTypo->STyRegleElem[i - 1]);
   for (i = 1; i <= pSchStr->SsNAttributes; i++)
      WriteReglesAttr (i, pSchTypo->STyAttribSem[i - 1], pSchStr);

   TtaWriteClose (outfile);
}
/* End Of Module wrschtyp */
