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
static PtrSSchema   AttrStruct[LgMaxAttributeMenu];
static int          AttrNumber[LgMaxAttributeMenu];
static boolean      AttrOblig[LgMaxAttributeMenu];

/* l'attribut concerne' par le formulaire de saisie affiche' */
static PtrSSchema   SchCurrentAttr = NULL;
static int          NumCurrentAttr = 0;
static int          ActiveAttr[100];
static int          CurrentAttr;

/* valeur de retour du formulaire de saisie */
static int          NumAttrValue;

#define LgMaxAttrText 500
static char         TextAttrValue[LgMaxAttrText];

/* contexte des attributs requis */
static PtrAttribute  PtrReqAttr;

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
static void         InitFormLanguage (Document doc, View view, PtrElement firstSel, PtrAttribute CurrAttr)
#else  /* __STDC__ */
static void         InitFormLanguage (doc, view, firstSel, CurrAttr)
Document            doc;
View                view;
PtrElement          firstSel;
PtrAttribute        CurrAttr;

#endif /* __STDC__ */
{
   int                 i, nbItem, nbLanguages, length;
   char                BufMenu[MAX_TXT_LEN];
   char                string[MAX_TXT_LEN];
   char                *ptr;
   Language            language;
   Name                languageValue;
   char                Lab[200];
   PtrAttribute        pHeritAttr;
   PtrElement          pElAttr;

   /* c'est l'attribut Langue, on initialise le formulaire Langue */
   languageValue[0] = '\0';
   if (CurrAttr != NULL)
      if (CurrAttr->AeAttrText != NULL)
	 strncpy (languageValue, CurrAttr->AeAttrText->BuContent, MAX_NAME_LENGTH);

   /* cree le formulaire avec les deux boutons Appliquer et Supprimer */
   strcpy (BufMenu, TtaGetMessage (LIB, APPLY));
   i = strlen (BufMenu) + 1;
   strcpy (&BufMenu[i], TtaGetMessage (LIB, DEL));
   TtaNewSheet (NumFormLanguage, 0, 0, 0,
      TtaGetMessage (LIB, LANGUAGE), 2, BufMenu, FALSE, 2, 'L', D_DONE);
   /* construit le selecteur des Langues */
   nbItem = 0;
   ptr = &BufMenu[0];
   language = '\0';
   nbLanguages = TtaGetNumberOfLanguages ();
   for (nbItem = 0; nbItem < nbLanguages; nbItem++)
     {
	strcpy (string, TtaGetLanguageName (language));
	length = strlen (string);
	if (length > 0)
	  {
	     language++;
	     strcpy (ptr, string);
	     ptr += length + 1;
	  }
     }

   if (nbItem == 0)
     {
	/* pas de langue definie, on cree une simple zone de saisie de texte */
	TtaNewTextForm (NumSelectLanguage, NumFormLanguage,
			TtaGetMessage (LIB, LANGUAGE), 30, 1, FALSE);
	TtaSetTextForm (NumFormLanguage, languageValue);
     }
   else
      /* on cree un selecteur */
     {
	if (nbItem >= 6)
	   length = 6;
	else
	   length = nbItem;
	TtaNewSelector (NumSelectLanguage, NumFormLanguage,
	      TtaGetMessage (LIB, LANG_OF_EL), nbItem, BufMenu,
			length, NULL, TRUE, FALSE);
	/* initialise le selecteur sur sa premiere entree */
	if (languageValue[0] == '\0')
	   TtaSetSelector (NumSelectLanguage, -1, NULL);
	else
	  {
	     i = (int) TtaGetLanguageIdFromName (languageValue);
	     TtaSetSelector (NumSelectLanguage, i, languageValue);
	  }
     }

   /* cherche la valeur heritee de l'attribut Langue */
   strcpy (Lab, TtaGetMessage (LIB, INHERITED_LANG));
   pHeritAttr = GetTypedAttrAncestor (firstSel, 1, NULL, &pElAttr);
   if (pHeritAttr != NULL)
      if (pHeritAttr->AeAttrText != NULL)
	 strcat (Lab, pHeritAttr->AeAttrText->BuContent);

   TtaNewLabel (NumLabelHeritedLanguage, NumFormLanguage, Lab);
   /* affiche le formulaire */
   TtaShowDialogue (NumFormLanguage, TRUE);
}


/* ---------------------------------------------------------------------- */
/* |    MenuValues construit la feuille de saisie des valeurs de       | */
/* |            l'attribut defini par la regle pAttr1.                  | */
/* |            required indique s'il s'agit d'un attribut obligatoire.   | */
/* |            CurrAttr donne la valeur courante de l'attribut.         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         MenuValues (TtAttribute * pAttr1, boolean required, PtrAttribute CurrAttr,
				 PtrDocument pDoc, int view)
#else  /* __STDC__ */
static void         MenuValues (pAttr1, required, CurrAttr, pDoc, view)
TtAttribute           *pAttr1;
boolean             required;
PtrAttribute         CurrAttr;
PtrDocument         pDoc;
int                 view;

#endif /* __STDC__ */

{
   int                 i, lgmenu, val;
   int                 form, subform;
   char                title[MAX_NAME_LENGTH + 2];
   char                BufMenu[MAX_TXT_LEN];
   Document            doc;

   doc = (Document) IdentDocument (pDoc);
   /* detruit la feuille de dialogue et la recree */
   strcpy (BufMenu, TtaGetMessage (LIB, APPLY));
   i = strlen (BufMenu) + 1;
   strcpy (&BufMenu[i], TtaGetMessage (LIB, DEL));
   if (required)
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

   strncpy (title, pAttr1->AttrName, MAX_NAME_LENGTH);
   switch (pAttr1->AttrType)
	 {
	    case AtNumAttr:
	       /* attribut a valeur numerique */
	       subform = form + 1;
	       TtaNewNumberForm (subform, form, title, -MAX_INT_ATTR_VAL, MAX_INT_ATTR_VAL, FALSE);
	       TtaAttachForm (subform);
	       if (CurrAttr == NULL)
		  i = 0;
	       else
		  i = CurrAttr->AeAttrValue;
	       TtaSetNumberForm (subform, i);
	       break;

	    case AtTextAttr:
	       /* attribut a valeur textuelle */
	       subform = form + 2;
	       TtaNewTextForm (subform, form, title, 40, 3, FALSE);
	       TtaAttachForm (subform);
	       if (CurrAttr == NULL)
		  TtaSetTextForm (subform, "");
	       else if (CurrAttr->AeAttrText == NULL)
		  TtaSetTextForm (subform, "");
	       else
		  TtaSetTextForm (subform, CurrAttr->AeAttrText->BuContent);
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
	       TtaNewSubmenu (subform, form, 0, title, val, BufMenu, NULL, FALSE);
	       TtaAttachForm (subform);
	       /* initialise le menu avec la valeur courante */
	       val = -1;
	       if (CurrAttr != NULL)
		  val = CurrAttr->AeAttrValue - 1;
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
   int                 length;

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
		  PtrReqAttr->AeAttrValue = val;
	       break;
	    case NumMenuAttrTextNeeded:
	       /* zonee de saisie du texte de l'attribut */
	       if (PtrReqAttr->AeAttrText == NULL)
		  GetBufTexte (&PtrReqAttr->AeAttrText);
	       else
		  ClearText (PtrReqAttr->AeAttrText);
	       CopyStringToText (txt, PtrReqAttr->AeAttrText, &length);
	       break;
	    case NumMenuAttrEnumNeeded:
	       /* menu des valeurs d'un attribut a valeurs enumerees */
	       val++;
	       PtrReqAttr->AeAttrValue = val;
	       break;
	    default:
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* |    ConstruitMenuAttrRequis construit le menu de saisie de la valeur| */
/* |            de l'attribut requis defini par la regle pRuleAttr.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ConstruitMenuAttrRequis (PtrAttribute pAttr, PtrDocument pDoc)
#else  /* __STDC__ */
void                ConstruitMenuAttrRequis (pAttr, pDoc)
PtrAttribute         pAttr;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   TtAttribute           *pRuleAttr;

   PtrReqAttr = pAttr;
   pRuleAttr = &pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1];
   /* toujours lie a la vue 1 du document */
   MenuValues (pRuleAttr, TRUE, NULL, pDoc, 1);
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
   PtrElement          firstSel, lastSel, pEl;
   int                 firstChar, lastChar;
   boolean             selok, nouveau;
   int                 lgmenu;
   PtrSSchema        pSS;
   PtrAttribute         pAttrNew;
   int                 att, nbOfEntries;
   SRule              *pRe1;
   TtAttribute           *pAt2;
   char                tempBuffer[MAX_NAME_LENGTH + 1];
   PtrSSchema        pSchExt;

   nbOfEntries = 0;
   /* demande quelle est la selection courante */
   selok = SelEditeur (&SelDoc, &firstSel, &lastSel, &firstChar, &lastChar);
   if (selok && SelDoc == pDoc)
      /* il y a une selection et elle est dans le document traite' */
     {
	/* cherche les attributs definis dans les differents schemas de */
	/* structure */
	pSS = NULL;
	pEl = firstSel;
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
		  for (i = 1; i <= nbOfEntries; i++)	/* parcourt la table */
		     if (pSS == AttrStruct[i - 1])	/* deja dans la table */
			nouveau = FALSE;
		  if (nouveau)
		     /* l'element utilise un schema de structure pas encore */
		     /* rencontre' */
		     /* met tous les attributs globaux de ce schema dans la table */
		    {
		       att = 0;
		       while (att < pSS->SsNAttributes && nbOfEntries < LgMaxAttributeMenu
			      && nbOfEntries < MAX_ENTRY - 1)
			 {
			    att++;
			    /* on saute les attributs locaux */
			    if (pSS->SsAttribute[att - 1].AttrGlobal)
			       /* on saute les attributs ayant l'exception Invisible */
			       if (!AttrHasException (ExcInvisible, att, pSS))
				  /* on saute l'attribut Langue, sauf la 1ere fois */
				  if (nbOfEntries == 0 || att != 1)
				     if (TteItemMenuAttr (pSS, att, firstSel, SelDoc))
				       {
					  nbOfEntries++;
					  /* conserve le schema de structure et le numero */
					  /* d'attribut de cette nouvelle entree du menu */
					  AttrStruct[nbOfEntries - 1] = pSS;
					  AttrNumber[nbOfEntries - 1] = att;
					  AttrOblig[nbOfEntries - 1] = FALSE;
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
	pSS = firstSel->ElStructSchema;
	pRe1 = &pSS->SsRule[firstSel->ElTypeNumber - 1];
	pSchExt = SelDoc->DocSSchema;
	do
	  {
	     if (pRe1 != NULL)
		/* prend les attributs locaux definis dans cette regle */
		for (att = 1; att <= pRe1->SrNLocalAttrs; att++)
		   if (nbOfEntries < LgMaxAttributeMenu && nbOfEntries < MAX_ENTRY - 1)
		      if (!AttrHasException (ExcInvisible, pRe1->SrLocalAttr[att - 1], pSS))
			 if (TteItemMenuAttr (pSS, pRe1->SrLocalAttr[att - 1], firstSel,
					      SelDoc))
			   {
			      nbOfEntries++;
			      /* conserve le schema de structure et le numero */
			      /* d'attribut de cette nouvelle entree du menu */
			      AttrStruct[nbOfEntries - 1] = pSS;
			      AttrNumber[nbOfEntries - 1] = pRe1->SrLocalAttr[att - 1];
			      AttrOblig[nbOfEntries - 1] = pRe1->SrRequiredAttr[att - 1];
			   }
	     /* passe a l'extension suivante du schema du document */
	     pSchExt = pSchExt->SsNextExtens;
	     /* cherche dans cette extension de schema la regle d'extension pour */
	     /* le premier element selectionne' */
	     if (pSchExt != NULL)
	       {
		  pSS = pSchExt;
		  pRe1 = ExtensionRule (firstSel->ElStructSchema, firstSel->ElTypeNumber,
				      pSchExt);
	       }
	  }
	while (pSchExt != NULL);

	/* la table contient tous les attributs applicables aux elements */
	/* selectionnes */
	GetAttr (&pAttrNew);
	if (nbOfEntries > 0)
	  {
	     lgmenu = 0;
	     /* met les noms des attributs de la table dans le menu */
	     for (att = 0; att < nbOfEntries; att++)
	       {
		  pAt2 = &AttrStruct[att]->SsAttribute[AttrNumber[att] - 1];

		  pAttrNew->AeAttrSSchema = AttrStruct[att];
		  pAttrNew->AeAttrNum = AttrNumber[att];
		  pAttrNew->AeDefAttr = FALSE;
		  if (pAt2->AttrType == AtEnumAttr && pAt2->AttrNEnumValues == 1)
		     /* attribut enumere' a une seule valeur (attribut booleen) */
		     sprintf (tempBuffer, "T%s", pAt2->AttrName);
		  else
		     sprintf (tempBuffer, "T%s...", pAt2->AttrName);
		  /* met une etoile devant le nom de l'attribut s'il a une */
		  /* valeur pour la selection courante */
		  if (AttributeValue (firstSel, pAttrNew) != NULL)
		     ActiveAttr[att] = 1;
		  else
		     ActiveAttr[att] = 0;
		  i = strlen (tempBuffer) + 1;
		  if (lgmenu + i < MAX_TXT_LEN)
		     strcpy (&BufMenu[lgmenu], tempBuffer);
		  lgmenu += i;
	       }
	  }
	DeleteAttribute (NULL, pAttrNew);
     }
   return nbOfEntries;
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
   int                 view, menu, menuID;
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
   for (view = 1; view <= MAX_VIEW_DOC; view++)
     {
	frame = pDoc->DocViewFrame[view - 1];
	if (frame != 0 && FrameTable[frame].MenuAttr != -1)
	  {
	     menuID = FrameTable[frame].MenuAttr;
	     menu = FindMenu (frame, menuID, &ptrmenu) - 1;
	     ref = (menu * MAX_FRAME) + frame + MAX_LocalMenu;
	     if (pDoc != SelectedDocument || NbItemAttr == 0)
	       {
		  /* le menu Attributs contient au moins un attribut */
		  TtaSetMenuOff (document, view, menuID);
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
		  TtaSetMenuOn (document, view, menuID);
	       }
	  }
     }

   /* Traite toutes les vues des arbres associes */
   for (view = 1; view <= MAX_ASSOC_DOC; view++)
     {
	frame = pDoc->DocAssocFrame[view - 1];
	if (frame != 0 && FrameTable[frame].MenuAttr != -1)
	  {
	     menuID = FrameTable[frame].MenuAttr;
	     menu = FindMenu (frame, menuID, &ptrmenu) - 1;
	     ref = (menu * MAX_FRAME) + frame + MAX_LocalMenu;
	     if (pDoc != SelectedDocument || NbItemAttr == 0)
	       {
		  /* le menu Attributs contient au moins un attribut */
		  TtaSetMenuOff (document, view, menu);
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
		  TtaSetMenuOn (document, view, menu);
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
void                RetMenuValAttr (int ref, int valmenu, char *valtext)
#else  /* __STDC__ */
void                RetMenuValAttr (ref, valmenu, valtext)
int                 ref;
int                 valmenu;
char               *valtext;

#endif /* __STDC__ */
{
   PtrDocument         SelDoc;
   PtrElement          firstSel, lastSel;
   int                 firstChar, lastChar;
   PtrAttribute        pAttrNew;
   int                 lg;

   switch (ref)
	 {
	    case NumMenuAttrNumber:
	       /* valeur d'un attribut numerique */
	       NumAttrValue = valmenu;
	       break;
	    case NumMenuAttrText:
	       /* valeur d'un attribut textuel */
	       strncpy (TextAttrValue, valtext, LgMaxAttrText);
	       break;
	    case NumMenuAttrEnum:
	       /* numero de la valeur d'un attribut enumere' */
	       NumAttrValue = valmenu + 1;
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
	       if (SelEditeur (&SelDoc, &firstSel, &lastSel, &firstChar, &lastChar))
		  /* il y a bien une selection */
		 {
		    GetAttr (&pAttrNew);
		    if (SchCurrentAttr != NULL)
		      {
			 pAttrNew->AeAttrSSchema = SchCurrentAttr;
			 pAttrNew->AeAttrNum = NumCurrentAttr;
			 pAttrNew->AeDefAttr = FALSE;
			 pAttrNew->AeAttrType = SchCurrentAttr->
			    SsAttribute[NumCurrentAttr - 1].AttrType;

			 switch (pAttrNew->AeAttrType)
			       {
				  case AtNumAttr:
				     if (valmenu == 2)
					/* Supprimer l'attribut */
					pAttrNew->AeAttrValue = MAX_INT_ATTR_VAL + 1;
				     else
					/* la valeur saisie devient la valeur courante */
					pAttrNew->AeAttrValue = NumAttrValue;
				     /* applique les attributs a la partie selectionnee */
				     AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel, firstSel,
						    SelDoc);
				     break;

				  case AtTextAttr:
				     if (valmenu == 2)
					/* suppression de l'attribut */
					pAttrNew->AeAttrText = NULL;
				     else
				       {
					  /* la valeur saisie devient la valeur courante */
					  if (pAttrNew->AeAttrText == NULL)
					     GetBufTexte (&pAttrNew->AeAttrText);
					  else
					     ClearText (pAttrNew->AeAttrText);
					  CopyStringToText (TextAttrValue,
					      pAttrNew->AeAttrText, &lg);
				       }
				     /* applique les attributs a la partie selectionnee */
				     AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel, firstSel,
						    SelDoc);
				     break;

				  case AtReferenceAttr:

				     break;

				  case AtEnumAttr:
				     if (valmenu == 2)
					/* suppression de l'attribut */
					pAttrNew->AeAttrValue = 0;
				     else
					/* la valeur choisie devient la valeur courante */
					pAttrNew->AeAttrValue = NumAttrValue;
				     /* applique les attributs a la partie selectionnee */
				     AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel, firstSel,
						    SelDoc);
				     break;

				  default:
				     break;
			       }
			 MajMenuAttr (SelDoc);
		      }
		    DeleteAttribute (NULL, pAttrNew);
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
   PtrAttribute         pAttrNew, CurrAttr;
   PtrDocument         SelDoc;
   PtrElement          firstSel, lastSel, AssocCreated;
   int                 firstChar, lastChar;
   PtrReference        Ref;
   Document            doc;
   View                view;

   VueDeFenetre (frame, &doc, &view);

   if (att >= 0)
      if (SelEditeur (&SelDoc, &firstSel, &lastSel, &firstChar, &lastChar))
	{
	   GetAttr (&pAttrNew);
	   pAttrNew->AeAttrSSchema = AttrStruct[att];
	   pAttrNew->AeAttrNum = AttrNumber[att];
	   pAttrNew->AeDefAttr = FALSE;
	   pAttr = &AttrStruct[att]->SsAttribute[AttrNumber[att] - 1];
	   pAttrNew->AeAttrType = pAttr->AttrType;
	   if (pAttr->AttrType == AtReferenceAttr)
	     {
		AssocCreated = NULL;
		/* attache un bloc reference a l'attribut */
		GetReference (&Ref);
		pAttrNew->AeAttrReference = Ref;
		pAttrNew->AeAttrReference->RdElement = NULL;
		pAttrNew->AeAttrReference->RdAttribute = pAttrNew;
		/* demande a l'utilisateur l'element reference' */
		if (LinkReference (firstSel, pAttrNew, SelDoc, &AssocCreated))
		  if (ThotLocalActions[T_checkextens]!=NULL)
		    (*ThotLocalActions[T_checkextens])
		      (pAttrNew, firstSel, lastSel, FALSE);
		if (AssocCreated != NULL)
		  {
		     CreeTousPaves (AssocCreated, SelDoc);
		     AbstractImageUpdated (SelDoc);
		  }
		/* applique l'attribut a la partie selectionnee */
		AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel, firstSel,
			       SelDoc);
		MajMenuAttr (SelDoc);
	     }
	   else
	     {
		/* cherche la valeur de cet attribut pour le premier element */
		/* selectionne' */
		CurrAttr = AttributeValue (firstSel, pAttrNew);
		if (pAttrNew->AeAttrNum == 1)
		  {
		     InitFormLanguage (doc, view, firstSel, CurrAttr);
		     /* memorise l'attribut concerne' par le formulaire */
		     SchCurrentAttr = pAttrNew->AeAttrSSchema;
		     NumCurrentAttr = 1;
		  }
		else if (pAttr->AttrType == AtEnumAttr && pAttr->AttrNEnumValues == 1)
		   /* attribut enumere' a une seule valeur(attribut booleen) */
		  {
		     if (CurrAttr == NULL)
			/* le premier element selectionne' n'a pas cet */
			/* attribut. On le lui met */
			pAttrNew->AeAttrValue = 1;
		     else
			/* suppression de l'attribut */
			pAttrNew->AeAttrValue = 0;
		     /* applique l'operation a la partie selectionnee */
		     AttachAttrToRange (pAttrNew, lastChar, firstChar, lastSel, firstSel,
				    SelDoc);
		  }
		else
		  {
		     /* construit le formulaire de saisie de la valeur de */
		     /* l'attribut */
		     MenuValues (pAttr, FALSE, CurrAttr, SelDoc, view);
		     /* memorise l'attribut concerne' par le formulaire */
		     SchCurrentAttr = AttrStruct[att];
		     NumCurrentAttr = AttrNumber[att];
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
		DeleteAttribute (NULL, pAttrNew);
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
		  TextAttrValue[0] = '\0';
	       else
		  strncpy (TextAttrValue, txt, LgMaxAttrText);
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




