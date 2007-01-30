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
#include "paneltypes_wx.h"

#include "AmayaSpeCharPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"
#include "displayview_f.h"
#include "mathml_filtres.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaSpeCharPanel, AmayaSubPanel)

  typedef struct _XmlEntity
  {
    char         *charName;      
    int           charCode;      
  } XmlEntity;

MathMLEntityHash AmayaSpeCharPanel::m_MathMLEntityHash;
int * AmayaSpeCharPanel::m_pActiveFiltre = NULL;

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharPanel
 *      Method:  AmayaSpeCharPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 -----------------------------------------------------------------------*/
AmayaSpeCharPanel::AmayaSpeCharPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_SPECHAR") )
{
  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SPECHAR)));

  m_pList = XRCCTRL(*this,"wxID_PANEL_MATH_LIST",wxComboBox);

  m_OffColour = XRCCTRL(*this, "wxID_PANEL_MATH_F1", wxBitmapButton)->GetBackgroundColour();
  m_OnColour  = wxColour(250, 200, 200);

  RefreshButtonState();

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharPanel
 *      Method:  ~AmayaSpeCharPanel
 * Description:  destructor
 *               TODO
 -----------------------------------------------------------------------*/
AmayaSpeCharPanel::~AmayaSpeCharPanel()
{
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharPanel
 *      Method:  GetPanelType
 * Description:  
 -----------------------------------------------------------------------*/
int AmayaSpeCharPanel::GetPanelType()
{
  return WXAMAYA_PANEL_SPECHAR;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::RefreshToolTips()
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

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre1
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::OnButtonFiltre1( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_greek;
  DoFilter( filtre_greek );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharPanel::wxSPECHAR_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre2
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::OnButtonFiltre2( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_greek_maj;
  DoFilter( filtre_greek_maj );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharPanel::wxSPECHAR_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre3
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::OnButtonFiltre3( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_maths;
  DoFilter( filtre_maths );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharPanel::wxSPECHAR_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre4
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::OnButtonFiltre4( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_operateurs;
  DoFilter( filtre_operateurs );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharPanel::wxSPECHAR_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre5
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::OnButtonFiltre5( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_relations_binaires;
  DoFilter( filtre_relations_binaires );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharPanel::wxSPECHAR_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre6
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::OnButtonFiltre6( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_relations_binaires_negation;
  DoFilter( filtre_relations_binaires_negation );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharPanel::wxSPECHAR_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre7
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::OnButtonFiltre7( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_divers;
  DoFilter( filtre_divers );
  
  AmayaParams p;
  p.param1 = (int)AmayaSpeCharPanel::wxSPECHAR_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre8
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::OnButtonFiltre8( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_fleches;
  DoFilter( filtre_fleches );
  
  AmayaParams p;
  p.param1 = (int)AmayaSpeCharPanel::wxSPECHAR_ACTION_REFRESH;
  AmayaSubPanelManager::GetInstance()->SendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  DoFilter
 * Description:  this method is called to refresh the entity list with a given filter
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::DoFilter( int * filtre )
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

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  RefreshButtonState
 * Description:  this method is called to refresh the button colors
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::RefreshButtonState()
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


/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonInsert
 * Description:  this method is called wants to insert a char
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::OnButtonInsert( wxCommandEvent& event )
{
  Document doc;
  int      view;

  if (m_pList->GetSelection() != wxNOT_FOUND)
    {
      int charactere = m_pActiveFiltre[m_pList->GetSelection()];
      //InsertChar (TtaGiveActiveFrame(), charactere, -1);
      FrameToView (TtaGiveActiveFrame(), &doc, &view);
      TtcInsertChar (doc, view, charactere);
      CloseTextInsertion ();
      TtaRedirectFocus();
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::SendDataToPanel( AmayaParams& p )
{
  int action = (int)p.param1;
  if (action == wxSPECHAR_ACTION_INIT)
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
  else if (action == wxSPECHAR_ACTION_REFRESH)
    {
      DoFilter( m_pActiveFiltre );
      RefreshButtonState();
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 -----------------------------------------------------------------------*/
void AmayaSpeCharPanel::DoUpdate()
{
  AmayaSubPanel::DoUpdate();
  DoFilter( m_pActiveFiltre );
}


/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharPanel
 *      Method:  IsActive
 * Description:  
 -----------------------------------------------------------------------*/
bool AmayaSpeCharPanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaSpeCharPanel, AmayaSubPanel)
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F1"), AmayaSpeCharPanel::OnButtonFiltre1 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F2"), AmayaSpeCharPanel::OnButtonFiltre2 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F3"), AmayaSpeCharPanel::OnButtonFiltre3 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F4"), AmayaSpeCharPanel::OnButtonFiltre4 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F5"), AmayaSpeCharPanel::OnButtonFiltre5 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F6"), AmayaSpeCharPanel::OnButtonFiltre6 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F7"), AmayaSpeCharPanel::OnButtonFiltre7 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F8"), AmayaSpeCharPanel::OnButtonFiltre8 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INSERT"), AmayaSpeCharPanel::OnButtonInsert ) 
  END_EVENT_TABLE()

#endif /* #ifdef _WX */
