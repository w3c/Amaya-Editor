#ifdef _WX

/* in this file we can find thotlib function exported for amaya */
extern void          ThotCallback ( int ref, int typedata, char *data );
extern wxString      TtaConvMessageToWX ( const char * p_message );
extern int           TtaGetWindowNumber( );
extern void          TtaCloseWindow( int window_id );
#endif /* _WX */
