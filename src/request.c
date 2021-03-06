/* request.c: Request structure */

#include "mq/request.h"

#include <stdlib.h>
#include <string.h>

/**
 * Create Request structure.
 * @param   method      Request method string.
 * @param   uri         Request uri string.
 * @param   body        Request body string.
 * @return  Newly allocated Request structure.
 */
Request * request_create(const char *method, const char *uri, const char *body) {
    Request *r = calloc(1, sizeof(Request));
    if (r) {
		if(method) r->method = strdup(method);
	    if(uri)    r->uri    = strdup(uri);
	    if(body)   r->body   = strdup(body);
	}
    return r;
}

/**
 * Delete Request structure.
 * @param   r           Request structure.
 */
void request_delete(Request *r) {
	free(r->method);
	free(r->uri);
	free(r->body);
    free(r);

}

/**
 * Write HTTP Request to stream:
 *  
 *  $METHOD $URI HTTP/1.0\r\n
 *  Content-Length: Length($BODY)\r\n
 *  \r\n
 *  $BODY
 *      
 * @param   r           Request structure.
 * @param   fs          Socket file stream.
 */
void request_write(Request *r, FILE *fs) {
	if(!r->body) {
		fprintf(fs, "%s %s HTTP/1.0\r\nContent-Length: 0\r\n\r\n\r\n", 
			r->method, r->uri); 
	} else {
		fprintf(fs, "%s %s HTTP/1.0\r\nContent-Length: %zu\r\n\r\n%s", 
			r->method, r->uri, strlen(r->body), r->body); 
	}
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */ 
