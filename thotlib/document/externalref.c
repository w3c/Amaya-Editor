
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   refext.c : primitives de gestion des fichiers de references externes
   Major changes:
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "constpiv.h"
#include "typemedia.h"
#include "storage.h"

#undef EXPORT
#define EXPORT extern
#include "edit.var"

#include "arbabs.f"
#include "memory.f"
#include "refelem.f"
#include "pivlec.f"
#include "storage.f"
#include "message.f"


/* ---------------------------------------------------------------------- */
/* |    LabelIntToString convertit l'entier num en un Label correct     | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                LabelIntToString (int num, LabelString strn)

#else  /* __STDC__ */
void                LabelIntToString (num, strn)
int                 num;
LabelString         strn;

#endif /* __STDC__ */

{
   sprintf (strn, "L%d", num);
}


/* ---------------------------------------------------------------------- */
/* |     rdLabel lit depuis le fichier fich, dans la variable lab, un    | */
/* |             label dont le type est defini par la marque c.          | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                rdLabel (char c, LabelString lab, BinFile fich)

#else  /* __STDC__ */
void                rdLabel (c, lab, fich)
char                c;
LabelString         lab;
BinFile             fich;

#endif /* __STDC__ */

{
   int                 j, k;

   /* lit la valeur du label selon le type de label */
   switch (c)
	 {
	    case C_PIV_SHORT_LABEL:
	       if (BIOreadShort (fich, &j))
		  LabelIntToString (j, lab);
	       else
		  lab[0] = '\0';
	       break;
	    case C_PIV_LONG_LABEL:
	       j = 0;
	       if (BIOreadShort (fich, &j))
		  if (BIOreadShort (fich, &k))
		     j = j * 65536 + k;		/* lit la fin du label */
	       LabelIntToString (j, lab);
	       break;
	    case C_PIV_LABEL:
	       j = 0;
	       do
		 {
		    /* on tronque le label s'il est trop long (mais on lit quand meme) */
		    if (j < MAX_LABEL_LEN)
		       j++;
		    BIOreadByte (fich, &lab[j - 1]);
		 }
	       while (lab[j - 1] != '\0');
	       break;
	    default:
	       PivotFormatError ("S");
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |    ChargeExt lit le fichier de references externes .EXT fich et    | */
/* |            charge son contenu pour le document pDocu, si pDocu     | */
/* |            n'est pas NULL. Si pDocu        est NULL, met le contenu du     | */
/* |            fichier dans une chaine de descripteurs d'elements      | */
/* |            reference's dont l'ancre est Ancre.                     | */
/* |            Si LabelSeul est vrai, ne carge que les labels, et pas  | */
/* |            les noms des documents externes.                        | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                ChargeExt (BinFile fich, PtrDocument pDocu, PtrReferredDescr * Ancre, boolean LabelSeul)

#else  /* __STDC__ */
void                ChargeExt (fich, pDocu, Ancre, LabelSeul)
BinFile             fich;
PtrDocument         pDocu;
PtrReferredDescr   *Ancre;
boolean             LabelSeul;

#endif /* __STDC__ */

{
   char                c;
   PtrReferredDescr    pDR, pDRPrec;
   PtrExternalDoc       pDE, NpDE;
   LabelString         lab;
   DocumentIdentifier     NDoc;
   boolean             stop;
   boolean             error;

   error = FALSE;
   /* lit la 1ere marque de label */
   if (!BIOreadByte (fich, &c))
      error = TRUE;
   pDRPrec = NULL;
   if (pDocu == NULL)
      *Ancre = NULL;
   while (!error)

     {
	rdLabel (c, lab, fich);	/* lit la valeur du label */
	if (pDocu == NULL)
	  {
	     /* acquiert un descripteur d'element reference' */
	     GetDescReference (&pDR);
	     /* met le label lu dans le descripteur */
	     strncpy (pDR->ReReferredLabel, lab, MAX_LABEL_LEN);
	     /* chaine le descripteur */
	     if (pDRPrec == NULL)
		/* premier descripteur de la chaine */
		*Ancre = pDR;
	     else
		/* chaine au precedent */
		pDRPrec->ReNext = pDR;
	     pDR->RePrevious = pDRPrec;
	     pDR->ReNext = NULL;
	     pDRPrec = pDR;
	  }
	else
	  {
	     /*cherche le descripteur d'element reference' portant ce label */
	     pDR = pDocu->DocReferredEl;
	     if (pDR != NULL)
		/* saute le premier descripteur bidon */
		pDR = pDR->ReNext;
	     /* parcourt la chaine des descripteurs d'element reference' */
	     stop = FALSE;
	     do
	       {
		  if (pDR == NULL)
		     stop = TRUE;	/* dernier descripteur du document */
		  else if (!pDR->ReExternalRef)
		     if (pDR->ReReferredElem != NULL)
			if (strcmp (pDR->ReReferredElem->ElLabel, lab) == 0)
			   stop = TRUE;		/* trouve' */
		  if (!stop)
		     /* passe au descripteur suivant */
		     pDR = pDR->ReNext;
	       }
	     while (!(stop));
#ifdef TRACE
	     if (pDR == NULL)
		printf ("L\'element portant le label %s est absent\n", lab);
#endif
	  }
	/* lit la liste des documents qui referencent l'element portant */
	/* ce label */
	/* lit la 1ere marque de nom de document */
	if (!BIOreadByte (fich, &c))
	   error = TRUE;
	if (c != (char) C_PIV_DOCNAME || error)
	  {
	     PivotFormatError ("T");	/* ce n'est pas une marque de nom */
	     error = TRUE;
	  }
	while (c == (char) C_PIV_DOCNAME && !error)
	   /* lit l'identificateur du document referencant */
	  {
	     BIOreadIdentDoc (fich, &NDoc);
	     if (pDR != NULL && !error && !LabelSeul)
		/* cree et chaine un descripteur d'element referencant */
	       {
		  GetDocExterne (&NpDE);
		  CopyIdentDoc (&NpDE->EdDocIdent, NDoc);
		  if (pDR->ReExtDocRef == NULL)
		     /* premier descripteur de document referencant */
		     pDR->ReExtDocRef = NpDE;
		  else
		     /* chaine le nouveau descripteur a la fin de la chaine */
		    {
		       pDE = pDR->ReExtDocRef;
		       while (pDE->EdNext != NULL)
			  pDE = pDE->EdNext;
		       pDE->EdNext = NpDE;
		    }
	       }
	     /* lit l'octet qui suit le nom */
	     if (!BIOreadByte (fich, &c))
		error = TRUE;
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    ChargeRef lit le fichier de mise a` jour des references         | */
/* |            sortantes .REF fich et met le contenu du fichier dans   | */
/* |            une chaine de descripteurs dont l'ancre est Ancre.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ChargeRef (BinFile fich, PtrChangedReferredEl * Ancre)

#else  /* __STDC__ */
void                ChargeRef (fich, Ancre)
BinFile             fich;
PtrChangedReferredEl     *Ancre;

#endif /* __STDC__ */

{
   PtrChangedReferredEl      Chng, ChngPrec;
   char                c;
   boolean             error;
   LabelString         lab;

   error = FALSE;
   *Ancre = NULL;
   ChngPrec = NULL;
   /* lit le premier caractere */
   if (!BIOreadByte (fich, &c))
      error = TRUE;
   while (!error)

     {
	rdLabel (c, lab, fich);	/* lit l'ancien label */
	/* acquiert un nouveau descripteur */
	GetElemRefChng (&Chng);
	/* le chaine en queue */
	Chng->CrNext = NULL;
	if (ChngPrec == NULL)
	   *Ancre = Chng;
	else
	   ChngPrec->CrNext = Chng;
	ChngPrec = Chng;
	/* lit l'ancien label */
	strncpy (Chng->CrOldLabel, lab, MAX_LABEL_LEN);
	/* lit le nouveau label */
	if (!BIOreadByte (fich, &c))
	   error = TRUE;
	rdLabel (c, lab, fich);
	if (!error)
	  {
	     strncpy (Chng->CrNewLabel, lab, MAX_LABEL_LEN);
	     /* lit le nom de l'ancien document */
	     /* lit la marque de nom de document */
	     if (!BIOreadByte (fich, &c))
		error = TRUE;
	     if (c != (char) C_PIV_DOCNAME)
	       {
		  PivotFormatError ("T");	/* ce n'est pas une marque de nom */
		  error = TRUE;
	       }
	     else
		/* lit le nom */
	       {
		  BIOreadIdentDoc (fich, &Chng->CrOldDocument);
		  /* lit le nom du nouveau document */
		  if (!error)
		     /* lit la marque de nom de document */
		     if (!BIOreadByte (fich, &c))
			error = TRUE;
		  if (c != (char) C_PIV_DOCNAME)
		    {
		       PivotFormatError ("T");	/* ce n'est pas une marque de nom */
		       error = TRUE;
		    }
		  else
		     /* lit le nom */
		    {
		       BIOreadIdentDoc (fich, &Chng->CrNewDocument);
		       /* lit l'octet qui suit le nom */
		       if (!error)
			  if (!BIOreadByte (fich, &c))
			     error = TRUE;
		    }
	       }
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    MiseAJourRef execute les demandes de mise a` jour de references | */
/* |            contenues dans la chaine de descripteurs dont l'ancre   | */
/* |            est Ancre et qui concernent le document pDoc.           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                MiseAJourRef (PtrChangedReferredEl Ancre, PtrDocument pDoc)

#else  /* __STDC__ */
void                MiseAJourRef (Ancre, pDoc)
PtrChangedReferredEl      Ancre;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrChangedReferredEl      Chng, ChngSuiv;
   PtrReferredDescr    pDR;
   PtrReference        pRef, pRefSuiv;
   boolean             trouve;

   Chng = Ancre;
   /* parcourt la chaine des descripteurs */
   while (Chng != NULL)
     {
	/* saute le premier descripteur d'element reference', bidon */
	pDR = pDoc->DocReferredEl;
	if (pDR != NULL)
	   pDR = pDR->ReNext;
	if (Chng->CrOldLabel[0] == '\0')
	   /* c'est un changement de nom d'un document reference' */
	  {
	     /* cherche tous les descripteurs qui representent des elements */
	     /* reference's appartenant a ce document et change le nom de */
	     /* document qui y figure */
	     while (pDR != NULL)
	       {
		  if (pDR->ReExternalRef)
		     /* c'est un descripteur d'element reference' externe */
		     if (MemeIdentDoc (pDR->ReExtDocument, Chng->CrOldDocument))
			/* l'element reference' appartient au document qui a */
			/* change' de nom, on change le nom dans le */
			/* descripteur */
			CopyIdentDoc (&pDR->ReExtDocument, Chng->CrNewDocument);
		  /* passe au descripteur d'element reference' suivant */
		  pDR = pDR->ReNext;
	       }
	  }
	else
	   /* c'est un element qui a ete detruit ou a change' de document */
	  {
	     /* Cherche le representant de l'ancien element */
	     trouve = FALSE;
	     while (pDR != NULL && !trouve)
	       {
		  if (pDR->ReExternalRef)
		     if (strcmp (pDR->ReReferredLabel, Chng->CrOldLabel) == 0)
			if (MemeIdentDoc (pDR->ReExtDocument, Chng->CrOldDocument))
			   trouve = TRUE;
		  if (!trouve)
		     pDR = pDR->ReNext;
	       }
	     if (trouve)
		/* modifie le descripteur d'element reference' externe */
	       {
		  if (Chng->CrNewLabel[0] != '\0')
		     /* l'element reference' a change' de document */
		    {
		       strncpy (pDR->ReReferredLabel, Chng->CrNewLabel, MAX_LABEL_LEN);
		       CopyIdentDoc (&pDR->ReExtDocument, Chng->CrNewDocument);
		    }
		  else
		     /* l'element reference' a ete detruit */
		    {
		       /* annule les references qui designaient cet element */
		       pRef = pDR->ReFirstReference;
		       while (pRef != NULL)
			 {
			    pRefSuiv = pRef->RdNext;
			    pRef->RdInternalRef = TRUE;
			    DeleteReference (pRef);
			    pRef = pRefSuiv;
			 }
		    }
	       }
	  }
	/* libere le descripteur qui a ete traite' */
	ChngSuiv = Chng->CrNext;
	FreeElemRefChng (Chng);
	/* passe au descripteur suivant */
	Chng = ChngSuiv;
     }
}
/* End of module refext */
