/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */

/*
 * Ce module sauve dans un fichier un schema de traduction qui a ete
 * compile' en memoire.
 *
 * Author: V. Quint (INRIA)
 *         R. Guetari (W3C/INRIA) : Unicode.
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "consttra.h"
#include "typemedia.h"
#include "typetra.h"
#include "fileaccess.h"

#define THOT_EXPORT extern
static BinFile      outfile;

#include "fileaccess_f.h"
#include "writetra_f.h"

/*----------------------------------------------------------------------
   WriteShort      ecrit un entier sur 2 octets			
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
   WriteSignedShort  ecrit un entier signe' sur 2 octets		
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
   WriteName                ecrit un nom				
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteName (Name n)

#else  /* __STDC__ */
void                WriteName (n)
Name                n;

#endif /* __STDC__ */

{
   int                 i;

   i = 0;
   do
      TtaWriteWideChar (outfile, n[i++], ISO_8859_1);
   while (n[i - 1] != TEXT('\0'));
}


/*----------------------------------------------------------------------
   WriteBoolean       ecrit un booleen				
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteBoolean (ThotBool b)

#else  /* __STDC__ */
void                WriteBoolean (b)
ThotBool             b;

#endif /* __STDC__ */

{

   if (b)
      TtaWriteByte (outfile, '\1');
   else
      TtaWriteByte (outfile, '\0');
}


/*----------------------------------------------------------------------
   WriteTransCondition  ecrit un type de condition d'application	
   de regle de traduction						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteTransCondition (TransCondition cond)

#else  /* __STDC__ */
void                WriteTransCondition (cond)
TransCondition      cond;

#endif /* __STDC__ */

{

   switch (cond)
	 {
	    case TcondFirst:
	       TtaWriteByte (outfile, C_TR_FIRST);
	       break;
	    case TcondLast:
	       TtaWriteByte (outfile, C_TR_LAST);
	       break;
	    case TcondDefined:
	       TtaWriteByte (outfile, C_TR_DEFINED);
	       break;
	    case TcondReferred:
	       TtaWriteByte (outfile, C_TR_REFERRED);
	       break;
	    case TcondFirstRef:
	       TtaWriteByte (outfile, C_TR_FIRSTREF);
	       break;
	    case TcondLastRef:
	       TtaWriteByte (outfile, C_TR_LAST_REF);
	       break;
	    case TcondWithin:
	       TtaWriteByte (outfile, C_TR_WITHIN);
	       break;
	    case TcondFirstWithin:
	       TtaWriteByte (outfile, C_TR_FIRST_WITHIN);
	       break;
	    case TcondAttr:
	       TtaWriteByte (outfile, C_TR_ATTRIBUTE);
	       break;
	    case TcondPresentation:
	       TtaWriteByte (outfile, C_TR_PRESENT);
	       break;
	    case TcondPRule:
	       TtaWriteByte (outfile, C_TR_PRULE);
	       break;
	    case TcondComment:
	       TtaWriteByte (outfile, C_TR_COMMENT);
	       break;
	    case TcondAlphabet:
	       TtaWriteByte (outfile, C_TR_ALPHABET);
	       break;
	    case TcondAttributes:
	       TtaWriteByte (outfile, C_TR_ATTRIBUTES);
	       break;
	    case TcondFirstAttr:
	       TtaWriteByte (outfile, C_TR_FIRSTATTR);
	       break;
	    case TcondLastAttr:
	       TtaWriteByte (outfile, C_TR_LASTATTR);
	       break;
	    case TcondComputedPage:
	       TtaWriteByte (outfile, C_TR_COMPUTEDPAGE);
	       break;
	    case TcondStartPage:
	       TtaWriteByte (outfile, C_TR_STARTPAGE);
	       break;
	    case TcondUserPage:
	       TtaWriteByte (outfile, C_TR_USERPAGE);
	       break;
	    case TcondReminderPage:
	       TtaWriteByte (outfile, C_TR_REMINDERPAGE);

	       break;
	    case TcondEmpty:
	       TtaWriteByte (outfile, C_TR_EMPTY);
	       break;
	    case TcondExternalRef:
	       TtaWriteByte (outfile, C_TR_EXTERNALREF);
	       break;
	    case TcondElementType:
	       TtaWriteByte (outfile, C_TR_ELEMENTTYPE);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteRelatNAscend						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteRelatNAscend (RelatNAscend rel)

#else  /* __STDC__ */
void                WriteRelatNAscend (rel)
RelatNAscend        rel;

#endif /* __STDC__ */

{
   switch (rel)
	 {
	    case RelGreater:
	       TtaWriteByte (outfile, C_WITHIN_GT);
	       break;
	    case RelLess:
	       TtaWriteByte (outfile, C_WITHIN_LT);
	       break;
	    case RelEquals:
	       TtaWriteByte (outfile, C_WITHIN_EQ);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteTRuleType      ecrit un type de regle de traduction	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteTRuleType (TRuleType typ)

#else  /* __STDC__ */
void                WriteTRuleType (typ)
TRuleType           typ;

#endif /* __STDC__ */

{

   switch (typ)
	 {
	    case TCreate:
	       TtaWriteByte (outfile, C_TR_CREATE);
	       break;
	    case TGet:
	       TtaWriteByte (outfile, C_TR_GET);
	       break;
	    case TUse:
	       TtaWriteByte (outfile, C_TR_USE);
	       break;
	    case TRemove:
	       TtaWriteByte (outfile, C_TR_REMOVE);
	       break;
	    case TWrite:
	       TtaWriteByte (outfile, C_TR_WRITE);
	       break;
	    case TRead:
	       TtaWriteByte (outfile, C_TR_READ);
	       break;
	    case TInclude:
	       TtaWriteByte (outfile, C_TR_INCLUDE);
	       break;
	    case TNoTranslation:
	       TtaWriteByte (outfile, C_TR_NOTRANSL);
	       break;
	    case TNoLineBreak:
	       TtaWriteByte (outfile, C_TR_NOLINEBREAK);
	       break;
	    case TCopy:
	       TtaWriteByte (outfile, C_TR_COPY);
	       break;
	    case TChangeMainFile:
	       TtaWriteByte (outfile, C_TR_CHANGEFILE);
	       break;
	    case TSetCounter:
	       TtaWriteByte (outfile, C_TR_SET_COUNTER);
	       break;
	    case TAddCounter:
	       TtaWriteByte (outfile, C_TR_ADD_COUNTER);
	       break;
	    case TIndent:
	       TtaWriteByte (outfile, C_TR_INDENT);
	       break;
	    case TRemoveFile:
	       TtaWriteByte (outfile, C_TR_REMOVEFILE);
	       break;
	 }

}


/*----------------------------------------------------------------------
   WriteTOrder ecrit la position ou il faut creer les chaines	
   produites par le traducteur					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteTOrder (TOrder order)

#else  /* __STDC__ */
void                WriteTOrder (order)
TOrder              order;

#endif /* __STDC__ */

{

   switch (order)
	 {
	    case TAfter:
	       TtaWriteByte (outfile, C_TR_AFTER);
	       break;
	    case TBefore:
	       TtaWriteByte (outfile, C_TR_BEFORE);
	       break;
	 }

}


/*----------------------------------------------------------------------
   WriteCreatedObject   ecrit le type de chose a creer		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteCreatedObject (CreatedObject obj)

#else  /* __STDC__ */
void                WriteCreatedObject (obj)
CreatedObject       obj;

#endif /* __STDC__ */

{

   switch (obj)
	 {
	    case ToConst:
	       TtaWriteByte (outfile, C_OB_CONST);
	       break;
	    case ToBuffer:
	       TtaWriteByte (outfile, C_OB_BUFFER);
	       break;
	    case ToVariable:
	       TtaWriteByte (outfile, C_OB_VAR);
	       break;
	    case ToAttr:
	       TtaWriteByte (outfile, C_OB_ATTR);
	       break;
	    case ToContent:
	       TtaWriteByte (outfile, C_OB_CONTENT);
	       break;
	    case ToComment:
	       TtaWriteByte (outfile, C_OB_COMMENT);
	       break;
	    case ToAllAttr:
	       TtaWriteByte (outfile, C_OB_ATTRIBUTES);
	       break;
	    case ToPRuleValue:
	       TtaWriteByte (outfile, C_OB_PRES_VAL);
	       break;
	    case ToAllPRules:
	       TtaWriteByte (outfile, C_OB_PRESENTATION);
	       break;
	    case ToRefId:
	       TtaWriteByte (outfile, C_OB_REFID);
	       break;
	    case ToPairId:
	       TtaWriteByte (outfile, C_OB_PAIRID);
	       break;
	    case ToReferredElem:
	       TtaWriteByte (outfile, C_OB_REFERRED_ELEM);
	       break;
	    case ToFileDir:
	       TtaWriteByte (outfile, C_OB_FILEDIR);
	       break;
	    case ToFileName:
	       TtaWriteByte (outfile, C_OB_FILENAME);
	       break;
	    case ToExtension:
	       TtaWriteByte (outfile, C_OB_EXTENSION);
	       break;
	    case ToDocumentName:
	       TtaWriteByte (outfile, C_OB_DOCUMENTNAME);
	       break;
	    case ToDocumentDir:
	       TtaWriteByte (outfile, C_OB_DOCUMENTDIR);
	       break;
	    case ToReferredDocumentName:
	       TtaWriteByte (outfile, C_OB_REFERRED_DOCNAME);
	       break;
	    case ToReferredDocumentDir:
	       TtaWriteByte (outfile, C_OB_REFERRED_DOCDIR);
	       break;
	    case ToReferredRefId:
	       TtaWriteByte (outfile, C_OB_REFERRED_REFID);
	       break;
	    case ToTranslatedAttr:
	       TtaWriteByte (outfile, C_OB_TRANSLATED_ATT);
	       break;
	 }

}


/*----------------------------------------------------------------------
   WriteRelativePosition   ecrit une position relative		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteRelativePosition (TRelatPosition pos)

#else  /* __STDC__ */
void                WriteRelativePosition (pos)
TRelatPosition      pos;

#endif /* __STDC__ */

{

   switch (pos)
	 {
	    case RpSibling:
	       TtaWriteByte (outfile, C_TR_SIBLING);
	       break;
	    case RpDescend:
	       TtaWriteByte (outfile, C_TR_DESCEND);
	       break;
	    case RpReferred:
	       TtaWriteByte (outfile, C_TR_REFERRED_EL);
	       break;
	    case RpAssoc:
	       TtaWriteByte (outfile, C_TR_ASSOC);
	       break;
	 }

}


/*----------------------------------------------------------------------
   WriteIndentType   ecrit un type de regle d'indentation		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         WriteIndentType (TIndentType typ)

#else  /* __STDC__ */
static void         WriteIndentType (typ)
TIndentType         typ;

#endif /* __STDC__ */

{

   switch (typ)
	 {
	    case ItAbsolute:
	       TtaWriteByte (outfile, C_TR_ABSOLUTE);
	       break;
	    case ItRelative:
	       TtaWriteByte (outfile, C_TR_RELATIVE);
	       break;
	    case ItSuspend:
	       TtaWriteByte (outfile, C_TR_SUSPEND);
	       break;
	    case ItResume:
	       TtaWriteByte (outfile, C_TR_RESUME);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteTCounterOp   ecrit le type d'une operation sur un compteur	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteTCounterOp (TCounterOp op)

#else  /* __STDC__ */
void                WriteTCounterOp (op)
TCounterOp          op;

#endif /* __STDC__ */

{

   switch (op)
	 {
	    case TCntrRank:
	       TtaWriteByte (outfile, C_TR_RANK);
	       break;
	    case TCntrRLevel:
	       TtaWriteByte (outfile, C_TR_RLEVEL);
	       break;
	    case TCntrSet:
	       TtaWriteByte (outfile, C_TR_SET);
	       break;
	    case TCntrNoOp:
	       TtaWriteByte (outfile, C_TR_NOOP);
	       break;
	 }

}


/*----------------------------------------------------------------------
   WriteTVarType ecrit le type des elements de variables de traduction 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteTVarType (TranslVarType typ)

#else  /* __STDC__ */
void                WriteTVarType (typ)
TranslVarType       typ;

#endif /* __STDC__ */

{
   switch (typ)
	 {
	    case VtText:
	       TtaWriteByte (outfile, C_TR_CONST);
	       break;
	    case VtCounter:
	       TtaWriteByte (outfile, C_TR_COUNTER);
	       break;
	    case VtBuffer:
	       TtaWriteByte (outfile, C_TR_BUFFER);
	       break;
	    case VtAttrVal:
	       TtaWriteByte (outfile, C_TR_ATTR);
	       break;
	    case VtFileDir:
	       TtaWriteByte (outfile, C_TR_FILEDIR);
	       break;
	    case VtFileName:
	       TtaWriteByte (outfile, C_TR_FILENAME);
	       break;
	    case VtExtension:
	       TtaWriteByte (outfile, C_TR_EXTENSION);
	       break;
	    case VtDocumentName:
	       TtaWriteByte (outfile, C_TR_DOCUMENTNAME);
	       break;
	    case VtDocumentDir:
	       TtaWriteByte (outfile, C_TR_DOCUMENTDIR);
	       break;
	 }
}

/*----------------------------------------------------------------------
   WriteCounterStyle ecrit un style de compteur dans le fichier	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteCounterStyle (CounterStyle style)

#else  /* __STDC__ */
void                WriteCounterStyle (style)
CounterStyle        style;

#endif /* __STDC__ */

{

   switch (style)
	 {
	    case CntArabic:
	       TtaWriteByte (outfile, C_NUM_ARABIC);
	       break;
	    case CntURoman:
	       TtaWriteByte (outfile, C_NUM_ROMAN);
	       break;
	    case CntLRoman:
	       TtaWriteByte (outfile, C_NUM_LOWER_ROMAN);
	       break;
	    case CntUppercase:
	       TtaWriteByte (outfile, C_NUM_UPPERCASE);
	       break;
	    case CntLowercase:
	       TtaWriteByte (outfile, C_NUM_LOWERCASE);
	       break;
	 }

}

/*----------------------------------------------------------------------
   WriteTRulePtr	ecrit un pointeur de regle de traduction	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteTRulePtr (PtrTRule ptr)

#else  /* __STDC__ */
void                WriteTRulePtr (ptr)
PtrTRule            ptr;

#endif /* __STDC__ */

{
   if (ptr == NULL)
      TtaWriteByte (outfile, '\0');
   else
      TtaWriteByte (outfile, '\1');
}


/*----------------------------------------------------------------------
   WriteTRules  ecrit une suite de regles de traduction		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteTRules (PtrTRule pTRule)

#else  /* __STDC__ */
void                WriteTRules (pTRule)
PtrTRule            pTRule;

#endif /* __STDC__ */

{
   PtrTRule            currentRule, nextRule;

   currentRule = pTRule;
   while (currentRule != NULL)
     {
	WriteTRulePtr (currentRule->TrNextTRule);
	WriteTOrder (currentRule->TrOrder);
	WriteTRuleType (currentRule->TrType);
	switch (currentRule->TrType)
	      {
		 case TCreate:
		 case TWrite:
		    WriteCreatedObject (currentRule->TrObject);
		    WriteShort (currentRule->TrObjectNum);
		    WriteName (currentRule->TrObjectNature);
		    WriteBoolean (currentRule->TrReferredObj);
		    WriteShort (currentRule->TrFileNameVar);
		    break;
		 case TGet:
		 case TCopy:
		    WriteShort (currentRule->TrElemType);
		    WriteName (currentRule->TrElemNature);
		    WriteRelativePosition (currentRule->TrRelPosition);
		    break;
		 case TUse:
		    WriteName (currentRule->TrNature);
		    WriteName (currentRule->TrTranslSchemaName);
		    break;
		 case TRemove:
		 case TNoTranslation:
		 case TNoLineBreak:

		    break;
		 case TRead:
		    WriteShort (currentRule->TrBuffer);
		    break;
		 case TInclude:
		    WriteCreatedObject (currentRule->TrBufOrConst);
		    WriteShort (currentRule->TrInclFile);
		    break;
		 case TChangeMainFile:
		 case TRemoveFile:
		    WriteShort (currentRule->TrNewFileVar);
		    break;
		 case TSetCounter:
		 case TAddCounter:
		    WriteShort (currentRule->TrCounterNum);
		    WriteShort (currentRule->TrCounterParam);
		    break;
		 case TIndent:
		    WriteShort (currentRule->TrIndentFileNameVar);
		    WriteSignedShort (currentRule->TrIndentVal);
		    WriteIndentType (currentRule->TrIndentType);
		 default:
		    break;
	      }

	nextRule = currentRule->TrNextTRule;
	free (currentRule);
	currentRule = nextRule;
     }
}

/*----------------------------------------------------------------------
   WriteBlockPtr							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteBlockPtr (PtrTRuleBlock pBlock)

#else  /* __STDC__ */
void                WriteBlockPtr (pBlock)
PtrTRuleBlock       pBlock;

#endif /* __STDC__ */

{

   if (pBlock == NULL)
      TtaWriteByte (outfile, '\0');
   else
      TtaWriteByte (outfile, '\1');
}


/*----------------------------------------------------------------------
   WriteBlocks   ecrit une suite de blocs de regles		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteBlocks (PtrTRuleBlock pBlock, PtrSSchema pSS)

#else  /* __STDC__ */
void                WriteBlocks (pBlock, pSS)
PtrTRuleBlock       pBlock;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   PtrTRuleBlock       curBlock, nextBlock;
   TranslCondition    *pCond;
   int                 cond;

   curBlock = pBlock;
   while (curBlock != NULL)
     {
	WriteBlockPtr (curBlock->TbNextBlock);
	WriteShort (curBlock->TbNConditions);
	/* ecrit les conditions d'application des regles du bloc */
	for (cond = 0; cond < curBlock->TbNConditions; cond++)
	  {
	     pCond = &curBlock->TbCondition[cond];
	     WriteTransCondition (pCond->TcCondition);
	     WriteBoolean (pCond->TcNegativeCond);
	     WriteBoolean (pCond->TcTarget);
	     WriteShort (pCond->TcAscendType);
	     WriteName (pCond->TcAscendNature);
	     WriteSignedShort (pCond->TcAscendRelLevel);
	     switch (pCond->TcCondition)
		   {
		      case TcondAlphabet:
			 TtaWriteByte (outfile, pCond->TcAlphabet);
			 break;
		      case TcondWithin:
		      case TcondFirstWithin:
			 WriteShort (pCond->TcElemType);
			 WriteName (pCond->TcElemNature);
			 WriteBoolean (pCond->TcImmediatelyWithin);
			 WriteRelatNAscend (pCond->TcAscendRel);
			 WriteShort (pCond->TcAscendLevel);
			 break;
		      case TcondAttr:
			 WriteShort (pCond->TcAttr);
			 switch (pSS->SsAttribute[pCond->TcAttr - 1].AttrType)
			       {
				  case AtNumAttr:
				     WriteSignedShort (pCond->TcLowerBound);
				     WriteSignedShort (pCond->TcUpperBound);
				     break;
				  case AtTextAttr:
				     WriteName (pCond->TcTextValue);
				     break;
				  case AtReferenceAttr:

				     break;
				  case AtEnumAttr:
				     WriteShort (pCond->TcAttrValue);
				     break;
				  default:
				     break;
			       }

			 break;
		      case TcondPRule:
			 WriteShort (pCond->TcAttr);
			 if (pCond->TcAttr == PtSize ||
			     pCond->TcAttr == PtIndent ||
			     pCond->TcAttr == PtLineSpacing ||
			     pCond->TcAttr == PtLineWeight ||
			     pCond->TcAttr == PtFillPattern ||
			     pCond->TcAttr == PtBackground ||
			     pCond->TcAttr == PtForeground)
			   {
			      WriteSignedShort (pCond->TcLowerBound);
			      WriteSignedShort (pCond->TcUpperBound);
			   }
			 else
			    TtaWriteByte (outfile, pCond->TcPresValue);
			 break;
		      case TcondElementType:
			 WriteShort (pCond->TcAttr);
			 break;
		      default:
			 break;
		   }
	  }
	WriteTRules (curBlock->TbFirstTRule);
	nextBlock = curBlock->TbNextBlock;
	free (curBlock);
	curBlock = nextBlock;
     }
}


/*----------------------------------------------------------------------
   WriteTRulesAttr  ecrit les regles de traduction de l'attribut	
   de numero att appartenant au schema de structure pSS		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WriteTRulesAttr (int att, PtrSSchema pSS, PtrTSchema pTSch)

#else  /* __STDC__ */
void                WriteTRulesAttr (att, pSS, pTSch)
int                 att;
PtrSSchema          pSS;
PtrTSchema          pTSch;

#endif /* __STDC__ */
{
   AttributeTransl    *pAttrT;
   TranslNumAttrCase  *pCase;
   int                 i;

   pAttrT = &pTSch->TsAttrTRule[att - 1];
   WriteShort (pAttrT->AtrElemType);
   switch (pSS->SsAttribute[att - 1].AttrType)
	 {
	    case AtNumAttr:
	       WriteShort (pAttrT->AtrNCases);
	       for (i = 0; i < pAttrT->AtrNCases; i++)
		 {
		    pCase = &pAttrT->AtrCase[i];
		    WriteSignedShort (pCase->TaLowerBound);
		    WriteSignedShort (pCase->TaUpperBound);
		    WriteBlocks (pCase->TaTRuleBlock, pSS);
		 }
	       break;
	    case AtTextAttr:
	       WriteName (pAttrT->AtrTextValue);
	       WriteBlocks (pAttrT->AtrTxtTRuleBlock, pSS);
	       break;
	    case AtReferenceAttr:
	       WriteBlocks (pAttrT->AtrRefTRuleBlock, pSS);
	       break;
	    case AtEnumAttr:
	       for (i = 0; i <= pSS->SsAttribute[att - 1].AttrNEnumValues; i++)
		  WriteBlocks (pAttrT->AtrEnuTRuleBlock[i], pSS);
	       break;
	    default:
	       break;
	 }
}

/*----------------------------------------------------------------------
   WritePRuleTrans ecrit les regles de traduction de la presentation 
   de numero pres							
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                WritePRuleTrans (int pres, PtrSSchema pSS, PtrTSchema pTSch)

#else  /* __STDC__ */
void                WritePRuleTrans (pres, pSS, pTSch)
int                 pres;
PtrSSchema          pSS;
PtrTSchema          pTSch;

#endif /* __STDC__ */

{
   PRuleTransl        *pPRuleTr;
   TranslNumAttrCase  *pCase;
   int                 i;

   pPRuleTr = &pTSch->TsPresTRule[pres - 1];
   if (pPRuleTr->RtExist)
      if (pres == PtSize + 1 || pres == PtIndent + 1 ||
	  pres == PtLineSpacing + 1 || pres == PtLineWeight + 1 ||
	  pres == PtFillPattern + 1 || pres == PtBackground + 1 ||
	  pres == PtForeground + 1)
	 /* presentation a valeur numerique */
	{
	   WriteShort (pPRuleTr->RtNCase);
	   for (i = 0; i < pPRuleTr->RtNCase; i++)
	     {
		pCase = &pPRuleTr->RtCase[i];
		WriteSignedShort (pCase->TaLowerBound);
		WriteSignedShort (pCase->TaUpperBound);
		WriteBlocks (pCase->TaTRuleBlock, pSS);
	     }
	}
      else
	 for (i = 0; i <= MAX_TRANSL_PRES_VAL; i++)
	   {
	      TtaWriteByte (outfile, pPRuleTr->RtPRuleValue[i]);
	      WriteBlocks (pPRuleTr->RtPRuleValueBlock[i], pSS);
	   }
}


/*----------------------------------------------------------------------
   WriteTranslationSchema cree le fichier de sortie et y ecrit le	
   schema de traduction						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             WriteTranslationSchema (Name fileName, PtrTSchema pTSch, PtrSSchema pSS)

#else  /* __STDC__ */
ThotBool             WriteTranslationSchema (fileName, pTSch, pSS)
Name                fileName;
PtrTSchema          pTSch;
PtrSSchema          pSS;

#endif /* __STDC__ */
{
   TCounter           *pCntr;
   TranslVariable     *pVar;
   TranslVarItem      *pVarItem;
   AttributeTransl    *pAttrTr;
   AlphabetTransl     *pAlphTr;
   StringTransl       *pStrnTr;
   PRuleTransl        *pPruleTr;
   int                 i, j;

   /* cree le fichier */
   outfile = TtaWriteOpen (fileName);
   if (outfile == 0)
      return False;
   /* copie le code d'identification du schema de structure */
   pTSch->TsStructCode = pSS->SsCode;
   /* ecrit la partie fixe du schema */
   WriteName (pTSch->TsStructName);
   WriteShort (pTSch->TsStructCode);
   WriteShort (pTSch->TsLineLength);
   WriteName (pTSch->TsEOL);
   WriteName (pTSch->TsTranslEOL);
   WriteShort (pTSch->TsNConstants);
   WriteShort (pTSch->TsNCounters);
   WriteShort (pTSch->TsNVariables);
   WriteShort (pTSch->TsNBuffers);
   for (i = 0; i < pTSch->TsNConstants; i++)
      WriteShort (pTSch->TsConstBegin[i]);
   for (i = 0; i < pTSch->TsNCounters; i++)
     {
	pCntr = &pTSch->TsCounter[i];
	WriteTCounterOp (pCntr->TnOperation);
	if (pCntr->TnOperation != TCntrNoOp)
	   WriteShort (pCntr->TnElemType1);
	if (pCntr->TnOperation == TCntrSet)
	  {
	     WriteShort (pCntr->TnElemType2);
	     WriteShort (pCntr->TnParam1);
	     WriteShort (pCntr->TnParam2);
	  }
	else if (pCntr->TnOperation == TCntrRank)
	   WriteSignedShort (pCntr->TnAcestorLevel);
	WriteShort (pCntr->TnAttrInit);
     }
   for (i = 0; i < pTSch->TsNVariables; i++)
     {
	pVar = &pTSch->TsVariable[i];
	WriteShort (pVar->TrvNItems);
	for (j = 0; j < pVar->TrvNItems; j++)
	  {
	     pVarItem = &pVar->TrvItem[j];
	     WriteTVarType (pVarItem->TvType);
	     WriteShort (pVarItem->TvItem);
	     if (pVarItem->TvType == VtCounter)
	       {
		  WriteShort (pVarItem->TvLength);
		  WriteCounterStyle (pVarItem->TvCounterStyle);
	       }
	  }
     }
   WriteShort (pTSch->TsPictureBuffer);
   for (i = 0; i < pSS->SsNRules; i++)
      WriteBlockPtr (pTSch->TsElemTRule[i]);
   for (i = 0; i < pSS->SsNRules; i++)
      WriteBoolean (pTSch->TsInheritAttr[i]);
   for (i = 0; i < pSS->SsNAttributes; i++)
     {
	pAttrTr = &pTSch->TsAttrTRule[i];
	switch (pSS->SsAttribute[i].AttrType)
	      {
		 case AtNumAttr:
		    WriteShort (pAttrTr->AtrNCases);
		    for (j = 0; j < pAttrTr->AtrNCases; j++)
		       WriteBlockPtr (pAttrTr->AtrCase[j].TaTRuleBlock);
		    break;
		 case AtTextAttr:
		    WriteBlockPtr (pAttrTr->AtrTxtTRuleBlock);
		    break;
		 case AtReferenceAttr:
		    WriteBlockPtr (pAttrTr->AtrRefTRuleBlock);
		    break;
		 case AtEnumAttr:
		    for (j = 0; j <= pSS->SsAttribute[i].AttrNEnumValues; j++)
		       WriteBlockPtr (pAttrTr->AtrEnuTRuleBlock[j]);
		    break;
		 default:
		    break;
	      }

     }
   for (i = 0; i < MAX_TRANSL_PRULE; i++)
     {
	pPruleTr = &pTSch->TsPresTRule[i];
	WriteBoolean (pPruleTr->RtExist);
	if (pPruleTr->RtExist)
	   if (i == PtSize || i == PtIndent || i == PtLineSpacing ||
	     i == PtLineWeight || i == PtFillPattern || i == PtBackground ||
	       i == PtForeground)
	      /* presentation a valeur numerique */
	     {
		WriteShort (pPruleTr->RtNCase);
		for (j = 0; j < pPruleTr->RtNCase; j++)
		   WriteBlockPtr (pPruleTr->RtCase[j].TaTRuleBlock);
	     }
	   else
	      for (j = 0; j <= MAX_TRANSL_PRES_VAL; j++)
		 WriteBlockPtr (pPruleTr->RtPRuleValueBlock[j]);
     }
   WriteShort (pTSch->TsNTranslAlphabets);
   for (i = 0; i < pTSch->TsNTranslAlphabets; i++)
     {
	pAlphTr = &pTSch->TsTranslAlphabet[i];
	TtaWriteByte (outfile, pAlphTr->AlAlphabet);
	WriteShort (pAlphTr->AlBegin);
	WriteShort (pAlphTr->AlEnd);
     }
   WriteShort (pTSch->TsSymbolFirst);
   WriteShort (pTSch->TsSymbolLast);
   WriteShort (pTSch->TsGraphicsFirst);
   WriteShort (pTSch->TsGraphicsLast);
   WriteShort (pTSch->TsNCharTransls);
   for (i = 0; i < pTSch->TsNCharTransls; i++)
     {
	pStrnTr = &pTSch->TsCharTransl[i];
	j = 0;
	do
	   TtaWriteWideChar (outfile, pStrnTr->StSource[j++], ISO_8859_1);
	while (pStrnTr->StSource[j - 1] != TEXT('\0'));
	j = 0;
	do
	   TtaWriteWideChar (outfile, pStrnTr->StTarget[j++], ISO_8859_1);
	while (pStrnTr->StTarget[j - 1] != TEXT('\0'));
     }
   for (i = 0; i < pTSch->TsNConstants; i++)
     {
	j = pTSch->TsConstBegin[i] - 1;
	do
	   TtaWriteWideChar (outfile, pTSch->TsConstant[j++], ISO_8859_1);
	while (pTSch->TsConstant[j - 1] != TEXT('\0'));
     }
   for (i = 0; i < pSS->SsNRules; i++)
      WriteBlocks (pTSch->TsElemTRule[i], pSS);
   for (i = 0; i < pSS->SsNAttributes; i++)
      WriteTRulesAttr (i + 1, pSS, pTSch);
   for (i = 0; i < MAX_TRANSL_PRULE; i++)
      WritePRuleTrans (i + 1, pSS, pTSch);
   TtaWriteClose (outfile);
   return True;
}
