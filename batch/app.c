#include "thot_sys.h"
#include "compilmsg.h"
#include "appmsg.h"
#include "message.h"
#include "APP.h"
#include "constgrm.h"
#include "constmedia.h"
#include "typemedia.h"
#include "typegrm.h"
#include "storage.h"
#include "app.h"
#include "modif.h"
#include "menuaction.h"
#include "registry.h"

#include "analsynt.f"
#include "compilmsg.f"
#include "filesystem.f"
#include "memory.f"
#include "message.f"
#include "rdschstr.f"
#include "storage.f"
#include "wrschApp.f"

#define EXPORT
#include "environ.var"
#include "analsynt.var"
#include "compil.var"
#undef EXPORT
#define EXPORT extern

EXPORT int         linenb;
EXPORT PtrEventsSet pAppli;
EXPORT PtrSSchema pSchStr;
extern int          IncNbIdent;

int                 linenb;	/* compteur de lignes dans le fichier source */
static Name          filename;
PtrSSchema        pSchStr;
PtrEventsSet        pAppli;

/* Pointeur sur la liste des schemas d'interfaces utilise's par EDITOR.A */
PtrAppName          SchemasUsed = NULL;
/* Pointeur sur la liste des noms de menus effectivement utilises */
PtrAppName          MenusUsed = NULL;
/* pointeur sur la liste des noms d'items effectivement utilises */
PtrAppName          ItemsUsed = NULL;
/* pointeur sur la liste des noms d'actions effectivement utilisees */
PtrAppName          ActionsUsed = NULL;
/* pointeur sur la chaine des menus de la fenetre principale de l'application */
PtrAppMenu          MainWindowMenus = NULL;
/* pointeur sur la chaine des menus des frames document de l'application */
PtrAppMenu          DocWindowMenus = NULL;
/* pointeur sur la chaine des descripteurs des menus de chaque type de document */
PtrAppDocType       DocTypeMenus = NULL;
static boolean      PairePremier = False; /* on a rencontre' le mot cle "First"  */
static boolean      PaireSecond = False;  /* on a rencontre' le mot cle "Second" */
static int          ruleNb;
static int          attribNb;
static int          curEvent;	/* l'evenement courant               */
static char        *eventAction;	/* l'action associee a cet evenement */
static boolean      PreEvent;
static boolean      DefaultSection;	/* on est dans la section DEFAULT    */
static boolean      ElementsSection;	/* on est dans la section ELEMENTS   */
static boolean      AttributesSection;	/* on est dans la section ATTRIBUTES */
static boolean      MenusSection;	/* on est dans la section MENUS      */
static PtrAppMenu  *MenuList;
static int          ViewNumber;
static char         MenuName[100];
static char         SubmenuName[100];
static char         ItemName[100];
static char         ItemType;	/* 'B' = Button,    'T' = Toggle,   */

				     /* 'S' = Separator, 'D' = Dynamic.  */
static char         ActionName[100];

/* le tableau RegisteredAppEvents doit etre coherent avec le type enum APPevent
   defini dans appaction.h */
char               *RegisteredAppEvents[] =
{
   "AttrMenu",
   "AttrCreate",
   "AttrModify",
   "AttrRead",
   "AttrSave",
   "AttrExport",
   "AttrDelete",
   "ElemMenu",
   "ElemNew",
   "ElemRead",
   "ElemSave",
   "ElemExport",
   "ElemDelete",
   "ElemSelect",
   "ElemExtendSelect",
   "ElemActivate",
   "ElemSetReference",
   "ElemInclude",
   "ElemCopy",
   "ElemPaste",
   "ElemChange",
   "ElemMove",
   "ElemTextModify",
   "ElemGraphModify",
   "PRuleCreate",
   "PRuleModify",
   "PRuleDelete",
   "DocOpen",
   "DocCreate",
   "DocClose",
   "DocSave",
   "DocExport",
   "ViewOpen",
   "ViewClose",
   "ViewResize",
   "ViewScroll",
   "Init",
   "Exit"
};

/* ---------------------------------------------------------------------- */
/* |    MenuActionList met dans la liste ActionsUsed les actions        | */
/* |    utilisees par les menus de la liste commencant par firstMenu,   | */
/* |    si elles ne sont pas deja dans la liste ActionsUsed.            | */
/* |    Fait de meme pour les noms de menus dans la liste MenusUsed     | */
/* |    et pour les noms d'items dans la liste ItemsUsed.               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         MenuActionList (PtrAppMenu firstMenu)

#else  /* __STDC__ */
static void         MenuActionList (firstMenu)
PtrAppMenu          firstMenu;

#endif /* __STDC__ */

{
   PtrAppName          curAction, prevAction;
   PtrAppName          curMenu, prevMenu;
   PtrAppName          curItem, prevItem;
   PtrAppMenu          menu;
   PtrAppMenuItem      item, menuitem;
   boolean             trouve;

   menu = firstMenu;
   /* parcourt la liste de menus a traiter */
   while (menu != NULL)
     {
	/* cherche si le nom de ce menu est deja dans la liste MenusUsed */
	curMenu = MenusUsed;
	trouve = False;
	prevMenu = NULL;
	while (!trouve && curMenu != NULL)
	  {
	     if (curMenu->AppNameValue != NULL &&
		 strcmp (curMenu->AppNameValue, menu->AppMenuName) == 0)
		/* le nom du menu est dans la liste */
		trouve = True;
	     else
	       {
		  prevMenu = curMenu;
		  /* passe au nom de menu suivant de la liste */
		  curMenu = curMenu->AppNextName;
	       }
	  }
	if (!trouve)
	   /* le nom du menu n'est pas dans la liste, on l'y met */
	  {
	     curMenu = (PtrAppName) TtaGetMemory (sizeof (AppName));
	     if (menu->AppMenuName == NULL)
		curMenu->AppNameValue = NULL;
	     else
		curMenu->AppNameValue = strdup (menu->AppMenuName);
	     curMenu->AppNextName = NULL;
	     if (prevMenu == NULL)
		MenusUsed = curMenu;
	     else
		prevMenu->AppNextName = curMenu;
	  }

	/* parcourt la liste des items du menu et des sous-menus */
	item = menu->AppMenuItems;
	menuitem = NULL;
	while (item != NULL)
	  {
	     /* skip menu separators */
	     if (item->AppItemType != 'S')
	       {
		  /* cherche si le nom l'item est dans la liste des noms d'items */
		  curItem = ItemsUsed;
		  trouve = False;
		  prevItem = NULL;
		  while (!trouve && curItem != NULL)
		    {
		       if (curItem->AppNameValue != NULL && item->AppItemName != NULL &&
			   strcmp (curItem->AppNameValue, item->AppItemName) == 0)
			  /* le nom de l'item est dans la liste */
			  trouve = True;
		       else
			 {
			    prevItem = curItem;
			    /* passe au nom suivant de la liste */
			    curItem = curItem->AppNextName;
			 }
		    }
		  if (!trouve)
		     /* le nom de l'item n'est pas dans la liste, on l'y met */
		    {
		       curItem = (PtrAppName) TtaGetMemory (sizeof (AppName));
		       curItem->AppNameValue = strdup (item->AppItemName);
		       curItem->AppNextName = NULL;
		       if (prevItem == NULL)
			  ItemsUsed = curItem;
		       else
			  prevItem->AppNextName = curItem;
		    }
	       }
	     if (item->AppItemActionName != NULL)
		/* cet item a une action definie */
	       {
		  /* cherche si l'action de l'item est dans la liste des actions */
		  curAction = ActionsUsed;
		  trouve = False;
		  prevAction = NULL;
		  while (!trouve && curAction != NULL)
		    {
		       if (curAction->AppNameValue != NULL &&
			   strcmp (curAction->AppNameValue, item->AppItemActionName) == 0)
			  /* l'action de l'item est dans la liste */
			  trouve = True;
		       else
			 {
			    prevAction = curAction;
			    /* passe a l'action suivante de la liste */
			    curAction = curAction->AppNextName;
			 }
		    }
		  if (!trouve)
		     /* l'action de l'item n'est pas dans la liste, on l'y met */
		    {
		       curAction = (PtrAppName) TtaGetMemory (sizeof (AppName));
		       curAction->AppNameValue = strdup (item->AppItemActionName);
		       curAction->AppStandardName = item->AppStandardAction;
		       curAction->AppNextName = NULL;
		       if (prevAction == NULL)
			  ActionsUsed = curAction;
		       else
			  prevAction->AppNextName = curAction;
		    }
	       }

	     if (item->AppSubMenu != NULL)
	       {
		  /* initialise le traitement les items du sous-menu */
		  menuitem = item;
		  item = item->AppSubMenu;
	       }
	     else
	       {
		  /* passe a l'item suivant */
		  item = item->AppNextItem;
		  if (menuitem != NULL && item == NULL)
		    {
		       /* reprend le traitement les items du menu */
		       item = menuitem->AppNextItem;
		       menuitem = NULL;
		    }
	       }
	  }
	/* passe au menu suivant */
	menu = menu->AppNextMenu;
     }
}

/* ---------------------------------------------------------------------- */
/* |    MakeMenusAndActionList construit la liste des noms de toutes    | */
/* |    les actions effectivement utiles, la liste de tous les noms de  | */
/* |    menus effectivement utiles et de tous les noms d'item utiles.   | */
/* ---------------------------------------------------------------------- */
static void         MakeMenusAndActionList ()
{
   PtrAppDocType       menusDoc;

   /* cherche les actions, les menus et les items de la fenetre principale */
   MenuActionList (MainWindowMenus);
   /* cherche les actions, les menus et les items des frames document
      normales */
   MenuActionList (DocWindowMenus);
   /* cherche les actions les menus et les items des frames document de type
      particulier */
   menusDoc = DocTypeMenus;
   while (menusDoc != NULL)
     {
	MenuActionList (menusDoc->AppDocTypeMenus);
	menusDoc = menusDoc->AppNextDocType;
     }
}

/* ---------------------------------------------------------------------- */
/* |    RegisteredEvent checks whether eventName is in the list of      | */
/* |    registered events and returns True if yes, and the rank of the  | */
/* |    event in the list (rank).                                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      RegisteredEvent (char *eventName, int *rank)
#else  /* __STDC__ */
static boolean      RegisteredEvent (eventName, rank)
char               *eventName;
int                *rank;

#endif /* __STDC__ */
{
   int                 evtNb, evt;
   boolean             found;

   /* cherche le nom de l'evenement dans la table */
   evtNb = sizeof (RegisteredAppEvents) / sizeof (char *);

   found = False;
   for (evt = 0; evt < evtNb && !found; (evt)++)
      if (strcmp (eventName, RegisteredAppEvents[evt]) == 0)
	{
	   found = True;
	   *rank = evt;
	}
   return found;
}

/* ---------------------------------------------------------------------- */
/* |    ConstructAbstractSchStruct                                      | */
/* ---------------------------------------------------------------------- */
static PtrSSchema ConstructAbstractSchStruct ()
{
   PtrSSchema        pSchStr;

   pSchStr = (PtrSSchema) TtaGetMemory (sizeof (StructSchema));
   pSchStr->SsCode = 0;

   /* initialise les types de base */
   strcpy (pSchStr->SsRule[CharString].SrName, "TEXT_UNIT");
   strcpy (pSchStr->SsRule[GraphicElem].SrName, "GRAPHICS_UNIT");
   strcpy (pSchStr->SsRule[Symbol].SrName, "SYMBOL_UNIT");
   strcpy (pSchStr->SsRule[Picture].SrName, "PICTURE_UNIT");
   strcpy (pSchStr->SsRule[Refer].SrName, "REFERENCE_UNIT");
   strcpy (pSchStr->SsRule[PageBreak].SrName, "PAGE_BREAK");
   pSchStr->SsNRules = MAX_BASIC_TYPE - 1;
   pSchStr->SsNAttributes = 0;
   return pSchStr;
}

/* ---------------------------------------------------------------------- */
/* |    NewMenuComplete                                                 | */
/* ---------------------------------------------------------------------- */
static void         NewMenuComplete ()
{
   PtrAppMenu          Menu, NewMenu, CurMenu;
   PtrAppMenuItem      Item, NewItem, SubMenu;
   boolean             found;

   /* les variables MenuList, MenuName, SubmenuName, ItemName, ItemType */
   /* et ActionName sont a jour. */

   /* On cherche d'abord le menu */
   if (MenuList != NULL)
      CurMenu = *MenuList;
   else
      CurMenu = NULL;
   Menu = NULL;
   while (Menu == NULL && CurMenu != NULL)
      if (CurMenu->AppMenuName != NULL &&
	  strcmp (MenuName, CurMenu->AppMenuName) == 0)
	 Menu = CurMenu;
      else
	 CurMenu = CurMenu->AppNextMenu;

   if (Menu == NULL)
      /* creation d'un nouveau menu */
     {
	NewMenu = (PtrAppMenu) TtaGetMemory (sizeof (AppMenu));
	NewMenu->AppMenuName = strdup (MenuName);
	NewMenu->AppMenuView = ViewNumber;	/* la vue concenee */
	NewMenu->AppMenuItems = NULL;
	NewMenu->AppNextMenu = NULL;
	if (*MenuList == NULL)
	   /* il n'y a pas encore de menus dans la liste */
	   *MenuList = NewMenu;
	else
	  {
	     /* ajoute le nouveau menu en fin de la liste des menus */
	     Menu = *MenuList;
	     while (Menu->AppNextMenu != NULL)
		Menu = Menu->AppNextMenu;
	     Menu->AppNextMenu = NewMenu;
	  }
	Menu = NewMenu;
     }

   SubMenu = NULL;
   Item = NULL;
   if (SubmenuName[0] != '\0')
      /* il y a un sous-menu. On cherche son entree dans le menu */
     {
	found = False;
	Item = Menu->AppMenuItems;
	while (Item != NULL && !found)
	   if (Item->AppItemName != NULL &&
	       strcmp (Item->AppItemName, SubmenuName) == 0)
	      found = True;
	   else
	      Item = Item->AppNextItem;
	if (!found)
	   /* cet item n'existe pas, on le met */
	  {
	     /* cree un nouvel item  */
	     NewItem = (PtrAppMenuItem) TtaGetMemory (sizeof (AppMenuItem));
	     NewItem->AppItemName = strdup (SubmenuName);
	     NewItem->AppItemActionName = NULL;
	     NewItem->AppSubMenu = NULL;
	     NewItem->AppItemType = ' ';
	     NewItem->AppStandardAction = False;
	     NewItem->AppNextItem = NULL;
	     /* chaine le nouvel item en fin de liste d'items du menu */
	     if (Menu->AppMenuItems == NULL)
		Menu->AppMenuItems = NewItem;
	     else
	       {
		  /* cherche le dernier item du menu */
		  Item = Menu->AppMenuItems;
		  while (Item->AppNextItem != NULL)
		     Item = Item->AppNextItem;
		  Item->AppNextItem = NewItem;
	       }
	     Item = NewItem;
	  }
	SubMenu = Item;
     }

   if (ItemName[0] != '\0' || ItemType == 'S')
     {
	if (SubMenu != NULL)
	   Item = SubMenu->AppSubMenu;
	else
	   Item = Menu->AppMenuItems;
	/* Cherche si cet item est deja dans le menu, sauf si c'est un */
	/* separateur: on peut avoir plusieurs separateurs dans le meme menu */
	found = False;
	if (ItemType != 'S')
	   while (Item != NULL && !found)
	      if (Item->AppItemName != NULL &&
		  strcmp (Item->AppItemName, ItemName) == 0)
		 found = True;
	      else
		 Item = Item->AppNextItem;
	if (found)
	   CompilerError (1, APP, FATAL, APP_ITEM_ALREADY_EXISTS, inputLine,
			  linenb);
	else
	   /* cet item n'existe pas, on le met */
	  {
	     /* cree un nouvel item  */
	     NewItem = (PtrAppMenuItem) TtaGetMemory (sizeof (AppMenuItem));
	     if (ItemName[0] == '\0')
		NewItem->AppItemName = NULL;
	     else
		NewItem->AppItemName = strdup (ItemName);
	     NewItem->AppItemActionName = NULL;
	     NewItem->AppSubMenu = NULL;
	     NewItem->AppItemType = ItemType;
	     NewItem->AppStandardAction = False;
	     NewItem->AppNextItem = NULL;
	     if (SubMenu == NULL)
		/* chaine le nouvel item en fin de liste d'items du menu */
		if (Menu->AppMenuItems == NULL)
		   Menu->AppMenuItems = NewItem;
		else
		  {
		     /* cherche le dernier item du menu */
		     Item = Menu->AppMenuItems;
		     while (Item->AppNextItem != NULL)
			Item = Item->AppNextItem;
		     Item->AppNextItem = NewItem;
		  }
	     else
		/* chaine le nouvel item en fin de liste */
	     if (SubMenu->AppSubMenu == NULL)
		SubMenu->AppSubMenu = NewItem;
	     else
	       {
		  Item = SubMenu->AppSubMenu;
		  while (Item->AppNextItem != NULL)
		     Item = Item->AppNextItem;
		  Item->AppNextItem = NewItem;
	       }
	     /* on met l'action, sauf si c'est un separateur ou un */
	     /* sous-menu dynamique */
	     if (ActionName[0] != '\0')
	       {
		  NewItem->AppItemActionName = strdup (ActionName);
		  /* Il faut tester s'il s'agit d'une action standard */
		  NewItem->AppStandardAction = (strncmp (ActionName, "Ttc", 3) == 0);
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    initMenu                                                        | */
/* ---------------------------------------------------------------------- */
static void         initMenu ()
{
   ViewNumber = 0;
   MenuName[0] = '\0';
   SubmenuName[0] = '\0';
   ItemName[0] = '\0';
   ItemType = ' ';
   ActionName[0] = '\0';
}


/* ---------------------------------------------------------------------- */
/* |    CopieNom copie le mot traite dans le nom n.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         CopieNom (Name n, iline wi, iline wl)

#else  /* __STDC__ */
static void         CopieNom (n, wi, wl)
Name                 n;
iline               wi;
iline               wl;

#endif /* __STDC__ */

{

   if (wl > MAX_NAME_LENGTH - 1)
      CompilerError (wi, COMPIL, FATAL, COMPIL_WORD_TOO_LONG, inputLine, linenb);
   else
     {
	strncpy (n, &inputLine[wi - 1], wl);
	n[wl] = '\0';
     }
}


/* ---------------------------------------------------------------------- */
/* |    GenShortKW genere un mot-cle court.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         GenShortKW (int x, grmcode r, grmcode pr)

#else  /* __STDC__ */
static void         GenShortKW (x, r, pr)
int                 x;
grmcode             r;
grmcode             pr;

#endif /* __STDC__ */
{
   int                 typeId;

   switch (x)
     {
       /* traitement selon le code du mot-cle court */
     case CHR_59:
       /*  ;  */
       switch (r)
	 {
	 case RULE_EvtAction:
	   if (DefaultSection)
	     typeId = 0;
	   else if (ElementsSection)
	     typeId = ruleNb;
	   else if (AttributesSection)
	     typeId = attribNb;
	   else
	     typeId = 0;
	   TteAddActionEvent (pAppli, typeId, curEvent, PreEvent, eventAction);
	   curEvent = 0;
	   PreEvent = True;
	   eventAction = NULL;
	   break;
	 case RULE_NewMenu:
	   NewMenuComplete ();
	   initMenu ();
	   break;
	 default:
	   break;
	 }
       break;
     case CHR_44:
       /*  ,  */
       break;
       
     case CHR_46:
       /* .  */
       break;
       
     case CHR_58:
       /* :  */
       if (r == RULE_Menus)
	 initMenu ();
       break;
       
     default:
       break;
     }
}


/* ---------------------------------------------------------------------- */
/* |    GenLongKW genere un mot-cle long.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         GenLongKW (int x, grmcode r, iline wi)

#else  /* __STDC__ */
static void         GenLongKW (x, r, wi)
int                 x;
grmcode             r;
iline               wi;

#endif /* __STDC__ */

{
   switch (x)
	 {
	       /* traitement selon le code du mot-cle */
	    case KWD_APPLICATION:
	       /* initialisation */
	       DefaultSection = False;
	       ElementsSection = False;
	       AttributesSection = False;
	       MenusSection = False;
	       break;

	    case KWD_USES:
	       /* le mot-cle' USES */
	       if (strcmp (filename, "EDITOR") != 0)
		  /* ce n'est pas EDITOR.A qu'on compile, refus */
		  CompilerError (wi, APP, FATAL, APP_ONLY_IN_EDITOR_I,
				 inputLine, linenb);
	       else
		  SchemasUsed = NULL;
	       break;

	    case KWD_DEFAULT:
	       DefaultSection = True;
	       break;

	    case KWD_ELEMENTS:
	       ElementsSection = True;
	       DefaultSection = False;
	       break;

	    case KWD_ATTRIBUTES:
	       AttributesSection = True;
	       DefaultSection = False;
	       ElementsSection = False;
	       break;

	    case KWD_MENUS:
	       /* debut de la section MENUS */
	       DefaultSection = False;	/* la section DEFAULT est donc finie */
	       ElementsSection = False;
	       AttributesSection = False;
	       MenusSection = True;
	       if (strcmp (filename, "EDITOR") != 0)
		  /* ce n'est pas EDITOR.A qu'on compile, refus */
		  CompilerError (wi, APP, FATAL, APP_ONLY_IN_EDITOR_I,
				 inputLine, linenb);
	       break;

	    case KWD_BEGIN:
	       if (r == RULE_MenuList)
		  initMenu ();
	       break;

	    case KWD_END:
	       break;

	    case KWD_Post:
	       if (curEvent == TteAttrMenu)
		  CompilerError (wi, APP, FATAL, APP_ONLY_PRE_ALLOWED, inputLine,
				 linenb);
	       PreEvent = False;
	       break;

	    case KWD_Pre:
	       PreEvent = True;
	       break;

	    case KWD_First:
	       PairePremier = True;
	       break;

	    case KWD_Second:
	       PaireSecond = True;
	       break;

	    case KWD_Main:
	       MenuList = &MainWindowMenus;
	       break;

	    case KWD_Window:
	       break;

	    case KWD_Document:
	       MenuList = &DocWindowMenus;
	       break;

	    case KWD_Windows:
	       break;

	    case KWD_Separator:
	       ItemType = 'S';
	       ItemName[0] = '\0';
	       ActionName[0] = '\0';
	       break;

	    case KWD_Button:
	       ItemType = 'B';
	       break;

	    case KWD_Toggle:
	       ItemType = 'T';
	       break;

	    case KWD_Dynamic:
	       ItemType = 'D';
	       ActionName[0] = '\0';
	       break;

	    default:
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* |     GenName genere un nom.                                         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         GenName (grmcode r, grmcode pr, iline wl, iline wi)
#else  /* __STDC__ */
static void         GenName (r, pr, wl, wi)
grmcode             r;
grmcode             pr;
iline               wl;
iline               wi;

#endif /* __STDC__ */
{
   int                 i;
   Name                 name;
   PtrAppDocType       DocType, NewDocType;
   PtrAppName          SchUsed, NewSchUsed;

   CopieNom (name, wi, wl);

   switch (r)
	 {
	       /* r = numero de la regle ou apparait le nom */

	    case RULE_ElemIdent:
	       ruleNb = 0;
	       if (pr == RULE_AppliModel)
		 {
		    if (strcmp (filename, "EDITOR") == 0)
		      {
			 /* construct an abstract schemas structure */
			 pSchStr = ConstructAbstractSchStruct ();
			 /* acquiert un schema */
			 pAppli = TteNewEventsSet (pSchStr->SsCode, filename);
			 /* pointeur sur la chaine des descripteurs des menus des */
			 /* differents types de document */
			 DocTypeMenus = NULL;
		      }
		    else
		      {
			 /* TypeName est ici le nom de la structure generique */
			 /* => on lit le schema de structure compile' */
			 pSchStr = (PtrSSchema) TtaGetMemory (sizeof (StructSchema));
			 if (!RdSchStruct (name, pSchStr))
			    CompilerError (wi, APP, FATAL, APP_STRUCT_SCHEM_NOT_FOUND,
					   inputLine, linenb);
			 else if (strcmp (name, pSchStr->SsName) != 0)
			    CompilerError (wi, APP, FATAL,
					APP_THE_STRUCT_SCHEM_DOES_NOT_MATCH,
					   inputLine, linenb);
			 else
			    /* acquiert un schema */
			    pAppli = TteNewEventsSet (pSchStr->SsCode, filename);
		      }
		 }
	       else
		 {
		    /* is it an Element? */
		    if (pr == RULE_ElemActions)
		      {
			 i = 0;
			 while (strcmp (name, pSchStr->SsRule[i].SrName) != 0
				&& i < pSchStr->SsNRules)
			    i++;
			 if (i < pSchStr->SsNRules)
			   {
			      if (pSchStr->SsRule[i].SrConstruct == CsPairedElement)
				 /* c'est un element CsPairedElement */
				 if (!PaireSecond && !PairePremier)
				    /* le nom du type n'etait pas precede' de First ou Second */
				    CompilerError (wi, APP, FATAL, APP_MISSING_FIRST_SECOND,
						   inputLine, linenb);
				 else
				   {
				      if (PaireSecond)
					 /* il s'agit du type suivant */
					 i++;
				   }
			      else
				 /* ce n'est pas un element CsPairedElement */
			      if (PaireSecond || PairePremier)
				 /* le nom du type etait precede' de First ou Second */
				 CompilerError (wi, APP, FATAL, APP_NOT_IN_PAIR, inputLine,
						linenb);
			      ruleNb = i + 1;
			   }
			 else
			   {
			      if (strcmp (filename, "EDITOR") == 0)
				{
				   strcpy (pSchStr->SsRule[i].SrName, name);
				   pSchStr->SsNRules++;
				   ruleNb = i + 1;
				}
			      else
				 CompilerError (wi, APP, FATAL, APP_UNKNOWN_TYPE,
						inputLine, linenb);
			   }
			 PairePremier = False;
			 PaireSecond = False;
		      }
		 }
	       break;

	    case RULE_DocumentType:
	       if (pr == RULE_SchemaList)
		  /* un nom de type de document dans l'instruction USES */
		 {
		    /* acquiert un descripteur de schema A utilise' */
		    NewSchUsed = (PtrAppName) TtaGetMemory (sizeof (AppName));
		    /* met le nom du schema A utilise' dans le descripteur */
		    NewSchUsed->AppNameValue = strdup (name);
		    NewSchUsed->AppStandardName = False;
		    /* chaine ce nouveau descripteur en fin de liste */
		    NewSchUsed->AppNextName = NULL;
		    if (SchemasUsed == NULL)
		       /* la chaine etait vide */
		       SchemasUsed = NewSchUsed;
		    else
		      {
			 SchUsed = SchemasUsed;
			 while (SchUsed->AppNextName != NULL)
			    SchUsed = SchUsed->AppNextName;
			 SchUsed->AppNextName = NewSchUsed;
		      }
		 }
	       else if (pr == RULE_Menus)
		  /* un type de document pour lequel on veut definir les boutons */
		  /* et menus des frames */
		 {
		    /* ajoute un type de document */
		    /* alloue un descripteur de type de document */
		    NewDocType = (PtrAppDocType) TtaGetMemory (sizeof (AppDocType));
		    /* initialise ce descripteur */
		    NewDocType->AppDocTypeName = strdup (name);
		    NewDocType->AppDocTypeMenus = NULL;
		    NewDocType->AppNextDocType = NULL;
		    if (DocTypeMenus == NULL)
		       DocTypeMenus = NewDocType;
		    else
		      {
			 DocType = DocTypeMenus;
			 while (DocType->AppNextDocType != NULL)
			    DocType = DocType->AppNextDocType;
			 DocType->AppNextDocType = NewDocType;
		      }
		    NewDocType->AppDocTypeMenus = NULL;
		    MenuList = &(NewDocType->AppDocTypeMenus);
		 }
	       break;

	    case RULE_EvtIdent:
	       /* Le nom de l'evenement */
	       PreEvent = True;
	       /* cherche si l'evenement est dans la table des evenements definis */
	       if (!RegisteredEvent (name, &curEvent))
		  /* il n'y est pas, erreur */
		  CompilerError (wi, APP, FATAL, APP_UNKNOWN_MESSAGE, inputLine, linenb);
	       else if (!DefaultSection)
		 {
		    /* on n'est pas dans la section DEFAULT du schema A */
		    /* on n'accepte pas les evenements pour les documents, pour les */
		    /* vues, ni pour l'application */
		    if (curEvent >= TteDocOpen)
		       CompilerError (wi, APP, FATAL, APP_NOT_IN_DEFAULT, inputLine,
				      linenb);
		 }
	       else
		 {
		    if (curEvent >= TteInit)
		       /* c'est un evenement pour l'application */
		      {
			 if (strcmp (filename, "EDITOR") != 0)
			    /* ce n'est pas EDITOR.A qu'on compile, refus */
			    CompilerError (wi, APP, FATAL, APP_ONLY_IN_EDITOR_I, inputLine,
					   linenb);
		      }
		    else if (AttributesSection)
		      {
			 if (curEvent > TteAttrDelete)
			    CompilerError (wi, APP, FATAL, APP_INVALID_FOR_AN_ATTR,
					   inputLine, linenb);
		      }
		    else if (ElementsSection)
		      {
			 if (curEvent <= TteAttrDelete)
			    CompilerError (wi, APP, FATAL, APP_INVALID_FOR_AN_ELEM,
					   inputLine, linenb);
		      }
		 }
	       break;

	    case RULE_ActionIdent:
	       if (pr == RULE_ItemAction)
		  /* action associee a un item de menu */
		  strcpy (ActionName, name);
	       else if (pr == RULE_EvtAction)
		 {
		    /* action associee a un evenement */
		    eventAction = strdup (name);
		    InsertAction (eventAction, 0);
		 }
	       break;

	    case RULE_AttrIdent:
	       attribNb = 0;
	       if (strcmp (filename, "EDITOR") == 0 && pSchStr == NULL)
		 {
		    pSchStr = ConstructAbstractSchStruct ();
		    pAppli = TteNewEventsSet (pSchStr->SsCode, filename);
		 }
	       if (pr == RULE_AttrActions)
		 {
		    i = 1;
		    while (strcmp (name, pSchStr->SsAttribute[i - 1].AttrOrigName) != 0
			   && i <= pSchStr->SsNAttributes)
		       i++;
		    if (i <= pSchStr->SsNAttributes)
		       attribNb = i;
		    else
		      {
			 if (strcmp (filename, "EDITOR") == 0)
			   {
			      /* the file .A is a EDITOR.A */
			      strcpy (pSchStr->SsAttribute[i - 1].AttrOrigName, name);
			      pSchStr->SsNAttributes = pSchStr->SsNAttributes + 1;
			      attribNb = i;
			   }
			 else
			    CompilerError (wi, APP, FATAL, APP_UNKNOWN_ATTRIBUTE, inputLine, linenb);
		      }
		 }
	       break;

	    case RULE_MenuIdent:
	       /* un nom de menu */
	       strcpy (MenuName, name);
	       SubmenuName[0] = '\0';
	       ItemName[0] = '\0';
	       ItemType = ' ';
	       ActionName[0] = '\0';
	       break;

	    case RULE_SubmenuIdent:
	       /* un nom de sous-menu dans une definition de menu */
	       strcpy (SubmenuName, name);
	       break;

	    case RULE_ItemIdent:
	       /* un nom d'item de menu dans une definition de menu */
	       strcpy (ItemName, name);
	       break;

	    default:
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* |     GenNumber genere un nombre.                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         GenNumber (grmcode r, iline wl, iline wi)
#else  /* __STDC__ */
static void         GenNumber (r, wl, wi)
grmcode             r;
iline               wl;
iline               wi;

#endif /* __STDC__ */
{
   int                 n;

   n = AsciiToInt (wi, wl);
   switch (r)
	 {
	       /* r = numero de la regle ou apparait le nombre */

	    case RULE_ViewNum:
	       ViewNumber = n;
	       break;

	    default:
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |    ProcessToken traite le mot commencant a` la position wi dans la | */
/* |            ligne courante, de longueur wl et de code grammatical c.| */
/* |            Si c'est un identif, nb contient son rang dans la table | */
/* |            des identificateurs. r est le numero de la regle dans   | */
/* |            laquelle apparait ce mot, pr est le numero de la regle  | */
/* |            precedente, celle qui a appele la regle r.              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ProcessToken (iline wi, iline wl, grmcode c, grmcode r, int nb, grmcode pr)
#else  /* __STDC__ */
static void         ProcessToken (wi, wl, c, r, nb, pr)
iline               wi;
iline               wl;
grmcode             c;
grmcode             r;
int                 nb;
grmcode             pr;

#endif /* __STDC__ */
{
   if (c < 1000)
     {
	/* symbole intermediaire de la grammaire, erreur */
	CompilerError (wi, APP, FATAL, APP_INTERMEDIATE_SYMBOL, inputLine,
		       linenb);
     }
   else if (c < 1100)
      /* mot-cle court */
      GenShortKW (c, r, pr);
   else if (c < 2000)
      /* mot-cle long */
      GenLongKW (c, r, wi);
   else
      /* type de base */
      switch (c)
	    {
	       case 3001:
		  /* un nom */
		  GenName (r, pr, wl, wi);
		  break;
	       case 3002:
		  /* un nombre */
		  GenNumber (r, wl, wi);
		  break;
	    }
}


/* ---------------------------------------------------------------------- */
/* |    write1char write a single character.                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         write1char (FILE * Hfile, unsigned char ch)

#else  /* __STDC__ */
static void         write1char (Hfile, ch)
FILE               *Hfile;
unsigned char       ch;

#endif /* __STDC__ */

{
   int                 code;

   if (ch < ' ' || ch > '~')
      /* non ASCII character. Replace it by an ASCII character or its octal code */
     {
	code = (int) ch;
	switch (code)
	      {
		 case 224:
		 case 225:
		 case 226:
		 case 227:
		 case 228:
		 case 229:
		    fprintf (Hfile, "a");
		    break;
		 case 230:
		    fprintf (Hfile, "ae");
		    break;
		 case 231:
		    fprintf (Hfile, "c");
		    break;
		 case 232:
		 case 233:
		 case 234:
		 case 235:
		    fprintf (Hfile, "e");
		    break;
		 case 236:
		 case 237:
		 case 238:
		 case 239:
		    fprintf (Hfile, "i");
		    break;
		 case 160:
		    fprintf (Hfile, "_");
		    break;
		 case 241:
		    fprintf (Hfile, "n");
		    break;
		 case 242:
		 case 243:
		 case 244:
		 case 245:
		 case 246:
		 case 248:
		    fprintf (Hfile, "o");
		    break;
		 case 247:
		    fprintf (Hfile, "oe");
		    break;
		 case 249:
		 case 250:
		 case 251:
		 case 252:
		    fprintf (Hfile, "u");
		    break;
		 case 253:
		    fprintf (Hfile, "y");
		    break;
		 default:
		    fprintf (Hfile, "_%o", code);
		    break;
	      }
     }
   else
      /* printable character. Write it as is */
      fprintf (Hfile, "%c", ch);
}

/* ---------------------------------------------------------------------- */
/* |    WriteName                                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         WriteName (FILE * Hfile, Name n)

#else  /* __STDC__ */
static void         WriteName (Hfile, n)
FILE               *Hfile;
Name                 n;

#endif /* __STDC__ */

{
   int                 i;

   i = 0;
   while (n[i] != '\0')
     {
	write1char (Hfile, n[i]);
	i++;
     }
}

/* ---------------------------------------------------------------------- */
/* |    WriteRuleName                                                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         WriteRuleName (FILE * Hfile, int r)

#else  /* __STDC__ */
static void         WriteRuleName (Hfile, r)
FILE               *Hfile;
int         r;

#endif /* __STDC__ */

{
   if (pSchStr->SsRule[r - 1].SrName[0] == '\0')
      fprintf (Hfile, "ID%d", r);
   else
      WriteName (Hfile, pSchStr->SsRule[r - 1].SrName);
}

/* ---------------------------------------------------------------------- */
/* |    WriteAttribute                                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         WriteAttribute (FILE * Hfile, int a)

#else  /* __STDC__ */
static void         WriteAttribute (Hfile, a)
FILE               *Hfile;
int      a;

#endif /* __STDC__ */

{
   int                 j;
   TtAttribute           *pAttr;

   pAttr = &pSchStr->SsAttribute[a];
   if (pAttr->AttrGlobal)
      return;			/* AttrGlobal means "attribute written" */
   fprintf (Hfile, "#define ");
   WriteName (Hfile, pSchStr->SsName);
   fprintf (Hfile, "_ATTR_");
   WriteName (Hfile, pAttr->AttrName);
   fprintf (Hfile, " %d\n", a + 1);
   if (pAttr->AttrType == AtEnumAttr)
      for (j = 0; j < pAttr->AttrNEnumValues; j++)
	{
	   fprintf (Hfile, "#define ");
	   WriteName (Hfile, pSchStr->SsName);
	   fprintf (Hfile, "_ATTR_");
	   WriteName (Hfile, pAttr->AttrName);
	   fprintf (Hfile, "_VAL_");
	   WriteName (Hfile, pAttr->AttrEnumValue[j]);
	   fprintf (Hfile, " %d\n", j + 1);
	}
   pAttr->AttrGlobal = True;
}

/* ---------------------------------------------------------------------- */
/* |    WriteBasicElements                                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         WriteBasicElements (FILE * Hfile)

#else  /* __STDC__ */
static void         WriteBasicElements (Hfile)
FILE               *Hfile;

#endif /* __STDC__ */

{
   int                 r;

   fprintf (Hfile, "#define ");
   WriteName (Hfile, pSchStr->SsName);
   r = CharString + 1;
   fprintf (Hfile, "_EL_");
   WriteRuleName (Hfile, r);
   fprintf (Hfile, " %d\n", r);

   fprintf (Hfile, "#define ");
   WriteName (Hfile, pSchStr->SsName);
   r = GraphicElem + 1;
   fprintf (Hfile, "_EL_");
   WriteRuleName (Hfile, r);
   fprintf (Hfile, " %d\n", r);

   fprintf (Hfile, "#define ");
   WriteName (Hfile, pSchStr->SsName);
   r = Symbol + 1;
   fprintf (Hfile, "_EL_");
   WriteRuleName (Hfile, r);
   fprintf (Hfile, " %d\n", r);

   fprintf (Hfile, "#define ");
   WriteName (Hfile, pSchStr->SsName);
   r = Picture + 1;
   fprintf (Hfile, "_EL_");
   WriteRuleName (Hfile, r);
   fprintf (Hfile, " %d\n", r);

   fprintf (Hfile, "#define ");
   WriteName (Hfile, pSchStr->SsName);
   r = PageBreak + 1;
   fprintf (Hfile, "_EL_");
   WriteRuleName (Hfile, r);
   fprintf (Hfile, " %d\n", r);
}

/* ---------------------------------------------------------------------- */
/* |    WriteRule                                                       | */
/* |    si pRegleExtens est non nul, il s'agit d'une regle d'extension  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         WriteRule (FILE * Hfile, int r, SRule * pRegleExtens)

#else  /* __STDC__ */
static void         WriteRule (Hfile, r, pRegleExtens)
FILE               *Hfile;
int         r;
SRule              *pRegleExtens;

#endif /* __STDC__ */

{
   int                 i;
   SRule              *pRule;

   if (pRegleExtens != NULL)
      pRule = pRegleExtens;
   else
      pRule = &pSchStr->SsRule[r];
   if (pRule->SrConstruct != CsNatureSchema &&
       !(pRule->SrConstruct == CsPairedElement && !pRule->SrFirstOfPair))
     {
	fprintf (Hfile, "#define ");
	WriteName (Hfile, pSchStr->SsName);
	if (pRegleExtens == NULL)
	  {
	     fprintf (Hfile, "_EL_");
	     WriteRuleName (Hfile, r);
	  }
	else
	  {
	     fprintf (Hfile, "_EXT_");
	     if (pRule->SrName[0] == '\0')
		fprintf (Hfile, "Root");
	     else
		WriteName (Hfile, pRule->SrName);
	  }
	fprintf (Hfile, " %d\n", r);

	if (pRule->SrNLocalAttrs > 0)
	   for (i = 0; i < pRule->SrNLocalAttrs; i++)
	      WriteAttribute (Hfile, pRule->SrLocalAttr[i] - 1);
     }
}

/* ---------------------------------------------------------------------- */
/* |    WriteDefineFile                                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         WriteDefineFile (char *fname)

#else  /* __STDC__ */
static void         WriteDefineFile (fname)
char               *fname;

#endif /* __STDC__ */

{
   boolean             FirstLocalAttribute;
   boolean             First;
   int         FirstRule;
   int                 i;
   int                 rule;
   SRule              *pRule;
   Name                 Hfilename;
   FILE               *Hfile;

   sprintf (Hfilename, "%s.h", fname);
   Hfile = fopen (Hfilename, "w");
   if (Hfile != NULL)
     {
	fprintf (Hfile, "/* Types and attributes for the document type %s */\n", pSchStr->SsName);
	/* write global attributes */
	if (pSchStr->SsNAttributes > 0 && pSchStr->SsAttribute[0].AttrGlobal)
	   fprintf (Hfile, "\n/* Global attributes */\n");

	for (i = 0; i < pSchStr->SsNAttributes; i++)
	   if (pSchStr->SsAttribute[i].AttrGlobal)
	     {
		pSchStr->SsAttribute[i].AttrGlobal = False;
		/* tell WriteAttribute that it should write attribute values */
		WriteAttribute (Hfile, i);
		pSchStr->SsAttribute[i].AttrGlobal = True;
	     }
	/* write local attributes */
	FirstLocalAttribute = True;
	for (i = 0; i < pSchStr->SsNAttributes; i++)
	   if (!pSchStr->SsAttribute[i].AttrGlobal)
	     {
		if (FirstLocalAttribute)
		  {
		     fprintf (Hfile, "\n/* Local attributes */\n");
		     FirstLocalAttribute = False;
		  }
		WriteAttribute (Hfile, i);
	     }
	/* write Thot basic types */
	fprintf (Hfile, "\n/* Basic elements */\n");
	WriteBasicElements (Hfile);

	/* write constants */
	rule = MAX_BASIC_TYPE;
	if (pSchStr->SsRule[rule].SrConstruct == CsConstant)
	  {
	     fprintf (Hfile, "\n/* Constants */\n");
	     while (pSchStr->SsRule[rule].SrConstruct == CsConstant)
		WriteRule (Hfile, ++rule, NULL);
	  }
	FirstRule = rule + 1;
	/* write parameters */
	First = True;
	for (rule = FirstRule; rule <= pSchStr->SsNRules; rule++)
	   if (pSchStr->SsRule[rule].SrParamElem)
	     {
		if (First)
		  {
		     fprintf (Hfile, "\n/* Parameters */\n");
		     First = False;
		  }
		WriteRule (Hfile, rule, NULL);
	     }
	/* write rules */
	if (pSchStr->SsNRules >= FirstRule)
	   fprintf (Hfile, "\n/* Elements */\n");
	for (rule = FirstRule; rule <= pSchStr->SsNRules; rule++)
	  {
	     pRule = &pSchStr->SsRule[rule];
	     /* skip parameters, associated elements, */
	     /* Extern and Included elements and units */
	     if (!pRule->SrParamElem &&
		 !pRule->SrAssocElem &&
		 !pRule->SrRefImportedDoc &&
		 !pRule->SrUnitElem)
		/* ignore lists added for associated elements */
		if (pRule->SrConstruct != CsList)
		   WriteRule (Hfile, rule, NULL);
		else if (!pSchStr->SsRule[pRule->SrListItem - 1].SrAssocElem)
		   WriteRule (Hfile, rule, NULL);
	  }
	/* write extension rules */
	if (pSchStr->SsExtension && pSchStr->SsNExtensRules > 0)
	  {
	     fprintf (Hfile, "\n/* Extension rules */\n");
	     for (rule = 0; rule < pSchStr->SsNExtensRules; rule++)
	       {
		  pRule = &pSchStr->SsExtensBlock->EbExtensRule[rule];
		  WriteRule (Hfile, rule, pRule);
	       }
	  }
	/* write associated elements */
	First = True;
	for (rule = FirstRule; rule <= pSchStr->SsNRules; rule++)
	   if (pSchStr->SsRule[rule].SrAssocElem)
	     {
		if (First)
		  {
		     fprintf (Hfile, "\n/* Associated elements */\n");
		     First = False;
		  }
		WriteRule (Hfile, rule, NULL);
	     }
	if (!First)
	   /* there is at least one associated element. Write LIST rules added */
	   /* for associated elements */
	  {
	     for (rule = FirstRule; rule <= pSchStr->SsNRules; rule++)
	       {
		  pRule = &pSchStr->SsRule[rule];
		  if (pRule->SrConstruct == CsList)
		     if (pSchStr->SsRule[pRule->SrListItem - 1].SrAssocElem)
			WriteRule (Hfile, rule, NULL);
	       }
	  }
	/* write exported elements */
	First = True;
	for (rule = FirstRule; rule <= pSchStr->SsNRules; rule++)
	   if (pSchStr->SsRule[rule].SrUnitElem)
	     {
		if (First)
		  {
		     fprintf (Hfile, "\n/* Units */\n");
		     First = False;
		  }
		WriteRule (Hfile, rule, NULL);
	     }

	First = True;
	for (rule = FirstRule; rule <= pSchStr->SsNRules; rule++)
	   if (pSchStr->SsRule[rule].SrConstruct == CsNatureSchema)
	     {
		if (First)
		  {
		     fprintf (Hfile, "\n/* Imported natures */\n");
		     First = False;
		  }
		fprintf (Hfile, "#define ");
		WriteName (Hfile, pSchStr->SsName);
		fprintf (Hfile, "_EL_");
		WriteRuleName (Hfile, rule + 1);
		fprintf (Hfile, " %d\n", rule + 1);
	     }
     }
}

/* ---------------------------------------------------------------------- */
/* |    Main pour le compilateur A.                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                main (int argc, char **argv)

#else  /* __STDC__ */
void                main (argc, argv)
int                 argc;
char              **argv;

#endif /* __STDC__ */
{
   FILE               *filedesc;
   boolean             fileOK;
   char                cppFileName[200];
   Name                 pfilename;
   int                 i;
   int                 wi;	/* position du debut du mot courant dans la ligne */
   int                 wl;	/* longueur du mot courant */
   nature              wn;	/* nature du mot courant */
   rnb                 r;	/* numero de regle */
   rnb                 pr;	/* numero de la regle precedente */
   grmcode             c;	/* code grammatical du mot trouve */
   int                 nb;	/* indice dans identtable du mot trouve, si */

   /* identificateur */

   TtaInitializeAppRegistry (argv[0]);
   APP = TtaGetMessageTable ("appdialogue", APP_MSG_MAX);
   COMPIL = TtaGetMessageTable ("compildialogue", COMPIL_MSG_MAX);
   error = False;
   /* initialise l'analyseur syntaxique */
   InitParser ();
   /* charge la grammaire du langage a compiler */
   InitSyntax ("APP.GRM");
   if (!error)
     {
	if (argc != 2)
	  {
	     fprintf (stderr, "usage: %s <input-file>\n", argv[0]);
	     exit (1);
	  }
	else
	  {
	     /* recupere le nom du schema a compiler */
	     strncpy (filename, argv[1], MAX_NAME_LENGTH - 1);
	     strncpy (pfilename, filename, MAX_NAME_LENGTH - 3);
	     /* ajoute le suffixe .A */
	     strcat (pfilename, ".A");
	     /* ouvre le fichier a compiler */
	     filedesc = fopen (pfilename, "r");
	     if (filedesc == 0)
		TtaDisplayMessage (FATAL, TtaGetMessage(APP, APP_NO_SUCH_FILE), pfilename);
	     else
	       {
		  /* le fichier a compiler est ouvert */
		  strcpy (pfilename, filename);
		  lgidenttable = 0;
		  /* table des identificateurs vide */
		  linenb = 0;
		  /* encore aucune ligne lue */
		  pSchStr = NULL;
		  /* pas (encore) de schema de structure */
		  fileOK = True;
		  /* lit tout le fichier et fait l'analyse */
		  while (fileOK && !error)
		     /* lit une ligne */
		    {
		       i = 0;
		       do
			  fileOK = BIOreadByte (filedesc, &inputLine[i++]);
		       while (i < linelen && inputLine[i - 1] != '\n' && fileOK);
		       /* marque la fin reelle de la ligne */
		       inputLine[i - 1] = '\0';
		       /* incremente le compteur de lignes lues */
		       linenb++;
		       if (i >= linelen)
			  /* ligne trop longue */
			  CompilerError (1, APP, FATAL, APP_LINE_TOO_LONG, inputLine, linenb);
		       else if (inputLine[0] == '#')
			  /* cette ligne contient une directive du preprocesseur cpp */
			 {
			    sscanf (inputLine, "# %d %s", &linenb, cppFileName);
			    linenb--;
			 }
		       else
			  /* traduit tous les caracteres de la ligne */
			 {
			    OctalToChar ();
			    /* analyse la ligne */
			    wi = 1;
			    wl = 0;
			    /* analyse tous les mots de la ligne courante */
			    do
			      {
				 i = wi + wl;
				 GetNextToken (i, &wi, &wl, &wn);
				 /* mot suivant */
				 if (wi > 0)
				    /* on a trouve un mot */
				   {
				      AnalyzeToken (wi, wl, wn, &c, &r, &nb, &pr);
				      /* on analyse le mot */
				      if (!error)
					 ProcessToken (wi, wl, c, r, nb, pr);	/* on le traite */
				   }
			      }
			    while (!(wi == 0 || error));
			    /* il n'y a plus de mots a analyser dans la ligne */
			 }
		    }
		  /* fin du fichier */
		  if (!error)
		     ParserEnd ();
		  /* fin d'analyse */
		  if (!error)
		    {
		       MakeMenusAndActionList ();
		       /* ecrit le schema compile' dans le fichier de sortie     */
		       /* le directory des schemas est le directory courant      */
		       DirectorySchemas[0] = '\0';
		       GenerateApplication (pfilename, pAppli, pSchStr);
		       strcpy (pfilename, filename);
		       if (strcmp (pfilename, "EDITOR") != 0)
			  WriteDefineFile (pfilename);
		    }
	       }
	  }
     }
   TtaSaveAppRegistry ();
   exit (0);
}
