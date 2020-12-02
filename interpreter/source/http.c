#include "http.h"

#include <stdio.h>

void http_get(const char *url, http_string_t *out) {
	httpcContext context;
	Result ret = 0;
	const size_t block_size = 4096;
	size_t current_size = 0;
	u32 read_size = 0;
	u8 *buffer = NULL;
	u32 status_code;

	httpcInit(0);
		httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);
		httpcSetSSLOpt(&context, SSLCOPT_DisableVerify); //enable HTTPS by disabling
		httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);
		httpcAddRequestHeaderField(&context, "User-Agent", "SmileLUA");
		httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

		if(httpcBeginRequest(&context) == 0) {
			httpcGetResponseStatusCode(&context, &status_code);

			if(status_code == 200) {
				buffer = malloc(block_size);
				while(1) {
					ret = httpcDownloadData(&context, buffer+current_size, block_size, &read_size);
					current_size += read_size;

					if(ret == (Result)HTTPC_RESULTCODE_DOWNLOADPENDING) {
						buffer = realloc(buffer, current_size+block_size);
					} else {
						break;
					}
				}
			}
		}
	httpcCloseContext(&context);
	httpcExit();

	out->ptr = buffer;
	out->length = current_size;
	//free(buffer); TO BE DONE IN THE CALLING FUNCTION
}
