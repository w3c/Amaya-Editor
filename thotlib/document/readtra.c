
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   rdschtrad.c : chargement des schemas de traduction
   Chargement des schemas de traduction
   V. Quint     Janvier 1988
   France Logiciel no de depot 88-39-001-00     

 */


#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "consttra.h"
#include "typemedia.h"
#include "typetra.h"
#include "storage.h"

#define EXPORT extern
#include "environ.var"

static boolean      erreurTra = False;

#include "environ.f"
#include "dofile.f"
#include "rdschtra.f"
#include "memory.f"
#include "storage.f"
#include "message.f"


/* ---------------------------------------------------------------------- */
/* |    erreurTraduc positionne erreurTra dans le cas d'une erreur de   | */
/* |            lecture.                                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                erreurTraduc (int err)

#else  /* __STDC__ */
void                erreurTraduc (err)
int                 err;

#endif /* __STDC__ */

{
   printf ("Error %d in translation schema\n", err);
   erreurTra = True;
}


/* ---------------------------------------------------------------------- */
/* |    RdTypeCondTrad lit un type de condition d'application de regle  | */
/* |            de traduction.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static TransCondition RdTypeCondTrad (BinFile fich)

#else  /* __STDC__ */
static TransCondition RdTypeCondTrad (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   TransCondition        type;

   type = TcondFirst;
   if (!BIOreadByte (fich, &c))
      erreurTraduc (1);
   else
      switch (c)
	    {
	       case C_TR_FIRST:
		  type = TcondFirst;
		  break;
	       case C_TR_LAST:
		  type = TcondLast;
		  break;
	       case C_TR_DEFINED:
		  type = TcondDefined;
		  break;
	       case C_TR_REFERRED:
		  type = TcondReferred;
		  break;
	       case C_TR_FIRSTREF:
		  type = TcondFirstRef;
		  break;
	       case C_TR_LAST_REF:
		  type = TcondLastRef;
		  break;
	       case C_TR_WITHIN:
		  type = TcondWithin;
		  break;
	       case C_TR_FIRST_WITHIN:
		  type = TcondFirstWithin;
		  break;
	       case C_TR_ATTRIBUTE:
		  type = TcondAttr;
		  break;
	       case C_TR_PRESENT:
		  type = TcondPresentation;
		  break;
	       case C_TR_PRULE:
		  type = TcondPRule;
		  break;
	       case C_TR_COMMENT:
		  type = TcondComment;
		  break;
	       case C_TR_ALPHABET:
		  type = TcondAlphabet;
		  break;
	       case C_TR_ATTRIBUTES:
		  type = TcondAttributes;
		  break;
	       case C_TR_FIRSTATTR:
		  type = TcondFirstAttr;
		  break;
	       case C_TR_LASTATTR:
		  type = TcondLastAttr;
		  break;
	       case C_TR_COMPUTEDPAGE:
		  type = TcondComputedPage;
		  break;
	       case C_TR_STARTPAGE:
		  type = TcondStartPage;
		  break;
	       case C_TR_USERPAGE:
		  type = TcondUserPage;
		  break;
	       case C_TR_REMINDERPAGE:
		  type = TcondReminderPage;
		  break;
	       case C_TR_EMPTY:
		  type = TcondEmpty;
		  break;
	       case C_TR_EXTERNALREF:
		  type = TcondExternalRef;
		  break;
	       default:
		  erreurTraduc (2);	/* erreur de codage */
		  type = TcondFirst;
		  break;
	    }

   return type;
}


/* ---------------------------------------------------------------------- */
/* |    RdTypeRegleTrad lit un type de regle de traduction.             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static TRuleType RdTypeRegleTrad (BinFile fich)

#else  /* __STDC__ */
static TRuleType RdTypeRegleTrad (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   TRuleType       type;

   type = TRemove;
   if (!BIOreadByte (fich, &c))
      erreurTraduc (3);
   else
      switch (c)
	    {
	       case C_TR_CREATE:
		  type = TCreate;
		  break;
	       case C_TR_GET:
		  type = TGet;
		  break;
	       case C_TR_USE:
		  type = TUse;
		  break;
	       case C_TR_REMOVE:
		  type = TRemove;
		  break;
	       case C_TR_WRITE:
		  type = TWrite;
		  break;
	       case C_TR_READ:
		  type = TRead;
		  break;
	       case C_TR_INCLUDE:
		  type = TInclude;
		  break;
	       case C_TR_NOTRANSL:
		  type = TNoTranslation;
		  break;
	       case C_TR_NOLINEBREAK:
		  type = TNoLineBreak;
		  break;
	       case C_TR_COPY:
		  type = TCopy;
		  break;
	       case C_TR_CHANGEFILE:
		  type = TChangeMainFile;
		  break;
	       case C_TR_SET_COUNTER:
		  type = TSetCounter;
		  break;
	       case C_TR_ADD_COUNTER:
		  type = TAddCounter;
		  break;
	       default:
		  erreurTraduc (4);	/* erreur de codage */
		  type = TRemove;
		  break;
	    }

   return type;
}


/* ---------------------------------------------------------------------- */
/* |    RdRelatNivAsc                                                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static RelatNAscend  RdRelatNivAsc (BinFile fich)

#else  /* __STDC__ */
static RelatNAscend  RdRelatNivAsc (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   RelatNAscend         ret;

   ret = RelEquals;		/* valeur par defaut ? */
   if (!BIOreadByte (fich, &c))
      erreurTraduc (3);
   else
      switch (c)
	    {
	       case C_WITHIN_GT:
		  ret = RelGreater;
		  break;
	       case C_WITHIN_LT:
		  ret = RelLess;
		  break;
	       case C_WITHIN_EQ:
		  ret = RelEquals;
		  break;
	    }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    RdTrPosition lit la position ou` il faut creer les chaines      | */
/* |            produites par le traducteur.                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static TOrder   RdTrPosition (BinFile fich)

#else  /* __STDC__ */
static TOrder   RdTrPosition (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   TOrder          position;

   position = TAfter;
   if (!BIOreadByte (fich, &c))
      erreurTraduc (5);
   else
      switch (c)
	    {
	       case C_TR_AFTER:
		  position = TAfter;
		  break;
	       case C_TR_BEFORE:
		  position = TBefore;
		  break;
	       default:
		  erreurTraduc (6);	/* erreur de codage */
		  position = TAfter;
		  break;
	    }

   return position;
}


/* ---------------------------------------------------------------------- */
/* |    RdTypeCree lit le type de chose a` creer dans le fichier de     | */
/* |            sortie ou au terminal.                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static CreatedObject     RdTypeCree (BinFile fich)

#else  /* __STDC__ */
static CreatedObject     RdTypeCree (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   CreatedObject            type;

   type = ToConst;
   if (!BIOreadByte (fich, &c))
      erreurTraduc (7);
   else
      switch (c)
	    {
	       case C_OB_CONST:
		  type = ToConst;
		  break;
	       case C_OB_BUFFER:
		  type = ToBuffer;
		  break;
	       case C_OB_VAR:
		  type = ToVariable;
		  break;
	       case C_OB_ATTR:
		  type = ToAttr;
		  break;
	       case C_OB_CONTENT:
		  type = ToContent;
		  break;
	       case C_OB_COMMENT:
		  type = ToComment;
		  break;
	       case C_OB_ATTRIBUTES:
		  type = ToAllAttr;
		  break;
	       case C_OB_PRES_VAL:
		  type = ToPRuleValue;
		  break;
	       case C_OB_PRESENTATION:
		  type = ToAllPRules;
		  break;
	       case C_OB_REFID:
		  type = ToRefId;
		  break;
	       case C_OB_PAIRID:
		  type = ToPairId;
		  break;
	       case C_OB_REFERRED_ELEM:
		  type = ToReferredElem;
		  break;
	       case C_OB_FILEDIR:
		  type = ToFileDir;
		  break;
	       case C_OB_FILENAME:
		  type = ToFileName;
		  break;
	       case C_OB_EXTENSION:
		  type = ToExtension;
		  break;
	       case C_OB_DOCUMENTNAME:
		  type = ToDocumentName;
		  break;
	       case C_OB_DOCUMENTDIR:
		  type = ToDocumentDir;
		  break;
	       case C_OB_REFERRED_DOCNAME:
		  type = ToReferredDocumentName;
		  break;
	       case C_OB_REFERRED_DOCDIR:
		  type = ToReferredDocumentDir;
		  break;
	       case C_OB_REFERRED_REFID:
		  type = ToReferredRefId;
		  break;
	       default:
		  erreurTraduc (8);	/* erreur de codage */
		  type = ToConst;
		  break;
	    }

   return type;
}


/* ---------------------------------------------------------------------- */
/* |    RdRelatPos lit la position relative d'un element a` prendre.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static TRelatPosition     RdRelatPos (BinFile fich)

#else  /* __STDC__ */
static TRelatPosition     RdRelatPos (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   TRelatPosition            position;

   position = RpSibling;
   if (!BIOreadByte (fich, &c))
      erreurTraduc (9);
   else
      switch (c)
	    {
	       case C_TR_SIBLING:
		  position = RpSibling;
		  break;
	       case C_TR_DESCEND:
		  position = RpDescend;
		  break;
	       case C_TR_REFERRED_EL:
		  position = RpReferred;
		  break;
	       case C_TR_ASSOC:
		  position = RpAssoc;
		  break;
	       default:
		  erreurTraduc (10);	/* erreur de codage */
		  position = RpSibling;
		  break;
	    }

   return position;
}


/* ---------------------------------------------------------------------- */
/* |    RdTrCptTypeOp lit le type d'une operation sur un compteur.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static TCounterOp  RdTrCptTypeOp (BinFile fich)

#else  /* __STDC__ */
static TCounterOp  RdTrCptTypeOp (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   TCounterOp         type;

   type = TCntrNoOp;
   if (!BIOreadByte (fich, &c))
      erreurTraduc (11);
   else
      switch (c)
	    {
	       case C_TR_RANK:
		  type = TCntrRank;
		  break;
	       case C_TR_RLEVEL:
		  type = TCntrRLevel;
		  break;
	       case C_TR_SET:
		  type = TCntrSet;
		  break;
	       case C_TR_NOOP:
		  type = TCntrNoOp;
		  break;
	       default:
		  erreurTraduc (12);	/* erreur de codage */
		  type = TCntrNoOp;
		  break;
	    }

   return type;
}


/* ---------------------------------------------------------------------- */
/* |    RdVarTradType lit le type des elements de variables de          | */
/* |            traduction.                                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static TranslVarType  RdVarTradType (BinFile fich)

#else  /* __STDC__ */
static TranslVarType  RdVarTradType (fich)
BinFile             fich;

#endif /* __STDC__ */

{
   char                c;
   TranslVarType         type;

   type = VtText;
   if (!BIOreadByte (fich, &c))
      erreurTraduc (13);
   else
      switch (c)
	    {
	       case C_TR_CONST:
		  type = VtText;
		  break;
	       case C_TR_COUNTER:
		  type = VtCounter;
		  break;
	       case C_TR_BUFFER:
		  type = VtBuffer;
		  break;
	       case C_TR_ATTR:
		  type = VtAttrVal;
		  break;
	       case C_TR_FILEDIR:
		  type = VtFileDir;
		  break;
	       case C_TR_FILENAME:
		  type = VtFileName;
		  break;
	       case C_TR_EXTENSION:
		  type = VtExtension;
		  break;
	       case C_TR_DOCUMENTNAME:
		  type = VtDocumentName;
		  break;
	       case C_TR_DOCUMENTDIR:
		  type = VtDocumentDir;
		  break;
	       default:
		  erreurTraduc (14);	/* erreur de codage */
		  type = VtText;
		  break;
	    }

   return type;
}


/* ---------------------------------------------------------------------- */
/* |    RdComptStyle    lit un style de compteur dans le fichier et     | */
/* |            retourne sa valeur.                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static CounterStyle   RdComptStyle (BinFile file)

#else  /* __STDC__ */
static CounterStyle   RdComptStyle (file)
BinFile             file;

#endif /* __STDC__ */
{
   char                c;
   CounterStyle          ret;


   if (!BIOreadByte (file, &c))
     {
	c = ' ';
	erreurTraduc (15);
     }
   switch (c)
	 {
	    case C_NUM_ARABIC:
	       ret = CntArabic;
	       break;
	    case C_NUM_ROMAN:
	       ret = CntURoman;
	       break;
	    case C_NUM_LOWER_ROMAN:
	       ret = CntLRoman;
	       break;
	    case C_NUM_UPPERCASE:
	       ret = CntUppercase;
	       break;
	    case C_NUM_LOWERCASE:
	       ret = CntLowercase;
	       break;
	    default:
	       erreurTraduc (16);	/* erreur de codage */
	       ret = CntArabic;
	       break;
	 }

   return ret;
}

/* ---------------------------------------------------------------------- */
/* |    Rdptrregle retourne un pointeur sur la regle suivante ou NULL   | */
/* |            s'il n'y a pas de regle suivante.                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrTRule RdPtrRegle (BinFile fich, PtrTRule * nextr)

#else  /* __STDC__ */
static PtrTRule RdPtrRegle (fich, nextr)
BinFile             fich;
PtrTRule       *nextr;

#endif /* __STDC__ */

{
   char                c;
   PtrTRule        regle;

   if (!BIOreadByte (fich, &c))
      erreurTraduc (17);
   if (c == '\0')
      regle = NULL;
   else
      regle = *nextr;
   return regle;
}


/* ---------------------------------------------------------------------- */
/* |    ReadRules ecrit une suite de regles chainees et fait pointer le | */
/* |            pR sur la premiere regle de la suite de regles lues.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ReadRules (BinFile fich, PtrTRule * pR, PtrTRule * nextr)

#else  /* __STDC__ */
static void         ReadRules (fich, pR, nextr)
BinFile             fich;
PtrTRule       *pR;
PtrTRule       *nextr;

#endif /* __STDC__ */

{
   PtrTRule        r;	/* pointeur sur la premiere regle qui va etre lue */
   PtrTRule        pRe1;

   *pR = *nextr;		/* lecture de la suite de regles */
   if (!erreurTra)
      do
	{
	   r = *nextr;		/* acquiert un buffer pour la regle suivante */
	   if ((*nextr = (PtrTRule) TtaGetMemory (sizeof (TranslRule))) == NULL)
	      erreurTraduc (18);
	   /* lit une regle */
	   pRe1 = r;
	   pRe1->TrNextTRule = RdPtrRegle (fich, nextr);
	   pRe1->TrOrder = RdTrPosition (fich);
	   pRe1->TrType = RdTypeRegleTrad (fich);
	   if (!erreurTra)
	      switch (pRe1->TrType)
		    {
		       case TCreate:
		       case TWrite:
			  pRe1->TrObject = RdTypeCree (fich);
			  BIOreadShort (fich, &pRe1->TrObjectNum);
			  BIOreadName (fich, (char *) &pRe1->TrObjectNature);
			  BIOreadBool (fich, &pRe1->TrReferredObj);
			  BIOreadShort (fich, &pRe1->TrFileNameVar);
			  break;
		       case TGet:
		       case TCopy:
			  BIOreadShort (fich, &pRe1->TrElemType);
			  BIOreadName (fich, (char *) &pRe1->TrElemNature);
			  pRe1->TrRelPosition = RdRelatPos (fich);
			  break;
		       case TUse:
			  BIOreadName (fich, (char *) &pRe1->TrNature);
			  BIOreadName (fich, (char *) &pRe1->TrTranslSchemaName);
			  break;
		       case TRemove:
		       case TNoTranslation:
		       case TNoLineBreak:

			  break;
		       case TRead:
			  BIOreadShort (fich, &pRe1->TrBuffer);
			  break;
		       case TInclude:
			  pRe1->TrBufOrConst = RdTypeCree (fich);
			  BIOreadShort (fich, &pRe1->TrInclFile);
			  break;
		       case TChangeMainFile:
			  BIOreadShort (fich, &pRe1->TrNewFileVar);
			  break;
		       case TSetCounter:
		       case TAddCounter:
			  BIOreadShort (fich, &pRe1->TrCounterNum);
			  BIOreadShort (fich, &pRe1->TrCounterParam);
			  break;
		       default:
			  break;
		    }
	   /* passe a la regle suivante */
	   if (pRe1->TrNextTRule != NULL)
	      pRe1->TrNextTRule = *nextr;
	}
      while (!(r->TrNextTRule == NULL || erreurTra));
}


/* ---------------------------------------------------------------------- */
/* |    LibereRegles    libere une suite de regles chainees.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         LibereRegles (PtrTRule pR)

#else  /* __STDC__ */
static void         LibereRegles (pR)
PtrTRule        pR;

#endif /* __STDC__ */

{
   PtrTRule        rule, nextRule;

   rule = pR;
   while (rule != NULL)
     {
	nextRule = rule->TrNextTRule;
	TtaFreeMemory ((char *) rule);
	rule = nextRule;
     }
}


/* ---------------------------------------------------------------------- */
/* |    RdPtrBloc retourne un pointeur sur le bloc de regles suivant ou | */
/* |            NULL s'il n'y a pas de bloc suivant.                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrTRuleBlock RdPtrBloc (BinFile fich, PtrTRuleBlock * nextb)

#else  /* __STDC__ */
static PtrTRuleBlock RdPtrBloc (fich, nextb)
BinFile             fich;
PtrTRuleBlock      *nextb;

#endif /* __STDC__ */

{
   char                c;
   PtrTRuleBlock       bloc;

   BIOreadByte (fich, &c);
   if (c == '\0')
      bloc = NULL;
   else
      bloc = *nextb;
   return bloc;
}


/* ---------------------------------------------------------------------- */
/* |    ReadBlocs lit une suite de blocs de regles.                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ReadBlocs (BinFile fich, PtrTRuleBlock * pB, PtrTRule * nextr, PtrSSchema * SS, PtrTRuleBlock * nextb)

#else  /* __STDC__ */
static void         ReadBlocs (fich, pB, nextr, SS, nextb)
BinFile             fich;
PtrTRuleBlock      *pB;
PtrTRule       *nextr;
PtrSSchema       *SS;
PtrTRuleBlock      *nextb;

#endif /* __STDC__ */

{
   PtrTRuleBlock       b;
   PtrTRuleBlock       pBl1;
   int                 ncond;
   TranslCondition        *Cond;

   if (*pB != NULL && !erreurTra)
      /* pointeur sur le premier bloc qui va etre lu */
     {
	*pB = *nextb;		/* lecture de la suite des blocs */
	do
	  {
	     b = *nextb;
	     /* acquiert un buffer pour le bloc suivant */
	     if ((*nextb = (PtrTRuleBlock) TtaGetMemory (sizeof (TRuleBlock))) == NULL)
		erreurTraduc (19);
	     /* lit un bloc */
	     pBl1 = b;
	     pBl1->TbNextBlock = RdPtrBloc (fich, nextb);
	     BIOreadShort (fich, &pBl1->TbNConditions);
	     for (ncond = 1; ncond <= pBl1->TbNConditions; ncond++)
	       {
		  Cond = &pBl1->TbCondition[ncond - 1];
		  Cond->TcCondition = RdTypeCondTrad (fich);
		  BIOreadBool (fich, &Cond->TcNegativeCond);
		  BIOreadBool (fich, &Cond->TcTarget);
		  BIOreadShort (fich, &Cond->TcAscendType);
		  BIOreadName (fich, (char *) &Cond->TcAscendNature);
		  BIOreadSignedShort (fich, &Cond->TcAscendRelLevel);
		  switch (Cond->TcCondition)
			{
			   case TcondAlphabet:
			      BIOreadByte (fich, &Cond->TcAlphabet);
			      break;
			   case TcondWithin:
			   case TcondFirstWithin:
			      BIOreadShort (fich, &Cond->TcElemType);
			      BIOreadName (fich, (char *) &Cond->TcElemNature);
			      BIOreadBool (fich, &Cond->TcImmediatelyWithin);
			      Cond->TcAscendRel = RdRelatNivAsc (fich);
			      BIOreadShort (fich, &Cond->TcAscendLevel);
			      break;
			   case TcondAttr:
			      BIOreadShort (fich, &Cond->TcAttr);
			      if (!erreurTra)
				 switch ((*SS)->SsAttribute[Cond->TcAttr - 1].AttrType)
				       {
					  case AtNumAttr:
					     BIOreadSignedShort (fich, &Cond->TcLowerBound);
					     BIOreadSignedShort (fich, &Cond->TcUpperBound);
					     break;
					  case AtTextAttr:
					     BIOreadName (fich, (char *) &Cond->TcTextValue);
					     break;
					  case AtReferenceAttr:

					     break;
					  case AtEnumAttr:
					     BIOreadShort (fich, &Cond->TcAttrValue);
					     break;
					  default:
					     break;
				       }

			      break;
			   case TcondPRule:
			      BIOreadShort (fich, &Cond->TcAttr);
			      if (!erreurTra)
				 if (Cond->TcAttr == PtSize ||
				     Cond->TcAttr == PtIndent ||
				     Cond->TcAttr == PtLineSpacing ||
				     Cond->TcAttr == PtLineWeight ||
				     Cond->TcAttr == PtFillPattern ||
				     Cond->TcAttr == PtBackground ||
				     Cond->TcAttr == PtForeground)
				   {
				      BIOreadSignedShort (fich, &Cond->TcLowerBound);
				      BIOreadSignedShort (fich, &Cond->TcUpperBound);
				   }
				 else
				    BIOreadByte (fich, &Cond->TcPresValue);
			      break;
			   default:
			      break;
			}
	       }
	     ReadRules (fich, &pBl1->TbFirstTRule, nextr);
	     /* passe a la regle suivante */
	     if (pBl1->TbNextBlock != NULL)
		pBl1->TbNextBlock = *nextb;
	  }
	while (!(b->TbNextBlock == NULL || erreurTra));
     }
}

/* ---------------------------------------------------------------------- */
/* |    LibereBlocs     libere une suite de blocs de regles ainsi que   | */
/* |            les regles attachees.                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         LibereBlocs (PtrTRuleBlock pB)

#else  /* __STDC__ */
static void         LibereBlocs (pB)
PtrTRuleBlock       pB;

#endif /* __STDC__ */

{
   PtrTRuleBlock       Bloc, nextBloc;

   Bloc = pB;
   while (Bloc != NULL)
     {
	nextBloc = Bloc->TbNextBlock;
	LibereRegles (Bloc->TbFirstTRule);
	TtaFreeMemory ((char *) Bloc);
	Bloc = nextBloc;
     }
}


/* ---------------------------------------------------------------------- */
/* |    ReadReglesAttr lit les regles de traduction de l'attribut de    | */
/* |            numero att appartenant au schema de structure SS.       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ReadReglesAttr (BinFile fich, int att, PtrTRuleBlock * nextb, PtrTRule * nextr, PtrSSchema * SS, PtrTSchema * pSchT)

#else  /* __STDC__ */
static void         ReadReglesAttr (fich, att, nextb, nextr, SS, pSchT)
BinFile             fich;
int                 att;
PtrTRuleBlock      *nextb;
PtrTRule       *nextr;
PtrSSchema       *SS;
PtrTSchema       *pSchT;

#endif /* __STDC__ */

{
   int                 i;
   AttributeTransl      *pRT1;
   TranslNumAttrCase        *pTC1;

   if (!erreurTra)
     {
	pRT1 = &(*pSchT)->TsAttrTRule[att - 1];
	BIOreadShort (fich, &pRT1->AtrElemType);
	switch ((*SS)->SsAttribute[att - 1].AttrType)
	      {
		 case AtNumAttr:
		    BIOreadShort (fich, &pRT1->AtrNCases);
		    for (i = 1; i <= pRT1->AtrNCases; i++)
		       if (!erreurTra)
			 {
			    pTC1 = &pRT1->AtrCase[i - 1];
			    BIOreadSignedShort (fich, &pTC1->TaLowerBound);
			    BIOreadSignedShort (fich, &pTC1->TaUpperBound);
			    ReadBlocs (fich, &pTC1->TaTRuleBlock, nextr, SS, nextb);
			 }
		    break;
		 case AtTextAttr:
		    BIOreadName (fich, (char *) &pRT1->AtrTextValue);
		    ReadBlocs (fich, &pRT1->AtrTxtTRuleBlock, nextr, SS, nextb);
		    break;
		 case AtReferenceAttr:
		    ReadBlocs (fich, &pRT1->AtrRefTRuleBlock, nextr, SS, nextb);
		    break;
		 case AtEnumAttr:
		    for (i = 0; i <= (*SS)->SsAttribute[att - 1].AttrNEnumValues; i++)
		       ReadBlocs (fich, &pRT1->AtrEnuTRuleBlock[i], nextr, SS, nextb);
		    break;
		 default:
		    break;
	      }
     }
}

/* ---------------------------------------------------------------------- */
/* |    LibereReglesAttr libere les regles de traduction d'unattribut.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         LibereReglesAttr (AttributeTransl * pRT1, TtAttribute * pAtt)

#else  /* __STDC__ */
static void         LibereReglesAttr (pRT1, pAtt)
AttributeTransl      *pRT1;
TtAttribute           *pAtt;

#endif /* __STDC__ */

{
   int                 i;

   switch (pAtt->AttrType)
	 {
	    case AtNumAttr:
	       for (i = 0; i < pRT1->AtrNCases; i++)
		  LibereBlocs (pRT1->AtrCase[i].TaTRuleBlock);
	       break;
	    case AtTextAttr:
	       LibereBlocs (pRT1->AtrTxtTRuleBlock);
	       break;
	    case AtReferenceAttr:
	       LibereBlocs (pRT1->AtrRefTRuleBlock);
	       break;
	    case AtEnumAttr:
	       for (i = 0; i <= pAtt->AttrNEnumValues; i++)
		  LibereBlocs (pRT1->AtrEnuTRuleBlock[i]);
	       break;
	    default:
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* |    ReadReglesPres lit les regles de traduction de la presentation  | */
/* |            de numero pres appartenant au schema de structure SS.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ReadReglesPres (BinFile fich, int pres, PtrTRuleBlock * nextb, PtrTRule * nextr, PtrSSchema * SS, PtrTSchema * pSchT)

#else  /* __STDC__ */
static void         ReadReglesPres (fich, pres, nextb, nextr, SS, pSchT)
BinFile             fich;
int                 pres;
PtrTRuleBlock      *nextb;
PtrTRule       *nextr;
PtrSSchema       *SS;
PtrTSchema       *pSchT;

#endif /* __STDC__ */

{
   int                 i;
   PRuleTransl       *pRT1;
   TranslNumAttrCase        *pTC1;

   if (!erreurTra)
     {
	pRT1 = &(*pSchT)->TsPresTRule[pres - 1];
	if (pRT1->RtExist)
	   if (pres == PtSize + 1 || pres == PtIndent + 1 ||
	       pres == PtLineSpacing + 1 || pres == PtLineWeight + 1 ||
	       pres == PtFillPattern + 1 || pres == PtBackground + 1 ||
	       pres == PtForeground + 1)
	      /* presentation a valeur numerique */
	     {
		BIOreadShort (fich, &pRT1->RtNCase);
		for (i = 0; i < pRT1->RtNCase; i++)
		   if (!erreurTra)
		     {
			pTC1 = &pRT1->RtCase[i];
			BIOreadSignedShort (fich, &pTC1->TaLowerBound);
			BIOreadSignedShort (fich, &pTC1->TaUpperBound);
			ReadBlocs (fich, &pTC1->TaTRuleBlock, nextr, SS, nextb);
		     }
	     }
	   else
	      for (i = 0; i <= MAX_TRANSL_PRES_VAL; i++)
		{
		   BIOreadByte (fich, &pRT1->RtPRuleValue[i]);
		   ReadBlocs (fich, &pRT1->RtPRuleValueBlock[i], nextr, SS, nextb);
		}

     }
}


/* ---------------------------------------------------------------------- */
/* |    LibereReglesPres libere les regles de traduction d'une regle de | */
/* |            presentation                                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         LibereReglesPres (int pres, PRuleTransl * pRT1)

#else  /* __STDC__ */
static void         LibereReglesPres (pres, pRT1)
int                 pres;
PRuleTransl       *pRT1;

#endif /* __STDC__ */

{
   int                 i;

   if (pRT1->RtExist)
      if (pres == PtSize + 1 || pres == PtIndent + 1 ||
	  pres == PtLineSpacing + 1 || pres == PtLineWeight + 1 ||
	  pres == PtFillPattern + 1 || pres == PtBackground + 1 ||
	  pres == PtForeground + 1)
	 /* presentation a valeur numerique */
	 for (i = 0; i < pRT1->RtNCase; i++)
	    LibereBlocs (pRT1->RtCase[i].TaTRuleBlock);
      else
	 for (i = 0; i <= MAX_TRANSL_PRES_VAL; i++)
	    LibereBlocs (pRT1->RtPRuleValueBlock[i]);
}


/* ---------------------------------------------------------------------- */
/* |    RdSchTrad lit un fichier contenant un schema de traduction et   | */
/* |            le charge en memoire. Retourne un pointeur sur le       | */
/* |            schema de presentation en memoire si chargement reussi, | */
/* |            NULL si echec.                                          | */
/* |            - fname: nom du fichier a lire, sans le suffixe .TRA    | */
/* |            - SS: schema de structure correspondant, deja rempli.   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrTSchema        RdSchTrad (Name fname, PtrSSchema SS)

#else  /* __STDC__ */
PtrTSchema        RdSchTrad (fname, SS)
Name                 fname;
PtrSSchema        SS;

#endif /* __STDC__ */

{
   int                 i, j;
   PtrTRule        nextr;
   PtrTRuleBlock       nextb;
   PtrTSchema        pSchT;
   PtrTSchema        pSc1;
   TCounter         *pTr1;
   TranslVariable            *pVa1;
   TranslVarItem        *pVarT1;
   AttributeTransl      *pRT1;
   AlphabetTransl            *pTrad1;
   StringTransl            *pTradCa6;
   PtrTSchema        schematrad;
   PRuleTransl       *pRTP1;
   PathBuffer          DirBuffer;
   BinFile             fich;
   char                texte[MAX_TXT_LEN];
   int                 NbElemStructInitial;

   erreurTra = False;
   pSchT = NULL;
   /* compose le nom du fichier a ouvrir avec le nom du directory */
   /* des schemas... */
   strncpy (DirBuffer, (char *) DirectorySchemas, MAX_PATH);
   BuildFileName (fname, "TRA", DirBuffer, texte, &i);

   /* ouvre le fichier */
   fich = BIOreadOpen (texte);
   if (fich == 0)
     {
	strncpy (texte, fname, MAX_NAME_LEN);
	strcat (texte, ".TRA");
	TtaDisplaySimpleMessageString (LIB, INFO, LIB_MISSING_FILE, texte);
     }
   else
     {
	if ((pSchT = (PtrTSchema) TtaGetMemory (sizeof (TranslSchema))) == NULL)
	   erreurTraduc (20);
	if ((nextr = (PtrTRule) TtaGetMemory (sizeof (TranslRule))) == NULL)
	   erreurTraduc (19);
	if ((nextb = (PtrTRuleBlock) TtaGetMemory (sizeof (TRuleBlock))) == NULL)
	   erreurTraduc (21);

	/* lit la partie fixe du schema de traduction */
	pSc1 = pSchT;
	BIOreadName (fich, (char *) &pSc1->TsStructName);
	BIOreadShort (fich, &pSc1->TsStructCode);
	BIOreadShort (fich, &pSc1->TsLineLength);
	BIOreadName (fich, (char *) &pSc1->TsEOL);
	BIOreadName (fich, (char *) &pSc1->TsTranslEOL);
	BIOreadShort (fich, &pSc1->TsNConstants);
	BIOreadShort (fich, &pSc1->TsNCounters);
	BIOreadShort (fich, &pSc1->TsNVariables);
	BIOreadShort (fich, &pSc1->TsNBuffers);
	if (!erreurTra)
	   for (i = 1; i <= pSc1->TsNConstants; i++)
	      BIOreadShort (fich, &pSc1->TsConstBegin[i - 1]);
	if (!erreurTra)
	   for (i = 1; i <= pSc1->TsNCounters; i++)
	     {
		pTr1 = &pSc1->TsCounter[i - 1];
		pTr1->TnOperation = RdTrCptTypeOp (fich);
		if (pTr1->TnOperation == TCntrNoOp)
		   pTr1->TnParam1 = 0;
		else
		   BIOreadShort (fich, &pTr1->TnElemType1);
		if (pTr1->TnOperation == TCntrSet)
		  {
		     BIOreadShort (fich, &pTr1->TnElemType2);
		     BIOreadShort (fich, &pTr1->TnParam1);
		     BIOreadShort (fich, &pTr1->TnParam2);
		  }
		else if (pTr1->TnOperation == TCntrRank)
		   BIOreadSignedShort (fich, &pTr1->TnAcestorLevel);
		BIOreadShort (fich, &pTr1->TnAttrInit);
	     }
	if (!erreurTra)
	   for (i = 1; i <= pSc1->TsNVariables; i++)
	     {
		pVa1 = &pSc1->TsVariable[i - 1];
		BIOreadShort (fich, &pVa1->TrvNItems);
		if (!erreurTra)
		   for (j = 1; j <= pVa1->TrvNItems; j++)
		     {
			pVarT1 = &pVa1->TrvItem[j - 1];
			pVarT1->TvType = RdVarTradType (fich);
			BIOreadShort (fich, &pVarT1->TvItem);
			if (pVarT1->TvType == VtCounter)
			  {
			     BIOreadShort (fich, &pVarT1->TvLength);
			     pVarT1->TvCounterStyle = RdComptStyle (fich);
			  }
		     }
	     }
	BIOreadShort (fich, &pSc1->TsPictureBuffer);
	for (i = 1; i <= MAX_TRANSL_BUFFER; i++)
	   pSc1->TsBuffer[i - 1][0] = '\0';
	if (SS->SsFirstDynNature == 0)
	   NbElemStructInitial = SS->SsNRules;
	else
	  {
	     NbElemStructInitial = SS->SsFirstDynNature - 1;
	     for (j = NbElemStructInitial + 1; j <= SS->SsNRules; j++)
		pSc1->TsElemTRule[j - 1] = NULL;
	  }
	if (!erreurTra)
	   for (i = 1; i <= NbElemStructInitial; i++)
	      pSc1->TsElemTRule[i - 1] = RdPtrBloc (fich, &nextb);
	if (!erreurTra)
	   for (i = 1; i <= NbElemStructInitial; i++)
	      BIOreadBool (fich, &pSc1->TsInheritAttr[i - 1]);
	if (!erreurTra)
	   for (i = 1; i <= SS->SsNAttributes; i++)
	      if (!erreurTra)
		{
		   pRT1 = &pSc1->TsAttrTRule[i - 1];
		   switch (SS->SsAttribute[i - 1].AttrType)
			 {
			    case AtNumAttr:
			       BIOreadShort (fich, &pRT1->AtrNCases);
			       if (!erreurTra)
				  for (j = 1; j <= pRT1->AtrNCases; j++)
				     pRT1->AtrCase[j - 1].TaTRuleBlock =
					RdPtrBloc (fich, &nextb);
			       break;
			    case AtTextAttr:
			       pRT1->AtrTxtTRuleBlock = RdPtrBloc (fich, &nextb);
			       break;
			    case AtReferenceAttr:
			       pRT1->AtrRefTRuleBlock = RdPtrBloc (fich, &nextb);
			       break;
			    case AtEnumAttr:
			       for (j = 0; j <= SS->SsAttribute[i - 1].AttrNEnumValues; j++)
				  pRT1->AtrEnuTRuleBlock[j] = RdPtrBloc (fich, &nextb);
			       break;
			    default:
			       break;
			 }
		}
	if (!erreurTra)
	   for (i = 1; i <= MAX_TRANSL_PRULE; i++)
	      if (!erreurTra)
		{
		   pRTP1 = &pSc1->TsPresTRule[i - 1];
		   BIOreadBool (fich, &pRTP1->RtExist);
		   if (pRTP1->RtExist)
		      if (i == PtSize + 1 || i == PtIndent + 1 ||
			  i == PtLineSpacing + 1 ||
			  i == PtLineWeight + 1 || i == PtFillPattern + 1 ||
			  i == PtBackground + 1 || i == PtForeground + 1)
			 /* presentation a valeur numerique */
			{
			   BIOreadShort (fich, &pRTP1->RtNCase);
			   if (!erreurTra)
			      for (j = 1; j <= pRTP1->RtNCase; j++)
				 pRTP1->RtCase[j - 1].TaTRuleBlock =
				    RdPtrBloc (fich, &nextb);
			}
		      else
			 for (j = 0; j <= MAX_TRANSL_PRES_VAL; j++)
			    pRTP1->RtPRuleValueBlock[j] = RdPtrBloc (fich, &nextb);
		}
	BIOreadShort (fich, &pSc1->TsNTranslAlphabets);
	if (!erreurTra)
	   for (i = 1; i <= pSc1->TsNTranslAlphabets; i++)
	     {
		pTrad1 = &pSc1->TsTranslAlphabet[i - 1];
		BIOreadByte (fich, &pTrad1->AlAlphabet);
		BIOreadShort (fich, &pTrad1->AlBegin);
		BIOreadShort (fich, &pTrad1->AlEnd);
	     }
	BIOreadShort (fich, &pSc1->TsSymbolFirst);
	BIOreadShort (fich, &pSc1->TsSymbolLast);
	BIOreadShort (fich, &pSc1->TsGraphicsFirst);
	BIOreadShort (fich, &pSc1->TsGraphicsLast);
	BIOreadShort (fich, &pSc1->TsNCharTransls);
	/* lit les tables de traduction de caracteres */
	if (!erreurTra)
	   for (i = 1; i <= pSc1->TsNCharTransls; i++)
	     {
		pTradCa6 = &pSc1->TsCharTransl[i - 1];
		/* lit la chaine source */
		j = 0;
		do
		  {
		     BIOreadByte (fich, &pTradCa6->StSource[j]);
		     j++;
		  }
		while (!(pTradCa6->StSource[j - 1] == '\0'));
		/* lit la chaine cible */
		j = 0;
		do
		  {
		     BIOreadByte (fich, &pTradCa6->StTarget[j]);
		     j++;
		  }
		while (!(pTradCa6->StTarget[j - 1] == '\0'));
	     }
	if (!erreurTra)
	   /* lit les constantes */
	   for (i = 1; i <= pSc1->TsNConstants; i++)
	     {
		j = pSc1->TsConstBegin[i - 1] - 1;
		do
		  {
		     j++;
		     BIOreadByte (fich, &pSc1->TsConstant[j - 1]);
		  }
		while (!(pSc1->TsConstant[j - 1] == '\0'));
		/* lit les blocs de regles des elements structures */
	     }
	if (!erreurTra)
	   for (i = 1; i <= NbElemStructInitial; i++)
	      ReadBlocs (fich, &pSc1->TsElemTRule[i - 1], &nextr, &SS, &nextb);
	/* lit les blocs de regles des attributs */
	for (i = 1; i <= SS->SsNAttributes; i++)
	   if (!erreurTra)
	      ReadReglesAttr (fich, i, &nextb, &nextr, &SS, &pSchT);
	/* lit les blocs de regles des presentation */
	for (i = 1; i <= MAX_TRANSL_PRULE; i++)
	   if (!erreurTra)
	      ReadReglesPres (fich, i, &nextb, &nextr, &SS, &pSchT);
	TtaFreeMemory ((char *) nextr);		/* on avait anticipe... */
	TtaFreeMemory ((char *) nextb);		/* on avait anticipe... */

	/* ferme le fichier */
	BIOreadClose (fich);
     }
   if (erreurTra)
     {
	TtaDisplaySimpleMessageString (LIB, INFO, LIB_INCOR_TRA_FILE, fname);
	schematrad = NULL;
     }
   else
      schematrad = pSchT;	/* rend la valeur de retour */
   return schematrad;
}

/* ---------------------------------------------------------------------- */
/* |    LibereSchTrad   libere le schema de traduction pSchT            | */
/* |            correspondant au schema de structure pSS.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                LibereSchTrad (PtrTSchema pSchT, PtrSSchema pSS)

#else  /* __STDC__ */
void                LibereSchTrad (pSchT, pSS)
PtrTSchema        pSchT;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int                 NbElemStructInitial;
   int                 i;

   if (pSS->SsFirstDynNature == 0)
      NbElemStructInitial = pSS->SsNRules;
   else
      NbElemStructInitial = pSS->SsFirstDynNature - 1;
   /* libere les blocs de regles  des elements */
   for (i = 0; i < NbElemStructInitial; i++)
      LibereBlocs (pSchT->TsElemTRule[i]);
   /* libere les blocs de regles des attributs */
   for (i = 0; i < pSS->SsNAttributes; i++)
      LibereReglesAttr (&pSchT->TsAttrTRule[i], &pSS->SsAttribute[i]);
   /* libere les blocs de regles des presentations */
   for (i = 0; i < MAX_TRANSL_PRULE; i++)
      LibereReglesPres (i + 1, &pSchT->TsPresTRule[i]);
   /* libere le schema de traduction lui-meme */
   TtaFreeMemory ((char *) pSchT);
}

/* End Of Module rdschtra */
