#ifdef _WX

#ifndef __AMAYACALLBACK_H__
#define __AMAYACALLBACK_H__

#include "wx/wx.h"
#include "wx/hashmap.h"

/*
 * =====================================================================================
 *        Class:  AmayaContext
 * 
 *  Description:  this class is used to be associated with a wxObject
 *                for exemple, a wxMenuItem could be associated with a context
 *                and this context is used when the corresponding callback is activated
 * 
 *       Author:  Stephane GULLY
 *      Created:  11/14/2003 10:49:06 AM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaContext : public wxObjectRefData
{
 public:
  AmayaContext( void * p_data = NULL) :
    wxObjectRefData()
    ,m_pData(p_data)
    {
    }
  AmayaContext( const AmayaContext & context ) :
    wxObjectRefData()
    ,m_pData(context.m_pData)
    {
    }
  virtual ~AmayaContext()
    {
    }
  
  void * GetData()
    {
      return m_pData;
    }
  void SetDate( void * p_data )
    {
      m_pData = p_data;
    }
 protected:
  void * m_pData;
};

#endif /* #ifndef __AMAYACALLBACK_H__ */

#endif /* #ifdef _WX */
