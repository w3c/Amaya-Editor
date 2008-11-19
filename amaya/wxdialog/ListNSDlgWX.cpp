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
#include "AmayaApp.h"
#include "ListNSDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
static int MyRef = 0;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ListNSDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_CANCEL2"),         ListNSDlgWX::OnCancelButton )
  EVT_BUTTON( XRCID("wxID_DELETE_ELEM_NS"), ListNSDlgWX::OnNSDelete )
  EVT_BUTTON( XRCID("wxID_ADD_ELEM_NS"),    ListNSDlgWX::OnNSAdd )
  EVT_LISTBOX(XRCID("wxID_LIST_ELEM_NS"),   ListNSDlgWX::OnNSSelected)
  EVT_CLOSE( ListNSDlgWX::OnClose )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ListNSDlgWX 
  params:
    + parent : parent window
    + title : dialog title
  ----------------------------------------------------------------------*/
ListNSDlgWX::ListNSDlgWX( int ref, wxWindow* parent, const wxArrayString& items,
			  const wxArrayString& rdfa_items ) :

  AmayaDialog( parent, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("ListNSDlgWX"));
  MyRef = ref;

  // update dialog labels with given ones
  // XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( list_title );
  XRCCTRL(*this, "wxID_CANCEL2", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CANCEL)) );

  // fill up the ns list
  XRCCTRL(*this, "wxID_LIST_ELEM_NS", wxListBox)->Append( items );

  // fillup  the combobox with default ns list
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_ELEM_NS", wxComboBox)->Append(rdfa_items);
#if defined(_WINDOWS)
  // select the string
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_ELEM_NS", wxComboBox)->SetSelection(0, -1);
#else /* _WINDOWS */
  // set te cursor to the end
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_ELEM_NS", wxComboBox)->SetInsertionPointEnd();
#endif /* _WINDOWS */

  SetSize(720, 420);
  Fit();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor.
  ----------------------------------------------------------------------*/
ListNSDlgWX::~ListNSDlgWX()
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user wants to exit from this dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void ListNSDlgWX::OnClose( wxCloseEvent& event )
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  NSUpdate called when the user change the selection
  returns:
  ----------------------------------------------------------------------*/
void ListNSDlgWX::NSUpdate( const wxArrayString& items )
{
  // fill up the ns list
  XRCCTRL(*this, "wxID_LIST_ELEM_NS", wxListBox)->Clear( );
  XRCCTRL(*this, "wxID_LIST_ELEM_NS", wxListBox)->Append( items );
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user wants to exit from this dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void ListNSDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ListNSDlgWX::OnNSAdd(wxCommandEvent& event)
{
  int        i;
  wxString   path = XRCCTRL(*this, "wxID_COMBOBOX_NEW_ELEM_NS", wxComboBox)->GetValue();
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_ELEM_NS", wxListBox);
  char       buffer[512];

  if (path.IsEmpty())
    return;
  for (i = 0; i < (int)box->GetCount(); i++)
    {
      box->GetString(i).mb_str(*wxConvCurrent);
      if (!strcmp (path.mb_str(wxConvUTF8), box->GetString(i).mb_str(wxConvUTF8)))
        {
          // this entry already exists
          box->SetSelection(box->GetCount()-1);
          return;
        }
    }
  box->Append(path);
  box->SetSelection(box->GetCount()-1);
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_ELEM_NS", wxComboBox)->SetValue( TtaConvMessageToWX("") );

  // allocate a temporary buffer
  strcpy( buffer, path.mb_str(wxConvUTF8));
  ThotCallback (MyRef + 1, STRING_DATA, buffer);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ListNSDlgWX::OnNSDelete(wxCommandEvent& event)
{
  char     buffer[512];
  wxString selected_item = XRCCTRL(*this, "wxID_LIST_ELEM_NS", wxListBox)->GetStringSelection();

  wxListBox *box = XRCCTRL(*this, "wxID_LIST_ELEM_NS", wxListBox);
  int sel = box->GetSelection();
  if (sel != wxNOT_FOUND)
  {
    box->Delete(sel);
    wxASSERT( selected_item.Len() < 512 );
    strcpy( buffer, (const char*)selected_item.mb_str(wxConvUTF8) );
    ThotCallback (MyRef + 2, STRING_DATA, buffer);
  }
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_ELEM_NS", wxComboBox)->SetValue( TtaConvMessageToWX("") );
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ListNSDlgWX::OnNSSelected(wxCommandEvent& event)
{
  if (event.IsSelection())
    XRCCTRL(*this, "wxID_COMBOBOX_NEW_ELEM_NS", wxComboBox)->SetValue(event.GetString());
}

#endif /* _WX */
