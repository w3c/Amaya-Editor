
/* -- Copyright (c) 1990 - 1994 Inria/Imag  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean PalSelectPre ( NotifyElement *event );
extern void PalActivate ( NotifyElement *event );
extern boolean ChargerPalette ( void );
extern void OuvrirPalette ( Document doc, View view );
extern void FermerPalette ( Document doc, View view );

#else /* __STDC__ */

extern boolean PalSelectPre (/* NotifyElement *event */);
extern void PalActivate (/* NotifyElement *event */);
extern boolean ChargerPalette (/* void */);
extern void OuvrirPalette (/* Document doc, View view */);
extern void FermerPalette (/* Document doc, View view */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
