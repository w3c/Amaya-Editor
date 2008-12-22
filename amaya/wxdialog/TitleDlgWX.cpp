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
#include "TitleDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "wxinclude.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(TitleDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),      TitleDlgWX::OnConfirmButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),  TitleDlgWX::OnCancelButton )
  EVT_TEXT_ENTER( XRCID("wxID_TITLE"),   TitleDlgWX::OnConfirmButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  TitleDlgWX create the dialog to change the title
  params:
    + parent : parent window
    + title : dialog title
    + doc_title : the current document title
  returns:
  ----------------------------------------------------------------------*/
  TitleDlgWX::TitleDlgWX( int ref, 
			  wxWindow* parent,
			  const wxString & title,
			  const wxString & doc_title ) :
    AmayaDialog( parent, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("TitleDlgWX"));
 
  SetTitle( title );

  // update dialog labels
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->SetValue(doc_title);

  // set te cursor to the end
  XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->SetInsertionPointEnd();
  // give focus to ...
  XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->SetFocus();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
TitleDlgWX::~TitleDlgWX()
{
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking on confirmbutton
  ----------------------------------------------------------------------*/
void TitleDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  wxString new_title = XRCCTRL(*this, "wxID_TITLE", wxTextCtrl)->GetValue( );
  char     buffer[MAX_LENGTH];

  // allocate a temporary buffer to copy the 'const char *' new_title buffer 
  strncpy( buffer, (const char*)new_title.mb_str(wxConvUTF8), MAX_LENGTH - 1);
  buffer[MAX_LENGTH - 1] = EOS;
  // give the new title to Amaya
  ThotCallback (BaseDialog + TitleText,  STRING_DATA, (char *)buffer );
  // set the new title
  ThotCallback (BaseDialog + TitleForm, INTEGER_DATA, (char*) 1);
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void TitleDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (BaseDialog + TitleForm, INTEGER_DATA, (char*) 0); 
  TtaRedirectFocus();
}

#endif /* _WX */
