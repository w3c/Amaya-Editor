/*
   Manage free-lists and memory allocation.
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "typecorr.h"
#include "libmsg.h"
#include "message.h"
#include "storage.h"

#undef  EXPORT
#define EXPORT
#include "boxes_tv.h"
#include "edit_tv.h"
/* Identification des messages */

static int          NbLibBufTexte;
static int          NbOccBufTexte;
static PtrTextBuffer PtFreeBufTexte;

static int          NbLibElement;
static int          NbOccElement;
static PtrElement   PtFreeElement;

static int          NbLibAttr;
static int          NbOccAttr;
static PtrAttribute  PtFreeAttr;

static int          NbLibDescRef;
static int          NbOccDescRef;
static PtrReferredDescr PtFreeDescRef;

static int          NbLibDocExterne;
static int          NbOccDocExterne;
static PtrExternalDoc PtFreeDocExterne;

static int          NbLibDescCopie;
static int          NbOccDescCopie;
static PtrCopyDescr PtFreeDescCopie;


static int          NbLibReference;
static int          NbOccReference;
static PtrReference PtFreeReference;

static int          NbLibRefSortante;
static int          NbOccRefSortante;
static PtrOutReference PtFreeRefSortante;

static int          NbLibElemRefChng;
static int          NbOccElemRefChng;
static PtrChangedReferredEl PtFreeElemRefChng;

static int          NbLibRefEntrantes;
static int          NbOccRefEntrantes;
static PtrEnteringReferences PtFreeRefEntrantes;

static int          NbLibFichRefChng;
static int          NbOccFichRefChng;
static PtrReferenceChange PtFreeFichRefChng;

static int          NbLibPave;
static int          NbOccPave;
static PtrAbstractBox      PtFreePave;

static int          NbLibDocument;
static int          NbOccDocument;
static PtrDocument  PtFreeDocument;

static int          NbLibSchPres;
static int          NbOccSchPres;
static PtrPSchema   PtFreeSPres;

static int          NbLibHandleSchPres;
static int          NbOccHandleSchPres;
static PtrHandlePSchema PtFreeHandleSchPres;

static int          NbLibSchStruct;
static int          NbOccSchStruct;
static PtrSSchema PtFreeSStruct;

static int          NbLibBlocExtens;
static int          NbOccBlocExtens;
static PtrExtensBlock PtFreeBlocExtens;

static int          NbLibReglePres;
static int          NbOccReglePres;
static PtrPRule PtFreeReglePres;

static int          NbLibCondReglePres;
static int          NbOccCondReglePres;
static PtrCondition PtFreeCondReglePres;

static int          NbLibRRetard;
static int          NbOccRRetard;
static PtrDelayedPRule PtFreeRRetard;

static PtrBox 	PtFreBox;	/* File des contextes de boite libres */
static int     	NbLibBox;
static int     	NbOccBox;
static PtrPosRelations 	PtFreBPos; /* File des blocs relation de position libres */
static int     	NbLibBPos;
static int     	NbOccBPos;
static PtrDimRelations 	PtFreBDim; /* File des blocs relation de dimension libres */
static int     	NbLibBDim;
static int     	NbOccBDim;
static PtrLine 	PtFreeLine;	/* File des contextes de ligne libres */
static int     	NbLibLine;
static int     	NbOccLine;

static int NbLibDico;
static int NbOccDico;
static PtrDico PtFreeDico;

#include "absboxlist_f.h"
#include "memory_f.h"
#include "fileaccess_f.h"

/* ---------------------------------------------------------------------- */
/* |    DicoAlloc effectue un malloc controle.                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void               *DicoAlloc (unsigned int n)

#else  /* __STDC__ */
void               *DicoAlloc (n)
unsigned int        n;

#endif /* __STDC__ */

{

   if (n > 0)
      return ((char *) malloc ((size_t) n));
   return (NULL);
}

/* ----------------------------------------------------------------------
   TtaGetMemory

   Allocates dynamically a buffer of a specific size

   Parameters:
   size: size in bytes (like using malloc) of the desired allocated
   buffer.

   Return value:
   the address of the allocated buffer.

   See also:
   TtaFreeMemory.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void               *TtaGetMemory (unsigned int n)

#else  /* __STDC__ */
void               *TtaGetMemory (n)
unsigned int        n;

#endif /* __STDC__ */

{
   void               *res;

   if (n == 0)
      n++;
   res = malloc ((size_t) n);
   if (!res)
      TtaDisplaySimpleMessage (FATAL, LIB, NOT_ENOUGH_MEMORY);
   return (res);
}


/* ----------------------------------------------------------------------
   TtaFreeMemory

   Frees the memory allocated to a buffer by TtaGetMemory

   Parameters:
   buffer: the address of the buffer

   See also:
   TtaGetMemory.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaFreeMemory (void *ptr)
#else  /* __STDC__ */
void                TtaFreeMemory (ptr)
void               *ptr;

#endif /* __STDC__ */

{
   if (ptr)
      free (ptr);
}


/* ----------------------------------------------------------------------
   TtaStrdup

   Copy the string given in argument to a newly allocated piece of memory.

   Parameters:
   str: a zero terminated string.

   See also:
   TtaGetMemory.
   ---------------------------------------------------------------------- */

#ifdef __STDC__
char               *TtaStrdup (char *str)

#else  /* __STDC__ */
char               *TtaStrdup (str)
char               *str;

#endif /* __STDC__ */

{
   char               *res;

   if (str == NULL)
      return (NULL);
   res = (char *) TtaGetMemory (strlen (str) + 1);
   if (res == NULL)
      return (res);
   strcpy (res, str);
   return (res);
}

/* ---------------------------------------------------------------------- */
/* |    ThotReAlloc effectue un realloc blinde'.                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void               *ThotRealloc (void *ptr, unsigned int n)

#else  /* __STDC__ */
void               *ThotRealloc (ptr, n)
void               *ptr;
unsigned int        n;

#endif /* __STDC__ */

{
   void               *res;

   if (n == 0)
      n++;
   res = realloc (ptr, (size_t) n);
   if (!res)			/* Plus de memoire */
      TtaDisplaySimpleMessage (FATAL, LIB, NOT_ENOUGH_MEMORY);
   return res;
}

/* ---------------------------------------------------------------------- */
/* |    EdInitMem effectue l'initialisation des variables de gestion de | */
/* |            liste.                                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                EdInitMem ()

#else  /* __STDC__ */
void                EdInitMem ()
#endif				/* __STDC__ */

{

   NbLibBufTexte = 0;
   NbOccBufTexte = 0;
   PtFreeBufTexte = NULL;

   NbLibElement = 0;
   NbOccElement = 0;
   PtFreeElement = NULL;

   NbLibAttr = 0;
   NbOccAttr = 0;
   PtFreeAttr = NULL;

   NbLibDescRef = 0;
   NbOccDescRef = 0;
   PtFreeDescRef = NULL;

   NbLibDescCopie = 0;
   NbOccDescCopie = 0;
   PtFreeDescCopie = NULL;

   NbLibDocExterne = 0;
   NbOccDocExterne = 0;
   PtFreeDocExterne = NULL;

   NbLibReference = 0;
   NbOccReference = 0;
   PtFreeReference = NULL;

   NbLibRefSortante = 0;
   NbOccRefSortante = 0;
   PtFreeRefSortante = NULL;

   NbLibElemRefChng = 0;
   NbOccElemRefChng = 0;
   PtFreeElemRefChng = NULL;

   NbLibRefEntrantes = 0;
   NbOccRefEntrantes = 0;
   PtFreeRefEntrantes = NULL;

   NbLibFichRefChng = 0;
   NbOccFichRefChng = 0;
   PtFreeFichRefChng = NULL;

   NbLibPave = 0;
   NbOccPave = 0;
   PtFreePave = NULL;

   NbLibDocument = 0;
   NbOccDocument = 0;
   PtFreeDocument = NULL;

   NbLibSchPres = 0;
   NbOccSchPres = 0;
   PtFreeSPres = NULL;

   NbLibHandleSchPres = 0;
   NbOccHandleSchPres = 0;
   PtFreeHandleSchPres = NULL;

   NbLibSchStruct = 0;
   NbOccSchStruct = 0;
   PtFreeSStruct = NULL;

   NbLibBlocExtens = 0;
   NbOccBlocExtens = 0;
   PtFreeBlocExtens = NULL;

   NbLibReglePres = 0;
   NbOccReglePres = 0;
   PtFreeReglePres = NULL;

   NbLibCondReglePres = 0;
   NbOccCondReglePres = 0;
   PtFreeCondReglePres = NULL;

   NbLibRRetard = 0;
   NbOccRRetard = 0;
   PtFreeRRetard = NULL;

/*CORR */ NbLibDico = 0;
/*CORR */ NbOccDico = 0;
/*CORR */ PtFreeDico = NULL;

}


/* ---------------------------------------------------------------------- */
/* |    ListeMem fait un listing de l'occupation memoire.               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ListeMem (FILE * outfile)

#else  /* __STDC__ */
void                ListeMem (outfile)
FILE               *outfile;

#endif /* __STDC__ */

{
#ifndef PRODUIT_V2
   int                 Size, Total, GrandTotal;

   GrandTotal = 0;
   fprintf (outfile, "%10d buffers libres\n", NbLibBufTexte);
   fprintf (outfile, "%10d buffers utilises\n", NbOccBufTexte);
   Size = sizeof (TextBuffer);
   Total = (NbLibBufTexte + NbOccBufTexte) * Size;
   fprintf (outfile, "     %10d octets par buffer, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d elements libres\n", NbLibElement);
   fprintf (outfile, "%10d elements utilises\n", NbOccElement);
   Size = sizeof (ElementDescr);
   Total = (NbLibElement + NbOccElement) * Size;
   fprintf (outfile, "     %10d octets par element, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d attributs libres\n", NbLibAttr);
   fprintf (outfile, "%10d attributs utilises\n", NbOccAttr);
   Size = sizeof (AttributeBlock);
   Total = (NbLibAttr + NbOccAttr) * Size;
   fprintf (outfile, "     %10d octets par attribut, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d descripteurs de reference libres\n", NbLibDescRef);
   fprintf (outfile, "%10d descripteurs de reference utilises\n", NbOccDescRef);
   Size = sizeof (ReferredElemDescriptor);
   Total = (NbLibDescRef + NbOccDescRef) * Size;
   fprintf (outfile, "     %10d octets par descripteur, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d descripteurs de copie libres\n", NbLibDescCopie);
   fprintf (outfile, "%10d descripteurs de copie utilises\n", NbOccDescCopie);
   Size = sizeof (CopyDescriptor);
   Total = (NbLibDescCopie + NbOccDescCopie) * Size;
   fprintf (outfile, "     %10d octets par descripteur, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d descripteurs de documents externes libres\n",
	    NbLibDocExterne);
   fprintf (outfile, "%10d descripteurs de documents externes utilises\n",
	    NbOccDocExterne);
   Size = sizeof (ExternalDoc);
   Total = (NbLibDocExterne + NbOccDocExterne) * Size;
   fprintf (outfile, "     %10d octets par descripteur, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d references libres\n", NbLibReference);
   fprintf (outfile, "%10d references utilisees\n", NbOccReference);
   Size = sizeof (ReferenceDescriptor);
   Total = (NbLibReference + NbOccReference) * Size;
   fprintf (outfile, "     %10d octets par reference, total =%10d\n", Size, Total);
   GrandTotal += Total;


   fprintf (outfile, "%10d references sortantes libres\n", NbLibRefSortante);
   fprintf (outfile, "%10d references sortantes utilisees\n", NbOccRefSortante);
   Size = sizeof (OutReference);
   Total = (NbLibRefSortante + NbOccRefSortante) * Size;
   fprintf (outfile, "     %10d octets par reference sortante, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d elem. ref. changes libres\n", NbLibElemRefChng);
   fprintf (outfile, "%10d elem. ref. changes utilisees\n", NbOccElemRefChng);
   Size = sizeof (ChangedReferredEl);
   Total = (NbLibElemRefChng + NbOccElemRefChng) * Size;
   fprintf (outfile, "     %10d octets par elem. ref. change, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d ref. entrantes libres\n", NbLibRefEntrantes);
   fprintf (outfile, "%10d ref. entrantes utilisees\n", NbOccRefEntrantes);
   Size = sizeof (RefEntrantes);
   Total = (NbLibRefEntrantes + NbOccRefEntrantes) * Size;
   fprintf (outfile, "     %10d octets par ref. entrante, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d fich. ref. chng. libres\n", NbLibFichRefChng);
   fprintf (outfile, "%10d fich. ref. chng. utilisees\n", NbOccFichRefChng);
   Size = sizeof (ReferenceChange);
   Total = (NbLibFichRefChng + NbOccFichRefChng) * Size;
   fprintf (outfile, "     %10d octets par fich. ref. chng., total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d paves libres\n", NbLibPave);
   fprintf (outfile, "%10d paves utilises\n", NbOccPave);
   Size = sizeof (AbstractBox);
   Total = (NbLibPave + NbOccPave) * Size;
   fprintf (outfile, "     %10d octets par pave, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d documents libres\n", NbLibDocument);
   fprintf (outfile, "%10d documents utilises\n", NbOccDocument);
   Size = sizeof (DocumentDescr);
   Total = (NbLibDocument + NbOccDocument) * Size;
   fprintf (outfile, "     %10d octets par document, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d schemas de presentation libres\n", NbLibSchPres);
   fprintf (outfile, "%10d schemas de presentation utilises\n", NbOccSchPres);
   Size = sizeof (PresentSchema);
   Total = (NbLibSchPres + NbOccSchPres) * Size;
   fprintf (outfile, "     %10d octets par schema, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d pointeurs de schemas de presentation libres\n", NbLibHandleSchPres);
   fprintf (outfile, "%10d pointeurs de schemas de presentation utilises\n", NbOccHandleSchPres);
   Size = sizeof (HandlePSchema);
   Total = (NbLibHandleSchPres + NbOccHandleSchPres) * Size;
   fprintf (outfile, "     %10d octets par pointeur de schema, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d schemas de structure libres\n", NbLibSchStruct);
   fprintf (outfile, "%10d schemas de structure utilises\n", NbOccSchStruct);
   Size = sizeof (StructSchema);
   Total = (NbLibSchStruct + NbOccSchStruct) * Size;
   fprintf (outfile, "     %10d octets par schema, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d blocs extension libres\n", NbLibBlocExtens);
   fprintf (outfile, "%10d blocs extension utilises\n", NbOccBlocExtens);
   Size = sizeof (ExtensBlock);
   Total = (NbLibBlocExtens + NbOccBlocExtens) * Size;
   fprintf (outfile, "     %10d octets par bloc, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d regles de presentation libres\n", NbLibReglePres);
   fprintf (outfile, "%10d regles de presentation utilisees\n", NbOccReglePres);
   Size = sizeof (PresRule);
   Total = (NbLibReglePres + NbOccReglePres) * Size;
   fprintf (outfile, "     %10d octets par regle, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d conditions de presentation libres\n", NbLibCondReglePres);
   fprintf (outfile, "%10d conditions de presentation utilisees\n", NbOccCondReglePres);
   Size = sizeof (Condition);
   Total = (NbLibCondReglePres + NbOccCondReglePres) * Size;
   fprintf (outfile, "     %10d octets par regle, total =%10d\n", Size, Total);
   GrandTotal += Total;

   fprintf (outfile, "%10d regles retardees libres\n", NbLibRRetard);
   fprintf (outfile, "%10d regles retardees utilisees\n", NbOccRRetard);
   Size = sizeof (DelayedPRule);
   Total = (NbLibRRetard + NbOccRRetard) * Size;
   fprintf (outfile, "     %10d octets par regle retardee, total =%10d\n",
	    Size, Total);
   GrandTotal += Total;

/*CORR */ fprintf (outfile, "%10d dicos libres\n", NbLibDico);
/*CORR */ fprintf (outfile, "%10d dicos utilises\n", NbOccDico);
/*CORR */ Size = sizeof (Dictionnaire);
/*CORR */ Total = (NbLibDico + NbOccDico) * Size;
/*CORR */ fprintf (outfile, "     %10d octets par dico, total =%10d\n",
/*CORR */ Size, Total);
/*CORR */ GrandTotal += Total;
/*CORR il faut ajouter la place occupe par les chaines de caracteres ??? */
/*CORR */
   fprintf (outfile, "\n");
   fprintf (outfile, "Total pour l\'Editeur : %10d octets.\n", GrandTotal);
   fprintf (outfile, "\n");
#endif
}

/* ---------------------------------------------------------------------- */
/* |    GetBufTexte alloue un buffer de texte.                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetBufTexte (PtrTextBuffer * pBT)

#else  /* __STDC__ */
void                GetBufTexte (pBT)
PtrTextBuffer     *pBT;

#endif /* __STDC__ */

{
   PtrTextBuffer      pBu1;

   if (PtFreeBufTexte == NULL)
     {
	/* pas de buffer dans la chaine des libres, acquiert un nouveau buffer */
	*pBT = (PtrTextBuffer) TtaGetMemory (sizeof (TextBuffer));
     }
   else
     {
	/* recupere un buffer en tete de la chaine des libres */
	*pBT = PtFreeBufTexte;
	PtFreeBufTexte = (*pBT)->BuNext;
	NbLibBufTexte--;
     }
   NbOccBufTexte++;
   /* initialise le buffer */
   pBu1 = *pBT;
   memset (pBu1, 0, sizeof (TextBuffer));
   pBu1->BuNext = NULL;
   pBu1->BuPrevious = NULL;
   pBu1->BuLength = 0;
   pBu1->BuContent[0] = '\0';
}

/* ---------------------------------------------------------------------- */
/* |    FreeBufTexte libere un buffer de texte.                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeBufTexte (PtrTextBuffer pBT)

#else  /* __STDC__ */
void                FreeBufTexte (pBT)
PtrTextBuffer      pBT;

#endif /* __STDC__ */

{

   /* insere le buffer en tete de la chaine des libres */
   if (pBT != NULL)
     {
	pBT->BuNext = PtFreeBufTexte;
	PtFreeBufTexte = pBT;
	NbLibBufTexte++;
	NbOccBufTexte--;
     }
}

/* ---------------------------------------------------------------------- */
/* |    GetElement alloue un element.                                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetElement (PtrElement * pEl)

#else  /* __STDC__ */
void                GetElement (pEl)
PtrElement         *pEl;

#endif /* __STDC__ */

{
   int                 i;
   PtrElement          pEl1;


   if (PtFreeElement == NULL)
      *pEl = (PtrElement) TtaGetMemory (sizeof (ElementDescr));
   else
     {
	*pEl = PtFreeElement;
	PtFreeElement = (*pEl)->ElNext;
	NbLibElement--;
     }
   NbOccElement++;
   pEl1 = *pEl;
   memset (pEl1, 0, sizeof (ElementDescr));
   pEl1->ElStructSchema = NULL;
   pEl1->ElTypeNumber = 0;
   pEl1->ElAssocNum = 0;
   pEl1->ElFirstAttr = NULL;
   pEl1->ElVolume = 0;
   pEl1->ElCopyDescr = NULL;
   pEl1->ElFirstPRule = NULL;
   pEl1->ElComment = NULL;
   for (i = 1; i <= MAX_VIEW_DOC; i++)
      pEl1->ElAbstractBox[i - 1] = NULL;
   pEl1->ElParent = NULL;
   pEl1->ElPrevious = NULL;
   pEl1->ElNext = NULL;
   pEl1->ElReferredDescr = NULL;
   pEl1->ElIsCopy = FALSE;
   pEl1->ElSource = NULL;
   pEl1->ElHolophrast = FALSE;
   pEl1->ElAccess = AccessInherited;
   pEl1->ElTransContent = FALSE;
   pEl1->ElTransAttr = FALSE;
   pEl1->ElTransPres = FALSE;
   pEl1->ElTerminal = FALSE;
   pEl1->ElFirstChild = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    FreeElement libere un element.                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeElement (PtrElement pEl)

#else  /* __STDC__ */
void                FreeElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{

   pEl->ElNext = PtFreeElement;
   PtFreeElement = pEl;
   NbLibElement++;
   NbOccElement--;
}

/* ---------------------------------------------------------------------- */
/* |    GetAttr alloue un attribut.                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetAttr (PtrAttribute * pAttr)

#else  /* __STDC__ */
void                GetAttr (pAttr)
PtrAttribute        *pAttr;

#endif /* __STDC__ */

{
   PtrAttribute         pAt1;


   if (PtFreeAttr == NULL)
      *pAttr = (PtrAttribute) TtaGetMemory (sizeof (TtAttribute));
   else
     {
	*pAttr = PtFreeAttr;
	PtFreeAttr = (*pAttr)->AeNext;
	NbLibAttr--;
     }
   NbOccAttr++;
   pAt1 = *pAttr;
   memset (pAt1, 0, sizeof (TtAttribute));
   pAt1->AeNext = NULL;
   pAt1->AeAttrSSchema = NULL;
   pAt1->AeAttrNum = 0;
   pAt1->AeAttrType = AtEnumAttr;
   pAt1->AeAttrValue = 0;
   pAt1->AeDefAttr = FALSE;
}

/* ---------------------------------------------------------------------- */
/* |    FreeAttr libere un attribut.                                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeAttr (PtrAttribute pAttr)

#else  /* __STDC__ */
void                FreeAttr (pAttr)
PtrAttribute         pAttr;

#endif /* __STDC__ */

{

   pAttr->AeNext = PtFreeAttr;
   PtFreeAttr = pAttr;
   NbLibAttr++;
   NbOccAttr--;
}

/* ---------------------------------------------------------------------- */
/* |    GetDescReference alloue un descripteur de reference.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetDescReference (PtrReferredDescr * pDR)

#else  /* __STDC__ */
void                GetDescReference (pDR)
PtrReferredDescr   *pDR;

#endif /* __STDC__ */

{
   PtrReferredDescr    pDe1;


   if (PtFreeDescRef == NULL)
      *pDR = (PtrReferredDescr) TtaGetMemory (sizeof (ReferredElemDescriptor));
   else
     {
	*pDR = PtFreeDescRef;
	PtFreeDescRef = (*pDR)->ReNext;
	NbLibDescRef--;
     }
   NbOccDescRef++;
   pDe1 = *pDR;
   memset (pDe1, 0, sizeof (ReferredElemDescriptor));
   pDe1->ReFirstReference = NULL;
   pDe1->ReExtDocRef = NULL;
   pDe1->RePrevious = NULL;
   pDe1->ReNext = NULL;
   pDe1->ReReferredLabel[0] = '\0';
   pDe1->ReExternalRef = FALSE;
   pDe1->ReReferredElem = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    FreeDescReference libere un descripteur de reference.           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeDescReference (PtrReferredDescr pDR)

#else  /* __STDC__ */
void                FreeDescReference (pDR)
PtrReferredDescr    pDR;

#endif /* __STDC__ */

{

   pDR->ReNext = PtFreeDescRef;
   PtFreeDescRef = pDR;
   NbLibDescRef++;
   NbOccDescRef--;
}

/* ---------------------------------------------------------------------- */
/* |    GetDescCopie alloue un descripteur de copie.                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetDescCopie (PtrCopyDescr * pDC)

#else  /* __STDC__ */
void                GetDescCopie (pDC)
PtrCopyDescr       *pDC;

#endif /* __STDC__ */

{
   PtrCopyDescr        pDe1;

   if (PtFreeDescCopie == NULL)
      *pDC = (PtrCopyDescr) TtaGetMemory (sizeof (CopyDescriptor));
   else
     {
	*pDC = PtFreeDescCopie;
	PtFreeDescCopie = (*pDC)->CdNext;
	NbLibDescCopie--;
     }
   NbOccDescCopie++;
   pDe1 = *pDC;
   memset (pDe1, 0, sizeof (CopyDescriptor));
   pDe1->CdCopiedAb = NULL;
   pDe1->CdCopiedElem = NULL;
   pDe1->CdCopyRule = NULL;
   pDe1->CdPrevious = NULL;
   pDe1->CdNext = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    FreeDescCopie libere un descripteur de copie.                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeDescCopie (PtrCopyDescr pDC)

#else  /* __STDC__ */
void                FreeDescCopie (pDC)
PtrCopyDescr        pDC;

#endif /* __STDC__ */

{
   pDC->CdNext = PtFreeDescCopie;
   PtFreeDescCopie = pDC;
   NbLibDescCopie++;
   NbOccDescCopie--;
}

/* ---------------------------------------------------------------------- */
/* |    GetDocExterne alloue un descripteur de document externe.        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetDocExterne (PtrExternalDoc * pDE)

#else  /* __STDC__ */
void                GetDocExterne (pDE)
PtrExternalDoc      *pDE;

#endif /* __STDC__ */

{
   PtrExternalDoc       pDo1;


   if (PtFreeDocExterne == NULL)
      *pDE = (PtrExternalDoc) TtaGetMemory (sizeof (ExternalDoc));
   else
     {
	*pDE = PtFreeDocExterne;
	PtFreeDocExterne = (*pDE)->EdNext;
	NbLibDocExterne--;
     }
   NbOccDocExterne++;
   pDo1 = *pDE;
   memset (pDo1, 0, sizeof (ExternalDoc));
   pDo1->EdNext = NULL;
   NulIdentDoc (&pDo1->EdDocIdent);
}

/* ---------------------------------------------------------------------- */
/* |    FreeDocExterne libere un descripteur de document externe.       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeDocExterne (PtrExternalDoc pDE)

#else  /* __STDC__ */
void                FreeDocExterne (pDE)
PtrExternalDoc       pDE;

#endif /* __STDC__ */

{

   pDE->EdNext = PtFreeDocExterne;
   PtFreeDocExterne = pDE;
   NbLibDocExterne++;
   NbOccDocExterne--;
}

/* ---------------------------------------------------------------------- */
/* |    GetReference alloue une reference.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetReference (PtrReference * pRef)

#else  /* __STDC__ */
void                GetReference (pRef)
PtrReference       *pRef;

#endif /* __STDC__ */

{
   PtrReference        pPR1;

   if (PtFreeReference == NULL)
      *pRef = (PtrReference) TtaGetMemory (sizeof (ReferenceDescriptor));
   else
     {
	*pRef = PtFreeReference;
	PtFreeReference = (*pRef)->RdNext;
	NbLibReference--;
     }
   NbOccReference++;
   pPR1 = *pRef;
   memset (pPR1, 0, sizeof (ReferenceDescriptor));
   pPR1->RdNext = NULL;
   pPR1->RdPrevious = NULL;
   pPR1->RdReferred = NULL;
   pPR1->RdElement = NULL;
   pPR1->RdAttribute = NULL;
   pPR1->RdTypeRef = RefFollow;
   pPR1->RdInternalRef = TRUE;
}

/* ---------------------------------------------------------------------- */
/* |    FreeReference libere une reference.                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeReference (PtrReference pRef)

#else  /* __STDC__ */
void                FreeReference (pRef)
PtrReference        pRef;

#endif /* __STDC__ */

{

   pRef->RdNext = PtFreeReference;
   PtFreeReference = pRef;
   NbLibReference++;
   NbOccReference--;
}

/* ---------------------------------------------------------------------- */
/* |    GetRefSortante alloue un descripteur de reference sortante.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetRefSortante (PtrOutReference * pRS)

#else  /* __STDC__ */
void                GetRefSortante (pRS)
PtrOutReference     *pRS;

#endif /* __STDC__ */

{
   PtrOutReference      pRs1;

   if (PtFreeRefSortante == NULL)
      *pRS = (PtrOutReference) malloc (sizeof (OutReference));
   else
     {
	*pRS = PtFreeRefSortante;
	PtFreeRefSortante = (*pRS)->OrNext;
	NbLibRefSortante--;
     }
   NbOccRefSortante++;
   pRs1 = *pRS;
   memset (pRs1, 0, sizeof (OutReference));
   pRs1->OrNext = NULL;
   pRs1->OrLabel[0] = '\0';
   NulIdentDoc (&pRs1->OrDocIdent);
}

/* ---------------------------------------------------------------------- */
/* |    FreeRefSortante libere un descripteur de reference sortante.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeRefSortante (PtrOutReference pRS)

#else  /* __STDC__ */
void                FreeRefSortante (pRS)
PtrOutReference      pRS;

#endif /* __STDC__ */

{
   pRS->OrNext = PtFreeRefSortante;
   PtFreeRefSortante = pRS;
   NbLibRefSortante++;
   NbOccRefSortante--;
}

/* ---------------------------------------------------------------------- */
/* |    GetElemRefChng alloue un descripteur de changement de reference.| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetElemRefChng (PtrChangedReferredEl * pER)

#else  /* __STDC__ */
void                GetElemRefChng (pER)
PtrChangedReferredEl     *pER;

#endif /* __STDC__ */

{
   PtrChangedReferredEl      pEr1;

   if (PtFreeElemRefChng == NULL)
      *pER = (PtrChangedReferredEl) malloc (sizeof (ChangedReferredEl));
   else
     {
	*pER = PtFreeElemRefChng;
	PtFreeElemRefChng = (*pER)->CrNext;
	NbLibElemRefChng--;
     }
   NbOccElemRefChng++;
   pEr1 = *pER;
   memset (pEr1, 0, sizeof (ChangedReferredEl));
   pEr1->CrNext = NULL;
   pEr1->CrOldLabel[0] = '\0';
   pEr1->CrNewLabel[0] = '\0';
   NulIdentDoc (&pEr1->CrOldDocument);
   NulIdentDoc (&pEr1->CrNewDocument);
   pEr1->CrReferringDoc = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    FreeElemRefChng libere un descripteur de changement de reference| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeElemRefChng (PtrChangedReferredEl pER)

#else  /* __STDC__ */
void                FreeElemRefChng (pER)
PtrChangedReferredEl      pER;

#endif /* __STDC__ */

{
   pER->CrNext = PtFreeElemRefChng;
   PtFreeElemRefChng = pER;
   NbLibElemRefChng++;
   NbOccElemRefChng--;
}

/* ---------------------------------------------------------------------- */
/* |    GetRefEntrantes alloue un descripteur de reference entrante.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetRefEntrantes (PtrEnteringReferences * pRE)

#else  /* __STDC__ */
void                GetRefEntrantes (pRE)
PtrEnteringReferences    *pRE;

#endif /* __STDC__ */

{
   PtrEnteringReferences     pRe1;

   if (PtFreeRefEntrantes == NULL)
      *pRE = (PtrEnteringReferences) malloc (sizeof (RefEntrantes));
   else
     {
	*pRE = PtFreeRefEntrantes;
	PtFreeRefEntrantes = (*pRE)->ErNext;
	NbLibRefEntrantes--;
     }
   NbOccRefEntrantes++;
   pRe1 = *pRE;
   memset (pRe1, 0, sizeof (RefEntrantes));
   pRe1->ErNext = NULL;
   pRe1->ErFirstReferredEl = NULL;
   NulIdentDoc (&pRe1->ErDocIdent);
   pRe1->ErFileName[0] = '\0';
}

/* ---------------------------------------------------------------------- */
/* |    FreeRefEntrantes libere un descripteur de reference entrante.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeRefEntrantes (PtrEnteringReferences pRE)

#else  /* __STDC__ */
void                FreeRefEntrantes (pRE)
PtrEnteringReferences     pRE;

#endif /* __STDC__ */

{
   pRE->ErNext = PtFreeRefEntrantes;
   PtFreeRefEntrantes = pRE;
   NbLibRefEntrantes++;
   NbOccRefEntrantes--;
}

/* ---------------------------------------------------------------------- */
/* |    GetFichRefChng alloue un descripteur de changement de reference | */
/* |            fichier.                                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetFichRefChng (PtrReferenceChange * pFRC)

#else  /* __STDC__ */
void                GetFichRefChng (pFRC)
PtrReferenceChange     *pFRC;

#endif /* __STDC__ */

{
   PtrReferenceChange      pFrc1;

   if (PtFreeFichRefChng == NULL)
      *pFRC = (PtrReferenceChange) malloc (sizeof (ReferenceChange));
   else
     {
	*pFRC = PtFreeFichRefChng;
	PtFreeFichRefChng = (*pFRC)->RcNext;
	NbLibFichRefChng--;
     }
   NbOccFichRefChng++;
   pFrc1 = *pFRC;
   memset (pFrc1, 0, sizeof (ReferenceChange));
   pFrc1->RcNext = NULL;
   pFrc1->RcFirstChange = NULL;
   NulIdentDoc (&pFrc1->RcDocIdent);
   pFrc1->RcFileName[0] = '\0';
}

/* ---------------------------------------------------------------------- */
/* |    FreeFichRefChng libere un descripteur de changement de reference| */
/* |            fichier                                                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeFichRefChng (PtrReferenceChange pFRC)

#else  /* __STDC__ */
void                FreeFichRefChng (pFRC)
PtrReferenceChange      pFRC;

#endif /* __STDC__ */

{
   pFRC->RcNext = PtFreeFichRefChng;
   PtFreeFichRefChng = pFRC;
   NbLibFichRefChng++;
   NbOccFichRefChng--;
}

/* ---------------------------------------------------------------------- */
/* |    GetPave alloue un pave.                                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetPave (PtrAbstractBox * pAb)

#else  /* __STDC__ */
void                GetPave (pAb)
PtrAbstractBox            *pAb;

#endif /* __STDC__ */

{
   PtrAbstractBox             pPa1;

   if (PtFreePave == NULL)
      *pAb = (PtrAbstractBox) TtaGetMemory (sizeof (AbstractBox));
   else
     {
	*pAb = PtFreePave;
	PtFreePave = (*pAb)->AbNext;
	NbLibPave--;
     }
   NbOccPave++;
   pPa1 = *pAb;
   memset (pPa1, 0, sizeof (AbstractBox));
   pPa1->AbElement = NULL;
   pPa1->AbEnclosing = NULL;
   pPa1->AbPrevious = NULL;
   pPa1->AbNext = NULL;
   pPa1->AbFirstEnclosed = NULL;
   pPa1->AbVolume = 0;
   pPa1->AbNextRepeated = NULL;
   pPa1->AbPreviousRepeated = NULL;
   pPa1->AbCopyDescr = NULL;
   pPa1->AbCreatorAttr = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    FreePave libere un pave.                                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreePave (PtrAbstractBox pAb)

#else  /* __STDC__ */
void                FreePave (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{

   if (pAb->AbLeafType == LtPicture)
     {
	/* FreeImage(pAb->AbBox, pAb->AbBox->BxPictInfo ); */
     }
   pAb->AbElement = NULL;
   pAb->AbEnclosing = NULL;
   pAb->AbPrevious = NULL;
   pAb->AbFirstEnclosed = NULL;
   pAb->AbNextRepeated = NULL;
   pAb->AbPreviousRepeated = NULL;
   pAb->AbNext = PtFreePave;
   PtFreePave = pAb;
   NbLibPave++;
   NbOccPave--;
}

/* ---------------------------------------------------------------------- */
/* |    GetDocument alloue un descripteur de document.                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetDocument (PtrDocument * pDoc)

#else  /* __STDC__ */
void                GetDocument (pDoc)
PtrDocument        *pDoc;

#endif /* __STDC__ */

{
   int                 i;
   PtrDocument         pDo1;

   if (PtFreeDocument == NULL)
      *pDoc = (PtrDocument) TtaGetMemory (sizeof (DocumentDescr));
   else
     {
	*pDoc = PtFreeDocument;
	PtFreeDocument = (*pDoc)->DocNext;
	NbLibDocument--;
     }
   NbOccDocument++;
   /* initialise le contexte de document */
   pDo1 = *pDoc;
   memset (pDo1, 0, sizeof (DocumentDescr));
   pDo1->DocNext = NULL;
   pDo1->DocComment = NULL;
   pDo1->DocSSchema = NULL;
   pDo1->DocRootElement = NULL;
   for (i = 0; i < MAX_ASSOC_DOC; i++)
     {
	pDo1->DocAssocRoot[i] = NULL;
	pDo1->DocAssocSubTree[i] = NULL;
	pDo1->DocAssocFrame[i] = 0;
	pDo1->DocAssocVolume[i] = 0;
	pDo1->DocAssocFreeVolume[i] = 0;
	pDo1->DocAssocNPages[i] = 0;
	pDo1->DocAssocModifiedAb[i] = NULL;
     }
   for (i = 0; i < MAX_PARAM_DOC; i++)
      pDo1->DocParameters[i] = NULL;
   /* cree et initialise un descripteur bidon de reference, debut */
   /* de la chaine des descripteurs de references du document */
   GetDescReference (&pDo1->DocReferredEl);
   for (i = 0; i < MAX_VIEW_DOC; i++)
     {
	pDo1->DocView[i].DvSSchema = NULL;
	pDo1->DocView[i].DvPSchemaView = 0;
	pDo1->DocView[i].DvSync = FALSE;
	pDo1->DocViewRootAb[i] = NULL;
	pDo1->DocViewSubTree[i] = NULL;
	pDo1->DocViewFrame[i] = 0;
	pDo1->DocViewVolume[i] = 0;
	pDo1->DocViewFreeVolume[i] = 0;
	pDo1->DocViewNPages[i] = 0;
	pDo1->DocViewModifiedAb[i] = NULL;
     }
   pDo1->DocDName[0] = '\0';
   NulIdentDoc (&pDo1->DocIdent);
   pDo1->DocDirectory[0] = '\0';
   pDo1->DocSchemasPath[0] = '\0';
   pDo1->DocBackUpInterval = 0;
   pDo1->DocReadOnly = FALSE;
   pDo1->DocExportStructure = FALSE;
   pDo1->DocLabelExpMax = 1;
   pDo1->DocMaxPairIdent = 0;
   pDo1->DocModified = FALSE;
   pDo1->DocNTypedChars = 0;
   pDo1->DocNewOutRef = NULL;
   pDo1->DocDeadOutRef = NULL;
   pDo1->DocChangedReferredEl = NULL;
   pDo1->DocNLanguages = 0;
   pDo1->DocNNatures = 0;
   pDo1->DocPivotVersion = 0;
   pDo1->DocLabels = NULL;
   pDo1->DocToBeChecked = FALSE;
   pDo1->DocPivotError = FALSE;
   pDo1->DocNotifyAll = FALSE;
}

/* ---------------------------------------------------------------------- */
/* |    FreeDocument libere un descripteur de document.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeDocument (PtrDocument pDoc)

#else  /* __STDC__ */
void                FreeDocument (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{

   pDoc->DocNext = PtFreeDocument;
   PtFreeDocument = pDoc;
   NbLibDocument++;
   NbOccDocument--;
}

/* ---------------------------------------------------------------------- */
/* |    GetSchPres alloue un schema de presentation.                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetSchPres (PtrPSchema * pSP)

#else  /* __STDC__ */
void                GetSchPres (pSP)
PtrPSchema         *pSP;

#endif /* __STDC__ */

{
   int                 i;

   if (PtFreeSPres == NULL)
      *pSP = (PtrPSchema) TtaGetMemory (sizeof (PresentSchema));
   else
     {
	*pSP = PtFreeSPres;
	PtFreeSPres = (*pSP)->PsNext;
	NbLibSchPres--;
     }
   memset ((*pSP), 0, sizeof (PresentSchema));
   (*pSP)->PsNext = NULL;
   (*pSP)->PsStructName[0] = '\0';
   (*pSP)->PsPresentName[0] = '\0';
   (*pSP)->PsStructCode = 0;
   (*pSP)->PsNViews = 0;
   for (i = 0; i < MAX_VIEW; i++)
     {
	(*pSP)->PsView[i][0] = '\0';
	(*pSP)->PsPaginatedView[i] = FALSE;
	(*pSP)->PsColumnView[i] = FALSE;
	(*pSP)->PsExportView[i] = FALSE;
     }
   (*pSP)->PsNPrintedViews = 0;
   for (i = 0; i < MAX_PRINT_VIEW; i++)
     {
	(*pSP)->PsPrintedView[i].VpAssoc = FALSE;
	(*pSP)->PsPrintedView[i].VpNumber = 0;
     }
   (*pSP)->PsNCounters = 0;
   (*pSP)->PsNConstants = 0;
   (*pSP)->PsNVariables = 0;
   (*pSP)->PsNPresentBoxes = 0;
   (*pSP)->PsFirstDefaultPRule = NULL;
   for (i = 0; i < MAX_PRES_BOX; i++)
     {
	(*pSP)->PsPresentBox[i].PbFirstPRule = NULL;
     }
   for (i = 0; i < MAX_ATTR_SSCHEMA; i++)
     {
	(*pSP)->PsAttrPRule[i] = NULL;
	(*pSP)->PsNAttrPRule[i] = 0;
	(*pSP)->PsNComparAttrs[i] = 0;
	(*pSP)->PsComparAttr[i] = NULL;
     }
   for (i = 0; i < MAX_RULES_SSCHEMA; i++)
     {
	(*pSP)->PsElemPRule[i] = NULL;
	(*pSP)->PsNInheritedAttrs[i] = 0;
	(*pSP)->PsInheritedAttr[i] = NULL;
	(*pSP)->PsAcceptPageBreak[i] = TRUE;
	(*pSP)->PsAcceptLineBreak[i] = TRUE;
	(*pSP)->PsBuildAll[i] = FALSE;
	(*pSP)->PsNotInLine[i] = FALSE;
	(*pSP)->PsInPageHeaderOrFooter[i] = FALSE;
	(*pSP)->PsAssocPaginated[i] = FALSE;
	(*pSP)->PsElemTransmit[i] = 0;
     }

   (*pSP)->PsNTransmElems = 0;

   NbOccSchPres++;
}

/* ---------------------------------------------------------------------- */
/* |    FreeSPres libere un schemas de presentation.                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeSPres (PtrPSchema pSP)

#else  /* __STDC__ */
void                FreeSPres (pSP)
PtrPSchema          pSP;

#endif /* __STDC__ */

{

   pSP->PsNext = PtFreeSPres;
   PtFreeSPres = pSP;
   NbLibSchPres++;
   NbOccSchPres--;
}

/* ---------------------------------------------------------------------- */
/* |    GetHandleSchPres alloue un element de chainage de schemas de    | */
/* |                     presentation.                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetHandleSchPres (PtrHandlePSchema * pHSP)

#else  /* __STDC__ */
void                GetHandleSchPres (pHSP)
PtrHandlePSchema   *pHSP;

#endif /* __STDC__ */

{
   if (PtFreeHandleSchPres == NULL)
      *pHSP = (PtrHandlePSchema) TtaGetMemory (sizeof (HandlePSchema));
   else
     {
	*pHSP = PtFreeHandleSchPres;
	PtFreeHandleSchPres = (*pHSP)->HdNextPSchema;
	NbLibHandleSchPres--;
     }
   memset ((*pHSP), 0, sizeof (HandlePSchema));
   (*pHSP)->HdPSchema = NULL;
   (*pHSP)->HdNextPSchema = NULL;
   (*pHSP)->HdPrevPSchema = NULL;
   NbOccHandleSchPres++;
}

/* ---------------------------------------------------------------------- */
/* |    FreeHandleSchPres libere un element de chainage de schemas de   | */
/* |                      presentation.                                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeHandleSchPres (PtrHandlePSchema pHSP)

#else  /* __STDC__ */
void                FreeHandleSchPres (pHSP)
PtrHandlePSchema    pHSP;

#endif /* __STDC__ */

{
   pHSP->HdPrevPSchema = NULL;
   pHSP->HdPSchema = NULL;
   pHSP->HdNextPSchema = PtFreeHandleSchPres;
   PtFreeHandleSchPres = pHSP;
   NbLibHandleSchPres++;
   NbOccHandleSchPres--;
}

/* ---------------------------------------------------------------------- */
/* |    GetBlocExtens alloue un bloc d'extension pour un schema de      | */
/* |    structure.                                                      | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                GetBlocExtens (PtrExtensBlock * pBE)

#else  /* __STDC__ */
void                GetBlocExtens (pBE)
PtrExtensBlock      *pBE;

#endif /* __STDC__ */

{

   if (PtFreeBlocExtens == NULL)
      *pBE = (PtrExtensBlock) TtaGetMemory (sizeof (ExtensBlock));
   else
     {
	*pBE = PtFreeBlocExtens;
	PtFreeBlocExtens = (*pBE)->EbNextBlock;
	NbLibBlocExtens--;
     }
   memset (*pBE, 0, sizeof (ExtensBlock));
   (*pBE)->EbNextBlock = NULL;
   NbOccBlocExtens++;
}

/* ---------------------------------------------------------------------- */
/* |    FreeBlocExtens libere un bloc extension de schema de structure. | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
void                FreeBlocExtens (PtrExtensBlock pBE)

#else  /* __STDC__ */
void                FreeBlocExtens (pBE)
PtrExtensBlock       pBE;

#endif /* __STDC__ */

{
   pBE->EbNextBlock = PtFreeBlocExtens;
   PtFreeBlocExtens = pBE;
   NbLibBlocExtens++;
   NbOccBlocExtens--;
}

/* ---------------------------------------------------------------------- */
/* |    GetSchStruct alloue un schema de structure.                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetSchStruct (PtrSSchema * pSS)

#else  /* __STDC__ */
void                GetSchStruct (pSS)
PtrSSchema       *pSS;

#endif /* __STDC__ */

{
   if (PtFreeSStruct == NULL)
      *pSS = (PtrSSchema) TtaGetMemory (sizeof (StructSchema));
   else
     {
	*pSS = PtFreeSStruct;
	PtFreeSStruct = (*pSS)->SsNextExtens;
	NbLibSchStruct--;
     }
   NbOccSchStruct++;
   memset ((*pSS), 0, sizeof (StructSchema));
   (*pSS)->SsNextExtens = NULL;
   (*pSS)->SsPrevExtens = NULL;
   (*pSS)->SsExtension = FALSE;
   (*pSS)->SsNExtensRules = 0;
   (*pSS)->SsExtensBlock = NULL;
   (*pSS)->SsFirstPSchemaExtens = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    FreeSStruct libere un schema de structure.                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeSStruct (PtrSSchema pSS)

#else  /* __STDC__ */
void                FreeSStruct (pSS)
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   if (pSS->SsExtensBlock != NULL)
      FreeBlocExtens (pSS->SsExtensBlock);
   pSS->SsNextExtens = PtFreeSStruct;
   PtFreeSStruct = pSS;
   NbLibSchStruct++;
   NbOccSchStruct--;
}

/* ---------------------------------------------------------------------- */
/* |    GetReglePres alloue une regle de presentation.                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetReglePres (PtrPRule * pRP)

#else  /* __STDC__ */
void                GetReglePres (pRP)
PtrPRule       *pRP;

#endif /* __STDC__ */

{

   if (PtFreeReglePres == NULL)
      *pRP = (PtrPRule) TtaGetMemory (sizeof (PresRule));
   else
     {
	*pRP = PtFreeReglePres;
	PtFreeReglePres = (*pRP)->PrNextPRule;
	NbLibReglePres--;
     }
   NbOccReglePres++;
   memset ((*pRP), 0, sizeof (PresRule));
   (*pRP)->PrNextPRule = NULL;
   (*pRP)->PrSpecifAttr = 0;
   (*pRP)->PrSpecifAttrSSchema = NULL;
   (*pRP)->PrCond = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    FreeReglePres libere une regle de presentation.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeReglePres (PtrPRule pRP)

#else  /* __STDC__ */
void                FreeReglePres (pRP)
PtrPRule        pRP;

#endif /* __STDC__ */

{
   PtrCondition        pCond, nextCond;

   pCond = pRP->PrCond;
   while (pCond != NULL)
     {
	nextCond = pCond->CoNextCondition;
	FreeCondReglePres (pCond);
	pCond = nextCond;
     }
   pRP->PrNextPRule = PtFreeReglePres;
   PtFreeReglePres = pRP;
   NbLibReglePres++;
   NbOccReglePres--;
}

/* ---------------------------------------------------------------------- */
/* |    GetCondReglePres alloue une condition pour une regle de         | */
/* |    presentation.                                                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetCondReglePres (PtrCondition * pCond)

#else  /* __STDC__ */
void                GetCondReglePres (pCond)
PtrCondition       *pCond;

#endif /* __STDC__ */

{
   if (PtFreeCondReglePres == NULL)
      *pCond = (PtrCondition) TtaGetMemory (sizeof (Condition));
   else
     {
	*pCond = PtFreeCondReglePres;
	PtFreeCondReglePres = (*pCond)->CoNextCondition;
	NbLibCondReglePres--;
     }
   NbOccCondReglePres++;
   memset ((*pCond), 0, sizeof (Condition));
   (*pCond)->CoNextCondition = NULL;
   (*pCond)->CoNotNegative = FALSE;
   (*pCond)->CoTarget = FALSE;
}

/* ---------------------------------------------------------------------- */
/* |    FreeCondReglePres libere une condition sur une regle de         | */
/* |    presentation.                                                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeCondReglePres (PtrCondition pCond)

#else  /* __STDC__ */
void                FreeCondReglePres (pCond)
PtrCondition        pCond;

#endif /* __STDC__ */

{
   pCond->CoNextCondition = PtFreeCondReglePres;
   PtFreeCondReglePres = pCond;
   NbLibCondReglePres++;
   NbOccCondReglePres--;
}

/* ---------------------------------------------------------------------- */
/* |    GetRRetard alloue une regle retardee.                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetRRetard (PtrDelayedPRule * pRR)

#else  /* __STDC__ */
void                GetRRetard (pRR)
PtrDelayedPRule    *pRR;

#endif /* __STDC__ */

{
   if (PtFreeRRetard == NULL)
      *pRR = (PtrDelayedPRule) TtaGetMemory (sizeof (DelayedPRule));
   else
     {
	*pRR = PtFreeRRetard;
	PtFreeRRetard = (*pRR)->DpNext;
	NbLibRRetard--;
     }
   NbOccRRetard++;
   memset (*pRR, 0, sizeof (DelayedPRule));
   (*pRR)->DpPRule = NULL;
   (*pRR)->DpPSchema = NULL;
   (*pRR)->DpAbsBox = NULL;
   (*pRR)->DpNext = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    FreeRRetard libere une regle retardee.                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeRRetard (PtrDelayedPRule pRR)

#else  /* __STDC__ */
void                FreeRRetard (pRR)
PtrDelayedPRule     pRR;

#endif /* __STDC__ */

{

   pRR->DpNext = PtFreeRRetard;
   PtFreeRRetard = pRR;
   NbLibRRetard++;
   NbOccRRetard--;
}

/* ---------------------------------------------------------------------- */
/* |    MemInit initialise l'allocation memoire pour le mediateur.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                MemInit ()

#else  /* __STDC__ */
void                MemInit ()
#endif				/* __STDC__ */

{
   int                 i;
   ViewFrame            *pFrame;

   /* Toutes les frames sont libres */
   for (i = 1; i <= MAX_FRAME; i++)
     {
	pFrame = &ViewFrameTable[i - 1];
	pFrame->FrAbstractBox = NULL;
	pFrame->FrXOrg = 0;
	pFrame->FrYOrg = 0;
	pFrame->FrClipXBegin = 0;
	pFrame->FrClipXEnd = 0;
	pFrame->FrClipYBegin = 0;
	pFrame->FrClipYEnd = 0;
     }
   /* Aucune boite allouee et liberee */
   PtFreBox = NULL;
   NbLibBox = 0;
   NbOccBox = 0;
   PtFreBPos = NULL;
   NbLibBPos = 0;
   NbOccBPos = 0;
   PtFreBDim = NULL;
   NbLibBDim = 0;
   NbOccBDim = 0;
   /* Aucune ligne allouee et liberee */
   PtFreeLine = NULL;
   NbLibLine = 0;
   NbOccLine = 0;
   FrameUpdating = FALSE;
   TextInserting = FALSE;

}

/* ---------------------------------------------------------------------- */
/* |     GetBPos alloue un bloc de relations de position.               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetBPos (PtrPosRelations * adbloc)

#else  /* __STDC__ */
void                GetBPos (adbloc)
PtrPosRelations     *adbloc;

#endif /* __STDC__ */

{
   int                 i;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   PtrPosRelations      pTa1;

   if (PtFreBPos == NULL)
      *adbloc = (PtrPosRelations) TtaGetMemory (sizeof (PosRelations));
   /* Sinon recupere le bloc en tete de la chaine des libres */
   else
     {
	*adbloc = PtFreBPos;
	PtFreBPos = (*adbloc)->PosRNext;
	NbLibBPos--;
     }
   NbOccBPos++;
   /* Initialisation du bloc */
   pTa1 = *adbloc;
   memset (pTa1, 0, sizeof (PosRelations));
   pTa1->PosRNext = NULL;

   for (i = 1; i <= MAX_RELAT_POS; i++)
      pTa1->PosRTable[i - 1].ReBox = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    FreeBPos libere le bloc de relations.                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeBPos (PtrPosRelations * adbloc)

#else  /* __STDC__ */
void                FreeBPos (adbloc)
PtrPosRelations     *adbloc;

#endif /* __STDC__ */

{

   /* Insere le bloc en tete de la chaine des libres */
   (*adbloc)->PosRNext = PtFreBPos;
   PtFreBPos = *adbloc;
   NbLibBPos++;
   NbOccBPos--;
}

/* ---------------------------------------------------------------------- */
/* |    GetBDim alloue un bloc de relations de dimension.               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetBDim (PtrDimRelations * adbloc)

#else  /* __STDC__ */
void                GetBDim (adbloc)
PtrDimRelations     *adbloc;

#endif /* __STDC__ */

{
   int                 i;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   PtrDimRelations      pTa1;

   if (PtFreBDim == NULL)
      *adbloc = (PtrDimRelations) TtaGetMemory (sizeof (DimRelations));
   /* Sinon recupere le bloc en tete de la chaine des libres */
   else
     {
	*adbloc = PtFreBDim;
	PtFreBDim = (*adbloc)->DimRNext;
	NbLibBDim--;
     }
   NbOccBDim++;
   /* Initialisation du bloc */
   pTa1 = *adbloc;
   memset (pTa1, 0, sizeof (DimRelations));
   pTa1->DimRNext = NULL;

   for (i = 1; i <= MAX_RELAT_DIM; i++)
     {
	pTa1->DimRTable[i - 1] = NULL;
	pTa1->DimRSame[i - 1] = TRUE;
     }
}

/* ---------------------------------------------------------------------- */
/* |    FreeBDim libere le bloc de relations de dimension.              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeBDim (PtrDimRelations * adbloc)

#else  /* __STDC__ */
void                FreeBDim (adbloc)
PtrDimRelations     *adbloc;

#endif /* __STDC__ */

{

   /* Insere le bloc en tete de la chaine des libres */
   (*adbloc)->DimRNext = PtFreBDim;
   PtFreBDim = *adbloc;
   NbLibBDim++;
   NbOccBDim--;
}

/* ---------------------------------------------------------------------- */
/* |    GetBox Alloue un nouveau contexte de boite pour le pave pAb. | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrBox            GetBox (PtrAbstractBox pAb)

#else  /* __STDC__ */
PtrBox            GetBox (pAb)
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   PtrBox            adboite;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   if (PtFreBox == NULL)
      adboite = (PtrBox) malloc (sizeof (Box));
   /* Sinon recupere le contexte en tete de la chaine des libres */
   else
     {
	adboite = PtFreBox;
	PtFreBox = adboite->BxNexChild;
	NbLibBox--;
     }
   NbOccBox++;

   /* Initialisation de la boite */
   if (adboite != NULL)
     {
	memset (adboite, 0, sizeof (Box));
	adboite->BxAbstractBox = pAb;
	adboite->BxPrevious = NULL;
	adboite->BxNext = NULL;
	adboite->BxType = BoComplete;
	adboite->BxNexChild = NULL;
	adboite->BxXOrg = 0;
	adboite->BxYOrg = 0;
	adboite->BxWidth = 0;
	adboite->BxHeight = 0;
	adboite->BxHorizRef = 0;
	adboite->BxVertRef = 0;
	adboite->BxFont = NULL;
	adboite->BxUnderline = 0;
	adboite->BxThickness = 0;
	adboite->BxMoved = NULL;
	adboite->BxHorizInc = NULL;
	adboite->BxVertInc = NULL;
	adboite->BxNChars = 0;
	adboite->BxHorizEdge = Left;
	adboite->BxVertEdge = Top;
	adboite->BxNSpaces = 0;
	adboite->BxSpaceWidth = 0;
	adboite->BxPosRelations = NULL;
	adboite->BxWidthRelations = NULL;
	adboite->BxHeightRelations = NULL;
	adboite->BxEndOfBloc = 0;
	/* Il n'y a pas de relations hors-structure */
	adboite->BxXOutOfStruct = FALSE;
	adboite->BxYOutOfStruct = FALSE;
	adboite->BxWOutOfStruct = FALSE;
	adboite->BxHOutOfStruct = FALSE;
	/* La boite n'est pas elastique */
	adboite->BxHorizFlex = FALSE;
	adboite->BxVertFlex = FALSE;
	adboite->BxHorizInverted = FALSE;
	adboite->BxVertInverted = FALSE;
	adboite->BxNew = FALSE;
	/* Initialisation du traitement des dimensions minimales */
	adboite->BxContentHeight = FALSE;
	adboite->BxContentWidth = FALSE;
	adboite->BxRuleHeigth = 0;
	adboite->BxRuleWidth = 0;
	adboite->BxBuffer = NULL;
	adboite->BxEndOfBloc = 0;
	adboite->BxNPixels = 0;
	adboite->BxFirstChar = 0;
	adboite->BxXToCompute = FALSE;
	adboite->BxYToCompute = FALSE;
     }
   return adboite;
}

/* ---------------------------------------------------------------------- */
/* |    FreeBox libere le contexte de boite adboite et retourne la boite | */
/* |            suivante.                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrBox            FreeBox (PtrBox adboite)

#else  /* __STDC__ */
PtrBox            FreeBox (adboite)
PtrBox            adboite;

#endif /* __STDC__ */

{
   PtrPosRelations      pPosRel;
   PtrPosRelations      nepos;
   PtrDimRelations      pDimRel;
   PtrDimRelations      nedim;
   PtrBox            pBo1;
   PtrBox            NextBox;

   /* Insere le contexte de boite en tete de la chaine des libres */
   pBo1 = adboite;
   pBo1->BxAbstractBox = NULL;
   pBo1->BxPrevious = NULL;
   pBo1->BxNext = NULL;
   /* On retire l'indicateur de fin de* bloc */
   pBo1->BxEndOfBloc = 0;
   pBo1->BxType = BoComplete;
   NextBox = pBo1->BxNexChild;
   pBo1->BxNexChild = PtFreBox;
   PtFreBox = adboite;
   NbLibBox++;
   NbOccBox--;
   /* On libere les differents blocs attaches a la boite */
   pPosRel = pBo1->BxPosRelations;
   while (pPosRel != NULL)
     {
	nepos = pPosRel->PosRNext;
	FreeBPos (&pPosRel);
	pPosRel = nepos;
     }

   pDimRel = pBo1->BxWidthRelations;
   while (pDimRel != NULL)
     {
	nedim = pDimRel->DimRNext;
	FreeBDim (&pDimRel);
	pDimRel = nedim;
     }

   pDimRel = pBo1->BxHeightRelations;
   while (pDimRel != NULL)
     {
	nedim = pDimRel->DimRNext;
	FreeBDim (&pDimRel);
	pDimRel = nedim;
     }
   return NextBox;
}

/* ---------------------------------------------------------------------- */
/* |    GetLine alloue un contexte de ligne.                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetLine (PtrLine * adligne)

#else  /* __STDC__ */
void                GetLine (adligne)
PtrLine           *adligne;

#endif /* __STDC__ */

{
   PtrLine            pLi1;

   /* Si pas de contexte dans la liste des libres -> acquiert un nouveau */
   if (PtFreeLine == NULL)
      *adligne = (PtrLine) malloc (sizeof (Line));
   /* Sinon recupere le contexte en tete de la chaine des libres */
   else
     {
	*adligne = PtFreeLine;
	PtFreeLine = (*adligne)->LiNext;
	NbLibLine--;
     }
   NbOccLine++;
   /* Initialisation de la ligne */
   if (*adligne != NULL)
     {
	pLi1 = *adligne;
	memset (pLi1, 0, sizeof (Line));
	pLi1->LiPrevious = NULL;
	pLi1->LiNext = NULL;
	pLi1->LiXOrg = 0;
	pLi1->LiYOrg = 0;
	pLi1->LiXMax = 0;
	pLi1->LiMinLength = 0;
	pLi1->LiHeight = 0;
	pLi1->LiHorizRef = 0;
	pLi1->LiNSpaces = 0;
	pLi1->LiSpaceWidth = 0;
	pLi1->LiNPixels = 0;
	pLi1->LiFirstBox = NULL;
	pLi1->LiLastBox = NULL;
	pLi1->LiFirstPiece = NULL;
	pLi1->LiLastPiece = NULL;
	pLi1->LiRealLength = 0;
     }
}

/* ---------------------------------------------------------------------- */
/* |    FreeLine libere le contexte de ligne.                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeLine (PtrLine adligne)

#else  /* __STDC__ */
void                FreeLine (adligne)
PtrLine            adligne;

#endif /* __STDC__ */

{

   /* Insere le contexte de ligne en tete de la chaine des libres */
   adligne->LiNext = PtFreeLine;
   PtFreeLine = adligne;
   PtFreeLine->LiPrevious = NULL;
   NbLibLine++;
   NbOccLine--;
}


/* ---------------------------------------------------------------------- */
/* | GetSearchContext alloue un contexte de recherche.                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetSearchContext (PtrSearchContext * pSearch)

#else  /* __STDC__ */
void                GetSearchContext (pSearch)
PtrSearchContext          *pSearch;

#endif /* __STDC__ */

{
   PtrSearchContext           s;

   if (pSearch != NULL)
     {
	s = (PtrSearchContext) malloc (sizeof (SearchContext));
	/* Initialisation des champs */
	*pSearch = s;
	memset (s, 0, sizeof (SearchContext));
	s->SDocument = NULL;
	s->STree = 0;
	s->SStartElement = NULL;
	s->SStartChar = 0;
	s->SEndElement = NULL;
	s->SEndChar = 0;
	s->SStartToEnd = TRUE;
	s->SWholeDocument = FALSE;
     }
}				/*GetSearchContext */

/* ---------------------------------------------------------------------- */
/* | FreeSearchContext libe`re un contexte de recherche.                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FreeSearchContext (PtrSearchContext * pSearch)

#else  /* __STDC__ */
void                FreeSearchContext (pSearch)
PtrSearchContext          *pSearch;

#endif /* __STDC__ */

{
   if (pSearch != NULL)
      if (*pSearch != NULL)
	{
	   free (*pSearch);
	   *pSearch = NULL;
	}
}				/*FreeSearchContext */


/* ---------------------------------------------------------------------- */
/* | FreeChaine libere les entre'es d'un dictionnaire.                  | */
/* ---------------------------------------------------------------------- */
/*CORR */

#ifdef __STDC__
void                FreeChaine (PtrDico pDict)

#else  /* __STDC__ */
void                FreeChaine (pDict)
PtrDico             pDict;

#endif /* __STDC__ */

{
/*CORR */ PtrChaine pCh1;
/*CORR */ PtrCommuns pCommun;
/*CORR */ PtrMots   pMot;
/*CORR */ int       i;

/*CORR */ pCh1 = pDict->chaine;
/*CORR */ if (pCh1 != NULL)
/*CORR */ free (pCh1);

/*CORR */ pMot = pDict->pdico;
/*CORR */ if (pMot != NULL)
/*CORR */ free (pMot);

/*CORR */ pCommun = pDict->commun;
/*CORR */ if (pCommun != NULL)
/*CORR */ free (pCommun);

   /*CORR *//* maj du contexte du dictionnaire : chaine et mots */
/*CORR */ pDict->nbcars = 0;
/*CORR */ pDict->chaine = NULL;
/*CORR */ pDict->nbmots = -1;
/*CORR */ pDict->pdico = NULL;
/*CORR */ pDict->commun = NULL;
/*CORR */ for (i = 0; i < MAX_WORD_LEN; i++)
/*CORR */ pDict->plgdico[i] = 0;
/*CORR */ 
}


/* ---------------------------------------------------------------------- */
/* | GetChaine retourne -1 en cas de manque de memoire                  | */
/* |            retourne  0 si OK.                                      | */
/* ---------------------------------------------------------------------- */
/*CORR */

#ifdef __STDC__
int                 GetChaine (PtrDico * pDict, boolean readonly)

#else  /* __STDC__ */
int                 GetChaine (pDict, readonly)
PtrDico            *pDict;
boolean             readonly;

#endif /* __STDC__ */

{
/*CORR */ PtrDico   pdict;
/*CORR */ unsigned int i;

/*CORR */ pdict = *pDict;
/*CORR */ pdict->MAXcars += (pdict->DicoReadOnly == FALSE) ? 600 : 2;
/*CORR */ i = pdict->MAXcars;
   /*CORR *//* alloue la chaine necessaire */
/*CORR */ pdict->chaine = (PtrChaine) DicoAlloc (i);
/*CORR */ if (pdict->chaine == NULL)
/*CORR */ 
     {
/*CORR */ FreeChaine (pdict);
/*CORR */ return (-1);
/*CORR */ 
     }

/*CORR */ pdict->MAXmots += (pdict->DicoReadOnly == FALSE) ? 50 : 2;
/*CORR */ i = pdict->MAXmots;
/*CORR */ pdict->commun = (PtrCommuns) DicoAlloc (i);
/*CORR */ if (pdict->commun == NULL)
/*CORR */ 
     {
/*CORR */ FreeChaine (pdict);
/*CORR */ return (-1);
/*CORR */ 
     }

   /*CORR *//* ATTENTION : ce sont des entiers */
/*CORR */ pdict->pdico = (PtrMots) DicoAlloc (i * sizeof (int));

/*CORR */ if (pdict->pdico == NULL)
/*CORR */ 
     {
/*CORR */ FreeChaine (pdict);
/*CORR */ return (-1);
/*CORR */ 
     }

/*CORR */ return (0);
/*CORR */ 
}


/* ---------------------------------------------------------------------- */
/* | GetDico retourne dans pDico un pointeur vers le dictionaire alloue | */
/* | ou NULL si manque de memoire                                       | */
/* ---------------------------------------------------------------------- */
/*CORR */

#ifdef __STDC__
void                GetDico (PtrDico * pDico)

#else  /* __STDC__ */
void                GetDico (pDico)
PtrDico            *pDico;

#endif /* __STDC__ */

{
/*CORR */ int       i;
/*CORR */ PtrDico   pdict;

/*CORR */ if (PtFreeDico == NULL)
/*CORR */ *pDico = (PtrDico) DicoAlloc (sizeof (Dictionnaire));
/*CORR */ 
   else
/*CORR */ 
     {
/*CORR */ *pDico = PtFreeDico;
/*CORR */ PtFreeDico = (*pDico)->DicoSuivant;
/*CORR */ NbLibDico--;
/*CORR */ 
     }
/*CORR */ if (*pDico != NULL)
/*CORR */ 
     {
/*CORR */ NbOccDico++;
/*CORR */
	/*CORR *//* initialise le contexte de dictionnaire */
/*CORR */ pdict = *pDico;

/*CORR */ pdict->DicoNom[0] = '\0';
/*CORR */ pdict->DicoDirectory[0] = '\0';
						/*CORR */ pdict->DicoReadOnly = TRUE;
						/* readonly */
						/*CORR */ pdict->DicoCharge = FALSE;
						/* contenu non charge' */
						/*CORR */ pdict->DicoModifie = FALSE;
						/* contenu non modifie' */
/*CORR */ pdict->chaine = NULL;
/*CORR */ pdict->pdico = NULL;
/*CORR */ pdict->commun = NULL;

/*CORR */ for (i = 0; i < MAX_WORD_LEN; i++)
/*CORR */ pdict->plgdico[i] = 0;
/*CORR */ pdict->MAXmots = 0;
/*CORR */ pdict->MAXcars = 0;
/*CORR */ pdict->nbmots = -1;
/*CORR */ pdict->nbcars = 0;
/*CORR */ 
     }
/*CORR */ 
}


/* ---------------------------------------------------------------------- */
/* | FreeDico libere le dictionnaire.                                   | */
/* ---------------------------------------------------------------------- */
/*CORR */

#ifdef __STDC__
void                FreeDico (PtrDico pDico)

#else  /* __STDC__ */
void                FreeDico (pDico)
PtrDico             pDico;

#endif /* __STDC__ */

{
/*CORR */ pDico->DicoSuivant = PtFreeDico;
/*CORR */ PtFreeDico = pDico;
/*CORR */ NbLibDico++;
/*CORR */ NbOccDico--;
/*CORR */ 
}
/* End Of Module memory */
