
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   attrmenu.c : creation du menu des attributs
   V. Quint     Mai 1992
   I. Vatton    Mai 1996
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "dialog.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "constmenu.h"
#include "appdialogue.h"

#undef EXPORT
#define EXPORT extern
#include "select_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

/* indicateur d'existence des formulaires TtAttribute */
static boolean      AttrFormExists = FALSE;
static boolean      MandatoryAttrFormExists = FALSE;

/* les attributs figurant dans le menu */
static PtrSSchema AttrStruct[LgMaxAttributeMenu];
static int          AttrNumero[LgMaxAttributeMenu];
static boolean      AttrOblig[LgMaxAttributeMenu];

/* l'attribut concerne' par le formulaire de saisie affiche' */
static PtrSSchema SchAttributCourant = NULL;
static int          NumAttributCourant = 0;
static int          ActiveAttr[100];
static int          CurrentAttr;

/* valeur de retour du formulaire de saisie */
static int          ValeurNumAttribut;

#define LgMaxAttrTexte 500
static char         ValeurTexteAttribut[LgMaxAttrTexte];

/* contexte des attributs requis */
static PtrAttribute  PtrAttrRequis;

#include "appli_f.h"
#include "tree_f.h"
#include "attrherit_f.h"
#include "attributes_f.h"
#include "structcreation_f.h"
#include "appdialogue_f.h"
#include "callback_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "changeabsbox_f.h"
#include "structselect_f.h"
#include "selectmenu_f.h"
#include "structschema_f.h"
#include "content_f.h"


/* ---------------------------------------------------------------------- */
/* |    InitFormLangue intialise le formulaire pour la saisie des       | */
/* |            valeurs de l'attribut Langue.                           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         InitFormLangue (Document doc, View view, PtrElement PremSel, PtrAttribute AttCour)
#else  /* __STDC__ */
static void         InitFormLangue (doc, view, PremSel, AttCour)
Document            doc;
View                view;
PtrElement          PremSel;
PtrAttribute         AttCour;

#endif /* __STDC__ */
{
   int                 i, nbitem, nbLangues, longueur;
   char                BufMenu[MAX_TXT_LEN];
   char                Chaine[MAX_TXT_LEN];
   char               *ptr;
   Language            langue;
   Name                 valLangue;
   char                Lab[200];
   PtrAttribute         pAttrHerit;
   PtrElement          pElAttr;

   /* c'est l'attribut Langue, on initialise le formulaire Langue */
   valLangue[0] = '\0';
   if (AttCour != NULL)
      if (AttCour->AeAttrText != NULL)
	 strncpy (valLangue, AttCour->AeAttrText->BuContent, MAX_NAME_LENGTH);

   /* cree le formulaire avec les deux boutons Appliquer et Supprimer */
   strcpy (BufMenu, TtaGetMessage (LIB, APPLY));
   i = strlen (BufMenu) + 1;
   strcpy (&BufMenu[i], TtaGetMessage (LIB, DEL));
   TtaNewSheet (NumFormLanguage, 0, 0, 0,
      TtaGetMessage (LIB, LANGUAGE), 2, BufMenu, FALSE, 2, 'L', D_DONE);
   /* construit le selecteur des Langues */
   nbitem = 0;
   ptr = &BufMenu[0];
   langue = '\0';
   nbLangues = TtaGetNumberOfLanguages ();
   for (nbitem = 0; nbitem < nbLangues; nbitem++)
     {
	strcpy (Chaine, TtaGetLanguageName (langue));
	longueur = strlen (Chaine);
	if (longueur > 0)
	  {
	     langue++;
	     strcpy (ptr, Chaine);
	     ptr += longueur + 1;
	  }
     }

   if (nbitem == 0)
     {
	/* pas de langue definie, on cree une simple zone de saisie de texte */
	TtaNewTextForm (NumSelectLanguage, NumFormLanguage,
			TtaGetMessage (LIB, LANGUAGE), 30, 1, FALSE);
	TtaSetTextForm (NumFormLanguage, valLangue);
     }
   else
      /* on cree un selecteur */
     {
	if (nbitem >= 6)
	   longueur = 6;
	else
	   longueur = nbitem;
	TtaNewSelector (NumSelectLanguage, NumFormLanguage,
	      TtaGetMessage (LIB, LANG_OF_EL), nbitem, BufMenu,
			longueur, NULL, TRUE, FALSE);
	/* initialise le selecteur sur sa premiere entree */
	if (valLangue[0] == '\0')
	   TtaSetSelector (NumSelectLanguage, -1, NULL);
	else
	  {
	     i = (int) TtaGetLanguageIdFromName (valLangue);
	     TtaSetSelector (NumSelectLanguage, i, valLangue);
	  }
     }

   /* cherche la valeur heritee de l'attribut Langue */
   strcpy (Lab, TtaGetMessage (LIB, INHERITED_LANG));
   pAttrHerit = GetTypedAttrAncestor (PremSel, 1, NULL, &pElAttr);
   if (pAttrHerit != NULL)
      if (pAttrHerit->AeAttrText != NULL)
	 strcat (Lab, pAttrHerit->AeAttrText->BuContent);

   TtaNewLabel (NumLabelHeritedLanguage, NumFormLanguage, Lab);
   /* affiche le formulaire */
   TtaShowDialogue (NumFormLanguage, TRUE);
}


/* ---------------------------------------------------------------------- */
/* |    MenuValeurs construit la feuille de saisie des valeurs de       | */
/* |            l'attribut defini par la regle pAttr1.                  | */
/* |            Requis indique s'il s'agit d'un attribut obligatoire.   | */
/* |            AttCour donne la valeur courante de l'attribut.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         MenuValeurs (TtAttribute * pAttr1, boolean Requis, PtrAttribute AttCour,
				 PtrDocument pDoc, int view)
#else  /* __STDC__ */
static void         MenuValeurs (pAttr1, Requis, AttCour, pDoc, view)
TtAttribute           *pAttr1;
boolean             Requis;
PtrAttribute         AttCour;
PtrDocument         pDoc;
int                 view;

#endif /* __STDC__ */

{
   int                 i, lgmenu, val;
   int                 form, subform;
   char                Titre[MAX_NAME_LENGTH + 2];
   char                BufMenu[MAX_TXT_LEN];
   Document            doc;

   doc = (Document) IdentDocument (pDoc);
   /* detruit la feuille de dialogue et la recree */
   strcpy (BufMenu, TtaGetMessage (LIB, APPLY));
   i = strlen (BufMenu) + 1;
   strcpy (&BufMenu[i], TtaGetMessage (LIB, DEL));
   if (Requis)
     {
	form = NumMenuAttrRequired;
	if (MandatoryAttrFormExists)
	  {
	     TtaUnmapDialogue (NumMenuAttrRequired);
	     TtaDestroyDialogue (NumMenuAttrRequired);
	  }
	TtaNewForm (NumMenuAttrRequired, TtaGetViewFrame (doc, view), 0, 0,
		 TtaGetMessage (LIB, ATTR), FALSE, 2, 'L', D_DONE);
	MandatoryAttrFormExists = TRUE;
     }
   else
     {
	form = NumMenuAttr;
	if (AttrFormExists)
	  {
	     TtaUnmapDialogue (NumMenuAttr);
	     TtaDestroyDialogue (NumMenuAttr);
	  }
	TtaNewSheet (NumMenuAttr, TtaGetViewFrame (doc, view), 0, 0,
		     TtaGetMessage (LIB, ATTR), 2, BufMenu, FALSE, 2, 'L', D_DONE);
	AttrFormExists = TRUE;
     }

   strncpy (Titre, pAttr1->AttrName, MAX_NAME_LENGTH);
   switch (pAttr1->AttrType)
	 {
	    case AtNumAttr:
	       /* attribut a valeur numerique */
	       subform = form + 1;
	       TtaNewNumberForm (subform, form, Titre, -MAX_INT_ATTR_VAL, MAX_INT_ATTR_VAL, FALSE);
	       TtaAttachForm (subform);
	       if (AttCour == NULL)
		  i = 0;
	       else
		  i = AttCour->AeAttrValue;
	       TtaSetNumberForm (subform, i);
	       break;

	    case AtTextAttr:
	       /* attribut a valeur textuelle */
	       subform = form + 2;
	       TtaNewTextForm (subform, form, Titre, 40, 3, FALSE);
	       TtaAttachForm (subform);
	       if (AttCour == NULL)
		  TtaSetTextForm (subform, "");
	       else if (AttCour->AeAttrText == NULL)
		  TtaSetTextForm (subform, "");
	       else
		  TtaSetTextForm (subform, AttCour->AeAttrText->BuContent);
	       break;

	    case AtEnumAttr:
	       /* attribut a valeurs enumerees */
	       subform = form + 3;
	       /* cree un menu de toutes les valeurs possibles de l'attribut */
	       lgmenu = 0;
	       val = 0;
	       /* boucle sur les valeurs possibles de l'attribut */
	       while (val < pAttr1->AttrNEnumValues)
		 {
		    i = strlen (pAttr1->AttrEnumValue[val]) + 2;	/* for 'B' and '\0' */
		    if (lgmenu + i < MAX_TXT_LEN)
		      {
			 BufMenu[lgmenu] = 'B';
			 strcpy (&BufMenu[lgmenu + 1], pAttr1->AttrEnumValue[val]);
			 val++;
		      }
		    lgmenu += i;
		 }
	       /* cree le menu des valeurs de l'attribut */
	       TtaNewSubmenu (subform, form, 0, Titre, val, BufMenu, NULL, FALSE);
	       TtaAttachForm (subform);
	       /* initialise le menu avec la valeur courante */
	       val = -1;
	       if (AttCour != NULL)
		  val = AttCour->AeAttrValue - 1;
	       TtaSetMenuForm (subform, val);
	       break;

	    case AtReferenceAttr:
	       /* attribut reference, on ne fait rien */
	       break;

	    default:
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* | RetMenuAttrRequis traite le retour du menu de saisie des           | */
/* |    attributs obligatoires.                                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuAttrRequis (int ref, int val, char *txt)
#else  /* __STDC__ */
void                RetMenuAttrRequis (ref, val, txt)
int                 ref;
int                 val;
char               *txt;

#endif /* __STDC__ */
{
   int                 lg;

   switch (ref)
	 {
	    case NumMenuAttrRequired:
	       /* retour de la feuille de dialogue elle-meme */
	       /* on detruit cette feuille de dialogue sauf si c'est */
	       /* un abandon */
	       if (val != 0)
		 {
		    TtaDestroyDialogue (NumMenuAttrRequired);
		    MandatoryAttrFormExists = FALSE;
		 }
	       /* on ne fait rien d'autre : tout a ete fait par les cas */
	       /* suivants */
	       break;
	    case NumMenuAttrNumNeeded:
	       /* zone de saisie de la valeur numerique de l'attribut */
	       if (val >= -MAX_INT_ATTR_VAL || val <= MAX_INT_ATTR_VAL)
		  PtrAttrRequis->AeAttrValue = val;
	       break;
	    case NumMenuAttrTextNeeded:
	       /* zonee de saisie du texte de l'attribut */
	       if (PtrAttrRequis->AeAttrText == NULL)
		  GetBufTexte (&PtrAttrRequis->AeAttrText);
	       else
		  ClearText (PtrAttrRequis->AeAttrText);
	       CopyStringToText (txt, PtrAttrRequis->AeAttrText, &lg);
	       break;
	    case NumMenuAttrEnumNeeded:
	       /* menu des valeurs d'un attribut a valeurs enumerees */
	       val++;
	       PtrAttrRequis->AeAttrValue = val;
	       break;
	    default:
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* |    ConstruitMenuAttrRequis construit le menu de saisie de la valeur| */
/* |            de l'attribut requis defini par la regle pRegleAttr.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ConstruitMenuAttrRequis (PtrAttribute pAttr, PtrDocument pDoc)
#else  /* __STDC__ */
void                ConstruitMenuAttrRequis (pAttr, pDoc)
PtrAttribute         pAttr;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   TtAttribute           *pRegleAttr;

   PtrAttrRequis = pAttr;
   pRegleAttr = &pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1];
   /* toujours lie a la vue 1 du document */
   MenuValeurs (pRegleAttr, TRUE, NULL, pDoc, 1);
   TtaShowDialogue (NumMenuAttrRequired, FALSE);
   TtaWaitShowDialogue ();
}


/* ---------------------------------------------------------------------- */
/* |    TteItemMenuAttr         envoie le message AttrMenu.Pre qui      | */
/* |            indique que l'editeur va mettre dans le menu Attributs  | */
/* |            l'item pour la creation d'un attribut de type           | */
/* |            (pSS, att) pour l'element pEl et retourne               | */
/* |            la reponse de l'application.                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      TteItemMenuAttr (PtrSSchema pSS, int att, PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
static boolean      TteItemMenuAttr (pSS, att, pEl, pDoc)
PtrSSchema        pSS;
int                 att;
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   NotifyAttribute     notifyAttr;
   boolean             OK;

   notifyAttr.event = TteAttrMenu;
   notifyAttr.document = (Document) IdentDocument (pDoc);
   notifyAttr.element = (Element) pEl;
   notifyAttr.attribute = NULL;
   notifyAttr.attributeType.AttrSSchema = (SSchema) pSS;
   notifyAttr.attributeType.AttrTypeNum = att;
   OK = !CallEventAttribute (&notifyAttr, TRUE);
   return OK;
}


/* ---------------------------------------------------------------------- */
/* |  ComposeMenuAttributs construit le menu Attributs et               | */
/* |  retourne le nombre d'attributs mis dans le menu compose'.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 ComposeMenuAttributs (char *BufMenu, PtrDocument pDoc, int ActiveAttr[])
#else  /* __STDC__ */
int                 ComposeMenuAttributs (BufMenu, pDoc, ActiveAttr)
char               *BufMenu;
PtrDocument         pDoc;
int                 ActiveAttr[];

#endif /* __STDC__ */
{
   int                 i;
   PtrDocument         SelDoc;
   PtrElement          PremSel, DerSel, pEl;
   int                 premcar, dercar;
   boolean             selok, nouveau;
   int                 lgmenu;
   PtrSSchema        pSS;
   PtrAttribute         pAttrNouv;
   int                 att, nbentrees;
   SRule              *pRe1;
   TtAttribute           *pAt2;
   char                BufEtoileNom[MAX_NAME_LENGTH + 1];
   PtrSSchema        pSchExt;

   nbentrees = 0;
   /* demande quelle est la selection courante */
   selok = SelEditeur (&SelDoc, &PremSel, &DerSel, &premcar, &dercar);
   if (selok && SelDoc == pDoc)
      /* il y a une selection et elle est dans le document traite' */
     {
	/* cherche les attributs definis dans les differents schemas de */
	/* structure */
	pSS = NULL;
	pEl = PremSel;
	/* cherche tous les schemas de structure utilises par les ascendants */
	/* du premier element selectionne' (lui-meme compris) */
	while (pEl != NULL)
	  {
	     pSS = pEl->ElStructSchema;	/* schema de struct de l'element courant */
	     /* on parcourt toutes les extensions de schema de ce schema */
	     do
	       {
		  /* on a deja traite' ce schema de structure ? */
		  nouveau = TRUE;
		  for (i = 1; i <= nbentrees; i++)	/* parcourt la table */
		     if (pSS == AttrStruct[i - 1])	/* deja dans la table */
			nouveau = FALSE;
		  if (nouveau)
		     /* l'element utilise un schema de structure pas encore */
		     /* rencontre' */
		     /* met tous les attributs globaux de ce schema dans la table */
		    {
		       att = 0;
		       while (att < pSS->SsNAttributes && nbentrees < LgMaxAttributeMenu
			      && nbentrees < MAX_ENTRY - 1)
			 {
			    att++;
			    /* on saute les attributs locaux */
			    if (pSS->SsAttribute[att - 1].AttrGlobal)
			       /* on saute les attributs ayant l'exception Invisible */
			       if (!AttrHasException (ExcInvisible, att, pSS))
				  /* on saute l'attribut Langue, sauf la 1ere fois */
				  if (nbentrees == 0 || att != 1)
				     if (TteItemMenuAttr (pSS, att, PremSel, SelDoc))
				       {
					  nbentrees++;
					  /* conserve le schema de structure et le numero */
					  /* d'attribut de cette nouvelle entree du menu */
					  AttrStruct[nbentrees - 1] = pSS;
					  AttrNumero[nbentrees - 1] = att;
					  AttrOblig[nbentrees - 1] = FALSE;
				       }
			 }
		    }
		  /* passe a l'extension de schema suivante */
		  pSS = pSS->SsNextExtens;
	       }
	     while (pSS != NULL);
	     pEl = pEl->ElParent;
	     /* passe a l'element ascendant */
	  }

	/* cherche les attributs locaux du premier element selectionne' */
	pSS = PremSel->ElStructSchema;
	pRe1 = &pSS->SsRule[PremSel->ElTypeNumber - 1];
	pSchExt = SelDoc->DocSSchema;
	do
	  {
	     if (pRe1 != NULL)
		/* prend les attributs locaux definis dans cette regle */
		for (att = 1; att <= pRe1->SrNLocalAttrs; att++)
		   if (nbentrees < LgMaxAttributeMenu && nbentrees < MAX_ENTRY - 1)
		      if (!AttrHasException (ExcInvisible, pRe1->SrLocalAttr[att - 1], pSS))
			 if (TteItemMenuAttr (pSS, pRe1->SrLocalAttr[att - 1], PremSel,
					      SelDoc))
			   {
			      nbentrees++;
			      /* conserve le schema de structure et le numero */
			      /* d'attribut de cette nouvelle entree du menu */
			      AttrStruct[nbentrees - 1] = pSS;
			      AttrNumero[nbentrees - 1] = pRe1->SrLocalAttr[att - 1];
			      AttrOblig[nbentrees - 1] = pRe1->SrRequiredAttr[att - 1];
			   }
	     /* passe a l'extension suivante du schema du document */
	     pSchExt = pSchExt->SsNextExtens;
	     /* cherche dans cette extension de schema la regle d'extension pour */
	     /* le premier element selectionne' */
	     if (pSchExt != NULL)
	       {
		  pSS = pSchExt;
		  pRe1 = ExtensionRule (PremSel->ElStructSchema, PremSel->ElTypeNumber,
				      pSchExt);
	       }
	  }
	while (pSchExt != NULL);

	/* la table contient tous les attributs applicables aux elements */
	/* selectionnes */
	GetAttr (&pAttrNouv);
	if (nbentrees > 0)
	  {
	     lgmenu = 0;
	     /* met les noms des attributs de la table dans le menu */
	     for (att = 0; att < nbentrees; att++)
	       {
		  pAt2 = &AttrStruct[att]->SsAttribute[AttrNumero[att] - 1];

		  pAttrNouv->AeAttrSSchema = AttrStruct[att];
		  pAttrNouv->AeAttrNum = AttrNumero[att];
		  pAttrNouv->AeDefAttr = FALSE;
		  if (pAt2->AttrType == AtEnumAttr && pAt2->AttrNEnumValues == 1)
		     /* attribut enumere' a une seule valeur (attribut booleen) */
		     sprintf (BufEtoileNom, "T%s", pAt2->AttrName);
		  else
		     sprintf (BufEtoileNom, "T%s...", pAt2->AttrName);
		  /* met une etoile devant le nom de l'attribut s'il a une */
		  /* valeur pour la selection courante */
		  if (AttributeValue (PremSel, pAttrNouv) != NULL)
		     ActiveAttr[att] = 1;
		  else
		     ActiveAttr[att] = 0;
		  i = strlen (BufEtoileNom) + 1;
		  if (lgmenu + i < MAX_TXT_LEN)
		     strcpy (&BufMenu[lgmenu], BufEtoileNom);
		  lgmenu += i;
	       }
	  }
	DeleteAttribute (NULL, pAttrNouv);
     }
   return nbentrees;
}


/* ---------------------------------------------------------------------- */
/* |  MajMenuAttr                                                       | */
/* |  Met a jour le menu des Attributs                                  | */
/* |    - de toutes les frames ouvertes du document pDoc.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                MajMenuAttr (PtrDocument pDoc)
#else  /* __STDC__ */
void                MajMenuAttr (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 NbItemAttr, i;
   char                BufMenuAttr[MAX_TXT_LEN];
   int                 vue, menu, menuID;
   int                 frame, ref;
   Document            document;
   Menu_Ctl           *ptrmenu;

   /* Compose le menu des attributs */
   if (pDoc == SelectedDocument)
      NbItemAttr = ComposeMenuAttributs (BufMenuAttr, pDoc, ActiveAttr);
   else
      NbItemAttr = 0;

   document = (Document) IdentDocument (pDoc);
   /* Traite toutes les vues de l'arbre principal */
   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
     {
	frame = pDoc->DocViewFrame[vue - 1];
	if (frame != 0 && FrameTable[frame].MenuAttr != -1)
	  {
	     menuID = FrameTable[frame].MenuAttr;
	     menu = FindMenu (frame, menuID, &ptrmenu) - 1;
	     ref = (menu * MAX_FRAME) + frame + MAX_LocalMenu;
	     if (pDoc != SelectedDocument || NbItemAttr == 0)
	       {
		  /* le menu Attributs contient au moins un attribut */
		  TtaSetMenuOff (document, vue, menuID);
		  TtaDestroyDialogue (ref);
	       }
	     else
	       {
		  TtaNewPulldown (ref, FrameTable[frame].WdMenus[menu], NULL,
				  NbItemAttr, BufMenuAttr, NULL);
		  /* marque les attributs actifs */
		  for (i = 0; i < NbItemAttr; i++)
		     if (ActiveAttr[i] == 1)
			TtaSetToggleMenu (ref, i, TRUE);
		  TtaSetMenuOn (document, vue, menuID);
	       }
	  }
     }

   /* Traite toutes les vues des arbres associes */
   for (vue = 1; vue <= MAX_ASSOC_DOC; vue++)
     {
	frame = pDoc->DocAssocFrame[vue - 1];
	if (frame != 0 && FrameTable[frame].MenuAttr != -1)
	  {
	     menuID = FrameTable[frame].MenuAttr;
	     menu = FindMenu (frame, menuID, &ptrmenu) - 1;
	     ref = (menu * MAX_FRAME) + frame + MAX_LocalMenu;
	     if (pDoc != SelectedDocument || NbItemAttr == 0)
	       {
		  /* le menu Attributs contient au moins un attribut */
		  TtaSetMenuOff (document, vue, menu);
		  TtaDestroyDialogue (ref);
	       }
	     else
	       {
		  TtaNewPulldown (ref, FrameTable[frame].WdMenus[menu - 1], NULL,
				  NbItemAttr, BufMenuAttr, NULL);
		  /* marque les attributs actifs */
		  for (i = 0; i < NbItemAttr; i++)
		     if (ActiveAttr[i] == 1)
			TtaSetToggleMenu (ref, i, TRUE);
		  TtaSetMenuOn (document, vue, menu);
	       }
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* | RetMenuValAttr traite le retour du formulaire de saisie des        | */
/* |    valeurs d'attribut : applique aux elements selectionne's les    | */
/* |    attributs choisis par l'utilisateur.                            | */
/* |    ref: reference de l'element de dialogue dont on traite le retour| */
/* |    valmenu: valeur choisie ou saisie dans cet element de dialogue. | */
/* |    valtexte: pointeur sur le texte saisi dans cet element de dialogue */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuValAttr (int ref, int valmenu, char *valtexte)
#else  /* __STDC__ */
void                RetMenuValAttr (ref, valmenu, valtexte)
int                 ref;
int                 valmenu;
char               *valtexte;

#endif /* __STDC__ */
{
   PtrDocument         SelDoc;
   PtrElement          PremSel, DerSel;
   int                 premcar, dercar;
   PtrAttribute         pAttrNouv;
   int                 lg;

   switch (ref)
	 {
	    case NumMenuAttrNumber:
	       /* valeur d'un attribut numerique */
	       ValeurNumAttribut = valmenu;
	       break;
	    case NumMenuAttrText:
	       /* valeur d'un attribut textuel */
	       strncpy (ValeurTexteAttribut, valtexte, LgMaxAttrTexte);
	       break;
	    case NumMenuAttrEnum:
	       /* numero de la valeur d'un attribut enumere' */
	       ValeurNumAttribut = valmenu + 1;
	       break;
	    case NumMenuAttr:
	       /* retour de la feuille de dialogue elle-meme */
	       if (valmenu == 0)
		  /* fermeture de la feuille de dialogue */
		 {
		    /* on detruit la feuille de dialogue */
		    TtaDestroyDialogue (NumMenuAttr);
		    AttrFormExists = FALSE;
		 }
	       else
		  /* ce n'est pas une simple fermeture de la feuille de dialogue */
		  /* demande quelle est la selection courante */
	       if (SelEditeur (&SelDoc, &PremSel, &DerSel, &premcar, &dercar))
		  /* il y a bien une selection */
		 {
		    GetAttr (&pAttrNouv);
		    if (SchAttributCourant != NULL)
		      {
			 pAttrNouv->AeAttrSSchema = SchAttributCourant;
			 pAttrNouv->AeAttrNum = NumAttributCourant;
			 pAttrNouv->AeDefAttr = FALSE;
			 pAttrNouv->AeAttrType = SchAttributCourant->
			    SsAttribute[NumAttributCourant - 1].AttrType;

			 switch (pAttrNouv->AeAttrType)
			       {
				  case AtNumAttr:
				     if (valmenu == 2)
					/* Supprimer l'attribut */
					pAttrNouv->AeAttrValue = MAX_INT_ATTR_VAL + 1;
				     else
					/* la valeur saisie devient la valeur courante */
					pAttrNouv->AeAttrValue = ValeurNumAttribut;
				     /* applique les attributs a la partie selectionnee */
				     AttachAttrToRange (pAttrNouv, dercar, premcar, DerSel, PremSel,
						    SelDoc);
				     break;

				  case AtTextAttr:
				     if (valmenu == 2)
					/* suppression de l'attribut */
					pAttrNouv->AeAttrText = NULL;
				     else
				       {
					  /* la valeur saisie devient la valeur courante */
					  if (pAttrNouv->AeAttrText == NULL)
					     GetBufTexte (&pAttrNouv->AeAttrText);
					  else
					     ClearText (pAttrNouv->AeAttrText);
					  CopyStringToText (ValeurTexteAttribut,
					      pAttrNouv->AeAttrText, &lg);
				       }
				     /* applique les attributs a la partie selectionnee */
				     AttachAttrToRange (pAttrNouv, dercar, premcar, DerSel, PremSel,
						    SelDoc);
				     break;

				  case AtReferenceAttr:

				     break;

				  case AtEnumAttr:
				     if (valmenu == 2)
					/* suppression de l'attribut */
					pAttrNouv->AeAttrValue = 0;
				     else
					/* la valeur choisie devient la valeur courante */
					pAttrNouv->AeAttrValue = ValeurNumAttribut;
				     /* applique les attributs a la partie selectionnee */
				     AttachAttrToRange (pAttrNouv, dercar, premcar, DerSel, PremSel,
						    SelDoc);
				     break;

				  default:
				     break;
			       }
			 MajMenuAttr (SelDoc);
		      }
		    DeleteAttribute (NULL, pAttrNouv);
		 }
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |    RetMenuAttributs traite les retours du menu 'Attributs':        | */
/* |    cree un formulaire pour saisir la valeur de l'attribut choisi.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuAttributs (int refmenu, int att, int frame)
#else  /* __STDC__ */
void                RetMenuAttributs (refmenu, att, frame)
int                 refmenu;
int                 att;
int                 frame;

#endif /* __STDC__ */
{
   TtAttribute           *pAttr;
   PtrAttribute         pAttrNouv, AttCour;
   PtrDocument         SelDoc;
   PtrElement          PremSel, DerSel, AssocCreated;
   int                 premcar, dercar;
   PtrReference        Ref;
   Document            doc;
   View                view;

   VueDeFenetre (frame, &doc, &view);

   if (att >= 0)
      if (SelEditeur (&SelDoc, &PremSel, &DerSel, &premcar, &dercar))
	{
	   GetAttr (&pAttrNouv);
	   pAttrNouv->AeAttrSSchema = AttrStruct[att];
	   pAttrNouv->AeAttrNum = AttrNumero[att];
	   pAttrNouv->AeDefAttr = FALSE;
	   pAttr = &AttrStruct[att]->SsAttribute[AttrNumero[att] - 1];
	   pAttrNouv->AeAttrType = pAttr->AttrType;
	   if (pAttr->AttrType == AtReferenceAttr)
	     {
		AssocCreated = NULL;
		/* attache un bloc reference a l'attribut */
		GetReference (&Ref);
		pAttrNouv->AeAttrReference = Ref;
		pAttrNouv->AeAttrReference->RdElement = NULL;
		pAttrNouv->AeAttrReference->RdAttribute = pAttrNouv;
		/* demande a l'utilisateur l'element reference' */
		if (LinkReference (PremSel, pAttrNouv, SelDoc, &AssocCreated))
		  if (ThotLocalActions[T_checkextens]!=NULL)
		    (*ThotLocalActions[T_checkextens])
		      (pAttrNouv, PremSel, DerSel, FALSE);
		if (AssocCreated != NULL)
		  {
		     CreeTousPaves (AssocCreated, SelDoc);
		     AbstractImageUpdated (SelDoc);
		  }
		/* applique l'attribut a la partie selectionnee */
		AttachAttrToRange (pAttrNouv, dercar, premcar, DerSel, PremSel,
			       SelDoc);
		MajMenuAttr (SelDoc);
	     }
	   else
	     {
		/* cherche la valeur de cet attribut pour le premier element */
		/* selectionne' */
		AttCour = AttributeValue (PremSel, pAttrNouv);
		if (pAttrNouv->AeAttrNum == 1)
		  {
		     InitFormLangue (doc, view, PremSel, AttCour);
		     /* memorise l'attribut concerne' par le formulaire */
		     SchAttributCourant = pAttrNouv->AeAttrSSchema;
		     NumAttributCourant = 1;
		  }
		else if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 1)
		   /* attribut enumere' a une seule valeur(attribut booleen) */
		  {
		     if (AttCour == NULL)
			/* le premier element selectionne' n'a pas cet */
			/* attribut. On le lui met */
			pAttrNouv->AeAttrValue = 1;
		     else
			/* suppression de l'attribut */
			pAttrNouv->AeAttrValue = 0;
		     /* applique l'operation a la partie selectionnee */
		     AttachAttrToRange (pAttrNouv, dercar, premcar, DerSel, PremSel,
				    SelDoc);
		  }
		else
		  {
		     /* construit le formulaire de saisie de la valeur de */
		     /* l'attribut */
		     MenuValeurs (pAttr, FALSE, AttCour, SelDoc, view);
		     /* memorise l'attribut concerne' par le formulaire */
		     SchAttributCourant = AttrStruct[att];
		     NumAttributCourant = AttrNumero[att];
		     /* restaure l'etat courant du toggle */
		     if (ActiveAttr[att] == 0)
			TtaSetToggleMenu (refmenu, att, FALSE);
		     else
			TtaSetToggleMenu (refmenu, att, TRUE);
		     /* et memorise l'attribut en cours de traitement */
		     CurrentAttr = att;
		     /* affiche le formulaire */
		     TtaShowDialogue (NumMenuAttr, TRUE);
		  }
		DeleteAttribute (NULL, pAttrNouv);
	     }
	}
}

/* ---------------------------------------------------------------------- */
/* | RetMenuLangue traite les retours du formulaire Langue.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuLangue (int ref, int val, char *txt)
#else  /* __STDC__ */
void                RetMenuLangue (ref, val, txt)
int                 ref;
int                 val;
char               *txt;

#endif /* __STDC__ */
{
   switch (ref)
	 {
	    case NumSelectLanguage:
	       /* retour de la langue choise par l'utilisateur */
	       if (txt == NULL)
		  ValeurTexteAttribut[0] = '\0';
	       else
		  strncpy (ValeurTexteAttribut, txt, LgMaxAttrTexte);
	       break;
	    case NumFormLanguage:
	       /* retour du formulaire lui-meme */

	       switch (val)
		     {
			case 0:
			   /* abandon du formulaire */
			   break;
			case 1:
			case 2:
			   /* appliquer la nouvelle valeur */
			   RetMenuValAttr (NumMenuAttr, val, NULL);
			   break;
		     }
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* | AttributeMenuLoadResources connecte les fonctions d'edition        | */
/* ---------------------------------------------------------------------- */
void                AttributeMenuLoadResources ()
{
   if (ThotLocalActions[T_chattr] == NULL)
     {
	/* Connecte les actions de selection */
	TteConnectAction (T_chattr, (Proc) MajMenuAttr);
	TteConnectAction (T_rattr, (Proc) RetMenuAttributs);
	TteConnectAction (T_rattrval, (Proc) RetMenuValAttr);
	TteConnectAction (T_rattrlang, (Proc) RetMenuLangue);
	TteConnectAction (T_attrreq, (Proc) ConstruitMenuAttrRequis);
	TteConnectAction (T_rattrreq, (Proc) RetMenuAttrRequis);
     }
}
