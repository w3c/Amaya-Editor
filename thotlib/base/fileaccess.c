/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 *  Input/output for Thot binary files (schemas and pivot files)
 *
 * Authors: V. Quint (INRIA)
 *          D. Veillard (INRIA) - new functions for MS-Windows
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "fileaccess.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"

#include "fileaccess_f.h"
#include "platform_f.h"
#include "registry_f.h"
#ifdef _WINDOWS
#include "winsys.h"
#endif /* _WINDOWS */

extern unsigned long offset[6] ;
/* = {
         0x00000000UL,
         0x00003080UL,
         0x000E2080UL,
         0x03C82080UL,
         0xFA082080UL,
         0x82082080UL
};*/
extern CHARSET CharEncoding;

#define LMASK           0x000000ffL
#define DECAL_3         (3 * 8)
#define DECAL_2         (2 * 8)
#define DECAL_1         (1 * 8)
#define SIGNED_SHORT_MASK	0xffff0000L

/*----------------------------------------------------------------------
   TtaReadByte reads a character (or byte) value.
  ----------------------------------------------------------------------*/
ThotBool TtaReadByte (BinFile file, unsigned char *bval)
{
  unsigned char v;

  if (fread (&v, sizeof (unsigned char), 1, file) == 0)
    {
      *bval = (char) 0;
      return FALSE;
    } 
  *bval = (char) v;
  return TRUE;
}
/*----------------------------------------------------------------------
   TtaRead4Byte reads a 4 characters (or bytes) value.
  ----------------------------------------------------------------------*/
ThotBool TtaRead4Byte (BinFile file, unsigned char bval[4])
{
  if (fread (bval, 4, 1, file) == 0)
    {
      return FALSE;
    } 
  return TRUE;
}

/*----------------------------------------------------------------------
   TtaReadWideChar reads a wide character value.
  ----------------------------------------------------------------------*/
ThotBool TtaReadWideChar (BinFile file, CHAR_T *bval)
{
#ifdef _I18N_
  int           nbBytesToRead, i;
  unsigned char car;
  wchar_t       res;

  if (TtaReadByte (file, &car) == 0)
    {
      *bval = (char) 0;
      return (FALSE);
    } 

  if (car < 0xC0)
    nbBytesToRead = 1;
  else if (car < 0xE0)
    nbBytesToRead = 2;
  else if (car < 0xF0)
    nbBytesToRead = 3;
  else if (car < 0xF8)
    nbBytesToRead = 4;
  else if (car < 0xFC)
    nbBytesToRead = 5;
  else if (car <= 0xFF)
    nbBytesToRead = 6;

  res = 0;
  /* See how many bytes to read to build a wide character */
  switch (nbBytesToRead)
    {        /** WARNING: There is not break statement between cases */
    case 6:
      res += car;
      res <<= 6;
      TtaReadByte (file, &car);
    case 5:
      res += car;
      res <<= 6;
      TtaReadByte (file, &car);
    case 4: res += car;
      res <<= 6;
      TtaReadByte (file, &car);
    case 3:
      res += car;
      res <<= 6;
      TtaReadByte (file, &car);
    case 2:
      res += car;
      res <<= 6;
      TtaReadByte (file, &car);
    case 1: res += car;
    }

  /* Leading bits in each byte are not masked or checked, but
     accounted for by subtracting the appropriate offset value.
     This assumes that the input is correct :-(. */
   i = offset[nbBytesToRead - 1];
  res -= (wchar_t) i;
  if (res <= 0xFFFF)
    *bval = res;
  else 
    *bval = '?';    
  return (TRUE);
#else  /* _I18N_ */
  return TtaReadByte (file, bval);
#endif /* _I18N_ */
}

/*----------------------------------------------------------------------
   TtaReadBool reads a ThotBool value.
  ----------------------------------------------------------------------*/
ThotBool TtaReadBool (BinFile file, ThotBool *bval)
{
   char       b1;

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
ThotBool TtaReadShort (BinFile file, int *sval)
{
  char      car;
 
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
ThotBool TtaReadSignedShort (BinFile file, int *sval)
{
  unsigned char      car;
 
  *sval = 0;
  if (!TtaReadByte (file, &car))
    return (FALSE);
  else
    {
     if ((int) car > 127 )
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
ThotBool TtaReadInteger (BinFile file, int *sval)
{
  char      car;
 
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

union ieee754_float {
  float f;
  /* This is the IEEE 754 float-precision format.  */
  struct {
#ifdef _WINDOWS
	unsigned int negative:1;
    unsigned int exponent:8;
    unsigned int mantissa:23;
#else /*_WINDOWS*/
#if defined(i386) || defined(__i386)
#if     __BYTE_ORDER == __BIG_ENDIAN
    unsigned int negative:1;
    unsigned int exponent:8;
    unsigned int mantissa:23;
#endif
#if     __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int mantissa:23;
    unsigned int exponent:8;
    unsigned int negative:1;
#endif
#else
#if defined(_BIG_ENDIAN)
    unsigned int negative:1;
    unsigned int exponent:8;
    unsigned int mantissa:23;
#endif
#if defined(_LITTLE_ENDIAN)
    unsigned int mantissa:23;
    unsigned int exponent:8;
    unsigned int negative:1;
#endif
#endif
#endif /*_WINDOWS*/
  } ieee;
};

float readFloat(unsigned int *bytes) 
{
  union ieee754_float f;

  f.ieee.negative = ((*bytes >> 31) != 0);
  f.ieee.exponent = (*bytes >> 23) & 0xff;
  f.ieee.mantissa = (f.ieee.exponent == 0)? (*bytes & 0x7fffff) << 1 : (*bytes & 0x7fffff) | 0x800000;
  return f.f;
}
/*----------------------------------------------------------------------
  TtaReadInteger reads an integer.
  ----------------------------------------------------------------------*/
ThotBool TtaReadFloat (BinFile file, float *sval)
{
  char      car;
  long      val;
  unsigned char test[4];

  *sval = 0;
  val = 0;

  if (!TtaRead4Byte (file, test))
    return (FALSE);
  *sval = readFloat ((unsigned int *) test);
  return (TRUE);


  if (!TtaReadByte (file, &car))
    return (FALSE);
  else
    {
      val = car;
      if (!TtaReadByte (file, &car))
	return (FALSE);
      else
	{
	  val = (val << 8) + car;
	  if (!TtaReadByte (file, &car))
	    return (FALSE);
	  else
	    {
	      val = (val << 8) + car;
	      if (!TtaReadByte (file, &car))
		{
		  return (FALSE);
		}
	      else
		{
		  val = (val << 8) + car;
		  *sval = (float) val;
		  return (TRUE);
		}
	    }
	}
    }
}

/*----------------------------------------------------------------------
   TtaReadName reads a Wide Character string value.
  ----------------------------------------------------------------------*/
ThotBool TtaReadName (BinFile file, char *name)
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
BinFile TtaReadOpen (CONST char *filename)
{
#ifdef _WINDOWS 
   char   *mode = "rb";
#else /* _WINDOWS */
   char   *mode = "r";
#endif /* _WINDOWS */

   if (filename && filename[0] != EOS)
     return fopen (filename, mode);
   else
     return (BinFile) NULL;
}


/*----------------------------------------------------------------------
   TtaReadClose closes a file.
  ----------------------------------------------------------------------*/
void TtaReadClose (BinFile file)
{
  if (file)
    fclose (file);
}


/*----------------------------------------------------------------------
   TtaWriteOpen opens a file for writing.
  ----------------------------------------------------------------------*/
BinFile TtaWriteOpen (CONST char *filename)
{
#ifdef _WINDOWS
  return fopen (filename, "wb+");
#else /* _WINDOWS */
  return fopen (filename, "w+");
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   TtaWriteClose closes a file.
  ----------------------------------------------------------------------*/
void TtaWriteClose (BinFile file)
{
  if (file)
    fclose (file);
}

/*----------------------------------------------------------------------
   TtaWriteByte writes a character (or byte) value.
  ----------------------------------------------------------------------*/
ThotBool TtaWriteByte (BinFile file, char bval)
{
  if (fwrite ((char *) &bval, sizeof (char), 1, file) == 0)
    return FALSE;
  else
    return TRUE;
}
/*----------------------------------------------------------------------
   TtaWrite4Byte writes a 4 characters (or bytes) value.
  ----------------------------------------------------------------------*/
ThotBool TtaWrite4Byte (BinFile file, unsigned char bval[4])
{
  if (fwrite ((char *) bval, 4, 1, file) == 0)
    return FALSE;
  else
    return TRUE;
}

/*----------------------------------------------------------------------
   TtaWriteWideChar writes a wide character value.
  ----------------------------------------------------------------------*/
ThotBool TtaWriteWideChar (BinFile file, CHAR_T val)
{
#ifdef _I18N_
   unsigned char mbc[MAX_BYTES + 1], *ptr;
   int           nbBytes;

   mbc[0] = EOS;
   ptr = mbc;
   nbBytes = TtaWCToMBstring (val, &ptr);
   if (nbBytes == -1)
      return FALSE;
   if (fwrite ((char *) mbc, sizeof (char), nbBytes, file) == 0)
      return FALSE;
   return TRUE;
#else  /* !_I18N_ */
   if (fwrite ((char *) &val, sizeof (char), 1, file) == 0)
      return FALSE;
   return TRUE;
#endif /* !_I18N_ */
}

/*----------------------------------------------------------------------
   TtaWriteShort reads an unsigned short value.
   -------------------------------------------------------------------- */
ThotBool TtaWriteShort (BinFile file, int sval)
{
  if (!TtaWriteByte (file, (char) ((sval >> DECAL_1) & LMASK)))
    return FALSE;
  if (!TtaWriteByte (file, (char) (sval & LMASK)))
    return FALSE;
   return TRUE;
}

/*----------------------------------------------------------------------
   TtaWriteInteger writes an integer.
   -------------------------------------------------------------------- */
ThotBool TtaWriteInteger (BinFile file, int lval)
{
   if (!TtaWriteByte (file, (char) ((lval >> DECAL_3) & LMASK)))
      return FALSE;
   if (!TtaWriteByte (file, (char) ((lval >> DECAL_2) & LMASK)))
      return FALSE;
   if (!TtaWriteByte (file, (char) ((lval >> DECAL_1) & LMASK)))
      return FALSE;
   if (!TtaWriteByte (file, (char) (lval & LMASK)))
      return FALSE;
   return TRUE;
}


/*----------------------------------------------------------------------
   TtaWriteDocIdent writes a document identifier.
  ----------------------------------------------------------------------*/
void TtaWriteDocIdent (BinFile file, DocumentIdentifier Ident)
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
void TtaReadDocIdent (BinFile file, DocumentIdentifier *Ident)
{
   int j = 0;
   
   do
   if (!TtaReadByte (file, &((*Ident)[j++])))
     (*Ident)[j - 1] = EOS;
   while (!(j >= MAX_DOC_IDENT_LEN || (*Ident)[j - 1] == EOS)) ;
}

/*----------------------------------------------------------------------
   CopyDocIdent
  ----------------------------------------------------------------------*/
void CopyDocIdent (DocumentIdentifier *Dest, DocumentIdentifier Source)
{
   strncpy (*Dest, Source, MAX_DOC_IDENT_LEN);
}

/*----------------------------------------------------------------------
   SameDocIdent
  ----------------------------------------------------------------------*/
ThotBool SameDocIdent (DocumentIdentifier Ident1, DocumentIdentifier Ident2)
{
   ThotBool            ret;

   ret = (strcmp (Ident1, Ident2) == 0);
   return ret;
}

/*----------------------------------------------------------------------
   ClearDocIdent     
  ----------------------------------------------------------------------*/
void ClearDocIdent (DocumentIdentifier *Ident)
{
   (*Ident)[0] = EOS;
}

/*----------------------------------------------------------------------
   DocIdentIsNull   
  ----------------------------------------------------------------------*/
ThotBool DocIdentIsNull (DocumentIdentifier Ident)
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
void TtaExtractName (char *text, char *aDirectory, char *aName)
{
   int                lg, i, j;
   char              *ptr;
   char              *oldptr;
   char               URL_DIR_SEP;

   if (text == NULL || aDirectory == NULL || aName == NULL)
     /* No input text or error in input parameters */
      return;
   if (text && strchr (text, '/'))
     URL_DIR_SEP = '/';
   else 
     URL_DIR_SEP = DIR_SEP;
   
   aDirectory[0] = EOS;
   aName[0] = EOS;
   lg = strlen (text);
   if (lg)
     {
       /* the text is not empty */
       ptr = oldptr = &text[0];
       do
	 {
	   ptr = strrchr (oldptr, URL_DIR_SEP);
	   if (ptr != NULL)
	     oldptr = &ptr[1];
	 }
       while (ptr != NULL);
       /* the length of the directory part */
       i = ((int) (oldptr) - (int) (text)) / sizeof (char);
       if (i > 1)
	 {
	   strncpy (aDirectory, text, i);
	   j = i - 1;
	   /* Suppresses the / characters at the end of the path */
	   while (aDirectory[j] == URL_DIR_SEP)
	     aDirectory[j--] = EOS;
	 }
       if (i != lg)
          strcpy (aName, oldptr);
     }
#ifdef _WINDOWS
     lg = strlen (aName);
     if (!strcasecmp (&aName[lg - 4], ".exe"))
        aName[lg - 4] = EOS;
#endif /* _WINDOWS */
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
void MakeCompleteName (char *fname, char *fext, char *directory_list,
		       char *completeName, int *length)
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
	while (directory_list[i - 1] != PATH_SEP && 
           directory_list[i - 1] != EOS      && 
           j < MAX_PATH                         && 
           i < MAX_PATH)
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
	   strncpy (first_directory, single_directory, MAX_PATH);
	/* on construit le nom */
	FindCompleteName (fname, fext, single_directory, completeName, length);
	if (TtaFileExist (completeName))
	  {
	     found = TRUE;
	     strncpy (directory_list, single_directory, MAX_PATH);
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
	   strncpy (directory_list, first_directory, MAX_PATH);
     }
}

/*----------------------------------------------------------------------
   GetPictureFileName construit dans fileName le nom absolu d'un   
   fichier image a` partir du nom contenu dans name et     
   des repertoires de documents ou de sche'mas.            
   Si le fichier n'existe pas retourne name.               
  ----------------------------------------------------------------------*/
void GetPictureFileName (char *name, char *fileName)
{
   int                 length;
   PathBuffer          directory;
   char                URL_DIR_SEP;

   if (name && strchr (name, '/'))
     URL_DIR_SEP = '/';
   else 
     URL_DIR_SEP = DIR_SEP;

   /* Recherche le fichier dans les repertoires de documents */
   if (name[0] == URL_DIR_SEP || name [1] == ':')
     strcpy (fileName, name);
   else
     {
       strcpy (directory, DocumentPath);
       MakeCompleteName (name, "", directory, fileName, &length);
       if (!TtaFileExist (fileName))
	 {
	   /* Recherche le fichier dans les repertoires de schemas */
	   strcpy (directory, SchemaPath);
	   MakeCompleteName (name, "", directory, fileName, &length);
	 }
     }
}

/*----------------------------------------------------------------------
   IsExtended compare la fin de fileName avec extension. Si la fin 
   est identique, retourne Vrai.                           
  ----------------------------------------------------------------------*/
static ThotBool IsExtended (char *fileName, char *extension)
{
   int                 i, j;
   int                 nameLength, extLength;
   ThotBool            ok;

   nameLength = 0;
   extLength = 0;

   /* on mesure extension */
   extLength = strlen (extension);
   /* on mesure fileName */
   nameLength = strlen (fileName);
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
	ok = ok && (fileName[j] == '.');
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
void FindCompleteName (char *fileName, char *extension,
		       PathBuffer directory, PathBuffer completeName,
		       int *length)
{
  int              i, j, k, h = 0;
  char            *home_dir = NULL;

  /* on recopie le repertoire */
  i = strlen (directory);
  j = strlen (fileName);
  /* check for tilde indicating the HOME directory */
  if (directory[0] == '~')
    {
#ifdef _WINDOWS
      home_dir = NULL;
#else  /* !_WINDOWS */
      home_dir = TtaGetEnvString ("HOME");
      if (home_dir != NULL)
	{
	  /* tilde will not be copied */
	  i--;
	  h = strlen (home_dir);
	}
#endif /* _WINDOWS */
    }
  if (i > 1)
    /* for the added DIR_STR */
    i++;

  /* si on cherche a ouvrir un fichier pivot et que le nom de fichier se
     termine par ".piv", on remplace ce suffixe par ".PIV" */
  if (strcmp (extension, "PIV") == 0)
    {
      if (j > 4 &&
	  fileName[j - 4] == '.' && fileName[j - 3] == 'p' &&
	  fileName[j - 2] == 'i' && fileName[j - 1] == 'v')
	{
	  fileName[j - 3] = 'P';
	  fileName[j - 2] = 'I';
	  fileName[j - 1] = 'V';
	}
    }
  if (!IsExtended (fileName, extension) && extension[0] != EOS)
    k = strlen (extension) + 1;	/* dont forget the '.' */
  else
    k = 0;
  if (i + j + k + h >= MAX_PATH)
    return;
  completeName[0] = EOS;
  if (home_dir)
    {
      strcat (completeName, home_dir);
      strcat (completeName, &directory[1]);
    }
  else
    strcat (completeName, directory);
  
  /* on ajoute un DIR_STR */
  if (i >= 1)
    strcat (completeName, DIR_STR);
  /* on recopie le nom */
  strcat (completeName, fileName);
  if (k != 0)
    {
      /* on ajoute l'extension */
      strcat (completeName, ".");
      strcat (completeName, extension);
    }
  /* on termine la chaine */
  *length = i + j + k + h;
}


/*----------------------------------------------------------------------
   GetDocIdent  
  ----------------------------------------------------------------------*/
void GetDocIdent (DocumentIdentifier* Ident, char *docName)
{
   strncpy (*Ident, docName, MAX_DOC_IDENT_LEN);
   *Ident[MAX_DOC_IDENT_LEN - 1] = EOS;
}

/*----------------------------------------------------------------------
   GetDocName                                                      
  ----------------------------------------------------------------------*/
void GetDocName (DocumentIdentifier Ident, char *docName)
{
   strncpy (docName, Ident, MAX_NAME_LENGTH);
   docName[MAX_NAME_LENGTH - 1] = EOS;
}


/*----------------------------------------------------------------------
   FileWriteAccess	returns the write access right for a file	
  ----------------------------------------------------------------------*/
int FileWriteAccess (char *fileName)
{
   int                 ret, i;
   char                c;
   char                URL_DIR_SEP;

   if (fileName && strchr (fileName, '/'))
     URL_DIR_SEP = '/';
   else 
     URL_DIR_SEP = DIR_SEP;
#ifdef _WINDOWS
   ret = access (fileName, 0);
#else  /* _WINDOWS */
   ret = access (fileName, F_OK);
#endif /* _WINDOWS */
   if (ret == -1)
      /* file does not exist */
     {
	/* check its directory */
	i = strlen (fileName);
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
	     ret = access (fileName, R_OK | W_OK | X_OK);
	     fileName[i] = c;
	  }
     }
   else
      /* file exists */
      ret = access (fileName, W_OK);
   return (ret);
}

/*----------------------------------------------------------------------
   	GetCounterValue	converts the value number into a character	
   	string according to a given style (arabic, roman, letter).	
  ----------------------------------------------------------------------*/
void GetCounterValue (int number, CounterStyle style, char *string, int *len)
{
   int                 c, i, begin;

   *len = 0;
   if (number < 0)
     {
	string[(*len)++] = '-';
	number = -number;
     }
   switch (style)
     {
     case CntArabic:
       if (number >= 100000)
	 {
	   string[(*len)++] = '?';
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
	   string[i - 1] = (char) ((int) ('0') + number % 10);
	   i--;
	   number = number / 10;
	 }
       while (number > 0);
       break;
     case CntURoman:
     case CntLRoman:
       if (number >= 4000)
	 string[(*len)++] = '?';
       else
	 {
	   begin = *len + 1;
	   while (number >= 1000)
	     {
	       string[(*len)++] = 'M';
	       number -= 1000;
	     }
	   if (number >= 900)
	     {
	       string[(*len)++] = 'C';
	       string[(*len)++] = 'M';
	       number -= 900;
	     }
	   else if (number >= 500)
	     {
	       string[(*len)++] = 'D';
	       number -= 500;
	     }
	   else if (number >= 400)
	     {
	       string[(*len)++] = 'C';
	       string[(*len)++] = 'D';
	       number -= 400;
	     }
	   while (number >= 100)
	     {
	       string[(*len)++] = 'C';
	       number -= 100;
	     }
	   if (number >= 90)
	     {
	       string[(*len)++] = 'X';
	       string[(*len)++] = 'C';
	       number -= 90;
	     }
	   else if (number >= 50)
	     {
	       string[(*len)++] = 'L';
	       number -= 50;
	     }
	   else if (number >= 40)
	     {
	       string[(*len)++] = 'X';
	       string[(*len)++] = 'L';
	       number -= 40;
	     }
	   while (number >= 10)
	     {
	       string[(*len)++] = 'X';
	       number -= 10;
	     }
	   if (number >= 9)
	     {
	       string[(*len)++] = 'I';
	       string[(*len)++] = 'X';
	       number -= 9;
	     }
	   else if (number >= 5)
	     {
	       string[(*len)++] = 'V';
	       number -= 5;
	     }
	   else if (number >= 4)
	     {
	       string[(*len)++] = 'I';
	       string[(*len)++] = 'V';
	       number -= 4;
	     }
	   while (number >= 1)
	     {
	       string[(*len)++] = 'I';
	       number--;
	     }
	   if (style == CntLRoman)
	     /* UPPERCASE --> lowercase */
	     for (i = begin; i <= *len; i++)
	       if (string[i - 1] != '?')
		 string[i - 1] = (char) ((int) (string[i - 1]) + 32);
	 }
       break;
     case CntUppercase:
     case CntLowercase:
       if (number > 475354)
	 {
	   string[(*len)++] = '?';
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
	     string[i - 1] = (char) ((number % 26) + (int) ('A'));
	   else
	     string[i - 1] = (char) ((number % 26) + (int) ('a'));
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
ThotBool TtaMakeDirectory (char *directory)
{
  int i;

  if (TtaCheckDirectory (directory))
    return TRUE;

#ifdef _WINDOWS
  i = _mkdir (directory);
#else /* _WINDOWS */
  i = mkdir (directory, S_IRWXU);
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
ThotBool TtaCheckDirectory (char *directory)
{
#ifdef _WINDOWS
   DWORD               attribs;

   /* NEW_WINDOWS - mark for furthur security stuff - EGP
      SECURITY_INFORMATION secInfo;
      SECURITY_DESCRIPTOR secDesc; */
   attribs = GetFileAttributes (directory);
   if (attribs == 0xFFFFFFFF)
      return FALSE;
   if (!(attribs & FILE_ATTRIBUTE_DIRECTORY))
      return FALSE;
   return TRUE;
#else  /* _WINDOWS */
   struct stat         fileStat;

   /* does the directory exist ? */
   if (strlen (directory) < 1)
      return (FALSE);
   else if (stat (directory, &fileStat) != 0)
      return (FALSE);
   else if (S_ISDIR (fileStat.st_mode))
      return (TRUE);
   else
      return (FALSE);
#endif /* _WINDOWS */
}

















