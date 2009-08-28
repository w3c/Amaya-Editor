/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"
#include "select_tv.h"


#include "callback_f.h"
#include "exceptions_f.h"
#include "structschema_f.h"
#include "structselect_f.h"

#include "appdialogue_wx_f.h"
#include "message_wx.h"
#include "AmayaPathControl.h"


#include <wx/tooltip.h>
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(AmayaPathControlItemList);


static wxString s_sep = wxT(">");
static wxString s_dots = wxT("...");
static ThotBool Set_ColumnSelected = FALSE;

/*------------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
AmayaPathControl::AmayaPathControl(wxWindow* parent, wxWindowID id,
                            const wxPoint& pos, const wxSize& size, long style):
wxControl(parent, id, pos, size, wxBORDER_NONE),
m_focused(NULL),
m_height(0)
{
  SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CLIC_ELEM_TO_SELECT_IT)));
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
AmayaPathControl::~AmayaPathControl()
{
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void AmayaPathControl::SetSelection(Element elem)
{
  wxClientDC          dc(this);
  PtrElement          pEl = (PtrElement)elem;
  PtrAttribute        pAttr;
  ElementType         elType;
  wxString            name;
  wxRect              rect;
  int                 length, kind, i;
  char                buffer[MAX_LENGTH], buff[MAX_LENGTH];
  ThotBool            xtiger = FALSE, file = FALSE;
  Document            doc = TtaGetDocument(elem);
  SSchema             htmlSSchema = TtaGetSSchema ("HTML", doc);
  AttributeType       attrType;
  Attribute           attr;
  const char*         elname;
  
#ifdef _WINDOWS
  dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#endif /* _WINDOWS */
  m_items.DeleteContents(true);
  m_items.clear();
  m_focused = NULL;
  m_height = 0;
  Set_ColumnSelected = WholeColumnSelected;
  while (pEl)
    {
      elType = TtaGetElementType (Element(pEl));
      xtiger = FALSE;
      if (pEl->ElParent &&
          !HiddenType (pEl) &&
          (!WholeColumnSelected ||
           pEl == (PtrElement) elem ||
           pEl->ElParent == NULL || pEl->ElParent->ElParent == NULL ||
           (!TypeHasException (ExcIsRow, pEl->ElTypeNumber, pEl->ElStructSchema) &&
           !TypeHasException (ExcIsTable, pEl->ElParent->ElParent->ElTypeNumber,
                              pEl->ElParent->ElParent->ElStructSchema))) &&     
          (!pEl->ElTerminal ||
           (pEl->ElLeafType != LtText && pEl->ElLeafType != LtPicture)))
        {
          buffer[0] = EOS;
          if ((pEl->ElStructSchema && pEl->ElStructSchema->SsName &&
              !strcmp (pEl->ElStructSchema->SsName, "Template")))
            {
              // The element is an XTiger element.
              // Replace its name by the attribute value
              xtiger = TRUE;
              pAttr = GetAttrElementWithException (ExcGiveName, pEl);
              if (pAttr)
                {
                  /* copy the NAME attribute into TargetName */
                  length = MAX_LENGTH-1;
                  TtaGiveTextAttributeValue ((Attribute)pAttr, buffer, &length);
                }
              else
                {
                  pAttr = GetAttrElementWithException (ExcGiveTypes, pEl);
                  if (pAttr)
                    {
                      /* get the first type name */
                      length = MAX_LENGTH-1;
                      TtaGiveTextAttributeValue ((Attribute)pAttr, buff, &length);
                      for (i = 0; i < length; i++)
                        if (buff[i] == SPACE)
                          {
                            buff[i] = EOS;
                            i = length;
                          }
                      strcat (buffer, buff);
                    }
                }
            }
          else if (pEl->ElStructSchema && pEl->ElStructSchema->SsName &&
                   !strcmp (pEl->ElStructSchema->SsName, "TextFile"))
            {
              // The showed document is a text file.
              // Display the selection line number
              file = TRUE;
              sprintf (buffer, "Line: %d", pEl->ElLineNb);
            }
          else if (WholeColumnSelected && pEl == (PtrElement) elem &&
                   pEl->ElStructSchema == (PtrSSchema)htmlSSchema)
            {
              // The selection is a table column
              elname = pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName;
              if (!strcmp (elname, "td") || !strcmp (elname, "th"))
                sprintf (buffer, "Column");
            }

          if (buffer[0] != EOS)
              // replace the element name by the attribute value
              name = TtaConvMessageToWX (buffer);
          else
            {
              name = TtaConvMessageToWX(TtaGetElementTypeName(elType));
              
              if (elType.ElSSchema == htmlSSchema)
                {
                  TtaGiveAttributeTypeFromName("class", Element(pEl), &attrType, &kind);
                  attr = TtaGetAttribute(Element(pEl), attrType);
                  if (attr)
                    {
                      /* get the first class name */
                      length = MAX_LENGTH;
                      TtaGiveTextAttributeValue(attr, buff, &length);
                      name.Alloc(name.Length()+length+1);
                      for (i = 0; i < length; i++)
                        {
                          if (buff[i] != SPACE)
                            {
                              name += wxT('.');
                              while (buff[i] != EOS && buff[i] != SPACE)
                                {
                                  name += wxChar (buff[i]);
                                  i++;
                                }
                            }
                        }
                    }
                }
            }
          dc.GetTextExtent (name, &rect.width, &rect.height);
          AmayaPathControlItem* item = new AmayaPathControlItem;
          item->label = name;
          item->elem = Element(pEl);
          item->rect = rect;
          item->isXTiger = xtiger;
           m_items.Append(item);
          if (rect.height > m_height)
            m_height = rect.height;
        }

      if (file)
        // don't display the hierarchy
        pEl = NULL;
      else
        pEl = pEl->ElParent;
    }
  Refresh();
}  

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void AmayaPathControl::OnDraw(wxPaintEvent& event)
{
  PreCalcPositions();
  
  wxPaintDC dc(this);
#ifdef _WINDOWS
  dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#endif /* _WINDOWS */
  wxSize sz = GetClientSize();
  
  wxSize szSep;
  dc.GetTextExtent(s_sep, &szSep.x, &szSep.y);
  szSep.x += 4;
  wxSize szDots;
  dc.GetTextExtent(s_dots, &szDots.x, &szDots.y);
  szDots.x += 4;
  
  int         x = 0;
  int         y = (sz.y-m_height)/2;
  bool        bIsFirst = true;
  AmayaPathControlItem *data;
  wxAmayaPathControlItemListNode *node = m_items.GetLast();

  while (node)
    {
      data = node->GetData();
      if (data->rect.x >= 0)
        {
          if (!bIsFirst)
            {
              dc.DrawText(s_sep, x+2, y);
              x += szSep.x;
            }
          else
            bIsFirst = false;
          data->Draw(dc, data == m_focused);
          x += data->rect.width;
        }
      else if (bIsFirst)
        {
          // display "..."
          dc.DrawText(s_dots, x+2, y);
          x = szDots.x;
          bIsFirst = false;
        }

      node = node->GetPrevious();
    }
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void AmayaPathControl::PreCalcPositions()
{
  wxClientDC dc(this);
  wxSize sz = GetClientSize();
  int y = (sz.y-m_height)/2, x, dx;

  wxSize szSep;
  dc.GetTextExtent(s_sep, &szSep.x, &szSep.y);
  szSep.x += 4;
  wxSize szDots;
  dc.GetTextExtent(s_dots, &szDots.x, &szDots.y);
  szDots.x += 4;

  int cx = 0;
  wxAmayaPathControlItemListNode *node;
  AmayaPathControlItem *data;
  node = m_items.GetLast();
  while (node)
    {
      // compute the initial position for each node
      data = node->GetData();
      data->rect.x = cx;
      data->rect.y = y;
      cx += data->rect.width + szSep.x;    
      node = node->GetPrevious();
    }

  if (cx > sz.x)
    {
      // replace some nodes by ...
      dx = cx - sz.x + szDots.x;
      x = szDots.x;
      node = m_items.GetLast();
      while (node)
        {
          data = node->GetData();
          cx = data->rect.x - dx;
          if (cx < szDots.x)
            // this node is not displayed
            data->rect.x = cx - szDots.x;
          else
            {
              // adding p '/' before
              x += szSep.x;
              data->rect.x = x;
              x += node->GetData()->rect.width;
            }
          node = node->GetPrevious();
        }
    }
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void AmayaPathControlItem::Draw(wxDC& dc, bool isFocused)
{
  if (rect.x >= 0)
  {
    if (isFocused)
    {
      wxColour col = dc.GetTextForeground();
      dc.SetTextForeground(wxColour(255, 0, 0));
      dc.DrawText(label, rect.x, rect.y);
      dc.SetTextForeground(col);
    }
    else if (isXTiger)
    {
      wxColour col = dc.GetTextForeground();
      dc.SetTextForeground(wxColour(140, 140, 10));
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
  -----------------------------------------------------------------------*/
void AmayaPathControl::OnSize(wxSizeEvent& event)
{
  Refresh();
  event.Skip();
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void AmayaPathControl::OnMouseMove(wxMouseEvent& event)
{
  wxPoint pos = event.GetPosition();
  if (!m_focused || pos.x < m_focused->rect.x ||
	  pos.x > m_focused->rect.x+m_focused->rect.width)
  {
    m_focused = NULL;
    
    wxAmayaPathControlItemListNode *node = m_items.GetLast();
    while(node!=NULL)
    {
      if (node->GetData()->rect.x >= 0)
      {
        if(pos.x >= node->GetData()->rect.x &&
			pos.x <= node->GetData()->rect.x+node->GetData()->rect.width)
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

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void AmayaPathControl::OnMouseEnter(wxMouseEvent& WXUNUSED(event))
{
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void AmayaPathControl::OnMouseLeave(wxMouseEvent& WXUNUSED(event))
{
  m_focused = NULL;
  Refresh();
}

extern void SynchronizeAppliedStyle (NotifyElement *event);
extern void SynchronizeNSDeclaration (NotifyElement *event);
/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
void AmayaPathControl::OnMouseLeftUp(wxMouseEvent& event)
{
  NotifyElement       notifyEl;
  Document            doc;
  PtrElement          pEl;
  wxPoint             pos = event.GetPosition();
  wxAmayaPathControlItemListNode *node = m_items.GetLast(), *previous;

  while (node)
    {
      previous = node->GetPrevious();
      if (node->GetData()->rect.x >= 0)
        {
          pEl = (PtrElement)node->GetData()->elem;
          if (pos.x >= node->GetData()->rect.x &&
             pos.x <= node->GetData()->rect.x+node->GetData()->rect.width)
            {
              doc = TtaGetDocument(node->GetData()->elem);
              if (doc)
                {
                  if (previous == NULL && Set_ColumnSelected)
                    TtaSelectEnclosingColumn ((Element)pEl);
                  else
                    {
                      TtaSelectElementWithoutPath (doc, node->GetData()->elem);
                      TtaRedirectFocus ();
                      /* WARNING: update applied style */
                      notifyEl.event = TteElemSelect;
                      notifyEl.document = doc;
                      notifyEl.element = (Element)pEl;
                      notifyEl.info = 0; /* not sent by undo */
                      notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
                      notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
                      notifyEl.position = 0;
                      SynchronizeAppliedStyle (&notifyEl);
                      /* WARNING: update NS list */
                      SynchronizeNSDeclaration (&notifyEl);
                    }
                  return;
                }
            }
        }
      node = previous;
    }
  TtaRedirectFocus ();
}


/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPathControl, wxControl)
  EVT_PAINT(AmayaPathControl::OnDraw)
  EVT_SIZE(AmayaPathControl::OnSize)
  EVT_ENTER_WINDOW(AmayaPathControl::OnMouseEnter)
  EVT_LEAVE_WINDOW(AmayaPathControl::OnMouseLeave)
  EVT_MOTION(AmayaPathControl::OnMouseMove)
  EVT_LEFT_UP(AmayaPathControl::OnMouseLeftUp)
END_EVENT_TABLE()

#endif /* _WX */
