
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void NoOp ( PtrElement pEl );
extern boolean SendAttributeMessage ( NotifyAttribute *notifyAttr, boolean pre );
extern boolean ThotSendMessage ( NotifyEvent *notifyEvent, boolean pre );

#else /* __STDC__ */

extern void NoOp (/* PtrElement pEl */);
extern boolean SendAttributeMessage (/* NotifyAttribute *notifyAttr, boolean pre */);
extern boolean ThotSendMessage (/* NotifyEvent *notifyEvent, boolean pre */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
