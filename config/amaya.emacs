# These functions are loosely based on emacs keys.
# You have to distinguish between <Alt/Control><Key> sequences and
# <Shift><Alt/Control><Key> sequences.
# For example:
# Alt <Key>+:		ZoomIn()	## is wrong
# Shift Alt <Key>+:	ZoomIn()	## is correct
# Alt <Key>=:	    ZoomIn()	## is correct
#
# Alt is used, with b,f,Home,r (for reload) and Return (for DoAction),
#             for navigating from page to page                     
# Alt-t is used for information types (Types that are in the Style menus)
#
# Ctrl- 1, 2, 3, 4, 5, 6 are used for headings.
# Ctrl- a, e, j, k are used for moving around
# Ctrl- c, d, w, y are used for cut, delete, copy, paste
# Ctrl-m is used for Math functions
# Ctrl-p is used for print
#
# ctrl-s is used for Style items
#
# Ctrl-t is used for Types in the types menu, and some table actions
# Ctrl-u is used for useful functions (currently only spellcheck,
#           but maybe also config and special functions later).
# Ctrl-v is used for view menus
# Ctrl-x is used for file functions
# Ctrl-z and Shift Ctrl-z are used for Undo/Redo
#
# Shift Ctrl- L, T, D and Shift-Alt-L are used for linking
# Shift Ctrl- B, I are used for strong and emphasis
# Shift Ctrl- N is for changing the title (Name...)
# Shift Ctrl- ! and S are used in the structure menu.
#
# Ctrl- Return, Space are used for BR and &nbsp;
#
# Ctrl-minus key is used for child element
#
# Ctrl- b, g, h, i, n, o, q, r are not used.
#

# personal keyboard file for Amaya

#moving around 
<Key>Delete:        TtcDeleteSelection() 
<Key>Backspace:     TtcDeletePreviousChar() 
<Key>F2:            TtcParentElement() 
<Key>Return:        TtcCreateElement() 
<Key>Home:          TtcStartOfLine() 
<Key>End:           TtcEndOfLine()
#Following commented out to remember default actions
#<Key>Up:           TtcPreviousLine() 
#<Key>Down:         TtcNextLine() 
#<Key>Right:        TtcNextChar() 
#<Key>Left:         TtcPreviousChar() 
#Ctrl <Key>Up:      TtcLineUp()
#Ctrl <Key>Down:    TtcLineDown()
<Key>L6:            TtcCopyToClipboard() 
<Key>L8:            TtcPasteFromClipboard() 
<Key>F6:            TtcCopyToClipboard() 
<Key>F8:            TtcPasteFromClipboard()
<Key>Tab:           NextLinkOrFormElement()
Shift <Key>Tab:     PreviousLinkOrFormElement()
Ctrl <Key>a:        TtcStartOfLine() 
Shift Ctrl <Key>A:  TtcSelStartOfLine() 
Ctrl <Key>e:        TtcEndOfLine() 
Shift Ctrl <Key>E:  TtcSelEndOfLine() 
Ctrl <Key>j:        TtcPreviousElement() 
Ctrl <Key>k:        TtcNextElement()
Ctrl <Key>-:        TtcChildElement()
Ctrl <Key>Home:     TtcPageTop() 
Ctrl <Key>End:      TtcPageEnd() 
Alt <Key>Return:    DoAction()
Alt <Key>Home:      GoToHome()
Ctrl <Key><:                                TtcInsertChar("\253")
Shift Ctrl <Key><:                          TtcInsertChar("\253")
Shift Ctrl <Key>>:                          TtcInsertChar("\273")
Alt <Key>e:                                 TtcInsertChar("&#8364;")

#File menu 
Ctrl <Key>p:                       PrintAs() 
Shift Ctrl <Key>P:                 SetupAndPrint() 
Ctrl <Key>x , Ctrl <Key>c:         AmayaClose()
Ctrl <Key>x , Ctrl <Key>k:         AmayaCloseWindow() 
Ctrl <Key>x , Ctrl <Key>t:         AmayaCloseTab() 
Ctrl <Key>x , Ctrl <Key>s:         SaveDocument() 
Ctrl <Key>x , Ctrl <Key>w:         SaveDocumentAs() 
Ctrl <Key>x , Ctrl <Key>z:         Synchronize()
Alt <Key>Left:                     GotoPreviousHTML() 
Alt <Key>Right:                    GotoNextHTML() 
<Key>F5:			   Reload() 

#edit menu 
Ctrl <Key>w:                       TtcCutSelection() 
Ctrl <Key>y:                       PasteBuffer() 
Ctrl <Key>z:                       TtcUndo() 
Shift Ctrl <Key>Z:                 TtcRedo() 
Ctrl <Key>f:                       TtcSearchText() 
Ctrl <Key>c:                       TtcCopySelection() 
Ctrl <Key>d:                       TtcDeleteSelection() 
#Shift Ctrl <Key>*:                SetBrowserEditor() 
<Key>F7:			   SpellCheck()

#Greek letters
Ctrl <Key>g , Ctrl <Key>a:               TtcInsertChar("&#x3B1;")
Ctrl <Key>g , Ctrl <Key>b:               TtcInsertChar("&#x3B2;")
Ctrl <Key>g , Ctrl <Key>c:               TtcInsertChar("&#x3B3;")
Ctrl <Key>g , Ctrl <Key>d:               TtcInsertChar("&#x3B4;")
Ctrl <Key>g , Ctrl <Key>e:               TtcInsertChar("&#x3B5;")
Ctrl <Key>g , Ctrl <Key>z:               TtcInsertChar("&#x3B6;")
Ctrl <Key>g , Ctrl <Key>h:               TtcInsertChar("&#x3B7;")
Ctrl <Key>g , Ctrl <Key>j:               TtcInsertChar("&#x3B8;")
Ctrl <Key>g , Ctrl <Key>i:               TtcInsertChar("&#x3B9;")
Ctrl <Key>g , Ctrl <Key>k:               TtcInsertChar("&#x3BA;")
Ctrl <Key>g , Ctrl <Key>l:               TtcInsertChar("&#x3BB;")
Ctrl <Key>g , Ctrl <Key>m:               TtcInsertChar("&#x3BC;")
Ctrl <Key>g , Ctrl <Key>n:               TtcInsertChar("&#x3BD;")
Ctrl <Key>g , Ctrl <Key>q:               TtcInsertChar("&#x3BE;")
Ctrl <Key>g , Ctrl <Key>o:               TtcInsertChar("&#x3BF;")
Ctrl <Key>g , Ctrl <Key>p:               TtcInsertChar("&#x3C0;")
Ctrl <Key>g , Ctrl <Key>r:               TtcInsertChar("&#x3C1;")
Ctrl <Key>g , Ctrl <Key>s:               TtcInsertChar("&#x3C3;")
Ctrl <Key>g , Ctrl <Key>t:               TtcInsertChar("&#x3C4;")
Ctrl <Key>g , Ctrl <Key>u:               TtcInsertChar("&#x3C5;")
Ctrl <Key>g , Ctrl <Key>f:               TtcInsertChar("&#x3C6;")
Ctrl <Key>g , Ctrl <Key>x                TtcInsertChar("&#x3C7;")
Ctrl <Key>g , Ctrl <Key>y:               TtcInsertChar("&#x3C8;")
Ctrl <Key>g , Ctrl <Key>w:               TtcInsertChar("&#x3C9;")

Ctrl <Key>g , Shift Ctrl <Key>A:   TtcInsertChar("&#x391;")
Ctrl <Key>g , Shift Ctrl <Key>B:   TtcInsertChar("&#x392;")
Ctrl <Key>g , Shift Ctrl <Key>C:   TtcInsertChar("&#x393;")
Ctrl <Key>g , Shift Ctrl <Key>D:   TtcInsertChar("&#x394;")
Ctrl <Key>g , Shift Ctrl <Key>E:   TtcInsertChar("&#x395;")
Ctrl <Key>g , Shift Ctrl <Key>Z:   TtcInsertChar("&#x396;")
Ctrl <Key>g , Shift Ctrl <Key>H:   TtcInsertChar("&#x397;")
Ctrl <Key>g , Shift Ctrl <Key>J:   TtcInsertChar("&#x398;")
Ctrl <Key>g , Shift Ctrl <Key>I:   TtcInsertChar("&#x399;")
Ctrl <Key>g , Shift Ctrl <Key>K:   TtcInsertChar("&#x39A;")
Ctrl <Key>g , Shift Ctrl <Key>L:   TtcInsertChar("&#x39B;")
Ctrl <Key>g , Shift Ctrl <Key>M:   TtcInsertChar("&#x39C;")
Ctrl <Key>g , Shift Ctrl <Key>N:   TtcInsertChar("&#x39D;")
Ctrl <Key>g , Shift Ctrl <Key>Q:   TtcInsertChar("&#x39E;")
Ctrl <Key>g , Shift Ctrl <Key>O:   TtcInsertChar("&#x39F;")
Ctrl <Key>g , Shift Ctrl <Key>P:   TtcInsertChar("&#x3A0;")
Ctrl <Key>g , Shift Ctrl <Key>R:   TtcInsertChar("&#x3A1;")
Ctrl <Key>g , Shift Ctrl <Key>S:   TtcInsertChar("&#x3A3;")
Ctrl <Key>g , Shift Ctrl <Key>T:   TtcInsertChar("&#x3A4;")
Ctrl <Key>g , Shift Ctrl <Key>U:   TtcInsertChar("&#x3A5;")
Ctrl <Key>g , Shift Ctrl <Key>F:   TtcInsertChar("&#x3A6;")
Ctrl <Key>g , Shift Ctrl <Key>X    TtcInsertChar("&#x3A7;")
Ctrl <Key>g , Shift Ctrl <Key>Y:   TtcInsertChar("&#x3A8;")
Ctrl <Key>g , Shift Ctrl <Key>W:   TtcInsertChar("&#x3A9;")

#SVG Graphics
Ctrl Shift <Key>G , Ctrl <Key>t:	CreateSVG_Text()

Ctrl Shift <Key>G , Ctrl Shift <Key>A:	CreateSVG_SimpleArrow()
Ctrl Shift <Key>G , Ctrl Shift <Key>C:	CreateSVG_Circle()
Ctrl Shift <Key>G , Ctrl Shift <Key>E:	CreateSVG_Ellipse()
Ctrl Shift <Key>G , Ctrl Shift <Key>D:	CreateSVG_Diamond()
Ctrl Shift <Key>G , Ctrl Shift <Key>F:	CreateSVG_ForeignObject()
Ctrl Shift <Key>G , Ctrl Shift <Key>G:	CreateSVG_Group()
Ctrl Shift <Key>G , Ctrl Shift <Key>L:	CreateSVG_Line()
Ctrl Shift <Key>G , Ctrl Shift <Key>M:	CreateSVG_Polyline()
Ctrl Shift <Key>G , Ctrl Shift <Key>N:	CreateSVG_Closed()
Ctrl Shift <Key>G , Ctrl Shift <Key>O:	CreateSVG_Spline()
Ctrl Shift <Key>G , Ctrl Shift <Key>P:	CreateSVG_Polygon()
Ctrl Shift <Key>G , Ctrl Shift <Key>R:	CreateSVG_Rectangle()
Ctrl Shift <Key>G , Ctrl Shift <Key>S:	CreateSVG_Square()
Ctrl Shift <Key>G , Ctrl Shift <Key>T:	CreateSVG_IsoscelesTriangle()

#Headings sbmenu
Shift Ctrl <Key>1:                 CreateHeading1() 
Shift Ctrl <Key>2:                 CreateHeading2() 
Shift Ctrl <Key>3:                 CreateHeading3() 
Shift Ctrl <Key>4:                 CreateHeading4() 
Shift Ctrl <Key>5:                 CreateHeading5() 
Shift Ctrl <Key>6:                 CreateHeading6() 
Ctrl <Key>1:                       CreateHeading1() 
Ctrl <Key>2:                       CreateHeading2() 
Ctrl <Key>3:                       CreateHeading3() 
Ctrl <Key>4:                       CreateHeading4() 
Ctrl <Key>5:                       CreateHeading5() 
Ctrl <Key>6:                       CreateHeading6()
# BR and &nbsp;
Ctrl <Key>Return:                  CreateBreak("\212") 
Ctrl <Key>Space:                   TtcInsertChar("\240")

#Types 
Shift <Key>Return:                 CreateParagraph() 
Ctrl <Key>h , Ctrl <Key>a:         CreateAddress() 
Ctrl <Key>h , Ctrl <Key>c:         CreateComment() 
Ctrl <Key>h , Ctrl <Key>d:         CreateDefinitionList() 
Ctrl <Key>h , Ctrl <Key>h:         CreateHorizontalRule() 
Ctrl <Key>h , Ctrl <Key>i:         CreateImage() 
Ctrl <Key>h , Ctrl <Key>l:         CreateList() 
Ctrl <Key>h , Ctrl <Key>m:         CreateMap()
Ctrl <Key>h , Ctrl <Key>n:         CreateNumberedList() 
Ctrl <Key>h , Ctrl <Key>o:         CreateObject() 
Ctrl <Key>h , Ctrl <Key>p:         CreatePreformatted() 
Ctrl <Key>h , Ctrl <Key>q:         CreateBlockQuote() 
Ctrl <Key>h , Ctrl <Key>r:         CreateRuby() 
Ctrl <Key>h , Ctrl <Key>t:         ChangeTitle()
Ctrl <Key>h , Ctrl <Key>v:         CreateDivision() 
Ctrl <Key>h , Ctrl <Key>w:         CreateParameter() 

#Style menu and Information types
Ctrl <Key>i , Ctrl <Key>a:         ApplyClass() 
Ctrl <Key>i , Ctrl <Key>b:         ChangeBackgroundImage() 
Ctrl <Key>i , Ctrl <Key>c:         CreateClass() 
Ctrl <Key>i , Ctrl <Key>d:                 SetOnOffDEL()
#Ctrl <Key>i , Ctrl <Key>d:         DisableCSS()
#Ctrl <Key>i , Ctrl <Key>e:         EnableCSS()
Ctrl <Key>i , Ctrl <Key>e:                 SetOnOffEmphasis() 
Ctrl <Key>i , Ctrl <Key>f:         TtcChangeFormat() 
Ctrl <Key>i , Ctrl <Key>h:         TtcChangeCharacters() 
Ctrl <Key>i , Ctrl <Key>i:                 SetOnOffINS() 
Ctrl <Key>i , Ctrl <Key>k:                 SetOnOffKeyboard() 
Ctrl <Key>i , Ctrl <Key>l:         LinkCSS()
Ctrl <Key>i , Ctrl <Key>m:                 SetOnOffSample() 
Ctrl <Key>i , Ctrl <Key>o:         OpenCSS() 
Ctrl <Key>i , Ctrl <Key>p:         TtcChangeColors() 
Ctrl <Key>i , Ctrl <Key>q:                 SetOnOffQuotation() 
Ctrl <Key>i , Ctrl <Key>r:         RemoveCSS() 
Ctrl <Key>i , Ctrl <Key>s:                 SetOnOffStrong() 
Ctrl <Key>i , Ctrl <Key>t:                 SetOnOffCode() 
Ctrl <Key>i , Ctrl <Key>u:                 SetOnOffAbbr() 
Ctrl <Key>i , Ctrl <Key>v:                 SetOnOffVariable() 
Ctrl <Key>i , Ctrl <Key>w:                 SetOnOffCite() 
Ctrl <Key>i , Ctrl <Key>x:                 SetOnOffDefinition() 
Ctrl <Key>i , Ctrl <Key>y:                 SetOnOffAcronym() 
Ctrl <Key>i , Ctrl <Key>z:                 SetOnOffBDO() 

#Links and SvgLibrary
Ctrl <Key>l , Ctrl <Key>a:         AddNewModelIntoLibrary()
Ctrl <Key>l , Ctrl <Key>d:         DeleteAnchor() 
Ctrl <Key>l , Ctrl <Key>f:	   AddNewModelIntoLibraryForm()
Ctrl <Key>l , Ctrl <Key>l:         CreateOrChangeLink() 
Ctrl <Key>l , Ctrl <Key>p:         LinkToPreviousTarget() 
Ctrl <Key>l , Ctrl <Key>s:	   ShowLibrary()
Ctrl <Key>l , Ctrl <Key>t:         CreateTarget()

#Math (types submenu)
Ctrl <Key>m , Ctrl <Key>Space:     CreateMSPACE()
Ctrl <Key>m , Ctrl <Key>6:         CreateMSUP()
Ctrl <Key>m , Ctrl <Key>a:         CreateApplyFunction()
Ctrl <Key>m , Ctrl <Key>b:         CreateMSUBSUP()
Ctrl <Key>m , Ctrl <Key>c:         SetOnOffChemistry()
Ctrl <Key>m , Ctrl <Key>d:         CreateMI()
Ctrl <Key>m , Ctrl <Key>e:         CreateMathEntity()
Ctrl <Key>m , Ctrl <Key>f:         CreateMFRAC()
Ctrl <Key>m , Ctrl <Key>g:         CreateMO()
Ctrl <Key>m , Ctrl <Key>h:         CreateMTABLE()
Ctrl <Key>m , Ctrl <Key>i:         CreateInvisibleTimes()
Ctrl <Key>m , Ctrl <Key>j:         CreateMPIECEWISE()
Ctrl <Key>m , Ctrl <Key>k:         CreateMUNDEROVER()
Ctrl <Key>m , Ctrl <Key>l:         CreateMROW()
Ctrl <Key>m , Ctrl <Key>m:         CreateMath()
Ctrl <Key>m , Ctrl <Key>n:         CreateMN()
Ctrl <Key>m , Ctrl <Key>o:         CreateMOVER()
Ctrl <Key>m , Ctrl <Key>p:         CreateMPARENTHESIS()
Ctrl <Key>m , Ctrl <Key>q:         CreateMSQRT()
Ctrl <Key>m , Ctrl <Key>r:         CreateMROOT()
Ctrl <Key>m , Ctrl <Key>s:         CreateMMULTISCRIPTS()
Ctrl <Key>m , Ctrl <Key>t:         CreateMMATRIX()
Ctrl <Key>m , Ctrl <Key>u:         CreateMUNDER()
Ctrl <Key>m , Ctrl <Key>v:         CreateMSUB()
Ctrl <Key>m , Ctrl <Key>x:         CreateMTEXT()

#Annotations, Bookmarks and New
Ctrl <Key>n , Ctrl <Key>a:	   AnnotateDocument()
Ctrl <Key>n , Ctrl <Key>b:	   BookmarkFile()
Ctrl <Key>n , Ctrl <Key>c:         NewCss() 
Ctrl <Key>n , Ctrl <Key>d:	   DeleteAnnotation()
Ctrl <Key>n , Ctrl <Key>e:	   AddSeparator()
Ctrl <Key>n , Ctrl <Key>f:	   FilterAnnot()
Ctrl <Key>n , Ctrl <Key>g:         NewSVG() 
Ctrl <Key>n , Ctrl <Key>h:         NewXHTML() 
Ctrl <Key>n , Ctrl <Key>i:	   MoveItem()
Ctrl <Key>n , Ctrl <Key>k:         NewXHTMLBasic() 
Ctrl <Key>n , Ctrl <Key>l:	   LoadAnnotations()
Ctrl <Key>n , Ctrl <Key>m:         NewMathML() 
Ctrl <Key>n , Ctrl <Key>o:	   ConfigAnnot()
Ctrl <Key>n , Ctrl <Key>p:	   PostAnnotation()
Ctrl <Key>n , Ctrl <Key>r:	   ReplyToAnnotation()
Ctrl <Key>n , Ctrl <Key>s:	   AnnotateSelection()
Ctrl <Key>n , Ctrl <Key>t:	   EditTopics()
Ctrl <Key>n , Ctrl <Key>u:         NewXHTMLStrict() 
Ctrl <Key>n , Ctrl <Key>v:	   ViewBookmarks()
Ctrl <Key>n , Ctrl <Key>x:         NewXHTML11() 

#Forms (types submenu)
Ctrl <Key>o , Ctrl <Key>a:         CreateTextArea()
Ctrl <Key>o , Ctrl <Key>b:         CreatePushButton()
Ctrl <Key>o , Ctrl <Key>c:         CreateFieldset()
Ctrl <Key>o , Ctrl <Key>e:        	    TtcInsertChar("&#339;")
Ctrl <Key>o , Ctrl <Key>f:         CreateForm()
Ctrl <Key>o , Ctrl <Key>g:         CreateOptGroup()
Ctrl <Key>o , Ctrl <Key>h:         CreateHiddenInput()
Ctrl <Key>o , Ctrl <Key>i:         CreateTextInput()
Ctrl <Key>o , Ctrl <Key>l:         CreateLabel()
Ctrl <Key>o , Ctrl <Key>m:         CreateImageInput()
Ctrl <Key>o , Ctrl <Key>n:         CreateOption()
Ctrl <Key>o , Ctrl <Key>o:         OpenDoc() 
Ctrl <Key>o , Ctrl <Key>p:         CreatePasswordInput()
Ctrl <Key>o , Ctrl <Key>r:         CreateRadio()
Ctrl <Key>o , Ctrl <Key>s:         CreateSubmit()
Ctrl <Key>o , Ctrl <Key>t:         CreateToggle()
Ctrl <Key>o , Ctrl <Key>u:         CreateFileInput()
Ctrl <Key>o , Ctrl <Key>w:         OpenDocInNewWindow() 
Ctrl <Key>o , Ctrl <Key>x:         CreateReset()
Shift Ctrl <Key>O , Shift Ctrl <Key>E:      TtcInsertChar("&#338;")

#Transform and tables submenu
Ctrl <Key>t , Ctrl <Key>a:         CreateColumnAfter()
Ctrl <Key>t , Ctrl <Key>b:         CreateColumnBefore()
Ctrl <Key>t , Ctrl <Key>c:         SelectColumn()
Ctrl <Key>t , Ctrl <Key>d:         ChangeToDataCell()
Ctrl <Key>t , Ctrl <Key>e:         CellHorizExtend()
Ctrl <Key>t , Ctrl <Key>h:         ChangeToHeadingCell()
Ctrl <Key>t , Ctrl <Key>i:         CreateRowBefore() 
Ctrl <Key>t , Ctrl <Key>j:         CellVertExtend()
Ctrl <Key>t , Ctrl <Key>l:         CreateCaption()
Ctrl <Key>t , Ctrl <Key>m:         CellVertShrink() 
Ctrl <Key>t , Ctrl <Key>n:         CreateRowAfter() 
Ctrl <Key>t , Ctrl <Key>p:         PasteBefore() 
Ctrl <Key>t , Ctrl <Key>r:         SelectRow() 
Ctrl <Key>t , Ctrl <Key>s:         CellHorizShrink()
Ctrl <Key>t , Ctrl <Key>t:         CreateTable() 
Ctrl <Key>t , Ctrl <Key>v:         PasteAfter() 
Ctrl <Key>t , Ctrl <Key>x:         TransformType()

#View Menu
<Key>F11:                          FullScreen()
<Key>F12:                          ShowPanel()
Ctrl <Key>v , Ctrl <Key>a:         ShowAddress()
Ctrl <Key>v , Ctrl <Key>b:         ShowButtons()
Ctrl <Key>v , Ctrl <Key>c:         ShowToC() 
Ctrl <Key>v , Ctrl <Key>f:         ShowFormatted()
Ctrl <Key>v , Ctrl <Key>h:         SplitHorizontally()
Ctrl <Key>v , Ctrl <Key>i:         ShowAlternate() 
Ctrl <Key>v , Ctrl <Key>l:         ShowLinks() 
Ctrl <Key>v , Ctrl <Key>m:         ShowMapAreas() 
Ctrl <Key>v , Ctrl <Key>o:         ShowSource()
Ctrl <Key>v , Ctrl <Key>s:         ShowStructure()
Ctrl <Key>v , Ctrl <Key>t:         ShowTargets() 
Ctrl <Key>v , Ctrl <Key>v:         SplitVertically()
Alt <Key>=:        ZoomIn() 
Alt <Key>-:        ZoomOut()
#Alt <Key>+:        ZoomIn()
#Alt <Key>1:        ZoomIn()

#Try to override bizzare behaviour
Shift <Key>End:     TtcSelEndOfLine()
Shift <Key>Home:    TtcSelStartOfLine()
