#ifdef _WX
#include "wx/wx.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "registry.h"
#include "appdialogue.h"
#include "message.h"
#include "dialogapi.h"
#include "application.h"
#include "appdialogue_wx.h"
#include "windowtypes_wx.h"
#include "registry_wx.h"
#include "logdebug.h"
#include "selection.h"

#include "exceptions_f.h"
#include "structschema_f.h"
#include "structselect_f.h"

#include "appdialogue_wx_f.h"
#include "message_wx.h"
#include "AmayaPathControl.h"


#include <wx/tooltip.h>
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(AmayaPathControlItemList);


/*------------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
AmayaPathControl::AmayaPathControl(wxWindow* parent, wxWindowID id,
                            const wxPoint& pos, const wxSize& size, long style):
wxControl(parent, id, pos, size, style),
m_focused(NULL)
{
}

AmayaPathControl::~AmayaPathControl()
{
}

void AmayaPathControl::SetSelection(Element elem)
{
  wxClientDC dc(this);
  
  wxRect rect;
  
  m_items.DeleteContents(true);
  m_items.clear();
  
  m_focused = NULL;
  while(elem!=NULL)
  {
    Element parent = TtaGetParent(elem);
    
    PtrElement ptr = (PtrElement)elem;
    /** @see BuildSelectionMessage () */
    if(parent && !HiddenType(ptr) && (!ptr->ElTerminal || (ptr->ElLeafType!=LtText && ptr->ElLeafType!=LtPicture)))
    {
      wxString name = TtaConvMessageToWX(TtaGetElementTypeName(TtaGetElementType(elem)));
      dc.GetTextExtent(name, &rect.width, &rect.height);
      AmayaPathControlItem* item = new AmayaPathControlItem;
      *item = (AmayaPathControlItem){name, elem, rect};
      m_items.Append(item);
    }    
    elem = parent;
  }
  Refresh();
}  

void AmayaPathControl::OnDraw(wxPaintEvent& event)
{
  PreCalcPositions();
  
  wxPaintDC dc(this);
  wxSize sz = GetClientSize();
  
  static wxString sep = wxT("/"); 
  wxSize szSep;
  dc.GetTextExtent(sep, &szSep.x, &szSep.y);
  
  int x = 0;
  bool bIsFirst = true;
  wxAmayaPathControlItemListNode *node = m_items.GetLast();
  while(node!=NULL)
  {
    if(node->GetData()->rect.x>=0)
    {
      if(!bIsFirst)
      {
        dc.DrawText(sep, x+2, 0);
        x += szSep.x + 4;
      }
      else
        bIsFirst = false;
      node->GetData()->Draw(dc, node->GetData()==m_focused);
      x += node->GetData()->rect.width;
    }
    node = node->GetPrevious();
  }
}

void AmayaPathControl::PreCalcPositions()
{
  wxClientDC dc(this);
  wxSize sz = GetClientSize();

  static wxString sep = wxT("/"); 
  wxSize szSep;
  dc.GetTextExtent(sep, &szSep.x, &szSep.y);

  int cx = 0;
  wxAmayaPathControlItemListNode *node;
  node = m_items.GetLast();
  while(node!=NULL)
  {
    node->GetData()->rect.x = cx;
    cx += node->GetData()->rect.width + szSep.x + 4;    
    node = node->GetPrevious();
  }

  if(cx>sz.x){
    cx -= sz.x;
    node = m_items.GetLast();
    while(node!=NULL)
    {
      node->GetData()->rect.x -= cx;
      node = node->GetPrevious();
    }
    
    node = m_items.GetLast();
    while(node!=NULL && node->GetData()->rect.x<0)
    {
      node = node->GetPrevious();
    }
    if(node!=NULL)
    {
      cx = node->GetData()->rect.x;
      while(node!=NULL)
      {
        node->GetData()->rect.x -= cx;
        node = node->GetPrevious();
      }
    }
  }
}

void AmayaPathControlItem::Draw(wxDC& dc, bool isFocused)
{
  if(rect.x>=0){
    if(isFocused)
    {
      wxColour col = dc.GetTextForeground();
      dc.SetTextForeground(wxSystemSettings::GetColour(isFocused?wxSYS_COLOUR_HIGHLIGHTTEXT:wxSYS_COLOUR_MENUTEXT));
      dc.DrawText(label, rect.x, rect.y);
      dc.SetTextForeground(col);
    }
    else
    {
      dc.DrawText(label, rect.x, rect.y);      
    }
  }
}


void AmayaPathControl::OnSize(wxSizeEvent& event)
{
  Refresh();
  event.Skip();
}

void AmayaPathControl::OnMouseMove(wxMouseEvent& event)
{
  wxPoint pos = event.GetPosition();
  if(!m_focused || (pos.x<m_focused->rect.x) || (pos.x>m_focused->rect.x+m_focused->rect.width))
  {
    m_focused = NULL;
    
    wxAmayaPathControlItemListNode *node = m_items.GetLast();
    while(node!=NULL)
    {
      if(node->GetData()->rect.x>=0)
      {
        if((pos.x>=node->GetData()->rect.x) && (pos.x<=node->GetData()->rect.x+node->GetData()->rect.width))
        {
          m_focused = node->GetData();
          Refresh();
          return;
        }
      }
      node = node->GetPrevious();
    }
    Refresh();
  }
}

void AmayaPathControl::OnMouseEnter(wxMouseEvent& WXUNUSED(event))
{
}

void AmayaPathControl::OnMouseLeave(wxMouseEvent& WXUNUSED(event))
{
  m_focused = NULL;
  Refresh();
}

void AmayaPathControl::OnMouseLeftUp(wxMouseEvent& event)
{
  wxPoint pos = event.GetPosition();
  wxAmayaPathControlItemListNode *node = m_items.GetLast();
  while(node)
  {
    if(node->GetData()->rect.x>=0)
    {
      if((pos.x>=node->GetData()->rect.x) && (pos.x<=node->GetData()->rect.x+node->GetData()->rect.width))
      {
        TtaSelectElement(TtaGetDocument(node->GetData()->elem), node->GetData()->elem);
        return;
      }
    }
    node = node->GetPrevious();
  }  
}


BEGIN_EVENT_TABLE(AmayaPathControl, wxControl)
  EVT_PAINT(AmayaPathControl::OnDraw)
  EVT_SIZE(AmayaPathControl::OnSize)
  EVT_ENTER_WINDOW(AmayaPathControl::OnMouseEnter)
  EVT_LEAVE_WINDOW(AmayaPathControl::OnMouseLeave)
  EVT_MOTION(AmayaPathControl::OnMouseMove)
  EVT_LEFT_UP(AmayaPathControl::OnMouseLeftUp)
END_EVENT_TABLE()

#endif /* _WX */
