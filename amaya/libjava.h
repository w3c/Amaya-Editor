/*
 * libjava.h : interface exported by the Java interface to
 *             amaya.
 */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void CloseJava ( void );
extern void StopRequest ( int doc );
extern int GetObjectWWW ( int doc,
			  char *urlName,
			  char *postString,
			  char *outputfile,
			  int mode,
			  void *incremental_cbf,
			  void *context_icbf,
			  void *terminate_cbf,
			  void *context_tcbf,
			  boolean error_html );
extern int PutObjectWWW ( int doc,
                          char *fileName,
                          char *urlName,
                          int mode,
                          PicType contentType,
                          void *terminate_cbf,
                          void *context_tcbf );

#else /* __STDC__ */

extern void CloseJava (/* void */);
extern void StopRequest (/* int doc */);
extern int GetObjectWWW (/* int doc,
                            char *urlName,
                            char *postString,
                            char *outputfile,
                            int mode,
                            void *incremental_cbf,
                            void *context_icbf,
                            void *terminate_cbf,
                            void *context_tcbf,
                            boolean error_html */);
extern int PutObjectWWW (/* int doc,
                            char *fileName,
                            char *urlName,
                            int mode,
                            PicType contentType,
                            void *terminate_cbf,
                            void *context_tcbf */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */


