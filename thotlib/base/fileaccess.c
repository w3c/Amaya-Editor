/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *  Input/output for Thot binary files (schemas and pivot files)
 *
 * Authors: V. Quint (INRIA)
 *          D. Veillard (INRIA) - new functions for MS-Windows
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "thotdir.h"
#include "fileaccess.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"

#include "fileaccess_f.h"
#include "platform_f.h"
#include "registry_f.h"

/* ---------------------------------------------- */
/* |  Constants for read and write operations   | */
/* ---------------------------------------------- */

#define LMASK           0x000000ffL
 
#define DECAL_3         (3 * 8)
#define DECAL_2         (2 * 8)
#define DECAL_1         (1 * 8)

#define SIGNED_SHORT_MASK	0xffff0000L

/*----------------------------------------------------------------------
   TtaReadByte reads a character (or byte) value.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             TtaReadByte (BinFile file, char* bval)
#else  /* __STDC__ */
ThotBool            TtaReadByte (file, bval)
BinFile             file;
char*               bval;

#endif /* __STDC__ */
{
   char v;
   if (fread (&v, sizeof (char), 1, file) == 0) {
      *bval = (CHAR_T) EOS;
      return (FALSE);
   } 
   *bval = (CHAR_T) v;
   return (TRUE);
}

/*----------------------------------------------------------------------
   TtaReadBool reads a ThotBool value.                              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaReadBool (BinFile file, ThotBool * bval)
#else  /* __STDC__ */
ThotBool            TtaReadBool (file, bval)
BinFile             file;
ThotBool           *bval;

#endif /* __STDC__ */
{
   CHAR_T       b1;

   if (!TtaReadByte (file, &b1))
     {
	*bval = FALSE;
	return (FALSE);
     }
   else
     {
       *bval = (b1 == 1);
       return (TRUE);
     }
}


/*----------------------------------------------------------------------
   TtaReadShort reads an unsigned short value.                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaReadShort (BinFile file, int *sval)
#else  /* __STDC__ */
ThotBool            TtaReadShort (file, sval)
BinFile             file;
int                *sval;

#endif /* __STDC__ */
{
  CHAR_T      car;
 
  *sval = 0; 
  if (!TtaReadByte (file, &car))
    return (FALSE);
  else
    {
      *sval |= ((((int) car) & LMASK) << DECAL_1);
      if (!TtaReadByte (file, &car))
	{
	  *sval = 0;
	  return (FALSE);
	}
      else
	{
	  *sval |= (((int) car) & LMASK);
	  return (TRUE);
	}
    }
}
 

/*----------------------------------------------------------------------
   TtaReadSignedShort reads a signed short value.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaReadSignedShort (BinFile file, int *sval)
#else  /* __STDC__ */
ThotBool            TtaReadSignedShort (file, sval)
BinFile             file;
int                *sval;

#endif /* __STDC__ */
{
  CHAR_T      car;
 
  *sval = 0;
  if (!TtaReadByte (file, &car))
    return (FALSE);
  else
    {
      if (((int) car) < 0 || ((int) car) > 127 )
	*sval = SIGNED_SHORT_MASK;
      *sval |= ((((int) car) & LMASK) << DECAL_1);
      if (!TtaReadByte (file, &car))
	{
	  *sval = 0;
	  return (FALSE);
	}
      else
	{
	  *sval |= (((int) car) & LMASK);
	  return (TRUE);
	}
    }
}


/*----------------------------------------------------------------------
   TtaReadInteger reads an integer.                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaReadInteger (BinFile file, int *sval)
#else  /* __STDC__ */
ThotBool            TtaReadInteger (file, sval)
BinFile             file;
int                *sval;

#endif /* __STDC__ */
{
  CHAR_T      car;
 
  *sval = 0;
   if (!TtaReadByte (file, &car))
       return (FALSE);
   else
     {
       *sval |= ((((int) car) & LMASK) << DECAL_3);
       if (!TtaReadByte (file, &car))
	 {
	   *sval = 0;
	   return (FALSE);
	 }
       else
	 {
	   *sval |= ((((int) car) & LMASK) << DECAL_2);
	   if (!TtaReadByte (file, &car))
	     {
	       *sval = 0;
	       return (FALSE);
	     }
	   else
	     {
	       *sval |= ((((int) car) & LMASK) << DECAL_1);
	       if (!TtaReadByte (file, &car))
		 {
		   *sval = 0;
		   return (FALSE);
		 }
	       else
		 {
		   *sval |= (((int) car) & LMASK);
		   return (TRUE);
		 }
	     }
	 }
     }
}


/*----------------------------------------------------------------------
   TtaReadName reads a string value.                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaReadName (BinFile file, STRING name)
#else  /* __STDC__ */
ThotBool            TtaReadName (file, name)
BinFile             file;
STRING              name;

#endif /* __STDC__ */
{
   int                 i;

   for (i = 0; i < MAX_NAME_LENGTH; i++)
     {
	if (!TtaReadByte (file, &name[i]))
	  {
	     name[i] = EOS;
	     return FALSE;
	  }
	if (name[i] == EOS)
	   break;
     }
   if (i >= MAX_NAME_LENGTH)
     {
	name[0] = EOS;
	return FALSE;
     }
   return TRUE;
}



/*----------------------------------------------------------------------
   TtaReadOpen opens a file for reading.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
BinFile             TtaReadOpen (CONST PathBuffer filename)
#else  /* __STDC__ */
BinFile             TtaReadOpen (filename)
CONST PathBuffer    filename;

#endif /* __STDC__ */
{
   if (filename && filename [0] != EOS)
#     ifdef _WINDOWS
      return ufopen (filename, _RBinaryMODE_);
#     else
      return fopen (filename, "r");
#     endif
   else
	   return (BinFile) NULL;
}


/*----------------------------------------------------------------------
   TtaReadClose closes a file.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaReadClose (BinFile file)
#else  /* __STDC__ */
void                TtaReadClose (file)
BinFile             file;

#endif /* __STDC__ */
{
   if (file != NULL)
      fclose (file);
}


/*----------------------------------------------------------------------
   TtaWriteOpen opens a file for writing.                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
BinFile             TtaWriteOpen (CONST STRING filename)
#else  /* __STDC__ */
BinFile             TtaWriteOpen (filename)
CONST STRING        filename;

#endif /* __STDC__ */
{
#ifdef _WINDOWS
   return ufopen (filename, _AppendBinaryMODE_);
#else
   return ufopen (filename, _AppendMODE_);
#endif
}


/*----------------------------------------------------------------------
   TtaWriteClose closes a file.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaWriteClose (BinFile file)
#else  /* __STDC__ */
void                TtaWriteClose (file)
BinFile             file;

#endif /* __STDC__ */
{
   if (file != NULL)
      fclose (file);
}

/*----------------------------------------------------------------------
   TtaWriteByte writes a character (or byte) value.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaWriteByte (BinFile file, CHAR_T bval)
#else  /* __STDC__ */
ThotBool            TtaWriteByte (file, bval)
BinFile             file;
CHAR_T                bval;

#endif /* __STDC__ */
{
   if (fwrite ((char*) &bval, sizeof (char), 1, file) == 0)
      return FALSE;
   return TRUE;
}


/*----------------------------------------------------------------------
   TtaWriteShort reads an unsigned short value.
   -------------------------------------------------------------------- */

#ifdef __STDC__
ThotBool  TtaWriteShort (BinFile file, int sval)
#else			     /* __STDC__ */
ThotBool  TtaWriteShort (file, sval)
BinFile   file;
int       sval;

#endif			     /* __STDC__ */

{

   if (!TtaWriteByte (file, (CHAR_T) ((sval >> DECAL_1) & LMASK)))
      return FALSE;

   if (!TtaWriteByte (file, (CHAR_T) (sval & LMASK)))
      return FALSE;

   return TRUE;
}

/*----------------------------------------------------------------------
   TtaWriteInteger writes an integer.
   -------------------------------------------------------------------- */

#ifdef __STDC__
ThotBool  TtaWriteInteger (BinFile file, int lval)
#else			     /* __STDC__ */
ThotBool  TtaWriteInteger (file, lval)
BinFile   file;
int       lval;

#endif			     /* __STDC__ */

{

   if (!TtaWriteByte (file, (CHAR_T) ((lval >> DECAL_3) & LMASK)))
      return FALSE;

   if (!TtaWriteByte (file, (CHAR_T) ((lval >> DECAL_2) & LMASK)))
      return FALSE;

   if (!TtaWriteByte (file, (CHAR_T) ((lval >> DECAL_1) & LMASK)))
      return FALSE;

   if (!TtaWriteByte (file, (CHAR_T) (lval & LMASK)))
      return FALSE;

   return TRUE;
}


/*----------------------------------------------------------------------
   TtaWriteDocIdent writes a document identifier.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaWriteDocIdent (BinFile file, DocumentIdentifier Ident)
#else  /* __STDC__ */
void                TtaWriteDocIdent (file, Ident)
BinFile             file;
DocumentIdentifier  Ident;

#endif /* __STDC__ */
{
   int                 j;

   j = 1;
   while (j < MAX_DOC_IDENT_LEN && Ident[j - 1] != EOS)
     {
	TtaWriteByte (file, Ident[j - 1]);
	j++;
     }
   /* termine le nom par un octet nul */
   TtaWriteByte (file, EOS);
}

/*----------------------------------------------------------------------
   TtaReadDocIdent                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaReadDocIdent (BinFile file, DocumentIdentifier * Ident)
#else  /* __STDC__ */
void                TtaReadDocIdent (file, Ident)
BinFile             file;
DocumentIdentifier *Ident;

#endif /* __STDC__ */
{
   int                 j;

   j = 0;
   do
      if (!TtaReadByte (file, &((*Ident)[j++])))
	 (*Ident)[j - 1] = EOS;
   while (!(j >= MAX_DOC_IDENT_LEN || (*Ident)[j - 1] == EOS)) ;
}

/*----------------------------------------------------------------------
   CopyDocIdent                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CopyDocIdent (DocumentIdentifier * Dest, DocumentIdentifier Source)
#else  /* __STDC__ */
void                CopyDocIdent (Dest, Source)
DocumentIdentifier *Dest;
DocumentIdentifier  Source;

#endif /* __STDC__ */
{
   ustrncpy (*Dest, Source, MAX_DOC_IDENT_LEN);
}

/*----------------------------------------------------------------------
   SameDocIdent                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            SameDocIdent (DocumentIdentifier Ident1, DocumentIdentifier Ident2)
#else  /* __STDC__ */
ThotBool            SameDocIdent (Ident1, Ident2)
DocumentIdentifier  Ident1;
DocumentIdentifier  Ident2;

#endif /* __STDC__ */
{
   ThotBool            ret;

   ret = (ustrcmp (Ident1, Ident2) == 0);
   return ret;
}

/*----------------------------------------------------------------------
   ClearDocIdent                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearDocIdent (DocumentIdentifier * Ident)
#else  /* __STDC__ */
void                ClearDocIdent (Ident)
DocumentIdentifier *Ident;

#endif /* __STDC__ */
{
   (*Ident)[0] = EOS;
}

/*----------------------------------------------------------------------
   DocIdentIsNull                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            DocIdentIsNull (DocumentIdentifier Ident)
#else  /* __STDC__ */
ThotBool            DocIdentIsNull (Ident)
DocumentIdentifier  Ident;

#endif /* __STDC__ */
{
   ThotBool            ret;

   ret = (Ident[0] == EOS);
   return ret;
}


/*----------------------------------------------------------------------
   TtaExtractName: extracts the directory and the file name.       
   aDirectory and aName must be arrays of characters       
   which sizes are sufficient to contain the path and      
   the file name.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaExtractName (char* text, char* aDirectory, char* aName)

#else  /* __STDC__ */
void                TtaExtractName (text, aDirectory, aName)
char*               text;
char*               aDirectory;
char*               aName;

#endif /* __STDC__ */
{
   int                 lg, i, j;
   STRING              ptr, oldptr;
   CHAR_T              URL_DIR_SEP;

   if (text == NULL || aDirectory == NULL || aName == NULL)
      return;			/* No input text or error in input parameters */

   if (text && ustrchr (text, TEXT('/')))
     URL_DIR_SEP = TEXT('/');
   else 
     URL_DIR_SEP = DIR_SEP;
   
   aDirectory[0] = EOS;
   aName[0] = EOS;
   lg = ustrlen (text);
   if (lg)
     {
       /* the text is not empty */
       ptr = oldptr = &text[0];
       do
	 {
	   ptr = ustrrchr (oldptr, URL_DIR_SEP);
	   if (ptr != NULL)
	     oldptr = &ptr[1];
	 }
       while (ptr != NULL);
       
       i = ((int) (oldptr) - (int) (text)) / sizeof (CHAR_T);	/* the length of the directory part */
       if (i > 1)
	 {
	   ustrncpy (aDirectory, text, i);
	   j = i - 1;
	   /* Suppresses the / characters at the end of the path */
	   while (aDirectory[j] == URL_DIR_SEP)
	     aDirectory[j--] = EOS;
	 }
       if (i != lg)
	 ustrcpy (aName, oldptr);
     }
#    ifdef _WINDOWS
     lg = ustrlen (aName);
     if (!ustrcasecmp (&aName[lg - 4], EXE_EXT))
        aName[lg - 4] = EOS;
#    endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   MakeCompleteName compose un nom de fichier absolu en concatenant 
   un nom de directory, le nom de fichier (fname) et l'extension (fext).
   Retourne le nom compose' dans completeName.                   
   Si fname se termine deja par fext, alors copie simplement fname dans
   completeName.
   La chaine directory_list peut contenir un path
   hierarchique ou` les noms de repertoires sont classe's
   par ordre d'importance, et separes par PATH_SEP
   Si le fichier existe, on retourne dans directory_list le nom du directory
   qui le contient, sinon directory_list n'est pas change'.       
   Si le fichier n'existe pas, on retourne completeName vide et
   dans directory_list le 1er nom du path fourni a` l'appel
   (MakeCompleteName est utilise pour la lecture)          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MakeCompleteName (char* fname, char* fext, char* directory_list, char* completeName, int *length)
#else  /* __STDC__ */
void                MakeCompleteName (fname, fext, directory_list, completeName, length)
char*               fname;
char*               fext;
char*               directory_list;
char*               completeName;
int*                length;

#endif /* __STDC__ */
{
   int                 i, j;
   PathBuffer          single_directory;
   PathBuffer          first_directory;
   ThotBool            found;

   found = FALSE;
   i = 1;
   first_directory[0] = EOS;
   while (directory_list[i - 1] != EOS && (!found))
     {
	j = 1;
	while (directory_list[i - 1] != PATH_SEP
	       && directory_list[i - 1] != EOS
	       && j < MAX_PATH
	       && i < MAX_PATH)
	  {
	     /* on decoupe la liste en directory individuels */
	     single_directory[j - 1] = directory_list[i - 1];
	     i++;
	     j++;
	  }
	/* on ajoute une fin de chaine */
	single_directory[j - 1] = EOS;
	/* on sauve ce nom de directory si c'est le 1er */
	if (first_directory[0] == EOS)
	   ustrncpy (first_directory, single_directory, MAX_PATH);
	/* on construit le nom */
	FindCompleteName (fname, fext, single_directory, completeName, length);
	if (TtaFileExist (completeName))
	  {
	     found = TRUE;
	     ustrncpy (directory_list, single_directory, MAX_PATH);
	  }
	else
	   /* on essaie avec un autre directory en sautant le PATH_SEP */
	if (directory_list[i - 1] == PATH_SEP)
	   i++;
     }
   if (!found)
     {
	completeName[0] = EOS;
	if (first_directory[0] != EOS)
	   ustrncpy (directory_list, first_directory, MAX_PATH);
     }
}

/*----------------------------------------------------------------------
   GetPictureFileName construit dans fileName le nom absolu d'un   
   fichier image a` partir du nom contenu dans name et     
   des repertoires de documents ou de sche'mas.            
   Si le fichier n'existe pas retourne name.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetPictureFileName (STRING name, STRING fileName)
#else  /* __STDC__ */
void                GetPictureFileName (name, fileName)
STRING              name;
STRING              fileName;

#endif /* __STDC__ */
{
   int                 length;
   PathBuffer          directory;
   CHAR_T                URL_DIR_SEP;

   if (name && ustrchr (name, TEXT('/')))
     URL_DIR_SEP = TEXT('/');
   else 
     URL_DIR_SEP = DIR_SEP;

   /* Recherche le fichier dans les repertoires de documents */
   if (name[0] == URL_DIR_SEP || name [1] == TEXT(':'))
     ustrcpy (fileName, name);
   else
     {
       ustrcpy (directory, DocumentPath);
       MakeCompleteName (name, _EMPTYSTR_, directory, fileName, &length);
       if (!TtaFileExist (fileName))
	 {
	   /* Recherche le fichier dans les repertoires de schemas */
	   ustrcpy (directory, SchemaPath);
	   MakeCompleteName (name, _EMPTYSTR_, directory, fileName, &length);
	 }
     }
}

/*----------------------------------------------------------------------
   IsExtended compare la fin de fileName avec extension. Si la fin 
   est identique, retourne Vrai.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsExtended (STRING fileName, STRING extension)
#else  /* __STDC__ */
static ThotBool     IsExtended (fileName, extension)
STRING              fileName;
STRING              extension;

#endif /* __STDC__ */
{
   int                 i, j;
   int                 nameLength, extLength;
   ThotBool            ok;

   nameLength = 0;
   extLength = 0;

   /* on mesure extension */
   extLength = ustrlen (extension);
   /* on mesure fileName */
   nameLength = ustrlen (fileName);
   if (nameLength >= THOT_MAX_CHAR)
      ok = FALSE;
   else if (extLength > 0 && nameLength > extLength)
     {
	ok = TRUE;
	j = nameLength - 1;
	for (i = extLength - 1; i >= 0; i--)
	  {
	     ok = (extension[i] == fileName[j]) && ok;
	     j--;
	  }
	ok = ok && (fileName[j] == TEXT('.'));
     }
   else
      ok = FALSE;
   return ok;
}


/*----------------------------------------------------------------------
   FindCompleteName compose un nom de fichier absolu en concatenant 
   le nom de directory, le nom de fichier (fileName) et    
   l'extension (extension).                                
   Retourne le nom compose dans completeName et la         
   longueur de ce nom dans length.                         
   Si fileName se termine deja par extension, alors copie  
   simplement fileName dans completeName.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FindCompleteName (STRING fileName, STRING extension, PathBuffer directory, PathBuffer completeName, int *length)
#else  /* __STDC__ */
void                FindCompleteName (fileName, extension, directory, completeName, length)
STRING              fileName;
STRING              extension;
PathBuffer          directory;
PathBuffer          completeName;
int                *length;

#endif /* __STDC__ */
{
   int                 i, j, k, h = 0;
   STRING              home_dir = NULL;

   /* on recopie le repertoire */
   i = ustrlen (directory);
   j = ustrlen (fileName);

   /* check for tilde indicating the HOME directory */
   if (directory[0] == TEXT('~'))
     {
#   ifdef _WINDOWS
    home_dir = NULL;
#   else  /* !_WINDOWS */
	home_dir = TtaGetEnvString ("HOME");
#   endif /* _WINDOWS */

	if (home_dir != NULL)
	  {
	    /* tilde will not be copied */
	    i--;
	    h = ustrlen (home_dir);
	  }
     }
   if (i > 1)
      /* for the added DIR_STR */
      i++;

   /* si on cherche a ouvrir un fichier pivot et que le nom de fichier se
      termine par ".piv", on remplace ce suffixe par ".PIV" */
   if (ustrcmp (extension, PIV_EXT2) == 0)
     {
	if (j > 4)
	   if (fileName[j - 4] == TEXT('.'))
	      if (fileName[j - 3] == TEXT('p'))
		 if (fileName[j - 2] == TEXT('i'))
		    if (fileName[j - 1] == TEXT('v'))
		      {
			 fileName[j - 3] = TEXT('P');
			 fileName[j - 2] = TEXT('I');
			 fileName[j - 1] = TEXT('V');
		      }
     }
   if (!IsExtended (fileName, extension) && extension[0] != EOS)
      k = ustrlen (extension) + 1;	/* dont forget the '.' */
   else
      k = 0;
   if (i + j + k + h >= MAX_PATH)
      return;

   completeName[0] = EOS;
   if (home_dir)
     {
       ustrcat (completeName, home_dir);
       ustrcat (completeName, &directory[1]);
     }
   else
     ustrcat (completeName, directory);

   /* on ajoute un DIR_STR */
   if (i >= 1)
     ustrcat (completeName, DIR_STR);

   /* on recopie le nom */
   ustrcat (completeName, fileName);
   if (k != 0)
     {
	/* on ajoute l'extension */
	ustrcat (completeName, TEXT("."));
	ustrcat (completeName, extension);
     }
   /* on termine la chaine */
   *length = i + j + k + h;
}


/*----------------------------------------------------------------------
   GetDocIdent                                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetDocIdent (DocumentIdentifier *Ident, Name docName)
#else  /* __STDC__ */
void                GetDocIdent (Ident, docName)
DocumentIdentifier *Ident;
Name                docName;

#endif /* __STDC__ */

{
   ustrncpy (*Ident, docName, MAX_DOC_IDENT_LEN);
   *Ident[MAX_DOC_IDENT_LEN - 1] = EOS;
}

/*----------------------------------------------------------------------
   GetDocName                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetDocName (DocumentIdentifier Ident, Name docName)
#else  /* __STDC__ */
void                GetDocName (Ident, docName)
DocumentIdentifier  Ident;
Name                docName;

#endif /* __STDC__ */
{
   ustrncpy (docName, Ident, MAX_NAME_LENGTH);
   docName[MAX_NAME_LENGTH - 1] = EOS;
}


/*----------------------------------------------------------------------
   FileWriteAccess	returns the write access right for a file	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 FileWriteAccess (STRING fileName)
#else  /* __STDC__ */
int                 FileWriteAccess (fileName)
STRING              fileName;

#endif /* __STDC__ */
{
   int                 ret, i;
   CHAR_T                c;
   CHAR_T                URL_DIR_SEP;

   if (fileName && ustrchr (fileName, TEXT('/')))
	  URL_DIR_SEP = TEXT('/');
   else 
	   URL_DIR_SEP = DIR_SEP;

#  ifdef _WINDOWS
   ret = uaccess (fileName, 0);
#  else  /* _WINDOWS */
   ret = uaccess (fileName, F_OK);
#  endif /* _WINDOWS */
   if (ret == -1)
      /* file does not exist */
     {
	/* check its directory */
	i = ustrlen (fileName);
	while ((i >= 0) && (fileName[i] != URL_DIR_SEP))
	   i--;
	if (i < 0)
	   /* no directory name: current directory */
	   ret = access (".", R_OK | W_OK | X_OK);
	else
	  {
	     /* isolate the directory name */
	     c = fileName[i];
	     fileName[i] = EOS;
	     /* get access right for the directory */
	     ret = uaccess (fileName, R_OK | W_OK | X_OK);
	     fileName[i] = c;
	  }
     }
   else
      /* file exists */
      ret = uaccess (fileName, W_OK);
   return (ret);
}

/*----------------------------------------------------------------------
   	GetCounterValue	converts the value number into a character	
   	string according to a given style (arabic, roman, letter).	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetCounterValue (int number, CounterStyle style, STRING string, int *len)
#else  /* __STDC__ */
void                GetCounterValue (number, style, string, len)
int                 number;
CounterStyle        style;
STRING              string;
int                *len;

#endif /* __STDC__ */
{
   int                 c, i, begin;

   *len = 0;
   if (number < 0)
     {
	string[(*len)++] = TEXT('-');
	number = -number;
     }

   switch (style)
	 {
	    case CntArabic:
	       if (number >= 100000)
		 {
		    string[(*len)++] = TEXT('?');
		    number = number % 100000;
		 }
	       if (number >= 10000)
		  c = 5;
	       else if (number >= 1000)
		  c = 4;
	       else if (number >= 100)
		  c = 3;
	       else if (number >= 10)
		  c = 2;
	       else
		  c = 1;
	       *len += c;
	       i = *len;
	       do
		 {
		    string[i - 1] = (CHAR_T) ((int) (TEXT('0')) + number % 10);
		    i--;
		    number = number / 10;
		 }
	       while (number > 0);
	       break;

	    case CntURoman:
	    case CntLRoman:
	       if (number >= 4000)
		  string[(*len)++] = TEXT('?');
	       else
		 {
		    begin = *len + 1;
		    while (number >= 1000)
		      {
			 string[(*len)++] = TEXT('M');
			 number -= 1000;
		      }
		    if (number >= 900)
		      {
			 string[(*len)++] = TEXT('C');
			 string[(*len)++] = TEXT('M');
			 number -= 900;
		      }
		    else if (number >= 500)
		      {
			 string[(*len)++] = TEXT('D');
			 number -= 500;
		      }
		    else if (number >= 400)
		      {
			 string[(*len)++] = TEXT('C');
			 string[(*len)++] = TEXT('D');
			 number -= 400;
		      }
		    while (number >= 100)
		      {
			 string[(*len)++] = TEXT('C');
			 number -= 100;
		      }
		    if (number >= 90)
		      {
			 string[(*len)++] = TEXT('X');
			 string[(*len)++] = TEXT('C');
			 number -= 90;
		      }
		    else if (number >= 50)
		      {
			 string[(*len)++] = TEXT('L');
			 number -= 50;
		      }
		    else if (number >= 40)
		      {
			 string[(*len)++] = TEXT('X');
			 string[(*len)++] = TEXT('L');
			 number -= 40;
		      }
		    while (number >= 10)
		      {
			 string[(*len)++] = TEXT('X');
			 number -= 10;
		      }
		    if (number >= 9)
		      {
			 string[(*len)++] = TEXT('I');
			 string[(*len)++] = TEXT('X');
			 number -= 9;
		      }
		    else if (number >= 5)
		      {
			 string[(*len)++] = TEXT('V');
			 number -= 5;
		      }
		    else if (number >= 4)
		      {
			 string[(*len)++] = TEXT('I');
			 string[(*len)++] = TEXT('V');
			 number -= 4;
		      }
		    while (number >= 1)
		      {
			 string[(*len)++] = TEXT('I');
			 number--;
		      }
		    if (style == CntLRoman)
		       /* UPPERCASE --> lowercase */
		       for (i = begin; i <= *len; i++)
			  if (string[i - 1] != TEXT('?'))
			     string[i - 1] = (CHAR_T) ((int) (string[i - 1]) + 32);
		 }
	       break;

	    case CntUppercase:
	    case CntLowercase:
	       if (number > 475354)
		 {
		  string[(*len)++] = TEXT('?');
		  number = number % 475254;
		 }
	       if (number > 18278)
		 c = 4;
	       else if (number > 702)
		 c = 3;
	       else if (number > 26)
		 c = 2;
	       else
		 c = 1;
	       *len += c;
	       i = *len;
	       do
		 {
	          number --;
	          if (style == CntUppercase)
		     string[i - 1] = (CHAR_T) ((number % 26) + (int) (TEXT('A')));
	          else
		     string[i - 1] = (CHAR_T) ((number % 26) + (int) (TEXT('a')));
		  i --;
		  c --;
		  number = number / 26;
		 }
	       while (c > 0);
	       break;

	    default:
	       break;
	 }
   string[*len] = EOS;
}

/*----------------------------------------------------------------------
   TtaMakeDirectory

   Platform independent call to the local mkdir function

   Parameter:
   directory: the directory name.
   Return value:
   TRUE if the directory could be created or if it existed already,
   FALSE otherwise.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaMakeDirectory (STRING directory)

#else  /* __STDC__ */
ThotBool            TtaMakeDirectory (directory)
STRING              directory;

#endif /* __STDC__ */

{
  int i;

  if (TtaCheckDirectory (directory))
    return TRUE;

#ifdef _WINDOWS
  i = umkdir (directory);
#else /* _WINDOWS */
  i = umkdir (directory, S_IRWXU);
#endif /* _WINDOWS */
  if (i != 0 && errno != EEXIST)
    return FALSE;
  else
    return TRUE;
}

/*----------------------------------------------------------------------
   TtaCheckDirectory

   Ckecks that a directory exists and can be accessed.

   Parameter:
   directory: the directory name.
   Return value:
   TRUE if the directory is OK, FALSE if not.
	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaCheckDirectory (char* directory)

#else  /* __STDC__ */
ThotBool            TtaCheckDirectory (directory)
char*               directory;

#endif /* __STDC__ */

{
#ifdef _WINDOWS
   DWORD               attribs;

   /* NEW_WINDOWS - mark for furthur security stuff - EGP
      SECURITY_INFORMATION secInfo;
      SECURITY_DESCRIPTOR secDesc; */
   attribs = GetFileAttributes (directory);
   if (attribs == 0xFFFFFFFF)
      return FALSE;
   else if (!(attribs & FILE_ATTRIBUTE_DIRECTORY))
      return FALSE;
   return TRUE;
#else  /* _WINDOWS */
   struct stat         fileStat;

   /* does the directory exist ? */
   if (ustrlen (directory) < 1)
      return (FALSE);
   else if (stat (directory, &fileStat) != 0)
      return (FALSE);
   else if (S_ISDIR (fileStat.st_mode))
      return (TRUE);
   else
      return (FALSE);
#endif /* !_WINDOWS */
}



