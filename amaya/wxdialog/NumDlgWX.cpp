#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"
#include "wx/arrstr.h"
#include "wx/spinctrl.h"

#include "AmayaApp.h"
#include "NumDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(NumDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),           NumDlgWX::OnOk )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       NumDlgWX::OnCancel )
  //  EVT_TEXT_ENTER( XRCID("wxID_TEXT"),         NumDlgWX::OnOk )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  NumDlgWX create the dialog used to 
    - Enter mandatory numeric attributes
  params:
    + parent : parent window
    + title : dialog title
    + ...
  returns:
  ----------------------------------------------------------------------*/
NumDlgWX::NumDlgWX( int ref, int subref,
		    wxWindow* parent,
		    const wxString & title,
		    const wxString & label,
		    int value ) :
    AmayaDialog( NULL, ref )
    ,m_SubRef(subref)
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("NumDlgWX"));

  // update dialog labels with given ones
  SetTitle( title );
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( label );
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  
  // fill initial value
  XRCCTRL(*this, "wxID_NUM", wxSpinCtrl)->SetValue(value);
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor.
  ----------------------------------------------------------------------*/
NumDlgWX::~NumDlgWX()
{
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnOk called when the user validates his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void NumDlgWX::OnOk( wxCommandEvent& event )
{
  // return the selected value
  int value = XRCCTRL(*this, "wxID_NUM", wxSpinCtrl)->GetValue( );
  ThotCallback (m_SubRef, INTEGER_DATA, (char *)value);
  ThotCallback (m_Ref, INTEGER_DATA, (char*)1);
}

/*----------------------------------------------------------------------
  OnCancel called when the user clicks on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void NumDlgWX::OnCancel( wxCommandEvent& event )
{
  Close();
}

#endif /* _WX */
