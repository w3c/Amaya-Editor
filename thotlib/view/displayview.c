/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles Abstract Views
 *
 * Author: I. Vatton (INRIA)
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "libmsg.h"
#include "message.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "picture.h"
#include "appaction.h"
#include "dialog.h"
#include "application.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "modif_tv.h"
#include "page_tv.h"
#include "select_tv.h"
#include "boxes_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "abspictures_f.h"
#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attrmenu_f.h"
#include "attrpresent_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "config_f.h"
#include "content_f.h"
#include "creationmenu_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "draw_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "presvariables_f.h"
#include "readpivot_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "views_f.h"
 
/* descriptor of the selection to do after redosplaying */
typedef struct _SelectionDescriptor
  {
     ThotBool            SDSelActive;
     Element             SDElemSel;
     int                 SDPremCar;
     int                 SDDerCar;
     Element             SDElemExt;
     int                 SDCarExt;
  }
SelectionDescriptor;
 
static SelectionDescriptor documentNewSelection[MAX_DOCUMENTS];

/*----------------------------------------------------------------------
   RedisplayDocViews demande le reaffichage de toutes les vues du	
   document pDoc.						
  ----------------------------------------------------------------------*/
void                RedisplayDocViews (PtrDocument pDoc)
{
  DisplayMode       displayMode;
  int                 i;

  displayMode = documentDisplayMode[IdentDocument (pDoc) - 1];
  if (displayMode == DisplayImmediately)
    {
      for (i = 0; i < MAX_VIEW_DOC; i++)
	if (pDoc->DocView[i].DvPSchemaView > 0)
	  /* open view */
	  DisplayFrame (pDoc->DocViewFrame[i]);
    }
}


/*----------------------------------------------------------------------
   AbstractImageUpdated	signale les modifications de l'image	
   abstraite du document pDoc.				
  ----------------------------------------------------------------------*/
void                AbstractImageUpdated (PtrDocument pDoc)
{
  DisplayMode       displayMode;
  PtrAbstractBox    pAb;
  int               i, h, frame;
  ThotBool          rootAbWillBeFree;

  displayMode = documentDisplayMode[IdentDocument (pDoc) - 1];
  if (displayMode == NoComputedDisplay)
    return;

  for (i = 0; i < MAX_VIEW_DOC; i++)
    if (pDoc->DocView[i].DvPSchemaView > 0
	&& pDoc->DocViewModifiedAb[i] != NULL)
      {
	/* on ne s'occupe pas de la hauteur de page */
	h = 0;
	frame = pDoc->DocViewFrame[i];
	pAb = pDoc->DocViewModifiedAb[i];
	if (pDoc->DocViewRootAb[i] == NULL)
	  pDoc->DocViewRootAb[i] = pAb;
	pDoc->DocViewModifiedAb[i] = NULL;
	ChangeConcreteImage (frame, &h, pAb);
	/* libere les paves morts */
	rootAbWillBeFree = pDoc->DocViewRootAb[i]->AbDead;
	if (rootAbWillBeFree)
	  pDoc->DocViewRootAb[i] = NULL;
	FreeDeadAbstractBoxes (pAb, frame);
      }
}

/*----------------------------------------------------------------------
   EnclosingAbsBoxesBreakable
   Returns TRUE if all abstract boxes in view v corresponding to
   ancestors of element pEl are allowed to be incomplete.
  ----------------------------------------------------------------------*/
static ThotBool	EnclosingAbsBoxesBreakable (PtrElement pEl, int v,
					    PtrDocument pDoc)
{
   PtrAbstractBox	pAb;

   pAb = NULL;
   while (pEl && !pAb)
     {
     pEl = pEl->ElParent;
     if (pEl)
        pAb = pEl->ElAbstractBox[v - 1];
     }
   return (IsBreakable (pAb, pDoc));
}

/*----------------------------------------------------------------------
   SupprFollowingAbsBoxes
   Delete from a given view all abstract boxes associated with elements
   that follow pEl (following siblings of pEl and its ancestors).
  ----------------------------------------------------------------------*/
static void SupprFollowingAbsBoxes (PtrElement pEl, PtrDocument pDoc, int view)
{
   PtrAbstractBox	pAb, pNextAb, pParentAb, pAbbRedisp, pAbbR;

   pAbbRedisp = NULL;
   /* look for the first abstract box (pAb) corresponding to a following
      sibling of pEl or to a following sibling of its ancestors */
   pAb = NULL;
   while (!pAb && pEl)
      {
      if (pEl->ElNext)
	 {
         pAb = pEl->ElNext->ElAbstractBox[view - 1];
         if (!pAb)
	    pEl = pEl->ElNext;
	 }
      else
	 pEl = pEl->ElParent;
      }
   /* now, delete the abstract box we have found and all following abstract
      boxes */
   if (pAb)
      {
      /* we will have to redisplay at least pAb */
      pAbbRedisp = pAb;
      do
	{
        pParentAb = pAb->AbEnclosing;
	/* kill pAb and all its following siblings */
	while (pAb)
	   {
	   pNextAb = pAb->AbNext;
           SetDeadAbsBox (pAb);
	   ApplyRefAbsBoxSupp (pAb, &pAbbR, pDoc);
	   pAbbRedisp = Enclosing (pAbbRedisp, pAbbR);
	   pAb = pNextAb;
	   }
        /* look for the first ancestor abstract box that has a next sibling */
	pAb = pParentAb;
	if (pAb)
	  {
	   if (pAb->AbNext)
	      pAb = pAb->AbNext;
	   else
	      {
	      pNextAb = NULL;
	      while (pAb && !pNextAb)
		 {
	         pAb = pAb->AbEnclosing;
		 if (pAb)
		    {
		    pNextAb = pAb->AbNext;
		    if (pNextAb)
		       pAb = pNextAb;
		    }
	         }
	      }
	  }
	}
      while (pAb);
      }
   /* remember the abstract box that has to be redisplayed */
   if (pAbbRedisp)
      {
      pDoc->DocViewModifiedAb[view - 1] =
		Enclosing (pAbbRedisp, pDoc->DocViewModifiedAb[view - 1]);
      }
}

/*----------------------------------------------------------------------
   BuildAbstractBoxes
   Cree dans toutes les vues ouvertes du document pDoc les paves qui
   correspondent a l'element pEl, dans la limite de la capacite' des vues.
  ----------------------------------------------------------------------*/
void BuildAbstractBoxes (PtrElement pEl, PtrDocument pDoc)
{
  PtrAbstractBox      pAb;
  int                 view;

  /* on ne cree les paves que s'ils tombent dans la partie de l'image */
  /* du document deja construite */
  /* traite toutes les vues du document */
  for (view = 1; view <= MAX_VIEW_DOC; view++)
    {
      if (pDoc->DocView[view - 1].DvPSchemaView > 0)
	/* la vue est ouverte */
	if (ElemWithinImage (pEl, view, pDoc->DocViewRootAb[view-1], pDoc))
	  /* l'element est a l'interieur de l'image deja construite */
	  {
	    if (!EnclosingAbsBoxesBreakable (pEl, view, pDoc) ||
		pEl->ElVolume + pDoc->DocViewRootAb[view - 1]->AbVolume
					< 2 * pDoc->DocViewVolume[view - 1])
	      /* on cree tous les paves du nouvel element */
	      pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
	    else
	      /* le volume du nouvel element ajoute' au volume existant
		 de la vue est tres superieur a ce que la fenetre peut
		 montrer d'un coup */
	      {
		/* on detruit la partie de l'image abstraite qui suit le
		   nouvel element */
		SupprFollowingAbsBoxes (pEl, pDoc, view);
		/* on creera seulement une partie des paves du nouvel
		   element */
		pDoc->DocViewFreeVolume[view - 1] =
			pDoc->DocViewVolume[view - 1] - pDoc->DocViewRootAb[view - 1]->AbVolume;
	      }
	    /* cree effectivement les paves du nouvel element dans la vue */
	    if (pDoc->DocViewFreeVolume[view - 1] > 0)
	      CreateNewAbsBoxes (pEl, pDoc, view);
	    else
	      {
		/* mark enclosing abstract boxes as truncated */
		pAb = pEl->ElParent->ElAbstractBox[view - 1];
		while (pAb && !pAb->AbTruncatedTail)
		  {
		    pAb->AbTruncatedTail = TRUE;
		    pAb = pAb->AbEnclosing;
		  }
	      }
	  }
    }
  /* applique les regles retardees concernant les paves cree's */
  ApplDelayedRule (pEl, pDoc);
  AbstractImageUpdated (pDoc);
}

/*----------------------------------------------------------------------
   RedisplayNewElement affiche un element qui vient d'etre ajoute'    
   dans un arbre abstrait.                                         
  ----------------------------------------------------------------------*/
void   RedisplayNewElement (Document document,
			    PtrElement newElement,
			    PtrElement sibling, ThotBool first,
			    ThotBool creation)
{
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* If the document doesn't have any presentation schema, do nothing */
   if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   if (sibling != NULL)
      /* l'element sibling n'est plus le dernier (ou premier) fils de son pere */
      ChangeFirstLast (sibling, pDoc, first, TRUE);
   /* cree les paves de l'element dans la limite de la capacite' des vues
      ouvertes */
   BuildAbstractBoxes (newElement, pDoc);
   /* reaffiche */
   RedisplayCommand (document);
   if (creation)
     {
	/* reaffiche les paves qui copient le nouvel element */
	RedisplayCopies (newElement, pDoc, (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
	/* reaffiche les numeros suivants qui changent */
	UpdateNumbers (newElement, newElement, pDoc,
	  (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
     }
}

/*----------------------------------------------------------------------
   TCloseDocument ferme toutes les vue d'un document et decharge ce	
   document. Si pDoc est NULL, demande a` l'utilisateur de 
   designer le document a` fermer et lui demande           
   confirmation, sinon pDoc designe le contexte du document
   a` fermer.                                              
   Detruit egalement le fichier .BAK du document.          
  ----------------------------------------------------------------------*/
void TCloseDocument (PtrDocument pDoc)
{
  NotifyDialog      notifyDoc;
  Document          document;

  if (pDoc != NULL)
    {
      document = (Document) IdentDocument (pDoc);
      notifyDoc.event = TteDocClose;
      notifyDoc.document = document;
      notifyDoc.view = 0;
      if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	{
	  /* if there is a "Spell checker" menu entry, close the spell checker
	     dialog box */
	  if (ThotLocalActions[T_clearhistory] != NULL)
	    (*ThotLocalActions[T_clearhistory]) (pDoc);
	  /* if some dialog boxes for attribute input are displayed
             for that document, close them */
          CloseAttributeDialogues (pDoc);
          TtaHandlePendingEvents ();
	  /* detruit toutes les vues ouvertes du document */
	  CloseAllViewsDoc (pDoc);
	  /* free document contents */
	  UnloadTree (document);
	  notifyDoc.event = TteDocClose;
	  notifyDoc.document = document;
	  notifyDoc.view = 0;
	  CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	  UnloadDocument (&pDoc);
	}
    }
}

/*----------------------------------------------------------------------
   NumberOfOpenViews retourne le nombre de vues qui existent pour	
   le document pDoc					
  ----------------------------------------------------------------------*/
int NumberOfOpenViews (PtrDocument pDoc)
{
   int                 view, result;

   result = 0;
   for (view = 0; view < MAX_VIEW_DOC; view++)
      if (pDoc->DocView[view].DvPSchemaView > 0)
	 result++;
   return result;
}

/*----------------------------------------------------------------------
   FreeView libere les paves et le contexte de la vue view du	
   document pDoc.						
  ----------------------------------------------------------------------*/
void                FreeView (PtrDocument pDoc, DocViewNumber view)
{
   PtrGuestViewDescr  guestView, next;

   view--;
   if (pDoc->DocViewRootAb[view] != NULL)
      FreeAbView (pDoc->DocViewRootAb[view], pDoc->DocViewFrame[view]);
   pDoc->DocViewRootAb[view] = NULL;
   pDoc->DocView[view].DvSSchema = NULL;
   pDoc->DocView[view].DvPSchemaView = 0;
   pDoc->DocView[view].DvSync = FALSE;
   if (pDoc->DocView[view].DvFirstGuestView)
      {
      guestView = pDoc->DocView[view].DvFirstGuestView;
      pDoc->DocView[view].DvFirstGuestView = NULL;
      while (guestView)
	 {
	 next = guestView->GvNextGuestView;
	 TtaFreeMemory (guestView);
	 guestView = next;
	 }
      }
   pDoc->DocViewFrame[view] = 0;
   pDoc->DocViewVolume[view] = 0;
   pDoc->DocViewFreeVolume[view] = 0;
   pDoc->DocViewSubTree[view] = NULL;
}

/*----------------------------------------------------------------------
   CloseDocumentView detruit la vue de numero view
   pour le document pDoc. S'il s'agit de la derniere vue, libere le
   document dans le cas seulement ou closeDoc est vrai.
  ----------------------------------------------------------------------*/
void  CloseDocumentView (PtrDocument pDoc, int view, ThotBool closeDoc)
{
  if (pDoc != NULL)
    /* on detruit la vue */
    {
      FreeView (pDoc, view);
      if (closeDoc)
	/* verifie qu'il reste au moins une vue pour ce document */
	if (NumberOfOpenViews (pDoc) < 1)
	  {
	    /* il ne reste plus de vue, on libere le document */
	    TCloseDocument (pDoc);
	  }
    }
}

/*----------------------------------------------------------------------
   CloseAllViewsDoc ferme toutes les vues ouvertes du document pDoc 
  ----------------------------------------------------------------------*/
void                CloseAllViewsDoc (PtrDocument pDoc)
{
  int                 view;

  if (pDoc != NULL)
    {
      /* detruit les vues de l'arbre principal */
      for (view = 0; view < MAX_VIEW_DOC; view++)
	if (pDoc->DocView[view].DvPSchemaView != 0)
	  {
	    DestroyFrame (pDoc->DocViewFrame[view]);
	    CloseDocumentView (pDoc, view + 1, FALSE);
	  }
    }
}

/*----------------------------------------------------------------------
   CleanImageView cleans the abstract image of View corresponding to pDoc. 
   View = view number
   complete = TRUE if the window is completely cleaned.           
  ----------------------------------------------------------------------*/
static void CleanImageView (int View, PtrDocument pDoc, ThotBool complete)
{
   PtrAbstractBox      pAb;
   int                 h;
   int                 frame;
   PtrAbstractBox      pAbbRoot;

   frame = 1;			/* initialization (for the compiler !) */
   pAbbRoot = NULL;		/* initialization (for the compiler !) */
   pAbbRoot = pDoc->DocViewRootAb[View - 1];
   frame = pDoc->DocViewFrame[View - 1];

   /* All abstract boxes included into the root abs. box are marked dead */
   if (pAbbRoot == NULL)
     return;

   if (complete)
     {
	SetDeadAbsBox (pAbbRoot);
	ChangeConcreteImage (frame, &h, pAbbRoot);
	CloseDocumentView (pDoc, View, TRUE);
	FrameTable[frame].FrDoc = 0;
	/* selection is no more displayed */
	ViewFrameTable[frame - 1].FrSelectShown = FALSE;
     }
   else
     {
	pAb = pAbbRoot->AbFirstEnclosed;
	while (pAb != NULL)
	  {
	     SetDeadAbsBox (pAb);
	     pAb = pAb->AbNext;
	  }
	h = 0;
	ChangeConcreteImage (frame, &h, pAbbRoot);
	/* Releases all dead abstract boxes of the view */
	FreeDeadAbstractBoxes (pAbbRoot, frame);

	/* Shows that one must apply presentation rules of the root abstract
	   box, for example to rebuild presentaion boxes, created by the
	   root and destroyed */
	pAbbRoot->AbSize = -1;
	/* The complete root abstract box is marked. This allows AbsBoxesCreate
	   to generate presentation abstract boxes created at the begenning */
	if (pAbbRoot->AbLeafType == LtCompound)
	   pAbbRoot->AbTruncatedHead = FALSE;
     }
}

/*----------------------------------------------------------------------
   DestroyImage detruit l'image abstraite de toutes les vues          
   ouvertes dudocument pDoc                                     
  ----------------------------------------------------------------------*/
static void         DestroyImage (PtrDocument pDoc)
{
  int                 view, frame;

  for (view = 0; view < MAX_VIEW_DOC; view++)
    {
      if (pDoc->DocView[view].DvPSchemaView > 0)
	{
	  CleanImageView (view + 1, pDoc, FALSE);
	  /* selection is no more displayed */
	  frame = pDoc->DocViewFrame[view];
	  ViewFrameTable[frame - 1].FrSelectShown = FALSE;
	}
    }
}

/*----------------------------------------------------------------------
   RebuildImage recree l'image abstraite de toutes les vues            
   ouvertes du document pDoc                                      
  ----------------------------------------------------------------------*/
static void         RebuildImage (PtrDocument pDoc)
{
  int                 view;
  PtrElement          pElRoot;
  PtrAbstractBox      pAbbRoot;
  ViewFrame          *pFrame;
  int                 frame, h, w;
  ThotBool            complete;

  for (view = 1; view <= MAX_VIEW_DOC; view++)
    if (pDoc->DocView[view - 1].DvPSchemaView > 0)
       {
       pElRoot = pDoc->DocDocElement;
       pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1];
       pAbbRoot = pDoc->DocViewRootAb[view - 1];
       frame = pDoc->DocViewFrame[view - 1];
       AbsBoxesCreate (pElRoot, pDoc, view, TRUE, TRUE, &complete);
       if (pAbbRoot == NULL)
	 pAbbRoot = pDoc->DocViewRootAb[view - 1] = pElRoot->ElAbstractBox[view - 1];
       h = 0;
       ChangeConcreteImage (frame, &h, pAbbRoot);
       /* force to redraw all the frame */
       pFrame = &ViewFrameTable[frame - 1];
       GetSizesFrame (frame, &w, &h);
       DefClip (frame, pFrame->FrXOrg, pFrame->FrYOrg, w, h);
       }
}


/*----------------------------------------------------------------------
   ChangeAbsBoxModifAttrIntoView change les booleens AbCanBeModified et AbReadOnly   
   dans tous les paves de l'element pEl qui appartiennent a la vue 
   vue. newAbsModif donne la nouvelle valeur de AbCanBeModified,          
   reaffiche indique si on veut reafficher.                        
  ----------------------------------------------------------------------*/
static void  ChangeAbsBoxModifAttrIntoView (PtrElement pEl, int view,
					    ThotBool newAbsModif,
					    ThotBool redisplay)
{
   PtrAbstractBox      pAb, pAbbChild;
   ThotBool            stop;

   pAb = pEl->ElAbstractBox[view - 1];
   if (pAb != NULL)
     {
	stop = FALSE;
	while (!stop)
	   if (pAb->AbElement != pEl)
	      /* ce n'est pas un pave de l'element, on arrete */
	      stop = TRUE;
	   else
	      /* c'est un pave de l'element, on le traite */
	     {
		pAb->AbReadOnly = !newAbsModif;
		if (redisplay)
		   pAb->AbAspectChange = TRUE;
		if (!pAb->AbPresentationBox)
		   /* c'est le pave principal de l'element */
		  {
		     /* les paves de presentation restent non modifiables */
		     pAb->AbCanBeModified = newAbsModif;
		     /* traite les paves de presentation crees par Create et */
		     /* CreateLast */
		     pAbbChild = pAb->AbFirstEnclosed;
		     while (pAbbChild != NULL)
		       {
			  if (pAbbChild->AbElement == pEl)
			     /* c'est un pave de l'element */
			    {
			       pAbbChild->AbReadOnly = !newAbsModif;
			       if (redisplay)
				  pAbbChild->AbAspectChange = TRUE;
			    }
			  pAbbChild = pAbbChild->AbNext;
		       }
		  }
		if (pAb->AbNext != NULL)
		   /* passe au pave suivant */
		   pAb = pAb->AbNext;
		else
		   stop = TRUE;
	     }
     }
}


/*----------------------------------------------------------------------
   ChangeAbsBoxModif change les booleens AbCanBeModified et AbReadOnly dans 
   tous les paves existants de l'element pEl et de sa descendance. 
  ----------------------------------------------------------------------*/
void                ChangeAbsBoxModif (PtrElement pEl, Document document,
				       ThotBool newAbsModif)
{
  PtrDocument         pDoc;
  int                 view;
  PtrElement          pChild;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
    return;
  /* si le document est en mode de non calcul de l'image, on ne fait rien */
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  /* demande au mediateur si une couleur est associee a ReadOnly */
  /* si oui, il faut reafficher les paves modifie's */
  /* on traite toutes les vues du document */
  for (view = 1; view <= MAX_VIEW_DOC; view++)
    {
      /* on traite tous les paves de l'element dans cette vue */
      ChangeAbsBoxModifAttrIntoView (pEl, view, newAbsModif, TRUE);
      if (pEl->ElAbstractBox[view - 1] != NULL)
	RedispAbsBox (pEl->ElAbstractBox[view - 1],
		      LoadedDocument[document - 1]);
    }
  /* on fait reafficher pour visualiser le changement de couleur */
  AbstractImageUpdated (LoadedDocument[document - 1]);
  RedisplayCommand (document);
  /* meme traitement pour les fils qui heritent les droits d'acces */
  if (!pEl->ElTerminal)
    {
      pChild = pEl->ElFirstChild;
      while (pChild != NULL)
	{
	  if (pChild->ElAccess == AccessInherited)
	    ChangeAbsBoxModif (pChild, document, newAbsModif);
	  pChild = pChild->ElNext;
	}
    }
}


/*----------------------------------------------------------------------
   RedisplayDefaultPresentation                                              
  ----------------------------------------------------------------------*/
void  RedisplayDefaultPresentation (Document document, PtrElement pEl,
				    PRuleType typeRuleP, FunctionType funcType,
				    int view)
{

   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (PresentationSchema (LoadedDocument[document - 1]->DocSSchema,
			   LoadedDocument[document - 1]) == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* if it's a function rule, nothing to do: there is no default presentation
      for functions */
   if (typeRuleP == PtFunction)
      return;
   ApplyStandardRule (pEl, LoadedDocument[document - 1], typeRuleP, funcType, view);
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* le retrait de la regle de presentation doit etre pris en compte */
   /* dans les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
}


/*----------------------------------------------------------------------
   HideElement "desaffiche" un element qui devient invisible       
   mais n'est pas detruit.                                         
  ----------------------------------------------------------------------*/
void                HideElement (PtrElement pEl, Document document)
{
   PtrDocument         pDoc;
   PtrElement          pChild;

   pDoc = LoadedDocument[document - 1];
   if (pDoc == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (!PresentationSchema (pDoc->DocSSchema, LoadedDocument [document - 1]))
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   if (ThotLocalActions[T_createhairline] != NULL)
     (*ThotLocalActions[T_checksel]) (pEl, document);
   if (pEl->ElParent == NULL)
      /* c'est la racine d'un arbre, on detruit les paves des fils */
      /* pour garder au moins le pave racine */
     {
	if (!pEl->ElTerminal)
	  {
	     pChild = pEl->ElFirstChild;
	     while (pChild != NULL)
	       {
		  DestroyAbsBoxes (pChild, pDoc, TRUE);
		  pChild = pChild->ElNext;
	       }
	  }
     }
   else
      /* ce n'est pas une racine, on detruit les paves de l'element */
      DestroyAbsBoxes (pEl, pDoc, TRUE);
   /* reevalue l'image de toutes les vues */
   AbstractImageUpdated (pDoc);
   /* pas d'operation de reaffichage secondaires */
   RedisplayCommand (document);
}


/*----------------------------------------------------------------------
   RedisplayNewPRule                                               
  ----------------------------------------------------------------------*/
void RedisplayNewPRule (Document document, PtrElement pEl, PtrPRule pRule)
{
   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (PresentationSchema (LoadedDocument[document - 1]->DocSSchema,
			   LoadedDocument[document - 1]) == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   ApplyNewRule (LoadedDocument[document - 1], pRule, pEl);
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* la nouvelle regle de presentation doit etre prise en compte dans */
   /* les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
}


/*----------------------------------------------------------------------
   UndisplayAttribute                                              
  ----------------------------------------------------------------------*/
void UndisplayAttribute (PtrElement pEl, PtrAttribute pAttr, Document document)
{
   ThotBool            inheritance, comparaison;
   PtrAttribute        pAttrAsc;
   PtrElement          pElAttr;
   PtrPSchema          pPS;

   if (LoadedDocument[document - 1] == NULL)
      return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (PresentationSchema (LoadedDocument[document - 1]->DocSSchema,
			   LoadedDocument[document - 1]) == NULL)
      return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
      return;
   /* doit-on se preoccuper des heritages et comparaisons d'attributs? */
   pPS = PresentationSchema (pAttr->AeAttrSSchema,
			     LoadedDocument[document - 1]);
   inheritance = (pPS->PsNHeirElems[pAttr->AeAttrNum - 1] > 0);
   comparaison = (pPS->PsNComparAttrs[pAttr->AeAttrNum - 1] > 0);
   if (inheritance || comparaison)
      /* cherche le premier attribut de meme type pose' sur un ascendant */
      /* de pEl */
      pAttrAsc = GetTypedAttrAncestor (pEl, pAttr->AeAttrNum,
				       pAttr->AeAttrSSchema, &pElAttr);
   else
      pAttrAsc = NULL;
   if (pAttrAsc != NULL)
     {
	/* les regles viennent maintenant de l'heritage ou */
	/* de la comparaison a un ascendant */
	/* on applique sur les elements du sous arbre pEl  */
	/* les regles de presentation liees a l'heritage de cet attribut */
	/* par le sous-arbre s'il existe des elements heritants de celui-ci */
	ApplyAttrPRulesToSubtree (pEl, LoadedDocument[document - 1], pAttrAsc,
				  pElAttr);
	/* puis on applique sur les elements du sous-arbre pEl */
	/* les regles de presentation liees a la comparaison d'un attribut */
	/* du sous-arbre avec ce type d'attribut */
	ApplyAttrPRules (pEl, LoadedDocument[document - 1], pAttrAsc);
     }
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* le nouvel attribut doit etre pris en compte dans */
   /* les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1],
		    (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
}


/*----------------------------------------------------------------------
   DisplayAttribute                                                
  ----------------------------------------------------------------------*/
void DisplayAttribute (PtrElement pEl, PtrAttribute pAttr, Document document)
{
   PtrElement          pElChild;
   PtrPSchema          pPS;
   ThotBool            inheritance, comparaison, reDisp;

   if (LoadedDocument[document - 1] == NULL || DocumentOfElement (pEl) == NULL)
     return;
   /* si le document n'a pas de schema de presentation, on ne fait rien */
   if (PresentationSchema (LoadedDocument[document - 1]->DocSSchema,
			   LoadedDocument[document - 1]) == NULL)
     return;
   /* si le document est en mode de non calcul de l'image, on ne fait rien */
   if (documentDisplayMode[document - 1] == NoComputedDisplay)
     return;
   /* doit-on se preoccuper des heritages et comparaisons d'attributs? */
   pPS = PresentationSchema (pAttr->AeAttrSSchema,
			     LoadedDocument[document - 1]);
   inheritance = (pPS->PsNHeirElems[pAttr->AeAttrNum - 1] > 0);
   comparaison = (pPS->PsNComparAttrs[pAttr->AeAttrNum - 1] > 0);
   reDisp = (documentDisplayMode[document - 1] == DisplayImmediately);
   /* d'abord on applique les regles de presentation liees */
   /* a l'attribut sur l'element lui-meme */
   ApplyAttrPRulesToElem (pEl, LoadedDocument[document - 1], pAttr, pEl, FALSE);
   /* puis on applique sur pEl et les elements du sous-arbre pEl */
   /* les regles de presentation liees a l'heritage de cet attribut */
   /* par le sous arbre s'il existe des elements heritants de celui-ci */
   if (inheritance)
      ApplyAttrPRulesToSubtree (pEl, LoadedDocument[document - 1], pAttr, pEl);
   /* puis on applique sur les elements du sous arbre pEl */
   /* les regles de presentation liees a la comparaison d'un attribut */
   /* du sous-arbre avec cetype d'attribut */
   if (!pEl->ElTerminal && comparaison)
      for (pElChild = pEl->ElFirstChild; pElChild != NULL; pElChild = pElChild->ElNext)
	 ApplyAttrPRules (pElChild, LoadedDocument[document - 1], pAttr);
   if (pAttr->AeAttrType == AtNumAttr)
      /* s'il s'agit d'un attribut initialisant un compteur, il */
      /* faut mettre a jour les boites utilisant ce compteur */
      UpdateCountersByAttr (pEl, pAttr, LoadedDocument[document - 1]);
   /* on applique les regles retardee */
   ApplDelayedRule (pEl, LoadedDocument[document - 1]);
   AbstractImageUpdated (LoadedDocument[document - 1]);
   RedisplayCommand (document);
   /* le nouvel attribut doit etre pris en compte dans */
   /* les copies-inclusions de l'element */
   RedisplayCopies (pEl, LoadedDocument[document - 1], reDisp);
}

/*----------------------------------------------------------------------
   RedisplayCommand        Selon le mode d'affichage, execute ou   
   met en attente une commande de reaffichage secondaire.          
  ----------------------------------------------------------------------*/
void         RedisplayCommand (Document document)
{
   if (documentDisplayMode[document - 1] == DisplayImmediately)
     {
	/* reaffiche ce qui a deja ete prepare' */
	RedisplayDocViews (LoadedDocument[document - 1]);
     }
}



/*----------------------------------------------------------------------
  NewSelection
  ----------------------------------------------------------------------*/
void    NewSelection (Document document, Element element,
		      int firstCharacter, int lastCharacter)
{

   /* annule l'extension precedente */
   documentNewSelection[document - 1].SDElemExt = NULL;
   documentNewSelection[document - 1].SDCarExt = 0;
   /* enregistre cette nouvelle selection */
   documentNewSelection[document - 1].SDSelActive = TRUE;
   documentNewSelection[document - 1].SDElemSel = element;
   documentNewSelection[document - 1].SDPremCar = firstCharacter;
   documentNewSelection[document - 1].SDDerCar = lastCharacter;
}

/*----------------------------------------------------------------------
  NewSelectionExtension
  ----------------------------------------------------------------------*/
void  NewSelectionExtension (Document doc, Element element, int lastCharacter)
{
   /* enregistre cette nouvelle extension de selection */
   documentNewSelection[doc - 1].SDElemExt = element;
   documentNewSelection[doc - 1].SDCarExt = lastCharacter;
}

/*----------------------------------------------------------------------
   TtaFreeView

   frees the view of the document. The window continues to exist but the
   document is no longer displayed in this window.

   Parameters:
   document: the document for which a view must be closed.
   view: the view to be closed.

  ----------------------------------------------------------------------*/
void                TtaFreeView (Document document, View view)
{
   PtrDocument         pDoc;

   UserErrorCode = 0;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
     {
	pDoc = LoadedDocument[document - 1];
	if (view < 1 || view > MAX_VIEW_DOC)
	  TtaError (ERR_invalid_parameter);
	else
	  CleanImageView (view, pDoc, TRUE);
     }
}

/*----------------------------------------------------------------------
  IsSelectionRegistered
  ----------------------------------------------------------------------*/
ThotBool     IsSelectionRegistered (Document document, ThotBool * abort)
{
   ThotBool            ret;

   ret = documentNewSelection[document - 1].SDSelActive;
   if (ret)
      *abort = (documentNewSelection[document - 1].SDElemSel == NULL);
   return ret;
}

/*----------------------------------------------------------------------
   TtaSetDisplayMode

   Changes display mode for a document. Three display modes are available.
   In the immediate mode, each modification made in the abstract tree of a
   document is immediately reflected in all opened views where the modification
   can be seen.

   In the deferred mode, the programmer can decide when the modifications are
   made visible to the user; this avoids the image of the document to blink when
   several elementary changes are made successively. Modifications are displayed
   when mode is changed to DisplayImmediately.

   In the NoComputedDisplay mode, the modifications are not displayed and they 
   are not computed inside the editor; the execution is more rapid but the current
   image is lost. When mode is changed to DisplayImmediately or DeferredMode,
   the image is completely redrawn by the editor.

   In the SuspendDisplay mode, the modifications are not displayed but stored 
   inside the editor; the execution is more and the current image is not lost.
   When mode is changed to DisplayImmediately or DeferredMode, the modifications
   are computed by the editor.

   An application that handles several documents at the same time can choose
   different modes for different documents. When a document is open or created,
   it is initially in the immediate mode.

   Parameters:
   document: the document.
   NewDisplayMode: new display mode for that document.
  ----------------------------------------------------------------------*/
void   TtaSetDisplayMode (Document document, DisplayMode newDisplayMode)
{
  DisplayMode       oldDisplayMode;
  PtrDocument       pDoc;

  UserErrorCode = 0;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is ok */
    {
      pDoc = LoadedDocument[document - 1];
      /* si le document n'a pas de schema de presentation, on ne fait rien */
      if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
	return;

      oldDisplayMode = documentDisplayMode[document - 1];
      if (oldDisplayMode != newDisplayMode)
	/* il y a effectivement changement de mode */
	{
	  if (oldDisplayMode == DisplayImmediately)
	    /* le document passe en mode affichage differe ou sans calcul
	       d'image */
	    {
	      /* si on passe au mode sans calcul d'image il faut detruire
		 l'image */
	      if (newDisplayMode == NoComputedDisplay)
		DestroyImage (pDoc);
	      else if (newDisplayMode == SuspendDisplay)
		TtaClearViewSelections ();
	      /* on met a jour le mode d'affichage */
	      documentDisplayMode[document - 1] = newDisplayMode;
	    }
	  else if (newDisplayMode == DisplayImmediately)
	    /* le document passe du mode affichage differe' ou sans calcul  */
	    /* d'image au mode  d'affichage immediat */
	    {
	      /* on met a jour le mode d'affichage */
	      documentDisplayMode[document - 1] = newDisplayMode;
	      
              if (oldDisplayMode == NoComputedDisplay)
		/* il faut recalculer l'image */
		RebuildImage (pDoc);
	      else if (oldDisplayMode == SuspendDisplay)
		AbstractImageUpdated (pDoc);
	      
	      if (!documentNewSelection[document - 1].SDSelActive)
		{
		  /* la selection n'a pas change', on la rallume */
		  if (oldDisplayMode == SuspendDisplay)
		    HighlightSelection (TRUE, FALSE);
		}
	      else
		/* la selection a change', on etablit la selection */
		/* enregistree */
		{
		  if (documentNewSelection[document - 1].SDElemSel == NULL)
		    /* c'est une annulation de selection */
		    {
		      if (ThotLocalActions[T_resetsel])
			(*ThotLocalActions[T_resetsel]) (pDoc);
		    }
		  else
		    {
		      /* il y a effectivement une selection a etablir */
		      if (documentNewSelection[document - 1].SDPremCar == 0 &&
			  documentNewSelection[document - 1].SDDerCar == 0)
			/* selection d'un element complet */
			SelectElement (pDoc, (PtrElement) (documentNewSelection[document - 1].SDElemSel), TRUE, TRUE);
		      else
			/* selection d'une chaine */
			if (ThotLocalActions[T_selstring])
			  (*ThotLocalActions[T_selstring]) (pDoc,
			      (PtrElement) (documentNewSelection[document - 1].SDElemSel),
			      documentNewSelection[document - 1].SDPremCar,
			      documentNewSelection[document - 1].SDDerCar);
		      /* il n'y a plus de selection a etablir */
		      documentNewSelection[document - 1].SDElemSel = NULL;
		    }
		  /* etablit l'extension de selection enregistree */
		  if (documentNewSelection[document - 1].SDElemExt != NULL)
		    /* il y a une extension de selection a etablir */
		    {
		      if (ThotLocalActions[T_extendsel])
			(*ThotLocalActions[T_extendsel]) ((PtrElement) (documentNewSelection[document - 1].SDElemExt),
				documentNewSelection[document - 1].SDCarExt,
				FALSE, FALSE, FALSE);
		      /* il n'y a plus d'extension de selection a etablir */
		      documentNewSelection[document - 1].SDElemExt = NULL;
		    }
		  /* plus de selection a faire pour ce document */
		  documentNewSelection[document - 1].SDSelActive = FALSE;
		}
	      
              /* reaffiche ce qui a deja ete prepare' */
              RedisplayDocViews (pDoc);
	    }
	  else if (newDisplayMode == NoComputedDisplay)
	    {
	      /* le document passe du mode affichage differe'  */
	      /* au mode d'affichage sans calcul d'image  */
	      DestroyImage (pDoc);
	      /* on met a jour le mode d'affichage */
	      documentDisplayMode[document - 1] = newDisplayMode;
	    }
	  else if (oldDisplayMode == NoComputedDisplay)
	    {
	      /* on met a jour le mode d'affichage */
	      documentDisplayMode[document - 1] = newDisplayMode;
	      /* le document passe du mode affichage sans calcul d'image   */
	      /* au mode d'affichage differe'  */
	      RebuildImage (pDoc);
	    }
	  else if (oldDisplayMode == SuspendDisplay &&
		   newDisplayMode == DeferredDisplay)
	    {
	      /* on met a jour le mode d'affichage */
	      documentDisplayMode[document - 1] = newDisplayMode;
	      AbstractImageUpdated (pDoc);
	    }
	  else
	    /* on met a jour le mode d'affichage */
	    documentDisplayMode[document - 1] = newDisplayMode;

	}
    }
}

/*----------------------------------------------------------------------
   TtaGetDisplayMode

   Returns the current display mode for a document.

   Parameter:
   document: the document.

   Return value:
   current display mode for that document.
  ----------------------------------------------------------------------*/
DisplayMode         TtaGetDisplayMode (Document document)
{
   DisplayMode         result;

   UserErrorCode = 0;
   result = DisplayImmediately;
   /* Checks the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is ok */
      result = documentDisplayMode[document - 1];
   return result;
}
