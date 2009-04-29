/*
 *
 *  (c) COPYRIGHT INRIA 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Structure schema compiler:
 *  Source file name must have the suffix .S
 * This compiler is directed by the structure language grammar (see file
 * STRUCT.GRM)
 * It generaes compiled files with suffix .STR which are used by the
 * Thot library.
 *
 * Author: V. Quint
 *         R. Guetari: Windows adaptation
 *         I. Vatton: new exceptions
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "strdef.h"
#include "constmedia.h"
#include "constgrm.h"
#include "compilmsg.h"
#include "message.h"
#include "strmsg.h"
#include "typemedia.h"
#include "typegrm.h"
#include "fileaccess.h"

#define MAX_SRULE_RECURS 15   /* maximum of included rule levels within a rule */
#define MAX_EXTERNAL_TYPES 20 /* maximum of different external document types */
typedef enum
  {
    AttrRef, ElemRef, ElContent
  }
ContStrExt;

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "compil_tv.h"
#include "platform_tv.h"
#include "analsynt_tv.h"

#ifdef _WINDOWS
#define FATAL_EXIT_CODE 33
#define COMP_SUCCESS     0
#else  /* !_WINDOWS */
#define FATAL_EXIT_CODE -1
#endif /* _WINDOWS */

int                 LineNum;	   /* lines counter in source file */
static Name         srceFileName;  /* file name of the schema cpp processed */
static PtrSSchema   pSSchema;	   /* pointer to the structure schema */
static int          TextConstPtr;  /* current index in constants buffer */
static SRule       *CurExtensRule; /* current extension rule */
static ThotBool     CompilAttr;    /* we are parsing global attributes */
static ThotBool     CompilLocAttr; /* we are parsing local attributes */
static ThotBool     CompilUnits;   /* we are parsing exported units */
static ThotBool     RootRule;	   /* we are waiting for the root rule */
static ThotBool     Rules;	   /* we are parsing rules */
static ThotBool     CompilExtens;  /* on we are parsing extension rules */
static ThotBool     ExceptExternalType; /* we met "EXTERN" before a type name
                                           in section EXCEPT */
static ThotBool     Minimum;	   /* minimum elements within a list */
static ThotBool     Maximum;	   /* maximum elements within a list */
static ThotBool     RRight[MAX_SRULE_RECURS]; /* we are within the right side
                                                 of the rule */
static int          RecursLevel;   /* recursivity level */
static int          CurRule[MAX_SRULE_RECURS];	/* rule number */
static Name         CurName;	   /* left name of the last met rule */
static int          CurNum;	   /* index of this name in the identifiers
                                  table */
static int          CurNLocAttr;   /* number of local attributes attached to
                                      element CurName */
static int          LocAttrTableSize; /* current size of tables CurLocAttr and
                                         CurReqAttr */
static NumTable    *CurLocAttr;    /* local attributes attached to element
                                      element CurName */
static BoolTable   *CurReqAttr;    /* 'Required' booleans of local attributes
                                      associated to element CurName */
static ThotBool     CurUnit;	   /* the last met rule is a exported unit */
static ThotBool     Equal;	   /* it is the equality rule*/
static ThotBool     Option;	   /* it is an aggregate optional component */
static ThotBool     MandatoryAttr; /* it is a mandatory attribute */
static int          Sign;	   /* -1 or 1 to give the sign of the last
                                attribute value */
static PtrSSchema   pExternSSchema;/* pointer to the external structure
                                      schema */
static int          RuleExportWith;/* current exported element to be managed */
static Name         ReferredTypeName; /* last name of the reference type */
static int          BeginReferredTypeName; /* position of this name in the
                                              line */
static ContStrExt   ExternalStructContext; /* context used by the external
                                              structure */
static ThotBool     UnknownContent;/* the content of exported element
                                      is not defined in the schema */
static Name         PreviousIdent; /* name of the last met type identifier */
static int          PreviousRule;

static int          NExternalTypes;/* number of types declared as external */
static Name         ExternalType[MAX_EXTERNAL_TYPES]; /* table of type names
                                                         declared as external*/
static ThotBool     IncludedExternalType[MAX_EXTERNAL_TYPES]; /* table of type
                                                                 names declared as included external */
static ThotBool     CompilExcept;  /* we are parsing exceptions */
static int          ExceptType;	   /* element type concerned by exceptions */
static int          ExceptAttr;	   /* attribute concerned by exceptions */
static int          CurBasicElem;  /* current basic type */
static ThotBool     FirstInPair;   /* we met the keyword "First" */
static ThotBool     SecondInPair;  /* we met the keyword "Second" */
static ThotBool     ReferenceAttr; /* we manage a reference attribute */
static ThotBool     ImportExcept;  /* we met exception ImportLine or
                                      ImportParagraph */

#include "platform_f.h"
#include "parser_f.h"
#include "writestr_f.h"
#include "readstr_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "compilmsg_f.h"
#include "registry_f.h"

#ifdef _WINDOWS
#include "compilers_f.h"
#ifndef DLLEXPORT
#define DLLEXPORT __declspec (dllexport)
#endif  /* DLLEXPORT */
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  InitBasicType                                                  
  ----------------------------------------------------------------------*/
static void InitBasicType (SRule *pRule, const char *name, BasicType typ)
{
  if (pRule->SrName == NULL)
    {
      pRule->SrName = (char *)TtaGetMemory (MAX_NAME_LENGTH);
      pRule->SrOrigName = (char *)TtaGetMemory (MAX_NAME_LENGTH);
    }
  strncpy (pRule->SrName, name, MAX_NAME_LENGTH);
  pRule->SrConstruct = CsBasicElement;
  pRule->SrBasicType = typ;
  pRule->SrUnitElem = False;
  pRule->SrExportedElem = False;
  pRule->SrFirstExcept = 0;
  pRule->SrLastExcept = 0;
  pRule->SrNDefAttrs = 0;
  pRule->SrRecursive = False;
  pRule->SrNLocalAttrs = 0;
  pRule->SrLocalAttr = NULL;
  pRule->SrRequiredAttr = NULL;
  pRule->SrNInclusions = 0;
  pRule->SrNExclusions = 0;
  pRule->SrRefImportedDoc = False;
}

/*----------------------------------------------------------------------
  Initialize initializes the structure schema in memory.
  ----------------------------------------------------------------------*/
static void         Initialize ()
{
  int                 i, size;
  PtrTtAttribute      pAttr;
  PtrSRule            pRule;

  pSSchema->SsExtension = False;
  pSSchema->SsNExtensRules = 0;
  pSSchema->SsExtensBlock = NULL;
  pSSchema->SsDocument = 0;
  pSSchema->SsRootElem = 0;
  CurExtensRule = NULL;
  pSSchema->SsNAttributes = 0;

  /* allocate memory for 10 attributes initially */
  size = 10;
  pSSchema->SsAttribute = (TtAttrTable*) malloc (size * sizeof (PtrTtAttribute));
  if (!pSSchema->SsAttribute)
    {
      TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);
      return;
    }
  else
    {
      pSSchema->SsAttrTableSize = size;
      for (i = 0; i < size; i++)
        pSSchema->SsAttribute->TtAttr[i] = NULL;
    }
  /* create the language attribute */
  pAttr = (PtrTtAttribute) malloc (sizeof (TtAttribute));
  pSSchema->SsAttribute->TtAttr[0] = pAttr;
  pAttr->AttrName = (char *)TtaGetMemory (MAX_NAME_LENGTH);   
  pAttr->AttrOrigName = (char *)TtaGetMemory (MAX_NAME_LENGTH);   
  strncpy (pAttr->AttrName, "Language", MAX_NAME_LENGTH);
  pAttr->AttrOrigName[0] = '\0';
  pAttr->AttrGlobal = True;
  pAttr->AttrFirstExcept = 0;
  pAttr->AttrLastExcept = 0;
  pAttr->AttrType = AtTextAttr;
  pSSchema->SsNAttributes++;

  pSSchema->SsNRules = 0;
  /* allocate memory for 10 rules initially */
  size = MAX_BASIC_TYPE + 2;
  pSSchema->SsRule = (SrRuleTable*) malloc (size * sizeof (PtrSRule));
  if (!pSSchema->SsRule)
    {
      TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);
      return;
    }
  else
    {
      pSSchema->SsRuleTableSize = size;
      for (i = 0; i < size; i++)
        pSSchema->SsRule->SrElem[i] = NULL;
    }

  /* first rules of the structure schema are those that define basic types */
  pRule = (PtrSRule) malloc (sizeof (SRule));
  pSSchema->SsRule->SrElem[CharString] = pRule;
  memset (pRule, 0, sizeof (SRule));
  InitBasicType (pRule, "TEXT_UNIT", CharString);

  pRule = (PtrSRule) malloc (sizeof (SRule));
  pSSchema->SsRule->SrElem[GraphicElem] = pRule;
  memset (pRule, 0, sizeof (SRule));
  InitBasicType (pRule, "GRAPHICS_UNIT", GraphicElem);

  pRule = (PtrSRule) malloc (sizeof (SRule));
  pSSchema->SsRule->SrElem[Symbol] = pRule;
  memset (pRule, 0, sizeof (SRule));
  InitBasicType (pRule, "SYMBOL_UNIT", Symbol);

  pRule = (PtrSRule) malloc (sizeof (SRule));
  pSSchema->SsRule->SrElem[tt_Picture] = pRule;
  memset (pRule, 0, sizeof (SRule));
  InitBasicType (pRule, "PICTURE_UNIT", tt_Picture);

  pRule = (PtrSRule) malloc (sizeof (SRule));
  pSSchema->SsRule->SrElem[Refer] = pRule;
  memset (pRule, 0, sizeof (SRule));
  InitBasicType (pRule, "REFERENCE_UNIT", Refer);

  pRule = (PtrSRule) malloc (sizeof (SRule));
  pSSchema->SsRule->SrElem[PageBreak] = pRule;
  memset (pRule, 0, sizeof (SRule));
  InitBasicType (pRule, "PAGE_BREAK", PageBreak);

  pRule = (PtrSRule) malloc (sizeof (SRule));
  pSSchema->SsRule->SrElem[AnyType] = pRule;
  memset (pRule, 0, sizeof (SRule));
  InitBasicType (pRule, "ANY_TYPE", AnyType);

  pSSchema->SsNRules = MAX_BASIC_TYPE;

  /* create the Document rule */
  pRule = (PtrSRule) malloc (sizeof (SRule));
  pSSchema->SsRule->SrElem[pSSchema->SsNRules++] = pRule;
  pSSchema->SsDocument = pSSchema->SsNRules;
  pRule->SrName = (char *)TtaGetMemory (MAX_NAME_LENGTH);
  pRule->SrOrigName = (char *)TtaGetMemory (MAX_NAME_LENGTH);
  strcpy (pRule->SrName, "Document");
  pRule->SrConstruct = CsDocument;
  pRule->SrUnitElem = False;
  pRule->SrExportedElem = False;
  pRule->SrFirstExcept = 0;
  pRule->SrLastExcept = 0;
  pRule->SrNDefAttrs = 0;
  pRule->SrRecursive = False;
  pRule->SrNLocalAttrs = 0;
  pRule->SrLocalAttr = NULL;
  pRule->SrRequiredAttr = NULL;
  pRule->SrNInclusions = 0;
  pRule->SrNExclusions = 0;
  pRule->SrRefImportedDoc = False;

  pSSchema->SsConstBuffer[0] = '\0';
  pSSchema->SsConstBuffer[1] = '\0';
  pSSchema->SsExport = False;
  pSSchema->SsNExceptions = 0;
  TextConstPtr = 1;
  CompilAttr = False;
  CompilLocAttr = False;
  CompilUnits = False;
  RootRule = False;
  Rules = False;
  CompilExtens = False;
  Minimum = False;
  Maximum = False;
  RecursLevel = 1;
  RRight[0] = False;

  CurRule[0] = pSSchema->SsNRules;
  CurName[0] = '\0';
  CurNum = 0;
  CurNLocAttr = 0;
  LocAttrTableSize = 20;
  CurLocAttr =  (NumTable*) TtaGetMemory (LocAttrTableSize * sizeof (int));
  CurReqAttr =  (BoolTable*) TtaGetMemory (LocAttrTableSize * sizeof (ThotBool));
  CurUnit = False;
  Equal = False;
  Sign = 1;
  NExternalTypes = 0;
  CompilExcept = False;
  ExceptType = 0;
  ExceptAttr = 0;
  CurBasicElem = 0;
  PreviousRule = 0;
  FirstInPair = False;
  SecondInPair = False;
  ImportExcept = False;
}

/*----------------------------------------------------------------------
  RuleNameExist checks if the last rule name of the rules table is
  already used within other rule of the same table.	
  Returns TRUE is it is the case.			
  ----------------------------------------------------------------------*/
static ThotBool      RuleNameExist ()
{
  int                 r;
  ThotBool            ret;
  Name                name;

  /* initialize the function return */
  ret = False;
  /* keep the last rule name */
  strncpy (name, pSSchema->SsRule->SrElem[pSSchema->SsNRules - 1]->SrName,
           MAX_NAME_LENGTH);
  if (name[0] != '\0')
    {
      /* index of the rule in the table */
      r = 0;
      do
        ret = strcmp (name, pSSchema->SsRule->SrElem[r++]->SrName) == 0;
      while (!ret && r < pSSchema->SsNRules - 1);
    }
  return ret;
}

/*----------------------------------------------------------------------
  AllocateNewRule
  ----------------------------------------------------------------------*/
static void      AllocateNewRule (PtrSSchema pSS)
{
  PtrSRule     pRule;
  int          i, size;

  if (pSS->SsNRules >= pSS->SsRuleTableSize)
    /* the rule table is full. Extend it */
    {
      /* add 10 new entries */
      size = pSS->SsNRules + 10;
      i = size * sizeof (PtrSRule);
      pSS->SsRule = (SrRuleTable*) realloc (pSS->SsRule, i);
      if (!pSS->SsRule)
        TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);
      else
        {
          pSS->SsRuleTableSize = size;
          for (i = pSS->SsNRules; i < size; i++)
            pSS->SsRule->SrElem[i] = NULL;
        }
    }
  /* create a new rule descriptor */
  pRule = (PtrSRule) malloc (sizeof (SRule));
  if (pRule == NULL)
    TtaDisplaySimpleMessage (FATAL, STR,STR_NOT_ENOUGH_MEM);
  else
    {
      memset (pRule, 0, sizeof (SRule));
      pRule->SrName = (char *)TtaGetMemory (MAX_NAME_LENGTH);
      pRule->SrOrigName = (char *)TtaGetMemory (MAX_NAME_LENGTH);
      pSS->SsRule->SrElem[pSS->SsNRules++] = pRule;
    }
}

/*----------------------------------------------------------------------
  UndefinedElem
  ----------------------------------------------------------------------*/
static void         UndefinedElem (int n)
{
  int                 j;
  SrcIdentDesc       *pIdent;
  PtrSRule            pRule;

  pIdent = &Identifier[n - 1];
  TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_EXTERNAL_STRUCT),
                     pIdent->SrcIdentifier);
  /* add a new rule at the end of the schema */
  AllocateNewRule (pSSchema);
  pIdent->SrcIdentDefRule = pSSchema->SsNRules;
  if (pIdent->SrcIdentLen > MAX_NAME_LENGTH - 1)
    TtaDisplaySimpleMessage (FATAL, STR, STR_WORD_TOO_LONG);
  else
    {
      pRule = pSSchema->SsRule->SrElem[pSSchema->SsNRules - 1];
      for (j = 0; j < pIdent->SrcIdentLen; j++)
        pRule->SrName[j] = pIdent->SrcIdentifier[j];
      pRule->SrName[pIdent->SrcIdentLen] = '\0';
      pRule->SrNLocalAttrs = 0;
      pRule->SrLocalAttr = NULL;
      pRule->SrRequiredAttr = NULL;
      pRule->SrNInclusions = 0;
      pRule->SrNExclusions = 0;
      pRule->SrRefImportedDoc = False;
      pRule->SrNDefAttrs = 0;
      pRule->SrUnitElem = False;
      pRule->SrConstruct = CsNatureSchema;
      pRule->SrSSchemaNat = NULL;
      pRule->SrRecursive = False;
      pRule->SrExportedElem = False;
      pRule->SrFirstExcept = 0;
      pRule->SrLastExcept = 0;
      if (RuleNameExist ())
        TtaDisplaySimpleMessage (FATAL, STR, STR_NAME_ALREADY_DECLARED);
    }
}

/*----------------------------------------------------------------------
  ChangeOneRule sets the rigth number of one rule instead of its identifier.
  Undefined elements are considered as external structures (natures).	
  ----------------------------------------------------------------------*/
static void         ChangeOneRule (PtrSRule pRule)
{
  int                 j;

  /* management of inclusions */
  for (j = 0; j < pRule->SrNInclusions; j++)
    {
      if (pRule->SrInclusion[j] > MAX_BASIC_TYPE)
        {
          if (Identifier[pRule->SrInclusion[j] - MAX_BASIC_TYPE - 1].
              SrcIdentDefRule == 0)
            UndefinedElem (pRule->SrInclusion[j] - MAX_BASIC_TYPE);
          pRule->SrInclusion[j] =
            Identifier[pRule->SrInclusion[j] - MAX_BASIC_TYPE - 1].
            SrcIdentDefRule;
        }
      else if (pRule->SrInclusion[j] < 0)
        pRule->SrInclusion[j] = -pRule->SrInclusion[j];
      pSSchema->SsRule->SrElem[pRule->SrInclusion[j] - 1]->SrRecursDone = False;
    }

  /* management of exclusions */
  for (j = 0; j < pRule->SrNExclusions; j++)
    {
      if (pRule->SrExclusion[j] > MAX_BASIC_TYPE)
        {
          if (Identifier[pRule->SrExclusion[j] - MAX_BASIC_TYPE - 1].
              SrcIdentDefRule == 0)
            UndefinedElem (pRule->SrExclusion[j] - MAX_BASIC_TYPE);
          pRule->SrExclusion[j] =
            Identifier[pRule->SrExclusion[j] - MAX_BASIC_TYPE - 1].
            SrcIdentDefRule;
        }
      else if (pRule->SrExclusion[j] < 0)
        pRule->SrExclusion[j] = -pRule->SrExclusion[j];
    }
  /* management depends on the rule constructor */
  switch (pRule->SrConstruct)
    {
    case CsReference:
      /* we consider only types defined in the same schema */
      if (pRule->SrRefTypeNat[0] == '\0')
        {
          if (pRule->SrReferredType > MAX_BASIC_TYPE)
            {
              if (Identifier[pRule->SrReferredType - MAX_BASIC_TYPE - 1].
                  SrcIdentDefRule == 0)
                UndefinedElem (pRule->SrReferredType - MAX_BASIC_TYPE);
              pRule->SrReferredType =
                Identifier[pRule->SrReferredType - MAX_BASIC_TYPE - 1].
                SrcIdentDefRule;
            }
          else if (pRule->SrReferredType < 0)
            pRule->SrReferredType = -pRule->SrReferredType;
        }
      break;
    case CsIdentity:
      if (pRule->SrIdentRule > MAX_BASIC_TYPE)
        {
          if (Identifier[pRule->SrIdentRule - MAX_BASIC_TYPE - 1].
              SrcIdentDefRule == 0)
            UndefinedElem (pRule->SrIdentRule - MAX_BASIC_TYPE);
          pRule->SrIdentRule =
            Identifier[pRule->SrIdentRule - MAX_BASIC_TYPE - 1].
            SrcIdentDefRule;
        }
      else if (pRule->SrIdentRule < 0)
        pRule->SrIdentRule = -pRule->SrIdentRule;
      pSSchema->SsRule->SrElem[pRule->SrIdentRule - 1]->SrRecursDone = False;
      break;
    case CsList:
      if (pRule->SrListItem > MAX_BASIC_TYPE)
        {
          if (Identifier[pRule->SrListItem - MAX_BASIC_TYPE - 1].
              SrcIdentDefRule == 0)
            UndefinedElem (pRule->SrListItem - MAX_BASIC_TYPE);
          pRule->SrListItem = Identifier[pRule->SrListItem-MAX_BASIC_TYPE-1].
            SrcIdentDefRule;
        }
      else if (pRule->SrListItem < 0)
        pRule->SrListItem = -pRule->SrListItem;
      pSSchema->SsRule->SrElem[pRule->SrListItem - 1]->SrRecursDone = False;
      break;
    case CsChoice:
      if (pRule->SrNChoices > 0)
        for (j = 0; j < pRule->SrNChoices; j++)
          {
            if (pRule->SrChoice[j] > MAX_BASIC_TYPE)
              {
                if (Identifier[pRule->SrChoice[j] - MAX_BASIC_TYPE - 1].
                    SrcIdentDefRule == 0)
                  UndefinedElem (pRule->SrChoice[j] - MAX_BASIC_TYPE);
                pRule->SrChoice[j] =
                  Identifier[pRule->SrChoice[j] - MAX_BASIC_TYPE - 1].
		              SrcIdentDefRule;
              }
            else if (pRule->SrChoice[j] < 0)
              pRule->SrChoice[j] = -pRule->SrChoice[j];
            pSSchema->SsRule->SrElem[pRule->SrChoice[j] - 1]->SrRecursDone = False;
          }
      break;
    case CsUnorderedAggregate:
    case CsAggregate:
      for (j = 0; j < pRule->SrNComponents; j++)
        {
          if (pRule->SrComponent[j] > MAX_BASIC_TYPE)
            {
              if (Identifier[pRule->SrComponent[j] - MAX_BASIC_TYPE - 1].
                  SrcIdentDefRule == 0)
                UndefinedElem (pRule->SrComponent[j] - MAX_BASIC_TYPE);
              pRule->SrComponent[j] =
                Identifier[pRule->SrComponent[j] - MAX_BASIC_TYPE - 1].
                SrcIdentDefRule;
            }
          else if (pRule->SrComponent[j] < 0)
            pRule->SrComponent[j] = -pRule->SrComponent[j];
          pSSchema->SsRule->SrElem[pRule->SrComponent[j] - 1]->SrRecursDone = False;
        }
      break;
    case CsConstant:
    case CsBasicElement:
    case CsNatureSchema:
    case CsPairedElement:
    case CsExtensionRule:
    case CsDocument:
    case CsAny:
    case CsEmpty:
      break;
    }
}

/*----------------------------------------------------------------------
  ChangeRules sets the rigth number of rules instead of their indentifiers.
  Undefined elements are considered as external structures (natures).	
  ----------------------------------------------------------------------*/
static void         ChangeRules ()
{
  int                 i;
  PtrTtAttribute      pAttr;
   
  /* use SrRecursDone with the mean `unused rule' */
  for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
    pSSchema->SsRule->SrElem[i]->SrRecursDone = True;
  /* go through all rules of built elements */
  for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
    ChangeOneRule (pSSchema->SsRule->SrElem[i]);
  /* go through all extension rules */
  for (i = 0; i < pSSchema->SsNExtensRules; i++)
    ChangeOneRule (&pSSchema->SsExtensBlock->EbExtensRule[i]);
  /* go through all attributes defined in the structure schema */
  for (i = 0; i < pSSchema->SsNAttributes; i++)
    {
      pAttr = pSSchema->SsAttribute->TtAttr[i];
      /* don't take care of reference attributes */
      if (pAttr->AttrType == AtReferenceAttr)
        /* only take care of types defined in the same schema */
        {
          if (pAttr->AttrTypeRefNature[0] == '\0')
            {
              if (pAttr->AttrTypeRef > MAX_BASIC_TYPE)
                {
                  if (Identifier[pAttr->AttrTypeRef - MAX_BASIC_TYPE - 1].
                      SrcIdentDefRule == 0)
                    UndefinedElem (pAttr->AttrTypeRef - MAX_BASIC_TYPE);
                  pAttr->AttrTypeRef =
                    Identifier[pAttr->AttrTypeRef - MAX_BASIC_TYPE - 1].
                    SrcIdentDefRule;
                }
              else if (pAttr->AttrTypeRef < 0)
                pAttr->AttrTypeRef = -pAttr->AttrTypeRef;
            }
        }
    }
}

/*----------------------------------------------------------------------
  CopyWord copies  the current word into the name parameter.
  ----------------------------------------------------------------------*/
static void         CopyWord (Name name, indLine wi, indLine wl)
{
  if (wl > MAX_NAME_LENGTH - 1)
    CompilerMessage (wi, STR, FATAL, STR_WORD_TOO_LONG, inputLine, LineNum);
  else
    {
      strncpy ((char *)name, (char *)&inputLine[wi - 1], wl);
      name[wl] = '\0';
    }
}

/*----------------------------------------------------------------------
  Push                                                            
  ----------------------------------------------------------------------*/
static void         Push (indLine wi)
{
  if (RecursLevel >= MAX_SRULE_RECURS)
    CompilerMessage (wi, STR, FATAL, STR_RULE_NESTING_TOO_DEEP, inputLine,
                     LineNum);
  else
    {
      RRight[RecursLevel] = False;
      CurRule[RecursLevel] = 0;
      RecursLevel++;
    }
}

/*----------------------------------------------------------------------
  RightIdentifier                                                 
  ----------------------------------------------------------------------*/
static void         RightIdentifier (int n, indLine wi)
{
  PtrSRule              pRule;

  if (PreviousRule > 0)
    {
      n = -PreviousRule;
      PreviousRule = 0;
    }
  pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
  switch (pRule->SrConstruct)
    {
    case CsReference:
      pRule->SrReferredType = n;
      break;
    case CsIdentity:
      pRule->SrIdentRule = n;
      break;
    case CsList:
      pRule->SrListItem = n;
      break;
    case CsChoice:
      if (pRule->SrNChoices >= MAX_OPTION_CASE)
        CompilerMessage (wi, STR, FATAL,
                         STR_TOO_MANY_OPTIONS_IN_THE_CASE_STATEMENT,
                         inputLine, LineNum);
      else
        {
          pRule->SrNChoices++;
          pRule->SrChoice[pRule->SrNChoices - 1] = n;
        }
      break;
    case CsUnorderedAggregate:
    case CsAggregate:
      if (pRule->SrNComponents >= MAX_COMP_AGG)
        CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_COMPONENTS_IN_AGGREGATE,
                         inputLine, LineNum);
      else
        {
          pRule->SrNComponents++;
          pRule->SrComponent[pRule->SrNComponents - 1] = n;
          if (Option)
            {
              pRule->SrOptComponent[pRule->SrNComponents - 1] = True;
              Option = False;
            }
        }
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
  CopyLocReqAttr
  ----------------------------------------------------------------------*/
static void CopyLocReqAttr (SRule *pRule)
{
  int i;

  pRule->SrLocalAttr = (NumTable*) TtaGetMemory (pRule->SrNLocalAttrs * sizeof (int));
  pRule->SrRequiredAttr = (BoolTable*) TtaGetMemory (pRule->SrNLocalAttrs * sizeof (ThotBool));
  for (i = 0; i < pRule->SrNLocalAttrs; i++)
    {
      pRule->SrLocalAttr->Num[i] = CurLocAttr->Num[i];
      pRule->SrRequiredAttr->Bln[i] = CurReqAttr->Bln[i];
    }
}

/*----------------------------------------------------------------------
  NewRule                                                         
  ----------------------------------------------------------------------*/
static void         NewRule (indLine wi)
{
  PtrSRule            pRule;

  AllocateNewRule (pSSchema);
  if (CurNum > 0)
    /* there is a rigth part in this rule */
    if (Identifier[CurNum - 1].SrcIdentDefRule > 0)
      /* already defined */
      CompilerMessage (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED,
                       inputLine, LineNum);
  if (!error)
    {
      CurRule[RecursLevel - 1] = pSSchema->SsNRules;
      if (Rules && RecursLevel > 1)
        {
          /* this current rule is used by a enclosing rule */
          RecursLevel--;
          if (CurNum > 0)
            RightIdentifier (CurNum + MAX_BASIC_TYPE, wi);
          else
            RightIdentifier (-CurRule[RecursLevel], wi);
          RecursLevel++;
        }
      if (CurNum > 0)
        Identifier[CurNum - 1].SrcIdentDefRule = pSSchema->SsNRules;
      if (Rules)
        RRight[RecursLevel - 1] = True;
      pRule = pSSchema->SsRule->SrElem[pSSchema->SsNRules - 1];
      strncpy (pRule->SrName, CurName, MAX_NAME_LENGTH);
      pRule->SrNDefAttrs = 0;
      if (pRule->SrNLocalAttrs > 0)
        /* this element has already local attributes */
        CompilerMessage (wi, STR, FATAL, STR_THIS_ELEM_HAS_LOCAL_ATTRS,
                         inputLine, LineNum);
      else
        pRule->SrNLocalAttrs = CurNLocAttr;
      CopyLocReqAttr (pRule);
      pRule->SrUnitElem = CurUnit;
      pRule->SrRecursive = False;
      pRule->SrExportedElem = False;
      pRule->SrFirstExcept = 0;
      /* no exception associated to this element type */
      pRule->SrLastExcept = 0;
      pRule->SrNInclusions = 0;
      pRule->SrNExclusions = 0;
      pRule->SrRefImportedDoc = False;
      if (RuleNameExist ())
        CompilerMessage (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED,
                         inputLine, LineNum);
      if (RootRule)
        {
          /* compare this name with the schema name */
          if (strcmp (pSSchema->SsName, CurName) == 0)
            /* it is the root element of the schema */
            pSSchema->SsRootElem = pSSchema->SsNRules;
          else if (!pSSchema->SsExtension)
            CompilerMessage (wi, STR, FATAL,
                             STR_FIRST_RULE_MUST_BE_THE_ROOT, inputLine,
                             LineNum);
          RootRule = False;
        }
      CurName[0] = '\0';
      CurNum = 0;
      CurNLocAttr = 0;
      CurUnit = False;
    }
}

/*----------------------------------------------------------------------
  RuleNumber returns the rule number that defines the current word as
  element type.
  If this word doesn't match with a previous defined element type,
  the function returns 0.
  ----------------------------------------------------------------------*/
static int          RuleNumber (indLine wl, indLine wi)
{
  int                 RuleNum;
  Name                N;
  ThotBool             ok;

  CopyWord (N, wi, wl);
  RuleNum = 0;
  do
    ok = strcmp (N, pSSchema->SsRule->SrElem[RuleNum++]->SrName) == 0;
  while (!ok && RuleNum < pSSchema->SsNRules);
  if (!ok)
    RuleNum = 0;
  return RuleNum;
}

/*----------------------------------------------------------------------
  AttributeNumber returns the attribute number associated with this name.
  If this word doesn't match with a previous defined attribute,
  the function returns 0.
  ----------------------------------------------------------------------*/
static int          AttributeNumber (indLine wl, indLine wi)
{
  int                 AttrNum;
  Name                N;
  ThotBool             ok;

  CopyWord (N, wi, wl);
  AttrNum = 0;
  do
    ok = strcmp (N, pSSchema->SsAttribute->TtAttr[AttrNum++]->AttrName) == 0;
  while (!ok && AttrNum < pSSchema->SsNAttributes);
  if (!ok)
    AttrNum = 0;
  return AttrNum;
}

/*----------------------------------------------------------------------
  ExceptionNum manages the exception num.
  If checkType is TRUE, the exception has to rest on an element type.
  If checkAttr is TRUE, the exception has to rest on an attribute.
  If checkIntAttr is TRUE, the exception has to rest on a numeric attribute.
  ----------------------------------------------------------------------*/
static void ExceptionNum (int num, ThotBool checkType, ThotBool checkAttr,
                          ThotBool CheckIntAttr, indLine wi)
{
  PtrSRule            pRule;
  PtrTtAttribute      pAttr;

  if (checkType && ExceptType == 0)
    CompilerMessage (wi, STR, FATAL, STR_ONLY_FOR_ELEMS, inputLine, LineNum);
  if (checkAttr && ExceptAttr == 0)
    CompilerMessage (wi, STR, FATAL, STR_ONLY_FOR_ATTRS, inputLine, LineNum);
  if (ExceptAttr > 0)
    {
      if (CheckIntAttr &&
          pSSchema->SsAttribute->TtAttr[ExceptAttr - 1]->AttrType != AtNumAttr)
        CompilerMessage (wi, STR, FATAL, STR_ONLY_FOR_NUMERICAL_ATTRS,
                         inputLine, LineNum);
      if (num == ExcActiveRef &&
          pSSchema->SsAttribute->TtAttr[ExceptAttr - 1]->AttrType != AtReferenceAttr)
        CompilerMessage (wi, STR, FATAL, STR_ONLY_FOR_REFERENCE_ATTRS,
                         inputLine, LineNum);
    }
   
  if (!error)
    {
      if (pSSchema->SsNExceptions >= MAX_EXCEPT_SSCHEMA)
        /* the list of exceptions is full */
        CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_EXCEPTS, inputLine,
                         LineNum);
      else
        /* add the new exception number into the list */
        /* of exceptions in the structure schema */
        {
          pSSchema->SsNExceptions++;
          pSSchema->SsException[pSSchema->SsNExceptions - 1] = num;
          if (ExceptType != 0)
            {
              if (CurExtensRule != NULL)
                pRule = CurExtensRule;
              else
                pRule = pSSchema->SsRule->SrElem[ExceptType - 1];
              if (pRule->SrFirstExcept == 0)
                pRule->SrFirstExcept = pSSchema->SsNExceptions;
              pRule->SrLastExcept = pSSchema->SsNExceptions;
            }
          else if (ExceptAttr != 0)
            {
              pAttr = pSSchema->SsAttribute->TtAttr[ExceptAttr - 1];
              if (pAttr->AttrFirstExcept == 0)
                pAttr->AttrFirstExcept = pSSchema->SsNExceptions;
              pAttr->AttrLastExcept = pSSchema->SsNExceptions;
            }
        }
    }
}

/*----------------------------------------------------------------------
  BasicEl                                                       
  ----------------------------------------------------------------------*/
static void         BasicEl (int n, indLine wi, SyntRuleNum pr)
{
  PtrSRule           pRule;

  pRule = NULL;
  if (pr == RULE_InclElem || pr == RULE_ExclElem)
    {
      if (CompilExtens)
        pRule = CurExtensRule;
      else
        pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
    }
  if (pr == RULE_InclElem)
    {
      if (pRule->SrNInclusions >= MAX_INCL_EXCL_SRULE)
        CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_INCLS_FOR_THAT_ELEM,
                         inputLine, LineNum);
      else
        {
          pRule->SrNInclusions++;
          pRule->SrInclusion[pRule->SrNInclusions - 1] = n;
        }
    }
  else if (pr == RULE_ExclElem)
    {
      if (pRule->SrNExclusions >= MAX_INCL_EXCL_SRULE)
        CompilerMessage (wi, STR, FATAL,
                         STR_TOO_MANY_EXCLUSIONS_FOR_THAT_ELEM, inputLine,
                         LineNum);
      else
        {
          pRule->SrNExclusions++;
          pRule->SrExclusion[pRule->SrNExclusions - 1] = n;
        }
    }
  else if (pr == RULE_ExceptType)
    {
      /* there is a basic element type within the exceptions set */
      ExceptType = n;
      if (pSSchema->SsRule->SrElem[ExceptType - 1]->SrFirstExcept != 0)
        CompilerMessage (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS,
                         inputLine, LineNum);
    }
  else
    {
      if (Equal)
        {
          NewRule (wi);
          Equal = False;
        }
       
      if (!error)
        {
          if (RRight[RecursLevel - 1])
            RightIdentifier (n, wi);
          else
            {
              RecursLevel--;
              RightIdentifier (n, wi);
              RecursLevel++;
            }
          CurBasicElem = n;
        }
    }
}

/*----------------------------------------------------------------------
  StoreConstText stores the constant text                         
  ----------------------------------------------------------------------*/
static int          StoreConstText (int wi, int wl)
{
  int                 i, pos;

  pos = TextConstPtr;
  if (TextConstPtr + wl >= MAX_LEN_ALL_CONST)
    CompilerMessage (wi, STR, FATAL, STR_CONSTANT_BUFFER_FULL, inputLine,
                     LineNum);
  else
    {
      for (i = 0; i <= wl - 2; i++)
        pSSchema->SsConstBuffer[TextConstPtr + i - 1] = inputLine[wi + i - 1];
      TextConstPtr += wl;
      /* mark the end of the constant */
      pSSchema->SsConstBuffer[TextConstPtr - 2] = '\0';
      /* mark the end of all  constants */
      pSSchema->SsConstBuffer[TextConstPtr - 1] = '\0';
    }
  return pos;
}

/*----------------------------------------------------------------------
  InitRule initializes a structure rule.             
  ----------------------------------------------------------------------*/
static void         InitRule (SRule * pRule)
{
  pRule->SrName[0] = '\0';
  pRule->SrNDefAttrs = 0;
  pRule->SrNLocalAttrs = 0;
  pRule->SrLocalAttr = NULL;
  pRule->SrRequiredAttr = NULL;
  pRule->SrUnitElem = False;
  pRule->SrRecursive = False;
  pRule->SrExportedElem = False;
  pRule->SrFirstExcept = 0;
  pRule->SrLastExcept = 0;
  pRule->SrNInclusions = 0;
  pRule->SrNExclusions = 0;
  pRule->SrRefImportedDoc = False;
}

/*----------------------------------------------------------------------
  DuplicatePairRule duplicates the rule if it is a paired rule.
  ----------------------------------------------------------------------*/
static void         DuplicatePairRule ()
{
  PtrSRule         newRule, prevRule;

  prevRule = pSSchema->SsRule->SrElem[pSSchema->SsNRules - 1];
  if (prevRule->SrConstruct == CsPairedElement && prevRule->SrFirstOfPair)
    {
      /* ajoute une regle CsPairedElement a la fin du schema */
      AllocateNewRule (pSSchema);
      newRule = pSSchema->SsRule->SrElem[pSSchema->SsNRules - 1];
      InitRule (newRule);
      strncpy (newRule->SrName, prevRule->SrName, MAX_NAME_LENGTH);
      newRule->SrConstruct = CsPairedElement;
      newRule->SrFirstOfPair = False;
    }
}

/*----------------------------------------------------------------------
  GetExtensionRule searchs an extension rule matching the name (wi, wl).
  ----------------------------------------------------------------------*/
static SRule       *GetExtensionRule (indLine wi, indLine wl)
{
  SRule              *pRule;
  Name                n;
  int                 r;
  ThotBool             found;

  pRule = NULL;
  if (pSSchema->SsExtensBlock != NULL)
    {
      CopyWord (n, wi, wl);
      found = False;
      r = 0;
      while (!found && r < pSSchema->SsNExtensRules)
        {
          pRule = &pSSchema->SsExtensBlock->EbExtensRule[r];
          if (strncmp (n, pRule->SrName, MAX_NAME_LENGTH) == 0)
            found = True;
          else
            r++;
        }
      if (!found)
        pRule = NULL;
    }
  return pRule;
}

/*----------------------------------------------------------------------
  NewExtensionRule creates and initializes a new extension rule.
  ----------------------------------------------------------------------*/
static SRule       *NewExtensionRule (indLine wi, indLine wl)
{
  SRule              *pRule;
  PtrExtensBlock      pEB;

  pRule = NULL;
  if (GetExtensionRule (wi, wl) != NULL)
    CompilerMessage (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED, inputLine,
                     LineNum);
  if (pSSchema->SsExtensBlock == NULL)
    {
      GetExternalBlock (&pEB);
      if (pEB == NULL)
        TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);
      pSSchema->SsExtensBlock = pEB;
    }

  if (pSSchema->SsExtensBlock != NULL)
    {
      if (pSSchema->SsNExtensRules >= MAX_EXTENS_SSCHEMA)
        CompilerMessage (wi, STR, FATAL, STR_TOO_MAN_RULES, inputLine, LineNum);
      else
        {
          pRule = &pSSchema->SsExtensBlock->EbExtensRule[pSSchema->
                                                         SsNExtensRules];
          pSSchema->SsNExtensRules++;
          InitRule (pRule);
          CopyWord (pRule->SrName, wi, wl);
          pRule->SrConstruct = CsExtensionRule;
        }
    }
  return pRule;
}

/*----------------------------------------------------------------------
  ProcessToken manages the next word starting at position wi in the
  current line.
  The parameter wl gives the length of the word and c is its grammatical
  code.
  If the word is an identifier, nb gives the index of this identifier
  in the identifiers table.
  The parameter r gives the rule number where the word appears.
  ----------------------------------------------------------------------*/
static void         ProcessToken (indLine wi, indLine wl, SyntacticCode c,
                                  SyntacticCode r, int nb, SyntRuleNum pr)
{
  int                 SynInteger, i, j, size;
  Name                N;
  PtrTtAttribute      pAttr;
  PtrSRule            pRule;
  ThotBool            ok;
  int                 attrNum;

  if (c < 1000)
    /* it's an intermediate symbol */
    CompilerMessage (wi, STR, FATAL, STR_INTERMEDIATE_SYMBOL, inputLine,
                     LineNum);
  else if (c < 1100)
    /* short keyword */
    switch (c)
      {
      case CHR_59:
        /*  ;  */
        if (r == RULE_LocAttrList)
          /* end of local attribute definition */
          {
            if (CompilExtens)
              /* within the extension rule */
              pAttr = pSSchema->SsAttribute->TtAttr[CurExtensRule->
                                                    SrLocalAttr->Num[CurExtensRule->SrNLocalAttrs - 1] - 1];
            else
              /* within the structure rule */
              pAttr = pSSchema->SsAttribute->TtAttr[CurLocAttr->Num[CurNLocAttr - 1] - 1];
            if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 0)
              /* no value defined */
              CompilerMessage (wi, STR, FATAL, STR_ATTR_WITHOUT_VALUE,
                               inputLine, LineNum);
          }
        if (!CompilAttr && !CompilLocAttr)
          if (r == RULE_RuleList || r == RULE_OptDefList || r == RULE_DefList)
            {
              /* end of structure rule */
              if (CurNum > 0)
                {
                  /* the last previous name was not managed */
                  RecursLevel--;
                  /* the name appears within the rigth part of */
                  /* the lower level rule */
                  RightIdentifier (CurNum + MAX_BASIC_TYPE, wi);
                  RecursLevel++;
                  CurNum = 0;
                  CurName[0] = '\0';
                  CurNLocAttr = 0;
                  CurUnit = False;
                }
              /* end of the rigth part of the rule */
              RRight[RecursLevel - 1] = False;
	       
              CurBasicElem = 0;
              DuplicatePairRule ();
            }
        if (r == RULE_ExpList)
          {
            /* end of exported element */
            if (UnknownContent)
              /* invalid contents */
              CompilerMessage (BeginReferredTypeName, STR, FATAL,
                               STR_TYPE_UNKNOWN, inputLine, LineNum);
          }
	 
        if (r == RULE_ExceptList)
          {
            /* end of exceptions on type or attribute */
            CurExtensRule = NULL;
            ExceptType = 0;
            ExceptAttr = 0;
          }
        break;
      case CHR_61:
        /*  =  */
        if (r == RULE_Rule || r == RULE_ExtOrDef)
          Equal = True;
        else if (r == RULE_LocalAttr)
          {
            /* define the type of a local attribute */
            pAttr = pSSchema->SsAttribute->TtAttr[pSSchema->SsNAttributes - 1];
            if (pAttr->AttrType != AtEnumAttr ||
                (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues > 0))
              /* attribute already defined */
              CompilerMessage (wi, STR, FATAL,
                               STR_THAT_ATTR_HAS_ALREADY_VALUES, inputLine,
                               LineNum);
          }
        break;
      case CHR_40:
        /*  (  */
        if (r == RULE_Constr)
          {
            if (pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1]->SrConstruct ==
                CsList)
              /* define elements of a list */
              Push (wi);
          }
	 
        if (r == RULE_LocAttrList)
          /* begin of local attributes of a type */
          {
            CompilLocAttr = True;
            CurNLocAttr = 0;
          }
        break;
      case CHR_41:
        /*  )  */
        if (r == RULE_LocAttrList)
          /* end of local attributes of a type */
          {
            CompilLocAttr = False;
            if (CompilExtens)
              /* within extension rule */
              pAttr = pSSchema->SsAttribute->TtAttr[CurExtensRule->
                                                    SrLocalAttr->Num[CurExtensRule->SrNLocalAttrs - 1] - 1];
            else
              /* within structure rule */
              pAttr = pSSchema->SsAttribute->TtAttr[CurLocAttr->Num[CurNLocAttr - 1] - 1];
            if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 0)
              /* no value defined */
              CompilerMessage (wi, STR, FATAL, STR_ATTR_WITHOUT_VALUE,
                               inputLine, LineNum);
            if (CurBasicElem > 0)
              /* end of local attributes of a basic element */
              {
                pRule = pSSchema->SsRule->SrElem[CurBasicElem - 1];
                if (pRule->SrNLocalAttrs > 0)
                  /* there are local attributes for this element */
                  CompilerMessage (wi, STR, FATAL,
                                   STR_THIS_ELEM_HAS_LOCAL_ATTRS, inputLine,
                                   LineNum);
                else
                  pRule->SrNLocalAttrs = CurNLocAttr;
                CopyLocReqAttr (pRule);
                CurBasicElem = 0;
                CurNLocAttr = 0;
              }
          }
        if (r == RULE_Constr)
          {
            CurBasicElem = 0;
            if (RecursLevel > 1)
              if (pSSchema->SsRule->SrElem[CurRule[RecursLevel-2] - 1]->SrConstruct ==
                  CsList)
                {
                  /* end of the right part of the rule LIST */
                  RRight[RecursLevel - 1] = False;
                  /* fixed attributes complete */
                  RecursLevel--;
                  if (CurNum > 0)
                    {
                      RightIdentifier (CurNum + MAX_BASIC_TYPE, wi);
                      CurNum = 0;
                      CurName[0] = '\0';
                      CurNLocAttr = 0;
                      CurUnit = False;
                    }
                  DuplicatePairRule ();
                }
          }
        if (r == RULE_AttrType || r == RULE_Constr)
          /* end of a reference definition (attribute or element) */
          {
            if (FirstInPair || SecondInPair)
              /* the referred element name is preceded by First or Second */
              {
                /* look at if the referred type is already defined */
                i = 0;
                do
                  {
                    ok = strcmp (ReferredTypeName,
                                 pSSchema->SsRule->SrElem[i]->SrName) == 0;
                    /* next rule */
                    i++;
                  }
                while (!ok && i < pSSchema->SsNRules);
                if (!ok)
                  CompilerMessage (BeginReferredTypeName, STR, FATAL,
                                   STR_TYPE_UNKNOWN, inputLine, LineNum);
                else
                  if (pSSchema->SsRule->SrElem[i - 1]->SrConstruct != CsPairedElement)
                    /* it is not a paired type */
                    CompilerMessage (BeginReferredTypeName, STR, FATAL,
                                     STR_FIRST_SECOND_FORBIDDEN, inputLine,
                                     LineNum);
                  else if (SecondInPair)
                    {
                      if (r == RULE_AttrType)
                        pSSchema->SsAttribute->TtAttr[pSSchema->SsNAttributes - 1]->
                          AttrTypeRef = -(i + 1);
                      else if (pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1]->
                               SrConstruct == CsReference)
                        pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1]->
                          SrReferredType = -(i + 1);
                    }
              }
            FirstInPair = False;
            SecondInPair = False;
          }
        break;
      case CHR_44:
        /*  ,  */
        if (r == RULE_ExpList)
          /* end of a exported element */
          if (UnknownContent)
            CompilerMessage (BeginReferredTypeName, STR, FATAL,
                             STR_TYPE_UNKNOWN, inputLine, LineNum);
        /* invalid contents */
        break;
      case CHR_91:
        /*  [  */
        Minimum = True;
        break;
      case CHR_46:
        /*  .  */
        Minimum = False;
        Maximum = True;
        break;
      case CHR_93:
        /*  ]  */
        Maximum = False;
        break;
      case CHR_63:
        /*  ?  */
        if (r == RULE_FixModValue)
          {
            /* the constant value of the attribut fixe will be modified */
            if (CompilExtens)
              pRule = CurExtensRule;
            else
              pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
            pRule->SrDefAttrModif[pRule->SrNDefAttrs - 1] = True;
          }
        else
          Option = True;
        break;
      case CHR_33:
        /*  !  */
        MandatoryAttr = True;
        break;
      case CHR_42:
        /*  *  */
        if (r == RULE_min)
          pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1]->SrMinItems = 0;
        else
          pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1]->SrMaxItems = 32000;
        break;
      case CHR_43:
        /*  +  */
        if (r == RULE_DefWith)
          if (!RRight[RecursLevel - 1])
            CompilerMessage (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM,
                             inputLine, LineNum);
        break;
      case CHR_45:
        /*  -  */
        if (r == RULE_FixedValue)
          /* negative sign for the next attribute value */
          Sign = -1;
        else if (r == RULE_DefWith)
          if (!RRight[RecursLevel - 1])
            CompilerMessage (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM,
                             inputLine, LineNum);
        break;
      case CHR_58:
        /*  :  */
        break;
      }
   
  else if (c < 2000)
    /* long keyword */
    switch (c)
      {
      case KWD_STRUCTURE:
        break;
      case KWD_EXTENSION:
        pSSchema->SsExtension = True;
        pSSchema->SsNExtensRules = 0;
        pSSchema->SsExtensBlock = NULL;
        break;
      case KWD_DEFPRES:
        break;
      case KWD_ATTR /* ATTR */ :
        if (r == RULE_StructModel)
          /* begin of global attributes */
          CompilAttr = True;
        else if (r == RULE_LocAttrList)
          /* begin of local attributes of a element type */
          /* by default the local attribut is not mandatory */
          MandatoryAttr = False;
        break;
      case KWD_CONST:
        CompilAttr = False;
        break;
      case KWD_STRUCT:
        /* the first rule is the root rule */
        RootRule = True;
        Rules = True;
        CompilAttr = False;
        break;
      case KWD_EXTENS:
        /* check if we are within a schema extension */
        if (!pSSchema->SsExtension)
          CompilerMessage (wi, STR, FATAL, STR_NOT_AN_EXTENSION, inputLine,
                           LineNum);
        else
          {
            CompilAttr = False;
            CompilExtens = True;
          }
        break;
      case KWD_UNITS:
        CompilUnits = True;
        CompilAttr = False;
        CompilExtens = False;
        break;
      case KWD_EXPORT:
        if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
          CompilerMessage (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING,
                           inputLine, LineNum);
        else if (pSSchema->SsExtension)
          CompilerMessage (wi, STR, FATAL, STR_NOT_ALLOWED_IN_AN_EXTENSION,
                           inputLine, LineNum);
        else
          {
            CompilAttr = False;
            CompilExtens = False;
            CompilUnits = False;
            ChangeRules ();
            /* set the right rule numbers */
            pSSchema->SsExport = True;
          }
        break;
      case KWD_EXCEPT:
        CompilAttr = False;
        CompilExtens = False;
        CompilUnits = False;
        CurExtensRule = NULL;
        CompilExcept = True;
        ExceptExternalType = False;
        break;
      case KWD_END:
        if (r == RULE_Constr)
          {
            RecursLevel--;
            if (CurNum > 0)
              {
                RightIdentifier (CurNum + MAX_BASIC_TYPE, wi);
                CurNum = 0;
                CurName[0] = '\0';
                CurNLocAttr = 0;
                CurUnit = False;
              }
          }
        break;
      case KWD_INTEGER:
        pSSchema->SsAttribute->TtAttr[pSSchema->SsNAttributes - 1]->AttrType =
          AtNumAttr;
        break;
      case KWD_TEXT:
        if (r == RULE_BasicType)
          /* element type */
          BasicEl (CharString + 1, wi, pr);
        else
          /* attribute type */
          pSSchema->SsAttribute->TtAttr[pSSchema->SsNAttributes - 1]->AttrType =
            AtTextAttr;
        break;
      case KWD_REFERENCE:
        if (r == RULE_Constr)
          /* element constructor */
          {
            ReferenceAttr = False;	/* it's areference element */
            Equal = False;
            NewRule (wi);
            if (!error)
              {
                pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
                pRule->SrConstruct = CsReference;
                pRule->SrRefTypeNat[0] = '\0';
                /* by default, the referred element type */
                /* is defined within the schema */
                pRule->SrRefImportedDoc = False;
                pRule->SrReferredType = 0;
                /* it's a simple reference, */
                /* not an external document inclusion */
                ExternalStructContext = ElemRef;
              }
          }
        else
          /* attribut type */
          {
            ReferenceAttr = True;	/* it's a reference attribute */
            pAttr = pSSchema->SsAttribute->TtAttr[pSSchema->SsNAttributes - 1];
            pAttr->AttrType = AtReferenceAttr;
            pAttr->AttrTypeRefNature[0] = '\0';
            pAttr->AttrTypeRef = 0;
            /* by default, the referred element type */
            /* is defined within the schema */
            ExternalStructContext = AttrRef;
          }
        break;
      case KWD_ANY:
        if (r == RULE_TypeRef)
          /* type of reference */
          {
            if (ReferenceAttr)
              pSSchema->SsAttribute->TtAttr[pSSchema->SsNAttributes - 1]->AttrTypeRef = 0;
            else
              pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1]->SrReferredType = 0;
          }
        else if (r == RULE_Constr)
          /* constructor Any */
          {
            Equal = False;
            NewRule (wi);
            if (!error)
              {
                pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1]-1];
                pRule->SrConstruct = CsAny;
              }
          }
        break;
      case KWD_First:
        FirstInPair = True;
        break;
      case KWD_Second:
        SecondInPair = True;
        break;
      case KWD_GRAPHICS:
        BasicEl (GraphicElem + 1, wi, pr);
        break;
      case KWD_SYMBOL:
        BasicEl (Symbol + 1, wi, pr);
        break;
      case KWD_PICTURE:
        BasicEl (tt_Picture + 1, wi, pr);
        break;
      case KWD_UNIT:
      case KWD_NATURE:
        if (pr == RULE_ExclElem || pr == RULE_InclElem)
          CompilerMessage (wi, STR, FATAL, STR_NOT_ALLOWED_HERE, inputLine,
                           LineNum);
        else
          {
            Equal = False;
            NewRule (wi);
            if (!error)
              {
                pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
                pRule->SrConstruct = CsChoice;
                if (c == KWD_UNIT)
                  pRule->SrNChoices = 0;	/* UNIT  */
                else
                  pRule->SrNChoices = -1;	/* NATURE  */
              }
          }
        break;
      case KWD_WITH:
        /* begin of fixed attributes */
        if (r == RULE_DefWith)
          if (!RRight[RecursLevel - 1])
            CompilerMessage (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM,
                             inputLine, LineNum);
        break;
      case KWD_EXTERN:
        if (r == RULE_ExceptType)
          ExceptExternalType = True;
        else if (r == RULE_ExtOrDef)
          {
            if (NExternalTypes >= MAX_EXTERNAL_TYPES)
              /* table of external types is full */
              CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_EXTERNAL_DOCS,
                               inputLine, LineNum);
            else
              /* add into the table the new type */
              {
                NExternalTypes++;
                IncludedExternalType[NExternalTypes - 1] = False;
                strncpy (ExternalType[NExternalTypes - 1], PreviousIdent,
                         MAX_NAME_LENGTH);
                if (strcmp (PreviousIdent, pSSchema->SsRule->
                            SrElem[pSSchema->SsRootElem - 1]->SrName) == 0)
                  /* the document type is used as external */
                  {
                    /* add a SyntacticType rule at the end of the schema */
                    AllocateNewRule (pSSchema);
                    pRule = pSSchema->SsRule->SrElem[pSSchema->SsNRules - 1];
                    strncpy (pRule->SrName, PreviousIdent, MAX_NAME_LENGTH);
                    pRule->SrNLocalAttrs = 0;
                    pRule->SrLocalAttr = NULL;
                    pRule->SrRequiredAttr = NULL;
                    pRule->SrNDefAttrs = 0;
                    pRule->SrUnitElem = False;
                    pRule->SrConstruct = CsNatureSchema;
                    pRule->SrSSchemaNat = NULL;
                    pRule->SrRecursive = False;
                    pRule->SrExportedElem = False;
                    pRule->SrFirstExcept = 0;
                    pRule->SrLastExcept = 0;
                    PreviousRule = pSSchema->SsNRules;
                  }
              }
          }
        break;
      case KWD_INCLUDED:
        /* included */
        if (NExternalTypes >= MAX_EXTERNAL_TYPES)
          /* table of external types is full */
          CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_EXTERNAL_DOCS,
                           inputLine, LineNum);
        else
          /* add into the table the new external type */
          {
            IncludedExternalType[NExternalTypes] = True;
            strncpy (ExternalType[NExternalTypes], PreviousIdent,
                     MAX_NAME_LENGTH);
            NExternalTypes++;
          }
        break;
      case KWD_LIST:
        Equal = False;
        NewRule (wi);
        if (!error)
          {
            pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
            pRule->SrConstruct = CsList;
            pRule->SrMinItems = 0;
            pRule->SrMaxItems = 32000;
          }
        break;
      case KWD_OF:
        break;
      case KWD_AGGREGATE:
      case KWD_BEGIN:
        Equal = False;
        NewRule (wi);
        if (!error)
          {
            pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
            if (c == KWD_BEGIN)
              pRule->SrConstruct = CsAggregate;
            else if (c == KWD_AGGREGATE)
              pRule->SrConstruct = CsUnorderedAggregate;
            pRule->SrNComponents = 0;
            for (i = 0; i < MAX_COMP_AGG; i++)
              pRule->SrOptComponent[i] = False;
            Option = False;
            Push (wi);
          }
        break;
      case KWD_CASE:
        Equal = False;
        NewRule (wi);
        if (!error)
          {
            pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
            pRule->SrConstruct = CsChoice;
            pRule->SrNChoices = 0;
            Push (wi);
          }
        break;
      case KWD_CONSTANT:
        break;
      case KWD_PAIR:
        Equal = False;
        NewRule (wi);
        if (!error)
          {
            if (pSSchema->SsRootElem == CurRule[RecursLevel - 1])
              CompilerMessage (wi, STR, FATAL, STR_ROOT_CANNOT_BE_A_PAIR,
                               inputLine, LineNum);
            else
              {
                pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
                pRule->SrConstruct = CsPairedElement;
                pRule->SrFirstOfPair = True;
              }
          }
        break;
      case KWD_Nothing:
        /* pno contents for the current exported element */
        pRule = pSSchema->SsRule->SrElem[RuleExportWith - 1];
        pRule->SrExportContent = 0;
        if ((pRule->SrConstruct == CsChoice) && (pRule->SrNChoices >= 1))
          for (j = 0; j < pRule->SrNChoices; j++)
            pSSchema->SsRule->SrElem[pRule->SrChoice[j] - 1]->SrExportContent = 0;
        break;
      case KWD_NoCut:
        ExceptionNum (ExcNoCut, True, False, False, wi);
        break;
      case KWD_CanCut:
        ExceptionNum (ExcCanCut, True, False, False, wi);
        break;
      case KWD_IsTable:
        ExceptionNum (ExcIsTable, True, False, False, wi);
        break;
      case KWD_IsRow:
        ExceptionNum (ExcIsRow, True, False, False, wi);
        break;
      case KWD_IsColHead:
        ExceptionNum (ExcIsColHead, True, False, False, wi);
        break;
      case KWD_IsCell:
        ExceptionNum (ExcIsCell, True, False, False, wi);
        break;
      case KWD_IsCaption:
        ExceptionNum (ExcIsCaption, True, False, False, wi);
        break;
      case KWD_Shadow:
        ExceptionNum (ExcShadow, True, False, False, wi);
        break;
      case KWD_NoCreate:
        ExceptionNum (ExcNoCreate, True, False, False, wi);
        break;
      case KWD_NoReplicate:
        ExceptionNum (ExcNoReplicate, True, False, False, wi);
        break;
      case KWD_NoHMove:
        ExceptionNum (ExcNoHMove, True, False, False, wi);
        break;
      case KWD_NoVMove:
        ExceptionNum (ExcNoVMove, True, False, False, wi);
        break;
      case KWD_NoHResize:
        ExceptionNum (ExcNoHResize, True, False, False, wi);
        break;
      case KWD_NoVResize:
        ExceptionNum (ExcNoVResize, True, False, False, wi);
        break;
      case KWD_EventAttr: /* on attribute */
        ExceptionNum (ExcEventAttr, False, True, False, wi);
        break;
      case KWD_NewWidth: /* on attribute */
        ExceptionNum (ExcNewWidth, False, True, True, wi);
        break;
      case KWD_NewPercentWidth: /* on attribute */
        ExceptionNum (ExcNewPercentWidth, False, True, True, wi);
        break;
      case KWD_ColRef: /* on attribute */
        ExceptionNum (ExcColRef, False, True, False, wi);
        break;
      case KWD_ColColRef: /* on attribute */
        ExceptionNum (ExcColColRef, False, True, False, wi);
        break;
      case KWD_ColSpan: /* on attribute */
        ExceptionNum (ExcColSpan, False, True, True, wi);
        break;
      case KWD_RowSpan: /* on attribute */
        ExceptionNum (ExcRowSpan, False, True, True, wi);
        break;
      case KWD_NewHeight: /* on attribute */
        ExceptionNum (ExcNewHeight, False, True, True, wi);
        break;
      case KWD_NewHPos: /* on attribute */
        ExceptionNum (ExcNewHPos, False, True, True, wi);
        break;
      case KWD_NewVPos: /* on attribute */
        ExceptionNum (ExcNewVPos, False, True, True, wi);
        break;
      case KWD_Invisible: /* on attribute */
        ExceptionNum (ExcInvisible, False, True, False, wi);
        break;
      case KWD_NoMove:
        ExceptionNum (ExcNoMove, True, False, False, wi);
        break;
      case KWD_NoResize:
        ExceptionNum (ExcNoResize, True, False, False, wi);
        break;
      case KWD_IsDraw:
        ExceptionNum (ExcIsDraw, True, False, False, wi);
        break;
      case KWD_IsMarker:
        ExceptionNum (ExcIsMarker, True, False, False, wi);
        break;
      case KWD_UsePaintServer:
        ExceptionNum (ExcUsePaintServer, True, False, False, wi);
        break;
      case KWD_UseMarkers:
        ExceptionNum (ExcUseMarkers, True, False, False, wi);
        break;
      case KWD_IsGhost:
        ExceptionNum (ExcIsGhost, True, False, False, wi);
        break;
      case KWD_IsImg:
        ExceptionNum (ExcIsImg, True, False, False, wi);
        break;
      case KWD_IsMap:
        ExceptionNum (ExcIsMap, True, False, False, wi);
        break;
      case KWD_DuplicateAttr: /* on attribute */
        ExceptionNum (ExcDuplicateAttr, False, True, False, wi);
        break;
      case KWD_GiveName: /* on attribute */
        ExceptionNum (ExcGiveName, False, True, False, wi);
        break;
      case KWD_GiveTypes: /* on attribute */
        ExceptionNum (ExcGiveTypes, False, True, False, wi);
        break;
      case KWD_NoShowBox:
        ExceptionNum (ExcNoShowBox, True, False, False, wi);
        break;
      case KWD_MoveResize:
        ExceptionNum (ExcMoveResize, True, False, False, wi);
        break;
      case KWD_NoSelect:
        ExceptionNum (ExcNoSelect, True, False, False, wi);
        break;
      case KWD_SelectParent:
        ExceptionNum (ExcSelectParent, True, False, False, wi);
        break;
      case KWD_NoSpellCheck:
        ExceptionNum (ExcNoSpellCheck, True, False, False, wi);
        break;
      case KWD_GraphCreation:
        /* GraphCreation */
        ExceptionNum (ExcGraphCreation, True, False, False, wi);
        break;
      case KWD_Hidden:
        /* Hidden */
        ExceptionNum (ExcHidden, True, False, False, wi);
        break;
      case KWD_PseudoParag:
        /* PseudoParag */
        ExceptionNum (ExcPseudoParag, True, False, False, wi);
        break;
      case KWD_PageBreak:
        ExceptionNum (ExcPageBreak, True, False, False, wi);
        break;
      case KWD_PageBreakAllowed:
        ExceptionNum (ExcPageBreakAllowed, True, False, False, wi);
        break;
      case KWD_PageBreakPlace:
        ExceptionNum (ExcPageBreakPlace, True, False, False, wi);
        break;
      case KWD_PageBreakRepetition:
        ExceptionNum (ExcPageBreakRepetition, True, False, False, wi);
        break;
      case KWD_PageBreakRepBefore:
        ExceptionNum (ExcPageBreakRepBefore, True, False, False, wi);
        break;
      case KWD_ActiveRef: /* on attribute */
        ExceptionNum (ExcActiveRef, False, True, False, wi);
        break;
      case KWD_NoPaginate:
        ExceptionNum (ExcNoPaginate, True, False, False, wi);
        break;
      case KWD_ImportLine:
        if (ImportExcept)
          CompilerMessage (wi, STR, FATAL, STR_ONLY_ONE_IMPORT_EXCEPTION,
                           inputLine, LineNum);
        else
          {
            ExceptionNum (ExcImportLine, True, False, False, wi);
            ImportExcept = True;
          }
        break;
      case KWD_ImportParagraph:
        if (ImportExcept)
          CompilerMessage (wi, STR, FATAL, STR_ONLY_ONE_IMPORT_EXCEPTION,
                           inputLine, LineNum);
        else
          {
            ExceptionNum (ExcImportParagraph, True, False, False, wi);
            ImportExcept = True;
          }
        break;
      case KWD_HighlightChildren:
        ExceptionNum (ExcHighlightChildren, True, False, False, wi);
        break;
      case KWD_ExtendedSelection:
        ExceptionNum (ExcExtendedSelection, True, False, False, wi);
        break;
      case KWD_ClickableSurface:
        ExceptionNum (ExcClickableSurface, True, False, False, wi);
        break;
      case KWD_ReturnCreateNL:
        ExceptionNum (ExcReturnCreateNL, True, False, False, wi);
        break;
      case KWD_ReturnCreateWithin:
        ExceptionNum (ExcReturnCreateWithin, True, False, False, wi);
        break;
      case KWD_NoBreakByReturn:
        ExceptionNum (ExcNoBreakByReturn, True, False, False, wi);
        break;
      case KWD_ParagraphBreak:
        ExceptionNum (ExcParagraphBreak, True, False, False, wi);
        break;
      case KWD_IsBreak:
        ExceptionNum (ExcIsBreak, True, False, False, wi);
        break;
      case KWD_ListItemBreak:
        ExceptionNum (ExcListItemBreak, True, False, False, wi);
        break;
      case KWD_CssBackground:
        ExceptionNum (ExcCssBackground, True, False, False, wi);
        break;
      case KWD_CssClass: /* on attribute */
        ExceptionNum (ExcCssClass, False, True, False, wi);
        break;
      case KWD_CssId: /* on attribute */
        ExceptionNum (ExcCssId, False, True, False, wi);
        break;
      case KWD_CssPseudoClass: /* on attribute */
        ExceptionNum (ExcCssPseudoClass, False, True, False, wi);
        break;
      case KWD_SetWindowBackground:
        ExceptionNum (ExcSetWindowBackground, True, False, False, wi);
        break;
      case KWD_IsGroup:
        ExceptionNum (ExcIsGroup, True, False, False, wi);
        break;
      case KWD_EmptyGraphic:
        ExceptionNum (ExcEmptyGraphic, True, False, False, wi);
        break;
      case KWD_SpacePreserve: /* on attribute */
        ExceptionNum (ExcSpacePreserve, False, True, False, wi);
        break;
      case KWD_IsPlaceholder:
        ExceptionNum (ExcIsPlaceholder, True, False, False, wi);
        break;
      case KWD_StartCounter:
        ExceptionNum (ExcStartCounter, False, True, True, wi);
        break;
      case KWD_SetCounter:
        ExceptionNum (ExcSetCounter, False, True, True, wi);
        break;
      case KWD_MarkupPreserve:
        /* Preserve <, >, & */
        ExceptionNum (ExcMarkupPreserve, True, False, False, wi);
        break;
      case KWD_NotAnElementNode:
        /* this Thot element is not a DOM element node */
        ExceptionNum (ExcNotAnElementNode, True, False, False, wi);
        break;
      case KWD_CheckAllChars:
        /* check any inserted character */
        ExceptionNum (ExcCheckAllChars, True, False, False, wi);
        break;
      case KWD_NewRoot:
        /* Manage that node as a root element */
        ExceptionNum (ExcNewRoot, True, False, False, wi);
        break;
      case KWD_Root:
        CurExtensRule = NewExtensionRule (wi, 0);
        CurExtensRule->SrName[0] = EOS;
        break;
        /* end of case c */
      }
   
  else
    /* basic type */
    switch (c)
      {
      case 3001:
        /* a name */
        /* r = rule number where the name appears */
        switch (r)
          {
          case RULE_ElemName /* VarElemName */ :
            if (pr == RULE_StructModel)
              /* after the keyword 'STRUCTURE' */
              /* keep the structure name */
              {
                pSSchema->SsName = (char *)TtaGetMemory (wl + 1);
                strncpy (pSSchema->SsName, (char *)&inputLine[wi - 1], wl);
                pSSchema->SsName[wl] = '\0';
                /* compare this name with the file name */
		 
                if (strcmp (pSSchema->SsName, srceFileName) != 0)
                  /* names are different */
                  CompilerMessage (wi, STR, FATAL,
                                   STR_FILE_NAME_AND_STRUCT_NAME_DIFFERENT,
                                   inputLine, LineNum);
              }
            if (pr == RULE_Rule)
              /* begin of the first level rule */
              {
                CopyWord (CurName, wi, wl);
                CurNum = nb;
                if (CompilUnits)
                  CurUnit = True;
                else
                  CurUnit = False;
              }
            if (pr == RULE_RootOrElem)
              /* begin of extension rule */
              CurExtensRule = NewExtensionRule (wi, wl);
            if ((pr == RULE_TypeRef && !ReferenceAttr) || pr == RULE_Element)
              {
                if (pr == RULE_Element)
                  CopyWord (PreviousIdent, wi, wl);
                /* keep the type name in case there is an extern after */
                if (Equal)
                  {
                    NewRule (wi);
                    Equal = False;
                  }
                if (!error)
                  {
                    if (RRight[RecursLevel - 1])
                      /* the rigth part identifier of the rule */
                      {
                        RightIdentifier (nb + MAX_BASIC_TYPE, wi);
                        CopyWord (ReferredTypeName, wi, wl);
                        /* keep the referred type in case */
                        /* it's defined in another structure */
                        BeginReferredTypeName = wi;
                      }
                    else
                      /* the left part identifier of the rule */
                      {
                        CopyWord (CurName, wi, wl);
                        CurNum = nb;
                      }
                  }
              }
            if (pr == RULE_TypeRef && ReferenceAttr)
              /* within a reference attribute */
              {
                pSSchema->SsAttribute->TtAttr[pSSchema->SsNAttributes - 1]->AttrTypeRef
                  = nb + MAX_BASIC_TYPE;
                CopyWord (ReferredTypeName, wi, wl);
                /* keep the referred type name in case it's */
                /* defined in another structure schema */
                BeginReferredTypeName = wi;
              }
            if (pr == RULE_ExtStruct)
              /* external structure name within reference or */
              /* within exported element contents */
              {
                CopyWord (N, wi, wl);
                /* keep the external schema name */
                /* and read the external structure schema */
                if (!ReadStructureSchema (N, pExternSSchema))
                  /* cannot read the schema */
                  CompilerMessage (wi, STR, FATAL,
                                   STR_CANNOT_READ_STRUCT_SCHEM, inputLine,
                                   LineNum);
                else
                  /* structure schema loaded */
                  /* search the referred element within */
                  {
                    i = 0;
                    while (strcmp (ReferredTypeName,
                                   pExternSSchema->SsRule->SrElem[i]->SrName) != 0 &&
                           i - 1 < pExternSSchema->SsNRules)
                      i++;
                    if (strcmp (ReferredTypeName,
                                pExternSSchema->SsRule->SrElem[i]->SrName) != 0)
                      /* unknown type */
                      CompilerMessage (BeginReferredTypeName, STR, FATAL,
                                       STR_TYPE_UNKNOWN, inputLine, LineNum);
                    else
                      /* the referred type is found: number i+1 */
                      switch (ExternalStructContext)
                        {
                        case AttrRef:
                          /* within reference attribute */
                          pAttr = pSSchema->SsAttribute->TtAttr[pSSchema->
                                                                SsNAttributes - 1];
                          if (SecondInPair)
                            pAttr->AttrTypeRef = i + 2;
                          else
                            pAttr->AttrTypeRef = i + 1;
                          strncpy (pAttr->AttrTypeRefNature, N,
                                   MAX_NAME_LENGTH);
                          FirstInPair = False;
                          SecondInPair = False;
                          break;
                        case ElemRef:
                          /* within construction CsReference */
                          pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel-1]-1];
                          if (SecondInPair)
                            pRule->SrReferredType = i + 2;
                          else
                            pRule->SrReferredType = i + 1;
                          strncpy (pRule->SrRefTypeNat, N, MAX_NAME_LENGTH);
                          FirstInPair = False;
                          SecondInPair = False;
                          break;
                        case ElContent:
                          /* within exported element contents */
                          pRule = pSSchema->SsRule->SrElem[RuleExportWith - 1];
                          pRule->SrExportContent = i + 1;
                          strncpy (pRule->SrNatExpContent, N,
                                   MAX_NAME_LENGTH);
                          UnknownContent = False;
                          if (pRule->SrConstruct == CsChoice)
                            if (pRule->SrNChoices >= 1)
                              for (j = 0; j < pRule->SrNChoices; j++)
                                {
                                  pSSchema->SsRule->SrElem[pRule->SrChoice[j] - 1]->
                                    SrExportContent = i + 1;
                                  strncpy (pSSchema->SsRule->SrElem[pRule->
                                                                    SrChoice[j] - 1]->SrNatExpContent,
                                           N, MAX_NAME_LENGTH);
                                }
                          break;
                        }
                  }
              }
            if (pr == RULE_Export)
              /* exported element name */
              /* check if the type is already declared */
              {
                i = RuleNumber (wl, wi);
                if (i == 0)
                  /* not declared */
                  CompilerMessage (wi, STR, FATAL, STR_TYPE_UNKNOWN,
                                   inputLine, LineNum);
                else
                  {
                    pRule = pSSchema->SsRule->SrElem[i - 1];
                    /* type exists */
                    pRule->SrExportedElem = True;
                    RuleExportWith = i;
                    pRule->SrExportContent = i;
                    /* by default push the whole contents */
                    pRule->SrNatExpContent[0] = '\0';
                    ExternalStructContext = ElContent;
                    /* choice exported, all options */
                    /* are also exported */
                    if (pRule->SrConstruct == CsChoice)
                      if (pRule->SrNChoices >= 1)
                        for (j = 0; j < pRule->SrNChoices; j++)
                          {
                            pSSchema->SsRule->SrElem[pRule->SrChoice[j] - 1]->
                              SrExportedElem = True;
                            pSSchema->SsRule->SrElem[pRule->SrChoice[j] - 1]->
                              SrExportContent = pRule->SrChoice[j];
                            pSSchema->SsRule->SrElem[pRule->SrChoice[j] - 1]->
                              SrNatExpContent[0] = '\0';
                          }
                    /* the first mark of the paired type is exported, */
                    /* the second one is also exported */
                    if (pRule->SrConstruct == CsPairedElement)
                      if (pRule->SrFirstOfPair)
                        {
                          pSSchema->SsRule->SrElem[i]->SrExportedElem = True;
                          pSSchema->SsRule->SrElem[i]->SrExportContent = i+1;
                          pSSchema->SsRule->SrElem[i]->SrNatExpContent[0] = '\0';
                        }
                  }
              }
            if (pr == RULE_Content)
              /* the content of a exported element */
              {
                CopyWord (ReferredTypeName, wi, wl);
                /* keep the exported type name in case */
                /* it's defined in another structure schema */
                BeginReferredTypeName = wi;
                i = RuleNumber (wl, wi);
                if (i == 0)
                  /* unknown type */
                  /* the contents can be defined in another */
                  /* schema, wait for next external schemas */
                  UnknownContent = True;
                else
                  /* it's a type defined in the schema */
                  if (pSSchema->SsRule->SrElem[i - 1]->SrConstruct != CsNatureSchema)
                    {
                      pRule = pSSchema->SsRule->SrElem[RuleExportWith - 1];
                      pRule->SrExportContent = i;
                      if (pRule->SrConstruct == CsChoice)
                        if (pRule->SrNChoices >= 1)
                          for (j = 0; j < pRule->SrNChoices; j++)
                            pSSchema->SsRule->SrElem[pRule->SrChoice[j] - 1]->
                              SrExportContent = i;
                      /* the first mark of the paired type is exported, */
                      /* the second one is also exported */
                      if (pRule->SrConstruct == CsPairedElement)
                        if (pRule->SrFirstOfPair)
                          {
                            if (i == RuleExportWith)
                              /* the contents of the second mark */
                              /* is the mark itself */
                              pSSchema->SsRule->SrElem[RuleExportWith]->SrExportContent =
                                i + 1;
                            else
                              pSSchema->SsRule->SrElem[RuleExportWith]->SrExportContent =
                                i;
                          }
                    }
                  else
                    /* the object contents depends on another */
                    /* structure schema, read the schema */
                    if (!ReadStructureSchema (pSSchema->SsRule->SrElem[i - 1]->SrName,
                                              pExternSSchema))
                      /* cannot load the schema */
                      CompilerMessage (wi, STR, FATAL,
                                       STR_CANNOT_READ_STRUCT_SCHEM,
                                       inputLine, LineNum);
                    else
                      /* structure schema loaded, the contents */
                      /* the root element of this schema */
                      {
                        pRule = pSSchema->SsRule->SrElem[RuleExportWith - 1];
                        pRule->SrExportContent = pExternSSchema->SsRootElem;
                        strncpy (pRule->SrNatExpContent,
                                 pExternSSchema->SsName, MAX_NAME_LENGTH);
                        if (pRule->SrConstruct == CsChoice)
                          if (pRule->SrNChoices >= 1)
                            for (j = 0; j < pRule->SrNChoices; j++)
                              {
                                pSSchema->SsRule->SrElem[pRule->SrChoice[j] - 1]->
                                  SrExportContent =pExternSSchema->SsRootElem;
                                strncpy (pSSchema->SsRule->SrElem[pRule->
                                                                  SrChoice[j] - 1]->SrNatExpContent,
                                         pExternSSchema->SsName,
                                         MAX_NAME_LENGTH);
                              }
                      }
              }
            if (pr == RULE_ExceptType)
              /* element type name within exceptions */
              {
                if (ExceptExternalType)
                  {
                    ExceptExternalType = False;
                    /* search an existing extension for this type */
                    CurExtensRule = GetExtensionRule (wi, wl);
                    if (CurExtensRule == NULL)
                      /* not found, create an extension rule */
                      CurExtensRule = NewExtensionRule (wi, wl);
                    ExceptType = 1;
                  }
                else
                  {
                    ExceptType = RuleNumber (wl, wi);
                    if (ExceptType == 0)
                      /* it's not declared within the schema */
                      /* can be an attribute */
                      {
                        ExceptAttr = AttributeNumber (wl, wi);
                        if (ExceptAttr == 0)
                          CompilerMessage (wi, STR, FATAL,
                                           STR_TYPE_OR_ATTR_UNKNOWN, inputLine,
                                           LineNum);
                        else if (pSSchema->SsAttribute->TtAttr[ExceptAttr - 1]->
                                 AttrFirstExcept != 0)
                          CompilerMessage (wi, STR, FATAL,
                                           STR_THIS_ATTR_ALREADY_HAS_EXCEPTS,
                                           inputLine, LineNum);
                      }
                    else
                      /* it's a declared type */
                      {
                        if (FirstInPair || SecondInPair)
                          /* the element name is preceded by First or Second */
                          {
                            if (pSSchema->SsRule->SrElem[ExceptType - 1]->SrConstruct !=
                                CsPairedElement)
                              /* it's not a  paired type */
                              CompilerMessage (wi, STR, FATAL,
                                               STR_FIRST_SECOND_FORBIDDEN,
                                               inputLine, LineNum);
                            else if (SecondInPair)
                              ExceptType++;
                          }
                        if (pSSchema->SsRule->SrElem[ExceptType - 1]->SrFirstExcept != 0)
                          CompilerMessage (wi, STR, FATAL,
                                           STR_THIS_TYPE_ALREADY_HAS_EXCEPTS,
                                           inputLine, LineNum);
                      }
                    FirstInPair = False;
                    SecondInPair = False;
                  }
              }
            if (pr == RULE_InclElem)
              {
                if (CompilExtens)
                  pRule = CurExtensRule;
                else
                  pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
                if (pRule->SrNInclusions >= MAX_INCL_EXCL_SRULE)
                  CompilerMessage (wi, STR, FATAL,
                                   STR_TOO_MANY_INCLS_FOR_THAT_ELEM,
                                   inputLine, LineNum);
                else
                  {
                    pRule->SrNInclusions++;
                    pRule->SrInclusion[pRule->SrNInclusions - 1] =
                      nb + MAX_BASIC_TYPE;
                  }
              }
            if (pr == RULE_ExclElem)
              {
                if (CompilExtens)
                  pRule = CurExtensRule;
                else
                  pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
                if (pRule->SrNExclusions >= MAX_INCL_EXCL_SRULE)
                  CompilerMessage (wi, STR, FATAL,
                                   STR_TOO_MANY_EXCLUSIONS_FOR_THAT_ELEM,
                                   inputLine, LineNum);
                else
                  {
                    pRule->SrNExclusions++;
                    pRule->SrExclusion[pRule->SrNExclusions - 1] =
                      nb + MAX_BASIC_TYPE;
                  }
              }
            break;
          case RULE_PresentName:
            /* PresentName */
            if (pSSchema->SsDefaultPSchema)
              TtaFreeMemory (pSSchema->SsDefaultPSchema);
            pSSchema->SsDefaultPSchema = (char *)TtaGetMemory (wl + 1);
            strncpy (pSSchema->SsDefaultPSchema, (char *)&inputLine[wi - 1], wl);
            pSSchema->SsDefaultPSchema[wl] = '\0';
            break;
          case RULE_AttrName:
            /* AttrName */
            attrNum = Identifier[nb - 1].SrcIdentDefRule;
            if (attrNum == 0)
              /* new name within the schema */
              if (strncmp ((char *)&inputLine[wi - 1],
                           (char *)pSSchema->SsAttribute->TtAttr[0]->AttrName, wl) == 0)
                /* it's the language attribute */
                attrNum = 1;
            if (CompilAttr || CompilLocAttr)
              /* attribute declaration */
              if (CompilAttr && attrNum > 0)
                /* try to redefine the attribute */
                CompilerMessage (wi, STR, FATAL, STR_ATTR_ALREADY_DECLARED,
                                 inputLine, LineNum);
              else
                {
                  if (attrNum > 0)
                    {
                      /* attribute already defined */
                      if (pSSchema->SsAttribute->TtAttr[attrNum - 1]->AttrGlobal &&
                          !MandatoryAttr)
                        /* the global attribute cannot be used as local
                           attribute, except if it's to make it mandatory */
                        CompilerMessage (wi, STR, FATAL, STR_GLOBAL_ATTR,
                                         inputLine, LineNum);
                    }
                  else
                    /* new attribute */
                    {
                      if (pSSchema->SsNAttributes >= pSSchema->SsAttrTableSize)
                        /* the attribute table is full. Extend it */
                        {
                          /* add 10 new entries */
                          size = pSSchema->SsNAttributes + 10;
                          i = size * sizeof (PtrTtAttribute);
                          pSSchema->SsAttribute = (TtAttrTable*) realloc (pSSchema->SsAttribute, i);
                          if (!pSSchema->SsAttribute)
                            TtaDisplaySimpleMessage (FATAL, STR,
                                                     STR_NOT_ENOUGH_MEM);
                          else
                            {
                              pSSchema->SsAttrTableSize = size;
                              for (i = pSSchema->SsNAttributes; i < size; i++)
                                pSSchema->SsAttribute->TtAttr[i] = NULL;
                            }
                        }
                      /* create a new attribute descriptor */
                      pAttr = (PtrTtAttribute) malloc (sizeof (TtAttribute));
                      if (pAttr == NULL)
                        TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);
                      else
                        {
                          pSSchema->SsAttribute->TtAttr[pSSchema->SsNAttributes] = pAttr;
                          pAttr->AttrName = (char *)TtaGetMemory (MAX_NAME_LENGTH);   
                          pAttr->AttrOrigName = (char *)TtaGetMemory (MAX_NAME_LENGTH);   
                          CopyWord (pAttr->AttrName, wi, wl);
                          pAttr->AttrOrigName[0] = '\0';
                          pAttr->AttrGlobal = !CompilLocAttr;
                          pAttr->AttrFirstExcept = 0;
                          pAttr->AttrLastExcept = 0;
                          pAttr->AttrType = AtEnumAttr;
                          pAttr->AttrNEnumValues = 0;
                          pSSchema->SsNAttributes++;
                          Identifier[nb - 1].SrcIdentDefRule =
                            pSSchema->SsNAttributes;
                        }
                    }
                  if (CompilLocAttr)
                    /* local attribute */
                    {
                      if (CurNLocAttr >= LocAttrTableSize)
                        {
                          LocAttrTableSize += 20;
                          CurLocAttr = (NumTable*) realloc (CurLocAttr,
                                                            LocAttrTableSize * sizeof (int));
                          CurReqAttr = (BoolTable*) realloc (CurReqAttr,
                                                             LocAttrTableSize * sizeof (ThotBool));
                        }
                      if (CompilExtens)
                        /* within extension rule */
                        {
                          CurExtensRule->SrNLocalAttrs++;
                          CurExtensRule->SrLocalAttr->Num[CurExtensRule->
                                                          SrNLocalAttrs - 1] = pSSchema->SsNAttributes;
                          CurExtensRule->SrRequiredAttr->Bln[CurExtensRule->
                                                             SrNLocalAttrs - 1] = MandatoryAttr;
                          MandatoryAttr = False;
                        }
                      else
                        /* within structure rule */
                        {
                          CurLocAttr->Num[CurNLocAttr] =
                            Identifier[nb - 1].SrcIdentDefRule;
                          CurReqAttr->Bln[CurNLocAttr] = MandatoryAttr;
                          CurNLocAttr++;
                          MandatoryAttr = False;
                        }
                    }
                }
            else if (CompilExcept)
              /* attribute name within exceptions set */
              {
                ExceptAttr = AttributeNumber (wl, wi);
                /* number of this attribute */
                if (ExceptAttr == 0)
                  /* it's an attribute declared in the schema */
                  /* it can be an element type */
                  {
                    ExceptType = RuleNumber (wl, wi);
                    if (ExceptType == 0)
                      CompilerMessage (wi, STR, FATAL,
                                       STR_TYPE_OR_ATTR_UNKNOWN, inputLine,
                                       LineNum);
                    else
                      if (pSSchema->SsRule->SrElem[ExceptType - 1]->SrFirstExcept != 0)
                        CompilerMessage (wi, STR, FATAL,
                                         STR_THIS_TYPE_ALREADY_HAS_EXCEPTS,
                                         inputLine, LineNum);
                  }
                else
                  if (pSSchema->SsAttribute->TtAttr[ExceptAttr - 1]->AttrFirstExcept !=
                      0)
                    CompilerMessage (wi, STR, FATAL,
                                     STR_THIS_ATTR_ALREADY_HAS_EXCEPTS,
                                     inputLine, LineNum);
              }
            else
              /* attribute used within a rule with the WITH keyword */
              if (attrNum == 0)
                CompilerMessage (wi, STR, FATAL, STR_ATTR_NOT_DECLARED,
                                 inputLine, LineNum);
              else
                {
                  if (CompilExtens)
                    pRule = CurExtensRule;
                  else
                    pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
                  if (pRule->SrNDefAttrs >= MAX_DEFAULT_ATTR)
                    CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_ATTRS,
                                     inputLine, LineNum);
                  else
                    {
                      pRule->SrNDefAttrs++;
                      pRule->SrDefAttr[pRule->SrNDefAttrs - 1] = attrNum;
                      pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] = 1;
                      /* by default, the initial value is not modifiable */
                      pRule->SrDefAttrModif[pRule->SrNDefAttrs - 1] = False;
                    }
                }
            break;
          case RULE_AttrValue:
            /* AttrValue */
            if (CompilAttr || CompilLocAttr)
              /* attribute value definition for the current */
              /* attribute (the last one for the moment) */
              {
                pAttr = pSSchema->SsAttribute->TtAttr[pSSchema->SsNAttributes - 1];
                if (pAttr->AttrNEnumValues >= MAX_ATTR_VAL)
                  /* overflow of values list */
                  CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_VALUES,
                                   inputLine, LineNum);
                else
                  /* new value for the attribute */
                  {
                    pAttr->AttrNEnumValues++;
                    /* add the new value into the list */
                    CopyWord (pAttr->AttrEnumValue[pAttr->AttrNEnumValues-1],
                              wi, wl);
                    /* check if the value already exists */
                    i = 1;
                    while (i < pAttr->AttrNEnumValues && !error)
                      if (!strcmp (pAttr->AttrEnumValue[i - 1],
                                   pAttr->AttrEnumValue[pAttr->AttrNEnumValues - 1]))
                        /* the same value two times */
                        CompilerMessage (wi, STR, FATAL,
                                         STR_VALUE_ALREADY_DECLARED,
                                         inputLine, LineNum);
                      else
                        i++;
                    if (!error)
                      /* it's a new value */
                      {
                        Identifier[nb - 1].SrcIdentDefRule =
                          pAttr->AttrNEnumValues;
                        Identifier[nb - 1].SrcIdentRefRule =
                          pSSchema->SsNAttributes;
                      }
                  }
              }
            else
              /* used within a rule with the With keyword */
              if (Identifier[nb - 1].SrcIdentDefRule == 0)
                CompilerMessage (wi, STR, FATAL, STR_ATTR_VALUE_NOT_DECLARED,
                                 inputLine, LineNum);
              else
                {
                  if (CompilExtens)
                    pRule = CurExtensRule;
                  else
                    pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
                  pAttr = pSSchema->SsAttribute->TtAttr[pRule->
                                                        SrDefAttr[pRule->SrNDefAttrs - 1] - 1];
                  if (pAttr->AttrType != AtEnumAttr)
                    CompilerMessage (wi, STR, FATAL,
                                     STR_INVALID_VALUE_FOR_THAT_ATTR,
                                     inputLine, LineNum);
                  else
                    {
                      /* check if the value already exists */
                      i = 1;
                      ok = False;
                      CopyWord (N, wi, wl);
                      while (i <= pAttr->AttrNEnumValues && !ok)
                        if (strcmp (N, pAttr->AttrEnumValue[i - 1]) == 0)
                          ok = True;
                        else
                          i++;
                      if (!ok)
                        CompilerMessage (wi, STR, FATAL,
                                         STR_INVALID_VALUE_FOR_THAT_ATTR,
                                         inputLine, LineNum);
                      else
                        pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] = i;
                    }
                }
            break;
          case RULE_ConstName /* ConstName */ :
            if (!Rules)
              /* constant definition */
              {
                CopyWord (CurName, wi, wl);
                CurNum = nb;
                CurUnit = False;
                NewRule (wi);
                /* cree une regle */
                if (!error)
                  {
                    pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
                    pRule->SrConstruct = CsConstant;
                    pRule->SrIndexConst = TextConstPtr;
                  }
              }
            else
              if (Identifier[nb - 1].SrcIdentDefRule == 0)
                /* constant used within an undefined rule */
                CompilerMessage (wi, STR, FATAL, STR_CONSTANT_NOT_DECLARED,
                                 inputLine, LineNum);
              else if (pr == RULE_Constr)
                /* constant used within structure rule */
                BasicEl (nb + MAX_BASIC_TYPE, wi, pr);
              else if (pr == RULE_ExceptType)
                /* constant within exceptions set */
                ExceptType = RuleNumber (wl, wi);
	     
            if (ExceptType > 0 &&
                pSSchema->SsRule->SrElem[ExceptType - 1]->SrFirstExcept != 0)
              CompilerMessage (wi, STR, FATAL,
                               STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine,
                               LineNum);
            break;
          default:
            break;
          }
        break;
      case 3002:
        /* a number */
        SynInteger = AsciiToInt (wi, wl);
        if (r == RULE_Integer)
          /* check min. and max. numbers in a list */
          {
            pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
            if (Minimum)
              /* min */
              pRule->SrMinItems = SynInteger;
            else if (Maximum)
              /* max */
              {
                pRule->SrMaxItems = SynInteger;
                if (pRule->SrMaxItems < pRule->SrMinItems)
                  CompilerMessage (wi, STR, FATAL,
                                   STR_MAXIMUM_LOWER_THAN_MINIMUM, inputLine,
                                   LineNum);
              }
          }
        if (r == RULE_NumValue)
          /* attribute value fixed */
          {
            if (CompilExtens)
              pRule = CurExtensRule;
            else
              pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
            if (pSSchema->SsAttribute->TtAttr[pRule->SrDefAttr
                                              [pRule->SrNDefAttrs - 1] - 1]->AttrType != AtNumAttr)
              CompilerMessage (wi, STR, FATAL,
                               STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine,
                               LineNum);
            else
              {
                pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] =
                  SynInteger * Sign;
                /* default sign for the value */
                Sign = 1;
              }
          }
        if (r == RULE_ExceptNum)
          /* exception number associated with element type */
          /* or attribute */
          {
            if (SynInteger <= 100)
              /* values less than 100 are reserved for predefined */
              /* exceptions */
              CompilerMessage (wi, STR, FATAL,
                               STR_THIS_NUMBER_MUST_BE_GREATER_THAN_100,
                               inputLine, LineNum);
            else
              ExceptionNum (SynInteger, False, False, False, wi);
          }
        break;
      case 3003:
        /* characters string */
        if (r == RULE_ConstValue)
          /* constant string */
          StoreConstText (wi, wl);
        else if (r == RULE_StrValue)
          /* text value for textual attribute */
          {
            if (CompilExtens)
              pRule = CurExtensRule;
            else
              pRule = pSSchema->SsRule->SrElem[CurRule[RecursLevel - 1] - 1];
            if (pSSchema->SsAttribute->TtAttr[pRule->SrDefAttr
                                              [pRule->SrNDefAttrs - 1] - 1]->AttrType != AtTextAttr)
              CompilerMessage (wi, STR, FATAL,
                               STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine,
                               LineNum);
            else
              pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] =
                StoreConstText (wi, wl);
          }
        break;
      }
}

/*----------------------------------------------------------------------
  ExternalTypes sets as external all elements stored in the external
  elements table of the structure schema.
  ----------------------------------------------------------------------*/
static void         ExternalTypes ()
{
  int                 i, j;
  SRule              *pRule;

  /* go through the table of external types */
  for (j = 0; j < NExternalTypes; j++)
    /* search the external type in the schema starting from the end */
    {
      i = pSSchema->SsNRules - 1;
      while (strcmp (ExternalType[j], pSSchema->SsRule->SrElem[i]->SrName) != 0)
        i--;
      /* the external type is defined by the i rule number */
      pRule = pSSchema->SsRule->SrElem[i];
      if (pRule->SrConstruct != CsNatureSchema)
        /* it's not an external SyntacticType */
        {
          if (IncludedExternalType[j])
            CompilerMessageString (0, STR, INFO, STR_CANNOT_BE_INCLUDED,
                                   (char *)inputLine, LineNum, pRule->SrName);
          else
            CompilerMessageString (0, STR, INFO, STR_CANNOT_BE_EXTERN,
                                   (char *)inputLine, LineNum, pRule->SrName);
        }
      else
        /* modify the CsNatureSchema rule */
        {
          /* don't read the schema if it is the schema currently compiled */
          if (strcmp(pRule->SrName, pSSchema->SsName) == 0)
            pRule->SrReferredType = pSSchema->SsRootElem;
          else
            if (!ReadStructureSchema (pRule->SrName, pExternSSchema))
              {
                /* cannot read the external schema */
                CompilerMessageString (0, STR, INFO,
                                       STR_EXTERNAL_STRUCT_NOT_FOUND,
                                       (char *)inputLine, LineNum, pRule->SrName);
                /* even if the external schema doesn't exist,
                   modify the rule */
                pRule->SrReferredType = MAX_BASIC_TYPE + 1;
              }
            else
              pRule->SrReferredType = pExternSSchema->SsRootElem;
          /* change the rule into CsReference rule */
          strncpy (pRule->SrRefTypeNat, pRule->SrName, MAX_NAME_LENGTH);
          pRule->SrRefImportedDoc = True;
          pRule->SrConstruct = CsReference;
        }
    }
}

/*----------------------------------------------------------------------
  CheckRecursivity                                                
  ----------------------------------------------------------------------*/
static void         CheckRecursivity (int r, int path[], int level,
                                      ThotBool busy[], ThotBool done[])
{
  int                 m;
  PtrSRule            pRule;

  pRule = pSSchema->SsRule->SrElem[r - 1];
  /* don't work, if the element is already known */
  if (done[r])
    return;
  /* if the element is `busy', it's a recursive element;
     it's necessarily within the stack and all following stacked
     elements are also recursive.
     Mark all of them. */
  if (busy[r])
    {
      for (m = level - 1; m >= 0; m--)
        {
          pSSchema->SsRule->SrElem[path[m] - 1]->SrRecursive = True;
          if (path[m] == r)
            break;
        }
      return;
    }
  busy[r] = True;
  path[level] = r;
  switch (pRule->SrConstruct)
    {
    case CsNatureSchema:
    case CsBasicElement:
    case CsReference:
    case CsConstant:
    case CsPairedElement:
      break;
    case CsChoice:
      for (m = 0; m < pRule->SrNChoices; m++)
        CheckRecursivity (pRule->SrChoice[m], path, level + 1, busy, done);
      break;
    case CsIdentity:
      CheckRecursivity (pRule->SrIdentRule, path, level + 1, busy, done);
      break;
    case CsList:
      CheckRecursivity (pRule->SrListItem, path, level + 1, busy, done);
      break;
    case CsUnorderedAggregate:
    case CsAggregate:
      for (m = 0; m < pRule->SrNComponents; m++)
        CheckRecursivity (pRule->SrComponent[m], path, level + 1, busy, done);
      break;
    default:
      break;
    }
  busy[r] = False;
  /* element explored, work is done */
  done[r] = True;
}

/*----------------------------------------------------------------------
  ChkRecurs                                                       
  ----------------------------------------------------------------------*/
static void         ChkRecurs ()
{
  int                i;
  int                path[100];
  ThotBool           *busy, *done;
  PtrSRule           pRule;

  busy = (ThotBool*) malloc ((pSSchema->SsNRules+1) * sizeof (ThotBool));
  done = (ThotBool*) malloc ((pSSchema->SsNRules+1) * sizeof (ThotBool));
  for (i = 0; i <= pSSchema->SsNRules; i++)
    busy[i] = done[i] = False;

  for (i = MAX_BASIC_TYPE + 1; i <= pSSchema->SsNRules; i++)
    CheckRecursivity (i, path, 0, busy, done);

  for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
    {
      pRule = pSSchema->SsRule->SrElem[i];
      if (pRule->SrRecursive)
        TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_RECURSIVE_ELEM),
                           pRule->SrName);
    }
  free (busy);
  free (done);
}

/*----------------------------------------------------------------------
  ListAliasesAndNotCreated
  Lists elements considered as aliases and then
  lists elements which will not be created by the editor.
  ----------------------------------------------------------------------*/
static void         ListAliasesAndNotCreated ()
{
  int          NAlias;	   /* number of aliases defined in the schema*/
  int          *Alias; /* rules which define aliases */
  int          r, rr, i;
  ThotBool     temp;
  PtrSRule     pRule, pRule2;

  /* Liste les elements consideres comme des alias et verifie les paires */
  Alias = (int*) malloc (pSSchema->SsNRules * sizeof (int));
  NAlias = 0;
  for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
    {
      if (pSSchema->SsRule->SrElem[i]->SrRecursDone)
        if (i + 1 != pSSchema->SsRootElem &&
            i + 1 != pSSchema->SsDocument &&
            !pSSchema->SsRule->SrElem[i]->SrUnitElem)
          {
            if (pSSchema->SsRule->SrElem[i]->SrConstruct == CsChoice)
              /* it's a choice that defines an alias */
              /* insert the element in table of aliases */
              {
                Alias[NAlias] = i + 1;
                NAlias++;
                TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_ALIAS),
                                   pSSchema->SsRule->SrElem[i]->SrName);
              }
            else
              /* the second elements of a CsPairedElement are never
                 referenced */
              if (pSSchema->SsRule->SrElem[i]->SrConstruct != CsPairedElement ||
                  pSSchema->SsRule->SrElem[i]->SrFirstOfPair)
                /* unnecessary definition */
                TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_UNUSED),
                                   pSSchema->SsRule->SrElem[i]->SrName);
          }
    }

  /* clear all creation indicators */
  /* (use SrRecursDone as creation incator) */
  for (r = 0; r < pSSchema->SsNRules; r++)
    pSSchema->SsRule->SrElem[r]->SrRecursDone = False;
  /* we will create the root */
  if (pSSchema->SsRootElem > 0)
    pSSchema->SsRule->SrElem[pSSchema->SsRootElem - 1]->SrRecursDone = True;
  /* go through the table of rules */
  for (r = 0; r < pSSchema->SsNRules; r++)
    {
      pRule = pSSchema->SsRule->SrElem[r];
      /* included elements (SGML) will be created */
      if (pRule->SrNInclusions > 0)
        for (i = 0; i < pRule->SrNInclusions; i++)
          pSSchema->SsRule->SrElem[pRule->SrInclusion[i] - 1]->SrRecursDone = True;
      if (r < MAX_BASIC_TYPE)
        pRule->SrRecursDone = True;
      switch (pRule->SrConstruct)
        {
        case CsNatureSchema:
          /* elements providing new structure will be created */
          pRule->SrRecursDone = True;
          break;
        case CsBasicElement:
          /* all basic elements will be created */
          pRule->SrRecursDone = True;
          break;
        case CsPairedElement:
          /* all marks (by pair) will be created */
          pRule->SrRecursDone = True;
          break;
        case CsReference:
          /* references will be created */
          pRule->SrRecursDone = True;
          break;
        case CsIdentity:
          /* no creation for identity */
          break;
        case CsList:
          /* list elements will be created */
          pSSchema->SsRule->SrElem[pRule->SrListItem - 1]->SrRecursDone = True;
          break;
        case CsChoice:
          /* choice elements will be created */
          if (pRule->SrNChoices > 0)
            {
              for (i = 0; i < pRule->SrNChoices; i++)
                {
                  pSSchema->SsRule->SrElem[pRule->SrChoice[i] - 1]->SrRecursDone =True;
                  /* if the choice is a exported unit, */
                  /* its options will become exported units */
                  if (pRule->SrUnitElem)
                    pSSchema->SsRule->SrElem[pRule->SrChoice[i] - 1]->SrUnitElem =True;
                }
              pRule->SrUnitElem = False;
            }
          break;
        case CsUnorderedAggregate:
        case CsAggregate:
          /* agreggate components will be created */
          for (i = 0; i < pRule->SrNComponents; i++)
            pSSchema->SsRule->SrElem[pRule->SrComponent[i] - 1]->SrRecursDone = True;
          break;
        case CsConstant:
          /* constants will be created */
          pRule->SrRecursDone = True;
          break;
        case CsDocument:
          /* document element will be created */
          pRule->SrRecursDone = True;
          break;
        default:
          break;
        }
    }

  /* write the result */
  for (r = 0; r < pSSchema->SsNRules; r++)
    {
      pRule = pSSchema->SsRule->SrElem[r];
      if (pRule->SrConstruct == CsChoice)
        /* all choices different which are not the root, without
           SGML exceptions and not within an agregate are temporary
           created, until they are removed by the effective choice value.
           If the choice is an alias, don't create the element. */
        {
          temp = True;
          if (r + 1 == pSSchema->SsRootElem)
            temp = False;
          else if (pRule->SrNInclusions > 0 || pRule->SrNExclusions > 0)
            temp = False;
          else
            for (rr = 0; rr < pSSchema->SsNRules; rr++)
              if (pSSchema->SsRule->SrElem[rr]->SrConstruct == CsAggregate ||
                  pSSchema->SsRule->SrElem[rr]->SrConstruct == CsUnorderedAggregate)
                for (i = 0; i < pSSchema->SsRule->SrElem[rr]->SrNComponents; i++)
                  if (pSSchema->SsRule->SrElem[rr]->SrComponent[i] == r + 1)
                    temp = False;
          if (temp)
            /* is it an alias ? Search in the aliases table */
            for (i = 0; i < NAlias; i++)
              if (Alias[i] == r + 1)
                /* element found, don't create */
                temp = False;
          if (temp)
            TtaDisplayMessage (INFO, TtaGetMessage (STR,
                                                    STR_IS_A_TEMPORARY_ELEM), pRule->SrName);
        }
      else if (!pRule->SrRecursDone)
        /* units cannot used in the schema */
        if (!pRule->SrUnitElem)
          {
            TtaDisplayMessage (INFO, TtaGetMessage (STR,STR_WON_T_BE_CREATED),
                               pRule->SrName);
            /* search if there are REFERENCES to this element type */
            for (rr = 0; rr < pSSchema->SsNRules; rr++)
              {
                pRule2 = pSSchema->SsRule->SrElem[rr];
                if (pRule2->SrConstruct == CsReference)
                  if (pRule2->SrRefTypeNat[0] == '\0')
                    if (pRule2->SrReferredType == r + 1)
                      TtaDisplayMessage (INFO, TtaGetMessage (STR,
                                                              STR_WON_T_BE_CREATED_AND_IS_REFD), pRule->SrName);
              }
          }
    }
  free (Alias);
}


/*----------------------------------------------------------------------
  main                                                            
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS 
int STRmain (HWND hwnd, HWND statusBar, int argc, char **argv, int *Y)
#else  /* !_WINDOWS */
     int main (int argc, char **argv)
#endif /* _WINDOWS */
{
  FILE               *inputFile;
  char                buffer[200], fname[200];
  char               *pwd, *ptr;
  indLine             i;	/* current position in current line */
  indLine             wi;	/* start position of current word in the line*/
  indLine             wl;	/* word length */
  SyntacticType       wn;	/* syntactic type of current word */
  SyntRuleNum         r;	/* rule number */
  SyntRuleNum         pr;	/* previous rule number */
  SyntacticCode       c;	/* grammatical code of found word */
  ThotBool            fileOK;
  int                 nb;	/* identifier index of found word if it is
                             an indentifier */
  int                 param;
  unsigned char       car;
#ifdef _WINDOWS 
  char               *CMD;
  char               *cmd [100];
  char                msg [800];
  int                 ndx, pIndex = 0;
  HANDLE              cppLib;
  /* FARPROC             ptrMainProc; */
  typedef int (*MYPROC) (HWND, int, char **, int *);
  MYPROC              ptrMainProc; 

#else  /* !_WINDOWS */
  char                cmd[800];
#endif /* _WINDOWS */

#ifdef _WINDOWS
  COMPWnd = hwnd;
  compilersDC = GetDC (hwnd);
  _CY_ = *Y;
  strcpy (msg, "Executing str ");
  for (ndx = 1; ndx < argc; ndx++)
    {
      strcat (msg, argv [ndx]);
      strcat (msg, " ");
    }

  TtaDisplayMessage (INFO, msg);

  SendMessage (statusBar, SB_SETTEXT, (WPARAM) 0, (LPARAM) &msg[0]);
  SendMessage (statusBar, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
#endif /* _WINDOWS */

  TtaInitializeAppRegistry (argv[0]);
  (void) TtaGetMessageTable ("libdialogue", TMSG_LIB_MSG_MAX);
  COMPIL = TtaGetMessageTable ("compildialogue", COMP_MSG_MAX);
  STR = TtaGetMessageTable ("strdialogue", STR_MSG_MAX);
  error = False;
  /* initialize the parser */
  InitParser ();
  InitSyntax ("STRUCT.GRM");
  if (!error)
    {
      /* prepare the cpp command */
#ifdef _WINDOWS 
      cmd [pIndex] = (char *) TtaGetMemory (4);
      strcpy (cmd [pIndex++], "cpp");
#else  /* !_WINDOWS */
      strcpy (cmd, CPP " ");
#endif /* _WINDOWS */
      param = 1;
      while (param < argc && argv[param][0] == '-')
        {
          /* keep cpp params */
#ifdef _WINDOWS
          cmd [pIndex] = (char *) TtaGetMemory (strlen (argv[param]) + 1);
          strcpy (cmd [pIndex++], argv[param]);
#else  /* !_WINDOWS */
          strcat (cmd, argv[param]);
          strcat (cmd, " ");
#endif /* _WINDOWS */
          param++;
        }
      /* keep the name of the schema to be compile */
      if (param >= argc)
        {
          TtaDisplaySimpleMessage (FATAL, STR, STR_NO_SUCH_FILE);
#ifdef _WINDOWS 
          ReleaseDC (hwnd, compilersDC);
          return FATAL_EXIT_CODE;
#else  /* _WINDOWS */
          exit (1);
#endif /* _WINDOWS */
        } 
       
      /* get the name of the file to be compiled */
      strncpy (srceFileName, argv[param], MAX_NAME_LENGTH - 1);
      srceFileName[MAX_NAME_LENGTH - 1] = '\0';
      param++;
      strcpy (fname, srceFileName);
      /* check if the name contains a suffix */
      ptr = strrchr(fname, '.');
      nb = strlen (srceFileName);
      if (!ptr) /* there is no suffix */
        strcat (srceFileName, ".S");
      else if (strcmp (ptr, ".S"))
        {
          /* it's not the valid suffix */
          TtaDisplayMessage (FATAL, TtaGetMessage (STR, STR_INVALID_FILE),
                             srceFileName);
#ifdef _WINDOWS 
          ReleaseDC (hwnd, compilersDC);
          return FATAL_EXIT_CODE;
#else  /* _WINDOWS */
          exit (1);
#endif /* _WINDOWS */
        }
      else
        {
          /* it's the valid suffix, cut the srcFileName here */
          ptr[0] = '\0';
          nb -= 2; /* length without the suffix */
        } 
      /* add the suffix .SCH in srceFileName */
      strcat (fname, ".SCH");
       
      /* does the file to compile exist? */
      if (!TtaFileExist(srceFileName))
        TtaDisplaySimpleMessage (FATAL, STR, STR_NO_SUCH_FILE);
      else
        {
          /* provide the real source file */
          TtaFileUnlink (fname);
          pwd = TtaGetEnvString ("PWD");
#ifndef _WINDOWS
          i = strlen (cmd);
#endif /* _WINDOWS */
          if (pwd != NULL)
            {
#ifdef _WINDOWS
              CMD = (char *) TtaGetMemory (3 + strlen (pwd));
              sprintf (CMD, "-I%s", pwd);
              cmd [pIndex] = (char *) TtaGetMemory (3 + strlen (pwd));
              strcpy (cmd [pIndex++], CMD);
              cmd [pIndex] = (char *) TtaGetMemory (3);
              strcpy (cmd [pIndex++], "-C");
              cmd [pIndex] = (char *) TtaGetMemory (strlen (srceFileName) + 1);
              strcpy (cmd [pIndex++], srceFileName);
              cmd [pIndex] = (char *) TtaGetMemory (strlen (fname) + 1);
              strcpy (cmd [pIndex++], fname);
#else  /* !_WINDOWS */
              sprintf (&cmd[i], "-I%s -C %s > %s", pwd, srceFileName, fname);
#endif /* _WINDOWS */
            }
          else
            {
#ifdef _WINDOWS
              cmd [pIndex] = (char *) TtaGetMemory (3);
              strcpy (cmd [pIndex++], "-C");
              cmd [pIndex] = (char *) TtaGetMemory (strlen (srceFileName) + 1);
              strcpy (cmd [pIndex++], srceFileName);
              cmd [pIndex] = (char *) TtaGetMemory (2);
              strcpy (cmd [pIndex++], ">");
              cmd [pIndex] = (char *) TtaGetMemory (strlen (fname) + 1);
              strcpy (cmd [pIndex++], fname);
#else  /* !_WINDOWS */
              sprintf (&cmd[i], "-C %s > %s", srceFileName, fname);
#endif /* _WINDOWS */
            } 
#ifdef _WINDOWS
          cppLib = LoadLibrary ("cpp");
          ptrMainProc = (MYPROC) GetProcAddress ((HMODULE)cppLib, "CPPmain");
          i = ptrMainProc (hwnd, pIndex, cmd, &_CY_);
          FreeLibrary ((HMODULE)cppLib);
          for (ndx = 0; ndx < pIndex; ndx++)
            {
              free (cmd [ndx]);
              cmd [ndx] = (char*) 0;
            }
#else  /* !_WINDOWS */
          i = system (cmd);
#endif /* _WINDOWS */
          if (i == FATAL_EXIT_CODE)
            {
              /* cpp is not available, copy directely the file */
              TtaDisplaySimpleMessage (INFO, STR, STR_CPP_NOT_FOUND);
              TtaFileCopy (srceFileName, fname);
            } 
          /* open the resulting file */
          inputFile = TtaReadOpen (fname);
          /* suppress the suffix ".SCH" */
          srceFileName[nb] = '\0';
          /* get memory for structure schema */
          GetSchStruct (&pSSchema);
          if (pSSchema == NULL)
            TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);
	   
          /* get memory for external structure schema */
          GetSchStruct (&pExternSSchema);
          if (pExternSSchema == NULL)
            TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);
	   
          /* table of identifiers is empty */
          NIdentifiers = 0;
          LineNum = 0;
          /* start the generation */
          Initialize ();
          /* read the whole file and parse */
          fileOK = True;
          while (fileOK && !error)
            {
              /* read a line */
              i = 0;
              do
                {
                  fileOK = TtaReadByte (inputFile, &car);
                  inputLine[i] = car;
                  /* fileOK = TtaReadByte (inputFile, &inputLine[i]); */
                  i++;
                }
              /* while (i < LINE_LENGTH && inputLine[i - 1] != EOS &&
                 fileOK); */
              while (i < LINE_LENGTH && car != '\n' && fileOK);
              /* mark the real end of line */
              inputLine[i - 1] = EOS;
              /* increment lines counter */
              LineNum++;
              if (i >= LINE_LENGTH)
                CompilerMessage (1, STR, FATAL, STR_LINE_TOO_LONG,
                                 inputLine, LineNum);
              else if (inputLine[0] == '#')
                {
                  /* this line contains a cpp directive */
                  sscanf ((char *)inputLine, "# %d %s", &LineNum, buffer);
                  LineNum--;
                }
              else
                {
                  /* translate line characters */
                  OctalToChar ();
                  /* analyze the line */
                  wi = 1;
                  wl = 0;
                  /* analyze all words in the current line */
                  do
                    {
                      i = wi + wl;
                      /* next word */
                      GetNextToken (i, &wi, &wl, &wn);
                      if (wi > 0)
                        { /* word found */
                          /* parse the word */
                          AnalyzeToken (wi, wl, wn, &c, &r, &nb, &pr);
                          if (!error) /* process the word */
                            ProcessToken (wi, wl, c, r, nb, pr);
                        } 
                    } while (wi != 0 && !error);
                  /* no more word in the line */
                } 
            } 

          /* end of file */
          TtaReadClose (inputFile);
          if (!error) /* stop the parser */
            ParserEnd ();
          if (!error)
            /* set right rule number when we met the keyword THOT_EXPORT */
            if (!pSSchema->SsExport)
              ChangeRules ();
          if (!error) /* list external type names */
            ExternalTypes ();
          if (!error) {
            /* list recursive rules */
            ChkRecurs ();
            /* list aliases and elements that will not be created by the
               editor */
            ListAliasesAndNotCreated ();
		
            /* write the compiled schema into the output file */
            SchemaPath[0] = '\0';	/* use current directory */
            if (!error)
              {
                /* remove temporary file */
                TtaFileUnlink (fname);
                strcat (srceFileName, ".STR");
                fileOK = WriteStructureSchema (srceFileName, pSSchema, 0);
                if (!fileOK)
                  TtaDisplayMessage (FATAL,
                                     TtaGetMessage (STR, STR_CANNOT_WRITE),
                                     srceFileName);
              } 
          } 
          FreeSchStruc (pSSchema);
          FreeSchStruc (pExternSSchema);
        } 
    } 
  //   fflush (stdout);
  TtaSaveAppRegistry ();
#ifdef _WINDOWS
  *Y = _CY_ ;
  ReleaseDC (hwnd, compilersDC);
  return COMP_SUCCESS;
#else  /* _WINDOWS */
  exit (0);
#endif /* _WINDOWS */
}
