#ifdef _WX

#ifndef __AMAYACALLBACK_H__
#define __AMAYACALLBACK_H__

#include "wx/wx.h"
#include "wx/hashmap.h"

// a callback prototype
typedef void        (*Proc) ();

/*
 * =====================================================================================
 *        Class:  AmayaCParam
 * 
 *  Description:  this class represent a structure for callback + parameters storage
 * 
 *       Author:  Stephane GULLY
 *      Created:  11/14/2003 10:49:06 AM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaCParam
{
  public:
    AmayaCParam ( Proc p_callback = 0,
                  void * p1 = 0,
		  void * p2 = 0,
		  void * p3 = 0,
		  void * p4 = 0,
		  void * p5 = 0  ) :
      pCallback( p_callback ),
      param1( p1 ),
      param2( p2 ),
      param3( p3 ),
      param4( p4 ),
      param5( p5 )
    {
    }
    AmayaCParam ( const AmayaCParam & c ) :
      pCallback( c.pCallback ),
      param1( c.param1 ),
      param2( c.param2 ),
      param3( c.param3 ),
      param4( c.param4 ),
      param5( c.param5 )
    {
    }
      
    // the callback
    Proc pCallback;
    
    // the callback params
    void * param1;
    void * param2;
    void * param3;
    void * param4;
    void * param5;
};

// declare a new hashmap id->callback type (require wxWindows)
WX_DECLARE_HASH_MAP( long,		// the id field (unique)
                     AmayaCParam,	// the callback field
                     wxIntegerHash,
                     wxIntegerEqual,
                     AmayaCallbackHash );

// declare a new hashmap wxObject*->id/callback type (require wxWindows)
WX_DECLARE_HASH_MAP( int,		// the wxObject* field (unique)
                     AmayaCallbackHash,	// the id/callback field
                     wxIntegerHash,
                     wxIntegerEqual,
                     AmayaCallbackMultiHash );

/*
 * =====================================================================================
 *        Class:  AmayaCallback
 * 
 *  Description:  this class propose interface to register/unregister callbacks
 * 
 *       Author:  Stephane GULLY
 *      Created:  11/14/2003 10:50:10 AM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaCallback
{
  public:
    AmayaCallback( );
    virtual ~AmayaCallback( );
    
    bool registerCallback( long id, const AmayaCParam & cparams );
    bool unregisterCallback( long id );
   
    bool registerCallback( int p_widget, long id, const AmayaCParam & cparams );
    bool unregisterCallback( int p_widget, long id );
  protected:
    AmayaCallbackHash       m_map_table;
    AmayaCallbackMultiHash  m_map_multi_table;
};

#endif /* #ifndef __AMAYACALLBACK_H__ */

#endif /* #ifdef _WX */
