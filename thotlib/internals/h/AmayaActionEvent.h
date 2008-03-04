#ifdef _WX
#ifndef AMAYAACTIONEVENT_H_
#define AMAYAACTIONEVENT_H_

#include "wx/event.h"

DECLARE_EVENT_TYPE(AMAYA_ACTION_EVENT, wxID_ANY)

/**
 * Menu action event
 **/ 
class AmayaActionEvent : public wxCommandEvent
{
protected:
  Document m_doc;
  View     m_view;
  ThotBool m_force;
public:
  AmayaActionEvent(int id, Document doc, View view, ThotBool force);
  AmayaActionEvent(const char* name, Document doc, View view, ThotBool force);
  
  AmayaActionEvent(const AmayaActionEvent& event);
  virtual ~AmayaActionEvent();

  virtual wxEvent* Clone() const;
  
  Document GetDocument()const{return m_doc;}
  View     GetView()const{return m_view;}
  ThotBool IsForced()const{return m_force;}
  
};



typedef void (wxEvtHandler::*AmayaActionEventFunction)(AmayaActionEvent&);

#define AmayaActionEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(AmayaActionEventFunction, &func)

#define EVT_AMAYA_ACTION(id, fn) wx__DECLARE_EVT1(AMAYA_ACTION_EVENT, id, AmayaActionEventHandler(fn))
#define EVT_AMAYA_ALL_ACTION(fn) wx__DECLARE_EVT1(AMAYA_ACTION_EVENT, wxID_ANY, AmayaActionEventHandler(fn))



#endif /*AMAYAACTIONEVENT_H_*/
#endif /* #ifdef _WX */
