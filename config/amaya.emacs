# Keyboard configuration file for Unix
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
<Key>Backspace:    TtcDeletePreviousChar() 
<Key>Escape:        TtcParentElement() 
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
Ctrl <Key>p:        TtcPreviousLine() 
Ctrl <Key>n:        TtcNextLine() 
Ctrl <Key>a:        TtcStartOfLine() 
Ctrl <Key>e:        TtcEndOfLine()
Ctrl <Key>f:        TtcNextChar() 
Ctrl <Key>b:        TtcPreviousChar()
Ctrl <Key>j:        TtcPreviousElement() 
Ctrl <Key>k:        TtcNextElement()
Ctrl <Key>-:        TtcChildElement()
Ctrl <Key>Home:     TtcPageTop() 
Ctrl <Key>End:      TtcPageEnd()
Ctrl <Key>v:        TtcPageDown()
Alt <Key>v:         TtcPageUp()
Alt <Key>Return:    DoAction() 
Alt <Key>Home:      GoToHome()
Alt <Key>Left:      TtcPageTop()
Alt <Key>Right:     TtcPageEnd()

#File menu 
Shift Ctrl <Key>N:                 ChangeTitle()
Ctrl <Key>x , Ctrl <Key>f:         OpenDoc() 
Ctrl <Key>x , Ctrl <Key>o:         OpenDocInNewWindow() 
Ctrl <Key>x , Ctrl <Key>n:         NewXHTML() 
Ctrl <Key>x , Ctrl <Key>m:         NewMathML() 
Ctrl <Key>x , Ctrl <Key>g:         NewSVG() 
Ctrl <Key>x , Ctrl <Key>t:         NewTemplate() 
Ctrl <Key>s , Ctrl <Key>n:         NewCss() 
Alt <Key>p:                        PrintAs() 
Shift Ctrl <Key>P:                 SetupAndPrint() 
Ctrl <Key>x , Ctrl <Key>s:         SaveDocument() 
Ctrl <Key>x , Ctrl <Key>w:         SaveDocumentAs() 
Shift Alt <Key>B:                  GotoPreviousHTML() 
Shift Alt <Key>F:                  GotoNextHTML() 
Alt <Key>r:                        Reload() 
Ctrl <Key>x , Ctrl <Key>k:         CloseDocument() 
Ctrl <Key>x , Ctrl <Key>c:         AmayaClose()
hift Alt <Key>V , Alt <Key>x:         TtcCloseView()
Ctrl <Key>x , Ctrl <Key>z:         Synchronize()

#edit menu 
Ctrl <Key>w:                       TtcCutSelection() 
Ctrl <Key>y:                       TtcPaste() 
Ctrl <Key>z:                       TtcUndo() 
Shift Ctrl <Key>Z:                 TtcRedo() 
Shift Alt <Key>%:                  TtcSearchText() 
Ctrl <Key>c:                       TtcCopySelection() 
Ctrl <Key>d:                       TtcDeleteSelection() 
Shift Ctrl <Key>*:                 SetBrowserEditor() 
Ctrl <Key>t , Ctrl <Key>x:         TransformType()
Ctrl <Key>u , Ctrl <Key>s:         SpellCheck()
 
#Types 
Shift <Key>Return:                 CreateParagraph() 
Ctrl <Key>t , Ctrl <Key>w:         CreatePreformatted() 
Ctrl <Key>t , Ctrl <Key>i:         CreateImage() 
Ctrl <Key>t , Ctrl <Key>-:         CreateHorizontalRule() 
Ctrl <Key>t , Ctrl <Key>a:         CreateAddress() 
Ctrl <Key>t , Ctrl <Key>q:         CreateBlockQuote() 
Ctrl <Key>t , Ctrl <Key>v:         CreateDivision() 
# BR and &nbsp;
Ctrl <Key>Return:                  CreateBreak("\212") 
Ctrl <Key>Space:                   TtcInsertChar("\240")

#lists - types submenu
#Don't need items - these are done with return
Ctrl <Key>l , Ctrl <Key>d:         CreateDefinitionList() 
Ctrl <Key>l , Ctrl <Key>o:         CreateNumberedList() 
Ctrl <Key>l , Ctrl <Key>u:         CreateList() 

#tables submenu
Ctrl <Key>t , Ctrl <Key>t:         CreateTable() 
Ctrl <Key>t , Ctrl <Key>h:         CreateHeadingCell()
Ctrl <Key>t , Ctrl <Key>Delete:    DeleteColumn()

#Object submenu
Ctrl <Key>t , Ctrl <Key>o:         CreateObject() 
Ctrl <Key>t , Ctrl <Key>p:         CreateParameter() 

#Headings sbmenu
Ctrl <Key>1:                       CreateHeading1() 
Ctrl <Key>2:                       CreateHeading2() 
Ctrl <Key>3:                       CreateHeading3() 
Ctrl <Key>4:                       CreateHeading4() 
Ctrl <Key>5:                       CreateHeading5() 
Ctrl <Key>6:                       CreateHeading6()

#Forms (types submenu)
Alt <Key>f , Alt <Key>f:           CreateForm() 
Alt <Key>f , Alt <Key>s:           CreateSubmit() 
Alt <Key>f , Alt <Key>x:           CreateReset() 
Alt <Key>f , Alt <Key>t:           CreateTextArea() 
Alt <Key>f , Alt <Key>i:           CreateTextInput() 
Alt <Key>f , Alt <Key>r:           CreateRadio() 
Alt <Key>f , Alt <Key>n:           CreateOptGroup() 
Alt <Key>f , Alt <Key>c:           CreateToggle() 
Alt <Key>f , Alt <Key>o:           CreateFileInput() 
Alt <Key>f , Alt <Key>d:	   CreateImageInput()
Alt <Key>f , Alt <Key>p:           CreatePasswordInput() 
Alt <Key>f , Alt <Key>h:           CreateHiddenInput() 
Alt <Key>f , Alt <Key>g:           CreateFieldset() 
Alt <Key>f , Alt <Key>l:           CreateLabel() 
Alt <Key>f , Alt <Key>b:           CreatePushButton() 
Alt <Key>f , Alt <Key>m:           CreateOption() 

#Math (types submenu)
Ctrl <Key>m , Ctrl <Key>6:         CreateMSUP() 
Ctrl <Key>m , Ctrl <Key>=:         CreateMO()
Ctrl <Key>m , Ctrl <Key>a:         CreateApplyFunction()
Ctrl <Key>m , Ctrl <Key>b:         CreateMSUBSUP() 
Ctrl <Key>m , Ctrl <Key>c:         CreateMENCLOSE()
Ctrl <Key>m , Ctrl <Key>d:         CreateMI()
Ctrl <Key>m , Ctrl <Key>e:         CreateMathEntity()
Ctrl <Key>m , Ctrl <Key>f:         CreateMFRAC() 
Ctrl <Key>m , Ctrl <Key>i:         CreateInvisibleTimes()
Ctrl <Key>m , Ctrl <Key>k:         CreateMUNDEROVER() 
Ctrl <Key>m , Ctrl <Key>m:         CreateMath() 
Ctrl <Key>m , Ctrl <Key>n:         CreateMN()
Ctrl <Key>m , Ctrl <Key>o:         CreateMOVER() 
Ctrl <Key>m , Ctrl <Key>p:         CreateMROW() 
Ctrl <Key>m , Ctrl <Key>q:         CreateMSQRT() 
Ctrl <Key>m , Ctrl <Key>r:         CreateMROOT() 
Ctrl <Key>m , Ctrl <Key>s:         CreateMMULTISCRIPTS()
Ctrl <Key>m , Ctrl <Key>t:         CreateMTABLE() 
Ctrl <Key>m , Ctrl <Key>u:         CreateMUNDER() 
Ctrl <Key>m , Ctrl <Key>v:         CreateMSUB() 
Ctrl <Key>m , Ctrl <Key>x:         CreateMTEXT()

#Structure 
Shift Ctrl <Key>!:                 CreateComment() 
Shift Ctrl <Key>S:                 CreateStyle()
 
#Links
Shift Alt <Key>L:                  LinkToPreviousTarget() 
Shift Ctrl <Key>L:                 CreateOrChangeLink() 
Shift Ctrl <Key>D:                 DeleteAnchor() 
Shift Ctrl <Key>T:                 CreateTarget()

#View Menu
Shift Alt <Key>V , Alt <Key>a:           TtcSwitchCommands() 
Shift Alt <Key>V , Alt <Key>b:           TtcSwitchButtonBar() 
Shift Alt <Key>V , Alt <Key>c:           ShowToC() 
Shift Alt <Key>V , Alt <Key>i:           ShowAlternate() 
Shift Alt <Key>V , Alt <Key>l:           ShowLinks() 
Shift Alt <Key>V , Alt <Key>m:           ShowMapAreas() 
Shift Alt <Key>V , Alt <Key>h:           ShowSource()
Shift Alt <Key>V , Alt <Key>s:           ShowStructure()
Shift Alt <Key>V , Alt <Key>t:           ShowTargets() 
Alt <Key>=:        ZoomIn() 
Alt <Key>-:        ZoomOut()
 
#Style menu 
Ctrl <Key>s , Ctrl <Key>c:         CreateClass() 
Ctrl <Key>s , Ctrl <Key>a:         ApplyClass() 
Ctrl <Key>s , Ctrl <Key>o:         OpenCSS() 
Ctrl <Key>s , Ctrl <Key>r:         RemoveCSS() 
Ctrl <Key>s , Ctrl <Key>t:         TtcChangeFormat() 
Ctrl <Key>s , Ctrl <Key>h:         TtcChangeColors() 
Ctrl <Key>s , Ctrl <Key>f:         TtcChangeCharacters() 
Ctrl <Key>s , Ctrl <Key>b:         ChangeBackgroundImage() 
Ctrl <Key>s , Ctrl <Key>l:         LinkCSS()
Ctrl <Key>s , Ctrl <Key>e:         EnableCSS()
Ctrl <Key>s , Ctrl <Key>d:         DisableCSS()

#Information types (in Style menu) 
Shift Ctrl <Key>B:                 SetOnOffStrong() 
Shift Ctrl <Key>I:                 SetOnOffEmphasis() 
Alt <Key>t , Alt <Key>Delete:      SetOnOffDEL() 
Alt <Key>t , Alt <Key>i:           SetOnOffINS() 
Alt <Key>t , Alt <Key>a:           SetOnOffAbbr() 
Alt <Key>t , Alt <Key>z:           SetOnOffAcronym() 
Alt <Key>t , Alt <Key>q:           SetOnOffQuotation() 
Alt <Key>t , Alt <Key>c:           SetOnOffCode() 
Alt <Key>t , Alt <Key>w:           SetOnOffCite() 
Alt <Key>t , Alt <Key>d:           SetOnOffDefinition() 
Alt <Key>t , Alt <Key>v:           SetOnOffVariable() 
Alt <Key>t , Alt <Key>s:           SetOnOffSample() 
Alt <Key>t , Alt <Key>k:           SetOnOffKeyboard() 
Alt <Key>t , Alt <Key>o:           SetOnOffBDO() 

#Annotations
Alt <Key>n , Alt <Key>s:	   AnnotateSelection()
Alt <Key>n , Alt <Key>a:	   AnnotateDocument()
Alt <Key>n , Alt <Key>p:	   PostAnnotation()
Alt <Key>n , Alt <Key>d:	   DeleteAnnotation()
Alt <Key>n , Alt <Key>l:	   LoadAnnotations()
Alt <Key>n , Alt <Key>f:	   FilterAnnot()
Alt <Key>n , Alt <Key>o:	   ConfigAnnot()
