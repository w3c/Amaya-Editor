void            MarquerIndex(/*pDoc*/);
void            EnleveFormMarquer(/*pDoc*/);
void            Marq_Init ();
void            Marq_Action (/* fenetre */);
void            Marq_SuppAction (/* fenetre */);
void            Marq_AttrTrouve(/* typeattr, val, num */);
void            Marq_Trouve(/* val */);

#ifdef MOTIF
void            CallbackMarq ( /* Ref, TypeData, Data */);
#endif
