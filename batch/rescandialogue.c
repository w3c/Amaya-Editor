
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
  Rescan dialogue files
  by Irene Vatton
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------------------------------------------------------- */
int main(int argc, char **argv)
{
  FILE *file1, *file2;
  int  i, val, len, comment;
  char s[1000], name[200], *ptr;

  if (argc < 2)
    printf("usage: rescandialogue filename\n");
  else
    {
      strcpy(name, argv[1]);
      strcat(name, ".old");
      if (rename (argv[1], name) < 0)
	{
	  printf("cannot rename %s into %s.old\n", argv[1], argv[1]);
	  exit (-1);
	}
      file1 = fopen(name, "r"); /* Open the old file */
      file2 = fopen(argv[1], "w"); /* Open the new file */
      if (!file1 || !file2)
	{
	  printf("cannot open %s or %s.new\n", argv[1], argv[1]);
	  /* restore the old name */
	  rename (name, argv[1]);
	}
      else
	{
          comment = 0;
	  i = 0;
	  len = 0;
	  ptr = NULL;
	  val = fread(&s[len], 1, 1, file1);
	  while (val != 0)
	    {
	      if (ptr == NULL && len == 0 && s[len] == '#')
		{
		  /* a line starting with '#' is considered as comment.
		     it is not numbered */
		  ptr = &s[len];
		  len++;
		  comment = 1;
		}
	      else if (ptr == NULL && (s[len] == ' ' || s[len] == '\t'))
		{
		  /* first space after the number */
		  ptr = &s[len];
		  len++;
		}
	      else if (s[len] == '\n')
		{
		  /* end of input line */
		  s[len] = '\0';
		  if (ptr != NULL)
		    {
		      if (comment)
			/* It's a comment linr. Output it as is */
			fprintf(file2, "%s\n", ptr);
		      else
			{
			  fprintf(file2, "%d%s\n", i, ptr);
			  i++;
			}
		    }
		  comment = 0;
		  len = 0;
		  ptr = NULL;
		}
	      else
		len++;
	      val = fread(&s[len], 1, 1, file1);
	    }
	  fclose(file1);
	  fclose(file2);
	}
    }
  return 0;
}
