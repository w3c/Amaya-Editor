
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   sysexec.c : interface unix pour impression & traduction.
   Gestion des messages externes.
   IV : Mai 92   adaptation Tool Kit
 */
#include "thot_sys.h"
#ifndef _XLIB_H_
#endif
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "libmsg.h"
#include "message.h"
#include "thotfile.h"

#undef EXPORT
#define EXPORT extern
#include "frame.var"
#include "environ.var"

#include "dofile_f.h"

static ThotFileHandle msgfile_fid = ThotFile_BADHANDLE;		/* le fichier temporaire des messages */
static char         msgfile_name[40];	/* son nom */
static ThotFileOffset msgfile_curpos;	/* la position courante dans le fichier */
static char         msg_buffer[1024];	/* le message courant */
static int          background;

/* Nombre de shells lances en background */

#ifdef __STDC__
extern char        *TtaGetEnvString (char *);
extern int          RemoveFile (char *);

#else  /* __STDC__ */
extern char        *TtaGetEnvString ();
extern int          RemoveFile ();

#endif /* __STDC__ */

#ifdef __STDC__
static void         initImpression (int user_orientation, char *thotdir, char *tempdir, char *dir, char *nom, char *realname,
				    char *imprimante, int pid, long thotWin, char *thotsch, char *thotdoc, char *traitement)
#else  /* __STDC__ */
static void         initImpression (user_orientation, thotdir, tempdir, dir, nom, realname, imprimante, pid, thotWin, thotsch, thotdoc, traitement)
int                 user_orientation;
char               *thotdir;
char               *tempdir;
char               *dir;
char               *nom;
char               *realname;
char               *imprimante;
int                 pid;
long                thotWin;
char               *thotsch;
char               *thotdoc;
char               *traitement;

#endif /* __STDC__ */
{
   char                cmd[800];
   char               *bak_name;
   char               *piv_name;

   if (user_orientation == 0)
      strcpy (orientation, "Portrait");
   else
      strcpy (orientation, "Paysage");

   sprintf (cmd, "/bin/mkdir %s\n", tempdir);
   system (cmd);
   sprintf (cmd, "chmod +rwx '%s'\n", tempdir);
   system (cmd);

   bak_name = (char *) TtaGetMemory (strlen (nom) + 5);
   sprintf (bak_name, "%s.BAK", nom);
   piv_name = (char *) TtaGetMemory (strlen (nom) + 5);
   sprintf (piv_name, "%s.PIV", nom);
   sprintf (cmd, "/bin/mv '%s'/'%s' '%s'/'%s'\n", dir, bak_name, tempdir, piv_name);
   system (cmd);
   sprintf (cmd, "traitement=%s\n", traitement);
   system (cmd);
   sprintf (cmd, "export traitement\n");
   system (cmd);
   sprintf (cmd, "realname=%s\n", realname);
   system (cmd);
   sprintf (cmd, "export realname\n");
   system (cmd);
   /*  sprintf (cmd, "printer_or_psname=%s\n", imprimante) ;
      system (cmd) ;
      sprintf (cmd, "export printer_or_psname\n") ;
      system (cmd) ; */
   sprintf (cmd, "thotpid=%d\n", pid);
   system (cmd);
   sprintf (cmd, "export pid\n");
   system (cmd);
   sprintf (cmd, "thotwindow=%ld\n", thotWin);
   system (cmd);
   sprintf (cmd, "export thotwindow\n");
   system (cmd);
   sprintf (cmd, "BIN=%s/bin\n", thotdir);
   system (cmd);
   sprintf (cmd, "export BIN\n");
   system (cmd);
   sprintf (cmd, "THOTDIR=%s\n", thotdir);
   system (cmd);
   sprintf (cmd, "export THOTDIR\n");
   system (cmd);
   sprintf (cmd, "THOTSCH=%s\n", thotsch);
   system (cmd);
   sprintf (cmd, "export THOTSCH\n");
   system (cmd);
   sprintf (cmd, "THOTDOC=%s:%s\n", tempdir, thotdoc);
   system (cmd);
   sprintf (cmd, "export THOTDOC\n");
   system (cmd);
}

/* ---------------------------------------------------------------------- */
/* |    Imprimer effectue le lancement du shell pour l'impression.      | */
/* ---------------------------------------------------------------------- */
/**513*/
#ifdef __STDC__
void                Imprimer (char *nom, char *dir, char *thotsch, char *thotdoc, char *thotpres, char *realname, char *realdir, char *imprimante, int pagedeb, int pagefin, int nbex, int decalage_h, int decalage_v, int user_orientation, int reduction, int nb_ppf, int suptrame, int alimmanuelle, int noiretblanc, int repaginer, char *vuesaimprimer)

#else  /* __STDC__ */
void                Imprimer (nom, dir, thotsch, thotdoc, thotpres, realname, realdir, imprimante, pagedeb, pagefin, nbex, decalage_h, decalage_v, user_orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, repaginer, vuesaimprimer)
char               *nom;
char               *dir;
char               *thotsch;
char               *thotdoc;
char               *thotpres;
char               *realname;
char               *realdir;
char               *imprimante;
int                 pagedeb;
int                 pagefin;
int                 nbex;
int                 decalage_h;
int                 decalage_v;
int                 user_orientation;
int                 reduction;
int                 nb_ppf;
int                 suptrame;
int                 alimmanuelle;
int                 noiretblanc;
int                 repaginer;
char               *vuesaimprimer;

#endif /* __STDC__ */

{
   ThotPid             pid = ThotPid_get ();

#ifndef NEW_WILLOWS
   char                cmd[800];
   int                 res;
   char               *thotdir;
   char               *tempdir;

   thotdir = (char *) TtaGetEnvString ("THOTDIR");
   if (!thotdir)
     {
	thotdir = ThotDir ();
     }

   tempdir = (char *) TtaGetMemory (40);
   sprintf (tempdir, "/tmp/Thot%d", pid);

   initImpression (user_orientation, thotdir, tempdir, dir, nom, realname, imprimante, pid, FrRef[0], thotsch, thotdoc, "Imprimer");
   if (imprimante[0] != '\0')
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s \"%s\" %s %d %d %d %s %d %d %d %d %d %ld Imprimer &\n",
	       DirectoryBinaries, nom, tempdir, repaginer, pagedeb, pagefin, vuesaimprimer, realname, imprimante, page_size, nbex,
	       decalage_h, decalage_v, orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, FrRef[0]);
   else
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %s %d %d %d %s %d %d %d %d %d %ld Imprimer &\n",
	       DirectoryBinaries, nom, tempdir, repaginer, pagedeb, pagefin, vuesaimprimer, realname, "lp", page_size, nbex,
	       decalage_h, decalage_v, orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, FrRef[0]);

   res = system (cmd);
   if (res == -1)
      TtaDisplaySimpleMessage (CONFIRM, LIB, LIB_ERROR_POSTSCRIPT_TRANSLATION);
   else
      /* Un nouveau shell en background */
      background++;
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    SauverPS effectue le lancement du shell pour sauvegarde PS.     | */
/* ---------------------------------------------------------------------- */
/**360*/
#ifdef __STDC__
void                SauverPS (char *nom, char *dir, char *thotsch, char *thotdoc, char *thotpres, char *realname, char *realdir, char *nomps, int pagedeb, int pagefin, int nbex, int decalage_h, int decalage_v, int user_orientation, int reduction, int nb_ppf, int suptrame, int alimmanuelle, int noiretblanc, int repaginer, char *vuesaimprimer)

#else  /* __STDC__ */
void                SauverPS (nom, dir, thotsch, thotdoc, thotpres, realname, realdir, nomps, pagedeb, pagefin, nbex, decalage_h, decalage_v, user_orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, repaginer, vuesaimprimer)
char               *nom;
char               *dir;
char               *thotsch;
char               *thotdoc;
char               *thotpres;
char               *realname;
char               *realdir;
char               *nomps;
int                 pagedeb;
int                 pagefin;
int                 nbex;
int                 decalage_h;
int                 decalage_v;
int                 user_orientation;
int                 reduction;
int                 nb_ppf;
int                 suptrame;
int                 alimmanuelle;
int                 noiretblanc;
int                 repaginer;
char               *vuesaimprimer;

#endif /* __STDC__ */

{
   char                cmd[800];
   int                 res;
   char               *thotdir;

   char               *tempdir;
   ThotPid             pid = ThotPid_get ();

   thotdir = TtaGetEnvString ("THOTDIR");
   if (!thotdir)
     {
	thotdir = ThotDir ();
     }
   tempdir = (char *) TtaGetMemory (40);
   sprintf (tempdir, "/tmp/Thot%d", pid);
   initImpression (user_orientation, thotdir, tempdir, dir, nom, realname, nomps, pid, FrRef[0], thotsch, thotdoc, "Sauver");

   if (nomps[0] != '\0')
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %s %d %d %d %s %d %d %d %d %d %ld Sauver &\n",
	       DirectoryBinaries, nom, tempdir, repaginer, pagedeb, pagefin, vuesaimprimer, realname, nomps, page_size, nbex,
	       decalage_h, decalage_v, orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, FrRef[0]);
   else
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %s %d %d %d %s %d %d %d %d %d %ld Sauver &\n",
	       DirectoryBinaries, nom, tempdir, repaginer, pagedeb, pagefin, vuesaimprimer, realname, "out.ps", page_size, nbex,
	       decalage_h, decalage_v, orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, FrRef[0]);

   res = system (cmd);
   if (res == -1)
      TtaDisplaySimpleMessage (CONFIRM, LIB, LIB_ERROR_POSTSCRIPT_TRANSLATION);
   else
      /* Un nouveau shell en background */
      background++;
}


/* ---------------------------------------------------------------------- */
/* |    GetTempNames genere un nom de fichier temporaire unique.          */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                GetTempNames (char *dir, char *file)
#else  /* __STDC__ */
void                GetTempNames (dir, file)
char               *dir;
char               *file;

#endif /* __STDC__ */
{
   strcpy (dir, "/tmp");
   strcpy (file, "ThotXXXXXX");
#ifdef WWW_MSWINDOWS
   _mktemp (file);
#else  /* WWW_MSWINDOWS */
   mktemp (file);
#endif /* !WWW_MSWINDOWS */
}
