#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "AuthentDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "print.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(AuthentDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_CONFIRMBUTTON"),       AuthentDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCELBUTTON"),        AuthentDlgWX::OnCancelButton )
  EVT_TEXT( XRCID("wxID_AU"),                    AuthentDlgWX::OnName )
  EVT_TEXT( XRCID("wxID_PASSWD"),                AuthentDlgWX::OnPassword )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  AuthentDlgWX create the Authent dialog 
  params:
    + parent : parent window
    + pathname : document location
  ----------------------------------------------------------------------*/
AuthentDlgWX::AuthentDlgWX( int ref,
			    wxWindow* parent,
			    const wxString & wx_identification) :
  AmayaDialog( NULL, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("AuthentDlgWX"));
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_GET_AUTHENTICATION) );
  SetTitle( wx_title );

  XRCCTRL(*this, "wxID_LABEL_AUTHENT", wxStaticText)->SetLabel( wx_identification );
  XRCCTRL(*this, "wxID_LABEL_NAME", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_NAME) ));
  XRCCTRL(*this, "wxID_LABEL_PASSWD", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PASSWORD) ));

  wxString wx_name = TtaConvMessageToWX( Answer_name );
  XRCCTRL(*this, "wxID_AU", wxTextCtrl)->SetValue(wx_name);
  wxString wx_password = TtaConvMessageToWX( Answer_password );
  XRCCTRL(*this, "wxID_PASSWD", wxTextCtrl)->SetValue(wx_password);

  // buttons
  XRCCTRL(*this, "wxID_CONFIRMBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  
  // Set focus to ...
  //  XRCCTRL(*this, "wxID_AU", wxTextCtrl)->SetFocus();

  Layout();
  SetAutoLayout( TRUE );
}

/*---------------------------------------------------------------------------
  Destructor. (Empty, as we don't need anything special done when destructing).
  ---------------------------------------------------------------------------*/
AuthentDlgWX::~AuthentDlgWX()
{
  ThotCallback (BaseDialog + FormAnswer, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking the Confirm button
  ----------------------------------------------------------------------*/
void AuthentDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  ThotCallback (BaseDialog + FormAnswer, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on Cancel button
  ----------------------------------------------------------------------*/
void AuthentDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (BaseDialog + FormAnswer, INTEGER_DATA, (char*) 0);
}

/*---------------------------------------------------------------
  OnName 
  ---------------------------------------------------------------*/
void AuthentDlgWX::OnName ( wxCommandEvent& event )
{
  char buffer[100];

  wxString wx_name = XRCCTRL(*this, "wxID_AU", wxTextCtrl)->GetValue( );
  // set the printer name
  // allocate a temporary buffer to copy the 'const char *' printer name buffer 
  wxASSERT( wx_name.Len() < 100 );
  strcpy( buffer, wx_name.ToAscii() );
  ThotCallback (BaseDialog + NameText,  STRING_DATA, (char *)buffer );
}

/*---------------------------------------------------------------
  OnPassword
  ---------------------------------------------------------------*/
void AuthentDlgWX::OnPassword ( wxCommandEvent& event )
{
  char buffer[100];

  wxString wx_passwd = XRCCTRL(*this, "wxID_PASSWD", wxTextCtrl)->GetValue( );
  // allocate a temporary buffer to copy the 'const char *' printer name buffer 
  wxASSERT( wx_passwd.Len() < 100 );
  strcpy( buffer, wx_passwd.ToAscii() );
  ThotCallback (BaseDialog + PasswordText,  STRING_DATA, (char *)buffer );
}

#endif /* _WX */
