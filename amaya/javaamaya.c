/*
 * javaamaya.h : defines part of the Java related Amaya API.
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "javaamaya.h"

extern int LaunchJavaApplet(char *appletclass, int doc);

/*----------------------------------------------------------------------
   GetUserAgentName returns the User Agent ID string
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetUserAgentName (void)
#else
char *GetUserAgentName ()

#endif
{
    static char UserAgentName[50];
    /* sprintf(UserAgentName,"%s/%s", HTAppName, HTAppVersion); */
    sprintf(&UserAgentName[0],"%s/%s", "Amaya", HTAppVersion);
    return(&UserAgentName[0]);
}

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
    char *str = TtaGetMessage(AMAYA, msg);
    if (str == NULL) {
        fprintf(stderr,"AmayaGetMessage(%d) failed\n", msg);
	return("");
    }
    return(str);
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
   GetTempDirectory returns the path to the Amaya temporary directory
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetTempDirectory (void)
#else
char *GetTempDirectory ()

#endif
{
    return(&TempFileDirectory[0]);
}

/*
 * The Java Applet Selector.
 */
#define JAVA_MAX_REF	 20

#define JAVA_FORM        1   /* For plugin's selection */
#define JAVA_APPLET_NAME 2   /* Selector for Applet Name */
#define JAVA_LOCAL_NAME  3   /* Selector for Applet Name */
#define JAVA_DIR_SELECT  4   /* Selector for Applet Name */
#define JAVA_APL_SELECT  5   /* Selector for Applet Name */

char*                 appletPath;
int                   appletCounter ;

static Document       JavaCurrentDocument = 0;
static int            BaseJava = 0;
static char           JavaApplet[MAX_PATH] = "";
static char           JavaAppletName[MAX_PATH] = "";
static char           JavaAppletDirectory[MAX_PATH] = "";


/*
 * Callback for the Java applets selector.
 */
#ifdef __STDC__
static void CallbackFormJava (int ref, int dataType, char* data)
#else  /* __STDC__ */
static void CallbackFormJava (ref, dataType, data)
int   ref; 
int   dataType; 
char* data;
#endif /* __STDC__ */
{
   int val = (int) data;
   char tempname[MAX_PATH];
   boolean change;

   switch (ref - BaseJava) {
       case JAVA_FORM:
       if (val == 2)
	  /* Clear */
	 {
	    JavaApplet[0] = EOS;
	    TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, JavaApplet);
	 }
       else if (val == 3)
	  /* Parse */
	 {
	    /* reinitialize directories and document lists */
	    TtaListDirectory (JavaAppletDirectory, BaseJava + JAVA_FORM,
			      TtaGetMessage (LIB, TMSG_DOC_DIR),
			      BaseJava + JAVA_DIR_SELECT,
			      ".class", TtaGetMessage (AMAYA, AM_FILES),
			      BaseJava + JAVA_APL_SELECT);
	 }
       else
	 {
	    TtaDestroyDialogue (BaseJava + JAVA_FORM);
	    if (val == 1)
	       /* OK */
	      {
		 if (JavaApplet[0] != EOS)
		   {
		     LaunchJavaApplet(JavaApplet, JavaCurrentDocument);
		   }
		 CurrentDocument = 0;
	      }
	 }
       break;
    case JAVA_APPLET_NAME:
       if (IsW3Path (data))
	 {
	    /* save the URL name */
	    strcpy (JavaApplet, data);
	    JavaAppletName[0] = EOS;
	 }
       else
	 {
	    JavaApplet[0] = EOS;
	    change = NormalizeFile (data, JavaApplet);
	    if (change)
	       TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, JavaApplet);

	    if (TtaCheckDirectory (JavaApplet))
	      {
		 strcpy (JavaAppletDirectory, JavaApplet);
		 JavaAppletName[0] = EOS;
		 /* reinitialize directories and document lists
		    TtaListDirectory (JavaAppletDirectory, BaseJava + JAVA_FORM,
		    TtaGetMessage (LIB, TMSG_DOC_DIR), BaseJava + JAVA_DIR_SELECT,
		    ".*htm*", TtaGetMessage (AMAYA, AM_FILES), BaseJava + JAVA_APL_SELECT);
		  */
	      }
	    else
	       TtaExtractName (JavaApplet, JavaAppletDirectory, JavaAppletName);
	 }

       break;
    case JAVA_DIR_SELECT:
       if (!strcmp (data, ".."))
	 {
	    /* suppress last directory */
	    strcpy (tempname, JavaAppletDirectory);
	    TtaExtractName (tempname, JavaAppletDirectory, JavaApplet);
	 }
       else
	 {
	    strcat (JavaAppletDirectory, DIR_STR);
	    strcat (JavaAppletDirectory, data);
	 }
       TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, JavaAppletDirectory);
       TtaListDirectory (JavaAppletDirectory, BaseJava + JAVA_FORM,
	                 TtaGetMessage (LIB, TMSG_DOC_DIR), 
			 BaseJava + JAVA_DIR_SELECT, ".class",
			 TtaGetMessage (AMAYA, AM_FILES),
			 BaseJava + JAVA_APL_SELECT);
       JavaAppletName[0] = EOS;
       break;
    case JAVA_APL_SELECT:
       if (JavaAppletDirectory[0] == EOS)
	  /* set path on current directory */
	  getcwd (JavaAppletDirectory, MAX_LENGTH);

       /* Extract suffix from document name */
       strcpy (JavaAppletName, data);
       JavaApplet[0] = EOS;
       /* construct the document full name */
       strcpy (JavaApplet, JavaAppletDirectory);
       strcat (JavaApplet, DIR_STR);
       strcat (JavaApplet, JavaAppletName);
       TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, JavaApplet);
       break;
   default:
       break;                         
   }
}

/*
 * the Java applets selector create routine.
 */
#ifdef __STDC__ 
void CreateFormJava (Document document, View view)
#else  /* __STDC__ */
void CreateFormJava (document, view) 
Document document; 
View     view;
#endif /* __STDC__ */
{
   int                 i;
   char               *s;

   if (BaseJava == 0) {
      BaseJava = TtaSetCallback ((Proc) CallbackFormJava, JAVA_MAX_REF);
      s = TtaGetEnvString("AMAYA_APPLET_DIR");
      if (s != NULL) strcpy(JavaAppletDirectory, s);
   }

   JavaCurrentDocument = document;
   s = TtaGetMemory (MAX_LENGTH);

   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));

   TtaNewSheet (BaseJava + JAVA_FORM, TtaGetViewFrame (document, view),
                TtaGetMessage (AMAYA, AM_OPEN_URL),
		3, s, TRUE, 2, 'L', D_CANCEL);
   TtaNewTextForm (BaseJava + JAVA_APPLET_NAME, BaseJava + JAVA_FORM,
		   TtaGetMessage (AMAYA, AM_OPEN_URL), 50, 1, TRUE);
   TtaNewLabel (BaseJava + JAVA_LOCAL_NAME, BaseJava + JAVA_FORM, " ");
   TtaListDirectory (JavaAppletDirectory, BaseJava + JAVA_FORM,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseJava + JAVA_DIR_SELECT, ".class",
		     TtaGetMessage (AMAYA, AM_FILES),
		     BaseJava + JAVA_APL_SELECT);
   if (JavaAppletName[0] != EOS)
      TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, JavaAppletName);
   else
     {
	strcpy (s, JavaAppletDirectory);
	strcat (s, DIR_STR);
	strcat (s, JavaAppletName);
	TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, s);
     }
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseJava + JAVA_FORM, FALSE);
   TtaFreeMemory (s);
}
