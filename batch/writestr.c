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
 * Ce module sauve dans un fichier un schema de structure qui a ete compile'
 * en memoire.
 *
 * Author: V. Quint (INRIA)
 *         R. Guetari (W3C/INRIA): Unciode.
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"

#define THOT_EXPORT extern
static BinFile      outfile;

#include "fileaccess_f.h"
#include "writestr_f.h"

/*----------------------------------------------------------------------
   UniqueIdent	retourne un nombre entier qui sera utilise	
   comme un identificateur unique du schema compile'.		
  ----------------------------------------------------------------------*/

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


/*----------------------------------------------------------------------
   WriteShort      ecrit l'entier n				
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
   WriteSignedShort  ecrit l'entier signe' n			
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
   WriteName       ecrit le nom name				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteName (Name name)

#else  /* __STDC__ */
void                WriteName (name)
Name                name;

#endif /* __STDC__ */

{
   int                 i;

   i = 0;
   do
      TtaWriteWideChar (outfile, name[i++], ISOLatin1 /* UTF8 */);
   while (name[i - 1] != TEXT('\0'));
}


/*----------------------------------------------------------------------
   WriteBoolean       ecrit le booleen b dans le fichier		
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
   WriteAttributeType   ecrit un type d'attribut			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteAttributeType (AttribType typ)

#else  /* __STDC__ */
void                WriteAttributeType (typ)
AttribType          typ;

#endif /* __STDC__ */

{

   switch (typ)
	 {
	    case AtNumAttr:
	       TtaWriteByte (outfile, C_INT_ATTR);
	       break;
	    case AtTextAttr:
	       TtaWriteByte (outfile, C_TEXT_ATTR);
	       break;
	    case AtReferenceAttr:
	       TtaWriteByte (outfile, C_REF_ATTR);
	       break;
	    case AtEnumAttr:
	       TtaWriteByte (outfile, C_ENUM_ATTR);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteConstructor       ecrit un constructeur			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteConstructor (RConstruct constr)

#else  /* __STDC__ */
void                WriteConstructor (constr)
RConstruct          constr;

#endif /* __STDC__ */

{
   switch (constr)
	 {
	    case CsIdentity:
	       TtaWriteByte (outfile, C_IDENTITY_CONSTR);
	       break;
	    case CsList:
	       TtaWriteByte (outfile, C_LIST_CONSTR);
	       break;
	    case CsChoice:
	       TtaWriteByte (outfile, C_CHOICE_CONSTR);
	       break;
	    case CsAggregate:
	       TtaWriteByte (outfile, C_AGG_CONSTR);
	       break;
	    case CsUnorderedAggregate:
	       TtaWriteByte (outfile, C_UNORD_AGG_CONSTR);
	       break;
	    case CsConstant:
	       TtaWriteByte (outfile, C_CONST_CONSTR);
	       break;
	    case CsReference:
	       TtaWriteByte (outfile, C_REF_CONSTR);
	       break;
	    case CsBasicElement:
	       TtaWriteByte (outfile, C_BASIC_TYPE_CONSTR);
	       break;
	    case CsNatureSchema:
	       TtaWriteByte (outfile, C_NATURE_CONSTR);
	       break;
	    case CsPairedElement:
	       TtaWriteByte (outfile, C_PAIR_CONSTR);
	       break;
	    case CsExtensionRule:
	       TtaWriteByte (outfile, C_EXTENS_CONSTR);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteBasicType   ecrit un type de base				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                WriteBasicType (BasicType typ)

#else  /* __STDC__ */
void                WriteBasicType (typ)
BasicType           typ;

#endif /* __STDC__ */

{
   switch (typ)
	 {
	    case CharString:
	       TtaWriteByte (outfile, C_CHAR_STRING);
	       break;
	    case GraphicElem:
	       TtaWriteByte (outfile, C_GRAPHICS);
	       break;
	    case Symbol:
	       TtaWriteByte (outfile, C_SYMBOL);
	       break;
	    case Picture:
	       TtaWriteByte (outfile, C_PICTURE);
	       break;
	    case Refer:
	       TtaWriteByte (outfile, C_REFER);
	       break;
	    case PageBreak:
	       TtaWriteByte (outfile, C_PAGE_BREAK);
	       break;
	    case UnusedBasicType:
	       TtaWriteByte (outfile, C_UNUSED);
	       break;
	 }
}


/*----------------------------------------------------------------------
   WriteRule     ecrit une regle de structure			
  ----------------------------------------------------------------------*/
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


/*----------------------------------------------------------------------
   WriteStructureSchema						
   ecrit le schema de structure pointe' par pSS dans le fichier	
   de nom fileName.						
   Si code est nul, ecrit le schema avec un nouveau code		
   d'identification, sinon avec code comme code d'identification.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             WriteStructureSchema (Name fileName, PtrSSchema pSS,
					  int code)

#else  /* __STDC__ */
ThotBool             WriteStructureSchema (fileName, pSS, code)
Name                fileName;
PtrSSchema          pSS;
int                 code;

#endif /* __STDC__ */

{
   TtAttribute        *pAttr;
   int                 i, j;

   /* ouvre le fichier */
   outfile = TtaWriteOpen (fileName);
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
      TtaWriteWideChar (outfile, pSS->SsConstBuffer[i++], ISOLatin1 /* UTF8 */);
   while (pSS->SsConstBuffer[i - 1] != '\0' || pSS->SsConstBuffer[i] != '\0');

   /* SsFirstDynNature */
   TtaWriteByte (outfile, '\0');

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
   TtaWriteClose (outfile);
   return True;
}
