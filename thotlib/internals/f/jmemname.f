
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void * jget_small ( size_t sizeofobject );
extern void jfree_small ( void * object );
extern long jmem_available ( long min_bytes_needed, long max_bytes_needed );
extern void jopen_backing_store ( backing_store_ptr info, long total_bytes_needed );
extern void jmem_init ( external_methods_ptr emethods );
extern void jmem_term ( void );

#else /* __STDC__ */

extern void * jget_small (/* size_t sizeofobject */);
extern void jfree_small (/* void * object */);
extern long jmem_available (/* long min_bytes_needed, long max_bytes_needed */);
extern void jopen_backing_store (/* backing_store_ptr info, long total_bytes_needed */);
extern void jmem_init (/* external_methods_ptr emethods */);
extern void jmem_term (/* void */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
