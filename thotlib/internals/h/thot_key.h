/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009.
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
