/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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

/*----------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  AmayaPopupList
 * Description:  
  -----------------------------------------------------------------------*/
AmayaPopupList::AmayaPopupList ( wxWindow * p_parent, int ref ) :  wxMenu()
					      ,m_Ref(ref)
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  ~AmayaPopupList
 * Description:  
  -----------------------------------------------------------------------*/
AmayaPopupList::~AmayaPopupList()
{
  TtaDestroyDialogue( m_Ref );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPopupList
 *      Method:  OnMenuItem
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaPopupList::OnMenuItem( wxCommandEvent& event )
{
  long id = event.GetId();
  ThotCallback (m_Ref, INTEGER_DATA, (char*)id);
}

#endif /* #ifdef _WX */
