#ifdef _WX

#include "wx/wx.h"
#include "wx/string.h"

// Thotlib includes
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

#include "appdialogue_wx_f.h"

#include "AmayaToolBar.h"
#include "AmayaWindow.h"
#include "AmayaFrame.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaToolBar, wxPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  AmayaToolBar
 * Description:  create a new toolbar
 *--------------------------------------------------------------------------------------
 */
AmayaToolBar::AmayaToolBar( wxWindow * p_parent, AmayaWindow * p_amaya_window_parent ) : 
  wxPanel( p_parent,
	   -1,
	   wxDefaultPosition,
	   wxDefaultSize /*wxSize(-1, 28)*/, /* here it's possible to setup the toolbar size (height) */
	   wxTAB_TRAVERSAL )
{
  m_pAmayaWindowParent = p_amaya_window_parent;

  // Creation of the top sizer to contain tools
  m_pTopSizer = new wxBoxSizer ( wxHORIZONTAL );
  SetSizer( m_pTopSizer );

  SetAutoLayout(TRUE);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  ~AmayaToolBar
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AmayaToolBar::~AmayaToolBar()
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  AddTool
 * Description:  add a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaToolBar::AddTool( wxWindow * p_tool, bool expand, int border )
{
  int flag = wxEXPAND;
  int proportion = 0;

  if ( border > 0 )
    flag |= wxALL;
  
  if (expand)
    proportion = 1;

  m_pTopSizer->Add( p_tool, proportion, flag, border );
  m_pTopSizer->Layout();
  GetParent()->Layout();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  AddSeparator
 * Description:  add a separator (vertical black line)
 *--------------------------------------------------------------------------------------
 */
void AmayaToolBar::AddSeparator()
{
  wxPanel * p_separator = new wxPanel( this, -1, wxDefaultPosition, wxSize(2, -1) );
  p_separator->SetBackgroundColour( wxColour(_T("black")) );
  AddTool( p_separator, FALSE, 3 );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  AddSpacer
 * Description:  add a spacer (transparent box)
 *--------------------------------------------------------------------------------------
 */
void AmayaToolBar::AddSpacer( int width )
{
  wxPanel * p_spacer = new wxPanel( this, -1, wxDefaultPosition, wxSize(width, -1) );
  AddTool( p_spacer, FALSE, 3 );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnSize
 * Description:  not used
 *--------------------------------------------------------------------------------------
 */
void AmayaToolBar::OnSize( wxSizeEvent& event )
{
  wxLogDebug( _T("AmayaToolBar::OnSize - ")+
	      wxString(_T(" w=%d h=%d")),
	      event.GetSize().GetWidth(),
	      event.GetSize().GetHeight() );

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaToolBar
 *      Method:  OnSize
 * Description:  called when a toolbar button is clicked
 *--------------------------------------------------------------------------------------
 */
void AmayaToolBar::OnButtonActivated( wxCommandEvent &event )
{
  wxLogDebug( _T("AmayaToolBar::OnButtonActivated - ")+
	      wxString(_T(" id=%d")), event.GetId() );

  // Activate the correspondig callback
  AmayaFrame * p_frame = m_pAmayaWindowParent->GetActiveFrame();
  wxASSERT( p_frame != NULL );
  if ( p_frame )
    {
      int frame_id         = p_frame->GetFrameId();
      APP_Callback_ToolBarButtonActivate ( frame_id, event.GetId() );
    }
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaToolBar, wxPanel)
  EVT_SIZE( 		    AmayaToolBar::OnSize )
  EVT_BUTTON( -1,           AmayaToolBar::OnButtonActivated )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
