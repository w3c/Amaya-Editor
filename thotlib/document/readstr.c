
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
 * Chargement des schemas de structure.
 *
 * Global Functions:
 * RdSchStruct
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"

/* variables importees */
#define EXPORT extern
#include "environ.var"

#include "dofile.f"
#include "storage.f"
#include "rdschstr.f"
#include "memory.f"


/* ---------------------------------------------------------------------- */
/* |    rdTypeAttr                                                      | */
/* |    lit un type d'attribut dans le fichier et retourne sa valeur.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      rdTypeAttr (BinFile file, AttribType * value)

#else  /* __STDC__ */
static boolean      rdTypeAttr (file, value)
BinFile             file;
AttribType       *value;

#endif /* __STDC__ */

{
   char                c;

   BIOreadByte (file, &c);
   switch (c)
	 {
	    case C_INT_ATTR:
	       *value = AtNumAttr;
	       break;
	    case C_TEXT_ATTR:
	       *value = AtTextAttr;
	       break;
	    case C_REF_ATTR:
	       *value = AtReferenceAttr;
	       break;
	    case C_ENUM_ATTR:
	       *value = AtEnumAttr;
	       break;
	    default:
	       *value = AtEnumAttr;
	       return False;
	       break;
	 }
   return True;
}


/* ---------------------------------------------------------------------- */
/* |    rdConstructeur  lit un constructeur dans le fichier             | */
/* |    et retourne sa valeur.                                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      rdConstructeur (BinFile file, RConstruct * value)

#else  /* __STDC__ */
static boolean      rdConstructeur (file, value)
BinFile             file;
RConstruct       *value;

#endif /* __STDC__ */

{
   char                c;

   BIOreadByte (file, &c);
   switch (c)
	 {
	    case C_IDENTITY_CONSTR:
	       *value = CsIdentity;
	       break;
	    case C_LIST_CONSTR:
	       *value = CsList;
	       break;
	    case C_CHOICE_CONSTR:
	       *value = CsChoice;
	       break;
	    case C_AGG_CONSTR:
	       *value = CsAggregate;
	       break;
	    case C_UNORD_AGG_CONSTR:
	       *value = CsUnorderedAggregate;
	       break;
	    case C_CONST_CONSTR:
	       *value = CsConstant;
	       break;
	    case C_REF_CONSTR:
	       *value = CsReference;
	       break;
	    case C_BASIC_TYPE_CONSTR:
	       *value = CsBasicElement;
	       break;
	    case C_NATURE_CONSTR:
	       *value = CsNatureSchema;
	       break;
	    case C_PAIR_CONSTR:
	       *value = CsPairedElement;
	       break;
	    case C_EXTENS_CONSTR:
	       *value = CsExtensionRule;
	       break;
	    default:
	       *value = CsIdentity;
	       return False;
	 }
   return True;
}


/* ---------------------------------------------------------------------- */
/* |    rdTypeBase                                                      | */
/* |    lit un type de base dans le fichier et retourne sa valeur.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      rdTypeBase (BinFile file, BasicType * value)

#else  /* __STDC__ */
static boolean      rdTypeBase (file, value)
BinFile             file;
BasicType         *value;

#endif /* __STDC__ */

{
   char                c;

   BIOreadByte (file, &c);
   switch (c)
	 {
	    case C_CHAR_STRING:
	       *value = CharString;
	       break;
	    case C_GRAPHICS:
	       *value = GraphicElem;
	       break;
	    case C_SYMBOL:
	       *value = Symbol;
	       break;
	    case C_PICTURE:
	       *value = Picture;
	       break;
	    case C_REFER:
	       *value = Refer;
	       break;
	    case C_PAGE_BREAK:
	       *value = PageBreak;
	       break;
	    case C_UNUSED:
	       *value = UnusedBasicType;
	       break;
	    default:
	       *value = CharString;
	       return False;
	 }
   return True;
}


/* ---------------------------------------------------------------------- */
/* |    read description of an attribute                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      rdAttributeDef (BinFile file, TtAttribute * pAttr)

#else  /* __STDC__ */
static boolean      rdAttributeDef (file, pAttr)
BinFile             file;
TtAttribute           *pAttr;

#endif /* __STDC__ */

{
   AttribType        type;
   int                 j;

   BIOreadName (file, pAttr->AttrName);
   strcpy (pAttr->AttrOrigName, pAttr->AttrName);
   BIOreadBool (file, &pAttr->AttrGlobal);
   BIOreadShort (file, &pAttr->AttrFirstExcept);
   BIOreadShort (file, &pAttr->AttrLastExcept);
   if (rdTypeAttr (file, &type))
     {
	pAttr->AttrType = type;
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
      return False;

   return True;
}


/* ---------------------------------------------------------------------- */
/* |    read description of an element                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      rdElementDef (BinFile file, SRule * pRule)

#else  /* __STDC__ */
static boolean      rdElementDef (file, pRule)
BinFile             file;
SRule              *pRule;

#endif /* __STDC__ */

{
   RConstruct        constr;
   int                 j;

   BIOreadName (file, pRule->SrName);
   BIOreadShort (file, &pRule->SrNDefAttrs);
   for (j = 0; j < pRule->SrNDefAttrs; j++)
      BIOreadShort (file, &pRule->SrDefAttr[j]);
   for (j = 0; j < pRule->SrNDefAttrs; j++)
      BIOreadSignedShort (file, &pRule->SrDefAttrValue[j]);
   for (j = 0; j < pRule->SrNDefAttrs; j++)
      BIOreadBool (file, &pRule->SrDefAttrModif[j]);

   BIOreadShort (file, &pRule->SrNLocalAttrs);
   for (j = 0; j < pRule->SrNLocalAttrs; j++)
      BIOreadShort (file, &pRule->SrLocalAttr[j]);
   for (j = 0; j < pRule->SrNLocalAttrs; j++)
      BIOreadBool (file, &pRule->SrRequiredAttr[j]);

   BIOreadBool (file, &pRule->SrAssocElem);
   BIOreadBool (file, &pRule->SrParamElem);
   BIOreadBool (file, &pRule->SrUnitElem);
   BIOreadBool (file, &pRule->SrRecursive);
   pRule->SrRecursDone = False;

   BIOreadBool (file, &pRule->SrExportedElem);
   if (pRule->SrExportedElem)
     {
	BIOreadShort (file, &pRule->SrExportContent);
	BIOreadName (file, pRule->SrNatExpContent);
     }

   BIOreadShort (file, &pRule->SrFirstExcept);
   BIOreadShort (file, &pRule->SrLastExcept);

   BIOreadShort (file, &pRule->SrNInclusions);
   for (j = 0; j < pRule->SrNInclusions; j++)
      BIOreadShort (file, &pRule->SrInclusion[j]);

   BIOreadShort (file, &pRule->SrNExclusions);
   for (j = 0; j < pRule->SrNExclusions; j++)
      BIOreadShort (file, &pRule->SrExclusion[j]);
   BIOreadBool (file, &pRule->SrRefImportedDoc);
   if (!rdConstructeur (file, &constr))
      return False;

   pRule->SrConstruct = constr;
   switch (pRule->SrConstruct)
	 {
	    case CsNatureSchema:
	       strcpy (pRule->SrOrigNat, pRule->SrName);
	       pRule->SrSSchemaNat = NULL;
	       break;
	    case CsBasicElement:
	       if (!rdTypeBase (file, &pRule->SrBasicType))
		  return False;
	       break;
	    case CsReference:
	       BIOreadShort (file, &pRule->SrReferredType);
	       BIOreadName (file, pRule->SrRefTypeNat);

	       break;
	    case CsIdentity:
	       BIOreadShort (file, &pRule->SrIdentRule);
	       break;
	    case CsList:
	       BIOreadShort (file, &pRule->SrListItem);
	       BIOreadShort (file, &pRule->SrMinItems);
	       BIOreadShort (file, &pRule->SrMaxItems);
	       break;
	    case CsChoice:
	       BIOreadSignedShort (file, &pRule->SrNChoices);
	       for (j = 1; j <= pRule->SrNChoices; j++)
		  BIOreadShort (file, &pRule->SrChoice[j - 1]);
	       break;
	    case CsAggregate:
	    case CsUnorderedAggregate:
	       BIOreadShort (file, &pRule->SrNComponents);
	       for (j = 1; j <= pRule->SrNComponents; j++)
		  BIOreadShort (file, &pRule->SrComponent[j - 1]);
	       for (j = 1; j <= pRule->SrNComponents; j++)
		  BIOreadBool (file, &pRule->SrOptComponent[j - 1]);
	       break;
	    case CsConstant:
	       BIOreadShort (file, &pRule->SrIndexConst);
	       break;
	    case CsPairedElement:
	       BIOreadBool (file, &pRule->SrFirstOfPair);
	       break;
	    case CsExtensionRule:
	       break;
	 }
   return True;
}

/* ---------------------------------------------------------------------- */
/* |    read Text of constants                                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      rdConstantText (BinFile file, PtrSSchema scheme)

#else  /* __STDC__ */
static boolean      rdConstantText (file, scheme)
BinFile             file;
PtrSSchema        scheme;

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
	     scheme->SsConstBuffer[i++] = c;
	  }
	while (c != '\0' && i < MAX_LEN_ALL_CONST);
	BIOreadByte (file, &c);
	scheme->SsConstBuffer[i++] = c;
     }
   while (c != '\0' && i < MAX_LEN_ALL_CONST);
   if (i >= MAX_LEN_ALL_CONST)
      return False;
   else
      return True;
}


/* ---------------------------------------------------------------------- */
/* |    RdSchStruct                                                     | */
/* |    lit un fichier contenant un schema de structure et charge       | */
/* |    ce schema en memoire.                                           | */
/* |    fname: nom du fichier a lire, sans le suffixe .STR              | */
/* |    scheme: schema de structure en memoire a remplir.               | */
/* |    Retourne VRAI si chargement reussi, FAUX si echec.              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             RdSchStruct (Name fname, PtrSSchema scheme)

#else  /* __STDC__ */
boolean             RdSchStruct (fname, scheme)
Name                 fname;
PtrSSchema        scheme;

#endif /* __STDC__ */

{
   int                 i;
   PathBuffer          filename;
   BinFile             strfile;
   PathBuffer          DirBuffer;

   /* compose le nom du fichier a ouvrir avec le nom du directory */
   /* des schemas... */
   strncpy (DirBuffer, DirectorySchemas, MAX_PATH);
   BuildFileName (fname, "STR", DirBuffer, filename, &i);

   /* ouvre le fichier */
   strfile = BIOreadOpen (filename);
   if (strfile == 0)
      /* echec */
     {
	strncpy (filename, fname, MAX_PATH);
	strcat (filename, ".STR");
	/* message 'Fichier inaccessible' */
	TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_MISSING_FILE), filename);
	return False;
     }
   else
     {
	scheme->SsActionList = NULL;
	/* lit la partie fixe du schema de structure */
	BIOreadName (strfile, scheme->SsName);
	BIOreadShort (strfile, &scheme->SsCode);
	BIOreadName (strfile, scheme->SsDefaultPSchema);
	scheme->SsPSchema = NULL;
	BIOreadBool (strfile, &scheme->SsExtension);
	scheme->SsNExtensRules = 0;
	scheme->SsExtensBlock = NULL;
	BIOreadShort (strfile, &scheme->SsRootElem);
	scheme->SsNObjects = 0;
	BIOreadShort (strfile, &scheme->SsNAttributes);
	BIOreadShort (strfile, &scheme->SsNRules);
	BIOreadBool (strfile, &scheme->SsExport);
	BIOreadShort (strfile, &scheme->SsNExceptions);
	for (i = 1; i <= scheme->SsNExceptions; i++)
	   BIOreadShort (strfile, &scheme->SsException[i - 1]);

	/* lit le texte des constantes */
	if (!rdConstantText (strfile, scheme))
	  {
	     /* message 'Fichier .STR incorrect ' */
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_INCOR_STR_FILE), fname);
	     return False;
	  }

	/* il n'y a pas encore de nature chargee dynamiquement */
	scheme->SsFirstDynNature = 0;

	/* lit les attributs */
	for (i = 0; i < scheme->SsNAttributes; i++)
	   if (!rdAttributeDef (strfile, &scheme->SsAttribute[i]))
	     {
		/* message 'Fichier .STR incorrect ' */
		TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_INCOR_STR_FILE), fname);
		return False;
	     }

	/* lit les elements */
	for (i = 0; i < scheme->SsNRules; i++)
	   if (!rdElementDef (strfile, &scheme->SsRule[i]))
	     {
		/* message 'Fichier .STR incorrect ' */
		TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_INCOR_STR_FILE), fname);
		return False;
	     }
	if (scheme->SsExtension)
	  {
	     if (BIOreadShort (strfile, &scheme->SsNExtensRules))
		if (scheme->SsNExtensRules > 0)
		  {
		     /* acquiert un bloc d'extension */
		     GetBlocExtens (&scheme->SsExtensBlock);
		     if (scheme->SsExtensBlock != NULL)
			/* lit les regles d'extension */
			for (i = 0; i < scheme->SsNExtensRules; i++)
			   if (!rdElementDef (strfile, &scheme->SsExtensBlock->EbExtensRule[i]))
			     {
				/* message 'Fichier .STR incorrect ' */
				TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_INCOR_STR_FILE), fname);
				return False;
			     }
		  }
	  }

	BIOreadClose (strfile);
	return True;
     }
}

/* End Of Module rdschstr */
