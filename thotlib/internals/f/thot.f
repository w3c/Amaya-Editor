
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int IdentDocument ( PtrDocument pDoc );
/* extern boolean SendAttributeMessage ( int *notifyAttr, boolean pre ); */
/* extern boolean ThotSendMessage ( int *notifyEvent, boolean pre ); */
extern void MotifCreerDocument ( void );
extern void MotifOuvrirDocument ( void );
extern void MotifQuitterThot ( void );
extern void MotifDossierSchemas ( void );
extern void MotifDossierDocuments ( void );
extern void MyMainLoop ( void );
extern void MyCallback ( int ref, int typedata, int data );
extern int main ( int argc, char **argv );

#else /* __STDC__ */

extern int IdentDocument (/* PtrDocument pDoc */);
extern void MotifCreerDocument (/* void */);
extern void MotifOuvrirDocument (/* void */);
extern void MotifQuitterThot (/* void */);
extern void MotifDossierSchemas (/* void */);
extern void MotifDossierDocuments (/* void */);
extern void MyMainLoop (/* void */);
extern void MyCallback (/* int ref, int typedata, int data */);
extern int main (/* int argc, char **argv */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
