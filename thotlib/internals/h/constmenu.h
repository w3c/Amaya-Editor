/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
#ifndef _CONSTMENU_H_
#define _CONSTMENU_H_

/* ALLOCATION DES NUMEROS DE CATALOGUES					*/
/* Catalogues reserves aux fonctions standards de l'editeur Thot	*/
#define NumFormCreateDoc		22
#define NumZoneDocNameToCreate	1
#define NumZoneDocDirToCreate	2
#define NumSelDocClassToCreate	3
#define NumFormOpenDoc	4
#define NumZoneDirOpenDoc	5
#define NumSelDoc		6
#define NumZoneDocNameToOpen	7
#define NumFormImportClass	8
#define NumSelectImportClass	9
#define NumFormConfirm		10
#define NumLabelConfirm		11
#define NumMenuImportFormat	12
#define NumFormImportFormat	13

#define NumFormDirSchemas	14
#define NumZoneDirSchemas	15
#define NumFormDirDocuments	16
#define NumZoneDirDocuments	17

#define NumFormSpace		18
#define NumMenuInputSpace	19
#define NumMenuDisplaySpace	20
#define NumLabelSpace		21

#define NumZoneFileCNRSe 	23
#define NumMenuTypeCNRSe 	24
#define NumMenuFrameCNRSe 	25
#define NumZoneDirCNRSe 	26
#define NumSelCNRSe 		27
#define	NumFormCNRSe 		28
#define NumFormNature		29
#define NumSelectNatureName	30

#define NumFormList		31
#define NumZoneFileNameList	32
#define NumMenuTypeList	33

#define NumFormPresentationSchema 34
#define NumZonePresentationSchema 35
#define NumMenuCreateReferenceElem 37
#define NumFormImage		38

#define NumFormSaveAs	40
#define NumZoneDirDocToSave	41
#define NumMenuFormatDocToSave 42
#define NumMenuCopyOrRename 43
#define NumZoneDocNameTooSave	44
#define NumFormAutoSave		45
#define NumZoneAutoSave		46
#define NumFormPrint		47
#define NumZoneScrollUp      	48
#define NumZoneScrollLeft  	49
#define NumMenuOrientation  	49
#define NumMenuOptions	  	50
#define NumMenuViewsToPrint 	51
#define NumMenuNbPagesPerSheet 	52
#define NumMenuSupport	 	53
#define NumZoneReduction 	54
#define NumZoneFirstPage 	55
#define NumZoneLastPage 	56
#define NumZoneNbOfCopies 	57
#define NumZonePrinterName 	58
#define NumEmptyLabel1	 	59
#define NumEmptyLabel2          60
#define NumMenuPaperFormat 	61
#define NumPrintingFormat	62
#define NumMenuSchPresent 	63
#define NumFormClose		64
#define NumLabelSaveBeforeClosing 65
#define NumMenuPresNature	66
#define NbMaxMenuPresNature	20 /* schema de presentation des natures */
#define NumMenuAttr    88	/* numero de la feuille de dialogue pour la */
                             	/* saisie des attributs */
#define NumMenuAttrNumber 89   	/* numero de la zone de saisie de la valeur */
                             	/* d'un attribut numerique */
#define NumMenuAttrText 90  	/* numero de la zone de saisie de la valeur */
                             	/* d'un attribut textuel */
#define NumMenuAttrEnum 91	/* numero du menu des valeurs d'un attribut */
                             	/* enumere' */

#define NumMenuAttrRequired 92	/* numero du menu de saisie des attributs */
                              	/* obligatoires. */
                                /* ATTENTION: les 3 numeros suivants sont */
                                /* reserve's */
#define NumMenuAttrNumNeeded 93 /* numero de la zone de saisie de la valeur */
#define NumMenuAttrTextNeeded 94 /* numero de la zone de saisie de la */
                         	/* valeur d'un attribut textuel obligatoire */
#define NumMenuAttrEnumNeeded 95 /* numero du menu des valeurs d'un */
#define NumMenuCharacters	96

#define NumMenuElChoice		100
#define NumMenuInsert		102
#define NumMenuPaste		103
#define NumMenuInclude		104

#define NumMenuSurround 105  /* numero du menu Englobe */
#define NumMenuChangeType 106 /* numero du menu Changer_en */

#define NumMenuViewsToOpen	110
#define NumMenuVisibility	111
#define NumTextVisibility	112
#define NumDocVisibility	113
#define NumMenuZoom		114
#define NumTextZoom		115
#define NumDocZoom		116

#define NumFormSearchText	120
#define NumMenuOrSearchText	121
#define NumZoneTextSearch	122
#define NumZoneTextReplace	123
#define NumMenuReplaceMode	124
#define NumMenuSearchNature	125
#define NumSelTypeToSearch	126
#define NumSelAttributeToSearch	127
#define NumLabelAttributeValue	128
#define NumFormSearchEmptyElement	129
#define NumLabelDocSearcheEmptyElement	130
#define NumLabelDocSearchEmptyRef	131
#define NumToggleUpperEqualLower	132
#define NumMenuOrSearchEmptyElem	133
#define NumLabelEmptyElemNotFound	134
#define NumFormSearchEmptyReference	135
#define NumMenuOrSearchEmptyReference	136
#define NumLabelEmptyRefereneceNotFound	137
#define NumFormSearchPage	138
#define NumZoneSearchPage	139
#define NumFormSearchReference	140
#define NumLabelSearchReference	141
#define NumLabelReferenceNotFound	142
#define NumMenuReferenceChoice	143

#define NumFormPresChar	150
#define NumMenuCharFamily	151
#define NumMenuStyleChar	152
#define NumMenuCharFontSize	153
#define NumMenuUnderlineType	154
#define NumMenuUnderlineWeight	155

#define NumFormPresFormat	157
#define NumMenuAlignment	158
#define	NumMenuJustification	159
#define NumZoneRecess	160
#define NumMenuRecessSense	161
#define NumZoneLineSpacing	162
#define NumMenuLineSpacing	163
#define NumMenuWordBreak	164
#define NumFormPresGraphics	165
#define NumMenuStrokeStyle	166
#define NumZoneStrokeWeight	167
#define NumSelectPattern	168
#define NumFormColors		169
#define NumSelectBackgroundColor	170
#define NumSelectForegroundColor	171
#define	NumFormPresentStandard	172
#define	NumMenuPresentStandard	173
#define NumToggleWidthUnchanged	174
#define NumTogglePatternUnchanged	175
#define NumToggleForegroundUnchanged	176
#define NumToggleBackgroundUnchanged	177

#define NumFormLanguage		178
#define NumSelectLanguage		179
#define NumLabelHeritedLanguage	180
#define NumMenuAlphaLanguage    181
#define NumMenuAttrName		182
#define MAX_LocalMenu		190

/* Number of references reserved by Thot       				*/
/* MAX_ITEM >= MAX_FRAME for X-Windows applications			*/
/* MAX_ITEM >= MAX_FRAME+MAX_MENU for Windows applications   		*/
#define MAX_MENU		30
#define MAX_ITEM		40
#define MAX_ThotMenu	((MAX_MENU + 1) * MAX_MENU * MAX_ITEM) + MAX_LocalMenu

#define MAX_ENTRIES 20
#define LgMaxMenu 20
#define LgMaxChoiceMenu		20
#define LgMaxInsertMenu		20
#define LgMaxSurroundMenu	25
#define LgMaxChangeTypeMenu	25
#define LgMaxAttributeMenu	25

#endif
