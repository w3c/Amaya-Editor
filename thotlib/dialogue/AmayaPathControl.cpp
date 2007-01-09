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
  m_focused(NULL),
  m_height(0)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaPathControl::~AmayaPathControl()
{
}

/*----------------------------------------------------------------------
  Initialize nodes contents
  ----------------------------------------------------------------------*/
void AmayaPathControl::SetSelection(Element elem)
{
  wxRect         rect;
  wxString       name;
  PtrElement     pEl = (PtrElement)elem;
  ElementType    elType;
  char          *val;
  wxClientDC     dc(this);

  m_items.DeleteContents(true);
  m_items.clear();
  
  m_focused = NULL;
  m_height = 0;
  while (pEl)
    {
      /** @see BuildSelectionMessage () */
      if (pEl->ElParent && !HiddenType(pEl) &&
          (!pEl->ElTerminal || (pEl->ElLeafType != LtText && pEl->ElLeafType != LtPicture)))
        {
          elType = TtaGetElementType ((Element)pEl);
          val = TtaGetElementTypeName (elType);
#ifdef IV
if (pEl->ElStructSchema &&
    //!strcmp (pEl->ElStructSchema->SsName, "Template") &&
    !strcmp (val, "use"))
  // get attribute name
  printf ("use\n");
#endif
          name = TtaConvMessageToWX (val);
          dc.GetTextExtent(name, &rect.width, &rect.height);
          AmayaPathControlItem* item = new AmayaPathControlItem;
          item->label = name;
          item->elem = (Element)pEl;
          item->rect = rect;
          m_items.Append(item);
          if (rect.height > m_height)
            m_height = rect.height;
        }    
      pEl = pEl->ElParent;
    }
  Refresh();
}  

/*----------------------------------------------------------------------
  Initial display of nodes
  ----------------------------------------------------------------------*/
void AmayaPathControl::OnDraw(wxPaintEvent& event)
{
  static wxString                 sep = wxT("/"), etc = wxT("..."); 
  wxClientDC                      dc(this);
  wxSize                          sz = GetClientSize();
  int                             y = (sz.y - m_height)/2;
  int                             cx = 0, x = 0, dx;
  wxAmayaPathControlItemListNode *node;
  wxSize                          szSep, szEtc;
  bool                            bIsFirst = true;

  dc.GetTextExtent(sep, &szSep.x, &szSep.y);
  szSep.x += 4;
  dc.GetTextExtent(etc, &szEtc.x, &szEtc.y);
  szEtc.x += 6;
  // Compute the position and size of all nodes
  node = m_items.GetLast();
  //node = m_items.GetFirst();
  while (node)
    {
      node->GetData()->rect.x = cx;
      node->GetData()->rect.y = y;
      cx += node->GetData()->rect.width + szSep.x;    
      node = node->GetPrevious();
      //node = node->GetNext();
    }

  if (cx > sz.x)
    {
      /* only a subset of nodes can be displayed */
      x = 0;
      dx = cx - sz.x + szEtc.x;
      // display "..."
      dc.DrawText(etc, x+2, y);
      x += szEtc.x;
      node = m_items.GetLast();
      while (node)
        {
          cx = node->GetData()->rect.x - dx;
          if (cx < szEtc.x)
            // keep space to display "..."
            node->GetData()->rect.x = cx;
          else
            {
              // add a '/' before
              dc.DrawText(sep, x+2, y);
              x += szSep.x;
              node->GetData()->rect.x = x;
              node->GetData()->Draw(dc, node->GetData() == m_focused);
              x += node->GetData()->rect.width;
            }
          node = node->GetPrevious();
        }
    }
  else
    {
      x = 0;
      // display first the root element
      node = m_items.GetLast();
      while (node)
        {
          if (bIsFirst)
            bIsFirst = false;
          else
            {
              // add a '/' before
              dc.DrawText(sep, x+2, y);
              x += szSep.x;
            }
          node->GetData()->Draw(dc, node->GetData() == m_focused);
          x += node->GetData()->rect.width;
          node = node->GetPrevious();
        }
    }
}


/*----------------------------------------------------------------------
  Redisplay all nodes
  ----------------------------------------------------------------------*/
void AmayaPathControlItem::Draw(wxDC& dc, bool isFocused)
{
  if (rect.x>=0)
    {
      if (isFocused)
        {
          wxColour col = dc.GetTextForeground();
          dc.SetTextForeground(/*wxSystemSettings::GetColour(isFocused?wxSYS_COLOUR_HIGHLIGHTTEXT:wxSYS_COLOUR_MENUTEXT)*/ wxColour(255, 0, 0));
          dc.DrawText(label, rect.x, rect.y);
          dc.SetTextForeground(col);
        }
      else
        {
          dc.DrawText(label, rect.x, rect.y);      
        }
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaPathControl::OnSize(wxSizeEvent& event)
{
  Refresh();
  event.Skip();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaPathControl::OnMouseMove(wxMouseEvent& event)
{
  wxPoint pos = event.GetPosition();
  if (!m_focused || (pos.x<m_focused->rect.x) ||
      (pos.x>m_focused->rect.x+m_focused->rect.width))
    {
      m_focused = NULL;
    
      // display first the root element
      wxAmayaPathControlItemListNode *node = m_items.GetLast();
      //wxAmayaPathControlItemListNode *node = m_items.GetFirst();
      while (node!=NULL)
        {
          if (node->GetData()->rect.x >= 0)
            {
              if ((pos.x>=node->GetData()->rect.x) &&
                  (pos.x<=node->GetData()->rect.x+node->GetData()->rect.width))
                {
                  m_focused = node->GetData();
                  Refresh();
                  return;
                }
            }
          node = node->GetPrevious();
          //node = node->GetNext();
        }
      Refresh();
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaPathControl::OnMouseEnter(wxMouseEvent& WXUNUSED(event))
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaPathControl::OnMouseLeave(wxMouseEvent& WXUNUSED(event))
{
  m_focused = NULL;
  Refresh();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaPathControl::OnMouseLeftUp(wxMouseEvent& event)
{
  wxPoint pos = event.GetPosition();

  // display first the root element
  wxAmayaPathControlItemListNode *node = m_items.GetLast();
  //wxAmayaPathControlItemListNode *node = m_items.GetFirst();
  while (node)
    {
      if (node->GetData()->rect.x >= 0)
        {
          if ((pos.x >= node->GetData()->rect.x) &&
              (pos.x <= node->GetData()->rect.x+node->GetData()->rect.width))
            {
              TtaSelectElement(TtaGetDocument(node->GetData()->elem), node->GetData()->elem);
              return;
            }
        }
      node = node->GetPrevious();
      //node = node->GetNext();
    }  
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPathControl, wxControl)
  EVT_PAINT(AmayaPathControl::OnDraw)
  EVT_SIZE(AmayaPathControl::OnSize)
  EVT_ENTER_WINDOW(AmayaPathControl::OnMouseEnter)
  EVT_LEAVE_WINDOW(AmayaPathControl::OnMouseLeave)
  EVT_MOTION(AmayaPathControl::OnMouseMove)
  EVT_LEFT_UP(AmayaPathControl::OnMouseLeftUp)
  END_EVENT_TABLE()

#endif /* _WX */
