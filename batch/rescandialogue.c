
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
  Rescan dialogue files
  by Irene Vatton
  */
#include <stdio.h>
#include <string.h>

/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void main(int argc, char **argv)
#else /* __STDC__ */
main(argc, argv)
	int argc;
	char **argv;
#endif /* __STDC__ */
{
  FILE *file1, *file2;
  int  i, val, len;
  char s[1000], name[200], *ptr;

  if (argc < 2)
    printf("usage: rescandialogue filename\n");
  else
    {
      strcpy(name, argv[1]);
      strcat(name, ".new");
      file1 = fopen(argv[1], "r"); /* Open the new file */
      file2 = fopen(name, "w"); /* Open the old file */
      if (!file1 || !file2)
	printf("cannot open %s or %s.new\n", argv[1], argv[1]);
      else
	{
	  i = 0;
	  len = 0;
	  ptr = NULL;
	  val = fread(&s[len], 1, 1, file1);
	  while (val != 0)
	    {
	      if (ptr == NULL && (s[len] == ' ' || s[len] == '\t'))
		{
		  /* the first space after the number */
		  ptr = &s[len];
		  len++;
		}
	      else if (s[len] == '\n')
		{
		  /* the end of new line */
		  s[len] = '\0';
		  fprintf(file2, "%d%s\n", i, ptr);
		  len = 0;
		  ptr = NULL;
		  i++;
		}
	      else
		len++;
	      val = fread(&s[len], 1, 1, file1);
	    }
	  fclose(file1);
	  fclose(file2);
	  
	}
    }
}
