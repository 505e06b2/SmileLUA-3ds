#ifndef _HTTP_H
#define _HTTP_H

#include <3ds.h>
#include <stdlib.h>

typedef struct http_string_s {
	u8 *ptr;
	size_t length;
} http_string_t;

void http_get(const char *, http_string_t *);

#endif
