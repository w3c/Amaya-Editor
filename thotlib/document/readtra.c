/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
  Ce module charge un schema de traduction depuis un fichier .TRA
*/
#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "zlib.h"
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
void TSchemaError (int err)
{
  TtaDisplayMessage (INFO, "Error %d in translation schema\n", err);
  error = TRUE;
}


/*----------------------------------------------------------------------
  ReadTransCondition lit un type de condition d'application de	
  regle de traduction.					
  ----------------------------------------------------------------------*/
static TransCondition ReadTransCondition (BinFile file)
{
  char                c;
  TransCondition      cond;

  cond = TcondFirst;
  if (!TtaReadByte (file, (unsigned char *)&c))
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
      case C_TR_ALPHABET:
        cond = TcondScript;
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
      case C_TR_ROOT:
        cond = TcondRoot;
        break;
      case C_TR_EXTERNALREF:
        cond = TcondExternalRef;
        break;
      case C_TR_ELEMENTTYPE:
        cond = TcondElementType;
        break;
      case C_TR_TRANSCLUSION:
        cond = TcondTransclusion;
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
static TRuleType    ReadTRuleType (BinFile file)
{
  char                c;
  TRuleType           ruleType;

  ruleType = TRemove;
  if (!TtaReadByte (file, (unsigned char *)&c))
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
      case C_TR_IGNORE:
        ruleType = TIgnore;
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
static RelatNAscend ReadRelatNAscend (BinFile file)
{
  char                c;
  RelatNAscend        relat;

  relat = RelEquals;		/* valeur par defaut */
  if (!TtaReadByte (file, (unsigned char *)&c))
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
static TOrder       ReadTOrder (BinFile file)
{
  char                c;
  TOrder              order;

  order = TAfter;
  if (!TtaReadByte (file, (unsigned char *)&c))
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
static CreatedObject ReadCreatedObject (BinFile file)
{
  char                c;
  CreatedObject       obj;

  obj = ToConst;
  if (!TtaReadByte (file, (unsigned char *)&c))
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
static TRelatPosition ReadTRelatPosition (BinFile file)
{
  char                c;
  TRelatPosition      position;

  position = RpSibling;
  if (!TtaReadByte (file, (unsigned char *)&c))
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
static TIndentType ReadIndentType (BinFile file)
{
  char                c;
  TIndentType		 typ;

  typ = ItAbsolute;
  if (!TtaReadByte (file, (unsigned char *)&c))
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
static TCounterOp   ReadTCounterOp (BinFile file)
{
  char                c;
  TCounterOp          op;

  op = TCntrNoOp;
  if (!TtaReadByte (file, (unsigned char *)&c))
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
static TranslVarType ReadTranslVarType (BinFile file)
{
  char                c;
  TranslVarType       varType;

  varType = VtText;
  if (!TtaReadByte (file, (unsigned char *)&c))
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
static CounterStyle ReadCounterStyle (BinFile file)
{
  char                c;
  CounterStyle        style;


  if (!TtaReadByte (file, (unsigned char *)&c))
    {
      c = SPACE;
      TSchemaError (15);
    }
  switch (c)
    {
    case C_NUM_DECIMAL:
      style = CntDecimal;
      break;
    case C_NUM_ZLDECIMAL:
      style = CntZLDecimal;
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
    case C_NUM_GREEK:
      style = CntLGreek;
      break;
    case C_NUM_UPPER_GREEK:
      style = CntUGreek;
      break;
    default:
      TSchemaError (16);
      style = CntDecimal;
      break;
    }
  return style;
}

/*----------------------------------------------------------------------
  ReadPtrTRule retourne un pointeur sur la regle suivante ou NULL   
  s'il n'y a pas de regle suivante.                       
  ----------------------------------------------------------------------*/
static PtrTRule ReadPtrTRule (BinFile file, PtrTRule *pNextTRule)
{
  char                c;
  PtrTRule            pTRule;

  if (!TtaReadByte (file, (unsigned char *)&c))
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
static void ReadTRules (BinFile file, PtrTRule *pFirstTRule, PtrTRule *pNextTRule)
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
              TtaReadName (file, (unsigned char *)pTRule->TrObjectNature);
              TtaReadBool (file, &pTRule->TrReferredObj);
              TtaReadShort (file, &pTRule->TrFileNameVar);
              break;
            case TGet:
            case TCopy:
              TtaReadShort (file, &pTRule->TrElemType);
              TtaReadName (file, (unsigned char *)pTRule->TrElemNature);
              pTRule->TrRelPosition = ReadTRelatPosition (file);
              break;
            case TUse:
              TtaReadName (file, (unsigned char *)pTRule->TrNature);
              TtaReadName (file, (unsigned char *)pTRule->TrTranslSchemaName);
              break;
            case TRemove:
            case TIgnore:
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
static PtrTRuleBlock ReadPtrTRuleBlock (BinFile file, PtrTRuleBlock *pNextBlock)
{
  char c;

  TtaReadByte (file, (unsigned char *)&c);
  if (c == EOS)
    return NULL;
  else
    return (*pNextBlock);
}


/*----------------------------------------------------------------------
  ReadBlocks lit une suite de blocs de regles.                    
  ----------------------------------------------------------------------*/
static void ReadBlocks (BinFile file, PtrTRuleBlock *pBlock, PtrTRule *pNextTRule,
                        PtrSSchema *pSS, PtrTRuleBlock *pNextBlock)
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
              TtaReadName (file, (unsigned char *)pCond->TcAscendNature);
              TtaReadSignedShort (file, &pCond->TcAscendRelLevel);
              switch (pCond->TcCondition)
                {
                case TcondScript:
                  TtaReadByte (file, (unsigned char *)&pCond->TcScript);
                  break;
                case TcondWithin:
                case TcondFirstWithin:
                  TtaReadShort (file, &pCond->TcElemType);
                  TtaReadName (file, (unsigned char *)pCond->TcElemNature);
                  TtaReadBool (file, &pCond->TcImmediatelyWithin);
                  pCond->TcAscendRel = ReadRelatNAscend (file);
                  TtaReadShort (file, &pCond->TcAscendLevel);
                  break;
                case TcondAttr:
                  TtaReadShort (file, &pCond->TcAttr);
                  if (!error)
                    switch ((*pSS)->SsAttribute->TtAttr[pCond->TcAttr - 1]->AttrType)
                      {
                      case AtNumAttr:
                        TtaReadSignedShort (file, &pCond->TcLowerBound);
                        TtaReadSignedShort (file, &pCond->TcUpperBound);
                        break;
                      case AtTextAttr:
                        TtaReadName (file, (unsigned char *)pCond->TcTextValue);
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
                    {
                      if (pCond->TcAttr == PtSize ||
                          pCond->TcAttr == PtIndent ||
                          pCond->TcAttr == PtLineSpacing ||
                          pCond->TcAttr == PtLineWeight ||
                          pCond->TcAttr == PtFillPattern ||
                          pCond->TcAttr == PtOpacity ||
                          pCond->TcAttr == PtFillOpacity ||
                          pCond->TcAttr == PtStrokeOpacity ||
                          pCond->TcAttr == PtStopOpacity ||
                          pCond->TcAttr == PtMarker ||
                          pCond->TcAttr == PtMarkerStart ||
                          pCond->TcAttr == PtMarkerMid ||
                          pCond->TcAttr == PtMarkerEnd ||
                          pCond->TcAttr == PtBackground ||
                          pCond->TcAttr == PtForeground ||
			  pCond->TcAttr == PtColor ||
			  pCond->TcAttr == PtStopColor)
                        {
                          TtaReadSignedShort (file, &pCond->TcLowerBound);
                          TtaReadSignedShort (file, &pCond->TcUpperBound);
                        }
                      else
                        TtaReadByte (file, (unsigned char *)&pCond->TcPresValue);
                    }
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
static void FreeBlocks (PtrTRuleBlock pBlock)
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
static void ReadAttrTRules (BinFile file, int att, PtrTRuleBlock *pNextBlock,
                            PtrTRule *pNextTRule, PtrSSchema *pSS,
                            PtrTSchema *pTSch)
{
  PtrAttributeTransl  pAttrT;
  TranslNumAttrCase  *pCase;
  int                 i;

  if (!error)
    {
      pAttrT = (*pTSch)->TsAttrTRule->TsAttrTransl[att];
      TtaReadShort (file, &pAttrT->AtrElemType);
      switch ((*pSS)->SsAttribute->TtAttr[att]->AttrType)
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
          TtaReadName (file, (unsigned char *)pAttrT->AtrTextValue);
          ReadBlocks (file, &pAttrT->AtrTxtTRuleBlock, pNextTRule,
                      pSS, pNextBlock);
          break;
        case AtReferenceAttr:
          ReadBlocks (file, &pAttrT->AtrRefTRuleBlock, pNextTRule,
                      pSS, pNextBlock);
          break;
        case AtEnumAttr:
          for (i = 0; i <= (*pSS)->SsAttribute->TtAttr[att]->AttrNEnumValues; i++)
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
static void FreeTRulesAttr (PtrAttributeTransl pAttrT, PtrTtAttribute pAttr)
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
static void ReadPresTRules (BinFile file, int pres, PtrTRuleBlock *pNextBlock,
                            PtrTRule *pNextTRule, PtrSSchema *pSS,
                            PtrTSchema *pTSch)
{
  PRuleTransl        *pPruleTr;
  TranslNumAttrCase  *pCase;
  int                 i;

  if (!error)
    {
      pPruleTr = &(*pTSch)->TsPresTRule[pres];
      if (pPruleTr->RtExist)
        {
          if (pres == PtSize || pres == PtIndent ||
              pres == PtLineSpacing || pres == PtLineWeight ||
              pres == PtFillPattern || pres == PtBackground ||
              pres == PtForeground || pres == PtColor || pres == PtStopColor)
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
                TtaReadByte (file, (unsigned char *)&pPruleTr->RtPRuleValue[i]);
                ReadBlocks (file, &pPruleTr->RtPRuleValueBlock[i],
                            pNextTRule, pSS, pNextBlock);
              }
        }
    }
}


/*----------------------------------------------------------------------
  FreeTRulesPres libere les regles de traduction d'une regle de	
  presentation                                            
  ----------------------------------------------------------------------*/
static void FreeTRulesPres (int pres, PRuleTransl *pPruleTr)
{
  int                 i;

  if (pPruleTr->RtExist)
    {
      if (pres == PtSize + 1 || pres == PtIndent + 1 ||
          pres == PtLineSpacing + 1 || pres == PtLineWeight + 1 ||
          pres == PtFillPattern + 1 || pres == PtBackground + 1 ||
          pres == PtForeground + 1 || pres == PtColor + 1 ||
	  pres == PtStopColor + 1)
        /* presentation a valeur numerique */
        for (i = 0; i < pPruleTr->RtNCase; i++)
          FreeBlocks (pPruleTr->RtCase[i].TaTRuleBlock);
      else
        for (i = 0; i <= MAX_TRANSL_PRES_VAL; i++)
          FreeBlocks (pPruleTr->RtPRuleValueBlock[i]);
    }
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
PtrTSchema ReadTranslationSchema (const char* fileName, PtrSSchema pSS)
{
  BinFile             file;
  PtrTRule            pNextTRule;
  PtrTRuleBlock       pNextBlock;
  PtrTSchema          pTSch;
  TCounter           *pCntr;
  TranslVariable     *pVar;
  TranslVarItem      *pVarItem;
  PtrAttributeTransl  pAttrTr;
  ScriptTransl     *pAlphTr;
  StringTransl       *pStringTr;
  PRuleTransl        *pPRuleTr;
  PathBuffer          dirBuffer;
  char                buf[MAX_TXT_LEN];
  int                 InitialNElems, i, j, size;

  error = FALSE;
  pTSch = NULL;
  /* compose le nom du fichier a ouvrir */
  strncpy (dirBuffer, SchemaPath, MAX_PATH);
  MakeCompleteName (fileName, "TRA", dirBuffer, buf, &i);

  /* ouvre le fichier */
  file = TtaReadOpen (buf);
  if (file == 0)
    {
      strncpy (buf, fileName, MAX_PATH);
      strcat (buf, ".TRA");
      TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_TRA_FILE_INCORRECT),
                         buf);
    }
  else
    {
      GetSchTra (&pTSch);
      if (pTSch == NULL)
        {
          TSchemaError (20);
          return NULL;
        }
      /* allocate the element translation table */
      size = pSS->SsNRules * sizeof (PtrTRuleBlock);
      pTSch->TsElemTRule = (ElemTransTable*) TtaGetMemory (size);
      if (pTSch->TsElemTRule)
        memset (pTSch->TsElemTRule, 0, size);

      /* allocate the attribute inherit table */
      size = pSS->SsNRules * sizeof (ThotBool);
      pTSch->TsInheritAttr = (BlnTable*) TtaGetMemory (size);
      if (pTSch->TsInheritAttr)
        memset (pTSch->TsInheritAttr, 0, size);

      /* allocate the attribute translation table */
      size = pSS->SsNAttributes * sizeof (PtrAttributeTransl);
      pTSch->TsAttrTRule = (AttrTransTable*) TtaGetMemory (size);
      if (pTSch->TsAttrTRule)
        memset (pTSch->TsAttrTRule, 0, size);

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
      TtaReadName (file, (unsigned char *)&(pTSch->TsStructName[0]));
      TtaReadShort (file, &(pTSch->TsStructCode));
      TtaReadShort (file, &(pTSch->TsLineLength));
      TtaReadName (file, (unsigned char *)&(pTSch->TsEOL[0]));
      TtaReadName (file, (unsigned char *)&(pTSch->TsTranslEOL[0]));
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
      TtaReadShort (file, &pTSch->TsNVarBuffers);
      for (i = 0; i < MAX_TRANSL_BUFFER; i++)
        {
          pTSch->TsVarBuffer[i].VbIdent[0] = EOS;
          pTSch->TsVarBuffer[i].VbNum = 0;
        }
      i = 0;
      for (i = 0; i < pTSch->TsNVarBuffers; i++)
        {
          TtaReadName (file, (unsigned char *)pTSch->TsVarBuffer[i].VbIdent);
          TtaReadShort (file, &pTSch->TsVarBuffer[i].VbNum);
        }
      for (i = 0; i < MAX_TRANSL_BUFFER; i++)
        pTSch->TsBuffer[i][0] = EOS;
      if (pSS->SsFirstDynNature == 0)
        InitialNElems = pSS->SsNRules;
      else
        {
          InitialNElems = pSS->SsFirstDynNature - 1;
          for (j = InitialNElems; j < pSS->SsNRules; j++)
            pTSch->TsElemTRule->TsElemTransl[j] = NULL;
        }
      if (!error)
        for (i = 0; i < InitialNElems; i++)
          pTSch->TsElemTRule->TsElemTransl[i] =
            ReadPtrTRuleBlock (file, &pNextBlock);
      if (!error)
        for (i = 0; i < InitialNElems; i++)
          TtaReadBool (file, &(pTSch->TsInheritAttr->Bln[i]));
      if (!error)
        for (i = 0; i < pSS->SsNAttributes && !error; i++)
          {
            GetAttributeTransl (&pAttrTr);
            if (!pAttrTr)
              TSchemaError (25);
            else
              {
                pTSch->TsAttrTRule->TsAttrTransl[i] = pAttrTr;
                switch (pSS->SsAttribute->TtAttr[i]->AttrType)
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
                    for (j = 0; j <= pSS->SsAttribute->TtAttr[i]->AttrNEnumValues; j++)
                      pAttrTr->AtrEnuTRuleBlock[j] =
                        ReadPtrTRuleBlock (file, &pNextBlock);
                    break;
                  default:
                    break;
                  }
              }
          }
      if (!error)
        for (i = 0; i < MAX_TRANSL_PRULE; i++)
          if (!error)
            {
              pPRuleTr = &pTSch->TsPresTRule[i];
              TtaReadBool (file, &pPRuleTr->RtExist);
              if (pPRuleTr->RtExist)
                {
                  if (i == PtSize || i == PtIndent || i == PtLineSpacing ||
                      i == PtLineWeight || i == PtFillPattern ||
                      i == PtBackground || i == PtForeground ||
		      i == PtColor || i == PtStopColor)
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
            }
      TtaReadShort (file, &(pTSch->TsNTranslScripts));
      if (!error)
        for (i = 0; i < pTSch->TsNTranslScripts; i++)
          {
            pAlphTr = &pTSch->TsTranslScript[i];
            TtaReadByte (file, (unsigned char *)&pAlphTr->AlScript);
            TtaReadShort (file, &pAlphTr->AlBegin);
            TtaReadShort (file, &pAlphTr->AlEnd);
          }
      TtaReadShort (file, &(pTSch->TsSymbolFirst));
      TtaReadShort (file, &(pTSch->TsSymbolLast));
      TtaReadShort (file, &pTSch->TsGraphicsFirst);
      TtaReadShort (file, &pTSch->TsGraphicsLast);
      TtaReadShort (file, &pTSch->TsNCharTransls);
      /* lit les tables de traduction de caracteres */
      if (!error)
        for (i = 0; i < pTSch->TsNCharTransls; i++)
          {
            pStringTr = &(pTSch->TsCharTransl[i]);
            /* lit la chaine source */
            j = 0;
            do
              TtaReadByte (file, (unsigned char *)&pStringTr->StSource[j++]);
            while (pStringTr->StSource[j - 1] != EOS);
            /* lit la chaine cible */
            j = 0;
            do
              TtaReadByte (file, (unsigned char *)&pStringTr->StTarget[j++]);
            while (pStringTr->StTarget[j - 1] != EOS);
          }
      if (!error)
        /* lit les constantes */
        for (i = 0; i < pTSch->TsNConstants; i++)
          {
            j = pTSch->TsConstBegin[i] - 1;
            do
              TtaReadByte (file, (unsigned char *)&pTSch->TsConstant[j++]);
            while (pTSch->TsConstant[j - 1] != EOS);
          }
      /* lit les blocs de regles des elements */
      if (!error)
        for (i = 0; i < InitialNElems; i++)
          ReadBlocks (file, &pTSch->TsElemTRule->TsElemTransl[i],
                      &pNextTRule, &pSS, &pNextBlock);
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
void FreeTranslationSchema (PtrTSchema pTSch, PtrSSchema pSS)
{
  int                 InitialNElems;
  int                 i;

  if (pSS->SsFirstDynNature == 0)
    InitialNElems = pSS->SsNRules;
  else
    InitialNElems = pSS->SsFirstDynNature - 1;
  /* libere les blocs de regles  des elements */
  for (i = 0; i < InitialNElems; i++)
    FreeBlocks (pTSch->TsElemTRule->TsElemTransl[i]);
  /* libere les blocs de regles des attributs */
  for (i = 0; i < pSS->SsNAttributes; i++)
    {
      FreeTRulesAttr (pTSch->TsAttrTRule->TsAttrTransl[i],
                      pSS->SsAttribute->TtAttr[i]);
      FreeAttributeTransl (pTSch->TsAttrTRule->TsAttrTransl[i]);
    }
  /* libere les blocs de regles des presentations */
  for (i = 0; i < MAX_TRANSL_PRULE; i++)
    FreeTRulesPres (i + 1, &pTSch->TsPresTRule[i]);
  if (pTSch->TsElemTRule)
    {
      TtaFreeMemory (pTSch->TsElemTRule);
      pTSch->TsElemTRule = NULL;
    }
  if (pTSch->TsInheritAttr)
    {
      TtaFreeMemory (pTSch->TsInheritAttr);
      pTSch->TsInheritAttr = NULL;
    }
  if (pTSch->TsAttrTRule)
    {
      TtaFreeMemory (pTSch->TsAttrTRule);
      pTSch->TsAttrTRule = NULL;
    }
  /* libere le schema de traduction lui-meme */
  FreeSchTra (pTSch, pSS);
}
