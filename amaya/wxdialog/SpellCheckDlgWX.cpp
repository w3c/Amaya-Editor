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
  EVT_TEXT( XRCID("wxID_IGNORE_CHAR"),          SpellCheckDlgWX::OnSpecialChar )
  EVT_SPINCTRL( XRCID("wxID_NB_PROPOSAL"),      SpellCheckDlgWX::OnChangeNbProposals )
  EVT_LISTBOX( XRCID("wxID_PROPOSALS_LIST"),    SpellCheckDlgWX::OnChangeProposals)
 END_EVENT_TABLE()


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::Set_Proposals ( )
{
  char  *proposal;
  char  *lang;
  int    i;

  wxLogDebug( _T("SpellCheckDlgWX::WX_Set_Proposals") );

  //update informations 
  // selected text
  TtaGetProposal (&proposal, 0);
  XRCCTRL(*this, "wxID_SELECTED_TXT",  wxButton)->SetLabel(TtaConvMessageToWX( proposal ) );
  // default correction
  TtaGetProposal (&proposal, 1);
  if (strcmp (proposal, "$") != 0)
    XRCCTRL(*this, "wxID_FIRST_PROPOSAL", wxTextCtrl)->SetValue(TtaConvMessageToWX( proposal ) );
  // list of proposals
  for (i = 1; i <= m_max_proposals; i++)
    {
      if (i <= m_nb_proposals)
	{
	  TtaGetProposal (&proposal, i);
	  if (strcmp (proposal, "$") != 0)
	    XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString (i-1, TtaConvMessageToWX( proposal ));
	}
      else
	XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(i-1, TtaConvMessageToWX( "" ));
      XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetSelection(0);
    }
  //checker language
  TtaGetChkrLanguageName (&lang);
  char buffer[100];
  strcpy (buffer, TtaGetMessage (LIB, TMSG_LANGUAGE) );
  strcat (buffer, ": ");
  strcat (buffer, lang);
  XRCCTRL(*this, "wxID_SPELL_LANGUAGE_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( buffer ));
}

/*----------------------------------------------------------------------
  SpellCheckDlgWX create the Spell Checker dialog 
  params:
    + parent : parent window
    + ps_file : postscript file
  ----------------------------------------------------------------------*/
SpellCheckDlgWX::SpellCheckDlgWX( int ref,
				  int base,
				  wxWindow* parent ) :
  AmayaDialog( parent, ref )
{
  m_base = base;
  m_ref = ref;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SpellCheckDlgWX"));
  wxLogDebug( _T("SpellCheckDlgWX::SpellCheckDlgWX"));
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_Correct) );
  SetTitle( wx_title );

  // proposals
  XRCCTRL(*this, "wxID_NB_PROPOSALS_TXT", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Number_Propositions) ));
  XRCCTRL(*this, "wxID_SELECTED_TXT",   wxButton)->SetLabel(TtaConvMessageToWX( " ") );
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(0, TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(1, TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(2, TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(3, TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->SetString(4, TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_FIRST_PROPOSAL", wxTextCtrl)->SetValue(TtaConvMessageToWX( "" ) );
  m_nb_proposals = XRCCTRL(*this, "wxID_NB_PROPOSAL", wxSpinCtrl)->GetValue();
  m_max_proposals = 5;

  // search area radio box
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_What) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(0, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_BEFORE_SEL) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(1, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_WITHIN_SEL) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(2, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_AFTER_SEL) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetString(3, TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC) ));
  XRCCTRL(*this, "wxID_CHECKING_AREA", wxRadioBox)->SetSelection(2);


  // 'ignore' check list and text ctrl
  XRCCTRL(*this, "wxID_IGNORE_TXT",  wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Ignore) ));
  XRCCTRL(*this, "wxID_IGNORE1_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Capitals) ));
  XRCCTRL(*this, "wxID_IGNORE2_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Arabics) ));
  XRCCTRL(*this, "wxID_IGNORE3_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Romans) ));
  XRCCTRL(*this, "wxID_IGNORE4_CHK", wxCheckBox)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Specials) ));
  XRCCTRL(*this, "wxID_IGNORE1_CHK", wxCheckBox)->SetValue(TRUE);
  XRCCTRL(*this, "wxID_IGNORE2_CHK", wxCheckBox)->SetValue(TRUE);
  XRCCTRL(*this, "wxID_IGNORE3_CHK", wxCheckBox)->SetValue(TRUE);
  XRCCTRL(*this, "wxID_IGNORE4_CHK", wxCheckBox)->SetValue(TRUE);
  m_special_char = TtaConvMessageToWX( "@#$&+~" );
  XRCCTRL(*this, "wxID_IGNORE_CHAR", wxTextCtrl)->SetValue( m_special_char );

  // buttons
  XRCCTRL(*this, "wxID_CANCEL_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_SEARCH_SKIP_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Pass_Without) ));
  XRCCTRL(*this, "wxID_SKIP_DIC_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Pass_With) ));
  XRCCTRL(*this, "wxID_REPLACE_NEXT_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Replace_Without) ));
  XRCCTRL(*this, "wxID_REPLACE_DIC_BUTTON", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_Replace_With) ));

  // Set focus - The winner is ...
  XRCCTRL(*this, "wxID_SEARCH_SKIP_BUTTON", wxButton)->SetFocus();

  Layout();
  
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SpellCheckDlgWX::~SpellCheckDlgWX()
{
  /* when the dialog is destroyed, It important to cleanup context */
  ThotCallback (m_ref, INTEGER_DATA, (char*) 0);
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
  SpellCheckDlgWX::Set_Proposals ();
}

/*----------------------------------------------------------------------
  OnSkipWithButton called when clicking on skip+dic button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnSkipWithButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 2);
  SpellCheckDlgWX::Set_Proposals ();
}

/*----------------------------------------------------------------------
  OnReplaceWithoutButton called when clicking on replace/next  button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnReplaceWithoutButton( wxCommandEvent& event )
{
  wxString selected_item = 
    XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->GetStringSelection();
  if ( !selected_item.IsEmpty() )
    {  
      // allocate a temporary buffer
      char buffer[100];
      wxASSERT( selected_item.Len() < 100 );
      strcpy( buffer, (const char*)selected_item.mb_str(wxConvUTF8) );
      ThotCallback (m_base + ChkrSelectProp, STRING_DATA, buffer);
    }
  ThotCallback (m_ref, INTEGER_DATA, (char*) 3);
  SpellCheckDlgWX::Set_Proposals ();
}

/*----------------------------------------------------------------------
  OnReplaceWithButton called when clicking on replace+dic button
  ----------------------------------------------------------------------*/
void SpellCheckDlgWX::OnReplaceWithButton( wxCommandEvent& event )
{
  wxString selected_item = 
    XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->GetStringSelection();
  if ( !selected_item.IsEmpty() )
    {  
      // allocate a temporary buffer
      char buffer[100];
      wxASSERT( selected_item.Len() < 100 );
      strcpy( buffer, (const char*)selected_item.mb_str(wxConvUTF8) );
      ThotCallback (m_base + ChkrSelectProp, STRING_DATA, buffer);
    }
  ThotCallback (m_ref, INTEGER_DATA, (char*) 4);
  SpellCheckDlgWX::Set_Proposals ();
}

/*---------------------------------------------------------------------------
  OnChangeProposals
  ----------------------------------------------------------------------------*/
void SpellCheckDlgWX::OnChangeProposals ( wxCommandEvent& event )
{
  wxLogDebug( _T("SpellCheckDlgWX::OnChangeProposals") );
  int i = XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->GetSelection();
  wxString selected_item = 
    XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->GetStringSelection();
  if ( !selected_item.IsEmpty() )
    {  
      wxLogDebug( _T("SpellCheckDlgWX::OnChangeProposals - selected=") + selected_item );
      // allocate a temporary buffer
      char buffer[100];
      wxASSERT( selected_item.Len() < 100 );
      strcpy( buffer, (const char*)selected_item.mb_str(wxConvUTF8) );
      ThotCallback (m_base + ChkrSelectProp, STRING_DATA, buffer);
      // update first proposal
      XRCCTRL(*this, "wxID_FIRST_PROPOSAL", wxTextCtrl)->SetValue(selected_item);
      XRCCTRL(*this, "wxID_PROPOSALS_LIST", wxListBox)->Deselect(i);
    }
}

/*---------------------------------------------------------------------------
  OnChangeNbProposals
  ----------------------------------------------------------------------------*/
void SpellCheckDlgWX::OnChangeNbProposals ( wxSpinEvent& event )
{
  char   * lang;

  wxLogDebug( _T("SpellCheckDlgWX::OnChangeNbProposals") );
  m_nb_proposals = XRCCTRL(*this, "wxID_NB_PROPOSAL", wxSpinCtrl)->GetValue();
  ThotCallback (m_base + ChkrCaptureNC, INTEGER_DATA, (char *) m_nb_proposals);
  SpellCheckDlgWX::Set_Proposals ();
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
  if (XRCCTRL(*this, "wxID_IGNORE4_CHK", wxCheckBox)->GetValue() == TRUE)
    XRCCTRL(*this, "wxID_IGNORE_CHAR", wxTextCtrl)->SetEditable( TRUE );
  else
    XRCCTRL(*this, "wxID_IGNORE_CHAR", wxTextCtrl)->SetEditable( FALSE );
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

/*---------------------------------------------------------------
  OnSpecialChar Set the special characters to ignore
  ---------------------------------------------------------------*/
void SpellCheckDlgWX::OnSpecialChar ( wxCommandEvent& event )
{
  wxString special_char = XRCCTRL(*this, "wxID_IGNORE_CHAR", wxTextCtrl)->GetValue( );
  wxLogDebug( _T("SpellCheckDlgWX::OnSpecialChar - special_char =")+ special_char);

  // allocate a temporary buffer to copy the 'const char *' special_char buffer 
  char buffer[100];
  wxASSERT( special_char.Len() < 100 );
  strcpy( buffer, special_char.ToAscii() );
  ThotCallback (m_base + ChkrSpecial,  STRING_DATA, (char *)buffer );
}

#endif /* _WX */
