/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
   This module loads a structure schema from a .STR file
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "zlib.h"
#include "fileaccess.h"

/* variables importees */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "fileaccess_f.h"
#include "platform_tv.h"
#include "readstr_f.h"
#include "memory_f.h"

/*----------------------------------------------------------------------
   ReadAttribType                                                  
   lit un type d'attribut et retourne sa valeur.   		
  ----------------------------------------------------------------------*/
static ThotBool ReadAttribType (BinFile file, AttribType *attrType)
{
   char c;

   TtaReadByte (file, (unsigned char *)&c);
   switch (c)
	 {
	    case C_INT_ATTR:
	       *attrType = AtNumAttr;
	       break;
	    case C_TEXT_ATTR:
	       *attrType = AtTextAttr;
	       break;
	    case C_REF_ATTR:
	       *attrType = AtReferenceAttr;
	       break;
	    case C_ENUM_ATTR:
	       *attrType = AtEnumAttr;
	       break;
	    default:
	       *attrType = AtEnumAttr;
	       return FALSE;
	       break;
	 }
   return TRUE;
}

/*----------------------------------------------------------------------
  ReadRConstruct							
  lit un constructeur et retourne sa valeur.                      
  ----------------------------------------------------------------------*/
static ThotBool ReadRConstruct (BinFile file, RConstruct *constr)
{
   char c;

   TtaReadByte (file, (unsigned char *)&c);
   switch (c)
	 {
	    case C_IDENTITY_CONSTR:
	       *constr = CsIdentity;
	       break;
	    case C_LIST_CONSTR:
	       *constr = CsList;
	       break;
	    case C_CHOICE_CONSTR:
	       *constr = CsChoice;
	       break;
	    case C_AGG_CONSTR:
	       *constr = CsAggregate;
	       break;
	    case C_UNORD_AGG_CONSTR:
	       *constr = CsUnorderedAggregate;
	       break;
	    case C_CONST_CONSTR:
	       *constr = CsConstant;
	       break;
	    case C_REF_CONSTR:
	       *constr = CsReference;
	       break;
	    case C_BASIC_TYPE_CONSTR:
	       *constr = CsBasicElement;
	       break;
	    case C_NATURE_CONSTR:
	       *constr = CsNatureSchema;
	       break;
	    case C_PAIR_CONSTR:
	       *constr = CsPairedElement;
	       break;
	    case C_EXTENS_CONSTR:
	       *constr = CsExtensionRule;
	       break;
	    case C_DOCUMENT_CONSTR:
	       *constr = CsDocument;
	       break;
	    case C_ANY_CONSTR:
	       *constr = CsAny;
	       break;
	    case C_EMPTY_CONSTR:
	       *constr = CsEmpty;
	       break;
	    default:
	       *constr = CsIdentity;
	       return FALSE;
	 }
   return TRUE;
}

/*----------------------------------------------------------------------
   ReadBasicType                                                   
   lit un type de base et retourne sa valeur.      		
  ----------------------------------------------------------------------*/
static ThotBool ReadBasicType (BinFile file, BasicType *typ)
{
   char c;

   TtaReadByte (file, (unsigned char *)&c);
   switch (c)
	 {
	    case C_CHAR_STRING:
	       *typ = CharString;
	       break;
	    case C_GRAPHICS:
	       *typ = GraphicElem;
	       break;
	    case C_SYMBOL:
	       *typ = Symbol;
	       break;
	    case C_PICTURE:
	       *typ = tt_Picture;
	       break;
	    case C_REFER:
	       *typ = Refer;
	       break;
	    case C_PAGE_BREAK:
	       *typ = PageBreak;
	       break;
	    case C_ANY_TYPE:
	       *typ = AnyType;
	       break;
	    default:
	       *typ = CharString;
	       return FALSE;
	 }
   return TRUE;
}

/*----------------------------------------------------------------------
   ReadAttribute                                			
  ----------------------------------------------------------------------*/
static ThotBool     ReadAttribute (BinFile file, PtrTtAttribute pAttr)
{
   AttribType          attrType;
   int                 j;
   PathBuffer          buffer;

   j = 0;
   do
     TtaReadByte (file, (unsigned char *)&buffer[j++]);
   while (buffer[j - 1] != EOS && j < MAX_PATH);
   pAttr->AttrName = TtaStrdup (buffer);
   pAttr->AttrOrigName = TtaStrdup (buffer);
   TtaReadBool (file, &pAttr->AttrGlobal);
   TtaReadShort (file, &pAttr->AttrFirstExcept);
   TtaReadShort (file, &pAttr->AttrLastExcept);
   if (ReadAttribType (file, &attrType))
     {
	pAttr->AttrType = attrType; 
	switch (pAttr->AttrType)
	      {
		 case AtNumAttr:
		 case AtTextAttr:
		    break;
		 case AtReferenceAttr:
		    TtaReadShort (file, &pAttr->AttrTypeRef);
		    TtaReadName (file, (unsigned char *)pAttr->AttrTypeRefNature);
		    break;
		 case AtEnumAttr:
		    TtaReadShort (file, &pAttr->AttrNEnumValues);
		    for (j = 0; j < pAttr->AttrNEnumValues; j++)
		      {
			TtaReadName (file, (unsigned char *)pAttr->AttrEnumValue[j]);
			strcpy (pAttr->AttrEnumOrigValue[j], pAttr->AttrEnumValue[j]);
		      }
		    break;
	      }
     }
   else 
      return FALSE;
   return TRUE;
}

/*----------------------------------------------------------------------
   ReadSRule                                  			
  ----------------------------------------------------------------------*/
static ThotBool     ReadSRule (BinFile file, PtrSRule pSRule)
{
   RConstruct          constr;
   int                 j;
   PathBuffer          buffer;

   j = 0;
   do
     TtaReadByte (file, (unsigned char *)&buffer[j++]);
   while (buffer[j - 1] != EOS && j < MAX_PATH);
   pSRule->SrName = TtaStrdup (buffer); 
   pSRule->SrOrigName = TtaStrdup (buffer);
   TtaReadShort (file, &pSRule->SrNDefAttrs);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      TtaReadShort (file, &pSRule->SrDefAttr[j]);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      TtaReadSignedShort (file, &pSRule->SrDefAttrValue[j]);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      TtaReadBool (file, &pSRule->SrDefAttrModif[j]);

   TtaReadShort (file, &pSRule->SrNLocalAttrs);
   if (pSRule->SrNLocalAttrs <= 0)
     {
       pSRule->SrLocalAttr = NULL;
       pSRule->SrRequiredAttr = NULL;
     }
   else
     {
       pSRule->SrLocalAttr = (NumTable*) TtaGetMemory (pSRule->SrNLocalAttrs * sizeof (int));
       for (j = 0; j < pSRule->SrNLocalAttrs; j++)
	 TtaReadShort (file, &pSRule->SrLocalAttr->Num[j]);
       pSRule->SrRequiredAttr = (BoolTable*) TtaGetMemory (pSRule->SrNLocalAttrs * sizeof (ThotBool));
       for (j = 0; j < pSRule->SrNLocalAttrs; j++)
	 TtaReadBool (file, &pSRule->SrRequiredAttr->Bln[j]);
     }

   TtaReadBool (file, &pSRule->SrUnitElem);
   TtaReadBool (file, &pSRule->SrRecursive);
   pSRule->SrRecursDone = FALSE;

   TtaReadBool (file, &pSRule->SrExportedElem);
   if (pSRule->SrExportedElem)
     {
	TtaReadShort (file, &pSRule->SrExportContent);
	TtaReadName (file, (unsigned char *)pSRule->SrNatExpContent);
     }

   TtaReadShort (file, &pSRule->SrFirstExcept);
   TtaReadShort (file, &pSRule->SrLastExcept);

   TtaReadShort (file, &pSRule->SrNInclusions);
   for (j = 0; j < pSRule->SrNInclusions; j++)
      TtaReadShort (file, &pSRule->SrInclusion[j]);

   TtaReadShort (file, &pSRule->SrNExclusions);
   for (j = 0; j < pSRule->SrNExclusions; j++)
      TtaReadShort (file, &pSRule->SrExclusion[j]);
   TtaReadBool (file, &pSRule->SrRefImportedDoc);
   if (!ReadRConstruct (file, &constr))
      return FALSE;

   pSRule->SrConstruct = constr;
   switch (pSRule->SrConstruct)
	 {
	    case CsNatureSchema:
	       pSRule->SrOrigNat = TtaStrdup (pSRule->SrName);
	       pSRule->SrSSchemaNat = NULL;
	       break;
	    case CsBasicElement:
	       if (!ReadBasicType (file, &pSRule->SrBasicType))
		  return FALSE;
	       break;
	    case CsReference:
	       TtaReadShort (file, &pSRule->SrReferredType);
	       TtaReadName (file, (unsigned char *)pSRule->SrRefTypeNat);
	       break;
	    case CsIdentity:
	       TtaReadShort (file, &pSRule->SrIdentRule);
	       break;
	    case CsList:
	       TtaReadShort (file, &pSRule->SrListItem);
	       TtaReadShort (file, &pSRule->SrMinItems);
	       TtaReadShort (file, &pSRule->SrMaxItems);
	       break;
	    case CsChoice:
	       TtaReadSignedShort (file, &pSRule->SrNChoices);
	       for (j = 0; j < pSRule->SrNChoices; j++)
		  TtaReadShort (file, &pSRule->SrChoice[j]);
	       break;
	    case CsAggregate:
	    case CsUnorderedAggregate:
	       TtaReadShort (file, &pSRule->SrNComponents);
	       for (j = 0; j < pSRule->SrNComponents; j++)
		  TtaReadShort (file, &pSRule->SrComponent[j]);
	       for (j = 0; j < pSRule->SrNComponents; j++)
		  TtaReadBool (file, &pSRule->SrOptComponent[j]);
	       break;
	    case CsConstant:
	       TtaReadShort (file, &pSRule->SrIndexConst);
	       break;
	    case CsPairedElement:
	       TtaReadBool (file, &pSRule->SrFirstOfPair);
	       break;
	    case CsExtensionRule:
	    case CsDocument:
	    case CsAny:
	    case CsEmpty:
	       break;
	 }
   return TRUE;
}

/*----------------------------------------------------------------------
   ReadConstants                                          		
  ----------------------------------------------------------------------*/
static ThotBool ReadConstants (BinFile file, PtrSSchema pSS)
{
  char                c;
  int                 i;

   i = 0;
   do
     {
	do
	  {
	     TtaReadByte (file, (unsigned char *)&c);
	     pSS->SsConstBuffer[i++] = c;
	  }
	while (c != EOS && i < MAX_LEN_ALL_CONST);
	TtaReadByte (file, (unsigned char *)&c);
	pSS->SsConstBuffer[i++] = c;
     }
   while (c != EOS && i < MAX_LEN_ALL_CONST);
   if (i >= MAX_LEN_ALL_CONST)
      return FALSE;
   else
      return TRUE;
}

/*----------------------------------------------------------------------
   ReadStructureSchema                                             
   lit un fichier contenant un schema de structure et charge       
   ce schema en memoire.                                           
   fileName: nom du fichier a lire, sans le suffixe .STR (la chaine peut etre modifiee par cette fonction)
   pSS: schema de structure en memoire a remplir.               	
   Retourne VRAI si chargement reussi, FAUX si echec.              
  ----------------------------------------------------------------------*/
ThotBool ReadStructureSchema (const char * fileName, PtrSSchema pSS)
{ 
   BinFile             file;
   PathBuffer          buf;
   PathBuffer          dirBuffer;
   int                 i, num;

   /* compose le nom du fichier a ouvrir */
   strncpy (dirBuffer, SchemaPath, MAX_PATH);
   MakeCompleteName (fileName, "STR", dirBuffer, buf, &i);

   /* ouvre le fichier */
   file = TtaReadOpen (buf);
   if (file == 0)
      /* echec */
     {
	strncpy (buf, fileName, MAX_PATH);
	strcat (buf, ".STR");
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_STR_FILE),
			   buf);
	return FALSE;
     }
   else
     {
	pSS->SsActionList = NULL;
	/* lit la partie fixe du schema de structure */
	if (pSS->SsName)
	  TtaFreeMemory (pSS->SsName);
	i = 0;
	do
	  TtaReadByte (file, (unsigned char *)&buf[i++]);
	while (buf[i - 1] != EOS && i < MAX_PATH);
	buf[MAX_PATH -1] = EOS;
	pSS->SsName = TtaStrdup (buf);
	TtaReadShort (file, &pSS->SsCode);
	i = 0;
	do
	  TtaReadByte (file, (unsigned char *)&buf[i++]);
	while (buf[i - 1] != EOS && i < MAX_PATH);
	buf[MAX_PATH -1] = EOS;
	pSS->SsDefaultPSchema = TtaStrdup (buf);
	TtaReadBool (file, &pSS->SsExtension);
	pSS->SsUriName = NULL;
	pSS->SsNExtensRules = 0;
	pSS->SsExtensBlock = NULL;
	pSS->SsIsXml = FALSE;
	TtaReadShort (file, &pSS->SsDocument);
	TtaReadShort (file, &pSS->SsRootElem);
	TtaReadShort (file, &pSS->SsNAttributes);
	TtaReadShort (file, &pSS->SsNRules);
	TtaReadBool (file, &pSS->SsExport);
	TtaReadShort (file, &pSS->SsNExceptions);
	for (i = 0; i < pSS->SsNExceptions; i++)
	   TtaReadShort (file, &pSS->SsException[i]);

	/* lit le texte des constantes */
	if (!ReadConstants (file, pSS))
	  {
	     /* message 'Fichier .STR incorrect ' */
	     TtaDisplayMessage (INFO,
				TtaGetMessage (LIB, TMSG_INCORRECT_STR_FILE),
				fileName);
	     return FALSE;
	  }

	/* il n'y a pas encore de nature chargee dynamiquement */
	pSS->SsFirstDynNature = 0;

	/* allocate the attribute table */
	if (!pSS->SsAttribute)
	  {
	    num = pSS->SsNAttributes;         /* table size */
	    pSS->SsAttribute = (TtAttrTable*) malloc (num * sizeof (PtrTtAttribute));
	    pSS->SsAttrTableSize = num;
	    for (i = 0; i < num; i++)
	      pSS->SsAttribute->TtAttr[i] = NULL;
	  }

	/* lit les attributs */
	for (i = 0; i < pSS->SsNAttributes; i++)
	  {
	    pSS->SsAttribute->TtAttr[i] = (PtrTtAttribute) malloc (sizeof (TtAttribute));
	    if (!ReadAttribute (file, pSS->SsAttribute->TtAttr[i]))
	      {
		/* message 'Fichier .STR incorrect ' */
		TtaDisplayMessage (INFO, TtaGetMessage (LIB,
					   TMSG_INCORRECT_STR_FILE), fileName);
		return FALSE;
	      }
	  }

	/* allocate the element table */
	if (!pSS->SsRule)
	  {
	    /* reserve 2 additional entries for counter aliases (see function
	       MakeAliasTypeCount in presvariables.c) */
	    num = pSS->SsNRules + 2;         /* table size */
	    pSS->SsRule = (SrRuleTable*) malloc (num * sizeof (PtrSRule));
	    pSS->SsRuleTableSize = num;
	    for (i = 0; i < num; i++)
	      pSS->SsRule->SrElem[i] = NULL;
	  }

	/* lit les elements */
	for (i = 0; i < pSS->SsNRules; i++)
	  {
	    pSS->SsRule->SrElem[i] = (PtrSRule) malloc (sizeof (SRule));
	    if (!ReadSRule (file, pSS->SsRule->SrElem[i]))
	      {
		/* message 'Fichier .STR incorrect ' */
		TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_STR_FILE), fileName);
		return FALSE;
	      }
	  }
	if (pSS->SsExtension)
	  {
	     if (TtaReadShort (file, &pSS->SsNExtensRules))
		if (pSS->SsNExtensRules > 0)
		  {
		     /* acquiert un bloc d'extension */
		     GetExternalBlock (&pSS->SsExtensBlock);
		     if (pSS->SsExtensBlock != NULL)
			/* lit les regles d'extension */
			for (i = 0; i < pSS->SsNExtensRules; i++)
			   if (!ReadSRule (file, &pSS->SsExtensBlock->EbExtensRule[i]))
			     {
				/* message 'Fichier .STR incorrect ' */
				TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_STR_FILE), fileName);
				return FALSE;
			     }
		  }
	  }
	TtaReadClose (file);
	return TRUE;
     }
}
