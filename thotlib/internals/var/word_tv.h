/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* convert table for alphabets */
#ifndef __WORD_VAR_H__
#define __WORD_VAR_H__

short _cType_[1 + 256] = {
        0,
        _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
        _C,     _C|_S,  _C|_S,  _C|_S,  _C|_S,  _C|_S,  _C,     _C,
        _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
        _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
        _S|_B,  _P,     _P,     _P,     _P,     _P,     _P,     _P,
        _P,     _P,     _P,     _P,     _P,     _P,     _P,     _P,
        _N,     _N,     _N,     _N,     _N,     _N,     _N,     _N,
        _N,     _N,     _P,     _P,     _P,     _P,     _P,     _P,
        _P,     _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U,
        _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U,
        _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U,
        _U,     _U,     _U,     _P,     _P,     _P,     _P,     _P,
        _P,     _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L,
        _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L,
        _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L,
        _L,     _L,     _L,     _P,     _P,     _P,     _P,     _C,
        0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,
        0,      0,      0,      0,      0,      0,      0,      0,
        _F|_H,  _H,     _F|_H,  _H,     _H,     _H,     _H,     _F|_H,
        _F|_H,  _F|_H,  _F|_H,  _F|_H,  _H,     _H,     _F|_H,  _F|_H,
        _H,     _H,     _H,     _H,     _F|_H,  _H,     _H,     _F|_H,
        _H,     _F|_H,  _H,     _F|_H,  _F|_H,  _H,     _H,     _H,
        _G|_I,  _I,     _G|_I,  _I,     _I,     _I,     _I,     _G|_I,
        _G|_I,  _G|_I,  _G|_I,  _G|_I,  _I,     _I,     _G|_I,  _G|_I,
        _I,     _I,     _I,     _I,     _G|_I,  _I,     _I,     _G|_I,
        _I,     _G|_I,  _I,     _G|_I,  _G|_I,  _I,     _I,     _I,
};

#endif /* #ifdef __WORD_VAR_H__ */
