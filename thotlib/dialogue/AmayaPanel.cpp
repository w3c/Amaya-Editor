#ifdef _WX

#include "AmayaPanel.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  AmayaPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaPanel::AmayaPanel(
//                 int            panel_id
      		wxWindow *     p_parent_window
		,wxWindowID     id
	        ,const wxPoint& pos
	        ,const wxSize&  size
		,long style
	      )
  :  wxPanel( wxDynamicCast(p_parent_window, wxWindow),
	      id, pos, size, style )
//          ,m_PanelId( panel_id )
{
  SetBackgroundColour( wxColour(_T("BLUE")) );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  ~AmayaPanel
 * Description:  destructor
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaPanel::~AmayaPanel()
{
  
}

#endif /* #ifdef _WX */
