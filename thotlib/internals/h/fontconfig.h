#ifndef __FONTCONFIG_H
#define __FONTCONFIG_H

int isnum(char number);
void FreeFontConfig ();
char *FontLoadFromConfig (char script, int family, int highlight);

#endif /*__FONTCONFIG_H*/

