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
#include "editcommands_f.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "panel_tv.h"

#include "AmayaMathMLPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

#include "mathml_filtres.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaMathMLPanel, AmayaSubPanel)

typedef struct _XmlEntity
{
  char         *charName;      
  int           charCode;      
} XmlEntity;

MathMLEntityHash AmayaMathMLPanel::m_MathMLEntityHash;
int * AmayaMathMLPanel::m_pActiveFiltre = NULL;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  AmayaMathMLPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaMathMLPanel::AmayaMathMLPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_MATHML") )
{
  wxLogDebug( _T("AmayaMathMLPanel::AmayaMathMLPanel") );

  //  m_pPanelContentDetach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);

  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_MATHML)));

  m_pList = XRCCTRL(*this,"wxID_PANEL_MATH_LIST",wxComboBox);

  m_OffColour = XRCCTRL(*this, "wxID_PANEL_MATH_F1", wxBitmapButton)->GetBackgroundColour();
  m_OnColour  = wxColour(250, 200, 200);

  RefreshButtonState();

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  ~AmayaMathMLPanel
 * Description:  destructor
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaMathMLPanel::~AmayaMathMLPanel()
{
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  GetPanelType
 * Description:  
 *--------------------------------------------------------------------------------------
 */
int AmayaMathMLPanel::GetPanelType()
{
  return WXAMAYA_PANEL_MATHML;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::RefreshToolTips()
{  
  XRCCTRL(*this,"wxID_PANEL_MATH_INSERT",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INSERT)));
  
  XRCCTRL(*this,"wxID_PANEL_MATH_F1",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_GREEK_ALPHABET)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F2",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_GREEK_CAP)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F3",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_MATHML)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F4",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_OPERATOR)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F5",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BINARY_REL)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F6",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BINARY_REL_NEG)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F7",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_MISC)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F8",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ARROW)));

  // TODO
  //XRCCTRL(*this,"wxID_PANEL_MATH_F8",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CUSTOM)));
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButton
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButton( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButton") );

  //  TtaExecuteMenuAction ("CreateMENCLOSE", doc, view, FALSE);

  Document doc;
  View view;
  TtaGetActiveView( &doc, &view );

  int id       = event.GetId();
  int amaya_id = -1;
  CloseTextInsertion ();
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_BMATH")) )
    TtaExecuteMenuAction ("CreateMath", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FENCE")) )
    TtaExecuteMenuAction ("CreateMROW", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SQRT")) )
    TtaExecuteMenuAction ("CreateMSQRT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ROOT")) )
    TtaExecuteMenuAction ("CreateMROOT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FRAC")) )
    TtaExecuteMenuAction ("CreateMFRAC", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MATRIX")) )
    TtaExecuteMenuAction ("CreateMTABLE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNDER")) )
    TtaExecuteMenuAction ("CreateMUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVER")) )
    TtaExecuteMenuAction ("CreateMOVER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERUNDER")) )
    TtaExecuteMenuAction ("CreateMUNDEROVER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MSCRIPT")) )
    TtaExecuteMenuAction ("CreateMMULTISCRIPTS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUB")) )
    TtaExecuteMenuAction ("CreateMSUB", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUP")) )
    TtaExecuteMenuAction ("CreateMSUP", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUBSUP")) )
    TtaExecuteMenuAction ("CreateMSUBSUP", doc, view, FALSE);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre1
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre1( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre1") );
  m_pActiveFiltre = filtre_greek;
  DoFilter( filtre_greek );

  AmayaParams p;
  p.param1 = (void*)AmayaMathMLPanel::wxMATHML_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_MATHML, p );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre2
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre2( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre2") );
  m_pActiveFiltre = filtre_greek_maj;
  DoFilter( filtre_greek_maj );

  AmayaParams p;
  p.param1 = (void*)AmayaMathMLPanel::wxMATHML_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_MATHML, p );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre3
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre3( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre3") );
  m_pActiveFiltre = filtre_maths;
  DoFilter( filtre_maths );

  AmayaParams p;
  p.param1 = (void*)AmayaMathMLPanel::wxMATHML_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_MATHML, p );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre4
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre4( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre4") );
  m_pActiveFiltre = filtre_operateurs;
  DoFilter( filtre_operateurs );

  AmayaParams p;
  p.param1 = (void*)AmayaMathMLPanel::wxMATHML_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_MATHML, p );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre5
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre5( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre5") );
  m_pActiveFiltre = filtre_relations_binaires;
  DoFilter( filtre_relations_binaires );

  AmayaParams p;
  p.param1 = (void*)AmayaMathMLPanel::wxMATHML_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_MATHML, p );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre6
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre6( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre6") );
  m_pActiveFiltre = filtre_relations_binaires_negation;
  DoFilter( filtre_relations_binaires_negation );

  AmayaParams p;
  p.param1 = (void*)AmayaMathMLPanel::wxMATHML_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_MATHML, p );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre7
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre7( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre7") );
  m_pActiveFiltre = filtre_divers;
  DoFilter( filtre_divers );
  
  AmayaParams p;
  p.param1 = (void*)AmayaMathMLPanel::wxMATHML_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_MATHML, p );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre8
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre8( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre8") );
  m_pActiveFiltre = filtre_fleches;
  DoFilter( filtre_fleches );
  
  AmayaParams p;
  p.param1 = (void*)AmayaMathMLPanel::wxMATHML_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_MATHML, p );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  DoFilter
 * Description:  this method is called to refresh the entity list with a given filter
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::DoFilter( int * filtre )
{
  if (!filtre)
    return;

  m_pList->Clear();
  int element_id = 0;
  while ( filtre[element_id] != -1 )
    {
      wxString & entity_name = m_MathMLEntityHash[filtre[element_id]];
      if (entity_name.IsEmpty())
      m_pList->Append( wxString((wxChar)filtre[element_id]) );
      else
	m_pList->Append( entity_name
			 + _T(" : ")
			 + wxString((wxChar)filtre[element_id]));
      element_id++;
    }
  m_pList->SetSelection(0);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  RefreshButtonState
 * Description:  this method is called to refresh the button colors
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::RefreshButtonState()
{
  if (m_pActiveFiltre == filtre_greek)
    XRCCTRL(*this, "wxID_PANEL_MATH_F1", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*this, "wxID_PANEL_MATH_F1", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  if (m_pActiveFiltre == filtre_greek_maj)
    XRCCTRL(*this, "wxID_PANEL_MATH_F2", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*this, "wxID_PANEL_MATH_F2", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  if (m_pActiveFiltre == filtre_maths)
    XRCCTRL(*this, "wxID_PANEL_MATH_F3", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*this, "wxID_PANEL_MATH_F3", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  if (m_pActiveFiltre == filtre_operateurs)
    XRCCTRL(*this, "wxID_PANEL_MATH_F4", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*this, "wxID_PANEL_MATH_F4", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  if (m_pActiveFiltre == filtre_relations_binaires)
    XRCCTRL(*this, "wxID_PANEL_MATH_F5", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*this, "wxID_PANEL_MATH_F5", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  if (m_pActiveFiltre == filtre_relations_binaires_negation)
    XRCCTRL(*this, "wxID_PANEL_MATH_F6", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*this, "wxID_PANEL_MATH_F6", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  if (m_pActiveFiltre == filtre_divers)
    XRCCTRL(*this, "wxID_PANEL_MATH_F7", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*this, "wxID_PANEL_MATH_F7", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  if (m_pActiveFiltre == filtre_fleches )
    XRCCTRL(*this, "wxID_PANEL_MATH_F8", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*this, "wxID_PANEL_MATH_F8", wxBitmapButton)->SetBackgroundColour( m_OffColour );
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonInsert
 * Description:  this method is called wants to insert a char
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonInsert( wxCommandEvent& event )
{
  if (m_pList->GetSelection() != wxNOT_FOUND)
    {
      int charactere = m_pActiveFiltre[m_pList->GetSelection()];
      InsertChar (TtaGiveActiveFrame(), charactere, -1);
      CloseTextInsertion ();
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::SendDataToPanel( AmayaParams& p )
{
  int action = (int)p.param1;
  if (action == wxMATHML_ACTION_INIT)
    {
      XmlEntity *MathEntityTable = (XmlEntity *)p.param2;
      
      // initialize entity hashtable
      int entity_id = 0;
      while (MathEntityTable[entity_id].charCode != -1)
	{
	  m_MathMLEntityHash[MathEntityTable[entity_id].charCode] = TtaConvMessageToWX(MathEntityTable[entity_id].charName);
	  entity_id++;
	}

      // now select the default filter
      if (!m_pActiveFiltre)
	{
	  m_pActiveFiltre = filtre_greek;
	  DoFilter( m_pActiveFiltre );
	  RefreshButtonState();
	}
    }
  else if (action == wxMATHML_ACTION_REFRESH)
    {
      DoFilter( m_pActiveFiltre );
      RefreshButtonState();
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::DoUpdate()
{
  wxLogDebug( _T("AmayaMathMLPanel::DoUpdate") );
  AmayaSubPanel::DoUpdate();
  DoFilter( m_pActiveFiltre );
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  IsActive
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaMathMLPanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaMathMLPanel, AmayaSubPanel)
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_BMATH"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FENCE"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SQRT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ROOT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FRAC"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MATRIX"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNDER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERUNDER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MSCRIPT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUB"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUP"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUBSUP"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F1"), AmayaMathMLPanel::OnButtonFiltre1 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F2"), AmayaMathMLPanel::OnButtonFiltre2 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F3"), AmayaMathMLPanel::OnButtonFiltre3 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F4"), AmayaMathMLPanel::OnButtonFiltre4 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F5"), AmayaMathMLPanel::OnButtonFiltre5 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F6"), AmayaMathMLPanel::OnButtonFiltre6 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F7"), AmayaMathMLPanel::OnButtonFiltre7 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F8"), AmayaMathMLPanel::OnButtonFiltre8 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INSERT"), AmayaMathMLPanel::OnButtonInsert ) 
END_EVENT_TABLE()

#endif /* #ifdef _WX */
