
/* -- Copyright (c) 1990 - 1995 Inria/CNRS  All rights reserved. -- */

/*
   config.c : gestion des fichiers de configuration et de langue.
   V. Quint : novembre 1995

   ajout typography
   H. Richy : fevrier 1996
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "thotdir.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "frame.h"
#include "language.h"
#include "application.h"
#include "appdialogue.h"

#undef EXPORT
#define EXPORT extern
#include "environ.var"
#include "edit.var"
#include "frame.var"
#include "appdialogue.var"

#define MAX_ITEM_CONF 100

#include "docvues.f"
#include "dofile.f"
#include "environ.f"
#include "inites.f"
#include "memory.f"
#include "message.f"
#include "thotmsg.f"

static char        *doc_items[MAX_ITEM_CONF];
static boolean      doc_import[MAX_ITEM_CONF];
static char        *doc_items_menu[MAX_ITEM_CONF];
static char        *nat_items[MAX_ITEM_CONF];
static char        *nat_items_menu[MAX_ITEM_CONF];
static char        *ext_items[MAX_ITEM_CONF];
static char        *ext_items_menu[MAX_ITEM_CONF];
static char        *pres_items[MAX_ITEM_CONF];
static char        *pres_items_menu[MAX_ITEM_CONF];
static char        *export_items[MAX_ITEM_CONF];
static char        *export_items_menu[MAX_ITEM_CONF];

/* ---------------------------------------------------------------------- */
/* |    ConfigInit initialise le module de configuration.               | */
/* ---------------------------------------------------------------------- */
void                ConfigInit ()
{
   int                 i;

   for (i = 0; i < MAX_ITEM_CONF; i++)
     {
	doc_items[i] = NULL;
	doc_import[i] = False;
	doc_items_menu[i] = NULL;
	nat_items[i] = NULL;
	nat_items_menu[i] = NULL;
	ext_items[i] = NULL;
	ext_items_menu[i] = NULL;
	pres_items[i] = NULL;
	pres_items_menu[i] = NULL;
	export_items[i] = NULL;
	export_items_menu[i] = NULL;
     }
}

/* ---------------------------------------------------------------------- */
/* |    getFirstWord                                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         getFirstWord (unsigned char *line, unsigned char *mot)
#else  /* __STDC__ */
static void         getFirstWord (line, mot)
unsigned char      *line;
unsigned char      *mot;

#endif /* __STDC__ */
{
   int                 indmot, indline;

   indline = 0;
   mot[0] = '\0';
   /* saute les espaces de debut de ligne */
   while (line[indline] <= ' ' && line[indline] != '\0')
      indline++;
   if (line[indline] == '#')
      /* cette ligne ne comporte qu'un commentaire */
      return;
   /* copie tous les caracteres jusqu'a rencontrer le 1er espace ou ":" */
   /* ou la fin de ligne */
   indmot = 0;
   while (line[indline] > ' ' && line[indline] != ':' &&
	  line[indline] != '\0')
      mot[indmot++] = line[indline++];
   /* marque la fin du mot trouve' */
   mot[indmot] = '\0';
}


/* ---------------------------------------------------------------------- */
/* |    getSecondWord                                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         getSecondWord (unsigned char *line, unsigned char *mot)
#else  /* __STDC__ */
static void         getSecondWord (line, mot)
unsigned char      *line;
unsigned char      *mot;

#endif /* __STDC__ */
{
   int                 indmot, indline;

   indline = 0;
   mot[0] = '\0';
   /* saute les espaces de debut de ligne */
   while (line[indline] <= ' ' && line[indline] != '\0')
      indline++;
   if (line[indline] == '#')
      /* cette ligne ne comporte qu'un commentaire */
      return;
   /* saute le 1er mot, jusqu'a rencontrer le 1er espace */
   /* ou la fin de ligne */
   while (line[indline] > ' ' && line[indline] != '\0')
      indline++;
   /* saute les espaces qui suivent le 1er mot */
   while (line[indline] <= ' ' && line[indline] != '\0')
      indline++;
   if (line[indline] == '#')
      /* le premier mot est suivi d'un commentaire */
      return;
   /* copie tous les caracteres du 2eme mot jusqu'a rencontrer le 1er */
   /* espace ou la fin de ligne */
   indmot = 0;
   while (line[indline] > ' ' && line[indline] != '\0')
      mot[indmot++] = line[indline++];
   /* marque la fin du mot trouve' */
   mot[indmot] = '\0';
}

/* ---------------------------------------------------------------------- */
/* |    singleWord                                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      singleWord (unsigned char *line)
#else  /* __STDC__ */
static boolean      singleWord (line)
unsigned char      *line;

#endif /* __STDC__ */
{
   int                 ind;

   ind = 0;
   /* saute les espaces de debut de ligne */
   while (line[ind] <= ' ' && line[ind] != '\0')
      ind++;
   if (line[ind] == '#')
      /* la ligne commence par un commentaire */
      return False;
   /* saute le premier mot */
   while (line[ind] > ' ' && line[ind] != '#' && line[ind] != ':' && line[ind] != '\0')
      ind++;
   /* saute les espaces qui suivent le 1er mot */
   while (line[ind] <= ' ' && line[ind] != '\0')
      ind++;
   if (line[ind] == '#' || line[ind] == '\0')
      /* il ne reste rien dans la ligne ou seulement un commentaire */
      return True;
   else
      /* il y a des caracteres significatifs apres le 1er mot */
      return False;
}


/* ---------------------------------------------------------------------- */
/* |    getStringAfterColon                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         getStringAfterColon (unsigned char *line, unsigned char *texte)
#else  /* __STDC__ */
static void         getStringAfterColon (line, texte)
unsigned char      *line;
unsigned char      *texte;

#endif /* __STDC__ */
{
   int                 indline, indtext;

   indline = 0;
   texte[0] = '\0';
   while (line[indline] != ':' && line[indline] != '\0')
      indline++;
   if (line[indline] == ':')
     {
	indline++;
	while (line[indline] <= ' ' && line[indline] != '\0')
	   indline++;
	if (line[indline] == '#' || line[indline] == '\0')
	   return;
	indtext = 0;
	while (line[indline] != '#' && line[indline] != '\0')
	   texte[indtext++] = line[indline++];
	/* elimine les blancs de fin de ligne */
	indtext--;
	while (texte[indtext] <= ' ' && indtext >= 0)
	   indtext--;
	indtext++;
	/* termine la chaine */
	texte[indtext] = '\0';
     }
}


/* ---------------------------------------------------------------------- */
/* |    readUntil       lit le fichier file (qui doit etre ouvert)      | */
/* |            jusqu'a trouver une ligne qui contienne un seul mot,    | */
/* |            soit word1 soit word2.                                  | */
/* |            Retourne                                                | */
/* |               0 si on ne trouve pas cette ligne,                   | */
/* |               1 si on trouve le 1er mot,                           | */
/* |               2 si on trouve le 2eme mot.                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      readUntil (FILE * file, char *word1, char *word2)
#else  /* __STDC__ */
static boolean      readUntil (file, word1, word2)
FILE               *file;
char               *word1;
char               *word2;

#endif /* __STDC__ */
{
   boolean             stop;
   int                 ret;
   char                line[MAX_TXT_LEN];
   char                mot[MAX_TXT_LEN];

   stop = False;
   ret = 0;
   do
      if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	 /* fin de fichier */
	 stop = True;
      else
	{
	   getFirstWord (line, mot);
	   if (singleWord (line))
	     {
		if (*word1 != '\0')
		   if (strcmp (mot, word1) == 0)
		      ret = 1;
		if (*word2 != '\0')
		   if (strcmp (mot, word2) == 0)
		      ret = 2;
	     }
	}
   while (!stop && ret == 0);
   return ret;
}

/* ---------------------------------------------------------------------- */
/* |    namesOfDocType                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         namesOfDocType (char *fname, char **doctypeOrig, char **doctypeTrans, int *typ, boolean * import)
#else  /* __STDC__ */
static void         namesOfDocType (fname, doctypeOrig, doctypeTrans, typ, import)
char               *fname;
char              **doctypeOrig;
char              **doctypeTrans;
int                *typ;
boolean            *import;

#endif /* __STDC__ */
{
   int                 i, l, point, res;
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                texte[MAX_TXT_LEN];
   char                mot[MAX_TXT_LEN];
   boolean             stop;

   *doctypeOrig = NULL;
   *doctypeTrans = NULL;
   *typ = 0;
   *import = False;
   /* ouvre le fichier */
   file = fopen (fname, "r");
   if (file == NULL)
     {
	fprintf (stderr, "cannot open file %s\n", fname);
	return;
     }
   /* cherche le premier mot du fichier, hors commentaires et espaces */
   stop = False;
   do
      if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	 stop = True;
      else
	{
	   getFirstWord (line, mot);
	   if (mot[0] != '\0')
	      stop = True;
	}
   while (!stop);
   if (!singleWord (line))
      /* le premier mot n'est pas seul dans la ligne, erreur */
      return;

   if (strcmp (mot, "document") == 0)
      *typ = 1;
   else if (strcmp (mot, "nature") == 0)
      *typ = 2;
   else if (strcmp (mot, "extension") == 0)
      *typ = 3;
   else if (strcmp (mot, "document-nature") == 0)
      *typ = 4;
   else
      /* le premier mot du fichier est invalide */
     {
	fprintf (stderr, "file %s: invalid first word %s\n", fname, mot);
	return;
     }

   /* cherche le "." marquant le suffixe a la fin du nom de fichier */
   i = strlen (fname);
   while (i > 0 && fname[i] != '.')
      i--;
   if (fname[i] == '.')
      point = i;
   else
      point = 0;
   /* cherche le dernier DIR_SEP du nom de fichier */
   while (i > 0 && fname[i] != DIR_SEP)
      i--;
   if (fname[i] == DIR_SEP)
      i++;
   if (fname[i] == '_')
      /* ignore les fichiers dont le nom commence par "-" */
      return;
   l = strlen (&fname[i]) + 1;
   *doctypeOrig = TtaGetMemory (l);
   if (point != 0)
      fname[point] = '\0';
   strcpy (*doctypeOrig, &fname[i]);
   /* retablit le '.' du suffixe dans le nom de fichier */
   if (point != 0)
      fname[point] = '.';

   if (*typ == 1 || *typ == 4)
      /* Il s'agit d'un type de document, on cherche une ligne */
      /* contenant un seul mot: "import" ou "translation" */
      res = readUntil (file, "import", "translation");
   else
      /* il s'agit d'une nature ou d'une extension, on ne cherche */
      /* que la ligne "translation" */
      res = readUntil (file, "", "translation");
   if (res == 1)
      /* on a trouve' le mot "import" */
     {
	*import = True;
	/* cherche la ligne comportant le seul mot "translation" */
	res = readUntil (file, "", "translation");
     }
   if (res == 2)
      /* on a trouve' le mot translation */
     {
	/* on cherche la ligne qui donne la traduction du nom de schema */
	stop = False;
	do
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	     {
		stop = True;
		mot[0] = '\0';
	     }
	   else
	     {
		getFirstWord (line, mot);
		if (strcmp (mot, *doctypeOrig) == 0)
		   stop = True;
	     }
	while (!stop);

	if (strcmp (mot, *doctypeOrig) == 0)
	   /* on a trouve' la ligne voulue */
	  {
	     getStringAfterColon (line, texte);
	     if (texte[0] == '\0')
		fprintf (stderr, "invalid line in file %s\n   %s\n", fname, line);
	     else
	       {
		  *doctypeTrans = TtaGetMemory (strlen (texte) + 1);
		  strcpy (*doctypeTrans, TransCani (texte));
	       }
	  }
     }
   fclose (file);

   /* Si le fichier de configuration ne definit pas de traduction pour */
   /* le nom du schema, on prend le nom d'origine comme traduction */
   if (*doctypeTrans == NULL)
     {
	*doctypeTrans = TtaGetMemory (l);
	strcpy (*doctypeTrans, *doctypeOrig);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ConfigReadConfigFiles (re)initialise les tables des schemas de  | */
/* |            structure (documents, natures et extensions) qui ont    | */
/* |            des fichiers de langue dans les directories de schemas. | */
/* ---------------------------------------------------------------------- */
void                ConfigReadConfigFiles ()

{
   int                 nbitemdoc, nbitemnat, nbitemext;
   int                 debut, i;
   int                 typ;
   boolean             import;
   char               *Dir;
   PathBuffer          DirBuffer;
   ThotDirBrowse       thotDir;

#define NAME_LENGTH     100
#define MAX_NAME         80
#define SELECTOR_NB_ITEMS 5
   char                fname[4 * NAME_LENGTH];
   char               *suffix;
   char               *nomOrig;
   char               *nomTrans;
   boolean             stop;

   suffix = TtaGetVarLANG ();

   /* libere les anciennes entrees des tables de types de documents */
   /* de natures et d'extensions */
   for (i = 0; i < MAX_ITEM_CONF; i++)
     {
	if (doc_items[i] != NULL)
	  {
	     TtaFreeMemory (doc_items[i]);
	     doc_items[i] = NULL;
	  }
	doc_import[i] = False;
	if (doc_items_menu[i] != NULL)
	  {
	     TtaFreeMemory (doc_items_menu[i]);
	     doc_items_menu[i] = NULL;
	  }
	if (nat_items[i] != NULL)
	  {
	     TtaFreeMemory (nat_items[i]);
	     nat_items[i] = NULL;
	  }
	if (nat_items_menu[i] != NULL)
	  {
	     TtaFreeMemory (nat_items_menu[i]);
	     nat_items_menu[i] = NULL;
	  }
	if (ext_items[i] != NULL)
	  {
	     TtaFreeMemory (ext_items[i]);
	     ext_items[i] = NULL;
	  }
	if (ext_items_menu[i] != NULL)
	  {
	     TtaFreeMemory (ext_items_menu[i]);
	     ext_items_menu[i] = NULL;
	  }
     }
   debut = 0;
   i = 0;
   nbitemdoc = 0;
   nbitemnat = 0;
   nbitemext = 0;
   /* traite successivement tous les directories du path des schemas */
   strncpy (DirBuffer, DirectorySchemas, MAX_PATH);
   stop = False;
   while (DirBuffer[i] != '\0' && i < MAX_PATH && !stop)
     {
	while (DirBuffer[i] != PATH_SEP && DirBuffer[i] != '\0' && i < MAX_PATH)
	   i++;
	if (DirBuffer[i] == '\0')
	   /* dernier directory du path. Il faut s'arreter apres ce directory */
	   stop = True;
	if (DirBuffer[i] == PATH_SEP)
	   DirBuffer[i] = '\0';
	if (DirBuffer[i] == '\0')
	   /* un directory de schema a ete isole' */
	  {
	     Dir = &DirBuffer[debut];
	     if (TtaCheckDirectory (Dir))
		/* c'est bien un directory */
	       {
		  /* commande "ls" sur le directory */
		  thotDir.buf = fname;
		  thotDir.bufLen = sizeof (fname);
		  thotDir.mask = ThotDirBrowse_FILES;
		  if (ThotDirBrowse_first (&thotDir, Dir, "*.", suffix) == 1)
		     do
		       {
			  namesOfDocType (fname, &nomOrig, &nomTrans, &typ, &import);
			  if (nomOrig != NULL)
			    {
			       if (typ == 1 || typ == 4)
				 {
				    doc_items[nbitemdoc] = nomOrig;
				    doc_items_menu[nbitemdoc] = nomTrans;
				    if (import)
				       doc_import[nbitemdoc] = True;
				    nbitemdoc++;
				 }
			       if (typ == 2 || typ == 4)
				 {
				    nat_items[nbitemnat] = nomOrig;
				    nat_items_menu[nbitemnat] = nomTrans;
				    nbitemnat++;
				 }
			       if (typ == 3)
				 {
				    ext_items[nbitemext] = nomOrig;
				    ext_items_menu[nbitemext] = nomTrans;
				    nbitemext++;
				 }
			    }
		       }
		     while (ThotDirBrowse_next (&thotDir) == 1);
		  ThotDirBrowse_close (&thotDir);
	       }
	     /* continue a chercher les directories dans le path des schemas */
	     i++;
	     debut = i;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    ConfigMakeDocTypeMenu cree dans BufMenu la liste des schemas de | */
/* |            structure qui ont des fichiers de langue dans les       | */
/* |            directories de schemas.                                 | */
/* |            Si doc == True on prend les schemas de documents, sinon | */
/* |            les schemas de nature.                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 ConfigMakeDocTypeMenu (char *BufMenu, int *lgmenu, boolean doc)

#else  /* __STDC__ */
int                 ConfigMakeDocTypeMenu (BufMenu, lgmenu, doc)
char               *BufMenu;
int                *lgmenu;
boolean             doc;

#endif /* __STDC__ */

{
   int                 nbitem, len, i;

   nbitem = 0;
   *lgmenu = 0;
   BufMenu[0] = '\0';
   i = 0;

   if (doc)
      /* parcourt la table des noms de types de documents */
      while (i < MAX_ITEM_CONF && doc_items[i] != NULL)
	{
	   if (doc_items_menu[i] != NULL)
	      /* cette entree de la table a une traduction, on la prend */
	     {
		len = strlen (doc_items_menu[i]);
		strcpy (BufMenu + (*lgmenu), doc_items_menu[i]);
	     }
	   else
	      /* pas de traduction, on prend le nom d'origine du schema */
	     {
		len = strlen (doc_items[i]);
		strcpy (BufMenu + (*lgmenu), doc_items[i]);
	     }
	   (*lgmenu) += len + 1;
	   nbitem++;
	   i++;
	}
   else
      /* parcourt la table des noms de natures */
      while (i < MAX_ITEM_CONF && nat_items[i] != NULL)
	{
	   if (nat_items_menu[i] != NULL)
	      /* cette entree de la table a une traduction, on la prend */
	     {
		len = strlen (nat_items_menu[i]);
		strcpy (BufMenu + (*lgmenu), nat_items_menu[i]);
	     }
	   else
	      /* pas de traduction, on prend le nom d'origine du schema */
	     {
		len = strlen (nat_items[i]);
		strcpy (BufMenu + (*lgmenu), nat_items[i]);
	     }
	   (*lgmenu) += len + 1;
	   nbitem++;
	   i++;
	}
   return nbitem;
}


/* ---------------------------------------------------------------------- */
/* |    ConfigNomExterneSSchema retourne dans NomUtilisateur le nom     | */
/* |    externe, dans la langue de l'utilisateur, du schema de          | */
/* |    structure dont le nom interne est NomSchema.                    | */
/* |    Typ indique s'il s'agit d'un schema de document (1), de         | */
/* |    nature (2) ou d'extension (3).                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ConfigNomExterneSSchema (char *NomUtilisateur, char *NomSchema, int Typ)

#else  /* __STDC__ */
void                ConfigNomExterneSSchema (NomUtilisateur, NomSchema, Typ)
char               *NomUtilisateur;
char               *NomSchema;
int                 Typ;

#endif /* __STDC__ */

{
   int                 i;
   boolean             trouve;

   i = 0;
   trouve = False;
   NomUtilisateur[0] = '\0';
   switch (Typ)
	 {
	    case 1:
	       while (i < MAX_ITEM_CONF && !trouve && doc_items[i] != NULL)
		 {
		    if (strcmp (NomSchema, doc_items[i]) == 0)
		      {
			 if (doc_items_menu[i] != NULL)
			    strcpy (NomUtilisateur, doc_items_menu[i]);
			 trouve = True;
		      }
		    else
		       i++;
		 }
	       break;

	    case 2:
	       while (i < MAX_ITEM_CONF && !trouve && nat_items[i] != NULL)
		 {
		    if (strcmp (NomSchema, nat_items[i]) == 0)
		      {
			 if (nat_items_menu[i] != NULL)
			    strcpy (NomUtilisateur, nat_items_menu[i]);
			 trouve = True;
		      }
		    else
		       i++;
		 }
	       break;

	    case 3:
	       while (i < MAX_ITEM_CONF && !trouve && ext_items[i] != NULL)
		 {
		    if (strcmp (NomSchema, ext_items[i]) == 0)
		      {
			 if (ext_items_menu[i] != NULL)
			    strcpy (NomUtilisateur, ext_items_menu[i]);
			 trouve = True;
		      }
		    else
		       i++;
		 }
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |    ConfigNomInterneSSchema donne le nom du schema de structure qui | */
/* |    correspond a un nom traduit dans la langue de l'utilisateur.    | */
/* |    Si Doc est vrai, il s'agit d'un schema de document, sinon c'est | */
/* |    un schema de nature.                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ConfigNomInterneSSchema (char *NomUtilisateur, char *NomSchema, boolean Doc)

#else  /* __STDC__ */
void                ConfigNomInterneSSchema (NomUtilisateur, NomSchema, Doc)
char               *NomUtilisateur;
char               *NomSchema;
boolean             Doc;

#endif /* __STDC__ */

{
   int                 i;
   boolean             trouve;

   i = 0;
   trouve = False;
   NomSchema[0] = '\0';
   if (Doc)
      while (i < MAX_ITEM_CONF && !trouve && doc_items_menu[i] != NULL)
	{
	   if (strcmp (NomUtilisateur, doc_items_menu[i]) == 0)
	     {
		if (doc_items[i] != NULL)
		   strcpy (NomSchema, doc_items[i]);
		trouve = True;
	     }
	   else
	      i++;
	}
   else
      while (i < MAX_ITEM_CONF && !trouve && nat_items_menu[i] != NULL)
	{
	   if (strcmp (NomUtilisateur, nat_items_menu[i]) == 0)
	     {
		if (nat_items[i] != NULL)
		   strcpy (NomSchema, nat_items[i]);
		trouve = True;
	     }
	   else
	      i++;
	}
}


/* ---------------------------------------------------------------------- */
/* |    openConfigFile                                                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static FILE        *openConfigFile (char *name, boolean lang)

#else	/* __STDC__ */
static FILE        *openConfigFile (name, lang)
char               *name;
boolean             lang;

#endif /* __STDC__ */

{

   char                suffix[10];
   char               *ptr;
   int                 i;
   PathBuffer          DirBuffer, filename;
   FILE               *file;

   if (lang)
     {
	ptr = TtaGetVarLANG ();
	strcpy (suffix, ptr);
     }
   else
      strcpy (suffix, "conf");

   /* compose le nom du fichier a ouvrir avec le nom du directory */
   /* des schemas et le suffixe */
   strncpy (DirBuffer, DirectorySchemas, MAX_PATH);
   BuildFileName (name, suffix, DirBuffer, filename, &i);
   /* ouvre le fichier */
   file = fopen (filename, "r");
   return file;
}

/* ---------------------------------------------------------------------- */
/* |    ConfigMakeMenuPres cree dans BufMenu la liste des schemas de    | */
/* |    presentation qui peuvent s'appliquer au schema de structure de  | */
/* |    nom schema.                                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 ConfigMakeMenuPres (char *schema, char *BufMenu)

#else  /* __STDC__ */
int                 ConfigMakeMenuPres (schema, BufMenu)
char               *schema;
char               *BufMenu;

#endif /* __STDC__ */

{
   int                 nbitem, len, indmenu;
   FILE               *file;
   boolean             stop;
   char                line[MAX_TXT_LEN];
   char                texte[MAX_TXT_LEN];
   char                texteISO[MAX_TXT_LEN];
   char                mot[MAX_TXT_LEN];

   nbitem = 0;
   indmenu = 0;
   if (BufMenu != NULL)
      BufMenu[0] = '\0';
   file = openConfigFile (schema, True);
   if (file == NULL)
      return 0;
   stop = False;
   if (readUntil (file, "presentation", ""))
      do
	{
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	      stop = True;
	   else
	     {
		getFirstWord (line, mot);
		if (mot[0] != '\0')
		   /* la ligne n'est pas vide */
		  {
		     /* si la ligne contient un mot cle marquant le debut d'une autre */
		     /* section, on a fini */
		     if (singleWord (line))
			if (strcmp (mot, "export") == 0)
			   stop = True;
			else if (strcmp (mot, "import") == 0)
			   stop = True;
			else if (strcmp (mot, "translation") == 0)
			   stop = True;
		     if (!stop)
		       {
			  getStringAfterColon (line, texte);
			  if (texte[0] == '\0')
			     fprintf (stderr, "invalid line in file %s\n   %s\n", schema, line);
			  else
			    {
			       strcpy (texteISO, TransCani (texte));
			       if (pres_items[nbitem] != NULL)
				  TtaFreeMemory (pres_items[nbitem]);
			       pres_items[nbitem] = TtaGetMemory (strlen (mot) + 1);
			       strcpy (pres_items[nbitem], mot);
			       if (pres_items_menu[nbitem] != NULL)
				  TtaFreeMemory (pres_items_menu[nbitem]);
			       len = strlen (texteISO) + 1;
			       pres_items_menu[nbitem] = TtaGetMemory (len);
			       strcpy (pres_items_menu[nbitem], texteISO);
			       if (BufMenu != NULL)
				 {
				    strcpy (&BufMenu[indmenu], texteISO);
				    indmenu += len;
				 }
			       nbitem++;
			    }
		       }
		  }
	     }
	}
      while (!stop);
   fclose (file);
   return nbitem;
}

/* ---------------------------------------------------------------------- */
/* |    ConfigGetNomPSchema recupere dans la table des schemas de       | */
/* |            presentation le nom interne du schema qui se trouve     | */
/* |            a l'entree de rang choix.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ConfigGetNomPSchema (int choix, char *schpres)

#else  /* __STDC__ */
void                ConfigGetNomPSchema (choix, schpres)
int                 choix;
char               *schpres;

#endif /* __STDC__ */

{
   strcpy (schpres, pres_items[choix - 1]);
}

/* ---------------------------------------------------------------------- */
/* |    ConfigMakeImportMenu cree dans BufMenu la liste des schemas de  | */
/* |            structure qui ont des fichiers de langue dans les       | */
/* |            directories de schemas.                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 ConfigMakeImportMenu (char *BufMenu)

#else  /* __STDC__ */
int                 ConfigMakeImportMenu (BufMenu)
char               *BufMenu;

#endif /* __STDC__ */

{
   int                 nbitem, len, i, lgmenu;

   lgmenu = 0;
   nbitem = 0;
   BufMenu[0] = '\0';
   i = 0;

   /* parcourt la table des types de documents */
   while (i < MAX_ITEM_CONF && doc_items[i] != NULL)
     {
	if (doc_import[i])
	  {
	     if (doc_items_menu[i] != NULL)
		/* cette entree de la table a une traduction, on la prend */
	       {
		  len = strlen (doc_items_menu[i]);
		  strcpy (&BufMenu[lgmenu], doc_items_menu[i]);
	       }
	     else
		/* pas de traduction, on prend le nom d'origine du schema */
	       {
		  len = strlen (doc_items[i]);
		  strcpy (&BufMenu[lgmenu], doc_items[i]);
	       }
	     lgmenu += len + 1;
	     nbitem++;
	  }
	i++;
     }
   return nbitem;
}


/* ---------------------------------------------------------------------- */
/* |    ConfigMakeMenuExport cree dans BufMenu la liste des schemas de  | */
/* |    traduction qui peuvent s'appliquer au schema de structure de    | */
/* |    nom schema.                                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 ConfigMakeMenuExport (char *schema, char *BufMenu)

#else  /* __STDC__ */
int                 ConfigMakeMenuExport (schema, BufMenu)
char               *schema;
char               *BufMenu;

#endif /* __STDC__ */

{
   int                 indmenu;
   int                 nbitem, len;
   FILE               *file;
   boolean             stop;
   char                line[MAX_TXT_LEN];
   char                texte[MAX_TXT_LEN];
   char                texteISO[MAX_TXT_LEN];
   char                mot[MAX_TXT_LEN];

   nbitem = 0;
   indmenu = 0;
   if (BufMenu != NULL)
      BufMenu[0] = '\0';
   file = openConfigFile (schema, True);
   if (file == NULL)
      return 0;
   stop = False;
   if (readUntil (file, "export", ""))
      do
	{
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	      stop = True;
	   else
	     {
		getFirstWord (line, mot);
		if (mot[0] != '\0')
		   /* la ligne n'est pas vide */
		  {
		     /* si la ligne contient un mot cle marquant le debut d'une autre */
		     /* section, on a fini */
		     if (singleWord (line))
			if (strcmp (mot, "presentation") == 0)
			   stop = True;
			else if (strcmp (mot, "import") == 0)
			   stop = True;
			else if (strcmp (mot, "translation") == 0)
			   stop = True;
		     if (!stop)
		       {
			  getStringAfterColon (line, texte);
			  if (texte[0] == '\0')
			     fprintf (stderr, "invalid line in file %s\n   %s\n", schema, line);
			  else
			    {
			       strcpy (texteISO, TransCani (texte));
			       if (export_items[nbitem] != NULL)
				  TtaFreeMemory (export_items[nbitem]);
			       export_items[nbitem] = TtaGetMemory (strlen (mot) + 1);
			       strcpy (export_items[nbitem], mot);
			       if (export_items_menu[nbitem] != NULL)
				  TtaFreeMemory (export_items_menu[nbitem]);
			       len = strlen (texteISO) + 1;
			       export_items_menu[nbitem] = TtaGetMemory (len);
			       strcpy (export_items_menu[nbitem], texteISO);
			       if (BufMenu != NULL)
				 {
				    strcpy (&BufMenu[indmenu], texteISO);
				    indmenu += len;
				 }
			       nbitem++;
			    }
		       }
		  }
	     }
	}
      while (!stop);
   fclose (file);
   return nbitem;
}

/* ---------------------------------------------------------------------- */
/* |    ConfigGetNomExportSchema recupere dans la table des schemas de  | */
/* |            traduction le nom interne du schema qui se trouve       | */
/* |            a l'entree de rang choix.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ConfigGetNomExportSchema (int choix, char *schtrad)

#else  /* __STDC__ */
void                ConfigGetNomExportSchema (choix, schtrad)
int                 choix;
char               *schtrad;

#endif /* __STDC__ */

{
   strcpy (schtrad, export_items[choix - 1]);
}


/* ---------------------------------------------------------------------- */
/* |    Translate                                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      Translate (PtrSSchema pSS, char *mot, char *trans)

#else  /* __STDC__ */
static boolean      Translate (pSS, mot, trans)
PtrSSchema        pSS;
char               *mot;
char               *trans;

#endif /* __STDC__ */

{
   boolean             found;
   int                 i, j;
   TtAttribute           *pAttr;
   char               *terme;

   found = False;
   terme = TransCani (mot);
   /* cherche le mot a traduire d'abord parmi les noms d'elements */
   for (i = 0; i < pSS->SsNRules && !found; i++)
      if (strcmp (terme, pSS->SsRule[i].SrName) == 0)
	{
	   strncpy (pSS->SsRule[i].SrName, TransCani (trans), MAX_NAME_LENGTH - 1);
	   found = True;
	}
   /* cherche ensuite parmi les noms d'attributs et de valeurs d'attributs */
   if (!found)
      for (i = 0; i < pSS->SsNAttributes && !found; i++)
	{
	   pAttr = &pSS->SsAttribute[i];
	   if (strcmp (terme, pAttr->AttrName) == 0)
	     {
		strncpy (pAttr->AttrName, TransCani (trans), MAX_NAME_LENGTH - 1);
		found = True;
	     }
	   else if (pAttr->AttrType == AtEnumAttr)
	      for (j = 0; j < pAttr->AttrNEnumValues && !found; j++)
		 if (strcmp (terme, pAttr->AttrEnumValue[j]) == 0)
		   {
		      strncpy (pAttr->AttrEnumValue[j], TransCani (trans), MAX_NAME_LENGTH - 1);
		      found = True;
		   }
	}
   /* cherche enfin parmi les regles d'extension, si c'est un schema d'ecxtension */
   if (!found && pSS->SsExtension)
      if (pSS->SsNExtensRules > 0 && pSS->SsExtensBlock != NULL)
	 for (i = 0; i < pSS->SsNExtensRules && !found; i++)
	    if (strcmp (terme, pSS->SsExtensBlock->EbExtensRule[i].SrName) == 0)
	      {
		 strncpy (pSS->SsExtensBlock->EbExtensRule[i].SrName,
			  TransCani (trans), MAX_NAME_LENGTH - 1);
		 found = True;
	      }
   return found;
}


/* ---------------------------------------------------------------------- */
/* |    ConfigTranslateSSchema                                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ConfigTranslateSSchema (PtrSSchema pSS)

#else  /* __STDC__ */
void                ConfigTranslateSSchema (pSS)
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   FILE               *file;
   boolean             stop, error;
   char                line[MAX_TXT_LEN];
   char                texte[MAX_TXT_LEN];
   char                mot[MAX_TXT_LEN];

   if (pSS == NULL)
      return;
   /* ouvre le fichier de configuration langue associe' au schema */
   file = openConfigFile (pSS->SsName, True);
   if (file == NULL)
      /* pas de fichier langue associe' a ce schema de structure */
      return;
   stop = False;
   /* avance dans le fichier jusqu'a la ligne qui contient le seul */
   /* mot "translation" */
   if (readUntil (file, "translation", ""))
      /* lit le fichier ligne a ligne */
      do
	{
	   error = False;
	   /* lit une ligne du fichier */
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	      /* fin de fichier */
	      stop = True;
	   else
	     {
		/* prend le premier mot de la ligne */
		getFirstWord (line, mot);
		if (mot[0] != '\0')
		   /* la ligne n'est pas vide */
		  {
		     /* si la ligne contient un mot cle marquant le debut d'une autre */
		     /* section, on a fini */
		     if (singleWord (line))
			if (strcmp (mot, "presentation") == 0)
			   stop = True;
			else if (strcmp (mot, "export") == 0)
			   stop = True;
			else if (strcmp (mot, "import") == 0)
			   stop = True;
			else
			  {
			     fprintf (stderr, "invalid line in file %s\n   %s\n", pSS->SsName, line);
			     error = True;
			  }
		     if (!stop && !error)
		       {
			  /* cherche la chaine de caracteres qui suit ':' */
			  getStringAfterColon (line, texte);
			  if (texte[0] == '\0')
			     fprintf (stderr, "invalid line in file %s\n   %s\n", pSS->SsName, line);
			  else if (!Translate (pSS, mot, texte))
			     fprintf (stderr, "invalid line in file %s\n   %s\n", pSS->SsName, line);
		       }
		  }
	     }
	}
      while (!stop);
   fclose (file);
}

/* ---------------------------------------------------------------------- */
/* |    ConfigDefaultPSchema    retourne dans schpres le nom du 1er     | */
/* |    schema de presentation associe' au schema de structure schstr   | */
/* |    dans le fichier .conf correspondant.                            | */
/* |    Retourne False si pas trouve', True si OK.                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             ConfigDefaultPSchema (char *schstr, char *schpres)

#else  /* __STDC__ */
boolean             ConfigDefaultPSchema (schstr, schpres)
char               *schstr;
char               *schpres;

#endif /* __STDC__ */

{
   boolean             ok, stop;
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                mot[MAX_TXT_LEN];

   ok = False;
   /* ouvre le fichier .conf associe' au schema de structure */
   file = openConfigFile (schstr, False);
   if (file != NULL)
      /* on a ouvert le fichier .conf */
     {
	/* on cherche la premiere ligne qui commence par le mot "style" */
	stop = False;
	do
	   /* lit une ligne */
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	      /* fin de fichier */
	      stop = True;
	   else
	     {
		/* prend le premier mot de la ligne */
		getFirstWord (line, mot);
		if (strcmp (mot, "style") == 0)
		  {
		     /* le 1er mot est "style". Cherche le mot qui suit : c'est le */
		     /* nom du schema de presentation cherche' */
		     getSecondWord (line, mot);
		     if (mot[0] != '\0')
			/* il y a bien un 2eme mot : succes */
		       {
			  strcpy (schpres, mot);
			  ok = True;
		       }
		     stop = True;
		  }
	     }
	while (!stop);
	fclose (file);
     }
   return ok;
}


/* ---------------------------------------------------------------------- */
/* |    readUntilStyle  lit le fichier file (qui doit etre ouvert)      | */
/* |            jusqu'a trouver une ligne qui contienne le mot "style"  | */
/* |            suivi du nom nomPSchema.                                | */
/* |            Retourne True si trouve, False sinon.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      readUntilStyle (FILE * file, char *nomPSchema)
#else  /* __STDC__ */
static boolean      readUntilStyle (file, nomPSchema)
FILE               *file;
char               *nomPSchema;

#endif /* __STDC__ */
{
   boolean             stop;
   boolean             ok;
   char                line[MAX_TXT_LEN];
   char                mot[MAX_TXT_LEN];

   stop = False;
   ok = False;
   do
      if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	 /* fin de fichier */
	 stop = True;
      else
	{
	   getFirstWord (line, mot);
	   if (strcmp (mot, "style") == 0)
	     {
		getSecondWord (line, mot);
		if (strcmp (mot, nomPSchema) == 0)
		   ok = True;
	     }
	}
   while (!stop && !ok);
   return ok;
}

/* ---------------------------------------------------------------------- */
/* |    openConfFileAndReadUntil ouvre le fichier .conf qui concerne    | */
/* |            le schema de structure pSS et avance dans ce fichier    | */
/* |            jusqu'a la ligne qui marque la section de nom sectName. | */
/* |            Retourne le file descriptor du fichier si on a trouve'  | */
/* |            le fichier .conf et la section, NULL sinon.             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static FILE        *openConfFileAndReadUntil (PtrSSchema pSS, char *sectName)

#else  /* __STDC__ */
static FILE        *openConfFileAndReadUntil (pSS, sectName)
PtrSSchema        pSS;
char               *sectName;

#endif /* __STDC__ */
{
   FILE               *file;

   /* ouvre le fichier .conf */
   file = NULL;
   if (pSS != NULL)
      file = openConfigFile (pSS->SsName, False);
   if (file != NULL)
      /* on a ouvert le fichier */
      /* cherche la ligne "style xxxx" qui correspond au schema P concerne' */
      if (!readUntilStyle (file, pSS->SsDefaultPSchema))
	 /* pas trouve' */
	{
	   fclose (file);
	   file = NULL;
	}
      else
	 /* cherche le debut de la section voulue */
      if (!readUntil (file, sectName, ""))
	 /* pas trouve' */
	{
	   fclose (file);
	   file = NULL;
	}
   return file;
}


/* ---------------------------------------------------------------------- */
/* |    getNextLineInSection    lit dans line la prochaine ligne du     | */
/* |            fichier file qui fasse partie de la meme section.       | */
/* |            Retourne True si succes, False si on a atteint la fin   | */
/* |            de la section courante ou du fichier.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      getNextLineInSection (FILE * file, char *line)

#else  /* __STDC__ */
static boolean      getNextLineInSection (file, line)
FILE               *file;
char               *line;

#endif /* __STDC__ */

{
   boolean             ok, stop;
   char                mot1[MAX_TXT_LEN];
   char                mot2[MAX_TXT_LEN];

   ok = False;
   stop = False;
   do
      /* lit une ligne */
      if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	 /* fin de fichier */
	 stop = True;
      else
	{
	   /* prend le permier mot de la ligne lue */
	   getFirstWord (line, mot1);
	   if (mot1[0] != '\0')
	      /* la ligne n'est pas vide */
	     {
		/* si la ligne contient un mot cle marquant le debut d'une autre */
		/* section, on a fini */
		if (singleWord (line))
		   /* la ligne contient un seul mot */
		  {
		     if (strcmp (mot1, "open") == 0)
			stop = True;
		     else if (strcmp (mot1, "geometry") == 0)
			stop = True;
		     else if (strcmp (mot1, "presentation") == 0)
			stop = True;
		     else if (strcmp (mot1, "options") == 0)
			stop = True;
		     else
			/* ligne contenant un seul mot. on considere que c'est OK... */
			ok = True;
		  }
		else
		   /* la ligne contient plus d'un mot */
		if (strcmp (mot1, "style") == 0)
		  {
		     getSecondWord (line, mot2);
		     if (mot2[0] != ':')
			/* la ligne est du type "style xxxx". C'est une fin de section */
			stop = True;
		     else
			ok = True;
		  }
		else
		   /* la ligne ne commence pas par "style". OK */
		   ok = True;
	     }
	}
   while (!stop && !ok);
   return ok;
}

/* ---------------------------------------------------------------------- */
/* |    getXYWidthHeight        lit les 4 entiers x, y, width, height   | */
/* |            suivent les deux-points dans une ligne de la section    | */
/* |            open ou geometry d'un fichier .conf                     | */
/* |            Retourne True si succes.                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      getXYWidthHeight (char *line, PtrDocument pDoc, int *x, int *y,
				      int *width, int *height)
#else  /* __STDC__ */
static boolean      getXYWidthHeight (line, pDoc, x, y, width, height)
char               *line;
PtrDocument         pDoc;
int                *x;
int                *y;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   char                suiteligne[MAX_TXT_LEN];
   int                 nbentiers;
   boolean             result;

   result = False;
   /* extrait la partie de la ligne qui suit les deux-points */
   getStringAfterColon (line, suiteligne);
   if (suiteligne[0] == '\0')
      fprintf (stderr, "invalid line in file %s.conf\n   %s\n",
	       pDoc->DocSSchema->SsName, line);
   else
     {
	/* extrait les 4 entiers */
	nbentiers = sscanf (suiteligne, "%d %d %d %d", x, y, width, height);
	if (nbentiers != 4)
	   fprintf (stderr, "invalid line in file %s.conf\n   %s\n",
		    pDoc->DocSSchema->SsName, line);
	else
	  {
	     /* convertit si necessaire en fonction de la resolution de l'ecran */
	     if (PTS_POUCE != 83)
	       {
		  *x = (int) ((float) (*x * 83) / (float) PTS_POUCE);
		  *width = (int) ((float) (*width * 83) / (float) PTS_POUCE);
		  *y = (int) ((float) (*y * 83) / (float) PTS_POUCE);
		  *height = (int) ((float) (*height * 83) / (float) PTS_POUCE);
	       }
	     result = True;
	  }
     }
   return result;
}

/* ---------------------------------------------------------------------- */
/* |    ConfigOpenFirstViews ouvre, pour le document pDoc, les vues     | */
/* |            specifiees dans la section open du fichier de           | */
/* |            configuration .conf                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ConfigOpenFirstViews (PtrDocument pDoc)
#else  /* __STDC__ */
void                ConfigOpenFirstViews (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   FILE               *file;
   int                 x, y, width, height;
   char                line[MAX_TXT_LEN];
   char                nomvue[MAX_TXT_LEN];

   /* ouvre le fichier .conf du document et avance jusqu'a la section "open" */
   file = openConfFileAndReadUntil (pDoc->DocSSchema, "open");
   if (file != NULL)
     {
	/* on a trouve' le debut de la section open. On lit le fichier .conf */
	/* ligne par ligne, jusqu'a la fin de cette section */
	while (getNextLineInSection (file, line))
	  {
	     /* le 1er mot de la ligne est le nom d'une vue a ouvrir */
	     getFirstWord (line, nomvue);
	     /* lit les coordonnees (x, y) et dimensions (width, height) de la */
	     /* frame ou doit s'afficher la vue */
	     if (getXYWidthHeight (line, pDoc, &x, &y, &width, &height))
		/* lecture reussie, on ouvre la vue */
		CreVueNommee (pDoc, nomvue, x, y, width, height);
	  }
	fclose (file);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ConfigGetViewGeometry retourne la position (x, y) et les        | */
/* |            dimensions (width, height) de la fenetre ou doit        | */
/* |            s'afficher la vue de non vue pour le document pDoc.     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ConfigGetViewGeometry (PtrDocument pDoc, char *vue, int *x, int *y, int *width, int *height)
#else  /* __STDC__ */
void                ConfigGetViewGeometry (pDoc, vue, x, y, width, height)
PtrDocument         pDoc;
char               *vue;
int                *x;
int                *y;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                nomvue[MAX_TXT_LEN];
   boolean             trouve;

   *x = 0;
   *y = 0;
   *width = 0;
   *height = 0;
   /* ouvre le fichier .conf du document et avance jusqu'a la section "open" */
   file = openConfFileAndReadUntil (pDoc->DocSSchema, "open");
   if (file != NULL)
     {
	/* on a trouve' le debut de la section open. On lit le fichier .conf */
	/* ligne par ligne, jusqu'a la ligne qui commence par le nom de la vue */
	trouve = False;
	while (!trouve && getNextLineInSection (file, line))
	  {
	     /* le 1er mot de la ligne est le nom d'une vue */
	     getFirstWord (line, nomvue);
	     /* est-ce le nom de la vue cherchee ? */
	     trouve = (strcmp (nomvue, vue) == 0);
	  }
	if (!trouve)
	   /* on n'a pas trouve' dans la section "open". On cherche dans la */
	   /* section "geometry" */
	  {
	     fclose (file);
	     file = openConfFileAndReadUntil (pDoc->DocSSchema, "geometry");
	     if (file != NULL)
		while (!trouve && getNextLineInSection (file, line))
		  {
		     /* le 1er mot de la ligne est le nom d'une vue */
		     getFirstWord (line, nomvue);
		     /* est-ce le nom de la vue cherchee ? */
		     trouve = (strcmp (nomvue, vue) == 0);
		  }
	  }
	if (trouve)
	   getXYWidthHeight (line, pDoc, x, y, width, height);

	if (file != NULL)
	   fclose (file);
     }
}
/* ---------------------------------------------------------------------- */
/* |    TtaGetViewGeometry returns the position (x, y) and sizes        | */
/* |            (width, height) of the frame wher view is displayed.    | */
/* |            Parameters:    document: the document.                  | */
/* |                           name: the name of the view in P schema.  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaGetViewGeometry (Document document, char *name, int *x, int *y, int *width, int *height)
#else  /* __STDC__ */
void                TtaGetViewGeometry (document, name, x, y, width, height)
Document            document;
char               *name;
int                *x;
int                *y;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   UserErrorCode = 0;
   *x = 0;
   *y = 0;
   *width = 0;
   *height = 0;
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (document != 0)
      ConfigGetViewGeometry (TabDocuments[document - 1], name, x, y, width, height);
}

/* ---------------------------------------------------------------------- */
/* |    ConfigGetPSchemaNature retourne dans presNature le nom du       | */
/* |            schema de presentation a appliquer a la nature de nom   | */
/* |            nomNature dans le contexte du schema de structure pSS   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             ConfigGetPSchemaNature (PtrSSchema pSS, char *nomNature, char *presNature)
#else  /* __STDC__ */
boolean             ConfigGetPSchemaNature (pSS, nomNature, presNature)
PtrSSchema        pSS;
char               *nomNature;
char               *presNature;

#endif /* __STDC__ */
{
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                suiteligne[MAX_TXT_LEN];
   char                nom[MAX_TXT_LEN];
   boolean             trouve;
   boolean             ok;

   presNature[0] = '\0';
   ok = False;
   /* ouvre le fichier .conf du document et avance jusqu'a la section "presentation" */
   file = openConfFileAndReadUntil (pSS, "presentation");
   if (file != NULL)
     {
	/* on a trouve' le debut de la section presentation. On lit le fichier */
	/* ligne par ligne jusqu'a la ligne qui commence par le nom de la nature */
	trouve = False;
	while (!trouve && getNextLineInSection (file, line))
	  {
	     /* le 1er mot de la ligne est le nom d'une nature */
	     getFirstWord (line, nom);
	     /* est-ce le nom de la nature cherchee ? */
	     trouve = (strcmp (nom, nomNature) == 0);
	  }
	if (trouve)
	   /* on a trouve' la ligne de la section presentation qui commence par */
	   /* le nom de la nature voulue */
	  {
	     /* le nom de nature est suivi, apres ":", du nom du schema de */
	     /* presentation a appliquer */
	     getStringAfterColon (line, suiteligne);
	     if (suiteligne[0] == '\0')
		fprintf (stderr, "invalid line in file %s.conf\n   %s\n", pSS->SsName, line);
	     else
	       {
		  strncpy (presNature, suiteligne, MAX_NAME_LENGTH - 1);
		  ok = True;
	       }
	  }
	fclose (file);
     }
   return ok;
}


/* ---------------------------------------------------------------------- */
/* |    ConfigGetPresentationOption cherche, dans le fichier .conf      | */
/* |            correspondant au schema de structure pSS, la valeur     | */
/* |            de l'option de presentation de nom optionName.          | */
/* |            Retourne la valeur trouvee dans optionValue, ou une     | */
/* |            chaine vide si l'option n'est pas trouvee.              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ConfigGetPresentationOption (PtrSSchema pSS, char *optionName, char *optionValue)

#else  /* __STDC__ */
void                ConfigGetPresentationOption (pSS, optionName, optionValue)
PtrSSchema        pSS;
char               *optionName;
char               *optionValue;

#endif /* __STDC__ */

{
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                suiteligne[MAX_TXT_LEN];
   char                nom[MAX_TXT_LEN];
   boolean             trouve;

   optionValue[0] = '\0';
   /* ouvre le fichier .conf du document et avance jusqu'a la section "options" */
   file = openConfFileAndReadUntil (pSS, "options");
   if (file != NULL)
     {
	/* on a trouve' le debut de la section options. On lit le fichier */
	/* ligne par ligne jusqu'a la ligne qui commence par le nom de l'option */
	/* chercheee */
	trouve = False;
	while (!trouve && getNextLineInSection (file, line))
	  {
	     /* le 1er mot de la ligne est le nom d'une option */
	     getFirstWord (line, nom);
	     /* est-ce le nom de l'option cherchee ? */
	     trouve = (strcmp (nom, optionName) == 0);
	  }
	if (trouve)
	   /* on a trouve' la ligne de la section options qui commence par */
	   /* le nom de l'option voulue */
	  {
	     /* le nom de l'option est suivi, apres ":", de la valeur de l'option */
	     getStringAfterColon (line, suiteligne);
	     if (suiteligne[0] == '\0')
		fprintf (stderr, "invalid line in file %s.conf\n   %s\n", pSS->SsName, line);
	     else
		strncpy (optionValue, suiteligne, MAX_NAME_LENGTH - 1);
	  }
	fclose (file);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ConfigGetPSchemaForPageSize cherche, dans le fichier .conf      | */
/* |            correspondant au schema de structure pSS, le schema     | */
/* |            de presentation qui comporte une option "pagesize"      | */
/* |            ayant la valeur pageSize.                               | */
/* |            Retourne le nom du schema trouve' dans schemaName, ou   | */
/* |            une chaine vide si pas trouve'.                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ConfigGetPSchemaForPageSize (PtrSSchema pSS, char *pageSize, char *schemaName)

#else  /* __STDC__ */
void                ConfigGetPSchemaForPageSize (pSS, pageSize)
PtrSSchema        pSS;
char               *pageSize;
char               *schemaName;

#endif /* __STDC__ */

{
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                mot[MAX_TXT_LEN];
   char                suiteligne[MAX_TXT_LEN];
   char                lastStyle[MAX_TXT_LEN];
   boolean             stop;

   schemaName[0] = '\0';
   /* ouvre le fichier .conf correspondant au type du document */
   file = NULL;
   if (pSS != NULL)
      file = openConfigFile (pSS->SsName, False);
   if (file != NULL)
      /* on a ouvert le fichier, on va le lire ligne par ligne */
     {
	lastStyle[0] = '\0';
	stop = False;
	do
	   /* on lit une ligne */
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	      /* fin de fichier */
	      stop = True;
	   else
	     {
		/* prend le 1er mot de la ligne lue */
		getFirstWord (line, mot);
		if (strcmp (mot, "style") == 0)
		   /* c'est une ligne "style". On conserve le nom du schema de */
		   /* presentation qui suit le mot-cle "style" */
		   getSecondWord (line, lastStyle);
		else if (strcmp (mot, "pagesize") == 0)
		   /* c'est une ligne "pagesize", on la traite */
		  {
		     getStringAfterColon (line, suiteligne);
		     if (suiteligne[0] == '\0')
			fprintf (stderr, "invalid line in file %s.conf\n   %s\n",
				 pSS->SsName, line);
		     else if (strcmp (suiteligne, pageSize) == 0)
			/* c'est le format de page cherche'. On a fini */
		       {
			  strcpy (schemaName, lastStyle);
			  stop = True;
		       }
		  }
	     }
	while (!stop);
	fclose (file);
     }
}


/* ---------------------------------------------------------------------- */
/* |    MakeMenuPattern cree dans BufMenu la liste des motifs de        | */
/* |    remplissage disponibles et retourne le nombre de motifs ou      | */
/* |    0 si echec.                                                     | */
/* |    LgMax indique la taille du buffer BufMenu.                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 MakeMenuPattern (char *BufMenu, int LgMax)

#else  /* __STDC__ */
int                 MakeMenuPattern (BufMenu, LgMax)
char               *BufMenu;
int                 LgMax;

#endif /* __STDC__ */

{
   int                 nbentree;
   char                name[80];
   int                 lgname;
   int                 lgmenu;
   int                 max;
   char               *ptr;

   nbentree = 0;
   if (BufMenu != NULL)
     {
	ptr = &BufMenu[0];
	lgmenu = 0;
	max = NumberOfPatterns ();

	do
	  {
	     /* Recupere le nom du pattern */
	     if (nbentree < max)
	       {
		  strcpy (name, PatternName (nbentree));
		  lgname = strlen (name) + 1;
	       }
	     else
		lgname = 1;

	     if (lgmenu + lgname > LgMax)
		lgmenu = LgMax;
	     else if (lgname > 1)
	       {
		  nbentree++;
		  strcpy (ptr, name);
		  ptr += lgname;
		  lgmenu += lgname;
	       }
	  }
	while (lgmenu < LgMax && lgname > 1);
     }
   return nbentree;
}


/* ---------------------------------------------------------------------- */
/* |    MakeMenuColor cree dans le buffer BufMenu la liste des couleurs | */
/* |    disponibles et retourne le nombre d'entrees creees ou           | */
/* |    0 si echec.                                                     | */
/* |    LgMax indique la taille du buffer BufMenu.                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 MakeMenuColor (char *BufMenu, int LgMax)

#else  /* __STDC__ */
int                 MakeMenuColor (BufMenu, LgMax)
char               *BufMenu;
int                 LgMax;

#endif /* __STDC__ */

{
   int                 nbentree;
   int                 lgmenu;
   int                 lgname;
   int                 max;
   char                name[80];
   char               *ptr;

   nbentree = 0;
   if (BufMenu != NULL)
     {
	ptr = &BufMenu[0];
	lgmenu = 0;
	max = NumberOfColors ();

	do
	  {
	     /* Recupere le nom de la couleur */
	     if (nbentree < max)
	       {
		  strcpy (name, ColorName (nbentree));
		  lgname = strlen (name) + 1;
	       }
	     else
		lgname = 1;

	     if (lgmenu + lgname > LgMax)
		lgmenu = LgMax;
	     else if (lgname > 1)
	       {
		  nbentree++;
		  strcpy (ptr, name);
		  ptr += lgname;
		  lgmenu += lgname;
	       }
	  }
	while (lgmenu < LgMax && lgname > 1);

     }
   return nbentree;
}

/* ---------------------------------------------------------------------- */
/* |    ConfigDefaultTypoSchema retourne dans schtypo le nom du         | */
/* |    schema de typographie a appliquer a la nature de nom            | */
/* |    nomNature dans le contexte du schema de structure pSS           | */
/* |    Retourne False si pas trouve', True si OK.                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             ConfigDefaultTypoSchema (PtrSSchema pSS,
					     char *nomNature,
					     char *schtypo)
#else  /* __STDC__ */
boolean             ConfigDefaultTypoSchema (pSS, nomNature, schtypo)
PtrSSchema        pSS;
char               *nomNature;
char               *schtypo;

#endif /* __STDC__ */

{
   boolean             ok, trouve;
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                suiteligne[MAX_TXT_LEN];
   char                nom[MAX_TXT_LEN];

   /* ouvre le fichier .conf du document */
   /* et avance jusqu'a la section "typography" */
   file = openConfFileAndReadUntil (pSS, "typography");
   ok = False;
   if (file != NULL)
     {
	/* on a trouve' le debut de la section typography. On lit le fichier */
	/* .conf ligne par ligne jusqu'a la ligne qui commence */
	/* par le nom de la nature */
	trouve = False;
	while (!trouve && getNextLineInSection (file, line))
	  {
	     /* le 1er mot de la ligne est le nom d'une nature */
	     getFirstWord (line, nom);
	     /* est-ce le nom de la nature cherchee ? */
	     trouve = (strcmp (nom, nomNature) == 0);
	  }
	if (trouve)
	   /* on a trouve' la ligne de la section typography qui commence par */
	   /* le nom de la nature voulue */
	  {
	     /* le nom de nature est suivi, apres ":", du nom du schema de */
	     /* typographie a appliquer */
	     getStringAfterColon (line, suiteligne);
	     if (suiteligne[0] == '\0')
		fprintf (stderr, "invalid line in file %s.conf\n   %s\n", pSS->SsName, line);
	     else
	       {
		  strncpy (schtypo, suiteligne, MAX_NAME_LENGTH - 1);
		  ok = True;
	       }
	  }
	fclose (file);
     }
   return ok;

}				/*ConfigDefaultTypoSchema */
