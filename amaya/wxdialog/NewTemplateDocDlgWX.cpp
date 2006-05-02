#ifdef TEMPLATES

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"
#include "wx/dir.h"

#include "AmayaApp.h"
#include "NewTemplateDocDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "thot_sys.h"
#include "init_f.h"
#include "templates_f.h"
#include "registry_wx.h"

static int      MyRef = 0;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(NewTemplateDocDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_OK"), NewTemplateDocDlgWX::OnCreateButton )
  EVT_BUTTON( XRCID("wxID_CLEAR"), NewTemplateDocDlgWX::OnClearButton )
  EVT_BUTTON( XRCID("wxID_CANCEL"), NewTemplateDocDlgWX::OnCancelButton )

  EVT_BUTTON( XRCID("wxID_BUTTON_INSTANCEFILENAME"), NewTemplateDocDlgWX::OnInstanceFilenameButton )
  EVT_BUTTON( XRCID("wxID_BUTTON_TEMPLATEDIRNAME"), NewTemplateDocDlgWX::OnTemplateDirNameButton )

  EVT_TEXT_ENTER( XRCID("wxID_FILENAME"), NewTemplateDocDlgWX::OnCreateButton )

  EVT_TEXT_ENTER( XRCID("wxID_TEMPLATEDIRNAME"), NewTemplateDocDlgWX::OnCreateButton )
  EVT_COMBOBOX( XRCID("wxID_TEMPLATEFILENAME"), NewTemplateDocDlgWX::OnTemplatenameSelected )
  
  
  EVT_TEXT( XRCID("wxID_TEMPLATEDIRNAME"), NewTemplateDocDlgWX::OnText_TemplateDirName )
  EVT_TEXT( XRCID("wxID_INSTANCEFILENAME"), NewTemplateDocDlgWX::OnText_InstanceFilename )
  END_EVENT_TABLE()

/*----------------------------------------------------------------------
  NewTemplateDocDlgWX create the dialog used to select a template and select
  the path where the instance will be saved.
  notice: the combobox is not filled in the constructor, it's filled in wxdialogapi.c
  params:
  + parent : parent window
  + title : dialog title
  + docName : ??? not used
  returns:
  ----------------------------------------------------------------------*/
NewTemplateDocDlgWX::NewTemplateDocDlgWX ( int ref,
                                           wxWindow* parent,
                                           int doc,
                                           const wxString & title,
                                           const wxString & templateDir,
                                           const wxString & filter,
                                           int * p_last_used_filter
                                           ) :
  AmayaDialog( parent, ref )
  ,m_Filter(filter)
  ,m_LockUpdateFlag(false)
  ,m_pLastUsedFilter(p_last_used_filter)
  ,m_doc(doc)
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("NewTemplateDocDlgWX"));
  MyRef = ref;

  // update dialog labels with given ones
  SetTitle( title );

  XRCCTRL(*this, "wxID_LABEL_TEMPLATE", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_TEMPLATE) ));
  XRCCTRL(*this, "wxID_LABEL_TEMPLATEFILENAME", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_FILE) ));
  XRCCTRL(*this, "wxID_LABEL_TEMPLATEDIRNAME", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_DIRECTORY) ));
  
  XRCCTRL(*this, "wxID_LABEL_INSTANCE", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_TEMPLATE_INSTANCE) ));
  XRCCTRL(*this, "wxID_LABEL_INSTANCEFILENAME", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_FILE)));
  XRCCTRL(*this, "wxID_BUTTON_INSTANCEFILENAME", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BROWSE)));

  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(0, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_REPLACECURRENT)));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_INNEWTAB)));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(2, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_INNEWWINDOW)));

  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_OPEN_URL) ));
  XRCCTRL(*this, "wxID_CLEAR", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CLEAR) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));

  // set the default WHERE_TO_OPEN value : in new tab
  int where_to_open_doc;
  TtaGetEnvInt("NEW_LOCATION", &where_to_open_doc);
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetSelection(where_to_open_doc);

  // dir separator
  wxChar dir_sep = DIR_SEP;
  m_DirSep = wxString(dir_sep);

  // set the default templates directory
  XRCCTRL(*this, "wxID_TEMPLATEDIRNAME", wxTextCtrl)->SetValue(templateDir);
  // Update the template combobox with templates in directory "templateDir"
  UpdateTemplateFromDir ();
  
  // set the default instance path
  wxString homedir = TtaGetHomeDir();
  wxString filename = TtaConvMessageToWX("New.html");
  XRCCTRL(*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->SetValue(homedir + m_DirSep + filename);
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
NewTemplateDocDlgWX::~NewTemplateDocDlgWX()
{
}


/*----------------------------------------------------------------------
  OnInstanceFilenameButton called when the user want to change the filename
  to an existing one with the filebrowser
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnInstanceFilenameButton( wxCommandEvent& event )
{
  // Create a generic filedialog
  wxFileDialog * p_dlg = new wxFileDialog
    (this,
     TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
     _T(""), _T(""), m_Filter,
     wxSAVE | wxOVERWRITE_PROMPT | wxCHANGE_DIR);
  p_dlg->SetFilterIndex(*m_pLastUsedFilter);
  p_dlg->SetPath(XRCCTRL(*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->GetValue());
  if (p_dlg->ShowModal() == wxID_OK)
    {
      *m_pLastUsedFilter = p_dlg->GetFilterIndex();
      UpdateInstanceFilenameFromString( p_dlg->GetPath() );
    }
  p_dlg->Destroy();
}

/*----------------------------------------------------------------------
  UpdateComboboxFromDirAndFilename
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::UpdateTemplateFromDir ()
{
  if (!m_LockUpdateFlag)
    {
      m_LockUpdateFlag = true;
      wxString dir_value      = XRCCTRL(*this, "wxID_TEMPLATEDIRNAME", wxTextCtrl)->GetValue();
#ifdef _WINDOWS
      if (dir_value.IsEmpty())
        dir_value = _T("C:\\");
#endif /* _WINDOWS */
      if (dir_value.Last() != m_DirSep)
        dir_value += m_DirSep;
      
      m_LockUpdateFlag = false;
      
      wxArrayString templateList;
      wxDir::GetAllFiles(dir_value, &templateList, _T("*.xtd"), wxDIR_FILES);
      XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->Clear();
      XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->Append( templateList );
    }
}

/*----------------------------------------------------------------------
  UpdateInstanceFilenameFromString
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::UpdateInstanceFilenameFromString( const wxString & full_path )
{
  if (!m_LockUpdateFlag)
    {
      m_LockUpdateFlag = true;
      if (!full_path.StartsWith(_T("http")))
        {
          int end_slash_pos = full_path.Find(DIR_SEP, true);
          wxString dir_value = full_path.SubString(0, end_slash_pos);
          wxString filename_value = full_path.SubString(end_slash_pos+1, full_path.Length());
          XRCCTRL(*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->SetValue(dir_value + filename_value);
        }
      else
        {
          XRCCTRL(*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->Clear();
        }
      m_LockUpdateFlag = false;
    }
}

/*----------------------------------------------------------------------
  OnCreateButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnCreateButton( wxCommandEvent& event )
{
  char temp[512], docname[512];

  // get the template path
  wxString templateUrl = XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->GetValue( );
  wxASSERT( templateUrl.Len() < 512 );
  strcpy( temp, (const char*)templateUrl.mb_str(wxConvUTF8) );
  if (TtaFileExist(temp))
    {
      // get the doc instance path
      wxString instanceUrl = XRCCTRL (*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->GetValue();
      wxASSERT( instanceUrl.Len() < 512 );
      strcpy( docname, (const char*)instanceUrl.mb_str(wxConvUTF8) );
      // get the "where to open" indicator
      int where_id = XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->GetSelection();
      ThotCallback (BaseDialog + OpenLocation , INTEGER_DATA, (char*)where_id);
  
      // give the new url to amaya (to do url completion)
      ThotCallback (BaseDialog + URLName,  STRING_DATA, (char *)docname );  
      
      CreateInstanceOfTemplate (m_doc, temp, docname, docHTML);
      TtaDestroyDialogue (MyRef);
    }
  else
    {
      wxMessageDialog msgdlg (this, 
                              TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_BAD_TEMPLATE)),
                              TtaConvMessageToWX(""),
                              (long) wxOK | wxICON_EXCLAMATION);
      msgdlg.ShowModal();
    }
}

/*----------------------------------------------------------------------
  OnClearButton called when the user wants to clear each fields
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnClearButton( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->SetValue(_T(""));
  XRCCTRL(*this, "wxID_FILENAME", wxTextCtrl)->Clear();
  XRCCTRL(*this, "wxID_TEMPLATEDIRNAME", wxTextCtrl)->Clear();
  XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->Clear();
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user click on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnCancelButton( wxCommandEvent& event )
{
  TtaDestroyDialogue (MyRef);      
}

/*----------------------------------------------------------------------
  Class:  NewTemplateDocDlgWX
  Method:  OnTemplateSelected
  Description:  
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnTemplatenameSelected( wxCommandEvent& event )
{
  event.Skip();
}


/*----------------------------------------------------------------------
  Class:  NewTemplateDocDlgWX
  Method:  OnText_InstanceFilename
  Description:  update the filename
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnText_InstanceFilename( wxCommandEvent& event )
{
  event.Skip();
}


/*----------------------------------------------------------------------
  Class:  NewTemplateDocDlgWX
  Method:  OnText_Templatename_Browse
  Description: Select a template from its path
  ----------------------------------------------------------------------*/


void NewTemplateDocDlgWX::OnText_TemplateDirName (wxCommandEvent& event )
{
  UpdateTemplateFromDir ();
  event.Skip();
}

void NewTemplateDocDlgWX::OnTemplateDirNameButton (wxCommandEvent& event)
{
  wxDirDialog * p_dlg = new wxDirDialog(this);
  p_dlg->SetStyle(p_dlg->GetStyle() | wxDD_NEW_DIR_BUTTON);
  p_dlg->SetPath(XRCCTRL(*this, "wxID_TEMPLATEDIRNAME", wxTextCtrl)->GetValue());
  if (p_dlg->ShowModal() == wxID_OK)
    {
      XRCCTRL(*this, "wxID_TEMPLATEDIRNAME", wxTextCtrl)->SetValue( p_dlg->GetPath() );
      p_dlg->Destroy();
    }
  else
    {
      p_dlg->Destroy();
    }
}

#endif /* TEMPLATES */
