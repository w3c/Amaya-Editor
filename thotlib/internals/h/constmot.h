/* constmot.h : Declaration des constantes du module de decoupage en mots */

/* Constantes pour les mots 8 bits (Eric Picheral)  */

/* types de caracteres (sur 8 bits) :
    U majuscule anglaise    L minuscule anglaise
    N numerique
    S espace, CR, NL, FF, TAB
    P ponctuation anglaise
    C caractere de commande
    X hexa maj ou min (a-f A-F)
    B
    F majuscule francaise   G minuscule francaise
    H majuscule ISO-Latin1  I minuscule ISO-Latin1
*/
#ifndef _U
#define	_U	01
#endif
#ifndef _L
#define	_L	02
#endif
#ifndef _N
#define	_N	04
#endif
#ifndef _S
#define	_S	010
#endif
#ifndef _P
#define _P	020
#endif
#ifndef _C
#define _C	040
#endif
#ifndef _X
#define _X	0100
#endif
#ifndef _B
#define	_B	0200
#endif
#ifndef _F
#define _F	0400
#endif
#ifndef _G
#define _G	01000
#endif
#ifndef _H
#define _H	02000
#endif
#ifndef _I
#define _I	04000
#endif

#define isfmaj(c)	((_cType_+1)[(unsigned char)c]&(_F|_U))
#define isfmin(c)	((_cType_+1)[(unsigned char)c]&(_G|_L))
#define isimin(c)	((_cType_+1)[(unsigned char)c]&(_I|_L))

#define isimaj(c)       ((_cType_+1)[(unsigned char)c]&(_H|_U))
#define isalphiso(c)    ((_cType_+1)[(unsigned char)c]&(_H|_I|_U|_L))
/*
#define toupper(c)      ((c)-'a'+'A')
#define tolower(c)      ((c)-'A'+'a')
*/
