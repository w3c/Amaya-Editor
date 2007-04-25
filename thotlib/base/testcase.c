/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Testcase managment
 *
 * Authors: S. Gully (INRIA)
 */

#include <time.h>

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "glwindowdisplay.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "logdebug.h"
#include "displayview_f.h"
#include "testcase_f.h"


typedef struct ThotBenchTimer_
{
  char     name[MAX_TXT_LEN];
  clock_t  start_time;
  clock_t  time;
  ThotBool not_free;
  FILE *   fp;
} ThotBenchTimer;

#define MAX_BENCHMARK_TIMER 5
static ThotBenchTimer g_benchmark_timer[MAX_BENCHMARK_TIMER];
static ThotBool g_benchmark_timer_is_init = FALSE;


/*----------------------------------------------------------------------
   TtaLaunchTestCase parse filename file and execute the found commands
   The file format must be something like that:

command_repeat_number\tcommand_name
   "command_repeat_number" indicates how many time the command must be repeated.

   or

m\tmaker_name
   "m" indicates a marker, "marker_name" a label to identifiy the marker

   example:
50	TtcLineDown
m	50TtcLineDown
50	TtcLineUp
m	50TtcLineUp
   (use tabulations to separate the command repetition number and the command name)
  ----------------------------------------------------------------------*/
void TtaLaunchTestCase( const char * filename )
{
  int active_doc_id, view;
  FrameToView(TtaGiveActiveFrame(), &active_doc_id, &view);
  
  // start a timer
  int bench_id = TtaStartBenchmarkTimer( "testcase" );
  
  FILE *fp;
  fp = TtaReadOpen (filename);
  if (fp == NULL)
    return;
  ThotBool found_marker;
  char  command[MAX_TXT_LEN];
  char  nb_command[MAX_TXT_LEN];
  int nb_cmd = 0;
  int i, c;
  while ((c = fgetc(fp)) != EOF)
    {
      found_marker = FALSE;
      
      // first : read how many time to repeat the command
      i = 0;
      do
        {
          nb_command[i] = c;
          c = fgetc(fp);
          i++;
        }
      while (c != '\t' && c != EOL && c != EOF);
      if (nb_command[0] == 'm')
        {
          // found a marker
          found_marker = TRUE;
        }
      else
        {
          nb_command[i] = '\0';
          nb_cmd = atoi(nb_command);
        }
      c = fgetc(fp);
      
      // secondly : read the command name
      i = 0;
      do
        {
          command[i] = c;
          c = fgetc(fp);
          i++;
        }
      while (c != EOL && c != EOF);
      command[i] = '\0';
      if (found_marker)
        {
          // set the marker
          TtaSetBenchmarkMark( bench_id, command );
        }
      else
        {
          // execute the commands
          while(nb_cmd>0)
            {
              TtaExecuteMenuAction (command, active_doc_id, view, FALSE);
              TtaHandlePendingEvents();
#ifdef _GL
              GL_DrawAll();
#endif /* _GL */
              nb_cmd--;
            }
        }
    }
  TtaReadClose (fp);
  
  // stop the timer
  TtaStopBenchmarkTimer( bench_id );
}

/*----------------------------------------------------------------------
  TtaStartBenchmarkTimer start a timer for statistic purpose
  it returns an id, use it to setup a marker or to stop the timer
  ----------------------------------------------------------------------*/
int TtaStartBenchmarkTimer( const char * name )
{
  ThotBool found = FALSE;
  int bench_id = 0;

  // init the array
  if (!g_benchmark_timer_is_init)
    {
      memset(g_benchmark_timer, 0, sizeof(ThotBenchTimer)*MAX_BENCHMARK_TIMER);
      g_benchmark_timer_is_init = TRUE;
    }

  // search for a free entry
  while (!found && bench_id<MAX_BENCHMARK_TIMER )
    {
      if (!g_benchmark_timer[bench_id].not_free)
	found = TRUE;
      else
	bench_id++;
    }

  // found an entry ?
  if (found)
    {
      // register a new entry
      strcpy(g_benchmark_timer[bench_id].name, name);
      g_benchmark_timer[bench_id].time = clock();
      g_benchmark_timer[bench_id].start_time = g_benchmark_timer[bench_id].time;
      g_benchmark_timer[bench_id].not_free = TRUE;

      // store statistic into a file
      char *appHome;
      char  filename[MAX_TXT_LEN];
      appHome = TtaGetEnvString ("THOTDIR");
      strcpy (filename, appHome);
      strcat (filename, DIR_STR);
      strcat (filename, "testcase_result");
      g_benchmark_timer[bench_id].fp = TtaWriteOpen (filename);     
      fprintf (g_benchmark_timer[bench_id].fp, "[%s] Start timer.\n",
	       g_benchmark_timer[bench_id].name);

      return bench_id;
    }
  else
    return -1; 
}

/*----------------------------------------------------------------------
  TtaStopBenchmarkTimer stop a timer precedently created by TtaStartBenchmarkTimer
  ----------------------------------------------------------------------*/
void TtaStopBenchmarkTimer( int bench_id )
{
  if (!g_benchmark_timer[bench_id].not_free)
    return;

  // note the new timer value
  g_benchmark_timer[bench_id].time = clock();

  // store statistic into a file
  FILE *fp = g_benchmark_timer[bench_id].fp;
  if (fp)
    {
      fprintf (fp, "[%s] Stop timer\tElapsed time=%ld ms.\n",
	       g_benchmark_timer[bench_id].name,
	       (g_benchmark_timer[bench_id].time-g_benchmark_timer[bench_id].start_time)/(CLOCKS_PER_SEC/1000));
      TtaWriteClose(fp);
    }

  // reset the entry
  memset(&g_benchmark_timer[bench_id],0,sizeof(ThotBenchTimer));
}

/*----------------------------------------------------------------------
  TtaSetBenchmarkMark setup a marker
  it writes into a file "THOT_DIR/testcase_result" how many time elapsed
  since last marker and since timer start
  ----------------------------------------------------------------------*/
void TtaSetBenchmarkMark( int bench_id, const char * mark_name )
{
  if (!g_benchmark_timer[bench_id].not_free)
    return;

  clock_t last_time = g_benchmark_timer[bench_id].time;

  // note the new timer value
  g_benchmark_timer[bench_id].time = clock();

  FILE *fp = g_benchmark_timer[bench_id].fp;
  if (fp)
    {
      fprintf (fp, "[%s] %s\tElapsed time=%ld ms\tDelta time=%ld ms.\n",
	       g_benchmark_timer[bench_id].name,
	       mark_name,
	       (g_benchmark_timer[bench_id].time-g_benchmark_timer[bench_id].start_time)/(CLOCKS_PER_SEC/1000),
	       (g_benchmark_timer[bench_id].time-last_time)/(CLOCKS_PER_SEC/1000));
    }
}
