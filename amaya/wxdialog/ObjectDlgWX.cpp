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

static int      MyRef;
static int      Waiting = 0;


//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ObjectDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),           ObjectDlgWX::OnOpenButton )
  EVT_BUTTON(     XRCID("wxID_BROWSEBUTTON"), ObjectDlgWX::OnBrowseButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       ObjectDlgWX::OnCancelButton )
  EVT_TEXT_ENTER( XRCID("wxID_COMBOBOX"),     ObjectDlgWX::OnOpenButton )
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
  XRCCTRL(*this, "wxID_MANDATORY", wxStaticText)->SetLabel( TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_BROWSEBUTTON", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BROWSE) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetValue(urlToOpen  );
  // set te cursor to the end
  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetInsertionPointEnd();
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

  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
ObjectDlgWX::~ObjectDlgWX()
{
  if (Waiting)
  // no return done
    ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnMimeTypeCbx called when selecting a MimeType Combobox entry
  ----------------------------------------------------------------------*/
void ObjectDlgWX::OnMimeTypeCbx( wxCommandEvent& event )
{
  wxString wx_mimetype = XRCCTRL(*this, "wxID_MIME_TYPE_CB", wxComboBox)->GetValue ();

  wxASSERT( wx_mimetype.Len() < MAX_LENGTH );
  strcpy( UserMimeType, (const char*)wx_mimetype.mb_str(wxConvUTF8) );
}

/*----------------------------------------------------------------------
  OnOpenButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void ObjectDlgWX::OnOpenButton( wxCommandEvent& event )
{
  char     buffer[512];
  char     Alt[512];

  // get the current url
  wxString url = XRCCTRL(*this, "wxID_URL", wxTextCtrl)->GetValue( );
  wxASSERT( url.Len() < 512 );
  strcpy( buffer, (const char*)url.mb_str(wxConvUTF8) );
  // give the new url to amaya (to do url completion)
  ThotCallback (BaseImage + ImageURL,  STRING_DATA, (char *)buffer );

  // get the current alt
  wxString alt = XRCCTRL(*this, "wxID_ALT", wxTextCtrl)->GetValue( );
  wxASSERT( alt.Len() < 512 );
  strcpy( Alt, (const char*)alt.mb_str(wxConvUTF8) );
  // give the new url to amaya (to do url completion)
  ThotCallback (BaseImage + ImageAlt,  STRING_DATA, (char *)Alt );

  if (Alt[0] == EOS)
    XRCCTRL(*this, "wxID_MANDATORY", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_ALT_MISSING) ));
  else
     {
       // load the image
       // return done
       Waiting = 0;
       ThotCallback (MyRef, INTEGER_DATA, (char*)1);
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
  wxFileDialog * p_dlg = new wxFileDialog
    (
     this,
     TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
     _T(""),
     _T(""), 
     m_Filter,
     wxOPEN | wxCHANGE_DIR /* wxCHANGE_DIR -> remember the last directory used. */
     );
  wxString url = XRCCTRL(*this, "wxID_URL", wxTextCtrl)->GetValue( );
  p_dlg->SetPath(url);
  p_dlg->SetFilterIndex(*m_pLastUsedFilter);

  if (p_dlg->ShowModal() == wxID_OK)
    {
      *m_pLastUsedFilter = p_dlg->GetFilterIndex();
      XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetValue( p_dlg->GetPath() );
      // destroy the dlg before calling thotcallback because it's a child of this
      // dialog and thotcallback will delete the dialog...
      // so if I do not delete it manualy here it will be deleted twice
      p_dlg->Destroy();
      // simulate the open command
      //OnOpenButton( event );
    }
  else
    {
      *m_pLastUsedFilter = p_dlg->GetFilterIndex();
      p_dlg->Destroy();
    }
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user click on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void ObjectDlgWX::OnCancelButton( wxCommandEvent& event )
{
  // return done
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

#endif /* _WX */
