
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
 * Constantes pour la traduction des documents
 * V. Quint	Janvier 1988
 *
 */

#define MAX_TRANSL_COUNTER 30		/* nombre max. de compteurs pour une
				   traduction */
#define MAX_TRANSL_CONST 2048	/* nombre max. de constantes pour traduction*/
#define MAX_TRANSL_CONST_LEN 24000	/* longueur max. totale des constantes de
				   traduction */
#define MAX_TRANSL_VARIABLE 50		/* nombre max. de variables pour une
				   traduction */
#define MAX_TRANSL_VAR_ITEM 12	/* nombre max d'elements constituant une var.
				   de trad. */
#define MAX_TRANSL_BUFFER 10	/* nombre max. de buffer pour une traduction*/
#define MAX_TRANSL_BUFFER_LEN 80		/* longueur maximum des buffers */
#define MAX_TRANSL_ALPHABET 4		/* nombre max. d'alphabets traduits */
#define MAX_TRANSL_CHAR 512		/* nombre max. d'entrees de la table de
				   traduction des caracteres */
#define MAX_SRCE_LEN 15		/* longueur max. des chaines de caracteres a
				   traduire */
#define MAX_TARGET_LEN 128		/* longueur max. des chaines de caracteres
				   traduites */
#define MAX_TRANSL_ATTR_CASE 5	/* nombre max de cas differents pour
				   l'application des regles de traduction
				   d'un attribut numerique */

#define MAX_TRANSL_COND 10		/* nombre max. de conditions qui controlent
				   l'application d'un bloc de regles de trad*/

#define MAX_TRANSL_PRULE 30		/* nombre de types de regles de presentation
				   auxquels on peut associer des traductions*/

#define MAX_TRANSL_PRES_CASE 20	/* nombre max de cas differents pour
				   l'application des regles de traduction
				   d'une presentation a valeur numerique */

#define MAX_TRANSL_PRES_VAL 5		/* nombre max. de valeurs differentes des
				   presentations non numeriques */

#define MAX_EOL_STRING_LEN  16 /* longuer max de la chaine de caracteres marquant
				     la fin de la ligne dans le fichier traduit */

	/* definition du codage des tables de traduction produites par le
	compilateur du langage T. */

	/* un type de condition d'application de regle de traduction */

#define C_TR_FIRST 'P'
#define C_TR_LAST 'D'
#define C_TR_DEFINED 'd'
#define C_TR_REFERRED 'R'
#define C_TR_FIRSTREF 'F'
#define C_TR_LAST_REF 'L'
#define C_TR_WITHIN 'T'
#define C_TR_FIRST_WITHIN 'f'
#define C_TR_ATTRIBUTE 'A'
#define C_TR_PRESENT 'S'
#define C_TR_PRULE 's'
#define C_TR_COMMENT 'c'
#define C_TR_ALPHABET 'a'
#define C_TR_ATTRIBUTES 'E'
#define C_TR_FIRSTATTR 'p'
#define C_TR_LASTATTR 'l'
#define C_TR_COMPUTEDPAGE 'w'
#define C_TR_STARTPAGE 'x'
#define C_TR_USERPAGE 'y'
#define C_TR_REMINDERPAGE 'z'
#define C_TR_EMPTY 'v'
#define C_TR_EXTERNALREF 'e'

	/* un type de regle de traduction */
#define C_TR_CREATE 'C'
#define C_TR_GET 'G'
#define C_TR_USE 'U'
#define C_TR_REMOVE 'R'
#define C_TR_WRITE 'W'
#define C_TR_READ 'r'
#define C_TR_INCLUDE 'I'
#define C_TR_NOTRANSL 'N'

#define C_TR_COPY 'c'
#define C_TR_CHANGEFILE 'F'
#define C_TR_SET_COUNTER 'S'
#define C_TR_ADD_COUNTER 'A'
#define C_TR_NOLINEBREAK 'B'

	/* position ou il faut creer les chaines produites par le traducteur*/
#define C_TR_AFTER 'A'
#define C_TR_BEFORE 'B'

	/* type de chose a creer dans le fichier de sortie ou au terminal */
#define C_OB_CONST 'C'
#define C_OB_BUFFER 'B'
#define C_OB_VAR 'V'
#define C_OB_ATTR 'A'
#define C_OB_CONTENT 'D'
#define C_OB_PRES_VAL 'P'
#define C_OB_PRESENTATION 'p'
#define C_OB_COMMENT 'c'
#define C_OB_ATTRIBUTES 'T'
#define C_OB_REFID 'R'

#define C_OB_PAIRID 'M'

#define C_OB_REFERRED_ELEM 'r'

#define C_OB_FILEDIR 'd'
#define C_OB_FILENAME 'f'
#define C_OB_EXTENSION 'E'
#define C_OB_DOCUMENTNAME 'N'
#define C_OB_DOCUMENTDIR 'I'
#define C_OB_REFERRED_DOCNAME 'n'
#define C_OB_REFERRED_DOCDIR 'i'
#define C_OB_REFERRED_REFID 'F'

	/* position relative dans l'arbre abstrait d'un element a prendre */
#define C_TR_SIBLING 'F'
#define C_TR_DESCEND 'D'
#define C_TR_REFERRED_EL 'd'
#define C_TR_ASSOC 'A'

	/* type d'une operation sur un compteur */
#define C_TR_RANK 'R'
#define C_TR_RLEVEL 'L'
#define C_TR_SET 'S'

#define C_TR_NOOP 'N'

	/* types des elements de variables de traduction */
#define C_TR_CONST 'T'
#define C_TR_COUNTER 'C'
#define C_TR_BUFFER 'B'
#define C_TR_ATTR 'A'

#define C_TR_FILEDIR 'd'
#define C_TR_FILENAME 'F'
#define C_TR_EXTENSION 'E'
#define C_TR_DOCUMENTNAME 'N'
#define C_TR_DOCUMENTDIR 'D'

	/* pour interpreter le champ TcAscendLevel */
#define C_WITHIN_GT 'S'
#define C_WITHIN_LT 'I'
#define C_WITHIN_EQ 'E'
