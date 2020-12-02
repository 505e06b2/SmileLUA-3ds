#include "qr.h"

extern PrintConsole console_top;
extern PrintConsole console_bottom;

#define WIDTH 400
#define HEIGHT 240
#define BUFFER_SIZE (WIDTH * HEIGHT * sizeof(u16))

static int running = 0;
static u8 *buffer = NULL;

//transposes the camera RGB565 frame for the 3ds screen
static void writeToFramebuffer(u16 *framebuffer, const u16 *image, u16 width, u16 height) {
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			u32 v = (height - y + x * height);
			framebuffer[v] = image[y * width + x];
		}
	}
}

//turns the initial camera frame greyscale
static void writeToQRBuffer(u8 *qr_buffer, const u16 *rgb565, u16 width, u16 height) {
	for(u16 y = 0; y < height; y++) {
		for(u16 x = 0; x < width; x++) {
			const size_t index = y * width + x;
			const u16 px = rgb565[y * WIDTH + x];
			const u8 r = ((px >> 11) & 0b00011111) << 3;
			const u8 g = ((px >>  5) & 0b00111111) << 2;
			const u8 b = ((px >>  0) & 0b00011111) << 3;
			qr_buffer[index] = (r + g + b) / 3;
		}
	}
}

static void qr_loop(void *arg) {
	u8 *qr_buffer = NULL;
	struct quirc *qr = NULL;
	struct quirc_code code;
	struct quirc_data data;
	quirc_decode_error_t err;
	int qr_width;
	int qr_height;

	qr = quirc_new();
	quirc_resize(qr, WIDTH, HEIGHT);

	while(aptMainLoop() && running) {
		qr_buffer = quirc_begin(qr, &qr_width, &qr_height);
		writeToQRBuffer(qr_buffer, (u16 *)buffer, qr_width, qr_height);
		quirc_end(qr);

		if(quirc_count(qr)) {
			quirc_extract(qr, 0, &code);
			err = quirc_decode(&code, &data);
			if(err == QUIRC_ERROR_DATA_ECC) {
				quirc_flip(&code);
				err = quirc_decode(&code, &data);
			}

			if(err) {
				//printf("DECODE FAILED: %s\n", quirc_strerror(err));
			} else {
				//printf("Data: %s\n", data.payload);

				//copy the data struct into arg
				struct quirc_data **output = (struct quirc_data **)arg;
				*output = malloc(sizeof(struct quirc_data));
				memcpy(*output, &data, sizeof(struct quirc_data));
				break;
			}
		}

		//keep CPU usage down
		svcSleepThread(500000000); //0.5s
		gspWaitForVBlank();
	}

	quirc_destroy(qr);
	running = 0;
	threadExit(0);
}

static void camera_loop() {
	camInit();

	CAMU_SetSize(SELECT_OUT1, SIZE_CTR_TOP_LCD, CONTEXT_A);
	CAMU_SetOutputFormat(SELECT_OUT1, OUTPUT_RGB_565, CONTEXT_A);
	CAMU_SetFrameRate(SELECT_OUT1, FRAME_RATE_15_TO_2);

	CAMU_SetNoiseFilter(SELECT_OUT1, 1);
	CAMU_SetAutoExposure(SELECT_OUT1, 1);
	CAMU_SetAutoWhiteBalance(SELECT_OUT1, 1);
	CAMU_SetContrast(SELECT_OUT1, CONTRAST_HIGH);

	CAMU_SetTrimming(PORT_CAM1, 0);

	u32 transfer_bytes;
	CAMU_GetMaxBytes(&transfer_bytes, WIDTH, HEIGHT);
	CAMU_SetTransferBytes(PORT_CAM1, transfer_bytes, WIDTH, HEIGHT);

	CAMU_Activate(SELECT_OUT1);

	CAMU_ClearBuffer(PORT_BOTH);
	CAMU_StartCapture(PORT_BOTH);

	Handle receive_event = 0;

	u32 keys_down;
	while(aptMainLoop() && running) {
		hidScanInput();
		keys_down = hidKeysHeld();

		if(keys_down & KEY_START) break;

		CAMU_SetReceiving(&receive_event, buffer, PORT_CAM1, BUFFER_SIZE, (s16)transfer_bytes);
		if(svcWaitSynchronization(receive_event, 1500000000)) { //1.5s timeout
			//camera is frozen, so reactivate - not sure if this is bad practice, but it seems to work for the most part on my new3ds
			CAMU_Activate(SELECT_OUT1);
			CAMU_ClearBuffer(PORT_BOTH);
			CAMU_StartCapture(PORT_BOTH);
			continue;
		}

		writeToFramebuffer((u16 *)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL), (u16 *)buffer, WIDTH, HEIGHT);

		svcCloseHandle(receive_event);

		gfxSwapBuffersGpu();
		gspWaitForVBlank();
	}

	CAMU_StopCapture(PORT_BOTH);
	CAMU_Activate(SELECT_NONE);

	running = 0;
	camExit();
}

struct quirc_data *qr_read() {
	consoleSelect(&console_bottom);
	printf("\x1b[2JHold start to cancel\n");

	running = 1;
	buffer = malloc(BUFFER_SIZE);
	struct quirc_data *output = NULL;

	Thread qr_thread = threadCreate(qr_loop, &output, 0x10000, 0x31, 0, 1);
	if(!qr_thread) {
		printf("Failed to launch QR thread\n");
		return NULL;
	}
	camera_loop();

	running = 0;
	threadJoin(qr_thread, 10000000000); //10s
	free(buffer);

	consoleSelect(&console_top);
	return output;
}
