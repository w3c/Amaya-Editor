
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* ======================================================================= */
/* |                                                                    | */
/* |                           Projet THOT                              | */
/* |                                                                    | */
/* |            Traitements specifiques a la structure Tableau          | */
/* |                                                                    | */
/* |                                                                    | */
/* |                                                                    | */
/* ======================================================================= */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

/* variables importees */
#define EXPORT extern
#include "select.var"
#include "creation.var"
#include "modif.var"
#include "appdialogue.var"

/* procedures importees de l'Editeur */
#include "tree_f.h"
#include "structcreation_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "createabsbox_f.h"
#include "changeabsbox_f.h"
#include "presrules_f.h"
#include "structselect_f.h"
#include "attributes_f.h"
#include "attrherit_f.h"
#include "references_f.h"
#include "tableau2_f.h"


/* exceptions definies dans Tableau */
#include "exc_tableau.h"

/* Tableau_OrphanCell repond true si la cellule comporte
   un attribut reference a une colonne qui se trouve
   dans le buffer Couper-Coller. */

#ifdef __STDC__
static void Tableau_OrphanCell(PtrElement pCell, PtrElement pSauve, boolean *InCutBuffer)

#else  /* __STDC__ */
static void Tableau_OrphanCell(pCell, pSauve, InCutBuffer)
PtrElement          pCell;
PtrElement          pSauve;
boolean             *InCutBuffer;

#endif /* __STDC__ */

{
   int                 attr;
   PtrAttribute         pAttr;
   PtrElement          pCol, pE;

  *InCutBuffer = FALSE;
   /* cherche l'attribut La_colonne de la cellule */
   pCol = NULL;
   attr = GetAttrWithException (EXC_ID_Ref_Colonne, pCell->ElSructSchema);
   pAttr = pCell->ElFirstAttr;
   while (pAttr != NULL)
      if (pAttr->AeAttrSSchema == pCell->ElSructSchema && pAttr->AeAttrNum == attr)
	 /* C'est l'attribut La_colonne */
	{
	   if (pAttr->AeAttrReference != NULL	/* attribut reference */
	       && pAttr->AeAttrReference->RdReferred != NULL	/* la reference existe */
	       && !pAttr->AeAttrReference->RdReferred->ReExternalRef)	/* c'est un objet interne au document */
	      pCol = pAttr->AeAttrReference->RdReferred->ReReferredElem;
	   else
	      pCol = NULL;
	   break;
	}
      else			/* passe a l'attribut suivant de la cellule */
	 pAttr = pAttr->AeNext;

   *InCutBuffer = FALSE;

   if (pCol != NULL)
     {
	/* La colonne trouvee est-elle dans le buffer couper-coller ?
	   Pour cela on regarde si elle est soeur de pSauve (debut de la chaine 
	   des elts sauves)  ou si un des elts freres de pSauve la contient */

	for (pE = pSauve; pE != NULL; pE = pE->ElNext)
	   if (ElemIsWithinSubtree (pCol, pE))
	     {
		*InCutBuffer = TRUE;
		break;
	     }
     }

}



/* Tableau_EscapeSpecial traite le cas du shift escape losqu'on est
   sur une cellule de tableau : Dans ce cas on remonte a la colonne et non
   au corps de ligne */

#ifdef __STDC__
static boolean             Tableau_EscapeSpecial (PtrDocument SelDoc)

#else  /* __STDC__ */
static boolean             Tableau_EscapeSpecial (SelDoc)
PtrDocument         SelDoc;

#endif /* __STDC__ */

{
   int                 attr;
   PtrAttribute         pAttr;

   if (SelPremier == SelDernier)
     {
	if (TypeHasException (EXC_ID_Cellule, SelPremier->ElTypeNumber, SelPremier->ElSructSchema))
	  {
	     /* cherche l'attribut La_colonne de la cellule */
	     attr = GetAttrWithException (EXC_ID_Ref_Colonne, SelPremier->ElSructSchema);
	     pAttr = SelPremier->ElFirstAttr;
	     while (pAttr != NULL)
		if (pAttr->AeAttrSSchema == SelPremier->ElSructSchema && pAttr->AeAttrNum == attr)
		   /* C'est l'attribut La_colonne */
		  {
		     if (pAttr->AeAttrReference != NULL
			 && pAttr->AeAttrReference->RdReferred != NULL)
		       {
			  SelectEl (SelDoc, pAttr->AeAttrReference->RdReferred->ReReferredElem, TRUE, TRUE);
			  return TRUE;
		       }
		  }
		else
		   /* passe a l'attribut suivant de la cellule */
		   pAttr = pAttr->AeNext;
	  }
     }
   return FALSE;
}

/* DistribAlignHorizVertic      met un attribut Alignement horizontal ou Vertical */
/* sur toutes les elements portant une exception EXC_TR_Cellule_ATTRIBUT */

#ifdef __STDC__
static void                Tableau_DistribAlignHorizVertic (PtrElement pEl, PtrAttribute pAttrCell, PtrDocument SelDoc)

#else  /* __STDC__ */
static void                Tableau_DistribAlignHorizVertic (pEl, pAttrCell, SelDoc)
PtrElement          pEl;
PtrAttribute         pAttrCell;
PtrDocument         SelDoc;

#endif /* __STDC__ */

{
   PtrElement          pCell;
   PtrAttribute         pAttr;
   boolean             PasDExtension;

   if (TypeHasException (EXC_TR_Cellule_ATTRIBUT, pEl->ElTypeNumber, pEl->ElSructSchema))
     {
	/* Les attributs d'alignement vertical et d'extension verticale */
	/* des cellules sont exclusifs */
	PasDExtension = TRUE;
	if (pAttrCell->AeAttrNum == GetAttrWithException (EXC_ID_Align_Vertic, pEl->ElSructSchema))
	   /* on veut mettre un attribut d'alignement vertical */
	  {
	     pAttr = pEl->ElFirstAttr;
	     while (pAttr != NULL && PasDExtension)
		if (AttrHasException (EXC_ID_Extens_Vertic, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
		   PasDExtension = FALSE;
		else		/* passe a l'attribut suivant de la cellule */
		   pAttr = pAttr->AeNext;
	  }
	if (PasDExtension)
	   /* l'element n'a pas d'attribut d'extension verticale */
	   AttachAttrWithValue (pEl, SelDoc, pAttrCell);
     }

   if (!pEl->ElTerminal)
     {
	pCell = pEl->ElFirstChild;
	while (pCell != NULL)
	  {
	     Tableau_DistribAlignHorizVertic (pCell, pAttrCell, SelDoc);
	     pCell = pCell->ElNext;
	  }
     }
}

/* TableauAttributSpecial traite : */
/* - le cas particulier de l'attribut Type_Tableau  */
/*   si cet attribut est mis ou retire du tableau on recree l'image abstraite du tableau */
/* - le cas particulier de l'attribut Alignement horizontal  */
/* - le cas particulier de l'attribut Alignement vertical */

#ifdef __STDC__
static void                TableauAttributSpecial (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc)

#else  /* __STDC__ */
static void                TableauAttributSpecial (pEl, pAttr, pDoc)
PtrElement          pEl;
PtrAttribute         pAttr;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   AttributeBlock          AttrCell;
   PtrAttribute         pAttrCell;

   if (TypeHasException (EXC_ID_Tableau, pEl->ElTypeNumber, pEl->ElSructSchema)
   && pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Type_Tableau, pEl->ElSructSchema))
     {
	DetrPaves (pEl, pDoc, FALSE);
	AbstractImageUpdated (pDoc);
	CreeTousPaves (pEl, pDoc);
	AbstractImageUpdated (pDoc);
     }

   else if (AttrHasException (EXC_ID_Align_HorizDistrib, pAttr->AeAttrNum, pEl->ElSructSchema))
     {
	/* distribution sur les elements cellule inclus d'un attribut Alignement horizontal */
	pAttrCell = &AttrCell;
	AttrCell = *pAttr;
	AttrCell.AeAttrNum = GetAttrWithException (EXC_ID_Align_Horiz, pEl->ElSructSchema);
	Tableau_DistribAlignHorizVertic (pEl, pAttrCell, pDoc);
     }

   else if (AttrHasException (EXC_ID_Align_VerticDistrib, pAttr->AeAttrNum, pEl->ElSructSchema))
     {
	/* distribution sur les elements cellule inclus d'un attribut Alignement Vertical */
	pAttrCell = &AttrCell;
	AttrCell = *pAttr;
	AttrCell.AeAttrNum = GetAttrWithException (EXC_ID_Align_Vertic, pEl->ElSructSchema);
	Tableau_DistribAlignHorizVertic (pEl, pAttrCell, pDoc);
     }
}


/* Tableau_TypeExclus retourne vrai si on peut inserer l'element dans un menu de choix */

#ifdef __STDC__
static boolean             Tableau_TypeExclus (PtrElement pEl, int eltype, PtrSSchema pSS)

#else  /* __STDC__ */
static boolean             Tableau_TypeExclus (pEl, eltype, pSS)
PtrElement          pEl;
int                 eltype;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int                 attr;
   PtrAttribute         pAttrTab;
   PtrElement          pElAttr;

   if (eltype == GetElemWithException (EXC_ID_Colonne_Composee, pSS)
       || eltype == GetElemWithException (EXC_ID_Ligne_Composee, pSS))
     {
	/* on est dans un tableau sur un element necessitant
	   un traitement particulier lors de son insertion dans un menu d'insertion */
	attr = GetAttrWithException (EXC_ID_Type_Tableau, pEl->ElSructSchema);
	if ((pAttrTab = GetTypedAttrAncestor (pEl, attr, pEl->ElSructSchema, &pElAttr)))
	  {
	     /* le tableau porte bien un attribut Type_Tableau */

	     /* on inhibe le choix colonne composee quand
	        quand l'attribut Type_Tableau vaut Lignes ou Tabulations */
	     if (eltype == GetElemWithException (EXC_ID_Colonne_Composee, pSS)
		 && (pAttrTab->AeAttrValue == 2 || pAttrTab->AeAttrValue == 4))
		return (TRUE);

	     /* on inhibe le choix ligne composee quand
	        quand l'attribut Type_Tableau vaut Colonnes ou Tabulations */
	     else if (eltype == GetElemWithException (EXC_ID_Ligne_Composee, pSS)
		  && (pAttrTab->AeAttrValue == 3 || pAttrTab->AeAttrValue == 4))
		return (TRUE);
	  }
     }

   return (FALSE);
}

/* TbAttributRef        retourne vrai si l'attribut pointe' par pAttr est */
/* l'attribut Ref_largeur ou l'attribut La_colonne. */

#ifdef __STDC__
static void             Tableau_AttributRef (PtrAttribute pAttr, boolean *ret)

#else  /* __STDC__ */
boolean             Tableau_AttributRef (pAttr, ret)
PtrAttribute         pAttr;
boolean             *ret;

#endif /* __STDC__ */

{

   *ret = FALSE;
   if (AttrHasException (EXC_ID_Ref_Titre, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
      *ret = TRUE;		/* c'est l'attribut Ref_largeur */
   else if (AttrHasException (EXC_ID_Ref_Colonne, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
      *ret = TRUE;		/* c'est l'attribut La_colonne */
}


/* MetAttrTitreLigne    met les attribut Ref_Largeur_Titre sur l'element pointe' par */
/* pTitre et l'attribut PourCent_largeur avec une valeur qui depend du */
/* degre' d'imbrication des lignes composees. */

#ifdef __STDC__
static void                Tableau_MetAttrTitreLigne (PtrElement pTitre, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_MetAttrTitreLigne (pTitre, pDoc)
PtrElement          pTitre;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pT, pTitreTableau;
   PtrAttribute         pAttr;
   int                 pourcent;

   /* cherche l'element Titre_Tableau */
   pTitreTableau = NULL;
   pE = pTitre;
   do				/* remonte a la racine du tableau */
      pE = pE->ElParent;
   while (!TypeHasException (EXC_ID_Tableau, pE->ElTypeNumber, pE->ElSructSchema));

   pTitreTableau = pE->ElFirstChild->ElFirstChild;
   if (pTitreTableau != NULL)
     {
	if (!TypeHasException (EXC_ID_Titre_Tableau, pTitreTableau->ElTypeNumber, pE->ElSructSchema))
	  {
	     printf ("On ne trouve pas le Titre du Tableau.\n");
	     return;
	  }
	/* met l'attribut Ref_largeur sur l'element et le fait pointer sur */
	/* l'element Titre_Tableau */
	pAttr = AttachAttrByExceptNum (EXC_ID_Ref_Titre, pTitre, pTitreTableau, pDoc);
     }
   /* met l'attribut PourCent_largeur sur l'element Titre */
   pAttr = AttachAttrByExceptNum (EXC_ID_PourCent_largeur, pTitre, NULL, pDoc);
   /* compte le nombre de Lignes composees qui contiennent notre */
   /* Titre de ligne pour calculer la valeur de l'attribut */
   pourcent = 100;
   pE = pTitre;
   while (!TypeHasException (EXC_ID_Les_Lignes, pE->ElTypeNumber, pE->ElSructSchema))
      /* on n'est pas encore arrive' a l'element Les_lignes */
     {
	if (TypeHasException (EXC_ID_Ligne_Composee, pE->ElTypeNumber, pE->ElSructSchema))
	   /* c'est une ligne composee */
	   pourcent = pourcent / 2;
	pE = pE->ElParent;
     }
   pAttr->AeAttrValue = pourcent;

   /* traite tous les titres de ligne imbriques */
   if (TypeHasException (EXC_ID_Titre_Ligne_Comp, pTitre->ElTypeNumber, pTitre->ElSructSchema))
      /* c'est un titre de ligne composee, il y a donc des titres de ligne imbriques */
     {
	pE = pTitre->ElNext;	/* passe a l'element Sous_lignes */
	if (pE != NULL)
	  {
	     pE = pE->ElFirstChild;	/* passe a la premiere ligne imbriquee */
	     while (pE != NULL)	/* traite toutes les lignes imbriquees */
	       {
		  pT = pE->ElFirstChild;	/* passe au titre de la ligne imbriquee */
		  if (pT != NULL)
		    {
		       if (TypeHasException (EXC_ID_Titre_Ligne, pT->ElTypeNumber, pT->ElSructSchema) ||
			   TypeHasException (EXC_ID_Titre_Ligne_Comp, pT->ElTypeNumber, pT->ElSructSchema))
			  /* c'est bien un element Titre_ligne ou Titre_ligne_comp, */
			  /* on verifie ses attributs */
			  Tableau_MetAttrTitreLigne (pT, pDoc);
		    }
		  pE = pE->ElNext;
	       }
	  }
     }
}

/* MetAttributLigne     met l'attribut Filet_Gauche et Filet_Bas a l'element */

#ifdef __STDC__
static void                Tableau_MetAttrLigne (PtrElement pLigne, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_MetAttrLigne (pLigne, pDoc)
PtrElement          pLigne;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Filet_Bas_LigSimp, pLigne, NULL, pDoc);
   pAttr->AeAttrValue = 2;

   if (TypeHasException (EXC_ID_Ligne_Composee, pLigne->ElTypeNumber, pLigne->ElSructSchema))
     {
	pAttr = AttachAttrByExceptNum (EXC_ID_Filet_Droit_LigComp, pLigne, NULL, pDoc);
	pAttr->AeAttrValue = 2;
     }
}


/* MetAttrTitreColonne  met l'attribut a l'element */

#ifdef __STDC__
void                Tableau_MetAttrTitreColonne (PtrElement pTitreCol, PtrDocument pDoc)

#else  /* __STDC__ */
void                Tableau_MetAttrTitreColonne (pTitreCol, pDoc)
PtrElement          pTitreCol;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
}


/* MetAttrColonneSimple met l'attribut Largeur a l'element */

#ifdef __STDC__
static void                Tableau_MetAttrColonneSimple (PtrElement pCol, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_MetAttrColonneSimple (pCol, pDoc)
PtrElement          pCol;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Largeur_Colonne, pCol, NULL, pDoc);
   pAttr->AeAttrValue = 50;
   pAttr = AttachAttrByExceptNum (EXC_ID_Filet_Droit_ColSimp, pCol, NULL, pDoc);
   pAttr->AeAttrValue = 2;
}


/* MetAttributColonneComp       met l'attribut Filet_Haut a l'element si */
/* la colonne est simple */

#ifdef __STDC__
static void                Tableau_MetAttrColonneComp (PtrElement pCol, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_MetAttrColonneComp (pCol, pDoc)
PtrElement          pCol;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Filet_Bas_ColComp, pCol, NULL, pDoc);
   pAttr->AeAttrValue = 2;
   pAttr = AttachAttrByExceptNum (EXC_ID_Filet_Droit_ColSimp, pCol, NULL, pDoc);
   pAttr->AeAttrValue = 2;
}


/* MetAttributTitreTableau met l'attribut Filet_Haut et Filet_Gauche */

#ifdef __STDC__
static void                Tableau_MetAttrTitreTableau (PtrElement pTitreTab, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_MetAttrTitreTableau (pTitreTab, pDoc)
PtrElement          pTitreTab;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Largeur_Titre, pTitreTab, NULL, pDoc);
   pAttr->AeAttrValue = 70;
}


/* MetAttributCellule   met l'attribut Ref_Colonne a l'element */
/* utilise pColonne comme valeur pour la reference */

#ifdef __STDC__
static void                Tableau_MetAttrCellule (PtrElement pCell, PtrElement pCol, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_MetAttrCellule (pCell, pCol, pDoc)
PtrElement          pCell;
PtrElement          pCol;
PtrDocument         pDoc;

#endif /* __STDC__ */

{

   AttachAttrByExceptNum (EXC_ID_Ref_Colonne, pCell, pCol, pDoc);
}


/* MetAttributLesColonnes met l'attribut Filet_Gauche */

#ifdef __STDC__
static void                Tableau_MetAttrLesColonnes (PtrElement pLesCol, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_MetAttrLesColonnes (pLesCol, pDoc)
PtrElement          pLesCol;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Filet_Gauche_LesCol, pLesCol, NULL, pDoc);
   pAttr->AeAttrValue = 2;
}


/* Tableau_MetAttrEnTetes met l'attribut Filet_Inferieur */

#ifdef __STDC__
static void                Tableau_MetAttrEnTetes (PtrElement pEnTetes, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_MetAttrEnTetes (pEnTetes, pDoc)
PtrElement          pEnTetes;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Filet_Inf_EnTetes, pEnTetes, NULL, pDoc);
   pAttr->AeAttrValue = 2;
}

/* MetAttributTable */

#ifdef __STDC__
static void                Tableau_MetAttrTable (PtrElement pTable, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_MetAttrTable (pTable, pDoc)
PtrElement          pTable;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Cadre_Tableau, pTable, NULL, pDoc);
   pAttr->AeAttrValue = 2;
}


/* CreeColSimple        On vient de creer l'element Colonne_simple pointe' */
/* par pEl, dans le document pointe' par pDoc. Ajoute l'attribut */
/* Filet-vertical a cet element et cree une cellule dans chaque ligne */
/* simple du tableau. Associe a chaque cellule creee un attribut */
/* La_colonne qui pointe sur la colonne simple. */

#ifdef __STDC__
static void                Tableau_CreeColSimple (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_CreeColSimple (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pCol, pLigne, pLesLignes, pCell, pNCell;
   int                 NType, TypeCell;
   PtrAttribute         pAttr;
   PtrElement          pElRef;
   int                 attr;
   boolean             PremCol, attrtrouve;

   /* met les attributs a l'element Colonne_simple */
   Tableau_MetAttrColonneSimple (pEl, pDoc);
   /* met les attributs sur le titre de colonne */
   if (pEl->ElFirstChild != NULL)
      Tableau_MetAttrTitreColonne (pEl->ElFirstChild, pDoc);
   /* cree les paves de l'element */
   CreeTousPaves (pEl, pDoc);

   /* remonte a l'element En_Tetes */
   NType = GetElemWithException (EXC_ID_En_Tetes, pEl->ElSructSchema);
   pE = pEl->ElParent;
   while (pE->ElTypeNumber != NType)
      pE = pE->ElParent;
   /* l'element apres En_Tetes est Les_lignes */
   pLesLignes = pE->ElNext;

   /* verifie s'il y a des colonnes simples qui precedent la nouvelle */
   /* colonne simple */
   pCol = FwdSearchTypedElem (pE, pEl->ElTypeNumber, pEl->ElSructSchema);
   PremCol = (pCol == pEl);

   /* numero du type d'element Ligne_simple */
   NType = GetElemWithException (EXC_ID_Ligne_Simple, pEl->ElSructSchema);
   /* numero du type d'element Cellule */
   TypeCell = GetElemWithException (EXC_ID_Cellule, pEl->ElSructSchema);

   /* Cherche toutes les lignes simples */
   pLigne = pLesLignes;
   while (pLigne != NULL)
     {
	pLigne = FwdSearchTypedElem (pLigne, NType, pEl->ElSructSchema);
	if (pLigne != NULL)
	   if (!ElemIsWithinSubtree (pLigne, pLesLignes))
	      /* la ligne trouvee ne fait pas partie du tableau */
	      pLigne = NULL;
	   else
	     {
		/* on a trouve' une ligne simple du tableau */
		/* on passe a la premiere cellule de la ligne */
		pCell = pLigne->ElFirstChild->ElNext->ElFirstChild;
		/* on cree une nouvelle cellule */
		pNCell = NewSubtree (TypeCell, pEl->ElSructSchema, pDoc,
			 pLigne->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		/* on insere cette nouvelle cellule dans l'arbre */
		if (pCell == NULL)
		   /* la ligne n'avait pas encore de cellules */
		   InsertFirstChild (pLigne->ElFirstChild->ElNext, pNCell);
		else if (PremCol)
		   /* on cree la 1ere colonne, on insere la nouvelle cellule */
		   /* avant la 1ere cellule */
		   InsertElementBefore (pCell, pNCell);
		else
		   /* on saute les premieres cellules */
		  {
		     /* cherche d'abord le numero de l'attribut La_colonne */
		     attr = GetAttrWithException (EXC_ID_Ref_Colonne, pCell->ElSructSchema);
		     /* on saute les cellules correspondant aux colonnes simples */
		     /* precedant notre colonne */
		     while (pCell != NULL)
		       {
			  /* Cherche d'abord l'attribut La_colonne de la cellule courante */
			  pAttr = pCell->ElFirstAttr;
			  attrtrouve = FALSE;
			  while (!attrtrouve && pAttr != NULL)
			     if (pAttr->AeAttrNum == attr && pAttr->AeAttrSSchema == pCell->ElSructSchema)
				attrtrouve = TRUE;
			     else
				pAttr = pAttr->AeNext;
			  if (attrtrouve)
			     /* l'element porte cet attribut */
			    {
			       /* on cherche d'abord l'element reference' par l'attribut */
			       pElRef = NULL;
			       /* a priori, pas d'element reference' */
			       if (pAttr->AeAttrReference != NULL)
				  if (pAttr->AeAttrReference->RdReferred != NULL)
				     if (!pAttr->AeAttrReference->RdReferred->ReExternalRef)
					pElRef = pAttr->AeAttrReference->RdReferred->ReReferredElem;
			       if (pElRef != NULL)
				  if (!ElemIsBefore (pElRef, pEl))
				     /* la tetiere de cette cellule est apres la nouvelle colonne */
				    {
				       /* Insere la nouvelle cellule avant cette cellule */
				       InsertElementBefore (pCell, pNCell);
				       pCell = NULL;
				       /* on arrete */
				    }
			    }
			  if (pCell != NULL)
			     if (pCell->ElNext != NULL)
				/* passe a la cellule suivante de la ligne */
				pCell = pCell->ElNext;
			     else
			       {
				  /* c'etait la derniere cellule de la ligne. On insere */
				  /* la nouvelle cellule apres la derniere */
				  InsertElementAfter (pCell, pNCell);
				  pCell = NULL;
			       }
		       }
		  }
		/* on met l'attribut La_colonne a la nouvelle cellule et le */
		/* fait pointer sur la nouvelle colonne */
		Tableau_MetAttrCellule (pNCell, pEl, pDoc);
		/* traite les attributs requis */
		AttachMandatoryAttributes (pNCell, pDoc);
		CreeTousPaves (pNCell, pDoc);
	     }
     }
}


/* passe a la ligne imbriquee suivante */
/* CreeLigneSimple      On vient de creer une ligne simple. Associe */
/* un attribut Filet_horizontal a cette ligne et cree une cellule dans */
/* la ligne pour chaque colonne simple du tableau. Associe a chaque */
/* Cellule creee un attribut La_colonne qui pointe vers la colonne */
/* correspondante */

#ifdef __STDC__
static void                Tableau_CreeLigneSimple (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_CreeLigneSimple (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pCol, pLesCol, pCell, pCellPrec;
   int                 TypeCol;


   /* met les attributs a la ligne */
   Tableau_MetAttrLigne (pEl, pDoc);
   /*pTitre = pEl->ElFirstChild; */
   /* met les attributs au titre de la ligne */
   Tableau_MetAttrTitreLigne (pEl->ElFirstChild, pDoc);

   /* remonte a l'element Les_lignes */
   pE = pEl->ElParent;
   while (!TypeHasException (EXC_ID_Les_Lignes, pE->ElTypeNumber, pEl->ElSructSchema))
      pE = pE->ElParent;
   /*pLigne = pE; */
   /* l'element Les_colonnes */
   pLesCol = pE->ElPrevious->ElFirstChild->ElNext;
   /* cherche le numero du type d'element Colonne_simple */
   TypeCol = GetElemWithException (EXC_ID_Colonne_Simple, pEl->ElSructSchema);

   /* cherche la premiere cellule dans la nouvelle ligne */
   pCell = pEl->ElFirstChild->ElNext->ElFirstChild; 
   /* parcourt toutes les colonnes simples du tableau */
   pCol = pLesCol;
   while (pCol != NULL)
     {
	pCol = FwdSearchTypedElem (pCol, TypeCol, pEl->ElSructSchema);
	if (pCol != NULL)
	   if (!ElemIsWithinSubtree (pCol, pLesCol))
	      /* la colonne trouvee ne fait pas partie du tableau */
	      pCol = NULL;
	   else
	     {
		/* on trouve' une colonne simple du meme tableau */
		if (pCell == NULL && pCellPrec != NULL)
		  {
		     /* on cree une nouvelle cellule */
		     NewSubtree (pCellPrec->ElTypeNumber, pEl->ElSructSchema, pDoc,
				 pCellPrec->ElAssocNum, TRUE, TRUE,
				 TRUE, TRUE);
		     /* on insere cette nouvelle cellule dans l'arbre */
		     InsertElementAfter (pCellPrec, pCell);
		  }
		/* on met les attributs a la nouvelle cellule */
		Tableau_MetAttrCellule (pCell, pCol, pDoc);
		/* traite les attributs requis */
		AttachMandatoryAttributes (pCell, pDoc);
		pCellPrec = pCell;
		pCell = pCell->ElNext;
	     }
     }

   if (pCell != NULL)
     {
	/* il y a trop de cellules par rapport aux colonnes */
	if (pCellPrec != NULL)
	   while (pCellPrec->ElNext != NULL)
	      RemoveElement (pCellPrec->ElNext);
	else
	   RemoveElement (pCell);
     }
}


/* CreeTable    complete le squelette de tableau cree par les */
/* procedures standard de l'editeur Thot. pEl pointe sur l'element */
/* Table. */

#ifdef __STDC__
static void                Cree_Tableau (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Cree_Tableau (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pL, pC, pNouv;
   int                 NType;
   PtrSSchema        pSS;


   pSS = pEl->ElSructSchema;
   /* met les attribut sur l'element Table */
   Tableau_MetAttrTable (pEl, pDoc);
   /* passe a l'element En_Tetes */
   pE = pEl->ElFirstChild;
   if (pE != NULL)
     {
	/* met les attributs sur En_Tetes */
	Tableau_MetAttrEnTetes (pE, pDoc);
	/* met les attributs sur Titre_Tableau */
	Tableau_MetAttrTitreTableau (pE->ElFirstChild, pDoc);

	pC = pE->ElFirstChild->ElNext;		/* element Les_colonnes */
	/* met les attributs sur Les colonnes */
	Tableau_MetAttrLesColonnes (pC, pDoc);

	pL = pE->ElNext;	/* element Les_lignes */


	if (!TypeHasException (EXC_ID_Les_Colonnes, pC->ElTypeNumber, pSS))
	   printf ("On ne trouve pas Les_colonnes\n");
	else
	  {
	     /* descend a la premiere Colonne */
	     pE = pC->ElFirstChild;
	     if (!TypeHasException (EXC_ID_Colonne, pE->ElTypeNumber, pSS))
		printf ("On ne trouve pas la 1ere Colonne\n");
	     else
	       {
		  /* transforme toutes les colonnes creees en Colonne_simple */
		  NType = GetElemWithException (EXC_ID_Colonne_Simple, pSS);
		  /* numero du type Colonne_simple */
		  do
		    {
		       /* transforme la Colonne en Colonne_simple */
		       pNouv = NewSubtree (NType, pSS, pDoc, pE->ElAssocNum, TRUE, TRUE,
					   TRUE, TRUE);
		       InsertOption (pE, &pNouv, pDoc);
		       /* met les attributs a la Colonne_simple */
		       Tableau_MetAttrColonneSimple (pE, pDoc);
		       /* met les attributs au Titre de la colonne */
		       Tableau_MetAttrTitreColonne (pE->ElFirstChild, pDoc);
		       /* traite les attributs requis */
		       AttachMandatoryAttributes (pE, pDoc);
		       pE = pE->ElNext;
		    }
		  while (!(pE == NULL));

		  if (!TypeHasException (EXC_ID_Les_Lignes, pL->ElTypeNumber, pSS))
		     printf ("On ne trouve pas Les_lignes\n");
		  else
		    {
		       /* passe a la premiere Line */
		       pE = pL->ElFirstChild;
		       if (!TypeHasException (EXC_ID_Ligne, pE->ElTypeNumber, pSS))
			  printf ("On ne trouve pas la 1ere Line\n");
		       else
			 {
			    /* traite toutes les lignes creees */
			    NType = GetElemWithException (EXC_ID_Ligne_Simple, pSS);
			    /* numero du type Ligne_simple */
			    do
			      {
				 /* transforme la Line en Ligne_simple */
				 pNouv = NewSubtree (NType, pSS, pDoc, pE->ElAssocNum,
					      TRUE, TRUE, TRUE, TRUE);
				 InsertOption (pE, &pNouv, pDoc);
				 /* traite les attributs requis */
				 AttachMandatoryAttributes (pE, pDoc);
				 Tableau_CreeLigneSimple (pE, pDoc);
				 /* passe a la ligne suivante */
				 pE = pE->ElNext;
			      }
			    while (!(pE == NULL));
			 }
		    }
	       }
	  }
     }
}


/* Tableau_CreeFiletLigne       Si l'element pLigne est bien une */
/* ligne de tableau et l'element pBasPage un Bas_tableau, cree */
/* la boite de presentation Filet_Bas de la ligne pLigne. */

#ifdef __STDC__
static void                Tableau_CreeFiletLigne (PtrElement pLigne, PtrElement pBasPage, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_CreeFiletLigne (pLigne, pBasPage, pDoc)
PtrElement          pLigne;
PtrElement          pBasPage;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 attr;
   PtrAttribute         pAttr;
   boolean             trouve;

   if (pBasPage != NULL && pLigne != NULL)
      if (TypeHasException (EXC_ID_BasTableau, pBasPage->ElTypeNumber, pBasPage->ElSructSchema))
	 /* le bas de page est bien un bas de page */
	 if (TypeHasException (EXC_ID_Ligne_Simple, pLigne->ElTypeNumber, pLigne->ElSructSchema) ||
	     TypeHasException (EXC_ID_Ligne_Composee, pLigne->ElTypeNumber, pLigne->ElSructSchema))
	    /* la ligne est bien une ligne de tableau */
	   {
	      /* cherche l'attribut Filet_Bas de la ligne */
	      attr = GetAttrWithException (EXC_ID_Filet_Bas_LigSimp, pLigne->ElSructSchema);
	      pAttr = pLigne->ElFirstAttr;
	      trouve = FALSE;
	      while (pAttr != NULL && !trouve)
		 if (pAttr->AeAttrSSchema == pLigne->ElSructSchema &&
		     pAttr->AeAttrNum == attr)
		    /* C'est l'attribut Filet_Bas */
		    trouve = TRUE;
		 else
		    /* passe a l'attribut suivant de la ligne */
		    pAttr = pAttr->AeNext;
	      if (trouve)
		 /* applique les regles de presentation de l'attribut pour */
		 /* faire creer la boite filet. */
		 ChngPresAttr (pLigne, pAttr, pDoc, FALSE, FALSE, NULL);
	   }
}



/* TbCreation   verifie si l'element pointe par pEl est une colonne ou */
/* une ligne de table et dans ce cas cree toutes les cellules de cette */
/* colonne ou de cette ligne. */
/* Si l'element pEl n'est pas un element terminal cherche dans le sous arbre */
/* si par hasard il ne striuve pas un element tableau */
/* Ce cas se produit quand on enchaine la creation d'un grand */
/* nombre d'elements */

#ifdef __STDC__
static void                Tableau_Creation_DoIt (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_Creation_DoIt (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pC;
   int                 NType;


   /* il s'agit bien de la creation d'un element de table */
   if (TypeHasException (EXC_ID_Tableau, pEl->ElTypeNumber, pEl->ElSructSchema))
      /* c'est la creation d'un tableau */
      Cree_Tableau (pEl, pDoc);

   else if (TypeHasException (EXC_ID_Colonne_Simple, pEl->ElTypeNumber, pEl->ElSructSchema))
      /* creation d'une Colonne_simple */
      Tableau_CreeColSimple (pEl, pDoc);

   else if (TypeHasException (EXC_ID_Colonne_Composee, pEl->ElTypeNumber, pEl->ElSructSchema))
      /* creation d'une Colonne_composee */
     {
	/* met les attributs de la colonne composee */
	Tableau_MetAttrColonneComp (pEl, pDoc);
	/* met les attributs sur le Titre_colonne_comp */
	Tableau_MetAttrTitreColonne (pEl->ElFirstChild, pDoc);
	/* cree les paves de l'element */
	CreeTousPaves (pEl, pDoc);
	AbstractImageUpdated (pDoc);
	/* descend a la premiere Colonne */
	pE = pEl->ElFirstChild->ElNext->ElFirstChild;
	if (!TypeHasException (EXC_ID_Colonne, pE->ElTypeNumber, pE->ElSructSchema) &&
	    !TypeHasException (EXC_ID_Colonne_Simple, pE->ElTypeNumber, pE->ElSructSchema) &&
	    !TypeHasException (EXC_ID_Colonne_Composee, pE->ElTypeNumber, pE->ElSructSchema))
	   printf ("On ne trouve pas la 1ere Colonne\n");
	else
	  {
	     /* traite toutes les colonnes creees */
	     /* numero du type Colonne_simple */
	     NType = GetElemWithException (EXC_ID_Colonne_Simple, pEl->ElSructSchema);
	     do
	       {
		  if (TypeHasException (EXC_ID_Colonne, pE->ElTypeNumber, pE->ElSructSchema))
		    {
		       /* transforme la Colonne en Colonne_simple */
		       pC = NewSubtree (NType, pEl->ElSructSchema, pDoc, pEl->ElAssocNum,
					TRUE, TRUE, TRUE, TRUE);
		       InsertOption (pE, &pC, pDoc);
		       /* traite les attributs requis */
		       AttachMandatoryAttributes (pE, pDoc);
		       Tableau_CreeColSimple (pE, pDoc);
		    }
		  pE = pE->ElNext;
	       }
	     while (!(pE == NULL));
	  }
     }

   else if (TypeHasException (EXC_ID_Ligne_Simple, pEl->ElTypeNumber, pEl->ElSructSchema))
      /* creation d'une Ligne_simple */
      Tableau_CreeLigneSimple (pEl, pDoc);

   else if (TypeHasException (EXC_ID_Ligne_Composee, pEl->ElTypeNumber, pEl->ElSructSchema))
      /* creation d'une Ligne_composee */
     {
	/* met l'attribut Filet_horizontal */
	Tableau_MetAttrLigne (pEl, pDoc);
	/* met l'attribut sur le Titre_ligne_comp */
	Tableau_MetAttrTitreLigne (pEl->ElFirstChild, pDoc);
	/* descend a la premiere Line */
	pE = pEl->ElFirstChild->ElNext->ElFirstChild;
	if (!TypeHasException (EXC_ID_Ligne, pE->ElTypeNumber, pE->ElSructSchema))
	   printf ("On ne trouve pas la 1ere Line\n");
	else
	  {
	     /* traite toutes les lignes creees */
	     /* numero du type Ligne_simple */
	     NType = GetElemWithException (EXC_ID_Ligne_Simple, pEl->ElSructSchema);
	     do
	       {
		  /* transforme la Line en Ligne_simple */
		  pC = NewSubtree (NType, pEl->ElSructSchema, pDoc, pEl->ElAssocNum,
				   TRUE, TRUE, TRUE, TRUE);
		  InsertOption (pE, &pC, pDoc);
		  /* traite les attributs requis */
		  AttachMandatoryAttributes (pE, pDoc);
		  Tableau_CreeLigneSimple (pE, pDoc);
		  pE = pE->ElNext;
	       }
	     while (!(pE == NULL));
	  }
     }
}


/* TbCreation   verifie si l'element pointe par pEl est une colonne ou */
/* une ligne de table et dans ce cas cree toutes les cellules de cette */
/* colonne ou de cette ligne. */
/* Si l'element pEl n'est pas un element terminal cherche dans le sous arbre */
/* si par hasard il ne striuve pas un element tableau */
/* Ce cas se produit quand on enchaine la creation d'un grand */
/* nombre d'elements */

#ifdef __STDC__
static void                Tableau_Creation (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_Creation (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE;

   if (pEl != NULL)
      if (TypeHasException (EXC_TR_Tableau_CREATION, pEl->ElTypeNumber, pEl->ElSructSchema))
	 Tableau_Creation_DoIt (pEl, pDoc);
      else
	{
	   /* On cherche un eventuel tableau dans la descendance de pEl */
	   if (!pEl->ElTerminal)
	     {
		pE = pEl->ElFirstChild;
		while (pE != NULL)
		  {
		     Tableau_Creation (pE, pDoc);
		     pE = pE->ElNext;
		  }
	     }
	}
}

/* SelectColSimple      selectionne toutes les cellules correspondant */
/* a la colonne simple pointe' par pEl. */

#ifdef __STDC__
static void                Tableau_SelectColSimple (PtrElement pEl)

#else  /* __STDC__ */
static void                Tableau_SelectColSimple (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pLigne, pLigneSuiv, pE, pLesLignes, pCell;
   PtrAttribute         pAttr;
   int                 NType, attr;

   if (pEl != NULL)
     {
	/* remonte a l'element En_Tetes */
	NType = GetElemWithException (EXC_ID_En_Tetes, pEl->ElSructSchema);
	pE = pEl->ElParent;
	while (pE->ElTypeNumber != NType)
	   pE = pE->ElParent;
	/* l'element apres En_Tetes est Les_lignes */
	pLesLignes = pE->ElNext;
	/* numero du type d'element Ligne_simple */
	NType = GetElemWithException (EXC_ID_Ligne_Simple, pEl->ElSructSchema);
	/* numero de l'attribut Ref_colonne */
	attr = GetAttrWithException (EXC_ID_Ref_Colonne, pEl->ElSructSchema);
	/* cherche la premiere ligne du tableau */
	pLigne = FwdSearchTypedElem (pEl, NType, pEl->ElSructSchema);
	if (pLigne != NULL)
	   if (!ElemIsWithinSubtree (pLigne, pLesLignes))
	      /* la ligne trouvee ne fait pas partie du meme tableau */
	      pLigne = NULL;
	/* parcourt toutes les lignes simples du tableau, dans l'ordre */

	while (pLigne != NULL)
	  {
	     /* on commence par chercher la ligne suivante */
	     pLigneSuiv = FwdSearchTypedElem (pLigne, NType, pEl->ElSructSchema);
	     if (pLigneSuiv != NULL)
		if (!ElemIsWithinSubtree (pLigneSuiv, pLesLignes))
		   /* la ligne trouvee ne fait pas partie du meme tableau */
		   pLigneSuiv = NULL;

	     /* on passe a la premiere cellule de la ligne */
	     pCell = NULL;
	     if (pLigne->ElFirstChild != NULL)
		if (pLigne->ElFirstChild->ElNext != NULL)
		   pCell = pLigne->ElFirstChild->ElNext->ElFirstChild;
	     /* cherche dans cette ligne la cellule qui reference cette */
	     /* colonne simple */
	     while (pCell != NULL)
	       {
		  pAttr = pCell->ElFirstAttr;
		  /* cherche l'attribut La_colonne de la cellule */
		  while (pAttr != NULL)
		     if (pAttr->AeAttrSSchema == pEl->ElSructSchema && pAttr->AeAttrNum == attr)
			/* C'est l'attribut La_colonne */
		       {
			  if (pAttr->AeAttrReference != NULL)
			     if (pAttr->AeAttrReference->RdReferred != NULL)
				if (pAttr->AeAttrReference->RdReferred->ReReferredElem == pEl)
				  {
				     /* l'attribut La_colonne de cette cellule reference */
				     /* la colonne simple traitee, on selectionne cette cellule */
				     SelAjoute (pCell, (pLigneSuiv == NULL));
				     pCell = NULL;	/* arrete la recherche des cellules */
				  }
			  pAttr = NULL;		/* arrete la recherche des attributs */
		       }
		     else	/* passe a l'attribut suivant de la cellule */
			pAttr = pAttr->AeNext;
		  if (pCell != NULL)	/* passe a la cellule suivante de la ligne */
		     pCell = pCell->ElNext;
	       }
	     /* passe a la ligne suivante */
	     pLigne = pLigneSuiv;
	  }
     }
}

/* Tableau_Selection    verifie si l'element pointe par pEl est une colonne */
/* de table et dans ce cas selectionne toutes les cellules de cette */
/* colonne. Si Ext est vrai, il s'agit d'une extension de selection. */
/* Retourne vrai si la selection est correcte, faux s'il s'agit d'une */
/* extension de selection invalide (on ne selectionne pas plus d'une */
/* colonne a la fois). */

#ifdef __STDC__
static void         Tableau_Selection(PtrElement pEl, PtrDocument pDoc, boolean Ext, boolean *ret)

#else  /* __STDC__ */
static void         Tableau_Selection(pEl, pDoc, Ext, ret)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             Ext;
boolean             *ret;

#endif /* __STDC__ */

{
   PtrElement          pE;

   pE = NULL;
   *ret = TRUE;
   if (Ext)
     {
	/* c'est une extension de la selection */
	if (SelPremier != NULL)
	  {
	     /* il y a deja une selection */
	     pEl = SelPremier;
	     if (TypeHasException (EXC_TR_Tableau_SELECT, pEl->ElTypeNumber, pEl->ElSructSchema))
		/* Deja une colonne selectionnee, on refuse l'extension */
		*ret = FALSE;
	  }
	if (TypeHasException (EXC_TR_Tableau_SELECT, pEl->ElTypeNumber, pEl->ElSructSchema))
	   /* l'element a ajouter dans la selection est une colonne, refus */
	   *ret = FALSE;
     }

   else
     {
	/* c'est une nouvelle  selection */
	if (TypeHasException (EXC_TR_Tableau_SELECT, pEl->ElTypeNumber, pEl->ElSructSchema))
	   /* il s'agit bien d'un element demandant une selection speciale */

	   if (TypeHasException (EXC_ID_Colonne_Simple, pEl->ElTypeNumber, pEl->ElSructSchema))
	     {
		/* selection d'un element Colonne_simple */
		Tableau_SelectColSimple (pEl);
		SelDernier = pEl;
	     }

	   else
	     {
		if (TypeHasException (EXC_ID_Colonne_Composee, pEl->ElTypeNumber, pEl->ElSructSchema))
		   /* selection d'un element Colonne_composee */
		  {
		     if (pEl->ElFirstChild != NULL)
			if (pEl->ElFirstChild->ElNext != NULL)
			   /* descend a la premiere sous-colonne */
			   pE = pEl->ElFirstChild->ElNext->ElFirstChild;
		  }
		else if (TypeHasException (EXC_ID_Sous_Colonnes, pEl->ElTypeNumber, pEl->ElSructSchema))
		   /* selection d'un element Sous Colonne */
		   pE = pEl->ElFirstChild;

		else if (TypeHasException (EXC_ID_Les_Colonnes, pEl->ElTypeNumber, pEl->ElSructSchema))
		   /* selection d'un element Les Colonne */
		   pE = pEl->ElFirstChild;

		/* traite toutes les sous-colonnes */
		while (pE != NULL && ret)
		  {
		     if (TypeHasException (EXC_ID_Colonne_Simple, pE->ElTypeNumber, pE->ElSructSchema))
			/* c'est une colonne simple */
			Tableau_SelectColSimple (pE);
		     else
			Tableau_Selection (pE, pDoc, Ext, ret);
		     pE = pE->ElNext;
		  }
		SelDernier = pEl;
	     }
     }
}

/* TbDernierSauve       Verifie si le premier element du buffer */
/* copier-couper-coller, qui est pointe' par pElSv, est un element */
/* colonne de tableau et repond vrai si oui. Dans ce cas, cet element */
/* est egalement considere comme le dernier du buffer, meme s'il est */
/* suivi de cellules, qui sont considerees comme lui appartenant. */

#ifdef __STDC__
static void        Tableau_DernierSauve(PtrElement pElSv, boolean *ret)

#else  /* __STDC__ */
static void         Tableau_DernierSauve(pElSv, ret)
PtrElement          pElSv;
boolean             *ret;

#endif /* __STDC__ */

{

   /* est-ce un element Colonne_simple ? */
   *ret = TypeHasException (EXC_ID_Colonne_Simple, pElSv->ElTypeNumber, pElSv->ElSructSchema);

   if ((*ret))			/* est-ce un element Colonne_composee ? */
      *ret = TypeHasException (EXC_ID_Colonne_Composee, pElSv->ElTypeNumber, pElSv->ElSructSchema);
}


/* ColleCellules        Colle les NbCell premieres cellules de la suite de */
/* cellules pointee par pSvCell dans les lignes simples du tableau */
/* auquel appartient l'element Colonne_simple pointe' par pCol. Dans */
/* chaque ligne simple, la cellule est collee apres la cellule de rang */
/* NbPreced. */
/* Si NbCell est inferieur au nombre de lignes du tableau, la colonne */
/* pCol est completee avec des cellules vides. */
/* Au retour, pSvCell pointe la cellule qui suit la derniere cellule */
/* collee. */

#ifdef __STDC__
static void                Tableau_ColleCellules (PtrElement pCol, int NbPreced, int NbCell, PtrElement * pSvCell, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_ColleCellules (pCol, NbPreced, NbCell, pSvCell, pDoc)
PtrElement          pCol;
int                 NbPreced;
int                 NbCell;
PtrElement         *pSvCell;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pLesLignes, pLigne, pCell, pNCell, pPere;
   int                 TypeLigne, TypeCell, i, NbCellCollees;


   NbCellCollees = 0;
   TypeCell = GetElemWithException (EXC_ID_Cellule, pCol->ElSructSchema);	/* numero du type Cellule */
   TypeLigne = GetElemWithException (EXC_ID_Les_Lignes, pCol->ElSructSchema);	/* numero du type Les_lignes */
   pLesLignes = FwdSearchTypedElem (pCol, TypeLigne, pCol->ElSructSchema);		/* cherche l'element Les_lignes */
   TypeLigne = GetElemWithException (EXC_ID_Ligne_Simple, pCol->ElSructSchema);	/* numero du type Ligne_simple */

   /* parcourt les lignes simples du tableau */
   pLigne = pLesLignes;
   while (pLigne != NULL)
     {
	pLigne = FwdSearchTypedElem (pLigne, TypeLigne, pCol->ElSructSchema);
	if (pLigne != NULL)
	   if (!ElemIsWithinSubtree (pLigne, pLesLignes))	/* la ligne trouvee ne fait pas partie du tableau */
	      pLigne = NULL;
	   else
	     {
		/* on a trouve' une ligne simple appartenant au meme tableau,
		   on passe a la premiere cellule de la ligne */
		pPere = pLigne->ElFirstChild->ElNext;
		pCell = pPere->ElFirstChild;
		if (NbCellCollees < NbCell)
		  {
		     /* on cree une copie de la cellule a coller */
		     pNCell = CopyTree (*pSvCell, DocDeSauve, pCol->ElAssocNum,
				pCol->ElSructSchema, pDoc, pPere, TRUE, TRUE);
		     NbCellCollees++;
		     *pSvCell = (*pSvCell)->ElNext;
		  }
		else
		  {
		     /* on a deja colle' le nombre de cellules voulu, on cree une
		        cellule vide pour completer la colonne */
		     pNCell = NewSubtree (TypeCell, pPere->ElSructSchema, pDoc,
				 pPere->ElAssocNum, TRUE, TRUE, TRUE,
					  TRUE);
		     /* on met les attributs a la nouvelle cellule */
		     Tableau_MetAttrCellule (pNCell, pCol, pDoc);
		  }

		/* on insere la nouvelle cellule dans l'arbre */
		if (pCell == NULL)
		   InsertFirstChild (pPere, pNCell);
		else if (NbPreced == 0)
		   InsertElementBefore (pCell, pNCell);
		else
		  {
		     /* on saute les premieres cellules */
		     for (i = 1; i <= NbPreced - 1; i++)
			pCell = pCell->ElNext;
		     InsertElementAfter (pCell, pNCell);
		  }
		/* traite les attributs requis */
		AttachMandatoryAttributes (pNCell, pDoc);
		NbNouveaux++;
		Nouveaux[NbNouveaux - 1] = pNCell;
	     }
     }

   /* a-t-on colle le nombre voulu de cellules ? */
   if (NbCellCollees < NbCell)
      /* non, on saute les cellules qui n'ont pas ete collees parce que */
      /* le tableau ou on colle a moins de lignes que le tableau d'ou */
      /* viennent les cellules a coller */
      for (i = NbCellCollees + 1; i <= NbCell; i++)
	 *pSvCell = (*pSvCell)->ElNext;
}


/* VerifColCell verifie que les Colonnes_simples contenues dans */
/* l'element pointe' par pCol ont chacune une cellule a partir de celle */
/* pointee par pCell. */
/* pCellPrec pointe sur la cellule precedant pCell. */
/* Au retour, pCellPrec pointe sur la derniere cellule traitee et */
/* pCell sur la prochaine a traiter. */

#ifdef __STDC__
static void                Tableau_VerifColCell (PtrElement pCol, PtrElement * pCell, PtrElement * pCellPrec, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_VerifColCell (pCol, pCell, pCellPrec, pDoc)
PtrElement          pCol;
PtrElement         *pCell;
PtrElement         *pCellPrec;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pC;
   int                 TypeColSimple;
   PtrElement          pEl1;


   TypeColSimple = GetElemWithException (EXC_ID_Colonne_Simple, pCol->ElSructSchema);
   if (pCol->ElTypeNumber == TypeColSimple)
     {
	/* c'est une colonne simple */
	if (*pCell == NULL && *pCellPrec != NULL)
	   /* il n'y a pas de cellule pour cette colonne */
	  {
	     /* cree une cellule */
	     pEl1 = *pCellPrec;
	     *pCell = NewSubtree (pEl1->ElTypeNumber, pEl1->ElSructSchema, pDoc,
			   pEl1->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	     /* insere cette cellule a la suite de la cellule precedente */
	     InsertElementAfter (*pCellPrec, *pCell);
	  }
	if (*pCell != NULL)
	  {
	     Tableau_MetAttrCellule (*pCell, pCol, pDoc);	/* verifie les attributs de la cellule */
	     /* traite les attributs requis */
	     AttachMandatoryAttributes (*pCell, pDoc);
	     *pCellPrec = *pCell;
	     *pCell = (*pCell)->ElNext;	/* passe a la cellule suivante */
	  }
     }

   else
     {
	/* c'est une colonne composee, traite toutes les sous-colonnes */
	pC = pCol->ElFirstChild;
	while (pC->ElNext != NULL)
	   /* cherche l'element Sous_colonnes */
	   pC = pC->ElNext;
	pC = pC->ElFirstChild;	/* 1ere sous-colonne */
	while (pC != NULL)
	   /* traite toutes les sous-colonnes */
	  {
	     Tableau_VerifColCell (pC, pCell, pCellPrec, pDoc);
	     pC = pC->ElNext;
	  }
     }
}


/* VerifieLigne traite toutes les lignes simples contenues dans */
/* l'element pointe' par pLigne */

#ifdef __STDC__
static void                Tableau_VerifieLigne (PtrElement pLigne, PtrElement pLesCol, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_VerifieLigne (pLigne, pLesCol, pDoc)
PtrElement          pLigne;
PtrElement          pLesCol;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pL, pCell, pCellPrec, pCellSuiv, pCol, pTitreL;
   int                 TypeLigneSimple;

   /* accede au titre de la ligne */
   pTitreL = pLigne->ElFirstChild;
   if (TypeHasException (EXC_ID_Titre_Ligne, pTitreL->ElTypeNumber, pTitreL->ElSructSchema)
       || TypeHasException (EXC_ID_Titre_Ligne_Comp, pTitreL->ElTypeNumber, pTitreL->ElSructSchema))
      /* c'est bien un element Titre_ligne ou Titre_ligne_comp */
      Tableau_MetAttrTitreLigne (pTitreL, pDoc);

   TypeLigneSimple = GetElemWithException (EXC_ID_Ligne_Simple, pLigne->ElSructSchema);
   if (pLigne->ElTypeNumber == TypeLigneSimple)	/* c'est une ligne simple on la traite */
     {
	pCell = pLigne->ElFirstChild;	/* accede a la premiere cellule */
	while (pCell->ElNext != NULL)
	   pCell = pCell->ElNext;
	pCell = pCell->ElFirstChild;	/* pCell pointe sur l'element Contenu_ligne */
	/* pCell: premiere cellule */
	/* parcourt toutes les colonne du tableau */
	pCol = pLesCol->ElFirstChild;
	pCellPrec = NULL;
	while (pCol != NULL)
	  {
	     Tableau_VerifColCell (pCol, &pCell, &pCellPrec, pDoc);
	     pCol = pCol->ElNext;
	  }
	while (pCell != NULL)
	  {
	     /* s'il y a trop de cellules dans la ligne, on les detruit */
	     pCellSuiv = pCell->ElNext;
	     RemoveElement (pCell);
	     DeleteElement (&pCell);
	     pCell = pCellSuiv;
	  }
     }

   else
      /* c'est une ligne composee passe a l'element Sous_lignes */
     {
	pL = pLigne->ElFirstChild;
	while (pL->ElNext != NULL)
	   pL = pL->ElNext;
	/* traite tous les elements contenus dans l'element Sous_lignes */
	pL = pL->ElFirstChild;
	while (pL != NULL)
	  {
	     Tableau_VerifieLigne (pL, pLesCol, pDoc);
	     pL = pL->ElNext;
	  }
     }
}


/* TbColleVoisin        effectue le traitement particulier a l'operation */
/* Coller lorsqu'elle s'applique a une colonne ou une ligne de tableau. */
/* pColle pointe sur l'element colle' et pElSv sur l'element */
/* a coller ensuite. */

#ifdef __STDC__
static void                Tableau_ColleVoisin (PtrElement pColle, PtrElement * pElSv, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_ColleVoisin (pColle, pElSv, pDoc)
PtrElement          pColle;
PtrElement         *pElSv;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pCellule, pColSimple;
   int                 NbColPreced, NbTotalCell, NbCellParCol, NbColCollees;
   int                 col;
   int                 TypeColSimple, TypeColComposee, NType;


   if (TypeHasException (EXC_TR_Tableau_COLLER, pColle->ElTypeNumber, pColle->ElSructSchema))
      /* Il s'agit d'un element de tableau devant faire l'objet d'un */
      /* traitement special pour la commande Coller (voir Tableau.SCH). */
     {
	TypeColSimple = GetElemWithException (EXC_ID_Colonne_Simple, pColle->ElSructSchema);
	TypeColComposee = GetElemWithException (EXC_ID_Colonne_Composee, pColle->ElSructSchema);

	if (pColle->ElTypeNumber == TypeColSimple || pColle->ElTypeNumber == TypeColComposee)
	  {
	     /* c'est un element Colonne_simple ou Colonne_composee qui a ete colle' */
	     /* remonte a l'element Les_colonnes */
	     NType = GetElemWithException (EXC_ID_Les_Colonnes, pColle->ElSructSchema);
	     pE = pColle->ElParent;
	     while (pE->ElTypeNumber != NType)
		pE = pE->ElParent;

	     /* compte le nombre de colonnes simples qui precedent l'element */
	     /* colle': NbColPreced. */
	     NbColPreced = 0;
	     pColSimple = pE;
	     do
	       {
		  pColSimple = FwdSearchTypedElem (pColSimple, TypeColSimple, pColle->ElSructSchema);
		  if (ElemIsWithinSubtree (pColSimple, pColle))
		     pColSimple = NULL;
		  else		/* on trouve' une colonne simple */
		     NbColPreced++;
	       }
	     while (!(pColSimple == NULL));

	     /* compte les cellules qui sont dans le buffer Couper-Coller */
	     pCellule = *pElSv;
	     NbTotalCell = 0;
	     while (pCellule != NULL)
	       {
		  NbTotalCell++;
		  pCellule = pCellule->ElNext;
	       }

	     /* a priori, c'est un element Colonne_simple qui a ete colle' */
	     NbColCollees = 1;
	     pColSimple = pColle;
	     if (TypeHasException (EXC_ID_Colonne_Composee, pColle->ElTypeNumber, pColle->ElSructSchema))
		/* c'est un element Colonne_composee */
		/* compte les elements Colonne_simple contenus dans l'element colle': NbColCollees. */
	       {
		  NbColCollees = 0;
		  pColSimple = NULL;
		  pE = pColle;
		  while (pE != NULL)
		    {
		       pE = FwdSearchTypedElem (pE, TypeColSimple, pColle->ElSructSchema);
		       if (pE != NULL)	/* on a trouve' une colonne simple */
			  if (ElemIsWithinSubtree (pE, pColle))
			    {
			       NbColCollees++;
			       if (pColSimple == NULL)
				  pColSimple = pE;	/* 1ere colonne simple collee */
			    }
			  else
			     pE = NULL;
		    }
	       }

	     /* calcule le nombre de cellules par colonne */
	     NbCellParCol = NbTotalCell / NbColCollees;
	     /* colle les cellules correspondant a chacune des colonnes */
	     /* simples collees. */
	     for (col = 1; col <= NbColCollees; col++)
	       {
		  Tableau_ColleCellules (pColSimple, NbColPreced, NbCellParCol, pElSv, pDoc);
		  NbColPreced++;
		  pColSimple = FwdSearchTypedElem (pColSimple, TypeColSimple, pColle->ElSructSchema);
	       }
	     *pElSv = NULL;	/* empeche ColleVoisin de coller les cellules */
	  }

	else
	   /* ce n'est ni un element Colonne_simple ni un element
	      Colonne_composee qui a ete colle' */
	   if (TypeHasException (EXC_ID_Ligne_Simple, pColle->ElTypeNumber, pColle->ElSructSchema)
	       || TypeHasException (EXC_ID_Ligne_Composee, pColle->ElTypeNumber, pColle->ElSructSchema))
	   /* c'est un element Ligne_simple ou Ligne_composee qui a ete colle' */
	   /* Pour chaque ligne simple collee, on verifie que chaque */
	   /* cellule a bien un attribut La_colonne (si elle ne l'a pas, */
	   /* on lui met) et que le nombre de cellules est egal au nombre */
	   /* de colonnes simples du tableau (si non, on supprime les */
	   /* cellules excedentaires ou on cree les cellules manquantes. */
	   /* remonte d'abord a l'element Les_lignes */
	  {
	     NType = GetElemWithException (EXC_ID_Les_Lignes, pColle->ElSructSchema);
	     pE = pColle->ElParent;
	     while (pE->ElTypeNumber != NType)
		pE = pE->ElParent;
	     /* retrouve l'element Les_colonnes */
	     pE = pE->ElPrevious->ElFirstChild->ElNext;
	     if (TypeHasException (EXC_ID_Les_Colonnes, pE->ElTypeNumber, pE->ElSructSchema))
		/* c'est bien Les_colonnes */
		Tableau_VerifieLigne (pColle, pE, pDoc);
	  }
     }
}

/* Exc_page_break_couper est appele' par CmdCut qui effectue le traitement */
/* de la commande Couper. */
/* PremSel et DerSel pointent le premier et le dernier element */
/* selectionne's, qui doivent etre coupe's. */
/* S'il s'agit d'un seul et meme element saut de page qui se trouve */
/* dans une structure demandant un traitemenrt special des sauts de */
/* pages, on etend la selection a l'element portant l'exception */
/* PageBreakRepBefore qui precede ce saut de page et a l'element */
/* portant l'exception PageBreakRepetition qui */
/* suit, pour que CmdCut coupe les 3 elements a la fois. */
/* Dans ce cas, on met Sauve a Faux (on ne sauvera pas les elements */
/* coupe's dans le buffer Couper-Copier-Coller) et DetruirePage a */
/* Vrai (on detruira le saut de page bien qu'il ne soit plus le seul */
/* selectionne'). */

#ifdef __STDC__
static void                Exc_Page_Break_Couper (PtrElement * PremSel, PtrElement * DerSel, PtrDocument pDoc, boolean * Sauve, boolean * DetruirePage)

#else  /* __STDC__ */
static void                Exc_Page_Break_Couper (PremSel, DerSel, pDoc, Sauve, DetruirePage)
PtrElement         *PremSel;
PtrElement         *DerSel;
PtrDocument         pDoc;
boolean            *Sauve;
boolean            *DetruirePage;

#endif /* __STDC__ */

{
   PtrElement          Prec, Suiv;
   boolean             stop;

   if (*PremSel == *DerSel)
      /* un seul element est selectionne' */
      if ((*PremSel)->ElTerminal)
	 /* c'est une feuille */
	 if ((*PremSel)->ElLeafType == LtPageColBreak)
	    /* c'est une marque de page */
	   {
	      /* les precedents peuvent etre des elements repete's */
	      Prec = (*PremSel)->ElPrevious;
	      stop = FALSE;
	      while (!stop)
		{
		   if (Prec == NULL)
		      stop = TRUE;	/* pas d'autre element precedent */
		   else if (!TypeHasException (ExcPageBreakRepBefore, Prec->ElTypeNumber,
					     Prec->ElSructSchema))
		      /* l'element precedent n'est pas une repetition */
		      stop = TRUE;
		   else if (Prec->ElSource == NULL)
		      /* l'element precedent n'est pas une inclusion */
		      stop = TRUE;
		   else
		      /* il faut supprimer cet element precedent */
		     {
			*PremSel = Prec;
			*Sauve = FALSE;
			*DetruirePage = TRUE;
			/* passe au precedent */
			Prec = Prec->ElPrevious;
		     }
		}
	      /* les suivants peuvent etre des elements repetes */
	      Suiv = (*DerSel)->ElNext;
	      stop = FALSE;
	      while (!stop)
		{
		   if (Suiv == NULL)
		      stop = TRUE;	/* pas d'autre element suivant */
		   else if (!TypeHasException (ExcPageBreakRepetition, Suiv->ElTypeNumber,
					     Suiv->ElSructSchema))
		      /* l'element suivant n'est pas une repetition */
		      stop = TRUE;
		   else if (Suiv->ElSource == NULL)
		      /* l'element suivant n'est pas une inclusion */
		      stop = TRUE;
		   else
		      /* il faut supprimer cet element suivant */
		     {
			*DerSel = Suiv;
			*Sauve = FALSE;
			*DetruirePage = TRUE;
			Suiv = Suiv->ElNext;
		     }
		}
	   }
}


/* Tableau_Couper       Si l'element pointe' par pBasTableau est */
/* effectivement un Bas_tableau, on reapplique les regles de hauteur */
/* des filets verticaux engendre's par l'En_Tete qui precede */

#ifdef __STDC__
static void                Tableau_Couper (PtrElement pBasTableau, PtrDocument pDoc)

#else  /* __STDC__ */
static void                Tableau_Couper (pBasTableau, pDoc)
PtrElement          pBasTableau;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 SauveType;

   if (TypeHasException (EXC_ID_BasTableau, pBasTableau->ElTypeNumber,
		       pBasTableau->ElSructSchema))
      /* c'est un element Bas_tableau */
     {
	/* reapplique les regles de hauteur des filets */
	/* verticaux engendre's par l'En_Tete qui precede */
	/* On camoufle d'abord l'element Bas_tableau */
	/* pour qu'il ne soit pas pris en compte */
	SauveType = pBasTableau->ElTypeNumber;
	pBasTableau->ElTypeNumber = 1;
	HauteurFilets (pBasTableau, pDoc);
	pBasTableau->ElTypeNumber = SauveType;
     }
}



/* TableauVerifExtension        verifie si l'attribut pAttr qui doit etre */
/* applique' a` tous les elements compris entre PremEl et DerEl */
/* est un attribut Extension_horiz ou Extension_vert. */
/* Dans ce cas, verifie si cet attribut designe un element correct */
/* et si ce n'est pas le cas, annule l'attribut ou le supprime si */
/* SupprimeAttr est Vrai. */


#ifdef __STDC__
static void                TableauVerifExtension (PtrAttribute pAttr, PtrElement PremEl, PtrElement DerEl, boolean SupprimeAttr)

#else  /* __STDC__ */
static void                TableauVerifExtension (pAttr, PremEl, DerEl, SupprimeAttr)
PtrAttribute         pAttr;
PtrElement          PremEl;
PtrElement          DerEl;
boolean             SupprimeAttr;

#endif /* __STDC__ */

{
   PtrElement          pEl, pElRef;
   DocumentIdentifier     IdentDoc;
   PtrDocument         pDoc;
   boolean             erreur, verif, stop;

   erreur = FALSE;
   verif = FALSE;
   /* accede a l'element designe' par l'attribut */
   pElRef = ReferredElement (pAttr->AeAttrReference, &IdentDoc, &pDoc);
   if (pElRef != NULL)
      if (AttrHasException (EXC_ID_Extens_Vertic, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	 /* c'est l'attribut Debordement_vert */
	 verif = TRUE;
      else if (AttrHasException (EXC_ID_Extens_Horiz, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	 /* c'est l'attribut Debordement_horiz */
	 if (PremEl != DerEl)
	    /* l'attribut doit s'appliquer a plusieurs elements, erreur */
	    erreur = TRUE;
	 else
	   {
	      if (PremEl == pElRef)
		 /* la cellule s'etend sur elle-meme, erreur */
		 erreur = TRUE;
	      else if (PremEl->ElParent != pElRef->ElParent)
		 /* les deux cellules n'appartiennent pas a la meme ligne */
		 erreur = TRUE;
	      else
		 verif = TRUE;
	   }
   if (verif)
      /* verifie que les elements qui doivent porter l'attribut sont */
      /* tous des Cellules de tableau et qu'ils sont avant l'element */
      /* designe' par l'attribut */
     {
	/* accede a l'element designe' par l'attribut */
	pElRef = ReferredElement (pAttr->AeAttrReference, &IdentDoc, &pDoc);
	pEl = PremEl;
	/* parcourt les elements auxquels il faut appliquer l'attribut */
	while (pEl != NULL && !erreur)
	  {
	     if (!TypeHasException (EXC_ID_Cellule, pEl->ElTypeNumber, pEl->ElSructSchema))
		/* l'element n'est pas une cellule, erreur */
		erreur = TRUE;
	     else if (!ElemIsBefore (pEl, pElRef))
		/* l'element ne precede pas l'element designe', erreur */
		erreur = TRUE;
	     if (!erreur)
		pEl = SelSuivant (pEl, DerEl);	/* element suivant */
	  }
	if (!erreur)
	   /* verifie qu'il s'agit bien du meme tableau */
	  {
	     pEl = PremEl;
	     /* remonte a la racine du tableau */
	     stop = FALSE;
	     while (!stop && pEl != NULL)
	       {
		  if (TypeHasException (EXC_ID_Tableau, pEl->ElTypeNumber, pEl->ElSructSchema))
		     stop = TRUE;	/* trouve' */
		  else
		     pEl = pEl->ElParent;
	       }
	     if (pEl == NULL)
		erreur = TRUE;	/* pas d'ascendant de type Table */
	     else if (!ElemIsAnAncestor (pEl, pElRef))
		/* l'element reference' n'est pas dans le meme tableau */
		erreur = TRUE;
	  }
     }
   if (erreur)
      /* il y a erreur, on annule ou supprime l'attribut */
      if (SupprimeAttr)
	 DeleteAttribute (PremEl, pAttr);
      else
	 DeleteReference (pAttr->AeAttrReference);
}

/** debut ajout */
/* ---------------------------------------------------------------------- */
/* |    Tableau_CanHolophrast   indique si on peut holophraster         | */
/* |            l'element pointe' par pEl.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void Tableau_CanHolophrast(PtrElement pEl, boolean *ret)

#else  /* __STDC__ */
static void Tableau_CanHolophrast(pEl, ret)
PtrElement          pEl;
boolean *ret;

#endif /* __STDC__ */

{

   *ret = TRUE;
   /* on n'holophraste pas les entetes de tableau */
   if (TypeHasException (EXC_ID_En_Tetes, pEl->ElTypeNumber, pEl->ElSructSchema))
      *ret = FALSE;
   /* on n'holophraste pas les colonnes composees */
   else if (TypeHasException (EXC_ID_Colonne_Composee, pEl->ElTypeNumber,
			    pEl->ElSructSchema))
      *ret = FALSE;
   /* on n'holophraste pas les colonnes */
   else if (TypeHasException (EXC_ID_Les_Colonnes, pEl->ElTypeNumber,
			    pEl->ElSructSchema))
      *ret = FALSE;
   /* on n'holophraste pas les colonnes simples */
   else if (TypeHasException (EXC_ID_Colonne_Simple, pEl->ElTypeNumber,
			    pEl->ElSructSchema))
      *ret = FALSE;
   /* on n'holophraste pas les sous-colonnes */
   else if (TypeHasException (EXC_ID_Sous_Colonnes, pEl->ElTypeNumber,
			    pEl->ElSructSchema))
      *ret = FALSE;
}
/** fin ajout */

/* ---------------------------------------------------------------------- */
/* |	Tableau_Except	indique si l'element pEl porte une exception  	| */
/* |		tableau EXC_ID_Tableau.			  	        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void Tableau_Except(PtrElement pEl, boolean *ret)
#else /* __STDC__ */
static void Tableau_Except(pEl, ret)
	PtrElement pEl;
        boolean *ret;
#endif /* __STDC__ */
{
       *ret = (TypeHasException(EXC_ID_Tableau, pEl->ElTypeNumber,
			  (pEl->ElSructSchema)));
}

/* ---------------------------------------------------------------------- */
/* |	Tableau_Except_CR	indique si l'element pEl porte une exception  	| */
/* |		tableau EXC_ID_Tableau_CREATION.	    	  	        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void Tableau_Except_CR(PtrElement pEl, boolean *ret)
#else /* __STDC__ */
static void Tableau_Except_CR(pEl, ret)
	PtrElement pEl;
        boolean *ret;
#endif /* __STDC__ */
{
       *ret = (TypeHasException(EXC_TR_Tableau_CREATION, pEl->ElTypeNumber,
			  (pEl->ElSructSchema)));
}

/* ---------------------------------------------------------------------- */
/* |  TableauLoadResources : connecte les ressources de traitement des  | */
/* |  tableaux            	                                        | */
/* ---------------------------------------------------------------------- */
void TableauLoadResources()
{

  if (ThotLocalActions[T_createtable] == NULL)
    { 
      /* initialisations */

      /* connection des ressources */
      TteConnectAction(T_createtable, (Proc)Tableau_Creation);
      TteConnectAction(T_selecttable, (Proc)Tableau_Selection);
      TteConnectAction(T_singlecell, (Proc)Tableau_OrphanCell);
      TteConnectAction(T_attrtable, (Proc)TableauAttributSpecial);
      TteConnectAction(T_lastsaved, (Proc)Tableau_DernierSauve);
      TteConnectAction(T_condlast, (Proc)Tableau_Cond_Dernier);
      TteConnectAction(T_pastesiblingtable, (Proc)Tableau_ColleVoisin);
      TteConnectAction(T_refattr, (Proc)Tableau_AttributRef);
      TteConnectAction(T_ruleattr, (Proc)Tableau_ApplRegleAttribut);
      TteConnectAction(T_createhairline, (Proc)Tableau_CreeFiletLigne);
      TteConnectAction(T_holotable, (Proc)Tableau_CanHolophrast);
      TteConnectAction(T_checkextens, (Proc)TableauVerifExtension);
      TteConnectAction(T_abref, (Proc)Tableau_PaveRef);
      TteConnectAction(T_vertspan, (Proc)TableauDebordeVertical);
      TteConnectAction(T_excepttable, (Proc)Tableau_Except);
      TteConnectAction(T_entertable, (Proc)Tableau_Except_CR);
      TteConnectAction(T_insertpage, (Proc)Exc_Page_Break_Inserer);
      TteConnectAction(T_cutpage, (Proc)Exc_Page_Break_Couper);
      TteConnectAction(T_deletepage, (Proc)Exc_Page_Break_Supprime);
      TteConnectAction(T_deletepageab, (Proc)Exc_Page_Break_Detruit_Pave);
    }
}

/* End Of Module table */
