#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/valgen.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "wxinclude.h"

#include "InitConfirmDlgWX.h"


//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(InitConfirmDlgWX, wxDialog)
  // Note that the ID here isn't a XRCID, it is one of the standard wx ID's.
  EVT_BUTTON( wxID_OK, InitConfirmDlgWX::OnOK )
  EVT_BUTTON( wxID_CANCEL, InitConfirmDlgWX::OnCancel )
//    EVT_INIT_DIALOG( InitConfirmDlgWX::OnInit )
  EVT_SIZE( InitConfirmDlgWX::OnSize )
END_EVENT_TABLE()

//-----------------------------------------------------------------------------
// Public members
//-----------------------------------------------------------------------------
InitConfirmDlgWX::InitConfirmDlgWX(wxWindow* parent, const wxString & title, const wxString & label)
{    
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("InitConfirmDlgWX"));
  m_Label = label;
  m_Title = title;
    
  SetTitle( m_Title );

  XRCCTRL(*this, "wxAmayaLabelId", wxStaticText)->SetValidator( wxGenericValidator(&m_Label) );

  // TODO : trouver une facon de redimensionner le dialogue pour qu'il prenne la taille du nouveau label
  GetSizer()->Fit( this );
  GetSizer()->SetSizeHints( this );  
  Layout();
  
  SetAutoLayout( TRUE );
}

// Destructor. (Empty, as I don't need anything special done when destructing).
InitConfirmDlgWX::~InitConfirmDlgWX()
{
}

void InitConfirmDlgWX::OnOK( wxCommandEvent& event )
{
  ThotCallback (BaseDialog + ConfirmForm, INTEGER_DATA, (char*) 1);
  EndModal( wxID_OK );
}

void InitConfirmDlgWX::OnCancel( wxCommandEvent& event )
{
  ThotCallback (BaseDialog + ConfirmForm, INTEGER_DATA, (char*) 0); 
  EndModal( wxID_CANCEL );
}

void InitConfirmDlgWX::OnSize( wxSizeEvent& event )
{
  wxLogDebug(_T("InitConfirmDlgWX - OnSize: w=%d h=%d"),
	event.GetSize().GetWidth(),
	event.GetSize().GetHeight() );

  GetSizer()->Fit( this );
  GetSizer()->SetSizeHints( this );  
  Layout();

  //  forward the event to parents
  event.Skip();
}

/*
void InitConfirmDlgWX::OnInit( wxInitDialogEvent& event )
{
    
    wxSize s = GetSizer()->CalcMin();
    wxLogDebug( _T("CalcMin: w=%d h=%d"), s.GetWidth(), s.GetHeight() );
    wxSize s2 = GetSizer()->GetSize();
    wxLogDebug( _T("GetSize: w=%d h=%d"), s2.GetWidth(), s2.GetHeight() );
    
    GetSizer()->Fit( this );
    GetSizer()->RecalcSizes();
    GetSizer()->Layout();
    Layout();
    XRCCTRL(*this, "wxID_OK", wxButton)->Layout();
}
*/

#endif /* _WX */
