/*
 * javastub.c: Kaffe stub generator using include external definition
 *             of functions.
 *
 *  Need :
 *     1/ support C comments in function decl
 *     2/ better cleanup (deallocation of strduped strings).
 *     3/ import types from an external (text) file.
 */

#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>

#define DEBUG_STUB
/* #define DEBUG_HASH */

#define MAX_PATH	100
#define MAX_PATH_LENGHT	256
#define MAX_INCLUDE	200
#define MAX_SUB_INCLUDE 30

#define MAX_ARGS	30
#define MAX_FUNCTION	250

/*
 * Registered type definition.
 */
typedef struct _Type {
    int   indir;  /* level of indirection, e.g. char * -> 1 */
    char *name;   /* name of the type e.g. "int"            */
    char *jname;  /* java name of the type e.g. "int"       */
    char *itype;  /* java internal type                     */
    char *convert;/* Need to provide a converter C2J and J2C */
} Type;

/*
 * add more type as needed.
 */

int nbTypes = 4;
Type tabType[1000] = {
{ 0, "void",	"",		"void",			NULL},
{ 0, "boolean",	"boolean",	"jint",			NULL},
{ 0, "int",	"int",		"jint",			NULL},
{ 1, "char",	"String",	"struct Hjava_lang_String*", "java_lang_String"},
};

/*
 * Parsed arg definition.
 */
typedef struct _Arg {
    char *name;   /* name of the arg e.g. "doc" if any */
    int   type;   /* index of the type in tabType */
} Arg;

/*
 * Parsed function definition.
 */
typedef struct _Function {
    char *name;   /* name of the C function e.g. "CreateDoc" */
    int   type;   /* index in tabType of the returned type   */
    int   nb_args;/* number of arguments */
    Arg   args[MAX_ARGS]; /* the Arguments       */
} Function;

/*
 * global variables.
 */
char *filename;
char *classname = "UnknownClassName";

int nbFunctions = 0;
Function tabFunctions[MAX_FUNCTION];

char *javaOutputFile = NULL;
char *stubCOutputFile = NULL;
char *stubHOutputFile = NULL;

int main_argc;
char **main_argv;
char pwd[MAX_PATH_LENGHT];


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
    *next = ++p;
    return(NULL);
}

static int parse_type(char **next)
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
	        (indir == tabType[i].indir))
	        return(i);
	if (verbose) {
	    fprintf(stderr,"Function %s : unknown type \"%s ",
	            tabFunctions[nbFunctions].name, name);
	    for (i = 0; i < indir;i++) fprintf(stderr,"*");
	    fprintf(stderr,"\"\n");
	}
        return(-1);
    }
    *next = ++p;
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

    while (1) {
        if (p >= mmap_map + filesize) {
	    *next = p;
	    return;
	}
        return_type = parse_type(&p);
	if (return_type < 0) continue;
	func_name = parse_identifier(&p);
	if (func_name == NULL) continue;
	SKIP_BLANK(p)

	if (*p != '(') continue;
	p++;
        
	/* check for redefinitions */
	for (i = 0;i < nbFunctions;i++)
	    if (!strcmp(func_name, tabFunctions[i].name)) goto cleanup;

        /* add preliminary to the table, don't increase nbFunctions now */
	tabFunctions[nbFunctions].name = strdup(func_name);
	tabFunctions[nbFunctions].type = return_type;
	tabFunctions[nbFunctions].nb_args = 
	nb_arg = 0;
	verbose = 1;
	while (1) {
	    SKIP_BLANK(p)
	    if (*p == ')') break;
	    arg_type = parse_type(&p);
	    if (arg_type < 0) goto cleanup;
	    if (!strcmp(tabType[arg_type].name, "void")) {
	        SKIP_BLANK(p);
		if (*p == ')') break;
	        goto cleanup;
	    }
	    arg_name = parse_identifier(&p);

	    if (arg_name == NULL) {
	        sprintf(dump_arg_name, "arg_%d", nb_arg + 1);
		arg_name = &dump_arg_name[0];
	    }
	    tabFunctions[nbFunctions].args[nb_arg].name = strdup(arg_name);
	    tabFunctions[nbFunctions].args[nb_arg].type = arg_type;
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
	p++;
        nbFunctions++;
	verbose = 0;
	continue;
        
cleanup:
	verbose = 0;
	p++;
    }
}

static void parse(char *next)
{
    while (1) {
        if (next >= mmap_map + filesize) return;
        parse_function(&next);
    }
}

static void do_parse(void)
{
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
	mmap_map = mmap(NULL, mmap_mapsize, PROT_READ, MAP_PRIVATE, fd, 0);
	if (-1 == (long)mmap_map) {
		perror("mkdep: mmap");
		close(fd);
		return;
	}
	close(fd);
	parse(mmap_map);
	munmap(mmap_map, mmap_mapsize);
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
    Arg *a;
    Type *t;
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
    for (i = 0;i < nbFunctions;i++) {
        f = &tabFunctions[i];
	t = &tabType[f->type];

	fprintf(out,"\tpublic static native ");
	if (t->jname[0] != '\0')
	   fprintf(out,"%s ", t->jname);
	else
	   fprintf(out,"void ");
	fprintf(out,"%s(", f->name);

        for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

	    if (n != 0) fprintf(out,", ");
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
       fprintf(out,"#include \"%s\"\n", name);
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
    fprintf(out,"#define %s_LOCK()\n", classname);
    fprintf(out,"#endif /* %s_LOCK */\n", classname);
    fprintf(out,"#ifndef %s_UNLOCK\n", classname);
    fprintf(out,"#define %s_UNLOCK()\n", classname);
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
	/*********************** BUG *************************
	if ((rt->convert != NULL) && (strcmp(rt->jname, "String"))) {
	   fprintf(out,"\tresult = (%s)\n", rt->itype);
	   fprintf(out,"\t\texecute_java_constructor(0, \"%s\",", rt->convert);
	   fprintf(out,"\n\t\t 0, VOID_SIGNATURE);\n");
	}
	 *****************************************************/
	for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

	    if (t->convert != NULL) {
	       if (!strcmp(t->jname, "String")) {
	          fprintf(out,"\tjavaString2CString(j%s, %s, sizeof(%s));\n",
		         a->name,a->name,a->name);
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
	    fprintf(out,") %s", a->name);
	}
	fprintf(out,");\n");

        /*
	 * Release the lock.
	 */
	fprintf(out,"\n\t%s_UNLOCK();\n", classname);

	/*
	 * Transform the parameter passed by pointer in the C function.
	 */
	for (n = 0;n < f->nb_args;n++) {
	    a = &f->args[n];
	    t = &tabType[a->type];

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
	   if (!strcmp(rt->jname, "String"))
	      fprintf(out,"\treturn(makeJavaString(res, sizeof(res)));\n");
	   else
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
#ifdef DEBUG_HASH
        fprintf(stderr,"total %d includes in hash\n", nb_include_in_hash);
#endif
	return 0;
}
