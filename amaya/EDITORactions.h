#ifndef _EDITOR_ACTIONS_F
#define _EDITOR_ACTIONS_F

#ifdef __STDC__
extern void         New (Document document, View view);
extern void         CreateIsIndex (Document document, View view);
extern void         CreateLinkInHead (Document document, View view);
extern void         CreateBase (Document document, View view);
extern void         CreateMeta (Document document, View view);
extern void         CreateComment (Document document, View view);
extern void         CreateParagraph (Document document, View view);
extern void         CreateHeading1 (Document document, View view);
extern void         CreateHeading2 (Document document, View view);
extern void         CreateHeading3 (Document document, View view);
extern void         CreateHeading4 (Document document, View view);
extern void         CreateHeading5 (Document document, View view);
extern void         CreateHeading6 (Document document, View view);
extern void         CreateList (Document document, View view);
extern void         CreateNumberedList (Document document, View view);
extern void         ThotCreateMenu (Document document, View view);
extern void         ThotCreateDirectory (Document document, View view);
extern void         CreateDefinitionList (Document document, View view);
extern void         CreateDefinitionTerm (Document document, View view);
extern void         CreateDefinitionDef (Document document, View view);
extern void         CreateHorizontalRule (Document document, View view);
extern void         CreateBlockQuote (Document document, View view);
extern void         CreatePreformatted (Document document, View view);
extern void         CreateAddress (Document document, View view);
extern void         CreateTable (Document document, View view);
extern void         CreateCaption (Document document, View view);
extern void         CreateTHead (Document document, View view);
extern void         CreateTBody (Document document, View view);
extern void         CreateTFoot (Document document, View view);
extern void         CreateRow (Document document, View view);
extern void         CreateDataCell (Document document, View view);
extern void         CreateHeadingCell (Document document, View view);
extern void         CreateForm (Document document, View view);
extern void         CreateToggleMenu (Document document, View view);
extern void         CreateRadioMenu (Document document, View view);
extern void         CreateOptionMenu (Document document, View view);
extern void         CreateTextInput (Document document, View view);
extern void         CreateTextArea (Document document, View view);
extern void         CreateCommandLine (Document document, View view);
extern void         SetAreaCoords (Document document, Element element);
extern void         CreateAreaRect (Document doc, View view);
extern void         CreateAreaCircle (Document doc, View view);
extern void         CreateAreaPoly (Document doc, View view);
extern void         CreateDivision (Document document, View view);
extern void         CreateApplet (Document document, View view);
extern void         CreateParameter (Document document, View view);
extern void         ChangeLink (Document doc, View view);
extern void         DeleteAnchor (Document doc, View view);

#else  /* __STDC__ */
extern void         New ();
extern void         CreateIsIndex ();
extern void         CreateLinkInHead ();
extern void         CreateBase ();
extern void         CreateMeta ();
extern void         CreateComment ();
extern void         CreateParagraph ();
extern void         CreateHeading1 ();
extern void         CreateHeading2 ();
extern void         CreateHeading3 ();
extern void         CreateHeading4 ();
extern void         CreateHeading5 ();
extern void         CreateHeading6 ();
extern void         CreateList ();
extern void         CreateNumberedList ();
extern void         ThotCreateMenu ();
extern void         ThotCreateDirectory ();
extern void         CreateDefinitionList ();
extern void         CreateDefinitionTerm ();
extern void         CreateDefinitionDef ();
extern void         CreateHorizontalRule ();
extern void         CreateBlockQuote ();
extern void         CreatePreformatted ();
extern void         CreateAddress ();
extern void         CreateTable ();
extern void         CreateCaption ();
extern void         CreateTHead ();
extern void         CreateTBody ();
extern void         CreateTFoot ();
extern void         CreateRow ();
extern void         CreateDataCell ();
extern void         CreateHeadingCell ();
extern void         CreateForm ();
extern void         CreateToggleMenu ();
extern void         CreateRadioMenu ();
extern void         CreateOptionMenu ();
extern void         CreateTextInput ();
extern void         CreateTextArea ();
extern void         CreateCommandLine ();
extern void         SetAreaCoords ();
extern void         CreateAreaRect ();
extern void         CreateAreaCircle ();
extern void         CreateAreaPoly ();
extern void         CreateDivision ();
extern void         CreateApplet ();
extern void         CreateParameter ();
extern void         ChangeLink ();
extern void         DeleteAnchor ();

#endif /* __STDC__ */

#endif /* _EDITOR_ACTIONS_F */
