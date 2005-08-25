#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/spinctrl.h"
#include "AmayaApp.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

#include "CreateTableDlgWX.h"

static int      MyRef;
static int      Waiting = 0;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CreateTableDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_OK"), CreateTableDlgWX::OnConfirmButton )
  EVT_BUTTON( XRCID("wxID_CANCEL"),  CreateTableDlgWX::OnCancelButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  CreateTableDlgWX create the CreateTable dialog 
  params:
    + parent : parent window
    + caption : dialog caption (including the document name)
    + border: -1 if there is no border else the initial value
  ----------------------------------------------------------------------*/
  CreateTableDlgWX::CreateTableDlgWX( int ref, 
				      wxWindow* parent,
				      int def_cols,
				      int def_rows,
				      int def_border,
				      const wxString & caption ) : 
    AmayaDialog( parent, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("CreateTableDlgWX"));
  SetTitle( caption );
  MyRef = ref;
  // waiting for a return
  Waiting = 1;

  // update dialog labels
  XRCCTRL(*this, "wxID_NUMBER_COL_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_COLS) ));
  XRCCTRL(*this, "wxID_NUMBER_ROW_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_ROWS) ));


  // update dialog Spin Ctrls
  XRCCTRL(*this, "wxID_NUMBER_COL", wxSpinCtrl)->SetValue(def_cols);
  XRCCTRL(*this, "wxID_NUMBER_ROW", wxSpinCtrl)->SetValue(def_rows);
  if (def_border >= 0)
    {
      XRCCTRL(*this, "wxID_TABLE_BORDER_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BORDER) ));
      XRCCTRL(*this, "wxID_TABLE_BORDER", wxSpinCtrl)->SetValue(def_border);
    }
  else
    {
      XRCCTRL(*this, "wxID_TABLE_BORDER_TXT", wxStaticText)->Hide();
      XRCCTRL(*this, "wxID_TABLE_BORDER", wxSpinCtrl)->Hide();
    }
  // update button labels
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

  // Gives the focus to the first spin control
  //  XRCCTRL(*this, "wxID_NUMBER_COL", wxSpinCtrl )->SetFocus();

  Layout();
  
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
CreateTableDlgWX::~CreateTableDlgWX()
{
  /* when the dialog is destroyed, It's important to cleanup context */
  if (Waiting)
  // no return done
    ThotCallback (MyRef, INTEGER_DATA, (char*) 0); 
}

/*----------------------------------------------------------------------
  OnConfirmButton called when clicking on confirmbutton
  ----------------------------------------------------------------------*/
void CreateTableDlgWX::OnConfirmButton( wxCommandEvent& event )
{
  m_cols = XRCCTRL(*this, "wxID_NUMBER_COL", wxSpinCtrl)->GetValue();
  m_rows = XRCCTRL(*this, "wxID_NUMBER_ROW", wxSpinCtrl)->GetValue();
  m_border = XRCCTRL(*this, "wxID_TABLE_BORDER", wxSpinCtrl)->GetValue();
  // return done
  Waiting = 0;
  ThotCallback (BaseDialog + TableCols, INTEGER_DATA, (char *) m_cols);
  ThotCallback (BaseDialog + TableRows, INTEGER_DATA, (char *) m_rows);
  ThotCallback (BaseDialog + TableBorder, INTEGER_DATA, (char *) m_border);
  ThotCallback (BaseDialog + TableForm, INTEGER_DATA, (char *) 1);

}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void CreateTableDlgWX::OnCancelButton( wxCommandEvent& event )
{
  // return done
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char *) 0);
}

#endif /* _WX */
