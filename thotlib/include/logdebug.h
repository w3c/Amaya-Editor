#ifndef __LOGDEBUG_H__
#define __LOGDEBUG_H__

extern int g_logdebug_filter;

enum {
  TTA_LOG_MISC     = 1,
  TTA_LOG_PANELS   = 2,
  TTA_LOG_DIALOG   = 4,
  TTA_LOG_INIT     = 8,
  TTA_LOG_KEYINPUT = 16,
  TTA_LOG_FOCUS    = 32,
  TTA_LOG_DRAW     = 64,
  TTA_LOG_SOCKET   = 128,
};

#if !defined(__WXDEBUG__)
#define TTALOGDEBUG_0( filter, message ) ;
#define TTALOGDEBUG_1( filter, message, p1 ) ;
#define TTALOGDEBUG_2( filter, message, p1, p2 ) ;
#define TTALOGDEBUG_3( filter, message, p1, p2, p3 ) ;
#define TTALOGDEBUG_4( filter, message, p1, p2, p3, p4 ) ;
#define TTALOGDEBUG_5( filter, message, p1, p2, p3, p4, p5 ) ;
#define TTALOGDEBUG_6( filter, message, p1, p2, p3, p4, p5, p6 ) ;
#define TTALOGDEBUG_7( filter, message, p1, p2, p3, p4, p5, p6, p7 ) ;
#define TTALOGDEBUG_8( filter, message, p1, p2, p3, p4, p5, p6, p7, p8 ) ;
#define TTALOGDEBUG_9( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9 ) ;
#endif /* __WXDEBUG__ */

#ifdef _WX

#ifdef __WXDEBUG__
#define TTALOGDEBUG_0( filter, message ) if ((filter) & g_logdebug_filter) wxLogDebug( message );
#define TTALOGDEBUG_1( filter, message, p1 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1 );
#define TTALOGDEBUG_2( filter, message, p1, p2 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2 );
#define TTALOGDEBUG_3( filter, message, p1, p2, p3 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3 );
#define TTALOGDEBUG_4( filter, message, p1, p2, p3, p4 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4 );
#define TTALOGDEBUG_5( filter, message, p1, p2, p3, p4, p5 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5 );
#define TTALOGDEBUG_6( filter, message, p1, p2, p3, p4, p5, p6 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5, p6 );
#define TTALOGDEBUG_7( filter, message, p1, p2, p3, p4, p5, p6, p7 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5, p6, p7 );
#define TTALOGDEBUG_8( filter, message, p1, p2, p3, p4, p5, p6, p7, p8 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5, p6, p7, p8 );
#define TTALOGDEBUG_9( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5, p6, p7, p8, p9 );
#endif /* __WXDEBUG__ */
#endif /* _WX */

#endif /* __LOGDEBUG_H__ */
