#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"

#include "AmayaApp.h"
#include "OpenDocDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(OpenDocDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OPENBUTTON"),   OpenDocDlgWX::OnOpenButton )
  EVT_BUTTON(     XRCID("wxID_BROWSEBUTTON"), OpenDocDlgWX::OnBrowseButton )
  EVT_BUTTON(     XRCID("wxID_CANCELBUTTON"), OpenDocDlgWX::OnCancelButton )
  EVT_TEXT_ENTER( XRCID("wxID_COMBOBOX"),     OpenDocDlgWX::OnOpenButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  OpenDocDlgWX create the dialog used to select a new document
  notice: the combobox is not filled in the constructor, it's filled in wxdialogapi.c
  params:
    + parent : parent window
    + title : dialog title
    + docName : ??? not used
  returns:
  ----------------------------------------------------------------------*/
OpenDocDlgWX::OpenDocDlgWX( int ref,
			    wxWindow* parent,
			    const wxString & title,
			    const wxString & docName,
			    const wxString & filter ) :
  AmayaDialog( NULL, ref ),
  m_Filter(filter)
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("OpenDocDlgWX"));

  wxLogDebug( _T("OpenDocDlgWX::OpenDocDlgWX - title=")+title );
  wxLogDebug( _T("OpenDocDlgWX::OpenDocDlgWX - docName=")+docName );

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL",        wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_LOCATION) ));
  XRCCTRL(*this, "wxID_OPENBUTTON",   wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_OPEN_URL) ));
  XRCCTRL(*this, "wxID_BROWSEBUTTON", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_BROWSE) ));
  XRCCTRL(*this, "wxID_CANCELBUTTON", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));

  // give focus to ...
  XRCCTRL(*this, "wxID_COMBOBOX",     wxComboBox)->SetFocus();


  // TODO ? peutetre mettre un boutton CLEAR
  // SetWindowText (GetDlgItem (hwnDlg, IDC_CLEAR), TtaGetMessage (AMAYA, AM_CLEAR));
  
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
OpenDocDlgWX::~OpenDocDlgWX()
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  AppendURL add a new url to the combobox
  params:
    + url : the url to append to combobox
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::AppendURL( const wxString & url )
{
  XRCCTRL(*this, "wxID_COMBOBOX",     wxComboBox)->Append( url );
}

/*----------------------------------------------------------------------
  SetCurrentURL Sets the text for the combobox text field.
  params:
    + url : the url to setup
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::SetCurrentURL( const wxString & url )
{
  XRCCTRL(*this, "wxID_COMBOBOX",     wxComboBox)->SetValue( url );
}

/*----------------------------------------------------------------------
  OnOpenButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnOpenButton( wxCommandEvent& event )
{
  // get the combobox current url
  wxString url = XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->GetValue( );
  wxLogDebug( _T("OpenDocDlgWX::OnOpenButton - url=")+url );
  
  // allocate a temporary buffer to copy the 'const char *' url buffer 
  char buffer[512];
  wxASSERT( url.Len() < 512 );
  strcpy( buffer, (const char*)url.mb_str(wxConvUTF8) );

  // give the new url to amaya (to do url completion)
  ThotCallback (BaseDialog + URLName,  STRING_DATA, (char *)buffer );
  // create or load the new document
  ThotCallback (m_Ref, INTEGER_DATA, (char*)1);
}

/*----------------------------------------------------------------------
  OnBrowseButton called when the user wants to search for a local file
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnBrowseButton( wxCommandEvent& event )
{
  // Create a generic filedialog
  wxFileDialog * p_dlg = new wxFileDialog
    (
     this,
     TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
     _T(""),
     _T(""), 
     m_Filter,
     wxOPEN | wxCHANGE_DIR /* wxCHANGE_DIR -> remember the last directory used. */
     );

  // do not force the directory, let wxWidgets choose for the current one
  // p_dlg->SetDirectory(wxGetHomeDir());
  
  if (p_dlg->ShowModal() == wxID_OK)
    {
      XRCCTRL(*this, "wxID_COMBOBOX",     wxComboBox)->SetValue( p_dlg->GetPath() );
      // destroy the dlg before calling thotcallback because it's a child of this
      // dialog and thotcallback will delete the dialog...
      // so if I do not delete it manualy here it will be deleted twice
      p_dlg->Destroy();
      // simulate the open command
      OnOpenButton( event );
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
void OpenDocDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);
}

#endif /* _WX */
