
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void out_char ( int type, int outch );
extern void parse_config ( char *conf_fname );
extern int main ( int argc, char **argv );
extern void send_first_comment ( char *begin_str );

#else /* __STDC__ */

extern void out_char (/* int type, int outch */);
extern void parse_config (/* char *conf_fname */);
extern int main (/* int argc, char **argv */);
extern void send_first_comment (/* char *begin_str */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
