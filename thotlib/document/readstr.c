/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*
   Ce module charge un schema de structure depuis un fichier .STR
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"

/* variables importees */
#define EXPORT extern
#include "platform_tv.h"


#include "fileaccess_f.h"
#include "readstr_f.h"
#include "memory_f.h"

/*----------------------------------------------------------------------
   ReadAttribType                                                  
   lit un type d'attribut et retourne sa valeur.   		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      ReadAttribType (BinFile file, AttribType * attrType)

#else  /* __STDC__ */
static boolean      ReadAttribType (file, attrType)
BinFile             file;
AttribType       *attrType;

#endif /* __STDC__ */

{
   char                c;

   BIOreadByte (file, &c);
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

#ifdef __STDC__
static boolean      ReadRConstruct (BinFile file, RConstruct * constr)

#else  /* __STDC__ */
static boolean      ReadRConstruct (file, constr)
BinFile             file;
RConstruct       *constr;

#endif /* __STDC__ */

{
   char                c;

   BIOreadByte (file, &c);
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

#ifdef __STDC__
static boolean      ReadBasicType (BinFile file, BasicType * typ)

#else  /* __STDC__ */
static boolean      ReadBasicType (file, typ)
BinFile             file;
BasicType         *typ;

#endif /* __STDC__ */

{
   char                c;

   BIOreadByte (file, &c);
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
	       *typ = Picture;
	       break;
	    case C_REFER:
	       *typ = Refer;
	       break;
	    case C_PAGE_BREAK:
	       *typ = PageBreak;
	       break;
	    case C_UNUSED:
	       *typ = UnusedBasicType;
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

#ifdef __STDC__
static boolean      ReadAttribute (BinFile file, TtAttribute * pAttr)

#else  /* __STDC__ */
static boolean      ReadAttribute (file, pAttr)
BinFile             file;
TtAttribute         *pAttr;

#endif /* __STDC__ */

{
   AttribType        attrType;
   int               j;

   BIOreadName (file, pAttr->AttrName);
   strcpy (pAttr->AttrOrigName, pAttr->AttrName);
   BIOreadBool (file, &pAttr->AttrGlobal);
   BIOreadShort (file, &pAttr->AttrFirstExcept);
   BIOreadShort (file, &pAttr->AttrLastExcept);
   if (ReadAttribType (file, &attrType))
     {
	pAttr->AttrType = attrType;
	switch (pAttr->AttrType)
	      {
		 case AtNumAttr:
		 case AtTextAttr:
		    break;
		 case AtReferenceAttr:
		    BIOreadShort (file, &pAttr->AttrTypeRef);
		    BIOreadName (file, pAttr->AttrTypeRefNature);
		    break;
		 case AtEnumAttr:
		    BIOreadShort (file, &pAttr->AttrNEnumValues);
		    for (j = 0; j < pAttr->AttrNEnumValues; j++)
		       BIOreadName (file, pAttr->AttrEnumValue[j]);
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

#ifdef __STDC__
static boolean      ReadSRule (BinFile file, SRule * pSRule)

#else  /* __STDC__ */
static boolean      ReadSRule (file, pSRule)
BinFile             file;
SRule              *pSRule;

#endif /* __STDC__ */

{
   RConstruct        constr;
   int               j;

   BIOreadName (file, pSRule->SrName);
   BIOreadShort (file, &pSRule->SrNDefAttrs);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      BIOreadShort (file, &pSRule->SrDefAttr[j]);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      BIOreadSignedShort (file, &pSRule->SrDefAttrValue[j]);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      BIOreadBool (file, &pSRule->SrDefAttrModif[j]);

   BIOreadShort (file, &pSRule->SrNLocalAttrs);
   for (j = 0; j < pSRule->SrNLocalAttrs; j++)
      BIOreadShort (file, &pSRule->SrLocalAttr[j]);
   for (j = 0; j < pSRule->SrNLocalAttrs; j++)
      BIOreadBool (file, &pSRule->SrRequiredAttr[j]);

   BIOreadBool (file, &pSRule->SrAssocElem);
   BIOreadBool (file, &pSRule->SrParamElem);
   BIOreadBool (file, &pSRule->SrUnitElem);
   BIOreadBool (file, &pSRule->SrRecursive);
   pSRule->SrRecursDone = FALSE;

   BIOreadBool (file, &pSRule->SrExportedElem);
   if (pSRule->SrExportedElem)
     {
	BIOreadShort (file, &pSRule->SrExportContent);
	BIOreadName (file, pSRule->SrNatExpContent);
     }

   BIOreadShort (file, &pSRule->SrFirstExcept);
   BIOreadShort (file, &pSRule->SrLastExcept);

   BIOreadShort (file, &pSRule->SrNInclusions);
   for (j = 0; j < pSRule->SrNInclusions; j++)
      BIOreadShort (file, &pSRule->SrInclusion[j]);

   BIOreadShort (file, &pSRule->SrNExclusions);
   for (j = 0; j < pSRule->SrNExclusions; j++)
      BIOreadShort (file, &pSRule->SrExclusion[j]);
   BIOreadBool (file, &pSRule->SrRefImportedDoc);
   if (!ReadRConstruct (file, &constr))
      return FALSE;

   pSRule->SrConstruct = constr;
   switch (pSRule->SrConstruct)
	 {
	    case CsNatureSchema:
	       strcpy (pSRule->SrOrigNat, pSRule->SrName);
	       pSRule->SrSSchemaNat = NULL;
	       break;
	    case CsBasicElement:
	       if (!ReadBasicType (file, &pSRule->SrBasicType))
		  return FALSE;
	       break;
	    case CsReference:
	       BIOreadShort (file, &pSRule->SrReferredType);
	       BIOreadName (file, pSRule->SrRefTypeNat);

	       break;
	    case CsIdentity:
	       BIOreadShort (file, &pSRule->SrIdentRule);
	       break;
	    case CsList:
	       BIOreadShort (file, &pSRule->SrListItem);
	       BIOreadShort (file, &pSRule->SrMinItems);
	       BIOreadShort (file, &pSRule->SrMaxItems);
	       break;
	    case CsChoice:
	       BIOreadSignedShort (file, &pSRule->SrNChoices);
	       for (j = 0; j < pSRule->SrNChoices; j++)
		  BIOreadShort (file, &pSRule->SrChoice[j]);
	       break;
	    case CsAggregate:
	    case CsUnorderedAggregate:
	       BIOreadShort (file, &pSRule->SrNComponents);
	       for (j = 0; j < pSRule->SrNComponents; j++)
		  BIOreadShort (file, &pSRule->SrComponent[j]);
	       for (j = 0; j < pSRule->SrNComponents; j++)
		  BIOreadBool (file, &pSRule->SrOptComponent[j]);
	       break;
	    case CsConstant:
	       BIOreadShort (file, &pSRule->SrIndexConst);
	       break;
	    case CsPairedElement:
	       BIOreadBool (file, &pSRule->SrFirstOfPair);
	       break;
	    case CsExtensionRule:
	       break;
	 }
   return TRUE;
}

/*----------------------------------------------------------------------
   ReadConstants                                          		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      ReadConstants (BinFile file, PtrSSchema pSS)

#else  /* __STDC__ */
static boolean      ReadConstants (file, pSS)
BinFile             file;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   char                c;
   int                 i;

   i = 0;
   do
     {
	do
	  {
	     BIOreadByte (file, &c);
	     pSS->SsConstBuffer[i++] = c;
	  }
	while (c != '\0' && i < MAX_LEN_ALL_CONST);
	BIOreadByte (file, &c);
	pSS->SsConstBuffer[i++] = c;
     }
   while (c != '\0' && i < MAX_LEN_ALL_CONST);
   if (i >= MAX_LEN_ALL_CONST)
      return FALSE;
   else
      return TRUE;
}


/*----------------------------------------------------------------------
   ReadStructureSchema                                             
   lit un fichier contenant un schema de structure et charge       
   ce schema en memoire.                                           
   fileName: nom du fichier a lire, sans le suffixe .STR       	
   pSS: schema de structure en memoire a remplir.               	
   Retourne VRAI si chargement reussi, FAUX si echec.              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
boolean             ReadStructureSchema (Name fileName, PtrSSchema pSS)

#else  /* __STDC__ */
boolean             ReadStructureSchema (fileName, pSS)
Name                fileName;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   BinFile             file;
   PathBuffer          buf;
   PathBuffer          dirBuffer;
   int                 i;

   /* compose le nom du fichier a ouvrir */
   strncpy (dirBuffer, SchemaPath, MAX_PATH);
   MakeCompleteName (fileName, "STR", dirBuffer, buf, &i);

   /* ouvre le fichier */
   file = BIOreadOpen (buf);
   if (file == 0)
      /* echec */
     {
	strncpy (buf, fileName, MAX_PATH);
	strcat (buf, ".STR");
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_LIB_MISSING_FILE), buf);
	return FALSE;
     }
   else
     {
	pSS->SsActionList = NULL;
	/* lit la partie fixe du schema de structure */
	BIOreadName (file, pSS->SsName);
	BIOreadShort (file, &pSS->SsCode);
	BIOreadName (file, pSS->SsDefaultPSchema);
	pSS->SsPSchema = NULL;
	BIOreadBool (file, &pSS->SsExtension);
	pSS->SsNExtensRules = 0;
	pSS->SsExtensBlock = NULL;
	BIOreadShort (file, &pSS->SsRootElem);
	pSS->SsNObjects = 0;
	BIOreadShort (file, &pSS->SsNAttributes);
	BIOreadShort (file, &pSS->SsNRules);
	BIOreadBool (file, &pSS->SsExport);
	BIOreadShort (file, &pSS->SsNExceptions);
	for (i = 0; i < pSS->SsNExceptions; i++)
	   BIOreadShort (file, &pSS->SsException[i]);

	/* lit le texte des constantes */
	if (!ReadConstants (file, pSS))
	  {
	     /* message 'Fichier .STR incorrect ' */
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_INCORRECT_STR_FILE),
				fileName);
	     return FALSE;
	  }

	/* il n'y a pas encore de nature chargee dynamiquement */
	pSS->SsFirstDynNature = 0;

	/* lit les attributs */
	for (i = 0; i < pSS->SsNAttributes; i++)
	   if (!ReadAttribute (file, &pSS->SsAttribute[i]))
	     {
		/* message 'Fichier .STR incorrect ' */
		TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_INCORRECT_STR_FILE), fileName);
		return FALSE;
	     }

	/* lit les elements */
	for (i = 0; i < pSS->SsNRules; i++)
	   if (!ReadSRule (file, &pSS->SsRule[i]))
	     {
		/* message 'Fichier .STR incorrect ' */
		TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_INCORRECT_STR_FILE), fileName);
		return FALSE;
	     }
	if (pSS->SsExtension)
	  {
	     if (BIOreadShort (file, &pSS->SsNExtensRules))
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
				TtaDisplayMessage (INFO, TtaGetMessage(LIB, TMSG_INCORRECT_STR_FILE), fileName);
				return FALSE;
			     }
		  }
	  }

	BIOreadClose (file);
	return TRUE;
     }
}
