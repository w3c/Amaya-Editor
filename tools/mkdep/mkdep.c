#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>

/* #define DEBUG_HASH */

#define MAX_PATH	100
#define MAX_PATH_LENGHT	256
#define MAX_INCLUDE	200
#define MAX_SUB_INCLUDE 30

/*
 * include tree + hash table access 
 */

typedef struct include_dep {
    struct include_dep *next; /* hash table link */
    char *name;               /* complete absolute filename */
    int nb_includes;          /* number of sub_includes or -1 if not filled */
    char *includes[MAX_SUB_INCLUDE]; /* full name of dependancies */
} *inc_dep;

inc_dep inc_hash[256];
int nb_include_in_hash = 0;

int get_hash(char *filename)
{
    char idx;
    char *cour = &filename[strlen(filename)];

    do {
        idx = *cour;
        cour--;
        if (*cour == '/') return((int) idx);
    } while (cour > filename);
    return((int) idx);
}

void init_dep()
{
    int i;

#ifdef DEBUG_HASH
    fprintf(stderr,"Init_dep\n");
#endif

    for (i = 0;i < 256;i++) inc_hash[i] = NULL;
    nb_include_in_hash = 0;
}

inc_dep add_inc(char *filename)
{
    inc_dep prev = NULL, cour;
    int res;
    int hash = get_hash(filename);

    cour = inc_hash[hash];

    /*
     * go through the list looking for the include.
     */
    while (cour != NULL) {
        res = strcmp(cour->name, filename);
        if (res == 0) return(cour);
        if (res < 0) break;
        prev = cour;
        cour = cour->next;
    }

#ifdef DEBUG_HASH
    fprintf(stderr,"Adding in slot %d : %s\n", hash, filename);
#endif

    /*
     * not found, allocate and fill a new one.
     */
    nb_include_in_hash++;
    cour = (inc_dep) malloc(sizeof(struct include_dep));
    cour->name = strdup(filename);
    cour->nb_includes = -1;

    /*
     * not found, allocate and fill a new one.
     */
    if (prev == NULL) {
        /*
         * add at the head of the hash list.
         */
        cour->next = inc_hash[hash];
        inc_hash[hash] = cour;
    } else {
        /*
         * add if after prev.
         */
        cour->next = prev->next;
        prev->next = cour;
    }
    return(cour);
}

void add_inc_dep(inc_dep cour, char *filename)
{
    if (cour->nb_includes < 0)
        cour->nb_includes = 0;
    if (cour->nb_includes == MAX_SUB_INCLUDE) {
	fprintf(stderr,"increase MAX_SUB_INCLUDE : %d not sufficient\n",
                MAX_SUB_INCLUDE);
        return;
    }
    cour->includes[cour->nb_includes++] = strdup(filename);
}

char *filename, *command, __depname[MAX_PATH_LENGHT] = "\n\t@touch ";

#define depname (__depname+9)

struct path_struct {
	int len;
	char buffer[MAX_PATH_LENGHT-sizeof(int)];
} path_array[MAX_PATH] = {
	{  0, "" }
};

int nb_path = 0;

typedef char include_name[MAX_PATH_LENGHT];

include_name include_list[MAX_INCLUDE];

int nb_include = 0;

int handling_includes = 0;
inc_dep current_include = NULL;

static void add_local_include(char *name)
{
	int i = 0;

        add_inc(name);
        if ((handling_includes != 0) &&
            (current_include != NULL))
            add_inc_dep(current_include, name);

        for (i = 0;i < nb_include;i++) {
	    if (!strcmp(include_list[i], name)) return;
	}
	strcpy(include_list[nb_include], name);
	nb_include++;
}

static void handle_local_include(char *name, int len)
{
	int plen;
	struct path_struct *path;
	int i = 0;

        for (i = 0;i < nb_path;i++) {
	   path = &path_array[i];
	   plen = path->len;
	   memcpy(path->buffer+plen, name, len);
	   plen += len;
	   path->buffer[plen] = '\0';
	   if (access(path->buffer, F_OK))
		   continue;

	   add_local_include(path->buffer);
	   return;
	}
#ifdef WITH_DEBUG
	{ 
	   char file_name[MAX_PATH_LENGHT];
	   strncpy(file_name,name,len);
	   file_name[len] = '\0';
	   fprintf(stderr,"Path for include %s not found\n", file_name);
	}
#endif
}

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

#define GETNEXT { \
next_byte(__buf); \
if (!__nrbuf) { \
        if (next >= mmap_map + filesize) return; /* For suns ! */ \
	__buf = *(unsigned long *) next; \
	__nrbuf = sizeof(unsigned long); \
	if (!__buf) \
		break; \
} next++; __nrbuf--; }
#define CASE(c,label) if (current == c) goto label
#define NOTCASE(c,label) if (current != c) goto label

static void state_machine(register char *next)
{
	for(;;) {
	register unsigned long __buf = 0;
	register unsigned long __nrbuf = 0;

normal:
	GETNEXT
__normal:
	CASE('/',slash);
	CASE('"',string);
	CASE('\'',char_const);
	CASE('#',preproc);
	goto normal;

slash:
	GETNEXT
	CASE('*',comment);
	goto __normal;

string:
	GETNEXT
	CASE('"',normal);
	NOTCASE('\\',string);
	GETNEXT
	goto string;

char_const:
	GETNEXT
	CASE('\'',normal);
	NOTCASE('\\',char_const);
	GETNEXT
	goto char_const;

comment:
	GETNEXT
__comment:
	NOTCASE('*',comment);
	GETNEXT
	CASE('/',normal);
	goto __comment;

preproc:
	GETNEXT
	CASE('\n',normal);
	CASE(' ',preproc);
	CASE('\t',preproc);
	CASE('i',i_preproc);
	GETNEXT

skippreproc:
	CASE('\n',normal);
	CASE('\\',skippreprocslash);
	GETNEXT
	goto skippreproc;

skippreprocslash:
	GETNEXT;
	GETNEXT;
	goto skippreproc;

i_preproc:
	GETNEXT
	CASE('f',if_line);
	NOTCASE('n',skippreproc);
	GETNEXT
	NOTCASE('c',skippreproc);
	GETNEXT
	NOTCASE('l',skippreproc);
	GETNEXT
	NOTCASE('u',skippreproc);
	GETNEXT
	NOTCASE('d',skippreproc);
	GETNEXT
	NOTCASE('e',skippreproc);

/* "# include" found */
include_line:
	GETNEXT
	CASE('\n',normal);
	NOTCASE('"', include_line);

/* "local" include file */
{
	char *incname = next;
local_include_name:
	GETNEXT
	CASE('\n',normal);
	NOTCASE('"', local_include_name);
	handle_local_include(incname, next-incname-1);
	goto skippreproc;
}

if_line:
if_start:
	GETNEXT
	CASE('\n', normal);
	CASE('_', if_middle);
	if (current >= 'a' && current <= 'z')
		goto if_middle;
	if (current < 'A' || current > 'Z')
		goto if_start;

if_middle:
	GETNEXT
	CASE('\n', normal);
	CASE('_', if_middle);
	if (current >= 'a' && current <= 'z')
		goto if_middle;
	if (current < 'A' || current > 'Z')
		goto if_start;
	goto if_middle;
	}
}

static void do_depend(void)
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
	state_machine(mmap_map);
	munmap(mmap_map, mmap_mapsize);
}

int main(int argc, char **argv)
{
	int i;
	int my_argc = argc;
	char **my_argv = argv;
	char pwd[MAX_PATH_LENGHT];
	char buffer[MAX_PATH_LENGHT];

        init_dep();
        getcwd(pwd, sizeof(pwd));

	while (--my_argc > 0) {
		int len;
		char *name = *++my_argv;

                if ((name[0] == '-') && (name[1] == 'I')) {
		    if (name[2] != '/') {
		        strcpy(buffer, "-I");
		        strcat(buffer, pwd);
		        strcat(buffer, "/");
		        strcat(buffer, &name[2]);
		        name = &buffer[0];
		    }

		    strcpy(path_array[nb_path].buffer, &name[2]);
		    len = strlen(path_array[nb_path].buffer);
		    if (path_array[nb_path].buffer[len - 1] != '/') {
		        strcat(path_array[nb_path].buffer,"/");
			len++;
		    }
		    path_array[nb_path].len = len;
		    nb_path++;
		    path_array[nb_path].len = 0;
		}
	}
	while (--argc > 0) {
		int len;
		char *name = *++argv;

                if (name[0] == '-') continue;
		if (name[0] != '/') {
		   strcpy(buffer, pwd);
                   strcat(buffer, "/");
                   strcat(buffer, name);
		   name = &buffer[0];
		}

		filename = name;
		len = strlen(name);
		memcpy(depname, name, len+1);
		command = __depname;
		if (len > 2 && name[len-2] == '.') {
			switch (name[len-1]) {
				case 'c':
				case 'S':
					depname[len-1] = 'o';
					command = "";
			}
		}
		nb_include = 0;

                /*
		 * print the base dependancy between the .o and .c file.
		 */
	        printf("%s : %s", depname,filename);

		/*
		 * do basic dependancies on the C source file.
		 */
                handling_includes = 0;
		do_depend();

		/*
		 * recurse dependancies on the included files.
		 * Warning, nb_include will grow over the loop.
		 */
                handling_includes = 1;
		for (i = 0;i < nb_include;i++) {
		    filename = include_list[i];
		    /*
		     * check the hash for existing dependencies.
		     * !!!
		     */
		    if (handling_includes) {
		        current_include = add_inc(filename);
		        if (current_include->nb_includes >= 0) {
                            int i;

			    /*
			     * do not load and parse the file,
                             * dump the cache instead.
			     */
                            for (i = 0;i < current_include->nb_includes;i++)
                                add_local_include(current_include->includes[i]);

                            continue;
		        }
		    }
		    current_include = NULL;
		    do_depend();
		}

                /*
		 * dump the dependancies found.
		 */
		for (i = 0;i < nb_include;i++)
		    printf(" \\\n   %s", include_list[i]);
		printf("\n\n");
	}
#ifdef DEBUG_HASH
        fprintf(stderr,"total %d includes in hash\n", nb_include_in_hash);
#endif
	return 0;
}
