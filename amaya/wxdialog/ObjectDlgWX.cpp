/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"

#include "AmayaApp.h"
#include "ObjectDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "registry_wx.h"

static int      MyRef;
static int      Waiting = 0;
static wxFileDialog *p_dlg = NULL;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ObjectDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_OK"),               ObjectDlgWX::OnOpenButton )
  EVT_BUTTON( XRCID("wxID_BROWSEBUTTON"),     ObjectDlgWX::OnBrowseButton )
  EVT_BUTTON( XRCID("wxID_CANCEL"),           ObjectDlgWX::OnCancelButton )
  EVT_BUTTON( XRCID("wxID_CLEAR"),            ObjectDlgWX::OnClearButton )
  EVT_TOOL( XRCID("wxID_INLINE"),             ObjectDlgWX::OnPosition ) 
  EVT_TOOL( XRCID("wxID_LEFT"),               ObjectDlgWX::OnPosition ) 
  EVT_TOOL( XRCID("wxID_RIGHT"),              ObjectDlgWX::OnPosition ) 
  EVT_TOOL( XRCID("wxID_CENTER"),             ObjectDlgWX::OnPosition ) 
  EVT_TEXT_ENTER( XRCID("wxID_ID_URL"),       ObjectDlgWX::OnOpenButton )
  EVT_TEXT_ENTER( XRCID("wxID_ID_ALT"),       ObjectDlgWX::OnOpenButton )
  EVT_COMBOBOX( XRCID("wxID_MIME_TYPE_CB"),   ObjectDlgWX::OnMimeTypeCbx )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ObjectDlgWX create the dialog used to select a new image
  params:
    + parent : parent window
    + title : dialog title
    + urlToOpen : the proposed url
    + type: the suggested type
  returns:
  ----------------------------------------------------------------------*/
ObjectDlgWX::ObjectDlgWX( int ref, wxWindow* parent, const wxString & title,
			    const wxString & urlToOpen, const wxString & mime_type,
			    const wxString & filter, int * p_last_used_filter) :
  AmayaDialog( NULL, ref ),
  m_Filter(filter),
  m_pLastUsedFilter(p_last_used_filter)
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("ObjectDlgWX"));
  MyRef = ref;
  // waiting for a return
  Waiting = 1;

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_NEWOBJECT) ));
  XRCCTRL(*this, "wxID_TYPE_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_SELECT_MIMETYPE) ));
  XRCCTRL(*this, "wxID_ALT_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_ALT) ));
  XRCCTRL(*this, "wxID_POSITION_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_POSITION) ));
  XRCCTRL(*this, "wxID_MANDATORY", wxStaticText)->SetLabel( TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CLEAR", wxButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CLEAR) ));
  XRCCTRL(*this, "wxID_BROWSEBUTTON", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BROWSE) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

  // set the mime-type
  XRCCTRL(*this, "wxID_MIME_TYPE_CB", wxComboBox)->Append( mime_type );
  if (strcmp (UserMimeType, "image/png"))
    XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	    wxComboBox)->Append( TtaConvMessageToWX( "image/png" ) );
  if (strcmp (UserMimeType, "image/jpeg"))
    XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	    wxComboBox)->Append( TtaConvMessageToWX( "image/jpeg" ) );
  if (strcmp (UserMimeType, "image/gif"))
    XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	    wxComboBox)->Append( TtaConvMessageToWX( "image/gif" ) );
  if (strcmp (UserMimeType, "image/svg+xml"))
    XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	    wxComboBox)->Append( TtaConvMessageToWX( "image/svg+xml" ) );
  if (strcmp (UserMimeType, "application/mathml+xml"))
    XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	    wxComboBox)->Append( TtaConvMessageToWX( "application/mathml+xml" ) );
  if (strcmp (UserMimeType, "text/mathml"))
    XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	    wxComboBox)->Append( TtaConvMessageToWX( "text/mathml" ) );
  if (strcmp (UserMimeType, "text/html"))
    XRCCTRL(*this, "wxID_MIME_TYPE_CB",
	    wxComboBox)->Append( TtaConvMessageToWX( "text/html" ) );
  XRCCTRL(*this, "wxID_MIME_TYPE_CB", wxComboBox)->SetValue( mime_type );

  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetValue(urlToOpen  );

  wxToolBar* tb = XRCCTRL(*this, "wxID_TOOL", wxToolBar);
  ImgPosition = 2;
  switch (ImgPosition)
    {
    case 1:
      tb->ToggleTool(XRCID("wxID_LEFT"), true);
      break;
    case 2:
      tb->ToggleTool(XRCID("wxID_CENTER"), true);
      break;
    case 3:
      tb->ToggleTool(XRCID("wxID_RIGHT"), true);
      break;
    default:
      tb->ToggleTool(XRCID("wxID_INLINE"), true);
      break;
    }

  SetAutoLayout( TRUE );

  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetFocus();
#if defined(_WINDOWS)
  // select the string
  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetSelection(0, -1);
#else /* _WINDOWS */
  // set te cursor to the end
  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetInsertionPointEnd();
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
ObjectDlgWX::~ObjectDlgWX()
{
  if (Waiting)
    {
      // no return done
      Waiting = 0;
      ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
      TtaDestroyDialogue( MyRef );
    }
}

/*----------------------------------------------------------------------
  OnMimeTypeCbx called when selecting a MimeType Combobox entry
  ----------------------------------------------------------------------*/
void ObjectDlgWX::OnMimeTypeCbx( wxCommandEvent& event )
{
  wxString wx_mimetype = XRCCTRL(*this, "wxID_MIME_TYPE_CB", wxComboBox)->GetValue ();
  strncpy( UserMimeType, (const char*)wx_mimetype.mb_str(wxConvUTF8), MAX_LENGTH - 1);
  UserMimeType[MAX_LENGTH - 1] = EOS;
}

/*----------------------------------------------------------------------
  OnClearButton called when the user wants to clear each fields
  params:
  returns:
  ----------------------------------------------------------------------*/
void ObjectDlgWX::OnClearButton( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetValue(_T(""));
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void ObjectDlgWX::OnPosition( wxCommandEvent& event )
{
  wxToolBar* tb = XRCCTRL(*this, "wxID_TOOL", wxToolBar);
  int id = event.GetId();
  if ( id == wxXmlResource::GetXRCID(_T("wxID_INLINE")) )
    {
      tb->ToggleTool(XRCID("wxID_INLINE"), true);
      tb->ToggleTool(XRCID("wxID_LEFT"), false);
      tb->ToggleTool(XRCID("wxID_CENTER"), false);
      tb->ToggleTool(XRCID("wxID_RIGHT"), false);
      ImgPosition = 0;
    }
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_LEFT")) )
    {
      tb->ToggleTool(XRCID("wxID_INLINE"), false);
      tb->ToggleTool(XRCID("wxID_LEFT"), true);
      tb->ToggleTool(XRCID("wxID_CENTER"), false);
      tb->ToggleTool(XRCID("wxID_RIGHT"), false);
      ImgPosition = 1;
    }
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_CENTER")) )
    {
      tb->ToggleTool(XRCID("wxID_INLINE"), false);
      tb->ToggleTool(XRCID("wxID_LEFT"), false);
      tb->ToggleTool(XRCID("wxID_CENTER"), true);
      tb->ToggleTool(XRCID("wxID_RIGHT"), false);
      ImgPosition = 2;
    }
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_RIGHT")) )
    {
      tb->ToggleTool(XRCID("wxID_INLINE"), false);
      tb->ToggleTool(XRCID("wxID_LEFT"), false);
      tb->ToggleTool(XRCID("wxID_CENTER"), false);
      tb->ToggleTool(XRCID("wxID_RIGHT"), true);
      ImgPosition = 3;
    }
}

/*----------------------------------------------------------------------
  OnOpenButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void ObjectDlgWX::OnOpenButton( wxCommandEvent& event )
{
  char     buffer[MAX_LENGTH];

  if (p_dlg)
    {
      p_dlg->Hide();
      p_dlg->Destroy();
      p_dlg = NULL;
    }
  // get the current url
  wxString url = XRCCTRL(*this, "wxID_URL", wxTextCtrl)->GetValue( );
  if (url.Len() == 0)
    {
      Waiting = 0;
      ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
      TtaRedirectFocus();
    }
  else
    {
      strncpy( buffer, (const char*)url.mb_str(wxConvUTF8), MAX_LENGTH - 1);
      buffer[MAX_LENGTH - 1] = EOS;
      // give the new url to amaya (to do url completion)
      ThotCallback (BaseImage + ImageURL,  STRING_DATA, (char *)buffer );
    }
  // get the current alt
  wxString alt = XRCCTRL(*this, "wxID_ALT", wxTextCtrl)->GetValue( );
  if (alt.Len() == 0)
    {
      XRCCTRL(*this, "wxID_MANDATORY", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_ALT_MISSING) ));
      XRCCTRL(*this, "wxID_ALT", wxTextCtrl)->SetFocus();
    }
  else
     {
       // load the image
       // return done
       Waiting = 0;
       strncpy (ImgAlt, (const char*)alt.mb_str(wxConvUTF8), MAX_LENGTH - 1);
       ImgAlt[MAX_LENGTH - 1] = EOS;
       ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
     }
}

/*----------------------------------------------------------------------
  OnBrowseButton called when the user wants to search for a local file
  params:
  returns:
  ----------------------------------------------------------------------*/
void ObjectDlgWX::OnBrowseButton( wxCommandEvent& event )
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

  // set an initial path
  wxString url = XRCCTRL(*this, "wxID_URL", wxTextCtrl)->GetValue( );
  if (url.StartsWith(_T("http")) ||
      url.StartsWith(TtaConvMessageToWX((TtaGetEnvString ("THOTDIR")))))
    p_dlg->SetDirectory(TtaConvMessageToWX(TtaGetDocumentsDir()));
  else
    p_dlg->SetPath(url);
  p_dlg->SetFilterIndex(*m_pLastUsedFilter);
  
  if (p_dlg->ShowModal() == wxID_OK)
    {
      *m_pLastUsedFilter = p_dlg->GetFilterIndex();
      XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetValue( p_dlg->GetPath() );
      // destroy the dlg before calling thotcallback because it's a child of this
      // dialog and thotcallback will delete the dialog...
      // so if I do not delete it manualy here it will be deleted twice
      if (p_dlg)
	p_dlg->Destroy();
      // simulate the open command
      //OnOpenButton( event );
    }
  else if (p_dlg)
    {
      *m_pLastUsedFilter = p_dlg->GetFilterIndex();
      p_dlg->Destroy();
    }
  p_dlg = NULL;
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user click on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void ObjectDlgWX::OnCancelButton( wxCommandEvent& event )
{
  if (p_dlg)
    {
      p_dlg->Hide();
      p_dlg->Destroy();
      p_dlg = NULL;
    }
  // return done
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
}

#endif /* _WX */
