/*
 *
 * main parsing routines for the cextract/cextdoc programs.
 * Also includes a number of generic routines applicable to both.
 *
 * These routines provide the methods used to extract comments,
 * prototypes and function names from C source files.
 *
 * Copyright (c) 1990, 1991, 1992 by Adam Bryant
 *
 * The only restrictions on the use of these routines is that they not
 * be used for monetary gain, and that this copyright notice be left
 * intact.
 *
 * Note:  These programs may be freely used to extract code and/or
 *        documentation at proprietary/corporate sites, as long as the
 *        actual source code is not used within any proprietary code.
 *
 *        The generated output (header) files are distributable in any
 *        manner desired as they are based on the scanned source code,
 *        not my program.
 */
/*
 * Version history:
 *
 * Changes for Version 0.40 of both cextract and cextdoc:
 *    - implemented new out_char() interface.
 *    - first comments will not be shown if no functions exist.
 *    - implemented concatenation of comments.
 *    - implemented troff output for cextdoc portion.
 *
 * Changes for Version 0.41 of both cextract and cextdoc:
 *    - switched the NO_SETBUFFER to SETBUFFER, making non-use standard.
 *    - added in the DO_VARARGS code to automatically account for the
 *      the variable argument setup in parsing.
 *
 * Changes for Version 0.42 of both:
 *    - made the DO_VARARGS code the default.  (removed #ifdefs)
 *
 * Changes for Version 0.50 (1/18/91) of both:
 *    - added the configuration file code.
 *    - removed much of the DEBUG print statements from stable code.
 *    - made the bad syntax encounters exit the program.
 *
 * Changes for Version 0.51 (1/21/91) of both:
 *    - implemented code to allow compilation on VAX VMS systems.
 *
 * Changes for Version 0.60 (4/15/91) of both:
 *    - more VMS stuff, and addition of files: patchlevel.h,
 *      README, and INSTALL.
 *    - removed all other DEBUG statements.
 *    - left the SETBUFFER code in place, but removed mention
 *      of it from the Makefile, since there was no noticable
 *      speed enhancement through its use.
 *    - upgraded to version 6.0 for release to comp.sources.reviewed.
 *
 * Changes for Version 1.0 (4/17/91-5/3/91) of both:
 *    - removed most system definitions from cproto.h and replaced
 *      with more includes of system header files.  But, there are
 *      still a number of system functions which are not in the header
 *      files on at least Sun systems.
 *    - now use the "is_switch()" function to parse the command line
 *      arguments.  This allowed me to use both '-' and '/' on VMS
 *      systems to avoid confusion in the manual pages.
 *    - simplified the Makefile, especially for the "make install".
 *    - fixed various mispellings/misphrasings in error messages.
 *    - checked the argument in calls to free() within pop_proto().
 *    - configuration files will be checked for in the following order:
 *        system file, home directory, current directory.
 *      any encountered will be read in.  This will allow overriding
 *      of customization options in a logical order.
 *    - implemented switch to control how the config files are read in.
 *    - redid the manual pages and split out the customization section
 *      into its own manual page.
 *    - adjusted the comment capturing code to assure that it doesn't
 *      misassign comments to the wrong functions.
 *    - fixed one minor memory leak.
 *    - adjust system configuration file locations.
 *    - redid VMS popen() and pclose() to assure unique temporary file.
 *    - added in an "err_msg()" routine to call perror() if it exits.
 *    - made sure to check return value of all calls to fputs().
 *    - merged "cextdoc" into "cextract".
 *    - added verbose command line options with same format as the
 *      configuration commands.
 *    - now use the "+" and "-" switches to indicate on and off.
 *    - added a number of new options to support the new merged
 *      programs and to allow displaying of settings.
 *    - added a -Hstring to indicate replacement of the sequence:
 *
 *          #ifndef __CEXTRACT__
 *            ... output ...
 *          #endif
 *
 *      with:
 *
 *          #ifndef string
 *          #define string
 *            ... output ...
 *          #endif
 *
 *    - wrote up a VMS help file for cextract.
 *
 * Version 1.1: Okay, so there were still more changes needed. :-)
 *    - fixed more occurances of proceed instead of precede.
 *    - fixed typos of "it's" instead of "its".
 *    - removed empty targets from Makefile
 *    - add sed facility to Makefile to properly configure man pages.
 *    - wrote a ".so" implementation for cextdoc manual instead of
 *      prior hard link method.
 *    - added a MANDIR variable to the Makefile.
 *    - tweaked the documentation in a few places.
 *    - allow the "!" or "no-" (Non-VMS) or "no" (VMS) prefix to
 *      command line options.
 *    - use "sys$login:" as HOME directory on VMS systems, if HOME
 *      environment variable is not defined.
 *    - changed the "sys$system:" to "sys$library".
 *    - fixed typo of "fclose(*stream)" in the VMS pclose.
 *    - adjusted the VMS qualifier building to build /define=(a,b,c), etc.
 *    - added in a "remove-names" option to eliminate variable names
 *      from the parameter list output.
 *    - fixed strncmp() bug in the check for the "replace" option.
 *    - fixed a number of calls to cfg_err() to pass cmd_line parameter.
 *    - implemented better check for empty or "void" parameter lists.
 *    - merged the void check and style determination into one routine
 *      and called it diverge_style().
 *    - changed a number of routines to use char* math instead of
 *      array stepping using counters.
 *    - added a configuration file section to the VMS help file.
 *    - tweaked the cextract.1 and cextrc.5 manual pages a bit more.
 *    - edited the README, INSTALL, and INSTALL.VMS files.
 *    - implemented checks to be sure that the files to be parsed are
 *      accessible before calling the C preprocessor.
 *    - renamed the files as:
 *         cproto.c     =>   parse.c
 *         cproto.h     =>   xtract.h
 *         cextract.c   =>   main.c
 *      this was done to avoid some of the conflict with files
 *      beginning with the same string of characters.
 *    - made sure that "make test" tested the version in the current
 *      directory.
 *
 * Version 1.2:  yet another pre-submission fix section.
 *    - fixed a typo in the comments.
 *    - separated the INSTALL definition in the Makefile into INSTBIN
 *      and INSTMAN command to allow those systems without "install"
 *      to perform installations properly.
 *    - added "/exe=cextract" to the build.com link operation.
 *    - fixed the character count in the copy_str() function.
 *      [This fixes the misread on the "replace" operation].
 *    - fixed up the "method" description in the README.
 *    - added ${CEXTRACT} to the "test" (ala "make test") dependency.
 *    - removed the "make links" and appended it to "make install"
 *    - changed the "skip-config" command to "read-config".
 *    - had one more go at the cextrc.5 manual page.
 *    - tweaked the cextract.1 manual page as well.
 *    - added check for declaration of array of function pointers.
 *    - added code to parse structure declarations within function
 *      parameter lists.  [why anyone would program like this is
 *      beyond me, but hey, I like to support valid C.]
 *    - made sure to only test for functions where a brace was
 *      preceded by a semi-colon or a paranthesis.
 *    - make sure to clear out the "code_info" array more often.
 *    - increased the array sizes to allow for larger buffer space.
 *      [temporary until implement dynamic memory routines.]
 *    - created a TODO file and removed those items from the bottom
 *      of the README file.
 *    - shortened the BUGS section of the cextract.1 manual page.
 *    - changed the program name check to strncmp() so VMS sites work.
 *    - created a newbuild.com file and mentioned it in INSTALL.VMS.
 *    - fixed errors in the newbuild.com file.
 *    - added a mentioning of the '/' character for VMS options.
 *    - cleaned up the README, INSTALL and other docs.
 *
 * Version 1.3:  first patch after comp.sources.reviewed
 *    - fixed bug where comments within parameter list could cause a
 *	function to be skipped.
 *    - added both the CPP and CPP_COMMENTS flags to the Makefile.
 *    - added patch from Martin Fouts (fouts@bozeman.clipper.ingr.com)
 *	to provide simple changes to allow port to the CLIX OS.
 *    - fixed stupid bug which caused the "-U" flag to not get parsed.
 *    - changed the "#ifdef __STDC__" to the proper "#if __STDC__".
 *    - wrote a new command and flag "build-config" to generate the system
 *	configuration file automatically.  [-B or -b for build]
 *    - made sure the replace macros do not contain duplicates.
 *    - prevented the creation of duplicate '-D','-I', or '-U' entries.
 *    - fixed the "macro_match()" function for all systems. [VMS?]
 *    - reversed the definitions of CFLAGS and COPTS in the Makefile.
 *    - made sure to test if substitution already took place in vargs_find().
 *    - fixed the 'Q' flag so that it properly reads in any numeric.
 *    - added a dependency for the system configuration file to the
 *      "install" target, so that it can be automatically generated.
 *    - fixed the INSTALL, INSTALL.VMS and documentation files to
 *      properly cover the new configuration file building mechanism.
 *    - implemented a better method for the NO_OPEN option.  This should
 *      now work for all systems, not just VMS.
 *    - fixed bug that the type of the function was not checked for
 *      replacement.
 *    - made sure that the subst macros where ordered as they came in.
 *    - fixed bug with parameters for function pointers getting int
 *      tagged on unnecessarily.
 *
 * Version 1.4:  [2/25/92-4/20/92]  Another iteration of fixes.
 *    - allow any sort of text inside of brackets and parenthesis
 *      in the variable declarations.
 *    - fixed the fprintf formats in the font error messages.
 *    - made sure to remove the use of register in prototype types.
 *    - added the +Z option to allow for the compression of the output
 *      using a macro.  [Also under the "merge-output" option.]
 *    - separated functions into a third file [io.c] to reduce the
 *      size of the other files.
 *
 * Version 1.5: [4/29/92-5/18/92] Second submission fixes.
 *    - added ability to exclude all but static functions from search
 *	to allow for the creation of header files for single files.
 *    - fixed the documentation to reflex the new "static" feature.
 *    - fixed some errors in the manual pages.
 *    - adjusted the Makefile:  make echo, sed, nroff and install
 *	configurable; had the "make test" command perform the diff
 *	directly; enhanced the mkdir lines (is it portable?).
 *    - fixed a bug in the out_str() function where it might have
 *	accessed invalid data if the parameter str was NULL.
 *    - for the SGI systems, added a "#define _POSIX_SOURCE" line.
 *    - allow both "no-" and "no" for turning off options.
 *    - implemented a "+W" ("+break-after-types") flag to separate
 *	function type from the function name.
 *    - implemented a "+w#" ("+wrap-parameters") flag to cause the
 *	output of the parameter list to wrap after a given number of
 *	characters.
 *    - added the two new options to the documentation files.
 *    - made use of minmatch_str's return value in io.c and cleaned up
 *	some minor typos.
 *    - fixed serious bug with the default settings for statics.
 *	[Thanks to Jon Whellams for catching this.  *sigh*]
 *    - major rewrite of the options system:
 *	  +  merged most of the options into a two dimensional array.
 *	  +  used this two-dimensional array to distinguish between
 *	     options used in extract mode and doc mode.
 *	  +  this makes it possible for the generated config files to
 *	     contain the proper "extract-only " or "doc-only "
 *	     prefixes.
 *    - merged the two static flags into one flag with multiple
 *	options, removing the confusing "-only-statics" flag.
 *    - fixed the build_rc() routine to properly prefix "doc-only " or
 *	"extract-only " to the output when appropriate.
 *    - the mode ("extract", "doc", etc.) will no longer be written
 *	when a configuration file is generated so "cextdoc" works.
 *
 * Version 1.6: [8/28/92] Third submission fixes.
 *    - added line to set the permissions on the config file during install.
 *    - regenerated the proto.h file to grab the main.c "parce.c" typo fix.
 *    - fixed the documentation for the "-w#" flag. ["-w=#" --> "-w#"]
 *    - for the INSTALL file, I now explicitly mention that installation
 *	of the configuration file might need root permission.
 *    - fixed outdated usages of pclose [pclose -> close_input].
 *    - fixed the wrong usage of "command" in the "open_input" function.
 *    - allow for alternative return values from system().
 *    - make sure that multiple replacements can take place.
 *    - fixed error with "replace" not recognizing "name" selection.
 *    - made sure an error is printed if a bad "replace" format is seen.
 *    - added support for // comments to begin future C++ support.
 *    - provided a "strstr()" function for Xenix sites.
 *    - expanded the "output-file" argument to support separate output
 *	files for extraction and documentation modes.
 *    - added the ability to select the C preprocessor at runtime.
 *    - updated all of the documentation to reflect the above changes.
 *    - preserve the doc/extract selection for macros and replace commands.
 *
 * Version 1.7: [10/30/92] Hopefully last enhancements before acceptance
 *    - keep track of line numbers within the file being parsed.
 *    - any syntax errors will be reported without aborting the file
 *	parsing.
 *    - syntax errors will be accurately reported by file and line number.
 *    - added a check to test for too many left parenthesis.
 *
 */
#include "xtract.h"

/* storage elements */
P_PROTO proto_list = NULL, proto_next = NULL;
P_MACRO macro_list = NULL, last_macro = NULL;
P_SUBST subst_list = NULL, last_subst = NULL;
FILE *fpin = NULL;
char code_info[MAX_SIZE+1], cfg_file[MID_SIZE];
char dummy_str[MID_SIZE], file_name[MID_SIZE];
int arg_ch_count = 0;
int out_file_flag = 0, comment_len = 0, dont_space = TRUE;
char start_comment[MAX_SIZE];
char prog_name[FNAME_SIZE];
char tmp_str[FNAME_SIZE];
int dont_stop = FALSE;
static char last_comment[MAX_SIZE];

/* options for the different parsing methods */
int global_opts[2][OPT_NUMBER];
int doc_extract = DOC_NONE, cfg_switch = 7;

/* variables for keeping track of things */
char errout_filename[MID_SIZE];
int start_block = FALSE, files_parsed = 0, total_out = 0;
int line_count = 0;

/* default font values */
char ft_title[3] = "C";
char ft_comment[3] = "CO";
char ft_name[3] = "B";
char ft_plist[3] = "R";

/* add a new substitution macro to the substitution list */
void
add_subst(type, select, f_str, t_str)
  int type, select;
  char *f_str, *t_str;
{
  P_SUBST subst_tmp;

  /* check for duplicates */
  for (subst_tmp = subst_list;
       subst_tmp != NULL;
       subst_tmp = subst_tmp->next) {
    if ((subst_tmp->submode == type) &&
	(subst_tmp->usewhen == select) &&
	(strcmp(subst_tmp->from_str, f_str) == 0)) {

      /* can exit routine for complete matches */
      if (strcmp(subst_tmp->to_str, t_str) == 0) {
	return;
      }

      /* otherwise just adjust the replacement */
      free(subst_tmp->to_str);
      if ((subst_tmp->to_str =
	   (char *) malloc(sizeof(char) * (1 + strlen(t_str)))) == NULL) {
	fprintf(stderr, "Serious Error: Memory Allocation Failure\n");
	exit(1);
      }
      strcpy(subst_tmp->to_str, t_str);      
      return;

    }
  }

  /* allocate the memory space */
  if ((subst_tmp = (P_SUBST) malloc(sizeof(S_SUBST))) == NULL) {
    fprintf(stderr, "Serious Error: Memory Allocation Failure\n");
    exit(1);
  }
  if (subst_list == NULL) {
    subst_list = subst_tmp;
  }
  if ((subst_tmp->from_str =
       (char *) malloc(sizeof(char) * (1 + strlen(f_str)))) == NULL) {
    fprintf(stderr, "Serious Error: Memory Allocation Failure\n");
    exit(1);
  }
  if ((subst_tmp->to_str =
       (char *) malloc(sizeof(char) * (1 + strlen(t_str)))) == NULL) {
    fprintf(stderr, "Serious Error: Memory Allocation Failure\n");
    exit(1);
  }

  /* now record the information */
  subst_tmp->submode = type;
  subst_tmp->usewhen = select;
  strcpy(subst_tmp->from_str, f_str);
  strcpy(subst_tmp->to_str, t_str);

  /* place the new link at the end of the chain */
  subst_tmp->next = NULL;
  if (last_subst != NULL) {
    last_subst->next = subst_tmp;
  }
  last_subst = subst_tmp;
}

/* add a new macro to the macro list */
void
add_macro(select, str)
  int select;
  char *str;
{
  P_MACRO macro_tmp;

  /* check for duplicate entries */
  for (macro_tmp = macro_list;
       macro_tmp != NULL;
       macro_tmp = macro_tmp->next) {
    /* no need to add something already there */
    if (strcmp(macro_tmp->m_str, str) == 0) {
      /* check for selection change */
      if (macro_tmp->usewhen != select) {
	macro_tmp->usewhen = 2;
      }
      return;
    }
  }

  /* allocate the space */
  if ((macro_tmp = (P_MACRO) malloc(sizeof(S_MACRO))) == NULL) {
    fprintf(stderr, "Serious Error: Memory Allocation Failure\n");
    exit(1);
  }
  if (macro_list == NULL) {
    macro_list = macro_tmp;
  }
  if ((macro_tmp->m_str =
       (char *) malloc(sizeof(char) * (strlen(str) + 1))) == NULL) {
    fprintf(stderr, "Serious Error: Memory Allocation Failure\n");
    exit(1);
  }
  strcpy(macro_tmp->m_str, str);
  macro_tmp->usewhen = select;
  macro_tmp->next = NULL;
  if (last_macro != NULL) {
    last_macro->next = macro_tmp;
  }
  last_macro = macro_tmp;
}

/* return TRUE if the macro definition is in the string */
static int
macro_match(macro, str)
  char *macro, *str;
{
  static int deflen = -1;
  int i, len, hold = FALSE;

  /* initialize */
  if (deflen == -1) {
    deflen = strlen(DEF_LEADER);
  }

  /* check it */
  if (strncmp(macro, DEF_LEADER, deflen) == 0) {
    len = strlen(str);
    for (i = 0; i < len; i++) {
      if (macro[i + deflen] != str[i]) break;
    }
    if ((i == len) &&
	((macro[i + deflen] == '=') ||
	 (macro[i + deflen] == '\0'))) {
      hold = TRUE;
    }
  }

  return(hold);
}

/* if the matching definition is in the list, remove it */
int
removed_macro(select, str)
  int select;
  char *str;
{
  P_MACRO macro_ptr, macro_tmp;
  int hold = FALSE;

  /* go through the list */
  if ((macro_ptr = macro_list) != NULL) {

    /* check the beginning element */
    if (macro_match(macro_ptr->m_str, str)) {
      if (macro_ptr->usewhen != select) {
	/* just return */
	return(FALSE);
      } else {
	macro_list = macro_list->next;
	macro_ptr->next = NULL;
	free(macro_ptr->m_str);
	free(macro_ptr);
	hold = TRUE;
      }
    }

    /* check the rest */
    macro_tmp = macro_ptr->next;
    while (hold == FALSE && macro_tmp != NULL) {
      if (macro_match(macro_tmp->m_str, str)) {
	if (macro_ptr->usewhen != select) {
	  /* just return */
	  return(FALSE);
	} else {
	  macro_ptr->next = macro_tmp->next;
	  macro_tmp->next = NULL;
	  free(macro_tmp->m_str);
	  free(macro_tmp);
	  hold = TRUE;
	}
      }

      /* onto the next */
      macro_ptr = macro_tmp;
      macro_tmp = macro_tmp->next;
    }
  }

  /* now assign the last_macro value and get back to work */
  if (hold == TRUE) {
    for (last_macro = macro_list;
	 (last_macro != NULL) &&
	 (last_macro->next != NULL);
	 last_macro = last_macro->next) ;
  }
  return(hold);
}

/* compare two strings considering cases the same */
int
str_test (s1, s2)
  char *s1, *s2;
{
  int ch1, ch2;

  for (; *s1 != '\0'; s1++, s2++) {
    ch1 = (islower(*s1) ? toupper(*s1) : *s1);
    ch2 = (islower(*s2) ? toupper(*s2) : *s2);
    if (ch1 != ch2) return(ch1 - ch2);
  }
  return(*s1 - *s2);
}

/* sort the list of functions and their prototypes */
void
sort_proto()
{
  P_PROTO p2_ptr;
  int done = FALSE;

  /* all done? */
  if ((proto_list == NULL) || (proto_list->next == NULL)) return;

  /* go until done sorting */
  while (done == FALSE) {

    /* initialize */
    proto_next = proto_list;
    done = TRUE;

    /* test for initial swap */
    if ( str_test( proto_next->name, (proto_next->next)->name ) > 0) {
      proto_list = proto_next->next;
      proto_next->next = proto_list->next;
      proto_list->next = proto_next;
      proto_next = proto_list;
    }
    
    /* sort remaining linked list */
    for (; (proto_next->next)->next != NULL; proto_next = proto_next->next) {
      p2_ptr = (proto_next->next)->next;
      if ( str_test( (proto_next->next)->name, p2_ptr->name ) > 0) {

	/* swap locations in list */
	(proto_next->next)->next = p2_ptr->next;
	p2_ptr->next = proto_next->next;
	proto_next->next = p2_ptr;

	/* continue sort */
	done = FALSE;
      }
    }

  }
}

/* remove the top item on the stack */
void
pop_proto()
{
  P_PROTO tmp_proto = proto_list;

  /* done? */
  if (proto_list == NULL) {
    proto_next = NULL;
    return;
  }
  if (proto_list == proto_next) {
    proto_next = NULL;
  }

  /* now do it */
  proto_list = proto_list->next;
  tmp_proto->next = NULL;
  if (tmp_proto->name != NULL) free(tmp_proto->name);
  if (tmp_proto->ftype != NULL) free(tmp_proto->ftype);
  if (tmp_proto->fname != NULL) free(tmp_proto->fname);
  if (tmp_proto->plist != NULL) free(tmp_proto->plist);
  if (tmp_proto->comment != NULL) free(tmp_proto->comment);
  free(tmp_proto);
}

/* initialize the page for troff output, if needed */
void
init_roff(omode)
  int omode;
{
  static int init_done = FALSE;

  /* perform the initialization */
  if (init_done == FALSE) {
    out_str(omode, ".sp 0.5i\n");
    out_str(omode, ".ps 10\n");
    out_str(omode, ".vs 12\n");
    out_str(omode, ".fp 1 ");
    out_str(omode, ft_title);
    out_str(omode, "\n.fp 2 ");
    out_str(omode, ft_comment);
    out_str(omode, "\n.fp 3 ");
    out_str(omode, ft_name);
    out_str(omode, "\n.fp 4 ");
    out_str(omode, ft_plist);
    out_char(omode, '\n');
    init_done = TRUE;
  }
}

/* check preprocessor statement for line numbering changes */
int
preprocessor_check()
{
  int curr_char = '#';
  int i, l, line_value;
  char temp_str[MID_SIZE], t2_str[MID_SIZE];

  /* get the whole line */
  if (fgets(temp_str, MID_SIZE, fpin) != NULL) {
    /* first handle the exit */
    l = strlen(temp_str);
    curr_char = temp_str[l - 1];

    /* now parse things */
    for (i = 0; i < l; i++) {
      if (isdigit(temp_str[i])) {
	if (sscanf(&(temp_str[i]), "%d %s", &line_value,
		   t2_str) == 2) {
	  line_count = line_value;
	  l = strlen(t2_str);
	  if (t2_str[l - 1] == '"') {
	    t2_str[l - 1] = '\0';
	  }
	  if (t2_str[0] == '"') {
	    i = 1;
	  } else {
	    i = 0;
	  }
	  strcpy(errout_filename, &(t2_str[i]));
	}
	break;
      } else if (!isspace(temp_str[i])) {
	if (strncmp(&(temp_str[i]), "line", 4) != 0) break;
	else {
	  i += 3;
	}
      }
    }
  }
  return(curr_char);
}

/* read and store all characters prior to end comment indicator */
static void
get_comment(tagon, head_str, cmt_str)
  int tagon;
  char head_str[], cmt_str[];
{
  int curr_ch;
  int prev_ch = ' ';

  /* keep going until scan is complete */
  if (tagon == TRUE) {
    if (head_str[0] == '\0') {
      /* catenate side by side comments */
      comment_len -= 2;
    } else {
      /* insert intermediary white space and comment */
      for (curr_ch = 0;
	   head_str[curr_ch] != '\0';
	   curr_ch++) {
	cmt_str[comment_len++] = head_str[curr_ch];
      }
      cmt_str[comment_len++] = '/';
      cmt_str[comment_len++] = '*';
    }
  } else {
    comment_len = 0;
  }
  while ((curr_ch = getc(fpin)) != EOF) {

    /* test for end of comment */
    if ((curr_ch == '/') && (prev_ch == '*')) {
      cmt_str[comment_len++] = '/';
      break;
    }

    /* store it */
    cmt_str[comment_len++] = prev_ch = curr_ch;
    if (curr_ch == '\n') line_count++;
  }

  /* close storage */
  cmt_str[comment_len] = '\0';
}

/* structure to hold definitions */
typedef struct s_param {
  char *name;		/* the name of the variable */
  char *desc;		/* description string */
  struct s_param *next;	/* pointer to the next element */
} S_PARAM, *P_PARAM;
P_PARAM param_list;

/* take all of the variables between the parenthesis and break them down */
static int
fill_param(str)
  char *str;
{
  int paren_loc, count, i, in_word = 1, len = strlen(str);
  P_PARAM last_param = NULL, param_tmp;
  char tempc[MID_SIZE];

  /* initialize the list */
  param_list = NULL;
  paren_loc = 0;
  count = 0;
  tempc[0] = '\0';
  for (i = 1; i < len; i++) {

    /* check for the end */
    if (str[i] == ')') {
      paren_loc = i;
      break;
    }

    /* find a variable name; check for separators */
    if (str[i] == ',') {

      /* check current information */
      if (in_word == 1) {
	syntax_err("empty variable list encountered");
	return(-1);
      }
      in_word = 1;

      /* store the name */
      tempc[count] = '\0';
      count = 0;
      if ((param_tmp = (P_PARAM) malloc(sizeof(S_PARAM))) == NULL) {
	fprintf(stderr, "Serious Error: Memory Allocation Failure\n");
	exit(1);
      }
      if ((param_tmp->name =
	   (char *) malloc(sizeof(char) * (strlen(tempc) + 1))) == NULL) {
	fprintf(stderr, "Serious Error: Out of memory\n");
	exit(1);
      }
      strcpy(param_tmp->name, tempc);
      param_tmp->next = NULL;
      param_tmp->desc = NULL;
      if (param_list == NULL) {
	param_list = param_tmp;
	last_param = param_list;
      } else {
	last_param->next = param_tmp;
	last_param = param_tmp;
      }

    } else if (str[i] == ' ') {
      if (in_word == 2) {
	in_word = 0;
      }
    } else if (id_char(str[i])) {
      if (in_word == 0) {
	syntax_err("invalid parameter list encountered");
	return(-1);
      }
      in_word = 2;
      tempc[count++] = str[i];
    } else {
      syntax_err("unable to parse the complex C code");
      return(-1);
    }

  }

  /* now store the last name if any was found */
  if (count != 0) {

    /* store the name */
    tempc[count] = '\0';
    if ((param_tmp = (P_PARAM) malloc(sizeof(S_PARAM))) == NULL) {
      fprintf(stderr, "Serious Error: Memory Allocation Failure\n");
      exit(1);
    }
    if ((param_tmp->name =
	 (char *) malloc(sizeof(char) * (strlen(tempc) + 1))) == NULL) {
      fprintf(stderr, "Serious Error: Out of memory\n");
      exit(1);
    }
    strcpy(param_tmp->name, tempc);
    param_tmp->desc = NULL;
    param_tmp->next = NULL;
    if (param_list == NULL) {
      param_list = param_tmp;
      last_param = param_list;
    } else {
      last_param->next = param_tmp;
      last_param = param_tmp;
    }
  }
  return(paren_loc);
}

/* remove any preceding and trailing spaces */
void
trim_str(str)
  char *str;
{
  register int i = 0, j = 0;
  int last_space = -1;

  /* trim leading spaces */
  while (isspace(str[i])) i++;

  /* shift string */
  while (1) {
    if ((str[j] = str[i++]) == '\0') {
      break;
    } else if (isspace(str[j])) {
      last_space = j;
    } else {
      last_space = -1;
    }
    j++;
  }

  /* now remove any trailing spaces */
  if (last_space != -1) {
    str[last_space] = '\0';
  }
}

/* just go through and send out the entire parameter list */
static int
oldc_output()
{
  int count = 0, lcount = 0;
  P_PARAM param_tmp;

  /* first test for the simple case */
  if (param_list == NULL) {
    strcpy(dummy_str, "void");
    return(4);
  }
  dummy_str[0] = '\0';

  /* go through the list */
  while (param_list != NULL) {

    /* add in separators */
    if (count++ > 0) {

      strcat(dummy_str, ", ");
      lcount += 2;

    }

    /* check the type */
    if (param_list->desc != NULL) {
      trim_str(param_list->desc);
      strcat(dummy_str, param_list->desc);
      lcount += strlen(param_list->desc);
      free(param_list->desc);
    } else {
      if (get_option(OPT_NONAMES)) {
	strcat(dummy_str, "int");
	lcount += 3;
      } else if (strcmp("int", param_list->name)) {
	strcat(dummy_str, "int ");
	strcat(dummy_str, param_list->name);
	lcount += 4 + strlen(param_list->name);
      } else {
	strcat(dummy_str, param_list->name);
	lcount += strlen(param_list->name);
      }
    }
    free(param_list->name);

    /* clean up storage */
    param_tmp = param_list;
    param_list = param_list->next;
    param_tmp->next = NULL;
    free(param_tmp);

  }

  /* finish it up */
  return(lcount);
}

/* return the proper element */
static P_PARAM
var_match(str)
  char *str;
{
  int i, len, len2;
  P_PARAM p_hold;

  /* go through all of the items and find a match */
  len2 = strlen(str);
  p_hold = param_list;
  while (p_hold != NULL) {

    /* check for a match */
    len = strlen(p_hold->name);
    for (i = 0; i < len2 - len + 1; i++) {
      /* check for the match */
      if (strncmp(str + i, p_hold->name, len) == 0) {
	/* found a match */
	if (!id_char(str[i + len])) {
	  return(p_hold);
	}
      } else if (id_char(str[i])) {
	/* definite non-match */
	i = len2 - len + 1;
      }
    }

    /* go to the next element */
    p_hold = p_hold->next;
  }

  return(p_hold);
}

/* remove the variable name from the given type and variable combo */
static void
kill_variable(str)
  char *str;
{
  register int i;

  /* no need to breakdown this baby */
  if (strcmp(str, "...") == 0) return;

  /* search for the variable name */
  trim_str(str);
  for (i = strlen(str) - 1; i >= 0; i--) {

    /* found it */
    if (id_char(str[i])) {

      /* clean it up */
      while ((i >= 0) &&
	     id_char(str[i])) {
	str[i--] = ' ';
      }
      i++;
      trim_str(str + i);
      if (i == 0) {
	strcpy(str, "int");
      } else {
	trim_str(str);
      }
      break;

    }

  }
}

/* separate and rebuild the ANSI format list */
static int
newc_parse (i_str)
  char *i_str;
{
  char hold_str[MID_SIZE], hold2_str[MID_SIZE];
  P_SUBST sub_tmp;
  int depth, cnt = 0, len, i;

  /* start the process */
  dummy_str[0] = '\0';
  do {

    /* move along the input string */
    switch (*i_str) {
    case ',':
    case '\0':
      /* finish off this item */
      if (cnt == 0) break;
      hold_str[cnt] = '\0';

      /* search for matches among the replacement code */
      for (sub_tmp = subst_list;
	   sub_tmp != NULL;
	   sub_tmp = sub_tmp->next) {

	/* check for usage */
	if ((sub_tmp->usewhen != 2) &&
	    (sub_tmp->usewhen != (doc_extract != DOC_NONE))) continue;

	/* check it */
	len = strlen(sub_tmp->from_str);
	switch (sub_tmp->submode) {
	case SUBST_FULL:
	  /* the full string needs replacing? */
	  if (strcmp(hold_str, sub_tmp->from_str) == 0) {
	    strcpy(hold_str, sub_tmp->to_str);
	  }
	  break;
	case SUBST_TYPE:
	  /* the type only needs replacing? */
	  if (strncmp(hold_str, sub_tmp->from_str, len) == 0) {
	    strcpy(hold2_str, hold_str + len);
	    strcpy(hold_str, sub_tmp->to_str);
	    strcat(hold_str, hold2_str);
	  }
	  break;
	case SUBST_NAME:
	  /* the variable only needs replacing? WHY!? */
	  if ((cnt > len) &&
	      (strcmp(hold_str + cnt - len, sub_tmp->from_str) == 0)) {
	    hold_str[cnt - len] = '\0';
	    strcat(hold_str, sub_tmp->to_str);
	  }
	  break;
	}
      }

      /* check for removal of register type */
      if (strncmp(hold_str, "register ", 9) == 0) {
	for (i = 0; i < 9; i++) {
	  hold_str[i] = ' ';
	}
      }

      /* do we clean it? */
      if (get_option(OPT_NONAMES)) {
	kill_variable(hold_str);
      } else {
	trim_str(hold_str);
      }

      /* append to output */
      if (dummy_str[0] != '\0') {
	strcat(dummy_str, ", ");
      }
      strcat(dummy_str, hold_str);
      cnt = 0;
      break;
    case '{':
      /* collect up all of the type declaration */
      depth = -1;
      do {
	hold_str[cnt++] = *i_str;
	switch (*i_str) {
	case '\0':
	  syntax_err("unexpected end of parameters");
	  return(-1);
	case '{':
	  depth++;
	  break;
	case '}':
	  depth--;
	  break;
	default:
	  /* ignore me */
	  break;
	}
	i_str++;
      } while ((depth > 0) ||
	       (*i_str != '}'));
      hold_str[cnt++] = *i_str;
      break;
    case ' ':
      if (cnt == 0) break;
    default:
      /* just copy it */
      hold_str[cnt++] = *i_str;
    }

  } while (*(i_str++) != '\0');

  /* give back the length */
  return(strlen(dummy_str));
}

/* extract all of the parameters using old style format */
static int
oldc_parse(str)
  char *str;
{
  int depth = 0, last_char = ')';
  int start, count2, in_var;
  P_PARAM p_tmp;
  P_SUBST sub_tmp;

  char type_name[MID_SIZE], var_name[MID_SIZE];
  char oldvar_name[MID_SIZE], tempc[MID_SIZE];

  /* build the parameter list */
  if ((start = fill_param(str)) == -1) {
    /* clean up and exit on error */
    while (param_list != NULL) {
      p_tmp = param_list;
      param_list = param_list->next;
      p_tmp->next = NULL;
      free(p_tmp);
    }
    return(-1);
  }
  if (str[start] != ')') {
    strcpy(dummy_str, "void");
    return(4);
  }

  /* begin with non-space character */
  for (str += start + 1; *str == ' '; str++) ;
  count2 = 0;
  in_var = FALSE;
  type_name[0] = '\0';

  /* now go through the entire structure */
  for (; *str != '\0'; str++) {

    switch (*str) {
    case ' ':
      /* add on to variable or append to type definition */
      if (in_var == TRUE) {
	var_name[count2++] = ' ';
	break;
      }
      break;
    case '{':
      /* must be a struct declaration */
      if ((depth != 0) ||
	  (in_var == TRUE)) {
	syntax_err("unexpected left brace encountered");
	return(-1);
      }
      depth = -1;
      if (var_name[count2] != ' ') {
	var_name[count2++] = ' ';
      }
      do {
	var_name[count2++] = *str;
	switch (*str) {
	case '\0':
	  syntax_err("unable to parse the complex C structure");
	  return(-1);
	case '{':
	  depth++;
	  break;
	case '}':
	  depth--;
	  break;
	default:
	  /* ignore me */
	  break;
	}
	str++;
      } while ((depth > 0) ||
	       (*str != '}'));
      goto saw_space;
      break;
    case ',':
      /* just add on if between parenthesis */
      if (depth != 0) {
	var_name[count2++] = ',';
	break;
      }
    case ';':
      /* found end of declaration? */
      var_name[count2] = '\0';
      if ((depth != 0) || (type_name[0] == '\0')) {
	syntax_err("unable to parse the complex C structure");
	return(-1);
      }
      if (var_name[0] == '\0') {
	syntax_err("unable to parse the complex C structure");
	return(-1);
      }

      /* build the proper string */
      trim_str(type_name);
      strcpy(tempc, type_name);
      strcat(tempc, " ");
      trim_str(var_name);
      strcpy(oldvar_name, var_name);
      strcat(tempc, var_name);

      /* search for matches among the replacement code */
      for (sub_tmp = subst_list;
	   sub_tmp != NULL;
	   sub_tmp = sub_tmp->next) {
	/* check it */
	switch (sub_tmp->submode) {
	case SUBST_FULL:
	  /* the full string needs replacing? */
	  if (strcmp(tempc, sub_tmp->from_str) == 0) {
	    strcpy(tempc, sub_tmp->to_str);
	  }
	  break;
	case SUBST_TYPE:
	  /* the type only needs replacing? */
	  if (strcmp(type_name, sub_tmp->from_str) == 0) {
	    strcpy(tempc, sub_tmp->to_str);
	    strcpy(type_name, sub_tmp->to_str);
	    strcat(tempc, " ");
	    strcat(tempc, var_name);
	  }
	  break;
	case SUBST_NAME:
	  /* the variable only needs replacing? WHY!? */
	  if (strcmp(var_name, sub_tmp->from_str) == 0) {
	    strcpy(tempc, type_name);
	    strcat(tempc, " ");
	    strcat(tempc, sub_tmp->to_str);
	    strcpy(var_name, sub_tmp->to_str);
	  }
	  break;
	}
      }

      /* now find the proper variable name */
      if ((p_tmp = var_match(oldvar_name)) == NULL) {
	char temp_str[MID_SIZE];
	sprintf(temp_str, "could not place variable %s properly",
		oldvar_name);
	syntax_err(temp_str);
	return(-1);
      }

      /* now check for variable name removal */
      if (get_option(OPT_NONAMES)) {
	kill_variable(tempc);
      }

      /* set aside space */
      if ((p_tmp->desc =
	   (char *) malloc(sizeof(char) * (strlen(tempc) + 1))) == NULL) {
	fprintf(stderr, "Serious Memory Allocation Error\n");
	exit(1);
      }

      /* now stow it */
      strcpy(p_tmp->desc, tempc);

      /* reset properly */
      count2 = 0;
      var_name[0] = '\0';
      if (*str == ';') {
	in_var = FALSE;
	type_name[0] = '\0';
      }
      break;
    case '*':
      /* just append on, must now be in variable */
      if ((last_char == ' ') && (in_var == FALSE)) {
	in_var = TRUE;
	goto saw_space;
      } else if (in_var == FALSE) {
	/* must push out type properly */
	var_name[count2] = '\0';
	count2 = 0;
	if ((type_name[0] != '\0') &&
	    (type_name[strlen(type_name) - 1] != ' '))  {
	  strcat(type_name, " ");
	}
	if ((type_name[0] != '\0') ||
	    (strcmp(var_name, "register") != 0)) {
	  strcat(type_name, var_name);
	}
	var_name[0] = '\0';
      }
      in_var = TRUE;
      var_name[count2++] = '*';
      break;
    case '(':
    case '[':
      /* count depth, assume balanced for both */
      depth++;
      if ((last_char == ' ') &&
	  (in_var == FALSE)) {
	in_var = TRUE;
	goto saw_space;
      }
      in_var = TRUE;
      var_name[count2++] = *str;
      break;
    case ')':
    case ']':
      /* check depth */
      depth--;
      if (depth < 0) {
	syntax_err("misbalanced parenthesis encountered");
	return(-1);
      }
      if (last_char == ' ') goto saw_space;
      var_name[count2++] = *str;
      break;
    default:
      if (!id_char(*str) && (depth == 0)) {
	syntax_err("unknown variable structure encountered");
	return(-1);
      }
      /* check for just seeing space */
      if (last_char == ' ' && in_var == FALSE) {

      saw_space:
	var_name[count2] = '\0';
	count2 = 0;
	if ((type_name[0] != '\0') &&
	    (type_name[strlen(type_name) - 1] != ' '))  {
	  strcat(type_name, " ");
	}
	if ((type_name[0] != '\0') ||
	    (strcmp(var_name, "register") != 0)) {
	  strcat(type_name, var_name);
	}
	var_name[0] = '\0';

      }
      var_name[count2++] = *str;
      break;
    }

    /* save the last character */
    last_char = *str;
  }

  /* now send everything out */
  return(oldc_output());
}

/* function to determine if the parameter list is K&R, ANSI or empty */
static int
diverge_style(str, len)
  char *str;
  int len;
{
  int void_fnd = FALSE, nspc_fnd = FALSE, in_word = 1;

  /* perform the first simple test */
  if (str[len - 1] == ';') return(2);

  /* should have a right parenthesis now  */
  if (str[len - 1] != ')') {
    fprintf(stderr, "Expecting right paren in file %s\n", file_name);
    return(1);
  }

  /* now check to see if it only lists all variables */
  for (str++; *str != '\0'; str++) {

    /* check for separators */
    switch (*str) {
    case ',':
      /* variable separator */
      if (in_word == 1) {
	fprintf(stderr, "Empty variable list in file %s", file_name);
	return(1);
      }
      nspc_fnd = TRUE;
      in_word = 1;
      break;
    case ' ':
      /* word separator */
      if (in_word == 2) {
	in_word = 0;
      }
      break;
    case ')':
      if (*(str + 1) != '\0') {
	return(1);
      }
      break;
    default:
      /* check for variable name */
      if ((void_fnd == FALSE) &&
	  (strncmp(str, "void", 4) == 0)) {
	void_fnd = TRUE;
	str += 3;
	in_word = 2;
      } else if (id_char(*str)) {
	if (in_word == 0) {
	  return(1);
	}
	in_word = 2;
	nspc_fnd = TRUE;
      } else {
	return(1);
      }
      break;
    }
  }
  return((nspc_fnd == TRUE)? 2 : 0);
}

/* return length of function name... and store it in space provide */
static int
find_name(out_name, in_desc, desc_len)
  char *out_name, *in_desc;
  int desc_len;
{
  int pos, count = 0;

  /* find the end of the name */
  for (pos = desc_len; pos > 0; pos--) {
    if (id_char(in_desc[pos])) break;
  }

  /* find the length of the name */
  while ((pos > 0) &&
	 (id_char(in_desc[pos]))) {
    count++;
    pos--;
  }

  /* copy it */
  if (!id_char(in_desc[pos])) {
    pos++;
  } else {
    count++;
  }
  strncpy(out_name, in_desc + pos, count);
  out_name[count] = '\0';
  return(count);
}

/* function to extract the function prototype from preceding characters */
static void
parse_func()
{
  P_SUBST sub_tmp;
  P_PROTO tmp_proto;
  int count, valid = TRUE, depth = 0, done = FALSE;
  int sep_point = 0, typelen, cmpstt, len, dummy_len = 0;
  char *func_declare, *func_list, name_space[MID_SIZE];

  /* clean up the input string */
  trim_str(code_info);
  len = strlen(code_info);

  /* now go backwards and find the first occurance */
  /* of a right parenthesis without a '[', ',' or ';' after */
  for (count = len - 1; done == FALSE && count > 0; count--) {

    /* check for a select group of characters */
    switch (code_info[count]) {
    case ' ':
      /* don't change validity for spaces */
      break;
    case ')':
      /* check if it is a good match */
      depth++;
      if (depth == 1 && valid == TRUE) {
	done = TRUE;
      }
      break;
    case '(':
      /* keep proper track of depth */
      depth--;
      if (depth < 0) {
	syntax_err("too many left parenthesis encountered");
	break;
      }

      /* watch for function pointers */
      valid = FALSE;
      break;
    case '[':
    case ';':
    case ',':
      /* any paranthesis before this is invalid */
      valid = FALSE;
      break;
    case '=':
      /* there should be no equal signs anywhere around this */
      valid = FALSE;
      count = 0;
      break;
    default:
      /* it can now be a valid parenthesis */
      valid = TRUE;
      break;
    }
  }

  /* get out if no function type was found */
  if (done != TRUE) return;

  /* now find the separation point for the function */
  done = FALSE;
  for (; done == FALSE && count > 0; count--) {
    valid = code_info[count];
    switch (valid) {
    case ' ':
      break;
    case ')':
      /* go deeper */
      depth++;
      break;
    case '(':
      /* rise higher */
      depth--;
      if (depth == 0) {
	sep_point = count;
	done = TRUE;
      }
      break;
    default:
      break;
    }
  }

  /* confirm separation point */
  if (done == FALSE) {
    syntax_err("too many right parenthesis encountered");
    return;
  }

  /* now find the start of the function declaration */
  for (; count > 0; count--) {
    if ((code_info[count] == ';') ||
	(code_info[count] == '}')) break;
    if (code_info[count] == '(') {
      syntax_err("unexpected left parenthesis found while parsing parameters");
      return;
    }
  }
  if ((code_info[count] == ';') ||
      (code_info[count] == '}')) count++;

  /* gain space */
  if ((func_list = (char *) malloc(sizeof(char) * (len - sep_point + 1)))
      == NULL) {
    fprintf(stderr, "Serious Error: Malloc failed\n");
    exit(1);
  }
  if ((func_declare =
       (char *) malloc(sizeof(char) * (sep_point - count + 1))) == NULL) {
    fprintf(stderr, "Serious Error: Malloc failed\n");
    exit(1);
  }

  /* now assign the locations */
  strcpy(func_list, &(code_info[sep_point]));
  strncpy(func_declare, &(code_info[count]), sep_point - count);
  func_declare[sep_point - count] = '\0';
  trim_str(func_list);
  trim_str(func_declare);

  /* find just the function name */
  len = strlen(func_declare);
  count = find_name(name_space, func_declare, len);

  /* just leave if there is no function */
  if (count == 0) {
    free(func_list);
    free(func_declare);
    return;
  }

  /* check for statics which shouldn't be shown */
  if (get_option(OPT_STATICMODE) != ANY_STATICS) {
    if (!strncmp(func_declare, "static ", 7) ==
	(get_option(OPT_STATICMODE) == NO_STATICS)) {
      last_comment[0] = '\0';
      comment_len = 0;
      free(func_list);
      free(func_declare);
      return;
    }
  }

  /* declare storage space for the function */
  if ((tmp_proto = (P_PROTO) malloc(sizeof(S_PROTO))) == NULL) {
    fprintf(stderr, "Memory allocation failure\n");
    exit(1);
  }
  tmp_proto->next = NULL;
  tmp_proto->name = NULL;
  tmp_proto->ftype = NULL;
  tmp_proto->fname = NULL;
  tmp_proto->plist = NULL;
  tmp_proto->comment = NULL;

  /* place it in the list */
  if ((proto_next == NULL) || (proto_list == NULL)) {
    proto_next = proto_list = tmp_proto;
  } else {
    proto_next->next = tmp_proto;
    proto_next = tmp_proto;
  }
  if ((proto_next->fname =
       (char *) malloc(sizeof(char) * (strlen(file_name) + 2))) == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }
  strcpy(proto_next->fname, file_name);

  /* now copy any preceding comments if desired */
  if (get_option(OPT_COMMENTS) &&
      (last_comment[0] != '\0')) {
    if ((proto_next->comment =
	 (char *) malloc(sizeof(char) * (comment_len + 5))) == NULL) {
      fprintf(stderr, "Memory allocation error\n");
      exit(1);
    }
    strcpy(proto_next->comment, "/*");
    strcat(proto_next->comment, last_comment);
    last_comment[0] = '\0';
    comment_len = 0;
  } else {
    proto_next->comment = NULL;
  }

  /* check for the conversion of the type declaration */
  if (count != len) {
    if (strncmp("extern ", func_declare, 7) == 0) {
      cmpstt = 7;
    } else {
      cmpstt = 0;
    }
    for (sub_tmp = subst_list;
	 sub_tmp != NULL;
	 sub_tmp = sub_tmp->next) {
      /* check it */
      if (sub_tmp->submode == SUBST_TYPE) {

	/* compare */
	typelen = strlen(sub_tmp->from_str);
	if (strncmp(func_declare + cmpstt, sub_tmp->from_str, typelen) == 0) {
	  strcpy(tmp_str, sub_tmp->to_str);
	  strcat(tmp_str, func_declare + cmpstt + typelen);
	  strcpy(func_declare, tmp_str);
	  len += (strlen(sub_tmp->to_str) - typelen - cmpstt);
	}

      }
    }
  }

  /* output extern if desired */
  if (get_option(OPT_EXTERNS) &&
      strncmp("extern ", func_declare, 7) != 0) {
    strcpy(dummy_str, "extern ");
    dummy_len = 7;
  } else {
    dummy_str[0] = '\0';
    dummy_len = 0;
  }

  /* now check if the function type is an integer */
  if (count == len) {
    strcat(dummy_str, "int ");
    strcat(dummy_str, func_declare);
    dummy_len += 4 + len;
  } else {
    strcat(dummy_str, func_declare);
    dummy_len += len;
  }

  /* store the function header */
  if ((proto_next->ftype =
       (char *) malloc(sizeof(char) * (dummy_len + 2))) == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }
  strcpy(proto_next->ftype, dummy_str);

  /* the function string */
  if ((proto_next->name =
       (char *) malloc(sizeof(char) * (count + 2))) == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }
  strcpy(proto_next->name, name_space);

  /* now output empty list amount */
  len = strlen(func_list);
  switch (diverge_style(func_list, len)) {
  case 0:
    /* empty or "void" parameter list */
    strcpy(dummy_str, "void");
    dummy_len = 4;
    break;
  case 1:
    /* ANSI C format! -- remove trailing parenthesis */
    func_list[--len] = '\0';
    if ((dummy_len = newc_parse(func_list + 1)) == -1) {
      free(func_list);
      free(func_declare);
      pop_proto();
      return;
    }
    break;
  default:
    /* K&R C format */
    if ((dummy_len = oldc_parse(func_list)) == -1) {
      free(func_list);
      free(func_declare);
      pop_proto();
      return;
    }
    break;
  }

  /* store it */
  if ((proto_next->plist =
       (char *) malloc(sizeof(char) * (dummy_len + 2))) == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(1);
  }
  strcpy(proto_next->plist, dummy_str);

  /* replace space */
  free(func_list);
  free(func_declare);
}

/* go through the file extracting functions */
void
parse_file()
{
  int curr_char, count = 0, was_comment = FALSE, may_flush = FALSE;
  int depth = 0, num_comment = 0, temp_count = 0, temp_lines = 0;
  char temp_list[MID_SIZE];
  int prev_char = '\n', old_prev_real = ' ', prev_real = ' ';

  /* go to it */
  start_comment[0] = '\0';
  last_comment[0] = '\0';
  temp_list[0] = '\0';
  while ((curr_char = getc(fpin)) != EOF) {

    /* check depth first */
    if (curr_char == '\n') line_count++;
    if (depth == 0) {

      /* process the characters */
      if (curr_char == '#' && prev_char == '\n') {

	/* nuke any preprocessor statements */
	was_comment = FALSE;
	prev_char = '#';
	curr_char = preprocessor_check();

      } else if (curr_char == '"') {

	/* nuke string quotes -- must be on same line */
	prev_char = curr_char;
	count = 0;
	while (!feof(fpin)) {
	  if ((curr_char = getc(fpin)) == '"') {
	    break;
	  } else if (curr_char == '\n') {
	    syntax_err("unexpected newline found in string");
	    line_count++;
	  } else if (curr_char == '\\') {
	    prev_char = curr_char;
	    curr_char = getc(fpin);
	  }
	  prev_char = curr_char;
	}

      } else if (curr_char == '\'') {

	/* nuke character quotes -- must be on same line */
	count = 0;
	prev_char = curr_char;
	while (!feof(fpin)) {
	  if ((curr_char = getc(fpin)) == '\'') {
	    break;
	  } else if (curr_char == '\n') {
	    syntax_err("unexpected newline in character constant");
	    line_count++;
	  } else if (curr_char == '\\') {
	    prev_char = curr_char;
	    curr_char = getc(fpin);
	  }
	  prev_char = curr_char;
	}

      } else if (curr_char == '*' && prev_char == '/') {

	/* clear out commments; treat as spaces */
	if (count > 0) count--;
	if (was_comment == FALSE) {
	  num_comment++;
	  temp_count = 0;
	} else {
	  temp_count--;
	  if ((get_option(OPT_SINGLECOMMENTS) == TRUE) &&
	      (temp_count > 0)) {
	    num_comment++;
	    was_comment = FALSE;
	    temp_count = 0;
	  }
	}
	temp_list[temp_count] = '\0';
	curr_char = ' ';
	if (get_option(OPT_COMMENTS) ||
	    ((num_comment == 1) &&
	     get_option(OPT_FIRSTCOMMENT))) {

	  /* process it */
	  if (num_comment == 1) {
	    get_comment(was_comment, temp_list, start_comment);
	  } else {
	    get_comment(was_comment, temp_list, last_comment);
	  }
	  was_comment = TRUE;
	  temp_lines = 0;
	  temp_count = (-1);

	} else {

	  while ((curr_char = getc(fpin)) != EOF) {

	    /* test for end of comment */
	    if (curr_char == '\n') line_count++;
	    if ((curr_char == '/') && (prev_char == '*')) {
	      break;
	    }
	    prev_char = curr_char;
	  }
	  curr_char = ' ';

	}
	prev_real = old_prev_real;

      } else if ((curr_char == '/') && (prev_char == '/')) {

	/* just yank out any C++ comments */
	if (count > 0) count--;
	while ((curr_char = getc(fpin)) != EOF) {

	  /* test for end of comment */
	  if (curr_char == '\n') {
	    line_count++;
	    break;
	  }
	  prev_char = curr_char;

	}
	curr_char = ' ';
	prev_real = old_prev_real;

      } else if (curr_char == '{') {

	/* reset recording process */
	was_comment = FALSE;
	may_flush = FALSE;
	depth++;

	/* now check if it is a function */
	if ((prev_real == ';') ||
	    (prev_real == ')')) {

	  /* found end of function, struct, or union definition */
	  code_info[count] = '\0';
	  parse_func();
	  code_info[count = 0] = '\0';

	} else {
	  code_info[count++] = '{';
	}

      } else if (curr_char == '}') {

	syntax_err("extra right brace encountered");
	break;

      }

      /* tag onto the list */
      if (depth == 0) {

	/* store the characters for later use */
	if (isspace(curr_char) &&
	    (count != 0 && code_info[count - 1] != ' ')) {

	  /* combine any white space into a single space character */
	  code_info[count++] = ' ';

	} else if (!isspace(curr_char)) {

	  /* store any other type directly */
	  if (curr_char != '/') {
	    was_comment = FALSE;
	    temp_count = 0;
	    temp_lines = 0;
	  }
	  code_info[count++] = curr_char;
	  old_prev_real = prev_real;
	  prev_real = curr_char;

	}

	/* count the lead in */
	if (was_comment == TRUE) {

	  /* no multiple newlines */
	  if (curr_char == '\n') {
	    if (temp_lines == 0) {
	      temp_lines = 1;
	    } else {
	      temp_lines = 0;
	      temp_count = -1;
	      was_comment = FALSE;
	    }
	  }

	  /* check beginning */
	  if (temp_count >= 0) {
	    temp_list[temp_count] = curr_char;
	  }
	  temp_count++;

	}

	/* now note what was last encountered */ 
	prev_char = curr_char;

      }

    } else {

      /* keep track of depth and got through code otherwise */
      if (curr_char == '#' && prev_char == '\n') {

	/* nuke any preprocessor statements */
	prev_char = '#';
	curr_char = preprocessor_check();

      } else if (curr_char == '"') {

	/* nuke string quotes -- must be on same line */
	may_flush = TRUE;
	while (!feof(fpin)) {
	  prev_char = curr_char;
	  if ((curr_char = getc(fpin)) == '\n') {
	    syntax_err("unexpected newline found in string");
	    line_count++;
	  } else if (curr_char == '\\') {
	    prev_char = curr_char;
	    curr_char = getc(fpin);
	    continue;
	  } else if (curr_char == '"') {
	    break;
	  }
	};

      } else if (curr_char == '\'') {

	/* nuke character quotes -- must be on same line */
	may_flush = TRUE;
	while (!feof(fpin)) {
	  prev_char = curr_char;
	  if ((curr_char = getc(fpin)) == '\n') {
	    syntax_err("unexpected newline in character constant");
	    line_count++;
	  } else if (curr_char == '\\') {
	    prev_char = curr_char;
	    curr_char = getc(fpin);
	  } else if (curr_char == '\'') {
	    break;
	  } 
	}

      } else if (curr_char == '*' && prev_char == '/') {

	/* nuke comments */
	num_comment++;
	while ((curr_char = getc(fpin)) != EOF) {

	  /* test for end of comment */
	  if (curr_char == '\n') line_count++;
	  if ((curr_char == '/') && (prev_char == '*')) {
	    break;
	  }
	  prev_char = curr_char;

	}
	curr_char = ' ';

      } else if (curr_char == '/' && prev_char == '/') {

	/* nuke C++ comments (don't count it) */
	while ((curr_char = getc(fpin)) != EOF) {

	  /* test for end of comment */
	  if (curr_char == '\n') {
	    line_count++;
	    break;
	  }
	  prev_char = curr_char;

	}
	curr_char = ' ';

      } else if (curr_char == '{') {

	depth++;
	code_info[count++] = curr_char;

      } else if (curr_char == '}') {

	--depth;
	if (count > 0) code_info[count++] = curr_char;
	if (may_flush) {
	  if (depth == 0) may_flush = FALSE;
	  code_info[count = 0] = '\0';
	}

      } else {
	/* just copy on the off chance */
	code_info[count++] = curr_char;
      }

      /* now note what was last encountered */ 
      prev_char = curr_char;

    }

  }
}

