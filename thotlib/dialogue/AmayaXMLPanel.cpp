/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
#include "registry_wx.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"
#include "containers.h"
#include "inites_f.h"
#include "presentmenu_f.h"

#include "Elemlist.h"

#include "AmayaXMLPanel.h"
#include "AmayaNormalWindow.h"

//
//
// AmayaXMLPanel
//
//


AmayaXMLPanel::AmayaXMLPanel():
  wxPanel()
,m_XMLRef(0)
,m_fnCallback(NULL)
{
}

AmayaXMLPanel::AmayaXMLPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style, const wxString& name, wxObject* extra):
  wxPanel()
,m_XMLRef(0)
,m_fnCallback(NULL)
{
  Create(parent, id, pos, size, style, name, extra);
}

AmayaXMLPanel::~AmayaXMLPanel()
{
}

bool AmayaXMLPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_XML")))
    return false;
  
  m_pXMLList = XRCCTRL(*this,"wxID_LIST_XML",wxListBox);
  XRCCTRL(*this,"wxID_REFRESH",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_REFRESH)));
  XRCCTRL(*this,"wxID_APPLY",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  return true;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaXMLPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
  -----------------------------------------------------------------------*/
void AmayaXMLPanel::SendDataToPanel( AmayaParams& p )
{
  int nb_el = (int)p.param1;
  m_fnCallback = (void*) p.param3;
  
  if (nb_el == -1)
    {
      ContainerNode node;
      ElemListElement elem;

      DLList list = (DLList) p.param2;
      DLList reflist = DLList_GetRefList(list, (Container_CompareFunction)ElemListElement_Compare);
      ForwardIterator iter = DLList_GetForwardIterator(reflist);

      m_pXMLList->Clear();    
      node = ForwardIterator_GetFirst(iter);
      while(node)
        {
          elem = (ElemListElement)node->elem;
      
// TODO removed code because not showing other than DefinedComponent
//      wxString str = TtaConvMessageToWX(ElemListElement_GetName(elem));
//      if (elem->typeClass == LanguageElement)
//        str.Prepend(TtaConvMessageToWX( TtaGetSSchemaName(elem->elem.element.type.ElSSchema)) + wxT(":"));
//      if(elem->comment)
//        str.append(wxT(" (") + TtaConvMessageToWX( elem->comment) + wxT(")"));
//      m_pXMLList->Append( str , (void*)elem);

          if (elem && elem->typeClass==DefinedComponent)
            {
              wxString str = TtaConvMessageToWX(ElemListElement_GetName(elem));
              if (elem->comment)
                str.append(wxT(" (") + TtaConvMessageToWX( elem->comment) + wxT(")"));
              m_pXMLList->Append( str , (void*)elem);
            }

          node = ForwardIterator_GetNext(iter);
        }
    
      TtaFreeMemory(iter);
      DLList_Destroy(reflist);
    }
  else
    {
      const char  *listBuffer = (char *)p.param2;
      const char  *currentEl = (char *)p.param3;
      intptr_t     ref = (intptr_t)p.param4;
  
      m_XMLRef = ref;
      /* fill the list */
      m_pXMLList->Clear();
      int i = 0;
      int index = 0;
      int sel = 0;
      if (nb_el == 0)
        return;
      while (i < nb_el && listBuffer[index] != EOS)
        {
          m_pXMLList->Append( TtaConvMessageToWX( &listBuffer[index] ) );
          if (!strcmp (&listBuffer[index], currentEl))
            /* current selected item */
            sel = i;
          index += strlen (&listBuffer[index]) + 1; /* one entry length */
          i++;
        }
      
      /* select the wanted item */
      m_pXMLList->SetSelection(sel);
      if (currentEl[0] != EOS)
        m_pXMLList->SetStringSelection(TtaConvMessageToWX(currentEl));
      
      /* recalculate layout */
      GetParent()->Layout();
      Layout();
    }
}


/*----------------------------------------------------------------------
 *       Class:  AmayaXMLPanel
 *      Method:  OnApply
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaXMLPanel::OnApply( wxCommandEvent& event )
{
  ElemListElement elem = NULL;
  
  ThotCallback(m_XMLRef, INTEGER_DATA, (char*) 1);
  if(m_pXMLList && m_pXMLList->GetSelection()!=wxNOT_FOUND)
  {
    elem = (ElemListElement)m_pXMLList->GetClientData(m_pXMLList->GetSelection());
    if(elem && m_fnCallback)
      ((ElemListElement_DoInsertElementFunction)m_fnCallback)(elem);
  }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXMLPanel
 *      Method:  OnSelected
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaXMLPanel::OnSelected( wxCommandEvent& event )
{
  wxString s_selected = XRCCTRL(*this, "wxID_LIST_XML", wxListBox)->GetStringSelection();
  
  // allocate a temporary buffer
  char buffer[512];
  wxASSERT( s_selected.Len() < 512 );
  strcpy( buffer, (const char*)s_selected.mb_str(wxConvUTF8) );
  
  // call the callback : send the selected string 
  ThotCallback(m_XMLRef+1, STRING_DATA, buffer);
}



/*----------------------------------------------------------------------
 *       Class:  AmayaXMLPanel
 *      Method:  OnRefresh
 * Description:  refresh the panel from current selection
  -----------------------------------------------------------------------*/
void AmayaXMLPanel::OnRefresh( wxCommandEvent& event )
{
  RefreshXMLPanel();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXMLPanel
 *      Method:  RefreshXMLPanel
 * Description:  refresh the panel from current selection
  -----------------------------------------------------------------------*/
void AmayaXMLPanel::RefreshXMLPanel()
{
  Document doc;
  View view;
  TtaGetActiveView( &doc, &view );
  if (doc > 0)
    TtaRefreshElementMenu (doc, view);
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaXMLPanel, wxPanel)
  EVT_BUTTON( XRCID("wxID_APPLY"), AmayaXMLPanel::OnApply )
  EVT_BUTTON( XRCID("wxID_REFRESH"), AmayaXMLPanel::OnRefresh )
  EVT_LISTBOX( XRCID("wxID_LIST_XML"), AmayaXMLPanel::OnSelected ) 
  EVT_LISTBOX_DCLICK( XRCID("wxID_LIST_XML"), AmayaXMLPanel::OnApply )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
