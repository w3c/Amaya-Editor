/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* thot_key.h - key mappings for thot keys to native system keys
 * 
 * currently supports X11 and MSWindows
 */
#define THOT_KEY_UNKNOWN	0x00	/* keys not supported by system */

/* moved from input.c because used by other files */
#define THOT_NO_MOD	0
#define THOT_MOD_CTRL	1
#define THOT_MOD_ALT	2
#define THOT_MOD_SHIFT	4
#define THOT_MOD_S_CTRL	5
#define THOT_MOD_S_ALT	6

/* thot mouse buttons identifiers */
#define THOT_NONE_BUTTON 	0
#define THOT_LEFT_BUTTON 	1
#define THOT_MIDDLE_BUTTON	2
#define THOT_RIGHT_BUTTON	3

#ifdef _WINGUI /* -- MSWindows Key names -- */
/* see winuser.h for these defs */

#define THOT_KEY_ControlMask	FCONTROL
#define THOT_KEY_ShiftMask	FSHIFT

#define THOT_KEY_Return		VK_RETURN
#define THOT_KEY_BackSpace	VK_BACK
#define THOT_KEY_Escape		VK_ESCAPE
#define THOT_KEY_Delete		VK_DELETE
#define THOT_KEY_Insert		VK_INSERT
#define THOT_KEY_Tab            VK_TAB
#define THOT_KEY_TAB            VK_TAB
#define THOT_KEY_F1		VK_F1
#define THOT_KEY_F2		VK_F2
#define THOT_KEY_F3		VK_F3
#define THOT_KEY_F4		VK_F4
#define THOT_KEY_F5		VK_F5
#define THOT_KEY_F6		VK_F6
#define THOT_KEY_F7		VK_F7
#define THOT_KEY_F8		VK_F8
#define THOT_KEY_F9		VK_F9
#define THOT_KEY_F10		VK_F10
#define THOT_KEY_F11		VK_F11
#define THOT_KEY_F12		VK_F12
#define THOT_KEY_F13		VK_F13
#define THOT_KEY_F14		VK_F14
#define THOT_KEY_F15		VK_F15
#define THOT_KEY_F16		VK_F16
#define THOT_KEY_F17		VK_F17
#define THOT_KEY_F18		VK_F18
#define THOT_KEY_F19		VK_F19
#define THOT_KEY_F20		VK_F20
#define THOT_KEY_Up		VK_UP
#define THOT_KEY_Down		VK_DOWN
#define THOT_KEY_Left		VK_LEFT
#define THOT_KEY_Right		VK_RIGHT
#define THOT_KEY_Home		VK_HOME
#define THOT_KEY_End		VK_END
#define THOT_KEY_Prior		VK_PRIOR
#define THOT_KEY_Next		VK_NEXT

#endif /* _WINGUI */

#if defined(_MOTIF) || defined(_GTK) /* -----------------------X11 key names-- */

#define THOT_KEY_ControlMask	ControlMask
#define THOT_KEY_ShiftMask	ShiftMask

#define THOT_KEY_Return		XK_Return
#define THOT_KEY_BackSpace	XK_BackSpace
#define THOT_KEY_Escape		XK_Escape
#define THOT_KEY_Delete		XK_Delete
#define THOT_KEY_Insert		XK_Insert
#define THOT_KEY_Tab            XK_Tab
#define THOT_KEY_TAB            XK_ISO_Left_Tab
#define THOT_KEY_F1		XK_F1
#define THOT_KEY_F2		XK_F2
#define THOT_KEY_F3		XK_F3
#define THOT_KEY_F4		XK_F4
#define THOT_KEY_F5		XK_F5
#define THOT_KEY_F6		XK_F6
#define THOT_KEY_F7		XK_F7
#define THOT_KEY_F8		XK_F8
#define THOT_KEY_F9		XK_F9
#define THOT_KEY_F10		XK_F10
#define THOT_KEY_F11		XK_F11
#define THOT_KEY_F12		XK_F12
#define THOT_KEY_F13		XK_F13
#define THOT_KEY_F14		XK_F14
#define THOT_KEY_F15		XK_F15
#define THOT_KEY_F16		XK_F16
#define THOT_KEY_F17		XK_F17
#define THOT_KEY_F18		XK_F18
#define THOT_KEY_F19		XK_F19
#define THOT_KEY_F20		XK_F20
#define THOT_KEY_R1		XK_R1
#define THOT_KEY_R2		XK_R2
#define THOT_KEY_R3		XK_R3
#define THOT_KEY_R4		XK_R4
#define THOT_KEY_R5		XK_R5
#define THOT_KEY_R6		XK_R6
#define THOT_KEY_R7		XK_R7
#define THOT_KEY_R8		XK_R8
#define THOT_KEY_R9		XK_R9
#define THOT_KEY_R10		XK_R10
#define THOT_KEY_R11		XK_R11
#define THOT_KEY_R12		XK_R12
#define THOT_KEY_R13		XK_R13
#define THOT_KEY_R14		XK_R14
#define THOT_KEY_R15		XK_R15
#define THOT_KEY_Up		XK_Up
#define THOT_KEY_Down		XK_Down
#define THOT_KEY_Left		XK_Left
#define THOT_KEY_Right		XK_Right
#define THOT_KEY_Home		XK_Home
#define THOT_KEY_End		XK_End
#define THOT_KEY_Prior		XK_Prior
#define THOT_KEY_Next		XK_Next

#endif /* #if defined(_MOTIF) */

#if defined(_WX)

#define THOT_KEY_ControlMask	WXK_CONTROL
#define THOT_KEY_ShiftMask	WXK_SHIFT

#define THOT_KEY_Return		WXK_RETURN
#define THOT_KEY_BackSpace	WXK_BACK
#define THOT_KEY_Escape		WXK_ESCAPE
#define THOT_KEY_Delete		WXK_DELETE
#define THOT_KEY_Insert		WXK_INSERT
#define THOT_KEY_Tab            WXK_TAB
#define THOT_KEY_TAB            WXK_TAB
#define THOT_KEY_F1		WXK_F1
#define THOT_KEY_F2		WXK_F2
#define THOT_KEY_F3		WXK_F3
#define THOT_KEY_F4		WXK_F4	
#define THOT_KEY_F5		WXK_F5
#define THOT_KEY_F6		WXK_F6
#define THOT_KEY_F7		WXK_F7
#define THOT_KEY_F8		WXK_F8
#define THOT_KEY_F9		WXK_F9
#define THOT_KEY_F10		WXK_F10
#define THOT_KEY_F11		WXK_F11
#define THOT_KEY_F12		WXK_F12
#define THOT_KEY_F13		WXK_F13
#define THOT_KEY_F14		WXK_F14
#define THOT_KEY_F15		WXK_F15
#define THOT_KEY_F16		WXK_F16
#define THOT_KEY_F17		WXK_F17
#define THOT_KEY_F18		WXK_F18
#define THOT_KEY_F19		WXK_F19
#define THOT_KEY_F20		WXK_F20
#define THOT_KEY_Up		WXK_UP
#define THOT_KEY_Down		WXK_DOWN
#define THOT_KEY_Left		WXK_LEFT
#define THOT_KEY_Right		WXK_RIGHT
#define THOT_KEY_Home		WXK_HOME
#define THOT_KEY_End		WXK_END
#define THOT_KEY_Prior		WXK_PRIOR
#define THOT_KEY_Next		WXK_NEXT

#endif /* _WX */

#if defined(_NOGUI)

#define THOT_KEY_ControlMask	0
#define THOT_KEY_ShiftMask	0

#define THOT_KEY_Return		0
#define THOT_KEY_BackSpace	1
#define THOT_KEY_Escape		2
#define THOT_KEY_Delete		3
#define THOT_KEY_Tab            4
#define THOT_KEY_TAB            5
#define THOT_KEY_F1		6
#define THOT_KEY_F2		7
#define THOT_KEY_F3		8
#define THOT_KEY_F4		9
#define THOT_KEY_F5		10
#define THOT_KEY_F6		11
#define THOT_KEY_F7		12
#define THOT_KEY_F8		13
#define THOT_KEY_F9		14
#define THOT_KEY_F10		15
#define THOT_KEY_F11		16
#define THOT_KEY_F12		17
#define THOT_KEY_F13		18
#define THOT_KEY_F14		19
#define THOT_KEY_F15		20
#define THOT_KEY_F16		21
#define THOT_KEY_F17		22
#define THOT_KEY_F18		23
#define THOT_KEY_F19		24
#define THOT_KEY_F20		25
#define THOT_KEY_R1		26
#define THOT_KEY_R2		27
#define THOT_KEY_R3		28
#define THOT_KEY_R4		29
#define THOT_KEY_R5		30
#define THOT_KEY_R6		31
#define THOT_KEY_R7		32
#define THOT_KEY_R8		33
#define THOT_KEY_R9		34
#define THOT_KEY_R10		35
#define THOT_KEY_R11		36
#define THOT_KEY_R12		37
#define THOT_KEY_R13		38
#define THOT_KEY_R14		39
#define THOT_KEY_R15		40
#define THOT_KEY_Up		41
#define THOT_KEY_Down		42
#define THOT_KEY_Left		43
#define THOT_KEY_Right		44
#define THOT_KEY_Home		45
#define THOT_KEY_End		46
#define THOT_KEY_Prior		47
#define THOT_KEY_Next		48
#define THOT_KEY_Insert		49

#endif /* _NOGUI */
