#ifndef AHTURL_TOOLS_F
#define AHTURL_TOOLS_F

#ifdef __STDC__
extern boolean      IsHTMLName (char *path);
extern boolean      IsImageName (char *path);
extern boolean      IsTextName (char *path);
extern boolean      IsHTTPPath (char *path);
extern boolean      IsWithParameters (char *path);
extern boolean      IsW3Path (char *path);
extern boolean      IsValidProtocol (char *path);
extern void         NormalizeURL (char *orgName, Document doc, char *newName, char *docName);
extern boolean      IsValidNormalizeURL (char *path);
extern char        *AHTMakeRelativeName (char *url, char *base_ur);

#else  /* __STDC__ */
extern boolean      IsHTMLName ();
extern boolean      IsImageName ();
extern boolean      IsTextName ();
extern boolean      IsHTTPPath ();
extern boolean      IsWithParameters ();
extern boolean      IsW3Path ();
extern boolean      IsValidProtocol ();
extern void         NormalizeURL ();
extern boolean      IsValidNormalizeURL ();
extern char        *AHTMakeRelativeName ();

#endif /* __STDC__ */


#endif /* AHTURL_TOOLS_F */
