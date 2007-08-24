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
#include "displayview_f.h"
#include "mathml_filtres.h"


//
//
// AmayaSpeCharToolPanel
//
//

typedef struct _XmlEntity
{
  char         *charName;      
  int           charCode;      
} XmlEntity;


MathMLEntityHash AmayaSpeCharToolPanel::m_MathMLEntityHash;
int * AmayaSpeCharToolPanel::m_pActiveFiltre = NULL;


IMPLEMENT_DYNAMIC_CLASS(AmayaSpeCharToolPanel, AmayaToolPanel)

AmayaSpeCharToolPanel::AmayaSpeCharToolPanel():
  AmayaToolPanel()
{
}

AmayaSpeCharToolPanel::~AmayaSpeCharToolPanel()
{
}

bool AmayaSpeCharToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_SPECHAR")))
    return false;
  
  m_pList = XRCCTRL(*this,"wxID_PANEL_MATH_LIST",wxComboBox);

  m_OffColour = XRCCTRL(*this, "wxID_PANEL_MATH_F1", wxBitmapButton)->GetBackgroundColour();
  m_OnColour  = wxColour(250, 200, 200);

  RefreshButtonState();
  
  XRCCTRL(*this,"wxID_PANEL_MATH_INSERT",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INSERT)));
  
  XRCCTRL(*this,"wxID_PANEL_MATH_F1",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_GREEK_ALPHABET)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F2",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_GREEK_CAP)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F3",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_MATHML)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F4",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_OPERATOR)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F5",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BINARY_REL)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F6",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BINARY_REL_NEG)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F7",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_MISC)));
  XRCCTRL(*this,"wxID_PANEL_MATH_F8",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ARROW)));
  
  return true;
}

wxString AmayaSpeCharToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SPECHAR));
}


/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre1
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::OnButtonFiltre1( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_greek;
  DoFilter( filtre_greek );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharToolPanel::wxSPECHAR_ACTION_REFRESH;
  TtaSendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre2
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::OnButtonFiltre2( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_greek_maj;
  DoFilter( filtre_greek_maj );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharToolPanel::wxSPECHAR_ACTION_REFRESH;
  TtaSendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre3
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::OnButtonFiltre3( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_maths;
  DoFilter( filtre_maths );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharToolPanel::wxSPECHAR_ACTION_REFRESH;
  TtaSendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre4
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::OnButtonFiltre4( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_operateurs;
  DoFilter( filtre_operateurs );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharToolPanel::wxSPECHAR_ACTION_REFRESH;
  TtaSendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre5
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::OnButtonFiltre5( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_relations_binaires;
  DoFilter( filtre_relations_binaires );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharToolPanel::wxSPECHAR_ACTION_REFRESH;
  TtaSendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre6
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::OnButtonFiltre6( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_relations_binaires_negation;
  DoFilter( filtre_relations_binaires_negation );

  AmayaParams p;
  p.param1 = (int)AmayaSpeCharToolPanel::wxSPECHAR_ACTION_REFRESH;
  TtaSendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre7
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::OnButtonFiltre7( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_divers;
  DoFilter( filtre_divers );
  
  AmayaParams p;
  p.param1 = (int)AmayaSpeCharToolPanel::wxSPECHAR_ACTION_REFRESH;
  TtaSendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre8
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::OnButtonFiltre8( wxCommandEvent& event )
{
  m_pActiveFiltre = filtre_fleches;
  DoFilter( filtre_fleches );
  
  AmayaParams p;
  p.param1 = (int)AmayaSpeCharToolPanel::wxSPECHAR_ACTION_REFRESH;
  TtaSendDataToPanel( WXAMAYA_PANEL_SPECHAR, p );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  DoFilter
 * Description:  this method is called to refresh the entity list with a given filter
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::DoFilter( int * filtre )
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
void AmayaSpeCharToolPanel::RefreshButtonState()
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
void AmayaSpeCharToolPanel::OnButtonInsert( wxCommandEvent& event )
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
 *       Class:  AmayaSpeCharToolPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::SendDataToPanel( AmayaParams& p )
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
 *       Class:  AmayaSpeCharToolPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::DoUpdate()
{
  AmayaToolPanel::DoUpdate();
  DoFilter( m_pActiveFiltre );
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaSpeCharToolPanel, AmayaToolPanel)
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F1"), AmayaSpeCharToolPanel::OnButtonFiltre1 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F2"), AmayaSpeCharToolPanel::OnButtonFiltre2 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F3"), AmayaSpeCharToolPanel::OnButtonFiltre3 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F4"), AmayaSpeCharToolPanel::OnButtonFiltre4 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F5"), AmayaSpeCharToolPanel::OnButtonFiltre5 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F6"), AmayaSpeCharToolPanel::OnButtonFiltre6 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F7"), AmayaSpeCharToolPanel::OnButtonFiltre7 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_F8"), AmayaSpeCharToolPanel::OnButtonFiltre8 )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INSERT"), AmayaSpeCharToolPanel::OnButtonInsert ) 
END_EVENT_TABLE()


#endif /* #ifdef _WX */
