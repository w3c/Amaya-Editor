/*************Juste pour faire l'edition de lien statique sur SUN ********/
void               *dlopen ()
{
   return ((void *) 0);
}

void               *dlsym ()
{
   return ((void *) 0);
}

#ifdef __STDC__
int                 dlclose ()
#else  /* __STDC__ */
int                 dlclose ()
#endif				/* __STDC__ */
{
   return -1;
}
