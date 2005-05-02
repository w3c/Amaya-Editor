/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
#include "logdebug.h"

/* here you can choose the traces to show, by default : nothing */
int g_logdebug_filter = 0; /*  = TTA_LOG_PANELS | TTA_LOG_DIALOG; */

/* here the array containing all the log filter names (used in the gui) */
tta_logdebug_filter_names g_logdebug_filter_names[] = {
  {"Misc",      "LOGDEBUG_MISC"},      /* TTA_LOG_MISC */
  {"Panels",    "LOGDEBUG_PANELS"},    /* TTA_LOG_PANELS */
  {"Dialog",    "LOGDEBUG_DIALOG"},    /* TTA_LOG_DIALOG */
  {"Init",      "LOGDEBUG_INIT"},      /* TTA_LOG_INIT */
  {"Key Input", "LOGDEBUG_KEYINPUT"},  /* TTA_LOG_KEYINPUT */
  {"Focus",     "LOGDEBUG_FOCUS"},     /* TTA_LOG_FOCUS */
  {"Draw",      "LOGDEBUG_DRAW"},      /* TTA_LOG_DRAW */
  {"Socket",    "LOGDEBUG_SOCKET"},    /* TTA_LOG_SOCKET */
  {"Clipboard", "LOGDEBUG_CLIPBOARD"}, /* TTA_LOG_CLIPBOARD */
  {"Font",      "LOGDEBUG_FONT"},      /* TTA_LOG_FONT */
  {"SVGEdit",   "LOGDEBUG_SVGEDIT"},   /* TTA_LOG_SVGEDIT */
  {"",          ""}                    /* dont remove this entry */
};
