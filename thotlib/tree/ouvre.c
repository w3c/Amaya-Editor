
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   ouvre.c : gestion de l'ouverture et de la fermeture d'un document.
   Major Changes:
   PMA 7/06/91  remise en forme
   IV : Septembre 92 adaptation Tool Kit
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"

#define EXPORT extern
#include "edit.var"
#include "environ.var"

#include "arbabs.f"
#include "dofile.f"
#include "storage.f"
#include "ouvre.f"
#include "memory.f"
#include "pivlec.f"
#include "refext.f"

/***** debut ajout *****/

#ifdef __STDC__
static void         LibereDescRefInutiles (PtrDocument pDoc)

#else  /* __STDC__ */
static void         LibereDescRefInutiles (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrReferredDescr    pr, prPrec;

   pr = pDoc->DocReferredEl;
   do
      /* parcourt la chaine des descripteurs de reference du document */
     {
	/* (Le premier descripteur de la chaine est bidon) */
	pr = pr->ReNext;
	if (pr != NULL)
	   if (!pr->ReExternalRef)
	      if (pr->ReExtDocRef == NULL)
		 if (pr->ReFirstReference == NULL)
		   {
		      if (pr->ReReferredElem != NULL)
			 pr->ReReferredElem->ElReferredDescr = NULL;
		      pr->ReReferredElem = NULL;
		      prPrec = pr->RePrevious;
		      prPrec->ReNext = pr->ReNext;
		      if (pr->ReNext != NULL)
			 pr->ReNext->RePrevious = prPrec;
		      FreeDescReference (pr);
		      pr = prPrec;
		   }
     }
   while (pr != NULL);
}

/***** fin ajout *****/

/* ---------------------------------------------------------------------- */
/* |    OuvreDoc ouvre le fichier document de nom NomDoc et le charge   | */
/* |            dans pDoc. ChargeDocExt indique s'il faut charger ou    | */
/* |            non les documents externes dont des parties sont        | */
/* |            incluses dans le document a` ouvrir. Retourne faux si   | */
/* |            le document n'a pas pu etre charge'. Charge le          | */
/* |            squelette si Squelette est True. Ne pas charger de      | */
/* |            schema de structure et utiliser pSCharge si             | */
/* |            pSCharge <> NULL.                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             OuvreDoc (Name NomDoc, PtrDocument pDoc, boolean ChargeDocExt, boolean Squelette, PtrSSchema pSCharge, boolean avecMsgAPP)

#else  /* __STDC__ */
boolean             OuvreDoc (NomDoc, pDoc, ChargeDocExt, Squelette, pSCharge, avecMsgAPP)
Name                 NomDoc;
PtrDocument         pDoc;
boolean             ChargeDocExt;
boolean             Squelette;
PtrSSchema        pSCharge;
boolean             avecMsgAPP;

#endif /* __STDC__ */

{
   boolean             ret;
   int                 i;
   PathBuffer          NomDirectory;
   PtrChangedReferredEl      ChngExt;
   BinFile             fichpiv;
   char                texte[MAX_TXT_LEN];

   ret = False;
   if (pDoc != NULL)
     {
	if (!IdentDocNul (pDoc->DocIdent))
	   /* l'identificateur du document est connu, on accede au document par */
	   /* cet identificateur */
	  {
	     /* on n'a pas d'autre outil de stockage des documents que le SGF UNIX */
	     /* On confond identificateur et nom de document */
	     strncpy (pDoc->DocDName, pDoc->DocIdent, MAX_NAME_LENGTH);
	  }
	else
	   /* le document n'a pas d'identificateur, on l'accede par son nom */
	  {
	     strncpy (pDoc->DocDName, NomDoc, MAX_NAME_LENGTH);
	     /* on n'a pas d'autre outil de stockage des documents que le SGF UNIX */
	     /* On confond identificateur et nom de document */
	     strncpy (pDoc->DocIdent, NomDoc, MAX_DOC_IDENT_LEN);
	  }
	if (pDoc->DocDName[0] > ' ')
	   /* nom de document non vide */
	  {
	     /* compose le nom du fichier a ouvrir avec le nom du directory */
	     /* des documents... */
	     if (pDoc->DocDirectory[0] == '\0')
		strncpy (NomDirectory, DirectoryDoc, MAX_PATH);
	     else
		strncpy (NomDirectory, pDoc->DocDirectory, MAX_PATH);
	     BuildFileName (pDoc->DocDName, "PIV", NomDirectory, texte, &i);
	     /* ouvre le fichier 'PIV' */
	     fichpiv = BIOreadOpen (texte);
	     if (fichpiv != 0)
		/* le fichier existe */
		/* internalise le fichier pivot sans charger les documents */
		/* externes qui contiennent des elements inclus. */
	       {
		  /* le document appartient au directory courant */
		  strncpy (pDoc->DocDirectory, NomDirectory, MAX_PATH);
		  ChargeDoc (fichpiv, pDoc, ChargeDocExt, Squelette, pSCharge, avecMsgAPP);
		  BIOreadClose (fichpiv);
		  if (pDoc->DocRootElement != NULL)
		     /* le document lu n'est pas vide */
		    {
		       /* le nom de fichier devient le nom du document */
		       ret = True;
		       /* lit le fichier des references externes s'il existe */
		       /* dans le meme directory que le fichier .PIV */
		       DoFileName (pDoc->DocDName, "EXT", NomDirectory, texte, &i);
		       fichpiv = BIOreadOpen (texte);
		       if (fichpiv != 0)
			 {
			    ChargeExt (fichpiv, pDoc, NULL, False);
			    BIOreadClose (fichpiv);
			 }
		       /* lit le fichier de mise a jour des references sortantes */
		       /* s'il existe dans le meme directory que le fichier .PIV */
		       DoFileName (pDoc->DocDName, "REF", NomDirectory, texte, &i);
		       fichpiv = BIOreadOpen (texte);
		       if (fichpiv != 0)
			 {
			    ChargeRef (fichpiv, &ChngExt);
			    BIOreadClose (fichpiv);
			    /* traite les mises a jour de */
			    /* references sortantes */
			    if (ChngExt != NULL)
			       MiseAJourRef (ChngExt, pDoc);
			 }
		       /* libere les descripteurs d'element reference' inutilise's */
		       LibereDescRefInutiles (pDoc);
		    }
	       }
	  }
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    pDocument cherche si le document d'identificateur IdentDocu est charge'.        | */
/* |            Retourne un pointeur sur son contexte, ou NULL s'il n'est| */
/* |            pas charge'.                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrDocument         pDocument (DocumentIdentifier IdentDocu)

#else  /* __STDC__ */
PtrDocument         pDocument (IdentDocu)
DocumentIdentifier     IdentDocu;

#endif /* __STDC__ */

{
   int                 d;

   for (d = 0; d < MAX_DOCUMENTS; d++)
      if (TabDocuments[d] != NULL)
	 if (MemeIdentDoc (TabDocuments[d]->DocIdent, IdentDocu))
	    return TabDocuments[d];
   return NULL;
}

/* ---------------------------------------------------------------------- */
/* |    SupprDoc supprime les arbres abstraits d'un document et de tous | */
/* |            ses elements associes et parametres. Les schemas de     | */
/* |            structure et de presentation utilises par le document   | */
/* |            ne sont pas liberes...                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                SupprDoc (PtrDocument pDoc)

#else  /* __STDC__ */
void                SupprDoc (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 i;
   PtrTextBuffer      pBuf;
   PtrTextBuffer      pBufSuiv;
   PtrOutReference      RefSort;
   PtrOutReference      RefSortSuiv;
   PtrChangedReferredEl      ERCh;
   PtrChangedReferredEl      ERChSuiv;
   PtrExternalDoc       DocExt;
   PtrExternalDoc       DocExtSuiv;

   if (pDoc != NULL)
     {
	/* libere les buffers de commentaire */
	pBuf = pDoc->DocComment;
	while (pBuf != NULL)
	  {
	     pBufSuiv = pBuf->BuNext;
	     FreeBufTexte (pBuf);
	     pBuf = pBufSuiv;
	  }
	pDoc->DocComment = NULL;
	/* libere tout l'arbre du document et ses descripteurs de reference */
	DeleteElement (&pDoc->DocRootElement);
	/* libere les elements associes */
	for (i = 1; i <= MAX_ASSOC_DOC; i++)
	   if (pDoc->DocAssocRoot[i - 1] != NULL)
	      DeleteElement (&pDoc->DocAssocRoot[i - 1]);
	/* libere les parametres */
	for (i = 1; i <= MAX_PARAM_DOC; i++)
	   if (pDoc->DocParameters[i - 1] != NULL)
	      DeleteElement (&pDoc->DocParameters[i - 1]);
	/* libere le 1er descripteur de reference (bidon) */
	FreeDescReference (pDoc->DocReferredEl);
	pDoc->DocReferredEl = NULL;
	/* libere les descripteurs de references sortantes creees */
	RefSort = pDoc->DocNewOutRef;
	while (RefSort != NULL)
	  {
	     RefSortSuiv = RefSort->OrNext;
	     FreeRefSortante (RefSort);
	     RefSort = RefSortSuiv;
	  }
	/* libere les descripteurs de references sortantes detruites */
	RefSort = pDoc->DocDeadOutRef;
	while (RefSort != NULL)
	  {
	     RefSortSuiv = RefSort->OrNext;
	     FreeRefSortante (RefSort);
	     RefSort = RefSortSuiv;
	  }
	/* libere les decripteurs d'element reference's change's */
	ERCh = pDoc->DocChangedReferredEl;

	while (ERCh != NULL)
	  {
	     ERChSuiv = ERCh->CrNext;
	     DocExt = ERCh->CrReferringDoc;
	     ERCh->CrReferringDoc = NULL;
	     while (DocExt != NULL)
	       {
		  DocExtSuiv = DocExt->EdNext;
		  FreeDocExterne (DocExt);
		  DocExt = DocExtSuiv;
	       }
	     FreeElemRefChng (ERCh);
	     ERCh = ERChSuiv;
	  }

     }
}

/* End Of Module ouvre */
