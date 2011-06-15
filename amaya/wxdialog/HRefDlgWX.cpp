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

#include "AmayaApp.h"
#include "HRefDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "registry_wx.h"

static int Waiting = 0;
static int m_doc = 0;
static int MyRef = 0;
static wxFileDialog *p_dlg = NULL;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(HRefDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),           HRefDlgWX::OnOk )
  EVT_BUTTON(     XRCID("wxID_BROWSE"),       HRefDlgWX::OnBrowse )
  EVT_BUTTON(     XRCID("wxID_CLICK"),        HRefDlgWX::OnClick )
  EVT_BUTTON(     XRCID("wxID_DELETE"),       HRefDlgWX::OnDelete )
  EVT_BUTTON(     XRCID("wxID_CLEAR"),        HRefDlgWX::OnClear )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       HRefDlgWX::OnCancel )
  EVT_TEXT_ENTER( XRCID("wxID_COMBOBOX"),     HRefDlgWX::OnOk )
  EVT_COMBOBOX( XRCID("wxID_COMBOBOX"),       HRefDlgWX::OnURLSelected )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  HRefDlgWX create the dialog used to 
    - Add CSS file
    - Create/Modify a link
  params:
    + parent : parent window
    + title : dialog title
    + ...
  returns:
  ----------------------------------------------------------------------*/
HRefDlgWX::HRefDlgWX( int ref, wxWindow* parent, const wxString & title,
                      const wxArrayString & url_list, const wxString & wx_init_value,
                      const wxString & filter, int * p_last_used_filter,
                      int doc) :
  AmayaDialog( parent, ref ),
  m_Filter(filter),
  m_pLastUsedFilter(p_last_used_filter)
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("HRefDlgWX"));
  Waiting = 1;
  m_doc = doc;
  MyRef = ref;

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_LOCATION) ));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_BROWSE", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BROWSE) ));
  XRCCTRL(*this, "wxID_CLICK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CLICK) ));
  XRCCTRL(*this, "wxID_CLICK", wxButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CREATE_BY_CLICK) ));
  XRCCTRL(*this, "wxID_CLEAR", wxButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CLEAR) ));
  XRCCTRL(*this, "wxID_DELETE", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_DELETE_LINK) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));

  // fill the combobox with url list
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->Append(wx_init_value);
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->Append(url_list);
  // initialize it
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue(wx_init_value);

  SetAutoLayout( TRUE );
  XRCCTRL(*this, "wxID_DELETE", wxButton)->Show(!LinkAsCSS);
  
  // give focus to ...
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetFocus();
  // set te cursor to the end
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetSelection (0, -1);
}

/*----------------------------------------------------------------------
  Destructor.
  ----------------------------------------------------------------------*/
HRefDlgWX::~HRefDlgWX()
{
  /* do not call this one because it cancel the link creation */
  if (Waiting == 1)
    {
      Waiting = 0;
      ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
      TtaDestroyDialogue( MyRef );
      m_doc = 0;
    }
}

/*----------------------------------------------------------------------
  OnURLSelected new value selected
  ----------------------------------------------------------------------*/
void HRefDlgWX::OnURLSelected( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetInsertionPointEnd();
}

/*----------------------------------------------------------------------
  OnOk called when the user validates his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void HRefDlgWX::OnOk( wxCommandEvent& event )
{
  if (p_dlg)
    {
      p_dlg->Hide();
      p_dlg->Destroy();
      p_dlg = NULL;
    }
  // get the combobox current url
  wxString url = XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->GetValue( );
  url = url.Trim(TRUE).Trim(FALSE);
  Waiting = 0;
  // allocate a temporary buffer to copy the 'const char *' url buffer 
  char buffer[MAX_LENGTH * 4];
  strncpy( buffer, (const char*)url.mb_str(wxConvUTF8),  MAX_LENGTH);
  buffer[MAX_LENGTH] = EOS;
  // give the new url to amaya (to do url completion)
  ThotCallback (BaseDialog + AttrHREFText, STRING_DATA, (char *)buffer);
  // create or load the new document
  ThotCallback (MyRef, INTEGER_DATA, (char*)1);
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnBrowse called when the user wants to search for a local file
  params:
  returns:
  ----------------------------------------------------------------------*/
void HRefDlgWX::OnBrowse( wxCommandEvent& event )
{
  // Create a generic filedialog
  if (p_dlg)
    {
      p_dlg->Raise();
      return;
    }
  p_dlg = new wxFileDialog
    (
     this,
     TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
     _T(""),
     _T(""), 
     m_Filter,
     wxOPEN | wxCHANGE_DIR /* wxCHANGE_DIR -> remember the last directory used. */
     );
  wxString url = XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->GetValue();
  if (url.IsEmpty())
    url = TtaConvMessageToWX(DocumentURLs[m_doc]);

  // set an initial path
  if (LinkAsImport)
    p_dlg->SetDirectory(TtaConvMessageToWX(TtaGetDocumentsDir()));    
  else if (url.StartsWith(_T("http")) ||
           url.StartsWith(TtaConvMessageToWX((TtaGetEnvString ("THOTDIR")))))
    p_dlg->SetDirectory(TtaConvMessageToWX(TtaGetDocumentsDir()));
  else
   p_dlg->SetPath(url);
  p_dlg->SetFilterIndex(*m_pLastUsedFilter);
  
  if (p_dlg->ShowModal() == wxID_OK)
    {
      *m_pLastUsedFilter = p_dlg->GetFilterIndex();
      XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue( p_dlg->GetPath() );
      // destroy the dlg before calling thotcallback because it's a child of this
      // dialog and thotcallback will delete the dialog...
      // so if I do not delete it manualy here it will be deleted twice
      if (p_dlg)
	{
	  p_dlg->Destroy();
	  // simulate the confirm command
	  OnOk( event );
	}
    }
  else if (p_dlg)
    {
      *m_pLastUsedFilter = p_dlg->GetFilterIndex();
      p_dlg->Destroy();
    }
  p_dlg = NULL;
}

/*----------------------------------------------------------------------
  OnDelete called when the user clicks on delete button
  params:
  returns:
  ----------------------------------------------------------------------*/
void HRefDlgWX::OnDelete( wxCommandEvent& event )
{
  if (p_dlg)
    {
      p_dlg->Hide();
      p_dlg->Destroy();
      p_dlg = NULL;
    }
  ThotCallback (MyRef, INTEGER_DATA, (char*) 4);
  ThotCallback (MyRef, INTEGER_DATA, (char*)1);
}

/*----------------------------------------------------------------------
  OnCancel called when the user clicks on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void HRefDlgWX::OnCancel( wxCommandEvent& event )
{
  // this callback is called into AmayaDialog::OnClose
  // usefull to cancel the link creation process
  //  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  //Close();
  if (p_dlg)
    {
      p_dlg->Hide();
      p_dlg->Destroy();
      p_dlg = NULL;
    }
  TtaDestroyDialogue( MyRef );
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnClick called when the user clicks on click button : wants to click a document
  params:
  returns:
  ----------------------------------------------------------------------*/
void HRefDlgWX::OnClick( wxCommandEvent& event )
{
  Waiting = 2;
  // no cancel  must be generated
  ThotCallback (MyRef, INTEGER_DATA, (char*) 3);
  // the dialog could be redisplayed
  Waiting = 1;
}

/*----------------------------------------------------------------------
  OnClear called when the user clicks on clear button
  params:
  returns:
  ----------------------------------------------------------------------*/
void HRefDlgWX::OnClear( wxCommandEvent& event )
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 4);
  XRCCTRL(*this, "wxID_COMBOBOX", wxComboBox)->SetValue( _T("") );
}

#endif /* _WX */
