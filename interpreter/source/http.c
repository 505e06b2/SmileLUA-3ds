#include "http.h"

#include <stdio.h>

char *http_get(const char *url) {
	httpcContext context;
	Result ret = 0;
	const size_t block_size = 4096;
	size_t current_size = 0;
	u32 read_size = 0;
	char *buffer = NULL;
	u32 status_code;

	httpcInit(0);
		httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);
		httpcSetSSLOpt(&context, SSLCOPT_DisableVerify); //enable HTTPS by disabling
		httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);
		httpcAddRequestHeaderField(&context, "User-Agent", "SmileLUA");
		httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

		if(httpcBeginRequest(&context)) {
			httpcCloseContext(&context);
			httpcExit();
			return buffer;
		}

		httpcGetResponseStatusCode(&context, &status_code);

		if(status_code != 200) {
			httpcCloseContext(&context);
			httpcExit();
			return buffer;
		}

		buffer = malloc(block_size);
		while(1) {
			ret = httpcDownloadData(&context, (u8 *)buffer+current_size, block_size, &read_size);
			current_size += read_size;

			if(ret == (Result)HTTPC_RESULTCODE_DOWNLOADPENDING) {
				buffer = realloc(buffer, current_size+block_size);
			} else {
				break;
			}
		}

	httpcCloseContext(&context);
	httpcExit();
	//free(buffer); TO BE DONE IN THE CALLING FUNCTION
	return buffer;
}
