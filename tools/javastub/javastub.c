/*
 * javastub.c: Kaffe stub generator using include external definition
 *             of functions.
 *
 *  Need :
 *     1/ better cleanup (deallocation of strduped strings).
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
#ifdef HAVE_MMAP
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
    char *jname;  /* java name of the type e.g. "int"       */
    char *itype;  /* java internal type                     */
    char *convert;/* Need to provide a converter C2J and J2C */
} Type;

/*
 * add more type as needed.
 */

int nbTypes = 5;
Type tabType[1000] = {
{ 0, TYPE_IN, "void",	"",		"void",			NULL},
{ 0, TYPE_IN, "boolean","boolean",	"jint",			NULL},
{ 0, TYPE_IN, "int",	"int",		"jint",			NULL},
{ 1, TYPE_IN, "char",	"String",	"struct Hjava_lang_String*", "java_lang_String"},
{ 1, TYPE_OUT,"char",	"StringBuffer",	"struct Hjava_lang_StringBuffer*", "java_lang_StringBuffer"},
};

/*
 * Parsed enum val definition.
 */
typedef struct _EnumVal {
    struct _EnumVal *next;
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
char *classname = "UnknownClassName";

int nbFunctions = 0;
Function tabFunctions[MAX_FUNCTION];

int nbConstants = 0;
Constant tabConstants[MAX_CONSTANT];

int nbEnum = 0;
Enum tabEnum[MAX_ENUM];

char *comment_start = NULL;
char *comment_end = NULL;

char *javaOutputFile = NULL;
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
 * read_type : read a sequence of type definitions from an external file.
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
    char jname[256];
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

	/* read the Java type name */
	q = &jname[0];
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

	/* read the Java internal type */
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

        /* read the Java internal type name if any */
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
	tabType[nbTypes].jname = strdup(jname);
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

    if (*next >= mmap_map + filesize) return(-1);
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

static char *parse_identifier(char **next)
{
    static char identifier[256];
    int index = 0;
    char *p = *next;

    if (*next >= mmap_map + filesize) return(NULL);;
    SKIP_BLANK(p)
    if (IS_ALPHA(*p)) {
        identifier[index++] = *p++;
	while ((IS_ALPHA(*p)) || (IS_NUM(*p)))
	   identifier[index++] = *p++;
	identifier[index] = '\0';
	*next = p;
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
	 * without added value are useless from Java side.
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
	name = parse_identifier(&p);
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
	mmap_map = mmap(NULL, mmap_mapsize, PROT_READ, MAP_PRIVATE, fd, 0);
	if (-1 == (long)mmap_map) {
		perror("mkdep: mmap");
		close(fd);
		return;
	}
	res = close(fd);
	parse(mmap_map);
	res = munmap(mmap_map, mmap_mapsize);
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
    int argc = main_argc;
    char **argv = main_argv;

    fprintf(out," * DO NOT EDIT\n");
    fprintf(out," * Generated by the following command from %s directory\n * ",
            pwd);

    while (argc-- > 0) {
       char *name = *argv++;

       fprintf(out,"%s ", name);
    }
    fprintf(out,"\n");
}

void dump_java(FILE *out) {
    int i,n;
    Function *f;
    Constant *c;
    Arg *a;
    Type *t;
    Enum e;
    char package[256];
    int idx;
    int is_package = 0;
    char *class_name = &package[0];

    /* Split classname into a package name and an class name */
    strcpy(package,classname);
    idx = strlen(package) - 1;
    for (;idx >= 0;idx--) {
        if (package[idx] == '_') {
	   if (!is_package) {
	      package[idx] = '\0';
	      class_name = &package[idx + 1];
	   } else
	      package[idx] = '.';
	   is_package = 1;
	}
    }
    *class_name = toupper(*class_name);
    fprintf(out,"/*\n");
    dump_cmd(out);
    fprintf(out," */\n");
    if (is_package) fprintf(out,"package %s;\n\n",package);


    fprintf(out,"\n/*\n");
    fprintf(out," * Java public native definitions for class %s\n",
           classname);
    fprintf(out," */\n\n");
    fprintf(out,"public class %s {\n", class_name);

    /*
     * Dump each constant.
     */
    for (i = 0;i < nbConstants;i++) {
        c = &tabConstants[i];
	t = &tabType[c->type];
	fprintf(out," public static final %s %s = %s;\n",
                t->jname, c->name, c->string);
    }
    if (nbConstants > 0) fprintf(out,"\n");

    /*
     * Dump each enum values.
     */
    for (i = 0;i < nbEnum;i++) {
        e = tabEnum[i];
	while (e != NULL) {
	    fprintf(out," public static final int %s = %d;\n",
		    e->name, e->value);
	    e = e->next;
	}
	fprintf(out,"\n");
    }


    for (i = 0;i < nbFunctions;i++) {
        f = &tabFunctions[i];
	t = &tabType[f->type];

        if (f->comment != NULL)
	    fprintf(out,"\n\n/*%s*/\n", f->comment);
	fprintf(out," public static native ");
	if (t->jname[0] != '\0')
	   fprintf(out,"%s ", t->jname);
	else
	   fprintf(out,"void ");
	fprintf(out,"%s(", f->name);

        for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

	    if (n != 0) fprintf(out,", ");

	    if (a->retval == TYPE_OUT)
	        fprintf(out,"/*OUT*/ ");
	    if (a->retval == TYPE_INOUT)
	        fprintf(out,"/*INOUT*/ ");

	    fprintf(out,"%s ", t->jname);
	    fprintf(out,"%s", a->name);
	}
	/* if (n == 0) fprintf(out,"void"); */
	fprintf(out,");\n");
    }
    fprintf(out,"}\n");
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
    fprintf(out,"/*\n * External definitions needed for class %s stubs\n",
           classname);
    fprintf(out," */\n\n");
    fprintf(out,"#ifndef _Included_%s_stub_h\n",classname);
    fprintf(out,"#define _Included_%s_stub_h\n",classname);

    while (--argc > 0) {
       char *name = *++argv;

       if (name[0] == '-') {
	   switch (name[1]) {
	       case 'c':
	       case 'C':
		   ++argv;
		   --argc;
		   break;
	       case 'h':
	       case 'H':
		   ++argv;
		   --argc;
		   break;
	       case 'j':
	       case 'J':
		   ++argv;
		   --argc;
		   break;
	       case 't':
	       case 'T':
		   ++argv;
		   --argc;
		   break;
	       case 'M':
	       case 'm':
		   ++argv;
		   --argc;
		   break;
	   }
	   continue;
       }
       ptr = &buffer[0];
       do {
           if (*name == '/') ptr = &buffer[0];
	   else if (*name == '\\') ptr = &buffer[0];
	   else *ptr++ = *name;
       } while (*name++ != 0);
       fprintf(out,"#include \"%s\"\n", &buffer[0]);
    }
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
    fprintf(out,"extern void register_%s_stubs(void);\n\n", classname);
    fprintf(out,"#endif /* _Included_%s_stub_h */\n",classname);
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
    fprintf(out,"/*\n * Kaffe stubs generated for class %s from includes\n",
           classname);
    fprintf(out," * The user need to write JavaTypes.h which gives the\n");
    fprintf(out," * signatures of Java2C and C2Java conversion handlers\n");
    fprintf(out," * As well as the Class LOCK/UNLOCK macros\n");
    fprintf(out," */\n\n");
    fprintf(out,"#include <native.h>\n");
    fprintf(out,"#include \"JavaTypes.h\"\n");
    fprintf(out,"#include \"%s.h\"\n", classname);
    fprintf(out,"#include \"%s\"\n\n", stubHOutputFile);
    fprintf(out,"#ifndef %s_LOCK\n", classname);
    fprintf(out,"#define %s_LOCK() fprintf(stderr,\"%s_LOCK undefined\");\n",
            classname, classname);
    fprintf(out,"#endif /* %s_LOCK */\n", classname);
    fprintf(out,"#ifndef %s_UNLOCK\n", classname);
    fprintf(out,"#define %s_UNLOCK() fprintf(stderr,\"%s_UNLOCK undefined\");\n",
            classname, classname);
    fprintf(out,"#endif /* %s_UNLOCK */\n\n", classname);

    /*
     * Dump each function.
     */
    for (i = 0;i < nbFunctions;i++) {
        f = &tabFunctions[i];
	rt = &tabType[f->type];

        fprintf(out,"/*\n * Java to C function %s stub.\n */\n",f->name);
	fprintf(out,"%s\n%s_%s(struct H%s* none", rt->itype, classname,
	       f->name, classname);
	for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

	    fprintf(out,", ");
	    fprintf(out,"%s ", t->itype);
	    if (t->convert != NULL)
		fprintf(out,"j");
	    fprintf(out,"%s", a->name);
	}
	fprintf(out,")\n{\n");

	/*
	 * Allocation of local variable to receive the result in
	 * both internal and external form.
	 */
	if (!strcmp(rt->jname, "String"))
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
	       if (!strcmp(t->jname, "String")) {
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
	       if (!strcmp(t->jname, "String")) {
	          fprintf(out,"\tif (j%s != NULL)\n", a->name);
		  fprintf(out,
		          "\t  javaString2CString(j%s, %s_ptr, sizeof(%s));\n",
		         a->name,a->name,a->name);
	          fprintf(out,"\telse\n");
	          fprintf(out,"\t  %s_ptr = NULL;\n", a->name);
	       } else {
		  fprintf(out,"\t/* convert arg %s j%s to %s ",
			 t->itype, a->name, t->name);
		  for (p = 0;p < t->indir;p++) fprintf(out,"*");
		  fprintf(out,"%s */\n", a->name);
		  fprintf(out,"\tJava%s2C%s",t->jname,t->name);
		  for (p = 0;p < t->indir;p++) fprintf(out,"Ptr");
		  fprintf(out,"(j%s,&%s);\n", a->name, a->name);
	       }
	    }
	}

        /*
	 * Take the lock.
	 */
	fprintf(out,"\n\t%s_LOCK();\n", classname);

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
	    if (!strcmp(t->jname, "String")) {
		fprintf(out,") %s_ptr", a->name);
	    } else {
		fprintf(out,") %s", a->name);
	    }
	}
	fprintf(out,");\n");

        /*
	 * Release the lock.
	 */
	fprintf(out,"\n\t%s_UNLOCK();\n\n", classname);

	/*
	 * Transform the parameter passed by pointer in the C function.
	 */
	for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

            if (a->retval == TYPE_IN) continue;
	    if (t->convert != NULL) {
	       if (!strcmp(t->jname, "String")) {
                  /*********************
	          fprintf(stderr,
		          "%s : Return value for String object not handled\n",
		          f->name);
	          fprintf(out,"\tCString2JavaString(j%s, %s, sizeof(%s));\n",
		         a->name,a->name,a->name);
                   *********************/
	       } else {
		  fprintf(out,"\t/* convert %s ", t->name);
		  for (p = 0;p < t->indir;p++) fprintf(out,"*");
		  fprintf(out,"%s to arg %s j%s */\n",
		          a->name, t->itype, a->name);
		  fprintf(out,"\tC%s",t->name);
		  for (p = 0;p < t->indir;p++) fprintf(out,"Ptr");
		  fprintf(out,"2Java%s(%s,&j%s);\n",t->jname, a->name, a->name);
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
	 * Transform the result to Java internal value.
	 */
	if (rt->convert != NULL) {
	   if (strcmp(rt->jname, "String")) {
	      fprintf(out,"\t/* convert %s ", rt->name);
	      for (p = 0;p < rt->indir;p++) fprintf(out,"*");
	      fprintf(out,"res to %s result */\n", rt->itype);
	      fprintf(out,"\tC%s", rt->name);
	      for (p = 0;p < rt->indir;p++) fprintf(out,"Ptr");
	      fprintf(out,"2Java%s(res, &result);\n", rt->jname);
	   }
	}

	/*
	 * Return the result, with a cast if needed.
	 */
	fprintf(out,"\n");
	if (rt->convert != NULL) {
	   if (!strcmp(rt->jname, "String")) {
	      fprintf(out,"\tif (res == NULL)\n");
	      fprintf(out,"\t\treturn(NULL);\n");
	      fprintf(out,"\telse;\n");
	      fprintf(out,"\t\treturn(makeJavaString(res, strlen(res)));\n");
	   } else
	      fprintf(out,"\treturn(result);\n");
        } else {
	   fprintf(out,"\treturn((%s) res);\n", rt->itype);
	}

	fprintf(out,"}\n\n");
    }

    /*
     * Generate the registering procedure.
     */

    fprintf(out,"/*\n * Function to register all %s stubs.\n */\n", classname);
    fprintf(out,"void register_%s_stubs(void)\n{\n", classname);
    for (i = 0;i < nbFunctions;i++) {
        f = &tabFunctions[i];
	fprintf(out,"\taddNativeMethod(\"%s_%s\", %s_%s);\n",
	        classname, f->name, classname, f->name);
    }
    fprintf(out,"}\n\n");
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
		        case 'j':
		        case 'J':
			    javaOutputFile = *++argv;
			    --argc;
			    break;
		        case 'h':
		        case 'H':
			    stubHOutputFile = *++argv;
			    --argc;
			    break;
		        case 'c':
		        case 'C':
			    stubCOutputFile = *++argv;
			    --argc;
			    break;
		        case 'M':
		        case 'm':
			    classname = *++argv;
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
		            fprintf(stderr,"\t-c c_stubs_output_file\n");
		            fprintf(stderr,"\t-h h_stubs_output_file\n");
		            fprintf(stderr,"\t-j java_class_output_file\n");
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
		 * Create the filenames.
		 */
		if (stubCOutputFile == NULL) {
		    sprintf(output, "%s_stubs.c", classname);
		    stubCOutputFile = strdup(output);
		}
		if (javaOutputFile == NULL) {
		    sprintf(output, "%s.java", classname);
		    javaOutputFile = strdup(output);
		}
		if (stubHOutputFile == NULL) {
		    sprintf(output, "%s_stubs.h", classname);
		    stubHOutputFile = strdup(output);
		}

		/*
		 * parse the file for definitions.
		 */
		nbFunctions = 0;
		do_parse();

                /*
		 * Dump the expected results.
		 */
		if ((out = fopen(stubCOutputFile,"w")) < 0) {
		    fprintf(stderr,"Cannot open %s\n", output);
		    exit(1);
		}
		dump_stubs(out);
		fclose(out);
		if ((out = fopen(javaOutputFile,"w")) < 0) {
		    fprintf(stderr,"Cannot open %s\n", output);
		    exit(1);
		}
		dump_java(out);
		fclose(out);
		if ((out = fopen(stubHOutputFile,"w")) < 0) {
		    fprintf(stderr,"Cannot open %s\n", output);
		    exit(1);
		}
		dump_h(out);
		fclose(out);

	}
	return 0;
}
