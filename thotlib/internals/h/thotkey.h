
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef _THOTKEY_H_
#define _THOTKEY_H_

#define SAUT_DE_LIGNE 138
#define SeeCtrlRC 182 /* visualisation SAUT_DE_LIGNE */
#define FINE 129
#define SeeFine 96 /* visualisation FINE */
#define DEMI_CADRATIN 130
#define SeeDemiCadratin 166 /* visualisation DEMI_CADRATIN */
#define BLANC_DUR 160
#define SeeBlancDur 185 /* visualisation BLANC_DUR */
#define BLANC 32
#define SeeBlanc 183 /* visualisation BLANC */

#define F_AGRAVE '\340'
#define F_AAIGU '\341'
#define F_ACIRC '\342'
#define F_ATILDE '\343'
#define F_ATREMA '\344'
#define F_AROND '\345'
#define F_AE '\346'

#define F_EGRAVE '\350'
#define F_EAIGU '\351'
#define F_ECIRC '\352'
#define F_ETREMA '\353'

#define F_IGRAVE '\354'
#define F_IAIGU '\355'
#define F_ICIRC '\356'
#define F_ITREMA '\357'

#define F_OGRAVE '\362'
#define F_OAIGU '\363'
#define F_OCIRC '\364'
#define F_OTILDE '\365'
#define F_OTREMA '\366'

#define F_OE '\367'

#define F_UGRAVE '\371'
#define F_UAIGU '\372'
#define F_UCIRC '\373'
#define F_UTREMA '\374'

#define F_CCEDIL '\347'

#define F_YAIGU '\375'
#define F_YTREMA '\377'

#define F_NTILDE '\361'

#define F_DEGRE_S "\260"

#define F_SECTION "\247"

#define F_STERLING "\243"

#define F_MU "\265"


#define F_TREMA_C '\250'
#define F_DEGRE_C '\260'

#ifdef RS
#define F_GRAVE_C '`'
#else
#define F_GRAVE_C '\`'
#endif

#define F_AIGU_C  '\''
#define F_CIRC_C  '^'
#define F_TILDE_C '~'
#define F_COMMA_C ','
#define F_E_C     'e'
#define F_COLON_C ':'
#define F_SEMI_C  ';'
#define F_SIX_C  '6'

#define ESCAPE_C  '\33'
#define LINEFEED_C  '\n'
#define CARRIAGE_C  '\r'
#define HARDSPACE_C '\207'

#endif
