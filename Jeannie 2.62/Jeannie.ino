/*
8 Voices DIY Synthsizer "Jeannie"

Free Software
by Rolf Degen rolfdegen@hotmail.com
DIY Hardware Andre Laska (tubeohm.com)

Date: 20.08.2023
Build Version 2.62

MIT License
Copyright (c) Rolf Degen (2021 - 2023)
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Teensy 4.1 Development Board
ARM Cortex-M7 CPU 720MHz 1024K RAM  8MB Flash 4K EEPROM
Board: "Teensy4.1"
USB Type: "Serial + MIDI"
CPU Speed: "720MHz"
Optimize: "Fastest"

Includes free code by:
ElectroTechnique http://electrotechnique.cc/ - TSynth 4.1
Vince R. Pearson - Exponential envelopes & glide
Mark Tillotson - Special thanks for band-limiting the waveforms in the Audio Library
Additional libraries:
 , Adafruit_GFX (available in Arduino libraries manager)
*/

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <MIDI.h>
#include <Mcp320x.h>
#include <Entropy.h>
#include <usb_midi.h>
#include "Audio.h"		// using local version to override Teensyduino version
#include "MidiCC.h"
#include "effect_waveshaper.h"
#include "effect_envelope.h"
#include "AudioPatching.h"
#include "Constants.h"
#include "Parameters.h"
#include "PatchMgr.h"
#include "HWControls.h"
#include "EepromMgr.h"
#include "Detune.h"
#include "Velocity.h"
#include "global.h"
#include "Voice.h"
#include "Sequencer.h"
#include "SysexDump.h"


//*************************************************************************
// defines and inits
//*************************************************************************
// 12Bit ADC MCP3208 for pots
#define SPI_CS      10			// SPI slave select
#define ADC_VREF    256			// 12Bit Resolution
#define ADC_CLK     1200000		// SPI clock (max 1MHz 2.7V)
MCP3208 MCP_adc(ADC_VREF, SPI_CS);

#define PARAMETER 0			//The main page for displaying the current patch and control (parameter) changes
#define SETTINGS 8			//Settings page
#define SETTINGSVALUE 9		//Settings page
#define Pages 12			// Menu Pages
#define UnisonoLED	2
#define dumpRec 1
#define dumpSend 0
#include "ST7735Display.h"
#include "Settings.h"

//MIDI 5 Pin DIN
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

//int prevNote = 48;//This is for glide to use previous note to glide from
uint8_t UnisonoNote = 0;
float previousMillis = millis(); //For MIDI Clk Sync
uint32_t count = 0;//For MIDI Clk Sync
//uint32_t voiceToReturn = -1; //Initialise
//long earliestTime = millis(); //For voice allocation - initialise to now
boolean ParmSelectFlag = false;
IntervalTimer myMidiTimer;	// Midi and Sequencer
IntervalTimer myEncTimer;	// Enc query
boolean SEQLed = false;			// Sequencer Lamp
boolean SEQLedStatus = false;	// Sequencer Lamp
//boolean SEQStepStatus = false;	// Sequencer red frame
//boolean TempoLEDchange = false;
//boolean TempoLEDstate = false;
boolean myPrgChangeFlag = false;
boolean myBankSelectFlag = false;
uint8_t myPrgChangeChannel = 0;
uint8_t myPrgChangeProgram = 0;
uint8_t myBankSelectChannel = 0;
uint8_t myBankSelectBank = 0;
Bounce btnBouncer = Bounce(PAGE_SW, 50);
boolean cardStatus = false;
boolean firstPatchLoaded = false;



//*************************************************************************
// Setup
//*************************************************************************
FLASHMEM void setup() {

	// init Hardware ------------------------------------------------------
	Serial.begin(19200);
	setupDisplay();
	setupHardware();
	Entropy.Initialize();	// Random generator
	SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
	pinMode(SPI_CS, OUTPUT);
	digitalWrite(SPI_CS, HIGH);
	SPI.begin();
	SPI.beginTransaction(settings);
	initPWMFx();
	initHC595();
	setFxPrg(0);
	
	// init Audio buffer --------------------------------------------------
	AudioMemory(144);	// Sample Blocks
	
	// init Waveforms and more --------------------------------------------
	constant1Dc.amplitude(ONE);
	pwmLfoA.amplitude(ONE);
	pwmLfoA.begin(PWMWAVEFORMA);
	pwmLfoB.amplitude(ONE);
	pwmLfoB.begin(PWMWAVEFORMB);
	pwmLfoB.phase(10.0f);//Off set phase of second osc
	pwmLfoA.offset(0.0f);
	pwmLfoB.offset(0.0f);
	filterMixer1.gain(1,-1.0f);
	pitchLfo.amplitude(1.0f);
	init_ModLfo1 ();
	init_ModLfo2 ();
	init_ModLfo3 ();

	//This removes dc offset for Scope and Peak function
	hpFilter1.octaveControl(1.0f);
	hpFilter2.octaveControl(1.0f);
	hpFilter1.frequency(20.0f);
	hpFilter2.frequency(20.0f);
	hpFilter1.resonance(0.7f);
	hpFilter2.resonance(0.7f);
	
	// init 10Hz Atouch Filter
	AtouchFilter.octaveControl(1.0f);
	AtouchFilter.frequency(5.0f);
	AtouchFilter.resonance(0.7f);
	AtouchDc.amplitude(1.0f);
	// init 10Hz FilterSweep Filter
	FilterModCC.octaveControl(1.0f);
	FilterModCC.frequency(5.0f);
	FilterModCC.resonance(0.7f);
	DC_FilterModCC.amplitude(1.0f);
	DC_FilterModwheel.amplitude(1.0f);
	
	init_waveformMod();
	init_FilterVelocity();
	init_Keytracking();
	init_WaveshaperTable();
	init_WaveshaperAmp();
	init_EnvelopeReleasePhase();
	init_EnvelopeCurve();
	init_Filter();
	init_LFO1envModMixer();
	init_LFO_envelopes();
	initAtouchPitch();
	init_voices();
	
	// init Midi 5pol -----------------------------------------------------
	MIDI.begin();
	MIDI.setHandleNoteOn(myNoteOn);
	MIDI.setHandleNoteOff(myNoteOff);
	MIDI.setHandlePitchBend(myPitchBend);
	MIDI.setHandleAfterTouchChannel(myAftertouch);
	MIDI.setHandleControlChange(myControlChange1);
	MIDI.setHandleProgramChange(myProgramChange);
	MIDI.setHandleClock(myMIDIClock);
	MIDI.setHandleStart(myMIDIClockStart);
	MIDI.setHandleContinue(myMIDIClockStart);
	MIDI.setHandleStop(myMIDIClockStop);
	
	// init USB Midi ------------------------------------------------------
	usbMIDI.begin();
	usbMIDI.setHandleNoteOff(myNoteOff);
	usbMIDI.setHandleNoteOn(myNoteOn);
	usbMIDI.setHandlePitchChange(myPitchBend);
	usbMIDI.setHandleAfterTouchChannel(myAftertouch);
	usbMIDI.setHandleControlChange(myControlChange1);
	usbMIDI.setHandleProgramChange(myProgramChange);
	usbMIDI.setHandleClock(myMIDIClock);
	usbMIDI.setHandleStart(myMIDIClockStart);
	usbMIDI.setHandleContinue(myMIDIClockStart);
	usbMIDI.setHandleStop(myMIDIClockStop);
	usbMIDI.setHandleSysEx(getSysexDump);
		
	//Read MIDI Channel from EEPROM ---------------------------------------
	midiChannel = getMIDIChannel();
	//timer_intMidiClk = micros();					// interval Intern Midi Clock ms
	myMidiTimer.begin(MidiClockTimer, 256);			// Midi Timer interrupt
	myMidiTimer.priority(254);						// Timer interrupt priority
	getMidiCkl();
	MIDIClkSignal = true;
	filterLFOMidiClkSync = true;
	
	// init EEPROM --------------------------------------------------------
	if (myEEPROMinit != getEEPROMinit()) {
		Serial.println("EEPROM data: fault!");
		storeEEPROMinit();
		delay(50);
		Serial.println("Write new EEPROM data..");
		// write EEPROM datas
		storeMidiChannel(0); // Midi channel ALL
		delay(50);
		storePatchBankNo(0);
		delay(50);
		storeSoundPatchNo(1);
		delay(50);
		storePRGchange(0);
		delay(50);
		storePickup(1);
		delay(200);
		// Soft Reset Teensy 4.1
		SCB_AIRCR = 0x05FA0004;
	}
	else {
		Serial.println("EEPROM data: ok!");
	}
	
	// load Global datas from EEPROM --------------------------------------
	getMIDIChannel();
	getPRGchange();
	//getPickup();
	
	// load Sound Programm ------------------------------------------------
	int mux5Read = MCP_adc.read(MCP3208::Channel::SINGLE_7);
	uint8_t value = mux5Read >> 5;
	
	// If the "Mute" button is pressed when switching on, then the system boots with patch A000
	if (value < (S4 + hysteresis) && value > (S4 - hysteresis)) {
		
		} else {
		initPatternData();
		Init_Patch();
	}
	
	Keylock = false;
	mute_before_load_patch();
	
	// read Temp.
	CPUdegree = tempmonGetTemp();
	
	// init potentiometer -------------------------------------------------
	pot_init();
}


//*************************************************************************
// inits
//*************************************************************************

// init LFO1 shape
FLASHMEM void init_ModLfo1 (void) {
	pitchLfo.amplitude(1.0f);
	pitchLfo.offset(NULL);
	pitchLfo.arbitraryWaveform(LFOwaveform, AWFREQ); // LFO3 shape No: 1
	pitchLfo.begin(WAVEFORM_ARBITRARY1);
	pitchLfo.frequency(1.0f);
	updateOscLFOAmt();
}

// init LFO2 shape
FLASHMEM void init_ModLfo2 (void) {
	filterLfo.amplitude(1.0f);
	filterLfo.offset(NULL);
	filterLfo.arbitraryWaveform(LFOwaveform, AWFREQ); // LFO3 shape No: 1
	filterLfo.begin(WAVEFORM_ARBITRARY2);
	filterLfo.frequency(1.0f);
	updateFilterLfoAmt();
}

// init LFO3 shape
FLASHMEM void init_ModLfo3 (void) {
	ModLfo3.amplitude(1.0f);
	ModLfo3.offset(NULL);
	ModLfo3.arbitraryWaveform(LFOwaveform, AWFREQ); // LFO3 shape No: 1
	ModLfo3.begin(WAVEFORM_ARBITRARY3);
	ModLfo3.frequency(1.0f);
	updateLFO3amt();
	LFO3ModMixer1Dc.amplitude(1.0f);
	
}

FLASHMEM void init_LFO_envelopes(void) {
	updateLFO1delay();
	updateLFO1fade();
	updateLFO1decay();
	updateLFO1sustain();
	updateLFO1release();
	updateLFO2delay();
	updateLFO2fade();
	updateLFO2decay();
	updateLFO2sustain();
	updateLFO2release();
	updateLFO3delay();
	updateLFO3fade();
	updateLFO3decay();
	updateLFO3sustain();
	updateLFO3release();
}


FLASHMEM void updateLFO1waveform () {
	if (oscLFOWaveform != 5) {
		pitchLfo.begin(WAVEFORM_ARBITRARY1);
	}
	else {
		pitchLfo.begin(WAVEFORM_SAMPLE_HOLD1);
	}
}

FLASHMEM void updateLFO2waveform () {
	if (filterLfoWaveform != 5) {
		filterLfo.begin(WAVEFORM_ARBITRARY2);
	}
	else {
		filterLfo.begin(WAVEFORM_SAMPLE_HOLD2);
	}
}

FLASHMEM void updateLFO3waveform () {
	if (Lfo3Waveform != 5) {
		ModLfo3.begin(WAVEFORM_ARBITRARY3);
	}
	else {
		
		ModLfo3.begin(WAVEFORM_SAMPLE_HOLD3);
	}
}



FLASHMEM void updateLFO3amt() {
	float value = Lfo3amt + AtouchLFO3amt;
	//float value = Lfo3amt;
	if (value >= 1.0) {
		value = 1.0f;
	}
	value = value / 2.0f;
	
	if (LFO3fadeTime != 0 || LFO3releaseTime != 0) {
		LFO3ModMixer1.gain(0, value);	// LFO3envelope
		LFO3ModMixer1.gain(1, (1.0f - value));
		LFO3ModMixer1.gain(2, 0);		// LFO3 > VCA off
	}
	else {
		LFO3ModMixer1.gain(0, 0);		// LFO3 without envelope
		LFO3ModMixer1.gain(1, (1.0f - value));
		LFO3ModMixer1.gain(2, value);	// LFO3 > VCA on
	}
}

FLASHMEM void init_voices() {
	voiceMixerM1.gain(0, VOICEMIXERLEVEL);
	voiceMixerM1.gain(1, VOICEMIXERLEVEL);
	voiceMixerM1.gain(2, VOICEMIXERLEVEL);
	voiceMixerM1.gain(3, VOICEMIXERLEVEL);
	voiceMixerM2.gain(0, VOICEMIXERLEVEL);
	voiceMixerM2.gain(1, VOICEMIXERLEVEL);
	voiceMixerM2.gain(2, VOICEMIXERLEVEL);
	voiceMixerM2.gain(3, VOICEMIXERLEVEL);
	// Scope/Peak Mixer
	ScopePeakMixer.gain(0, VOICEMIXERLEVEL);
	ScopePeakMixer.gain(1, VOICEMIXERLEVEL);
}

FLASHMEM void init_waveformMod() {
	
	float lev = 1.0f;
	float frq = 440.0f;
	short wav = 3;
	float oct = 2.0f;
	loadArbWaveformA(PARABOLIC_WAVE);
	loadArbWaveformB(PARABOLIC_WAVE);
	
	for (uint8_t i = 0; i < 8; i++) {
		waveformModa[i].begin(lev, frq, wav);
		waveformModb[i].begin(lev, frq, wav);
		waveformModa[i].frequencyModulation(oct);
		waveformModb[i].frequencyModulation(oct);
	}
}

FLASHMEM void init_Filter() {
	float filterFreq = 18000.0f;
	filter1.frequency(filterFreq);
	filter2.frequency(filterFreq);
	filter3.frequency(filterFreq);
	filter4.frequency(filterFreq);
	filter5.frequency(filterFreq);
	filter6.frequency(filterFreq);
	filter7.frequency(filterFreq);
	filter8.frequency(filterFreq);
	filterOctave = 6.9999f;
}

FLASHMEM void init_Keytracking() {
	const float val = 1.0f;
	keytracking1.amplitude(val);
	keytracking2.amplitude(val);
	keytracking3.amplitude(val);
	keytracking4.amplitude(val);
	keytracking5.amplitude(val);
	keytracking6.amplitude(val);
	keytracking7.amplitude(val);
	keytracking8.amplitude(val);
}

FLASHMEM void init_FilterVelocity() {
	const float val = 1.0f;
	FilterVelo1.amplitude(val);
	FilterVelo2.amplitude(val);
	FilterVelo3.amplitude(val);
	FilterVelo4.amplitude(val);
	FilterVelo5.amplitude(val);
	FilterVelo6.amplitude(val);
	FilterVelo7.amplitude(val);
	FilterVelo8.amplitude(val);
}


FLASHMEM void init_WaveshaperTable() {
	const int val = 3;
	for (uint8_t i = 0; i < 8; i++) {
		waveshaper[i].shape(WAVESHAPER_TABLE1, val);
	}
}

FLASHMEM void init_WaveshaperAmp() {
	
	const float val = 1.0f;
	for (uint8_t i = 0; i < 8; i++) {
		WaveshaperAmp[i].gain(val);
	}
}

FLASHMEM void init_LFO1envModMixer () {
	const float val = 0.125f;
	LFO1envModMixer1.gain(0, val);
	LFO1envModMixer1.gain(1, val);
	LFO1envModMixer1.gain(2, val);
	LFO1envModMixer1.gain(3, val);
	LFO1envModMixer2.gain(0, val);
	LFO1envModMixer2.gain(1, val);
	LFO1envModMixer2.gain(2, val);
	LFO1envModMixer2.gain(3, val);
}

FLASHMEM void init_EnvelopeReleasePhase() {
	const float time = 1.0f;
	filterEnvelope[0].releaseNoteOn(time);
	filterEnvelope[1].releaseNoteOn(time);
	filterEnvelope[2].releaseNoteOn(time);
	filterEnvelope[3].releaseNoteOn(time);
	filterEnvelope[4].releaseNoteOn(time);
	filterEnvelope[5].releaseNoteOn(time);
	filterEnvelope[6].releaseNoteOn(time);
	filterEnvelope[7].releaseNoteOn(time);
	ampEnvelope[0].releaseNoteOn(time);
	ampEnvelope[1].releaseNoteOn(time);
	ampEnvelope[2].releaseNoteOn(time);
	ampEnvelope[3].releaseNoteOn(time);
	ampEnvelope[4].releaseNoteOn(time);
	ampEnvelope[5].releaseNoteOn(time);
	ampEnvelope[6].releaseNoteOn(time);
	ampEnvelope[7].releaseNoteOn(time);
	LFO1Envelope[0].releaseNoteOn(time);
	LFO1Envelope[1].releaseNoteOn(time);
	LFO1Envelope[2].releaseNoteOn(time);
	LFO1Envelope[3].releaseNoteOn(time);
	LFO1Envelope[4].releaseNoteOn(time);
	LFO1Envelope[5].releaseNoteOn(time);
	LFO1Envelope[6].releaseNoteOn(time);
	LFO1Envelope[7].releaseNoteOn(time);
	LFO2Envelope[0].releaseNoteOn(time);
	LFO2Envelope[1].releaseNoteOn(time);
	LFO2Envelope[2].releaseNoteOn(time);
	LFO2Envelope[3].releaseNoteOn(time);
	LFO2Envelope[4].releaseNoteOn(time);
	LFO2Envelope[5].releaseNoteOn(time);
	LFO2Envelope[6].releaseNoteOn(time);
	LFO2Envelope[7].releaseNoteOn(time);
	LFO3EnvelopeAmp.releaseNoteOn(time);
}

FLASHMEM void set_Filter_Envelope_releaseNoteOn() {
	const float time = 1.0f;
	filterEnvelope[0].releaseNoteOn(time);
	filterEnvelope[1].releaseNoteOn(time);
	filterEnvelope[2].releaseNoteOn(time);
	filterEnvelope[3].releaseNoteOn(time);
	filterEnvelope[4].releaseNoteOn(time);
	filterEnvelope[5].releaseNoteOn(time);
	filterEnvelope[6].releaseNoteOn(time);
	filterEnvelope[7].releaseNoteOn(time);
}

FLASHMEM void set_Amp_Envelope_releaseNoteOn() {
	const float time = 1.0f;
	ampEnvelope[0].releaseNoteOn(time);
	ampEnvelope[1].releaseNoteOn(time);
	ampEnvelope[2].releaseNoteOn(time);
	ampEnvelope[3].releaseNoteOn(time);
	ampEnvelope[4].releaseNoteOn(time);
	ampEnvelope[5].releaseNoteOn(time);
	ampEnvelope[6].releaseNoteOn(time);
	ampEnvelope[7].releaseNoteOn(time);
}

FLASHMEM void init_EnvelopeCurve() {
	const int8_t envelopeType = 0;	// 0 linear, -8 fast exponential, -8 slow exponential
	ampEnvelope[0].setEnvType(envelopeType);
	ampEnvelope[1].setEnvType(envelopeType);
	ampEnvelope[2].setEnvType(envelopeType);
	ampEnvelope[3].setEnvType(envelopeType);
	ampEnvelope[4].setEnvType(envelopeType);
	ampEnvelope[5].setEnvType(envelopeType);
	ampEnvelope[6].setEnvType(envelopeType);
	ampEnvelope[7].setEnvType(envelopeType);
	filterEnvelope[0].setEnvType(envelopeType);
	filterEnvelope[1].setEnvType(envelopeType);
	filterEnvelope[2].setEnvType(envelopeType);
	filterEnvelope[3].setEnvType(envelopeType);
	filterEnvelope[4].setEnvType(envelopeType);
	filterEnvelope[5].setEnvType(envelopeType);
	filterEnvelope[6].setEnvType(envelopeType);
	filterEnvelope[7].setEnvType(envelopeType);
	LFO1Envelope[0].setEnvType(envelopeType);
	LFO1Envelope[1].setEnvType(envelopeType);
	LFO1Envelope[2].setEnvType(envelopeType);
	LFO1Envelope[3].setEnvType(envelopeType);
	LFO1Envelope[4].setEnvType(envelopeType);
	LFO1Envelope[5].setEnvType(envelopeType);
	LFO1Envelope[6].setEnvType(envelopeType);
	LFO1Envelope[7].setEnvType(envelopeType);
	LFO2Envelope[0].setEnvType(envelopeType);
	LFO2Envelope[1].setEnvType(envelopeType);
	LFO2Envelope[2].setEnvType(envelopeType);
	LFO2Envelope[3].setEnvType(envelopeType);
	LFO2Envelope[4].setEnvType(envelopeType);
	LFO2Envelope[5].setEnvType(envelopeType);
	LFO2Envelope[6].setEnvType(envelopeType);
	LFO2Envelope[7].setEnvType(envelopeType);
	LFO3EnvelopeAmp.setEnvType(envelopeType);
}

FLASHMEM void updateLFOsyn() {
	/*
	if (oscLfoRetrig == 2 && oscLFOWaveform == WAVEFORM_SAWTOOTH ) {
	lfo1ph = 180;
	}
	if (filterLfoRetrig == 2 && filterLfoWaveform == WAVEFORM_SAWTOOTH2 ) {
	lfo2ph = 180;
	}
	*/
}



FLASHMEM void updateLFO1delay () {
	float t_delay = 0;
	
	LFO1Envelope[0].delay(t_delay);
	LFO1Envelope[1].delay(t_delay);
	LFO1Envelope[2].delay(t_delay);
	LFO1Envelope[3].delay(t_delay);
	LFO1Envelope[4].delay(t_delay);
	LFO1Envelope[5].delay(t_delay);
	LFO1Envelope[6].delay(t_delay);
	LFO1Envelope[7].delay(t_delay);
}

FLASHMEM void updateLFO2delay () {
	float t_delay = 0;
	LFO2Envelope[0].delay(t_delay);
	LFO2Envelope[1].delay(t_delay);
	LFO2Envelope[2].delay(t_delay);
	LFO2Envelope[3].delay(t_delay);
	LFO2Envelope[4].delay(t_delay);
	LFO2Envelope[5].delay(t_delay);
	LFO2Envelope[6].delay(t_delay);
	LFO2Envelope[7].delay(t_delay);
	
}

FLASHMEM void updateLFO3delay () {
	float t_delay = 0;
	LFO3EnvelopeAmp.delay(t_delay);
}

FLASHMEM void updateLFO1fade () {
	float t_fade = LFO1fadeTime;
	
	LFO1Envelope[0].attack(t_fade);
	LFO1Envelope[1].attack(t_fade);
	LFO1Envelope[2].attack(t_fade);
	LFO1Envelope[3].attack(t_fade);
	LFO1Envelope[4].attack(t_fade);
	LFO1Envelope[5].attack(t_fade);
	LFO1Envelope[6].attack(t_fade);
	LFO1Envelope[7].attack(t_fade);
}

FLASHMEM void updateLFO2fade () {
	float t_fade = LFO2fadeTime;
	LFO2Envelope[0].attack(t_fade);
	LFO2Envelope[1].attack(t_fade);
	LFO2Envelope[2].attack(t_fade);
	LFO2Envelope[3].attack(t_fade);
	LFO2Envelope[4].attack(t_fade);
	LFO2Envelope[5].attack(t_fade);
	LFO2Envelope[6].attack(t_fade);
	LFO2Envelope[7].attack(t_fade);
	
}

FLASHMEM void updateLFO3fade () {
	float value = 0.0f;
	LFO3EnvelopeAmp.decay(value);
	value = LFO3fadeTime;
	LFO3EnvelopeAmp.attack(value);
}

FLASHMEM void updateLFO1decay () {
	float t_decay = 0.0f;
	LFO1Envelope[0].decay(t_decay);
	LFO1Envelope[1].decay(t_decay);
	LFO1Envelope[2].decay(t_decay);
	LFO1Envelope[3].decay(t_decay);
	LFO1Envelope[4].decay(t_decay);
	LFO1Envelope[5].decay(t_decay);
	LFO1Envelope[6].decay(t_decay);
	LFO1Envelope[7].decay(t_decay);
}

FLASHMEM void updateLFO2decay () {
	float t_decay = 0.0f;
	LFO2Envelope[0].decay(t_decay);
	LFO2Envelope[1].decay(t_decay);
	LFO2Envelope[2].decay(t_decay);
	LFO2Envelope[3].decay(t_decay);
	LFO2Envelope[4].decay(t_decay);
	LFO2Envelope[5].decay(t_decay);
	LFO2Envelope[6].decay(t_decay);
	LFO2Envelope[7].decay(t_decay);	
}

FLASHMEM void updateLFO3decay () {
	float t_decay = 0.0f;
	LFO3EnvelopeAmp.decay(t_decay);
}

FLASHMEM void updateLFO1sustain () {
	float val = 1.0f;
	LFO1Envelope[0].sustain(val);
	LFO1Envelope[1].sustain(val);
	LFO1Envelope[2].sustain(val);
	LFO1Envelope[3].sustain(val);
	LFO1Envelope[4].sustain(val);
	LFO1Envelope[5].sustain(val);
	LFO1Envelope[6].sustain(val);
	LFO1Envelope[7].sustain(val);
}

FLASHMEM void updateLFO2sustain () {
	float val = 1.0f;
	LFO2Envelope[0].sustain(val);
	LFO2Envelope[1].sustain(val);
	LFO2Envelope[2].sustain(val);
	LFO2Envelope[3].sustain(val);
	LFO2Envelope[4].sustain(val);
	LFO2Envelope[5].sustain(val);
	LFO2Envelope[6].sustain(val);
	LFO2Envelope[7].sustain(val);
	
}

FLASHMEM void updateLFO3sustain () {
	LFO3EnvelopeAmp.sustain(1.0f);
}

FLASHMEM void updateLFO1release () {
	float t_release = LFO1releaseTime;
	
	if (LFO1releaseTime > ampRelease) {
		t_release = ampRelease;
	}
	
	LFO1Envelope[0].release(t_release);
	LFO1Envelope[1].release(t_release);
	LFO1Envelope[2].release(t_release);
	LFO1Envelope[3].release(t_release);
	LFO1Envelope[4].release(t_release);
	LFO1Envelope[5].release(t_release);
	LFO1Envelope[6].release(t_release);
	LFO1Envelope[7].release(t_release);
	
}

FLASHMEM void updateLFO2release () {
	float t_release = LFO2releaseTime;
	
	if (LFO2releaseTime > ampRelease) {
		t_release = ampRelease;
	}
	LFO2Envelope[0].release(t_release);
	LFO2Envelope[1].release(t_release);
	LFO2Envelope[2].release(t_release);
	LFO2Envelope[3].release(t_release);
	LFO2Envelope[4].release(t_release);
	LFO2Envelope[5].release(t_release);
	LFO2Envelope[6].release(t_release);
	LFO2Envelope[7].release(t_release);
	
}

FLASHMEM void updateLFO3release () {
	
	float t_release;
	
	if (LFO2releaseTime > ampRelease) {
		t_release = ampRelease;
	}
	else {
		t_release = LFO3releaseTime;
	}
	
	LFO3EnvelopeAmp.release(t_release);
}

FLASHMEM void ampEnvelopes_off() {
	float val = 25.0f;
	ampEnvelope[0].release(val);
	ampEnvelope[1].release(val);
	ampEnvelope[2].release(val);
	ampEnvelope[3].release(val);
	ampEnvelope[4].release(val);
	ampEnvelope[5].release(val);
	ampEnvelope[6].release(val);
	ampEnvelope[7].release(val);
}



FLASHMEM void updateFilterFM() {
	filterModMixer1b.gain(1, filterFM);
	filterModMixer2b.gain(1, filterFM);
	filterModMixer3b.gain(1, filterFM);
	filterModMixer4b.gain(1, filterFM);
	filterModMixer5b.gain(1, filterFM);
	filterModMixer6b.gain(1, filterFM);
	filterModMixer7b.gain(1, filterFM);
	filterModMixer8b.gain(1, filterFM);
}

FLASHMEM void updateFilterFM2() {
	filterModMixer1b.gain(2, filterFM2);
	filterModMixer2b.gain(2, filterFM2);
	filterModMixer3b.gain(2, filterFM2);
	filterModMixer4b.gain(2, filterFM2);
	filterModMixer5b.gain(2, filterFM2);
	filterModMixer6b.gain(2, filterFM2);
	filterModMixer7b.gain(2, filterFM2);
	filterModMixer8b.gain(2, filterFM2);
}


//*************************************************************************
// init Potentiometer
//*************************************************************************
FLASHMEM void pot_init () {
	
	int mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
	mux1ValuesPrev = mux1Read;
	int mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
	mux2ValuesPrev = mux2Read;
	int mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
	mux3ValuesPrev = mux3Read;
	int mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
	mux4ValuesPrev = mux4Read;
	
	// read Pot1 (change Patch Bank)
	mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
	if (mux1Read > (mux1ValuesPrev + (QUANTISE_FACTOR * 4)) || mux1Read < (mux1ValuesPrev - (QUANTISE_FACTOR *4))) {
		mux1ValuesPrev = mux1Read;
	}
	// read Pot2 (non)
	mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
	if (mux2Read > (mux2ValuesPrev + (QUANTISE_FACTOR * 4)) || mux2Read < (mux2ValuesPrev - (QUANTISE_FACTOR *4))) {
		mux2ValuesPrev = mux2Read;
	}
	// read Pot3 (non)
	mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
	if (mux3Read > (mux3ValuesPrev + (QUANTISE_FACTOR * 4)) || mux3Read < (mux3ValuesPrev - (QUANTISE_FACTOR *4))) {
		mux3ValuesPrev = mux3Read;
	}
	// read Pot4 (Cutoff value)
	mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
	if (mux4Read > (mux4ValuesPrev + QUANTISE_FACTOR) || mux4Read < (mux4ValuesPrev - QUANTISE_FACTOR)) {
		mux4ValuesPrev = mux4Read;
	}
}

//*************************************************************************
// get LFO Waveform
//*************************************************************************
FLASHMEM int getLFOWaveform(int value) {
	/*
	if (value >= 0 && value < 8) {
	return WAVEFORM_SINE;
	} else if (value >= 8 && value < 30) {
	return WAVEFORM_TRIANGLE;
	} else if (value >= 30 && value < 63) {
	return WAVEFORM_SAWTOOTH_REVERSE;
	} else if (value >= 63 && value < 92) {
	return WAVEFORM_ARBITRARY;
	} else if (value >= 92 && value < 111) {
	return WAVEFORM_SQUARE;
	} else {
	return WAVEFORM_SAMPLE_HOLD;
	}*/
}

//*************************************************************************
// convert LFO1 Waveform
//*************************************************************************
FLASHMEM int convertLFOWaveform(int value) {
	
	return 20;
	
	/*
	switch (value)
	{
	case WAVEFORM_SINE: return 0;break;
	case WAVEFORM_TRIANGLE: return 1;break;
	case WAVEFORM_SAWTOOTH: return 2;break;
	case WAVEFORM_ARBITRARY: return 3;break;
	case WAVEFORM_SQUARE: return 4;break;
	case WAVEFORM_SAMPLE_HOLD: return 5;break;
	}
	*/
}

//*************************************************************************
// convert LFO2 Waveform
//*************************************************************************
FLASHMEM int convertLFO2Waveform(int value) {
	
	return 22;
	
	/*
	switch (value)
	{
	case WAVEFORM_SINE2: return 0;break;
	case WAVEFORM_TRIANGLE2: return 1;break;
	case WAVEFORM_SAWTOOTH2: return 2;break;
	case WAVEFORM_ARBITRARY2: return 3;break;
	case WAVEFORM_SQUARE2: return 4;break;
	case WAVEFORM_SAMPLE_HOLD2: return 5;break;
	}
	*/
	
}

//*************************************************************************
// convert LFO3 Waveform
//*************************************************************************
FLASHMEM int convertLFO3Waveform(int value) {
	
	return 24;
	
	/*
	switch (value)
	{
	case WAVEFORM_SINE3: return 0;break;
	case WAVEFORM_TRIANGLE3: return 1;break;
	case WAVEFORM_SAWTOOTH3: return 2;break;
	case WAVEFORM_ARBITRARY3: return 3;break;
	case WAVEFORM_SQUARE3: return 4;break;
	case WAVEFORM_SAMPLE_HOLD3: return 5;break;
	}
	*/
}

//*************************************************************************
// convert pitchEnv
//*************************************************************************
FLASHMEM int convert_pitchEnv(float pitch_Env){
	
	float Value1 = (pitch_Env / OSCMODMIXERMAX);
	for (int i = 0; i < 128; i++) {
		float Value2 = LINEARCENTREZERO[i];
		if (Value1 <= Value2){
			return i;
		}
	}
	return 0;
}

//*************************************************************************
// load arbitrary Waveforms
//*************************************************************************
FLASHMEM void loadArbWaveformA(const int16_t * wavedata) {
	
	for (uint8_t i = 0; i < 8; i++) {
		waveformModa[i].arbitraryWaveform(wavedata, AWFREQ);
	}
}

FLASHMEM void loadArbWaveformB(const int16_t * wavedata) {
	
	for (uint8_t i = 0; i < 8; i++) {
		waveformModb[i].arbitraryWaveform(wavedata, AWFREQ);
	}
}

//*************************************************************************
// get LFO TempoRate
//*************************************************************************
FLASHMEM float getLFOTempoRate(int value) {
	lfoTempoValue = LFOTEMPO[value];
	return lfoSyncFreq * LFOTEMPO[value];
}

//*************************************************************************
// get Pitch
//*************************************************************************
FLASHMEM int getPitch(int value) {
	return PITCH[value];
}

//*************************************************************************
// get Pitch
//*************************************************************************
FLASHMEM int getTranspose(int value) {
	return TRANSPOSE[value];
}

//*************************************************************************
// get Pitch
//*************************************************************************
FLASHMEM int getMasterTune(int value) {
	return MASTERTUNE[value];
}

//*************************************************************************
// set PwmMixerA_LFO
//*************************************************************************
FLASHMEM void setPwmMixerALFO(float value) {
	pwMixer1a.gain(0, value);
	pwMixer2a.gain(0, value);
	pwMixer3a.gain(0, value);
	pwMixer4a.gain(0, value);
	pwMixer5a.gain(0, value);
	pwMixer6a.gain(0, value);
	pwMixer7a.gain(0, value);
	pwMixer8a.gain(0, value);
}

//*************************************************************************
// set PwmMixerB_LFO
//*************************************************************************
FLASHMEM void setPwmMixerBLFO(float value) {
	pwMixer1b.gain(0, value);
	pwMixer2b.gain(0, value);
	pwMixer3b.gain(0, value);
	pwMixer4b.gain(0, value);
	pwMixer5b.gain(0, value);
	pwMixer6b.gain(0, value);
	pwMixer7b.gain(0, value);
	pwMixer8b.gain(0, value);
}

FLASHMEM void setPwmMixerAPW(float value) {
	pwMixer1a.gain(1, value);
	pwMixer2a.gain(1, value);
	pwMixer3a.gain(1, value);
	pwMixer4a.gain(1, value);
	pwMixer5a.gain(1, value);
	pwMixer6a.gain(1, value);
	pwMixer7a.gain(1, value);
	pwMixer8a.gain(1, value);
}

FLASHMEM void setPwmMixerBPW(float value) {
	pwMixer1b.gain(1, value);
	pwMixer2b.gain(1, value);
	pwMixer3b.gain(1, value);
	pwMixer4b.gain(1, value);
	pwMixer5b.gain(1, value);
	pwMixer6b.gain(1, value);
	pwMixer7b.gain(1, value);
	pwMixer8b.gain(1, value);
}

FLASHMEM void setPwmMixerAFEnv(float value) {
	pwMixer1a.gain(2, value);
	pwMixer2a.gain(2, value);
	pwMixer3a.gain(2, value);
	pwMixer4a.gain(2, value);
	pwMixer5a.gain(2, value);
	pwMixer6a.gain(2, value);
	pwMixer7a.gain(2, value);
	pwMixer8a.gain(2, value);
}

FLASHMEM void setPwmMixerBFEnv(float value) {
	pwMixer1b.gain(2, value);
	pwMixer2b.gain(2, value);
	pwMixer3b.gain(2, value);
	pwMixer4b.gain(2, value);
	pwMixer5b.gain(2, value);
	pwMixer6b.gain(2, value);
	pwMixer7b.gain(2, value);
	pwMixer8b.gain(2, value);
}

FLASHMEM void updateUnison() {
		allNotesOff();					// Unison off
		updateDetune();
	for (uint8_t i = 0; i < 8; i++) {
		voices[i].voiceOn = 0;
		voices[i].timeOn = 0;
	}
}

//*************************************************************************
// update Glide
//*************************************************************************
FLASHMEM void updateGlide() {
}

//*************************************************************************
// update Waveforms
//*************************************************************************
FLASHMEM void updateWaveformA(void) {
	
	int newWaveform;
	
	if (Osc1WaveBank == 0 && oscWaveformA <= 12 || oscWaveformA == 0){
		newWaveform = waveform[oscWaveformA];
		} else {
		if (Osc1WaveBank == 0) {	// Bank A (waveform 1-12 calculated waveforms. Rest arbitrary waveforms)
			loadArbWaveformA(ArbBank + (256 * (oscWaveformA - 12)));
			} else { // Bank B-O (arbitrary waveforms)
			loadArbWaveformA(ArbBank + ((16384 * Osc1WaveBank) + (oscWaveformA * 256)));
		}
		newWaveform = WAVEFORM_ARBITRARY;
	}
	for (uint8_t i = 0; i < 8; i++) {
		waveformModa[i].begin(newWaveform);
	}
	
	currentWaveformA = oscWaveformA;
	currentOsc1WaveBank = Osc1WaveBank;
}

FLASHMEM void updateWaveformB() {
	
	int newWaveform;
	
	if (Osc2WaveBank == 0 && oscWaveformB <= 12 || oscWaveformB == 0){
		newWaveform = waveform_B[oscWaveformB];
		} else {
		if (Osc2WaveBank == 0) {	// Bank A (waveform 1-12 calculated waveforms. Rest arbitrary waveforms)
			loadArbWaveformB(ArbBank + (256 * (oscWaveformB - 12)));
			} else { // Bank B-O (arbitrary waveforms)
			loadArbWaveformB(ArbBank + ((16384 * Osc2WaveBank) + (oscWaveformB * 256)));
		}
		newWaveform = WAVEFORM_ARBITRARY;
	}
	
	for (uint8_t i = 0; i < 8; i++) {
		waveformModb[i].begin(newWaveform);
	}
	
	currentWaveformB = oscWaveformB;
	currentOsc2WaveBank = Osc2WaveBank;
	
}

//*************************************************************************
// set Waveshaper Table
//*************************************************************************
FLASHMEM void setWaveShaperTable (uint8_t WShaperNo){
	
	int length;
	const float *WS_array_addr;
	
	switch(WShaperNo) {
		case 0: WS_array_addr = WAVESHAPER_TABLE1; break;
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
	
	// 1st WaveShaper table has 3 entries. Others have 257
	if (WShaperNo == 0) {
		length = 3;
		} else {
		length = 257;
	}
	
	for (uint8_t i = 0; i < 8; i++) {
		waveshaper[i].shape(WS_array_addr,length);
	}
}

//*************************************************************************
// set Waveshaper drive value
//*************************************************************************
FLASHMEM void setWaveShaperDrive (float value){
	
	for (uint8_t i = 0; i < 8; i++) {
		WaveshaperAmp[i].gain(value);
	}
}


//*************************************************************************
// update Pitch + Detune
//*************************************************************************
FLASHMEM void updatePitchA() {
	updatesAllVoices();
}

FLASHMEM void updatePitchB() {
	updatesAllVoices();
}

FLASHMEM void updateDetune() {
	updatesAllVoices();
}

FLASHMEM void updateFilterEnvelopeType() {
	// set Envelupe curve
	// 0 linear, -8 fast exponential, -8 slow exponential
	//envelopeType1 = -128;	// old liniear curve
	
	filterEnvelope[0].setEnvType(envelopeType1);
	filterEnvelope[1].setEnvType(envelopeType1);
	filterEnvelope[2].setEnvType(envelopeType1);
	filterEnvelope[3].setEnvType(envelopeType1);
	filterEnvelope[4].setEnvType(envelopeType1);
	filterEnvelope[5].setEnvType(envelopeType1);
	filterEnvelope[6].setEnvType(envelopeType1);
	filterEnvelope[7].setEnvType(envelopeType1);
}


FLASHMEM void updateAmpEnvelopeType() {
	// set Envelupe curve
	// 0 linear, -8 fast exponential, -8 slow exponential
	//envelopeType2 = -128;	// old linear curve
	
	ampEnvelope[0].setEnvType(envelopeType2);
	ampEnvelope[1].setEnvType(envelopeType2);
	ampEnvelope[2].setEnvType(envelopeType2);
	ampEnvelope[3].setEnvType(envelopeType2);
	ampEnvelope[4].setEnvType(envelopeType2);
	ampEnvelope[5].setEnvType(envelopeType2);
	ampEnvelope[6].setEnvType(envelopeType2);
	ampEnvelope[7].setEnvType(envelopeType2);
}

FLASHMEM void updateLFO1EnvelopeType() {
	// set Envelupe curve
	// 0 linear, -8 fast exponential, -8 slow exponential
	//envelopeType2 = -128;	// old linear curve
	
	for (uint8_t i = 0; i < 8; i++) {
		//LFO1Envelope[i].setEnvType(LFO1envCurve);
	}	
}

FLASHMEM void updateLFO2EnvelopeType() {
	// set Envelupe curve
	// 0 linear, -8 fast exponential, -8 slow exponential
	//envelopeType2 = -128;	// old linear curve
	
	LFO2Envelope[0].setEnvType(LFO2envCurve);
	LFO2Envelope[1].setEnvType(LFO2envCurve);
	LFO2Envelope[2].setEnvType(LFO2envCurve);
	LFO2Envelope[3].setEnvType(LFO2envCurve);
	LFO2Envelope[4].setEnvType(LFO2envCurve);
	LFO2Envelope[5].setEnvType(LFO2envCurve);
	LFO2Envelope[6].setEnvType(LFO2envCurve);
	LFO2Envelope[7].setEnvType(LFO2envCurve);
	
}

FLASHMEM void updateLFO3EnvelopeType() {
	// set Envelupe curve
	// 0 linear, -8 fast exponential, -8 slow exponential
	//envelopeType2 = -128;	// old linear curve
	LFO2Envelope[0].setEnvType(LFO2envCurve);
	LFO2Envelope[1].setEnvType(LFO2envCurve);
	LFO2Envelope[2].setEnvType(LFO2envCurve);
	LFO2Envelope[3].setEnvType(LFO2envCurve);
	LFO2Envelope[4].setEnvType(LFO2envCurve);
	LFO2Envelope[5].setEnvType(LFO2envCurve);
	LFO2Envelope[6].setEnvType(LFO2envCurve);
	LFO2Envelope[7].setEnvType(LFO2envCurve);
	
	LFO3EnvelopeAmp.setEnvType(LFO3envCurve);
	
}


FLASHMEM void updatePWMSourceA() {
	if (pwmSource == PWMSOURCELFO) {
		setPwmMixerAFEnv(0);//Set filter mod to zero
		//setPwmMixerBFEnv(0);//Set filter mod to zero
		if (pwmRateA > -5) {
			setPwmMixerALFO(pwmAmtA);//Set LFO mod
			//setPwmMixerBLFO(pwmAmtB);//Set LFO mod
		}
		} else {
		setPwmMixerALFO(0);//Set LFO mod to zero
		//setPwmMixerBLFO(0);//Set LFO mod to zero
		if (pwmRateA > -5) {
			setPwmMixerAFEnv(pwmAmtA);//Set filter mod
			//setPwmMixerBFEnv(pwmAmtB);//Set filter mod
		}
	}
}

FLASHMEM void updatePWMSourceB() {
	if (pwmSource == PWMSOURCELFO) {
		//setPwmMixerAFEnv(0);//Set filter mod to zero
		setPwmMixerBFEnv(0);//Set filter mod to zero
		if (pwmRateB > -5) {
			//setPwmMixerALFO(pwmAmtA);//Set LFO mod
			setPwmMixerBLFO(pwmAmtB);//Set LFO mod
		}
		} else {
		//setPwmMixerALFO(0);//Set LFO mod to zero
		setPwmMixerBLFO(0);//Set LFO mod to zero
		if (pwmRateB > -5) {
			//setPwmMixerAFEnv(pwmAmtA);//Set filter mod
			setPwmMixerBFEnv(pwmAmtB);//Set filter mod
		}
	}
}

FLASHMEM void updatePWMRateA() {
	pwmLfoA.frequency(pwmRateA);
	//pwmLfoB.frequency(pwmRate);
	if (pwmRateA == -10) {
		//Set to fixed PW mode
		setPwmMixerALFO(0);//LFO Source off
		//setPwmMixerBLFO(0);
		setPwmMixerAFEnv(0);//Filter Env Source off
		//setPwmMixerBFEnv(0);
		setPwmMixerAPW(1);//Manually adjustable pulse width on
		//setPwmMixerBPW(1);
		} else if (pwmRateA == -5) {
		//Set to Filter Env Mod source
		pwmSource = PWMSOURCEFENV;
		updatePWMSourceA();
		setPwmMixerAFEnv(pwmAmtA);
		//setPwmMixerBFEnv(pwmAmtB);
		setPwmMixerAPW(0);
		//setPwmMixerBPW(0);
		} else {
		pwmSource = PWMSOURCELFO;
		updatePWMSourceA();
		setPwmMixerAPW(0);
		//setPwmMixerBPW(0);
	}
}

FLASHMEM void updatePWMRateB() {
	//pwmLfoA.frequency(pwmRate);
	pwmLfoB.frequency(pwmRateB);
	if (pwmRateB == -10) {
		//Set to fixed PW mode
		//setPwmMixerALFO(0);//LFO Source off
		setPwmMixerBLFO(0);
		//setPwmMixerAFEnv(0);//Filter Env Source off
		setPwmMixerBFEnv(0);
		//setPwmMixerAPW(1);//Manually adjustable pulse width on
		setPwmMixerBPW(1);
		} else if (pwmRateB == -5) {
		//Set to Filter Env Mod source
		pwmSource = PWMSOURCEFENV;
		updatePWMSourceB();
		//setPwmMixerAFEnv(pwmAmtA);
		setPwmMixerBFEnv(pwmAmtB);
		//setPwmMixerAPW(0);
		setPwmMixerBPW(0);
		} else {
		pwmSource = PWMSOURCELFO;
		updatePWMSourceB();
		//setPwmMixerAPW(0);
		setPwmMixerBPW(0);
	}
}

FLASHMEM void updatePWMAmount() {
	//MIDI only - sets both osc
	if (PageNr == 1) {
		pwA = 0;
		setPwmMixerALFO(pwmAmtA);
		} else {
		pwB = 0;
		setPwmMixerBLFO(pwmAmtB);
	}
}

FLASHMEM void updatePWA() {
	if (pwmRateA == -10) {
		//if PWM amount is around zero, fixed PW is enabled
		setPwmMixerALFO(0);
		//setPwmMixerBLFO(0);
		setPwmMixerAFEnv(0);
		//setPwmMixerBFEnv(0);
		setPwmMixerAPW(1);
		//setPwmMixerBPW(1);
		} else {
		setPwmMixerAPW(0);
		//setPwmMixerBPW(0);
		if (pwmSource == PWMSOURCELFO) {
			//PW alters PWM LFO amount for waveform A
			setPwmMixerALFO(pwmAmtA);
			} else {
			//PW alters PWM Filter Env amount for waveform A
			setPwmMixerAFEnv(pwmAmtA);
		}
	}
	float pwA_Adj = pwA;//Prevent silence when pw = +/-1.0 on pulse
	if (pwA > 0.98) pwA_Adj = 0.98f;
	if (pwA < -0.98) pwA_Adj = -0.98f;
	pwa.amplitude(pwA_Adj);
}

FLASHMEM void updatePWB() {
	if (pwmRateB == -10)  {
		//if PWM amount is around zero, fixed PW is enabled
		//setPwmMixerALFO(0);
		setPwmMixerBLFO(0);
		//setPwmMixerAFEnv(0);
		setPwmMixerBFEnv(0);
		//setPwmMixerAPW(1);
		setPwmMixerBPW(1);
		} else {
		//setPwmMixerAPW(0);
		setPwmMixerBPW(0);
		if (pwmSource == PWMSOURCELFO) {
			//PW alters PWM LFO amount for waveform B
			setPwmMixerBLFO(pwmAmtB);
			} else {
			//PW alters PWM Filter Env amount for waveform B
			setPwmMixerBFEnv(pwmAmtB);
		}
	}
	float pwB_Adj = pwB;//Prevent silence when pw = +/-1 on pulse
	if (pwB > 0.98) pwB_Adj = 0.98f;
	if (pwB < -0.98) pwB_Adj = -0.98f;
	pwb.amplitude(pwB_Adj);
}

//*************************************************************************
//  Update OscFX
//*************************************************************************
FLASHMEM void updateOscFX() {
	
	
	switch (oscFX) {
		case 0:	// oscFX Mode: OFF
		setfrequencyModulation();
		setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
		updateOscLevelA();
		updateOscLevelB();
		setRingModulation(false);
		break;
		
		case 1:	// oscFX Mode: XOR
		setfrequencyModulation();
		setOscFXCombineMode(AudioEffectDigitalCombine::XOR);
		updateOscLevelA();
		updateOscLevelB();
		setRingModulation(false);
		break;
		
		case 2:	// oscFX Mode: XMO
		setfrequencyModulation();
		setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
		updateOscLevelA();
		updateOscLevelB();
		setRingModulation(false);
		break;
		
		case 3:	// oscFX Mode: MOD
		setfrequencyModulation();
		setOscFXCombineMode(AudioEffectDigitalCombine::MODULO);
		updateOscLevelA();
		updateOscLevelB();
		setRingModulation(false);
		break;
		
		case 4:	// oscFX Mode: AND
		setfrequencyModulation();
		setOscFXCombineMode(AudioEffectDigitalCombine::AND);
		updateOscLevelA();
		updateOscLevelB();
		setRingModulation(false);
		break;
		
		case 5:	// oscFX Mode: PHA
		setphaseModulation();
		setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
		updateOscLevelA();
		updateOscLevelB();
		setRingModulation(false);
		break;
		
		case 6:	// oscFX Mode: FM
		setfrequencyModulation();
		setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
		updateOscLevelA();
		updateOscLevelB();
		setRingModulation(false);
		break;
		
		case 7:	// oscFX Mode: RING
		setRingModulation(true);
		setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
		updateOscLevelA();
		updateOscLevelB();
		break;
	}
}


FLASHMEM void updateOscLevelA() {

	switch (oscFX) {
		
		case 0:	// oscFX Mode: off
		setOscModMixerA(3, 0);	//Feed from Osc2: off
		setWaveformMixerLevel(0, (oscALevel / 2));	// Osc1 Mix Level 0.5
		setWaveformMixerLevel(3, 0);	// oscFX out: off
		break;
		
		case 1:	// oscFX Mode: XOR
		setOscModMixerA(3, 0);	//Feed from Osc2: off
		setWaveformMixerLevel(0, (oscALevel / 2));//Osc 1 (A)
		setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt));//oscFX XOR level
		break;
		
		case 2:	// oscFX Mode: XMO
		if (oscALevel == 1.0f && oscBLevel <= 1.0f) {
			setOscModMixerA(3, ((1 - oscBLevel) * Osc1ModAmt) * Osc1ModAmt);//Feed from Osc 2 (B)
			setWaveformMixerLevel(0, ONE);//Osc 1 (A)
			setWaveformMixerLevel(1, 0);//Osc 2 (B)
		}
		setWaveformMixerLevel(3, 0);	// oscFX out: off
		break;
		
		case 3:	// oscFX Mode: MOD
		setOscModMixerA(3, 0);//XMod off
		setOscModMixerB(3, 0);//XMod off
		setWaveformMixerLevel(0, (oscALevel / 2));	//Osc 1 (A)
		setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt));//oscFX XOR level
		break;
		
		case 4:	// oscFX Mode: AND
		setOscModMixerA(3, 0);//XMod off
		setWaveformMixerLevel(0, (oscALevel / 2));//Osc 1 (A)
		setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt));//oscFX XOR level
		break;
		
		case 5:	// oscFX Mode: PHA
		setOscModMixerB(3, 0);//XMod off
		setWaveformMixerLevel(0, (oscALevel / 2));//Osc 1 (A)
		setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt));//oscFX XOR level
		setOscModMixerA(3, ((oscBLevel / 2) * Osc1ModAmt));
		break;
		
		case 6:	// oscFX Mode: FM
		setOscModMixerB(3, 0);//XMod off
		setWaveformMixerLevel(0, (oscALevel / 2));//Osc 1 (A)
		setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt));//oscFX XOR level
		setOscModMixerA(3, ((oscBLevel / 2) * Osc1ModAmt));
		break;
		
		case 7:	// oscFX Mode: Ring
		setOscModMixerA(3, 0);//XMod off
		setWaveformMixerLevel(0, (oscALevel * (1.0f - Osc1ModAmt)));	//Osc1
		setWaveformMixerLevel(3, ((oscALevel + oscBLevel) * (Osc1ModAmt * 2)));//oscFX XOR level
	}
}

FLASHMEM void updateOscLevelB() {
	
	switch (oscFX) {
		case 0: // oscFX Mode: off
		setOscModMixerB(3, 0);	//Feed from Osc1: off
		setWaveformMixerLevel(1, (oscBLevel / 2)); // Osc2 Mix Level 0.5
		setWaveformMixerLevel(3, 0);	// oscFX out: off
		break;
		
		case 1:	// oscFX Mode: XOR
		setOscModMixerB(3, 0);	//Feed from Osc1: off
		setWaveformMixerLevel(1, (oscBLevel / 2));//Osc 2 (B)
		setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt));//oscFX XOR level
		break;
		
		case 2:	// oscFX Mode: XMO
		if (oscBLevel == 1.0f && oscALevel < 1.0f) {
			setOscModMixerA(3, ((1 - oscALevel) * Osc1ModAmt) * Osc1ModAmt);//Feed from Osc 2 (B)
			setWaveformMixerLevel(0, 0);//Osc 1 (A)
			setWaveformMixerLevel(1, ONE);//Osc 2 (B)
		}
		setWaveformMixerLevel(3, 0);	// oscFX out: off
		break;
		
		case 3:	// oscFX Mode: MOD
		setOscModMixerA(3, 0);//XMod off
		setOscModMixerB(3, 0);//XMod off
		setWaveformMixerLevel(1, (oscBLevel / 2));	//Osc 1 (A)
		setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt));//oscFX XOR level
		break;
		
		case 4:	// oscFX Mode: AND
		setOscModMixerB(3, 0);//XMod off
		setWaveformMixerLevel(1, (oscBLevel / 2));//Osc 2 (B)
		setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt));//oscFX XOR level
		break;
		
		case 5:	// oscFX Mode: PHA
		setWaveformMixerLevel(1, (oscBLevel / 2));//Osc 2 (B)
		setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt));//oscFX XOR level
		setOscModMixerA(3, ((oscALevel / 2) * Osc1ModAmt));
		break;
		
		case 6:	// oscFX Mode: FM
		setWaveformMixerLevel(1, (oscBLevel / 2));//Osc 2 (B)
		setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt));//oscFX XOR level
		setOscModMixerA(3, ((oscALevel / 2) * Osc1ModAmt));
		break;
		
		case 7:	// oscFX Mode: Ring
		setOscModMixerB(3, 0);//XMod off
		setWaveformMixerLevel(1, (oscBLevel * (1.0f - Osc1ModAmt)));	//Osc2
		setWaveformMixerLevel(3, ((oscALevel + oscBLevel) * (Osc1ModAmt * 2)));//oscFX XOR level
		break;
	}
}



//****************************************************************************************************************

FLASHMEM void setWaveformMixerLevel(int channel, float level) {
	level = level * oscGainLevel;
	waveformMixer1.gain(channel, level);
	waveformMixer2.gain(channel, level);
	waveformMixer3.gain(channel, level);
	waveformMixer4.gain(channel, level);
	waveformMixer5.gain(channel, level);
	waveformMixer6.gain(channel, level);
	waveformMixer7.gain(channel, level);
	waveformMixer8.gain(channel, level);
}

FLASHMEM void setAmpLevel(float level) {
	oscGainLevel = level;
	updateOscLevelA();
	updateOscLevelB();
}

FLASHMEM void setOscModMixerA(int channel, float level) {
	oscModMixer1a.gain(channel, level);
	oscModMixer2a.gain(channel, level);
	oscModMixer3a.gain(channel, level);
	oscModMixer4a.gain(channel, level);
	oscModMixer5a.gain(channel, level);
	oscModMixer6a.gain(channel, level);
	oscModMixer7a.gain(channel, level);
	oscModMixer8a.gain(channel, level);
}

FLASHMEM void setOscModMixerB(int channel, float level) {
	oscModMixer1b.gain(channel, level);
	oscModMixer2b.gain(channel, level);
	oscModMixer3b.gain(channel, level);
	oscModMixer4b.gain(channel, level);
	oscModMixer5b.gain(channel, level);
	oscModMixer6b.gain(channel, level);
	oscModMixer7b.gain(channel, level);
	oscModMixer8b.gain(channel, level);
}

FLASHMEM void updateNoiseLevel() {
	if (noiseLevel > 0) {
		pink.amplitude(noiseLevel);
		white.amplitude(0.0f);
		} else if (noiseLevel < 0) {
		pink.amplitude(0.0f);
		white.amplitude(abs(noiseLevel));
		} else {
		pink.amplitude(noiseLevel);
		white.amplitude(noiseLevel);
	}
}

FLASHMEM void updateFilterFreq() {
	if (myFilter == 1) {
		filter1.frequency(filterFreq);
		filter2.frequency(filterFreq);
		filter3.frequency(filterFreq);
		filter4.frequency(filterFreq);
		filter5.frequency(filterFreq);
		filter6.frequency(filterFreq);
		filter7.frequency(filterFreq);
		filter8.frequency(filterFreq);
		filterOctave = 7.0f;
		filter1.octaveControl(filterOctave);
		filter2.octaveControl(filterOctave);
		filter3.octaveControl(filterOctave);
		filter4.octaveControl(filterOctave);
		filter5.octaveControl(filterOctave);
		filter6.octaveControl(filterOctave);
		filter7.octaveControl(filterOctave);
		filter8.octaveControl(filterOctave);
		} else {
		filter2_1.frequency(filterFreq);
		filter2_2.frequency(filterFreq);
		filter2_3.frequency(filterFreq);
		filter2_4.frequency(filterFreq);
		filter2_5.frequency(filterFreq);
		filter2_6.frequency(filterFreq);
		filter2_7.frequency(filterFreq);
		filter2_8.frequency(filterFreq);
		filterOctave = 7.0f;
		filter2_1.octaveControl(filterOctave);
		filter2_2.octaveControl(filterOctave);
		filter2_3.octaveControl(filterOctave);
		filter2_4.octaveControl(filterOctave);
		filter2_5.octaveControl(filterOctave);
		filter2_6.octaveControl(filterOctave);
		filter2_7.octaveControl(filterOctave);
		filter2_8.octaveControl(filterOctave);
	}
}

FLASHMEM void updateFilterSweeb(int value) {
	
	// calc cutoff
	float val;
	float filterOctave = 7.0f;	// set filter ctrl range (7 octav)
	float frequenz = 440.0f;	// set filter's corner frequency
	
	val = DIV255 * value * 1.30f;
	val = -0.66 + val;
	DC_FilterModCC.amplitude(val);
	
	// State Variable Filter
	if (myFilter == 1) {	
		filter1.frequency(frequenz);
		filter2.frequency(frequenz);
		filter3.frequency(frequenz);
		filter4.frequency(frequenz);
		filter5.frequency(frequenz);
		filter6.frequency(frequenz);
		filter7.frequency(frequenz);
		filter8.frequency(frequenz);
		filter1.octaveControl(filterOctave);
		filter2.octaveControl(filterOctave);
		filter3.octaveControl(filterOctave);
		filter4.octaveControl(filterOctave);
		filter5.octaveControl(filterOctave);
		filter6.octaveControl(filterOctave);
		filter7.octaveControl(filterOctave);
		filter8.octaveControl(filterOctave); } 
		// Ladder Filter
		else {
		filter2_1.frequency(frequenz);
		filter2_2.frequency(frequenz);
		filter2_3.frequency(frequenz);
		filter2_4.frequency(frequenz);
		filter2_5.frequency(frequenz);
		filter2_6.frequency(frequenz);
		filter2_7.frequency(frequenz);
		filter2_8.frequency(frequenz);
		filter2_1.octaveControl(filterOctave);
		filter2_2.octaveControl(filterOctave);
		filter2_3.octaveControl(filterOctave);
		filter2_4.octaveControl(filterOctave);
		filter2_5.octaveControl(filterOctave);
		filter2_6.octaveControl(filterOctave);
		filter2_7.octaveControl(filterOctave);
		filter2_8.octaveControl(filterOctave);
	}
}

FLASHMEM void updateHPFFilterFreq() {
	uint16_t hpf_value = HPF_filterFreq_value + ccModwheelHPFamt;
	if (hpf_value >= 0xFF) {
		hpf_value = 0xFF;
	}
	HPF_filterFreq = HPF_FILTERFREQS256[hpf_value]; // convert 7bit value into frequency
	filterOctave = 7.0f;
	hpFilter1.frequency(HPF_filterFreq);
	hpFilter1.octaveControl(filterOctave);
	hpFilter2.frequency(HPF_filterFreq);
	hpFilter2.octaveControl(filterOctave);
}

FLASHMEM void updateHPFFilterRes() {
	HPFRes = (14.29f * LINEAR[HPF_Res_value]) + 0.71f; // convert 7bit value into frequency
	hpFilter1.resonance(HPFRes);
	hpFilter2.resonance(HPFRes);
}

FLASHMEM void updateFilterRes() {
	
	float maxReso;
	
	if (myFilter == 1) {
		maxReso = 15.0f;
		if (filterRes >= maxReso) {
			filterRes = maxReso;
		}
		filter1.resonance(filterRes);
		filter2.resonance(filterRes);
		filter3.resonance(filterRes);
		filter4.resonance(filterRes);
		filter5.resonance(filterRes);
		filter6.resonance(filterRes);
		filter7.resonance(filterRes);
		filter8.resonance(filterRes);
		} else {
		maxReso = 1.8f;
		if (filterRes >= maxReso) {
			filterRes = maxReso;
		}
		filter2_1.resonance(filterRes);
		filter2_2.resonance(filterRes);
		filter2_3.resonance(filterRes);
		filter2_4.resonance(filterRes);
		filter2_5.resonance(filterRes);
		filter2_6.resonance(filterRes);
		filter2_7.resonance(filterRes);
		filter2_8.resonance(filterRes);
	}
}

//*******************************************************************
// Update Filter
//*******************************************************************
FLASHMEM void updateFilterMixer() {
	
	float LP = 1.0f;
	float BP = 0.0f;
	float HP = 0.0f;
	float LA = 0.0f;
	
	// State Variable Filter
	if (myFilter == 1) {
		String filterStr;
		if (filterMix == LINEAR_FILTERMIXER[127]) {
			//BP mode
			LP = 0.0f;
			BP = 1.0f;
			HP = 0.0f;
			LA = 0.0f;
			filterStr = "Band Pass";
			} else {
			//LP-HP mix mode - a notch filter
			LP = 1.0f - filterMix;
			BP = 0.0f;
			HP = filterMix;
			LA = 0.0f;			
		}
		} else {		// Ladder Filter
		LP = 0.0f;
		BP = 0.0f;
		HP = 0.0f;
		LA = 1.0f;
	}

	filterMixer1.gain(0, LP);
	filterMixer1.gain(1, BP);
	filterMixer1.gain(2, HP);
	filterMixer1.gain(3, LA);
	filterMixer2.gain(0, LP);
	filterMixer2.gain(1, BP);
	filterMixer2.gain(2, HP);
	filterMixer2.gain(3, LA);
	filterMixer3.gain(0, LP);
	filterMixer3.gain(1, BP);
	filterMixer3.gain(2, HP);
	filterMixer3.gain(3, LA);
	filterMixer4.gain(0, LP);
	filterMixer4.gain(1, BP);
	filterMixer4.gain(2, HP);
	filterMixer4.gain(3, LA);
	filterMixer5.gain(0, LP);
	filterMixer5.gain(1, BP);
	filterMixer5.gain(2, HP);
	filterMixer5.gain(3, LA);
	filterMixer6.gain(0, LP);
	filterMixer6.gain(1, BP);
	filterMixer6.gain(2, HP);
	filterMixer6.gain(3, LA);
	filterMixer7.gain(0, LP);
	filterMixer7.gain(1, BP);
	filterMixer7.gain(2, HP);
	filterMixer7.gain(3, LA);
	filterMixer8.gain(0, LP);
	filterMixer8.gain(1, BP);
	filterMixer8.gain(2, HP);
	filterMixer8.gain(3, LA);
}

FLASHMEM void updateLadderFilterDrive(uint8_t value) {
	if (myFilter == 2) {
		if (value <= 1) {
			value = 1;
		}
		float Div = 4.0f / 128;
		float drv = float(value * Div);
		filter2_1.inputDrive(drv);
		filter2_2.inputDrive(drv);
		filter2_3.inputDrive(drv);
		filter2_4.inputDrive(drv);
		filter2_5.inputDrive(drv);
		filter2_6.inputDrive(drv);
		filter2_7.inputDrive(drv);
		filter2_8.inputDrive(drv);
	}
}

FLASHMEM void updateLadderFilterPassbandGain(uint8_t value) {

	if (myFilter == 2) {
		float Div = 0.5f / 128;
		float drv = float(value * Div);
		filter2_1.passbandGain(drv);
		filter2_2.passbandGain(drv);
		filter2_3.passbandGain(drv);
		filter2_4.passbandGain(drv);
		filter2_5.passbandGain(drv);
		filter2_6.passbandGain(drv);
		filter2_7.passbandGain(drv);
		filter2_8.passbandGain(drv);
	}
}

FLASHMEM void updateLFO1env(){
	/*
	float t_delay = LFO1delayTime;
	LFO1Envelope1.delay(t_delay);
	LFO1Envelope2.delay(t_delay);
	LFO1Envelope3.delay(t_delay);
	LFO1Envelope4.delay(t_delay);
	LFO1Envelope5.delay(t_delay);
	LFO1Envelope6.delay(t_delay);
	LFO1Envelope7.delay(t_delay);
	LFO1Envelope8.delay(t_delay);
	
	float t_fade = LFO1fadeTime;
	LFO1Envelope1.attack(t_fade);
	LFO1Envelope2.attack(t_fade);
	LFO1Envelope3.attack(t_fade);
	LFO1Envelope4.attack(t_fade);
	LFO1Envelope5.attack(t_fade);
	LFO1Envelope6.attack(t_fade);
	LFO1Envelope7.attack(t_fade);
	LFO1Envelope8.attack(t_fade);
	
	
	
	
	
	LFO1Envelope1.delay(LFO1delayTime);
	LFO1Envelope2.delay(LFO1delayTime);
	LFO1Envelope3.delay(LFO1delayTime);
	LFO1Envelope4.delay(LFO1delayTime);
	LFO1Envelope5.delay(LFO1delayTime);
	LFO1Envelope6.delay(LFO1delayTime);
	LFO1Envelope7.delay(LFO1delayTime);
	LFO1Envelope8.delay(LFO1delayTime);
	LFO1Envelope1.attack(LFO1fadeTime);
	LFO1Envelope2.attack(LFO1fadeTime);
	LFO1Envelope3.attack(LFO1fadeTime);
	LFO1Envelope4.attack(LFO1fadeTime);
	LFO1Envelope5.attack(LFO1fadeTime);
	LFO1Envelope6.attack(LFO1fadeTime);
	LFO1Envelope7.attack(LFO1fadeTime);
	LFO1Envelope8.attack(LFO1fadeTime);
	LFO1Envelope1.decay(0.0f);
	LFO1Envelope2.decay(0.0f);
	LFO1Envelope3.decay(0.0f);
	LFO1Envelope4.decay(0.0f);
	LFO1Envelope5.decay(0.0f);
	LFO1Envelope6.decay(0.0f);
	LFO1Envelope7.decay(0.0f);
	LFO1Envelope8.decay(0.0f);
	LFO1Envelope1.sustain(1.0f);
	LFO1Envelope2.sustain(1.0f);
	LFO1Envelope3.sustain(1.0f);
	LFO1Envelope4.sustain(1.0f);
	LFO1Envelope5.sustain(1.0f);
	LFO1Envelope6.sustain(1.0f);
	LFO1Envelope7.sustain(1.0f);
	LFO1Envelope8.sustain(1.0f);
	LFO1Envelope1.release(ampRelease);
	LFO1Envelope2.release(ampRelease);
	LFO1Envelope3.release(ampRelease);
	LFO1Envelope4.release(ampRelease);
	LFO1Envelope5.release(ampRelease);
	LFO1Envelope6.release(ampRelease);
	LFO1Envelope7.release(ampRelease);
	LFO1Envelope8.release(ampRelease);
	*/

}

FLASHMEM void updateFilterEnv() {
	setFilterModMixer(0, filterEnv);
}

FLASHMEM void updatePitchEnv() {
	setOscModMixerA(1, pitchEnvA);
	setOscModMixerB(1, pitchEnvB);
}


FLASHMEM void updateKeyTracking() {
	setFilterModMixer(2, keytrackingValue);
}

FLASHMEM void setFilterModMixer(int channel, float level) {
	filterModMixer_a[0].gain(channel, level);
	filterModMixer_a[1].gain(channel, level);
	filterModMixer_a[2].gain(channel, level);
	filterModMixer_a[3].gain(channel, level);
	filterModMixer_a[4].gain(channel, level);
	filterModMixer_a[5].gain(channel, level);
	filterModMixer_a[6].gain(channel, level);
	filterModMixer_a[7].gain(channel, level);
}

FLASHMEM void updateAtouchPitch() {
	AtouchModAmp2.gain(AtouchPitch);
}

FLASHMEM void updateAtouchCutoff() {
	float val = (AtouchCutoff / 2.0f);
	AtouchModAmp1.gain(val);
}

FLASHMEM void updateOscLFOAmt() {
	float value = oscLfoAmt + (AtouchLFO1amt * 0.0015f) + ccModwheelAmt;
	if (value >= 0.50f) {
		value = 0.50f;
	}
	
	if (LFO1fadeTime != 0.0f || LFO1releaseTime != 0.0f) {
		oscGlobalModMixer1.gain(1,value);	// set pitch LFO fade value
		oscGlobalModMixer2.gain(1,value);
		oscGlobalModMixer3.gain(1,value);
		oscGlobalModMixer4.gain(1,value);
		oscGlobalModMixer5.gain(1,value);
		oscGlobalModMixer6.gain(1,value);
		oscGlobalModMixer7.gain(1,value);
		oscGlobalModMixer8.gain(1,value);
		oscGlobalModMixer1.gain(2,0);		// set pitch LFO off
		oscGlobalModMixer2.gain(2,0);
		oscGlobalModMixer3.gain(2,0);
		oscGlobalModMixer4.gain(2,0);
		oscGlobalModMixer5.gain(2,0);
		oscGlobalModMixer6.gain(2,0);
		oscGlobalModMixer7.gain(2,0);
		oscGlobalModMixer8.gain(2,0);
	}
	else {
		oscGlobalModMixer1.gain(1,0);	// set pitch LFO fade off
		oscGlobalModMixer2.gain(1,0);
		oscGlobalModMixer3.gain(1,0);
		oscGlobalModMixer4.gain(1,0);
		oscGlobalModMixer5.gain(1,0);
		oscGlobalModMixer6.gain(1,0);
		oscGlobalModMixer7.gain(1,0);
		oscGlobalModMixer8.gain(1,0);
		oscGlobalModMixer1.gain(2,value);	// set pitch LFO value
		oscGlobalModMixer2.gain(2,value);
		oscGlobalModMixer3.gain(2,value);
		oscGlobalModMixer4.gain(2,value);
		oscGlobalModMixer5.gain(2,value);
		oscGlobalModMixer6.gain(2,value);
		oscGlobalModMixer7.gain(2,value);
		oscGlobalModMixer8.gain(2,value);
	}
}

FLASHMEM void updateModWheel() {
	float Amt = oscLfoAmt + ccModwheelAmt;
	oscGlobalModMixer1.gain(2,Amt);
	oscGlobalModMixer2.gain(2,Amt);
	oscGlobalModMixer3.gain(2,Amt);
	oscGlobalModMixer4.gain(2,Amt);
	oscGlobalModMixer5.gain(2,Amt);
	oscGlobalModMixer6.gain(2,Amt);
	oscGlobalModMixer7.gain(2,Amt);
	oscGlobalModMixer8.gain(2,Amt);
	/*
	oscGlobalModMixer1.gain(2,ccModwheelAmt);
	oscGlobalModMixer2.gain(2,ccModwheelAmt);
	oscGlobalModMixer3.gain(2,ccModwheelAmt);
	oscGlobalModMixer4.gain(2,ccModwheelAmt);
	oscGlobalModMixer5.gain(2,ccModwheelAmt);
	oscGlobalModMixer6.gain(2,ccModwheelAmt);
	oscGlobalModMixer7.gain(2,ccModwheelAmt);
	oscGlobalModMixer8.gain(2,ccModwheelAmt);
	
	filterModMixAtCC1.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC2.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC3.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC4.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC5.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC6.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC7.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC8.gain(2,ccModwheelCutoffAmt);
	*/
}

FLASHMEM void updateModWheelCutoff() {
	filterModMixAtCC1.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC2.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC3.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC4.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC5.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC6.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC7.gain(2,ccModwheelCutoffAmt);
	filterModMixAtCC8.gain(2,ccModwheelCutoffAmt);
}


FLASHMEM void initAtouchPitch() {
	AtouchModAmp1.gain(0.0f);
	AtouchModAmp2.gain(0.0f);
	AtouchModAmp3.gain(0.0f);
	AtouchModAmp4.gain(0.0f);
}

FLASHMEM void updatePitchLFORate() {
	pitchLfo.frequency(oscLfoRate);
}

FLASHMEM void updatePitchLFOWaveform() {
	pitchLfo.begin(oscLFOWaveform);
}

//MIDI CC only
FLASHMEM void updatePitchLFOMidiClkSync() {
}

FLASHMEM void updateFilterLfoRate() {
	filterLfo.frequency(filterLfoRate);
}

FLASHMEM void updateLfo3Rate() {
	ModLfo3.frequency(Lfo3Rate);
}

FLASHMEM void updateFilterLfoAmt() {
	float value = filterLfoAmt + (AtouchLFO2amt * 0.0039f);
	if (value >= 0.5f) {
		value = 0.5f;
	}
	if (LFO2fadeTime != 0.0f || LFO2releaseTime != 0.0f) {
		LFO2mix1.gain(0,value);
		LFO2mix2.gain(0,value);
		LFO2mix3.gain(0,value);
		LFO2mix4.gain(0,value);
		LFO2mix5.gain(0,value);
		LFO2mix6.gain(0,value);
		LFO2mix7.gain(0,value);
		LFO2mix8.gain(0,value);
		LFO2mix1.gain(1,0);
		LFO2mix2.gain(1,0);
		LFO2mix3.gain(1,0);
		LFO2mix4.gain(1,0);
		LFO2mix5.gain(1,0);
		LFO2mix6.gain(1,0);
		LFO2mix7.gain(1,0);
		LFO2mix8.gain(1,0);
	}
	else {
		LFO2mix1.gain(0,0);
		LFO2mix2.gain(0,0);
		LFO2mix3.gain(0,0);
		LFO2mix4.gain(0,0);
		LFO2mix5.gain(0,0);
		LFO2mix6.gain(0,0);
		LFO2mix7.gain(0,0);
		LFO2mix8.gain(0,0);
		LFO2mix1.gain(1,value);
		LFO2mix2.gain(1,value);
		LFO2mix3.gain(1,value);
		LFO2mix4.gain(1,value);
		LFO2mix5.gain(1,value);
		LFO2mix6.gain(1,value);
		LFO2mix7.gain(1,value);
		LFO2mix8.gain(1,value);
	}
}

FLASHMEM void updateFilterLFOWaveform() {
	filterLfo.begin(filterLfoWaveform);
}

FLASHMEM void updatePitchLFORetrig() {
}

FLASHMEM void updateFilterLFORetrig() {
}

FLASHMEM void updateFilterLFOMidiClkSync() {
}

// update Filter Envelope -------------------------------------------------
FLASHMEM void updateFilterAttack() {
	filterEnvelope[0].delay(0);
	filterEnvelope[1].delay(0);
	filterEnvelope[2].delay(0);
	filterEnvelope[3].delay(0);
	filterEnvelope[4].delay(0);
	filterEnvelope[5].delay(0);
	filterEnvelope[6].delay(0);
	filterEnvelope[7].delay(0);
	filterEnvelope[0].attack(filterAttack);
	filterEnvelope[1].attack(filterAttack);
	filterEnvelope[2].attack(filterAttack);
	filterEnvelope[3].attack(filterAttack);
	filterEnvelope[4].attack(filterAttack);
	filterEnvelope[5].attack(filterAttack);
	filterEnvelope[6].attack(filterAttack);
	filterEnvelope[7].attack(filterAttack);
	filterEnvelope[0].hold(0);
	filterEnvelope[1].hold(0);
	filterEnvelope[2].hold(0);
	filterEnvelope[3].hold(0);
	filterEnvelope[4].hold(0);
	filterEnvelope[5].hold(0);
	filterEnvelope[6].hold(0);
	filterEnvelope[7].hold(0);
}
FLASHMEM void updateFilterDecay() {
	filterEnvelope[0].decay(filterDecay);
	filterEnvelope[1].decay(filterDecay);
	filterEnvelope[2].decay(filterDecay);
	filterEnvelope[3].decay(filterDecay);
	filterEnvelope[4].decay(filterDecay);
	filterEnvelope[5].decay(filterDecay);
	filterEnvelope[6].decay(filterDecay);
	filterEnvelope[7].decay(filterDecay);
}
FLASHMEM void updateFilterSustain() {
	filterEnvelope[0].sustain(filterSustain);
	filterEnvelope[1].sustain(filterSustain);
	filterEnvelope[2].sustain(filterSustain);
	filterEnvelope[3].sustain(filterSustain);
	filterEnvelope[4].sustain(filterSustain);
	filterEnvelope[5].sustain(filterSustain);
	filterEnvelope[6].sustain(filterSustain);
	filterEnvelope[7].sustain(filterSustain);
}
FLASHMEM void updateFilterRelease() {
	filterEnvelope[0].release(filterRelease);
	filterEnvelope[1].release(filterRelease);
	filterEnvelope[2].release(filterRelease);
	filterEnvelope[3].release(filterRelease);
	filterEnvelope[4].release(filterRelease);
	filterEnvelope[5].release(filterRelease);
	filterEnvelope[6].release(filterRelease);
	filterEnvelope[7].release(filterRelease);
	filterEnvelope[0].releaseNoteOn(0);
	filterEnvelope[1].releaseNoteOn(0);
	filterEnvelope[2].releaseNoteOn(0);
	filterEnvelope[3].releaseNoteOn(0);
	filterEnvelope[4].releaseNoteOn(0);
	filterEnvelope[5].releaseNoteOn(0);
	filterEnvelope[6].releaseNoteOn(0);
	filterEnvelope[7].releaseNoteOn(0);
}

// update Amp Envelope ----------------------------------------------------
FLASHMEM void updateAttack() {
	ampEnvelope[0].delay(0);
	ampEnvelope[1].delay(0);
	ampEnvelope[2].delay(0);
	ampEnvelope[3].delay(0);
	ampEnvelope[4].delay(0);
	ampEnvelope[5].delay(0);
	ampEnvelope[6].delay(0);
	ampEnvelope[7].delay(0);
	ampEnvelope[0].attack(ampAttack);
	ampEnvelope[1].attack(ampAttack);
	ampEnvelope[2].attack(ampAttack);
	ampEnvelope[3].attack(ampAttack);
	ampEnvelope[4].attack(ampAttack);
	ampEnvelope[5].attack(ampAttack);
	ampEnvelope[6].attack(ampAttack);
	ampEnvelope[7].attack(ampAttack);
	ampEnvelope[0].hold(0);
	ampEnvelope[1].hold(0);
	ampEnvelope[2].hold(0);
	ampEnvelope[3].hold(0);
	ampEnvelope[4].hold(0);
	ampEnvelope[5].hold(0);
	ampEnvelope[6].hold(0);
	ampEnvelope[7].hold(0);
}
FLASHMEM void updateDecay() {
	ampEnvelope[0].decay(ampDecay);
	ampEnvelope[1].decay(ampDecay);
	ampEnvelope[2].decay(ampDecay);
	ampEnvelope[3].decay(ampDecay);
	ampEnvelope[4].decay(ampDecay);
	ampEnvelope[5].decay(ampDecay);
	ampEnvelope[6].decay(ampDecay);
	ampEnvelope[7].decay(ampDecay);
}
FLASHMEM void updateSustain() {
	ampEnvelope[0].sustain(ampSustain);
	ampEnvelope[1].sustain(ampSustain);
	ampEnvelope[2].sustain(ampSustain);
	ampEnvelope[3].sustain(ampSustain);
	ampEnvelope[4].sustain(ampSustain);
	ampEnvelope[5].sustain(ampSustain);
	ampEnvelope[6].sustain(ampSustain);
	ampEnvelope[7].sustain(ampSustain);
}
FLASHMEM void updateRelease() {
	ampEnvelope[0].release(ampRelease);
	ampEnvelope[1].release(ampRelease);
	ampEnvelope[2].release(ampRelease);
	ampEnvelope[3].release(ampRelease);
	ampEnvelope[4].release(ampRelease);
	ampEnvelope[5].release(ampRelease);
	ampEnvelope[6].release(ampRelease);
	ampEnvelope[7].release(ampRelease);
	ampEnvelope[0].releaseNoteOn(0);
	ampEnvelope[1].releaseNoteOn(0);
	ampEnvelope[2].releaseNoteOn(0);
	ampEnvelope[3].releaseNoteOn(0);
	ampEnvelope[4].releaseNoteOn(0);
	ampEnvelope[5].releaseNoteOn(0);
	ampEnvelope[6].releaseNoteOn(0);
	ampEnvelope[7].releaseNoteOn(0);
}

FLASHMEM void updateFilterVelocity() {
	float value = myFilVelocity;
	if (value >= 1.0f) {
		value = 1.0f;
	}
	FilterVelo1.amplitude(value);
	FilterVelo2.amplitude(value);
	FilterVelo3.amplitude(value);
	FilterVelo4.amplitude(value);
	FilterVelo5.amplitude(value);
	FilterVelo6.amplitude(value);
	FilterVelo7.amplitude(value);
	FilterVelo8.amplitude(value);
}



FLASHMEM void setphaseModulation() {
	
	float degrees = 180.0f;
	
	for (uint8_t i = 0; i < 8; i++) {
		waveformModa[i].phaseModulation(degrees);
	}	
}

FLASHMEM void setfrequencyModulation() {
	
	float value = 2.0f;
	
	for (uint8_t i = 0; i < 8; i++) {
		waveformModa[i].frequencyModulation(value);
		waveformModb[i].frequencyModulation(value);
	}
}

FLASHMEM void setRingModulation(boolean enabled) {
	if (enabled == true) {
		oscFxMix1.gain(0, 0);
		oscFxMix2.gain(0, 0);
		oscFxMix3.gain(0, 0);
		oscFxMix4.gain(0, 0);
		oscFxMix5.gain(0, 0);
		oscFxMix6.gain(0, 0);
		oscFxMix7.gain(0, 0);
		oscFxMix8.gain(0, 0);
		oscFxMix1.gain(1, 0.5f);
		oscFxMix2.gain(1, 0.5f);
		oscFxMix3.gain(1, 0.5f);
		oscFxMix4.gain(1, 0.5f);
		oscFxMix5.gain(1, 0.5f);
		oscFxMix6.gain(1, 0.5f);
		oscFxMix7.gain(1, 0.5f);
		oscFxMix8.gain(1, 0.5f);
	} else {
		oscFxMix1.gain(0, 1);
		oscFxMix2.gain(0, 1);
		oscFxMix3.gain(0, 1);
		oscFxMix4.gain(0, 1);
		oscFxMix5.gain(0, 1);
		oscFxMix6.gain(0, 1);
		oscFxMix7.gain(0, 1);
		oscFxMix8.gain(0, 1);
		oscFxMix1.gain(1, 0);
		oscFxMix2.gain(1, 0);
		oscFxMix3.gain(1, 0);
		oscFxMix4.gain(1, 0);
		oscFxMix5.gain(1, 0);
		oscFxMix6.gain(1, 0);
		oscFxMix7.gain(1, 0);
		oscFxMix8.gain(1, 0);
	}
	
}

FLASHMEM void setOscFXCombineMode(AudioEffectDigitalCombine::combineMode mode) {
	
	for (uint8_t i = 0; i < 8; i++) {
		oscEffect[i].setCombineMode(mode);
	}
}

//*************************************************************************
// Update Osc overdrive
//*************************************************************************
FLASHMEM void updateOverdrive() {
	setAmpLevel(driveLevel);
}

//*************************************************************************
// Update Osc overdrive
//*************************************************************************
FLASHMEM void updateOscVCFMOD() {
	oscGlobalModMixer1.gain(2, OscVCFMOD);
	oscGlobalModMixer2.gain(2, OscVCFMOD);
	oscGlobalModMixer3.gain(2, OscVCFMOD);
	oscGlobalModMixer4.gain(2, OscVCFMOD);
	oscGlobalModMixer5.gain(2, OscVCFMOD);
	oscGlobalModMixer6.gain(2, OscVCFMOD);
	oscGlobalModMixer7.gain(2, OscVCFMOD);
	oscGlobalModMixer8.gain(2, OscVCFMOD);
}

//*************************************************************************
// Update FX Chip VF1
//*************************************************************************
FLASHMEM void updateFxChip (void)
{
	
	if (FxClkRate <= 10000) {
		FxClkRate = 10000;
	}
	analogWriteFrequency (PWM5, FxClkRate);
	analogWrite(PWM1, FxPot1value * 2);
	analogWrite(PWM2, FxPot2value * 2);
	analogWrite(PWM3, FxPot3value * 2);
	analogWrite(PWM4, FxMixValue * 2);
	
}

//*************************************************************************
// Update PitchBend
//*************************************************************************
FLASHMEM void myPitchBend(byte channel, int bend) {
	bend = (bend * PitchWheelAmt);
	pitchBend.amplitude(bend * 0.000122); // 0.000061
}

//*************************************************************************
// Update Boost
//*************************************************************************
FLASHMEM void updateBoost (void) {
	
	if (myBoost == 1) {
		BassBoostStatus = 1;
		digitalWrite(BassBoost,LOW);  // Boost on
	}
	else {
		BassBoostStatus = 0;
		digitalWrite(BassBoost,HIGH);  // Boost off
	}
}

//*************************************************************************
// Midi Aftertouch
//*************************************************************************
void myAftertouch(byte channel, byte value) {
	
	uint16_t valFx;

	// calc Aftertouch float
	float val;
	val = DIV127 * value;
	AfterTouchValue = value;		// save
	AtouchDc.amplitude(val);
	
	// Aftertouch Pitch
	if (AtouchPitchAmt > 0 || AtouchPitch > 0) {
		AtouchPitch = POWER[AtouchPitchAmt];
		updateAtouchPitch();
	}
	
	// Aftertouch Cutoff
	if (AtouchAmt > 0 || AtouchCutoff > 0) {
		AtouchCutoff = DIV127 * AtouchAmt;
		updateAtouchCutoff();
	}
	
	
	// LFO1 Amt
	if (AtouchLFO1Amt > 0 || AtouchLFO1amt > 0) {
		//val = DIV127 * AtouchLFO1Amt;
		AtouchLFO1amt = POWER[value] * AtouchLFO1Amt;
		updateOscLFOAmt();
	}
	
	// LFO2 Amt
	if (AtouchLFO2Amt > 0 || AtouchLFO2amt > 0) {
		//val = DIV127 * AtouchLFO2Amt;
		AtouchLFO2amt = POWER[value] * AtouchLFO2Amt;
		updateFilterLfoAmt();
	}
	
	// LFO3 Amt
	if (AtouchLFO3Amt > 0 || AtouchLFO3amt > 0) {
		//val = DIV127 * AtouchLFO3Amt;
		AtouchLFO3amt = POWER[value] * AtouchLFO3Amt;
		updateLFO3amt();
	}
	
	// FxP1 Amt
	if (AtouchFxP1Amt > 0 || AtouchFxP1amt > 0) {
		AtouchFxP1amt = AtouchFxP1Amt * val;
		//valFx = (AtouchFxP1amt * 2) + FxPot1Val;
		//analogWrite(PWM1, valFx);  // max 8bit value
	}
	// FxP2 Amt
	if (AtouchFxP2Amt > 0 || AtouchFxP2amt > 0) {
		AtouchFxP2amt = AtouchFxP2Amt * val;	// inv. value
		//valFx = (AtouchFxP2amt * 2) + FxPot2Val;
		//analogWrite(PWM2, valFx);  // max 8bit value
	}
	// FxP3 Amt
	if (AtouchFxP3Amt > 0 || AtouchFxP3amt) {
		AtouchFxP3amt = AtouchFxP3Amt * val;
		//valFx = (AtouchFxP3amt * 2) + FxPot3Val;
		//analogWrite(PWM3, valFx);  // max 8bit value
	}
	
	// FxClk Amt
	if (AtouchFxClkAmt > 0 || AtouchFxCLKamt > 0) {
		AtouchFxCLKamt = AtouchFxClkAmt * val;
		valFx = (AtouchFxCLKamt * 2) + FxPot4Val;	// parameter update into LFO3 routine
		/*
		if (valFx >= 255) {
		valFx = 255;
		}
		uint16_t Rate = (valFx * 196.08f) + 10000;
		analogWriteFrequency (PWM5, Rate);
		*/
	}
	
	// FxMix Amt
	if (AtouchFxMixAmt > 0 || AtouchFxMIXamt > 0) {
		AtouchFxMIXamt = AtouchFxMixAmt * val;			// PWM4 update into LFO3 routine
	}
}

//*************************************************************************
// Midi CC (extern midi ControlChanges)
//*************************************************************************
FLASHMEM void update_MODWHEEL_value(byte value) {
	
		last_modwheel_value = value;
		value = (value * MODWheelAmt);
		ccModwheelAmt = POWER[value] * modWheelDepth; //Variable LFO amount from mod wheel - Settings Option
		updateOscLFOAmt();
		value = (last_modwheel_value * ModWheelCutoffAmt);
		ccModwheelCutoffAmt = (POWER[value] * ModWheelCutoffAmt) * 2; // Variable Cutoff amount from mod wheel - Settings Option
		updateModWheelCutoff();
		ccModwheelHPFamt = (last_modwheel_value * ModWheelHPFamt) * 2;
		updateHPFFilterFreq();
}


//*************************************************************************
// Midi CC (extern midi ControlChanges)
//*************************************************************************
FLASHMEM void myControlChange1(byte channel, byte control, byte value) {
	
	// Modulation Wheel	---------------------------------------------------
	if (control == CCmodwheel) {
		update_MODWHEEL_value(value);
		return;
	}
	
	// HPF/VCF: convert CC value from 127 to 255 --------------------------
	if (control == 62 || control == 74) {
		value = value << 1;
	}
	
	// convert Midi CC to intern control data -----------------------------
	control = MidiCCTabel[control];
	
	myControlChange(channel, control, value);
}


//*************************************************************************
// inter CC 0-50
//*************************************************************************
FLASHMEM void myCCgroup1 (byte control, byte value)
{
	// CC control No: 3
	if (control == CCosclfoamt) {
		//Pick up
		if (!pickUpActive && pickUp && (oscLfoAmtPrevValue <  POWER[value - TOLERANCE] || oscLfoAmtPrevValue >  POWER[value + TOLERANCE])) return; //PICK-UP
		oscLfoAmt = POWER[value];
		updateOscLFOAmt();
		oscLfoAmtPrevValue = oscLfoAmt;//PICK-UP
	}
	
	// CC control No: 5
	else if (control == CCglide) {
		glideSpeed = POWER[value];
		//updateGlide();
		// draw parameter value on menu page1
		if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
			ParameterNr = 3;
			drawParamterFrame(PageNr, ParameterNr);
			tft.fillRoundRect(54,77,22,9,2,ST7735_BLUE);
			tft.setCursor(56,78);
			tft.setTextColor(ST7735_WHITE);
			tft.print(value);
			ParUpdate = true;
			usbMIDI.sendControlChange(30, value, channel);
		}
	}
	
	// CC control No: 9 (Osc1 WaveBank)
	else if (control == CCosc1WaveBank) {
		uint8_t newWaveBank = value;
		if (newWaveBank > 14) {
			newWaveBank = 14;
		}
		if (newWaveBank != Osc1WaveBank) {
			Osc1WaveBank = newWaveBank;
			updateWaveformA();
			if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
				ParameterNr = 0;
				drawParamterFrame(PageNr, ParameterNr);
				tft.fillRoundRect(34,20,7,10,2,ST7735_RED);
				tft.setCursor(35,21);
				tft.setTextColor(ST7735_WHITE);
				tft.println(char(65+newWaveBank));
				draw_Waveform(oscWaveformA, ST7735_RED);
				print_quadsaw_pwamt();  // print PWAMT/SPREAD & PWMOD/SAWMIX
				usbMIDI.sendControlChange(14, Osc1WaveBank, channel);
			}
		}
	}
	
	// CC control No: 10 (Osc2 WaveBank)
	else if (control == CCosc2WaveBank) {
		uint8_t newWaveBank = value;
		if (newWaveBank > 14) {
			newWaveBank = 14;
		}
		if (newWaveBank != Osc2WaveBank) {
			Osc2WaveBank = newWaveBank;
			updateWaveformB();
			if (PageNr == 2 && myPageShiftStatus[PageNr] == false) {
				ParameterNr = 0;
				drawParamterFrame(PageNr, ParameterNr);
				tft.fillRoundRect(34,20,7,10,2,ST7735_RED);
				tft.setCursor(35,21);
				tft.setTextColor(ST7735_WHITE);
				tft.println(char(65+newWaveBank));
				draw_Waveform(oscWaveformB, ST7735_ORANGE);
				print_quadsaw_pwamt();  // print PWAMT/SPREAD & PWMOD/SAWMIX
				usbMIDI.sendControlChange(15, Osc2WaveBank, channel);
			}
		}
	}
	
	// CC control No: 14
	else if (control == CCoscwaveformA) {
		oscWaveformA = value;
		if (currentWaveformA != oscWaveformA) {
			updateWaveformA();
			currentWaveformA = oscWaveformA;
			if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
				ParameterNr = 0;
				drawParamterFrame(PageNr, ParameterNr);
				tft.fillRoundRect(54,19,22,10,2,ST7735_BLUE);
				tft.setCursor(56,21);
				tft.setTextColor(ST7735_WHITE);
				tft.print(oscWaveformA);
				draw_Waveform(oscWaveformA, ST7735_RED);
				printPWMrate(pwmRateA);
				ParUpdate = true;
				print_quadsaw_pwamt();  // print PWAMT/SPREAD & PWMOD/SAWMIX
				usbMIDI.sendControlChange(21, oscWaveformA, channel);
			}
		}
	}
	
	// CC control No: 15
	else if (control == CCoscwaveformB) {
		oscWaveformB = value;
		if (currentWaveformB != oscWaveformB) {
			updateWaveformB();
			currentWaveformB = oscWaveformB;
			if (PageNr == 2 && myPageShiftStatus[PageNr] == false) {
				ParameterNr = 0;
				drawParamterFrame(PageNr, ParameterNr);
				tft.fillRoundRect(54,19,22,10,2,ST7735_BLUE);
				tft.setCursor(56,21);
				tft.setTextColor(ST7735_WHITE);
				tft.print(oscWaveformB);
				draw_Waveform(oscWaveformB, ST7735_ORANGE);
				printPWMrate(pwmRateB);
				ParUpdate = true;
				print_quadsaw_pwamt();  // print PWAMT/SPREAD & PWMOD/SAWMIX
				usbMIDI.sendControlChange(22, oscWaveformB, channel);
			}
		}
	}
	
	// CC control No: 16
	else if (control == CCfilterenv) {
		if (value >= 126) {
			value = 126;
		}
		filterEnv = (LINEARCENTREZERO[value] * FILTERMODMIXERMAX); //Bipolar
		updateFilterEnv();
		if (PageNr == 3 && myPageShiftStatus[3] < 2) {
			draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
			if (myPageShiftStatus[3] == 0) {
				tft.fillRect(82, 116, 30, 7, ST7735_BLACK);
				print_value_63(value, 90, 116, ST7735_GRAY);
				printRedMarker (2, value);
			}
			usbMIDI.sendControlChange(90, value, channel);
		}
	}
	
	// CC control No: 19
	else if (control == CCfiltermixer) {
		filterMix = LINEAR_FILTERMIXER[value];
		filterMixStr = LINEAR_FILTERMIXERSTR[value];
		updateFilterMixer();
		if (PageNr == 3 && myPageShiftStatus[3] < 2) {
			draw_filter_curves(FilterCut >> 1, FilterRes, value);
			if (myFilter == 1 && myPageShiftStatus[3] == 0) {
				printFilterTyp(value);
			}
		}
	}
	
	// CC control No: 20
	else if (control == CCoscLevelA) {
		oscALevel = LINEAR[value];
		updateOscLevelA();
	}
	
	// CC control No: 21
	else if (control == CCoscLevelB) {
		oscBLevel = LINEAR[value];
		updateOscLevelB();
	}
	
	// CC control No: 23
	else if (control == CCnoiseLevel) {
		if (value >= 62 && value <= 64) {
			value = 63;
		}
		else if (value >= 126) {
			value = 126;
		}
		noiseLevel = LINEARCENTREZERO[value];
		updateNoiseLevel();
		// draw noise level on submenu Osc1+2 submenu
		if ((PageNr == 1 || PageNr == 2) && myPageShiftStatus[PageNr] == true) {
			ParameterNr = 5;
			drawParamterFrame(PageNr, ParameterNr);
			tft.fillRoundRect(133,59,21,8,2,ST7735_BLUE);
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
				tft.fillRect(122,59,5,7,ST7735_BLACK);
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
		}
	}
	
	// CC control No: 24
	else if (control == CCoscfx) {
		uint8_t val = (value * 0.056);
		oscFX = (val <= 7)? val : 0;
		updateOscFX();
	}
	
	// CC control No: 26
	else if (control == CCpitchA) {
		oscPitchA = getPitch(value);
		updatePitchA();
		// draw parameter value on menu page1
		if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
			ParameterNr = 1;
			drawParamterFrame(PageNr, ParameterNr);
			tft.fillRect(54,38,22,10,ST7735_BLUE);
			tft.setCursor(56,40);
			tft.setTextColor(ST7735_WHITE);
			int temp = getPitch(value);
			if (temp > 0) {
				tft.print("+");
			}
			tft.print(temp);
			usbMIDI.sendControlChange(26, value, channel);
		}
	}
	
	// CC control No: 27
	else if (control == CCpitchB) {
		oscPitchB = getPitch(value);
		updatePitchB();
		// draw parameter value on menu page1
		if (PageNr == 2 && myPageShiftStatus[PageNr] == false) {
			ParameterNr = 1;
			drawParamterFrame(PageNr, ParameterNr);
			tft.fillRect(54,38,22,10,ST7735_BLUE);
			tft.setCursor(56,40);
			tft.setTextColor(ST7735_WHITE);
			int temp = getPitch(value);
			if (temp > 0) {
				tft.print("+");
			}
			tft.print(temp);
			usbMIDI.sendControlChange(27, value, channel);
		}
	}
	
	// CC control No: 30
	else if (control == CCosclforetrig) {
		value > 0 ? oscLfoRetrig = 1 : oscLfoRetrig = 0;
		updatePitchLFORetrig();
	}
	
	// CC control No: 31
	else if (control == CCfilterlforetrig) {
		value > 0 ? filterLfoRetrig = 1 : filterLfoRetrig = 0;
		updateFilterLFORetrig();
	}
}

//*************************************************************************
// inter CC 51-100
//*************************************************************************
FLASHMEM void myCCgroup2 (byte control, byte value)
{
	// CC control No: 60
	if (control == CCfilterlforate) {
		//Pick up
		if (!pickUpActive && pickUp && (filterLfoRatePrevValue <  LFOMAXRATE * POWER[value - TOLERANCE] || filterLfoRatePrevValue > LFOMAXRATE * POWER[value + TOLERANCE])) return; //PICK-UP
		if (filterLFOMidiClkSync == 1) {
			filterLfoRate = getLFOTempoRate(value);
			filterLFOTimeDivStr = LFOTEMPOSTR[value];
			} else {
			filterLfoRate = LFOMAXRATE * POWER[value];
		}
		updateFilterLfoRate();
		filterLfoRatePrevValue = filterLfoRate;//PICK-UP
	}
	
	// CC control No: 62
	else if (control == CCHPFfilter) {
		//HPF_filterFreq = HPF_FILTERFREQS256[value];
		HPF_filterFreq_value = value;
		updateHPFFilterFreq();
		if (PageNr == 3 && myPageShiftStatus[3] == 2) {
			value = value >> 1;
			draw_HPF_filter_curves(value, HPF_Res_value);
			printDataValue (0, value);
			printRedMarker (0, value);
			usbMIDI.sendControlChange(62, value, channel);
		}
	}
	
	// CC control No: 63
	else if (control == CCHPFres) {
		//HPFRes = (14.29f * LINEAR[value]) + 0.71f; //If <1.1 there is noise at high cutoff freq
		HPF_Res_value = value;
		updateHPFFilterRes();
		if (PageNr == 3 && myPageShiftStatus[3] == 2) {
			draw_HPF_filter_curves(HPF_filterFreq_value >> 1, HPF_Res_value);
			printDataValue (1, value);
			printRedMarker (1, value);
			usbMIDI.sendControlChange(63, value, channel);
		}
	}
	
	// CC control No: 64
	else if (control == CCsustain) {
		if (value <= 63 && ccSustain == true) {
			ccSustain = false;
			for (uint8_t v = 0; v < NO_OF_VOICES; v++ ) {
				if (voiceSustain[v] == true) {
					voiceSustain[v] = false;
					endVoice(v + 1);
				}
			}
		}
		else if (value >= 64 && ccSustain == false) {
			ccSustain = true;
		}
	}
	
	// CC control No: 74
	else if (control == CCfilterfreq) {
		filterFreq = FILTERFREQS256[value];
		updateFilterSweeb(value);
		FilterCut = value;
		value = value >> 1;
		if (PageNr == 3 && myPageShiftStatus[3] < 2) {
			draw_filter_curves(value, FilterRes, FilterMix);
			if (myPageShiftStatus[3] == 0) {
				printDataValue (0, value);	// Value 0...127
				printRedMarker (0, value);
				cutoffPickupFlag = false;
			}
			usbMIDI.sendControlChange(74, value, channel);
		}
	}
	
	// CC control No: 75
	else if (control == CCfilterres) {
		// State Variable Filter -------------------
		if (myFilter == 1) {
			if (control == CCfilterres) {   // Filter_variable
				//Pick up
				if (!pickUpActive && pickUp && (resonancePrevValue <  ((14.29f * LINEAR[value - TOLERANCE]) + 0.71f) || resonancePrevValue >  ((14.29f * LINEAR[value + TOLERANCE]) + 0.71f))) return; //PICK-UP
				filterRes = (14.29f * LINEAR[value]) + 0.71f; //If <1.1 there is noise at high cutoff freq
				SVF_filterRes = filterRes;
				updateFilterRes();
				FilterRes = value;
				if (PageNr == 3 && myPageShiftStatus[3] < 2) {
					draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
					if (myPageShiftStatus[3] == 0) {
						printDataValue (1, value);
						printRedMarker (1, value);
					}
					usbMIDI.sendControlChange(75, value, channel);
				}
			}
		}
		// Ladder Filter ---------------------------
		else {
			if (!pickUpActive && pickUp && (resonancePrevValue <  (1.8f * LINEAR[value - TOLERANCE]) || resonancePrevValue >  (1.8f * LINEAR[value + TOLERANCE]))) return; //PICK-UP
			filterRes = (1.8f * LINEAR[value]);
			LAD_filterRes = filterRes;
			updateFilterRes();
			resonancePrevValue = filterRes;//PICK-UP
			FilterRes = value;
			if (PageNr == 3 && myPageShiftStatus[3] < 2) {
				draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
				if (myPageShiftStatus[3] == 0) {
					printDataValue (1, value);
					printRedMarker (1, value);
				}
				usbMIDI.sendControlChange(75, value, channel);
			}
		}
	}
	
	// CC control No: 77
	else if (control == CCfilterlfoamt) {
		/*
		//Pick up
		if (!pickUpActive && pickUp && (filterLfoAmtPrevValue <  LINEAR[value - TOLERANCE] * FILTERMODMIXERMAX || filterLfoAmtPrevValue >  LINEAR[value + TOLERANCE] * FILTERMODMIXERMAX)) return; //PICK-UP
		filterLfoAmt = LINEAR[value] * FILTERMODMIXERMAX;
		updateFilterLfoAmt();
		filterLfoAmtPrevValue = filterLfoAmt;//PICK-UP
		*/
	}
	
	
	// CC control No: 76
	else if (control == CCfilterattack) {
		filterAttack = ENVTIMES[value];
		updateFilterAttack();
		updateFilterRelease();	// Realeas needs to be updated when Attack is updated!
		set_Filter_Envelope_releaseNoteOn();
		Env1Atk = value;
		if (PageNr == 4 && myPageShiftStatus[PageNr] == false) {
			drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
			printDataValue (0, value);
			printRedMarker (0, value);
			usbMIDI.sendControlChange(76, value, channel);
		}
	}
	
	// CC control No: 77
	else if (control == CCfilterdecay) {
		filterDecay = ENVTIMES[value];
		updateFilterDecay();
		Env1Dcy = value;
		if (PageNr == 4 && myPageShiftStatus[PageNr] == false) {
			drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
			printDataValue (1, value);
			printRedMarker (1, value);
			usbMIDI.sendControlChange(77, value, channel);
		}
	}
	
	// CC control No: 78
	else if (control == CCfiltersustain) {
		filterSustain = LINEAR[value];
		updateFilterSustain();
		Env1Sus = value;
		if (PageNr == 4 && myPageShiftStatus[PageNr] == false) {
			drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
			printDataValue (2, value);
			printRedMarker (2, value);
			usbMIDI.sendControlChange(78, value, channel);
		}
	}
	
	// CC control No: 79
	else if (control == CCfilterrelease) {
		filterRelease = ENVTIMES[value];
		updateFilterRelease();
		set_Filter_Envelope_releaseNoteOn();
		Env1Rel = value;
		if (PageNr == 4 && myPageShiftStatus[PageNr] == false) {
			drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
			printDataValue (3, value);
			printRedMarker (3, value);
			usbMIDI.sendControlChange(79, value, channel);
		}
	}
	
	// CC control No: 80
	else if (control == CCampattack) {
		ampAttack = ENVTIMES[value];
		updateAttack();
		updateRelease();		// Release needs to be updated when Attack is updated!
		set_Amp_Envelope_releaseNoteOn();
		Env2Atk = value;
		if (PageNr == 5 && myPageShiftStatus[PageNr] == false) {
			drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
			printDataValue (0, value);
			printRedMarker (0, value);
			usbMIDI.sendControlChange(80, value, channel);
		}
	}
	
	// CC control No: 81
	else if (control == CCampdecay) {
		ampDecay = ENVTIMES[value];
		updateDecay();
		Env2Dcy = value;
		if (PageNr == 5 && myPageShiftStatus[PageNr] == false) {
			drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
			printDataValue (1, value);
			printRedMarker (1, value);
			usbMIDI.sendControlChange(81, value, channel);
		}
	}
	
	// CC control No: 82
	else if (control == CCampsustain) {
		ampSustain = LINEAR[value];
		updateSustain();
		Env2Sus = value;
		if (PageNr == 5 && myPageShiftStatus[PageNr] == false) {
			drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
			printDataValue (2, value);
			printRedMarker (2, value);
			usbMIDI.sendControlChange(82, value, channel);
		}
	}
	
	// CC control No: 83
	else if (control == CCamprelease) {
		ampRelease = ENVTIMES[value];
		updateRelease();
		set_Amp_Envelope_releaseNoteOn();
		if (ampRelease < LFO1releaseTime) {
			updateLFO1release();
		}
		Env2Rel = value;
		if (PageNr == 5 && myPageShiftStatus[PageNr] == false) {
			drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
			printDataValue (3, value);
			printRedMarker (3, value);
			usbMIDI.sendControlChange(83, value, channel);
		}
	}
	
	// CC control No: 85
	else if (control == CCpwA) {
		pwA = LINEARCENTREZERO[value]; //Bipolar
		pwmAmtA = LINEAR[value];
		updatePWA();
		if (PageNr == 1 && myPageShiftStatus[1] == 0 && Osc1WaveBank == 0 && (oscWaveformA == 5 || oscWaveformA == 8
		|| oscWaveformA == 12)) {
			ParameterNr = 5;
			drawParamterFrame(PageNr, ParameterNr);
			ParameterNrMem[PageNr] = ParameterNr;
			tft.fillRoundRect(133,57,22,10,2,ST7735_BLUE);
			tft.setCursor(135,59);
			tft.setTextColor(ST7735_WHITE);
			tft.print(value);
			if (Osc1WaveBank == 0 && oscWaveformA != 0 && (oscWaveformA == 5 || oscWaveformA == 12)) {
				if (value >= 126) {
					value = 126;
				}
				draw_PWM_curve(LINEARCENTREZERO[value]);
				printPWMrate (pwmRateA);
			}
		}
	}
	
	// CC control No: 86
	else if (control == CCpwB) {
		pwB = LINEARCENTREZERO[value]; //Bipolar
		pwmAmtB = LINEAR[value];
		updatePWB();
		if (PageNr == 2 && myPageShiftStatus[2] == 0 && Osc2WaveBank == 0 && (oscWaveformB == 5 || oscWaveformB == 8
		|| oscWaveformB == 12)) {
			ParameterNr = 5;
			drawParamterFrame(PageNr, ParameterNr);
			ParameterNrMem[PageNr] = ParameterNr;
			tft.fillRoundRect(133,57,22,10,2,ST7735_BLUE);
			tft.setCursor(135,59);
			tft.setTextColor(ST7735_WHITE);
			tft.print(value);
			if (Osc2WaveBank == 0 && oscWaveformB != 0 && (oscWaveformB == 5 || oscWaveformB == 12)) {
				if (value >= 126) {
					value = 126;
				}
				draw_PWM_curve(LINEARCENTREZERO[value]);
				printPWMrate (pwmRateB);
			}
		}
	}
	
	// CC control No: 89
	else if (control == CCkeytracking) {
		keytrackingAmount = (value * DIV127);
		updateKeyTracking();
		if (PageNr == 3 && myPageShiftStatus[3] == 1) {
			printDataValue (0, value);
			printRedMarker (0, value);
			usbMIDI.sendControlChange(89, value, channel);
		}
	}
	
	// CC control No: 94 (max -0.3% Notefrq)
	else if (control == CCdetune) {
		static uint8_t detuneVal = 0;
		if (detuneVal != value) {
			detune = 1.0f - (MAXDETUNE * POWER[value]);
			chordDetune = value;
			detuneVal = value;
			updateDetune();
			if (PageNr == 2 && myPageShiftStatus[PageNr] == false) {
				ParameterNr = 3;
				drawParamterFrame(PageNr, ParameterNr);
				tft.fillRoundRect(50,76,26,10,2,ST7735_BLACK);
				tft.fillRoundRect(54,76,22,10,2,ST7735_BLUE);
				tft.setCursor(56,78);
				if (unison == 0 || (unison == 1 && Voice_mode <= 4)) {
					tft.setTextColor(ST7735_WHITE);
					tft.print(value);
				}
				else {
					tft.fillRoundRect(50,76,26,10,2,ST7735_BLUE);
					tft.setCursor(52,78);
					tft.print(CDT_STR[value]);
				}
			}
			usbMIDI.sendControlChange(31, value, channel);
		}
	}
}

//*************************************************************************
// inter CC 101-150
//*************************************************************************
FLASHMEM void myCCgroup3 (byte control, byte value)
{
	// CC control No: 102
	if (control == CCoscLfoRate) {
		//Pick up
		if (!pickUpActive && pickUp && (oscLfoRatePrevValue <  LFOMAXRATE * POWER[value - TOLERANCE] || oscLfoRatePrevValue > LFOMAXRATE * POWER[value + TOLERANCE])) return; //PICK-UP
		if (oscLFOMidiClkSync == 1) {
			oscLfoRate = getLFOTempoRate(value);
			oscLFOTimeDivStr = LFOTEMPOSTR[value];
		}
		else {
			oscLfoRate = LFOMAXRATE * POWER[value];
		}
		updatePitchLFORate();
		oscLfoRatePrevValue = oscLfoRate;//PICK-UP
	}
	
	// CC control No: 103
	else if (control == CCoscLfoWaveform) {
		if (oscLFOWaveform == getLFOWaveform(value))return;
		oscLFOWaveform = getLFOWaveform(value);
		updatePitchLFOWaveform();
	}
	
	// CC control No: 104
	else if (control == CCfilterLFOMidiClkSync) {
		value > 0 ? filterLFOMidiClkSync = 1 : filterLFOMidiClkSync = 0;
		updateFilterLFOMidiClkSync();
	}
	
	// CC control No: 105
	else if (control == CCoscLFOMidiClkSync) {
		value > 0 ? oscLFOMidiClkSync = 1 : oscLFOMidiClkSync = 0;
		updatePitchLFOMidiClkSync();
	}
	
	// CC control No: 106
	else if (control == CCpwmSource) {
		value > 0 ? pwmSource = PWMSOURCEFENV : pwmSource = PWMSOURCELFO;
		if (PageNr == 1) {
			updatePWMSourceA();
			} else {
			updatePWMSourceB();
		}
		
	}
	
	// CC control No: 123
	else if (control == CCallnotesoff) {
		allNotesOff();
	}
	
	// CC control No: 126
	else if (control == CCunison) {
		switch (value) {
			case 0:
			unison = 0;
			break;
			case 1:
			unison = 1;
			break;
			default:
			unison = 0;
			break;
		}
		updateUnison();
		KeyLED2State = true;
	}

	// CC control No: 141  LFO1 shape
	else if (control == myLFO1shape) {
		value = value * 0.51f;
		if (value >= 63) {
			value = 63;
		}
		if (oscLFOWaveform != value) {
			oscLFOWaveform = value;
			pitchLfo.arbitraryWaveform(LFOwaveform + (256 * oscLFOWaveform), AWFREQ); // half sine
			updateLFO1waveform();
			if (PageNr == 6) {
				drawLFOwaveform(oscLFOWaveform, ST77XX_RED);
				uint8_t frame_no = value % 6;
				drawLFOframe(frame_no);
				printDataValue (0, (value + 1));
				printRedMarker (0, (value * 2));
			}
		}
	}
	
	// CC control No: 142  LFO1 Rate (Freq. max 25Hz)
	else if (control == myLFO1rate) {
		if (value == 0) {
			value = 1;
		}
		oscLfoRate = LFOMAXRATE * POWER[value];
		updatePitchLFORate();
		myLFO1RateValue = value;
		if (PageNr == 6 && myPageShiftStatus[6] == false) {
			printDataValue (1, value);
			printRedMarker (1, value);
		}
	}
	
	// CC control No: 143  LFO1 amt
	else if (control == myLFO1amt) {
		oscLfoAmt = LINEARLFO[value];
		updateOscLFOAmt();
		if (PageNr == 6 && myPageShiftStatus[6] == false) {
			printDataValue (2, value);
			printRedMarker (2, value);
		}
	}
	
	// CC control No: 144  LFO2 shape
	else if (control == myLFO2shape) {
		value = value * 0.51f;
		if (value >= 63) {
			value = 63;
		}
		if (filterLfoWaveform != value) {
			filterLfoWaveform = value;
			filterLfo.arbitraryWaveform(LFOwaveform + (256 * filterLfoWaveform), AWFREQ); // half sine
			updateLFO2waveform();
			if (PageNr == 7) {
				drawLFOwaveform(filterLfoWaveform, ST7735_ORANGE);
				uint8_t frame_no = value % 6;
				drawLFOframe(frame_no);
				printDataValue (0, (value + 1));
				printRedMarker (0, (value * 2));
			}
		}
	}
	
	// CC control No: 145  LFO2 Rate (freq. max 25Hz)
	else if (control == myLFO2rate) {
		if (value == 0) {
			value = 1;
		}
		filterLfoRate = LFOMAXRATE * POWER[value];
		updateFilterLfoRate();
		myLFO2RateValue = value;
		if (PageNr == 7 && myPageShiftStatus[7] == false) {
			printDataValue (1, value);
			printRedMarker (1, value);
		}
	}
	
	// CC control No: 146 LFO2 amt
	else if (control == myLFO2amt) {
		filterLfoAmt = LINEARLFO[value];
		updateFilterLfoAmt();
		if (PageNr == 7 && myPageShiftStatus[7] == false) {
			printDataValue (2, value);
			printRedMarker (2, value);
			usbMIDI.sendControlChange(51, value, channel);
		}
		// Filter page
		else if (PageNr == 3 && myPageShiftStatus[3] < 2) {
			draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
			if (myPageShiftStatus[3] == 1) {
				printDataValue (2, value);
				printRedMarker (2, value);
			}
			usbMIDI.sendControlChange(51, value, channel);
		}
	}
	
	// CC control No: 150
	else if (control == myOscMix) {
		uint8_t valA = OSCMIXA[value];
		uint8_t valB = OSCMIXB[value];
		oscALevel = LINEAR[valA];
		oscBLevel = LINEAR[valB];
		updateOscLevelA();
		updateOscLevelB();
		if ((PageNr == 1 || PageNr == 2) && myPageShiftStatus[PageNr] == false) {
			ParameterNr = 7;
			drawParamterFrame(PageNr, ParameterNr);
			printOscMixVal(value);
			usbMIDI.sendControlChange(25, value, channel);
		}
	}
}

//*************************************************************************
// inter CC 151-300
//*************************************************************************
FLASHMEM void myCCgroup4 (byte control, byte value)
{
	// CC control No: 151
	if (control == CCpitchenvA) {
		if (value >= 126) {
			value = 126;
		}
		pitchEnvA = LINEARCENTREZERO[value] * OSCMODMIXERMAX;
		updatePitchEnv();
		if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
			ParameterNr = 2;
			drawParamterFrame(PageNr, ParameterNr);
			printOscPitchEnv(value);
			usbMIDI.sendControlChange(28, value, channel);
		}
	}
	
	// CC control No: 152
	else if (control == CCpitchenvB) {
		if (value >= 126) {
			value = 126;
		}
		pitchEnvB = LINEARCENTREZERO[value] * OSCMODMIXERMAX;
		updatePitchEnv();
		if (PageNr == 2 && myPageShiftStatus[PageNr] == false) {
			ParameterNr = 2;
			drawParamterFrame(PageNr, ParameterNr);
			printOscPitchEnv(value);
			usbMIDI.sendControlChange(29, value, channel);
		}
	}
	
	// CC control No: 153
	else if (control == myDrive) {
		driveLevel = ((LINEAR[value]) * 1.25f);
		setAmpLevel(driveLevel);
		if ((PageNr == 1 || PageNr == 2) && myPageShiftStatus[PageNr] == false) {
			ParameterNr = 4;
			drawParamterFrame(PageNr, ParameterNr);
			tft.fillRoundRect(54,95,22,10,2,ST7735_BLUE);
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
			//tft.print(int(DIV100 * value));
			tft.print(value);
			usbMIDI.sendControlChange(24, value, channel);
			
		}
	}
	
	// CC control No: 155
	else if (control == myFilterVelo) {
		myFilVelocity = ((DIV127 * value));
		if (PageNr == 3 && myPageShiftStatus[PageNr] == 1) {
			printDataValue (1, value);
			printRedMarker (1, value);
		}
	}
	
	// CC control No: 156
	else if (control == myAmplifierVelo) {
		myAmpVelocity = ((DIV127 * value) * 0.5f);
		if (PageNr == 5 && myPageShiftStatus[PageNr] == true) {
			printDataValue (1, value);
			printRedMarker (1, value);
		}
		
	}
	
	// CC control No: 160
	else if (control == myWaveshaperTable) {
		uint8_t val = (uint8_t)(value * 0.23f);
		if (val >= 29) {
			val = 29;
		}
		if (WShaperNo != val) {
			WShaperNo = val;
			setWaveShaperTable(val);
			if ((PageNr == 1 || PageNr == 2) && myPageShiftStatus[PageNr] == 1) {
				ParameterNr = 0;
				drawParamterFrame(PageNr, ParameterNr);
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
	}
	
	// CC control No: 161
	else if (control == myWaveshaperDrive) {
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
		if ((PageNr == 1 || PageNr == 2) && myPageShiftStatus[PageNr] == 1) {
			ParameterNr = 1;
			drawParamterFrame(PageNr, ParameterNr);
			tft.fillRect(56,40,17,7,ST7735_BLUE);
			tft.setTextColor(ST7735_WHITE);
			tft.setCursor(56, 40);
			tft.print(val,1);
		}
	}
	
	
	
	// CC control No: 179 Bank select from Midi Keyboard
	else if (control == CCBankSelectMSB) {
		//myBankSelect16(midiChannel, value);
		//Serial.println("BankSelectMSB");
	}
	
	// CC control No: 180 Bank select from Midi Keyboard
	else if (control == CCBankSelectLSB) {
		myBankSelectLSB(midiChannel, value);
	}
	
	// CC control No: 181
	else if (control == CCprgChange) {
		myProgramChange(midiChannel, value);
	}
	
	
	
	// CC control No: 182
	else if (control == CCfxP1 && FxPrgNo >= 1) {
		FxPot1value = value;
		FxPot1Val = value << 1;
		if (PageNr == 10) {
			printFxValFrame(0);
			printFxPOT(0, FxPrgNo);
			printDataValue (1, value);
			printRedMarker (1, value);
			myFxSelValue = 0;
			printFxPotValue(myFxSelValue, value);
		}
	}
	
	// CC control No: 183
	else if (control == CCfxP2  && FxPrgNo >= 1) {
		FxPot2value = value;
		FxPot2Val = value << 1;
		if (PageNr == 10) {
			printFxValFrame(1);
			printFxPOT(1, FxPrgNo);
			printDataValue (1, value);
			printRedMarker (1, value);
			myFxSelValue = 1;
			printFxPotValue(myFxSelValue, value);
		}
	}
	
	// CC control No: 184
	else if (control == CCfxP3  && FxPrgNo >= 1) {
		FxPot3value = value;
		FxPot3Val = value << 1;
		if (PageNr == 10) {
			printFxValFrame(2);
			printFxPOT(2, FxPrgNo);
			printDataValue (1, value);
			printRedMarker (1, value);
			myFxSelValue = 2;
			printFxPotValue(myFxSelValue, value);
		}
	}
	
	
	// CC control No: 185
	else if (control == CCfxCLK  && FxPrgNo >= 1) {
		FxClkRate = (value * 393.8f) + 10000; // intern value max 255 (cc 127)
		//FxClkRate = (value * 196.08f) + 10000;
		analogWriteFrequency (PWM5, FxClkRate);
		if (PageNr == 10) {
			myFxSelValue = 3;
			printFxValFrame(3);
			printFxPOT(3, FxPrgNo);
			printDataValue (1, value);
			printRedMarker (1, value);
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
	
	// CC control No: 186
	else if (control == CCfxMIX) {
		analogWrite(PWM4, value << 1);
		FxMixValue = value;
		if (PageNr == 10) {
			printDataValue (2, value);
			printRedMarker (2, value);
			drawFxGrafic (FxMixValue, FxTimeValue, FxFeedbackValue, FxPot3value);
		}
	}
	
	// CC control No: 187
	else if (control == CCfxPRG) {
		if (value != FxPrgNo && value <= 15) {
			FxPrgNo = value;
			setFxPrg(value);
			FxPrgNoChange = true;	// FxPrg if change in main-loop
			if (PageNr == 10) {
				if (value == 0) {
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
					printRedMarker (3, 0);
					//setLED(1, false);		// Fx LED is change in main-loop
					printFxValFrame(99); // clear green Frame
					tft.fillRect(3,71,155,10,ST7735_BLACK);
					tft.setCursor(75,72);
					tft.setTextColor(ST7735_GRAY);
					tft.print("Time/Feedb.");
					} else {
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
					//setLED(1, true);	// Fx LED is change in main-loop
					printFxValFrame(myFxSelValue); // clear green Frame
					printFxPOT(myFxSelValue, value);
				}
				
			}
		}
	}
	
	// CC control No: 188 Bank select from Midi controller
	/*
	else if (control == myBankSelect) {
	if (PrgChangeSW == true) {
	value = value >> 3;
	if (currentPatchBank != value) {
	currentPatchBank = value;
	recallPatch(patchNo);
	}
	}
	}
	*/
	
	// CC control No: 189 Ladder Filter Drive
	else if (control == CCLadderFilterDrive) {
		if (value <= 1) {
			value = 1;
		}
		LadderFilterDrive = value; // store value
		updateLadderFilterDrive(value);
		printDataValue (3, value);
		printRedMarker (3, value);
	}
	
	// CC control No: 190 Ladder Filter passband Gain
	else if (control == CCLadderFilterPassbandGain) {
		LadderFilterpassbandgain = value; // store value
		updateLadderFilterPassbandGain(value);
		printDataValue (3, value);
		printRedMarker (3, value);
	}
	
	// CC control No: 191 Filter Envelope curve
	else if (control == myFilterEnvCurve) {
	}
	
	// CC control No: 192 Amp Envelope curve
	else if (control == myFilterEnvCurve) {
	}
	
	// CC control No: 193 Filter Typ (SVF/Ladder)
	else if (control == myFilterSwitch) {
		
		uint8_t filter_temp;
		if (value <= 63) {
			filter_temp = 1;
		} else filter_temp = 2;
		
		if (myFilter != filter_temp) {
			myFilter = filter_temp;
			tft.setTextColor(ST7735_GRAY);
			tft.fillRect(130,116,17,8,ST7735_BLACK);
			
			if (myFilter == 1) {
				printRedMarker (3, 0);
				print_String(146,130,116);	// print "STA"
				filterRes = SVF_filterRes;
				int i;
				for (i = 0; i < 40; i++) {
					StateVariableFilter_connections[i].connect();
				}
				for (i = 0; i < 24; i++) {
					ladderFilter_connections[i].disconnect();
				}
			}
			else {
				printRedMarker (3, 127);
				print_String(147,130,116);	// print "LAD"
				filterRes = LAD_filterRes;
				int i;
				for (i = 0; i < 24; i++) {
					ladderFilter_connections[i].connect();
				}
				for (i = 0; i < 40; i++) {
					StateVariableFilter_connections[i].disconnect();
				}
			}
			updateFilterMixer();
			updateFilterRes();
			updateFilterFreq();
		}
	}
	
	// CC control No: 206
	else if (control == CCpwmRateA) {
		pwmRateA = PWMRATE[value];
		updatePWMRateA();	// PWMOD Osc1
		if (PageNr == 1 && myPageShiftStatus[1] == false && Osc1WaveBank == 0 && (oscWaveformA == 5
		|| oscWaveformA == 8 || oscWaveformA == 12)) {
			ParameterNr = 6;
			drawParamterFrame(PageNr, ParameterNr);
			tft.fillRoundRect(133,76,22,10,2,ST7735_BLUE);
			tft.setCursor(135,78);
			tft.setTextColor(ST7735_WHITE);
			if (value <= 2) {
				tft.print("PW");
				draw_Waveform(oscWaveformA, ST7735_RED);
			}
			else if (value <= 6) {
				tft.print("ENV");
				tft.setFont(&Picopixel);
				if (oscWaveformA == 5) {
					tft.fillRoundRect(135,25,17,9,2,ST7735_GRAY);
					tft.setCursor(137,31);
				}
				else if (oscWaveformA == 8) {
					tft.fillRoundRect(135,25,17,9,2,ST7735_GRAY);
					tft.setCursor(137,31);
				}
				else if (oscWaveformA == 12) {
					tft.fillRoundRect(135,25,17,9,2,ST7735_GRAY);
					tft.setCursor(137,31);
				}
				tft.print("ENV");
				tft.setFont(NULL);
			}
			else {
				value = value - 6;
				tft.print(value);
				tft.setFont(&Picopixel);
				tft.fillRoundRect(135,25,17,9,2,ST7735_DARKGREEN);
				tft.setCursor(138,31);
				tft.print("LFO");
				tft.setFont(NULL);
			}
		}
	}
	
	// CC control No: 207
	else if (control == CCpwmRateB) {
		//Uses combination of PWMRate, PWa and PWb
		pwmRateB = PWMRATE[value];
		updatePWMRateB();	// PWMOD Osc2
		if (PageNr == 2 && myPageShiftStatus[2] == false && Osc2WaveBank == 0 && (oscWaveformB == 5
		|| oscWaveformB == 8 || oscWaveformB == 12)) {
			ParameterNr = 6;
			drawParamterFrame(PageNr, ParameterNr);
			tft.fillRoundRect(133,76,22,10,2,ST7735_BLUE);
			tft.setCursor(135,78);
			tft.setTextColor(ST7735_WHITE);
			if (value <= 2) {
				tft.print("PW");
				draw_Waveform(oscWaveformB, ST7735_RED);
			}
			else if (value <= 6) {
				tft.print("ENV");
				tft.setFont(&Picopixel);
				if (oscWaveformB == 5) {
					tft.fillRoundRect(135,25,17,9,2,ST7735_GRAY);
					tft.setCursor(137,31);
				}
				else if (oscWaveformB == 8) {
					tft.fillRoundRect(135,25,17,9,2,ST7735_GRAY);
					tft.setCursor(137,31);
				}
				else if (oscWaveformB == 12) {
					tft.fillRoundRect(135,25,17,9,2,ST7735_GRAY);
					tft.setCursor(137,31);
				}
				tft.print("ENV");
				tft.setFont(NULL);
			}
			else {
				value = value - 6;
				tft.print(value);
				tft.setFont(&Picopixel);
				tft.fillRoundRect(135,25,17,9,2,ST7735_DARKGREEN);
				tft.setCursor(138,31);
				tft.print("LFO");
				tft.setFont(NULL);
			}
		}
	}
	
	// CC control No: 217  LFO3 shape
	else if (control == myLFO3shape) {
		
		
		value = value * 0.51f;
		if (value >= 63) {
			value = 63;
		}
		if (Lfo3Waveform != value) {
			Lfo3Waveform = value;
			ModLfo3.arbitraryWaveform(LFOwaveform + (256 * Lfo3Waveform), AWFREQ); // half sine
			updateLFO3waveform();
			if (PageNr == 8) {
				drawLFOwaveform(Lfo3Waveform, ST7735_MAGENTA);
				uint8_t frame_no = value % 6;
				drawLFOframe(frame_no);
				printDataValue (0, (value + 1));
				printRedMarker (0, (value * 2));
			}
		}
	}
	
	// CC control No: 218  LFO3 freq. max 25Hz
	else if (control == myLFO3rate) {
		if (value == 0) {
			value = 1;
		}
		Lfo3Rate = LFOMAXRATE2 * POWER[value];
		updateLfo3Rate();
		myLFO3RateValue = value;
		if (PageNr == 8 && myPageShiftStatus[8] == false) {
			printDataValue (1, value);
			printRedMarker (1, value);
		}
	}
	
	// CC control No: 219  LFO3 amt
	else if (control == myLFO3amt) {
		Lfo3amt = (DIV127 * value);
		updateLFO3amt();
		if (PageNr == 8 && myPageShiftStatus[8] == false) {
			printDataValue (2, value);
			printRedMarker (2, value);
		}
	}
	
	// CC control No: 230
	else if (control == mySpreadA) {
		SupersawSpreadA = value;
		if (PageNr == 1 && myPageShiftStatus[1] == 0 && Osc1WaveBank == 0 && oscWaveformA == 3) {
			ParameterNr = 5;
			drawParamterFrame(PageNr, ParameterNr);
			ParameterNrMem[PageNr] = ParameterNr;
			tft.fillRoundRect(133,57,22,10,2,ST7735_BLUE);
			tft.setCursor(135,59);
			tft.setTextColor(ST7735_WHITE);
			tft.print(value);
		}
	}
	
	// CC control No: 231
	else if (control == mySpreadB) {
		SupersawSpreadB = value;
		if (PageNr == 2 && myPageShiftStatus[2] == 0 && Osc2WaveBank == 0 && oscWaveformB == 3) {
			ParameterNr = 5;
			drawParamterFrame(PageNr, ParameterNr);
			ParameterNrMem[PageNr] = ParameterNr;
			tft.fillRoundRect(133,57,22,10,2,ST7735_BLUE);
			tft.setCursor(135,59);
			tft.setTextColor(ST7735_WHITE);
			tft.print(value);
		}
	}
	
	// CC control No: 232
	else if (control == mySupersawMixA) {
		SupersawMixA = value;
		int valA = 127 - value;
		int valB = value;
		//Supersaw_gain1A = 0.0075f * valA;
		//Supersaw_gain2A = 0.0016f * valB;
		Supersaw_gain1A = 0.0078f * valA;
		Supersaw_gain2A = 0.0050f * valB;
		if (PageNr == 1 && myPageShiftStatus[1] == 0 && Osc1WaveBank == 0 && oscWaveformA == 3) {
			ParameterNr = 6;
			drawParamterFrame(PageNr, ParameterNr);
			ParameterNrMem[PageNr] = ParameterNr;
			tft.fillRoundRect(133,76,22,10,2,ST7735_BLUE);
			tft.setCursor(135,78);
			tft.setTextColor(ST7735_WHITE);
			tft.print(value);
		}
	}
	
	// CC control No: 233
	else if (control == mySupersawMixB) {
		SupersawMixB = value;
		int valA = 127 - value;
		int valB = value;
		Supersaw_gain1B = 0.0078f * valA;
		Supersaw_gain2B = 0.0050f * valB;
		if (PageNr == 2 && myPageShiftStatus[2] == 0 && Osc2WaveBank == 0 && oscWaveformB == 3) {
			ParameterNr = 6;
			drawParamterFrame(PageNr, ParameterNr);
			ParameterNrMem[PageNr] = ParameterNr;
			tft.fillRoundRect(133,76,22,10,2,ST7735_BLUE);
			tft.setCursor(135,78);
			tft.setTextColor(ST7735_WHITE);
			tft.print(value);
		}
	}
	
	// CC control No: 235
	else if (control == myPan_Value) {
		if (pan_value != value) {
			pan_value = value;
			update_panorama();
		}
		if (PageNr == 5 && myPageShiftStatus[5] == 1) {
			printDataValue (2, value);
			printRedMarker (2, value);
		}
	}
	
}

//*************************************************************************
// intern CC (intern ControlChanges)
//*************************************************************************
FLASHMEM void myControlChange(byte channel, byte control, byte value) {
	
	if (midiChannel == channel || midiChannel == 0) {
		if (control <= 50) {
			myCCgroup1(control, value);
		}
		else if (control <= 100) {
			myCCgroup2(control, value);
		}
		else if (control <= 150) {
			myCCgroup3(control, value);
		}
		else if (control <= 300) {
			myCCgroup4(control, value);
		}
	}
}

//*************************************************************************
// PrgChange and Bank select
//*************************************************************************

FLASHMEM void myProgramChange(byte channel, byte program) {

	// save channel and program
	myPrgChangeChannel = channel;
	myPrgChangeProgram = program;
	myPrgChangeFlag = true;
}

FLASHMEM void myPrgChange() {

	if ((millis() - timer_prgChange) > 980) {
		
		if (PrgChangeSW == true && myPrgChangeFlag == true) {
			if (patchNo != myPrgChangeProgram + 1) {
				patchNo = myPrgChangeProgram + 1;
				
				Keylock = true;
				recallPatch(patchNo);
				Keylock = false;
				mute_before_load_patch();
				currentPatchName = patchName;
				/*
				recallPatch(patchNo);
				storeSoundPatchNo(patchNo);
				storePatchBankNo(currentPatchBank);
				*/
				if (PageNr > 0){
					renderCurrentPatchPage(); // refresh menu page
				}
			}
			myPrgChangeFlag = false;
			RefreshMainScreenFlag = true;
			timer_prgChange = millis();
			
			//Serial.print("PrgChange: ");
			//Serial.println(myPrgChangeProgram);
		}
	}
}

FLASHMEM void myBankSelectLSB(byte cannel, byte bank) {

	if (bank >= 15) {
		bank = 15;
	}
	
	// save channel and program
	myBankSelectChannel = channel;
	myBankSelectBank = bank;
	myBankSelectFlag = true;
	
	//Serial.println("BankSelect LSB");

}

FLASHMEM void myMidiBankSel() {

	if ((millis() - timer_banksel) > 980) {
		
		if (PrgChangeSW == true && myBankSelectFlag == true) {
			if (currentPatchBank != myBankSelectBank) {
				currentPatchBank = myBankSelectBank;
				recallPatch(patchNo);
				storeSoundPatchNo(patchNo);
				storePatchBankNo(currentPatchBank);
				if (PageNr > 0){
					renderCurrentPatchPage(); // refresh menu page
				}
			}
			myBankSelectFlag = false;
			RefreshMainScreenFlag = true;
			timer_banksel = millis();
		}
	}
}

//*************************************************************************
// init Patch data
//*************************************************************************
FLASHMEM void set_initPatchData(void)
{
	allNotesOff();
	
	/* 0 */ patchName = INITPATCHNAME;
	
	if (Keylock == 0) {
		
		currentPatchName = patchName;
		oldPatchName = patchName;
		newPatchName = patchName;
		
		/* 1 */ oscALevel = 1.00f;
		/* 2 */ oscBLevel = 0.00f;
		/* 3 */ noiseLevel = 0.00f;
		/* 4 */ unison = 0;
		/* 5 */ oscFX = 0;
		/* 6 */ detune = 1.00000f;
		/* 7 */ lfoSyncFreq = 1.00f;
		/* 8 */ midiClkTimeInterval = 0.00f;
		/* 9 */ lfoTempoValue = 2.00f;
		/* 10 */ keytrackingAmount = 0.5039f;
		/* 11 */ glideSpeed = 0.00f;
		/* 12 */ oscPitchA = 0;
		/* 13 */ oscPitchB = 0;
		/* 14 */ oscWaveformA = 10;
		/* 15 */ oscWaveformB = 0;
		/* 16 */ pwmSource = 1;
		/* 17 */ pwmAmtA = 0.0f;
		/* 18 */ pwmAmtB = 0.0f;
		/* 19 */ pwmRate = -10.00f;	// old PW Parameter
		/* 20 */ pwA = 0.0f;
		/* 21 */ pwB = 0.0f;
		/* 22 */ filterRes = 0.0f;
		resonancePrevValue = filterRes;//Pick-up
		/* 23 */ filterFreq = 18000.00f;
		//filterfreqPrevValue = filterFreq; //Pick-up
		/* 24 */ filterMix = 0.00f;
		filterMixPrevValue = filterMix; //Pick-up
		/* 25 */ filterEnv = 0.00f;
		/* 26 */ oscLfoAmt = 0.00f;
		oscLfoAmtPrevValue = oscLfoAmt;//PICK-UP
		/* 27 */ oscLfoRate = 5.5f;	// 48
		oscLfoRatePrevValue = oscLfoRate;//PICK-UP
		/* 28 */ oscLFOWaveform = 0;
		/* 29 */ oscLfoRetrig = 0;
		/* 30 */ oscLFOMidiClkSync = 0;
		/* 31 */ filterLfoRate = 1.00f;
		filterLfoRatePrevValue = filterLfoRate;//PICK-UP
		/* 32 */ filterLfoRetrig = 0;
		/* 33 */ filterLFOMidiClkSync = 0;
		/* 34 */ filterLfoAmt = 0.00f;
		filterLfoAmtPrevValue = filterLfoAmt;//PICK-UP
		/* 35 */ filterLfoWaveform = 20;
		/* 36 */ filterAttack = 1.00f;
		/* 37 */ filterDecay = 0.00f;
		/* 38 */ filterSustain = 1.00f;
		/* 39 */ filterRelease = 241.00f;
		/* 40 */ ampAttack = 1.00f;
		/* 41 */ ampDecay = 0.00f;
		/* 42 */ ampSustain = 1.00f;
		/* 43 */ ampRelease = 276.00f;
		/* 44 */ fxAmt = 5.00f;
		fxAmtPrevValue = fxAmt;//PICK-UP
		/* 45 */ fxMix = 0.73f;
		fxMixPrevValue = fxMix;//PICK-UP
		/* 46 */ pitchEnv = 0.00f;
		/* 47 */ velocitySens = 0;
		/* 48 */ chordDetune = 0;
		/* 49 */ FxPot1value = 122;
		/* 50 */ FxPot2value = 110;
		/* 51 */ FxPot3value = 100;
		/* 52 */ FxPrgNo = 0;
		/* 53 */ FxMixValue = 0;
		if (FxPrgNo == 0) {
			FxMixValue = 0;
		}
		/* 54 */ FxClkRate = 32000;
		/* 55 */ Osc1WaveBank = 0;
		/* 56 */ Osc2WaveBank = 0;
		currentOsc1WaveBank = Osc1WaveBank;
		currentOsc2WaveBank = Osc2WaveBank;
		/* 57 */ myBoost = 0;
		/* 58 */ pitchEnvA = 0.00f;
		/* 59 */ pitchEnvB = 0.00f;
		/* 60 */ driveLevel = 0.886f;
		/* 61 */ myFilVelocity = 0.00f;
		/* 62 */ myAmpVelocity = 0.00f;
		/* 63 */ myUnisono = 0;
		/* 64 */  // dummy1
		/* 65 */  // dummy2
		/* 66 */ WShaperNo = 0;
		/* 67 */ WShaperDrive = 1.00f;
		if (WShaperDrive == 0) {
			WShaperDrive = 1.0f;
		}
		
		filterFM = 0.0f;
		filterFM2 = 0.0f;
		LFO1phase = 0;
		LFO2phase = 0;
		oscTranspose = 0;
		oscMasterTune = 1.0f;
		PitchWheelvalue = 127;
		MODWheelvalue = 127;
		LadderFilterDrive = 63;
		envelopeType1 = 0;
		envelopeType2 = 0;
		cutoffPickupFlag = false;
		cutoffScreenFlag = false;
		MODWheelAmt = 0.087f;	// 11
		PitchWheelAmt = 0.27f;	// 34
		myFilter = 1;	// State Variable Filter
		pwmRateA = -10.0f;	// PW default
		pwmRateB = -10.0f;	// PW default
		LFO1delayTime = 0.0f;
		LFO1fadeTime = 0;
		LFO1releaseTime = 0;
		Osc1ModAmt = 0.0f;
		
		oscLFOWaveform = 0;		// LFO1
		filterLfoWaveform = 0;	// LFO2
		Lfo3Waveform = 0;		// LFO3
		LFO1envCurve = 0;
		LFO2envCurve = 0;
		LFO1mode = 0;
		lfo1oneShoot = LFO1mode;
		LFO2mode = 0;
		lfo2oneShoot = LFO2mode;
		LFO3envCurve  = 0;
		LFO3mode = 0;
		lfo3oneShoot = LFO3mode;
		LFO3fadeTime = 0;
		LFO3releaseTime = 0;
		Lfo3amt = 0.0f;
		AtouchAmt = 0;
		AtouchPitchAmt = 0;
		myLFO1RateValue = 58;
		oscLfoRate = LFOMAXRATE2 * POWER[myLFO1RateValue];
		myLFO2RateValue = 58;
		filterLfoRate = LFOMAXRATE2 * POWER[myLFO2RateValue];
		myLFO3RateValue = 58;
		Lfo3Rate = LFOMAXRATE2 * POWER[myLFO3RateValue];
		LFO3phase = 0;
		oscDetuneSync = 1;
		AtouchLFO1Amt = 0;
		AtouchLFO2Amt = 0;
		AtouchLFO3Amt = 0;
		FxPot1amt = 0;
		FxPot2amt = 0;
		FxPot3amt = 0;
		FxCLKamt = 0;
		FxMIXamt = 0;
		AtouchFxP1Amt = 0;
		AtouchFxP2Amt = 0;
		AtouchFxP3Amt = 0;
		AtouchFxClkAmt = 0;
		AtouchFxMixAmt = 0;
		PWMaShape = 0;
		PWMbShape = 0;
		HPF_filterFreq = 20.0f;
		HPF_filterFreq_value = convert_HPF_frq_to_value();
		HPFRes = 0.0f;
		SupersawSpreadA = 0;
		SupersawSpreadB = 0;
		SupersawMixA = 0;
		SupersawMixB = 0;
		UserPot[0] = 1;
		UserPot[1] = 2;
		UserPot[2] = 0;
		UserPot[3] = 0;
		pan_value = 0;
		ModWheelCutoffAmt = 0.0f;
		ModWheelHPFamt = 0.0f;
		last_modwheel_value = 0;
		
		// Update Parameter -------------------------------
		updateUnison();
		updateWaveformA();
		updateWaveformB();
		updatePitchA();
		updatePitchB();
		updateDetune();
		updatePWMSourceA();
		updatePWMSourceB();
		updatePWA();
		updatePWB();
		updatePWMRateA();
		updatePWMRateB();
		updateNoiseLevel();
		updateFilterFreq();
		updateFilterRes();
		// set Filter
		int i;
		if (myFilter < 2) {
			for (i = 0; i < 40; i++) {
				StateVariableFilter_connections[i].connect();
			}
			for (i = 0; i < 24; i++) {
				ladderFilter_connections[i].disconnect();
			}
		}
		else {	// Ladder Filter
			for (i = 0; i < 24; i++) {
				ladderFilter_connections[i].connect();
			}
			for (i = 0; i < 40; i++) {
				StateVariableFilter_connections[i].disconnect();
			}
			updateLadderFilterPassbandGain(LadderFilterpassbandgain);
			updateLadderFilterDrive(LadderFilterDrive);
		}
		updateFilterMixer();
		updateHPFFilterFreq();
		updateHPFFilterRes();
		updateFilterEnv();
		updateKeyTracking();
		updateOscLFOAmt();
		updatePitchLFORate();
		updatePitchLFOWaveform();
		updatePitchLFOMidiClkSync();
		updateFilterLfoRate();
		updateFilterLfoAmt();
		updateFilterLFOWaveform();
		updateFilterLFOMidiClkSync();
		updateFilterLFORetrig();
		updateFilterAttack();
		updateFilterDecay();
		updateFilterSustain();
		updateFilterRelease();
		updateFilterEnvelopeType();
		updateAmpEnvelopeType();
		updateAttack();
		updateDecay();
		updateSustain();
		updateRelease();
		updateLFO1delay();
		updateLFO1fade ();		// LFO1 FADE-IN
		updateLFO1release();	// LFO1 FADE-OUT
		pitchLfo.arbitraryWaveform(LFOwaveform, AWFREQ); // half sine
		updateLFO1waveform();
		filterLfo.arbitraryWaveform(LFOwaveform, AWFREQ); // half sine
		updateLFO2waveform();
		ModLfo3.arbitraryWaveform(LFOwaveform, AWFREQ); // half sine
		updateLFO3waveform();
		updateOverdrive();
		updatePitchEnv();
		updateFxChip();
		updateBoost();
		updateOscVCFMOD();
		setWaveShaperTable(WShaperNo);
		setWaveShaperDrive(WShaperDrive);
		updateOscFX();
		updateFilterFM();
		updateFilterFM2();
		renderCurrentPatchPage();
		if (SEQrunStatus == false) {
			initPatternData();
		}
		KeyLED1State = true;
		KeyLED2State = true;
		KeyLED3State = true;
		KeyLED4State = true;
		update_panorama();
	}
}


//*************************************************************************
// recall (load) Patch
//*************************************************************************
FLASHMEM void recallPatch(int patchNo) {
	
	// get data string
	String numString = (patchNo);
	String bankString = char(currentPatchBank + 65);
	String fileString = (bankString + "/" + numString);
	
	// get Sound File
	File patchFile = SD.open(fileString.c_str());
	// Patch unavailable
	if (!patchFile) {
		patchFile.close();
		set_initPatchData();
		if (SEQrunStatus == false) {
			initPatternData();
		}
	}
	else {
		String data[NO_OF_PARAMS]; //Array of data read in
		recallPatchData(patchFile, data);
		setCurrentPatchData(data);
		patchFile.close();
		
	}
}

//*************************************************************************
// recall2 (load) Patch
//*************************************************************************
FLASHMEM void recallPatch2(int patchNo, int patchBank) {
	
	String numString = (patchNo);
	String bankString = char(patchBank + 65);
	String fileString = (bankString + "/" + numString);
	File patchFile = SD.open(fileString.c_str());
	// Patch unavailable
	if (!patchFile) {
		patchFile.close();
		//Serial.println(F("File not found"));
		// load init Patch from Flash
		set_initPatchData();
		if (SEQrunStatus == false) {
			initPatternData();
		}
	}
	else {
		String data[NO_OF_PARAMS]; //Array of data read in
		recallPatchData(patchFile, data);
		setCurrentPatchData(data);
		patchFile.close();
	}
}

//*************************************************************************
// Update UserPots
//*************************************************************************
FLASHMEM void updateUserPots(void)
{
	// If all values are empty then set devault values
	if (UserPot1 == 0 && UserPot2 == 0 && UserPot3 == 0 && UserPot4 == 0) {
		UserPot1 = 1;	// default VCF Cutoff
		UserPot2 = 2;	// default VCF Resonance
	}
	UserPot[0] = UserPot1;
	UserPot[1] = UserPot2;
	UserPot[2] = UserPot3;
	UserPot[3] = UserPot4;
}

//*************************************************************************
// convert HPF frequency into 8bit value
//*************************************************************************
FLASHMEM uint8_t convert_HPF_frq_to_value() {
	
	int Frq = float(HPF_filterFreq);
	for (int i = 0; i < 256; i++) {
		int frequency = HPF_FILTERFREQS256[i];
		if (Frq <= frequency){
			return i;
		}
	}
	return 0;
}

//*************************************************************************
// convert HPF resonancy into 8bit value
//*************************************************************************
FLASHMEM uint8_t convert_HPF_res_to_value() {
	
	uint8_t hpf_reso;
	
	if (HPFRes >= 0.71) {
		float Reso = ((HPFRes - 0.71f) / 14.29f);
		for (int i = 0; i < 128; i++) {
			float myresonance = LINEAR[i];
			if ((myresonance - Reso) < 0.001f) {
				hpf_reso = 127;
			}
			else {
				hpf_reso = i-1;
				return hpf_reso;
			}
		}
	}
	else return 0;
}

//*************************************************************************
// set Patch data
//*************************************************************************
FLASHMEM void setCurrentPatchData(String data[]) {
	
	// load PatchName
	patchName = data[0];
	currentPatchName = patchName;
	
	// load Sequencer indicator
	SeqSymbol = false;
	for (uint8_t i = 0; i < 16; i++) {
		int dataAddr = 175 + i;
		uint8_t Ncount = data[dataAddr].toInt();
		if (Ncount > 0) {
			SeqSymbol = true;
			break;
		}
	}
	
	// stop loading if only show PatchName and Sequencer indicator
	if (Keylock == true) {
		return;
	}
	
	// load Patch Parameter
	oscALevel = data[1].toFloat();
	oscBLevel = data[2].toFloat();
	noiseLevel = data[3].toFloat();
	unison = data[4].toInt();
	oscFX = data[5].toInt();
	detune = data[6].toFloat();
	lfoSyncFreq = data[7].toInt();
	midiClkTimeInterval = data[8].toInt();
	lfoTempoValue = data[9].toFloat();
	keytrackingAmount = data[10].toFloat();
	glideSpeed = data[11].toFloat();
	oscPitchA = data[12].toFloat();
	oscPitchB = data[13].toFloat();
	oscWaveformA = data[14].toInt();
	oscWaveformB = data[15].toInt();
	pwmSource = data[16].toInt();
	pwmAmtA = data[17].toFloat();
	pwmAmtB = data[18].toFloat();
	pwmRate = -10.00f;	// old Patch Parameter
	pwA = data[20].toFloat();
	pwB = data[21].toFloat();
	filterRes = data[22].toFloat();
	resonancePrevValue = filterRes;//Pick-up
	filterFreq = data[23].toFloat();
	filterfreqPrevValue = filterFreq; //Pick-up on Main-Page
	filterMix = data[24].toFloat();
	filterMixPrevValue = filterMix; //Pick-up
	filterEnv = data[25].toFloat();
	oscLfoAmt = data[26].toFloat();
	oscLfoAmtPrevValue = oscLfoAmt;//PICK-UP
	oscLfoRate = data[27].toFloat();
	oscLfoRatePrevValue = oscLfoRate;//PICK-UP
	// calc LFO1 Rate value
	float LFOfreq = (oscLfoRate / LFOMAXRATE);
	for (int i = 0; i < 128; i++) {
		float lforate = POWER[i];
		if (LFOfreq <= lforate){
			myLFO1RateValue = i;
			break;
		}
	}
	oscLFOWaveform = data[28].toInt();
	oscLfoRetrig = data[29].toInt();
	oscLFOMidiClkSync = data[30].toFloat(); //MIDI CC Only
	//myFilterLFORateValue = data[31].toInt();
	myLFO2RateValue = data[31].toInt();
	filterLfoRatePrevValue = myLFO2RateValue;//PICK-UP
	filterLfoRate = LFOMAXRATE * POWER[myLFO2RateValue];	
	filterLfoRetrig = data[32].toInt();
	filterLFOMidiClkSync = data[33].toInt();
	filterLfoAmt = data[34].toFloat();
	filterLfoAmtPrevValue = filterLfoAmt;//PICK-UP
	if (filterLfoRetrig != 3) {
		filterLfoRate = LFOMAXRATE * POWER[myLFO2RateValue];
	} else filterLfoRate = getLFOTempoRate(myLFO2RateValue);
	filterLfoWaveform = data[35].toInt();
	filterAttack = data[36].toFloat();
	filterDecay = data[37].toFloat();
	filterSustain = data[38].toFloat();
	filterRelease = data[39].toFloat();
	ampAttack = data[40].toFloat();
	ampDecay = data[41].toFloat();
	ampSustain = data[42].toFloat();
	ampRelease = data[43].toFloat();
	fxAmt = data[44].toFloat();
	fxAmtPrevValue = fxAmt;//PICK-UP
	fxMix = data[45].toFloat();
	fxMixPrevValue = fxMix;//PICK-UP
	pitchEnv = data[46].toFloat();
	velocitySens = data[47].toFloat();
	chordDetune = data[48].toInt();
	FxPot1value = data[49].toInt();
	FxPot2value = data[50].toInt();
	FxPot3value = data[51].toInt();
	FxPot1Val = FxPot1value << 1;
	FxPot2Val = FxPot2value << 1;
	FxPot3Val = FxPot3value << 1;
	FxPrgNo = data[52].toInt();
	FxMixValue = data[53].toInt();
	if (FxPrgNo == 0) {
		FxMixValue = 0;
	}
	_FxMixValue = FxMixValue;
	FxClkRate = data[54].toInt();
	Osc1WaveBank = data[55].toInt();
	Osc2WaveBank = data[56].toInt();
	myBoost = data[57].toInt();
	pitchEnvA = data[58].toFloat();
	pitchEnvB = data[59].toFloat();
	driveLevel = data[60].toFloat();
	myFilVelocity = data[61].toFloat();
	myAmpVelocity = data[62].toFloat();
	myUnisono = data[63].toInt();
	dummy1 = data[64].toInt();			// dummy Data
	dummy2 = data[65].toInt();			// dummy Data
	WShaperNo = data[66].toInt();
	WShaperDrive = data[67].toFloat();
	if (WShaperDrive == 0) {
		WShaperDrive = 1.0f;
	}
	LFO1phase = data[68].toInt();
	LFO2phase = data[69].toInt();
	oscDetuneSync = data[107].toInt();
	oscTranspose = data[108].toInt();
	if (oscTranspose > 12) {
		oscTranspose = 0;
	}
	oscMasterTune = data[109].toFloat();
	if (oscMasterTune < 0.9f) {
		oscMasterTune = 1.0f;
	}
	OscVCFMOD = data[110].toFloat();
	if (OscVCFMOD < 0 || OscVCFMOD > 1) {
		OscVCFMOD = 0;
	}
	
	// Sequencer data -----------------------------------------------------

	// if sequencer off check Sequencer notes avaible
	if (SEQrunStatus == false) {
		SeqNotesAvailable = false;
		SeqSymbol = false;
		for (uint8_t i = 0; i < 16; i++) {
			int dataAddr = 175 + i;
			SeqNoteCount[i] = data[dataAddr].toInt();
			if (SeqNoteCount[i] > 0) {
				SeqNotesAvailable = true;
				SeqSymbol = true;
				break;
			}
		}
		// load Sequencer data
		if (SeqNotesAvailable == true) {
			
			for (uint8_t i = 0; i < 16; i++) { // 1.Notes
				int x = 70 + i;
				SeqNote1Buf[i] = data[x].toInt();
			}
			for (uint8_t i = 0; i < 16; i++) {
				int x = 86+i;
				SeqNoteBufStatus[i] = data[x].toInt();
			}
			SEQbpmValue = data[102].toInt();
			SEQdivValue = data[103].toFloat();
			float bpm = (SEQbpmValue / SEQdivValue);
			SEQclkRate = (60000000 / bpm);
			SEQstepNumbers = data[104].toInt();
			SEQselectStepNo = 0;
			SEQGateTime = data[105].toFloat();
			if (SEQGateTime <= 1.0f || SEQGateTime >= 16.0f) {
				SEQGateTime = (3.94f); // Pot Value 32
			}
			Interval = SEQclkRate;
			gateTime = (float)(SEQclkRate / SEQGateTime);
			
			SEQdirection = data[106].toInt();
			SEQdirectionFlag = false;
			
			// Sequencer Velocity data
			for (uint8_t i = 0; i < 16; i++) {
				int dataAddr = i + 111;
				SeqVeloBuf[i] = data[dataAddr].toInt();
			}
			
			// Sequencer Noten Buffer 2.Note, 3.Note, 4.Note
			for (uint8_t i = 0; i < 48; i++) {
				int bufAddr = i + 16;
				int dataAddr = i + 127;
				SeqNote1Buf[bufAddr] = data[dataAddr].toInt();
			}
			// Sequencer Noten counts
			for (uint8_t i = 0; i < 16; i++) {
				int dataAddr = 175 + i;
				SeqNoteCount[i] = data[dataAddr].toInt();
			}
			} else if (SEQrunStatus == false) {
			initPatternData();
		}
		
		// Sequencer function
		SEQmode = data[191].toInt();
		if (SEQmode == 2) {
			SEQmode = 0;  // Key mode
		}
		SEQselectStepNo = 0;
		SEQMidiClkSwitch = 0;//data[192].toInt();
	}
	
	// Ladder Filter --------------------------------------------------
	LadderFilterpassbandgain = data[193].toInt();
	LadderFilterDrive = data[194].toInt();
	if (LadderFilterDrive < 1) {	// old Patches
		LadderFilterDrive = 64;
	}
	
	// EnvelopeTyp ---------------------------------------------------
	envelopeType1 = data[195].toInt();
	envelopeType2 = data[196].toInt();
	
	// other parameters
	cutoffPickupFlag = false;
	cutoffScreenFlag = false;
	RefreshMainScreenFlag = true;
	if (data[197] == "") {
		PitchWheelAmt = 0.27f;
		} else {
		PitchWheelAmt = data[197].toFloat();
	}
	if (data[198] == "") {
		MODWheelAmt = 0.27f;
		} else {
		MODWheelAmt = data[198].toFloat();
	}	
	myFilter = data[199].toInt();
	if (myFilter < 1) {
		myFilter = 1;
	}
	pwmRateA = data[200].toFloat();
	pwmRateB = data[201].toFloat();
	LFO1fadeTime = data[202].toFloat();
	LFO1releaseTime = data[203].toFloat();
	filterFM = data[204].toFloat();
	filterFM2 = data[205].toFloat();
	LFO2fadeTime = data[206].toFloat();
	LFO2releaseTime = data[207].toFloat();
	if (data[208] == "") {
		Osc1ModAmt = 1.0f;
		} else {
		Osc1ModAmt = data[208].toFloat();
	}
	LFO1envCurve = data[209].toInt();
	LFO2envCurve = data[210].toInt();
	LFO1mode = data[211].toInt();
	lfo1oneShoot = LFO1mode;
	LFO2mode = data[212].toInt();
	lfo2oneShoot = LFO2mode;
	LFO3envCurve  = data[213].toInt();
	LFO3mode = data[214].toInt();
	lfo3oneShoot = LFO3mode;
	LFO3fadeTime = data[215].toFloat();
	LFO3releaseTime = data[216].toFloat();
	Lfo3amt = data[217].toFloat();
	AtouchAmt = data[218].toInt();
	AtouchPitchAmt = data[219].toInt();
	Lfo3Waveform = data[220].toInt();
	myLFO3RateValue = data[221].toInt();
	Lfo3Rate = LFOMAXRATE2 * POWER[myLFO3RateValue];
	LFO3phase = data[222].toInt();
	AtouchLFO1Amt = data[223].toInt();
	AtouchLFO2Amt = data[224].toInt();
	AtouchLFO3Amt = data[225].toInt();
	FxPot1amt = data[226].toInt();
	FxPot2amt = data[227].toInt();
	FxPot3amt = data[228].toInt();
	FxCLKamt = data[229].toInt();
	FxMIXamt = data[230].toInt();
	AtouchFxP1Amt = data[231].toInt();
	AtouchFxP2Amt = data[232].toInt();
	AtouchFxP3Amt = data[233].toInt();
	AtouchFxClkAmt = data[234].toInt();
	AtouchFxMixAmt = data[235].toInt();
	PWMaShape = data[236].toInt();
	PWMbShape = data[237].toInt();
	HPF_filterFreq = data[238].toFloat();
	HPF_filterFreq_value = convert_HPF_frq_to_value();
	UserPot1 = data[239].toInt();
	UserPot2 = data[240].toInt();
	UserPot3 = data[241].toInt();
	UserPot4 = data[242].toInt();
	HPFRes = data[243].toFloat();
	HPF_Res_value = convert_HPF_res_to_value();
	SupersawSpreadA = data[244].toInt();
	SupersawSpreadB = data[245].toInt();
	SupersawMixA = data[246].toInt();
	SupersawMixB = data[247].toInt();
	// The new version only has two Unison Modes and 5 Voice_mode
	if (unison > 1) {
		unison = 1;
		Voice_mode = 5;
	} else
	{
		Voice_mode = data[248].toInt();
		if (Voice_mode >= 5) {
			Voice_mode = 5;
		}
	}
	pan_value = data[249].toInt();
	ModWheelCutoffAmt = data[250].toFloat();
	ModWheelHPFamt	 = data[251].toFloat();
	last_modwheel_value = 0;
	ccModwheelHPFamt = 0;
	
	// Update parameter ---------------------------------------------------
	updateOscFX();
	int oscFX_temp = oscFX;
	oscFX = 0;
	updateOscFX();
	oscFX = oscFX_temp;
	updateOscFX();
	setWaveShaperTable(WShaperNo);
	setWaveShaperDrive(WShaperDrive);
	updateFilterEnvelopeType();
	updateAmpEnvelopeType();
	updateFilterEnv();
	updateOverdrive();
	updatePitchEnv();
	updateOscVCFMOD();
	updateUnison();
	updateWaveformA();
	updateWaveformB();
	updatePitchA();
	updatePitchB();
	updateDetune();
	updatePWMSourceA();
	updatePWMSourceB();
	updatePWA();
	updatePWB();
	updatePWMRateA();
	updatePWMRateB();
	updateNoiseLevel();
		
	// calc Cutoff frequency
	int cutoff_value;
	int Frq = float(filterFreq);
	for (int i = 0; i < 256; i++) {
		int frequency = FILTERFREQS256[i];
		if (Frq <= frequency){
			cutoff_value = i;
			break;
		}
	}
	updateFilterSweeb(cutoff_value);
	updateFilterRes();
		
	// set Filter	
	int i;
	if (myFilter < 2) {
		for (i = 0; i < 40; i++) {
			StateVariableFilter_connections[i].connect();
		}
		for (i = 0; i < 24; i++) {
			ladderFilter_connections[i].disconnect();
		}
	}
	else {	// Ladder Filter
		for (i = 0; i < 24; i++) {
			ladderFilter_connections[i].connect();
		}
		for (i = 0; i < 40; i++) {
			StateVariableFilter_connections[i].disconnect();
		}
		updateLadderFilterPassbandGain(LadderFilterpassbandgain);
		updateLadderFilterDrive(LadderFilterDrive);
	}
		
	updateFilterMixer();
	updateHPFFilterFreq();
	updateHPFFilterRes();
	updateKeyTracking();
	updateOscLFOAmt();
	updatePitchLFORate();
	updatePitchLFOWaveform();
	updateFilterLFOWaveform();
	updatePitchLFOMidiClkSync();
	updateFilterLfoRate();
	updateFilterLfoAmt();
	updateFilterLFORetrig();
	init_EnvelopeReleasePhase(); // Envelope.releaseNoteOn(time); 1ms
	updateFilterAttack();
	updateFilterDecay();
	updateFilterSustain();
	updateFilterRelease();
	updateAttack();
	updateDecay();
	updateSustain();
	updateRelease();
	
	//updateLFOsyn();
	updateLFO1delay();
	updateLFO1fade ();		// LFO1 FADE-IN
	updateLFO2release();	// LFO1 FADE-OUT
	updateLFO2fade ();		// LFO2 FADE-IN
	updateLFO1release();	// LFO2 FADE-OUT

	updateLFO1EnvelopeType();
	updateLFO2EnvelopeType();
	updateLfo3Rate();
	updateLFO3delay();
	updateLFO3fade();
	updateLFO3release();
	updateLFO3amt();
	LFO3EnvelopeAmp.setEnvType(LFO3envCurve);
	pitchLfo.arbitraryWaveform(LFOwaveform + (256 * oscLFOWaveform), AWFREQ); // half sine
	updateLFO1waveform();
	filterLfo.arbitraryWaveform(LFOwaveform + (256 * filterLfoWaveform), AWFREQ); // half sine
	updateLFO2waveform();
	ModLfo3.arbitraryWaveform(LFOwaveform + (256 * Lfo3Waveform), AWFREQ); // half sine
	updateLFO3waveform();
	update_PWM_LFO();
	updateAtouchCutoff();
	updateModWheelCutoff();
	updateFilterFM();
	updateFilterFM2();
	updateSupersawMix();
	updateUserPots();
	update_panorama();
	update_MODWHEEL_value(0);
	
	// send Prg.Data to USB
	usbMIDI.sendControlChange(14, Osc1WaveBank, channel);
	usbMIDI.sendControlChange(15, Osc2WaveBank, channel);
	usbMIDI.sendControlChange(21, oscWaveformA, channel);
	usbMIDI.sendControlChange(22, oscWaveformB, channel);
	usbMIDI.sendControlChange(26, oscPitchA + 63, channel);
	usbMIDI.sendControlChange(27, oscPitchB + 63, channel);
	usbMIDI.sendControlChange(28, convert_pitchEnv(pitchEnvA), channel);
	usbMIDI.sendControlChange(29, convert_pitchEnv(pitchEnvB), channel);
	
}

//*************************************************************************
// load Patch data
//*************************************************************************
FLASHMEM String getCurrentPatchData() {
	
	// Parameter
	return patchName
	+ "," + String(oscALevel) + "," + String(oscBLevel) + "," + String(noiseLevel) + "," + String(unison) + "," +
	String(oscFX) + "," + String(detune, 5) + "," + String(lfoSyncFreq) + "," + String(midiClkTimeInterval) + "," +
	String(lfoTempoValue) + "," + String(keytrackingAmount, 4) + "," + String(glideSpeed, 5) + "," + String(oscPitchA) + "," +
	String(oscPitchB) + "," + String(oscWaveformA) + "," + String(oscWaveformB) + "," +String(pwmSource) + "," +
	String(pwmAmtA) + "," + String(pwmAmtB) + "," + String(pwmRate) + "," + String(pwA) + "," + String(pwB) + "," +
	String(filterRes) + "," + String(filterFreq) + "," + String(filterMix) + "," + String(filterEnv) + "," +
	String(oscLfoAmt, 6) + "," + String(oscLfoRate, 5) + "," + String(oscLFOWaveform) + "," + String(oscLfoRetrig) + "," +
	String(oscLFOMidiClkSync) + "," + String(myLFO2RateValue) + "," +	String(filterLfoRetrig) + "," + String(filterLFOMidiClkSync) + "," +
	String(filterLfoAmt,6) + "," + String(filterLfoWaveform) + "," + String(filterAttack) + "," + String(filterDecay) + "," + String(filterSustain) + "," +
	String(filterRelease) + "," + String(ampAttack) + "," + String(ampDecay) + "," + String(ampSustain) + "," + String(ampRelease) + "," + String(fxAmt) + "," +
	String(fxMix) + "," + String(pitchEnv) + "," + String(velocitySens) + "," + String(chordDetune) + "," + String(FxPot1value) + "," +
	String(FxPot2value) + "," + String(FxPot3value) + "," + String(FxPrgNo) + "," + String(FxMixValue) + "," + String(FxClkRate) + "," +
	String(Osc1WaveBank) + "," + String(Osc2WaveBank) + "," + String(myBoost) + "," + String(pitchEnvA, 5) + "," +
	String(pitchEnvB, 5) + "," + String(driveLevel, 3) + "," + String(myFilVelocity, 4) + "," + String(myAmpVelocity, 4) + "," +
	String(myUnisono) + "," + String(dummy1) + "," + String(dummy2) + "," + String(WShaperNo) + "," +
	String(WShaperDrive,3) + "," + String(LFO1phase) + "," + String(LFO2phase)
	// Sequencer Pattern
	+ "," + String(SeqNote1Buf[0]) + "," + String(SeqNote1Buf[1]) + "," + String(SeqNote1Buf[2]) + "," + String(SeqNote1Buf[3])
	+ "," + String(SeqNote1Buf[4]) + "," + String(SeqNote1Buf[5]) + "," + String(SeqNote1Buf[6]) + "," + String(SeqNote1Buf[7])
	+ "," + String(SeqNote1Buf[8]) + "," + String(SeqNote1Buf[9]) + "," + String(SeqNote1Buf[10]) + "," + String(SeqNote1Buf[11])
	+ "," + String(SeqNote1Buf[12]) + "," + String(SeqNote1Buf[13]) + "," + String(SeqNote1Buf[14]) + "," + String(SeqNote1Buf[15])
	+ "," + String(SeqNoteBufStatus[0]) + "," + String(SeqNoteBufStatus[1])	+ "," + String(SeqNoteBufStatus[2]) + "," + String(SeqNoteBufStatus[3])
	+ "," + String(SeqNoteBufStatus[4]) + "," + String(SeqNoteBufStatus[5])	+ "," + String(SeqNoteBufStatus[6]) + "," + String(SeqNoteBufStatus[7])
	+ "," + String(SeqNoteBufStatus[8]) + "," + String(SeqNoteBufStatus[9])	+ "," + String(SeqNoteBufStatus[10]) + "," + String(SeqNoteBufStatus[11])
	+ "," + String(SeqNoteBufStatus[12]) + "," + String(SeqNoteBufStatus[13])	+ "," + String(SeqNoteBufStatus[14]) + "," + String(SeqNoteBufStatus[15])
	+ "," + String(SEQbpmValue) + "," + String(SEQdivValue,8) + "," + String(SEQstepNumbers) + "," + String(SEQGateTime,6)
	+ "," + String(SEQdirection)  + "," + String(oscDetuneSync)  + "," + String(oscTranspose)  + "," + String(oscMasterTune,6)
	+ "," + String(OscVCFMOD,6)
	+ "," + String(SeqVeloBuf[0]) + "," + String(SeqVeloBuf[1]) + "," + String(SeqVeloBuf[2]) + "," + String(SeqVeloBuf[3]) + "," + String(SeqVeloBuf[4])
	+ "," + String(SeqVeloBuf[5]) + "," + String(SeqVeloBuf[6]) + "," + String(SeqVeloBuf[7]) + "," + String(SeqVeloBuf[8]) + "," + String(SeqVeloBuf[9])
	+ "," + String(SeqVeloBuf[10]) + "," + String(SeqVeloBuf[11]) + "," + String(SeqVeloBuf[12]) + "," + String(SeqVeloBuf[13]) + "," + String(SeqVeloBuf[14])
	+ "," + String(SeqVeloBuf[15])
	+ "," + String(SeqNote1Buf[16]) + "," + String(SeqNote1Buf[17]) + "," + String(SeqNote1Buf[18]) + "," + String(SeqNote1Buf[19])
	+ "," + String(SeqNote1Buf[20]) + "," + String(SeqNote1Buf[21]) + "," + String(SeqNote1Buf[22]) + "," + String(SeqNote1Buf[23])
	+ "," + String(SeqNote1Buf[24]) + "," + String(SeqNote1Buf[25]) + "," + String(SeqNote1Buf[26]) + "," + String(SeqNote1Buf[27])
	+ "," + String(SeqNote1Buf[28]) + "," + String(SeqNote1Buf[29]) + "," + String(SeqNote1Buf[30]) + "," + String(SeqNote1Buf[31])
	+ "," + String(SeqNote1Buf[32]) + "," + String(SeqNote1Buf[33]) + "," + String(SeqNote1Buf[34]) + "," + String(SeqNote1Buf[35])
	+ "," + String(SeqNote1Buf[36]) + "," + String(SeqNote1Buf[37]) + "," + String(SeqNote1Buf[38]) + "," + String(SeqNote1Buf[39])
	+ "," + String(SeqNote1Buf[40]) + "," + String(SeqNote1Buf[41]) + "," + String(SeqNote1Buf[42]) + "," + String(SeqNote1Buf[43])
	+ "," + String(SeqNote1Buf[44]) + "," + String(SeqNote1Buf[45]) + "," + String(SeqNote1Buf[46]) + "," + String(SeqNote1Buf[47])
	+ "," + String(SeqNote1Buf[48]) + "," + String(SeqNote1Buf[49]) + "," + String(SeqNote1Buf[50]) + "," + String(SeqNote1Buf[51])
	+ "," + String(SeqNote1Buf[52]) + "," + String(SeqNote1Buf[53]) + "," + String(SeqNote1Buf[54]) + "," + String(SeqNote1Buf[55])
	+ "," + String(SeqNote1Buf[56]) + "," + String(SeqNote1Buf[57]) + "," + String(SeqNote1Buf[58]) + "," + String(SeqNote1Buf[59])
	+ "," + String(SeqNote1Buf[60]) + "," + String(SeqNote1Buf[61]) + "," + String(SeqNote1Buf[62]) + "," + String(SeqNote1Buf[63])
	+ "," + String(SeqNoteCount[0]) + "," + String(SeqNoteCount[1]) + "," + String(SeqNoteCount[2]) + "," + String(SeqNoteCount[3])
	+ "," + String(SeqNoteCount[4]) + "," + String(SeqNoteCount[5]) + "," + String(SeqNoteCount[6]) + "," + String(SeqNoteCount[7])
	+ "," + String(SeqNoteCount[8]) + "," + String(SeqNoteCount[9]) + "," + String(SeqNoteCount[10]) + "," + String(SeqNoteCount[11])
	+ "," + String(SeqNoteCount[12]) + "," + String(SeqNoteCount[13]) + "," + String(SeqNoteCount[14]) + "," + String(SeqNoteCount[15])
	+ "," + String(SEQmode) + "," + String(SEQMidiClkSwitch)
	// Parameter
	+ "," + String(LadderFilterpassbandgain) + "," + String(LadderFilterDrive) + "," + String(envelopeType1) + "," + String(envelopeType2)
	+ "," + String(PitchWheelAmt,6) + "," + String(MODWheelAmt,6) + "," + String(myFilter) + "," + String(pwmRateA,6) + "," + String(pwmRateB,6)
	+ "," + String(LFO1fadeTime) + "," + String(LFO1releaseTime)+ "," + String(filterFM,6) + "," + String(filterFM2,6) + "," + String(LFO2fadeTime)
	+ "," + String(LFO2releaseTime) + "," + String(Osc1ModAmt,6) + "," + String(LFO1envCurve) + "," + String(LFO2envCurve)
	+ "," + String(LFO1mode) + "," + String(LFO2mode)  + "," + String(LFO3envCurve) + "," + String(LFO3mode) + "," + String(LFO3fadeTime)
	+ "," + String(LFO3releaseTime) + "," + String(Lfo3amt,6) + "," + String(AtouchAmt) + "," + String(AtouchPitchAmt) + "," + String(Lfo3Waveform)
	+ "," + String(myLFO3RateValue) + "," + String(LFO3phase) + "," + String(AtouchLFO1Amt) + "," + String(AtouchLFO2Amt) + "," + String(AtouchLFO3Amt)
	+ "," + String(FxPot1amt) + "," + String(FxPot2amt) + "," + String(FxPot3amt) + "," + String(FxCLKamt) + "," + String(FxMIXamt)
	+ "," + String(AtouchFxP1Amt) + "," + String(AtouchFxP2Amt) + "," + String(AtouchFxP3Amt) + "," + String(AtouchFxClkAmt) + "," + String(AtouchFxMixAmt)
	+ "," + String(PWMaShape) + "," + String(PWMbShape) + "," + String(HPF_filterFreq) + "," + String(UserPot1) + "," + String(UserPot2)
	+ "," + String(UserPot3) + "," + String(UserPot4) + "," + String(HPFRes) + "," + String(SupersawSpreadA) + "," + String(SupersawSpreadB)
	+ "," + String(SupersawMixA) + "," + String(SupersawMixB) + "," + String(Voice_mode) + "," + String(pan_value) + "," + String(ModWheelCutoffAmt,6)
	+ "," + String(ModWheelHPFamt,6);
}
//************************************************************************
// load Sequencer Patch data
//************************************************************************
FLASHMEM String getCurrentPatternData() {
	
	return patternName
	+ "," + String(SeqNote1Buf[0]) + "," + String(SeqNote1Buf[1]) + "," + String(SeqNote1Buf[2]) + "," + String(SeqNote1Buf[3])
	+ "," + String(SeqNote1Buf[4]) + "," + String(SeqNote1Buf[5]) + "," + String(SeqNote1Buf[6]) + "," + String(SeqNote1Buf[7])
	+ "," + String(SeqNote1Buf[8]) + "," + String(SeqNote1Buf[9]) + "," + String(SeqNote1Buf[10]) + "," + String(SeqNote1Buf[11])
	+ "," + String(SeqNote1Buf[12]) + "," + String(SeqNote1Buf[13]) + "," + String(SeqNote1Buf[14]) + "," + String(SeqNote1Buf[15])
	+ "," + String(SeqNoteBufStatus[0]) + "," + String(SeqNoteBufStatus[1])	+ "," + String(SeqNoteBufStatus[2]) + "," + String(SeqNoteBufStatus[3])
	+ "," + String(SeqNoteBufStatus[4]) + "," + String(SeqNoteBufStatus[5])	+ "," + String(SeqNoteBufStatus[6]) + "," + String(SeqNoteBufStatus[7])
	+ "," + String(SeqNoteBufStatus[8]) + "," + String(SeqNoteBufStatus[9])	+ "," + String(SeqNoteBufStatus[10]) + "," + String(SeqNoteBufStatus[11])
	+ "," + String(SeqNoteBufStatus[12]) + "," + String(SeqNoteBufStatus[13])	+ "," + String(SeqNoteBufStatus[14]) + "," + String(SeqNoteBufStatus[15])
	+ "," + String(SEQbpmValue) + "," + String(SEQdivValue,8) + "," + String(SEQstepNumbers) + "," + String(SEQGateTime,6)
	+ "," + String(SEQdirection)
	+ "," + String(SeqVeloBuf[0]) + "," + String(SeqVeloBuf[1]) + "," + String(SeqVeloBuf[2]) + "," + String(SeqVeloBuf[3]) + "," + String(SeqVeloBuf[4])
	+ "," + String(SeqVeloBuf[5]) + "," + String(SeqVeloBuf[6]) + "," + String(SeqVeloBuf[7]) + "," + String(SeqVeloBuf[8]) + "," + String(SeqVeloBuf[9])
	+ "," + String(SeqVeloBuf[10]) + "," + String(SeqVeloBuf[11]) + "," + String(SeqVeloBuf[12]) + "," + String(SeqVeloBuf[13]) + "," + String(SeqVeloBuf[14])
	+ "," + String(SeqVeloBuf[15])
	+ "," + String(SeqNote1Buf[16]) + "," + String(SeqNote1Buf[17]) + "," + String(SeqNote1Buf[18]) + "," + String(SeqNote1Buf[19])
	+ "," + String(SeqNote1Buf[20]) + "," + String(SeqNote1Buf[21]) + "," + String(SeqNote1Buf[22]) + "," + String(SeqNote1Buf[23])
	+ "," + String(SeqNote1Buf[24]) + "," + String(SeqNote1Buf[25]) + "," + String(SeqNote1Buf[26]) + "," + String(SeqNote1Buf[27])
	+ "," + String(SeqNote1Buf[28]) + "," + String(SeqNote1Buf[29]) + "," + String(SeqNote1Buf[30]) + "," + String(SeqNote1Buf[31])
	+ "," + String(SeqNote1Buf[32]) + "," + String(SeqNote1Buf[33]) + "," + String(SeqNote1Buf[34]) + "," + String(SeqNote1Buf[35])
	+ "," + String(SeqNote1Buf[36]) + "," + String(SeqNote1Buf[37]) + "," + String(SeqNote1Buf[38]) + "," + String(SeqNote1Buf[39])
	+ "," + String(SeqNote1Buf[40]) + "," + String(SeqNote1Buf[41]) + "," + String(SeqNote1Buf[42]) + "," + String(SeqNote1Buf[43])
	+ "," + String(SeqNote1Buf[44]) + "," + String(SeqNote1Buf[45]) + "," + String(SeqNote1Buf[46]) + "," + String(SeqNote1Buf[47])
	+ "," + String(SeqNote1Buf[48]) + "," + String(SeqNote1Buf[49]) + "," + String(SeqNote1Buf[50]) + "," + String(SeqNote1Buf[51])
	+ "," + String(SeqNote1Buf[52]) + "," + String(SeqNote1Buf[53]) + "," + String(SeqNote1Buf[54]) + "," + String(SeqNote1Buf[55])
	+ "," + String(SeqNote1Buf[56]) + "," + String(SeqNote1Buf[57]) + "," + String(SeqNote1Buf[58]) + "," + String(SeqNote1Buf[59])
	+ "," + String(SeqNote1Buf[60]) + "," + String(SeqNote1Buf[61]) + "," + String(SeqNote1Buf[62]) + "," + String(SeqNote1Buf[63])
	+ "," + String(SeqNoteCount[0]) + "," + String(SeqNoteCount[1]) + "," + String(SeqNoteCount[2]) + "," + String(SeqNoteCount[3])
	+ "," + String(SeqNoteCount[4]) + "," + String(SeqNoteCount[5]) + "," + String(SeqNoteCount[6]) + "," + String(SeqNoteCount[7])
	+ "," + String(SeqNoteCount[8]) + "," + String(SeqNoteCount[9]) + "," + String(SeqNoteCount[10]) + "," + String(SeqNoteCount[11])
	+ "," + String(SeqNoteCount[12]) + "," + String(SeqNoteCount[13]) + "," + String(SeqNoteCount[14]) + "," + String(SeqNoteCount[15]);
	
}



//*************************************************************************
// update sysex Patch
//*************************************************************************
FLASHMEM void updateSysExPatch(void)
{
	setWaveShaperTable(WShaperNo);
	setWaveShaperDrive(WShaperDrive);
	updateFilterEnvelopeType();
	updateAmpEnvelopeType();
	updateFilterEnv();
	updateOverdrive();
	updatePitchEnv();
	updateFxChip();
	updateBoost();
	updateOscVCFMOD();
	updateUnison();
	updateWaveformA();
	updateWaveformB();
	updatePitchA();
	updatePitchB();
	updateDetune();
	updatePWMSourceA();
	updatePWMSourceB();
	updatePWA();
	updatePWB();
	updatePWMRateA();
	updatePWMRateB();
	updateNoiseLevel();
	updateFilterFreq();
	updateFilterRes();
	if (myFilter == 2) {		// Ladder Filter
		updateLadderFilterPassbandGain(LadderFilterpassbandgain);
		updateLadderFilterDrive(LadderFilterDrive);
	}
	updateFilterMixer();
	updateKeyTracking();
	updateOscLFOAmt();
	updatePitchLFORate();
	updatePitchLFOWaveform();
	updateFilterLFOWaveform();
	updatePitchLFOMidiClkSync();
	updateFilterLfoRate();
	updateFilterLfoAmt();
	updateFilterLFORetrig();
	updateFilterAttack();
	updateFilterDecay();
	updateFilterSustain();
	updateFilterRelease();
	updateAttack();
	updateDecay();
	updateSustain();
	updateRelease();
	updateLFO1delay();
	updateLFO1fade ();		// LFO1 FADE-IN
	updateLFO2release();	// LFO1 FADE-OUT
	updateLFO2fade ();		// LFO2 FADE-IN
	updateLFO1release();	// LFO2 FADE-OUT
	updateLFO1EnvelopeType();
	updateLFO2EnvelopeType();
	updateFilterFM();
	updateFilterFM2();
	updateOscFX();
	int oscFX_temp = oscFX;
	oscFX = 0;
	updateOscFX();
	oscFX = oscFX_temp;
	updateOscFX();
	KeyLED1State = true;
	KeyLED2State = true;
	KeyLED3State = true;
	KeyLED4State = true;
	
}
//*************************************************************************
// seleced Paramter
//*************************************************************************
FLASHMEM int selecdParameter(uint8_t PageNr, uint8_t ParameterNr)
{
	int parameter = 0;
	
	if (PageNr == 1){
		
		// Osc1 main page
		if (myPageShiftStatus[PageNr] == false) {	// Osc1 main menu
			switch (ParameterNr){
				case 0:
				parameter = CCoscwaveformA;
				break;
				case 1:
				parameter = CCpitchA;
				break;
				case 2:
				parameter = CCpitchenvA;
				break;
				case 3:
				parameter = CCglide;
				break;
				case 4:
				parameter = myDrive;			// LEVEL
				break;
				case 5:
				if (oscWaveformA == 3 && Osc1WaveBank == 0) {
					parameter = mySpreadA;		// Supersaw spread
					} else if (Osc1WaveBank == 0 && (oscWaveformA == 5 || oscWaveformA == 8 || oscWaveformA == 12)) {
					parameter = CCpwA;			// PWAMT
				}
				break;
				case 6:
				if (oscWaveformA == 3 && Osc1WaveBank == 0) {
					parameter = mySupersawMixA;	// SupersawMix
					}  else if (Osc1WaveBank == 0 && (oscWaveformA == 5 || oscWaveformA == 8 || oscWaveformA == 12)) {
					parameter = CCpwmRateA;		// PWMOD
				}
				break;
				case 7:
				parameter = myOscMix;
				break;
			}
		}
		// Osc1 sub page
		else
		{
			switch (ParameterNr){
				case 0:
				parameter = myWaveshaperTable;
				break;
				case 1:
				parameter = myWaveshaperDrive;
				break;
				case 2:
				parameter = myOscSync;
				break;
				case 3:
				parameter = myTranspose;
				break;
				case 4:
				parameter = myMasterTune;
				break;
				case 5:
				parameter = CCnoiseLevel;
				break;
				case 6:
				parameter = CCoscfx;
				break;
				case 7:
				parameter = myOsc1ModAmt;
				break;
			}
		}
	}
	
	// Osc2 main page
	else if (PageNr == 2){
		if (myPageShiftStatus[PageNr] == false) {	// shift key disabled
			switch (ParameterNr){
				case 0:
				parameter = CCoscwaveformB;
				break;
				case 1:
				parameter = CCpitchB;
				break;
				case 2:
				parameter = CCpitchenvB;
				break;
				case 3:
				parameter = CCdetune;
				break;
				case 4:
				parameter = myDrive;
				break;
				case 5:
				if (oscWaveformB == 3 && Osc2WaveBank == 0) {
					parameter = mySpreadB;		// Supersaw spread
					}  else if (Osc2WaveBank == 0 && (oscWaveformB == 5 || oscWaveformB == 8 || oscWaveformB == 12)) {
					parameter = CCpwB;			// PWAMT
				}
				break;
				case 6:
				if (oscWaveformB == 3 && Osc2WaveBank == 0) {
					parameter = mySupersawMixB;	// SupersawMix
					}   else if (Osc2WaveBank == 0 && (oscWaveformB == 5 || oscWaveformB == 8 || oscWaveformB == 12)) {
					parameter = CCpwmRateB;		// PWMOD
				}
				break;
				case 7:
				parameter = myOscMix;
				break;
			}
		}
		// Osc2 sub page
		else
		{
			switch (ParameterNr){
				case 0:
				parameter = myWaveshaperTable;
				break;
				case 1:
				parameter = myWaveshaperDrive;
				break;
				case 2:
				parameter = myOscSync;
				break;
				case 3:
				parameter = myTranspose;
				break;
				case 4:
				parameter = myMasterTune;
				break;
				case 5:
				parameter = CCnoiseLevel;
				break;
				case 6:
				parameter = CCoscfx;
				break;
				case 7:
				parameter = myOsc1ModAmt;
				break;
			}
		}
	}
	
	// System main page
	else if (PageNr == 12 && myPageShiftStatus[PageNr] == false){
		switch (ParameterNr){
			case 0:
			parameter = myMidiCha;
			break;
			case 1:
			parameter = myVelocity;
			break;
			case 2:
			parameter = myPickup;
			break;
			case 3:
			parameter = myMidiSyncSwitch;
			break;
			case 4:
			parameter = myPRGChange;
			break;
			case 5:
			parameter = myUserPot1;
			break;
			case 6:
			parameter = myUserPot2;
			break;
			case 7:
			parameter = myUserPot3;
			break;
			case 8:
			parameter = myUserPot4;
			break;
			case 9:
			parameter = myVoiceMode; // 234
			break;
		}
	}
	
	// System sub page
	else if (PageNr == 12 && myPageShiftStatus[PageNr] == true){
		switch (ParameterNr){
			case 0:
			parameter = mysysexDest;
			break;
			case 1:
			parameter = mysysexTyp;
			break;
			case 2:
			parameter = mysysexBank;
			break;
			case 3:
			parameter = mysysexSource;
			break;
			case 4:
			parameter = mysysexDump;
			break;
		}
	}

	return parameter;
}

//*************************************************************************
// set pick up parameter
//*************************************************************************
FLASHMEM void Set_parameters_1(uint8_t pot, uint8_t midiChannel, uint8_t myCC, int value)
{
	pot -= 1;
	int value_2 = 0;
	boolean temp_pickupFlag = pickupFlag;
	
	if (PageNr == 0) {
		pickupFlag = true;
	}
	
	
	// change max value
	if (myCC == CCfilterfreq || myCC == CCHPFfilter) {
		value_2 = value / 15.75f;		// max 255
		if (value_2 >= 255) {
			value_2 = 255;
		}
	}
	else {
		//value_2 = value >> 5;			// max 127
		value_2 = value / 31.6f;		// max 127
		if (value_2 >= 127) {
			value_2 = 127;
		}
	}
	
	//value >>= 5;						// Pickup max 127
	value = value / 31.6f;				// Pickup max 127
	if (value >= 127) {
		value = 127;
	}
	
	
	if (PrevFlag[pot] == false && value >= (PrevValue[pot] - Prev_Factor) && value <= (PrevValue[pot] + Prev_Factor)) {
		PrevFlag[pot] = true;
		PrevValue[pot] = value;
	}
	else if (PrevFlag[pot] == true) {
		myControlChange(midiChannel, myCC, value_2);
	}
	
	if (myCC == CCfilterfreq || myCC == CCfilterres || myCC == CCLadderFilterDrive || myCC == CCkeytracking || myCC == myFilterVelo
	|| myCC == myLFO1amt || myCC == myLFO2amt || myCC == myLFO3amt || myCC == myLFO1rate || myCC == myLFO2rate || myCC == myLFO3rate
	|| myCC == CCHPFfilter || myCC == CCHPFres || myCC == CCLadderFilterPassbandGain || myCC == myAmplifierVelo){
		printPickupValue(pot, PrevValue[pot], value, PrevFlag[pot]);}
		
	else if (myCC == myFilterEnvCurve || myCC == myAmpEnvCurve){
		printPickupInt8(pot, PrevValue[pot], value, PrevFlag[pot]);}
	else if (myCC == CCfilterenv){
		printPickupInt64(pot, PrevValue[pot], value, PrevFlag[pot]);}
	else if (myCC == CCfiltermixer){
		printPickupVCFTyp(pot, PrevValue[pot], value, PrevFlag[pot]);}

	pickupFlag = temp_pickupFlag;
}


//*************************************************************************
// Set parameters in the menu
//*************************************************************************
FLASHMEM void set_menu_parameter (uint8_t index, int value)
{
	switch (PageNr)
	{
		// PageNo 0: Main page --------------------------------------------
		case 0:
		// Main Menu
		if (myPageShiftStatus[PageNr] == false) {
			if (index == 4) {		// Pot 4 : Bank
				renderCurrentParameter(PageNr,setPatchBank,(value >> 8));
			}
		}
		// Main Sub Menu
		else {
			if (index == 1) {		// Pot 1 : Parameter
				if (UserPot[0] == 1) {
					Set_parameters_1(index, midiChannel, CCfilterfreq, value);
				}
				else if (UserPot[0] == 2) {
					Set_parameters_1(index, midiChannel, CCfilterres, value);
				}
				else if (UserPot[0] == 3) {
					Set_parameters_1(index, midiChannel, myLFO1amt, value);
				}
				else if (UserPot[0] == 4) {
					Set_parameters_1(index, midiChannel, myLFO2amt, value);
				}
				else if (UserPot[0] == 5) {
					Set_parameters_1(index, midiChannel, myLFO3amt, value);
				}
				else if (UserPot[0] == 6) {
					Set_parameters_1(index, midiChannel, myLFO1rate, value);
				}
				else if (UserPot[0] == 7) {
					Set_parameters_1(index, midiChannel, myLFO2rate, value);
				}
				else if (UserPot[0] == 8) {
					Set_parameters_1(index, midiChannel, myLFO3rate, value);
				}
				else if (UserPot[0] == 9) {
					Set_parameters_1(index, midiChannel, CCHPFfilter, value);
				}
				else if (UserPot[0] == 10) {
					Set_parameters_1(index, midiChannel, CCHPFres, value);
				}
			}
			else if (index == 2) {	// Pot 2 : Parameter
				
				if (UserPot[1] == 1) {
					Set_parameters_1(index, midiChannel, CCfilterfreq, value);
				}
				else if (UserPot[1] == 2) {
					Set_parameters_1(index, midiChannel, CCfilterres, value);
				}
				else if (UserPot[1] == 3) {
					Set_parameters_1(index, midiChannel, myLFO1amt, value);
				}
				else if (UserPot[1] == 4) {
					Set_parameters_1(index, midiChannel, myLFO2amt, value);
				}
				else if (UserPot[1] == 5) {
					Set_parameters_1(index, midiChannel, myLFO3amt, value);
				}
				else if (UserPot[1] == 6) {
					Set_parameters_1(index, midiChannel, myLFO1rate, value);
				}
				else if (UserPot[1] == 7) {
					Set_parameters_1(index, midiChannel, myLFO2rate, value);
				}
				else if (UserPot[1] == 8) {
					Set_parameters_1(index, midiChannel, myLFO3rate, value);
				}
				else if (UserPot[1] == 9) {
					Set_parameters_1(index, midiChannel, CCHPFfilter, value);
				}
				else if (UserPot[1] == 10) {
					Set_parameters_1(index, midiChannel, CCHPFres, value);
				}
			}
			else if (index == 3) {	// Pot 3 : Parameter
				
				if (UserPot[2] == 1) {
					Set_parameters_1(index, midiChannel, CCfilterfreq, value);
				}
				else if (UserPot[2] == 2) {
					Set_parameters_1(index, midiChannel, CCfilterres, value);
				}
				else if (UserPot[2] == 3) {
					Set_parameters_1(index, midiChannel, myLFO1amt, value);
				}
				else if (UserPot[2] == 4) {
					Set_parameters_1(index, midiChannel, myLFO2amt, value);
				}
				else if (UserPot[2] == 5) {
					Set_parameters_1(index, midiChannel, myLFO3amt, value);
				}
				else if (UserPot[2] == 6) {
					Set_parameters_1(index, midiChannel, myLFO1rate, value);
				}
				else if (UserPot[2] == 7) {
					Set_parameters_1(index, midiChannel, myLFO2rate, value);
				}
				else if (UserPot[2] == 8) {
					Set_parameters_1(index, midiChannel, myLFO3rate, value);
				}
				else if (UserPot[2] == 9) {
					Set_parameters_1(index, midiChannel, CCHPFfilter, value);
				}
				else if (UserPot[2] == 10) {
					Set_parameters_1(index, midiChannel, CCHPFres, value);
				}
			}
			else if (index == 4) {	// Pot 3 : Parameter
				
				if (UserPot[3] == 1) {
					Set_parameters_1(index, midiChannel, CCfilterfreq, value);
				}
				else if (UserPot[3] == 2) {
					Set_parameters_1(index, midiChannel, CCfilterres, value);
				}
				else if (UserPot[3] == 3) {
					Set_parameters_1(index, midiChannel, myLFO1amt, value);
				}
				else if (UserPot[3] == 4) {
					Set_parameters_1(index, midiChannel, myLFO2amt, value);
				}
				else if (UserPot[3] == 5) {
					Set_parameters_1(index, midiChannel, myLFO3amt, value);
				}
				else if (UserPot[3] == 6) {
					Set_parameters_1(index, midiChannel, myLFO1rate, value);
				}
				else if (UserPot[3] == 7) {
					Set_parameters_1(index, midiChannel, myLFO2rate, value);
				}
				else if (UserPot[3] == 8) {
					Set_parameters_1(index, midiChannel, myLFO3rate, value);
				}
				else if (UserPot[3] == 9) {
					Set_parameters_1(index, midiChannel, CCHPFfilter, value);
				}
				else if (UserPot[3] == 10) {
					Set_parameters_1(index, midiChannel, CCHPFres, value);
				}
			}
		}

		break;
		
		// PageNo 1: Osc1 -------------------------------------------------
		case 1:
		// Osc1 Main Menu
		if (myPageShiftStatus[PageNr] == false) {
			if (index == 1) {			// Pot 1 : select
				uint16_t tempParameterNr = (0.062f * (value >> 5));	// 8 rows
				if (ParameterNr != tempParameterNr){
					ParameterNr = tempParameterNr;
					drawParamterFrame(PageNr, ParameterNr);
					ParameterNrMem[PageNr] = ParameterNr;
					ParmSelectFlag = false;
				}
			}
			else if (index == 2) {		// Pot 2 : value
				renderCurrentParameter(PageNr,(selecdParameter(PageNr, ParameterNr)),(value >> 5));
				// value for CCoscwaveform max 63
				if (selecdParameter(PageNr, ParameterNr) == CCoscwaveformA) {
					value = value >> 1;
				}
				myControlChange(midiChannel,(selecdParameter(PageNr, ParameterNr)), (value >> 5));
			}
			else if (index == 3) {		// Pot 3 : Waveform Bank
				if (ParameterNr == 0){
					myCCgroup1 (CCosc1WaveBank, (value >> 8));
				}
			}
		}
		// Osc Sub Menu
		else {
			if (index == 1) {		// Pot 1
				uint16_t tempParameterNr = (0.062f * (value >> 5));	// 8 rows
				if (ParameterNr != tempParameterNr){
					ParameterNr = tempParameterNr;
					drawParamterFrame(PageNr, ParameterNr);
					ParameterNrMem[3] = ParameterNr;
					ParmSelectFlag = false;
				}
			}
			else if (index == 2) {	// Pot 2
				renderCurrentParameter(PageNr,(selecdParameter(PageNr, ParameterNr)),(value >> 5));
				myControlChange(midiChannel,(selecdParameter(PageNr, ParameterNr)),(value >> 5));
			}
		}
		break;
		
		// PageNo 2: Osc2 -------------------------------------------------
		case 2:
		// Osc 2 Main Menu
		if (myPageShiftStatus[PageNr] == false) {
			if (index == 1) {		// Pot 1 : select
				uint8_t tempParameterNr = (0.062f * (value >> 5));	// 8 rows
				if (ParameterNr != tempParameterNr){
					ParameterNr = tempParameterNr;
					drawParamterFrame(PageNr, ParameterNr);
					ParameterNrMem[PageNr] = ParameterNr;
					ParmSelectFlag = false;
				}
			}
			else if (index == 2) {	// Pot 2 : value
				renderCurrentParameter(PageNr,(selecdParameter(PageNr, ParameterNr)),(value >> 5));
				// value for CCoscwaveform max 63
				if (selecdParameter(PageNr, ParameterNr) == CCoscwaveformB) {
					value = value >> 1;
				}
				myControlChange(midiChannel, (selecdParameter(PageNr, ParameterNr)),(value >> 5));
			}
			else if (index == 3) {	// Pot 3 : Waveform Bank
				if (ParameterNr == 0){
					myCCgroup1 (CCosc2WaveBank, (value >> 8));
				}
			}
			break;
		}
		// Osc Sub Menu
		else {
			if (index == 1) {		// Pot 1 : select
				uint16_t tempParameterNr = (0.062f * (value >> 5));	// 8 rows
				if (ParameterNr != tempParameterNr){
					ParameterNr = tempParameterNr;
					drawParamterFrame(PageNr, ParameterNr);
					ParameterNrMem[3] = ParameterNr;
					ParmSelectFlag = false;
				}
			}
			else if (index == 2) {	// Pot 2
				renderCurrentParameter(PageNr,(selecdParameter(PageNr, ParameterNr)),(value >> 5));
				myControlChange(midiChannel, (selecdParameter(PageNr, ParameterNr)), (value >> 5));
			}
			break;
		}
		
		// PageNo 3: Filter -----------------------------------------------
		case 3:
		// Filter 1 Main Menu (State Variable Filter)
		if (myFilter == 1 && myPageShiftStatus[PageNr] == false) {
			if (index == 1) {		// Pot 1 : Cutoff
				myControlChange(midiChannel, CCfilterfreq,(value >> 4));	// Value 0...255
			}
			else if (index == 2) {	// Pot 2 : Resonance
				myControlChange(midiChannel, CCfilterres,(value >> 5));		// Value 0...127
			}
			else if (index == 3) {	// Pot 3 : Filter Envelope
				myControlChange(midiChannel, CCfilterenv,(value >> 5));		// Value -63...+63
			}
			else if (index == 4) {	// Pot 4 : Typ
				FilterMix = (value >> 5);
				myControlChange(midiChannel, CCfiltermixer,(value >> 5));   // Value 0...127 (LP/HP/BP)
			}
		}

		// Filter 1 Sub Menu1 (State Variable Filter)
		else if (myFilter == 1 && myPageShiftStatus[PageNr] == true) {
			if (index == 1) {		// Pot 1 : Key tracking
				myControlChange(midiChannel, CCkeytracking,(value >> 5));	// Value 0...127
			}
			else if (index == 2) {	// Pot 2 : Velocity
				myControlChange(midiChannel, myFilterVelo,(value >> 5));	// Value 0...127
			}
			else if (index == 3) {	// Pot 3 : LFO2 Amt
				myControlChange(midiChannel, myLFO2amt,(value >> 5));		// Value 0...127
			}
		}
		// HPF Filter  Sub Menu2
		else if (myPageShiftStatus[PageNr] == 2) {
			if (index == 1) {		// Pot 1 : HPF filter frequency
				myControlChange(midiChannel, CCHPFfilter,(value >> 4));		// Value 0...255
			}
			else if (index == 2) {	// Pot 2 : Resonance
				myControlChange(midiChannel, CCHPFres,(value >> 5));		// Value 0...127
			}
			else if (index == 4) {		// Pot 4 : filter switch
				myControlChange(midiChannel, myFilterSwitch,(value >> 5));	// Value 0...127
			}
		}

		// Filter 2 (Ladder) Main Menu
		else if (myFilter == 2 && myPageShiftStatus[PageNr] == false) {
			if (index == 1) {		// Pot 1 : Cutoff
				myControlChange(midiChannel, CCfilterfreq,(value >> 4));	// Value 0...255
			}
			else if (index == 2) {	// Pot 2 : Resonance
				myControlChange(midiChannel, CCfilterres,(value >> 5));		// Value 0...127
			}
			else if (index == 3) {	// Pot 3 : Filter Envelope
				myControlChange(midiChannel, CCfilterenv,(value >> 5));		// Value -63...+63
			}
			else if (index == 4) {	// Pot 4
				myControlChange(midiChannel, CCLadderFilterDrive,(value >> 5));	// Value 0...127
			}
		}
		
		// Filter 2 (Ladder) Sub Menu
		else if (myFilter == 2 && myPageShiftStatus[PageNr] == true) {
			if (index == 1) {		// Pot 1 : Key tracking
				myControlChange(midiChannel, CCkeytracking,(value >> 5));	// Value 0...127
			}
			else if (index == 2) {	// Pot 2 : Filter Velocity
				myControlChange(midiChannel, myFilterVelo,(value >> 5));	// Value 0...127
			}
			else if (index == 3) {	// Pot 4 : LFO 2 Amt
				myControlChange(midiChannel, myLFO2amt,(value >> 5));		// Value 0...127
			}
			else if (index == 4) {	// Pot 3 : PassbandGain
				myControlChange(midiChannel, CCLadderFilterPassbandGain,(value >> 5));		// Value 0...127
			}
		}
		break;

		// PageNo 4: Filter Envelope --------------------------------------
		case 4:
		// VCF Envelope Main Menu
		if (myPageShiftStatus[PageNr] == false) {
			if (index == 1) {		// Pot 1 : Attack
				myControlChange(midiChannel, CCfilterattack,(value >> 5));
			}
			else if (index == 2) {	// Pot 2 : Decay
				myControlChange(midiChannel, CCfilterdecay,(value >> 5));
			}
			else if (index == 3) {	// Pot 3 : Sustain
				myControlChange(midiChannel, CCfiltersustain,(value >> 5));
			}
			else if (index == 4) {	// Pot 4 : Release
				myControlChange(midiChannel, CCfilterrelease,(value >> 5));
			}
		}
		
		// VCF Envelope Sub Menu
		if (myPageShiftStatus[PageNr] == true) {
			if (index == 1) {		// Pot 1 : Curve
				envelopeType1 = ((value >> 5) / 7.48f) - 8;
				Set_parameters_1(index, midiChannel, myFilterEnvCurve, value);
				updateFilterEnvelopeType();
				drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
			}
		}
		break;
		
		// PageNo 5: AMP Envelope -----------------------------------------
		case 5:
		// AMP Envelope Main Menu
		if (myPageShiftStatus[PageNr] == false) {
			if (index == 1) {		// Pot 1 : Attack
				myControlChange(midiChannel, CCampattack,(value >> 5));
			}
			else if (index == 2) {	// Pot 2 : Decay
				myControlChange(midiChannel, CCampdecay,(value >> 5));
			}
			else if (index == 3) {	// Pot 3 : Sustain
				myControlChange(midiChannel, CCampsustain,(value >> 5));
			}
			else if (index == 4) {	// Pot 4 : Release
				myControlChange(midiChannel, CCamprelease,(value >> 5));
			}
		}
		
		// AMP Envelope Sub Menu
		if (myPageShiftStatus[PageNr] == true) {
			if (index == 1) {		// Pot 1 : Curve
				envelopeType2 = ((value >> 5) / 7.48f) - 8;
				Set_parameters_1(index, midiChannel, myAmpEnvCurve, value);
				updateAmpEnvelopeType();
				drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
			}
			else if (index == 2) {	// Pot 2 : Velocity
				myControlChange(midiChannel, myAmplifierVelo, (value >> 5));
			}
			else if (index == 3) {	// Pot 3 : Pan
				myControlChange(midiChannel, myPan_Value, (value >> 5));
			}
		}
		break;
		
		// PageNo 6: LFO1 -------------------------------------------------
		case 6:
		if (myPageShiftStatus[PageNr] == false) {
			if (index == 1) {		// Pot 1 : Shape
				myControlChange(midiChannel, myLFO1shape,(value >> 5));
			}
			else if (index == 2) {	// Pot 2 : Rate
				myControlChange(midiChannel, myLFO1rate,(value >> 5));
			}
			else if (index == 3) {	// Pot 3 : Amt
				myControlChange(midiChannel, myLFO1amt,(value >> 5));
			}
			else if (index == 4) {	// Pot 4 : Sync
				renderCurrentParameter(PageNr,myLFO1syn,(value >> 4));
			}
		}
		// LFO1 Sub Menu
		else {
			if (index == 1) {		// Pot 1 : Mode
				renderCurrentParameter(PageNr,myLFO1mode,(value >> 4));
			}
			else if (index == 2) {	// Pot 2 : Fade In
				renderCurrentParameter(PageNr,myLFO1fade,(value >> 5));
			}
			else if (index == 3) {	// Pot 3 : Fade Out
				renderCurrentParameter(PageNr,myLFO1fadeOut,(value >> 5));
			}
			else if (index == 4) {	// Pot 4 : Curve
				renderCurrentParameter(PageNr,myLFO1envCurve,(value >> 5));
			}
		}
		break;
		
		// PageNo 7: LFO2 -------------------------------------------------
		case 7:
		if (myPageShiftStatus[PageNr] == false) {
			if (index == 1) {		// Pot 1 : Shape
				myControlChange(midiChannel, myLFO2shape,(value >> 5));
			}
			else if (index == 2) {	// Pot 2 : Rate
				myControlChange(midiChannel, myLFO2rate,(value >> 5));
			}
			else if (index == 3) {	// Pot 3 : Amt
				myControlChange(midiChannel, myLFO2amt, (value >> 5));
			}
			else if (index == 4) {	// Pot 4 : Sync
				renderCurrentParameter(PageNr,myLFO2syn,(value >> 4));
			}
		}
		// LFO2 Sub Menu
		else {
			if (index == 1) {		// Pot 1 : Mode
				renderCurrentParameter(PageNr,myLFO2mode,(value >> 4));
			}
			else if (index == 2) {	// Pot 2 : Fade In
				renderCurrentParameter(PageNr,myLFO2fade,(value >> 5));
			}
			else if (index == 3) {	// Pot 3 : Fade Out
				renderCurrentParameter(PageNr,myLFO2fadeOut,(value >> 5));
			}
			else if (index == 4) {	// Pot 4 : Curve
				renderCurrentParameter(PageNr,myLFO2envCurve,(value >> 5));
			}
		}
		break;
		
		// PageNo 8: LFO3 -------------------------------------------------
		case 8:
		if (myPageShiftStatus[PageNr] == false) {
			if (index == 1) {		// Pot 1 : Shape
				myControlChange(midiChannel, myLFO3shape,(value >> 5));
			}
			else if (index == 2) {	// Pot 2 : Rate
				myControlChange(midiChannel, myLFO3rate,(value >> 5));
			}
			else if (index == 3) {	// Pot 3 : Amt
				myControlChange(midiChannel, myLFO3amt, (value >> 5));
			}
			else if (index == 4) {	// Pot 4 : Sync
				renderCurrentParameter(PageNr,myLFO3syn,(value >> 4));
			}
		}
		// LFO3 Sub Menu
		else {
			if (index == 1) {		// Pot 1 : Mode
				renderCurrentParameter(PageNr,myLFO3mode,(value >> 4));
			}
			else if (index == 2) {	// Pot 2 : Fade In
				renderCurrentParameter(PageNr,myLFO3fade,(value >> 5));
			}
			else if (index == 3) {	// Pot 3 : Fade Out
				renderCurrentParameter(PageNr,myLFO3fadeOut,(value >> 5));
			}
			else if (index == 4) {	// Pot 4 : Curve
				renderCurrentParameter(PageNr,myLFO3envCurve,(value >> 5));
			}
		}
		break;
		
		// PageNo 9: Modulation -------------------------------------------
		case 9:
		if (index == 1) {		// Pot 1
			uint8_t tempParameterNr = (0.19f * (value >> 5));
			if (tempParameterNr >= 23) {
				tempParameterNr = 23;
			}
			if (ParameterNr != tempParameterNr){
				ParameterNr = tempParameterNr;
				uint8_t ParNo = ParameterNr % 4;
				printModParameter(ParameterNr);
				drawParamterFrame2(PageNr, ParNo);
				readModMatrixParameter(ParameterNr);
				ParameterNrMem[PageNr] = ParameterNr;
				ParmSelectFlag = false;
			}
		}
		else if (index == 2) {	// Pot 2
			renderModParameter(ParameterNr, (value >> 5));
		}
		break;
		
		// PageNo 10: FX DSP ----------------------------------------------
		case 10:
		if (index == 1) {		// Pot 1 : Select
			renderCurrentParameter(PageNr,myFxSel,(value >> 5));
		}
		else if (index == 2) {	// Pot 2 : Value
			renderCurrentParameter(PageNr,myFxVal,(value >> 4));
		}
		else if (index == 3) {	// Pot 3 : Mix
			renderCurrentParameter(PageNr,myFxMix,(value >> 4));
		}
		else if (index == 4) {	// Pot 4 : PRG
			renderCurrentParameter(PageNr,myFxPrg,(value >> 8));
		}
		else if (index == 5) {	// Pot X : Clock Rate
			renderCurrentParameter(PageNr,myFxClkrate,(value >> 5));
		}
		break;
		
		
		// PageNo 9: Sequencer --------------------------------------------
		case 11:
		if (myPageShiftStatus[PageNr] == false) {
			if (index == 1) {		// Pot 1 : Step
				renderCurrentParameter(PageNr,mySEQStep,(value >> 4));
			}
			else if (index == 2) {	// Pot 2 : Pitch
				renderCurrentParameter(PageNr,mySEQPitch,(value >> 4));
			}
			else if (index == 3) {	// Pot 3 : Rate
				renderCurrentParameter(PageNr,mySEQRate,(value >> 4));
			}
			else if (index == 4) {	// Pot 4 : Div
				renderCurrentParameter(PageNr,mySEQdiv,(value >> 4));
			}
		}
		// Sequencer Sub Menu
		else {
			if (index == 1) {		// Pot 1 : Len
				renderCurrentParameter(PageNr, mySEQLen,(value >> 4));
			}
			else if (index == 2) {	// Pot 2 : Time
				renderCurrentParameter(PageNr,mySEQGateTime,(value >> 4));
			}
			else if (index == 3) {	// Pot 3 : Dir
				renderCurrentParameter(PageNr,mySEQdir,(value >> 4));
			}
			else if (index == 4) {	// Pot 4 : Mode
				renderCurrentParameter(PageNr,mySEQmode,(value >> 4));
			}
		}
		break;
		
		// PageNo 12: System ----------------------------------------------
		case 12:
		if (index == 1) {		// Pot 1
			
			// System Main menu
			if (myPageShiftStatus[PageNr] == false) {						// Main page
				
				uint8_t tempParameterNr = (0.0393f * (value >> 4));	// Sub page (5 rows)
				if (ParameterNr != tempParameterNr){
					if (tempParameterNr >= 9) {
						tempParameterNr = 9;
					}
					ParameterNr = tempParameterNr;
					if (ParameterNr <= 4) {
						drawParamterFrame(PageNr, ParameterNr);
						//if (ParameterNr == 4){
						drawSystemPar1();
						//}
						} else {
						drawParamterFrame(PageNr, ParameterNr - 5);
						//if (ParameterNr == 5){
						drawSystemPar2();
						//}
					}
					ParameterNrMem[PageNr] = ParameterNr;	// Main page
					ParmSelectFlag = false;
				}
				
			}
			
			// System Sub menu (SysEx Dump)
			else {
				uint8_t tempParameterNr = (0.0393f * (value >> 5));	// Sub page (5 rows)
				if (ParameterNr != tempParameterNr){
					ParameterNr = tempParameterNr;
					drawParamterFrame(PageNr, ParameterNr);
					ParameterNrMem[PageNr+1] = ParameterNr; // Sub page
					ParmSelectFlag = false;
				}
			}
		}
		else if (index == 2) {	// Pot 2 : value
			if (myPageShiftStatus[PageNr] == false) {						// Main page
				renderCurrentParameter(PageNr,(selecdParameter(PageNr, ParameterNr)),(value >> 5));
				myControlChange(midiChannel,(selecdParameter(PageNr, ParameterNr)), (value >> 5));
				} else {														// Sub page
				renderCurrentParameter(PageNr,(selecdParameter(PageNr, ParameterNr)),(value >> 5));
				myControlChange(midiChannel,(selecdParameter(PageNr, ParameterNr)), (value >> 5));
			}
			
		}
		break;

		// PageNo 97: Load Sequencer Pattern ------------------------------
		case 97:
		if (index == 1) {		// Pot 1 : PatternNo
			renderCurrentParameter(PageNr,myPatternNo,(value >> 6));
		}
		break;

		// PageNo 98: Save Sequencer Pattern ------------------------------
		case 98:
		if (index == 1) {		// Pot 1 : PatternNo
			renderCurrentParameter(PageNr,myPatternNo,(value >> 6));
		}
		else if (index == 2) {	// Pot 2 : Char
			renderCurrentParameter(PageNr,myChar,(value >> 5));
		}
		else if (index == 3) {	// Pot 3 : Cursor
			renderCurrentParameter(PageNr,myCursor,(value >> 5));
		}
		break;
		
		// PageNo 99: Save Patch ------------------------------------------
		case 99:
		if (index == 1) {		// Pot 1 : Bank select
			renderCurrentParameter(PageNr,myBankSelect,(value >> 8));
		}
		else if (index == 2) {	// Pot 2 : PatchNo
			renderCurrentParameter(PageNr, myPatchNo,(value >> 5));
		}
		else if (index == 3) {	// Pot 3 : Char
			renderCurrentParameter(PageNr,myChar,(value >> 5));
		}
		else if (index == 4) {	// Pot 4 : Cursor
			renderCurrentParameter(PageNr,myCursor,(value >> 5));
		}
		break;
	}
}

//*************************************************************************
// Potentiometer Query
//*************************************************************************
FLASHMEM void checkPots(void) {
	
	int mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
	int mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
	int mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
	int mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
	int mux5Read = MCP_adc.read(MCP3208::Channel::SINGLE_4);
	
	if (mux1Read > (mux1ValuesPrev + QUANTISE_FACTOR) || mux1Read < (mux1ValuesPrev - QUANTISE_FACTOR)) {
		mux1ValuesPrev = mux1Read;
		set_menu_parameter(1, mux1Read);
	}
	
	if (mux2Read > (mux2ValuesPrev + QUANTISE_FACTOR) || mux2Read < (mux2ValuesPrev - QUANTISE_FACTOR)) {
		mux2ValuesPrev = mux2Read;
		set_menu_parameter(2, mux2Read);
	}
	
	if (mux3Read > (mux3ValuesPrev + QUANTISE_FACTOR) || mux3Read < (mux3ValuesPrev - QUANTISE_FACTOR)) {
		mux3ValuesPrev = mux3Read;
		set_menu_parameter(3, mux3Read);
	}
	
	if (mux4Read > (mux4ValuesPrev + QUANTISE_FACTOR) || mux4Read < (mux4ValuesPrev - QUANTISE_FACTOR)) {
		mux4ValuesPrev = mux4Read;
		set_menu_parameter(4, mux4Read);
	}
	
	if (mux5Read > (mux5ValuesPrev + QUANTISE_FACTOR) || mux5Read < (mux5ValuesPrev - QUANTISE_FACTOR)) {
		mux5ValuesPrev = mux5Read;
		set_menu_parameter(5, mux5Read);
	}
}

//*************************************************************************
// check Switches every 5ms
//*************************************************************************
FLASHMEM void checkSwitches(void) {
	
	uint8_t noise_sample;
	
	if ((micros() - timer_keyquery) > 530){
		timer_keyquery = micros();
		uint8_t parameter = 0;
		static int Debounce = 20;
		static uint16_t Debounce_less;
		static int waitnext = 320; // 2.menu page on load/save key
		static uint8_t oldkeyvalue = 0;
		uint8_t value = 0;
		
		// Encoder switch -----------------------------------------------------
		if (btnBouncer.update()) {
			if (btnBouncer.fallingEdge() == true && Keylock == false) {
				
				// Main Page
				if (PAGE_SW_Status == false) {
					PAGE_SW_Status = true;
					if (PageNr < 1) {
						PageNr = 1;
					}
					LastPageNr = PageNr;
					PageNr = 0;
					RefreshMainScreenFlag = true;
					clearScreenFlag = true;
					renderCurrentPatchPage();
				}
				// Parameter Pages
				else {
					PAGE_SW_Status = false;
					PageNr = LastPageNr;
					if (PageNr < 1) {
						PageNr = 1;
					}
					renderCurrentPatchPage();
				}
			}
		}

		// read key value
		value = MCP_adc.read(MCP3208::Channel::SINGLE_7) >> 5;
		
		// Key S2 "UNISONO" ----------------------------------------
		if (value < (S2 + hysteresis) && value > (S2 - hysteresis)) {
			KeyDebounce++;
			if (KeyDebounce == Debounce && KeyStatus == 0) {
				KeyStatus = 1;
				Debounce_less = 50;
				parameter = CCunison;
				value = 0;
				if (unison == 0) {
					unison = 1;
					NoteStack_ptr = 0;
					printVoiceMode();
				}
				else if (unison == 1) {
					unison = 0;
					NoteStack_ptr = 0;
					printVoiceMode();
				}
				myControlChange(midiChannel, parameter, unison);
			}
		}
		
		// Key S3 "SEQ" --------------------------------------------
		else if (value < (S3 + hysteresis) && value > (S3 - hysteresis)) {
			KeyDebounce++;
			if (KeyDebounce == Debounce && KeyStatus == 0 && Keylock == false) {
				KeyStatus = 1;
				Debounce_less = 50;
				parameter = myARPSEQ;
				if (SEQrunStatus == false && SeqNotesAvailable == true && SEQMidiClkSwitch == false) {	// run sequencer
					SEQselectStepNo = 0;
					SeqTranspose = 0;
					Interval = SEQclkRate;
					gateTime = (float)(SEQclkRate / SEQGateTime);	// set Interval and gateTime
					timer_intMidiClk = micros();
					timer_intMidiClk -= SEQclkRate;
					SEQdirectionFlag = false;
					SEQrunStatus = true;
				}
				else {		// stop Sequencer
					SEQrunStatus = false;
					if (SEQselectStepNo > 0) {
						SEQselectStepNo--;
					}
					//myMIDIClockStop();
					allNotesOff();
					SeqRecNoteCount = 0;
					SEQselectStepNo = 0;
					NoteStack_ptr = 0;
				}
			}
		}
		
		// Key S4 "MUTE/PANIC" -------------------------------
		else if (value < (S4 + hysteresis) && value > (S4 - hysteresis)) {
			KeyDebounce++;
			if (KeyDebounce == Debounce && KeyStatus == 0 && Keylock == false) {
				KeyStatus = 1;
				Debounce_less = 50;
				if (PageNr == 11 && SEQrunStatus == false && SEQmode == 2) {
					if (SeqNoteBufStatus[SEQselectStepNo] == 1) {
						SeqNoteBufStatus[SEQselectStepNo] = 0;	// mute note
						drawSEQpitchValue2(SEQselectStepNo);
						SEQselectStepNo++;
						if (SEQselectStepNo > SEQstepNumbers) {
							SEQselectStepNo = 0;
						}
						drawSEQStepFrame(SEQselectStepNo);
					}
					else {
						SeqNoteBufStatus[SEQselectStepNo] = 1;	// note on
						drawSEQpitchValue2(SEQselectStepNo);
						SEQselectStepNo++;
						if (SEQselectStepNo > SEQstepNumbers) {
							SEQselectStepNo = 0;
						}
						drawSEQStepFrame(SEQselectStepNo);
					}
				}
			}
			if (KeyDebounce == waitnext && KeyStatus == 1 && PageNr == 11 && SEQrunStatus == false) {  // init Sequencer
				clrSEQflag = true;
			}
			else if (KeyDebounce == waitnext && KeyStatus == 1 && PageNr !=11 && SEQrunStatus == false) {  // Panic all notes off
				allNotesOff();
			}
		}
		
		// Key S5 "BOOST" -------------------------------------
		else if (value < (S5 + hysteresis) && value > (S5 - hysteresis)) {
			KeyDebounce++;
			if (KeyDebounce == Debounce && KeyStatus == 0) {
				KeyStatus = 1;
				Debounce_less = 50;
				if (BassBoostStatus == 0) {
					BassBoostStatus = 1;
					myBoost = 1;
					digitalWrite(BassBoost,LOW);  // Boost on
					KeyLED4State = true;
				}
				else {
					BassBoostStatus = 0;
					myBoost = 0;
					digitalWrite(BassBoost,HIGH);  // Boost off
					KeyLED4State = true;
				}
			}
		}

		// Key S6 "SHIFT" -------------------------------------------------
		else if (value < (S6 + hysteresis) && value > (S6 - hysteresis)) {
			KeyDebounce++;
			if (KeyDebounce == Debounce && KeyStatus == 0 && PageNr > 0) {
				KeyStatus = 1;
				Debounce_less = 50;
				if (Keylock == true) {
					Keylock = false;
					S7KeyStatus = 0;
					PageNr = PageNr_old;
					if (PageNr == 0) {
						clearScreenFlag = true;
						RefreshMainScreenFlag = true;
					}
					renderCurrentPatchPage();	// draw old screen
				}
				else if (PageNr != 3) {
					if (myPageShiftStatus[PageNr] == false) {
						myPageShiftStatus[PageNr] = true;
					}
					else {
						myPageShiftStatus[PageNr] = false;
					}
					renderCurrentPatchPage();
					blinkiTime = 250;
					blink = true;
				}
				else if (PageNr == 3) {
					if (myPageShiftStatus[3] == 0) {
						myPageShiftStatus[3] = 1;
					}
					else if (myPageShiftStatus[3] == 1) {
						myPageShiftStatus[3] = 2;
					}
					else {
						myPageShiftStatus[3] = false;
					}
					renderCurrentPatchPage();
					blinkiTime = 250;
					blink = true;
				}
				
			}
			if (KeyDebounce == Debounce && KeyStatus == 0 && PageNr == 0) {
				PrgSelShift = true;
				Debounce_less = 50;
				if (myPageShiftStatus[0] == false) {
					myPageShiftStatus[0] = true;
					RefreshMainScreenFlag = true;
				}
				else {
					myPageShiftStatus[0] = false;
				}
			}
			
		}
		
		// Key S7 "LOAD/SAVE" ----------------------------------------
		else if (value < (S7 + hysteresis) && value > (S7 - hysteresis) && Debounce_less == 0) {
			KeyDebounce++;
			if (KeyDebounce == Debounce && KeyStatus == 0) {
				KeyStatus = 1;
				Debounce_less = 50;
				if (S7KeyStatus == 0) {
					S7KeyStatus = 1;
					PageNr_old = PageNr;
					if (PageNr < 11 || (PageNr == 12 && myPageShiftStatus[PageNr] == false)) {
						PageNr = 99;				// save Patch data
						newPatchNo = patchNo;
						Keylock = true;
						SEQrunStatus = false;
						renderCurrentPatchPage();	// draw "Save" screen
					}
					// Transmit / Receive SysEx Data
					else if (PageNr == 12 && myPageShiftStatus[PageNr] == true && sendSysExFlag == false) {
						Keylock = false;
						S7KeyStatus = 0;
						Debounce_less = 50;
						if (ParameterNrMem[13] == 4) {
							if (sysexDump == 0 && sendSysExFlag == false) {
								sendSysExFlag = true;
								sysexRecTimeStatus = true;
								sysexInit = true;
							}
							else if (sysexDump == 1 && sysexRecTimeStatus == false) {
								sysexRecTimeStatus = true;
								sysexInit = true;
							}
						}
					}
					// --------------------------------------------------
					else {
						PageNr = 97;
						Keylock = true;
						SEQrunStatus = false;
						allNotesOff();
						SEQselectStepNo = 0;
						NoteStack_ptr = 0;
						renderCurrentPatchPage();	// draw "Load Pattern" screen
					}
				}
				else if (S7KeyStatus == 1) {
					if (PageNr == 99) {				// save init Patch Data
						patchName = oldPatchName;
						currentPatchName = oldPatchName;
						newPatchName = oldPatchName;
						patchNo = newPatchNo;
						currentPatchBank = newPatchBank;
						String numString = (patchNo);
						String bankString = char(currentPatchBank + 65);
						String fileString = (bankString + "/" + numString);
						savePatch(String(fileString).c_str(), getCurrentPatchData());
						storeSoundPatchNo(patchNo);
						storePatchBankNo(currentPatchBank);
						recallPatch(patchNo);
						Keylock = false;
					}
					else if (PageNr == 98) {			// save Sequencer Pattern
						patternName = newPatternName;
						String numString = (SEQPatternNo);
						String folderString = "SEQ";					// Folder name
						String fileString = (folderString + "/" + numString);
						savePattern(String(fileString).c_str(), getCurrentPatternData());
						Keylock = false;
					}
					else if (PageNr == 97) {				// load Sequencer Pattern
						Keylock = false;
						SEQMidiClkSwitch = false;
						recallPattern(SEQPatternNo);
						SEQselectStepNo = 0;
						SeqTranspose = 0;
						Interval = SEQclkRate;
						gateTime = (float)(SEQclkRate / SEQGateTime);	// set Interval and gateTime
						timer_intMidiClk = micros();
						timer_intMidiClk -= SEQclkRate;
						SEQdirectionFlag = false;
						NoteStack_ptr = 0;
						SeqNotesAvailable = true;
						SEQrunStatus = true;					// start Sequencer
					}
					else if (PageNr == 96) {					// init Patch
						Keylock = false;
						set_initPatchData();
						if (PageNr_old == 0) {					// if Init Patch is called on the main page, 						
							patchName = oldPatchName;			// then save patch data directly to SD card
							currentPatchName = oldPatchName;
							newPatchName = oldPatchName;
							patchNo = newPatchNo;
							currentPatchBank = newPatchBank;
							String numString = (patchNo);
							String bankString = char(currentPatchBank + 65);
							String fileString = (bankString + "/" + numString);
							savePatch(String(fileString).c_str(), getCurrentPatchData());
							storeSoundPatchNo(patchNo);
							storePatchBankNo(currentPatchBank);
							recallPatch(patchNo);
							Keylock = false;
						}
						
					}
					S7KeyStatus = 0;
					Keylock = false;
					PageNr = PageNr_old;
					if (PageNr == 0) {
						PageNr = 99;
						renderCurrentPatchPage();	// Draw page again because of graphic error
						clearScreenFlag = true;
						RefreshMainScreenFlag = true;
						PageNr = 0;
					}
					renderCurrentPatchPage();	// return last menu page
				}
			}
			
			// long press LOAD/SAVE Key ---------------------------------
			if (KeyDebounce == waitnext && KeyStatus == 1) {
				Debounce_less = 50;
				if (PageNr == 97) {
					PageNr = 98;				// draw "Save Pattern" Menu
					renderCurrentPatchPage();
				}
				else if (PageNr == 99) {
					PageNr = 96;
					renderCurrentPatchPage();	// draw "init Patch" Menu
				}
			}
		}
		
		// Debounce ----------------------------------------------------
		else {
			if (Debounce_less == 0) {
				KeyDebounce = 0;
				KeyStatus = 0;
				PrgSelShift = false;
			}
			else Debounce_less--;
		}
	}
}

//*************************************************************************
// set FxPrg into 74HC595 Register
//*************************************************************************
FLASHMEM void setFxPrg (uint8_t PrgNo)
{
	int myPrg[16] = {5,0,1,2,3,4,6,7,8,9,10,11,12,13,14,15};
	uint8_t data =  myPrg[PrgNo];
	data = data << 4;
	if (PrgNo >= 8) {
		HC595Register &= 0b10001111;
	}
	else HC595Register &= 0b00001111;
	HC595Register |= data;
	SPI.transfer(HC595Register);
	digitalWrite(CS_HC595, LOW);
	digitalWrite(CS_HC595, HIGH);
	SPI.endTransaction();
}

//*************************************************************************
// init PWM for Fx
//*************************************************************************
FLASHMEM void initPWMFx (void)
{
	analogWriteFrequency (PWM1, 100000);
	analogWriteFrequency (PWM2, 100000);
	analogWriteFrequency (PWM3, 100000);
	analogWriteFrequency (PWM4, 100000);
	analogWriteFrequency (PWM5, 60000);		// Fx clock
	analogWrite(PWM1, 0);
	analogWrite(PWM2, 0);
	analogWrite(PWM3, 0);
	analogWrite(PWM4, 0);
	analogWrite(PWM5, 127);
}


//*************************************************************************
// init 74HC595 LED Driver
//*************************************************************************
FLASHMEM void initHC595 (void)
{
	HC595Register = 0;
	SPI.transfer(HC595Register);
	digitalWrite(CS_HC595, LOW);
	digitalWrite(CS_HC595, HIGH);
	SPI.endTransaction();
}


//*************************************************************************
// init Potentiometer
//*************************************************************************
FLASHMEM void initPotentiometers (void)
{
	// init Pots for first time
	if (initStatus == 1) {
		int mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
		int mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
		int mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
		int mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
		mux1ValuesPrev = mux1Read;
		mux2ValuesPrev = mux2Read;
		mux3ValuesPrev = mux3Read;
		mux4ValuesPrev = mux4Read;
		initStatus = 0;
	}
}

//*************************************************************************
// Encoder query
//*************************************************************************
FLASHMEM void checkEncoder(void)
{
	// if no encoder data or Load/Save function active than return
	if (encoderflag == false || Keylock == true) {
		return;
	}
	
	int encRead = encValue;
	encoderflag = false;
	
	// selected Parameter Pages -----------------------------------------
	if (PageNr > 0) {
		//long encRead = encoder.read();
		uint8_t oldPageNr = PageNr;
		if ((encCW && encRead > encPrevious) || (!encCW && encRead < encPrevious)) {
			PageNr++;
			if (PageNr > Pages){
				PageNr = Pages;
			}
			else if (PageNr < 1){
				PageNr = 1;
			}
			if (oldPageNr != PageNr) {
				renderCurrentPatchPage();
			}
			encPrevious = encRead;
		}
		
		else if ((encCW && encRead < encPrevious) || (!encCW && encRead > encPrevious)) {
			PageNr--;
			if (PageNr > Pages){
				PageNr = Pages;
			}
			else if (PageNr < 1){
				PageNr = 1;
			}
			if (oldPageNr != PageNr) {
				renderCurrentPatchPage();
				
			}
			encPrevious = encRead;
		}
	}
	// selected PatchNo --------------------------------------------------
	else {
		uint8_t oldpatchNo = patchNo;
		//long encRead = encoder.read();
		if ((encCW && encRead > encPrevious) || (!encCW && encRead < encPrevious)) {
			if (PrgSelShift == false) {
				patchNo++;
			} else patchNo += 10;
			if (patchNo > 128) {
				if (currentPatchBank < 15) {	// Bank A-P
					currentPatchBank++;
					patchNo = patchNo - 128;
				}
				else {
					patchNo = 128;
					currentPatchBank = 15;
				}
			}
			if (oldpatchNo != patchNo) {
				mute_before_load_patch();
				Keylock = true;
				recallPatch(patchNo);	// read only Patch name
				Keylock = false;
				currentPatchName = patchName;
				newPatchNo = patchNo;
				newPatchBank = currentPatchBank;
				RefreshMainScreenFlag = true;
			}
			encPrevious = encRead;
		}
		else if ((encCW && encRead < encPrevious) || (!encCW && encRead > encPrevious)) {
			if (PrgSelShift == false) {
				patchNo--;
			} else patchNo -= 10;
			if (patchNo < 1 || patchNo > 128) {
				if (currentPatchBank > 0 && currentPatchBank <= 15 ) {
					patchNo = patchNo - 128;
					currentPatchBank--;
				}
				else {
					patchNo = 1;
					currentPatchBank = 0;
				}
				
			}
			if (oldpatchNo != patchNo) {
				mute_before_load_patch();
				Keylock = true;
				recallPatch(patchNo);	// read only Patch name
				Keylock = false;
				currentPatchName = patchName;
				RefreshMainScreenFlag = true;
			}
			encPrevious = encRead;
		}
	}
}


//*************************************************************************
// Midi out
//*************************************************************************
FLASHMEM void midiCCOut(byte cc, byte value) {
	if (midiOutCh > 0) {
		// usbMIDI.sendControlChange(cc, value, midiOutCh);
		//midi1.sendControlChange(cc, value, midiOutCh);
	}
}

//*************************************************************************
// init PatchNo: 1
//*************************************************************************
FLASHMEM void Init_Patch(void)
{
	if (Init_flag == true){
		Init_flag = false;
		patchNo = getSoundPachNo();
		newPatchNo = patchNo;
		currentPatchBank = getPatchBankNo();
		newPatchBank = currentPatchBank;
		recallPatch(patchNo);
	}
}

//*************************************************************************
// CPU Monitor
//*************************************************************************
FLASHMEM void printCPUmon(void) {
	tft.fillRect(54,57,22,10,ST7735_BLACK);
	tft.setCursor(56,59);
	tft.setTextColor(ST7735_GRAY);
	tft.setTextSize(1);
	tft.print(CPUaudioMem);
	tft.print("%");
	tft.fillRect(50,95,29,10,ST7735_BLACK);
	tft.setTextColor(ST7735_GRAY);
	tft.setTextSize(1);
	
	// calc CPU Mem Block
	//uint8_t MemBlock = CPUmem * 0.78125f;
	uint8_t MemBlock = CPUmem * 0.69444f;
	if (MemBlock < 100) {
		tft.setCursor(56, 97);
		} else {
		tft.setCursor(51, 97);
	}
	tft.print(MemBlock);
	tft.print("%");
}


//*************************************************************************
// print CPU temperatur
//*************************************************************************
extern float tempmonGetTemp(void);

FLASHMEM void printTemperature (void)
{
	if (PageNr == 12 && myPageShiftStatus[12] == false){
		tft.fillRoundRect(54,38,22,10,2,ST7735_BLACK);
		tft.setCursor(55,40);
		tft.setTextColor(ST7735_GRAY);
		tft.setTextSize(1);
		tft.print(CPUdegree);
		tft.drawPixel(68,40,ST7735_GRAY);
		print_String(161,70,40);			// print "C"
	}
}

//*************************************************************************
// print SD card error
//*************************************************************************
FLASHMEM void printError(uint8_t index) {
	enableScope(false);
	tft.fillScreen(ST7735_BLACK);
	tft.fillRect(20,20,120,88,ST7735_RED);
	tft.setTextColor(ST7735_WHITE);
	tft.setTextSize(0);
	print_String(159,30,50);			// print "SD card error !"
	print_String(160,30,70);			// "Press power switch."
	tftUpdate = true;
}

//*************************************************************************
// clear Pick up Values
//*************************************************************************
FLASHMEM void clr_pickup (void)
{
	for (uint8_t i = 0; i < 4; i++) {
		PrevValue[i] = 0;
		if (pickupFlag == false) {
			PrevFlag[i] = true;
			} else {
			PrevFlag[i] = false;
		}
		
	}
}

//***********************************************************************
// Midi Clock Start
//***********************************************************************
FLASHMEM void myMIDIClockStart() {
	
	if (SEQMidiClkSwitch == true) {
		MidiClkTiming_Flag = true;
		SEQselectStepNo = 0;
		ARPSEQstatus = 0;
		SEQrunStatus = true;
		MidiCLKcount = 0;
	}
}

//**********************************************************************
// Midi Clock Stop
//**********************************************************************
FLASHMEM void myMIDIClockStop() {
	
	if (SEQMidiClkSwitch == true) {
		SEQrunStatus = false;
		ARPSEQstatus = false;
		SeqTranspose = 0;
		TempoLEDstate = false;	// set Tempo LED off
		TempoLEDchange = true;
		PlayFlag = false;
		MidiClkTiming_Flag = false;
		MidiCLKcount = 0;
		SEQselectStepNo = 0;
		SeqRecNoteCount = 0;
		NoteStack_ptr = 0;
		allNotesOff();
	}
}

//**********************************************************************
// receive Midi Clock data
//**********************************************************************
FLASHMEM void myMIDIClock(void) {
	
	// running Sequencer with Midi Clock -----------------------------------
	if (SEQMidiClkSwitch == true && SEQrunStatus == true) {
		
		MidiCLKcount++;
		
		if (MidiCLKcount == 1) {
			Sequencer2(true); // note on
			MidiCLKcountOff = float((16.5f - SEQGateTime) / 3.2f) + 2;
		}
		else if (MidiCLKcount == 2) {
			// calc note length
			if (MidiCLKcount >= MidiCLKcountOff) {
				Sequencer2(false); // note off
			}
			
		}
		else if (MidiCLKcount >= MidiCLKcountOff) {
			Sequencer2(false); // note off
		}
		
		if (MidiCLKcount >= MidiClkDiv) {
			MidiCLKcount = 0;
		}
	}
}

//*************************************************************************
// Play sequencer notes from extern midi clock
//*************************************************************************
FLASHMEM void Sequencer2 (boolean SEQNoteState) {
	
	if (SEQMidiClkSwitch == false || SEQrunStatus == false) {
		return;
	}

	if (SEQNoteState == true) {
		if (ARPSEQstatus == 0 && SEQrunStatus == true) {
			ARPSEQstatus = 1;
			SEQStepStatus = true;
			if (SeqNoteBufStatus[SEQselectStepNo] == 1) {
				uint8_t velo = SeqVeloBuf[SEQselectStepNo];
				if (velo <= 0) {
					velo = 127;
				}
				uint8_t noteCount = SeqNoteCount[SEQselectStepNo];
				for (uint8_t i = 0; i < noteCount; i++) {
					int bufAddr = (i * 16);
					int myNote = SeqNote1Buf[SEQselectStepNo + bufAddr];
					myNoteOn2(1,myNote, velo);
				}
				if (SEQselectStepNo == 0 || SEQselectStepNo == 4 || SEQselectStepNo == 8 || SEQselectStepNo == 12) {
					TempoLEDstate = true;	// set Tempo LED on
					TempoLEDchange = true;
				}
			}
		}
		} else {
		if (ARPSEQstatus == true && SEQrunStatus == true) {
			ARPSEQstatus = false;
			if (SEQselectStepNo == 1 || SEQselectStepNo == 5 || SEQselectStepNo == 9 || SEQselectStepNo == 13) {
				TempoLEDstate = false;	// set Tempo LED off
				TempoLEDchange = true;
			}
			if (SeqNoteBufStatus[SEQselectStepNo] == 1) {
				uint8_t noteCount = SeqNoteCount[SEQselectStepNo];
				for (uint8_t i = 0; i < noteCount; i++) {
					myNoteOff(midiChannel, SeqNote1Buf[(SEQselectStepNo + (i * 16))], 0);
				}
			}
			if (SEQrunOneNote == false) {
				// Direction ----------------------------------------------
				if (SEQdirection == 0) {  // ">"
					SEQselectStepNo++;
					if (SEQselectStepNo > SEQstepNumbers) {
						SEQselectStepNo = 0;
					}
				}
				else if (SEQdirection == 1) {	// "<"
					SEQselectStepNo--;
					if (SEQselectStepNo < 0) {
						SEQselectStepNo = SEQstepNumbers;
					}
				}
				else if (SEQdirection == 2 && SEQstepNumbers > 0) {	// "<>"
					if (SEQdirectionFlag == false) {
						SEQselectStepNo++;
						if (SEQselectStepNo > (SEQstepNumbers -1)) {
							SEQstepNumbers = SEQstepNumbers;
							SEQdirectionFlag = true;
						}
						} else {
						SEQselectStepNo--;
						if (SEQselectStepNo == 0) {
							SEQselectStepNo = 0;
							SEQdirectionFlag = false;
						}
					}
				}
				else if (SEQdirection == 2 && SEQstepNumbers == 0) {	// "<>"
					SEQselectStepNo = 0;
					SEQdirectionFlag = false;
				}
				else if (SEQdirection == 3) {	// "RND"
					SEQselectStepNo = Entropy.random(0,SEQstepNumbers + 1);
				}
			}
		}
	}
}


//*************************************************************************
// draw Sequencer recording Notes
//*************************************************************************
FLASHMEM void drawSEQRecNotes (void)
{
	drawSEQStepFrame(SEQselectStepNo); // draw yellow frame
	drawSEQpitchValue2(SEQselectStepNo);
}

//*************************************************************************
// Update Lfo3 Fx Modulation
//*************************************************************************
FLASHMEM void updateLfo3FxMod (void) {
	
	uint8_t val2;
	uint16_t val3;
	
	// convert LFO3 output level to uint 8Bit
	val2 = Lfo3Modoutput - 128;
	
	// Fx Pot1 Parameter
	val3 = (val2 *(DIV255 * (FxPot1amt * 2))) + FxPot1Val + (AtouchFxP1amt * 2);
	analogWrite(PWM1, val3);
	
	// Fx Pot2 Parameter
	val3 = (val2 *(DIV255 * (FxPot2amt * 2))) + FxPot2Val + (AtouchFxP2amt * 2);
	analogWrite(PWM2, val3);
	
	// Fx Pot3 Parameter
	val3 = (val2 *(DIV255 * (FxPot3amt * 2))) + FxPot3Val + (AtouchFxP3amt * 2);
	analogWrite(PWM3, val3);
		
	// FxMIX
	val3 = (val2 *(DIV255 * (FxMIXamt * 2))) + (FxMixValue * 2) + (AtouchFxMIXamt * 2);
	if (mute_sound == false) {
		analogWrite(PWM4, val3);
	}
}

//*************************************************************************
// update SupersawMix
//*************************************************************************
FLASHMEM void updateSupersawMix (void) {
	
	int valA = 127 - SupersawMixA;
	int valB = SupersawMixA;
	Supersaw_gain1A = 0.0078f * valA;
	Supersaw_gain2A = 0.0050f * valB;
	valA = 127 - SupersawMixB;
	valB = SupersawMixB;
	Supersaw_gain1B = 0.0078f * valA;
	Supersaw_gain2B = 0.0050f * valB;
}


//*************************************************************************
// update PWM LFO
//*************************************************************************
FLASHMEM void update_PWM_LFO (void)
{
	if (PWMaShape == 3) {
		pwmLfoA.offset(1.0f);
	} else pwmLfoA.offset(0.0f);
	pwmLfoA.begin(PWMWAVEFORMA + PWMaShape);
	
	if (PWMbShape == 3) {
		pwmLfoB.offset(1.0f);
	} else pwmLfoB.offset(0.0f);
	pwmLfoB.begin(PWMWAVEFORMB + PWMbShape);
	
}

//*************************************************************************
// get SysEx Dump from USB
//*************************************************************************
void getSysexDump(const uint8_t *buffer, uint16_t lenght, boolean last) {

	int filenumbers;
	boolean ProgBar_enabled;
	int ProgBar_style;
	int ProgBar_value;
	static int ProgBar_count = 0;
	static uint8_t numbers = 0;
	int ProgBar_maxcount;
	
	
	// copy 1.dump block into sysex_buffer and return
	if (last == 0) {
		for (int i = 0; i < lenght; i++) {
			Syx_Buf[i] = buffer[i];
		}
		sysex_buf_pointer = lenght;	// save buffer pointer
		return;
		} else {
		for (int i = 0; i < lenght; i++) {
			Syx_Buf[sysex_buf_pointer + i] = buffer[i];
		}
	}
	
	// check Dump lenght
	if (NO_OF_SysEx_Data > (sysex_buf_pointer + lenght)) {	// if record dump is too small then add data
		uint16_t rest = NO_OF_SysEx_Data - (sysex_buf_pointer + lenght);
		for (uint16_t i = 0; i < rest; i++) {
			Syx_Buf[(sysex_buf_pointer + lenght - 1) + i] = 0;
		}
		Syx_Buf[NO_OF_SysEx_Data - 1] = 0xF7;
	}
	if (NO_OF_SysEx_Data < (sysex_buf_pointer + lenght)) { // record dump to long than return;
		sysex_buf_pointer = 0;
		Serial.print("SysEx Dump too large!");
		return;
	}
	
	
	// check sysex start_byte and end_byte
	if (Syx_Buf[0] != 0xF0 || Syx_Buf[NO_OF_SysEx_Data - 1] != 0xF7) {
		Serial.println("SysEx data error!");
		Serial.print("Start_byte: ");
		Serial.println(Syx_Buf[0]);
		Serial.print("End_byte: ");
		Serial.println(Syx_Buf[NO_OF_SysEx_Data - 1]);
		sysex_buf_pointer = 0;
		return;
	}
	sysex_buf_pointer = 0;
	
	// save SysEx data ----------------------------------------------------
	if (PageNr == 12 && myPageShiftStatus[12] == true && sysexDump == true && sysexRecTimeStatus == true) {
		
		// get SysEx data
		get_SysEx_Dump();
		
		// How many files (one Patch or Patch Bank)
		if (sysexTyp == false) {
			filenumbers = 1;		// init counts for one patch file (Patch)
			numbers = 0;
			ProgBar_enabled = true;
			ProgBar_style = 2;
			ProgBar_value = 127;
			ProgBar_maxcount = 127;
			} else {
			filenumbers = 128;		// init counts for 128 patch files (Bank)
			//numbers = 0;
			ProgBar_enabled = true;
			ProgBar_style = 2;
			ProgBar_value = 1;
			ProgBar_maxcount = 127;
		}
		
		numbers++;	// inc file numbers
		currentPatchBank = sysexBank;
		
		// get Patch name for Progressbar info
		count = 7;														// (0) Patch Name
		String patch_Name = "";
		for (int i = 0; i < 12; i++) {
			patch_Name += (char)(Syx_Buf[count++]);
		}
		//Serial.println(patch_Name);
		
		// save patch name for Progressbar info
		currentPatchName = patch_Name;
		patchName = patch_Name; // save name in Patch datas
		String ProgString = "Rec: ";
		ProgString = ProgString + patch_Name;

		// set Progressbar values
		if (sysexInit == true) {
			ProgBar_count = 0;
			sysexInit = false;
		}
		ProgBar_count += ProgBar_value;
		if (ProgBar_count >= ProgBar_maxcount) {
			ProgBar_count = ProgBar_maxcount;
		}
		setProgressbar(ProgBar_enabled, ProgBar_style, ProgBar_count, ProgBar_maxcount, ProgString);
		
		// Store SysEx data (Patch) on SD card
		
		uint8_t FNo;
		if (filenumbers != 1){
			FNo = numbers;
			} else {
			FNo = sysexSource + 1;
		}
		String numString = (FNo);
		String bankString = char(currentPatchBank + 65);
		String fileString = (bankString + "/" + numString);
		savePatch(String(fileString).c_str(), getCurrentPatchData());
		//Serial.println(fileString);
		time_sysexRecStatus = false;
		//Serial.println("Receive on SDcard ok");	// print ok message

		uint8_t max_patchnumber;
		
		if (sysexTyp == false) { // Patch
			max_patchnumber = 1;
			patchNo = sysexSource + 1;
		}
		else {
			max_patchnumber = 128; // Bank
			patchNo = 1;
		}

		if (numbers == max_patchnumber) {		// end of sysex receive
			numbers = 0;
			sysexRecTimeStatus = false;
			recallPatch(patchNo);				// Set the 1st received patch as current patch
			newPatchNo = patchNo;
			newPatchBank = currentPatchBank;
			clearScreenFlag = true;
			RefreshMainScreenFlag = true;
			sysexRecTimeStatus = false;
			time_sysexRecStatus = false;
			ProgBar_count = 0;
			mute_sound = true;
		}
	}
}

//*************************************************************************
// send Midi system exclusive dump
//*************************************************************************
FLASHMEM void mySendSysEx(uint8_t PatchNo, uint8_t BankNo)
{
	//byte sysexData[512];										// SysEx send buffer
	uint8_t data_len = NO_OF_PARAMS;
	uint16_t sysexCount = 0;
	String numString = String(PatchNo + 1);
	String bankString = char(BankNo + 65);
	String fileString = (bankString + "/" + numString);
	
	// get Sound File String
	File patchFile = SD.open(fileString.c_str());
	
	// if Patch file not available create new init patch file
	if (!patchFile) {
		// Serial.print("Patch Error: ");
		// Serial.println(fileString);
		patchFile.close();
		set_initPatchData();
		savePatch(String(fileString).c_str(), getCurrentPatchData());
		patchFile = SD.open(fileString.c_str());
	}
	
	// load patch data string
	String data[data_len]; //Array of data read in
	recallPatchData(patchFile, data);
	patchFile.close();
	
	// convert Patch data into 7bit SysEx format
	// status data												// Ind. len   value
	Syx_Buf[sysexCount++] = 0xF0;								// -	1	0	- Start SysEx
	Syx_Buf[sysexCount++] = 0x00;								// -	1	1	- ID
	Syx_Buf[sysexCount++] = 0x00;								// -	1	2	- ID
	Syx_Buf[sysexCount++] = 0x00;								// -	1	3	- ID
	Syx_Buf[sysexCount++] = 0x00;								// -	1	4	- Device ID 0-64
	Syx_Buf[sysexCount++] = PatchNo;							// -	1	5	- Patch No
	Syx_Buf[sysexCount++] = BankNo;								// -	1	6	- Bank No
	
	// copy patchname for progressbar string
	patchname = data[0];
	float_to_string(data[0], 12, sysexCount, Syx_Buf);			// (0)	12	7	- Patch Name
	
	// Parameter data
	float_to_sysex1Byte(data[1], sysexCount, Syx_Buf);			// (1)	1	19  - oscALevel (0 - 1.00)
	float_to_sysex1Byte(data[2], sysexCount, Syx_Buf);			// (2)	1	20  - oscBLevel (0 - 1.00)
	float_to_sysex2Bytes(data[3], sysexCount, Syx_Buf);			// (3)	2	21  - noiseLevel ( -1.00 - +1.00)
	uint8_to_sysex1Byte(data[4], sysexCount, Syx_Buf);			// (4)	1	23  - unison (0 - 2)
	uint8_to_sysex1Byte(data[5], sysexCount, Syx_Buf);			// (5)	1	24  - oscFX	(0 - 6)
	float_to_sysex5Bytes(data[6], sysexCount, Syx_Buf);			// (6)	5 	25  - detune (0 - 1.00000)
	// (7) 			- (not available)
	float_to_string(data[8], 4, sysexCount, Syx_Buf);			// (8)	4	30	- midiClkTimeInterval
	// (9)			- (not available)
	float_to_sysex5Bytes(data[10], sysexCount, Syx_Buf);		// (10) 5 	34  - keytrackingAmount (0 - 1.0000)
	float_to_sysex5Bytes(data[11], sysexCount, Syx_Buf);		// (11)	5	39  - glideSpeed (0 - 1.00000)
	int8_to_sysex2Bytes(data[12], sysexCount, Syx_Buf);			// (12)	2	44  - oscPitchA (-24 - +24)
	int8_to_sysex2Bytes(data[13], sysexCount, Syx_Buf);			// (13) 2 	46  - oscPitchB (-24 - +24)
	uint8_to_sysex1Byte(data[14], sysexCount, Syx_Buf);			// (14) 1 	48  - oscWaveformA (0 - 63)
	uint8_to_sysex1Byte(data[15], sysexCount, Syx_Buf);			// (15)	1	49	- oscWaveformB (0 - 63)
	// (16)			- (not available)
	float_to_sysex1Byte(data[17], sysexCount, Syx_Buf);			// (17) 1   50  - pwmAmtA (0 - 0.99)
	float_to_sysex1Byte(data[18], sysexCount, Syx_Buf);			// (18) 1   51  - pwmAmtB (0 - 0.99)
	// (19)			- (not available)
	float_to_sysex2Bytes(data[20], sysexCount, Syx_Buf);		// (20) 2 	52  - pwA (-1.00 - +1.00)
	float_to_sysex2Bytes(data[21], sysexCount, Syx_Buf);		// (21) 2 	54  - pwB (-1.00 - +1.00)
	float_to_string(data[22], 4, sysexCount, Syx_Buf);			// (22) 4	56  - filterRes (0 - 15.0)
	uint14_to_sysex2Bytes(data[23], sysexCount, Syx_Buf);		// (23) 2 	60  - filterFreq (18 - 12000)
	float_to_string(data[24], 4, sysexCount, Syx_Buf);			// (24) 4 	62  - filterMix (0 - -99.0)
	float_to_sysex2Bytes(data[25], sysexCount, Syx_Buf);		// (25) 2	66  - filterEnv (-1.00 - +1.00)
	float_to_sysex5Bytes(data[26], sysexCount, Syx_Buf);		// (26) 5	67  - oscLfoAmt (0 - 1.00000)
	float_to_sysex5Bytes(data[27], sysexCount, Syx_Buf);		// (27) 5 	73  - oscLfoRate (0 - 40.0000)
	uint8_to_sysex1Byte(data[28], sysexCount, Syx_Buf);			// (28) 1 	78  - oscLFOWaveform (0 - 12)
	uint8_to_sysex1Byte(data[29], sysexCount, Syx_Buf);			// (29) 1 	79  - oscLfoRetrig (0 - 1)
	// (30) 	    - (not available)
	uint8_to_sysex1Byte(data[31], sysexCount, Syx_Buf);			// (31) 1 	80  - myFilterLFORateValue (1 - 127)
	uint8_to_sysex1Byte(data[32], sysexCount, Syx_Buf);			// (32) 1	81  - filterLfoRetrig (0 - 1)
	// (33) 	    - (not available)
	float_to_sysex5Bytes(data[34], sysexCount, Syx_Buf);		// (34) 5 	82  - filterLfoAmt (0 - 1.00000)
	uint8_to_sysex1Byte(data[35], sysexCount, Syx_Buf);			// (35) 1	87  - filterLFOWaveform (0 - 12)
	uint14_to_sysex2Bytes(data[36], sysexCount, Syx_Buf);		// (36) 2	88  - filterAttack (0 - 11880)
	uint14_to_sysex2Bytes(data[37], sysexCount, Syx_Buf);		// (37) 2 	90  - filterDecay (0 - 11880)
	float_to_sysex1Byte(data[38], sysexCount, Syx_Buf);			// (38) 1	92  - filterSustain (0 - 1.00)
	uint14_to_sysex2Bytes(data[39], sysexCount, Syx_Buf);		// (39) 2	94  - filterRelease (0 - 11880)
	uint14_to_sysex2Bytes(data[40], sysexCount, Syx_Buf);		// (40) 2	96  - ampAttack (0 - 11880)
	uint14_to_sysex2Bytes(data[41], sysexCount, Syx_Buf);		// (41) 2 	98  - ampDecay (0 - 11880)
	float_to_sysex1Byte(data[42], sysexCount, Syx_Buf);			// (42) 1	100  - ampSustain (0 - 1.00)
	uint14_to_sysex2Bytes(data[43], sysexCount, Syx_Buf);		// (43) 2 	101  - ampRelease (0 - 11880)
	// (44) 	    - (not available)
	// (45) 	    - (not available)
	// (46) 	    - (not available)
	uint8_to_sysex1Byte(data[47], sysexCount, Syx_Buf);			// (47) 1	102  - velocitySens (0-4)
	uint8_to_sysex1Byte(data[48], sysexCount, Syx_Buf);			// (48) 1	103  - chordDetune (0 - 127)
	uint8_to_sysex1Byte(data[49], sysexCount, Syx_Buf);			// (49) 1	104  - FxPot1value (0 - 127)
	uint8_to_sysex1Byte(data[50], sysexCount, Syx_Buf);			// (50) 1	105  - FxPot2value (0 - 127)
	uint8_to_sysex1Byte(data[51], sysexCount, Syx_Buf);			// (51) 1	106  - FxPot3value (0 - 127)
	uint8_to_sysex1Byte(data[52], sysexCount, Syx_Buf);			// (52) 1	107  - FxPrgNo (0 - 15)
	uint8_to_sysex1Byte(data[53], sysexCount, Syx_Buf);			// (53) 1	108  - FxMixValue (0 - 127)
	float_to_sysex5Bytes(data[54], sysexCount, Syx_Buf);		// (54) 5	109  - FxMixValue (10000 - 60000)
	uint8_to_sysex1Byte(data[55], sysexCount, Syx_Buf);			// (55) 1	114  - Osc1WaveBank (0 - 15)
	uint8_to_sysex1Byte(data[56], sysexCount, Syx_Buf);			// (56) 1	115  - Osc1WaveBank (0 - 15)
	uint8_to_sysex1Byte(data[57], sysexCount, Syx_Buf);			// (57) 1	116  - myBoost (0 - 1)
	float_to_sysex5Bytes(data[58], sysexCount, Syx_Buf);		// (58) 5	117  - pitchEnvA (-1.00 - +1.00)
	float_to_sysex5Bytes(data[59], sysexCount, Syx_Buf);		// (59) 5	122  - pitchEnvB (-1.00 - +1.00)
	float_to_sysex2Bytes(data[60], sysexCount, Syx_Buf);		// (60) 2	127  - driveLevel (Osc level) (0.00 - 1.25)
	float_to_sysex5Bytes(data[61], sysexCount, Syx_Buf);		// (61) 5	129  - myFilVelocity (0 - 1.00)
	float_to_sysex5Bytes(data[62], sysexCount, Syx_Buf);		// (62) 5	134  - myAmpVelocity (0 - 1.00)
	uint8_to_sysex1Byte(data[63], sysexCount, Syx_Buf);			// (63) 1	139  - myUnisono (0-2)
	// (64)			 - (not available)
	// (65)			 - (not available)
	uint8_to_sysex1Byte(data[66], sysexCount, Syx_Buf);			// (66) 1	140  - WShaperNo (0-14)
	float_to_sysex5Bytes(data[67], sysexCount, Syx_Buf);		// (67) 5	141  - WShaperDrive (0.10 - 5.00)
	uint14_to_sysex2Bytes(data[68], sysexCount, Syx_Buf);		// (68) 2	146  - LFO1phase (0 - 180.0)
	uint14_to_sysex2Bytes(data[69], sysexCount, Syx_Buf);		// (69) 2	148  - LFO2phase (0 - 180.0)
	for (uint8_t i = 0; i < 16; i++) {
		uint8_to_sysex1Byte(data[70+i], sysexCount, Syx_Buf);	// (70-85)  16	150 - SeqNote1Buf (0 - 127)
	}
	for (uint8_t i = 0; i < 16; i++) {
		uint8_to_sysex1Byte(data[86+i], sysexCount, Syx_Buf);	// (86-101) 16	166 - SeqNoteBufStatus (0 - 1)
	}
	uint14_to_sysex2Bytes(data[102], sysexCount, Syx_Buf);		// (102) 2 	182  - SEQbpmValue (101 - 462)
	float_to_sysex5Bytes(data[103], sysexCount, Syx_Buf);		// (103) 5	184  - SEQdivValue (float)
	uint8_to_sysex1Byte(data[104], sysexCount, Syx_Buf);		// (104) 1	189  - SEQstepNumbers (0 - 15)
	float_to_sysex5Bytes(data[105], sysexCount, Syx_Buf);		// (105) 5	190  - SEQGateTime (float)
	uint8_to_sysex1Byte(data[106], sysexCount, Syx_Buf);		// (106) 1	195  - SEQdirection (0 - 3)
	uint8_to_sysex1Byte(data[107], sysexCount, Syx_Buf);		// (107) 1	196  - oscDetuneSync (0 - 1)
	int8_to_sysex2Bytes(data[108], sysexCount, Syx_Buf);		// (108) 2	197  - oscPitchA (-12 - +12)
	float_to_sysex5Bytes(data[109], sysexCount, Syx_Buf);		// (109) 5	199  - oscMasterTune (float)
	float_to_sysex5Bytes(data[110], sysexCount, Syx_Buf);		// (110) 5	204  - OscVCFMOD (float)
	for (uint8_t i = 0; i < 16; i++) {
		uint8_to_sysex1Byte(data[111+i], sysexCount, Syx_Buf);	// (111-126) 16	 209 - SeqVeloBuf (0 - 127)
	}
	for (uint8_t i = 0; i < 48; i++) {
		uint8_to_sysex1Byte(data[127+i], sysexCount, Syx_Buf);	// (127-174) 48	 225 - SeqNote1Buf (0 - 127)
	}
	for (uint8_t i = 0; i < 16; i++) {
		uint8_to_sysex1Byte(data[175+i], sysexCount, Syx_Buf);	// (175-190) 16	 273 - SeqNoteCount (0 - 3)
	}
	uint8_to_sysex1Byte(data[191], sysexCount, Syx_Buf);		// (191) 1	289 - SEQmode (0 - 2)
	uint8_to_sysex1Byte(data[192], sysexCount, Syx_Buf);		// (192) 1	290 - SEQMidiClkSwitch (0 - 1)
	uint8_to_sysex1Byte(data[193], sysexCount, Syx_Buf);		// (193) 1	291 - LadderFilterpassbandgain (0 - 127)
	uint8_to_sysex1Byte(data[194], sysexCount, Syx_Buf);		// (194) 1	292 - LadderFilterDrive (1 - 127)
	int8_to_sysex2Bytes(data[195], sysexCount, Syx_Buf);		// (195) 2	293 - envelopeType1 (-8 - +8)
	int8_to_sysex2Bytes(data[196], sysexCount, Syx_Buf);		// (196) 2	295 - envelopeType2 (-8 - +8)
	float_to_sysex5Bytes(data[197], sysexCount, Syx_Buf);		// (197) 5	297 - PitchWheelAmt (float)
	float_to_sysex5Bytes(data[198], sysexCount, Syx_Buf);		// (198) 5	302 - MODWheelAmt (float)
	uint8_to_sysex1Byte(data[199], sysexCount, Syx_Buf);		// (199) 1	307 - myFilter (1 - 2)
	float_to_sysex5Bytes(data[200], sysexCount, Syx_Buf);		// (200) 5	308 - pwmRateA (-10.00 - + 10.00)
	float_to_sysex5Bytes(data[201], sysexCount, Syx_Buf);		// (201) 5	313 - pwmRateB (-10.00 - + 10.00)
	uint14_to_sysex2Bytes(data[202], sysexCount, Syx_Buf);		// (202) 2	318 - LFO1fadeTime (0 - 12000) Fade in
	uint14_to_sysex2Bytes(data[203], sysexCount, Syx_Buf);		// (203) 2	320 - LFO1releaseTime (0 - 12000) Fade out
	float_to_sysex5Bytes(data[204], sysexCount, Syx_Buf);		// (204) 5	322 - filterFM (0.00000 - 1.00000)  Osc1
	float_to_sysex5Bytes(data[205], sysexCount, Syx_Buf);		// (205) 5	327 - filterFM2 (0.00000 - 1.00000)	Osc2
	uint14_to_sysex2Bytes(data[206], sysexCount, Syx_Buf);		// (206) 2	332 - LFO2fadeTime (0 - 12000) Fade in
	uint14_to_sysex2Bytes(data[207], sysexCount, Syx_Buf);		// (207) 2	334 - LFO2releaseTime (0 - 12000) Fade out
	float_to_sysex5Bytes(data[208], sysexCount, Syx_Buf);		// (208) 5	336 - Osc1ModAmt (0.00000 - 1.00000) Fx Mod
	int8_to_sysex2Bytes(data[209], sysexCount, Syx_Buf);		// (209) 2	341 - LFO1enCurve (-8 - +8)
	int8_to_sysex2Bytes(data[210], sysexCount, Syx_Buf);		// (210) 2	343 - LFO2enCurve (-8 - +8)
	uint8_to_sysex1Byte(data[211], sysexCount, Syx_Buf);		// (211) 1	345 - LFO1mode (0 - 1)
	uint8_to_sysex1Byte(data[212], sysexCount, Syx_Buf);		// (212) 1	346 - LFO2mode (0 - 1)
	
	int8_to_sysex2Bytes(data[213], sysexCount, Syx_Buf);		// (213) 2	347 - LFO3enCurve (-8 - +8)
	uint8_to_sysex1Byte(data[214], sysexCount, Syx_Buf);		// (214) 1	349 - LFO3mode (0 - 1)
	uint14_to_sysex2Bytes(data[215], sysexCount, Syx_Buf);		// (215) 2	350 - LFO3fadeTime (0 - 12000) Fade in
	uint14_to_sysex2Bytes(data[216], sysexCount, Syx_Buf);		// (216) 2	352 - LFO3releaseTime (0 - 12000) Fade out
	float_to_sysex5Bytes(data[217], sysexCount, Syx_Buf);		// (217) 5 	354  - Lfo3amt (0 - 1.00000)
	uint8_to_sysex1Byte(data[218], sysexCount, Syx_Buf);		// (218) 1	359 - AtouchAmt (1 - 127)
	uint8_to_sysex1Byte(data[219], sysexCount, Syx_Buf);		// (219) 1	360 - AtouchPitchAmt (1 - 127)
	uint8_to_sysex1Byte(data[220], sysexCount, Syx_Buf);		// (220) 1	361 - Lfo3Waveform (1 - 127)
	uint8_to_sysex1Byte(data[221], sysexCount, Syx_Buf);		// (221) 1	362 - myLFO3RateValue (1 - 127)
	uint14_to_sysex2Bytes(data[222], sysexCount, Syx_Buf);		// (222) 2	363 - LFO3phase (0 - 180.0)
	uint8_to_sysex1Byte(data[223], sysexCount, Syx_Buf);		// (223) 1	365 - AtouchLFO1Amt (1 - 127)
	uint8_to_sysex1Byte(data[224], sysexCount, Syx_Buf);		// (224) 1	366 - AtouchLFO2Amt (1 - 127)
	uint8_to_sysex1Byte(data[225], sysexCount, Syx_Buf);		// (225) 1	367 - AtouchLFO3Amt (1 - 127)
	uint8_to_sysex1Byte(data[226], sysexCount, Syx_Buf);		// (226) 1	368 - FxPot1amt (1 - 127)
	uint8_to_sysex1Byte(data[227], sysexCount, Syx_Buf);		// (227) 1	369 - FxPot2amt (1 - 127)
	uint8_to_sysex1Byte(data[228], sysexCount, Syx_Buf);		// (228) 1	370 - FxPot3amt (1 - 127)
	uint8_to_sysex1Byte(data[229], sysexCount, Syx_Buf);		// (229) 1	371 - FxCLKamt (1 - 127)
	uint8_to_sysex1Byte(data[230], sysexCount, Syx_Buf);		// (230) 1	372 - FxMIXamt (1 - 127)
	uint8_to_sysex1Byte(data[231], sysexCount, Syx_Buf);		// (231) 1	373 - AtouchFxP1Amt (1 - 127)
	uint8_to_sysex1Byte(data[232], sysexCount, Syx_Buf);		// (232) 1	374 - AtouchFxP2Amt (1 - 127)
	uint8_to_sysex1Byte(data[233], sysexCount, Syx_Buf);		// (233) 1	375 - AtouchFxP3Amt (1 - 127)
	uint8_to_sysex1Byte(data[234], sysexCount, Syx_Buf);		// (234) 1	376 - AtouchFxClkAmt (1 - 127)
	uint8_to_sysex1Byte(data[235], sysexCount, Syx_Buf);		// (235) 1	377 - AtouchFxMixAmt (1 - 127)
	uint8_to_sysex1Byte(data[236], sysexCount, Syx_Buf);		// (236) 1	378 - PWMaShape (1 - 127)
	uint8_to_sysex1Byte(data[237], sysexCount, Syx_Buf);		// (237) 1	379 - PWMbShape (1 - 127)
	float_to_sysex5Bytes(data[238], sysexCount, Syx_Buf);		// (238) 5	380 - HPF_filterFreq (float)
	uint8_to_sysex1Byte(data[239], sysexCount, Syx_Buf);		// (239) 1	385 - UserPot1 (0 - xxx)
	uint8_to_sysex1Byte(data[240], sysexCount, Syx_Buf);		// (240) 1	386 - UserPot2 (0 - xxx)
	uint8_to_sysex1Byte(data[241], sysexCount, Syx_Buf);		// (241) 1	387 - UserPot3 (0 - xxx)
	uint8_to_sysex1Byte(data[242], sysexCount, Syx_Buf);		// (242) 1	388 - UserPot4 (0 - xxx)
	float_to_sysex5Bytes(data[243], sysexCount, Syx_Buf);		// (243) 5	389 - HPFRes (float)
	uint8_to_sysex1Byte(data[244], sysexCount, Syx_Buf);		// (244) 1	394 - SupersawSpreadA (0-127)
	uint8_to_sysex1Byte(data[245], sysexCount, Syx_Buf);		// (245) 1	395 - SupersawSpreadB (0-127)
	uint8_to_sysex1Byte(data[246], sysexCount, Syx_Buf);		// (246) 1	396 - SupersawMixA (0-127)
	uint8_to_sysex1Byte(data[247], sysexCount, Syx_Buf);		// (247) 1	397 - SupersawMixB (0-127)
	uint8_to_sysex1Byte(data[248], sysexCount, Syx_Buf);		// (248) 1	398 - Voice_mode (0-8)
	uint8_to_sysex1Byte(data[249], sysexCount, Syx_Buf);		// (249) 1	399 - VCA PAN (0...127)
	float_to_sysex5Bytes(data[250], sysexCount, Syx_Buf);		// (250) 5	389 - ModWheelCutoffAmt (float)
	float_to_sysex5Bytes(data[251], sysexCount, Syx_Buf);		// (255) 5	406 - ModWheelCutoffAmt (float)
	Syx_Buf[sysexCount++] = 0xF7;								//		 1	411 - End SysEx (0xF7)
	if (sysexCount < NO_OF_SysEx_Data) {
		Serial.print("Error File: ");
		Serial.println(fileString);
	}
	
	// Send SysEx Dump
	usbMIDI.sendSysEx(sysexCount, Syx_Buf, true);				// send SysEx Dump via USB	
}

//*************************************************************************
// update send SysEx Data
//*************************************************************************
FLASHMEM void update_Sendsysex (void)
{
	boolean ProgBar_enabled;
	uint8_t ProgBar_style;
	static uint8_t Patch_count = 0;
	uint8_t ProgBar_count;
	uint8_t ProgBar_maxcount;
	String ProgBar_string;
	uint8_t send_speed = 126;
	
	if ((millis() - timer_sysexwait) < send_speed) {
		return;
	}
	timer_sysexwait = millis();
	
	if (sendSysExFlag == true){
		
		// send a Patch via USB -------------------------------------------
		if (sysexDest == false && sysexTyp == false) {
			
			// set progressbar inits
			ProgBar_enabled = true;
			ProgBar_style = 1;
			ProgBar_count = 127;
			ProgBar_maxcount = 127;
			
			// send SysEx data
			mySendSysEx(sysexSource, sysexBank);
			//usb_midi_flush_output();
			
			// set ProgressBar
			ProgBar_string = "Send: " + patchname;
			setProgressbar(ProgBar_enabled, ProgBar_style, ProgBar_count, ProgBar_maxcount, ProgBar_string);
			
			// sendSysExFlag is set by Key query and will be deleted here
			sendSysExFlag = false;
		}
		
		// send Patch Bank via USB ----------------------------------------
		else if (sysexDest == false && sysexTyp == true) {
			
			// set progressbar inits
			ProgBar_enabled = true;
			ProgBar_style = 1;
			ProgBar_count = Patch_count;
			ProgBar_maxcount = 127;
			
			// send SysEx data
			mySendSysEx(Patch_count, sysexBank);
			usb_midi_flush_output();
			
			// set ProgressBar
			ProgBar_string = "Send: " + patchname;
			setProgressbar(ProgBar_enabled, ProgBar_style, ProgBar_count, ProgBar_maxcount, ProgBar_string);
			
			// inc PatchNo
			Patch_count++;
			if (Patch_count >= 128) {
				Patch_count = 0;
				
				// sendSysExFlag is set by Key query and will be deleted here
				sendSysExFlag = false;
			}
		}
	}
}

//*************************************************************************
// update CPU Temp. und Audio Por. Usage
//*************************************************************************
FLASHMEM void updateTempAudioUsage (void)
{
	if ((millis() - timer_Temperature) > 1000){
		static uint8_t count = 0;
		CPUdegree_temp += tempmonGetTemp();
		count++;
		if (count >= 32 ) {
			CPUdegree = (CPUdegree_temp >> 5);
			printTemperature();
			CPUdegree_temp = 0;
			count = 0;
		}
		timer_Temperature = millis();
	}
	
	if ((millis() - timer_CPUmon) > 500){
		if (PageNr == 12 && myPageShiftStatus[12] == false) {
			CPUaudioMem = AudioProcessorUsageMax(); //AudioMemoryUsageMax();
			CPUmem = AudioMemoryUsageMax(); //AudioMemoryUsageMax();
			printCPUmon();
			AudioProcessorUsageMaxReset(); //AudioMemoryUsageMaxReset();
			AudioMemoryUsageMaxReset();
		}
		// disable scope line if envelopes is stoped
		if (PageNr == 0 || PageNr == 12) {
			if (ampEnvelope[0].isActive() == false &&
			ampEnvelope[1].isActive() == false &&
			ampEnvelope[2].isActive() == false &&
			ampEnvelope[3].isActive() == false &&
			ampEnvelope[4].isActive() == false &&
			ampEnvelope[5].isActive() == false &&
			ampEnvelope[6].isActive() == false &&
			ampEnvelope[7].isActive() == false) {
				EnvIdelFlag = false;
				if (EnvelopeIdle == false) {
					EnvelopeIdle = true;
				}
				} else {
				EnvelopeIdle = false;
			}
		}
		
		// Unisono LED blink
		if (unison == 2) {
			uint8_t LED_bit_pos = 0b00000010;
			if (unisoFlashStatus == 0) {
				unisoFlashStatus = 1;
				HC595Register &= ~(LED_bit_pos);
				SPI.transfer(HC595Register);
				digitalWrite(CS_HC595, LOW);
				digitalWrite(CS_HC595, HIGH);
				SPI.endTransaction();
			}
			else {
				unisoFlashStatus = 0;
				HC595Register |= LED_bit_pos;
				SPI.transfer(HC595Register);
				digitalWrite(CS_HC595, LOW);
				digitalWrite(CS_HC595, HIGH);
				SPI.endTransaction();
			}
		}
		timer_CPUmon = millis();
	}
}

//*************************************************************************
// update LED Status
//*************************************************************************
FLASHMEM void updateLEDstatus (void)
{
	static long LED_state_time = 0;
	uint8_t LED_bit_pos = 0;
	boolean change_state = false;
	
	if ((millis() - LED_state_time) > 20) {
		LED_state_time = millis();
		
		// FX LED
		if (KeyLED1State == true) {
			LED_bit_pos = 0b00000001;
			if (FxPrgNo > 0) {
				HC595Register |= LED_bit_pos;
			}
			else {
				HC595Register &= ~(LED_bit_pos);
			}
			KeyLED1State = false;
			change_state = true;
		}
		
		// Unisono LED
		if (KeyLED2State == true) {
			LED_bit_pos = 0b00000010;
			if (unison == 0) {
				HC595Register &= ~(LED_bit_pos);
				} else if (unison == 1) {
				HC595Register |= LED_bit_pos;
				} else {
				HC595Register |= LED_bit_pos;
			}
			KeyLED2State = false;
			change_state = true;
		}
		
		// Tempo LED
		if (TempoLEDchange == true) {
			LED_bit_pos = 0b00000100;
			if (TempoLEDstate == true) {
				HC595Register |= LED_bit_pos;
			}
			else {
				HC595Register &= ~(LED_bit_pos);
			}
			TempoLEDchange = false;
			change_state = true;
		}
		
		// Boost LED
		if (KeyLED4State == true) {
			LED_bit_pos = 0b00001000;
			if (myBoost == 1) {
				HC595Register |= LED_bit_pos;
			}
			else {
				HC595Register &= ~(LED_bit_pos);
			}
			KeyLED4State = false;
			change_state = true;
		}
		
		// set LED register and transmit
		if (change_state == true) {
			SPI.transfer(HC595Register);
			digitalWrite(CS_HC595, LOW);
			digitalWrite(CS_HC595, HIGH);
			SPI.endTransaction();
		}
	}
}

//*************************************************************************
FLASHMEM void clearSequencerData (void)
{
	allNotesOff();
	clearPatternData();
	SEQselectStepNo = 0;
	SeqNotesAvailable = false;
	SeqRecNoteCount = 0;
	renderCurrentPatchPage();
}


//*************************************************************************
// Encoder Query
//*************************************************************************
FLASHMEM void EncoderQuery(void)
{
	encValue = encoder.read() >> 2;
	if (encValue != previousValue) {
		previousValue = encValue;
		encoderflag = true;
	}
}

//*************************************************************************
// mute Sound before load new patch and set mute timer8
//*************************************************************************
FLASHMEM void mute_before_load_patch(void)
{
	// all voice off
	for (uint8_t i = 0; i < 8; i++) {
		endVoice(i);
	}
	
	ampEnvelopes_off();
	setFxPrg(0);
	timer8 = millis();
	mute_sound = true;
}


//*************************************************************************
// update Sound mute time
//*************************************************************************
FLASHMEM void update_Sound_mute(void)
{	
	if (mute_sound == true) {				// If you switch a patch, mute_sound = true
		if ((millis() - timer8) > t8_value){			
			recallPatch(patchNo);	
			storeSoundPatchNo(patchNo);
			storePatchBankNo(currentPatchBank);
			RefreshMainScreenFlag = true;
			setFxPrg (FxPrgNo);
			updateFxChip();
			updateBoost();
			KeyLED1State = true;
			KeyLED2State = true;
			KeyLED3State = true;
			KeyLED4State = true;
			// clear voices and note stack
			for (int i = 0; i < NO_OF_VOICES; i++) {
				voices[i].voiceOn = 0;
				NoteStack_pool[i] = 0xFF;
				NoteStack_ptr = 0;
			}		
			mute_sound = false;	// clr mute flag
		}
	}
}

//*************************************************************************
// Midi Timer Interrupt
//*************************************************************************
FLASHMEM void MidiClockTimer (void)
{
	if (clrSEQflag == true) {
		return;
	}
	
	//read Midi datas
	MIDI.read(midiChannel);				// MIDI 5 Pin DIN
	usbMIDI.read(midiChannel);			// usbMidi
	
	// play Sequencer notes
	polySequencer();
	
	// update LFO3 Modulation 
	updateLfo3FxMod();
	
	// Encoder Query
	EncoderQuery();
	
}

//*************************************************************************
// Main Loop
//*************************************************************************
FLASHMEM void loop(void) {
	
	// Special function for Midi CC PrgChange & Bank select----------------
	myPrgChange();
	myMidiBankSel();
	
	// Potentiometer query ------------------------------------------------
	checkPots();
	
	// Encoder query ------------------------------------------------------
	checkEncoder();
	
	// Mute time for switch Patch
	update_Sound_mute();
	
	// Key query ----------------------------------------------------------
	checkSwitches();

	// update mySendSysEx -------------------------------------------------
	update_Sendsysex();
	
	// update Step frame into Sequencer -----------------------------------
	if (PageNr == 11 && SEQStepStatus == true) {
		drawSEQStepFrame(SEQselectStepNo);
		SEQStepStatus = false;
	}
	
	// clear Sequencer data -----------------------------------------------
	if (clrSEQflag == true) {
		clearSequencerData();
		clrSEQflag = false;
	}
	
	// draw Sequencer recording Notes -------------------------------------
	if (SEQRecNoteFlag == true) {
		drawSEQRecNotes();
		SEQRecNoteFlag = false;
	}
	
	// refresh screen -----------------------------------------------------
	displayThread();
	
	// update LED Status --------------------------------------------------
	updateLEDstatus();
	
	// read CPU Temp and AudioPro. usage ----------------------------------
	updateTempAudioUsage();
}

