/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   docvues.c : manipulation des documents et des vues.
   Ce module effectue le demarrage de l'editeur.
   Il contient la boucle principale et effectue la
   gestion des documents et des vues.
   V. Quint     Avril 1985
   IV : Mai 92   adaptation Tool Kit
 */


#include "thot_gui.h"
#include "thot_sys.h"

#include "constmenu.h"
#include "libmsg.h"
#include "message.h"
#include "functions.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"
#include "appdialogue.h"
#include "thotfile.h"
#include "thotdir.h"

#undef EXPORT
#define EXPORT extern
#include "platform_tv.h"
#include "modif_tv.h"
#include "page_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "appli_f.h"
#include "tree_f.h"
#include "attributes_f.h"
#include "search_f.h"
#include "config_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "draw_f.h"
#include "dofile_f.h"
#include "callback_f.h"
#include "viewcommands_f.h"
#include "platform_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "structmodif_f.h"
#include "structcommands_f.h"
#include "memory_f.h"
#include "changeabsbox_f.h"
#include "ouvre_f.h"
#include "paginate_f.h"
#include "writepivot_f.h"
#include "schemas_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "fileaccess_f.h"
#include "references_f.h"
#include "structschema_f.h"
#include "presvariables_f.h"
#include "appdialogue_f.h"


static AvailableView    ToutesLesVuesAOuvrir;
static int          EntreesMenuVues[MAX_VIEW_OPEN];

#ifdef __STDC__
extern void         DisplayFrame (int);

#else
extern void         DisplayFrame ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    docModify positionne le flag modification d'un document a` TRUE.| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                docModify (PtrDocument doc, PtrElement pEl)
#else  /* __STDC__ */
void                docModify (doc, pEl)
PtrDocument         doc;
PtrElement          pEl;

#endif /* __STDC__ */
{
   if (doc != NULL)
     {
	doc->DocModified = TRUE;
	doc->DocNTypedChars += 10;
     }
}

/* ---------------------------------------------------------------------- */
/* |    docUnmodify positionne le flag modification d'un document a`    | */
/* |            FALSE.                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         docUnmodify (PtrDocument doc)
#else  /* __STDC__ */
static void         docUnmodify (doc)
PtrDocument         doc;

#endif /* __STDC__ */
{
   if (doc != NULL)
     {
	doc->DocModified = FALSE;
	doc->DocNTypedChars = 0;
     }
}


/* ---------------------------------------------------------------------- */
/* |    VueAvecPage rend vrai si la vue Vue du document pDoc est une vue avec page | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             VueAvecPage (PtrDocument pDoc, int vue, boolean assoc)

#else  /* __STDC__ */
boolean             VueAvecPage (pDoc, vue, assoc)
PtrDocument         pDoc;
int                 vue;
boolean             assoc;

#endif /* __STDC__ */

{
   boolean             paginer;
   PtrElement          ElAssoc;

   if (assoc)
     {
	ElAssoc = pDoc->DocAssocRoot[vue - 1];
	if (ElAssoc == NULL)
	   paginer = FALSE;
	else
	   paginer = ElAssoc->ElStructSchema->SsPSchema->
	      PsAssocPaginated[ElAssoc->ElTypeNumber - 1];
     }
   else
     {
	if (pDoc->DocView[vue - 1].DvSSchema != pDoc->DocSSchema)
	   /* ce n'est pas une vue definie par le schema du document, */
	   /* on ne la pagine pas */
	   paginer = FALSE;
	else
	   paginer = pDoc->DocView[vue - 1].DvSSchema->SsPSchema->
	      PsPaginatedView[pDoc->DocView[vue - 1].DvPSchemaView - 1];
     }
   return paginer;
}				/* VueAvecPage */


/* ---------------------------------------------------------------------- */
/* |    VueFen retourne le pointeur sur le numero de vue (nv)           | */
/* |            dans le document pD, correspondant a`                   | */
/* |            la fenetre de numero nframe. Si c'est une frame         | */
/* |            d'elements associes, rend assoc vrai et nv = numero     | */
/* |            d'element associe, sinon rend assoc faux.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                VueFen (int nframe, PtrDocument pD, int *nv, boolean * assoc)
#else  /* __STDC__ */
void                VueFen (nframe, pD, nv, assoc)
int                 nframe;
PtrDocument         pD;
int                *nv;
boolean            *assoc;

#endif /* __STDC__ */
{
   int                 vue;

   *nv = 0;
   vue = 0;
   *assoc = FALSE;
   /* cherche d'abord dans les vues de l'arbre principal */
   do
     {
	if (pD->DocView[vue].DvPSchemaView > 0 && pD->DocViewFrame[vue] == nframe)
	   *nv = vue + 1;
	else
	   vue++;
     }
   while (vue < MAX_VIEW_DOC && *nv == 0);

   if (vue >= MAX_VIEW_DOC)
      /* cherche dans les arbres des elements associes */
     {
	vue = 0;
	do
	  {
	     if (pD->DocAssocFrame[vue] == nframe)
	       {
		  *nv = vue + 1;
		  *assoc = TRUE;
	       }
	     else
		vue++;
	  }
	while (vue < MAX_ASSOC_DOC && *nv == 0);
     }
}				/* VueFen */


/* ---------------------------------------------------------------------- */
/* |    DocVueFen retourne le pointeur sur le document (pD) et le       | */
/* |            numero de vue (nv) dans ce document, correspondant a`   | */
/* |            la fenetre de numero nframe. Si c'est une frame         | */
/* |            d'elements associes, rend assoc vrai et nv = numero     | */
/* |            d'element associe, sinon rend assoc faux.               | */
/* |            Rend pD = NULL si la selection a echoue.                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DocVueFen (int frame, PtrDocument * pD, int *nv, boolean * assoc)
#else  /* __STDC__ */
void                DocVueFen (frame, pD, nv, assoc)
int                 nframe;
PtrDocument        *pD;
int                *nv;
boolean            *assoc;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;

   *pD = NULL;			/* on n'a pas encore trouve... */
   *nv = 0;
   if (FrameTable[frame].FrDoc != 0)
     {
	pDoc = LoadedDocument[FrameTable[frame].FrDoc - 1];
	if (pDoc != NULL)
	  {
	     /* il y a un document pour cette entree de la table des documents */
	     VueFen (frame, pDoc, nv, assoc);
	  }
     }
   if (*nv == 0)
      TtaDisplaySimpleMessage (INFO, LIB, MISSING_VIEW);
   else
      *pD = pDoc;
}


/* ---------------------------------------------------------------------- */
/* |    LesVuesDunSchStr trouve toutes les vues creees d'un document.   | */
/* |            On cherche les vues declarees dans les schemas de       | */
/* |            presentation des **todo: revoir.                        | */
/* |            Pour les schemas de presentation des natures contenues  | */
/* |            dans le document, on ne met pas la vue 1, qui est       | */
/* |            integree a` la vue 1 du document.                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         LesVuesDunSchStr (PtrDocument pDoc, PtrSSchema pS, AvailableView LesVues, int *nbvues, boolean Nature)
#else  /* __STDC__ */
static void         LesVuesDunSchStr (pDoc, pS, LesVues, nbvues, Nature)
PtrDocument         pDoc;
PtrSSchema        pS;
AvailableView           LesVues;
int                *nbvues;
boolean             Nature;

#endif /* __STDC__ */
{
   int                 v, w, vuedocu;
   boolean             ouverte, vuepossible, dejala;
   PtrPSchema          pSc1;
   DocViewDescr         *pDe1;
   SRule              *pRe1;

   if (pS != NULL)
     {
	pRe1 = &pS->SsRule[pS->SsRootElem - 1];
	if (!pRe1->SrRefImportedDoc || pS->SsExtension)
	  {
	     pSc1 = pS->SsPSchema;
	     if (Nature || pS->SsExtension)
		v = 2;
	     else
		v = 1;
	     while (v <= pSc1->PsNViews)
	       {
		  if (pDoc->DocExportStructure)
		    {
		       /* exportables, on ne propose que la vue EXPORT */
		       vuepossible = pSc1->PsExportView[v - 1];
		    }
		  else
		    {
		       /* document charge' completement, on ne propose pas la  vue EXPORT */
		       vuepossible = !pSc1->PsExportView[v - 1];
		    }
		  if (vuepossible)
		    {
		       /* met le nom de la vue possible dans le catalogue, s'il n'y est pas deja */
		       /* on verifie que la vue n'est pas deja dans le catalogue */
		       dejala = FALSE;
		       w = 0;
		       while (w < *nbvues && !dejala)
			 {
			    w++;
			    dejala = (LesVues[w - 1].VdView == v
				      && LesVues[w - 1].VdSSchema->SsCode == pS->SsCode);
			 }
		       if (!dejala)
			 {
			    /* cette vue n'est pas dans le catalogue, on la met */
			    (*nbvues)++;
			    LesVues[*nbvues - 1].VdView = v;
			    LesVues[*nbvues - 1].VdAssocNum = 0;
			    strncpy (LesVues[*nbvues - 1].VdViewName, pSc1->PsView[v - 1], MAX_NAME_LENGTH);
			    LesVues[*nbvues - 1].VdSSchema = pS;
			    LesVues[*nbvues - 1].VdAssoc = FALSE;
			    LesVues[*nbvues - 1].VdExist = FALSE;
			    LesVues[*nbvues - 1].VdNature = Nature;
			    LesVues[*nbvues - 1].VdPaginated = pSc1->PsPaginatedView[v - 1];
			    ouverte = FALSE;
			    vuedocu = 1;
			    while (!ouverte && vuedocu <= MAX_VIEW_DOC)
			      {
				 pDe1 = &pDoc->DocView[vuedocu - 1];
				 if (pDe1->DvPSchemaView > 0)
				    if (pDe1->DvSSchema->SsPSchema == pS->SsPSchema
					&& pDe1->DvPSchemaView == v)
				       ouverte = TRUE;	/* vue deja ouverte */
				 vuedocu++;
			      }
			    LesVues[*nbvues - 1].VdOpen = ouverte;

			 }
		    }
		  v++;
	       }
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    LesVuesDesNatures retourne dans LesVues les descripteurs de vue | */
/* |             des natures.                                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         LesVuesDesNatures (PtrSSchema pSS, int *nbvues, AvailableView LesVues, PtrDocument pDoc)

#else  /* __STDC__ */
static void         LesVuesDesNatures (pSS, nbvues, LesVues, pDoc)
PtrSSchema        pSS;
int                *nbvues;
AvailableView           LesVues;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int         r;
   PtrSSchema        pSc1;
   SRule              *pRe1;

   pSc1 = pSS;
   /* parcourt les regles de structure */
   for (r = 1; r <= pSc1->SsNRules; r++)
     {
	pRe1 = &pSc1->SsRule[r - 1];
	if (pRe1->SrConstruct == CsNatureSchema)
	   /* c'est une regle de nature */
	   if (pRe1->SrSSchemaNat != NULL)
	      /* la nature est chargee */
	      if (pRe1->SrSSchemaNat->SsNObjects > 0)
		{
		   /* Il existe au moins un objet de cette nature dans le document */

		   /* les vues non principales de cette nature */
		   LesVuesDunSchStr (pDoc, pRe1->SrSSchemaNat, LesVues, nbvues, TRUE);

		   /* les vues des natures contenues dans cette nature */
		   LesVuesDesNatures (pRe1->SrSSchemaNat, nbvues, LesVues, pDoc);
		}
     }
}

/* ---------------------------------------------------------------------- */
/* |    LesVuesDunDoc cherche toutes les vues possibles d'un document:  | */
/* |            - celles de l'arbre principal.                          | */
/* |            - celles des natures.                                   | */
/* |            - celles des elements assoocies.                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 LesVuesDunDoc (PtrDocument pDoc, AvailableView LesVues)

#else  /* __STDC__ */
int                 LesVuesDunDoc (pDoc, LesVues)
PtrDocument         pDoc;
AvailableView           LesVues;

#endif /* __STDC__ */

{
   int                 a, r, nbvues;
   boolean             assocexiste, existe;
   PtrSSchema        pSc1;
   SRule              *pRe1;

   nbvues = 0;
   if (pDoc->DocSSchema != NULL)
     {
	/* Les vues du schema de presentation de l'arbre principal */
	LesVuesDunSchStr (pDoc, pDoc->DocSSchema, LesVues, &nbvues, FALSE);
	/* Les vues definies pour les extension du schema du document */
	pSc1 = pDoc->DocSSchema->SsNextExtens;
	while (pSc1 != NULL)
	  {
	     LesVuesDunSchStr (pDoc, pSc1, LesVues, &nbvues, FALSE);
	     pSc1 = pSc1->SsNextExtens;
	  }

	/* Les vues des natures contenues dans le document */
	LesVuesDesNatures (pDoc->DocSSchema, &nbvues, LesVues, pDoc);

	/* Les vues des natures contenues dans les natures contenues dans le */
	/* document */
	pSc1 = pDoc->DocSSchema;
	/* parcourt les regles de structure du schema du document */
	for (r = 1; r <= pSc1->SsNRules; r++)
	  {
	     pRe1 = &pSc1->SsRule[r - 1];
	     if (pRe1->SrConstruct == CsNatureSchema)
		/* c'est une regle de nature */
		if (pRe1->SrSSchemaNat != NULL)
		   /* la nature est chargee */
		   if (pRe1->SrSSchemaNat->SsNObjects > 0)
		      /* il existe au moins */
		      /* un objet de cette nature dans le document */
		      LesVuesDunSchStr (pDoc, pRe1->SrSSchemaNat, LesVues,
					&nbvues, TRUE);
	  }

	/* Les vues des elements associes du document. */
	/* On prend d'abord les elements associes definis dans le schema de */
	/* structure du document, puis ceux definis dans les extensions de */
	/* ce schema */
	pSc1 = pDoc->DocSSchema;
	do
	  {
	     if (pSc1->SsFirstDynNature == 0)
		/* r: derniere regle qui pourrait etre une liste d'elem. assoc. */
		r = pSc1->SsNRules;
	     else
		r = pSc1->SsFirstDynNature - 1;
	     /* boucle sur les regles a la recherche des listes d'elements */
	     /* associes definis dans le schema */
	     do
	       {
		  if (pSc1->SsRule[r - 1].SrConstruct == CsList)
		     if (pSc1->SsRule[pSc1->SsRule[r - 1].SrListItem - 1].
			 SrAssocElem)
			/* c'est une regle liste d'elements associes */
			if (!pSc1->SsPSchema->
			    PsInPageHeaderOrFooter[pSc1->SsRule[r - 1].SrListItem - 1])
			   /* ces elements associes ne sont pas affiches dans une */
			   /* boite de haut ou de bas de page */
			  {
			     /* cherche s'il existe dans le document des elements */
			     /* associes de ce type et si leur vue est deja ouverte */
			     assocexiste = FALSE;
			     existe = FALSE;
			     a = 0;
			     do
			       {
				  a++;
				  if (pDoc->DocAssocRoot[a - 1] != NULL)
				     if (pDoc->DocAssocRoot[a - 1]->ElTypeNumber == r)
					if (pDoc->DocAssocRoot[a - 1]->ElStructSchema == pSc1)
					   /* il existe des elements associes de ce type */
					  {
					     assocexiste = TRUE;
					     if (pDoc->DocAssocFrame[a - 1] != 0)
						/* la vue est ouverte */
						existe = TRUE;
					  }
			       }
			     while (!(assocexiste || a >= MAX_ASSOC_DOC));

			     nbvues++;
			     if (existe)
				LesVues[nbvues - 1].VdView = a;
			     else
				LesVues[nbvues - 1].VdView = 0;
			     LesVues[nbvues - 1].VdAssocNum = r;
			     strncpy (LesVues[nbvues - 1].VdViewName,
				   pSc1->SsRule[r - 1].SrName, MAX_NAME_LENGTH);
			     LesVues[nbvues - 1].VdSSchema = pSc1;
			     LesVues[nbvues - 1].VdOpen = existe;
			     LesVues[nbvues - 1].VdAssoc = TRUE;
			     LesVues[nbvues - 1].VdExist = assocexiste;
			     LesVues[nbvues - 1].VdNature = FALSE;
			     LesVues[nbvues - 1].VdPaginated =
				pSc1->SsPSchema->PsAssocPaginated[r - 1];
			  }
		  r--;
		  /* regle precedente */
	       }
	     while (r > 1);
	     /* passe a l'extension de schema suivante */
	     pSc1 = pSc1->SsNextExtens;
	  }
	while (pSc1 != NULL);
     }
   return nbvues;
}

/* ---------------------------------------------------------------------- */
/* |    PaginerDoc pagine toutes les vues creees d'un document.         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                PaginerDoc (PtrDocument pDoc)

#else  /* __STDC__ */
void                PaginerDoc (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 i, nbvues, vuedoc;
   AvailableView           ToutesLesVues;
   boolean             trouve;

   nbvues = LesVuesDunDoc (pDoc, ToutesLesVues);
   for (i = 1; i <= nbvues; i++)
      if (ToutesLesVues[i - 1].VdOpen)
	 if (ToutesLesVues[i - 1].VdPaginated)
	    if (!ToutesLesVues[i - 1].VdNature)
	       /* c'est une vue a paginer */
	      {
		 if (ToutesLesVues[i - 1].VdAssoc)
		    /* c'est une vue d'elements associes, */
		    /* on prend le numero d'el. assoc. */
		    vuedoc = ToutesLesVues[i - 1].VdView;
		 else
		   {
		      /* cherche pour cette vue son */
		      /* numero (vuedoc) dans le document */
		      vuedoc = 1;
		      trouve = FALSE;
		      while (!trouve && vuedoc <= MAX_VIEW_DOC)
			{
			   if (pDoc->DocView[vuedoc - 1].DvPSchemaView ==
			       ToutesLesVues[i - 1].VdView
			       && pDoc->DocView[vuedoc - 1].DvSSchema ==
			       ToutesLesVues[i - 1].VdSSchema)
			      trouve = TRUE;
			   else
			      vuedoc++;
			}
		   }
		 /* pagine cette vue */
		 Pages (pDoc, vuedoc, ToutesLesVues[i - 1].VdAssoc);
	      }
}

/* ---------------------------------------------------------------------- */
/* |    LibDocument libere le document dont le descripteur est pointe'  | */
/* |            par pDoc.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                LibDocument (PtrDocument * pDoc)
#else  /* __STDC__ */
void                LibDocument (pDoc)
PtrDocument        *pDoc;

#endif /* __STDC__ */
{
   int                 d;

   if (*pDoc != NULL)
      /* cherche dans la table le descripteur de document a liberer */
     {
	d = 1;
	while (LoadedDocument[d - 1] != *pDoc && d < MAX_DOCUMENTS)
	   d++;
	if (LoadedDocument[d - 1] == *pDoc)
	   /* supprime la selection dans ce document */
	  {
	     /* fait disparaitre les menus et formulaires concernant le */
	     /* document et qui sont encore a l'ecran */
	     /* enleve la selection de ce document */
	     DeSelDoc (*pDoc);
	     /* libere le contenu du buffer s'il s'agit d'une partie de ce docum. */
	     if (DocOfSavedElements == *pDoc)
		LibBufEditeur ();
	     /* liberer tout l'arbre interne */
	     DeleteAllTrees (*pDoc);
	     /* liberer les schemas : document, natures, presentations */
	     LibSchemas (*pDoc);
	     FreeDocument (LoadedDocument[d - 1]);
	     LoadedDocument[d - 1] = NULL;
	     *pDoc = NULL;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    dest1vue libere les paves et le contexte de la vue v du         | */
/* |            document pointe par pDoc.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                dest1vue (PtrDocument pDoc, DocViewNumber v)
#else  /* __STDC__ */
void                dest1vue (pDoc, v)
PtrDocument         pDoc;
DocViewNumber           v;

#endif /* __STDC__ */
{
   if (pDoc->DocViewRootAb[v - 1] != NULL)
      LibAbbView (pDoc->DocViewRootAb[v - 1]);
   pDoc->DocViewRootAb[v - 1] = NULL;
   pDoc->DocView[v - 1].DvSSchema = NULL;
   pDoc->DocView[v - 1].DvPSchemaView = 0;
   pDoc->DocView[v - 1].DvSync = FALSE;

   pDoc->DocViewFrame[v - 1] = 0;
   pDoc->DocViewVolume[v - 1] = 0;
   pDoc->DocViewFreeVolume[v - 1] = 0;
   pDoc->DocViewSubTree[v - 1] = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    NbVueExiste retourne le nombre de vues qui existent pour le     | */
/* |            document pointe par pDoc.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 NbVueExiste (PtrDocument pDoc)
#else  /* __STDC__ */
int                 NbVueExiste (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 nv, a, nvues;

   nvues = 0;
   /* compte les vues de l'arbre principal */
   for (nv = 1; nv <= MAX_VIEW_DOC; nv++)
      if (pDoc->DocView[nv - 1].DvPSchemaView > 0)
	 nvues++;
   /* compte les vues des elements associes */
   for (a = 1; a <= MAX_ASSOC_DOC; a++)
      if (pDoc->DocAssocFrame[a - 1] > 0)
	 nvues++;

   return nvues;
}

/* ---------------------------------------------------------------------- */
/* |    detruit detruit la vue de numero vue (si assoc est faux) pour   | */
/* |            le document pointe' par pDoc. S'il s'agit de la derniere| */
/* |            vue, libere le document dans le cas seulement ou        | */
/* |            AvecFermeDoc est vrai. Si assoc est vrai, detruit la    | */
/* |            vue des elements associes de numero vue du document.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                detruit (PtrDocument pDoc, int vue, boolean assoc, boolean AvecFermeDoc)
#else  /* __STDC__ */
void                detruit (pDoc, vue, assoc, AvecFermeDoc)
PtrDocument         pDoc;
int                 vue;
boolean             assoc;
boolean             AvecFermeDoc;

#endif /* __STDC__ */
{
   NotifyDialog        notifyDoc;

   if (pDoc != NULL)
      /* on detruit la vue */
     {
	if (!assoc)
	   dest1vue (pDoc, vue);
	else
	  {
	     LibAbbView (pDoc->DocAssocRoot[vue - 1]->ElAbstractBox[0]);
	     pDoc->DocAssocFrame[vue - 1] = 0;
	  }
	if (AvecFermeDoc)
	   /* verifie qu'il reste au moins une vue pour ce document */
	   if (NbVueExiste (pDoc) < 1)
	      /* il ne reste plus de vue, on libere le document */
	     {
		notifyDoc.event = TteDocClose;
		notifyDoc.document = (Document) IdentDocument (pDoc);
		notifyDoc.view = 0;
		if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
		  {
		     if (ThotLocalActions[T_corrector] != NULL)
			(*ThotLocalActions[T_rscorrector]) (-1, 0, (char *) pDoc);
		     notifyDoc.event = TteDocClose;
		     notifyDoc.document = (Document) IdentDocument (pDoc);
		     notifyDoc.view = 0;
		     CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		     LibDocument (&pDoc);
		  }
	     }
     }
}


/* ---------------------------------------------------------------------- */
/* |    MajPavesAccessMode met a` jour le mode d'acces accessMode sur   | */
/* |            le pave pAb et tous ses descendants.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         MajPavesAccessMode (PtrAbstractBox pAb, int accessMode)
#else  /* __STDC__ */
static void         MajPavesAccessMode (pAb, accessMode)
PtrAbstractBox             pAb;
int                 accessMode;

#endif /* __STDC__ */
{
   PtrAbstractBox             PavFils;

   if (accessMode == 0)		/* read only */
     {
	pAb->AbCanBeModified = FALSE;	/* non modifiable */
	pAb->AbReadOnly = TRUE;
	/* le pave a change (pour que le mediateur reevalue les regles) */
	pAb->AbChange = TRUE;
     }
   else
      /* read write */
      /* on laisse en read only si l'element est en read only */ if (!(ElementIsReadOnly (pAb->AbElement)))
     {
	if (!pAb->AbPresentationBox)
	   /* ce n'est pas un pave de presentation, il est donc modifiable */
	   pAb->AbCanBeModified = TRUE;
	else if (PavPresentModifiable (pAb))
	   pAb->AbCanBeModified = TRUE;
	pAb->AbReadOnly = FALSE;
	/* le pave a change (pour que le mediateur reevalue les regles) */
	pAb->AbChange = TRUE;
     }
   /* on passe aux fils */
   PavFils = pAb->AbFirstEnclosed;
   while (PavFils != NULL)
     {
	MajPavesAccessMode (PavFils, accessMode);
	PavFils = PavFils->AbNext;
     }
}
/* ---------------------------------------------------------------------- */
/* |    MajAccessMode met a` jour le mode d'acces sur tout les pave's   | */
/* |          de tous les elements de toutes les vues du document pDoc. | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MajAccessMode (PtrDocument pDoc, int accessMode)
#else  /* __STDC__ */
void                MajAccessMode (pDoc, accessMode)
PtrDocument         pDoc;
int                 accessMode;

#endif /* __STDC__ */
{
   int                 nv, a;

   /* met a jour les vues de l'arbre principal */
   for (nv = 1; nv <= MAX_VIEW_DOC; nv++)
      if (pDoc->DocView[nv - 1].DvPSchemaView > 0)
	{
	   MajPavesAccessMode (pDoc->DocRootElement->ElAbstractBox[nv - 1], accessMode);
	   pDoc->DocViewModifiedAb[nv - 1] = pDoc->DocRootElement->ElAbstractBox[nv - 1];
	}
   /* met a jour es vues des elements associes */
   for (a = 1; a <= MAX_ASSOC_DOC; a++)
      if (pDoc->DocAssocFrame[a - 1] > 0)
	{
	   MajPavesAccessMode (pDoc->DocAssocRoot[a - 1]->ElAbstractBox[0], accessMode);
	   pDoc->DocAssocModifiedAb[a - 1] = pDoc->DocAssocRoot[a - 1]->ElAbstractBox[0];
	}
   /* reaffiche toutes les vues */
   AbstractImageUpdated (pDoc);
   RedisplayDocViews (pDoc);
}


/* ---------------------------------------------------------------------- */
/* |    MajElInclus met a` jour et reaffiche l'element pEl inclus dans  | */
/* |            le document pDoc.                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MajElInclus (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
void                MajElInclus (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrElement          pF, pL;
   PtrTextBuffer      pBT, pBTSuiv;
   int                 vue;
   boolean             ToCreate[MAX_VIEW_DOC];
   PtrElement          pEl1;

   /* conserve la liste des vues ou l'element a des paves */
   if (!AssocView (pEl))
      for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	 ToCreate[vue - 1] = pEl->ElAbstractBox[vue - 1] != NULL;
   else
      /* vue d'elements associes */
      ToCreate[0] = pEl->ElAbstractBox[0] != NULL;
   /* detruit les paves de l'element */
   DetrPaves (pEl, pDoc, FALSE);
   AbstractImageUpdated (pDoc);
   /* supprime l'ancienne copie dans l'arbre abstrait */
   pEl1 = pEl;
   if (pEl1->ElTerminal)
      switch (pEl1->ElLeafType)
	    {
	       case LtPicture:
	       case LtText:
		  pBT = pEl1->ElText;
		  while (pBT != NULL)
		    {
		       pBTSuiv = pBT->BuNext;
		       FreeBufTexte (pBT);
		       pBT = pBTSuiv;
		    }
		  pEl1->ElText = NULL;
		  pEl1->ElTextLength = 0;
		  break;
	       case LtPlyLine:
		  pBT = pEl1->ElPolyLineBuffer;
		  while (pBT != NULL)
		    {
		       pBTSuiv = pBT->BuNext;
		       FreeBufTexte (pBT);
		       pBT = pBTSuiv;
		    }
		  pEl1->ElPolyLineBuffer = NULL;
		  pEl1->ElNPoints = 0;
		  pEl1->ElVolume = 0;
		  pEl1->ElPolyLineType = '\0';
		  break;
	       case LtSymbol:
	       case LtGraphics:
		  pEl1->ElGraph = '\0';
		  break;
	       default:
		  break;
	    }
   else
     {
	pF = pEl->ElFirstChild;
	while (pF != NULL)
	  {
	     pL = pF->ElNext;
	     DeleteElement (&pF);
	     pF = pL;
	     /* effectue une nouvelle copie */
	  }
     }
   CopyIncludedElem (pEl, pDoc);
   /* cree les paves de la nouvelle copie dans les vues ou il y avait */
   /* deja des paves */
   if (!AssocView (pEl))
     {
	for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
	   if (ToCreate[vue - 1])
	     {
		pDoc->DocViewFreeVolume[vue - 1] = pDoc->DocViewVolume[vue - 1];
		CrPaveNouv (pEl, pDoc, vue);
	     }
     }
   else
      /* vue d'elements associes */
   if (ToCreate[0])
     {
	pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] =
	   pDoc->DocAssocVolume[pEl->ElAssocNum - 1];
	CrPaveNouv (pEl, pDoc, 1);
     }
   ApplDelayedRule (pEl, pDoc);
   /* reaffiche l'element dans toutes les vues ou il existe */
   AbstractImageUpdated (pDoc);
   RedisplayDocViews (pDoc);
   /* Reaffiche les numeros suivants qui changent */
   MajNumeros (NextElement (pEl), pEl, pDoc, TRUE);
}


/* ---------------------------------------------------------------------- */
/* |    MajInclus met a` jour les elements inclus d'un document.        | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         MajInclus (PtrDocument pDoc)
#else  /* __STDC__ */
static void         MajInclus (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrReferredDescr    pDR;
   PtrReference        pRef;
   PtrDocument         pDocRef;
   PtrExternalDoc       pDE;
   boolean             RestaureSel;

   RestaureSel = FALSE;
   /* parcourt la chaine des descripteurs d'elements reference's */
   pDR = pDoc->DocReferredEl;
   if (pDR != NULL)
      /* saute le premier descripteur bidon */
      pDR = pDR->ReNext;
   while (pDR != NULL)
      /* on ne considere que les elements reference's internes au document */
     {
	if (!pDR->ReExternalRef)
	  {
	     pDocRef = NULL;
	     pRef = NULL;
	     pDE = NULL;

	     /* cherche toutes les references a cet element qui se trouvent
	        dans un document charge' dans l'editeur */
	     do
	       {

		  pRef = NextReferenceToEl (pDR->ReReferredElem, pDoc, FALSE, pRef, &pDocRef, &pDE, TRUE);
		  if (pRef != NULL)
		     if (pRef->RdTypeRef == RefInclusion)
			/* c'est une inclusion */
			if (pRef->RdElement != NULL)
			   if (pRef->RdElement->ElSource != NULL)
			      /* c'est une inclusion avec expansion */
			      if (pDocRef != pDoc)
				 /* l'element inclus est dans un autre doc */
				 /* on traite cette inclusion */
				{
				   /* eteint la selection si ca n'a pas deja */
				   /* ete fait */
				   if (!RestaureSel)
				     {
					ClearAllViewSelection ();
					RestaureSel = TRUE;
				     }
				   /* refait la copie de l'element inclus */
				   MajElInclus (pRef->RdElement, pDocRef);
				}
	       }
	     while (!(pRef == NULL));
	  }
	/* passe au descripteur d'element reference' suivant */
	if (pDR != NULL)
	   pDR = pDR->ReNext;
     }
   if (RestaureSel)
      /* rallume la selection */
      AllumeSelection (FALSE, FALSE);
}


/* ---------------------------------------------------------------------- */
/* |    changenomdoc change le nom d'un document pDoc en Nm.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                changenomdoc (PtrDocument pDoc, char *Nm)
#else  /* __STDC__ */
void                changenomdoc (pDoc, Nm)
PtrDocument         pDoc;
char               *Nm;

#endif /* __STDC__ */
{
   int                 lg, vue;
   DocViewDescr         *pDe1;
   char                texte[MAX_TXT_LEN];

   /* met dans le buffer le nom du document... */
   lg = 0;
   strcpy (texte, Nm);
   strncpy (pDoc->DocDName, Nm, MAX_NAME_LENGTH);
   strncpy (pDoc->DocIdent, Nm, MAX_DOC_IDENT_LEN);
   lg = strlen (Nm);
   if (strcmp (Nm + lg - 4, ".PIV") == 0)
     {
	texte[lg - 4] = '\0';
	pDoc->DocDName[lg - 4] = '\0';
	pDoc->DocIdent[lg - 4] = '\0';
     }
   strcat (texte, "  ");
   lg = strlen (texte);
   /* traite les vues de l'arbre principal */
   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
      if (pDoc->DocView[vue - 1].DvPSchemaView > 0)
	 /* met dans le buffer le nom de la vue */
	{
	   pDe1 = &pDoc->DocView[vue - 1];
	   strncpy (&texte[lg], pDe1->DvSSchema->SsPSchema->PsView[pDe1->DvPSchemaView - 1], MAX_NAME_LENGTH);
	   ChangeTitre (pDoc->DocViewFrame[vue - 1], texte);
	}
   /* traite les vues des elements associes */
   for (vue = 1; vue <= MAX_ASSOC_DOC; vue++)
      if (pDoc->DocAssocRoot[vue - 1] != NULL)
	 if (pDoc->DocAssocFrame[vue - 1] != 0)
	    /* met dans le buffer le nom des elements associes */
	   {
	      strncpy (&texte[lg], pDoc->DocAssocRoot[vue - 1]->ElStructSchema->SsRule[pDoc->DocAssocRoot[vue - 1]->ElTypeNumber - 1].SrName,
		       MAX_NAME_LENGTH);
	      /* change le titre de la fenetre */
	      ChangeTitre (pDoc->DocAssocFrame[vue - 1], texte);
	   }
}

/* ---------------------------------------------------------------------- */
/* |    simpleSave sauve un document sous forme pivot dans un fichier   | */
/* |            dont le nom est donne par name, et ne fait rien d'autre.| */
/* |            Rend false si l'ecriture n'a pu se faire.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      simpleSave (PtrDocument pDoc, char *name, boolean withAPP)
#else  /* __STDC__ */
static boolean      simpleSave (pDoc, name, withAPP)
PtrDocument         pDoc;
char               *name;
boolean             withAPP;

#endif /* __STDC__ */
{
   BinFile             fichpivot;
   NotifyDialog        notifyDoc;
   boolean             ok;

   if (!pDoc->DocReadOnly)
     {
	fichpivot = BIOwriteOpen (name);
	if (fichpivot == 0)
	   return FALSE;
	else
	  {
	     if (withAPP)
	       {
		  /* envoie le message DocSave.Pre a l'application */
		  notifyDoc.event = TteDocSave;
		  notifyDoc.document = (Document) IdentDocument (pDoc);
		  notifyDoc.view = 0;
		  ok = !CallEventType ((NotifyEvent *) & notifyDoc, TRUE);
	       }
	     else
		ok = TRUE;
	     if (ok)
		/* l'application laisse Thot effectuer la sauvegarde */
	       {
		  /* ecrit le document dans ce fichier sous la forme pivot */
		  SauveDoc (fichpivot, pDoc);
		  BIOwriteClose (fichpivot);
		  if (withAPP)
		    {
		       /* envoie le message DocSave.Post a l'application */
		       notifyDoc.event = TteDocSave;
		       notifyDoc.document = (Document) IdentDocument (pDoc);
		       notifyDoc.view = 0;
		       CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		    }
	       }
	     return TRUE;
	  }
     }
   return FALSE;
}


/* ---------------------------------------------------------------------- */
/* |    saveWithExtension sauve un document sous forme pivot en         | */
/* |            concatenant l'extension au nom stocke' dans le document.| */
/* |            Envoie un message et rend false si l'ecriture n'a pu se | */
/* |            faire.                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      saveWithExtension (PtrDocument pDoc, char *extension)
#else  /* __STDC__ */
static boolean      saveWithExtension (pDoc, extension)
PtrDocument         pDoc;
char               *extension;

#endif /* __STDC__ */
{
   int                 i;
   char                texte[MAX_TXT_LEN];

   if (pDoc == NULL)
      return FALSE;
   DoFileName (pDoc->DocDName, extension, pDoc->DocDirectory, texte, &i);
   if (simpleSave (pDoc, texte, FALSE))
     {
	MajInclus (pDoc);
	return TRUE;
     }
   else
     {
	TtaDisplayMessage (CONFIRM, TtaGetMessage(LIB, WRITING_IMP), texte);
	return FALSE;
     }
}

/* ---------------------------------------------------------------------- */
/* |    SauverDoc       effectue la sauvegarde du document pDoc         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             SauverDoc (PtrDocument pDoc, Name NomDuDocument, PathBuffer NomDirectory, boolean SauveDocAvecCopie, boolean SauveDocAvecMove)
#else  /* __STDC__ */
boolean             SauverDoc (pDoc, NomDuDocument, NomDirectory, SauveDocAvecCopie, SauveDocAvecMove)
PtrDocument         pDoc;
Name                 NomDuDocument;
PathBuffer          NomDirectory;
boolean             SauveDocAvecCopie;
boolean             SauveDocAvecMove;

#endif /* __STDC__ */
{
   boolean             status, ok;
   PathBuffer          NomAuto;	/* sauvegardes auto */
   PathBuffer          NomFichier;	/* le .PIV final */
   PathBuffer          NomTemporaire;	/* le .Tmp */
   PathBuffer          NomBackup;	/* .OLD: ancien .PIV */
   PathBuffer          DirectoryOrig;
   char                texte[MAX_TXT_LEN];
   boolean             SauverDansMemeFichier;
   int                 i;
   NotifyDialog        notifyDoc;

   notifyDoc.event = TteDocSave;
   notifyDoc.document = (Document) IdentDocument (pDoc);
   notifyDoc.view = 0;
   if (CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
      /* l'application a pris la sauvegarde en charge */
      status = TRUE;
   else
     {
	status = TRUE;
	SauverDansMemeFichier = TRUE;
	if (strcmp (NomDuDocument, pDoc->DocDName) != 0)
	   SauverDansMemeFichier = FALSE;
	if (strcmp (NomDirectory, pDoc->DocDirectory) != 0)
	   SauverDansMemeFichier = FALSE;

	/* construit le nom complet de l'ancien fichier de sauvegarde */
	DoFileName (pDoc->DocDName, "BAK", pDoc->DocDirectory, NomAuto, &i);
	strncpy (DirectoryOrig, pDoc->DocDirectory, MAX_PATH);
	/*     SECURITE:                                         */
	/*     on ecrit sur un fichier nomme' X.Tmp et non pas   */
	/*     directement X.PIV ...                             */
	/*     On fait ensuite des renommages                    */
	DoFileName (NomDuDocument, "PIV", NomDirectory, texte, &i);
	/* on teste d'abord le droit d'ecriture sur le .PIV */
	ok = OuvrEcr (texte) == 0;
	if (ok)
	  {
	     DoFileName (NomDuDocument, "Tmp", NomDirectory, NomTemporaire, &i);
	     /* on teste le droit d'ecriture sur le .Tmp */
	     ok = OuvrEcr (NomTemporaire) == 0;
	     if (ok)
	       {
		  TtaDisplaySimpleMessage (INFO, LIB, WRITING);
		  ok = simpleSave (pDoc, NomTemporaire, FALSE);
	       }
	     if (ok)
		MajInclus (pDoc);
	  }
	if (!ok)
	  {
	     /* on indique un nom connu de l'utilisateur... */
	     DoFileName (NomDuDocument, "PIV", NomDirectory, texte, &i);
	     TtaDisplayMessage (CONFIRM, TtaGetMessage(LIB, WRITING_IMP),
					    texte);
	     status = FALSE;
	  }
	else
	  {
	     /* 1- faire mv .PIV sur .OLD sauf si c'est une copie */
	     /* Le nom et le directory du document peuvent avoir change'. */
	     /* le fichier .OLD reste dans l'ancien directory, avec */
	     /* l'ancien nom */
	     DoFileName (pDoc->DocDName, "PIV", DirectoryOrig, NomFichier, &i);
	     if (!SauveDocAvecCopie)
	       {
		  DoFileName (pDoc->DocDName, "OLD", DirectoryOrig, NomBackup, &i);
		  i = rename (NomFichier, NomBackup);
	       }
	     /* 2- faire mv du .Tmp sur le .PIV */
	     DoFileName (NomDuDocument, "PIV", NomDirectory, NomFichier, &i);
	     i = rename (NomTemporaire, NomFichier);
	     if (i >= 0)
		/* >> tout s'est bien passe' << */
		/* detruit l'ancienne sauvegarde */
	       {
		  RemoveFile (NomAuto);
		  TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_DOC_WRITTEN),
						 NomFichier);
		  /* c'est trop tot pour perdre l'ancien nom du fichier et son */
		  /* directory d'origine. */
		  docUnmodify (pDoc);

		  /* modifie les fichiers .EXT des documents nouvellement */
		  /* reference's ou qui ne sont plus reference's par */
		  /* notre document */
		  UpdateExt (pDoc);
		  /* modifie les fichiers .REF des documents qui */
		  /* referencent des elements qui ne sont plus dans notre */
		  /* document et met a jour le fichier .EXT de notre */
		  /* document */
		  UpdateRef (pDoc);
		  /* detruit le fichier .REF du document sauve' */
		  DoFileName (pDoc->DocDName, "REF", DirectoryOrig, texte, &i);
		  RemoveFile (texte);
		  if (!SauverDansMemeFichier)
		    {
		       if (strcmp (NomDirectory, DirectoryOrig) != 0 &&
			   strcmp (NomDuDocument, pDoc->DocDName) == 0)
			  /* changement de directory sans changement de nom */
			  if (SauveDocAvecMove)
			    {
			       /* deplacer le fichier .EXT dans le nouveau directory */
			       DoFileName (pDoc->DocDName, "EXT", DirectoryOrig, texte, &i);
			       DoFileName (pDoc->DocDName, "EXT", NomDirectory, NomFichier, &i);
			       rename (texte, NomFichier);
			       /* detruire l'ancien fichier PIV */
			       DoFileName (pDoc->DocDName, "PIV", DirectoryOrig, texte, &i);
			       RemoveFile (texte);
			    }

		       if (strcmp (NomDuDocument, pDoc->DocDName) != 0)
			 {
			    /* il y a effectivement changement de nom */
			    if (SauveDocAvecCopie)
			       /* l'utilisateur veut creer une copie du document. */
			       /* on fait apparaitre le document copie dans les */
			       /* fichiers .EXT des documents reference's */
			       ChangeNomExt (pDoc, NomDuDocument, TRUE);
			    if (SauveDocAvecMove)
			      {
				 /* il s'agit d'un changement de nom du document */
				 /* change le nom du document dans les fichiers */
				 /* .EXT de tous les documents reference's */
				 ChangeNomExt (pDoc, NomDuDocument, FALSE);
				 /* indique le changement de nom a tous les */
				 /* documents qui referencent ce document */
				 ChangeNomRef (pDoc, NomDuDocument);
				 /* renomme le fichier .EXT du document qui change */
				 /* de nom */
				 DoFileName (pDoc->DocDName, "EXT", DirectoryOrig, texte,
					     &i);
				 DoFileName (NomDuDocument, "EXT", NomDirectory,
					     NomFichier, &i);
				 rename (texte, NomFichier);
				 /* detruit l'ancien fichier .PIV */
				 DoFileName (pDoc->DocDName, "PIV", DirectoryOrig, texte,
					     &i);
				 RemoveFile (texte);
			      }
			 }
		       strncpy (pDoc->DocDName, NomDuDocument, MAX_NAME_LENGTH);
		       strncpy (pDoc->DocIdent, NomDuDocument, MAX_DOC_IDENT_LEN);
		       strncpy (pDoc->DocDirectory, NomDirectory, MAX_PATH);
		       changenomdoc (pDoc, NomDuDocument);
		    }
	       }
	     notifyDoc.event = TteDocSave;
	     notifyDoc.document = (Document) IdentDocument (pDoc);
	     notifyDoc.view = 0;
	     CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	  }
     }
   return status;
}

/* ---------------------------------------------------------------------- */
/* |    interactiveSave sauve un document sous forme pivot en proposant | */
/* |            un menu a` l'utilisateur (si 'ask'). Rend false si      | */
/* |            l'ecriture n'a pu se faire.                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      interactiveSave (PtrDocument pDoc, boolean ask)

#else  /* __STDC__ */
static boolean      interactiveSave (pDoc, ask)
PtrDocument         pDoc;
boolean             ask;

#endif /* __STDC__ */

{
   boolean             ok;
   boolean             status;
   Name                 nomdoc;
   char                nomdir[MAX_PATH];

   status = FALSE;
   if (pDoc->DocReadOnly)
      /* on ne sauve pas les documents qui sont en lecture seule */
      TtaDisplaySimpleMessage (INFO, LIB, RO_DOC_FORBIDDEN);
   else if (pDoc->DocSSchema == NULL)
      TtaDisplaySimpleMessage (INFO, LIB, EMPTY_DOC_NOT_WRITTEN);
   else
     {
	strncpy (nomdoc, pDoc->DocDName, MAX_NAME_LENGTH);
	/* on prend le directory ou le document a ete lu */
	strncpy (nomdir, pDoc->DocDirectory, MAX_PATH);
	/* recherche le nom du fichier en proposant le nom courant */
	ok = !ask;
	if (ok && !pDoc->DocReadOnly)
	   status = SauverDoc (pDoc, nomdoc, nomdir, FALSE, FALSE);
     }
   if (status && ask)
      docUnmodify (pDoc);
   return status;
}


/* ---------------------------------------------------------------------- */
/* |    SauveDocument sauve sous forme pivot le document pointe' par    | */
/* |            pDoc. Retourne Vrai si le document a pu etre sauve,     | */
/* |            Faux si echec.                                          | */
/* |            - Mode = 0 : demander le nom de fichier a` l'utilisateur| */
/* |            - Mode = 1 : fichier de sauvegarde automatique (.BAK)   | */
/* |            - Mode = 2 : fichier scratch (pas de message)           | */
/* |            - Mode = 3 : fichier de sauvegarde urgente (.SAV)       | */
/* |            - Mode = 4 : sauve sans demander de nom.                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             SauveDocument (PtrDocument pDoc, int Mode)

#else  /* __STDC__ */
boolean             SauveDocument (pDoc, Mode)
PtrDocument         pDoc;
int                 Mode;

#endif /* __STDC__ */

{
   boolean             ok;

   ok = FALSE;
   if (pDoc != NULL)
      if (Mode >= 0 && Mode <= 4)
	 switch (Mode)
	       {
		  case 0:
		     ok = interactiveSave (pDoc, TRUE);
		     break;
		  case 1:
		     ok = saveWithExtension (pDoc, "BAK");
		     if (ok)
			TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_DOC_WRITTEN), pDoc->DocDName);
		     break;
		  case 2:
		     ok = saveWithExtension (pDoc, "BAK");
		     break;
		  case 3:
		     ok = saveWithExtension (pDoc, "SAV");
		     break;
		  case 4:
		     ok = interactiveSave (pDoc, FALSE);
		     break;
	       }
   return ok;
}


/* ---------------------------------------------------------------------- */
/* |    DestVue libere tous les paves de la vue correspondant a` la     | */
/* |            frame de numero nframe. Appele par le mediateur lorsque | */
/* |            l'utilisateur detruit une frame.                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DestVue (int nframe)

#else  /* __STDC__ */
void                DestVue (nframe)
int                 nframe;

#endif /* __STDC__ */

{
   PtrDocument         pD;
   int                 nv;
   NotifyDialog        notifyDoc;
   boolean             assoc;

   /* cherche le document auquel appartient la fenetre detruite */
   DocVueFen (nframe, &pD, &nv, &assoc);
   if (pD != NULL)
     {
	notifyDoc.event = TteViewClose;
	notifyDoc.document = (Document) IdentDocument (pD);
	if (assoc)
	   notifyDoc.view = nv + 100;
	else
	   notifyDoc.view = nv;
	CallEventType ((NotifyEvent *) & notifyDoc, TRUE);
	/* desactive la vue si elle est active */
	DesactVue (pD, nv, assoc);
	/* detruit la fenetre */
	DestroyFrame (nframe);
	CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	/* detruit le contexte de la vue */
	detruit (pD, nv, assoc, TRUE);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ReaffRefExternes cherche, pour tous les elements du document    | */
/* |            pDoc qui sont designes par des references, toutes les   | */
/* |            references appartenant a` d'autres documents charges et | */
/* |            fait reafficher ces references si elles sont deja       | */
/* |            affichees.                                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ReaffRefExternes (PtrDocument pDoc)

#else  /* __STDC__ */
static void         ReaffRefExternes (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrReferredDescr    pDescElRef;
   PtrReference        pRef;
   PtrElement          pEl;
   PtrDocument         pDocRef;
   PtrExternalDoc       pDE;

   /* parcourt les descripteurs d'elements reference's du document */
   pDescElRef = pDoc->DocReferredEl->ReNext;
   while (pDescElRef != NULL)
     {
	if (!pDescElRef->ReExternalRef)
	   if (pDescElRef->ReExtDocRef != NULL)
	      /* il y a des references a cet element depuis d'autres */
	      /* documents cherche toutes les references a cet element */
	     {
		pEl = pDescElRef->ReReferredElem;
		/* l'element reference' */
		pRef = NULL;
		pDocRef = NULL;
		pDE = NULL;
		/* on n'a pas encore trouve' de reference */
		do
		   /* cherche la reference suivant a l'element */
		  {

		     pRef = NextReferenceToEl (pEl, pDoc, FALSE, pRef, &pDocRef, &pDE, TRUE);
		     if (pRef != NULL)
			/* on a trouve' une reference a cet element */
			if (pDocRef != pDoc)
			   /* la reference trouvee n'est pas dans le meme */
			   /* document que l'element reference', on reaffiche */
			   /* tous ses paves qui copient l'element reference'. */
			  {
			     if (pRef->RdTypeRef == RefInclusion)
				/* c'est une inclusion */
				if (pRef->RdElement != NULL)
				   if (pRef->RdElement->ElSource != NULL)
				      /* c'est une inclusion avec expansion, on */
				      /* copie d'abord l'element inclus */
				      CopyIncludedElem (pRef->RdElement, pDocRef);
			     ReafReference (pRef, NULL, pDocRef);
			  }
		  }
		while (!(pRef == NULL));
	     }
	/* passe au descripteur d'element reference' suivant */
	pDescElRef = pDescElRef->ReNext;
     }
}

/* ---------------------------------------------------------------------- */
/* |    OuvreVuesInit   ouvre, pour le document pDoc, toutes les vues   | */
/* |            qui doivent etre ouvertes a l'ouverture du document.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                OuvreVuesInit (PtrDocument pDoc)

#else  /* __STDC__ */
void                OuvreVuesInit (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 vue, i, X, Y, L, H;
   int                 VueSch;
   PtrPSchema          pSchP;

   boolean /*bool, */ complet, skeleton;
   NotifyDialog        notifyDoc;

   /* si le document a ete charge' sous le forme de ses seuls */
   /* elements visibles de l'exterieur, on ouvre la vue EXPORT, */
   /* sinon, on ouvre la premiere vue. */
   skeleton = FALSE;
   if (pDoc->DocExportStructure)
     {
	pSchP = pDoc->DocSSchema->SsPSchema;
	vue = 0;
	do
	   vue++;
	while (!(vue == pSchP->PsNViews || pSchP->PsExportView[vue - 1]));
	if (!pSchP->PsExportView[vue - 1])
	   vue = 1;		/* pas de vue EXPORT, on prend la vue 1 */
	else
	   skeleton = TRUE;
     }
   else
      vue = 1;
   /* demande la creation d'une frame pour la vue a ouvrir */
   /* chercher la geometrie de la fenetre dans le fichier .conf */
   ConfigGetViewGeometry (pDoc, pDoc->DocSSchema->SsPSchema->PsView[vue - 1], &X, &Y, &L, &H);
   notifyDoc.event = TteViewOpen;
   notifyDoc.document = (Document) IdentDocument (pDoc);
   notifyDoc.view = 0;
   if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
     {
	VueSch = pDoc->DocView[vue - 1].DvPSchemaView;
	pDoc->DocViewFrame[0] = CrFenTitre (pDoc, VueSch,
			    pDoc->DocSSchema->SsPSchema->PsView[vue - 1],
					&pDoc->DocViewVolume[0], X, Y, L, H);
     }
   if (pDoc->DocViewFrame[0] == 0)
      /* echec creation frame */
     {
	LibDocument (&pDoc);
	TtaDisplaySimpleMessage (INFO, LIB, OPENING_NEW_FRAME_IMP);
     }
   else
     {
	pDoc->DocView[0].DvSSchema = pDoc->DocSSchema;
	pDoc->DocView[0].DvPSchemaView = vue;
	pDoc->DocView[0].DvSync = TRUE;
	pDoc->DocViewFreeVolume[0] = pDoc->DocViewVolume[0];
	/* met a jour les menus variables de la fenetre */
	if (ThotLocalActions[T_chselect] != NULL)
	   (*ThotLocalActions[T_chselect]) (pDoc);
	if (ThotLocalActions[T_chattr] != NULL)
	   (*ThotLocalActions[T_chattr]) (pDoc);
	if (pDoc->DocRootElement != NULL)
	  {
#ifdef __COLPAGE__
	     /* test si pagine */
	     if (TypeBPage (pDoc->DocRootElement->ElFirstChild, vue, &pSchPage) != 0)
		/* document pagine, on initialise NbPages et VolLibre */
	       {
		  pDoc->DocViewNPages[0] = 0;
		  pDoc->DocViewFreeVolume[0] = THOT_MAXINT;
	       }
#endif /* __COLPAGE__ */
	     pDoc->DocViewRootAb[0] = AbsBoxesCreate (pDoc->DocRootElement, pDoc, 1,
						TRUE, TRUE, &complet);
#ifdef __COLPAGE__
	     /* sauvegarde de l'image abstraite pour tests */
	     list = fopen ("/perles/roisin/debug/toto", "w");
	     if (list != NULL)
	       {
		  NumPav (pDo1->DocViewRootAb[0]);
		  AffPaves (pDo1->DocViewRootAb[0], 2, list);
		  fclose (list);
	       }
#endif /* __COLPAGE__ */
	     i = 0;
	     /* on ne s'occupe pas de la hauteur de page */
	     ChangeConcreteImage (pDoc->DocViewFrame[0], &i, pDoc->DocViewRootAb[0]);
	     DisplayFrame (pDoc->DocViewFrame[0]);
	     notifyDoc.event = TteViewOpen;
	     notifyDoc.document = (Document) IdentDocument (pDoc);
	     notifyDoc.view = 1;
	     CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	     /* Ouvre les vues specifiees dans la section open */
	     /* du fichier .config, sauf s'il s'agit d'un document */
	     /* charge' sous forme de squelette. */
	     if (!skeleton)
		ConfigOpenFirstViews (pDoc);
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    LoadDocument charge le document que contient le fichier nomme'  | */
/* |            nomfichier dans le descripteur pointe par pDoc. Au      | */
/* |            retour pDoc est NIL si le document n'a pas pu etre      | */
/* |            charge.                                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                LoadDocument (PtrDocument * pDoc, char *nomfichier)
#else  /* __STDC__ */
void                LoadDocument (pDoc, nomfichier)
PtrDocument        *pDoc;
char               *nomfichier;

#endif /* __STDC__ */
{
   int                 i, j, lg;
   boolean             ok;

   /*boolean               bool; */
   PtrDocument         pDo1;

   PathBuffer          SaveDirDoc;

#ifdef __COLPAGE__
   FILE               *list;
   PtrPSchema          pSchPage;

#endif /* __COLPAGE__ */
   /* bool = FALSE; */
   SaveDirDoc[0] = '\0';

   if (nomfichier != NULL)
      /* nom de document fourni a l'appel, on le recopie dans DefaultDocumentName */
     {
	lg = strlen (nomfichier);
	if (lg > 4)
	   if (strcmp (nomfichier + lg - 4, ".PIV") == 0)
	      nomfichier[lg - 4] = '\0';
	i = 1;
	if (nomfichier[0] != DIR_SEP)
	  {
	     if (nomfichier != DefaultDocumentName)
		strncpy (DefaultDocumentName, nomfichier, MAX_NAME_LENGTH);
	     /* nom de document relatif */
	     strncpy ((*pDoc)->DocDName, DefaultDocumentName, MAX_NAME_LENGTH);
	     strncpy ((*pDoc)->DocIdent, DefaultDocumentName, MAX_DOC_IDENT_LEN);
	     if ((*pDoc)->DocDirectory[0] == '\0')
		strncpy ((*pDoc)->DocDirectory, DocumentPath, MAX_PATH);
	  }
	else
	  {
	     /* nom absolu */
	     j = 1;
	     while (nomfichier[i - 1] != '\0' && i < MAX_PATH)
	       {
		  (*pDoc)->DocDirectory[i - 1] = nomfichier[i - 1];
		  if ((*pDoc)->DocDirectory[i - 1] == DIR_SEP)
		     j = i;
		  i++;
	       }
	     (*pDoc)->DocDirectory[j] = '\0';
	     i = 1;
	     while (nomfichier[i - 1] != '\0' && i < MAX_NAME_LENGTH)
	       {
		  DefaultDocumentName[i - 1] = nomfichier[j];
		  i++;
		  j++;
	       }
	     DefaultDocumentName[i - 1] = '\0';
	     strncpy ((*pDoc)->DocDName, DefaultDocumentName, MAX_NAME_LENGTH);
	     strncpy ((*pDoc)->DocIdent, DefaultDocumentName, MAX_DOC_IDENT_LEN);
	     /* sauve le path des documents avant de l'ecraser */
	     strncpy (SaveDirDoc, DocumentPath, MAX_PATH);
	     strncpy (DocumentPath, (*pDoc)->DocDirectory, MAX_PATH);
	  }
     }

   if (*pDoc != NULL)
     {
	/* on ouvre le document en chargeant temporairement les documents */
	/* externes qui contiennent les elements inclus dans notre document */
	TtaDisplaySimpleMessage (INFO, LIB, READING_DOC);
	ok = OpenDocument (DefaultDocumentName, *pDoc, TRUE, FALSE, NULL, TRUE);
	/* restaure le path des documents s'il a ete ecrase */
	if (SaveDirDoc[0] != '\0')
	   strncpy (DocumentPath, SaveDirDoc, MAX_PATH);
	if (!ok)
	  {
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, OPEN_DOC_IMP), DefaultDocumentName);
	     LibDocument (pDoc);
	     *pDoc = NULL;
	  }
     }
   if (*pDoc != NULL)
     {
	pDo1 = *pDoc;
	/* conserve le path actuel des schemas dans le contexte du document */
	strncpy (pDo1->DocSchemasPath, SchemaPath, MAX_PATH);
	/* ouvre les vues a ouvrir */
	OuvreVuesInit (pDo1);
	if (pDo1->DocRootElement != NULL)
	   /* Pour tous les elements du document que l'on vient de */
	   /* charger qui sont designe's par des references, cherche */
	   /* toutes les references appartenant a d'autres documents */
	   /* charges et fait reafficher ces references si elles sont */
	   /* deja affichees */
	   ReaffRefExternes (*pDoc);
     }
}

/* ---------------------------------------------------------------------- */
/* |    NewDocument cree un document vide, conforme au schema de nom    | */
/* |            nomschema, dans le descripteur pointe' par pDoc.        | */
/* |            nomdoc est le nom a donner au document                  | */
/* |            nomdir est le directory ou il faut creer le document    | */
/* |            Au retour pDoc est NIL si le document n'a pas ete cree. | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                NewDocument (PtrDocument * pDoc, PtrBuffer nomschema, Name nomdoc, PathBuffer nomdir)
#else  /* __STDC__ */
void                NewDocument (pDoc, nomschema, nomdoc, nomdir)
PtrDocument        *pDoc;
PtrBuffer           nomschema;
Name                 nomdoc;
PathBuffer          nomdir;

#endif /* __STDC__ */
{
   Name                 NomSchPrs;
   int                 i;
   Name                 NomDocu;
   Name                 NomClasse;
   PtrDocument         pDo1;
   PtrElement          pEl;
   PathBuffer          NomDirectory;
   PathBuffer          BufNomFichier;

#ifdef __COLPAGE__
   PtrPSchema          pSchPage;

#else  /* __COLPAGE__ */
   PtrPSchema          pSchPres;
   int                 vue;

#endif /* __COLPAGE__ */
   NotifyDialog        notifyDoc;

   if (*pDoc != NULL)
      if (nomschema == NULL || nomschema[0] == '\0')
	 /* L'utilisateur n'a pas fourni de nom de schema */
	 LibDocument (pDoc);
      else
	{
	   pDo1 = *pDoc;
	   strncpy (pDo1->DocDirectory, DocumentPath, MAX_PATH);
	   /* si c'est un path, retient seulement le 1er directory */
	   i = 1;
	   while (pDo1->DocDirectory[i - 1] != '\0' &&
		  pDo1->DocDirectory[i - 1] != PATH_SEP && i < MAX_PATH)
	      i++;
	   pDo1->DocDirectory[i - 1] = '\0';
	   /* on suppose que le mon de schema est dans la langue de */
	   /* l'utilisateur: on le traduit en nom interne */
	   ConfigNomInterneSSchema ((char *) nomschema, NomClasse, TRUE);
	   if (NomClasse[0] == '\0')
	      /* ce nom n'est pas dans le fichier langue, on le prend */
	      /* tel quel */
	      strncpy (NomClasse, (char *) nomschema, MAX_NAME_LENGTH);
	   /* compose le nom du fichier a ouvrir avec le nom du directory */
	   /* des schemas... */
	   strncpy (NomDirectory, SchemaPath, MAX_PATH);
	   BuildFileName (NomClasse, "STR", NomDirectory, BufNomFichier, &i);
	   /* teste si le fichier '.STR' existe */

	   if (FileExist (BufNomFichier) == 0)
	     {
		strncpy (BufNomFichier, NomClasse, MAX_NAME_LENGTH);
		strcat (BufNomFichier, ".STR");
		TtaDisplayMessage (INFO, TtaGetMessage(LIB, SCHEMA_NOT_FIND), BufNomFichier);
	     }
	   else
	     {
		/* charge le schema de structure et le schema de presentation */
		NomSchPrs[0] = '\0';
		/* pas de preference pour un schema de */
		/* presentation particulier */
		LoadSchemas (NomClasse, NomSchPrs, &pDo1->DocSSchema, NULL, FALSE);
		if (nomdoc[0] != '\0')
		   strncpy (NomDocu, nomdoc, MAX_NAME_LENGTH);
		else
		  {
		     strncpy (NomDocu, (char *) nomschema, MAX_NAME_LENGTH);
		     strcat (NomDocu, "X");
		  }
		if (pDo1->DocSSchema != NULL)
		   if (pDo1->DocSSchema->SsPSchema != NULL)
		     {
			notifyDoc.event = TteDocCreate;
			notifyDoc.document = (Document) IdentDocument (pDo1);
			notifyDoc.view = 0;
			if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
			  {
			     /* cree la representation interne d'un document vide */
			     pDo1->DocRootElement = NewSubtree (pDo1->DocSSchema->SsRootElem,
								pDo1->DocSSchema, pDo1, 0, TRUE, TRUE, TRUE, TRUE);
			     /* supprime les elements exclus (au sens SGML) */
			     RemoveExcludedElem (&pDo1->DocRootElement);
			  }
		     }
	     }
	   if (pDo1->DocRootElement == NULL)
	      /* echec creation document */
	      LibDocument (pDoc);
	   else
	     {

		pDo1->DocRootElement->ElAccess = AccessReadWrite;
		CheckLanguageAttr (pDo1, pDo1->DocRootElement);
#ifdef __COLPAGE__
		/* il n'est plus necessaire d'ajouter un saut de page */
		/* a la fin de l'arbre principal */
#else  /* __COLPAGE__ */
		/* ajoute un saut de page a la fin de l'arbre principal */
		/* pour toutes les vues qui sont mises en page */
		/* schema de presentation du document */
		pSchPres = pDo1->DocSSchema->SsPSchema;
		/* examine toutes les vues definies dans le schema */
		for (vue = 1; vue <= pSchPres->PsNViews; vue++)
		   if (pSchPres->PsPaginatedView[vue - 1])
		      /* cette vue est mise en page */
		      AjoutePageEnFin (pDo1->DocRootElement, vue, pDo1, TRUE);

#endif /* __COLPAGE__ */
		/* le document appartient au directory courant */
		if (nomdir[0] != '\0')
		   strncpy (NomDirectory, nomdir, MAX_PATH);
		else
		  {
		     strncpy (NomDirectory, DocumentPath, MAX_PATH);
		     /* si c'est un path, retient seulement le 1er directory */
		     i = 1;
		     while (NomDirectory[i - 1] != '\0' &&
			    NomDirectory[i - 1] != PATH_SEP && i < MAX_PATH)
			i++;
		     NomDirectory[i - 1] = '\0';
		  }
		DoFileName (NomDocu, "PIV", NomDirectory, BufNomFichier, &i);
		strncpy (pDo1->DocDName, NomDocu, MAX_NAME_LENGTH);
		strncpy (pDo1->DocIdent, NomDocu, MAX_NAME_LENGTH);
		/* le document appartient au directory courant */
		strncpy (pDo1->DocDirectory, NomDirectory, MAX_PATH);
		/* conserve le path actuel des schemas dans le contexte du document */
		strncpy (pDo1->DocSchemasPath, SchemaPath, MAX_PATH);
		notifyDoc.event = TteDocCreate;
		notifyDoc.document = (Document) IdentDocument (pDo1);
		notifyDoc.view = 0;
		CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		/* traitement des attributs requis */
		AttachMandatoryAttributes (pDo1->DocRootElement, pDo1);
		if (pDo1->DocSSchema != NULL)
		   /* le document n'a pas ete ferme' pendant l'attente */
		   /* des attributs requis */
		  {
		     /* traitement des exceptions */
		    if (ThotLocalActions[T_createtable]!= NULL)
		      (*ThotLocalActions[T_createtable])
			(pDo1->DocRootElement, pDo1);
		     /* ouvre les vues du document cree' */
		     OuvreVuesInit (pDo1);
		     /* selectionne la 1ere feuille */
		     pEl = FirstLeaf (pDo1->DocRootElement);
		     SelectEl (pDo1, pEl, TRUE, TRUE);
		  }
	     }
	}
}

/* ---------------------------------------------------------------------- */
/* |    NouveauDocument cree un document par le nom du schema.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                NouveauDocument (PtrBuffer nomsch)
#else  /* __STDC__ */
void                NouveauDocument (nomsch)
PtrBuffer           nomsch;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;

   CreateDocument (&pDoc);
   if (pDoc != NULL)
      NewDocument (&pDoc, nomsch, TtaGetMessage (LIB, NO_NAME), "");
}

/* ---------------------------------------------------------------------- */
/* |    CreeImageAbstraite cree l'image abstraite pour une vue du       | */
/* |            document pDoc. Si v est nul, il                         | */
/* |            s'agit d'une vue d'elements associes de type r,         | */
/* |            appartenant au schema de structure pSS; sinon c'est la  | */
/* |            vue de numero v definie dans le schema de presentation  | */
/* |            associe' au schema de structure pointe' par pSS.        | */
/* |            Si RacineVue est NULL, la vue presentera un arbre       | */
/* |            entier, sinon elle n'affichera que le sous-arbre de     | */
/* |            racine RacineVue.                                       | */
/* |    Retourne 0 si echec ou le numero de vue pour le document ou le  | */
/* |            numero d'element associe' de la vue creee.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 CreeImageAbstraite (PtrDocument pDoc, int v, int r, PtrSSchema pSS, int vuedesignee, boolean debut, PtrElement RacineVue)
#else  /* __STDC__ */
int                 CreeImageAbstraite (pDoc, v, r, pSS, vuedesignee, debut, RacineVue)
PtrDocument         pDoc;
int                 v;
int                 r;
PtrSSchema        pSS;
int                 vuedesignee;
boolean             debut;
PtrElement          RacineVue;

#endif /* __STDC__ */
{
   int                 vuedoc;
   int                 elass, vol;
   int                 vuelibre;
   boolean             stop, sel, selprinc;
   boolean             coupetete, assocexiste;
   PtrDocument         SelDoc;
   PtrElement          PremSel, DerSel;
   int                 PremCar, DerCar;
   PtrAbstractBox             pAb;
   int                 retour;
   NotifyElement       notifyEl;
   boolean             complet;

#ifdef __COLPAGE__
   PtrPSchema          pSchPage;
   FILE               *list;

#endif /* __COLPAGE__ */

   retour = 0;
   vuelibre = 0;
   elass = 0;
   assocexiste = FALSE;
   if (v == 0)
      /* c'est une vue pour des elements associes */
     {
	if (r == 0)
	   assocexiste = TRUE;
	else
	  {
	     /* cherche si les elements existent deja */
	     assocexiste = FALSE;
	     elass = 0;
	     do
	       {
		  elass++;
		  if (pDoc->DocAssocRoot[elass - 1] != NULL)
		     if (pDoc->DocAssocRoot[elass - 1]->ElStructSchema->SsCode
			 == pSS->SsCode)
			assocexiste = pDoc->DocAssocRoot[elass - 1]->ElTypeNumber == r;
	       }
	     while (!(assocexiste || elass >= MAX_ASSOC_DOC));
	  }
     }
   else
     {
	/* c'est une vue de l'arbre principal */
	/* cherche la premiere vue libre dans le descripteur du document */
	vuelibre = 0;
	vuedoc = 1;
	while (vuelibre == 0 && vuedoc <= MAX_VIEW_DOC)
	   if (pDoc->DocView[vuedoc - 1].DvPSchemaView == 0)
	      vuelibre = vuedoc;
	   else
	      vuedoc++;
     }
   /* volume de l'image abstraite a creer */
   vol = 1000;
   if (v == 0)			/* on cree une vue d'elements associes */
     {
	if (!assocexiste)
	   /* il n'existe pas d'elements associes de ce type */
	  {
	     elass = 1;
	     /* cherche une entree libre dans la table des */
	     /* arbres d'elements associes du document */
	     stop = FALSE;
	     do
		if (elass > MAX_ASSOC_DOC)
		   stop = TRUE;
		else if (pDoc->DocAssocRoot[elass - 1] == NULL)
		   stop = TRUE;
		else
		   elass++;
	     while (!(stop));
	     if (pDoc->DocAssocRoot[elass - 1] == NULL)
		/* on a trouve' une entree libre, on cree un */
		/* arbre pour ces elements associes */
	       {
		  notifyEl.event = TteElemNew;
		  notifyEl.document = (Document) IdentDocument (pDoc);
		  notifyEl.element = NULL;
		  notifyEl.elementType.ElTypeNum = r;
		  notifyEl.elementType.ElSSchema = (SSchema) pSS;
		  notifyEl.position = 0;
		  if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		    {
		       pDoc->DocAssocRoot[elass - 1] =
			  NewSubtree (r, pSS, pDoc, elass, TRUE, TRUE, TRUE, TRUE);
		       /* supprime les elements exclus (au sens SGML) */
		       RemoveExcludedElem (&pDoc->DocAssocRoot[elass - 1]);
		       if (pDoc->DocAssocRoot[elass - 1] != NULL)
			 {
			    pDoc->DocAssocRoot[elass - 1]->ElAccess = AccessReadWrite;
			    CheckLanguageAttr (pDoc, pDoc->DocAssocRoot[elass - 1]);
			    /* traitement des exceptions */
			    if (ThotLocalActions[T_createtable]!= NULL)
			      (*ThotLocalActions[T_createtable])	
				(pDoc->DocAssocRoot[elass - 1], pDoc);  
			    /* creation d'une table */
			    /* traitement des attributs requis */
			    AttachMandatoryAttributes (pDoc->DocAssocRoot[elass - 1], pDoc);
#ifdef __COLPAGE__
			    /* Inutile d'ajouter un saut de page a la fin */
#else  /* __COLPAGE__ */
			    if (pDoc->DocSSchema != NULL)
			      {
				 /* Ajoute un saut de page a la fin si necessaire */
				 AjoutePageEnFin (pDoc->DocAssocRoot[elass - 1],
						  1, pDoc, TRUE);
			      }
#endif /* __COLPAGE__ */
			    /* envoie l'evenement ElemNew.Post */
			    NotifySubTree (TteElemNew, pDoc, pDoc->DocAssocRoot[elass - 1], 0);
			 }
		    }
	       }
	  }
	if ((pDoc->DocAssocRoot[elass - 1] != NULL) &&
	    (elass <= MAX_ASSOC_DOC) && pDoc->DocSSchema != NULL)
	   /* on construit l'image abstraite des elements associes */
	  {
	     pDoc->DocAssocFrame[elass - 1] = 0;
	     pDoc->DocAssocSubTree[elass - 1] = RacineVue;
	     pDoc->DocAssocVolume[elass - 1] = vol;
	     pDoc->DocAssocFreeVolume[elass - 1] = pDoc->DocAssocVolume[elass - 1];
#ifdef __COLPAGE__
	     if (TypeBPage (pDoc->DocAssocRoot[elass - 1]->ElFirstChild, 1, &pSchPage) != 0)
		/* document pagine */
	       {
		  pDoc->DocAssocNPages[elass - 1] = 0;	/* nbpages = 0 */
		  pDoc->DocAssocFreeVolume[elass - 1] = THOT_MAXINT;
	       }
#endif /* __COLPAGE__ */
	     if (!debut)
	       {
		  /* prend la selection courante */
		  sel = SelEditeur (&SelDoc, &PremSel, &DerSel, &PremCar, &DerCar);
		  if (!sel)
		     /* pas de selection, on construit l'image du debut */
		     debut = TRUE;
		  else if (SelDoc != pDoc || PremSel->ElAssocNum != elass)
		     /* la selection courante n'est pas dans les */
		     /* elements associes dont on cree l'image */
		     /* on cree l'image abstraite du debut */
		     debut = TRUE;
	       }
	     if (debut)
		pAb = AbsBoxesCreate (pDoc->DocAssocRoot[elass - 1], pDoc, 1, TRUE, TRUE, &complet);
	     else
		/* on cree l'image abstraite autour du premier */
		/* element selectionne' */
		VerifAbsBoxe (PremSel, 1, pDoc, FALSE, FALSE);
	  }
#ifdef __COLPAGE__
	/* sauvegarde de l'image abstraite pour tests */
	list = fopen ("/perles/roisin/debug/totoassoc", "w");
	if (list != NULL)
	  {
	     NumPav (pDoc->DocAssocRoot[elass - 1]->ElAbstractBox[0]);
	     AffPaves (pDoc->DocAssocRoot[elass - 1]->ElAbstractBox[0], 2, list);
	     fclose (list);
	  }
#endif /* __COLPAGE__ */
	retour = elass;
     }
   else
     {
	/* on cree une vue de l'arbre principal du document */
	pDoc->DocViewSubTree[vuelibre - 1] = RacineVue;
	pDoc->DocView[vuelibre - 1].DvSSchema = pSS;
	pDoc->DocView[vuelibre - 1].DvPSchemaView = v;
	pDoc->DocView[vuelibre - 1].DvSync = TRUE;

	pDoc->DocViewVolume[vuelibre - 1] = vol;
	pDoc->DocViewFreeVolume[vuelibre - 1] = pDoc->DocViewVolume[vuelibre - 1];
#ifdef __COLPAGE__
	if (TypeBPage (pDoc->DocRootElement->ElFirstChild, v, &pSchPage) != 0)
	   /* document pagine */
	  {
	     pDoc->DocViewNPages[vuelibre - 1] = 0;	/* nbpages = 0 */
	     pDoc->DocViewFreeVolume[vuelibre - 1] = THOT_MAXINT;
	  }
#endif /* __COLPAGE__ */
	retour = vuelibre;

	if (debut)
	   /* la fenetre designee par l'utilisateur contient des */
	   /* elements associes on cree la nouvelle image depuis */
	   /* le debut du document */

	   pDoc->DocViewRootAb[vuelibre - 1] =
	      AbsBoxesCreate (pDoc->DocRootElement, pDoc, vuelibre, TRUE, TRUE, &complet);

	else
	  {
	     /* cree l'image de la meme partie du document que */
	     /* celle affichee dans la fenetre designee par */
	     /* l'utilisateur. */
	     /* prend la selection courante */
	     sel = SelEditeur (&SelDoc, &PremSel, &DerSel, &PremCar, &DerCar);
	     selprinc = FALSE;
	     if (sel)
		if (SelDoc == pDoc && PremSel->ElAssocNum == 0)
		   selprinc = TRUE;
	     if (selprinc)
		/* la selection courante est dans l'une des vues */
		/* de l'arbre principal du document concerne', */
		/* on cree l'image abstraite de la vue avec */
		/* l'element selectionne' au milieu */
		VerifAbsBoxe (PremSel, vuelibre, pDoc, FALSE, FALSE);
	     else
	       {
		  pAb = pDoc->DocRootElement->ElAbstractBox[vuedesignee - 1];
		  if (pAb == NULL)
		     coupetete = FALSE;
		  else if (pAb->AbLeafType != LtCompound)
		     coupetete = FALSE;
		  else if (pAb->AbInLine)
		     coupetete = FALSE;
		  else
		     coupetete = pAb->AbTruncatedHead;
		  if (!coupetete)
		     /* la vue designee commence au debut du */
		     /* document, on cree la nouvelle image depuis */
		     /* le debut du document */

		     pDoc->DocViewRootAb[vuelibre - 1] = AbsBoxesCreate (pDoc->DocRootElement,
				      pDoc, vuelibre, TRUE, TRUE, &complet);

		  else
		    {
		       /* cherche dans la vue designee le premier pave 
		          dont le debut n'est pas coupe' */
		       stop = FALSE;
		       do
			  if (pAb == NULL)
			     stop = TRUE;
			  else
			    {
			       if (pAb->AbLeafType != LtCompound)
				  coupetete = FALSE;
			       else if (pAb->AbInLine)
				  coupetete = FALSE;
			       else
				  coupetete = pAb->AbTruncatedHead;
			       if (!coupetete)
				  stop = TRUE;
			       else
				  pAb = pAb->AbFirstEnclosed;
			    }
		       while (!(stop));

		       /* cree la nouvelle vue a partir de cet element */
		       if (pAb == NULL)
			  VerifAbsBoxe (pDoc->DocRootElement, vuelibre, pDoc, TRUE, FALSE);
		       else
			  VerifAbsBoxe (pAb->AbElement, vuelibre, pDoc, TRUE, FALSE);

		    }
	       }
	  }
     }
   return retour;
}


/* ---------------------------------------------------------------------- */
/* |    OuvreVueCreee ouvre une vue dont on a deja cree' l'image        | */
/* |            pDoc: document concerne'.                               | */
/* |            vue: si Assoc est faux, numero de la vue,               | */
/* |                 si Assoc est vrai, numero des elements associes    | */
/* |                 dont on ouvre la vue.                              | */
/* |            X,Y,L,H: position et dimensions de la fenetre en mm.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                OuvreVueCreee (PtrDocument pDoc, int vue, boolean Assoc, int X, int Y, int L, int H)
#else  /* __STDC__ */
void                OuvreVueCreee (pDoc, vue, Assoc, X, Y, L, H)
PtrDocument         pDoc;
int                 vue;
boolean             Assoc;
int                 X;
int                 Y;
int                 L;
int                 H;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   int                 vol = 0;
   int                 frame;
   int                 h;

   /*boolean     bool; */
   Name                 nom;
   int                 VueSch;

   frame = 0;
   if (vue > 0)
     {
	/* prepare le nom de la vue */
	if (Assoc)
	  {
	     VueSch = 1;
	     pEl = pDoc->DocAssocRoot[vue - 1];
	     strncpy (nom, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName, MAX_NAME_LENGTH);
	  }
	else
	  {
	     VueSch = pDoc->DocView[vue - 1].DvPSchemaView;
	     strncpy (nom, pDoc->DocView[vue - 1].DvSSchema->SsPSchema->PsView[VueSch - 1], MAX_NAME_LENGTH);
	  }
	/* creation d'une frame pour la vue */
	frame = CrFenTitre (pDoc, VueSch, nom, &vol, X, Y, L, H);
     }
   if (frame == 0)
      /* on n'a pas pu creer la fenetre, echec */
     {
	TtaDisplaySimpleMessage (INFO, LIB, LIB_TOO_MANY_VIEWS);
	if (!Assoc)
	   pDoc->DocView[vue - 1].DvPSchemaView = 0;
     }
   else
      /* la fenetre a ete creee correctement, on affiche l'image qui est */
      /* deja prete */
     {
	h = 0;			/* on ne s'occupe pas de la hauteur de page */
	if (Assoc)
	   /* vue d'elements associes */
	  {
	     pDoc->DocAssocFrame[vue - 1] = frame;
	     pDoc->DocAssocVolume[vue - 1] = vol;
	     ChangeConcreteImage (frame, &h, pDoc->DocAssocRoot[vue - 1]->ElAbstractBox[0]);
	     DisplayFrame (frame);
	     VisuSelect (pDoc->DocAssocRoot[vue - 1]->ElAbstractBox[0], TRUE);
	  }
	else
	   /* vue de l'arbre principal */
	  {
	     pDoc->DocViewFrame[vue - 1] = frame;
	     pDoc->DocViewVolume[vue - 1] = vol;
	     ChangeConcreteImage (frame, &h, pDoc->DocViewRootAb[vue - 1]);
	     DisplayFrame (frame);
	     VisuSelect (pDoc->DocViewRootAb[vue - 1], TRUE);
	  }
	/* met a jour les menus de la fenetre */
	if (ThotLocalActions[T_chselect] != NULL)
	   (*ThotLocalActions[T_chselect]) (pDoc);
	if (ThotLocalActions[T_chattr] != NULL)
	   (*ThotLocalActions[T_chattr]) (pDoc);
     }
}

/* ---------------------------------------------------------------------- */
/* |    LaVueExiste recherche par nom d'une vue d'un document.          | */
/* |            -> Nvue : numero vue ou associe.                        | */
/* |            -> Assoc indique si vue elements associes.              | */
/* |            ATTENTION : Ne cherche pas dans les schemas de nature.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      LaVueExiste (PtrDocument pDoc, int numvue)
#else  /* __STDC__ */
static boolean      LaVueExiste (pDoc, numvue)
PtrDocument         pDoc;
int                 numvue;

#endif /* __STDC__ */
{
   int                 vuededocu;
   boolean             existe;

   vuededocu = 1;
   existe = FALSE;
   while (vuededocu <= MAX_VIEW_DOC && !existe)
      if (pDoc->DocView[vuededocu - 1].DvPSchemaView == numvue)
	 existe = TRUE;
      else
	 vuededocu++;

   return existe;
}


/* ---------------------------------------------------------------------- */
/* |    ChercheVueNom recherche la vue de nom Nomvue.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      ChercheVueNom (PtrDocument pDoc, Name Nomvue, int *Nvue, boolean * Assoc, PtrSSchema * pSS)
#else  /* __STDC__ */
static boolean      ChercheVueNom (pDoc, Nomvue, Nvue, Assoc, pSS)
PtrDocument         pDoc;
Name                 Nomvue;
int                *Nvue;
boolean            *Assoc;
PtrSSchema       *pSS;

#endif /* __STDC__ */
{
   int                 v, r, a;
   boolean             existe;
   PtrPSchema          pSc1;
   PtrSSchema        pSchS1;
   SRule              *pRe1;
   boolean             ret;

   ret = FALSE;
   /* vues declarees dans le schema de presentation et non creees */
   pSc1 = pDoc->DocSSchema->SsPSchema;
   for (v = 1; v <= pSc1->PsNViews && !ret; v++)
      if (!LaVueExiste (pDoc, v))
	 if (strcmp (pSc1->PsView[v - 1], Nomvue) == 0)
	   {
	      *Nvue = v;
	      *Assoc = FALSE;
	      *pSS = pDoc->DocSSchema;
	      ret = TRUE;
	   }
   if (!ret)
     {
	/* vues des elements associes non encore creees */
	/* examine les elements associes definis dans le schema de */
	/* structure du document et dans toutes ses extensions */
	pSchS1 = pDoc->DocSSchema;
	do
	  {
	     /* r: derniere regle qui pourrait etre une liste d'elem. assoc. */
	     if (pSchS1->SsFirstDynNature == 0)
		r = pSchS1->SsNRules;
	     else
		r = pSchS1->SsFirstDynNature - 1;
	     do			/* boucle sur les regles a la recherche des listes d'elements associes */
	       {
		  if (pSchS1->SsRule[r - 1].SrConstruct == CsList)
		     if (pSchS1->SsRule[pSchS1->SsRule[r - 1].SrListItem - 1].
			 SrAssocElem)
			if (!pSchS1->SsPSchema->
			    PsInPageHeaderOrFooter[pSchS1->SsRule[r - 1].SrListItem - 1])
			   /* ces elements associes ne sont pas affiches dans une */
			   /* boite de haut ou de bas de page */
			   /* cherche si la vue existe deja */
			  {
			     existe = FALSE;
			     a = 0;
			     do
			       {
				  a++;
				  if (pDoc->DocAssocRoot[a - 1] != NULL)
				     if (pDoc->DocAssocRoot[a - 1]->ElTypeNumber == r)
					if (pDoc->DocAssocFrame[a - 1] > 0)
					   existe = TRUE;
			       }
			     while (!(existe || a >= MAX_ASSOC_DOC));
			     if (!existe)
			       {
				  pRe1 = &pSchS1->SsRule[r - 1];
				  if (strcmp (pRe1->SrName, Nomvue) == 0)
				    {
				       *Nvue = r;
				       *Assoc = TRUE;
				       *pSS = pSchS1;
				       ret = TRUE;
				    }
			       }
			  }
		  r--;
		  /* regle precedente */
	       }
	     while (r > 1 && !ret);
	     pSchS1 = pSchS1->SsNextExtens;
	  }
	while (pSchS1 != NULL && !ret);
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    CreVueNommee cree une vue d'un document par son nom.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 CreVueNommee (PtrDocument pDoc, Name nom, int X, int Y, int L, int H)
#else  /* __STDC__ */
int                 CreVueNommee (pDoc, nom, X, Y, L, H)
PtrDocument         pDoc;
Name                 nom;
int                 X;
int                 Y;
int                 L;
int                 H;

#endif /* __STDC__ */
{
   int                 numvue;
   boolean             assoc;
   int                 vuedoc, vuelibre;
   int                 ret;
   PtrSSchema        pSS;
   NotifyDialog        notifyDoc;

   vuelibre = 0;
   vuedoc = 1;
   ret = 0;
   /* cherche la premiere vue libre dans le descripteur du document */
   while (vuelibre == 0 && vuedoc <= MAX_VIEW_DOC)
      if (pDoc->DocView[vuedoc - 1].DvPSchemaView == 0)
	 vuelibre = vuedoc;
      else
	 vuedoc++;
   if (vuelibre > 0)
     {
	if (!ChercheVueNom (pDoc, nom, &numvue, &assoc, &pSS))
	   ret = 0;
	else
	  {
	     notifyDoc.event = TteViewOpen;
	     notifyDoc.document = (Document) IdentDocument (pDoc);
	     notifyDoc.view = 0;
	     if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	       {
		  ret = CreeImageAbstraite (pDoc, numvue, 0, pSS, 1, assoc, NULL);
		  OuvreVueCreee (pDoc, ret, assoc, X, Y, L, H);
		  notifyDoc.event = TteViewOpen;
		  notifyDoc.document = (Document) IdentDocument (pDoc);
		  if (assoc)
		     notifyDoc.view = ret + 100;
		  else
		     notifyDoc.view = ret;
		  CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	       }
	  }
     }
   else
      ret = 0;
   return ret;
}



/* ---------------------------------------------------------------------- */
/* | TraiteRetMenuVues ouvre effectivement une vue apres les retours    | */
/* |            des menus d'ouverture de Vues                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TraiteRetMenuVues (PtrDocument pDoc, int VdView, PtrElement SousArbreVueAOuvrir, DocViewNumber VueDeReference)

#else  /* __STDC__ */
void                TraiteRetMenuVues (pDoc, VdView, SousArbreVueAOuvrir, VueDeReference)
PtrDocument         pDoc;
int                 VdView;
PtrElement          SousArbreVueAOuvrir;
DocViewNumber           VueDeReference;

#endif /* __STDC__ */

{

   int                 X, Y, W, H;
   int                 NumeroVueAOuvrir;
   int                 vue;
   NotifyDialog        notifyDoc;

   if (VdView != -1)
      /* une vue a ete choisie */
     {
	notifyDoc.event = TteViewOpen;
	notifyDoc.document = (Document) IdentDocument (pDoc);
	notifyDoc.view = 0;
	if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	  {
	     NumeroVueAOuvrir = EntreesMenuVues[VdView];

	     /* cherche la geometrie de la vue dans le fichier .conf */
	     ConfigGetViewGeometry (pDoc,
			  ToutesLesVuesAOuvrir[NumeroVueAOuvrir - 1].VdViewName,
				    &X, &Y, &W, &H);
	     /* cree effectivement la vue */
	     if (ToutesLesVuesAOuvrir[NumeroVueAOuvrir - 1].VdAssoc)
	       {
		  vue = CreeImageAbstraite (pDoc, 0, ToutesLesVuesAOuvrir[NumeroVueAOuvrir - 1].VdAssocNum,
					    ToutesLesVuesAOuvrir[NumeroVueAOuvrir - 1].VdSSchema, VueDeReference,
					    TRUE, SousArbreVueAOuvrir);
		  OuvreVueCreee (pDoc, vue, TRUE, X, Y, W, H);
		  vue += 100;
	       }
	     else
	       {
		  vue = CreeImageAbstraite (pDoc, ToutesLesVuesAOuvrir[NumeroVueAOuvrir - 1].VdView, 0,
					    ToutesLesVuesAOuvrir[NumeroVueAOuvrir - 1].VdSSchema, VueDeReference,
					    FALSE, SousArbreVueAOuvrir);
		  OuvreVueCreee (pDoc, vue, FALSE, X, Y, W, H);
	       }
	     notifyDoc.event = TteViewOpen;
	     notifyDoc.document = (Document) IdentDocument (pDoc);
	     notifyDoc.view = vue;
	     CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    MenuVuesAOuvrir construit le menu des vues qu'il est possible   | */
/* |    d'ouvrir pour le document pDoc.                                 | */
/* |    Buf: buffer pour le texte du menu.                              | */
/* |    Au retour nbitem indique le nombre d'items dans le menu.        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                MenuVuesAOuvrir (PtrDocument pDoc, char *Buf, int *nbitem)

#else  /* __STDC__ */
void                MenuVuesAOuvrir (pDoc, Buf, nbitem)
PtrDocument         pDoc;
char               *Buf;
int                *nbitem;

#endif /* __STDC__ */

{
   int                 i, j, longueur;
   int                 nbvues;
   DocViewNumber           vuedoc;
   DocViewNumber           vuelibre;
   DocumentView           *pVu1;

   if (pDoc != NULL)
     {
	/* cherche la premiere vue libre dans le descripteur du document */
	vuedoc = 1;
	vuelibre = 0;
	while (vuelibre == 0 && vuedoc <= MAX_VIEW_DOC)
	   if (pDoc->DocView[vuedoc - 1].DvPSchemaView == 0)
	      vuelibre = vuedoc;
	   else
	      vuedoc++;
	/* Si (vuelibre == 0) il n'y a plus de place pour une vue de */
	/* l'arbre principal */

	/* cree le catalogue des vues qu'il est possible de creer */
	i = 0;			/* i: index courant dans le buffer du catalogue */
	*nbitem = 0;		/* nbitem: nombre d'entrees dans le catalogue */
	nbvues = LesVuesDunDoc (pDoc, ToutesLesVuesAOuvrir);
	for (j = 1; j <= nbvues; j++)
	  {
	     pVu1 = &ToutesLesVuesAOuvrir[j - 1];
	     /* Si une vue Assoc n'est pas ouverte ou s'il reste des vues */
	     /* principales libres... */
	     /* Si le document est en lecture seule, on ne propose */
	     /* pas d'ouvrir une vue pour des elements associes qui */
	     /* n'existent pas (ce qui reviendrait a les creer) */
	     if ((pVu1->VdAssoc && !pVu1->VdOpen &&
		  (!pDoc->DocReadOnly || pVu1->VdExist))
		 || (!pVu1->VdAssoc && (vuelibre > 0)))
	       {
		  (*nbitem)++;
		  /* L'entree nbitem du menu est l'entree j dans ToutesLesVuesAOuvrir. */
		  EntreesMenuVues[*nbitem - 1] = j;
		  longueur = strlen (pVu1->VdViewName) + 1;
		  if (longueur + i < MAX_TXT_LEN)
		    {
		       strcpy (Buf + i, pVu1->VdViewName);
		       i += longueur;
		    }
		  if (pVu1->VdOpen)
		    {
		       /* Marque par une etoile a la fin du nom que la vue est deja ouverte */
		       Buf[i - 1] = '*';
		       Buf[i] = '\0';
		       i++;
		    }
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    FermerVueDoc ferme la vue de numero nv du document pDoc, ou le  | */
/* |    document complet s'il s'agit de la derniere vue de ce document. | */
/* |    Si assoc est vrai, nv un numero d'elements associe's            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FermerVueDoc (PtrDocument pDoc, int nv, boolean assoc)

#else  /* __STDC__ */
void                FermerVueDoc (pDoc, nv, assoc)
PtrDocument         pDoc;
int                 nv;
boolean             assoc;

#endif /* __STDC__ */

{
   boolean             ok;
   boolean             Sauver;
   NotifyDialog        notifyDoc;

   if (pDoc != NULL)
     {
	notifyDoc.event = TteViewClose;
	notifyDoc.document = (Document) IdentDocument (pDoc);
	if (assoc)
	   notifyDoc.view = nv + 100;
	else
	   notifyDoc.view = nv;
	if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	  {
	     if (NbVueExiste (pDoc) <= 1)
	       {
		  /* On va detruire la derniere vue du document, on laisse a */
		  /* l'utilisateur la possibilite de sauver le document */
		  if (pDoc->DocModified)
		    {
		       ok = TRUE;
		       if (ThotLocalActions[T_confirmclose] != NULL)
			 {
			    (*ThotLocalActions[T_confirmclose]) (pDoc, &ok, &Sauver);
			    if (Sauver)
			      {
				 if (DocOfSavedElements == pDoc)
				    LibBufEditeur ();
				 ok = SauveDocument (pDoc, 0);
			      }
			 }
		    }
		  else
		     ok = TRUE;
	       }
	     else
		ok = TRUE;
	     if (ok)
	       {
		  /* desactive la vue si elle est active */
		  DesactVue (pDoc, nv, assoc);
		  /* fait detruire la fenetre par le mediateur */
		  if (assoc)
		     DestroyFrame (pDoc->DocAssocFrame[nv - 1]);
		  else
		     DestroyFrame (pDoc->DocViewFrame[nv - 1]);
		  notifyDoc.event = TteViewClose;
		  notifyDoc.document = (Document) IdentDocument (pDoc);
		  if (assoc)
		     notifyDoc.view = nv + 100;
		  else
		     notifyDoc.view = nv;
		  CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		  /* detruit le contexte de la vue */
		  detruit (pDoc, nv, assoc, TRUE);
	       }
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    FermerVue ferme une vue d'un document, ou un document complet   | */
/* |            s'il s'agit de la derniere vue de ce document.          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FermerVue ()

#else  /* __STDC__ */
void                FermerVue ()
#endif				/* __STDC__ */

{
   PtrDocument         pDoc;
   int                 nv;
   boolean             assoc;

   SelectVue (&pDoc, &nv, &assoc);
   FermerVueDoc (pDoc, nv, assoc);
}
