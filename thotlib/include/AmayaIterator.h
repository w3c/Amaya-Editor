#ifdef _WX

#ifndef __AMAYAITERATOR_H__
#define __AMAYAITERATOR_H__

/*
 * =====================================================================================
 *        Class:  AmayaIterator
 * 
 *  Description: used to iterate on objects lists
 *
 *       Author:  Stephane GULLY
 *      Created:  12/10/2003 04:45:34 PM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaIterator
{
 public:
  AmayaIterator() {};
  virtual ~AmayaIterator() {};

  virtual void first() = 0;
  virtual void next() = 0;
  virtual bool isDone() = 0;
  virtual void * currentElement() = 0;
};

#endif // __AMAYAITERATOR_H__

#endif /* #ifdef _WX */
