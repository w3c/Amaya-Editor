/* header for routines and structures storing C function prototypes */

/* system header files */
#ifdef SGI
/* I hate having to do this! */
#if __STDC__
#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif /* __EXTENSIONS__ */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif /* _POSIX_SOURCE */
#endif /* __STDC__ */
#endif /* SGI */
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#ifdef _POSIX_SOURCE
#include <stdlib.h>
#endif /* _POSIX_SOURCE_ */
#if __STDC__ 
#ifndef VMS
#include <unistd.h>
#endif /* VMS */
#endif /* __STDC__ */

/* patchlevel information */
#include "patchlevel.h"

/* === start of system configurations === */

/*
 * This option is enabled if a VMS compiler doesn't have "popen()"
 * command to open a pipe to a system command; this is standard
 * Unix I/O function.
 */
#ifdef VAXC
#define NO_POPEN	/* use a hack to get a "pipe" command */
#endif /* VAXC */

/*
 * This definition checks for a failure by the "system()" function.
 */
#ifdef VAXC
#define SYSCMD_FAILED	0	/* if system() fails, it returns a zero? */
#else
#define SYSCMD_FAILED	-1	/* if system() fails, it returns a -1 */
#endif /* VAXC */

/* the command to perform the C preprocessing (full path best) */
#ifndef CPP
#ifndef VMS
#define CPP "cpp"
#else
#define CPP "cc"
#endif /* VMS */
#endif /* CPP */

/* flag to CPP to retain comments */
#ifndef VAXC
#define CPP_COMMENTS "-C"
#else
#define CPP_COMMENTS ""
#endif /* VAXC */

/* an sprintf format statement with 1 %d to build a temporary file */
#ifndef TMPFILE_FMT
#ifndef VMS
#define TMPFILE_FMT	"/tmp/.cxtprs.%d"
#else
#define TMPFILE_FMT	"sys$scratch:cxtprs.%d"
#endif /* VMS */
#endif /* TMPFILE_FMT */

/* same thing, but only needed for systems without the popen() command */
#ifdef NO_POPEN
#ifndef PIPETMP_FMT
#ifndef VMS
#define PIPETMP_FMT	"/tmp/.cxtpip.%d"
#else
#define PIPETMP_FMT	"sys$scratch:cxtpip.%d"
#endif /* VMS */
#endif /* PIPETMP_FMT */
#endif /* NO_POPEN */

/* how to generate the output to a temp file if there is no popen() */
/* note:  need two %s entries:  first = command, second = outfile */
#ifdef NO_POPEN
#ifndef PIPEOUT_FMT
#ifndef VMS
#define PIPEOUT_FMT "%s > %s"
#else
#define PIPEOUT_FMT "%s /preprocess=%s"
#endif /* VMS */
#endif /* PIPEOUT_FMT */
#endif /* NO_POPEN */

/* the name for the documentation builder */
#ifndef CEXTDOC_NAME
#define CEXTDOC_NAME "cextdoc"
#endif /* CEXTDOC_NAME */

/* the name for the configuration file */
#ifndef CONFIG_FILE
#ifndef VMS
#define CONFIG_FILE	".cextrc"
#else
#define CONFIG_FILE	"cext.cnf"
#endif /* VMS */
#endif /* CONFIG_FILE */

/* full path to any possible system configuration file */
#ifndef SYS_CONFIG
#ifndef VMS
#define SYS_CONFIG	"/usr/local/lib/cext.config"
#else
#define SYS_CONFIG	"sys$library:cext.cnf"
#endif /* VMS */
#endif /* SYS_CONFIG */

/* directory separator on the system */
#ifndef DIR_SEPARATOR
#ifdef VMS
#define DIR_SEPARATOR	""
#else
#ifdef MS_DOS
#define DIR_SEPARATOR	"\\"
#else
#define DIR_SEPARATOR	"/"
#endif /* MS_DOS */
#endif /* VMS */
#endif /* DIR_SEPARATOR */

/* commands to C processor and Switch on invocation line */
#ifndef DEF_LEADER
#ifndef VAXC
#define DEF_LEADER	"-D"
#else
#define DEF_LEADER	"/define="
#endif /* VAXC */
#endif /* DEF_LEADER */

#ifndef UNDEF_LEADER
#ifndef VAXC
#define UNDEF_LEADER	"-U"
#else
#define UNDEF_LEADER	"/undefine="
#endif /* VAXC */
#endif /* UNDEF_LEADER */

#ifndef INC_LEADER
#ifndef VAXC
#define INC_LEADER	"-I"
#else
#define INC_LEADER	"/include="
#endif /* VAXC */
#endif /* INC_LEADER */

/* === end of system configurations === */

/* structure for prototype storage */
typedef struct s_proto {
  char *name;		/* name of the function */
  char *ftype;		/* type of the function */
  char *fname;		/* name of file where located */
  char *plist;		/* ANSI format prototype list */
  char *comment;	/* comment associated with function */
  struct s_proto *next;
} S_PROTO, *P_PROTO;

/* structure definition for holding macros */
typedef struct s_macro {
  char *m_str;		/* storage for the macro element */
  struct s_macro *next;	/* pointer to next macro element */
  char usewhen;		/* indicator of when it is to be used */
} S_MACRO, *P_MACRO;

/* structure definition for substitutions */
typedef struct s_subst {
  char *from_str;	/* the string to substitute for */
  char *to_str;		/* the string to substitute with */
  char submode;		/* indicator of substitution type */
  char usewhen;		/* indicator of when it is to be used */
  struct s_subst *next;	/* pointer to the next string */
} S_SUBST, *P_SUBST;

/* pseudo boolean definitions */
#ifndef TRUE
#define TRUE	(1)
#define FALSE	(0)
#endif

/* the list of substitution types */
#define SUBST_FULL	1	/* replace both variable type and name */
#define SUBST_TYPE	2	/* replace for match with type only */
#define SUBST_NAME	3	/* replace for complete name match only */

/* now declare the variable space */
extern P_SUBST subst_list;

/* list of options flags */
typedef enum e_opts {
  OPT_COMMENTS, OPT_FIRSTCOMMENT, OPT_SINGLECOMMENTS, OPT_PREPEND,
  OPT_EXTERNS, OPT_STATICMODE, OPT_WRAPPOINT, OPT_TYPEWRAP,
  OPT_SHOWANYWAY, OPT_STDCUSE, OPT_BOTHUSE, OPT_SORTMODE,
  OPT_TABWIDTH, OPT_NONAMES, OPT_COMPACT, OPT_ONEARG
} Optype;

/* how many different options are there? */
#define OPT_NUMBER	16

/* modes of output */
#define MODE_ANSI	0
#define MODE_COMMENT	1
#define MODE_OLDC	2

/* modes of sorting */
#define SORT_NONE	0
#define SORT_FILE	1
#define SORT_ALL	2

/* different documentation modes */
#define DOC_NORMAL	0
#define DOC_ROFF	1
#define DOC_NONE	2

/* different static modes */
#define NO_STATICS	0
#define ONLY_STATICS	1
#define ANY_STATICS	2

/* the number of characters allocated for parsing */
#define MAX_SIZE 100000
#define MID_SIZE 4000
#define FNAME_SIZE 256

/* structure for the internal temporary storage */
typedef struct s_bufdata {
  char data[MID_SIZE + 1];	/* work with 1,000 characters at a time */
  struct s_bufdata *next;	/* to the next element */
} S_BUFDATA, *P_BUFDATA;

#ifdef SETBUFFER
/* allocation for buffering */
#define BUFFER_SIZE	20480	/* 20 kilobytes of buffering */
#endif /* SETBUFFER */

/* set the version information */
#define VERSION	1

/* quickie macro for variable component recognition */
#define id_char(x)	(isalnum(x) || (x == '_'))

/* call in the file with the prototypes */
#ifndef __CEXTRACT__
#include "proto.h"
#endif /* __CEXTRACT__ */

/* data that should be defined */
extern char prog_name[FNAME_SIZE];

/* system functions */
#if defined(SYSV) || !defined(BSD)
#include <string.h>
#define index strchr
#else
#include <strings.h>
#endif /* SYSV */
#ifdef VAXC
#define unlink remove
#endif /* VAXC */

#ifdef XENIX
/* Provide a strstr() replacement on Xenix sites */
#define NO_STRSTR
#endif /* XENIX */

/* I don't want to see this on SunOS */
#ifdef BSD
#ifdef SYSV
#ifndef toupper
extern int toupper();
#endif /* toupper */
extern int fprintf(), sscanf();
extern int _flsbuf(), _filbuf();
extern void perror();
extern long time();
extern int fputs(), fclose(), pclose();
#ifdef SETBUFFER
extern void setbuffer();
#endif /* SETBUFFER */
#endif /* SYSV */
#endif /* BSD */

/* options for different parsing methods */
extern int global_opts[2][OPT_NUMBER];
extern int doc_extract, cfg_switch;

/* variables for keeping track of things */
extern int start_block, files_parsed, total_out, dont_space;

/* storage */
extern FILE *fpin;
extern P_MACRO macro_list, last_macro;
extern P_PROTO proto_list;
extern char file_name[], start_comment[], cfg_file[], tmp_str[];

/* various fonts for documentation troff formats */
extern char ft_title[3], ft_comment[3], ft_name[3], ft_plist[3];

/* other data declarations */
extern P_BUFDATA tempbuf[2];
extern char cur_cfg_file[FNAME_SIZE], header_string[MID_SIZE];
#ifdef SETBUFFER
extern char outbuffer[BUFFER_SIZE], inbuffer[BUFFER_SIZE];
#endif /* SETBUFFER */
extern FILE *fpout;
extern char output_file[2][MID_SIZE];
extern char cpp_prog[MID_SIZE], errout_filename[MID_SIZE];
extern int out_filenum, line_count;
