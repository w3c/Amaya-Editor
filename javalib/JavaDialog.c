/*
 * JavaDialog.c : Link to the Standard Thot Library interface
 *                for lauching new classes and managing threads.
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "app.h"
#include "application.h"
#include "dialog.h"
#include "interface.h"
#include "libmsg.h"
#include "message.h"
#include "browser.h"

#include "native.h"
#include "javamsg.h"

extern int LaunchJavaApplet(char *appletclass, int doc, char *args);
extern int ListJavaUserThreads(char *list, int len);
void CreateTuningFormJava (Document document, View view);

/*
 * The Java Applet Selector.
 */
#define JAVA_MAX_REF	 20

#define JAVA_FORM        1   /* For plugin's selection */
#define JAVA_DIR_SELECT  2   /* Selector for Directory */
#define JAVA_APL_SELECT  3   /* Selector for Name */
#define JAVA_APPLET_NAME 4   /* Full Applet path entry */
#define JAVA_LOCAL_NAME  5   /*                        */
#define JAVA_APPLET_ARGS 6   /* Full Applet path entry */
#define JAVA_LOCAL_NAME2 7   /*                        */

#define JAVA_THREAD_FORM 10  /* Thread form            */
#define JAVA_THREAD_LIST 11  /* Thread list            */

char*                 appletPath;
int                   appletCounter ;

static Document       JavaCurrentDocument = 0;
static int            BaseJava = 0;
static char           JavaApplet[MAX_PATH] = "";
static char           JavaAppletName[MAX_PATH] = "";
static char           JavaAppletDirectory[MAX_PATH] = "";
static char           JavaAppletArgs[MAX_PATH] = "";
static char           JavaThread[300] = "";

static int            JavaDialogueInitialized = 0;

#define IS_SPACE(p) ((*(p) == ' ') || (*(p) == '\t'))
#define MAX_ARGS	30

/*----------------------------------------------------------------------
   LaunchJavaApplet

   Start a Java applet.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                LaunchJavaApplet (char *appletclass, int doc, char *args)
#else
int                LaunchJavaApplet (appletclass, doc, args)
char *appletclass;
int doc;
char *args;
#endif
{
    char name[100];
    char directory[MAX_PATH];
    char arg_list[MAX_PATH];
    char *args_list[MAX_ARGS];
    char *ptr;
    int  i,j;
    jword res;
    struct Hjava_lang_String* jname;
    struct Hjava_lang_String** cur_arg;
    HArrayOfObject* jargs;

    TtaExtractName(appletclass, directory, name);

    /*
     * Extract the Class name.
     */
    if ((strlen(name) > 6) &&
        (!strcmp(&name[strlen(name) - 6],".class")))
	name[strlen(name) - 6] = '\0';

    /*
     * Add the directory to the ClassPath of Kaffe
     */
    if (addClasspath(directory) < 0) {
	return(-1);
    }

    /*
     * Parse the args string to build the String table.
     */
    if (args != NULL) {
        /* Copy the initial args onto a clean buffer */
        strncpy(arg_list, args, sizeof(arg_list) -1);

	/* clean up the args_list tab */
	for (i = 0;i < MAX_ARGS;i++) args_list[i] = NULL;

	/* Split the buffer into the arg list */
	ptr = &arg_list[0];
	while (IS_SPACE(ptr)) ptr++;
	args_list[0] = ptr;
	i = 0;
	if (*ptr != '\0')
	    for (; (i < MAX_ARGS);ptr++) {
		if (*ptr == '\0') {
		    i++;
		    break;
		}
		if (IS_SPACE(ptr)) {
		    *ptr = 0;
		    i++;
		    ptr++;
		    while (IS_SPACE(ptr)) ptr++;
		    if (*ptr == '\0') break;
		    args_list[i] = ptr;
		}
	    }

	/*
	 * allocate the array of String container and
	 * fill it with Java strings build from the parsing
	 */
	jargs = (HArrayOfObject*) AllocObjectArray(i , "Ljava/lang/String;");
	cur_arg = (Hjava_lang_String**) (unhand(jargs));
	for (j = 0;j < i;j++)
	    cur_arg[j] = makeJavaString(args_list[j], strlen(args_list[j]));
    } else
	jargs = (HArrayOfObject*) AllocObjectArray(0 , "Ljava/lang/String;");

    /*
     * Ask for a new thread handling the job.
     */
    jname = makeJavaString(name, strlen(name));
    res = do_execute_java_class_method("thotlib.userThreadPool",
	      "LaunchJavaApplet", "(Ljava/lang/String;I[Ljava/lang/String;)I",
	      jname, doc, jargs);

    return(res);
}

/*----------------------------------------------------------------------
   KillJavaApplet

   Kill a Java applet, by it's name from the list.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                KillJavaApplet (char *appletname)
#else
int                KillJavaApplet (appletname)
char *appletname;
#endif
{
    int thread_no;

    if (sscanf(appletname, "%d : ", &thread_no) == 1) {
        do_execute_java_class_method("thotlib.userThreadPool",
				     "Kill", "(I)V", thread_no);
    }
    return(0);
}

/*----------------------------------------------------------------------
   ResumeJavaApplet

   Resumes a Java applet, by it's name from the list.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                ResumeJavaApplet (char *appletname)
#else
int                ResumeJavaApplet (appletname)
char *appletname;
#endif
{
    int thread_no;

    if (sscanf(appletname, "%d : ", &thread_no) == 1) {
        do_execute_java_class_method("thotlib.userThreadPool",
				     "Resume", "(I)V", thread_no);
    }
    return(0);
}

/*----------------------------------------------------------------------
   SuspendJavaApplet

   Suspends a Java applet, by it's name from the list.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                SuspendJavaApplet (char *appletname)
#else
int                SuspendJavaApplet (appletname)
char *appletname;
#endif
{
    int thread_no;

    if (sscanf(appletname, "%d : ", &thread_no) == 1) {
        do_execute_java_class_method("thotlib.userThreadPool",
				     "Suspend", "(I)V", thread_no);
    }
    return(0);
}

/*----------------------------------------------------------------------
   ListJavaUserThreads

   List all the user's thread, store in the buffer and returns
   the actual number of threads found.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                ListJavaUserThreads(char *list, int len)
#else
int                ListJavaUserThreads (list, len)
char *list;
int len;
#endif
{
   int i,t;
   int nb;
   char name[200];
   struct Hjava_lang_String* str;

   i = 0;
   nb = 0;
   for (t = 0; t < 10;t++) {
    str = makeJavaString(name, strlen(name));
       str = (struct Hjava_lang_String*)
               do_execute_java_class_method("thotlib.userThreadPool",
	                                    "Name",
					    "(I)Ljava/lang/String;", t);
       if (str == NULL) continue;
       javaString2CString(str, name, sizeof(name));
       if (i + strlen(name) >= len) break;
       strcpy (&list[i], name);
       i += strlen (&list[i]) + 1;
       nb++;
   }
   return(nb);
}

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

   switch (ref - BaseJava) {
       case JAVA_FORM:
       if (val == 1)
	  /* Load */
	 {
	     if (JavaApplet[0] != '\0')
	       {
		 LaunchJavaApplet(JavaApplet, JavaCurrentDocument,
		                  JavaAppletArgs);
	       }
	 }
       else if (val == 2)
	  /* Clear */
	 {
	    JavaApplet[0] = '\0';
	    TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, JavaApplet);
	    JavaAppletArgs[0] = '\0';
	    TtaSetTextForm (BaseJava + JAVA_APPLET_ARGS, JavaAppletArgs);
	 }
       else if (val == 3)
	  /* Reread */
	 {
	    /* reinitialize directories and document lists */
	    TtaListDirectory (JavaAppletDirectory, BaseJava + JAVA_FORM,
			      TtaGetMessage (LIB, TMSG_DOC_DIR),
			      BaseJava + JAVA_DIR_SELECT,
			      ".class", TtaGetMessage (JAVA, JA_FILES),
			      BaseJava + JAVA_APL_SELECT);
	 }
       else if (val == 4)
	  /* Tuning */
	 {
	     CreateTuningFormJava (JavaCurrentDocument, 1);
	 }
       else
	 {
	    TtaDestroyDialogue (BaseJava + JAVA_FORM);
	    JavaCurrentDocument = 0;
	 }
       break;
    case JAVA_APPLET_NAME:
       if ((!strncmp(data, "http://", 7)) ||
           (!strncmp(data, "ftp://", 6)))
	 {
	    /* save the URL name */
	    strcpy (JavaApplet, data);
	    JavaAppletName[0] = '\0';
	 }
       else
	 {
	    strcpy(JavaApplet, data);
	    TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, JavaApplet);

	    if (TtaCheckDirectory (JavaApplet))
	      {
		 strcpy (JavaAppletDirectory, JavaApplet);
		 JavaAppletName[0] = '\0';
		 /* reinitialize directories and document lists
		    TtaListDirectory (JavaAppletDirectory, BaseJava + JAVA_FORM,
		    TtaGetMessage (LIB, TMSG_DOC_DIR), BaseJava + JAVA_DIR_SELECT,
		    ".*htm*", TtaGetMessage (JAVA, JA_FILES), BaseJava + JAVA_APL_SELECT);
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
			 TtaGetMessage (JAVA, JA_FILES),
			 BaseJava + JAVA_APL_SELECT);
       JavaAppletName[0] = '\0';
       break;
    case JAVA_APL_SELECT:
       if (JavaAppletDirectory[0] == '\0')
	  /* set path on current directory */
	  getcwd (JavaAppletDirectory, MAX_PATH);

       /* Extract suffix from document name */
       strcpy (JavaAppletName, data);
       JavaApplet[0] = '\0';
       /* construct the document full name */
       strcpy (JavaApplet, JavaAppletDirectory);
       strcat (JavaApplet, DIR_STR);
       strcat (JavaApplet, JavaAppletName);
       TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, JavaApplet);
       break;
    case JAVA_APPLET_ARGS:
       strcpy (JavaAppletArgs, data);
       break;
    case JAVA_THREAD_FORM:
       if (val == 1)
	  /* Refresh */
	 {
	   int i;
	   char s[MAX_PATH];

	   i = ListJavaUserThreads(s, MAX_PATH);
	   TtaNewSelector (BaseJava + JAVA_THREAD_LIST,
			   BaseJava + JAVA_THREAD_FORM,
			   TtaGetMessage (JAVA, JA_JAVA_THREADS),
			   i, s, 10, NULL, FALSE, TRUE);
	   break;
	 }
       else if (val == 2)
	  /* Kill */
	 {
	   int i;
	   char s[MAX_PATH];

           KillJavaApplet(JavaThread);

	   i = ListJavaUserThreads(s, MAX_PATH);
	   TtaNewSelector (BaseJava + JAVA_THREAD_LIST,
			   BaseJava + JAVA_THREAD_FORM,
			   TtaGetMessage (JAVA, JA_JAVA_THREADS),
			   i, s, 10, NULL, FALSE, TRUE);
	   break;
	 }
       else if (val == 3)
	  /* Suspend */
	 {
           SuspendJavaApplet(JavaThread);
	   break;
	 }
       else if (val == 4)
	  /* Resume */
	 {
           ResumeJavaApplet(JavaThread);
	   break;
	 }
       else
	 {
	    TtaDestroyDialogue (BaseJava + JAVA_THREAD_FORM);
	 }
       break;
    case JAVA_THREAD_LIST:
       strcpy (JavaThread, data);
       break;
   default:
       break;                         
   }
}

/*
 * InitJavaDialogue.
 */
#ifdef __STDC__
void InitJavaDialogue (void)
#else  /* __STDC__ */
void InitJavaDialogue ()
#endif /* __STDC__ */
{
    char *s;

    if (JavaDialogueInitialized) return;
    JAVA = TtaGetMessageTable ("javamsg", JAVA_MSG_MAX);
    BaseJava = TtaSetCallback ((Proc) CallbackFormJava, JAVA_MAX_REF);
    s = TtaGetEnvString("AMAYA_APPLET_DIR");
    if (s != NULL) strcpy(JavaAppletDirectory, s);
    JavaDialogueInitialized = 1;
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

   if (BaseJava == 0) InitJavaDialogue();

   JavaCurrentDocument = document;
   s = TtaGetMemory (MAX_PATH);

   i = 0;
   strcpy (&s[i], TtaGetMessage (JAVA, JA_LOAD));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (JAVA, JA_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (JAVA, JA_PARSE));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (JAVA, JA_TUNING));

   TtaNewSheet (BaseJava + JAVA_FORM, TtaGetViewFrame (document, view),
                TtaGetMessage (JAVA, JA_LOAD_JAVA_CLASS),
		4, s, TRUE, 2, 'L', D_CANCEL);
   TtaListDirectory (JavaAppletDirectory, BaseJava + JAVA_FORM,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseJava + JAVA_DIR_SELECT, ".class",
		     TtaGetMessage (JAVA, JA_FILES),
		     BaseJava + JAVA_APL_SELECT);
   TtaNewTextForm (BaseJava + JAVA_APPLET_NAME, BaseJava + JAVA_FORM,
		   TtaGetMessage (JAVA, JA_LOAD_JAVA_CLASS), 50, 1, TRUE);
   if (JavaAppletName[0] != '\0')
      TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, JavaAppletName);
   else
     {
	strcpy (s, JavaAppletDirectory);
	strcat (s, DIR_STR);
	strcat (s, JavaAppletName);
	TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, s);
     }
   TtaNewLabel (BaseJava + JAVA_LOCAL_NAME, BaseJava + JAVA_FORM, " ");
   TtaNewTextForm (BaseJava + JAVA_APPLET_ARGS, BaseJava + JAVA_FORM,
		   TtaGetMessage (JAVA, JA_LOAD_JAVA_ARGS), 50, 1, TRUE);
   if (JavaAppletName[0] != '\0')
      TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, JavaAppletName);
   else
     {
	strcpy (s, JavaAppletDirectory);
	strcat (s, DIR_STR);
	strcat (s, JavaAppletName);
	TtaSetTextForm (BaseJava + JAVA_APPLET_NAME, s);
     }
   TtaNewLabel (BaseJava + JAVA_LOCAL_NAME2, BaseJava + JAVA_FORM, " ");
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseJava + JAVA_FORM, TRUE);
   TtaFreeMemory (s);
}

/*
 * the Java applets selector create routine.
 */
#ifdef __STDC__ 
void CreateTuningFormJava (Document document, View view)
#else  /* __STDC__ */
void CreateFormJava (document, view) 
Document document; 
View     view;
#endif /* __STDC__ */
{
   int                 i;
   char               *s;

   if (BaseJava == 0) InitJavaDialogue();

   JavaCurrentDocument = document;
   s = TtaGetMemory (MAX_PATH);

   i = 0;
   strcpy (&s[i], TtaGetMessage (JAVA, JA_PARSE));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (JAVA, JA_KILL_JAVA));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (JAVA, JA_SUSPEND_JAVA));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (JAVA, JA_RESTART_JAVA));

   TtaNewSheet (BaseJava + JAVA_THREAD_FORM, TtaGetViewFrame (document, view),
                TtaGetMessage (JAVA, JA_JAVA_THREADS),
		4, s, TRUE, 1, 'L', D_CANCEL);
   i = ListJavaUserThreads(s, MAX_PATH);
   TtaNewSelector (BaseJava + JAVA_THREAD_LIST,
                   BaseJava + JAVA_THREAD_FORM,
		   TtaGetMessage (JAVA, JA_JAVA_THREADS),
		   i, s, 10, NULL, FALSE, TRUE);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseJava + JAVA_THREAD_FORM, TRUE);
   TtaFreeMemory (s);
}

