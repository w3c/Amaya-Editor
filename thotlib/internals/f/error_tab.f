
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void DetectedError ( short FnCode, short ErrCode, int *NoError );
extern short GetErrorCode ( int NoError );
extern short GetFunctionCode ( int NoError );

#else /* __STDC__ */

extern void DetectedError (/* short FnCode, short ErrCode, int *NoError */);
extern short GetErrorCode (/* int NoError */);
extern short GetFunctionCode (/* int NoError */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
