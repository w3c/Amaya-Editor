
/* ======================================================================= */
/* |                                                                    | */
/* |                                                                    | */
/* |    Ce module sauve un schema de traduction compile'.               | */
/* |                                                                    | */
/* |                                                                    | */
/* ======================================================================= */

#include "thot_sys.h"
#include "constmedia.h"
#include "consttra.h"
#include "typemedia.h"
#include "typetra.h"
#include "storage.h"

#define EXPORT extern
static BinFile      outfile;

#include "fileaccess_f.h"
#include "writetra_f.h"

/* WriteShort      ecrit dans le fichier de sortie un entier sur 2 octets */
#ifdef __STDC__
void                WriteShort (int n)
#else  /* __STDC__ */
void                WriteShort (n)
int                 n;

#endif /* __STDC__ */
{
   BIOwriteByte (outfile, (char) (n / 256));
   BIOwriteByte (outfile, (char) (n % 256));
}

/* WriteSignedShort  ecrit dans le fichier de sortie un entier signe' sur */
/* 2 octets */
#ifdef __STDC__
void                WriteSignedShort (int n)

#else  /* __STDC__ */
void                WriteSignedShort (n)
int             n;

#endif /* __STDC__ */

{
   if (n >= 0)
      WriteShort (n);
   else
      WriteShort (n + 65536);
}

/* wrnom                ecrit un nom dans le fichier de sortie */

#ifdef __STDC__
void                wrnom (Name n)

#else  /* __STDC__ */
void                wrnom (n)
Name                 n;

#endif /* __STDC__ */

{
   int                 i;


   i = 0;
   do
	BIOwriteByte (outfile, n[i++]);
   while (n[i - 1] != '\0');
}


/* wrbool       ecrit un booleen dans le fichier de sortie */

#ifdef __STDC__
void                wrbool (boolean b)

#else  /* __STDC__ */
void                wrbool (b)
boolean             b;

#endif /* __STDC__ */

{

   if (b)
      BIOwriteByte (outfile, '\1');
   else
      BIOwriteByte (outfile, '\0');
}


/* WriteTransCondition  ecrit un type de condition d'application de regle de */
/* traduction */

#ifdef __STDC__
void                WriteTransCondition (TransCondition T)

#else  /* __STDC__ */
void                WriteTransCondition (T)
TransCondition        T;

#endif /* __STDC__ */

{

   switch (T)
	 {
	    case TcondFirst:
	       BIOwriteByte (outfile, C_TR_FIRST);
	       break;
	    case TcondLast:
	       BIOwriteByte (outfile, C_TR_LAST);
	       break;
	    case TcondDefined:
	       BIOwriteByte (outfile, C_TR_DEFINED);
	       break;
	    case TcondReferred:
	       BIOwriteByte (outfile, C_TR_REFERRED);
	       break;
	    case TcondFirstRef:
	       BIOwriteByte (outfile, C_TR_FIRSTREF);
	       break;
	    case TcondLastRef:
	       BIOwriteByte (outfile, C_TR_LAST_REF);
	       break;
	    case TcondWithin:
	       BIOwriteByte (outfile, C_TR_WITHIN);
	       break;
	    case TcondFirstWithin:
	       BIOwriteByte (outfile, C_TR_FIRST_WITHIN);
	       break;
	    case TcondAttr:
	       BIOwriteByte (outfile, C_TR_ATTRIBUTE);
	       break;
	    case TcondPresentation:
	       BIOwriteByte (outfile, C_TR_PRESENT);
	       break;
	    case TcondPRule:
	       BIOwriteByte (outfile, C_TR_PRULE);
	       break;
	    case TcondComment:
	       BIOwriteByte (outfile, C_TR_COMMENT);
	       break;
	    case TcondAlphabet:
	       BIOwriteByte (outfile, C_TR_ALPHABET);
	       break;
	    case TcondAttributes:
	       BIOwriteByte (outfile, C_TR_ATTRIBUTES);
	       break;
	    case TcondFirstAttr:
	       BIOwriteByte (outfile, C_TR_FIRSTATTR);
	       break;
	    case TcondLastAttr:
	       BIOwriteByte (outfile, C_TR_LASTATTR);
	       break;
	    case TcondComputedPage:
	       BIOwriteByte (outfile, C_TR_COMPUTEDPAGE);
	       break;
	    case TcondStartPage:
	       BIOwriteByte (outfile, C_TR_STARTPAGE);
	       break;
	    case TcondUserPage:
	       BIOwriteByte (outfile, C_TR_USERPAGE);
	       break;
	    case TcondReminderPage:
	       BIOwriteByte (outfile, C_TR_REMINDERPAGE);

	       break;
	    case TcondEmpty:
	       BIOwriteByte (outfile, C_TR_EMPTY);
	       break;
	    case TcondExternalRef:
	       BIOwriteByte (outfile, C_TR_EXTERNALREF);
	       break;
	 }
}


/* WriteRelatNivAsc */

#ifdef __STDC__
void                WriteRelatNivAsc (RelatNAscend R)

#else  /* __STDC__ */
void                WriteRelatNivAsc (R)
RelatNAscend         R;

#endif /* __STDC__ */

{
   switch (R)
	 {
	    case RelGreater:
	       BIOwriteByte (outfile, C_WITHIN_GT);
	       break;
	    case RelLess:
	       BIOwriteByte (outfile, C_WITHIN_LT);
	       break;
	    case RelEquals:
	       BIOwriteByte (outfile, C_WITHIN_EQ);
	       break;
	 }
}


/* WriteTRuleType      ecrit un type de regle de traduction */

#ifdef __STDC__
void                WriteTRuleType (TRuleType T)

#else  /* __STDC__ */
void                WriteTRuleType (T)
TRuleType       T;

#endif /* __STDC__ */

{

   switch (T)
	 {
	    case TCreate:
	       BIOwriteByte (outfile, C_TR_CREATE);
	       break;
	    case TGet:
	       BIOwriteByte (outfile, C_TR_GET);
	       break;
	    case TUse:
	       BIOwriteByte (outfile, C_TR_USE);
	       break;
	    case TRemove:
	       BIOwriteByte (outfile, C_TR_REMOVE);
	       break;
	    case TWrite:
	       BIOwriteByte (outfile, C_TR_WRITE);
	       break;
	    case TRead:
	       BIOwriteByte (outfile, C_TR_READ);
	       break;
	    case TInclude:
	       BIOwriteByte (outfile, C_TR_INCLUDE);
	       break;
	    case TNoTranslation:
	       BIOwriteByte (outfile, C_TR_NOTRANSL);
	       break;
	    case TNoLineBreak:
	       BIOwriteByte (outfile, C_TR_NOLINEBREAK);
	       break;
	    case TCopy:
	       BIOwriteByte (outfile, C_TR_COPY);
	       break;
	    case TChangeMainFile:
	       BIOwriteByte (outfile, C_TR_CHANGEFILE);
	       break;
	    case TSetCounter:
	       BIOwriteByte (outfile, C_TR_SET_COUNTER);
	       break;
	    case TAddCounter:
	       BIOwriteByte (outfile, C_TR_ADD_COUNTER);
	       break;
	 }

}


/* WriteTOrder ecrit la position ou il faut creer les chaines */
/* produites par le traducteur */

#ifdef __STDC__
void                WriteTOrder (TOrder P)

#else  /* __STDC__ */
void                WriteTOrder (P)
TOrder          P;

#endif /* __STDC__ */

{

   switch (P)
	 {
	    case TAfter:
	       BIOwriteByte (outfile, C_TR_AFTER);
	       break;
	    case TBefore:
	       BIOwriteByte (outfile, C_TR_BEFORE);
	       break;
	 }

}


/* WriteCreatedObject   ecrit le type de chose a creer dans le fichier de */
/* sortie ou au terminal */

#ifdef __STDC__
void                WriteCreatedObject (CreatedObject T)

#else  /* __STDC__ */
void                WriteCreatedObject (T)
CreatedObject            T;

#endif /* __STDC__ */

{

   switch (T)
	 {
	    case ToConst:
	       BIOwriteByte (outfile, C_OB_CONST);
	       break;
	    case ToBuffer:
	       BIOwriteByte (outfile, C_OB_BUFFER);
	       break;
	    case ToVariable:
	       BIOwriteByte (outfile, C_OB_VAR);
	       break;
	    case ToAttr:
	       BIOwriteByte (outfile, C_OB_ATTR);
	       break;
	    case ToContent:
	       BIOwriteByte (outfile, C_OB_CONTENT);
	       break;
	    case ToComment:
	       BIOwriteByte (outfile, C_OB_COMMENT);
	       break;
	    case ToAllAttr:
	       BIOwriteByte (outfile, C_OB_ATTRIBUTES);
	       break;
	    case ToPRuleValue:
	       BIOwriteByte (outfile, C_OB_PRES_VAL);
	       break;
	    case ToAllPRules:
	       BIOwriteByte (outfile, C_OB_PRESENTATION);
	       break;
	    case ToRefId:
	       BIOwriteByte (outfile, C_OB_REFID);
	       break;
	    case ToPairId:
	       BIOwriteByte (outfile, C_OB_PAIRID);
	       break;
	    case ToReferredElem:
	       BIOwriteByte (outfile, C_OB_REFERRED_ELEM);
	       break;
	    case ToFileDir:
	       BIOwriteByte (outfile, C_OB_FILEDIR);
	       break;
	    case ToFileName:
	       BIOwriteByte (outfile, C_OB_FILENAME);
	       break;
	    case ToExtension:
	       BIOwriteByte (outfile, C_OB_EXTENSION);
	       break;
	    case ToDocumentName:
	       BIOwriteByte (outfile, C_OB_DOCUMENTNAME);
	       break;
	    case ToDocumentDir:
	       BIOwriteByte (outfile, C_OB_DOCUMENTDIR);
	       break;
	    case ToReferredDocumentName:
	       BIOwriteByte (outfile, C_OB_REFERRED_DOCNAME);
	       break;
	    case ToReferredDocumentDir:
	       BIOwriteByte (outfile, C_OB_REFERRED_DOCDIR);
	       break;
	    case ToReferredRefId:
	       BIOwriteByte (outfile, C_OB_REFERRED_REFID);
	       break;
	 }

}


/* WriteRelativePosition   ecrit la position relative dans le fichier de */
/* sortie ou au terminal */

#ifdef __STDC__
void                WriteRelativePosition (TRelatPosition P)

#else  /* __STDC__ */
void                WriteRelativePosition (P)
TRelatPosition            P;

#endif /* __STDC__ */

{

   switch (P)
	 {
	    case RpSibling:
	       BIOwriteByte (outfile, C_TR_SIBLING);
	       break;
	    case RpDescend:
	       BIOwriteByte (outfile, C_TR_DESCEND);
	       break;
	    case RpReferred:
	       BIOwriteByte (outfile, C_TR_REFERRED_EL);
	       break;
	    case RpAssoc:
	       BIOwriteByte (outfile, C_TR_ASSOC);
	       break;
	 }

}


/* WriteTCounterOp        ecrit le type d'une operation sur un compteur */

#ifdef __STDC__
void                WriteTCounterOp (TCounterOp T)

#else  /* __STDC__ */
void                WriteTCounterOp (T)
TCounterOp         T;

#endif /* __STDC__ */

{

   switch (T)
	 {
	    case TCntrRank:
	       BIOwriteByte (outfile, C_TR_RANK);
	       break;
	    case TCntrRLevel:
	       BIOwriteByte (outfile, C_TR_RLEVEL);
	       break;
	    case TCntrSet:
	       BIOwriteByte (outfile, C_TR_SET);
	       break;
	    case TCntrNoOp:
	       BIOwriteByte (outfile, C_TR_NOOP);
	       break;
	 }

}


/* WriteTVarType        ecrit le type des elements de variables de traduction */

#ifdef __STDC__
void                WriteTVarType (TranslVarType T)

#else  /* __STDC__ */
void                WriteTVarType (T)
TranslVarType         T;

#endif /* __STDC__ */

{
   switch (T)
	 {
	    case VtText:
	       BIOwriteByte (outfile, C_TR_CONST);
	       break;
	    case VtCounter:
	       BIOwriteByte (outfile, C_TR_COUNTER);
	       break;
	    case VtBuffer:
	       BIOwriteByte (outfile, C_TR_BUFFER);
	       break;
	    case VtAttrVal:
	       BIOwriteByte (outfile, C_TR_ATTR);
	       break;
	    case VtFileDir:
	       BIOwriteByte (outfile, C_TR_FILEDIR);
	       break;
	    case VtFileName:
	       BIOwriteByte (outfile, C_TR_FILENAME);
	       break;
	    case VtExtension:
	       BIOwriteByte (outfile, C_TR_EXTENSION);
	       break;
	    case VtDocumentName:
	       BIOwriteByte (outfile, C_TR_DOCUMENTNAME);
	       break;
	    case VtDocumentDir:
	       BIOwriteByte (outfile, C_TR_DOCUMENTDIR);
	       break;
	 }
}

/* WriteCounterStyle ecrit un style de compteur dans le fichier */

#ifdef __STDC__
void                WriteCounterStyle (CounterStyle S)

#else  /* __STDC__ */
void                WriteCounterStyle (S)
CounterStyle          S;

#endif /* __STDC__ */

{

   switch (S)
	 {
	    case CntArabic:
	       BIOwriteByte (outfile, C_NUM_ARABIC);
	       break;
	    case CntURoman:
	       BIOwriteByte (outfile, C_NUM_ROMAN);
	       break;
	    case CntLRoman:
	       BIOwriteByte (outfile, C_NUM_LOWER_ROMAN);
	       break;
	    case CntUppercase:
	       BIOwriteByte (outfile, C_NUM_UPPERCASE);
	       break;
	    case CntLowercase:
	       BIOwriteByte (outfile, C_NUM_LOWERCASE);
	       break;
	 }

}

#ifdef __STDC__
void                wrptrregle (PtrTRule p)

#else  /* __STDC__ */
void                wrptrregle (p)
PtrTRule        p;

#endif /* __STDC__ */

{

   if (p == NULL)
      BIOwriteByte (outfile, '\0');
   else
      BIOwriteByte (outfile, '\1');
}


/* WriteRegles  ecrit une suite de regles de traduction */

#ifdef __STDC__
void                WriteRegles (PtrTRule pR)

#else  /* __STDC__ */
void                WriteRegles (pR)
PtrTRule        pR;

#endif /* __STDC__ */

{
   PtrTRule        CurRegle, NextRegle;
   PtrTRule        pRe1;


   CurRegle = pR;
   while (CurRegle != NULL)
     {
	pRe1 = CurRegle;
	wrptrregle (pRe1->TrNextTRule);
	WriteTOrder (pRe1->TrOrder);
	WriteTRuleType (pRe1->TrType);
	switch (pRe1->TrType)
	      {
		 case TCreate:
		 case TWrite:
		    WriteCreatedObject (pRe1->TrObject);
		    WriteShort (pRe1->TrObjectNum);
		    wrnom (pRe1->TrObjectNature);
		    wrbool (pRe1->TrReferredObj);
		    WriteShort (pRe1->TrFileNameVar);
		    break;
		 case TGet:
		 case TCopy:
		    WriteShort (pRe1->TrElemType);
		    wrnom (pRe1->TrElemNature);
		    WriteRelativePosition (pRe1->TrRelPosition);
		    break;
		 case TUse:
		    wrnom (pRe1->TrNature);
		    wrnom (pRe1->TrTranslSchemaName);
		    break;
		 case TRemove:
		 case TNoTranslation:
		 case TNoLineBreak:

		    break;
		 case TRead:
		    WriteShort (pRe1->TrBuffer);
		    break;
		 case TInclude:
		    WriteCreatedObject (pRe1->TrBufOrConst);
		    WriteShort (pRe1->TrInclFile);
		    break;
		 case TChangeMainFile:
		    WriteShort (pRe1->TrNewFileVar);
		    break;
		 case TSetCounter:
		 case TAddCounter:
		    WriteShort (pRe1->TrCounterNum);
		    WriteShort (pRe1->TrCounterParam);
		    break;
		 default:
		    break;
	      }

	NextRegle = pRe1->TrNextTRule;
	free (CurRegle);
	CurRegle = NextRegle;
     }
}

#ifdef __STDC__
void                wrptrbloc (PtrTRuleBlock b)

#else  /* __STDC__ */
void                wrptrbloc (b)
PtrTRuleBlock       b;

#endif /* __STDC__ */

{

   if (b == NULL)
      BIOwriteByte (outfile, '\0');
   else
      BIOwriteByte (outfile, '\1');
}


/* WriteBlocks   ecrit une suite de blocs de regles */

#ifdef __STDC__
void                WriteBlocks (PtrTRuleBlock pB, PtrSSchema pSchStr)

#else  /* __STDC__ */
void                WriteBlocks (pB, pSchStr)
PtrTRuleBlock       pB;
PtrSSchema        pSchStr;

#endif /* __STDC__ */

{
   PtrTRuleBlock       CurBloc, NextBloc;
   int                 ncond;
   TranslCondition        *Cond;

   CurBloc = pB;
   while (CurBloc != NULL)
     {
	wrptrbloc (CurBloc->TbNextBlock);
	WriteShort (CurBloc->TbNConditions);
	/* ecrit les conditions d'application des regles du bloc */
	for (ncond = 1; ncond <= CurBloc->TbNConditions; ncond++)
	  {
	     Cond = &CurBloc->TbCondition[ncond - 1];
	     WriteTransCondition (Cond->TcCondition);
	     wrbool (Cond->TcNegativeCond);
	     wrbool (Cond->TcTarget);
	     WriteShort (Cond->TcAscendType);
	     wrnom (Cond->TcAscendNature);
	     WriteSignedShort (Cond->TcAscendRelLevel);
	     switch (Cond->TcCondition)
		   {
		      case TcondAlphabet:
			 BIOwriteByte (outfile, Cond->TcAlphabet);
			 break;
		      case TcondWithin:
		      case TcondFirstWithin:
			 WriteShort (Cond->TcElemType);
			 wrnom (Cond->TcElemNature);
			 wrbool (Cond->TcImmediatelyWithin);
			 WriteRelatNivAsc (Cond->TcAscendRel);
			 WriteShort (Cond->TcAscendLevel);
			 break;
		      case TcondAttr:
			 WriteShort (Cond->TcAttr);
			 switch (pSchStr->SsAttribute[Cond->TcAttr - 1].AttrType)
			       {
				  case AtNumAttr:
				     WriteSignedShort (Cond->TcLowerBound);
				     WriteSignedShort (Cond->TcUpperBound);
				     break;
				  case AtTextAttr:
				     wrnom (Cond->TcTextValue);
				     break;
				  case AtReferenceAttr:

				     break;
				  case AtEnumAttr:
				     WriteShort (Cond->TcAttrValue);
				     break;
				  default:
				     break;
			       }

			 break;
		      case TcondPRule:
			 WriteShort (Cond->TcAttr);
			 if (Cond->TcAttr == PtSize ||
			     Cond->TcAttr == PtIndent ||
			     Cond->TcAttr == PtLineSpacing ||
			     Cond->TcAttr == PtLineWeight ||
			     Cond->TcAttr == PtFillPattern ||
			     Cond->TcAttr == PtBackground ||
			     Cond->TcAttr == PtForeground)
			   {
			      WriteSignedShort (Cond->TcLowerBound);
			      WriteSignedShort (Cond->TcUpperBound);
			   }
			 else
			    BIOwriteByte (outfile, Cond->TcPresValue);
			 break;
		      default:
			 break;
		   }
	  }
	WriteRegles (CurBloc->TbFirstTRule);
	NextBloc = CurBloc->TbNextBlock;
	free (CurBloc);
	CurBloc = NextBloc;
     }
}


/* WriteReglesAttr      ecrit les regles de traduction de l'attribut */
/* de numero att appartenant au schema de structure pointe' par pSchStr */

#ifdef __STDC__
void                WriteReglesAttr (int att, PtrSSchema pSchStr, PtrTSchema pSchTra)

#else  /* __STDC__ */
void                WriteReglesAttr (att, pSchStr, pSchTra)
int                 att;
PtrSSchema        pSchStr;
PtrTSchema        pSchTra;

#endif /* __STDC__ */

{
   int                 i;
   AttributeTransl      *pRT1;
   TranslNumAttrCase        *pTC1;


   pRT1 = &pSchTra->TsAttrTRule[att - 1];
   WriteShort (pRT1->AtrElemType);
   switch (pSchStr->SsAttribute[att - 1].AttrType)
	 {
	    case AtNumAttr:
	       WriteShort (pRT1->AtrNCases);
	       for (i = 1; i <= pRT1->AtrNCases; i++)
		 {
		    pTC1 = &pRT1->AtrCase[i - 1];
		    WriteSignedShort (pTC1->TaLowerBound);
		    WriteSignedShort (pTC1->TaUpperBound);
		    WriteBlocks (pTC1->TaTRuleBlock, pSchStr);
		 }
	       break;
	    case AtTextAttr:
	       wrnom (pRT1->AtrTextValue);
	       WriteBlocks (pRT1->AtrTxtTRuleBlock, pSchStr);
	       break;
	    case AtReferenceAttr:
	       WriteBlocks (pRT1->AtrRefTRuleBlock, pSchStr);
	       break;
	    case AtEnumAttr:
	       for (i = 0; i <= pSchStr->SsAttribute[att - 1].AttrNEnumValues; i++)
		  WriteBlocks (pRT1->AtrEnuTRuleBlock[i], pSchStr);
	       break;
	    default:
	       break;
	 }
}

/* WriteReglesPres      ecrit les regles de traduction de la presentation */
/* de numero pres */

#ifdef __STDC__
void                WriteReglesPres (int pres, PtrSSchema pSchStr, PtrTSchema pSchTra)

#else  /* __STDC__ */
void                WriteReglesPres (pres, pSchStr, pSchTra)
int                 pres;
PtrSSchema        pSchStr;
PtrTSchema        pSchTra;

#endif /* __STDC__ */

{
   int                 i;
   PRuleTransl       *pRT1;
   TranslNumAttrCase        *pTC1;


   pRT1 = &pSchTra->TsPresTRule[pres - 1];
   if (pRT1->RtExist)
      if (pres == PtSize + 1 || pres == PtIndent + 1 ||
       pres == PtLineSpacing + 1 || pres == PtLineWeight + 1 ||
	  pres == PtFillPattern + 1 || pres == PtBackground + 1 ||
	  pres == PtForeground + 1)
	 /* presentation a valeur numerique */
	{
	   WriteShort (pRT1->RtNCase);
	   for (i = 1; i <= pRT1->RtNCase; i++)
	     {
		pTC1 = &pRT1->RtCase[i - 1];
		WriteSignedShort (pTC1->TaLowerBound);
		WriteSignedShort (pTC1->TaUpperBound);
		WriteBlocks (pTC1->TaTRuleBlock, pSchStr);
	     }
	}
      else
	 for (i = 0; i <= MAX_TRANSL_PRES_VAL; i++)
	   {
	      BIOwriteByte (outfile, pRT1->RtPRuleValue[i]);
	      WriteBlocks (pRT1->RtPRuleValueBlock[i], pSchStr);
	   }
}


/* WriteTranslationSchemas    cree le fichier de sortie et y ecrit le schema de */
/* traduction */
#ifdef __STDC__
boolean             WriteTranslationSchemas (Name fname, PtrTSchema pSchTra, PtrSSchema pSchStr)

#else  /* __STDC__ */
boolean             WriteTranslationSchemas (fname, pSchTra, pSchStr)
Name                 fname;
PtrTSchema        pSchTra;
PtrSSchema        pSchStr;

#endif /* __STDC__ */

{
   int                 i, j;	/* met le suffixe .TRA a la fin du

				 * nom de fichier */
   PtrTSchema        pSc1;
   TCounter         *pTr1;
   TranslVariable            *pVa1;
   TranslVarItem        *pVarT1;
   AttributeTransl      *pRT1;
   AlphabetTransl            *pTrad1;
   StringTransl            *pTradCa6;
   PRuleTransl       *pRTP1;

   /* cree le fichier */
   outfile = BIOwriteOpen (fname);
   if (outfile == 0)
      return False;
   /* recopie le code d'identification du schema de structure */
   pSchTra->TsStructCode = pSchStr->SsCode;		/* ecrit la partie fixe */
   pSc1 = pSchTra;
   wrnom (pSc1->TsStructName);
   WriteShort (pSc1->TsStructCode);
   WriteShort (pSc1->TsLineLength);
   wrnom (pSc1->TsEOL);
   wrnom (pSc1->TsTranslEOL);
   WriteShort (pSc1->TsNConstants);
   WriteShort (pSc1->TsNCounters);
   WriteShort (pSc1->TsNVariables);
   WriteShort (pSc1->TsNBuffers);
   for (i = 1; i <= pSc1->TsNConstants; i++)
      WriteShort (pSc1->TsConstBegin[i - 1]);
   for (i = 1; i <= pSc1->TsNCounters; i++)
     {
	pTr1 = &pSc1->TsCounter[i - 1];
	WriteTCounterOp (pTr1->TnOperation);
	if (pTr1->TnOperation != TCntrNoOp)
	   WriteShort (pTr1->TnElemType1);
	if (pTr1->TnOperation == TCntrSet)
	  {
	     WriteShort (pTr1->TnElemType2);
	     WriteShort (pTr1->TnParam1);
	     WriteShort (pTr1->TnParam2);
	  }
	else if (pTr1->TnOperation == TCntrRank)
	   WriteSignedShort (pTr1->TnAcestorLevel);
	WriteShort (pTr1->TnAttrInit);
     }
   for (i = 1; i <= pSc1->TsNVariables; i++)
     {
	pVa1 = &pSc1->TsVariable[i - 1];
	WriteShort (pVa1->TrvNItems);
	for (j = 1; j <= pVa1->TrvNItems; j++)
	  {
	     pVarT1 = &pVa1->TrvItem[j - 1];
	     WriteTVarType (pVarT1->TvType);
	     WriteShort (pVarT1->TvItem);
	     if (pVarT1->TvType == VtCounter)
	       {
		  WriteShort (pVarT1->TvLength);
		  WriteCounterStyle (pVarT1->TvCounterStyle);
	       }
	  }
     }
   WriteShort (pSc1->TsPictureBuffer);
   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrptrbloc (pSc1->TsElemTRule[i - 1]);
   for (i = 1; i <= pSchStr->SsNRules; i++)
      wrbool (pSc1->TsInheritAttr[i - 1]);
   for (i = 1; i <= pSchStr->SsNAttributes; i++)
     {
	pRT1 = &pSc1->TsAttrTRule[i - 1];
	switch (pSchStr->SsAttribute[i - 1].AttrType)
	      {
		 case AtNumAttr:
		    WriteShort (pRT1->AtrNCases);
		    for (j = 1; j <= pRT1->AtrNCases; j++)
		       wrptrbloc (pRT1->AtrCase[j - 1].TaTRuleBlock);
		    break;
		 case AtTextAttr:
		    wrptrbloc (pRT1->AtrTxtTRuleBlock);
		    break;
		 case AtReferenceAttr:
		    wrptrbloc (pRT1->AtrRefTRuleBlock);
		    break;
		 case AtEnumAttr:
		    for (j = 0; j <= pSchStr->SsAttribute[i - 1].AttrNEnumValues; j++)
		       wrptrbloc (pRT1->AtrEnuTRuleBlock[j]);
		    break;
		 default:
		    break;
	      }

     }
   for (i = 1; i <= MAX_TRANSL_PRULE; i++)
     {
	pRTP1 = &pSc1->TsPresTRule[i - 1];
	wrbool (pRTP1->RtExist);
	if (pRTP1->RtExist)
	   if (i == PtSize + 1 || i == PtIndent + 1 ||
	       i == PtLineSpacing + 1 ||
	       i == PtLineWeight + 1 || i == PtFillPattern + 1 ||
	       i == PtBackground + 1 || i == PtForeground + 1)
	      /* presentation a valeur numerique */
	     {
		WriteShort (pRTP1->RtNCase);
		for (j = 1; j <= pRTP1->RtNCase; j++)
		   wrptrbloc (pRTP1->RtCase[j - 1].TaTRuleBlock);
	     }
	   else
	      for (j = 0; j <= MAX_TRANSL_PRES_VAL; j++)
		 wrptrbloc (pRTP1->RtPRuleValueBlock[j]);
     }
   WriteShort (pSc1->TsNTranslAlphabets);
   for (i = 1; i <= pSc1->TsNTranslAlphabets; i++)
     {
	pTrad1 = &pSc1->TsTranslAlphabet[i - 1];
	BIOwriteByte (outfile, pTrad1->AlAlphabet);
	WriteShort (pTrad1->AlBegin);
	WriteShort (pTrad1->AlEnd);
     }
   WriteShort (pSc1->TsSymbolFirst);
   WriteShort (pSc1->TsSymbolLast);
   WriteShort (pSc1->TsGraphicsFirst);
   WriteShort (pSc1->TsGraphicsLast);
   WriteShort (pSc1->TsNCharTransls);
   for (i = 1; i <= pSc1->TsNCharTransls; i++)
     {
	pTradCa6 = &pSc1->TsCharTransl[i - 1];
	j = 0;
	do
	     BIOwriteByte (outfile, pTradCa6->StSource[j++]);
	while (pTradCa6->StSource[j - 1] != '\0');
	j = 0;
	do
	     BIOwriteByte (outfile, pTradCa6->StTarget[j++]);
	while (pTradCa6->StTarget[j - 1] != '\0');
     }
   for (i = 0; i < pSc1->TsNConstants; i++)
     {
	j = pSc1->TsConstBegin[i] - 1;
	do
	     BIOwriteByte (outfile, pSc1->TsConstant[j++]);
	while (pSc1->TsConstant[j - 1] != '\0');
     }
   for (i = 0; i < pSchStr->SsNRules; i++)
      WriteBlocks (pSc1->TsElemTRule[i], pSchStr);
   for (i = 0; i < pSchStr->SsNAttributes; i++)
      WriteReglesAttr (i+1, pSchStr, pSchTra);
   for (i = 0; i < MAX_TRANSL_PRULE; i++)
      WriteReglesPres (i+1, pSchStr, pSchTra);
   BIOwriteClose (outfile);
   return True;
}

