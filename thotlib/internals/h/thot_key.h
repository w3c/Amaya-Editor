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

#ifdef _WINDOWS /* -- MSWindows Key names -- */

/* see winuser.h for these defs */

#define THOT_KEY_ControlMask	FCONTROL

#define THOT_KEY_Return		VK_RETURN
#define THOT_KEY_BackSpace	VK_BACK
#define THOT_KEY_Escape		VK_ESCAPE
#define THOT_KEY_Delete		VK_DELETE
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
/*
#define THOT_KEY_R1		THOT_KEY_UNKNOWN
#define THOT_KEY_R2		THOT_KEY_UNKNOWN
#define THOT_KEY_R3		THOT_KEY_UNKNOWN
#define THOT_KEY_R4		THOT_KEY_UNKNOWN
#define THOT_KEY_R5		THOT_KEY_UNKNOWN
#define THOT_KEY_R6		THOT_KEY_UNKNOWN
#define THOT_KEY_R7		THOT_KEY_UNKNOWN
#define THOT_KEY_R8		THOT_KEY_UNKNOWN
#define THOT_KEY_R9		THOT_KEY_UNKNOWN
#define THOT_KEY_R10		THOT_KEY_UNKNOWN
#define THOT_KEY_R11		THOT_KEY_UNKNOWN
#define THOT_KEY_R12		THOT_KEY_UNKNOWN
#define THOT_KEY_R13		THOT_KEY_UNKNOWN
#define THOT_KEY_R14		THOT_KEY_UNKNOWN
#define THOT_KEY_R15		THOT_KEY_UNKNOWN
*/
#define THOT_KEY_Up		VK_UP
#define THOT_KEY_Down		VK_DOWN
#define THOT_KEY_Left		VK_LEFT
#define THOT_KEY_Right		VK_RIGHT
#define THOT_KEY_Home		VK_HOME
#define THOT_KEY_End		VK_END
#define THOT_KEY_Prior		VK_PRIOR
#define THOT_KEY_Next		VK_NEXT

#else /* -----------------------X11 key names-- */

#define THOT_KEY_ControlMask	ControlMask

#define THOT_KEY_Return		XK_Return
#define THOT_KEY_BackSpace	XK_BackSpace
#define THOT_KEY_Escape		XK_Escape
#define THOT_KEY_Delete		XK_Delete
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

#endif
