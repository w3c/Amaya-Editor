#ifndef _CONSTMENU_H_
#define _CONSTMENU_H_

/* ALLOCATION DES NUMEROS DE CATALOGUES					*/
/* Catalogues reserves aux fonctions standards de l'editeur Thot	*/
#define NumFormCreerDoc		22
#define NumZoneNomDocACreer	1
#define NumZoneDirDocACreer	2
#define NumSelClasseDocACreer	3
#define NumFormOuvrirDoc	4
#define NumZoneDirOuvrirDoc	5
#define NumSelDoc		6
#define NumZoneNomDocAOuvrir	7
#define NumFormClasseImport	8
#define NumSelectClasseImport	9
#define NumFormConfirm		10
#define NumLabelConfirm		11
#define NumMenuFormatImport	12
#define NumFormFormatImport	13

#define NumFormDirSchemas	14
#define NumZoneDirSchemas	15
#define NumFormDirDocuments	16
#define NumZoneDirDocuments	17

#define NumFormEspace		18
#define NumMenuSaisieEspace	19
#define NumMenuAffichageEspace	20
#define NumLabelEspace		21

#define NumZoneFichierCNRSe 	23
#define NumMenuTypeCNRSe 	24
#define NumMenuCadrageCNRSe 	25
#define NumZoneDirCNRSe 	26
#define NumSelCNRSe 		27
#define	NumFormCNRSe 		28
#define NumFormNature		29
#define NumSelectNomNature	30

#define NumFormLister		31
#define NumZoneNomFichierListe	32
#define NumMenuTypeListe	33

#define NumFormSchemaPresentation 34
#define NumZoneSchemaPresentation 35
#define NumMenuCreerElemReference 37
#define NumFormImage		38

#define NumFormSauverComme	40
#define NumZoneDirDocASauver	41
#define NumMenuFormatDocASauver 42
#define NumMenuCopierOuRenommer 43
#define NumZoneNomDocASauver	44
#define NumFormAutoSave		45
#define NumZoneAutoSave		46
#define NumFormImprimer		47
#define NumZoneDecaleHaut      	48
#define NumZoneDecaleGauche  	49
#define NumMenuOrientation  	49
#define NumMenuOptions	  	50
#define NumMenuVuesAImprimer 	51
#define NumMenuPagesParFeuille 	52
#define NumMenuSupport	 	53
#define NumZoneReduction 	54
#define NumMenuFormatReduit 	55
#define NumZonePremierePage 	56
#define NumZoneDernierePage 	57
#define NumZoneNbExemplaires 	58
#define NumZoneNomImprimante 	59
#define NumLabelEtoileVueImprimee 60
#define NumMenuFormatPapier 	61
#define NumFormatImpression	62
#define NumMenuSchPresent 	63
#define NumFormFermer		64
#define NumLabelSauverAvantFermer 65
#define NumMenuPresNature	66
#define NbMaxMenuPresNature	67 /* schema de presentation des natures */
#define NumMenuAttr    68	/* numero de la feuille de dialogue pour la */
                             	/* saisie des attributs */
#define NumMenuAttrNum 69   	/* numero de la zone de saisie de la valeur */
                             	/* d'un attribut numerique */
#define NumMenuAttrText 70  	/* numero de la zone de saisie de la valeur */
                             	/* d'un attribut textuel */
#define NumMenuAttrEnum 71	/* numero du menu des valeurs d'un attribut */
                             	/* enumere' */

#define NumMenuAttrRequis 72	/* numero du menu de saisie des attributs */
                              	/* obligatoires. */
                                /* ATTENTION: les 3 numeros suivants sont */
                                /* reserve's */
#define NumMenuAttrNumRequis 73 /* numero de la zone de saisie de la valeur */
#define NumMenuAttrTexteRequis 74 /* numero de la zone de saisie de la */
                         	/* valeur d'un attribut textuel obligatoire */
#define NumMenuAttrEnumRequis 75 /* numero du menu des valeurs d'un */
#define NumMenuCaracteres	76

#define NumMenuChoixEl		100
#define NumMenuInsert		102
#define NumMenuPaste		103
#define NumMenuInclude		104

#define NumMenuSurround 105  /* numero du menu Englobe */
#define NumMenuChangeType 106 /* numero du menu Changer_en */

#define NumMenuVuesAOuvrir	110
#define NumMenuVisibilite	111
#define NumTextVisibilite	112
#define NumDocVisibilite	113
#define NumMenuZoom		114
#define NumTextZoom		115
#define NumDocZoom		116

#define NumFormChercheTexte	120
#define NumMenuOuChercherTexte	121
#define NumZoneTexteCherche	122
#define NumZoneTexteRemplacement	123
#define NumMenuModeRemplacement	124
#define NumMenuChercherNature	125
#define NumSelTypeAChercher	126
#define NumSelAttributAChercher	127
#define NumLabelValeurAttribut	128
#define NumFormChercheElementVide	129
#define NumLabelDocChercheElVide	130
#define NumLabelDocChercheRefVide	131
#define NumToggleExpressionReguliere	132
#define NumMenuOuChercherElemVide	133
#define NumLabelElemVidePasTrouve	134
#define NumFormChercheReferenceVide	135
#define NumMenuOuChercherReferVide	136
#define NumLabelReferVidePasTrouve	137
#define NumFormCherchePage	138
#define NumZoneCherchePage	139
#define NumFormChercheReference	140
#define NumLabelChercheReference	141
#define NumLabelReferencePasTrouve	142
#define NumMenuChoixRefer	143

#define NumFormPresCaract	150
#define NumMenuFamilleCaract	151
#define NumMenuStyleCaract	152
#define NumMenuCorpsCaract	153
#define NumMenuTypeSouligne	154
#define NumMenuEpaisSouligne	155

#define NumFormPresFormat	157
#define NumMenuAlignement	158
#define	NumMenuJustification	159
#define NumZoneRenfoncement	160
#define NumMenuSensRenfoncement	161
#define NumZoneInterligne	162
#define NumMenuInterligne	163
#define NumMenuCoupureMots	164
#define NumFormPresGraphiques	165
#define NumMenuStyleTraits	166
#define NumZoneEpaisseurTraits	167
#define NumSelectPattern	168
#define NumFormCouleurs		169
#define NumSelectCouleurFond	170
#define NumSelectCouleurTrace	171
#define	NumFormPresentStandard	172
#define	NumMenuPresentStandard	173
#define NumToggleEpaisseurInchangee	174
#define NumTogglePatternInchange	175
#define NumToggleForegroundInchange	176
#define NumToggleBackgroundInchange	177

#define NumFormLangue		178
#define NumSelectLangue		179
#define NumLabelLangueHeritee	180
#define NumMenuNomAttr		181
#define MAX_LocalMenu		190

/* References des catalogues variables du dialogue de l'application	*/
/* (menu * MAX_FRAME + fenetre + MAX_LocalMenu	*/
#define MAX_MENU		20
#define MAX_ITEM		20
/* 1eres references produites par l'application				*/
/* MAX_MENU doit etre superieur a MAX_FRAME				*/
#define MAX_MenuThot	((MAX_MENU + 1) * MAX_ITEM * MAX_FRAME) + MAX_LocalMenu

#define maxentree 20
#define LgMaxMenu 20
#define LgMaxMenuChoix		20
#define LgMaxMenuInsert		20
#define LgMaxMenuSurround	25
#define LgMaxMenuChangeType	25
#define LgMaxMenuAttribut	25

#endif
