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
 * gestion des fichiers de configuration et de langue.
 *
 * Author: V. Quint (INRIA)
 *
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
#include "fileaccess.h"
#include "document.h"
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "units_tv.h"
#include "appdialogue_tv.h"

#define MAX_ITEM_CONF 100

#include "applicationapi_f.h"
#include "fileaccess_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "message_f.h"
#include "thotmsg_f.h"
#include "views_f.h"

static char*        doc_items[MAX_ITEM_CONF];
static ThotBool     doc_import[MAX_ITEM_CONF];
static char*        doc_items_menu[MAX_ITEM_CONF];
static char*        nat_items[MAX_ITEM_CONF];
static char*        nat_items_menu[MAX_ITEM_CONF];
static char*        ext_items[MAX_ITEM_CONF];
static char*        ext_items_menu[MAX_ITEM_CONF];
static char*        pres_items[MAX_ITEM_CONF];
static char*        pres_items_menu[MAX_ITEM_CONF];
static char*        export_items[MAX_ITEM_CONF];
static char*        export_items_menu[MAX_ITEM_CONF];

/*----------------------------------------------------------------------
   ConfigInit initializes the configuration module
  ----------------------------------------------------------------------*/
void                ConfigInit ()
{
   int                 i;

   for (i = 0; i < MAX_ITEM_CONF; i++)
     {
	doc_items[i] = NULL;
	doc_import[i] = FALSE;
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
   TtaConfigReadConfigFiles (SchemaPath);
}

/*----------------------------------------------------------------------
   getFirstWord                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         getFirstWord (unsigned char* line, unsigned char* word)
#else  /* __STDC__ */
static void         getFirstWord (line, word)
unsigned char*      line;
unsigned char*      word;

#endif /* __STDC__ */
{
   int                 indword, indline;

   indline = 0;
   word[0] = EOS;
   /* saute les espaces de debut de ligne */
   while (line[indline] <= SPACE && line[indline] != EOS)
      indline++;
   if (line[indline] == '#')
      /* cette ligne ne comporte qu'un commentaire */
      return;
   /* copie tous les caracteres jusqu'a rencontrer le 1er espace ou ":" */
   /* ou la fin de ligne */
   indword = 0;
   while (line[indline] > SPACE && line[indline] != ':' &&
	  line[indline] != EOS)
      word[indword++] = line[indline++];
   /* marque la fin du mot trouve' */
   word[indword] = EOS;
}


/*----------------------------------------------------------------------
   getSecondWord                                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         getSecondWord (unsigned char* line, unsigned char* word)
#else  /* __STDC__ */
static void         getSecondWord (line, word)
unsigned char*      line;
unsigned char*      word;

#endif /* __STDC__ */
{
   int                 indword, indline;

   indline = 0;
   word[0] = EOS;
   /* saute les espaces de debut de ligne */
   while (line[indline] <= SPACE && line[indline] != EOS)
      indline++;
   if (line[indline] == TEXT('#'))
      /* cette ligne ne comporte qu'un commentaire */
      return;
   /* saute le 1er mot, jusqu'a rencontrer le 1er espace */
   /* ou la fin de ligne */
   while (line[indline] > SPACE && line[indline] != EOS)
      indline++;
   /* saute les espaces qui suivent le 1er mot */
   while (line[indline] <= SPACE && line[indline] != EOS)
      indline++;
   if (line[indline] == TEXT('#'))
      /* le premier mot est suivi d'un commentaire */
      return;
   /* copie tous les caracteres du 2eme mot jusqu'a rencontrer le 1er */
   /* espace ou la fin de ligne */
   indword = 0;
   while (line[indline] > SPACE && line[indline] != EOS)
      word[indword++] = line[indline++];
   /* marque la fin du mot trouve' */
   word[indword] = EOS;
}

/*----------------------------------------------------------------------
   singleWord                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     singleWord (unsigned char* line)
#else  /* __STDC__ */
static ThotBool     singleWord (line)
unsigned char*      line;

#endif /* __STDC__ */
{
   int                 ind;

   ind = 0;
   /* saute les espaces de debut de ligne */
   while (line[ind] <= SPACE && line[ind] != EOS)
      ind++;
   if (line[ind] == TEXT('#'))
      /* la ligne commence par un commentaire */
      return FALSE;
   /* saute le premier mot */
   while (line[ind] > SPACE && line[ind] != TEXT('#') && line[ind] != TEXT(':') && line[ind] != EOS)
      ind++;
   /* saute les espaces qui suivent le 1er mot */
   while (line[ind] <= SPACE && line[ind] != EOS)
      ind++;
   if (line[ind] == TEXT('#') || line[ind] == EOS)
      /* il ne reste rien dans la ligne ou seulement un commentaire */
      return TRUE;
   else
      /* il y a des caracteres significatifs apres le 1er mot */
      return FALSE;
}


/*----------------------------------------------------------------------
   getStringAfterColon                                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         getStringAfterColon (unsigned char* line, unsigned char* text)
#else  /* __STDC__ */
static void         getStringAfterColon (line, text)
unsigned char*      line;
unsigned char*      text;

#endif /* __STDC__ */
{
   int                 indline, indtext;

   indline = 0;
   text[0] = EOS;
   while (line[indline] != TEXT(':') && line[indline] != EOS)
      indline++;
   if (line[indline] == TEXT(':'))
     {
	indline++;
	while (line[indline] <= SPACE && line[indline] != EOS)
	   indline++;
	if (line[indline] == TEXT('#') || line[indline] == EOS)
	   return;
	indtext = 0;
	while (line[indline] != TEXT('#') && line[indline] != EOS)
	   text[indtext++] = line[indline++];
	/* elimine les blancs de fin de ligne */
	indtext--;
	while (text[indtext] <= SPACE && indtext >= 0)
	   indtext--;
	indtext++;
	/* termine la chaine */
	text[indtext] = EOS;
     }
}


/*----------------------------------------------------------------------
   readUntil       lit le fichier file (qui doit etre ouvert)      
   jusqu'a trouver une ligne qui contienne un seul mot,    
   soit word1 soit word2.                                  
   Retourne                                                
   0 si on ne trouve pas cette ligne,                   
   1 si on trouve le 1er mot,                           
   2 si on trouve le 2eme mot.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     readUntil (FILE * file, char* word1, char* word2)
#else  /* __STDC__ */
static ThotBool     readUntil (file, word1, word2)
FILE               *file;
char*               word1;
char*               word2;

#endif /* __STDC__ */
{
   ThotBool            stop;
   int                 ret;
   char                line[MAX_TXT_LEN];
   char                word[MAX_TXT_LEN];

   stop = FALSE;
   ret = 0;
   do
      if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	 /* fin de fichier */
	 stop = TRUE;
      else
	{
	   getFirstWord (line, word);
	   if (singleWord (line))
	     {
		if (*word1 != EOS)
		   if (strcmp (word, word1) == 0)
		      ret = 1;
		if (*word2 != EOS)
		   if (strcmp (word, word2) == 0)
		      ret = 2;
	     }
	}
   while (!stop && ret == 0);
   return ret;
}

/*----------------------------------------------------------------------
   namesOfDocType                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         namesOfDocType (char* fname, char** doctypeOrig, char** doctypeTrans, int *typ, ThotBool * import)
#else  /* __STDC__ */
static void         namesOfDocType (fname, doctypeOrig, doctypeTrans, typ, import)
PathBuffer          fname;
char**              doctypeOrig;
char**              doctypeTrans;
int*                typ;
ThotBool*           import;

#endif /* __STDC__ */
{
   int                 i, l, point, res;
   FILE*               file;
   char                line[MAX_TXT_LEN];
   char                text[MAX_TXT_LEN];
   char                word[MAX_TXT_LEN];
   ThotBool            stop;
   char                URL_DIR_SEP;

   *doctypeOrig = NULL;
   *doctypeTrans = NULL;
   *typ = CONFIG_UNKNOWN_TYPE;
   *import = FALSE;

   if (fname && strchr (fname, '/'))
	  URL_DIR_SEP = '/';
   else 
	   URL_DIR_SEP = DIR_SEP;

   /* ouvre le fichier */
   file = TtaReadOpen (fname);
   if (file == NULL)
     {
	fprintf (stderr, "cannot open file %s\n", fname);
	return;
     }
   /* cherche le premier mot du fichier, hors commentaires et espaces */
   stop = FALSE;
   do
      if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	 stop = TRUE;
      else
	{
	   getFirstWord (line, word);
	   if (word[0] != EOS)
	      stop = TRUE;
	}
   while (!stop);
   if (!singleWord (line))
      /* le premier mot n'est pas seul dans la ligne, erreur */
      return;

   if (strcmp (word, "document") == 0)
      *typ = CONFIG_DOCUMENT_STRUCT;
   else if (strcmp (word, "nature") == 0)
      *typ = CONFIG_NATURE_STRUCT;
   else if (strcmp (word, "extension") == 0)
      *typ = CONFIG_EXTENSION_STRUCT;
   else if (strcmp (word, "document-nature") == 0)
      *typ = CONFIG_EXCLUSION;
   else
      /* le premier mot du fichier est invalide */
     {
	fprintf (stderr, "file %s: invalid first word %s\n", fname, word);
	return;
     }

   /* cherche le "." marquant le suffixe a la fin du nom de fichier */
   i = strlen (fname);
   while (i > 0 && fname[i] != TEXT('.'))
      i--;
   if (fname[i] == TEXT('.'))
      point = i;
   else
      point = 0;
   /* cherche le dernier DIR_SEP du nom de fichier */
   while (i > 0 && fname[i] != URL_DIR_SEP)
      i--;
   if (fname[i] == URL_DIR_SEP)
      i++;
   if (fname[i] == TEXT('_'))
      /* ignore les fichiers dont le nom commence par "-" */
      return;
   l = strlen (&fname[i]) + 1;
   *doctypeOrig =	(char*) TtaGetMemory (l);
   if (point != 0)
      fname[point] = EOS;
   strcpy (*doctypeOrig, &fname[i]);
   /* retablit le '.' du suffixe dans le nom de fichier */
   if (point != 0)
      fname[point] = TEXT('.');

   if (*typ == CONFIG_DOCUMENT_STRUCT || *typ == CONFIG_EXCLUSION)
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
	*import = TRUE;
	/* cherche la ligne comportant le seul mot "translation" */
	res = readUntil (file, "", "translation");
     }
   if (res == 2)
      /* on a trouve' le mot translation */
     {
	/* on cherche la ligne qui donne la traduction du nom de schema */
	stop = FALSE;
	do
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	     {
		stop = TRUE;
		word[0] = EOS;
	     }
	   else
	     {
		getFirstWord (line, word);
		if (strcmp (word, *doctypeOrig) == 0)
		   stop = TRUE;
	     }
	while (!stop);

	if (strcmp (word, *doctypeOrig) == 0)
	   /* on a trouve' la ligne voulue */
	  {
	     getStringAfterColon (line, text);
	     if (text[0] == EOS)
		fprintf (stderr, "invalid line in file %s\n   %s\n", fname, line);
	     else
	       {
		  *doctypeTrans = TtaGetMemory (strlen (text) + 1);
		  strcpy (*doctypeTrans, AsciiTranslate (text));
	       }
	  }
     }
   TtaReadClose (file);

   /* Si le fichier de configuration ne definit pas de traduction pour */
   /* le nom du schema, on prend le nom d'origine comme traduction */
   if (*doctypeTrans == NULL)
     {
	*doctypeTrans = TtaGetMemory (l);
	strcpy (*doctypeTrans, *doctypeOrig);
     }
}


/*----------------------------------------------------------------------
   TtaConfigReadConfigFiles (re)initialise les tables des schemas de
   structure (documents, natures et extensions) qui ont    
   des fichiers de langue dans les directories de schemas. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaConfigReadConfigFiles (char* aSchemaPath)
#else                        /* __STDC__ */
void                TtaConfigReadConfigFiles (aSchemaPath)
char*               aSchemaPath;
#endif                       /* __STDC__ */
{
   int                 nbitemdoc, nbitemnat, nbitemext;
   int                 beginning, i;
   int                 typ;
   ThotBool            import;
   char*               Dir;
   PathBuffer          DirBuffer;
   ThotDirBrowse       thotDir;

#define NAME_LENGTH     100
#define MAX_NAME         80
#define SELECTOR_NB_ITEMS 5
   PathBuffer          fname;
   char*               suffix;
   char*               nameOrig;
   char*               nameTrans;
   ThotBool            stop;

   suffix = TtaGetVarLANG ();

#  ifdef _WINDOWS
   if (!_strnicmp (suffix, "fr", 2))
      app_lang = FR_LANG;
   else if (!_strnicmp (suffix, "en", 2))
      app_lang = EN_LANG;
   else if (!_strnicmp (suffix, "de", 2))
      app_lang = DE_LANG;
#  endif /* _WINDOWS */

   /* libere les anciennes entrees des tables de types de documents */
   /* de natures et d'extensions */
   for (i = 0; i < MAX_ITEM_CONF; i++)
     {
	if (doc_items[i] != NULL)
	  {
	     TtaFreeMemory (doc_items[i]);
	     doc_items[i] = NULL;
	  }
	doc_import[i] = FALSE;
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
   beginning = 0;
   i = 0;
   nbitemdoc = 0;
   nbitemnat = 0;
   nbitemext = 0;
   /* traite successivement tous les directories du path des schemas */
   strncpy (DirBuffer, aSchemaPath, MAX_PATH);
   stop = FALSE;
   while (DirBuffer[i] != EOS && i < MAX_PATH && !stop)
     {
	while (DirBuffer[i] != PATH_SEP && DirBuffer[i] != EOS && i < MAX_PATH)
	   i++;
	if (DirBuffer[i] == EOS)
	   /* dernier directory du path. Il faut s'arreter apres ce directory */
	   stop = TRUE;
	if (DirBuffer[i] == PATH_SEP)
	   DirBuffer[i] = EOS;
	if (DirBuffer[i] == EOS)
	   /* un directory de schema a ete isole' */
	  {
	     Dir = &DirBuffer[beginning];
	     if (TtaCheckDirectory (Dir))
		/* c'est bien un directory */
	       {
		  /* commande "ls" sur le directory */
		  thotDir.buf = fname;
		  thotDir.bufLen = sizeof (fname);
		  thotDir.PicMask = ThotDirBrowse_FILES;
		  if (ThotDirBrowse_first (&thotDir, Dir, "*.", suffix) == 1)
		     do
		       {
			  namesOfDocType (fname, &nameOrig, &nameTrans, &typ, &import);
			  if (nameOrig != NULL)
			    {
			       if (typ == CONFIG_DOCUMENT_STRUCT || typ == CONFIG_EXCLUSION)
				 {
				    doc_items[nbitemdoc] = nameOrig;
				    doc_items_menu[nbitemdoc] = nameTrans;
				    if (import)
				       doc_import[nbitemdoc] = TRUE;
				    nbitemdoc++;
				 }
			       if (typ == CONFIG_NATURE_STRUCT)
				 {
				    nat_items[nbitemnat] = nameOrig;
				    nat_items_menu[nbitemnat] = nameTrans;
				    nbitemnat++;
				 }
			       if (typ == CONFIG_EXCLUSION)
				 {
				    nat_items[nbitemnat] = TtaGetMemory (strlen (nameOrig) + 1);
				    nat_items_menu[nbitemnat] = TtaGetMemory (strlen (nameTrans) + 1);
				    strcpy (nat_items[nbitemnat], nameOrig);
                    strcpy (nat_items_menu[nbitemnat], nameTrans);
				    nbitemnat++;
				 }
			       if (typ == CONFIG_EXTENSION_STRUCT)
				 {
				    ext_items[nbitemext] = nameOrig;
				    ext_items_menu[nbitemext] = nameTrans;
				    nbitemext++;
				 }
			    }
		       }
		     while (ThotDirBrowse_next (&thotDir) == 1);
		  ThotDirBrowse_close (&thotDir);
	       }
	     /* continue a chercher les directories dans le path des schemas */
	     i++;
	     beginning = i;
	  }
     }
}


/*----------------------------------------------------------------------
   ConfigMakeDocTypeMenu cree dans BufMenu la liste des schemas de 
   structure qui ont des fichiers de langue dans les       
   directories de schemas.                                 
   Si doc == TRUE on prend les schemas de documents, sinon 
   les schemas de nature.                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ConfigMakeDocTypeMenu (char* BufMenu, int *lgmenu, ThotBool doc)

#else  /* __STDC__ */
int                 ConfigMakeDocTypeMenu (BufMenu, lgmenu, doc)
STRING              BufMenu;
int                *lgmenu;
ThotBool            doc;

#endif /* __STDC__ */

{
   int                 nbitem, len, i;

   nbitem = 0;
   *lgmenu = 0;
   BufMenu[0] = EOS;
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


/*----------------------------------------------------------------------
   TtaConfigSSchemaExternalName retourne dans nameUser le nom     
   externe, dans la langue de l'utilisateur, du schema de          
   structure dont le nom interne est nameSchema.                    
   Typ indique s'il s'agit d'un schema de document (1), de         
   nature (2) ou d'extension (3).                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaConfigSSchemaExternalName (char* nameUser, char* nameSchema, int Typ)

#else  /* __STDC__ */
void                TtaConfigSSchemaExternalName (nameUser, nameSchema, Typ)
char*               nameUser;
char*               nameSchema;
int                 Typ;

#endif /* __STDC__ */

{
   int                 i;
   ThotBool            found;

   i = 0;
   found = FALSE;
   nameUser[0] = EOS;
   switch (Typ)
	 {
	    case CONFIG_DOCUMENT_STRUCT:
	       while (i < MAX_ITEM_CONF && !found && doc_items[i] != NULL)
		 {
		    if (strcmp (nameSchema, doc_items[i]) == 0)
		      {
			 if (doc_items_menu[i] != NULL)
			    strcpy (nameUser, doc_items_menu[i]);
			 found = TRUE;
		      }
		    else
		       i++;
		 }
	       break;

	    case CONFIG_NATURE_STRUCT:
	       while (i < MAX_ITEM_CONF && !found && nat_items[i] != NULL)
		 {
		    if (strcmp (nameSchema, nat_items[i]) == 0)
		      {
			 if (nat_items_menu[i] != NULL)
			    strcpy (nameUser, nat_items_menu[i]);
			 found = TRUE;
		      }
		    else
		       i++;
		 }
	       break;

	    case CONFIG_EXTENSION_STRUCT:
	       while (i < MAX_ITEM_CONF && !found && ext_items[i] != NULL)
		 {
		    if (strcmp (nameSchema, ext_items[i]) == 0)
		      {
			 if (ext_items_menu[i] != NULL)
			    strcpy (nameUser, ext_items_menu[i]);
			 found = TRUE;
		      }
		    else
		       i++;
		 }
	       break;
	 }
}


/*----------------------------------------------------------------------
   ConfigSSchemaInternalName donne le nom du schema de structure qui 
   correspond a un nom traduit dans la langue de l'utilisateur.    
   Si Doc est vrai, il s'agit d'un schema de document, sinon c'est 
   un schema de nature.                                            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ConfigSSchemaInternalName (char* nameUser, char* nameSchema, ThotBool Doc)

#else  /* __STDC__ */
void                ConfigSSchemaInternalName (nameUser, nameSchema, Doc)
char*               nameUser;
char*               nameSchema;
ThotBool            Doc;

#endif /* __STDC__ */

{
   int                 i;
   ThotBool            found;

   i = 0;
   found = FALSE;
   nameSchema[0] = EOS;
   if (Doc)
      while (i < MAX_ITEM_CONF && !found && doc_items_menu[i] != NULL)
	{
	   if (strcmp (nameUser, doc_items_menu[i]) == 0)
	     {
		if (doc_items[i] != NULL)
		   strcpy (nameSchema, doc_items[i]);
		found = TRUE;
	     }
	   else
	      i++;
	}
   else
      while (i < MAX_ITEM_CONF && !found && nat_items_menu[i] != NULL)
	{
	   if (strcmp (nameUser, nat_items_menu[i]) == 0)
	     {
		if (nat_items[i] != NULL)
		   strcpy (nameSchema, nat_items[i]);
		found = TRUE;
	     }
	   else
	      i++;
	}
}


/*----------------------------------------------------------------------
   openConfigFile                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static FILE        *openConfigFile (char* name, ThotBool lang)

#else  /* __STDC__ */
static FILE        *openConfigFile (name, lang)
char*               name;
ThotBool            lang;

#endif /* __STDC__ */

{

   char                suffix[10];
   char*               ptr;
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

   /* Search in HOME directory */
   ptr = TtaGetEnvString ("APP_HOME");
   strcpy (DirBuffer, ptr);
   MakeCompleteName (name, suffix, DirBuffer, filename, &i);
   if (!TtaFileExist (filename))
     {
       /* compose le nom du fichier a ouvrir avec le nom du directory */
       /* des schemas et le suffixe */
       strncpy (DirBuffer, SchemaPath, MAX_PATH);
       MakeCompleteName (name, suffix, DirBuffer, filename, &i);
     }
   /* ouvre le fichier */
   file = TtaReadOpen (filename);
   return (file);
}

/*----------------------------------------------------------------------
   ConfigMakeMenuPres cree dans BufMenu la liste des schemas de    
   presentation qui peuvent s'appliquer au schema de structure de  
   nom schema.                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ConfigMakeMenuPres (char* schema, char* BufMenu)

#else  /* __STDC__ */
int                 ConfigMakeMenuPres (schema, BufMenu)
char*               schema;
char*               BufMenu;

#endif /* __STDC__ */

{
   int                 nbitem, len, indmenu;
   FILE*               file;
   ThotBool            stop;
   char                line[MAX_TXT_LEN];
   char                text[MAX_TXT_LEN];
   char                textISO[MAX_TXT_LEN];
   char                word[MAX_TXT_LEN];

   nbitem = 0;
   indmenu = 0;
   if (BufMenu != NULL)
      BufMenu[0] = EOS;
   file = openConfigFile (schema, TRUE);
   if (file == NULL)
      return 0;
   stop = FALSE;
   if (readUntil (file, "presentation", ""))
      do
	{
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	      stop = TRUE;
	   else
	     {
		getFirstWord (line, word);
		if (word[0] != EOS)
		   /* la ligne n'est pas vide */
		  {
		     /* si la ligne contient un mot cle marquant le debut d'une autre */
		     /* section, on a fini */
		     if (singleWord (line))
			if (strcmp (word, "export") == 0)
			   stop = TRUE;
			else if (strcmp (word, "import") == 0)
			   stop = TRUE;
			else if (strcmp (word, "translation") == 0)
			   stop = TRUE;
		     if (!stop)
		       {
			  getStringAfterColon (line, text);
			  if (text[0] == EOS)
			     fprintf (stderr, "invalid line in file %s\n   %s\n", schema, line);
			  else
			    {
			       strcpy (textISO, AsciiTranslate (text));
			       if (pres_items[nbitem] != NULL)
				  TtaFreeMemory (pres_items[nbitem]);
			       pres_items[nbitem] = TtaGetMemory (strlen (word) + 1);
			       strcpy (pres_items[nbitem], word);
			       if (pres_items_menu[nbitem] != NULL)
				  TtaFreeMemory (pres_items_menu[nbitem]);
			       len = strlen (textISO) + 1;
			       pres_items_menu[nbitem] = TtaGetMemory (len);
			       strcpy (pres_items_menu[nbitem], textISO);
			       if (BufMenu != NULL)
				 {
				    strcpy (&BufMenu[indmenu], textISO);
				    indmenu += len;
				 }
			       nbitem++;
			    }
		       }
		  }
	     }
	}
      while (!stop);
   TtaReadClose (file);
   return nbitem;
}

/*----------------------------------------------------------------------
   ConfigGetPSchemaName recupere dans la table des schemas de       
   presentation le nom interne du schema qui se trouve     
   a l'entree de rang choix.                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ConfigGetPSchemaName (int choix, char* schpres)

#else  /* __STDC__ */
void                ConfigGetPSchemaName (choix, schpres)
int                 choix;
char*               schpres;

#endif /* __STDC__ */

{
   strcpy (schpres, pres_items[choix - 1]);
}

/*----------------------------------------------------------------------
   ConfigMakeImportMenu cree dans BufMenu la liste des schemas de  
   structure qui ont des fichiers de langue dans les       
   directories de schemas.                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ConfigMakeImportMenu (char* BufMenu)

#else  /* __STDC__ */
int                 ConfigMakeImportMenu (BufMenu)
char*               BufMenu;

#endif /* __STDC__ */

{
   int                 nbitem, len, i, lgmenu;

   lgmenu = 0;
   nbitem = 0;
   BufMenu[0] = EOS;
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


/*----------------------------------------------------------------------
   ConfigMakeMenuExport cree dans BufMenu la liste des schemas de  
   traduction qui peuvent s'appliquer au schema de structure de    
   nom schema.                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ConfigMakeMenuExport (char* schema, char* BufMenu)

#else  /* __STDC__ */
int                 ConfigMakeMenuExport (schema, BufMenu)
char*               schema;
char*              BufMenu;

#endif /* __STDC__ */

{
   int                 indmenu;
   int                 nbitem, len;
   FILE               *file;
   ThotBool            stop;
   char                line[MAX_TXT_LEN];
   char                text[MAX_TXT_LEN];
   char                textISO[MAX_TXT_LEN];
   char                word[MAX_TXT_LEN];

   nbitem = 0;
   indmenu = 0;
   if (BufMenu != NULL)
      BufMenu[0] = EOS;
   file = openConfigFile (schema, TRUE);
   if (file == NULL)
      return 0;
   stop = FALSE;
   if (readUntil (file, "export", ""))
      do
	{
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	      stop = TRUE;
	   else
	     {
		getFirstWord (line, word);
		if (word[0] != EOS)
		   /* la ligne n'est pas vide */
		  {
		     /* si la ligne contient un mot cle marquant le debut d'une autre */
		     /* section, on a fini */
		     if (singleWord (line))
			if (strcmp (word, "presentation") == 0)
			   stop = TRUE;
			else if (strcmp (word, "import") == 0)
			   stop = TRUE;
			else if (strcmp (word, "translation") == 0)
			   stop = TRUE;
		     if (!stop)
		       {
			  getStringAfterColon (line, text);
			  if (text[0] == EOS)
			     fprintf (stderr, "invalid line in file %s\n   %s\n", schema, line);
			  else
			    {
			       strcpy (textISO, AsciiTranslate (text));
			       if (export_items[nbitem] != NULL)
				  TtaFreeMemory (export_items[nbitem]);
			       export_items[nbitem] = TtaGetMemory (strlen (word) + 10);
			       strcpy (export_items[nbitem], word);
			       if (export_items_menu[nbitem] != NULL)
				  TtaFreeMemory (export_items_menu[nbitem]);
			       len = strlen (textISO) + 1;
			       export_items_menu[nbitem] = TtaGetMemory (len);
			       strcpy (export_items_menu[nbitem], textISO);
			       if (BufMenu != NULL)
				 {
				    strcpy (&BufMenu[indmenu], textISO);
				    indmenu += len;
				 }
			       nbitem++;
			    }
		       }
		  }
	     }
	}
      while (!stop);
   TtaReadClose (file);
   return nbitem;
}

/*----------------------------------------------------------------------
   ConfigGetExportSchemaName recupere dans la table des schemas de  
   traduction le nom interne du schema qui se trouve       
   a l'entree de rang choix.                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ConfigGetExportSchemaName (int choix, char* schtrad)

#else  /* __STDC__ */
void                ConfigGetExportSchemaName (choix, schtrad)
int                 choix;
char*               schtrad;

#endif /* __STDC__ */

{
   strcpy (schtrad, export_items[choix - 1]);
}


/*----------------------------------------------------------------------
   Translate                                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static ThotBool     Translate (PtrSSchema pSS, char* word, char* trans)

#else  /* __STDC__ */
static ThotBool     Translate (pSS, word, trans)
PtrSSchema          pSS;
char*               word;
char*               trans;

#endif /* __STDC__ */

{
   ThotBool            found;
   int                 i, j;
   TtAttribute        *pAttr;
   char 		       terme[MAX_NAME_LENGTH];

   found = FALSE;
   strncpy (terme, AsciiTranslate (word), MAX_NAME_LENGTH - 1);
   /* cherche le mot a traduire d'abord parmi les noms d'elements */
   for (i = 0; i < pSS->SsNRules; i++)
      if (strcmp (terme, pSS->SsRule[i].SrName) == 0)
	{
	   strncpy (pSS->SsRule[i].SrName, AsciiTranslate (trans), MAX_NAME_LENGTH - 1);
	   found = TRUE;
	}
   /* cherche ensuite parmi les noms d'attributs et de valeurs d'attributs */
   for (i = 0; i < pSS->SsNAttributes; i++)
	{
	   pAttr = &pSS->SsAttribute[i];
	   if (strcmp (terme, pAttr->AttrName) == 0)
	     {
		strncpy (pAttr->AttrName, AsciiTranslate (trans), MAX_NAME_LENGTH - 1);
		found = TRUE;
	     }
	   else if (pAttr->AttrType == AtEnumAttr)
	      for (j = 0; j < pAttr->AttrNEnumValues; j++)
		 if (strcmp (terme, pAttr->AttrEnumValue[j]) == 0)
		   {
		      strncpy (pAttr->AttrEnumValue[j], AsciiTranslate (trans), MAX_NAME_LENGTH - 1);
		      found = TRUE;
		   }
	}
   /* cherche enfin parmi les regles d'extension, si c'est un schema d'extension */
   if (pSS->SsExtension)
      if (pSS->SsNExtensRules > 0 && pSS->SsExtensBlock != NULL)
	 for (i = 0; i < pSS->SsNExtensRules; i++)
	    if (strcmp (terme, pSS->SsExtensBlock->EbExtensRule[i].SrName) == 0)
	      {
		 strncpy (pSS->SsExtensBlock->EbExtensRule[i].SrName, AsciiTranslate (trans), MAX_NAME_LENGTH - 1);
		 found = TRUE;
	      }
   return found;
}

/*----------------------------------------------------------------------
   ConfigTranslateSSchema                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ConfigTranslateSSchema (PtrSSchema pSS)

#else  /* __STDC__ */
void                ConfigTranslateSSchema (pSS)
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   FILE*   file;
   ThotBool stop, error;
   char*    line;
   char*    text;
   char*    word;

   if (pSS == NULL)
      return;
   /* ouvre le fichier de configuration langue associe' au schema */
   file = openConfigFile (pSS->SsName, TRUE);
   if (file == NULL) 
      /* pas de fichier langue associe' a ce schema de structure */
      return;
   stop = FALSE;
   /* avance dans le fichier jusqu'a la ligne qui contient le seul */
   /* mot "translation" */
   line = TtaGetMemory (MAX_TXT_LEN);
   text = TtaGetMemory (MAX_TXT_LEN);
   word = TtaGetMemory (MAX_TXT_LEN);
   if (readUntil (file, "translation", ""))
      /* lit le fichier ligne a ligne */
      do
	{
	   error = FALSE;
	   /* lit une ligne du fichier */

	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	      /* fin de fichier */
	      stop = TRUE;
	   else
	     {
		/* prend le premier mot de la ligne */
		getFirstWord (line, word);
		if (word[0] != EOS)
		   /* la ligne n'est pas vide */
		  {
		     /* si la ligne contient un mot cle marquant le debut d'une autre */
		     /* section, on a fini */
		     if (singleWord (line))
			if (strcmp (word, "presentation") == 0)
			   stop = TRUE;
			else if (strcmp (word, "export") == 0)
			   stop = TRUE;
			else if (strcmp (word, "import") == 0)
			   stop = TRUE;
			else
			  {
			     fprintf (stderr, "invalid line in file %s\n   %s\n", pSS->SsName, line);
			     error = TRUE;
			  }
		     if (!stop && !error)
		       {
			  /* cherche la chaine de caracteres qui suit ':' */
			  getStringAfterColon (line, text);
			  if (text[0] == EOS)
			     fprintf (stderr, "invalid line in file %s\n   %s\n", pSS->SsName, line);
			  else if (!Translate (pSS, word, text))
			     fprintf (stderr, "invalid line in file %s\n   %s\n", pSS->SsName, line);
		       }
		  }
	     }
	}
      while (!stop);
   TtaFreeMemory (line);
   TtaFreeMemory (text);
   TtaFreeMemory (word);
   TtaReadClose (file);
}

/*----------------------------------------------------------------------
   ConfigDefaultPSchema    retourne dans schpres le nom du 1er     
   schema de presentation associe' au schema de structure schstr   
   dans le fichier .conf correspondant.                            
   Retourne FALSE si pas trouve', TRUE si OK.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ConfigDefaultPSchema (char* schstr, char* schpres)

#else  /* __STDC__ */
ThotBool            ConfigDefaultPSchema (schstr, schpres)
char*               schstr;
char*               schpres;

#endif /* __STDC__ */

{
   ThotBool            ok, stop;
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                word[MAX_TXT_LEN];

   ok = FALSE;
   /* ouvre le fichier .conf associe' au schema de structure */
   file = openConfigFile (schstr, FALSE);
   if (file != NULL)
      /* on a ouvert le fichier .conf */
     {
	/* on cherche la premiere ligne qui commence par le mot "style" */
	stop = FALSE;
	do
	   /* lit une ligne */
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	      /* fin de fichier */
	      stop = TRUE;
	   else
	     {
		/* prend le premier mot de la ligne */
		getFirstWord (line, word);
		if (strcmp (word, "style") == 0)
		  {
		     /* le 1er mot est "style". Cherche le mot qui suit : c'est le */
		     /* nom du schema de presentation cherche' */
		     getSecondWord (line, word);
		     if (word[0] != EOS)
			/* il y a bien un 2eme mot : succes */
		       {
			  strcpy (schpres, word);
			  ok = TRUE;
		       }
		     stop = TRUE;
		  }
	     }
	while (!stop);
	TtaReadClose (file);
     }
   return ok;
}

/*----------------------------------------------------------------------
   readUntilStyle  lit le fichier file (qui doit etre ouvert)      
   jusqu'a trouver une ligne qui contienne le mot "style"  
   suivi du nom namePSchema.                                
   Retourne TRUE si trouve, FALSE sinon.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     readUntilStyle (FILE * file, char* namePSchema)
#else  /* __STDC__ */
static ThotBool     readUntilStyle (file, namePSchema)
FILE               *file;
char*               namePSchema;

#endif /* __STDC__ */
{
   ThotBool            stop;
   ThotBool            ok;
   char                line[MAX_TXT_LEN];
   char                word[MAX_TXT_LEN];

   stop = FALSE;
   ok = FALSE;
   do
      if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	 /* fin de fichier */
	 stop = TRUE;
      else
	{
	   getFirstWord (line, word);
	   if (strcmp (word, "style") == 0)
	     {
		getSecondWord (line, word);
		if (strcmp (word, namePSchema) == 0)
		   ok = TRUE;
	     }
	}
   while (!stop && !ok);
   return ok;
}

/*----------------------------------------------------------------------
   openConfFileAndReadUntil ouvre le fichier .conf qui concerne    
   le schema de structure pSS et avance dans ce fichier    
   jusqu'a la ligne qui marque la section de nom sectName. 
   Retourne le file descriptor du fichier si on a trouve'  
   le fichier .conf et la section, NULL sinon.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static FILE        *openConfFileAndReadUntil (PtrSSchema pSS, char* sectName)

#else  /* __STDC__ */
static FILE        *openConfFileAndReadUntil (pSS, sectName)
PtrSSchema          pSS;
char*               sectName;

#endif /* __STDC__ */
{
   FILE               *file;

   /* ouvre le fichier .conf */
   file = NULL;
   if (pSS != NULL)
      file = openConfigFile (pSS->SsName, FALSE);
   if (file != NULL)
      /* on a ouvert le fichier */
      /* cherche la ligne "style xxxx" qui correspond au schema P concerne' */
      if (!readUntilStyle (file, pSS->SsDefaultPSchema))
	 /* pas trouve' */
	{
	   TtaReadClose (file);
	   file = NULL;
	}
      else
	 /* cherche le debut de la section voulue */
      if (!readUntil (file, sectName, ""))
	 /* pas trouve' */
	{
	   TtaReadClose (file);
	   file = NULL;
	}
   return file;
}


/*----------------------------------------------------------------------
   getNextLineInSection    lit dans line la prochaine ligne du     
   fichier file qui fasse partie de la meme section.       
   Retourne TRUE si succes, FALSE si on a atteint la fin   
   de la section courante ou du fichier.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     getNextLineInSection (FILE * file, char* line)

#else  /* __STDC__ */
static ThotBool     getNextLineInSection (file, line)
FILE               *file;
char*               line;

#endif /* __STDC__ */

{
   ThotBool            ok, stop;
   char                word1[MAX_TXT_LEN];
   char                word2[MAX_TXT_LEN];

   ok = FALSE;
   stop = FALSE;
   do
      /* lit une ligne */
      if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	 /* fin de fichier */
	 stop = TRUE;
      else
	{
	   /* prend le permier mot de la ligne lue */
	   getFirstWord (line, word1);
	   if (word1[0] != EOS)
	      /* la ligne n'est pas vide */
	     {
		/* si la ligne contient un mot cle marquant le debut d'une autre */
		/* section, on a fini */
		if (singleWord (line))
		   /* la ligne contient un seul mot */
		  {
		     if (strcmp (word1, "open") == 0)
			stop = TRUE;
		     else if (strcmp (word1, "geometry") == 0)
			stop = TRUE;
		     else if (strcmp (word1, "presentation") == 0)
			stop = TRUE;
		     else if (strcmp (word1, "options") == 0)
			stop = TRUE;
		     else
			/* ligne contenant un seul mot. on considere que c'est OK... */
			ok = TRUE;
		  }
		else
		   /* la ligne contient plus d'un mot */
		if (strcmp (word1, "style") == 0)
		  {
		     getSecondWord (line, word2);
		     if (word2[0] != TEXT(':'))
			/* la ligne est du type "style xxxx". C'est une fin de section */
			stop = TRUE;
		     else
			ok = TRUE;
		  }
		else
		   /* la ligne ne commence pas par "style". OK */
		   ok = TRUE;
	     }
	}
   while (!stop && !ok);
   return ok;
}

/*----------------------------------------------------------------------
   ConfigKeyboard.                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void             ConfigKeyboard (int *x, int *y)
#else  /* __STDC__ */
void             ConfigKeyboard (x, y)
int             *x;
int             *y;
#endif /* __STDC__ */
{
   FILE         *file;
   char          seqLine[MAX_TXT_LEN];
   char          line[MAX_TXT_LEN];
   int           nbIntegers;

   *x = 600;
   *y = 100;
   file = openConfigFile ("keyboard", FALSE);
   if (file == NULL)
      return;

   getNextLineInSection (file, line);

   /* extrait la partie de la ligne qui suit les deux-points */
   getStringAfterColon (line, seqLine);
   if (seqLine[0] != EOS)
     {
       /* extrait les 4 entiers */
       nbIntegers = sscanf (seqLine, "%d %d", x, y);
       if (nbIntegers == 2)
         if (DOT_PER_INCHE != 83)
	   {
	     /* convertit si necessaire en fonction de la resolution de l'ecran */
	     *x = (int) ((float) (*x * 83) / (float) DOT_PER_INCHE);
	     *y = (int) ((float) (*y * 83) / (float) DOT_PER_INCHE);
	   }
     }
   TtaReadClose (file);
}

/*----------------------------------------------------------------------
   getXYWidthHeight        lit les 4 entiers x, y, width, height   
   suivent les deux-points dans une ligne de la section    
   open ou geometry d'un fichier .conf                     
   Retourne TRUE si succes.                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     getXYWidthHeight (char* line, PtrDocument pDoc, int *x, int *y,
				      int *width, int *height)
#else  /* __STDC__ */
static ThotBool     getXYWidthHeight (line, pDoc, x, y, width, height)
char*               line;
PtrDocument         pDoc;
int                *x;
int                *y;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   char                seqLine[MAX_TXT_LEN];
   int                 nbIntegers;
   ThotBool            result;

   result = FALSE;
   /* extrait la partie de la ligne qui suit les deux-points */
   getStringAfterColon (line, seqLine);
   if (seqLine[0] == EOS)
      fprintf (stderr, "invalid line in file %s.conf\n   %s\n",
	       pDoc->DocSSchema->SsName, line);
   else
     {
	/* extrait les 4 entiers */
	nbIntegers = sscanf (seqLine, "%d %d %d %d", x, y, width, height);
	if (nbIntegers != 4)
	   fprintf (stderr, "invalid line in file %s.conf\n   %s\n",
		    pDoc->DocSSchema->SsName, line);
	else
	  {
	     /* convertit si necessaire en fonction de la resolution de l'ecran */
	     if (DOT_PER_INCHE != 83)
	       {
		  *x = (int) ((float) (*x * 83) / (float) DOT_PER_INCHE);
		  *width = (int) ((float) (*width * 83) / (float) DOT_PER_INCHE);
		  *y = (int) ((float) (*y * 83) / (float) DOT_PER_INCHE);
		  *height = (int) ((float) (*height * 83) / (float) DOT_PER_INCHE);
	       }
	     result = TRUE;
	  }
     }
   return result;
}

/*----------------------------------------------------------------------
   ConfigOpenFirstViews ouvre, pour le document pDoc, les vues     
   specifiees dans la section open du fichier de           
   configuration .conf                                     
  ----------------------------------------------------------------------*/
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
   char                nameview[MAX_TXT_LEN];

   /* ouvre le fichier .conf du document et avance jusqu'a la section "open" */
   file = openConfFileAndReadUntil (pDoc->DocSSchema, "open");
   if (file != NULL)
     {
	/* on a trouve' le debut de la section open. On lit le fichier .conf */
	/* ligne par ligne, jusqu'a la fin de cette section */
	while (getNextLineInSection (file, line))
	  {
	     /* le 1er mot de la ligne est le nom d'une vue a ouvrir */
	     getFirstWord (line, nameview);
	     /* lit les coordonnees (x, y) et dimensions (width, height) de la */
	     /* frame ou doit s'afficher la vue */
	     if (getXYWidthHeight (line, pDoc, &x, &y, &width, &height))
		/* lecture reussie, on ouvre la vue */
		OpenViewByName (pDoc, nameview, x, y, width, height);
	  }
	TtaReadClose (file);
     }
}

/*----------------------------------------------------------------------
   ConfigGetViewGeometry retourne la position (x, y) et les        
   dimensions (width, height) de la fenetre ou doit        
   s'afficher la vue de non view pour le document pDoc.     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ConfigGetViewGeometry (PtrDocument pDoc, char* view, int *x, int *y, int *width, int *height)
#else  /* __STDC__ */
void                ConfigGetViewGeometry (pDoc, view, x, y, width, height)
PtrDocument         pDoc;
char*               view;
int                *x;
int                *y;
int                *width;
int                *height;

#endif /* __STDC__ */
{
   FILE               *file;
   char               line[MAX_TXT_LEN];
   char               nameview[MAX_TXT_LEN];
   ThotBool            found;

   *x = 0;
   *y = 0;
   *width = 0;
   *height = 0;

   /* ouvre le fichier .conf du document et avance jusqu'a la section 
      "open" */
   file = openConfFileAndReadUntil (pDoc->DocSSchema, "open");
   if (file != NULL)
     {
       /* on a trouve' le debut de la section open. On lit le fichier 
	  .conf ligne par ligne, jusqu'a la ligne qui commence par le 
	  name de la vue */
       found = FALSE;
       while (!found && getNextLineInSection (file, line))
	 {
	   /* le 1er mot de la ligne est le nom d'une vue */
	   getFirstWord (line, nameview);
	   /* est-ce le nom de la vue cherchee ? */
	   found = (strcmp (nameview, view) == 0);
	 }
       if (!found)
	 /* on n'a pas trouve' dans la section "open". On cherche dans la
	    section "geometry" */
	 {
	   TtaReadClose (file);
	   file = openConfFileAndReadUntil (pDoc->DocSSchema, "geometry");
	   if (file != NULL)
	       while (!found && getNextLineInSection (file, line))
		 {
		   /* le 1er mot de la ligne est le nom d'une vue */
		   getFirstWord (line, nameview);
		   /* est-ce le nom de la vue cherchee ? */
		   found = (strcmp (nameview, view) == 0);
		 }
	 }
       TtaReadClose (file);
       if (found)
	 getXYWidthHeight (line, pDoc, x, y, width, height);
     }
}

/*----------------------------------------------------------------------
   pixeltomm converts pixels into mm
   motif_conversion is true whenever we want to convert the geometry value
   returned by motif.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          pixeltomm (int N, int horiz, ThotBool motif_conversion)
#else  /* __STDC__ */
static int          pixeltomm (N, horiz, motif_conversion)
int                 N;
int                 horiz;
ThotBool            motif_conversion;

#endif /* __STDC__ */
{
  if (DOT_PER_INCHE != 83)
    {
      /* converts the resolution of the screen, if needed */
      N = (int) (((float) (N * DOT_PER_INCHE) + 0.05) / 83);
    }

  if (motif_conversion)
    {
      if (horiz)
	N =  (N * 254) / (DOT_PER_INCHE * 10);
      else
	N = (N * 254) / (DOT_PER_INCHE * 10);
    }

  return N;
}

/*----------------------------------------------------------------------
   TtaGetViewWH returns the current width and height values associated
   with the frame where a view is displayed
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGetViewWH (Document doc, int view, int *width, int *height)
#else  /* __STDC__ */
void                TtaGetViewWH (doc, view, width, height)
Document       doc;
int            view;
int           *width;
int           *height;
#endif /* __STDC__ */
{
#ifndef _WINDOWS
  int                 frame;
  int                 n;
  ThotWidget          widget;
#ifdef _GTK
  GtkArg              args[20];
#else /* ! _GTK */
  Arg                 args[20];
#endif /* ! _GTK */
  Dimension           w, h;

  frame =  GetWindowNumber (doc, view);
  widget = (ThotWidget) FrameTable[frame].WdFrame;
#ifdef _GTK
  widget = widget->parent->parent;
    
  args[0].name = "width";
  args[1].name = "height";
  gtk_object_getv(GTK_OBJECT(widget), 
                  2, 
                  args);


#else /* !_GTK */
  widget = XtParent (XtParent (widget));
  /* Ask X what's the geometry of the frame */
  n = 0;
  XtSetArg (args[n], XmNwidth, &w);
  n++;
  XtSetArg (args[n], XmNheight, &h);
  n++;
  XtGetValues (widget, args, n);
#endif /* !_GTK */
  /* convert the result into mm */
  *width = pixeltomm ((int) w, 1, TRUE);
  *height = pixeltomm ((int) h, 0, TRUE);
#else /* !_WINDOWS */
  int  frame;
  HWND hWnd;
  RECT rect;

  frame = GetWindowNumber (doc, view);
  hWnd = FrMainRef[frame];
  /* ask Windows what's the geometry of the frame */
  if (!GetWindowRect (hWnd, &rect))
    *width = *height = 0;
  else 
    {
      /* convert the result into mm */
      *width = (int) (rect.right - rect.left);
      *height = (int) (rect.bottom - rect.top);
      *width = pixeltomm (*width, 1, TRUE);
      *height = pixeltomm (*height, 0, TRUE);
    }
#endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
   TtaGetViewGeometryRegistry returns the position (x, y) and sizes        
   (width, height) of the frame where the view is displayed. These values
   are read from the Thot registry.
   Parameters:    document: the document.                  
   name: the name of the view in P schema.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGetViewGeometryRegistry (Document document, char* name,
						int *x, int *y, 
						int *width, int *height)
#else  /* __STDC__ */
void                TtaGetViewGeometryRegistry (document, name, x, y, width, 
						height)
Document            document;
char*              name;
int                *x;
int                *y;
int                *width;
int                *height;

#endif /* __STDC__ */
{
  PtrDocument pDoc;
  char     line[MAX_TXT_LEN];
  char*    ptr;
  ThotBool found;
  UserErrorCode = 0;
  *x = 0;
  *y = 0;
  *width = 0;
  *height = 0;

  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (document != 0)
    {
      pDoc = LoadedDocument[document - 1];
      ptr = TtaGetEnvString (name);
      if (!ptr || ptr[0] == EOS)
	found = FALSE;
      else
	found = TRUE;
      
      if (found)
	{
	  sprintf (line, ":%s", ptr);
	  getXYWidthHeight (line, pDoc, x, y, width, height);
	}
      else
	ConfigGetViewGeometry (pDoc, name, x, y, width, height);
    }
}

/*----------------------------------------------------------------------
   TtaGetViewGeometry returns the position (x, y) and sizes        
   (width, height) of the frame wher view is displayed.    
   Parameters:    document: the document.                  
   name: the name of the view in P schema.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGetViewGeometry (Document document, char* name, int *x, int *y, int *width, int *height)
#else  /* __STDC__ */
void                TtaGetViewGeometry (document, name, x, y, width, height)
Document            document;
char*               name;
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
      ConfigGetViewGeometry (LoadedDocument[document - 1], name, x, y, width, height);
}

/*----------------------------------------------------------------------
   TtaGetViewGeometryMM returns the position (x, y) and sizes        
   (width, height) of the frame where a view is displayed. The values
   returned are in mm
   Parameters:    document: the document.                  
   name: the name of the view in P schema.  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGetViewGeometryMM (Document document, char* name, int *x, int *y, int *width, int *height)
#else  /* __STDC__ */
void                TtaGetViewGeometryMM (document, name, x, y, width, height)
Document            document;
char*               name;
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
      ConfigGetViewGeometry (LoadedDocument[document - 1], name, x, y, width, height);
   /* the above function returns the geometry in pixels, so we'll now convert
      it to mm */
   *x = pixeltomm (*x, 1, FALSE);
   *y = pixeltomm (*y, 0, FALSE);
   *width = pixeltomm (*width, 1, FALSE);
   *height = pixeltomm (*height, 0, FALSE);
}

/*----------------------------------------------------------------------
   ConfigGetPSchemaNature retourne dans presNature le nom du       
   schema de presentation a appliquer a la nature de nom   
   nameNature dans le contexte du schema de structure pSS   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ConfigGetPSchemaNature (PtrSSchema pSS, char* nameNature, char* presNature)
#else  /* __STDC__ */
ThotBool            ConfigGetPSchemaNature (pSS, nameNature, presNature)
PtrSSchema          pSS;
char*               nameNature;
char*               presNature;

#endif /* __STDC__ */
{
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                seqLine[MAX_TXT_LEN];
   char                name[MAX_TXT_LEN];
   ThotBool            found;
   ThotBool            ok;

   presNature[0] = EOS;
   ok = FALSE;
   /* ouvre le fichier .conf du document et avance jusqu'a la section "presentation" */
   file = openConfFileAndReadUntil (pSS, "presentation");
   if (file != NULL)
     {
	/* on a trouve' le debut de la section presentation. On lit le fichier */
	/* ligne par ligne jusqu'a la ligne qui commence par le nom de la nature */
	found = FALSE;
	while (!found && getNextLineInSection (file, line))
	  {
	     /* le 1er mot de la ligne est le nom d'une nature */
	     getFirstWord (line, name);
	     /* est-ce le nom de la nature cherchee ? */
	     found = (strcmp (name, nameNature) == 0);
	  }
	if (found)
	   /* on a trouve' la ligne de la section presentation qui commence par */
	   /* le nom de la nature voulue */
	  {
	     /* le nom de nature est suivi, apres ":", du nom du schema de */
	     /* presentation a appliquer */
	     getStringAfterColon (line, seqLine);
	     if (seqLine[0] == EOS)
		fprintf (stderr, "invalid line in file %s.conf\n   %s\n", pSS->SsName, line);
	     else
	       {
		  strncpy (presNature, seqLine, MAX_NAME_LENGTH - 1);
		  ok = TRUE;
	       }
	  }
	TtaReadClose (file);
     }
   return ok;
}

/*----------------------------------------------------------------------
   ConfigGetPresentationOption cherche, dans le fichier .conf      
   correspondant au schema de structure pSS, la valeur     
   de l'option de presentation de nom optionName.          
   Retourne la valeur trouvee dans optionValue, ou une     
   chaine vide si l'option n'est pas trouvee.              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ConfigGetPresentationOption (PtrSSchema pSS, char* optionName, char* optionValue)

#else  /* __STDC__ */
void                ConfigGetPresentationOption (pSS, optionName, optionValue)
PtrSSchema          pSS;
char*               optionName;
char*               optionValue;

#endif /* __STDC__ */

{
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                seqLine[MAX_TXT_LEN];
   char                name[MAX_TXT_LEN];
   ThotBool            found;

   optionValue[0] = EOS;
   /* ouvre le fichier .conf du document et avance jusqu'a la section "options" */
   file = openConfFileAndReadUntil (pSS, "options");
   if (file != NULL)
     {
	/* on a trouve' le debut de la section options. On lit le fichier */
	/* ligne par ligne jusqu'a la ligne qui commence par le nom de l'option */
	/* chercheee */
	found = FALSE;
	while (!found && getNextLineInSection (file, line))
	  {
	     /* le 1er mot de la ligne est le nom d'une option */
	     getFirstWord (line, name);
	     /* est-ce le nom de l'option cherchee ? */
	     found = (strcmp (name, optionName) == 0);
	  }
	if (found)
	   /* on a trouve' la ligne de la section options qui commence par */
	   /* le nom de l'option voulue */
	  {
	     /* le nom de l'option est suivi, apres ":", de la valeur de l'option */
	     getStringAfterColon (line, seqLine);
	     if (seqLine[0] == EOS)
		fprintf (stderr, "invalid line in file %s.conf\n   %s\n", pSS->SsName, line);
	     else
		strncpy (optionValue, seqLine, MAX_NAME_LENGTH - 1);
	  }
	TtaReadClose (file);
     }
}


/*----------------------------------------------------------------------
   ConfigGetPSchemaForPageSize cherche, dans le fichier .conf      
   correspondant au schema de structure pSS, le schema     
   de presentation qui comporte une option "pagesize"      
   ayant la valeur pageSize.                               
   Retourne le nom du schema trouve' dans schemaName, ou   
   une chaine vide si pas trouve'.                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ConfigGetPSchemaForPageSize (PtrSSchema pSS, char* pageSize, char* schemaName)

#else  /* __STDC__ */
void                ConfigGetPSchemaForPageSize (pSS, pageSize)
PtrSSchema          pSS;
char*               pageSize;
char*               schemaName;

#endif /* __STDC__ */

{
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                word[MAX_TXT_LEN];
   char                seqLine[MAX_TXT_LEN];
   char                lastStyle[MAX_TXT_LEN];
   char	               bestStyle[MAX_TXT_LEN];
   int		           lastPrefixLen, bestPrefixLen;
   int		           score, i;
   ThotBool            stop;

   schemaName[0] = EOS;
   score = 0;
   /* ouvre le fichier .conf correspondant au type du document */
   file = NULL;
   if (pSS != NULL)
      file = openConfigFile (pSS->SsName, FALSE);
   if (file != NULL)
      /* on a ouvert le fichier, on va le lire ligne par ligne */
     {
	lastStyle[0] = EOS;
	bestStyle[0] = EOS;
	bestPrefixLen = 0;
	stop = FALSE;
	do
	   /* on lit une ligne */
	   if (fgets (line, MAX_TXT_LEN - 1, file) == NULL)
	      /* fin de fichier */
	      stop = TRUE;
	   else
	     {
		/* prend le 1er mot de la ligne lue */
		getFirstWord (line, word);
		if (strcmp (word, "style") == 0)
		   /* c'est une ligne "style". On conserve le nom du schema de */
		   /* presentation qui suit le mot-cle "style" */
		  {
		    getSecondWord (line, lastStyle);
		    lastPrefixLen=0;
		    while(lastStyle[lastPrefixLen]!=EOS &&
			  (pSS->SsDefaultPSchema)[lastPrefixLen]!=EOS &&
			  lastStyle[lastPrefixLen]==(pSS->SsDefaultPSchema)[lastPrefixLen])
		      lastPrefixLen ++;
		    score = lastPrefixLen;
		    i = 0;
		    while(lastStyle[lastPrefixLen]!=EOS)
		      {
			if(pageSize[i] ==EOS ||
			   pageSize[i]!= lastStyle[lastPrefixLen])
			  score --;
			lastPrefixLen++;
			i++;
		      }
			  
		  }
		else if (strcmp (word, "pagesize") == 0)
		   /* c'est une ligne "pagesize", on la traite */
		  {
		     getStringAfterColon (line, seqLine);
		     if (seqLine[0] == EOS)
			fprintf (stderr, "invalid line in file %s.conf\n   %s\n",
				 pSS->SsName, line);
		     else if (strcmp (seqLine, pageSize) == 0)
			/* c'est le format de page cherche'. On a fini */
		       if(score > bestPrefixLen)
			 {
			   strcpy (bestStyle, lastStyle);
			   bestPrefixLen = score;
			 }
		  }
	     }
	while (!stop);
	TtaReadClose (file);
	strcpy (schemaName, bestStyle);
     }
}


/*----------------------------------------------------------------------
   MakeMenuPattern cree dans BufMenu la liste des motifs de        
   remplissage disponibles et retourne le nombre de motifs ou      
   0 si echec.                                                     
   LgMax indique la taille du buffer BufMenu.                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 MakeMenuPattern (STRING BufMenu, int LgMax)

#else  /* __STDC__ */
int                 MakeMenuPattern (BufMenu, LgMax)
STRING              BufMenu;
int                 LgMax;

#endif /* __STDC__ */

{
   int                 nbentree;
   CHAR_T                name[80];
   int                 lgname;
   int                 lgmenu;
   int                 max;
   STRING              ptr;

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
		  ustrcpy (name, PatternName (nbentree));
		  lgname = ustrlen (name) + 1;
	       }
	     else
		lgname = 1;

	     if (lgmenu + lgname > LgMax)
		lgmenu = LgMax;
	     else if (lgname > 1)
	       {
		  nbentree++;
		  ustrcpy (ptr, name);
		  ptr += lgname;
		  lgmenu += lgname;
	       }
	  }
	while (lgmenu < LgMax && lgname > 1);
     }
   return nbentree;
}


/*----------------------------------------------------------------------
   MakeMenuColor cree dans le buffer BufMenu la liste des couleurs 
   disponibles et retourne le nombre d'entrees creees ou           
   0 si echec.                                                     
   LgMax indique la taille du buffer BufMenu.                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 MakeMenuColor (STRING BufMenu, int LgMax)

#else  /* __STDC__ */
int                 MakeMenuColor (BufMenu, LgMax)
STRING              BufMenu;
int                 LgMax;

#endif /* __STDC__ */

{
   int                 nbentree;
   int                 lgmenu;
   int                 lgname;
   int                 max;
   char                name[80];
   char*               ptr;

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

/*----------------------------------------------------------------------
   ConfigDefaultTypoSchema retourne dans schtypo le nom du         
   schema de typographie a appliquer a la nature de nom            
   nameNature dans le contexte du schema de structure pSS           
   Retourne FALSE si pas trouve', TRUE si OK.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ConfigDefaultTypoSchema (PtrSSchema pSS,
					     char* nameNature,
					     char* schtypo)
#else  /* __STDC__ */
ThotBool            ConfigDefaultTypoSchema (pSS, nameNature, schtypo)
PtrSSchema          pSS;
char*               nameNature;
char*               schtypo;

#endif /* __STDC__ */

{
   ThotBool            ok, found;
   FILE               *file;
   char                line[MAX_TXT_LEN];
   char                seqLine[MAX_TXT_LEN];
   char                name[MAX_TXT_LEN];

   /* ouvre le fichier .conf du document */
   /* et avance jusqu'a la section "typography" */
   file = openConfFileAndReadUntil (pSS, "typography");
   ok = FALSE;
   if (file != NULL)
     {
	/* on a trouve' le debut de la section typography. On lit le fichier */
	/* .conf ligne par ligne jusqu'a la ligne qui commence */
	/* par le nom de la nature */
	found = FALSE;
	while (!found && getNextLineInSection (file, line))
	  {
	     /* le 1er mot de la ligne est le nom d'une nature */
	     getFirstWord (line, name);
	     /* est-ce le nom de la nature cherchee ? */
	     found = (strcmp (name, nameNature) == 0);
	  }
	if (found)
	   /* on a trouve' la ligne de la section typography qui commence par */
	   /* le nom de la nature voulue */
	  {
	     /* le nom de nature est suivi, apres ":", du nom du schema de */
	     /* typographie a appliquer */
	     getStringAfterColon (line, seqLine);
	     if (seqLine[0] == EOS)
		fprintf (stderr, "invalid line in file %s.conf\n   %s\n", pSS->SsName, line);
	     else
	       {
		  strncpy (schtypo, seqLine, MAX_NAME_LENGTH - 1);
		  ok = TRUE;
	       }
	  }
	TtaReadClose (file);
     }
   return ok;

}				/*ConfigDefaultTypoSchema */
