/*
 * libilu.h : interface exported by the ILU interface to
 *             Amaya.
 *
 * Daniel Veillard 1997
 */

#ifndef __LIBILU_H__
#define __LIBILU_H__

/*
 * Note : ILU MANDATE an Ansi C compiler.
 */

extern void ILUserver_Initialize(void);
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
                          ThotBool error_html,
                          char *content_type );
extern int PutObjectWWW ( int doc,
                          char *fileName,
                          char *urlName,
                          int mode,
                          PicType contentType,
                          void *terminate_cbf,
                          void *context_tcbf );
#endif /* __LIBILU_H__ */


