/*
 * javaamaya.c : defines part of the Java related Amaya API.
 *
 * Daniel Veillard, 1997
 */

#ifdef AMAYA_JAVA

#define THOT_EXPORT extern
#include "amaya.h"
#include "javaamaya.h"

#include "html2thot_f.h"

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

/*----------------------------------------------------------------------
   GetHTMLtypeNo returns the thot internal type number for an HTML tag
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int GetHTMLtypeNo(char *tag)
#else
int GetHTMLtypeNo(tag)
char *tag;

#endif
{
    ElementType	elType;
    GIType(tag, &elType);
    return(elType.ElTypeNum); /**** should return elType *****/
}

/*----------------------------------------------------------------------
   GetHTMLTag return the HTML tag for a given Thot type
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetHTMLTag(ElementType elType)
#else
char *GetHTMLTag(elType)
ElementType elType;

#endif
{
    return(GITagNameByType(elType));
}

/*----------------------------------------------------------------------
   GetHTMLAttrNo returns the thot internal HTML attribute number
                 An attribute name may be used on different element
		 with different semantic, hence one can give an element
		 name to better specify the attribute. If null, the first
		 ID corresponding of an attribute spelled tag will be
		 returned
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int GetHTMLAttrNo(char *tag, char *elem)
#else
int GetHTMLAttrNo(tag, elem)
char *tag;
char *elem;

#endif
{
    int res = 0;
    /* AttrType(tag, &res); */
    return(res);
}

/*----------------------------------------------------------------------
   GetHTMLAttr return thot internal type number for an HTML attribute name
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetHTMLAttr(int type)
#else
char *GetHTMLAttr(type)
int type;

#endif
{
    /* return(AttrTagNameByType(type, elem)); */
    return(NULL);
}

#endif /* AMAYA_JAVA */

