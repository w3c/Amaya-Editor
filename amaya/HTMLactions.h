#ifdef __STDC__
extern void DisplayImage(Document doc, Element el, char *imageName);
extern void FetchImage(Document doc, Element el);
extern void UpdateTitle(Element el, Document doc);
extern void UpdateContextSensitiveMenus (Document doc);

#else /* __STDC__*/
extern void DisplayImage(/*Document doc, Element el, char *imageName*/);
extern void FetchImage(/*Document doc, Element el*/);
extern void UpdateTitle(/* Element el, Document doc */);
extern void UpdateContextSensitiveMenus (/*Document doc*/);
#endif
