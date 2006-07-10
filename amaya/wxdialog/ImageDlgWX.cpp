#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"

#include "AmayaApp.h"
#include "ImageDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

static int      MyRef;
static int      Waiting = 0;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ImageDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OPENBUTTON"),   ImageDlgWX::OnOpenButton )
  EVT_BUTTON(     XRCID("wxID_BROWSEBUTTON"), ImageDlgWX::OnBrowseButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       ImageDlgWX::OnCancelButton )
  EVT_TEXT_ENTER( XRCID("wxID_URL"),          ImageDlgWX::OnOpenButton )
  EVT_TEXT_ENTER( XRCID("wxID_ALT"),          ImageDlgWX::OnOpenButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ImageDlgWX create the dialog used to select a new image
  params:
    + parent : parent window
    + title : dialog title
    + urlToOpen : the proposed url
    + alt: the suggested alt
  returns:
  ----------------------------------------------------------------------*/
ImageDlgWX::ImageDlgWX( int ref, wxWindow* parent, const wxString & title,
                        const wxString & urlToOpen, const wxString & alt,
                        const wxString & filter, int * p_last_used_filter ) :
  AmayaDialog( NULL, ref ),
  m_Filter(filter),
  m_pLastUsedFilter(p_last_used_filter)
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("ImageDlgWX"));
  MyRef = ref;
  // waiting for a return
  Waiting = 1;

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_BUTTON_IMG) ));
  XRCCTRL(*this, "wxID_ALT_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_ALT) ));
  XRCCTRL(*this, "wxID_MANDATORY", wxStaticText)->SetLabel( TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_OPENBUTTON", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_BROWSEBUTTON", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BROWSE) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

  XRCCTRL(*this, "wxID_ALT", wxTextCtrl)->SetValue( alt );

  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetValue(urlToOpen  );
#ifdef _WINDOW
  // select the string
  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetSelection(0, -1);
#else /* _WINDOW */
#ifndef _MACOS
  // set te cursor to the end
  XRCCTRL(*this, "wxID_URL", wxTextCtrl)->SetInsertionPointEnd();
#endif /* _MACOS */
#endif /* _WINDOW */

  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
ImageDlgWX::~ImageDlgWX()
{
  if (Waiting)
  // no return done
    ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnOpenButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void ImageDlgWX::OnOpenButton( wxCommandEvent& event )
{
  char     Buffer[512];
  char     Alt[512];

  // get the current url
  wxString url = XRCCTRL(*this, "wxID_URL", wxTextCtrl)->GetValue( );
  wxASSERT( url.Len() < 512 );
  strcpy( Buffer, (const char*)url.mb_str(wxConvUTF8) );
  // give the new url to amaya (to do url completion)
  ThotCallback (BaseImage + ImageURL,  STRING_DATA, (char *)Buffer );

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
void ImageDlgWX::OnBrowseButton( wxCommandEvent& event )
{
  // Create a generic filedialog
  wxFileDialog * p_dlg = new wxFileDialog
    (this,
     TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
     _T(""),
     _T(""), 
     m_Filter,
     wxOPEN | wxCHANGE_DIR /* remember the last directory used. */);

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
      OnOpenButton( event );
    }
  else
    p_dlg->Destroy();
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user click on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void ImageDlgWX::OnCancelButton( wxCommandEvent& event )
{
  // return done
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

#endif /* _WX */
