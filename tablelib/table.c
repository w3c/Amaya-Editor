/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

 /*
 * Warning:
 * This module is part of Thot, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */

 /*
 *
 * Authors: V. Quint (INRIA)
 *          C. Roisin (INRIA) 
 */

/*----------------------------------------------------------------------
      Traitements specifiques a la structure Tableau             
  ----------------------------------------------------------------------*/


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

/* variables importees */
#define THOT_EXPORT extern
#include "select_tv.h"
#include "creation_tv.h"
#include "modif_tv.h"
#include "appdialogue_tv.h"

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
#include "attrpresent_f.h"
#include "references_f.h"
#include "table2_f.h"


/* exceptions definies dans Tableau */
#include "exc_Table.h"

/* OrphanCell repond true si la cellule comporte
   un attribut reference a une colonne qui se trouve
   dans le buffer Couper-Coller. */

#ifdef __STDC__
static void         OrphanCell (PtrElement pCell, PtrElement pElSave, boolean * InCutBuffer)

#else  /* __STDC__ */
static void         OrphanCell (pCell, pElSave, InCutBuffer)
PtrElement          pCell;
PtrElement          pElSave;
boolean            *InCutBuffer;

#endif /* __STDC__ */

{
   int                 attr;
   PtrAttribute        pAttr;
   PtrElement          pCol, pE;

   *InCutBuffer = FALSE;
   /* cherche l'attribut La_Column de la cellule */
   pCol = NULL;
   attr = GetAttrWithException (EXC_ID_Ref_Column, pCell->ElStructSchema);
   pAttr = pCell->ElFirstAttr;
   while (pAttr != NULL)
      if (pAttr->AeAttrSSchema == pCell->ElStructSchema && pAttr->AeAttrNum == attr)
	 /* C'est l'attribut La_Column */
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
	/* La Column trouvee est-elle dans le buffer couper-coller ?
	   Pour cela on regarde si elle est soeur de pElSave (debut de la chaine 
	   des elts sauves)  ou si un des elts freres de pElSave la contient */

	for (pE = pElSave; pE != NULL; pE = pE->ElNext)
	   if (ElemIsWithinSubtree (pCol, pE))
	     {
		*InCutBuffer = TRUE;
		break;
	     }
     }

}




/* DistribAlignHorizVertic      met un attribut Alignement horizontal ou Vertical */
/* sur toutes les elements portant une exception EXC_TR_Cell_ATTRIBUT */

#ifdef __STDC__
static void         DistribAlignHorizVertic (PtrElement pEl, PtrAttribute pAttrCell, PtrDocument SelDoc)

#else  /* __STDC__ */
static void         DistribAlignHorizVertic (pEl, pAttrCell, SelDoc)
PtrElement          pEl;
PtrAttribute        pAttrCell;
PtrDocument         SelDoc;

#endif /* __STDC__ */

{
   PtrElement          pCell;
   PtrAttribute        pAttr;
   boolean             NoExtension;

   if (TypeHasException (EXC_TR_Cell_ATTRIBUT, pEl->ElTypeNumber, pEl->ElStructSchema))
     {
	/* Les attributs d'alignement vertical et d'extension verticale */
	/* des cellules sont exclusifs */
	NoExtension = TRUE;
	if (pAttrCell->AeAttrNum == GetAttrWithException (EXC_ID_Align_Vertic, pEl->ElStructSchema))
	   /* on veut mettre un attribut d'alignement vertical */
	  {
	     pAttr = pEl->ElFirstAttr;
	     while (pAttr != NULL && NoExtension)
		if (AttrHasException (EXC_ID_Extens_Vertic, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
		   NoExtension = FALSE;
		else		/* passe a l'attribut suivant de la cellule */
		   pAttr = pAttr->AeNext;
	  }
	if (NoExtension)
	   /* l'element n'a pas d'attribut d'extension verticale */
	   AttachAttrWithValue (pEl, SelDoc, pAttrCell);
     }

   if (!pEl->ElTerminal)
     {
	pCell = pEl->ElFirstChild;
	while (pCell != NULL)
	  {
	     DistribAlignHorizVertic (pCell, pAttrCell, SelDoc);
	     pCell = pCell->ElNext;
	  }
     }
}

/* AttributSpecial traite : */
/* - le cas particulier de l'attribut Type_Table  */
/*   si cet attribut est mis ou retire du tableau on recree l'image abstraite du tableau */
/* - le cas particulier de l'attribut Alignement horizontal  */
/* - le cas particulier de l'attribut Alignement vertical */

#ifdef __STDC__
static void         AttributSpecial (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc)

#else  /* __STDC__ */
static void         AttributSpecial (pEl, pAttr, pDoc)
PtrElement          pEl;
PtrAttribute        pAttr;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   AttributeBlock      AttrCell;
   PtrAttribute        pAttrCell;

   if (TypeHasException (EXC_ID_Table, pEl->ElTypeNumber, pEl->ElStructSchema)
       && pAttr->AeAttrNum == GetAttrWithException (EXC_ID_Type_Table, pEl->ElStructSchema))
     {
	DestroyAbsBoxes (pEl, pDoc, FALSE);
	AbstractImageUpdated (pDoc);
	CreateAllAbsBoxesOfEl (pEl, pDoc);
	AbstractImageUpdated (pDoc);
     }

   else if (AttrHasException (EXC_ID_Align_HorizDistrib, pAttr->AeAttrNum, pEl->ElStructSchema))
     {
	/* distribution sur les elements cellule inclus d'un attribut Alignement horizontal */
	pAttrCell = &AttrCell;
	AttrCell = *pAttr;
	AttrCell.AeAttrNum = GetAttrWithException (EXC_ID_Align_Horiz, pEl->ElStructSchema);
	DistribAlignHorizVertic (pEl, pAttrCell, pDoc);
     }

   else if (AttrHasException (EXC_ID_Align_VerticDistrib, pAttr->AeAttrNum, pEl->ElStructSchema))
     {
	/* distribution sur les elements cellule inclus d'un attribut Alignement Vertical */
	pAttrCell = &AttrCell;
	AttrCell = *pAttr;
	AttrCell.AeAttrNum = GetAttrWithException (EXC_ID_Align_Vertic, pEl->ElStructSchema);
	DistribAlignHorizVertic (pEl, pAttrCell, pDoc);
     }
}



/* CheckAttrRef        retourne vrai si l'attribut pointe' par pAttr est */
/* l'attribut Ref_largeur ou l'attribut La_Column. */

#ifdef __STDC__
static void         CheckAttrRef (PtrAttribute pAttr, boolean * ret)

#else  /* __STDC__ */
boolean             CheckAttrRef (pAttr, ret)
PtrAttribute        pAttr;
boolean            *ret;

#endif /* __STDC__ */

{

   *ret = FALSE;
   if (AttrHasException (EXC_ID_Ref_Title, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
      *ret = TRUE;		/* c'est l'attribut Ref_largeur */
   else if (AttrHasException (EXC_ID_Ref_Column, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
      *ret = TRUE;		/* c'est l'attribut La_Column */
}


/* SetAttrTitleRaw    met les attribut Ref_Title_Width sur l'element pointe' par */
/* pTitre et l'attribut Width_Percent avec une valeur qui depend du */
/* degre' d'imbrication des lignes composees. */

#ifdef __STDC__
static void         SetAttrTitleRaw (PtrElement pElTitle, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SetAttrTitleRaw (pElTitle, pDoc)
PtrElement          pElTitle;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pT, pElTableTitle;
   PtrAttribute        pAttr;
   int                 percent;

   /* cherche l'element Title_Table */
   pElTableTitle = NULL;
   pE = pElTitle;
   do				/* remonte a la racine du tableau */
      pE = pE->ElParent;
   while (!TypeHasException (EXC_ID_Table, pE->ElTypeNumber, pE->ElStructSchema));

   pElTableTitle = pE->ElFirstChild->ElFirstChild;
   if (pElTableTitle != NULL)
     {
	if (!TypeHasException (EXC_ID_Title_Table, pElTableTitle->ElTypeNumber, pE->ElStructSchema))
	  {
	     printf ("On ne trouve pas le Titre du Tableau.\n");
	     return;
	  }
	/* met l'attribut Ref_largeur sur l'element et le fait pointer sur */
	/* l'element Title_Table */
	pAttr = AttachAttrByExceptNum (EXC_ID_Ref_Title, pElTitle, pElTableTitle, pDoc);
     }
   /* met l'attribut Width_Percent sur l'element Titre */
   pAttr = AttachAttrByExceptNum (EXC_ID_Width_Percent, pElTitle, NULL, pDoc);
   /* compte le nombre de Lignes composees qui contiennent notre */
   /* Titre de ligne pour calculer la valeur de l'attribut */
   percent = 100;
   pE = pElTitle;
   while (!TypeHasException (EXC_ID_The_Raws, pE->ElTypeNumber, pE->ElStructSchema))
      /* on n'est pas encore arrive' a l'element The_Raws */
     {
	if (TypeHasException (EXC_ID_Compound_Raw, pE->ElTypeNumber, pE->ElStructSchema))
	   /* c'est une ligne composee */
	   percent = percent / 2;
	pE = pE->ElParent;
     }
   pAttr->AeAttrValue = percent;

   /* traite tous les titres de ligne imbriques */
   if (TypeHasException (EXC_ID_Title_Compound_Raw, pElTitle->ElTypeNumber, pElTitle->ElStructSchema))
      /* c'est un titre de ligne composee, il y a donc des titres de ligne imbriques */
     {
	pE = pElTitle->ElNext;	/* passe a l'element Sub_Raws */
	if (pE != NULL)
	  {
	     pE = pE->ElFirstChild;	/* passe a la premiere ligne imbriquee */
	     while (pE != NULL)	/* traite toutes les lignes imbriquees */
	       {
		  pT = pE->ElFirstChild;	/* passe au titre de la ligne imbriquee */
		  if (pT != NULL)
		    {
		       if (TypeHasException (EXC_ID_Title_Raw, pT->ElTypeNumber, pT->ElStructSchema) ||
			   TypeHasException (EXC_ID_Title_Compound_Raw, pT->ElTypeNumber, pT->ElStructSchema))
			  /* c'est bien un element Title_Raw ou Title_Compound_Raw, */
			  /* on verifie ses attributs */
			  SetAttrTitleRaw (pT, pDoc);
		    }
		  pE = pE->ElNext;
	       }
	  }
     }
}

/* SetRawAttribute     met l'attribut Filet_Gauche et Filet_Bas a l'element */

#ifdef __STDC__
static void         SetRawAttribute (PtrElement pLigne, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SetRawAttribute (pLigne, pDoc)
PtrElement          pLigne;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Foot_Hairline_SimpRaw, pLigne, NULL, pDoc);
   pAttr->AeAttrValue = 2;

   if (TypeHasException (EXC_ID_Compound_Raw, pLigne->ElTypeNumber, pLigne->ElStructSchema))
     {
	pAttr = AttachAttrByExceptNum (EXC_ID_Right_Hairline_CompRaw, pLigne, NULL, pDoc);
	pAttr->AeAttrValue = 2;
     }
}


/* SetAttrColTitle  met l'attribut a l'element */

#ifdef __STDC__
void                SetAttrColTitle (PtrElement pElTitleCol, PtrDocument pDoc)

#else  /* __STDC__ */
void                SetAttrColTitle (pElTitleCol, pDoc)
PtrElement          pElTitleCol;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
}


/* SetAttrSimpleCol met l'attribut Largeur a l'element */

#ifdef __STDC__
static void         SetAttrSimpleCol (PtrElement pCol, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SetAttrSimpleCol (pCol, pDoc)
PtrElement          pCol;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Column_Width, pCol, NULL, pDoc);
   pAttr->AeAttrValue = 50;
   pAttr = AttachAttrByExceptNum (EXC_ID_Right_Hairline_SimpCol, pCol, NULL, pDoc);
   pAttr->AeAttrValue = 2;
}


/* SetAttrHairlineCol      met l'attribut Filet_Haut a l'element si */
/* la Column est simple */

#ifdef __STDC__
static void         SetAttrHairlineCol (PtrElement pCol, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SetAttrHairlineCol (pCol, pDoc)
PtrElement          pCol;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Foot_Hairline_CompCol, pCol, NULL, pDoc);
   pAttr->AeAttrValue = 2;
   pAttr = AttachAttrByExceptNum (EXC_ID_Right_Hairline_SimpCol, pCol, NULL, pDoc);
   pAttr->AeAttrValue = 2;
}


/* SetAttrTableTitle met l'attribut Filet_Haut et Filet_Gauche */

#ifdef __STDC__
static void         SetAttrTableTitle (PtrElement pElTitleTab, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SetAttrTableTitle (pElTitleTab, pDoc)
PtrElement          pElTitleTab;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Title_Width, pElTitleTab, NULL, pDoc);
   pAttr->AeAttrValue = 70;
}


/* SetAttrColToCell   met l'attribut Ref_Column a l'element */
/* utilise pColumn comme valeur pour la reference */

#ifdef __STDC__
static void         SetAttrColToCell (PtrElement pCell, PtrElement pCol, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SetAttrColToCell (pCell, pCol, pDoc)
PtrElement          pCell;
PtrElement          pCol;
PtrDocument         pDoc;

#endif /* __STDC__ */

{

   AttachAttrByExceptNum (EXC_ID_Ref_Column, pCell, pCol, pDoc);
}


/*  SetAttrHairlineToCols met l'attribut Filet_Gauche */

#ifdef __STDC__
static void         SetAttrHairlineToCols (PtrElement pElCols, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SetAttrHairlineToCols (pElCols, pDoc)
PtrElement          pElCols;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Left_Hairline_Cols, pElCols, NULL, pDoc);
   pAttr->AeAttrValue = 2;
}


/* SetAttrHairlineToHead met l'attribut Hairline_Inferieur */

#ifdef __STDC__
static void         SetAttrHairlineToHead (PtrElement pElHead, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SetAttrHairlineToHead (pElHead, pDoc)
PtrElement          pElHead;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Foot_Hairline_Headings, pElHead, NULL, pDoc);
   pAttr->AeAttrValue = 2;
}

/* SetAttrFrameTable */

#ifdef __STDC__
static void         SetAttrFrameTable (PtrElement pTable, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SetAttrFrameTable (pTable, pDoc)
PtrElement          pTable;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr;

   pAttr = AttachAttrByExceptNum (EXC_ID_Table_Frame, pTable, NULL, pDoc);
   pAttr->AeAttrValue = 2;
}


/* CreateSimpleCol        On vient de creer l'element Simple_Column pointe' */
/* par pEl, dans le document pointe' par pDoc. Ajoute l'attribut */
/* Filet-vertical a cet element et cree une cellule dans chaque ligne */
/* simple du tableau. Associe a chaque cellule creee un attribut */
/* La_Column qui pointe sur la Column simple. */

#ifdef __STDC__
static void         CreateSimpleCol (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void         CreateSimpleCol (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pCol, pElRaw, pElTheRaws, pCell, pNCell;
   int                 NType, TypeCell;
   PtrAttribute        pAttr;
   PtrElement          pElRef;
   int                 attr;
   boolean             firstCol, attrFound;

   /* met les attributs a l'element Simple_Column */
   SetAttrSimpleCol (pEl, pDoc);
   /* met les attributs sur le titre de Column */
   if (pEl->ElFirstChild != NULL)
      SetAttrColTitle (pEl->ElFirstChild, pDoc);
   /* cree les paves de l'element */
   CreateAllAbsBoxesOfEl (pEl, pDoc);

   /* remonte a l'element En_Tetes */
   NType = GetElemWithException (EXC_ID_Headings, pEl->ElStructSchema);
   pE = pEl->ElParent;
   while (pE->ElTypeNumber != NType)
      pE = pE->ElParent;
   /* l'element apres En_Tetes est The_Raws */
   pElTheRaws = pE->ElNext;

   /* verifie s'il y a des Columns simples qui precedent la nouvelle */
   /* Column simple */
   pCol = FwdSearchTypedElem (pE, pEl->ElTypeNumber, pEl->ElStructSchema);
   firstCol = (pCol == pEl);

   /* numero du type d'element Ligne_simple */
   NType = GetElemWithException (EXC_ID_Simple_Raw, pEl->ElStructSchema);
   /* numero du type d'element Cellule */
   TypeCell = GetElemWithException (EXC_ID_Cell, pEl->ElStructSchema);

   /* Cherche toutes les lignes simples */
   pElRaw = pElTheRaws;
   while (pElRaw != NULL)
     {
	pElRaw = FwdSearchTypedElem (pElRaw, NType, pEl->ElStructSchema);
	if (pElRaw != NULL)
	   if (!ElemIsWithinSubtree (pElRaw, pElTheRaws))
	      /* la ligne trouvee ne fait pas partie du tableau */
	      pElRaw = NULL;
	   else
	     {
		/* on a trouve' une ligne simple du tableau */
		/* on passe a la premiere cellule de la ligne */
		pCell = pElRaw->ElFirstChild->ElNext->ElFirstChild;
		/* on cree une nouvelle cellule */
		pNCell = NewSubtree (TypeCell, pEl->ElStructSchema, pDoc,
				pElRaw->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
		/* on insere cette nouvelle cellule dans l'arbre */
		if (pCell == NULL)
		   /* la ligne n'avait pas encore de cellules */
		   InsertFirstChild (pElRaw->ElFirstChild->ElNext, pNCell);
		else if (firstCol)
		   /* on cree la 1ere Column, on insere la nouvelle cellule */
		   /* avant la 1ere cellule */
		   InsertElementBefore (pCell, pNCell);
		else
		   /* on saute les premieres cellules */
		  {
		     /* cherche d'abord le numero de l'attribut La_Column */
		     attr = GetAttrWithException (EXC_ID_Ref_Column, pCell->ElStructSchema);
		     /* on saute les cellules correspondant aux Columns simples */
		     /* precedant notre Column */
		     while (pCell != NULL)
		       {
			  /* Cherche d'abord l'attribut La_Column de la cellule courante */
			  pAttr = pCell->ElFirstAttr;
			  attrFound = FALSE;
			  while (!attrFound && pAttr != NULL)
			     if (pAttr->AeAttrNum == attr && pAttr->AeAttrSSchema == pCell->ElStructSchema)
				attrFound = TRUE;
			     else
				pAttr = pAttr->AeNext;
			  if (attrFound)
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
				     /* la tetiere de cette cellule est apres la nouvelle Column */
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
		/* on met l'attribut La_Column a la nouvelle cellule et le */
		/* fait pointer sur la nouvelle Column */
		SetAttrColToCell (pNCell, pEl, pDoc);
		/* traite les attributs requis */
		AttachMandatoryAttributes (pNCell, pDoc);
		CreateAllAbsBoxesOfEl (pNCell, pDoc);
	     }
     }
}


/* passe a la ligne imbriquee suivante */
/* CreateSimpleRaw      On vient de creer une ligne simple. Associe */
/* un attribut Filet_horizontal a cette ligne et cree une cellule dans */
/* la ligne pour chaque Column simple du tableau. Associe a chaque */
/* Cellule creee un attribut La_Column qui pointe vers la Column */
/* correspondante */

#ifdef __STDC__
static void         CreateSimpleRaw (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void         CreateSimpleRaw (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pCol, pElCols, pCell, pCellPrec;
   int                 TypeCol;

   pCellPrec = NULL;
   /* met les attributs a la ligne */
   SetRawAttribute (pEl, pDoc);
   /*pElTitle = pEl->ElFirstChild; */
   /* met les attributs au titre de la ligne */
   SetAttrTitleRaw (pEl->ElFirstChild, pDoc);

   /* remonte a l'element The_Raws */
   pE = pEl->ElParent;
   while (!TypeHasException (EXC_ID_The_Raws, pE->ElTypeNumber, pEl->ElStructSchema))
      pE = pE->ElParent;
   /*pElRaw = pE; */
   /* l'element The_Columns */
   pElCols = pE->ElPrevious->ElFirstChild->ElNext;
   /* cherche le numero du type d'element Simple_Column */
   TypeCol = GetElemWithException (EXC_ID_Simple_Column, pEl->ElStructSchema);

   /* cherche la premiere cellule dans la nouvelle ligne */
   pCell = pEl->ElFirstChild->ElNext->ElFirstChild;
   /* parcourt toutes les Columns simples du tableau */
   pCol = pElCols;
   while (pCol != NULL)
     {
	pCol = FwdSearchTypedElem (pCol, TypeCol, pEl->ElStructSchema);
	if (pCol != NULL)
	   if (!ElemIsWithinSubtree (pCol, pElCols))
	      /* la Column trouvee ne fait pas partie du tableau */
	      pCol = NULL;
	   else
	     {
		/* on trouve' une Column simple du meme tableau */
		if (pCell == NULL && pCellPrec != NULL)
		  {
		     /* on cree une nouvelle cellule */
		     pCell = NewSubtree (pCellPrec->ElTypeNumber, pEl->ElStructSchema, pDoc,
				 pCellPrec->ElAssocNum, TRUE, TRUE,
				 TRUE, TRUE);
		     /* on insere cette nouvelle cellule dans l'arbre */
		     InsertElementAfter (pCellPrec, pCell);
		  }
		/* on met les attributs a la nouvelle cellule */
		SetAttrColToCell (pCell, pCol, pDoc);
		/* traite les attributs requis */
		AttachMandatoryAttributes (pCell, pDoc);
		pCellPrec = pCell;
		pCell = pCell->ElNext;
	     }
     }

   if (pCell != NULL)
     {
	/* il y a trop de cellules par rapport aux Columns */
	if (pCellPrec != NULL)
	   while (pCellPrec->ElNext != NULL)
	      RemoveElement (pCellPrec->ElNext);
	else
	   RemoveElement (pCell);
     }
}


/* CreateTable    complete le squelette de tableau cree par les */
/* procedures standard de l'editeur Thot. pEl pointe sur l'element */
/* Table. */

#ifdef __STDC__
static void         CreateTable (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void         CreateTable (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pL, pC, pElNew;
   int                 NType;
   PtrSSchema          pSS;


   pSS = pEl->ElStructSchema;
   /* met les attribut sur l'element Table */
   SetAttrFrameTable (pEl, pDoc);
   /* passe a l'element En_Tetes */
   pE = pEl->ElFirstChild;
   if (pE != NULL)
     {
	/* met les attributs sur En_Tetes */
	SetAttrHairlineToHead (pE, pDoc);
	/* met les attributs sur Title_Table */
	SetAttrTableTitle (pE->ElFirstChild, pDoc);

	pC = pE->ElFirstChild->ElNext;	/* element The_Columns */
	/* met les attributs sur Les Columns */
	SetAttrHairlineToCols (pC, pDoc);

	pL = pE->ElNext;	/* element The_Raws */


	if (!TypeHasException (EXC_ID_The_Columns, pC->ElTypeNumber, pSS))
	   printf ("On ne trouve pas The_Columns\n");
	else
	  {
	     /* descend a la premiere Column */
	     pE = pC->ElFirstChild;
	     if (!TypeHasException (EXC_ID_Column, pE->ElTypeNumber, pSS))
		printf ("On ne trouve pas la 1ere Column\n");
	     else
	       {
		  /* transforme toutes les Columns creees en Simple_Column */
		  NType = GetElemWithException (EXC_ID_Simple_Column, pSS);
		  /* numero du type Simple_Column */
		  do
		    {
		       /* transforme la Column en Simple_Column */
		       pElNew = NewSubtree (NType, pSS, pDoc, pE->ElAssocNum, TRUE, TRUE,
					   TRUE, TRUE);
		       InsertOption (pE, &pElNew, pDoc);
		       /* met les attributs a la Simple_Column */
		       SetAttrSimpleCol (pE, pDoc);
		       /* met les attributs au Titre de la Column */
		       SetAttrColTitle (pE->ElFirstChild, pDoc);
		       /* traite les attributs requis */
		       AttachMandatoryAttributes (pE, pDoc);
		       pE = pE->ElNext;
		    }
		  while (pE != NULL);

		  if (!TypeHasException (EXC_ID_The_Raws, pL->ElTypeNumber, pSS))
		     printf ("On ne trouve pas The_Raws\n");
		  else
		    {
		       /* passe a la premiere Line */
		       pE = pL->ElFirstChild;
		       if (!TypeHasException (EXC_ID_Raw, pE->ElTypeNumber, pSS))
			  printf ("On ne trouve pas la 1ere Line\n");
		       else
			 {
			    /* traite toutes les lignes creees */
			    NType = GetElemWithException (EXC_ID_Simple_Raw, pSS);
			    /* numero du type Ligne_simple */
			    do
			      {
				 /* transforme la Line en Ligne_simple */
				 pElNew = NewSubtree (NType, pSS, pDoc, pE->ElAssocNum,
						     TRUE, TRUE, TRUE, TRUE);
				 InsertOption (pE, &pElNew, pDoc);
				 /* traite les attributs requis */
				 AttachMandatoryAttributes (pE, pDoc);
				 CreateSimpleRaw (pE, pDoc);
				 /* passe a la ligne suivante */
				 pE = pE->ElNext;
			      }
			    while (pE != NULL);
			 }
		    }
	       }
	  }
     }
}


/* CreateRawHairline       Si l'element pElRaw est bien une */
/* ligne de tableau et l'element pBasPage un Bas_Table, cree */
/* la boite de presentation Filet_Bas de la ligne pElRaw. */

#ifdef __STDC__
static void         CreateRawHairline (PtrElement pElRaw, PtrElement pElFootPage, PtrDocument pDoc)

#else  /* __STDC__ */
static void         CreateRawHairline (pElRaw, pElFootPage, pDoc)
PtrElement          pElRaw;
PtrElement          pElFootPage;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 attr;
   PtrAttribute        pAttr;
   boolean             found;

   if (pElFootPage != NULL && pElRaw != NULL)
      if (TypeHasException (EXC_ID_FootTable, pElFootPage->ElTypeNumber, pElFootPage->ElStructSchema))
	 /* le bas de page est bien un bas de page */
	 if (TypeHasException (EXC_ID_Simple_Raw, pElRaw->ElTypeNumber, pElRaw->ElStructSchema) ||
	     TypeHasException (EXC_ID_Compound_Raw, pElRaw->ElTypeNumber, pElRaw->ElStructSchema))
	    /* la ligne est bien une ligne de tableau */
	   {
	      /* cherche l'attribut Filet_Bas de la ligne */
	      attr = GetAttrWithException (EXC_ID_Foot_Hairline_SimpRaw, pElRaw->ElStructSchema);
	      pAttr = pElRaw->ElFirstAttr;
	      found = FALSE;
	      while (pAttr != NULL && !found)
		 if (pAttr->AeAttrSSchema == pElRaw->ElStructSchema &&
		     pAttr->AeAttrNum == attr)
		    /* C'est l'attribut Filet_Bas */
		    found = TRUE;
		 else
		    /* passe a l'attribut suivant de la ligne */
		    pAttr = pAttr->AeNext;
	      if (found)
		 /* applique les regles de presentation de l'attribut pour */
		 /* faire creer la boite filet. */
		 UpdatePresAttr (pElRaw, pAttr, pDoc, FALSE, FALSE, NULL);
	   }
}



/* VerifAndCreate   verifie si l'element pointe par pEl est une Column ou */
/* une ligne de table et dans ce cas cree toutes les cellules de cette */
/* Column ou de cette ligne. */
/* Si l'element pEl n'est pas un element terminal cherche dans le sous arbre */
/* si par hasard il ne se trouve pas un element tableau */
/* Ce cas se produit quand on enchaine la creation d'un grand */
/* nombre d'elements */

#ifdef __STDC__
static void         VerifAndCreate (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void         VerifAndCreate (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pC;
   int                 NType;


   /* il s'agit bien de la creation d'un element de table */
   if (TypeHasException (EXC_ID_Table, pEl->ElTypeNumber, pEl->ElStructSchema))
      /* c'est la creation d'un tableau */
      CreateTable (pEl, pDoc);

   else if (TypeHasException (EXC_ID_Simple_Column, pEl->ElTypeNumber, pEl->ElStructSchema))
      /* creation d'une Simple_Column */
      CreateSimpleCol (pEl, pDoc);

   else if (TypeHasException (EXC_ID_Compound_Column, pEl->ElTypeNumber, pEl->ElStructSchema))
      /* creation d'une Compound_Column */
     {
	/* met les attributs de la Column composee */
	SetAttrHairlineCol (pEl, pDoc);
	/* met les attributs sur le Title_Column_comp */
	SetAttrColTitle (pEl->ElFirstChild, pDoc);
	/* cree les paves de l'element */
	CreateAllAbsBoxesOfEl (pEl, pDoc);
	AbstractImageUpdated (pDoc);
	/* descend a la premiere Column */
	pE = pEl->ElFirstChild->ElNext->ElFirstChild;
	if (!TypeHasException (EXC_ID_Column, pE->ElTypeNumber, pE->ElStructSchema) &&
	    !TypeHasException (EXC_ID_Simple_Column, pE->ElTypeNumber, pE->ElStructSchema) &&
	    !TypeHasException (EXC_ID_Compound_Column, pE->ElTypeNumber, pE->ElStructSchema))
	   printf ("On ne trouve pas la 1ere Column\n");
	else
	  {
	     /* traite toutes les Columns creees */
	     /* numero du type Simple_Column */
	     NType = GetElemWithException (EXC_ID_Simple_Column, pEl->ElStructSchema);
	     do
	       {
		  if (TypeHasException (EXC_ID_Column, pE->ElTypeNumber, pE->ElStructSchema))
		    {
		       /* transforme la Column en Simple_Column */
		       pC = NewSubtree (NType, pEl->ElStructSchema, pDoc, pEl->ElAssocNum,
					TRUE, TRUE, TRUE, TRUE);
		       InsertOption (pE, &pC, pDoc);
		       /* traite les attributs requis */
		       AttachMandatoryAttributes (pE, pDoc);
		       CreateSimpleCol (pE, pDoc);
		    }
		  pE = pE->ElNext;
	       }
	     while (pE != NULL);
	  }
     }

   else if (TypeHasException (EXC_ID_Simple_Raw, pEl->ElTypeNumber, pEl->ElStructSchema))
      /* creation d'une Ligne_simple */
      CreateSimpleRaw (pEl, pDoc);

   else if (TypeHasException (EXC_ID_Compound_Raw, pEl->ElTypeNumber, pEl->ElStructSchema))
      /* creation d'une Ligne composee */
     {
	/* met l'attribut Filet_horizontal */
	SetRawAttribute (pEl, pDoc);
	/* met l'attribut sur le Title_Compound_Raw */
	SetAttrTitleRaw (pEl->ElFirstChild, pDoc);
	/* descend a la premiere Line */
	pE = pEl->ElFirstChild->ElNext->ElFirstChild;
	if (!TypeHasException (EXC_ID_Raw, pE->ElTypeNumber, pE->ElStructSchema))
	   printf ("On ne trouve pas la 1ere Line\n");
	else
	  {
	     /* traite toutes les lignes creees */
	     /* numero du type Ligne_simple */
	     NType = GetElemWithException (EXC_ID_Simple_Raw, pEl->ElStructSchema);
	     do
	       {
		  /* transforme la Line en Ligne_simple */
		  pC = NewSubtree (NType, pEl->ElStructSchema, pDoc, pEl->ElAssocNum,
				   TRUE, TRUE, TRUE, TRUE);
		  InsertOption (pE, &pC, pDoc);
		  /* traite les attributs requis */
		  AttachMandatoryAttributes (pE, pDoc);
		  CreateSimpleRaw (pE, pDoc);
		  pE = pE->ElNext;
	       }
	     while (pE != NULL);
	  }
     }
}


/* SearchAndCreateTable  Parcours recursivement l'arbre pointe par pEl
et cree les elements de tableau (table, Columns, lignes) */

#ifdef __STDC__
static void         SearchAndCreateTable (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SearchAndCreateTable (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE;

   if (pEl != NULL)
      if (TypeHasException (EXC_TR_Table_CREATION, pEl->ElTypeNumber, pEl->ElStructSchema))
	 VerifAndCreate (pEl, pDoc);
      else
	{
	   /* On cherche un eventuel tableau dans la descendance de pEl */
	   if (!pEl->ElTerminal)
	     {
		pE = pEl->ElFirstChild;
		while (pE != NULL)
		  {
		     SearchAndCreateTable (pE, pDoc);
		     pE = pE->ElNext;
		  }
	     }
	}
}

/* SelectColSimple      selectionne toutes les cellules correspondant */
/* a la Column simple pointe' par pEl. */

#ifdef __STDC__
static void         SelectSimpleCol (PtrElement pEl)

#else  /* __STDC__ */
static void         SelectSimpleCol (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pElRaw, pElRawSuiv, pE, pElTheRaws, pCell;
   PtrAttribute        pAttr;
   int                 NType, attr;

   if (pEl != NULL)
     {
	/* remonte a l'element En_Tetes */
	NType = GetElemWithException (EXC_ID_Headings, pEl->ElStructSchema);
	pE = pEl->ElParent;
	while (pE->ElTypeNumber != NType)
	   pE = pE->ElParent;
	/* l'element apres En_Tetes est The_Raws */
	pElTheRaws = pE->ElNext;
	/* numero du type d'element Ligne_simple */
	NType = GetElemWithException (EXC_ID_Simple_Raw, pEl->ElStructSchema);
	/* numero de l'attribut Ref_Column */
	attr = GetAttrWithException (EXC_ID_Ref_Column, pEl->ElStructSchema);
	/* cherche la premiere ligne du tableau */
	pElRaw = FwdSearchTypedElem (pEl, NType, pEl->ElStructSchema);
	if (pElRaw != NULL)
	   if (!ElemIsWithinSubtree (pElRaw, pElTheRaws))
	      /* la ligne founde ne fait pas partie du meme tableau */
	      pElRaw = NULL;
	/* parcourt toutes les lignes simples du tableau, dans l'ordre */

	while (pElRaw != NULL)
	  {
	     /* on commence par chercher la ligne suivante */
	     pElRawSuiv = FwdSearchTypedElem (pElRaw, NType, pEl->ElStructSchema);
	     if (pElRawSuiv != NULL)
		if (!ElemIsWithinSubtree (pElRawSuiv, pElTheRaws))
		   /* la ligne founde ne fait pas partie du meme tableau */
		   pElRawSuiv = NULL;

	     /* on passe a la premiere cellule de la ligne */
	     pCell = NULL;
	     if (pElRaw->ElFirstChild != NULL)
		if (pElRaw->ElFirstChild->ElNext != NULL)
		   pCell = pElRaw->ElFirstChild->ElNext->ElFirstChild;
	     /* cherche dans cette ligne la cellule qui reference cette */
	     /* Column simple */
	     while (pCell != NULL)
	       {
		  pAttr = pCell->ElFirstAttr;
		  /* cherche l'attribut La_Column de la cellule */
		  while (pAttr != NULL)
		     if (pAttr->AeAttrSSchema == pEl->ElStructSchema && pAttr->AeAttrNum == attr)
			/* C'est l'attribut La_Column */
		       {
			  if (pAttr->AeAttrReference != NULL)
			     if (pAttr->AeAttrReference->RdReferred != NULL)
				if (pAttr->AeAttrReference->RdReferred->ReReferredElem == pEl)
				  {
				     /* l'attribut La_Column de cette cellule reference */
				     /* la Column simple traitee, on selectionne cette cellule */
				     AddInSelection (pCell, (pElRawSuiv == NULL));
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
	     pElRaw = pElRawSuiv;
	  }
     }
}

/* TableSelection    verifie si l'element pointe par pEl est une Column */
/* de table et dans ce cas selectionne toutes les cellules de cette */
/* Column. Si selExtension est vrai, il s'agit d'une selextension de selection. */
/* Retourne vrai si la selection est correcte, faux s'il s'agit d'une */
/* extension de selection invalide (on ne selectionne pas plus d'une */
/* Column a la fois). */

#ifdef __STDC__
static void         TableSelection (PtrElement pEl, PtrDocument pDoc, boolean selExtension, boolean * ret)

#else  /* __STDC__ */
static void         TableSelection (pEl, pDoc, selExtension, ret)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             selExtension;
boolean            *ret;

#endif /* __STDC__ */

{
   PtrElement          pE;

   pE = NULL;
   *ret = TRUE;
   if (selExtension)
     {
	/* c'est une extension de la selection */
	if (FirstSelectedElement != NULL)
	  {
	     /* il y a deja une selection */
	     pEl = FirstSelectedElement;
	     if (TypeHasException (EXC_TR_Table_SELECT, pEl->ElTypeNumber, pEl->ElStructSchema))
		/* Deja une Column selectionnee, on refuse l'extension */
		*ret = FALSE;
	  }
	if (TypeHasException (EXC_TR_Table_SELECT, pEl->ElTypeNumber, pEl->ElStructSchema))
	   /* l'element a ajouter dans la selection est une Column, refus */
	   *ret = FALSE;
     }

   else
     {
	/* c'est une nouvelle  selection */
	if (TypeHasException (EXC_TR_Table_SELECT, pEl->ElTypeNumber, pEl->ElStructSchema))
	   /* il s'agit bien d'un element demandant une selection speciale */

	   if (TypeHasException (EXC_ID_Simple_Column, pEl->ElTypeNumber, pEl->ElStructSchema))
	     {
		/* selection d'un element Simple_Column */
		SelectSimpleCol (pEl);
		LastSelectedElement = pEl;
	     }

	   else
	     {
		if (TypeHasException (EXC_ID_Compound_Column, pEl->ElTypeNumber, pEl->ElStructSchema))
		   /* selection d'un element Compound_Column */
		  {
		     if (pEl->ElFirstChild != NULL)
			if (pEl->ElFirstChild->ElNext != NULL)
			   /* descend a la premiere sous-Column */
			   pE = pEl->ElFirstChild->ElNext->ElFirstChild;
		  }
		else if (TypeHasException (EXC_ID_Sub_Columns, pEl->ElTypeNumber, pEl->ElStructSchema))
		   /* selection d'un element Sous Column */
		   pE = pEl->ElFirstChild;

		else if (TypeHasException (EXC_ID_The_Columns, pEl->ElTypeNumber, pEl->ElStructSchema))
		   /* selection d'un element Les Column */
		   pE = pEl->ElFirstChild;

		/* traite toutes les sous-Columns */
		while (pE != NULL && *ret) /* pb BUG ret ?? */
		  {
		     if (TypeHasException (EXC_ID_Simple_Column, pE->ElTypeNumber, pE->ElStructSchema))
			/* c'est une Column simple */
			SelectSimpleCol (pE);
		     else
			TableSelection (pE, pDoc, selExtension, ret);
		     pE = pE->ElNext;
		  }
		LastSelectedElement = pEl;
	     }
     }
}

/* LastSavedIsAColumn       Verifie si le premier element du buffer */
/* copier-couper-coller, qui est pointe' par pElSv, est un element */
/* Column de tableau et repond vrai si oui. Dans ce cas, cet element */
/* est egalement considere comme le dernier du buffer, meme s'il est */
/* suivi de cellules, qui sont considerees comme lui appartenant. */

#ifdef __STDC__
 static void         LastSavedIsAColumn (PtrElement pElSv, boolean * ret)

#else  /* __STDC__ */
 static void         LastSavedIsAColumn (pElSv, ret)
 PtrElement          pElSv;
 boolean            *ret;

#endif /* __STDC__ */

 {

   /* est-ce un element Simple_Column ? */
    *ret = TypeHasException (EXC_ID_Simple_Column, pElSv->ElTypeNumber, pElSv->ElStructSchema);

    if (*ret)			/* est-ce un element Compound_Column ? */
       *ret = TypeHasException (EXC_ID_Compound_Column, pElSv->ElTypeNumber, pElSv->ElStructSchema);
 }


/* PasteCells      Colle les NbCell premieres cellules de la suite de */
/* cellules pointee par pSvCell dans les lignes simples du tableau */
/* auquel appartient l'element Simple_Column pointe' par pCol. Dans */
/* chaque ligne simple, la cellule est collee apres la cellule de rang */
/* NbPreced. */
/* Si NbCell est inferieur au nombre de lignes du tableau, la Column */
/* pCol est completee avec des cellules vides. */
/* Au retour, pSvCell pointe la cellule qui suit la derniere cellule */
/* collee. */

#ifdef __STDC__
static void         PasteCells (PtrElement pCol, int NbPreced, int NbCell, PtrElement * pSvCell, PtrDocument pDoc)

#else  /* __STDC__ */
static void         PasteCells (pCol, NbPreced, NbCell, pSvCell, pDoc)
PtrElement          pCol;
int                 NbPreced;
int                 NbCell;
PtrElement         *pSvCell;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pElTheRaws, pElRaw, pCell, pNCell, pElParent;
   int                 rawType, cellType, i, nbPastedCells;


   nbPastedCells = 0;
   cellType = GetElemWithException (EXC_ID_Cell, pCol->ElStructSchema);	/* numero du type Cellule */
   rawType = GetElemWithException (EXC_ID_The_Raws, pCol->ElStructSchema);	/* numero du type The_Raws */
   pElTheRaws = FwdSearchTypedElem (pCol, rawType, pCol->ElStructSchema);	/* cherche l'element The_Raws */
   rawType = GetElemWithException (EXC_ID_Simple_Raw, pCol->ElStructSchema);	/* numero du type Ligne_simple */

   /* parcourt les lignes simples du tableau */
   pElRaw = pElTheRaws;
   while (pElRaw != NULL)
     {
	pElRaw = FwdSearchTypedElem (pElRaw, rawType, pCol->ElStructSchema);
	if (pElRaw != NULL)
	   if (!ElemIsWithinSubtree (pElRaw, pElTheRaws))	/* la ligne founde ne fait pas partie du tableau */
	      pElRaw = NULL;
	   else
	     {
		/* on a found' une ligne simple appartenant au meme tableau,
		   on passe a la premiere cellule de la ligne */
		pElParent = pElRaw->ElFirstChild->ElNext;
		pCell = pElParent->ElFirstChild;
		if (nbPastedCells < NbCell)
		  {
		     /* on cree une copie de la cellule a coller */
		     pNCell = CopyTree (*pSvCell, DocOfSavedElements, pCol->ElAssocNum,
			     pCol->ElStructSchema, pDoc, pElParent, TRUE, TRUE);
		     nbPastedCells++;
		     *pSvCell = (*pSvCell)->ElNext;
		  }
		else
		  {
		     /* on a deja colle' le nombre de cellules voulu, on cree une
		        cellule vide pour completer la Column */
		     pNCell = NewSubtree (cellType, pElParent->ElStructSchema, pDoc,
					pElParent->ElAssocNum, TRUE, TRUE, TRUE,
					  TRUE);
		     /* on met les attributs a la nouvelle cellule */
		     SetAttrColToCell (pNCell, pCol, pDoc);
		  }

		/* on insere la nouvelle cellule dans l'arbre */
		if (pCell == NULL)
		   InsertFirstChild (pElParent, pNCell);
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
		NCreatedElements++;
		CreatedElement[NCreatedElements - 1] = pNCell;
	     }
     }

   /* a-t-on colle le nombre voulu de cellules ? */
   if (nbPastedCells < NbCell)
      /* non, on saute les cellules qui n'ont pas ete collees parce que */
      /* le tableau ou on colle a moins de lignes que le tableau d'ou */
      /* viennent les cellules a coller */
      for (i = nbPastedCells + 1; i <= NbCell; i++)
	 *pSvCell = (*pSvCell)->ElNext;
}


/* VerifColCell verifie que les Columns_simples contenues dans */
/* l'element pointe' par pCol ont chacune une cellule a partir de celle */
/* pointee par pCell. */
/* pCellPrec pointe sur la cellule precedant pCell. */
/* Au retour, pCellPrec pointe sur la derniere cellule traitee et */
/* pCell sur la prochaine a traiter. */

#ifdef __STDC__
static void         CheckColCell (PtrElement pCol, PtrElement * pCell, PtrElement * pCellPrec, PtrDocument pDoc)

#else  /* __STDC__ */
static void         CheckColCell (pCol, pCell, pCellPrec, pDoc)
PtrElement          pCol;
PtrElement         *pCell;
PtrElement         *pCellPrec;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pC;
   int                 colSimpleType;
   PtrElement          pEl;


   colSimpleType = GetElemWithException (EXC_ID_Simple_Column, pCol->ElStructSchema);
   if (pCol->ElTypeNumber == colSimpleType)
     {
	/* c'est une Column simple */
	if (*pCell == NULL && *pCellPrec != NULL)
	   /* il n'y a pas de cellule pour cette Column */
	  {
	     /* cree une cellule */
	     pEl = *pCellPrec;
	     *pCell = NewSubtree (pEl->ElTypeNumber, pEl->ElStructSchema, pDoc,
				  pEl->ElAssocNum, TRUE, TRUE, TRUE, TRUE);
	     /* insere cette cellule a la suite de la cellule precedente */
	     InsertElementAfter (*pCellPrec, *pCell);
	  }
	if (*pCell != NULL)
	  {
	     SetAttrColToCell (*pCell, pCol, pDoc);	/* verifie les attributs de la cellule */
	     /* traite les attributs requis */
	     AttachMandatoryAttributes (*pCell, pDoc);
	     *pCellPrec = *pCell;
	     *pCell = (*pCell)->ElNext;		/* passe a la cellule suivante */
	  }
     }

   else
     {
	/* c'est une Column composee, traite toutes les sous-Columns */
	pC = pCol->ElFirstChild;
	while (pC->ElNext != NULL)
	   /* cherche l'element Sub_Columns */
	   pC = pC->ElNext;
	pC = pC->ElFirstChild;	/* 1ere sous-Column */
	while (pC != NULL)
	   /* traite toutes les sous-Columns */
	  {
	     CheckColCell (pC, pCell, pCellPrec, pDoc);
	     pC = pC->ElNext;
	  }
     }
}


/* CheckRaw traite toutes les lignes simples contenues dans */
/* l'element pointe' par pElRaw */

#ifdef __STDC__
static void         CheckRaw (PtrElement pElRaw, PtrElement pElCols, PtrDocument pDoc)

#else  /* __STDC__ */
static void         CheckRaw (pElRaw, pElCols, pDoc)
PtrElement          pElRaw;
PtrElement          pElCols;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pSubRaw, pCell, pCellPrec, pCellSuiv, pCol, pElTitleRaw;
   int                 rawTypeSimple;

   /* accede au titre de la ligne */
   pElTitleRaw = pElRaw->ElFirstChild;
   if (TypeHasException (EXC_ID_Title_Raw, pElTitleRaw->ElTypeNumber, pElTitleRaw->ElStructSchema)
       || TypeHasException (EXC_ID_Title_Compound_Raw, pElTitleRaw->ElTypeNumber, pElTitleRaw->ElStructSchema))
      /* c'est bien un element Title_Raw ou Title_Compound_Raw */
      SetAttrTitleRaw (pElTitleRaw, pDoc);

   rawTypeSimple = GetElemWithException (EXC_ID_Simple_Raw, pElRaw->ElStructSchema);
   if (pElRaw->ElTypeNumber == rawTypeSimple)		/* c'est une ligne simple on la traite */
     {
	pCell = pElRaw->ElFirstChild;	/* accede a la premiere cellule */
	while (pCell->ElNext != NULL)
	   pCell = pCell->ElNext;
	pCell = pCell->ElFirstChild;	/* pCell pointe sur l'element Contenu_Raw */
	/* pCell: premiere cellule */
	/* parcourt toutes les Column du tableau */
	pCol = pElCols->ElFirstChild;
	pCellPrec = NULL;
	while (pCol != NULL)
	  {
	     CheckColCell (pCol, &pCell, &pCellPrec, pDoc);
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
      /* c'est une ligne composee passe a l'element Sub_Raws */
     {
	pSubRaw = pElRaw->ElFirstChild;
	while (pSubRaw->ElNext != NULL)
	   pSubRaw = pSubRaw->ElNext;
	/* traite tous les elements contenus dans l'element Sub_Raws */
	pSubRaw = pSubRaw->ElFirstChild;
	while (pSubRaw != NULL)
	  {
	     CheckRaw (pSubRaw, pElCols, pDoc);
	     pSubRaw = pSubRaw->ElNext;
	  }
     }
}


/* PasteSibling      effectue le traitement particulier a l'operation */
/* Coller lorsqu'elle s'applique a une Column ou une ligne de tableau. */
/* pElPasted pointe sur l'element colle' et pElSv sur l'element */
/* a coller ensuite. */

#ifdef __STDC__
static void         PasteSibling (PtrElement pElPasted, PtrElement * pElSv, PtrDocument pDoc)

#else  /* __STDC__ */
static void         PasteSibling (pElPasted, pElSv, pDoc)
PtrElement          pElPasted;
PtrElement         *pElSv;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pE, pCell, pSimpleCol;
   int                 nbColPrec, nbTotalCell, nbCellbyCol, nbPastedCols;
   int                 col;
   int                 colSimpleType, colCopoundType, typeNumber;


   if (TypeHasException (EXC_TR_Table_PASTE, pElPasted->ElTypeNumber, pElPasted->ElStructSchema))
      /* Il s'agit d'un element de tableau devant faire l'objet d'un */
      /* traitement special pour la commande Coller (voir Tableau.SCH). */
     {
	colSimpleType = GetElemWithException (EXC_ID_Simple_Column, pElPasted->ElStructSchema);
	colCopoundType = GetElemWithException (EXC_ID_Compound_Column, pElPasted->ElStructSchema);

	if (pElPasted->ElTypeNumber == colSimpleType || pElPasted->ElTypeNumber == colCopoundType)
	  {
	     /* c'est un element Simple_Column ou Compound_Column qui a ete colle' */
	     /* remonte a l'element The_Columns */
	     typeNumber = GetElemWithException (EXC_ID_The_Columns, pElPasted->ElStructSchema);
	     pE = pElPasted->ElParent;
	     while (pE->ElTypeNumber != typeNumber)
		pE = pE->ElParent;

	     /* compte le nombre de Columns simples qui precedent l'element */
	     /* colle': nbColPrec. */
	     nbColPrec = 0;
	     pSimpleCol = pE;
	     do
	       {
		  pSimpleCol = FwdSearchTypedElem (pSimpleCol, colSimpleType, pElPasted->ElStructSchema);
		  if (ElemIsWithinSubtree (pSimpleCol, pElPasted))
		     pSimpleCol = NULL;
		  else		/* on found' une Column simple */
		     nbColPrec++;
	       }
	     while (pSimpleCol != NULL);

	     /* compte les cellules qui sont dans le buffer Couper-Coller */
	     pCell = *pElSv;
	     nbTotalCell = 0;
	     while (pCell != NULL)
	       {
		  nbTotalCell++;
		  pCell = pCell->ElNext;
	       }

	     /* a priori, c'est un element Simple_Column qui a ete colle' */
	     nbPastedCols = 1;
	     pSimpleCol = pElPasted;
	     if (TypeHasException (EXC_ID_Compound_Column, pElPasted->ElTypeNumber, pElPasted->ElStructSchema))
		/* c'est un element Compound_Column */
		/* compte les elements Simple_Column contenus dans l'element colle': nbPastedCols. */
	       {
		  nbPastedCols = 0;
		  pSimpleCol = NULL;
		  pE = pElPasted;
		  while (pE != NULL)
		    {
		       pE = FwdSearchTypedElem (pE, colSimpleType, pElPasted->ElStructSchema);
		       if (pE != NULL)	/* on a found' une Column simple */
			  if (ElemIsWithinSubtree (pE, pElPasted))
			    {
			       nbPastedCols++;
			       if (pSimpleCol == NULL)
				  pSimpleCol = pE;	/* 1ere Column simple collee */
			    }
			  else
			     pE = NULL;
		    }
	       }

	     /* calcule le nombre de cellules par Column */
	     nbCellbyCol = nbTotalCell / nbPastedCols;
	     /* colle les cellules correspondant a chacune des Columns */
	     /* simples collees. */
	     for (col = 1; col <= nbPastedCols; col++)
	       {
		  PasteCells (pSimpleCol, nbColPrec, nbCellbyCol, pElSv, pDoc);
		  nbColPrec++;
		  pSimpleCol = FwdSearchTypedElem (pSimpleCol, colSimpleType, pElPasted->ElStructSchema);
	       }
	     *pElSv = NULL;	/* empeche PasteBeforeOrAfter de coller les cellules */
	  }

	else
	   /* ce n'est ni un element Simple_Column ni un element
	      Compound_Column qui a ete colle' */
	   if (TypeHasException (EXC_ID_Simple_Raw, pElPasted->ElTypeNumber, pElPasted->ElStructSchema)
	       || TypeHasException (EXC_ID_Compound_Raw, pElPasted->ElTypeNumber, pElPasted->ElStructSchema))
	   /* c'est un element Ligne_simple ou Ligne_Composed qui a ete colle' */
	   /* Pour chaque ligne simple collee, on verifie que chaque */
	   /* cellule a bien un attribut La_Column (si elle ne l'a pas, */
	   /* on lui met) et que le nombre de cellules est egal au nombre */
	   /* de Columns simples du tableau (si non, on supprime les */
	   /* cellules excedentaires ou on cree les cellules manquantes. */
	   /* remonte d'abord a l'element The_Raws */
	  {
	     typeNumber = GetElemWithException (EXC_ID_The_Raws, pElPasted->ElStructSchema);
	     pE = pElPasted->ElParent;
	     while (pE->ElTypeNumber != typeNumber)
		pE = pE->ElParent;
	     /* refound l'element The_Columns */
	     pE = pE->ElPrevious->ElFirstChild->ElNext;
	     if (TypeHasException (EXC_ID_The_Columns, pE->ElTypeNumber, pE->ElStructSchema))
		/* c'est bien The_Columns */
		CheckRaw (pElPasted, pE, pDoc);
	  }
     }
}


/* CutTable       Si l'element pointe' par pFootTable est */
/* effectivement un Bas_Table, on reapplique les regles de hauteur */
/* des filets verticaux engendre's par l'En_Tete qui precede */

#ifdef __STDC__
static void         CutTable (PtrElement pFootTable, PtrDocument pDoc)

#else  /* __STDC__ */
static void         CutTable (pFootTable, pDoc)
PtrElement          pFootTable;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 typeSaved;

   if (TypeHasException (EXC_ID_FootTable, pFootTable->ElTypeNumber,
			 pFootTable->ElStructSchema))
      /* c'est un element Bas_Table */
     {
	/* reapplique les regles de hauteur des filets */
	/* verticaux engendre's par l'En_Tete qui precede */
	/* On camoufle d'abord l'element Bas_Table */
	/* pour qu'il ne soit pas pris en compte */
	typeSaved = pFootTable->ElTypeNumber;
	pFootTable->ElTypeNumber = 1;
	ApplHeightRuleToHairlines (pFootTable, pDoc);
	pFootTable->ElTypeNumber = typeSaved;
     }
}

/* ExcCutPage est appele' par CutCommand qui effectue le traitement */
/* de la commande Couper. */
/* pElFirstSel et pElLastSel pointent le premier et le dernier element */
/* selectionne's, qui doivent etre coupe's. */
/* S'il s'agit d'un seul et meme element saut de page qui se trouve */
/* dans une structure demandant un traitement special des sauts de */
/* pages, on etend la selection a l'element portant l'exception */
/* PageBreakRepBefore qui precede ce saut de page et a l'element */
/* portant l'exception PageBreakRepetition qui */
/* suit, pour que CutCommand coupe les 3 elements a la fois. */
/* Dans ce cas, on met toBeSaved a Faux (on ne sauvera pas les elements */
/* coupe's dans le buffer Couper-Copier-Coller) et deletePage a */
/* Vrai (on detruira le saut de page bien qu'il ne soit plus le seul */
/* selectionne'). */

#ifdef __STDC__
static void         ExcCutPage (PtrElement * pElFirstSel, PtrElement * pElLastSel, PtrDocument pDoc, boolean * toBeSaved, boolean * deletePage)

#else  /* __STDC__ */
static void         ExcCutPage (pElFirstSel, pElLastSel, pDoc, toBeSaved, deletePage)
PtrElement         *pElFirstSel;
PtrElement         *pElLastSel;
PtrDocument         pDoc;
boolean            *toBeSaved;
boolean            *deletePage;

#endif /* __STDC__ */

{
   PtrElement          pElPrec, pElNext;
   boolean             stop;

   if (*pElFirstSel == *pElLastSel)
      /* un seul element est selectionne' */
      if ((*pElFirstSel)->ElTerminal)
	 /* c'est une feuille */
	 if ((*pElFirstSel)->ElLeafType == LtPageColBreak)
	    /* c'est une marque de page */
	   {
	      /* les precedents peuvent etre des elements repete's */
	      pElPrec = (*pElFirstSel)->ElPrevious;
	      stop = FALSE;
	      while (!stop)
		{
		   if (pElPrec == NULL)
		      stop = TRUE;	/* pas d'autre element precedent */
		   else if (!TypeHasException (ExcPageBreakRepBefore, pElPrec->ElTypeNumber,
					       pElPrec->ElStructSchema))
		      /* l'element precedent n'est pas une repetition */
		      stop = TRUE;
		   else if (pElPrec->ElSource == NULL)
		      /* l'element precedent n'est pas une inclusion */
		      stop = TRUE;
		   else
		      /* il faut supprimer cet element precedent */
		     {
			*pElFirstSel = pElPrec;
			*toBeSaved = FALSE;
			*deletePage = TRUE;
			/* passe au precedent */
			pElPrec = pElPrec->ElPrevious;
		     }
		}
	      /* les suivants peuvent etre des elements repetes */
	      pElNext = (*pElLastSel)->ElNext;
	      stop = FALSE;
	      while (!stop)
		{
		   if (pElNext == NULL)
		      stop = TRUE;	/* pas d'autre element suivant */
		   else if (!TypeHasException (ExcPageBreakRepetition, pElNext->ElTypeNumber,
					       pElNext->ElStructSchema))
		      /* l'element suivant n'est pas une repetition */
		      stop = TRUE;
		   else if (pElNext->ElSource == NULL)
		      /* l'element suivant n'est pas une inclusion */
		      stop = TRUE;
		   else
		      /* il faut supprimer cet element suivant */
		     {
			*pElLastSel = pElNext;
			*toBeSaved = FALSE;
			*deletePage = TRUE;
			pElNext = pElNext->ElNext;
		     }
		}
	      if (*deletePage == TRUE)
		CutTable (*pElFirstSel, pDoc);
	   }
}



/* CheckExtension        verifie si l'attribut pAttr qui doit etre */
/* applique' a` tous les elements compris entre pElFirst et pElLast */
/* est un attribut Extension_horiz ou Extension_vert. */
/* Dans ce cas, verifie si cet attribut designe un element correct */
/* et si ce n'est pas le cas, annule l'attribut ou le supprime si */
/* deleteAttr est Vrai. */


#ifdef __STDC__
static void         CheckExtension (PtrAttribute pAttr, PtrElement pElFirst, PtrElement pElLast, boolean deleteAttr)

#else  /* __STDC__ */
static void         CheckExtension (pAttr, pElFirst, pElLast, deleteAttr)
PtrAttribute        pAttr;
PtrElement          pElFirst;
PtrElement          pElLast;
boolean             deleteAttr;

#endif /* __STDC__ */

{
   PtrElement          pEl, pElRef;
   DocumentIdentifier  IdentDoc;
   PtrDocument         pDoc;
   boolean             error, verif, stop;

   error = FALSE;
   verif = FALSE;
   /* accede a l'element designe' par l'attribut */
   pElRef = ReferredElement (pAttr->AeAttrReference, &IdentDoc, &pDoc);
   if (pElRef != NULL)
      if (AttrHasException (EXC_ID_Extens_Vertic, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	 /* c'est l'attribut Debordement_vert */
	 verif = TRUE;
      else if (AttrHasException (EXC_ID_Extens_Horiz, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
	 /* c'est l'attribut Debordement_horiz */
	 if (pElFirst != pElLast)
	    /* l'attribut doit s'appliquer a plusieurs elements, erreur */
	    error = TRUE;
	 else
	   {
	      if (pElFirst == pElRef)
		 /* la cellule s'etend sur elle-meme, erreur */
		 error = TRUE;
	      else if (pElFirst->ElParent != pElRef->ElParent)
		 /* les deux cellules n'appartiennent pas a la meme ligne */
		 error = TRUE;
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
	pEl = pElFirst;
	/* parcourt les elements auxquels il faut appliquer l'attribut */
	while (pEl != NULL && !error)
	  {
	     if (!TypeHasException (EXC_ID_Cell, pEl->ElTypeNumber, pEl->ElStructSchema))
		/* l'element n'est pas une cellule, erreur */
		error = TRUE;
	     else if (!ElemIsBefore (pEl, pElRef))
		/* l'element ne precede pas l'element designe', erreur */
		error = TRUE;
	     if (!error)
		pEl = NextInSelection (pEl, pElLast);	/* element suivant */
	  }
	if (!error)
	   /* verifie qu'il s'agit bien du meme tableau */
	  {
	     pEl = pElFirst;
	     /* remonte a la racine du tableau */
	     stop = FALSE;
	     while (!stop && pEl != NULL)
	       {
		  if (TypeHasException (EXC_ID_Table, pEl->ElTypeNumber, pEl->ElStructSchema))
		     stop = TRUE;	/* found' */
		  else
		     pEl = pEl->ElParent;
	       }
	     if (pEl == NULL)
		error = TRUE;	/* pas d'ascendant de type Table */
	     else if (!ElemIsAnAncestor (pEl, pElRef))
		/* l'element reference' n'est pas dans le meme tableau */
		error = TRUE;
	  }
     }
   if (error)
      /* il y a erreur, on annule ou supprime l'attribut */
      if (deleteAttr)
	 DeleteAttribute (pElFirst, pAttr);
      else
	 DeleteReference (pAttr->AeAttrReference);
}


/*----------------------------------------------------------------------
   CanHolophrast   indique si on peut holophraster         
   l'element pointe' par pEl.                              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         CanHolophrast (PtrElement pEl, boolean * ret)

#else  /* __STDC__ */
static void         CanHolophrast (pEl, ret)
PtrElement          pEl;
boolean            *ret;

#endif /* __STDC__ */

{

   *ret = TRUE;
   /* on n'holophraste pas les entetes de tableau */
   if (TypeHasException (EXC_ID_Headings, pEl->ElTypeNumber, pEl->ElStructSchema))
      *ret = FALSE;
   /* on n'holophraste pas les Columns composees */
   else if (TypeHasException (EXC_ID_Compound_Column, pEl->ElTypeNumber,
			      pEl->ElStructSchema))
      *ret = FALSE;
   /* on n'holophraste pas les Columns */
   else if (TypeHasException (EXC_ID_The_Columns, pEl->ElTypeNumber,
			      pEl->ElStructSchema))
      *ret = FALSE;
   /* on n'holophraste pas les Columns simples */
   else if (TypeHasException (EXC_ID_Simple_Column, pEl->ElTypeNumber,
			      pEl->ElStructSchema))
      *ret = FALSE;
   /* on n'holophraste pas les sous-Columns */
   else if (TypeHasException (EXC_ID_Sub_Columns, pEl->ElTypeNumber,
			      pEl->ElStructSchema))
      *ret = FALSE;
}


/*----------------------------------------------------------------------
   	HasTableExc	indique si l'element pEl porte une exception  	
   		tableau EXC_ID_Table.			  	        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         HasTableExc (PtrElement pEl, boolean * ret)
#else  /* __STDC__ */
static void         HasTableExc (pEl, ret)
PtrElement          pEl;
boolean            *ret;

#endif /* __STDC__ */
{
   *ret = (TypeHasException (EXC_ID_Table, pEl->ElTypeNumber,
			     (pEl->ElStructSchema)));
}

/*----------------------------------------------------------------------
   	HasTableExcCreate	indique si l'element pEl porte une exception  	
   		tableau EXC_ID_SearchAndCreate.	    	  	        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         HasTableExcCreate (PtrElement pEl, boolean * ret)
#else  /* __STDC__ */
static void         HasTableExcCreate (pEl, ret)
PtrElement          pEl;
boolean            *ret;

#endif /* __STDC__ */
{
   *ret = (TypeHasException (EXC_TR_Table_CREATION, pEl->ElTypeNumber,
			     (pEl->ElStructSchema)));
}

/*----------------------------------------------------------------------
   TableauLoadResources : connecte les ressources de traitement des  
   tableaux            	                                        
  ----------------------------------------------------------------------*/
void                TableauLoadResources ()
{

   if (ThotLocalActions[T_createtable] == NULL)
     {
	/* initialisations */

	/* connexion des ressources */
	TteConnectAction (T_createtable, (Proc) SearchAndCreateTable);
	TteConnectAction (T_selecttable, (Proc) TableSelection);
	TteConnectAction (T_singlecell, (Proc) OrphanCell);
	TteConnectAction (T_attrtable, (Proc) AttributSpecial);
	TteConnectAction (T_lastsaved, (Proc) LastSavedIsAColumn);
	TteConnectAction (T_pastesiblingtable, (Proc) PasteSibling);
	TteConnectAction (T_refattr, (Proc) CheckAttrRef);
	TteConnectAction (T_createhairline, (Proc) CreateRawHairline);
	TteConnectAction (T_holotable, (Proc) CanHolophrast);
	TteConnectAction (T_checkextens, (Proc) CheckExtension);
	TteConnectAction (T_ruleattr, (Proc) CanApplAttrRules);
	TteConnectAction (T_abref, (Proc) CheckHeightRuleHairline);
	TteConnectAction (T_condlast, (Proc) CheckNextIsFootTable);
	TteConnectAction (T_vertspan, (Proc) SetVertOverflow);
	TteConnectAction (T_excepttable, (Proc) HasTableExc);
	TteConnectAction (T_entertable, (Proc) HasTableExcCreate);
	TteConnectAction (T_insertpage, (Proc) InsertPageInTable);
	TteConnectAction (T_cutpage, (Proc) ExcCutPage);
	TteConnectAction (T_deletepage, (Proc) DeletePageInTable);
	TteConnectAction (T_deletepageab, (Proc) DeletePageAbsBoxes);
     }
}

/* End Of Module table */
