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
   Module de gestion des vues des documents

 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmenu.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "appdialogue.h"
#include "fileaccess.h"
#include "thotdir.h"
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "modif_tv.h"
#include "page_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"


#include "absboxes_f.h"
#include "actions_f.h"
#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributeapi_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "closedoc_f.h"
#include "config_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "draw_f.h"
#include "fileaccess_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "paginate_f.h"
#include "platform_f.h"
#include "presvariables_f.h"
#include "references_f.h"
#include "search_f.h"
#include "searchref_f.h"
#include "structschema_f.h"
#include "schemas_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structcommands_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "viewcommands_f.h"
#include "writepivot_f.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

static AvailableView AllViews;
static int          ViewMenuItem[MAX_VIEW_OPEN];

/*----------------------------------------------------------------------
   PaginatedView rend vrai si la vue Vue du document pDoc est une	
   		vue paginee						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            PaginatedView (PtrDocument pDoc, int view, ThotBool assoc)
#else  /* __STDC__ */
ThotBool            PaginatedView (pDoc, view, assoc)
PtrDocument         pDoc;
int                 view;
ThotBool            assoc;
#endif /* __STDC__ */

{
   ThotBool            paginate;
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


/*----------------------------------------------------------------------
   GetViewFromFrame retourne le pointeur sur le numero de vue (viewNum)      
   dans le document pDoc, correspondant a`                 	
   la fenetre de numero nframe. Si c'est une frame         	
   d'elements associes, rend assoc vrai et viewNum = numero	
   d'element associe, sinon rend assoc faux.               	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetViewFromFrame (int nframe, PtrDocument pDoc, int *viewNum, ThotBool * assoc)
#else  /* __STDC__ */
void                GetViewFromFrame (nframe, pDoc, viewNum, assoc)
int                 nframe;
PtrDocument         pDoc;
int                *viewNum;
ThotBool           *assoc;

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

/*----------------------------------------------------------------------
   GetDocAndView retourne le pointeur sur le document (pDoc) et le	
   numero de vue (viewNum) dans ce document, correspondant a	
   la fenetre de numero nframe. Si c'est une fenetre		
   d'elements associes, rend assoc vrai et viewNum = numero	
   d'element associe, sinon rend assoc faux.			
   Rend pDoc = NULL si la selection a echoue.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetDocAndView (int frame, PtrDocument * pDoc, int *viewNum, ThotBool * assoc)
#else  /* __STDC__ */
void                GetDocAndView (frame, pDoc, viewNum, assoc)
int                 nframe;
PtrDocument        *pDoc;
int                *viewNum;
ThotBool           *assoc;

#endif /* __STDC__ */
{
   PtrDocument         pD;

   *pDoc = NULL;
   *viewNum = 0;
   pD = NULL;
   if (FrameTable[frame].FrDoc != 0)
     {
	pD = LoadedDocument[FrameTable[frame].FrDoc - 1];
	if (pD != NULL)
	  {
	     /* il y a un document pour cette entree de la table des documents */
	     GetViewFromFrame (frame, pD, viewNum, assoc);
	  }
     }
   if (*viewNum == 0)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_MISSING_VIEW);
   else
      *pDoc = pD;
}

/*----------------------------------------------------------------------
   BuildSSchemaViewList						
   	Construit la liste des vues possibles d'un document.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BuildSSchemaViewList (PtrDocument pDoc, PtrSSchema pSS, AvailableView viewList, int *nViews, ThotBool nature)
#else  /* __STDC__ */
static void         BuildSSchemaViewList (pDoc, pSS, viewList, nViews, nature)
PtrDocument         pDoc;
PtrSSchema          pSS;
AvailableView       viewList;
int                *nViews;
ThotBool            nature;

#endif /* __STDC__ */
{
   PtrPSchema          pPSchema;
   DocViewDescr       *pView;
   SRule              *pSRule;
   int                 view, i;
   ThotBool            open, viewOK, present;

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
			    ustrncpy (viewList[*nViews].VdViewName, pPSchema->PsView[view], MAX_NAME_LENGTH);
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

/*----------------------------------------------------------------------
   BuildNatureList	cree la liste des natures du schema		
   	de structure pSS.						
  ----------------------------------------------------------------------*/

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
   SRule              *pSRule;
   int                 rule;

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
		   BuildSSchemaViewList (pDoc, pSRule->SrSSchemaNat, viewList, nViews, TRUE);

		   /* les vues des natures contenues dans cette nature */
		   BuildNatureList (pSRule->SrSSchemaNat, nViews, viewList, pDoc);
		}
     }
}

/*----------------------------------------------------------------------
   BuildDocumentViewList	construit la liste des vues definies pour	
   	le document pDoc: vues de l'arbre principal, vues des elements	
   	assoocies et vues des natures.                                  
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 BuildDocumentViewList (PtrDocument pDoc, AvailableView viewList)

#else  /* __STDC__ */
int                 BuildDocumentViewList (pDoc, viewList)
PtrDocument         pDoc;
AvailableView       viewList;

#endif /* __STDC__ */

{
   PtrSSchema          pSS;
   SRule              *pSRule;
   int                 a, rule, nViews;
   ThotBool            assocPres, present;

   nViews = 0;
   if (pDoc->DocSSchema != NULL)
     {
	/* vues du schema de presentation de l'arbre principal */
	BuildSSchemaViewList (pDoc, pDoc->DocSSchema, viewList, &nViews, FALSE);
	/* vues definies pour les extensions du schema du document */
	pSS = pDoc->DocSSchema->SsNextExtens;
	while (pSS != NULL)
	  {
	     BuildSSchemaViewList (pDoc, pSS, viewList, &nViews, FALSE);
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
		      BuildSSchemaViewList (pDoc, pSRule->SrSSchemaNat, viewList,
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
			     viewList[nViews].VdAssocNum = rule + 1;
			     ustrncpy (viewList[nViews].VdViewName,
				 pSS->SsRule[rule].SrName, MAX_NAME_LENGTH);
			     viewList[nViews].VdSSchema = pSS;
			     viewList[nViews].VdOpen = present;
			     viewList[nViews].VdAssoc = TRUE;
			     viewList[nViews].VdExist = assocPres;
			     viewList[nViews].VdNature = FALSE;
			     viewList[nViews].VdPaginated =
				pSS->SsPSchema->PsAssocPaginated[rule + 1];
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


/*----------------------------------------------------------------------
   ChangeDocumentName change le nom d'un document pDoc en newName	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeDocumentName (PtrDocument pDoc, STRING newName)
#else  /* __STDC__ */
void                ChangeDocumentName (pDoc, newName)
PtrDocument         pDoc;
STRING              newName;

#endif /* __STDC__ */
{
   DocViewDescr       *pView;
   int                 len, view;
   CHAR_T                buffer[MAX_TXT_LEN];

   len = 0;
   ustrcpy (buffer, newName);
   ustrncpy (pDoc->DocDName, newName, MAX_NAME_LENGTH);
   pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
   ustrncpy (pDoc->DocIdent, newName, MAX_DOC_IDENT_LEN);
   pDoc->DocIdent[MAX_DOC_IDENT_LEN - 1] = EOS;
   len = ustrlen (newName);
   if (ustrcmp (newName + len - 4, ".PIV") == 0)
     {
	buffer[len - 4] = EOS;
	pDoc->DocDName[len - 4] = EOS;
	pDoc->DocIdent[len - 4] = EOS;
     }
   ustrcat (buffer, TEXT("  "));
   len = ustrlen (buffer);
   /* traite les vues de l'arbre principal */
   for (view = 0; view < MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view].DvPSchemaView > 0)
	 /* met dans le buffer le nom de la vue */
	{
	   pView = &pDoc->DocView[view];
	   ChangeFrameTitle (pDoc->DocViewFrame[view], buffer);
	}
   /* traite les vues des elements associes */
   for (view = 0; view < MAX_ASSOC_DOC; view++)
      if (pDoc->DocAssocRoot[view] != NULL)
	 if (pDoc->DocAssocFrame[view] != 0)
	    /* met dans le buffer le nom des elements associes */
	   {
	      ustrncpy (&buffer[len], pDoc->DocAssocRoot[view]->ElStructSchema->SsRule[pDoc->DocAssocRoot[view]->ElTypeNumber - 1].SrName,
		       MAX_NAME_LENGTH);
	      /* change le titre de la fenetre */
	      ChangeFrameTitle (pDoc->DocAssocFrame[view], buffer);
	   }
}


/*----------------------------------------------------------------------
   TtaSetDocumentName

   Sets or changes the name of a document. The document must be loaded.

   Parameters:
   document: the document whose name is set.
   documentName: new document name. This is only the name, without any
   suffix, without directory name. See function TtaSetDocumentDirectory
   for changing the directory of a document.
   The name must not exceed 31 characters.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetDocumentName (Document document, STRING documentName)
#else  /* __STDC__ */
void                TtaSetDocumentName (document, documentName)
Document            document;
STRING              documentName;
#endif /* __STDC__ */

{
   UserErrorCode = 0;
   /* verifies the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   /* parameter document is correct */
   else if (ustrlen (documentName) >= MAX_NAME_LENGTH)
     TtaError (ERR_buffer_too_small);
   else
     ChangeDocumentName (LoadedDocument[document - 1], documentName);
}

/*----------------------------------------------------------------------
   ViewClosed	libere tous les paves de la vue correspondant a 	
   la fenetre de numero nframe. Appele lorsque l'utilisateur	
   	      ferme une fenetre.					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ViewClosed (int nFrame)

#else  /* __STDC__ */
void                ViewClosed (nFrame)
int                 nFrame;

#endif /* __STDC__ */

{
   PtrDocument         pDoc;
   NotifyDialog        notifyDoc;
   int                 view;
   ThotBool            assoc;

   /* cherche le document auquel appartient la fenetre detruite */
   GetDocAndView (nFrame, &pDoc, &view, &assoc);
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
	DeactivateView (pDoc, view, assoc);
	/* detruit la fenetre */
	DestroyFrame (nFrame);
	CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	/* detruit le contexte de la vue */
	CloseDocumentView (pDoc, view, assoc, TRUE);
     }
}


/*----------------------------------------------------------------------
   OpenDefaultViews ouvre, pour le document pDoc, toutes les vues	
   qui doivent etre ouvertes a l'ouverture du document.    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                OpenDefaultViews (PtrDocument pDoc)
#else  /* __STDC__ */
void                OpenDefaultViews (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */
{
  Document          document;
  PtrPSchema        pPSchema;
  NotifyDialog      notifyDoc;
  int               view, i, X, Y, width, height, schView;
  ThotBool          bool, skeleton;

  /* si le document a ete charge' sous le forme de ses seuls elements 
     exporte's, on ouvre la vue export sinon, on ouvre la premiere vue. */
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
  document = (Document) IdentDocument (pDoc);
  notifyDoc.event = TteViewOpen;
  notifyDoc.document = document;
  notifyDoc.view = 0;
  if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
    {
      schView = pDoc->DocView[view - 1].DvPSchemaView;
      pDoc->DocViewFrame[0] = CreateWindowWithTitle (pDoc, schView,
						     pDoc->DocSSchema->SsPSchema->PsView[view - 1],
						     &pDoc->DocViewVolume[0], X, Y, width, height);
    }
  if (pDoc->DocViewFrame[0] == 0)
    /* echec creation fenetre */
    {
      UnloadTree (document);
      UnloadDocument (&pDoc);
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_OPENING_NEW_FRAME_IMP);
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
	  pDoc->DocViewRootAb[0] = AbsBoxesCreate (pDoc->DocRootElement,
						   pDoc, 1, TRUE, TRUE, &bool);
	  i = 0;
	  /* on ne s'occupe pas de la hauteur de page */
	  ChangeConcreteImage (pDoc->DocViewFrame[0], &i, pDoc->DocViewRootAb[0]);
	  DisplayFrame (pDoc->DocViewFrame[0]);
	  notifyDoc.event = TteViewOpen;
	  notifyDoc.document = document;
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

/*----------------------------------------------------------------------
   CreateAbstractImage cree l'image abstraite pour une vue du       
   document pDoc. Si v est nul, il                         
   s'agit d'une vue d'elements associes de type r,         
   appartenant au schema de structure pSS; sinon c'est la  
   vue de numero v definie dans le schema de presentation  
   associe' au schema de structure pointe' par pSS.        
   Si viewRoot est NULL, la vue presentera un arbre        
   entier, sinon elle n'affichera que le sous-arbre de     
   racine viewRoot.                                        
   Retourne 0 si echec ou le numero de vue pour le document ou le  
   numero d'element associe' de la vue creee.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 CreateAbstractImage (PtrDocument pDoc, int v, int r, PtrSSchema pSS, int chosenView, ThotBool begin, PtrElement viewRoot)
#else  /* __STDC__ */
int                 CreateAbstractImage (pDoc, v, r, pSS, chosenView, begin, viewRoot)
PtrDocument         pDoc;
int                 v;
int                 r;
PtrSSchema          pSS;
int                 chosenView;
ThotBool            begin;
PtrElement          viewRoot;

#endif /* __STDC__ */
{
   PtrDocument         pSelDoc;
   PtrElement          firstSel, lastSel;
   PtrAbstractBox      pAb;
   NotifyElement       notifyEl;
   int                 view, freeView, assoc, volume, firstChar, lastChar,
                       ret;
   ThotBool            stop, sel, selInMainTree, bool;
   ThotBool            truncHead, assocPresent;

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
		       /* supprime les elements exclus */
		       RemoveExcludedElem (&pDoc->DocAssocRoot[assoc - 1], pDoc);
		       if (pDoc->DocAssocRoot[assoc - 1] != NULL)
			 {
			    pDoc->DocAssocRoot[assoc - 1]->ElAccess = AccessReadWrite;
			    CheckLanguageAttr (pDoc, pDoc->DocAssocRoot[assoc - 1]);
			    /* traitement des exceptions */
			    if (ThotLocalActions[T_createtable] != NULL)
			       (*ThotLocalActions[T_createtable])
				  (pDoc->DocAssocRoot[assoc - 1], pDoc);
			    /* creation d'une table */
			    /* traitement des attributs requis */
			    AttachMandatoryAttributes (pDoc->DocAssocRoot[assoc - 1], pDoc);
			    if (pDoc->DocSSchema != NULL)
			      {
				 /* Ajoute un saut de page a la fin si necessaire */
				 AddLastPageBreak (pDoc->DocAssocRoot[assoc - 1],
						   1, pDoc, TRUE);
			      }
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
	     if (!begin)
	       {
		  /* prend la selection courante */
		  sel = GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar);
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
		CheckAbsBox (firstSel, 1, pDoc, FALSE, FALSE);
	  }
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
	     sel = GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar);
	     selInMainTree = FALSE;
	     if (sel)
		if (pSelDoc == pDoc && firstSel->ElAssocNum == 0)
		   selInMainTree = TRUE;
	     if (selInMainTree)
		/* la selection courante est dans l'une des vues */
		/* de l'arbre principal du document concerne', */
		/* on cree l'image abstraite de la vue avec */
		/* l'element selectionne' au milieu */
		CheckAbsBox (firstSel, freeView, pDoc, FALSE, FALSE);
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
			  CheckAbsBox (pDoc->DocRootElement, freeView, pDoc, TRUE, FALSE);
		       else
			  CheckAbsBox (pAb->AbElement, freeView, pDoc, TRUE, FALSE);

		    }
	       }
	  }
     }
   return ret;
}


/*----------------------------------------------------------------------
   OpenCreatedView ouvre une vue dont on a deja cree' l'image        
   pDoc: document concerne'.                               
   view: si assoc est faux, numero de la vue,              
   si assoc est vrai, numero des elements associes    
   dont on ouvre la vue.                              
   X, Y, width, height: position et dimensions de la	
   		     fenetre en mm.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                OpenCreatedView (PtrDocument pDoc, int view, ThotBool assoc, int X, int Y, int width, int height)
#else  /* __STDC__ */
void                OpenCreatedView (pDoc, view, assoc, X, Y, width, height)
PtrDocument         pDoc;
int                 view;
ThotBool            assoc;
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
   if (view > 0) {
      /* prepare le nom de la vue */
      if (assoc) {
         schView = 1;
         pEl = pDoc->DocAssocRoot[view - 1];
         ustrncpy (viewName, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName, MAX_NAME_LENGTH);
	  } else {
             schView = pDoc->DocView[view - 1].DvPSchemaView;
             ustrncpy (viewName, pDoc->DocView[view - 1].DvSSchema->SsPSchema->PsView[schView - 1], MAX_NAME_LENGTH);
	  } 
      /* creation d'une fenetre pour la vue */
      frame = CreateWindowWithTitle (pDoc, schView, viewName, &volume, X, Y, width, height);
   } 
   if (frame == 0) {
      /* on n'a pas pu creer la fenetre, echec */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_LIB_TOO_MANY_VIEWS);
      if (!assoc)
         pDoc->DocView[view - 1].DvPSchemaView = 0;
   } else {
          /* la fenetre a ete creee correctement, on affiche l'image qui est */
          /* deja prete */
          /* on ne s'occupe pas de la hauteur de page */
#         ifdef _WINDOWS
          WIN_GetDeviceContext (frame);
#         endif /* _WINDOWS */
          h = 0;
          if (assoc) {
             /* vue d'elements associes */
             pDoc->DocAssocFrame[view - 1] = frame;
             pDoc->DocAssocVolume[view - 1] = volume;
             ChangeConcreteImage (frame, &h, pDoc->DocAssocRoot[view - 1]->ElAbstractBox[0]);
             DisplayFrame (frame);
             ShowSelection (pDoc->DocAssocRoot[view - 1]->ElAbstractBox[0], TRUE);
		  } else {
                 /* vue de l'arbre principal */
                 pDoc->DocViewFrame[view - 1] = frame;
                 pDoc->DocViewVolume[view - 1] = volume;
                 ChangeConcreteImage (frame, &h, pDoc->DocViewRootAb[view - 1]);
                 DisplayFrame (frame);
                 ShowSelection (pDoc->DocViewRootAb[view - 1], TRUE);
		  }
          /* Update Paste entry in menu */
          if ((FirstSavedElement == NULL && ClipboardThot.BuLength == 0) || pDoc->DocReadOnly)
             SwitchPaste (pDoc, FALSE);
          else
               SwitchPaste(pDoc, TRUE);

          /* check the Undo state of the document */
          if (pDoc->DocNbEditsInHistory == 0)
             SwitchUndo (pDoc, FALSE);
          else
               SwitchUndo (pDoc, TRUE);

          /* check the Redo state of the document */
          if (pDoc->DocNbUndone == 0)
             SwitchRedo (pDoc, FALSE);
          else
               SwitchRedo (pDoc, TRUE);

          /* met a jour les menus de la fenetre */
          if (ThotLocalActions[T_chselect] != NULL)
             (*ThotLocalActions[T_chselect]) (pDoc);
          if (ThotLocalActions[T_chattr] != NULL)
             (*ThotLocalActions[T_chattr]) (pDoc);
#         ifdef _WINDOWS
          WIN_ReleaseDeviceContext ();
#         endif /* _WINDOWS */
   }
}


/*----------------------------------------------------------------------
   GetViewByName cherche la vue de nom viewName.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     GetViewByName (PtrDocument pDoc, Name viewName, int *view, ThotBool * assoc, PtrSSchema * pSS)
#else  /* __STDC__ */
static ThotBool     GetViewByName (pDoc, viewName, view, assoc, pSS)
PtrDocument         pDoc;
Name                viewName;
int                *view;
ThotBool           *assoc;
PtrSSchema         *pSS;

#endif /* __STDC__ */
{
   PtrPSchema          pPSch;
   PtrSSchema          pSSch;
   SRule              *pSRule;
   int                 viewSch, viewDoc, rule, ass;
   ThotBool            open, present, ret;

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
	   if (ustrcmp (pPSch->PsView[viewSch - 1], viewName) == 0)
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
				     if (pDoc->DocAssocRoot[ass]->ElTypeNumber == rule + 1)
					if (pDoc->DocAssocFrame[ass] > 0)
					   present = TRUE;
				  ass++;
			       }
			     while (!present && ass < MAX_ASSOC_DOC);
			     if (!present)
			       {
				  pSRule = &pSSch->SsRule[rule];
				  if (ustrcmp (pSRule->SrName, viewName) == 0)
				    {
				       *view = rule + 1;
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


/*----------------------------------------------------------------------
   OpenViewByName ouvre la vue de nom viewName			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 OpenViewByName (PtrDocument pDoc, Name viewName, int X, int Y, int width, int height)
#else  /* __STDC__ */
int                 OpenViewByName (pDoc, viewName, X, Y, width, height)
PtrDocument         pDoc;
Name                viewName;
int                 X;
int                 Y;
int                 width;
int                 height;

#endif /* __STDC__ */
{
   PtrSSchema          pSS;
   NotifyDialog        notifyDoc;
   int                 view, freeView, ret;
   ThotBool            assoc;

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
	if (!GetViewByName (pDoc, viewName, &view, &assoc, &pSS))
	   ret = 0;
	else
	  {
	     notifyDoc.event = TteViewOpen;
	     notifyDoc.document = (Document) IdentDocument (pDoc);
	     notifyDoc.view = 0;
	     if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	       {
		  ret = CreateAbstractImage (pDoc, view, 0, pSS, 1, assoc, NULL);
		  OpenCreatedView (pDoc, ret, assoc, X, Y, width, height);
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

/*----------------------------------------------------------------------
   OpenViewByMenu ouvre effectivement une vue apres les retours	
   des menus d'ouverture de Vues                           
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                OpenViewByMenu (PtrDocument pDoc, int menuItem, PtrElement subTree, DocViewNumber selectedView)

#else  /* __STDC__ */
void                OpenViewByMenu (pDoc, menuItem, subTree, selectedView)
PtrDocument         pDoc;
int                 menuItem;
PtrElement          subTree;
DocViewNumber       selectedView;

#endif /* __STDC__ */

{
   NotifyDialog        notifyDoc;
   int                 X, Y, width, height, theView, view;
   ThotBool            viewHasBeenOpen;

   viewHasBeenOpen = TRUE;

   if (menuItem != -1)
      /* une vue a ete choisie dans le menu */
     {
	notifyDoc.event = TteViewOpen;
	notifyDoc.document = (Document) IdentDocument (pDoc);
	notifyDoc.view = 0;
	if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	  {
	     theView = ViewMenuItem[menuItem];

	     /* cherche la geometrie de la vue dans le fichier .conf */
	     ConfigGetViewGeometry (pDoc, AllViews[theView - 1].VdViewName,
				    &X, &Y, &width, &height);
	     /* cree effectivement la vue */
	     if (AllViews[theView - 1].VdAssoc)
	       {
		  view = CreateAbstractImage (pDoc, 0, AllViews[theView - 1].VdAssocNum,
			      AllViews[theView - 1].VdSSchema, selectedView,
					      TRUE, subTree);
                  if (pDoc->DocAssocRoot[view - 1] == NULL)
                    /*** Associated tree creation has been refused. ***/
                    viewHasBeenOpen = FALSE;
                  else
                    {
		       OpenCreatedView (pDoc, view, TRUE, X, Y, width, height);
		       view += 100;
                    }
	       }
	     else
	       {
		  view = CreateAbstractImage (pDoc, AllViews[theView - 1].VdView, 0,
			      AllViews[theView - 1].VdSSchema, selectedView,
					      FALSE, subTree);
		  OpenCreatedView (pDoc, view, FALSE, X, Y, width, height);
	       }
             if (viewHasBeenOpen)
               {
	          notifyDoc.event = TteViewOpen;
	          notifyDoc.document = (Document) IdentDocument (pDoc);
	          notifyDoc.view = view;
	          CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
               }
	  }
     }
}


/*----------------------------------------------------------------------
   BuildViewList construit le menu des vues qu'il est possible	
   d'ouvrir pour le document pDoc.                                 
   buffer: buffer pour le texte du menu.                           
   Au retour nItems indique le nombre d'items dans le menu.        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                BuildViewList (PtrDocument pDoc, STRING buffer, int *nItems)

#else  /* __STDC__ */
void                BuildViewList (pDoc, buffer, nItems)
PtrDocument         pDoc;
STRING              buffer;
int                *nItems;

#endif /* __STDC__ */

{
   int                 i, j, longueur, nViews;
   DocViewNumber       view, freeView;

   if (pDoc != NULL)
     {
	/* cherche la premiere vue libre dans le descripteur du document */
	view = 1;
	freeView = 0;
	while (freeView == 0 && view <= MAX_VIEW_DOC)
	   if (pDoc->DocView[view - 1].DvPSchemaView == 0)
	      freeView = view;
	   else
	      view++;
	/* Si (freeView == 0) il n'y a plus de place pour une vue de */
	/* l'arbre principal */

	/* cree le catalogue des vues qu'il est possible de creer */
	/* i: index courant dans le buffer du menu */
	i = 0;
	/* nItems: nombre d'entrees dans le menu */
	*nItems = 0;
	nViews = BuildDocumentViewList (pDoc, AllViews);
	for (j = 0; j < nViews; j++)
	  {
	     /* Si une vue Assoc n'est pas ouverte ou s'il reste des vues */
	     /* principales libres... */
	     /* Si le document est en lecture seule, on ne propose */
	     /* pas d'ouvrir une vue pour des elements associes qui */
	     /* n'existent pas (ce qui reviendrait a les creer) */
	     if ((AllViews[j].VdAssoc && !AllViews[j].VdOpen &&
		  (!pDoc->DocReadOnly || AllViews[j].VdExist))
		 || (!AllViews[j].VdAssoc && (freeView > 0)))
	       {
		  /* L'entree nItems du menu est l'entree j dans AllViews. */
		  ViewMenuItem[(*nItems)++] = j + 1;
		  longueur = ustrlen (AllViews[j].VdViewName) + 1;
		  if (longueur + i < MAX_TXT_LEN)
		    {
		       ustrcpy (buffer + i, AllViews[j].VdViewName);
		       i += longueur;
		    }
		  if (AllViews[j].VdOpen)
		    {
		       /* Marque par une etoile a la fin du nom que la vue est deja ouverte */
		       buffer[i - 1] = TEXT('*');
		       buffer[i] = EOS;
		       i++;
		    }
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   CloseView ferme la vue de numero viewNb du document pDoc, ou le
   document complet s'il s'agit de la derniere vue de ce document. 
   Si assoc est vrai, viewNb un numero d'elements associe's          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CloseView (PtrDocument pDoc, int viewNb, ThotBool assoc)

#else  /* __STDC__ */
void                CloseView (pDoc, viewNb, assoc)
PtrDocument         pDoc;
int                 viewNb;
ThotBool            assoc;

#endif /* __STDC__ */

{
   NotifyDialog        notifyDoc;
   ThotBool            ok, Save;
   View                view;
   Document            document;

   if (pDoc != NULL)
     {
        document = (Document) IdentDocument (pDoc);
	if (assoc)
	   view = viewNb + 100;
	else
	   view = viewNb;
        
	notifyDoc.event = TteViewClose;
	notifyDoc.document = document;
	notifyDoc.view = view;
	if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	  {
	     if (NumberOfOpenViews (pDoc) <= 1)
	       {
		  /* On va detruire la derniere vue du document, on laisse */
		  /* a l'utilisateur la possibilite de sauver le document */
		  if (pDoc->DocModified)
		    {
		       ok = TRUE;
		       /* Faut-il creer le formulaire TtcCloseDocument */
		       if (ThotLocalActions[T_confirmclose] == NULL)
			 {
			   /* Connecte le traitement de la TtcCloseDocument */
			   TteConnectAction (T_confirmclose, (Proc) AskToConfirm);
			   TteConnectAction (T_rconfirmclose, (Proc) CallbackCloseDocMenu);
			 }
		       (*ThotLocalActions[T_confirmclose]) (pDoc, document, view, &ok, &Save);
		       if (Save)
			 {
			   if (DocOfSavedElements == pDoc &&
			       ThotLocalActions[T_freesavedel])
			     (*ThotLocalActions[T_freesavedel]) ();
			   ok = (*(Func)ThotLocalActions[T_writedocument]) (pDoc, 0);
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
		  DeactivateView (pDoc, viewNb, assoc);
		  /* fait detruire la fenetre par le mediateur */
		  if (assoc)
		     DestroyFrame (pDoc->DocAssocFrame[viewNb - 1]);
		  else
		     DestroyFrame (pDoc->DocViewFrame[viewNb - 1]);
		  notifyDoc.event = TteViewClose;
		  notifyDoc.document = (Document) IdentDocument (pDoc);
		  notifyDoc.view = view;
		  CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		  /* detruit le contexte de la vue */
		  CloseDocumentView (pDoc, viewNb, assoc, TRUE);
	       }
	  }
     }
}
