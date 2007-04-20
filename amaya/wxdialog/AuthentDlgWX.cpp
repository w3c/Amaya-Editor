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

static int      MyRef;
static int      Waiting = 0;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(AuthentDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_OK"),         AuthentDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCEL"),     AuthentDlgWX::OnCancelButton )
  EVT_TEXT( XRCID("wxID_AU"),           AuthentDlgWX::OnName )
  EVT_TEXT( XRCID("wxID_PASSWD"),       AuthentDlgWX::OnPassword )
  EVT_TEXT_ENTER( XRCID("wxID_AU"),     AuthentDlgWX::OnConfirmButton )
  EVT_TEXT_ENTER( XRCID("wxID_PASSWD"), AuthentDlgWX::OnConfirmButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  AuthentDlgWX create the Authent dialog 
  params:
    + parent : parent window
    + pathname : document location
  ----------------------------------------------------------------------*/
AuthentDlgWX::AuthentDlgWX( int ref, wxWindow * parent, char *auth_realm,
			    char *server, char *name, char *pwd) :
  AmayaDialog( parent, ref )
{
  char    *ptr1, *ptr2, *label;
  int      len = 20;
  ThotBool value;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("AuthentDlgWX"));
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_GET_AUTHENTICATION) );
  SetTitle( wx_title );
  MyRef = ref;
  // waiting for a return
  Waiting = 1;

  ptr1 = TtaGetMessage (AMAYA, AM_AUTHENTICATION_REALM);
  ptr2 = TtaGetMessage (AMAYA, AM_AUTHENTICATION_SERVER);
  if (ptr1)
    len += strlen (ptr1);
  if (ptr2)
    len += strlen (ptr2);
  if (auth_realm)
    len += strlen (auth_realm);
  if (server)
    len += strlen (server);
  label = (char *)TtaGetMemory (len); /*a bit more than enough memory */
  if (label)
    {
      if (auth_realm)
        sprintf (label, ptr1, auth_realm);
      else
        sprintf (label, ptr1, "");
      XRCCTRL(*this, "wxID_LABEL_AUTHENT", wxStaticText)->SetLabel(TtaConvMessageToWX( label ) );
      if (server)
        sprintf (label, ptr2, server);
      else
        sprintf (label, ptr2, "");
      XRCCTRL(*this, "wxID_LABEL_SERVER", wxStaticText)->SetLabel(TtaConvMessageToWX( label ) );
      TtaFreeMemory (label);
    }

  XRCCTRL(*this, "wxID_LABEL_NAME", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_NAME) ));
  XRCCTRL(*this, "wxID_LABEL_PASSWD", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PASSWORD) ));

  /*
  wxString wx_name = TtaConvMessageToWX( Answer_name );
  XRCCTRL(*this, "wxID_AU", wxTextCtrl)->SetValue(wx_name);
  XRCCTRL(*this, "wxID_AU", wxTextCtrl)->SetSelection (0, -1);
  wxString wx_password = TtaConvMessageToWX( Answer_password );
  XRCCTRL(*this, "wxID_PASSWD", wxTextCtrl)->SetValue(wx_password);
  */
  wxString wx_name = TtaConvMessageToWX( name );
  XRCCTRL(*this, "wxID_AU", wxTextCtrl)->SetValue(wx_name);
  XRCCTRL(*this, "wxID_AU", wxTextCtrl)->SetSelection (0, -1);
  wxString wx_password = TtaConvMessageToWX( pwd );
  XRCCTRL(*this, "wxID_PASSWD", wxTextCtrl)->SetValue(wx_password);

  // buttons
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  
  // 'Save password' checkbox
  // XRCCTRL(*this, "wxID_CHECK_PWD",  wxCheckBox)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA ,AM_SAVE_PWD)));
  XRCCTRL(*this, "wxID_CHECK_PWD",  wxCheckBox)->SetLabel(TtaConvMessageToWX( "Do you want Amaya to remember this password ?"));
  // check if the user doesn't want to save it's password
  wxCheckBox * p_cbox = XRCCTRL(*this, "wxID_CHECK_PWD", wxCheckBox);
  TtaSetEnvBoolean("SHOW_CONFIRM_SAVE_PWD", FALSE, FALSE);
  TtaGetEnvBoolean ("SHOW_CONFIRM_SAVE_PWD", &value);
  p_cbox->SetValue( value );

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
  /* when the dialog is destroyed, It's important to cleanup context */
  if (Waiting)
  // no return done
    ThotCallback (MyRef, INTEGER_DATA, (char*) 0); 
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking the Confirm button
  ----------------------------------------------------------------------*/
void AuthentDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  wxCheckBox * p_cbox = XRCCTRL(*this, "wxID_CHECK_PWD", wxCheckBox);
  if (!p_cbox->IsChecked())
    // desactivate the save of password
    TtaSetEnvBoolean("SHOW_CONFIRM_SAVE_PWD", FALSE, TRUE);
  else
    TtaSetEnvBoolean("SHOW_CONFIRM_SAVE_PWD", TRUE, TRUE);

  // return done
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on Cancel button
  ----------------------------------------------------------------------*/
void AuthentDlgWX::OnCancelButton( wxCommandEvent& event )
{
  // return done
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*---------------------------------------------------------------
  OnName 
  ---------------------------------------------------------------*/
void AuthentDlgWX::OnName ( wxCommandEvent& event )
{
  wxString wx_name = XRCCTRL(*this, "wxID_AU", wxTextCtrl)->GetValue( );

  // allocate a temporary buffer to convert wxString to (char *) UTF-8 buffer
  char buffer[512];
  wxASSERT( wx_name.Len() < 512 );
  strcpy( buffer, (const char*)wx_name.mb_str(wxConvUTF8) );

  ThotCallback (BaseDialog + NameText,  STRING_DATA, (char *)buffer );
}

/*---------------------------------------------------------------
  OnPassword
  ---------------------------------------------------------------*/
void AuthentDlgWX::OnPassword ( wxCommandEvent& event )
{
  wxString wx_passwd = XRCCTRL(*this, "wxID_PASSWD", wxTextCtrl)->GetValue( );

  // allocate a temporary buffer to convert wxString to (char *) UTF-8 buffer
  char buffer[512];
  wxASSERT( wx_passwd.Len() < 512 );
  strcpy( buffer, (const char*)wx_passwd.mb_str(wxConvUTF8) );

  ThotCallback (BaseDialog + PasswordText,  STRING_DATA, (char *)buffer );
}

#endif /* _WX */
