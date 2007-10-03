#ifdef _WX

#ifndef LISTBOXBOOK_H_
#define LISTBOXBOOK_H_

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "wx/notebook.h"
#include "wx/listbook.h"
#include "wx/listbox.h"

DECLARE_EVENT_TYPE(wxEVT_COMMAND_LISTBOXBOOK_PAGE_CHANGED, wxID_ANY)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_LISTBOXBOOK_PAGE_CHANGING, wxID_ANY)



// ----------------------------------------------------------------------------
// wxListBoxbook
// ----------------------------------------------------------------------------
class wxListBoxBook : public wxBookCtrlBase
{
public:
  wxListBoxBook()
    {
        Init();
    }

  wxListBoxBook(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxEmptyString)
    {
        Init();

        (void)Create(parent, id, pos, size, style, name);
    }

    // quasi ctor
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxEmptyString);


    // overridden base class methods
    virtual int GetSelection() const;
    virtual bool SetPageText(size_t n, const wxString& strText);
    virtual wxString GetPageText(size_t n) const;
    virtual int GetPageImage(size_t n) const;
    virtual bool SetPageImage(size_t n, int imageId);
    virtual wxSize CalcSizeFromPage(const wxSize& sizePage) const;
    virtual bool InsertPage(size_t n,
                            wxWindow *page,
                            const wxString& text,
                            bool bSelect = false,
                            int imageId = -1);
    virtual int SetSelection(size_t n);
    virtual int ChangeSelection(size_t n) { return DoSetSelection(n); }
    virtual void SetImageList(wxImageList *imageList);

    virtual bool DeleteAllPages();

    wxListBox* GetListBox() const { return (wxListBox*)m_bookctrl; }

protected:
    virtual wxWindow *DoRemovePage(size_t page);

    // get the size which the list control should have
    virtual wxSize GetControllerSize() const;

    void UpdateSelectedPage(size_t newsel);

    wxBookCtrlBaseEvent* CreatePageChangingEvent() const;
    void MakeChangedEvent(wxBookCtrlBaseEvent &event);

    // event handlers
    void OnListSelected(wxCommandEvent& event);

    // the currently selected page or wxNOT_FOUND if none
    int m_selection;

private:
    // common part of all constructors
    void Init();

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxListBoxBook)
};


// ----------------------------------------------------------------------------
// listboxbook event class and related stuff
// ----------------------------------------------------------------------------

class wxListBoxBookEvent : public wxBookCtrlBaseEvent
{
public:
  wxListBoxBookEvent(wxEventType commandType = wxEVT_NULL, int id = 0,
                    int nSel = wxNOT_FOUND, int nOldSel = wxNOT_FOUND)
        : wxBookCtrlBaseEvent(commandType, id, nSel, nOldSel)
    {
    }

  wxListBoxBookEvent(const wxListbookEvent& event)
        : wxBookCtrlBaseEvent(event)
    {
    }

    virtual wxEvent *Clone() const { return new wxListBoxBookEvent(*this); }

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxListBoxBookEvent)
};

typedef void (wxEvtHandler::*wxListBoxBookEventFunction)(wxListBoxBookEvent&);

#define wxListBoxBookEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxListBoxBookEventFunction, &func)

#define EVT_LISTBOXBOOK_PAGE_CHANGED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_LISTBOXBOOK_PAGE_CHANGED, winid, wxListBoxBookEventHandler(fn))

#define EVT_LISTBOXBOOK_PAGE_CHANGING(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_LISTBOXBOOK_PAGE_CHANGING, winid, wxListBoxBookEventHandler(fn))


#endif /*LISTBOXBOOK_H_*/

#endif /* _WX */
