
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* ======================================================================= */
/* |                                                                    | */
/* |                           Projet THOT                              | */
/* |                                                                    | */
/* |    Ce module sauve un schema de structure compile'.                | */
/* |                                                                    | */
/* |                                                                    | */
/* |                    V. Quint        Juin 1984                       | */
/* |                                                                    | */
/* ======================================================================= */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "typetime.h"
#include "storage.h"

#define EXPORT extern
static BinFile      outfile;

#include "fileaccess_f.h"
#include "writestr_f.h"

/* retourne un nombre entier qui sera utilise comme un identificateur */
/* unique */
#ifdef __STDC__
int                 uniqueident ()

#else  /* __STDC__ */
int                 uniqueident ()
#endif				/* __STDC__ */

{
   int                 r;

   /* heure en secondes (nombre aleatoire) */
   r = time (NULL);
   return (r % 65536);
}


/* wrshort      ecrit l'entier n dans le fichier .STR */
#ifdef __STDC__
void                wrshort (int n)

#else  /* __STDC__ */
void                wrshort (n)
int                 n;

#endif /* __STDC__ */

{

   BIOwriteByte (outfile, (char) (n / 256));
   BIOwriteByte (outfile, (char) (n % 256));
}


/* wrsignshort  ecrit l'entier signe' n dans le fichier .STR */
#ifdef __STDC__
void                wrsignshort (int n)

#else  /* __STDC__ */
void                wrsignshort (n)
int                 n;

#endif /* __STDC__ */

{

   if (n >= 0)
      wrshort (n);
   else
      wrshort (n + 65536);
}


/* wrNom                ecrit le nom n dans le fichier .STR */
#ifdef __STDC__
void                wrNom (Name n)

#else  /* __STDC__ */
void                wrNom (n)
Name                 n;

#endif /* __STDC__ */

{
   int                 k;


   k = 0;
   do
	BIOwriteByte (outfile, n[k++]);
   while (n[k - 1] != '\0');
}


/* wrBool       ecrit le booleen b dans le fichier .STR */
#ifdef __STDC__
void                wrBool (boolean b)

#else  /* __STDC__ */
void                wrBool (b)
boolean             b;

#endif /* __STDC__ */

{

   if (b)
      BIOwriteByte (outfile, '\1');
   else
      BIOwriteByte (outfile, '\0');
}


/* wrTypeAttr   ecrit le type d'attribut T dans le fichier .STR */
#ifdef __STDC__
void                wrTypeAttr (AttribType T)

#else  /* __STDC__ */
void                wrTypeAttr (T)
AttribType        T;

#endif /* __STDC__ */

{

   switch (T)
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


/* wrConstructeur       ecrit le constructeur C dans le fichier .STR */
#ifdef __STDC__
void                wrConstructeur (RConstruct C)

#else  /* __STDC__ */
void                wrConstructeur (C)
RConstruct        C;

#endif /* __STDC__ */

{

   switch (C)
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


/* wrTypeBase   ecrit le type de base T dans le fichier .STR */
#ifdef __STDC__
void                wrTypeBase (BasicType T)

#else  /* __STDC__ */
void                wrTypeBase (T)
BasicType          T;

#endif /* __STDC__ */

{

   switch (T)
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


/*  wrRegle     ecrit une regle de structure dans le fichier .STR */
#ifdef __STDC__
static void         wrRegle (SRule * pRe1)

#else  /* __STDC__ */
static void         wrRegle (pRe1)
SRule              *pRe1;

#endif /* __STDC__ */

{
   int                 j;

   wrNom (pRe1->SrName);
   wrshort (pRe1->SrNDefAttrs);
   for (j = 1; j <= pRe1->SrNDefAttrs; j++)
      wrshort (pRe1->SrDefAttr[j - 1]);
   for (j = 1; j <= pRe1->SrNDefAttrs; j++)
      wrsignshort (pRe1->SrDefAttrValue[j - 1]);
   for (j = 1; j <= pRe1->SrNDefAttrs; j++)
      wrBool (pRe1->SrDefAttrModif[j - 1]);
   wrshort (pRe1->SrNLocalAttrs);
   for (j = 1; j <= pRe1->SrNLocalAttrs; j++)
      wrshort (pRe1->SrLocalAttr[j - 1]);
   for (j = 1; j <= pRe1->SrNLocalAttrs; j++)
      wrBool (pRe1->SrRequiredAttr[j - 1]);
   wrBool (pRe1->SrAssocElem);
   wrBool (pRe1->SrParamElem);
   wrBool (pRe1->SrUnitElem);
   wrBool (pRe1->SrRecursive);	/* SrRecursDone */
   wrBool (pRe1->SrExportedElem);
   if (pRe1->SrExportedElem)
     {
	wrshort (pRe1->SrExportContent);
	wrNom (pRe1->SrNatExpContent);
     }
   wrshort (pRe1->SrFirstExcept);
   wrshort (pRe1->SrLastExcept);
   wrshort (pRe1->SrNInclusions);
   for (j = 1; j <= pRe1->SrNInclusions; j++)
      wrshort (pRe1->SrInclusion[j - 1]);
   wrshort (pRe1->SrNExclusions);
   for (j = 1; j <= pRe1->SrNExclusions; j++)
      wrshort (pRe1->SrExclusion[j - 1]);
   wrBool (pRe1->SrRefImportedDoc);
   wrConstructeur (pRe1->SrConstruct);
   switch (pRe1->SrConstruct)
	 {
	    case CsNatureSchema:

	       break;
	    case CsBasicElement:
	       wrTypeBase (pRe1->SrBasicType);
	       break;
	    case CsReference:
	       wrshort (pRe1->SrReferredType);
	       wrNom (pRe1->SrRefTypeNat);

	       break;
	    case CsIdentity:
	       wrshort (pRe1->SrIdentRule);
	       break;
	    case CsList:
	       wrshort (pRe1->SrListItem);
	       wrshort (pRe1->SrMinItems);
	       wrshort (pRe1->SrMaxItems);
	       break;
	    case CsChoice:
	       wrsignshort (pRe1->SrNChoices);
	       if (pRe1->SrNChoices > 0)
		  for (j = 1; j <= pRe1->SrNChoices; j++)
		     wrshort (pRe1->SrChoice[j - 1]);
	       break;
	    case CsAggregate:
	    case CsUnorderedAggregate:
	       wrshort (pRe1->SrNComponents);
	       for (j = 1; j <= pRe1->SrNComponents; j++)
		  wrshort (pRe1->SrComponent[j - 1]);
	       for (j = 1; j <= pRe1->SrNComponents; j++)
		  wrBool (pRe1->SrOptComponent[j - 1]);
	       break;
	    case CsConstant:
	       wrshort (pRe1->SrIndexConst);
	       break;
	    case CsPairedElement:
	       wrBool (pRe1->SrFirstOfPair);
	       break;
	    case CsExtensionRule:
	       break;
	 }
}


/* WrSchStruct  ecrit le schema de structure pointe' par pSchStr dans */
/* le fichier de nom fname. Ajoute le suffixe .STR au nom de fichier. */
/* Si Code est nul, ecrit le schema avec un nouveau code d'identification */
/* sinon avec Code comme code d'identification. */
#ifdef __STDC__
boolean             WrSchStruct (Name fname, PtrSSchema pSchStr, int Code)

#else  /* __STDC__ */
boolean             WrSchStruct (fname, pSchStr, Code)
Name                 fname;
PtrSSchema        pSchStr;
int                 Code;

#endif /* __STDC__ */

{
   int                 i, j;	/* met le suffixe STR a la fin du nom

				 * de fichier */
   PtrSSchema        pSc1;
   TtAttribute           *pAt1;

   /* ouvre le fichier */
   outfile = BIOwriteOpen (fname);
   if (outfile == 0)
      return False;
   pSc1 = pSchStr;
   /* ecrit la partie fixe du schema de structure */
   wrNom (pSc1->SsName);
   if (Code == 0)
      /* alloue un nouveau code d'identification au schema compile' */
      wrshort (uniqueident ());	/* SsCode */
   else
      /* le schema compile' a le code d'identification Code */
      wrshort (Code);
   wrNom (pSc1->SsDefaultPSchema);
   /* SsPSchema */
   wrBool (pSc1->SsExtension);
   wrshort (pSc1->SsRootElem);
   wrshort (pSc1->SsNAttributes);
   wrshort (pSc1->SsNRules);
   wrBool (pSc1->SsExport);
   wrshort (pSc1->SsNExceptions);
   for (i = 1; i <= pSc1->SsNExceptions; i++)
      wrshort (pSc1->SsException[i - 1]);	/* ecrit le texte des
						 * constantes */
   i = 0;
   do
     {
	i++;
	BIOwriteByte (outfile, pSc1->SsConstBuffer[i - 1]);
     }
   while (pSc1->SsConstBuffer[i - 1] != '\0' || pSc1->SsConstBuffer[i] != '\0');
   BIOwriteByte (outfile, '\0');	/* SsFirstDynNature */
   /* ecrit les attributs */
   for (i = 1; i <= pSc1->SsNAttributes; i++)
     {
	pAt1 = &pSc1->SsAttribute[i - 1];
	wrNom (pAt1->AttrName);
	wrBool (pAt1->AttrGlobal);
	wrshort (pAt1->AttrFirstExcept);
	wrshort (pAt1->AttrLastExcept);
	wrTypeAttr (pAt1->AttrType);
	switch (pAt1->AttrType)
	      {
		 case AtNumAttr:
		 case AtTextAttr:

		    break;
		 case AtReferenceAttr:
		    wrshort (pAt1->AttrTypeRef);
		    wrNom (pAt1->AttrTypeRefNature);
		    break;
		 case AtEnumAttr:
		    wrshort (pAt1->AttrNEnumValues);
		    for (j = 1; j <= pAt1->AttrNEnumValues; j++)
		       wrNom (pAt1->AttrEnumValue[j - 1]);
		    break;
	      }

     }
   /* ecrit les regles de structuration */
   for (i = 1; i <= pSc1->SsNRules; i++)
      wrRegle (&pSc1->SsRule[i - 1]);
   if (pSc1->SsExtension)
     {
	wrshort (pSc1->SsNExtensRules);
	/* ecrit les regles d'extension */
	for (i = 1; i <= pSc1->SsNExtensRules; i++)
	   wrRegle (&pSc1->SsExtensBlock->EbExtensRule[i - 1]);
     }
   BIOwriteClose (outfile);
   return True;
}
