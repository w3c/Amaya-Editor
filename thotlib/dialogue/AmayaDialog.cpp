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
#include "registry_wx.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "font_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "font_f.h"
#include "appli_f.h"
#include "profiles_f.h"
#include "appdialogue_f.h"
#include "boxparams_f.h"
#include "dialogapi_f.h"
#include "callback_f.h"
#include "AmayaParams.h"
#include "appdialogue_wx_f.h"

#include "AmayaDialog.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaDialog, wxDialog)

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(AmayaDialog, wxDialog)
  EVT_CLOSE( AmayaDialog::OnClose )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaDialog::AmayaDialog( wxWindow * p_parent, int ref ) :
  wxDialog(),
  m_Ref(ref)
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaDialog::AmayaDialog - ref=%d"), ref);
 
  // SetIcon provoque un warning, on ne l'utilise pas pr le moment.
  //SetIcon( AmayaApp::GetAppIcon() );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
AmayaDialog::~AmayaDialog()
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaDialog::~AmayaDialog"));

  //  TtaDestroyDialogue( m_Ref );

  /* then give focus to canvas */
  //  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnClose
  ----------------------------------------------------------------------*/
void AmayaDialog::OnClose( wxCloseEvent& event )
{
  TTALOGDEBUG_1( TTA_LOG_DIALOG, _T("AmayaDialog::OnClose - ref=%d"), m_Ref);

  // maybe it's better to use :
  // this callback is usefull for :
  // - Add CSS / Create link
  // because it cancel the link creation process.
  // TODO : verifier que l'appel generique de cette callback ne gene pas pour certains dialogues.
  // ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);

  //TtaDestroyDialogue( m_Ref );

  //  forward the event to parents
  event.Skip();
}

#endif /* _WX */
