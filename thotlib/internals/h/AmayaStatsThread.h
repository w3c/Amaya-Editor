#ifdef _WX

#ifndef __AMAYASTATSTHREAD_H__
#define __AMAYASTATSTHREAD_H__

#include "wx/wx.h"
#include "wx/thread.h"

class AmayaStatsThread : public wxThread
{
 public:
  AmayaStatsThread();
  virtual ~AmayaStatsThread();

  void * Entry();
};

#endif // __AMAYASTATSTHREAD_H__

#endif /* #ifdef _WX */
