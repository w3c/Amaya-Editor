#ifndef __LOGDEBUG_H__
#define __LOGDEBUG_H__

typedef struct tta_logdebug_filter_names_
{
  const char * gui_name;
  const char * registry_name;
} tta_logdebug_filter_names;

extern int g_logdebug_filter;
extern tta_logdebug_filter_names g_logdebug_filter_names[];

enum {
  TTA_LOG_MISC      = 1 << 0,
  TTA_LOG_PANELS    = 1 << 1,
  TTA_LOG_DIALOG    = 1 << 2,
  TTA_LOG_INIT      = 1 << 3,
  TTA_LOG_KEYINPUT  = 1 << 4,
  TTA_LOG_FOCUS     = 1 << 5,
  TTA_LOG_DRAW      = 1 << 6,
  TTA_LOG_SOCKET    = 1 << 7,
  TTA_LOG_CLIPBOARD = 1 << 8,
  TTA_LOG_FONT      = 1 << 9,
  TTA_LOG_SVGEDIT   = 1 << 10,
  TTA_LOG_MAX       = 1 << 11,
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
#define TTALOGDEBUG_10( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 ) ;
#define TTALOGDEBUG_11( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11 ) ;
#define TTALOGDEBUG_12( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12 ) ;
#define TTALOGDEBUG_13( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13 ) ;
#define TTALOGDEBUG_14( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14 ) ;
#define TTALOGDEBUG_15( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15 ) ;
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
#define TTALOGDEBUG_10( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 );
#define TTALOGDEBUG_11( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11 );
#define TTALOGDEBUG_12( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12 );
#define TTALOGDEBUG_13( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13 );
#define TTALOGDEBUG_14( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14 );
#define TTALOGDEBUG_15( filter, message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15 ) if ((filter) & g_logdebug_filter) wxLogDebug( message, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15 );
#endif /* __WXDEBUG__ */
#endif /* _WX */

#endif /* __LOGDEBUG_H__ */
