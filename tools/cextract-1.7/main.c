/*
 *
 * The purpose of this program is to extract the function descriptions
 * (prototypes) from C source code.  It also provides for the creation
 * of documentation based on those prototypes.
 *
 * The specific reason for the creation of this program was to
 * provide a method for automatically creating header files to
 * describe all of the functions to be used within a multi-file
 * program.
 *
 * This file makes use of code in the companion files parse.c and io.c.
 *
 * Copyright (c) 1990, 1991, 1992 by Adam Bryant
 *
 * See Copyright notice in the file parse.c or in the manual page.
 *
 */
/*
 * Version history:
 *
 * For Version 0.30:  file parse functions were separated out into a
 *   separate file so that other programs could make use of them.  The
 *   functions descriptions are now stored in dynamic memory and
 *   functions are provided to allow the manipulation of those
 *   descriptions.
 * Version 0.31:  Only a few minor adjustments were made to cextract...
 *   only as much as was needed to build the cextdoc program using the
 *   cproto.[ch] files.
 * Version 0.40:  Totally rewrote the parsing code so that the use of
 *   temporary files is unnecessary.  Instead, two dynamic storage
 *   elements have been created.  Besides being much faster, this
 *   method is also much more elegant than the prior method.
 * Version 0.41:  Switched the NOSETBUFFER to a SETBUFFER flag.
 *   Added in the ability to handle the vararg system to convert it
 *   to the "..." for ANSI C.
 *
 * ... change notes for later versions in parse.c
 *
 * Note on Version 1.0: merged the cextract and cextdoc into one
 *   program, with differential being made based on the command line
 *   or in the program name.  Version sent to comp.sources.reviewed.
 *
 */
#include "xtract.h"
#ifndef VMS
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <types.h>
#include <stat.h>
#endif /* VMS */

/* definition strings to be set when the C preprocessor runs */
#ifndef VAXC
#define CPP_GIVEN "-D__CEXTRACT__"
#define CPP_GIVEN2 "-D__CEXTDOC__"
#else
#define CPP_GIVEN "/define=__CEXTRACT__"
#define CPP_GIVEN2 "/define=__CEXTDOC__"
#endif /* VAXC */

/* output filter from the parsing routines */
void
out_char(type, outch)
  int type, outch;
{
  /* to keep track of where the output list is */
  int tab_width = get_option(OPT_TABWIDTH);
  static P_BUFDATA curbuf[2] = { NULL, NULL };
  static int curpos[2] = { 0, 0 };
  static int tab_count = 0;

  /* split output properly */
  switch (type) {
  case 0:
    /* documentation mode should never get this */
    if (doc_extract != DOC_NONE) {
      fprintf(stderr, "Serious Internal Error: bad out_char() data\n");
      exit(1);
    }
  case 1:
    /* give it to the first one */
    if (tempbuf[type] == NULL) {

      /* start the list */
      if ((tempbuf[type] = (P_BUFDATA) malloc(sizeof(S_BUFDATA))) == NULL) {
	fprintf(stderr, "Memory Allocation Error\n");
	exit(1);
      }
      tempbuf[type]->next = NULL;
      curbuf[type] = tempbuf[type];
    }

    /* have we got enough in this one? */
    if (curpos[type] == MID_SIZE) {

      /* finish it off */
    build_memory:
      curbuf[type]->data[MID_SIZE] = '\0';
      curpos[type] = 0;

      /* start the next one */
      if ((curbuf[type]->next =
	   (P_BUFDATA) malloc(sizeof(S_BUFDATA))) == NULL) {
	fprintf(stderr, "Memory Allocation Error\n");
	exit(1);
      }
      curbuf[type] = curbuf[type]->next;
      curbuf[type]->next = NULL;

    }

    /* special output filter for the documentation mode */
    if (doc_extract != DOC_NONE) {

      /* tab expansion? */
      if (tab_width > 0) {
	if (outch == '\t') {
	  /* fill out tabs properly */
	  while (tab_count++ < tab_width) {

	    /* check that it will fit */
	    if (curpos[type] == MID_SIZE) {
	      tab_count--;
	      goto build_memory;
	    }

	    /* put out the space */
	    curbuf[type]->data[(curpos[type])++] = ' ';

	  }
	  tab_count = 0;
	  break;
	} else if (outch == '\\') {
	  /* account for font changes */
	  tab_count -= 2;
	} else if (outch == '\n') {
	  /* now on a new line */
	  tab_count = 0;
	} else {
	  /* just count */
	  if (++tab_count == tab_width) {
	    tab_count = 0;
	  }
	}
      }

    }

    /* now store it */
    curbuf[type]->data[(curpos[type])++] = outch;
    break;
  case -1:
    /* close everything up */
    if (curbuf[0] != NULL) {
      curbuf[0]->data[curpos[0]] = '\0';
    }
    if (curbuf[1] != NULL) {
      curbuf[1]->data[curpos[1]] = '\0';
    }
    break;
  default:
    /* ain't one of mine */
    fprintf(stderr, "Serious Internal Error: bad data to out_char()\n");
    break;
  }

}

/* show the version of the program */
void
show_version ()
{
  fprintf(stderr, "%s: version %d.%d, Copyright 1992 by Adam Bryant\n",
	  prog_name, VERSION, PATCHLEVEL);
}

/* quickly show a sting and indicate if it is on or off */
static void
quick_show (str, mode)
  char *str;
  int mode;
{
  fprintf(stderr, "\t%s is %s.\n", str, mode ? "on":"off");
}

/* show all of the settings */
void
show_settings ()
{
  P_MACRO macro_temp;
  P_SUBST sub_tmp;

  /* give the version info and current settings */
  show_version();
  fprintf(stderr, "\n  List of current settings:\n");
  fprintf(stderr, "\tC preprocessor in use: %s\n", cpp_prog);
  switch (doc_extract) {
  case DOC_NONE:
    fprintf(stderr, "\tfunction prototype extraction mode.\n");
    if (output_file[0][0] != '\0') {
      fprintf(stderr, "\tthe output file is \"%s\".\n", output_file[0]);
    }
    if (header_string[0] != '\0') {
      fprintf(stderr, "\tthe header string is \"%s\".\n", header_string);
    }
    quick_show("merged ANSI and K&R C output", get_option(OPT_COMPACT));
    if (!get_option(OPT_COMPACT)) {
      quick_show("showing both ANSI and K&R C", get_option(OPT_BOTHUSE));
      if (!get_option(OPT_BOTHUSE)) {
	quick_show("ANSI C output format", get_option(OPT_STDCUSE));
      }
    }
    quick_show("showing prototypes at all times", get_option(OPT_SHOWANYWAY));
    break;
  case DOC_NORMAL:
    fprintf(stderr, "\ttext documentation mode.\n");
    if (output_file[1][0] != '\0') {
      fprintf(stderr, "\tthe output file is \"%s\".\n", output_file[1]);
    }
    break;
  case DOC_ROFF:
    fprintf(stderr, "\ttroff documentation mode.\n");
    if (output_file[1][0] != '\0') {
      fprintf(stderr, "\tthe output file is \"%s\".\n", output_file[1]);
    }
    break;
  default:
    fprintf(stderr, "\tError, unknown mode.\n\n");
    exit(1);
    break;
  }
  if (get_option(OPT_SORTMODE) == SORT_ALL) {
    quick_show("sorting for all functions", TRUE);
  } else if (get_option(OPT_SORTMODE) == SORT_FILE) {
    quick_show("sorting for each file", TRUE);
  } else {
    quick_show("sorting", FALSE);
  }
  if (get_option(OPT_WRAPPOINT) > 0) {
    fprintf(stderr, "\tlong prototype lists wrap at column %d.\n",
	    get_option(OPT_WRAPPOINT));
  } else {
    quick_show("wrapping of long prototypes", FALSE);
  }
  quick_show("separating function name and type", get_option(OPT_TYPEWRAP));
  quick_show("showing first comment in each file",
	     get_option(OPT_FIRSTCOMMENT));
  quick_show("sending file name with first comment",
	     get_option(OPT_PREPEND));
  quick_show("comment capturing", get_option(OPT_COMMENTS));
  if (get_option(OPT_STATICMODE) == ONLY_STATICS) {
    quick_show("only static functions are searched for", TRUE);
  } else {
    quick_show("including static functions in search",
	       get_option(OPT_STATICMODE) == ANY_STATICS);
  }
  quick_show("prepending extern to function declarations",
	     get_option(OPT_EXTERNS));
  quick_show("testing for multi-line comments",
	     !get_option(OPT_SINGLECOMMENTS));

  /* now show all of the macro definitions */
  if (macro_list != NULL) {
    fprintf(stderr, "\n  List of preprocessor definitions:\n");
    for (macro_temp = macro_list;
	 macro_temp != NULL;
	 macro_temp = macro_temp->next) {
      /* use? */
      if ((macro_temp->usewhen != 2) &&
	  (macro_temp->usewhen != (doc_extract != DOC_NONE))) continue;
      fprintf(stderr, "\t%s\n", macro_temp->m_str);
    }
  }

  /* now check the substitutions */
  if (subst_list != NULL) {
    fprintf(stderr, "\n  List of prototype substitutions:\n");
    for (sub_tmp = subst_list;
	 sub_tmp != NULL;
	 sub_tmp = sub_tmp->next) {
      /* used? */
      if ((sub_tmp->usewhen != 2) &&
	  (sub_tmp->usewhen != (doc_extract != DOC_NONE))) continue;

      /* check it */
      switch (sub_tmp->submode) {
      case SUBST_FULL:
	/* the full string needs replacing? */
	fprintf(stderr, "\treplace all \"%s\" with \"%s\"\n",
		sub_tmp->from_str, sub_tmp->to_str);
	break;
      case SUBST_TYPE:
	/* the type only needs replacing? */
	fprintf(stderr, "\treplace type \"%s\" with \"%s\"\n",
		sub_tmp->from_str, sub_tmp->to_str);
	break;
      case SUBST_NAME:
	/* the variable only needs replacing? WHY!? */
	fprintf(stderr, "\treplace name \"%s\" with \"%s\"\n",
		sub_tmp->from_str, sub_tmp->to_str);
	break;
      }
    }
  }

  /* all done */
  fprintf(stderr, "\n");
  exit(0);
}

/* quick function to copy a "string" */
int
copy_str (o_str, in_str)
  char *o_str, *in_str;
{
  int ch, count = 0, ch_read = 0;

  /* find the beginning of the string */
  while ((*in_str != '"') &&
	 (*in_str != '\0')) {
    ch_read++;
    in_str++;
  }

  /* now was a string found? */
  if (*in_str == '\0') {
    return(-1);
  }
  ch_read++;
  in_str++;

  /* copy all of the string */
  while (((ch = *(in_str)) != '\0') &&
	 (ch != '"')) {
    /* copy it */
    o_str[count++] = ch;
    ch_read++;
    in_str++;
  }
  if (ch == '"') ch_read++;

  /* finish up and get out */
  o_str[count] = '\0';
  return(ch_read);
}

/* show the usage for the program */
void
show_usage()
{
#ifndef VMS
  fprintf(stderr,
	  "Usage: %s [-Q#] [+AaBbOPpNnVvxZ -Hstr -Yprg -o ofile] [[-opts] file ... ]\n",
	  prog_name);
  fprintf(stderr, "\t+A        alphabetically sort all functions [incompatable with +C]\n");
  fprintf(stderr, "\t+a        alphabetically sort functions within each file [default off]\n");
  fprintf(stderr, "\t-B        build a system configuration file based on settings\n");
  fprintf(stderr, "\t-b        build a configuration file in the current directory\n");
  fprintf(stderr, "\t+C        include first comment from each file\n");
  fprintf(stderr, "\t+c        include comments immediately preceding functions\n");
  fprintf(stderr, "\t-Dexpr    send a macro definition to the C preprocessor\n");
  fprintf(stderr, "\t+E        add a preceding 'extern' to each function definition\n");
  fprintf(stderr, "\t+F        toggle prepending of file name to first comment [need +C]\n");
  fprintf(stderr, "\t-f#cc     in troff doc-mode set various font styles\n");
  fprintf(stderr, "\t-Hstring  build header file with read test on string\n");
  fprintf(stderr, "\t-Idir     add directory to search list for include files\n");
  fprintf(stderr, "\t-o file   send output to a file instead of standard output\n");
  fprintf(stderr, "\t           [take first non-flag as filename for output]\n");
  fprintf(stderr, "\t-O        set only one arg per line\n");
  fprintf(stderr, "\t+m        allow multi-line comments when parsing comments\n");
  fprintf(stderr, "\t-N        do documentation output in troff ms format\n");
  fprintf(stderr, "\t-n        do documentation output in normal text\n");
  fprintf(stderr, "\t+P        create output in both ANSI and non-ANSI prototypes\n");
  fprintf(stderr, "\t+p        create output in ANSI C prototype format\n");
  fprintf(stderr, "\t-Q#       do not read in config files, or only those specified\n");
  fprintf(stderr, "\t-qfile    read in a specified configuration file\n");
  fprintf(stderr, "\t+r        remove variable names prior to output\n");
  fprintf(stderr, "\t+S        show parameters for non-ANSI portion of output\n");
  fprintf(stderr, "\t+s        include static function definitions\n");
  fprintf(stderr, "\t-s=only   capture only static function definitions\n");
  fprintf(stderr, "\t-s=none   exclude static function definitions\n");
  fprintf(stderr, "\t-s=any    include static function definitions\n");
  fprintf(stderr, "\t-TN       set tab width in doc-mode to N characters [0 = off]\n");
  fprintf(stderr, "\t-Uname    undefine the given macro name\n");
  fprintf(stderr, "\t-V        display the program settings then exit\n");
  fprintf(stderr, "\t-v        display version information then exit\n");
  fprintf(stderr, "\t+W        place function type on a separate line.\n");
  fprintf(stderr, "\t+w#       wrap long parameter output [dflt length 72]\n");
  fprintf(stderr, "\t-x        run program in extraction mode\n");
  fprintf(stderr, "\t-Yprog    specify program to use as a C preprocessor\n");
  fprintf(stderr, "\t+Z        create the output as a compact, merged list\n");
#else /* VMS */
  fprintf(stderr, "USAGE: %s [/READ-CONFIG=#] [/OUTPUT=outfile] [[/OPTS] FILE ... ]\n",
	  prog_name);
#endif /* VMS */
  exit(1);
}

/* set the value of a global option */
void
set_option(which, how, value)
  Optype which;
  int how, value;
{
#ifdef CHECK_INPUTS
  /* check input values */
  if ((which < 0) ||
      (which >= OPT_NUMBER)) {
    fprintf(stderr, "SERIOUS ERROR: Internal option value out of range %d\n",
	    which);
    exit(1);
  }
  if ((how < 0) ||
      (how > 2)) {
    fprintf(stderr, "SERIOUS ERROR: Internal option method out of range %d\n",
	    how);
    exit(1);
  }
#endif /* CHECK_INPUTS */

  /* now set properly */
  if (how == 2) {
    global_opts[0][which] = value;
    global_opts[1][which] = value;
  } else {
    global_opts[how][which] = value;
  }
}

/* retrieve the value of a global option */
int
get_option(which)
  Optype which;
{
#ifdef CHECK_INPUTS
  /* check input values */
  if ((which < 0) ||
      (which >= OPT_NUMBER)) {
    fprintf(stderr, "SERIOUS ERROR: Internal option value out of range %d\n",
	    which);
    exit(1);
  }
#endif /* CHECK_INPUTS */

  /* return whichever setting is needed */
  if (doc_extract == DOC_NONE) {
    return(global_opts[0][which]);
  }
  return(global_opts[1][which]);
}

/* set configuration settings based on the initial name */
static void
cext_init()
{
  /* enter the proper mode */
  if (strncmp(CEXTDOC_NAME, prog_name, strlen(prog_name)) == 0) {
    doc_extract = DOC_NORMAL;
  } else {
    doc_extract = DOC_NONE;
  }

  /* set documentation mode options properly */
  set_option(OPT_COMMENTS, 1, TRUE);
  set_option(OPT_STATICMODE, 1, ANY_STATICS);
  set_option(OPT_EXTERNS, 1, FALSE);
  set_option(OPT_STDCUSE, 1, TRUE);
  set_option(OPT_SORTMODE, 1, SORT_FILE);

  /* set extraction mode options now */
  set_option(OPT_COMMENTS, 0, FALSE);
  set_option(OPT_STATICMODE, 0, NO_STATICS);
  set_option(OPT_EXTERNS, 0, TRUE);
  set_option(OPT_STDCUSE, 0, FALSE);
  set_option(OPT_SORTMODE, 0, SORT_NONE);

  /* now set common options */
  set_option(OPT_NONAMES, 2, FALSE);
  set_option(OPT_COMPACT, 2, FALSE);
  set_option(OPT_TYPEWRAP, 2, FALSE);
  set_option(OPT_WRAPPOINT, 2, 0);
  set_option(OPT_BOTHUSE, 2, TRUE);
  set_option(OPT_SINGLECOMMENTS, 2, TRUE);
  set_option(OPT_FIRSTCOMMENT, 2, FALSE);
  set_option(OPT_SHOWANYWAY, 2, TRUE);
  set_option(OPT_PREPEND, 2, FALSE);
  set_option(OPT_TABWIDTH, 2, 8);
  set_option(OPT_ONEARG, 2, FALSE);
}

/* return FALSE if character is not a normal part of a filename */
int
fname_char(ch)
  int ch;
{
  switch (ch) {
#ifdef MS_DOS
  case '\\':
#endif /* MS_DOS */
#ifdef VMS
  case ']':
  case ':':
#endif /* VMS */
  case '/':
    /* nope */
    return(FALSE);
  }
  return(TRUE);
}

/* routine to build the macros onto the prototype line */
static void
append_macros(str_out)
  char *str_out;
{
  P_MACRO macro_temp;

#ifndef VAXC
  for (macro_temp = macro_list;
       macro_temp != NULL;
       macro_temp = macro_temp->next) {

    /* check for use */
    if ((macro_temp->usewhen != 2) &&
	(macro_temp->usewhen != (doc_extract != DOC_NONE))) continue;

    /* use it */
    strcat(str_out, macro_temp->m_str);
    strcat(str_out, " ");

  }
#else
  char und_str[MID_SIZE], def_str[MID_SIZE], inc_str[MID_SIZE];
  int und_uses = 0, def_uses = 0, inc_uses = 0;
  int und_len = strlen(UNDEF_LEADER);
  int inc_len = strlen(UNDEF_LEADER);
  int def_len = strlen(DEF_LEADER);

  /* initialize */
  und_str[0] = '\0';
  def_str[0] = '\0';
  inc_str[0] = '\0';

  /* properly build the command line for the C compiler */
  for (macro_temp = macro_list;
       macro_temp != NULL;
       macro_temp = macro_temp->next) {

    /* check for use */
    if ((macro_temp->usewhen != 2) &&
	(macro_temp->usewhen != (doc_extract != DOC_NONE))) continue;

    /* check for the definition statement */
    if (strncmp(DEF_LEADER, macro_temp->m_str, def_len) == 0) {
      if (def_uses++ > 0) {
	strcat(def_str, ",");
      }
      if (macro_temp->m_str[def_len] == '(') {
	strcat(def_str, macro_temp->m_str + def_len + 1);
	def_str[strlen(def_str) - 1] = '\0';
	def_uses++;
      } else {
	strcat(def_str, macro_temp->m_str + def_len);
      }
    } else if (strncmp(UNDEF_LEADER, macro_temp->m_str, und_len) == 0) {
      if (und_uses++ > 0) {
	strcat(und_str, ",");
      }
      if (macro_temp->m_str[und_len] == '(') {
	strcat(und_str, macro_temp->m_str + und_len + 1);
	und_str[strlen(und_str) - 1] = '\0';
	def_uses++;
      } else {
	strcat(und_str, macro_temp->m_str + und_len);
      }
    } else if (strncmp(INC_LEADER, macro_temp->m_str, inc_len) == 0) {
      if (inc_uses++ > 0) {
	strcat(inc_str, ",");
      }
      if (macro_temp->m_str[inc_len] == '(') {
	strcat(inc_str, macro_temp->m_str + inc_len + 1);
	und_str[strlen(inc_str) - 1] = '\0';
	def_uses++;
      } else {
	strcat(inc_str, macro_temp->m_str + inc_len);
      }
    } else {
      strcat(str_out, macro_temp->m_str);
    }

  }

  /* now finish everything off */
  if (def_uses > 0) {
    strcat(str_out, DEF_LEADER);
    if (def_uses > 1) strcat(str_out, "(");
    strcat(str_out, def_str);
    if (def_uses > 1) strcat(str_out, ")");
  }
  if (und_uses > 0) {
    strcat(str_out, UNDEF_LEADER);
    if (und_uses > 1) strcat(str_out, "(");
    strcat(str_out, und_str);
    if (und_uses > 1) strcat(str_out, ")");
  }
  if (inc_uses > 0) {
    strcat(str_out, INC_LEADER);
    if (inc_uses > 1) strcat(str_out, "(");
    strcat(str_out, inc_str);
    if (inc_uses > 1) strcat(str_out, ")");
  }
  strcat(str_out, " ");
#endif /* VAXC */
}

/* control program flow and command line options */
int
main (argc, argv)
  int argc;
  char **argv;
{
  char cur_argstr[MID_SIZE], sysl_out[MID_SIZE];
  struct stat fst;
  int count, j, len;

  /* initialize some things */
  fpout = stdout;
  len = strlen(argv[0]);
  for (j = len - 1; j > 0; j--) {
    if (fname_char(argv[0][j]) == FALSE) {
      j++;
      break;
    }
  }
  strcpy(prog_name, &(argv[0][j]));
  out_filenum = 0;
  start_comment[0] = '\0';
  header_string[0] = '\0';
  cur_cfg_file[0] = '\0';
  output_file[0][0] = '\0';
  output_file[1][0] = '\0';
  cfg_file[0] = '\0';
  sprintf(cpp_prog, "%s %s", CPP, CPP_COMMENTS);
  macro_list = last_macro = NULL;
  cext_init();

  /* check for config file flags */
  count = 1;
  if ((count < argc) &&
      is_switch(argv[count][0])) {

#ifndef VMS
    /* are we skipping config files? */
    if (argv[count][1] == 'Q') {

      /* check the type */
      if (isdigit(argv[count][2])) {
	cfg_switch = argv[count][2] - '0';
	if ((cfg_switch < 0) ||
	    (cfg_switch > 7) ||
	    (argv[count][3] != '\0')) {
	  fprintf(stderr, "Invalid value for -Q flag\n");
	  show_usage();
	}
      } else if (argv[count][2] != '\0') {
	fprintf(stderr, "Invalid data following the -Q flag\n");
	show_usage();
      } else {
	cfg_switch = 0;
      }

      /* processed this one */
      count++;
    } else {
#endif /* VMS */

      /* now test for full length string */
      if (minmatch_str(&(argv[count][1]), "read-config", 4)) {
	len = strlen(argv[count]);
	for (j = 4; j < len; j++) {
	  if ((argv[count][j] == ':') ||
	      (argv[count][j] == '=')) {
	    j++;
	    break;
	  }
	}
	if (j == len) {
	  cfg_switch = 0;
	} else if (isdigit(argv[count][j])) {
	  cfg_switch = argv[count][j] - '0';
	  if ((cfg_switch < 0) ||
	      (cfg_switch > 7) ||
	      (argv[count][j + 1] != '\0')) {
	    fprintf(stderr, "Invalid value for -read-config flag\n");
	    show_usage();
	  }
	} else {
	  fprintf(stderr, "Invalid data following the -read-config flag\n");
	  show_usage();
	}
	count++;
      }

#ifndef VMS
    }
#endif /* VMS */
  }

  /* now do the configurations */
  add_macro(2, CPP_GIVEN);
  add_macro(1, CPP_GIVEN2);
  do_config();

  /* go through the entire argument list */
  while (count 	< argc) {

    /* check for any switches */
    if (is_switch(argv[count][0])) {

      /* call the routine for command line parsing */
      if (argv[count][0] == '-') {
	j = FALSE;
      } else {
	j = TRUE;
      }
      strcpy(cur_argstr, &(argv[count][1]));
      parse_cmd(cur_argstr, j, TRUE);

    } else {

      /* treat it as a file name */
      if (out_filenum == 1) {

	/* store the output file name */
	strcpy(output_file[0], argv[count]);
	strcpy(output_file[1], argv[count]);
	out_filenum = 2;

      } else {

	/* check if both C and A are on */
	if (get_option(OPT_FIRSTCOMMENT) &&
	    (get_option(OPT_SORTMODE) == SORT_ALL)) {
	  fprintf(stderr, "Sorting all functions and displaying file names does not mix\n");
	  show_usage();
	}

	/* yes, a file has been parsed */
	out_filenum = 3;

	/* process the file for input */
	strcpy(file_name, argv[count]);
	strcpy(errout_filename, file_name);
	line_count = 1;

	/* check if the file exits */
	if (stat( file_name, &fst ) != 0) {
	  sprintf(cur_argstr, "error: could not access file <%s>",
		  file_name);
	  err_msg(cur_argstr);
	  exit(1);
	} else if ((fpin = fopen(file_name, "r")) == NULL) {
	  sprintf(cur_argstr, "error: could not read file <%s>",
		  file_name);
	  err_msg(cur_argstr);
	  exit(1);
	}
	fclose(fpin);

	/* build /lib/cpp line */
	sprintf(sysl_out, "%s ", cpp_prog);
	append_macros(sysl_out);
	strcat(sysl_out, file_name);
 
	/* open it */
	if ((fpin = open_input(sysl_out, "r")) == NULL) {
	  sprintf(tmp_str, "unable to open CPP \"pipe\" to file <%s>",
		  file_name);
	  err_msg(tmp_str);
	  exit(1);
	}

#ifdef SETBUFFER
	/* now set the buffer */
	setbuffer(fpin, inbuffer, BUFFER_SIZE);
#endif /* SETBUFFER */

	/* now do it */
	parse_file();
	files_parsed++;
	if (get_option(OPT_SORTMODE) != SORT_ALL) {
	  send_file();
	}
	close_input(fpin);

      }

    }

    /* now go to the next argument */
    count++;
  }

  /* check if -o flag is unprocessed */
  if (out_filenum == 1) {
    fprintf(stderr, "No output file specified\n");
    show_usage();
  }
  if ((out_filenum == 2) || (out_filenum == 0)) {
    fprintf(stderr, "No files for parsing specified\n");
    show_usage();
  }

  /* send out everything, if it hasn't been done */
  if (get_option(OPT_SORTMODE) == SORT_ALL) {
    send_file();
  }

  /* now close things up and combine if needed */
  cxt_close();
  exit(0);
  return(0);
}

/* transmit the currently stored comment to the output file */
void
send_first_comment(begin_str)
  char *begin_str;
{
  int mode;

  /* check for last comment */
  if (start_comment[0] != '\0') {

    /* check the mode */
    if (doc_extract == DOC_NONE) {
      mode = 0;
    } else {
      mode = 1;
    }

    /* give the header for troff output */
    dont_space = FALSE;
    if (doc_extract == DOC_ROFF) {
      start_block = TRUE;
      init_roff(1);
      if (total_out > 0) {
	out_str(1, ".bp\n");
	out_str(1, ".sp 0.5i\n");
      }
      out_str(1, ".KS\n");
      out_str(1, ".nf\n");
      out_str(1, ".ft 2\n");
    } else {
      out_char(mode, '\n');
    }

    /* duplicate if needed */
    if (!get_option(OPT_COMPACT) &&
	(doc_extract == DOC_NONE) &&
	(get_option(OPT_SHOWANYWAY) &&
	 get_option(OPT_BOTHUSE))) {
      out_char(1, '\n');
      out_str(0, begin_str);
      out_str(1, begin_str);
      out_str(0, start_comment);
      out_str(1, start_comment);
      out_char(0, '\n');
      out_char(1, '\n');
    } else {
      out_str(mode, begin_str);
      out_str(mode, start_comment);
      out_char(mode, '\n');
    }

  }
}

#ifdef TEST
/* dummy function that will never be used but makes a nice test */
char (*(*test_it())[])(same_foo, x, y, fpfoo, z, fpp2)
  int *x, z;
  FILE* fpfoo, fpp2;
  char (*(*same_foo())[])();
  int *y;
{
  /* As scary a bit of code as I could think of.  Is it valid? */
}
#endif /*TEST*/
