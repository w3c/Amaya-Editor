/*
 * ilustub.c: ISL generator using include external definition of functions.
 *
 *  Daniel Veillard, 1997
 */

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#ifdef HAVE_SYS_FCNTL_H
#include <sys/fcntl.h>
#endif
#endif
#ifdef linux
#include <sys/mman.h>
#endif

#define MAX_PATH	100
#define MAX_PATH_LENGHT	256
#define MAX_INCLUDE	200
#define MAX_SUB_INCLUDE 30

#define MAX_ARGS	30
#define MAX_FUNCTION	250
#define MAX_CONSTANT	500
#define MAX_ENUM	50

char *ilu_reserved_word[] = {
    "ARRAY", "ASYNCHRONOUS", "AUTHENTIFICATION", "BOOLEAN", "BRAND",
    "CARDINAL", "CHARACTER", "CLASS", "COLLECTIBLE", "CONSTANT",
    "DEFAULT", "END", "ENUMERATION", "EXCEPTION", "FALSE", "FROM",
    "FUNCTIONNAL", "IMPORTS", "IN", "INOUT", "INTEGER", "INTERFACE",
    "LIMIT", "LONG", "METHODS", "OBJECT", "OF", "OPTIONNAL", "OTHERS",
    "OUT", "PICKLE", "RAISES", "REAL", "RECORD", "SEQUENCE", "SHORT",
    "SIBLING", "SINGLETON", "SINK", "SOURCE", "SUPERCLASS",
    "SUPERCLASSES", "SUPERTYPES", "TRUE", "TYPE", "TYPEID", "UNION"
};

typedef enum {
    TYPE_NONE = 0,
    TYPE_IN = 1,
    TYPE_OUT = 2,
    TYPE_INOUT = 3
} inout;

/*
 * Registered type definition.
 */
typedef struct _Type {
    int   indir;  /* level of indirection, e.g. char * -> 1 */
    int   retval; /* Is this an output parameter (if indir) */
    char *name;   /* name of the type e.g. "int"            */
    char *iname;  /* ilu name of the type e.g. "int"        */
    char *itype;  /* internal type if any                   */
    char *convert;/* Is there a function to convert types   */
} Type;

/*
 * add more type as needed.
 */

int nbTypes = 5;
Type tabType[1000] = {
{ 0, TYPE_IN, "void",	"",		"void",			NULL},
{ 0, TYPE_IN, "boolean","BOOLEAN",	"boolean",		NULL},
{ 0, TYPE_IN, "int",	"INTEGER",	"CORBA_long",		NULL},
{ 1, TYPE_IN, "char",	"ilu.CString",	"char *",		NULL},
{ 1, TYPE_OUT,"char",	"ilu.CString",	"char *",		NULL},
};

/*
 * Parsed enum val definition.
 */
typedef struct _EnumVal {
    struct _EnumVal *next;
    char *type_name; /* name of the enum type */
    char *name;   /* name of the value */
    int  value;  /* value associated */
} EnumVal, *Enum;

/*
 * Parsed arg definition.
 */
typedef struct _Arg {
    char *name;   /* name of the arg e.g. "doc" if any */
    int   type;   /* index of the type in tabType */
    inout retval; /* Whether this is an OUT or IN/OUT value */
} Arg;

/*
 * Parsed function definition.
 */
typedef struct _Function {
    char *comment;/* pointers to the comment block           */
    char *name;   /* name of the C function e.g. "CreateDoc" */
    int   type;   /* index in tabType of the returned type   */
    int   nb_args;/* number of arguments */
    Arg   args[MAX_ARGS]; /* the Arguments       */
} Function;

/*
 * Parsed constant definition.
 */
typedef struct _Constant {
    char *name;   /* name of the C function e.g. "CreateDoc" */
    int   type;   /* index in tabType of the returned type   */
    char *string; /* value */
} Constant;

/*
 * global variables.
 */
char *filename;
char *modulename = "UnknownClassName";
char *stubname = "UnknownStubName";

int nbFunctions = 0;
Function tabFunctions[MAX_FUNCTION];

int nbConstants = 0;
Constant tabConstants[MAX_CONSTANT];

int nbEnum = 0;
Enum tabEnum[MAX_ENUM];

char *comment_start = NULL;
char *comment_end = NULL;

char *stubsOutputFile = NULL;
char *iluOutputFile = NULL;
char *stubCOutputFile = NULL;
char *stubHOutputFile = NULL;

int main_argc;
char **main_argv;
char pwd[MAX_PATH_LENGHT];

#ifndef HAVE_STRDUP
char *strdup(const char *s)
{
    char *str;

    if (s == NULL) return(NULL);
    str = malloc(strlen(s) + 1);
    if (str == NULL) return(NULL);
    strcpy(str, s);
    return(str);
}
#endif

#if defined(__alpha__) || defined(__i386__)
#define LE_MACHINE
#endif

#ifdef LE_MACHINE
#define next_byte(x) (x >>= 8)
#define current ((unsigned char) __buf)
#else
#define next_byte(x) (x <<= 8)
#define current (__buf >> 8*(sizeof(unsigned long)-1))
#endif

char *mmap_map;
int mmap_mapsize;
int filesize;
int verbose = 0;

int idx;
int is_package = 0;
char package[256];
char *interface_name = &package[0];

/*************
#define NEXT(p) { (p)++ ; if ((*(p) == '/') && (*(p + 1) == '*') { \
  p+=3; while (*
 *************/

#define NEXT(p) (p)++;
#define IS_WHITE(c) (((c) == ' ') || ((c) == '\t'))

#define SKIP_WHITE(p) { while (IS_WHITE(*p)) NEXT(p) }
#define IS_BLANK(c) (((c) == ' ') || ((c) == '\n') || \
                     ((c) == '\r') || ((c) == '\t'))

#define SKIP_BLANK(p) { while (IS_BLANK(*p)) NEXT(p) }
#define IS_ALPHA(c) ((((c) >= 'a') && ((c) <= 'z')) || \
                     (((c) >= 'A') && ((c) <= 'Z')) || \
		     ((c) == '_')) 
#define IS_NUM(c) (((c) >= '0') && ((c) <= '9'))

/*
 * pointer_type : returns an interger ILU type for pointers depending
 *                on the local side of pointers.
 *          it returns either "INTEGER" or "LONG INTEGER" depending
 *          on the target architecture.
 */

char *pointer_type(void) {
    if (sizeof(void *) == 4) return("INTEGER");
    else if (sizeof(void *) == 8) return("LONG INTEGER");
    else {
        fprintf(stderr, "sizeof(void *) is neither 4 nor 8 !\n");
	exit(1);
    }
}

/*
 * read_type : read a sequence of type definitions from an external file.
 *             the "pointer" value for iname will be replaced by the
 *             correct integer size.
 */
void read_type(char *filename)
{
    char buffer[1000];
    char *p, *q;
    FILE *in;
    int line = 0;
    int indir;
    int retval;
    char name[256];
    char iname[256];
    char itype[256];
    char convert[256];
    int basetype = nbTypes;
    int newtype = 0;
    int i;

    in = fopen(filename, "r");
    if (in == NULL) {
        fprintf(stderr,"Cannot open file %s\n", filename);
        perror("fopen");
	return;
    }
    while (1) {
        line++;
        if ((fgets(buffer, sizeof(buffer), in)) == NULL) break;
	p = &buffer[0];
	SKIP_BLANK(p)
	if (*p == '#') continue;

	/* read the number of indirections */
	indir = 0;
	if (!(IS_NUM(*p))) {
	    fprintf(stderr,"file %s, line %d : bad input\n%s\n",
	            filename, line, buffer);
            continue;
	}
	while (IS_NUM(*p)) { indir = indir * 10 + *p - '0'; p++; }

	/* read whether this is a return value */
	retval = 0;
	SKIP_BLANK(p)
	if (!(IS_NUM(*p))) {
	    fprintf(stderr,"file %s, line %d : bad input\n%s\n",
	            filename, line, buffer);
            continue;
	}
	while (IS_NUM(*p)) { retval = retval * 10 + *p - '0'; p++; }

	if (retval == 0) retval = TYPE_IN;
	else retval = TYPE_OUT;

	/* read the C type name */
	q = &name[0];
	SKIP_BLANK(p)
	if (*p == '"') {
	    p++;
	    while ((*p != '\0') && (*p != '"')) *q++ = *p++;
	    *q = '\0';
	} else {
	    while ((*p != '\0') && !IS_BLANK(*p)) *q++ = *p++;
	    *q = '\0';
	}
	if (*p == '\0') {
	    fprintf(stderr,"file %s, line %d : bad input\n%s\n",
	            filename, line, buffer);
            continue;
	}
	p++;

	/* read the ILU type name */
	q = &iname[0];
	SKIP_BLANK(p)
	if (*p == '"') {
	    p++;
	    while ((*p != '\0') && (*p != '"')) *q++ = *p++;
	    *q = '\0';
	} else {
	    while ((*p != '\0') && !IS_BLANK(*p)) *q++ = *p++;
	    *q = '\0';
	}
	if (*p == '\0') {
	    fprintf(stderr,"file %s, line %d : bad input\n%s\n",
	            filename, line, buffer);
            continue;
	}
	p++;

	/* read the ILU internal type */
	q = &itype[0];
	SKIP_BLANK(p)
	if (*p == '"') {
	    p++;
	    while ((*p != '\0') && (*p != '"')) *q++ = *p++;
	    *q = '\0';
	} else {
	    while ((*p != '\0') && !IS_BLANK(*p)) *q++ = *p++;
	    *q = '\0';
	}
	if (*p == '\0') {
	    fprintf(stderr,"file %s, line %d : input\n%s\n",
	            filename, line, buffer);
            continue;
	}
	p++;

        /* read the ILU internal type name if any */
	q = &convert[0];
	*q = 0;
	SKIP_BLANK(p)
	if (*p == '"') {
	    p++;
	    while ((*p != '\0') && (*p != '"')) *q++ = *p++;
	    *q = '\0';
	} else {
	    while ((*p != '\0') && !IS_BLANK(*p)) *q++ = *p++;
	    *q = '\0';
	}

	tabType[nbTypes].indir = indir;
	tabType[nbTypes].retval = retval;
	tabType[nbTypes].name = strdup(name);
	if (!strcmp(iname, "pointer"))
	    tabType[nbTypes].iname = pointer_type();
	else
	    tabType[nbTypes].iname = strdup(iname);
	tabType[nbTypes].itype = strdup(itype);
	if (convert[0])
	    tabType[nbTypes].convert = strdup(convert);
        else
	    tabType[nbTypes].convert = NULL;
	nbTypes++;
	newtype++;
    }
    fclose(in);
    printf("Read %d types from %s : ", newtype, filename);
    for (;basetype < nbTypes;basetype++) {
        printf("%s ", tabType[basetype].name);
        for (i = 0;i < tabType[basetype].indir;i++) printf("*");
	printf(", ");
    }
    printf("\n");
}

static inout parse_comment(char **next)
{
    char *p = *next;
    inout retval = TYPE_NONE;

    if (*next >= mmap_map + filesize) return(retval);
    SKIP_BLANK(p)
    if ((*p == '/') && (*(p+1) == '*')) {
        p += 2;
	comment_start = p;
	SKIP_BLANK(p)
	if (!strncmp(p, "INOUT", 5)) {
	    retval = TYPE_INOUT;
	    p += 5;
	} else if (!strncmp(p, "IN", 2)) {
	    retval = TYPE_IN;
	    p += 2;
	} else if (!strncmp(p, "OUT", 3)) {
	    retval = TYPE_OUT;
	    p += 3;
	}
	while ((*p != '*') || (*(p+1) != '/')) p++;
	comment_end = p;
	p += 2;
	SKIP_BLANK(p)
    }
    *next = p;
    return(retval);
}

/*
 * Parse an identifier : due to some restrictions with ILU,
 * underscore are removed and the next letter is upper-cased.
 *
 *   e.g. "xbm_type" is transformed in "xbmType"
 */
static char *parse_identifier(char **next)
{
    static char identifier[256];
    int index = 0;
    char *p = *next;
    int toup = 0;
    int i;

    if (*next >= mmap_map + filesize) return(NULL);;
    SKIP_BLANK(p)
    if (IS_ALPHA(*p)) {
        identifier[index++] = *p++;
	while ((IS_ALPHA(*p)) || (IS_NUM(*p))) {
	   if (*p == '_') {
	       toup = 1;
	       p++;
	   } else if (toup) {
	       char c = *p++;

	       identifier[index++] = toupper(c);
	       toup = 0;
	   } else
	       identifier[index++] = *p++;
	}
	identifier[index] = '\0';
	*next = p;

	/*
	 * check for conflicts with ILU reserved words.
	 */
	for (i = 0;
	     i < ((sizeof(ilu_reserved_word))/(sizeof(ilu_reserved_word[0])));
	     i++)
	    if (!strcasecmp(identifier, ilu_reserved_word[i])) {
		identifier[index] = '1';
		identifier[index + 1] = '\0';
	    }

	return(&identifier[0]);
    }
    *next = p;
    return(NULL);
}

static int parse_type(char **next, inout retval)
{
    static char name[256];
    int index = 0;
    char *p = *next;
    int i;
    int indir;

    if (*next >= mmap_map + filesize) return(-1);

    SKIP_BLANK(p)
    if (IS_ALPHA(*p)) {
	/* Ignore any "const" or "CONST" keyword */
	if ((!strncmp(p, "const", 5)) && (IS_BLANK(p[5]))) {
	    p += 5;
            SKIP_BLANK(p)
	} else if ((!strncmp(p, "CONST", 5)) && (IS_BLANK(p[5]))) {
	    p += 5;
            SKIP_BLANK(p)
	}

        /* parse the type base name */
        name[index++] = *p++;
	while ((IS_ALPHA(*p)) || (IS_NUM(*p)))
	   name[index++] = *p++;
	name[index] = '\0';

	/* handle indirections */
	SKIP_BLANK(p)
	indir = 0;
	while (*p == '*') { indir++; p++; SKIP_BLANK(p) }

	*next = p;
        for (i = 0; i < nbTypes;i++)
	    if ((!strcmp(name, tabType[i].name)) &&
	        (indir == tabType[i].indir)) {
		if ((retval & TYPE_OUT) == (tabType[i].retval & TYPE_OUT))
		    return(i);
	    }

	if (verbose) {
	    fprintf(stderr,"Function %s : unknown type \"%s ",
	            tabFunctions[nbFunctions].name, name);
	    for (i = 0; i < indir;i++) fprintf(stderr,"*");
	    fprintf(stderr,"\"\n");
	}
        return(-1);
    }
    *next = p;
    return(-1);
}

static void parse_function(char **next)
{
    int return_type;
    char *func_name;
    int arg_type;
    char *arg_name;
    char *p = *next;
    char dump_arg_name[50];
    int nb_arg;
    int i;
    inout retval;
    char *com_start;
    char *com_end;

    if (p >= mmap_map + filesize) {
	*next = p;
	return;
    }
    parse_comment(&p);
    com_start = comment_start;
    com_end = comment_end;

    return_type = parse_type(&p, TYPE_IN);
    if (return_type < 0) {
	if (p == *next) p++;
        goto cleanup;
    }
    func_name = parse_identifier(&p);
    if (func_name == NULL) goto cleanup;
    SKIP_BLANK(p)

    if (*p != '(') goto cleanup;
    p++;
    
    /* check for redefinitions */
    for (i = 0;i < nbFunctions;i++)
	if (!strcmp(func_name, tabFunctions[i].name)) goto cleanup;

    /* add preliminary to the table, don't increase nbFunctions now */
    tabFunctions[nbFunctions].name = strdup(func_name);
    tabFunctions[nbFunctions].type = return_type;
    tabFunctions[nbFunctions].nb_args = nb_arg = 0;
    verbose = 1;
    while (1) {
	SKIP_BLANK(p)
	if (*p == ')') break;

	/*
	 * Parse any comment, looking for OUT or INOUT markers
	 */
	retval = parse_comment(&p);
	if (retval == TYPE_NONE) retval = TYPE_IN;

	/*
	 * Parse the type of the argument.
	 */
	arg_type = parse_type(&p, retval);
	if (arg_type < 0) goto cleanup;

	/*
	 * case of a procedure (no args).
	 */
	if (!strcmp(tabType[arg_type].name, "void")) {
	    SKIP_BLANK(p);
	    if (*p == ')') break;
	    goto cleanup;
	}

        /*
	 * Parse the argument name, if available.
	 */
	parse_comment(&p);
	arg_name = parse_identifier(&p);

	if (arg_name == NULL) {
	    sprintf(dump_arg_name, "arg_%d", nb_arg + 1);
	    arg_name = &dump_arg_name[0];
	}
	tabFunctions[nbFunctions].args[nb_arg].name = strdup(arg_name);
	tabFunctions[nbFunctions].args[nb_arg].type = arg_type;
	tabFunctions[nbFunctions].args[nb_arg].retval = retval;
	nb_arg++;
	tabFunctions[nbFunctions].nb_args = nb_arg;

	SKIP_BLANK(p);
	if (*p == ')') break;
	if (*p == ',') {
	    p++;
	    continue;
	}
	goto cleanup;

    }
    if ((com_start != NULL) && (com_end != NULL)) {
        int len = com_end - com_start;
        char *p = malloc(len + 1);

	if (p != NULL)
	    strncpy(p, com_start, len);

	tabFunctions[nbFunctions].comment = p;
    } else {
	tabFunctions[nbFunctions].comment = NULL;
    }
    comment_start = comment_end = NULL;
    nbFunctions++;
    
cleanup:
    verbose = 0;
    *next = p;
}

static void parse_constant(char **next)
{
    char buffer[1000];
    char *buf = &buffer[0];
    char *name;
    char *p = *next;

    if (p >= mmap_map + filesize) {
	goto cleanup;
    }
    if (*p != '#') {
	goto cleanup;
    }
    p++;
    if (strncmp(p, "define", 6)) {
	goto cleanup;
    }
    p += 6;
    SKIP_WHITE(p);
    name = parse_identifier(&p);

    if (name == NULL) {
	goto cleanup;
    }
    SKIP_WHITE(p);
    if ((*p == '\n') || (*p == '\r')) {
        /*
	 * Simple preprocessor directives #define toto
	 * without added value are useless from Ilu side.
	 */
	goto cleanup;
    }
    if (*p == '(') {
        /*
	 * We don't parse macro's (yet).
	 */
	goto cleanup;
    }
    if (*p == '\"') {
        /*
	 * This is a string.
	 * should handle \" in strings ...
	 */
	do
	  *buf++ = *p++;
	while ((*p != '\"') && (*p != '\n') && (*p != '\r'));
	if (*p == '\"') {
	    *buf++ = '\"';
	    *buf++ = '\0';
	    tabConstants[nbConstants].name = strdup(name);
	    tabConstants[nbConstants].string = strdup(buffer);
	    tabConstants[nbConstants].type = 3;
	    nbConstants++;
	}
	/*
	 * malformed string ...
	 */
	goto cleanup;
    }
    if ((*p >= '0') && (*p <= '9')) {
        /*
	 * This might be a number.
	 */
	if ((*p == '0') && ((*(p + 1) == 'x') || (*(p + 1) == 'X'))) {
	    /*
	     * Read an hexadecimal number.
	     */
	    *buf++ = *p++;
	    *buf++ = *p++;
	    while (((*p >= '0') && (*p <= '9')) ||
	           ((*p >= 'A') && (*p <= 'F')) ||
		   ((*p >= 'a') && (*p <= 'f'))) 
		*buf++ = *p++;
	    *buf++ = '\0';
	    tabConstants[nbConstants].name = strdup(name);
	    tabConstants[nbConstants].string = strdup(buffer);
	    tabConstants[nbConstants].type = 2;
	    nbConstants++;
	} else {
	    /*
	     * Read a decimal number.
	     */
	    while ((*p >= '0') && (*p <= '9'))
		*buf++ = *p++;
	    *buf++ = '\0';
	    tabConstants[nbConstants].name = strdup(name);
	    tabConstants[nbConstants].string = strdup(buffer);
	    tabConstants[nbConstants].type = 2;
	    nbConstants++;
	}
    }

        
cleanup:
    *next = p;
    return;
}

static void parse_enum(char **next)
{
    char *name;
    char *type_name;
    char *p = *next;
    Enum first = NULL;
    Enum prev = NULL;
    Enum cour = NULL;
    int value = 0;

    if (p >= mmap_map + filesize) {
	goto cleanup;
    }
    if (strncmp(p, "enum", 4)) {
	goto cleanup;
    }
    p += 4;
    SKIP_BLANK(p);
    if (IS_ALPHA(*p)) {
	type_name = strdup(parse_identifier(&p));
	SKIP_BLANK(p);
    }
    if (*p != '{') goto cleanup;
    p++;

    while (1) {
	SKIP_BLANK(p);
	if (*p == '}') break;
	name = parse_identifier(&p);

	if (name == NULL) {
	    goto cleanup;
	}
	SKIP_BLANK(p);
	if (*p == '=') {
	    p++;
	    SKIP_BLANK(p);

	    /*
	     * This should be a number.
	     */
	    if ((*p == '0') && ((*(p + 1) == 'x') || (*(p + 1) == 'X'))) {
		/*
		 * Read an hexadecimal number.
		 */
		p++;
		p++;
		value = 0;
		while (((*p >= '0') && (*p <= '9')) ||
		       ((*p >= 'A') && (*p <= 'F')) ||
		       ((*p >= 'a') && (*p <= 'f'))) {
		    if ((*p >= '0') && (*p <= '9'))
		        value = value * 16 + *p++ - '0';
		    else if ((*p >= 'A') && (*p <= 'F'))
		        value = value * 16 + *p++ - 'A' + 10;
		    else if ((*p >= 'a') && (*p <= 'f'))
		        value = value * 16 + *p++ - 'a' + 10;
		}
	    } else {
		/*
		 * Read a decimal number.
		 */
		value = 0;
		while ((*p >= '0') && (*p <= '9'))
		    value = value * 10 + *p++ - '0';
	    }
	} else if (prev != NULL) {
	    value = prev->value + 1;
	} else
	    value = 0;
	/*
	 * allocate a new element.
	 */
	cour = (Enum) malloc(sizeof(EnumVal));
	if (cour == NULL) goto cleanup;
	cour->type_name = type_name;
	cour->name = strdup(name);
	cour->value = value;
	cour->next = NULL;
	if (first == NULL) first = cour;
	if (prev != NULL) prev->next = cour;

	SKIP_BLANK(p);
	if (*p == ',') p++;

	SKIP_BLANK(p);
	if (*p == '}') break;

	prev = cour;
    }
    /*
     * try to fetch an external name for the union type, if any.
     */
    if (*p == '}') {
        p++;
	SKIP_BLANK(p);
	if (IS_ALPHA(*p)) {
	    first->type_name = strdup(parse_identifier(&p));
	    SKIP_BLANK(p);
	}
    }
    tabEnum[nbEnum] = first;
    nbEnum++;

        
cleanup:
    *next = p;
    return;
}

static void parse(char *next)
{
    while (1) {
        if (next >= mmap_map + filesize) return;
	SKIP_BLANK(next);
	if (*next == '#') parse_constant(&next);
	else if (!strncmp(next, "enum", 4)) parse_enum(&next);
        else parse_function(&next);
    }
}

static void do_parse(void)
{
	int res;
	int pagesizem1 = getpagesize()-1;
	int fd = open(filename, O_RDONLY);
	struct stat st;

	if (fd < 0) {
		perror("mkdep: open");
		return;
	}
	fstat(fd, &st);
	filesize = st.st_size;
	mmap_mapsize = st.st_size + 2*sizeof(unsigned long);
	mmap_mapsize = (mmap_mapsize+pagesizem1) & ~pagesizem1;
#ifdef linux
	mmap_map = mmap(NULL, (size_t) mmap_mapsize, PROT_READ,
                        MAP_PRIVATE, fd, 0);
	if (-1 == (long)mmap_map) {
		perror("mkdep: mmap");
		close(fd);
		return;
	}
	res = close(fd);
	parse(mmap_map);
	res = munmap(mmap_map, (size_t) mmap_mapsize);
        if (res < 0) perror("munmap failed");
#else
        mmap_map = malloc(mmap_mapsize);
	if (mmap_map == NULL) {
	    perror("malloc failed");
	    exit(1);
	}
	res = read(fd, mmap_map, filesize);
	if (res < filesize) {
	    perror("read didn't complete");
	    exit(1);
	}
	close(fd);
	parse(mmap_map);
	free(mmap_map);
#endif
}

void dump_cmd(FILE *out) {
#if 0
    int argc = main_argc;
    char **argv = main_argv;
#endif

    fprintf(out," * DO NOT EDIT\n");
#if 0
    /* Removed additionnal infos, was too verbose and polluted CVS */
    fprintf(out," * Generated by the following command from %s directory\n * ",
            pwd);

    while (argc-- > 0) {
       char *name = *argv++;

       fprintf(out,"%s ", name);
    }
#else
    fprintf(out," * automatically generated by ilustub");
#endif
    fprintf(out,"\n");
}

void dump_ilu(FILE *out) {
    int i,n;
    Function *f;
    Constant *c;
    Arg *a;
    Type *t;
    Enum e;
    char *enum_name = NULL;

    fprintf(out,"(*\n");
    dump_cmd(out);
    fprintf(out," *)\n");
    if (is_package) fprintf(out,"\nINTERFACE %s;\n",
                            interface_name);


    fprintf(out,"\n(*\n");
    fprintf(out," * ILU interface for %s\n",
           modulename);
    fprintf(out," *)\n\n");

    /*
     * Dump each constant.
     */
    for (i = 0;i < nbConstants;i++) {
        c = &tabConstants[i];
	t = &tabType[c->type];
	fprintf(out,"CONSTANT %s : %s = %s;\n",
                c->name, t->iname, c->string);
    }
    if (nbConstants > 0) fprintf(out,"\n");

    /*
     * Dump each enum values.
     */
    for (i = 0;i < nbEnum;i++) {
        e = tabEnum[i];
	fprintf(out,"TYPE %s = ENUMERATION\n", e->type_name);
	while (e != NULL) {
	    fprintf(out,"  %s = %d",
		    e->name, e->value);
            if (e->next != NULL) fprintf(out,",\n");
	    e = e->next;
	}
	fprintf(out,"\n  END;\n\n");
    }

    /*
     * Dump the main TYPE for this interface.
     */
    fprintf(out, "TYPE %s = OBJECT\n", package);
    if (nbFunctions > 0)
	fprintf(out, "  METHODS\n");
    for (i = 0;i < nbFunctions;i++) {
        f = &tabFunctions[i];
	t = &tabType[f->type];

        if (i != 0) fprintf(out,",\n\n");
	fprintf(out,"    %s (", f->name);

        for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

	    if (n != 0) fprintf(out,", ");

	    if (a->retval == TYPE_IN)
	        fprintf(out,"IN ");
	    if (a->retval == TYPE_OUT)
	        fprintf(out,"OUT ");
	    if (a->retval == TYPE_INOUT)
	        fprintf(out,"INOUT ");

	    fprintf(out,"%s", a->name);
	    fprintf(out,": %s", t->iname);
	}
	fprintf(out,") ");
	t = &tabType[f->type];
	if (t->iname[0] != '\0')
	   fprintf(out,": %s ", t->iname);
	/*****
        if (f->comment != NULL)
	    fprintf(out,"\n\"%s\"", f->comment);
	 *****/
    }
    if (nbFunctions > 0)
	fprintf(out, "\n  END;\n");
    fprintf(out,"\n");
}

void dump_h(FILE *out) {
    int i,n,p;
    Function *f;
    Arg *a;
    Type *t;
    int argc = main_argc;
    char **argv = main_argv;
    char buffer[256];
    char *ptr;

    fprintf(out,"/*\n");
    dump_cmd(out);
    fprintf(out," */\n");
    fprintf(out,"/*\n * External definitions needed for ILU %s server stubs\n",
           modulename);
    fprintf(out," */\n\n");
    fprintf(out,"#ifndef _Included_%s_stub_h\n",modulename);
    fprintf(out,"#define _Included_%s_stub_h\n",modulename);

    fprintf(out,"\n\n");
    for (i = 0;i < nbFunctions;i++) {
        f = &tabFunctions[i];
	t = &tabType[f->type];

        fprintf(out,"extern %s ", t->name);
	for (p = 0;p < t->indir;p++) fprintf(out,"*");
	fprintf(out,"%s(", f->name);

        for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

	    if (n != 0) fprintf(out,", ");

	    if (a->retval == TYPE_OUT)
	        fprintf(out,"/*OUT*/ ");
	    if (a->retval == TYPE_INOUT)
	        fprintf(out,"/*INOUT*/ ");

	    fprintf(out,"%s ", t->name);
            for (p = 0;p < t->indir;p++) fprintf(out,"*");
	    fprintf(out,"%s", a->name);
	}
        if (n == 0) fprintf(out,"void");
	fprintf(out,");\n");
    }
    fprintf(out,"extern void register_%s_stubs(void);\n\n", modulename);
    fprintf(out,"#endif /* _Included_%s_stub_h */\n",modulename);
}

void dump_stubs(FILE *out) {
    int i,n,p;
    Function *f;
    Arg *a;
    Type *t;
    Type *rt;

    fprintf(out,"/*\n");
    dump_cmd(out);
    fprintf(out," */\n");
    /*
     * header and inclusion defs.
     */
    fprintf(out,"/*\n * ILU server stubs generated for class %s from includes\n",
           modulename);
    fprintf(out," * The user need to write ILUTypes.h which gives the\n");
    fprintf(out," * signatures of ILU2C and C2ILU conversion handlers\n");
    fprintf(out," * As well as the Class LOCK/UNLOCK macros\n");
    fprintf(out," */\n\n");
    fprintf(out,"#include \"ILUTypes.h\"\n");
    fprintf(out,"#include \"%s.h\"\n", interface_name);
    fprintf(out,"#include \"%s\"\n\n", filename);
    fprintf(out,"#ifndef %s_LOCK\n", modulename);
    fprintf(out,"#define %s_LOCK() fprintf(stderr,\"%s_LOCK undefined\");\n",
            modulename, modulename);
    fprintf(out,"#endif /* %s_LOCK */\n", modulename);
    fprintf(out,"#ifndef %s_UNLOCK\n", modulename);
    fprintf(out,"#define %s_UNLOCK() fprintf(stderr,\"%s_UNLOCK undefined\");\n",
            modulename, modulename);
    fprintf(out,"#endif /* %s_UNLOCK */\n\n", modulename);

    /*
     * Dump each function.
     */
    for (i = 0;i < nbFunctions;i++) {
        f = &tabFunctions[i];
	rt = &tabType[f->type];

        fprintf(out,"/*\n * ILU to C function %s stub.\n */\n",f->name);
	fprintf(out,"%s\nserver_%s_%s_%s(", rt->itype, interface_name,
	       package, f->name);
	fprintf(out,"%s_%s object", interface_name, package);
	for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

	    fprintf(out,", ");
	    fprintf(out,"%s ", t->itype);
	    if (t->convert != NULL)
		fprintf(out,"j");
	    fprintf(out,"%s", a->name);
	}
	fprintf(out,", ILU_C_ENVIRONMENT *env");
	fprintf(out,")\n{\n");

	/*
	 * Allocation of local variable to receive the result in
	 * both internal and external form.
	 */
	if (!strcmp(rt->iname, "ilu.CString"))
	      fprintf(out,"\tchar *res;\n");
	else if (rt->convert != NULL) {
	      fprintf(out,"\t%s result;\n", rt->itype);
	      fprintf(out,"\t%s res;\n", rt->name);
	} else if (strcmp(rt->name, "void"))
	      fprintf(out,"\t%s res;\n", rt->name);

	for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

	    if (t->convert != NULL) {
	       if (!strcmp(t->iname, "ilu.CString")) {
	          fprintf(out,"\tchar %s[1024];\n", a->name);
	          fprintf(out,"\tchar *%s_ptr = &%s[0];\n", a->name, a->name);
	       } else {
		  fprintf(out,"\t%s ", t->name);
		  for (p = 0;p < t->indir;p++) fprintf(out,"*");
		  fprintf(out,"%s;\n", a->name);
	       }
	    }
	}

        /*
	 * initialize the local variables. First the function result,
	 * then the intermediate convertion variables.
	 */
	fprintf(out,"\n");
	for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

            /* if (a->retval == TYPE_OUT) continue; */
	    if (t->convert != NULL) {
	       if (!strcmp(t->iname, "ilu.CString")) {
	          fprintf(out,"\tif (j%s != NULL)\n", a->name);
		  fprintf(out,
		          "\t  iluString2CString(j%s, %s_ptr, sizeof(%s));\n",
		         a->name,a->name,a->name);
	          fprintf(out,"\telse\n");
	          fprintf(out,"\t  %s_ptr = NULL;\n", a->name);
	       } else {
		  fprintf(out,"\t/* convert arg %s j%s to %s ",
			 t->itype, a->name, t->name);
		  for (p = 0;p < t->indir;p++) fprintf(out,"*");
		  fprintf(out,"%s */\n", a->name);
		  fprintf(out,"\tILU%s2C%s",t->iname,t->name);
		  for (p = 0;p < t->indir;p++) fprintf(out,"Ptr");
		  fprintf(out,"(j%s,&%s);\n", a->name, a->name);
	       }
	    }
	}

        /*
	 * Take the lock.
	 */
	fprintf(out,"\n\t%s_LOCK();\n", modulename);

        /*
	 * Call the C function.
	 */
	if (strcmp(rt->name, "void"))
	   fprintf(out,"\n\tres = %s(", f->name);
	else
	   fprintf(out,"\n\t%s(", f->name);
        for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

	    if (n != 0) fprintf(out,", ");
	    fprintf(out,"(%s ", t->name);
            for (p = 0;p < t->indir;p++) fprintf(out,"*");
	    fprintf(out,") %s", a->name);
	}
	fprintf(out,");\n");

        /*
	 * Release the lock.
	 */
	fprintf(out,"\n\t%s_UNLOCK();\n\n", modulename);

	/*
	 * Transform the parameter passed by pointer in the C function.
	 */
	for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

            if (a->retval == TYPE_IN) continue;
	    if (t->convert != NULL) {
	       if (!strcmp(t->iname, "ilu.CString")) {
                  /*********************
	          fprintf(stderr,
		          "%s : Return value for String object not handled\n",
		          f->name);
	          fprintf(out,"\tCString2ILUString(j%s, %s, sizeof(%s));\n",
		         a->name,a->name,a->name);
                   *********************/
	       } else {
		  fprintf(out,"\t/* convert %s ", t->name);
		  for (p = 0;p < t->indir;p++) fprintf(out,"*");
		  fprintf(out,"%s to arg %s j%s */\n",
		          a->name, t->itype, a->name);
		  fprintf(out,"\tC%s",t->name);
		  for (p = 0;p < t->indir;p++) fprintf(out,"Ptr");
		  fprintf(out,"2ILU%s(%s,&j%s);\n",t->iname, a->name, a->name);
	       }
	    }
	}

        /*
	 * in case of a void function, return.
	 */
	if (!strcmp(rt->name, "void")) {
	   fprintf(out,"}\n\n");
	   continue;
	}

	/*
	 * Transform the result to ILU internal value.
	 */
	if (rt->convert != NULL) {
	   if (strcmp(rt->iname, "ilu.CString")) {
	      fprintf(out,"\t/* convert %s ", rt->name);
	      for (p = 0;p < rt->indir;p++) fprintf(out,"*");
	      fprintf(out,"res to %s result */\n", rt->itype);
	      fprintf(out,"\tC%s", rt->name);
	      for (p = 0;p < rt->indir;p++) fprintf(out,"Ptr");
	      fprintf(out,"2ILU%s(res, &result);\n", rt->iname);
	   }
	}

	/*
	 * Return the result, with a cast if needed.
	 */
	fprintf(out,"\n");
	if (rt->convert != NULL) {
	   if (!strcmp(rt->iname, "ilu.CString")) {
	      fprintf(out,"\tif (res == NULL)\n");
	      fprintf(out,"\t\treturn(NULL);\n");
	      fprintf(out,"\telse;\n");
	      fprintf(out,"\t\treturn(makeILUString(res, strlen(res)));\n");
	   } else
	      fprintf(out,"\treturn(result);\n");
        } else {
	   fprintf(out,"\treturn((%s) res);\n", rt->itype);
	}

	fprintf(out,"}\n\n");
    }

    /*
     * Dump the initialization function, responsible for initializing
     * ILU server for this package and publishing it.
     */
    fprintf(out,"/*\n * %s_%s_initServer : call it to instanciate a factory\n",
            interface_name, package);
    fprintf(out," * \t\tand publish the interfaces on an ILU server.\n */\n");
    fprintf(out,"void %s_%s_initServer(ILU_C_Server server) { \n",
            interface_name, package);
    fprintf(out,"#ifdef DEBUG_ILU\n");
    fprintf(out,"    ilu_string stringResourceHandle;\n");
    fprintf(out,"#endif\n");
    fprintf(out,"    static int initialized = 0;\n");
    fprintf(out,"    %s_%s theFactory;\n\n", interface_name, package);
    fprintf(out,"    if (! initialized) %s__InitializeServer();\n", interface_name);
    fprintf(out,"    initialized = 1;\n\n");
    fprintf(out,"    theFactory = %s_%s__CreateTrue(\"%s\", server, NULL);\n",
            interface_name,  package, interface_name);
    fprintf(out,"    if (theFactory == NULL) {\n");
    fprintf(out,"        fprintf (stderr, \"Couldn't create true Resource for %s\\n\");\n",
            interface_name);
    fprintf(out,"        return;\n");
    fprintf(out,"    }\n\n");
    fprintf(out,"    if (! ILU_C_PublishObject(theFactory)) {\n");
    fprintf(out,"        fprintf (stderr, \"Couldn't publish %s resource\\n\");\n",
            interface_name);
    fprintf(out,"        return;\n");
    fprintf(out,"    }\n\n");
    fprintf(out,"#ifdef DEBUG_ILU\n");
    fprintf(out,"    stringResourceHandle = ILU_C_SBHOfObject(theFactory);\n");
    fprintf(out,"    fprintf(stderr, \"Published %s resources as : %%s\\n\",\n",
            interface_name);
    fprintf(out,"            stringResourceHandle);\n");
    fprintf(out,"    ilu_free(stringResourceHandle);\n");
    fprintf(out,"#endif\n");
    fprintf(out,"}\n");
}

int main(int argc, char **argv)
{
	char buffer[MAX_PATH_LENGHT];
	char output[MAX_PATH_LENGHT];
	FILE *out;

	main_argc = argc;
	main_argv = argv;
        getcwd(pwd, sizeof(pwd));

	while (--argc > 0) {
		char *name = *++argv;

                if (name[0] == '-') {
		    switch (name[1]) {
		        case 's':
		        case 'S':
			    stubsOutputFile = *++argv;
			    --argc;
			    break;
		        case 'i':
		        case 'I':
			    iluOutputFile = *++argv;
			    --argc;
			    break;
		        case 'M':
		        case 'm':
			    modulename = *++argv;
			    --argc;
			    break;
		        case 't':
		        case 'T':
			    name = *++argv;
			    --argc;
			    read_type(name);
			    break;
			default:
			    /*
			     * NOTE : dump_h has to be modified accordingly
			     *   if new options are added !
			     */
			    fprintf(stderr,"%s [options]  includes ...\n",
			            argv[0]);
		            fprintf(stderr,"Options :\n");
		            fprintf(stderr,"\t-m modulename\n");
		            fprintf(stderr,"\t-t type_definition_file\n");
		            fprintf(stderr,"\t-i ilu_ISL_output_file\n");
		            fprintf(stderr,"\t-s ilu_stubs_output_file\n");
		            exit(1);
		    }
		    continue;
		}
		if (name[0] != '/') {
		   strcpy(buffer, pwd);
                   strcat(buffer, "/");
                   strcat(buffer, name);
		   name = &buffer[0];
		}

		filename = name;

		/*
		 * parse the file for definitions.
		 */
		nbFunctions = 0;
		do_parse();

		/* Split modulename into a package name and an interface name */
		strcpy(package,modulename);
		idx = strlen(package) - 1;
		for (;idx >= 0;idx--) {
		    if (package[idx] == '_') {
		       if (!is_package) {
			  package[idx] = '\0';
			  interface_name = &package[idx + 1];
		       } else
			  package[idx] = '.';
		       is_package = 1;
		    }
		}
		*interface_name = toupper(*interface_name);

                /*
		 * Create the filenames.
		 */
		if (iluOutputFile == NULL) {
		    sprintf(output, "%s.isl", modulename);
		    iluOutputFile = strdup(output);
		}
		if (stubsOutputFile == NULL) {
		    sprintf(output, "%s-stubs.c", interface_name);
		    stubsOutputFile = strdup(output);
		}

                /*
		 * Dump the expected results.
		 */
		if ((out = fopen(iluOutputFile,"w")) == NULL) {
		    fprintf(stderr,"Cannot open %s\n", output);
		    exit(1);
		}
		dump_ilu(out);
		fclose(out);
		if ((out = fopen(stubsOutputFile,"w")) == NULL) {
		    fprintf(stderr,"Cannot open %s\n", output);
		    exit(1);
		}
		dump_stubs(out);
		fclose(out);
	}
	return 0;
}



