/*
   Input/output for Thot binary files (schemas and pivot files)
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"

#include "fileaccess_f.h"

/* ---------------------------------------------------------------------- */
/* |    BIOreadByte reads a character (or byte) value.                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             BIOreadByte (BinFile file, char *bval)
#else  /* __STDC__ */
boolean             BIOreadByte (file, bval)
BinFile             file;
char               *bval;

#endif /* __STDC__ */
{
   *bval = getc (file);
   if (feof (file) || ferror (file))
     {
	*bval = '\0';
	return FALSE;
     }
   return TRUE;
}

/* ---------------------------------------------------------------------- */
/* |    BIOreadBool reads a boolean value.                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             BIOreadBool (BinFile file, boolean * bval)
#else  /* __STDC__ */
boolean             BIOreadBool (file, bval)
BinFile             file;
boolean            *bval;

#endif /* __STDC__ */
{
   unsigned char       b1;

   if (!BIOreadByte (file, &b1))
     {
	*bval = FALSE;
	return FALSE;
     }
   *bval = (b1 == 1);
   return TRUE;
}


/* ---------------------------------------------------------------------- */
/* |    BIOreadShort reads an unsigned short value.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             BIOreadShort (BinFile file, int *sval)
#else  /* __STDC__ */
boolean             BIOreadShort (file, sval)
BinFile             file;
int                *sval;

#endif /* __STDC__ */
{
   unsigned char                b1, b2;

   if (!BIOreadByte (file, &b1))
     {
	*sval = 0;
	return FALSE;
     }
   if (!BIOreadByte (file, &b2))
     {
	*sval = 0;
	return FALSE;
     }
   *sval = 256 * ((int) b1) + ((int) b2);
   return TRUE;
}


/* ---------------------------------------------------------------------- */
/* |    BIOreadSignedShort reads a signed short value.                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             BIOreadSignedShort (BinFile file, int *sval)
#else  /* __STDC__ */
boolean             BIOreadSignedShort (file, sval)
BinFile             file;
int                *sval;

#endif /* __STDC__ */
{
   unsigned char       b1, b2;

   if (!BIOreadByte (file, &b1))
     {
	*sval = 0;
	return FALSE;
     }
   if (!BIOreadByte (file, &b2))
     {
	*sval = 0;
	return FALSE;
     }
   *sval = 256 * ((int)b1) + ((int) b2);
   if (*sval > 32767)
      *sval = *sval - 65536;
   return TRUE;
}


/* ---------------------------------------------------------------------- */
/* |    BIOreadInteger reads an integer.                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             BIOreadInteger (BinFile file, int *sval)
#else  /* __STDC__ */
boolean             BIOreadInteger (file, sval)
BinFile             file;
int                *sval;

#endif /* __STDC__ */
{
   int                 s1, s2;

   if (!BIOreadShort (file, &s1))
     {
	*sval = 0;
	return FALSE;
     }
   if (!BIOreadShort (file, &s2))
     {
	*sval = 0;
	return FALSE;
     }
   *sval = 65536 * s1 + s2;
   return TRUE;
}


/* ---------------------------------------------------------------------- */
/* |    BIOreadName reads a string value.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             BIOreadName (BinFile file, char *name)
#else  /* __STDC__ */
boolean             BIOreadName (file, name)
BinFile             file;
char               *name;

#endif /* __STDC__ */
{
   int                 i;

   for (i = 0; i < MAX_NAME_LENGTH; i++)
     {
	if (!BIOreadByte (file, &name[i]))
	  {
	     name[i] = '\0';
	     return FALSE;
	  }
	if (name[i] == '\0')
	   break;
     }
   if (i >= MAX_NAME_LENGTH)
     {
	name[0] = '\0';
	return FALSE;
     }
   return TRUE;
}



/* ---------------------------------------------------------------------- */
/* |    BIOreadOpen opens a file for reading.                           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
BinFile             BIOreadOpen (char *filename)
#else  /* __STDC__ */
BinFile             BIOreadOpen (filename)
char               *filename;

#endif /* __STDC__ */
{
   return fopen (filename, "r");
}


/* ---------------------------------------------------------------------- */
/* |    BIOreadClose closes a file.                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                BIOreadClose (BinFile file)
#else  /* __STDC__ */
void                BIOreadClose (file)
BinFile             file;

#endif /* __STDC__ */
{
   if (file != NULL)
      fclose (file);
}


/* ---------------------------------------------------------------------- */
/* |    BIOwriteOpen opens a file for writing.                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
BinFile             BIOwriteOpen (char *filename)
#else  /* __STDC__ */
BinFile             BIOwriteOpen (filename)
char               *filename;

#endif /* __STDC__ */
{
   return fopen (filename, "w");
}


/* ---------------------------------------------------------------------- */
/* |    BIOwriteClose closes a file.                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                BIOwriteClose (BinFile file)
#else  /* __STDC__ */
void                BIOwriteClose (file)
BinFile             file;

#endif /* __STDC__ */
{
   if (file != NULL)
      fclose (file);
}

/* ---------------------------------------------------------------------- */
/* |    BIOwriteByte writes a character (or byte) value.                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             BIOwriteByte (BinFile file, char bval)
#else  /* __STDC__ */
boolean             BIOwriteByte (file, bval)
BinFile             file;
char                bval;

#endif /* __STDC__ */
{
   if (ferror (file))
      return FALSE;
   putc (bval, file);
   return TRUE;
}


/* ---------------------------------------------------------------------- */
/* |    BIOwriteIdentDoc writes a document identifier.                  | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                BIOwriteIdentDoc (BinFile file, DocumentIdentifier Ident)
#else  /* __STDC__ */
void                BIOwriteIdentDoc (file, Ident)
BinFile             file;
DocumentIdentifier     Ident;

#endif /* __STDC__ */
{
   int                 j;

   j = 1;
   while (j < MAX_DOC_IDENT_LEN && Ident[j - 1] != '\0')
     {
	BIOwriteByte (file, Ident[j - 1]);
	j++;
     }
   /* termine le nom par un octet nul */
   BIOwriteByte (file, '\0');
}

/* ---------------------------------------------------------------------- */
/* |    BIOreadIdentDoc                                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                BIOreadIdentDoc (BinFile file, DocumentIdentifier * Ident)
#else  /* __STDC__ */
void                BIOreadIdentDoc (file, Ident)
BinFile             file;
DocumentIdentifier    *Ident;

#endif /* __STDC__ */
{
   int                 j;

   j = 0;
   do
      if (!BIOreadByte (file, &((*Ident)[j++])))
	 (*Ident)[j - 1] = '\0';
   while (!(j >= MAX_DOC_IDENT_LEN || (*Ident)[j - 1] == '\0')) ;
}

/* ---------------------------------------------------------------------- */
/* |    CopyIdentDoc                                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                CopyIdentDoc (DocumentIdentifier * Dest, DocumentIdentifier Source)
#else  /* __STDC__ */
void                CopyIdentDoc (Dest, Source)
DocumentIdentifier    *Dest;
DocumentIdentifier     Source;

#endif /* __STDC__ */
{
   strncpy (*Dest, Source, MAX_DOC_IDENT_LEN);
}

/* ---------------------------------------------------------------------- */
/* |    MemeIdentDoc                                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             MemeIdentDoc (DocumentIdentifier Ident1, DocumentIdentifier Ident2)
#else  /* __STDC__ */
boolean             MemeIdentDoc (Ident1, Ident2)
DocumentIdentifier     Ident1;
DocumentIdentifier     Ident2;

#endif /* __STDC__ */
{
   boolean             ret;

   ret = (strcmp (Ident1, Ident2) == 0);
   return ret;
}

/* ---------------------------------------------------------------------- */
/* |    NulIdentDoc                                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                NulIdentDoc (DocumentIdentifier * Ident)
#else  /* __STDC__ */
void                NulIdentDoc (Ident)
DocumentIdentifier    *Ident;

#endif /* __STDC__ */
{
   (*Ident)[0] = '\0';
}

/* ---------------------------------------------------------------------- */
/* |    IdentDocNul                                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             IdentDocNul (DocumentIdentifier Ident)
#else  /* __STDC__ */
boolean             IdentDocNul (Ident)
DocumentIdentifier     Ident;

#endif /* __STDC__ */
{
   boolean             ret;

   ret = (Ident[0] == '\0');
   return ret;
}

/* ---------------------------------------------------------------------- */
/* |    GetDocIdent                                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                GetDocIdent (DocumentIdentifier * Ident, Name docName)
#else  /* __STDC__ */
void                GetDocIdent (Ident, docName)
DocumentIdentifier    *Ident;
Name                 docName;

#endif /* __STDC__ */

{
   strncpy (*Ident, docName, MAX_DOC_IDENT_LEN);
}

/* ---------------------------------------------------------------------- */
/* |    GetDocName                                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                GetDocName (DocumentIdentifier Ident, Name docName)
#else  /* __STDC__ */
void                GetDocName (Ident, docName)
DocumentIdentifier     Ident;
Name                 docName;

#endif /* __STDC__ */
{
   strncpy (docName, Ident, MAX_NAME_LENGTH);
}


/* ---------------------------------------------------------------------- */
/* |     OuvrEcr	returns the write access right for a file	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 OuvrEcr (char *fileName)
#else  /* __STDC__ */
int                 OuvrEcr (fileName)
char               *fileName;

#endif /* __STDC__ */
{
   int                 ret, i;
   char                c;

#ifdef NEW_WILLOWS
   ret = _access (fileName, 0);
#else  /* NEW_WILLOWS */
   ret = access (fileName, 0);
#endif /* NEW_WILLOWS */
   if (ret == -1)
     /* file does not exist */
     {
	/* check its directory */
	i = strlen (fileName);
	while ((i >= 0) && (fileName[i] != DIR_SEP))
	   i--;
	if (i < 0)
	   /* no directory name: current directory */
	   ret = access (".", 2);
	else
	  {
	     /* isolate the directory name */
	     c = fileName[i];
	     fileName[i] = '\0';
	     /* get access right for the directory */
	     ret = access (fileName, 2);
	     fileName[i] = c;
	  }
     }
   else
     /* file exists */
	ret = access (fileName, 2);
   return (ret);
}

/* ---------------------------------------------------------------------- */
/* |	ConvertitNombre	converts the value number into a character	| */
/* |	string according to a given style (arabic, roman, letter).	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ConvertitNombre (int number, CounterStyle style, char *string, int *len)
#else  /* __STDC__ */
void                ConvertitNombre (number, style, string, len)
int                 number;
CounterStyle        style;
char               *string;
int                *len;

#endif /* __STDC__ */
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
	       while (!(number == 0));
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

	       if (number > 26)
		  string[(*len)++] = '?';
	       else if (style == CntUppercase)
		  string[(*len)++] = (char) (number + (int) ('@'));
	       else
		  string[(*len)++] = (char) (number + (int) ('`'));
	       break;
	    default:
	       break;
	 }
   string[*len] = '\0';
}
