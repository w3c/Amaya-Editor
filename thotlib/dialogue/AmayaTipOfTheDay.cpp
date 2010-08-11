/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/tipdlg.h"
#include "wx/artprov.h"


#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "message_wx.h"
#include "libmsg.h"
#include "frame.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern

#include "AmayaTipOfTheDay.h"
#include "tips.h"

static const int s_tips[] = 
{
    LIB, TIP_LANGUAGES,
    LIB, TIP_AMAYA_PROFILES,
    LIB, TIP_PANEL_MODES,
    LIB, TIP_SVG_SHAPE_EDIT,
    LIB, TIP_PARENT_SELECTION,
    LIB, TIP_SPLIT_NOTEBOOK,
    LIB, TIP_SVG_ROTATE,
    LIB, TIP_MOVE_PANELS,
    LIB, TIP_MANY_COLUMNS,
    LIB, TIP_LINK_TARGET,
    LIB, TIP_EDITING_MODE,
    LIB, TIP_ERROR_BUTTON,
    LIB, TIP_SVG_LINES,
    LIB, TIP_PATH_CONTROL,
    LIB, TIP_DOCUMENT_LIST_MENU,
    LIB, TIP_SVG_ASPECT_RATIO
};

//  TtaGetEnvInt

static const int s_tipsCount = sizeof(s_tips)/sizeof(int)/2;
static int delta = 0;


/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
AmayaTipProvider::AmayaTipProvider(size_t currentTip):
  wxTipProvider(currentTip)
{
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
AmayaTipProvider::~AmayaTipProvider()
{
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
wxString AmayaTipProvider::GetTip()
{
  int num = (m_currentTip+delta+s_tipsCount)%s_tipsCount;
  m_currentTip = num;
  return TtaConvMessageToWX(TtaGetMessage(s_tips[num*2],
                                          s_tips[num*2+1]));
}

// ----------------------------------------------------------------------------
// wxTipDialog
// ----------------------------------------------------------------------------

static const int wxID_NEXT_TIP = 32000;  // whatever
static const int wxID_PREVIOUS_TIP = 32001;  // whatever

BEGIN_EVENT_TABLE(AmayaTipDialog, wxDialog)
    EVT_BUTTON(XRCID("wxID_TIP_NEXT"), AmayaTipDialog::OnNextTip)
    EVT_BUTTON(XRCID("wxID_TIP_PREVIOUS"), AmayaTipDialog::OnPreviousTip)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
AmayaTipDialog::AmayaTipDialog(wxWindow *parent,
                         wxTipProvider *tipProvider,
                         bool showAtStartup)
           : wxDialog(parent, wxID_ANY, TtaConvMessageToWX(TtaGetMessage(LIB, TIP_DIALOG_TITLE)),
                      wxDefaultPosition, wxDefaultSize,
                      wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER
                      )
{
    m_tipProvider = tipProvider;

    // 1) create all controls in tab order
    wxButton *btnClose = new wxButton(this, XRCID("wxID_TIP_CLOSE"), TtaConvMessageToWX(TtaGetMessage(LIB, TIP_DIALOG_TIP_CLOSE)));
    SetAffirmativeId(XRCID("wxID_TIP_CLOSE"));

    m_checkbox = new wxCheckBox(this, wxID_ANY, TtaConvMessageToWX(TtaGetMessage(LIB, TIP_DIALOG_STARTUP)));
    m_checkbox->SetValue(showAtStartup);

    wxButton *btnNext = new wxButton(this, XRCID("wxID_TIP_NEXT"), TtaConvMessageToWX(TtaGetMessage(LIB, TIP_DIALOG_NEXT_TIP)));

    wxButton *btnPrevious = new wxButton(this, XRCID("wxID_TIP_PREVIOUS"), TtaConvMessageToWX(TtaGetMessage(LIB, TIP_DIALOG_PREVIOUS_TIP)));

    wxStaticText *text = new wxStaticText(this, wxID_ANY, TtaConvMessageToWX(TtaGetMessage(LIB, TIP_DIALOG_DID_YOU_KNOW)));

    wxFont font = text->GetFont();
    font.SetPointSize(int(1.6 * font.GetPointSize()));
    font.SetWeight(wxFONTWEIGHT_BOLD);
    text->SetFont(font);

    m_text = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                            wxDefaultPosition, wxSize(200, 160),
                            wxTE_MULTILINE |
                            wxTE_READONLY |
                            wxTE_NO_VSCROLL |
                            wxTE_RICH2 | // a hack to get rid of vert scrollbar
                            wxDEFAULT_CONTROL_BORDER
                            );
#if defined(__WXMSW__)
    m_text->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL));
#endif

    wxIcon icon = wxArtProvider::GetIcon(wxART_TIP, wxART_CMN_DIALOG);
    wxStaticBitmap *bmp = new wxStaticBitmap(this, wxID_ANY, icon);

    // 2) put them in boxes

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *icon_text = new wxBoxSizer( wxHORIZONTAL );
    icon_text->Add( bmp, 0, wxCENTER );
    icon_text->Add( text, 1, wxCENTER | wxLEFT, 20 );
    topsizer->Add( icon_text, 0, wxEXPAND | wxALL, 10 );

    topsizer->Add( m_text, 1, wxEXPAND | wxLEFT|wxRIGHT, 10 );

    wxBoxSizer *bottom = new wxBoxSizer( wxHORIZONTAL );
    bottom->Add( m_checkbox, 0, wxCENTER );

    bottom->Add( 10,10,1 );
    bottom->Add( btnPrevious, 0, wxCENTER | wxLEFT, 10 );
    bottom->Add( btnNext, 0, wxCENTER | wxLEFT, 10 );
    bottom->Add( btnClose, 0, wxCENTER | wxLEFT, 10 );

    topsizer->Add( bottom, 0, wxEXPAND | wxALL, 10 );

    SetTipText();

    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    Centre(wxBOTH | wxCENTER_FRAME);
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
static bool AmayaShowTip(wxWindow *parent,
               wxTipProvider *tipProvider,
               bool showAtStartup)
{
  AmayaTipDialog dlg(parent, tipProvider, showAtStartup);
  dlg.ShowModal();

  return dlg.ShowTipsOnStartup();
}



#endif /* #ifdef _WX */


/*----------------------------------------------------------------------
 * Show TipOfTheDay dialog
 -----------------------------------------------------------------------*/
void TtaShowTipOfTheDay()
{
#ifdef _WX
  int num = 0;
  ThotBool res, show = TtaShowTipOfTheDayAtStartup();
  
  TtaGetEnvInt("TIP_OF_THE_DAY_NUMBER", &num);
  
  AmayaTipProvider prov(num);
  res = AmayaShowTip(NULL,& prov, show);
  
  if(res!=show)
    TtaSetShowTipOfTheDayAtStartup(res);

  //TtaSetEnvInt("TIP_OF_THE_DAY_NUMBER", (int)prov.GetCurrentTip(), TRUE);
  // Update current tip
  num = (int)prov.GetCurrentTip();
  int tip = (num+1+s_tipsCount)%s_tipsCount;
  TtaSetEnvInt("TIP_OF_THE_DAY_NUMBER", tip, TRUE);

#endif /* _WX */
}

/*----------------------------------------------------------------------
 * Test if TipOfTheDay must be shown at startup
 -----------------------------------------------------------------------*/
ThotBool TtaShowTipOfTheDayAtStartup()
{
  ThotBool show = TRUE;
  TtaGetEnvBoolean("TIP_OF_THE_DAY_STARTUP", &show);
  return show;
}

/*----------------------------------------------------------------------
 * Set if TipOfTheDay must be shown at startup
 -----------------------------------------------------------------------*/
void TtaSetShowTipOfTheDayAtStartup(ThotBool show)
{
  TtaSetEnvBoolean("TIP_OF_THE_DAY_STARTUP", show, TRUE);
}

// the tip dialog has "Show tips on startup" checkbox - return true if it
// was checked (or wasn't unchecked)
bool AmayaTipDialog::ShowTipsOnStartup() const
{ return m_checkbox->GetValue(); }

// sets the (next) tip text
void AmayaTipDialog::SetTipText()
{ m_text->SetValue(m_tipProvider->GetTip()); }

// "Next" button handler
void AmayaTipDialog::OnNextTip(wxCommandEvent& WXUNUSED(event)) {
  delta = +1;
  SetTipText(); }

// "Previous" button handler
void AmayaTipDialog::OnPreviousTip(wxCommandEvent& WXUNUSED(event)) {
  delta = -1;
  SetTipText(); }
