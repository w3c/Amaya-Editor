/*
   Module de gestion des documents et des vues.

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
#include "applicationapi_f.h"
#include "tree_f.h"
#include "attributes_f.h"
#include "search_f.h"
#include "searchref_f.h"
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

static AvailableView    AllViews;
static int          ViewMenuItem[MAX_VIEW_OPEN];

#ifdef __STDC__
extern void         DisplayFrame (int);

#else
extern void         DisplayFrame ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    docModify positionne le flag modification d'un document a` TRUE.| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                docModify (PtrDocument pDoc, PtrElement pEl)
#else  /* __STDC__ */
void                docModify (pDoc, pEl)
PtrDocument         pDoc;
PtrElement          pEl;

#endif /* __STDC__ */
{
   if (pDoc != NULL)
     {
	pDoc->DocModified = TRUE;
	pDoc->DocNTypedChars += 10;
     }
}


/* ---------------------------------------------------------------------- */
/* |    VueAvecPage rend vrai si la vue Vue du document pDoc est une	| */
/* |		vue paginee						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             VueAvecPage (PtrDocument pDoc, int view, boolean assoc)

#else  /* __STDC__ */
boolean             VueAvecPage (pDoc, view, assoc)
PtrDocument         pDoc;
int                 view;
boolean             assoc;

#endif /* __STDC__ */

{
   boolean             paginate;
   PtrElement          pEl;

   if (assoc)
     {
	pEl = pDoc->DocAssocRoot[view - 1];
	if (pEl == NULL)
	   paginate = FALSE;
	else
	   paginate = pEl->ElStructSchema->SsPSchema->
	      PsAssocPaginated[pEl->ElTypeNumber - 1];
     }
   else
     {
	if (pDoc->DocView[view - 1].DvSSchema != pDoc->DocSSchema)
	   /* ce n'est pas une vue definie par le schema du document, on ne */
	   /* la pagine pas */
	   paginate = FALSE;
	else
	   paginate = pDoc->DocView[view - 1].DvSSchema->SsPSchema->
	      PsPaginatedView[pDoc->DocView[view - 1].DvPSchemaView - 1];
     }
   return paginate;
}


/* ---------------------------------------------------------------------- */
/* |    VueFen retourne le pointeur sur le numero de vue (viewNum)      | */
/* |        dans le document pDoc, correspondant a`                 	| */
/* |        la fenetre de numero nframe. Si c'est une frame         	| */
/* |        d'elements associes, rend assoc vrai et viewNum = numero	| */
/* |        d'element associe, sinon rend assoc faux.               	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                VueFen (int nframe, PtrDocument pDoc, int *viewNum, boolean * assoc)
#else  /* __STDC__ */
void                VueFen (nframe, pDoc, viewNum, assoc)
int                 nframe;
PtrDocument         pDoc;
int                *viewNum;
boolean            *assoc;

#endif /* __STDC__ */
{
   int                 view;

   *viewNum = 0;
   view = 0;
   *assoc = FALSE;
   /* cherche d'abord dans les vues de l'arbre principal */
   do
     {
	if (pDoc->DocView[view].DvPSchemaView > 0 && pDoc->DocViewFrame[view] == nframe)
	   *viewNum = view + 1;
	else
	   view++;
     }
   while (view < MAX_VIEW_DOC && *viewNum == 0);

   if (view >= MAX_VIEW_DOC)
      /* cherche dans les arbres des elements associes */
     {
	view = 0;
	do
	  {
	     if (pDoc->DocAssocFrame[view] == nframe)
	       {
		  *viewNum = view + 1;
		  *assoc = TRUE;
	       }
	     else
		view++;
	  }
	while (view < MAX_ASSOC_DOC && *viewNum == 0);
     }
}


/* ---------------------------------------------------------------------- */
/* |    DocVueFen retourne le pointeur sur le document (pDoc) et le	| */
/* |    numero de vue (viewNum) dans ce document, correspondant a	| */
/* |    la fenetre de numero nframe. Si c'est une fenetre		| */
/* |    d'elements associes, rend assoc vrai et viewNum = numero	| */
/* |    d'element associe, sinon rend assoc faux.			| */
/* |    Rend pDoc = NULL si la selection a echoue.			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DocVueFen (int frame, PtrDocument * pDoc, int *viewNum, boolean * assoc)
#else  /* __STDC__ */
void                DocVueFen (frame, pDoc, viewNum, assoc)
int                 nframe;
PtrDocument        *pDoc;
int                *viewNum;
boolean            *assoc;

#endif /* __STDC__ */
{
   PtrDocument         pD;

   *pDoc = NULL;
   *viewNum = 0;
   if (FrameTable[frame].FrDoc != 0)
     {
	pD = LoadedDocument[FrameTable[frame].FrDoc - 1];
	if (pD != NULL)
	  {
	     /* il y a un document pour cette entree de la table des documents */
	     VueFen (frame, pD, viewNum, assoc);
	  }
     }
   if (*viewNum == 0)
      TtaDisplaySimpleMessage (INFO, LIB, MISSING_VIEW);
   else
      *pDoc = pD;
}


/* ---------------------------------------------------------------------- */
/* |    LesVuesDunSchStr						| */
/* |	Construit la liste des vues possibles d'un document.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         LesVuesDunSchStr (PtrDocument pDoc, PtrSSchema pSS, AvailableView viewList, int *nViews, boolean nature)
#else  /* __STDC__ */
static void         LesVuesDunSchStr (pDoc, pSS, viewList, nViews, nature)
PtrDocument         pDoc;
PtrSSchema          pSS;
AvailableView       viewList;
int                *nViews;
boolean             nature;

#endif /* __STDC__ */
{
   PtrPSchema          pPSchema;
   DocViewDescr       *pView;
   SRule              *pSRule;
   int                 view, i;
   boolean             open, viewOK, present;

   if (pSS != NULL)
     {
	pSRule = &pSS->SsRule[pSS->SsRootElem - 1];
	if (!pSRule->SrRefImportedDoc || pSS->SsExtension)
	  {
	     pPSchema = pSS->SsPSchema;
	     if (nature || pSS->SsExtension)
		view = 1;
	     else
		view = 0;
	     while (view < pPSchema->PsNViews)
	       {
		  if (pDoc->DocExportStructure)
		       /* only Export view */
		       viewOK = pPSchema->PsExportView[view];
		  else
		       /* no Export view */
		       viewOK = !pPSchema->PsExportView[view];
		  if (viewOK)
		    {
		       /* verifie que la vue n'est pas encore dans la liste */
		       present = FALSE;
		       for (i = 0; i < *nViews && !present; i++)
			    present = (viewList[i].VdView == view + 1
				      && viewList[i].VdSSchema->SsCode == pSS->SsCode);
		       if (!present)
			 {
			    /* on met la vue dans la liste */
			    viewList[*nViews].VdView = view + 1;
			    viewList[*nViews].VdAssocNum = 0;
			    strncpy (viewList[*nViews].VdViewName, pPSchema->PsView[view], MAX_NAME_LENGTH);
			    viewList[*nViews].VdSSchema = pSS;
			    viewList[*nViews].VdAssoc = FALSE;
			    viewList[*nViews].VdExist = FALSE;
			    viewList[*nViews].VdNature = nature;
			    viewList[*nViews].VdPaginated = pPSchema->PsPaginatedView[view];
			    open = FALSE;
			    for (i = 0; i < MAX_VIEW_DOC && !open; i++)
			      {
				 pView = &pDoc->DocView[i];
				 if (pView->DvPSchemaView > 0)
				    if (pView->DvSSchema->SsPSchema == pSS->SsPSchema
					&& pView->DvPSchemaView == view + 1)
				       open = TRUE;
			      }
			    viewList[*nViews].VdOpen = open;
			    (*nViews)++;
			 }
		    }
		  view++;
	       }
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    BuildNatureList	cree la liste des natures du schema		| */
/* |	de structure pSS.						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         BuildNatureList (PtrSSchema pSS, int *nViews, AvailableView viewList, PtrDocument pDoc)

#else  /* __STDC__ */
static void         BuildNatureList (pSS, nViews, viewList, pDoc)
PtrSSchema          pSS;
int                *nViews;
AvailableView       viewList;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   SRule      *pSRule;
   int         rule;

   /* parcourt les regles de structure */
   for (rule = 0; rule < pSS->SsNRules; rule++)
     {
	pSRule = &pSS->SsRule[rule];
	if (pSRule->SrConstruct == CsNatureSchema)
	   /* c'est une regle de nature */
	   if (pSRule->SrSSchemaNat != NULL)
	      /* la nature est chargee */
	      if (pSRule->SrSSchemaNat->SsNObjects > 0)
		{
		   /* Il existe au moins un objet de cette nature dans le document */

		   /* les vues non principales de cette nature */
		   LesVuesDunSchStr (pDoc, pSRule->SrSSchemaNat, viewList, nViews, TRUE);

		   /* les vues des natures contenues dans cette nature */
		   BuildNatureList (pSRule->SrSSchemaNat, nViews, viewList, pDoc);
		}
     }
}

/* ---------------------------------------------------------------------- */
/* |    LesVuesDunDoc	construit la liste des vues definies pour	| */
/* |	le document pDoc: vues de l'arbre principal, vues des elements	| */
/* |	assoocies et vues des natures.                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 LesVuesDunDoc (PtrDocument pDoc, AvailableView viewList)

#else  /* __STDC__ */
int                 LesVuesDunDoc (pDoc, viewList)
PtrDocument         pDoc;
AvailableView       viewList;

#endif /* __STDC__ */

{
   PtrSSchema        pSS;
   SRule             *pSRule;
   int               a, rule, nViews;
   boolean           assocPres, present;

   nViews = 0;
   if (pDoc->DocSSchema != NULL)
     {
	/* vues du schema de presentation de l'arbre principal */
	LesVuesDunSchStr (pDoc, pDoc->DocSSchema, viewList, &nViews, FALSE);
	/* vues definies pour les extensions du schema du document */
	pSS = pDoc->DocSSchema->SsNextExtens;
	while (pSS != NULL)
	  {
	     LesVuesDunSchStr (pDoc, pSS, viewList, &nViews, FALSE);
	     pSS = pSS->SsNextExtens;
	  }
	/* vues des natures contenues dans le document */
	BuildNatureList (pDoc->DocSSchema, &nViews, viewList, pDoc);
	/* vues des natures contenues dans les natures contenues dans le */
	/* document */
	pSS = pDoc->DocSSchema;
	/* parcourt les regles de structure du schema du document */
	for (rule = 0; rule < pSS->SsNRules; rule++)
	  {
	     pSRule = &pSS->SsRule[rule];
	     if (pSRule->SrConstruct == CsNatureSchema)
		/* c'est une regle de nature */
		if (pSRule->SrSSchemaNat != NULL)
		   /* la nature est chargee */
		   if (pSRule->SrSSchemaNat->SsNObjects > 0)
		      /* il existe au moins un objet de cette nature dans
			 le document */
		      LesVuesDunSchStr (pDoc, pSRule->SrSSchemaNat, viewList,
					&nViews, TRUE);
	  }

	/* vues des elements associes */
	/* On prend d'abord les elements associes definis dans le schema de */
	/* structure du document, puis ceux definis dans les extensions de */
	/* ce schema */
	pSS = pDoc->DocSSchema;
	do
	  {
	     if (pSS->SsFirstDynNature == 0)
		/* rule: derniere regle qui pourrait etre une liste d'elem. assoc. */
		rule = pSS->SsNRules;
	     else
		rule = pSS->SsFirstDynNature - 1;
	     do
	       {
		  rule--;
		  if (pSS->SsRule[rule].SrConstruct == CsList)
		     if (pSS->SsRule[pSS->SsRule[rule].SrListItem - 1].
			 SrAssocElem)
			/* c'est une regle liste d'elements associes */
			if (!pSS->SsPSchema->
			    PsInPageHeaderOrFooter[pSS->SsRule[rule].SrListItem - 1])
			   /* ces elements associes ne sont pas affiches dans une */
			   /* boite de haut ou de bas de page */
			  {
			     /* cherche s'il existe dans le document des elements */
			     /* associes de ce type et si leur vue est deja ouverte */
			     assocPres = FALSE;
			     present = FALSE;
			     a = 0;
			     do
			       {
				  if (pDoc->DocAssocRoot[a] != NULL)
				     if (pDoc->DocAssocRoot[a]->ElTypeNumber == rule + 1)
					if (pDoc->DocAssocRoot[a]->ElStructSchema == pSS)
					   /* il existe des elements associes de ce type */
					  {
					     assocPres = TRUE;
					     if (pDoc->DocAssocFrame[a] != 0)
						/* la vue est ouverte */
						present = TRUE;
					  }
				  a++;
			       }
			     while (!assocPres && a < MAX_ASSOC_DOC);

			     if (present)
				viewList[nViews].VdView = a;
			     else
				viewList[nViews].VdView = 0;
			     viewList[nViews].VdAssocNum = rule+1;
			     strncpy (viewList[nViews].VdViewName,
				   pSS->SsRule[rule].SrName, MAX_NAME_LENGTH);
			     viewList[nViews].VdSSchema = pSS;
			     viewList[nViews].VdOpen = present;
			     viewList[nViews].VdAssoc = TRUE;
			     viewList[nViews].VdExist = assocPres;
			     viewList[nViews].VdNature = FALSE;
			     viewList[nViews].VdPaginated =
				pSS->SsPSchema->PsAssocPaginated[rule+1];
			     nViews++;
			  }
	       }
	     while (rule > 1);
	     /* passe a l'extension de schema suivante */
	     pSS = pSS->SsNextExtens;
	  }
	while (pSS != NULL);
     }
   return nViews;
}

/* ---------------------------------------------------------------------- */
/* |    PaginerDoc	pagine toutes les vues du document pDoc		| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                PaginerDoc (PtrDocument pDoc)

#else  /* __STDC__ */
void                PaginerDoc (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   AvailableView       viewList;
   int                 i, nViews, docView;
   boolean             found;

   nViews = LesVuesDunDoc (pDoc, viewList);
   for (i = 0; i < nViews; i++)
      if (viewList[i].VdOpen)
	 if (viewList[i].VdPaginated)
	    if (!viewList[i].VdNature)
	       /* c'est une vue a paginer */
	      {
		 if (viewList[i].VdAssoc)
		    /* c'est une vue d'elements associes, */
		    /* utilise le numero d'element assoc. */
		    docView = viewList[i].VdView;
		 else
		   {
		      /* cherche le numero de vue dans le document */
		      found = FALSE;
		      for (docView = 0; docView < MAX_VIEW_DOC && !found; docView++)
			   if (pDoc->DocView[docView].DvPSchemaView ==
			       viewList[i].VdView
			       && pDoc->DocView[docView].DvSSchema ==
			       viewList[i].VdSSchema)
			      found = TRUE;
		   }
		 /* pagine la vue */
		 Pages (pDoc, docView+1, viewList[i].VdAssoc);
	      }
}

/* ---------------------------------------------------------------------- */
/* |    LibDocument libere le document pDoc				| */
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
	d = 0;
	while (LoadedDocument[d] != *pDoc && d < MAX_DOCUMENTS - 1)
	   d++;
	if (LoadedDocument[d] == *pDoc)
	  {
	     /* enleve la selection de ce document */
	     DeSelDoc (*pDoc);
	     /* libere le contenu du buffer s'il s'agit d'une partie de ce docum. */
	     if (DocOfSavedElements == *pDoc)
		LibBufEditeur ();
	     /* libere tous les arbres abstraits */
	     DeleteAllTrees (*pDoc);
	     /* libere les schemas */
	     LibSchemas (*pDoc);
	     FreeDocument (LoadedDocument[d]);
	     LoadedDocument[d] = NULL;
	     *pDoc = NULL;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    NumberOfOpenViews retourne le nombre de vues qui existent pour	| */
/* |            le document pDoc					| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static int                 NumberOfOpenViews (PtrDocument pDoc)
#else  /* __STDC__ */
static int                 NumberOfOpenViews (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 view, assoc, result;

   result = 0;
   /* compte les vues de l'arbre principal */
   for (view = 0; view < MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view].DvPSchemaView > 0)
	 result++;
   /* compte les vues des elements associes */
   for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
      if (pDoc->DocAssocFrame[assoc] > 0)
	 result++;
   return result;
}

/* ---------------------------------------------------------------------- */
/* |    dest1vue libere les paves et le contexte de la vue view du	| */
/* |            document pDoc.						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                dest1vue (PtrDocument pDoc, DocViewNumber view)
#else  /* __STDC__ */
void                dest1vue (pDoc, view)
PtrDocument         pDoc;
DocViewNumber           view;

#endif /* __STDC__ */
{
   view--;
   if (pDoc->DocViewRootAb[view] != NULL)
      LibAbbView (pDoc->DocViewRootAb[view]);
   pDoc->DocViewRootAb[view] = NULL;
   pDoc->DocView[view].DvSSchema = NULL;
   pDoc->DocView[view].DvPSchemaView = 0;
   pDoc->DocView[view].DvSync = FALSE;
   pDoc->DocViewFrame[view] = 0;
   pDoc->DocViewVolume[view] = 0;
   pDoc->DocViewFreeVolume[view] = 0;
   pDoc->DocViewSubTree[view] = NULL;
}

/* ---------------------------------------------------------------------- */
/* |    CloseDocumentView detruit la vue de numero view (si assoc est	| */
/* |		faux) pour le document pDoc. S'il s'agit de la derniere	| */
/* |            vue, libere le document dans le cas seulement ou        | */
/* |            closeDoc est vrai. Si assoc est vrai, detruit la vue	| */
/* |            des elements associes de numero view du document.	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                CloseDocumentView (PtrDocument pDoc, int view, boolean assoc, boolean closeDoc)
#else  /* __STDC__ */
void                CloseDocumentView (pDoc, view, assoc, closeDoc)
PtrDocument         pDoc;
int                 view;
boolean             assoc;
boolean             closeDoc;

#endif /* __STDC__ */
{
   NotifyDialog        notifyDoc;

   if (pDoc != NULL)
      /* on detruit la vue */
     {
	if (!assoc)
	   dest1vue (pDoc, view);
	else
	  {
	     LibAbbView (pDoc->DocAssocRoot[view - 1]->ElAbstractBox[0]);
	     pDoc->DocAssocFrame[view - 1] = 0;
	  }
	if (closeDoc)
	   /* verifie qu'il reste au moins une vue pour ce document */
	   if (NumberOfOpenViews (pDoc) < 1)
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
/* |    SetAbsBoxAccessMode met a` jour le mode d'acces accessMode sur  | */
/* |            le pave pAb et tous ses descendants.                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         SetAbsBoxAccessMode (PtrAbstractBox pAb, int accessMode)
#else  /* __STDC__ */
static void         SetAbsBoxAccessMode (pAb, accessMode)
PtrAbstractBox             pAb;
int                 accessMode;

#endif /* __STDC__ */
{
   PtrAbstractBox             pAbChild;

   if (accessMode == 0)
     /* read only */
     {
	pAb->AbCanBeModified = FALSE;
	pAb->AbReadOnly = TRUE;
	pAb->AbChange = TRUE;
     }
   else
      /* read write */
      /* on laisse en read only si l'element est en read only */
      if (!ElementIsReadOnly (pAb->AbElement))
     {
	if (!pAb->AbPresentationBox)
	   /* ce n'est pas un pave de presentation, il est donc modifiable */
	   pAb->AbCanBeModified = TRUE;
	else if (PavPresentModifiable (pAb))
	   pAb->AbCanBeModified = TRUE;
	pAb->AbReadOnly = FALSE;
	pAb->AbChange = TRUE;
     }
   /* on passe aux fils */
   pAbChild = pAb->AbFirstEnclosed;
   while (pAbChild != NULL)
     {
	SetAbsBoxAccessMode (pAbChild, accessMode);
	pAbChild = pAbChild->AbNext;
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
   int                 view, assoc;

   /* met a jour les vues de l'arbre principal */
   for (view = 0; view < MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view].DvPSchemaView > 0)
	{
	   SetAbsBoxAccessMode (pDoc->DocRootElement->ElAbstractBox[view], accessMode);
	   pDoc->DocViewModifiedAb[view] = pDoc->DocRootElement->ElAbstractBox[view];
	}
   /* met a jour les vues des elements associes */
   for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
      if (pDoc->DocAssocFrame[assoc] > 0)
	{
	   SetAbsBoxAccessMode (pDoc->DocAssocRoot[assoc]->ElAbstractBox[0], accessMode);
	   pDoc->DocAssocModifiedAb[assoc] = pDoc->DocAssocRoot[assoc]->ElAbstractBox[0];
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
   PtrElement          pChild, pNext;
   PtrTextBuffer       pBuf, pNextBuf;
   int                 view;
   boolean             ToCreate[MAX_VIEW_DOC];

   /* conserve la liste des vues ou l'element a des paves */
   if (!AssocView (pEl))
      for (view = 0; view < MAX_VIEW_DOC; view++)
	 ToCreate[view] = pEl->ElAbstractBox[view] != NULL;
   else
      /* vue d'elements associes */
      ToCreate[0] = pEl->ElAbstractBox[0] != NULL;
   /* detruit les paves de l'element */
   DetrPaves (pEl, pDoc, FALSE);
   AbstractImageUpdated (pDoc);
   if (pEl->ElTerminal)
      switch (pEl->ElLeafType)
	    {
	       case LtPicture:
	       case LtText:
		  pBuf = pEl->ElText;
		  while (pBuf != NULL)
		    {
		       pNextBuf = pBuf->BuNext;
		       FreeBufTexte (pBuf);
		       pBuf = pNextBuf;
		    }
		  pEl->ElText = NULL;
		  pEl->ElTextLength = 0;
		  break;
	       case LtPlyLine:
		  pBuf = pEl->ElPolyLineBuffer;
		  while (pBuf != NULL)
		    {
		       pNextBuf = pBuf->BuNext;
		       FreeBufTexte (pBuf);
		       pBuf = pNextBuf;
		    }
		  pEl->ElPolyLineBuffer = NULL;
		  pEl->ElNPoints = 0;
		  pEl->ElVolume = 0;
		  pEl->ElPolyLineType = '\0';
		  break;
	       case LtSymbol:
	       case LtGraphics:
		  pEl->ElGraph = '\0';
		  break;
	       default:
		  break;
	    }
   else
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL)
	  {
	     pNext = pChild->ElNext;
	     DeleteElement (&pChild);
	     pChild = pNext;
	  }
     }
   /* effectue une nouvelle copie */
   CopyIncludedElem (pEl, pDoc);
   /* cree les paves de la nouvelle copie dans les vues ou il y avait */
   /* deja des paves */
   if (!AssocView (pEl))
     {
	for (view = 0; view < MAX_VIEW_DOC; view++)
	   if (ToCreate[view])
	     {
		pDoc->DocViewFreeVolume[view] = pDoc->DocViewVolume[view];
		CrPaveNouv (pEl, pDoc, view+1);
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
/* |    UpdateAllInclusions met a` jour tous les elements inclus d'un	| */
/* |		document.						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         UpdateAllInclusions (PtrDocument pDoc)
#else  /* __STDC__ */
static void         UpdateAllInclusions (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrReference        pRef;
   PtrReferredDescr    pRefD;
   PtrDocument         pRefDoc;
   PtrExternalDoc      pExtDoc;
   boolean             setSelect;

   setSelect = FALSE;
   /* parcourt la chaine des descripteurs d'elements reference's */
   pRefD = pDoc->DocReferredEl;
   if (pRefD != NULL)
      /* saute le premier descripteur bidon */
      pRefD = pRefD->ReNext;
   while (pRefD != NULL)
      /* on ne considere que les elements reference's internes au document */
     {
	if (!pRefD->ReExternalRef)
	  {
	     pRefDoc = NULL;
	     pRef = NULL;
	     pExtDoc = NULL;
	     /* cherche toutes les references a cet element qui se trouvent
	        dans un document charge' dans l'editeur */
	     do
	       {

		  pRef = NextReferenceToEl (pRefD->ReReferredElem, pDoc,
				FALSE, pRef, &pRefDoc, &pExtDoc, TRUE);
		  if (pRef != NULL)
		     if (pRef->RdTypeRef == RefInclusion)
			/* c'est une inclusion */
			if (pRef->RdElement != NULL)
			   if (pRef->RdElement->ElSource != NULL)
			      /* c'est une inclusion avec expansion */
			      if (pRefDoc != pDoc)
				 /* l'element inclus est dans un autre doc */
				 /* on traite cette inclusion */
				{
				   /* eteint la selection si ca n'a pas deja */
				   /* ete fait */
				   if (!setSelect)
				     {
					ClearAllViewSelection ();
					setSelect = TRUE;
				     }
				   /* refait la copie de l'element inclus */
				   MajElInclus (pRef->RdElement, pRefDoc);
				}
	       }
	     while (pRef != NULL);
	  }
	/* passe au descripteur d'element reference' suivant */
	if (pRefD != NULL)
	   pRefD = pRefD->ReNext;
     }
   if (setSelect)
      /* rallume la selection */
      AllumeSelection (FALSE, FALSE);
}


/* ---------------------------------------------------------------------- */
/* |    changenomdoc change le nom d'un document pDoc en newName	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                changenomdoc (PtrDocument pDoc, char *newName)
#else  /* __STDC__ */
void                changenomdoc (pDoc, newName)
PtrDocument         pDoc;
char               *newName;

#endif /* __STDC__ */
{
   DocViewDescr        *pView;
   int                 len, view;
   char                buffer[MAX_TXT_LEN];

   len = 0;
   strcpy (buffer, newName);
   strncpy (pDoc->DocDName, newName, MAX_NAME_LENGTH);
   strncpy (pDoc->DocIdent, newName, MAX_DOC_IDENT_LEN);
   len = strlen (newName);
   if (strcmp (newName + len - 4, ".PIV") == 0)
     {
	buffer[len - 4] = '\0';
	pDoc->DocDName[len - 4] = '\0';
	pDoc->DocIdent[len - 4] = '\0';
     }
   strcat (buffer, "  ");
   len = strlen (buffer);
   /* traite les vues de l'arbre principal */
   for (view = 0; view < MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view].DvPSchemaView > 0)
	 /* met dans le buffer le nom de la vue */
	{
	   pView = &pDoc->DocView[view];
	   strncpy (&buffer[len], pView->DvSSchema->SsPSchema->PsView[pView->DvPSchemaView - 1], MAX_NAME_LENGTH);
	   ChangeTitre (pDoc->DocViewFrame[view], buffer);
	}
   /* traite les vues des elements associes */
   for (view = 0; view < MAX_ASSOC_DOC; view++)
      if (pDoc->DocAssocRoot[view] != NULL)
	 if (pDoc->DocAssocFrame[view] != 0)
	    /* met dans le buffer le nom des elements associes */
	   {
	      strncpy (&buffer[len], pDoc->DocAssocRoot[view]->ElStructSchema->SsRule[pDoc->DocAssocRoot[view]->ElTypeNumber - 1].SrName,
		       MAX_NAME_LENGTH);
	      /* change le titre de la fenetre */
	      ChangeTitre (pDoc->DocAssocFrame[view], buffer);
	   }
}

/* ---------------------------------------------------------------------- */
/* |    DestVue	libere tous les paves de la vue correspondant a 	| */
/* |          la fenetre de numero nframe. Appele lorsque l'utilisateur	| */
/* |	      ferme une fenetre.					| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DestVue (int nFrame)

#else  /* __STDC__ */
void                DestVue (nFrame)
int                 nFrame;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   NotifyDialog        notifyDoc;
   int                 view;
   boolean             assoc;

   /* cherche le document auquel appartient la fenetre detruite */
   DocVueFen (nFrame, &pDoc, &view, &assoc);
   if (pDoc != NULL)
     {
	notifyDoc.event = TteViewClose;
	notifyDoc.document = (Document) IdentDocument (pDoc);
	if (assoc)
	   notifyDoc.view = view + 100;
	else
	   notifyDoc.view = view;
	CallEventType ((NotifyEvent *) & notifyDoc, TRUE);
	/* desactive la vue si elle est active */
	DesactVue (pDoc, view, assoc);
	/* detruit la fenetre */
	DestroyFrame (nFrame);
	CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	/* detruit le contexte de la vue */
	CloseDocumentView (pDoc, view, assoc, TRUE);
     }
}


/* ---------------------------------------------------------------------- */
/* | RedisplayExternalRefs cherche, pour tous les elements du document	| */
/* |            pDoc qui sont designes par des references, toutes les   | */
/* |            references appartenant a` d'autres documents charges et | */
/* |            fait reafficher ces references si elles sont deja       | */
/* |            affichees.                                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         RedisplayExternalRefs (PtrDocument pDoc)

#else  /* __STDC__ */
static void         RedisplayExternalRefs (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrReferredDescr    pDescElRef;
   PtrReference        pRef;
   PtrDocument         pRefDoc;
   PtrExternalDoc      pExtDoc;
   PtrElement          pEl;

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
		pRefDoc = NULL;
		pExtDoc = NULL;
		/* on n'a pas encore trouve' de reference */
		do
		   /* cherche la reference suivant a l'element */
		  {

		     pRef = NextReferenceToEl (pEl, pDoc, FALSE, pRef,
					       &pRefDoc, &pExtDoc, TRUE);
		     if (pRef != NULL)
			/* on a trouve' une reference a cet element */
			if (pRefDoc != pDoc)
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
				      CopyIncludedElem (pRef->RdElement, pRefDoc);
			     ReafReference (pRef, NULL, pRefDoc);
			  }
		  }
		while (pRef != NULL);
	     }
	/* passe au descripteur d'element reference' suivant */
	pDescElRef = pDescElRef->ReNext;
     }
}

/* ---------------------------------------------------------------------- */
/* | OuvreVuesInit ouvre, pour le document pDoc, toutes les vues	| */
/* |            qui doivent etre ouvertes a l'ouverture du document.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                OuvreVuesInit (PtrDocument pDoc)

#else  /* __STDC__ */
void                OuvreVuesInit (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPSchema          pPSchema;
   NotifyDialog        notifyDoc;
   int                 view, i, X, Y, width, height, schView;
   boolean	       bool, skeleton;

   /* si le document a ete charge' sous le forme de ses seuls elements 
      exporte's, on ouvre la vue EXPORT, sinon, on ouvre la premiere vue. */
   skeleton = FALSE;
   if (pDoc->DocExportStructure)
     {
	pPSchema = pDoc->DocSSchema->SsPSchema;
	view = 0;
	do
	   view++;
	while (view != pPSchema->PsNViews && !pPSchema->PsExportView[view - 1]);
	if (!pPSchema->PsExportView[view - 1])
	   view = 1;
	else
	   skeleton = TRUE;
     }
   else
      view = 1;
   /* demande la creation d'une fenetre pour la vue a ouvrir */
   /* chercher la geometrie de la fenetre dans le fichier .conf */
   ConfigGetViewGeometry (pDoc, pDoc->DocSSchema->SsPSchema->PsView[view - 1],
			  &X, &Y, &width, &height);
   notifyDoc.event = TteViewOpen;
   notifyDoc.document = (Document) IdentDocument (pDoc);
   notifyDoc.view = 0;
   if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
     {
	schView = pDoc->DocView[view - 1].DvPSchemaView;
	pDoc->DocViewFrame[0] = CrFenTitre (pDoc, schView,
			    pDoc->DocSSchema->SsPSchema->PsView[view - 1],
			    &pDoc->DocViewVolume[0], X, Y, width, height);
     }
   if (pDoc->DocViewFrame[0] == 0)
      /* echec creation fenetre */
     {
	LibDocument (&pDoc);
	TtaDisplaySimpleMessage (INFO, LIB, OPENING_NEW_FRAME_IMP);
     }
   else
     {
	pDoc->DocView[0].DvSSchema = pDoc->DocSSchema;
	pDoc->DocView[0].DvPSchemaView = view;
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
	     if (TypeBPage (pDoc->DocRootElement->ElFirstChild, view, &pSchPage) != 0)
		/* document pagine, on initialise NbPages et VolLibre */
	       {
		  pDoc->DocViewNPages[0] = 0;
		  pDoc->DocViewFreeVolume[0] = THOT_MAXINT;
	       }
#endif /* __COLPAGE__ */
	     pDoc->DocViewRootAb[0] = AbsBoxesCreate (pDoc->DocRootElement,
						pDoc, 1, TRUE, TRUE, &bool);
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
/* |            fileName dans le descripteur pointe par pDoc. Au	| */
/* |            retour pDoc est NIL si le document n'a pas pu etre      | */
/* |            charge.                                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                LoadDocument (PtrDocument * pDoc, char *fileName)
#else  /* __STDC__ */
void                LoadDocument (pDoc, fileName)
PtrDocument        *pDoc;
char               *fileName;

#endif /* __STDC__ */
{
   PathBuffer          directoryBuffer;
#ifdef __COLPAGE__
   FILE               *list;
   PtrPSchema          pPagePsch;
#endif /* __COLPAGE__ */
   int                 i, j, len;
   boolean             ok;

   directoryBuffer[0] = '\0';
   if (fileName != NULL)
      /* nom de document fourni a l'appel, on le recopie dans DefaultDocumentName */
     {
	len = strlen (fileName);
	if (len > 4)
	   if (strcmp (fileName + len - 4, ".PIV") == 0)
	      fileName[len - 4] = '\0';
	if (fileName[0] != DIR_SEP)
	  {
	     if (fileName != DefaultDocumentName)
		strncpy (DefaultDocumentName, fileName, MAX_NAME_LENGTH);
	     /* nom de document relatif */
	     strncpy ((*pDoc)->DocDName, DefaultDocumentName, MAX_NAME_LENGTH);
	     strncpy ((*pDoc)->DocIdent, DefaultDocumentName, MAX_DOC_IDENT_LEN);
	     if ((*pDoc)->DocDirectory[0] == '\0')
		strncpy ((*pDoc)->DocDirectory, DocumentPath, MAX_PATH);
	  }
	else
	  {
	     /* nom absolu */
	     i = 0;
	     j = 0;
	     while (fileName[i] != '\0' && i < MAX_PATH - 1)
	       {
		  (*pDoc)->DocDirectory[i] = fileName[i];
		  if ((*pDoc)->DocDirectory[i] == DIR_SEP)
		     j = i;
		  i++;
	       }
	     (*pDoc)->DocDirectory[j+1] = '\0';
	     i = 0;
	     while (fileName[i] != '\0' && i < MAX_NAME_LENGTH - 1)
	       {
		  DefaultDocumentName[i] = fileName[j+1];
		  i++;
		  j++;
	       }
	     DefaultDocumentName[i] = '\0';
	     strncpy ((*pDoc)->DocDName, DefaultDocumentName, MAX_NAME_LENGTH);
	     strncpy ((*pDoc)->DocIdent, DefaultDocumentName, MAX_DOC_IDENT_LEN);
	     /* sauve le path des documents avant de l'ecraser */
	     strncpy (directoryBuffer, DocumentPath, MAX_PATH);
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
	if (directoryBuffer[0] != '\0')
	   strncpy (DocumentPath, directoryBuffer, MAX_PATH);
	if (!ok)
	  {
	     TtaDisplayMessage (INFO, TtaGetMessage(LIB, OPEN_DOC_IMP), DefaultDocumentName);
	     LibDocument (pDoc);
	     *pDoc = NULL;
	  }
     }
   if (*pDoc != NULL)
     {
	/* conserve le path actuel des schemas dans le contexte du document */
	strncpy ((*pDoc)->DocSchemasPath, SchemaPath, MAX_PATH);
	/* ouvre les vues a ouvrir */
	OuvreVuesInit (*pDoc);
	if ((*pDoc)->DocRootElement != NULL)
	   /* Pour tous les elements du document que l'on vient de */
	   /* charger qui sont designe's par des references, cherche */
	   /* toutes les references appartenant a d'autres documents */
	   /* charges et fait reafficher ces references si elles sont */
	   /* deja affichees */
	   RedisplayExternalRefs (*pDoc);
     }
}

/* ---------------------------------------------------------------------- */
/* |    NewDocument cree un document vide, conforme au schema de nom    | */
/* |            SSchemaName, dans le descripteur pointe' par pDoc.      | */
/* |            docName est le nom a donner au document                 | */
/* |            directory est le directory ou il faut creer le document | */
/* |            Au retour pDoc est NIL si le document n'a pas ete cree. | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                NewDocument (PtrDocument * pDoc, PtrBuffer SSchemaName, Name docName, PathBuffer directory)
#else  /* __STDC__ */
void                NewDocument (pDoc, SSchemaName, docName, directory)
PtrDocument        *pDoc;
PtrBuffer           SSchemaName;
Name                docName;
PathBuffer          directory;

#endif /* __STDC__ */
{ 
   PtrElement          pEl;
   NotifyDialog        notifyDoc;
   Name                PSchemaName, docNameBuffer, docType;
   PathBuffer          directoryBuffer;
   PathBuffer          fileNameBuffer;
#ifdef __COLPAGE__
   PtrPSchema          pPagePsch;
#else  /* __COLPAGE__ */
   PtrPSchema          pPSchema;
   int                 view;
#endif /* __COLPAGE__ */
   int                 i;

   if (*pDoc != NULL)
      if (SSchemaName == NULL || SSchemaName[0] == '\0')
	 /* L'utilisateur n'a pas fourni de nom de schema */
	 LibDocument (pDoc);
      else
	{
	   strncpy ((*pDoc)->DocDirectory, DocumentPath, MAX_PATH);
	   /* si c'est un path, retient seulement le 1er directory */
	   i = 0;
	   while ((*pDoc)->DocDirectory[i] != '\0' &&
		  (*pDoc)->DocDirectory[i] != PATH_SEP && i < MAX_PATH - 1)
	      i++;
	   (*pDoc)->DocDirectory[i] = '\0';
	   /* on suppose que le mon de schema est dans la langue de */
	   /* l'utilisateur: on le traduit en nom interne */
	   ConfigNomInterneSSchema ((char *) SSchemaName, docType, TRUE);
	   if (docType[0] == '\0')
	      /* ce nom n'est pas dans le fichier langue, on le prend */
	      /* tel quel */
	      strncpy (docType, (char *) SSchemaName, MAX_NAME_LENGTH);
	   /* compose le nom du fichier a ouvrir avec le nom du directory */
	   /* des schemas... */
	   strncpy (directoryBuffer, SchemaPath, MAX_PATH);
	   BuildFileName (docType, "STR", directoryBuffer, fileNameBuffer, &i);
	   /* teste si le fichier '.STR' existe */

	   if (FileExist (fileNameBuffer) == 0)
	     {
		strncpy (fileNameBuffer, docType, MAX_NAME_LENGTH);
		strcat (fileNameBuffer, ".STR");
		TtaDisplayMessage (INFO, TtaGetMessage(LIB, SCHEMA_NOT_FIND), fileNameBuffer);
	     }
	   else
	     {
		/* charge le schema de structure et le schema de presentation */
		PSchemaName[0] = '\0';
		/* pas de preference pour un schema de */
		/* presentation particulier */
		LoadSchemas (docType, PSchemaName, &((*pDoc)->DocSSchema), NULL, FALSE);
		if (docName[0] != '\0')
		   strncpy (docNameBuffer, docName, MAX_NAME_LENGTH);
		else
		  {
		     strncpy (docNameBuffer, (char *) SSchemaName, MAX_NAME_LENGTH);
		     strcat (docNameBuffer, "X");
		  }
		if ((*pDoc)->DocSSchema != NULL)
		   if ((*pDoc)->DocSSchema->SsPSchema != NULL)
		     {
			notifyDoc.event = TteDocCreate;
			notifyDoc.document = (Document) IdentDocument (*pDoc);
			notifyDoc.view = 0;
			if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
			  {
			     /* cree la representation interne d'un document vide */
			     (*pDoc)->DocRootElement = NewSubtree ((*pDoc)->DocSSchema->SsRootElem,
				(*pDoc)->DocSSchema, *pDoc, 0, TRUE, TRUE,
				TRUE, TRUE);
			     /* supprime les elements exclus (au sens SGML) */
			     RemoveExcludedElem (&((*pDoc)->DocRootElement));
			  }
		     }
	     }
	   if ((*pDoc)->DocRootElement == NULL)
	      /* echec creation document */
	      LibDocument (pDoc);
	   else
	     {

		(*pDoc)->DocRootElement->ElAccess = AccessReadWrite;
		CheckLanguageAttr (*pDoc, (*pDoc)->DocRootElement);
#ifdef __COLPAGE__
		/* il n'est plus necessaire d'ajouter un saut de page */
		/* a la fin de l'arbre principal */
#else  /* __COLPAGE__ */
		/* ajoute un saut de page a la fin de l'arbre principal */
		/* pour toutes les vues qui sont mises en page */
		/* schema de presentation du document */
		pPSchema = (*pDoc)->DocSSchema->SsPSchema;
		/* examine toutes les vues definies dans le schema */
		for (view = 0; view < pPSchema->PsNViews; view++)
		   if (pPSchema->PsPaginatedView[view])
		      /* cette vue est mise en page */
		      AjoutePageEnFin ((*pDoc)->DocRootElement, view+1, *pDoc, TRUE);
#endif /* __COLPAGE__ */
		/* le document appartient au directory courant */
		if (directory[0] != '\0')
		   strncpy (directoryBuffer, directory, MAX_PATH);
		else
		  {
		     strncpy (directoryBuffer, DocumentPath, MAX_PATH);
		     /* si c'est un path, retient seulement le 1er directory */
		     i = 0;
		     while (directoryBuffer[i ] != '\0' &&
			    directoryBuffer[i] != PATH_SEP && i < MAX_PATH - 1)
			i++;
		     directoryBuffer[i] = '\0';
		  }
		DoFileName (docNameBuffer, "PIV", directoryBuffer, fileNameBuffer, &i);
		strncpy ((*pDoc)->DocDName, docNameBuffer, MAX_NAME_LENGTH);
		strncpy ((*pDoc)->DocIdent, docNameBuffer, MAX_NAME_LENGTH);
		/* le document appartient au directory courant */
		strncpy ((*pDoc)->DocDirectory, directoryBuffer, MAX_PATH);
		/* conserve le path actuel des schemas dans le contexte du
		   document */
		strncpy ((*pDoc)->DocSchemasPath, SchemaPath, MAX_PATH);
		notifyDoc.event = TteDocCreate;
		notifyDoc.document = (Document) IdentDocument (*pDoc);
		notifyDoc.view = 0;
		CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		/* traitement des attributs requis */
		AttachMandatoryAttributes ((*pDoc)->DocRootElement, *pDoc);
		if ((*pDoc)->DocSSchema != NULL)
		   /* le document n'a pas ete ferme' pendant l'attente */
		   /* des attributs requis */
		  {
		     /* traitement des exceptions */
		    if (ThotLocalActions[T_createtable]!= NULL)
		      (*ThotLocalActions[T_createtable])
			((*pDoc)->DocRootElement, *pDoc);
		     /* ouvre les vues du document cree' */
		     OuvreVuesInit (*pDoc);
		     /* selectionne la 1ere feuille */
		     pEl = FirstLeaf ((*pDoc)->DocRootElement);
		     SelectEl (*pDoc, pEl, TRUE, TRUE);
		  }
	     }
	}
}


/* ---------------------------------------------------------------------- */
/* |    CreeImageAbstraite cree l'image abstraite pour une vue du       | */
/* |            document pDoc. Si v est nul, il                         | */
/* |            s'agit d'une vue d'elements associes de type r,         | */
/* |            appartenant au schema de structure pSS; sinon c'est la  | */
/* |            vue de numero v definie dans le schema de presentation  | */
/* |            associe' au schema de structure pointe' par pSS.        | */
/* |            Si viewRoot est NULL, la vue presentera un arbre        | */
/* |            entier, sinon elle n'affichera que le sous-arbre de     | */
/* |            racine viewRoot.                                        | */
/* |    Retourne 0 si echec ou le numero de vue pour le document ou le  | */
/* |            numero d'element associe' de la vue creee.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 CreeImageAbstraite (PtrDocument pDoc, int v, int r, PtrSSchema pSS, int chosenView, boolean begin, PtrElement viewRoot)
#else  /* __STDC__ */
int                 CreeImageAbstraite (pDoc, v, r, pSS, chosenView, begin, viewRoot)
PtrDocument         pDoc;
int                 v;
int                 r;
PtrSSchema          pSS;
int                 chosenView;
boolean             begin;
PtrElement          viewRoot;

#endif /* __STDC__ */
{
   PtrDocument         pSelDoc;
   PtrElement          firstSel, lastSel;
   PtrAbstractBox      pAb;
   NotifyElement       notifyEl;
   int                 view, freeView, assoc, volume, firstChar, lastChar, ret;
   boolean             stop, sel, selInMainTree, bool;
   boolean             truncHead, assocPresent;
#ifdef __COLPAGE__
   PtrPSchema          pPagePsch;
   FILE               *list;
#endif /* __COLPAGE__ */

   ret = 0;
   freeView = 0;
   assoc = 0;
   assocPresent = FALSE;
   if (v == 0)
      /* c'est une vue pour des elements associes */
     {
	if (r == 0)
	   assocPresent = TRUE;
	else
	  {
	     /* cherche si les elements existent deja */
	     assocPresent = FALSE;
	     assoc = 0;
	     do
	       {
		  if (pDoc->DocAssocRoot[assoc] != NULL)
		     if (pDoc->DocAssocRoot[assoc]->ElStructSchema->SsCode
			 == pSS->SsCode)
			assocPresent = pDoc->DocAssocRoot[assoc]->ElTypeNumber == r;
		  assoc++;
	       }
	     while (!assocPresent && assoc < MAX_ASSOC_DOC);
	  }
     }
   else
     {
	/* c'est une vue de l'arbre principal */
	/* cherche la premiere vue libre dans le descripteur du document */
	freeView = 0;
	view = 0;
	while (freeView == 0 && view < MAX_VIEW_DOC)
	   if (pDoc->DocView[view].DvPSchemaView == 0)
	      freeView = view + 1;
	   else
	      view++;
     }
   /* volume de l'image abstraite a creer */
   volume = 1000;
   if (v == 0)
     /* on cree une vue d'elements associes */
     {
	if (!assocPresent)
	   /* il n'existe pas d'elements associes de ce type */
	  {
	     assoc = 1;
	     /* cherche une entree libre dans la table des */
	     /* arbres d'elements associes du document */
	     stop = FALSE;
	     do
		if (assoc > MAX_ASSOC_DOC)
		   stop = TRUE;
		else if (pDoc->DocAssocRoot[assoc - 1] == NULL)
		   stop = TRUE;
		else
		   assoc++;
	     while (!stop);
	     if (pDoc->DocAssocRoot[assoc - 1] == NULL)
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
		       pDoc->DocAssocRoot[assoc - 1] =
			  NewSubtree (r, pSS, pDoc, assoc, TRUE, TRUE, TRUE, TRUE);
		       /* supprime les elements exclus (au sens SGML) */
		       RemoveExcludedElem (&pDoc->DocAssocRoot[assoc - 1]);
		       if (pDoc->DocAssocRoot[assoc - 1] != NULL)
			 {
			    pDoc->DocAssocRoot[assoc - 1]->ElAccess = AccessReadWrite;
			    CheckLanguageAttr (pDoc, pDoc->DocAssocRoot[assoc - 1]);
			    /* traitement des exceptions */
			    if (ThotLocalActions[T_createtable]!= NULL)
			      (*ThotLocalActions[T_createtable])	
				(pDoc->DocAssocRoot[assoc - 1], pDoc);  
			    /* creation d'une table */
			    /* traitement des attributs requis */
			    AttachMandatoryAttributes (pDoc->DocAssocRoot[assoc - 1], pDoc);
#ifdef __COLPAGE__
			    /* Inutile d'ajouter un saut de page a la fin */
#else  /* __COLPAGE__ */
			    if (pDoc->DocSSchema != NULL)
			      {
				 /* Ajoute un saut de page a la fin si necessaire */
				 AjoutePageEnFin (pDoc->DocAssocRoot[assoc - 1],
						  1, pDoc, TRUE);
			      }
#endif /* __COLPAGE__ */
			    /* envoie l'evenement ElemNew.Post */
			    NotifySubTree (TteElemNew, pDoc, pDoc->DocAssocRoot[assoc - 1], 0);
			 }
		    }
	       }
	  }
	if ((pDoc->DocAssocRoot[assoc - 1] != NULL) &&
	    (assoc <= MAX_ASSOC_DOC) && pDoc->DocSSchema != NULL)
	   /* on construit l'image abstraite des elements associes */
	  {
	     pDoc->DocAssocFrame[assoc - 1] = 0;
	     pDoc->DocAssocSubTree[assoc - 1] = viewRoot;
	     pDoc->DocAssocVolume[assoc - 1] = volume;
	     pDoc->DocAssocFreeVolume[assoc - 1] = pDoc->DocAssocVolume[assoc - 1];
#ifdef __COLPAGE__
	     if (TypeBPage (pDoc->DocAssocRoot[assoc - 1]->ElFirstChild, 1, &pPagePsch) != 0)
		/* document pagine */
	       {
		  pDoc->DocAssocNPages[assoc - 1] = 0;	/* nbpages = 0 */
		  pDoc->DocAssocFreeVolume[assoc - 1] = THOT_MAXINT;
	       }
#endif /* __COLPAGE__ */
	     if (!begin)
	       {
		  /* prend la selection courante */
		  sel = SelEditeur (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar);
		  if (!sel)
		     /* pas de selection, on construit l'image du debut */
		     begin = TRUE;
		  else if (pSelDoc != pDoc || firstSel->ElAssocNum != assoc)
		     /* la selection courante n'est pas dans les */
		     /* elements associes dont on cree l'image */
		     /* on cree l'image abstraite du debut */
		     begin = TRUE;
	       }
	     if (begin)
		pAb = AbsBoxesCreate (pDoc->DocAssocRoot[assoc - 1], pDoc, 1,
					TRUE, TRUE, &bool);
	     else
		/* on cree l'image abstraite autour du premier */
		/* element selectionne' */
		VerifAbsBoxe (firstSel, 1, pDoc, FALSE, FALSE);
	  }
#ifdef __COLPAGE__
	/* sauvegarde de l'image abstraite pour tests */
	list = fopen ("/perles/roisin/debug/totoassoc", "w");
	if (list != NULL)
	  {
	     NumPav (pDoc->DocAssocRoot[assoc - 1]->ElAbstractBox[0]);
	     AffPaves (pDoc->DocAssocRoot[assoc - 1]->ElAbstractBox[0], 2, list);
	     fclose (list);
	  }
#endif /* __COLPAGE__ */
	ret = assoc;
     }
   else
     {
	/* on cree une vue de l'arbre principal du document */
	pDoc->DocViewSubTree[freeView - 1] = viewRoot;
	pDoc->DocView[freeView - 1].DvSSchema = pSS;
	pDoc->DocView[freeView - 1].DvPSchemaView = v;
	pDoc->DocView[freeView - 1].DvSync = TRUE;

	pDoc->DocViewVolume[freeView - 1] = volume;
	pDoc->DocViewFreeVolume[freeView - 1] = pDoc->DocViewVolume[freeView - 1];
#ifdef __COLPAGE__
	if (TypeBPage (pDoc->DocRootElement->ElFirstChild, v, &pPagePsch) != 0)
	   /* document pagine */
	  {
	     pDoc->DocViewNPages[freeView - 1] = 0;	/* nbpages = 0 */
	     pDoc->DocViewFreeVolume[freeView - 1] = THOT_MAXINT;
	  }
#endif /* __COLPAGE__ */
	ret = freeView;

	if (begin)
	   /* la fenetre designee par l'utilisateur contient des */
	   /* elements associes on cree la nouvelle image depuis */
	   /* le debut du document */

	   pDoc->DocViewRootAb[freeView - 1] =
	      AbsBoxesCreate (pDoc->DocRootElement, pDoc, freeView, TRUE, TRUE, &bool);

	else
	  {
	     /* cree l'image de la meme partie du document que */
	     /* celle affichee dans la fenetre designee par */
	     /* l'utilisateur. */
	     /* prend la selection courante */
	     sel = SelEditeur (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar);
	     selInMainTree = FALSE;
	     if (sel)
		if (pSelDoc == pDoc && firstSel->ElAssocNum == 0)
		   selInMainTree = TRUE;
	     if (selInMainTree)
		/* la selection courante est dans l'une des vues */
		/* de l'arbre principal du document concerne', */
		/* on cree l'image abstraite de la vue avec */
		/* l'element selectionne' au milieu */
		VerifAbsBoxe (firstSel, freeView, pDoc, FALSE, FALSE);
	     else
	       {
		  pAb = pDoc->DocRootElement->ElAbstractBox[chosenView - 1];
		  if (pAb == NULL)
		     truncHead = FALSE;
		  else if (pAb->AbLeafType != LtCompound)
		     truncHead = FALSE;
		  else if (pAb->AbInLine)
		     truncHead = FALSE;
		  else
		     truncHead = pAb->AbTruncatedHead;
		  if (!truncHead)
		     /* la vue designee commence au debut du */
		     /* document, on cree la nouvelle image depuis */
		     /* le debut du document */

		     pDoc->DocViewRootAb[freeView - 1] = AbsBoxesCreate (pDoc->DocRootElement,
				      pDoc, freeView, TRUE, TRUE, &bool);

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
				  truncHead = FALSE;
			       else if (pAb->AbInLine)
				  truncHead = FALSE;
			       else
				  truncHead = pAb->AbTruncatedHead;
			       if (!truncHead)
				  stop = TRUE;
			       else
				  pAb = pAb->AbFirstEnclosed;
			    }
		       while (!stop);

		       /* cree la nouvelle vue a partir de cet element */
		       if (pAb == NULL)
			  VerifAbsBoxe (pDoc->DocRootElement, freeView, pDoc, TRUE, FALSE);
		       else
			  VerifAbsBoxe (pAb->AbElement, freeView, pDoc, TRUE, FALSE);

		    }
	       }
	  }
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    OuvreVueCreee ouvre une vue dont on a deja cree' l'image        | */
/* |            pDoc: document concerne'.                               | */
/* |            view: si assoc est faux, numero de la vue,              | */
/* |                 si assoc est vrai, numero des elements associes    | */
/* |                 dont on ouvre la vue.                              | */
/* |            X, Y, width, height: position et dimensions de la	| */
/* |		     fenetre en mm.					| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                OuvreVueCreee (PtrDocument pDoc, int view, boolean assoc, int X, int Y, int width, int height)
#else  /* __STDC__ */
void                OuvreVueCreee (pDoc, view, assoc, X, Y, width, height)
PtrDocument         pDoc;
int                 view;
boolean             assoc;
int                 X;
int                 Y;
int                 width;
int                 height;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   int                 volume = 0;
   int                 frame;
   int                 h;
   Name                viewName;
   int                 schView;

   frame = 0;
   if (view > 0)
     {
	/* prepare le nom de la vue */
	if (assoc)
	  {
	     schView = 1;
	     pEl = pDoc->DocAssocRoot[view - 1];
	     strncpy (viewName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName, MAX_NAME_LENGTH);
	  }
	else
	  {
	     schView = pDoc->DocView[view - 1].DvPSchemaView;
	     strncpy (viewName, pDoc->DocView[view - 1].DvSSchema->SsPSchema->PsView[schView - 1], MAX_NAME_LENGTH);
	  }
	/* creation d'une fenetre pour la vue */
	frame = CrFenTitre (pDoc, schView, viewName, &volume, X, Y, width, height);
     }
   if (frame == 0)
      /* on n'a pas pu creer la fenetre, echec */
     {
	TtaDisplaySimpleMessage (INFO, LIB, LIB_TOO_MANY_VIEWS);
	if (!assoc)
	   pDoc->DocView[view - 1].DvPSchemaView = 0;
     }
   else
      /* la fenetre a ete creee correctement, on affiche l'image qui est */
      /* deja prete */
     {
	/* on ne s'occupe pas de la hauteur de page */
	h = 0;
	if (assoc)
	   /* vue d'elements associes */
	  {
	     pDoc->DocAssocFrame[view - 1] = frame;
	     pDoc->DocAssocVolume[view - 1] = volume;
	     ChangeConcreteImage (frame, &h, pDoc->DocAssocRoot[view - 1]->ElAbstractBox[0]);
	     DisplayFrame (frame);
	     VisuSelect (pDoc->DocAssocRoot[view - 1]->ElAbstractBox[0], TRUE);
	  }
	else
	   /* vue de l'arbre principal */
	  {
	     pDoc->DocViewFrame[view - 1] = frame;
	     pDoc->DocViewVolume[view - 1] = volume;
	     ChangeConcreteImage (frame, &h, pDoc->DocViewRootAb[view - 1]);
	     DisplayFrame (frame);
	     VisuSelect (pDoc->DocViewRootAb[view - 1], TRUE);
	  }
	/* met a jour les menus de la fenetre */
	if (ThotLocalActions[T_chselect] != NULL)
	   (*ThotLocalActions[T_chselect]) (pDoc);
	if (ThotLocalActions[T_chattr] != NULL)
	   (*ThotLocalActions[T_chattr]) (pDoc);
     }
}


/* ---------------------------------------------------------------------- */
/* |    GetViewByName cherche la vue de nom viewName.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      GetViewByName (PtrDocument pDoc, Name viewName, int *view, boolean * assoc, PtrSSchema * pSS)
#else  /* __STDC__ */
static boolean      GetViewByName (pDoc, viewName, view, assoc, pSS)
PtrDocument         pDoc;
Name                viewName;
int                *view;
boolean            *assoc;
PtrSSchema         *pSS;

#endif /* __STDC__ */
{
   PtrPSchema          pPSch;
   PtrSSchema          pSSch;
   SRule              *pSRule;
   int                 viewSch, viewDoc, rule, ass;
   boolean             open, present, ret;

   ret = FALSE;
   /* cherche parmi les vues declarees dans le schema de presentation et
      non ouvertes */
   pPSch = pDoc->DocSSchema->SsPSchema;
   for (viewSch = 1; viewSch <= pPSch->PsNViews && !ret; viewSch++)
      {
      open = FALSE;
      for (viewDoc = 0; viewDoc < MAX_VIEW_DOC && !open; viewDoc++)
         if (pDoc->DocView[viewDoc].DvPSchemaView == viewSch)
	    open = TRUE;
      if (!open)
	 if (strcmp (pPSch->PsView[viewSch - 1], viewName) == 0)
	   {
	      *view = viewSch;
	      *assoc = FALSE;
	      *pSS = pDoc->DocSSchema;
	      ret = TRUE;
	   }
      }
   if (!ret)
     {
	/* cherche parmi les vues des elements associes non ouvertes */
	/* examine les elements associes definis dans le schema de */
	/* structure du document et dans toutes ses extensions */
	pSSch = pDoc->DocSSchema;
	do
	  {
	     /* rule: derniere regle qui pourrait etre une liste d'elements
		associes */
	     if (pSSch->SsFirstDynNature == 0)
		rule = pSSch->SsNRules;
	     else
		rule = pSSch->SsFirstDynNature - 1;
	     /* boucle sur les regles a la recherche des listes d'elements
		associes */
	     while (rule > 1 && !ret)
	       {
		  rule--;
		  if (pSSch->SsRule[rule].SrConstruct == CsList)
		     if (pSSch->SsRule[pSSch->SsRule[rule].SrListItem - 1].
			 SrAssocElem)
			if (!pSSch->SsPSchema->
			    PsInPageHeaderOrFooter[pSSch->SsRule[rule].SrListItem - 1])
			   /* ces elements associes ne sont pas affiches dans une */
			   /* boite de haut ou de bas de page */
			   /* cherche si la vue existe deja */
			  {
			     present = FALSE;
			     ass = 0;
			     do
			       {
				  if (pDoc->DocAssocRoot[ass] != NULL)
				     if (pDoc->DocAssocRoot[ass]->ElTypeNumber == rule+1)
					if (pDoc->DocAssocFrame[ass] > 0)
					   present = TRUE;
				  ass++;
			       }
			     while (!present && ass < MAX_ASSOC_DOC);
			     if (!present)
			       {
				  pSRule = &pSSch->SsRule[rule];
				  if (strcmp (pSRule->SrName, viewName) == 0)
				    {
				       *view = rule+1;
				       *assoc = TRUE;
				       *pSS = pSSch;
				       ret = TRUE;
				    }
			       }
			  }
	       }
	     /* next schema extension */
	     pSSch = pSSch->SsNextExtens;
	  }
	while (pSSch != NULL && !ret);
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    CreVueNommee cree une vue d'un document par son nom.            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 CreVueNommee (PtrDocument pDoc, Name nom, int X, int Y, int L, int height)
#else  /* __STDC__ */
int                 CreVueNommee (pDoc, nom, X, Y, L, height)
PtrDocument         pDoc;
Name                 nom;
int                 X;
int                 Y;
int                 L;
int                 height;

#endif /* __STDC__ */
{
   int                 numvue;
   boolean             assoc;
   int                 view, freeView;
   int                 ret;
   PtrSSchema        pSS;
   NotifyDialog        notifyDoc;

   freeView = 0;
   view = 1;
   ret = 0;
   /* cherche la premiere vue libre dans le descripteur du document */
   while (freeView == 0 && view <= MAX_VIEW_DOC)
      if (pDoc->DocView[view - 1].DvPSchemaView == 0)
	 freeView = view;
      else
	 view++;
   if (freeView > 0)
     {
	if (!GetViewByName (pDoc, nom, &numvue, &assoc, &pSS))
	   ret = 0;
	else
	  {
	     notifyDoc.event = TteViewOpen;
	     notifyDoc.document = (Document) IdentDocument (pDoc);
	     notifyDoc.view = 0;
	     if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	       {
		  ret = CreeImageAbstraite (pDoc, numvue, 0, pSS, 1, assoc, NULL);
		  OuvreVueCreee (pDoc, ret, assoc, X, Y, L, height);
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

   int                 X, Y, W, height;
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
	     NumeroVueAOuvrir = ViewMenuItem[VdView];

	     /* cherche la geometrie de la vue dans le fichier .conf */
	     ConfigGetViewGeometry (pDoc,
			  AllViews[NumeroVueAOuvrir - 1].VdViewName,
				    &X, &Y, &W, &height);
	     /* cree effectivement la vue */
	     if (AllViews[NumeroVueAOuvrir - 1].VdAssoc)
	       {
		  vue = CreeImageAbstraite (pDoc, 0, AllViews[NumeroVueAOuvrir - 1].VdAssocNum,
					    AllViews[NumeroVueAOuvrir - 1].VdSSchema, VueDeReference,
					    TRUE, SousArbreVueAOuvrir);
		  OuvreVueCreee (pDoc, vue, TRUE, X, Y, W, height);
		  vue += 100;
	       }
	     else
	       {
		  vue = CreeImageAbstraite (pDoc, AllViews[NumeroVueAOuvrir - 1].VdView, 0,
					    AllViews[NumeroVueAOuvrir - 1].VdSSchema, VueDeReference,
					    FALSE, SousArbreVueAOuvrir);
		  OuvreVueCreee (pDoc, vue, FALSE, X, Y, W, height);
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
   int                 nViews;
   DocViewNumber           vuedoc;
   DocViewNumber           freeView;
   DocumentView           *pVu1;

   if (pDoc != NULL)
     {
	/* cherche la premiere vue libre dans le descripteur du document */
	vuedoc = 1;
	freeView = 0;
	while (freeView == 0 && vuedoc <= MAX_VIEW_DOC)
	   if (pDoc->DocView[vuedoc - 1].DvPSchemaView == 0)
	      freeView = vuedoc;
	   else
	      vuedoc++;
	/* Si (freeView == 0) il n'y a plus de place pour une vue de */
	/* l'arbre principal */

	/* cree le catalogue des vues qu'il est possible de creer */
	i = 0;			/* i: index courant dans le buffer du catalogue */
	*nbitem = 0;		/* nbitem: nombre d'entrees dans le catalogue */
	nViews = LesVuesDunDoc (pDoc, AllViews);
	for (j = 1; j <= nViews; j++)
	  {
	     pVu1 = &AllViews[j - 1];
	     /* Si une vue Assoc n'est pas ouverte ou s'il reste des vues */
	     /* principales libres... */
	     /* Si le document est en lecture seule, on ne propose */
	     /* pas d'ouvrir une vue pour des elements associes qui */
	     /* n'existent pas (ce qui reviendrait a les creer) */
	     if ((pVu1->VdAssoc && !pVu1->VdOpen &&
		  (!pDoc->DocReadOnly || pVu1->VdExist))
		 || (!pVu1->VdAssoc && (freeView > 0)))
	       {
		  (*nbitem)++;
		  /* L'entree nbitem du menu est l'entree j dans AllViews. */
		  ViewMenuItem[*nbitem - 1] = j;
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
/* |    FermerVueDoc ferme la vue de numero view du document pDoc, ou le  | */
/* |    document complet s'il s'agit de la derniere vue de ce document. | */
/* |    Si assoc est vrai, view un numero d'elements associe's            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                FermerVueDoc (PtrDocument pDoc, int view, boolean assoc)

#else  /* __STDC__ */
void                FermerVueDoc (pDoc, view, assoc)
PtrDocument         pDoc;
int                 view;
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
	   notifyDoc.view = view + 100;
	else
	   notifyDoc.view = view;
	if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	  {
	     if (NumberOfOpenViews (pDoc) <= 1)
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
		  DesactVue (pDoc, view, assoc);
		  /* fait detruire la fenetre par le mediateur */
		  if (assoc)
		     DestroyFrame (pDoc->DocAssocFrame[view - 1]);
		  else
		     DestroyFrame (pDoc->DocViewFrame[view - 1]);
		  notifyDoc.event = TteViewClose;
		  notifyDoc.document = (Document) IdentDocument (pDoc);
		  if (assoc)
		     notifyDoc.view = view + 100;
		  else
		     notifyDoc.view = view;
		  CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		  /* detruit le contexte de la vue */
		  CloseDocumentView (pDoc, view, assoc, TRUE);
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
   int                 view;
   boolean             assoc;

   SelectVue (&pDoc, &view, &assoc);
   FermerVueDoc (pDoc, view, assoc);
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
		  /* envoie l'evenement DocSave.Pre a l'application */
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
		       /* envoie l'evenement DocSave.Post a l'application */
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
   char                buffer[MAX_TXT_LEN];

   if (pDoc == NULL)
      return FALSE;
   DoFileName (pDoc->DocDName, extension, pDoc->DocDirectory, buffer, &i);
   if (simpleSave (pDoc, buffer, FALSE))
     {
	UpdateAllInclusions (pDoc);
	return TRUE;
     }
   else
     {
	TtaDisplayMessage (CONFIRM, TtaGetMessage(LIB, WRITING_IMP), buffer);
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
   char                buffer[MAX_TXT_LEN];
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
	DoFileName (NomDuDocument, "PIV", NomDirectory, buffer, &i);
	/* on teste d'abord le droit d'ecriture sur le .PIV */
	ok = OuvrEcr (buffer) == 0;
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
		UpdateAllInclusions (pDoc);
	  }
	if (!ok)
	  {
	     /* on indique un nom connu de l'utilisateur... */
	     DoFileName (NomDuDocument, "PIV", NomDirectory, buffer, &i);
	     TtaDisplayMessage (CONFIRM, TtaGetMessage(LIB, WRITING_IMP),
					    buffer);
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
		  pDoc->DocModified = FALSE;
		  pDoc->DocNTypedChars = 0;

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
		  DoFileName (pDoc->DocDName, "REF", DirectoryOrig, buffer, &i);
		  RemoveFile (buffer);
		  if (!SauverDansMemeFichier)
		    {
		       if (strcmp (NomDirectory, DirectoryOrig) != 0 &&
			   strcmp (NomDuDocument, pDoc->DocDName) == 0)
			  /* changement de directory sans changement de nom */
			  if (SauveDocAvecMove)
			    {
			       /* deplacer le fichier .EXT dans le nouveau directory */
			       DoFileName (pDoc->DocDName, "EXT", DirectoryOrig, buffer, &i);
			       DoFileName (pDoc->DocDName, "EXT", NomDirectory, NomFichier, &i);
			       rename (buffer, NomFichier);
			       /* detruire l'ancien fichier PIV */
			       DoFileName (pDoc->DocDName, "PIV", DirectoryOrig, buffer, &i);
			       RemoveFile (buffer);
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
				 DoFileName (pDoc->DocDName, "EXT", DirectoryOrig, buffer,
					     &i);
				 DoFileName (NomDuDocument, "EXT", NomDirectory,
					     NomFichier, &i);
				 rename (buffer, NomFichier);
				 /* detruit l'ancien fichier .PIV */
				 DoFileName (pDoc->DocDName, "PIV", DirectoryOrig, buffer,
					     &i);
				 RemoveFile (buffer);
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
   Name                 docName;
   char                directory[MAX_PATH];

   status = FALSE;
   if (pDoc->DocReadOnly)
      /* on ne sauve pas les documents qui sont en lecture seule */
      TtaDisplaySimpleMessage (INFO, LIB, RO_DOC_FORBIDDEN);
   else if (pDoc->DocSSchema == NULL)
      TtaDisplaySimpleMessage (INFO, LIB, EMPTY_DOC_NOT_WRITTEN);
   else
     {
	strncpy (docName, pDoc->DocDName, MAX_NAME_LENGTH);
	/* on prend le directory ou le document a ete lu */
	strncpy (directory, pDoc->DocDirectory, MAX_PATH);
	/* recherche le nom du fichier en proposant le nom courant */
	ok = !ask;
	if (ok && !pDoc->DocReadOnly)
	   status = SauverDoc (pDoc, docName, directory, FALSE, FALSE);
     }
   if (status && ask)
	{
	pDoc->DocModified = FALSE;
	pDoc->DocNTypedChars = 0;
	}
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
