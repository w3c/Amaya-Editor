#ifdef _WX

#include "AmayaCallback.h"


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCallback
 *      Method:  AmayaCallback
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
AmayaCallback::AmayaCallback( ) : m_map_table(), m_map_multi_table()
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCallback
 *      Method:  ~AmayaCallback
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AmayaCallback::~AmayaCallback( )
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCallback
 *      Method:  registerCallback
 * Description:  register a callback for a given id
 * 		 return true if the callback is registred
 * 		 return false if a previous callback was registred for the same id
 *--------------------------------------------------------------------------------------
 */
bool AmayaCallback::registerCallback( long id, const AmayaCParam & cparams )
{
  AmayaCParam callback = m_map_table[id];
  if ( callback.pCallback == 0)
  {
    m_map_table[id] = cparams;
    return true;
  }
  else
  {
    return false;
  }

  // TODO faire attention si on desire enregistrer plusieurs callback pour le meme id
}

/*
 *--------------------------------------------------------------------------------------
 *       Class: AmayaCallback 
 *      Method: unregisterCallback 
 * Description: unregister a callback 
 *              return true if a callback has been removed
 *              return false if there was no callback
 *--------------------------------------------------------------------------------------
 */
bool AmayaCallback::unregisterCallback( long id )
{
  AmayaCParam callback = m_map_table[id];
  m_map_table[id] = AmayaCParam();
  return (callback.pCallback != 0);
  // TODO faire attention si on desire enregistrer plusieurs callback pour le meme id
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCallback
 *      Method:  registerCallback
 * Description:  register a callback for a given widget/id (wxObject*)
 * 		 return true if the callback is registred
 * 		 return false if a previous callback was registred for the same id
 *--------------------------------------------------------------------------------------
 */
bool AmayaCallback::registerCallback( int p_widget, long id, const AmayaCParam & cparams )
{
  AmayaCallbackHash        & callback_multi = m_map_multi_table[p_widget];
  AmayaCParam              & callback       = callback_multi[id];
  if ( callback.pCallback == 0)
  {
    m_map_multi_table[p_widget][id] = cparams;
    return true;
  }
  else
  {
    return false;
  }
  // TODO faire attention si on desire enregistrer plusieurs callback pour le meme id
}

/*
 *--------------------------------------------------------------------------------------
 *       Class: AmayaCallback 
 *      Method: unregisterCallback 
 * Description: unregister a callback 
 *              return true if a callback has been removed
 *              return false if there was no callback
 *--------------------------------------------------------------------------------------
 */
bool AmayaCallback::unregisterCallback( int p_widget, long id )
{
  AmayaCallbackHash & callback_multi = m_map_multi_table[p_widget];
  AmayaCParam       & callback       = callback_multi[id];

  m_map_multi_table[p_widget][id] = AmayaCParam();

  return (callback.pCallback != 0);
  // TODO faire attention si on desire enregistrer plusieurs callback pour le meme id
}



#endif /* #ifdef _WX */
