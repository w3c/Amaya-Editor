/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/** typeind.h **/

typedef int       *PtrTabInd;  /* pointeur sur une liste d entiers a trier */
typedef char      *PtrTabCle;  /* pointeur sur une liste de cles a trier */

typedef struct _Tab_tri *PtrTabTri;

typedef struct _Tab_tri
{
  char       cle[MaxNiveau+1][Lg_Cle];
  int        pres;    /* attribut sur le terme */
  int        page;    /* -1 si renvoi */
  int        pagefin; /* !=0 si intervalle */
  int        note;   /* 0 ou numero de note */
  int        homo;   /* != 0 pour les homographes (avec semantique) */
  PtrElement refdesc; /* vers Elem_Desc ou Elem_Crois */
  PtrElement refmarque; /* vers Marque debut */
  PtrDocument doc; /* vers le Document contenant le Refdesc */

} Tab_tri;

typedef enum
{
  ALPHA = 1, ALPHANUM, ALPHATYPO, ALPHAPERSO

}  TAlgotri;

typedef enum
{
  PMAJUSCULE = 1, PMINUSCULE, P1CAPITALE, PCAPITALE, PTELQUEL

}  TPresTerme;

