/*
 * dns_daemon.c : ask and return to DNS requests.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

#include <ctype.h>
#include <string.h>

void handle_request(const char *hostname) {
    int i,j;
    unsigned char *addr;
    struct hostent *result = NULL;
    
    if (hostname == NULL) return;

    result = gethostbyname(hostname);
    if (result == NULL) {
        printf("%s 0 0 00.00.00.00\n", hostname);
    } else {
        printf("%s ", hostname);
	printf("%d ", result->h_addrtype);
	printf("%d", result->h_length);
	i = 0;
        while ((addr = result->h_addr_list[i++]) != NULL) {
	    for (j = 0;j < result->h_length;j++)
	        if (j == 0) printf(" %02d", (int) addr[j]);
		else printf(".%02d", (int) addr[j]);
	}
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    char buffer[500];
    int i;

    if (argc > 1)
        for (i = 1;i < argc; i++) handle_request(argv[i]);
    else {
        while (1) {
	    if (!fgets(buffer, sizeof(buffer), stdin)) break;
	    for (i = 0;i < sizeof(buffer);i++) {
	        if ((buffer[i] <= ' ') || (buffer[i] == '\t'))
		    buffer[i] = '\0';
		if (buffer[i] == '\0') break;
	    }
	    handle_request(buffer);
	}
    }
    return(0);
}

