
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
 * searchmenu.c : diverse document search functions.
 *
 * Authors: I. Vatton (INRIA)
 *          S. Bonhomme (INRIA) - Separation between structured and
 *                                unstructured editing modes
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "dialog.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "interface.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "platform_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "tree_f.h"
#include "structcreation_f.h"
#include "references_f.h"
#include "structmodif_f.h"
#include "viewcommands_f.h"

#include "absboxes_f.h"
#include "changeabsbox_f.h"
#include "schemas_f.h"
#include "createabsbox_f.h"
#include "search_f.h"
#include "exceptions_f.h"
#include "actions_f.h"
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

/* table des natures utilisees dans le document ou on cherche un type */
#define LgTable 10
static PtrSSchema   TableNaturesDoc[LgTable];
static int          LgTableNaturesDoc;

/* table des natures prises en compte dans le recherche de type */
static ThotBool     TableNaturesCherchees[LgTable];
static CHAR_T         NomTypeAChercher[THOT_MAX_CHAR];		/* le nom du type a chercher */
static CHAR_T         NomAttrAChercher[THOT_MAX_CHAR];		/* le nom de l'attribut a chercher */

					  /* defini l'attribut recherche' */
int                 ValAttrCherche;	/* valeur de l'attribut recherche' */

#define LgMaxAttrTxtCherche 80
CHAR_T                ValAttrTxtCherche[LgMaxAttrTxtCherche];	/* valeur de l'attribut recherche', */

#define LgLabelBuffer 200

/* description des attributs qui se trouvent dans le menu de recherche */
/* des attributs */
#define nbmaxentrees 40
static int          NbEntreesTableAttr;
static PtrSSchema   AttrStructCh[nbmaxentrees];
static int          AttrNumeroCh[nbmaxentrees];
static PtrSSchema   SchAttrCherche;	/* schema de structure ou est */

  /* defini  l'attribut recherche' */
static int          NumAttrCherche;	/* numero de l'attribut recherche' */
static PtrSSchema   pStrTypeCherche;
static int          NumTypeCherche;
static PtrAttribute pAttrTrouve;

/*----------------------------------------------------------------------
   ConstruitSelecteurAttributs construit le selecteur des attributs
   a chercher pour le document pDoc.                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ConstruitSelecteurAttributs (PtrDocument pDoc)
#else
static void         ConstruitSelecteurAttributs (pDoc)
PtrDocument         pDoc;

#endif
{
#define LgMaxListeAttr 980
   int                 i, nbitem;
   CHAR_T                ListeAttr[LgMaxListeAttr];
   int                 lgmenu;
   int                 nbentrees;
   int                 entree;
   int                 att;
   TtAttribute        *pAt1;

   /* construit la table des attributs definis dans tous ces schemas */
   nbentrees = 0;
   ListeAttr[0] = EOS;
   /* on met l'attribut Langue en tete de la table des attributs a */
   /* chercher */
   AttrStructCh[nbentrees] = NULL;
   AttrNumeroCh[nbentrees] = 1;
   nbentrees++;
   /* parcourt la table des natures du document concerne' */
   for (i = 0; i < LgTableNaturesDoc; i++)
      if (TableNaturesCherchees[i])
	 /* cette nature doit etre prise en compte */
	 /* met tous les attributs d'un schema dans la table */
	{
	   /* on saute l'attribut Langue */
	   att = 1;
	   while (att < TableNaturesDoc[i]->SsNAttributes
		  && nbentrees < nbmaxentrees)
	     {
		att++;
		if (!AttrHasException (ExcInvisible, att, TableNaturesDoc[i]))
		   /* l'attribut est montrable a l'utilisateur */
		  {
		     /* conserve dans la table des attributs a chercher le */
		     /* schema de structure et le numero d'attribut de */
		     /* cette nouvelle entree du menu */
		     AttrStructCh[nbentrees] = TableNaturesDoc[i];
		     AttrNumeroCh[nbentrees] = att;
		     nbentrees++;
		  }
	     }
	}
   NbEntreesTableAttr = nbentrees;
   if (nbentrees >= 1)
      /* il y a des attributs declares */
     {
	/* met l'entree 'Quelconque' au debut de la liste des attributs */
	ustrcpy (ListeAttr, TtaGetMessage (LIB, TMSG_ANY));
	nbitem = 1;
	lgmenu = ustrlen (ListeAttr) + 1;
	for (entree = 0; entree < nbentrees &&
	     lgmenu < LgMaxListeAttr - MAX_NAME_LENGTH - 4; entree++)
	   /* met les noms de tous les attributs dans la liste */
	  {
	     if (AttrStructCh[entree] == NULL)
		pAt1 = &pDoc->DocSSchema->SsAttribute[AttrNumeroCh[entree] - 1];
	     else
		pAt1 = &AttrStructCh[entree]->SsAttribute[AttrNumeroCh[entree] - 1];
	     ustrncpy (ListeAttr + lgmenu, pAt1->AttrName, MAX_NAME_LENGTH);
	     lgmenu += ustrlen (pAt1->AttrName) + 1;
	     nbitem++;
	  }
	if (entree < nbentrees + 1 && lgmenu >= LgMaxListeAttr - MAX_NAME_LENGTH - 4)
	   /* le buffer est trop petit ... */
	  {
	     ustrncpy (ListeAttr + lgmenu, TEXT("..."), 4);
	     lgmenu += 4;
	     nbitem++;
	  }
	/* cree le selecteur des attributs a chercher */
	TtaNewSelector (NumSelAttributeToSearch, NumFormSearchText,
			TtaGetMessage (LIB, TMSG_ATTR_TO_SEARCH), nbitem, ListeAttr, 5, NULL, TRUE, FALSE);
	/* initialise le selecteur (rien n'est selectionne') */
	TtaSetSelector (NumSelAttributeToSearch, -1, _EMPTYSTR_);
     }
}


/*----------------------------------------------------------------------
   ConstruitSelecteurTypes construit le selecteur qui donne le choix  
   des types definis dans les schemas de nature demandees  
   par l'utilisateur.                                      
  ----------------------------------------------------------------------*/
static void         ConstruitSelecteurTypes ()
{
#define LgMaxListeTypes 980
   CHAR_T                ListeTypes[LgMaxListeTypes];
   int                 nbitem, lgmenu;
   int                 nat, regle, premregle;
   PtrSSchema          pSS;



   nbitem = 0;
   ListeTypes[0] = EOS;
   lgmenu = 0;
   premregle = 1;
   /* parcourt la table des natures utilisees dans le document traite' */
   for (nat = 0; nat < LgTableNaturesDoc; nat++)
      if (TableNaturesCherchees[nat])
	 /* cette nature doit etre prise en compte */
	{
	   pSS = TableNaturesDoc[nat];
	   /* parcourt la table des regles du schema de structure de la */
	   /* nature */
	   for (regle = premregle; regle <= pSS->SsNRules &&
		lgmenu < LgMaxListeTypes - MAX_NAME_LENGTH - 4; regle++)
	      if (regle != (int) Refer + 1)
		 /* on ne retient que les types qui ne portent pas */
		 /* l'exception Hidden */
		 if (!TypeHasException (ExcHidden, regle, pSS))
		   {
		      ustrncpy (ListeTypes + lgmenu, pSS->SsRule[regle - 1].SrName,
			       MAX_NAME_LENGTH);
		      lgmenu += ustrlen (pSS->SsRule[regle - 1].SrName) + 1;
		      nbitem++;
		   }
	   premregle = MAX_BASIC_TYPE + 1;
	   if (regle <= pSS->SsNRules && lgmenu >= LgMaxListeTypes - MAX_NAME_LENGTH - 4)
	      /* le buffer est trop petit ... */
	     {
		ustrncpy (ListeTypes + lgmenu, TEXT("..."), 4);
		lgmenu += 4;
		nbitem++;
	     }
	}
   /* cree le selecteur des types disponibles */
   if (nbitem == 0)
      TtaNewSelector (NumSelTypeToSearch, NumFormSearchText,
		      TtaGetMessage (LIB, TMSG_TYPE_TO_SEARCH), 1, TEXT(" "), 5, NULL, TRUE, FALSE);
   else
      TtaNewSelector (NumSelTypeToSearch, NumFormSearchText,
		      TtaGetMessage (LIB, TMSG_TYPE_TO_SEARCH), nbitem, ListeTypes, 5, NULL, TRUE, FALSE);
   /* initialise le selecteur (rien n'est selectionne') */
   TtaSetSelector (NumSelTypeToSearch, -1, _EMPTYSTR_);
}


/*----------------------------------------------------------------------
   ChAttr  cherche dans le domaine decrit par context et a partir  
   partir de (et a l'interieur de) l'element elCour, le    
   premier element portant l'attribut GetAttributeOfElement (cet attribut   
   est defini dans le schema de structure pointe' par pSS).
   Retourne NULL si pas trouve', sinon retourne un         
   pointeur sur le bloc attribut trouve' et selectionne    
   l'element trouve'.                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrAttribute ChAttr (PtrElement elCour, PtrSearchContext context, int GetAttributeOfElement, PtrSSchema pSS)
#else
static PtrAttribute ChAttr (elCour, context, GetAttributeOfElement, pSS)
PtrElement          elCour;
PtrSearchContext    context;
int                 GetAttributeOfElement;
PtrSSchema          pSS;

#endif
{
   PtrElement          pEl;
   PtrElement          pAscendant;
   PtrAttribute        pAttr;
   ThotBool            trouve;

   pAttr = NULL;
   trouve = FALSE;
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
   do
     {
	if (context->SStartToEnd)
	   /* Recherche en avant */
	   pEl = FwdSearchAttribute (pEl, GetAttributeOfElement, ValAttrCherche,
				     ValAttrTxtCherche, pSS);
	else
	   /* Recherche en arriere */
	   pEl = BackSearchAttribute (pEl, GetAttributeOfElement, ValAttrCherche,
				      ValAttrTxtCherche, pSS);
	if (pEl != NULL)
	   /* on a trouve' un element portant l'attribut voulu, on verifie */
	   /* que cet element ne fait pas partie d'une inclusion, n'est */
	   /* pas cache' a l'utilisateur et que l'attribut trouve' n'est */
	   /* pas cache' a l'utilisateur */
	  {
	     pAscendant = pEl;
	     while (pAscendant->ElParent != NULL && pAscendant->ElSource == NULL)
		pAscendant = pAscendant->ElParent;
	     if (pAscendant->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		   /* l'element n'est pas cache' */
		  {
		     /* cherche l'attribut sur l'element */
		     pAttr = pEl->ElFirstAttr;
		     if (pAttr != NULL)
			/* si on cherche un attribut quelconque, on prend le */
			/* premier attribut qui n'est pas cache' */
			/* parcourt les attributs de l'element */
			do
			  {
			     if ((pSS == NULL || pAttr->AeAttrSSchema->SsCode == pSS->SsCode) &&
				 (GetAttributeOfElement == 0 || pAttr->AeAttrNum == GetAttributeOfElement))
				/* c'est l'attribut cherche' */
				if (!AttrHasException (ExcInvisible, pAttr->AeAttrNum,
						       pAttr->AeAttrSSchema))
				   /* l'attribut est montrable a l'utilisateur */
				   trouve = TRUE;
			     if (!trouve)
				pAttr = pAttr->AeNext;
			  }
			while (pAttr != NULL && !trouve);
		  }
	  }
     }
   while (pEl != NULL && !trouve);

   if (pEl != NULL && trouve)
     {
	/* on a trouve' un element portant l'attribut cherche' */
	/* l'element trouve' est pointe' par pEl */
	if (context->SStartToEnd)
	  {
	     if (context->SEndElement != NULL)
		/* il faut s'arreter avant l'extremite' du document */
		if (pEl != context->SEndElement)
		   /* l'element trouve' n'est pas l'element ou` il faut */
		   /* s'arreter */
		   if (ElemIsBefore (context->SEndElement, pEl))
		      /* l'element trouve' est apres l'element de fin, on */
		      /* fait comme si on n'avait pas trouve' */
		      pEl = NULL;
	  }
	else if (context->SStartElement != NULL)
	   /* il faut s'arreter avant l'extremite' du document */
	   if (pEl != context->SStartElement)
	      /* l'element trouve' n'est pas l'element ou` il faut */
	      /* s'arreter */
	      if (ElemIsBefore (pEl, context->SStartElement))
		 /* l'element trouve' est avant le debut du domaine, on */
		 /* fait comme si on n'avait pas trouve' */
		 pEl = NULL;
	if (pEl != NULL)
	   /* on selectionne l'element trouve' */
	   SelectElementWithEvent (context->SDocument, pEl, TRUE, FALSE);
     }
   return (pAttr);
}

/*----------------------------------------------------------------------
   ChType  cherche dans le domaine decrit par context, a partir de 
   (et a l'interieur de) l'element elCour, le premier      
   element dont le type a pour nom NomType.                
   Retourne un pointeur sur l'element si trouve', ou NULL  
   si echec.                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   ChType (PtrElement elCour, PtrSearchContext context, STRING NomType)
#else
static PtrElement   ChType (elCour, context, NomType)
PtrElement          elCour;
PtrSearchContext    context;
STRING               NomType;

#endif
{
   PtrElement          pEl;
   PtrElement          pAscendant;
   ThotBool            trouve;

   trouve = FALSE;
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
   do
     {
	if (context->SStartToEnd)
	   /* Recherche en avant */
	   pEl = FwdSearchElemByTypeName (pEl, NomType);
	else
	   /* Recherche en arriere */
	   pEl = BackSearchElemByTypeName (pEl, NomType);
	if (pEl != NULL)
	   /* on a trouve' un element du type voulu, on verifie que cet */
	   /* element ne fait pas partie d'une inclusion et n'est pas */
	   /* cache' a l'utilisateur */
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
   while (pEl != NULL && !trouve);
   if (pEl != NULL && trouve)
      /* on a trouve' */
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
      else
	{
	   if (context->SStartElement != NULL)
	      /* il faut s'arreter avant l'extremite' du document */
	      if (pEl != context->SStartElement)
		 /*l'element trouve' n'est pas l'element ou il faut s'arreter */
		 if (ElemIsBefore (pEl, context->SStartElement))
		    /* l'element trouve' est apres l'element de fin, on */
		    /* fait comme si on n'avait pas trouve' */
		    pEl = NULL;
	}
   return (pEl);
}


/*----------------------------------------------------------------------
   InitMenuNatures    initialise le menu des natures a chercher       
   pour le document pDoc.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitMenuNatures (PtrDocument pDoc)
#else
static void         InitMenuNatures (pDoc)
PtrDocument         pDoc;

#endif
{
   int                 nbitem;
   CHAR_T                ListeTypes[LgMaxListeTypes];
   int                 lgmenu;
   int                 nat;
   SRule              *pRe;

   /* met d'abord le schema de structure du document dans la table des */
   /* natures utilisees dans le document */
   TableNaturesDoc[0] = pDoc->DocSSchema;
   LgTableNaturesDoc = 1;
   /* cherche tous les schemas de structure utilise's dans le document */
   SearchNatures (pDoc->DocSSchema, TableNaturesDoc, &LgTableNaturesDoc, TRUE);
   if (LgTableNaturesDoc == 1)
      /* une seule nature dans le document, on ne met pas le menu des */
      /* natures */
      TtaDetachForm (NumMenuSearchNature);
   else
     {
	/* construit le menu des natures */
	nbitem = 0;
	ListeTypes[0] = EOS;
	lgmenu = 0;
	for (nat = 0; nat < LgTableNaturesDoc && lgmenu < LgMaxListeTypes - MAX_NAME_LENGTH; nat++)
	  {
	     ustrcpy (ListeTypes + lgmenu, TEXT("B"));
	     if (TableNaturesDoc[nat]->SsExtension)
	       {
		  ustrncpy (ListeTypes + lgmenu + 1, TableNaturesDoc[nat]->SsName,
			   MAX_NAME_LENGTH);
		  lgmenu += ustrlen (TableNaturesDoc[nat]->SsName) + 2;
	       }
	     else
	       {
		  /* on cherche la regle racine du schema de structure pour */
		  /* avoir le nom traduit dans la langue de l'utilisateur */
		  pRe = &TableNaturesDoc[nat]->SsRule
		     [TableNaturesDoc[nat]->SsRootElem - 1];
		  ustrncpy (ListeTypes + lgmenu + 1, pRe->SrName, MAX_NAME_LENGTH);
		  lgmenu += ustrlen (pRe->SrName) + 2;
	       }
	     nbitem++;
	  }
	TtaNewToggleMenu (NumMenuSearchNature, NumFormSearchText,
	 TtaGetMessage (LIB, TMSG_NATURES), nbitem, ListeTypes, NULL, TRUE);
	TtaSetToggleMenu (NumMenuSearchNature, 0, TRUE);
	TtaAttachForm (NumMenuSearchNature);
     }
   /* a priori on recherche la nature racine */
   TableNaturesCherchees[0] = TRUE;
   for (nat = 1; nat < LgTableNaturesDoc; nat++)
      TableNaturesCherchees[nat] = FALSE;
}

/*----------------------------------------------------------------------
  BuildStructSearchMenu
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildStructSearchMenu (PtrDocument pDoc)
#else
void                BuildStructSearchMenu (pDoc)
PtrDocument         pDoc;

#endif
{
   CHAR_T BufMenu [MAX_TXT_LEN];

   /* menu des natures utilisees dans le document */
   /* NumMenuSearchNature, cree' dynamiquement par cherche.c */
   BufMenu[0] = 'B';
   BufMenu[1] = EOS;
   ustrcat (BufMenu,TtaGetMessage (LIB, TMSG_ANY));
   TtaNewToggleMenu (NumMenuSearchNature, NumFormSearchText,
	TtaGetMessage (LIB, TMSG_NATURES), 1, BufMenu,
		     NULL, TRUE);

   /* selecteur pour la saisie du type de l'element a chercher */
   /* NumSelTypeToSearch, cree' dynamiquement par cherche.c */
   TtaNewSelector (NumSelTypeToSearch, NumFormSearchText,
		   TtaGetMessage (LIB, TMSG_TYPE_TO_SEARCH), 1,
		   TtaGetMessage (LIB, TMSG_ANY), 5, NULL, TRUE, FALSE);

   /* selecteur de choix de l'attribut a chercher */
   /* NumSelAttributeToSearch, cree' dynamiquement par ChercherAttribut */
   TtaNewSelector (NumSelAttributeToSearch, NumFormSearchText,
		   TtaGetMessage (LIB, TMSG_ATTR_TO_SEARCH), 1,
		   TtaGetMessage (LIB, TMSG_ANY), 5, NULL, TRUE, FALSE);

   /* label indiquant la valeur de l'attribut trouve' */
   TtaNewLabel (NumLabelAttributeValue, NumFormSearchText, TEXT(" "));
   /* annule le label donnant la valeur de l'attribut trouve' */
   TtaNewLabel (NumLabelAttributeValue, NumFormSearchText, TEXT(" "));
   NomTypeAChercher[0] = EOS;
   /* construit le menu des natures utilisees dans le document */
   InitMenuNatures ((PtrDocument) pDoc);
   /* construit le selecteur donnant la liste des types definis dans le */
   /* schema de structure du document */
   ConstruitSelecteurTypes ();
   ValAttrCherche = 0;
   ValAttrTxtCherche[0] = EOS;
   /* initialise le selecteur des attributs a chercher */
   ConstruitSelecteurAttributs ((PtrDocument) pDoc);
   if (NbEntreesTableAttr < 1)
      /* pas d'attributs declare's dans les schemas de structure */
      /* utilise's par le document, on detache le selecteur */
      TtaDetachForm (NumSelAttributeToSearch);
   else
      /* il y a des attributs declares, on attache le selecteur */
      TtaAttachForm (NumSelAttributeToSearch);
}


/*----------------------------------------------------------------------
   ElPossedeAttr   Si l'element pEl possede l'attribut de numero   
   NumAttr defini dans le schema de structure pSchAttr,    
   retourne un pointeur sur ce bloc attribut,              
   sinon, retourne NULL.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrAttribute ElPossedeAttr (PtrElement pEl, int NumAttr, PtrSSchema pSchAttr)
#else
static PtrAttribute ElPossedeAttr (pEl, NumAttr, pSchAttr)
PtrElement          pEl;
int                 NumAttr;
PtrSSchema          pSchAttr;

#endif
{
   PtrAttribute        pA;
   ThotBool            trouve;

   trouve = FALSE;
   pA = pEl->ElFirstAttr;
   if (pA != NULL)
      /* l'element a au moins un attribut */
      if (pSchAttr == NULL && NumAttr == 0)
	 /* on cherche un attribut quelconque, on a trouve */
	 trouve = TRUE;
      else
	 /* parcourt les attributs de l'element */
	 do
	    if ((pSchAttr == NULL || pA->AeAttrSSchema->SsCode == pSchAttr->SsCode)
		&& pA->AeAttrNum == NumAttr)
	       /* c'est l'attribut cherche' */
	       trouve = TRUE;
	    else
	       pA = pA->AeNext;
	 while (pA != NULL && !trouve);
   if (trouve && (ValAttrCherche != 0 || ValAttrTxtCherche[0] != EOS))
      /* on a trouve l'attribut cherche', on verifie sa valeur */
     {
	trouve = FALSE;
	switch (pA->AeAttrType)
	      {
		 case AtNumAttr:
		 case AtEnumAttr:
		    if (pA->AeAttrValue == ValAttrCherche)
		       trouve = TRUE;
		    break;
		 case AtTextAttr:
		    if (StringAndTextEqual (ValAttrTxtCherche, pA->AeAttrText))
		       trouve = TRUE;
		    break;
		 case AtReferenceAttr:
		    trouve = TRUE;
		    break;
		 default:
		    break;
	      }
	if (!trouve)
	   pA = NULL;
     }
   return pA;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   ChTypeAttr (PtrElement elCour, PtrSearchContext context, STRING NomType, ThotBool AvecAttribut, int NumAttrCherche, PtrSSchema SchAttrCherche, PtrAttribute * AttrTrouve)
#else
static PtrElement   ChTypeAttr (elCour, context, NomType, AvecAttribut, NumAttrCherche, SchAttrCherche, AttrTrouve)
PtrElement          elCour;
PtrSearchContext    context;
STRING               NomType;
ThotBool            AvecAttribut;
int                 NumAttrCherche;
PtrSSchema          SchAttrCherche;
PtrAttribute       *AttrTrouve;

#endif
{
   PtrElement          pEl;
   PtrElement          pAsc;
   PtrElement          ElSuiv;
   int                 i;
   ThotBool            trouve;
   PtrAttribute        pAttrTrouve;

   trouve = FALSE;
   pAttrTrouve = NULL;
   pEl = elCour;
   do
     {
	pEl = ChType (pEl, context, NomType);
	if (pEl != NULL)
	   /* on a trouve' un element du type cherche' */
	   if (!AvecAttribut)
	      /* on ne cherche que sur le type, on a trouve' */
	      trouve = TRUE;
	   else
	      /* on cherche egalement un attribut */
	      /* on verifie que cet element ou un de ses ascendants porte */
	      /* l'attribut cherche' */
	     {
		pAsc = pEl;
		do
		  {
		     pAttrTrouve = ElPossedeAttr (pAsc, NumAttrCherche,
						  SchAttrCherche);
		     if (pAttrTrouve == NULL)
			/* l'element ne porte pas cet attribut, on passe au pere */
			pAsc = pAsc->ElParent;
		     else
			/* l'element porte l'attribut cherche' */
		     if (SchAttrCherche == NULL && NumAttrCherche == 0)
			/* on cherchait un attribut quelconque */
			if (pAsc != pEl)
			   /* l'attribut n'est pas sur l'element lui-meme */
			   if (pAttrTrouve->AeAttrNum == 1)
			      /* c'est un attribut Langue, on ignore */
			     {
				pAttrTrouve = NULL;
				pAsc = pAsc->ElParent;
			     }
		  }
		while (pAsc != NULL && pAttrTrouve == NULL);
		trouve = (pAttrTrouve != NULL);
	     }
     }
   while (pEl != NULL && !trouve);

   *AttrTrouve = pAttrTrouve;
   if (pEl != NULL && trouve)
      /* on a trouve' */
      SelectElementWithEvent (context->SDocument, pEl, TRUE, FALSE);
   if (!trouve)
      if (context->SWholeDocument)
	 /* il faut rechercher dans tout le document */
	 /* cherche l'arbre a traiter apres celui ou` on n'a pas trouve' */
	 if (NextTree (&ElSuiv, &i, context))
	    /* il y a un autre arbre a traiter, on continue avec le */
	    /* debut de ce nouvel arbre */
	    pEl = ChTypeAttr (ElSuiv, context, NomType,
			      AvecAttribut, NumAttrCherche, SchAttrCherche,
			      AttrTrouve);
   return pEl;
}

#ifdef __STDC__
void                CallbackStructSearchMenu (int ref, int val, STRING txt, PtrSearchContext DomaineCherche)
#else
void                CallbackStructSearchMenu (ref, val, txt, DomaineCherche)
int                 ref;
int                 val;
STRING               txt;
PtrSearchContext    DomaineCherche;

#endif
{
   switch (ref)
	 {
	    case NumMenuSearchNature:
	       /* toggle menu des natures a chercher */
	       TableNaturesCherchees[val] = !TableNaturesCherchees[val];
	       /* l'utilisateur demande a changer les natures */
	       ConstruitSelecteurTypes ();
	       ConstruitSelecteurAttributs (DomaineCherche->SDocument);
	       break;
	    case NumSelTypeToSearch:
	       /* selecteur de saisie du type de l'element a chercher */
	       ustrncpy (NomTypeAChercher, txt, MAX_NAME_LENGTH - 1);
	       break;
	    case NumSelAttributeToSearch:
	       /* selecteur de choix de l'attribut a chercher */
	       ustrncpy (NomAttrAChercher, txt, MAX_NAME_LENGTH - 1);
	       break;
	    default:
	       break;
	 }
}

/*----------------------------------------------------------------------
   BuildGoToPageMenu traite la commande Aller page numero              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                BuildSearchOptions (ThotBool * erreur, PtrSearchContext DomaineCherche)
#else
void                BuildSearchOptions (erreur, DomaineCherche)
ThotBool           *erreur;
PtrSearchContext    DomaineCherche;

#endif
{
   ThotBool            trouve;
   int                 i;
   int                 entree;
   TtAttribute        *pAt1;


   /* annule le label donnant la valeur de l'attribut trouve' */
   TtaNewLabel (NumLabelAttributeValue, NumFormSearchText, TEXT(" "));
   for (i = 1; i <= LgTableNaturesDoc; i++)
      TableNaturesCherchees[i - 1] = FALSE;
   /* cherche le nom de type dans le schema de structure */
   NumTypeCherche = 0;
   if (NomTypeAChercher[0] != EOS)
     {
	pStrTypeCherche = DomaineCherche->SDocument->DocSSchema;
	GetSRuleFromName (&NumTypeCherche, &pStrTypeCherche, NomTypeAChercher, USER_NAME);
	if (NumTypeCherche == 0)
	  {
	     /* message 'Type inconnu' dans la feuille de saisie */
	     TtaNewLabel (NumLabelAttributeValue,
			  NumFormSearchText,
			  TtaGetMessage (LIB, TMSG_LIB_UNKNOWN_TYPE));
	     NomTypeAChercher[0] = EOS;
	     *erreur = TRUE;
	  }
     }

   /* cherche le nom d'attribut */
   NumAttrCherche = 0;
   SchAttrCherche = NULL;
   if (NomAttrAChercher[0] != EOS)
      /* il y a bien un attribut a chercher */
      if (ustrcmp (NomAttrAChercher, TtaGetMessage (LIB, TMSG_ANY)) != 0)
	{
	   /* cherche le nom de l'attribut dans la table */
	   trouve = FALSE;
	   for (entree = 1; entree <= NbEntreesTableAttr &&
		(!trouve); entree++)
	     {
		if (AttrStructCh[entree - 1] == NULL)
		   pAt1 = &DomaineCherche->SDocument->DocSSchema->
		      SsAttribute[AttrNumeroCh[entree - 1] - 1];
		else
		   pAt1 = &AttrStructCh[entree - 1]->
		      SsAttribute[AttrNumeroCh[entree - 1] - 1];
		trouve = ustrcmp (NomAttrAChercher, pAt1->AttrName) == 0;
		if (trouve)
		  {
		     SchAttrCherche = AttrStructCh[entree - 1];
		     NumAttrCherche = AttrNumeroCh[entree - 1];
		  }
	     }
	   if (!trouve)
	     {
		/* message 'TtAttribute inconnu' dans la feuille */
		TtaNewLabel (NumLabelAttributeValue,
			     NumFormSearchText,
			     TtaGetMessage (LIB, TMSG_UNKNOWN_ATTR));
		NomAttrAChercher[0] = EOS;
		*erreur = TRUE;
	     }
	}
   *erreur = *erreur || (NomTypeAChercher[0] == EOS && NomAttrAChercher[0] == EOS);
}


#ifdef __STDC__
void                StructSearch (PtrElement elCour, PtrSearchContext DomaineCherche, ThotBool * trouve)
#else
void                StructSearch (elCour, DomaineCherche, trouve)
ThotBool           *trouve;
PtrElement          elCour;
PtrSearchContext    DomaineCherche;

#endif
{
   ThotBool            stop;
   PtrElement          pEl;
   int                 i;

   pAttrTrouve = NULL;
   if (NomAttrAChercher[0] != EOS &&
       NomTypeAChercher[0] == EOS)
      /* on cherche uniquement un attribut */
      do
	{
	   /* lance la recherche de l'attribut demande' */
	   pAttrTrouve = ChAttr (elCour, DomaineCherche,
				 NumAttrCherche, SchAttrCherche);
	   *trouve = (pAttrTrouve != NULL);
	   stop = TRUE;
	   if (!*trouve)
	      if (DomaineCherche->SWholeDocument)
		 /* il faut rechercher dans tout le document */
		 /* cherche l'arbre a traiter apres celui ou` on */
		 /* n'a pas trouve' */
		 if (NextTree (&elCour, &i, DomaineCherche))
		    /* relance la recherche dans le nouvel arbre */
		    stop = FALSE;
	}
      while (!stop);
   else if (NomTypeAChercher[0] != EOS)
      /* on cherche un type d'element  */
      /* lance la recherche du type d'element demande' */
      do
	{
	   pEl = ChTypeAttr (elCour, DomaineCherche,
			     NomTypeAChercher,
			     (ThotBool)(NomAttrAChercher[0] != EOS),
			     NumAttrCherche, SchAttrCherche,
			     &pAttrTrouve);
	   *trouve = (pEl != NULL);
	   stop = TRUE;
	   if (!*trouve)
	      if (DomaineCherche->SWholeDocument)
		 /* il faut rechercher dans tout le document */
		 /* cherche l'arbre a traiter apres celui ou` on */
		 /* n'a pas trouve' */
		 if (NextTree (&elCour, &i, DomaineCherche))
		    /* relance la recherche dans le nouvel arbre */
		    stop = FALSE;
	}
      while (!stop);
}


#ifdef __STDC__
void                StructAndAttrSearch (PtrElement premsel, ThotBool * ok)
#else
void                StructAndAttrSearch (premsel, ok)
PtrElement          premsel;
ThotBool           *ok;

#endif
{
   PtrElement          pAsc;
   ThotBool            trouve;

   trouve = TRUE;
   if (NomTypeAChercher[0] != EOS)
      /* on cherche aussi un type d'element */
     {
	pAsc = premsel;
	do
	  {
	     trouve = (ustrcmp (NomTypeAChercher, pAsc->ElStructSchema->SsRule[pAsc->ElTypeNumber - 1].SrName) == 0);
	     if (!trouve)
		pAsc = pAsc->ElParent;
	  }
	while (pAsc != NULL && !trouve);
     }
   if (trouve && NomAttrAChercher[0] != EOS)
      /* on cherche aussi un attribut */
     {
	pAsc = premsel;
	do
	  {
	     pAttrTrouve = ElPossedeAttr (pAsc, NumAttrCherche,
					  SchAttrCherche);
	     if (pAttrTrouve == NULL)
		pAsc = pAsc->ElParent;
	  }
	while (pAsc != NULL && pAttrTrouve == NULL);
	trouve = (pAttrTrouve != NULL);
     }
   *ok = trouve;
}

#ifdef __STDC__
void                ValAttrSearch ()
#else
void                ValAttrSearch ()
#endif
{
   CHAR_T                NomAtt[100];
   int                 lg, lg1;
   CHAR_T                LabelBuffer[LgLabelBuffer];

   if (NomAttrAChercher[0] != EOS && pAttrTrouve != NULL)
      /* on cherche un attribut et on l'a trouve' */
      /* on ecrit dans la feuille de dialogue la valeur */
      /* de l'attribut trouve */
     {
	if (NumAttrCherche == 0)
	   /* on cherchait un attribut quelconque, on va */
	   /* afficher le nom de l'attribut trouve' */
	   if (pAttrTrouve->AeAttrType == AtReferenceAttr)
	      ustrcpy (LabelBuffer, pAttrTrouve->AeAttrSSchema->SsAttribute[pAttrTrouve->AeAttrNum - 1].AttrName);
	   else
	      usprintf (NomAtt, TEXT("%s = "), pAttrTrouve->AeAttrSSchema->SsAttribute[pAttrTrouve->AeAttrNum - 1].AttrName);
	else
	   ustrcpy (NomAtt, TtaGetMessage (LIB, TMSG_VALUE_OF_ATTR));
	switch (pAttrTrouve->AeAttrType)
	      {
		 case AtReferenceAttr:
		    break;
		 case AtNumAttr:
		    usprintf (LabelBuffer, TEXT("%s %d"), NomAtt, pAttrTrouve->AeAttrValue);
		    break;
		 case AtEnumAttr:
		    usprintf (LabelBuffer,TEXT("%s %s"), NomAtt,
			     pAttrTrouve->AeAttrSSchema->SsAttribute
			     [pAttrTrouve->AeAttrNum - 1].AttrEnumValue
			     [pAttrTrouve->AeAttrValue - 1]);
		    break;
		 case AtTextAttr:
		    ustrcpy (LabelBuffer, NomAtt);
		    lg = ustrlen (LabelBuffer);
		    lg1 = LgLabelBuffer - lg - 1;
		    CopyTextToString (pAttrTrouve->AeAttrText,
				      LabelBuffer + lg, &lg1);
		    break;
	      }
	TtaNewLabel (NumLabelAttributeValue,
		     NumFormSearchText, LabelBuffer);
     }
}

void                StructSearchLoadResources ()
{
   if (ThotLocalActions[T_strsearchconstmenu] == NULL)
     {
	TteConnectAction (T_strsearchconstmenu, (Proc) BuildStructSearchMenu);
	TteConnectAction (T_strsearchgetparams, (Proc) BuildSearchOptions);
	TteConnectAction (T_strsearchonly, (Proc) StructSearch);
	TteConnectAction (T_strsearcheletattr, (Proc) StructAndAttrSearch);
	TteConnectAction (T_strsearchshowvalattr, (Proc) ValAttrSearch);
	TteConnectAction (T_strsearchretmenu, (Proc) CallbackStructSearchMenu);
     }
}
