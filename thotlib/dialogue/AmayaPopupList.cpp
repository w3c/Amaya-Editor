#ifdef _WX

#include "wx/wx.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"
#include "view.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "attrmenu_f.h"
#include "frame_tv.h"
#include "views_f.h"

#include "message_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"


#include "AmayaPopupList.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaPopupList, wxMenu)

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPopupList, wxMenu)
  EVT_MENU( -1, AmayaPopupList::OnMenuItem ) 
END_EVENT_TABLE()

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  AmayaPopupList
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaPopupList::AmayaPopupList ( int ref ) :  wxMenu()
					      ,m_Ref(ref)
{
  wxLogDebug( _T("AmayaPopupList::AmayaPopupList") );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  ~AmayaPopupList
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaPopupList::~AmayaPopupList()
{
  wxLogDebug( _T("AmayaPopupList::~AmayaPopupList") );
  TtaDestroyDialogue( m_Ref );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  OnMenuItem
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaPopupList::OnMenuItem( wxCommandEvent& event )
{
  long id = event.GetId();
  ThotCallback (m_Ref, INTEGER_DATA, (char*)id);
}

#endif /* #ifdef _WX */
