#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"

#include "AmayaApp.h"
#include "BgImageDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(BgImageDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OPENBUTTON"),   BgImageDlgWX::OnOpenButton )
  EVT_BUTTON(     XRCID("wxID_CLEARBUTTON"),  BgImageDlgWX::OnClearButton )
  EVT_BUTTON(     XRCID("wxID_BROWSEBUTTON"), BgImageDlgWX::OnBrowseButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"), BgImageDlgWX::OnCancelButton )
  EVT_TEXT_ENTER( XRCID("wxID_URL"),          BgImageDlgWX::OnOpenButton )

  EVT_BUTTON(     XRCID("wxID_BBT_REPEAT"),   BgImageDlgWX::OnRepeatButton )
  EVT_BUTTON(     XRCID("wxID_BBT_REPEAT_X"), BgImageDlgWX::OnRepeatXButton )
  EVT_BUTTON(     XRCID("wxID_BBT_REPEAT_Y"), BgImageDlgWX::OnRepeatYButton )
  EVT_BUTTON(     XRCID("wxID_BBT_NOREPEAT"), BgImageDlgWX::OnNoRepeatButton )

END_EVENT_TABLE()

/*----------------------------------------------------------------------
  BgImageDlgWX create the dialog used to select a new image
  params:
    + parent : parent window
    + title : dialog title
    + urlToOpen : the proposed url
    + alt: the suggested alt
  returns:
  ----------------------------------------------------------------------*/
BgImageDlgWX::BgImageDlgWX( int ref, wxWindow* parent, const wxString & urlToOpen, int RepeatValue ) :
  AmayaDialog( NULL, ref )
  ,m_RepeatMode(RepeatValue)
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("BgImageDlgWX"));

  // update dialog labels with given ones
  SetTitle( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BACKGROUND_IMAGE)) );
  XRCCTRL(*this, "wxID_LABEL_URL", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BACKGROUND_IMAGE)) );
  XRCCTRL(*this, "wxID_OPENBUTTON", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CLEARBUTTON", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_CLEAR) ));
  XRCCTRL(*this, "wxID_BROWSEBUTTON", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BROWSE) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

  XRCCTRL(*this, "wxID_LABEL_REPEATMODE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_REPEAT_MODE)));
  XRCCTRL(*this, "wxID_BBT_REPEAT", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_REPEAT)));
  XRCCTRL(*this, "wxID_BBT_REPEAT_X", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_REPEAT_X)));
  XRCCTRL(*this, "wxID_BBT_REPEAT_Y", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_REPEAT_Y)));
  XRCCTRL(*this, "wxID_BBT_NOREPEAT", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_NO_REPEAT)));

  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetValue( urlToOpen );

  m_OffColour = XRCCTRL(*this, "wxID_BBT_REPEAT", wxBitmapButton)->GetBackgroundColour();
  m_OnColour  = wxColour(250, 200, 200);
  RedrawRepeatButtons();

  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
BgImageDlgWX::~BgImageDlgWX()
{
}

/*----------------------------------------------------------------------
  OnOpenButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void BgImageDlgWX::OnOpenButton( wxCommandEvent& event )
{
  // get the current url
  wxString url = XRCCTRL(*this, "wxID_URL", wxTextCtrl)->GetValue( );
  
  // allocate a temporary buffer to copy the 'const char *' url buffer 
  char buffer[512];
  wxASSERT( url.Len() < 512 );
  strcpy( buffer, (const char*)url.mb_str(wxConvUTF8) );

  // call the thotlib callback
  ThotCallback (BaseImage + ImageURL, STRING_DATA, (char *)buffer);
  ThotCallback (BaseImage + RepeatImage, INTEGER_DATA, (char*)m_RepeatMode);
  ThotCallback (m_Ref, INTEGER_DATA, (char*)1);
}

/*----------------------------------------------------------------------
  OnClearButton called when the user click on clear button
  params:
  returns:
  ----------------------------------------------------------------------*/
void BgImageDlgWX::OnClearButton( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetValue(_T(""));
  ThotCallback (BaseImage + RepeatImage, INTEGER_DATA, (char*)m_RepeatMode);
  ThotCallback (m_Ref, INTEGER_DATA, (char*)2);
}

/*----------------------------------------------------------------------
  OnBrowseButton called when the user wants to search for a local file
  params:
  returns:
  ----------------------------------------------------------------------*/
void BgImageDlgWX::OnBrowseButton( wxCommandEvent& event )
{
  // Create a generic filedialog
  wxFileDialog * p_dlg = new wxFileDialog
    (
     this,
     TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
     _T(""),
     _T(""), 
     _T("*.*"),
     wxOPEN | wxCHANGE_DIR // wxCHANGE_DIR -> remember the last directory used.
     );

  // do not force the directory, let wxWidgets choose for the current one
  // p_dlg->SetDirectory(wxGetHomeDir());
  
  if (p_dlg->ShowModal() == wxID_OK)
    {
      XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetValue( p_dlg->GetPath() );
      // destroy the dlg before calling thotcallback because it's a child of this
      // dialog and thotcallback will delete the dialog...
      // so if I do not delete it manualy here it will be deleted twice
      p_dlg->Destroy();
    }
  else
    {
      p_dlg->Destroy();
    }
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user click on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void BgImageDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*)0);
}

/*----------------------------------------------------------------------
  OnRepeatButton
  params:
  returns:
  ----------------------------------------------------------------------*/
void BgImageDlgWX::OnRepeatButton( wxCommandEvent& event )
{
  m_RepeatMode = 0;
  RedrawRepeatButtons();
}

/*----------------------------------------------------------------------
  OnRepeatXButton
  params:
  returns:
  ----------------------------------------------------------------------*/
void BgImageDlgWX::OnRepeatXButton( wxCommandEvent& event )
{
  m_RepeatMode = 1;
  RedrawRepeatButtons();
}

/*----------------------------------------------------------------------
  OnRepeatYButton
  params:
  returns:
  ----------------------------------------------------------------------*/
void BgImageDlgWX::OnRepeatYButton( wxCommandEvent& event )
{
  m_RepeatMode = 2;
  RedrawRepeatButtons();
}

/*----------------------------------------------------------------------
  OnNoRepeatButton
  params:
  returns:
  ----------------------------------------------------------------------*/
void BgImageDlgWX::OnNoRepeatButton( wxCommandEvent& event )
{
  m_RepeatMode = 3;
  RedrawRepeatButtons();
}

/*----------------------------------------------------------------------
  RedrwaRepeatButtons
  params:
  returns:
  ----------------------------------------------------------------------*/
void BgImageDlgWX::RedrawRepeatButtons()
{
  XRCCTRL(*this, "wxID_BBT_REPEAT",   wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*this, "wxID_BBT_REPEAT_X", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*this, "wxID_BBT_REPEAT_Y", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*this, "wxID_BBT_NOREPEAT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  switch (m_RepeatMode)
    {
    case 0:
      XRCCTRL(*this, "wxID_BBT_REPEAT", wxBitmapButton)->SetBackgroundColour( m_OnColour );
      break;
    case 1:
      XRCCTRL(*this, "wxID_BBT_REPEAT_X", wxBitmapButton)->SetBackgroundColour( m_OnColour );
      break;
    case 2:
      XRCCTRL(*this, "wxID_BBT_REPEAT_Y", wxBitmapButton)->SetBackgroundColour( m_OnColour );
      break;
    case 3:
      XRCCTRL(*this, "wxID_BBT_NOREPEAT", wxBitmapButton)->SetBackgroundColour( m_OnColour );
      break;
    }
}

#endif /* _WX */
