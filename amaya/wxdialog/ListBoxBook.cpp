/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"
#include "ListBoxBook.h"
#include "wx/event.h"

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxListBoxBook, wxBookCtrlBase)
IMPLEMENT_DYNAMIC_CLASS(wxListBoxBookEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_LISTBOXBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_LISTBOXBOOK_PAGE_CHANGING)

BEGIN_EVENT_TABLE(wxListBoxBook, wxBookCtrlBase)
 EVT_LISTBOX(wxID_ANY, wxListBoxBook::OnListSelected)
END_EVENT_TABLE()

// ============================================================================
// wxListBoxBook implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxListBoxBook creation
// ----------------------------------------------------------------------------

void wxListBoxBook::Init()
{
    m_selection = wxNOT_FOUND;
}

bool
wxListBoxBook::Create(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style|wxBK_LEFT,
                            wxDefaultValidator, name) )
        return false;

    m_bookctrl = new wxListBox (this, wxID_ANY,
                    wxDefaultPosition, wxSize(100,160),
                    0, NULL, wxLB_SINGLE);

#ifdef __WXMSW__
    // On XP with themes enabled the GetViewRect used in GetControllerSize() to
    // determine the space needed for the list view will incorrectly return
    // (0,0,0,0) the first time.  So send a pending event so OnSize will be
    // called again after the window is ready to go.  Technically we don't
    // need to do this on non-XP windows, but if things are already sized
    // correctly then nothing changes and so there is no harm.
    wxSizeEvent evt;
    GetEventHandler()->AddPendingEvent(evt);
#endif
    return true;
}

// ----------------------------------------------------------------------------
// wxListBoxBook geometry management
// ----------------------------------------------------------------------------

wxSize wxListBoxBook::GetControllerSize() const
{
    const wxSize sizeClient = GetClientSize(),
                 sizeBorder = m_bookctrl->GetSize() - m_bookctrl->GetClientSize(),
                 sizeList = GetListBox()->GetSize() + sizeBorder;

    wxSize size;

    if ( IsVertical() )
    {
        size.x = sizeClient.x;
        size.y = sizeList.y;
    }
    else // left/right aligned
    {
        size.x = sizeList.x;
        size.y = sizeClient.y;
    }

    return size;
}

wxSize wxListBoxBook::CalcSizeFromPage(const wxSize& sizePage) const
{
    // we need to add the size of the list control and the border between
    const wxSize sizeList = GetControllerSize();

    wxSize size = sizePage;
    if ( IsVertical() )
    {
        size.y += sizeList.y + GetInternalBorder();
    }
    else // left/right aligned
    {
        size.x += sizeList.x + GetInternalBorder();
    }

    return size;
}

// ----------------------------------------------------------------------------
// accessing the pages
// ----------------------------------------------------------------------------

bool wxListBoxBook::SetPageText(size_t n, const wxString& strText)
{
    Freeze();
    GetListBox()->SetString(n, strText);
    Thaw();
    return true;
}

wxString wxListBoxBook::GetPageText(size_t n) const
{
    return GetListBox()->GetString(n);
}

int wxListBoxBook::GetPageImage(size_t WXUNUSED(n)) const
{
    wxFAIL_MSG( _T("wxListBoxBook do not support images") );

    return wxNOT_FOUND;
}

bool wxListBoxBook::SetPageImage(size_t n, int imageId)
{
    wxFAIL_MSG( _T("wxListBoxBook do not support images") );
    return false;
}

// ----------------------------------------------------------------------------
// image list stuff
// ----------------------------------------------------------------------------

void wxListBoxBook::SetImageList(wxImageList *WXUNUSED(imageList))
{
  wxFAIL_MSG( _T("wxListBoxBook do not support images") );
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------


int wxListBoxBook::SetSelection(size_t n)
{
  if(n<GetPageCount())
    return DoSetSelection(n, SetSelection_SendEvent);
  else if(GetPageCount() == 0)
    return DoSetSelection(0, SetSelection_SendEvent);
  else
    return DoSetSelection(GetPageCount()-1, SetSelection_SendEvent);
}

void wxListBoxBook::UpdateSelectedPage(size_t newsel)
{
    m_selection = newsel;
    GetListBox()->SetSelection(newsel);
}

int wxListBoxBook::GetSelection() const
{
    return m_selection;
}

wxBookCtrlBaseEvent* wxListBoxBook::CreatePageChangingEvent() const
{
  return new wxListBoxBookEvent(wxEVT_COMMAND_LISTBOXBOOK_PAGE_CHANGING, m_windowId);
}

void wxListBoxBook::MakeChangedEvent(wxBookCtrlBaseEvent &event)
{
  event.SetEventType(wxEVT_COMMAND_LISTBOXBOOK_PAGE_CHANGED);
}

// ----------------------------------------------------------------------------
// adding/removing the pages
// ----------------------------------------------------------------------------

bool
wxListBoxBook::InsertPage(size_t n,
                       wxWindow *page,
                       const wxString& text,
                       bool bSelect,
                       int imageId)
{
    if ( !wxBookCtrlBase::InsertPage(n, page, text, bSelect, imageId) )
        return false;

    GetListBox()->Insert(text, n);

    // if the inserted page is before the selected one, we must update the
    // index of the selected page
    if ( int(n) <= m_selection )
    {
        // one extra page added
        m_selection++;
        GetListBox()->SetSelection(m_selection);
    }

    // some page should be selected: either this one or the first one if there
    // is still no selection
    int selNew = -1;
    if ( bSelect )
        selNew = n;
    else if ( m_selection == -1 )
        selNew = 0;

    if ( selNew != m_selection )
        page->Hide();

    if ( selNew != -1 )
        SetSelection(selNew);

    if (GetPageCount() == 1)
    {
        wxSizeEvent sz(GetSize(), GetId());
        ProcessEvent(sz);
    }
    return true;
}

wxWindow *wxListBoxBook::DoRemovePage(size_t page)
{
    const size_t page_count = GetPageCount();
    wxWindow *win = wxBookCtrlBase::DoRemovePage(page);

    if ( win )
    {
        GetListBox()->Delete(page);

        if (m_selection >= (int)page)
        {
            // force new sel valid if possible
            int sel = m_selection - 1;
            if (page_count == 1)
                sel = wxNOT_FOUND;
            else if ((page_count == 2) || (sel == -1))
                sel = 0;

            // force sel invalid if deleting current page - don't try to hide it
            m_selection = (m_selection == (int)page) ? wxNOT_FOUND : m_selection - 1;

            if ((sel != wxNOT_FOUND) && (sel != m_selection))
                SetSelection(sel);
        }

        if (GetPageCount() == 0)
        {
            wxSizeEvent sz(GetSize(), GetId());
            ProcessEvent(sz);
        }
    }

    return win;
}


bool wxListBoxBook::DeleteAllPages()
{
    GetListBox()->Clear();
    if (!wxBookCtrlBase::DeleteAllPages())
        return false;

    m_selection = -1;

    wxSizeEvent sz(GetSize(), GetId());
    ProcessEvent(sz);

    return true;
}

// ----------------------------------------------------------------------------
// wxListBoxBook events
// ----------------------------------------------------------------------------

void wxListBoxBook::OnListSelected(wxCommandEvent& event)
{
    if ( event.GetEventObject() != m_bookctrl )
    {
        event.Skip();
        return;
    }

    const int selNew = event.GetInt();

    if ( selNew == m_selection )
    {
        // this event can only come from our own Select(m_selection) below
        // which we call when the page change is vetoed, so we should simply
        // ignore it
        return;
    }

    SetSelection(selNew);

    // change wasn't allowed, return to previous state
    if (m_selection != selNew)
    {
        GetListBox()->SetSelection(m_selection);
    }
}


#endif /* _WX */
