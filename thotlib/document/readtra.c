/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
   Ce module charge un schema de traduction depuis un fichier .TRA
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"

#define THOT_EXPORT extern
#include "platform_tv.h"

static ThotBool     error = FALSE;


#include "readtra_f.h"
#include "memory_f.h"
#include "fileaccess_f.h"
#include "message_f.h"


/*----------------------------------------------------------------------
   TSchemaError positionne error dans le cas d'une erreur de lecture
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TSchemaError (int err)
#else  /* __STDC__ */
void                TSchemaError (err)
int                 err;
#endif /* __STDC__ */
{
   TtaDisplayMessage (INFO, TEXT("Error %d in translation schema\n"), err);
   error = TRUE;
}


/*----------------------------------------------------------------------
   ReadTransCondition lit un type de condition d'application de	
   regle de traduction.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static TransCondition ReadTransCondition (BinFile file)
#else  /* __STDC__ */
static TransCondition ReadTransCondition (file)
BinFile             file;
#endif /* __STDC__ */
{
   char                c;
   TransCondition      cond;

   cond = TcondFirst;
   if (!TtaReadByte (file, &c))
      TSchemaError (1);
   else
      switch (c)
	    {
	       case C_TR_FIRST:
		  cond = TcondFirst;
		  break;
	       case C_TR_LAST:
		  cond = TcondLast;
		  break;
	       case C_TR_DEFINED:
		  cond = TcondDefined;
		  break;
	       case C_TR_REFERRED:
		  cond = TcondReferred;
		  break;
	       case C_TR_FIRSTREF:
		  cond = TcondFirstRef;
		  break;
	       case C_TR_LAST_REF:
		  cond = TcondLastRef;
		  break;
	       case C_TR_WITHIN:
		  cond = TcondWithin;
		  break;
	       case C_TR_FIRST_WITHIN:
		  cond = TcondFirstWithin;
		  break;
	       case C_TR_ATTRIBUTE:
		  cond = TcondAttr;
		  break;
	       case C_TR_PRESENT:
		  cond = TcondPresentation;
		  break;
	       case C_TR_PRULE:
		  cond = TcondPRule;
		  break;
	       case C_TR_COMMENT:
		  cond = TcondComment;
		  break;
	       case C_TR_ALPHABET:
		  cond = TcondAlphabet;
		  break;
	       case C_TR_ATTRIBUTES:
		  cond = TcondAttributes;
		  break;
	       case C_TR_FIRSTATTR:
		  cond = TcondFirstAttr;
		  break;
	       case C_TR_LASTATTR:
		  cond = TcondLastAttr;
		  break;
	       case C_TR_COMPUTEDPAGE:
		  cond = TcondComputedPage;
		  break;
	       case C_TR_STARTPAGE:
		  cond = TcondStartPage;
		  break;
	       case C_TR_USERPAGE:
		  cond = TcondUserPage;
		  break;
	       case C_TR_REMINDERPAGE:
		  cond = TcondReminderPage;
		  break;
	       case C_TR_EMPTY:
		  cond = TcondEmpty;
		  break;
	       case C_TR_EXTERNALREF:
		  cond = TcondExternalRef;
		  break;
	       case C_TR_ELEMENTTYPE:
		  cond = TcondElementType;
		  break;
	       default:
		  TSchemaError (2);
		  cond = TcondFirst;
		  break;
	    }
   return cond;
}


/*----------------------------------------------------------------------
   ReadTRuleType lit un type de regle de traduction.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static TRuleType    ReadTRuleType (BinFile file)
#else  /* __STDC__ */
static TRuleType    ReadTRuleType (file)
BinFile             file;
#endif /* __STDC__ */
{
   char                c;
   TRuleType           ruleType;

   ruleType = TRemove;
   if (!TtaReadByte (file, &c))
      TSchemaError (3);
   else
      switch (c)
	    {
	       case C_TR_CREATE:
		  ruleType = TCreate;
		  break;
	       case C_TR_GET:
		  ruleType = TGet;
		  break;
	       case C_TR_USE:
		  ruleType = TUse;
		  break;
	       case C_TR_REMOVE:
		  ruleType = TRemove;
		  break;
	       case C_TR_WRITE:
		  ruleType = TWrite;
		  break;
	       case C_TR_READ:
		  ruleType = TRead;
		  break;
	       case C_TR_INCLUDE:
		  ruleType = TInclude;
		  break;
	       case C_TR_NOTRANSL:
		  ruleType = TNoTranslation;
		  break;
	       case C_TR_NOLINEBREAK:
		  ruleType = TNoLineBreak;
		  break;
	       case C_TR_COPY:
		  ruleType = TCopy;
		  break;
	       case C_TR_CHANGEFILE:
		  ruleType = TChangeMainFile;
		  break;
	       case C_TR_SET_COUNTER:
		  ruleType = TSetCounter;
		  break;
	       case C_TR_ADD_COUNTER:
		  ruleType = TAddCounter;
		  break;
	       case C_TR_INDENT:
		  ruleType = TIndent;
		  break;
	       case C_TR_REMOVEFILE:
		  ruleType = TRemoveFile;
		  break;
	       default:
		  TSchemaError (4);
		  ruleType = TRemove;
		  break;
	    }

   return ruleType;
}


/*----------------------------------------------------------------------
   ReadRelatNAscend                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static RelatNAscend ReadRelatNAscend (BinFile file)
#else  /* __STDC__ */
static RelatNAscend ReadRelatNAscend (file)
BinFile             file;
#endif /* __STDC__ */
{
   char                c;
   RelatNAscend        relat;

   relat = RelEquals;		/* valeur par defaut */
   if (!TtaReadByte (file, &c))
      TSchemaError (3);
   else
      switch (c)
	    {
	       case C_WITHIN_GT:
		  relat = RelGreater;
		  break;
	       case C_WITHIN_LT:
		  relat = RelLess;
		  break;
	       case C_WITHIN_EQ:
		  relat = RelEquals;
		  break;
	    }
   return relat;
}


/*----------------------------------------------------------------------
   ReadTOrder lit la position ou` il faut creer les chaines	
   produites par le traducteur.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static TOrder       ReadTOrder (BinFile file)
#else  /* __STDC__ */
static TOrder       ReadTOrder (file)
BinFile             file;
#endif /* __STDC__ */
{
   char                c;
   TOrder              order;

   order = TAfter;
   if (!TtaReadByte (file, &c))
      TSchemaError (5);
   else
      switch (c)
	    {
	       case C_TR_AFTER:
		  order = TAfter;
		  break;
	       case C_TR_BEFORE:
		  order = TBefore;
		  break;
	       default:
		  TSchemaError (6);
		  order = TAfter;
		  break;
	    }

   return order;
}


/*----------------------------------------------------------------------
   ReadCreatedObject lit le type de chose a` creer dans le fichier 
   de sortie.                                  		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CreatedObject ReadCreatedObject (BinFile file)
#else  /* __STDC__ */
static CreatedObject ReadCreatedObject (file)
BinFile             file;
#endif /* __STDC__ */
{
   char                c;
   CreatedObject       obj;

   obj = ToConst;
   if (!TtaReadByte (file, &c))
      TSchemaError (7);
   else
      switch (c)
	    {
	       case C_OB_CONST:
		  obj = ToConst;
		  break;
	       case C_OB_BUFFER:
		  obj = ToBuffer;
		  break;
	       case C_OB_VAR:
		  obj = ToVariable;
		  break;
	       case C_OB_ATTR:
		  obj = ToAttr;
		  break;
	       case C_OB_CONTENT:
		  obj = ToContent;
		  break;
	       case C_OB_COMMENT:
		  obj = ToComment;
		  break;
	       case C_OB_ATTRIBUTES:
		  obj = ToAllAttr;
		  break;
	       case C_OB_PRES_VAL:
		  obj = ToPRuleValue;
		  break;
	       case C_OB_PRESENTATION:
		  obj = ToAllPRules;
		  break;
	       case C_OB_REFID:
		  obj = ToRefId;
		  break;
	       case C_OB_PAIRID:
		  obj = ToPairId;
		  break;
	       case C_OB_REFERRED_ELEM:
		  obj = ToReferredElem;
		  break;
	       case C_OB_FILEDIR:
		  obj = ToFileDir;
		  break;
	       case C_OB_FILENAME:
		  obj = ToFileName;
		  break;
	       case C_OB_EXTENSION:
		  obj = ToExtension;
		  break;
	       case C_OB_DOCUMENTNAME:
		  obj = ToDocumentName;
		  break;
	       case C_OB_DOCUMENTDIR:
		  obj = ToDocumentDir;
		  break;
	       case C_OB_REFERRED_DOCNAME:
		  obj = ToReferredDocumentName;
		  break;
	       case C_OB_REFERRED_DOCDIR:
		  obj = ToReferredDocumentDir;
		  break;
	       case C_OB_REFERRED_REFID:
		  obj = ToReferredRefId;
		  break;
	       case C_OB_TRANSLATED_ATT:
		  obj = ToTranslatedAttr;
		  break;
	       default:
		  TSchemaError (8);
		  obj = ToConst;
		  break;
	    }

   return obj;
}


/*----------------------------------------------------------------------
   ReadTRelatPosition lit la position relative d'un element a` prendre 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static TRelatPosition ReadTRelatPosition (BinFile file)

#else  /* __STDC__ */
static TRelatPosition ReadTRelatPosition (file)
BinFile             file;

#endif /* __STDC__ */

{
   char                c;
   TRelatPosition      position;

   position = RpSibling;
   if (!TtaReadByte (file, &c))
      TSchemaError (9);
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
		  TSchemaError (10);
		  position = RpSibling;
		  break;
	    }
   return position;
}


/*----------------------------------------------------------------------
   ReadIndentType	lit un type de regle d'indentation
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static TIndentType ReadIndentType (BinFile file)

#else  /* __STDC__ */
static TIndentType ReadIndentType (file)
BinFile             file;

#endif /* __STDC__ */

{
   char                c;
   TIndentType		 typ;

   typ = ItAbsolute;
   if (!TtaReadByte (file, &c))
      TSchemaError (23);
   else
      switch (c)
	    {
	       case C_TR_ABSOLUTE:
		  typ = ItAbsolute;
		  break;
	       case C_TR_RELATIVE:
		  typ = ItRelative;
		  break;
	       case C_TR_SUSPEND:
		  typ = ItSuspend;
		  break;
	       case C_TR_RESUME:
		  typ = ItResume;
		  break;
	       default:
		  TSchemaError (24);
		  break;
	    }
   return typ;
}


/*----------------------------------------------------------------------
   ReadTCounterOp lit le type< d'une operation sur un compteur.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static TCounterOp   ReadTCounterOp (BinFile file)
#else  /* __STDC__ */
static TCounterOp   ReadTCounterOp (file)
BinFile             file;
#endif /* __STDC__ */

{
   char                c;
   TCounterOp          op;

   op = TCntrNoOp;
   if (!TtaReadByte (file, &c))
      TSchemaError (11);
   else
      switch (c)
	    {
	       case C_TR_RANK:
		  op = TCntrRank;
		  break;
	       case C_TR_RLEVEL:
		  op = TCntrRLevel;
		  break;
	       case C_TR_SET:
		  op = TCntrSet;
		  break;
	       case C_TR_NOOP:
		  op = TCntrNoOp;
		  break;
	       default:
		  TSchemaError (12);
		  op = TCntrNoOp;
		  break;
	    }
   return op;
}


/*----------------------------------------------------------------------
   ReadTranslVarType lit le type des elements de variables de      
   traduction.                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static TranslVarType ReadTranslVarType (BinFile file)
#else  /* __STDC__ */
static TranslVarType ReadTranslVarType (file)
BinFile             file;
#endif /* __STDC__ */
{
   char                c;
   TranslVarType       varType;

   varType = VtText;
   if (!TtaReadByte (file, &c))
      TSchemaError (13);
   else
      switch (c)
	    {
	       case C_TR_CONST:
		  varType = VtText;
		  break;
	       case C_TR_COUNTER:
		  varType = VtCounter;
		  break;
	       case C_TR_BUFFER:
		  varType = VtBuffer;
		  break;
	       case C_TR_ATTR:
		  varType = VtAttrVal;
		  break;
	       case C_TR_FILEDIR:
		  varType = VtFileDir;
		  break;
	       case C_TR_FILENAME:
		  varType = VtFileName;
		  break;
	       case C_TR_EXTENSION:
		  varType = VtExtension;
		  break;
	       case C_TR_DOCUMENTNAME:
		  varType = VtDocumentName;
		  break;
	       case C_TR_DOCUMENTDIR:
		  varType = VtDocumentDir;
		  break;
	       default:
		  TSchemaError (14);
		  varType = VtText;
		  break;
	    }

   return varType;
}


/*----------------------------------------------------------------------
   ReadCounterStyle  lit un style de compteur dans le fichier et   
   retourne sa valeur.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static CounterStyle ReadCounterStyle (BinFile file)
#else  /* __STDC__ */
static CounterStyle ReadCounterStyle (file)
BinFile             file;
#endif /* __STDC__ */
{
   char                c;
   CounterStyle        style;


   if (!TtaReadByte (file, &c))
     {
	c = SPACE;
	TSchemaError (15);
     }
   switch (c)
	 {
	    case C_NUM_ARABIC:
	       style = CntArabic;
	       break;
	    case C_NUM_ROMAN:
	       style = CntURoman;
	       break;
	    case C_NUM_LOWER_ROMAN:
	       style = CntLRoman;
	       break;
	    case C_NUM_UPPERCASE:
	       style = CntUppercase;
	       break;
	    case C_NUM_LOWERCASE:
	       style = CntLowercase;
	       break;
	    default:
	       TSchemaError (16);
	       style = CntArabic;
	       break;
	 }
   return style;
}

/*----------------------------------------------------------------------
   ReadPtrTRule retourne un pointeur sur la regle suivante ou NULL   
   s'il n'y a pas de regle suivante.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrTRule     ReadPtrTRule (BinFile file, PtrTRule * pNextTRule)
#else  /* __STDC__ */
static PtrTRule     ReadPtrTRule (file, pNextTRule)
BinFile             file;
PtrTRule           *pNextTRule;
#endif /* __STDC__ */
{
   char                c;
   PtrTRule            pTRule;

   if (!TtaReadByte (file, &c))
      TSchemaError (17);
   if (c == EOS)
      pTRule = NULL;
   else
      pTRule = *pNextTRule;
   return pTRule;
}


/*----------------------------------------------------------------------
   ReadTRules ecrit une suite de regles chainees et fait pointer	
   pFirstTRule sur la premiere regle de la suite de regles lues.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReadTRules (BinFile file, PtrTRule * pFirstTRule, PtrTRule * pNextTRule)
#else  /* __STDC__ */
static void         ReadTRules (file, pFirstTRule, pNextTRule)
BinFile             file;
PtrTRule           *pFirstTRule;
PtrTRule           *pNextTRule;
#endif /* __STDC__ */
{
   PtrTRule            pTRule;

   *pFirstTRule = *pNextTRule;
   /* lecture de la suite de regles */
   if (!error)
      do
	{
	   pTRule = *pNextTRule;
	   /* acquiert un buffer pour la regle suivante */
	   GetTRule (pNextTRule);
	   if (*pNextTRule == NULL)
	      TSchemaError (18);
	   /* lit une regle */
	   pTRule->TrNextTRule = ReadPtrTRule (file, pNextTRule);
	   pTRule->TrOrder = ReadTOrder (file);
	   pTRule->TrType = ReadTRuleType (file);
	   if (!error)
	      switch (pTRule->TrType)
		    {
		       case TCreate:
		       case TWrite:
			  pTRule->TrObject = ReadCreatedObject (file);
			  TtaReadShort (file, &pTRule->TrObjectNum);
			  TtaReadName (file, pTRule->TrObjectNature);
			  TtaReadBool (file, &pTRule->TrReferredObj);
			  TtaReadShort (file, &pTRule->TrFileNameVar);
			  break;
		       case TGet:
		       case TCopy:
			  TtaReadShort (file, &pTRule->TrElemType);
			  TtaReadName (file, pTRule->TrElemNature);
			  pTRule->TrRelPosition = ReadTRelatPosition (file);
			  break;
		       case TUse:
			  TtaReadName (file, pTRule->TrNature);
			  TtaReadName (file, pTRule->TrTranslSchemaName);
			  break;
		       case TRemove: 
		       case TNoTranslation:
		       case TNoLineBreak:

			  break;
		       case TRead:
			  TtaReadShort (file, &pTRule->TrBuffer);
			  break;
		       case TInclude:
			  pTRule->TrBufOrConst = ReadCreatedObject (file);
			  TtaReadShort (file, &pTRule->TrInclFile);
			  break;
		       case TChangeMainFile:
		       case TRemoveFile:
			  TtaReadShort (file, &pTRule->TrNewFileVar);
			  break;
		       case TSetCounter:
		       case TAddCounter:
			  TtaReadShort (file, &pTRule->TrCounterNum);
			  TtaReadShort (file, &pTRule->TrCounterParam);
			  break;
		       case TIndent:
			  TtaReadShort (file, &pTRule->TrIndentFileNameVar);
			  TtaReadSignedShort (file, &pTRule->TrIndentVal);
			  pTRule->TrIndentType = ReadIndentType (file);
			  break;
		       default:
			  break;
		    }
	   /* passe a la regle suivante */
	   if (pTRule->TrNextTRule != NULL)
	      pTRule->TrNextTRule = *pNextTRule;
	}
      while (pTRule->TrNextTRule != NULL && !error);
}


/*----------------------------------------------------------------------
   ReadPtrTRuleBlock retourne un pointeur sur le bloc de regles	
   	suivant ou NULL s'il n'y a pas de bloc suivant.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrTRuleBlock ReadPtrTRuleBlock (BinFile file, PtrTRuleBlock * pNextBlock)
#else  /* __STDC__ */
static PtrTRuleBlock ReadPtrTRuleBlock (file, pNextBlock)
BinFile             file;
PtrTRuleBlock      *pNextBlock;
#endif /* __STDC__ */
{
   char c;

   TtaReadByte (file, &c);
   if (c == EOS)
      return NULL;
   else
      return (*pNextBlock);
}


/*----------------------------------------------------------------------
   ReadBlocks lit une suite de blocs de regles.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReadBlocks (BinFile file, PtrTRuleBlock * pBlock, PtrTRule * pNextTRule, PtrSSchema * pSS, PtrTRuleBlock * pNextBlock)
#else  /* __STDC__ */
static void         ReadBlocks (file, pBlock, pNextTRule, pSS, pNextBlock)
BinFile             file;
PtrTRuleBlock      *pBlock;
PtrTRule           *pNextTRule;
PtrSSchema         *pSS;
PtrTRuleBlock      *pNextBlock;

#endif /* __STDC__ */

{
   PtrTRuleBlock       pBl;
   TranslCondition    *pCond;
   int                 cond;

   if (*pBlock != NULL && !error)
      /* pointeur sur le premier bloc qui va etre lu */
     {
	*pBlock = *pNextBlock;
	/* lecture de la suite des blocs */
	do
	  {
	     pBl = *pNextBlock;
	     /* acquiert un buffer pour le bloc suivant */
	     if ((*pNextBlock = (PtrTRuleBlock) TtaGetMemory (sizeof (TRuleBlock))) == NULL)
		TSchemaError (19);
	     /* lit un bloc */
	     pBl->TbNextBlock = ReadPtrTRuleBlock (file, pNextBlock);
	     TtaReadShort (file, &pBl->TbNConditions);
	     for (cond = 0; cond < pBl->TbNConditions; cond++)
	       {
		  pCond = &pBl->TbCondition[cond];
		  pCond->TcCondition = ReadTransCondition (file);
		  TtaReadBool (file, &pCond->TcNegativeCond);
		  TtaReadBool (file, &pCond->TcTarget);
		  TtaReadShort (file, &pCond->TcAscendType);
		  TtaReadName (file, pCond->TcAscendNature);
		  TtaReadSignedShort (file, &pCond->TcAscendRelLevel);
		  switch (pCond->TcCondition)
			{
			   case TcondAlphabet:
			      TtaReadByte (file, &pCond->TcAlphabet);
			      break;
			   case TcondWithin:
			   case TcondFirstWithin:
			      TtaReadShort (file, &pCond->TcElemType);
			      TtaReadName (file, pCond->TcElemNature);
			      TtaReadBool (file, &pCond->TcImmediatelyWithin);
			      pCond->TcAscendRel = ReadRelatNAscend (file);
			      TtaReadShort (file, &pCond->TcAscendLevel);
			      break;
			   case TcondAttr:
			      TtaReadShort (file, &pCond->TcAttr);
			      if (!error)
				 switch ((*pSS)->SsAttribute[pCond->TcAttr - 1].AttrType)
				       {
					  case AtNumAttr:
					     TtaReadSignedShort (file, &pCond->TcLowerBound);
					     TtaReadSignedShort (file, &pCond->TcUpperBound);
					     break;
					  case AtTextAttr:
					     TtaReadName (file, pCond->TcTextValue);
					     break;
					  case AtReferenceAttr:

					     break;
					  case AtEnumAttr:
					     TtaReadShort (file, &pCond->TcAttrValue);
					     break;
					  default:
					     break;
				       }

			      break;
			   case TcondPRule:
			      TtaReadShort (file, &pCond->TcAttr);
			      if (!error)
				 if (pCond->TcAttr == PtSize ||
				     pCond->TcAttr == PtIndent ||
				     pCond->TcAttr == PtLineSpacing ||
				     pCond->TcAttr == PtLineWeight ||
				     pCond->TcAttr == PtFillPattern ||
				     pCond->TcAttr == PtBackground ||
				     pCond->TcAttr == PtForeground)
				   {
				      TtaReadSignedShort (file, &pCond->TcLowerBound);
				      TtaReadSignedShort (file, &pCond->TcUpperBound);
				   }
				 else
				    TtaReadByte (file, &pCond->TcPresValue);
			      break;
			   case TcondElementType:
			      TtaReadShort (file, &pCond->TcAttr);
			      break;
			   default:
			      break;
			}
	       }
	     ReadTRules (file, &pBl->TbFirstTRule, pNextTRule);
	     /* passe a la regle suivante */
	     if (pBl->TbNextBlock != NULL)
		pBl->TbNextBlock = *pNextBlock;
	  }
	while (pBl->TbNextBlock != NULL && !error);
     }
}


/*----------------------------------------------------------------------
   FreeBlocks     libere une suite de blocs de regles ainsi que    
   les regles attachees.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreeBlocks (PtrTRuleBlock pBlock)
#else  /* __STDC__ */
static void         FreeBlocks (pBlock)
PtrTRuleBlock       pBlock;
#endif /* __STDC__ */
{
   PtrTRuleBlock       pNextBlock;
   PtrTRule            pTRule, nextRule;

   while (pBlock != NULL)
     {
	pNextBlock = pBlock->TbNextBlock;
	pTRule = pBlock->TbFirstTRule;
	pBlock->TbFirstTRule = NULL;
	while (pTRule != NULL)
	  {
	    nextRule = pTRule->TrNextTRule;
	    FreeTRule (pTRule);
	    pTRule = nextRule;
	  }
	TtaFreeMemory (pBlock);
	pBlock = pNextBlock;
     }
}


/*----------------------------------------------------------------------
   ReadAttrTRules lit les regles de traduction de l'attribut de    
   numero att appartenant au schema de structure pSS.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReadAttrTRules (BinFile file, int att, PtrTRuleBlock * pNextBlock, PtrTRule * pNextTRule, PtrSSchema * pSS, PtrTSchema * pTSch)
#else  /* __STDC__ */
static void         ReadAttrTRules (file, att, pNextBlock, pNextTRule, pSS, pTSch)
BinFile             file;
int                 att;
PtrTRuleBlock      *pNextBlock;
PtrTRule           *pNextTRule;
PtrSSchema         *pSS;
PtrTSchema         *pTSch;
#endif /* __STDC__ */
{
   AttributeTransl    *pAttrT;
   TranslNumAttrCase  *pCase;
   int                 i;

   if (!error)
     {
	pAttrT = &(*pTSch)->TsAttrTRule[att];
	TtaReadShort (file, &pAttrT->AtrElemType);
	switch ((*pSS)->SsAttribute[att].AttrType)
	      {
		 case AtNumAttr:
		    TtaReadShort (file, &pAttrT->AtrNCases);
		    for (i = 0; i < pAttrT->AtrNCases; i++)
		       if (!error)
			 {
			    pCase = &pAttrT->AtrCase[i];
			    TtaReadSignedShort (file, &pCase->TaLowerBound);
			    TtaReadSignedShort (file, &pCase->TaUpperBound);
			    ReadBlocks (file, &pCase->TaTRuleBlock, pNextTRule,
					pSS, pNextBlock);
			 }
		    break;
		 case AtTextAttr:
		    TtaReadName (file, pAttrT->AtrTextValue);
		    ReadBlocks (file, &pAttrT->AtrTxtTRuleBlock, pNextTRule,
				pSS, pNextBlock);
		    break;
		 case AtReferenceAttr:
		    ReadBlocks (file, &pAttrT->AtrRefTRuleBlock, pNextTRule,
				pSS, pNextBlock);
		    break;
		 case AtEnumAttr:
		    for (i = 0; i <= (*pSS)->SsAttribute[att].AttrNEnumValues; i++)
		       ReadBlocks (file, &pAttrT->AtrEnuTRuleBlock[i],
				   pNextTRule, pSS, pNextBlock);
		    break;
		 default:
		    break;
	      }
     }
}

/*----------------------------------------------------------------------
   FreeTRulesAttr libere les regles de traduction d'un attribut.   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreeTRulesAttr (AttributeTransl * pAttrT, TtAttribute * pAttr)
#else  /* __STDC__ */
static void         FreeTRulesAttr (pAttrT, pAttr)
AttributeTransl    *pAttrT;
TtAttribute        *pAttr;
#endif /* __STDC__ */
{
   int                 i;

   switch (pAttr->AttrType)
	 {
	    case AtNumAttr:
	       for (i = 0; i < pAttrT->AtrNCases; i++)
		  FreeBlocks (pAttrT->AtrCase[i].TaTRuleBlock);
	       break;
	    case AtTextAttr:
	       FreeBlocks (pAttrT->AtrTxtTRuleBlock);
	       break;
	    case AtReferenceAttr:
	       FreeBlocks (pAttrT->AtrRefTRuleBlock);
	       break;
	    case AtEnumAttr:
	       for (i = 0; i <= pAttr->AttrNEnumValues; i++)
		  FreeBlocks (pAttrT->AtrEnuTRuleBlock[i]);
	       break;
	    default:
	       break;
	 }
}

/*----------------------------------------------------------------------
   ReadPresTRules lit les regles de traduction de la presentation  
   de numero pres appartenant au schema de structure pSS.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ReadPresTRules (BinFile file, int pres, PtrTRuleBlock * pNextBlock, PtrTRule * pNextTRule, PtrSSchema * pSS, PtrTSchema * pTSch)
#else  /* __STDC__ */
static void         ReadPresTRules (file, pres, pNextBlock, pNextTRule, pSS, pTSch)
BinFile             file;
int                 pres;
PtrTRuleBlock      *pNextBlock;
PtrTRule           *pNextTRule;
PtrSSchema         *pSS;
PtrTSchema         *pTSch;
#endif /* __STDC__ */
{
   PRuleTransl        *pPruleTr;
   TranslNumAttrCase  *pCase;
   int                 i;

   if (!error)
     {
	pPruleTr = &(*pTSch)->TsPresTRule[pres];
	if (pPruleTr->RtExist)
	   if (pres == PtSize || pres == PtIndent ||
	       pres == PtLineSpacing || pres == PtLineWeight ||
	       pres == PtFillPattern || pres == PtBackground ||
	       pres == PtForeground)
	      /* presentation a valeur numerique */
	     {
		TtaReadShort (file, &pPruleTr->RtNCase);
		for (i = 0; i < pPruleTr->RtNCase; i++)
		   if (!error)
		     {
			pCase = &pPruleTr->RtCase[i];
			TtaReadSignedShort (file, &pCase->TaLowerBound);
			TtaReadSignedShort (file, &pCase->TaUpperBound);
			ReadBlocks (file, &pCase->TaTRuleBlock, pNextTRule,
				    pSS, pNextBlock);
		     }
	     }
	   else
	      for (i = 0; i <= MAX_TRANSL_PRES_VAL; i++)
		{
		   TtaReadByte (file, &pPruleTr->RtPRuleValue[i]);
		   ReadBlocks (file, &pPruleTr->RtPRuleValueBlock[i],
			       pNextTRule, pSS, pNextBlock);
		}
     }
}


/*----------------------------------------------------------------------
   FreeTRulesPres libere les regles de traduction d'une regle de	
   presentation                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreeTRulesPres (int pres, PRuleTransl * pPruleTr)
#else  /* __STDC__ */
static void         FreeTRulesPres (pres, pPruleTr)
int                 pres;
PRuleTransl        *pPruleTr;
#endif /* __STDC__ */
{
   int                 i;

   if (pPruleTr->RtExist)
      if (pres == PtSize + 1 || pres == PtIndent + 1 ||
	  pres == PtLineSpacing + 1 || pres == PtLineWeight + 1 ||
	  pres == PtFillPattern + 1 || pres == PtBackground + 1 ||
	  pres == PtForeground + 1)
	 /* presentation a valeur numerique */
	 for (i = 0; i < pPruleTr->RtNCase; i++)
	    FreeBlocks (pPruleTr->RtCase[i].TaTRuleBlock);
      else
	 for (i = 0; i <= MAX_TRANSL_PRES_VAL; i++)
	    FreeBlocks (pPruleTr->RtPRuleValueBlock[i]);
}


/*----------------------------------------------------------------------
   ReadTranslationSchema						
   		lit un fichier contenant un schema de traduction et     
   le charge en memoire. Retourne un pointeur sur le       
   schema de presentation en memoire si chargement reussi, 
   NULL si echec.                                          
   - fileName: nom du fichier a lire, sans le suffixe .TRA 
   - pSS: schema de structure correspondant, deja rempli.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrTSchema          ReadTranslationSchema (Name fileName, PtrSSchema pSS)
#else  /* __STDC__ */
PtrTSchema          ReadTranslationSchema (fileName, pSS)
Name                fileName;
PtrSSchema          pSS;
#endif /* __STDC__ */
{
   BinFile             file;
   PtrTRule            pNextTRule;
   PtrTRuleBlock       pNextBlock;
   PtrTSchema          pTSch;
   TCounter           *pCntr;
   TranslVariable     *pVar;
   TranslVarItem      *pVarItem;
   AttributeTransl    *pAttrTr;
   AlphabetTransl     *pAlphTr;
   StringTransl       *pStringTr;
   PRuleTransl        *pPRuleTr;
   PathBuffer          dirBuffer;
   CHAR_T              buf[MAX_TXT_LEN];
   int                 InitialNElems, i, j;

   error = FALSE;
   pTSch = NULL;
   /* compose le nom du fichier a ouvrir */
   ustrncpy (dirBuffer, SchemaPath, MAX_PATH);
   MakeCompleteName (fileName, TEXT("TRA"), dirBuffer, buf, &i);

   /* ouvre le fichier */
   file = TtaReadOpen (buf);
   if (file == 0)
     {
	ustrncpy (buf, fileName, MAX_PATH);
	ustrcat (buf, TEXT(".TRA"));
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_MISSING_FILE), buf);
     }
   else
     {
        GetSchTra (&pTSch);
	if (pTSch == NULL)
	  {
	     TSchemaError (20);
	     return NULL;
	  }
	GetTRule (&pNextTRule);
	if (pNextTRule == NULL)
	  {
	     TSchemaError (21);
	     return NULL;
	  }
	if ((pNextBlock = (PtrTRuleBlock) TtaGetMemory (sizeof (TRuleBlock))) == NULL)
	  {
	     TSchemaError (22);
	     return NULL;
	  }
	else
	  memset (pNextBlock, 0, sizeof (TRuleBlock));

	/* lit la partie fixe du schema de traduction */
	TtaReadName (file, pTSch->TsStructName);
	TtaReadShort (file, &pTSch->TsStructCode);
	TtaReadShort (file, &pTSch->TsLineLength);
	TtaReadName (file, pTSch->TsEOL);
	TtaReadName (file, pTSch->TsTranslEOL);
	TtaReadShort (file, &pTSch->TsNConstants);
	TtaReadShort (file, &pTSch->TsNCounters);
	TtaReadShort (file, &pTSch->TsNVariables);
	TtaReadShort (file, &pTSch->TsNBuffers);
	if (!error)
	   for (i = 0; i < pTSch->TsNConstants; i++)
	      TtaReadShort (file, &pTSch->TsConstBegin[i]);
	if (!error)
	   for (i = 0; i < pTSch->TsNCounters; i++)
	     {
		pCntr = &pTSch->TsCounter[i];
		pCntr->TnOperation = ReadTCounterOp (file);
		if (pCntr->TnOperation == TCntrNoOp)
		   pCntr->TnParam1 = 0;
		else
		   TtaReadShort (file, &pCntr->TnElemType1);
		if (pCntr->TnOperation == TCntrSet)
		  {
		     TtaReadShort (file, &pCntr->TnElemType2);
		     TtaReadShort (file, &pCntr->TnParam1);
		     TtaReadShort (file, &pCntr->TnParam2);
		  }
		else if (pCntr->TnOperation == TCntrRank)
		   TtaReadSignedShort (file, &pCntr->TnAcestorLevel);
		TtaReadShort (file, &pCntr->TnAttrInit);
	     }
	if (!error)
	   for (i = 0; i < pTSch->TsNVariables; i++)
	     {
		pVar = &pTSch->TsVariable[i];
		TtaReadShort (file, &pVar->TrvNItems);
		if (!error)
		   for (j = 0; j < pVar->TrvNItems; j++)
		     {
			pVarItem = &pVar->TrvItem[j];
			pVarItem->TvType = ReadTranslVarType (file);
			TtaReadShort (file, &pVarItem->TvItem);
			if (pVarItem->TvType == VtCounter)
			  {
			     TtaReadShort (file, &pVarItem->TvLength);
			     pVarItem->TvCounterStyle = ReadCounterStyle (file);
			  }
		     }
	     }
	TtaReadShort (file, &pTSch->TsPictureBuffer);
	for (i = 0; i < MAX_TRANSL_BUFFER; i++)
	   pTSch->TsBuffer[i][0] = EOS;
	if (pSS->SsFirstDynNature == 0)
	   InitialNElems = pSS->SsNRules;
	else
	  {
	     InitialNElems = pSS->SsFirstDynNature - 1;
	     for (j = InitialNElems; j < pSS->SsNRules; j++)
		pTSch->TsElemTRule[j] = NULL;
	  }
	if (!error)
	   for (i = 0; i < InitialNElems; i++)
	      pTSch->TsElemTRule[i] = ReadPtrTRuleBlock (file, &pNextBlock);
	if (!error)
	   for (i = 0; i < InitialNElems; i++)
	      TtaReadBool (file, &pTSch->TsInheritAttr[i]);
	if (!error)
	   for (i = 0; i < pSS->SsNAttributes; i++)
	      if (!error)
		{
		   pAttrTr = &pTSch->TsAttrTRule[i];
		   switch (pSS->SsAttribute[i].AttrType)
			 {
			    case AtNumAttr:
			       TtaReadShort (file, &pAttrTr->AtrNCases);
			       if (!error)
				  for (j = 0; j < pAttrTr->AtrNCases; j++)
				     pAttrTr->AtrCase[j].TaTRuleBlock =
					ReadPtrTRuleBlock (file, &pNextBlock);
			       break;
			    case AtTextAttr:
			       pAttrTr->AtrTxtTRuleBlock =
				  ReadPtrTRuleBlock (file, &pNextBlock);
			       break;
			    case AtReferenceAttr:
			       pAttrTr->AtrRefTRuleBlock =
				  ReadPtrTRuleBlock (file, &pNextBlock);
			       break;
			    case AtEnumAttr:
			       for (j = 0; j <= pSS->SsAttribute[i].AttrNEnumValues; j++)
				  pAttrTr->AtrEnuTRuleBlock[j] =
				     ReadPtrTRuleBlock (file, &pNextBlock);
			       break;
			    default:
			       break;
			 }
		}
	if (!error)
	   for (i = 0; i < MAX_TRANSL_PRULE; i++)
	      if (!error)
		{
		   pPRuleTr = &pTSch->TsPresTRule[i];
		   TtaReadBool (file, &pPRuleTr->RtExist);
		   if (pPRuleTr->RtExist)
		      if (i == PtSize || i == PtIndent || i == PtLineSpacing ||
			  i == PtLineWeight || i == PtFillPattern ||
			  i == PtBackground || i == PtForeground)
			 /* presentation a valeur numerique */
			{
			   TtaReadShort (file, &pPRuleTr->RtNCase);
			   if (!error)
			      for (j = 0; j < pPRuleTr->RtNCase; j++)
				 pPRuleTr->RtCase[j].TaTRuleBlock =
				    ReadPtrTRuleBlock (file, &pNextBlock);
			}
		      else
			 for (j = 0; j <= MAX_TRANSL_PRES_VAL; j++)
			    pPRuleTr->RtPRuleValueBlock[j] =
			       ReadPtrTRuleBlock (file, &pNextBlock);
		}
	TtaReadShort (file, &pTSch->TsNTranslAlphabets);
	if (!error)
	   for (i = 0; i < pTSch->TsNTranslAlphabets; i++)
	     {
		pAlphTr = &pTSch->TsTranslAlphabet[i];
		TtaReadByte (file, &pAlphTr->AlAlphabet);
		TtaReadShort (file, &pAlphTr->AlBegin);
		TtaReadShort (file, &pAlphTr->AlEnd);
	     }
	TtaReadShort (file, &pTSch->TsSymbolFirst);
	TtaReadShort (file, &pTSch->TsSymbolLast);
	TtaReadShort (file, &pTSch->TsGraphicsFirst);
	TtaReadShort (file, &pTSch->TsGraphicsLast);
	TtaReadShort (file, &pTSch->TsNCharTransls);
	/* lit les tables de traduction de caracteres */
	if (!error)
	   for (i = 0; i < pTSch->TsNCharTransls; i++)
	     {
		pStringTr = &pTSch->TsCharTransl[i];
		/* lit la chaine source */
		j = 0;
		do
		   TtaReadWideChar (file, &pStringTr->StSource[j++], ISO_8859_1);
		while (pStringTr->StSource[j - 1] != WC_EOS);
		/* lit la chaine cible */
		j = 0;
		do
		   TtaReadWideChar (file, &pStringTr->StTarget[j++], ISO_8859_1);
		while (pStringTr->StTarget[j - 1] != WC_EOS);
	     }
	if (!error)
	   /* lit les constantes */
	   for (i = 0; i < pTSch->TsNConstants; i++)
	     {
		j = pTSch->TsConstBegin[i] - 1;
		do
		   TtaReadWideChar (file, &pTSch->TsConstant[j++], ISO_8859_1);
		while (pTSch->TsConstant[j - 1] != WC_EOS);
	     }
	/* lit les blocs de regles des elements */
	if (!error)
	   for (i = 0; i < InitialNElems; i++)
	      ReadBlocks (file, &pTSch->TsElemTRule[i], &pNextTRule, &pSS,
			  &pNextBlock);
	/* lit les blocs de regles des attributs */
	for (i = 0; i < pSS->SsNAttributes; i++)
	   if (!error)
	      ReadAttrTRules (file, i, &pNextBlock, &pNextTRule, &pSS,
			      &pTSch);
	/* lit les blocs de regles des presentation */
	for (i = 0; i < MAX_TRANSL_PRULE; i++)
	   if (!error)
	      ReadPresTRules (file, i, &pNextBlock, &pNextTRule, &pSS,
			      &pTSch);
	TtaFreeMemory ( pNextTRule);
	TtaFreeMemory ( pNextBlock);

	/* ferme le fichier */
	TtaReadClose (file);
     }
   if (error)
     {
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_TRA_FILE_INCORRECT),
			   fileName);
	if (pTSch != NULL)
	   FreeTranslationSchema (pTSch, pSS);
	pTSch = NULL;
     }
   return pTSch;
}

/*----------------------------------------------------------------------
   FreeTranslationSchema   libere le schema de traduction pTSch    
   correspondant au schema de structure pSS.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeTranslationSchema (PtrTSchema pTSch, PtrSSchema pSS)

#else  /* __STDC__ */
void                FreeTranslationSchema (pTSch, pSS)
PtrTSchema          pTSch;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   int                 InitialNElems;
   int                 i;

   if (pSS->SsFirstDynNature == 0)
      InitialNElems = pSS->SsNRules;
   else
      InitialNElems = pSS->SsFirstDynNature - 1;
   /* libere les blocs de regles  des elements */
   for (i = 0; i < InitialNElems; i++)
      FreeBlocks (pTSch->TsElemTRule[i]);
   /* libere les blocs de regles des attributs */
   for (i = 0; i < pSS->SsNAttributes; i++)
      FreeTRulesAttr (&pTSch->TsAttrTRule[i], &pSS->SsAttribute[i]);
   /* libere les blocs de regles des presentations */
   for (i = 0; i < MAX_TRANSL_PRULE; i++)
      FreeTRulesPres (i + 1, &pTSch->TsPresTRule[i]);
   /* libere le schema de traduction lui-meme */
   TtaFreeMemory ( pTSch);
}
