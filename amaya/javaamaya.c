/*
 * javaamaya.c : defines part of the Java related Amaya API.
 *
 * Daniel Veillard, 1997
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "javaamaya.h"
#include "parser.h"

#include "css_f.h"
#include "EDITstyle_f.h"
#include "fetchHTMLname_f.h"
#include "html2thot_f.h"
#include "init_f.h"
#include "styleparser_f.h"

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
    /* sprintf (UserAgentName,"%s/%s", HTAppName, HTAppVersion); */
    sprintf (&UserAgentName[0],"%s/%s", "Amaya", HTAppVersion);
    return (&UserAgentName[0]);
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
    if ((doc < 0) || (doc >= DocumentTableLength))
      return (NULL);
    else
      return (DocumentURLs[doc]);
}

/*----------------------------------------------------------------------
   AmayaGetMessage returns an amaya message
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING AmayaGetMessage (int msg)
#else
STRING AmayaGetMessage (msg)
int msg;

#endif
{
    STRING str = TtaGetMessage (AMAYA, msg);
    if (str == NULL) 
      {
        fprintf (stderr,"AmayaGetMessage(%d) failed\n", msg);
	return ("");
      }
    else
      return (str);
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
    InitFormAnswer (doc, 1, TEXT(""), TEXT(""));
    if ((Answer_name[0] == EOS) && (Answer_password[0] == EOS))
      return (-1);
    else
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
    return (&Answer_name[0]);
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
    return (&Answer_password[0]);
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
    return (&Answer_text[0]);
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
    return (&TempFileDirectory[0]);
}

/*----------------------------------------------------------------------
   GetHTMLtypeNo returns the thot internal type number for an HTML tag
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int GetHTMLtypeNo (char *tag)
#else
int GetHTMLtypeNo (tag)
char *tag;
#endif
{
    ElementType	elType;

    GIType (tag, &elType, 0);
    return (elType.ElTypeNum); /**** should return elType *****/
}

/*----------------------------------------------------------------------
   GetHTMLTag return the HTML tag for a given Thot type
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetHTMLTag (ElementType elType)
#else
char *GetHTMLTag (elType)
ElementType elType;
#endif
{
  return (GITagNameByType (elType));
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
int GetHTMLAttrNo (char *tag, char *elem)
#else
int GetHTMLAttrNo (tag, elem)
char *tag;
char *elem;
#endif
{
  int res = 0;

  /* AttrType (tag, &res); */
  return (res);
}

/*----------------------------------------------------------------------
   GetHTMLAttr return thot internal type number for an HTML attribute name
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetHTMLAttr (int type)
#else
char *GetHTMLAttr (type)
int type;
#endif
{
  /* return (AttrTagNameByType (type, elem)); */
  return (NULL);
}

/*----------------------------------------------------------------------
  AddStyleSheet : Add a stylesheet for a document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void AddStyleSheet (char *url, int document)
#else
void AddStyleSheet (url, document)
char *url
int document;
#endif
{
    char URL[MAX_LENGTH];

    strcpy (URL, url);
    LoadStyleSheet (&URL[0], document, NULL, NULL);
}

/*----------------------------------------------------------------------
  AddAlternateStyleSheet : load a stylesheet for a document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void AddAlternateStyleSheet (char *url, int document)
#else
void AddAlternateStyleSheet (url, document)
char *url
int document;
#endif
{
  /* There is currently no support for alternate styles in Amaya. */
}

/*----------------------------------------------------------------------
  GetElementStyle : Get the CSS syntax for the style applied to an element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetElementStyle (Element el, int document)
#else
char *GetElementStyle (el, document)
Element el;
int document;
#endif
{
    /*
     * This is Ok to use a static variable, since it will be copied
     * to a String object on the way back to the Java V.M..
     * And since the Thot/Amaya access is available to only 1 thread
     * at a time ...
     */
    static char css[5000];
    int l;

    l = 5000;
    GetHTMLStyleString (el, document, &css[0], &l);
    return (&css[0]);
}

/*----------------------------------------------------------------------
  SetElementStyle : Set CSS style to an element. Remove any existing style
  on the element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void SetElementStyle (char *style, Element el, int document)
#else
void SetElementStyle (style, el, document)
char *style;
Element el;
int document;

#endif
{
    /* direct mapping of an internal function */
    RemoveElementStyle (el, document, FALSE);
    ParseHTMLSpecificStyle (el, style, document, FALSE);
}

/*----------------------------------------------------------------------
  AddElementStyle : Add CSS style to an element. This enrich the existing style
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void AddElementStyle (char *style, Element el, int document)
#else
void AddElementStyle (style, el, document)
char *style;
Element el;
int document;
#endif
{
    /* direct mapping of an internal function */
    ParseHTMLSpecificStyle (el, style, document, FALSE);
}

/*----------------------------------------------------------------------
  AmayaLoadHTMLDocument : Load an HTML Document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int AmayaLoadHTMLDocument (char *URL, int document)
#else
int AmayaLoadHTMLDocument (URL, document)
char *URL;
int document;
#endif
{
    Document res;
    
    res = GetHTMLDocument (URL, NULL, document, document, CE_ABSOLUTE, TRUE, NULL, NULL);

    return((int) res);
}

