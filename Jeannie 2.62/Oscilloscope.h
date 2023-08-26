//*************************************************************************
// ElectroTechnique TSynth scope
// Change scale for TFT ER-TFTM018-3 Display 128x160 Pixel
// and implement Idle flag when no audio signal 
//
// https://github.com/ElectroTechnique/TSynth-Teensy4.1
//
//*************************************************************************
#ifndef Oscilloscope_h_
#define Oscilloscope_h_
#include "AudioStream.h"
#include "ST7735_t3.h"
boolean EnvIdelFlag = false;
boolean scopeIdel = false;

uint8_t bufferBlock = 0;
uint8_t bufcount = 0;
uint8_t pixel_x = 0;
int16_t pixel_y = 0;
int16_t prev_pixel_y = 0;
boolean MainShift = false;

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
	if (MainShift == false) {
		pixel_x = 0;
		prev_pixel_y = map(buffer[0], 32767, -32768, -110, 110) + 93;
		if (prev_pixel_y < 65) prev_pixel_y = 65;
		if (prev_pixel_y > 121)prev_pixel_y = 121;

		for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES - 1; i++) {
			pixel_y = map(buffer[i], 32767, -32768, -110, 110) + 93;
			if (pixel_y < 65) pixel_y = 65;
			if (pixel_y > 121)pixel_y = 121;
			display->drawLine(pixel_x + 15, prev_pixel_y, pixel_x + 16, pixel_y, 0x07B0);
			prev_pixel_y = pixel_y;
			pixel_x++;
		}
	}
	else {
		pixel_x = 0;
		prev_pixel_y = map(buffer[0], 32767, -32768, -60, 60) + 83;
		if (prev_pixel_y < 65) prev_pixel_y = 65;
		if (prev_pixel_y > 101)prev_pixel_y = 101;

		for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES - 1; i++) {
			pixel_y = map(buffer[i], 32767, -32768, -60, 60) + 83;
			if (pixel_y < 65) pixel_y = 65;
			if (pixel_y > 100)pixel_y = 101;
			display->drawLine(pixel_x + 15, prev_pixel_y, pixel_x + 16, pixel_y, 0x07B0);
			prev_pixel_y = pixel_y;
			pixel_x++;
		}
	}
}

void Oscilloscope::AddtoBuffer(int16_t *audio) {
	audio++;
	if (bufferBlock == 0) {
		if (*(audio - 1) > -16 && *(audio + 3) < 16) {
			bufferBlock = 1;
			bufcount = 0;
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
	if (!display || scopeIdel == true) return;
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
