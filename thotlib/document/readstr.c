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
   Ce module charge un schema de structure depuis un fichier .STR
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"

/* variables importees */
#define THOT_EXPORT extern
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
AttribType         *attrType;

#endif /* __STDC__ */

{
   char                c;

   TtaReadByte (file, &c);
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
RConstruct         *constr;

#endif /* __STDC__ */

{
   char                c;

   TtaReadByte (file, &c);
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
BasicType          *typ;

#endif /* __STDC__ */

{
   char                c;

   TtaReadByte (file, &c);
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
TtAttribute        *pAttr;

#endif /* __STDC__ */

{
   AttribType          attrType;
   int                 j;

   TtaReadName (file, pAttr->AttrName);
   strcpy (pAttr->AttrOrigName, pAttr->AttrName);
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
		    TtaReadName (file, pAttr->AttrTypeRefNature);
		    break;
		 case AtEnumAttr:
		    TtaReadShort (file, &pAttr->AttrNEnumValues);
		    for (j = 0; j < pAttr->AttrNEnumValues; j++)
		       TtaReadName (file, pAttr->AttrEnumValue[j]);
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
   RConstruct          constr;
   int                 j;

   TtaReadName (file, pSRule->SrName);
   strcpy (pSRule->SrOrigName, pSRule->SrName);
   TtaReadShort (file, &pSRule->SrNDefAttrs);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      TtaReadShort (file, &pSRule->SrDefAttr[j]);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      TtaReadSignedShort (file, &pSRule->SrDefAttrValue[j]);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      TtaReadBool (file, &pSRule->SrDefAttrModif[j]);

   TtaReadShort (file, &pSRule->SrNLocalAttrs);
   for (j = 0; j < pSRule->SrNLocalAttrs; j++)
      TtaReadShort (file, &pSRule->SrLocalAttr[j]);
   for (j = 0; j < pSRule->SrNLocalAttrs; j++)
      TtaReadBool (file, &pSRule->SrRequiredAttr[j]);

   TtaReadBool (file, &pSRule->SrAssocElem);
   TtaReadBool (file, &pSRule->SrParamElem);
   TtaReadBool (file, &pSRule->SrUnitElem);
   TtaReadBool (file, &pSRule->SrRecursive);
   pSRule->SrRecursDone = FALSE;

   TtaReadBool (file, &pSRule->SrExportedElem);
   if (pSRule->SrExportedElem)
     {
	TtaReadShort (file, &pSRule->SrExportContent);
	TtaReadName (file, pSRule->SrNatExpContent);
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
	       strcpy (pSRule->SrOrigNat, pSRule->SrName);
	       pSRule->SrSSchemaNat = NULL;
	       break;
	    case CsBasicElement:
	       if (!ReadBasicType (file, &pSRule->SrBasicType))
		  return FALSE;
	       break;
	    case CsReference:
	       TtaReadShort (file, &pSRule->SrReferredType);
	       TtaReadName (file, pSRule->SrRefTypeNat);

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
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   char                c;
   int                 i;

   i = 0;
   do
     {
	do
	  {
	     TtaReadByte (file, &c);
	     pSS->SsConstBuffer[i++] = c;
	  }
	while (c != EOS && i < MAX_LEN_ALL_CONST);
	TtaReadByte (file, &c);
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
   file = TtaReadOpen (buf);
   if (file == 0)
      /* echec */
     {
	strncpy (buf, fileName, MAX_PATH);
	strcat (buf, ".STR");
	TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_MISSING_FILE), buf);
	return FALSE;
     }
   else
     {
	pSS->SsActionList = NULL;
	/* lit la partie fixe du schema de structure */
	TtaReadName (file, pSS->SsName);
	TtaReadShort (file, &pSS->SsCode);
	TtaReadName (file, pSS->SsDefaultPSchema);
	pSS->SsPSchema = NULL;
	TtaReadBool (file, &pSS->SsExtension);
	pSS->SsNExtensRules = 0;
	pSS->SsExtensBlock = NULL;
	TtaReadShort (file, &pSS->SsRootElem);
	pSS->SsNObjects = 0;
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
	     TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_STR_FILE),
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
		TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_STR_FILE), fileName);
		return FALSE;
	     }

	/* lit les elements */
	for (i = 0; i < pSS->SsNRules; i++)
	   if (!ReadSRule (file, &pSS->SsRule[i]))
	     {
		/* message 'Fichier .STR incorrect ' */
		TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCORRECT_STR_FILE), fileName);
		return FALSE;
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
