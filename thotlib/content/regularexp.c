/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
   This module processes regular expressions.
 */

#ifdef RS
#pragma alloca
#endif

#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"

#include "memory_f.h"
#include "structselect_f.h"
#include "tree_f.h"

/* Define number of error messages */
#define Invalid_pattern 1
#define Unmatched_leftparen 2
#define Unmatched_rightparen 3
#define Premature_end_of_regular_expression 4
#define Nesting_too_deep 5
#define Regular_expression_too_big 6
#define Memory_exhausted 7

/* Define number of parens for which we record the beginnings and ends.
   This affects how much space the `struct re_registers' type takes up.  */
#define RE_NREGS 10

#define NFAILURES 80
#define BYTEWIDTH 8

/* This data structure is used to represent a compiled pattern. */
struct re_pattern_buffer
{
  char               *buffer;       /* Space holding the compiled pattern commands. */
  int                 allocated;    /* Size of space that  buffer  points to */
  int                 used;	/* Length of portion of buffer actually occupied */
  char               *fastmap;      /* Pointer to fastmap, if any, or zero if none. */
  /* re_search uses the fastmap, if there is one, to
     skip quickly over totally implausible characters */
  char               *translate;    /* Translate table to apply to all characters
				       before comparing.
				       Or zero for no translation.
				       The translation is applied to a pattern when it is
				       compiled and to data when it is matched. */
  char                fastmap_accurate;
  /* Set to zero when a new pattern is stored,
     set to one when the fastmap is updated from it. */
  char                can_be_null;  /* Set to one by compiling fastmap
				       if this pattern might match the null string.
				       It does not necessarily match the null string
				       in that case, but if this is zero, it cannot.
				       2 as value means can match null string
				       but at end of range or before a character
				       listed in the fastmap.  */
};


/* Structure to store "register" contents data in.
   Pass the address of such a structure as an argument to re_match, etc.,
   if you want this information back.
   
   start[i] and end[i] record the string matched by \( ... \) grouping i,
   for i from 1 to RE_NREGS - 1.
   start[0] and end[0] record the entire string matched. */

struct re_registers
{
  int                 start[RE_NREGS];
  int                 end[RE_NREGS];
};

struct t_stack
{
  unsigned char      *pchar;
  PtrElement          pEl;
  int                 index;
};


/* These are the command codes that appear in compiled regular expressions,
   one per byte.
   Some command codes are followed by argument bytes.
   A command code can specify any interpretation whatever for its arguments.
   Zero-bytes may appear in the compiled regular expression. */

enum regexpcode
{
  unused,
  exactn,			/* followed by one byte giving n, and then by n literal bytes */
  begline,			/* fails unless at beginning of line */
  endline,			/* fails unless at end of line */
  jump,			/* followed by two bytes giving relative address to jump to */
  on_failure_jump,		/* followed by two bytes giving relative address of
				   place to resume at in case of failure. */
  finalize_jump,		/* Throw away latest failure point and then jump to
				   address. */
  maybe_finalize_jump,	/* Like jump but finalize if safe to do so.
			   This is used to jump back to the beginning
			   of a repeat.  If the command that follows
			   this jump is clearly incompatible with the
			   one at the beginning of the repeat, such that
			   we can be sure that there is no use backtracking
			   out of repetitions already completed,
			   then we finalize. */
  dummy_failure_jump,	/* jump, and push a dummy failure point.
			   This failure point will be thrown away
			   if an attempt is made to use it for a failure.
			   A + construct makes this before the first repeat */
  anychar,		   /* matches any one character */
  charset,		   /* matches any one char belonging to specified set.
			      First following byte is # bitmap bytes.
			      Then come bytes for a bit-map saying which chars are in.
			      Bits in each byte are ordered low-bit-first.
			      A character is in the set if its bit is 1.
			      A character too large to have a bit in the map
			      is automatically not in the set */
  charset_not,		/* similar but match any character that is NOT one of those
			   specified */
  start_memory,		/* starts remembering the text that is matched
			   and stores it in a memory register.
			   followed by one byte containing the register number.
			   Register numbers must be in the range 0 through NREGS. */
  stop_memory,		/* stops remembering the text that is matched
			   and stores it in a memory register.
			   followed by one byte containing the register number.
			   Register numbers must be in the range 0 through NREGS. */
  duplicate,			/* match a duplicate of something remembered.
				   Followed by one byte containing the index of the memory
				   register. */
  before_dot,		/* Succeeds if before dot */
  at_dot,			/* Succeeds if at dot */
  after_dot,			/* Succeeds if after dot */
  begbuf,			/* Succeeds if at beginning of buffer */
  endbuf,			/* Succeeds if at end of buffer */
  wordchar,			/* Matches any word-constituent character */
  notwordchar,		/* Matches any char that is not a word-constituent */
  wordbeg,			/* Succeeds if at word beginning */
  wordend,			/* Succeeds if at word end */
  wordbound,			/* Succeeds if at a word boundary */
  notwordbound,		/* Succeeds if not at a word boundary */
  syntaxspec,		/* Matches any character whose syntax is specified.
			   followed by a byte which contains a syntax code, Sword or
			   such like */
  notsyntaxspec		/* Matches any character whose syntax differs from the
			   specified. */
};


/* A syntax table is a Lisp vector of length 0400, whose elements are integers.
   The low 8 bits of the integer is a code, as follows: */

enum syntaxcode
{
  Sspace,			/* for a separator character */
  Swhitespace,		/* for a whitespace character */
  Spunct,			/* for random punctuation characters */
  Sword,			/* for a word constituent */
  Ssymbol,			/* symbol constituent but not word constituent */
  Sopen,			/* for a beginning delimiter */
  Sclose,			/* for an ending delimiter */
  Squote,			/* for a prefix character like Lisp ' */
  Sstring,			/* for a string-grouping character like Lisp " */
  Sinter,			/* for a high punctuation ? or ! */
  Sescape,			/* for a character that begins a C-style escape */
  Scharquote,		/* for a character that quotes the following character */
  Scomment,			/* for a comment-starting character */
  Sendcomment,		/* for a comment-ending character */
  Smax			/* Upper bound on codes that are meaningful */
};


/* Indexed by a character, gives the upper case equivalent of the character */

static unsigned char upcase[0400] =
{000, 001, 002, 003, 004, 005, 006, 007, 010, 011,
 012, 013, 014, 015, 016, 017, 020, 021, 022, 023,
 024, 025, 026, 027, 030, 031, 032, 033, 034, 035,
 036, 037, 040, 041, 042, 043, 044, 045, 046, 047,
 050, 051, 052, 053, 054, 055, 056, 057, 060, 061,
 062, 063, 064, 065, 066, 067, 070, 071, 072, 073,
 074, 075, 076, 077, 0100, 0101, 0102, 0103, 0104, 0105,
 0106, 0107, 0110, 0111, 0112, 0113, 0114, 0115, 0116, 0117,
 0120, 0121, 0122, 0123, 0124, 0125, 0126, 0127, 0130, 0131,
 0132, 0133, 0134, 0135, 0136, 0137, 0140, 0101, 0102, 0103,
 0104, 0105, 0106, 0107, 0110, 0111, 0112, 0113, 0114, 0115,
 0116, 0117, 0120, 0121, 0122, 0123, 0124, 0125, 0126, 0127,
 0130, 0131, 0132, 0173, 0174, 0175, 0176, 0177, 0200, 0201,
 0202, 0203, 0204, 0205, 0206, 0207, 0210, 0211, 0212, 0213,
 0214, 0215, 0216, 0217, 0220, 0221, 0222, 0223, 0224, 0225,
 0226, 0227, 0231, 0231, 0232, 0233, 0234, 0235, 0236, 0237,
 0240, 0241, 0242, 0243, 0244, 0245, 0246, 0247, 0250, 0251,
 0252, 0253, 0254, 0255, 0256, 0257, 0260, 0261, 0262, 0263,
 0264, 0265, 0266, 0267, 0270, 0271, 0272, 0273, 0274, 0275,
 0276, 0277, 0300, 0301, 0302, 0303, 0304, 0305, 0306, 0307,
 0310, 0311, 0312, 0313, 0314, 0315, 0316, 0317, 0320, 0321,
 0322, 0323, 0324, 0325, 0326, 0327, 0330, 0331, 0332, 0333,
 0334, 0335, 0336, 0337, 0300, 0301, 0302, 0303, 0304, 0305,
 0306, 0307, 0310, 0311, 0312, 0313, 0314, 0315, 0316, 0317,
 0360, 0321, 0322, 0323, 0324, 0325, 0326, 0367, 0330, 0331,
 0332, 0333, 0334, 0335, 0336, 0377
};

/* Convert a letter which signifies a syntax code into the code it signifies.
   This is used by modify-syntax-entry, and other things. */

unsigned char       syntax_spec_code[0400] =
{0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 (char) Swhitespace, (char) Sinter, (char) Sstring, 0377,
 0377, 0377, 0377, (char) Squote,
 (char) Sopen, (char) Sclose, 0377, 0377,
 0377, (char) Sspace, (char) Spunct, (char) Scharquote,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377,
 (char) Scomment, 0377, (char) Sendcomment, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,	/* @, A, ... */
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, (char) Sword,
 0377, 0377, 0377, 0377, (char) Sescape, 0377, 0377, (char) Ssymbol,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,	/* `, a, ... */
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, (char) Sword,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,

 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377,
 0377, 0377, 0377, 0377, 0377, 0377, 0377, 0377

};

/* Indexed by syntax code, give the letter that describes it. */

char                syntax_code_spec[14] =
{
   '-', ' ', '.', 'w', '_', '(', ')', '\'', '\"', '!', '\\', '/', '<', '>'
};

#ifndef SYSV
#ifndef SONY
#ifndef SIMPLE
#endif
#endif
#endif
#ifdef RISC
#ifndef SIMPLE
#define alloca(x) __builtin_alloca(x)
#endif
#else
#ifdef SYSV
#define STACK_DIRECTION -1
#else
#ifdef RS
#endif
#endif
#endif

#ifdef _WINGUI
#define alloca(x) _alloca(x)
#endif

 /* messages identification */
static char         MAJiso[] =
{'\300', '\302', '\306', '\307', '\310', '\311', '\312', '\313', '\316', '\317', '\324', '\327', '\331', '\333', '\334'};
static char         miniso[] =
{'\340', '\342', '\346', '\347', '\350', '\351', '\352', '\353', '\356', '\357', '\364', '\367', '\371', '\373', '\374'};

#define SYNTAX(c) re_syntax_table[c]
static int          re_syntax_table[256];

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void         InitSyntaxTable ()
{
   register int        c, i;
   int                 lg;

   for (c = 'a'; c <= 'z'; c++)
      re_syntax_table[c] = Sword;
   for (c = 'A'; c <= 'Z'; c++)
      re_syntax_table[c] = Sword;
   for (c = '0'; c <= '9'; c++)
      re_syntax_table[c] = Sword;


/*
   re_syntax_table['-'] = Sword; 
 */
   /* accented UPPERCASE characters */
   lg = strlen (MAJiso);
   for (i = 0; i < lg; i++)
      re_syntax_table[(int) MAJiso[i]] = Sword;

   /* accented lowercase characters */
   lg = strlen (MAJiso);
   for (i = 0; i < lg; i++)
      re_syntax_table[(int) miniso[i]] = Sword;

   /* opening delimiters */
   re_syntax_table['('] = Sopen;
   re_syntax_table['['] = Sopen;
   re_syntax_table['{'] = Sopen;
   /* closing delimiters */
   re_syntax_table[')'] = Sclose;
   re_syntax_table[']'] = Sclose;
   re_syntax_table['}'] = Sclose;
   /* spaces */
   re_syntax_table[' '] = Swhitespace;	/* space = \40 */
   re_syntax_table[160] = Swhitespace;	/* ctrl space = \240 */
   re_syntax_table[129] = Swhitespace;	/* fine = 1/4 em = \201 */
   re_syntax_table[130] = Swhitespace;	/* 1/2 em = \202 */
   re_syntax_table[138] = Swhitespace;	/* ctrl Return = \212 */
   /* la ponctuation */
   re_syntax_table['.'] = Spunct;
   re_syntax_table[';'] = Spunct;
   re_syntax_table[','] = Spunct;
   re_syntax_table[':'] = Spunct;
   /* la ponctuation haute */
   re_syntax_table['!'] = Sinter;
   re_syntax_table['?'] = Sinter;
   /* debut de citation */
   re_syntax_table[171] = Sopen;	/* opening french guillemet = \253 */
   /* fin de citation */
   re_syntax_table[187] = Sclose;	/* closing french guillemet = \273 */



}

#define SIGN_EXTEND_CHAR(x) (x)

#define PATPUSH(ch) (*b++ = (char) (ch))

#define PATFETCH(c) \
{if (p == pend) goto end_of_pattern; \
   c = * (unsigned char *) p++; \
     if (translate) c = translate[c]; }

#define PATFETCH_RAW(c) \
{if (p == pend) goto end_of_pattern; \
   c = * (unsigned char *) p++; }

#define PATUNFETCH p--

#define EXTEND_BUFFER \
{ char *old_buffer = bufp->buffer; \
    if (bufp->allocated == (1<<16)) goto too_big; \
      bufp->allocated *= 2; \
	if (bufp->allocated > (1<<16)) bufp->allocated = (1<<16); \
    if (!(bufp->buffer = (char *) realloc (bufp->buffer, bufp->allocated))) \
      goto memory_exhausted; \
    c = bufp->buffer - old_buffer; \
      b += c; \
	if (fixup_jump) \
	  fixup_jump += c; \
    if (laststart) \
      laststart += c; \
	begalt += c; \
    if (pending_exact) \
      pending_exact += c; \
      }

/*----------------------------------------------------------------------
   Store where `from' points a jump operation to jump to where `to'   
   points. `opcode' is the opcode to store.                         
  ----------------------------------------------------------------------*/
static void store_jump (char *from, char opcode, char *to)
{
   from[0] = opcode;
   from[1] = (to - (from + 3)) & 0377;
   from[2] = (to - (from + 3)) >> 8;
}

/*----------------------------------------------------------------------
   Open up space at char FROM, and insert there a jump to TO.         
   CURRENT_END gives te end of the storage no in use,              
   so we know how much data to copy up.                            
   OP is the opcode of the jump to insert.                         
   If you call this function, you must zero out pending_exact.     
  ----------------------------------------------------------------------*/
static void insert_jump (char op, char *from, char *to, char *current_end)
{
   register char      *pto = current_end + 3;
   register char      *pfrom = current_end;

   while (pfrom != from)
      *--pto = *--pfrom;
   store_jump (from, op, to);
}

/*----------------------------------------------------------------------
   ReCompilePattern takes a regular-expression string and converts    
   it into a buffer full of byte commands for matching.             
   if the regular-expression is correctly compiled, it returns 0;   
   otherwise, it returns a number of an error message.              
  ----------------------------------------------------------------------*/
static int ReCompilePattern (char *pattern, int size, struct re_pattern_buffer *bufp)
{
   register char      *b = bufp->buffer;
   register char      *p = pattern;
   char               *pend = pattern + size;
   register unsigned   c, c1;
   char               *p1;
   unsigned char      *translate = (unsigned char *) bufp->translate;
   static int          done = 0;

   /* address of the count-byte of the most recently inserted "exactn" command.
      This makes it possible to tell whether a new exact-match character
      can be added to that command or requires a new "exactn" command. */

   char               *pending_exact = 0;

   /* address of the place where a forward-jump should go
      to the end of the containing expression.
      Each alternative of an "or", except the last, ends with a forward-jump
      of this sort. */

   char               *fixup_jump = 0;

   /* address of start of the most recently finished expression.
      This tells postfix * where to find the start of its operand. */

   char               *laststart = 0;

   /* In processing a repeat, 1 means zero matches is allowed */

   char                zero_times_ok;

   /* In processing a repeat, 1 means many matches is allowed */

   char                many_times_ok;

   /* address of beginning of regexp, or inside of last \( */

   char               *begalt = b;

   /* Stack of information saved by \( and restored by \). */

   int                 stackb[40];
   int                *stackp = stackb;
   int                *stacke = stackb + 40;
   int                *stackt;

   /* Counts \('s as they are encountered.  Remembered for the matching \),
      where it becomes the "register number" to put in the stop_memory command */


   int                 regnum = 1;

   if (done != 1)
     {
	InitSyntaxTable ();
	done = 1;
     }

   bufp->fastmap_accurate = 0;

   if (bufp->allocated == 0)
     {
	bufp->allocated = 28;
	if (bufp->buffer)
	   /* EXTEND_BUFFER loses when bufp->allocated is 0 */
	   bufp->buffer = (char *) realloc (bufp->buffer, 28);
	else
	   /* Caller did not allocate a buffer.  Do it for him */
	   bufp->buffer = (char *) TtaGetMemory (28);
	if (!bufp->buffer)
	   goto memory_exhausted;
	begalt = b = bufp->buffer;
     }

   while (p != pend)
     {
	if (b - bufp->buffer > bufp->allocated - 10)
	   /* Note that EXTEND_BUFFER clobbers c */
	   EXTEND_BUFFER;

	PATFETCH (c);

	switch (c)
	      {
		    /*    case '$':

		       $ means succeed if at end of line, but only in special contexts.
		       If randomly in the middle of a pattern, it is a normal character.
		       if (p == pend || *p == '\n' 
		       || (*p == '\\' && p[1] == ')') 
		       || (*p == '\\' && p[1] == '|'))
		       {
		       PATPUSH (endline);
		       break;
		       }
		       goto normal_char;
		     */

		    /*  case '^':
		       ^ means succeed if at beg of line, but only if no preceding pattern.

		       if (laststart && p[-2] != '\n')
		       {
		       goto normal_char;
		       }

		       PATPUSH (begline);
		       break;
		     */
		 case '+':
		 case '?':
		 case '*':
		    /* If there is no previous pattern, char not special. */
		    if (!laststart)
		       goto normal_char;
		    /* If there is a sequence of repetition chars,
		       collapse it down to equivalent to just one.  */
		    zero_times_ok = 0;
		    many_times_ok = 0;
		    while (1)
		      {
			 zero_times_ok |= c != '+';
			 many_times_ok |= c != '?';
			 if (p == pend)
			    break;
			 PATFETCH (c);
			 if (c == '*')
			    ;
			 else if (c == '+' || c == '?')
			    ;
			 else
			   {
			      PATUNFETCH;
			      break;
			   }
		      }

		    /* Star, etc. applied to an empty pattern is equivalent
		       to an empty pattern.  */
		    if (!laststart)
		       break;

		    /* Now we know whether 0 matches is allowed,
		       and whether 2 or more matches is allowed.  */
		    if (many_times_ok)
		      {
			 /* If more than one repetition is allowed,
			    put in a backward jump at the end.  */
			 store_jump (b, maybe_finalize_jump, laststart - 3);
			 b += 3;
		      }
		    insert_jump (on_failure_jump, laststart, b + 3, b);
		    pending_exact = 0;
		    b += 3;
		    if (!zero_times_ok)
		      {
			 /* At least one repetition required: insert before the loop
			    a skip over the initial on-failure-jump instruction */

			 insert_jump (dummy_failure_jump, laststart, laststart + 6, b);
			 b += 3;
		      }
		    break;

		 case '.':
		    laststart = b;
		    PATPUSH (anychar);
		    break;

		 case '[':
		    while (b - bufp->buffer
		       > bufp->allocated - 3 - (1 << BYTEWIDTH) / BYTEWIDTH)
		       /* Note that EXTEND_BUFFER clobbers c */
		       EXTEND_BUFFER;

		    laststart = b;
		    if (*p == '^')
		       PATPUSH (charset_not), p++;
		    else
		       PATPUSH (charset);
		    p1 = p;

		    PATPUSH ((1 << BYTEWIDTH) / BYTEWIDTH);

		    /* Clear the whole map */
		    memset (b, 0, (1 << BYTEWIDTH) / BYTEWIDTH);

		    /* Read in characters and ranges, setting map bits */
		    while (1)
		      {
			 PATFETCH (c);
			 if (c == ']' && p != p1 + 1)
			    break;
			 if (*p == '-' && p[1] != ']')
			   {
			      PATFETCH (c1);
			      PATFETCH (c1);
			      while (c <= c1)
				 b[c / BYTEWIDTH] |= 1 << (c % BYTEWIDTH), c++;
			   }
			 else
			   {
			      b[c / BYTEWIDTH] |= 1 << (c % BYTEWIDTH);
			   }
		      }
		    /* Discard any bitmap bytes that are all 0 at the end of the map.
		       Decrement the map-length byte too. */
		    while ((int) b[-1] > 0 && b[b[-1] - 1] == 0)
		       b[-1]--;
		    b += b[-1];
		    break;

		 case '\\':
		    if (p == pend)
		       goto invalid_pattern;
		    PATFETCH_RAW (c);
		    switch (c)
			  {
			     case '(':
				if (stackp == stacke)
				   goto nesting_too_deep;
				if (regnum < RE_NREGS)
				  {
				     PATPUSH (start_memory);
				     PATPUSH (regnum);
				  }
				*stackp++ = b - bufp->buffer;
				*stackp++ = fixup_jump ? fixup_jump - bufp->buffer + 1 : 0;
				*stackp++ = regnum++;
				*stackp++ = begalt - bufp->buffer;
				fixup_jump = 0;
				laststart = 0;
				begalt = b;
				break;

			     case ')':
				if (stackp == stackb)
				   goto unmatched_close;
				begalt = *--stackp + bufp->buffer;
				if (fixup_jump)
				   store_jump (fixup_jump, jump, b);
				if (stackp[-1] < RE_NREGS)
				  {
				     PATPUSH (stop_memory);
				     PATPUSH (stackp[-1]);
				  }
				stackp -= 2;
				fixup_jump = 0;
				if (*stackp)
				   fixup_jump = *stackp + bufp->buffer - 1;
				laststart = *--stackp + bufp->buffer;
				break;

			     case '|':
				insert_jump (on_failure_jump, begalt, b + 6, b);
				pending_exact = 0;
				b += 3;
				if (fixup_jump)
				   store_jump (fixup_jump, jump, b);
				fixup_jump = b;
				b += 3;
				laststart = 0;
				begalt = b;
				break;

/**/
	    case '=':
	      PATPUSH (at_dot);
	      break;

	    case 's':	
	      laststart = b;
	      PATPUSH (syntaxspec);
	      PATFETCH (c);
	      PATPUSH (syntax_spec_code[c]);
	      break;
	      
	    case 'S':
	      laststart = b;
	      PATPUSH (notsyntaxspec);
	      PATFETCH (c);
	      PATPUSH (syntax_spec_code[c]);
	      break;
/**/
			     case 'w':
				laststart = b;
				PATPUSH (wordchar);
				break;

			     case 'W':
				laststart = b;
				PATPUSH (notwordchar);
				break;

			     case '<':
				PATPUSH (wordbeg);
				break;

			     case '>':
				PATPUSH (wordend);
				break;

			     case 'b':
				PATPUSH (wordbound);
				break;

			     case 'B':
				PATPUSH (notwordbound);
				break;

			     case '1':
			     case '2':
			     case '3':
			     case '4':
			     case '5':
			     case '6':
			     case '7':
			     case '8':
			     case '9':
				c1 = c - '0';
				if (c1 >= regnum)
				   goto normal_char;
				for (stackt = stackp - 2; stackt > stackb; stackt -= 4)
				   if (*stackt == c1)
				      goto normal_char;
				laststart = b;
				PATPUSH (duplicate);
				PATPUSH (c1);
				break;

			     case '+':
			     case '?':
			     default:
				/* You might think it would be useful for \ to mean
				   not to translate; but if we don't translate it
				   it will never match anything.  */
				if (translate)
				   c = translate[c];
				goto normal_char;
			  }
		    break;

		 default:
		  normal_char:
		    if (!pending_exact || pending_exact + *pending_exact + 1 != b
			|| *pending_exact == 0177
			|| *p == '*'
			|| *p == '^'
			|| (*p == '+' || *p == '?'))
		      {
			 laststart = b;
			 PATPUSH (exactn);
			 pending_exact = b;
			 PATPUSH (0);
		      }
		    PATPUSH (c);
		    (*pending_exact)++;
	      }
     }

   if (fixup_jump)
      store_jump (fixup_jump, jump, b);

   if (stackp != stackb)
      goto unmatched_open;

   bufp->used = b - bufp->buffer;
   return 0;

 invalid_pattern:
   return Invalid_pattern;

 unmatched_open:
   return Unmatched_leftparen;

 unmatched_close:
   return Unmatched_rightparen;

 end_of_pattern:
   return Premature_end_of_regular_expression;

 nesting_too_deep:
   return Nesting_too_deep;

 too_big:
   return Regular_expression_too_big;

 memory_exhausted:
   return Memory_exhausted;
}


/*----------------------------------------------------------------------
   ReCompileFastmap Given a pattern, compute a fastmap from it.     
   The fastmap records which of the (1 << BYTEWIDTH) possible    
   characters can start a string that matches the pattern.       
   This fastmap is used by re_search to skip quickly over totally
   implausible text.                                             
  ----------------------------------------------------------------------*/
static void         ReCompileFastmap (struct re_pattern_buffer *bufp)
{
   unsigned char      *pattern = (unsigned char *) bufp->buffer;
   int                 size = bufp->used;
   register char      *fastmap = bufp->fastmap;
   register unsigned char *p = pattern;
   register unsigned char *pend = pattern + size;
   register int        j, k;
   unsigned char      *translate = (unsigned char *) bufp->translate;

   unsigned char      *stackb[NFAILURES];
   unsigned char     **stackp = stackb;

   memset (fastmap, 0, (1 << BYTEWIDTH));
   bufp->fastmap_accurate = 1;
   bufp->can_be_null = 0;

   while (p)
     {
	if (p == pend)
	  {
	     bufp->can_be_null = 1;
	     break;
	  }

	switch ((enum regexpcode) *p++)

	      {
		 case exactn:
		    if (translate)
		       fastmap[translate[p[1]]] = 1;
		    else
		       fastmap[p[1]] = 1;
		    break;

		 case begline:
/*      case before_dot:
   case at_dot:
   case after_dot:
 */
		 case wordbound:
		 case notwordbound:
		 case wordbeg:
		 case wordend:
		    continue;

		 case endline:
		    if (translate)
		       fastmap[translate['\n']] = 1;
		    else
		       fastmap['\n'] = 1;
		    if (bufp->can_be_null != 1)
		       bufp->can_be_null = 2;
		    break;

		 case finalize_jump:
		 case maybe_finalize_jump:
		 case jump:
		 case dummy_failure_jump:
		    bufp->can_be_null = 1;
		    j = *p++ & 0377;
		    j += SIGN_EXTEND_CHAR (*(char *) p) << 8;
		    p += j + 1;	/* The 1 compensates for missing ++ above */
		    if (j > 0)
		       continue;
		    /* Jump backward reached implies we just went through
		       the body of a loop and matched nothing.
		       Opcode jumped to should be an on_failure_jump.
		       Just treat it like an ordinary jump.
		       For a * loop, it has pushed its failure point already;
		       if so, discard that as redundant.  */
		    if ((enum regexpcode) *p != on_failure_jump)
		       continue;
		    p++;
		    j = *p++ & 0377;
		    j += SIGN_EXTEND_CHAR (*(char *) p) << 8;
		    p += j + 1;	/* The 1 compensates for missing ++ above */
		    if (stackp != stackb && *stackp == p)
		       stackp--;
		    continue;

		 case on_failure_jump:
		    j = *p++ & 0377;
		    j += SIGN_EXTEND_CHAR (*(char *) p) << 8;
		    p++;
		    *++stackp = p + j;
		    continue;

		 case start_memory:
		 case stop_memory:
		    p++;
		    continue;

		 case duplicate:
		    bufp->can_be_null = 1;
		    fastmap['\n'] = 1;

		 case anychar:
		    for (j = 0; j < (1 << BYTEWIDTH); j++)
		       if (j != '\n')
			  fastmap[j] = 1;
		    if (bufp->can_be_null)
		       return;
		    /* Don't return; check the alternative paths
		       so we can set can_be_null if appropriate.  */
		    break;

		 case wordchar:
		    for (j = 0; j < (1 << BYTEWIDTH); j++)
		       if (SYNTAX (j) == Sword)
			  fastmap[j] = 1;
		    break;

		 case notwordchar:
		    for (j = 0; j < (1 << BYTEWIDTH); j++)
		       if (SYNTAX (j) != Sword)
			  fastmap[j] = 1;
		    break;

		    /* #ifdef emacs */

		 case syntaxspec:
		    k = *p++;
		    for (j = 0; j < (1 << BYTEWIDTH); j++)
		       if (SYNTAX (j) == (enum syntaxcode) k)
			  fastmap[j] = 1;
		    break;

		 case notsyntaxspec:
		    k = *p++;
		    for (j = 0; j < (1 << BYTEWIDTH); j++)
		       if (SYNTAX (j) != (enum syntaxcode) k)
			  fastmap[j] = 1;
		    break;

		    /* #endif emacs */

		 case charset:
		    for (j = *p++ * BYTEWIDTH - 1; j >= 0; j--)
		       if (p[j / BYTEWIDTH] & (1 << (j % BYTEWIDTH)))
			 {
			    if (translate)
			       fastmap[translate[j]] = 1;
			    else
			       fastmap[j] = 1;
			 }
		    break;

		 case charset_not:
		    /* Chars beyond end of map must be allowed */
		    for (j = *p * BYTEWIDTH; j < (1 << BYTEWIDTH); j++)
		       if (translate)
			  fastmap[translate[j]] = 1;
		       else
			  fastmap[j] = 1;

		    for (j = *p++ * BYTEWIDTH - 1; j >= 0; j--)
		       if (!(p[j / BYTEWIDTH] & (1 << (j % BYTEWIDTH))))
			 {
			    if (translate)
			       fastmap[translate[j]] = 1;
			    else
			       fastmap[j] = 1;
			 }
		    break;
		 default:
		    break;
	      }

	/* Get here means we have successfully found the possible starting
	   characters of one path of the pattern.  We need not follow this path
	   any farther.
	   Instead, look at the next alternative remembered in the stack. */

	if (stackp != stackb)
	   p = *stackp--;
	else
	   break;
     }
}


/*----------------------------------------------------------------------
   NextStruct: Advances pEl in the next element in any direction.  
   If the next element is a text element return TRUE       
   Otherwise return FALSE.                                 
  ----------------------------------------------------------------------*/
static ThotBool NextStruct (ThotBool Forward, PtrElement *pEl, int *charNb)
{

   PtrElement          pElTmp, pEl1;

   pElTmp = NULL;
   pEl1 = *pEl;
   *charNb = 0;

   if (pEl1 != NULL)
     {
	if (!Forward)
	  {
	     pElTmp = pEl1->ElPrevious;
	     if (pElTmp == NULL)
	       {
		  /* pEl points to the last sibling. No advances made
		     We already point to a boarder. */
		  while ((*pEl)->ElPrevious == NULL && (*pEl)->ElParent != NULL)
		     *pEl = (*pEl)->ElParent;
		  *pEl = (*pEl)->ElPrevious;
		  return FALSE;
	       }
	  }
	else
	  {
	     pElTmp = pEl1->ElNext;
	     if (pElTmp == NULL)
	       {
		  /* pEl points to the last sibling. No advances made
		     We already point to a boarder. */
		  while ((*pEl)->ElNext == NULL && (*pEl)->ElParent != NULL)
		     *pEl = (*pEl)->ElParent;
		  *pEl = (*pEl)->ElNext;
		  if ((*pEl) != NULL)
		     pEl1 = *pEl;
		  else
		     return FALSE;
	       }
	  }

	if (pElTmp != NULL)
	   if (!Forward)
	      pEl1 = pEl1->ElPrevious;
	   else
	      pEl1 = pEl1->ElNext;

	if (pEl1->ElTerminal && pEl1->ElLeafType == LtText)
	  {
	     if (!Forward)
	       {
		  /* The leftmost character */
		  *charNb = pEl1->ElTextLength;
		  *pEl = pEl1;
		  return TRUE;
	       }
	     else
	       {
		  /* The rightmost character */
		  *charNb = 1;
		  *pEl = pEl1;
		  return TRUE;
	       }
	  }
	else
	   /* the next element is not a text element */
	  {
	     *pEl = pEl1;
	     return FALSE;
	  }
     }
   else				/* return FALSE if *pEl == NULL */
      return FALSE;
}


/*----------------------------------------------------------------------
   GetOrd: Gets the ordinate value of the character charNb pointed 
   to by pEl. The value is placed in textChar. If pEl is   
   not a text-struct, -1 is returned in textChar.          
  ----------------------------------------------------------------------*/
static void GetOrd (int *textChar, PtrElement pEl, int charNb)
{
   int                 skipChar;
   PtrTextBuffer       textBuffer, PtrTmp;

   *textChar = -1;
   if (pEl != NULL)
      if (pEl->ElTypeNumber == 1 && charNb <= pEl->ElTextLength)
	{
	   skipChar = charNb;
	   textBuffer = pEl->ElText;
	   PtrTmp = textBuffer->BuNext;

	   /* Finds the buffer where the character charNb is. 
	      After the loop textbuffer points at the text-buffer
	      and skipChar contains the index in the array. */
	   while (PtrTmp != NULL && skipChar > textBuffer->BuLength)
	     {
		PtrTmp = PtrTmp->BuNext;
		skipChar = skipChar - textBuffer->BuLength;
		textBuffer = textBuffer->BuNext;
	     }
	   *textChar = (unsigned char) textBuffer->BuContent[skipChar - 1];
	}
}


/*----------------------------------------------------------------------
   NextChar finds the next character and returns it in textChar    
   If it doesn't find a character it returns -1 in         
   textChar (textChar contain ordinate-value of the char)  
   While advancing through the text pEl and charNb is      
   kept up to date.                                        
  ----------------------------------------------------------------------*/
static void NextChar (ThotBool Forward, int *textChar, PtrElement *pEl, int *charNb)
{
   ThotBool            nextstructure;

   nextstructure = TRUE;
   if (*pEl != NULL)
     {
	/* Text unit and not a copy */
	if ((*pEl)->ElTerminal && (*pEl)->ElLeafType == LtText)
	  {
	     if (!Forward)
		if ((*charNb) <= 1)
		   nextstructure = NextStruct (Forward, pEl, charNb);
		else if ((*charNb) <= (*pEl)->ElTextLength)
		   (*charNb)--;
		else
		  {
		     /* This piece of code should never
		        be entered. charNb should allways
		        be < pEl->ElTextLength */
		  }
	     else if ((*charNb) == (*pEl)->ElTextLength)
		nextstructure = NextStruct (Forward, pEl, charNb);
	     else if ((*charNb) >= 0)
		(*charNb)++;
	     else
	       {
		  /* This piece of code should never
		     be entered. charNb should always
		     be > 0  */
	       }
	  }
	else
	   nextstructure = NextStruct (Forward, pEl, charNb);
     }
   else
     {
	/* *pEl == NULL, nothing to do, perhaps an error. */
     }

   if (nextstructure)
      GetOrd (textChar, *pEl, *charNb);
   else
      *textChar = -1;
}

/*----------------------------------------------------------------------
   nbmatch computes the number of characters in the expression found	
   	and returns that number.					
   Used in backwards search.					
  ----------------------------------------------------------------------*/
static int nbmatch (PtrElement pEl1, int pos1, PtrElement pEl2, int pos2)
{
   int                 textchar;
   int                 nbmatched = 1;

   while (((pEl1 != pEl2) || (pos1 != pos2)) && pEl1 != NULL)
     {
	NextChar (TRUE, &textchar, &pEl1, &pos1);
	nbmatched += 1;
     }
   return nbmatched;
}

/*----------------------------------------------------------------------
   ReMatch matchs the pattern described by PBUFP against data which   
   start at the position described by pEl1 and pos1.             
   1 is returned if thers is a match.                            
   -1 is returned if there is no match.                          
   -2 is returned if there is an error (such as match stack      
   overflow).                                                    
  ----------------------------------------------------------------------*/
static int ReMatch (struct re_pattern_buffer *pbufp, PtrElement pEl1,
		    int pos1, PtrElement * pEl2, int *pos2, PtrElement pEndEl,
		    int endCharNb, ThotBool forw)
{
   PtrElement          pElBack;
   int                 posBack;
   int                 textcharBack;

   register unsigned char *p = (unsigned char *) pbufp->buffer;
   register unsigned char *pend = p + pbufp->used;
   int                 textchar;
   int                 textchar2;
   ThotBool            stop = FALSE;
   PtrElement          pElTmp;
   int                 posTmp;

   register int        mcnt;
   unsigned char      *translate = (unsigned char *) pbufp->translate;
   int                 re_max_failures = 2000;

   /* Failure point stack.  Each place that can handle a failure further down the line
      pushes a failure point on this stack.  It consists of two char *'s.
      The first one pushed is where to resume scanning the pattern;
      the second pushed is where to resume scanning the strings.
      If the latter is zero, the failure point is a "dummy".
      If a failure happens and the innermost failure point is dormant,
      it discards that failure point and tries the next one. */

   struct t_stack     *initial_stack[NFAILURES];
   struct t_stack    **stackb = initial_stack;
   struct t_stack    **stackp = stackb;
   struct t_stack    **stacke = &stackb[NFAILURES];
   struct t_stack    **stackx;

   unsigned char      *regstart[RE_NREGS];
   unsigned char      *regend[RE_NREGS];
   unsigned char       regstart_seg1[RE_NREGS], regend_seg1[RE_NREGS];
   unsigned int        i;

   pElTmp = NULL;
   posTmp = 0;
   /* Initialize \) text positions to -1 to mark ones that no \(or \) has been seen for.  */

   for (mcnt = 0; mcnt < sizeof (regend) / sizeof (*regend); mcnt++)
      regend[mcnt] = (unsigned char *) -1;

   *pEl2 = pEl1;
   *pos2 = pos1;

   /* This loop loops over pattern commands.
      It exits by returning from the function if match is complete,
      or it drops through if match fails at this starting point in the input data. */


   while (1)
     {
	if (p == pend)
	  {
	     *pEl2 = pElTmp;
	     *pos2 = posTmp;
	     if (*pEl2 != NULL)
		return 1;
	     else		/* nothing is matched */
		return -1;

	  }
	if (!stop)
	  {
	     if (pEl1 != *pEl2)
	       {
		  if (pEl1->ElTypeNumber == 1)
		     /* considers element change as a separator */
		    {
		       *pEl2 = pEl1;
		       *pos2 = (*pEl2)->ElTextLength + 1;
		       textchar = 32;	/* space */
		    }
		  else
		     /* start searching */
		    {
		       pEl1 = *pEl2;
		       *pos2 = forw ? 0 : (*pEl2)->ElTextLength + 1;
		       textchar = 32;	/* space */
		    }
	       }
	     else
		/* keep searching */
	       {
		  if ((*pEl2) != NULL)
		    {
		       if ((*pEl2)->ElTypeNumber != 1)
			 {
			    /* go to next text element */
			    *pEl2 = forw ?
			       FwdSearchTypedElem (*pEl2, CharString + 1, NULL,
						   NULL) :
			       BackSearchTypedElem (*pEl2, CharString + 1,
						    NULL, NULL);
			    if ((*pEl2) != NULL)
			       *pos2 = forw ? 1 : (*pEl2)->ElTextLength;
			    else
			       /* no text found */
			       *pos2 = -1;
			 }
		       else
			 {
			    if (pos1 == 0 && !forw)
			      {
				 /* go to previous text element */
				 *pEl2 = BackSearchTypedElem (*pEl2, CharString + 1, NULL, NULL);
				 if ((*pEl2) != NULL)
				    *pos2 = forw ? 1 : (*pEl2)->ElTextLength;
				 else
				    /* no text found */
				    *pos2 = -1;
			      }
			    else if (pos1 > pEl1->ElTextLength && forw)
			      {
				 /* go to next text */
				 *pEl2 = FwdSearchTypedElem (*pEl2, CharString + 1, NULL, NULL);
				 if ((*pEl2) != NULL)
				    *pos2 = forw ? 1 : (*pEl2)->ElTextLength;
				 else
				    /* no text found */
				    *pos2 = -1;
			      }
			 }
		    }
		  if ((*pEl2) != NULL)
		     GetOrd (&textchar, *pEl2, *pos2);
	       }
	  }
	switch ((enum regexpcode) *p++)

	      {

		 case start_memory:

		    regstart[*p] = 0;
		    regstart_seg1[*p++] = 0;
		    break;

		 case stop_memory:
		    regend[*p] = 0;
		    regend_seg1[*p++] = 0;
		    break;

		 case anychar:
		    /* Match anything but a newline and a whitespace.  */
		    if (!stop)
		       GetOrd (&textchar, *pEl2, *pos2);
		    if ((translate ? translate[textchar] : textchar) == '\n'
		      || (translate ? translate[textchar] : textchar) == ' '
		    || (translate ? translate[textchar] : textchar) == '\240'
		    || (translate ? translate[textchar] : textchar) == '\201'
		    || (translate ? translate[textchar] : textchar) == '\202'
		    || (translate ? translate[textchar] : textchar) == '\212'
			|| textchar == -1)
		      {
			 goto fail;
		      }
		    pElTmp = *pEl2;
		    posTmp = *pos2;

		    /* advance a character */

		    if (*pEl2 == pEndEl && *pos2 == endCharNb)
		      {
			 textchar = -1;
			 stop = TRUE;
		      }
		    else
		      {
			 NextChar (TRUE, &textchar, pEl2, pos2);
			 if (textchar == -1)
			    stop = TRUE;
		      }
		    break;

		 case charset:
		 case charset_not:
		    if (textchar != -1)
		      {
			 /* Nonzero for charset_not */
			 int                 not = 0;
			 register int        c;

			 if (*(p - 1) == (unsigned char) charset_not)
			    not = 1;

			 if (translate)
			    c = translate[textchar];
			 else
			    c = textchar;

			 if (c < *p * BYTEWIDTH
			   && p[1 + c / BYTEWIDTH] & (1 << (c % BYTEWIDTH)))
			    not = !not;

			 p += 1 + *p;

			 if (!not)
			    goto fail;
			 pElTmp = *pEl2;
			 posTmp = *pos2;
			 if (*pEl2 == pEndEl && *pos2 == endCharNb)
			   {
			      textchar = -1;
			      stop = TRUE;
			   }
			 else
			   {
			      pElBack = *pEl2;
			      posBack = *pos2;
			      NextChar (TRUE, &textchar, pEl2, pos2);	/* advance a character */
			      if (textchar == -1)
				{
				   stop = TRUE;
				   *pEl2 = pElBack;
				   *pos2 = posBack;
				}
			   }
			 break;
		      }
		    else
		       goto fail;
		 case begline:
		    if (((*pEl2)->ElPrevious == NULL) && (*pos2 == 1))
		       break;
		    else
		      {
			 NextChar (FALSE, &textchar, pEl2, pos2);
			 if (textchar == '\n')
			   {
			      NextChar (TRUE, &textchar, pEl2, pos2);
			      break;
			   }
			 else
			    goto fail;
		      }

		 case endline:
		    if (((*pEl2)->ElNext == NULL) && (*pos2 == (*pEl2)->ElTextLength))
		       break;
		    else
		      {
			 if (textchar == '\n')
			    break;
			 else
			    goto fail;
		      }

		    /* "or" constructs ("|") are handled by starting each alternative
		       with an on_failure_jump that points to the start of the next alternative.
		       Each alternative except the last ends with a jump to the joining point.
		       (Actually, each jump except for the last one really jumps
		       to the following jump, because tensioning the jumps is a hassle.) */
		    /* The start of a stupid repeat has an on_failure_jump that points
		       past the end of the repeat text.
		       This makes a failure point so that, on failure to match a repetition,
		       matching restarts past as many repetitions have been found
		       with no way to fail and look for another one.  */

		    /* A smart repeat is similar but loops back to the on_failure_jump
		       so that each repetition makes another failure point. */

		 case on_failure_jump:
		    if (stackp == stacke)
		      {
			 if (stacke - stackb > re_max_failures)
			    return -2;
			 stackx = (struct t_stack **) TtaGetMemory (2 * (stacke - stackb) * sizeof (struct t_stack *));
			 memcpy (stackx, stackb, (size_t) ((stacke - stackb) * sizeof (struct t_stack *)));

			 stackp = stackx + (stackp - stackb);
			 stacke = stackx + 2 * (stacke - stackb);
			 stackb = stackx;
		      }
		    mcnt = *p++ & 0377;
		    mcnt += SIGN_EXTEND_CHAR (*(char *) p) << 8;
		    p++;
		    *stackp = (struct t_stack *) TtaGetMemory (sizeof (struct t_stack));

		    (*stackp)->pchar = mcnt + p;
		    (*stackp)->pEl = *pEl2;
		    (*stackp)->index = *pos2;
		    stackp++;
		    break;

		    /* The end of a smart repeat has an maybe_finalize_jump back.
		       Change it either to a finalize_jump or an ordinary jump. */

		 case maybe_finalize_jump:
		    mcnt = *p++ & 0377;
		    mcnt += SIGN_EXTEND_CHAR (*(char *) p) << 8;
		    p++;

		    /* Compare what follows with the begining of the repeat.
		       If we can establish that there is nothing that they would
		       both match, we can change to finalize_jump */

		    if (p == pend)
		       p[-3] = (unsigned char) finalize_jump;
		    else if (*p == (unsigned char) exactn
			     || *p == (unsigned char) endline)
		      {
			 register int        c = *p == (unsigned char) endline ? '\n' : p[2];
			 register unsigned char *p1 = p + mcnt;

			 /* p1[0] ... p1[2] are an on_failure_jump. Examine what follows that */

			 if (p1[3] == (unsigned char) exactn && p1[5] != c)
			    p[-3] = (unsigned char) finalize_jump;
			 else if (p1[3] == (unsigned char) charset || p1[3] == (unsigned char) charset_not)
			   {
			      int                 not = p1[3] == (unsigned char) charset_not;

			      if (c < p1[4] * BYTEWIDTH && p1[5 + c / BYTEWIDTH] & (1 << (c % BYTEWIDTH)))
				 not = !not;
			      /* not is 1 if c would match */
			      /* That means it is not safe to finalize */
			      if (!not)
				 p[-3] = (unsigned char) finalize_jump;
			   }
		      }
		    p -= 2;
		    if (p[-1] != (unsigned char) finalize_jump)
		      {
			 p[-1] = (unsigned char) jump;
			 goto nofinalize;
		      }

		    /* The end of a stupid repeat has a finalize-jump
		       back to the start, where another failure point will be made
		       which will point after all the repetitions found so far. */

		 case finalize_jump:
		    stackp -= 1;
		 case jump:
		  nofinalize:
		    mcnt = *p++ & 0377;
		    mcnt += SIGN_EXTEND_CHAR (*(char *) p) << 8;
		    p += mcnt + 1;	/* The 1 compensates for missing ++ above */
		    break;

		 case dummy_failure_jump:
		    if (stackp == stacke)
		      {
			 i = 2 * (stacke - stackb) * sizeof (struct t_stack *);

			 stackx = (struct t_stack **) alloca (i);
			 memcpy (stackx, stackx, (size_t) ((stacke - stackb) * sizeof (struct t_stack *)));

			 stackp = stackx + (stackp - stackb);
			 stacke = stackx + 2 * (stacke - stackb);
			 stackb = stackx;
		      }
		    *stackp = (struct t_stack *) TtaGetMemory (sizeof (struct t_stack));

		    (*stackp)->pchar = NULL;
		    (*stackp)->pEl = NULL;
		    (*stackp)->index = 0;
		    stackp++;
		    goto nofinalize;

		 case wordbound:
		    pElBack = *pEl2;
		    posBack = *pos2;
		    textcharBack = textchar;

		    NextChar (FALSE, &textchar, pEl2, pos2);
		    if (textchar != -1)
		      {
			 textchar2 = textchar;
			 NextChar (TRUE, &textchar, pEl2, pos2);
			 if ((SYNTAX (textchar2) == Sword)
			     != (SYNTAX (textchar) == Sword))
			    break;
		      }
		    else
		      {
			 *pEl2 = pElBack;
			 *pos2 = posBack;
			 textchar = textcharBack;
			 if (SYNTAX (textchar) == Sword)
			    break;
		      }
		    goto fail;

		 case notwordbound:
		    pElBack = *pEl2;
		    posBack = *pos2;
		    textcharBack = textchar;

		    NextChar (FALSE, &textchar, pEl2, pos2);

		    if (textchar != -1)
		      {
			 textchar2 = textchar;
			 NextChar (TRUE, &textchar, pEl2, pos2);
			 if ((SYNTAX (textchar2) == Sword)
			     != (SYNTAX (textchar) == Sword))
			    goto fail;
		      }
		    else
		       goto fail;
		    break;

		 case wordbeg:
		    if (SYNTAX (textchar) != Sword)	/* next char not a letter */
		       goto fail;
		    pElBack = *pEl2;
		    posBack = *pos2;
		    textcharBack = textchar;
		    NextChar (FALSE, &textchar, pEl2, pos2);
		    if (textchar != -1)
		      {
			 textchar2 = textchar;
			 NextChar (TRUE, &textchar, pEl2, pos2);
		      }
		    else
		      {
			 textchar2 = textchar;
			 *pEl2 = pElBack;
			 *pos2 = posBack;
			 textchar = textcharBack;
		      }
		    if (textchar2 == -1		/* begining of an element */
			|| SYNTAX (textchar2) != Sword)		/* prev char not a letter */
		       break;
		    else
		       goto fail;

		 case wordend:
		    if (textchar != -1)
		      {
			 pElBack = *pEl2;
			 posBack = *pos2;
			 textchar2 = textchar;
			 NextChar (TRUE, &textchar, pEl2, pos2);
			 if (SYNTAX (textchar2) != Sword)
			    goto fail;
			 if (textchar == -1 || SYNTAX (textchar) != Sword)
			   {
			      *pEl2 = pElBack;
			      *pos2 = posBack;
			      textchar = textchar2;
			      break;
			   }
			 goto fail;
		      }
		    else
		       /* end of a (or a set of ) text element */
		      {
			 GetOrd (&textchar, *pEl2, *pos2);
			 if (SYNTAX (textchar) == Sword)
			   {
			      textchar = -1;
			      break;
			   }
			 else
			    goto fail;

		      }

		 case wordchar:
		    mcnt = (int) Sword;
		    goto matchsyntax;

		 case syntaxspec:
		    mcnt = *p++;

		  matchsyntax:

		    if (SYNTAX (textchar) != (enum syntaxcode) mcnt)
		      {
			 goto fail;
		      }
		    pElTmp = *pEl2;
		    posTmp = *pos2;
		    if ((*pEl2 == pEndEl && *pos2 == endCharNb)
			|| (*pEl2 == NULL && *pos2 == -1))
		      {
			 textchar = -1;
			 stop = TRUE;
		      }
		    else
		       NextChar (TRUE, &textchar, pEl2, pos2);	/* advance a character */
		    break;

		 case notwordchar:
		    mcnt = (int) Sword;
		    goto matchnotsyntax;

		 case notsyntaxspec:
		    mcnt = *p++;

		  matchnotsyntax:

		    if (SYNTAX (textchar) == (enum syntaxcode) mcnt)
		      {
			 goto fail;
		      }
		    pElTmp = *pEl2;
		    posTmp = *pos2;
		    if (*pEl2 == pEndEl && *pos2 == endCharNb)
		      {
			 textchar = -1;
			 stop = TRUE;
		      }
		    else
		       NextChar (TRUE, &textchar, pEl2, pos2);	/* advance a character */
		    break;


		 case exactn:
		    /* Match the next few pattern characters exactly.
		       mcnt is how many characters to match. */
		    mcnt = *p++;
		    if (translate)
		      {
			 do
			   {
			      if (translate[textchar] != *p++)
				{
				   goto fail;
				}
			      pElTmp = *pEl2;
			      posTmp = *pos2;
			      if (*pEl2 == pEndEl && *pos2 == endCharNb)
				{
				   textchar = -1;
				   stop = TRUE;
				}
			      else
				 NextChar (TRUE, &textchar, pEl2, pos2);	/* advance a character */
			   }
			 while (--mcnt);
		      }
		    else
		      {
			 do
			   {
			      if (textchar != *p++)
				{
				   goto fail;
				}
			      pElTmp = *pEl2;
			      posTmp = *pos2;
			      if (*pEl2 == pEndEl && *pos2 == endCharNb)
				{
				   textchar = -1;
				   stop = TRUE;
				}
			      else
				 NextChar (TRUE, &textchar, pEl2, pos2);	/* advance a character */
			   }
			 while (--mcnt);
		      }
		    break;
		 default:
		    break;
	      }
	continue;		/* Successfully matched one pattern command; keep matching */

	/* Jump here if any matching operation fails. */
      fail:
	if (stackp != stackb)
	   /* A restart point is known.  Restart there and pop it. */
	  {
	     --stackp;
	     if ((*stackp)->pEl == NULL)
	       {		/* If innermost failure point is dormant, flush it and keep looking */
		  goto fail;
	       }
	     *pos2 = (*stackp)->index;
	     *pEl2 = (*stackp)->pEl;
	     p = (*stackp)->pchar;
	     stop = FALSE;

	  }
	else
	   break;		/* Matching at this starting point really fails! */
     }
   *pEl2 = pElTmp;
   *pos2 = posTmp;
   return -1;			/* Failure to match */
}


/*----------------------------------------------------------------------
   SearchRegularExpression	search a regular expression				
  ----------------------------------------------------------------------*/
ThotBool SearchRegularExpression (PtrElement *firstEl, int *firstChar,
				  PtrElement *lastEl, int *lastChar,
				  ThotBool forward, ThotBool caseEquiv,
				  char *expression)
{

   PtrElement          pEl1, pEl1old;
   PtrElement         *pEl2;
   PtrElement          pElTmp1;
   PtrElement          pElTmp2;
   int                 inx1;
   int                 inx2;
   int                 posTmp1;
   int                 posTmp2;
   int                *textchar;
   ThotBool            found;
   ThotBool            stop;
   struct re_pattern_buffer buf;
   char                fastmap[(1 << BYTEWIDTH)];
   int                 compilResult;
   register unsigned char *translate;
   int                 val;
   int                 nbmatched1 = 0;
   int                 nbmatched2 = 0;
   char                express[256];

   strcpy (express, expression);


   buf.allocated = 40;
   buf.buffer = (char *) malloc (buf.allocated);
   buf.fastmap = fastmap;
   buf.translate = caseEquiv ? upcase : NULL;
   posTmp2 = 0;
   posTmp1 = 0;
   stop = FALSE;
   found = FALSE;
   pElTmp1 = pElTmp2 = NULL;

   compilResult = ReCompilePattern (express, strlen (express), &buf);
   if (compilResult != 0)
      /* error */
     {
	switch (compilResult)
	      {
		 case 1:
		    TtaDisplaySimpleMessage (INFO, LIB, TMSG_INCORRECT_REG_EXP);
		    break;
		 case 2:
		    TtaDisplaySimpleMessage (INFO, LIB, TMSG_UNMATCHED_LEFT);
		    break;
		 case 3:
		    TtaDisplaySimpleMessage (INFO, LIB, TMSG_UNMATCHED_RIGHT);
		    break;
		 case 4:
		    TtaDisplaySimpleMessage (INFO, LIB, TMSG_PREMATURE_END_REG_EXP);
		    break;
		 case 5:
		    TtaDisplaySimpleMessage (INFO, LIB, TMSG_LIB_NESTING_TOO_DEEP);
		    break;
		 case 6:
		    TtaDisplaySimpleMessage (INFO, LIB, TMSG_REG_EXP_TOO_BIG);
		    break;
		 case 7:
		    TtaDisplaySimpleMessage (INFO, LIB, TMSG_NO_MEMORY);
		    break;
	      }
	return TRUE;
     }
   else
     {
	ReCompileFastmap (&buf);
	translate = (unsigned char *) buf.translate;
	pEl2 = (PtrElement *) malloc (sizeof (PtrElement));
	textchar = (int *) malloc (sizeof (int));

	pEl1 = *firstEl;
	inx1 = *firstChar;

	while (!found && !stop)
	  {
	     /* If a fastmap is supplied, skip quickly over characters
	        that cannot possibly be the start of a match.
	        Note, however, that if the pattern can possibly match
	        the null string, we must test it at each starting point
	        so that we take the first null string we get.  */

	     if (buf.can_be_null != 1)
	       {

		  if (pEl1 != NULL && pEl1->ElTypeNumber != 1)
		    {
		       pEl1 = forward ?
			  FwdSearchTypedElem (pEl1, CharString + 1, NULL, NULL) :
			  BackSearchTypedElem (NextElement (pEl1), CharString + 1, NULL, NULL);
		       if (pEl1 != NULL)
			  inx1 = forward ? 1 : pEl1->ElTextLength;
		    }

		  GetOrd (textchar, pEl1, inx1);
		  if (translate)
		     *textchar = translate[*textchar];
		  while (!fastmap[*textchar])
		    {
		       if (nbmatched2 > 0)
			  goto Lstop;
		       else
			 {
			    if (pEl1 != *lastEl || inx1 != *lastChar)
			      {
				 NextChar (forward, textchar, &pEl1, &inx1);
				 if (*textchar == -1)
				   {
				      pEl1 = forward ?
					 FwdSearchTypedElem (pEl1, CharString + 1, NULL, NULL) :
					 BackSearchTypedElem (NextElement (pEl1), CharString + 1, NULL, NULL);
				      if (pEl1 == NULL)
					{
					   stop = TRUE;
					   break;
					}
				      else
					 inx1 = forward ? 1 : pEl1->ElTextLength;
				      GetOrd (textchar, pEl1, inx1);
				   }
				 if (translate)
				    *textchar = translate[*textchar];
			      }
			    else
			      {
				 stop = TRUE;
				 break;
			      }
			 }
		    }
	       }

	     if (stop)
		break;
	     else
	       {
		  val = ReMatch (&buf, pEl1, inx1, pEl2, &inx2,
				 *lastEl, *lastChar, forward);
		  if (inx1 < 0)
		     /* go to the beginning of the text element */
		     inx1 = 0;
	       }
	     /* Propagate error indication if worse than mere failure.  */
	     if (val == -2)
	       {
		  TtaDisplaySimpleMessage (INFO, LIB, TMSG_NO_MEMORY);
		  return (TRUE);
	       }
	     if (val == 1)
	       {
		  if (*pEl2 == NULL)
		     break;
		  if (forward)
		    {
		       found = TRUE;
		       break;
		    }
		  else
		     /* search a larger expression backwards */
		    {
		       if (inx2 == 0)
			 {
			    *pEl2 = pEl1;
			    inx2 = pEl1->ElTextLength;
			 }
		       nbmatched1 = nbmatched2;
		       nbmatched2 = nbmatch (pEl1, inx1, *pEl2, inx2);
		       if (nbmatched2 > nbmatched1)
			 {
			    pElTmp1 = pEl1;
			    posTmp1 = inx1;
			    pElTmp2 = *pEl2;
			    posTmp2 = inx2;
			    nbmatched1 = nbmatched2;
			    goto Lcontinue;
			 }
		       else
			  goto Lstop;
		    }
	       }
	     if (val == -1)	/* match was failed from the position inx1 */
	       {
		  if (nbmatched1 != 0)
		     break;
		  else
		    {
		     Lcontinue:
		       if (pEl1 != *lastEl || inx1 != *lastChar)
			 {
			    pEl1old = pEl1;
			    NextChar (forward, textchar, &pEl1, &inx1);
			    if (*textchar == -1)
			      {
				 pEl1 = forward ?
				    FwdSearchTypedElem (pEl1old, CharString + 1, NULL, NULL) :
				    BackSearchTypedElem (pEl1old, CharString + 1,
							 NULL, NULL);
				 if (pEl1 == NULL)
				    goto Lstop;
				 else
				    inx1 = forward ? 1 : pEl1->ElTextLength;
			      }
			 }
		       else
			  break;
		    }
	       }
	  }

      Lstop:if (pElTmp1 != NULL && !forward)
	  {
	     pEl1 = pElTmp1;
	     inx1 = posTmp1;
	     *pEl2 = pElTmp2;
	     inx2 = posTmp2;
	     found = TRUE;
	  }
	if (found)
	   /* element found is pointed to by pEl1 and inx1 is the rank of
	      the first character found in that element */
	   if (*lastEl != NULL)
	      /* stop before document end */
	      if (pEl1 == *lastEl)
		 /* string found is in the last element of the search domain */
		{
		   if (forward)
		     {
			if (inx2 - 1 > *lastChar)
			   /* found string end after the last character of the
			      search domain */
			   found = FALSE;
		     }
		   else
		     {
			if (*lastChar > 0)
			   if (inx1 < *lastChar)
			      found = FALSE;
		     }
		}
	      else if (forward)
		{
		   if (ElemIsBefore (*lastEl, pEl1))
		      /* found element is after the search domain */
		      found = FALSE;
		}
	      else if (ElemIsBefore (pEl1, *lastEl))
		 found = FALSE;
	if (found)
	  {
	     *firstEl = pEl1;
	     *firstChar = inx1;
	     *lastEl = *pEl2;
	     *lastChar = inx2;
	  }
	return found;
     }
}
