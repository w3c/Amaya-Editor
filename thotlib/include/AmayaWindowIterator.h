#ifdef _WX

#ifndef __AMAYAWINDOWITERATOR_H__
#define __AMAYAWINDOWITERATOR_H__

#include "AmayaIterator.h"

class AmayaWindow;

/*
 * =====================================================================================
 *        Class:  AmayaWindowIterator
 * 
 *  Description:  used to iterate on AmayaWindow lists
 *
 *       Author:  Stephane GULLY
 *      Created:  12/10/2003 04:45:34 PM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaWindowIterator : public AmayaIterator
{
 public:
  AmayaWindowIterator() : m_CurrentId(0) {};
  virtual ~AmayaWindowIterator() {};

  virtual void first();
  virtual void next();
  virtual bool isDone();
  virtual void * currentElement();
  int currentWindowId();
 protected:
  int  m_CurrentId;
};

#endif // __AMAYAWINDOWITERATOR_H__

#endif /* #ifdef _WX */
