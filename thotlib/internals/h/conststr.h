
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
 * Declarations de constantes pour les schemas de structure de
 * l'Editeur
 * V. Quint	Juin 1984
 *
 */

#define MAX_NAME_LENGTH 32		/* longueur maximum d'un nom */
#define MAX_RULES_SSCHEMA 350		/* nombre max. de regles dans un schema de
				   structure */
#define MAX_PARAM_SSCHEMA 10	/* nombre max de definitions de parametres
				   dans un schema de structure */
#define MAX_ATTR_SSCHEMA 200	/* nombre max. de definitions d'attributs
				   dans un schema  de structure */
#define MAX_INT_ATTR_VAL 32000	/* valeur maximum d'un attribut numerique */
#define MAX_OPTION_CASE 28		/* nombre max. de possibilites dans un choix*/
#define MAX_COMP_AGG 28		/* nombre max. de composants d'un agregat */
#define MAX_DEFAULT_ATTR 10		/* nombre max. d'attributs imposes dans une
				   regle de structure */
#define MAX_LOCAL_ATTR 25	/* nombre max. d'attributs locaux dans une
				   regle de structure */
#define MAX_MAND_ATTR 10		/* nombre max. d'attributs imposes dans une
				   regle de structure */
#define MAX_ATTR_VAL 20		/* nombre max. de valeurs possibles d'un
				   attribut */
#define MAX_LEN_ALL_CONST 512		/* longueur max. totale des chaines
				   constantes */

#define MAX_EXCEPT_SSCHEMA 350		/* nombre max. de numeros d'exception dans un
				   schema de structure */
#define MAX_INCL_EXCL_SRULE 5		/* nombre max. d'inclusions et d'exclusions
				   SGML dans une regle de structure */

#define MAX_EXTENS_SSCHEMA 20		/* nombre max. de regles d'extensions */

/* Constantes definissant les numeros d'exception */
#define ExcNoCut 10
#define ExcNoCreate 11
#define ExcNoHMove 12
#define ExcNoVMove 13
#define ExcNoHResize 14
#define ExcNoVResize 15
#define ExcNewWidth 16
#define ExcNewHeight 17
#define ExcNewHPos 18
#define ExcNewVPos 19
#define ExcInvisible 20
#define ExcNoMove 21
#define ExcNoResize 22
#define ExcNoSelect 23
#define ExcGraphCreation 24
#define ExcHidden 25
#define ExcPageBreak 26
#define ExcPageBreakAllowed 27
#define ExcPageBreakPlace 28
#define ExcPageBreakRepetition 29
#define ExcPageBreakRepBefore 30
#define ExcActiveRef 31
#define ExcCreateHolophrasted 32
#define ExcNoPaginate 33
#define ExcImportLine 34
#define ExcImportParagraph 35
#define ExcParagraphBreak 36
/* Draw exceptions */
#define ExcHighlightChildren 311
#define ExcExtendedSelection 312
#define ExcIsDraw 313
#define ExcMoveResize 314
/* SpellCheck exception */
#define ExcNoSpellCheck 400
/* 
  marques pour le codage des tables de structure produites par le
  compilateur du langage S.
*/

/* codage d'un type d'attribut */
#define C_INT_ATTR 'N'
#define C_TEXT_ATTR 'T'
#define C_REF_ATTR 'R'
#define C_ENUM_ATTR 'E'

/* codage d'un constructeur */
#define C_IDENTITY_CONSTR 'I'
#define C_LIST_CONSTR 'L'
#define C_CHOICE_CONSTR 'C'
#define C_AGG_CONSTR 'A'
#define C_UNORD_AGG_CONSTR 'S'
#define C_CONST_CONSTR 'O'
#define C_REF_CONSTR 'R'
#define C_BASIC_TYPE_CONSTR 'B'
#define C_NATURE_CONSTR 'X'

#define C_PAIR_CONSTR 'P'

#define C_EXTENS_CONSTR 'E'

/* codage d'un type de base */
#define C_CHAR_STRING 'C'
#define C_GRAPHICS 'G'
#define C_SYMBOL 'S'
#define C_PICTURE 'I'
#define C_REFER 'R'
#define C_PAGE_BREAK 'P'

#define C_UNUSED 'U'
