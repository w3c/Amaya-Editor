#ifdef _WX

#ifndef AMAYATIPOFTHEDAY_H_
#define AMAYATIPOFTHEDAY_H_

#include "wx/wx.h"
#include "wx/tipdlg.h"

/**
 * Provider for Amaya tips
 */
class AmayaTipProvider : public wxTipProvider
{
public:
  AmayaTipProvider(size_t currentTip);
  virtual ~AmayaTipProvider();
  
  virtual wxString GetTip();
};


/**
 * Dialog to show tips.
 * Lightly modifier version of wx's one to translate labels.
 * \see wxTipDialog
 */
class AmayaTipDialog : public wxDialog
{
public:
  AmayaTipDialog(wxWindow *parent,
                wxTipProvider *tipProvider,
                bool showAtStartup);

    // the tip dialog has "Show tips on startup" checkbox - return true if it
    // was checked (or wasn't unchecked)
    bool ShowTipsOnStartup() const { return m_checkbox->GetValue(); }

    // sets the (next) tip text
    void SetTipText() { m_text->SetValue(m_tipProvider->GetTip()); }

    // "Next" button handler
    void OnNextTip(wxCommandEvent& WXUNUSED(event)) { SetTipText(); }

private:
    wxTipProvider *m_tipProvider;

    wxTextCtrl *m_text;
    wxCheckBox *m_checkbox;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(AmayaTipDialog)
};



#endif /*AMAYATIPOFTHEDAY_H_*/

#endif /* #ifdef _WX */
