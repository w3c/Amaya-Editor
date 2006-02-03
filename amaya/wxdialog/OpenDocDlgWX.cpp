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
#include "thot_sys.h"
static int      Waiting = 0;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(OpenDocDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),              OpenDocDlgWX::OnOpenButton )
  EVT_BUTTON(     XRCID("wxID_CLEAR"),           OpenDocDlgWX::OnClearButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),          OpenDocDlgWX::OnCancelButton )
  EVT_BUTTON(     XRCID("wxID_BUTTON_DIR"),      OpenDocDlgWX::OnDirButton )
  EVT_BUTTON(     XRCID("wxID_BUTTON_FILENAME"), OpenDocDlgWX::OnFilenameButton )
  EVT_TEXT_ENTER( XRCID("wxID_COMBOBOX"),        OpenDocDlgWX::OnOpenButton )
  EVT_COMBOBOX( XRCID("wxID_COMBOBOX"),          OpenDocDlgWX::OnURLSelected )
  EVT_TEXT_ENTER( XRCID("wxID_FILENAME"),        OpenDocDlgWX::OnOpenButton )
  EVT_TEXT_ENTER( XRCID("wxID_DIR"),             OpenDocDlgWX::OnOpenButton )
  EVT_COMBOBOX( XRCID("wxID_PROFILE"),           OpenDocDlgWX::OnProfileSelected )

  EVT_TEXT( XRCID("wxID_FILENAME"), OpenDocDlgWX::OnText_Filename )
  EVT_TEXT( XRCID("wxID_DIR"),      OpenDocDlgWX::OnText_Dir )
  EVT_TEXT( XRCID("wxID_COMBOBOX"), OpenDocDlgWX::OnText_Combobox )
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
OpenDocDlgWX::OpenDocDlgWX( int ref, wxWindow* parent, const wxString & title,
                            const wxString & docName, const wxArrayString & urlList,
                            const wxString & urlToOpen, const wxString & filter,
                            int * p_last_used_filter, const wxString & profiles) :
  AmayaDialog( parent, ref )
  ,m_Filter(filter)
  ,m_LockUpdateFlag(false)
  ,m_pLastUsedFilter(p_last_used_filter)
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("OpenDocDlgWX"));
  // waiting for a return
  Waiting = 1;

  // update dialog labels with given ones
  SetTitle( title );
  //XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_LOCATION) ));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_OPEN_URL) ));
  XRCCTRL(*this, "wxID_CLEAR", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CLEAR) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(0, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_REPLACECURRENT)));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_INNEWTAB)));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(2, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_INNEWWINDOW)));
  XRCCTRL(*this, "wxID_BUTTON_FILENAME", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BROWSE)));

  if (profiles.IsEmpty())
    {
      // Open document
      XRCCTRL(*this, "wxID_LABEL_FILENAME", wxStaticText)->Hide();
      XRCCTRL(*this, "wxID_FILENAME", wxTextCtrl)->Hide();
      XRCCTRL(*this, "wxID_LABEL_DIR", wxStaticText)->Hide();
      XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->Hide();
      XRCCTRL(*this, "wxID_BUTTON_DIR", wxBitmapButton)->Hide();
      XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->Hide();
      XRCCTRL(*this, "wxID_PROFILE_LABEL", wxStaticText)->Hide();
      GetSizer()->SetSizeHints( this );
    }
  else
    {
      // New document
      XRCCTRL(*this, "wxID_LABEL_FILENAME", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_FILE)));
      XRCCTRL(*this, "wxID_LABEL_DIR", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DIRECTORY)));
      XRCCTRL(*this, "wxID_BUTTON_DIR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SEL)));
      XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->SetValue( profiles );
      XRCCTRL(*this, "wxID_PROFILE_LABEL", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_XML_PROFILE)));
    }

  // get the default NEW_LOCATION value : in new tab
  int where_to_open_doc;
  TtaGetEnvInt("NEW_LOCATION", &where_to_open_doc);
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetSelection(where_to_open_doc);

  // setup combobox values
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->Append( urlList );
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue( urlToOpen );
  UpdateDirAndFilenameFromString( urlToOpen );

  // dir separator
  wxChar dir_sep = DIR_SEP;
  m_DirSep = wxString(dir_sep);

  Fit();
  Refresh();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
OpenDocDlgWX::~OpenDocDlgWX()
{
  /* when the dialog is destroyed, It's important to cleanup context */
  if (Waiting)
    // no return done
    ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnDirButton called when the user want to change the folder
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnDirButton( wxCommandEvent& event )
{
  // Create a generic filedialog
  wxDirDialog * p_dlg = new wxDirDialog(this);
  p_dlg->SetStyle(p_dlg->GetStyle() | wxDD_NEW_DIR_BUTTON);
  p_dlg->SetPath(XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->GetValue());
  if (p_dlg->ShowModal() == wxID_OK)
    {
      XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->SetValue( p_dlg->GetPath() );
      p_dlg->Destroy();
    }
  else
    p_dlg->Destroy();
}

/*----------------------------------------------------------------------
  OnFilenameButton called when the user want to change the filename
  to an existing one with the filebrowser
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnFilenameButton( wxCommandEvent& event )
{
  // Create a generic filedialog
  wxFileDialog * p_dlg = new wxFileDialog
    (this,
     TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
     _T(""),
     _T(""),
     m_Filter,
     wxOPEN | wxCHANGE_DIR /* remember last directory */);
  p_dlg->SetPath(XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->GetValue());
  p_dlg->SetFilterIndex(*m_pLastUsedFilter);

  if (p_dlg->ShowModal() == wxID_OK)
    {
      *m_pLastUsedFilter = p_dlg->GetFilterIndex();
      XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue( p_dlg->GetPath() );
      UpdateDirAndFilenameFromString( p_dlg->GetPath() );
      p_dlg->Destroy();
    }
  else
    p_dlg->Destroy();
}

/*----------------------------------------------------------------------
  UpdateComboboxFromDirAndFilename
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::UpdateComboboxFromDirAndFilename()
{
  if (!m_LockUpdateFlag)
    {
      m_LockUpdateFlag = true;
      wxString dir_value      = XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->GetValue();
      wxString filename_value = XRCCTRL(*this, "wxID_FILENAME", wxTextCtrl)->GetValue();
#ifdef _WINDOWS
      if (dir_value.IsEmpty())
        dir_value = _T("C:\\");
#endif /* _WINDOWS */
      if (dir_value.Last() != m_DirSep)
        dir_value += m_DirSep;
      XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue( dir_value + filename_value );
      m_LockUpdateFlag = false;
    }
}

/*----------------------------------------------------------------------
  UpdateDirAndFilenameFromCombobox
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::UpdateDirAndFilenameFromString( const wxString & full_path )
{
  if (!m_LockUpdateFlag)
    {
      m_LockUpdateFlag = true;
      if (!full_path.StartsWith(_T("http")))
        {
          int end_slash_pos = full_path.Find(DIR_SEP, true);
          wxString dir_value = full_path.SubString(0, end_slash_pos);
          wxString filename_value = full_path.SubString(end_slash_pos+1, full_path.Length());
          XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->SetValue(dir_value);
          XRCCTRL(*this, "wxID_FILENAME", wxTextCtrl)->SetValue(filename_value);
        }
      else
        {
          XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->Clear();
          XRCCTRL(*this, "wxID_FILENAME", wxTextCtrl)->Clear();
        }
      m_LockUpdateFlag = false;
    }
}


/*----------------------------------------------------------------------
  OnOpenButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnOpenButton( wxCommandEvent& event )
{
  if (!Waiting)
    // no return done
    return;
  // get the "where to open" indicator
  int where_id = XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->GetSelection();
  ThotCallback (BaseDialog + OpenLocation , INTEGER_DATA, (char*)where_id);

  // get the combobox current url
  wxString url = XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->GetValue( );
  // allocate a temporary buffer to copy the 'const char *' url buffer 
  char buffer[512];
  wxASSERT( url.Len() < 512 );
  strcpy( buffer, (const char*)url.mb_str(wxConvUTF8) );
  // give the new url to amaya (to do url completion)
  ThotCallback (BaseDialog + URLName,  STRING_DATA, (char *)buffer );

  // get the combobox profile
  wxString profile = XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->GetValue( );
  if (!profile.IsEmpty())
    {
      strcpy( buffer, (const char*)profile.mb_str(wxConvUTF8) );
      // give the profile to amaya
      ThotCallback (BaseDialog + DocInfoDocType,  STRING_DATA, (char *)buffer );
    }
  // return done
  Waiting = 0;
  // create or load the new document
  ThotCallback (m_Ref, INTEGER_DATA, (char*)1);

  /* The dialogue is no longer destroyed in the callback to prevent a crash on Mac */
  TtaDestroyDialogue (m_Ref);
}

/*----------------------------------------------------------------------
  OnClearButton called when the user wants to clear each fields
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnClearButton( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue(_T(""));
  XRCCTRL(*this, "wxID_FILENAME", wxTextCtrl)->Clear();
  XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->Clear();
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user click on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnCancelButton( wxCommandEvent& event )
{
  // return done
  Waiting = 0;
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  OnURLSelected
  Description:  update the filename and dir fields
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnURLSelected( wxCommandEvent& event )
{
}

/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  OnProfileSelected
  Description:  select the profile
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnProfileSelected( wxCommandEvent& event )
{
}

/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  OnText_Filename
  Description:  update the filename
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnText_Filename( wxCommandEvent& event )
{
  UpdateComboboxFromDirAndFilename();
  event.Skip();
}

/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  OnText_Dir
  Description:  update the dir
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnText_Dir( wxCommandEvent& event )
{
  UpdateComboboxFromDirAndFilename();
  event.Skip();
}

/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  OnText_Combobox
  Description: select an URL
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnText_Combobox( wxCommandEvent& event )
{
  UpdateDirAndFilenameFromString( XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->GetValue() );
  event.Skip();
}

#endif /* _WX */
