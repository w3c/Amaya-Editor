#ifdef _WX

#include "wx/wx.h"

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
#include "view.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "attrmenu_f.h"
#include "frame_tv.h"

#include "message_wx.h"
#include "registry_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"

#include "AmayaSubPanelManager.h"
#include "AmayaNormalWindow.h"
#include "AmayaPanel.h"

// the only requirement for the rest is to be AFTER the full declaration of
// MyListElement (for WX_DECLARE_LIST forward declaration is enough), but
// usually it will be found in the source file and not in the header
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(SubPanelList);

AmayaSubPanelManager * AmayaSubPanelManager::m_pInstance = 0;

AmayaSubPanelManager::AmayaSubPanelManager()
{

}

AmayaSubPanelManager::~AmayaSubPanelManager()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  GetInstance
 * Description:  implemente a singleton
 *--------------------------------------------------------------------------------------
 */
AmayaSubPanelManager * AmayaSubPanelManager::GetInstance()
{
  if (!m_pInstance)
    m_pInstance = new AmayaSubPanelManager();
  return m_pInstance;  
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  RegisterSubPanel
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaSubPanelManager::RegisterSubPanel( AmayaSubPanel * p_panel )
{
  wxLogDebug( _T("AmayaSubPanelManager::RegisterSubPanel") );

  // take care to not register twice
  bool ret = false;
  if (!m_RegistredPanel.Find(p_panel))
    {
      m_RegistredPanel.Append(p_panel);
      
      // the panel must be updated
      p_panel->ShouldBeUpdated();
      p_panel->DoUpdate();

      // the panel states must be synchronized depending on other panel stats
      AmayaSubPanel * floating_panel = NULL;
      for ( SubPanelList::Node *node = m_RegistredPanel.GetFirst(); node && !floating_panel; node = node->GetNext() )
	{
	  AmayaSubPanel * current = node->GetData();
	  if ( current->GetPanelType() == p_panel->GetPanelType() )
	    if ( current->GetState() & AmayaSubPanel::wxAMAYA_SPANEL_FLOATING )
	      // oupss there is already a floating panel of same type
	      floating_panel = current;
	}
      if (floating_panel)
	  p_panel->ChangeState( p_panel->GetState()|AmayaSubPanel::wxAMAYA_SPANEL_FLOATING);
      
      ret = true;
    }

  DebugSubPanelList();

  if (ret)
    wxLogDebug( _T("AmayaSubPanelManager::RegisterSubPanel : OK") );
  else
    wxLogDebug( _T("AmayaSubPanelManager::RegisterSubPanel : !OK") );   

  return ret;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  UnregisterSubPanel
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaSubPanelManager::UnregisterSubPanel( AmayaSubPanel * p_panel )
{
  // take care to not delete an existing node
  bool ret = false;
  SubPanelList::Node* p_node = m_RegistredPanel.Find(p_panel);
  if (p_node)
    {
      // force to the unfloting state
      DoUnfloat( p_panel );
      
      ret = m_RegistredPanel.DeleteNode(p_node);
    }

  DebugSubPanelList();
  
  if (ret)
    wxLogDebug( _T("AmayaSubPanelManager::UnregisterSubPanel [%d] OK"), p_panel->GetPanelType() );
  else
    wxLogDebug( _T("AmayaSubPanelManager::UnregisterSubPanel [%d] !OK"), p_panel->GetPanelType() );   

  return ret;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  CanChangeState
 * Description:  return yes if the given panel is allowed to change his state
 *               if no maybe do something because it means that the user can't do an action
 *               so maybe a warning message, or just raise the blocking floating panel if it exists.
 *--------------------------------------------------------------------------------------
 */
bool AmayaSubPanelManager::CanChangeState( AmayaSubPanel * p_panel, unsigned int new_state )
{
  unsigned int old_state    = p_panel->GetState();
  unsigned int changing_bit = old_state ^ new_state;

  // the panel wants to change its flot state
  if ( (changing_bit & AmayaSubPanel::wxAMAYA_SPANEL_FLOATING) )
    {
      if (old_state & AmayaSubPanel::wxAMAYA_SPANEL_FLOATING)
	{
	  // the panel was floating, it wants now to be attached
	  
	  // no probleme, a panel can be attached without constraint
	  wxLogDebug( _T("AmayaSubPanelManager::CanChangeState - was floating, you can attach") );
	  return true;
	}
      else
	{
	  // the panel was attached, it wants now to float

	  // check that it does not existe a floating panel somewhere, raise it if it is found
	  // let's iterate over the list
	  AmayaSubPanel * floating_panel = NULL;
	  for ( SubPanelList::Node *node = m_RegistredPanel.GetFirst(); node && !floating_panel; node = node->GetNext() )
	    {
	      AmayaSubPanel * current = node->GetData();
	      if ( current->GetPanelType() == p_panel->GetPanelType() )
		if ( current->GetState() & AmayaSubPanel::wxAMAYA_SPANEL_FLOATING )
		  // oupss there is already a floating panel of same type
		  floating_panel = current;
	    }
	  // raise the floating panel it if found
	  if (floating_panel)
	    {
	      floating_panel->Raise();
	      wxLogDebug( _T("AmayaSubPanelManager::CanChangeState - want float, you can't float, another is already floating") );
	      return false; // do not allow to create a new floating panel
	    }
	  else
	    {
	      wxLogDebug( _T("AmayaSubPanelManager::CanChangeState - want float, you can float") );
	      return true; // nothing so ok, the request is accept, it cans create a new floating panel
	    }
	}
    }

  // the panel wants to change its expand state
  if ( (changing_bit & AmayaSubPanel::wxAMAYA_SPANEL_EXPANDED) )
    {
      if (old_state & AmayaSubPanel::wxAMAYA_SPANEL_EXPANDED)
	{
	  // the panel was expanded, it wants now to be unexpanded
	  wxLogDebug( _T("AmayaSubPanelManager::CanChangeState - was expanded, you can unexpand") );
	  return true;
	}
      else
	{
	  // the panel was unexpanded, it wants now to be expanded
	  wxLogDebug( _T("AmayaSubPanelManager::CanChangeState - was unexpanded, you can expand") );
	  return true;
	}
    }

  // the panel's state didn't change, do nothing
  return false;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  DebugSubPanelList
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanelManager::DebugSubPanelList()
{
  // let's iterate over the list
  wxString sp_list;
  for ( SubPanelList::Node *node = m_RegistredPanel.GetFirst(); node; node = node->GetNext() )
    {
      AmayaSubPanel * current = node->GetData();
      sp_list += wxString::Format(_T("%x "), current);
    }
  wxLogDebug( _T("AmayaSubPanelManager::DebugSubPanelList - @registred_panel_list=[")+sp_list+_T("]") );
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  UnExpand
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanelManager::UnExpand( AmayaSubPanel * p_panel )
{
  wxLogDebug( _T("AmayaSubPanelManager::UnExpand [%x]"), p_panel );

  // do nothing if the content is already unexpanded or if it is floating
  if ( !CanChangeState(p_panel, p_panel->GetState()&~AmayaSubPanel::wxAMAYA_SPANEL_EXPANDED) )
    return;

  p_panel->ChangeState( p_panel->GetState()&~AmayaSubPanel::wxAMAYA_SPANEL_EXPANDED );
  p_panel->UnExpand();
  p_panel->DoUpdate();
  p_panel->GetParent()->FitInside();

  SaveSubPanelState(p_panel);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  Expand
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanelManager::Expand( AmayaSubPanel * p_panel )
{
  wxLogDebug( _T("AmayaSubPanelManager::Expand [%x]"), p_panel );

  // do nothing if the content is already expanded or if it is floating
  if ( !CanChangeState(p_panel, p_panel->GetState()|AmayaSubPanel::wxAMAYA_SPANEL_EXPANDED) )
    return;

  p_panel->ChangeState( p_panel->GetState()|AmayaSubPanel::wxAMAYA_SPANEL_EXPANDED );
  p_panel->Expand();
  p_panel->DoUpdate();
  p_panel->GetParent()->FitInside();

  SaveSubPanelState(p_panel);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  DoFloat
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanelManager::DoFloat( AmayaSubPanel * p_panel )
{
  wxLogDebug( _T("AmayaSubPanelManager::DoFloat [%x]"), p_panel );

  // can float ?
  if ( !CanChangeState(p_panel, p_panel->GetState()|AmayaSubPanel::wxAMAYA_SPANEL_FLOATING) )
    return;

  // - every side-panels must unexpand before showing the unique floating panel
  // - every side-panels must have floating state on
  for ( SubPanelList::Node *node = m_RegistredPanel.GetFirst(); node; node = node->GetNext() )
    {
      AmayaSubPanel * current = node->GetData();
      if ( current->GetPanelType() == p_panel->GetPanelType() )
	{
	  current->ChangeState( current->GetState()|AmayaSubPanel::wxAMAYA_SPANEL_FLOATING );
	  current->UnExpand();
	}
    }

  // the panel should be updated because floating state change
  p_panel->ShouldBeUpdated();

  // ok now float the panel
  p_panel->DoFloat();
  p_panel->DoUpdate();
  p_panel->GetParent()->FitInside();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  DoUnfloat
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanelManager::DoUnfloat( AmayaSubPanel * p_panel )
{
  wxLogDebug( _T("AmayaSubPanelManager::DoUnfloat [%x][%d]"), p_panel, p_panel->GetPanelType() );

  // can unfloat ?
  if ( !CanChangeState(p_panel, p_panel->GetState()&~AmayaSubPanel::wxAMAYA_SPANEL_FLOATING) )
    return;
  

  // besure that the panel has right unfloating state before switching references
  p_panel->ChangeState( p_panel->GetState()&~AmayaSubPanel::wxAMAYA_SPANEL_FLOATING );
  // the panel should be updated because floating state change
  p_panel->ShouldBeUpdated();

  // - hide every (unique) floating panel
  // - restore every side-panels
  // - every side-panels must have floating state off
  for ( SubPanelList::Node *node = m_RegistredPanel.GetFirst(); node ; node = node->GetNext() )
    {
      AmayaSubPanel * current = node->GetData();
      if ( current->GetPanelType() == p_panel->GetPanelType() )
	{
	  current->ChangeState( current->GetState()&~AmayaSubPanel::wxAMAYA_SPANEL_FLOATING );
	  current->DoUnfloat();
	  if (current->IsExpanded())
	    current->Expand();
	  else
	    current->UnExpand();
	}
    }  
  p_panel->DoUpdate();
  p_panel->GetParent()->FitInside();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  SendDataToPanel
 * Description:  distribute new values to the given panel and all its brothers
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanelManager::SendDataToPanel( int panel_type, AmayaParams& params )
{
  // Get the active window
  AmayaWindow * activeWindow = TtaGetActiveWindow();

  // send data to each registred panel which have same type
  for ( SubPanelList::Node *node = m_RegistredPanel.GetFirst(); node ; node = node->GetNext() )
    {
      AmayaSubPanel * current = node->GetData();
      if ( current->GetPanelType() == panel_type &&
	   (current->IsFloating () || current->GetTopAmayaWindow() == activeWindow) )
	current->SendDataToPanel( params );
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  IsActive
 * Description:  returns true if one or more panel of the given type is active (=visible)
 *--------------------------------------------------------------------------------------
 */
bool AmayaSubPanelManager::IsActive( int panel_type )
{
  bool is_active = false;
  // search into the panel list , one active panel
  for ( SubPanelList::Node *node = m_RegistredPanel.GetFirst(); node && !is_active ; node = node->GetNext() )
    {
      AmayaSubPanel * current = node->GetData();
      if ( current->GetPanelType() == panel_type )
	is_active |= current->IsActive();
    }
  return is_active;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  ShouldBeUpdated
 * Description:  call this function to switch on/off list update
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanelManager::ShouldBeUpdated( int panel_type, bool should_update )
{
  // warn each panel to update its content when it can
  for ( SubPanelList::Node *node = m_RegistredPanel.GetFirst(); node; node = node->GetNext() )
    {
      AmayaSubPanel * current = node->GetData();
      if ( current->GetPanelType() == panel_type )
	current->ShouldBeUpdated( should_update );
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  CheckForDoUpdate
 * Description:  verify if panels should be updated or not, update it if necessary
 *               if pannel_type == WXAMAYA_PANEL_UNKNOWN then check for every panels
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanelManager::CheckForDoUpdate( int panel_type )
{
  // warn each panel to update its content when it can
  for ( SubPanelList::Node *node = m_RegistredPanel.GetFirst(); node; node = node->GetNext() )
    {
      AmayaSubPanel * current = node->GetData();
      if ( current->GetPanelType() == panel_type || panel_type == WXAMAYA_PANEL_UNKNOWN )
	current->DoUpdate();
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanelManager
 *      Method:  SaveSubPanelState
 * Description:  save into registry subpanel expanded state
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanelManager::SaveSubPanelState( AmayaSubPanel * p_panel )
{
  ThotBool value = p_panel->IsExpanded();
  switch (p_panel->GetPanelType())
    {
    case WXAMAYA_PANEL_XHTML:
      TtaSetEnvBoolean("OPEN_PANEL_XHTML", value, TRUE);
      break;
    case WXAMAYA_PANEL_ATTRIBUTE:
      TtaSetEnvBoolean("OPEN_PANEL_ATTRIBUTE", value, TRUE);
      break;
    case WXAMAYA_PANEL_COLORS:
      TtaSetEnvBoolean("OPEN_PANEL_COLORS", value, TRUE);
      break;
    case WXAMAYA_PANEL_CHARSTYLE:
      TtaSetEnvBoolean("OPEN_PANEL_CHARSTYLE", value, TRUE);
      break;
    case WXAMAYA_PANEL_FORMAT:
      TtaSetEnvBoolean("OPEN_PANEL_FORMAT", value, TRUE);
      break;
    case WXAMAYA_PANEL_APPLYCLASS:
      TtaSetEnvBoolean("OPEN_PANEL_APPLYCLASS", value, TRUE);
      break;
    case WXAMAYA_PANEL_MATHML:
      TtaSetEnvBoolean("OPEN_PANEL_MATHML", value, TRUE);
      break;
    case WXAMAYA_PANEL_XML:
      TtaSetEnvBoolean("OPEN_PANEL_XML", value, TRUE);
      break;
    case WXAMAYA_PANEL_SPECHAR:
      TtaSetEnvBoolean("OPEN_PANEL_SPECHAR", value, TRUE);
      break;
    }
}

#endif /* _WX */
