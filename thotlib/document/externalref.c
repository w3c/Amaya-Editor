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
   Management of external references files.

 */

#include "thot_sys.h"
#include "constmedia.h"
#include "constpiv.h"
#include "typemedia.h"
#include "fileaccess.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"

#include "tree_f.h"
#include "memory_f.h"
#include "references_f.h"
#include "readpivot_f.h"
#include "fileaccess_f.h"
#include "message_f.h"


/*----------------------------------------------------------------------
   ConvertIntToLabel converts integer num into a Thot label		
  ----------------------------------------------------------------------*/


#ifdef __STDC__
void                ConvertIntToLabel (int num, LabelString strng)

#else  /* __STDC__ */
void                ConvertIntToLabel (num, strng)
int                 num;
LabelString         strng;

#endif /* __STDC__ */

{
   usprintf (strng, TEXT("L%d"), num);
}


/*----------------------------------------------------------------------
   ReadLabel reads a label from a file				
   		The label type is specified by labelType.		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ReadLabel (CHAR_T labelType, LabelString label, BinFile file)

#else  /* __STDC__ */
void                ReadLabel (labelType, label, file)
CHAR_T              labelType;
LabelString         label;
BinFile             file;

#endif /* __STDC__ */

{
   int                 j, k;

   /* lit la valeur du label selon le type de label */
   switch (labelType)
	 {
	    case C_PIV_SHORT_LABEL:
	       if (TtaReadShort (file, &j))
		  ConvertIntToLabel (j, label);
	       else
		  /* error */
		  label[0] = EOS;
	       break;
	    case C_PIV_LONG_LABEL:
	       j = 0;
	       if (TtaReadShort (file, &j))
		  if (TtaReadShort (file, &k))
		     j = j * 65536 + k;
	       ConvertIntToLabel (j, label);
	       break;
	    case C_PIV_LABEL:
	       j = 0;
	       do
		 {
		    TtaReadByte (file, &label[j]);
		    /* drop last bytes if the label is too long */
		    if (j < MAX_LABEL_LEN)
		       j++;
		 }
	       while (label[j - 1] != EOS);
	       break;
	    default:
	       DisplayPivotMessage (TEXT("S"));
	       break;
	 }
}


/*----------------------------------------------------------------------
   LoadEXTfile lit le fichier de references externes.EXT et		
   charge son contenu pour le document pDoc, si pDoc	
   n'est pas NULL. Si pDoc est NULL, met le contenu du	
   fichier dans une chaine de descripteurs d'elements      
   reference's dont l'ancre est Anchor.			
   Si labelsOnly est vrai, ne charge que les labels,	
   et pas les noms des documents externes.			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                LoadEXTfile (BinFile file, PtrDocument pDoc, PtrReferredDescr * Anchor, ThotBool labelsOnly)

#else  /* __STDC__ */
void                LoadEXTfile (file, pDoc, Anchor, labelsOnly)
BinFile             file;
PtrDocument         pDoc;
PtrReferredDescr   *Anchor;
ThotBool            labelsOnly;

#endif /* __STDC__ */

{
   PtrReferredDescr    pRefD, pPrevRefD;
   PtrExternalDoc      pExtDoc, pNewExtDoc;
   LabelString         label;
   DocumentIdentifier  docIdent;
   ThotBool            stop, error;
   CHAR_T                c;

   error = FALSE;
   /* lit la 1ere marque de label */
   if (!TtaReadByte (file, &c))
      error = TRUE;
   pPrevRefD = NULL;
   if (pDoc == NULL)
      *Anchor = NULL;
   while (!error)
     {
	ReadLabel (c, label, file);
	if (pDoc == NULL)
	  {
	     /* acquiert un descripteur d'element reference' */
	     GetReferredDescr (&pRefD);
	     /* met le label lu dans le descripteur */
	     ustrncpy (pRefD->ReReferredLabel, label, MAX_LABEL_LEN);
	     /* chaine le descripteur */
	     if (pPrevRefD == NULL)
		/* premier descripteur de la chaine */
		*Anchor = pRefD;
	     else
		/* chaine au precedent */
		pPrevRefD->ReNext = pRefD;
	     pRefD->RePrevious = pPrevRefD;
	     pRefD->ReNext = NULL;
	     pPrevRefD = pRefD;
	  }
	else
	  {
	     /*cherche le descripteur d'element reference' portant ce label */
	     pRefD = pDoc->DocReferredEl;
	     if (pRefD != NULL)
		/* saute le premier descripteur bidon */
		pRefD = pRefD->ReNext;
	     /* parcourt la chaine des descripteurs d'element reference' */
	     stop = FALSE;
	     do
	       {
		  if (pRefD == NULL)
		     stop = TRUE;	/* dernier descripteur du document */
		  else if (!pRefD->ReExternalRef)
		     if (pRefD->ReReferredElem != NULL)
			if (ustrcmp (pRefD->ReReferredElem->ElLabel, label) == 0)
			   /* trouve' */
			   stop = TRUE;
		  if (!stop)
		     /* passe au descripteur suivant */
		     pRefD = pRefD->ReNext;
	       }
	     while (!stop);
#ifdef TRACE
	     if (pRefD == NULL)
		printf ("Element with label %s not found\n", label);
#endif
	  }
	/* lit la liste des documents qui referencent l'element portant */
	/* ce label */
	/* lit la 1ere marque de nom de document */
	if (!TtaReadByte (file, &c))
	   error = TRUE;
	if (c != (CHAR_T) C_PIV_DOCNAME || error)
	  {
	     /* ce n'est pas une marque de nom */
		DisplayPivotMessage (TEXT("T"));
	     error = TRUE;
	  }
	while (c == (CHAR_T) C_PIV_DOCNAME && !error)
	   /* lit l'identificateur du document referencant */
	  {
	     TtaReadDocIdent (file, &docIdent);
	     if (pRefD != NULL && !error && !labelsOnly)
		/* cree et chaine un descripteur d'element referencant */
	       {
		  GetExternalDoc (&pNewExtDoc);
		  CopyDocIdent (&pNewExtDoc->EdDocIdent, docIdent);
		  if (pRefD->ReExtDocRef == NULL)
		     /* premier descripteur de document referencant */
		     pRefD->ReExtDocRef = pNewExtDoc;
		  else
		     /* chaine le nouveau descripteur a la fin de la chaine */
		    {
		       pExtDoc = pRefD->ReExtDocRef;
		       while (pExtDoc->EdNext != NULL)
			  pExtDoc = pExtDoc->EdNext;
		       pExtDoc->EdNext = pNewExtDoc;
		    }
	       }
	     /* lit l'octet qui suit le nom */
	     if (!TtaReadByte (file, &c))
		error = TRUE;
	  }
     }
}

/*----------------------------------------------------------------------
   LoadREFfile lit le fichier de mise a` jour des references		
   sortantes .REF et met le contenu du fichier dans	
   une chaine de descripteurs dont l'ancre est Anchor.	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                LoadREFfile (BinFile file, PtrChangedReferredEl * Anchor)

#else  /* __STDC__ */
void                LoadREFfile (file, Anchor)
BinFile             file;
PtrChangedReferredEl *Anchor;

#endif /* __STDC__ */

{
   PtrChangedReferredEl pChnRef, pPrevChnRef;
   CHAR_T                c;
   ThotBool            error;
   LabelString         label;

   error = FALSE;
   *Anchor = NULL;
   pPrevChnRef = NULL;
   /* read first character in file */
   if (!TtaReadByte (file, &c))
      error = TRUE;
   while (!error)

     {
	/* read old label */
	ReadLabel (c, label, file);
	/* acquiert un nouveau descripteur */
	GetChangedReferredEl (&pChnRef);
	/* le chaine en queue */
	pChnRef->CrNext = NULL;
	if (pPrevChnRef == NULL)
	   *Anchor = pChnRef;
	else
	   pPrevChnRef->CrNext = pChnRef;
	pPrevChnRef = pChnRef;
	/* lit l'ancien label */
	ustrncpy (pChnRef->CrOldLabel, label, MAX_LABEL_LEN);
	/* lit le nouveau label */
	if (!TtaReadByte (file, &c))
	   error = TRUE;
	ReadLabel (c, label, file);
	if (!error)
	  {
	     ustrncpy (pChnRef->CrNewLabel, label, MAX_LABEL_LEN);
	     /* lit le nom de l'ancien document */
	     /* lit la marque de nom de document */
	     if (!TtaReadByte (file, &c))
		error = TRUE;
	     if (c != (CHAR_T) C_PIV_DOCNAME)
	       {
		  /* a name was expected */
		  DisplayPivotMessage (TEXT("T"));
		  error = TRUE;
	       }
	     else
		/* read the name */
	       {
		  TtaReadDocIdent (file, &pChnRef->CrOldDocument);
		  /* lit le nom du nouveau document */
		  if (!error)
		     /* lit la marque de nom de document */
		     if (!TtaReadByte (file, &c))
			error = TRUE;
		  if (c != (CHAR_T) C_PIV_DOCNAME)
		    {
		       /* a name was expected */
		       DisplayPivotMessage (TEXT("T"));
		       error = TRUE;
		    }
		  else
		     /* lit le nom */
		    {
		       TtaReadDocIdent (file, &pChnRef->CrNewDocument);
		       /* lit l'octet qui suit le nom */
		       if (!error)
			  if (!TtaReadByte (file, &c))
			     error = TRUE;
		    }
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   UpdateREFdescriptors execute les demandes de mise a` jour de references 
   contenues dans la chaine de descripteurs dont l'ancre   
   est Anchor et qui concernent le document pDoc.          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                UpdateREFdescriptors (PtrChangedReferredEl Anchor, PtrDocument pDoc)

#else  /* __STDC__ */
void                UpdateREFdescriptors (Anchor, pDoc)
PtrChangedReferredEl Anchor;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrChangedReferredEl pChnRef, pNextChnRef;
   PtrReferredDescr    pRefD;
   PtrReference        pRef, pRefSuiv;
   ThotBool            found;

   pChnRef = Anchor;
   /* parcourt la chaine des descripteurs */
   while (pChnRef != NULL)
     {
	/* saute le premier descripteur d'element reference', bidon */
	pRefD = pDoc->DocReferredEl;
	if (pRefD != NULL)
	   pRefD = pRefD->ReNext;
	if (pChnRef->CrOldLabel[0] == EOS)
	   /* c'est un changement de nom d'un document reference' */
	  {
	     /* cherche tous les descripteurs qui representent des elements */
	     /* reference's appartenant a ce document et change le nom de */
	     /* document qui y figure */
	     while (pRefD != NULL)
	       {
		  if (pRefD->ReExternalRef)
		     /* c'est un descripteur d'element reference' externe */
		     if (SameDocIdent (pRefD->ReExtDocument, pChnRef->CrOldDocument))
			/* l'element reference' appartient au document qui a */
			/* change' de nom, on change le nom dans le */
			/* descripteur */
			CopyDocIdent (&pRefD->ReExtDocument, pChnRef->CrNewDocument);
		  /* passe au descripteur d'element reference' suivant */
		  pRefD = pRefD->ReNext;
	       }
	  }
	else
	   /* c'est un element qui a ete detruit ou a change' de document */
	  {
	     /* Cherche le representant de l'ancien element */
	     found = FALSE;
	     while (pRefD != NULL && !found)
	       {
		  if (pRefD->ReExternalRef)
		     if (ustrcmp (pRefD->ReReferredLabel, pChnRef->CrOldLabel) == 0)
			if (SameDocIdent (pRefD->ReExtDocument, pChnRef->CrOldDocument))
			   found = TRUE;
		  if (!found)
		     pRefD = pRefD->ReNext;
	       }
	     if (found)
		/* modifie le descripteur d'element reference' externe */
	       {
		  if (pChnRef->CrNewLabel[0] != EOS)
		     /* l'element reference' a change' de document */
		    {
		       ustrncpy (pRefD->ReReferredLabel, pChnRef->CrNewLabel, MAX_LABEL_LEN);
		       CopyDocIdent (&pRefD->ReExtDocument, pChnRef->CrNewDocument);
		    }
		  else
		     /* l'element reference' a ete detruit */
		    {
		       /* annule les references qui designaient cet element */
		       pRef = pRefD->ReFirstReference;
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
	pNextChnRef = pChnRef->CrNext;
	FreeChangedReferredEl (pChnRef);
	/* passe au descripteur suivant */
	pChnRef = pNextChnRef;
     }
}
