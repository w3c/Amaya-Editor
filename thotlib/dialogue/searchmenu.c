/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*
   cherche.c : gestion de la commande de recherche.

 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "dialog.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"
#include "interface.h"
#include "appdialogue.h"

#define EXPORT extern
#include "platform_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "tree_f.h"
#include "structcreation_f.h"
#include "references_f.h"
#include "structmodif_f.h"
#include "viewcommands_f.h"
#include "ouvre_f.h"
#include "absboxes_f.h"
#include "changeabsbox_f.h"
#include "schemas_f.h"
#include "createabsbox_f.h"
#include "search_f.h"
#include "exceptions_f.h"
#include "word_f.h"
#include "appli_f.h"
#include "searchmenu_f.h"
#include "structschema_f.h"
#include "fileaccess_f.h"
#include "memory_f.h"
#include "regexp_f.h"
#include "views_f.h"
#include "structselect_f.h"
#include "content_f.h"

static boolean      CaseDistinction;	/* dans la recherche de texte, on */

					/* confond majuscules et minuscules */
static boolean      WithReplace;	/* il faut remplacer les chaines trouvees */

/* pointeur sur le domaine de recherche concernant la commande en cours */
static PtrSearchContext    searchDomain = NULL;

/* entree desactivee dans le sous-menu "Ou chercher" */
static PtrDocument  SearchedPageDoc;	/* le document ou on cherche une page */
static PtrElement   SearchedPageRoot;		/* racine de l'arbre ou on cherche */

					/* une page */
static int          ViewSearchedPageDoc;		/* vue (du document) pour laquelle on */

					/* cherche une page */
static int          SearchedPageSchView;		/* vue (du schema de presentation) */

					/* pour laquelle on cherche une page */
static int          SearchedPageNumber;	/* numero de la page chercheee */


static char         pPrecedentString[MAX_CHAR];	/* la chaine cherchee precedente */
static char         pSearchedString[MAX_CHAR];	/* la chaine cherchee */
static int          SearchedStringLen;	/* longueur de cette chaine */
static boolean      RegExp;	/* pSearchedString est une 

						   expression reguliere */
static char         pReplaceString[MAX_CHAR];	/* la chaine de remplacement */
static int          ReplaceStringLen;	/* longueur de cette chaine */

					  /* si attribut  textuel */
static PtrReference CurrRef;	/* pointeur sur la reference courante */
static PtrDocument  CurrRefDoc;	/* document auquel appartient la ref cour */

static PtrElement   CurrRefElem;	/* element dont on cherche les references */
static PtrDocument  CurrRefElemDoc;	/* document auquel appartient */

					  /* l'element dont on cherche les references */
static PtrExternalDoc pExtCurrDoc;	/* pointeur sur le descripteur de document */

					  /* externe contenant la reference courante */
static boolean      SearchReferenceEnd;

static boolean      AutoReplace;
boolean             StartSearch;
static boolean      ReplaceDone;
static boolean      DoReplace;

static boolean      TextOK = FALSE;
static PtrElement   ElemTextOK;
static int          FirstCharTextOK;
static int          LastCharTextOK;
static PtrDocument  DocTextOK;

static int          ReturnValueSelectReferMenu;

#ifdef __STDC__
void                SearchLoadResources (void);

#else  /* __STDC__ */
void                SearchLoadResources ();

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   Initialise le sous-menu "Ou rechercher"                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitMenuWhereToSearch (int ref)
#else  /* __STDC__ */
static void         InitMenuWhereToSearch (ref)
int                 ref;

#endif /* __STDC__ */
{
   int                 i;
   char                string[200];

   i = 0;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, BEFORE_SEL));
   i += strlen (&string[i]) + 1;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, WITHIN_SEL));
   i += strlen (&string[i]) + 1;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, AFTER_SEL));
   i += strlen (&string[i]) + 1;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, IN_WHOLE_DOC));
   /* sous-menu Ou` rechercher element vide */
   TtaNewSubmenu (NumMenuOrSearchText, ref, 0,
	     TtaGetMessage (LIB, SEARCH_WHERE), 4, string, NULL, FALSE);
   TtaSetMenuForm (NumMenuOrSearchText, 2);
}

/*----------------------------------------------------------------------
   Active le sous-menu "Ou rechercher"                               
  ----------------------------------------------------------------------*/
static void         ActivateMenuWhereToSearch ()
{
   TtaRedrawMenuEntry (NumMenuOrSearchText, 0, NULL, -1, 1);
   TtaRedrawMenuEntry (NumMenuOrSearchText, 1, NULL, -1, 1);
   TtaRedrawMenuEntry (NumMenuOrSearchText, 2, NULL, -1, 1);
}


/*----------------------------------------------------------------------
   CallbackWhereToSearch traite le retour du sous-menu "Ou rechercher" 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackWhereToSearch (int ref, int val)
#else  /* __STDC__ */
void                CallbackWhereToSearch (ref, val)
int                 val;
int                 menu;

#endif /* __STDC__ */
{
   /* determine le point de depart de la recherche et le point */
   /* de terminaison */
   switch (val)
	 {
	    case 0:
	       /* ElemIsBefore la selection */
	       searchDomain->SStartToEnd = FALSE;
	       break;
	    case 1:
	       /* Dans la selection */
	       StartSearch = TRUE;
 	       break;
	    case 2:
	       /* Apres la selection */
	       searchDomain->SStartToEnd = TRUE;
	       break;
	    case 3:
	       /* Dans tout le document */
	       StartSearch = TRUE;
	       break;
	 }
   if (StartSearch)
     {
	InitSearchDomain (val, searchDomain);
	if (!AutoReplace)
	   /* On prepare la recherche suivante */
	   if (searchDomain->SStartToEnd)
	      TtaSetMenuForm (ref, 2);	/* Apres selection */
	   else
	      TtaSetMenuForm (ref, 0);	/* ElemIsBefore selection */
     }
}


/*----------------------------------------------------------------------
   ChElemVide cherche dans le domaine decrit par context et a      
   partir de (et a l'interieur de) l'element elCour,       
   le premier element vide.                                
   Retourne vrai si trouve', et dans ce cas selectionne    
   l'element trouve'.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      SearchEmptyElem (PtrElement pCurrEl, PtrSearchContext pContext)
#else  /* __STDC__ */
static boolean      SearchEmptyElem (pCurrEl, pContext)
PtrElement          pCurrEl;
PtrSearchContext    pContext;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrElement          pParent;
   PtrElement          pNextEl;
   int                 i;
   boolean             found;
   boolean             ok;

   found = FALSE;
   ok = FALSE;
   if (pCurrEl == NULL)
      /* debut de recherche */
      if (pContext->SStartToEnd)
	{
	   pEl = pContext->SStartElement;
	   if (pEl == NULL)
	      pEl = pContext->SDocument->DocRootElement;
	}
      else
	 pEl = pContext->SEndElement;
   else
      pEl = pCurrEl;
   while (pEl != NULL && !found)
     {
	if (pContext->SStartToEnd)
	   /* Recherche en avant */
	   pEl = FwdSearchRefOrEmptyElem (pEl, 1);
	else
	   /* Recherche en arriere */
	   pEl = BackSearchRefOrEmptyElem (pEl, 1);
	if (pEl != NULL)
	   /* on a trouve' un element vide, on verifie que cet element ne */
	   /* fait pas partie d'une inclusion et n'est pas cache' a */
	   /* l'utilisateur */
	  {
	     pParent = pEl;
	     while (pParent->ElParent != NULL && pParent->ElSource == NULL)
		pParent = pParent->ElParent;
	     if (pParent->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		   /* l'element n'est pas cache' a l'utilisateur */
		   found = TRUE;
	  }
     }

   if (pEl != NULL && found)
      /* on a trouve' */
     {
	/* l'element trouve' est pointe' par pEl */
	if (pContext->SStartToEnd)
	  {
	     if (pContext->SEndElement != NULL)
		/* il faut s'arreter avant l'extremite' du document */
		if (pEl != pContext->SEndElement)
		   /*l'element trouve' n'est pas l'element ou il faut s'arreter */
		   if (ElemIsBefore (pContext->SEndElement, pEl))
		      /* l'element trouve' est apres l'element de fin, on */
		      /* fait comme si on n'avait pas trouve' */
		      pEl = NULL;
	  }
	else if (pContext->SStartElement != NULL)
	   /* il faut s'arreter avant l'extremite' du document */
	   if (pEl != pContext->SStartElement)
	      /*l'element trouve' n'est pas l'element ou il faut s'arreter */
	      if (ElemIsBefore (pEl, pContext->SStartElement))
		 /* l'element trouve' est avant le debut du domaine, on */
		 /* fait comme si on n'avait pas trouve' */
		 pEl = NULL;
	if (pEl != NULL)
	   SelectElementWithEvent (pContext->SDocument, pEl, TRUE, FALSE);
     }
   ok = (pEl != NULL);
   if (!found)
      if (pContext->SWholeDocument)
	 /* il faut rechercher dans tout le document */
	 /* cherche l'arbre a traiter apres celui ou` on n'a pas trouve' */
	 if (NextTree (&pNextEl, &i, pContext))
	    /* il y a un autre arbre a traiter, on continue avec le */
	    /* debut de ce nouvel arbre */
	    ok = SearchEmptyElem (pNextEl, pContext);
   return ok;
}


/*----------------------------------------------------------------------
   ChReferVide cherche dans le domaine decrit par pContext et a     
   partir de (et a l'interieur de) l'element pCurrEl,       
   la premiere reference vide.                             
   Retourne vrai si trouve', et dans ce cas selectionne    
   l'element trouve'.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      SearchEmptyRefer (PtrElement pCurrEl, PtrSearchContext pContext)
#else  /* __STDC__ */
static boolean      SearchEmptyRefer (pCurrEl, pContext)
PtrElement          pCurrEl;
PtrSearchContext    pContext;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrElement          pParent;
   PtrElement          pNextEl;
   int                 i;
   boolean             found;
   boolean             ok;

   found = FALSE;
   ok = FALSE;
   if (pCurrEl == NULL)
      /* debut de recherche */
      if (pContext->SStartToEnd)
	{
	   pEl = pContext->SStartElement;
	   if (pEl == NULL)
	      pEl = pContext->SDocument->DocRootElement;
	}
      else
	 pEl = pContext->SEndElement;
   else
      pEl = pCurrEl;
   while (!found && pEl != NULL)
     {
	if (pContext->SStartToEnd)
	   /* Recherche en avant */
	   pEl = FwdSearchRefOrEmptyElem (pEl, 2);
	else
	   /* Recherche en arriere */
	   pEl = BackSearchRefOrEmptyElem (pEl, 2);
	if (pEl != NULL)
	   /* on a trouve' une reference vide, on verifie que cette */
	   /* reference ne fait pas partie d'une inclusion ou d'un arbre */
	   /* cache' a l'utilisateur */
	  {
	     pParent = pEl;
	     while (pParent->ElParent != NULL && pParent->ElSource == NULL)
		pParent = pParent->ElParent;
	     if (pParent->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		   /* l'element n'est pas cache' a l'utilisateur */
		   if (pEl->ElReference == NULL)
		      /* la reference est vide */
		      found = TRUE;
		   else
		      /* la reference n'est pas vide */
		   if (pEl->ElReference->RdReferred == NULL)
		      /* la reference est vide */
		      found = TRUE;
		   else if (pEl->ElReference->RdInternalRef)
		      if (!pEl->ElReference->RdReferred->ReExternalRef)
			 if (IsASavedElement (pEl->ElReference->RdReferred->ReReferredElem))
			    /* l'element reference' est dans le buffer des */
			    /* elements coupe's */
			    found = TRUE;
	  }
     }
   if (pEl != NULL && found)
      /* on a trouve' */
     {
	/* l'element trouve' est pointe' par pEl */
	if (pContext->SStartToEnd)
	  {
	     if (pContext->SEndElement != NULL)
		/* il faut s'arreter avant l'extremite' du document */
		if (pEl != pContext->SEndElement)
		   /*l'element trouve' n'est pas l'element ou il faut s'arreter */
		   if (ElemIsBefore (pContext->SEndElement, pEl))
		      /* l'element trouve' est apres l'element de fin, on */
		      /* fait comme si on n'avait pas trouve' */
		      pEl = NULL;
	  }
	else if (pContext->SStartElement != NULL)
	   /* il faut s'arreter avant l'extremite' du document */
	   if (pEl != pContext->SStartElement)
	      /*l'element trouve' n'est pas l'element ou il faut s'arreter */
	      if (ElemIsBefore (pEl, pContext->SStartElement))
		 /* l'element trouve' est avant le debut du domaine, on */
		 /* fait comme si on n'avait pas trouve' */
		 pEl = NULL;
	if (pEl != NULL)
	   SelectElementWithEvent (pContext->SDocument, pEl, TRUE, FALSE);
     }
   ok = (pEl != NULL);
   if (!found)
      if (pContext->SWholeDocument)
	 /* il faut rechercher dans tout le document */
	 /* cherche l'arbre a traiter apres celui ou` on n'a pas trouve' */
	 if (NextTree (&pNextEl, &i, pContext))
	    /* il y a un autre arbre a traiter, on continue avec le */
	    /* debut de ce nouvel arbre */
	    ok = SearchEmptyRefer (pNextEl, pContext);
   return ok;
}


/*----------------------------------------------------------------------
   CallbackSearchEmptyEl traite les retours du formulaire Recherche   
   elements vides.                                          
   ref: reference de l'element de dialogue a traiter        
   val: valeur de l'element de dialogue                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackSearchEmptyEl (int ref, int val)
#else  /* __STDC__ */
void                CallbackSearchEmptyEl (ref, val)
int                 ref;
int                 val;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrElement          pCurrEl;
   PtrDocument         pDocSel;
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   int                 firstChar;
   int                 lastChar;

   if (searchDomain->SDocument != NULL)
      if (searchDomain->SDocument->DocSSchema != NULL)
	 /* le document concerne' est toujours la */
	 /* feuille de dialogue Rechercher element vide */
	 if (val == 1)
	    /* lancer la recherche */
	   {
	      if (StartSearch)
		{
		   pCurrEl = NULL;
		   StartSearch = FALSE;
		}
	      else
		{
		   ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar,
				    &lastChar);
		   if (!ok || pDocSel != searchDomain->SDocument)
		      /* la selection a change' de document, on refuse */
		     {
			TtaNewLabel (NumLabelEmptyElemNotFound,
				     NumFormSearchEmptyElement,
			   TtaGetMessage (LIB, DO_NOT_CHANGE_DOC));
			StartSearch = TRUE;
			return;
		     }
		   else if (searchDomain->SStartToEnd)
		      pCurrEl = pLastSel;
		   else
		      pCurrEl = pFirstSel;
		}
	      ok = SearchEmptyElem (pCurrEl, searchDomain);
	      if (ok)
		{
		   /* on a trouve' et selectionne' */
		   StartSearch = FALSE;
		   /* on reactive les entrees du sous-menu "Ou chercher" */
		   ActivateMenuWhereToSearch ();
		   /* efface le message "Pas trouve'" dans le formulaire */
		   TtaNewLabel (NumLabelEmptyElemNotFound, NumFormSearchEmptyElement, " ");
		}
	      else
		 /* on n'a pas trouve' */
		{
		   /* message 'Pas trouve' dans le formulaire */
		   TtaNewLabel (NumLabelEmptyElemNotFound, NumFormSearchEmptyElement,
				TtaGetMessage (LIB, NOT_FOUND));
		   StartSearch = TRUE;
		}
	   }
}


/*----------------------------------------------------------------------
   ChercherElementVide lance la commande de recherche des elements 
   vides pour le document pDoc.                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSearchEmptyElement (Document document, View view)
#else  /* __STDC__ */
void                TtcSearchEmptyElement (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrDocument         pDocSel;
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   int                 firstChar;
   int                 lastChar;
   char                buffTitle[200];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   SearchLoadResources ();

   ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
   if (!ok)
     {
	pDocSel = pDoc;
	pFirstSel = pDoc->DocRootElement;
     }
   if (pDocSel != pDoc)
      /* pas de selection dans le document d'ou vient la commande */
      return;

   /* fait disparaitre les autres formulaires de recherche qui sont affiches */
   TtaDestroyDialogue (NumFormSearchEmptyReference);
   TtaDestroyDialogue (NumFormSearchText);

   StartSearch = TRUE;
   /* feuille de dialogue Rechercher element vide */
   /* compose le titre "Recherche dans le document..." */
   strcpy (buffTitle, TtaGetMessage (LIB, SEARCH_IN));
   strcat (buffTitle, pDoc->DocDName);
   TtaNewSheet (NumFormSearchEmptyElement, TtaGetViewFrame (document, view), 0, 0,
		buffTitle,
		1, TtaGetMessage (LIB, LIB_CONFIRM), FALSE, 3, 'L', D_DONE);
   /* label indiquant la recherche d'elements vides */
   TtaNewLabel (NumLabelDocSearcheEmptyElement, NumFormSearchEmptyElement,
		TtaGetMessage (LIB, SEARCH_EMPTY_EL));
   InitMenuWhereToSearch (NumFormSearchEmptyElement);

   /* le message "Pas trouve'" */
   TtaNewLabel (NumLabelEmptyElemNotFound, NumFormSearchEmptyElement, " ");
   /* active le formulaire */
   TtaShowDialogue (NumFormSearchEmptyElement, TRUE);
   if (!ok)
     {
	InitSearchDomain (3, searchDomain);
	TtaSetMenuForm (NumMenuOrSearchText, 3);
     }
   searchDomain->SDocument = pDoc;
}


/*----------------------------------------------------------------------
   CallbackSearchEmptyref traite les retours du formulaire Recherche  
   references vides.                                        
   ref: reference de l'element de dialogue a traiter        
   val: valeur de l'element de dialogue                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackSearchEmptyref (int ref, int val)
#else  /* __STDC__ */
void                CallbackSearchEmptyref (ref, val)
int                 ref;
int                 val;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrElement          pCurrEl;
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   PtrDocument         pDocSel;
   int                 firstChar;
   int                 lastChar;

   if (searchDomain->SDocument != NULL)
      if (searchDomain->SDocument->DocSSchema != NULL)
	{
	   /* feuille de dialogue Rechercher reference vide */
	   if (val == 1)
	     {
		/* lancer la recherche */
		if (StartSearch)
		  {
		     pCurrEl = NULL;
		     StartSearch = FALSE;
		  }
		else
		  {
		     ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
		     if (!ok || pDocSel != searchDomain->SDocument)
			/* la selection a change' de document, on refuse */
		       {
			  TtaNewLabel (NumLabelEmptyRefereneceNotFound,
				       NumFormSearchEmptyReference,
			   TtaGetMessage (LIB, DO_NOT_CHANGE_DOC));
			  StartSearch = TRUE;
			  return;
		       }
		     else if (searchDomain->SStartToEnd)
			pCurrEl = pLastSel;
		     else
			pCurrEl = pFirstSel;
		  }
		ok = SearchEmptyRefer (pCurrEl, searchDomain);
		if (ok)
		  {
		     StartSearch = FALSE;
		     /* on a trouve' et selectionne'. On reactive les */
		     /* entrees du sous-menu "Ou chercher" */
		     ActivateMenuWhereToSearch ();
		     /* efface le message 'Pas trouve' dans la feuille de */
		     /* saisie */
		     TtaNewLabel (NumLabelEmptyRefereneceNotFound, NumFormSearchEmptyReference, " ");
		  }
		else
		   /* on n'a pas trouve' */
		  {
		     /* message 'Pas trouve' dans la feuille de saisie */
		     TtaNewLabel (NumLabelEmptyRefereneceNotFound, NumFormSearchEmptyReference,
				  TtaGetMessage (LIB, NOT_FOUND));
		     StartSearch = TRUE;
		  }
	     }
	   else
	      TtaDestroyDialogue (NumFormSearchEmptyReference);
	}
}


/*----------------------------------------------------------------------
   ChercherReferenceVide lance la commande de recherche des        
   references vides pour le document pDoc.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSearchEmptyReference (Document document, View view)
#else  /* __STDC__ */
void                TtcSearchEmptyReference (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrDocument         pDocSel;
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   int                 firstChar;
   int                 lastChar;
   char                buffTitle[200];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   SearchLoadResources ();

   ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
   if (!ok)
     {
	pDocSel = pDoc;
	pFirstSel = pDoc->DocRootElement;
     }

   if (pDocSel != pDoc)
      /* pas de selection dans le document d'ou vient la commande */
      return;

   /* fait disparaitre les autres formulaires de recherche qui sont affiches */
   TtaDestroyDialogue (NumFormSearchEmptyElement);
   TtaDestroyDialogue (NumFormSearchText);

   StartSearch = TRUE;
   /* compose le titre "Recherche dans le document..." */
   strcpy (buffTitle, TtaGetMessage (LIB, SEARCH_IN));
   strcat (buffTitle, pDoc->DocDName);
   /* feuille de dialogue Rechercher reference vide */
   TtaNewSheet (NumFormSearchEmptyReference, TtaGetViewFrame (document, view), 0, 0,
		buffTitle,
		1, TtaGetMessage (LIB, LIB_CONFIRM), FALSE, 3, 'L', D_DONE);

   /* label indiquant la recherche de references vides */
   TtaNewLabel (NumLabelDocSearchEmptyRef, NumFormSearchEmptyReference,
		TtaGetMessage (LIB, SEARCH_EMPTY_REF));
   InitMenuWhereToSearch (NumFormSearchEmptyReference);

   /* le message "Pas trouve'" */
   TtaNewLabel (NumLabelEmptyRefereneceNotFound, NumFormSearchEmptyReference, " ");
   /* active le formulaire */
   TtaShowDialogue (NumFormSearchEmptyReference, TRUE);
   if (!ok)
     {
	InitSearchDomain (3, searchDomain);
	TtaSetMenuForm (NumMenuOrSearchText, 3);
     }
   searchDomain->SDocument = pDoc;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CallbackReferenceMenu (int val)
#else  /* __STDC__ */
void                CallbackReferenceMenu (val)
int                 val;

#endif /* __STDC__ */
{
   ReturnValueSelectReferMenu = val;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildReferenceMenu (char *bufMenu, int nbEntries, int *selEntry)
#else  /* __STDC__ */
void                BuildReferenceMenu (bufMenu, nbEntries, selEntry)
char               *bufMenu;
int                 nbEntries;
int                *selEntry;

#endif /* __STDC__ */
{
   char                bufMenuB[MAX_TXT_LEN];
   char               *src;
   char               *dest;
   int                 k, l;

   ReturnValueSelectReferMenu = 0;
   /* ajoute 'B' au debut de chaque entree */
   dest = &bufMenuB[0];
   src = &bufMenu[0];
   for (k = 1; k <= nbEntries; k++)
     {
	strcpy (dest, "B");
	dest++;
	l = strlen (src);
	strcpy (dest, src);
	dest += l + 1;
	src += l + 1;
     }
   TtaNewPopup (NumMenuReferenceChoice, 0, TtaGetMessage (LIB, LINK), nbEntries,
		bufMenuB, NULL, 'L');
   TtaSetDialoguePosition ();
   TtaShowDialogue (NumMenuReferenceChoice, FALSE);
   /* attend que l'utilisateur ait repondu au menu et que le */
   /* mediateur ait appele' CallbackReferenceMenu */
   TtaWaitShowDialogue ();
   *selEntry = ReturnValueSelectReferMenu;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SearchAReference (boolean docExtNext)
#else  /* __STDC__ */
static void         SearchAReference (docExtNext)
boolean             docExtNext;

#endif /* __STDC__ */
{
   char                msg[100];

   /* chercher une reference */
   FindReference (&CurrRef, &CurrRefDoc, &CurrRefElem,
	     &CurrRefElemDoc, &pExtCurrDoc, docExtNext);
   if (CurrRef != NULL)
      /* on a trouve', on efface le message qui traine */
      strcpy (msg, " ");
   else if (pExtCurrDoc != NULL)
      /* references dans un document non charge' */
     {
	strcpy (msg, TtaGetMessage (LIB, REF_IN_DOC));
	GetDocName (pExtCurrDoc->EdDocIdent, msg + strlen (msg));
     }
   else
     {
	/* on n'a trouve' aucune reference */
	SearchReferenceEnd = TRUE;
	strcpy (msg, TtaGetMessage (LIB, NOT_FOUND));
     }
   TtaNewLabel (NumLabelReferenceNotFound, NumFormSearchReference, msg);
}


/*----------------------------------------------------------------------
   BuildSearchReferences lance la commande de recherche des        
   references qui designent l'element selectionne' du      
   document pDoc.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildSearchReferences (PtrDocument pDoc)
#else  /* __STDC__ */
void                BuildSearchReferences (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   PtrDocument         pDocSel;
   int                 firstChar;
   int                 lastChar;
   char                bufText[200];
   boolean             found;

   if (!GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar))
      /* message 'Pas de selection' */
      TtaDisplaySimpleMessage (INFO, LIB, DEBUG_NO_SEL);
   else
     {
	/* initialise les variables de recherche des references */
	CurrRef = NULL;
	CurrRefElem = NULL;
	pExtCurrDoc = NULL;
	/* cherche le premier ascendant reference' de la selection */
	pEl = pFirstSel;
	found = FALSE;
	do
	  {
	     if (pEl->ElReferredDescr != NULL)
		/* l'element a un descripteur d'element reference' */
		if (pEl->ElReferredDescr->ReFirstReference != NULL ||
		    pEl->ElReferredDescr->ReExtDocRef != NULL)
		   /* l'element est effectivement reference' */
		   found = TRUE;
	     if (!found)
		/* l'element n'est pas reference', on passe au pere */
		pEl = pEl->ElParent;
	  }
	while (!found && pEl != NULL);
	if (found)
	   /* il y a effectivement un ascendant reference' */
	  {
	     SearchReferenceEnd = FALSE;
	     /* initialise le label du formulaire en y mettant le type de */
	     /* l'element reference' */
	     strcpy (bufText, TtaGetMessage (LIB, EARCH_REF_TO_EL));
	     strcat (bufText, " ");
	     strcat (bufText, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
	     TtaNewLabel (NumLabelSearchReference, NumFormSearchReference,
			  bufText);
	     TtaNewLabel (NumLabelReferenceNotFound, NumFormSearchReference,
			  " ");
	     /* active le formulaire de recherche */
	     TtaShowDialogue (NumFormSearchReference, FALSE);
	  }
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSearchReference (Document document, View view)
#else  /* __STDC__ */
void                TtcSearchReference (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   char                BufMenu[100];
   int                 i;

   pDoc = LoadedDocument[document - 1];
   SearchLoadResources ();

   if (pDoc != NULL)
     {
	/* feuille de dialogue pour la recherche des references a` un element */
	strcpy (BufMenu, TtaGetMessage (LIB, SEARCH));
	i = strlen (TtaGetMessage (LIB, SEARCH)) + 1;
	strcpy (BufMenu + i, TtaGetMessage (LIB, OPEN));
	TtaNewSheet (NumFormSearchReference, TtaGetViewFrame (document, view), 0, 0,
		     TtaGetMessage (LIB, SEARCH_REF), 2, BufMenu, TRUE, 1, 'L', D_DONE);
	/* label indiquant le type d'element dont on cherche les references */
	TtaNewLabel (NumLabelSearchReference, NumFormSearchReference,
		     TtaGetMessage (LIB, EARCH_REF_TO_EL));

	/* label "Pas trouve'" pour Recherche des references a` un element */
	TtaNewLabel (NumLabelReferenceNotFound, NumFormSearchReference, " ");
	BuildSearchReferences (pDoc);
     }
}


/*----------------------------------------------------------------------
   CallbackReferenceTo      traite les retours de la feuille de     
   dialogue "Recherche les references a`".                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackReferenceTo (int ref, int data)
#else  /* __STDC__ */
void                CallbackReferenceTo (ref, data)
int                 ref;
int                 data;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;

   if (ref == NumFormSearchReference && !SearchReferenceEnd)
      switch (data)
	    {
	       case 1:
		  if (CurrRefDoc != NULL)
		     if (CurrRefDoc->DocSSchema == NULL)
			/* le document ou se trouvait la reference courante a */
			/* ete ferme'. On repart du debut */
		       {
			  CurrRef = NULL;
			  CurrRefElem = NULL;
			  pExtCurrDoc = NULL;
		       }
		  SearchAReference (TRUE);
		  break;

	       case 2:
		  /* charger le document contenant la reference cherchee */
		  if (CurrRef == NULL && pExtCurrDoc != NULL)
		     /* il s'agit bien d'une reference dans un document externe */
		     /* non charge' */
		     if (!DocIdentIsNull (pExtCurrDoc->EdDocIdent))
		       {
			  /* acquiert et initialise un descripteur de document */
			  CreateDocument (&pDoc);
			  if (pDoc != NULL)
			    {
			       /* charge le document */
			       LoadDocument (&pDoc, pExtCurrDoc->EdDocIdent);
			       if (pDoc != NULL)
				 {
				    /* efface le label "References dans le document X" */
				    TtaNewLabel (NumLabelReferenceNotFound,
					      NumFormSearchReference, " ");
				    SearchAReference (FALSE);
				 }
			    }
		       }
		  break;
	       default:
		  TtaDestroyDialogue (ref);
		  break;
	    }
}


/*----------------------------------------------------------------------
   ChercherRemplacerTexte lance la commande de recherche et/ou     
   remplacement de texte, type d'element et attribut       
   pour le document pDoc.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSearchText (Document document, View view)
#else  /* __STDC__ */
void                TtcSearchText (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrDocument         pDocSel;
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   int                 firstChar;
   int                 lastChar, i;
   char                bufTitle[200], string[200];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];

   ok = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
   if (ok)
      if (pDoc != pDocSel)
	 ok = FALSE;
   if (!ok)
     {
	pDocSel = pDoc;
	pFirstSel = pLastSel = pDoc->DocRootElement;
	firstChar = lastChar = 0;
     }

   /* fait disparaitre les autres formulaires de recherche qui sont affiches */
   TtaDestroyDialogue (NumFormSearchEmptyElement);
   TtaDestroyDialogue (NumFormSearchEmptyReference);
   StartSearch = TRUE;

   /* compose le titre du formulaire "Recherche dans le document..." */
   strcpy (bufTitle, TtaGetMessage (LIB, SEARCH_IN));
   strcat (bufTitle, pDoc->DocDName);
   /* feuille de dialogue Rechercher texte et structure */
   strcpy (string, TtaGetMessage (LIB, LIB_CONFIRM));
   i = strlen (TtaGetMessage (LIB, LIB_CONFIRM)) + 1;
   strcpy (string + i, TtaGetMessage (LIB, DO_NOT_REPLACE));
   TtaNewSheet (NumFormSearchText, TtaGetViewFrame (document, view), 0, 0,
		bufTitle, 2, string, FALSE, 6, 'L', D_DONE);

   /* zone de saisie du texte a` rechercher */
   TtaNewTextForm (NumZoneTextSearch, NumFormSearchText,
		   TtaGetMessage (LIB, SEARCH_FOR), 30, 1, FALSE);
   TtaSetTextForm (NumZoneTextSearch, pSearchedString);

   /* Toggle button "Expression reguliere" */
   i = 0;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, REG_EXP));
   i += strlen (&string[i]) + 1;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, UPPERCASE_EQ_LOWERCASE));
   TtaNewToggleMenu (NumToggleRegExp, NumFormSearchText,
		     NULL, 2, string, NULL, FALSE);
   TtaSetToggleMenu (NumToggleRegExp, 0, RegExp);
   TtaSetToggleMenu (NumToggleRegExp, 1, CaseDistinction);

   /* zone de saisie du texte de remplacement */
   TtaNewTextForm (NumZoneTextReplace, NumFormSearchText,
		   TtaGetMessage (LIB, REPLACE_BY), 30, 1, TRUE);
   TtaSetTextForm (NumZoneTextReplace, pReplaceString);

   /* sous-menu mode de remplacement */
   if (!pDoc->DocReadOnly)
     {
	/* on autorise les remplacement */
	/* attache le sous-menu remplacement */
	i = 0;
	sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, NO_REPLACE));
	i += strlen (&string[i]) + 1;
	sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, REPLACE_ON_REQU));
	i += strlen (&string[i]) + 1;
	sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, AUTO_REPLACE));
	TtaNewSubmenu (NumMenuReplaceMode, NumFormSearchText, 0,
		  TtaGetMessage (LIB, REPLACE), 3, string, NULL, FALSE);
	if (WithReplace)
	  {
	     if (AutoReplace)
		TtaSetMenuForm (NumMenuReplaceMode, 2);
	     else
		TtaSetMenuForm (NumMenuReplaceMode, 1);
	  }
	else
	   TtaSetMenuForm (NumMenuReplaceMode, 0);
     }
   else
      TtaNewLabel (NumMenuReplaceMode, NumFormSearchText, " ");

   /* sous-menu Ou` rechercher */
   InitMenuWhereToSearch (NumFormSearchText);

   WithReplace = FALSE;
   ReplaceDone = FALSE;
   AutoReplace = FALSE;
   strcpy (pPrecedentString, "");

   /* efface le label "References dans le document X" */
   TtaNewLabel (NumLabelAttributeValue, NumFormSearchText, " ");
   SearchLoadResources ();
   /* complete la feuille de dialogue avec les menus de recherche de types */
   /*  d'elt et d'attributs si la ressource de recherche avec structure est chargee */
   if (ThotLocalActions[T_strsearchconstmenu] != NULL)
      (*ThotLocalActions[T_strsearchconstmenu]) (pDoc);

   /* active le formulaire */
   TtaShowDialogue (NumFormSearchText, TRUE);
   if (!ok)
     {
	InitSearchDomain (3, searchDomain);
	TtaSetMenuForm (NumMenuOrSearchText, 3);
     }
   searchDomain->SDocument = pDoc;
   TextOK = FALSE;
}


/*----------------------------------------------------------------------
   CallbackTextReplace traite les retours du formulaire Recherche-   
   Remplacement de texte.                                   
   ref: reference de l'element de dialogue a traiter        
   val: valeur de l'element de dialogue                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackTextReplace (int ref, int val, char *txt)
#else  /* __STDC__ */
void                CallbackTextReplace (ref, val, txt)
int                 ref;
int                 val;
char               *txt;

#endif /* __STDC__ */
{
   PtrElement          pFirstSel;
   PtrElement          pLastSel;
   PtrDocument         pDocSel;
   int                 firstChar;
   int                 lastChar;
   PtrElement          pCurrEl;
   boolean             selectionOK;
   boolean             found, stop;
   boolean             foundString;
   boolean             error;


   error = FALSE;
   if (searchDomain->SDocument != NULL)
      if (searchDomain->SDocument->DocSSchema != NULL)
	 switch (ref)
	       {
		  case NumZoneTextSearch:
		     /* Chaine a chercher */
		     strcpy (pSearchedString, txt);
		     if (strcmp (pSearchedString, pPrecedentString) != 0)
		       {
			  ReplaceDone = FALSE;
			  strcpy (pPrecedentString, pSearchedString);
		       }
		     SearchedStringLen = strlen (pSearchedString);
		     break;
		  case NumZoneTextReplace:
		     /* Chaine a remplacer */
		     strcpy (pReplaceString, txt);
		     ReplaceStringLen = strlen (pReplaceString);
		     /* bascule automatiquement le remplacement */
		     if (!WithReplace)
		       {
			  WithReplace = TRUE;
			  DoReplace = TRUE;
			  TtaSetMenuForm (NumMenuReplaceMode, 1);
		       }
		     break;
		  case NumToggleRegExp:
		     if (val == 0)
			/* toggle button expression reguliere */
			RegExp = !RegExp;
		     else
			/* toggle button MAJUSCULES = minuscules */
			CaseDistinction = !CaseDistinction;
		     break;
		  case NumMenuReplaceMode:
		     /* sous-menu mode de remplacement */
		     switch (val)
			   {
			      case 0:
				 /* Sans remplacement */
				 WithReplace = FALSE;
				 AutoReplace = FALSE;
				 DoReplace = FALSE;
				 break;
			      case 1:
				 /* Remplacement a la demande */
				 WithReplace = TRUE;
				 AutoReplace = FALSE;
				 DoReplace = TRUE;
				 break;
			      case 2:
				 /* Remplacement automatique */
				 WithReplace = TRUE;
				 AutoReplace = TRUE;
				 DoReplace = TRUE;
				 break;
			   }
		     break;
		  case NumFormSearchText:
		     /* Boutons de la feuille de dialogue */
		     if (val == 2 && WithReplace && !StartSearch)
			DoReplace = FALSE;
		     else if (val == 0)
			/* Abandon de la recherhce */
			return;

		     selectionOK = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel, &firstChar, &lastChar);
		     if (selectionOK)
			if (pDocSel != searchDomain->SDocument)
			   selectionOK = FALSE;
		     if (!selectionOK && StartSearch)
		       {
			  /* Pas de selection -> recherche dans tout le document */
			  pCurrEl = NULL;
		       }
		     else if (pDocSel != searchDomain->SDocument)
		       {
			  /* pas de selection dans le document d'ou vient la commande */
			  TtaDestroyDialogue (NumFormSearchText);
			  return;
		       }
		     else if (StartSearch)
		       {
			  pCurrEl = NULL;
		       }
		     else if (searchDomain->SStartToEnd)
		       {
			  pCurrEl = pLastSel;
		       }
		     else
		       {
			  pCurrEl = pFirstSel;
		       }

		     /* la recherche est demandee, on recupere les parametres */
		     if (ThotLocalActions[T_strsearchgetparams] != NULL)
			(*ThotLocalActions[T_strsearchgetparams]) (&error, searchDomain);
		     if (!error || SearchedStringLen != 0)
		       {
			  found = FALSE;
			  if (SearchedStringLen == 0)
			    {
			       if (ThotLocalActions[T_strsearchonly] != NULL)
				  (*ThotLocalActions[T_strsearchonly]) (pCurrEl, searchDomain, &found);
			    }
			  else
			     /* on cherche une chaine de caracteres */
			     /* eventuellement, avec remplacement */
			    {
			       pFirstSel = pCurrEl;
			       do
				 {
				    stop = TRUE;
				    /* on sortira de la boucle si on ne */
				    /* trouve pas le texte cherche' */
				    if (WithReplace && DoReplace && !StartSearch
					&& TextOK
				    && DocTextOK == searchDomain->SDocument
					&& ElemTextOK == pFirstSel
					&& FirstCharTextOK == firstChar
					&& LastCharTextOK == lastChar)
				       /* il faut faire un remplacement et on est sur le */
				       /* texte cherche' */
				       /* on ne remplace pas dans un sous-arbre en */
				       /* lecture seule */
				       if (ElementIsReadOnly (pFirstSel))
					  TtaNewLabel (NumLabelAttributeValue, NumFormSearchText,
						       TtaGetMessage (LIB, EL_RO));
				       else if (!pFirstSel->ElIsCopy && pFirstSel->ElText != NULL
						&& pFirstSel->ElTerminal
					&& pFirstSel->ElLeafType == LtText)
					  /* on ne remplace pas dans une copie */
					 {
					    found = TRUE;
					    if (ThotLocalActions[T_strsearcheletattr] != NULL)
					       (*ThotLocalActions[T_strsearcheletattr]) (pFirstSel, &found);
					    if (found)
					      {
						 /* effectue le remplacement du texte */
						 ReplaceString (searchDomain->SDocument,
								pFirstSel, firstChar, SearchedStringLen,
								pReplaceString, ReplaceStringLen,
						  !AutoReplace);
						 ReplaceDone = TRUE;
						 StartSearch = FALSE;
						 /* met eventuellement a jour la borne de */
						 /* fin du domaine de recherche */
						 if (pFirstSel == searchDomain->SEndElement)
						    /* la borne est dans l'element ou` on a */
						    /* fait le remplacement */
						    if (searchDomain->SEndChar != 0)
						       /* la borne n'est pas a la fin de */
						       /* l'element, on decale la borne */
						       searchDomain->SEndChar += ReplaceStringLen - SearchedStringLen;
						 /* recupere les parametres de la nouvelle */
						 /* chaine */
						 if (!AutoReplace)
						    selectionOK = GetCurrentSelection (&pDocSel, &pFirstSel, &pLastSel,
							 &firstChar, &lastChar);
					      }
					 }

				    do
				      {
					 /*Recherche de la prochaine occurence du texte cherche' */
					 if (pFirstSel == NULL)
					   {
					      /* debut de recherche */
					      if (searchDomain->SStartToEnd)
						{
						   pFirstSel = searchDomain->SStartElement;
						   firstChar = searchDomain->SStartChar;
						   if (pFirstSel == NULL)
						      pFirstSel = searchDomain->SDocument->DocRootElement;
						}
					      else
						{
						   pFirstSel = searchDomain->SEndElement;
						   firstChar = searchDomain->SEndChar;
						}
					   }
					 else if (searchDomain->SStartToEnd)
					   {
					      pFirstSel = pLastSel;
					      firstChar = lastChar + 1;
					   }

					 if (searchDomain->SStartToEnd)
					   {
					      pLastSel = searchDomain->SEndElement;
					      lastChar = searchDomain->SEndChar;
					   }
					 else
					   {
					      pLastSel = searchDomain->SStartElement;
					      lastChar = searchDomain->SStartChar;
					   }

					 if (RegExp)
					   {
					      if (!searchDomain->SStartToEnd)
						 if (firstChar > 0)
						    firstChar--;
					      found = SearchRegularExpression (&pFirstSel, &firstChar, &pLastSel,
								 &lastChar, searchDomain->SStartToEnd,
					       CaseDistinction, pSearchedString);
					   }
					 else
					   {
					      found = SearchText (searchDomain->SDocument, &pFirstSel,
						 &firstChar, &pLastSel, &lastChar,
						searchDomain->SStartToEnd,
								CaseDistinction, pSearchedString, SearchedStringLen);
					      if (found)
						 lastChar--;
					   }

					 foundString = found;
					 if (found)
					    /* on a trouve' la chaine cherchee */
					   {
					      stop = FALSE;
					      if (ThotLocalActions[T_strsearcheletattr] != NULL)
						 (*ThotLocalActions[T_strsearcheletattr]) (pFirstSel, &found);
					   }
				      }
				    while (foundString && !found);

				    if (found)
				      {
					 /* on a trouve la chaine recherchee dans le bon type */
					 /* d'element et avec le bon attribut */
					 if (!AutoReplace)
					   {
					      /* selectionne la chaine trouvee */
					      SelectStringWithEvent (searchDomain->SDocument,
						  pFirstSel, firstChar, lastChar);
					      /* arrete la boucle de recherche */
					      stop = TRUE;
					      lastChar++;
					   }
					 TextOK = TRUE;
					 DocTextOK = searchDomain->SDocument;
					 ElemTextOK = pFirstSel;
					 FirstCharTextOK = firstChar;
					 LastCharTextOK = lastChar;
				      }
				    else
				      {
					 TextOK = FALSE;
					 stop = TRUE;
					 if (searchDomain->SWholeDocument)
					    /* il faut rechercher dans tout le document */
					    /* cherche l'arbre a traiter apres celui ou` on */
					    /* n'a pas trouve' */
					    if (NextTree (&pFirstSel, &firstChar, searchDomain))
					      {
						 stop = FALSE;
						 pLastSel = pFirstSel;
						 lastChar = 0;
					      }
				      }
				    StartSearch = FALSE;
				 }
			       while (!stop);
			    }
			  if (found)
			    {
			       /* on a trouve' et selectionne'. */
			       StartSearch = FALSE;
			       if (ThotLocalActions[T_strsearchshowvalattr] != NULL)
				  (*ThotLocalActions[T_strsearchshowvalattr]) ();
			    }
			  else
			     /* on n'a pas trouve' */
			    {
			       if (WithReplace && ReplaceDone)
				  /* message "Plus de remplacement" */
				  TtaNewLabel (NumLabelAttributeValue,
					       NumFormSearchText,
					       TtaGetMessage (LIB, NOTHING_TO_REPLACE));
			       else
				  /* message "Pas trouve'" */
				  TtaNewLabel (NumLabelAttributeValue,
					       NumFormSearchText,
					TtaGetMessage (LIB, NOT_FOUND));
			       StartSearch = TRUE;
			    }
		       }
		     break;
		  default:
		     if (ThotLocalActions[T_strsearchretmenu] != NULL)
			(*ThotLocalActions[T_strsearchretmenu]) (ref, val, txt, searchDomain);
		     break;
	       }
}

/*----------------------------------------------------------------------
   BuildGoToPageMenu traite la commande Aller page numero              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildGoToPageMenu (PtrDocument pDoc, int docView, int schView, boolean assoc)
#else  /* __STDC__ */
void                BuildGoToPageMenu (pDoc, docView, schView, assoc)
PtrDocument         pDoc;
int                 docView;
int                 schView;
boolean             assoc;

#endif /* __STDC__ */
{
   char                buffTitle[200];

   if (ThotLocalActions[T_searchpage] == NULL)
      TteConnectAction (T_searchpage, (Proc) CallbackGoToPageMenu);

   /* garde le pointeur sur le document concerne' par la commande */
   SearchedPageDoc = pDoc;
   /* garde le  numero de vue (dans le document) de la vue concernee */
   if (assoc)
      ViewSearchedPageDoc = 1;
   else
      ViewSearchedPageDoc = docView;
   /* garde la racine de l'arbre ou on va chercher une page, ainsi que */
   /* le numero (dans le schema de presentation) de la vue concernee */
   if (assoc)
      /* c'est une vue d'elements associes */
     {
	/* les elements associes n'ont qu'une vue, la vue 1 */
	SearchedPageSchView = 1;
	SearchedPageRoot = pDoc->DocAssocRoot[docView - 1];
     }
   else
      /* c'est une vue de l'arbre principal */
     {
	/* cherche le numero de vue dans le schema de presentation */
	/* applique' au document */
	SearchedPageSchView = AppliedView (pDoc->DocRootElement, NULL, pDoc,
					    docView);
	SearchedPageRoot = pDoc->DocRootElement;
     }
   /* compose le titre "Recherche dans le document..." */
   strcpy (buffTitle, TtaGetMessage (LIB, SEARCH_IN));
   strcat (buffTitle, pDoc->DocDName);
   /* cree formulaire de saisie du numero de la page cherchee */
   TtaNewSheet (NumFormSearchPage, 0, 0, 0,
      buffTitle, 1, TtaGetMessage (LIB, LIB_CONFIRM), TRUE, 1, 'L', D_DONE);

   /* cree zone de saisie du numero de la page cherchee */
   TtaNewNumberForm (NumZoneSearchPage, NumFormSearchPage,
		     TtaGetMessage (LIB, GOTO_PAGE), 0, 9999, FALSE);
   /* affiche le formulaire */
   TtaShowDialogue (NumFormSearchPage, FALSE);
}

#ifdef IV
/*----------------------------------------------------------------------
   MoveToPage effectue un deplacement sur les pages.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MoveToPage (int frame, int func)
#else  /* __STDC__ */
void                MoveToPage (frame, func)
int                 frame;
int                 func;

#endif /* __STDC__ */
{
   PtrElement          PP;
   PtrElement          pFirstVisible;
   PtrDocument         pDocSel;
   int                 docView;
   int                 schView;
   boolean             assoc;

   PP = NULL;
   if (frame > 0)
     {
	GetDocAndView (frame, &pDocSel, &docView, &assoc);
	if (assoc)
	   /* c'est une vue d'elements associes */
	   /* les elements associes n'ont qu'une vue */
	   schView = 1;
	else
	   /* c'est une vue de l'arbre principal */
	   /* cherche le numero de vue dans le schema */
	   /* de presentation applique' au document */
	   schView = AppliedView (pDocSel->DocRootElement,
				   NULL, pDocSel, docView);
	pFirstVisible = FirstVisible (pDocSel, docView, assoc);
	if (func >= 1 && func <= 5 && pFirstVisible != NULL)
	  {
	     switch (func)
		   {
		      case 1:
			 /* Premiere page ou debut du document */
			 PP = SearchPageBreak (pFirstVisible, schView, -999999, TRUE);
			 break;
		      case 2:
			 /* Derniere page ou fin du document */
			 PP = SearchPageBreak (pFirstVisible, schView, 999999, TRUE);
			 PP = SearchPageBreak (PP, schView, -1, TRUE);
			 break;
		      case 3:
			 /* Page GetNextBox */
			 PP = SearchPageBreak (pFirstVisible, schView, 1, TRUE);
			 break;
		      case 4:
			 /* Page GetPreviousBox */
			 PP = SearchPageBreak (pFirstVisible, schView, -1, TRUE);
			 break;
		      case 5 /* Page Numero N */ :
			 BuildGoToPageMenu (pDocSel, docView, schView, assoc);
			 break;

		   }
	     if (func != 5)
	       {
		  /* docView represente maintenant le numero de vue dans le */
		  /* document, meme pour les elements associes */
		  if (assoc)
		     docView = 1;
		  ScrollPageToTop (PP, docView, pDocSel);
	       }
	  }
     }
}
#endif

/*----------------------------------------------------------------------
   CallbackGoToPageMenu traite les retours du formulaire de saisie du  
   numero de la page ou aller.                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackGoToPageMenu (int ref, int val)
#else  /* __STDC__ */
void                CallbackGoToPageMenu (ref, val)
int                 ref;
int                 val;

#endif /* __STDC__ */
{
   PtrElement          pPage;

   if (SearchedPageDoc != NULL)
      if (SearchedPageDoc->DocSSchema != NULL)
	 /* le document concerne' est toujours la */
	 switch (ref)
	       {
		  case NumZoneSearchPage:
		     /* zone de saisie du numero de la page cherchee */
		     SearchedPageNumber = val;
		     break;
		  case NumFormSearchPage:
		     /* formulaire de saisie du numero de la page cherchee */
		     /* cherche la page */
		     pPage = SearchPageBreak (SearchedPageRoot, SearchedPageSchView,
					  SearchedPageNumber, FALSE);
		     /* fait afficher la page trouvee en haut de sa frame */
		     ScrollPageToTop (pPage, ViewSearchedPageDoc, SearchedPageDoc);
		     TtaDestroyDialogue (NumFormSearchPage);
		     break;
	       }
}

/* End Of Module cherche */

/*----------------------------------------------------------------------
   Initialise les variables des commandes de recherche.            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                SearchLoadResources (void)
#else
void                SearchLoadResources ()
#endif				/* __STDC__ */
{
   if (ThotLocalActions[T_searchtext] == NULL)
     {
	/* Connecte les actions liees au traitement du search */
	TteConnectAction (T_searchtext, (Proc) CallbackTextReplace);
	TteConnectAction (T_locatesearch, (Proc) CallbackWhereToSearch);
	CurrRef = NULL;
	CurrRefDoc = NULL;
	CurrRefElem = NULL;
	pExtCurrDoc = NULL;
	GetSearchContext (&searchDomain);
     }
   pSearchedString[0] = '\0';
   SearchedStringLen = 0;
   RegExp = FALSE;
   CaseDistinction = FALSE;
   WithReplace = FALSE;
   pReplaceString[0] = '\0';
   ReplaceStringLen = 0;
}



