/* constind.h : Declaration des constantes d'index */
/* H. Richy - Juin 91 - corrige Thot janvier 1996  */

/* Constantes pour la construction des tables d'index */
#define MaxNiveau 3               /* nombre max de cles dans un descripteur*/
#define Lg_Cle 50                 /* taille max d'une cle */
#define NbCle 200                 /* nb max d'entrees a trier */

/* Constantes pour definir le tri */
#define NbLtrTri 150              /* nb max de caracteres differents */
#define MaxHomo 3                 /* nb max d'homographes d'une meme cle */
 
/* Constantes pour trier les references d'une entree */
#define MaxRefCle1     50          /* nb max references */



#define IndexMenuQuoi       1
#define IndexMenuPagination 2
#define IndexMenuOptions    3
#define IndexFormConstruire 4
#define MAX_MENU_INDEX 5
