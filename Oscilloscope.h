#ifndef Oscilloscope_h_
#define Oscilloscope_h_
#include "AudioStream.h"
#include "ST7735_t3.h"

uint8_t bufferBlock = 0;
uint8_t bufcount = 0;
int16_t prev_pixel_y = 0;
const int lineColor = 0x07B0;
boolean EnvIdelFlag = false;

class Oscilloscope : public AudioStream {
	public:
	Oscilloscope(void) : AudioStream(1, inputQueueArray) {
	}
	virtual void update(void);
	void ScreenSetup(ST7735_t3*);
	void Display(void);
	void AddtoBuffer(int16_t*);

	private:
	audio_block_t *inputQueueArray[1];
	ST7735_t3 *display;
	int16_t buffer[AUDIO_BLOCK_SAMPLES];
};
#endif

void Oscilloscope::ScreenSetup(ST7735_t3 *screen) {
	display = screen;
}

void Oscilloscope::Display() {
	uint8_t pixel_x = 0;
	prev_pixel_y = map(buffer[0], 32767, -32768, -65, 65) + 95;	// 1.Pixel to start drawing scope line
	if (prev_pixel_y < 65) prev_pixel_y = 65;
	if (prev_pixel_y > 126)prev_pixel_y = 126;
	for (uint16_t i = 0; i < AUDIO_BLOCK_SAMPLES - 1; i++) {
		int16_t pixel_y = map(buffer[i], 32767, -32768, -65, 65) + 95;
		if (pixel_y < 65) pixel_y = 65;
		if (pixel_y > 126)pixel_y = 126;
		display->drawLine(pixel_x + 17, prev_pixel_y, pixel_x + 18, pixel_y, lineColor);
		prev_pixel_y = pixel_y;
		pixel_x++;
	}
}

// Scope scale 11.6ms For a better view at low frequencies
void Oscilloscope::AddtoBuffer(int16_t *audio) {
	int16_t prev_audio = 0;
	prev_audio = *audio;
	audio++;
	if (bufferBlock == 0) {
		if (prev_audio > 0 && *audio < 512) {		// change Scope Trigger for more screen refresh
			bufferBlock = 1;
			bufcount = 0;							// ignore the first buffer block for trigger
		}
	}
	else {
		for (uint16_t i = 0; i < 32; i++) {
			buffer[bufcount++] = *audio;
			audio += 4;
		}
		bufferBlock++;
		if (bufferBlock >= 5) {
			bufferBlock = 0;
		}
	}
}

void Oscilloscope::update(void) {
	if (!display) return;
	audio_block_t *block;
	block = receiveReadOnly(0);
	if (block) {
		AddtoBuffer(block->data);
		release(block);
		if (bufferBlock == 0) {
			if (EnvIdelFlag == true) {
				Display();
			}
		}
		
	}
}

