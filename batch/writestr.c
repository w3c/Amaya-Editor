/*
   Ce module sauve dans un fichier un schema de structure qui a ete compile'
   en memoire.
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "typetime.h"
#include "storage.h"

#define EXPORT extern
static BinFile      outfile;

#include "fileaccess_f.h"
#include "writestr_f.h"

/* ---------------------------------------------------------------------- */
/* |	UniqueIdent	retourne un nombre entier qui sera utilise	| */
/* |	comme un identificateur unique du schema compile'.		| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 UniqueIdent ()

#else  /* __STDC__ */
int                 UniqueIdent ()
#endif				/* __STDC__ */

{
   int                 r;

   /* l'heure en secondes est considere'e comme un nombre aleatoire */
   r = time (NULL);
   return (r % 65536);
}


/* ---------------------------------------------------------------------- */
/* |	WriteShort      ecrit l'entier n				| */
/* ---------------------------------------------------------------------- */
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


/* ---------------------------------------------------------------------- */
/* |	WriteSignedShort  ecrit l'entier signe' n			| */
/* ---------------------------------------------------------------------- */
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


/* ---------------------------------------------------------------------- */
/* |	WriteName       ecrit le nom name				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WriteName (Name name)

#else  /* __STDC__ */
void                WriteName (name)
Name                 name;

#endif /* __STDC__ */

{
   int                 i;

   i = 0;
   do
	BIOwriteByte (outfile, name[i++]);
   while (name[i - 1] != '\0');
}


/* ---------------------------------------------------------------------- */
/* |	WriteBoolean       ecrit le booleen b dans le fichier		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WriteBoolean (boolean b)

#else  /* __STDC__ */
void                WriteBoolean (b)
boolean             b;

#endif /* __STDC__ */

{
   if (b)
      BIOwriteByte (outfile, '\1');
   else
      BIOwriteByte (outfile, '\0');
}


/* ---------------------------------------------------------------------- */
/* |	WriteAttributeType   ecrit un type d'attribut			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WriteAttributeType (AttribType typ)

#else  /* __STDC__ */
void                WriteAttributeType (typ)
AttribType        typ;

#endif /* __STDC__ */

{

   switch (typ)
	 {
	    case AtNumAttr:
	       BIOwriteByte (outfile, C_INT_ATTR);
	       break;
	    case AtTextAttr:
	       BIOwriteByte (outfile, C_TEXT_ATTR);
	       break;
	    case AtReferenceAttr:
	       BIOwriteByte (outfile, C_REF_ATTR);
	       break;
	    case AtEnumAttr:
	       BIOwriteByte (outfile, C_ENUM_ATTR);
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |	WriteConstructor       ecrit un constructeur			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WriteConstructor (RConstruct constr)

#else  /* __STDC__ */
void                WriteConstructor (constr)
RConstruct        constr;

#endif /* __STDC__ */

{
   switch (constr)
	 {
	    case CsIdentity:
	       BIOwriteByte (outfile, C_IDENTITY_CONSTR);
	       break;
	    case CsList:
	       BIOwriteByte (outfile, C_LIST_CONSTR);
	       break;
	    case CsChoice:
	       BIOwriteByte (outfile, C_CHOICE_CONSTR);
	       break;
	    case CsAggregate:
	       BIOwriteByte (outfile, C_AGG_CONSTR);
	       break;
	    case CsUnorderedAggregate:
	       BIOwriteByte (outfile, C_UNORD_AGG_CONSTR);
	       break;
	    case CsConstant:
	       BIOwriteByte (outfile, C_CONST_CONSTR);
	       break;
	    case CsReference:
	       BIOwriteByte (outfile, C_REF_CONSTR);
	       break;
	    case CsBasicElement:
	       BIOwriteByte (outfile, C_BASIC_TYPE_CONSTR);
	       break;
	    case CsNatureSchema:
	       BIOwriteByte (outfile, C_NATURE_CONSTR);
	       break;
	    case CsPairedElement:
	       BIOwriteByte (outfile, C_PAIR_CONSTR);
	       break;
	    case CsExtensionRule:
	       BIOwriteByte (outfile, C_EXTENS_CONSTR);
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |	WriteBasicType   ecrit un type de base				| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WriteBasicType (BasicType typ)

#else  /* __STDC__ */
void                WriteBasicType (typ)
BasicType          typ;

#endif /* __STDC__ */

{
   switch (typ)
	 {
	    case CharString:
	       BIOwriteByte (outfile, C_CHAR_STRING);
	       break;
	    case GraphicElem:
	       BIOwriteByte (outfile, C_GRAPHICS);
	       break;
	    case Symbol:
	       BIOwriteByte (outfile, C_SYMBOL);
	       break;
	    case Picture:
	       BIOwriteByte (outfile, C_PICTURE);
	       break;
	    case Refer:
	       BIOwriteByte (outfile, C_REFER);
	       break;
	    case PageBreak:
	       BIOwriteByte (outfile, C_PAGE_BREAK);
	       break;
	    case UnusedBasicType:
	       BIOwriteByte (outfile, C_UNUSED);
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |	WriteRule     ecrit une regle de structure			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         WriteRule (SRule * pSRule)

#else  /* __STDC__ */
static void         WriteRule (pSRule)
SRule              *pSRule;

#endif /* __STDC__ */

{
   int                 j;

   WriteName (pSRule->SrName);
   WriteShort (pSRule->SrNDefAttrs);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      WriteShort (pSRule->SrDefAttr[j]);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      WriteSignedShort (pSRule->SrDefAttrValue[j]);
   for (j = 0; j < pSRule->SrNDefAttrs; j++)
      WriteBoolean (pSRule->SrDefAttrModif[j]);
   WriteShort (pSRule->SrNLocalAttrs);
   for (j = 0; j < pSRule->SrNLocalAttrs; j++)
      WriteShort (pSRule->SrLocalAttr[j]);
   for (j = 0; j < pSRule->SrNLocalAttrs; j++)
      WriteBoolean (pSRule->SrRequiredAttr[j]);
   WriteBoolean (pSRule->SrAssocElem);
   WriteBoolean (pSRule->SrParamElem);
   WriteBoolean (pSRule->SrUnitElem);
   WriteBoolean (pSRule->SrRecursive);
   WriteBoolean (pSRule->SrExportedElem);
   if (pSRule->SrExportedElem)
     {
	WriteShort (pSRule->SrExportContent);
	WriteName (pSRule->SrNatExpContent);
     }
   WriteShort (pSRule->SrFirstExcept);
   WriteShort (pSRule->SrLastExcept);
   WriteShort (pSRule->SrNInclusions);
   for (j = 0; j < pSRule->SrNInclusions; j++)
      WriteShort (pSRule->SrInclusion[j]);
   WriteShort (pSRule->SrNExclusions);
   for (j = 0; j < pSRule->SrNExclusions; j++)
      WriteShort (pSRule->SrExclusion[j]);
   WriteBoolean (pSRule->SrRefImportedDoc);
   WriteConstructor (pSRule->SrConstruct);
   switch (pSRule->SrConstruct)
	 {
	    case CsNatureSchema:
	       /* don't write anything */
	       break;
	    case CsBasicElement:
	       WriteBasicType (pSRule->SrBasicType);
	       break;
	    case CsReference:
	       WriteShort (pSRule->SrReferredType);
	       WriteName (pSRule->SrRefTypeNat);
	       break;
	    case CsIdentity:
	       WriteShort (pSRule->SrIdentRule);
	       break;
	    case CsList:
	       WriteShort (pSRule->SrListItem);
	       WriteShort (pSRule->SrMinItems);
	       WriteShort (pSRule->SrMaxItems);
	       break;
	    case CsChoice:
	       WriteSignedShort (pSRule->SrNChoices);
	       if (pSRule->SrNChoices > 0)
		  for (j = 0; j < pSRule->SrNChoices; j++)
		     WriteShort (pSRule->SrChoice[j]);
	       break;
	    case CsAggregate:
	    case CsUnorderedAggregate:
	       WriteShort (pSRule->SrNComponents);
	       for (j = 0; j < pSRule->SrNComponents; j++)
		  WriteShort (pSRule->SrComponent[j]);
	       for (j = 0; j < pSRule->SrNComponents; j++)
		  WriteBoolean (pSRule->SrOptComponent[j]);
	       break;
	    case CsConstant:
	       WriteShort (pSRule->SrIndexConst);
	       break;
	    case CsPairedElement:
	       WriteBoolean (pSRule->SrFirstOfPair);
	       break;
	    case CsExtensionRule:
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |	WriteStructureSchema						| */
/* |	ecrit le schema de structure pointe' par pSS dans le fichier	| */
/* |	de nom fileName.						| */
/* |	Si code est nul, ecrit le schema avec un nouveau code		| */
/* |	d'identification, sinon avec code comme code d'identification.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             WriteStructureSchema (Name fileName, PtrSSchema pSS,
					  int code)

#else  /* __STDC__ */
boolean             WriteStructureSchema (fileName, pSS, code)
Name                fileName;
PtrSSchema          pSS;
int                 code;

#endif /* __STDC__ */

{
   TtAttribute      *pAttr;
   int              i, j;

   /* ouvre le fichier */
   outfile = BIOwriteOpen (fileName);
   if (outfile == 0)
      return False;

   /* ecrit la partie fixe du schema de structure */
   WriteName (pSS->SsName);
   if (code == 0)
      /* alloue un nouveau code d'identification au schema compile' */
      WriteShort (UniqueIdent ());
   else
      /* le schema compile' a le code d'identification code */
      WriteShort (code);
   WriteName (pSS->SsDefaultPSchema);
   WriteBoolean (pSS->SsExtension);
   WriteShort (pSS->SsRootElem);
   WriteShort (pSS->SsNAttributes);
   WriteShort (pSS->SsNRules);
   WriteBoolean (pSS->SsExport);

   WriteShort (pSS->SsNExceptions);
   for (i = 0; i < pSS->SsNExceptions; i++)
      WriteShort (pSS->SsException[i]);

   /* ecrit le texte des constantes */
   i = 0;
   do
	BIOwriteByte (outfile, pSS->SsConstBuffer[i++]);
   while (pSS->SsConstBuffer[i - 1] != '\0' || pSS->SsConstBuffer[i] != '\0');

   /* SsFirstDynNature */
   BIOwriteByte (outfile, '\0');

   /* ecrit les attributs */
   for (i = 0; i < pSS->SsNAttributes; i++)
     {
	pAttr = &pSS->SsAttribute[i];
	WriteName (pAttr->AttrName);
	WriteBoolean (pAttr->AttrGlobal);
	WriteShort (pAttr->AttrFirstExcept);
	WriteShort (pAttr->AttrLastExcept);
	WriteAttributeType (pAttr->AttrType);
	switch (pAttr->AttrType)
	      {
		 case AtNumAttr:
		 case AtTextAttr:
		    break;
		 case AtReferenceAttr:
		    WriteShort (pAttr->AttrTypeRef);
		    WriteName (pAttr->AttrTypeRefNature);
		    break;
		 case AtEnumAttr:
		    WriteShort (pAttr->AttrNEnumValues);
		    for (j = 0; j < pAttr->AttrNEnumValues; j++)
		       WriteName (pAttr->AttrEnumValue[j]);
		    break;
	      }
     }
   /* ecrit les regles de structure */
   for (i = 0; i < pSS->SsNRules; i++)
      WriteRule (&pSS->SsRule[i]);

   /* ecrit les regles d'extension */
   if (pSS->SsExtension)
     {
	WriteShort (pSS->SsNExtensRules);
	for (i = 0; i < pSS->SsNExtensRules; i++)
	   WriteRule (&pSS->SsExtensBlock->EbExtensRule[i]);
     }
   BIOwriteClose (outfile);
   return True;
}
