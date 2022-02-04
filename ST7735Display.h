// ST7735Display.h

// Teensy 4.1 Port connection
#define sclk 27		// 100R series Resitor for better sound interference
#define mosi 26
#define cs 2
#define dc 3
#define rst 9
#define DISPLAYTIMEOUT 700
#define BUFFPIXEL 20

#include "ST7735_t3.h" // Local copy from TD1.48 that works for 0.96" IPS 160x80 display

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
String currentPatchName = "            ";
String newPatchName = "            ";
String oldPatchName = "            ";
String newPatternName = "            ";
int newPatchNo = 1;
int newPatternNo = 1;
int newPatchBank = 0;
uint8_t CharPosPatchName = 0;
char * currentSettingsOption = "";
char * currentSettingsValue = "";
uint32_t currentSettingsPart = SETTINGS;
uint32_t paramType = PARAMETER;

boolean MIDIClkSignal = false;
int MidiCLKcount = 0;
boolean MidiCLKstatus = false;
float MidiCLKzeitAlt = 0;
uint32_t peakCount = 0;
uint16_t prevLen = 0;

uint32_t colour[NO_OF_VOICES] = {ST7735_BLUE, ST7735_BLUE, ST7735_BLUE, ST7735_BLUE, ST7735_BLUE, ST7735_BLUE, ST7735_BLUE, ST7735_BLUE};

unsigned long timer = 0;
unsigned long timer1 = 0;
unsigned long timer2 = 0;
unsigned long timer3 = 0;
unsigned long timer4 = 0;
unsigned long timer5 = 0;
unsigned long timer6 = 0;
unsigned long timer7 = 0;	// refresh menu header (VU, Midi note and voice lamps)

uint8_t currentPatchNameStatus = 0;
uint8_t currentPatchNameHoldTime = 0;
uint8_t bpmCount = 0;
float Clock_Time = 0;


//*************************************************************************
// Functionprototyp
//*************************************************************************
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
void printPWMrate (void);
void draw_PWM_curve(float value);
void drawEnvMarkerVCF(int posx);
void drawLFOMarkerVCF(int posx);
void drawSEQpitchValue2(uint8_t SEQselectStepNo);

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
FLASHMEM void printPWMrate (void)
{
	uint8_t value = 0;
	
	for (uint8_t i = 0; i < 128; i++) {
		float myValue = PWMRATE[i];
		if (pwmRate == myValue) {
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
	tft.print(int(DIV100 * value));
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
FLASHMEM read16(File f) {
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

FLASHMEM read32(File f) {
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
	uint32_t pos = 0, startTime = millis();

	uint16_t awColors[320];  // hold colors for one row at a time...

	if ((x >= tft.width()) || (y >= tft.height())) return;

/*
	Serial.println();
	Serial.print(F("Loading image '"));
	Serial.print(filename);
	Serial.println('\'');
	*/

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
	
	if (WShaperNo == 0) {
		tft.setCursor(103,30);
		tft.setTextColor(ST7735_WHITE);
		tft.print("WS OFF");
		return;
	}
	
	// draw curves ----------------------------------
	
	else if (WShaperNo == 1) {
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE2[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 1; i < 129; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE2[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 3;
		}
		tft.drawLine(old_xpos,old_ypos, xpos+2, ypos, ST7735_YELLOW);
	}
	
	else if (WShaperNo == 2) {
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE3[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 1; i < 129; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE3[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 3;
		}
		tft.drawLine(old_xpos,old_ypos, xpos+2, ypos, ST7735_YELLOW);
	}
	
	else if (WShaperNo == 3) {
		// draw curve
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE4[27];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 28; i < 129; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE4[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 3;
		}
	}
	
	else if (WShaperNo == 4) {
		// draw curve
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE5[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 57; i < 2049; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE5[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 57;
		}
	}
	
	else if (WShaperNo == 5) {
		// draw curve
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE6[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 27; i < 1025; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE6[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 27;
		}
	}
	
	else if (WShaperNo == 6) {
		// draw curve
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE7[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 0; i < 17; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE7[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
		}
	}

	else if (WShaperNo == 7) {
		// draw curve
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE8[57];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 58; i < 2049; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE8[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 57;
		}
	}
	
	else if (WShaperNo == 8) {
		// draw curve
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE9[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 0; i < 17; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE9[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
		}
	}
	
	else if (WShaperNo == 9) {
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE10[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 1; i < 129; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE10[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 3;
		}
		tft.drawLine(old_xpos,old_ypos, xpos+2, ypos, ST7735_YELLOW);
	}
	
	else if (WShaperNo == 10) {
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE11[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 1; i < 129; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE11[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 3;
		}
		tft.drawLine(old_xpos,old_ypos, xpos+2, ypos, ST7735_YELLOW);
	}
	
	else if (WShaperNo == 11) {
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE12[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 1; i < 129; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE12[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 3;
		}
		tft.drawLine(old_xpos,old_ypos, xpos+2, ypos, ST7735_YELLOW);
	}
	
	else if (WShaperNo == 12) {
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE13[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 1; i < 129; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE13[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 3;
		}
		tft.drawLine(old_xpos,old_ypos, xpos+2, ypos, ST7735_YELLOW);
	}
	
	else if (WShaperNo == 13) {
		uint8_t xpos = 102;
		uint8_t old_xpos = 102;
		uint8_t ypos = 49;
		uint8_t old_ypos = 49;
		
		float value = WAVESHAPER_TABLE14[0];
		value = value + 1.0f;
		uint8_t val = (uint8_t)(value * 16.0f);
		ypos = 49 - (val);
		old_ypos = ypos;
		
		for (uint16_t i = 1; i < 129; i++) {
			tft.drawLine(old_xpos,old_ypos, xpos, ypos, ST7735_YELLOW);
			xpos++;
			value = WAVESHAPER_TABLE14[i];
			value = value + 1.0f;
			val = (uint8_t)(value * 16.0f);
			old_ypos = ypos;
			old_xpos = xpos;
			ypos = 49 - (val);
			i += 3;
		}
		tft.drawLine(old_xpos,old_ypos, xpos+2, ypos, ST7735_YELLOW);
	}
	
	
	
	if (WShaperNo == 1) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(140,17);
		tft.setTextColor(ST7735_WHITE);
		tft.print("0.8");
		tft.setCursor(84,17);
		tft.print("CLIP");
	}
	else if (WShaperNo == 2) {
		tft.setCursor(140,22);
		tft.setTextColor(ST7735_WHITE);
		tft.print("0.5");
		tft.setCursor(84,17);
		tft.print("CLIP");
	}
	else if (WShaperNo == 3) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("TAHY");
	}
	else if (WShaperNo == 4) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("SIN1");
	}
	else if (WShaperNo == 5) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("ARB1");
	}
	else if (WShaperNo == 6) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("STAIR");
	}
	else if (WShaperNo == 7) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("ARB2");
	}
	else if (WShaperNo == 8) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("ARB3");
	}
	else if (WShaperNo == 9) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("SIN2");
	}
	else if (WShaperNo == 10) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("SIN3");
	}
	else if (WShaperNo == 11) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("OTON");
	}
	else if (WShaperNo == 12) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("DUAL");
	}
	else if (WShaperNo == 13) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(84,17);
		tft.print("TANH");
	}
	
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
// draw Voice LED
//*************************************************************************
FLASHMEM void drawVoiceLED (void)
{
	if (PageNr == 0) {
		return;
	}
	
	// draw Voices LED ----------------------------------------------------
	tft.fillRect(108,2,22,10,ST7735_GRAY);		// clear all voices LED
	
	// draw clear LED
	for (uint8_t i = 0; i < 8; i++) {
			if (i < 4) {
				tft.drawCircle(110 + (i * 6), 3, 2, ST7735_LIGHTGRAY);
				} else {
				tft.drawCircle(110 + ((i-4) * 6), 9, 2, ST7735_LIGHTGRAY);
			}
	}
	
	// set voice LED
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

//*************************************************************************
// print Patch name if Midi Prgchange
//*************************************************************************
FLASHMEM void printCurrentPatchName (void) 
{
	if (currentPatchNameStatus == 1) {
		
		tft.fillRect(25,43,110,45,ST7735_WHITE);
		tft.fillRect(26,44,108,43,ST77XX_DARKGREY);
		
		uint16_t x1 = 0;
		uint16_t y1 = 0;
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
	int PeakColor = 0;
	
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
	else if (PeakFlag == true && PeakTime == 0 || PeakPeakTime == 1) {
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
FLASHMEM drawEnvelopeMeter ()
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
		case 19:			// Silent
			waveNo = 0;
		break;
		case 3:				// Triangle
			waveNo = 1;
		break;
		case 11:			// bandlimit Square
			waveNo = 2;
		break;
		case 9:				// bandlimit Sawtooth
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
		case 19:			// Silent
			waveNo = 0;
		break;
		case 7:				// S&H
			waveNo = 1;
		break;
		case 11:			// bandlimit Square
			waveNo = 2;
		break;
		case 9:				// bandlimit Sawtooth
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
	else {
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
		tft.drawRect(Xpos,15+(19*col),width+2,18,ST7735_ORANGE);
		tft.drawRect(Xpos+1,16+(19*col),width,16,ST7735_ORANGE);
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
	tft.drawRect(63,38,53,11,ST7735_BLACK);
	const uint16_t myColor = ST7735_YELLOW;
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
			tft.drawRect(63,38,53,11,myColor);
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
	/*
	else {
		FxClkRate = (value * 434) + 5000;
		tft.setCursor(65,40);
		tft.setTextColor(ST7735_GRAY);
		tft.print("Clk");
		analogWriteFrequency (PWM5, FxClkRate);
		tft.fillRect(85,40,29,7,ST7735_BLACK);
		tft.setCursor(85,40);
		tft.print(FxClkRate / 1000);
		tft.print("Khz");
	}
	*/
	
	if (index <= 2) {
		tft.setCursor(xpos,88);
		tft.setTextColor(ST7735_WHITE);
		tft.print(value);
		drawFxGrafic(FxMixValue, FxTimeValue, FxFeedbackValue, FxPot3value);
	}
}

//*************************************************************************
// draw Fx Grafic
//*************************************************************************
FLASHMEM void drawFxGrafic (uint8_t volume, uint8_t time, uint8_t feedback, uint8_t Pot3value)
{
	uint16_t x1 = 23;
	uint16_t x2 = x1 + 3 + (time /8);
	uint8_t  y1 = 70;
	uint8_t  y2 = 70;
	
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
	y2 = 70;
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
// draw Fx Grafic
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
	tft.fillRect(3,71,155,10,ST7735_BLACK);
	tft.setCursor(75,72);
	tft.setTextColor(ST7735_GRAY);
	tft.print("Time/Feedb.");
	
	switch (PrgNo) {
		
		case 1:			// Chorus 1
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(10,72);
			tft.print("RevMix");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(67,72);
			tft.print("Rate");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(112,72);
			tft.print("ChorMix");
			break;
		};
		break;
		
		case 2:			// Flanger
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(10,72);
			tft.print("RevMix");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(67,72);
			tft.print("Rate");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(112,72);
			tft.print("FlanMix");
			break;
		};
		break;
		
		case 3:			// Tremolo
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(10,72);
			tft.print("RevMix");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(67,72);
			tft.print("Rate");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(112,72);
			tft.print("TremMix");
			break;
		};
		break;
		
		case 4:			// Pitch-Shift
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(10,72);
			tft.print("Pitch");
			break;
			case 1:
			break;
			case 2:
			break;
		};
		break;
		
		case 5:			// Pitch-Echo
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(12,72);
			tft.print("Shift");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(61,72);
			tft.print("EchoDel");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(113,72);
			tft.print("EchoMix");
			break;
		};
		break;
		
		case 6:			// Reverb 1
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(15,72);
			tft.print("Time");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(76,72);
			tft.print("HP");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(128,72);
			tft.print("LP");
			break;
		};
		break;
		
		case 7:			// Reverb 2
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(15,72);
			tft.print("Time");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(76,72);
			tft.print("HP");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(128,72);
			tft.print("LP");
			break;
		};
		break;
		
		case 8:			// X-Delay
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(15,72);
			tft.print("Time");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(57,72);
			tft.print("Feedback");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(110,72);
			tft.print("DMP/High");
			break;
		};
		break;
		
		case 9:			// Chorus 2
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(7,72);
			tft.print("LFO-FRQ");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(59,72);
			tft.print("AMT/DEL");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(127,72);
			tft.print("MIX");
			break;
		};
		break;
		
		case 10:			// TONATOR
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(7,72);
			tft.print("Pitch-L");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(59,72);
			tft.print("Pitch-R");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(110,72);
			tft.print("Feedback");
			break;
		};
		break;
		
		case 11:			// Phaser
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(9,72);
			tft.print("Depth");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(59,72);
			tft.print("LFO-Frq");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(110,72);
			tft.print("FrqRange");
			break;
		};
		break;
		
		case 12:			// Flangerator
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(7,72);
			tft.print("LFO-Frq");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(57,72);
			tft.print("Time/Frq");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(118,72);
			tft.print("Depth");
			break;
		};
		break;
		
		case 13:			// Distorter
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(18,72);
			tft.print("Mix");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(59,72);
			tft.print("DistFrq");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(110,72);
			tft.print("Feedback");
			break;
		};
		break;
		
		case 14:			// Stereo-Echo-Rev
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(16,72);
			tft.print("Time");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(62,72);
			tft.print("Repeat");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(115,72);
			tft.print("Reverb");
			break;
		};
		break;
		
		case 15:			// Gate
		switch(PotNo){
			case 0:
			tft.fillRoundRect(3,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(5,72);
			tft.print("PreTime");
			break;
			case 1:
			tft.fillRoundRect(56,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(59,72);
			tft.print("GatTime");
			break;
			case 2:
			tft.fillRoundRect(109,71,49,10,2,ST7735_GREEN);
			tft.setTextColor(ST7735_BLACK);
			tft.setCursor(112,72);
			tft.print("Damping");
			break;
		};
		break;
	};	
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
// draw Filter curves (0-127)
//*************************************************************************
FLASHMEM void draw_filter_curves (uint8_t FilterFrq, uint8_t FilterRes, uint8_t FilterMix)
{
	// Ladder Filter
	if (Filter == 2) {
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
		uint16_t MixColor = 0;	// yellow mix
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
			uint16_t MixColor = 0;	// Red mix
						
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
				uint16_t MixColor = 0;	// yellow mix
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
			//int xx = (Contrl_1_cut / 1.36f) + x_plot + i_;
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
	else { for (int i = 42; i < 86; i++) {
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
// draw Env curve
//*************************************************************************
FLASHMEM void drawEnvCurve(uint8_t ATKvalue, uint8_t DCYvalue, uint8_t SUSvalue, uint8_t RELvalue)
{
	int8_t envelopeType;
	uint8_t xplot1 = EXP127[ATKvalue];
	uint8_t xplot1_decay = EXP127[DCYvalue];
	uint8_t yplot2_sustain = SUSvalue;
	uint8_t xplot3_release = EXP127[RELvalue];
	
	// static Vars
	static uint8_t xplot1_temp = 10;
	static uint8_t yplot2_sustain_temp = 40;
	static uint8_t xplot1_decay_temp = 0;
	static uint8_t xplot3_release_temp = 0;
	
	if (PageNr == 4) {
		envelopeType = envelopeType1;
	} else envelopeType = envelopeType2;
	
	// delete old Envelope lines ------------------------------------------
	tft.fillRect(8,36,90,52,ST7735_BLACK);
	
	
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
	xplot3_release = (90 + (((EXP127[RELvalue]) * 25)/127));
	
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
	
	// save current lines
	xplot1_temp = xplot1;
	yplot2_sustain_temp = yplot2_sustain;
	xplot1_decay_temp = xplot1_decay;
	xplot3_release_temp = xplot3_release;

}

//*************************************************************************
// draw Waveform
//*************************************************************************
FLASHMEM void draw_Waveform(int WaveNr, uint16_t waveColor)
{
	int x1 = 87;
	int x2 = 87;
	int y1 = 45;
	int y2 = 45;
	uint16_t sample = 0;
	
	tft.fillRect(82,17,75,33,ST7735_BLACK);		// clear Wave Screen
	
	// set Osc WaveBank
	uint8_t WaveBank = 0;
	if (PageNr == 1) {
		WaveBank = Osc1WaveBank;
	}
	else if (PageNr == 2) {
		WaveBank = Osc2WaveBank;
	}
	
	// Osc off (Waveform No 0)
	if (WaveNr == 0) {
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(&Picopixel);
		tft.setCursor(140, 22);
		tft.print("OFF");
		tft.setFont(NULL);
	}
	
	// Standard and Band Limited waveforms (Waveform No 1 - 13)
	else if (WaveNr <= 12 && WaveBank == 0) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * (WaveNr-1));
			sample = pgm_read_byte(&(WAVEFORM_SYMBOLES_8Bit[phase_x])); // Load spezial Symboles
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
		if (WaveNr == 5) {
			float value = 0;
			if (PageNr == 1) {
				value = pwA;
			}
			else if (PageNr == 2) {
				value = pwB;
			}
			draw_PWM_curve(value);
			/*
			tft.setFont(&Picopixel);
			tft.setCursor(136, 22);
			tft.print("PWM");
			tft.setFont(NULL);
			*/
		}
		else if (WaveNr == 8) {
			tft.setFont(&Picopixel);
			tft.setCursor(129, 22);
			tft.print("TRI VAR");
			tft.setFont(NULL);
		}
		else if (WaveNr >= 9 && WaveNr <= 11) {
			tft.setFont(&Picopixel);
			tft.setCursor(118, 22);
			tft.print("BANDLIMIT");	// Bandlimidet
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
			/*
			tft.setFont(&Picopixel);
			tft.setCursor(127, 22);
			tft.print("PWM BL");	// Bandlimidet PWM
			tft.setFont(NULL);
			*/
		}
	}
	
	
	// waveforms Bank A (Waveform No 13 - 63)
	else if (WaveNr >= 13 && WaveBank == 0) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * (WaveNr-12));
			sample = pgm_read_word(&(ArbBankA[phase_x]));
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
	}
	
	// waveforms Bank B (Waveform No 1 - 63)
	else if (WaveBank == 1) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankB[phase_x]));
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
	}
	
	// waveforms Bank C (Waveform No 1 - 63)
	else if (WaveBank == 2) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankC[phase_x]));
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
	}
	
	// waveforms Bank D (Waveform No 1 - 63)
	else if (WaveBank == 3) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankD[phase_x]));
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
	}
	
	// waveforms Bank E (Waveform No 1 - 63)
	else if (WaveBank == 4) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankE[phase_x]));
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
	}
	
	// waveforms Bank F (Waveform No 1 - 63)
	else if (WaveBank == 5) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankF[phase_x]));
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
	}
	
	// waveforms Bank G (Waveform No 1 - 63)
	else if (WaveBank == 6) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankG[phase_x]));
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
	}
	
	// waveforms Bank H (Waveform No 1 - 63)
	else if (WaveBank == 7) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankH[phase_x]));
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
	}
	
	// waveforms Bank I (Waveform No 1 - 63)
	else if (WaveBank == 8) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankI[phase_x]));
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
	}
	
	// waveforms Bank J (Waveform No 1 - 63)
	else if (WaveBank == 9) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankJ[phase_x]));
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
	}
	
	// waveforms Bank K (Waveform No 1 - 63)
	else if (WaveBank == 10) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankK[phase_x]));
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
	}
	
	// waveforms Bank L (Waveform No 1 - 63)
	else if (WaveBank == 11) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankL[phase_x]));
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
	}
	
	// waveforms Bank M (Waveform No 1 - 63)
	else if (WaveBank == 12) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankM[phase_x]));
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
	}
	
	// waveforms Bank N (Waveform No 1 - 63)
	else if (WaveBank == 13) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankN[phase_x]));
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
	}
	
	// waveforms Bank O (Waveform No 1 - 63)
	else if (WaveBank == 14) {
		for (int i = 0; i < 64; i++) {
			int16_t phase_x = i * 4 + (256 * WaveNr);
			sample = pgm_read_word(&(ArbBankO[phase_x]));
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
	}
}

//*************************************************************************
// draw LFO shape
//*************************************************************************
FLASHMEM void drawLFOshape(uint16_t waveColor)
{
	uint8_t shapeNo = 0;
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
			tablePoint = 256 * 2; col = 0; row = 37;
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
		if (count == 0 || count == 1 || count == 5) {
			for (int i = 0; i < 43; i++) {
				int16_t phase_x = (i * 6 + tablePoint);
				if (count == 0) {
					sample = 255 - pgm_read_byte(&(WAVEFORM_SYMBOLES_8Bit[phase_x])); // invert Sine Symboles
				} else {
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
		if (count == 2) {
			tft.drawLine(119, 50, 143,30, waveColor);
			tft.drawLine(120, 50, 143,31, waveColor);
			tft.drawFastVLine(143,30,21, waveColor);
		}
		// invert Sawtooth
		if (count == 3) {
			tft.drawFastVLine(14, 68, 21, waveColor);
			tft.drawLine(14, 68, 38, 88, waveColor);
			tft.drawLine(15, 68, 38, 89, waveColor);
		}
		// Regtangle
		if (count == 4) {
			tft.drawFastHLine(61, 88, 17, waveColor);
			tft.drawFastVLine(78, 68, 21, waveColor);
			tft.drawFastHLine(78, 68, 17, waveColor);
			tft.drawFastVLine(95, 68, 21, waveColor);
		}
		
	}
}

//*************************************************************************
// draw LFO shape
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
		case 0: tft.drawRect(2,22,50,35,ST7735_GREEN); break;
		case 1: tft.drawRect(54,22,50,35,ST7735_GREEN); break;
		case 2: tft.drawRect(106,22,50,35,ST7735_GREEN); break;
		case 3: tft.drawRect(2,60,50,35,ST7735_GREEN); break;
		case 4: tft.drawRect(54,60,50,35,ST7735_GREEN); break;
		case 5: tft.drawRect(106,60,50,35,ST7735_GREEN); break;	
	}
}

//*************************************************************************
// draw Sequencer red Step Frame
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
	tft.drawRect(14 + (9 * SEQselectStepNo),21+3,10,73,ST7735_RED);

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
	uint8_t firstNote = 0;
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
				recallPatch(patchNo);
				storePatchBankNo(currentPatchBank);
				RefreshMainScreenFlag = true;
			}
		}
	}
	// Page:1 Osc1 --------------------------------------------------------
	if (Page == 1){
		
		// shift key enabled
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
				uint8_t val = (value / 21);
				if (val == 0) {
					tft.print("OFF");
				}
				else if (val == 1) {
					tft.print("XOR");
				}
				else if (val == 2) {
					tft.print("XMO");
				}
				else if (val == 3) {
					tft.print("MOD");
				}
				else if (val == 4) {
					tft.print("AND");
				}
				else if (val == 5) {
					tft.setCursor(137,78);
					tft.print("OR");
				}
				else if (val == 6) {
					tft.setCursor(137,78);
					tft.print("FM");
				}
			}
			
			// OscVCFMOD
			else if (ParameterNr == myOscVCFMOD) {
				if (OscVCFMOD != value) {
					OscVCFMOD = (DIV127 * value);
					oscGlobalModMixer.gain(2, OscVCFMOD);
					tft.fillRoundRect(133,97,21,8,2,ST7735_BLUE);
					tft.setCursor(135,97);
					tft.setTextColor(ST7735_WHITE);
					tft.print(value);
				}
			}
		}
	}
	
	// Page:2 Osc2 --------------------------------------------------------
	if (Page == 2){
		
		// Shift key enabled
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
				uint8_t val = (value / 21);
				if (val == 0) {
					tft.print("OFF");
				}
				else if (val == 1) {
					tft.print("XOR");
				}
				else if (val == 2) {
					tft.print("XMO");
				}
				else if (val == 3) {
					tft.print("MOD");
				}
				else if (val == 4) {
					tft.print("AND");
				}
				else if (val == 5) {
					tft.setCursor(137,78);
					tft.print("OR");
				}
				else if (val == 6) {
					tft.setCursor(137,78);
					tft.print("FM");
				}
			}
			// OscVCFMOD
			else if (ParameterNr == myOscVCFMOD) {
				if (OscVCFMOD != value) {
					OscVCFMOD = (DIV127 * value);
					oscGlobalModMixer.gain(2, OscVCFMOD);
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
				printRedMarker (1, value >> 1);
				printPercentValue(1, (DIV100 * (value >> 1)));
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
		
		// Sift Page enabled
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
	
	// Page:6 Fx Mix ----------------------------------------------------
	else if (Page == 6){
		
		// FxDSP
		if (myPageShiftStatus[PageNr] == false) {
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
						analogWrite(PWM1, value);
						FxPot1value = value >> 1;
					}
					break;
					case 1:
					if (FxPot2Val != value) {
						FxPot2Val = value;
						analogWrite(PWM2, value);
						FxPot2value = value >> 1;
					}
					break;
					case 2: 
					if (FxPot3Val != value) {
						FxPot3Val = value;
						analogWrite(PWM3, value);
						FxPot3value = value >> 1;
					}
					break;
					case 3:
					if (FxPot4Val != value) {
						FxPot4Val = value;
						FxClkRate = (value * 196.08f) + 10000;
						tft.setCursor(65,40);
						tft.setTextColor(ST7735_GRAY);
						tft.print("Clk");
						analogWriteFrequency (PWM5, FxClkRate);
						tft.fillRect(85,40,29,7,ST7735_BLACK);
						tft.setCursor(85,40);
						tft.print(FxClkRate / 1000);
						tft.print("Khz");
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
				analogWrite(PWM4, value);
				printDataValue (2, value >> 1);
				printRedMarker (2, value >> 1);
				FxMixValue = value >> 1;
				drawFxGrafic (FxMixValue, FxTimeValue, FxFeedbackValue, FxPot3value);
			}
			// PRG
			else if (ParameterNr == myFxPrg){
				if (value != FxPrgNo) {
					if (value == 0) {	// Fx off
						setFxPrg (0);
						tft.fillRect(65,27,90,9,ST7735_BLACK);
						tft.setTextColor(ST7735_RED);
						tft.setCursor(65,28);
						tft.print("OFF");
						tft.fillRect(122, 116, 30, 7, ST7735_BLACK);
						tft.setTextColor(ST7735_GRAY);
						tft.setCursor(130,116);
						tft.print("OFF");
						analogWrite(PWM4, 0);
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
						setLED(1, false);
						printFxValFrame(99); // clear green Frame
						tft.fillRect(3,71,155,10,ST7735_BLACK);
						tft.setCursor(75,72);
						tft.setTextColor(ST7735_GRAY);
						tft.print("Time/Feedb.");
						
					}
					else {
						setFxPrg (value);
						printDataValue (1, myFxValValue);
						printRedMarker (1, myFxValValue);
						printDataValue (2, FxMixValue);
						printRedMarker (2, FxMixValue);
						analogWrite(PWM4, FxMixValue << 1);
						printFxPotValue(0, FxPot1value);
						printFxPotValue(1, FxPot2value);
						printFxPotValue(2, FxPot3value);
						printDataValue (3, value);
						printRedMarker (3, (value * 8.5));
						printFxName(value);
						drawFxGrafic (FxMixValue, FxTimeValue, FxFeedbackValue, FxPot3value);
						setLED(1, true);
						printFxValFrame(myFxSelValue); // clear green Frame
						printFxPOT(myFxSelValue, value);
					}
					FxPrgNo = value;
				}
			}
		}
		// EFFECT 2
		else {
			
		}
		
	}
	
	// Page:7 LFO 1 -----------------------------------------------------
	else if (Page == 7) {
		
		// SYN
		if (ParameterNr == myLFO1syn) {
			value = value >> 2;
			tft.fillRect(122, 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			if (value < 10) {
				tft.setCursor(130, 116);
				tft.print("OFF");
				printRedMarker (3, 0);
				oscLfoRetrig = 0;
				updatePitchLFORetrig();
			}
			else if (value >= 10 && value <= 25 ) {
				tft.setCursor(134, 116);
				tft.print("0");
				tft.drawRect(141,116,3,3,ST7735_GRAY);
				printRedMarker (3, 64);
				oscLfoRetrig = 1;
				LFO1phase = 180.0f;
				updatePitchLFORetrig();
			}
			else {
				tft.setCursor(128, 116);
				tft.print("180");
				tft.drawRect(147,116,3,3,ST7735_GRAY);
				printRedMarker (3, 128);
				oscLfoRetrig = 1;
				LFO1phase = 0.0f;
				updatePitchLFORetrig();
			}
		}
	}
	
	// Page:8 LFO 2 -----------------------------------------------------
	else if (Page == 8) {		
		// SYN
		if (ParameterNr == myLFO2syn) {
			value = value >> 6;
			tft.fillRect(122, 116, 30, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_GRAY);
			switch(value) {
				case 0: 
					filterLfoRetrig = 0; 
					filterLFOMidiClkSync = 0;
					updateFilterLfoRate();
					filterLfoRate = LFOMAXRATE * POWER[myFilterLFORateValue];
					tft.setCursor(130, 116); 
					tft.print("OFF");
					printRedMarker (3, 0);
					tft.fillRect(47,103,23,7,ST7735_BLACK);
					tft.setTextColor(ST7735_WHITE);
					tft.setCursor(47, 103);
					tft.print("RATE");
					printDataValue (1, myFilterLFORateValue); 
				break;
				case 1: 
					filterLfoRetrig = 1;
					filterLFOMidiClkSync = 0;
					LFO2phase = 180.0f;
					updateFilterLfoRate();
					filterLfoRate = LFOMAXRATE * POWER[myFilterLFORateValue];
					tft.setCursor(134, 116); 
					tft.print("0");
					tft.drawRect(141,116,3,3,ST7735_GRAY);
					printRedMarker (3, 42);
					tft.fillRect(47,103,23,7,ST7735_BLACK);
					tft.setTextColor(ST7735_WHITE);
					tft.setCursor(47, 103);
					tft.print("RATE");
					printDataValue (1, myFilterLFORateValue); 
					
				break;
				case 2:
					filterLfoRetrig = 2;
					filterLFOMidiClkSync = 0;
					LFO2phase = 0.0f;
					updateFilterLfoRate();
					filterLfoRate = LFOMAXRATE * POWER[myFilterLFORateValue]; 
					tft.setCursor(128, 116); 
					tft.print("180"); 
					tft.drawRect(147,116,3,3,ST7735_GRAY);
					printRedMarker (3, 84);
					tft.fillRect(47,103,23,7,ST7735_BLACK);
					tft.setTextColor(ST7735_WHITE);
					tft.setCursor(47, 103);
					tft.print("RATE");
					printDataValue (1, myFilterLFORateValue);
				break;
				case 3: 
					filterLfoRetrig = 3;
					filterLFOMidiClkSync = 1;
					tft.setCursor(128, 116); 
					tft.print("MIDI");
					printRedMarker (3, 127);
					tft.fillRect(47,103,23,7,ST7735_BLACK);
					tft.setTextColor(ST7735_WHITE);
					tft.setCursor(49, 103);
					tft.print("DIV");
					filterLfoRate = getLFOTempoRate(myFilterLFORateValue);
					filterLFOTimeDivStr = LFOTEMPOSTR[myFilterLFORateValue];
					tft.fillRect(46, 116, 23, 7, ST7735_BLACK);
					tft.setCursor(46,116);
					tft.setTextColor(ST7735_GRAY);
					int16_t x1, y1;
					uint16_t w1, h1;
					tft.getTextBounds(filterLFOTimeDivStr , 0, 0, &x1, &y1, &w1, &h1); // string width in pixels
					tft.setCursor(58 - (w1 / 2), 116);	// print string in the middle
					tft.print(filterLFOTimeDivStr);
					if (filterLfoRetrig == 3 && MidiSyncSwitch == false) {
						tft.fillRect(46, 116, 24, 7, ST7735_BLACK);
						tft.setTextColor(ST7735_RED);
						tft.setCursor(46,116);
						tft.print("CLK?");
					} 
				break;
			}
		}
	}
	
	// Page:9 SEQUENCER  --------------------------------------------------
	else if (Page == 9) {
		
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
					Interval = 37500;
					gateTime = 18750;
					SEQrunOneNote = true;
					timer_intMidiClk = micros();
					timer_intMidiClk -= Interval;	// fixed clk-time for editor (160bpm, 1/8)
					SEQrunStatus = true;
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
					Interval = 37500;
					gateTime = 18750;
					SEQrunOneNote = true;
					timer_intMidiClk = micros();
					timer_intMidiClk -= Interval; // fixed clk-time for editor (160bpm, 1/8)
					SEQrunStatus = true;
				}
			}
		}
		
		// BPM (intern Clocksignal)
		if (ParameterNr == mySEQRate) {
			if (value > 0) {
				if (SEQMidiClkSwitch == true) {
					SEQrunStatus = false;
				}
				SEQMidiClkSwitch = false;
				uint8_t val = (value / 1.340314136f);
				SEQbpmValue = (SeqClkRate[val]);
				float bpm = (SEQbpmValue / SEQdivValue);
				SEQclkRate = float(60000000 / bpm);
				printDataValue (2, (SeqClkRate[val]));
				printRedMarker (2, (value >> 1));
				tft.fillRect(79, 103, 35,8, ST7735_BLACK);
				tft.setTextColor(ST7735_WHITE);
				tft.setCursor(89, 103);
				tft.print("BPM");
				Interval = SEQclkRate;
				gateTime = (float)(SEQclkRate / SEQGateTime);
				if (gateTime <= 3200) {
					gateTime = 3200;
				}
			} else {	// Midi Clk
				SEQMidiClkSwitch = true;
				allNotesOff();
				printRedMarker (2, 0);
				tft.fillRect(82, 116, 30, 7, ST7735_BLACK);
				tft.setTextColor(ST7735_GRAY);
				tft.setCursor(86,116);
				tft.print("MIDI");
				tft.fillRect(79, 103, 35,8, ST7735_BLACK);
				tft.setTextColor(ST7735_WHITE);
				tft.setCursor(89, 103);
				tft.print("BPM");
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
				
				
				
				/*
				uint8_t val = (value / 85);
				if (val == 0) {
					MidiClkDiv = 48;
				}
				else if (val == 1) {
					MidiClkDiv = 24;
				}
				else if (val == 2) {
					MidiClkDiv = 12;
				}
				else if (val == 3) {
					MidiClkDiv = 6;
				}
				printRedMarker (3, value >> 1);
				printSEQclkDiv (value);
				*/
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
			tft.fillRect(130,116,20,8,ST7735_BLACK);
			SEQmode = value >> 6;
			if (SEQmode >= 2) {
				SEQmode = 2;
			}
			if (SEQmode == 0) {
				tft.setTextColor(ST7735_GRAY);
				tft.setCursor(130,116);
				tft.print("KEY");
			}
			else if (SEQmode == 1){
				tft.setTextColor(ST7735_GRAY);
				tft.setCursor(130,116);
				tft.print("TRP");
			}
			else {
				tft.setTextColor(ST7735_GRAY);
				tft.setCursor(130,116);
				tft.print("REC");
			}
			int mode = SEQmode << 6;
			if (mode >= 127) {
				mode = 127;
			}
			printRedMarker (3, mode);
		}
	}
	
	// Page:10 System  ----------------------------------------------------
	else if (Page == 10) {
		
		// PitchWheel
		if (ParameterNr == myPitchWheel) {
			tft.fillRoundRect(54,76,22,10,2,ST7735_BLUE);
			tft.setCursor(56,78);
			tft.print(value);
			if (PitchWheelvalue != value) {
				PitchWheelvalue = value;
				PitchWheelAmt = float(value / 127.0f);
			}
		}
		
		// MODWheel
		if (ParameterNr == myMODWheel) {
			tft.fillRoundRect(54,95,22,10,2,ST7735_BLUE);
			tft.setCursor(56,97);
			tft.print(value);
			if (MODWheelvalue != value) {
				MODWheelvalue = value;
				MODWheelAmt = float(value / 127.0f);
			}
		}
		
		// Midi RxChanel
		else if (ParameterNr == myMidiCha) {
			value = (value / 7.9375f);
			if (value != midiChannel) {
				midiChannel = value;
				storeMidiChannel(midiChannel);
				tft.fillRoundRect(133,19,22,10,2,ST7735_BLUE);
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
			uint8_t val = 1 + (0.047 * value);
			if (val >= 4) {
				val = 4;
			}
			tft.fillRect(136,40,17,7,ST7735_BLUE);
			tft.setCursor(137, 40);
			tft.print(val);
			velocitySens = val;
		}
		
		/*
		// Unisono mode
		else if (ParameterNr == myUnisonoMode) {
			uint8_t val = (0.047 * value);
			if (val >= 1) {
				val = 1;
			}
			tft.fillRect(136,59,17,7,ST7735_BLUE);
			tft.setCursor(136, 59);
			if (val == 0) {
				tft.print("8-1");
			}
			else if (val == 1) {
				tft.print("4-2");
			}
			myUnisono = val;
		} */
		
		// MidiClk
		else if (ParameterNr == myMidiSyncSwitch) {
			uint8_t val = (0.047f * value);
			if (val >= 1) {
				val = 1;
			}
			if (val != MidiSyncSwitch) {
				tft.fillRect(136,78,17,7,ST7735_BLUE);
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
		
		// PRGChange
		if (ParameterNr == myPRGChange) {
			tft.fillRoundRect(133,95,22,10,2,ST7735_BLUE);
			value = value >> 6;
			if (value == 0) {
				tft.setCursor(136,97);
				tft.print("OFF");
			} else {
				tft.setCursor(138,97);
				tft.print("ON");
			}
			if (PrgChangeSW != value) {
				PrgChangeSW = value;
				storePRGchange(PrgChangeSW);
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
			uint8_t charName = (value * 0.71f);
			tft.fillRect(7 + (CharPosPatchName * 12),48,12,18, ST7735_BLACK);
			tft.setFont(NULL);
			tft.setTextSize(2);
			tft.setCursor(8 + (CharPosPatchName * 12), 50);
			tft.setTextColor(ST7735_WHITE);
			tft.println(char (charName + 32));
			oldPatchName.setCharAt(CharPosPatchName, charName + 32);
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
			uint8_t charName = (value * 0.71f);
			tft.fillRect(7 + (CharPosPatchName * 12),48,12,18, ST7735_BLACK);
			tft.setFont(NULL);
			tft.setTextSize(2);
			tft.setCursor(8 + (CharPosPatchName * 12), 50);
			tft.setTextColor(ST7735_WHITE);
			tft.println(char (charName + 32));
			newPatternName.setCharAt(CharPosPatchName, charName + 32);
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
FLASHMEM void printFilterEnvAmt(uint8_t EnvVal)
{
	if (EnvVal >= 126) {
		EnvVal = 126;
	}
	
	if (EnvVal == 63) {
		tft.fillRect(82, 116, 30, 7, ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(94, 116);
		tft.print(0);
	}
	else if (EnvVal < 63) {
		tft.fillRect(82, 116, 30, 7, ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(90, 116);
		tft.print("-");
		tft.print(63 - EnvVal);
	}
	else {
		tft.fillRect(82, 116, 30, 7, ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(90, 116);
		tft.print("+");
		tft.print(EnvVal - 63);
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

//*************************************************************************
// draw Peakmeter
//*************************************************************************
FLASHMEM void drawPeakmeter (void)
{
	tft.fillRect(149,1,9,11,ST7735_LIGHTGRAY);
	tft.drawFastHLine(149,11,9,ST7735_GREEN);
	tft.drawFastVLine(153,1,11,ST7735_GRAY);
}

//*************************************************************************
// draw Main page
//*************************************************************************
FLASHMEM void drawMainPage (void) {
	
	int16_t x1, y1;
	uint16_t w1, h1;

	long RedrawTime = 0; // micros() - timer6;
	timer6 = micros();
	
	if (clearScreenFlag == true) {
		tft.fillScreen(ST7735_BLACK);
		clearScreenFlag = false;
	}
	
	if (RefreshMainScreenFlag == true) {	
		// clear PatchNo
		tft.fillRect(64,8,47,18,ST7735_BLACK);
		// clear Patch Name
		tft.fillRect(0,40,160,17,ST7735_BLACK);
		const uint8_t ypos = 22;
		tft.fillRoundRect(47,7,16,20,2, ST7735_RED);
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(&FreeSans9pt7b);
		tft.setCursor(xposBankNo[currentPatchBank], ypos);
		tft.print(BankNo[currentPatchBank]);
		
		tft.setFont(&FreeSans9pt7b);
		tft.setCursor(65, 22);
		tft.setTextColor(ST7735_YELLOW);
		tft.setTextSize(1);
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
		
		// print Patch Name
		currentPatchName.trim();	// delete spaces
		tft.drawFastHLine(10, 63, tft.width() - 20, ST7735_RED);
		tft.setFont(&FreeSans9pt7b);
		tft.getTextBounds(currentPatchName , 0, 0, &x1, &y1, &w1, &h1); // string width in pixels
		tft.setCursor(80 - (w1 / 2), 52);	// print string in the middle
		tft.setTextColor(ST7735_WHITE);
		tft.println(currentPatchName);
		
		if (SeqSymbol == true) {
			tft.setFont(&Picopixel);
			tft.fillCircle(105,16,5,ST7735_DARKGREEN);
			tft.setTextColor(ST7735_WHITE);
			tft.setCursor(104,18);
			tft.print("S");
		}
		RefreshMainScreenFlag = false;
	}
	
	// clear Scope screen
	tft.fillRect(10,64,140,63,ST7735_BLACK);
	
	// print Text
	tft.setFont(NULL);
	tft.setCursor(10,115);
	tft.setTextColor(ST7735_GRAY);
	tft.print("BANK");
	
	// print Cutoff	PicUp function in main screen)
	if (cutoffScreenFlag == false) {
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(115,115);
		tft.print("CUTOFF");
		} else {
		int Frq = float(filterFreq);
		int FrqVal;
		for (FrqVal = 0; FrqVal < 255; FrqVal++) {
			int frequency = FILTERFREQS256[FrqVal];
			if (Frq <= frequency){
				break;
			}
		}
		uint8_t xpos = 115;
		uint8_t ypos = 117;
		FrqVal = FrqVal >> 1;
		// set green marker
		tft.drawFastHLine(xpos + 2, ypos + 1, 27, ST7735_BLUE);
		int redPos = (0.205f * FrqVal);
		// set red marker
		tft.drawFastVLine(xpos + 2 + redPos, ypos, 4, ST7735_RED);
		if (cutoffPickupFlag == false) {
			Frq = float(filterfreqPrevValue);
			uint8_t FrqVal;
			for (FrqVal = 0; FrqVal < 255; FrqVal++) {
				int frequency = FILTERFREQS256[FrqVal];
				if (Frq <= frequency){
					break;
				}
			}
			
			redPos = (0.205f * (FrqVal >> 1));
			//tft.drawFastVLine(xpos + 2 + redPos, ypos, 4, ST7735_GREEN);
			tft.fillCircle(xpos + 2 + redPos, ypos+1, 1, ST7735_GREEN);
		}
	}
	// calc cutoffScreenFlag Timer
	if ((millis() - timer5) > 2151){
		timer5 = millis();
		cutoffScreenFlag = false;
	}
	

	// draw Voices LED ----------------------------------------------------
	tft.fillRect(116,7,39,19,ST7735_BLACK);		// clear voices LED
	tft.drawFastHLine(117,7,38,ST7735_BLUE);	// draw LED grid
	tft.drawFastHLine(117,15,38,ST7735_BLUE);
	tft.drawFastHLine(117,17,38,ST7735_BLUE);
	tft.drawFastHLine(117,25,38,ST7735_BLUE);
	tft.drawFastVLine(117,7,18,ST7735_BLUE);
	tft.drawFastVLine(125,7,18,ST7735_BLUE);
	tft.drawFastVLine(127,7,18,ST7735_BLUE);
	tft.drawFastVLine(135,7,18,ST7735_BLUE);
	tft.drawFastVLine(137,7,18,ST7735_BLUE);
	tft.drawFastVLine(145,7,18,ST7735_BLUE);
	tft.drawFastVLine(147,7,18,ST7735_BLUE);
	tft.drawFastVLine(155,7,19,ST7735_BLUE);
	tft.drawFastVLine(126,7,19,ST7735_BLACK);
	tft.drawFastVLine(136,7,19,ST7735_BLACK);
	tft.drawFastVLine(146,7,19,ST7735_BLACK);
	tft.drawFastHLine(117,16,39,ST7735_BLACK);
	
	// set voice on LED	
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
	
	enableScope(true);	
}

//*************************************************************************
// draw Osc1 page
//*************************************************************************
FLASHMEM void drawOsc1Page (void)
{
	float myValue = 0;
	int value = 0;
	int parameter = 0;
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	
	if (myPageShiftStatus[PageNr] == false) {	// shift key disabled
		
		tft.setCursor(0,0);
		tft.fillRect(0,0,160,13,ST7735_GRAY);
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		tft.setCursor(5,3);
		tft.println("OSC");
		tft.setCursor(24,3);
		tft.println("1");
		tft.setCursor(5,21);
		tft.setTextColor(ST7735_GRAY);
		tft.println("WAVE");
		tft.setCursor(5,40);
		tft.println("PITCH");
		tft.setCursor(5,59);
		tft.println("P.ENV");
		tft.setCursor(5,78);
		tft.println("GLIDE");
		tft.setCursor(5,97);
		tft.println("LEVEL");
		tft.setCursor(85,59);
		tft.println("PWAMT");
		tft.setCursor(85,78);
		tft.println("PWMOD");
		tft.setCursor(85,97);
		tft.println("OSCMIX");
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(5,115);
		tft.println("SELECT");
		tft.setCursor(50,115);
		tft.println("VALUE");
		tft.setCursor(95,115);
		tft.println("BANK");
		tft.setCursor(135,115);
		tft.println("---");

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
		tft.drawFastVLine(128,79,5,ST7735_YELLOW);
		tft.drawFastVLine(129,80,3,ST7735_YELLOW);
		tft.drawPixel(130,81,ST7735_YELLOW);
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
		tft.setCursor(33,3);
		tft.setTextColor(ST7735_LIGHTGRAY);
		tft.print("SUB");
		
		// draw WaveNr and WaveBank ---------------------------------------
		tft.setCursor(56,21);
		tft.setTextColor(ST7735_WHITE);
		tft.print(oscWaveformA);
		draw_Waveform(oscWaveformA, ST7735_RED);
		
		// draw WaveBank
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
		float Value1 = (pitchEnvA / OSCMODMIXERMAX);
		for (int i = 0; i < 128; i++) {
			float Value2 = LINEARCENTREZERO[i];
			if (Value1 <= Value2){
				value = i;
				break;
			}
		}
		printOscPitchEnv(value);

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

		// PWA ------------------------------------------------------------
		for (uint8_t i = 0; i < 128; i++) {
			myValue = LINEARCENTREZERO[i];
			if (pwA == myValue) {
				value = i;
				break;
			}
		}
		tft.setCursor(135,59);
		tft.setTextColor(ST7735_WHITE);
		tft.print(value);

		// PWM Rate -------------------------------------------------------
		printPWMrate();
		
		// OSCMIX ---------------------------------------------------------
		//tft.fillRoundRect(133,95,22,10,2,ST7735_BLUE);
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
	
	// Shift key enabled --------------------------------------------------
	else
	{
		tft.setCursor(0,0);
		tft.fillRect(0,0,160,13,ST7735_GRAY);
		tft.setTextColor(ST7735_GREEN);
		tft.setFont(NULL);
		tft.setTextSize(0);
		tft.setCursor(5,3);
		tft.println("OSC");
		tft.setCursor(24,3);
		tft.println("1");
		tft.setCursor(5,21);
		tft.setTextColor(ST7735_GRAY);
		tft.println("WSHAPE");
		tft.setCursor(5,40);
		tft.println("GAIN");
		tft.setCursor(5,59);
		tft.println("SYNC");
		tft.setCursor(5,78);
		tft.println("TRANSP");
		tft.setCursor(5,97);
		tft.println("TUNE");
		tft.setCursor(85,59);
		tft.println("NOISE");
		tft.setCursor(85,78);
		tft.println("OSCMOD");
		tft.setCursor(85,97);
		tft.println("LFO2MOD");	
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(5,115);
		tft.println("SELECT");
		tft.setCursor(50,115);
		tft.println("VALUE");
		tft.setCursor(95,115);
		tft.println("---");
		tft.setCursor(135,115);
		tft.println("---");
		
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
		
		// Wafeshaper -----------------------------------------------------
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(56, 21);
		if (WShaperNo == 0) {
			tft.print("OFF");
		}
		else tft.print(WShaperNo);
		drawWaveshaperCurve(WShaperNo);
		
		// Waveshaper Drive
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(56, 40);
		tft.print(WShaperDrive,1);
		
		// SYNC -----------------------------------------------------------
		tft.fillRoundRect(54,57,22,10,2,ST7735_BLUE);
		tft.setTextColor(ST7735_WHITE);
		if (oscDetuneSync == true) {
			tft.setCursor(59,59);
			tft.print("ON");
			} else {
			tft.setCursor(56,59);
			tft.print("OFF");
		}
		
		// Transpose ------------------------------------------------------
		tft.setCursor(56,78);
		tft.setTextColor(ST7735_WHITE);
		if (oscTranspose > 0) {
			tft.print("+");
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
			tft.setCursor(135,59);
			tft.setTextColor(ST7735_WHITE);
			tft.print("OFF");
			tft.fillRect(128,59,5,7,ST7735_BLACK);
		}
		else if (value < 62) {
			tft.println(62 - value);
			tft.setCursor(122,59);
			tft.fillRect(122,59,5,7,ST7735_BLACK);
			tft.setTextColor(ST7735_WHITE);
			tft.print("W");
		}
		else {
			tft.println(value - 64);
			tft.setCursor(122,59);
			tft.fillRect(122,59,5,7,ST7735_BLACK);
			tft.setTextColor(ST77XX_MAGENTA);
			tft.print("P");
		}
		
		// OSCMOD ---------------------------------------------------------
		tft.fillRoundRect(133,78,21,8,2,ST7735_BLUE);
		tft.setCursor(135,78);
		tft.setTextColor(ST7735_WHITE);
		if (oscFX == 0) {
			tft.println("OFF");
		}
		else if (oscFX == 1) {
			tft.println("XOR");
		}
		else if (oscFX == 2) {
			tft.println("XMO");
		}
		else if (oscFX == 3) {
			tft.println("MOD");
		}
		else if (oscFX == 4) {
			tft.println("AND");
		}
		else if (oscFX == 5) {
			tft.setCursor(137,78);
			tft.println("OR");
		}
		else if (oscFX == 6) {
			tft.setCursor(137,78);
			tft.println("FM");
		}
		
		// VCFMOD ---------------------------------------------------------
		uint8_t oscvcfmod = (OscVCFMOD * 127);
		tft.fillRoundRect(133,97,21,8,2,ST7735_BLUE);
		tft.setCursor(135,97);
		tft.setTextColor(ST7735_WHITE);
		tft.print(oscvcfmod);
		
		// Update parameter -----------------------------------------------
		ParameterNr = ParameterNrMem[3];	// SUB Page
		drawParamterFrame(PageNr, ParameterNr);
	}
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
	int parameter = 0;
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	
	
	if (myPageShiftStatus[PageNr] == false) {	// shift key disabled
		tft.setCursor(0,0);
		tft.fillRect(0,0,160,13,ST7735_GRAY);
		tft.setTextColor(ST7735_WHITE);
		tft.setFont(NULL);
		tft.setTextSize(0);
		tft.setCursor(5,3);
		tft.println("OSC");
		tft.setCursor(24,3);
		tft.println("2");
		tft.setCursor(5,21);
		tft.setTextColor(ST7735_GRAY);
		tft.println("WAVE");
		tft.setCursor(5,40);
		tft.println("PITCH");
		tft.setCursor(5,59);
		tft.println("P.ENV");
		tft.setCursor(5,78);
		tft.println("DETUNE");
		tft.setCursor(5,97);
		tft.println("LEVEL");
		tft.setCursor(85,59);
		tft.println("PWAMT");
		tft.setCursor(85,78);
		tft.println("PWMOD");
		tft.setCursor(85,97);
		tft.print("OSCMIX");
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(5,115);
		tft.println("SELECT");
		tft.setCursor(50,115);
		tft.println("VALUE");
		tft.setCursor(95,115);
		tft.println("BANK");
		tft.setCursor(135,115);
		tft.println("---");
		
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
		tft.drawFastVLine(128,79,5,ST7735_YELLOW);
		tft.drawFastVLine(129,80,3,ST7735_YELLOW);
		tft.drawPixel(130,81,ST7735_YELLOW);
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
		tft.setCursor(33,3);
		tft.setTextColor(ST7735_LIGHTGRAY);
		tft.print("SUB");
		
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
		float Value1 = (pitchEnvB / OSCMODMIXERMAX);
		for (int i = 0; i < 128; i++) {
			float Value2 = LINEARCENTREZERO[i];
			if (Value1 <= Value2){
				value = i;
				break;
			}
		}
		printOscPitchEnv(value);
		
		// Detune ---------------------------------------------------------
		printUnisonDetune();
		
		// Level ----------------------------------------------------------
		printOscLevel();
		
		// PWMAMT (pwB) ---------------------------------------------------
		for (uint8_t i = 0; i < 128; i++) {
			myValue = LINEARCENTREZERO[i];
			if (pwB == myValue) {
				value = i;
				break;
			}
		}
		tft.setCursor(135,59);
		tft.setTextColor(ST7735_WHITE);
		tft.print(value);
		
		// PWM Rate -------------------------------------------------------
		printPWMrate();
		
		// OscMix ---------------------------------------------------------
		//tft.fillRoundRect(133,95,22,10,2,ST7735_BLUE);
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
		ParameterNr = ParameterNrMem[2]; // SUB Page
		drawParamterFrame(PageNr, ParameterNr);
	}
	
	// Shift key enabled --------------------------------------------------
	else
	{
		tft.setCursor(0,0);
		tft.fillRect(0,0,160,13,ST7735_GRAY);
		tft.setTextColor(ST7735_GREEN);
		tft.setFont(NULL);
		tft.setTextSize(0);
		tft.setCursor(5,3);
		tft.println("OSC");
		tft.setCursor(24,3);
		tft.println("2");
		tft.setCursor(5,21);
		tft.setTextColor(ST7735_GRAY);
		tft.println("WSHAPE");
		tft.setCursor(5,40);
		tft.println("GAIN");
		tft.setCursor(5,59);
		tft.println("SYNC");
		tft.setCursor(5,78);
		tft.println("TRANSP");
		tft.setCursor(5,97);
		tft.println("TUNE");
		tft.setCursor(85,59);
		tft.println("NOISE");
		tft.setCursor(85,78);
		tft.println("OSCMOD");
		tft.setCursor(85,97);
		tft.println("LFO2MOD");
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(5,115);
		tft.println("SELECT");
		tft.setCursor(50,115);
		tft.println("VALUE");
		tft.setCursor(95,115);
		tft.println("---");
		tft.setCursor(135,115);
		tft.println("---");
		
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
		
		// Wafeshaper and Gain --------------------------------------------
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(56, 21);
		if (WShaperNo == 0) {
			tft.print("OFF");
		}
		else tft.print(WShaperNo);
		drawWaveshaperCurve(WShaperNo);
		
		// Waveshaper Gain
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(56, 40);
		tft.print(WShaperDrive,1);
		
		// SYNC -----------------------------------------------------------
		tft.fillRoundRect(54,57,22,10,2,ST7735_BLUE);
		tft.setTextColor(ST7735_WHITE);
		if (oscDetuneSync == true) {
			tft.setCursor(59,59);
			tft.print("ON");
			} else {
			tft.setCursor(56,59);
			tft.print("OFF");
		}
		
		// Transpose ------------------------------------------------------
		tft.setCursor(56,78);
		tft.setTextColor(ST7735_WHITE);
		if (oscTranspose > 0) {
			tft.print("+");
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
			tft.setCursor(135,59);
			tft.setTextColor(ST7735_WHITE);
			tft.print("OFF");
			tft.fillRect(128,59,5,7,ST7735_BLACK);
		}
		else if (value < 62) {
			tft.println(62 - value);
			tft.setCursor(122,59);
			tft.fillRect(122,59,5,7,ST7735_BLACK);
			tft.setTextColor(ST7735_WHITE);
			tft.print("W");
		}
		else {
			tft.println(value - 64);
			tft.setCursor(122,59);
			tft.fillRect(122,59,5,7,ST7735_BLACK);
			tft.setTextColor(ST77XX_MAGENTA);
			tft.print("P");
		}
		
		// OSCMOD ---------------------------------------------------------
		tft.fillRoundRect(133,78,21,8,2,ST7735_BLUE);
		tft.setCursor(135,78);
		tft.setTextColor(ST7735_WHITE);
		if (oscFX == 0) {
			tft.println("OFF");
		}
		else if (oscFX == 1) {
			tft.println("XOR");
		}
		else if (oscFX == 2) {
			tft.println("XMO");
		}
		else if (oscFX == 3) {
			tft.println("MOD");
		}
		else if (oscFX == 4) {
			tft.println("AND");
		}
		else if (oscFX == 5) {
			tft.setCursor(137,78);
			tft.println("OR");
		}
		else if (oscFX == 6) {
			tft.setCursor(137,78);
			tft.println("FM");
		}
		
		// VCFMOD ---------------------------------------------------------
		uint8_t oscvcfmod = (OscVCFMOD * 127);
		tft.fillRoundRect(133,97,21,8,2,ST7735_BLUE);
		tft.setCursor(135,97);
		tft.setTextColor(ST7735_WHITE);
		tft.print(oscvcfmod);
		
		// Update parameter -----------------------------------------------
		ParameterNr = ParameterNrMem[3]; // SUB Page
		drawParamterFrame(PageNr, ParameterNr);
	}
	
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
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
	
	// State Variable Filter ----------------------------------------------
	if (Filter == 1) { 
		if (myPageShiftStatus[PageNr] == false) {	// Main page enabled
			tft.setTextColor(ST7735_WHITE);
			tft.setFont(NULL);
			tft.setTextSize(0);
			tft.setCursor(5,3);
			tft.println("FILTER");
			tft.setTextColor(ST7735_LIGHTGRAY);
			tft.setCursor(5,19);
			tft.print("STATE VARIABLE");
			tft.setTextColor(ST7735_WHITE);
			tft.setCursor(10, 103);
			tft.print("CUT");
			tft.setCursor(50, 103);
			tft.print("RES");
			tft.setCursor(90, 103);
			tft.print("ENV");
			tft.setCursor(130, 103);
			tft.print("TYP");
			
			// draw inactive SUB maker
			tft.setCursor(45,3);
			tft.setTextColor(ST7735_LIGHTGRAY);
			tft.print("SUB");
		}
		else {	// SUB page enabled
			tft.setTextColor(ST7735_GREEN);
			tft.setFont(NULL);
			tft.setTextSize(0);
			tft.setCursor(5,3);
			tft.println("FILTER");
			tft.setCursor(5,19);
			tft.setTextColor(ST7735_LIGHTGRAY);
			tft.print("STATE VARIABLE");
			tft.setTextColor(ST7735_WHITE);
			tft.setCursor(9, 103);
			tft.print("KEY");
			tft.setCursor(50, 103);
			tft.print("VEL");
			tft.setCursor(90, 103);
			tft.print("LFO");
			tft.setCursor(130, 103);
			tft.print("---");
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
		//uint16_t Frq = 0;
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
		float Reso = ((filterRes - 1.1f) / 3.9f);
		for (int i = 0; i < 128; i++) {
			float myresonance = POWER[i];
			if ((myresonance - Reso) < 0.00001f) {
				FilterReso = 128;
			}
			else {
				FilterReso = i-1;
				break;
			}
		}
		
		// calc Key Trakking
		float k_factor = 100;
		FilterKey = (uint8_t)(k_factor * keytrackingAmount);
		
		// calc Filter Typ (Filter-Mix)
		if (filterMix < 0) {
			Filter_mix = 125;
		}
		else {
			Filter_mix = (uint8_t)(128.0f * filterMix);
		}
		draw_filter_curves(Frq_value >> 1, FilterReso, Filter_mix);
		FilterCut = Frq_value;
		FilterRes = FilterReso;
		FilterMix = Filter_mix;
		
		// Main page enabled -----------------------
		if (myPageShiftStatus[PageNr] == false) {
			//printFilterFrq(filterFreq);
			printDataValue (0, (DIV100 * (FilterCut >> 1)));
			printRedMarker (0, (FilterCut >> 1));
			printFilterRes(FilterReso * 0.788f);	// value 0-100
			
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
			printFilterEnvAmt(EnvVal);
			printRedMarker (2,EnvVal);
			
			// print FilterMix
			printFilterTyp(Filter_mix);
		}
		
		// SUB Page enabled
		else {
			
			// Filter Keytraking
			printRedMarker (0, (1.27f * FilterKey));
			printPercentValue(0,FilterKey);
			
			// calc and print Filter velocity
			uint8_t myVelo = ((127 * myFilVelocity) * 2);
			printRedMarker (1, myVelo);
			printPercentValue(1, (DIV100 * myVelo));
			
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
			
			// rest is zerro
			printDataValue (3, 0);
			printRedMarker (3, 0);
		}
	} 
	else {
		// Ladder Filter ----------------------------------------------------
			if (myPageShiftStatus[PageNr] == false) {	// Main Page
			tft.setTextColor(ST7735_WHITE);
			tft.setFont(NULL);
			tft.setTextSize(0);
			tft.setCursor(5,3);
			tft.println("FILTER");
			tft.setTextColor(ST7735_LIGHTGRAY);
			tft.setCursor(5,19);
			tft.print("LADDER VERSION");
			tft.setTextColor(ST7735_WHITE);
			tft.setCursor(10, 103);
			tft.print("CUT");
			tft.setCursor(50, 103);
			tft.print("RES");
			tft.setCursor(90, 103);
			tft.print("ENV");
			tft.setCursor(130, 103);
			tft.print("DRV");
			
			// draw inactive SUB maker
			tft.setCursor(45,3);
			tft.setTextColor(ST7735_LIGHTGRAY);
			tft.print("SUB");
		}
		else {									// SUB Page
			tft.setTextColor(ST7735_GREEN);
			tft.setFont(NULL);
			tft.setTextSize(0);
			tft.setCursor(5,3);
			tft.println("FILTER");
			tft.setTextColor(ST7735_LIGHTGRAY);
			tft.setCursor(5,19);
			tft.print("LADDER VERSION");
			tft.setTextColor(ST7735_WHITE);
			tft.setCursor(9, 103);
			tft.print("KEY");
			tft.setCursor(50, 103);
			tft.print("VEL");
			tft.setCursor(90, 103);
			tft.print("PBG");
			tft.setCursor(130, 103);
			tft.print("LFO");
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
		//uint16_t Frq = 0;
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
		
		// calc Resonancy Ladder Filter
		float Reso = (filterRes / 1.8f);
		for (int i = 0; i < 128; i++) {
			float myresonance = LINEAR[i];
			if ((myresonance - Reso) < 0.00001f) {
				FilterReso = 128;
			}
			else {
				FilterReso = i-1;
				break;
			}
		}
		
		// calc Key Trakking
		float k_factor = 100;
		FilterKey = (uint8_t)(k_factor * keytrackingAmount);
		Filter_mix = 0;
		draw_filter_curves(Frq_value >> 1, FilterReso, Filter_mix);
		FilterCut = Frq_value;
		FilterRes = FilterReso;
		FilterMix = Filter_mix;
		
		// Filter main Page enabled ---------------------------
		if (myPageShiftStatus[PageNr] == false) {
			
			// print cutoff;
			printDataValue (0, (DIV100 * (FilterCut >> 1)));
			printRedMarker (0, (FilterCut >> 1));
			
			// print resonance
			printFilterRes(FilterReso * 0.788f);	// value 0-100
			
			// Filter Enveloope
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
			printFilterEnvAmt(EnvVal);
			printRedMarker (2,EnvVal);
			
			// print Drive
			if (LadderFilterDrive <= 1) {
				LadderFilterDrive = 1;
			}
			printDataValue(3, LadderFilterDrive);
			printRedMarker (3, LadderFilterDrive);
		}
		// SUB Page enabled -------------------------------------
		else {
			// Filter Keytraking
			printRedMarker (0, (1.27f * FilterKey));
			printPercentValue(0,FilterKey);
			
			// calc and print Filter velocity
			uint8_t myVelo = ((127 * myFilVelocity) * 2);
			printRedMarker (1, myVelo);
			printPercentValue(1, (DIV100 * myVelo));
			
			// print passband Gain
			printDataValue(2, LadderFilterpassbandgain);
			printRedMarker (2, LadderFilterpassbandgain);
			
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
			printDataValue (3, value);
			printRedMarker (3, value);
		}
	} 
	
	MidiStatusSymbol = 2; MidiSymbol();
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
		tft.setCursor(5,3);
		tft.print("VCF");
		tft.setCursor(25,3);
		tft.print("ENV");
		
		tft.fillRoundRect(109,25,38,19,2,ST7735_ORANGE);
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(111,27);
		tft.setTextSize(2);
		tft.print("VCF");
		
		
		// draw inactive SUB maker
		tft.setCursor(45,3);
		tft.setTextColor(ST7735_LIGHTGRAY);
		tft.setTextSize(NULL);
		tft.print("SUB");
		
		tft.setTextColor(ST7735_WHITE);
		// shift key disabled
		if (myPageShiftStatus[PageNr] == false) {
			tft.setCursor(10, 103);
			tft.print("ATK");
			tft.setCursor(50, 103);
			tft.print("DCY");
			tft.setCursor(90, 103);
			tft.print("SUS");
			tft.setCursor(130, 103);
			tft.print("REL");
		}
		// shift key akctivated
		else {
			tft.setTextColor(ST7735_GREEN);
			tft.setFont(NULL);
			tft.setTextSize(0);
			tft.setCursor(5,3);
			tft.println("VCF");
			tft.setCursor(25,3);
			tft.print("ENV");
			tft.setTextColor(ST7735_WHITE);
			tft.setCursor(10, 103);
			tft.print("CUR");
			tft.setCursor(10 + 40, 103);
			tft.print("---");
			tft.setCursor(10 + 40 + 40, 103);
			tft.print("---");
			tft.setCursor(6 + 40 + 40 + 40+4, 103);
			tft.print("---");
		}
		
		uint8_t ATKvalue = 0; 
		uint8_t DCYvalue = 0; 
		uint8_t SUSvalue = 0; 
		uint8_t RELvalue = 0;
		
		
		for (int i = 0; i < 128; i++) {
			float timeMS = ENVTIMES[i];
			if (filterAttack <= timeMS){
				ATKvalue = i;
				break;
			}
		}
		for (int i = 0; i < 128; i++) {
			float timeMS = ENVTIMES[i];
			if (filterDecay <= timeMS){
				DCYvalue = i;
				break;
			}
		}
		for (int i = 0; i < 128; i++) {
			float timeMS = LINEAR[i];
			if (filterSustain <= timeMS){
				SUSvalue = i;
				break;
			}
		}
		for (int i = 0; i < 128; i++) {
			float timeMS = ENVTIMES[i];
			if (filterRelease <= timeMS){
				RELvalue = i;
				break;
			}
		}
		
		drawEnvCurve(ATKvalue, DCYvalue, SUSvalue, RELvalue);
		
		// shift key disabled
		if (myPageShiftStatus[PageNr] == false) {
			printEnvATKvalues(ATKvalue);
			printEnvDCYvalues(DCYvalue);
			printEnvSUSvalues(SUSvalue);
			printEnvRELvalues(RELvalue);
			Env1Atk = ATKvalue;
			Env1Dcy = DCYvalue;
			Env1Sus = SUSvalue;
			Env1Rel = RELvalue;
		}
		// shift key enabled
		else {
			
			printDataValue (1, 0);
			printRedMarker (1, 0);
			printDataValue (2, 0);
			printRedMarker (2, 0);
			printDataValue (3, 0);
			printRedMarker (3, 0);
			
			// print CUR value
			int8_t curVal = (envelopeType1 * 7.47f);
			curVal += 64;
			printRedMarker (0, curVal);
			
			if (envelopeType1 < 0) {
				tft.setCursor(13,116);
				tft.setTextColor(ST7735_GRAY);
				tft.print(envelopeType1);
			}
			else if (envelopeType1 == 0) {
				tft.setCursor(15,116);
				tft.setTextColor(ST7735_GRAY);
				tft.print(envelopeType1);
			}
			else if (envelopeType1 > 0) {
				tft.setCursor(13,116);
				tft.setTextColor(ST7735_GRAY);
				tft.print("+");
				tft.print(envelopeType1);
			}
		}
				
		MidiStatusSymbol = 2; MidiSymbol();
		drawPeakmeter();
}

//*************************************************************************
// draw Amp Env Page
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
	tft.setCursor(5,3);
	tft.print("AMP");
	tft.setCursor(25,3);
	tft.print("ENV");
	
	// draw inactive SUB maker
	tft.setCursor(45,3);
	tft.setTextColor(ST7735_LIGHTGRAY);
	tft.print("SUB");
	
	tft.setTextColor(ST7735_WHITE);
	
	// AMP Main page enabled
	if (myPageShiftStatus[PageNr] == false) {
		tft.setCursor(10, 103);
		tft.print("ATK");
		tft.setCursor(50, 103);
		tft.print("DCY");
		tft.setCursor(90, 103);
		tft.print("SUS");
		tft.setCursor(130, 103);
		tft.print("REL");
	}
	// AMP SUB page enabled
	else {
		tft.setTextColor(ST7735_GREEN);
		tft.setFont(NULL);
		tft.setTextSize(0);
		tft.setCursor(5,3);
		tft.println("AMP");
		tft.setCursor(25,3);
		tft.print("ENV");
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(10, 103);
		tft.print("CUR");
		tft.setCursor(10 + 40, 103);
		tft.print("VEL");
		tft.setCursor(10 + 40 + 40, 103);
		tft.print("---");
		tft.setCursor(6 + 40 + 40 + 40+4, 103);
		tft.print("---");
	}
	
	uint8_t ATKvalue = 0;
	uint8_t DCYvalue = 0;
	uint8_t SUSvalue = 0;
	uint8_t RELvalue = 0;
	uint8_t DELvalue = 0;
	uint8_t HOLvalue = 0;
	
	for (int i = 0; i < 128; i++) {
		float timeMS = ENVTIMES[i];
		if (ampAttack <= timeMS){
			ATKvalue = i;
			break;
		}
	}
	for (int i = 0; i < 128; i++) {
		float timeMS = ENVTIMES[i];
		if (ampDecay <= timeMS){
			DCYvalue = i;
			break;
		}
	}
	for (int i = 0; i < 128; i++) {
		float timeMS = LINEAR[i];
		if (ampSustain <= timeMS){
			SUSvalue = i;
			break;
		}
	}
	for (int i = 0; i < 128; i++) {
		float timeMS = ENVTIMES[i];
		if (ampRelease <= timeMS){
			RELvalue = i;
			break;
		}
	}
	
	drawEnvCurve(ATKvalue, DCYvalue, SUSvalue, RELvalue);
	
	// Main page enabled
	if (myPageShiftStatus[PageNr] == false) {
		printEnvATKvalues(ATKvalue);
		printEnvDCYvalues(DCYvalue);
		printEnvSUSvalues(SUSvalue);
		printEnvRELvalues(RELvalue);
		Env2Atk = ATKvalue;
		Env2Dcy = DCYvalue;
		Env2Sus = SUSvalue;
		Env2Rel = RELvalue;
	}
	// Sub page enabled
	else {
		// print VEL value
		uint8_t myVelo = ((127 * myAmpVelocity) * 2) + 0.1f;	// 0.1f fürs runden
		printRedMarker (1, myVelo);
		printPercentValue(1, (DIV100 * myVelo)); // to %
		
		printDataValue (2, 0);
		printRedMarker (2, 0);
		printDataValue (3, 0);
		printRedMarker (3, 0);
		
		// print CUR value
		int8_t curVal = (envelopeType2 * 7.47f);
		curVal += 64;
		printRedMarker (0, curVal);
		
		if (envelopeType2 < 0) {
			tft.setCursor(13,116);
			tft.setTextColor(ST7735_GRAY);
			tft.print(envelopeType2);
		}
		else if (envelopeType2 == 0) {
			tft.setCursor(15,116);
			tft.setTextColor(ST7735_GRAY);
			tft.print(envelopeType2);
		}
		else if (envelopeType2 > 0) {
			tft.setCursor(13,116);
			tft.setTextColor(ST7735_GRAY);
			tft.print("+");
			tft.print(envelopeType2);
		}
	}
	
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
	tft.setCursor(5,3);
	tft.println("FxDSP");
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(10, 103);
	tft.print("SEL");
	tft.setCursor(50, 103);
	tft.print("VAL");
	tft.setCursor(90, 103);
	tft.print("MIX");
	tft.setCursor(130, 103);
	tft.print("PRG");
	tft.setCursor(5, 88);
	tft.setTextColor(ST7735_GRAY);
	tft.print("P1");
	tft.setCursor(58, 88);
	tft.print("P2");
	tft.setCursor(112, 88);
	tft.print("P3");
	tft.drawRect(0,14,160,86,ST7735_GRAY);
	tft.drawFastHLine(0,83,160,ST7735_GRAY);
	tft.drawFastVLine(53,83,16,ST7735_GRAY);
	tft.drawFastVLine(107,83,16,ST7735_GRAY);
	tft.fillRoundRect(22,86,25,10,2,ST7735_BLUE);
	tft.fillRoundRect(76,86,25,10,2,ST7735_BLUE);
	tft.fillRoundRect(128,86,25,10,2,ST7735_BLUE);
	tft.setTextColor(ST7735_GRAY);
	tft.setCursor(5,28);
	tft.print("Vol");
	tft.setCursor(75,72);
	tft.print("Time/Feedb.");
	if (FxPrgNo >= 1) {
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
		printRedMarker (3, FxPrgNo);
		printFxName(FxPrgNo);
		printFxPOT(myFxSelValue, FxPrgNo);
	}
	else {
		setFxPrg (5);
		tft.fillRect(65,27,90,9,ST7735_BLACK);
		tft.setTextColor(ST7735_RED);
		tft.setCursor(65,28);
		tft.print("OFF");
		tft.fillRect(122, 120, 30, 7, ST7735_BLACK);
		tft.setTextColor(ST7735_GRAY);
		tft.setCursor(130,116);
		tft.print("OFF");
		analogWrite(PWM4, 0);
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
		setLED(1, false);
		tft.fillRect(3,71,155,10,ST7735_BLACK);
		tft.setCursor(75,72);
		tft.setTextColor(ST7735_GRAY);
		tft.print("Time/Feedb.");
	}
	
	tft.setCursor(65,40);
	tft.setTextColor(ST7735_GRAY);
	tft.print("Clk");
	tft.fillRect(85,40,29,7,ST7735_BLACK);
	tft.setCursor(85,40);
	tft.print(FxClkRate / 1000);
	tft.print("Khz");
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
	tft.setCursor(5,3);
	tft.println("LFO1 (Osc)");
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(3, 103);
	tft.print("SHAPE");
	tft.setCursor(47, 103);
	tft.print("RATE");
	tft.setCursor(90, 103);
	tft.print("AMT");
	tft.setCursor(130, 103);
	tft.print("SYN");
		
	// draw LFO Shapes and Frames
	drawLFOshape(ST7735_RED);
	tft.drawRect(2,22,50,35,ST7735_GRAY);
	tft.drawRect(54,22,50,35,ST7735_GRAY);
	tft.drawRect(106,22,50,35,ST7735_GRAY);
	tft.drawRect(2,60,50,35,ST7735_GRAY);
	tft.drawRect(54,60,50,35,ST7735_GRAY);
	tft.drawRect(106,60,50,35,ST7735_GRAY);
	
	// LFO Waveform
	uint8_t myLFOwaveNo = convertLFOWaveform(oscLFOWaveform);
	drawLFOframe(myLFOwaveNo);
	printDataValue (0, (1+ myLFOwaveNo));
	printRedMarker (0, (myLFOwaveNo * 25));
	
	// LFO Rate
	float LFOfreq = (oscLfoRate / LFOMAXRATE);
	uint8_t value = 0;
	for (int i = 0; i < 128; i++) {
		float lforate = POWER[i];
		if (LFOfreq <= lforate){
			value = i;
			break;
		}
	}
	if (value <= 1) {
		value = 1;
	}
	printDataValue (1, value);
	printRedMarker (1, value);
	
	// LFO AMT
	float LFOamt = (oscLfoAmt * FILTERMODMIXERMAX);
	value = 0;
	for (int i = 0; i < 128; i++) {
		float LFOoscAmt = LINEARLFO[i];
		if (LFOamt <= LFOoscAmt){
			value = i;
			break;
		}
	}
	printDataValue (2, value);
	printRedMarker (2, value);
	
	// LFO Trigger (Sync)
	if (oscLfoRetrig == 0) {
		tft.setCursor(130, 116);
		tft.print("OFF");
		printRedMarker (3, 0);
	}
	else if (LFO1phase == 180.0f ) {
		tft.setCursor(134, 116);
		tft.print("0");
		tft.drawRect(141,116,3,3,ST7735_GRAY);
		printRedMarker (3, 64);
	}
	else {
		tft.setCursor(128, 116);
		tft.print("180");
		tft.drawRect(147,120,3,3,ST7735_GRAY);
		printRedMarker (3, 128);
	}
	
	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw LFO2 Page (Filter)
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
	tft.setCursor(5,3);
	tft.println("LFO2 (Filter)");
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(3, 103);
	tft.print("SHAPE");
	
	if (filterLfoRetrig == 3) {
		tft.setCursor(49, 103);
		tft.print("DIV");
		} else {
		tft.setCursor(47, 103);
		tft.print("RATE");
	}
	tft.setCursor(90, 103);
	tft.print("AMT");
	tft.setCursor(130, 103);
	tft.print("SYN");
		
	// draw LFO Shapes and Frames
	drawLFOshape(ST7735_ORANGE);
	tft.drawRect(2,22,50,35,ST7735_GRAY);
	tft.drawRect(54,22,50,35,ST7735_GRAY);
	tft.drawRect(106,22,50,35,ST7735_GRAY);
	tft.drawRect(2,60,50,35,ST7735_GRAY);
	tft.drawRect(54,60,50,35,ST7735_GRAY);
	tft.drawRect(106,60,50,35,ST7735_GRAY);
	
	// LFO Waveform
	uint8_t myLFOwaveNo = convertLFOWaveform(filterLfoWaveform);
	drawLFOframe(myLFOwaveNo);
	printDataValue (0, (1+ myLFOwaveNo));
	printRedMarker (0, (myLFOwaveNo * 25));
	
	// LFO Rate
	printRedMarker (1, myFilterLFORateValue);
	if (filterLfoRetrig == 3) {
		filterLFOTimeDivStr = LFOTEMPOSTR[myFilterLFORateValue];
		int16_t x1, y1;
		uint16_t w1, h1;
		tft.getTextBounds(filterLFOTimeDivStr , 0, 0, &x1, &y1, &w1, &h1); // string width in pixels
		tft.setCursor(58 - (w1 / 2), 116);	// print string in the middle
		tft.print(filterLFOTimeDivStr);
		
		} else {
			if (myFilterLFORateValue <= 1) {
				myFilterLFORateValue = 1;
			}
			printDataValue (1, myFilterLFORateValue);
		}
		
		if (filterLfoRetrig == 3 && MidiSyncSwitch == false) {
			tft.fillRect(46, 116, 24, 7, ST7735_BLACK);
			tft.setTextColor(ST7735_RED);
			tft.setCursor(46,116);
			tft.print("CLK?");
		}
	
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
	
	// LFO Sync
	switch(filterLfoRetrig) {
		case 0:
		tft.setCursor(130, 116);
		tft.print("OFF");
		printRedMarker (3, 0);
		break;
		case 1:
		tft.setCursor(134, 116);
		tft.print("0");
		tft.drawRect(141,120,3,3,ST7735_GRAY);
		printRedMarker (3, 42);
		break;
		case 2:
		tft.setCursor(128, 116);
		tft.print("180");
		tft.drawRect(147,120,3,3,ST7735_GRAY);
		printRedMarker (3, 84);
		break;
		case 3:
		tft.setCursor(128, 116);
		tft.print("MIDI");
		printRedMarker (3, 127);
		break;
	}

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
	tft.setCursor(5,3);
	tft.print("SEQ");
	
	// draw inactive SUB maker
	tft.setCursor(26,3);
	tft.setTextColor(ST7735_LIGHTGRAY);
	tft.print("SUB");
	
	} else {
		tft.setTextColor(ST7735_GREEN);
		tft.setFont(NULL);
		tft.setTextSize(0);
		tft.setCursor(5,3);
		tft.print("SEQ");
	}
	tft.setTextColor(ST7735_GREEN);
	tft.setCursor(65,3);
	tft.print("PAT:");
	if (SEQPatternNo == 0) {
		tft.print("--");
	} else {
		if (SEQPatternNo < 10) {
			tft.print("0");
		}
		tft.print(SEQPatternNo);
	}
	
	// draw all 16 Note Steps
	for (uint8_t i = 0; i < 16; i++) {
		drawSEQpitchValue2(i);
	}
	
	// draw red Step frame
	drawSEQStepFrame(SEQselectStepNo);
	
	// draw number of steps
	drawSEQstepNumbersMarker(SEQstepNumbers);
	
	// Main Page ---------------------------------------------------
	if (myPageShiftStatus[PageNr] == false) {
		tft.setTextColor(ST7735_WHITE);
		tft.setCursor(7, 103);
		tft.print("STEP");
		tft.setCursor(43, 103);
		tft.print("PITCH");
		tft.setCursor(89, 103);
		tft.print("BPM");
		tft.setCursor(130, 103);
		tft.print("DIV");
		printDataValue (0, SEQselectStepNo + 1);
		printRedMarker (0, SEQselectStepNo * 8);
		printSeqPitchNote();
		if (SEQMidiClkSwitch == false) {
			printDataValue (2, SEQbpmValue);
		} else {
			tft.setCursor(86,116);
			tft.print("MIDI");
		}
		printRedMarker (2, ((SEQbpmValue - 50)/1.484f));
		
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
		tft.setCursor(10, 103);
		tft.print("LEN");
		tft.setCursor(46, 103);
		tft.print("TIME");
		tft.setCursor(89, 103);
		tft.print("DIR");
		tft.setCursor(126, 103);
		tft.print("MODE");
		
		// calc GateTime
		uint8_t gateTimeVal = 0;
		for (uint8_t i = 0; i < 64; i++) {
			if ((16.01f - SEQGateTime) == SEQGATETIME[i]) {
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
			tft.setCursor(130,116);
			tft.print("KEY");
			} 
		else if (SEQmode == 1){
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(130,116);
			tft.print("TRP");
		}
		else {
			tft.setTextColor(ST7735_GRAY);
			tft.setCursor(130,116);
			tft.print("REC");
		}
		
		int mode = SEQmode << 6;
		if (mode >= 127) {
			mode = 127;
		}
		printRedMarker (3, mode);
	}

	MidiStatusSymbol = 2; MidiSymbol();
	drawPeakmeter();
}

//*************************************************************************
// draw System Page
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
	tft.setCursor(5,3);
#if Filter == 1
	tft.println("System V1.22");
#else
tft.println("System V1.22.L");
#endif
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(5,115);
	tft.println("SELECT");
	tft.setCursor(50,115);
	tft.println("VALUE");
	tft.setCursor(95,115);
	tft.println("---");
	tft.setCursor(135,115);
	tft.println("---");
	tft.setTextColor(ST7735_GRAY);
	tft.setCursor(5,21);
	tft.println("CPU-MHz");
	tft.setCursor(5,40);
	tft.println("CPU-TEMP");
	tft.setCursor(5,59);
	tft.println("CPU-MEM");
	tft.setCursor(5,78);
	tft.println("PIWHEEL");
	tft.setCursor(5,97);
	tft.println("MOWHEEL");
	tft.setCursor(85,21);
	tft.println("MIDICHA");
	tft.setCursor(85,40);
	tft.println("VELCURV");
	tft.setCursor(85,59);
	tft.println("UNISONO");
	tft.setCursor(85,78);
	tft.println("MIDICLK");
	tft.setCursor(85,97);
	tft.println("PCHANGE");
	
	// draw Rect and Lines
	for (uint8_t i = 0; i < 5; i++){
		tft.fillRoundRect(54,19+(19*i),22,10,2,ST7735_BLUE);
	}
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
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(56, 21);
	tft.print(CPUclk / 1000000);
	
	// CPU Temp
	printTemperature();
		
	// CPU Mem
	printCPUmon();
	
	// PitchWheel
	tft.setCursor(56,78);
	PitchWheelvalue = (PitchWheelAmt * 127);
	tft.print(PitchWheelvalue);
	
	// MODWheel
	tft.setCursor(56,97);
	MODWheelvalue = (MODWheelAmt * 127);
	tft.print(MODWheelvalue);
	
	// Midi Channel
	if (midiChannel == 0) {
		tft.setCursor(136,21);
		tft.print("ALL");
	}
	else {
		tft.setCursor(137,21);
		tft.print(midiChannel);
	}
	
	// print Velocity
	tft.setTextColor(ST7735_WHITE);
	if (velocitySens == 0) {
		tft.setCursor(137, 40);
		tft.print("OFF");
	}
	else {
		tft.setCursor(137, 40);
		tft.print(velocitySens);
	}
	
	// Unisono mode
	tft.setTextColor(ST7735_WHITE);
	tft.setCursor(136, 59);
	if (myUnisono == 0) {
		tft.print("8-1");
	}
	else if (myUnisono == 1) {
		tft.print("4-2");
	}
	
	// MidiSync
	tft.setTextColor(ST7735_WHITE);
	if (MidiSyncSwitch == true) {
		tft.setCursor(136, 78);
		tft.print("EXT");
	} else {
		tft.setCursor(136, 78);
		tft.print("INT");
	}
	
	// PRGchange
	if (PrgChangeSW == false) {
		tft.setCursor(136,97);
		tft.print("OFF");
	} else {
		tft.setCursor(138,97);
		tft.print("ON");
	}
	
	ParameterNr = ParameterNrMem[PageNr];
	drawParamterFrame(PageNr, ParameterNr);
	
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
	tft.setCursor(5,3);
	tft.setTextColor(ST7735_WHITE);
	tft.println("SAVE PROGRAM");
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
	tft.setCursor(8,57);
	tft.setTextSize(2);
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
	tft.setCursor(5,117);
	tft.println("BANK");
	tft.setCursor(50,117);
	tft.println("PATCH");
	tft.setCursor(95,117);
	tft.println("CHAR");
	tft.setCursor(135,117);
	tft.println("<->");
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
		case 6: drawFxDspPage(); break;
		case 7: drawLFO1Page();	break;
		case 8: drawLFO2Page();	break;
		case 9: drawSEQPage(); break;
		case 10: drawSystemPage(); break;
		case 96: drawInitPatchPage(); break;
		case 97: drawLoadSeqPattern(); break;
		case 98: drawSaveSeqPattern(); break;
		case 99: drawSavePage(); break;
	}
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

FLASHMEM void renderSettingsPage() {
  tft.fillScreen(ST7735_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(1);
  tft.setCursor(0, 53);
  tft.println(currentSettingsOption);
  if (currentSettingsPart == SETTINGS) renderUpDown(140, 42, ST7735_YELLOW);
  tft.drawFastHLine(10, 63, tft.width() - 20, ST7735_RED);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(5, 90);
  tft.println(currentSettingsValue);
  if (currentSettingsPart == SETTINGSVALUE) renderUpDown(140, 80, ST7735_WHITE);
}

FLASHMEM void showCurrentParameterPage( const char *param, float val, int pType) {
  currentParameter = param;
  currentValue = String(val);
  currentFloatValue = val;
  paramType = pType;
  startTimer();
}

FLASHMEM void showCurrentParameterPage(const char *param, String val, int pType) {
  if (state == SETTINGS || state == SETTINGSVALUE)state = PARAMETER;//Exit settings page if showing
  currentParameter = param;
  currentValue = val;
  paramType = pType;
  startTimer();
}

FLASHMEM void showCurrentParameterPage(const char *param, String val) {
  showCurrentParameterPage(param, val, PARAMETER);
}

FLASHMEM void showPatchPage(String number, String patchName) {
  currentPgmNum = number;
  currentPatchName = patchName;
}

FLASHMEM void showSettingsPage(char *  option, char * value, int settingsPart) {
  currentSettingsOption = option;
  currentSettingsValue = value;
  currentSettingsPart = settingsPart;
}

FLASHMEM void drawSubPageInfo () {
	
	static boolean blink;
	uint8_t xpos[11] = {0,32,32,45,45,45,0,70,70,26,0};
	
	if (PageNr >= 1 && PageNr <= 5 || PageNr == 9) {
		if (myPageShiftStatus[PageNr] == true) {
			if (blink == true) {
				tft.fillRoundRect(xpos[PageNr],2,19,9,2,ST7735_GREEN);
				tft.setCursor(xpos[PageNr]+1,3);
				tft.setTextColor(ST7735_BLACK);
				tft.print("SUB");
				blink = false;
				} else {
				tft.fillRoundRect(xpos[PageNr],2,19,9,2,ST7735_GRAY);
				blink = true;
			}
		}
	}
}

//*************************************************************************
// Display Thread
//*************************************************************************
void displayThread() {

	const uint8_t timeVal1 = 25;
	const uint8_t timeVal2 = 50;
	const uint8_t blinkiTime = 250;
	
		if (PageNr == 0) {					
			vuMeter = false;
			if ((millis() - timer1) > timeVal1){	// refresh Main Page and Scope all 25ms
				renderCurrentPatchPage();
				tft.updateScreen();	
				timer1 = millis();
				VoicLEDtime = 2;			// note off time for LED symbols	
			}
		}
		if (PageNr > 0) {
			if ((millis() - timer7) > timeVal2){	// refresh menu pages all 50ms
				vuMeter = true;
				MidiSymbol();
				renderPeak();
				drawVoiceLED();
				tft.updateScreen();
				timer7 = millis();
				VoicLEDtime = 2;			// note off time for LED symbols		
			}
			if ((millis() - timer4) > blinkiTime){	// Time for green SUB Page Marker
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
	tft.setRotation(1);
	tft.invertDisplay(false);
	tft.fillScreen(ST7735_BLACK);
	
	// init SD card -------------------------------------------------------
	boolean cardStatus = SD.begin(BUILTIN_SDCARD);
	if (cardStatus) {
		Serial.println(F("SD card is connected"));
	}
	else {
		tft.fillScreen(ST7735_RED);
		tft.setTextColor(ST7735_WHITE);
		tft.setTextSize(2,2);
		tft.setCursor(15,55);
		tft.print("No SD card!");
		while(1) { };
	}
	
	// Draw intro pics -------------------------------------------------
	Serial.println(F("Draw Power up pics"));
	tft.setTextColor(ST7735_LIGHTGRAY);
	const uint8_t xpos = 38;
	const uint8_t ypos = 38;
	const char* version ="LADDER VERSION";
	
	if (Filter == 1) {
		version = " ";
	}
	
	uint8_t waitTime = 1;
	for (uint8_t i = 0; i < 2; i++) {
		bmpDraw("PIC/PIC1.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC2.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC3.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC4.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC5.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC6.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC7.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC8.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC9.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC10.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC11.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC12.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC10.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC8.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC7.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC5.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);
		delay(waitTime);
		bmpDraw("PIC/PIC3.bmp",0,0);
		tft.setCursor(xpos,ypos);
		tft.print(version);		
	}
	
	delay(waitTime);
	bmpDraw("PIC/PIC12.bmp",0,0);
	tft.setCursor(xpos,ypos);
	tft.print(version);
	delay(2000);
	
	tft.useFrameBuffer(true); // activate Screen Buffer
}

