#ifndef __HTML_FORM_F__
#define __HTML_FORM_F__

#ifdef __STDC__
extern void         SelectOneOption (Document doc, Element element);
extern void         SelectCheckbox (Document doc, Element element);
extern void         SelectOneRadio (Document doc, Element element);
extern void         SubmitForm (Document doc, Element element);

#else  /* __STDC__ */
extern void         SelectOneOption ();
extern void         SelectCheckbox ();
extern void         SelectOneRadio ();
extern void         SubmitForm ();

#endif /* __STDC__ */

#endif /* __HTML_FORM_F__ */
