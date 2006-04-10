#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"
#include "wx/arrstr.h"

#include "AmayaApp.h"
#include "TextDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

static int    MyRef;
static int    MySubRef;
static char   Buffer[512];

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(TextDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),           TextDlgWX::OnOk )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       TextDlgWX::OnCancel )
  EVT_TEXT_ENTER( XRCID("wxID_TEXT"),         TextDlgWX::OnOk )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  TextDlgWX create the dialog used to 
    - Add CSS file
    - Create/Modify a link
  params:
    + parent : parent window
    + title : dialog title
    + ...
  returns:
  ----------------------------------------------------------------------*/
  TextDlgWX::TextDlgWX( int ref, int subref,
			wxWindow* parent,
			const wxString & title,
			const wxString & label,
			const wxString & value ) :
  AmayaDialog( NULL, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("TextDlgWX"));
  MyRef = ref;
  MySubRef = subref;

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( label );
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  
  // fill initial value
  XRCCTRL(*this, "wxID_Text", wxTextCtrl)->SetValue ( value);
  // set te cursor to the end
  XRCCTRL(*this, "wxID_Text", wxTextCtrl)->SetInsertionPointEnd();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor.
  ----------------------------------------------------------------------*/
TextDlgWX::~TextDlgWX()
{
  /* do not call this one because it cancel the link creation */
  /*  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);*/
}

/*----------------------------------------------------------------------
  OnOk called when the user validates his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void TextDlgWX::OnOk( wxCommandEvent& event )
{
  // return the selected value
  wxString value = XRCCTRL(*this, "wxID_Text", wxTextCtrl)->GetValue( );
  wxASSERT( value.Len() < 512 );
  strcpy( Buffer, (const char*)value.mb_str(wxConvUTF8) );
  ThotCallback (MySubRef, STRING_DATA, (char *)Buffer);
  ThotCallback (MyRef, INTEGER_DATA, (char*)1);
}

/*----------------------------------------------------------------------
  OnCancel called when the user clicks on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void TextDlgWX::OnCancel( wxCommandEvent& event )
{
  // this callback is called into AmayaDialog::OnClose
  // usefull to cancel the link creation process
  //  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  Close();
}

#endif /* _WX */
