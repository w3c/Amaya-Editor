
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   cherche.c : gestion de la commande de recherche.
   Major Changes:
   V. Quint     Mai 1992
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

static boolean      MajEgalMin;	/* dans la recherche de texte, on */

					/* confond majuscules et minuscules */
static boolean      AvecRemplace;	/* il faut remplacer les chaines trouvees */

/* pointeur sur le domaine de recherche concernant la commande en cours */
static PtrSearchContext    DomaineCherche = NULL;

/* entree desactivee dans le sous-menu "Ou chercher" */
static PtrDocument  DocPageCherchee;	/* le document ou on cherche une page */
static PtrElement   RacinePageCherchee;		/* racine de l'arbre ou on cherche */

					/* une page */
static int          VueDocPageCherchee;		/* vue (du document) pour laquelle on */

					/* cherche une page */
static int          VueSchPageCherchee;		/* vue (du schema de presentation) */

					/* pour laquelle on cherche une page */
static int          NumPageCherchee;	/* numero de la page chercheee */


static char         pChainePreced[MAX_CHAR];	/* la chaine cherchee precedente */
static char         pChaineCherchee[MAX_CHAR];	/* la chaine cherchee */
static int          LgChaineCh;	/* longueur de cette chaine */
static boolean      ExpressionReguliere;	/* pChaineCherchee est une 

						   expression reguliere */
static char         pChaineRemplace[MAX_CHAR];	/* la chaine de remplacement */
static int          LgChaineRempl;	/* longueur de cette chaine */

					  /* si attribut  textuel */
static int          CmdRefPrec;	/* derniere commande reference */
static PtrReference ReferCour;	/* pointeur sur la reference courante */
static PtrDocument  DocDeReferCour;	/* document auquel appartient la ref cour */

static PtrElement   ElemRefCour;	/* element dont on cherche les references */
static PtrDocument  DocDeElemRefCour;	/* document auquel appartient */

					  /* l'element dont on cherche les references */
static PtrExternalDoc pDocExtCour;	/* pointeur sur le descripteur de document */

					  /* externe contenant la reference courante */
static boolean      FinRechercheReferences;

static boolean      RemplacementAutomatique;
boolean             DebutRecherche;
static boolean      RemplacementFait;
static boolean      FaireRemplacement;

static boolean      TextOK = FALSE;
static PtrElement   ElemTextOK;
static int          FirstCharTextOK;
static int          LastCharTextOK;
static PtrDocument  DocTextOK;

#ifdef __STDC__
void                SearchLoadResources (void);

#else  /* __STDC__ */
void                SearchLoadResources ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |  Initialise le sous-menu "Ou rechercher"                           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         InitMenuOuRechercher (int ref)
#else  /* __STDC__ */
static void         InitMenuOuRechercher (ref)
int                 ref;

#endif /* __STDC__ */
{
   int                 i;
   char                chaine[200];

   i = 0;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, BEFORE_SEL));
   i += strlen (&chaine[i]) + 1;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, WITHIN_SEL));
   i += strlen (&chaine[i]) + 1;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, AFTER_SEL));
   i += strlen (&chaine[i]) + 1;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, IN_WHOLE_DOC));
   /* sous-menu Ou` rechercher element vide */
   TtaNewSubmenu (NumMenuOrSearchText, ref, 0,
	     TtaGetMessage (LIB, SEARCH_WHERE), 4, chaine, NULL, FALSE);
   TtaSetMenuForm (NumMenuOrSearchText, 2);
}

/* ---------------------------------------------------------------------- */
/* |  Active le sous-menu "Ou rechercher"                               | */
/* ---------------------------------------------------------------------- */
static void         ActiveMenuOuRechercher ()
{
   TtaRedrawMenuEntry (NumMenuOrSearchText, 0, NULL, -1, 1);
   TtaRedrawMenuEntry (NumMenuOrSearchText, 1, NULL, -1, 1);
   TtaRedrawMenuEntry (NumMenuOrSearchText, 2, NULL, -1, 1);
}


/* ---------------------------------------------------------------------- */
/* |  RetMenuOuRechercher traite le retour du sous-menu "Ou rechercher" | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuOuRechercher (int ref, int val)
#else  /* __STDC__ */
void                RetMenuOuRechercher (ref, val)
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
	       DomaineCherche->SStartToEnd = FALSE;
	       break;
	    case 1:
	       /* Dans la selection */
	       DebutRecherche = TRUE;
	       break;
	    case 2:
	       /* Apres la selection */
	       DomaineCherche->SStartToEnd = TRUE;
	       break;
	    case 3:
	       /* Dans tout le document */
	       DebutRecherche = TRUE;
	       break;
	 }
   if (DebutRecherche)
     {
	InitSearchDomain (val, DomaineCherche);
	if (!RemplacementAutomatique)
	   /* On prepare la recherche suivante */
	   if (DomaineCherche->SStartToEnd)
	      TtaSetMenuForm (ref, 2);	/* Apres selection */
	   else
	      TtaSetMenuForm (ref, 0);	/* ElemIsBefore selection */
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChElemVide cherche dans le domaine decrit par context et a      | */
/* |            partir de (et a l'interieur de) l'element elCour,       | */
/* |            le premier element vide.                                | */
/* |            Retourne vrai si trouve', et dans ce cas selectionne    | */
/* |            l'element trouve'.                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      ChElemVide (PtrElement elCour, PtrSearchContext context)
#else  /* __STDC__ */
static boolean      ChElemVide (elCour, context)
PtrElement          elCour;
PtrSearchContext           context;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrElement          pAscendant;
   PtrElement          ElSuiv;
   int                 i;
   boolean             trouve;
   boolean             ok;

   trouve = FALSE;
   ok = FALSE;
   if (elCour == NULL)
      /* debut de recherche */
      if (context->SStartToEnd)
	{
	   pEl = context->SStartElement;
	   if (pEl == NULL)
	      pEl = context->SDocument->DocRootElement;
	}
      else
	 pEl = context->SEndElement;
   else
      pEl = elCour;
   while (pEl != NULL && !trouve)
     {
	if (context->SStartToEnd)
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
	     pAscendant = pEl;
	     while (pAscendant->ElParent != NULL && pAscendant->ElSource == NULL)
		pAscendant = pAscendant->ElParent;
	     if (pAscendant->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		   /* l'element n'est pas cache' a l'utilisateur */
		   trouve = TRUE;
	  }
     }

   if (pEl != NULL && trouve)
      /* on a trouve' */
     {
	/* l'element trouve' est pointe' par pEl */
	if (context->SStartToEnd)
	  {
	     if (context->SEndElement != NULL)
		/* il faut s'arreter avant l'extremite' du document */
		if (pEl != context->SEndElement)
		   /*l'element trouve' n'est pas l'element ou il faut s'arreter */
		   if (ElemIsBefore (context->SEndElement, pEl))
		      /* l'element trouve' est apres l'element de fin, on */
		      /* fait comme si on n'avait pas trouve' */
		      pEl = NULL;
	  }
	else if (context->SStartElement != NULL)
	   /* il faut s'arreter avant l'extremite' du document */
	   if (pEl != context->SStartElement)
	      /*l'element trouve' n'est pas l'element ou il faut s'arreter */
	      if (ElemIsBefore (pEl, context->SStartElement))
		 /* l'element trouve' est avant le debut du domaine, on */
		 /* fait comme si on n'avait pas trouve' */
		 pEl = NULL;
	if (pEl != NULL)
	   SelectElementWithEvent (context->SDocument, pEl, TRUE, FALSE);
     }
   ok = (pEl != NULL);
   if (!trouve)
      if (context->SWholeDocument)
	 /* il faut rechercher dans tout le document */
	 /* cherche l'arbre a traiter apres celui ou` on n'a pas trouve' */
	 if (NextTree (&ElSuiv, &i, context))
	    /* il y a un autre arbre a traiter, on continue avec le */
	    /* debut de ce nouvel arbre */
	    ok = ChElemVide (ElSuiv, context);
   return ok;
}


/* ---------------------------------------------------------------------- */
/* |    ChReferVide cherche dans le domaine decrit par context et a     | */
/* |            partir de (et a l'interieur de) l'element elCour,       | */
/* |            la premiere reference vide.                             | */
/* |            Retourne vrai si trouve', et dans ce cas selectionne    | */
/* |            l'element trouve'.                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      ChReferVide (PtrElement elCour, PtrSearchContext context)
#else  /* __STDC__ */
static boolean      ChReferVide (elCour, context)
PtrElement          elCour;
PtrSearchContext           context;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrElement          pAscendant;
   PtrElement          ElSuiv;
   int                 i;
   boolean             trouve;
   boolean             ok;

   trouve = FALSE;
   ok = FALSE;
   if (elCour == NULL)
      /* debut de recherche */
      if (context->SStartToEnd)
	{
	   pEl = context->SStartElement;
	   if (pEl == NULL)
	      pEl = context->SDocument->DocRootElement;
	}
      else
	 pEl = context->SEndElement;
   else
      pEl = elCour;
   while (!trouve && pEl != NULL)
     {
	if (context->SStartToEnd)
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
	     pAscendant = pEl;
	     while (pAscendant->ElParent != NULL && pAscendant->ElSource == NULL)
		pAscendant = pAscendant->ElParent;
	     if (pAscendant->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		   /* l'element n'est pas cache' a l'utilisateur */
		   if (pEl->ElReference == NULL)
		      /* la reference est vide */
		      trouve = TRUE;
		   else
		      /* la reference n'est pas vide */
		   if (pEl->ElReference->RdReferred == NULL)
		      /* la reference est vide */
		      trouve = TRUE;
		   else if (pEl->ElReference->RdInternalRef)
		      if (!pEl->ElReference->RdReferred->ReExternalRef)
			 if (IsASavedElement (pEl->ElReference->RdReferred->ReReferredElem))
			    /* l'element reference' est dans le buffer des */
			    /* elements coupe's */
			    trouve = TRUE;
	  }
     }
   if (pEl != NULL && trouve)
      /* on a trouve' */
     {
	/* l'element trouve' est pointe' par pEl */
	if (context->SStartToEnd)
	  {
	     if (context->SEndElement != NULL)
		/* il faut s'arreter avant l'extremite' du document */
		if (pEl != context->SEndElement)
		   /*l'element trouve' n'est pas l'element ou il faut s'arreter */
		   if (ElemIsBefore (context->SEndElement, pEl))
		      /* l'element trouve' est apres l'element de fin, on */
		      /* fait comme si on n'avait pas trouve' */
		      pEl = NULL;
	  }
	else if (context->SStartElement != NULL)
	   /* il faut s'arreter avant l'extremite' du document */
	   if (pEl != context->SStartElement)
	      /*l'element trouve' n'est pas l'element ou il faut s'arreter */
	      if (ElemIsBefore (pEl, context->SStartElement))
		 /* l'element trouve' est avant le debut du domaine, on */
		 /* fait comme si on n'avait pas trouve' */
		 pEl = NULL;
	if (pEl != NULL)
	   SelectElementWithEvent (context->SDocument, pEl, TRUE, FALSE);
     }
   ok = (pEl != NULL);
   if (!trouve)
      if (context->SWholeDocument)
	 /* il faut rechercher dans tout le document */
	 /* cherche l'arbre a traiter apres celui ou` on n'a pas trouve' */
	 if (NextTree (&ElSuiv, &i, context))
	    /* il y a un autre arbre a traiter, on continue avec le */
	    /* debut de ce nouvel arbre */
	    ok = ChReferVide (ElSuiv, context);
   return ok;
}


/* ----------------------------------------------------------------------- */
/* | RetMenuChercheElemVide traite les retours du formulaire Recherche   | */
/* |            elements vides.                                          | */
/* |            ref: reference de l'element de dialogue a traiter        | */
/* |            val: valeur de l'element de dialogue                     | */
/* ----------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuChercheElemVide (int ref, int val)
#else  /* __STDC__ */
void                RetMenuChercheElemVide (ref, val)
int                 ref;
int                 val;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrElement          elCour;
   PtrDocument         docsel;
   PtrElement          premsel;
   PtrElement          dersel;
   int                 premcar;
   int                 dercar;

   if (DomaineCherche->SDocument != NULL)
      if (DomaineCherche->SDocument->DocSSchema != NULL)
	 /* le document concerne' est toujours la */
	 /* feuille de dialogue Rechercher element vide */
	 if (val == 1)
	    /* lancer la recherche */
	   {
	      if (DebutRecherche)
		{
		   elCour = NULL;
		   DebutRecherche = FALSE;
		}
	      else
		{
		   ok = GetCurrentSelection (&docsel, &premsel, &dersel, &premcar,
				    &dercar);
		   if (!ok || docsel != DomaineCherche->SDocument)
		      /* la selection a change' de document, on refuse */
		     {
			TtaNewLabel (NumLabelEmptyElemNotFound,
				     NumFormSearchEmptyElement,
			   TtaGetMessage (LIB, DO_NOT_CHANGE_DOC));
			DebutRecherche = TRUE;
			return;
		     }
		   else if (DomaineCherche->SStartToEnd)
		      elCour = dersel;
		   else
		      elCour = premsel;
		}
	      ok = ChElemVide (elCour, DomaineCherche);
	      if (ok)
		{
		   /* on a trouve' et selectionne' */
		   DebutRecherche = FALSE;
		   /* on reactive les entrees du sous-menu "Ou chercher" */
		   ActiveMenuOuRechercher ();
		   /* efface le message "Pas trouve'" dans le formulaire */
		   TtaNewLabel (NumLabelEmptyElemNotFound, NumFormSearchEmptyElement, " ");
		}
	      else
		 /* on n'a pas trouve' */
		{
		   /* message 'Pas trouve' dans le formulaire */
		   TtaNewLabel (NumLabelEmptyElemNotFound, NumFormSearchEmptyElement,
				TtaGetMessage (LIB, NOT_FOUND));
		   DebutRecherche = TRUE;
		}
	   }
}


/* ---------------------------------------------------------------------- */
/* |    ChercherElementVide lance la commande de recherche des elements | */
/* |            vides pour le document pDoc.                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcSearchEmptyElement (Document document, View view)
#else  /* __STDC__ */
void                TtcSearchEmptyElement (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrDocument         docsel;
   PtrElement          premsel;
   PtrElement          dersel;
   int                 premcar;
   int                 dercar;
   char                buffTitre[200];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   SearchLoadResources ();

   ok = GetCurrentSelection (&docsel, &premsel, &dersel, &premcar, &dercar);
   if (!ok)
     {
	docsel = pDoc;
	premsel = pDoc->DocRootElement;
     }
   if (docsel != pDoc)
      /* pas de selection dans le document d'ou vient la commande */
      return;

   /* fait disparaitre les autres formulaires de recherche qui sont affiches */
   TtaDestroyDialogue (NumFormSearchEmptyReference);
   TtaDestroyDialogue (NumFormSearchText);

   DebutRecherche = TRUE;
   /* feuille de dialogue Rechercher element vide */
   /* compose le titre "Recherche dans le document..." */
   strcpy (buffTitre, TtaGetMessage (LIB, SEARCH_IN));
   strcat (buffTitre, pDoc->DocDName);
   TtaNewSheet (NumFormSearchEmptyElement, TtaGetViewFrame (document, view), 0, 0,
		buffTitre,
		1, TtaGetMessage (LIB, LIB_CONFIRM), FALSE, 3, 'L', D_DONE);
   /* label indiquant la recherche d'elements vides */
   TtaNewLabel (NumLabelDocSearcheEmptyElement, NumFormSearchEmptyElement,
		TtaGetMessage (LIB, SEARCH_EMPTY_EL));
   InitMenuOuRechercher (NumFormSearchEmptyElement);

   /* le message "Pas trouve'" */
   TtaNewLabel (NumLabelEmptyElemNotFound, NumFormSearchEmptyElement, " ");
   /* active le formulaire */
   TtaShowDialogue (NumFormSearchEmptyElement, TRUE);
   if (!ok)
     {
	InitSearchDomain (3, DomaineCherche);
	TtaSetMenuForm (NumMenuOrSearchText, 3);
     }
   DomaineCherche->SDocument = pDoc;
}


/* ----------------------------------------------------------------------- */
/* | RetMenuChercheReferVide traite les retours du formulaire Recherche  | */
/* |            references vides.                                        | */
/* |            ref: reference de l'element de dialogue a traiter        | */
/* |            val: valeur de l'element de dialogue                     | */
/* ----------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuChercheReferVide (int ref, int val)
#else  /* __STDC__ */
void                RetMenuChercheReferVide (ref, val)
int                 ref;
int                 val;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrElement          elCour;
   PtrElement          premsel;
   PtrElement          dersel;
   PtrDocument         docsel;
   int                 premcar;
   int                 dercar;

   if (DomaineCherche->SDocument != NULL)
      if (DomaineCherche->SDocument->DocSSchema != NULL)
	{
	   /* feuille de dialogue Rechercher reference vide */
	   if (val == 1)
	     {
		/* lancer la recherche */
		if (DebutRecherche)
		  {
		     elCour = NULL;
		     DebutRecherche = FALSE;
		  }
		else
		  {
		     ok = GetCurrentSelection (&docsel, &premsel, &dersel, &premcar, &dercar);
		     if (!ok || docsel != DomaineCherche->SDocument)
			/* la selection a change' de document, on refuse */
		       {
			  TtaNewLabel (NumLabelEmptyRefereneceNotFound,
				       NumFormSearchEmptyReference,
			   TtaGetMessage (LIB, DO_NOT_CHANGE_DOC));
			  DebutRecherche = TRUE;
			  return;
		       }
		     else if (DomaineCherche->SStartToEnd)
			elCour = dersel;
		     else
			elCour = premsel;
		  }
		ok = ChReferVide (elCour, DomaineCherche);
		if (ok)
		  {
		     DebutRecherche = FALSE;
		     /* on a trouve' et selectionne'. On reactive les */
		     /* entrees du sous-menu "Ou chercher" */
		     ActiveMenuOuRechercher ();
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
		     DebutRecherche = TRUE;
		  }
	     }
	   else
	      TtaDestroyDialogue (NumFormSearchEmptyReference);
	}
}


/* ---------------------------------------------------------------------- */
/* |    ChercherReferenceVide lance la commande de recherche des        | */
/* |            references vides pour le document pDoc.                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcSearchEmptyReference (Document document, View view)
#else  /* __STDC__ */
void                TtcSearchEmptyReference (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrDocument         docsel;
   PtrElement          premsel;
   PtrElement          dersel;
   int                 premcar;
   int                 dercar;
   char                buffTitre[200];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   SearchLoadResources ();

   ok = GetCurrentSelection (&docsel, &premsel, &dersel, &premcar, &dercar);
   if (!ok)
     {
	docsel = pDoc;
	premsel = pDoc->DocRootElement;
     }

   if (docsel != pDoc)
      /* pas de selection dans le document d'ou vient la commande */
      return;

   /* fait disparaitre les autres formulaires de recherche qui sont affiches */
   TtaDestroyDialogue (NumFormSearchEmptyElement);
   TtaDestroyDialogue (NumFormSearchText);

   DebutRecherche = TRUE;
   /* compose le titre "Recherche dans le document..." */
   strcpy (buffTitre, TtaGetMessage (LIB, SEARCH_IN));
   strcat (buffTitre, pDoc->DocDName);
   /* feuille de dialogue Rechercher reference vide */
   TtaNewSheet (NumFormSearchEmptyReference, TtaGetViewFrame (document, view), 0, 0,
		buffTitre,
		1, TtaGetMessage (LIB, LIB_CONFIRM), FALSE, 3, 'L', D_DONE);

   /* label indiquant la recherche de references vides */
   TtaNewLabel (NumLabelDocSearchEmptyRef, NumFormSearchEmptyReference,
		TtaGetMessage (LIB, SEARCH_EMPTY_REF));
   InitMenuOuRechercher (NumFormSearchEmptyReference);

   /* le message "Pas trouve'" */
   TtaNewLabel (NumLabelEmptyRefereneceNotFound, NumFormSearchEmptyReference, " ");
   /* active le formulaire */
   TtaShowDialogue (NumFormSearchEmptyReference, TRUE);
   if (!ok)
     {
	InitSearchDomain (3, DomaineCherche);
	TtaSetMenuForm (NumMenuOrSearchText, 3);
     }
   DomaineCherche->SDocument = pDoc;
}


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
static int          ValRetourMenuChoixRefer;

#ifdef __STDC__
void                RetMenuChoixRefer (int val)
#else  /* __STDC__ */
void                RetMenuChoixRefer (val)
int                 val;

#endif /* __STDC__ */
{
   ValRetourMenuChoixRefer = val;
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                CreeEtActiveMenuReferences (char *bufMenu, int nbEntrees, int *entreeChoisie)
#else  /* __STDC__ */
void                CreeEtActiveMenuReferences (bufMenu, nbEntrees, entreeChoisie)
char               *bufMenu;
int                 nbEntrees;
int                *entreeChoisie;

#endif /* __STDC__ */
{
   char                bufMenuB[MAX_TXT_LEN];
   char               *src;
   char               *dest;
   int                 k, l;

   ValRetourMenuChoixRefer = 0;
   /* ajoute 'B' au debut de chaque entree */
   dest = &bufMenuB[0];
   src = &bufMenu[0];
   for (k = 1; k <= nbEntrees; k++)
     {
	strcpy (dest, "B");
	dest++;
	l = strlen (src);
	strcpy (dest, src);
	dest += l + 1;
	src += l + 1;
     }
   TtaNewPopup (NumMenuReferenceChoice, 0, TtaGetMessage (LIB, LINK), nbEntrees,
		bufMenuB, NULL, 'L');
   TtaSetDialoguePosition ();
   TtaShowDialogue (NumMenuReferenceChoice, FALSE);
   /* attend que l'utilisateur ait repondu au menu et que le */
   /* mediateur ait appele' RetMenuChoixRefer */
   TtaWaitShowDialogue ();
   *entreeChoisie = ValRetourMenuChoixRefer;
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ChercheUneReference (boolean DocExtSuiv)
#else  /* __STDC__ */
static void         ChercheUneReference (DocExtSuiv)
boolean             DocExtSuiv;

#endif /* __STDC__ */
{
   char                msg[100];

   /* chercher une reference */
   FindReference (&ReferCour, &DocDeReferCour, &ElemRefCour,
	     &DocDeElemRefCour, &pDocExtCour, DocExtSuiv);
   if (ReferCour != NULL)
      /* on a trouve', on efface le message qui traine */
      strcpy (msg, " ");
   else if (pDocExtCour != NULL)
      /* references dans un document non charge' */
     {
	strcpy (msg, TtaGetMessage (LIB, REF_IN_DOC));
	GetDocName (pDocExtCour->EdDocIdent, msg + strlen (msg));
     }
   else
     {
	/* on n'a trouve' aucune reference */
	FinRechercheReferences = TRUE;
	strcpy (msg, TtaGetMessage (LIB, NOT_FOUND));
     }
   TtaNewLabel (NumLabelReferenceNotFound, NumFormSearchReference, msg);
}


/* ---------------------------------------------------------------------- */
/* |    ChercherLesReferences lance la commande de recherche des        | */
/* |            references qui designent l'element selectionne' du      | */
/* |            document pDoc.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChercherLesReferences (PtrDocument pDoc)
#else  /* __STDC__ */
void                ChercherLesReferences (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrElement          premsel;
   PtrElement          dersel;
   PtrDocument         docsel;
   int                 premcar;
   int                 dercar;
   char                buftext[200];
   boolean             trouve;

   if (!GetCurrentSelection (&docsel, &premsel, &dersel, &premcar, &dercar))
      /* message 'Pas de selection' */
      TtaDisplaySimpleMessage (INFO, LIB, DEBUG_NO_SEL);
   else
     {
	/* initialise les variables de recherche des references */
	ReferCour = NULL;
	ElemRefCour = NULL;
	pDocExtCour = NULL;
	/* cherche le premier ascendant reference' de la selection */
	pEl = premsel;
	trouve = FALSE;
	do
	  {
	     if (pEl->ElReferredDescr != NULL)
		/* l'element a un descripteur d'element reference' */
		if (pEl->ElReferredDescr->ReFirstReference != NULL ||
		    pEl->ElReferredDescr->ReExtDocRef != NULL)
		   /* l'element est effectivement reference' */
		   trouve = TRUE;
	     if (!trouve)
		/* l'element n'est pas reference', on passe au pere */
		pEl = pEl->ElParent;
	  }
	while (!trouve && pEl != NULL);
	if (trouve)
	   /* il y a effectivement un ascendant reference' */
	  {
	     FinRechercheReferences = FALSE;
	     /* initialise le label du formulaire en y mettant le type de */
	     /* l'element reference' */
	     strcpy (buftext, TtaGetMessage (LIB, EARCH_REF_TO_EL));
	     strcat (buftext, " ");
	     strcat (buftext, pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrName);
	     TtaNewLabel (NumLabelSearchReference, NumFormSearchReference,
			  buftext);
	     TtaNewLabel (NumLabelReferenceNotFound, NumFormSearchReference,
			  " ");
	     /* active le formulaire de recherche */
	     TtaShowDialogue (NumFormSearchReference, FALSE);
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
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
	ChercherLesReferences (pDoc);
     }
}


/* ---------------------------------------------------------------------- */
/* |    RetMenuChercheRefA      traite les retours de la feuille de     | */
/* |            dialogue "Recherche les references a`".                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuChercheRefA (int Ref, int Data)
#else  /* __STDC__ */
void                RetMenuChercheRefA (Ref, Data)
int                 Ref;
int                 Data;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;

   if (Ref == NumFormSearchReference && !FinRechercheReferences)
      switch (Data)
	    {
	       case 1:
		  if (DocDeReferCour != NULL)
		     if (DocDeReferCour->DocSSchema == NULL)
			/* le document ou se trouvait la reference courante a */
			/* ete ferme'. On repart du debut */
		       {
			  ReferCour = NULL;
			  ElemRefCour = NULL;
			  pDocExtCour = NULL;
		       }
		  ChercheUneReference (TRUE);
		  break;

	       case 2:
		  /* charger le document contenant la reference cherchee */
		  if (ReferCour == NULL && pDocExtCour != NULL)
		     /* il s'agit bien d'une reference dans un document externe */
		     /* non charge' */
		     if (!DocIdentIsNull (pDocExtCour->EdDocIdent))
		       {
			  /* acquiert et initialise un descripteur de document */
			  CreateDocument (&pDoc);
			  if (pDoc != NULL)
			    {
			       /* charge le document */
			       LoadDocument (&pDoc, pDocExtCour->EdDocIdent);
			       if (pDoc != NULL)
				 {
				    /* efface le label "References dans le document X" */
				    TtaNewLabel (NumLabelReferenceNotFound,
					      NumFormSearchReference, " ");
				    ChercheUneReference (FALSE);
				 }
			    }
		       }
		  break;
	       default:
		  TtaDestroyDialogue (Ref);
		  break;
	    }
}


/* ---------------------------------------------------------------------- */
/* |    ChercherRemplacerTexte lance la commande de recherche et/ou     | */
/* |            remplacement de texte, type d'element et attribut       | */
/* |            pour le document pDoc.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcSearchText (Document document, View view)
#else  /* __STDC__ */
void                TtcSearchText (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   boolean             ok;
   PtrDocument         docsel;
   PtrElement          premsel;
   PtrElement          dersel;
   int                 premcar;
   int                 dercar, i;
   char                bufTitre[200], chaine[200];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];

   ok = GetCurrentSelection (&docsel, &premsel, &dersel, &premcar, &dercar);
   if (ok)
      if (pDoc != docsel)
	 ok = FALSE;
   if (!ok)
     {
	docsel = pDoc;
	premsel = dersel = pDoc->DocRootElement;
	premcar = dercar = 0;
     }

   /* fait disparaitre les autres formulaires de recherche qui sont affiches */
   TtaDestroyDialogue (NumFormSearchEmptyElement);
   TtaDestroyDialogue (NumFormSearchEmptyReference);
   DebutRecherche = TRUE;

   /* compose le titre du formulaire "Recherche dans le document..." */
   strcpy (bufTitre, TtaGetMessage (LIB, SEARCH_IN));
   strcat (bufTitre, pDoc->DocDName);
   /* feuille de dialogue Rechercher texte et structure */
   strcpy (chaine, TtaGetMessage (LIB, LIB_CONFIRM));
   i = strlen (TtaGetMessage (LIB, LIB_CONFIRM)) + 1;
   strcpy (chaine + i, TtaGetMessage (LIB, DO_NOT_REPLACE));
   TtaNewSheet (NumFormSearchText, TtaGetViewFrame (document, view), 0, 0,
		bufTitre, 2, chaine, FALSE, 6, 'L', D_DONE);

   /* zone de saisie du texte a` rechercher */
   TtaNewTextForm (NumZoneTextSearch, NumFormSearchText,
		   TtaGetMessage (LIB, SEARCH_FOR), 30, 1, FALSE);
   TtaSetTextForm (NumZoneTextSearch, pChaineCherchee);

   /* Toggle button "Expression reguliere" */
   i = 0;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, REG_EXP));
   i += strlen (&chaine[i]) + 1;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UPPERCASE_EQ_LOWERCASE));
   TtaNewToggleMenu (NumToggleRegExp, NumFormSearchText,
		     NULL, 2, chaine, NULL, FALSE);
   TtaSetToggleMenu (NumToggleRegExp, 0, ExpressionReguliere);
   TtaSetToggleMenu (NumToggleRegExp, 1, MajEgalMin);

   /* zone de saisie du texte de remplacement */
   TtaNewTextForm (NumZoneTextReplace, NumFormSearchText,
		   TtaGetMessage (LIB, REPLACE_BY), 30, 1, TRUE);
   TtaSetTextForm (NumZoneTextReplace, pChaineRemplace);

   /* sous-menu mode de remplacement */
   if (!pDoc->DocReadOnly)
     {
	/* on autorise les remplacement */
	/* attache le sous-menu remplacement */
	i = 0;
	sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, NO_REPLACE));
	i += strlen (&chaine[i]) + 1;
	sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, REPLACE_ON_REQU));
	i += strlen (&chaine[i]) + 1;
	sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, AUTO_REPLACE));
	TtaNewSubmenu (NumMenuReplaceMode, NumFormSearchText, 0,
		  TtaGetMessage (LIB, REPLACE), 3, chaine, NULL, FALSE);
	if (AvecRemplace)
	  {
	     if (RemplacementAutomatique)
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
   InitMenuOuRechercher (NumFormSearchText);

   AvecRemplace = FALSE;
   RemplacementFait = FALSE;
   RemplacementAutomatique = FALSE;
   strcpy (pChainePreced, "");

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
	InitSearchDomain (3, DomaineCherche);
	TtaSetMenuForm (NumMenuOrSearchText, 3);
     }
   DomaineCherche->SDocument = pDoc;
   TextOK = FALSE;
}


/* ----------------------------------------------------------------------- */
/* | RetMenuRemplacerTexte traite les retours du formulaire Recherche-   | */
/* |            Remplacement de texte.                                   | */
/* |            ref: reference de l'element de dialogue a traiter        | */
/* |            val: valeur de l'element de dialogue                     | */
/* ----------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuRemplacerTexte (int ref, int val, char *txt)
#else  /* __STDC__ */
void                RetMenuRemplacerTexte (ref, val, txt)
int                 ref;
int                 val;
char               *txt;

#endif /* __STDC__ */
{
   PtrElement          premsel;
   PtrElement          dersel;
   PtrDocument         docsel;
   int                 premcar;
   int                 dercar;
   PtrElement          elCour;
   boolean             selok;
   boolean             trouve, stop;
   boolean             chaineTrouve;
   boolean             erreur;


   erreur = FALSE;
   if (DomaineCherche->SDocument != NULL)
      if (DomaineCherche->SDocument->DocSSchema != NULL)
	 switch (ref)
	       {
		  case NumZoneTextSearch:
		     /* Chaine a chercher */
		     strcpy (pChaineCherchee, txt);
		     if (strcmp (pChaineCherchee, pChainePreced) != 0)
		       {
			  RemplacementFait = FALSE;
			  strcpy (pChainePreced, pChaineCherchee);
		       }
		     LgChaineCh = strlen (pChaineCherchee);
		     break;
		  case NumZoneTextReplace:
		     /* Chaine a remplacer */
		     strcpy (pChaineRemplace, txt);
		     LgChaineRempl = strlen (pChaineRemplace);
		     /* bascule automatiquement le remplacement */
		     if (!AvecRemplace)
		       {
			  AvecRemplace = TRUE;
			  FaireRemplacement = TRUE;
			  TtaSetMenuForm (NumMenuReplaceMode, 1);
		       }
		     break;
		  case NumToggleRegExp:
		     if (val == 0)
			/* toggle button expression reguliere */
			ExpressionReguliere = !ExpressionReguliere;
		     else
			/* toggle button MAJUSCULES = minuscules */
			MajEgalMin = !MajEgalMin;
		     break;
		  case NumMenuReplaceMode:
		     /* sous-menu mode de remplacement */
		     switch (val)
			   {
			      case 0:
				 /* Sans remplacement */
				 AvecRemplace = FALSE;
				 RemplacementAutomatique = FALSE;
				 FaireRemplacement = FALSE;
				 break;
			      case 1:
				 /* Remplacement a la demande */
				 AvecRemplace = TRUE;
				 RemplacementAutomatique = FALSE;
				 FaireRemplacement = TRUE;
				 break;
			      case 2:
				 /* Remplacement automatique */
				 AvecRemplace = TRUE;
				 RemplacementAutomatique = TRUE;
				 FaireRemplacement = TRUE;
				 break;
			   }
		     break;
		  case NumFormSearchText:
		     /* Boutons de la feuille de dialogue */
		     if (val == 2 && AvecRemplace && !DebutRecherche)
			FaireRemplacement = FALSE;
		     else if (val == 0)
			/* Abandon de la recherhce */
			return;

		     selok = GetCurrentSelection (&docsel, &premsel, &dersel, &premcar, &dercar);
		     if (selok)
			if (docsel != DomaineCherche->SDocument)
			   selok = FALSE;
		     if (!selok && DebutRecherche)
		       {
			  /* Pas de selection -> recherche dans tout le document */
			  elCour = NULL;
		       }
		     else if (docsel != DomaineCherche->SDocument)
		       {
			  /* pas de selection dans le document d'ou vient la commande */
			  TtaDestroyDialogue (NumFormSearchText);
			  return;
		       }
		     else if (DebutRecherche)
		       {
			  elCour = NULL;
		       }
		     else if (DomaineCherche->SStartToEnd)
		       {
			  elCour = dersel;
		       }
		     else
		       {
			  elCour = premsel;
		       }

		     /* la recherche est demandee, on recupere les parametres */
		     if (ThotLocalActions[T_strsearchgetparams] != NULL)
			(*ThotLocalActions[T_strsearchgetparams]) (&erreur, DomaineCherche);
		     if (!erreur || LgChaineCh != 0)
		       {
			  trouve = FALSE;
			  if (LgChaineCh == 0)
			    {
			       if (ThotLocalActions[T_strsearchonly] != NULL)
				  (*ThotLocalActions[T_strsearchonly]) (elCour, DomaineCherche, &trouve);
			    }
			  else
			     /* on cherche une chaine de caracteres */
			     /* eventuellement, avec remplacement */
			    {
			       premsel = elCour;
			       do
				 {
				    stop = TRUE;
				    /* on sortira de la boucle si on ne */
				    /* trouve pas le texte cherche' */
				    if (AvecRemplace && FaireRemplacement && !DebutRecherche
					&& TextOK
				    && DocTextOK == DomaineCherche->SDocument
					&& ElemTextOK == premsel
					&& FirstCharTextOK == premcar
					&& LastCharTextOK == dercar)
				       /* il faut faire un remplacement et on est sur le */
				       /* texte cherche' */
				       /* on ne remplace pas dans un sous-arbre en */
				       /* lecture seule */
				       if (ElementIsReadOnly (premsel))
					  TtaNewLabel (NumLabelAttributeValue, NumFormSearchText,
						       TtaGetMessage (LIB, EL_RO));
				       else if (!premsel->ElIsCopy && premsel->ElText != NULL
						&& premsel->ElTerminal
					&& premsel->ElLeafType == LtText)
					  /* on ne remplace pas dans une copie */
					 {
					    trouve = TRUE;
					    if (ThotLocalActions[T_strsearcheletattr] != NULL)
					       (*ThotLocalActions[T_strsearcheletattr]) (premsel, &trouve);
					    if (trouve)
					      {
						 /* effectue le remplacement du texte */
						 ReplaceString (DomaineCherche->SDocument,
								premsel, premcar, LgChaineCh,
								pChaineRemplace, LgChaineRempl,
						  !RemplacementAutomatique);
						 RemplacementFait = TRUE;
						 DebutRecherche = FALSE;
						 /* met eventuellement a jour la borne de */
						 /* fin du domaine de recherche */
						 if (premsel == DomaineCherche->SEndElement)
						    /* la borne est dans l'element ou` on a */
						    /* fait le remplacement */
						    if (DomaineCherche->SEndChar != 0)
						       /* la borne n'est pas a la fin de */
						       /* l'element, on decale la borne */
						       DomaineCherche->SEndChar += LgChaineRempl - LgChaineCh;
						 /* recupere les parametres de la nouvelle */
						 /* chaine */
						 if (!RemplacementAutomatique)
						    selok = GetCurrentSelection (&docsel, &premsel, &dersel,
							 &premcar, &dercar);
					      }
					 }

				    do
				      {
					 /*Recherche de la prochaine occurence du texte cherche' */
					 if (premsel == NULL)
					   {
					      /* debut de recherche */
					      if (DomaineCherche->SStartToEnd)
						{
						   premsel = DomaineCherche->SStartElement;
						   premcar = DomaineCherche->SStartChar;
						   if (premsel == NULL)
						      premsel = DomaineCherche->SDocument->DocRootElement;
						}
					      else
						{
						   premsel = DomaineCherche->SEndElement;
						   premcar = DomaineCherche->SEndChar;
						}
					   }
					 else if (DomaineCherche->SStartToEnd)
					   {
					      premsel = dersel;
					      premcar = dercar + 1;
					   }

					 if (DomaineCherche->SStartToEnd)
					   {
					      dersel = DomaineCherche->SEndElement;
					      dercar = DomaineCherche->SEndChar;
					   }
					 else
					   {
					      dersel = DomaineCherche->SStartElement;
					      dercar = DomaineCherche->SStartChar;
					   }

					 if (ExpressionReguliere)
					   {
					      if (!DomaineCherche->SStartToEnd)
						 if (premcar > 0)
						    premcar--;
					      trouve = SearchRegularExpression (&premsel, &premcar, &dersel,
								 &dercar, DomaineCherche->SStartToEnd,
					       MajEgalMin, pChaineCherchee);
					   }
					 else
					   {
					      trouve = SearchText (DomaineCherche->SDocument, &premsel,
						 &premcar, &dersel, &dercar,
						DomaineCherche->SStartToEnd,
								MajEgalMin, pChaineCherchee, LgChaineCh);
					      if (trouve)
						 dercar--;
					   }

					 chaineTrouve = trouve;
					 if (trouve)
					    /* on a trouve' la chaine cherchee */
					   {
					      stop = FALSE;
					      if (ThotLocalActions[T_strsearcheletattr] != NULL)
						 (*ThotLocalActions[T_strsearcheletattr]) (premsel, &trouve);
					   }
				      }
				    while (chaineTrouve && !trouve);

				    if (trouve)
				      {
					 /* on a trouve la chaine recherchee dans le bon type */
					 /* d'element et avec le bon attribut */
					 if (!RemplacementAutomatique)
					   {
					      /* selectionne la chaine trouvee */
					      SelectStringWithEvent (DomaineCherche->SDocument,
						  premsel, premcar, dercar);
					      /* arrete la boucle de recherche */
					      stop = TRUE;
					      dercar++;
					   }
					 TextOK = TRUE;
					 DocTextOK = DomaineCherche->SDocument;
					 ElemTextOK = premsel;
					 FirstCharTextOK = premcar;
					 LastCharTextOK = dercar;
				      }
				    else
				      {
					 TextOK = FALSE;
					 stop = TRUE;
					 if (DomaineCherche->SWholeDocument)
					    /* il faut rechercher dans tout le document */
					    /* cherche l'arbre a traiter apres celui ou` on */
					    /* n'a pas trouve' */
					    if (NextTree (&premsel, &premcar, DomaineCherche))
					      {
						 stop = FALSE;
						 dersel = premsel;
						 dercar = 0;
					      }
				      }
				    DebutRecherche = FALSE;
				 }
			       while (!stop);
			    }
			  if (trouve)
			    {
			       /* on a trouve' et selectionne'. */
			       DebutRecherche = FALSE;
			       if (ThotLocalActions[T_strsearchshowvalattr] != NULL)
				  (*ThotLocalActions[T_strsearchshowvalattr]) ();
			    }
			  else
			     /* on n'a pas trouve' */
			    {
			       if (AvecRemplace && RemplacementFait)
				  /* message "Plus de remplacement" */
				  TtaNewLabel (NumLabelAttributeValue,
					       NumFormSearchText,
					       TtaGetMessage (LIB, NOTHING_TO_REPLACE));
			       else
				  /* message "Pas trouve'" */
				  TtaNewLabel (NumLabelAttributeValue,
					       NumFormSearchText,
					TtaGetMessage (LIB, NOT_FOUND));
			       DebutRecherche = TRUE;
			    }
		       }
		     break;
		  default:
		     if (ThotLocalActions[T_strsearchretmenu] != NULL)
			(*ThotLocalActions[T_strsearchretmenu]) (ref, val, txt, DomaineCherche);
		     break;
	       }
}

/* ---------------------------------------------------------------------- */
/* |    MenuAllerPage traite la commande Aller page numero              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MenuAllerPage (PtrDocument pDoc, int VueDoc, int VueSch, boolean Assoc)
#else  /* __STDC__ */
void                MenuAllerPage (pDoc, VueDoc, VueSch, Assoc)
PtrDocument         pDoc;
int                 VueDoc;
int                 VueSch;
boolean             Assoc;

#endif /* __STDC__ */
{
   char                buffTitre[200];

   if (ThotLocalActions[T_searchpage] == NULL)
      TteConnectAction (T_searchpage, (Proc) RetMenuAllerPage);

   /* garde le pointeur sur le document concerne' par la commande */
   DocPageCherchee = pDoc;
   /* garde le  numero de vue (dans le document) de la vue concernee */
   if (Assoc)
      VueDocPageCherchee = 1;
   else
      VueDocPageCherchee = VueDoc;
   /* garde la racine de l'arbre ou on va chercher une page, ainsi que */
   /* le numero (dans le schema de presentation) de la vue concernee */
   if (Assoc)
      /* c'est une vue d'elements associes */
     {
	/* les elements associes n'ont qu'une vue, la vue 1 */
	VueSchPageCherchee = 1;
	RacinePageCherchee = pDoc->DocAssocRoot[VueDoc - 1];
     }
   else
      /* c'est une vue de l'arbre principal */
     {
	/* cherche le numero de vue dans le schema de presentation */
	/* applique' au document */
	VueSchPageCherchee = AppliedView (pDoc->DocRootElement, NULL, pDoc,
					    VueDoc);
	RacinePageCherchee = pDoc->DocRootElement;
     }
   /* compose le titre "Recherche dans le document..." */
   strcpy (buffTitre, TtaGetMessage (LIB, SEARCH_IN));
   strcat (buffTitre, pDoc->DocDName);
   /* cree formulaire de saisie du numero de la page cherchee */
   TtaNewSheet (NumFormSearchPage, 0, 0, 0,
      buffTitre, 1, TtaGetMessage (LIB, LIB_CONFIRM), TRUE, 1, 'L', D_DONE);

   /* cree zone de saisie du numero de la page cherchee */
   TtaNewNumberForm (NumZoneSearchPage, NumFormSearchPage,
		     TtaGetMessage (LIB, GOTO_PAGE), 0, 9999, FALSE);
   /* affiche le formulaire */
   TtaShowDialogue (NumFormSearchPage, FALSE);
}

#ifdef IV
/* ---------------------------------------------------------------------- */
/* |    MoveToPage effectue un deplacement sur les pages.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MoveToPage (int frame, int func)
#else  /* __STDC__ */
void                MoveToPage (frame, func)
int                 frame;
int                 func;

#endif /* __STDC__ */
{
   PtrElement          PP;
   PtrElement          premv;
   PtrDocument         docsel;
   int                 VueDoc;
   int                 VueSch;
   boolean             assoc;

   PP = NULL;
   if (frame > 0)
     {
	GetDocAndView (frame, &docsel, &VueDoc, &assoc);
	if (assoc)
	   /* c'est une vue d'elements associes */
	   /* les elements associes n'ont qu'une vue */
	   VueSch = 1;
	else
	   /* c'est une vue de l'arbre principal */
	   /* cherche le numero de vue dans le schema */
	   /* de presentation applique' au document */
	   VueSch = AppliedView (docsel->DocRootElement,
				   NULL, docsel, VueDoc);
	premv = FirstVisible (docsel, VueDoc, assoc);
	if (func >= 1 && func <= 5 && premv != NULL)
	  {
	     switch (func)
		   {
		      case 1:
			 /* Premiere page ou debut du document */
			 PP = SearchPageBreak (premv, VueSch, -999999, TRUE);
			 break;
		      case 2:
			 /* Derniere page ou fin du document */
			 PP = SearchPageBreak (premv, VueSch, 999999, TRUE);
			 PP = SearchPageBreak (PP, VueSch, -1, TRUE);
			 break;
		      case 3:
			 /* Page GetNextBox */
			 PP = SearchPageBreak (premv, VueSch, 1, TRUE);
			 break;
		      case 4:
			 /* Page GetPreviousBox */
			 PP = SearchPageBreak (premv, VueSch, -1, TRUE);
			 break;
		      case 5 /* Page Numero N */ :
			 MenuAllerPage (docsel, VueDoc, VueSch, assoc);
			 break;

		   }
	     if (func != 5)
	       {
		  /* VueDoc represente maintenant le numero de vue dans le */
		  /* document, meme pour les elements associes */
		  if (assoc)
		     VueDoc = 1;
		  ScrollPageToTop (PP, VueDoc, docsel);
	       }
	  }
     }
}
#endif

/* ---------------------------------------------------------------------- */
/* |    RetMenuAllerPage traite les retours du formulaire de saisie du  | */
/* |            numero de la page ou aller.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuAllerPage (int ref, int val)
#else  /* __STDC__ */
void                RetMenuAllerPage (ref, val)
int                 ref;
int                 val;

#endif /* __STDC__ */
{
   PtrElement          pPage;

   if (DocPageCherchee != NULL)
      if (DocPageCherchee->DocSSchema != NULL)
	 /* le document concerne' est toujours la */
	 switch (ref)
	       {
		  case NumZoneSearchPage:
		     /* zone de saisie du numero de la page cherchee */
		     NumPageCherchee = val;
		     break;
		  case NumFormSearchPage:
		     /* formulaire de saisie du numero de la page cherchee */
		     /* cherche la page */
		     pPage = SearchPageBreak (RacinePageCherchee, VueSchPageCherchee,
					  NumPageCherchee, FALSE);
		     /* fait afficher la page trouvee en haut de sa frame */
		     ScrollPageToTop (pPage, VueDocPageCherchee, DocPageCherchee);
		     TtaDestroyDialogue (NumFormSearchPage);
		     break;
	       }
}

/* End Of Module cherche */

/* ---------------------------------------------------------------------- */
/* |    Initialise les variables des commandes de recherche.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                SearchLoadResources (void)
#else
void                SearchLoadResources ()
#endif				/* __STDC__ */
{
   if (ThotLocalActions[T_searchtext] == NULL)
     {
	/* Connecte les actions liees au traitement du search */
	TteConnectAction (T_searchtext, (Proc) RetMenuRemplacerTexte);
	TteConnectAction (T_locatesearch, (Proc) RetMenuOuRechercher);
	ReferCour = NULL;
	DocDeReferCour = NULL;
	ElemRefCour = NULL;
	pDocExtCour = NULL;
	CmdRefPrec = 0;
	GetSearchContext (&DomaineCherche);
     }
   pChaineCherchee[0] = '\0';
   LgChaineCh = 0;
   ExpressionReguliere = FALSE;
   MajEgalMin = FALSE;
   AvecRemplace = FALSE;
   pChaineRemplace[0] = '\0';
   LgChaineRempl = 0;
}
