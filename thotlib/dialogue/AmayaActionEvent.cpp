/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
// Thotlib includes
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"

#include "appdialogue_f.h"

#include "AmayaActionEvent.h"

DEFINE_EVENT_TYPE(AMAYA_ACTION_EVENT)


AmayaActionEvent::AmayaActionEvent(int id, Document doc, View view, ThotBool force):
  wxCommandEvent(AMAYA_ACTION_EVENT, id),
  m_doc(doc),
  m_view(view),
  m_force(force)
{
}

AmayaActionEvent::AmayaActionEvent(const char* name, Document doc, View view, ThotBool force):
  wxCommandEvent(AMAYA_ACTION_EVENT, FindMenuAction (name)),
  m_doc(doc),
  m_view(view),
  m_force(force)
{
}

AmayaActionEvent::AmayaActionEvent(const AmayaActionEvent& event):
  wxCommandEvent(event.GetEventType(), event.GetId()),
  m_doc(event.GetDocument()),
  m_view(event.GetView()),
  m_force(event.IsForced())
{
}

AmayaActionEvent::~AmayaActionEvent()
{
}

wxEvent* AmayaActionEvent::Clone() const
{
  return new AmayaActionEvent(*this);
}

#endif /* _WX */
