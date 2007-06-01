#ifdef TEMPLATES

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"
#include "wx/arrstr.h"
#include "wx/dir.h"
#include "application.h"
#include "AmayaApp.h"
#include "NewTemplateDocDlgWX.h"

#include "Elemlist.h"
#include "containers.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "thot_sys.h"
#include "init_f.h"
#include "templates.h"
#include "templates_f.h"
#include "registry_wx.h"
#include "MENUconf_f.h"

static int      MyRef = 0;
static int      Mydoc = 0;
static int      Waiting = 0;



//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(NewTemplateDocDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_OK"), NewTemplateDocDlgWX::OnCreateButton )
  EVT_BUTTON( XRCID("wxID_CLEAR"), NewTemplateDocDlgWX::OnClearButton )
  EVT_BUTTON( XRCID("wxID_CANCEL"), NewTemplateDocDlgWX::OnCancelButton )
  EVT_BUTTON( XRCID("wxID_BUTTON_INSTANCENAME"), NewTemplateDocDlgWX::OnInstanceFilenameButton )
  EVT_BUTTON( XRCID("wxID_BUTTON_TEMPLATE"), NewTemplateDocDlgWX::OnInstanceFilenameButton )
  EVT_BUTTON( XRCID("wxID_BUTTON_INSTANCEDIR"), NewTemplateDocDlgWX::OnDirNameButton )
  EVT_TEXT_ENTER( XRCID("wxID_FILENAME"), NewTemplateDocDlgWX::OnCreateButton )

  EVT_TEXT_ENTER( XRCID("wxID_TEMPLATEFILENAME"), NewTemplateDocDlgWX::OnCreateButton )
  EVT_TEXT_ENTER( XRCID("wxID_INSTANCEDIR"), NewTemplateDocDlgWX::OnCreateButton )
  EVT_TEXT_ENTER( XRCID("wxID_INSTANCEFILENAME"), NewTemplateDocDlgWX::OnCreateButton )
  EVT_COMBOBOX( XRCID("wxID_TEMPLATEFILENAME"), NewTemplateDocDlgWX::OnTemplatenameSelected )
  EVT_TEXT( XRCID("wxID_INSTANCE"), NewTemplateDocDlgWX::OnText_InstanceDirName )
  EVT_TEXT( XRCID("wxID_INSTANCEFILENAME"), NewTemplateDocDlgWX::OnText_InstanceFilename )
  EVT_CLOSE( NewTemplateDocDlgWX::OnClose )
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
                                           const wxString & filter,
                                           int * p_last_used_filter
                                           ) :
  m_Filter(filter)
  ,m_LockUpdateFlag(false)
  ,m_pLastUsedFilter(p_last_used_filter)
{
  char      buffer[MAX_LENGTH];
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("NewTemplateDocDlgWX"));
  Waiting = 1;
  MyRef = ref;
  Mydoc = doc;
  // update dialog labels with given ones
  SetTitle( title );
  GetTemplatesConf ();

  XRCCTRL(*this, "wxID_LABEL_INSTANCEDIR", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_DIRECTORY) ));
  
  XRCCTRL(*this, "wxID_LABEL_INSTANCEFILENAME", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_TEMPLATE_INSTANCE) ));
  XRCCTRL(*this, "wxID_BUTTON_TEMPLATE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SEL)));
  XRCCTRL(*this, "wxID_BUTTON_INSTANCENAME", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BROWSE)));
  XRCCTRL(*this, "wxID_BUTTON_INSTANCEDIR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SEL)));

  XRCCTRL(*this, "wxID_LABEL_TITLE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_BM_TITLE)));
  XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->SetValue(TtaConvMessageToWX(""));

  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(0, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_REPLACECURRENT)));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_INNEWTAB)));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(2, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_INNEWWINDOW)));

  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_OPEN_URL) ));
  XRCCTRL(*this, "wxID_CLEAR", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CLEAR) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_ERROR", wxStaticText)->SetLabel(TtaConvMessageToWX(""));

  // set the default WHERE_TO_OPEN value : in new tab
  int where_to_open_doc;
  TtaGetEnvInt("NEW_LOCATION", &where_to_open_doc);
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetSelection(where_to_open_doc);

  // dir separator
  wxChar dir_sep = DIR_SEP;
  m_DirSep = wxString(dir_sep);

  // set the default instance path
  wxString homedir = TtaGetHomeDir ();
  wxString filename = TtaConvMessageToWX ("New.html");
  wxString   full_path = homedir + m_DirSep + filename;

  // propose a new filename
  strncpy( buffer, (const char*)full_path.mb_str(wxConvUTF8), MAX_LENGTH - 1);
  buffer[MAX_LENGTH - 1] = EOS;
  if (TtaFileExist(buffer))
    {
      int        i = 1, len;
      len = strlen (buffer);
      while (buffer[len] != '.')
        len--;
      do
        {
          sprintf (&buffer[len], "%d.html", i);
          i++;
        }
      while (TtaFileExist(buffer));
      full_path = TtaConvMessageToWX (buffer);
    }
  XRCCTRL(*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->SetValue(full_path);
  UpdateDirFromString (full_path);

  // Update the template combobox with pre-declared templates
  UpdateTemplateList ();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
NewTemplateDocDlgWX::~NewTemplateDocDlgWX()
{
  if (Waiting)
    {
      TtaDestroyDialogue (MyRef);
      Waiting = 0;
    }
}


/*----------------------------------------------------------------------
  UpdateDirFromString
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::UpdateDirFromString( const wxString & full_path)
{
  if (!m_LockUpdateFlag)
    {
      m_LockUpdateFlag = true;
      if (!full_path.StartsWith(_T("http")))
        {
          int end_slash_pos = full_path.Find(DIR_SEP, true);
          wxString dir_value = full_path.SubString(0, end_slash_pos);
          XRCCTRL(*this, "wxID_INSTANCEDIR", wxTextCtrl)->SetValue(dir_value);
        }
      else
        XRCCTRL(*this, "wxID_INSTANCEDIR", wxTextCtrl)->Clear();
      m_LockUpdateFlag = false;
    }
}

/*----------------------------------------------------------------------
  OnInstanceFilenameButton called when the user want to change the filename
  to an existing one with the filebrowser
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnInstanceFilenameButton( wxCommandEvent& event )
{
  wxFileDialog  *p_dlg;
  wxString       url;
  int id = event.GetId();
  int temp_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_TEMPLATE"));

  // Create a generic filedialog
  if (id == temp_id)
    {
      p_dlg = new wxFileDialog(this,
                               TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
                               _T(""), _T(""), _T("Templates (*.xtd)|*.xtd"),
                               wxOPEN | wxCHANGE_DIR);
      url = XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->GetValue( );
    }
  else
    {
      p_dlg = new wxFileDialog(this,
                               TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
                               _T(""), _T(""), m_Filter,
                               wxOPEN | wxCHANGE_DIR);
      url = XRCCTRL(*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->GetValue( );
      p_dlg->SetFilterIndex(*m_pLastUsedFilter);
    }

  // set an initial path
  if (url.StartsWith(_T("http")) ||
      url.StartsWith(TtaConvMessageToWX((TtaGetEnvString ("THOTDIR")))))
    p_dlg->SetDirectory(wxGetHomeDir());
  else
    p_dlg->SetPath(url);
      
  if (p_dlg->ShowModal() == wxID_OK)
    {
      if (id == temp_id)
        {
            XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->SetValue( p_dlg->GetPath() );
        }
      else
        {
          *m_pLastUsedFilter = p_dlg->GetFilterIndex();
          UpdateInstanceFromString( p_dlg->GetPath() );
        }
    }
  p_dlg->Destroy();
}

/*----------------------------------------------------------------------
  UpdateTemplateFromDir
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::UpdateTemplateList ()
{
  Prop_Templates       prop = GetProp_Templates();
  Prop_Templates_Path *path = prop.FirstPath;
  wxArrayString        templateList;
  wxString             value;
  bool                 initialized = false;

  if (!m_LockUpdateFlag)
    {
      m_LockUpdateFlag = true;
      XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->Clear();
      XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->SetValue( TtaConvMessageToWX(""));
      while (path)
        {
          value = TtaConvMessageToWX(path->Path);
          XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->Append(value);
          if (!initialized)
            {
              XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->SetStringSelection(value);
              initialized = true;
            }
          path = path->NextPath;
        }
      m_LockUpdateFlag = false;
   }
}

/*----------------------------------------------------------------------
  UpdateInstanceFromDir
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::UpdateInstanceFromDir ()
{
  wxString value, dir_value, filename_value;
  
  if (!m_LockUpdateFlag)
    {
      m_LockUpdateFlag = true;
      value = XRCCTRL (*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->GetValue();
      int end_slash_pos = value.Find(DIR_SEP, true);
      filename_value = value.SubString(end_slash_pos+1, value.Length());
      dir_value = XRCCTRL(*this, "wxID_INSTANCEDIR", wxTextCtrl)->GetValue();
#ifdef _WINDOWS
      if (dir_value.IsEmpty())
        dir_value = _T("C:\\");
#endif /* _WINDOWS */
      if (dir_value.Last() != m_DirSep)
        dir_value += m_DirSep;
      
      m_LockUpdateFlag = false;
      XRCCTRL(*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->SetValue(dir_value + filename_value);
    }
}

/*----------------------------------------------------------------------
  UpdateInstanceFromString
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::UpdateInstanceFromString( const wxString & full_path )
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
        XRCCTRL(*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->Clear();
      UpdateDirFromString (full_path);
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
  wxString        value, title;
  char            temp[MAX_LENGTH], docname[MAX_LENGTH];
  char            buffer[MAX_LENGTH];

  if (!Waiting)
    // avoid double click
    return;
  // get the template path
  value = XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->GetValue( );
  strncpy (temp, (const char*)value.mb_str(wxConvUTF8), MAX_LENGTH - 1);
  temp[MAX_LENGTH - 1] = EOS;
  if (!strncmp (temp, "http:", 5) || TtaFileExist(temp))
    {
      // get the doc instance path
      value = XRCCTRL (*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->GetValue();
      strncpy (docname, (const char*)value.mb_str(wxConvUTF8), MAX_LENGTH - 1);
      docname[MAX_LENGTH - 1] = EOS;

      // get the document title
      title = XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->GetValue( );
      if (title.Len() == 0)
        {
          if (Waiting == 1)
            {
              // request the title
              XRCCTRL(*this, "wxID_ERROR", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage (AMAYA, AM_MISSING_TITLE)));
              Waiting = 2;
              return;
            }
          else
            {
              int end_slash_pos = value.Find(DIR_SEP, true);
              m_LockUpdateFlag = true;
              title = value.SubString(end_slash_pos+1, value.Length());
              m_LockUpdateFlag = false;
              strncpy( buffer, (const char*)title.mb_str(wxConvUTF8), MAX_LENGTH - 1);
              buffer[MAX_LENGTH - 1] = EOS;
              ThotCallback (BaseDialog + TitleText,  STRING_DATA, (char *)buffer);
            }
        }
      else
        {
          strncpy( buffer, (const char*)title.mb_str(wxConvUTF8), MAX_LENGTH - 1);
          buffer[MAX_LENGTH - 1] = EOS;
          ThotCallback (BaseDialog + TitleText,  STRING_DATA, (char *)buffer);
        }

      // get the "where to open" indicator
      int where_id = XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->GetSelection();
      ThotCallback (BaseDialog + OpenLocation , INTEGER_DATA, (char*)where_id);
  
      // give the new url to amaya (to do url completion)
      ThotCallback (BaseDialog + URLName, STRING_DATA, (char *)docname);  
      
      CreateInstanceOfTemplate (Mydoc, temp, docname);
      TtaDestroyDialogue (MyRef);
    }
  else
    {
      wxMessageDialog msgdlg (this, 
                              TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_BAD_TEMPLATE)),
                              TtaConvMessageToWX(""),
                              (long) wxOK | wxICON_EXCLAMATION);
      msgdlg.ShowModal();
      TtaDestroyDialogue (MyRef);
    }
  // return done
  Waiting = 1;
}

/*----------------------------------------------------------------------
  OnClearButton called when the user wants to clear each fields
  params:
  returns:
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnClearButton( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->Clear();
  XRCCTRL(*this, "wxID_INSTANCEDIR", wxTextCtrl)->Clear();
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
  OnClose
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnClose( wxCloseEvent& event )
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
  wxString   full_path = XRCCTRL(*this, "wxID_INSTANCEFILENAME", wxTextCtrl)->GetValue();
  UpdateDirFromString (full_path);
  event.Skip();
}


/*----------------------------------------------------------------------
  Class:  NewTemplateDocDlgWX
  Method:  OnText_InstanceDirName
  Description: Select a template from its path
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnText_InstanceDirName (wxCommandEvent& event )
{
  UpdateInstanceFromDir ();
  event.Skip();
}

/*----------------------------------------------------------------------
  Class:  NewTemplateDocDlgWX
  Method:  OnDirNameButton
  Description: Select a template or instance from its path
  ----------------------------------------------------------------------*/
void NewTemplateDocDlgWX::OnDirNameButton (wxCommandEvent& event)
{
  wxDirDialog *p_dlg = new wxDirDialog(this);
  //p_dlg->SetStyle(p_dlg->GetStyle() | wxDD_NEW_DIR_BUTTON);
  p_dlg->SetPath(XRCCTRL(*this, "wxID_INSTANCEDIR", wxTextCtrl)->GetValue());
  if (p_dlg->ShowModal() == wxID_OK)
    {
      XRCCTRL(*this, "wxID_INSTANCEDIR", wxTextCtrl)->SetValue( p_dlg->GetPath() );
      p_dlg->Destroy();
    }
  else
    p_dlg->Destroy();
}

#endif /* TEMPLATES */
