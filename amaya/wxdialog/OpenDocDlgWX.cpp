/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"
#include "wx/arrstr.h"
#include "wx/dir.h"
#include "application.h"
#include "AmayaApp.h"
#include "OpenDocDlgWX.h"

#include "Elemlist.h"
#include "containers.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "thot_sys.h"
#include "templates.h"
#include "templates_f.h"
#include "registry_wx.h"
#include "MENUconf.h"
#include "MENUconf_f.h"
#include "AHTURLTools_f.h"

static int      Waiting = 0;
static int      MyRef = 0;
static int      Ref_doc = 0;
static ThotBool Mandatory_title = FALSE;
static ThotBool New_File = FALSE;
static char    *compound_string;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(OpenDocDlgWX, AmayaDialog)
  EVT_BUTTON(XRCID("wxID_OK"),                    OpenDocDlgWX::OnOpenButton )
  EVT_BUTTON(XRCID("wxID_CLEAR"),                 OpenDocDlgWX::OnClearButton )
  EVT_BUTTON(XRCID("wxID_CANCEL"),                OpenDocDlgWX::OnCancelButton )
  EVT_BUTTON(XRCID("wxID_BUTTON_DIR"),            OpenDocDlgWX::OnDirButton )
  EVT_BUTTON(XRCID("wxID_BUTTON_FILENAME"),       OpenDocDlgWX::OnFilenameButton )
  EVT_TEXT_ENTER( XRCID("wxID_COMBOBOX"),         OpenDocDlgWX::OnOpenButton )
  EVT_COMBOBOX(XRCID("wxID_COMBOBOX"),            OpenDocDlgWX::OnURLSelected )
  EVT_COMBOBOX(XRCID("wxID_PROFILE"),             OpenDocDlgWX::OnProfileSelected )
  EVT_CHECKBOX(XRCID("wxID_USE_TEMPLATE"),        OpenDocDlgWX::OnUseTemplate )
  EVT_COMBOBOX(XRCID("wxID_TEMPLATEFILENAME"),    OpenDocDlgWX::OnTemplateSelected )
  EVT_TEXT_ENTER(XRCID("wxID_TEMPLATEFILENAME"),  OpenDocDlgWX::OnOpenWithTemplate )
  EVT_BUTTON(XRCID("wxID_BUTTON_TEMPLATE"),       OpenDocDlgWX::OnTemplateButton )

END_EVENT_TABLE()


/*----------------------------------------------------------------------
  UpdateTemplateFromDir
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::UpdateTemplateList ()
{
#ifdef TEMPLATES
  Prop_Templates_Path *path = TemplateRepositoryPaths;
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
#endif /* TEMPLATES */
}

/*----------------------------------------------------------------------
  OpenDocDlgWX create the dialog used to select a new document
  notice: the combobox is not filled in the constructor, it's filled in wxdialogapi.c
  params:
    + parent : parent window
    + title : dialog title
  returns:
  ----------------------------------------------------------------------*/
OpenDocDlgWX::OpenDocDlgWX( int ref, wxWindow* parent, const wxString & title,
                            const wxArrayString & urlList,
                            const wxString & urlToOpen, const wxString & filter,
                            int * p_last_used_filter, const wxString & profiles,
                            int doc, ThotBool newfile) :
  AmayaDialog( parent, ref )
  ,m_Filter(filter)
  ,m_LockUpdateFlag(false)
  ,m_pLastUsedFilter(p_last_used_filter)
{
  char      *s;
  char       buffer[MAX_LENGTH], *suffix;
  int        i = 1, len, d;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("OpenDocDlgWX"));
  // waiting for a return
  Waiting = 1;
  MyRef = ref;
  Ref_doc = doc;
  compound_string = TtaGetMessage(AMAYA,AM_COMPOUND_DOCUMENT);

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_CLEAR", wxButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CLEAR) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(0, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_REPLACECURRENT)));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_INNEWTAB)));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(2, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_INNEWWINDOW)));

  if (newfile)
    {
      New_File = TRUE;
      // New HTML document
      XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CREATE) ));
      XRCCTRL(*this, "wxID_BUTTON_DIR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SEL)));
      XRCCTRL(*this, "wxID_BUTTON_FILENAME", wxBitmapButton)->Hide();

      // document charset
      XRCCTRL(*this, "wxID_CHARSET", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DOCINFO_CHARSET)) );
      s = TtaGetEnvString ("DOCUMENT_CHARSET");
      if (s && s[0] != EOS)
        XRCCTRL(*this, "wxID_CHOICE_CHARSET", wxChoice)->SetStringSelection(TtaConvMessageToWX(s));
      else
        XRCCTRL(*this, "wxID_CHOICE_CHARSET", wxChoice)->SetStringSelection( TtaConvMessageToWX("iso-8859-1") );
      if (profiles.IsEmpty())
        {
          // not a new HTML document
          Mandatory_title = FALSE;
          XRCCTRL(*this, "wxID_LABEL_TITLE", wxStaticText)->Hide();
          XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->Hide();
          XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->Hide();
          XRCCTRL(*this, "wxID_PROFILE_LABEL", wxStaticText)->Hide();
          XRCCTRL(*this, "wxID_ERROR", wxStaticText)->Hide();
          XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->Hide();
          XRCCTRL(*this, "wxID_BUTTON_TEMPLATE", wxBitmapButton)->Hide();
          XRCCTRL(*this, "wxID_USE_TEMPLATE", wxCheckBox)->Hide();
          //XRCCTRL(*this, "wxID_TEMPLATE_SIZER", wxStaticBoxSizer)->Hide();
          GetSizer()->SetSizeHints( this );
        }
      else
        {
          // document title
          Mandatory_title = TRUE;
          //XRCCTRL(*this, "wxID_LABEL_TITLE", wxStaticText)->Hide();
          //XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->Hide();
          XRCCTRL(*this, "wxID_LABEL_TITLE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_BM_TITLE)));
          XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->SetValue(TtaConvMessageToWX(""));
      XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SVG_Information)));
#ifdef TEMPLATES
          XRCCTRL(*this, "wxID_BUTTON_TEMPLATE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BROWSE)));
          XRCCTRL(*this, "wxID_USE_TEMPLATE", wxCheckBox)->SetLabel(TtaConvMessageToWX(TtaGetMessage (AMAYA, AM_NEW_TEMPLATE)));
#else /* TEMPLATES */
          XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->Hide();
          XRCCTRL(*this, "wxID_BUTTON_TEMPLATE", wxBitmapButton)->Hide();
          XRCCTRL(*this, "wxID_USE_TEMPLATE", wxCheckBox)->Hide();
#endif /* TEMPLATES */
          XRCCTRL(*this, "wxID_ERROR", wxStaticText)->SetLabel( TtaConvMessageToWX(""));
          // Get the last selected profile
          XRCCTRL(*this, "wxID_PROFILE_LABEL", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_XML_PROFILE)));
          s = TtaGetEnvString ("XHTML_Profile");
          XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->Append(TtaConvMessageToWX(compound_string));
          if (s && s[0] != EOS)
            XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->SetValue(TtaConvMessageToWX(s));
          else
            XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->SetValue( profiles );

          // Update the template combobox with pre-declared templates
          UpdateTemplateList ();
          GetSizer()->SetSizeHints( this );
        }
    }
  else
    {
      New_File = FALSE;
      // Not a new HTML document
      XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_OPEN_URL) ));
      Mandatory_title = FALSE;
      XRCCTRL(*this, "wxID_BUTTON_FILENAME", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BROWSE)));
      XRCCTRL(*this, "wxID_BUTTON_DIR", wxBitmapButton)->Hide();
      XRCCTRL(*this, "wxID_LABEL_TITLE", wxStaticText)->Hide();
      XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->Hide();
      XRCCTRL(*this, "wxID_CHARSET", wxStaticText)->Hide();
      XRCCTRL(*this, "wxID_CHOICE_CHARSET", wxChoice)->Hide();
      XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->Hide();
      XRCCTRL(*this, "wxID_PROFILE_LABEL", wxStaticText)->Hide();
      XRCCTRL(*this, "wxID_ERROR", wxStaticText)->Hide();
      XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->Hide();
      XRCCTRL(*this, "wxID_BUTTON_TEMPLATE", wxBitmapButton)->Hide();
      XRCCTRL(*this, "wxID_USE_TEMPLATE", wxCheckBox)->Hide();
      //XRCCTRL(*this, "wxID_TEMPLATE_SIZER", wxStaticBoxSizer)->Hide();
      GetSizer()->SetSizeHints( this );
    }

  // get the default NEW_LOCATION value : in new tab
  int where_to_open_doc;
  TtaGetEnvInt("NEW_LOCATION", &where_to_open_doc);
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetSelection(where_to_open_doc);

  // Initalize the combobox of file names
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->Append( urlList );
  if (newfile)
    {
      // generate the default name
      strncpy( buffer, (const char*)urlToOpen.mb_str(wxConvUTF8), MAX_LENGTH - 1);
      buffer[MAX_LENGTH - 1] = EOS;
      // keep the current suffix
      len = strlen (buffer);
      while (buffer[len] != '.')
        len--;
      suffix = TtaStrdup (&buffer[len]);
      if (TtaFileExist(buffer))
        {
          do
            {
              sprintf (&buffer[len], "%d%s", i, suffix);
              i++;
            }
          while (TtaFileExist(buffer));
        }
      // check also already open documents
      for (d = 1; d < MAX_DOCUMENTS; d++)
        {
          if (DocumentURLs[d] && !strcmp (DocumentURLs[d], buffer))
          {
            sprintf (&buffer[len], "%d%s", i, suffix);
            i++;
          }
        }
      TtaFreeMemory (suffix);
      XRCCTRL(*this, "wxID_COMBOBOX",wxComboBox )->SetValue(TtaConvMessageToWX(buffer));
    }
  else
      XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue( urlToOpen );

  Fit();
  Refresh();
  SetAutoLayout( TRUE );

  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetFocus();
#if defined(_WINDOWS)
  // select the string
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetSelection(0, -1);
#else /* _WINDOWS */
  // set te cursor to the end
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetInsertionPointEnd();
#endif /* _WINDOWS */

}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
OpenDocDlgWX::~OpenDocDlgWX()
{
  /* when the dialog is destroyed, It's important to cleanup context */
  if (Waiting)
    // no return done
    ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  //else
  // clean up the dialog context
  //TtaDestroyDialogue (MyRef);
}

/*----------------------------------------------------------------------
  OnDirButton called when the user want to change the folder
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnDirButton( wxCommandEvent& event )
{
  wxString        url, dir_value;
  wxDirDialog    *p_dlg;
  char            buffer[MAX_LENGTH];
  int             len, end_pos;

  // Create a generic filedialog
  url = XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->GetValue( );
  end_pos = url.Find(DIR_SEP, true);
  if (end_pos)
    dir_value = url.Mid(0, end_pos+1);
  else
    dir_value = url;
  p_dlg = new wxDirDialog(this);

  // set an initial path
  if (url.StartsWith(_T("http")) ||
      url.StartsWith(TtaConvMessageToWX((TtaGetEnvString ("THOTDIR")))))
    p_dlg->SetPath(TtaConvMessageToWX(TtaGetDocumentsDir()));
  else
    p_dlg->SetPath(dir_value);

  if (p_dlg->ShowModal() == wxID_OK)
    {
      dir_value =  p_dlg->GetPath();
      strncpy (buffer, (const char*)dir_value.mb_str(wxConvUTF8),MAX_LENGTH-3);
      buffer[MAX_LENGTH-2] = EOS;
      if (IsHTMLName (buffer))
        {
          // make sure there is no directory name of the same name
          wxRmdir (dir_value);
        }
      else
        {
          len = strlen (buffer);
          if (buffer[len-1] == DIR_SEP)
            buffer[len-1] = EOS;
          dir_value = url.Mid(end_pos);
          strcat (buffer, (const char*)dir_value.mb_str(wxConvUTF8));
        }
      XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue(TtaConvMessageToWX(buffer));
    }
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
  wxFileDialog  *p_dlg;
  wxString       url, file_value;
  int            id = event.GetId();
  int            temp_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_TEMPLATE"));

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
      p_dlg = new wxFileDialog (this,
                                TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
                                _T(""),_T(""), m_Filter,
                                wxOPEN | wxCHANGE_DIR);

      // get the combobox current url
      url = XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->GetValue( );
      p_dlg->SetFilterIndex(*m_pLastUsedFilter);
    }

 // set an initial path
 if (url.StartsWith(_T("http")) ||
     url.StartsWith(TtaConvMessageToWX((TtaGetEnvString ("THOTDIR")))))
   p_dlg->SetDirectory(TtaConvMessageToWX(TtaGetDocumentsDir()));
  else
    {
      file_value = url.AfterLast (DIR_SEP);
      p_dlg->SetPath (url);
      p_dlg->SetFilename (file_value);
      // Open the directory in the url
      wxString address = url.BeforeLast (DIR_SEP);
      p_dlg->SetDirectory(address);
    }

   // Get the rigth position
  int display_width_px, display_height_px;
  wxPoint pos = wxGetMousePosition();
  wxDisplaySize(&display_width_px, &display_height_px);
  if (pos.x + 200 > display_width_px)
    pos.x = display_width_px - 200;
  if (pos.y + 100 > display_height_px)
    pos.x = display_height_px - 100;
  p_dlg->Move(pos);
  if (p_dlg->ShowModal() == wxID_OK)
    {
      if (id == temp_id)
        XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->SetValue( p_dlg->GetPath() );
      else
        {
          *m_pLastUsedFilter = p_dlg->GetFilterIndex();
          XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue( p_dlg->GetPath() );
          XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetInsertionPointEnd();
        }
    }
  p_dlg->Destroy();

  //Focus the open button after the file blowser is closed
  XRCCTRL(*this, "wxID_OK", wxButton)->SetFocus();
}


/*----------------------------------------------------------------------
  OnOpenButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnUseTemplate( wxCommandEvent& event )
{
  UseTemplate(event.IsChecked());
}


/*----------------------------------------------------------------------
  OnOpenButton called when the user validate his selection with template
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnOpenWithTemplate( wxCommandEvent& event )
{
  OnTemplateSelected (event);
  OnOpenButton (event);
}

/*----------------------------------------------------------------------
  OnOpenButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnOpenButton( wxCommandEvent& event )
{
  wxString        value, title, name;
  char            buffer[MAX_LENGTH];
  char            temp[MAX_LENGTH], docname[MAX_LENGTH];
  int             where_id, end_pos;

  if (!Waiting)
    // no return done
    return;

  // get the template path
  if (TemplateUsed())
    {
      value = XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->GetValue( );
      value = value.Trim(TRUE).Trim(FALSE);
      strncpy (temp, (const char*)value.mb_str(wxConvUTF8), MAX_LENGTH - 1);
      temp[MAX_LENGTH - 1] = EOS;
    }
  else
    temp[0] = EOS;

  if (Mandatory_title)
    {
      // get the selected charset
      value = XRCCTRL(*this, "wxID_CHOICE_CHARSET", wxChoice)->GetStringSelection();
      strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), MAX_LENGTH-1);
      buffer[MAX_LENGTH - 1] = EOS;
      TtaSetEnvString ("DOCUMENT_CHARSET", buffer, TRUE);
    }
  // get the combobox current url
  value = XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->GetValue( );
  value = value.Trim(TRUE).Trim(FALSE);
  strncpy (docname, (const char*)value.mb_str(wxConvUTF8), MAX_LENGTH - 1);
  docname[MAX_LENGTH - 1] = EOS;
  // Get the document title
  if (Mandatory_title)
    {
      // get the donument name as default document title
      end_pos = value.Find(DIR_SEP, true);
      if (end_pos)
        title = value.Mid(end_pos+1, value.Length());
      else
        title = value;
      end_pos = title.Find('.', true);
      if (end_pos)
        name = title.Mid(0, end_pos);
      else
        name = title;
      // use default title
      strncpy( buffer, (const char*)name.mb_str(wxConvUTF8), MAX_LENGTH - 1);
      buffer[MAX_LENGTH - 1] = EOS;
      ThotCallback (BaseDialog + TitleText,  STRING_DATA, (char *)buffer);

      // get the document title
      title = XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->GetValue( );
      if (title.Len() != 0)
#ifdef IV
        {
          // get the document name as default document title
          end_pos = value.Find(DIR_SEP, true);
          if (end_pos)
            title = value.Mid(end_pos+1, value.Length());
          else
            title = value;
          end_pos = title.Find('.', true);
          if (end_pos)
            name = title.Mid(0, end_pos);
          else
            name = title;
              // use default title
              strncpy( buffer, (const char*)name.mb_str(wxConvUTF8), MAX_LENGTH - 1);
              buffer[MAX_LENGTH - 1] = EOS;
              ThotCallback (BaseDialog + TitleText,  STRING_DATA, (char *)buffer);
        }
      else
#endif
        {
          strncpy( buffer, (const char*)title.mb_str(wxConvUTF8), MAX_LENGTH - 1);
          buffer[MAX_LENGTH - 1] = EOS;
          ThotCallback (BaseDialog + TitleText,  STRING_DATA, (char *)buffer);
        }
    }


  // get the "where to open" indicator
  where_id = XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->GetSelection();
  ThotCallback (BaseDialog + OpenLocation , INTEGER_DATA, (char*)where_id);

  // get the combobox profile for new file
  if (New_File)
    {
      value = XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->GetValue( );
      if (!value.IsEmpty())
	{
	  strcpy( buffer, (const char*)value.mb_str(wxConvUTF8) );
	  // give the profile to amaya
	  TtaSetEnvString ("XHTML_Profile", buffer, TRUE);
	  ThotCallback (BaseDialog + DocInfoDocType,  STRING_DATA, (char *)buffer );
	}
    }

  // give the new url to amaya (to do url completion)
  ThotCallback (BaseDialog + URLName,  STRING_DATA, (char *)docname);
  if (temp[0] != EOS)
    {
      CreateInstanceOfTemplate (Ref_doc, temp, docname);
      Waiting = 0;
      TtaDestroyDialogue (MyRef);
    }
  else
    {
      // return done
      Waiting = 0;
      // create or load the new document
      ThotCallback (MyRef, INTEGER_DATA, (char*)1);
    }
  /* The dialogue is no longer destroyed in the callback to prevent a crash on Mac */
  TtaDestroyDialogue (MyRef);
}

/*----------------------------------------------------------------------
  OnClearButton called when the user wants to clear each fields
  params:
  returns:
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnClearButton( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue(_T(""));
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
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  // redirect focus to the canvas because when an action is done 
  // it's more probable that the user wants to type some characteres after executing the action
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  OnURLSelected
  Description:  update the filename and dir fields
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnURLSelected( wxCommandEvent& event )
{
#ifndef _WINDOWS
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetInsertionPointEnd();
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  OnProfileSelected
  Description:  select the profile
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnProfileSelected( wxCommandEvent& event )
{
  wxString     url;
  char         buffer[MAX_LENGTH], suffix[MAX_LENGTH];
  ThotBool     iscompound;

  wxString value = XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->GetValue( );
  if (!value.IsEmpty())
    {
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8));
      iscompound = !strcmp (buffer, compound_string);
      url = XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->GetValue( );
      // check the suffix
      strncpy( buffer, (const char*)url.mb_str(wxConvUTF8), MAX_LENGTH - 1);
      buffer[MAX_LENGTH - 1] = EOS;
      TtaExtractSuffix (buffer, suffix);
      if (iscompound)
        {
          if (strcmp (suffix, "xml"))
            {
              strcat (buffer, ".xml");
              XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue(TtaConvMessageToWX(buffer));
            }
          
          // check the charset
          XRCCTRL(*this, "wxID_CHOICE_CHARSET", wxChoice)->SetStringSelection(TtaConvMessageToWX("utf-8"));
        }
      else if (!strcmp (suffix, "xml"))
        {
          strcat (buffer, ".html");
          XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue(TtaConvMessageToWX(buffer));
        }
    }
}

/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  OnTemplateSelected
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnTemplateSelected( wxCommandEvent& event )
{
  UseTemplate(true);
}


/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  TemplateUsed
  Test if templates are used.
  ----------------------------------------------------------------------*/
bool OpenDocDlgWX::TemplateUsed()const
{
  return XRCCTRL(*this, "wxID_USE_TEMPLATE", wxCheckBox)->GetValue();
}

/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  UseTemplate
  Set if template are used
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::UseTemplate(bool use)
{
  XRCCTRL(*this, "wxID_USE_TEMPLATE", wxCheckBox)->SetValue(use);
  
  XRCCTRL(*this, "wxID_PROFILE", wxComboBox)->Show(!use);
  XRCCTRL(*this, "wxID_PROFILE_LABEL", wxStaticText)->Show(!use);
}

/*----------------------------------------------------------------------
  Class:  OpenDocDlgWX
  Method:  OnTemplateButton
  Show a FileDialog to choose a template file
  ----------------------------------------------------------------------*/
void OpenDocDlgWX::OnTemplateButton( wxCommandEvent& event )
{
  wxFileDialog  *p_dlg;
  wxString       url, file_value;

  p_dlg = new wxFileDialog (this, TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
                            _T(""), _T(""), _T("Templates (*.xtd)|*.xtd"),
                            wxOPEN | wxCHANGE_DIR);
  url = XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->GetValue( );
 // set an initial path
 if (url.StartsWith(_T("http")) ||
     url.StartsWith(TtaConvMessageToWX((TtaGetEnvString ("THOTDIR")))))
   p_dlg->SetDirectory(wxGetHomeDir());
  else
    {
      file_value = url.AfterLast (DIR_SEP);
      p_dlg->SetPath (url);
      p_dlg->SetFilename (file_value);
      // Open the directory in the url
      wxString address = url.BeforeLast (DIR_SEP);
      p_dlg->SetDirectory(address);
    }
  
  if (p_dlg->ShowModal() == wxID_OK)
    {
      XRCCTRL(*this, "wxID_TEMPLATEFILENAME", wxComboBox)->SetValue(p_dlg->GetPath());
      UseTemplate(true);
    }
  p_dlg->Destroy();

  //Focus the open button after the file blowser is closed
  XRCCTRL(*this, "wxID_OK", wxButton)->SetFocus();
}


#endif /* _WX */
