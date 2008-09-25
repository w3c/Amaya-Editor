/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

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
#include "message_wx.h"
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

#include "AmayaConfirmCloseTab.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaConfirmCloseTab, wxDialog)

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(AmayaConfirmCloseTab, wxDialog)
  EVT_BUTTON( XRCID("wxID_OK"),      AmayaConfirmCloseTab::OnOk )
  EVT_BUTTON( XRCID("wxID_CANCEL"),  AmayaConfirmCloseTab::OnCancel )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaConfirmCloseTab::AmayaConfirmCloseTab( wxWindow * p_parent, int nb_tab ) : wxDialog()
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaConfirmCloseTab::AmayaConfirmCloseTab"));
  wxXmlResource::Get()->LoadDialog(this, p_parent, wxT("ConfirmCloseTab"));
  
  // setup labels
  SetTitle(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_LIB_CONFIRM)));
  XRCCTRL(*this, "wxID_OK",      wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_LIB_CLOSETABS)));
  XRCCTRL(*this, "wxID_CANCEL",  wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CANCEL)));
  XRCCTRL(*this, "wxID_LABEL_Q",  wxStaticText)->SetLabel(wxString::Format(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_LIB_WARNCTAB_INFO)), nb_tab));
  XRCCTRL(*this, "wxID_CHECK_W",  wxCheckBox)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_LIB_WARNCTAB_CHECK)));

  // check if the user doesn't want to show the dialog
  wxCheckBox * p_cbox = XRCCTRL(*this, "wxID_CHECK_W", wxCheckBox);
  p_cbox->SetValue( DoesUserWantToShowMe() );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
AmayaConfirmCloseTab::~AmayaConfirmCloseTab()
{
  TTALOGDEBUG_0( TTA_LOG_DIALOG, _T("AmayaConfirmCloseTab::~AmayaConfirmCloseTab"));
}

/*----------------------------------------------------------------------
  OnOk
  ----------------------------------------------------------------------*/
void AmayaConfirmCloseTab::OnOk( wxCommandEvent& event )
{
  wxCheckBox * p_cbox = XRCCTRL(*this, "wxID_CHECK_W", wxCheckBox);
  if (!p_cbox->IsChecked())
    // desactivate the close tab warning dialog
    TtaSetEnvBoolean("SHOW_CONFIRM_CLOSE_TAB", FALSE, TRUE);
  else
    TtaSetEnvBoolean("SHOW_CONFIRM_CLOSE_TAB", TRUE, TRUE);

  // close the modal dialog
  EndModal(wxID_OK);
}

/*----------------------------------------------------------------------
  OnCancel
  ----------------------------------------------------------------------*/
void AmayaConfirmCloseTab::OnCancel( wxCommandEvent& event )
{
  EndModal(wxID_CANCEL);
}

/*----------------------------------------------------------------------
  DoesUserWantToShowMe check the internal thot variable to know if
  the dialog should be shown or not
  ----------------------------------------------------------------------*/
bool AmayaConfirmCloseTab::DoesUserWantToShowMe()
{
  ThotBool value;
  TtaSetEnvBoolean("SHOW_CONFIRM_CLOSE_TAB", TRUE, FALSE);
  TtaGetEnvBoolean ("SHOW_CONFIRM_CLOSE_TAB", &value);
  return value;
}

#endif /* _WX */
