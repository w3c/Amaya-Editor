#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "SpellCheckDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SpellCheckDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_CANCEL_BUTTON"),      SpellCheckDlgWX::OnCancelButton )
  EVT_BUTTON( XRCID("wxID_SEARCH_SKIP_BUTTON"), SpellCheckDlgWX::OnSkipWithoutButton )
  EVT_BUTTON( XRCID("wxID_SKIP_DIC_BUTTON"),    SpellCheckDlgWX::OnSkipWithButton )
  EVT_BUTTON( XRCID("wxID_REPLACE_NEXT_BUTTON"),SpellCheckDlgWX::OnReplaceWithoutButton )
  EVT_BUTTON( XRCID("wxID_REPLACE_DIC_BUTTON"), SpellCheckDlgWX::OnReplaceWithButton )
  EVT_CHECKBOX( XRCID("wxID_IGNORE1_CHK"),      SpellCheckDlgWX::OnIgnoreCapitalsChkBox )
  EVT_CHECKBOX( XRCID("wxID_IGNORE2_CHK"),      SpellCheckDlgWX::OnIgnoreArabicsChkBox )
  EVT_CHECKBOX( XRCID("wxID_IGNORE3_CHK"),      SpellCheckDlgWX::OnIgnoreRomansChkBox )
  EVT_CHECKBOX( XRCID("wxID_IGNORE4_CHK"),      SpellCheckDlgWX::OnIgnoreSpecialsChkBox )
  EVT_RADIOBOX( XRCID("wxID_CHECKING_AREA"),    SpellCheckDlgWX::OnAreaRadioBox )
 /*
 EVT_TEXT( XRCID("wxID_FILE_TXT_CTRL"),        SpellCheckDlgWX::OnTypePrinter )
  */
 END_EVENT_TABLE()

/*----------------------------------------------------------------------
  SpellCheckDlgWX create the Spell Checker dialog 
  params:
    + parent : parent window
    + ps_file : postscript file
  ----------------------------------------------------------------------*/
SpellCheckDlgWX::SpellCheckDlgWX( int ref,
				  int base,
				  wxWindow* parent ) :
  AmayaDialog( NULL, ref )
{
  m_base = base;
  m_ref = ref;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SpellCheckDlgWX"));
  wxLogDebug( _T("SpellCheckDlgWX::SpellCheckDlgWX"));
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_Correct) );
  SetTitle( wx_title );

  // proposals
  XRCCTRL(*this, "wxID_NB_PROPOSALS_TXT",  wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Number_Propositions) ));

  // search area radio box
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_What) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_BEFORE_SEL) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_WITHIN_SEL) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_AFTER_SEL) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(3, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC) ));

  // 'ignore' check list
  XRCCTRL(*this, "wxID_IGNORE_TXT",  wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Ignore) ));
  XRCCTRL(*this, "wxID_IGNORE1_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Capitals) ));
  XRCCTRL(*this, "wxID_IGNORE2_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Arabics) ));
  XRCCTRL(*this, "wxID_IGNORE3_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Romans) ));
  XRCCTRL(*this, "wxID_IGNORE4_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Specials) ));
  XRCCTRL(*this, "wxID_IGNORE1_CHK", wxCheckBox)->SetValue(FALSE);
  XRCCTRL(*this, "wxID_IGNORE2_CHK", wxCheckBox)->SetValue(FALSE);
  XRCCTRL(*this, "wxID_IGNORE3_CHK", wxCheckBox)->SetValue(FALSE);
  XRCCTRL(*this, "wxID_IGNORE4_CHK", wxCheckBox)->SetValue(FALSE);

  // buttons
  XRCCTRL(*this, "wxID_CANCEL_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_SEARCH_SKIP_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Pass_Without) ));
  XRCCTRL(*this, "wxID_SKIP_DIC_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Pass_With) ));
  XRCCTRL(*this, "wxID_REPLACE_NEXT_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Replace_Without) ));
  XRCCTRL(*this, "wxID_REPLACE_DIC_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Replace_With) ));

  // Set focus to ...
  XRCCTRL(*this, "wxID_SEARCH_SKIP_BUTTON", wxButton)->SetFocus();

  Layout();
  
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SpellCheckDlgWX::~SpellCheckDlgWX()
{
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancel button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnSkipWithoutButton called when clicking on skip button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnSkipWithoutButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnSkipWithButton called when clicking on skip+dic button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnSkipWithButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 2);
}

/*----------------------------------------------------------------------
  OnReplaceWithoutButton called when clicking on replace/next  button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnReplaceWithoutButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 3);
}

/*----------------------------------------------------------------------
  OnReplaceWithButton called when clicking on replace+dic button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnReplaceWithButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 4);
}

/*---------------------------------------------------------------
  OnIgnoreCapitalsChkBox
  ---------------------------------------------------------------*/
void SpellCheckDlgWX::OnIgnoreCapitalsChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SpellCheckDlgWX::OnIgnoreCapitalsChkBox") );
  ThotCallback (m_base + ChkrMenuIgnore, INTEGER_DATA, (char*) 0);
}

/*---------------------------------------------------------------
  OnIgnoreArabicsChkBox
  ---------------------------------------------------------------*/
void SpellCheckDlgWX::OnIgnoreArabicsChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SpellCheckDlgWX::OnIgnoreArabicsChkBox") );
  ThotCallback (m_base + ChkrMenuIgnore, INTEGER_DATA, (char*) 1);
}

/*---------------------------------------------------------------
  OnIgnoreRomansChkBox
  ---------------------------------------------------------------*/
void SpellCheckDlgWX::OnIgnoreRomansChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SpellCheckDlgWX::OnIgnoreRomansChkBox") );
  ThotCallback (m_base + ChkrMenuIgnore, INTEGER_DATA, (char*) 2);
}

/*---------------------------------------------------------------
  OnIgnoreSpecialsChkBox
  ---------------------------------------------------------------*/
void SpellCheckDlgWX::OnIgnoreSpecialsChkBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SpellCheckDlgWX::OnIgnoreSpecialsChkBox") );
  ThotCallback (m_base + ChkrMenuIgnore, INTEGER_DATA, (char*) 3);
}

/*---------------------------------------------------------------------------
  OnAreaRadioBox
  ----------------------------------------------------------------------------*/
void SpellCheckDlgWX::OnAreaRadioBox ( wxCommandEvent& event )
{
  wxLogDebug( _T("SpellCheckDlgWX::OnAreaRadioBox") );
  ThotCallback (m_base + ChkrMenuOR, INTEGER_DATA,
		(char*)(XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->GetSelection( )) );
}

#endif /* _WX */
