/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _THOTLIB_CONSTMOT_H_
#define _THOTLIB_CONSTMOT_H_

/* Constants for 8 bits words (Eric Picheral)  */

/* character types (8 bits) :
    U english capital letter
    L english small letter
    N numerique
    S space, CR, NL, FF, TAB
    P english punctuation
    C command character
    X hexa capital or small letter (a-f A-F)
    B
    F french capital letter
    G french small letter
    H ISO-Latin1 capital letter
    I ISO-Latin1 small letter
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

#endif /* _THOTLIB_CONSTMOT_H_ */
