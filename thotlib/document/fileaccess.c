/*
   storage.c : Primitives d'entrees/sorties pour les fichiers binaires
   (schemas et pivot).
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
void                GetDocIdent (DocumentIdentifier * Ident, Name NomDoc)
#else  /* __STDC__ */
void                GetDocIdent (Ident, NomDoc)
DocumentIdentifier    *Ident;
Name                 NomDoc;

#endif /* __STDC__ */

{
   strncpy (*Ident, NomDoc, MAX_DOC_IDENT_LEN);
}

/* ---------------------------------------------------------------------- */
/* |    GetDocName                                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                GetDocName (DocumentIdentifier Ident, Name NomDoc)
#else  /* __STDC__ */
void                GetDocName (Ident, NomDoc)
DocumentIdentifier     Ident;
Name                 NomDoc;

#endif /* __STDC__ */
{
   strncpy (NomDoc, Ident, MAX_NAME_LENGTH);
}


/* ---------------------------------------------------------------------- */
/* |    OuvrEcr teste le droit d'ecriture du fichier :                  | */
/* |            - Si le fichier existe, on teste son droit d'ecriture.  | */
/* |            - Sinon on teste le droit d'ecriture du repertoire.     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 OuvrEcr (char *fichier)
#else  /* __STDC__ */
int                 OuvrEcr (fichier)
char               *fichier;

#endif /* __STDC__ */
{
   int                 ret, i;
   char                sauve;

#ifdef NEW_WILLOWS
   ret = _access (fichier, 0);
#else  /* NEW_WILLOWS */
   ret = access (fichier, 0);
#endif /* NEW_WILLOWS */
   if (ret == -1)
     {
	/* Le fichier n'existe pas */
	/* On recherche le repertoire de creation */
	i = strlen (fichier);
	while ((i >= 0) && (fichier[i] != DIR_SEP))
	   i--;
	if (i < 0)
	   ret = access (".", 2);
	/* C'est le repertoire courant */
	else
	  {
	     /* On prend le nom du repertoire */
	     sauve = fichier[i];
	     fichier[i] = '\0';
	     ret = access (fichier, 2);
	     /* On restaure le nom du fichier */
	     fichier[i] = sauve;
	  }
     }
   else
     {
	/* Le fichier existe */
	ret = access (fichier, 2);
	/* Droit d'ecriture */
     }
   return (ret);
}

/* ---------------------------------------------------------------------- */
/* |    ConvertitNombre convertit le nombre nb sous la forme d'une      | */
/* |    chaine de caracteres et met le resultat dans Chaine.            | */
/* |    Au retour, lg contient la longueur du nombre converti.          | */
/* |    Style indique s'il faut convertir en chiffre arabes, en chiffres| */
/* |    romains ou en lettres.                                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ConvertitNombre (int nb, CounterStyle Style, char *Chaine, int *lg)
#else  /* __STDC__ */
void                ConvertitNombre (nb, Style, Chaine, lg)
int                 nb;
CounterStyle          Style;
char               *Chaine;
int                *lg;

#endif /* __STDC__ */
{
   int                 n, c, i, debut;

   n = nb;
   *lg = 0;
   if (n < 0)
     {
	Chaine[(*lg)++] = '-';
	n = -n;
     }

   switch (Style)
	 {
	    case CntArabic:
	       if (n >= 100000)
		 {
		    Chaine[(*lg)++] = '?';
		    n = n % 100000;
		 }
	       if (n >= 10000)
		  c = 5;
	       else if (n >= 1000)
		  c = 4;
	       else if (n >= 100)
		  c = 3;
	       else if (n >= 10)
		  c = 2;
	       else
		  c = 1;
	       *lg += c;
	       i = *lg;
	       do
		 {
		    Chaine[i - 1] = (char) ((int) ('0') + n % 10);
		    i--;
		    n = n / 10;
		 }
	       while (!(n == 0));
	       break;
	    case CntURoman:
	    case CntLRoman:
	       if (n >= 4000)
		  Chaine[(*lg)++] = '?';
	       else
		 {
		    debut = *lg + 1;
		    while (n >= 1000)
		      {
			 Chaine[(*lg)++] = 'M';
			 n -= 1000;
		      }
		    if (n >= 900)
		      {
			 Chaine[(*lg)++] = 'C';
			 Chaine[(*lg)++] = 'M';
			 n -= 900;
		      }
		    else if (n >= 500)
		      {
			 Chaine[(*lg)++] = 'D';
			 n -= 500;
		      }
		    else if (n >= 400)
		      {
			 Chaine[(*lg)++] = 'C';
			 Chaine[(*lg)++] = 'D';
			 n -= 400;
		      }
		    while (n >= 100)
		      {
			 Chaine[(*lg)++] = 'C';
			 n -= 100;
		      }
		    if (n >= 90)
		      {
			 Chaine[(*lg)++] = 'X';
			 Chaine[(*lg)++] = 'C';
			 n -= 90;
		      }
		    else if (n >= 50)
		      {
			 Chaine[(*lg)++] = 'L';
			 n -= 50;
		      }
		    else if (n >= 40)
		      {
			 Chaine[(*lg)++] = 'X';
			 Chaine[(*lg)++] = 'L';
			 n -= 40;
		      }
		    while (n >= 10)
		      {
			 Chaine[(*lg)++] = 'X';
			 n -= 10;
		      }
		    if (n >= 9)
		      {
			 Chaine[(*lg)++] = 'I';
			 Chaine[(*lg)++] = 'X';
			 n -= 9;
		      }
		    else if (n >= 5)
		      {
			 Chaine[(*lg)++] = 'V';
			 n -= 5;
		      }
		    else if (n >= 4)
		      {
			 Chaine[(*lg)++] = 'I';
			 Chaine[(*lg)++] = 'V';
			 n -= 4;
		      }
		    while (n >= 1)
		      {
			 Chaine[(*lg)++] = 'I';
			 n--;
		      }
		    if (Style == CntLRoman)
		       /* traduit en minuscules */
		       for (i = debut; i <= *lg; i++)
			  if (Chaine[i - 1] != '?')
			     Chaine[i - 1] = (char) ((int) (Chaine[i - 1]) + 32);
		 }
	       break;
	    case CntUppercase:
	    case CntLowercase:

	       if (n > 26)
		  Chaine[(*lg)++] = '?';
	       else if (Style == CntUppercase)
		  Chaine[(*lg)++] = (char) (n + (int) ('@'));
	       else
		  Chaine[(*lg)++] = (char) (n + (int) ('`'));
	       break;
	    default:
	       break;
	 }
   Chaine[*lg] = '\0';
}
