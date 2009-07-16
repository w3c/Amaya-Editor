/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Module de gestion des vues des documents
 *
 */
#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

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
#include "boxes_tv.h"


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
#include "writepivot_f.h"

#include "AmayaParams.h"
#include "appdialogue_wx_f.h"

#ifdef _WX
#include "AmayaFrame.h"
#endif /* _WX */

#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */

static AvailableView AllViews;
static int          ViewMenuItem[MAX_VIEW_OPEN];

/*----------------------------------------------------------------------
   PaginatedView rend vrai si la vue Vue du document pDoc est une	
   		vue paginee						
  ----------------------------------------------------------------------*/
ThotBool            PaginatedView (PtrDocument pDoc, int view)
{
  ThotBool            paginate;
  PtrPSchema          pPS;

  if (pDoc->DocView[view - 1].DvSSchema != pDoc->DocSSchema)
    /* ce n'est pas une vue definie par le schema du document, on ne */
    /* la pagine pas */
    paginate = FALSE;
  else
    {
      pPS = PresentationSchema (pDoc->DocView[view - 1].DvSSchema, pDoc);
      paginate = pPS->PsPaginatedView[pDoc->DocView[view - 1].DvPSchemaView - 1];
    }
  return paginate;
}



/*----------------------------------------------------------------------
   GetDocAndView retourne le pointeur sur le document (pDoc) et le
   numero de vue (viewNum) dans ce document, correspondant a
   la fenetre de numero nframe.
   Rend pDoc = NULL si la selection a echoue.
  ----------------------------------------------------------------------*/
void GetDocAndView (int frame, PtrDocument *pDoc, int *viewNum)
{
   PtrDocument         pD;

   *pDoc = NULL;
   *viewNum = 0;
   pD = NULL;
   if (FrameTable[frame].FrDoc != 0)
     {
	pD = LoadedDocument[FrameTable[frame].FrDoc - 1];
	if (pD != NULL)
	   /* il y a un document pour cette entree de la table des documents */
	   GetViewFromFrame (frame, pD, viewNum);
     }
     *pDoc = pD;
}

/*----------------------------------------------------------------------
  BuildSSchemaViewList
  Construit la liste des vues possibles d'un document.
  ----------------------------------------------------------------------*/
static void BuildSSchemaViewList (PtrDocument pDoc, PtrSSchema pSS,
                                  AvailableView viewList, int *nViews,
				  ThotBool nature)
{
   PtrPSchema          pPSchema, pPS;
   DocViewDescr       *pView;
   PtrSRule            pSRule;
   int                 view, i;
   ThotBool            open, viewOK, present;

   if (pSS != NULL)
      {
      pSRule = pSS->SsRule->SrElem[pSS->SsRootElem - 1];
      if (!pSRule->SrRefImportedDoc || pSS->SsExtension)
	 {
	 pPSchema = PresentationSchema (pSS, pDoc);
	 if (!pPSchema)
	   return;
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
		 present = (viewList[i].VdView == view + 1 &&
			    !strcmp (viewList[i].VdSSchema->SsName, pSS->SsName));
	       if (!present && *nViews < MAX_VIEW_OPEN)
		  {
		  /* on met la vue dans la liste */
		  viewList[*nViews].VdView = view + 1;
		  strncpy (viewList[*nViews].VdViewName,
			    pPSchema->PsView[view], MAX_NAME_LENGTH);
		  viewList[*nViews].VdSSchema = pSS;
		  viewList[*nViews].VdExist = FALSE;
		  viewList[*nViews].VdNature = nature;
		  viewList[*nViews].VdPaginated = pPSchema->PsPaginatedView[view];
		  open = FALSE;
		  for (i = 0; i < MAX_VIEW_DOC && !open; i++)
		     {
		     pView = &pDoc->DocView[i];
		     if (pView->DvPSchemaView > 0)
		       {
			 pPS = PresentationSchema (pView->DvSSchema, pDoc);
			 if (pPS == pPSchema &&
			     pView->DvPSchemaView == view + 1)
			   open = TRUE;
		       }
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
   BuildDocumentViewList construit la liste des vues definies pour
   le document pDoc: vues de l'arbre principal et vues des natures.
  ----------------------------------------------------------------------*/
int BuildDocumentViewList (PtrDocument pDoc, AvailableView viewList)
{
   int                 i, nViews;

   nViews = 0;
   if (pDoc->DocSSchema != NULL)
      {
      BuildDocNatureTable (pDoc);
      for (i = 0; i < pDoc->DocNNatures; i++)
        BuildSSchemaViewList (pDoc, pDoc->DocNatureSSchema[i], viewList,
			      &nViews, FALSE);
      }
   return nViews;
}

/*----------------------------------------------------------------------
   ChangeDocumentName change le nom d'un document pDoc en newName	
  ----------------------------------------------------------------------*/
void ChangeDocumentName (PtrDocument pDoc, char *newName)
{
   DocViewDescr       *pView;
   int                 len, view;
   char                buffer[MAX_TXT_LEN];

   len = 0;
   strcpy (buffer, newName);
   strncpy (pDoc->DocDName, newName, MAX_NAME_LENGTH);
   pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
   len = strlen (newName);
   if (len > 4 && strcmp (newName + len - 4, ".PIV") == 0)
     {
	buffer[len - 4] = EOS;
	pDoc->DocDName[len - 4] = EOS;
     }
   strcat (buffer, "  ");
   len = strlen (buffer);
   /* traite toutes les vues du document */
   for (view = 0; view < MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view].DvPSchemaView > 0)
	 /* met dans le buffer le nom de la vue */
	{
	   pView = &pDoc->DocView[view];
	   ChangeFrameTitle (pDoc->DocViewFrame[view], (unsigned char *)buffer,
			     TtaGetDefaultCharset ());
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
void TtaSetDocumentName (Document document, char *documentName)
{
   UserErrorCode = 0;
   /* verifies the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   /* parameter document is correct */
   else if (strlen (documentName) >= MAX_NAME_LENGTH)
     TtaError (ERR_buffer_too_small);
   else
     ChangeDocumentName (LoadedDocument[document - 1], documentName);
}

/*----------------------------------------------------------------------
   ViewClosed	libere tous les paves de la vue correspondant a 	
   la fenetre de numero nframe. Appele lorsque l'utilisateur	
   	      ferme une fenetre.					
  ----------------------------------------------------------------------*/
void ViewClosed (int nFrame)
{
   PtrDocument         pDoc;
   NotifyDialog        notifyDoc;
   int                 view;

   /* cherche le document auquel appartient la fenetre detruite */
   GetDocAndView (nFrame, &pDoc, &view);
   if (pDoc != NULL)
     {
	notifyDoc.event = TteViewClose;
	notifyDoc.document = (Document) IdentDocument (pDoc);
	notifyDoc.view = view;
	CallEventType ((NotifyEvent *) & notifyDoc, TRUE);
	/* desactive la vue si elle est active */
	DeactivateView (pDoc, view);
	/* detruit la fenetre */
	DestroyFrame (nFrame);
	CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	/* detruit le contexte de la vue */
	CloseDocumentView (pDoc, view, TRUE);
     }
}

/*----------------------------------------------------------------------
   OpenDefaultViews ouvre, pour le document pDoc, toutes les vues	
   qui doivent etre ouvertes a l'ouverture du document.    
  ----------------------------------------------------------------------*/
void OpenDefaultViews (PtrDocument pDoc)
{
  Document          document;
  PtrPSchema        pPSchema;
  PtrSSchema        pSS;
  NotifyDialog      notifyDoc;
  int               view, i, X, Y, width, height, schView;
  ThotBool          b, skeleton;

  /* si le document a ete charge' sous le forme de ses seuls elements 
     exporte's, on ouvre la vue export sinon, on ouvre la premiere vue. */
  skeleton = FALSE;
  pPSchema = PresentationSchema (pDoc->DocSSchema, pDoc);
  pSS = pDoc->DocSSchema;
  if (pDoc->DocExportStructure)
     {
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
  ConfigGetViewGeometry (pDoc, pPSchema->PsView[view - 1],
			 &X, &Y, &width, &height);
  document = (Document) IdentDocument (pDoc);
  notifyDoc.event = TteViewOpen;
  notifyDoc.document = document;
  notifyDoc.view = 0;
  if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
     {
     schView = pDoc->DocView[view - 1].DvPSchemaView;
     pDoc->DocViewFrame[0] = MakeFrame (pSS->SsName, schView, pDoc->DocDName,
					X, Y, width, height,
					&pDoc->DocViewVolume[0],
					IdentDocument (pDoc), TRUE, TRUE);
     }
  if (pDoc->DocViewFrame[0] == 0)
     /* echec creation fenetre */
       CloseDocumentAndViews(pDoc, FALSE);
  else
     {
     pDoc->DocView[0].DvSSchema = pSS;
     pDoc->DocView[0].DvPSchemaView = view;
     pDoc->DocView[0].DvSync = TRUE;
     pDoc->DocView[0].DvFirstGuestView = NULL;
     /* create the guest view list for that view */
     CreateGuestViewList (pDoc, 1);

     pDoc->DocViewFreeVolume[0] = pDoc->DocViewVolume[0];
     /* met a jour les menus variables de la fenetre */
     if (ThotLocalActions[T_chattr] != NULL)
	(*(Proc1)ThotLocalActions[T_chattr]) ((void*)pDoc);
     if (pDoc->DocDocElement != NULL)
	{
	pDoc->DocViewRootAb[0] = AbsBoxesCreate (pDoc->DocDocElement, pDoc,
						 1, TRUE, TRUE, &b);
	i = 0;
	/* on ne s'occupe pas de la hauteur de page */
	ChangeConcreteImage (pDoc->DocViewFrame[0], &i,pDoc->DocViewRootAb[0]);
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
   CreateAbstractImage
   cree l'image abstraite pour une vue du document pDoc.
   C'est la vue de numero v definie dans le schema de presentation  
   associe' au schema de structure pointe' par pSS.        
   Si viewRoot est NULL, la vue presentera un arbre        
   entier, sinon elle n'affichera que le sous-arbre de     
   racine viewRoot.                                        
   Retourne 0 si echec ou le numero de vue pour le document
  ----------------------------------------------------------------------*/
int CreateAbstractImage (PtrDocument pDoc, int v, PtrSSchema pSS,
                         int chosenView, ThotBool begin, PtrElement viewRoot)
{
   PtrDocument         pSelDoc;
   PtrElement          firstSel, lastSel;
   PtrAbstractBox      pAb;
   int                 view, freeView, volume, firstChar, lastChar,
                       ret;
   ThotBool            stop, sel, selInMainTree, b;
   ThotBool            truncHead;

   freeView = 0;
   firstSel = NULL;
   /* cherche la premiere vue libre dans le descripteur du document */
   freeView = 0;
   view = 0;
   while (freeView == 0 && view < MAX_VIEW_DOC)
     if (pDoc->DocView[view].DvPSchemaView == 0)
       freeView = view + 1;
     else
       view++;
   /* volume de l'image abstraite a creer */
   volume = 1000;
   pDoc->DocViewSubTree[freeView - 1] = viewRoot;
   pDoc->DocView[freeView - 1].DvSSchema = pSS;
   pDoc->DocView[freeView - 1].DvPSchemaView = v;
   pDoc->DocView[freeView - 1].DvSync = TRUE;
   pDoc->DocView[freeView - 1].DvFirstGuestView = NULL;
   /* create the guest view list for that view */
   CreateGuestViewList (pDoc, freeView);

   pDoc->DocViewVolume[freeView - 1] = volume;
   pDoc->DocViewFreeVolume[freeView -1] = pDoc->DocViewVolume[freeView-1];
   ret = freeView;

   if (begin)
     /* on cree la nouvelle image depuis le debut du document */
     pDoc->DocViewRootAb[freeView-1] = AbsBoxesCreate (pDoc->DocDocElement,
						       pDoc, freeView, TRUE,
						       TRUE, &b);
   else
     {
       /* cree l'image de la meme partie du document que */
       /* celle affichee dans la fenetre designee par l'utilisateur. */
       /* prend la selection courante */
       sel = GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
				  &lastChar);
       selInMainTree = FALSE;
       if (firstSel)
	 if (pSelDoc == pDoc)
	   selInMainTree = TRUE;
       if (selInMainTree)
	 /* la selection courante est dans l'une des vues */
	 /* de l'arbre principal du document concerne', */
	 /* on cree l'image abstraite de la vue avec */
	 /* l'element selectionne' au milieu */
	 CheckAbsBox (firstSel, freeView, pDoc, FALSE, FALSE);
       else
	 {
	   pAb = pDoc->DocDocElement->ElAbstractBox[chosenView - 1];
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
	     pDoc->DocViewRootAb[freeView - 1] = AbsBoxesCreate (pDoc->DocDocElement,
					   pDoc, freeView, TRUE, TRUE, &b);
	   else
	     {
	       /* cherche dans la vue designee le premier pave dont le debut
		  n'est pas coupe' */
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
		 CheckAbsBox (pDoc->DocDocElement, freeView, pDoc, TRUE,
			      FALSE);
	       else
		 CheckAbsBox (pAb->AbElement, freeView, pDoc, TRUE, FALSE);
	     }
	 }
     }
   return ret;
}

/*----------------------------------------------------------------------
  IsEmptyDocument returns TRUE when the document is an empty document
  ----------------------------------------------------------------------*/
ThotBool IsEmptyDocument (PtrDocument pDoc)
{
  if (pDoc && pDoc->DocDName && !strcmp (pDoc->DocDName, "empty"))
    return TRUE;
  return FALSE;
}

/*----------------------------------------------------------------------
   OpenCreatedView ouvre une vue dont on a deja cree' l'image        
   pDoc: document concerne'.                               
   view: numero de la vue a ouvrir           
   X, Y, width, height: position et dimensions de la	
   		     fenetre en mm.					
  ----------------------------------------------------------------------*/
void OpenCreatedView (PtrDocument pDoc, int view,
                      int X, int Y, int width, int height,
                      ThotBool withMenu, ThotBool withButton,
                      int window_id, int page_id, int page_position, 
                      const char * viewName)
{
  PtrSSchema          pSS;
  int                 volume = 0;
  int                 frame, frame_id;
  int                 h, doc_id, split;
  int                 schView, view_id;

  frame = 0;
  if (view > 0)
    {
      /* prepare le nom de la vue */
      schView = pDoc->DocView[view - 1].DvPSchemaView;
      /* creation d'une fenetre pour la vue */
      pSS = pDoc->DocSSchema;

#ifdef _WX
      {
        // close previous open view
        frame_id = TtaGetFrameId (window_id, page_id, page_position);
        if (frame_id > 0 && FrameTable[frame_id].FrPagePos == page_position)
          {
            FrameToView(frame_id, &doc_id, &view_id);
            DestroyFrame(frame_id);
            TtaFreeView (doc_id, view_id);
          }
      }
  
      /* the new document needs a new frame */
      doc_id = IdentDocument(pDoc);
      frame = TtaMakeFrame (pSS->SsName, schView, doc_id,
                            pDoc->DocDName, width, height, &volume,
                            viewName, window_id, page_id, page_position);
      pDoc->DocViewFrame[view - 1] = frame;
      pDoc->DocViewVolume[view - 1] = volume;
      if (viewName && !strcmp (viewName,"Structure_view"))
        split = 2;
      else
        split = 1;
      /* the new document need to be attached to a page */      
      TtaAttachFrame (frame, window_id, page_id, page_position, split);
#endif /* _WX */
    }

  if (frame == 0)
    {
      /* on n'a pas pu creer la fenetre, echec */
      pDoc->DocView[view - 1].DvPSchemaView = 0;
    }
  else
    {
      /* la fenetre a ete creee correctement, on affiche l'image qui est */
      /* deja prete */
      /* on ne s'occupe pas de la hauteur de page */
      h = 0;
      if (schView == 1)
        /* it's a main view. Set a minimum volume for this view, to make sure
           that the whole abstract image get generated for relatively small
           documents. This allows these small documents to scroll smoothly */
        {
          if (volume < 10000)
            volume = 10000;
        }
      pDoc->DocViewFrame[view - 1] = frame;
      pDoc->DocViewVolume[view - 1] = volume;
      // change the default background of an empty document
      if (IsEmptyDocument (pDoc) && pDoc->DocViewRootAb[view - 1])
        pDoc->DocViewRootAb[view - 1]->AbBackground = -1;
      ChangeConcreteImage (frame, &h, pDoc->DocViewRootAb[view - 1]);
      DisplayFrame (frame);
      ShowSelection (pDoc->DocViewRootAb[view - 1], TRUE);
	  
      /* Update Paste entry in menu */
      /* do not disable paste entry for editable documents because 
       * the external clipboard buffer can contains something */
      if (pDoc->DocReadOnly)
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
      if (ThotLocalActions[T_chattr] != NULL)
      (*(Proc1)ThotLocalActions[T_chattr]) ((void*)pDoc);

#ifdef _WX
      TtaDoPostFrameCreation( frame );
#endif /* _WX */
    }
}

/*----------------------------------------------------------------------
   GetViewByName cherche la vue de nom viewName.                   
  ----------------------------------------------------------------------*/
static ThotBool GetViewByName (PtrDocument pDoc, Name viewName, int *view,
                               PtrSSchema *pSS)
{
   PtrPSchema          pPSch;
   int                 viewSch, viewDoc;
   ThotBool            open, ret;

   ret = FALSE;
   /* cherche parmi les vues declarees dans le schema de presentation et
      non ouvertes */
   pPSch = PresentationSchema (pDoc->DocSSchema, pDoc);
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
	    *pSS = pDoc->DocSSchema;
	    ret = TRUE;
	    }
      }
   return ret;
}

/*----------------------------------------------------------------------
   OpenViewByName ouvre la vue de nom viewName			
  ----------------------------------------------------------------------*/
int OpenViewByName (PtrDocument pDoc, Name viewName, int X, int Y,
		    int width, int height)
{
   PtrSSchema          pSS = NULL;
   NotifyDialog        notifyDoc;
   int                 view, freeView, ret;
   int                 doc_id, schView, window_id, page_id, page_position;

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
	if (!GetViewByName (pDoc, viewName, &view, &pSS))
	   ret = 0;
	else
	  {
	     notifyDoc.event = TteViewOpen;
	     notifyDoc.document = (Document) IdentDocument (pDoc);
	     notifyDoc.view = 0;
	     if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	        {
		ret = CreateAbstractImage (pDoc, view, pSS, 1, FALSE, NULL);

                /* look for the current windows, current page, and current page position (top/bottom)*/
		doc_id = IdentDocument(pDoc);
		schView = -1; /* pDoc->DocView[view - 1].DvPSchemaView;*/
      
		window_id = TtaGetDocumentWindowId( doc_id, schView );
		TtaGetDocumentPageId( doc_id, schView, &page_id, &page_position );
		
		OpenCreatedView (pDoc, ret, X, Y, width, height, TRUE, TRUE,
                                 window_id, page_id, page_position, viewName);
		
		notifyDoc.event = TteViewOpen;
		notifyDoc.document = (Document) IdentDocument (pDoc);
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
   BuildViewList construit le menu des vues qu'il est possible	
   d'ouvrir pour le document pDoc.                                 
   buffer: buffer pour le texte du menu.                           
   Au retour nItems indique le nombre d'items dans le menu.        
  ----------------------------------------------------------------------*/
void BuildViewList (PtrDocument pDoc, char *buffer, int *nItems)
{
  int                 i, j, length, nViews;
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
      /* nItems: nombre d'entrees dans le menu */
      *nItems = 0;
      /* Si (freeView == 0) il n'y a plus de place pour une vue */
      if (freeView > 0)
	{
	  /* cree le catalogue des vues qu'il est possible de creer */
	  /* i: index courant dans le buffer du menu */
	  i = 0;
	  nViews = BuildDocumentViewList (pDoc, AllViews);
	  for (j = 0; j < nViews; j++)
	    {
	      /* L'entree nItems du menu est l'entree j dans AllViews. */
	      ViewMenuItem[(*nItems)++] = j + 1;
	      length = strlen (AllViews[j].VdViewName) + 1;
	      if (length + i < MAX_TXT_LEN)
		{
		  strcpy (buffer + i, AllViews[j].VdViewName);
		  i += length;
		}
	      if (AllViews[j].VdOpen)
		{
		  /* Marque par une etoile a la fin du nom que la vue est
		     deja ouverte */
		  buffer[i - 1] = '*';
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
  ----------------------------------------------------------------------*/
void CloseView (PtrDocument pDoc, int viewNb)
{
  NotifyDialog        notifyDoc;
  ThotBool            ok, Save;
  View                view;
  Document            document;
  
  if (pDoc != NULL)
    {
      document = (Document) IdentDocument (pDoc);
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
                      TteConnectAction (T_rconfirmclose,
                                        (Proc) CallbackCloseDocMenu);
                    }
                  (*(Proc5)ThotLocalActions[T_confirmclose]) (
                                                              (void*)pDoc,
                                                              (void*)document,
                                                              (void*)view,
                                                              (void*)&ok,
                                                              (void*)&Save);
                  if (Save)
                    ok = (*(Func2)ThotLocalActions[T_writedocument]) (
                                                                      (void*)pDoc,
                                                                      (void*)0);
                }
              else
                ok = TRUE;
            }
          else
            ok = TRUE;
          if (ok)
            {
              /* desactive la vue si elle est active */
              DeactivateView (pDoc, viewNb);
              /* free widgets and fonts */
              DestroyFrame (pDoc->DocViewFrame[viewNb - 1]);
              notifyDoc.event = TteViewClose;
              notifyDoc.document = (Document) IdentDocument (pDoc);
              notifyDoc.view = view;
              CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
              /* detruit le contexte de la vue */
              CloseDocumentView (pDoc, viewNb, TRUE);
            }
        }
    }
}

/*----------------------------------------------------------------------
   TtcCloseView
   closes a document view.
  ----------------------------------------------------------------------*/
void TtcCloseView (Document document, View viewIn)
{
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   if (pDoc != NULL)
     CloseView (pDoc, viewIn);
}
