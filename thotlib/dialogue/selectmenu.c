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
 * Handle selection commands.
 *
 * Authors: I. Vatton (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "message.h"
#include "constmenu.h"
#include "constmedia.h"
#include "dialog.h"
#include "typemedia.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "select_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "appli_f.h"
#include "tree_f.h"
#include "attrmenu_f.h"
#include "search_f.h"
#include "createabsbox_f.h"
#include "views_f.h"
#include "appdialogue_f.h"
#include "actions_f.h"
#include "viewcommands_f.h"
#include "input_f.h"
#include "keyboards_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "applicationapi_f.h"


/*----------------------------------------------------------------------
   BuildSelectMenu compose dans le buffer BufMenu le menu       
   Selection et retourne le nombre d'entrees de ce menu.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 BuildSelectMenu (char BufMenu[MAX_TXT_LEN])
#else  /* __STDC__ */
int                 BuildSelectMenu (BufMenu)
char                BufMenu[MAX_TXT_LEN];

#endif /* __STDC__ */
{
   int                 i;
   PtrElement          pEl;
   char               *NomElem;

#ifdef __COLPAGE__
   boolean             stop;
   boolean             stop1;

#endif /* __COLPAGE__ */

   /* element englobant non cache' */
   i = 0;
   strcpy (&BufMenu[i], "B^ ");
   pEl = SelMenuParentEl;
   if (pEl != NULL && pEl->ElStructSchema != NULL)
      strcat (&BufMenu[i], pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
   i += strlen (&BufMenu[i]) + 1;

   /* element selectionable precedent */
   strcpy (&BufMenu[i], "B< ");
   pEl = SelMenuPreviousEl;
   if (pEl != NULL && pEl->ElStructSchema != NULL)
     {
#ifdef __COLPAGE__
	if (pEl->ElTypeNumber == PageBreak + 1)
	   NomElem = TypePageCol (pEl);
	else
#else  /* __COLPAGE__ */
	NomElem = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName;
	strcat (&BufMenu[i], NomElem);
#endif /* __COLPAGE__ */
     }
   i += strlen (&BufMenu[i]) + 1;

   /* element selectionable suivant */
   strcpy (&BufMenu[i], "B> ");
   pEl = SelMenuNextEl;
   if (pEl != NULL && pEl->ElStructSchema != NULL)
     {
#ifdef __COLPAGE__
	if (pEl->ElTypeNumber == PageBreak + 1)
	   NomElem = TypePageCol (pEl);
	else
#else  /* __COLPAGE__ */
	NomElem = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName;
	strcat (&BufMenu[i], NomElem);
#endif /* __COLPAGE__ */
     }
   i += strlen (&BufMenu[i]) + 1;

   /* cherche le type du 1er element englobe' selectionable */
   strcpy (&BufMenu[i], "B_ ");
   pEl = SelMenuChildEl;
   if (pEl != NULL && pEl->ElStructSchema != NULL)
     {
#ifdef __COLPAGE__
	if (pEl->ElTypeNumber == PageBreak + 1)
	   NomElem = TypePageCol (pEl);
	else
#else  /* __COLPAGE__ */
	NomElem = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName;
	strcat (&BufMenu[i], NomElem);
#endif /* __COLPAGE__ */
     }
   i += strlen (&BufMenu[i]) + 1;
#ifdef __COLPAGE__
   /* composition du menu selection des elements de structure physique */
   /* TODO il faudrait tester si le document (la vue ? ) */
   /* est mis en page */
   /* affichage d'une ligne de separation entre structure logique */
   /* et structure physique */

   strcpy (&BufMenu[i], "S");
   i += strlen (&BufMenu[i]) + 1;

   /* on recherche d'abord la structure englobante */
   /* l'element de structure physique englobant est parmi les freres */
   /* precedents de FirstSelectedElement, et c'est une feuille */
   strcpy (&BufMenu[i], "B^ ");
   SelMenuPageColParent = NULL;
   stop = FALSE;
   pEl = FirstSelectedElement;
   while (!stop && pEl != NULL)
      if (pEl->ElPrevious != NULL)
	{
	   pEl = pEl->ElPrevious;
	   if (!ElementIsHidden (pEl))
	      pEl2 = pEl;
	   stop1 = FALSE;
	   while (!stop1)
	      if (pEl2->ElTerminal)
		{
		   stop1 = TRUE;
		   /* on determine dans quels cas on sort de la boucle */
		   /* principale de recherche */
		   if (pEl2->ElTypeNumber == PageBreak + 1)
		      /* TODO plus tard ?    && pEl2->ElViewPSchema == VueSch) */
		      if (FirstSelectedElement->ElTypeNumber != PageBreak + 1)
			 /* le pave de structure physique est le premier trouve */
			 /* il faudrait tester la vue ! */
			 stop = TRUE;
		      else if (FirstSelectedElement->ElViewPSchema == pEl2->ElViewPSchema)
			 /* on est sur une marque de la meme vue */
			 /* on analyse les englobements possibles */
			 /* si cela ne correspond pas, on continuera */
			 /* la recherche */
			 switch (FirstSelectedElement->ElPageType)
			       {
				  case ColBegin:
				  case ColComputed:
				  case ColUser:
				     if (pEl2->ElPageType == ColGroup)
					stop = TRUE;
				     break;
				  case ColGroup:
				     if (pEl2->ElPageType == PgBegin
					 || pEl2->ElPageType == PgComputed
					 || pEl2->ElPageType == PgUser)
					stop = TRUE;
				     break;
				  case PgBegin:
				  case PgComputed:
				  case PgUser:
				     stop = TRUE;
				     pEl = NULL;	/* pas d'englobant possible */
				     break;
			       }
		}
	      else if (pEl2->ElFirstChild == NULL)
		 stop1 = TRUE;	/* on a rien trouve sur pEl */
	   /* et ses descendants */
	   /* on remonte a la boucle englobante */
	      else
		{
		   pEl2 = pEl2->ElFirstChild;
		   while (pEl2->ElNext != NULL)
		      pEl2 = pEl2->ElNext;
		   while (ElementIsHidden (pEl2) && pEl2->ElPrevious != NULL)
		      pEl2 = pEl2->ElPrevious;
		   if (!ElementIsHidden (pEl2))
		      pEl = pEl2;	/* on continue avec pEl2 */
		   else
		      stop1 = TRUE;	/* on a rien trouve sur pEl */
		   /* et ses descendants */
		   /* on remonte a la boucle englobante */
		}
	}
      else
	 pEl = pEl->ElParent;
   if (pEl != NULL)
     {
	if (pEl->ElTypeNumber == PageBreak + 1)
	   NomElem = TypePageCol (pEl);
	else
	   NomElem = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName;
	strcat (&BufMenu[i], NomElem);
	SelMenuPageColParent = pEl;
     }
   i += strlen (&BufMenu[i]) + 1;

   /* on recherche ensuite la structure precedente */
   /* l'element de structure physique precedente est parmi les freres */
   /* precedents de FirstSelectedElement, et c'est une feuille */
   strcpy (&BufMenu[i], "< ");
   SelMenuPageColPrev = NULL;
   stop = FALSE;
   pEl = FirstSelectedElement;
   while (!stop && pEl != NULL)
      if (pEl->ElPrevious != NULL)
	{
	   pEl = pEl->ElPrevious;
	   if (!ElementIsHidden (pEl))
	      pEl2 = pEl;
	   stop1 = FALSE;
	   while (!stop1)
	      if (pEl2->ElTerminal)
		{
		   stop1 = TRUE;
		   /* on determine dans quels cas on sort de la boucle */
		   /* principale de recherche */
		   if (pEl2->ElTypeNumber == PageBreak + 1)
		      /* TODO plus tard ?    && pEl2->ElViewPSchema == VueSch) */
		      if (FirstSelectedElement->ElTypeNumber != PageBreak + 1)
			 /* pas de precedent si l'element n'est pas une */
			 /* marque page ou colonne */
			{
			   pEl = NULL;
			   stop = TRUE;
			}
		      else if (FirstSelectedElement->ElViewPSchema == pEl2->ElViewPSchema)
			 /* on est sur une marque de la meme vue */
			 /* on analyse les englobements possibles */
			 /* si cela ne correspond pas, on continuera */
			 /* la recherche */
			 switch (FirstSelectedElement->ElPageType)
			       {
				  case ColBegin:
				  case ColComputed:
				  case ColUser:
				     if (pEl2->ElPageType == ColBegin
					 || pEl2->ElPageType == ColComputed
					 || pEl2->ElPageType == ColUser)
					stop = TRUE;
				     break;
				  case ColGroup:
				     if (pEl2->ElPageType == ColGroup)
					stop = TRUE;
				     break;
				  case PgBegin:
				  case PgComputed:
				  case PgUser:
				     if (pEl2->ElPageType == PgBegin
					 || pEl2->ElPageType == PgComputed
					 || pEl2->ElPageType == PgUser)
					stop = TRUE;
				     break;
			       }
		}
	      else if (pEl2->ElFirstChild == NULL)
		 stop1 = TRUE;	/* on a rien trouve sur pEl */
	   /* et ses descendants */
	   /* on remonte a la boucle englobante */
	      else
		{
		   pEl2 = pEl2->ElFirstChild;
		   while (pEl2->ElNext != NULL)
		      pEl2 = pEl2->ElNext;
		   while (ElementIsHidden (pEl2) && pEl2->ElPrevious != NULL)
		      pEl2 = pEl2->ElPrevious;
		   if (!ElementIsHidden (pEl2))
		      pEl = pEl2;	/* on continue avec pEl2 */
		   else
		      stop1 = TRUE;	/* on a rien trouve sur pEl */
		   /* et ses descendants */
		   /* on remonte a la boucle englobante */
		}
	}
      else
	 pEl = pEl->ElParent;
   if (pEl != NULL)
     {
	if (pEl->ElTypeNumber == PageBreak + 1)
	   NomElem = TypePageCol (pEl);
	else
	   NomElem = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName;
	strcat (&BufMenu[i], NomElem);
	SelMenuPageColPrev = pEl;
     }
   i += strlen (&BufMenu[i]) + 1;

   /* on recherche ensuite la structure suivante */
   /* l'element de structure physique precedente est parmi les freres */
   /* suivants de FirstSelectedElement, et c'est une feuille */
   strcpy (&BufMenu[i], "> ");
   stop = FALSE;
   SelMenuPageColNext = NULL;
   pEl = LastSelectedElement;
   while (!stop && pEl != NULL)
      if (pEl->ElNext != NULL)
	{
	   pEl = pEl->ElNext;
	   if (!ElementIsHidden (pEl))
	      pEl2 = pEl;
	   stop1 = FALSE;
	   while (!stop1)
	      if (pEl2->ElTerminal)
		{
		   stop1 = TRUE;
		   /* on determine dans quels cas on sort de la boucle */
		   /* principale de recherche */
		   if (pEl2->ElTypeNumber == PageBreak + 1)
		      /* TODO plus tard ?    && pEl2->ElViewPSchema == VueSch) */
		      if (LastSelectedElement->ElTypeNumber != PageBreak + 1)
			 stop = TRUE;	/* on a trouve la structure suivante */
		      else if (LastSelectedElement->ElViewPSchema == pEl2->ElViewPSchema)
			 /* on est sur une marque de la meme vue */
			 /* on analyse les englobements possibles */
			 /* si cela ne correspond pas, on continuera */
			 /* la recherche */
			 switch (LastSelectedElement->ElPageType)
			       {
				  case ColBegin:
				  case ColComputed:
				  case ColUser:
				     if (pEl2->ElPageType == ColBegin
					 || pEl2->ElPageType == ColComputed
					 || pEl2->ElPageType == ColUser)
					stop = TRUE;
				     break;
				  case ColGroup:
				     if (pEl2->ElPageType == ColGroup)
					stop = TRUE;
				     break;
				  case PgBegin:
				  case PgComputed:
				  case PgUser:
				     if (pEl2->ElPageType == PgBegin
					 || pEl2->ElPageType == PgComputed
					 || pEl2->ElPageType == PgUser)
					stop = TRUE;
				     break;
			       }
		}
	      else if (pEl2->ElFirstChild == NULL)
		 stop1 = TRUE;	/* on a rien trouve sur pEl */
	   /* et ses descendants */
	   /* on remonte a la boucle englobante */
	      else
		{
		   pEl2 = pEl2->ElFirstChild;
		   while (ElementIsHidden (pEl2) && pEl2->ElNext != NULL)
		      pEl2 = pEl2->ElNext;
		   if (!ElementIsHidden (pEl2))
		      pEl = pEl2;	/* on continue avec pEl2 */
		   else
		      stop1 = TRUE;	/* on a rien trouve sur pEl */
		   /* et ses descendants */
		   /* on remonte a la boucle englobante */
		}
	}
      else
	 pEl = pEl->ElParent;

   if (pEl != NULL)
     {
	if (pEl->ElTypeNumber == PageBreak + 1)
	   NomElem = TypePageCol (pEl);
	else
	   NomElem = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName;
	strcat (&BufMenu[i], NomElem);
	SelMenuPageColNext = pEl;
     }
   i += strlen (&BufMenu[i]) + 1;

   /* on recherche en dernier la structure physique englobee  */
   /* l'element de structure physique englobee est parmi les freres */
   /* suivant de LastSelectedElement, et c'est une feuille */
   strcpy (&BufMenu[i], "_ ");
   stop = FALSE;
   pEl = LastSelectedElement;
   SelMenuPageColChild = NULL;
   while (!stop && pEl != NULL)
      if (pEl->ElNext != NULL)
	{
	   pEl = pEl->ElNext;
	   if (!ElementIsHidden (pEl))
	      pEl2 = pEl;
	   stop1 = FALSE;
	   while (!stop1)
	      if (pEl2->ElTerminal)
		{
		   stop1 = TRUE;
		   /* on determine dans quels cas on sort de la boucle */
		   /* principale de recherche */
		   if (pEl2->ElTypeNumber == PageBreak + 1)
		      /* TODO plus tard ?    && pEl2->ElViewPSchema == VueSch) */
		      if (LastSelectedElement->ElTypeNumber != PageBreak + 1)
			{
			   stop = TRUE;		/* pas de structure englobee */
			   pEl = NULL;
			}
		      else if (LastSelectedElement->ElViewPSchema == pEl2->ElViewPSchema)
			 /* dans tous les cas, on s'arretera */
			{
			   stop = TRUE;
			   /* on est sur une marque de la meme vue */
			   /* on analyse les englobements possibles */
			   /* si cela ne correspond pas, on arretera */
			   /* la recherche (continuer n'aurait aucun sens) */
			   switch (LastSelectedElement->ElPageType)
				 {
				    case ColBegin:
				    case ColComputed:
				    case ColUser:
				       pEl = NULL;	/* pas d'englobe pour les */
				       /* colonnes simples */
				       break;
				    case ColGroup:
				       if (!(pEl2->ElPageType == ColBegin
					  || pEl2->ElPageType == ColComputed
					     || pEl2->ElPageType == ColUser))
					  pEl = NULL;	/* pas d'englobe si pas */
				       /* colonnes simples */
				       break;
				    case PgBegin:
				    case PgComputed:
				    case PgUser:
				       if (pEl2->ElPageType != ColGroup)
					  pEl = NULL;	/* pas d'englobe si pas */
				       /* colonnes groupees */
				       break;
				 }
			}
		}
	      else if (pEl2->ElFirstChild == NULL)
		 stop1 = TRUE;	/* on a rien trouve sur pEl */
	   /* et ses descendants */
	   /* on remonte a la boucle englobante */
	      else
		{
		   pEl2 = pEl2->ElFirstChild;
		   while (ElementIsHidden (pEl2) && pEl2->ElNext != NULL)
		      pEl2 = pEl2->ElNext;
		   if (!ElementIsHidden (pEl2))
		      pEl = pEl2;	/* on continue avec pEl2 */
		   else
		      stop1 = TRUE;	/* on a rien trouve sur pEl */
		   /* et ses descendants */
		   /* on remonte a la boucle englobante */
		}
	}
      else
	 pEl = pEl->ElParent;
   if (pEl != NULL)
     {
	if (pEl->ElTypeNumber == PageBreak + 1)
	   NomElem = TypePageCol (pEl);
	else
	   NomElem = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName;
	strcat (&BufMenu[i], NomElem);
	SelMenuPageColChild = pEl;
     }
   i += strlen (&BufMenu[i]) + 1;
   return (9);
#else  /* __COLPAGE__ */
   return (4);
#endif /* __COLPAGE__ */
}

/*----------------------------------------------------------------------
   UpdateSelectMenu                                                     
   Met a jour le menu de Selection                                   
   - de toutes les frames ouvertes du document pDoc                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateSelectMenu (PtrDocument pDoc)
#else  /* __STDC__ */
void                UpdateSelectMenu (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 NbItemSel;
   char                BufMenuSel[MAX_TXT_LEN];
   int                 vue, menu, menuID;
   int                 frame, ref;
   Document            document;
   Menu_Ctl           *ptrmenu;

   if (pDoc == SelectedDocument)
      NbItemSel = BuildSelectMenu (BufMenuSel);
   else
      NbItemSel = 0;

   document = (Document) IdentDocument (pDoc);
   /* Traite toutes les vues de l'arbre principal */
   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
     {
	frame = pDoc->DocViewFrame[vue - 1];
	if (frame != 0 && FrameTable[frame].MenuSelect != -1)
	  {
	     menuID = FrameTable[frame].MenuSelect;
	     menu = FindMenu (frame, menuID, &ptrmenu) - 1;
	     ref = (menu * MAX_ITEM) + frame + MAX_LocalMenu;
	     if (NbItemSel == 0)
	       {
		  /* le menu Selection contient au moins un item */
		  TtaSetMenuOff (document, vue, menuID);
		  TtaDestroyDialogue (ref);
	       }
	     else
	       {
		  TtaNewPulldown (ref, FrameTable[frame].WdMenus[menu], NULL,
				  NbItemSel, BufMenuSel, NULL);
		  TtaSetMenuOn (document, vue, menuID);
	       }
	  }
     }

   /* Traite toutes les vues des arbres associes */
   for (vue = 1; vue <= MAX_ASSOC_DOC; vue++)
     {
	frame = pDoc->DocAssocFrame[vue - 1];
	if (frame != 0 && FrameTable[frame].MenuSelect != -1)
	  {
	     menuID = FrameTable[frame].MenuSelect;
	     menu = FindMenu (frame, menuID, &ptrmenu) - 1;
	     ref = (menu * MAX_ITEM) + frame + MAX_LocalMenu;
	     if (NbItemSel == 0)
	       {
		  /* le menu Selection contient au moins un item */
		  TtaSetMenuOff (document, vue, menu);
		  TtaDestroyDialogue (ref);
	       }
	     else
	       {
		  TtaNewPulldown (ref, FrameTable[frame].WdMenus[menu], NULL,
				  NbItemSel, BufMenuSel, NULL);
		  TtaSetMenuOn (document, vue, menu);
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   CallbackAttrMenu traite les retours du menu 'Attributs':        
   cree un formulaire pour saisir la valeur de l'attribut choisi.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackSelectMenu (int refmenu, int val, int frame)
#else  /* __STDC__ */
void                CallbackSelectMenu (refmenu, val, frame)
int                 refmenu;
int                 val;
int                 frame;

#endif /* __STDC__ */
{
   SelectAround (val);
}

/*----------------------------------------------------------------------
   EditingLoadResources connecte les fonctions de selection.          
  ----------------------------------------------------------------------*/
void                SelectionMenuLoadResources ()
{
   if (ThotLocalActions[T_chselect] == NULL)
     {
	/* Connecte les actions de selection */
	TteConnectAction (T_chselect, (Proc) UpdateSelectMenu);
	TteConnectAction (T_rselect, (Proc) CallbackSelectMenu);
     }
}
