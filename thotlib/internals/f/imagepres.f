
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void SetImageRule(PtrElement pEl, int x, int y, int w, int h, int typeimage, PictureScaling presimage);
extern void NewImageDescriptor ( PtrAbstractBox ppav, char * filename, int imagetype );
extern void FreeImageDescriptor(int *desc);
extern void UpdateImageDescriptor(int *Imdcopie, int *Imdsource);

#else /* __STDC__ */

extern void SetImageRule(/*PtrElement pEl, int x, int y, int w, int h, int typeimage, PictureScaling presimage*/);
extern void NewImageDescriptor (/* PtrAbstractBox ppav, char * filename, int imagetype */);
extern void FreeImageDescriptor(/*int * desc*/);
extern void UpdateImageDescriptor(/*int *Imdcopie, int *Imdsource*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
