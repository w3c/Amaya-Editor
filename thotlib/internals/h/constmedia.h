
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
 * Les constantes du mediateur.
 * I. Vatton - Juillet 87
 *
 *
 */

#include "constint.h"
#include "thotkey.h"

/* Constantes pour la gestion des chaines de caracteres */
#define NUL 0
#define MAX_WORD_LEN       30		/* Long. max d'un mot */

/* constante pour la gestion des regles en attente lors de la */
/* construction de l'image abstaite (partagee entre crimabs et crpages) */
#define MAX_QUEUE_LEN 10

/* Constantes pour la gestion des images */
#define FULL_BUFFER 99		/* Nombre max effectif caracteres dans buff */
#define MAX_RELAT_POS 10		/* Nombre dependances de positions par bloc */
#define MAX_RELAT_DIM 5		/* Nombre dependances de dimensions par bloc*/
#define MAX_ALPHABET 3		/* Nombre max d'alphabets utilises          */
#define MAX_FONT_STYLE 3		/* Nombre max de styles de polices utilises */
#define	HANDLE_WIDTH 5			/* Largeur des poingnees de la selection    */
#define DELTA_SEL 6              /* Precision de la selection selection      */
#define EXTRA_GRAPH 10		/* debordement des boites graphiques        */

/*iso*/
#define MAX_HIGHLIGHT 8		/* Nombre max de niveaux de mise en evidence*/
#define MAX_LOG_SIZE 30		/* Taille logique maximale                  */
#define MAX_TAILLE 127		/* Taille absolue maximale                  */
#define MAX_FRAME 10		/* Nombre maximum de Vues affichables       */
#define MAX_SIZE 65535		/* Taille max des structures systeme        */
#define MAX_UNDERLINE 4		/* Nombre max de styles de souligne utilises*/
#define MAX_UNDER_THICK 2		/* Epaisseur maximale du souligne   */

/* Constantes pour la gestion des catalogues */
#define MAX_TXT_LEN 1024		/* Longueur maximum des chaines rendues     */
#define MAX_ENTRY 40		/* Nombre maximum d'entrees d'un catalogue  */
#define INTEGER 0
#define STRING 1
#define CLICK 2
#define EMPTY 3
#define ENUMERATE 4
#define PRODUCT 5
#define UNION 6
#define OTHER   7
#define SHIFT_16 65536
#define CAT_IMAGE 2000
#define CAT_ZOOM 2001
#define CAT_VISIB 2002
#define CAT_OPTION 2003

/* Constantes pour gerer les commandes et les boutons */
#define MAX_NCOMMANDS 15
#define MAX_ITEM_MSG_SEL 16
#define TEXT_INSERT 5
#define TEXT_CUT 6
#define TEXT_PASTE 7
#define TEXT_COPY 8
#define TEXT_DEL 9
#define TEXT_X_PASTE 10
