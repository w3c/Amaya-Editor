#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

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
#include "libmsg.h"
#include "frame.h"
#include "message_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"
#include "panel.h"
#include "registry_wx.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "panel_tv.h"
#include "colors_f.h"
#include "inites_f.h"
#include "presentmenu_f.h"

#include "AmayaFormatPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaFormatPanel, AmayaSubPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  AmayaFormatPanel
 * Description:  construct a panel (bookmarks, elements, attributes, colors ...)
 *--------------------------------------------------------------------------------------
 */
AmayaFormatPanel::AmayaFormatPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_FORMAT") )
{
  wxLogDebug( _T("AmayaFormatPanel::AmayaFormatPanel") );

  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_FORMAT)));

  //TtaGetMessage (LIB, TMSG_ALIGN)
  XRCCTRL(*m_pPanelContentDetach, "wxID_LABEL_FORMATINDENT", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INDENT_PTS)));
  //TtaGetMessage (LIB, TMSG_INDENT)
  XRCCTRL(*m_pPanelContentDetach, "wxID_LABEL_FORMATLINESPACE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_LINE_SPACING_PTS)));
  //TtaGetMessage (LIB, TMSG_LINE_SPACING)

  m_OffColour = XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATLEFT", wxBitmapButton)->GetBackgroundColour();
  m_OnColour  = wxColour(250, 200, 200);

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );

  ResetPresentMenus();
  RefreshFormatPanel();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  ~AmayaFormatPanel
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AmayaFormatPanel::~AmayaFormatPanel()
{  
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  GetPanelType
 * Description:  
 *--------------------------------------------------------------------------------------
 */
int AmayaFormatPanel::GetPanelType()
{
  return WXAMAYA_PANEL_FORMAT;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::RefreshToolTips()
{  
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_FORMAT",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_INDENT",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_LINESPACE",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));
  
  //  XRCCTRL(*m_pPanelContentDetach, "wxID_APPLY_LS", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  //  XRCCTRL(*m_pPanelContentDetach, "wxID_APPLY_FI", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATLEFT", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_FORMATLEFT)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATRIGHT", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_FORMATRIGHT)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATCENTER", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_FORMATCENTER)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATJUSTIFY", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_FORMATJUSTIFY)));

  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY_INDENT",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY_LINESPACE",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_REFRESH",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_REFRESH)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::SendDataToPanel( AmayaParams& p )
{
  int modif_type       = (int)p.param1;
  int align_num        = (int)p.param2;
  int indent_value     = (int)p.param3;
  int line_space_value = (int)p.param4;

  if ( modif_type & wxFORMAT_MODIF_FORMAT)
    {
      XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATLEFT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
      XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATRIGHT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
      XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATCENTER", wxBitmapButton)->SetBackgroundColour( m_OffColour );
      XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATJUSTIFY", wxBitmapButton)->SetBackgroundColour( m_OffColour );
      switch(align_num)
	{
	case 0:
	  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATLEFT", wxBitmapButton)->SetBackgroundColour( m_OnColour );
	  break;
	case 1:
	  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATRIGHT", wxBitmapButton)->SetBackgroundColour( m_OnColour );
	  break;
	case 2:
	  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATCENTER", wxBitmapButton)->SetBackgroundColour( m_OnColour );
	  break;
	case 3:
	  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATJUSTIFY", wxBitmapButton)->SetBackgroundColour( m_OnColour );
	  break;
	}
    }

  if ( modif_type & wxFORMAT_MODIF_INDENT)
    XRCCTRL(*m_pPanelContentDetach,"wxID_SPIN_FORMATINDENT",wxSpinCtrl)->SetValue(indent_value);
  
  if ( modif_type & wxFORMAT_MODIF_LINESPACE)
    XRCCTRL(*m_pPanelContentDetach,"wxID_SPIN_FORMATLINESPACE",wxSpinCtrl)->SetValue(line_space_value);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::DoUpdate()
{
  wxLogDebug( _T("AmayaFormatPanel::DoUpdate") );
  AmayaSubPanel::DoUpdate();
  //  ThotUpdatePalette();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  IsActive
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaFormatPanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnApply
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnApply( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaFormatPanel::OnApply") );
  ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnRefresh
 * Description:  refresh the panel from current selection
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnRefresh( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaFormatPanel::OnRefresh") );
  RefreshFormatPanel();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  RefreshFormatPanel
 * Description:  refresh the panel from current selection
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::RefreshFormatPanel()
{
  Document doc;
  View view;

  TtaGetActiveView( &doc, &view );
  /* force the refresh */
  TtaExecuteMenuAction ("TtcChangeFormat", doc, view, TRUE);

  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_FORMAT",wxBitmapButton)->SetBackgroundColour(m_OffColour);
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_INDENT",wxBitmapButton)->SetBackgroundColour(m_OffColour);
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_LINESPACE",wxBitmapButton)->SetBackgroundColour(m_OffColour);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnFormatLeftChanged
 * Description:  when a format button is pressed (left,right,center,justify)
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnFormatLeftChanged( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaFormatPanel::OnFormatLeftChanged") );

  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATLEFT", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATRIGHT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATCENTER", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATJUSTIFY", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_FORMAT",wxBitmapButton)->SetBackgroundColour(m_OffColour);
  ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 0);
  ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1); /* Apply */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnFormatRightChanged
 * Description:  when a format button is pressed (left,right,center,justify)
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnFormatRightChanged( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaFormatPanel::OnFormatRightChanged") );

  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATLEFT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATRIGHT", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATCENTER", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATJUSTIFY", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_FORMAT",wxBitmapButton)->SetBackgroundColour(m_OffColour);
  ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 1);
  ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1); /* Apply */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnFormatCenterChanged
 * Description:  when a format button is pressed (left,right,center,justify)
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnFormatCenterChanged( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaFormatPanel::OnFormatCenterChanged") );

  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATLEFT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATRIGHT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATCENTER", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATJUSTIFY", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_FORMAT",wxBitmapButton)->SetBackgroundColour(m_OffColour);
  ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 2);
  ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1); /* Apply */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnFormatJustifyChanged
 * Description:  when a format button is pressed (left,right,center,justify)
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnFormatJustifyChanged( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaFormatPanel::OnFormatJustifyChanged") );

  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATLEFT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATRIGHT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATCENTER", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATJUSTIFY", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_FORMAT",wxBitmapButton)->SetBackgroundColour(m_OffColour);
  ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 3);
  ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1); /* Apply */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  IndentChanged
 * Description:  when the indent spin ctrl value is changed
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::IndentChanged()
{
  wxLogDebug( _T("AmayaFormatPanel::IndentChanged") );

  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_INDENT",wxBitmapButton)->SetBackgroundColour(m_OffColour);
  int indent_value = XRCCTRL(*m_pPanelContentDetach,"wxID_SPIN_FORMATINDENT",wxSpinCtrl)->GetValue();
  ThotCallback (NumZoneRecess, INTEGER_DATA, (char*)indent_value);
  ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1); /* Apply */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnIndentChanged
 * Description:  when the indent spin ctrl value is changed
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnIndentChanged( wxSpinEvent& event )
{
  IndentChanged();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnIndentChangedButton
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnIndentChangedButton( wxCommandEvent& event )
{
  IndentChanged();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  LineSpaceChanged
 * Description:  when the line space spin ctrl value is changed
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::LineSpaceChanged()
{
  wxLogDebug( _T("AmayaFormatPanel::LineSpaceChanged") );

  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_LINESPACE",wxBitmapButton)->SetBackgroundColour(m_OffColour);
  int linespace_value = XRCCTRL(*m_pPanelContentDetach,"wxID_SPIN_FORMATLINESPACE",wxSpinCtrl)->GetValue();
  ThotCallback (NumZoneLineSpacing, INTEGER_DATA, (char*)linespace_value);
  ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1); /* Apply */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnLineSpaceChanged
 * Description:  when the line space spin ctrl value is changed
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnLineSpaceChanged( wxSpinEvent& event )
{
  LineSpaceChanged();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnLineSpaceChangedButton
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnLineSpaceChangedButton( wxCommandEvent& event )
{
  LineSpaceChanged();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnDefaultFormat
 * Description:  when the default button is pressed
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnDefaultFormat( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaFormatPanel::OnDefaultFormat") );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATLEFT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATRIGHT", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATCENTER", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATJUSTIFY", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_FORMAT",wxBitmapButton)->SetBackgroundColour(m_OnColour);
  ThotCallback (NumMenuAlignment, INTEGER_DATA, (char*) 4);
  ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1); /* Apply */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnDefaultIndent
 * Description:  when the default button is pressed
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnDefaultIndent( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaFormatPanel::OnDefaultIdent") );
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_INDENT",wxBitmapButton)->SetBackgroundColour(m_OnColour);
  //  XRCCTRL(*m_pPanelContentDetach,"wxID_SPIN_FORMATINDENT",wxSpinCtrl)->SetValue(_T(""));
  ThotCallback (NumMenuRecessSense, INTEGER_DATA, (char*) 2);
  ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1); /* Apply */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFormatPanel
 *      Method:  OnDefaultLineSpace
 * Description:  when the default button is pressed
 *--------------------------------------------------------------------------------------
 */
void AmayaFormatPanel::OnDefaultLineSpace( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaFormatPanel::OnDefaultLineSpace") );
  XRCCTRL(*m_pPanelContentDetach,"wxID_DEFAULT_LINESPACE",wxBitmapButton)->SetBackgroundColour(m_OnColour);
  //  XRCCTRL(*m_pPanelContentDetach,"wxID_SPIN_FORMATLINESPACE",wxSpinCtrl)->SetValue(_T(""));
  ThotCallback (NumMenuLineSpacing, INTEGER_DATA, (char*) 3);
  ThotCallback (NumFormPresFormat, INTEGER_DATA, (char*) 1); /* Apply */
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaFormatPanel, AmayaSubPanel)
  EVT_SPINCTRL( XRCID("wxID_SPIN_FORMATINDENT"),     AmayaFormatPanel::OnIndentChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_FORMATLINESPACE"),  AmayaFormatPanel::OnLineSpaceChanged ) 
  EVT_BUTTON( XRCID("wxID_APPLY_INDENT"),      AmayaFormatPanel::OnIndentChangedButton ) 
  EVT_BUTTON( XRCID("wxID_APPLY_LINESPACE"),   AmayaFormatPanel::OnLineSpaceChangedButton ) 

  EVT_BUTTON( XRCID("wxID_BMPBUTTON_FORMATLEFT"),    AmayaFormatPanel::OnFormatLeftChanged ) 
  EVT_BUTTON( XRCID("wxID_BMPBUTTON_FORMATRIGHT"),   AmayaFormatPanel::OnFormatRightChanged ) 
  EVT_BUTTON( XRCID("wxID_BMPBUTTON_FORMATCENTER"),  AmayaFormatPanel::OnFormatCenterChanged ) 
  EVT_BUTTON( XRCID("wxID_BMPBUTTON_FORMATJUSTIFY"), AmayaFormatPanel::OnFormatJustifyChanged ) 

  EVT_BUTTON( XRCID("wxID_DEFAULT_FORMAT"),    AmayaFormatPanel::OnDefaultFormat ) 
  EVT_BUTTON( XRCID("wxID_DEFAULT_INDENT"),    AmayaFormatPanel::OnDefaultIndent ) 
  EVT_BUTTON( XRCID("wxID_DEFAULT_LINESPACE"), AmayaFormatPanel::OnDefaultLineSpace ) 

  EVT_BUTTON( XRCID("wxID_APPLY"), AmayaFormatPanel::OnApply )
  EVT_BUTTON( XRCID("wxID_REFRESH"), AmayaFormatPanel::OnRefresh )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
