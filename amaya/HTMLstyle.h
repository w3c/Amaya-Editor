#ifndef __HTML_STYLE_H__
#define __HTML_STYLE_H__

#include "amaya.h"
#include "genericdriver.h"

#ifdef __STDC__
extern char         CSSparser (AmayaReadChar readfunc, Document doc);
extern char        *GetHTML3Name (Element elem, Document doc);
extern int          GetHTML3Names (Element elem, Document doc, char **lst, int max);
extern void         UpdateStyleDelete (NotifyAttribute * event);
extern void         UpdateStylePost (NotifyAttribute * event);
extern void         GetHTML3StyleString (Element elem, Document doc, char *buf, int *len);
extern void         ParseHTMLStyleDecl (PresentationTarget target, PresentationContext context, char *attrstr);
extern void         ParseHTMLSpecificStyle (Element elem, char *attrstr, Document doc);
extern char        *ParseHTMLGenericSelector (char *selector, char *attrstr, GenericContext ctxt, Document doc, PSchema gPres);
extern void         ParseHTMLGenericStyle (char *selector, char *attrstr, Document doc, PSchema gPres);
extern void         ParseHTMLStyleDeclaration (Element elem, char *attrstr, Document doc, PSchema gPres);
extern void         ParseHTMLClass (Element elem, char *attrstr, Document doc);
extern void         ParseHTMLStyleSheet (char *fragment, Document doc, PSchema gPres);
extern int          EvaluateClassContext (Element elem, char *class, char *selector, Document doc);
extern int          EvaluateClassSelector (Element elem, char *class, char *selector, Document doc);
extern Element      CreateWWWElement (Document doc, int type);
extern Element      CreateNewWWWElement (Document doc, int type);
extern int          IsImplicitClassName (char *class, Document doc);
extern Element      SearchClass (char *class, Document doc);
extern void         ApplyStyleRule (Element elem, Element stylerule, Document doc);
extern void         RemoveStyleRule (Element elClass, Document doc);
extern void         RemoveStyle (Element elem, Document doc);
extern void         SetHTMLStyleParserDestructiveMode (Bool mode);
extern void         HTMLSetBackgroundColor (Document doc, Element elem, char *color);
extern void         HTMLSetForegroundColor (Document doc, Element elem, char *color);
extern void         HTMLResetBackgroundColor (Document doc, Element elem);
extern void         HTMLResetForegroundColor (Document doc, Element elem);
extern void         HTMLSetAlinkColor (Document doc, char *color);
extern void         HTMLSetAactiveColor (Document doc, char *color);
extern void         HTMLSetAvisitedColor (Document doc, char *color);
extern void         HTMLResetAlinkColor (Document doc);
extern void         HTMLResetAactiveColor (Document doc);
extern void         HTMLResetAvisitedColor (Document doc);

#else  /* __STDC__ */
extern char         CSSparser ();
extern char        *GetHTML3Name ();
extern int          GetHTML3Names ();
extern void         UpdateStyleDelete ();
extern void         UpdateStylePost ();
extern void         GetHTML3StyleString ();
extern void         ParseHTMLStyleDecl ();
extern void         ParseHTMLSpecificStyle ();
extern char        *ParseHTMLGenericSelector ();
extern void         ParseHTMLGenericStyle ();
extern void         ParseHTMLStyleDeclaration ();
extern void         ParseHTMLClass ();
extern void         ParseHTMLStyleSheet ();
extern int          EvaluateClassContext ();
extern int          EvaluateClassSelector ();
extern Element      CreateWWWElement ();
extern Element      CreateNewWWWElement ();
extern int          IsImplicitClassName ();
extern Element      SearchClass ();
extern void         ApplyStyleRule ();
extern void         RemoveStyleRule ();
extern void         RemoveStyle ();
extern void         SetHTMLStyleParserDestructiveMode ();
extern void         HTMLSetBackgroundColor ();
extern void         HTMLSetForegroundColor ();
extern void         HTMLResetBackgroundColor ();
extern void         HTMLResetForegroundColor ();
extern void         HTMLSetAlinkColor ();
extern void         HTMLSetAactiveColor ();
extern void         HTMLSetAvisitedColor ();
extern void         HTMLResetAlinkColor ();
extern void         HTMLResetAactiveColor ();
extern void         HTMLResetAvisitedColor ();

#endif /* __STDC__ */

#endif /* __HTML_STYLE_H__ */
