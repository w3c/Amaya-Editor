/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
  EVT_BUTTON( XRCID("wxID_FORMAT_BUTTON"),  CreateTableDlgWX::OnSelectFormat )
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
  ThotBool   valset;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("CreateTableDlgWX"));
  SetTitle( caption );
  MyRef = ref;
  // waiting for a return
  Waiting = 1;

  // update dialog labels
  XRCCTRL(*this, "wxID_PARAMETERS", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_OPTIONS) ));
  XRCCTRL(*this, "wxID_SIZE", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_SIZE) ));
  XRCCTRL(*this, "wxID_FORMAT_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_USE_THEME) ));
  XRCCTRL(*this, "wxID_NUMBER_COL_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_COLS) ));
  XRCCTRL(*this, "wxID_NUMBER_ROW_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_ROWS) ));
  // extend
  XRCCTRL(*this, "wxID_EXTEND_WIDTH", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_MAX_WIDTH) ));
  TtaGetEnvBoolean ("TABLE_EXTEND_WIDTH", &valset);
  XRCCTRL(*this, "wxID_EXTEND_WIDTH", wxCheckBox)->SetValue(valset);
  // caption
  XRCCTRL(*this, "wxID_CAPTION", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_CAPTION) ));
  TtaGetEnvBoolean ("TABLE_CAPTION", &valset);
  XRCCTRL(*this, "wxID_CAPTION", wxCheckBox)->SetValue(valset);

  // update dialog Spin Ctrls
  XRCCTRL(*this, "wxID_NUMBER_ROW", wxSpinCtrl)->SetValue(def_rows);
  XRCCTRL(*this, "wxID_NUMBER_COL", wxSpinCtrl)->SetValue(def_cols);
  XRCCTRL(*this, "wxID_NUMBER_COL", wxSpinCtrl)->SetSelection(0,-1);
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

  // Gives spin ranges
  XRCCTRL(*this, "wxID_NUMBER_ROW", wxSpinCtrl )->SetRange(1, 1000);
  XRCCTRL(*this, "wxID_NUMBER_COL", wxSpinCtrl )->SetRange(1, 1000);

  XRCCTRL(*this, "wxID_FORMAT_BUTTON", wxButton)->Hide();
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
  NumberCols = XRCCTRL(*this, "wxID_NUMBER_COL", wxSpinCtrl)->GetValue();
  TtaSetEnvInt ("TABLE_COLUMNS", NumberCols, TRUE);
  NumberRows = XRCCTRL(*this, "wxID_NUMBER_ROW", wxSpinCtrl)->GetValue();
      TtaSetEnvInt ("TABLE_ROWS", NumberRows, TRUE);
  TBorder = XRCCTRL(*this, "wxID_TABLE_BORDER", wxSpinCtrl)->GetValue();
  TtaSetEnvInt ("TABLE_BORDER", TBorder, TRUE);

  // return done
  Waiting = 0;
  if (XRCCTRL(*this, "wxID_EXTEND_WIDTH", wxCheckBox)->IsChecked())
    TMAX_Width = TRUE;
  else
    TMAX_Width = FALSE;
  TtaSetEnvBoolean ("TABLE_EXTEND_WIDTH", TMAX_Width, TRUE);
  if (XRCCTRL(*this, "wxID_CAPTION", wxCheckBox)->IsChecked())
    {
      TCaption = 1;
      TtaSetEnvBoolean ("TABLE_CAPTION", TRUE, TRUE);
    }
  else
    {
      TCaption = 0;
      TtaSetEnvBoolean ("TABLE_CAPTION", FALSE, TRUE);
    }
  ThotCallback (BaseDialog + TableForm, INTEGER_DATA, (char *) 1);
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancelbutton
  ----------------------------------------------------------------------*/
void CreateTableDlgWX::OnCancelButton( wxCommandEvent& event )
{
  // return done
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char *) 0);
  TtaRedirectFocus();
}


/*----------------------------------------------------------------------
  OnSelectFormat called when clicking on selectformat
  ----------------------------------------------------------------------*/
void CreateTableDlgWX::OnSelectFormat( wxCommandEvent& event )
{
}

#endif /* _WX */
