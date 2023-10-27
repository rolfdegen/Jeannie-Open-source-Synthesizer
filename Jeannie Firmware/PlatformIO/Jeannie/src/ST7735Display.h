// ST7735Display.h

// Teensy 4.1 Port connection
#define sclk 27
#define mosi 26
#define cs 2
#define dc 3
#define rst 9
#define DISPLAYTIMEOUT 700
#define BUFFPIXEL 20

#include "ST7735_t3.h"
#include "Fonts/Org_01.h"
#include "Yeysk16pt7b.h"
#include "Fonts/FreeSansBold18pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSansBold9pt7b.h"
#include "Fonts/FreeSansOblique24pt7b.h"
#include "Fonts/FreeSansBoldOblique24pt7b.h"
#include "Fonts/Picopixel.h"
#include "global.h"

#define PULSE 1
#define VAR_TRI 2
#define FILTER_ENV 3
#define AMP_ENV 4

ST7735_t3 tft = ST7735_t3(cs, dc, mosi, sclk, rst);

String currentParameter = "";
String currentValue = "";
float currentFloatValue = 0.0;
String currentPgmNum = "";
//String currentPatchName = "            ";
String newPatchName = "            ";
String oldPatchName = "            ";
String newPatternName = "            ";
//int newPatchNo = 1;
int newPatternNo = 1;
//int newPatchBank = 0;
uint8_t CharPosPatchName = 0;
//char * currentSettingsOption = "";
//char * currentSettingsValue = "";
//uint32_t currentSettingsPart = SETTINGS;
//uint32_t paramType = PARAMETER;
uint32_t currentSettingsPart = 0;
uint32_t paramType = 8;

boolean MIDIClkSignal = false;
int MidiCLKcount = 0;
boolean MidiCLKstatus = false;
float MidiCLKzeitAlt = 0;
uint32_t peakCount = 0;
uint16_t prevLen = 0;

uint32_t colour[NO_OF_VOICES] = {ST7735_BLUE, ST7735_BLUE, ST7735_BLUE, ST7735_BLUE, ST7735_BLUE, ST7735_BLUE, ST7735_BLUE, ST7735_BLUE};
uint8_t currentPatchNameStatus = 0;
uint8_t currentPatchNameHoldTime = 0;
uint8_t bpmCount = 0;
float Clock_Time = 0;


//*************************************************************************
// Prototyp
//*************************************************************************
void print_String(uint16_t index, uint8_t xpos, uint8_t ypos);
void renderCurrentPatchPage();
void printFilterFrq(uint16_t value);
void printFilterRes(uint8_t value);
void printFilterKey(uint8_t value);
void printFilterTyp(uint8_t value);
void printFilterEnvAmt(uint8_t EnvVal);
void draw_filter_curves (uint8_t FilterFrq, uint8_t FilterRes, uint8_t FilterMix);
void printEnvATKvalues (uint8_t value);
void printEnvDCYvalues (uint8_t value);
void printEnvSUSvalues (uint8_t value);
void printEnvRELvalues (uint8_t value);
void printEnvDELvalues (uint8_t value);
void printEnvHOLvalues (uint8_t value);
void drawEnvCurve(uint8_t ATKvalue, uint8_t DCYvalue, uint8_t SUSvalue, uint8_t RELvalue);
void printEnvValue (uint8_t value, uint8_t xpos, uint8_t ypos);
void MidiSymbol ();
void printDataValue (uint8_t index, uint16_t value);
void printRedMarker (uint8_t index, uint8_t value);
void printFxValFrame(uint8_t value);
void printFxPotValue(uint8_t index, uint8_t value);
void drawFxGrafic (uint8_t volume, uint8_t time, uint8_t feedback, uint8_t Pot3value);
void printFxName(uint8_t PrgNo);
void printCurrentPatchName (void);
void drawLFOshape(uint16_t waveColor);
void drawLFOframe(uint8_t frameNo);
void drawMainPage (void);
void drawOsc1Page (void);
void drawOsc2Page (void);
void drawOscSubMenu (void);
void drawFilterPage (void);
void drawFilterEnvPage (void);
void drawAmpEnvPage (void);
void drawFxDspPage (void);
void drawLFO1Page (void);
void drawLFO2Page (void);
void drawSystemPage (void);
void drawSavePage (void);
void printOscMixVal(uint8_t value);
void printOscPitchEnv(uint8_t Value);
void printOscMix (void);
void bmpDraw(char *filename, uint8_t x, uint16_t y);
void printPercentValue (uint8_t index, uint8_t value);
void drawVoiceLED (void);
void printFxPOT(uint8_t PotNo, uint8_t PrgNo);
void drawWaveshaperCurve(uint8_t WShaperNo);
void drawSeqStep (uint8_t step_x, uint8_t step_y, uint16_t color);
void drawSEQStepFrame(uint8_t SEQselectStepNo);
void printSEQclkDiv (uint8_t SEQdivider);
void drawSEQstepNumbersMarker(uint8_t SEQstepNumbers);
void printSeqPitchNote (void);
void drawSaveSeqPattern (void);
void printOscLevel (void);
void printPWMrate (float pwmRate);
void draw_PWM_curve(float value);
void drawEnvMarkerVCF(int posx);
void drawLFOMarkerVCF(int posx);
void drawSEQpitchValue2(uint8_t SEQselectStepNo);
void renderModParameter(uint8_t ParameterNr, uint8_t value);
uint8_t readModParameter(uint8_t ParameterNr);
void readModMatrixParameter (uint8_t ParameterNo);
void print_UserPOT(uint8_t potnumber);
void printVoiceMode(void);
void myControlChange(byte channel, byte control, byte value);
void updateFilterFM(void);
void updateFilterFM2(void);
void updateLFO3amt(void);
void updateAtouchCutoff(void);
void updateFilterLfoAmt(void);
void recallPatch(int patchNo);
void mute_before_load_patch(void);
int getTranspose(int value);
void updateOscFX(void);
void setWaveShaperTable (uint8_t WShaperNo);
void setWaveShaperDrive (float value);
void updateFilterEnvelopeType(void);
void updateAmpEnvelopeType(void);
void updateLFO1fade(void);
void updateOscLFOAmt(void);
void updateLFO1release(void);
void updateLFO1EnvelopeType(void);
void updateLFO2fade(void);
void updateLFO2release(void);
void updateLFO2EnvelopeType(void);
void updateLFO3fade(void);
void updateLFO3release(void);
void updateLFO3EnvelopeType(void);
void setFxPrg (uint8_t PrgNo);
void recallPatch2(int patchNo, int patchBank);
void recallPattern(int patternNo);
void clr_pickup(void);
void printOscPitchEnv(uint8_t Value);
int convert_pitchEnv(float pitch_Env);
void printTemperature(void);
void printCPUmon(void);
void updateModWheelCutoff(void);
void updateHPFFilterFreq(void);
void update_LFO2_Osc1_PRMA_mod (void);
void update_LFO2_Osc1_PRMB_mod (void);
void update_LFO2_Osc2_PRMA_mod (void);
void update_LFO2_Osc2_PRMB_mod (void);
void update_LFO3_Osc1_PRMA_mod (void);
void update_LFO3_Osc1_PRMB_mod (void);
void update_LFO3_Osc2_PRMA_mod (void);
void update_LFO3_Osc2_PRMB_mod (void);
void update_filterEnv_Osc1_PRMA_mod (void);
void update_filterEnv_Osc1_PRMB_mod (void);
void update_filterEnv_Osc2_PRMA_mod (void);
void update_filterEnv_Osc2_PRMB_mod (void);

//*************************************************************************
// print const Text string from flash memory to xpos ypos on screen
//*************************************************************************
FLASHMEM void print_String(uint16_t index, uint8_t xpos, uint8_t ypos) {
	
	char buffer[20];
	strcpy_P(buffer, (char*)pgm_read_dword(&(String_Tab[index])));
	tft.setCursor(xpos, ypos);
	tft.println(buffer);
}

//*************************************************************************
// draw Filter EnvMarker
//*************************************************************************
FLASHMEM void drawEnvMarkerVCF(int posx)
{
	tft.fillRoundRect(posx,30,16,9,2,ST7735_GRAY);
	tft.setFont(&Picopixel);
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(posx+2,36);
	tft.print("ENV");
	tft.setFont(NULL);
}

//*************************************************************************
// draw Filter EnvMarker
//*************************************************************************
FLASHMEM void drawLFOMarkerVCF(int posx)
{
	tft.fillRoundRect(posx,60,16,9,2,ST7735_DARKGREEN);
	tft.setFont(&Picopixel);
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(posx+3,66);
	tft.print("LFO");
	tft.setFont(NULL);
}


//*************************************************************************
// draw PWM Arrows
//*************************************************************************
FLASHMEM void drawPWMarrows (uint8_t value)
{
	// left arrow
	tft.drawFastVLine(89,33,5,ST7735_WHITE);
	tft.drawFastVLine(88,34,3,ST7735_WHITE);
	tft.drawPixel(87,35,ST7735_WHITE);
	
	// right arrow
	tft.drawFastVLine(90+value,33,5,ST7735_WHITE);
	tft.drawFastVLine(91+value,34,3,ST7735_WHITE);
	tft.drawPixel(92+value,35,ST7735_WHITE);
	
	//tft.drawFastHLine(90,34,value,ST7735_WHITE);
	// Pixel line
	for (uint8_t i = 0; i < value; i++) {
		tft.drawPixel(90+i,35,ST7735_WHITE);
		i++;
	}
}

//*************************************************************************
// draw Osc PWM curve
//*************************************************************************
FLASHMEM void draw_PWM_curve(float value)
{
	uint16_t color = ST7735_RED;
	
	tft.fillRect(82,17,75,33,ST7735_BLACK);		// clear Wave Screen
	if (PageNr == 2) {
		color = ST7735_ORANGE;
	}
	tft.drawFastVLine(87,23,24,color);
	uint8_t val = (float)((value + 1.0f) * 26);
	tft.drawFastHLine(87,22,5+val,color);
	tft.drawFastVLine(87+5+val,22,24,color);
	tft.drawFastHLine(87+5+val,22+24,58-val,color);
	drawPWMarrows (val);
	
	
	
	tft.setFont(&Picopixel);
	if (oscWaveformA == 5 || oscWaveformB == 5) {
		tft.setCursor(136, 22);
		tft.print("PWM");
		tft.setFont(NULL);
	}
	else if (oscWaveformA == 12 || oscWaveformB == 12) {
		tft.setCursor(127, 22);
		tft.print("PWM BL");
		tft.setFont(NULL);
	}
}

//*************************************************************************
// print Osc PWMRATE
//*************************************************************************
FLASHMEM void printPWMrate (float pwmRate)
{	
	uint8_t value = 0;
	
	for (uint8_t i = 0; i < 128; i++) {
		float myValue = PWMRATE[i];
		if (myValue >= pwmRate) {
			value = i;
			break;
		}
	}
	
	tft.setCursor(135,78);
	tft.setTextColor(ST7735_WHITE);
	if (value <= 1) {
		tft.print("PW");
	}
	else if (value <= 6) {
		tft.print("ENV");
		if (PageNr == 1 && Osc1WaveBank == 0 && (oscWaveformA == 5 || oscWaveformA == 8 || oscWaveformA == 12)) {
			tft.setFont(&Picopixel);
			tft.fillRoundRect(135,25,17,9,2,ST7735_GRAY);
			tft.setCursor(137,31);
			tft.print("ENV");
			tft.setFont(NULL);
		}
		else if (PageNr == 2 && Osc2WaveBank == 0 && (oscWaveformB == 5 || oscWaveformB == 8 || oscWaveformB == 12)) {
			tft.setFont(&Picopixel);
			tft.fillRoundRect(135,25,17,9,2,ST7735_GRAY);
			tft.setCursor(137,31);
			tft.print("ENV");
			tft.setFont(NULL);
		}
	}
	else {
		value = value - 6;
		tft.print(value);
		if (PageNr == 1 && Osc1WaveBank == 0 && (oscWaveformA == 5 || oscWaveformA == 8 || oscWaveformA == 12)) {
			tft.setFont(&Picopixel);
			tft.fillRoundRect(135,25,17,9,2,ST7735_DARKGREEN);
			tft.setCursor(138,31);
			tft.print("LFO");
			tft.setFont(NULL);
		}
		else if (PageNr == 2 && Osc2WaveBank == 0 && (oscWaveformB == 5 || oscWaveformB == 8 || oscWaveformB == 12)) {
			tft.setFont(&Picopixel);
			tft.fillRoundRect(135,25,17,9,2,ST7735_DARKGREEN);
			tft.setCursor(138,31);
			tft.print("LFO");
			tft.setFont(NULL);
		}
	}
}

//*************************************************************************
// print Osc Level
//*************************************************************************
FLASHMEM void printOscLevel (void)
{
	uint8_t value = 0;
	
	for (uint8_t i = 0; i < 128; i++) {
		float myValue = LINEAR[i];
		if ((driveLevel /1.25f) <= myValue) {
			value = i;
			break;
		}
	}
	tft.setCursor(5,97);
	if (value == 0) {
		tft.setTextColor(ST7735_RED);
		tft.println("LEVEL");
		} else {
		tft.setTextColor(ST7735_GRAY);
		tft.println("LEVEL");
	}
	tft.setCursor(56,97);
	tft.setTextColor(ST7735_WHITE);
	tft.print(value);
}

//*************************************************************************
// start Timer
//*************************************************************************
FLASHMEM void startTimer()
{
	timer1 = millis();
	timer2 = millis();
}

//*************************************************************************
// enable Scope
//*************************************************************************
FLASHMEM void enableScope(boolean enable) {
	enable ? scope.ScreenSetup(&tft) : scope.ScreenSetup(NULL);
}

//*************************************************************************
// draw Bitmap
//*************************************************************************
FLASHMEM uint16_t read16(File f) {
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

FLASHMEM uint32_t read32(File f) {
	uint32_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

//*************************************************************************
// Load bmp Image from SD card
//*************************************************************************
FLASHMEM void bmpDraw(const char *filename, uint8_t x, uint16_t y) {
	File     bmpFile;
	int      bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
	uint16_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	boolean  goodBmp = false;       // Set to true on valid header parse
	boolean  flip    = true;        // BMP is stored bottom-to-top
	int      w, h, row, col;
	uint8_t  r, g, b;
	uint32_t pos = 0;

	uint16_t awColors[320];  // hold colors for one row at a time...

	if ((x >= tft.width()) || (y >= tft.height())) return;

	// Open requested file on SD card
	bmpFile = SD.open(filename);
	if (!bmpFile) {
		Serial.print("File not found");
		tft.fillScreen(ST7735_BLACK);
		tft.setCursor(20, tft.height() / 2 - 12);
		tft.setTextColor(ST7735_RED);
		tft.print("Pic files not found!");
		tft.setCursor(20, tft.height() / 2 + 12);
		delay(100);
		return;
	}

	// Parse BMP header
	if (read16(bmpFile) == 0x4D42) { // BMP signature
		read32(bmpFile);
		(void)read32(bmpFile); // Read & ignore creator bytes
		bmpImageoffset = read32(bmpFile); // Start of image data
		read32(bmpFile);
		bmpWidth  = read32(bmpFile);
		bmpHeight = read32(bmpFile);
		if (read16(bmpFile) == 1) { // # planes -- must be '1'
			bmpDepth = read16(bmpFile); // bits per pixel
			if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
				goodBmp = true; // Supported BMP format -- proceed!
				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if (bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip      = false;
				}

				// Crop area to be loaded
				w = bmpWidth;
				h = bmpHeight;
				if ((x + w - 1) >= tft.width())  w = tft.width()  - x;
				if ((y + h - 1) >= tft.height()) h = tft.height() - y;

				for (row = 0; row < h; row++) { // For each scanline...

					// Seek to start of scan line.  It might seem labor-
					// intensive to be doing this on every line, but this
					// method covers a lot of gritty details like cropping
					// and scanline padding.  Also, the seek only takes
					// place if the file position actually needs to change
					// (avoids a lot of cluster math in SD library).
					if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
					pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
					else     // Bitmap is stored top-to-bottom
					pos = bmpImageoffset + row * rowSize;
					if (bmpFile.position() != pos) { // Need seek?
						bmpFile.seek(pos);
						buffidx = sizeof(sdbuffer); // Force buffer reload
					}

					for (col = 0; col < w; col++) { // For each pixel...
						// Time to read more pixel data?
						if (buffidx >= sizeof(sdbuffer)) { // Indeed
							bmpFile.read(sdbuffer, sizeof(sdbuffer));
							buffidx = 0; // Set index to beginning
						}

						// Convert pixel from BMP to TFT format, push to display
						#ifdef IMAGE_COLORS_BGR
						b = sdbuffer[buffidx++];
						g = sdbuffer[buffidx++];
						r = sdbuffer[buffidx++];
						#else
						r = sdbuffer[buffidx++];
						g = sdbuffer[buffidx++];
						b = sdbuffer[buffidx++];
						#endif
						awColors[col] = tft.Color565(r, g, b);
					} // end pixel
					tft.writeRect(0, row, w, 1, awColors);
				} // end scanline
			} // end goodBmp
		}
	}

	bmpFile.close();
	if (!goodBmp) Serial.println(F("BMP format not recognized"));

}

//*************************************************************************
// print SEQdirection
//*************************************************************************
FLASHMEM void printSEQdirection (void)
{
	tft.fillRect(88,116,20,8,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	if (SEQdirection == 0) {
		tft.setCursor(94,116);
		tft.print(">");
	}
	else if (SEQdirection == 1) {
		tft.setCursor(94,116);
		tft.print("<");
	}
	else if (SEQdirection == 2) {
		tft.setCursor(90,116);
		tft.print("<>");
	}
	else if (SEQdirection == 3) {
		tft.setCursor(88,116);
		tft.print("RND");
	}
}

//*************************************************************************
// draw Waveshaper curve
//*************************************************************************
FLASHMEM void drawWaveshaperCurve(uint8_t WShaperNo)
{
	// clear old curve ----------------------------
	tft.fillRect(82,16,75,35,ST7735_BLACK);
	
	// draw grid ----------------------------------
	tft.drawFastHLine(101, 16, 37, ST77XX_DARKGREY);
	tft.drawFastHLine(101, 16+9, 37, ST77XX_DARKGREY);
	tft.drawFastHLine(101, 33, 37, ST77XX_DARKGREY);
	tft.drawFastHLine(101, 33+9, 37, ST77XX_DARKGREY);
	tft.drawFastHLine(101, 50, 37, ST77XX_DARKGREY);
	tft.drawFastVLine(82+19, 16, 35, ST77XX_DARKGREY);
	tft.drawFastVLine(82+19+9, 16, 35, ST77XX_DARKGREY);
	tft.drawFastVLine(82+38, 16, 35, ST77XX_DARKGREY);
	tft.drawFastVLine(82+38+9, 16, 35, ST77XX_DARKGREY);
	tft.drawFastVLine(82+38+9+9, 16, 35, ST77XX_DARKGREY);
	
	// draw curves ----------------------------------
	
	// Waveshaper off
	if (WShaperNo == 0) {
		tft.setFont(&Picopixel);
		tft.setTextColor(ST7735_WHITE);
		print_String(18, 115, 35);	// print "OFF"
		tft.setFont(NULL);
		return;
	}
	
	uint8_t xpos = 102;
	uint8_t old_xpos = 102;
	uint8_t ypos = 49;
	uint8_t old_ypos = 49;
	
	const float *WS_array_addr = 0;  //!!!!!!!!!!!FB
	
	switch(WShaperNo) {
		case 1: WS_array_addr = WAVESHAPER_TABLE2; break;
		case 2: WS_array_addr = WAVESHAPER_TABLE3; break;
		case 3: WS_array_addr = WAVESHAPER_TABLE4; break;
		case 4: WS_array_addr = WAVESHAPER_TABLE5; break;
		case 5: WS_array_addr = WAVESHAPER_TABLE6; break;
		case 6: WS_array_addr = WAVESHAPER_TABLE7; break;
		case 7: WS_array_addr = WAVESHAPER_TABLE8; break;
		case 8: WS_array_addr = WAVESHAPER_TABLE9; break;
		case 9: WS_array_addr = WAVESHAPER_TABLE10; break;
		case 10: WS_array_addr = WAVESHAPER_TABLE11; break;
		case 11: WS_array_addr = WAVESHAPER_TABLE12; break;
		case 12: WS_array_addr = WAVESHAPER_TABLE13; break;
		case 13: WS_array_addr = WAVESHAPER_TABLE14; break;
		case 14: WS_array_addr = WAVESHAPER_TABLE15; break;
		case 15: WS_array_addr = WAVESHAPER_TABLE16; break;
		case 16: WS_array_addr = WAVESHAPER_TABLE17; break;
		case 17: WS_array_addr = WAVESHAPER_TABLE18; break;
		case 18: WS_array_addr = WAVESHAPER_TABLE19; break;
		case 19: WS_array_addr = WAVESHAPER_TABLE20; break;
		case 20: WS_array_addr = WAVESHAPER_TABLE21; break;
		case 21: WS_array_addr = WAVESHAPER_TABLE22; break;
		case 22: WS_array_addr = WAVESHAPER_TABLE23; break;
		case 23: WS_array_addr = WAVESHAPER_TABLE24; break;
		case 24: WS_array_addr = WAVESHAPER_TABLE25; break;
		case 25: WS_array_addr = WAVESHAPER_TABLE26; break;
		case 26: WS_array_addr = WAVESHAPER_TABLE27; break;
		case 27: WS_array_addr = WAVESHAPER_TABLE28; break;
		case 28: WS_array_addr = WAVESHAPER_TABLE29; break;
		case 29: WS_array_addr = WAVESHAPER_TABLE30; break;
	}
		
	float value = WS_array_addr[0];
	value = value + 1.0f;
	uint8_t val = (uint8_t)(value * 16.0f);
	ypos = 49 - (val);
	old_ypos = ypos;

	for (uint16_t i = 7; i < 257; i++) {
		tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
		xpos++;
		value = WS_array_addr[i];
		value = value + 1.05f;
		val = (uint8_t)(value * 16.0f);
		old_ypos = ypos;
		old_xpos = xpos;
		ypos = 49 - (val);
		i += 6;
	}
	tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
	tft.setFont(&Picopixel);
	tft.setTextColor(ST7735_WHITE);
	print_String(247 + WShaperNo, 84, 22);	// print waveshaper name
	tft.setFont(NULL);

}

//*************************************************************************
// Midi-IN Symbol
//*************************************************************************
FLASHMEM void MidiSymbol ()
{
	uint8_t xpos = 137;
	uint8_t ypos = 2;
	
	// draw Midi Symbol
	if (MidiStatusSymbol == 1) {
		tft.drawFastVLine(xpos, ypos + 6, 2, ST7735_YELLOW);
		tft.drawFastVLine(xpos + 1, ypos + 5, 4, ST7735_YELLOW);
		tft.drawFastVLine(xpos + 2, ypos + 5, 4, ST7735_YELLOW);
		tft.drawFastVLine(xpos + 3, ypos, 8, ST7735_YELLOW);
		tft.drawFastVLine(xpos + 4, ypos + 1, 2, ST7735_YELLOW);
		tft.drawPixel(xpos + 5, ypos + 2, ST7735_YELLOW);
		MidiStatusSymbol = 2;
		MidiStatusHoldTime = 1;
		tftUpdate = false;
	}

	// clear Midi Symbol
	else if (MidiStatusSymbol == 2 && MidiStatusHoldTime == 0) {
		tft.drawFastVLine(xpos, ypos + 6, 2, ST7735_LIGHTGRAY);
		tft.drawFastVLine(xpos + 1, ypos + 5, 4, ST7735_LIGHTGRAY);
		tft.drawFastVLine(xpos + 2, ypos + 5, 4, ST7735_LIGHTGRAY);
		tft.drawFastVLine(xpos + 3, ypos, 8, ST7735_LIGHTGRAY);
		tft.drawFastVLine(xpos + 4, ypos + 1, 2, ST7735_LIGHTGRAY);
		tft.drawPixel(xpos + 5, ypos + 2, ST7735_LIGHTGRAY);
		MidiStatusSymbol = 0;
		tftUpdate = false;
	}
	
	// dec timer
	else if (MidiStatusHoldTime > 0) {
		MidiStatusHoldTime--;
	}
}

//*************************************************************************
// draw voice LED if recall Menu pages
//*************************************************************************
FLASHMEM void draw_Voice_State(void)
{
	// set voice LED
	for (uint8_t i = 0; i < 8; i++) {
		if (voices[i].voiceOn == 1) {
			if (i < 4) {
				tft.fillCircle(110 + (i * 6), 3, 2, ST7735_ORANGE);
				} else {
				tft.fillCircle(110 + ((i-4) * 6), 9, 2, ST7735_ORANGE);
			}
		}
		else {
			if (i < 4) {
				tft.fillCircle(110 + (i * 6), 3, 2, ST7735_GRAY);
				tft.drawCircle(110 + (i * 6), 3, 2, ST7735_LIGHTGRAY);
				} else {
				tft.fillCircle(110 + ((i-4) * 6), 9, 2, ST7735_GRAY);
				tft.drawCircle(110 + ((i-4) * 6), 9, 2, ST7735_LIGHTGRAY);
			}
		}
	}
}


//*************************************************************************
// draw voice LED is aktiv
//*************************************************************************
FLASHMEM void drawVoiceLED (void)
{	
	if (PageNr >= 1) {
		for (uint8_t i = 0; i < 8; i++) {
			if (i < 4) {
				tft.fillCircle(110 + (i * 6), 3, 2, ST7735_GRAY);
				tft.drawCircle(110 + (i * 6), 3, 2, ST7735_LIGHTGRAY);
				} else {
				tft.fillCircle(110 + ((i-4) * 6), 9, 2, ST7735_GRAY);
				tft.drawCircle(110 + ((i-4) * 6), 9, 2, ST7735_LIGHTGRAY);
			}
		}
		
		for (uint8_t i = 0; i < 8; i++) {
			if (voices[i].voiceOn == 1 || VoicesLEDtime[i] != 0) {
				if (i < 4) {
					tft.fillCircle(110 + (i * 6), 3, 2, ST7735_ORANGE);
					} else {
					tft.fillCircle(110 + ((i-4) * 6), 9, 2, ST7735_ORANGE);
				}
				VoicesLEDtime[i]--;	// LED on time afther Voice off
			}
		}
	}
}


//*************************************************************************
// draw voice LED in main page
//*************************************************************************
FLASHMEM void drawVoiceLED_mainpage(void)
{
	tft.fillRect(117, 7, 37, 17, ST7735_BLACK);
	
	for (uint8_t i = 0; i < 8; i++) {
		if (i < 4) {
			tft.drawRect(117 + (i * 10), 7, 8, 8, ST7735_BLUE);
			} else {
			tft.drawRect(117 + ((i-4) * 10), 17, 8, 8, ST7735_BLUE);
		}
	}
	
	for (uint8_t i = 0; i < 8; i++) {
		if (voices[i].voiceOn == 1 || VoicesLEDtime[i] != 0) {
			if (i < 4) {
				tft.fillRect(117 + (i * 10), 7, 8, 8, ST7735_BLUE);
				} else {
				tft.fillRect(117 + ((i-4) * 10), 17, 8, 8, ST7735_BLUE);
			}
			VoicesLEDtime[i]--;	// LED on time afther Voice off
		}
	}
}

//*************************************************************************
// print Patch name if Midi Prgchange
//*************************************************************************
FLASHMEM void printCurrentPatchName (void)
{
	if (currentPatchNameStatus == 1) {
		
		tft.fillRect(25,43,110,45,ST7735_WHITE);
		tft.fillRect(26,44,108,43,ST77XX_DARKGREY);
		
		int16_t x1 = 0;
		int16_t y1 = 0;
		uint16_t w1 = 0;
		uint16_t h1 = 0;
		tft.getTextBounds(currentPgmNum, 0, 0, &x1, &y1, &w1, &h1); // string width in pixels
		tft.setCursor(80 - (w1 / 2), 53);
		tft.setTextColor(ST7735_YELLOW);
		tft.println(currentPgmNum);
		tft.getTextBounds(currentPatchName , 0, 0, &x1, &y1, &w1, &h1); // string width in pixels
		tft.setCursor(80 - (w1 / 2), 68);
		tft.setTextColor(ST7735_WHITE);
		tft.println(currentPatchName);
		currentPatchNameStatus = 2;
		currentPatchNameHoldTime = 25;
	}
	// refresh Menu Page
	else if (currentPatchNameStatus == 2 && currentPatchNameHoldTime == 0) {
		renderCurrentPatchPage();
		currentPatchNameStatus = 0;
		tftUpdate = false;
	}
	// dec timer
	else if (currentPatchNameHoldTime > 0) {
		currentPatchNameHoldTime--;
	}
}

//*************************************************************************
// render Peakmeter
//*************************************************************************
FLASHMEM void renderPeak() {
	
	uint8_t BARnew = 0;
	static uint8_t BARold = 0;
	int BARval = 0;
	static boolean PeakFlag = false;
	static uint8_t PeakTime = 0;
	static uint8_t PeakPeakTime = 0;
		
	// Peak Attack phase ------------------------------------------------
	if (vuMeter && peak.available()) {
		BARval = (int)(peak.read() * 1000.0f);
		
		if (BARval >= 1000)		 {BARnew = 11;}		// -0dB
		else if (BARval >= 890)	 {BARnew = 10;}		// -1dB
		else if (BARval >= 710)  {BARnew = 9;}		// -3dB
		else if (BARval >= 500)  {BARnew = 8;}		// -6dB
		else if (BARval >= 354)  {BARnew = 7;}		// -9dB
		else if (BARval >= 251)  {BARnew = 6;}		// -12dB
		else if (BARval >= 125)  {BARnew = 5;}		// -18dB
		else if (BARval >= 63)   {BARnew = 4;}		// -24dB
		else if (BARval >= 31)   {BARnew = 3;}		// -30dB
		else if (BARval >= 15)   {BARnew = 2;}		// -36dB
		else if	(BARval >= 8)	 {BARnew = 1;}		// -42dB
		
		//clear old Bar line
		tft.fillRect(149,1, 9,10, ST7735_LIGHTGRAY);
		tft.drawFastVLine(153,1,11,ST7735_GRAY);
		
		// draw new Bar line
		tft.fillRect(149,12 - BARnew, 9, BARnew, ST7735_GREEN);
		
		// draw Peak
		if (BARnew >= 9) {
			tft.drawFastHLine(149,1,9,ST7735_RED);
			tft.drawFastHLine(149,2,9,ST7735_RED);
			tft.drawFastHLine(149,3,9,ST7735_ORANGE);
		}
		else if (BARnew == 8) {
			tft.drawFastHLine(149,3,9,ST7735_ORANGE);
		}
		
		// PEAK time
		if (BARnew >= BARold) {
			BARold = BARnew;
			PeakPeakTime = 15;
		}
		if (PeakPeakTime > 0) {
			if (BARold >= 9) {
				tft.drawFastHLine(149,1,9,ST7735_RED);
				tft.drawFastHLine(149,2,9,ST7735_RED);
			}
			else {
				if (BARold >= 1) {
					tft.drawFastHLine(149,11 - BARold, 9, ST7735_YELLOW);
				}
				else {
					tft.drawFastHLine(149,11, 9, ST7735_GREEN);
				}
				
			}
			PeakPeakTime--;
		}
		else {
			BARold = BARnew;
		}
		
		// clear Peak line
		tft.drawFastVLine(153,1,11,ST7735_GRAY);
		PeakFlag = true;
		PeakTime = 1;
		
	}
	
	// Peak Release phase -----------------------------------------------
	else if (PeakPeakTime > 1) {
		//clear Bar line
		tft.fillRect(149,1, 9,10, ST7735_LIGHTGRAY);
		if (BARold >= 9) {
			tft.drawFastHLine(149,1,9,ST7735_RED);
			tft.drawFastHLine(149,2,9,ST7735_RED);
			//tft.drawFastHLine(149,3,9,ST7735_ORANGE);
		}
		else if (BARnew == 8) {
			tft.drawFastHLine(149,3,9,ST7735_ORANGE);
		}
		else {
			if (BARold >= 1) {
				tft.drawFastHLine(149,11 - BARold, 9, ST7735_YELLOW);
			}
			else {
				tft.drawFastHLine(149,11, 9, ST7735_GREEN);
			}
		}
		tft.drawFastVLine(153,1,11,ST7735_GRAY);
		PeakPeakTime--;
	}
	else if ((PeakFlag == true && PeakTime == 0) || PeakPeakTime == 1) { //!!!!!!!!!!!!
		//clear Bar line
		tft.fillRect(149,1, 9,10, ST7735_LIGHTGRAY);
		tft.drawFastVLine(153,1,11,ST7735_GRAY);
		BARold = 0;
		PeakPeakTime = 0;
	}
	
	if (PeakFlag == true) {
		PeakTime--;
	}
}

//*************************************************************************
// draw Envelope Meter 5ms Task
//*************************************************************************
FLASHMEM void drawEnvelopeMeter ()
{
	static int val;
	static uint8_t counter;
	static uint8_t xpos;
	
	
	if (peak.available()) {
		val += (int)(peak.read() * 250.0f);
		
		counter++;
		if (counter >= 10) {
			counter = 0;
			val = val / 10;
			if (val >= 25) {
				val = 25;
			}
			tft.drawFastVLine(110 + xpos, (65 - val), val, ST7735_ORANGE);
			xpos++;
			if (xpos >= 35) {
				xpos = 0;
				counter = 0;
				tft.fillRect(110,40,35,25,ST7735_BLACK);
			}
		}
	}
}

//*************************************************************************
// encoding WaveformNo
//*************************************************************************
FLASHMEM int encodingWaveformANo (int value)
{
	int waveNo = 0;
	
	switch(value){
		case 19:		// Silent
		waveNo = 0;
		break;
		case 3:			// Triangle
		waveNo = 1;
		break;
		case 11:		// bandlimit Square
		waveNo = 2;
		break;
		case 9:			// bandlimit Sawtooth
		waveNo = 3;
		break;
		case 12:
		waveNo = 4;		// bandlimit Puls
		break;
		case 8:
		waveNo = 5;		// variable Triangle
		break;
		case 103:
		waveNo = 6;		// Parabolic
		break;
		case 104:
		waveNo = 7;		// Harmonic
		break;
	}
	
	return waveNo;
}

//*************************************************************************
// encoding WaveformNo
//*************************************************************************
FLASHMEM int encodingWaveformBNo (int value)
{
	int waveNo = 0;
	
	switch(value){
		case 19:		// Silent
		waveNo = 0;
		break;
		case 7:			// S&H
		waveNo = 1;
		break;
		case 11:		// bandlimit Square
		waveNo = 2;
		break;
		case 9:			// bandlimit Sawtooth
		waveNo = 3;
		break;
		case 12:
		waveNo = 4;		// bandlimit Puls
		break;
		case 8:
		waveNo = 5;		// variable Triangle
		break;
		case 103:
		waveNo = 6;		// Parabolic
		break;
		case 104:
		waveNo = 7;		// Harmonic
		break;
	}
	
	return waveNo;
}

//*************************************************************************
// draw Parameter Frame
// ************************************************************************
FLASHMEM void drawParamterFrame(uint8_t PageNr, uint8_t ParameterNr)
{
	if (PageNr <= 2){
		uint8_t Xpos = 1;
		uint8_t width = 77;
		if (ParameterNr > 4){
			Xpos = 81;
			width = 76;
		}
		
		// clear all Frames
		for (uint8_t i = 0; i < 5; i++){
			tft.drawRect(1,15+(19*i),79,18,ST7735_BLACK);
			tft.drawRect(2,16+(19*i),77,16,ST7735_BLACK);
		}
		for (uint8_t i = 2; i < 5; i++){
			tft.drawRect(81,15+(19*i),78,18,ST7735_BLACK);
			tft.drawRect(82,16+(19*i),76,16,ST7735_BLACK);
		}
		
		uint8_t col = ParameterNr;
		if (ParameterNr > 4){
			col = col - 3;
		}
		// draw current Frame
		tft.drawRect(Xpos,15+(19*col),width+2,18,ST7735_YELLOW);
	}
	else if (PageNr == 10) {
		uint8_t Xpos = 1;
		uint8_t width = 77;
		
		if (ParameterNr >= 5){
			Xpos = 81;
			width = 76;
		}
		
		// clear all Frames
		for (uint8_t i = 0; i < 5; i++){
			tft.drawRect(1,15+(19*i),79,18,ST7735_BLACK);
			tft.drawRect(2,16+(19*i),77,16,ST7735_BLACK);
		}
		for (uint8_t i = 0; i < 5; i++){
			tft.drawRect(81,15+(19*i),78,18,ST7735_BLACK);
			tft.drawRect(82,16+(19*i),76,16,ST7735_BLACK);
		}
		
		uint8_t col = ParameterNr;
		if (ParameterNr >= 5){
			col = col - 5;
		}
		
		// draw current Frame
		tft.drawRect(Xpos,15+(19*col),width+2,18,ST7735_YELLOW);
	}
	
	// System main page ---------------------------------------------------
	else if (PageNr == 12 && myPageShiftStatus[PageNr] == false) {
		uint8_t Xpos = 81;
		uint8_t width = 76;

		for (uint8_t i = 0; i < 5; i++){
			tft.drawRect(81,15+(19*i),78,18,ST7735_BLACK);
			tft.drawRect(82,16+(19*i),76,16,ST7735_BLACK);
		}
		
		uint8_t col = ParameterNr;
		
		// draw current Frame
		tft.drawRect(Xpos,15+(19*col),width+2,18,ST7735_YELLOW);
	}
	
	// SYSTEM sub page (SysEx Dump) ---------------------------------------
	else if(PageNr == 12 && myPageShiftStatus[PageNr] == true) {
		uint8_t Xpos = 119;
		uint8_t width = 39;
		uint8_t hight = 18;
		
		// clear all Frames
		for (uint8_t i = 0; i < 5; i++){
			tft.drawRect(Xpos,15+(19*i),width,hight,ST7735_BLACK);
		}
			
		// draw current Frame
		uint8_t col = ParameterNr;
		tft.drawRect(Xpos,15+(19*col),width,hight,ST7735_YELLOW);
	}
}

//*************************************************************************
// draw Parameter Frame into MODMATRIX
// ************************************************************************
FLASHMEM void drawParamterFrame2(uint8_t PageNr, uint8_t ParameterNr)
{
	uint8_t Xpos = 66;
	uint8_t width = 30;
	
	if (ParameterNr >= 4){
		return;
	}
	
	// clear all Frames
	for (uint8_t i = 0; i < 4; i++){
		tft.drawRect(Xpos,34+(19*i),width,18,ST7735_BLACK);
	}
	
	// draw current Frame
	uint8_t col = ParameterNr;
	tft.drawRect(Xpos,34+(19*col),width,18,ST7735_YELLOW);
}

//*************************************************************************
// read ModMatrix Parameter
//*************************************************************************
FLASHMEM void readModMatrixParameter (uint8_t ParameterNo) {
	
	uint8_t xpos, ypos, parNo, value, Page;
	
	Page = ParameterNo / 4;
	
	for (uint8_t i = 0; i < 4; i++) {
		value = readModParameter((Page * 4) + i);		
		ypos = 40 + (19 * i);
		parNo = ((Page * 4) + i);
		tft.setTextColor(ST7735_WHITE);
		tft.fillRoundRect(70,38+(19 * i),22,10,2,ST7735_BLUE);
		
		if (value < 10) {
			xpos = 78;
		}
		else if (value < 100) {
			xpos = 76;
		}
		else xpos = 72;
		
		// PWM1/PWM2 waveform
		if (parNo == 4 || parNo == 5) {
			xpos = 72;
			value = value >> 5;
			if (value == 0) {
				print_String(278, xpos,ypos); // print "SIN"
			}
			else if (value == 1) {
				print_String(279, xpos,ypos); // print "TRI"
			}
			else if (value == 2) {
				print_String(280, xpos,ypos); // print "SAW"
			}
			else {
				print_String(281, xpos,ypos); // print "SQR"
			}
		}
		else {
			tft.setCursor(xpos, ypos);
			tft.print(value);
		}
	}
}

//*************************************************************************
// print ModMatrix Parameter
//*************************************************************************
FLASHMEM void printModParameter(uint8_t ParameterNo)
{
	tft.fillRect(2,32,155,80,ST7735_BLACK);
	// draw Rect and Lines
	for (uint8_t i = 0; i < 4; i++){
		tft.fillRoundRect(70,38+(19*i),22,10,2,ST7735_BLUE);
	}
	tft.drawFastVLine(0,14,95,ST7735_GRAY);
	//tft.drawFastVLine(80,14,95,ST7735_GRAY);
	tft.drawFastVLine(159,14,95,ST7735_GRAY);
	for (uint8_t i = 0; i < 6; i++){
		tft.drawFastHLine(0,14+(19*i),160,ST7735_GRAY);
	}
	
	// print Slot No
	uint8_t SlotPage = (ParameterNo / 4);
	tft.setTextColor(ST7735_GRAY);
	for (uint8_t i = 0; i < 4; i++) {
		tft.setCursor(4,39 + (i * 19));
		tft.print((SlotPage * 4) + i + 1);
	}

	tft.setTextColor(ST7735_WHITE);
	for (uint8_t i = 0; i < 4; i++) {
		tft.setCursor(78,40 + (i * 19));
		tft.print("0");
	}
	
	tft.setTextColor(ST7735_GRAY);

	if (ParameterNo < 4) {
		print_String(179,19,(39 + (0 * 19)));	// print "LFO1"
		print_String(180,19,(39 + (1 * 19)));	// print "LFO2"
		print_String(183,19,(39 + (2 * 19)));	// print "LFO1*MW"
		print_String(138,19,(39 + (3 * 19)));	// print "PBEND"
		print_String(181,103,(39 + (0 * 19)));	// print "Pitch"
		print_String(182,103,(39 + (1 * 19)));	// print "CUTOFF"
		print_String(181,103,(39 + (2 * 19)));	// print "Pitch"
		print_String(181,103,(39 + (3 * 19)));	// print "Pitch"
	}
	else if (ParameterNr < 8) {
		print_String(184,19,(39 + (0 * 19)));	// print "PWMA"
		print_String(185,19,(39 + (1 * 19)));	// print "PWMB"
		print_String(186,19,(39 + (2 * 19)));	// print "OSC1"
		print_String(187,19,(39 + (3 * 19)));	// print "OSC2"
		print_String(186,103,(39 + (0 * 19)));	// print "OSC1"
		print_String(187,103,(39 + (1 * 19)));	// print "OSC2"
		print_String(182,103,(39 + (2 * 19)));	// print "Cutoff"
		print_String(182,103,(39 + (3 * 19)));	// print "Cutoff"
	}
	else if (ParameterNr < 12) {
		print_String(215,24,(39 + (0 * 19)));	// print "LFO3"
		print_String(215,24,(39 + (1 * 19)));	// print "LFO3"
		print_String(215,24,(39 + (2 * 19)));	// print "LFO3"
		print_String(215,24,(39 + (3 * 19)));	// print "LFO3"
		print_String(53,103,(39 + (0 * 19)));	// print "VCA"
		print_String(205,103,(39 + (1 * 19)));	// print "FxP1"
		print_String(206,103,(39 + (2 * 19)));	// print "FxP2"
		print_String(207,103,(39 + (3 * 19)));	// print "FxP3"
		
	}
	else if (ParameterNr < 16) {
		print_String(139,24,(39 + (0 * 19)));	// print "MWHEEL"
		print_String(215,24,(39 + (1 * 19)));	// print "LFO3"
		print_String(212,24,(39 + (2 * 19)));	// print "Atouch"
		print_String(212,24,(39 + (3 * 19)));	// print "Atouch"
		print_String(182,103,(39 + (0 * 19)));	// print "CUTOFF"
		print_String(210,103,(39 + (1 * 19)));	// print "FxMIX"
		print_String(182,103,(39 + (2 * 19)));	// print "Cutoff"
		print_String(181,103,(39 + (3 * 19)));	// print "Pitch"
		
	}
	else if (ParameterNr < 20) {
		print_String(212,24,(39 + (0 * 19)));	// print "Atouch"
		print_String(212,24,(39 + (1 * 19)));	// print "Atouch"
		print_String(212,24,(39 + (2 * 19)));	// print "Atouch"
		print_String(212,24,(39 + (3 * 19)));	// print "Atouch"
		print_String(217,103,(39 + (0 * 19)));	// print "LFO1 Amt"
		print_String(218,103,(39 + (1 * 19)));	// print "LFO2 Amt"
		print_String(205,103,(39 + (2 * 19)));	// print "FxP1"
		print_String(206,103,(39 + (3 * 19)));	// print "FxP2"
		
	}
	else if (ParameterNr < 24) {
		print_String(212,24,(39 + (0 * 19)));	// print "Atouch"
		print_String(139,24,(39 + (1 * 19)));	// print "MWHEEL"
		print_String(212,24,(39 + (2 * 19)));	// print "Atouch"
		print_String(164,24,(39 + (3 * 19)));	// print " "
		print_String(207,103,(39 + (0 * 19)));	// print "FxP3"
		print_String(282,103,(39 + (1 * 19)));	// print "HPF"
		print_String(210,103,(39 + (2 * 19)));	// print "FxMIX"
		print_String(164,103,(39 + (3 * 19)));	// print " "
	}
	else if (ParameterNr < 28) {
		print_String(180,24,(39 + (0 * 19)));	// print "LFO2"
		print_String(180,24,(39 + (1 * 19)));	// print "LFO2"
		print_String(180,24,(39 + (2 * 19)));	// print "LFO2"
		print_String(180,24,(39 + (3 * 19)));	// print "LFO2"
		print_String(284,103,(39 + (0 * 19)));	// print "Os1 PRM_A"
		print_String(285,103,(39 + (1 * 19)));	// print "Os1 PRM_B"
		print_String(286,103,(39 + (2 * 19)));	// print "Os2 PRM_A"
		print_String(287,103,(39 + (3 * 19)));	// print "Os2 PRM_B"
	}
	else if (ParameterNr < 32) {
		print_String(215,24,(39 + (0 * 19)));	// print "LFO3"
		print_String(215,24,(39 + (1 * 19)));	// print "LFO3"
		print_String(215,24,(39 + (2 * 19)));	// print "LFO3"
		print_String(215,24,(39 + (3 * 19)));	// print "LFO3"
		print_String(284,103,(39 + (0 * 19)));	// print "Os1 PRM_A"
		print_String(285,103,(39 + (1 * 19)));	// print "Os1 PRM_B"
		print_String(286,103,(39 + (2 * 19)));	// print "Os2 PRM_A"
		print_String(287,103,(39 + (3 * 19)));	// print "Os2 PRM_B"
	}
	else if (ParameterNr < 36) {
		print_String(288,24,(39 + (0 * 19)));	// print "ENV1"
		print_String(288,24,(39 + (1 * 19)));	// print "ENV1"
		print_String(288,24,(39 + (2 * 19)));	// print "ENV1"
		print_String(288,24,(39 + (3 * 19)));	// print "ENV1"
		print_String(284,103,(39 + (0 * 19)));	// print "Os1 PRM_A"
		print_String(285,103,(39 + (1 * 19)));	// print "Os1 PRM_B"
		print_String(286,103,(39 + (2 * 19)));	// print "Os2 PRM_A"
		print_String(287,103,(39 + (3 * 19)));	// print "Os2 PRM_B"
	}
}

//*************************************************************************
// print green Frame into FxMix
//*************************************************************************
FLASHMEM void printFxValFrame(uint8_t value)
{
	// clear all frames
	tft.drawRect(1,84,52,15,ST7735_BLACK);
	tft.drawRect(2,85,50,13,ST7735_BLACK);
	tft.drawRect(54,84,53,15,ST7735_BLACK);
	tft.drawRect(55,85,51,13,ST7735_BLACK);
	tft.drawRect(108,84,51,15,ST7735_BLACK);
	tft.drawRect(109,85,49,13,ST7735_BLACK);
	tft.fillRect(63,38,65,11,ST7735_BLACK);
	const uint16_t myColor = ST7735_YELLOW;
	
	tft.setTextColor(ST7735_GRAY);
	tft.setCursor(65,40);
	tft.print("Clk");
	uint16_t fxRate = (FxClkRate / 1000);
	tft.setCursor(85,40);
	tft.print(fxRate);
	fxRate = (FxClkRate % 1000);
	fxRate = (fxRate / 100);
	tft.setCursor(96,40);
	tft.print(".");
	tft.setCursor(103,40);
	tft.print(fxRate);
	tft.setCursor(110,40);
	tft.print("KHz");
	
	if (value < 99)
	{
		if (value == 0) {
			tft.drawRect(1,84,52,15,myColor);
		}
		else if (value == 1) {
			tft.drawRect(54,84,53,15,myColor);
		}
		else if (value == 2) {
			tft.drawRect(108,84,51,15,myColor);
		}
		else {
			tft.fillRoundRect(63,38,65,11,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(65,40);
			tft.print("Clk");
			uint16_t fxRate = (FxClkRate / 1000);
			tft.setCursor(85,40);
			tft.print(fxRate);
			fxRate = (FxClkRate % 1000);
			fxRate = (fxRate / 100);
			tft.setCursor(96,40);
			tft.print(".");
			tft.setCursor(103,40);
			tft.print(fxRate);
			tft.setCursor(110,40);
			tft.print("KHz");
		}
	}
}

//*************************************************************************
// print green Frame into FxMix
//*************************************************************************
FLASHMEM void printFxPotValue(uint8_t index, uint8_t value)
{
	uint8_t xpos = 0;
	
	if (index == 0) {
		tft.fillRoundRect(22,86,25,10,2,ST7735_BLUE);
		FxTimeValue = value;
		//FxPot1value = value;
		if (value < 10) {
			xpos = 32;
		}
		else if (value < 100) {
			xpos = 29;
		}
		else {
			xpos = 25;
		}
	}
	else if (index == 1) {
		tft.fillRoundRect(76,86,25,10,2,ST7735_BLUE);
		FxFeedbackValue = value;
		//FxPot2value = value;
		if (value < 10) {
			xpos = 86;
		}
		else if (value < 100) {
			xpos = 83;
		}
		else {
			xpos = 79;
		}
	}
	else if (index == 2) {
		tft.fillRoundRect(128,86,25,10,2,ST7735_BLUE);
		//FxPot3value = value;
		if (value < 10) {
			xpos = 138;
		}
		else if (value < 100) {
			xpos = 135;
		}
		else {
			xpos = 131;
		}
	}
	if (index <= 2) {
		tft.setCursor(xpos,88);
		tft.setTextColor(ST7735_WHITE);
		tft.print(value);
		drawFxGrafic(FxMixValue, FxTimeValue, FxFeedbackValue, FxPot3value);
	}
}

//*************************************************************************
// draw Fx grafic
//*************************************************************************
FLASHMEM void drawFxGrafic (uint8_t volume, uint8_t time, uint8_t feedback, uint8_t Pot3value)
{
	uint16_t x1 = 23;
	uint16_t x2 = x1 + 3 + (time /8);
	uint8_t  y1 = 70;
		
	//clear grafic
	tft.fillRect(23,27,8,43,ST7735_BLACK);
	tft.fillRect(33,49,108,21,ST7735_BLACK);
	
	// draw pillar position
	for (uint8_t i = 0; i < 12; i++)
	{
		tft.fillRect(x1,y1 -1,1,1,ST7735_ORANGE);
		x1 = x1 + 10;
		x2 = x2 + 10;
	}
	
	// draw new grafic
	x1 = 23;
	x2 = 2 + (time /20);
	y1 = volume * 100 / 295;
	uint8_t temp = y1;
	uint8_t fb_loop = (feedback / 11);
	tft.fillRect(x1,70-temp,x2,temp,ST7735_ORANGE);
	float myX2 = (float)(x2 / 127.0f) * Pot3value;
	uint8_t xx2 = (uint8_t)myX2;
	tft.fillRect(x1,70-(temp-1),xx2-1,temp-1,ST7735_YELLOW);	// Pot3 value
	
	for (uint8_t i = 1; i <= fb_loop; i++)
	{
		x1 = x1 + 10;
		temp = (y1 / (i+1));
		tft.fillRect(x1,70-temp,x2,temp,ST7735_ORANGE);
		tft.fillRect(x1,70-(temp-1),xx2-1,temp-1,ST7735_YELLOW); // Pot3 value
	}
}

//*************************************************************************
// draw Fx prg.name
//*************************************************************************
FLASHMEM void printFxName(uint8_t PrgNo)
{
	tft.fillRect(65,27,90,9,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	tft.setCursor(65,28);
	
	switch (PrgNo) {
		case 1: tft.print("CHORUS 1"); break;
		case 2: tft.print("FLANGER"); break;
		case 3: tft.print("TREMOLO"); break;
		case 4: tft.print("PITCH-SHIFT"); break;
		case 5: tft.print("PITCH-ECHO"); break;
		case 6: tft.print("REVERB 1"); break;
		case 7: tft.print("REVERB 2"); break;
		case 8: tft.print("X-DELAY"); break;
		case 9: tft.print("CHORUS 2"); break;
		case 10: tft.print("TONATOR"); break;
		case 11: tft.print("PHASER"); break;
		case 12: tft.print("FLANGERATOR"); break;
		case 13: tft.print("DISTORTER"); break;
		case 14: tft.print("STEREO-ECHO-REV"); break;
		case 15: tft.print("GATE"); break;
	}
}

//*************************************************************************
// draw Fx Pot
//*************************************************************************
FLASHMEM void printFxPOT(uint8_t PotNo, uint8_t PrgNo)
{
	if (PrgNo == 0) {
		return;
	}
	
	tft.fillRect(3,71,155,10,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	print_String(65,75,72);			// print "Time/Feedb."
	
	if (PotNo == 0) {
		tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
		tft.setTextColor(ST7735_BLACK);
		// print Fx Parameter Name
		print_String((FxName[PrgNo][PotNo]),FxName[PrgNo][PotNo + 3],72);
	}
	else if (PotNo == 1) {
		tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
		tft.setTextColor(ST7735_BLACK);
		// print Fx Parameter Name
		print_String((FxName[PrgNo][PotNo]),FxName[PrgNo][PotNo + 3],72);
	}
	else if (PotNo == 2) {
		tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
		tft.setTextColor(ST7735_BLACK);
		// print Fx Parameter Name
		print_String((FxName[PrgNo][PotNo]),FxName[PrgNo][PotNo + 3],72);
	}
}

//*************************************************************************
// coding pot WaveNo
//*************************************************************************
FLASHMEM int codingPotWaveNo (int value)
{
	if (value >= 0 && value < 7) {
		return 0;
		} else if (value >= 7 && value < 23) {
		return 1;
		} else if (value >= 23 && value < 40) {
		return 2;
		} else if (value >= 40 && value < 60) {
		return 3;
		} else if (value >= 60 && value < 80) {
		return 4;
		} else if (value >= 80 && value < 100) {
		return 5;
		} else if (value >= 100 && value < 120) {
		return 6;
		} else {
		return 7;
	}
}

//*************************************************************************
// draw SVF/Ladder filter curves (0-127)
//*************************************************************************
FLASHMEM void draw_filter_curves (uint8_t FilterFrq, uint8_t FilterRes, uint8_t FilterMix) {
	
	uint16_t MixColor = 0;
	
	// Ladder Filter
	if (myFilter == 2) {
		FilterMix = 0;
	}
	
	// Filer_variable
	int Contrl_1_cut = FilterFrq;
	int Contrl_2_res = FilterRes;
	
	uint8_t FilterTyp = 0;
	if (FilterMix >= 63 && FilterMix < 125){
		FilterTyp = 1;
	}
	else if(FilterMix >= 125) {
		FilterTyp = 2;
	}
	
	uint8_t mixValue = FilterMix;
	if (mixValue > 63) {
		mixValue = 63;
	}
	
	// clear old curve
	tft.fillRect(20,28,127,58,ST7735_BLACK);
	
	// LowPass curve (Red Color) ----------------------------------------
	if (FilterTyp == 0){
		const uint8_t x_plot = 40;
		const uint8_t y_plot = 85;
		uint8_t x = 40;
		uint16_t r = 0;
		uint8_t y = 0;
		uint8_t y_ = 0;
		uint8_t i = 0;
		uint8_t i_ = 0;
		uint8_t q_typ = 12;			// Filter Q
		uint16_t resonance = 0;
		int xx = 0;
		
		for (i = 0; i < 48; i++) {
			resonance = (FILTERCURVE[(i * 8)]);
			resonance = (((resonance >> 2) * Contrl_2_res) >> 7);
			r = (x + r - y) + resonance;
			y = r / q_typ;
			xx = (Contrl_1_cut / 1.36f) + x_plot - i_;
			if (xx >= 21) {
				tft.drawLine(xx, (y_plot - y_ / 2), (Contrl_1_cut / 1.36f) + x_plot - i, (y_plot - y / 2), ST7735_RED);
			}
			i_ = i;
			y_ = y;
		}
		xx = (Contrl_1_cut / 1.36f) - 5;
		if (xx >= 21) {
			tft.drawLine(20, (y_plot - y_ / 2), (Contrl_1_cut / 1.36f) -5, (y_plot - y_ / 2), ST7735_RED);
		}
		
		
		// HighPass Mix curve (Yello color)
		if (FilterMix > 40 && FilterMix < 85){
			
			// colormix b, g, r
			MixColor = tft.Color565(0, Colormix[mixValue], Colormix[mixValue]);
			
			const int8_t x_plot = -5;
			const uint8_t y_plot = 85;
			uint8_t x = 40;
			uint16_t r = 0;
			uint8_t y = 0;
			uint8_t y_ = 0;
			uint8_t i = 0;
			uint8_t i_ = 0;
			uint8_t q_typ = 12;			// Filter Q
			uint16_t resonance = 0;
			
			for (i = 0; i < 48; i++) {
				resonance = (FILTERCURVE[(i * 8)]);
				resonance = (((resonance >> 2) * Contrl_2_res) >> 7);
				r = (x + r - y) + resonance;
				y = r / q_typ;
				xx = (Contrl_1_cut / 1.36f) + x_plot + i_;
				if (xx >= 21 && xx < 131) {
					tft.drawLine(xx, (y_plot - y_ / 2), (Contrl_1_cut / 1.36f) + x_plot + i, (y_plot - y / 2), MixColor);
				}
				i_ = i;
				y_ = y;
			}
			xx = 43 + (Contrl_1_cut / 1.36);
			if (xx > 21 && xx < 131){
				tft.drawLine(xx, (y_plot - y_ / 2), 131, (y_plot - y_ / 2), MixColor);
			}
		}
	}
	
	// HighPass curve (Yellow color) ------------------------------------
	else if (FilterTyp == 1){
		
		// FilterMix (draw LP curve in background)
		if (FilterMix >= 63 && FilterMix < 125){
			// colormix b, g, r
			MixColor = tft.Color565(0, 0, Colormix[(125 - FilterMix)]);
			const uint8_t x_plot = 40;
			const uint8_t y_plot = 85;
			uint8_t x = 40;
			uint16_t r = 0;
			uint8_t y = 0;
			uint8_t y_ = 0;
			uint8_t i = 0;
			uint8_t i_ = 0;
			uint8_t q_typ = 12;			// Filter Q
			uint16_t resonance = 0;
			int xx = 0;
			
			for (i = 0; i < 48; i++) {
				resonance = (FILTERCURVE[(i * 8)]);
				resonance = (((resonance >> 2) * Contrl_2_res) >> 7);
				r = (x + r - y) + resonance;
				y = r / q_typ;
				xx = (Contrl_1_cut / 1.36f) + x_plot - i_;
				if (xx >= 21) {
					tft.drawLine(xx, (y_plot - y_ / 2), (Contrl_1_cut / 1.36f) + x_plot - i, (y_plot - y / 2), MixColor);
				}
				i_ = i;
				y_ = y;
			}
			xx = (Contrl_1_cut / 1.36f) - 5;
			if (xx >= 21) {
				tft.drawLine(21, (y_plot - y_ / 2), (Contrl_1_cut / 1.36f) -5, (y_plot - y_ / 2), MixColor);
			}
			
			
			// HighPass Mix curve (Yello color)
			if (FilterMix >= 63 && FilterMix < 125){
				
				// colormix b, g, r
				MixColor = tft.Color565(0, Colormix[mixValue], Colormix[mixValue]);
				
				const int8_t x_plot = -5;
				const uint8_t y_plot = 85;
				uint8_t x = 40;
				uint16_t r = 0;
				uint8_t y = 0;
				uint8_t y_ = 0;
				uint8_t i = 0;
				uint8_t i_ = 0;
				uint8_t q_typ = 12;			// Filter Q
				uint16_t resonance = 0;
				
				for (i = 0; i < 48; i++) {
					resonance = (FILTERCURVE[(i * 8)]);
					resonance = (((resonance >> 2) * Contrl_2_res) >> 7);
					r = (x + r - y) + resonance;
					y = r / q_typ;
					xx = (Contrl_1_cut / 1.36f) + x_plot + i_;
					if (xx >= 21 && xx < 131) {
						tft.drawLine(xx, (y_plot - y_ / 2), (Contrl_1_cut / 1.36f) + x_plot + i, (y_plot - y / 2), ST7735_YELLOW);
					}
					i_ = i;
					y_ = y;
				}
				xx = 43 + (Contrl_1_cut / 1.36);
				if (xx > 21 && xx < 131){
					tft.drawLine(xx, (y_plot - y_ / 2), 131, (y_plot - y_ / 2), ST7735_YELLOW);
				}
			}
		}
	}
	
	// BandPass curve ---------------------------------------------------
	else if (FilterTyp == 2){
		
		const int8_t x_plot = -5;
		const uint8_t y_plot = 85;
		uint8_t x = 40;
		uint16_t r = 0;
		uint8_t y = 0;
		uint8_t y_ = 0;
		uint8_t i = 0;
		uint8_t i_ = 0;
		uint8_t q_typ = 12;			// Filter Q
		uint16_t resonance = 0;
		
		for (i = 0; i < 23; i++)
		{
			resonance = (FILTERCURVE[(i * 8)]);
			resonance = (((resonance >> 2) * Contrl_2_res) >> 7);
			r = (x + r - y) + resonance;
			y = r / q_typ;
			int xx = (Contrl_1_cut / 1.36f) + x_plot + i_;
			if (xx > 19 && xx < 131) {
				tft.drawLine(xx, (y_plot - y_ / 2), (Contrl_1_cut / 1.36f) + x_plot + i, (y_plot - y / 2), ST7735_GREEN);
			}
			xx = (Contrl_1_cut / 1.36f);
			if (xx < 1) {
				xx = 1;
			}
			tft.drawLine(xx + x_plot + 46 - i_,(y_plot - y_ / 2),xx + x_plot + 46 - i,(y_plot - y / 2),ST7735_GREEN);
			i_ = i;
			y_ = y;
		}
	}
	
	// draw dotline for ENV and LFO ---------------------------------------
	
	int xplot = (Contrl_1_cut / 1.36f) + 18;
	
	// draw vertikal cutoff dotline
	if (xplot > 23) {
		for (int i = 42; i < 86; i++) {
			tft.drawPixel(xplot, i, ST7735_WHITE);
			i++;
		}
	}
	else {
		for (int i = 42; i < 86; i++) {
			tft.drawPixel(23, i, ST7735_WHITE);
			i++;
		}
	}

	int vcfamt = filterEnv * 55;

	if (xplot > 23) {
		if (vcfamt < 0) {
			vcfamt = -vcfamt;
			for (int i = 0; i < vcfamt; i++) {
				int plotx = xplot - i;
				if (plotx <= 23) {
					plotx = 23;
				}
				// draw left Env arrow line
				tft.drawPixel(plotx, 44, ST7735_WHITE);
				i++;
			}
			// draw left arrow
			int posx = xplot - vcfamt;
			if (posx <= 23) {
				posx = 23;
			}
			tft.drawFastVLine(posx,42,5,ST7735_WHITE);
			tft.drawFastVLine(posx - 1,43,3,ST7735_WHITE);
			tft.drawPixel(posx - 2,44,ST7735_WHITE);
			posx -= 8;
			if (posx <= 25) {
				posx = 25;
			}
			drawEnvMarkerVCF(posx);
		}
		// draw Env arrow in middle
		else if(vcfamt == 0){
			int posx = xplot + vcfamt;
			tft.drawFastVLine(posx,42,5,ST7735_WHITE);
			tft.drawFastVLine(posx + 1,43,3,ST7735_WHITE);
			tft.drawPixel(posx + 2,44,ST7735_WHITE);
			tft.drawFastVLine(posx,42,5,ST7735_WHITE);
			tft.drawFastVLine(posx - 1,43,3,ST7735_WHITE);
			tft.drawPixel(posx - 2,44,ST7735_WHITE);
			posx -= 8;
			if (posx <= 25) {
				posx = 25;
			}
			drawEnvMarkerVCF(posx);
		}
		// draw right Env arrow line
		else if(vcfamt > 0){
			for (int i = 0; i < vcfamt; i++) {
				int posx = xplot + i;
				if (posx >= 130) {
					posx = 130;
				}
				tft.drawPixel(posx, 44, ST7735_WHITE);
				i++;
			}
			int posx = xplot + vcfamt;
			if (posx >= 130) {
				posx = 130;
			}
			tft.drawFastVLine(posx,42,5,ST7735_WHITE);
			tft.drawFastVLine(posx + 1,43,3,ST7735_WHITE);
			tft.drawPixel(posx + 2,44,ST7735_WHITE);
			posx = posx - 8;
			if (posx <= 25) {
				posx = 25;
			}
			if (posx >= 118) {
				posx = 118;
			}
			drawEnvMarkerVCF(posx);
		}
	}

	else if(xplot > 0 && xplot <= 23 && vcfamt >= 0){
		int posx = 0;
		for (int i = 0; i < vcfamt; i++) {
			posx = 23 + i;
			if (posx <= 23) {
				posx = 23;
			}
			tft.drawPixel(posx, 44, ST7735_WHITE);
			i++;
		}
		posx = 23 + vcfamt;
		if (vcfamt == 0) {
			tft.drawFastVLine(posx,42,5,ST7735_WHITE);
			tft.drawFastVLine(posx + 1,43,3,ST7735_WHITE);
			tft.drawPixel(posx + 2,44,ST7735_WHITE);
			tft.drawFastVLine(posx,42,5,ST7735_WHITE);
			tft.drawFastVLine(posx - 1,43,3,ST7735_WHITE);
			tft.drawPixel(posx - 2,44,ST7735_WHITE);
		}
		else if (vcfamt > 0) {
			tft.drawFastVLine(posx,42,5,ST7735_WHITE);
			tft.drawFastVLine(posx + 1,43,3,ST7735_WHITE);
			tft.drawPixel(posx + 2,44,ST7735_WHITE);
		}
		else {
			tft.drawFastVLine(posx,42,5,ST7735_WHITE);
			tft.drawFastVLine(posx - 1,43,3,ST7735_WHITE);
			tft.drawPixel(posx - 2,44,ST7735_WHITE);
		}
		posx = posx - 8;
		if (posx <= 25){
			posx = 25;
		}
		drawEnvMarkerVCF(posx);
	}

	else if (vcfamt < 0) {
		int posx = 23;
		tft.drawFastVLine(posx,42,5,ST7735_WHITE);
		tft.drawFastVLine(posx - 1,43,3,ST7735_WHITE);
		tft.drawPixel(posx - 2,44,ST7735_WHITE);
		if (posx <= 25){
			posx = 25;
		}
		drawEnvMarkerVCF(posx);
	}

	// draw horizontal LFO line
	float LFOamt = (filterLfoAmt * FILTERMODMIXERMAX);
	uint8_t FILvalue = 0;
	for (int i = 0; i < 128; i++) {
		float LFOfilterAmt = LINEARLFO[i];
		if (LFOamt <= LFOfilterAmt){
			FILvalue = i;
			break;
		}
	}
	FILvalue = FILvalue / 2.3f;

	if (xplot > 0) {
		int posx = xplot + 7;
		drawLFOMarkerVCF(posx);
		posx -= 7;
		if (posx <= 23) {
			posx = 23;
		}
		if (filterLfoAmt == 0) {
			// draw middle LFO arrows
			tft.drawFastVLine(posx,72,5,ST7735_WHITE);
			tft.drawFastVLine(posx + 1,73,3,ST7735_WHITE);
			tft.drawPixel(posx + 2,74,ST7735_WHITE);
			tft.drawFastVLine(posx,72,5,ST7735_WHITE);
			tft.drawFastVLine(posx - 1,73,3,ST7735_WHITE);
			tft.drawPixel(posx - 2,74,ST7735_WHITE);
		}
		else {
			for (int i = 0; i < FILvalue; i++) {
				posx = xplot + i;
				if (posx  >= 130) {
					posx = 130;
				}
				if (posx <= 23) {
					posx = 23;
				}
				// draw right LFO arrow line
				tft.drawPixel(posx, 74, ST7735_WHITE);
				posx = xplot - i;
				if (posx <= 23) {
					posx = 23;
				}
				// draw left LFO arrow line
				tft.drawPixel(posx, 74, ST7735_WHITE);
				i++;
			}
			posx = xplot + FILvalue;
			if (posx >= 130) {
				posx = 130;
			}
			if (posx <= 23) {
				posx = 23;
			}
			// draw right LFO arrow
			tft.drawFastVLine(posx,72,5,ST7735_WHITE);
			tft.drawFastVLine(posx + 1,73,3,ST7735_WHITE);
			tft.drawPixel(posx + 2,74,ST7735_WHITE);
			posx = xplot - FILvalue;
			if (posx <= 23) {
				posx = 23;
			}
			// draw left LFO arrow
			tft.drawFastVLine(posx,72,5,ST7735_WHITE);
			tft.drawFastVLine(posx - 1,73,3,ST7735_WHITE);
			tft.drawPixel(posx - 2,74,ST7735_WHITE);
		}
		
	}
}


//*************************************************************************
// draw HPF filter curves (0-127)
//*************************************************************************
FLASHMEM void draw_HPF_filter_curves (uint8_t FilterFrq, uint8_t FilterRes)
{
	// Filer_variable
	int Contrl_1_cut = FilterFrq;
	int Contrl_2_res = FilterRes;	
	const int8_t x_plot = -5;
	const uint8_t y_plot = 85;
	uint8_t x = 40;
	uint16_t r = 0;
	uint8_t y = 0;
	uint8_t y_ = 0;
	uint8_t i = 0;
	uint8_t i_ = 0;
	uint8_t q_typ = 12;			// Filter Q
	uint16_t resonance = 0;
	int xx = 0;
	
	// clear old curve
	tft.fillRect(20,28,127,58,ST7735_BLACK);
	
	for (i = 0; i < 48; i++) {
		resonance = (FILTERCURVE[(i * 8)]);
		resonance = (((resonance >> 2) * Contrl_2_res) >> 7);
		r = (x + r - y) + resonance;
		y = r / q_typ;
		xx = (Contrl_1_cut / 1.36f) + x_plot + i_;
		if (xx >= 21 && xx < 131) {
			tft.drawLine(xx, (y_plot - y_ / 2), (Contrl_1_cut / 1.36f) + x_plot + i, (y_plot - y / 2), ST7735_MAGENTA);
		}
		i_ = i;
		y_ = y;
	}
	xx = 43 + (Contrl_1_cut / 1.36);
	if (xx > 21 && xx < 131){
		tft.drawLine(xx, (y_plot - y_ / 2), 131, (y_plot - y_ / 2), ST7735_MAGENTA);
	}
}

//*************************************************************************
// draw Envelope curve
//*************************************************************************
FLASHMEM void drawEnvCurve(uint8_t ATKvalue, uint8_t DCYvalue, uint8_t SUSvalue, uint8_t RELvalue)
{
	int8_t envelopeType;
	uint8_t xplot1 = EXP127[ATKvalue];
	uint8_t xplot1_decay = EXP127[DCYvalue];
	uint8_t yplot2_sustain = SUSvalue;
		
	if (PageNr == 4) {
		envelopeType = envelopeType1;
	} else envelopeType = envelopeType2;
	
	// delete old Envelope lines ------------------------------------------
	tft.fillRect(8,36,115,52,ST7735_BLACK);
	
	// draw new Attack line ----------------------------------------------
	float k = (envelopeType / 2);
	float i;
	float I = ((EXP127[ATKvalue]) / 4);
	
	float result;
	float i2 = 0;
	float value2 = 0;
	
	if (k == 0) {
		k = 0.01f;
	}
	
	xplot1 = 10 + ((EXP127[ATKvalue]) / 4);
	
	tft.fillRect(xplot1 - 2,37,5,5,ST7735_BLUE);  // draw blue little rectangle
	if (I <= 3) {
		tft.drawLine(9 + 0, 85, 9 + I, 85 - 45,ST7735_RED);
	}
	else {
		for (i = 0; i <= I; i++) {
			result = (exp(k*i/I) - exp(0)) / (exp(k) - exp(0));
			int value = float(result * 45);
			tft.drawLine(9 + i2, 85 - value2, 9 + i, 85 - value ,ST7735_RED);
			i2 = i;
			value2 = value;
		}
	}
	
	// draw new Decay Line -----------------------------------------------
	xplot1_decay = ((EXP127[DCYvalue]) / 4);
	yplot2_sustain = 85 - ((SUSvalue * 85)/239);
	tft.fillRect(xplot1 + xplot1_decay -2,yplot2_sustain-3,5,5,ST7735_BLUE);
	
	k = -4.0f;
	I = ((EXP127[DCYvalue]) / 4);
	i2 = 0;
	value2 = 0;
	
	if (I <= 3) {
		tft.drawLine(xplot1,40,xplot1 + xplot1_decay,yplot2_sustain,ST7735_RED);
		} else {
		for (i = 0; i <= I; i++) {
			result = (exp(k*i/I) - exp(0)) / (exp(k) - exp(0));
			int value = float(result * (46 - (SUSvalue / 2.8f)));
			tft.drawLine(xplot1 + i2, 40 + value2, xplot1 + i, 40 + value, ST7735_RED);
			i2 = i;
			value2 = value;
		}
	}
	
	// draw new Sustain Line ---------------------------------------------
	tft.fillRect(88,yplot2_sustain-3,5,5,ST7735_BLUE);
	tft.drawLine(xplot1 + xplot1_decay,yplot2_sustain,90,yplot2_sustain,ST7735_RED);
	
	// draw new Release line ---------------------------------------------
	k = -4.0f;
	I = ((EXP127[RELvalue]) / 4);
	i2 = 0;
	value2 = 0;
	
	if (I <= 3) {
		tft.drawLine(90, 85 - (SUSvalue / 2.8f), 90, 85,ST7735_RED);
		} else {
		for (i = 0; i <= I; i++) {
			result = (exp(k*i/I) - exp(0)) / (exp(k) - exp(0));
			int value = float(result * (45 -(45 - (SUSvalue / 2.8f))));
			tft.drawLine(90 + i2, yplot2_sustain + value2, 90 + i, yplot2_sustain + value,ST7735_RED);
			i2 = i;
			value2 = value;
		}
	}
	
	// draw vertikal point line
	for (uint8_t i = 40; i <= 85; i++){
		tft.drawPixel(90,i,ST7735_GRAY);
		i++;
		i++;
	}
}

//*************************************************************************
// draw waveform in oscillator menu page
//*************************************************************************
FLASHMEM void draw_Waveform(int WaveNr, uint16_t waveColor)
{
	int x1 = 87;
	int x2 = 87;
	int y1 = 45;
	int y2 = 45;
	uint16_t sample = 0;
	uint8_t WaveBank = 0;
	
	tft.fillRect(82,17,75,33,ST7735_BLACK);		// clear Wave Screen
	
	// Osc off (Waveform No 0)
	if (WaveNr == 0) {
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(&Picopixel);
		print_String(18,140,22);		// print "OFF"
		tft.setFont(NULL);
		return;
	}
	
	if (PageNr == 1) {
		WaveBank = Osc1WaveBank;
	}
	else if (PageNr == 2) {
		WaveBank = Osc2WaveBank;
	}
	
	// Band Limited waveforms (Waveform No 1 - 13)
	int16_t phase_x;
	if (WaveNr <= 12 && WaveBank == 0) {
		for (int i = 0; i < 64; i++) {
			if (WaveNr == 3) {
				phase_x = i * 4 + (256 * 12);
			} else {
				phase_x = i * 4 + (256 * (WaveNr-1));
			}
			if (WaveNr == 7 || WaveNr == 9 || WaveNr == 10) {
				sample = 255 - pgm_read_byte(&(WAVEFORM_SYMBOLES_8Bit[phase_x])); // Load spezial Symboles and invert
			} else {
				sample = pgm_read_byte(&(WAVEFORM_SYMBOLES_8Bit[phase_x])); // Load spezial Symboles
			}
			int8_t value = (int8_t)sample;
			y2 = 35 - (value / 10 );
			if (i == 0) {
				tft.drawLine(x1,y1,x2,y2, ST7735_BLACK);
			}
			else tft.drawLine(x1,y1,x2,y2, waveColor);
			x1++;
			x2 = x1;
			y1 = y2;
		}
		if (WaveNr == 3) {
			tft.setFont(&Picopixel);
			print_String(233,120,22);	// print "MULTISAW"
			tft.setFont(NULL);
		}
		else if (WaveNr == 5) {
			float value = 0;
			if (PageNr == 1) {
				value = pwA;
			}
			else if (PageNr == 2) {
				value = pwB;
			}
			draw_PWM_curve(value);
		}
		else if (WaveNr == 8) {
			tft.setFont(&Picopixel);
			print_String(162,129,22);	// print "TRI VAR"
			tft.setFont(NULL);
		}
		else if (WaveNr >= 9 && WaveNr <= 11) {
			tft.setFont(&Picopixel);
			print_String(163,118,22);	// print "BANDLIMIT"
			tft.setFont(NULL);
		}
		else if (WaveNr == 12) {
			float value = 0;
			if (PageNr == 1) {
				value = pwA;
			}
			else if (PageNr == 2) {
				value = pwB;
			}
			draw_PWM_curve(value);
		}
		return;
	}
	
	// waveforms Bank A (Waveform No 13 - 63)
	if (WaveNr >= 13 && WaveBank == 0) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * (WaveNr-12));
			sample = 65536 - pgm_read_word(&(ArbBank[phase_x]));
			int16_t value = (int16_t)sample;
			y2 = 35 - (value / 3000 );
			if (i == 0) {
				tft.drawLine(x1,y1,x2,y2, ST7735_BLACK);
			}
			else tft.drawLine(x1,y1,x2,y2, waveColor);
			x1++;
			x2 = x1;
			y1 = y2;
		}
		return;
	}
	
	// waveforms Bank B-O (Waveform No 1 - 63)
	if (WaveBank >= 1 && WaveBank <= 14) {
		long WaveAddr = (16384 * WaveBank);
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = 65536 - pgm_read_word(&((ArbBank + WaveAddr)[phase_x]));
			int16_t value = (int16_t)sample;
			y2 = 35 - (value / 3000 );
			if (i == 0) {
				tft.drawLine(x1,y1,x2,y2, ST7735_BLACK);
			}
			else tft.drawLine(x1,y1,x2,y2, waveColor);
			x1++;
			x2 = x1;
			y1 = y2;
		}
		return;
	}
	
	// Bank P (Mutable Instruments Braids)
	if (WaveBank == 15) {
		if (WaveNr >= 15)
		{
			WaveNr = 15;
		}
		
		tft.setFont(NULL);
		switch(WaveNr) {
			case 1:
				print_String(297,105,24);	// BRAIDS
				print_String(283,109,38);	// VOWEL
			break;
			case 2:
				print_String(298,101,24);	// SHRUTHI
				print_String(291,109,38);	// ZSAW
			break;
			case 3:
				print_String(298,101,24);	// SHRUTHI
				print_String(292,105,38);	// ZSYNC
			break;
			case 4:
				print_String(298,101,24);	// SHRUTHI
				print_String(293,109,38);	// ZTRI
			break;
			case 5:
				print_String(298,101,24);	// SHRUTHI
				print_String(294,105,38);	// ZRESO
			break;
			case 6:
				print_String(298,101,24);	// SHRUTHI
				print_String(295,105,38);	// ZPULSE
			break;
			case 7:
				print_String(298,101,24);	// SHRUTHI
				print_String(296,92,38);	// CRUSHED SINE
			break;
			case 8:
				print_String(297,101,24);	// BRAIDS
				print_String(299,107,38);	// CSAW
			break;	
			case 9:
				print_String(297,101,24);	// BRAIDS
				print_String(300,103,38);	// VOSIM
			break;	
			case 10:
				print_String(297,101,24);	// BRAIDS
				print_String(301,109,38);	// TOY
			break;	
			case 11:
				print_String(297,101,24);	// BRAIDS
				print_String(302,95,38);	// SAWSWARM
			break;	
			case 12:
				print_String(297,101,24);	// BRAIDS
				print_String(303,107,38);	// ZLPF
			break;
			case 13:
				print_String(297,101,24);	// BRAIDS
				print_String(304,107,38);	// ZPKF
			break;	
			case 14:
				print_String(297,101,24);	// BRAIDS
				print_String(305,107,38);	// ZBPF
			break;	
			case 15:
				print_String(297,101,24);	// BRAIDS
				print_String(306,107,38);	// ZHPF
			break;				
		}
		
	}
}

//*************************************************************************
// draw LFO shape
//*************************************************************************
FLASHMEM void drawLFOshape(uint16_t waveColor)
{
	//uint8_t shapeNo = 0;
	uint16_t sample = 0;
	uint16_t tablePoint;
	uint8_t col = 0;
	uint8_t row = 0;
	
	for (uint8_t count = 0; count < 6; count++) {
		
		switch (count) {
			case 0:
			tablePoint = 0; col = 0; row = 0;
			break;
			case 1:
			tablePoint = 256; col = 53; row = 0;
			break;
			case 2:
			tablePoint = 256 * 6; col = 105; row = 0;
			break;
			case 3:
			tablePoint = 0; col = 0; row = 37;
			break;
			case 4:
			tablePoint = 256 * 3; col = 53; row = 37;
			break;
			case 5:
			tablePoint = 256 * 5; col = 105; row = 37;
			break;
		}
		
		int x1 = 4;
		int x2 = 4;
		int y1 = 50;
		int y2 = 50;
		
		// draw Sine, Triangle, Random
		if (count == 0 || count == 1  || count == 3 || count == 5) {
			for (int i = 0; i < 43; i++) {
				int16_t phase_x = ((i * 6) + tablePoint);
				if (count == 1) {
					sample = 255 - pgm_read_byte(&(WAVEFORM_SYMBOLES_8Bit[phase_x])); // Triangle and Random Symboles
				}
				else if (count == 3) {
					sample = pgm_read_word(&(PARABOLIC_WAVE[phase_x])) >> 8; // Parabolic (half sine) Symbole
				}
				else {
					sample = pgm_read_byte(&(WAVEFORM_SYMBOLES_8Bit[phase_x])); // Triangle and Random Symboles
				}
				int8_t value = (int8_t)sample;
				y2 = (40 + row) - (value / 10 );
				if (i == 0) {
					tft.drawLine(x1 + col,y1,x2 + col,y2, ST7735_BLACK);
				}
				else tft.drawLine(x1 + col,y1,x2 + col,y2, waveColor);
				x1++;
				x2 = x1;
				y1 = y2;
			}
		}
		
		// Sawtooth
		else if (count == 2) {
			tft.drawLine(119, 50, 143,30, waveColor);
			tft.drawLine(120, 50, 143,31, waveColor);
			tft.drawFastVLine(143,30,21, waveColor);
		}
		
		// Regtangle
		else if (count == 4) {
			tft.drawFastHLine(61, 88, 17, waveColor);
			tft.drawFastVLine(78, 68, 21, waveColor);
			tft.drawFastHLine(78, 68, 17, waveColor);
			tft.drawFastVLine(95, 68, 21, waveColor);
		}
		
	}
}

//*************************************************************************
// draw LFO Waveform
//*************************************************************************
FLASHMEM void drawLFOwaveform(uint8_t shapeNo, uint16_t waveColor)
{
	int x1 = 4;
	int x2 = 4;
	int y1 = 50;
	int y2 = 50;
	uint16_t sample = 0;
	// uint8_t WaveBank = 0;
	uint8_t shape_no = 0;
	
	shape_no = (shapeNo / 6) * 6;	// select wave group
	
	tft.fillRect(3,22,152,72,ST7735_BLACK);		// clear Wave Screen
	tft.drawRect(2,22,50,35,ST7735_GRAY);
	tft.drawRect(54,22,50,35,ST7735_GRAY);
	tft.drawRect(106,22,50,35,ST7735_GRAY);
	tft.drawRect(2,60,50,35,ST7735_GRAY);
	tft.drawRect(54,60,50,35,ST7735_GRAY);
	tft.drawRect(106,60,50,35,ST7735_GRAY);
	
	
	
	for (uint8_t i2 = 1; i2 < 3; i2++) {
		x1 = 4 + (i2 * 52);
		x2 = 4 + (i2 * 52);
		
		for (uint8_t i1 = 0; i1 < 3; i1++) {
			x1 = 4 + (i1 * 52);
			x2 = 4 + (i1 * 52);
			if (i2 == 1) {
				y1 = 39;
				y2 = 39;	
			} else {
				y1 = 77;
				y2 = 77;
			}
			
			for (int i = 0; i < 45; i++) {
				int16_t phase_x = i * 5.5f + (256 * shape_no);
				sample = pgm_read_word(&((LFOwaveform)[phase_x]));
				int16_t value = (int16_t)sample;
				if (i2 == 1) {
					y2 = 39 - (value / 2600 );
				} else {
					y2 = 77 - (value / 2600 );
				}
				
				if (i == 0) {
					tft.drawLine(x1,y1,x2,y2, ST7735_BLACK);
				}
				else {
					if (shape_no > 63) {
						waveColor = ST7735_BLACK;
					}
					tft.drawLine(x1,y1,x2,y2, waveColor);
				}
				x1++;
				x2 = x1;
				y1 = y2;
			}
			shape_no++;
		}
	}
	
	if (shapeNo < 6) {
		tft.setFont(&Picopixel);
		tft.setTextColor(ST7735_WHITE);
		print_String(211,142,67);	// print "RND"
		tft.setFont(NULL);
	}
}

//*************************************************************************
// draw selected LFO frame
//*************************************************************************
FLASHMEM void drawLFOframe(uint8_t frameNo)
{
	// clear frames
	tft.drawRect(2,22,50,35,ST7735_GRAY);
	tft.drawRect(54,22,50,35,ST7735_GRAY);
	tft.drawRect(106,22,50,35,ST7735_GRAY);
	tft.drawRect(2,60,50,35,ST7735_GRAY);
	tft.drawRect(54,60,50,35,ST7735_GRAY);
	tft.drawRect(106,60,50,35,ST7735_GRAY);
	
	// draw selected frame
	switch (frameNo) {
		case 0: tft.drawRect(2,22,50,35,ST7735_YELLOW); break;
		case 1: tft.drawRect(54,22,50,35,ST7735_YELLOW); break;
		case 2: tft.drawRect(106,22,50,35,ST7735_YELLOW); break;
		case 3: tft.drawRect(2,60,50,35,ST7735_YELLOW); break;
		case 4: tft.drawRect(54,60,50,35,ST7735_YELLOW); break;
		case 5: tft.drawRect(106,60,50,35,ST7735_YELLOW); break;
	}
}

//*************************************************************************
// draw Sequencer red step frame and step no
//*************************************************************************
FLASHMEM void drawSEQStepFrame(uint8_t SEQselectStepNo)
{
	// clear old Step Marker
	for (uint8_t i = 0; i < 17; i++) {
		tft.drawFastVLine(14 + (i * 9),21+3,76,ST7735_GRAY);
	}
	tft.drawFastHLine(14,21+3,144,ST7735_GRAY);
	tft.drawFastHLine(14,93+3,144,ST7735_GRAY);

	// draw new Step Marker
	tft.drawRect(14 + (9 * SEQselectStepNo),21+3,10,73,ST7735_YELLOW);

	uint8_t Octav = SeqOctavStep[SEQselectStepNo];
	if (Octav <= 6) {
		tft.setFont(NULL);
		tft.setTextSize(0);
		tft.setTextColor(ST7735_BLACK);
		tft.setCursor(2,16);
		tft.fillRect(2,16,12,7,ST7735_GRAY);
		tft.print("C");
		tft.println(Octav);
	}
	
	// print Step No
	if (myPageShiftStatus[PageNr] == false) {
		printDataValue (0, SEQselectStepNo + 1);
		printRedMarker (0, SEQselectStepNo * 8);
	}
}

//*************************************************************************
// draw SEQpitchValue2
//*************************************************************************
FLASHMEM void drawSEQpitchValue2(uint8_t SEQselectStepNo)
{
	uint8_t SeqRecNoteCount = SeqNoteCount[SEQselectStepNo];
	uint16_t color = ST77XX_DARKGREY;
	
	// clear column if 1.note into a Step
	//if (SeqRecNoteCount == 1) {
	if (SEQselectStepNo == 0 || SEQselectStepNo == 4 || SEQselectStepNo == 8 || SEQselectStepNo == 12) {
		color = ST77XX_BLACK;
		} else {
		color = ST77XX_DARKGREY;
	}
	tft.fillRect(15 + (9 * SEQselectStepNo),22+3,8,71,color);
	color = ST7735_GRAY;
	for (uint8_t i = 0; i < 11; i++) {
		tft.drawFastHLine(15 + (9 * SEQselectStepNo),27+3 + (i * 6),8,color);
	}
	//}
	
	// if note mute than color dark_red
	if (SeqNoteBufStatus[SEQselectStepNo] == false) {
		color = ST77XX_DARKRED;
	}
	else {
		// set velo color for note
		uint8_t velo = SeqVeloBuf[SEQselectStepNo];
		color = veloColorTabel[velo];
	}
	// get notes from seq buffer ------------------------------------------
	// uint8_t firstNote = 0;
	for (uint8_t i = 0; i < SeqRecNoteCount; i++) {
		uint8_t note = SeqNote1Buf[(SEQselectStepNo + (i * 16))];
		uint8_t pitch = 12 - (note % 12);
		drawSeqStep(SEQselectStepNo + 1, pitch -1, color);
	}
	
	uint8_t pitchVal = SeqNote1Buf[SEQselectStepNo];
	uint8_t Octav = (pitchVal / 12) -1;
	SeqOctavStep[SEQselectStepNo] = Octav; // save OctavNr for Step (faster for Sequencer routine)
	
	// print Octav
	if (Octav <= 6) {
		tft.setFont(NULL);
		tft.setTextSize(0);
		tft.setTextColor(ST7735_BLACK);
		tft.setCursor(2,16);
		tft.fillRect(2,16,12,7,ST7735_GRAY);
		tft.print("C");
		tft.println(Octav);
	}
}

//*************************************************************************
// print SEQ pitch note
//*************************************************************************
FLASHMEM void printSeqPitchNote (void)
{
	uint8_t seqpitchval = SeqNote1Buf[SEQselectStepNo];
	uint8_t pitchVal = (seqpitchval % 12);
	uint8_t pitchOct = (seqpitchval / 12) -1;
	
	if (pitchOct == 255) {
		pitchOct = 0;
	}
	
	printRedMarker (1, uint8_t((seqpitchval - 36) * 2.4f));
	tft.setCursor(50,116);
	tft.fillRect(50,116,18,8,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	tft.print(NOTES[pitchVal]);
	tft.print(pitchOct);
	
}

//*************************************************************************
// draw SEQ step numbers Marker
//*************************************************************************
FLASHMEM void drawSEQstepNumbersMarker(uint8_t SEQstepNumbers)
{
	uint8_t xpos = 16;
	uint8_t ypos = 18;
	uint16_t color = ST7735_GRAY;
	tft.fillRect(16,18,142,4,color);
	color = ST7735_BLACK;
	tft.drawFastHLine(xpos + (SEQstepNumbers * 9), ypos, 7, color);
	tft.drawFastHLine(xpos + 1 + (SEQstepNumbers * 9), ypos + 1, 5, color);
	tft.drawFastHLine(xpos + 2 + (SEQstepNumbers * 9), ypos + 2, 3, color);
	tft.drawFastHLine(xpos + 3 + (SEQstepNumbers * 9), ypos + 3, 1, color);
}

//*************************************************************************
// draw Sequencer Step (step 1-16, row 1-12)
//*************************************************************************
FLASHMEM void drawSeqStep (uint8_t step_x, uint8_t step_y, uint16_t color)
{
	uint8_t xpos = 15;
	uint8_t ypos = 25;
	step_x -= 1;
	
	tft.fillRect(xpos + (9 * step_x),ypos + (6 * step_y),8,5,color);
}

//*************************************************************************
// Update Osc PRM_A/B modulation
//*************************************************************************
FLASHMEM void update_LFO2_Osc1_PRMA_mod (void) {
	float amount = Lfo2Osc1PrmAAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_A[i].gain(0, amount * 0.5f);
	}
}

FLASHMEM void update_LFO2_Osc1_PRMB_mod (void) {
	float amount = Lfo2Osc1PrmBAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_B[i].gain(0, amount);
	}
}

FLASHMEM void update_LFO2_Osc2_PRMA_mod (void) {
	float amount = Lfo2Osc2PrmAAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_C[i].gain(0, amount);
	}
}

FLASHMEM void update_LFO2_Osc2_PRMB_mod (void) {
	float amount = Lfo2Osc2PrmBAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_D[i].gain(0, amount);
	}
}

FLASHMEM void update_LFO3_Osc1_PRMA_mod (void) {
	float amount = Lfo3Osc1PrmAAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_A[i].gain(1, amount);
	}
}

FLASHMEM void update_LFO3_Osc1_PRMB_mod (void) {
	float amount = Lfo3Osc1PrmBAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_B[i].gain(1, amount);
	}
}

FLASHMEM void update_LFO3_Osc2_PRMA_mod (void) {
	float amount = Lfo3Osc2PrmAAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_C[i].gain(1, amount);
	}
}

FLASHMEM void update_LFO3_Osc2_PRMB_mod (void) {
	float amount = Lfo3Osc2PrmBAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_D[i].gain(1, amount);
	}
}

FLASHMEM void update_filterEnv_Osc1_PRMA_mod (void) {
	float amount = filterEnvOsc1PrmAAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_A[i].gain(2, (amount * 0.5f));
	}
}

FLASHMEM void update_filterEnv_Osc1_PRMB_mod (void) {
	float amount = filterEnvOsc1PrmBAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_B[i].gain(2, (amount * 0.5f));
	}
}

FLASHMEM void update_filterEnv_Osc2_PRMA_mod (void) {
	float amount = filterEnvOsc2PrmAAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_C[i].gain(2, (amount * 0.5f));
	}
}

FLASHMEM void update_filterEnv_Osc2_PRMB_mod (void) {
	float amount = filterEnvOsc2PrmBAmt * DIV127;
	for (size_t i = 0; i < 8; i++)
	{
		Osc_Prm_mixer_D[i].gain(2, (amount * 0.5f));
	}
}

//*************************************************************************
// render ModMatrix Parameter
//*************************************************************************
FLASHMEM void renderModParameter(uint8_t ParameterNr, uint8_t value)
{
	uint8_t xpos;
	uint8_t parNo = (ParameterNr % 4);
	tft.setTextColor(ST7735_WHITE);
	tft.fillRoundRect(70,38+(19 * parNo),22,10,2,ST7735_BLUE);
	
	if (value < 10) {
		xpos = 78;
	}
	else if (value < 100) {
		xpos = 76;
	}
	else xpos = 72;
	
	if (ParameterNr == 4) {
		xpos = 72;
		tft.setCursor(xpos,40 + (19 * parNo));
		value = value >> 5;
		if (value == 0) {
			tft.print("SIN");
			PWMaShape = 0;
		}
		else if (value == 1) {
			tft.print("TRI");
			PWMaShape = 1;
		}
		else if (value == 2) {
			tft.print("SAW");
			PWMaShape = 2;
		}
		else {
			tft.print("SQR");
			PWMaShape = 3;
		}
	}
	else if (ParameterNr == 5) {
		xpos = 72;
		tft.setCursor(xpos,40 + (19 * parNo));
		value = value >> 5;
		if (value == 0) {
			tft.print("SIN");
			PWMbShape = 0;
		}
		else if (value == 1) {
			tft.print("TRI");
			PWMbShape = 1;
		}
		else if (value == 2) {
			tft.print("SAW");
			PWMbShape = 2;
		}
		else {
			tft.print("SQR");
			PWMbShape = 3;
		}
	}
	else {
		tft.setCursor(xpos,40 + (19 * parNo));
		tft.print(value);
	}
	
	
	if (ParameterNr == 0) {		// LFO1 > Pitch
		myControlChange(midiChannel, myLFO1amt, value);
	}
	else if (ParameterNr == 1) {	// LFO2 > Cutoff
		myControlChange(midiChannel, myLFO2amt, value);
	}
	else if (ParameterNr == 2) {	// LFO1*MW > Pitch
		MODWheelvalue = value;
		MODWheelAmt = float(value / 127.0f);
		value = (last_modwheel_value * MODWheelAmt);
		ccModwheelAmt = POWER[value] * modWheelDepth; //Variable LFO amount from mod wheel - Settings Option
		//updateModWheel();
		updateOscLFOAmt();
	}
	else if (ParameterNr == 3) {	// PBEND > Pitch
		PitchWheelvalue = value;
		PitchWheelAmt = float(value / 127.0f);
	}
	else if (ParameterNr == 4) {	// PWMA > OSC1
		if (PWMaShape == 3) {
			pwmLfoA.offset(1.0f);
		} else pwmLfoA.offset(0.0f);
		pwmLfoA.begin(PWMWAVEFORMA + PWMaShape);
	}
	else if (ParameterNr == 5) {	// PWMB > OSC2
		if (PWMbShape == 3) {
			pwmLfoB.offset(1.0f);
		} else pwmLfoB.offset(0.0f);
		pwmLfoB.begin(PWMWAVEFORMB + PWMbShape);
	}
	else if (ParameterNr == 6) {	// OSC1 > Cutoff
		filterFM = (DIV127 * value);
		updateFilterFM();
	}
	else if (ParameterNr == 7) {	// OSC2 > Cutoff
		filterFM2 = (DIV127 * value);
		updateFilterFM2();
	}
	else if (ParameterNr == 8) {	// LFO3 > VCA
		Lfo3amt = (DIV127 * value);
		updateLFO3amt();
	}
	else if (ParameterNr == 9) {	// LFO3 > FxPot1amt
		FxPot1amt = value;
	}
	else if (ParameterNr == 10) {	// LFO3 > FxPot2amt
		FxPot2amt = value;
	}
	else if (ParameterNr == 11) {	// LFO3 > FxPot3amt
		FxPot3amt = value;
	}
	else if (ParameterNr == 12) {	// MODWHEEL > Cutoff
		 ModWheelCutoffAmt = (DIV127 * value);
		 value = (last_modwheel_value * ModWheelCutoffAmt);
		 ccModwheelCutoffAmt = POWER[value] * ModWheelCutoffAmt; // Variable Cutoff amount from mod wheel - Settings Option
		 updateModWheelCutoff();
	}
	else if (ParameterNr == 13) {	// LFO3 > FxMIXamt
		FxMIXamt = value;
	}
	else if (ParameterNr == 14) {	// Atouch > Cutoff
		AtouchAmt = value;
		updateAtouchCutoff();
	}
	else if (ParameterNr == 15) {	// Atouch > Pitch
		AtouchPitchAmt = value;
	}
	else if (ParameterNr == 16) {	// Atouch > LFO1 Amt
		AtouchLFO1Amt = value;
	}
	else if (ParameterNr == 17) {	// Atouch > LFO2 Amt
		AtouchLFO2Amt = value;
		float val = DIV127 * AtouchLFO2Amt;
		AtouchLFO2amt = POWER[AfterTouchValue] * val;
		updateFilterLfoAmt();
	}
	else if (ParameterNr == 18) {	// Atouch > FxP1 Amt
		AtouchFxP1Amt = value;
	}
	else if (ParameterNr == 19) {	// Atouch > FxP2 Amt
		AtouchFxP2Amt = value;
	}
	else if (ParameterNr == 20) {	// Atouch > FxP3 Amt
		AtouchFxP3Amt = value;
	}
	else if (ParameterNr == 21) {	// MWHEEL < HPF
		ModWheelHPFamt = float(value / 127.0f);
		ccModwheelHPFamt = (last_modwheel_value * ModWheelHPFamt) * 2;
		updateHPFFilterFreq();
	}
	else if (ParameterNr == 22) {	// Atouch > FxMix Amt
		AtouchFxMixAmt = value;
	}
	else if (ParameterNr == 23) {	// 
		//  = value;
	}
	else if (ParameterNr == 24) {	// LFO2 > Osc1 PRM_A Amt
		Lfo2Osc1PrmAAmt = value;
		update_LFO2_Osc1_PRMA_mod();
	}
	else if (ParameterNr == 25) {	// LFO2 > Osc1 PRM_B Amt
		Lfo2Osc1PrmBAmt = value;
		update_LFO2_Osc1_PRMB_mod();
	}
	else if (ParameterNr == 26) {	// LFO2 > Osc2 PRM_A Amt
		Lfo2Osc2PrmAAmt = value;
		update_LFO2_Osc2_PRMA_mod();
	}
	else if (ParameterNr == 27) {	// LFO2 > Osc2 PRM_B Amt
		Lfo2Osc2PrmBAmt = value;
		update_LFO2_Osc2_PRMB_mod();
	}
	else if (ParameterNr == 28) {	// LFO3 > Osc1 PRM_A Amt
		Lfo3Osc1PrmAAmt = value;
		update_LFO3_Osc1_PRMA_mod();
	}
	else if (ParameterNr == 29) {	// LFO3 > Osc1 PRM_B Amt
		Lfo3Osc1PrmBAmt = value;
		update_LFO3_Osc1_PRMB_mod();
	}
	else if (ParameterNr == 30) {	// LFO3 > Osc2 PRM_A Amt
		Lfo3Osc2PrmAAmt = value;
		update_LFO3_Osc2_PRMA_mod();
	}
	else if (ParameterNr == 31) {	// LFO3 > Osc2 PRM_B Amt
		Lfo3Osc2PrmBAmt = value;
		update_LFO3_Osc2_PRMB_mod();
	}
	else if (ParameterNr == 32) {	// Filter Envelope > Osc1 PRM_A Amt
		filterEnvOsc1PrmAAmt = value;
		update_filterEnv_Osc1_PRMA_mod();
	}
	else if (ParameterNr == 33) {	// Filter Envelope > Osc1 PRM_B Amt
		filterEnvOsc1PrmBAmt = value;
		update_filterEnv_Osc1_PRMB_mod();
	}
	else if (ParameterNr == 34) {	// Filter Envelope > Osc2 PRM_A Amt
		filterEnvOsc2PrmAAmt = value;
		update_filterEnv_Osc2_PRMA_mod();
	}
	else if (ParameterNr == 35) {	// Filter Envelope > Osc2 PRM_B Amt
		filterEnvOsc2PrmBAmt = value;
		update_filterEnv_Osc2_PRMB_mod();
	}										
}

//*************************************************************************
// read ModMatrix Parameter
//*************************************************************************
FLASHMEM uint8_t readModParameter(uint8_t ParameterNr)
{
	uint8_t value;
	
	if (ParameterNr == 0) {	// LFO1 > Pitch Amt
		float LFOamt = (oscLfoAmt * FILTERMODMIXERMAX);
		value = 0;
		for (int i = 0; i < 128; i++) {
			float LFOoscAmt = LINEARLFO[i];
			if (LFOamt <= LFOoscAmt){
				value = i;
				break;
			}
		}
		return value;
	}
	else if (ParameterNr == 1) {	// LFO2amt > Cutoff Amt
		// LFO AMT
		float LFOamt = (filterLfoAmt * FILTERMODMIXERMAX);
		value = 0;
		for (int i = 0; i < 128; i++) {
			float LFOfilterAmt = LINEARLFO[i];
			if (LFOamt <= LFOfilterAmt){
				value = i;
				break;
			}
		}
		return value;
	}
	else if (ParameterNr == 2) {	// LFO1*MW > Pitch
		MODWheelvalue = round(MODWheelAmt * 127);
		value = MODWheelvalue;
		return value;
	}
	else if (ParameterNr == 3) {	// PWHEEL > Pitch
		PitchWheelvalue = round(PitchWheelAmt * 127);
		value = PitchWheelvalue;
		return value;
	}
	else if (ParameterNr == 4) {	// PWMA > OSC1
		value = PWMaShape << 5;
		return value;
	}
	else if (ParameterNr == 5) {	// PWMB > OSC2
		value = PWMbShape << 5;
		return value;
	}
	else if (ParameterNr == 6) {	// OSC1 > Cutoff
		value = round(filterFM * 127);
		return value;
	}
	else if (ParameterNr == 7) {	// OSC2 > Cutoff
		value = round(filterFM2 * 127);
		return value;
	}
	
	else if (ParameterNr == 8) {	// LFO3 > AMT
		value = round(Lfo3amt * 127);
		return value;
	}
	
	else if (ParameterNr == 9) {	// LFO3 > FxPot1amt
		value = FxPot1amt;// / DIV127;
		return value;
	}
	else if (ParameterNr == 10) {	// LFO3 > FxPot2amt
		value = FxPot2amt;// / DIV127;
		return value;
	}
	else if (ParameterNr == 11) {	// LFO3 > FxPot3amt
		value = FxPot3amt;// / DIV127;
		return value;
	}
	else if (ParameterNr == 12) {	// MODWHEEL > Cutoff
		value = round(ModWheelCutoffAmt * 127);
		return value;
	}
	else if (ParameterNr == 13) {	// LFO3 > FxMIX
		value = FxMIXamt;// / DIV127;
		return value;
	}
	else if (ParameterNr == 14) {	// Atouch > Cutoff
		value = AtouchAmt;
		return value;
	}
	else if (ParameterNr == 15) {	// Atouch > Pitch
		value = AtouchPitchAmt;
		return value;
	}
	else if (ParameterNr == 16) {	// Atouch > LFO1
		value = AtouchLFO1Amt;
		return value;
	}
	else if (ParameterNr == 17) {	// Atouch > LFO2
		value = AtouchLFO2Amt;
		return value;
	}
	else if (ParameterNr == 18) {	// Atouch > FxP1
		value = AtouchFxP1Amt;
		return value;
	}
	else if (ParameterNr == 19) {	// Atouch > FxP2
		value = AtouchFxP2Amt;
		return value;
	}
	else if (ParameterNr == 20) {	// Atouch > FxP3
		value = AtouchFxP3Amt;
		return value;
	}
	else if (ParameterNr == 21) {	// MWHEEL > HPF
		value = round(ModWheelHPFamt * 127);
		return value;
	}
	else if (ParameterNr == 22) {	// Atouch > FxMix
		value = AtouchFxMixAmt;
		return value;
	}
	else if (ParameterNr == 23) {	// 
		value = 0;
		return value;
	}
	else if (ParameterNr == 24) {	// LFO2 > Osc1 PRM_A
		value = Lfo2Osc1PrmAAmt;
		return value;
	}
	else if (ParameterNr == 25) {	// LFO2 > Osc1 PRM_B
		value = Lfo2Osc1PrmBAmt;
		return value;
	}
	else if (ParameterNr == 26) {	// LFO2 > Osc2 PRM_A
		value = Lfo2Osc2PrmAAmt;
		return value;
	}
	else if (ParameterNr == 27) {	// LFO2 > Osc2 PRM_B
		value = Lfo2Osc2PrmBAmt;
		return value;
	}
	else if (ParameterNr == 28) {	// LFO3 > Osc1 PRM_A
		value = Lfo3Osc1PrmAAmt;
		return value;
	}
	else if (ParameterNr == 29) {	// LFO3 > Osc1 PRM_B
		value = Lfo3Osc1PrmBAmt;
		return value;
	}
	else if (ParameterNr == 30) {	// LFO3 > Osc2 PRM_A
		value = Lfo3Osc2PrmAAmt;
		return value;
	}
	else if (ParameterNr == 31) {	// LFO3 > Osc2 PRM_B
		value = Lfo3Osc2PrmBAmt;
		return value;
	}
	else if (ParameterNr == 32) {	// filterEnvelope > Osc1 PRM_A
		value = filterEnvOsc1PrmAAmt;
		return value;
	}
	else if (ParameterNr == 33) {	// filterEnvelope > Osc1 PRM_B
		value = filterEnvOsc1PrmBAmt;
		return value;
	}
	else if (ParameterNr == 34) {	// filterEnvelope > Osc2 PRM_A
		value = filterEnvOsc2PrmAAmt;
		return value;
	}
	else if (ParameterNr == 35) {	// filterEnvelope > Osc2 PRM_B
		value = filterEnvOsc2PrmBAmt;
		return value;
	}
		
	else return 0;
	
}

//*************************************************************************
// render current Parameter
//*************************************************************************
FLASHMEM void renderCurrentParameter(uint8_t Page,uint16_t ParameterNr, uint8_t value)
{
	// Page:0 Main --------------------------------------------------------
	if (Page == 0){
		if (ParameterNr == setPatchBank){
			// set Patchbank
			if (value >= 15) {
				value = 15;
			}
			if (currentPatchBank != value) {
				currentPatchBank = value;
				Keylock = true;
				recallPatch(patchNo);
				Keylock = false;
				mute_before_load_patch();
				currentPatchName = patchName;
				RefreshMainScreenFlag = true;
			}
		}
	}
	// Page:1 Osc1 --------------------------------------------------------
	if (Page == 1){
		// Osc1 Sub page
		if (myPageShiftStatus[PageNr] == true) {						
			// SYNC
			if (ParameterNr == myOscSync) {
				tft.fillRoundRect(54,57,22,10,2,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				value = value >> 3;
				if (value < 1 ) {
					oscDetuneSync = 0;
					print_String(18,56,59);		// print "OFF"
					} else {
					oscDetuneSync = 1;
					print_String(33,59,59);		// print "ON"
				}
			}
			// Transpose (max -6% NoteFrq)
			else if (ParameterNr == myTranspose) {
				tft.fillRect(54,77,22,8,ST7735_BLUE);
				tft.setCursor(56,78);
				tft.setTextColor(ST7735_WHITE);
				int temp = getTranspose(value);
				if (temp != oscTranspose) {
					oscTranspose = temp;
					updatesAllVoices();
				}
				if (temp > 0) {
					tft.print("+");
				}
				tft.print(temp);
			}
			// myMasterTune (max +-6% NoteFrq)
			else if (ParameterNr == myMasterTune) {
				float mtvalue = MASTERTUNE[value];
				if (mtvalue != oscMasterTune) {
					oscMasterTune = mtvalue;
					updatesAllVoices();
				}
				tft.fillRect(54,96,22,8,ST7735_BLUE);
				tft.setCursor(56,97);
				tft.setTextColor(ST7735_WHITE);
				if (value >= 64) {
					value--;
				}
				if (value >63) {
					tft.print("+");
				}
				tft.print(value - 63);
			}
			
			// OSCMOD (CCoscfx)
			else if (ParameterNr == CCoscfx){
				tft.fillRoundRect(133,77,21,8,2,ST7735_BLUE);
				tft.setCursor(135,78);
				tft.setTextColor(ST7735_WHITE);
				uint8_t val = (value * 0.056);
				if (val <= 6) {
					print_String((val + 18),135,78);	// print OscFx Mode 0-6
				} else {
					print_String(247,135,78);			// print OscFx Mode 7
				}
			}
			
			// Osc1ModAmt
			else if (ParameterNr == myOsc1ModAmt) {
				if (Osc1ModAmt != value) {
					Osc1ModAmt = (DIV127 * value);
					//updateOscVCFMOD();
					updateOscFX();
					tft.fillRoundRect(133,96,21,8,2,ST7735_BLUE);
					tft.setCursor(135,97);
					tft.setTextColor(ST7735_WHITE);
					tft.print(value);
				}
			}
		}
	}
	
	// Page:2 Osc2 --------------------------------------------------------
	if (Page == 2){
		// SUB page
		if (myPageShiftStatus[PageNr] == true) {
			if (ParameterNr == myWaveshaperTable){
				uint8_t val = (uint8_t)(value / 9);
				if (val >= 13) {
					val = 13;
				}
				if (WShaperNo != val) {
					WShaperNo = val;
					setWaveShaperTable(val);
					tft.fillRoundRect(54,19,22,10,2,ST7735_BLUE);
					tft.setCursor(56,21);
					tft.setTextColor(ST7735_WHITE);
					if (WShaperNo == 0) {
						tft.print("OFF");
						WShaperDrive = 1.0f;
						val = 1.0f;
						setWaveShaperDrive(val);
						tft.fillRect(56,40,17,7,ST7735_BLUE);
						tft.setCursor(56, 40);
						tft.print("1.0");
					}
					else tft.print(val,1);
					drawWaveshaperCurve(WShaperNo);
				}
			}
			// Waveshaper gain
			if (ParameterNr == myWaveshaperDrive) {
				float valmax = 5.0f;
				float val = 0;
				val = ((valmax / 127.0f) * value);
				if (val <= 0.1f) {
					val = 0.1f;
				}
				if (WShaperNo == 0) {
					val = 1.0f;
				}
				WShaperDrive = val;
				setWaveShaperDrive(val);
				tft.fillRect(56,40,17,7,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				tft.setCursor(56, 40);
				tft.print(val,1);
			}
			// SYNC
			if (ParameterNr == myOscSync) {
				tft.fillRoundRect(54,57,22,10,2,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				value = value >> 3;
				if (value < 3 ) {
					oscDetuneSync = false;
					tft.setCursor(56,59);
					tft.print("OFF");
					} else {
					oscDetuneSync = true;
					tft.setCursor(59,59);
					tft.print("ON");
				}
			}
			// Transpose (max -6% NoteFrq)
			else if (ParameterNr == myTranspose) {
				tft.fillRect(54,78,22,10,ST7735_BLUE);
				tft.setCursor(56,78);
				tft.setTextColor(ST7735_WHITE);
				int temp = getTranspose(value);
				if (temp != oscTranspose) {
					oscTranspose = temp;
					updatesAllVoices();
				}
				if (temp > 0) {
					tft.print("+");
				}
				tft.print(temp);
			}
			
			// myMasterTune (max +-6% NoteFrq)
			else if (ParameterNr == myMasterTune) {
				float mtvalue = MASTERTUNE[value];
				if (mtvalue != oscMasterTune) {
					oscMasterTune = mtvalue;
					updatesAllVoices();
				}
				tft.fillRect(54,96,22,10,ST7735_BLUE);
				tft.setCursor(56,97);
				tft.setTextColor(ST7735_WHITE);
				if (value >= 64) {
					value--;
				}
				if (value >63) {
					tft.print("+");
				}
				tft.print(value - 63);
			}
			
			// OSCMOD (CCoscfx)
			else if (ParameterNr == CCoscfx){
				tft.fillRoundRect(133,78,21,8,2,ST7735_BLUE);
				tft.setCursor(135,78);
				tft.setTextColor(ST7735_WHITE);
				uint8_t val = (value * 0.056);
				if (val <= 6) {
					print_String((val + 18),135,78);	// print OscFx Mode 0-6
					} else {
					print_String(247,135,78);			// print OscFx Mode 7
				}
			}
			
			// Osc1ModAmt
			else if (ParameterNr == myOsc1ModAmt) {
				if (Osc1ModAmt != value) {
					Osc1ModAmt = (DIV127 * value);
					//updateOscVCFMOD();
					updateOscFX();
					tft.fillRoundRect(133,97,21,8,2,ST7735_BLUE);
					tft.setCursor(135,97);
					tft.setTextColor(ST7735_WHITE);
					tft.print(value);
				}
			}
		}
	}
	
	// Page:3 Filter ------------------------------------------------------
	else if (Page == 3){
		
		
		if (myPageShiftStatus[PageNr] == true) {
			// Keytracking
			if (ParameterNr == CCkeytracking){
				printRedMarker (0, value);
				printPercentValue(0,(DIV100 * value));
			}
			// Velocity
			else if (ParameterNr == myFilterVelo){
				if (myFilter == 1) {
					myFilVelocity = (DIV127 * value) * 1.5f;	// State Variable Filter
				}
				else {
					myFilVelocity = (DIV127 * value) * 4.5f;	// Ladder Filter
				}
				printRedMarker (1, value);
				printPercentValue(1, (DIV100 * value));
			}
			
			// LFO2 AMT
			else if (ParameterNr == myLFO2amt) {
				printDataValue (2, (value >> 1));
				printRedMarker (2, (value >> 1));
				filterLfoAmt = LINEARLFO[value >> 1];
				updateFilterLfoAmt();
			}
		}
		
	}
	
	// Page:4 Filter Envelope --------------------------------------------
	else if (Page == 4){
		
		if (myPageShiftStatus[PageNr] == true) {
			// Amp velocity
			if (ParameterNr == myFilterVelo){
				printDataValue (1, value >> 1);
				printRedMarker (1, value >> 1);
			}
			// AMT
			else if (ParameterNr == myLFO2amt) {
				printDataValue (3, (value >> 1));
				printRedMarker (3, (value >> 1));
				filterLfoAmt = LINEARLFO[value >> 1];
				updateFilterLfoAmt();
			}
			// CUR	Envelope curve
			else if (ParameterNr == myFilterEnvCurve){
				printRedMarker (0, value);
				value = (value / 7.48f);
				if (value < 8) {
					value = 8 - value;
					envelopeType1 = -value;
					updateFilterEnvelopeType();
					drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
					tft.setCursor(13,116);
					tft.fillRect(13,116,20,8,ST7735_BLACK);
					tft.setCursor(13,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("-");
					tft.print(value);
				}
				else if (value == 8) {
					envelopeType1 = 0;
					updateFilterEnvelopeType();
					drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
					tft.setCursor(13,116);
					tft.fillRect(13,116,20,8,ST7735_BLACK);
					tft.setCursor(15,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("0");
				}
				else if (value > 8) {
					value = (value - 8);
					envelopeType1 = value;
					updateFilterEnvelopeType();
					drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
					tft.setCursor(13,116);
					tft.fillRect(13,116,20,8,ST7735_BLACK);
					tft.setCursor(13,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("+");
					tft.print(value);
				}
			}
		}
	}
	
	// Page:5 AMP Envelope ----------------------------------------------
	else if (Page == 5){
		
		// SUB Menu
		if (myPageShiftStatus[PageNr] == true) {
			if (ParameterNr == myAmpEnvCurve){
				printRedMarker (0, value);
				value = (value / 7.48f);
				if (value < 8) {
					value = 8 - value;
					envelopeType2 = -value;
					updateAmpEnvelopeType();
					drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
					tft.setCursor(13,116);
					tft.fillRect(13,116,20,8,ST7735_BLACK);
					tft.setCursor(13,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("-");
					tft.print(value);
				}
				else if (value == 8) {
					envelopeType2 = 0;
					updateAmpEnvelopeType();
					drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
					tft.setCursor(13,116);
					tft.fillRect(13,116,20,8,ST7735_BLACK);
					tft.setCursor(15,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("0");
				}
				else if (value > 8) {
					value = (value - 8);
					envelopeType2 = value;
					updateAmpEnvelopeType();
					drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
					tft.setCursor(13,116);
					tft.fillRect(13,116,20,8,ST7735_BLACK);
					tft.setCursor(13,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("+");
					tft.print(value);
				}
			}
			// Amp velocity
			if (ParameterNr == myAmplifierVelo){
				printRedMarker (1, value);
				value = (value * DIV100);
				printPercentValue (1, value);
			}
		}
	}
	
	// Page:6 LFO1 --------------------------------------------------------
	else if (Page == 6) {
		// LFO1 main menu
		if (myPageShiftStatus[PageNr] == false) {			
			// SYN
			if (ParameterNr == myLFO1syn) {
				value = value >> 6;
				tft.fillRect(122, 116, 30, 7, ST7735_BLACK);
				tft.setTextColor(ST7735_GRAY);
				if (value == 0) {
					tft.setCursor(130, 116);
					tft.print("OFF");
					printRedMarker (3, 0);
					oscLfoRetrig = 0;
					pitchLfo.LFO_phase(oscLfoRetrig);
					if (LFO1mode == 0) {
						lfo1oneShoot = false;
						pitchLfo.LFO_oneShoot(lfo1oneShoot);
						} else {
						lfo1oneShoot = true;
						pitchLfo.LFO_oneShoot(lfo1oneShoot);
					}
				}
				else if (value == 1 ) {
					tft.setCursor(134, 116);
					tft.print("0");
					tft.drawRect(141,116,3,3,ST7735_GRAY);
					printRedMarker (3, 63);
					oscLfoRetrig = 1;
					pitchLfo.LFO_phase(oscLfoRetrig);
					if (LFO1mode == 0) {
						lfo1oneShoot = false;
						pitchLfo.LFO_oneShoot(lfo1oneShoot);
						} else {
						lfo1oneShoot = true;
						pitchLfo.LFO_oneShoot(lfo1oneShoot);
					}
					lfo1ph = 0;
					LFO1phase = 0;
				}
				else {
					tft.setCursor(128, 116);
					tft.print("180");
					tft.drawRect(147,116,3,3,ST7735_GRAY);
					printRedMarker (3, 127);
					oscLfoRetrig = 2;
					pitchLfo.LFO_phase(oscLfoRetrig);
					if (LFO1mode == 0) {
						lfo1oneShoot = false;
						pitchLfo.LFO_oneShoot(lfo1oneShoot);
						} else {
						lfo1oneShoot = true;
						pitchLfo.LFO_oneShoot(lfo1oneShoot);
					}
					lfo1ph = 180;
					LFO1phase = 180;
				}
			}
		}
		// LFO1 sub menu
		else {
			// MODE
			if (ParameterNr == myLFO1mode) {
				value = value >> 7;
				if (value != LFO1mode) {
					tft.fillRect(5, 116, 25, 7, ST7735_BLACK);
					tft.setTextColor(ST7735_GRAY);
					if (value == 0) {
						LFO1mode = 0;
						pitchLfo.LFO_mode(LFO1mode);
						lfo1oneShoot = false;
						pitchLfo.LFO_oneShoot(lfo1oneShoot);
						print_String(170,6,116);		// print "CONT"
						printRedMarker (0, 0);
					}
					else {
						LFO1mode = 1;
						pitchLfo.LFO_mode(LFO1mode);
						lfo1oneShoot = true;
						pitchLfo.LFO_oneShoot(lfo1oneShoot);
						print_String(171,8,116);		// print "ONE"
						printRedMarker (0, 127);
					}
				}
				
			}
			
			// FADE-IN
			else if (ParameterNr == myLFO1fade) {
				LFO1fadeTime = (uint32_t)(value * 94);
				updateLFO1fade();
				updateOscLFOAmt();
				tft.fillRect(43,116,29,8,ST7735_BLACK);
				tft.setTextColor(ST7735_GRAY);
				uint8_t xpos = 50;
				uint8_t ypos = 116;
				printEnvValue (value, xpos, ypos);
				printRedMarker(1,value);
			}
			
			// FADE-OUT
			else if (ParameterNr == myLFO1fadeOut) {
				LFO1releaseTime = (uint32_t)(value * 94);
				updateLFO1release();
				updateOscLFOAmt();
				tft.fillRect(83,116,29,8,ST7735_BLACK);
				tft.setTextColor(ST7735_GRAY);
				uint8_t xpos = 90;
				uint8_t ypos = 116;
				printEnvValue (value, xpos, ypos);
				printRedMarker(2,value);
			}
			
			// EnvCurve
			else if (ParameterNr == myLFO1envCurve){
				printRedMarker (3, value);
				value = (value / 7.48f);
				if (value < 8) {
					value = 8 - value;
					LFO1envCurve = -value;
					updateLFO1EnvelopeType();
					tft.fillRect(131,116,20,8,ST7735_BLACK);
					tft.setCursor(133,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("-");
					tft.print(value);
				}
				else if (value == 8) {
					LFO1envCurve = 0;
					updateLFO1EnvelopeType();
					tft.fillRect(131,116,20,8,ST7735_BLACK);
					tft.setCursor(135,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("0");
				}
				else if (value > 8) {
					value = (value - 8);
					LFO1envCurve = value;
					updateLFO1EnvelopeType();
					tft.fillRect(131,116,20,8,ST7735_BLACK);
					tft.setCursor(133,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("+");
					tft.print(value);
				}
			}
		}
	}
	
	// Page:7 LFO2 --------------------------------------------------------
	else if (Page == 7) {
		// LFO2 main menu
		if (myPageShiftStatus[PageNr] == false) {
			
			// SYN
			if (ParameterNr == myLFO2syn) {
				value = value >> 6;
				tft.fillRect(122, 116, 30, 7, ST7735_BLACK);
				tft.setTextColor(ST7735_GRAY);
				if (value == 0) {
					tft.setCursor(130, 116);
					tft.print("OFF");
					printRedMarker (3, 0);
					filterLfoRetrig = 0;
					filterLfo.LFO_phase(filterLfoRetrig);
					if (LFO2mode == 0) {
						lfo2oneShoot = false;
						filterLfo.LFO_oneShoot(lfo2oneShoot);
						} else {
						lfo2oneShoot = true;
						filterLfo.LFO_oneShoot(lfo2oneShoot);
					}
				}
				else if (value == 1 ) {
					tft.setCursor(134, 116);
					tft.print("0");
					tft.drawRect(141,116,3,3,ST7735_GRAY);
					printRedMarker (3, 63);
					filterLfoRetrig = 1;
					filterLfo.LFO_phase(filterLfoRetrig);
					if (LFO2mode == 0) {
						lfo2oneShoot = false;
						filterLfo.LFO_oneShoot(lfo2oneShoot);
						} else {
						lfo2oneShoot = true;
						filterLfo.LFO_oneShoot(lfo2oneShoot);
					}
					lfo2ph = 0;
					LFO2phase = 0;
				}
				else {
					tft.setCursor(128, 116);
					tft.print("180");
					tft.drawRect(147,116,3,3,ST7735_GRAY);
					printRedMarker (3, 127);
					filterLfoRetrig = 2;
					filterLfo.LFO_phase(filterLfoRetrig);
					if (LFO2mode == 0) {
						lfo2oneShoot = false;
						filterLfo.LFO_oneShoot(lfo2oneShoot);
						} else {
						lfo2oneShoot = true;
						filterLfo.LFO_oneShoot(lfo2oneShoot);
					}
					lfo2ph = 180;
					LFO2phase = 180;
				}
			}
		}
		
		// LFO2 sub menu -------------
		else {
			// MODE
			if (ParameterNr == myLFO2mode) {
				value = value >> 7;
				if (value != LFO2mode) {
					tft.fillRect(5, 116, 25, 7, ST7735_BLACK);
					tft.setTextColor(ST7735_GRAY);
					if (value == 0) {
						LFO2mode = 0;
						filterLfo.LFO_mode(LFO2mode);
						lfo2oneShoot = false;
						filterLfo.LFO_oneShoot(lfo2oneShoot);
						print_String(170,6,116);		// print "CONT"
						printRedMarker (0, 0);
					}
					else {
						LFO2mode = 1;
						filterLfo.LFO_mode(LFO2mode);
						lfo2oneShoot = true;
						filterLfo.LFO_oneShoot(lfo2oneShoot);
						print_String(171,8,116);		// print "ONE"
						printRedMarker (0, 127);
					}
				}
				
			}
			
			// FADE-IN
			else if (ParameterNr == myLFO2fade) {
				LFO2fadeTime = (uint32_t)(value * 94);
				updateLFO2fade();
				updateFilterLfoAmt();
				tft.fillRect(43,116,29,8,ST7735_BLACK);
				tft.setTextColor(ST7735_GRAY);
				uint8_t xpos = 50;
				uint8_t ypos = 116;
				printEnvValue (value, xpos, ypos);
				printRedMarker(1,value);
			}
			
			// FADE-OUT
			else if (ParameterNr == myLFO2fadeOut) {
				LFO2releaseTime = (uint32_t)(value * 94);
				updateLFO2release();
				updateFilterLfoAmt();
				tft.fillRect(83,116,29,8,ST7735_BLACK);
				tft.setTextColor(ST7735_GRAY);
				uint8_t xpos = 90;
				uint8_t ypos = 116;
				printEnvValue (value, xpos, ypos);
				printRedMarker(2,value);
			}
			
			// EnvCurve
			else if (ParameterNr == myLFO2envCurve){
				printRedMarker (3, value);
				value = (value / 7.48f);
				if (value < 8) {
					value = 8 - value;
					LFO2envCurve = -value;
					updateLFO2EnvelopeType();
					tft.fillRect(131,116,20,8,ST7735_BLACK);
					tft.setCursor(133,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("-");
					tft.print(value);
				}
				else if (value == 8) {
					LFO2envCurve = 0;
					updateLFO2EnvelopeType();
					tft.fillRect(131,116,20,8,ST7735_BLACK);
					tft.setCursor(135,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("0");
				}
				else if (value > 8) {
					value = (value - 8);
					LFO2envCurve = value;
					updateLFO2EnvelopeType();
					tft.fillRect(131,116,20,8,ST7735_BLACK);
					tft.setCursor(133,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("+");
					tft.print(value);
				}
			}
		}
	}
	
	// Page:8 LFO3 --------------------------------------------------------
	else if (Page == 8) {
		// LFO3 main menu
		if (myPageShiftStatus[PageNr] == false) {			
			// SYNC
			if (ParameterNr == myLFO3syn) {
				value = value >> 6;
				tft.fillRect(122, 116, 30, 7, ST7735_BLACK);
				tft.setTextColor(ST7735_GRAY);
				if (value == 0) {
					tft.setCursor(130, 116);
					tft.print("OFF");
					printRedMarker (3, 0);
					Lfo3Retrig = 0;
					ModLfo3.LFO_phase(Lfo3Retrig);
					if (LFO3mode == 0) {
						lfo3oneShoot = false;
						ModLfo3.LFO_oneShoot(lfo3oneShoot);
						} else {
						lfo3oneShoot = true;
						ModLfo3.LFO_oneShoot(lfo3oneShoot);
					}
				}
				else if (value == 1 ) {
					tft.setCursor(134, 116);
					tft.print("0");
					tft.drawRect(141,116,3,3,ST7735_GRAY);
					printRedMarker (3, 63);
					Lfo3Retrig = 1;
					ModLfo3.LFO_phase(Lfo3Retrig);
					if (LFO3mode == 0) {
						lfo3oneShoot = false;
						ModLfo3.LFO_oneShoot(lfo3oneShoot);
						} else {
						lfo3oneShoot = true;
						ModLfo3.LFO_oneShoot(lfo3oneShoot);
					}
					lfo3ph = 0;
					LFO3phase = 0;
				}
				else {
					tft.setCursor(128, 116);
					tft.print("180");
					tft.drawRect(147,116,3,3,ST7735_GRAY);
					printRedMarker (3, 127);
					Lfo3Retrig = 2;
					ModLfo3.LFO_phase(Lfo3Retrig);
					if (LFO3mode == 0) {
						lfo3oneShoot = false;
						ModLfo3.LFO_oneShoot(lfo3oneShoot);
						} else {
						lfo3oneShoot = true;
						ModLfo3.LFO_oneShoot(lfo3oneShoot);
					}
					lfo3ph = 180;
					LFO3phase = 180;
				}
			}
		}

		// LFO3 sub menu -------------
		else { 
			// MODE
			if (ParameterNr == myLFO3mode) {
				value = value >> 7;
				if (value != LFO3mode) {
					tft.fillRect(5, 116, 25, 7, ST7735_BLACK);
					tft.setTextColor(ST7735_GRAY);
					if (value == 0) {
						LFO3mode = 0;
						ModLfo3.LFO_mode(LFO3mode);
						lfo3oneShoot = false;
						ModLfo3.LFO_oneShoot(lfo3oneShoot);
						print_String(170,6,116);		// print "CONT"
						printRedMarker (0, 0);
					}
					else {
						LFO3mode = 1;
						ModLfo3.LFO_mode(LFO3mode);
						lfo3oneShoot = true;
						ModLfo3.LFO_oneShoot(lfo3oneShoot);
						print_String(171,8,116);		// print "ONE"
						printRedMarker (0, 127);
					}
				}
				
			}
			
			// FADE-IN
			else if (ParameterNr == myLFO3fade) {
				LFO3fadeTime = (uint32_t)(value * 94);
				updateLFO3fade();
				updateLFO3amt();
				tft.fillRect(43,116,29,8,ST7735_BLACK);
				tft.setTextColor(ST7735_GRAY);
				uint8_t xpos = 50;
				uint8_t ypos = 116;
				printEnvValue (value, xpos, ypos);
				printRedMarker(1,value);
				
			}
			
			// FADE-OUT
			else if (ParameterNr == myLFO3fadeOut) {
				LFO3releaseTime = (uint32_t)(value * 94);
				updateLFO3release();
				updateLFO3amt();
				tft.fillRect(83,116,29,8,ST7735_BLACK);
				tft.setTextColor(ST7735_GRAY);
				uint8_t xpos = 90;
				uint8_t ypos = 116;
				printEnvValue (value, xpos, ypos);
				printRedMarker(2,value);
			}
			
			// EnvCurve
			else if (ParameterNr == myLFO3envCurve){
				printRedMarker (3, value);
				value = (value / 7.48f);
				if (value < 8) {
					value = 8 - value;
					LFO3envCurve = -value;
					LFO3envCurve = (int)(value);
					updateLFO3EnvelopeType();
					tft.fillRect(131,116,20,8,ST7735_BLACK);
					tft.setCursor(133,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("-");
					tft.print(value);
				}
				else if (value == 8) {
					LFO3envCurve = 0;
					updateLFO3EnvelopeType();
					tft.fillRect(131,116,20,8,ST7735_BLACK);
					tft.setCursor(135,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("0");
				}
				else if (value > 8) {
					value = (value - 8);
					LFO3envCurve = value;
					updateLFO3EnvelopeType();
					tft.fillRect(131,116,20,8,ST7735_BLACK);
					tft.setCursor(133,116);
					tft.setTextColor(ST7735_GRAY);
					tft.print("+");
					tft.print(value);
				}
			}
		}
	}
	
	// Page:10 Fx DSP -----------------------------------------------------
	else if (Page == 10){

			// SEL
			if (ParameterNr == myFxSel && FxPrgNo >= 1){
				uint8_t valx = value >> 5;
				if(valx >= 3) {
					valx = 3;
				}
				myFxSelValue = valx;
				printDataValue (0, (valx) + 1);
				printRedMarker (0, valx * 43);
				printFxValFrame(valx);
				printFxPOT(myFxSelValue, FxPrgNo);
			}
			// VAL
			else if (ParameterNr == myFxVal && FxPrgNo >= 1){
				switch (myFxSelValue) {
					case 0:
					if (FxPot1Val != value) {
						FxPot1Val = value;
						//uint16_t valFx = (AtouchFxP1amt * 2) + FxPot1Val;
						//analogWrite(PWM1, valFx);  // max 8bit value
						FxPot1value = value >> 1;
					}
					break;
					case 1:
					if (FxPot2Val != value) {
						FxPot2Val = value;
						//uint16_t valFx = (AtouchFxP2amt * 2) + FxPot2Val;
						//analogWrite(PWM2, valFx);  // max 8bit value
						FxPot2value = value >> 1;
					}
					break;
					case 2:
					if (FxPot3Val != value) {
						FxPot3Val = value;
						//uint16_t valFx = (AtouchFxP3amt * 2) + FxPot3Val;
						//analogWrite(PWM3, valFx);  // max 8bit value
						FxPot3value = value >> 1;
					}
					break;
					case 3:
					if (FxPot4Val != value) {
						FxPot4Val = value;
						uint16_t valFx = (AtouchFxCLKamt * 2) + FxPot4Val;
						if (valFx >= 255) {
							valFx = 255;
						}
						FxClkRate = (value * 196.08f) + 10000;
						analogWriteFrequency (PWM5, FxClkRate);
						tft.fillRoundRect(63,38,65,11,2,ST7735_GREEN);
						tft.setTextColor(ST7735_BLACK);
						tft.setCursor(65,40);
						tft.print("Clk");
						uint16_t fxRate = (FxClkRate / 1000);
						tft.setCursor(85,40);
						tft.print(fxRate);
						fxRate = (FxClkRate % 1000);
						fxRate = (fxRate / 100);
						tft.setCursor(96,40);
						tft.print(".");
						tft.setCursor(103,40);
						tft.print(fxRate);
						tft.setCursor(110,40);
						tft.print("KHz");					
					}
					break;
				}
				printDataValue (1, value >> 1);
				printRedMarker (1, value >> 1);
				printFxPotValue(myFxSelValue, value >> 1);
				myFxValValue = value >> 1;
			}
			// MIX
			else if (ParameterNr == myFxMix && FxPrgNo >= 1){
				//uint16_t valFx = (AtouchFxMIXamt * 2) + (FxMixValue * 2);
				//analogWrite(PWM4, valFx);  // max 8bit value
				printDataValue (2, value >> 1);
				printRedMarker (2, value >> 1);
				FxMixValue = value >> 1;
				_FxMixValue = FxMixValue; // temp var
				drawFxGrafic (FxMixValue, FxTimeValue, FxFeedbackValue, FxPot3value);
			}
			// PRG
			else if (ParameterNr == myFxPrg){
				if (value != FxPrgNo) {
					if (value == 0) {	// Fx off
						setFxPrg (0);
						_FxMixValue = FxMixValue;
						FxMixValue = 0;	
						analogWrite(PWM4, FxMixValue);
						tft.fillRect(65,27,90,9,ST7735_BLACK);
						tft.setTextColor(ST7735_RED);
						tft.setCursor(65,28);
						tft.print("OFF");
						tft.fillRect(122, 116, 30, 7, ST7735_BLACK);
						tft.setTextColor(ST7735_GRAY);
						tft.setCursor(130,116);
						tft.print("OFF");
						printFxPotValue(0, 0);
						printFxPotValue(1, 0);
						printFxPotValue(2, 0);
						drawFxGrafic (0, 0, 0, 0);
						printDataValue (1, 0);
						printRedMarker (1, 0);
						printDataValue (2, 0);
						printRedMarker (2, 0);
						//printDataValue (3, 0);
						printRedMarker (3, 0);
						//setLED(1, false);
						KeyLED1State = true;
						printFxValFrame(99); // clear green Frame
						tft.fillRect(3,71,155,10,ST7735_BLACK);
						tft.setCursor(75,72);
						tft.setTextColor(ST7735_GRAY);
						tft.print("Time/Feedb.");
						
					}
					else {
						setFxPrg (value);
						FxMixValue = _FxMixValue;
						analogWrite(PWM4, FxMixValue << 1);
						printDataValue (1, myFxValValue);
						printRedMarker (1, myFxValValue);
						printDataValue (2, FxMixValue);
						printRedMarker (2, FxMixValue);
						printFxPotValue(0, FxPot1value);
						printFxPotValue(1, FxPot2value);
						printFxPotValue(2, FxPot3value);
						printDataValue (3, value);
						printRedMarker (3, (value * 8.5));
						printFxName(value);
						drawFxGrafic (FxMixValue, FxTimeValue, FxFeedbackValue, FxPot3value);
						//setLED(1, true);
						KeyLED1State = true;
						printFxValFrame(myFxSelValue); // clear green Frame
						printFxPOT(myFxSelValue, value);
					}
					FxPrgNo = value;
				}
			}
	}
	
	// Page:11 SEQUENCER  --------------------------------------------------
	else if (Page == 11) {
		
		// STEP
		if (ParameterNr == mySEQStep) {
			if (SEQrunStatus == false) {
				// new value ?
				value = value >> 4;
				if (value != SEQselectStepNo) {
					SEQselectStepNo = value;
					drawSEQStepFrame(SEQselectStepNo);
					printDataValue (0, SEQselectStepNo + 1);
					printRedMarker (0, SEQselectStepNo * 8);
					printSeqPitchNote();
					//Interval = 37500;
					//gateTime = 18750;
					Interval = 120000;
					gateTime = 80000;
					SEQrunOneNote = true;
					timer_intMidiClk = micros();
					timer_intMidiClk -= Interval;	// fixed clk-time for editor (160bpm, 1/8)
					SEQrunStatus = true;
					SeqRecNoteCount = 0;
					SeqStepClk = true;			
				}
			}
		}
		
		// Pitch
		if (ParameterNr == mySEQPitch) {
			if (SEQrunStatus == false) {
				//uint8_t val = value >> 1;
				printRedMarker (1, value >> 1);
				value = uint8_t((value / 5.3333f) +36);  // 72 Notes (C2-C)
				if (value != SEQpitchValue) {		// the value has changed ?
					
					SEQpitchValue = value;	// convert value to int
					uint8_t noteCount = SeqNoteCount[SEQselectStepNo]; // Check number of notes
					uint8_t smallestNote = 0;
					
					for (uint8_t i = 0; i < noteCount;i++) {
						smallestNote = SeqNote1Buf[SEQselectStepNo];
						for (uint8_t i = 1; i < noteCount; i++) {
							if (smallestNote > SeqNote1Buf[SEQselectStepNo + (i * 16)]){
								smallestNote = SeqNote1Buf[SEQselectStepNo + (i * 16)];
							}
						}
					}
					SeqSmallestNoteStep = smallestNote; // save smallestNote from Step
					
					// Transpose notes
					for (uint8_t i = 0; i < noteCount; i++) {
						uint8_t NotePitch = SeqNote1Buf[SEQselectStepNo + (i * 16)];
						SeqNote1Buf[SEQselectStepNo + (i * 16)] = (NotePitch - smallestNote) + SEQpitchValue;
					}
					
					// draw notes
					drawSEQpitchValue2(SEQselectStepNo);
					
					// set Note mute status to off
					for (uint8_t i = 0; i < noteCount; i++) {
						SeqNoteBufStatus[SEQselectStepNo + (i * 16)] = 1; // Note mute off
					}
					//Interval = 37500;
					//gateTime = 18750;
					Interval = 120000;
					gateTime = 80000;
					SEQrunOneNote = true;
					timer_intMidiClk = micros();
					timer_intMidiClk -= Interval; // fixed clk-time for editor (160bpm, 1/8)
					SEQrunStatus = true;
				}
				SeqStepClk = true;
			}
		}
		
		// BPM (intern Clocksignal)
		if (ParameterNr == mySEQRate) {
			static uint8_t temp_val = 0;
			if (value != temp_val) {
				temp_val = value;
				if (value > 5) {
					if (SEQMidiClkSwitch == true) {
						SEQrunStatus = false;
					}
					SEQMidiClkSwitch = false;
					uint8_t val = (value - 5) * 0.761f;
					SEQbpmValue = (SeqClkRate[val]);
					float bpm = (SEQbpmValue / SEQdivValue);
					SEQclkRate = float(60000000 / bpm);
					printDataValue (2, (SeqClkRate[val]));
					printRedMarker (2, (value >> 1));
					Interval = SEQclkRate;
					gateTime = (float)(SEQclkRate / SEQGateTime);
					if (gateTime <= 3200) {
						gateTime = 3200;
					}
					} else {	// Midi Clk
					SEQMidiClkSwitch = true;
					SEQbpmValue = 0;
					allNotesOff();
					printRedMarker (2, 0);
					tft.fillRect(79, 116, 30,7, ST7735_BLACK);
					tft.setTextColor(ST7735_GRAY);
					print_String(148, 89, 116);		// print "EXT"
				}
			} 
		}
		
		// DIV (intern Clocksignal)
		if (ParameterNr == mySEQdiv) {
			
			if (SEQMidiClkSwitch == false) {
				//printSEQclkDiv (value);
				value = value >> 1;
				uint8_t val = (value / 10.0f);
				if (val == 0) {
					SEQdivValue = ((1.0f / 2.0f) * 4.0f);	// 1/2
				}
				else if (val == 1) {
					SEQdivValue = ((3.0f / 8.0f) * 4.0f);	// 3/8
				}
				else if (val == 2) {
					SEQdivValue = ((1.0f / 3.0f) * 4.0f);	// 1/3
				}
				else if (val == 3) {
					SEQdivValue = ((1.0f / 4.0f) * 4.0f);	// 1/4
				}
				else if (val == 4) {
					SEQdivValue = ((3.0f / 16.0f) * 4.0f);	// 3/16
				}
				else if (val == 5) {
					SEQdivValue = ((1.0f / 6.0f) * 4.0f);	// 1/6
				}
				else if (val == 6) {
					SEQdivValue = ((1.0f / 8.0f) * 4.0f);	// 1/8
				}
				else if (val == 7) {
					SEQdivValue = ((3.0f / 32.0f) * 4.0f);	// 3/32
				}
				else if (val == 8) {
					SEQdivValue = ((1.0f / 12.0f) * 4.0f);	// 1/12
				}
				else if (val == 9) {
					SEQdivValue = ((1.0f / 16.0f) * 4.0f);	// 1/16
				}
				else if (val == 10) {
					SEQdivValue = ((1.0f / 24.0f) * 4.0f);	// 1/24
				}
				else if (val == 11) {
					SEQdivValue = ((1.0f / 32.0f) * 4.0f);	// 1/32
				}
				else if (val == 12) {
					SEQdivValue = ((1.0f / 48.0f) * 4.0f);	// 1/24
				}
				SEQdividerValue = val;
				float bpm = (SEQbpmValue / SEQdivValue);
				SEQclkRate = float(60000000 / bpm);
				printSEQclkDiv (val);
				printRedMarker (3, value);
				Interval = SEQclkRate;
				gateTime = (float)(SEQclkRate / SEQGateTime);
				if (gateTime <= 3200) {
					gateTime = 3200;
				}
				} else {
				
				printSEQclkDiv (value);
				value = value >> 1;
				uint8_t val = (value / 10.0f);
				if (val == 0) {
					MidiClkDiv = 48;
					
				}
				else if (val == 1) {
					MidiClkDiv = 48;
					SEQdivValue = ((1.0f / 2.0f) * 4.0f);	// 1/2
				}
				else if (val == 2) {
					MidiClkDiv = 48;
					SEQdivValue = ((1.0f / 2.0f) * 4.0f);	// 1/2
				}
				else if (val == 3) {
					MidiClkDiv = 24;
					SEQdivValue = ((1.0f / 4.0f) * 4.0f);	// 1/4
				}
				else if (val == 4) {
					MidiClkDiv = 24;
					SEQdivValue = ((1.0f / 4.0f) * 4.0f);	// 1/4
				}
				else if (val == 5) {
					MidiClkDiv = 24;
					SEQdivValue = ((1.0f / 4.0f) * 4.0f);	// 1/4
				}
				else if (val == 6) {
					MidiClkDiv = 12;
					SEQdivValue = ((1.0f / 8.0f) * 4.0f);	// 1/8
				}
				else if (val == 7) {
					MidiClkDiv = 12;
					SEQdivValue = ((1.0f / 8.0f) * 4.0f);	// 1/8
				}
				else if (val == 8) {
					MidiClkDiv = 12;
					SEQdivValue = ((1.0f / 8.0f) * 4.0f);	// 1/8
				}
				else if (val == 9) {
					MidiClkDiv = 6;
					SEQdivValue = ((1.0f / 16.0f) * 4.0f);	// 1/16
				}
				else if (val == 10) {
					MidiClkDiv = 6;
					SEQdivValue = ((1.0f / 16.0f) * 4.0f);	// 1/16
				}
				else if (val == 11) {
					MidiClkDiv = 6;
					SEQdivValue = ((1.0f / 16.0f) * 4.0f);	// 1/16
				}
				else if (val == 12) {
					MidiClkDiv = 6;
					SEQdivValue = ((1.0f / 16.0f) * 4.0f);	// 1/16
				}
				printSEQclkDiv (val);
				printRedMarker (3, value);
			}
		}
		
		// LEN (No of Steps)
		if (ParameterNr == mySEQLen) {
			uint8_t val = value >> 4;
			if (val != SEQstepNumbers) {
				SEQstepNumbers = val;
				drawSEQstepNumbersMarker(SEQstepNumbers);
				printDataValue (0, SEQstepNumbers + 1);
				printRedMarker (0, value >> 1);
			}
		}
		
		// GateTime
		if (ParameterNr == mySEQGateTime) {
			if (value >= 250) {
				value = 250;
			}
			MidiClkGateTime = float(value / 98.0f); // Midi Clock gate time
			uint8_t val = value >> 2;
			float gateTimeX = (SEQGATETIME[val]);
			gateTimeX = 16.01f - gateTimeX;
			if (gateTimeX != SEQGateTime) {
				SEQGateTime = gateTimeX;
				printDataValue (1, (value >> 2) + 1);
				printRedMarker (1, value >> 1);
			}
			Interval = SEQclkRate;
			gateTime = (float)(SEQclkRate / SEQGateTime);
			if (gateTime <= 6200) {
				gateTime = 6200;
			}
		}
		
		// Direction
		if (ParameterNr == mySEQdir) {
			SEQdirection = (value >> 6);
			printSEQdirection();
			printRedMarker (2, value >> 1);
		}
		
		// Seq Mode
		if (ParameterNr == mySEQmode) {
			value = value >> 6;
			if (SEQmode != value) {
				tft.fillRect(130,116,20,8,ST7735_BLACK);
				if (value == 0) {
					SEQmode = 0;
					tft.setTextColor(ST7735_GRAY);
					print_String(44,130,116);		// print "KEY"
					printRedMarker (3, 0);
				}
				else if (value == 1){
					SEQmode = 1;
					tft.setTextColor(ST7735_GRAY);
					print_String(131,130,116);		// print "TRP"
					printRedMarker (3, 63);
				}
				else {
					SEQmode = 2;
					tft.setTextColor(ST7735_GRAY);
					print_String(132,130,116);		// print "REC"
					printRedMarker (3, 127);
				}
			}
		}
	}
	
	
	
	// Page:12 System  ----------------------------------------------------
	else if (Page == 12) {
		
		// System main page	
		if (myPageShiftStatus[PageNr] == false) {
			// Midi RxChanel
			if (ParameterNr == myMidiCha) {
				value = (value * 0.133f);
				if (value != midiChannel) {
					midiChannel = value;
					storeMidiChannel(midiChannel);
					tft.fillRoundRect(133,19,22,10,2,ST7735_BLUE);
					tft.setTextColor(ST7735_WHITE);
					if (midiChannel == 0) {
						tft.setCursor(136,21);
						tft.print("ALL");
					}
					else {
						tft.setCursor(137,21);
						tft.print(midiChannel);
					}
				}
			}
			
			// Velocity curve
			else if (ParameterNr == myVelocity) {
				uint8_t val = (0.04 * value);
				if (val >= 4) {
					val = 4;
				}
				tft.fillRect(136,40,17,7,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				tft.setCursor(137, 40);
				tft.print(val+1);
				velocitySens = val;
			}
			
			// Pickup
			else if (ParameterNr == myPickup) {
				tft.fillRoundRect(133,59,22,10,2,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				value = value >> 6;
				if (value == 0) {
					print_String(18,136,59);	// print "OFF"
					} else {
					print_String(33,138,59);	// print "ON"
				}
				if (pickupFlag != value) {
					pickupFlag = value;
					storePickup(pickupFlag);
				}
			}
			
			// MidiClk
			/*
			else if (ParameterNr == myMidiSyncSwitch) {
				uint8_t val = (0.047f * value);
				if (val >= 1) {
					val = 1;
				}
				if (val != MidiSyncSwitch) {
					tft.fillRect(136,78,17,7,ST7735_BLUE);
					tft.setTextColor(ST7735_WHITE);
					if (val == 0) {
						MidiSyncSwitch = false;
						storeMidiCkl(MidiSyncSwitch);
						tft.setCursor(136, 78);
						tft.print("INT");
					}
					else {
						MidiSyncSwitch = true;
						storeMidiCkl(MidiSyncSwitch);
						tft.setCursor(136, 78);
						tft.print("EXT");
					}
				}
			}
			*/
			
			// PRGChange
			else if (ParameterNr == myPRGChange) {
				tft.fillRoundRect(133,95,22,10,2,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				value = value >> 6;
				if (value == 0) {
					print_String(18,136,97);	// print "OFF"
					} else {
					print_String(33,138,97);	// print "ON"
				}
				if (PrgChangeSW != value) {
					PrgChangeSW = value;
					storePRGchange(PrgChangeSW);
				}
			}

			// UserPot1
			else if (ParameterNr == myUserPot1) {
				tft.fillRect(136,21,17,7,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				tft.setCursor(137, 21);
					value = value * 0.085f;
					if (value >= 10) {
						value = 10;
					}
					UserPot[0] = value;
					print_UserPOT(0);
			}
			
			// UserPot2
			else if (ParameterNr == myUserPot2) {
				tft.fillRect(136,40,17,7,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				tft.setCursor(137, 40);
				value = value * 0.085f;
				if (value >= 10) {
					value = 10;
				}
				UserPot[1] = value;
				print_UserPOT(1);
			}
			
			// UserPot3
			else if (ParameterNr == myUserPot3) {
				tft.fillRect(136,59,17,7,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				tft.setCursor(137, 59);
				value = value * 0.085f;
				if (value >= 10) {
					value = 10;
				}
				UserPot[2] = value;
				print_UserPOT(2);
			}
			
			// UserPot4
			else if (ParameterNr == myUserPot4) {
				tft.fillRect(136,78,17,7,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				tft.setCursor(137, 78);
				value = value * 0.085f;
				if (value >= 10) {
					value = 10;
				}
				UserPot[3] = value;
				print_UserPOT(3);
			}
			// VoiceMode (Unison mode)
			else if (ParameterNr == myVoiceMode && unison == 1) {
				uint8_t val = value * 0.048f;
				if (val >= 5) {
					val = 5;
				}
				if (val != Voice_mode) {
					Voice_mode = val;
					NoteStack_ptr = 0;
					printVoiceMode();
					myControlChange(midiChannel, CCunison, unison);
				}				
			}
		}
		
		// System sub page (SysEx Dump)
		else {
			// DESTINATION
			if (ParameterNr == mysysexDest) {
					value = (value / 128);
					if (value != sysexDest) {
						sysexDest = value;
						tft.fillRoundRect(124,19,30,10,2,ST7735_BLUE);
						tft.setTextColor(ST7735_WHITE);
						if (sysexDest == 0) {
							print_String(196,130,21); // print "USB"
						}
						else {
							//print_String(125,128,21); // print "MIDI"
						}
					}
			}
			// DUMP-TYP
			else if (ParameterNr == mysysexTyp) {
				value = (value / 64);
				if (value != sysexTyp) {
					sysexTyp = value;
					tft.fillRoundRect(124,38,30,10,2,ST7735_BLUE);
					tft.setTextColor(ST7735_WHITE);
					if (sysexTyp == 0) {
						print_String(156,124,21+19); // print "PATCH"
						SourceDisabled = false;
						tft.fillRoundRect(124,76,30,10,2,ST7735_BLUE);
						tft.setTextColor(ST7735_WHITE);
						if ((sysexSource+1) < 10) {
							tft.setCursor(136, 78);
						}
						else if ((sysexSource+1) < 100) {
							tft.setCursor(133, 78);
							} else {
							tft.setCursor(130, 78);
						}
						tft.print(sysexSource + 1);
					}
					else {
						print_String(36,127,21+19); // print "BANK"
						// disable Source
						SourceDisabled = true;
						tft.fillRoundRect(124,76,30,10,2,ST7735_BLUE);
						tft.setTextColor(ST7735_WHITE);
						print_String(31,130,78); // print "---"
					}
					
				}
			} 
			// DUMP-BANK
			else if (ParameterNr == mysysexBank) {
				value = (value / 8);
				if (value != sysexBank) {
					sysexBank = value;
				tft.fillRoundRect(124,57,30,10,2,ST7735_BLUE);
				tft.setTextColor(ST7735_WHITE);
				tft.setCursor(136, 59);
				tft.print(char(sysexBank + 65)); // print "A" - "P"
				}
			}
			// Source
			else if (ParameterNr == mysysexSource) {
				if (value != sysexSource && SourceDisabled == false) {
					sysexSource = value;
					tft.fillRoundRect(124,76,30,10,2,ST7735_BLUE);
					tft.setTextColor(ST7735_WHITE);
					if ((sysexSource+1) < 10) {
						tft.setCursor(136, 78);
					}
					else if ((sysexSource+1) < 100) {
						tft.setCursor(133, 78);
					} else {
						tft.setCursor(130, 78);
					}
					tft.print(sysexSource + 1);
				}
			}
			
			else if (ParameterNr == mysysexDump) {
				value = (value / 64);
				if (value != sysexDump) {
					sysexDump = value;
					tft.fillRoundRect(124,95,30,10,2,ST7735_BLUE);
					tft.setTextColor(ST7735_WHITE);
					if (sysexDump == 0) {
						print_String(197,127,97); // print "SEND"
					}
					else {
						print_String(198,130,97); // print "REC"
					}
				}
			}
		}
	}
	
	
	// Page:99 Save Patch Page --------------------------------------------
	else if (Page == 99) {
		// Patch Bank
		if (ParameterNr == myBankSelect){
			if (value >= 15) {
				value = 15;
			}
			if (value != newPatchBank) {
				newPatchBank = value;
				tft.fillRoundRect(6,21,14,19,2,ST7735_RED);
				tft.setCursor(8, 23);
				tft.setTextColor(ST7735_WHITE);
				tft.setFont(NULL);
				tft.setTextSize(2);
				tft.println(char(newPatchBank + 65)); // print char BankNo
				recallPatch2(newPatchNo, newPatchBank);
				// print Patch Name (max 12 char)
				newPatchName = patchName;
				uint8_t stringLen = newPatchName.length();
				// test String lenght (max 12 Char)
				if (stringLen > 12) {
					newPatchName = patchName.substring(0, 12);
				}
				// if String less than 12 fill in with spaces
				else if (stringLen < 12) {
					for (uint8_t i = stringLen; i < 12; i++) {
						newPatchName.concat(" ");
					}
				}
				tft.fillRect(70,30,85,16, ST7735_BLACK);
				tft.setCursor(70,30);
				tft.setTextColor(ST7735_GRAY);
				tft.setTextSize(1);
				tft.println(newPatchName);
			}
		}
		
		// PatchNo
		else if (ParameterNr == myPatchNo){
			tft.setCursor(23, 23);
			tft.fillRect(23,23,34,14,ST7735_BLACK);
			tft.setCursor(23, 23);
			tft.setTextColor(ST7735_YELLOW);
			tft.setTextSize(2);
			tft.setFont(NULL);
			int patchnumber = value + 1;
			if (patchnumber >= 128) {
				patchnumber = 128;
			}
			if (patchnumber < 10) {
				tft.print("00");
				tft.print(patchnumber);
			}
			else if (patchnumber < 100) {
				tft.print("0");
				tft.print(patchnumber);
			}
			else tft.print(patchnumber);
			recallPatch2(patchnumber, newPatchBank);
			newPatchNo = patchnumber;
			// print Patch Name (max 12 char)
			newPatchName = patchName;
			uint8_t stringLen = newPatchName.length();
			// test String lenght (max 12 Char)
			if (stringLen > 12) {
				newPatchName = patchName.substring(0, 12);
			}
			// if String less than 12 fill in with spaces
			else if (stringLen < 12) {
				for (uint8_t i = stringLen; i < 12; i++) {
					newPatchName.concat(" ");
				}
			}
			tft.fillRect(70,30,85,16, ST7735_BLACK);
			tft.setCursor(70,30);
			tft.setTextColor(ST7735_GRAY);
			tft.setTextSize(1);
			tft.println(newPatchName);
			
		}
		// Char
		else if (ParameterNr == myChar){
			uint8_t val = (value * 0.62f);
			uint8_t charName = charMap[val];
			tft.fillRect(7 + (CharPosPatchName * 12),48,12,18, ST7735_BLACK);
			tft.setFont(NULL);
			tft.setTextSize(2);
			tft.setCursor(8 + (CharPosPatchName * 12), 50);
			tft.setTextColor(ST7735_WHITE);
			tft.println(char (charName));
			oldPatchName.setCharAt(CharPosPatchName, charName);
		}
		// Cursor
		else if (ParameterNr == myCursor){
			CharPosPatchName = (value * 0.094f);
			
			tft.fillRect(7 + (CharPosPatchName * 12),48,12,18, ST7735_BLACK);
			tft.setFont(NULL);
			tft.setTextSize(2);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(8, 50);
			tft.println(oldPatchName);
			tft.setCursor(8,57);
			tft.setTextColor(ST7735_GRAY);
			tft.println("____________");
			tft.setCursor(8 + (CharPosPatchName * 12), 50);
			tft.setTextColor(ST7735_WHITE);
			tft.println(oldPatchName.charAt(CharPosPatchName));
			tft.setCursor(8 + (CharPosPatchName * 12), 57);
			tft.setTextColor(ST7735_WHITE);
			tft.println("_");
		}
	}
	
	// Page:98 save Sequencer Pattern Page --------------------------------
	else if (Page == 98) {
		// PatternNo
		if (ParameterNr == myPatternNo){
			SEQPatternNo = value + 1;
			if (oldPatternNo != SEQPatternNo ) {
				tft.setCursor(123, 23);
				tft.fillRect(123,23,34,14,ST7735_BLACK);
				tft.setCursor(123, 23);
				tft.setTextColor(ST7735_RED);
				tft.setTextSize(2);
				tft.setFont(NULL);
				if (SEQPatternNo < 10) {
					tft.print("0");
				}
				tft.print(SEQPatternNo);
				oldPatternNo = SEQPatternNo;
				recallPattern(SEQPatternNo);
				// print Pattern Name (max 12 char)
				newPatternName = patternName;
				uint8_t stringLen = newPatternName.length();
				// test String lenght (max 12 Char)
				if (stringLen > 12) {
					newPatternName = patternName.substring(0, 12);
				}
				// if String less than 12 fill in with spaces
				else if (stringLen < 12) {
					for (uint8_t i = stringLen; i < 12; i++) {
						newPatternName.concat(" ");
					}
				}
				tft.fillRect(8,50,142,16, ST7735_BLACK);
				tft.setCursor(8,50);
				tft.setTextColor(ST7735_GRAY);
				tft.println(newPatternName);
			}
		}
		
		// Char
		else if (ParameterNr == myChar){
			uint8_t val = (value * 0.62f);
			uint8_t charName = charMap[val];
			tft.fillRect(7 + (CharPosPatchName * 12),48,12,18, ST7735_BLACK);
			tft.setFont(NULL);
			tft.setTextSize(2);
			tft.setCursor(8 + (CharPosPatchName * 12), 50);
			tft.setTextColor(ST7735_WHITE);
			tft.println(char (charName));
			newPatternName.setCharAt(CharPosPatchName, charName);
		}
		// Cursor
		else if (ParameterNr == myCursor){
			CharPosPatchName = (value * 0.094f);
			
			tft.fillRect(7 + (CharPosPatchName * 12),48,12,18, ST7735_BLACK);
			tft.setFont(NULL);
			tft.setTextSize(2);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(8, 50);
			tft.println(newPatternName);
			tft.setCursor(8,57);
			tft.setTextColor(ST7735_GRAY);
			tft.println("____________");
			tft.setCursor(8 + (CharPosPatchName * 12), 50);
			tft.setTextColor(ST7735_WHITE);
			tft.println(newPatternName.charAt(CharPosPatchName));
			tft.setCursor(8 + (CharPosPatchName * 12), 57);
			tft.setTextColor(ST7735_WHITE);
			tft.println("_");
		}
	}
	
	// Page:98 Load Sequencer Pattern Page --------------------------------
	else if (Page == 97) {
		// PatternNo
		if (ParameterNr == myPatternNo){
			SEQPatternNo = value + 1;
			if (oldPatternNo != SEQPatternNo ) {
				tft.setCursor(123, 23);
				tft.fillRect(123,23,34,14,ST7735_BLACK);
				tft.setCursor(123, 23);
				tft.setTextColor(ST7735_GREEN);
				tft.setTextSize(2);
				tft.setFont(NULL);
				if (SEQPatternNo < 10) {
					tft.print("0");
				}
				tft.print(SEQPatternNo);
				oldPatternNo = SEQPatternNo;
				recallPattern(SEQPatternNo);
				// print Pattern Name (max 12 char)
				newPatternName = patternName;
				uint8_t stringLen = newPatternName.length();
				// test String lenght (max 12 Char)
				if (stringLen > 12) {
					newPatternName = patternName.substring(0, 12);
				}
				// if String less than 12 fill in with spaces
				else if (stringLen < 12) {
					for (uint8_t i = stringLen; i < 12; i++) {
						newPatternName.concat(" ");
					}
				}
				tft.fillRect(8,50,142,16, ST7735_BLACK);
				tft.setCursor(8,50);
				tft.setTextColor(ST7735_GRAY);
				tft.println(newPatternName);
			}
		}
	}
}

//*************************************************************************
// print FilterFreq to filter page
//*************************************************************************
FLASHMEM void printFilterFrq(uint16_t value)
{
	// clear old value
	tft.fillRect(2,115,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	
	// set red marker
	tft.drawLine(4,116,30,116,ST7735_BLUE);
	// calc linear value for marker
	uint8_t Frq_value = 0;
	float Frq = value;
	for (int i = 0; i < 256; i++) {
		float frequency = FILTERFREQS256[i];
		if (Frq <= frequency){
			Frq_value = i;
			break;
		}
	}
	int redPos = (0.1016f * Frq_value);
	tft.drawFastVLine(4 + redPos, 115, 4, ST7735_RED);
	
	// print current value
	if (value < 100){
		tft.setCursor(6, 120);
		tft.print(value);
		tft.print("Hz");
	}
	else if (value < 1000){
		tft.setCursor(2, 120);
		tft.print(value);
		tft.print("Hz");
	}
	else if (value < 10000){
		value = value / 1000;
		tft.setCursor(6, 120);
		tft.print(value);
		tft.print("KHz");
	}
	else if (value < 100000){
		value = value / 1000;
		tft.setCursor(2, 120);
		tft.print(value);
		tft.print("KHz");
	}
}

//*************************************************************************
// print FilterRes to filter page
//*************************************************************************
FLASHMEM void printFilterRes(uint8_t value)
{
	// clear old value
	tft.fillRect(44,112,27,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	
	// set red marker
	tft.drawLine(44,113,70,113,ST7735_BLUE);
	int redPos = (0.26f * value);
	tft.drawFastVLine(44 + redPos, 112, 4, ST7735_RED);
	
	// print current value
	if (value < 10){
		tft.setCursor(54,116);
	}
	else if (value < 100){
		tft.setCursor(50,116);
	}
	else tft.setCursor(46,116);
	tft.print(value);
	tft.print("%");
}

//*************************************************************************
// print FilterTraking to filter page
//*************************************************************************
FLASHMEM void printFilterKey(uint8_t value)
{
	// clear old value
	tft.fillRect(84,115,27,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	
	// set red marker
	tft.drawLine(84,116,110,116,ST7735_BLUE);
	int redPos = (0.26f * value);
	tft.drawFastVLine(84 + redPos, 115, 4, ST7735_RED);
	
	// print current value
	if (value < 10){
		tft.setCursor(94,116);
	}
	else if (value < 100){
		tft.setCursor(90,116);
	}
	else tft.setCursor(86,116);
	tft.print(value);
	tft.print("%");
}

//*************************************************************************
// print Percent value (8Bit integer)
//*************************************************************************
FLASHMEM void printPercentValue (uint8_t index, uint8_t value)
{
	uint8_t xpos = 0;
	uint8_t ypos = 116;
	
	if (index == 0) {
		xpos = 2;
	}
	else if (index == 1) {
		xpos = 42;
	}
	else if (index == 2) {
		xpos = 82;
	}
	else {
		xpos = 122;
	}
	
	// clear old value and marker
	tft.fillRect(xpos, ypos, 30, 7, ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	if (value < 10){
		tft.setCursor(xpos + 10, ypos);
		tft.print(value);
	}
	else if (value < 100){
		tft.setCursor(xpos + 8, ypos);
		tft.print(value);
	}
	else {
		tft.setCursor(xpos + 3, ypos);
		tft.print(value);
	}
	tft.print("%");
}

//*************************************************************************
// print FilterTyp to filter page
//*************************************************************************
FLASHMEM void printFilterTyp(uint8_t value)
{
	// clear old value
	tft.fillRect(124,112,27,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	
	// set red marker
	tft.drawLine(124,113,150,113,ST7735_BLUE);
	tft.drawFastVLine(149, 112, 4, ST7735_GREEN);
	int redPos = (0.205f * value);
	tft.drawFastVLine(124 + redPos, 112, 4, ST7735_RED);
	
	// print new value
	tft.setCursor(134,116);
	if (value < 63){
		tft.println("LP");
		FilterTyp = 0;
	}
	else if (value >= 63 && value < 125){
		tft.println("HP");
		FilterTyp = 1;
	}
	else {
		tft.println("BP");
		FilterTyp = 2;
	}
}

//*************************************************************************
// print Filter Envelope Amount
//*************************************************************************
FLASHMEM void printFilterEnvAmt(uint8_t Value)
{
	if (Value >= 126) {
		Value = 126;
	}
	
	if (Value == 63) {
		tft.fillRect(82, 116, 30, 7, ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(94, 116);
		tft.print(0);
	}
	else if (Value < 63) {
		tft.fillRect(82, 116, 30, 7, ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(90, 116);
		tft.print("-");
		tft.print(63 - Value);
	}
	else {
		tft.fillRect(82, 116, 30, 7, ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(90, 116);
		tft.print("+");
		tft.print(Value - 63);
	}
}

//*************************************************************************
// print Value -63...+63
//*************************************************************************
FLASHMEM void print_value_63(uint8_t Value, uint8_t Xpos, uint8_t Ypos, uint16_t Text_color)
{	
	if (Value >= 126) {
		Value = 126;
	}
	
	if (Value == 63) {
		tft.setTextColor(Text_color);
		tft.setCursor(Xpos + 5, Ypos);
		tft.print(0);
	}
	else if (Value < 63) {
		tft.setTextColor(Text_color);
		tft.setCursor(Xpos, Ypos);
		tft.print("-");
		tft.print(63 - Value);
	}
	else {
		tft.setTextColor(Text_color);
		tft.setCursor(Xpos, Ypos);
		tft.print("+");
		tft.print(Value - 63);
	}
}

//*************************************************************************
// print OscMix value
//*************************************************************************
FLASHMEM void printOscMixVal(uint8_t Value)
{
	if (Value == 63 || Value == 64) {
		tft.fillRoundRect(133,95,22,10,2,ST7735_BLUE);
		tft.setCursor(135,97);
		tft.setTextColor(ST7735_WHITE);
		tft.print(0);
	}
	else if (Value < 63) {
		tft.fillRoundRect(133,95,22,10,2,ST7735_BLUE);
		tft.setCursor(135,97);
		tft.setTextColor(ST7735_WHITE);
		tft.print("-");
		tft.print(63 - Value);
	}
	else {
		tft.fillRoundRect(133,95,22,10,2,ST7735_BLUE);
		tft.setCursor(135,97);
		tft.setTextColor(ST7735_WHITE);
		tft.print("+");
		tft.print(Value - 64);
	}
}

//*************************************************************************
//
//*************************************************************************
void printOscMix (void)
{
	uint8_t value = 0;
	
	tft.fillRoundRect(133,95,22,10,2,ST7735_BLUE);
	tft.setCursor(135,97);
	tft.setTextColor(ST7735_WHITE);
	
	if (oscALevel == 1.0000 && oscBLevel == 1.0000) {
		tft.print("0");
	}
	else if (oscBLevel == 1.0) {
		for (uint8_t i = 0; i < 128; i++) {
			float myValue = LINEAR[i];
			if (oscALevel <= myValue) {
				value = i;
				tft.print("+");
				tft.print(63 - (value / 2));
				break;
			}
		}
	}
	else {
		for (uint8_t i = 0; i < 128; i++) {
			float myValue = LINEAR[i];
			if (oscBLevel <= myValue) {
				value = i;
				tft.print("-");
				tft.print(63 - (value / 2));
				break;
			}
		}
	}
}

//*************************************************************************
// print Osc Pitch Env
//*************************************************************************
FLASHMEM void printOscPitchEnv(uint8_t Value)
{
	if (Value == 63) {
		tft.fillRoundRect(54,57,22,10,2,ST7735_BLUE);
		tft.setCursor(56,59);
		tft.setTextColor(ST7735_WHITE);
		tft.print(0);
	}
	else if (Value < 63) {
		tft.fillRoundRect(54,57,22,10,2,ST7735_BLUE);
		tft.setCursor(56,59);
		tft.setTextColor(ST7735_WHITE);
		tft.print("-");
		tft.print(63 - Value);
	}
	else {
		tft.fillRoundRect(54,57,22,10,2,ST7735_BLUE);
		tft.setCursor(56,59);
		tft.setTextColor(ST7735_WHITE);
		tft.print("+");
		tft.print(Value - 63);
	}
}

//*************************************************************************
// print Data value
//*************************************************************************
FLASHMEM void printDataValue (uint8_t index, uint16_t value)
{
	uint8_t xpos = 0;
	uint8_t ypos = 116;
	
	if (index == 0) {
		xpos = 2;
	}
	else if (index == 1) {
		xpos = 42;
	}
	else if (index == 2) {
		xpos = 82;
	}
	else {
		xpos = 122;
	}
	
	// clear old value and marker
	tft.fillRect(xpos, ypos, 30, 7, ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	if (value < 10){
		tft.setCursor(xpos + 14, ypos);
		tft.print(value);
	}
	else if (value < 100){
		tft.setCursor(xpos + 11, ypos);
		tft.print(value);
	}
	else {
		tft.setCursor(xpos + 7, ypos);
		tft.print(value);
	}
}

//*************************************************************************
// print Sequencer Clk div
//*************************************************************************
FLASHMEM void printSEQclkDiv (uint8_t SEQdivider) {
	
	uint8_t xpos = 122;
	uint8_t ypos = 116;
	
	tft.fillRect(xpos, ypos, 30, 7, ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	tft.setCursor(xpos + 7, ypos);
	
	if (SEQMidiClkSwitch == false) {
		switch (SEQdivider)
		{
			case 0:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/2");
			break;
			case 1:
			tft.setCursor(xpos + 7, ypos);
			tft.print("3/8");
			break;
			case 2:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/3");
			break;
			case 3:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/4");
			break;
			case 4:
			tft.setCursor(xpos + 5, ypos);
			tft.print("3/16");
			break;
			case 5:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/6");
			break;
			case 6:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/8");
			break;
			case 7:
			tft.setCursor(xpos + 5, ypos);
			tft.print("3/32");
			break;
			case 8:
			tft.setCursor(xpos + 5, ypos);
			tft.print("1/12");
			break;
			case 9:
			tft.setCursor(xpos + 5, ypos);
			tft.print("1/16");
			break;
			case 10:
			tft.setCursor(xpos + 5, ypos);
			tft.print("1/24");
			break;
			case 11:
			tft.setCursor(xpos + 5, ypos);
			tft.print("1/32");
			break;
			case 12:
			tft.setCursor(xpos + 5, ypos);
			tft.print("1/48");
			break;
		}
		} else {
		switch (SEQdivider)
		{
			case 0:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/2");
			break;
			case 1:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/2");
			break;
			case 2:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/2");
			break;
			case 3:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/4");
			break;
			case 4:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/4");
			break;
			case 5:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/4");
			break;
			case 6:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/8");
			break;
			case 7:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/8");
			break;
			case 8:
			tft.setCursor(xpos + 7, ypos);
			tft.print("1/8");
			break;
			case 9:
			tft.setCursor(xpos + 5, ypos);
			tft.print("1/16");
			break;
			case 10:
			tft.setCursor(xpos + 5, ypos);
			tft.print("1/16");
			break;
			case 11:
			tft.setCursor(xpos + 5, ypos);
			tft.print("1/16");
			break;
			case 12:
			tft.setCursor(xpos + 5, ypos);
			tft.print("1/16");
			break;
		}
	}
	
}

//*************************************************************************
// print Red marker
//*************************************************************************
FLASHMEM void printRedMarker (uint8_t index, uint8_t value)
{
	uint8_t xpos = 0;
	uint8_t ypos = 112;
	
	if (index == 0) {
		xpos = 2;
	}
	else if (index == 1) {
		xpos = 42;
	}
	else if (index == 2) {
		xpos = 82;
	}
	else {
		xpos = 122;
	}
	
	// clear old value and marker
	tft.fillRect(xpos, ypos, 30, 4, ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawFastHLine(xpos + 2, ypos + 1, 27, ST7735_BLUE);
	int redPos = (0.205f * value);
	tft.drawFastVLine(xpos + 2 + redPos, ypos, 4, ST7735_RED);
}


//*************************************************************************
// print Envelope value
//*************************************************************************
FLASHMEM void printEnvValue (uint8_t value, uint8_t xpos, uint8_t ypos)
{
	int TimeMS = ENVTIMES[value];
	
	// print current value
	if (TimeMS < 10){
		tft.setCursor(xpos,ypos);
		tft.print(TimeMS);
		tft.print("ms");
	}
	else if (TimeMS < 100){
		tft.setCursor(xpos - 4,ypos);
		tft.print(TimeMS);
		tft.print("ms");
	}
	else if (TimeMS < 1000){
		tft.setCursor(xpos - 7,ypos);
		tft.print(TimeMS);
		tft.print("ms");
	}
	else if (TimeMS < 10000){
		tft.setCursor(xpos - 5,ypos);
		float Sekunde = (ENVTIMES[value]);
		tft.print(Sekunde / 1000,1);
		tft.print("s");
	}
	else if (TimeMS < 100000){
		tft.setCursor(xpos - 7,ypos);
		float Sekunde = (ENVTIMES[value]);
		tft.print(Sekunde / 1000,1);
		tft.print("s");
	}
}

//*************************************************************************
// print Envelope values
//*************************************************************************
FLASHMEM void printEnvATKvalues (uint8_t value)
{
	// clear old value and marker
	tft.fillRect(2,112,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(4,113,30,113,ST7735_BLUE);
	int redPos = (0.205f * value);
	tft.drawFastVLine(4 + redPos, 112, 4, ST7735_RED);
	// print currentvalue
	uint8_t xpos = 10;
	uint8_t ypos = 116;
	printEnvValue (value, xpos, ypos);
}
FLASHMEM void printEnvDCYvalues (uint8_t value)
{
	// clear old value and marker
	tft.fillRect(42,112,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(44,113,70,113,ST7735_BLUE);
	int redPos = (0.205f * value);
	tft.drawFastVLine(44 + redPos, 112, 4, ST7735_RED);
	// print current value
	uint8_t xpos = 50;
	uint8_t ypos = 116;
	printEnvValue (value, xpos, ypos);
}
FLASHMEM void printEnvSUSvalues (uint8_t value)
{
	// clear old value and marker
	tft.fillRect(82,112,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(84,113,110,113,ST7735_BLUE);
	int redPos = (0.205 * value);
	tft.drawFastVLine(84 + redPos, 112, 4, ST7735_RED);
	// print current value
	printPercentValue(2, (value * 0.788));
}
FLASHMEM void printEnvRELvalues (uint8_t value)
{
	// clear old value and marker
	tft.fillRect(122,112,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(124,113,150,113,ST7735_BLUE);
	int redPos = (0.205f * value);
	tft.drawFastVLine(124 + redPos, 112, 4, ST7735_RED);
	// print current value
	uint8_t xpos = 130;
	uint8_t ypos = 116;
	printEnvValue (value, xpos, ypos);
}

FLASHMEM void printEnvATKvaluesX (uint8_t value, uint8_t prevValue, boolean prevFlag)
{
	const uint8_t xpos = 10;
	const uint8_t ypos = 116;
	
	// clear old value and marker
	tft.fillRect(2,112,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(4,113,30,113,ST7735_BLUE);
	if (prevFlag == false && value != prevValue) {
		int redPos = (0.205f * value);
		tft.drawCircle(4 + redPos, 113, 1, ST7735_GREEN);
		tft.setTextColor(ST7735_GRAY);
		printEnvValue (value, xpos, ypos);
		tft.setCursor(xpos + 5, 92);
		tft.setTextColor(ST7735_GREEN);
		if (value > prevValue) {
			tft.print(">");
		} else {
			tft.print("<");
		}
		tft.setTextColor(ST7735_GRAY);
	}
	else {
		// print currentvalue
		printEnvValue (prevValue, xpos, ypos);
		tft.fillRect(xpos + 5, 92,5,8,ST7735_BLACK);
	}
	int redPos = (0.205f * prevValue);
	tft.drawFastVLine(4 + redPos, 112, 4, ST7735_RED);
	
}

FLASHMEM void printEnvDCYvaluesX (uint8_t value, uint8_t prevValue, boolean prevFlag)
{
	const uint8_t xpos = 50;
	const uint8_t ypos = 116;
	
	// clear old value and marker
	tft.fillRect(42,112,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(44,113,70,113,ST7735_BLUE);
	if (prevFlag == false && value != prevValue) {
		int redPos = (0.205f * value);
		tft.drawCircle(44 + redPos, 113, 1, ST7735_GREEN);
		tft.setTextColor(ST7735_GRAY);
		printEnvValue (value, xpos, ypos);
		tft.setCursor(xpos + 5, 92);
		tft.setTextColor(ST7735_GREEN);
		if (value > prevValue) {
			tft.print(">");
			} else {
			tft.print("<");
		}
		tft.setTextColor(ST7735_GRAY);
	}
	else {
		// print current value
		printEnvValue (prevValue, xpos, ypos);
		printEnvValue (prevValue, xpos, ypos);
		tft.fillRect(xpos + 5, 92,5,8,ST7735_BLACK);
	}
	int redPos = (0.205f * prevValue);
	tft.drawFastVLine(44 + redPos, 112, 4, ST7735_RED);
	
}

FLASHMEM void printEnvSUSvaluesX (uint8_t value, uint8_t prevValue, boolean prevFlag)
{
	const uint8_t xpos = 90;
	
	// clear old value and marker
	tft.fillRect(82,112,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(84,113,110,113,ST7735_BLUE);
	if (prevFlag == false && value != prevValue) {
		int redPos = (0.205f * value);
		tft.drawCircle(84 + redPos, 113, 1, ST7735_GREEN);
		tft.setTextColor(ST7735_GRAY);
		printPercentValue(2, (value * 0.788f));
		tft.setCursor(xpos + 5, 92);
		tft.setTextColor(ST7735_GREEN);
		if (value > prevValue) {
			tft.print(">");
			} else {
			tft.print("<");
		}
		tft.setTextColor(ST7735_GRAY);
	}
	else {
		// print currentvalue
		printPercentValue(2, (prevValue * 0.788f));
		tft.fillRect(xpos + 5, 92,5,8,ST7735_BLACK);
	}
	int redPos = (0.205f * prevValue);
	tft.drawFastVLine(84 + redPos, 112, 4, ST7735_RED);
	
}

FLASHMEM void printEnvRELvaluesX (uint8_t value, uint8_t prevValue, boolean prevFlag)
{
	const uint8_t xpos = 130;
	const uint8_t ypos = 116;
	
	// clear old value and marker
	tft.fillRect(122,112,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(124,113,150,113,ST7735_BLUE);
	if (prevFlag == false && value != prevValue) {
		int redPos = (0.205f * value);
		tft.drawCircle(124 + redPos, 113, 1, ST7735_GREEN);
		tft.setTextColor(ST7735_GRAY);
		printEnvValue (value, xpos, ypos);
		tft.setCursor(xpos + 5, 92);
		tft.setTextColor(ST7735_GREEN);
		if (value > prevValue) {
			tft.print(">");
			} else {
			tft.print("<");
		}
		tft.setTextColor(ST7735_GRAY);
	}
	else {
		// print currentvalue
		printEnvValue (prevValue, xpos, ypos);
		tft.fillRect(xpos + 5, 92,5,8,ST7735_BLACK);
	}
	int redPos = (0.205f * prevValue);
	tft.drawFastVLine(124 + redPos, 112, 4, ST7735_RED);
	
}

//*************************************************************************
// print Pick up value 0-100 / pot 0-3
//*************************************************************************
FLASHMEM void printPickupValue (uint8_t pot, uint8_t value, uint8_t prevValue, boolean prevFlag)
{
	uint8_t xpos = 10 + (40 * pot);
	const uint8_t ypos = 116;
	//uint8_t val;
	boolean temp_pickupFlag = pickupFlag;
	
	if (PageNr == 0) {
		pickupFlag = true;
	}
	
	if (pickupFlag == false) {
		prevFlag = true;
	}
	
	// clear old value and marker
	tft.fillRect(xpos-8,ypos-4,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(xpos-6,ypos-3,30 + (40 * pot),ypos-3,ST7735_BLUE);
	
	if (prevFlag == false && value != prevValue) {
		int redPos = roundf(0.205f * value);
		if (pickupFlag == true) {
			tft.drawCircle(xpos-6 + redPos,ypos-3, 1, ST7735_GREEN);
		}
		if (value < 10) {
			tft.setCursor(xpos + 6, ypos);
		}
		else if ( value < 100) {
			tft.setCursor(xpos + 3, ypos);
		}
		else {
			tft.setCursor(xpos, ypos);
		}
		tft.print(value);
	}
	else {
		// print current value
		if (prevValue < 10) {
			tft.setCursor(xpos + 6, ypos);
		}
		else if ( prevValue < 100) {
			tft.setCursor(xpos + 3, ypos);
		}
		else {
			tft.setCursor(xpos, ypos);
		}
		tft.print(prevValue);
	}
	int redPos = roundf(0.205f * prevValue);  // par = 26 pixel lenght
	tft.drawFastVLine(xpos-6 + redPos, ypos-4, 4, ST7735_RED);
	
	pickupFlag = temp_pickupFlag;
}

//*************************************************************************
// print Pick up value +-64 / pot 0-3
//*************************************************************************
FLASHMEM void printPickupInt64 (uint8_t pot, int8_t value, int8_t prevValue, boolean prevFlag)
{
	uint8_t xpos = 10 + (40 * pot);
	const uint8_t ypos = 116;
	uint8_t val;
	
	if (pickupFlag == false) {
		prevFlag = true;
	}
	
	// clear old value and marker
	tft.fillRect(xpos-8,ypos-4,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(xpos-6,ypos-3,30 + (40 * pot),ypos-3,ST7735_BLUE);
	
	val = prevValue;
	if (val >= 126){
		val = 126;
	}
	
	if (prevFlag == false && value != val) {
		int redPos = (0.205f * value);
		if (pickupFlag == true) {
			tft.drawCircle(xpos-6 + redPos,ypos-3, 1, ST7735_GREEN);
		}
		val = value;
		if (val >= 126) {
			val = 126;
		}
		if (val == 63) {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor((xpos + 5), 116);
			tft.print(0);
		}
		else if (val < 63) {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(xpos, 116);
			tft.print("-");
			tft.print(63 - val);
		}
		else {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(xpos, 116);
			tft.print("+");
			tft.print(val - 63);
		}
		// draw green Arrow
		tft.setCursor(xpos + 5, 92);
		tft.setTextColor(ST7735_GREEN);
		if (value > prevValue) {
			tft.print(">");
			} else {
			tft.print("<");
		}
		tft.setTextColor(ST7735_GRAY);
		
	}
	else {
		val = prevValue;
		if (val >= 126) {
			val = 126;
		}
		if (val == 63) {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor((xpos + 5), 116);
			tft.print(0);
		}
		else if (val < 63) {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(xpos, 116);
			tft.print("-");
			tft.print(63 - val);
		}
		else {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(xpos, 116);
			tft.print("+");
			tft.print(val - 63);
		}
		//clear grren Arrow
		tft.fillRect(xpos + 5, 92,5,8,ST7735_BLACK);
	}
	
	int redPos = (0.205f * prevValue);  // line lenght = 26 pixel
	tft.drawFastVLine(xpos-6 + redPos, ypos-4, 4, ST7735_RED);
	
}

//*************************************************************************
// print Pick up value +-8 / pot 0-3
//*************************************************************************
FLASHMEM void printPickupInt8 (uint8_t pot, int8_t value, int8_t prevValue, boolean prevFlag)
{
	uint8_t xpos = 10 + (40 * pot);
	const uint8_t ypos = 116;
	int8_t val;
	
	if (pickupFlag == false) {
		prevFlag = true;
	}
	
	// clear old value and marker
	tft.fillRect(xpos-8,ypos-4,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(xpos-6,ypos-3,30 + (40 * pot),ypos-3,ST7735_BLUE);
	
	val = (prevValue / 7.48f);
	if (val >= 17){
		val = 17;
	}
	
	uint8_t valx =(value / 7.48f);
	
	if (prevFlag == false && valx != val) {
		int redPos = (0.205f * value);
		if (pickupFlag == true) {
			tft.drawCircle(xpos-6 + redPos,ypos-3, 1, ST7735_GREEN);
		}
		val = (value / 7.48f);
		if (val >= 17) {
			val = 17;
		}
		if (val == 8) {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor((xpos + 5), 116);
			tft.print(0);
		}
		else if (val < 8) {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(xpos, 116);
			tft.print("-");
			tft.print(8 - val);
		}
		else {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(xpos, 116);
			tft.print("+");
			tft.print(val - 8);
		}
		// draw green Arrow
		tft.setCursor(xpos + 5, 92);
		tft.setTextColor(ST7735_GREEN);
		if (value > prevValue) {
			tft.print(">");
			} else {
			tft.print("<");
		}
		tft.setTextColor(ST7735_GRAY);
		
	}
	else {
		val = (prevValue / 7.48f);
		if (val >= 17) {
			val = 17;
		}
		if (val == 8) {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor((xpos + 5), 116);
			tft.print(0);
		}
		else if (val < 8) {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(xpos, 116);
			tft.print("+");
			tft.print(8 - val);
		}
		else {
			tft.fillRect((xpos - 8), 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(xpos, 116);
			tft.print("-");
			tft.print(val - 8);
		}
		//clear green Arrow
		tft.fillRect(xpos + 5, 92,5,8,ST7735_BLACK);
		// draw envelope curve
		
		//updateFilterEnvelopeType();
		//drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
	}
	
	int redPos = (0.205f * prevValue);  // line lenght = 26 pixel
	tft.drawFastVLine(xpos-6 + redPos, ypos-4, 4, ST7735_RED);
	
}

//*************************************************************************
// print Pick up value 0-100 / pot 0-3
//*************************************************************************
FLASHMEM void printPickupVCFTyp (uint8_t pot, uint8_t value, uint8_t prevValue, boolean prevFlag)
{
	uint8_t xpos = 10 + (40 * pot);
	const uint8_t ypos = 116;
	// uint8_t val;
	
	if (pickupFlag == false) {
		prevFlag = true;
	}
	
	// clear old value and marker
	tft.fillRect(xpos-8,ypos-4,30,12,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	// set current marker
	tft.drawLine(xpos-6,ypos-3,30 + (40 * pot),ypos-3,ST7735_BLUE);
	tft.drawFastVLine(149, 112, 4, ST7735_GREEN);
	
	if (prevFlag == false && value != prevValue) {
		int redPos = (0.205f * value);
		if (pickupFlag == true) {
			tft.drawCircle(xpos-6 + redPos,ypos-3, 1, ST7735_GREEN);
		}
		tft.setCursor(xpos + 4,ypos);
		if (value < 63){
			tft.println("LP");
			FilterTyp = 0;
		}
		else if (value >= 63 && value < 125){
			tft.println("HP");
			FilterTyp = 1;
		}
		else {
			tft.println("BP");
			FilterTyp = 2;
		}
		// draw green Arrow
		tft.setCursor(xpos + 5, 92);
		tft.setTextColor(ST7735_GREEN);
		if (value > prevValue) {
			tft.print(">");
			} else {
			tft.print("<");
		}
		tft.setTextColor(ST7735_GRAY);
	}
	else {
		// print current value
		tft.setCursor(xpos + 4,ypos);
		if (prevValue < 63){
			tft.println("LP");
			FilterTyp = 0;
		}
		else if (prevValue >= 63 && prevValue < 125){
			tft.println("HP");
			FilterTyp = 1;
		}
		else {
			tft.println("BP");
			FilterTyp = 2;
		}
		//clear grren Arrow
		tft.fillRect(xpos + 5, 92,5,8,ST7735_BLACK);
	}
	int redPos = (0.205f * prevValue);  // par = 26 pixel lenght
	tft.drawFastVLine(xpos-6 + redPos, ypos-4, 4, ST7735_RED);
	
}

//*************************************************************************
// draw Peakmeter
//*************************************************************************
FLASHMEM void drawPeakmeter (void)
{
	tft.fillRect(149,1,9,11,ST7735_LIGHTGRAY);
	tft.drawFastHLine(149,11,9,ST7735_GREEN);
	tft.drawFastVLine(153,1,11,ST7735_GRAY);
}

// ************************************************************************
// print PWAMT/SPREAD & PWMOD/SAWMIX
//*************************************************************************
FLASHMEM void print_quadsaw_pwamt(void)
{
	tft.fillRect(85,58,36,9,ST7735_BLACK);
	tft.fillRect(85,77,36,9,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	tft.setFont(NULL);
	if (PageNr == 1 && (Osc1WaveBank == 0 && oscWaveformA == 3)) {
		print_String(231,85,59);	// print "SPREAD"
		print_String(232,85,78);	// print "SAWMIX"
		tft.fillRoundRect(133,57,22,10,2,ST7735_BLUE);
		tft.fillRoundRect(133,76,22,10,2,ST7735_BLUE);
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(135,59);
		tft.print(SupersawSpreadA);
		tft.setCursor(135,78);
		tft.print(SupersawMixA);
			
	}
	else if (PageNr == 2 && (Osc2WaveBank == 0 && oscWaveformB == 3)) {
		print_String(231,85,59);	// print "SPREAD"
		print_String(232,85,78);	// print "SAWMIX"
		tft.fillRoundRect(133,57,22,10,2,ST7735_BLUE);
		tft.fillRoundRect(133,76,22,10,2,ST7735_BLUE);
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(135,59);
		tft.print(SupersawSpreadB);
		tft.setCursor(135,78);
		tft.print(SupersawMixB);
		
	}
	else if (PageNr == 1 && Osc1WaveBank == 0 && (oscWaveformA == 5 || oscWaveformA == 8 || oscWaveformA == 12)) {
		print_String(6,85,59);		// print "PWAMT"
		print_String(7,85,78);		// print "PWMOD"
		tft.fillRoundRect(133,57,22,10,2,ST7735_BLUE);
		tft.fillRoundRect(133,76,22,10,2,ST7735_BLUE);
		tft.setTextColor(ST7735_WHITE);
		uint8_t value = 0;  //!!!!!!!!!!!
		for (uint8_t i = 0; i < 128; i++) {
			float myValue = LINEARCENTREZERO[i];
			if (pwA == myValue) {
				value = i;
				break;
			}
		}
		tft.setCursor(135,59);
		tft.print(value);
		tft.setCursor(135,59);
		printPWMrate(pwmRateA);
	}
	else if (PageNr == 2 && Osc2WaveBank == 0 && (oscWaveformB == 5 || oscWaveformB == 8 || oscWaveformB == 12)) {
		print_String(6,85,59);		// print "PWAMT"
		print_String(7,85,78);		// print "PWMOD"
		tft.fillRoundRect(133,57,22,10,2,ST7735_BLUE);
		tft.fillRoundRect(133,76,22,10,2,ST7735_BLUE);
		tft.setTextColor(ST7735_WHITE);
		uint8_t value = 0;
		for (uint8_t i = 0; i < 128; i++) {
			float myValue = LINEARCENTREZERO[i];
			if (pwB == myValue) {
				value = i;
				break;
			}
		}
		tft.setCursor(135,59);
		tft.print(value);
		tft.setCursor(135,78);
		printPWMrate(pwmRateB);
	}
	else if (PageNr == 1 && (Osc1WaveBank == 15 && oscWaveformA >= 1)) {
		print_String(289,85,59);	// print "PRM_A"
		print_String(290,85,78);	// print "PRM_A"
		tft.fillRoundRect(133,57,22,10,2,ST7735_BLUE);
		tft.fillRoundRect(133,76,22,10,2,ST7735_BLUE);
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(135,59);
		tft.print(SupersawSpreadA);
		tft.setCursor(135,78);
		tft.print(SupersawMixA);
			
	}
	else if (PageNr == 2 && (Osc2WaveBank == 15 && oscWaveformB >= 1)) {
		print_String(289,85,59);	// print "PRM_A"
		print_String(290,85,78);	// print "PRM_A"
		tft.fillRoundRect(133,57,22,10,2,ST7735_BLUE);
		tft.fillRoundRect(133,76,22,10,2,ST7735_BLUE);
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(135,59);
		tft.print(SupersawSpreadB);
		tft.setCursor(135,78);
		tft.print(SupersawMixB);
			
	}
	else {		
		print_String(6,85,59);		// print "PWAMT"
		print_String(7,85,78);		// print "PWMOD"
		tft.fillRoundRect(133,57,22,10,2,ST7735_GRAY);
		tft.fillRoundRect(133,76,22,10,2,ST7735_GRAY);
	}
}

//*************************************************************************
// calc Picup value into Main page and return value
//*************************************************************************

uint8_t VCF_cutoff_picup (void) {
	
	int myval = 0;
	int Frq = float(filterFreq);
	
	for (int i = 0; i < 256; i++) {
		int frequency = FILTERFREQS256[i];
		if (Frq <= frequency){
			myval = i;
			break;
		}
		myval = i;
	}
	return myval >> 1;
}

uint8_t VCF_resonance_picup (void) {
	
	int myval = 0;
	
	// SVF resonancy
	if (myFilter == 1) {
		// calc Resonance SVF
		if (filterRes >= 0.71) {
			float Reso = ((filterRes - 0.71f) / 14.29f);
			for (int i = 0; i < 128; i++) {
				float myresonance = LINEAR[i];
				if ((myresonance - Reso) < 0.000001f) {
					myval = 127;
				}
				else {
					myval = i-1;
					break;
				}
			}
		}
		else {
			myval = 0;
		}
	}
	else {
		// Ladder resonancy
		float Reso = (filterRes / 1.8f);
		for (int i = 0; i < 128; i++) {
			float myresonance = LINEAR[i];
			if ((myresonance - Reso) < 0.00001f) {
				myval = 127;
			}
			else {
				myval = i-1;
				break;
			}
		}
	}
	return myval;
}

//*************************************************************************
// Pot live control into main page
//*************************************************************************
FLASHMEM void read_live_ctrl_values (void) {
	
	uint8_t live_ctrl_val[8] = {0,0,0,0,0,0,0,0};  //!!!!!!!!!!!!!!!!!!FB
		
	// LF1 AMT
	float LFOamt = (oscLfoAmt * FILTERMODMIXERMAX);
	for (int i = 0; i < 128; i++) {
		float LFOoscAmt = LINEARLFO[i];
		if (LFOamt <= LFOoscAmt){
			//LF1_val = i;
			live_ctrl_val[0] = i;
			break;
		}
	}
	
	// LF2 AMT
	LFOamt = (filterLfoAmt * FILTERMODMIXERMAX);
	for (int i = 0; i < 128; i++) {
		float LFOfilterAmt = LINEARLFO[i];
		if (LFOamt <= LFOfilterAmt){
			//LF2_val = i;
			live_ctrl_val[1] = i;
			break;
		}
	}
	
	// LF3 (LFO3 AMT)
	live_ctrl_val[2] = float(Lfo3amt * 127.0f);
	live_ctrl_val[3] = myLFO1RateValue;
	live_ctrl_val[4] = myLFO2RateValue;
	live_ctrl_val[5] = myLFO3RateValue;
	live_ctrl_val[6] = HPF_filterFreq_value >> 1;
	live_ctrl_val[7] = HPF_Res_value;
	
	
	// load Pick up values for User Pots ------------------------------
	clr_pickup();
	int potVal[4];
	potVal[0] = MCP_adc.read(MCP3208::Channel::SINGLE_0);
	potVal[1] = MCP_adc.read(MCP3208::Channel::SINGLE_1);
	potVal[2] = MCP_adc.read(MCP3208::Channel::SINGLE_2);
	potVal[3] = MCP_adc.read(MCP3208::Channel::SINGLE_3);
	
	for (uint8_t i = 0; i < 4; i++) {
		// set PrevValue Pot1
		if (UserPot[i] == 0) {
			PrevValue[i] = 0;
			printRedMarker (i, 0);
			printDataValue (i, 0);
		}
		else if (UserPot[i] == 1) {
			uint8_t val = VCF_cutoff_picup();
			PrevValue[i] = val;
			printPickupValue (i, val, potVal[i] >> 5, 0);
		}
		else if (UserPot[i] == 2) {
			uint8_t val = VCF_resonance_picup();
			PrevValue[i] = val;
			printPickupValue (i, val, potVal[i] >> 5, 0);
		}
		else if (UserPot[i] == 3) {
			PrevValue[i] = live_ctrl_val[0];
			printPickupValue (i, live_ctrl_val[0], potVal[i] >> 5, 0);
		}
		else if (UserPot[i] == 4) {
			PrevValue[i] = live_ctrl_val[1];
			printPickupValue (i, live_ctrl_val[1], potVal[i] >> 5, 0);
		}
		else if (UserPot[i] == 5) {
			PrevValue[i] = live_ctrl_val[2];
			printPickupValue (i, live_ctrl_val[2], potVal[i] >> 5, 0);
		}
		else if (UserPot[i] == 6) {
			PrevValue[i] = live_ctrl_val[3];
			printPickupValue (i, live_ctrl_val[3], potVal[i] >> 5, 0);
		}
		else if (UserPot[i] == 7) {
			PrevValue[i] = live_ctrl_val[4];
			printPickupValue (i, live_ctrl_val[4], potVal[i] >> 5, 0);
		}
		else if (UserPot[i] == 8) {
			PrevValue[i] = live_ctrl_val[5];
			printPickupValue (i, live_ctrl_val[5], potVal[i] >> 5, 0);
		}
		else if (UserPot[i] == 9) {
			PrevValue[i] = live_ctrl_val[6];
			printPickupValue (i, live_ctrl_val[6], potVal[i] >> 5, 0);
		}
		else if (UserPot[i] == 10) {
			PrevValue[i] = live_ctrl_val[7];
			printPickupValue (i, live_ctrl_val[7], potVal[i] >> 5, 0);
		}
	}
}

//*************************************************************************
// draw Main page
//*************************************************************************
FLASHMEM void drawMainPage (void) {
	
	int16_t x1, y1;
	uint16_t w1, h1;
	boolean PickUpflag = false;
	boolean temp_pickupFlag = pickupFlag;
	
	pickupFlag = true;
	
	enableScope(false);

	// clear full screen after menu site ----------------------------------
	if (clearScreenFlag == true) {
		tft.fillScreen(ST7735_BLACK);
		clearScreenFlag = false;
		PickUpflag = true;
	}
	
	// print patch number and name and bank -------------------------------
	if (RefreshMainScreenFlag == true) {
		PickUpflag = true;
		// clear PatchNo
		tft.fillRect(61,8,47,18,ST7735_BLACK);
		// clear Patch Name
		tft.fillRect(0,39,160,18,ST7735_BLACK);
		// print bank nr
		const uint8_t ypos = 22;
		tft.fillRoundRect(44,7,16,20,2, ST7735_RED);
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(&FreeSans9pt7b);
		tft.setTextSize(1);
		tft.setCursor(xposBankNo[currentPatchBank], ypos);
		tft.print(BankNo[currentPatchBank]);
		tft.setTextColor(ST7735_YELLOW);
		tft.setCursor(62, 22);
		int patchnumber = patchNo;
		if (patchnumber < 10) {
			tft.print("00");
			tft.print(patchnumber);
		}
		else if (patchnumber < 100) {
			tft.print("0");
			tft.print(patchnumber);
		}
		else tft.print(patchnumber);
		// Patch Name
		currentPatchName.trim();	// delete spaces
		tft.drawFastHLine(10, 63, tft.width() - 20, ST7735_RED);
		tft.getTextBounds(currentPatchName , 0, 0, &x1, &y1, &w1, &h1); // string width in pixels
		tft.setCursor(80 - (w1 / 2), 52);	// print string in the middle
		tft.setTextColor(ST7735_WHITE);
		tft.println(currentPatchName);
		
		// clr SEQ.Symbol and Voice mode text
		tft.fillRect(98,6,15,20,ST7735_BLACK);
		if (SeqSymbol == true) {
			tft.fillCircle(105,11,5,ST7735_DARKGREEN);
			tft.setTextColor(ST7735_WHITE);
			} else {
			tft.fillCircle(105,11,5,ST7735_GRAY);
			tft.setTextColor(ST77XX_DARKGREY);
		}
		tft.setFont(&Picopixel);
		tft.setCursor(104,13);
		tft.print("S");
		printVoiceMode();
	}
	
	// Main Menu ----------------------------------------------------------
	if (myPageShiftStatus[0] == false) {
		MainShift = false;	//set shift state for scope resolution (hight)
		tft.fillRect(0,65,160,68,ST7735_BLACK);	// clear Scope screen
		tft.setFont(NULL);
		tft.setTextColor(ST7735_GRAY);
		print_String(36,127,112); // print "BANK"
		PickUpflag = true;
	}
	else {
		MainShift = true;	//set shift state for scope resolution (hight)
		tft.fillRect(0,65,160,38,ST7735_BLACK);	// clear Scope screen
	}
	
	// Live_ctrl_values (sub menu) ----------------------------------------
	if (MainShift == true && PickUpflag == true) {
		PickUpflag = false;
		tft.fillRect(0,65,160,68,ST7735_BLACK);	// clear Scope screen
		// print Pot Parameter text
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setCursor(10, 103);
		tft.print(UserPotValue[UserPot[0]]);
		tft.setCursor(49, 103);
		tft.print(UserPotValue[UserPot[1]]);
		tft.setCursor(90, 103);
		tft.print(UserPotValue[UserPot[2]]);
		tft.setCursor(130, 103);
		tft.print(UserPotValue[UserPot[3]]);
		read_live_ctrl_values();
	}
	
	// Mute sound ---------------------------------------------------------
	if (mute_sound == true) {
		tft.fillRect(0,65,160,38,ST7735_BLACK);	// clear Scope screen
		tft.setTextColor(ST7735_WHITE);
		
		for (uint8_t i = 0; i < 8; i++) {
			if (i < 4) {
				tft.drawRect(117 + (i * 10), 7, 8, 8, ST7735_BLUE);
				} else {
				tft.drawRect(117 + ((i-4) * 10), 17, 8, 8, ST7735_BLUE);
			}
		}
		
		if (MainShift == true) {
			tft.fillRoundRect(47,78,67,13,2,ST7735_RED);
			tft.setCursor(52,80);
			tft.setFont(NULL);
			tft.print("Loading...");
			} else {
			tft.fillRoundRect(47,88,67,13,2,ST7735_RED);
			tft.setCursor(52,90);
			tft.setFont(NULL);
			tft.print("Loading...");
		}
		
		enableScope(false);
		} else {
		
		enableScope(true);
		// draw Voices LED
		drawVoiceLED_mainpage();
		pickupFlag = temp_pickupFlag;
	}
}

//*************************************************************************
// draw Osc1 page
//*************************************************************************
FLASHMEM void drawOsc1Page (void)
{
	float myValue = 0;
	int value = 0;
	//int parameter = 0;
	
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	
	if (myPageShiftStatus[PageNr] == false) {	// Osc1 Main Menu
		tft.setCursor(0,0);
		tft.fillRect(0,0,160,13,ST7735_GRAY);
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(0,5,3);			// print "OSC"
		print_String(27,24,3);			// print "1"
		tft.setTextColor(ST7735_GRAY);
		print_String(1,5,21);			// print "WAVE"
		print_String(2,5,40);			// print "PITCH"
		print_String(3,5,59);			// print "P.ENV"
		print_String(4,5,78);			// print "GLIDE"
		print_String(5,5,97);			// print "LEVEL"
		if (Osc1WaveBank == 0 && oscWaveformA == 3) {
			print_String(231,85,59);	// print "SPREAD"
			print_String(232,85,78);	// print "SAWMIX"
		}
		else if (Osc1WaveBank == 0 && (oscWaveformA == 5 || oscWaveformA == 8 || oscWaveformA == 12)){
			print_String(6,85,59);		// print "PWAMT"
			print_String(7,85,78);		// print "PWMOD"
		}
		else if (Osc1WaveBank == 15 && oscWaveformA >= 1) {
			print_String(289,85,59);	// print "PRM_A"
			print_String(290,85,78);	// print "PRM_B"
		}
		print_String(8,85,97);			// print "OSCMIX" 
		tft.setTextColor(ST7735_WHITE);
		print_String(29,5,115);			// print "SELECT"
		print_String(30,50,115);		// print "VALUE"
		print_String(36,95,115);		// print "BANK"
		print_String(31,135,115);		// print "---"
		
		// draw Rect and Lines
		for (uint8_t i = 0; i < 5; i++){
			tft.fillRoundRect(54,19+(19*i),22,10,2,ST7735_BLUE);
		}
		for (uint8_t i = 2; i < 5; i++){
			tft.fillRoundRect(133,19+(19*i),22,10,2,ST7735_BLUE);
		}
		// draw yello arrows
		tft.drawFastVLine(49,98,5,ST7735_YELLOW);
		tft.drawFastVLine(50,99,3,ST7735_YELLOW);
		tft.drawPixel(51,100,ST7735_YELLOW);
		//tft.drawFastVLine(128,79,5,ST7735_YELLOW);
		//tft.drawFastVLine(129,80,3,ST7735_YELLOW);
		//tft.drawPixel(130,81,ST7735_YELLOW);
		tft.drawFastVLine(128,98,5,ST7735_YELLOW);
		tft.drawFastVLine(129,99,3,ST7735_YELLOW);
		tft.drawPixel(130,100,ST7735_YELLOW);
		
		tft.drawFastVLine(0,14,95,ST7735_GRAY);
		tft.drawFastVLine(80,14,95,ST7735_GRAY);
		tft.drawFastVLine(159,14,95,ST7735_GRAY);
		for (uint8_t i = 0; i < 6; i++){
			tft.drawFastHLine(0,14+(19*i),160,ST7735_GRAY);
		}
		tft.drawFastHLine(81,33,78,ST7735_BLACK);
		
		// draw inactive SUB maker
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(35,33,3);		// print "SUB"
		
		// print WaveNr ---------------------------------------------------
		tft.setCursor(56,21);
		tft.setTextColor(ST7735_WHITE);
		tft.print(oscWaveformA);
		draw_Waveform(oscWaveformA, ST7735_RED);
		
		// print WaveBank -------------------------------------------------
		tft.fillRoundRect(34,20,7,10,2,ST7735_RED);
		tft.setCursor(35,21);
		tft.setTextColor(ST7735_WHITE);
		tft.print(char(0x41 + Osc1WaveBank));

		// Pitch ----------------------------------------------------------
		value = oscPitchA;
		tft.setCursor(56,40);
		tft.setTextColor(ST7735_WHITE);
		if (value > 0){
			tft.print("+");
			tft.println(value);
		}
		else tft.print(value);
		
		// Pitch Env ------------------------------------------------------
		printOscPitchEnv(convert_pitchEnv(pitchEnvA));
		

		// Glide ----------------------------------------------------------
		for (uint8_t i = 0; i < 128; i++) {
			myValue = POWER[i];
			if (myValue == glideSpeed) {
				value = i;
				break;
			}
		}
		tft.setCursor(56,78);
		tft.setTextColor(ST7735_WHITE);
		tft.print(value);
		
		// Level ----------------------------------------------------------
		printOscLevel();
		
		// print PWAMT/SPREAD & PWMOD/SAWMIX ------------------------------
		print_quadsaw_pwamt();
		
		// OSCMIX ---------------------------------------------------------
		tft.setCursor(135,97);
		tft.setTextColor(ST7735_WHITE);
		
		if (oscALevel == 1.0000 && oscBLevel == 1.0000) {
			tft.print("0");
		}
		else if (oscBLevel == 1.0) {
			for (uint8_t i = 0; i < 128; i++) {
				float myValue = LINEAR[i];
				if (oscALevel <= myValue) {
					value = i;
					tft.print("+");
					tft.print(63 - (value / 2));
					break;
				}
			}
		}
		else {
			for (uint8_t i = 0; i < 128; i++) {
				float myValue = LINEAR[i];
				if (oscBLevel <= myValue) {
					value = i;
					tft.print("-");
					tft.print(63 - (value / 2));
					break;
				}
			}
		}
		
		// Update parameter -----------------------------------------------
		ParameterNr = ParameterNrMem[1];
		drawParamterFrame(PageNr, ParameterNr);
	}
	
	// Osc1 SUB Menu --------------------------------------------------
	else { drawOscSubMenu();
	}
	
	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw Osc2 page
//*************************************************************************
FLASHMEM void drawOsc2Page (void)
{
	float myValue = 0;
	int value = 0;
	// float Value1;
	// float Value2;	
	
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	
	
	if (myPageShiftStatus[PageNr] == false) {	// Osc2 main page
		tft.setCursor(0,0);
		tft.fillRect(0,0,160,13,ST7735_GRAY);
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(0,5,3);		// print "OSC"
		print_String(28,24,3);		// print "2"
		tft.setTextColor(ST7735_GRAY);
		print_String(1,5,21);		// print "WAVE"
		print_String(2,5,40);		// print "PITCH"
		print_String(3,5,59);		// print "P.ENV"
		print_String(9,5,78);		// print "DETUNE"
		print_String(5,5,97);		// print "LEVEL"
		if (oscWaveformB == 3 && Osc2WaveBank == 0) {
			print_String(231,85,59);	// print "SPREAD"
			print_String(232,85,78);	// print "SAWMIX"
			} else {
			print_String(6,85,59);		// print "PWAMT"
			print_String(7,85,78);		// print "PWMOD"
		}
		print_String(8,85,97);		// print "OSCMIX"
		tft.setTextColor(ST7735_WHITE);
		print_String(29,5,115);		// print "SELECT"
		print_String(30,50,115);	// print "VALUE"
		print_String(36,95,115);	// print "BANK"
		print_String(31,135,115);	// print "---"
		
		// draw Rect and Lines
		for (uint8_t i = 0; i < 5; i++){
			tft.fillRoundRect(54,19+(19*i),22,10,2,ST7735_BLUE);
		}
		for (uint8_t i = 2; i < 5; i++){
			tft.fillRoundRect(133,19+(19*i),22,10,2,ST7735_BLUE);
		}
		
		// draw yello arrows
		tft.drawFastVLine(49,98,5,ST7735_YELLOW);
		tft.drawFastVLine(50,99,3,ST7735_YELLOW);
		tft.drawPixel(51,100,ST7735_YELLOW);
		//tft.drawFastVLine(128,79,5,ST7735_YELLOW);
		//tft.drawFastVLine(129,80,3,ST7735_YELLOW);
		//tft.drawPixel(130,81,ST7735_YELLOW);
		tft.drawFastVLine(128,98,5,ST7735_YELLOW);
		tft.drawFastVLine(129,99,3,ST7735_YELLOW);
		tft.drawPixel(130,100,ST7735_YELLOW);
		
		tft.drawFastVLine(0,14,95,ST7735_GRAY);
		tft.drawFastVLine(80,14,95,ST7735_GRAY);
		tft.drawFastVLine(159,14,95,ST7735_GRAY);
		for (uint8_t i = 0; i < 6; i++){
			tft.drawFastHLine(0,14+(19*i),160,ST7735_GRAY);
		}
		tft.drawFastHLine(81,33,78,ST7735_BLACK);
		
		// draw inactive SUB maker
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(35,33,3);					// print "SUB"
		
		// draw Wave and BankNo -------------------------------------------
		tft.setCursor(56,21);
		tft.setTextColor(ST7735_WHITE);
		tft.print(oscWaveformB);
		draw_Waveform(oscWaveformB, ST7735_ORANGE);
		
		// draw WaveBank
		tft.fillRoundRect(34,20,7,10,2,ST7735_RED);
		tft.setCursor(35,21);
		tft.setTextColor(ST7735_WHITE);
		tft.print(char(0x41 + Osc2WaveBank));
		
		// Pitch (oscPitchB) ----------------------------------------------
		value = (oscPitchB);
		tft.setCursor(56,40);
		tft.setTextColor(ST7735_WHITE);
		if (value > 0){
			tft.print("+");
			tft.println(value);
		}
		else tft.print(value);
		
		// Pitch Env ------------------------------------------------------
		printOscPitchEnv(convert_pitchEnv(pitchEnvB));
		
		// Detune ---------------------------------------------------------
		printVoiceMode();
		
		// Level ----------------------------------------------------------
		printOscLevel();
		
		// print PWAMT/SPREAD & PWMOD/SAWMIX ------------------------------
		print_quadsaw_pwamt();
		
		// OscMix ---------------------------------------------------------
		tft.setCursor(135,97);
		tft.setTextColor(ST7735_WHITE);
		
		if (oscALevel == 1.0000 && oscBLevel == 1.0000) {
			tft.print("0");
		}
		else if (oscBLevel == 1.0) {
			for (uint8_t i = 0; i < 128; i++) {
				myValue = LINEAR[i];
				if (oscALevel <= myValue) {
					value = i;
					tft.print("+");
					tft.print(63 - (value / 2));
					break;
				}
			}
		}
		else {
			for (uint8_t i = 0; i < 128; i++) {
				myValue = LINEAR[i];
				if (oscBLevel <= myValue) {
					value = i;
					tft.print("-");
					tft.print(63 - (value / 2));
					break;
				}
			}
		}
		
		
		// Update parameter -----------------------------------------------
		ParameterNr = ParameterNrMem[2]; // SUB Page
		drawParamterFrame(PageNr, ParameterNr);
	}
	
	// SUB menu -----------------------------------------------------------
	else { drawOscSubMenu();
	}
	
	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// Osc SUB Menu
//*************************************************************************
FLASHMEM void drawOscSubMenu () {
	
	float myValue = 0;
	int value = 0;
	
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setTextColor(ST7735_WHITE);
	tft.setFont(NULL);
	tft.setTextSize(0);
	print_String(0,5,3);			// print "OSC"
	tft.setTextColor(ST7735_GRAY);
	print_String(10,5,21);			// print "WSHAPE"
	print_String(11,5,40);			// print "GAIN"
	print_String(12,5,59);			// print "SYNC"
	print_String(13,5,78);			// print "TRANSP"
	print_String(14,5,97);			// print "TUNE"
	print_String(15,85,59);			// print "NOISE"
	print_String(16,85,78);			// print "OSCMOD"
	print_String(188,85,97);		// print "MODAMT"
	tft.setTextColor(ST7735_WHITE);
	print_String(29,5,115);			// print "SELECT"
	print_String(30,50,115);		// print "VALUE"
	print_String(31,95,115);		// print "---"
	print_String(31,135,115);		// print "---"
	
	// draw Rect and Lines
	for (uint8_t i = 0; i < 5; i++){
		tft.fillRoundRect(54,19+(19*i),22,10,2,ST7735_BLUE);
	}
	for (uint8_t i = 2; i < 5; i++){
		tft.fillRoundRect(133,19+(19*i),22,10,2,ST7735_BLUE);
	}
	tft.drawFastVLine(0,14,95,ST7735_GRAY);
	tft.drawFastVLine(80,14,95,ST7735_GRAY);
	tft.drawFastVLine(159,14,95,ST7735_GRAY);
	for (uint8_t i = 0; i < 6; i++){
		tft.drawFastHLine(0,14+(19*i),160,ST7735_GRAY);
	}
	tft.drawFastHLine(81,33,78,ST7735_BLACK);
	
	// Wafeshaper --------------------------------------------
	tft.setTextColor(ST7735_WHITE);
	if (WShaperNo == 0) {
		print_String(18,56,21);		// print "OFF"
	}
	else {
		tft.setCursor(56,21);
		tft.print(WShaperNo);
	}
	drawWaveshaperCurve(WShaperNo);
	
	// Waveshaper Gain -----------------------------------------------
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(56, 40);
	tft.print(WShaperDrive,1);
	
	// SYNC -----------------------------------------------------------
	tft.fillRoundRect(54,57,22,10,2,ST7735_BLUE);
	tft.setTextColor(ST7735_WHITE);
	if (oscDetuneSync >= 1) {
		print_String(33,59,59);		// print "ON"
		} else {
		print_String(18,56,59);		// print "OFF"
	}
	
	// Transpose ------------------------------------------------------
	tft.setTextColor(ST7735_WHITE);
	if (oscTranspose > 0) {
		print_String(34,56,78);		// print "+"
		tft.setCursor(62,78);
	} else {
		tft.setCursor(56,78);
	}
	tft.print(oscTranspose);
	
	// Tune -----------------------------------------------------------
	tft.setCursor(56,97);
	tft.setTextColor(ST7735_WHITE);
	int val = 0;
	for (uint8_t i = 0; i < 128; i++) {
		float mtune = MASTERTUNE[i];
		if (mtune == oscMasterTune) {
			val = i;
			if (val >= 64) {
				val--;
			}
			break;
		}
	}
	if (val > 63) {
		tft.print("+");
	}
	tft.print(val-63);
	
	// Noise ----------------------------------------------------------
	for (uint8_t i = 0; i < 128; i++) {
		myValue = LINEARCENTREZERO[i];
		if (noiseLevel == myValue) {
			value = i;
			break;
		}
	}
	tft.setCursor(135,59);
	tft.setTextColor(ST7735_WHITE);
	if (value >= 126) {
		value = 126;
	}
	if (value >= 62 && value <= 64) {
		tft.fillRoundRect(133,59,21,8,2,ST7735_BLUE);
		tft.setTextColor(ST7735_WHITE);
		print_String(18,135,59);		// print "OFF"
		tft.fillRect(128,59,5,7,ST7735_BLACK);
	}
	else if (value < 62) {
		tft.println(62 - value);
		tft.fillRect(122,59,5,7,ST7735_BLACK);
		tft.setTextColor(ST7735_WHITE);
		print_String(32,122,59);		// print "W"
	}
	else {
		tft.println(value - 64);
		tft.fillRect(122,59,5,7,ST7735_BLACK);
		tft.setTextColor(ST77XX_MAGENTA);
		print_String(25,122,59);		// print "P"
	}
	
	// OSCMOD ---------------------------------------------------------
	tft.fillRoundRect(133,78,21,8,2,ST7735_BLUE);
	tft.setTextColor(ST7735_WHITE);
	if (oscFX <= 6) {
		print_String((18 + oscFX),135,78);
	} else {
		print_String(247,135,78);
	}
	
	// MODAMT ---------------------------------------------------------
	uint8_t ModAmt = (Osc1ModAmt * 127);
	tft.fillRoundRect(133,97,21,8,2,ST7735_BLUE);
	tft.setCursor(135,97);
	tft.setTextColor(ST7735_WHITE);
	tft.print(ModAmt);
	
	// Update parameter -----------------------------------------------
	ParameterNr = ParameterNrMem[3]; // SUB Page
	drawParamterFrame(PageNr, ParameterNr);
}

//*************************************************************************
// draw State Variable Filter Page
//*************************************************************************
FLASHMEM void draw_StateVariable_Filter_page (void)
{		
	if (myPageShiftStatus[PageNr] == 0) {	// Main page enabled
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(37,5,3);					// print "FILTER"
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(38,5,19);					// print "STATE VARIABLE"
		tft.setTextColor(ST7735_WHITE);
		print_String(228,10,103);				// print "FRQ"
		print_String(41,50,103);				// print "RES"
		print_String(42,90,103);				// print "ENV"
		print_String(43,130,103);				// print "TYP"
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(35,45,3);					// print "SUB"
	}
	else if (myPageShiftStatus[PageNr] == 1){	// SUB page enabled
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(37,5,3);					// print "FILTER"
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(38,5,19);					// print "STATE VARIABLE"
		tft.setTextColor(ST7735_WHITE);
		print_String(44,9,103);					// print "KEY"
		print_String(45,50,103);				// print "VEL"
		print_String(46,90,103);				// print "LFO"
		print_String(31,130,103);				// print "---"
	}
	else if (myPageShiftStatus[PageNr] == 2){	// SUB page enabled
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(37,5,3);					// print "FILTER"
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(229,5,19);					// print "HPF Filter"
		tft.setTextColor(ST7735_WHITE);
		print_String(228,9,103);				// print "FRQ"
		print_String(41,50,103);				// print "RES"
		print_String(31,90,103);				// print "---"
		print_String(230,130,103);				// print "FLT"
	}
	
	
	// draw diagramm line
	tft.drawLine(19,39,19,86,ST7735_GRAY);
	tft.drawLine(19,86,134,86,ST7735_GRAY);
	tft.setTextColor(ST7735_GRAY);
	tft.setCursor(7, 40);
	tft.print("dB");
	tft.setCursor(110, 88);
	tft.print("f/Hz");
	
	uint8_t Frq_value = 0;
	uint8_t FilterReso = 0;
	uint8_t Filter_mix = 0;
	uint8_t FilterKey = 0;
	
	// calc Cutoff frequency
	int Frq = float(filterFreq);
	for (int i = 0; i < 256; i++) {
		int frequency = FILTERFREQS256[i];
		if (Frq <= frequency){
			Frq_value = i;
			break;
		}
	}
	
	// calc Resonance
	if (filterRes >= 0.71) {
		float Reso = ((filterRes - 0.71f) / 14.29f);
		for (int i = 0; i < 128; i++) {
			float myresonance = LINEAR[i];
			if ((myresonance - Reso) < 0.000001f) {
				FilterReso = 127;
			}
			else {
				FilterReso = i-1;
				break;
			}
		}
	}
	else FilterReso = 0;
	
	// calc Key Trakking
	float k_factor = 127;
	FilterKey = (uint8_t)(k_factor * keytrackingAmount);
	
	// calc Filter Typ (Filter-Mix)
	if (filterMix < 0) {
		Filter_mix = 125;
	}
	else {
		Filter_mix = (uint8_t)(128.0f * filterMix);
	}
	
	if (myPageShiftStatus[PageNr] == 2) {
		draw_HPF_filter_curves(HPF_filterFreq_value >> 1, HPF_Res_value);
		} else {
		draw_filter_curves(Frq_value >> 1, FilterReso, Filter_mix);
	}
	
	FilterCut = Frq_value;
	FilterRes = FilterReso;
	FilterMix = Filter_mix;
	
	// Main page enabled ----------------------------------------------
	if (myPageShiftStatus[PageNr] == 0) {
		
		// calc and print Filter Env Amount
		float EnvAmt = filterEnv / FILTERMODMIXERMAX;
		int EnvVal = 0;
		for (int i = 0; i < 128; i++) {
			float FilterEnvAmt = LINEARCENTREZERO[i];
			if (EnvAmt <= FilterEnvAmt) {
				EnvVal = i;
				break;
			}
		}
			
		// print parameters on Main page
		printDataValue (0, Frq_value >> 1);				// value 0...127
		printRedMarker (0, Frq_value >> 1);
		printDataValue (1, FilterReso);					// value 0...127
		printRedMarker (1, FilterReso);					
		print_value_63(EnvVal, 90, 116, ST7735_GRAY);	// value -63...+63
		printRedMarker (2, EnvVal);
		printPickupVCFTyp(3, FilterMix, FilterMix, 0);	// LP/HP/BP
		
	}
	
	// SUB Page enabled
	else if (myPageShiftStatus[PageNr] == 1) {
		
		// calc Filter velocity
		uint8_t myVelo = roundf(myFilVelocity / DIV127);
		
		// calc LFO AMT
		float LFOamt = (filterLfoAmt * FILTERMODMIXERMAX);
		uint8_t Amtvelo = 0;
		for (int i = 0; i < 128; i++) {
			float LFOfilterAmt = LINEARLFO[i];
			if (LFOamt <= LFOfilterAmt){
				Amtvelo = i;
				break;
			}
		}
		
		// print parameter values
		printDataValue (0, FilterKey);
		printRedMarker (0, FilterKey);
		printDataValue (1, myVelo);
		printRedMarker (1, myVelo);
		printDataValue (2, Amtvelo);
		printRedMarker (2, Amtvelo);
		printDataValue (3, 0);
		printRedMarker (3, 0);
	}
	
	// SUB Page enabled (HP-Filter)
	else if (myPageShiftStatus[PageNr] == 2) {
		
		// print parameter values
		printDataValue (0, HPF_filterFreq_value >> 1);
		printRedMarker (0, HPF_filterFreq_value >> 1);
		printDataValue (1, HPF_Res_value);
		printRedMarker (1, HPF_Res_value);		
		printDataValue (2, 0);
		printRedMarker (2, 0);
		
		// Filter Typ
		tft.setTextColor(ST7735_GRAY);
		if (myFilter == 1) {
			printRedMarker (3, 0);
			print_String(146,130,116);	// print "STA"
		}
		else if (myFilter == 2) {
			printRedMarker (3, 127);
			print_String(147,130,116);	// print "LAD"
		}
	}
}

//*************************************************************************
// draw Ladder Filter Page
//*************************************************************************
FLASHMEM void draw_Ladder_Filter_page (void)
{
	if (myPageShiftStatus[PageNr] == 0) {	// Main Page
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(37,5,3);					// print "FILTER"
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(39,5,19);					// print "LADDER VERSION"
		tft.setTextColor(ST7735_WHITE);
		print_String(40,10,103);				// print "CUT"
		print_String(41,50,103);				// print "RES"
		print_String(42,90,103);				// print "ENV"
		print_String(47,130,103);				// print "DRV"
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(35,45,3);					// print "SUB"
	}
	else if (myPageShiftStatus[PageNr] == 1) {	// Filter SUB1 menu
		tft.setTextColor(ST7735_GREEN);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(37,5,3);					// print "FILTER"
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(39,5,19);					// print "LADDER VERSION"
		tft.setTextColor(ST7735_WHITE);
		print_String(44,9,103);					// print "KEY"
		print_String(45,50,103);				// print "VEL"
		print_String(49,90,103);				// print "LFO"
		print_String(48,130,103);				// print "PBG"
	}
	else if (myPageShiftStatus[PageNr] == 2){	// SUB page enabled
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(37,5,3);					// print "FILTER"
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(229,5,19);					// print "HPF Filter"
		tft.setTextColor(ST7735_WHITE);
		print_String(228,9,103);				// print "FRQ"
		print_String(41,50,103);				// print "RES"
		print_String(31,90,103);				// print "---"
		print_String(230,130,103);				// print "FLT"
	}
	
	// draw diagramm line
	tft.drawLine(19,39,19,86,ST7735_GRAY);
	tft.drawLine(19,86,134,86,ST7735_GRAY);
	tft.setTextColor(ST7735_GRAY);
	print_String(50,7,40);						// print dB"
	print_String(51,110,88);					// print "f/Hz"
	
	uint8_t Frq_value = 0;
	uint8_t FilterReso = 0;
	uint8_t Filter_mix = 0;
	uint8_t FilterKey = 0;
	//uint8_t HPF_value = 0;
	//uint8_t HPFRes_value = 0;
	
	// calc Cutoff frequency
	int Frq = float(filterFreq);
	for (int i = 0; i < 256; i++) {
		int frequency = FILTERFREQS256[i];
		if (Frq <= frequency){
			Frq_value = i;
			break;
		}
	}
	
	// calc Resonancy Ladder Filter
	float Reso = (filterRes / 1.8f);
	for (int i = 0; i < 128; i++) {
		float myresonance = LINEAR[i];
		if ((myresonance - Reso) < 0.00001f) {
			FilterReso = 127;
		}
		else {
			FilterReso = i-1;
			break;
		}
	}
	
	// calc Key Trakking
	float k_factor = 127;
	FilterKey = (uint8_t)(k_factor * keytrackingAmount);
	
	Filter_mix = 0;
	
	if (myPageShiftStatus[PageNr] == 2) {
		//draw_HPF_filter_curves(HPF_value >> 1, HPF_Res_value);
		draw_HPF_filter_curves(HPF_filterFreq_value >> 1, HPF_Res_value);
		} else {
		draw_filter_curves(Frq_value >> 1, FilterReso, Filter_mix);
	}
	
	FilterCut = Frq_value;
	FilterRes = FilterReso;
	FilterMix = Filter_mix;
	
	// Filter main Page enabled ---------------------------
	if (myPageShiftStatus[3] == 0) {
		
		// calc and print Filter Env Amount
		float EnvAmt = filterEnv / FILTERMODMIXERMAX;
		int EnvVal = 0;
		for (int i = 0; i < 128; i++) {
			float FilterEnvAmt = LINEARCENTREZERO[i];
			if (EnvAmt <= FilterEnvAmt) {
				EnvVal = i;
				break;
			}
		}
		
		if (LadderFilterDrive <= 1) {
			LadderFilterDrive = 1;
		}
		
		// print parameters on Main page
		printDataValue (0, Frq_value >> 1);				// value 0...127
		printRedMarker (0, Frq_value >> 1);
		printDataValue (1, FilterReso);					// value 0...127
		printRedMarker (1, FilterReso);
		print_value_63(EnvVal, 90, 116, ST7735_GRAY);	// value -63...+63
		printRedMarker (2, EnvVal);
		printDataValue (3, LadderFilterDrive);			// value 0...127
		printRedMarker (3, LadderFilterDrive);
		
	}
	// SUB Page enabled -------------------------------------
	else if (myPageShiftStatus[3] == 1) {
		
		// calc Filter velocity
		uint8_t myVelo = roundf(myFilVelocity / DIV127);
		
		// calc LFO AMT
		float LFOamt = (filterLfoAmt * FILTERMODMIXERMAX);
		uint8_t Amtvelo = 0;
		for (int i = 0; i < 128; i++) {
			float LFOfilterAmt = LINEARLFO[i];
			if (LFOamt <= LFOfilterAmt){
				Amtvelo = i;
				break;
			}
		}
		
		// print parameters on Main page
		printDataValue (0, FilterKey);			// value 0...127
		printRedMarker (0, FilterKey);
		printDataValue (1, myVelo);					// value 0...127
		printRedMarker (1, myVelo);
		printDataValue (2, Amtvelo);					// value 0...127
		printRedMarker (2, Amtvelo);
		printDataValue (3, LadderFilterpassbandgain);	// value 0...127
		printRedMarker (3, LadderFilterpassbandgain);
	}
	
	// SUB Page enabled
	else if (myPageShiftStatus[PageNr] == 2) {
		
		// print parameter values
		printDataValue (0, HPF_filterFreq_value >> 1);
		printRedMarker (0, HPF_filterFreq_value >> 1);
		printDataValue (1, HPF_Res_value);
		printRedMarker (1, HPF_Res_value);
		printDataValue (2, 0);
		printRedMarker (2, 0);
		
		// Filter Typ
		tft.setTextColor(ST7735_GRAY);
		if (myFilter == 1) {
			printRedMarker (3, 0);
			print_String(146,130,116);	// print "STA"
		}
		else if (myFilter == 2) {
			printRedMarker (3, 127);
			print_String(147,130,116);	// print "LAD"
		}
		
	}
	
}

//*************************************************************************
// draw Filter page
//*************************************************************************
FLASHMEM void drawFilterPage (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	
	if (myFilter == 1) {
		draw_StateVariable_Filter_page();
	}
	else if (myFilter == 2) {
		draw_Ladder_Filter_page();
	}
	
	draw_Voice_State();
	MidiStatusSymbol = 2; 
	MidiSymbol();
	drawPeakmeter();		
}

//*************************************************************************
// draw Filter Env Page
//*************************************************************************
FLASHMEM void drawFilterEnvPage (void)
{ 
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setTextColor(ST7735_WHITE);
	tft.setFont(NULL);
	tft.setTextSize(0);
	print_String(52,5,3);		// print "VCF"
	print_String(42,25,3);		// print "ENV"
	tft.fillRoundRect(126,25,21,11,2,ST7735_ORANGE);
	tft.setTextColor(ST7735_WHITE);
	print_String(52,128,27);		// print "VCF"
	tft.setTextColor(ST7735_LIGHTGRAY);
	print_String(35,45,3);			// print "SUB"
	
	// read env time
	for (int i = 0; i < 128; i++) {
		uint16_t timeMS = ENVTIMES[i];
		if (filterAttack <= timeMS){
			Env1Atk = i;
			break;
		}
	}
	for (int i = 0; i < 128; i++) {
		uint16_t timeMS = ENVTIMES[i];
		if (filterDecay <= timeMS){
			Env1Dcy = i;
			break;
		}
	}
	for (int i = 0; i < 128; i++) {
		float timeMS = LINEAR[i];
		if (filterSustain <= timeMS){
			Env1Sus = i;
			break;
		}
	}
	for (int i = 0; i < 128; i++) {
		uint16_t timeMS = ENVTIMES[i];
		if (filterRelease <= timeMS){
			Env1Rel = i;
			break;
		}
	}
		
	// print parameters on Main page
	if (myPageShiftStatus[PageNr] == false) {
		tft.setTextColor(ST7735_WHITE);
		print_String(54,10,103);		// print "ATK"
		print_String(55,50,103);		// print "DCY"
		print_String(56,90,103);		// print "SUS"
		print_String(57,130,103);		// print "REL"
		printDataValue (0, Env1Atk);
		printRedMarker (0, Env1Atk);
		printDataValue (1, Env1Dcy);
		printRedMarker (1, Env1Dcy);
		printDataValue (2, Env1Sus);
		printRedMarker (2, Env1Sus);
		printDataValue (3, Env1Rel);
		printRedMarker (3, Env1Rel);
	}
	// print parameters on Sub page
	else {
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(52,5,3);			// print "VCF"
		print_String(42,25,3);			// print "ENV"
		tft.setTextColor(ST7735_WHITE);
		print_String(58,10,103);		// print "CUR"
		print_String(31,50,103);		// print "---"
		print_String(31,90,103);		// print "---"
		print_String(31,130,103);		// print "---"
		uint8_t Value;
		// print Curve value (+-8)
		Value = 128 - (64 - (envelopeType1 * 7.48f));
		printPickupInt8 (0, Value, Value, 0);
		printDataValue (1, 0);
		printRedMarker (1, 0);
		printDataValue (2, 0);
		printRedMarker (2, 0);
		printDataValue (3, 0);
		printRedMarker (3, 0);
	}
	
	// update Envelope curve, Midi symbol and peakmeter
	drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw Amp Envelope Page
//*************************************************************************
FLASHMEM void drawAmpEnvPage (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setTextColor(ST7735_WHITE);
	tft.setFont(NULL);
	tft.setTextSize(0);
	print_String(53,5,3);			// print "AMP"
	print_String(42,25,3);			// print "ENV"
	
	tft.fillRoundRect(126,25,21,11,2,ST7735_DARKGREEN);
	tft.setTextColor(ST7735_WHITE);
	print_String(53,128,27);		// print "VCA"
	tft.setTextColor(ST7735_LIGHTGRAY);
	print_String(35,45,3);			// print "SUB"
	
	// read envelope times
	for (int i = 0; i < 128; i++) {
		uint16_t timeMS = ENVTIMES[i];
		if (ampAttack <= timeMS){
			Env2Atk = i;
			break;
		}
	}
	for (int i = 0; i < 128; i++) {
		uint16_t timeMS = ENVTIMES[i];
		if (ampDecay <= timeMS){
			Env2Dcy = i;
			break;
		}
	}
	for (int i = 0; i < 128; i++) {
		float timeMS = LINEAR[i];
		if (ampSustain <= timeMS){
			Env2Sus = i;
			break;
		}
	}
	for (int i = 0; i < 128; i++) {
		uint16_t timeMS = ENVTIMES[i];
		if (ampRelease <= timeMS){
			Env2Rel = i;
			break;
		}
	}
	
	
	// print parameters on Main page
	if (myPageShiftStatus[PageNr] == false) {
		tft.setTextColor(ST7735_WHITE);
		print_String(54,10,103);		// print "ATK"
		print_String(55,50,103);		// print "DCY"
		print_String(56,90,103);		// print "SUS"
		print_String(57,130,103);		// print "REL"
		printDataValue (0, Env2Atk);
		printRedMarker (0, Env2Atk);
		printDataValue (1, Env2Dcy);
		printRedMarker (1, Env2Dcy);
		printDataValue (2, Env2Sus);
		printRedMarker (2, Env2Sus);
		printDataValue (3, Env2Rel);
		printRedMarker (3, Env2Rel);
	}
	// print parameters on Sub page
	else {
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(53,5,3);			// print "AMP"
		print_String(42,25,3);			// print "ENV"
		tft.setTextColor(ST7735_WHITE);
		print_String(58,10,103);		// print "CUR"
		print_String(45,50,103);		// print "VEL"
		print_String(246,90,103);		// print "PAN"
		print_String(31,130,103);		// print "---"
		uint8_t Value;
		// print Curve value (+-8)
		Value = 128 - (64 - (envelopeType2 * 7.48f));
		printPickupInt8 (0, Value, Value, 0);
		// print Velocity value	(0-127)	
		Value = myAmpVelocity * 254;
		printDataValue (1, Value);
		printRedMarker (1, Value);
		// print Panorama value (0-127)
		printDataValue (2, pan_value);
		printRedMarker (2, pan_value);
		// non
		printDataValue (3, 0);
		printRedMarker (3, 0);
	}
	
	// update Envelope curve, Midi symbol and peakmeter
	drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw FxDSP Page
//*************************************************************************
FLASHMEM void drawFxDspPage (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setTextColor(ST7735_WHITE);
	tft.setFont(NULL);
	tft.setTextSize(0);
	print_String(59,5,3);				// print "FX DSP"
	tft.setTextColor(ST7735_WHITE);
	print_String(60,10,103);			// print "SEL"
	print_String(61,50,103);			// print "VAL"
	print_String(62,90,103);			// print "MIX"
	print_String(63,130,103);			// print "PRG"
	tft.setTextColor(ST7735_GRAY);
	print_String(110,5,88);				// print "P1"
	print_String(111,58,88);			// print "P2"
	print_String(112,112,88);			// print "P3"
	tft.drawRect(0,14,160,86,ST7735_GRAY);
	tft.drawFastHLine(0,83,160,ST7735_GRAY);
	tft.drawFastVLine(53,83,16,ST7735_GRAY);
	tft.drawFastVLine(107,83,16,ST7735_GRAY);
	tft.fillRoundRect(22,86,25,10,2,ST7735_BLUE);
	tft.fillRoundRect(76,86,25,10,2,ST7735_BLUE);
	tft.fillRoundRect(128,86,25,10,2,ST7735_BLUE);
	tft.setTextColor(ST7735_GRAY);
	print_String(64,5,28);				// print "Vol"
	print_String(65,75,72);				// print "Time/Feedb."
	if (FxPrgNo >= 1) {
		FxMixValue = _FxMixValue;
		drawFxGrafic(FxMixValue,FxTimeValue,FxFeedbackValue, FxPot3value);
		printFxPotValue(0, FxPot1value);
		printFxPotValue(1, FxPot2value);
		printFxPotValue(2, FxPot3value);
		printFxValFrame(myFxSelValue);
		printDataValue (0, (myFxSelValue) + 1);
		printRedMarker (0, myFxSelValue << 6);
		printDataValue (1, myFxValValue);
		printRedMarker (1, myFxValValue);
		printDataValue (2, FxMixValue);
		printRedMarker (2, FxMixValue);
		printDataValue (3, FxPrgNo);	// Fx PrgNo 1-16
		printRedMarker (3, (FxPrgNo << 3));
		printFxName(FxPrgNo);
		printFxPOT(myFxSelValue, FxPrgNo);
	}
	else {
		setFxPrg (0);
		FxMixValue = 0;	
		analogWrite(PWM4, FxMixValue);	// Fx MIX off
		tft.fillRect(65,27,90,9,ST7735_BLACK);
		tft.setTextColor(ST7735_RED);
		tft.setCursor(65,28);
		tft.print("OFF");
		tft.fillRect(122, 120, 30, 7, ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(130,116);
		tft.print("OFF");
		printFxPotValue(0, 0);
		printFxPotValue(1, 0);
		printFxPotValue(2, 0);
		printDataValue (0, (myFxSelValue) + 1);
		printRedMarker (0, myFxSelValue << 6);
		printDataValue (1, 0);
		printRedMarker (1, 0);
		printDataValue (2, 0);
		printRedMarker (2, 0);
		printRedMarker (3, 0);
		drawFxGrafic (0, 0, 0, 0);
		//setLED(1, false);
		tft.fillRect(3,71,155,10,ST7735_BLACK);
		tft.setCursor(75,72);
		tft.setTextColor(ST7735_GRAY);
		tft.print("Time/Feedb.");
	}
	
	tft.setCursor(65,40);
	if (myFxSelValue == 3) {
		tft.setTextColor(ST7735_BLACK);
	} else {
		tft.setTextColor(ST7735_GRAY);
	}
	tft.print("Clk");
	tft.setCursor(85,40);
	uint16_t fxRate = (FxClkRate / 1000);
	tft.print(fxRate);
	fxRate = (FxClkRate % 1000);
	fxRate = (fxRate / 100);
	tft.setCursor(96,40);
	tft.print(".");
	tft.setCursor(103,40);
	tft.print(fxRate);
	tft.setCursor(110,40);
	tft.print("KHz");
	
	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw LFO1 Page
//*************************************************************************
FLASHMEM void drawLFO1Page (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setTextColor(ST7735_WHITE);
	tft.setFont(NULL);
	tft.setTextSize(0);
	print_String(115,5,3);				// print "LFO"
	print_String(27,24,3);				// print "1"
	tft.setTextColor(ST7735_LIGHTGRAY);
	print_String(35,33,3);				// print "SUB"
	drawLFOwaveform(oscLFOWaveform, ST7735_RED);
	
	// LFO Shape frame
	uint8_t frame_no = oscLFOWaveform % 6;
	drawLFOframe(frame_no);
	
	// LFO1 main page -----------------------------------------------------
	if (myPageShiftStatus[PageNr] == false) {
		
		tft.setTextColor(ST7735_WHITE);
		print_String(116,3,103);			// print "SHAPE"
		print_String(165,47,103);			// print "RATE"
		print_String(166,90,103);			// print "AMT"
		print_String(167,130,103);			// print "SYN"
		
		// LFO1 Shape
		printDataValue (0, (oscLFOWaveform + 1));
		printRedMarker (0, (oscLFOWaveform * 2));
		
		// LFO1 Rate
		printRedMarker (1, myLFO1RateValue);
		printDataValue (1, myLFO1RateValue);
		
		// LFO AMT
		float LFOamt = (oscLfoAmt * FILTERMODMIXERMAX);
		uint8_t value = 0;
		for (int i = 0; i < 128; i++) {
			float LFOoscAmt = LINEARLFO[i];
			if (LFOamt <= LFOoscAmt){
				value = i;
				break;
			}
		}
		printDataValue (2, value);
		printRedMarker (2, value);
		
		// LFO1 Trigger (Sync)
		if (oscLfoRetrig == 0) {
			tft.setCursor(130, 116);
			tft.print("OFF");
			printRedMarker (3, 0);
		}
		else if (oscLfoRetrig == 1 ) {
			tft.setCursor(134, 116);
			tft.print("0");
			tft.drawRect(141,116,3,3,ST7735_GRAY);
			printRedMarker (3, 64);
		}
		else {
			tft.setCursor(128, 116);
			tft.print("180");
			tft.drawRect(147,116,3,3,ST7735_GRAY);
			printRedMarker (3, 128);
		}
	}
	
	// LFO1 sub page ------------------------------------------------------
	else {
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(115,5,3);				// print "LFO"
		print_String(27,24,3);				// print "1"
		print_String(35,33,3);				// print "SUB"
		tft.setTextColor(ST7735_WHITE);
		print_String(168,6,103);			// print "MODE"
		print_String(172,46,103);			// print "F-IN"
		print_String(169,83,103);			// print "F-OUT"
		print_String(58,130,103);			// print "CUR"
		
		// MODE
		tft.setTextColor(ST7735_GRAY);
		if (LFO1mode == 0) {
			print_String(170,6,116);		// print "CONT"
			printRedMarker (0, 0);
		}
		else {
			print_String(171,8,116);		// print "ONE"
			printRedMarker (0, 127);
		}
		
		// FADE-IN
		uint8_t value = (LFO1fadeTime / 94);
		tft.fillRect(43,116,29,8,ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		uint8_t xpos = 50;
		uint8_t ypos = 116;
		printEnvValue (value, xpos, ypos);
		printRedMarker(1,value);
		
		// FADE-OUT
		value = (LFO1releaseTime / 94);
		tft.fillRect(83,116,29,8,ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		xpos = 90;
		ypos = 116;
		printEnvValue (value, xpos, ypos);
		printRedMarker(2,value);
		
		// print CUR value
		int8_t curVal = (LFO1envCurve * 7.47f);
		curVal += 64;
		printRedMarker (3, curVal);
		
		if (LFO1envCurve < 0) {
			tft.fillRect(131,116,20,8,ST7735_BLACK);
			tft.setCursor(133,116);
			tft.print(LFO1envCurve);
		}
		else if (LFO1envCurve == 0) {
			tft.fillRect(131,116,20,8,ST7735_BLACK);
			tft.setCursor(135,116);
			tft.print(LFO1envCurve);
		}
		else if (LFO1envCurve > 0) {
			tft.fillRect(131,116,20,8,ST7735_BLACK);
			tft.setCursor(133,116);
			tft.print("+");
			tft.print(LFO1envCurve);
		}
	}
	
	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw LFO2 Page
//*************************************************************************
FLASHMEM void drawLFO2Page (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setTextColor(ST7735_WHITE);
	tft.setFont(NULL);
	tft.setTextSize(0);
	print_String(115,5,3);				// print "LFO"
	print_String(28,24,3);				// print "2"
	tft.setTextColor(ST7735_LIGHTGRAY);
	print_String(35,33,3);				// print "SUB"
	drawLFOwaveform(filterLfoWaveform,ST7735_ORANGE);
	
	// LFO Shape frame
	uint8_t frame_no = filterLfoWaveform % 6;
	drawLFOframe(frame_no);
	
	// LFO2 main page -----------------------------------------------------
	if (myPageShiftStatus[PageNr] == false) {
		
		tft.setTextColor(ST7735_WHITE);
		print_String(116,3,103);			// print "SHAPE"
		print_String(165,47,103);			// print "RATE"
		print_String(166,90,103);			// print "AMT"
		print_String(167,130,103);			// print "SYN"
		
		// LFO2 Shape
		printDataValue (0, (filterLfoWaveform + 1));
		printRedMarker (0, (filterLfoWaveform * 2));
		
		// LFO2 Rate
		printRedMarker (1, myLFO2RateValue);
		printDataValue (1, myLFO2RateValue);
		
		// LFO AMT
		float LFOamt = (filterLfoAmt * FILTERMODMIXERMAX);
		uint8_t value = 0;
		for (int i = 0; i < 128; i++) {
			float LFOfilterAmt = LINEARLFO[i];
			if (LFOamt <= LFOfilterAmt){
				value = i;
				break;
			}
		}
		printDataValue (2, value);
		printRedMarker (2, value);
		
		
		// LFO2 Sync
		if (filterLfoRetrig == 3 && MidiSyncSwitch == false) {
			tft.fillRect(46, 116, 24, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_RED);
			tft.setCursor(46,116);
			tft.print("CLK?");
		}
		
		// LFO2 Trigger (Sync)
		if (filterLfoRetrig == 0) {
			tft.setCursor(130, 116);
			tft.print("OFF");
			printRedMarker (3, 0);
		}
		else if (filterLfoRetrig == 1 ) {
			tft.setCursor(134, 116);
			tft.print("0");
			tft.drawRect(141,116,3,3,ST7735_GRAY);
			printRedMarker (3, 64);
		}
		else {
			tft.setCursor(128, 116);
			tft.print("180");
			tft.drawRect(147,116,3,3,ST7735_GRAY);
			printRedMarker (3, 128);
		}
	}
	
	// LFO2 sub page ------------------------------------------------------
	else {
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(115,5,3);				// print "LFO"
		print_String(28,24,3);				// print "2"
		print_String(35,33,3);				// print "SUB"
		tft.setTextColor(ST7735_WHITE);
		print_String(168,6,103);			// print "MODE"
		print_String(172,46,103);			// print "F-IN"
		print_String(169,83,103);			// print "F-OUT"
		print_String(58,130,103);			// print "CUR"
		
		// MODE
		tft.setTextColor(ST7735_GRAY);
		if (LFO2mode == 0) {
			print_String(170,6,116);		// print "CONT"
			printRedMarker (0, 0);
		}
		else {
			print_String(171,8,116);		// print "ONE"
			printRedMarker (0, 127);
		}
		
		// FADE-IN
		uint8_t value = (LFO2fadeTime / 94);
		tft.fillRect(43,116,29,8,ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		uint8_t xpos = 50;
		uint8_t ypos = 116;
		printEnvValue (value, xpos, ypos);
		printRedMarker(1,value);
		
		// FADE-OUT
		value = (LFO2releaseTime / 94);
		tft.fillRect(83,116,29,8,ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		xpos = 90;
		ypos = 116;
		printEnvValue (value, xpos, ypos);
		printRedMarker(2,value);
		
		// print CUR value
		int8_t curVal = (LFO2envCurve * 7.47f);
		curVal += 64;
		printRedMarker (3, curVal);
		
		if (LFO2envCurve < 0) {
			tft.fillRect(131,116,20,8,ST7735_BLACK);
			tft.setCursor(133,116);
			tft.print(LFO2envCurve);
		}
		else if (LFO2envCurve == 0) {
			tft.fillRect(131,116,20,8,ST7735_BLACK);
			tft.setCursor(135,116);
			tft.print(LFO2envCurve);
		}
		else if (LFO2envCurve > 0) {
			tft.fillRect(131,116,20,8,ST7735_BLACK);
			tft.setCursor(133,116);
			tft.print("+");
			tft.print(LFO2envCurve);
		}
	}
	
	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw LFO3 Page
//*************************************************************************
FLASHMEM void drawLFO3Page (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setTextColor(ST7735_WHITE);
	tft.setFont(NULL);
	tft.setTextSize(0);
	print_String(115,5,3);				// print "LFO"
	print_String(202,24,3);				// print "3"
	tft.setTextColor(ST7735_LIGHTGRAY);
	print_String(35,33,3);				// print "SUB"
	drawLFOwaveform(Lfo3Waveform,ST77XX_MAGENTA);
	
	// LFO Shape frame
	uint8_t frame_no = Lfo3Waveform % 6;
	drawLFOframe(frame_no);
	
	// LFO3 main page -----------------------------------------------------
	if (myPageShiftStatus[PageNr] == false) {
		
		tft.setTextColor(ST7735_WHITE);
		print_String(116,3,103);			// print "SHAPE"
		print_String(165,47,103);			// print "RATE"
		print_String(166,90,103);			// print "AMT"
		print_String(167,130,103);			// print "SYN"
		
		// LFO Waveform
		printDataValue (0, (Lfo3Waveform + 1));
		printRedMarker (0, (Lfo3Waveform * 2));
		
		// LFO3 Rate
		printDataValue (1, myLFO3RateValue);
		printRedMarker (1, myLFO3RateValue);
				
		// LFO AMT
		uint8_t value = Lfo3amt / DIV127;
		printDataValue (2, value);
		printRedMarker (2, value);
		
		// LFO Trigger (Sync)
		if (Lfo3Retrig == 0) {
			tft.setCursor(130, 116);
			tft.print("OFF");
			printRedMarker (3, 0);
		}
		else if (Lfo3Retrig == 1 ) {
			tft.setCursor(134, 116);
			tft.print("0");
			tft.drawRect(141,116,3,3,ST7735_GRAY);
			printRedMarker (3, 64);
		}
		else {
			tft.setCursor(128, 116);
			tft.print("180");
			tft.drawRect(147,116,3,3,ST7735_GRAY);
			printRedMarker (3, 128);
		}
	}
	
	// LFO3 sub page ------------------------------------------------------
	else {
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(115,5,3);				// print "LFO"
		print_String(202,24,3);				// print "3"
		print_String(35,33,3);				// print "SUB"
		tft.setTextColor(ST7735_WHITE);
		print_String(168,6,103);			// print "MODE"
		print_String(172,46,103);			// print "F-IN"
		print_String(169,83,103);			// print "F-OUT"
		print_String(58,130,103);			// print "CUR"
		
		// MODE
		tft.setTextColor(ST7735_GRAY);
		if (LFO3mode == 0) {
			print_String(170,6,116);		// print "CONT"
			printRedMarker (0, 0);
		}
		else {
			print_String(171,8,116);		// print "ONE"
			printRedMarker (0, 127);
		}
		
		// FADE-IN
		uint8_t value = (LFO3fadeTime / 94);
		tft.fillRect(43,116,29,8,ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		uint8_t xpos = 50;
		uint8_t ypos = 116;
		printEnvValue (value, xpos, ypos);
		printRedMarker(1,value);
		
		// FADE-OUT
		value = (LFO3releaseTime / 94);
		tft.fillRect(83,116,29,8,ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		xpos = 90;
		ypos = 116;
		printEnvValue (value, xpos, ypos);
		printRedMarker(2,value);
		
		// print CUR value
		int8_t curVal = (LFO3envCurve * 7.47f);
		curVal += 64;
		printRedMarker (3, curVal);
		
		if (LFO3envCurve < 0) {
			tft.fillRect(131,116,20,8,ST7735_BLACK);
			tft.setCursor(133,116);
			tft.print(LFO3envCurve);
		}
		else if (LFO3envCurve == 0) {
			tft.fillRect(131,116,20,8,ST7735_BLACK);
			tft.setCursor(135,116);
			tft.print(LFO3envCurve);
		}
		else if (LFO3envCurve > 0) {
			tft.fillRect(131,116,20,8,ST7735_BLACK);
			tft.setCursor(133,116);
			tft.print("+");
			tft.print(LFO3envCurve);
		}
	}
	
	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw SEQUENCER Page
//*************************************************************************
FLASHMEM void drawSEQPage (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	
	// draw matrix
	tft.fillRect(0,14,160,86,ST7735_GRAY);
	tft.fillRect(15,22+3,143,71,ST77XX_DARKGREY);
	
	
	// Note raster
	tft.fillRect(15,22+3,9,71,ST7735_BLACK);
	tft.fillRect(15+(4*9),22+3,9,71,ST7735_BLACK);
	tft.fillRect(15+(8*9),22+3,9,71,ST7735_BLACK);
	tft.fillRect(15+(12*9),22+3,9,71,ST7735_BLACK);
	
	for (uint8_t i = 0; i < 15; i++) {
		tft.drawFastVLine(23 + (i * 9),21+4,71,ST7735_GRAY);
	}
	
	// draw Keys C-H
	tft.fillRect(2,22+3,12,71,ST7735_WHITE);
	tft.fillRect(2,22+6+3,11,4,ST7735_BLACK);	// C#
	tft.fillRect(2,22+18+3,11,4,ST7735_BLACK);	// D#
	tft.fillRect(2,22+30+3,11,4,ST7735_BLACK);	// D#
	tft.fillRect(2,22+48+3,11,4,ST7735_BLACK);	// D#
	tft.fillRect(2,22+60+3,11,4,ST7735_BLACK);	// D#
	
	for (uint8_t i = 0; i < 11; i++) {
		tft.drawFastHLine(1,27+3 + (i * 6),157,ST7735_GRAY);
	}
	
	if (myPageShiftStatus[PageNr] == false) {
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(117,5,3);			// print "SEQ"
		
		// draw inactive SUB maker
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(35,26,3);			// print "SUB"
		
		} else {
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		print_String(117,5,3);			// print "SEQ"
	}
	tft.setTextColor(ST7735_GREEN);
	print_String(118,65,3);			// print "PAT:"
	if (SEQPatternNo == 0) {
		print_String(119,90,3);			// print "--"
		} else {
		if (SEQPatternNo < 10) {
			print_String(152,90,3);			// print "0"
			tft.setCursor(96,3);
			tft.print(SEQPatternNo);
			} else {
			tft.setCursor(90,3);
			tft.print(SEQPatternNo);
		}
	}
	
	// draw all 16 Note Steps
	for (uint8_t i = 0; i < 16; i++) {
		drawSEQpitchValue2(i);
	}
	
	// draw yellow Step frame
	drawSEQStepFrame(SEQselectStepNo);
	
	// draw number of steps
	drawSEQstepNumbersMarker(SEQstepNumbers);
	
	// Main Page ---------------------------------------------------
	if (myPageShiftStatus[PageNr] == false) {
		tft.setTextColor(ST7735_WHITE);
		print_String(121,7,103);			// print "STEP"
		print_String(122,43,103);			// print "PITCH"
		print_String(123,89,103);			// print "BPM"
		print_String(124,130,103);			// print "BPM"
		printDataValue (0, SEQselectStepNo + 1);
		printRedMarker (0, SEQselectStepNo * 8);
		printSeqPitchNote();
		if (SEQbpmValue > 0) {
			printDataValue (2, SEQbpmValue);
			printRedMarker (2, ((SEQbpmValue - 50)/1.484f));
		} else {
			tft.setTextColor(ST7735_GRAY);
			print_String(148, 89, 116);		// print "EXT"
			printRedMarker (2, 0);
		}
		// print DIV value int Clock
		float DivVal = 0;
		for (uint8_t i = 0; i < 13; i++) {
			DivVal = SeqDivValues[i];
			if (DivVal == SEQdivValue) {
				SEQdividerValue = i;
				break;
			}
		}
		printSEQclkDiv(SEQdividerValue);
		printRedMarker (3, (SEQdividerValue * 9.84615f));
	}
	// SUB Page ---------------------------------------------------
	else {
		tft.setTextColor(ST7735_WHITE);
		print_String(126,10,103);		// print "LEN"
		print_String(127,46,103);		// print "TIME"
		print_String(128,89,103);		// print "DIR"
		print_String(129,126,103);		// print "MODE"
		
		// calc GateTime
		uint8_t gateTimeVal = 0;
		for (uint8_t i = 0; i < 64; i++) {
			if ((16.01f - SEQGateTime) >= SEQGATETIME[i]) {
				gateTimeVal = i + 1;
			}
		}
		printDataValue (0, SEQstepNumbers + 1);
		printRedMarker (0, SEQstepNumbers << 3);
		printDataValue (1, gateTimeVal);
		printRedMarker (1, (gateTimeVal << 1));
		printSEQdirection();
		printRedMarker(2, SEQdirection * 32);
		
		// print REC Mode
		if (SEQmode == 0) {
			tft.setTextColor(ST7735_GRAY);
			print_String(130,130,116);		// print "KEY"
			printRedMarker (3, 0);
		}
		else if (SEQmode == 1){
			tft.setTextColor(ST7735_GRAY);
			print_String(131,130,116);		// print "TRP"
			printRedMarker (3, 63);
		}
		else {
			tft.setTextColor(ST7735_GRAY);
			print_String(132,130,116);		// print "REC"
			printRedMarker (3, 127);
		}
	}

	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw Modulationsmatrix
//*************************************************************************
FLASHMEM void drawModMatrixPage (void) {
	
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setTextColor(ST7735_WHITE);
	tft.setFont(NULL);
	tft.setTextSize(0);
	print_String(173,5,3);				// print "MODULATION"
	tft.fillRect(0,14,160,18,ST7735_GRAY);
	tft.setFont(&Picopixel);
	tft.setTextColor(ST7735_BLACK);
	print_String(174,3,24);				// print "SLOT"
	print_String(175,26,24);			// print "SOURCE"
	print_String(177,75,24);			// print "AMT"
	print_String(176,107,24);			// print "DESTINATION"
	tft.setFont(NULL);
	tft.setTextColor(ST7735_WHITE);
	print_String(174,10,115);			// print "SLOT"
	print_String(177,52,115);			// print "AMT"
	print_String(31,95,115);			// print "---"
	print_String(31,135,115);			// print "---"
	
	ParameterNr = ParameterNrMem[PageNr];
	printModParameter(ParameterNr);
	readModMatrixParameter (ParameterNr);
	uint8_t ParNo = ParameterNr % 4;
	drawParamterFrame2(PageNr, ParNo);
	
	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// print Parameter 1-5 in System main page
//*************************************************************************
FLASHMEM void drawSystemPar1 (void)
{
	for (uint8_t i = 0; i < 5; i++){
		tft.fillRect(83,19+(19*i),45,10,ST7735_BLACK);
	}
	
	tft.setTextColor(ST7735_GRAY);
	print_String(140,85,21);	// print "MIDICHA"
	print_String(141,85,40);	// print "VELCURV"
	print_String(114,85,59);	// print " "
	print_String(114,85,78);	// print " "
	print_String(144,85,97);	// print "PCHANGE"
	
	for (uint8_t i = 0; i < 5; i++){
		tft.fillRoundRect(133,19+(19*i),22,10,2,ST7735_BLUE);
	}
	
	tft.setTextColor(ST7735_WHITE);
	
	// Midi Channel
	if (midiChannel == 0) {
		print_String(145,136,21);	// print "ALL"
	}
	else {
		tft.setCursor(137,21);
		tft.print(midiChannel);
	}
	
	// print Velocity
	tft.setCursor(137, 40);
	tft.print(velocitySens + 1);
	
	
	// Free
	/*
	if (pickupFlag == true) {
		print_String(114,138,59);	// print " "
	}
	*/
	
	
	// free
	/*
	if (MidiSyncSwitch == true) {
		print_String(114,136,78);	// print " "
	}
	*/
	
	// PRGchange
	if (PrgChangeSW == false) {
		print_String(18,136,97);	// print "OFF"
		} else {
		tft.setCursor(138,97);
		tft.print("ON");
		print_String(33,138,97);	// print "ON"
	}
}

//*************************************************************************
// print Parameter values POT1-POT4 in System Main Page
//*************************************************************************
FLASHMEM void print_UserPOT (uint8_t potnumber)
{
	uint8_t xpos = 136;
	uint8_t ypos = 21 + (potnumber * 19);
	
	tft.setCursor(xpos, ypos);
	tft.setTextColor(ST7735_WHITE);
	tft.setTextSize(1); //!!!!!!!!!!!FB
	tft.print(UserPotValue[UserPot[potnumber]]);
	
	UserPot1 = UserPot[0];
	UserPot2 = UserPot[1];
	UserPot3 = UserPot[2];
	UserPot4 = UserPot[3];
}

//*************************************************************************
// draw System Parameter 5-10
//*************************************************************************
FLASHMEM void drawSystemPar2 (void)
{
	for (uint8_t i = 0; i < 5; i++){
		tft.fillRect(83,19+(19*i),45,10,ST7735_BLACK);
	}
	
	tft.setTextColor(ST7735_GRAY);
	print_String(222,85,21);		// print "CTRL1"
	print_String(223,85,40);		// print "CTRL2"
	print_String(224,85,59);		// print "CTRL3"
	print_String(225,85,78);		// print "CTRL4"
	print_String(234,85,97);		// print "UNISONO"
	
	for (uint8_t i = 0; i < 5; i++){
		tft.fillRoundRect(133,19+(19*i),22,10,2,ST7735_BLUE);
	}
	
	// POT1 - POT4
	print_UserPOT(0);
	print_UserPOT(1);
	print_UserPOT(2);
	print_UserPOT(3);
	
	// Unisono Mode
	printVoiceMode();
}


//*************************************************************************
// draw System Page 12
//*************************************************************************
FLASHMEM void drawSystemPage (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setTextColor(ST7735_WHITE);
	tft.setFont(NULL);
	tft.setTextSize(0);
	
	// Main page ----------------------------------------------------------
	if (myPageShiftStatus[PageNr] == false) {
		tft.setTextColor(ST7735_WHITE);
		print_String(190,5,3);			// print "SYSTEM"
		print_String(29,5,115);			// print "SELECTL"
		print_String(30,50,115);		// print "VALUE"
		print_String(31,95,115);		// print "---"
		print_String(31,135,115);		// print "---"
		// draw inactive SUB maker
		tft.setTextColor(ST7735_LIGHTGRAY);
		print_String(35,45,3);			// print "SUB"
		tft.setTextColor(ST7735_GRAY);
		print_String(133,5,21);			// print "VER 1.xx"
		print_String(136,5,40);			// print "TEMP"
		print_String(137,5,59);			// print "LOAD"
		print_String(135,5,78);			// print "CPU"
		print_String(134,5,97);			// print "MEM"
		
		// draw Grafic
		for (uint8_t i = 0; i < 5; i++){
			tft.fillRoundRect(133,19+(19*i),22,10,2,ST7735_BLUE);
		}
		tft.drawFastVLine(0,14,95,ST7735_GRAY);
		tft.drawFastVLine(80,14,95,ST7735_GRAY);
		tft.drawFastVLine(159,14,95,ST7735_GRAY);
		for (uint8_t i = 0; i < 6; i++){
			tft.drawFastHLine(0,14+(19*i),160,ST7735_GRAY);
		}
		
		// read CPU Speed
		int CPUclk = (F_CPU_ACTUAL);
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(40, 78);
		tft.print(CPUclk / 1000000);
		
		// read Audio Memory
		CPUmem = AudioMemoryUsageMax();
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(65, 97);
		// calc Mem
		uint8_t calcmem = CPUmem * 0.78125f;
		tft.print(calcmem);
		tft.setCursor(69,97);
		tft.print("%");
		
		// CPU Temp
		printTemperature();
		
		// CPU Mem
		printCPUmon();
		
		// print adjustable System Parameter
		ParameterNr = ParameterNrMem[PageNr];
		if (ParameterNr <= 4) {
			drawSystemPar1();
			drawParamterFrame(PageNr, ParameterNr);
			} else {
			drawSystemPar2();
			drawParamterFrame(PageNr, ParameterNr - 5);
		}
	}
	
	// SUB page (SysEx Dump) ----------------------------------------------
	else {
		tft.setTextColor(ST7735_WHITE);
		print_String(190,5,3);		// print "SYSTEM"
		tft.setTextColor(ST7735_GREEN);
		print_String(189,70,3);		// print "DUMP"
		for (uint8_t i = 0; i < 5; i++){
			tft.fillRoundRect(123,19+(19*i),31,10,2,ST7735_BLUE);
		}
		tft.drawFastVLine(0,14,95,ST7735_GRAY);
		//tft.drawFastVLine(80,14,95,ST7735_GRAY);
		tft.drawFastVLine(159,14,95,ST7735_GRAY);
		for (uint8_t i = 0; i < 6; i++){
			tft.drawFastHLine(0,14+(19*i),160,ST7735_GRAY);
		}
		tft.setTextColor(ST7735_WHITE);
		print_String(29,5,115);		// print "SELECTL"
		print_String(30,50,115);	// print "VALUE"
		print_String(31,95,115);	// print "---"
		print_String(31,135,115);	// print "---"
		
		tft.setTextColor(ST7735_GRAY);
		print_String(191,5,21);		// print "DESTINATION"
		print_String(192,5,40);		// print "DUMP-TYP"
		print_String(193,5,59);		// print "DUMP-BANK"
		print_String(194,5,78);		// print "SOURCE"
		print_String(195,5,97);		// print "SYSEX-DUMP"
		
		// print Destination value
		tft.setTextColor(ST7735_WHITE);
		if (sysexDest == 0) {
			print_String(196,130,21); // print "USB"
		}
		else {
			print_String(125,128,21); // print "MIDI"
		}
		// print DUMP-TYP value
		if (sysexTyp == 0) {
			print_String(156,124,40); // print "PATCH"
		}
		else {
			print_String(36,127,40); // print "BANK"
		}
		
		
		// print DUMP BANK value
		tft.setCursor(136, 59);
		tft.print(char(sysexBank + 65));
		
		// print Source
		tft.setTextColor(ST7735_WHITE);
		if (SourceDisabled == false) {
			if ((sysexSource+1) < 10) {
				tft.setCursor(136, 78);
			}
			else if ((sysexSource + 1) < 100) {
				tft.setCursor(133, 78);
				} else {
				tft.setCursor(130, 78);
			}
			tft.print(sysexSource + 1);	// print 1-128
			} else {
			print_String(31,130,78);	// print "---"
		}
		
		
		// print Dump (Send/Rec)
		tft.setTextColor(ST7735_WHITE);
		if (sysexDump == 0) {
			print_String(197,127,97); // print "SEND"
		}
		else {
			print_String(198,130,97); // print "REC"
		}
		
		
		ParameterNr = ParameterNrMem[PageNr + 1];
		drawParamterFrame(PageNr, ParameterNr);
	}
	
	draw_Voice_State();
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw Save Patch Page
//*************************************************************************
FLASHMEM void drawSavePage (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.drawRect(0,0,160,110, ST7735_GRAY);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setFont(NULL);
	tft.setTextSize(0);
	tft.setTextColor(ST7735_WHITE);
	print_String(150,5,3);			// print "SAVE PROGRAM"
	// print PatchNo (1-128)
	tft.setCursor(23, 23);
	tft.setTextColor(ST7735_YELLOW);
	tft.setTextSize(2);
	int patchnumber = patchNo;
	if (patchnumber > 128) {
		patchnumber = 128;
	}
	if (patchnumber < 10) {
		tft.print("00");
		tft.print(patchnumber);
	}
	else if (patchnumber < 100) {
		tft.print("0");
		tft.print(patchnumber);
	}
	else tft.print(patchnumber);
	tft.fillRoundRect(6,21,14,19,2,ST7735_RED);
	tft.setCursor(8, 23);
	tft.setTextColor(ST7735_WHITE);
	tft.println(char(currentPatchBank + 65)); // print char BankNo
	// print Patch Name (max 12 char)
	newPatchName = currentPatchName;
	uint8_t stringLen = newPatchName.length();
	// test String lenght (max 12 Char)
	if (stringLen > 12) {
		newPatchName = currentPatchName.substring(0, 12);
	}
	// if String less than 12 fill in with spaces
	else if (stringLen < 12) {
		for (uint8_t i = stringLen; i < 12; i++) {
			newPatchName.concat(" ");
			currentPatchName = newPatchName;
		}
	}
	// save old Patchname
	oldPatchName = newPatchName;
	tft.setFont(NULL);
	tft.setTextSize(1);
	tft.setCursor(70,30);
	tft.setTextColor(ST7735_GRAY);
	tft.println(oldPatchName);
	tft.setTextSize(2);
	tft.setCursor(8,50);
	tft.setTextColor(ST7735_GRAY);
	tft.println(newPatchName);
	tft.setTextSize(2);
	tft.setTextColor(ST7735_WHITE);
	print_String(153,8,57);			// print "____________"
	tft.setTextColor(ST7735_GRAY);
	tft.setFont(NULL);
	tft.setTextSize(0);
	print_String(154,25,80);		// print "Press SAVE or SHIFT"
	print_String(155,25,90);		// print "for cancel"
	tft.setTextColor(ST7735_WHITE);
	print_String(36,5,115);			// print "BANK"
	print_String(156,50,115);		// print "PATCH"
	print_String(157,95,115);		// print "CHAR"
	print_String(158,135,115);		// print "<->"
}

//*************************************************************************
// draw Load Sequencer Pattern Page
//*************************************************************************
FLASHMEM void drawLoadSeqPattern (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.drawRect(0,0,160,110, ST7735_GRAY);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setFont(NULL);
	tft.setTextSize(0);
	tft.setCursor(5,3);
	tft.setTextColor(ST7735_WHITE);
	tft.println("LOAD PATTERN");
	// print PatterNo (1-64)
	tft.setCursor(15, 23);
	tft.setTextColor(ST7735_GREEN);
	tft.setTextSize(2);
	tft.print("LOAD PAT:");
	if (SEQPatternNo == 0) {
		SEQPatternNo = 1;
	}
	if (SEQPatternNo < 10) {
		tft.print("0");
		tft.print(SEQPatternNo);
	}
	else tft.print(SEQPatternNo);
	tft.setFont(NULL);
	tft.setTextSize(2);
	tft.setCursor(8,50);
	tft.setTextColor(ST7735_GRAY);
	recallPattern(SEQPatternNo);
	tft.println(patternName);
	tft.setCursor(8,57);
	tft.setTextColor(ST7735_WHITE);
	tft.println("____________");
	tft.setTextColor(ST7735_GRAY);
	tft.setFont(NULL);
	tft.setTextSize(0);
	tft.setCursor(25,80);
	tft.println("Press LOAD or SHIFT");
	tft.setCursor(25,90);
	tft.println("for cancel");
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(7,117);
	tft.println("PAT");
	tft.setCursor(50,117);
	tft.println("---");
	tft.setCursor(95,117);
	tft.println("---");
	tft.setCursor(135,117);
	tft.println("---");
}
//*************************************************************************
// draw Save Sequencer Pattern Page
//*************************************************************************
FLASHMEM void drawSaveSeqPattern (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.drawRect(0,0,160,110, ST7735_GRAY);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setFont(NULL);
	tft.setTextSize(0);
	tft.setCursor(5,3);
	tft.setTextColor(ST7735_WHITE);
	tft.println("SAVE PATTERN");
	// print PatterNo (1-64)
	tft.setCursor(15, 23);
	tft.setTextColor(ST7735_RED);
	tft.setTextSize(2);
	tft.print("SAVE PAT:");
	if (SEQPatternNo < 10) {
		tft.print("0");
		tft.print(SEQPatternNo);
	}
	else tft.print(SEQPatternNo);
	
	tft.setFont(NULL);
	tft.setTextSize(2);
	tft.setCursor(8,50);
	tft.setTextColor(ST7735_GRAY);
	recallPattern(SEQPatternNo);
	tft.println(patternName);
	tft.setCursor(8,57);
	tft.setTextColor(ST7735_WHITE);
	tft.println("____________");
	tft.setTextColor(ST7735_GRAY);
	tft.setFont(NULL);
	tft.setTextSize(0);
	tft.setCursor(25,80);
	tft.println("Press SAVE or SHIFT");
	tft.setCursor(25,90);
	tft.println("for cancel");
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(7,117);
	tft.println("PAT");
	tft.setCursor(50,117);
	tft.println("CHAR");
	tft.setCursor(95,117);
	tft.println("<->");
	tft.setCursor(135,117);
	tft.println("---");
}

//*************************************************************************
// draw init Patch Page
//*************************************************************************
FLASHMEM void drawInitPatchPage (void)
{
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.drawRect(0,0,160,110, ST7735_GRAY);
	tft.setCursor(0,0);
	tft.fillRect(0,0,160,13,ST7735_GRAY);
	tft.setFont(NULL);
	tft.setTextSize(0);
	tft.setCursor(5,3);
	tft.setTextColor(ST7735_WHITE);
	tft.println("init Patch");
	tft.setCursor(15, 23);
	tft.setTextColor(ST7735_RED);
	tft.setTextSize(2);
	tft.print("init Patch");
	tft.setCursor(8,57);
	tft.setTextColor(ST7735_WHITE);
	tft.println("____________");
	tft.setTextColor(ST7735_GRAY);
	tft.setFont(NULL);
	tft.setTextSize(0);
	tft.setCursor(25,80);
	tft.println("Press SAVE or SHIFT");
	tft.setCursor(25,90);
	tft.println("for cancel");
}

//*************************************************************************
// print Unisono Mode
//*************************************************************************
FLASHMEM void printVoiceMode(void) {
	
	// POLY,UNI2,UNI3,UNI4,UNI6,MONO,MON2,MON3,MON4,MON6,CHOR
	
	if (PageNr == 0) {
		tft.setFont(&Picopixel);
		tft.setTextColor(ST7735_WHITE);
		tft.fillRect(96,20,18,6,ST7735_BLACK);
		
		if (unison == 0) {
			print_String(235,98,24);		// print "POLY"
		}
		else if (unison == 1) {
			switch(Voice_mode) {
				case 0: print_String(240,96,24); break;		// print "MONO"
				case 1: print_String(241,96,24); break;		// print "MON2"
				case 2: print_String(242,96,24); break;		// print "MON3"
				case 3: print_String(243,96,24); break;		// print "MON4"
				case 4: print_String(244,96,24); break;		// print "MON6"
				case 5: print_String(245,97,24); break;		// print "CHOR"
			}
		}
		RefreshMainScreenFlag = false;
		tft.setFont(NULL);					// reset font
	}
	
	else if (PageNr == 2) {
		uint8_t detune_val = 0;
		tft.fillRoundRect(50,76,26,10,2,ST7735_BLACK);
		tft.setTextColor(ST7735_WHITE);
		for (uint8_t i = 0; i < 128; i++) {
			float myDetune = 1.0f - (MAXDETUNE * POWER[i]);
			if ((myDetune - detune) < 0.000001) {	// compare float!
				detune_val = i;
				break;
			}
		}
		if (unison == 0 || (unison == 1 && Voice_mode <= 4)) {
			tft.fillRoundRect(54,76,22,10,2,ST7735_BLUE);
			tft.setCursor(56,78);
			tft.print(detune_val);	// print detune value
		}
		else if (unison == 1 && Voice_mode == 5) {
			tft.fillRoundRect(50,76,26,10,2,ST7735_BLUE);
			tft.setCursor(52,78);
			tft.print(CDT_STR[detune_val]);	// print chord typ
		}
	}
	
	else if (PageNr == 12) {
		tft.fillRect(85,97,36,7,ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		print_String(234,85,97);		// print "VOICE-M"
		tft.fillRoundRect(133,95,22,10,2,ST7735_GRAY);
		
		if (unison == 0) {
			tft.fillRoundRect(133,95,22,10,2,ST7735_BLUE);
			tft.setTextColor(ST7735_WHITE);
			print_String(235, 132, 97);		// print "Poly"
			return;
		}
		
		if (unison == 1) {
			tft.fillRoundRect(132,95,23,10,2,ST7735_BLUE);
			tft.setTextColor(ST7735_WHITE);
			
			switch(Voice_mode) {
				case 0: print_String(240,132,97); break;		// print "MONO"
				case 1: print_String(241,132,97); break;		// print "MON2"
				case 2: print_String(242,132,97); break;		// print "MON3"
				case 3: print_String(243,132,97); break;		// print "MON4"
				case 4: print_String(244,132,97); break;		// print "MON6"
				case 5: print_String(245,132,97); break;		// print "CHOR"
			}
		}
	}
}

//*************************************************************************
// draw Menu Page
//*************************************************************************
FLASHMEM void renderCurrentPatchPage(void)
{
	switch (PageNr) {
		case 0: drawMainPage();	break;
		case 1: drawOsc1Page();	break;
		case 2: drawOsc2Page();	break;
		case 3: drawFilterPage(); break;
		case 4: drawFilterEnvPage(); break;
		case 5: drawAmpEnvPage(); break;
		case 6: drawLFO1Page();	break;
		case 7: drawLFO2Page();	break;
		case 8: drawLFO3Page();	break;
		case 9: drawModMatrixPage(); break;
		case 10: drawFxDspPage(); break;
		case 11: drawSEQPage(); break;
		case 12: drawSystemPage(); break;
		case 96: drawInitPatchPage(); break;
		case 97: drawLoadSeqPattern(); break;
		case 98: drawSaveSeqPattern(); break;
		case 99: drawSavePage(); break;
	}
	sysexRecTimeStatus = false;
	time_sysexRecStatus = false;
	
	ParUpdate = true;
	tftUpdate = true;
}

//*************************************************************************
// render PulseWidth
//*************************************************************************
FLASHMEM void renderPulseWidth(float value) {
	tft.drawFastHLine(108, 74, 15 + (value * 13), ST7735_CYAN);
	tft.drawFastVLine(123 + (value * 13), 74, 20, ST7735_CYAN);
	tft.drawFastHLine(123 + (value * 13), 94, 16 - (value * 13), ST7735_CYAN);
	if (value < 0) {
		tft.drawFastVLine(108, 74, 21, ST7735_CYAN);
		} else {
		tft.drawFastVLine(138, 74, 21, ST7735_CYAN);
	}
}

FLASHMEM void renderVarTriangle(float value) {
	tft.drawLine(110, 94, 123 + (value * 13), 74, ST7735_CYAN);
	tft.drawLine(123 + (value * 13), 74, 136, 94, ST7735_CYAN);
}

FLASHMEM void renderEnv(float att, float dec, float sus, float rel) {
	tft.drawLine(100, 94, 100 + (att * 15), 74, ST7735_CYAN);
	tft.drawLine(100 + (att * 15), 74.0, 100 + ((att + dec) * 15), 94 - (sus * 20), ST7735_CYAN);
	tft.drawFastHLine(100 + ((att + dec) * 15), 94 - (sus * 20), 40 - ((att + dec) * 15), ST7735_CYAN);
	tft.drawLine(139, 94 - (sus * 20), 139 + (rel * 13), 94, ST7735_CYAN);
}



//*************************************************************************
FLASHMEM void renderCurrentParameterPage() {
	switch (state) {
		case PARAMETER:
		tft.fillScreen(ST7735_BLACK);
		tft.setFont(&FreeSans12pt7b);
		tft.setCursor(0, 53);
		tft.setTextColor(ST7735_YELLOW);
		tft.setTextSize(1);
		tft.println(currentParameter);

		// Not a necessary feature perhaps
		//      if (midiOutCh > 0) {
		//        tft.setTextColor(ST77XX_ORANGE);
		//        tft.setFont(&Org_01);
		//        tft.setTextSize(2);
		//        tft.setCursor(140, 35);
		//        tft.println(midiOutCh);
		//        tft.setFont(&FreeSans12pt7b);
		//        tft.setTextSize(1);
		//      }
		renderPeak();
		
		tft.drawFastHLine(10, 63, tft.width() - 20, ST7735_RED);
		tft.setCursor(1, 90);
		tft.setTextColor(ST7735_WHITE);
		tft.println(currentValue);
		if (pickUpActive) {
			tft.fillCircle(150, 70, 5, ST77XX_DARKGREY);
			tft.drawFastHLine(146, 70, 4, ST7735_WHITE);
		}
		switch (paramType) {
			case PULSE:
			renderPulseWidth(currentFloatValue);
			break;
			case VAR_TRI:
			renderVarTriangle(currentFloatValue);
			break;
			case FILTER_ENV:
			renderEnv(filterAttack * 0.0001f, filterDecay * 0.0001f, filterSustain, filterRelease * 0.0001f);
			break;
			case AMP_ENV:
			renderEnv(ampAttack * 0.0001f, ampDecay * 0.0001f, ampSustain, ampRelease * 0.0001f);
			break;
		}
		break;
	}
}

FLASHMEM void renderDeletePatchPage() {
	tft.fillScreen(ST7735_BLACK);
	tft.setFont(&FreeSansBold18pt7b);
	tft.setCursor(5, 53);
	tft.setTextColor(ST7735_YELLOW);
	tft.setTextSize(1);
	tft.println("Delete?");
	tft.drawFastHLine(10, 60, tft.width() - 20, ST7735_RED);
	tft.setFont(&FreeSans9pt7b);
	tft.setCursor(0, 78);
	tft.setTextColor(ST7735_YELLOW);
	tft.println(patches.last().patchNo);
	tft.setCursor(35, 78);
	tft.setTextColor(ST7735_WHITE);
	tft.println(patches.last().patchName);
	tft.fillRect(0, 85, tft.width(), 23, ST77XX_DARKRED);
	tft.setCursor(0, 98);
	tft.setTextColor(ST7735_YELLOW);
	tft.println(patches.first().patchNo);
	tft.setCursor(35, 98);
	tft.setTextColor(ST7735_WHITE);
	tft.println(patches.first().patchName);
}

FLASHMEM void renderDeleteMessagePage() {
	tft.fillScreen(ST7735_BLACK);
	tft.setFont(&FreeSans12pt7b);
	tft.setCursor(2, 53);
	tft.setTextColor(ST7735_YELLOW);
	tft.setTextSize(1);
	tft.println("Renumbering");
	tft.setCursor(10, 90);
	tft.println("SD Card");
}

FLASHMEM void renderSavePage() {
	tft.fillScreen(ST7735_BLACK);
	tft.setFont(&FreeSansBold18pt7b);
	tft.setCursor(5, 53);
	tft.setTextColor(ST7735_YELLOW);
	tft.setTextSize(1);
	tft.println("Save?");
	tft.drawFastHLine(10, 60, tft.width() - 20, ST7735_RED);
	tft.setFont(&FreeSans9pt7b);
	tft.setCursor(0, 78);
	tft.setTextColor(ST7735_YELLOW);
	tft.println(patches[patches.size() - 2].patchNo);
	tft.setCursor(35, 78);
	tft.setTextColor(ST7735_WHITE);
	tft.println(patches[patches.size() - 2].patchName);
	tft.fillRect(0, 85, tft.width(), 23, ST77XX_DARKRED);
	tft.setCursor(0, 98);
	tft.setTextColor(ST7735_YELLOW);
	tft.println(patches.last().patchNo);
	tft.setCursor(35, 98);
	tft.setTextColor(ST7735_WHITE);
	tft.println(patches.last().patchName);
}

FLASHMEM void renderReinitialisePage()
{
	tft.fillScreen(ST7735_BLACK);
	tft.setFont(&FreeSans12pt7b);
	tft.setTextColor(ST7735_YELLOW);
	tft.setTextSize(1);
	tft.setCursor(5, 53);
	tft.println("Initialise to");
	tft.setCursor(5, 90);
	tft.println("panel setting");
}

FLASHMEM void renderPatchNamingPage()
{
	tft.fillScreen(ST7735_BLACK);
	tft.setFont(&FreeSans12pt7b);
	tft.setTextColor(ST7735_YELLOW);
	tft.setTextSize(1);
	tft.setCursor(0, 53);
	tft.println("Rename Patch");
	tft.drawFastHLine(10, 63, tft.width() - 20, ST7735_RED);
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(5, 90);
	tft.println(newPatchName);
}

FLASHMEM void renderRecallPage()
{
	tft.fillScreen(ST7735_BLACK);
	tft.setFont(&FreeSans9pt7b);
	tft.setCursor(0, 45);
	tft.setTextColor(ST7735_YELLOW);
	tft.println(patches.last().patchNo);
	tft.setCursor(35, 45);
	tft.setTextColor(ST7735_WHITE);
	tft.println(patches.last().patchName);

	tft.fillRect(0, 56, tft.width(), 23, 0xA000);
	tft.setCursor(0, 72);
	tft.setTextColor(ST7735_YELLOW);
	tft.println(patches.first().patchNo);
	tft.setCursor(35, 72);
	tft.setTextColor(ST7735_WHITE);
	tft.println(patches.first().patchName);

	tft.setCursor(0, 98);
	tft.setTextColor(ST7735_YELLOW);
	patches.size() > 1 ? tft.println(patches[1].patchNo) : tft.println(patches.last().patchNo);
	tft.setCursor(35, 98);
	tft.setTextColor(ST7735_WHITE);
	patches.size() > 1 ? tft.println(patches[1].patchName) : tft.println(patches.last().patchName);
}

FLASHMEM void showRenamingPage(String newName) {
	newPatchName = newName;
}

FLASHMEM void renderUpDown(uint16_t  x, uint16_t  y, uint16_t  colour) {
	//Produces up/down indicator glyph at x,y
	tft.setCursor(x, y);
	tft.fillTriangle(x, y, x + 8, y - 8, x + 16, y, colour);
	tft.fillTriangle(x, y + 4, x + 8, y + 12, x + 16, y + 4, colour);
}

//*************************************************************************
// draw SUB page blink marker
//*************************************************************************
FLASHMEM void drawSubPageInfo () {
	
	uint8_t xpos[13] = {0,32,32,45,45,45,32,32,32,26,0,25,43};	// Text Position
	
	if (PageNr != 0 && PageNr != 9 && PageNr != 10) {
		if (myPageShiftStatus[PageNr] >= 1) {
			if (blink == true) {
				if (PageNr != 3) {
					tft.fillRoundRect(xpos[PageNr],2,19,9,2,ST7735_YELLOW);
					tft.setCursor(xpos[PageNr]+1,3);
					tft.setTextColor(ST7735_BLACK);
					tft.print("SUB");
				} else {
					tft.fillRoundRect(xpos[PageNr],2,25,9,2,ST7735_YELLOW);
					tft.setCursor(xpos[PageNr]+1,3);
					tft.setTextColor(ST7735_BLACK);
					if (myPageShiftStatus[3] == 1) {
						tft.print("SUB1");
					} else if (myPageShiftStatus[3] == 2) {
						tft.print("SUB2");
					}
				}
				blink = false;
				} else {
					if (PageNr != 3) {
						tft.fillRoundRect(xpos[PageNr],2,19,9,2,ST7735_GRAY);
					} else {
						tft.fillRoundRect(xpos[PageNr],2,25,9,2,ST7735_GRAY);
					}
				
				blink = true;
			}
		}
	}
}


//*************************************************************************
// set/clear SysEx recording timer
//*************************************************************************
FLASHMEM void SysExRecTimer(){
	
	// wait Time to SysEx Receive Data (15sec)
	if (sysexRecTimeStatus == true && time_sysexRecStatus == false) {
		timer_sysexRec = millis();
		time_sysexRecStatus = true;
		if (PageNr == 12 && myPageShiftStatus[12] == true) {
			
			if (sysexDump == true) {
				tft.fillRoundRect(123,20+(19*4),31,10,2,ST7735_RED);
				tft.setTextColor(ST7735_WHITE);
				print_String(198,130,97); // print "REC"
			}
			else {
				tft.fillRoundRect(123,19+(19*4),31,10,2,ST7735_GREEN);
				tft.setTextColor(ST7735_BLACK);
				print_String(197,127,97); // print "SEND"
			}
		}
	}
	
	// wait Time end
	else if (sysexRecTimeStatus == true && time_sysexRecStatus == true) {
		if ((millis() - timer_sysexRec) >= 20000){
			sysexRecTimeStatus = false;
			time_sysexRecStatus = false;
			if (PageNr == 12 && myPageShiftStatus[12] == true) {
				if (sysexDump == true) {
					tft.fillRoundRect(123,20+(19*4),31,10,2,ST7735_BLACK);
					tft.fillRoundRect(123,19+(19*4),31,10,2,ST7735_BLUE);
					tft.setTextColor(ST7735_WHITE);
					print_String(198,130,97); // print "REC"
				}
				else {
					tft.fillRoundRect(123,20+(19*4),31,10,2,ST7735_BLACK);
					tft.fillRoundRect(123,19+(19*4),31,10,2,ST7735_BLUE);
					tft.setTextColor(ST7735_WHITE);
					print_String(197,128,97); // print "SEND"
				}
			}
		}
	}
}

//*************************************************************************
// draw ProgressBar
//*************************************************************************
FLASHMEM void drawProgressbar() {

	float value;
	static boolean aktiveFlag = false;
	static boolean waitFlag = false;
	static uint16_t value_x = 0;
	
	if (ProgBar_enabled == false) {
		return;
	}
	
	// draw Progressbar
	tft.fillRect(20,44,120,37,ST7735_WHITE);
	tft.drawRect(25,61,110,15,ST7735_GRAY);
	
	// set Progressbar typ
	if (ProgBar_style == 1) {			// transmit
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(26,48);
		tft.print(Progbar_string);
		
	} else if (ProgBar_style == 2) {	// receive 
		tft.setTextColor(ST7735_RED);
		tft.setCursor(26,48);
		tft.print(Progbar_string);
	}
	
	// calc progressbar fill lenght
	value_x +=20;
	if (value_x >= ProgBar_count) {
		value_x = ProgBar_count;
	}
	
	value = ((109.0f / Progbar_maxcount) * value_x);
	if (value >= 109.0f) {
		value = 109;
		// start Progressbar hold timer
		if (aktiveFlag == false) {
			timer_Progressbar = millis();
			aktiveFlag = true;
			waitFlag = true;
		}
	}
	
	// draw fill bar
	if (ProgBar_style == 1) {
		tft.fillRect(26,62,value - 1,13,ST7735_DARKGREEN);	// send = GREEN
		} 
	else if (ProgBar_style == 2) {
		tft.fillRect(26,62,value - 1,13,ST7735_RED);	// receive = Red
	}
	
	// wait a bit before transmission ends and redraw menu page
	if ((millis() - timer_Progressbar) > 800 && waitFlag == true) {
		aktiveFlag = false;
		waitFlag = false;
		ProgBar_enabled = false;
		value_x = 0;
		renderCurrentPatchPage();
	}
}

//*************************************************************************
// Display Thread
//*************************************************************************
void displayThread() {

	const uint8_t timeVal1 = 38;
	const uint8_t timeVal2 = 32;
	
	if (PageNr == 0) {
		vuMeter = false;
		if ((millis() - timer1) > timeVal1){	// refresh Main Page and Scope all 25ms
			enableScope(true);
			renderCurrentPatchPage();
			tft.updateScreen();
			timer1 = millis();
			VoicLEDtime = 1;			// note off time for LED symbols
		}
	}
	if (PageNr > 0) {
		if ((millis() - timer7) > timeVal2){	// refresh menu pages all 50ms
			vuMeter = true;
			MidiSymbol();
			renderPeak();
			drawVoiceLED();
			SysExRecTimer();
			drawProgressbar();
			tft.updateScreen();
			timer7 = millis();
			VoicLEDtime = 1;			// note off time for LED symbols
		}
		if ((millis() - timer4) > blinkiTime){	// Time for green SUB Page Marker
			blinkiTime = 250;
			timer4 = millis();
			drawSubPageInfo();
		}
	}
}


//*************************************************************************
// set Display and init Thread
//*************************************************************************
FLASHMEM void setupDisplay() {
	tft.useFrameBuffer(false);
	tft.initR(INITR_BLACKTAB);	// 1.8" TFT 128x160 Pixel
	//tft.initR(INITR_GREENTAB);	// 1.8" TFT 128x160 Pixel
	//tft.initR(INITR_REDTAB);	// 1.8" TFT 128x160 Pixel
	tft.setRotation(1);
	tft.invertDisplay(false);
	tft.fillScreen(ST7735_BLACK);
	
	// init SD card -------------------------------------------------------
	if (!SD.begin(chipSelect)) {
		tft.fillScreen(ST7735_RED);
		tft.setTextColor(ST7735_WHITE);
		tft.setTextSize(2,2);
		tft.setCursor(15,55);
		tft.print("No SD card!");
		while(1) { };
	}
	else {
		Serial.println(F("SD card is connected"));
	}
	
	// Draw intro pics -------------------------------------------------
	Serial.println(F("Draw Power up pics"));
	tft.setTextColor(ST7735_LIGHTGRAY);
	uint8_t waitTime = 1;
	for (uint8_t i = 0; i < 1; i++) {
		bmpDraw("PIC/PIC1.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC2.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC3.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC4.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC5.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC6.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC7.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC8.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC9.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC10.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC11.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC12.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC11.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC10.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC9.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC8.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC7.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC6.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC5.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC4.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC3.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC2.bmp",0,0);
		delay(waitTime);
		bmpDraw("PIC/PIC1.bmp",0,0);
	}
	
	tft.useFrameBuffer(true); // activate Screen Buffer
}

