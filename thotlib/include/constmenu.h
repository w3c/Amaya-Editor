/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _CONSTMENU_H_
#define _CONSTMENU_H_

/* ALLOCATION DES NUMEROS DE CATALOGUES					*/
/* Catalogues reserves aux fonctions standards de l'editeur Thot	*/
#define NumFormLanguage		1
#define NumSelectLanguage	2
#define NumLabelHeritedLanguage	3
#define NumMenuAlphaLanguage    4
#define NumMenuAttrName		5
#define NumMenuElChoice		6
#define NumMenuInsert		7
#define NumMenuPaste		8
#define NumMenuInclude		9
#define	NumFormPresentStandard	10
#define	NumMenuPresentStandard	11
#define NumToggleWidthUnchanged	12
#define NumTogglePatternUnchanged	13
#define NumFormColors		14
#define NumSelectBackgroundColor	15
#define NumSelectForegroundColor	16
#define NumFormPresFormat	17
#define NumFormSpace		18
#define NumMenuInputSpace	19
#define NumMenuDisplaySpace	20
#define NumLabelSpace		21
#define NumMenuAlignment	22
#define NumZoneFileCNRSe 	23
#define NumMenuTypeCNRSe 	24
#define NumMenuFrameCNRSe 	25
#define NumZoneDirCNRSe 	26
#define NumSelCNRSe 		27
#define	NumFormCNRSe 		28
#define NumFormNature		29
#define NumSelectNatureName	30
#define NumFormPresChar		31
#define NumMenuCharFamily	32
#define NumMenuCharFontStyle	33
#define NumMenuCharFontWeight	34
#define NumMenuCharFontSize	35
#define NumMenuUnderlineType	36
#define NumMenuUnderlineWeight	37
#define NumFormSearchText	38
#define NumMenuOrSearchText	39
#define NumZoneTextSearch	40
#define NumZoneTextReplace	41
#define NumMenuReplaceMode	42
#define NumMenuSearchNature	43
#define NumSelTypeToSearch	44
#define NumSelAttributeToSearch	45
#define NumToggleUpperEqualLower  46

#define NumFormClose		47
#define NumLabelSaveBeforeClosing 48
#define NumMenuPresNature	49

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
#define NumFormElemToBeCreated  97
#define NumSelectElemToBeCreated 98

#define NumZoneRecess		100
#define NumMenuRecessSense	101
#define NumZoneLineSpacing	102
#define NumMenuLineSpacing	103
#define NumFormPresGraphics	104
#define NumMenuStrokeStyle	105
#define NumZoneStrokeWeight	106
#define NumSelectPattern	107

#define MAX_LocalMenu		108

/* Number of references reserved by Thot       				*/
/* MAX_ITEM >= MAX_FRAME for X-Windows applications			*/
/* MAX_ITEM >= MAX_FRAME+MAX_MENU for Windows applications   		*/
#define MAX_MENU		30 /* max number of entries and menus   */
#define MAX_ITEM		40
#define MAX_ThotMenu	((MAX_MENU + 1) * MAX_MENU * MAX_ITEM) + MAX_LocalMenu

/* Speel Checker menu */
#define ChkrMenuOR			1
#define ChkrCaptureNC			2
#define ChkrLabelLanguage		3
#define ChkrMenuIgnore			4
#define ChkrSpecial			5
#define ChkrSelectProp			6
#define ChkrFormCorrect		        7
#define ChkrFormNC			8
#define ChkrLabelDocument	       	9
#define ChkrLabelNotFound		10
#define ChkrMaxDialogue			11


#endif
