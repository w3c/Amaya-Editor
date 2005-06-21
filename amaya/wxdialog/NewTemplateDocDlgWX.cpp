#ifdef _WX

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

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(NewTemplateDocDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),              NewTemplateDocDlgWX::OnCreateButton )
  EVT_BUTTON(     XRCID("wxID_CLEAR"),           NewTemplateDocDlgWX::OnClearButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),          NewTemplateDocDlgWX::OnCancelButton )

  EVT_BUTTON(     XRCID("wxID_BUTTON_FILENAME"), NewTemplateDocDlgWX::OnFilenameButton )
  EVT_BUTTON(     XRCID("wxID_BUTTON_DIR"),      NewTemplateDocDlgWX::OnDirButton )
  EVT_BUTTON( XRCID("wxID_BUTTON_TEMPLATENAME_BROWSE"), NewTemplateDocDlgWX::OnTemplatename_BrowseButton )

  EVT_TEXT_ENTER( XRCID("wxID_FILENAME"),        NewTemplateDocDlgWX::OnCreateButton )
  EVT_TEXT_ENTER( XRCID("wxID_DIR"),             NewTemplateDocDlgWX::OnCreateButton )

  EVT_TEXT_ENTER( XRCID("wxID_TEMPLATENAME_BROWSE"),    NewTemplateDocDlgWX::OnCreateButton )
  EVT_COMBOBOX( XRCID("wxID_TEMPLATENAME"),      NewTemplateDocDlgWX::OnTemplatenameSelected )
  

  EVT_TEXT( XRCID("wxID_TEMPLATENAME_BROWSE"),   NewTemplateDocDlgWX::OnText_Templatename_Browse )
  EVT_TEXT( XRCID("wxID_FILENAME"),              NewTemplateDocDlgWX::OnText_Filename )
  EVT_TEXT( XRCID("wxID_DIR"),                   NewTemplateDocDlgWX::OnText_Dirname )
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
					     const wxString & docName,
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

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_OPEN_URL) ));
  XRCCTRL(*this, "wxID_CLEAR", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CLEAR) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_BUTTON_DIR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BROWSE)));
  XRCCTRL(*this, "wxID_BUTTON_FILENAME", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BROWSE)));
  XRCCTRL(*this, "wxID_LABEL_FILENAME", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_FILE)));
  XRCCTRL(*this, "wxID_LABEL_DIR", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DIRECTORY)));

  // set the default WHERE_TO_OPEN value : in new tab
  int where_to_open_doc = 1;
  TtaSetEnvInt("WHERE_TO_OPEN_DOC", where_to_open_doc, FALSE);
  TtaGetEnvInt("WHERE_TO_OPEN_DOC", &where_to_open_doc);
  if (where_to_open_doc < 0 || where_to_open_doc > 2)
    where_to_open_doc = 1;


  // set the default templates directory
  XRCCTRL(*this, "wxID_TEMPLATENAME_BROWSE", wxTextCtrl)->SetValue(templateDir);
  // Update the template combobox with templates in directory "templateDir"
  UpdateTemplateFromDir ();
  
  
  // dir separator
  wxChar dir_sep = DIR_SEP;
  m_DirSep = wxString(dir_sep);

  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
NewTemplateDocDlgWX::~NewTemplateDocDlgWX()
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnDirButton called when the user want to change the folder
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnDirButton( wxCommandEvent& event )
{
  wxDirDialog * p_dlg = new wxDirDialog(this);
  p_dlg->SetStyle(p_dlg->GetStyle() | wxDD_NEW_DIR_BUTTON);
  p_dlg->SetPath(XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->GetValue());
  if (p_dlg->ShowModal() == wxID_OK)
    {
      XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->SetValue( p_dlg->GetPath() );
      p_dlg->Destroy();
    }
  else
    {
      p_dlg->Destroy();
    }
}

/*----------------------------------------------------------------------
  OnFilenameButton called when the user want to change the filename
  to an existing one with the filebrowser
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnFilenameButton( wxCommandEvent& event )
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
  p_dlg->SetPath(XRCCTRL(*this, "wxID_TEMPLATENAME", wxComboBox)->GetValue());
  p_dlg->SetFilterIndex(*m_pLastUsedFilter);

  if (p_dlg->ShowModal() == wxID_OK)
    {
      *m_pLastUsedFilter = p_dlg->GetFilterIndex();
      UpdateDirAndFilenameFromString( p_dlg->GetPath() );
      p_dlg->Destroy();
    }
  else
    {
      p_dlg->Destroy();
    }
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
      wxString dir_value      = XRCCTRL(*this, "wxID_TEMPLATENAME_BROWSE", wxTextCtrl)->GetValue();
#ifdef _WINDOWS
      if (dir_value.IsEmpty())
        dir_value = _T("C:\\");
#endif /* _WINDOWS */
      if (dir_value.Last() != m_DirSep)
        dir_value += m_DirSep;
      
      m_LockUpdateFlag = false;
      
      wxArrayString templateList;
      wxDir::GetAllFiles(dir_value, &templateList, _T("*.html"), wxDIR_FILES);
      XRCCTRL(*this, "wxID_TEMPLATENAME", wxComboBox)->Clear();
      XRCCTRL(*this, "wxID_TEMPLATENAME", wxComboBox)->Append( templateList );
      
    }
}

/*----------------------------------------------------------------------
  UpdateDirAndFilenameFromString
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::UpdateDirAndFilenameFromString( const wxString & full_path )
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
  OnCreateButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnCreateButton( wxCommandEvent& event )
{
  wxString templateUrl = XRCCTRL(*this, "wxID_TEMPLATENAME", wxComboBox)->GetValue( );

  wxString instanceFileUrl = XRCCTRL (*this, "wxID_FILENAME", wxTextCtrl)->GetValue();
  wxString instanceDirUrl = XRCCTRL (*this, "wxID_DIR", wxTextCtrl)->GetValue();
  wxString instanceUrl = instanceDirUrl + m_DirSep + instanceFileUrl;


  // allocate a temporary buffer to copy the 'const char *' template and instance url buffer   
  char bufferTemplate[512];
  wxASSERT( templateUrl.Len() < 512 );
  strcpy( bufferTemplate, (const char*)templateUrl.mb_str(wxConvUTF8) );

    char bufferInstance[512];
  wxASSERT( instanceUrl.Len() < 512 );
  strcpy( bufferInstance, (const char*)instanceUrl.mb_str(wxConvUTF8) );

  // give the new url to amaya (to do url completion)
  ThotCallback (BaseDialog + URLName,  STRING_DATA, (char *)bufferInstance );
  
  ThotCallback (m_Ref, INTEGER_DATA, (char*)1);

  
  CreateInstanceOfTemplate (m_doc, bufferTemplate, bufferInstance,
			    docHTML);
  Close();
}
/*----------------------------------------------------------------------
  OnClearButton called when the user wants to clear each fields
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnClearButton( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_TEMPLATENAME", wxComboBox)->SetValue(_T(""));
  XRCCTRL(*this, "wxID_FILENAME", wxTextCtrl)->Clear();
  XRCCTRL(*this, "wxID_TEMPLATENAME_BROWSE", wxTextCtrl)->Clear();
  XRCCTRL(*this, "wxID_DIR", wxTextCtrl)->Clear();
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user click on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnCancelButton( wxCommandEvent& event )
{
  Close();
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
  Method:  OnText_Filename
  Description:  update the filename
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnText_Filename( wxCommandEvent& event )
{
  event.Skip();
}

/*----------------------------------------------------------------------
  Class:  NewTemplateDocDlgWX
  Method:  OnText_Dir
  Description:  update the dir
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnText_Dirname( wxCommandEvent& event )
{
  event.Skip();
}

/*----------------------------------------------------------------------
  Class:  NewTemplateDocDlgWX
  Method:  OnText_Templatename_Browse
  Description: Select a template from its path
  ----------------------------------------------------------------------*/


void NewTemplateDocDlgWX::OnText_Templatename_Browse (wxCommandEvent& event )
{
  UpdateTemplateFromDir ();
  event.Skip();
}

void NewTemplateDocDlgWX::OnTemplatename_BrowseButton (wxCommandEvent& event)
{
  wxDirDialog * p_dlg = new wxDirDialog(this);
  p_dlg->SetStyle(p_dlg->GetStyle() | wxDD_NEW_DIR_BUTTON);
  p_dlg->SetPath(XRCCTRL(*this, "wxID_TEMPLATENAME_BROWSE", wxTextCtrl)->GetValue());
  if (p_dlg->ShowModal() == wxID_OK)
    {
      XRCCTRL(*this, "wxID_TEMPLATENAME_BROWSE", wxTextCtrl)->SetValue( p_dlg->GetPath() );
      p_dlg->Destroy();
    }
  else
    {
      p_dlg->Destroy();
    }
}

#endif /* _WX */
