/* 
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Structure schema compiler:
 *  Source file name must have the suffix .S
 * This compiler is directed by the structure language grammar (see file
 * STRUCT.GRM)
 * It provides compiled files with suffix .STR which are used by the
 * Thot library.
 *
 * Author: V. Quint (INRIA)
 *         R. Guetari (W3C/INRIA): Windows routines.
 *
 */

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
#include "fileaccess.h"
#include "thotdir.h"

#define MAX_SRULE_RECURS 15	/* maximum of included rule levels within a rule */
#define MAX_EXTERNAL_TYPES 20	/* maximum of different external document types */
typedef enum
  {
     AttrRef, ElemRef, ElContent
  }
ContStrExt;

#define THOT_EXPORT
#include "compil_tv.h"
#include "platform_tv.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "analsynt_tv.h"

#ifdef _WINDOWS
#      define FATAL_EXIT_CODE 33
#      define COMP_SUCCESS     0
#else  /* !_WINDOWS */
#      define FATAL_EXIT_CODE -1
#endif /* _WINDOWS */

int                 LineNum;	 /* lines counter in source file */

static Name         srceFileName;/* file name of the schema cpp processed */
static PtrSSchema   pSSchema;	 /* pointer to the structure schema */
static int          TextConstPtr;/* current index in constants buffer */
static SRule       *CurExtensRule;/* current extension rule */
static ThotBool      CompilAttr;	 /* we are parsing global attributes */
static ThotBool      CompilLocAttr;/* we are parsing local attributes */
static ThotBool      CompilParam; /* we are parsing parameters */
static ThotBool      CompilAssoc; /* we are parsing associed elements */
static ThotBool      CompilUnits; /* we are parsing exported units */
static ThotBool      RootRule;	 /* we are waiting for the root rule */
static ThotBool      Rules;	 /* we are parsing rules */
static ThotBool      CompilExtens;/* on we are parsing extension rules */
static ThotBool      ExceptExternalType;/* we met "EXTERN" before a type name */
				 /* in section EXCEPT */
static ThotBool      Minimum;	 /* minimum elements within a list */
static ThotBool      Maximum;	 /* maximum elements within a list */
static ThotBool      RRight[MAX_SRULE_RECURS];/* within the right side of the rule */
static int          RecursLevel;	/* recursivity level */
static int          CurRule[MAX_SRULE_RECURS];	/* rule number */
static Name         CurName;	 /* left name of the last met rule */
static int          CurNum;	 /* index of this name in the identifiers table */
static int          CurNLocAttr; /* number of local attributes attached to CurName */
static int          CurLocAttr[MAX_LOCAL_ATTR];/* local attributes attached to CurName */
static ThotBool      CurReqAttr[MAX_LOCAL_ATTR];/* 'Required' ThotBools of local
						  attributes associated to CurName */
static ThotBool      CurParam;	 /* the last met rule is a parameter */
static ThotBool      CurAssoc;	 /* the last met rule is a associated element */
static ThotBool      CurUnit;	 /* the last met rule is a exported unit */
static ThotBool      Equal;	 /* it is the equality rule*/
static ThotBool      Option;	 /* it is an aggregate optional component */
static ThotBool      MandatoryAttr;/* it is a mandatory attribute */
static int          Sign;	 /* -1 or 1 to give the sign of the last attribute value */
static PtrSSchema   pExternSSchema;/* pointer to the external structure schema */
static int          RuleExportWith;/* current exported element to be managed */
static Name         ReferredTypeName;/* last name of the reference type */
static int          BeginReferredTypeName;/* position of this name in the line */
static ContStrExt   ExternalStructContext;/* context used by the external structure */
static ThotBool      UnknownContent;/* the content of exported element
				      is not defined in the schema */
static Name         PreviousIdent;/* name of the last met type identifier */
static int          PreviousRule;

static int          NExternalTypes;/* number of types declared as external */
static Name         ExternalType[MAX_EXTERNAL_TYPES];/* table of type names
							declared as external */
static ThotBool      IncludedExternalType[MAX_EXTERNAL_TYPES];/* table of type names */
				 /* declared as included external */
static ThotBool      CompilExcept;/* we are parsing exceptions */
static int          ExceptType;	 /* element type concerned by exceptions */
static int          ExceptAttr;	 /* attribute concerned by exceptions */
static int          CurBasicElem;/* current basic type */
static int          NAlias;	 /* number of aliases defined in the schema */
static int          Alias[MAX_RULES_SSCHEMA];/* rules which define aliases */
static ThotBool      FirstInPair; /* we met the keyword "First" */
static ThotBool      SecondInPair;/* we met the keyword "Second" */
static ThotBool      ReferenceAttr;/* we manage a reference attribute */
static ThotBool      ImportExcept;/* we met exception ImportLine or ImportParagraph */

#include "platform_f.h"
#include "parser_f.h"
#include "writestr_f.h"
#include "readstr_f.h"
#include "fileaccess_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "compilmsg_f.h"
#include "registry_f.h"

#ifdef _WINDOWS
#include "compilers_f.h"
#      ifndef DLLEXPORT
#      define DLLEXPORT __declspec (dllexport)
#      endif  /* DLLEXPORT */
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   InitBasicType                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitBasicType (SRule * pRule, STRING name, BasicType typ)
#else  /* __STDC__ */
static void         InitBasicType (pRule, name, typ)
SRule              *pRule;
STRING              name;
BasicType           typ;
#endif /* __STDC__ */
{
   ustrncpy (pRule->SrName, name, MAX_NAME_LENGTH);
   pRule->SrConstruct = CsBasicElement;
   pRule->SrBasicType = typ;
   pRule->SrAssocElem = False;
   pRule->SrParamElem = False;
   pRule->SrUnitElem = False;
   pRule->SrExportedElem = False;
   pRule->SrFirstExcept = 0;
   pRule->SrLastExcept = 0;
   pRule->SrNDefAttrs = 0;
   pRule->SrRecursive = False;
   pRule->SrNLocalAttrs = 0;
   pRule->SrNInclusions = 0;
   pRule->SrNExclusions = 0;
   pRule->SrRefImportedDoc = False;
}


/*----------------------------------------------------------------------
   Initialize initializes the structure schema in memory.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Initialize ()
#else  /* __STDC__ */
static void         Initialize ()
#endif /* __STDC__ */
{
   int                 i;
   TtAttribute        *pAttr;
   SRule              *pRule;

   pSSchema->SsExtension = False;
   pSSchema->SsNExtensRules = 0;
   pSSchema->SsExtensBlock = NULL;
   pSSchema->SsRootElem = 0;
   CurExtensRule = NULL;
   pSSchema->SsNAttributes = 0;
   for (i = 0; i < MAX_ATTR_SSCHEMA; i++)
     {
	pAttr = &pSSchema->SsAttribute[i];
	pAttr->AttrOrigName[0] = '\0';
	pAttr->AttrGlobal = True;
	/* all attributes are global */
	pAttr->AttrFirstExcept = 0;
	/* no exception for the attribute */
	pAttr->AttrLastExcept = 0;
	pAttr->AttrType = AtEnumAttr;
	pAttr->AttrNEnumValues = 0;
     }
   /* create the language attribute */
   pAttr = &pSSchema->SsAttribute[0];
   ustrncpy (pAttr->AttrName, TEXT("Langue"), MAX_NAME_LENGTH);
   pAttr->AttrOrigName[0] = '\0';
   pAttr->AttrGlobal = True;
   pAttr->AttrType = AtTextAttr;
   pSSchema->SsNAttributes++;

   pSSchema->SsNRules = 0;
   /* first rules of the structure schema are those that define basic types */
   pRule = &pSSchema->SsRule[CharString];
   InitBasicType (pRule, TEXT("TEXT_UNIT"), CharString);

   pRule = &pSSchema->SsRule[GraphicElem];
   InitBasicType (pRule, TEXT("GRAPHICS_UNIT"), GraphicElem);

   pRule = &pSSchema->SsRule[Symbol];
   InitBasicType (pRule, TEXT("SYMBOL_UNIT"), Symbol);

   pRule = &pSSchema->SsRule[Picture];
   InitBasicType (pRule, TEXT("PICTURE_UNIT"), Picture);

   pRule = &pSSchema->SsRule[Refer];
   InitBasicType (pRule, TEXT("REFERENCE_UNIT"), Refer);

   pRule = &pSSchema->SsRule[PageBreak];
   InitBasicType (pRule, TEXT("PAGE_BREAK"), PageBreak);

   pRule = &pSSchema->SsRule[UnusedBasicType];
   InitBasicType (pRule, TEXT("UNUSED"), UnusedBasicType);

   pSSchema->SsNRules = MAX_BASIC_TYPE;
   pSSchema->SsConstBuffer[0] = '\0';
   pSSchema->SsConstBuffer[1] = '\0';
   pSSchema->SsExport = False;
   pSSchema->SsNExceptions = 0;
   TextConstPtr = 1;
   CompilAttr = False;
   CompilLocAttr = False;
   CompilParam = False;
   CompilAssoc = False;
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
   CurParam = False;
   CurAssoc = False;
   CurUnit = False;
   Equal = False;
   Sign = 1;
   NExternalTypes = 0;
   CompilExcept = False;
   ExceptType = 0;
   ExceptAttr = 0;
   CurBasicElem = 0;
   NAlias = 0;
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
#ifdef __STDC__
static ThotBool      RuleNameExist ()
#else  /* __STDC__ */
static ThotBool      RuleNameExist ()
#endif /* __STDC__ */
{
   int                 r;
   ThotBool             ret;
   Name                name;

   /* initialize the function return */
   ret = False;
   /* keep the last rule name */
   ustrncpy (name, pSSchema->SsRule[pSSchema->SsNRules - 1].SrName, MAX_NAME_LENGTH);
   if (name[0] != '\0')
     {
	/* index of the rule in the table */
	r = 0;
	do
	   ret = ustrcmp (name, pSSchema->SsRule[r++].SrName) == 0;
	while (!ret && r < pSSchema->SsNRules - 1);
     }
   return ret;
}


/*----------------------------------------------------------------------
   Undefined                                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Undefined (int n)
#else  /* __STDC__ */
static void         Undefined (n)
int                 n;
#endif /* __STDC__ */
{
   int                 j;
   SrcIdentDesc       *pIdent;
   SRule              *pRule;

   pIdent = &Identifier[n - 1];
   TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_EXTERNAL_STRUCT), pIdent->SrcIdentifier);
   if (pSSchema->SsNRules >= MAX_RULES_SSCHEMA)
      /* too many rules */
      TtaDisplaySimpleMessage (FATAL, STR, STR_TOO_MAN_RULES);
   else
     /* add a new rule at the end of the schema */
     {
	pSSchema->SsNRules++;
	pIdent->SrcIdentDefRule = pSSchema->SsNRules;
	if (pIdent->SrcIdentLen > MAX_NAME_LENGTH - 1)
	   TtaDisplaySimpleMessage (FATAL, STR, STR_WORD_TOO_LONG);
	else
	  {
	     pRule = &pSSchema->SsRule[pSSchema->SsNRules - 1];
	     for (j = 0; j < pIdent->SrcIdentLen; j++)
		pRule->SrName[j] = pIdent->SrcIdentifier[j];
	     pRule->SrName[pIdent->SrcIdentLen] = '\0';
	     pRule->SrNLocalAttrs = 0;
	     pRule->SrNInclusions = 0;
	     pRule->SrNExclusions = 0;
	     pRule->SrRefImportedDoc = False;
	     pRule->SrNDefAttrs = 0;
	     pRule->SrAssocElem = False;
	     pRule->SrParamElem = False;
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
}


/*----------------------------------------------------------------------
   ChangeOneRule sets the rigth number of one rule instead of its indentifier.
   Undefined elements are considered as external structures (natures).	
   Unreferred elements are considered as errors if they are associated elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ChangeOneRule (SRule * pRule)
#else  /* __STDC__ */
static void         ChangeOneRule (pRule)
SRule              *pRule;
#endif /* __STDC__ */
{
   int                 j;

   /* management of inclusions */
   for (j = 0; j < pRule->SrNInclusions; j++)
     {
	if (pRule->SrInclusion[j] > MAX_BASIC_TYPE)
	  {
	     if (Identifier[pRule->SrInclusion[j] - MAX_BASIC_TYPE - 1].SrcIdentDefRule == 0)
		Undefined (pRule->SrInclusion[j] - MAX_BASIC_TYPE);
	     pRule->SrInclusion[j] =
		Identifier[pRule->SrInclusion[j] - MAX_BASIC_TYPE - 1].SrcIdentDefRule;
	  }
	else if (pRule->SrInclusion[j] < 0)
	   pRule->SrInclusion[j] = -pRule->SrInclusion[j];
	pSSchema->SsRule[pRule->SrInclusion[j] - 1].SrRecursDone = False;
     }

   /* management of exclusions */
   for (j = 0; j < pRule->SrNExclusions; j++)
     {
	if (pRule->SrExclusion[j] > MAX_BASIC_TYPE)
	  {
	     if (Identifier[pRule->SrExclusion[j] - MAX_BASIC_TYPE - 1].SrcIdentDefRule == 0)
		Undefined (pRule->SrExclusion[j] - MAX_BASIC_TYPE);
	     pRule->SrExclusion[j] =
		Identifier[pRule->SrExclusion[j] - MAX_BASIC_TYPE - 1].SrcIdentDefRule;
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
		  if (pRule->SrReferredType > MAX_BASIC_TYPE)
		    {
		       if (Identifier[pRule->SrReferredType - MAX_BASIC_TYPE - 1].SrcIdentDefRule == 0)
			  Undefined (pRule->SrReferredType - MAX_BASIC_TYPE);
		       pRule->SrReferredType = Identifier[pRule->SrReferredType - MAX_BASIC_TYPE - 1].
			  SrcIdentDefRule;
		    }
		  else if (pRule->SrReferredType < 0)
		     pRule->SrReferredType = -pRule->SrReferredType;
	       break;
	    case CsIdentity:
	       if (pRule->SrIdentRule > MAX_BASIC_TYPE)
		 {
		    if (Identifier[pRule->SrIdentRule - MAX_BASIC_TYPE - 1].SrcIdentDefRule == 0)
		       Undefined (pRule->SrIdentRule - MAX_BASIC_TYPE);
		    pRule->SrIdentRule = Identifier[pRule->SrIdentRule - MAX_BASIC_TYPE - 1].
		       SrcIdentDefRule;
		 }
	       else if (pRule->SrIdentRule < 0)
		  pRule->SrIdentRule = -pRule->SrIdentRule;
	       pSSchema->SsRule[pRule->SrIdentRule - 1].SrRecursDone = False;
	       break;
	    case CsList:
	       if (pRule->SrListItem > MAX_BASIC_TYPE)
		 {
		    if (Identifier[pRule->SrListItem - MAX_BASIC_TYPE - 1].SrcIdentDefRule == 0)
		       Undefined (pRule->SrListItem - MAX_BASIC_TYPE);
		    pRule->SrListItem = Identifier[pRule->SrListItem - MAX_BASIC_TYPE - 1].
		       SrcIdentDefRule;
		 }
	       else if (pRule->SrListItem < 0)
		  pRule->SrListItem = -pRule->SrListItem;
	       pSSchema->SsRule[pRule->SrListItem - 1].SrRecursDone = False;
	       break;
	    case CsChoice:
	       if (pRule->SrNChoices > 0)
		  for (j = 0; j < pRule->SrNChoices; j++)
		    {
		       if (pRule->SrChoice[j] > MAX_BASIC_TYPE)
			 {
			    if (Identifier[pRule->SrChoice[j] - MAX_BASIC_TYPE - 1].SrcIdentDefRule == 0)
			       Undefined (pRule->SrChoice[j] - MAX_BASIC_TYPE);
			    pRule->SrChoice[j] =
			       Identifier[pRule->SrChoice[j] - MAX_BASIC_TYPE - 1].SrcIdentDefRule;
			 }
		       else if (pRule->SrChoice[j] < 0)
			  pRule->SrChoice[j] = -pRule->SrChoice[j];
		       pSSchema->SsRule[pRule->SrChoice[j] - 1].SrRecursDone = False;
		    }
	       break;
	    case CsUnorderedAggregate:
	    case CsAggregate:
	       for (j = 0; j < pRule->SrNComponents; j++)
		 {
		    if (pRule->SrComponent[j] > MAX_BASIC_TYPE)
		      {
			 if (Identifier[pRule->SrComponent[j] - MAX_BASIC_TYPE - 1].SrcIdentDefRule == 0)
			    Undefined (pRule->SrComponent[j] - MAX_BASIC_TYPE);
			 pRule->SrComponent[j] =
			    Identifier[pRule->SrComponent[j] - MAX_BASIC_TYPE - 1].SrcIdentDefRule;
		      }
		    else if (pRule->SrComponent[j] < 0)
		       pRule->SrComponent[j] = -pRule->SrComponent[j];
		    pSSchema->SsRule[pRule->SrComponent[j] - 1].SrRecursDone = False;
		 }
	       break;
	    case CsConstant:
	       break;
	    case CsBasicElement:
	       break;
	    case CsNatureSchema:
	       break;
	    case CsPairedElement:
	       break;
	    case CsExtensionRule:
	       break;
	 }
}


/*----------------------------------------------------------------------
   ChangeRules sets the rigth number of rules instead of their indentifiers.
   Undefined elements are considered as external structures (natures).	
   Unreferred elements are considered as errors if they are associated elements.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ChangeRules ()
#else  /* __STDC__ */
static void         ChangeRules ()
#endif /* __STDC__ */
{
   int                 i;
   TtAttribute        *pAttr;

   /* use SrRecursDone with the mean `unused rule' */
   for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
      pSSchema->SsRule[i].SrRecursDone = True;
   /* go through all rules of built elements */
   for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
      ChangeOneRule (&pSSchema->SsRule[i]);
   /* go through all extension rules */
   for (i = 0; i < pSSchema->SsNExtensRules; i++)
      ChangeOneRule (&pSSchema->SsExtensBlock->EbExtensRule[i]);
   /* go through all attributes defined in the structure schema */
   for (i = 0; i < pSSchema->SsNAttributes; i++)
     {
	pAttr = &pSSchema->SsAttribute[i];
	/* don't take care of reference attributes */
	if (pAttr->AttrType == AtReferenceAttr)
	   /* only take care of types defined in the same schema */
	   if (pAttr->AttrTypeRefNature[0] == '\0')
	      if (pAttr->AttrTypeRef > MAX_BASIC_TYPE)
		{
		   if (Identifier[pAttr->AttrTypeRef - MAX_BASIC_TYPE - 1].SrcIdentDefRule == 0)
		      Undefined (pAttr->AttrTypeRef - MAX_BASIC_TYPE);
		   pAttr->AttrTypeRef =
		      Identifier[pAttr->AttrTypeRef - MAX_BASIC_TYPE - 1].SrcIdentDefRule;
		}
	      else if (pAttr->AttrTypeRef < 0)
		 pAttr->AttrTypeRef = -pAttr->AttrTypeRef;
     }
}


/*----------------------------------------------------------------------
   CopyWord copies into the name parameter the current word.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CopyWord (Name name, indLine wi, indLine wl)
#else  /* __STDC__ */
static void         CopyWord (name, wi, wl)
Name                name;
indLine             wi;
indLine             wl;
#endif /* __STDC__ */
{
   if (wl > MAX_NAME_LENGTH - 1)
      CompilerMessage (wi, STR, FATAL, STR_WORD_TOO_LONG, inputLine, LineNum);
   else
     {
	ustrncpy (name, &inputLine[wi - 1], wl);
	name[wl] = TEXT('\0');
     }
}


/*----------------------------------------------------------------------
   Push                                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Push (indLine wi)
#else  /* __STDC__ */
static void         Push (wi)
indLine             wi;
#endif /* __STDC__ */
{
   if (RecursLevel >= MAX_SRULE_RECURS)
      CompilerMessage (wi, STR, FATAL, STR_RULE_NESTING_TOO_DEEP, inputLine, LineNum);
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
#ifdef __STDC__
static void         RightIdentifier (int n, indLine wi)
#else  /* __STDC__ */
static void         RightIdentifier (n, wi)
int                 n;
indLine             wi;
#endif /* __STDC__ */
{
   SRule              *pRule;

   if (PreviousRule > 0)
     {
	n = -PreviousRule;
	PreviousRule = 0;
     }
   pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
		  CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_OPTIONS_IN_THE_CASE_STATEMENT, inputLine, LineNum);
	       else
		 {
		    pRule->SrNChoices++;
		    pRule->SrChoice[pRule->SrNChoices - 1] = n;
		 }
	       break;
	    case CsUnorderedAggregate:
	    case CsAggregate:
	       if (pRule->SrNComponents >= MAX_COMP_AGG)
		  CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_COMPONENTS_IN_AGGREGATE, inputLine, LineNum);
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
   NewRule                                                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NewRule (indLine wi)
#else  /* __STDC__ */
static void         NewRule (wi)
indLine             wi;
#endif /* __STDC__ */
{
   int                 i;
   SRule              *pRule;

   if (pSSchema->SsNRules >= MAX_RULES_SSCHEMA)
      CompilerMessage (wi, STR, FATAL, STR_TOO_MAN_RULES, inputLine, LineNum);
   else
      pSSchema->SsNRules++;
   if (CurNum > 0)
      /* there is a rigth part in this rule */
      if (Identifier[CurNum - 1].SrcIdentDefRule > 0)
	 /* already defined */
	 CompilerMessage (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED, inputLine, LineNum);
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
	pRule = &pSSchema->SsRule[pSSchema->SsNRules - 1];
	ustrncpy (pRule->SrName, CurName, MAX_NAME_LENGTH);
	pRule->SrNDefAttrs = 0;
	if (pRule->SrNLocalAttrs > 0)
	   /* this element has already local attributes */
	   CompilerMessage (wi, STR, FATAL, STR_THIS_ELEM_HAS_LOCAL_ATTRS, inputLine, LineNum);
	else
	   pRule->SrNLocalAttrs = CurNLocAttr;
	for (i = 0; i < pRule->SrNLocalAttrs; i++)
	  {
	     pRule->SrLocalAttr[i] = CurLocAttr[i];
	     pRule->SrRequiredAttr[i] = CurReqAttr[i];
	  }
	pRule->SrParamElem = CurParam;
	pRule->SrAssocElem = CurAssoc;
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
	   CompilerMessage (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED, inputLine, LineNum);
	if (RootRule)
	  {
	     /* compare this name with the schema name */
	     if (ustrcmp (pSSchema->SsName, CurName) == 0)
		/* it is the root element of the schema */
		pSSchema->SsRootElem = pSSchema->SsNRules;
	     else if (!pSSchema->SsExtension)
		CompilerMessage (wi, STR, FATAL, STR_FIRST_RULE_MUST_BE_THE_ROOT, inputLine, LineNum);
	     RootRule = False;
	  }
	CurName[0] = '\0';
	CurNum = 0;
	CurNLocAttr = 0;
	CurParam = False;
	CurAssoc = False;
	CurUnit = False;
     }
}


/*----------------------------------------------------------------------
   RuleNumber returns the rule number that defines the current word as
   element type.
   If this word doesn't match with a previous defined element type,
   the function returns 0.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          RuleNumber (indLine wl, indLine wi)
#else  /* __STDC__ */
static int          RuleNumber (wl, wi)
indLine             wl;
indLine             wi;
#endif /* __STDC__ */
{
   int                 RuleNum;
   Name                N;
   ThotBool             ok;

   CopyWord (N, wi, wl);
   RuleNum = 0;
   do
      ok = ustrcmp (N, pSSchema->SsRule[RuleNum++].SrName) == 0;
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
#ifdef __STDC__
static int          AttributeNumber (indLine wl, indLine wi)
#else  /* __STDC__ */
static int          AttributeNumber (wl, wi)
indLine             wl;
indLine             wi;
#endif /* __STDC__ */
{
   int                 AttrNum;
   Name                N;
   ThotBool             ok;

   CopyWord (N, wi, wl);
   AttrNum = 0;
   do
      ok = ustrcmp (N, pSSchema->SsAttribute[AttrNum++].AttrName) == 0;
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
#ifdef __STDC__
static void         ExceptionNum (int num, ThotBool checkType, ThotBool checkAttr, ThotBool CheckIntAttr, indLine wi)
#else  /* __STDC__ */
static void         ExceptionNum (num, checkType, checkAttr, CheckIntAttr, wi)
int                 num;
ThotBool             checkType;
ThotBool             checkAttr;
ThotBool             CheckIntAttr;
indLine             wi;
#endif /* __STDC__ */
{
   SRule              *pRule;
   TtAttribute        *pAttr;

   if (checkType && ExceptType == 0)
      CompilerMessage (wi, STR, FATAL, STR_ONLY_FOR_ELEMS, inputLine, LineNum);
   if (checkAttr && ExceptAttr == 0)
      CompilerMessage (wi, STR, FATAL, STR_ONLY_FOR_ATTRS, inputLine, LineNum);
   if (ExceptAttr > 0)
     {
	if (CheckIntAttr && pSSchema->SsAttribute[ExceptAttr - 1].AttrType != AtNumAttr)
	   CompilerMessage (wi, STR, FATAL, STR_ONLY_FOR_NUMERICAL_ATTRS, inputLine, LineNum);
	if (num == ExcActiveRef && pSSchema->SsAttribute[ExceptAttr - 1].AttrType != AtReferenceAttr)
	   CompilerMessage (wi, STR, FATAL, STR_ONLY_FOR_REFERENCE_ATTRS, inputLine, LineNum);
     }

   if (!error)
     {
	if (pSSchema->SsNExceptions >= MAX_EXCEPT_SSCHEMA)
	   /* the list of exceptions is full */
	   CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_EXCEPTS, inputLine, LineNum);
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
		     pRule = &pSSchema->SsRule[ExceptType - 1];
		  if (pRule->SrFirstExcept == 0)
		     pRule->SrFirstExcept = pSSchema->SsNExceptions;
		  pRule->SrLastExcept = pSSchema->SsNExceptions;
	       }
	     else if (ExceptAttr != 0)
	       {
		  pAttr = &pSSchema->SsAttribute[ExceptAttr - 1];
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
#ifdef __STDC__
static void         BasicEl (int n, indLine wi, SyntRuleNum pr)
#else  /* __STDC__ */
static void         BasicEl (n, wi, pr)
int                 n;
indLine             wi;
SyntRuleNum         pr;
#endif /* __STDC__ */
{
   SRule              *pRule;

   pRule = NULL;
   if (pr == RULE_InclElem || pr == RULE_ExclElem)
      if (CompilExtens)
	 pRule = CurExtensRule;
      else
	 pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
   if (pr == RULE_InclElem)
     {
	if (pRule->SrNInclusions >= MAX_INCL_EXCL_SRULE)
	   CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_INCLS_FOR_THAT_ELEM, inputLine, LineNum);
	else
	  {
	     pRule->SrNInclusions++;
	     pRule->SrInclusion[pRule->SrNInclusions - 1] = n;
	  }
     }
   else if (pr == RULE_ExclElem)
     {
	if (pRule->SrNExclusions >= MAX_INCL_EXCL_SRULE)
	   CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_EXCLUSIONS_FOR_THAT_ELEM, inputLine, LineNum);
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
	if (pSSchema->SsRule[ExceptType - 1].SrFirstExcept != 0)
	   CompilerMessage (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, LineNum);
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
#ifdef __STDC__
static int          StoreConstText (int wi, int wl)
#else  /* __STDC__ */
static int          StoreConstText (wi, wl)
int                 wi;
int                 wl;
#endif /* __STDC__ */
{
   int                 i, pos;

   pos = TextConstPtr;
   if (TextConstPtr + wl >= MAX_LEN_ALL_CONST)
      CompilerMessage (wi, STR, FATAL, STR_CONSTANT_BUFFER_FULL, inputLine, LineNum);
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
#ifdef __STDC__
static void         InitRule (SRule * pRule)
#else  /* __STDC__ */
static void         InitRule (pRule)
SRule              *pRule;
#endif /* __STDC__ */
{
   pRule->SrName[0] = '\0';
   pRule->SrNDefAttrs = 0;
   pRule->SrNLocalAttrs = 0;
   pRule->SrAssocElem = False;
   pRule->SrParamElem = False;
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
#ifdef __STDC__
static void         DuplicatePairRule ()
#else  /* __STDC__ */
static void         DuplicatePairRule ()
#endif /* __STDC__ */
{
   SRule              *newRule;
   SRule              *prevRule;

   prevRule = &pSSchema->SsRule[pSSchema->SsNRules - 1];
   if (prevRule->SrConstruct == CsPairedElement && prevRule->SrFirstOfPair)
     {
	/* ajoute une regle CsPairedElement a la fin du schema */
	pSSchema->SsNRules++;
	newRule = &pSSchema->SsRule[pSSchema->SsNRules - 1];
	InitRule (newRule);
	ustrncpy (newRule->SrName, prevRule->SrName, MAX_NAME_LENGTH);
	newRule->SrConstruct = CsPairedElement;
	newRule->SrFirstOfPair = False;
     }
}


/*----------------------------------------------------------------------
   GetExtensionRule searchs an extension rule matching the name (wi, wl).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static SRule       *GetExtensionRule (indLine wi, indLine wl)
#else  /* __STDC__ */
static SRule       *GetExtensionRule (wi, wl)
indLine             wi;
indLine             wl;
#endif /* __STDC__ */
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
	     if (ustrncmp (n, pRule->SrName, MAX_NAME_LENGTH) == 0)
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
#ifdef __STDC__
static SRule       *NewExtensionRule (indLine wi, indLine wl)
#else  /* __STDC__ */
static SRule       *NewExtensionRule (wi, wl)
indLine             wi;
indLine             wl;
#endif /* __STDC__ */
{
   SRule              *pRule;
   PtrExtensBlock      pEB;

   pRule = NULL;
   if (GetExtensionRule (wi, wl) != NULL)
      CompilerMessage (wi, STR, FATAL, STR_NAME_ALREADY_DECLARED, inputLine, LineNum);
   if (pSSchema->SsExtensBlock == NULL)
     {
      
      GetExternalBlock (&pEB);
      if (pEB == NULL)
	 TtaDisplaySimpleMessage (FATAL, STR, STR_NOT_ENOUGH_MEM);
      pSSchema->SsExtensBlock = pEB;
     }

   if (pSSchema->SsExtensBlock != NULL)
      if (pSSchema->SsNExtensRules >= MAX_EXTENS_SSCHEMA)
	 CompilerMessage (wi, STR, FATAL, STR_TOO_MAN_RULES, inputLine, LineNum);
      else
	{
	   pRule = &pSSchema->SsExtensBlock->EbExtensRule[pSSchema->SsNExtensRules];
	   pSSchema->SsNExtensRules++;
	   InitRule (pRule);
	   CopyWord (pRule->SrName, wi, wl);
	   pRule->SrConstruct = CsExtensionRule;
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
#ifdef __STDC__
static void         ProcessToken (indLine wi, indLine wl, SyntacticCode c, SyntacticCode r, int nb, SyntRuleNum pr)
#else  /* __STDC__ */
static void         ProcessToken (wi, wl, c, r, nb, pr)
indLine             wi;
indLine             wl;
SyntacticCode       c;
SyntacticCode       r;
int                 nb;
SyntRuleNum         pr;
#endif /* __STDC__ */
{
   int                 SynInteger, i, j;
   Name                N;
   TtAttribute        *pAttr;
   SRule              *pRule;
   ThotBool             ok;
   int                 attrNum;

   if (c < 1000)
      /* it's an intermediate symbol */
      CompilerMessage (wi, STR, FATAL, STR_INTERMEDIATE_SYMBOL, inputLine, LineNum);
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
			  pAttr = &pSSchema->SsAttribute[CurExtensRule->SrLocalAttr[CurExtensRule->SrNLocalAttrs - 1] - 1];
		       else
			  /* within the structure rule */
			  pAttr = &pSSchema->SsAttribute[CurLocAttr[CurNLocAttr - 1] - 1];
		       if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 0)
			  /* no value defined */
			  CompilerMessage (wi, STR, FATAL, STR_ATTR_WITHOUT_VALUE, inputLine, LineNum);
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
			       CurParam = False;
			       CurAssoc = False;
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
			  CompilerMessage (BeginReferredTypeName, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, LineNum);
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
		       pAttr = &pSSchema->SsAttribute[pSSchema->SsNAttributes - 1];
		       if (pAttr->AttrType != AtEnumAttr
			   || (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues > 0))
			  /* attribute already defined */
			  CompilerMessage (wi, STR, FATAL, STR_THAT_ATTR_HAS_ALREADY_VALUES, inputLine, LineNum);
		    }
		  break;
	       case CHR_40:
		  /*  (  */
		  if (r == RULE_Constr)
		    {
		       if (pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrConstruct == CsList)
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
			  pAttr = &pSSchema->SsAttribute[CurExtensRule->SrLocalAttr[CurExtensRule->SrNLocalAttrs - 1] - 1];
		       else
			  /* within structure rule */
			  pAttr = &pSSchema->SsAttribute[CurLocAttr[CurNLocAttr - 1] - 1];
		       if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 0)
			  /* no value defined */
			  CompilerMessage (wi, STR, FATAL, STR_ATTR_WITHOUT_VALUE, inputLine, LineNum);
		       if (CurBasicElem > 0)
			  /* end of local attributes of a basic element */
			 {
			    pRule = &pSSchema->SsRule[CurBasicElem - 1];
			    if (pRule->SrNLocalAttrs > 0)
			       /* there are local attributes for this element */
			       CompilerMessage (wi, STR, FATAL, STR_THIS_ELEM_HAS_LOCAL_ATTRS, inputLine, LineNum);
			    else
			       pRule->SrNLocalAttrs = CurNLocAttr;
			    for (i = 0; i < pRule->SrNLocalAttrs; i++)
			      {
				 pRule->SrLocalAttr[i] = CurLocAttr[i];
				 pRule->SrRequiredAttr[i] = CurReqAttr[i];
			      }
			    CurBasicElem = 0;
			    CurNLocAttr = 0;
			 }
		    }
		  if (r == RULE_Constr)
		    {
		       CurBasicElem = 0;
		       if (RecursLevel > 1)
			  if (pSSchema->SsRule[CurRule[RecursLevel - 2] - 1].SrConstruct == CsList)
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
				    CurParam = False;
				    CurAssoc = False;
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
				 ok = ustrcmp (ReferredTypeName, pSSchema->SsRule[i].SrName) == 0;
				 /* next rule */
				 i++;
			      }
			    while (!ok && i < pSSchema->SsNRules);
			    if (!ok)
			       CompilerMessage (BeginReferredTypeName, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, LineNum);
			    else if (pSSchema->SsRule[i - 1].SrConstruct != CsPairedElement)
			       /* it is not a paired type */
			       CompilerMessage (BeginReferredTypeName, STR, FATAL, STR_FIRST_SECOND_FORBIDDEN, inputLine, LineNum);
			    else if (SecondInPair)
			      {
				 if (r == RULE_AttrType)
				    pSSchema->SsAttribute[pSSchema->SsNAttributes - 1].
				       AttrTypeRef = -(i + 1);
				 else if (pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrConstruct
					  == CsReference)
				    pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrReferredType
				       = -(i + 1);
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
			CompilerMessage (BeginReferredTypeName, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, LineNum);
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
			  pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
		     pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrMinItems = 0;
		  else
		     pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrMaxItems = 32000;
		  break;
	       case CHR_43:
		  /*  +  */
		  if (r == RULE_DefWith)
		     if (!RRight[RecursLevel - 1])
			CompilerMessage (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM, inputLine, LineNum);
		  break;
	       case CHR_45:
		  /*  -  */
		  if (r == RULE_FixedValue)
		    /* negative sign for the next attribute value */
		     Sign = -1;
		  else if (r == RULE_DefWith)
		     if (!RRight[RecursLevel - 1])
			CompilerMessage (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM, inputLine, LineNum);
		  break;
	       case CHR_58:
		  /*  :  */
		  break;
	    }

   else if (c < 2000)
      /* large keyword */
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
	       case KWD_PARAM:
		  if (pSSchema->SsExtension)
		     CompilerMessage (wi, STR, FATAL, STR_NOT_ALLOWED_IN_AN_EXTENSION, inputLine, LineNum);
		  else
		    {
		       CompilParam = True;
		       Rules = True;
		       CompilAttr = False;
		    }
		  break;
	       case KWD_STRUCT:
		  /* the first rule is the root rule */
		  RootRule = True;
		  Rules = True;
		  CompilParam = False;
		  CompilAttr = False;
		  break;
	       case KWD_EXTENS:
		  /* check if we are within a schema extension */
		  if (!pSSchema->SsExtension)
		     CompilerMessage (wi, STR, FATAL, STR_NOT_AN_EXTENSION, inputLine, LineNum);
		  else
		    {
		       CompilParam = False;
		       CompilAttr = False;
		       CompilExtens = True;
		    }
		  break;
	       case KWD_ASSOC:
		  if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
		     CompilerMessage (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, LineNum);
		  else
		    {
		       CompilParam = False;
		       CompilAttr = False;
		       CompilAssoc = True;
		       CompilExtens = False;
		    }
		  break;
	       case KWD_UNITS:
		  if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
		     CompilerMessage (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, LineNum);
		  else
		    {
		       CompilUnits = True;
		       CompilAssoc = False;
		       CompilParam = False;
		       CompilAttr = False;
		       CompilExtens = False;
		    }
		  break;
	       case KWD_EXPORT:
		  if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
		     CompilerMessage (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, LineNum);
		  else if (pSSchema->SsExtension)
		     CompilerMessage (wi, STR, FATAL, STR_NOT_ALLOWED_IN_AN_EXTENSION, inputLine, LineNum);
		  else
		    {
		       CompilParam = False;
		       CompilAttr = False;
		       CompilExtens = False;
		       CompilAssoc = False;
		       CompilUnits = False;
		       ChangeRules ();
		       /* set the right rule numbers */
		       pSSchema->SsExport = True;
		    }
		  break;
	       case KWD_EXCEPT:
		  if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
		     CompilerMessage (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, LineNum);
		  else
		    {
		       CompilParam = False;
		       CompilAttr = False;
		       CompilExtens = False;
		       CompilAssoc = False;
		       CompilUnits = False;
		       CurExtensRule = NULL;
		       CompilExcept = True;
		       ExceptExternalType = False;
		    }
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
			    CurParam = False;
			    CurAssoc = False;
			    CurUnit = False;
			 }
		    }
		  else if (r == RULE_StructModel)
		     if (pSSchema->SsRootElem == 0 && !pSSchema->SsExtension)
			CompilerMessage (wi, STR, FATAL, STR_STRUCT_SECTION_MISSING, inputLine, LineNum);
		  break;
	       case KWD_INTEGER:
		  pSSchema->SsAttribute[pSSchema->SsNAttributes - 1].AttrType = AtNumAttr;
		  break;
	       case KWD_TEXT:
		  if (r == RULE_BasicType)
		     /* element type */
		     BasicEl (CharString + 1, wi, pr);
		  else
		     /* attribute type */
		     pSSchema->SsAttribute[pSSchema->SsNAttributes - 1].AttrType = AtTextAttr;
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
			    pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
		       pAttr = &pSSchema->SsAttribute[pSSchema->SsNAttributes - 1];
		       pAttr->AttrType = AtReferenceAttr;
		       pAttr->AttrTypeRefNature[0] = '\0';
		       pAttr->AttrTypeRef = 0;
		       /* by default, the referred element type */
		       /* is defined within the schema */
		       ExternalStructContext = AttrRef;
		    }
		  break;
	       case KWD_ANY:
		  if (ReferenceAttr)
		     pSSchema->SsAttribute[pSSchema->SsNAttributes - 1].AttrTypeRef = 0;
		  else
		     pSSchema->SsRule[CurRule[RecursLevel - 1] - 1].SrReferredType = 0;
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
		  BasicEl (Picture + 1, wi, pr);
		  break;
	       case KWD_UNIT:
	       case KWD_NATURE:
		  if (pr == RULE_ExclElem || pr == RULE_InclElem)
		     CompilerMessage (wi, STR, FATAL, STR_NOT_ALLOWED_HERE, inputLine, LineNum);
		  else
		    {
		       Equal = False;
		       NewRule (wi);
		       if (!error)
			 {
			    pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
			    pRule->SrConstruct = CsChoice;
			    if (c == KWD_UNIT)
			       /* 18:     */
			       pRule->SrNChoices = 0;	/* UNIT  */
			    else
			       /* 19:     */
			       pRule->SrNChoices = -1;	/* NATURE  */
			 }
		    }
		  break;
	       case KWD_WITH:
		  /* begin of fixed attributes */
		  if (r == RULE_DefWith)
		     if (!RRight[RecursLevel - 1])
			CompilerMessage (wi, STR, FATAL, STR_GIVE_A_NAME_TO_THAT_ELEM, inputLine, LineNum);

		  break;
	       case KWD_EXTERN:
		  if (r == RULE_ExceptType)
		     ExceptExternalType = True;
		  else if (r == RULE_ExtOrDef)
		     if (NExternalTypes >= MAX_EXTERNAL_TYPES)
			/* table of external types is full */
			CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_EXTERNAL_DOCS, inputLine, LineNum);
		     else
			/* add into the table the new type */
		       {
			  NExternalTypes++;
			  IncludedExternalType[NExternalTypes - 1] = False;
			  ustrncpy (ExternalType[NExternalTypes - 1], PreviousIdent, MAX_NAME_LENGTH);
			  if (ustrcmp (PreviousIdent, pSSchema->
			      SsRule[pSSchema->SsRootElem - 1].SrName) == 0)
			     /* the document type is used as external */
			    {
			       /* add a SyntacticType rule at the end of the schema */
			       pRule = &pSSchema->SsRule[pSSchema->SsNRules++];
			       ustrncpy (pRule->SrName, PreviousIdent, MAX_NAME_LENGTH);
			       pRule->SrNLocalAttrs = 0;
			       pRule->SrNDefAttrs = 0;
			       pRule->SrAssocElem = False;
			       pRule->SrParamElem = False;
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
		  break;
	       case KWD_INCLUDED:
		  /* included */
		  if (NExternalTypes >= MAX_EXTERNAL_TYPES)
		     /* table of external types is full */
		     CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_EXTERNAL_DOCS, inputLine, LineNum);
		  else
		     /* add into the table the new external type */
		    {
		       IncludedExternalType[NExternalTypes] = True;
		       ustrncpy (ExternalType[NExternalTypes], PreviousIdent, MAX_NAME_LENGTH);
		       NExternalTypes++;
		    }
		  break;
	       case KWD_LIST:
		  Equal = False;
		  NewRule (wi);
		  if (!error)
		    {
		       pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
		       pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
		       pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
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
		     if (pSSchema->SsRootElem == CurRule[RecursLevel - 1])
			CompilerMessage (wi, STR, FATAL, STR_ROOT_CANNOT_BE_A_PAIR, inputLine, LineNum);
		     else
		       {
			  pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
			  pRule->SrConstruct = CsPairedElement;
			  pRule->SrFirstOfPair = True;
		       }
		  break;
	       case KWD_Nothing:
		  /* pno contents for the current exported element */
		  pRule = &pSSchema->SsRule[RuleExportWith - 1];
		  pRule->SrExportContent = 0;
		  if ((pRule->SrConstruct == CsChoice) && (pRule->SrNChoices >= 1))
		     for (j = 0; j < pRule->SrNChoices; j++)
			pSSchema->SsRule[pRule->SrChoice[j] - 1].SrExportContent = 0;
		  break;
	       case KWD_NoCut:
		  ExceptionNum (ExcNoCut, True, False, False, wi);
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
	       case KWD_Shadow:
		  ExceptionNum (ExcShadow, True, False, False, wi);
		  break;
	       case KWD_NoCreate:
		  ExceptionNum (ExcNoCreate, True, False, False, wi);
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
	       case KWD_EventAttr:
		  ExceptionNum (ExcEventAttr, False, True, False, wi);
		  break;
	       case KWD_NewWidth:
		  ExceptionNum (ExcNewWidth, False, True, True, wi);
		  break;
	       case KWD_NewPercentWidth:
		  ExceptionNum (ExcNewPercentWidth, False, True, True, wi);
		  break;
	       case KWD_ColRef:
		  ExceptionNum (ExcColRef, False, True, False, wi);
		  break;
	       case KWD_ColSpan:
		  ExceptionNum (ExcColSpan, False, True, True, wi);
		  break;
	       case KWD_RowSpan:
		  ExceptionNum (ExcRowSpan, False, True, True, wi);
		  break;
	       case KWD_NewHeight:
		  ExceptionNum (ExcNewHeight, False, True, True, wi);
		  break;
	       case KWD_NewHPos:
		  ExceptionNum (ExcNewHPos, False, True, True, wi);
		  break;
	       case KWD_NewVPos:
		  ExceptionNum (ExcNewVPos, False, True, True, wi);
		  break;
	       case KWD_Invisible:
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
	       case KWD_NoShowBox:
		  ExceptionNum (ExcNoShowBox, True, False, False, wi);
		  break;
	       case KWD_MoveResize:
		  ExceptionNum (ExcMoveResize, True, False, False, wi);
		  break;
	       case KWD_NoSelect:
		  ExceptionNum (ExcNoSelect, True, False, False, wi);
		  /* end case c */
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
	       case KWD_ActiveRef:
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
	       case KWD_ReturnCreateNL:
		  ExceptionNum (ExcReturnCreateNL, True, False, False, wi);
		  break;
	       case KWD_ParagraphBreak:
		  ExceptionNum (ExcParagraphBreak, True, False, False, wi);
		  break;
	       case KWD_CssBackground:
		  ExceptionNum (ExcCssBackground, True, False, False, wi);
		  break;
	       case KWD_CssClass:
		  ExceptionNum (ExcCssClass, False, True, False, wi);
		  break;
	       case KWD_CssId:
		  ExceptionNum (ExcCssId, False, True, False, wi);
		  break;
	       case KWD_CssPseudoClass:
		  ExceptionNum (ExcCssPseudoClass, False, True, False, wi);
		  break;
	       case KWD_Root:
		  CurExtensRule = NewExtensionRule (wi, 0);
		  CurExtensRule->SrName[0] = '\0';
		  break;
		  /* end of case c */
	    }

   else
      /* basic type */
      switch (c)
	    {
	       case 3001:
		  /* a name */
		  switch (r)
			{
			      /* r = rule number where the name appears */
			   case RULE_ElemName /* VarElemName */ :
			      if (pr == RULE_StructModel)
				 /* after the keyword 'STRUCTURE' */
				 /* keep the structure name */
				{
				   CopyWord (pSSchema->SsName, wi, wl);
				   /* compare this name with the file name */

				   if (ustrcmp (pSSchema->SsName, srceFileName) != 0)
				      /* names differ */
				      CompilerMessage (wi, STR, FATAL, STR_FILE_NAME_AND_STRUCT_NAME_DIFFERENT, inputLine, LineNum);
				}
			      if (pr == RULE_Rule)
				 /* begin of the first level rule */
				{
				   CopyWord (CurName, wi, wl);
				   CurNum = nb;
				   if (CompilParam)
				      CurParam = True;
				   else
				      CurParam = False;
				   if (CompilAssoc)
				      CurAssoc = True;
				   else
				      CurAssoc = False;
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
			      if (pr == RULE_TypeRef && ReferenceAttr)
				 /* within a reference attribute */
				{
				   pSSchema->SsAttribute[pSSchema->SsNAttributes - 1].AttrTypeRef =
				      nb + MAX_BASIC_TYPE;
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
				      CompilerMessage (wi, STR, FATAL, STR_CANNOT_READ_STRUCT_SCHEM, inputLine, LineNum);
				   else
				      /* structure schema loaded */
				      /* search the referred element within */
				     {
					i = 0;
					while (ustrcmp (ReferredTypeName, pExternSSchema->SsRule[i].SrName) != 0
					&& i - 1 < pExternSSchema->SsNRules)
					   i++;
					if (ustrcmp (ReferredTypeName, pExternSSchema->SsRule[i].SrName) != 0)
					   /* unknown type */
					   CompilerMessage (BeginReferredTypeName, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, LineNum);
					else
					   /* the referred type is found: number i+1 */
					   switch (ExternalStructContext)
						 {
						    case AttrRef:
						       /* within reference attribute */
						       pAttr = &pSSchema->SsAttribute[pSSchema->SsNAttributes - 1];
						       if (SecondInPair)
							  pAttr->AttrTypeRef = i + 2;
						       else
							  pAttr->AttrTypeRef = i + 1;
						       ustrncpy (pAttr->AttrTypeRefNature, N, MAX_NAME_LENGTH);
						       FirstInPair = False;
						       SecondInPair = False;
						       break;
						    case ElemRef:
						       /* within construction CsReference */
						       pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
						       if (SecondInPair)
							  pRule->SrReferredType = i + 2;
						       else
							  pRule->SrReferredType = i + 1;
						       ustrncpy (pRule->SrRefTypeNat, N, MAX_NAME_LENGTH);
						       FirstInPair = False;
						       SecondInPair = False;
						       break;
						    case ElContent:
						       /* within exported element contents */
						       pRule = &pSSchema->SsRule[RuleExportWith - 1];
						       pRule->SrExportContent = i + 1;
						       ustrncpy (pRule->SrNatExpContent, N, MAX_NAME_LENGTH);
						       UnknownContent = False;
						       if (pRule->SrConstruct == CsChoice)
							  if (pRule->SrNChoices >= 1)
							     for (j = 0; j < pRule->SrNChoices; j++)
							       {
								  pSSchema->SsRule[pRule->SrChoice[j] - 1].SrExportContent = i + 1;
								  ustrncpy (pSSchema->SsRule[pRule->
											    SrChoice[j] - 1].SrNatExpContent, N, MAX_NAME_LENGTH);
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
				      CompilerMessage (wi, STR, FATAL, STR_TYPE_UNKNOWN, inputLine, LineNum);
				   else
				     {
					pRule = &pSSchema->SsRule[i - 1];
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
						   pSSchema->SsRule[pRule->SrChoice[j] - 1].SrExportedElem = True;
						   pSSchema->SsRule[pRule->SrChoice[j] - 1].SrExportContent = pRule->SrChoice[j];
						   pSSchema->SsRule[pRule->SrChoice[j] - 1].SrNatExpContent[0] = '\0';
						}
					/* the first mark of the paired type is exported, */
					/* the second one is also exported */
					if (pRule->SrConstruct == CsPairedElement)
					   if (pRule->SrFirstOfPair)
					     {
						pSSchema->SsRule[i].SrExportedElem = True;
						pSSchema->SsRule[i].SrExportContent = i + 1;
						pSSchema->SsRule[i].SrNatExpContent[0] = '\0';
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
				   if (pSSchema->SsRule[i - 1].SrConstruct != CsNatureSchema)
				     {
					pRule = &pSSchema->SsRule[RuleExportWith - 1];
					pRule->SrExportContent = i;
					if (pRule->SrConstruct == CsChoice)
					   if (pRule->SrNChoices >= 1)
					      for (j = 0; j < pRule->SrNChoices; j++)
						 pSSchema->SsRule[pRule->SrChoice[j] - 1].
						    SrExportContent = i;
					/* the first mark of the paired type is exported, */
					/* the second one is also exported */
					if (pRule->SrConstruct == CsPairedElement)
					   if (pRule->SrFirstOfPair)
					      if (i == RuleExportWith)
						 /* the contents of the second mark */
						 /* is the mark itself */
						 pSSchema->SsRule[RuleExportWith].SrExportContent = i + 1;
					      else
						 pSSchema->SsRule[RuleExportWith].SrExportContent = i;
				     }
				   else
				      /* the object contents depends on another */
				      /* structure schema, read the schema */
				   if (!ReadStructureSchema (pSSchema->SsRule[i - 1].SrName, pExternSSchema))
				      /* cannot load the schema */
				      CompilerMessage (wi, STR, FATAL, STR_CANNOT_READ_STRUCT_SCHEM, inputLine, LineNum);
				   else
				      /* structure schema loaded, the contents */
				      /* the root element of this schema */
				     {
					pRule = &pSSchema->SsRule[RuleExportWith - 1];
					pRule->SrExportContent = pExternSSchema->SsRootElem;
					ustrncpy (pRule->SrNatExpContent, pExternSSchema->SsName, MAX_NAME_LENGTH);
					if (pRule->SrConstruct == CsChoice)
					   if (pRule->SrNChoices >= 1)
					      for (j = 0; j < pRule->SrNChoices; j++)
						{
						   pSSchema->SsRule[pRule->SrChoice[j] - 1].
						      SrExportContent = pExternSSchema->SsRootElem;
						   ustrncpy (pSSchema->
							    SsRule[pRule->SrChoice[j] - 1].SrNatExpContent,
							    pExternSSchema->SsName, MAX_NAME_LENGTH);
						}
				     }
				}
			      if (pr == RULE_ExceptType)
				 /* element type name within exceptions */
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
					      CompilerMessage (wi, STR, FATAL, STR_TYPE_OR_ATTR_UNKNOWN, inputLine, LineNum);
					   else if (pSSchema->SsAttribute[ExceptAttr - 1].AttrFirstExcept != 0)
					      CompilerMessage (wi, STR, FATAL, STR_THIS_ATTR_ALREADY_HAS_EXCEPTS, inputLine, LineNum);
					}
				      else
					 /* it's a declared type */
					{
					   if (FirstInPair || SecondInPair)
					      /* the element name is preceded by First or Second */
					      if (pSSchema->SsRule[ExceptType - 1].SrConstruct != CsPairedElement)
						 /* it's not a  paired type */
						 CompilerMessage (wi, STR, FATAL, STR_FIRST_SECOND_FORBIDDEN, inputLine, LineNum);
					      else if (SecondInPair)
						 ExceptType++;
					   if (pSSchema->SsRule[ExceptType - 1].SrFirstExcept != 0)
					      CompilerMessage (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, LineNum);
					}
				      FirstInPair = False;
				      SecondInPair = False;
				   }
			      if (pr == RULE_InclElem)
				{
				   if (CompilExtens)
				      pRule = CurExtensRule;
				   else
				      pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
				   if (pRule->SrNInclusions >= MAX_INCL_EXCL_SRULE)
				      CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_INCLS_FOR_THAT_ELEM, inputLine, LineNum);
				   else
				     {
					pRule->SrNInclusions++;
					pRule->SrInclusion[pRule->SrNInclusions - 1] = nb + MAX_BASIC_TYPE;
				     }
				}
			      if (pr == RULE_ExclElem)
				{
				   if (CompilExtens)
				      pRule = CurExtensRule;
				   else
				      pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
				   if (pRule->SrNExclusions >= MAX_INCL_EXCL_SRULE)
				      CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_EXCLUSIONS_FOR_THAT_ELEM, inputLine, LineNum);
				   else
				     {
					pRule->SrNExclusions++;
					pRule->SrExclusion[pRule->SrNExclusions - 1] = nb + MAX_BASIC_TYPE;
				     }
				}
			      break;
			   case RULE_PresentName:
			      /* PresentName */
			      CopyWord (pSSchema->SsDefaultPSchema, wi, wl);
			      break;
			      case RULE_AttrName
			   /* AttrName */ :
			      attrNum = Identifier[nb - 1].SrcIdentDefRule;
			      if (attrNum == 0)
				 /* new name within the schema */
				 if (ustrncmp (&inputLine[wi - 1], pSSchema->SsAttribute[0].AttrName, wl) == 0)
				    /* it's the language attribute */
				    attrNum = 1;
			      if (CompilAttr || CompilLocAttr)
				 /* attribute declaration */
				 if (CompilAttr && attrNum > 0)
				    /* try to redefine the attribute */
				    CompilerMessage (wi, STR, FATAL, STR_ATTR_ALREADY_DECLARED, inputLine, LineNum);
				 else
				   {
				      if (attrNum > 0)
					{
					   /* attribute already defined */
					   if (pSSchema->SsAttribute[attrNum - 1].AttrGlobal)
					      /* the global attribute cannot be */
					      /* used as local attribute */
					      CompilerMessage (wi, STR, FATAL, STR_GLOBAL_ATTR, inputLine, LineNum);
					}
				      else
					 /* new attribute */
				      if (pSSchema->SsNAttributes >= MAX_ATTR_SSCHEMA)
					 CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_ATTRS, inputLine, LineNum);
				      /* table of attributes is full */
				      else
					{
					   pSSchema->SsNAttributes++;
					   Identifier[nb - 1].SrcIdentDefRule = pSSchema->SsNAttributes;
					   pAttr = &pSSchema->SsAttribute[pSSchema->SsNAttributes - 1];
					   if (CompilLocAttr)
					      /* local attribute */
					      pAttr->AttrGlobal = False;
					   CopyWord (pAttr->AttrName, wi, wl);
					   pAttr->AttrType = AtEnumAttr;
					   pAttr->AttrNEnumValues = 0;
					}
				      if (CompilLocAttr)
					 /* local attribute */
					 if (CurNLocAttr >= MAX_LOCAL_ATTR)
					    /* too many local attributes for this element */
					    CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_ATTRS, inputLine, LineNum);
					 else if (CompilExtens)
					    /* within extension rule */
					   {
					      CurExtensRule->SrNLocalAttrs++;
					      CurExtensRule->SrLocalAttr[CurExtensRule->SrNLocalAttrs - 1] = pSSchema->SsNAttributes;
					      CurExtensRule->SrRequiredAttr[CurExtensRule->SrNLocalAttrs - 1] = MandatoryAttr;
					      MandatoryAttr = False;
					   }
					 else
					    /* within structure rule */
					   {
					      CurLocAttr[CurNLocAttr] = Identifier[nb - 1].
						 SrcIdentDefRule;
					      CurReqAttr[CurNLocAttr] = MandatoryAttr;
					      CurNLocAttr++;
					      MandatoryAttr = False;
					   }
				   }
			      else if (CompilExcept)
				 /* attribute name within exceptions set */
				{
				   ExceptAttr = AttributeNumber (wl, wi);
				   /* number of this attribute */
				   if (ExceptAttr == 0)
				      /* it's an attribute declared in the */
				      /* schema */
				      /* it can be an element type */
				     {
					ExceptType = RuleNumber (wl, wi);
					if (ExceptType == 0)
					   CompilerMessage (wi, STR, FATAL, STR_TYPE_OR_ATTR_UNKNOWN, inputLine, LineNum);
					else if (pSSchema->SsRule[ExceptType - 1].SrFirstExcept != 0)
					   CompilerMessage (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, LineNum);
				     }
				   else if (pSSchema->SsAttribute[ExceptAttr - 1].AttrFirstExcept != 0)
				      CompilerMessage (wi, STR, FATAL, STR_THIS_ATTR_ALREADY_HAS_EXCEPTS, inputLine, LineNum);
				}
			      else
				 /* attribute used within a rule with the WITH keyword */
			      if (attrNum == 0)
				 CompilerMessage (wi, STR, FATAL, STR_ATTR_NOT_DECLARED, inputLine, LineNum);
			      else
				{
				   if (CompilExtens)
				      pRule = CurExtensRule;
				   else
				      pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
				   if (pRule->SrNDefAttrs >= MAX_DEFAULT_ATTR)
				      CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_ATTRS, inputLine, LineNum);
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
				   pAttr = &pSSchema->SsAttribute[pSSchema->SsNAttributes - 1];
				   if (pAttr->AttrNEnumValues >= MAX_ATTR_VAL)
				      /* overflow of values list */
				      CompilerMessage (wi, STR, FATAL, STR_TOO_MANY_VALUES, inputLine, LineNum);
				   else
				      /* new value for the attribute */
				     {
					pAttr->AttrNEnumValues++;
					/* add the new value into the list */
					CopyWord (pAttr->AttrEnumValue[pAttr->AttrNEnumValues - 1], wi, wl);
					/* check if the value already exists */
					i = 1;
					while (i < pAttr->AttrNEnumValues && !error)
					   if (!ustrcmp (pAttr->AttrEnumValue[i - 1], pAttr->AttrEnumValue[pAttr->AttrNEnumValues - 1]))
					      /* the same value two times */
					      CompilerMessage (wi, STR, FATAL, STR_VALUE_ALREADY_DECLARED, inputLine, LineNum);
					   else
					      i++;
					if (!error)
					   /* it's a new value */
					  {
					     Identifier[nb - 1].SrcIdentDefRule = pAttr->AttrNEnumValues;
					     Identifier[nb - 1].SrcIdentRefRule = pSSchema->SsNAttributes;
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
				      pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
				   pAttr = &pSSchema->SsAttribute[pRule->SrDefAttr[pRule->SrNDefAttrs - 1] - 1];
				   if (pAttr->AttrType != AtEnumAttr)
				      CompilerMessage (wi, STR, FATAL,
						     STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, LineNum);
				   else
				     {
					/* check if the value already exists */
					i = 1;
					ok = False;
					CopyWord (N, wi, wl);
					while (i <= pAttr->AttrNEnumValues && !ok)
					   if (ustrcmp (N, pAttr->AttrEnumValue[i - 1]) == 0)
					      ok = True;
					   else
					      i++;
					if (!ok)
					   CompilerMessage (wi, STR, FATAL, STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, LineNum);
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
				   CurParam = False;
				   CurAssoc = False;
				   CurUnit = False;
				   NewRule (wi);
				   /* cree une regle */
				   if (!error)
				     {
					pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
					pRule->SrConstruct = CsConstant;
					pRule->SrIndexConst = TextConstPtr;
				     }
				}
			      else if (Identifier[nb - 1].SrcIdentDefRule == 0)
				/* constant used within an undefined rule */
				CompilerMessage (wi, STR, FATAL, STR_CONSTANT_NOT_DECLARED, inputLine, LineNum);
			      else if (pr == RULE_Constr)
				 /* constant used within structure rule */
				 BasicEl (nb + MAX_BASIC_TYPE, wi, pr);
			      else if (pr == RULE_ExceptType)
				 /* constant within exceptions set */
				 ExceptType = RuleNumber (wl, wi);

			      if (ExceptType > 0 && pSSchema->SsRule[ExceptType - 1].SrFirstExcept != 0)
				 CompilerMessage (wi, STR, FATAL, STR_THIS_TYPE_ALREADY_HAS_EXCEPTS, inputLine, LineNum);
			      break;
			   default:
			      break;
			}
		  break;
	       case 3002:
		  /* un nombre */
		  SynInteger = AsciiToInt (wi, wl);
		  /* le nombre lu */
		  if (r == RULE_Integer)
		     /* check min. and max. numbers in a list */
		    {
		       pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
		       if (Minimum)
			  /* min */
			  pRule->SrMinItems = SynInteger;
		       else if (Maximum)
			  /* max */
			 {
			    pRule->SrMaxItems = SynInteger;
			    if (pRule->SrMaxItems < pRule->SrMinItems)
			       CompilerMessage (wi, STR, FATAL, STR_MAXIMUM_LOWER_THAN_MINIMUM, inputLine, LineNum);
			 }
		    }
		  if (r == RULE_NumValue)
		     /* attribute value fixed */
		    {
		       if (CompilExtens)
			  pRule = CurExtensRule;
		       else
			  pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
		       if (pSSchema->SsAttribute[pRule->SrDefAttr[pRule->SrNDefAttrs - 1] - 1].AttrType != AtNumAttr)
			  CompilerMessage (wi, STR, FATAL, STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, LineNum);
		       else
			 {
			    pRule->SrDefAttrValue[pRule->SrNDefAttrs - 1] = SynInteger * Sign;
			    /* default sign for the value */
			    Sign = 1;
			 }
		    }
		  if (r == RULE_ExceptNum)
		     /* exception number associated with element type */
		     /* or attribute */
		     if (SynInteger <= 100)
			/* values less than 100 are reserved for predefined */
			/* exceptions */
			CompilerMessage (wi, STR, FATAL, STR_THIS_NUMBER_MUST_BE_GREATER_THAN_100, inputLine, LineNum);
		     else
			ExceptionNum (SynInteger, False, False, False, wi);
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
			  pRule = &pSSchema->SsRule[CurRule[RecursLevel - 1] - 1];
		       if (pSSchema->SsAttribute[pRule->SrDefAttr[
			pRule->SrNDefAttrs - 1] - 1].AttrType != AtTextAttr)
			  CompilerMessage (wi, STR, FATAL, STR_INVALID_VALUE_FOR_THAT_ATTR, inputLine, LineNum);
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
#ifdef __STDC__
static void         ExternalTypes ()
#else  /* __STDC__ */
static void         ExternalTypes ()
#endif /* __STDC__ */
{
   int                 i, j;
   SRule              *pRule;

   /* go through the table of external types */
   for (j = 0; j < NExternalTypes; j++)
      /* search the external type in the schema starting from the end */
     {
	i = pSSchema->SsNRules - 1;
	while (ustrcmp (ExternalType[j], pSSchema->SsRule[i].SrName) != 0)
	   i--;
	/* the external type is defined by the i rule number */
	pRule = &pSSchema->SsRule[i];
	if (pRule->SrConstruct != CsNatureSchema)
	   /* it's not an external SyntacticType */
	  {
	     if (IncludedExternalType[j])
		CompilerMessageString (0, STR, INFO, STR_CANNOT_BE_INCLUDED, inputLine, LineNum, pRule->SrName);
	     else
		CompilerMessageString (0, STR, INFO, STR_CANNOT_BE_EXTERN, inputLine, LineNum, pRule->SrName);
	  }
	else
	   /* modify the CsNatureSchema rule */
	  {
	    /* don't read the schema if it is the schema currently compiled */
	    if (ustrcmp(pRule->SrName, pSSchema->SsName) == 0)
	      pRule->SrReferredType = pSSchema->SsRootElem;
	    else
	      if (!ReadStructureSchema (pRule->SrName, pExternSSchema))
		{
		  /* cannot read the external schema */
		  CompilerMessageString (0, STR, INFO, STR_EXTERNAL_STRUCT_NOT_FOUND, inputLine, LineNum, pRule->SrName);
		  /* even if the external schema doesn't exist, modify the rule */
		  pRule->SrReferredType = MAX_BASIC_TYPE + 1;
		}
	      else
		pRule->SrReferredType = pExternSSchema->SsRootElem;
	     /* change the rule into CsReference rule */
	     ustrncpy (pRule->SrRefTypeNat, pRule->SrName, MAX_NAME_LENGTH);
	     pRule->SrRefImportedDoc = True;
	     pRule->SrConstruct = CsReference;
	  }
     }
}

/*----------------------------------------------------------------------
   CheckRecursivity                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckRecursivity (int r, int path[], int level, ThotBool busy[], ThotBool done[])
#else  /* __STDC__ */
static void         CheckRecursivity (r, path, level, busy, done)
int                 r;
int                 path[];
int                 level;
ThotBool             busy[];
ThotBool             done[];
#endif /* __STDC__ */
{
   int                 m;
   SRule              *pRule;

   pRule = &pSSchema->SsRule[r - 1];
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
	     pSSchema->SsRule[path[m] - 1].SrRecursive = True;
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
		  CheckRecursivity (pRule->SrChoice[m],
				    path, level + 1, busy, done);
	       break;
	    case CsIdentity:
	       CheckRecursivity (pRule->SrIdentRule,
				 path, level + 1, busy, done);
	       break;
	    case CsList:
	       CheckRecursivity (pRule->SrListItem,
				 path, level + 1, busy, done);
	       break;
	    case CsUnorderedAggregate:
	    case CsAggregate:
	       for (m = 0; m < pRule->SrNComponents; m++)
		  CheckRecursivity (pRule->SrComponent[m],
				    path, level + 1, busy, done);
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
#ifdef __STDC__
static void         ChkRecurs ()
#else  /* __STDC__ */
static void         ChkRecurs ()
#endif /* __STDC__ */
{
   int                 i;
   int                 path[100];
   ThotBool             busy[MAX_RULES_SSCHEMA + 1], done[MAX_RULES_SSCHEMA + 1];
   SRule              *pRule;

   for (i = 0; i <= pSSchema->SsNRules; i++)
      busy[i] = done[i] = False;

   for (i = MAX_BASIC_TYPE + 1; i <= pSSchema->SsNRules; i++)
      CheckRecursivity (i, path, 0, busy, done);

   for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
     {
	pRule = &pSSchema->SsRule[i];
	if (pRule->SrRecursive)
	   TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_RECURSIVE_ELEM), pRule->SrName);
     }
}

/*----------------------------------------------------------------------
   ListAssocElem       liste les elements consideres comme		
   parametres et elements associes                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ListAssocElem ()
#else  /* __STDC__ */
static void         ListAssocElem ()
#endif /* __STDC__ */
{
   int                 i;
   SRule              *pRule;

   /* go through all rules table */
   for (i = MAX_BASIC_TYPE; i < pSSchema->SsNRules; i++)
     {
	if (pSSchema->SsRule[i].SrParamElem)
	   /* display a message */
	   TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_PARAMETER), pSSchema->SsRule[i].SrName);
	if (pSSchema->SsRule[i].SrAssocElem)
	   if (!pSSchema->SsRule[i].SrRecursDone)
	      /* the associated element associe is used within another rule */
	     {
		CompilerMessageString (0, STR, INFO, STR_THE_ASSOC_ELEM_IS_USED_IN_ANOTHER_RULE, inputLine, LineNum, pSSchema->SsRule[i].SrName);
		error = True;
	     }
	   else
	      /* create a new list rule at the end of rules table */
	     {
		if (pSSchema->SsNRules >= MAX_RULES_SSCHEMA)
		   TtaDisplaySimpleMessage (FATAL, STR, STR_TOO_MAN_RULES);
		/* table of rules is full */
		else
		   pSSchema->SsNRules++;
		pRule = &pSSchema->SsRule[pSSchema->SsNRules - 1];
		/* the new list rule takes the name of its elements... */
		ustrncpy (pRule->SrName, pSSchema->SsRule[i].SrName, MAX_NAME_LENGTH - 2);
		/* ... followed by 's' */
		pRule->SrName[MAX_NAME_LENGTH - 2] = '\0';
		ustrcat (pRule->SrName, TEXT("s"));
		pRule->SrNDefAttrs = 0;
		pRule->SrNLocalAttrs = 0;
		pRule->SrNInclusions = 0;
		pRule->SrNExclusions = 0;
		pRule->SrRefImportedDoc = False;
		pRule->SrAssocElem = False;
		pRule->SrParamElem = False;
		pRule->SrUnitElem = False;
		pRule->SrRecursive = False;
		pRule->SrExportedElem = False;
		pRule->SrFirstExcept = 0;
		pRule->SrLastExcept = 0;
		pRule->SrConstruct = CsList;
		pRule->SrListItem = i + 1;
		pRule->SrMinItems = 0;
		pRule->SrMaxItems = 32000;
		/* write a message */
		TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_ASSOC_ELEMS), pRule->SrName);
		if (RuleNameExist ())
		   TtaDisplaySimpleMessage (FATAL, STR, STR_NAME_ALREADY_DECLARED);
	     }
	else
	   /* it's not a associated element */
	if (pSSchema->SsRule[i].SrRecursDone)
	   if (i + 1 != pSSchema->SsRootElem && !pSSchema->SsRule[i].SrUnitElem)
	     {
		if (pSSchema->SsRule[i].SrConstruct == CsChoice)
		   /* it's a choice that defines an alias */
		   /* insert the element in table of aliases */
		  {
		     NAlias++;
		     Alias[NAlias - 1] = i + 1;
		     TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_ALIAS), pSSchema->SsRule[i].SrName);
		  }
		else
		   /* the second elements of a CsPairedElement are never referenced */
		   if (pSSchema->SsRule[i].SrConstruct != CsPairedElement ||
		       pSSchema->SsRule[i].SrFirstOfPair)
		   /* unnecessary definition */
		   TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_UNUSED), pSSchema->SsRule[i].SrName);
	     }
     }
}


/*----------------------------------------------------------------------
   ListNotCreated lists elements which will not be created by the editor.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ListNotCreated ()
#else  /* __STDC__ */
static void         ListNotCreated ()
#endif /* __STDC__ */
{
   int                 r, rr;
   int                 i;
   ThotBool             temp;
   SRule              *pRule;
   SRule              *pRule2;

   /* clear all creation indicators */
   /* (use SrRecursDone as creation incator) */
   for (r = 0; r < pSSchema->SsNRules; r++)
      pSSchema->SsRule[r].SrRecursDone = False;
   /* we will create the root */
   if (pSSchema->SsRootElem > 0)
      pSSchema->SsRule[pSSchema->SsRootElem - 1].SrRecursDone = True;
   /* go through the table of rules */
   for (r = 0; r < pSSchema->SsNRules; r++)
     {
	pRule = &pSSchema->SsRule[r];
	if (pRule->SrParamElem)
	   /* parameters will be create */
	   pRule->SrRecursDone = True;
	/* included elements (SGML) will be created */
	if (pRule->SrNInclusions > 0)
	   for (i = 0; i < pRule->SrNInclusions; i++)
	      pSSchema->SsRule[pRule->SrInclusion[i] - 1].SrRecursDone = True;
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
		    pSSchema->SsRule[pRule->SrListItem - 1].SrRecursDone = True;
		    /* associated list elements will be created */
		    if (pSSchema->SsRule[pRule->SrListItem - 1].SrAssocElem)
		       pRule->SrRecursDone = True;
		    break;
		 case CsChoice:
		    /* choice elements will be created */
		    if (pRule->SrNChoices > 0)
		      {
			 for (i = 0; i < pRule->SrNChoices; i++)
			   {
			      pSSchema->SsRule[pRule->SrChoice[i] - 1].SrRecursDone = True;
			      /* if the choice is a exported unit, */
			      /* its options will become exported units */
			      if (pRule->SrUnitElem)
				 pSSchema->SsRule[pRule->SrChoice[i] - 1].SrUnitElem = True;
			   }
			 pRule->SrUnitElem = False;
		      }
		    break;
		 case CsUnorderedAggregate:
		 case CsAggregate:
		    /* agreggate components will be created */
		    for (i = 0; i < pRule->SrNComponents; i++)
		       pSSchema->SsRule[pRule->SrComponent[i] - 1].SrRecursDone = True;
		    break;
		 case CsConstant:
		    /* constants will be created */
		    pRule->SrRecursDone = True;
		    break;
		 default:
		    break;
	      }
     }

   /* write the result */
   for (r = 0; r < pSSchema->SsNRules; r++)
     {
	pRule = &pSSchema->SsRule[r];
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
		   if (pSSchema->SsRule[rr].SrConstruct == CsAggregate ||
		   pSSchema->SsRule[rr].SrConstruct == CsUnorderedAggregate)
		      for (i = 0; i < pSSchema->SsRule[rr].SrNComponents; i++)
			 if (pSSchema->SsRule[rr].SrComponent[i] == r + 1)
			    temp = False;
	     if (temp)
		/* is it an alias ? Search in the aliases table */
		for (i = 0; i < NAlias; i++)
		   if (Alias[i] == r + 1)
		      /* element found, don't create */
		      temp = False;
	     if (temp)
		TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_IS_A_TEMPORARY_ELEM), pRule->SrName);
	  }
	else if (!pRule->SrRecursDone)
	   /* units cannot used in the schema */
	   if (!pRule->SrUnitElem)
	     {
		TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_WON_T_BE_CREATED), pRule->SrName);
		/* search if there are REFERENCES to this element type */
		for (rr = 0; rr < pSSchema->SsNRules; rr++)
		  {
		     pRule2 = &pSSchema->SsRule[rr];
		     if (pRule2->SrConstruct == CsReference)
			if (pRule2->SrRefTypeNat[0] == '\0')
			   if (pRule2->SrReferredType == r + 1)
			      TtaDisplayMessage (INFO, TtaGetMessage (STR, STR_WON_T_BE_CREATED_AND_IS_REFD), pRule->SrName);
		  }
	     }
     }
}


/*----------------------------------------------------------------------
   main                                                            
  ----------------------------------------------------------------------*/
#ifdef _WINDOWS 
#ifdef __STDC__
int                 STRmain (HWND hwnd, HWND statusBar, int argc, STRING *argv, int* Y)
#else  /* __STDC__ */
int                 STRmain (hwnd, argc, argv, hDC, Y)
HWND                hwnd;
int                 argc;
STRING*              argv;
int*                Y;
#endif /* __STDC__ */
#else  /* !_WINDOWS */
#ifdef __STDC__
int                 main (int argc, char **argv)
#else  /* __STDC__ */
int                 main (argc, argv)
int                 argc;
char              **argv;
#endif /* __STDC__ */
#endif /* _WINDOWS */
{
   FILE               *inputFile;
   CHAR_T              buffer[200], fname[200];
   STRING              pwd, ptr;
   indLine             i;	/* current position in current line */
   indLine             wi;	/* start position of current word in the line */
   indLine             wl;	/* word length */
   SyntacticType       wn;	/* syntactic type of current word */
   SyntRuleNum         r;	/* rule number */
   SyntRuleNum         pr;	/* previous rule number */
   SyntacticCode       c;	/* grammatical code of found word */
   ThotBool            fileOK;
   int                 nb;	/* identifier index of found word if it is
				   an indentifier */
   int                 LIBDLG;
   int                 param;
   unsigned char       car;
#  ifdef _WINDOWS 
   char*               cmd [100];
   int                 ndx, pIndex = 0;
   CHAR_T              msg [800];
   HANDLE              cppLib;
   FARPROC             ptrMainProc;
#  else  /* !_WINDOWS */
   char                cmd[800];
#  endif /* _WINDOWS */

#  ifdef _WINDOWS
   COMPWnd = hwnd;
   compilersDC = GetDC (hwnd);
   _CY_ = *Y;
   ustrcpy (msg, TEXT("Executing str "));
   for (ndx = 1; ndx < argc; ndx++) {
       ustrcat (msg, argv [ndx]);
       ustrcat (msg, TEXT(" "));
   }

   TtaDisplayMessage (INFO, msg);

   SendMessage (statusBar, SB_SETTEXT, (WPARAM) 0, (LPARAM) &msg[0]);
   SendMessage (statusBar, WM_PAINT, (WPARAM) 0, (LPARAM) 0);
#  endif /* _WINDOWS */

   TtaInitializeAppRegistry (argv[0]);
   (void) TtaGetMessageTable (TEXT("libdialogue"), TMSG_LIB_MSG_MAX);
   COMPIL = TtaGetMessageTable (TEXT("compildialogue"), COMP_MSG_MAX);
   STR = TtaGetMessageTable (TEXT("strdialogue"), STR_MSG_MAX);
   error = False;
   /* initialize the parser */
   InitParser ();
   InitSyntax (TEXT("STRUCT.GRM"));
   if (!error) {
      /* prepare the cpp command */
#     ifdef _WINDOWS 
      cmd [pIndex] = TtaGetMemory (4);
      strcpy (cmd [pIndex++], "cpp");
#     else  /* !_WINDOWS */
      strcpy (cmd, CPP " ");
#     endif /* _WINDOWS */
      param = 1;
      while (param < argc && argv[param][0] == '-') {
            /* keep cpp params */
#           ifdef _WINDOWS
            cmd [pIndex] = TtaGetMemory (ustrlen (argv[param]) + 1);
            wc2iso_strcpy (cmd [pIndex++], argv[param]);
#           else  /* !_WINDOWS */
            strcat (cmd, argv[param]);
            strcat (cmd, " ");
#           endif /* _WINDOWS */
            param++;
	  }
      /* keep the name of the schema to be compile */
      if (param >= argc) {
	     TtaDisplaySimpleMessage (FATAL, STR, STR_NO_SUCH_FILE);
#        ifdef _WINDOWS 
         ReleaseDC (hwnd, compilersDC);
         return FATAL_EXIT_CODE;
#        else  /* _WINDOWS */
	     exit (1);
#        endif /* _WINDOWS */
	  } 

      /* get the name of the file to be compiled */
      ustrncpy (srceFileName, argv[param], MAX_NAME_LENGTH - 1);
      srceFileName[MAX_NAME_LENGTH - 1] = '\0';
      param++;
      ustrcpy (fname, srceFileName);
      /* check if the name contains a suffix */
      ptr = ustrrchr(fname, '.');
      nb = ustrlen (srceFileName);
      if (!ptr) /* there is no suffix */
         ustrcat (srceFileName, TEXT(".S"));
      else if (ustrcmp (ptr, TEXT(".S"))) {
           /* it's not the valid suffix */
           TtaDisplayMessage (FATAL, TtaGetMessage (STR, STR_INVALID_FILE), srceFileName);
#          ifdef _WINDOWS 
           ReleaseDC (hwnd, compilersDC);
           return FATAL_EXIT_CODE;
#          else  /* _WINDOWS */
           exit (1);
#          endif /* _WINDOWS */
	  }	else {
             /* it's the valid suffix, cut the srcFileName here */
             ptr[0] = '\0';
             nb -= 2; /* length without the suffix */
	  } 
      /* add the suffix .SCH in srceFileName */
      ustrcat (fname, TEXT(".SCH"));
	
      /* does the file to compile exist */
      if (TtaFileExist (srceFileName) == 0)
         TtaDisplaySimpleMessage (FATAL, STR, STR_NO_SUCH_FILE);
      else {
           /* provide the real source file */
           TtaFileUnlink (fname);
           pwd = TtaGetEnvString ("PWD");
#          ifndef _WINDOWS
           i = strlen (cmd);
#          endif /* _WINDOWS */
           if (pwd != NULL) {
#             ifdef _WINDOWS
              CHAR_T* CMD;
              CMD = TtaAllocString (3 + ustrlen (pwd));
              usprintf (CMD, TEXT("-I%s"), pwd);
              cmd [pIndex] = TtaGetMemory (3 + ustrlen (pwd));
              wc2iso_strcpy (cmd [pIndex++], CMD);
              cmd [pIndex] = TtaGetMemory (3);
              strcpy (cmd [pIndex++], "-C");
              cmd [pIndex] = TtaGetMemory (ustrlen (srceFileName) + 1);
              wc2iso_strcpy (cmd [pIndex++], srceFileName);
              cmd [pIndex] = TtaGetMemory (ustrlen (fname) + 1);
              wc2iso_strcpy (cmd [pIndex++], fname);
#             else  /* !_WINDOWS */
              sprintf (&cmd[i], "-I%s -C %s > %s", pwd, srceFileName, fname);
#             endif /* _WINDOWS */
		   } else {
#                 ifdef _WINDOWS
                  cmd [pIndex] = TtaGetMemory (3);
                  strcpy (cmd [pIndex++], "-C");
                  cmd [pIndex] = TtaGetMemory (ustrlen (srceFileName) + 1);
                  wc2iso_strcpy (cmd [pIndex++], srceFileName);
                  cmd [pIndex] = TtaGetMemory (2);
                  strcpy (cmd [pIndex++], ">");
                  cmd [pIndex] = TtaGetMemory (ustrlen (fname) + 1);
                  wc2iso_strcpy (cmd [pIndex++], fname);
#                 else  /* !_WINDOWS */
                  sprintf (&cmd[i], "-C %s > %s", srceFileName, fname);
#                 endif /* _WINDOWS */
		   } 
#          ifdef _WINDOWS
           cppLib = LoadLibrary (TEXT("cpp"));
           ptrMainProc = GetProcAddress (cppLib, "CPPmain");
           i = ptrMainProc (hwnd, pIndex, cmd, &_CY_);
           FreeLibrary (cppLib);
           for (ndx = 0; ndx < pIndex; ndx++) {
               free (cmd [ndx]);
               cmd [ndx] = (char*) 0;
		   }
#          else  /* !_WINDOWS */
           i = system (cmd);
#          endif /* _WINDOWS */
           if (i == FATAL_EXIT_CODE) {
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
           while (fileOK && !error) {
                 /* read a line */
                 i = 0;
                 do {
                    fileOK = TtaReadByte (inputFile, &car);
                    inputLine[i] = car;
                    /* fileOK = TtaReadByte (inputFile, &inputLine[i]); */
                    i++;
				 /* } while (i < LINE_LENGTH && inputLine[i - 1] != WC_EOS && fileOK); */
				 } while (i < LINE_LENGTH && car != '\n' && fileOK);
                 /* mark the real end of line */
                 inputLine[i - 1] = WC_EOS;
                 /* increment lines counter */
                 LineNum++;
                 if (i >= LINE_LENGTH)
                    CompilerMessage (1, STR, FATAL, STR_LINE_TOO_LONG, inputLine, LineNum);
                 else if (inputLine[0] == TEXT('#')) {
                      /* this line contains a cpp directive */
                      usscanf (inputLine, TEXT("# %d %s"), &LineNum, buffer);
                      LineNum--;
				 } else {
                        /* translate line characters */
                        OctalToChar ();
                        /* analyze the line */
                        wi = 1;
                        wl = 0;
                        /* analyze all words in the current line */
                        do {
                           i = wi + wl;
                           /* next word */
                           GetNextToken (i, &wi, &wl, &wn);
                           if (wi > 0) { /* word found */
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
           if (!error) /* set right rule number when we met the keyword THOT_EXPORT */
              if (!pSSchema->SsExport)
                 ChangeRules ();
           if (!error) /* list external type names */
              ExternalTypes ();
           if (!error) {
              /* list recursive rules */
              ChkRecurs ();
              /* list associated elements */
              ListAssocElem ();
              /* list elements which will not be created by the editor */
              ListNotCreated ();
		
              /* write the compiled schema into the output file */
              SchemaPath[0] = '\0';	/* use current directory */
              if (!error) {
                 /* remove temporary file */
                 TtaFileUnlink (fname);
                 ustrcat (srceFileName, TEXT(".STR"));
                 fileOK = WriteStructureSchema (srceFileName, pSSchema, 0);
                 if (!fileOK)
                    TtaDisplayMessage (FATAL, TtaGetMessage (STR, STR_CANNOT_WRITE), srceFileName);
			  } 
		   } 
           free (pSSchema);
           free (pExternSSchema);
	  } 
   } 
   fflush (stdout);
   TtaSaveAppRegistry ();
#  ifdef _WINDOWS
   *Y = _CY_ ;
   ReleaseDC (hwnd, compilersDC);
   return COMP_SUCCESS;
#  else  /* _WINDOWS */
   exit (0);
#  endif /* _WINDOWS */
}
