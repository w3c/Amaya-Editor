/*
 * javaamaya.h : defines part of the programmable Amaya API.
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "javaamaya.h"


/*----------------------------------------------------------------------
   GetDocumentURL returns the URL of the document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetDocumentURL (Document doc)
#else
char *GetDocumentURL (doc)
Document doc;

#endif
{
    if ((doc < 0) || (doc >= DocumentTableLength)) return(NULL);
    return(DocumentURLs[doc]);
}

/*----------------------------------------------------------------------
   AmayaGetMessage returns an amaya message
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *AmayaGetMessage (int msg)
#else
char *AmayaGetMessage (msg)
int msg;

#endif
{
    return(TtaGetMessage(AMAYA, msg));
}

/*----------------------------------------------------------------------
   PopUpAuthDialog : open the request for User/Passwd dialog.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int PopUpAuthDialog (int doc)
#else
int PopUpAuthDialog ()
int doc;

#endif
{
    Answer_name[0] = EOS;
    Answer_password[0] = EOS;
    Lg_password = 0;
    InitFormAnswer(doc, 1);
    if ((Answer_name[0] == EOS) && (Answer_password[0] == EOS)) return(-1);
    return(0);
}

/*----------------------------------------------------------------------
   GetUserString returns the user name string entered
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetUserString (void)
#else
char *GetUserString ()

#endif
{
    return(&Answer_name[0]);
}

/*----------------------------------------------------------------------
   GetPasswdString returns the password string entered
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetPasswdString (void)
#else
char *GetPasswdString ()

#endif
{
    return(&Answer_password[0]);
}

/*----------------------------------------------------------------------
   GetAnswerString returns the string entered
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetAnswerString (void)
#else
char *GetAnswerString ()

#endif
{
    return(&Answer_text[0]);
}

/*----------------------------------------------------------------------
   GetTempDirectory returns the path to the temprorary directory
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetTempDirectory (void)
#else
char *GetTempDirectory ()

#endif
{
    return(&TempFileDirectory[0]);
}

