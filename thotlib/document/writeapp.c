
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   wrschApp.c : Ce module sauve un schema APP compile'.
 */
#include "stdio.h"
#include "thot_sys.h"
#include "conststr.h"
#include "constpiv.h"
#include "constgrm.h"
#include "typegrm.h"
#include "typestr.h"
#include "app.h"
#include "menuaction.h"
#include "logo.xpm"
#include "logo.xbm"

extern PtrAction    externalActions;
extern PtrExternAppliList pAppli;
extern PtrSSchema pSchStr;
extern char        *RegisteredAppEvents[];
extern PtrAppName   SchemasUsed;
extern PtrAppName   MenusUsed;
extern PtrAppName   ItemsUsed;
extern PtrAppName   ActionsUsed;
extern PtrAppMenu   MainWindowMenus;
extern PtrAppMenu   DocWindowMenus;
extern PtrAppDocType DocTypeMenus;

static FILE        *outputFile;
static FILE        *rcFile;
static char         local[50];

#define APP_NOOPCHAR "BIDON"

#include "filesystem.f"

#ifdef __STDC__
extern char        *TtaGetEnvString (char *);
extern void         TtaFreeMemory (void *);

#else
extern char        *TtaGetEnvString ();
extern void         TtaFreeMemory ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |                                                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static char        *AppwrEvent (APPevent event)
#else  /* __STDC__ */
static char        *AppwrEvent (event)
APPevent            event;

#endif /* __STDC__ */
{
   if (event > TteExit)
      return NULL;

   strcpy (&local[3], RegisteredAppEvents[event]);
   /* add prefix Tte to the event name */
   local[0] = 'T';
   local[1] = 't';
   local[2] = 'e';
   return local;
}

/* ---------------------------------------------------------------------- */
/* |                                                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         WriteRcHeaders (char *fname, FILE * file)

#else  /* __STDC__ */
static void         WriteRcHeaders (fname, file)
char               *fname;
FILE               *file;

#endif /* __STDC__ */

{
   char                buf[500], *cour = &buf[0];
   char               *user;
   struct tm          *tm;
   time_t              current_date;

   (void) time (&current_date);
   tm = localtime (&current_date);
   (void) strftime (cour, 100, "%x %X", tm);
   while (*cour != 0)
      cour++;
   user = TtaGetEnvString ("USER");
   if (user == NULL)
      user = "unknown user";
   sprintf (cour, " by %s on ", user);
   while (*cour != 0)
      cour++;
   (void) gethostname (cour, 100);
   while (*cour != 0)
      cour++;
   sprintf (cour, "\n */\n\n");

   fprintf (file, "/*\n * %sAPP.rc : %s Windows Resource Script\n",
	    fname, fname);
   fprintf (file, " * Last generated : %si\n */", cour);
   fprintf (file, "#include \"%s.h\"\n", fname);
   fprintf (file, "#include \"ICONS.rc\"\n\n");
}



/* ---------------------------------------------------------------------- */
/* |                                                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         WriteHeaders (char *fname, FILE * file)

#else  /* __STDC__ */
static void         WriteHeaders (fname, file)
char               *fname;
FILE               *file;

#endif /* __STDC__ */

{
   PtrAppName          menuAction;

   fprintf (file, "/* Included headerfiles */\n");
   fprintf (file, "#include \"thot_gui.h\"\n");
   fprintf (file, "#include \"thot_sys.h\"\n");
   fprintf (file, "#include \"application.h\"\n#include \"app.h\"\n");
   fprintf (file, "#include \"interface.h\"\n");
   fprintf (file, "#include \"%s.h\"\n", fname);

   /* regarde s'il y a des actions de menus non standard */
   menuAction = ActionsUsed;
   while (menuAction != NULL)
      if (!menuAction->AppStandardName)
	{
	   fprintf (file, "#include \"document.h\"\n");
	   fprintf (file, "#include \"view.h\"\n");
	   menuAction = NULL;
	}
      else
	 menuAction = menuAction->AppNextName;
}


/* ---------------------------------------------------------------------- */
/* |    WriteAnStructEvent generates event/action initialisations       | */
/* |       for one specific event.                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         WriteAnEvent (char *eventName, PtrActionBindingList binding)

#else  /* __STDC__ */
static void         WriteAnEvent (eventName, binding)
char               *eventName;
PtrActionBindingList binding;

#endif /* __STDC__ */
{

   if (binding == NULL)
      return;

   WriteAnEvent (eventName, binding->next);
   fprintf (outputFile, "  InitEventActions(appliActions, %d, %s, %d, \"%s\");\n",
	    binding->type,
	    eventName,
	    binding->pre,
	    binding->action->ActionName);
}


/* ---------------------------------------------------------------------- */
/* |    WriteEventsList generates event/action initialisation calls.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         WriteEventsList (PtrExternAppliList pAppli)

#else  /* __STDC__ */
static void         WriteEventsList (pAppli)
PtrExternAppliList  pAppli;

#endif /* __STDC__ */

{
   int                 i;

   fprintf (outputFile, "   /* Generate the event/action entries */\n");
   for (i = 0; i <= TteExit; i++)
      WriteAnEvent (AppwrEvent (i), pAppli->eventsList[i]);
   fprintf (outputFile, "\n");
}

/* ---------------------------------------------------------------------- */
/* |    PrintSubMenu                                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PrintSubMenu (PtrAppMenuItem item, WindowType winType, char *schemaName, char *menuName)

#else  /* __STDC__ */
static void         PrintSubMenus (item, winType, schemaName, menuName)
PtrAppMenuItem      item;
WindowType          winType;
char               *schemaName;
char               *menuName;

#endif /* __STDC__ */

{
   PtrAppMenuItem      subitem;
   int                 itemsNumber;

   /* calcule le nombre d'entrees dans le sous-menu */
   subitem = item->AppSubMenu;
   itemsNumber = 0;
   while (subitem != NULL)
     {
	itemsNumber++;
	subitem = subitem->AppNextItem;
     }
   /*fprintf(outputFile, "\n"); */
   fprintf (outputFile, "  TteAddSubMenu(");
   switch (winType)
	 {
	    case MainWindow:
	       fprintf (outputFile, "MainWindow");
	       break;
	    case DocWindow:
	       fprintf (outputFile, "DocWindow");
	       break;
	    case DocTypeWindow:
	       fprintf (outputFile, "DocTypeWindow");
	       break;
	 }
   fprintf (outputFile, ", \"%s\", %s, %s, %d);\n", schemaName, menuName,
	    item->AppItemName, itemsNumber);

   /* traite la liste des items du sous-menu */
   subitem = item->AppSubMenu;
   while (subitem != NULL)
     {
	fprintf (outputFile, "    TteAddMenuItem(");
	switch (winType)
	      {
		 case MainWindow:
		    fprintf (outputFile, "MainWindow");
		    break;
		 case DocWindow:
		    fprintf (outputFile, "DocWindow");
		    break;
		 case DocTypeWindow:
		    fprintf (outputFile, "DocTypeWindow");
		    break;
	      }
	fprintf (outputFile, ", \"%s\", %s, %s", schemaName, menuName, item->AppItemName);
	if (subitem->AppItemName == NULL)
	   fprintf (outputFile, ", 0");
	else
	   fprintf (outputFile, ", %s", subitem->AppItemName);
	if (subitem->AppItemActionName == NULL)
	   fprintf (outputFile, ", NULL, '%c');\n", subitem->AppItemType);
	else
	   fprintf (outputFile, ", \"%s\", '%c');\n", subitem->AppItemActionName, subitem->AppItemType);

	/* item suivant */
	subitem = subitem->AppNextItem;
     }
}


/* ---------------------------------------------------------------------- */
/* |    PrintMenus                                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         PrintMenus (PtrAppMenu firstMenu, WindowType winType, char *schemaName)

#else  /* __STDC__ */
static void         PrintMenus (firstMenu, winType, schemaName)
PtrAppMenu          firstMenu;
WindowType          winType;
char               *schemaName;

#endif /* __STDC__ */

{
   PtrAppMenu          menu;
   PtrAppMenuItem      item;
   int                 itemsNumber;

   if (firstMenu == NULL)
     {
	fprintf (outputFile, "  TteZeroMenu(");
	switch (winType)
	      {
		 case MainWindow:
		    fprintf (outputFile, "MainWindow");
		    break;
		 case DocWindow:
		    fprintf (outputFile, "DocWindow");
		    break;
		 case DocTypeWindow:
		    fprintf (outputFile, "DocTypeWindow");
		    break;
	      }
	fprintf (outputFile, ", \"%s\");\n", schemaName);
     }
   else
     {
	menu = firstMenu;
	while (menu != NULL)
	  {
	     /* calcule le nombre d'entrees dans le menu */
	     itemsNumber = 0;
	     item = menu->AppMenuItems;
	     while (item != NULL)
	       {
		  itemsNumber++;
		  item = item->AppNextItem;
	       }

	     /*fprintf(outputFile, "\n"); */
	     fprintf (rcFile, "    POPUP ");
	     fprintf (outputFile, "  TteAddMenu(");
	     switch (winType)
		   {
		      case MainWindow:
			 fprintf (outputFile, "MainWindow");
			 break;
		      case DocWindow:
			 fprintf (outputFile, "DocWindow");
			 break;
		      case DocTypeWindow:
			 fprintf (outputFile, "DocTypeWindow");
			 break;
		   }
	     fprintf (rcFile, "\"&%s\" {\n", menu->AppMenuName);
	     fprintf (outputFile, ", \"%s\", %d, %s, %d", schemaName,
		      menu->AppMenuView, menu->AppMenuName, itemsNumber);
	     /* Declare les menus dynamiques */
	     if (!strcmp (menu->AppMenuName, "Attributes_"))
	       {
		  fprintf (outputFile, ", \"MenuAttribute\");\n");
		  fprintf (outputFile, "  AttributeMenuLoadResources();\n");
	       }
	     else if (!strcmp (menu->AppMenuName, "Selection_"))
	       {
		  fprintf (outputFile, ", \"MenuSelection\");\n");
		  fprintf (outputFile, "  SelectionMenuLoadResources();\n");
	       }
	     else if (!strcmp (menu->AppMenuName, "Help_"))
	       {
		  fprintf (outputFile, ", \"MenuHelp\");\n");
	       }
	     else
		fprintf (outputFile, ", \"\");\n");

	     /* traite la liste des items du menu */
	     item = menu->AppMenuItems;
	     while (item != NULL)
	       {
		  if (item->AppSubMenu != NULL)
		     /* traitement du sous-menu */
		     PrintSubMenu (item, winType, schemaName, menu->AppMenuName);
		  else
		    {
		       fprintf (rcFile, "        MENUITEM ");
		       fprintf (outputFile, "    TteAddMenuItem(");
		       switch (winType)
			     {
				case MainWindow:
				   fprintf (outputFile, "MainWindow");
				   break;
				case DocWindow:
				   fprintf (outputFile, "DocWindow");
				   break;
				case DocTypeWindow:
				   fprintf (outputFile, "DocTypeWindow");
				   break;
			     }
		       /* update the Resource file for windows */
		       if (item->AppItemType == 'S')
			  fprintf (rcFile, "SEPARATOR\n");
		       else if (item->AppItemActionName == NULL)
			  fprintf (rcFile, "\"Noname\",\t\t%s\n", item->AppItemName);
		       else
			  fprintf (rcFile, "\"%s\",\t\t%s\n",
				item->AppItemActionName, item->AppItemName);
		       fprintf (outputFile, ", \"%s\", %s, -1", schemaName, menu->AppMenuName);
		       if (item->AppItemName == NULL)
			  fprintf (outputFile, ", 0");
		       else
			  fprintf (outputFile, ", %s", item->AppItemName);
		       if (item->AppItemActionName == NULL)
			  fprintf (outputFile, ", NULL, '%c');\n", item->AppItemType);
		       else
			  fprintf (outputFile, ", \"%s\", '%c');\n", item->AppItemActionName, item->AppItemType);
		    }
		  /* item suivant */
		  item = item->AppNextItem;
	       }
	     fprintf (rcFile, "    }\n");
	     menu = menu->AppNextMenu;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    WriteAppliInit                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         WriteAppliInit (char *fname, PtrExternAppliList pAppli)

#else  /* __STDC__ */
static void         WriteAppliInit (fname, pAppli)
char               *fname;
PtrExternAppliList  pAppli;

#endif /* __STDC__ */

{
   if (strcmp (fname, "EDITOR") == 0)
     {
	fprintf (outputFile, "#include \"logo.xpm\"\n#include \"logo.xbm\"\n#include \"message.h\"\n\n");
	fprintf (outputFile, "int    appArgc;\nchar    **appArgv;\n");
	fprintf (outputFile, "Pixmap image;  /* logo pixmap */\n");
	fprintf (outputFile, "Pixmap image;  /* logo pixmap */\n");
	fprintf (outputFile, "Pixmap icon;   /* icon pixmap */\n\n");
     }
   fprintf (outputFile, "\n/* ---------------------------------------------------------------------- */\n");
   fprintf (outputFile, "void %sApplicationInitialise()\n", fname);
   fprintf (outputFile, "{\n PtrExternAppliList appliActions;\n\n");
   fprintf (outputFile, "  /* Create the new application context*/\n");
   fprintf (outputFile, "  appliActions = MakeNewApplicationStruct(%d, \"%s\");\n",
	    pAppli->strId, pAppli->appId);

   WriteEventsList (pAppli);
   fprintf (outputFile, "}\n\n");
}


/* ---------------------------------------------------------------------- */
/* |                                                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         WriteActionList (char *fname)
#else  /* __STDC__ */
static void         WriteActionList (fname)
char               *fname;

#endif /* __STDC__ */
{
   PtrAction           action;
   char                actionFname[50];
   FILE               *actionFile;
   char                s[200];
   PtrAppDocType       menusDoc;
   PtrAppName          SchUsed;
   PtrAppName          menuAction;
   int                 nbActions;
   boolean             editingResource;
   boolean             structSelectResource;

   strcpy (actionFname, fname);
   strcat (actionFname, "actions.c.skel");
   actionFile = fopen (actionFname, "w");	/* Opens the skeleton action-file */

   WriteHeaders (fname, actionFile);	/* Writes the standard headers to the action-file */

   fprintf (outputFile, "/*########## Init Appplication action-list #################*/\n");
   /* Traite la commande USES */
   if (SchemasUsed != NULL)
     {
	SchUsed = SchemasUsed;
	while (SchUsed != NULL)
	  {
	     fprintf (outputFile, "extern void %sLoadResources();\n",
		      SchUsed->AppNameValue);
	     SchUsed = SchUsed->AppNextName;
	  }
     }

   action = externalActions;
   fprintf (outputFile, "#ifdef __STDC__\n");
   while (action != NULL)
     {
	fprintf (actionFile, "\n/* ---------------------------------------------------------------------- */\n");
	fprintf (actionFile, "#ifdef __STDC__\n");
	strcpy (s, "#else /* __STDC__*/\n");
	fprintf (outputFile, "extern ");
	if (strcmp (action->ActionName, APP_NOOPCHAR))
	  {
	     /* Output the data to the actions-file */
	     if (action->actPre)
	       {
		  fprintf (outputFile, "boolean %s(", action->ActionName);
		  fprintf (actionFile, "boolean %s(", action->ActionName);
		  strcat (s, "boolean ");
		  strcat (s, action->ActionName);
		  strcat (s, "(");
	       }
	     else
	       {
		  fprintf (outputFile, "void %s(", action->ActionName);
		  fprintf (actionFile, "void %s(", action->ActionName);
		  strcat (s, "void ");
		  strcat (s, action->ActionName);
		  strcat (s, "(");
	       }

	     switch (action->actEvent)
		   {
		      case TteAttrMenu:
		      case TteAttrCreate:
		      case TteAttrModify:
		      case TteAttrRead:
		      case TteAttrSave:
		      case TteAttrExport:
		      case TteAttrDelete:
			 fprintf (outputFile, "NotifyAttribute *event);\n");
			 fprintf (actionFile, "NotifyAttribute *event)\n");
			 strcat (s, "event)\n     NotifyAttribute *event;\n");
			 break;
		      case TteElemMenu:
		      case TteElemNew:
		      case TteElemRead:
		      case TteElemSave:
		      case TteElemExport:
		      case TteElemDelete:
		      case TteElemSelect:
		      case TteElemExtendSelect:
		      case TteElemActivate:
		      case TteElemInclude:
		      case TteElemCopy:
		      case TteElemChange:
		      case TteElemMove:
			 fprintf (outputFile, "NotifyElement *event);\n");
			 fprintf (actionFile, "NotifyElement *event)\n");
			 strcat (s, "event)\n     NotifyElement *event;\n");
			 break;
		      case TteElemSetReference:
		      case TteElemTextModify:
			 fprintf (outputFile, "NotifyOnTarget *event);\n");
			 fprintf (actionFile, "NotifyOnTarget *event)\n");
			 strcat (s, "event)\n     NotifyOnTarget *event;\n");
			 break;
		      case TteElemPaste:
			 if (action->actPre)
			   {
			      fprintf (outputFile, "NotifyOnValue *event);\n");
			      fprintf (actionFile, "NotifyOnValue *event)\n");
			      strcat (s, "event)\n     NotifyOnValue *event;\n");
			   }
			 else
			   {
			      fprintf (outputFile, "NotifyElement *event);\n");
			      fprintf (actionFile, "NotifyElement *event)\n");
			      strcat (s, "event)\n     NotifyElement *event;\n");
			   }
			 break;
		      case TteElemGraphModify:
			 fprintf (outputFile, "NotifyOnValue *event);\n");
			 fprintf (actionFile, "NotifyOnValue *event)\n");
			 strcat (s, "event)\n     NotifyOnValue *event;\n");
			 break;
		      case TtePRuleCreate:
		      case TtePRuleModify:
		      case TtePRuleDelete:
			 fprintf (outputFile, "NotifyPresentation *event);\n");
			 fprintf (actionFile, "NotifyPresentation *event)\n");
			 strcat (s, "event)\n     NotifyPresentation *event;\n");
			 break;
		      case TteDocOpen:
		      case TteDocCreate:
		      case TteDocClose:
		      case TteDocSave:
		      case TteDocExport:
		      case TteViewOpen:
		      case TteViewClose:
			 fprintf (outputFile, "NotifyDialog *event);\n");
			 fprintf (actionFile, "NotifyDialog *event)\n");
			 strcat (s, "event)\n     NotifyDialog *event;\n");
			 break;
		      case TteViewResize:
		      case TteViewScroll:
			 fprintf (outputFile, "NotifyWindow *event);\n");
			 fprintf (actionFile, "NotifyWindow *event)\n");
			 break;
		      case TteInit:
		      case TteExit:
			 fprintf (outputFile, "NotifyEvent *event);\n");
			 fprintf (actionFile, "NotifyEvent *event)\n");
			 strcat (s, "event)\n     NotifyWindow *event;\n");
			 break;
		      default:
			 break;
		   }
	     fprintf (actionFile, s);
	     fprintf (actionFile, "#endif /* __STDC__*/\n{\n");
	     fprintf (actionFile, "  /* This function has to be written */\n");
	     if (action->actPre)
		fprintf (actionFile, "  return False; /* let Thot perform normal operation */\n");
	     fprintf (actionFile, "}\n\n");
	  }
	action = action->next;
     }

   /* Seconde boucle */
   action = externalActions;
   fprintf (outputFile, "#else /* __STDC__*/\n");
   while (action != NULL)
     {
	fprintf (outputFile, "extern ");
	if (strcmp (action->ActionName, APP_NOOPCHAR))
	  {
	     /* Output the data to the actions-file */
	     if (action->actPre)
		fprintf (outputFile, "boolean %s(", action->ActionName);
	     else
		fprintf (outputFile, "void %s(", action->ActionName);

	     switch (action->actEvent)
		   {
		      case TteAttrMenu:
		      case TteAttrCreate:
		      case TteAttrModify:
		      case TteAttrRead:
		      case TteAttrSave:
		      case TteAttrExport:
		      case TteAttrDelete:
			 fprintf (outputFile, "/* NotifyAttribute *event */);\n");
			 break;
		      case TteElemMenu:
		      case TteElemNew:
		      case TteElemRead:
		      case TteElemSave:
		      case TteElemExport:
		      case TteElemDelete:
		      case TteElemSelect:
		      case TteElemExtendSelect:
		      case TteElemActivate:
		      case TteElemInclude:
		      case TteElemCopy:
		      case TteElemChange:
		      case TteElemMove:
			 fprintf (outputFile, "/* NotifyElement *event */);\n");
			 break;
		      case TteElemSetReference:
		      case TteElemTextModify:
			 fprintf (outputFile, "/* NotifyOnTarget *event */);\n");
			 break;
		      case TteElemPaste:
			 if (action->actPre)
			    fprintf (outputFile, "/* NotifyOnValue *event */);\n");
			 else
			   {
			      fprintf (outputFile, "/* NotifyElement *event */);\n");
			   }
			 break;
		      case TteElemGraphModify:
			 fprintf (outputFile, "/* NotifyOnValue *event */);\n");
			 break;
		      case TtePRuleCreate:
		      case TtePRuleModify:
		      case TtePRuleDelete:
			 fprintf (outputFile, "/* NotifyPresentation *event */);\n");
			 break;
		      case TteDocOpen:
		      case TteDocCreate:
		      case TteDocClose:
		      case TteDocSave:
		      case TteDocExport:
		      case TteViewOpen:
		      case TteViewClose:
			 fprintf (outputFile, "/* NotifyDialog *event */);\n");
			 break;
		      case TteViewResize:
		      case TteViewScroll:
			 fprintf (outputFile, "/* NotifyWindow *event */);\n");
			 break;
		      case TteInit:
		      case TteExit:
			 fprintf (outputFile, "/* NotifyEvent *event */);\n");
			 break;
		      default:
			 break;
		   }
	  }

	action = action->next;
     }

   fprintf (outputFile, "#endif /* __STDC__*/\n\n");
   fprintf (outputFile, "void %sActionListInit()\n", fname);
   fprintf (outputFile, "{\n");

   action = externalActions;
   while (action != NULL)
     {
	fprintf (outputFile, "  InsertAction(\"%s\", (Proc)%s);\n",
		 action->ActionName,
		 action->ActionName);
	action = action->next;
     }

   fprintf (outputFile, "}\n\n");

   if (strcmp (fname, "EDITOR") == 0)
     {
	fprintf (rcFile, "EDITORAPP MENU {\n");

	/* c'est bien EDITOR.A qu'on compile, on ajoute le main avec les
	   initialisation qu'il faut */
	structSelectResource = False;	/* no structselectResource */
	fprintf (outputFile, "\nvoid TteLoadApplications()\n");
	fprintf (outputFile, "{\n");
	fprintf (outputFile, "  %sActionListInit();\n", fname);
	fprintf (outputFile, "  %sApplicationInitialise();\n", fname);
	/* Traite la commande USES */
	if (SchemasUsed != NULL)
	  {
	     SchUsed = SchemasUsed;
	     while (SchUsed != NULL)
	       {
		  if (!strcmp (SchUsed->AppNameValue, "StructSelect"))
		     structSelectResource = True;
		  fprintf (outputFile, "  %sLoadResources();\n",
			   SchUsed->AppNameValue);
		  SchUsed = SchUsed->AppNextName;
	       }
	  }
	if (!structSelectResource)
	   fprintf (outputFile, "  NoStructSelectLoadResources();\n");
	fprintf (outputFile, "}\n\n");

	/* compte les elements de la liste ActionsUsed */
	menuAction = ActionsUsed;
	nbActions = 0;
	fprintf (outputFile, "#ifdef __STDC__\n");
	while (menuAction != NULL)
	  {
	     nbActions++;
	     if (!menuAction->AppStandardName)
		/* declare les nouvelles fonctions non standard des menus */
		fprintf (outputFile, "extern void %s(Document document, View view);\n",
			 menuAction->AppNameValue);
	     menuAction = menuAction->AppNextName;
	  }
	/* Seconde boucle */
	fprintf (outputFile, "#else /* __STDC__*/\n");
	menuAction = ActionsUsed;
	while (menuAction != NULL)
	  {
	     if (!menuAction->AppStandardName)
		/* declare les nouvelles fonctions non standard des menus */
		fprintf (outputFile, "extern void %s(/*Document document, View view*/);\n",
			 menuAction->AppNameValue);
	     menuAction = menuAction->AppNextName;
	  }
	fprintf (outputFile, "#endif /* __STDC__*/\n\n");
	fprintf (outputFile, "/*################### Main program #########################*/\n");
	fprintf (outputFile, "#ifdef _WINDOWS\n");
	fprintf (outputFile, "#ifdef __STDC__\n");
	fprintf (outputFile, "int thotmain(int argc, char **argv)\n");
	fprintf (outputFile, "#else /* __STDC__ */\n");
	fprintf (outputFile, "int thotmain(argc, argv)\n");
	fprintf (outputFile, "        int argc;\n");
	fprintf (outputFile, "        char **argv;\n");
	fprintf (outputFile, "#endif /* __STDC__ */\n");
	fprintf (outputFile, "#else /* _WINDOWS */\n");
	fprintf (outputFile, "#ifdef __STDC__\n");
	fprintf (outputFile, "int main(int argc, char **argv)\n");
	fprintf (outputFile, "#else /* __STDC__ */\n");
	fprintf (outputFile, "int main(argc, argv)\n");
	fprintf (outputFile, "        int argc;\n");
	fprintf (outputFile, "        char **argv;\n");
	fprintf (outputFile, "#endif /* __STDC__ */\n");
	fprintf (outputFile, "#endif /* _WINDOWS */\n");
	fprintf (outputFile, "{\n");

	fprintf (outputFile, "  int lg; /* identify dialogue messages */\n");
	fprintf (outputFile, "  char appName[MAX_PATH]; /* name of the application */\n");
	fprintf (outputFile, "  char workName[MAX_PATH]; /* path of the application */\n");

	fprintf (outputFile, "  /* initialize the Registry */\n");
	fprintf (outputFile, "  TtaInitializeAppRegistry(argv[0]);\n");
	fprintf (outputFile, "  /* save argc and argv */\n");
	fprintf (outputFile, "  appArgc = argc;\n  appArgv = argv;\n");
	fprintf (outputFile, "  /* extract the name of the application */\n");
	fprintf (outputFile, "  TtaExtractName(argv[0], workName, appName);\n");
	fprintf (outputFile, "  /* application name is limited to 19 characters */\n");
	fprintf (outputFile, "  lg = strlen(appName);\n");
	fprintf (outputFile, "  if (lg > 19)\n");
	fprintf (outputFile, "    appName[19] = (char)0;\n");
	fprintf (outputFile, "  TtaInitialize(appName);\n");
	fprintf (outputFile, "\n  TteInitMenus(appName, %d);\n", nbActions);


	editingResource = False;	/* no editing Resource loaded by default */
	menuAction = ActionsUsed;
	while (menuAction != NULL)
	  {
	     fprintf (outputFile, "  TteAddMenuAction(\"%s\", (Proc)%s);\n",
		      menuAction->AppNameValue, menuAction->AppNameValue);
	     if (!menuAction->AppStandardName)
		/* ecrit les fonctions non standard des menus dans XXXaction.c */
	       {
		  fprintf (actionFile, "\n/* -------------------------------------------------------------- */\n");
		  fprintf (actionFile, "#ifdef __STDC__\n");
		  fprintf (actionFile, "void %s(Document document, View view)\n",
			   menuAction->AppNameValue);
		  fprintf (actionFile, "#else /* __STDC__*/\n");
		  fprintf (actionFile, "void %s(document, view)\n",
			   menuAction->AppNameValue);
		  fprintf (actionFile, "     Document document;\n     View view;\n");
		  fprintf (actionFile, "#endif /* __STDC__*/\n{\n");
		  fprintf (actionFile, "  /* This function has to be written */\n");
		  fprintf (actionFile, "}\n\n");
	       }
	     else
	       {
		  /* Is it an editing standard function ? */
		  if (!strcmp (menuAction->AppNameValue, "TtcInsert")
		      || !strcmp (menuAction->AppNameValue, "TtcPasteFromClipboard")
		      || !strcmp (menuAction->AppNameValue, "TtcPaste")
		    || !strcmp (menuAction->AppNameValue, "TtcCutSelection")
		  || !strcmp (menuAction->AppNameValue, "TtcDeleteSelection")
		      || !strcmp (menuAction->AppNameValue, "TtcInsertChar")
		      || !strcmp (menuAction->AppNameValue, "TtcDeletePreviousChar")
		   || !strcmp (menuAction->AppNameValue, "TtcCopySelection")
		      || !strcmp (menuAction->AppNameValue, "TtcInclude"))
		     editingResource = True;
	       }
	     menuAction = menuAction->AppNextName;
	  }

	fprintf (outputFile, "\n");
	PrintMenus (MainWindowMenus, MainWindow, "MAIN");

	fprintf (outputFile, "\n");
	PrintMenus (DocWindowMenus, DocWindow, "DOC");

	menusDoc = DocTypeMenus;
	while (menusDoc != NULL)
	  {
	     fprintf (outputFile, "\n");
	     PrintMenus (menusDoc->AppDocTypeMenus, DocTypeWindow,
			 menusDoc->AppDocTypeName);
	     menusDoc = menusDoc->AppNextDocType;
	  }

	fprintf (outputFile, "  /* load appName+\"dialogue\" message file */\n");
	fprintf (outputFile, "  strcpy(workName, appName);\n");
	fprintf (outputFile, "  strcat(workName, \"dialogue\");\n");
	fprintf (outputFile, "  TtaGetMessageTable(workName, MAX_EDITOR_LABEL);\n");
	/* if necessary load editing Resources */
	if (editingResource)
	   fprintf (outputFile, "  EditingLoadResources();\n");
	fprintf (outputFile, "\n\n  image = TtaCreatePixmapLogo(logo_xpm);\n");
	fprintf (outputFile, "  icon = TtaCreateBitmapLogo(logo_width, logo_height, logo_bits);\n");
	fprintf (outputFile, "  TteOpenMainWindow(appName, image, icon);\n");
	fprintf (outputFile, "  TtaMainLoop();\n  exit(0);\n");
	fprintf (outputFile, "}\n\n");

	fprintf (rcFile, "}\n");
     }

   fprintf (outputFile, "void %sLoadResources()\n", fname);
   fprintf (outputFile, "{\n");

   fprintf (outputFile, "  %sActionListInit();\n", fname);
   fprintf (outputFile, "  %sApplicationInitialise();\n}\n\n", fname);

   fclose (actionFile);
}


/* ---------------------------------------------------------------------- */
/* |                                                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         WriteDotHFile (FILE * dotHFile, FILE * dialogueFile)
#else  /* __STDC__ */
static void         WriteDotHFile (dotHFile, dialogueFile)
FILE               *dotHFile;
FILE               *dialogueFile;

#endif /* __STDC__ */
{
   int                 i, num;
   PtrAppName          name;

   fprintf (dotHFile, "/* Basic types */\n");
   for (i = 0; i < pSchStr->SsNRules; i++)
      fprintf (dotHFile, "#define %s  %d\n", pSchStr->SsRule[i].SrName, i+1);

   /* liste tous les noms de menus et d'items reellement utilises */
   num = 0;
   fprintf (dotHFile, "\n/* Pull-down menus */\n");
   name = MenusUsed;
   while (name != NULL)
     {
	fprintf (dotHFile, "#define %s  %d\n", name->AppNameValue, num);
	fprintf (dialogueFile, "%d  %s\n", num, name->AppNameValue);
	name = name->AppNextName;
	num++;
     }

   fprintf (dotHFile, "\n/* Pull-down menu items */\n");
   name = ItemsUsed;
   while (name != NULL)
     {
	if (name->AppNameValue != NULL)
	  {
	     fprintf (dotHFile, "#define %s  %d\n", name->AppNameValue, num);
	     fprintf (dialogueFile, "%d  %s\n", num, name->AppNameValue);
	     num++;
	  }
	name = name->AppNextName;
     }
   fprintf (dotHFile, "\n#define MAX_EDITOR_LABEL       %d\n", num + 1);
}


/* ---------------------------------------------------------------------- */
/* |    WrSchAPP cree le fichier de sortie et y ecrit le schema APP.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                WrSchAPP (char *fname, PtrExternAppliList pAppli, PtrSSchema pSchStr)

#else  /* __STDC__ */
void                WrSchAPP (fname, pAppli, pSchStr)
char               *fname;
PtrExternAppliList  pAppli;
PtrSSchema        pSchStr;

#endif /* __STDC__ */

{
   int                 i;
   int                 lg, ht;
   char              **ptr;
   unsigned char      *bits;
   char               *fileSuffix;
   FILE               *dotHFile;
   FILE               *infoFILE;
   Name                 rcFileName;

   i = 0;
   fileSuffix = strdup (fname);
   /* met le suffixe APP.rc a la fin du nom de fichier */
   sprintf (rcFileName, "%sAPP.rc", fname);
   rcFile = fopen (rcFileName, "w");
   WriteRcHeaders (fileSuffix, rcFile);

   /* met le suffixe APP.c a la fin du nom de fichier */
   while (fname[i] != ' ' && fname[i] != '\0')
      i++;
   fname[i] = '\0';
   strcpy (&fname[i], "APP.c");

   /* cree le fichier */
   outputFile = fopen (fname, "w");
   WriteHeaders (fileSuffix, outputFile);
   WriteAppliInit (fileSuffix, pAppli);
   WriteActionList (fileSuffix);
   fclose (outputFile);

   /* cree les fichiers EDITOR.h et EDITORdialogue */
   if (strcmp ("EDITOR", fileSuffix) == 0)
     {
	dotHFile = fopen ("EDITOR.h", "w");
	infoFILE = fopen ("EDITORdialogue", "w");
	rcFile = fopen ("EDITOR.rc", "w");
	WriteDotHFile (dotHFile, infoFILE);
	fclose (infoFILE);
	fclose (dotHFile);
	if (!FileExist ("logo.xpm"))
	  {
	     /* cree le fichier logo.xpm */
	     infoFILE = fopen ("logo.xpm", "w");
	     fprintf (infoFILE, "/* XPM */\nstatic char * logo_xpm[] = {\n");
	     ptr = logo_xpm;
	     lg = sizeof (logo_xpm) / sizeof (char *) - 1;

	     for (i = 0; i < lg; i++)
	       {
		  fprintf (infoFILE, "\"%s\",\n", ptr[i]);
	       }
	     fprintf (infoFILE, "\"%s\"};\n", ptr[i]);
	     fclose (infoFILE);
	  }
	if (!FileExist ("logo.xbm"))
	  {
	     /* cree le fichier logo.xbm */
	     infoFILE = fopen ("logo.xbm", "w");
	     lg = logo_width;
	     ht = logo_height;
	     fprintf (infoFILE, "#define logo_width %d\n", lg);
	     fprintf (infoFILE, "#define logo_height %d\n", ht);
	     fprintf (infoFILE, "static unsigned char logo_bits[] = {\n");
	     bits = logo_bits;
	     lg = sizeof (logo_bits) / sizeof (unsigned char) - 1;

	     i = 0;
	     while (i < lg)
	       {
		  fprintf (infoFILE, " 0x%.2x,", (unsigned int) bits[i]);
		  i++;
		  if (i % 12 == 0)
		     fprintf (infoFILE, "\n");
	       }
	     fprintf (infoFILE, " 0x%.2x, };\n", (unsigned int) bits[i]);
	     fclose (infoFILE);
	  }
     }
   fclose (rcFile);
   TtaFreeMemory (fileSuffix);
}
