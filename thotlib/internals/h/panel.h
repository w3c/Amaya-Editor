/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef __PANELCTL_H__
#define __PANELCTL_H__

#ifndef NODISPLAY

#define MAX_BUTTON  30

#if defined(_WX)

typedef struct _Panel_Ctl
{
  Proc          Call_Panel[MAX_BUTTON];     /* callbacks for XHTML panel */
  const char *  Tooltip_Panel[MAX_BUTTON];  /* tooltips for XHTML panel */
} Panel_Ctl;

#endif /* #if defined(_WX) */


#endif /* !NODISPLAY */

#endif /* __PANELCTL_H__ */
