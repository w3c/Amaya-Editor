#ifdef _WX

#include "wx/wx.h"

#ifdef __WXDEBUG__

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "message_wx.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "logdebug.h"

#include "AmayaLogDebug.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaLogDebug, wxDialog)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaLogDebug
 *      Method:  AmayaLogDebug
 * Description:  create a new AmayaLogDebug
 *--------------------------------------------------------------------------------------
 */
AmayaLogDebug::AmayaLogDebug( wxWindow * p_parent ) : 
  wxDialog( NULL, -1,
	    _T(""),
	    wxDefaultPosition,
	    wxSize(500,500),
	    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCAPTION )
{   
  // setup the title
  SetTitle( _T("LogDebug") );

  // create a top sizer to contains the label and cancel button
  m_pTopSizer = new wxBoxSizer ( wxVERTICAL );

  // create the checkboxs and add it to the sizer
  int          i = 0;
  ThotBool     value = FALSE;
  wxCheckBox * p_checkbox = NULL;
  while( g_logdebug_filter_names[i].gui_name[0] != '\0' )
    {
      // initialize registry default value
      TtaSetEnvBoolean((char *)g_logdebug_filter_names[i].registry_name, FALSE, FALSE);

      // create the checkbox
      p_checkbox = new wxCheckBox( this, i, TtaConvMessageToWX(g_logdebug_filter_names[i].gui_name) );
      m_pTopSizer->Add( p_checkbox, 0, wxEXPAND | wxALL, 5 );

      // initialize the checkbox state
      TtaGetEnvBoolean((char *)g_logdebug_filter_names[i].registry_name, &value);
      p_checkbox->SetValue(value);
      DoCheck(i, value);

      i++;
    }

  // create the close button
  m_pCancelButton = new wxButton(this, wxID_CANCEL, _T("Close"));
  m_pTopSizer->Add( m_pCancelButton, 0, wxEXPAND | wxALL, 5 );

  SetSizer(m_pTopSizer);
  m_pTopSizer->Fit(this);
  m_pTopSizer->Layout();



  SetAutoLayout(TRUE);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaLogDebug
 *      Method:  ~AmayaLogDebug
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AmayaLogDebug::~AmayaLogDebug()
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaLogDebug
 *      Method:  OnCheckButton
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaLogDebug::OnCheckButton(wxCommandEvent& event)
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaLogDebug::OnCheckButton id=%d checked=%s"),
		 event.GetId(),
		 event.IsChecked() ? _T("yes") : _T("no") );
  DoCheck( event.GetId(), event.IsChecked() );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaLogDebug
 *      Method:  DoCheck
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaLogDebug::DoCheck(int id, bool checked)
{
  int filter_value = (1 << id);
  if (checked)
    {
      g_logdebug_filter = g_logdebug_filter | filter_value;
      TtaSetEnvBoolean((char *)g_logdebug_filter_names[id].registry_name, TRUE, TRUE);
    }
  else
    {
      g_logdebug_filter = (g_logdebug_filter & (~filter_value));
      TtaSetEnvBoolean((char *)g_logdebug_filter_names[id].registry_name, FALSE, TRUE);
    }
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaLogDebug, wxDialog)
  EVT_CHECKBOX( -1, AmayaLogDebug::OnCheckButton )
END_EVENT_TABLE()

#endif /* #ifdef __WXDEBUG__ */

#endif /* #ifdef _WX */
