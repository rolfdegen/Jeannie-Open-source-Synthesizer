//*************************************************************************
//  8 Voices DIY Synthsizer "Jeannie"
//
//  Rolf Degen, Andre Laska (tubeohm.com)
//
//  Free Software by Rolf Degen & http://electrotechnique.cc/
// //
//  Build Version 1.43 with State Variable Filter and Ladder Filter
//  Info: Filter type can switch on the System menu
//
//  Date: 22.04.2022
//  Teensy 4.1 Development Board
//  ARM Cortex-M7 CPU 720MHz 1024K RAM  8MB Flash 4K EEPROM
//*************************************************************************

#include "Audio.h" //Using local version to override Teensyduino version

#include "MidiCC.h"
#include "effect_waveshaper.h"
#include <Entropy.h>
#include <MIDI.h>
#include <Mcp320x.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <usb_midi.h>

//#include "effect_envelope.h"
#include "Constants.h"
#include "Parameters.h"
#include "AudioPatching.h"
#include "Detune.h"
#include "EepromMgr.h"
#include "HWControls.h"
#include "PatchMgr.h"
#include "Velocity.h"
#include "global.h"

// 12Bit ADC MCP3208
#define SPI_CS 10       // SPI slave select
#define ADC_VREF 256    // 12Bit Resolution
#define ADC_CLK 1200000 // SPI clock (max 1MHz 2.7V)
MCP3208 MCP_adc(ADC_VREF, SPI_CS);

#define PARAMETER                                                              \
  0 // The main page for displaying the current patch and control (parameter)
    // changes
#define RECALL 1        // Patches list
#define SAVE 2          // Save patch page
#define REINITIALISE 3  // Reinitialise message
#define PATCH 4         // Show current patch bypassing PARAMETER
#define PATCHNAMING 5   // Patch naming page
#define DELETE 6        // Delete patch page
#define DELETEMSG 7     // Delete patch message page
#define SETTINGS 8      // Settings page
#define SETTINGSVALUE 9 // Settings page
#define Pages 11        // Menu Pages
#define OscFxLed 1
#define UnisonoLED 2
#define TempoLED 3

uint32_t state = PARAMETER;
int PageNr = 0;     // global
int PageNr_old = 0; // global
int LastPageNr = 0;
int Number_of_Pages = 10;
uint8_t ParameterNrMem[11];
uint8_t ParameterNr = 0;
boolean tftUpdate = true;
boolean ParUpdate = true;
boolean Init_flag = true;
long timer_Temperature = 2011;      // Measurement interval time (ms)
long timer_CPUmon = 577;            // Measurement interval time (ms)
long timer_keyquery = 1;            // Key query interval time 1ms
long timer_encquery = 20;           // Encoder query interval time 1ms
long timer_potquery = 50;           // pot query interval time 50ms
uint32_t timer_intMidiClk = 0;      // interval Intern Midi Clock ms
unsigned long timer_extMidiClk = 0; // interval Intern Midi Clock ms
unsigned long timer_envelopeMonitor = 0;
int WaveNr = 0;
uint8_t Osc1WaveBank = 0;
uint8_t Osc2WaveBank = 0;
uint8_t FilterTyp = 0; // LowPass
uint8_t FilterEnv = 0;
uint8_t FilterCut = 0; // Cutoff level for drawing	filter curve
uint8_t FilterRes = 0; // Resonance level for drawing Filter curve
uint8_t FilterMix = 0;
uint8_t LadderFilterDrive = 0;
uint8_t LadderFilterpassbandgain = 0;
uint8_t Env1Atk = 0;
uint8_t Env1Dcy = 0;
uint8_t Env1Sus = 0;
uint8_t Env1Rel = 0;
uint8_t Env2Atk = 0;
uint8_t Env2Dcy = 0;
uint8_t Env2Sus = 0;
uint8_t Env2Rel = 0;
uint16_t KeyDebounce = 2;
uint8_t KeyStatus = 0;
uint8_t S1KeyStatus = 0;
uint8_t S2KeyStatus = 0;
uint8_t S3KeyStatus = 0;
uint8_t S4KeyStatus = 0;
uint8_t S5KeyStatus = 0;
uint8_t S6KeyStatus = 0;
uint8_t S7KeyStatus = 0;
uint8_t HC595Register = 0;
uint8_t FxPrgNo = 0;
uint8_t FxMixValue = 0;
uint8_t FxTimeValue = 0;
uint8_t FxFeedbackValue = 0;
// uint8_t FxParameter3Value = 0;
uint8_t FxPot1value = 0;
uint8_t FxPot2value = 0;
uint8_t FxPot3value = 0;
uint8_t myFxSelValue = 0;
uint8_t myFxValValue = 0;
uint16_t FxClkRate = 32000;
uint8_t myLFO1shapeNo = 0;
uint8_t myLFO2shapeNo = 0;

uint8_t MidiStatusSymbol = 0; // 0:off, 1:on; 2: wait to off
uint16_t MidiStatusHoldTime = 0;
Bounce btnBouncer = Bounce(PAGE_SW, 50);
boolean PAGE_SW_Status = 1;
boolean BassBoostStatus = 0;
uint8_t myBoost = 0;

boolean initStatus = 1;
// boolean initStatus2 = 1;
boolean unisoFlashStatus = 0;
float oscGainLevel = 1.0;
const static uint32_t WAVEFORM_PARABOLIC = 103;
const static uint32_t WAVEFORM_HARMONIC = 104;

struct VoiceAndNote {
  uint32_t note;
  long timeOn;
  uint32_t voiceOn;
};

// 8 voices
struct VoiceAndNote voices[NO_OF_VOICES] = {{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0},
                                            {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0},
                                            {-1, 0, 0}, {-1, 0, 0}};
uint32_t notesOn = 0;

#include "ST7735Display.h"
#include "Settings.h"

boolean cardStatus = false;
boolean firstPatchLoaded = false;

// MIDI 5 Pin DIN
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
/*
USB HOST MIDI Class Compliant
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
MIDIDevice midi1(myusb);
*/

int prevNote = 48; // This is for glide to use previous note to glide from
uint8_t UnisonoNote = 0;
float previousMillis = millis(); // For MIDI Clk Sync

uint32_t count = 0; // For MIDI Clk Sync
// int patchNo = 1;//Current patch no
uint32_t voiceToReturn = -1;  // Initialise
long earliestTime = millis(); // For voice allocation - initialise to now
boolean ParmSelectFlag = false;
int ADC_Ch1 = 0;
int ADC_Ch2 = 0;
int ADC_Ch3 = 0;
int ADC_Ch4 = 0;
int ADC_Ch5 = 0;
int ADC_Ch6 = 0;
int ADC_Ch7 = 0;
int ADC_Ch8 = 0;

IntervalTimer myMidiTimer; // Midi and Sequencer
IntervalTimer myEncTimer;  // Enc query

boolean SEQLed = false;        // Sequencer Lamp
boolean SEQLedStatus = false;  // Sequencer Lamp
boolean SEQStepStatus = false; // Sequencer red frame
boolean TempoLEDchange = false;
boolean TempoLEDstate = false;
boolean myPrgChangeFlag = false;
uint8_t myPrgChangeChannel = 0;
uint8_t myPrgChangeProgram = 0;

// test free ram -----------------------------------------------------
#define NUM_SHOW sizeofFreeITCM
uint32_t *ptrFreeITCM;   // Set to Usable ITCM free RAM
uint32_t sizeofFreeITCM; // sizeof free RAM in uint32_t units.
uint32_t SizeLeft_etext;
extern unsigned long _stextload; // FROM LINKER
extern unsigned long _stext;
extern unsigned long _etext;
boolean voiceLEDflag = false;

//*************************************************************************
// Setup
//*************************************************************************
FLASHMEM void setup() {

  Serial.begin(9600);
  setupDisplay();
  setupHardware();
  Entropy.Initialize(); // Random generator
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);
  SPI.begin();
  SPI.beginTransaction(settings);
  initPWMFx();
  initHC595();
  setLED(1, false);
  setLED(2, false);
  setLED(3, false);
  setLED(4, false);

  // Audio Memory buffer ------------------------------------------------
  AudioMemory(128); // Sample Blocks

  // init Waveforms -----------------------------------------------------
  constant1Dc.amplitude(ONE);
  pwmLfoA.amplitude(ONE);
  pwmLfoA.begin(PWMWAVEFORM);
  pwmLfoB.amplitude(ONE);
  pwmLfoB.begin(PWMWAVEFORM);
  pwmLfoB.phase(10.0f); // Off set phase of second osc

  filterMixer1.gain(1, -1.0f);

  waveformMod1a.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod1a.begin(WAVEFORMLEVEL, 440.0f, oscWaveformA);
  waveformMod1b.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod1b.begin(WAVEFORMLEVEL, 440.0f, oscWaveformB);
  waveformMod2a.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod2a.begin(WAVEFORMLEVEL, 440.0f, oscWaveformA);
  waveformMod2b.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod2b.begin(WAVEFORMLEVEL, 440.0f, oscWaveformB);
  waveformMod3a.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod3a.begin(WAVEFORMLEVEL, 440.0f, oscWaveformA);
  waveformMod3b.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod3b.begin(WAVEFORMLEVEL, 440.0f, oscWaveformB);
  waveformMod4a.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod4a.begin(WAVEFORMLEVEL, 440.0f, oscWaveformA);
  waveformMod4b.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod4b.begin(WAVEFORMLEVEL, 440.0f, oscWaveformB);
  waveformMod5a.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod5a.begin(WAVEFORMLEVEL, 440.0f, oscWaveformA);
  waveformMod5b.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod5b.begin(WAVEFORMLEVEL, 440.0f, oscWaveformB);
  waveformMod6a.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod6a.begin(WAVEFORMLEVEL, 440.0f, oscWaveformA);
  waveformMod6b.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod6b.begin(WAVEFORMLEVEL, 440.0f, oscWaveformB);
  waveformMod7a.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod7a.begin(WAVEFORMLEVEL, 440.0f, oscWaveformA);
  waveformMod7b.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod7b.begin(WAVEFORMLEVEL, 440.0f, oscWaveformB);
  waveformMod8a.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod8a.begin(WAVEFORMLEVEL, 440.0f, oscWaveformA);
  waveformMod8b.frequencyModulation(PITCHLFOOCTAVERANGE);
  waveformMod8b.begin(WAVEFORMLEVEL, 440.0f, oscWaveformB);

  // Arbitary waveform needs initialising to something
  loadArbWaveformA(PARABOLIC_WAVE);
  loadArbWaveformB(PARABOLIC_WAVE);
  voiceMixerM.gain(0, VOICEMIXERLEVEL);
  voiceMixerM.gain(1, VOICEMIXERLEVEL);
  voiceMixerM.gain(2, 0);
  voiceMixerM.gain(3, 0);
  filterModMixer1.gain(0, 0);
  filterModMixer2.gain(0, 0);
  filterModMixer3.gain(0, 0);
  filterModMixer4.gain(0, 0);
  filterModMixer5.gain(0, 0);
  filterModMixer6.gain(0, 0);
  filterModMixer7.gain(0, 0);
  filterModMixer8.gain(0, 0);
  filterModMixer1.gain(1, 0.5);
  filterModMixer2.gain(1, 0.5);
  filterModMixer3.gain(1, 0.5);
  filterModMixer4.gain(1, 0.5);
  filterModMixer5.gain(1, 0.5);
  filterModMixer6.gain(1, 0.5);
  filterModMixer7.gain(1, 0.5);
  filterModMixer8.gain(1, 0.5);
  filterModMixer1.gain(2, 0);
  filterModMixer2.gain(2, 0);
  filterModMixer3.gain(2, 0);
  filterModMixer4.gain(2, 0);
  filterModMixer5.gain(2, 0);
  filterModMixer6.gain(2, 0);
  filterModMixer7.gain(2, 0);
  filterModMixer8.gain(2, 0);
  filterModMixer1.gain(3, 0);
  filterModMixer2.gain(3, 0.5);
  filterModMixer3.gain(3, 0.5);
  filterModMixer4.gain(3, 0.5);
  filterModMixer5.gain(3, 0.5);
  filterModMixer6.gain(3, 0.5);
  filterModMixer7.gain(3, 0.5);
  filterModMixer8.gain(3, 0.5);
  keytracking1.amplitude(1.0f);
  keytracking2.amplitude(1.0f);
  keytracking3.amplitude(1.0f);
  keytracking4.amplitude(1.0f);
  keytracking5.amplitude(1.0f);
  keytracking6.amplitude(1.0f);
  keytracking7.amplitude(1.0f);
  keytracking8.amplitude(1.0f);
  FilterVelo1.amplitude(0.5f);
  FilterVelo2.amplitude(0.5f);
  FilterVelo3.amplitude(0.5f);
  FilterVelo4.amplitude(0.5f);
  FilterVelo5.amplitude(0.5f);
  FilterVelo6.amplitude(0.5f);
  FilterVelo7.amplitude(0.5f);
  FilterVelo8.amplitude(0.5f);

  pitchLfo.amplitude(1.0f);

  // This removes dc offset for Scope and Peak function
  dcOffsetFilter.octaveControl(1.0f);
  dcOffsetFilter.frequency(5.0f);

  // init Waveshaper
  waveshaper1.shape(WAVESHAPER_TABLE1, 3);
  waveshaper2.shape(WAVESHAPER_TABLE1, 3);
  waveshaper3.shape(WAVESHAPER_TABLE1, 3);
  waveshaper4.shape(WAVESHAPER_TABLE1, 3);
  waveshaper5.shape(WAVESHAPER_TABLE1, 3);
  waveshaper6.shape(WAVESHAPER_TABLE1, 3);
  waveshaper7.shape(WAVESHAPER_TABLE1, 3);
  waveshaper8.shape(WAVESHAPER_TABLE1, 3);
  WaveshaperAmp1.gain(1.0f);
  WaveshaperAmp2.gain(1.0f);
  WaveshaperAmp3.gain(1.0f);
  WaveshaperAmp4.gain(1.0f);
  WaveshaperAmp5.gain(1.0f);
  WaveshaperAmp6.gain(1.0f);
  WaveshaperAmp7.gain(1.0f);
  WaveshaperAmp8.gain(1.0f);

  // special releaseNoteOn Phase for Attack
  float time = 4.0f;
  filterEnvelope1.releaseNoteOn(time);
  filterEnvelope2.releaseNoteOn(time);
  filterEnvelope3.releaseNoteOn(time);
  filterEnvelope4.releaseNoteOn(time);
  filterEnvelope5.releaseNoteOn(time);
  filterEnvelope6.releaseNoteOn(time);
  filterEnvelope7.releaseNoteOn(time);
  filterEnvelope8.releaseNoteOn(time);
  ampEnvelope1.releaseNoteOn(time);
  ampEnvelope2.releaseNoteOn(time);
  ampEnvelope3.releaseNoteOn(time);
  ampEnvelope4.releaseNoteOn(time);
  ampEnvelope5.releaseNoteOn(time);
  ampEnvelope6.releaseNoteOn(time);
  ampEnvelope7.releaseNoteOn(time);
  ampEnvelope8.releaseNoteOn(time);

  LFO1Envelope1.releaseNoteOn(time);
  LFO1Envelope2.releaseNoteOn(time);
  LFO1Envelope3.releaseNoteOn(time);
  LFO1Envelope4.releaseNoteOn(time);
  LFO1Envelope5.releaseNoteOn(time);
  LFO1Envelope6.releaseNoteOn(time);
  LFO1Envelope7.releaseNoteOn(time);
  LFO1Envelope8.releaseNoteOn(time);

  LFO2Envelope1.releaseNoteOn(time);
  LFO2Envelope2.releaseNoteOn(time);
  LFO2Envelope3.releaseNoteOn(time);
  LFO2Envelope4.releaseNoteOn(time);
  LFO2Envelope5.releaseNoteOn(time);
  LFO2Envelope6.releaseNoteOn(time);
  LFO2Envelope7.releaseNoteOn(time);
  LFO2Envelope8.releaseNoteOn(time);

  // set Envelupe curve
  int8_t envelopeType =
      +8; // 0 linear, -8 fast exponential, -8 slow exponential
  ampEnvelope1.setEnvType(envelopeType);
  ampEnvelope2.setEnvType(envelopeType);
  ampEnvelope3.setEnvType(envelopeType);
  ampEnvelope4.setEnvType(envelopeType);
  ampEnvelope5.setEnvType(envelopeType);
  ampEnvelope6.setEnvType(envelopeType);
  ampEnvelope7.setEnvType(envelopeType);
  ampEnvelope8.setEnvType(envelopeType);
  filterEnvelope1.setEnvType(envelopeType);
  filterEnvelope2.setEnvType(envelopeType);
  filterEnvelope3.setEnvType(envelopeType);
  filterEnvelope4.setEnvType(envelopeType);
  filterEnvelope5.setEnvType(envelopeType);
  filterEnvelope6.setEnvType(envelopeType);
  filterEnvelope7.setEnvType(envelopeType);
  filterEnvelope8.setEnvType(envelopeType);
  LFO1Envelope1.setEnvType(envelopeType);
  LFO1Envelope2.setEnvType(envelopeType);
  LFO1Envelope3.setEnvType(envelopeType);
  LFO1Envelope4.setEnvType(envelopeType);
  LFO1Envelope5.setEnvType(envelopeType);
  LFO1Envelope6.setEnvType(envelopeType);
  LFO1Envelope7.setEnvType(envelopeType);
  LFO1Envelope8.setEnvType(envelopeType);
  LFO2Envelope1.setEnvType(envelopeType);
  LFO2Envelope2.setEnvType(envelopeType);
  LFO2Envelope3.setEnvType(envelopeType);
  LFO2Envelope4.setEnvType(envelopeType);
  LFO2Envelope5.setEnvType(envelopeType);
  LFO2Envelope6.setEnvType(envelopeType);
  LFO2Envelope7.setEnvType(envelopeType);
  LFO2Envelope8.setEnvType(envelopeType);

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
  LFO1envModMixer1.gain(0, 0.125f);
  LFO1envModMixer1.gain(1, 0.125f);
  LFO1envModMixer1.gain(2, 0.125f);
  LFO1envModMixer1.gain(3, 0.125f);
  LFO1envModMixer2.gain(0, 0.125f);
  LFO1envModMixer2.gain(1, 0.125f);
  LFO1envModMixer2.gain(2, 0.125f);
  LFO1envModMixer2.gain(3, 0.125f);

  // disabled Filter FM
  filterModMixer1b.gain(1, 0.0f);
  filterModMixer2b.gain(1, 0.0f);
  filterModMixer3b.gain(1, 0.0f);
  filterModMixer4b.gain(1, 0.0f);
  filterModMixer5b.gain(1, 0.0f);
  filterModMixer6b.gain(1, 0.0f);
  filterModMixer7b.gain(1, 0.0f);
  filterModMixer8b.gain(1, 0.0f);

  // enabled Filter-Envelop, -LFO, -KeyTraking, -Velocity
  filterModMixer1b.gain(0, 1.0);
  filterModMixer2b.gain(0, 1.0);
  filterModMixer3b.gain(0, 1.0);
  filterModMixer4b.gain(0, 1.0);
  filterModMixer5b.gain(0, 1.0);
  filterModMixer6b.gain(0, 1.0);
  filterModMixer7b.gain(0, 1.0);
  filterModMixer8b.gain(0, 1.0);

  // init USB Midi ------------------------------------------------------
  usbMIDI.begin();
  // usbMIDI.setHandleNoteOff(myNoteOff);
  // usbMIDI.setHandleNoteOn(myNoteOn);
  /*
  usbMIDI.setHandleAfterTouchPoly(myAfterTouchPoly)
  usbMIDI.setHandleControlChange(myControlChange)
  usbMIDI.setHandleProgramChange(myProgramChange)
  usbMIDI.setHandleAfterTouch(myAfterTouch)
  usbMIDI.setHandlePitchChange(myPitchChange)
  */

  // usbMIDI.setHandleSystemExclusive(mySystemExclusiveChunk);
  /*
  usbMIDI.setHandleTimeCodeQuarterFrame(myTimeCodeQuarterFrame);
  usbMIDI.setHandleSongPosition(mySongPosition);
  usbMIDI.setHandleSongSelect(mySongSelect);
  usbMIDI.setHandleTuneRequest(myTuneRequest);
  usbMIDI.setHandleClock(myClock);
  usbMIDI.setHandleStart(myStart);
  usbMIDI.setHandleContinue(myContinue);
  usbMIDI.setHandleStop(myStop);
  usbMIDI.setHandleActiveSensing(myActiveSensing);
  usbMIDI.setHandleSystemReset(mySystemReset);
  usbMIDI.setHandleRealTimeSystem(myRealTimeSystem);
  */

  // init Midi 5pol -----------------------------------------------------
  MIDI.begin();
  MIDI.setHandleNoteOn(myNoteOn);
  MIDI.setHandleNoteOff(myNoteOff);
  MIDI.setHandlePitchBend(myPitchBend);
  MIDI.setHandleControlChange(myControlChange1);
  MIDI.setHandleProgramChange(myProgramChange);
  MIDI.setHandleClock(myMIDIClock);
  MIDI.setHandleStart(myMIDIClockStart);
  MIDI.setHandleContinue(myMIDIClockStart);
  MIDI.setHandleStop(myMIDIClockStop);
  Serial.println(F("MIDI In DIN Listening"));
  // Read MIDI Channel from EEPROM
  midiChannel = getMIDIChannel();
  Serial.println("MIDI Ch:" + String(midiChannel) + " (0 is Omni On)");
  // timer_intMidiClk = micros();		// interval Intern Midi Clock ms
  myMidiTimer.begin(MidiClockTimer, 1000); // Timer interrupt Midi and Seq
  myMidiTimer.priority(100);               // Timer interrupt priority
  myEncTimer.begin(EncClockTimer, 20000);  // Encoder Timer interrupt
  myEncTimer.priority(101);
  getMidiCkl();
  MIDIClkSignal = true;
  filterLFOMidiClkSync = true;

  // init Voices --------------------------------------------------------

  voiceMixerM.gain(0, 0.0f);
  voiceMixerM.gain(1, 0.0f);
  voiceMixerM.gain(2, 0.0f);
  voiceMixerM.gain(3, 0.0f);

  for (uint8_t i = 0; i < 8; i++) {
    myNoteOn(0, 64 + i, 0);
  }
  for (uint8_t i = 0; i < 8; i++) {
    myNoteOff(0, 64 + i, 0);
  }

  voiceMixerM.gain(0, VOICEMIXERLEVEL);
  voiceMixerM.gain(1, VOICEMIXERLEVEL);
  voiceMixerM.gain(2, -0.0000155f); // little DC level (1Bit) into MixerM.
                                    // Prevents clicks when muting the DAC
  voiceMixerM.gain(3, 0);

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
    delay(200);
    // System Reset
    SCB_AIRCR = 0x05F10465;
  } else {
    Serial.println("EEPROM data: ok!");
  }

  // load Global datas from EEPROM --------------------------------------
  getMIDIChannel();
  getPRGchange();

  // load Sound Programm ------------------------------------------------
  mux5Read = MCP_adc.read(MCP3208::Channel::SINGLE_7);
  uint8_t value = mux5Read >> 5;

  // If the "Mute" button is pressed when switching on, then the system boots
  // with patch A000
  if (value < (S4 + hysteresis) && value > (S4 - hysteresis)) {

  } else {
    initPatternData();
    Init_Patch();
  }

  // read Temp.
  CPUdegree = tempmonGetTemp();

  // freeMem Monitor ----------------------------------------------------
  Serial.println();
  flexRamInfo();

  // init potentiometer -------------------------------------------------
  pot_init();
}

FLASHMEM void updateLFO1delay() {

  float t_delay = 0;
  LFO1Envelope1.delay(t_delay);
  LFO1Envelope2.delay(t_delay);
  LFO1Envelope3.delay(t_delay);
  LFO1Envelope4.delay(t_delay);
  LFO1Envelope5.delay(t_delay);
  LFO1Envelope6.delay(t_delay);
  LFO1Envelope7.delay(t_delay);
  LFO1Envelope8.delay(t_delay);
}

FLASHMEM void updateLFO2delay() {

  float t_delay = 0;
  LFO2Envelope1.delay(t_delay);
  LFO2Envelope2.delay(t_delay);
  LFO2Envelope3.delay(t_delay);
  LFO2Envelope4.delay(t_delay);
  LFO2Envelope5.delay(t_delay);
  LFO2Envelope6.delay(t_delay);
  LFO2Envelope7.delay(t_delay);
  LFO2Envelope8.delay(t_delay);
}

FLASHMEM void updateLFO1fade() {

  float t_fade = LFO1fadeTime;
  LFO1Envelope1.attack(t_fade);
  LFO1Envelope2.attack(t_fade);
  LFO1Envelope3.attack(t_fade);
  LFO1Envelope4.attack(t_fade);
  LFO1Envelope5.attack(t_fade);
  LFO1Envelope6.attack(t_fade);
  LFO1Envelope7.attack(t_fade);
  LFO1Envelope8.attack(t_fade);
}

FLASHMEM void updateLFO2fade() {

  float t_fade = LFO2fadeTime;
  LFO2Envelope1.attack(t_fade);
  LFO2Envelope2.attack(t_fade);
  LFO2Envelope3.attack(t_fade);
  LFO2Envelope4.attack(t_fade);
  LFO2Envelope5.attack(t_fade);
  LFO2Envelope6.attack(t_fade);
  LFO2Envelope7.attack(t_fade);
  LFO2Envelope8.attack(t_fade);
}

FLASHMEM void updateLFO1decay() {

  float t_decay = 0.0f;
  LFO1Envelope1.decay(t_decay);
  LFO1Envelope2.decay(t_decay);
  LFO1Envelope3.decay(t_decay);
  LFO1Envelope4.decay(t_decay);
  LFO1Envelope5.decay(t_decay);
  LFO1Envelope6.decay(t_decay);
  LFO1Envelope7.decay(t_decay);
  LFO1Envelope8.decay(t_decay);
}

FLASHMEM void updateLFO2decay() {

  float t_decay = 0.0f;
  LFO2Envelope1.decay(t_decay);
  LFO2Envelope2.decay(t_decay);
  LFO2Envelope3.decay(t_decay);
  LFO2Envelope4.decay(t_decay);
  LFO2Envelope5.decay(t_decay);
  LFO2Envelope6.decay(t_decay);
  LFO2Envelope7.decay(t_decay);
  LFO2Envelope8.decay(t_decay);
}

FLASHMEM void updateLFO1sustain() {

  LFO1Envelope1.sustain(1.0f);
  LFO1Envelope2.sustain(1.0f);
  LFO1Envelope3.sustain(1.0f);
  LFO1Envelope4.sustain(1.0f);
  LFO1Envelope5.sustain(1.0f);
  LFO1Envelope6.sustain(1.0f);
  LFO1Envelope7.sustain(1.0f);
  LFO1Envelope8.sustain(1.0f);
}

FLASHMEM void updateLFO2sustain() {

  LFO2Envelope1.sustain(1.0f);
  LFO2Envelope2.sustain(1.0f);
  LFO2Envelope3.sustain(1.0f);
  LFO2Envelope4.sustain(1.0f);
  LFO2Envelope5.sustain(1.0f);
  LFO2Envelope6.sustain(1.0f);
  LFO2Envelope7.sustain(1.0f);
  LFO2Envelope8.sustain(1.0f);
}

FLASHMEM void updateLFO1release() {

  float t_release = LFO1releaseTime;

  if (LFO1releaseTime > ampRelease) {
    t_release = ampRelease;
  }
  LFO1Envelope1.release(t_release);
  LFO1Envelope2.release(t_release);
  LFO1Envelope3.release(t_release);
  LFO1Envelope4.release(t_release);
  LFO1Envelope5.release(t_release);
  LFO1Envelope6.release(t_release);
  LFO1Envelope7.release(t_release);
  LFO1Envelope8.release(t_release);
}

FLASHMEM void updateLFO2release() {

  float t_release = LFO2releaseTime;

  if (LFO2releaseTime > ampRelease) {
    t_release = ampRelease;
  }
  LFO2Envelope1.release(t_release);
  LFO2Envelope2.release(t_release);
  LFO2Envelope3.release(t_release);
  LFO2Envelope4.release(t_release);
  LFO2Envelope5.release(t_release);
  LFO2Envelope6.release(t_release);
  LFO2Envelope7.release(t_release);
  LFO2Envelope8.release(t_release);
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
//
//*************************************************************************
FLASHMEM void pot_init() {

  mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
  mux1ValuesPrev[0] = mux1Read;
  mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
  mux3ValuesPrev[0] = mux3Read;
  mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
  mux3ValuesPrev[0] = mux3Read;
  mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
  mux4ValuesPrev[0] = mux4Read;
  // read Pot1 (change Patch Bank)
  mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
  if (mux1Read > (mux1ValuesPrev[0] + (QUANTISE_FACTOR * 4)) ||
      mux1Read < (mux1ValuesPrev[0] - (QUANTISE_FACTOR * 4))) {
    mux1ValuesPrev[0] = mux1Read;
  }
  // read Pot2 (non)
  mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
  if (mux2Read > (mux2ValuesPrev[0] + (QUANTISE_FACTOR * 4)) ||
      mux2Read < (mux2ValuesPrev[0] - (QUANTISE_FACTOR * 4))) {
    mux2ValuesPrev[0] = mux2Read;
  }
  // read Pot3 (non)
  mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
  if (mux3Read > (mux3ValuesPrev[0] + (QUANTISE_FACTOR * 4)) ||
      mux3Read < (mux3ValuesPrev[0] - (QUANTISE_FACTOR * 4))) {
    mux3ValuesPrev[0] = mux3Read;
  }
  // read Pot4 (Cutoff value)
  mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
  if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
      mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
    mux4ValuesPrev[0] = mux4Read;
  }
}

//*************************************************************************
// convert Wavetable int16_t to float and print
//*************************************************************************
/*
FLASHMEM void convertINTtoFloat (void) {
double myDoubleValue = 0;
uint8_t count = 0;
for (uint16_t i = 0; i < 2048; i++) {
int16_t myIntValue = (WAVESHAPER_Convert[i]);
//myDoubleValue = ((double)myIntValue - 0) / ((double)32767 - 0);
myDoubleValue = ((double)myIntValue - 0) / ((double)128 - 0);
float myfloat = (float) myDoubleValue;
Serial.print(myfloat,8); Serial.print(", ");
count++;
if (count == 8 || i == 0 || i == 255) {
Serial.println();
count = 0;
}
}
}
*/

//*************************************************************************
// inc NotesON
//*************************************************************************
FLASHMEM void incNotesOn(void) {
  if (notesOn < MAXUNISON)
    notesOn++;
}

//*************************************************************************
//  NotesON
//*************************************************************************
FLASHMEM void decNotesOn() {
  if (notesOn > 0)
    notesOn--;
}

//*************************************************************************
// Note On from Midi IN
//*************************************************************************
FLASHMEM void myNoteOn(byte channel, byte note, byte velocity) {
  for (int i = 0; i < NO_OF_VOICES; i++) {
    if (voices[i].note == note && voices[i].voiceOn == 1) {
      endVoice(i + 1);
    }
  }

  // Transpose mode ? ---------------------------------------------------
  if (SEQmode == 1 && SEQrunStatus == true) {

    for (uint8_t i = 0; i < SEQstepNumbers; i++) {
      if (SeqNoteBufStatus[i] == 1) {
        SeqTranspose = note - SeqNote1Buf[i];
        return;
      }
    }
    SeqTranspose = 0;
    return;
  }
  //---------------------------------------------------------------------
  if (PageNr == 9 && SEQrunStatus == false && SEQmode == 2) {
    if (SeqRecNoteCount < 8) {
      SeqRecNoteCount++;
    }
  }

  // Unisono Mode 0 -----------------------------------------------------
  if (unison == 0) {
    switch (getVoiceNo(-1)) {
    case 1:
      if (oscDetuneSync == true) {
        waveformMod1a.sync();
        waveformMod1b.sync();
      }
      voice1On(note, velocity, VOICEMIXERLEVEL);
      updateVoice1();
      break;
    case 2:
      if (oscDetuneSync == true) {
        waveformMod2a.sync();
        waveformMod2b.sync();
      }
      voice2On(note, velocity, VOICEMIXERLEVEL);
      updateVoice2();
      break;
    case 3:
      if (oscDetuneSync == true) {
        waveformMod3a.sync();
        waveformMod3b.sync();
      }
      voice3On(note, velocity, VOICEMIXERLEVEL);
      updateVoice3();
      break;
    case 4:
      if (oscDetuneSync == true) {
        waveformMod4a.sync();
        waveformMod4b.sync();
      }
      voice4On(note, velocity, VOICEMIXERLEVEL);
      updateVoice4();
      break;
    case 5:
      if (oscDetuneSync == true) {
        waveformMod5a.sync();
        waveformMod5b.sync();
      }
      voice5On(note, velocity, VOICEMIXERLEVEL);
      updateVoice5();
      break;
    case 6:
      if (oscDetuneSync == true) {
        waveformMod6a.sync();
        waveformMod6b.sync();
      }
      voice6On(note, velocity, VOICEMIXERLEVEL);
      updateVoice6();
      break;
    case 7:
      if (oscDetuneSync == true) {
        waveformMod7a.sync();
        waveformMod7b.sync();
      }
      voice7On(note, velocity, VOICEMIXERLEVEL);
      updateVoice7();
      break;
    case 8:
      if (oscDetuneSync == true) {
        waveformMod8a.sync();
        waveformMod8b.sync();
      }
      voice8On(note, velocity, VOICEMIXERLEVEL);
      updateVoice8();
      break;
    }
  } else {
    // Unisono Mode 1  ------------------------------------------------
    if (unison == 1)
      incNotesOn(); // For Unison mode

    if (unison != 0 && oscDetuneSync == true) {
      waveformMod1a.sync();
      waveformMod1b.sync();
      waveformMod2a.sync();
      waveformMod2b.sync();
      waveformMod3a.sync();
      waveformMod3b.sync();
      waveformMod4a.sync();
      waveformMod4b.sync();
      waveformMod5a.sync();
      waveformMod5b.sync();
      waveformMod6a.sync();
      waveformMod6b.sync();
      waveformMod7a.sync();
      waveformMod7b.sync();
      waveformMod8a.sync();
      waveformMod8b.sync();
    }

    voice1On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice2On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice3On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice4On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice5On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice6On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice7On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice8On(note, velocity, UNISONVOICEMIXERLEVEL);
    updatesAllVoices(); // Set detune values
    prevNote = note;
  }

  // Pitch LFO sync -----------------------------------------------------
  if (oscLfoRetrig == 1) {
    pitchLfo.sync();
    pitchLfo.phase(LFO1phase);
  }

  // Filter LFO sync ----------------------------------------------------
  if (filterLfoRetrig == 1 || filterLfoRetrig == 2) {
    filterLfo.sync();
    filterLfo.phase(LFO2phase);
  }

  MidiStatusSymbol = 1;
  EnvIdelFlag = true; // oscilloscope enabled
  // drawVoiceLED ();
  voiceLEDflag = true;

  // Sequencer Recording notes
  SequencerRecNotes(note, velocity);

  // update LFO randomFlag
  LFO1randomFlag = false;
  LFO2randomFlag = false;
  LFO3randomFlag = false;
}

//*************************************************************************
// Note On from Sequencer
//*************************************************************************
FLASHMEM void myNoteOn2(byte channel, byte note, byte velocity) {

  if (SeqRecNoteCount < 8)
    SeqRecNoteCount++;

  // Unisono Mode 0 -----------------------------------------------------
  if (unison == 0) {
    switch (getVoiceNo(-1)) {
    case 1:
      if (oscDetuneSync == true) {
        waveformMod1a.sync();
        waveformMod1b.sync();
      }
      voice1On(note, velocity, VOICEMIXERLEVEL);
      updateVoice1();
      break;
    case 2:
      if (oscDetuneSync == true) {
        waveformMod2a.sync();
        waveformMod2b.sync();
      }
      voice2On(note, velocity, VOICEMIXERLEVEL);
      updateVoice2();
      break;
    case 3:
      if (oscDetuneSync == true) {
        waveformMod3a.sync();
        waveformMod3b.sync();
      }
      voice3On(note, velocity, VOICEMIXERLEVEL);
      updateVoice3();
      break;
    case 4:
      if (oscDetuneSync == true) {
        waveformMod4a.sync();
        waveformMod4b.sync();
      }
      voice4On(note, velocity, VOICEMIXERLEVEL);
      updateVoice4();
      break;
    case 5:
      if (oscDetuneSync == true) {
        waveformMod5a.sync();
        waveformMod5b.sync();
      }
      voice5On(note, velocity, VOICEMIXERLEVEL);
      updateVoice5();
      break;
    case 6:
      if (oscDetuneSync == true) {
        waveformMod6a.sync();
        waveformMod6b.sync();
      }
      voice6On(note, velocity, VOICEMIXERLEVEL);
      updateVoice6();
      break;
    case 7:
      if (oscDetuneSync == true) {
        waveformMod7a.sync();
        waveformMod7b.sync();
      }
      voice7On(note, velocity, VOICEMIXERLEVEL);
      updateVoice7();
      break;
    case 8:
      if (oscDetuneSync == true) {
        waveformMod8a.sync();
        waveformMod8b.sync();
      }
      voice8On(note, velocity, VOICEMIXERLEVEL);
      updateVoice8();
      break;
    }
  } else {
    // Unisono Mode 1  ------------------------------------------------
    if (unison == 1)
      incNotesOn(); // For Unison mode

    if (unison != 0 && oscDetuneSync == true) {
      waveformMod1a.sync();
      waveformMod1b.sync();
      waveformMod2a.sync();
      waveformMod2b.sync();
      waveformMod3a.sync();
      waveformMod3b.sync();
      waveformMod4a.sync();
      waveformMod4b.sync();
      waveformMod5a.sync();
      waveformMod5b.sync();
      waveformMod6a.sync();
      waveformMod6b.sync();
      waveformMod7a.sync();
      waveformMod7b.sync();
      waveformMod8a.sync();
      waveformMod8b.sync();
    }

    voice1On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice2On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice3On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice4On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice5On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice6On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice7On(note, velocity, UNISONVOICEMIXERLEVEL);
    voice8On(note, velocity, UNISONVOICEMIXERLEVEL);
    updatesAllVoices(); // Set detune values
    prevNote = note;
  }

  // Pitch LFO sync -----------------------------------------------------
  if (oscLfoRetrig == 1) {
    pitchLfo.sync();
    pitchLfo.phase(LFO1phase);
  }

  // Filter LFO sync ----------------------------------------------------
  if (filterLfoRetrig == 1 || filterLfoRetrig == 2) {
    filterLfo.sync();
    filterLfo.phase(LFO2phase);
  }

  MidiStatusSymbol = 1;
  EnvIdelFlag = true; // oscilloscope enabled
  drawVoiceLED();

  // Sequencer Recording notes
  SequencerRecNotes(note, velocity);

  // update LFO randomFlag
  LFO1randomFlag = false;
  LFO2randomFlag = false;
  LFO3randomFlag = false;
}

//*************************************************************************
// play Voices
//*************************************************************************
FLASHMEM void voice1On(byte note, byte velocity, float level) {
  keytrackingValue = KEYTRACKINGAMT[note] * keytrackingAmount;
  filterModMixer1.gain(2, keytrackingValue);
  voices[0].note = note;
  voices[0].timeOn = millis();
  float velo = VELOCITY[velocitySens][velocity];
  voiceMixer1.gain(0, ((0.5 - myAmpVelocity) + velo * myAmpVelocity));
  filterModMixer1.gain(3, (velo * myFilVelocity)); // Filter Velocity
  filterEnvelope1.noteOn();
  ampEnvelope1.noteOn();
  LFO1Envelope1.noteOn();
  LFO2Envelope1.noteOn();
  voices[0].voiceOn = 1;
  if (glideSpeed > 0 && note != prevNote) {
    glide1.amplitude((prevNote - note) *
                     DIV24); // Set glide to previous note frequency (limited to
                             // 1 octave max)
    glide1.amplitude(0, glideSpeed * GLIDEFACTOR); // Glide to current note
  }
  if (unison == 0)
    prevNote = note;
}

FLASHMEM void voice2On(byte note, byte velocity, float level) {
  keytrackingValue = KEYTRACKINGAMT[note] * keytrackingAmount;
  filterModMixer2.gain(2, keytrackingValue);
  voices[1].note = note;
  voices[1].timeOn = millis();
  float velo = VELOCITY[velocitySens][velocity];
  voiceMixer1.gain(1, ((0.5 - myAmpVelocity) + velo * myAmpVelocity));
  filterModMixer2.gain(3, (velo * myFilVelocity));
  filterEnvelope2.noteOn();
  ampEnvelope2.noteOn();
  LFO1Envelope2.noteOn();
  LFO2Envelope2.noteOn();
  voices[1].voiceOn = 1;
  if (glideSpeed > 0 && note != prevNote) {
    glide2.amplitude((prevNote - note) *
                     DIV24); // Set glide to previous note frequency (limited to
                             // 1 octave max)
    glide2.amplitude(0, glideSpeed * GLIDEFACTOR); // Glide to current note
  }
  if (unison == 0)
    prevNote = note;
}

FLASHMEM void voice3On(byte note, byte velocity, float level) {
  keytrackingValue = KEYTRACKINGAMT[note] * keytrackingAmount;
  filterModMixer3.gain(2, keytrackingValue);
  voices[2].note = note;
  voices[2].timeOn = millis();
  float velo = VELOCITY[velocitySens][velocity];
  voiceMixer1.gain(2, ((0.5 - myAmpVelocity) + velo * myAmpVelocity));
  filterModMixer3.gain(3, (velo * myFilVelocity));
  filterEnvelope3.noteOn();
  ampEnvelope3.noteOn();
  LFO1Envelope3.noteOn();
  LFO2Envelope3.noteOn();
  voices[2].voiceOn = 1;
  if (glideSpeed > 0 && note != prevNote) {
    glide3.amplitude((prevNote - note) *
                     DIV24); // Set glide to previous note frequency (limited to
                             // 1 octave max)
    glide3.amplitude(0, glideSpeed * GLIDEFACTOR); // Glide to current note
  }
  if (unison == 0)
    prevNote = note;
}

FLASHMEM void voice4On(byte note, byte velocity, float level) {
  keytrackingValue = KEYTRACKINGAMT[note] * keytrackingAmount;
  filterModMixer4.gain(2, keytrackingValue);
  voices[3].note = note;
  voices[3].timeOn = millis();
  float velo = VELOCITY[velocitySens][velocity];
  voiceMixer1.gain(3, ((0.5 - myAmpVelocity) + velo * myAmpVelocity));
  filterModMixer4.gain(3, (velo * myFilVelocity));
  filterEnvelope4.noteOn();
  ampEnvelope4.noteOn();
  LFO1Envelope4.noteOn();
  LFO2Envelope4.noteOn();
  voices[3].voiceOn = 1;
  if (glideSpeed > 0 && note != prevNote) {
    glide4.amplitude((prevNote - note) *
                     DIV24); // Set glide to previous note frequency (limited to
                             // 1 octave max)
    glide4.amplitude(0, glideSpeed * GLIDEFACTOR); // Glide to current note
  }
  if (unison == 0)
    prevNote = note;
}

FLASHMEM void voice5On(byte note, byte velocity, float level) {
  keytrackingValue = KEYTRACKINGAMT[note] * keytrackingAmount;
  filterModMixer5.gain(2, keytrackingValue);
  voices[4].note = note;
  voices[4].timeOn = millis();
  float velo = VELOCITY[velocitySens][velocity];
  voiceMixer2.gain(0, ((0.5 - myAmpVelocity) + velo * myAmpVelocity));
  filterModMixer5.gain(3, (velo * myFilVelocity));
  filterEnvelope5.noteOn();
  ampEnvelope5.noteOn();
  LFO1Envelope5.noteOn();
  LFO2Envelope5.noteOn();
  voices[4].voiceOn = 1;
  if (glideSpeed > 0 && note != prevNote) {
    glide5.amplitude((prevNote - note) *
                     DIV24); // Set glide to previous note frequency (limited to
                             // 1 octave max)
    glide5.amplitude(0, glideSpeed * GLIDEFACTOR); // Glide to current note
  }
  if (unison == 0)
    prevNote = note;
}

FLASHMEM void voice6On(byte note, byte velocity, float level) {
  keytrackingValue = KEYTRACKINGAMT[note] * keytrackingAmount;
  filterModMixer6.gain(2, keytrackingValue);
  voices[5].note = note;
  voices[5].timeOn = millis();
  float velo = VELOCITY[velocitySens][velocity];
  voiceMixer2.gain(1, ((0.5 - myAmpVelocity) + velo * myAmpVelocity));
  filterModMixer6.gain(3, (velo * myFilVelocity));
  filterEnvelope6.noteOn();
  ampEnvelope6.noteOn();
  LFO1Envelope6.noteOn();
  LFO2Envelope6.noteOn();
  voices[5].voiceOn = 1;
  if (glideSpeed > 0 && note != prevNote) {
    glide6.amplitude((prevNote - note) *
                     DIV24); // Set glide to previous note frequency (limited to
                             // 1 octave max)
    glide6.amplitude(0, glideSpeed * GLIDEFACTOR); // Glide to current note
  }
  if (unison == 0)
    prevNote = note;
}

FLASHMEM void voice7On(byte note, byte velocity, float level) {
  keytrackingValue = KEYTRACKINGAMT[note] * keytrackingAmount;
  filterModMixer7.gain(2, keytrackingValue);
  voices[6].note = note;
  voices[6].timeOn = millis();
  float velo = VELOCITY[velocitySens][velocity];
  voiceMixer2.gain(2, ((0.5 - myAmpVelocity) + velo * myAmpVelocity));
  filterModMixer7.gain(3, (velo * myFilVelocity));
  filterEnvelope7.noteOn();
  ampEnvelope7.noteOn();
  LFO1Envelope7.noteOn();
  LFO2Envelope7.noteOn();
  voices[6].voiceOn = 1;
  if (glideSpeed > 0 && note != prevNote) {
    glide7.amplitude((prevNote - note) *
                     DIV24); // Set glide to previous note frequency (limited to
                             // 1 octave max)
    glide7.amplitude(0, glideSpeed * GLIDEFACTOR); // Glide to current note
  }
  if (unison == 0)
    prevNote = note;
}

FLASHMEM void voice8On(byte note, byte velocity, float level) {
  keytrackingValue = KEYTRACKINGAMT[note] * keytrackingAmount;
  filterModMixer8.gain(2, keytrackingValue);
  voices[7].note = note;
  voices[7].timeOn = millis();
  float velo = VELOCITY[velocitySens][velocity];
  voiceMixer2.gain(3, ((0.5 - myAmpVelocity) + velo * myAmpVelocity));
  filterModMixer8.gain(3, (velo * myFilVelocity));
  filterEnvelope8.noteOn();
  ampEnvelope8.noteOn();
  LFO1Envelope8.noteOn();
  LFO2Envelope8.noteOn();
  voices[7].voiceOn = 1;
  if (glideSpeed > 0 && note != prevNote) {
    glide8.amplitude((prevNote - note) *
                     DIV24); // Set glide to previous note frequency (limited to
                             // 1 octave max)
    glide8.amplitude(0, glideSpeed * GLIDEFACTOR); // Glide to current note
  }
  if (unison == 0)
    prevNote = note;
}

//*************************************************************************
// endVoice
//*************************************************************************
FLASHMEM void endVoice(int voice) {

  switch (voice) {
  case 1:
    filterEnvelope1.noteOff();
    ampEnvelope1.noteOff();
    LFO1Envelope1.noteOff();
    LFO2Envelope1.noteOff();
    voices[0].voiceOn = 0;
    VoicesLEDtime[0] = VoicLEDtime;
    break;
  case 2:
    filterEnvelope2.noteOff();
    ampEnvelope2.noteOff();
    LFO1Envelope2.noteOff();
    LFO2Envelope2.noteOff();
    voices[1].voiceOn = 0;
    VoicesLEDtime[1] = VoicLEDtime;
    break;
  case 3:
    filterEnvelope3.noteOff();
    ampEnvelope3.noteOff();
    LFO1Envelope3.noteOff();
    LFO2Envelope3.noteOff();
    voices[2].voiceOn = 0;
    VoicesLEDtime[2] = VoicLEDtime;
    break;
  case 4:
    filterEnvelope4.noteOff();
    ampEnvelope4.noteOff();
    LFO1Envelope4.noteOff();
    LFO2Envelope4.noteOff();
    voices[3].voiceOn = 0;
    VoicesLEDtime[3] = VoicLEDtime;
    break;
  case 5:
    filterEnvelope5.noteOff();
    ampEnvelope5.noteOff();
    LFO1Envelope5.noteOff();
    LFO2Envelope5.noteOff();
    prevNote = voices[4].note;
    voices[4].voiceOn = 0;
    VoicesLEDtime[4] = VoicLEDtime;
    break;
  case 6:
    filterEnvelope6.noteOff();
    ampEnvelope6.noteOff();
    LFO1Envelope6.noteOff();
    LFO2Envelope6.noteOff();
    voices[5].voiceOn = 0;
    VoicesLEDtime[5] = VoicLEDtime;
    break;
  case 7:
    filterEnvelope7.noteOff();
    ampEnvelope7.noteOff();
    LFO1Envelope7.noteOff();
    LFO2Envelope7.noteOff();
    voices[6].voiceOn = 0;
    VoicesLEDtime[6] = VoicLEDtime;
    break;
  case 8:
    filterEnvelope8.noteOff();
    ampEnvelope8.noteOff();
    LFO1Envelope8.noteOff();
    LFO2Envelope8.noteOff();
    voices[7].voiceOn = 0;
    VoicesLEDtime[7] = VoicLEDtime;
    break;
  default:
    // Do nothing
    break;
  }
}

//*************************************************************************
// my Note Off
//*************************************************************************
FLASHMEM void myNoteOff(byte channel, byte note, byte velocity) {

  if (SeqRecNoteCount > 0) {
    SeqRecNoteCount--;
    if (SeqRecNoteCount == 0) {
      SequencerRecNotes(note, velocity);
    }
  }

  decNotesOn();
  if (unison == 0) {
    endVoice(getVoiceNo(note));
  } else {
    // UNISON MODE
    if (myUnisono == 0) {
      for (int i = 0; i < 8; i++) {
        endVoice(getVoiceNo(note));
      }
    } else if (myUnisono == 1) {
      for (int i = 0; i < 8; i++) {
        endVoice(getVoiceNo(note));
      }
    }
  }
}

//*************************************************************************
// all Notes Off
//*************************************************************************
FLASHMEM void allNotesOff() {
  notesOn = 0;
  for (int v = 0; v < NO_OF_VOICES; v++) {
    endVoice(v + 1);
  }
}

//*************************************************************************
// get VoiceNo
//*************************************************************************
FLASHMEM int getVoiceNo(int note) {
  voiceToReturn = -1;      // Initialise
  earliestTime = millis(); // Initialise to now
  if (note == -1) {
    // NoteOn() - Get the oldest free voice (recent voices may be still on
    // release stage)
    for (int i = 0; i < NO_OF_VOICES; i++) {
      if (voices[i].voiceOn == 0) {
        if (voices[i].timeOn < earliestTime) {
          earliestTime = voices[i].timeOn;
          voiceToReturn = i;
        }
      }
    }
    if (voiceToReturn == -1) {
      // No free voices, need to steal oldest sounding voice
      earliestTime = millis(); // Reinitialise
      for (int i = 0; i < NO_OF_VOICES; i++) {
        if (voices[i].timeOn < earliestTime) {
          earliestTime = voices[i].timeOn;
          voiceToReturn = i;
        }
      }
    }
    return voiceToReturn + 1;
  } else {
    // NoteOff() - Get voice number from note
    for (int i = 0; i < NO_OF_VOICES; i++) {
      if (voices[i].note == note && voices[i].voiceOn == 1) {
        return i + 1;
      }
    }
    // Unison - Note on without previous note off?
    return voiceToReturn;
  }
  // Shouldn't get here, return voice 1
  return 1;
}

//*************************************************************************
// update Voices
//*************************************************************************
FLASHMEM void updateVoice1() {

  int pitchNotea = oscPitchA + oscTranspose + SeqTranspose;
  int pitchNoteb = oscPitchB + oscTranspose + SeqTranspose;

  if (unison == 1) {
    waveformMod1a.frequency(
        (NOTEFREQS[voices[0].note + pitchNotea] * oscMasterTune));
    waveformMod1b.frequency(
        (NOTEFREQS[voices[0].note + pitchNoteb] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][1]))) *
        oscMasterTune);
  } else if (unison == 2) {
    waveformMod1a.frequency((NOTEFREQS[voices[0].note + pitchNotea +
                                       CHORD_DETUNE[0][chordDetune]]) *
                            oscMasterTune);
    waveformMod1b.frequency(
        (NOTEFREQS[voices[0].note + pitchNoteb + CHORD_DETUNE[0][chordDetune]] *
         CDT_DETUNE) *
        oscMasterTune);
  } else {
    waveformMod1a.frequency(NOTEFREQS[voices[0].note + pitchNotea] *
                            oscMasterTune);
    waveformMod1b.frequency((NOTEFREQS[voices[0].note + pitchNoteb] * detune) *
                            oscMasterTune);
  }
}

FLASHMEM void updateVoice2() {

  int pitchNotea = oscPitchA + oscTranspose + SeqTranspose;
  int pitchNoteb = oscPitchB + oscTranspose + SeqTranspose;

  if (unison == 1) {
    waveformMod2a.frequency(
        (NOTEFREQS[voices[1].note + pitchNotea] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][2]))) *
        oscMasterTune);
    waveformMod2b.frequency(
        (NOTEFREQS[voices[1].note + pitchNoteb] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][3]))) *
        oscMasterTune);
  } else if (unison == 2) {
    waveformMod2a.frequency((NOTEFREQS[voices[1].note + pitchNotea +
                                       CHORD_DETUNE[1][chordDetune]]) *
                            oscMasterTune);
    waveformMod2b.frequency(
        (NOTEFREQS[voices[1].note + pitchNoteb + CHORD_DETUNE[1][chordDetune]] *
         CDT_DETUNE) *
        oscMasterTune);
  } else {
    waveformMod2a.frequency(NOTEFREQS[voices[1].note + pitchNotea] *
                            oscMasterTune);
    waveformMod2b.frequency((NOTEFREQS[voices[1].note + pitchNoteb] * detune) *
                            oscMasterTune);
  }
}

FLASHMEM void updateVoice3() {
  int pitchNotea = oscPitchA + oscTranspose + SeqTranspose;
  int pitchNoteb = oscPitchB + oscTranspose + SeqTranspose;

  if (unison == 1) {
    waveformMod3a.frequency(
        (NOTEFREQS[voices[2].note + pitchNotea] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][4]))) *
        oscMasterTune);
    waveformMod3b.frequency(
        (NOTEFREQS[voices[2].note + pitchNoteb] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][5]))) *
        oscMasterTune);
  } else if (unison == 2) {
    waveformMod3a.frequency((NOTEFREQS[voices[2].note + pitchNotea +
                                       CHORD_DETUNE[2][chordDetune]]) *
                            oscMasterTune);
    waveformMod3b.frequency(
        (NOTEFREQS[voices[2].note + pitchNoteb + CHORD_DETUNE[2][chordDetune]] *
         CDT_DETUNE) *
        oscMasterTune);
  } else {
    waveformMod3a.frequency(NOTEFREQS[voices[2].note + pitchNotea] *
                            oscMasterTune);
    waveformMod3b.frequency((NOTEFREQS[voices[2].note + pitchNoteb] * detune) *
                            oscMasterTune);
  }
}
FLASHMEM void updateVoice4() {

  int pitchNotea = oscPitchA + oscTranspose + SeqTranspose;
  int pitchNoteb = oscPitchB + oscTranspose + SeqTranspose;

  if (unison == 1) {
    waveformMod4a.frequency(
        (NOTEFREQS[voices[3].note + pitchNotea] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][6]))) *
        oscMasterTune);
    waveformMod4b.frequency(
        (NOTEFREQS[voices[3].note + pitchNoteb] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][7]))) *
        oscMasterTune);
  } else if (unison == 2) {
    waveformMod4a.frequency((NOTEFREQS[voices[3].note + pitchNotea +
                                       CHORD_DETUNE[3][chordDetune]]) *
                            oscMasterTune);
    waveformMod4b.frequency(
        (NOTEFREQS[voices[3].note + pitchNoteb + CHORD_DETUNE[3][chordDetune]] *
         CDT_DETUNE) *
        oscMasterTune);
  } else {
    waveformMod4a.frequency(NOTEFREQS[voices[3].note + pitchNotea] *
                            oscMasterTune);
    waveformMod4b.frequency((NOTEFREQS[voices[3].note + pitchNoteb] * detune) *
                            oscMasterTune);
  }
}

FLASHMEM void updateVoice5() {

  int pitchNotea = oscPitchA + oscTranspose + SeqTranspose;
  int pitchNoteb = oscPitchB + oscTranspose + SeqTranspose;

  if (unison == 1) {
    waveformMod5a.frequency(
        (NOTEFREQS[voices[4].note + pitchNotea] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][8]))) *
        oscMasterTune);
    waveformMod5b.frequency(
        (NOTEFREQS[voices[4].note + pitchNoteb] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][9]))) *
        oscMasterTune);
  } else if (unison == 2) {
    waveformMod5a.frequency((NOTEFREQS[voices[4].note + pitchNotea +
                                       CHORD_DETUNE[4][chordDetune]]) *
                            oscMasterTune);
    waveformMod5b.frequency(
        (NOTEFREQS[voices[4].note + pitchNoteb + CHORD_DETUNE[4][chordDetune]] *
         CDT_DETUNE) *
        oscMasterTune);
  } else {
    waveformMod5a.frequency(NOTEFREQS[voices[4].note + pitchNotea] *
                            oscMasterTune);
    waveformMod5b.frequency((NOTEFREQS[voices[4].note + pitchNoteb] * detune) *
                            oscMasterTune);
  }
}

FLASHMEM void updateVoice6() {

  int pitchNotea = oscPitchA + oscTranspose + SeqTranspose;
  int pitchNoteb = oscPitchB + oscTranspose + SeqTranspose;

  if (unison == 1) {
    waveformMod6a.frequency(
        (NOTEFREQS[voices[5].note + pitchNotea] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][10]))) *
        oscMasterTune);
    waveformMod6b.frequency(
        (NOTEFREQS[voices[5].note + pitchNoteb] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][11]))) *
        oscMasterTune);
  } else if (unison == 2) {
    waveformMod6a.frequency((NOTEFREQS[voices[5].note + pitchNotea +
                                       CHORD_DETUNE[5][chordDetune]]) *
                            oscMasterTune);
    waveformMod6b.frequency(
        (NOTEFREQS[voices[5].note + pitchNoteb + CHORD_DETUNE[5][chordDetune]] *
         CDT_DETUNE) *
        oscMasterTune);
  } else {
    waveformMod6a.frequency(NOTEFREQS[voices[5].note + pitchNotea] *
                            oscMasterTune);
    waveformMod6b.frequency((NOTEFREQS[voices[5].note + pitchNoteb] * detune) *
                            oscMasterTune);
  }
}

FLASHMEM void updateVoice7() {

  int pitchNotea = oscPitchA + oscTranspose + SeqTranspose;
  int pitchNoteb = oscPitchB + oscTranspose + SeqTranspose;

  if (unison == 1) {
    waveformMod7a.frequency(
        (NOTEFREQS[voices[6].note + pitchNotea] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][12]))) *
        oscMasterTune);
    waveformMod7b.frequency(
        (NOTEFREQS[voices[6].note + pitchNoteb] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][13]))) *
        oscMasterTune);
  } else if (unison == 2) {
    waveformMod7a.frequency((NOTEFREQS[voices[6].note + pitchNotea +
                                       CHORD_DETUNE[6][chordDetune]]) *
                            oscMasterTune);
    waveformMod7b.frequency(
        (NOTEFREQS[voices[6].note + pitchNoteb + CHORD_DETUNE[6][chordDetune]] *
         CDT_DETUNE) *
        oscMasterTune);
  } else {
    waveformMod7a.frequency(NOTEFREQS[voices[6].note + pitchNotea] *
                            oscMasterTune);
    waveformMod7b.frequency((NOTEFREQS[voices[6].note + pitchNoteb] * detune) *
                            oscMasterTune);
  }
}

FLASHMEM void updateVoice8() {

  int pitchNotea = oscPitchA + oscTranspose + SeqTranspose;
  int pitchNoteb = oscPitchB + oscTranspose + SeqTranspose;

  if (unison == 1) {
    waveformMod8a.frequency(
        (NOTEFREQS[voices[7].note + pitchNotea] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][14]))) *
        oscMasterTune);
    waveformMod8b.frequency(
        (NOTEFREQS[voices[7].note + pitchNoteb] *
         (detune + ((1 - detune) * DETUNE[notesOn - 1][15]))) *
        oscMasterTune);
  } else if (unison == 2) {
    waveformMod8a.frequency((NOTEFREQS[voices[7].note + pitchNotea +
                                       CHORD_DETUNE[7][chordDetune]]) *
                            oscMasterTune);
    waveformMod8b.frequency(
        (NOTEFREQS[voices[7].note + pitchNoteb + CHORD_DETUNE[7][chordDetune]] *
         CDT_DETUNE) *
        oscMasterTune);
  } else {
    waveformMod8a.frequency(NOTEFREQS[voices[7].note + pitchNotea] *
                            oscMasterTune);
    waveformMod8b.frequency((NOTEFREQS[voices[7].note + pitchNoteb] * detune) *
                            oscMasterTune);
  }
}

//*************************************************************************
// get LFO Waveform
//*************************************************************************
FLASHMEM int getLFOWaveform(int value) {
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
  }
}

//*************************************************************************
// convert LFO1 Waveform
//*************************************************************************
FLASHMEM int convertLFOWaveformCC(int value) {
  switch (value) {
  case WAVEFORM_SINE:
    return 0;
    break;
  case WAVEFORM_TRIANGLE:
    return 19;
    break;
  case WAVEFORM_SAWTOOTH:
    return 41;
    break;
  case WAVEFORM_ARBITRARY:
    return 74;
    break;
  case WAVEFORM_SQUARE:
    return 103;
    break;
  case WAVEFORM_SAMPLE_HOLD:
    return 123;
    break;
  }
}
//*************************************************************************
// convert LFO1 Waveform
//*************************************************************************
FLASHMEM int convertLFOWaveform(int value) {
  switch (value) {
  case WAVEFORM_SINE:
    return 0;
    break;
  case WAVEFORM_TRIANGLE:
    return 1;
    break;
  case WAVEFORM_SAWTOOTH:
    return 2;
    break;
  case WAVEFORM_ARBITRARY:
    return 3;
    break;
  case WAVEFORM_SQUARE:
    return 4;
    break;
  case WAVEFORM_SAMPLE_HOLD:
    return 5;
    break;
  }
}
//*************************************************************************
// convert LFO2 Waveform
//*************************************************************************
FLASHMEM int convertLFO2Waveform(int value) {
  switch (value) {
  case WAVEFORM_SINE2:
    return 0;
    break;
  case WAVEFORM_TRIANGLE2:
    return 1;
    break;
  case WAVEFORM_SAWTOOTH2:
    return 2;
    break;
  case WAVEFORM_ARBITRARY2:
    return 3;
    break;
  case WAVEFORM_SQUARE2:
    return 4;
    break;
  case WAVEFORM_SAMPLE_HOLD2:
    return 5;
    break;
  }
}

//*************************************************************************
// load arbitrary Waveforms
//*************************************************************************
FLASHMEM void loadArbWaveformA(const int16_t *wavedata) {
  waveformMod1a.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod2a.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod3a.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod4a.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod5a.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod6a.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod7a.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod8a.arbitraryWaveform(wavedata, AWFREQ);
}

FLASHMEM void loadArbWaveformB(const int16_t *wavedata) {
  waveformMod1b.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod2b.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod3b.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod4b.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod5b.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod6b.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod7b.arbitraryWaveform(wavedata, AWFREQ);
  waveformMod8b.arbitraryWaveform(wavedata, AWFREQ);
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
FLASHMEM int getPitch(int value) { return PITCH[value]; }

//*************************************************************************
// get Pitch
//*************************************************************************
FLASHMEM int getTranspose(int value) { return TRANSPOSE[value]; }

//*************************************************************************
// get Pitch
//*************************************************************************
FLASHMEM int getMasterTune(int value) { return MASTERTUNE[value]; }

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
  showCurrentParameterPage("1. PWM LFO", String(value));
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
  showCurrentParameterPage("2. PWM LFO", String(value));
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
  if (unison == 0) {
    allNotesOff(); // Unison off
    // oscDetuneSync = true;
    updateDetune();
    setLED(2, false);
  } else if (unison == 1) { // Dyn. Unison
    setLED(2, true);
  } else { // Chd. Unison
    setLED(2, true);
  }
}

//*************************************************************************
// update Glide
//*************************************************************************
FLASHMEM void updateGlide() {
  /*
  if (glideSpeed * GLIDEFACTOR < 1000) {
  showCurrentParameterPage("Glide", String(int(glideSpeed * GLIDEFACTOR)) + "
  ms"); } else { showCurrentParameterPage("Glide", String((glideSpeed *
  GLIDEFACTOR) / 1000) + " s");
  }
  */
}

//*************************************************************************
// update Waveforms
//*************************************************************************
FLASHMEM void updateWaveformA(void) {

  int newWaveform;

  if (Osc1WaveBank == 0 && oscWaveformA <= 12 || oscWaveformA == 0) {
    newWaveform = waveform[oscWaveformA];
  } else {
    if (Osc1WaveBank == 0) { // Bank A (waveform 1-12 calculated waveforms. Rest
                             // arbitrary waveforms)
      loadArbWaveformA(ArbBank + (256 * (oscWaveformA - 12)));
    } else { // Bank B-O (arbitrary waveforms)
      loadArbWaveformA(ArbBank +
                       ((16384 * Osc1WaveBank) + (oscWaveformA * 256)));
    }
    newWaveform = WAVEFORM_ARBITRARY;
  }
  waveformMod1a.begin(newWaveform);
  waveformMod2a.begin(newWaveform);
  waveformMod3a.begin(newWaveform);
  waveformMod4a.begin(newWaveform);
  waveformMod5a.begin(newWaveform);
  waveformMod6a.begin(newWaveform);
  waveformMod7a.begin(newWaveform);
  waveformMod8a.begin(newWaveform);
  currentWaveformA = oscWaveformA;
  currentOsc1WaveBank = Osc1WaveBank;
}

FLASHMEM void updateWaveformB() {

  int newWaveform;

  if (Osc2WaveBank == 0 && oscWaveformB <= 12 || oscWaveformB == 0) {
    newWaveform = waveform[oscWaveformB];
  } else {
    if (Osc2WaveBank == 0) { // Bank A (waveform 1-12 calculated waveforms. Rest
                             // arbitrary waveforms)
      loadArbWaveformB(ArbBank + (256 * (oscWaveformB - 12)));
    } else { // Bank B-O (arbitrary waveforms)
      loadArbWaveformB(ArbBank +
                       ((16384 * Osc2WaveBank) + (oscWaveformB * 256)));
    }
    newWaveform = WAVEFORM_ARBITRARY;
  }
  waveformMod1b.begin(newWaveform);
  waveformMod2b.begin(newWaveform);
  waveformMod3b.begin(newWaveform);
  waveformMod4b.begin(newWaveform);
  waveformMod5b.begin(newWaveform);
  waveformMod6b.begin(newWaveform);
  waveformMod7b.begin(newWaveform);
  waveformMod8b.begin(newWaveform);
  currentWaveformB = oscWaveformB;
  currentOsc2WaveBank = Osc2WaveBank;
}

//*************************************************************************
// set Waveshaper Table
//*************************************************************************
FLASHMEM void setWaveShaperTable(uint8_t tableNo) {

  // -1.0 - 1.0
  if (tableNo == 0) {
    waveshaper1.shape(WAVESHAPER_TABLE1, 3);
    waveshaper2.shape(WAVESHAPER_TABLE1, 3);
    waveshaper3.shape(WAVESHAPER_TABLE1, 3);
    waveshaper4.shape(WAVESHAPER_TABLE1, 3);
    waveshaper5.shape(WAVESHAPER_TABLE1, 3);
    waveshaper6.shape(WAVESHAPER_TABLE1, 3);
    waveshaper7.shape(WAVESHAPER_TABLE1, 3);
    waveshaper8.shape(WAVESHAPER_TABLE1, 3);
  }
  // -0.5 - 0.5
  else if (tableNo == 1) {
    waveshaper1.shape(WAVESHAPER_TABLE2, 129);
    waveshaper2.shape(WAVESHAPER_TABLE2, 129);
    waveshaper3.shape(WAVESHAPER_TABLE2, 129);
    waveshaper4.shape(WAVESHAPER_TABLE2, 129);
    waveshaper5.shape(WAVESHAPER_TABLE2, 129);
    waveshaper6.shape(WAVESHAPER_TABLE2, 129);
    waveshaper7.shape(WAVESHAPER_TABLE2, 129);
    waveshaper8.shape(WAVESHAPER_TABLE2, 129);
  }
  // -0.3 - 0.3
  else if (tableNo == 2) {
    waveshaper1.shape(WAVESHAPER_TABLE3, 129);
    waveshaper2.shape(WAVESHAPER_TABLE3, 129);
    waveshaper3.shape(WAVESHAPER_TABLE3, 129);
    waveshaper4.shape(WAVESHAPER_TABLE3, 129);
    waveshaper5.shape(WAVESHAPER_TABLE3, 129);
    waveshaper6.shape(WAVESHAPER_TABLE3, 129);
    waveshaper7.shape(WAVESHAPER_TABLE3, 129);
    waveshaper8.shape(WAVESHAPER_TABLE3, 129);
  }
  // spezial 1
  else if (tableNo == 3) {
    waveshaper1.shape(WAVESHAPER_TABLE4, 129);
    waveshaper2.shape(WAVESHAPER_TABLE4, 129);
    waveshaper3.shape(WAVESHAPER_TABLE4, 129);
    waveshaper4.shape(WAVESHAPER_TABLE4, 129);
    waveshaper5.shape(WAVESHAPER_TABLE4, 129);
    waveshaper6.shape(WAVESHAPER_TABLE4, 129);
    waveshaper7.shape(WAVESHAPER_TABLE4, 129);
    waveshaper8.shape(WAVESHAPER_TABLE4, 129);
  }
  // spezial 2
  else if (tableNo == 4) {
    waveshaper1.shape(WAVESHAPER_TABLE5, 2049);
    waveshaper2.shape(WAVESHAPER_TABLE5, 2049);
    waveshaper3.shape(WAVESHAPER_TABLE5, 2049);
    waveshaper4.shape(WAVESHAPER_TABLE5, 2049);
    waveshaper5.shape(WAVESHAPER_TABLE5, 2049);
    waveshaper6.shape(WAVESHAPER_TABLE5, 2049);
    waveshaper7.shape(WAVESHAPER_TABLE5, 2049);
    waveshaper8.shape(WAVESHAPER_TABLE5, 2049);
  }
  // spezial 3
  else if (tableNo == 5) {
    waveshaper1.shape(WAVESHAPER_TABLE6, 1025);
    waveshaper2.shape(WAVESHAPER_TABLE6, 1025);
    waveshaper3.shape(WAVESHAPER_TABLE6, 1025);
    waveshaper4.shape(WAVESHAPER_TABLE6, 1025);
    waveshaper5.shape(WAVESHAPER_TABLE6, 1025);
    waveshaper6.shape(WAVESHAPER_TABLE6, 1025);
    waveshaper7.shape(WAVESHAPER_TABLE6, 1025);
    waveshaper8.shape(WAVESHAPER_TABLE6, 1025);
  }

  // spezial 4
  else if (tableNo == 6) {
    waveshaper1.shape(WAVESHAPER_TABLE7, 17);
    waveshaper2.shape(WAVESHAPER_TABLE7, 17);
    waveshaper3.shape(WAVESHAPER_TABLE7, 17);
    waveshaper4.shape(WAVESHAPER_TABLE7, 17);
    waveshaper5.shape(WAVESHAPER_TABLE7, 17);
    waveshaper6.shape(WAVESHAPER_TABLE7, 17);
    waveshaper7.shape(WAVESHAPER_TABLE7, 17);
    waveshaper8.shape(WAVESHAPER_TABLE7, 17);
  }

  // spezial 5
  else if (tableNo == 7) {
    waveshaper1.shape(WAVESHAPER_TABLE8, 2049);
    waveshaper2.shape(WAVESHAPER_TABLE8, 2049);
    waveshaper3.shape(WAVESHAPER_TABLE8, 2049);
    waveshaper4.shape(WAVESHAPER_TABLE8, 2049);
    waveshaper5.shape(WAVESHAPER_TABLE8, 2049);
    waveshaper6.shape(WAVESHAPER_TABLE8, 2049);
    waveshaper7.shape(WAVESHAPER_TABLE8, 2049);
    waveshaper8.shape(WAVESHAPER_TABLE8, 2049);
  }

  // spezial 6
  else if (tableNo == 8) {
    waveshaper1.shape(WAVESHAPER_TABLE9, 17);
    waveshaper2.shape(WAVESHAPER_TABLE9, 17);
    waveshaper3.shape(WAVESHAPER_TABLE9, 17);
    waveshaper4.shape(WAVESHAPER_TABLE9, 17);
    waveshaper5.shape(WAVESHAPER_TABLE9, 17);
    waveshaper6.shape(WAVESHAPER_TABLE9, 17);
    waveshaper7.shape(WAVESHAPER_TABLE9, 17);
    waveshaper8.shape(WAVESHAPER_TABLE9, 17);
  }

  // Overton 2+3
  else if (tableNo == 9) {
    waveshaper1.shape(WAVESHAPER_TABLE10, 129);
    waveshaper2.shape(WAVESHAPER_TABLE10, 129);
    waveshaper3.shape(WAVESHAPER_TABLE10, 129);
    waveshaper4.shape(WAVESHAPER_TABLE10, 129);
    waveshaper5.shape(WAVESHAPER_TABLE10, 129);
    waveshaper6.shape(WAVESHAPER_TABLE10, 129);
    waveshaper7.shape(WAVESHAPER_TABLE10, 129);
    waveshaper8.shape(WAVESHAPER_TABLE10, 129);
  }

  // Double Sine
  else if (tableNo == 10) {
    waveshaper1.shape(WAVESHAPER_TABLE11, 129);
    waveshaper2.shape(WAVESHAPER_TABLE11, 129);
    waveshaper3.shape(WAVESHAPER_TABLE11, 129);
    waveshaper4.shape(WAVESHAPER_TABLE11, 129);
    waveshaper5.shape(WAVESHAPER_TABLE11, 129);
    waveshaper6.shape(WAVESHAPER_TABLE11, 129);
    waveshaper7.shape(WAVESHAPER_TABLE11, 129);
    waveshaper8.shape(WAVESHAPER_TABLE11, 129);
  }

  // Obertone 0.5
  else if (tableNo == 11) {
    waveshaper1.shape(WAVESHAPER_TABLE12, 129);
    waveshaper2.shape(WAVESHAPER_TABLE12, 129);
    waveshaper3.shape(WAVESHAPER_TABLE12, 129);
    waveshaper4.shape(WAVESHAPER_TABLE12, 129);
    waveshaper5.shape(WAVESHAPER_TABLE12, 129);
    waveshaper6.shape(WAVESHAPER_TABLE12, 129);
    waveshaper7.shape(WAVESHAPER_TABLE12, 129);
    waveshaper8.shape(WAVESHAPER_TABLE12, 129);
  }

  // DualSinus 0.5
  else if (tableNo == 12) {
    waveshaper1.shape(WAVESHAPER_TABLE13, 129);
    waveshaper2.shape(WAVESHAPER_TABLE13, 129);
    waveshaper3.shape(WAVESHAPER_TABLE13, 129);
    waveshaper4.shape(WAVESHAPER_TABLE13, 129);
    waveshaper5.shape(WAVESHAPER_TABLE13, 129);
    waveshaper6.shape(WAVESHAPER_TABLE13, 129);
    waveshaper7.shape(WAVESHAPER_TABLE13, 129);
    waveshaper8.shape(WAVESHAPER_TABLE13, 129);
  }

  // TanHYP
  else if (tableNo == 13) {
    waveshaper1.shape(WAVESHAPER_TABLE14, 129);
    waveshaper2.shape(WAVESHAPER_TABLE14, 129);
    waveshaper3.shape(WAVESHAPER_TABLE14, 129);
    waveshaper4.shape(WAVESHAPER_TABLE14, 129);
    waveshaper5.shape(WAVESHAPER_TABLE14, 129);
    waveshaper6.shape(WAVESHAPER_TABLE14, 129);
    waveshaper7.shape(WAVESHAPER_TABLE14, 129);
    waveshaper8.shape(WAVESHAPER_TABLE14, 129);
  }
}

//*************************************************************************
// set Waveshaper drive value
//*************************************************************************
FLASHMEM void setWaveShaperDrive(float value) {
  WaveshaperAmp1.gain(value);
  WaveshaperAmp2.gain(value);
  WaveshaperAmp3.gain(value);
  WaveshaperAmp4.gain(value);
  WaveshaperAmp5.gain(value);
  WaveshaperAmp6.gain(value);
  WaveshaperAmp7.gain(value);
  WaveshaperAmp8.gain(value);
}

//*************************************************************************
// update Pitch
//*************************************************************************
FLASHMEM void updatePitchA() { updatesAllVoices(); }

FLASHMEM void updatePitchB() { updatesAllVoices(); }

//*************************************************************************
// update Detune
//*************************************************************************
FLASHMEM void updateDetune() { updatesAllVoices(); }

FLASHMEM void updatesAllVoices() {
  updateVoice1();
  updateVoice2();
  updateVoice3();
  updateVoice4();
  updateVoice5();
  updateVoice6();
  updateVoice7();
  updateVoice8();
}

FLASHMEM void updateFilterEnvelopeType() {
  // set Envelupe curve
  // 0 linear, -8 fast exponential, -8 slow exponential
  // envelopeType1 = -128;	// old liniear curve

  filterEnvelope1.setEnvType(envelopeType1);
  filterEnvelope2.setEnvType(envelopeType1);
  filterEnvelope3.setEnvType(envelopeType1);
  filterEnvelope4.setEnvType(envelopeType1);
  filterEnvelope5.setEnvType(envelopeType1);
  filterEnvelope6.setEnvType(envelopeType1);
  filterEnvelope7.setEnvType(envelopeType1);
  filterEnvelope8.setEnvType(envelopeType1);
}

FLASHMEM void updateAmpEnvelopeType() {
  // set Envelupe curve
  // 0 linear, -8 fast exponential, -8 slow exponential
  // envelopeType2 = -128;	// old linear curve

  ampEnvelope1.setEnvType(envelopeType2);
  ampEnvelope2.setEnvType(envelopeType2);
  ampEnvelope3.setEnvType(envelopeType2);
  ampEnvelope4.setEnvType(envelopeType2);
  ampEnvelope5.setEnvType(envelopeType2);
  ampEnvelope6.setEnvType(envelopeType2);
  ampEnvelope7.setEnvType(envelopeType2);
  ampEnvelope8.setEnvType(envelopeType2);
}

FLASHMEM void updateLFO1EnvelopeType() {
  // set Envelupe curve
  // 0 linear, -8 fast exponential, -8 slow exponential
  // envelopeType2 = -128;	// old linear curve

  LFO1Envelope1.setEnvType(LFO1envCurve);
  LFO1Envelope2.setEnvType(LFO1envCurve);
  LFO1Envelope3.setEnvType(LFO1envCurve);
  LFO1Envelope4.setEnvType(LFO1envCurve);
  LFO1Envelope5.setEnvType(LFO1envCurve);
  LFO1Envelope6.setEnvType(LFO1envCurve);
  LFO1Envelope7.setEnvType(LFO1envCurve);
  LFO1Envelope8.setEnvType(LFO1envCurve);
}

FLASHMEM void updateLFO2EnvelopeType() {
  // set Envelupe curve
  // 0 linear, -8 fast exponential, -8 slow exponential
  // envelopeType2 = -128;	// old linear curve

  LFO2Envelope1.setEnvType(LFO2envCurve);
  LFO2Envelope2.setEnvType(LFO2envCurve);
  LFO2Envelope3.setEnvType(LFO2envCurve);
  LFO2Envelope4.setEnvType(LFO2envCurve);
  LFO2Envelope5.setEnvType(LFO2envCurve);
  LFO2Envelope6.setEnvType(LFO2envCurve);
  LFO2Envelope7.setEnvType(LFO2envCurve);
  LFO2Envelope8.setEnvType(LFO2envCurve);
}

/*
FLASHMEM void updatePWMSource() {
if (pwmSource == PWMSOURCELFO) {
setPwmMixerAFEnv(0);//Set filter mod to zero
setPwmMixerBFEnv(0);//Set filter mod to zero
if (pwmRate > -5) {
setPwmMixerALFO(pwmAmtA);//Set LFO mod
setPwmMixerBLFO(pwmAmtB);//Set LFO mod
}
showCurrentParameterPage("PWM Source", "LFO"); //Only shown when updated via
MIDI } else { setPwmMixerALFO(0);//Set LFO mod to zero setPwmMixerBLFO(0);//Set
LFO mod to zero if (pwmRate > -5) { setPwmMixerAFEnv(pwmAmtA);//Set filter mod
setPwmMixerBFEnv(pwmAmtB);//Set filter mod
}
showCurrentParameterPage("PWM Source", "Filter Env");
}
}*/

FLASHMEM void updatePWMSourceA() {
  if (pwmSource == PWMSOURCELFO) {
    setPwmMixerAFEnv(0); // Set filter mod to zero
    // setPwmMixerBFEnv(0);//Set filter mod to zero
    if (pwmRateA > -5) {
      setPwmMixerALFO(pwmAmtA); // Set LFO mod
      // setPwmMixerBLFO(pwmAmtB);//Set LFO mod
    }
  } else {
    setPwmMixerALFO(0); // Set LFO mod to zero
    // setPwmMixerBLFO(0);//Set LFO mod to zero
    if (pwmRateA > -5) {
      setPwmMixerAFEnv(pwmAmtA); // Set filter mod
      // setPwmMixerBFEnv(pwmAmtB);//Set filter mod
    }
  }
}

FLASHMEM void updatePWMSourceB() {
  if (pwmSource == PWMSOURCELFO) {
    // setPwmMixerAFEnv(0);//Set filter mod to zero
    setPwmMixerBFEnv(0); // Set filter mod to zero
    if (pwmRateB > -5) {
      // setPwmMixerALFO(pwmAmtA);//Set LFO mod
      setPwmMixerBLFO(pwmAmtB); // Set LFO mod
    }
  } else {
    // setPwmMixerALFO(0);//Set LFO mod to zero
    setPwmMixerBLFO(0); // Set LFO mod to zero
    if (pwmRateB > -5) {
      // setPwmMixerAFEnv(pwmAmtA);//Set filter mod
      setPwmMixerBFEnv(pwmAmtB); // Set filter mod
    }
  }
}

/*
FLASHMEM void updatePWMRate() {
pwmLfoA.frequency(pwmRate);
pwmLfoB.frequency(pwmRate);
if (pwmRate == -10) {
//Set to fixed PW mode
setPwmMixerALFO(0);//LFO Source off
setPwmMixerBLFO(0);
setPwmMixerAFEnv(0);//Filter Env Source off
setPwmMixerBFEnv(0);
setPwmMixerAPW(1);//Manually adjustable pulse width on
setPwmMixerBPW(1);
//showCurrentParameterPage("PW Mode", "On");
} else if (pwmRate == -5) {
//Set to Filter Env Mod source
pwmSource = PWMSOURCEFENV;
updatePWMSource();
setPwmMixerAFEnv(pwmAmtA);
setPwmMixerBFEnv(pwmAmtB);
setPwmMixerAPW(0);
setPwmMixerBPW(0);
//showCurrentParameterPage("PWM Source", "Filter Env");
} else {
pwmSource = PWMSOURCELFO;
updatePWMSource();
setPwmMixerAPW(0);
setPwmMixerBPW(0);
//showCurrentParameterPage("PWM Rate", String(2* pwmRate) + " Hz");//PWM goes
through mid to maximum, sounding effectively twice as fast
}
}*/

FLASHMEM void updatePWMRateA() {
  pwmLfoA.frequency(pwmRateA);
  // pwmLfoB.frequency(pwmRate);
  if (pwmRateA == -10) {
    // Set to fixed PW mode
    setPwmMixerALFO(0); // LFO Source off
    // setPwmMixerBLFO(0);
    setPwmMixerAFEnv(0); // Filter Env Source off
    // setPwmMixerBFEnv(0);
    setPwmMixerAPW(1); // Manually adjustable pulse width on
    // setPwmMixerBPW(1);
  } else if (pwmRateA == -5) {
    // Set to Filter Env Mod source
    pwmSource = PWMSOURCEFENV;
    updatePWMSourceA();
    setPwmMixerAFEnv(pwmAmtA);
    // setPwmMixerBFEnv(pwmAmtB);
    setPwmMixerAPW(0);
    // setPwmMixerBPW(0);
  } else {
    pwmSource = PWMSOURCELFO;
    updatePWMSourceA();
    setPwmMixerAPW(0);
    // setPwmMixerBPW(0);
  }
}

FLASHMEM void updatePWMRateB() {
  // pwmLfoA.frequency(pwmRate);
  pwmLfoB.frequency(pwmRateB);
  if (pwmRateB == -10) {
    // Set to fixed PW mode
    // setPwmMixerALFO(0);//LFO Source off
    setPwmMixerBLFO(0);
    // setPwmMixerAFEnv(0);//Filter Env Source off
    setPwmMixerBFEnv(0);
    // setPwmMixerAPW(1);//Manually adjustable pulse width on
    setPwmMixerBPW(1);
  } else if (pwmRateB == -5) {
    // Set to Filter Env Mod source
    pwmSource = PWMSOURCEFENV;
    updatePWMSourceB();
    // setPwmMixerAFEnv(pwmAmtA);
    setPwmMixerBFEnv(pwmAmtB);
    // setPwmMixerAPW(0);
    setPwmMixerBPW(0);
  } else {
    pwmSource = PWMSOURCELFO;
    updatePWMSourceB();
    // setPwmMixerAPW(0);
    setPwmMixerBPW(0);
  }
}

FLASHMEM void updatePWMAmount() {
  // MIDI only - sets both osc
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
    // if PWM amount is around zero, fixed PW is enabled
    setPwmMixerALFO(0);
    // setPwmMixerBLFO(0);
    setPwmMixerAFEnv(0);
    // setPwmMixerBFEnv(0);
    setPwmMixerAPW(1);
    // setPwmMixerBPW(1);
  } else {
    setPwmMixerAPW(0);
    // setPwmMixerBPW(0);
    if (pwmSource == PWMSOURCELFO) {
      // PW alters PWM LFO amount for waveform A
      setPwmMixerALFO(pwmAmtA);
    } else {
      // PW alters PWM Filter Env amount for waveform A
      setPwmMixerAFEnv(pwmAmtA);
    }
  }
  float pwA_Adj = pwA; // Prevent silence when pw = +/-1.0 on pulse
  if (pwA > 0.98)
    pwA_Adj = 0.98f;
  if (pwA < -0.98)
    pwA_Adj = -0.98f;
  pwa.amplitude(pwA_Adj);
}

FLASHMEM void updatePWB() {
  if (pwmRateB == -10) {
    // if PWM amount is around zero, fixed PW is enabled
    // setPwmMixerALFO(0);
    setPwmMixerBLFO(0);
    // setPwmMixerAFEnv(0);
    setPwmMixerBFEnv(0);
    // setPwmMixerAPW(1);
    setPwmMixerBPW(1);
  } else {
    // setPwmMixerAPW(0);
    setPwmMixerBPW(0);
    if (pwmSource == PWMSOURCELFO) {
      // PW alters PWM LFO amount for waveform B
      setPwmMixerBLFO(pwmAmtB);
    } else {
      // PW alters PWM Filter Env amount for waveform B
      setPwmMixerBFEnv(pwmAmtB);
    }
  }
  float pwB_Adj = pwB; // Prevent silence when pw = +/-1 on pulse
  if (pwB > 0.98)
    pwB_Adj = 0.98f;
  if (pwB < -0.98)
    pwB_Adj = -0.98f;
  pwb.amplitude(pwB_Adj);
}

/*
//*************************************************************************
// Update OscFX
//*************************************************************************
FLASHMEM void updateOscFX() {

        // OscFX off
        if (oscFX == 0) {
                setfrequencyModulation();
                setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
                updateOscLevelA();
                updateOscLevelB();
        }

        // XOR
        else if (oscFX == 1) {
                setfrequencyModulation();
                setOscFXCombineMode(AudioEffectDigitalCombine::XOR);
                updateOscLevelA();
                updateOscLevelB();
        }

        // XMO (Cross)
        else if (oscFX == 2) {
                setfrequencyModulation();
                setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
                updateOscLevelA();
                updateOscLevelB();
        }

        // Modulo
        else if (oscFX == 3) {
                setfrequencyModulation();
                setOscFXCombineMode(AudioEffectDigitalCombine::MODULO);
                updateOscLevelA();
                updateOscLevelB();
        }

        // AND
        else if (oscFX == 4) {
                setfrequencyModulation();
                setOscFXCombineMode(AudioEffectDigitalCombine::AND);
                updateOscLevelA();
                updateOscLevelB();
        }

        // Phase
        else if (oscFX == 5 || oscFX == 7) {
                setphaseModulation();
                setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
                updateOscLevelA();
                updateOscLevelB();
        }

        // FM
        else if (oscFX == 6) {
                setfrequencyModulation();
                setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
                updateOscLevelA();
                updateOscLevelB();
        }
}

//*************************************************************************
// Update Osc FX Mix
//*************************************************************************
FLASHMEM void updateOscLevelA() {


        switch (oscFX) {

                case 0:	// FX off
                setOscModMixerA(3, 0);//Feed from Osc 2 (B)
                setWaveformMixerLevel(0, (oscALevel / 2));	// Osc1 Level
0.5 setWaveformMixerLevel(3, 0); // oscFX off break;

                case 1:	// XOR
                setOscModMixerA(3, 0);// Modmix off
                setWaveformMixerLevel(0, (oscALevel / 2));//Osc 1 (A)
                setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) *
Osc1ModAmt));//oscFX XOR level break;

                case 2:	// XMO (Cross)
                setWaveformMixerLevel(3, 0);// oscFX off
                if (oscALevel == 1.0f && oscBLevel <= 1.0f) {
                        setOscModMixerA(3, ((1 - oscBLevel) *
Osc1ModAmt));//Feed from Osc 2 (B) setWaveformMixerLevel(0, ONE);//Osc 1 (A)
                        setWaveformMixerLevel(1, 0);//Osc 2 (B)
                }
                break;

                case 3://Modulo
                setOscModMixerA(3, 0);//XMod off
                setOscModMixerB(3, 0);//XMod off
                setWaveformMixerLevel(0, (oscALevel / 2));	//Osc 1 (A)
                setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) *
Osc1ModAmt));//oscFX XOR level break;

                case 4://AND
                setOscModMixerA(3, 0);//XMod off
                setOscModMixerB(3, 0);//XMod off
                setWaveformMixerLevel(0, (oscALevel / 2));	//Osc 1 (A)
                setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) *
Osc1ModAmt));//oscFX XOR level break;

                case 5:// Phase
                setWaveformMixerLevel(3, 0);// oscFX off
                if (oscALevel == 1.0f && oscBLevel <= 1.0f) {
                        setOscModMixerA(3, ((1 - oscBLevel) *
Osc1ModAmt));//Feed from Osc 2 (B) setWaveformMixerLevel(0, ONE);//Osc 1 (A)
                        setWaveformMixerLevel(1, 0);//Osc 2 (B)
                }
                break;

                case 6://FM
                setOscModMixerA(3, 0);//XMod off
                setOscModMixerB(3, 0);//XMod off
                setWaveformMixerLevel(0, (oscALevel / 2));//Osc 1 (A)
                setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) *
Osc1ModAmt));//oscFX XOR level setOscModMixerA(3, ((oscBLevel / 2) *
Osc1ModAmt)); break;

                case 7:// Phase II
                setOscModMixerA(3, 0);//XMod off
                setOscModMixerB(3, 0);//XMod off
                setWaveformMixerLevel(0, (oscALevel / 2));//Osc 1 (A)
                setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) *
Osc1ModAmt));//oscFX XOR level setOscModMixerA(3, ((oscBLevel / 2) *
Osc1ModAmt)); break;
        }
}

FLASHMEM void updateOscLevelB() {

                switch (oscFX) {

                case 0: // FX off
                setOscModMixerB(3, 0);//Feed from Osc 1 (A)
                setWaveformMixerLevel(1, (oscBLevel / 2)); // Osc2 Level 0.5
                setWaveformMixerLevel(3, 0);//XOR
                break;

                case 1: // XOR
                setOscModMixerB(3, 0); // Modmix off
                setWaveformMixerLevel(1, (oscBLevel / 2));//Osc 2 (B)
                setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) *
Osc1ModAmt));//oscFX XOR level break;

                case 2: // XMO
                setWaveformMixerLevel(3, 0);// oscFX off
                if (oscBLevel == 1.0f && oscALevel < 1.0f) {
                        setOscModMixerB(3, ((1 - oscALevel) *
Osc1ModAmt));//Feed from Osc 1 (A) setWaveformMixerLevel(0, 0);//Osc 1 (A)
                        setWaveformMixerLevel(1, ONE);//Osc 2 (B)
                }
                break;

                case 3://Modulo
                setOscModMixerA(3, 0);//XMod off
                setOscModMixerB(3, 0);//XMod off
                setWaveformMixerLevel(1, (oscBLevel / 2));	//Osc 1 (A)
                setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) *
Osc1ModAmt));//oscFX XOR level break;

                case 4://AND
                setOscModMixerA(3, 0);//XMod off
                setOscModMixerB(3, 0);//XMod off
                setWaveformMixerLevel(1, (oscBLevel / 2));	//Osc 1 (A)
                setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) *
Osc1ModAmt));//oscFX XOR level break;

                case 5://Phase
                setWaveformMixerLevel(3, 0);// oscFX off
                if (oscBLevel == 1.0f && oscALevel < 1.0f) {
                        setOscModMixerB(3, ((1 - oscALevel) *
Osc1ModAmt));//Feed from Osc 1 (A) setWaveformMixerLevel(0, 0);//Osc 1 (A)
                        setWaveformMixerLevel(1, ONE);//Osc 2 (B)
                }
                break;

                case 6://FM
                setWaveformMixerLevel(1, (oscBLevel / 2));//Osc 2 (B)
                setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) *
Osc1ModAmt));//oscFX XOR level setOscModMixerA(3, ((oscALevel / 2) *
Osc1ModAmt)); break;

                case 7://Phase II
                setWaveformMixerLevel(1, (oscBLevel / 2));//Osc 2 (B)
                setWaveformMixerLevel(3, (((oscALevel + oscBLevel) / 2.0f) *
Osc1ModAmt));//oscFX XOR level setOscModMixerA(3, ((oscALevel / 2) *
Osc1ModAmt)); break;
        }
}
*/

//*************************************************************************
//  Update OscFX
//*************************************************************************
FLASHMEM void updateOscFX() {

  switch (oscFX) {
  case 0: // oscFX Mode: OFF
    setfrequencyModulation();
    setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
    updateOscLevelA();
    updateOscLevelB();
    break;

  case 1: // oscFX Mode: XOR
    setfrequencyModulation();
    setOscFXCombineMode(AudioEffectDigitalCombine::XOR);
    updateOscLevelA();
    updateOscLevelB();
    break;

  case 2: // oscFX Mode: XMO
    setfrequencyModulation();
    setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
    updateOscLevelA();
    updateOscLevelB();
    break;

  case 3: // oscFX Mode: MOD
    setfrequencyModulation();
    setOscFXCombineMode(AudioEffectDigitalCombine::MODULO);
    updateOscLevelA();
    updateOscLevelB();
    break;

  case 4: // oscFX Mode: AND
    setfrequencyModulation();
    setOscFXCombineMode(AudioEffectDigitalCombine::AND);
    updateOscLevelA();
    updateOscLevelB();
    break;

  case 5: // oscFX Mode: PHA
    setphaseModulation();
    setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
    updateOscLevelA();
    updateOscLevelB();
    break;

  case 6: // oscFX Mode: FM
    setfrequencyModulation();
    setOscFXCombineMode(AudioEffectDigitalCombine::OFF);
    updateOscLevelA();
    updateOscLevelB();
    break;
  }
}

FLASHMEM void updateOscLevelA() {
  switch (oscFX) {

  case 0:                                      // oscFX Mode: off
    setOscModMixerA(3, 0);                     // Feed from Osc2: off
    setWaveformMixerLevel(0, (oscALevel / 2)); // Osc1 Mix Level 0.5
    setWaveformMixerLevel(3, 0);               // oscFX out: off
    break;

  case 1:                                      // oscFX Mode: XOR
    setOscModMixerA(3, 0);                     // Feed from Osc2: off
    setWaveformMixerLevel(0, (oscALevel / 2)); // Osc 1 (A)
    setWaveformMixerLevel(
        3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt)); // oscFX XOR level
    break;

  case 2: // oscFX Mode: XMO
    if (oscALevel == 1.0f && oscBLevel <= 1.0f) {
      setOscModMixerA(3, ((1 - oscBLevel) * Osc1ModAmt) *
                             Osc1ModAmt); // Feed from Osc 2 (B)
      setWaveformMixerLevel(0, ONE);      // Osc 1 (A)
      setWaveformMixerLevel(1, 0);        // Osc 2 (B)
    }
    setWaveformMixerLevel(3, 0); // oscFX out: off
    break;

  case 3:                                      // oscFX Mode: MOD
    setOscModMixerA(3, 0);                     // XMod off
    setOscModMixerB(3, 0);                     // XMod off
    setWaveformMixerLevel(0, (oscALevel / 2)); // Osc 1 (A)
    setWaveformMixerLevel(
        3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt)); // oscFX XOR level
    break;

  case 4:                                      // oscFX Mode: AND
    setOscModMixerA(3, 0);                     // XMod off
    setWaveformMixerLevel(0, (oscALevel / 2)); // Osc 1 (A)
    setWaveformMixerLevel(
        3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt)); // oscFX XOR level
    break;

  case 5:                                      // oscFX Mode: PHA
    setOscModMixerB(3, 0);                     // XMod off
    setWaveformMixerLevel(0, (oscALevel / 2)); // Osc 1 (A)
    setWaveformMixerLevel(
        3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt)); // oscFX XOR level
    setOscModMixerA(3, ((oscBLevel / 2) * Osc1ModAmt));
    break;

  case 6:                                      // oscFX Mode: FM
    setOscModMixerB(3, 0);                     // XMod off
    setWaveformMixerLevel(0, (oscALevel / 2)); // Osc 1 (A)
    setWaveformMixerLevel(
        3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt)); // oscFX XOR level
    setOscModMixerA(3, ((oscBLevel / 2) * Osc1ModAmt));
    break;
  }
}

FLASHMEM void updateOscLevelB() {
  switch (oscFX) {
  case 0:                                      // oscFX Mode: off
    setOscModMixerB(3, 0);                     // Feed from Osc1: off
    setWaveformMixerLevel(1, (oscBLevel / 2)); // Osc2 Mix Level 0.5
    setWaveformMixerLevel(3, 0);               // oscFX out: off
    break;

  case 1:                                      // oscFX Mode: XOR
    setOscModMixerB(3, 0);                     // Feed from Osc1: off
    setWaveformMixerLevel(1, (oscBLevel / 2)); // Osc 2 (B)
    setWaveformMixerLevel(
        3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt)); // oscFX XOR level
    break;

  case 2: // oscFX Mode: XMO
    if (oscBLevel == 1.0f && oscALevel < 1.0f) {
      setOscModMixerA(3, ((1 - oscALevel) * Osc1ModAmt) *
                             Osc1ModAmt); // Feed from Osc 2 (B)
      setWaveformMixerLevel(0, 0);        // Osc 1 (A)
      setWaveformMixerLevel(1, ONE);      // Osc 2 (B)
    }
    setWaveformMixerLevel(3, 0); // oscFX out: off
    break;

  case 3:                                      // oscFX Mode: MOD
    setOscModMixerA(3, 0);                     // XMod off
    setOscModMixerB(3, 0);                     // XMod off
    setWaveformMixerLevel(1, (oscBLevel / 2)); // Osc 1 (A)
    setWaveformMixerLevel(
        3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt)); // oscFX XOR level
    break;

  case 4:                                      // oscFX Mode: AND
    setOscModMixerB(3, 0);                     // XMod off
    setWaveformMixerLevel(1, (oscBLevel / 2)); // Osc 2 (B)
    setWaveformMixerLevel(
        3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt)); // oscFX XOR level
    break;

  case 5:                                      // oscFX Mode: PHA
    setWaveformMixerLevel(1, (oscBLevel / 2)); // Osc 2 (B)
    setWaveformMixerLevel(
        3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt)); // oscFX XOR level
    setOscModMixerA(3, ((oscALevel / 2) * Osc1ModAmt));
    break;

  case 6:                                      // oscFX Mode: FM
    setWaveformMixerLevel(1, (oscBLevel / 2)); // Osc 2 (B)
    setWaveformMixerLevel(
        3, (((oscALevel + oscBLevel) / 2.0f) * Osc1ModAmt)); // oscFX XOR level
    setOscModMixerA(3, ((oscALevel / 2) * Osc1ModAmt));
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
    showCurrentParameterPage("Noise Level", "Pink " + String(noiseLevel));
  } else if (noiseLevel < 0) {
    pink.amplitude(0.0f);
    white.amplitude(abs(noiseLevel));
    showCurrentParameterPage("Noise Level", "White " + String(abs(noiseLevel)));
  } else {
    pink.amplitude(noiseLevel);
    white.amplitude(noiseLevel);
    showCurrentParameterPage("Noise Level", "Off");
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
    filterOctave = 6.9999f;
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
    filterOctave = 6.9999f;
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
      // BP mode
      LP = 0.0f;
      BP = 1.0f;
      HP = 0.0f;
      LA = 0.0f;
      filterStr = "Band Pass";
    } else {
      // LP-HP mix mode - a notch filter
      LP = 1.0f - filterMix;
      BP = 0.0f;
      HP = filterMix;
      LA = 0.0f;
    }
  } else { // Ladder Filter
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

FLASHMEM void updateLFO1env() {
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

FLASHMEM void updateFilterEnv() { setFilterModMixer(0, filterEnv); }

FLASHMEM void updatePitchEnv() {
  setOscModMixerA(1, pitchEnvA);
  setOscModMixerB(1, pitchEnvB);
}

FLASHMEM void updateKeyTracking() { setFilterModMixer(2, keytrackingValue); }

FLASHMEM void setFilterModMixer(int channel, float level) {
  filterModMixer1.gain(channel, level);
  filterModMixer2.gain(channel, level);
  filterModMixer3.gain(channel, level);
  filterModMixer4.gain(channel, level);
  filterModMixer5.gain(channel, level);
  filterModMixer6.gain(channel, level);
  filterModMixer7.gain(channel, level);
  filterModMixer8.gain(channel, level);
}

FLASHMEM void updateOscLFOAmt() {
  // float Amt = oscLfoAmt + ccModwheelAmt;
  // pitchLfo.amplitude(Amt);
  oscGlobalModMixer1.gain(1, oscLfoAmt);
  oscGlobalModMixer2.gain(1, oscLfoAmt);
  oscGlobalModMixer3.gain(1, oscLfoAmt);
  oscGlobalModMixer4.gain(1, oscLfoAmt);
  oscGlobalModMixer5.gain(1, oscLfoAmt);
  oscGlobalModMixer6.gain(1, oscLfoAmt);
  oscGlobalModMixer7.gain(1, oscLfoAmt);
  oscGlobalModMixer8.gain(1, oscLfoAmt);
}

FLASHMEM void updateModWheel() {
  // float Amt = oscLfoAmt + ccModwheelAmt;
  // pitchLfo.amplitude(Amt);
  oscGlobalModMixer1.gain(2, ccModwheelAmt);
  oscGlobalModMixer2.gain(2, ccModwheelAmt);
  oscGlobalModMixer3.gain(2, ccModwheelAmt);
  oscGlobalModMixer4.gain(2, ccModwheelAmt);
  oscGlobalModMixer5.gain(2, ccModwheelAmt);
  oscGlobalModMixer6.gain(2, ccModwheelAmt);
  oscGlobalModMixer7.gain(2, ccModwheelAmt);
  oscGlobalModMixer8.gain(2, ccModwheelAmt);
}

FLASHMEM void updatePitchLFORate() {
  pitchLfo.frequency(oscLfoRate);
  // showCurrentParameterPage("LFO Rate", String(oscLfoRate) + " Hz");
}

FLASHMEM void updatePitchLFOWaveform() { pitchLfo.begin(oscLFOWaveform); }

// MIDI CC only
FLASHMEM void updatePitchLFOMidiClkSync() {
  // showCurrentParameterPage("P. LFO Sync", oscLFOMidiClkSync == 1 ? "On" :
  // "Off");
}

FLASHMEM void updateFilterLfoRate() { filterLfo.frequency(filterLfoRate); }

FLASHMEM void updateFilterLfoAmt() {
  filterLfo.amplitude(filterLfoAmt);
  // showCurrentParameterPage("F. LFO Amt", String(filterLfoAmt));
}

FLASHMEM void updateFilterLFOWaveform() { filterLfo.begin(filterLfoWaveform); }

FLASHMEM void updatePitchLFORetrig() {
  // showCurrentParameterPage("P. LFO Retrig", oscLfoRetrig == 1 ? "On" :
  // "Off");
}

FLASHMEM void updateFilterLFORetrig() {
  // showCurrentParameterPage("F. LFO Retrig", filterLfoRetrig == 1 ? "On" :
  // "Off"); digitalWriteFast(RETRIG_LED, filterLfoRetrig == 1 ? HIGH : LOW); //
  // LED
}

FLASHMEM void updateFilterLFOMidiClkSync() {
  // Serial.println("MidiClkSync!");
  // showCurrentParameterPage("Tempo Sync", filterLFOMidiClkSync == 1 ? "On" :
  // "Off"); digitalWriteFast(TEMPO_LED, filterLFOMidiClkSync == 1 ? HIGH :
  // LOW);  // LED
}

FLASHMEM void updateFilterAttack() {

  // Envelope hold time
  filterEnvelope1.hold(0.0f);
  filterEnvelope2.hold(0.0f);
  filterEnvelope3.hold(0.0f);
  filterEnvelope4.hold(0.0f);
  filterEnvelope5.hold(0.0f);
  filterEnvelope6.hold(0.0f);
  filterEnvelope7.hold(0.0f);
  filterEnvelope8.hold(0.0f);

  filterEnvelope1.attack(filterAttack);
  filterEnvelope2.attack(filterAttack);
  filterEnvelope3.attack(filterAttack);
  filterEnvelope4.attack(filterAttack);
  filterEnvelope5.attack(filterAttack);
  filterEnvelope6.attack(filterAttack);
  filterEnvelope7.attack(filterAttack);
  filterEnvelope8.attack(filterAttack);
}

FLASHMEM void updateFilterDecay() {
  filterEnvelope1.decay(filterDecay);
  filterEnvelope2.decay(filterDecay);
  filterEnvelope3.decay(filterDecay);
  filterEnvelope4.decay(filterDecay);
  filterEnvelope5.decay(filterDecay);
  filterEnvelope6.decay(filterDecay);
  filterEnvelope7.decay(filterDecay);
  filterEnvelope8.decay(filterDecay);
}

FLASHMEM void updateFilterSustain() {
  filterEnvelope1.sustain(filterSustain);
  filterEnvelope2.sustain(filterSustain);
  filterEnvelope3.sustain(filterSustain);
  filterEnvelope4.sustain(filterSustain);
  filterEnvelope5.sustain(filterSustain);
  filterEnvelope6.sustain(filterSustain);
  filterEnvelope7.sustain(filterSustain);
  filterEnvelope8.sustain(filterSustain);
}

FLASHMEM void updateFilterRelease() {
  filterEnvelope1.release(filterRelease);
  filterEnvelope2.release(filterRelease);
  filterEnvelope3.release(filterRelease);
  filterEnvelope4.release(filterRelease);
  filterEnvelope5.release(filterRelease);
  filterEnvelope6.release(filterRelease);
  filterEnvelope7.release(filterRelease);
  filterEnvelope8.release(filterRelease);
}

FLASHMEM void updateAttack() {

  // Envelope Hold time
  ampEnvelope1.hold(0.0f);
  ampEnvelope2.hold(0.0f);
  ampEnvelope3.hold(0.0f);
  ampEnvelope4.hold(0.0f);
  ampEnvelope5.hold(0.0f);
  ampEnvelope6.hold(0.0f);
  ampEnvelope7.hold(0.0f);
  ampEnvelope8.hold(0.0f);

  ampEnvelope1.release(ampAttack);
  ampEnvelope2.release(ampAttack);
  ampEnvelope3.release(ampAttack);
  ampEnvelope4.release(ampAttack);
  ampEnvelope5.release(ampAttack);
  ampEnvelope6.release(ampAttack);
  ampEnvelope7.release(ampAttack);
  ampEnvelope8.release(ampAttack);

  ampEnvelope1.attack(ampAttack);
  ampEnvelope2.attack(ampAttack);
  ampEnvelope3.attack(ampAttack);
  ampEnvelope4.attack(ampAttack);
  ampEnvelope5.attack(ampAttack);
  ampEnvelope6.attack(ampAttack);
  ampEnvelope7.attack(ampAttack);
  ampEnvelope8.attack(ampAttack);
}

FLASHMEM void updateDecay() {
  ampEnvelope1.decay(ampDecay);
  ampEnvelope2.decay(ampDecay);
  ampEnvelope3.decay(ampDecay);
  ampEnvelope4.decay(ampDecay);
  ampEnvelope5.decay(ampDecay);
  ampEnvelope6.decay(ampDecay);
  ampEnvelope7.decay(ampDecay);
  ampEnvelope8.decay(ampDecay);
}

FLASHMEM void updateSustain() {
  ampEnvelope1.sustain(ampSustain);
  ampEnvelope2.sustain(ampSustain);
  ampEnvelope3.sustain(ampSustain);
  ampEnvelope4.sustain(ampSustain);
  ampEnvelope5.sustain(ampSustain);
  ampEnvelope6.sustain(ampSustain);
  ampEnvelope7.sustain(ampSustain);
  ampEnvelope8.sustain(ampSustain);
}

FLASHMEM void updateRelease() {
  // updateReleaseNoteOn();
  ampEnvelope1.release(ampRelease);
  ampEnvelope2.release(ampRelease);
  ampEnvelope3.release(ampRelease);
  ampEnvelope4.release(ampRelease);
  ampEnvelope5.release(ampRelease);
  ampEnvelope6.release(ampRelease);
  ampEnvelope7.release(ampRelease);
  ampEnvelope8.release(ampRelease);
}

FLASHMEM void updateFilterVelocity() {
  FilterVelo1.amplitude(myFilVelocity);
  FilterVelo2.amplitude(myFilVelocity);
  FilterVelo3.amplitude(myFilVelocity);
  FilterVelo4.amplitude(myFilVelocity);
  FilterVelo5.amplitude(myFilVelocity);
  FilterVelo6.amplitude(myFilVelocity);
  FilterVelo7.amplitude(myFilVelocity);
  FilterVelo8.amplitude(myFilVelocity);
}

FLASHMEM void setphaseModulation() {
  float degrees = 180.0f;
  waveformMod1a.phaseModulation(degrees);
  waveformMod2a.phaseModulation(degrees);
  waveformMod3a.phaseModulation(degrees);
  waveformMod4a.phaseModulation(degrees);
  waveformMod5a.phaseModulation(degrees);
  waveformMod6a.phaseModulation(degrees);
  waveformMod7a.phaseModulation(degrees);
  waveformMod8a.phaseModulation(degrees);
}

FLASHMEM void setfrequencyModulation() {
  float value = 2.0f;
  waveformMod1a.frequencyModulation(value);
  waveformMod2a.frequencyModulation(value);
  waveformMod3a.frequencyModulation(value);
  waveformMod4a.frequencyModulation(value);
  waveformMod5a.frequencyModulation(value);
  waveformMod6a.frequencyModulation(value);
  waveformMod7a.frequencyModulation(value);
  waveformMod8a.frequencyModulation(value);
  waveformMod1b.frequencyModulation(value);
  waveformMod2b.frequencyModulation(value);
  waveformMod3b.frequencyModulation(value);
  waveformMod4b.frequencyModulation(value);
  waveformMod5b.frequencyModulation(value);
  waveformMod6b.frequencyModulation(value);
  waveformMod7b.frequencyModulation(value);
  waveformMod8b.frequencyModulation(value);
}

FLASHMEM void setOscFXCombineMode(AudioEffectDigitalCombine::combineMode mode) {
  oscFX1.setCombineMode(mode);
  oscFX2.setCombineMode(mode);
  oscFX3.setCombineMode(mode);
  oscFX4.setCombineMode(mode);
  oscFX5.setCombineMode(mode);
  oscFX6.setCombineMode(mode);
  oscFX7.setCombineMode(mode);
  oscFX8.setCombineMode(mode);
}

//*************************************************************************
// Update Osc overdrive
//*************************************************************************
FLASHMEM void updateOverdrive() { setAmpLevel(driveLevel); }

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
FLASHMEM void updateFxChip(void) {
  analogWrite(PWM1, FxPot1value * 2);
  analogWrite(PWM2, FxPot2value * 2);
  analogWrite(PWM3, FxPot3value * 2);
  analogWrite(PWM4, FxMixValue * 2);
  if (FxClkRate <= 10000) {
    FxClkRate = 10000;
  }
  analogWriteFrequency(PWM5, FxClkRate);
  setFxPrg(FxPrgNo);
  if (FxPrgNo > 0) {
    setLED(1, true);
  } else {
    setLED(1, false);
  }
}

//*************************************************************************
// Update Patch Name
//*************************************************************************
FLASHMEM void updatePatchname() { showPatchPage(String(patchNo), patchName); }

//*************************************************************************
// Update PitchBend
//*************************************************************************
FLASHMEM void myPitchBend(byte channel, int bend) {
  bend = (bend * PitchWheelAmt);
  // 0.5 to give 1oct max - spread of mod is 2oct
  // pitchBend.amplitude(bend * 0.5f * pitchBendRange * DIV12 * DIV8192);
  pitchBend.amplitude(bend * 0.000061);
}

//*************************************************************************
// Update Boost
//*************************************************************************
FLASHMEM void updateBoost(void) {

  if (myBoost == 1) {
    BassBoostStatus = 1;
    setLED(4, true);
    digitalWrite(BassBoost, LOW); // Boost on
  } else {
    BassBoostStatus = 0;
    setLED(4, false);
    digitalWrite(BassBoost, HIGH); // Boost off
  }
}

//*************************************************************************
// Midi CC (extern midi ControlChanges)
//*************************************************************************
FLASHMEM void myControlChange1(byte channel, byte control, byte value) {
  Serial.print("cc: control:");
  Serial.print(control);
  Serial.print(" mapped to:");

  // convert Midi CC datas ----------------------------------------------
  control = MidiCCTabel[control];
  Serial.print(control);
  Serial.print(" channel:");
  Serial.print(channel);
  Serial.print(" value:");
  Serial.println(value);

  // Cutoff: convert CC value from 127 to 255 ---------------------------
  if (control == 74) {
    value = value << 1;
  }

  myControlChange(channel, control, value);
}

FLASHMEM void sendCC(byte channel, byte control, byte value) {
  channel = 1;
  // Cutoff: convert CC value from 255 to 127 ---------------------------
  if (control == 74) {
    value = value >> 1;
  }
  int i;
  for (i = 0; i < 128; i++) {
    byte cc = MidiCCTabel[i];
    if (cc == control) {
      /*
      Serial.print("send cc: control:");
      Serial.print(i);
      Serial.print(" value:");
      Serial.print(value);
      Serial.print(" channel:");
      Serial.println(channel);
      */
      MIDI.sendControlChange(i, value, channel); // cc, value, midiOutCh);
    }
  }
}
FLASHMEM int glsearch(float array[],int numelems, float inval) {
  int i = 0;
  Serial.print("numelems:");
  Serial.println(numelems);

  while (i < numelems && array[++i] < inval) {
          Serial.print("i:");
      Serial.print(i);
      Serial.print(" array[i]:");
      Serial.print(array[i]);
      Serial.print(" inval:");
      Serial.println(inval);
  }
  return i;
}
FLASHMEM int glsearch_uint32_t(uint32_t array[],int numelems, uint32_t inval) {
  int i = 0;
  while (i < numelems && array[++i] < inval) {
    ;
  }
  return i;
}

FLASHMEM byte sendWholePatchAsCC() {
  byte channel = 1;
  byte i;
  Serial.println("sendWholePatchAsCC");

  for (i = 0; i < 128; i++) {
    byte cc = MidiCCTabel[i];
    byte value = 0;

    switch (cc) { // was in myControlChange und myCCgroup genutzt wird wird

      // intern <-> midi
    case 1: // CC 1		Modulation Wheel
      value = glsearch(POWER,sizeof POWER / sizeof POWER[0],( ccModwheelAmt / modWheelDepth) / MODWheelAmt);
      break;
    case 3: // CC 3		Pitch LFO Amount
      value = glsearch(POWER,sizeof POWER / sizeof POWER[0], oscLfoAmt);
      break;
    case 5: // CC 5		Glide
      value = glsearch(POWER,sizeof POWER / sizeof POWER[0], glideSpeed);
      break;
    case 9: // CC 9		Osc1 WaveBank
      value = Osc1WaveBank << 3;
      break;
    case 10: // CC 10	Osc2 WaveBank
      value = Osc2WaveBank << 3;
      break;
    case 14: // CC 14	Osc 1 Waveform
      value = oscWaveformA << 1;
      break;
    case 15: // CC 15	Osc 2 Waveform
      value = oscWaveformB << 1;
      break;
    case 16: // CC 16	Filter Envelope Amount
      value = glsearch(LINEARCENTREZERO,sizeof LINEARCENTREZERO / sizeof LINEARCENTREZERO[0], filterEnv / FILTERMODMIXERMAX);
      break;
    case 19: // CC 19	Filter Type
      value = glsearch(LINEAR_FILTERMIXER,sizeof LINEAR_FILTERMIXER / sizeof LINEAR_FILTERMIXER[0], filterMix);
      break;
    case 23: // CC 23	Noise Level (0 with: 63 off: 127 pink)
      value = glsearch(LINEARCENTREZERO,sizeof LINEARCENTREZERO / sizeof LINEARCENTREZERO[0], noiseLevel);
      break;
    case 24: // CC 24	oscMOD
      value = oscFX * 21;
      break;
    case 26: // CC 26	Osc 1 Pitch
      value = glsearch(PITCH,sizeof PITCH / sizeof PITCH[0], oscPitchA);
      Serial.print("26: value:");
      Serial.print(value);
      Serial.print(" oscPitchA:");
      Serial.println(oscPitchA);
      break;
    case 27: // CC 27	Osc 2 Pitch
      value = glsearch(PITCH,sizeof PITCH / sizeof PITCH[0], oscPitchB);
      break;
    case 30: // CC 30	Osc LFO SYNC (Retrig)
      value = oscLfoRetrig;
      break;
    case 31: // CC 31	Filter LFO SYNC (Retrig)
      value = filterLfoRetrig;
      break;
    case 71: // CC 71	Filter Resonance
      if (myFilter == 1) {
        value = glsearch(LINEAR,sizeof LINEAR / sizeof LINEAR[0], (filterRes - 0.71f) / 14.29f);
      } else {
        value = glsearch(LINEAR,sizeof LINEAR / sizeof LINEAR[0], filterRes / 1.8f);
      }
      break;
    case 72: // CC 72	AMP Release Time
      value = glsearch_uint32_t(ENVTIMES,sizeof ENVTIMES / sizeof ENVTIMES[0], ampRelease);
      break;
    case 73: // CC 73	AMP Attack Time
      value = glsearch_uint32_t(ENVTIMES,sizeof ENVTIMES / sizeof ENVTIMES[0], ampAttack);
      break;
    case 74: // CC 74	Filter Cutoff
      value = glsearch(FILTERFREQS256,sizeof FILTERFREQS256 / sizeof FILTERFREQS256[0], filterFreq);
      break;
    case 75: // CC 75	AMP Decay Time
      value = glsearch_uint32_t(ENVTIMES,sizeof ENVTIMES / sizeof ENVTIMES[0], ampDecay);
      break;
    case 76: // CC 76	Filter LFO Rate
      if (filterLFOMidiClkSync == 1) {
        value = glsearch(LFOTEMPO,sizeof LFOTEMPO / sizeof LFOTEMPO[0], filterLfoRate / lfoSyncFreq);
      } else {
        value = glsearch(POWER,sizeof POWER / sizeof POWER[0], filterLfoRate / LFOMAXRATE);
      }
      break;
    case 77: // CC 77	Filter LFO Amount
      value = glsearch(LINEAR,sizeof LINEAR / sizeof LINEAR[0], filterLfoAmt / FILTERMODMIXERMAX);
      break;
    case 79: // CC 79	AMP Sustain Level
      value = glsearch(LINEAR,sizeof LINEAR / sizeof LINEAR[0], ampSustain);
      break;
    case 80: // CC 80	Filter Attack Time
      value = glsearch_uint32_t(ENVTIMES,sizeof ENVTIMES / sizeof ENVTIMES[0], filterAttack);
      break;
    case 81: // CC 81	Filter Decay Time
      value = glsearch_uint32_t(ENVTIMES,sizeof ENVTIMES / sizeof ENVTIMES[0], filterDecay);
      break;
    case 82: // CC 82	Filter Sustain Time
      value = glsearch(LINEAR,sizeof LINEAR / sizeof LINEAR[0], filterSustain);
      break;
    case 83: // CC 83	Filter Release Time
      value = glsearch_uint32_t(ENVTIMES,sizeof ENVTIMES / sizeof ENVTIMES[0], filterRelease);
      break;
    case 85: // CC 85	Osc1 PWAMT	(Pulse width)
      value = glsearch(LINEAR,sizeof LINEAR / sizeof LINEAR[0], pwmAmtA);
      break;
    case 86: // CC 86	Osc2 PWAMT	(Pulse width)
      value = glsearch(LINEAR,sizeof LINEAR / sizeof LINEAR[0], pwmAmtB);
      break;
    case 87:    // CC 87	PWM Rate
      cc = 255; // do not send
      break;
    case 89: // CC 89	Key Tracking
      value = keytrackingAmount / DIV127;
      break;
    case 90: // CC 90	Filter LFO Waveform
      value = convertLFOWaveformCC(filterLfoWaveform);
      break;
    case 94: // CC 94	Detune
      value = chordDetune;
      break;
    case 123:   // CC 123	All Notes Off
      cc = 255; // do not send
      break;
    case 126: // CC 126	Unisono (o Off: >0 On)
      value = unison;
      break;
    case 141: // CC 100	LFO1 Shape
      value = myLFO1shapeNo * 25;
      break;
    case 142: // CC 101	LFO1 Rate
      value = glsearch(POWER,sizeof POWER / sizeof POWER[0], oscLfoRate / LFOMAXRATE);
      break;
    case 143: // CC 102	LFO1 Amt
      value = glsearch(LINEARLFO,sizeof LINEARLFO / sizeof LINEARLFO[0], oscLfoAmt);
      break;
    case 144: // CC 104	LFO2 Shape
      value = myLFO2shapeNo * 5;
      break;
    case 145: // CC 105	LFO2 Rate
      value = glsearch(POWER,sizeof POWER / sizeof POWER[0], filterLfoRate / LFOMAXRATE);
      break;
    case 146: // CC 106	LFO2 Amt
      value = glsearch(LINEARLFO,sizeof LINEARLFO / sizeof LINEARLFO[0], filterLfoAmt);
      break;
    case 150: // CC 8		OscMix
      value = glsearch_uint32_t(OSCMIXA,sizeof OSCMIXA / sizeof OSCMIXA[0], glsearch(LINEAR,sizeof LINEAR / sizeof LINEAR[0], oscALevel));
      break;
    case 151: // CC 28	Osc 1 Pitch Envelope
      value = glsearch(LINEARCENTREZERO,sizeof LINEARCENTREZERO / sizeof LINEARCENTREZERO[0], pitchEnvA / OSCMODMIXERMAX);
      break;
    case 152: // CC 29	Osc 2 Pitch Envelope
      value = glsearch(LINEARCENTREZERO,sizeof LINEARCENTREZERO / sizeof LINEARCENTREZERO[0], pitchEnvB / OSCMODMIXERMAX);
      break;
    case 153: // CC 7		Oscillator 1+2 Level
      value = glsearch(LINEAR,sizeof LINEAR / sizeof LINEAR[0], driveLevel / 1.25f);
      break;
    case 180: // CC 33	Prg Change
      value = patchNo - 1;
      break;
    case 181: // CC 32	Bank Select
      value = currentPatchBank;
      break;
    case 182: // CC 108	Fx P1
      value = FxPot1value;
      break;
    case 183: // CC 109	Fx P2
      value = FxPot2value;
      break;
    case 184: // CC 110	Fx P3
      value = FxPot3value;
      break;
    case 185: // CC 111	Fx CLK
      value = (FxClkRate - 10000) / 196.08f;
      break;
    case 186: // CC 112	Fx MIX
      value = FxMixValue;
      break;
    case 187: // CC 113	Fx PRG
      value = FxPrgNo;
      break;
    case 188: // CC 0
      value = currentPatchBank << 3;
      break;
    default:
      cc = 255;
      break;
    }
    if (cc != 255) {
      if (value > 127)
        value = 127;
      MIDI.sendControlChange(i, value, channel); // cc, value, midiOutCh);
/*
      Serial.print("send cc: control:");
      Serial.print(i);
      Serial.print(" value:");
      Serial.print(value);
      Serial.print(" channel:");
      Serial.println(channel);
      */
    }
  }
}

//*************************************************************************
// inter CC 0-50
//*************************************************************************
FLASHMEM void myCCgroup1(byte control, byte value) {
  // CC control No: 1
  if (control == CCmodwheel) {
    value = (value * MODWheelAmt);
    ccModwheelAmt =
        POWER[value] *
        modWheelDepth; // Variable LFO amount from mod wheel - Settings Option
    updateModWheel();
  }

  // CC control No: 3
  else if (control == CCosclfoamt) {
    // Pick up
    if (!pickUpActive && pickUp &&
        (oscLfoAmtPrevValue < POWER[value - TOLERANCE] ||
         oscLfoAmtPrevValue > POWER[value + TOLERANCE]))
      return; // PICK-UP
    oscLfoAmt = POWER[value];
    updateOscLFOAmt();
    oscLfoAmtPrevValue = oscLfoAmt; // PICK-UP
  }

  // CC control No: 5
  else if (control == CCglide) {
    glideSpeed = POWER[value];
    updateGlide();
    // draw parameter value on menu page1
    if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
      ParameterNr = 3;
      drawParamterFrame(PageNr, ParameterNr);
      tft.fillRoundRect(54, 77, 22, 9, 2, ST7735_BLUE);
      tft.setCursor(56, 78);
      tft.setTextColor(ST7735_WHITE);
      tft.print(value);
      ParUpdate = true;
    }
  }

  // CC control No: 9 (Osc1 WaveBank)
  else if (control == CCosc1WaveBank) {
    uint8_t newWaveBank = value >> 3;
    if (newWaveBank > 14) {
      newWaveBank = 14;
    }
    if (newWaveBank != Osc1WaveBank) {
      Osc1WaveBank = newWaveBank;
      updateWaveformA();
      if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
        ParameterNr = 0;
        drawParamterFrame(PageNr, ParameterNr);
        tft.fillRoundRect(34, 20, 7, 10, 2, ST7735_RED);
        tft.setCursor(35, 21);
        tft.setTextColor(ST7735_WHITE);
        tft.println(char(65 + newWaveBank));
        draw_Waveform(oscWaveformA, ST7735_RED);
      }
    }
  }

  // CC control No: 10 (Osc2 WaveBank)
  else if (control == CCosc2WaveBank) {
    uint8_t newWaveBank = value >> 3;
    if (newWaveBank > 14) {
      newWaveBank = 14;
    }
    if (newWaveBank != Osc2WaveBank) {
      Osc2WaveBank = newWaveBank;
      updateWaveformB();
      if (PageNr == 2 && myPageShiftStatus[PageNr] == false) {
        ParameterNr = 0;
        drawParamterFrame(PageNr, ParameterNr);
        tft.fillRoundRect(34, 20, 7, 10, 2, ST7735_RED);
        tft.setCursor(35, 21);
        tft.setTextColor(ST7735_WHITE);
        tft.println(char(65 + newWaveBank));
        draw_Waveform(oscWaveformB, ST7735_ORANGE);
      }
    }
  }

  // CC control No: 14
  else if (control == CCoscwaveformA) {
    oscWaveformA = value >> 1;
    if (currentWaveformA != oscWaveformA) {
      updateWaveformA();
      currentWaveformA = oscWaveformA;
      if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
        ParameterNr = 0;
        drawParamterFrame(PageNr, ParameterNr);
        tft.fillRoundRect(54, 19, 22, 10, 2, ST7735_BLUE);
        tft.setCursor(56, 21);
        tft.setTextColor(ST7735_WHITE);
        tft.print(oscWaveformA);
        draw_Waveform(oscWaveformA, ST7735_RED);
        printPWMrate(pwmRateA);
        ParUpdate = true;
      }
    }
  }

  // CC control No: 15
  else if (control == CCoscwaveformB) {
    oscWaveformB = value >> 1;
    if (currentWaveformB != oscWaveformB) {
      updateWaveformB();
      currentWaveformB = oscWaveformB;
      if (PageNr == 2 && myPageShiftStatus[PageNr] == false) {
        ParameterNr = 0;
        drawParamterFrame(PageNr, ParameterNr);
        tft.fillRoundRect(54, 19, 22, 10, 2, ST7735_BLUE);
        tft.setCursor(56, 21);
        tft.setTextColor(ST7735_WHITE);
        tft.print(oscWaveformB);
        draw_Waveform(oscWaveformB, ST7735_ORANGE);
        printPWMrate(pwmRateB);
        ParUpdate = true;
      }
    }
  }

  // CC control No: 16
  else if (control == CCfilterenv) {
    if (value >= 126) {
      value = 126;
    }
    filterEnv = (LINEARCENTREZERO[value] * FILTERMODMIXERMAX); // Bipolar
    updateFilterEnv();
    if (PageNr == 3) {
      draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
      if (myPageShiftStatus[PageNr] == false) {
        printFilterEnvAmt(value);
        printRedMarker(2, value);
      }
    }

  }

  // CC control No: 19
  else if (control == CCfiltermixer) {
    // Pick up
    // if (!pickUpActive && pickUp && (filterMixPrevValue <
    // LINEAR_FILTERMIXER[value - TOLERANCE] || filterMixPrevValue >
    // LINEAR_FILTERMIXER[value + TOLERANCE])) return; //PICK-UP
    filterMix = LINEAR_FILTERMIXER[value];
    filterMixStr = LINEAR_FILTERMIXERSTR[value];
    updateFilterMixer();
    if (PageNr == 3) {
      draw_filter_curves(FilterCut >> 1, FilterRes, value);
      if (myPageShiftStatus[PageNr] == false) {
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
    } else if (value >= 126) {
      value = 126;
    }
    noiseLevel = LINEARCENTREZERO[value];
    updateNoiseLevel();
    // draw noise level on submenu Osc1+2 submenu
    if ((PageNr == 1 || PageNr == 2) && myPageShiftStatus[PageNr] == true) {
      ParameterNr = 5;
      drawParamterFrame(PageNr, ParameterNr);
      tft.fillRoundRect(133, 59, 21, 8, 2, ST7735_BLUE);
      tft.setCursor(135, 59);
      tft.setTextColor(ST7735_WHITE);
      if (value >= 126) {
        value = 126;
      }
      if (value >= 62 && value <= 64) {
        tft.fillRoundRect(133, 59, 21, 8, 2, ST7735_BLUE);
        tft.setCursor(135, 59);
        tft.setTextColor(ST7735_WHITE);
        tft.print("OFF");
        tft.fillRect(122, 59, 5, 7, ST7735_BLACK);
      } else if (value < 62) {
        tft.println(62 - value);
        tft.setCursor(122, 59);
        tft.fillRect(122, 59, 5, 7, ST7735_BLACK);
        tft.setTextColor(ST7735_WHITE);
        tft.print("W");
      } else {
        tft.println(value - 64);
        tft.setCursor(122, 59);
        tft.fillRect(122, 59, 5, 7, ST7735_BLACK);
        tft.setTextColor(ST77XX_MAGENTA);
        tft.print("P");
      }
    }
  }

  // CC control No: 24
  else if (control == CCoscfx) {
    uint8_t val = (value / 21);
    oscFX = (val < 7) ? val : 0;
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
      tft.fillRect(54, 38, 22, 10, ST7735_BLUE);
      tft.setCursor(56, 40);
      tft.setTextColor(ST7735_WHITE);
      int temp = getPitch(value);
      if (temp > 0) {
        tft.print("+");
      }
      tft.print(temp);
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
      tft.fillRect(54, 38, 22, 10, ST7735_BLUE);
      tft.setCursor(56, 40);
      tft.setTextColor(ST7735_WHITE);
      int temp = getPitch(value);
      if (temp > 0) {
        tft.print("+");
      }
      tft.print(temp);
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
FLASHMEM void myCCgroup2(byte control, byte value) {
  // CC control No: 71
  if (control == CCfilterres) {
    // State Variable Filter -------------------
    if (myFilter == 1) {
      if (control == CCfilterres) { // Filter_variable
        // Pick up
        if (!pickUpActive && pickUp &&
            (resonancePrevValue <
                 ((14.29f * LINEAR[value - TOLERANCE]) + 0.71f) ||
             resonancePrevValue >
                 ((14.29f * LINEAR[value + TOLERANCE]) + 0.71f)))
          return; // PICK-UP
        filterRes = (14.29f * LINEAR[value]) +
                    0.71f; // If <1.1 there is noise at high cutoff freq
        updateFilterRes();
        resonancePrevValue = filterRes; // PICK-UP
        FilterRes = value;
        if (PageNr == 3) {
          draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
          if (myPageShiftStatus[PageNr] == false) {
            printFilterRes(value * DIV100); // value 0-100
          }
        }
      }
    }
    // Ladder Filter ---------------------------
    else {
      if (!pickUpActive && pickUp &&
          (resonancePrevValue < (1.8f * LINEAR[value - TOLERANCE]) ||
           resonancePrevValue > (1.8f * LINEAR[value + TOLERANCE])))
        return; // PICK-UP
      filterRes = (1.8f * LINEAR[value]);
      updateFilterRes();
      resonancePrevValue = filterRes; // PICK-UP
      FilterRes = value;
      if (PageNr == 3) {
        draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
        if (myPageShiftStatus[PageNr] == false) {
          printFilterRes(value * DIV100); // value 0-100
        }
      }
    }
  }

  // CC control No: 72
  else if (control == CCamprelease) {
    ampRelease = ENVTIMES[value];
    updateRelease();
    if (ampRelease < LFO1releaseTime) {
      updateLFO1release();
    }
    Env2Rel = value;
    if (PageNr == 5) {
      drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
      if (myPageShiftStatus[PageNr] == false) {
        printEnvRELvalues(Env2Rel);
      }
    }
  }

  // CC control No: 73
  else if (control == CCampattack) {
    ampAttack = ENVTIMES[value];
    updateAttack();
    updateRelease(); // Release needs to be updated when Attack is updated!
    Env2Atk = value;
    if (PageNr == 5) {
      drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
      if (myPageShiftStatus[PageNr] == false) {
        printEnvATKvalues(Env2Atk);
      }
    }
  }

  // CC control No: 74
  else if (control == CCfilterfreq) {

    filterFreq = FILTERFREQS256[value];

    if (PageNr == 0) {
      timer5 = millis(); // cutoffScreen Timer

      int val1 = value - TOLERANCE;
      int val2 = value + TOLERANCE;

      if (val1 < 0) {
        val1 = 0;
      }
      if (val2 >= 255) {
        val2 = 255;
      }
      int freq1 = FILTERFREQS256[val1];
      int freq2 = FILTERFREQS256[val2];

      if (filterfreqPrevValue >= freq1 &&
          filterfreqPrevValue <= freq2) { // PICK-UP
        filterfreqPrevValue = filterFreq; // PICK-UP
        cutoffPickupFlag = true;
      }
    }

    if (PageNr >= 1 || cutoffPickupFlag == true) {
      updateFilterFreq();
    }

    FilterCut = value;
    if (PageNr == 3) {
      draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
      if (myPageShiftStatus[PageNr] == false) {
        printDataValue(0, (DIV100 * (value >> 1)));
        printRedMarker(0, (value >> 1));
      }
    }
  }

  // CC control No: 75
  else if (control == CCampdecay) {
    ampDecay = ENVTIMES[value];
    updateDecay();
    Env2Dcy = value;
    if (PageNr == 5) {
      drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
      if (myPageShiftStatus[PageNr] == false) {
        printEnvDCYvalues(Env2Dcy);
      }
    }
  }

  // CC control No: 76
  else if (control == CCfilterlforate) {
    // Pick up
    if (!pickUpActive && pickUp &&
        (filterLfoRatePrevValue < LFOMAXRATE * POWER[value - TOLERANCE] ||
         filterLfoRatePrevValue > LFOMAXRATE * POWER[value + TOLERANCE]))
      return; // PICK-UP
    if (filterLFOMidiClkSync == 1) {
      filterLfoRate = getLFOTempoRate(value);
      filterLFOTimeDivStr = LFOTEMPOSTR[value];
    } else {
      filterLfoRate = LFOMAXRATE * POWER[value];
    }
    updateFilterLfoRate();
    filterLfoRatePrevValue = filterLfoRate; // PICK-UP
  }

  // CC control No: 77
  else if (control == CCfilterlfoamt) {
    // Pick up
    if (!pickUpActive && pickUp &&
        (filterLfoAmtPrevValue <
             LINEAR[value - TOLERANCE] * FILTERMODMIXERMAX ||
         filterLfoAmtPrevValue > LINEAR[value + TOLERANCE] * FILTERMODMIXERMAX))
      return; // PICK-UP
    filterLfoAmt = LINEAR[value] * FILTERMODMIXERMAX;
    updateFilterLfoAmt();
    filterLfoAmtPrevValue = filterLfoAmt; // PICK-UP
  }

  // CC control No: 79
  else if (control == CCampsustain) {
    ampSustain = LINEAR[value];
    updateSustain();
    Env2Sus = value;
    if (PageNr == 5) {
      drawEnvCurve(Env2Atk, Env2Dcy, Env2Sus, Env2Rel);
      if (myPageShiftStatus[PageNr] == false) {
        printEnvSUSvalues(Env2Sus);
      }
    }
  }

  // CC control No: 80
  else if (control == CCfilterattack) {
    filterAttack = ENVTIMES[value];
    updateFilterAttack();
    updateFilterRelease(); // Realeas needs to be updated when Attack is
                           // updated!
    Env1Atk = value;
    if (PageNr == 4) {
      drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
      if (myPageShiftStatus[PageNr] == false) {
        printEnvATKvalues(Env1Atk);
      }
    }
  }

  // CC control No: 81
  else if (control == CCfilterdecay) {
    filterDecay = ENVTIMES[value];
    updateFilterDecay();
    Env1Dcy = value;
    if (PageNr == 4) {
      drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
      if (myPageShiftStatus[PageNr] == false) {
        printEnvDCYvalues(Env1Dcy);
      }
    }
  }

  // CC control No: 82
  else if (control == CCfiltersustain) {
    filterSustain = LINEAR[value];
    updateFilterSustain();
    Env1Sus = value;
    if (PageNr == 4) {
      drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
      if (myPageShiftStatus[PageNr] == false) {
        printEnvSUSvalues(Env1Sus);
      }
    }
  }

  // CC control No: 83
  else if (control == CCfilterrelease) {
    filterRelease = ENVTIMES[value];
    updateFilterRelease();
    Env1Rel = value;
    if (PageNr == 4) {
      drawEnvCurve(Env1Atk, Env1Dcy, Env1Sus, Env1Rel);
      if (myPageShiftStatus[PageNr] == false) {
        printEnvRELvalues(Env1Rel);
      }
    }
  }

  // CC control No: 85
  else if (control == CCpwA) {
    if (value >= 126) {
      value = 126;
    }
    pwA = LINEARCENTREZERO[value]; // Bipolar
    pwmAmtA = LINEAR[value];
    updatePWA();
    if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
      ParameterNr = 5;
      drawParamterFrame(PageNr, ParameterNr);
      tft.fillRoundRect(133, 57, 22, 10, 2, ST7735_BLUE);
      tft.setCursor(135, 59);
      tft.setTextColor(ST7735_WHITE);
      if (value >= 126) {
        value = 126;
      }
      tft.print(value);
      if (Osc1WaveBank == 0 && oscWaveformA != 0 &&
          (oscWaveformA == 5 || oscWaveformA == 12)) {
        draw_PWM_curve(LINEARCENTREZERO[value]);
        printPWMrate(pwmRateA);
      }
    }
  }

  // CC control No: 86
  else if (control == CCpwB) {
    if (value >= 126) {
      value = 126;
    }
    pwB = LINEARCENTREZERO[value]; // Bipolar
    pwmAmtB = LINEAR[value];
    updatePWB();
    if (PageNr == 2 && myPageShiftStatus[PageNr] == false) {
      ParameterNr = 5;
      drawParamterFrame(PageNr, ParameterNr);
      tft.fillRoundRect(133, 57, 22, 10, 2, ST7735_BLUE);
      tft.setCursor(135, 59);
      tft.setTextColor(ST7735_WHITE);
      if (value >= 126) {
        value = 126;
      }
      tft.print(value);
      if (Osc2WaveBank == 0 && oscWaveformB != 0 &&
          (oscWaveformB == 5 || oscWaveformB == 12)) {
        draw_PWM_curve(LINEARCENTREZERO[value]);
        printPWMrate(pwmRateB);
      }
    }
  }

  // CC control No: 89
  else if (control == CCkeytracking) {
    /// keytrackingAmount = KEYTRACKINGAMT[value];
    keytrackingAmount = (DIV127 * value);
    updateKeyTracking();
  }

  // CC control No: 90
  else if (control == CCfilterlfowaveform) {
    if (filterLfoWaveform == getLFOWaveform(value))
      return;
    filterLfoWaveform = getLFOWaveform(value);
    updateFilterLFOWaveform();
  }

  // CC control No: 94 (max -6% Notefrq)
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
        tft.fillRoundRect(50, 76, 26, 10, 2, ST7735_BLACK);
        tft.fillRoundRect(54, 76, 22, 10, 2, ST7735_BLUE);
        tft.setCursor(56, 78);
        if (unison != 2) {
          tft.setTextColor(ST7735_WHITE);
          tft.print(value);
          if (unison == 2) {
            // unisono2Sync = true;
          }
        } else {
          tft.fillRoundRect(50, 76, 26, 10, 2, ST7735_BLUE);
          tft.setCursor(52, 78);
          tft.print(CDT_STR[value]);
        }
      }
    }
  }
}

//*************************************************************************
// inter CC 101-150
//*************************************************************************
FLASHMEM void myCCgroup3(byte control, byte value) {
  // CC control No: 102
  if (control == CCoscLfoRate) {
    // Pick up
    if (!pickUpActive && pickUp &&
        (oscLfoRatePrevValue < LFOMAXRATE * POWER[value - TOLERANCE] ||
         oscLfoRatePrevValue > LFOMAXRATE * POWER[value + TOLERANCE]))
      return; // PICK-UP
    if (oscLFOMidiClkSync == 1) {
      oscLfoRate = getLFOTempoRate(value);
      oscLFOTimeDivStr = LFOTEMPOSTR[value];
    } else {
      oscLfoRate = LFOMAXRATE * POWER[value];
    }
    updatePitchLFORate();
    oscLfoRatePrevValue = oscLfoRate; // PICK-UP
  }

  // CC control No: 103
  else if (control == CCoscLfoWaveform) {
    if (oscLFOWaveform == getLFOWaveform(value))
      return;
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
    case 2:
    default:
      unison = 2;
      break;
    }
    updateUnison();
  }

  // CC control No: 141
  else if (control == myLFO1shape) {
    value = value / 25;
    if (value >= 5) {
      value = 5;
    }
    if (value != myLFO1shapeNo) {
      myLFO1shapeNo = value;
      switch (value) {
      case 0:
        oscLFOWaveform = WAVEFORM_SINE;
        if (lfo1ph == 0) {
          LFO1phase = 0.0f;
        } else if (lfo1ph == 180) {
          LFO1phase = 180.0f;
        }
        updatePitchLFOWaveform();
        break;
      case 1:
        oscLFOWaveform = WAVEFORM_TRIANGLE;
        if (lfo1ph == 0) {
          LFO1phase = 0.0f;
        } else if (lfo1ph == 180) {
          LFO1phase = 180.0f;
        }
        updatePitchLFOWaveform();
        break;
      case 2:
        oscLFOWaveform = WAVEFORM_SAWTOOTH;
        if (lfo1ph == 0) {
          LFO1phase = 180.0f;
        } else if (lfo1ph == 180) {
          LFO1phase = 180.0f;
        }
        updatePitchLFOWaveform();
        break;
      case 3:
        pitchLfo.arbitraryWaveform(PARABOLIC_WAVE, AWFREQ); // half sine
        oscLFOWaveform = WAVEFORM_ARBITRARY;
        if (lfo1ph == 0) {
          LFO1phase = 0.0f;
        } else if (lfo1ph == 180) {
          LFO1phase = 180.0f;
        }
        updatePitchLFOWaveform();
        break;
      case 4:
        oscLFOWaveform = WAVEFORM_SQUARE;
        if (lfo1ph == 0) {
          LFO1phase = 180.0f;
        } else if (lfo1ph == 180) {
          LFO1phase = 0.0f;
        }
        updatePitchLFOWaveform();
        break;
      case 5:
        oscLFOWaveform = WAVEFORM_SAMPLE_HOLD;
        if (lfo1ph == 0) {
          LFO1phase = 0.0f;
        } else if (lfo1ph == 180) {
          LFO1phase = 180.0f;
        }
        updatePitchLFOWaveform();
        break;
      }
      if (PageNr == 7) {
        drawLFOframe(value);
        printDataValue(0, (value) + 1);
        printRedMarker(0, (value * 25.4f));
      }
    }
  }

  // CC control No: 142
  else if (control == myLFO1rate) {
    oscLfoRate = LFOMAXRATE * POWER[value];
    updatePitchLFORate();
    if (PageNr == 7) {
      printDataValue(1, value);
      printRedMarker(1, value);
    }
  }

  // CC control No: 143
  else if (control == myLFO1amt) {
    oscLfoAmt = LINEARLFO[value];
    updateOscLFOAmt();
    if (PageNr == 7) {
      printDataValue(2, value);
      printRedMarker(2, value);
    }
  }

  // CC control No: 144
  else if (control == myLFO2shape) {
    value = value / 25;
    if (value >= 5) {
      value = 5;
    }
    if (value != myLFO2shapeNo) {
      myLFO2shapeNo = value;
      switch (value) {
      case 0:
        filterLfoWaveform = WAVEFORM_SINE2;
        if (lfo2ph == 0) {
          LFO2phase = 0.0f;
        } else if (lfo2ph == 180) {
          LFO2phase = 180.0f;
        }
        updateFilterLFOWaveform();
        break;
      case 1:
        filterLfoWaveform = WAVEFORM_TRIANGLE2;
        if (lfo2ph == 0) {
          LFO2phase = 0.0f;
        } else if (lfo2ph == 180) {
          LFO2phase = 180.0f;
        }
        updateFilterLFOWaveform();
        break;
      case 2:
        filterLfoWaveform = WAVEFORM_SAWTOOTH2;
        if (lfo2ph == 0) {
          LFO2phase = 180.0f;
        } else if (lfo2ph == 180) {
          LFO2phase = 180.0f;
        }
        updateFilterLFOWaveform();
        break;
      case 3:
        filterLfo.arbitraryWaveform(PARABOLIC_WAVE, AWFREQ); // half sine
        filterLfoWaveform = WAVEFORM_ARBITRARY2;
        if (lfo2ph == 0) {
          LFO2phase = 0.0f;
        } else if (lfo2ph == 180) {
          LFO2phase = 180.0f;
        }
        updateFilterLFOWaveform();
        break;
      case 4:
        filterLfoWaveform = WAVEFORM_SQUARE2;
        if (lfo2ph == 0) {
          LFO2phase = 180.0f;
        } else if (lfo2ph == 180) {
          LFO2phase = 0.0f;
        }
        updateFilterLFOWaveform();
        break;
      case 5:
        filterLfoWaveform = WAVEFORM_SAMPLE_HOLD2;
        if (lfo2ph == 0) {
          LFO2phase = 0.0f;
        } else if (lfo2ph == 180) {
          LFO2phase = 180.0f;
        }
        updateFilterLFOWaveform();
        break;
      }
      if (PageNr == 8) {
        drawLFOframe(value);
        printDataValue(0, (value) + 1);
        printRedMarker(0, (value * 25.4f));
      }
    }
  }

  // CC control No: 145
  else if (control == myLFO2rate) {
    if (value == 0) {
      value = 1;
    }
    filterLfoRate = LFOMAXRATE * POWER[value];
    if (filterLfoRetrig == 3 && MidiSyncSwitch == true) {
      if (filterLFOTimeDivStr != LFOTEMPOSTR[myFilterLFORateValue]) {
        updateFilterLfoRate();
      }
    } else
      updateFilterLfoRate();

    myFilterLFORateValue = value;

    if (PageNr == 8) {
      printDataValue(1, value);
      printRedMarker(1, value);
      if (filterLfoRetrig == 3 && MidiSyncSwitch == true) {
        filterLfoRate = getLFOTempoRate(myFilterLFORateValue);
        filterLFOTimeDivStr = LFOTEMPOSTR[myFilterLFORateValue];
        tft.fillRect(46, 116, 24, 7, ST7735_BLACK);
        tft.setTextColor(ST7735_GRAY);
        int16_t x1, y1;
        uint16_t w1, h1;
        tft.getTextBounds(filterLFOTimeDivStr, 0, 0, &x1, &y1, &w1,
                          &h1);            // string width in pixels
        tft.setCursor(58 - (w1 / 2), 116); // print string in the middle
        tft.print(filterLFOTimeDivStr);

      } else if (filterLfoRetrig == 3 && MidiSyncSwitch == false) {
        tft.fillRect(46, 116, 24, 7, ST7735_BLACK);
        tft.setTextColor(ST7735_RED);
        tft.setCursor(46, 116);
        tft.print("CLK?");
      }
    }
  }

  // CC control No: 146
  else if (control == myLFO2amt) {
    filterLfoAmt = LINEARLFO[value];
    updateFilterLfoAmt();
    if (PageNr == 3 && myPageShiftStatus[PageNr] == true) {
      printDataValue(3, value);
      printRedMarker(3, value);
    } else if (PageNr == 8) {
      printDataValue(2, value);
      printRedMarker(2, value);
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
    }
  }
}

//*************************************************************************
// inter CC 151-200
//*************************************************************************
FLASHMEM void myCCgroup4(byte control, byte value) {
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
    }
  }

  // CC control No: 153
  else if (control == myDrive) {
    driveLevel = ((LINEAR[value]) * 1.25f);
    setAmpLevel(driveLevel);
    if ((PageNr == 1 || PageNr == 2) && myPageShiftStatus[PageNr] == false) {
      ParameterNr = 4;
      drawParamterFrame(PageNr, ParameterNr);
      tft.fillRoundRect(54, 95, 22, 10, 2, ST7735_BLUE);
      tft.setCursor(5, 97);
      if (value == 0) {
        tft.setTextColor(ST7735_RED);
        tft.println("LEVEL");
      } else {
        tft.setTextColor(ST7735_GRAY);
        tft.println("LEVEL");
      }
      tft.setCursor(56, 97);
      tft.setTextColor(ST7735_WHITE);
      tft.print(int(DIV100 * value));
    }
  }

  // CC control No: 155
  else if (control == myFilterVelo) {
    myFilVelocity = ((DIV127 * value));
  }

  // CC control No: 156
  else if (control == myAmplifierVelo) {
    myAmpVelocity = ((DIV127 * value) / 2);
  }

  /*
  // CC control No: 179
  else if (control == CCbankSelect) {
  if (value >=15) {
  value = 15;
  }
  currentPatchBank = value;
  recallPatch(patchNo);
  }
  */

  // CC control No: 180
  else if (control == CCprgChange) {
    if (PrgChangeSW == true) {
      if (patchNo != patchNo + 1) {
        patchNo = value + 1;
        recallPatch(patchNo);
      }
    }
  }

  // CC control No: 181 Bank select from Midi Keyboard
  else if (control == CCBankSelect16) {
    if (PrgChangeSW == true) {
      if (currentPatchBank != value) {
        currentPatchBank = value;
        recallPatch(patchNo);
      }
    }
  }

  // CC control No: 182
  else if (control == CCfxP1) {
    analogWrite(PWM1, value << 1);
    FxPot1value = value;
    if (PageNr == 6) {
      printFxValFrame(0);
      printFxPOT(0, FxPrgNo);
      printDataValue(1, value);
      printRedMarker(1, value);
      myFxSelValue = 0;
      printFxPotValue(myFxSelValue, value);
    }
  }

  // CC control No: 183
  else if (control == CCfxP2) {
    analogWrite(PWM2, value << 1);
    FxPot2value = value;
    if (PageNr == 6) {
      printFxValFrame(1);
      printFxPOT(1, FxPrgNo);
      printDataValue(1, value);
      printRedMarker(1, value);
      myFxSelValue = 1;
      printFxPotValue(myFxSelValue, value);
    }
  }

  // CC control No: 184
  else if (control == CCfxP3) {
    analogWrite(PWM3, value << 1);
    FxPot3value = value;
    if (PageNr == 6) {
      printFxValFrame(2);
      printFxPOT(2, FxPrgNo);
      printDataValue(1, value);
      printRedMarker(1, value);
      myFxSelValue = 2;
      printFxPotValue(myFxSelValue, value);
    }
  }

  // CC control No: 185
  else if (control == CCfxCLK) {
    FxClkRate = (value * 196.08f) + 10000;
    analogWriteFrequency(PWM5, FxClkRate);
    if (PageNr == 6) {
      printFxValFrame(3);
      printFxPOT(3, FxPrgNo);
      printDataValue(1, value);
      printRedMarker(1, value);
      tft.setCursor(65, 40);
      tft.setTextColor(ST7735_GRAY);
      tft.print("Clk");
      analogWriteFrequency(PWM5, FxClkRate);
      tft.fillRect(85, 40, 29, 7, ST7735_BLACK);
      tft.setCursor(85, 40);
      tft.print(FxClkRate / 1000);
      tft.print("Khz");
    }
  }

  // CC control No: 186
  else if (control == CCfxMIX) {
    analogWrite(PWM4, value << 1);
    if (PageNr == 6) {
      printDataValue(2, value);
      printRedMarker(2, value);
      FxMixValue = value;
      drawFxGrafic(FxMixValue, FxTimeValue, FxFeedbackValue, FxPot3value);
    }
  }

  // CC control No: 187
  else if (control == CCfxPRG) {
    value = value >> 3;
    if (value != FxPrgNo) {
      FxPrgNo = value;
      // setFxPrg(value);
      FxPrgNoChange = true; // FxPrg if change in main-loop
      analogWrite(PWM4, FxMixValue);
      if (PageNr == 6) {
        if (value == 0) {
          tft.fillRect(65, 27, 90, 9, ST7735_BLACK);
          tft.setTextColor(ST7735_RED);
          tft.setCursor(65, 28);
          tft.print("OFF");
          tft.fillRect(122, 120, 30, 7, ST7735_BLACK);
          tft.setTextColor(ST7735_GRAY);
          tft.setCursor(130, 120);
          tft.print("OFF");
          analogWrite(PWM4, 0);
          printFxPotValue(0, 0);
          printFxPotValue(1, 0);
          printFxPotValue(2, 0);
          drawFxGrafic(0, 0, 0, 0);
          printDataValue(1, 0);
          printRedMarker(1, 0);
          printDataValue(2, 0);
          printRedMarker(2, 0);
          printRedMarker(3, 0);
          // setLED(1, false);		// Fx LED is change in main-loop
          printFxValFrame(99); // clear green Frame
          tft.fillRect(3, 71, 155, 10, ST7735_BLACK);
          tft.setCursor(75, 72);
          tft.setTextColor(ST7735_GRAY);
          tft.print("Time/Feedb.");
        } else {
          printDataValue(1, myFxValValue);
          printRedMarker(1, myFxValValue);
          printDataValue(2, FxMixValue);
          printRedMarker(2, FxMixValue);
          printFxPotValue(0, FxPot1value);
          printFxPotValue(1, FxPot2value);
          printFxPotValue(2, FxPot3value);
          printDataValue(3, value);
          printRedMarker(3, (value * 8.5));
          printFxName(value);
          drawFxGrafic(FxMixValue, FxTimeValue, FxFeedbackValue, FxPot3value);
          // setLED(1, true);	// Fx LED is change in main-loop
          printFxValFrame(myFxSelValue); // clear green Frame
          printFxPOT(myFxSelValue, value);
        }
      }
    }
  }

  // CC control No: 188 Bank select from Midi controller
  else if (control == myBankSelect) {
    if (PrgChangeSW == true) {
      value = value >> 3;
      if (currentPatchBank != value) {
        currentPatchBank = value;
        recallPatch(patchNo);
      }
    }
  }

  // CC control No: 189 Ladder Filter Drive
  else if (control == CCLadderFilterDrive) {
    if (value <= 1) {
      value = 1;
    }
    LadderFilterDrive = value; // store value
    updateLadderFilterDrive(value);
    printDataValue(3, value);
    printRedMarker(3, value);
  }

  // CC control No: 190 Ladder Filter passband Gain
  else if (control == CCLadderFilterPassbandGain) {
    LadderFilterpassbandgain = value; // store value
    updateLadderFilterPassbandGain(value);
    printDataValue(2, value);
    printRedMarker(2, value);
  }

  // CC control No: 203
  else if (control == CCpwmRateA) {
    // Uses combination of PWMRate, PWa and PWb
    pwmRateA = PWMRATE[value];
    updatePWMRateA(); // PWMOD Osc1
    if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
      ParameterNr = 6;
      drawParamterFrame(PageNr, ParameterNr);
      tft.fillRoundRect(133, 76, 22, 10, 2, ST7735_BLUE);
      tft.setCursor(135, 78);
      tft.setTextColor(ST7735_WHITE);
      if (value <= 2) {
        tft.print("PW");
        // tft.fillRoundRect(135,31,17,9,2,ST7735_BLACK);
        draw_Waveform(oscWaveformA, ST7735_RED);
      } else if (value <= 6) {
        tft.print("ENV");
        if (Osc1WaveBank == 0 &&
            (oscWaveformA == 5 || oscWaveformA == 8 || oscWaveformA == 12)) {
          tft.setFont(&Picopixel);
          if (oscWaveformA == 5) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          } else if (oscWaveformA == 8) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          } else if (oscWaveformA == 12) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          }
          tft.print("ENV");
          tft.setFont(NULL);
        }
      } else {
        value = value - 6;
        tft.print(value);
        if (Osc1WaveBank == 0 &&
            (oscWaveformA == 5 || oscWaveformA == 8 || oscWaveformA == 12)) {
          tft.setFont(&Picopixel);
          tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_DARKGREEN);
          tft.setCursor(138, 31);
          tft.print("LFO");
          tft.setFont(NULL);
        }
      }
    } else if (PageNr == 2 && myPageShiftStatus[PageNr] == false) {
      ParameterNr = 6;
      drawParamterFrame(PageNr, ParameterNr);
      tft.fillRoundRect(133, 76, 22, 10, 2, ST7735_BLUE);
      tft.setCursor(135, 78);
      tft.setTextColor(ST7735_WHITE);
      if (value <= 2) {
        tft.print("PW");
        // tft.fillRoundRect(135,31,17,9,2,ST7735_BLACK);
        draw_Waveform(oscWaveformB, ST7735_ORANGE);
      } else if (value <= 6) {
        tft.print("ENV");
        if (Osc2WaveBank == 0 &&
            (oscWaveformB == 5 || oscWaveformB == 8 || oscWaveformB == 12)) {
          tft.setFont(&Picopixel);
          if (oscWaveformB == 5) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          } else if (oscWaveformB == 8) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          } else if (oscWaveformB == 12) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          }
          tft.print("ENV");
          tft.setFont(NULL);
        }
      } else {
        value = value - 6;
        tft.print(value);
        if (Osc2WaveBank == 0 &&
            (oscWaveformB == 5 || oscWaveformB == 8 || oscWaveformB == 12)) {
          tft.setFont(&Picopixel);
          tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_DARKGREEN);
          tft.setCursor(138, 31);
          tft.print("LFO");
          tft.setFont(NULL);
        }
      }
    }
  }

  // CC control No: 203
  else if (control == CCpwmRateB) {
    // Uses combination of PWMRate, PWa and PWb
    pwmRateB = PWMRATE[value];
    updatePWMRateB(); // PWMOD Osc1
    if (PageNr == 1 && myPageShiftStatus[PageNr] == false) {
      ParameterNr = 6;
      drawParamterFrame(PageNr, ParameterNr);
      tft.fillRoundRect(133, 76, 22, 10, 2, ST7735_BLUE);
      tft.setCursor(135, 78);
      tft.setTextColor(ST7735_WHITE);
      if (value <= 2) {
        tft.print("PW");
        // tft.fillRoundRect(135,31,17,9,2,ST7735_BLACK);
        draw_Waveform(oscWaveformA, ST7735_RED);
      } else if (value <= 6) {
        tft.print("ENV");
        if (Osc1WaveBank == 0 &&
            (oscWaveformA == 5 || oscWaveformA == 8 || oscWaveformA == 12)) {
          tft.setFont(&Picopixel);
          if (oscWaveformA == 5) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          } else if (oscWaveformA == 8) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          } else if (oscWaveformA == 12) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          }
          tft.print("ENV");
          tft.setFont(NULL);
        }
      } else {
        value = value - 6;
        tft.print(value);
        if (Osc1WaveBank == 0 &&
            (oscWaveformA == 5 || oscWaveformA == 8 || oscWaveformA == 12)) {
          tft.setFont(&Picopixel);
          tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_DARKGREEN);
          tft.setCursor(138, 31);
          tft.print("LFO");
          tft.setFont(NULL);
        }
      }
    } else if (PageNr == 2 && myPageShiftStatus[PageNr] == false) {
      ParameterNr = 6;
      drawParamterFrame(PageNr, ParameterNr);
      tft.fillRoundRect(133, 76, 22, 10, 2, ST7735_BLUE);
      tft.setCursor(135, 78);
      tft.setTextColor(ST7735_WHITE);
      if (value <= 2) {
        tft.print("PW");
        // tft.fillRoundRect(135,31,17,9,2,ST7735_BLACK);
        draw_Waveform(oscWaveformB, ST7735_ORANGE);
      } else if (value <= 6) {
        tft.print("ENV");
        if (Osc2WaveBank == 0 &&
            (oscWaveformB == 5 || oscWaveformB == 8 || oscWaveformB == 12)) {
          tft.setFont(&Picopixel);
          if (oscWaveformB == 5) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          } else if (oscWaveformB == 8) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          } else if (oscWaveformB == 12) {
            tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_GRAY);
            tft.setCursor(137, 31);
          }
          tft.print("ENV");
          tft.setFont(NULL);
        }
      } else {
        value = value - 6;
        tft.print(value);
        if (Osc2WaveBank == 0 &&
            (oscWaveformB == 5 || oscWaveformB == 8 || oscWaveformB == 12)) {
          tft.setFont(&Picopixel);
          tft.fillRoundRect(135, 25, 17, 9, 2, ST7735_DARKGREEN);
          tft.setCursor(138, 31);
          tft.print("LFO");
          tft.setFont(NULL);
        }
      }
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
    } else if (control <= 100) {
      myCCgroup2(control, value);
    } else if (control <= 150) {
      myCCgroup3(control, value);
    } else if (control <= 300) {
      myCCgroup4(control, value);
    }
  }
}

//*************************************************************************
//
//*************************************************************************
FLASHMEM void myProgramChange(byte channel, byte program) {

  // save channel and program
  myPrgChangeChannel = channel;
  myPrgChangeProgram = program;
  myPrgChangeFlag = true;
}

FLASHMEM void myPrgChange() {

  if (PrgChangeSW == true && myPrgChangeFlag == true) {
    if (patchNo != myPrgChangeProgram + 1) {
      patchNo = myPrgChangeProgram + 1;
      recallPatch(patchNo);
      storeSoundPatchNo(patchNo);
      storePatchBankNo(currentPatchBank);
      if (PageNr > 0) {
        renderCurrentPatchPage(); // refresh menu page
      }
    }
    myPrgChangeFlag = false;
    RefreshMainScreenFlag = true;
  }
}

//*************************************************************************
// close all Enveloopes
//*************************************************************************
FLASHMEM void closeEnvelopes() {
  filterEnvelope1.close();
  filterEnvelope2.close();
  filterEnvelope3.close();
  filterEnvelope4.close();
  filterEnvelope5.close();
  filterEnvelope6.close();
  filterEnvelope7.close();
  filterEnvelope8.close();
  ampEnvelope1.close();
  ampEnvelope2.close();
  ampEnvelope3.close();
  ampEnvelope4.close();
  ampEnvelope5.close();
  ampEnvelope6.close();
  ampEnvelope7.close();
  ampEnvelope8.close();
  LFO1Envelope1.close();
  LFO1Envelope2.close();
  LFO1Envelope3.close();
  LFO1Envelope4.close();
  LFO1Envelope5.close();
  LFO1Envelope6.close();
  LFO1Envelope7.close();
  LFO1Envelope8.close();
  LFO2Envelope1.close();
  LFO2Envelope2.close();
  LFO2Envelope3.close();
  LFO2Envelope4.close();
  LFO2Envelope5.close();
  LFO2Envelope6.close();
  LFO2Envelope7.close();
  LFO2Envelope8.close();
}

//*************************************************************************
// init Patch data
//*************************************************************************
FLASHMEM void set_initPatchData() {
  allNotesOff();
  closeEnvelopes();

  /* 0 */ patchName = INITPATCHNAME;

  if (Keylock == 0) {
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
    /* 17 */ pwmAmtA = 0.00f;
    /* 18 */ pwmAmtB = 0.00f;
    /* 19 */ pwmRate = -10.00f;
    /* 20 */ pwA = -1.00f;
    /* 21 */ pwB = -1.00f;
    if (myFilter == 1) {
      /* 22 */ filterRes = 1.10f; // State Variable Filter
    } else {
      /* 22 */ filterRes = 0.0f; // Ladder Filter
    }
    resonancePrevValue = filterRes; // Pick-up
    /* 23 */ filterFreq = 12000.00f;
    filterfreqPrevValue = filterFreq; // Pick-up
    /* 24 */ filterMix = 0.00f;
    filterMixPrevValue = filterMix; // Pick-up
    /* 25 */ filterEnv = 0.00f;
    /* 26 */ oscLfoAmt = 0.00f;
    oscLfoAmtPrevValue = oscLfoAmt; // PICK-UP
    /* 27 */ oscLfoRate = 1.00f;
    oscLfoRatePrevValue = oscLfoRate; // PICK-UP
    /* 28 */ oscLFOWaveform = 0;
    /* 29 */ oscLfoRetrig = 0;
    /* 30 */ oscLFOMidiClkSync = 0;
    /* 31 */ filterLfoRate = 1.00f;
    filterLfoRatePrevValue = filterLfoRate; // PICK-UP
    /* 32 */ filterLfoRetrig = 0;
    /* 33 */ filterLFOMidiClkSync = 0;
    /* 34 */ filterLfoAmt = 0.00f;
    filterLfoAmtPrevValue = filterLfoAmt; // PICK-UP
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
    fxAmtPrevValue = fxAmt; // PICK-UP
    /* 45 */ fxMix = 0.73f;
    fxMixPrevValue = fxMix; // PICK-UP
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
    /* 64 */ // dummy1
    /* 65 */ // dummy2
    /* 66 */ WShaperNo = 0;
    /* 67 */ WShaperDrive = 1.00f;
    if (WShaperDrive == 0) {
      WShaperDrive = 1.0f;
    }
    LFO1phase = 0.0f;
    LFO2phase = 0.0f;
    oscTranspose = 0;
    oscMasterTune = 1.0f;
    PitchWheelAmt = 1.0f;
    PitchWheelvalue = 127;
    MODWheelAmt = 1.0f;
    MODWheelvalue = 127;
    LadderFilterDrive = 63;
    envelopeType1 = 0;
    envelopeType2 = 0;
    cutoffPickupFlag = false;
    cutoffScreenFlag = false;
    PitchWheelAmt = 0.27f;
    MODWheelAmt = 0.27f;
    myFilter = 1; // State Variable Filter
    pwmRateA = 0.0f;
    pwmRateB = 0.0f;
    LFO1delayTime = 0.0f;
    LFO1fadeTime = 0.0f;
    LFO1releaseTime = 0.0f;
    Osc1ModAmt = 0.0f;
    LFO1mode = 0;
    LFO2mode = 0;

    // Update Parameter -------------------------------
    updatePatchname();
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
    if (myFilter == 2) { // Ladder Filter
      updateLadderFilterPassbandGain(LadderFilterpassbandgain);
      updateLadderFilterDrive(LadderFilterDrive);
    }
    updateFilterMixer();
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
    updateLFO1fade();    // LFO1 FADE-IN
    updateLFO1release(); // LFO1 FADE-OUT
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
    closeEnvelopes();
    renderCurrentPatchPage();
  }
}

//*************************************************************************
// recall (load) Patch
//*************************************************************************
FLASHMEM void recallPatch(int patchNo) {

  if (PageNr == 0) {
    allNotesOff();
    closeEnvelopes();
  }

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
  } else {
    String data[NO_OF_PARAMS]; // Array of data read in
    recallPatchData(patchFile, data);
    setCurrentPatchData(data);
    patchFile.close();
    sendWholePatchAsCC();
  }
}

//*************************************************************************
// recall2 (load) Patch
//*************************************************************************
FLASHMEM void recallPatch2(int patchNo, int patchBank) {

  if (PageNr == 0) {
    allNotesOff();
    closeEnvelopes();
  }

  String numString = (patchNo);
  String bankString = char(patchBank + 65);
  String fileString = (bankString + "/" + numString);
  File patchFile = SD.open(fileString.c_str());
  // Patch unavailable
  if (!patchFile) {
    patchFile.close();
    // Serial.println(F("File not found"));
    //  load init Patch from Flash
    set_initPatchData();
    if (SEQrunStatus == false) {
      initPatternData();
    }
  } else {
    String data[NO_OF_PARAMS]; // Array of data read in
    recallPatchData(patchFile, data);
    setCurrentPatchData(data);
    patchFile.close();
    sendWholePatchAsCC();
  }
}

//*************************************************************************
// recall (load) Sequencer Pattern
//*************************************************************************
FLASHMEM void recallPattern(int patternNo) {

  if (SD.exists("SEQ/")) {
    // Serial.println(F("SEQ-Folder ok"));
  } else {
    Serial.println(F("SEQ-Folder Error!"));
    SDErrorFlag = 1;
  }
  String numString = (patternNo);
  String fileString = ("SEQ/" + numString);
  // Serial.print(fileString);
  File patternFile = SD.open(fileString.c_str());
  // Patch unavailable
  if (!patternFile) {
    patternFile.close();
    // Serial.println(F("Pattern not found"));
    //  load init Patch from Flash
    initPatternData();
  } else {
    String data[NO_OF_SEQ_PARAMS]; // Array of data read in
    recallPatternData(patternFile, data);
    setCurrentPatternData(data);
    patternFile.close();
  }
}

//*************************************************************************
// set Patch data
//*************************************************************************
FLASHMEM void setCurrentPatchData(String data[]) {
  patchName = data[0];
  if (Keylock == true) {
    return;
  }
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

  pwA = data[20].toFloat();
  pwB = data[21].toFloat();
  filterRes = data[22].toFloat();
  resonancePrevValue = filterRes; // Pick-up
  filterFreq = data[23].toInt();
  filterfreqPrevValue = filterFreq; // Pick-up
  filterMix = data[24].toFloat();
  filterMixPrevValue = filterMix; // Pick-up
  filterEnv = data[25].toFloat();
  oscLfoAmt = data[26].toFloat();
  oscLfoAmtPrevValue = oscLfoAmt; // PICK-UP
  oscLfoRate = data[27].toFloat();
  oscLfoRatePrevValue = oscLfoRate; // PICK-UP
  oscLFOWaveform = data[28].toFloat();
  oscLfoRetrig = data[29].toInt();
  oscLFOMidiClkSync = data[30].toFloat(); // MIDI CC Only
  myFilterLFORateValue = data[31].toInt();
  filterLfoRatePrevValue = filterLfoRate; // PICK-UP
  filterLfoRetrig = data[32].toInt();
  filterLFOMidiClkSync = data[33].toInt();
  filterLfoAmt = data[34].toFloat();
  filterLfoAmtPrevValue = filterLfoAmt; // PICK-UP
  if (filterLfoRetrig != 3) {
    filterLfoRate = LFOMAXRATE * POWER[myFilterLFORateValue];
  } else
    filterLfoRate = getLFOTempoRate(myFilterLFORateValue);
  filterLfoWaveform = data[35].toFloat();
  filterAttack = data[36].toFloat();
  filterDecay = data[37].toFloat();
  filterSustain = data[38].toFloat();
  filterRelease = data[39].toFloat();
  ampAttack = data[40].toFloat();
  ampDecay = data[41].toFloat();
  ampSustain = data[42].toFloat();
  ampRelease = data[43].toFloat();
  fxAmt = data[44].toFloat();
  fxAmtPrevValue = fxAmt; // PICK-UP
  fxMix = data[45].toFloat();
  fxMixPrevValue = fxMix; // PICK-UP
  pitchEnv = data[46].toFloat();
  velocitySens = data[47].toFloat();
  chordDetune = data[48].toInt();
  FxPot1value = data[49].toInt();
  FxPot2value = data[50].toInt();
  FxPot3value = data[51].toInt();
  FxPrgNo = data[52].toInt();
  FxMixValue = data[53].toInt();
  if (FxPrgNo == 0) {
    FxMixValue = 0;
  }
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
  dummy1 = data[64].toInt(); // dummy Data
  dummy2 = data[65].toInt(); // dummy Data
  WShaperNo = data[66].toInt();
  WShaperDrive = data[67].toFloat();
  if (WShaperDrive == 0) {
    WShaperDrive = 1.0f;
  }
  LFO1phase = data[68].toFloat();
  LFO2phase = data[69].toFloat();
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

  if (SEQrunStatus == true) {
    SeqSymbol = false;
    for (uint8_t i = 0; i < 16; i++) {
      int dataAddr = 175 + i;
      uint8_t Ncount = data[dataAddr].toInt();
      if (Ncount > 0) {
        SeqSymbol = true;
        break;
      }
    }
  }

  // if sequencer off check Sequencer notes avaible ---------------------
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
    // load Sequencer notes -------------------------------------------
    if (SeqNotesAvailable == true) {

      for (uint8_t i = 0; i < 16; i++) { // 1.Notes
        int x = 70 + i;
        SeqNote1Buf[i] = data[x].toInt();
      }
      for (uint8_t i = 0; i < 16; i++) {
        int x = 86 + i;
        SeqNoteBufStatus[i] = data[x].toInt();
      }
      SEQbpmValue = data[102].toInt();
      SEQdivValue = data[103].toFloat();
      float bpm = (SEQbpmValue / SEQdivValue);
      SEQclkRate = (60000000 / bpm);
      SEQstepNumbers = data[104].toInt();
      SEQselectStepNo = 0;
      SEQGateTime = data[105].toFloat();
      if (SEQGateTime == 0) {
        SEQGateTime = (16.01f - 12.0683f); // Pot Value 32
      }
      Interval = SEQclkRate;
      gateTime = (float)(SEQclkRate / SEQGateTime);
      SEQdirection = data[106].toInt();
      SEQdirectionFlag = false;

      // Sequencer Velocity data ------------------------------------
      for (uint8_t i = 0; i < 16; i++) {
        int dataAddr = i + 111;
        SeqVeloBuf[i] = data[dataAddr].toInt();
      }

      // Sequencer Noten Buffer 2.Note, 3.Note, 4.Note --------------
      for (uint8_t i = 0; i < 48; i++) {
        int bufAddr = i + 16;
        int dataAddr = i + 127;
        SeqNote1Buf[bufAddr] = data[dataAddr].toInt();
      }
      // Sequencer Noten counts -------------------------------------
      for (uint8_t i = 0; i < 16; i++) {
        int dataAddr = 175 + i;
        SeqNoteCount[i] = data[dataAddr].toInt();
      }
    } else if (SEQrunStatus == false) {
      initPatternData();
    }

    // Sequencer function ---------------------------------------------
    SEQmode = data[191].toInt();
    if (SEQmode >= 2) {
      SEQmode = 2;
    }

    SEQMidiClkSwitch = data[192].toInt();
  }

  // Ladder Filter --------------------------------------------------
  LadderFilterpassbandgain = data[193].toInt();
  LadderFilterDrive = data[194].toInt();
  if (LadderFilterDrive < 1) { // old Patches
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
  LFO2mode = data[212].toInt();

  // Update parameter ---------------------------------------------------

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
  updatePatchname();
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
  if (myFilter == 2) { // Ladder Filter
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
  updateLFO1fade();    // LFO1 FADE-IN
  updateLFO2release(); // LFO1 FADE-OUT
  updateLFO2fade();    // LFO2 FADE-IN
  updateLFO1release(); // LFO2 FADE-OUT
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
  closeEnvelopes();
}

//*************************************************************************
// load Patch data
//*************************************************************************
FLASHMEM String getCurrentPatchData() {

  return patchName + "," + String(oscALevel) + "," + String(oscBLevel) + "," +
         String(noiseLevel) + "," + String(unison) + "," + String(oscFX) + "," +
         String(detune, 5) + "," + String(lfoSyncFreq) + "," +
         String(midiClkTimeInterval) + "," + String(lfoTempoValue) + "," +
         String(keytrackingAmount, 4) + "," + String(glideSpeed, 5) + "," +
         String(oscPitchA) + "," + String(oscPitchB) + "," +
         String(oscWaveformA) + "," + String(oscWaveformB) + "," +
         String(pwmSource) + "," + String(pwmAmtA) + "," + String(pwmAmtB) +
         "," + String(pwmRate) + "," + String(pwA) + "," + String(pwB) + "," +
         String(filterRes) + "," + String(filterFreq) + "," +
         String(filterMix) + "," + String(filterEnv) + "," +
         String(oscLfoAmt, 5) + "," + String(oscLfoRate, 5) + "," +
         String(oscLFOWaveform) + "," + String(oscLfoRetrig) + "," +
         String(oscLFOMidiClkSync) + "," + String(myFilterLFORateValue) + "," +
         String(filterLfoRetrig) + "," + String(filterLFOMidiClkSync) + "," +
         String(filterLfoAmt) + "," + String(filterLfoWaveform) + "," +
         String(filterAttack) + "," + String(filterDecay) + "," +
         String(filterSustain) + "," + String(filterRelease) + "," +
         String(ampAttack) + "," + String(ampDecay) + "," + String(ampSustain) +
         "," + String(ampRelease) + "," + String(fxAmt) + "," + String(fxMix) +
         "," + String(pitchEnv) + "," + String(velocitySens) + "," +
         String(chordDetune) + "," + String(FxPot1value) + "," +
         String(FxPot2value) + "," + String(FxPot3value) + "," +
         String(FxPrgNo) + "," + String(FxMixValue) + "," + String(FxClkRate) +
         "," + String(Osc1WaveBank) + "," + String(Osc2WaveBank) + "," +
         String(myBoost) + "," + String(pitchEnvA, 5) + "," +
         String(pitchEnvB, 5) + "," + String(driveLevel, 3) + "," +
         String(myFilVelocity, 4) + "," + String(myAmpVelocity, 4) + "," +
         String(myUnisono) + "," + String(dummy1) + "," + String(dummy2) + "," +
         String(WShaperNo) + "," + String(WShaperDrive, 3) + "," +
         String(LFO1phase) + "," +
         String(LFO2phase)
         // Sequencer Pattern
         + "," + String(SeqNote1Buf[0]) + "," + String(SeqNote1Buf[1]) + "," +
         String(SeqNote1Buf[2]) + "," + String(SeqNote1Buf[3]) + "," +
         String(SeqNote1Buf[4]) + "," + String(SeqNote1Buf[5]) + "," +
         String(SeqNote1Buf[6]) + "," + String(SeqNote1Buf[7]) + "," +
         String(SeqNote1Buf[8]) + "," + String(SeqNote1Buf[9]) + "," +
         String(SeqNote1Buf[10]) + "," + String(SeqNote1Buf[11]) + "," +
         String(SeqNote1Buf[12]) + "," + String(SeqNote1Buf[13]) + "," +
         String(SeqNote1Buf[14]) + "," + String(SeqNote1Buf[15]) + "," +
         String(SeqNoteBufStatus[0]) + "," + String(SeqNoteBufStatus[1]) + "," +
         String(SeqNoteBufStatus[2]) + "," + String(SeqNoteBufStatus[3]) + "," +
         String(SeqNoteBufStatus[4]) + "," + String(SeqNoteBufStatus[5]) + "," +
         String(SeqNoteBufStatus[6]) + "," + String(SeqNoteBufStatus[7]) + "," +
         String(SeqNoteBufStatus[8]) + "," + String(SeqNoteBufStatus[9]) + "," +
         String(SeqNoteBufStatus[10]) + "," + String(SeqNoteBufStatus[11]) +
         "," + String(SeqNoteBufStatus[12]) + "," +
         String(SeqNoteBufStatus[13]) + "," + String(SeqNoteBufStatus[14]) +
         "," + String(SeqNoteBufStatus[15]) + "," + String(SEQbpmValue) + "," +
         String(SEQdivValue, 8) + "," + String(SEQstepNumbers) + "," +
         String(SEQGateTime, 6) + "," + String(SEQdirection) + "," +
         String(oscDetuneSync) + "," + String(oscTranspose) + "," +
         String(oscMasterTune, 6) + "," + String(OscVCFMOD, 6) + "," +
         String(SeqVeloBuf[0]) + "," + String(SeqVeloBuf[1]) + "," +
         String(SeqVeloBuf[2]) + "," + String(SeqVeloBuf[3]) + "," +
         String(SeqVeloBuf[4]) + "," + String(SeqVeloBuf[5]) + "," +
         String(SeqVeloBuf[6]) + "," + String(SeqVeloBuf[7]) + "," +
         String(SeqVeloBuf[8]) + "," + String(SeqVeloBuf[9]) + "," +
         String(SeqVeloBuf[10]) + "," + String(SeqVeloBuf[11]) + "," +
         String(SeqVeloBuf[12]) + "," + String(SeqVeloBuf[13]) + "," +
         String(SeqVeloBuf[14]) + "," + String(SeqVeloBuf[15]) + "," +
         String(SeqNote1Buf[16]) + "," + String(SeqNote1Buf[17]) + "," +
         String(SeqNote1Buf[18]) + "," + String(SeqNote1Buf[19]) + "," +
         String(SeqNote1Buf[20]) + "," + String(SeqNote1Buf[21]) + "," +
         String(SeqNote1Buf[22]) + "," + String(SeqNote1Buf[23]) + "," +
         String(SeqNote1Buf[24]) + "," + String(SeqNote1Buf[25]) + "," +
         String(SeqNote1Buf[26]) + "," + String(SeqNote1Buf[27]) + "," +
         String(SeqNote1Buf[28]) + "," + String(SeqNote1Buf[29]) + "," +
         String(SeqNote1Buf[30]) + "," + String(SeqNote1Buf[31]) + "," +
         String(SeqNote1Buf[32]) + "," + String(SeqNote1Buf[33]) + "," +
         String(SeqNote1Buf[34]) + "," + String(SeqNote1Buf[35]) + "," +
         String(SeqNote1Buf[36]) + "," + String(SeqNote1Buf[37]) + "," +
         String(SeqNote1Buf[38]) + "," + String(SeqNote1Buf[39]) + "," +
         String(SeqNote1Buf[40]) + "," + String(SeqNote1Buf[41]) + "," +
         String(SeqNote1Buf[42]) + "," + String(SeqNote1Buf[43]) + "," +
         String(SeqNote1Buf[44]) + "," + String(SeqNote1Buf[45]) + "," +
         String(SeqNote1Buf[46]) + "," + String(SeqNote1Buf[47]) + "," +
         String(SeqNote1Buf[48]) + "," + String(SeqNote1Buf[49]) + "," +
         String(SeqNote1Buf[50]) + "," + String(SeqNote1Buf[51]) + "," +
         String(SeqNote1Buf[52]) + "," + String(SeqNote1Buf[53]) + "," +
         String(SeqNote1Buf[54]) + "," + String(SeqNote1Buf[55]) + "," +
         String(SeqNote1Buf[56]) + "," + String(SeqNote1Buf[57]) + "," +
         String(SeqNote1Buf[58]) + "," + String(SeqNote1Buf[59]) + "," +
         String(SeqNote1Buf[60]) + "," + String(SeqNote1Buf[61]) + "," +
         String(SeqNote1Buf[62]) + "," + String(SeqNote1Buf[63]) + "," +
         String(SeqNoteCount[0]) + "," + String(SeqNoteCount[1]) + "," +
         String(SeqNoteCount[2]) + "," + String(SeqNoteCount[3]) + "," +
         String(SeqNoteCount[4]) + "," + String(SeqNoteCount[5]) + "," +
         String(SeqNoteCount[6]) + "," + String(SeqNoteCount[7]) + "," +
         String(SeqNoteCount[8]) + "," + String(SeqNoteCount[9]) + "," +
         String(SeqNoteCount[10]) + "," + String(SeqNoteCount[11]) + "," +
         String(SeqNoteCount[12]) + "," + String(SeqNoteCount[13]) + "," +
         String(SeqNoteCount[14]) + "," + String(SeqNoteCount[15]) + "," +
         String(SEQmode) + "," + String(SEQMidiClkSwitch) + "," +
         String(LadderFilterpassbandgain) + "," + String(LadderFilterDrive) +
         "," + String(envelopeType1) + "," + String(envelopeType2) + "," +
         String(PitchWheelAmt) + "," + String(MODWheelAmt) + "," +
         String(myFilter) + "," + String(pwmRateA) + "," + String(pwmRateB) +
         "," + String(LFO1fadeTime) + "," + String(LFO1releaseTime) + "," +
         String(filterFM) + "," + String(filterFM2) + "," +
         String(LFO2fadeTime) + "," + String(LFO2releaseTime) + "," +
         String(Osc1ModAmt) + "," + String(LFO1envCurve) + "," +
         String(LFO2envCurve) + "," + String(LFO1mode) + "," + String(LFO2mode);
}
//************************************************************************
// load Sequencer Patch data
//************************************************************************
FLASHMEM String getCurrentPatternData() {

  return patternName + "," + String(SeqNote1Buf[0]) + "," +
         String(SeqNote1Buf[1]) + "," + String(SeqNote1Buf[2]) + "," +
         String(SeqNote1Buf[3]) + "," + String(SeqNote1Buf[4]) + "," +
         String(SeqNote1Buf[5]) + "," + String(SeqNote1Buf[6]) + "," +
         String(SeqNote1Buf[7]) + "," + String(SeqNote1Buf[8]) + "," +
         String(SeqNote1Buf[9]) + "," + String(SeqNote1Buf[10]) + "," +
         String(SeqNote1Buf[11]) + "," + String(SeqNote1Buf[12]) + "," +
         String(SeqNote1Buf[13]) + "," + String(SeqNote1Buf[14]) + "," +
         String(SeqNote1Buf[15]) + "," + String(SeqNoteBufStatus[0]) + "," +
         String(SeqNoteBufStatus[1]) + "," + String(SeqNoteBufStatus[2]) + "," +
         String(SeqNoteBufStatus[3]) + "," + String(SeqNoteBufStatus[4]) + "," +
         String(SeqNoteBufStatus[5]) + "," + String(SeqNoteBufStatus[6]) + "," +
         String(SeqNoteBufStatus[7]) + "," + String(SeqNoteBufStatus[8]) + "," +
         String(SeqNoteBufStatus[9]) + "," + String(SeqNoteBufStatus[10]) +
         "," + String(SeqNoteBufStatus[11]) + "," +
         String(SeqNoteBufStatus[12]) + "," + String(SeqNoteBufStatus[13]) +
         "," + String(SeqNoteBufStatus[14]) + "," +
         String(SeqNoteBufStatus[15]) + "," + String(SEQbpmValue) + "," +
         String(SEQdivValue, 8) + "," + String(SEQstepNumbers) + "," +
         String(SEQGateTime, 6) + "," + String(SEQdirection) + "," +
         String(SeqVeloBuf[0]) + "," + String(SeqVeloBuf[1]) + "," +
         String(SeqVeloBuf[2]) + "," + String(SeqVeloBuf[3]) + "," +
         String(SeqVeloBuf[4]) + "," + String(SeqVeloBuf[5]) + "," +
         String(SeqVeloBuf[6]) + "," + String(SeqVeloBuf[7]) + "," +
         String(SeqVeloBuf[8]) + "," + String(SeqVeloBuf[9]) + "," +
         String(SeqVeloBuf[10]) + "," + String(SeqVeloBuf[11]) + "," +
         String(SeqVeloBuf[12]) + "," + String(SeqVeloBuf[13]) + "," +
         String(SeqVeloBuf[14]) + "," + String(SeqVeloBuf[15]) + "," +
         String(SeqNote1Buf[16]) + "," + String(SeqNote1Buf[17]) + "," +
         String(SeqNote1Buf[18]) + "," + String(SeqNote1Buf[19]) + "," +
         String(SeqNote1Buf[20]) + "," + String(SeqNote1Buf[21]) + "," +
         String(SeqNote1Buf[22]) + "," + String(SeqNote1Buf[23]) + "," +
         String(SeqNote1Buf[24]) + "," + String(SeqNote1Buf[25]) + "," +
         String(SeqNote1Buf[26]) + "," + String(SeqNote1Buf[27]) + "," +
         String(SeqNote1Buf[28]) + "," + String(SeqNote1Buf[29]) + "," +
         String(SeqNote1Buf[30]) + "," + String(SeqNote1Buf[31]) + "," +
         String(SeqNote1Buf[32]) + "," + String(SeqNote1Buf[33]) + "," +
         String(SeqNote1Buf[34]) + "," + String(SeqNote1Buf[35]) + "," +
         String(SeqNote1Buf[36]) + "," + String(SeqNote1Buf[37]) + "," +
         String(SeqNote1Buf[38]) + "," + String(SeqNote1Buf[39]) + "," +
         String(SeqNote1Buf[40]) + "," + String(SeqNote1Buf[41]) + "," +
         String(SeqNote1Buf[42]) + "," + String(SeqNote1Buf[43]) + "," +
         String(SeqNote1Buf[44]) + "," + String(SeqNote1Buf[45]) + "," +
         String(SeqNote1Buf[46]) + "," + String(SeqNote1Buf[47]) + "," +
         String(SeqNote1Buf[48]) + "," + String(SeqNote1Buf[49]) + "," +
         String(SeqNote1Buf[50]) + "," + String(SeqNote1Buf[51]) + "," +
         String(SeqNote1Buf[52]) + "," + String(SeqNote1Buf[53]) + "," +
         String(SeqNote1Buf[54]) + "," + String(SeqNote1Buf[55]) + "," +
         String(SeqNote1Buf[56]) + "," + String(SeqNote1Buf[57]) + "," +
         String(SeqNote1Buf[58]) + "," + String(SeqNote1Buf[59]) + "," +
         String(SeqNote1Buf[60]) + "," + String(SeqNote1Buf[61]) + "," +
         String(SeqNote1Buf[62]) + "," + String(SeqNote1Buf[63]) + "," +
         String(SeqNoteCount[0]) + "," + String(SeqNoteCount[1]) + "," +
         String(SeqNoteCount[2]) + "," + String(SeqNoteCount[3]) + "," +
         String(SeqNoteCount[4]) + "," + String(SeqNoteCount[5]) + "," +
         String(SeqNoteCount[6]) + "," + String(SeqNoteCount[7]) + "," +
         String(SeqNoteCount[8]) + "," + String(SeqNoteCount[9]) + "," +
         String(SeqNoteCount[10]) + "," + String(SeqNoteCount[11]) + "," +
         String(SeqNoteCount[12]) + "," + String(SeqNoteCount[13]) + "," +
         String(SeqNoteCount[14]) + "," + String(SeqNoteCount[15]);
}

//************************************************************************
// set Sequencer Pattern data
//************************************************************************
FLASHMEM void setCurrentPatternData(String data[]) {

  patternName = data[0];

  if (Keylock == false) {

    // Sequencer Step 1.Note ------------------------------------------
    for (uint8_t i = 0; i < 16; i++) {
      int x = 1 + i;
      SeqNote1Buf[i] = data[x].toInt();
    }
    for (uint8_t i = 0; i < 16; i++) {
      int x = 17 + i;
      SeqNoteBufStatus[i] = data[x].toInt();
    }

    SEQbpmValue = data[33].toInt();
    SEQdivValue = data[34].toFloat();
    float bpm = (SEQbpmValue / SEQdivValue);
    SEQclkRate = (60000000 / bpm);
    SEQstepNumbers = data[35].toInt();
    SEQGateTime = data[36].toFloat();
    if (SEQGateTime == 0) {
      SEQGateTime = (16.01f - 12.0683f); // Pot Value 32
    }
    Interval = SEQclkRate;
    gateTime = (float)(SEQclkRate / SEQGateTime);
    SEQdirection = data[37].toInt();
    SEQdirectionFlag = false;
    // Sequencer Velocity data ----------------------------------------
    for (uint8_t i = 0; i < 16; i++) {
      int x = 38 + i;
      SeqVeloBuf[i] = data[x].toInt();
    }
    // Sequencer Step 2.Note 3.Note 4.Note ----------------------------
    for (uint8_t i = 0; i < 48; i++) {
      int x = 54 + i;
      int addr = 16 + i;
      SeqNote1Buf[addr] = data[x].toInt();
    }
    // Sequencer Step note count --------------------------------------
    for (uint8_t i = 0; i < 16; i++) {
      int x = 102 + i;
      SeqNoteCount[i] = data[x].toInt();
    }
  }
}

//************************************************************************
// int Sequencer Pattern data
//************************************************************************
FLASHMEM void initPatternData() {
  patternName = INITPATTERNNAME;
  if (Keylock == true) {
    return;
  }
  for (uint8_t i = 0; i < 64; i++) {
    SeqNote1Buf[i] = 0; // Midi Note C0
  }
  for (uint8_t i = 0; i < 16; i++) {
    SeqNoteCount[i] = 0; // Step note count
  }
  for (uint8_t i = 0; i < 16; i++) {
    SeqVeloBuf[i] = 0; // Velocity
  }
  for (uint8_t i = 0; i < 16; i++) {
    SeqNoteBufStatus[i] = false; // all Notes mute
  }

  SEQbpmValue = 120;         // BPM = 120
  SEQdivValue = 1.00000000f; // DIV = 1/4
  float bpm = float(SEQbpmValue / SEQdivValue);
  SEQclkRate = (60000000 / bpm);
  SEQstepNumbers = 15; // 16 Steps
  Interval = SEQclkRate;
  gateTime = (float)(SEQclkRate / 2.0f);
  SEQdirection = 0;
  SEQdirectionFlag = false;
  SeqSymbol = false;
}

//*************************************************************************
// seleced Paramter
//*************************************************************************
FLASHMEM int selecdParameter(uint8_t PageNr, uint8_t ParameterNr) {
  int parameter = 0;

  if (PageNr == 1) {

    if (myPageShiftStatus[PageNr] == false) { // Osc1 main menu
      switch (ParameterNr) {
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
        parameter = myDrive;
        break;
      case 5:
        parameter = CCpwA;
        break;
      case 6:
        parameter = CCpwmRateA;
        break;
      case 7:
        parameter = myOscMix;
        break;
      }
    }
    // Osc1 sub menu
    else {
      switch (ParameterNr) {
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

  if (PageNr == 2) {
    if (myPageShiftStatus[PageNr] == false) { // shift key disabled
      switch (ParameterNr) {
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
        parameter = CCpwB;
        break;
      case 6:
        parameter = CCpwmRateB;
        break;
      case 7:
        parameter = myOscMix;
        break;
      }
    }
    // shift key enabled
    else {
      switch (ParameterNr) {
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

  if (PageNr == 11) {
    switch (ParameterNr) {
    case 5:
      parameter = myMidiCha;
      break;
    case 6:
      parameter = myVelocity;
      break;
    case 7:
      parameter = myFilterSwitch;
      break;
    case 8:
      parameter = myMidiSyncSwitch;
      break;
    case 9:
      parameter = myPRGChange;
      break;
    }
  }

  return parameter;
}

//*************************************************************************
// check Potentiometers
//*************************************************************************
FLASHMEM void checkPots(void) {
  uint16_t parameter = 0;

  // Page:1 (Main) ----------------------------------------------------
  if (PageNr == 0) {
    // read Pot1 (change Patch Bank)
    mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
    if (mux1Read > (mux1ValuesPrev[0] + (QUANTISE_FACTOR * 4)) ||
        mux1Read < (mux1ValuesPrev[0] - (QUANTISE_FACTOR * 4))) {
      mux1ValuesPrev[0] = mux1Read;
      int mux1Readtemp = (mux1Read >> 8);
      parameter = setPatchBank;
      renderCurrentParameter(PageNr, parameter, mux1Readtemp);
    }

    // read Pot2 (non)
    mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
    if (mux2Read > (mux2ValuesPrev[0] + (QUANTISE_FACTOR * 4)) ||
        mux2Read < (mux2ValuesPrev[0] - (QUANTISE_FACTOR * 4))) {
      mux2ValuesPrev[0] = mux2Read;
    }

    // read Pot3 (non)
    mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
    if (mux3Read > (mux3ValuesPrev[0] + (QUANTISE_FACTOR * 4)) ||
        mux3Read < (mux3ValuesPrev[0] - (QUANTISE_FACTOR * 4))) {
      mux3ValuesPrev[0] = mux3Read;
    }

    // read Pot4 (Cutoff value)
    mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
    // higher resolution for cutoff
    if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux4ValuesPrev[0] = mux4Read;
      int mux4Readtemp = (mux4Read >> 4);
      static int count = 0;
      cutoffScreenFlag = true;
      myCCgroup2(CCfilterfreq, mux4Readtemp);
      sendCC(midiChannel, CCfilterfreq, mux4Readtemp);
    }
  }

  // Page:1 (Osc1) ----------------------------------------------------
  if (PageNr == 1) {

    initPotentiometers();

    // Osc1 Main page -----------------------------------
    if (myPageShiftStatus[PageNr] == false) {

      // read Pot1	(select Parameter)
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        int mux1Readtemp = (mux1Read >> 5);
        uint16_t tempParameterNr = (0.062f * mux1Readtemp); // 8 rows
        if (ParameterNr != tempParameterNr) {
          ParameterNr = tempParameterNr;
          drawParamterFrame(PageNr, ParameterNr);
          ParameterNrMem[PageNr] = ParameterNr;
          ParmSelectFlag = false;
        }
      }
      // read Pot2	(change Parameter)
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        int mux2Readtemp = (mux2Read >> 5);
        parameter = selecdParameter(PageNr, ParameterNr);
        renderCurrentParameter(PageNr, parameter, mux2Readtemp);
        myControlChange(midiChannel, parameter, mux2Readtemp);
        sendCC(midiChannel, parameter, mux2Readtemp);
      }
      // read Pot3	(Waveform Bank)
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
        int mux3Readtemp = (mux3Read >> 5);
        if (ParameterNr == 0) {
          parameter = CCosc1WaveBank;
          // renderCurrentParameter(PageNr,parameter,mux3Readtemp);
          myCCgroup1(CCosc1WaveBank, mux3Readtemp);
          sendCC(midiChannel, CCosc1WaveBank, mux3Readtemp);
        }
      }
    }
    // Osc1 Subpage -----------------------------------
    else {
      // read Pot1	(select Parameter)
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        int mux1Readtemp = (mux1Read >> 5);
        uint16_t tempParameterNr = (0.062f * mux1Readtemp); // 8 rows
        if (ParameterNr != tempParameterNr) {
          ParameterNr = tempParameterNr;
          drawParamterFrame(PageNr, ParameterNr);
          ParameterNrMem[3] = ParameterNr;
          ParmSelectFlag = false;
        }
      }
      // read Pot2	(change Parameter)
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        int mux2Readtemp = (mux2Read >> 5);
        parameter = selecdParameter(PageNr, ParameterNr);
        renderCurrentParameter(PageNr, parameter, mux2Readtemp);
        myControlChange(midiChannel, parameter, mux2Readtemp);
        sendCC(midiChannel, parameter, mux2Readtemp);
      }
    }
  }

  // Page:2 (Osc2) ----------------------------------------------------
  else if (PageNr == 2) {

    // shift key disabled -----------------------------------
    if (myPageShiftStatus[PageNr] == false) {
      // select parameter
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        int mux1Readtemp = (mux1Read >> 5);
        uint8_t tempParameterNr = (0.062f * mux1Readtemp); // 8 rows
        if (ParameterNr != tempParameterNr) {
          ParameterNr = tempParameterNr;
          drawParamterFrame(PageNr, ParameterNr);
          ParameterNrMem[PageNr] = ParameterNr;
          ParmSelectFlag = false;
        }
      }
      // change parameter
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        int mux2Readtemp = (mux2Read >> 5);
        parameter = selecdParameter(PageNr, ParameterNr);
        renderCurrentParameter(PageNr, parameter, mux2Readtemp);
        myControlChange(midiChannel, parameter, mux2Readtemp);
        sendCC(midiChannel, parameter, mux2Readtemp);
      }
      // read Pot3	(select Waveform Bank)
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
        int mux3Readtemp = (mux3Read >> 5);
        if (ParameterNr == 0) {
          parameter = CCosc2WaveBank;
          // renderCurrentParameter(PageNr,parameter,mux3Readtemp);
          myCCgroup1(CCosc2WaveBank, mux3Readtemp);
          sendCC(midiChannel, CCosc2WaveBank, mux3Readtemp);
        }
      }
    }

    // shift key enabed -----------------------------------
    else {
      // read Pot1	(select Parameter)
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        int mux1Readtemp = (mux1Read >> 5);
        uint16_t tempParameterNr = (0.062f * mux1Readtemp); // 8 rows
        if (ParameterNr != tempParameterNr) {
          ParameterNr = tempParameterNr;
          drawParamterFrame(PageNr, ParameterNr);
          ParameterNrMem[3] = ParameterNr;
          ParmSelectFlag = false;
        }
      }
      // read Pot2	(change Parameter)
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        int mux2Readtemp = (mux2Read >> 5);
        parameter = selecdParameter(PageNr, ParameterNr);
        renderCurrentParameter(PageNr, parameter, mux2Readtemp);
        myControlChange(midiChannel, parameter, mux2Readtemp);
        sendCC(midiChannel, parameter, mux2Readtemp);
      }
    }
  }

  // Page:3 (Filter) --------------------------------------------------
  else if (PageNr == 3) {

    // State Variable Filter Main Page
    if (myFilter == 1) {
      if (myPageShiftStatus[PageNr] == false) {

        // change Cutoff
        mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
        if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux1ValuesPrev[0] = mux1Read;
          int mux1Readtemp = (mux1Read >> 4);
          parameter = CCfilterfreq;
          FilterCut = mux1Readtemp;
          myCCgroup2(CCfilterfreq, FilterCut);
          sendCC(midiChannel, CCfilterfreq, FilterCut);
          if (PageNr >= 1) {
            cutoffPickupFlag = false;
            filterfreqPrevValue = FILTERFREQS256[FilterCut];
          }
        }
        // change Resonanc
        mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
        if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux2ValuesPrev[0] = mux2Read;
          int mux2Readtemp = (mux2Read >> 5);
          FilterRes = mux2Readtemp;
          parameter = CCfilterres;
          myCCgroup2(CCfilterres, FilterRes);
          sendCC(midiChannel, CCfilterres, FilterRes);
        }
        // change Filter Amount
        mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
        if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux3ValuesPrev[0] = mux3Read;
          int mux3Readtemp = (mux3Read >> 4);
          parameter = CCfilterenv;
          renderCurrentParameter(PageNr, parameter, mux3Readtemp);
          myControlChange(midiChannel, parameter, mux3Readtemp >> 1);
          sendCC(midiChannel, parameter, mux3Readtemp >> 1);
          draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
        }
        // change Filter typ
        mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
        if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux4ValuesPrev[0] = mux4Read;
          int mux4Readtemp = (mux4Read >> 5);
          FilterMix = mux4Readtemp;
          parameter = CCfiltermixer;
          renderCurrentParameter(PageNr, parameter, mux4Readtemp);
          myCCgroup1(CCfiltermixer, FilterMix);
          sendCC(midiChannel, CCfiltermixer, FilterMix);
        }
      }
      // State Variable Filter SUB Page ------------------------------------
      else {
        // Keytracking
        mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
        if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux1ValuesPrev[0] = mux1Read;
          int mux1Readtemp = (mux1Read >> 5);
          parameter = CCkeytracking;
          renderCurrentParameter(PageNr, parameter, mux1Readtemp);
          myControlChange(midiChannel, parameter, mux1Readtemp);
          sendCC(midiChannel, parameter, mux1Readtemp);
        }
        // Velocity
        mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
        if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux2ValuesPrev[0] = mux2Read;
          int mux2Readtemp = (mux2Read >> 5); // 0-127
          parameter = myFilterVelo;
          renderCurrentParameter(PageNr, parameter, mux2Readtemp);
          myControlChange(midiChannel, parameter, mux2Readtemp);
          sendCC(midiChannel, parameter, mux2Readtemp);
        }
        // LFO2 AMT
        mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
        if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux3ValuesPrev[0] = mux3Read;
          int mux3Readtemp = (mux3Read >> 4);
          parameter = myLFO2amt;
          renderCurrentParameter(PageNr, parameter, mux3Readtemp);
          draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
        }
        // non
        mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
        if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux4ValuesPrev[0] = mux4Read;
        }
      }
    }

    // Ladder Filter Main Page --------------------------------------
    else {
      if (myPageShiftStatus[PageNr] == false) {

        // change Cutoff
        mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
        if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux1ValuesPrev[0] = mux1Read;
          int mux1Readtemp = (mux1Read >> 4);
          parameter = CCfilterfreq;
          FilterCut = mux1Readtemp;
          myCCgroup2(CCfilterfreq, FilterCut);
          sendCC(midiChannel, CCfilterfreq, FilterCut);
        }
        // change Resonanc
        mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
        if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux2ValuesPrev[0] = mux2Read;
          int mux2Readtemp = (mux2Read >> 5);
          FilterRes = mux2Readtemp;
          parameter = CCfilterres;
          myCCgroup2(CCfilterres, FilterRes);
          sendCC(midiChannel, CCfilterres, FilterRes);
        }
        // Ladder Filter Envelope amount
        mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
        if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux3ValuesPrev[0] = mux3Read;
          int mux3Readtemp = (mux3Read >> 4);
          parameter = CCfilterenv;
          myCCgroup1(CCfilterenv, mux3Readtemp >> 1);
          sendCC(midiChannel, CCfilterenv, mux3Readtemp >> 1);
          draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
        }
        // change Ladder Filter Drive
        mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
        if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux4ValuesPrev[0] = mux4Read;
          int mux4Readtemp = (mux4Read >> 5);
          LadderFilterDrive = mux4Readtemp;
          parameter = CCLadderFilterDrive;
          myCCgroup4(CCLadderFilterDrive, LadderFilterDrive);
          sendCC(midiChannel, CCLadderFilterDrive, LadderFilterDrive);
        }
      }
      // Ladder Filter SUB page -----------------------------
      else {
        // change Filter Env Amount
        mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
        if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux1ValuesPrev[0] = mux1Read;
          int mux1Readtemp = (mux1Read >> 5);
          parameter = CCkeytracking;
          renderCurrentParameter(PageNr, parameter, mux1Readtemp);
          myControlChange(midiChannel, parameter, mux1Readtemp);
          sendCC(midiChannel, parameter, mux1Readtemp);
        }
        // Filter Velocity
        mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
        if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux2ValuesPrev[0] = mux2Read;
          int mux2Readtemp = (mux2Read >> 5); // 0-127
          parameter = myFilterVelo;
          renderCurrentParameter(PageNr, parameter, mux2Readtemp);
          myControlChange(midiChannel, parameter, mux2Readtemp);
          sendCC(midiChannel, parameter, mux2Readtemp);
        }
        // change Filter PassBandGain
        mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
        if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux3ValuesPrev[0] = mux3Read;
          int mux3Readtemp = (mux3Read >> 5);
          LadderFilterpassbandgain = mux3Readtemp;
          parameter = CCLadderFilterPassbandGain;
          myCCgroup4(CCLadderFilterPassbandGain, LadderFilterpassbandgain);
          sendCC(midiChannel, CCLadderFilterPassbandGain,
                 LadderFilterpassbandgain);
        }
        // LFO2 AMT
        mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
        if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
            mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
          mux4ValuesPrev[0] = mux4Read;
          int mux4Readtemp = (mux4Read >> 4);
          parameter = myLFO2amt;
          myCCgroup3(myLFO2amt, mux4Readtemp >> 1);
          sendCC(midiChannel, myLFO2amt, mux4Readtemp >> 1);

          draw_filter_curves(FilterCut >> 1, FilterRes, FilterMix);
        }
      }
    }
  }

  // Page:4 (Filter Envelope) -----------------------------------------
  else if (PageNr == 4) {
    // shift key disabled -----------------------------------
    if (myPageShiftStatus[PageNr] == false) {
      // Attack
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        myControlChange(midiChannel, CCfilterattack, (mux1Read >> 5));
        sendCC(midiChannel, CCfilterattack, (mux1Read >> 5));
      }
      // Decay
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        myControlChange(midiChannel, CCfilterdecay, (mux2Read >> 5));
        sendCC(midiChannel, CCfilterdecay, (mux2Read >> 5));
      }
      // Sustain
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
        myControlChange(midiChannel, CCfiltersustain, (mux3Read >> 5));
        sendCC(midiChannel, CCfiltersustain, (mux3Read >> 5));
      }
      // Release
      mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
      if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux4ValuesPrev[0] = mux4Read;
        myControlChange(midiChannel, CCfilterrelease, (mux4Read >> 5));
        sendCC(midiChannel, CCfilterrelease, (mux4Read >> 5));
      }

    }
    // shift key enabled -----------------------------------
    else {
      // Envelope curve
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        int mux1Readtemp = (mux1Read >> 5);
        parameter = myFilterEnvCurve;
        renderCurrentParameter(PageNr, parameter, mux1Readtemp);
      }
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
      }
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
      }
      mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
      if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux4ValuesPrev[0] = mux4Read;
      }
    }
  }

  // Page:5 (AMP Envelope) ----------------------------------------------
  else if (PageNr == 5) {
    // shift key disabled
    if (myPageShiftStatus[PageNr] == false) {
      // Attack
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        myControlChange(midiChannel, CCampattack, (mux1Read >> 5));
        sendCC(midiChannel, CCampattack, (mux1Read >> 5));
      }
      // Decay
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        myControlChange(midiChannel, CCampdecay, (mux2Read >> 5));
        sendCC(midiChannel, CCampdecay, (mux2Read >> 5));
      }
      // Sustain
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
        myControlChange(midiChannel, CCampsustain, (mux3Read >> 5));
        sendCC(midiChannel, CCampsustain, (mux3Read >> 5));
      }
      // Release
      mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
      if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux4ValuesPrev[0] = mux4Read;
        myControlChange(midiChannel, CCamprelease, (mux4Read >> 5));
        sendCC(midiChannel, CCamprelease, (mux4Read >> 5));
      }
    }
    // SUB Menu
    else {
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        int mux1Readtemp = (mux1Read >> 5);
        parameter = myAmpEnvCurve;
        renderCurrentParameter(PageNr, parameter, mux1Readtemp);
      }
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        int mux2Readtemp = (mux2Read >> 5);
        parameter = myAmplifierVelo;
        renderCurrentParameter(PageNr, parameter, mux2Readtemp);
        myControlChange(midiChannel, parameter, mux2Readtemp);
        sendCC(midiChannel, parameter, mux2Readtemp);
      }
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
      }
      mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
      if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux4ValuesPrev[0] = mux4Read;
      }
    }
  }

  // Page:6 (FxDSP) -----------------------------------------------------
  else if (PageNr == 6) {
    // SEL
    mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
    if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux1ValuesPrev[0] = mux1Read;
      int mux1Readtemp = (mux1Read >> 5);
      parameter = myFxSel;
      renderCurrentParameter(PageNr, parameter, mux1Readtemp);
      tftUpdate = true;
      ParUpdate = true;
    }
    // VAL
    mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
    if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux2ValuesPrev[0] = mux2Read;
      int mux2Readtemp = (mux2Read >> 4);
      parameter = myFxVal;
      renderCurrentParameter(PageNr, parameter, mux2Readtemp);
      tftUpdate = true;
      ParUpdate = true;
    }
    // Mix
    mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
    if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux3ValuesPrev[0] = mux3Read;
      int mux3Readtemp = (mux3Read >> 4);
      parameter = myFxMix;
      renderCurrentParameter(PageNr, parameter, mux3Readtemp);
      tftUpdate = true;
      ParUpdate = true;
    }
    // Prg
    mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
    if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux4ValuesPrev[0] = mux4Read;
      int mux4Readtemp = (mux4Read >> 8);
      parameter = myFxPrg;
      renderCurrentParameter(PageNr, parameter, mux4Readtemp);
      tftUpdate = true;
      ParUpdate = true;
    }
    // Clockrate
    mux6Read = MCP_adc.read(MCP3208::Channel::SINGLE_6);
    if (mux6Read > (mux6ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux6Read < (mux6ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux6ValuesPrev[0] = mux6Read;
      int mux6Readtemp = (mux6Read >> 5);
      parameter = myFxClkrate;
      renderCurrentParameter(PageNr, parameter, mux6Readtemp);
      tftUpdate = true;
      ParUpdate = true;
    }
  }

  // Page:7 (LFO 1) -----------------------------------------
  else if (PageNr == 7) {
    // LFO1 main menu
    if (myPageShiftStatus[PageNr] == false) {
      // SHAPE
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        myControlChange(midiChannel, myLFO1shape, mux1Read >> 5);
        sendCC(midiChannel, myLFO1shape, mux1Read >> 5);
      }
      // RATE
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        myControlChange(midiChannel, myLFO1rate, (mux2Read >> 5));
        sendCC(midiChannel, myLFO1rate, (mux2Read >> 5));
      }
      // AMT
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
        myControlChange(midiChannel, myLFO1amt, (mux3Read >> 5));
        sendCC(midiChannel, myLFO1amt, (mux3Read >> 5));
      }
      // Syn
      mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
      if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux4ValuesPrev[0] = mux4Read;
        int mux4Readtemp = (mux4Read >> 4);
        parameter = myLFO1syn;
        renderCurrentParameter(PageNr, parameter, mux4Readtemp);
      }
    }
    // LFO1 sub menu
    else {
      // MODE
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        int mux1Readtemp = (mux1Read >> 4);
        parameter = myLFO1mode;
        renderCurrentParameter(PageNr, parameter, mux1Readtemp);
      }
      // FADE-IN
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        int mux2Readtemp = (mux2Read >> 5);
        parameter = myLFO1fade;
        renderCurrentParameter(PageNr, parameter, mux2Readtemp);
      }
      // FADE-OUT
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
        int mux3Readtemp = (mux3Read >> 5);
        parameter = myLFO1fadeOut;
        renderCurrentParameter(PageNr, parameter, mux3Readtemp);
      }
      // EnvCurve
      mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
      if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux4ValuesPrev[0] = mux4Read;
        int mux4Readtemp = (mux4Read >> 5);
        parameter = myLFO1envCurve;
        renderCurrentParameter(PageNr, parameter, mux4Readtemp);
      }
    }

  }

  // Page:8 (LFO 2) -----------------------------------------
  else if (PageNr == 8) {
    // LFO2 main menu
    if (myPageShiftStatus[PageNr] == false) {
      // SHAPE
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        myControlChange(midiChannel, myLFO2shape, mux1Read >> 5);
        sendCC(midiChannel, myLFO2shape, mux1Read >> 5);
      }
      // RATE
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        myControlChange(midiChannel, myLFO2rate, (mux2Read >> 5));
        sendCC(midiChannel, myLFO2rate, (mux2Read >> 5));
      }
      // AMT
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
        myControlChange(midiChannel, myLFO2amt, (mux3Read >> 5));
        sendCC(midiChannel, myLFO2amt, (mux3Read >> 5));
      }
      // Syn
      mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
      if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux4ValuesPrev[0] = mux4Read;
        int mux4Readtemp = (mux4Read >> 4);
        parameter = myLFO2syn;
        renderCurrentParameter(PageNr, parameter, mux4Readtemp);
      }
    }
    // LFO2 sub menu
    else {
      // MODE
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        int mux1Readtemp = (mux1Read >> 4);
        parameter = myLFO2mode;
        renderCurrentParameter(PageNr, parameter, mux1Readtemp);
      }
      // FADE-IN
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        int mux2Readtemp = (mux2Read >> 5);
        parameter = myLFO2fade;
        renderCurrentParameter(PageNr, parameter, mux2Readtemp);
      }
      // FADE-OUT
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
        int mux3Readtemp = (mux3Read >> 5);
        parameter = myLFO2fadeOut;
        renderCurrentParameter(PageNr, parameter, mux3Readtemp);
      }
      // EnvCurve
      mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
      if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux4ValuesPrev[0] = mux4Read;
        int mux4Readtemp = (mux4Read >> 5);
        parameter = myLFO2envCurve;
        renderCurrentParameter(PageNr, parameter, mux4Readtemp);
      }
    }

  }

  // Page:9 (SEQUENCER) -----------------------------------------
  else if (PageNr == 9) {
    // shift key disabled -----------------------------------
    if (myPageShiftStatus[PageNr] == false) {
      // STEP
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        int mux1Readtemp = (mux1Read >> 4);
        parameter = mySEQStep;
        renderCurrentParameter(PageNr, parameter, mux1Readtemp);
      }
      // PITCH
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        int mux2Readtemp = (mux2Read >> 4);
        parameter = mySEQPitch;
        renderCurrentParameter(PageNr, parameter, mux2Readtemp);
      }
      // BPM
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
        int mux3Readtemp = (mux3Read >> 4);
        parameter = mySEQRate;
        renderCurrentParameter(PageNr, parameter, mux3Readtemp);
      }
      // DIV
      mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
      if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux4ValuesPrev[0] = mux4Read;
        int mux4Readtemp = (mux4Read >> 4);
        parameter = mySEQdiv;
        renderCurrentParameter(PageNr, parameter, mux4Readtemp);
      }
    }

    // Sequencer SUB Page -----------------------------------
    else {
      // LEN
      mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
      if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux1ValuesPrev[0] = mux1Read;
        int mux1Readtemp = (mux1Read >> 4);
        parameter = mySEQLen;
        renderCurrentParameter(PageNr, parameter, mux1Readtemp);
      }
      // TIME
      mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
      if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux2ValuesPrev[0] = mux2Read;
        int mux2Readtemp = (mux2Read >> 4);
        parameter = mySEQGateTime;
        renderCurrentParameter(PageNr, parameter, mux2Readtemp);
      }
      // DIR
      mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
      if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux3ValuesPrev[0] = mux3Read;
        int mux3Readtemp = (mux3Read >> 4);
        parameter = mySEQdir;
        renderCurrentParameter(PageNr, parameter, mux3Readtemp);
      }
      // Seq Mode
      mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
      if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
          mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
        mux4ValuesPrev[0] = mux4Read;
        int mux4Readtemp = (mux4Read >> 4);
        parameter = mySEQmode;
        renderCurrentParameter(PageNr, parameter, mux4Readtemp);
      }
    }
  }

  // Page:10 (MODMATRIX) -----------------------------------------
  else if (PageNr == 10) {
    // read Pot1	(select Parameter)
    mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
    if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux1ValuesPrev[0] = mux1Read;
      int mux1Readtemp = (mux1Read >> 4);
      uint16_t tempParameterNr = (0.0787f * mux1Readtemp);
      if (ParameterNr != tempParameterNr) {
        ParameterNr = tempParameterNr;
        uint8_t ParNo = ParameterNr % 4;
        printModParameter(ParameterNr);
        drawParamterFrame2(PageNr, ParNo);
        readModMatrixParameter(ParameterNr);
        ParameterNrMem[PageNr] = ParameterNr;
        ParmSelectFlag = false;
      }
    }
    // read Pot2	(change Parameter)
    mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
    if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux2ValuesPrev[0] = mux2Read;
      int mux2Readtemp = (mux2Read >> 5);
      // parameter = selecdParameter(PageNr, ParameterNr);
      renderModParameter(ParameterNr, mux2Readtemp);
    }
  }

  // Page:11 (SYSTEM) -----------------------------------------
  else if (PageNr == 11) {
    // read Pot1	(select Parameter)
    mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
    if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux1ValuesPrev[0] = mux1Read;
      int mux1Readtemp = (mux1Read >> 5);
      uint16_t tempParameterNr = (0.0787f * mux1Readtemp); // 10 rows
      if (ParameterNr != tempParameterNr) {
        ParameterNr = tempParameterNr;
        drawParamterFrame(PageNr, ParameterNr);
        ParameterNrMem[PageNr] = ParameterNr;
        ParmSelectFlag = false;
      }
    }
    // read Pot2	(change Parameter)
    mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
    if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux2ValuesPrev[0] = mux2Read;
      int mux2Readtemp = (mux2Read >> 5);
      parameter = selecdParameter(PageNr, ParameterNr);
      renderCurrentParameter(PageNr, parameter, mux2Readtemp);
      myControlChange(midiChannel, parameter, mux2Readtemp);
      sendCC(midiChannel, parameter, mux2Readtemp);
    }
  }

  // Page:99 (Save Patch) -----------------------------------------
  else if (PageNr == 99) {
    // BankNo
    mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
    if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux1ValuesPrev[0] = mux1Read;
      int mux1Readtemp = (mux1Read >> 8);
      parameter = myBankSelect;
      renderCurrentParameter(PageNr, parameter, mux1Readtemp);
    }
    // PatchNo
    mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
    if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux2ValuesPrev[0] = mux2Read;
      int mux2Readtemp = (mux2Read >> 5);
      parameter = myPatchNo;
      renderCurrentParameter(PageNr, parameter, mux2Readtemp);
    }
    // Char
    mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
    if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux3ValuesPrev[0] = mux3Read;
      int mux3Readtemp = (mux3Read >> 5);
      parameter = myChar;
      renderCurrentParameter(PageNr, parameter, mux3Readtemp);
    }
    // Cursor
    mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
    if (mux4Read > (mux4ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux4Read < (mux4ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux4ValuesPrev[0] = mux4Read;
      int mux4Readtemp = (mux4Read >> 5);
      parameter = myCursor;
      renderCurrentParameter(PageNr, parameter, mux4Readtemp);
    }
  }

  // Page:98 (Save Sequencer Pattern) -----------------------------------------
  else if (PageNr == 98) {
    // PatternNo
    mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
    if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux1ValuesPrev[0] = mux1Read;
      int mux1Readtemp = (mux1Read >> 6);
      parameter = myPatternNo;
      renderCurrentParameter(PageNr, parameter, mux1Readtemp);
      tftUpdate = true;
      ParUpdate = true;
    }
    // Char
    mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
    if (mux2Read > (mux2ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux2Read < (mux2ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux2ValuesPrev[0] = mux2Read;
      int mux2Readtemp = (mux2Read >> 5);
      parameter = myChar;
      renderCurrentParameter(PageNr, parameter, mux2Readtemp);
      tftUpdate = true;
      ParUpdate = true;
    }
    // Cursor
    mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
    if (mux3Read > (mux3ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux3Read < (mux3ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux3ValuesPrev[0] = mux3Read;
      int mux3Readtemp = (mux3Read >> 5);
      parameter = myCursor;
      renderCurrentParameter(PageNr, parameter, mux3Readtemp);
      tftUpdate = true;
      ParUpdate = true;
    }
  }

  // Page:97 (Load Sequencer Pattern) -----------------------------------
  else if (PageNr == 97) {
    // PatternNo
    mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
    if (mux1Read > (mux1ValuesPrev[0] + QUANTISE_FACTOR) ||
        mux1Read < (mux1ValuesPrev[0] - QUANTISE_FACTOR)) {
      mux1ValuesPrev[0] = mux1Read;
      int mux1Readtemp = (mux1Read >> 6);
      parameter = myPatternNo;
      renderCurrentParameter(PageNr, parameter, mux1Readtemp);
      tftUpdate = true;
      ParUpdate = true;
    }
  }

  // Fx control via midi -----------------------------------------------
  if (FxPrgNoChange == true) {
    setFxPrg(FxPrgNo);
    FxPrgNoChange = false;
    if (FxPrgNo == 0) {
      setLED(1, false);
    } else
      setLED(1, true);
  }
}

//*************************************************************************
// check Switches
//*************************************************************************
FLASHMEM void checkSwitches(void) {
  uint8_t parameter = 0;
  static int Debounce = 5;
  static int waitnext = 70; // 2.menu page on load/save key
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
        // tft.fillScreen(ST7735_BLACK);
        RefreshMainScreenFlag = true;
        clearScreenFlag = true;
        VoicLEDtime = 1;
        renderCurrentPatchPage();
      }
      // Parameter Pages
      else {
        PAGE_SW_Status = false;
        PageNr = LastPageNr;
        if (PageNr < 1) {
          PageNr = 1;
        }
        VoicLEDtime = 3;
        renderCurrentPatchPage();
      }
    }
  }

  // key query ----------------------------------------------------------
  mux5Read = MCP_adc.read(MCP3208::Channel::SINGLE_7);
  value = mux5Read >> 5;

  // Key S2 "UNISONO" ----------------------------------------
  if (value < (S2 + hysteresis) && value > (S2 - hysteresis)) {
    KeyDebounce++;
    if (KeyDebounce == Debounce && KeyStatus == 0) {
      KeyStatus = 1;
      parameter = CCunison;
      value = 0;
      if (unison == 0) {
        unison = 1;
      } else if (unison == 1) {
        unison = 2;
        printUnisonDetune();
      } else {
        unison = 0;
        printUnisonDetune();
      }
      myControlChange(midiChannel, parameter, unison);
      sendCC(midiChannel, parameter, unison);
    }
  }

  // Key S3 "SEQ" --------------------------------------------
  else if (value < (S3 + hysteresis) && value > (S3 - hysteresis)) {
    KeyDebounce++;
    if (KeyDebounce == Debounce && KeyStatus == 0 && Keylock == false) {
      KeyStatus = 1;
      parameter = myARPSEQ;
      if (SEQrunStatus == 0 && SeqNotesAvailable == true) { // run sequencer
        SEQselectStepNo = 0;
        SeqTranspose = 0;
        Interval = SEQclkRate;
        gateTime =
            (float)(SEQclkRate / SEQGateTime); // set Interval and gateTime
        timer_intMidiClk = micros();
        timer_intMidiClk -= SEQclkRate;
        SEQdirectionFlag = false;
        SEQrunStatus = true;
      } else { // stop Sequencer
        SEQrunStatus = false;
        if (SEQselectStepNo > 0) {
          SEQselectStepNo--;
        }
        myMIDIClockStop();
      }
    }
  }

  // Key S4 "MUTE/PANIC" -------------------------------
  else if (value < (S4 + hysteresis) && value > (S4 - hysteresis)) {
    KeyDebounce++;
    if (KeyDebounce == Debounce && KeyStatus == 0 && Keylock == false) {
      KeyStatus = 1;
      if (PageNr == 9 && SEQrunStatus == false) {
        if (SeqNoteBufStatus[SEQselectStepNo] == 1) {
          SeqNoteBufStatus[SEQselectStepNo] = 0; // mute note
          drawSEQpitchValue2(SEQselectStepNo);
          if (SEQmode == 2) {
            SEQselectStepNo++;
            if (SEQselectStepNo > SEQstepNumbers) {
              SEQselectStepNo = 0;
            }
            drawSEQStepFrame(SEQselectStepNo);
          }
        } else {
          SeqNoteBufStatus[SEQselectStepNo] = 1; // note on
          drawSEQpitchValue2(SEQselectStepNo);
          SEQselectStepNo++;
          if (SEQselectStepNo > SEQstepNumbers) {
            SEQselectStepNo = 0;
          }
          drawSEQStepFrame(SEQselectStepNo);
        }
      }
    }
    if (KeyDebounce == waitnext && KeyStatus == 1 && PageNr == 9 &&
        SEQrunStatus == false) { // init Sequencer
      allNotesOff();
      initPatternData();
      SEQselectStepNo = 0;
      SeqNotesAvailable = false;
      renderCurrentPatchPage();
    } else if (KeyDebounce == waitnext && KeyStatus == 1 && PageNr != 9 &&
               SEQrunStatus == false) { // Panic all notes off
      allNotesOff();
    }
  }

  // Key S5 "BOOST" -------------------------------------
  else if (value < (S5 + hysteresis) && value > (S5 - hysteresis)) {

    KeyDebounce++;
    if (KeyDebounce == Debounce && KeyStatus == 0) {
      KeyStatus = 1;
      if (BassBoostStatus == 0) {
        BassBoostStatus = 1;
        myBoost = 1;
        setLED(4, true);
        digitalWrite(BassBoost, LOW); // Boost on
        Serial.print("Bass ON: ");
        Serial.println(value);
      } else {
        BassBoostStatus = 0;
        myBoost = 0;
        setLED(4, false);
        digitalWrite(BassBoost, HIGH); // Boost off
        Serial.print("Bas OFF: ");
        Serial.println(value);
      }
    }
  }

  // Key S6 "SHIFT" -----------------------------------------------------
  else if (value < (S6 + hysteresis) && value > (S6 - hysteresis)) {
    KeyDebounce++;
    if (KeyDebounce == Debounce && KeyStatus == 0 && PageNr > 0) {
      KeyStatus = 1;
      if (Keylock == true) {
        Keylock = false;
        S7KeyStatus = 0;
        PageNr = PageNr_old;
        if (PageNr == 0) {
          clearScreenFlag = true;
          RefreshMainScreenFlag = true;
        }
        renderCurrentPatchPage(); // draw old screen
      } else {
        if (myPageShiftStatus[PageNr] == false) {
          myPageShiftStatus[PageNr] = true;
        } else {
          myPageShiftStatus[PageNr] = false;
        }
        renderCurrentPatchPage();
      }
    }
    if (KeyDebounce == Debounce && KeyStatus == 0 && PageNr == 0) {
      PrgSelShift = true;
    }
  }

  // Key S7 "LOAD/SAVE" ----------------------------------------
  else if (value < (S7 + hysteresis)) {
    KeyDebounce++;
    if (KeyDebounce == Debounce && KeyStatus == 0) {
      KeyStatus = 1;
      // if (PageNr > 0 && S7KeyStatus == 0) {
      if (S7KeyStatus == 0) {
        S7KeyStatus = 1;
        PageNr_old = PageNr;
        if (PageNr != 9 && PageNr != 11) { // Sequencer page
          PageNr = 99;                     // save Patch data
          newPatchNo = patchNo;
          Keylock = true;
          SEQrunStatus = false;
          allNotesOff();
          closeEnvelopes();
          renderCurrentPatchPage(); // draw "Save" screen
        } else if (PageNr == 11) {
          Keylock = true;
          // Serial.println("send SysEx!");
          sendSysExFlag = true;
        }

        else {
          PageNr = 97;
          Keylock = true;
          SEQrunStatus = false;
          allNotesOff();
          closeEnvelopes();
          SEQselectStepNo = 0;
          renderCurrentPatchPage(); // draw "Load Pattern" screen
        }
      } else if (S7KeyStatus == 1) {
        if (PageNr == 99) { // save Patch Data
          patchName = oldPatchName;
          currentPatchName = oldPatchName;
          patchNo = newPatchNo;
          currentPatchBank = newPatchBank;
          Serial.print("Bank: ");
          Serial.println((char(currentPatchBank + 65)));
          String numString = (patchNo);
          String bankString = char(currentPatchBank + 65);
          String fileString = (bankString + "/" + numString);
          savePatch(String(fileString).c_str(), getCurrentPatchData());
          storeSoundPatchNo(patchNo);
          storePatchBankNo(currentPatchBank);
          delay(50);
          recallPatch(patchNo);
          Serial.println("save Patch!");
          Serial.println(patchNo);
          Serial.println((char(currentPatchBank + 65)));
          Keylock = false;
        } else if (PageNr == 98) { // save Sequencer Pattern
          patternName = newPatternName;
          String numString = (SEQPatternNo);
          String folderString = "SEQ"; // Folder name
          String fileString = (folderString + "/" + numString);
          savePattern(String(fileString).c_str(), getCurrentPatternData());
          Keylock = false;
        } else if (PageNr == 97) { // load Sequencer Pattern
          Keylock = false;
          recallPattern(SEQPatternNo);
          SeqTranspose = 0;
          SeqNotesAvailable = true;
          SEQrunStatus = true;     // start Sequencer
        } else if (PageNr == 96) { // init Patch
          Keylock = false;
          set_initPatchData();
          initPatternData();
        }
        S7KeyStatus = 0;
        Keylock = false;
        PageNr = PageNr_old;
        if (PageNr == 0) {
          clearScreenFlag = true;
          RefreshMainScreenFlag = true;
        }
        renderCurrentPatchPage(); // return last menu page
      }
    }

    // long press LOAD/SAVE Key ---------------------------------
    if (KeyDebounce == waitnext && KeyStatus == 1) {
      if (PageNr == 97) {
        PageNr = 98; // draw "Save Pattern" Menu
        renderCurrentPatchPage();
      } else if (PageNr == 99) {
        PageNr = 96;
        renderCurrentPatchPage(); // draw "init Patch" Menu
      }
    }
  }

  // Debounce ----------------------------------------------------
  else {
    KeyDebounce = 0;
    KeyStatus = 0;
    PrgSelShift = false;
  }
}

//*************************************************************************
// set LEDs into 74HC595 Register
//*************************************************************************
FLASHMEM void setLED(uint8_t Pin, boolean Status) {
  uint8_t data = 0;

  if (Pin == 1) {
    data = 0b00000001;
    if (Status == true) {
      HC595Register |= data;
    } else {
      HC595Register &= ~(data);
    }

  } else if (Pin == 2) {
    data = 0b00000010;
    if (Status == true) {
      HC595Register |= data;
    } else {
      HC595Register &= ~(data);
    }
  } else if (Pin == 3) {
    data = 0b00000100;
    if (Status == true) {
      HC595Register |= data;
    } else {
      HC595Register &= ~(data);
    }
  } else if (Pin == 4) {
    data = 0b00001000;
    if (Status == true) {
      HC595Register |= data;
    } else {
      HC595Register &= ~(data);
    }
  }
  SPI.transfer(HC595Register);
  digitalWrite(CS_HC595, LOW);
  digitalWrite(CS_HC595, HIGH);
  SPI.endTransaction();
}

//*************************************************************************
// set FxPrg into 74HC595 Register
//*************************************************************************
FLASHMEM void setFxPrg(uint8_t PrgNo) {
  int myPrg[16] = {5, 0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  uint8_t data = myPrg[PrgNo];
  data = data << 4;
  if (PrgNo >= 8) {
    HC595Register &= 0b10001111;
  } else
    HC595Register &= 0b00001111;
  HC595Register |= data;
  SPI.transfer(HC595Register);
  digitalWrite(CS_HC595, LOW);
  digitalWrite(CS_HC595, HIGH);
  SPI.endTransaction();
}

//*************************************************************************
// init PWM for Fx
//*************************************************************************
FLASHMEM void initPWMFx(void) {
  analogWriteFrequency(PWM1, 100000);
  analogWriteFrequency(PWM2, 100000);
  analogWriteFrequency(PWM3, 100000);
  analogWriteFrequency(PWM4, 100000);
  analogWriteFrequency(PWM5, 60000); // Fx clock
  analogWrite(PWM1, 0);
  analogWrite(PWM2, 0);
  analogWrite(PWM3, 0);
  analogWrite(PWM4, 0);
  analogWrite(PWM5, 127);
}

//*************************************************************************
// init 74HC595 LED Driver
//*************************************************************************
FLASHMEM void initHC595(void) {
  HC595Register = 0;
  SPI.transfer(HC595Register);
  digitalWrite(CS_HC595, LOW);
  digitalWrite(CS_HC595, HIGH);
  SPI.endTransaction();
}

//*************************************************************************
// init Potentiometer
//*************************************************************************
FLASHMEM void initPotentiometers(void) {
  // init Pots for first time
  if (initStatus == 1) {
    mux1Read = MCP_adc.read(MCP3208::Channel::SINGLE_0);
    mux2Read = MCP_adc.read(MCP3208::Channel::SINGLE_1);
    mux3Read = MCP_adc.read(MCP3208::Channel::SINGLE_2);
    mux4Read = MCP_adc.read(MCP3208::Channel::SINGLE_3);
    mux1ValuesPrev[0] = mux1Read;
    mux2ValuesPrev[0] = mux2Read;
    mux3ValuesPrev[0] = mux3Read;
    mux4ValuesPrev[0] = mux4Read;
    initStatus = 0;
  }
}

//*************************************************************************
// Encoder query
//*************************************************************************
FLASHMEM void checkEncoder(void) {
  // if no encoder data or Load/Save function active than return
  if (encoderflag == false || Keylock == true) {
    return;
  }

  int encRead = encValue;
  encoderflag = false;

  // selected Parameter Pages -----------------------------------------
  if (PageNr > 0) {
    // long encRead = encoder.read();
    uint8_t oldPageNr = PageNr;
    if ((encCW && encRead > encPrevious) || (!encCW && encRead < encPrevious)) {
      PageNr++;
      if (PageNr > Pages) {
        PageNr = Pages;
      } else if (PageNr < 1) {
        PageNr = 1;
      }
      if (oldPageNr != PageNr) {
        renderCurrentPatchPage();
      }
      encPrevious = encRead;
    }

    else if ((encCW && encRead < encPrevious) ||
             (!encCW && encRead > encPrevious)) {
      PageNr--;
      if (PageNr > Pages) {
        PageNr = Pages;
      } else if (PageNr < 1) {
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
    // long encRead = encoder.read();
    if ((encCW && encRead > encPrevious) || (!encCW && encRead < encPrevious)) {
      if (PrgSelShift == false) {
        patchNo++;
      } else
        patchNo += 10;
      if (patchNo > 128) {
        if (currentPatchBank < 15) { // Bank A-P
          currentPatchBank++;
          patchNo = patchNo - 128;
        } else {
          patchNo = 128;
          currentPatchBank = 15;
        }
      }
      if (oldpatchNo != patchNo) {
        recallPatch(patchNo);
        storeSoundPatchNo(patchNo);
        storePatchBankNo(currentPatchBank);
        newPatchNo = patchNo;
        newPatchBank = currentPatchBank;
        RefreshMainScreenFlag = true;
        // set voices LED off
        for (uint8_t i = 0; i < 8; i++) {
          VoicesLEDtime[i] = 0;
        }
      }
      encPrevious = encRead;
    } else if ((encCW && encRead < encPrevious) ||
               (!encCW && encRead > encPrevious)) {
      if (PrgSelShift == false) {
        patchNo--;
      } else
        patchNo -= 10;
      if (patchNo < 1 || patchNo > 128) {
        if (currentPatchBank > 0 && currentPatchBank <= 15) {
          patchNo = patchNo - 128;
          currentPatchBank--;
        } else {
          patchNo = 1;
          currentPatchBank = 0;
        }
      }
      if (oldpatchNo != patchNo) {
        recallPatch(patchNo);
        storeSoundPatchNo(patchNo);
        storePatchBankNo(currentPatchBank);
        RefreshMainScreenFlag = true;
        // set voices LED off
        for (uint8_t i = 0; i < 8; i++) {
          VoicesLEDtime[i] = 0;
        }
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
    // midi1.sendControlChange(cc, value, midiOutCh);
  }
}

//*************************************************************************
// init PatchNo: 1
//*************************************************************************
FLASHMEM void Init_Patch(void) {
  if (Init_flag == true) {
    Init_flag = false;
    patchNo = getSoundPachNo();
    newPatchNo = patchNo;
    currentPatchBank = getPatchBankNo();
    newPatchBank = currentPatchBank;
    recallPatch(patchNo);
  }
}
//*************************************************************************
// print Unisono Detune1 (Osc2 page)
//*************************************************************************
FLASHMEM void printUnisonDetune(void) {
  if (PageNr == 2) {
    if (unison < 2) {
      uint8_t detune_val = 0;
      tft.fillRoundRect(50, 76, 26, 10, 2, ST7735_BLACK);
      tft.fillRoundRect(54, 76, 22, 10, 2, ST7735_BLUE);
      tft.setCursor(56, 78);
      tft.setTextColor(ST7735_WHITE);
      for (uint8_t i = 0; i < 128; i++) {
        float myDetune = DETUNEValue[i];
        if ((myDetune - detune) < 0.000001) { // compare float!
          detune_val = i;
          break;
        }
      }
      tft.print(detune_val);
    } else {
      tft.fillRoundRect(50, 76, 26, 10, 2, ST7735_BLUE);
      tft.setCursor(52, 78);
      uint8_t detune_val = 0;
      for (uint8_t i = 0; i < 128; i++) {
        float myDetune = DETUNEValue[i];
        if (myDetune == detune) { // compare float!
          detune_val = i;
          break;
        }
      }
      tft.print(CDT_STR[detune_val]);
    }
  }
}

//*************************************************************************
// CPU Monitor
//*************************************************************************
FLASHMEM void printCPUmon(void) {
  tft.fillRoundRect(54, 57, 22, 10, 2, ST7735_BLUE);
  tft.setCursor(56, 59);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print(CPUaudioMem);
  tft.setCursor(69, 59);
  tft.print("%");
}

//*************************************************************************
// print freeMem
//*************************************************************************

FLASHMEM void startup_late_hook(void) {
  extern unsigned long _ebss;
  unsigned long *p = &_ebss;
  size_t size =
      (size_t)(uint8_t *)__builtin_frame_address(0) - 16 - (uintptr_t)&_ebss;
  memset((void *)p, 0, size);
}

FLASHMEM unsigned long maxstack(void) {
  extern unsigned long _ebss;
  extern unsigned long _estack;
  unsigned long *p = &_ebss;
  while (*p == 0)
    p++;
  return (unsigned)&_estack - (unsigned)p;
}

FLASHMEM void flexRamInfo(void) {

#if defined(__IMXRT1062__)
  const unsigned DTCM_START = 0x20000000UL;
  const unsigned OCRAM_START = 0x20200000UL;
  const unsigned OCRAM_SIZE = 512;
  const unsigned FLASH_SIZE = 8192;
#endif

  int itcm = 0;
  int dtcm = 0;
  int ocram = 0;
  uint32_t gpr17 = IOMUXC_GPR_GPR17;

  char __attribute__((unused)) dispstr[17] = {0};
  dispstr[16] = 0;

  for (int i = 15; i >= 0; i--) {
    switch ((gpr17 >> (i * 2)) & 0b11) {
    default:
      dispstr[15 - i] = '.';
      break;
    case 0b01:
      dispstr[15 - i] = 'O';
      ocram++;
      break;
    case 0b10:
      dispstr[15 - i] = 'D';
      dtcm++;
      break;
    case 0b11:
      dispstr[15 - i] = 'I';
      itcm++;
      break;
    }
  }

  const char *fmtstr = "%-6s%7d %5.02f%% of %4dkB (%7d Bytes free) %s\n";
  extern unsigned long _stext;
  extern unsigned long _etext;
  extern unsigned long _sdata;
  extern unsigned long _ebss;
  extern unsigned long _flashimagelen;
  extern unsigned long _heap_start;
  extern unsigned long _estack;

  Serial.printf(fmtstr, "ITCM:", (unsigned)&_etext - (unsigned)&_stext,
                (float)((unsigned)&_etext - (unsigned)&_stext) /
                    ((float)itcm * 32768.0f) * 100.0f,
                itcm * 32,
                itcm * 32768 - ((unsigned)&_etext - (unsigned)&_stext),
                "(RAM1) FASTRUN");

  Serial.printf(fmtstr, "OCRAM:", (unsigned)&_heap_start - OCRAM_START,
                (float)((unsigned)&_heap_start - OCRAM_START) /
                    (OCRAM_SIZE * 1024.0f) * 100.0f,
                OCRAM_SIZE,
                OCRAM_SIZE * 1024 - ((unsigned)&_heap_start - OCRAM_START),
                "(RAM2) DMAMEM, Heap");

  Serial.printf(fmtstr, "FLASH:", (unsigned)&_flashimagelen,
                ((unsigned)&_flashimagelen) / (FLASH_SIZE * 1024.0f) * 100.0f,
                FLASH_SIZE, FLASH_SIZE * 1024 - ((unsigned)&_flashimagelen),
                "FLASHMEM, PROGMEM");
}

//*************************************************************************
// print CPU temperatur
//*************************************************************************
extern float tempmonGetTemp(void);

FLASHMEM void printTemperature(void) {
  if (PageNr == 11) {
    tft.fillRoundRect(54, 38, 22, 10, 2, ST7735_BLUE);
    tft.setCursor(55, 40);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.print(CPUdegree);
    tft.drawPixel(68, 40, ST7735_WHITE);
    print_String(161, 70, 40); // print "C"
  }
}

//*************************************************************************
// print SD card error
//*************************************************************************
FLASHMEM void printError(uint8_t index) {
  enableScope(false);
  tft.fillScreen(ST7735_BLACK);
  tft.fillRect(20, 20, 120, 88, ST7735_RED);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(0);
  print_String(159, 30, 50); // print "SD card error !"
  print_String(160, 30, 70); // "Press power switch."
  tftUpdate = true;
}

//***********************************************************************
// Midi Clock Start
//***********************************************************************
FLASHMEM void myMIDIClockStart() {

  if (MidiSyncSwitch == true) {
    if (SEQMidiClkSwitch == true) {
      MidiClkTiming_Flag = true;
      SEQselectStepNo = 0;
      ARPSEQstatus = 0;
      SEQrunStatus = true;
      MidiCLKcount = 0;
    } else {
      SEQselectStepNo = 0;
      PlayFlag = true;
    }
  }
}

//**********************************************************************
// Midi Clock Stop
//**********************************************************************
FLASHMEM void myMIDIClockStop() {

  if (MidiSyncSwitch == true) {
    SEQrunStatus = false;
    ARPSEQstatus = false;
    SeqTranspose = 0;
    TempoLEDstate = false; // set Tempo LED off
    TempoLEDchange = true;
    PlayFlag = false;
    MidiClkTiming_Flag = false;
    MidiCLKcount = 0;
    SEQselectStepNo = 0;
    allNotesOff();
  }
}

//**********************************************************************
// receive Midi Clock data
//**********************************************************************
FLASHMEM void myMIDIClock(void) {

  if (MidiSyncSwitch == true) {
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
    // start Sequencer with MIDI Start message and disabled Midi Clock Switch
    else if (SEQMidiClkSwitch == false && PlayFlag == true) {
      // SEQselectStepNo = 0;
      SeqTranspose = 0;
      Interval = SEQclkRate;
      gateTime = (float)(SEQclkRate / SEQGateTime); // set Interval and gateTime
      timer_intMidiClk = micros();
      timer_intMidiClk -= SEQclkRate;
      SEQdirectionFlag = false;
      SEQrunStatus = true;
      PlayFlag = false;
    }
  }
}

//*************************************************************************
// Play sequencer notes from extern midi clock
//*************************************************************************
FLASHMEM void Sequencer2(boolean SEQNoteState) {

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
          myNoteOn2(1, myNote, velo);
        }
        if (SEQselectStepNo == 0 || SEQselectStepNo == 4 ||
            SEQselectStepNo == 8 || SEQselectStepNo == 12) {
          TempoLEDstate = true; // set Tempo LED on
          TempoLEDchange = true;
        }
      }
    }
  } else {
    if (ARPSEQstatus == true && SEQrunStatus == true) {
      ARPSEQstatus = false;
      if (SEQselectStepNo == 1 || SEQselectStepNo == 5 ||
          SEQselectStepNo == 9 || SEQselectStepNo == 13) {
        TempoLEDstate = false; // set Tempo LED off
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
        if (SEQdirection == 0) { // ">"
          SEQselectStepNo++;
          if (SEQselectStepNo > SEQstepNumbers) {
            SEQselectStepNo = 0;
          }
        } else if (SEQdirection == 1) { // "<"
          SEQselectStepNo--;
          if (SEQselectStepNo < 0) {
            SEQselectStepNo = SEQstepNumbers;
          }
        } else if (SEQdirection == 2 && SEQstepNumbers > 0) { // "<>"
          if (SEQdirectionFlag == false) {
            SEQselectStepNo++;
            if (SEQselectStepNo > (SEQstepNumbers - 1)) {
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
        } else if (SEQdirection == 2 && SEQstepNumbers == 0) { // "<>"
          SEQselectStepNo = 0;
          SEQdirectionFlag = false;
        } else if (SEQdirection == 3) { // "RND"
          SEQselectStepNo = Entropy.random(0, SEQstepNumbers + 1);
        }
      }
    }
  }
}

//*************************************************************************
// Play sequencer notes from intern clock
//*************************************************************************
FLASHMEM void Sequencer(void) {

  // Interval and gateTime is calc with Seq.Parameters BPM/DIV/TIME

  if (SEQMidiClkSwitch == false) {

    // Play Sequencer Note
    if ((micros() - timer_intMidiClk) > Interval) {

      timer_intMidiClk += Interval;

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
            myNoteOn2(1, myNote, velo);
            EnvIdelFlag = true; // oscilloscope enabled
          }
          if (SEQselectStepNo == 0 || SEQselectStepNo == 4 ||
              SEQselectStepNo == 8 || SEQselectStepNo == 12) {
            TempoLEDstate = true; // set Tempo LED on
            TempoLEDchange = true;
          }
        }
      }
    }
    // If gateTime is over then turn off Sequencer Note
    if ((micros() - timer_intMidiClk) > gateTime) {
      if (ARPSEQstatus == true && SEQrunStatus == true) {
        ARPSEQstatus = false;
        if (SEQselectStepNo == 1 || SEQselectStepNo == 5 ||
            SEQselectStepNo == 9 || SEQselectStepNo == 13) {
          TempoLEDstate = false; // set Tempo LED off
          TempoLEDchange = true;
        }
        if (SeqNoteBufStatus[SEQselectStepNo] == 1) {
          uint8_t noteCount = SeqNoteCount[SEQselectStepNo];
          for (uint8_t i = 0; i < noteCount; i++) {
            myNoteOff(midiChannel, SeqNote1Buf[(SEQselectStepNo + (i * 16))],
                      0);
          }
        }
        // play Sequencer note (in Play Mode)
        if (SEQrunOneNote == false) {
          // Direction ----------------------------------------------
          if (SEQdirection == 0) { // ">"
            SEQselectStepNo++;
            if (SEQselectStepNo > SEQstepNumbers) {
              SEQselectStepNo = 0;
            }
          } else if (SEQdirection == 1) { // "<"
            SEQselectStepNo--;
            if (SEQselectStepNo < 0) {
              SEQselectStepNo = SEQstepNumbers;
            }
          } else if (SEQdirection == 2 && SEQstepNumbers > 0) { // "<>"
            if (SEQdirectionFlag == false) {
              SEQselectStepNo++;
              if (SEQselectStepNo > (SEQstepNumbers - 1)) {
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
          } else if (SEQdirection == 2 && SEQstepNumbers == 0) { // "<>"
            SEQselectStepNo = 0;
            SEQdirectionFlag = false;
          } else if (SEQdirection == 3) { // "RND"
            SEQselectStepNo = Entropy.random(0, SEQstepNumbers + 1);
          }
          // play Step note (in Edit Mode) --------------------------
        } else {
          SEQrunStatus = false;
          SEQrunOneNote = false;
        }
      }
      // Turn off all active sequencer notes when sequencer is turned off
      else if (ARPSEQstatus == true && SEQrunStatus == false) {
        for (uint8_t i = 0; i < 16; i++) {
          if (SeqNoteBufStatus[i] == true) {
            myNoteOff(midiChannel, SeqNote1Buf[i], 0);
          }
        }
        allNotesOff();
        ARPSEQstatus = false;
        SeqTranspose = 0;
        TempoLEDstate = false; // set Tempo LED off
        TempoLEDchange = true;
      }
    }
  }
}

//*************************************************************************
// Recording notes into sequencer
//*************************************************************************
FLASHMEM void SequencerRecNotes(uint8_t note, uint8_t velo) {
  if (PageNr == 9 && SEQmode == 2 && SEQrunStatus == false &&
      SeqRecNoteCount <= 4) {

    // if all notes Off then next step ----------------------------
    if (SeqRecNoteCount == 0) {
      SEQselectStepNo++;
      if (SEQselectStepNo > SEQstepNumbers) {
        SEQselectStepNo = 0;
      }
      drawSEQStepFrame(SEQselectStepNo); // draw red frame
      return;
    }

    // save notes to seq buffer -----------------------------------
    SeqNote1Buf[(SEQselectStepNo + ((SeqRecNoteCount - 1) * 16))] =
        note; // Seq Note buffer

    // Sequencer notes available
    SeqNotesAvailable = true;

    // if 1.Note than save velo and status
    if (SeqRecNoteCount == 1) {
      SeqNoteCount[SEQselectStepNo] = 1;  // NoteOn Status
      SeqVeloBuf[SEQselectStepNo] = velo; // Velo from 1.Note
      SeqNoteBufStatus[SEQselectStepNo] = true;
    }
    // next notes save note count and draw notes
    SeqNoteCount[SEQselectStepNo] = SeqRecNoteCount;
    drawSEQpitchValue2(SEQselectStepNo);
  }
}

//*************************************************************************
// usbMidi receive SystemExclusive
//*************************************************************************
FLASHMEM void mySystemExclusiveChunk(byte *data, unsigned int length) {
  Serial.println("SysEx data: ");

  for (int i = 0; i < length; i++) {
    int value = data[i];
    Serial.println(value);
  }
}

//*************************************************************************
// convert parameter float to string
//*************************************************************************
FLASHMEM int float_to_string(String value, uint8_t len, int sysexCount,
                             byte *sysexData) {
  for (uint8_t i = 0; i < len; i++) {
    sysexData[sysexCount] = value[i];
    sysexCount++;
  }
  return sysexCount;
}

//*************************************************************************
// convert parameter float into uint7Bit  (float 0 - 0.127)
//*************************************************************************
FLASHMEM int float_to_sysex1Byte(String value, int sysexCount,
                                 byte *sysexData) {
  sysexData[sysexCount++] = int(value.toFloat() * 100);

  return sysexCount;
}

//*************************************************************************
// convert parameter float into uint7Bit  (- 127 - +127)
//*************************************************************************
FLASHMEM int int8_to_sysex2Bytes(String value, int sysexCount,
                                 byte *sysexData) {
  uint8_t l_byte = (value.toInt());
  uint8_t h_byte = 0;

  if (l_byte >= 128) {
    l_byte &= 0x7F;
    h_byte = 1;
  }

  sysexData[sysexCount++] = h_byte; // Sign (- = 1 / + = 0)
  sysexData[sysexCount++] = l_byte; // data

  return sysexCount;
}

//*************************************************************************
// convert parameter string into int7 (float -1.27 - +1.27)
//*************************************************************************
FLASHMEM int float_to_sysex2Bytes(String value, int sysexCount,
                                  byte *sysexData) {
  uint8_t h_byte = int8_t(value.toFloat() * 100);
  int8_t l_byte = h_byte;

  // calc h_byte
  h_byte >>= 7;

  // l_byte is sign
  if (l_byte < 0) {
    l_byte = l_byte * (-1);
  }

  sysexData[sysexCount++] = h_byte; // Sign (- = 1 / + = 0)
  sysexData[sysexCount++] = l_byte; // data

  return sysexCount;
}

//*************************************************************************
// convert parameter flout into uint14 (float 0.00 - 16.383)
//*************************************************************************
FLASHMEM int float_to_uint14bit(String value, int sysexCount, byte *sysexData) {
  uint8_t h_byte = int8_t(value.toFloat() * 100);
  int8_t l_byte = h_byte;

  // calc h_byte
  h_byte >>= 7;

  // l_byte is sign
  if (l_byte < 0) {
    l_byte = l_byte * (-1);
  }

  sysexData[sysexCount++] = h_byte; // Sign (- = 1 / + = 0)
  sysexData[sysexCount++] = l_byte; // data

  return sysexCount;
}

//*************************************************************************
// convert parameter string into 1 Byte ( 7Bit Number from 0-127)
//*************************************************************************
FLASHMEM int uint8_to_sysex1Byte(String value, int sysexCount,
                                 byte *sysexData) {
  sysexData[sysexCount++] = value.toInt();

  return sysexCount;
}

//*************************************************************************
// convert parameter uint16 into 2 Byte (2x7Bit Number from 0-16383)
//*************************************************************************
FLASHMEM int uint14_to_sysex2Bytes(String value, int sysexCount,
                                   byte *sysexData) {
  uint16_t var16 = (value.toInt());
  uint16_t var16_x = var16;

  // calc h_byte & l_byte -----------
  var16 = var16 << 1;
  uint8_t h_byte = var16 >> 8;
  uint8_t l_byte = var16_x & 0x7F;

  sysexData[sysexCount++] = h_byte;
  sysexData[sysexCount++] = l_byte;

  return sysexCount;
}

//*************************************************************************
// convert 32bit float into 5 Byte for sysex
//*************************************************************************
FLASHMEM int float_to_sysex5Bytes(String value, int sysexCount,
                                  byte *sysexData) {
  uint8_t sysexBytes[5];
  uint8_t temp_val = 0;
  uint8_t bit_val = 0;

  union {
    float fval;
    byte bval[4];
  } floatAsBytes;

  floatAsBytes.fval = (value.toFloat());

  //  Bit mask
  for (uint8_t i = 0; i < 4; i++) {

    // Bit 0-6 to Byte 1-4
    sysexBytes[i] = floatAsBytes.bval[i] & 0x7F;

    // Bit7 to 5.Byte
    temp_val = floatAsBytes.bval[i];
    bit_val = bit_val << 1;
    if (temp_val >= 128) {
      bit_val = (bit_val | 0x01);
    }
  }
  sysexBytes[4] = bit_val;

  // write sysex buffer
  for (uint8_t i = 0; i < 5; i++) {
    sysexData[sysexCount++] = sysexBytes[i];
  }

  return sysexCount;
}

//*************************************************************************
// usbMidi send SystemExclusive
//*************************************************************************
FLASHMEM void mySendSysEx(void) {
  /*
  byte sysexData[256];	// SysEx buffer 256 Byte

  uint8_t patchNo = 32;			// send PrgNr "B 032"
  uint8_t currentPatchBank = 1;
  String numString = (patchNo);
  String bankString = char(currentPatchBank + 65);
  String fileString = (bankString + "/" + numString);
  uint8_t data_len = NO_OF_PARAMS;
  int sysexCount = 0;

  // get Sound File String
  File patchFile = SD.open(fileString.c_str());
  String data[data_len]; //Array of data read in
  recallPatchData(patchFile, data);
  patchFile.close();

  // Sysex data lenght [xxx];
  // byte lenght sysexData[sysexCount++] = 0xF0;
  // 1	- Start SysEx
  sysexData[sysexCount++] = 0x00;
  // 1	- ID
  sysexData[sysexCount++] = 0x00;
  // 1	- ID
  sysexData[sysexCount++] = 0x00;
  // 1	- ID
  sysexData[sysexCount++] = 0x00;
  // 1	- Device ID 0-64 sysexData[sysexCount++] = patchNo;
  // 1	- Folder No sysexData[sysexCount++] = currentPatchBank;
  // 1	- Patch No sysexCount = float_to_string(data[0], 12, sysexCount,
  &sysexData[0]);	// 12	- Patch Name sysexCount =
  float_to_sysex1Byte(data[1], sysexCount, &sysexData[0]);	// 1	-
  oscALevel (0 - 1.00) sysexCount = float_to_sysex1Byte(data[2], sysexCount,
  &sysexData[0]);	// 1	- oscBLevel (0 - 1.00) sysexCount =
  float_to_sysex2Bytes(data[3], sysexCount, &sysexData[0]);	// 2	-
  noiseLevel ( -1.00 - +1.00) sysexCount = uint8_to_sysex1Byte(data[4],
  sysexCount, &sysexData[0]);	// 1	- unison (0 - 2) sysexCount =
  uint8_to_sysex1Byte(data[5], sysexCount, &sysexData[0]);	// 1	- oscFX
  (0 - 6) sysexCount = float_to_sysex5Bytes(data[6], sysexCount, &sysexData[0]);
  // 5 	- detune (0 - 1.00000)
  //sysexCount = String_to_bin(data[7], 4, sysexCount);
  //  	- lfoSyncFreq (not available) sysexCount = float_to_string(data[8], 4,
  sysexCount, &sysexData[0]);	// 4		- midiClkTimeInterval
  //sysexCount = String_to_bin(data[9], 4, sysexCount);
  //  	- lfoTempoValue (not available) sysexCount =
  float_to_sysex5Bytes(data[10], sysexCount, &sysexData[0]);	// 5 	-
  keytrackingAmount (0 - 1.0000) sysexCount = float_to_sysex5Bytes(data[11],
  sysexCount, &sysexData[0]);	// 5	- glideSpeed (0 - 1.00000) sysexCount =
  int8_to_sysex2Bytes(data[12], sysexCount, &sysexData[0]);	// 2	-
  oscPitchA (-24 - +24) sysexCount = int8_to_sysex2Bytes(data[13], sysexCount,
  &sysexData[0]);	// 2 	- oscPitchB (-24 - +24) sysexCount =
  uint8_to_sysex1Byte(data[14], sysexCount, &sysexData[0]);	// 1 	-
  oscWaveformA (0 - 63) sysexCount = uint8_to_sysex1Byte(data[15], sysexCount,
  &sysexData[0]);	// 1 	- oscWaveformB (0 - 63)
  //sysexCount = String_to_bin(data[16], 1, sysexCount, &sysexData[0]);	// -
  pwmSource (1) (not available) sysexCount = float_to_sysex1Byte(data[17],
  sysexCount, &sysexData[0]);	// 1   	- pwmAmtA (0 - 0.99) sysexCount =
  float_to_sysex1Byte(data[18], sysexCount, &sysexData[0]);	// 1   	-
  pwmAmtB (0 - 0.99)
  //sysexCount = String_to_bin(data[19], 6, sysexCount);
  //  	- pwmRate (not available) sysexCount = float_to_sysex2Bytes(data[20],
  sysexCount, &sysexData[0]);	// 2 	- pwA (-1.00 - +1.00) sysexCount =
  float_to_sysex2Bytes(data[21], sysexCount, &sysexData[0]);	// 2 	- pwB
  (-1.00 - +1.00) sysexCount = float_to_string(data[22], 4, sysexCount,
  &sysexData[0]);	// 4	- filterRes (0 - 15.0) sysexCount =
  uint14_to_sysex2Bytes(data[23], sysexCount, &sysexData[0]);// 2 	-
  filterFreq (18 - 12000) sysexCount = float_to_string(data[24], 4, sysexCount,
  &sysexData[0]);	// 4 	- filterMix (0 - -99.0) sysexCount =
  float_to_sysex2Bytes(data[25], sysexCount, &sysexData[0]);	// 2	-
  filterEnv (-1.00 - +1.00) sysexCount = float_to_sysex5Bytes(data[26],
  sysexCount, &sysexData[0]);	// 5	- oscLfoAmt (0 - 1.00000) sysexCount =
  float_to_sysex5Bytes(data[27], sysexCount, &sysexData[0]);	// 5 	-
  oscLfoRate (0 - 40.0000) sysexCount = uint8_to_sysex1Byte(data[28],
  sysexCount, &sysexData[0]);	// 1 	- oscLFOWaveform (0 - 12) sysexCount =
  uint8_to_sysex1Byte(data[29], sysexCount, &sysexData[0]);	// 1 	-
  oscLfoRetrig (0 - 1)
  //sysexCount = String_to_bin(data[30], 1, sysexCount);
  //  	- oscLFOMidiClkSync (not available) sysexCount =
  uint8_to_sysex1Byte(data[31], sysexCount, &sysexData[0]);	// 1 	-
  myFilterLFORateValue (1 - 127) sysexCount = uint8_to_sysex1Byte(data[32],
  sysexCount, &sysexData[0]);	// 1	- filterLfoRetrig (0 - 1)
  //sysexCount = String_to_bin(data[33], 1, sysexCount);
  //  	- oscLFOMidiClkSync (not available) sysexCount =
  float_to_sysex5Bytes(data[34], sysexCount, &sysexData[0]);	// 5 	-
  filterLfoAmt (0 - 1.00000) sysexCount = uint8_to_sysex1Byte(data[35],
  sysexCount, &sysexData[0]);	// 1	- filterLFOWaveform (0 - 12) sysexCount
  = uint14_to_sysex2Bytes(data[36], sysexCount, &sysexData[0]);// 2	-
  filterAttack (0 - 11880) sysexCount = uint14_to_sysex2Bytes(data[37],
  sysexCount, &sysexData[0]);// 2 	- filterDecay (0 - 11880) sysexCount =
  float_to_sysex1Byte(data[38], sysexCount, &sysexData[0]);	// 1	-
  filterSustain (0 - 1.00) sysexCount = uint14_to_sysex2Bytes(data[39],
  sysexCount, &sysexData[0]);// 2	- filterRelease (0 - 11880) sysexCount =
  uint14_to_sysex2Bytes(data[40], sysexCount, &sysexData[0]);// 2	-
  ampAttack (0 - 11880) sysexCount = uint14_to_sysex2Bytes(data[41], sysexCount,
  &sysexData[0]);// 2 	- ampDecay (0 - 11880) sysexCount =
  float_to_sysex1Byte(data[42], sysexCount, &sysexData[0]);	// 1	-
  ampSustain (0 - 1.00) sysexCount = uint14_to_sysex2Bytes(data[43], sysexCount,
  &sysexData[0]);// 2 	- ampRelease (0 - 11880)
  //sysexCount = float_to_sysex5Bytes(data[44], sysexCount, &sysexData[0]);// -
  fxAmt (not available)
  //sysexCount = float_to_sysex5Bytes(data[45], sysexCount, &sysexData[0]);// -
  fxMix (not available)
  //sysexCount = float_to_sysex5Bytes(data[46], sysexCount, &sysexData[0]);// -
  pitchEnv (not available) sysexCount = float_to_sysex1Byte(data[47],
  sysexCount, &sysexData[0]);	// 1	- velocitySens (0-4) sysexCount =
  uint8_to_sysex1Byte(data[48], sysexCount, &sysexData[0]);	// 1	-
  chordDetune (0 - 127) sysexCount = uint8_to_sysex1Byte(data[49], sysexCount,
  &sysexData[0]);	// 1	- FxPot1value (0 - 127) sysexCount =
  uint8_to_sysex1Byte(data[50], sysexCount, &sysexData[0]);	// 1	-
  FxPot2value (0 - 127) sysexCount = uint8_to_sysex1Byte(data[51], sysexCount,
  &sysexData[0]);	// 1	- FxPot3value (0 - 127) sysexCount =
  uint8_to_sysex1Byte(data[52], sysexCount, &sysexData[0]);	// 1	-
  FxPrgNo (0 - 15) sysexCount = uint8_to_sysex1Byte(data[53], sysexCount,
  &sysexData[0]);	// 1	- FxMixValue (0 - 127) sysexCount =
  float_to_sysex5Bytes(data[54], sysexCount, &sysexData[0]);	// 5	-
  FxMixValue (10000 - 60000) sysexCount = uint8_to_sysex1Byte(data[55],
  sysexCount, &sysexData[0]);	// 1	- Osc1WaveBank (0 - 15) sysexCount =
  uint8_to_sysex1Byte(data[56], sysexCount, &sysexData[0]);	// 1	-
  Osc1WaveBank (0 - 15) sysexCount = uint8_to_sysex1Byte(data[57], sysexCount,
  &sysexData[0]);	// 1	- myBoost (0 - 1) sysexCount =
  float_to_sysex2Bytes(data[58], sysexCount, &sysexData[0]);	// 2	-
  pitchEnvA (-1.00 - +1.00) sysexCount = float_to_sysex2Bytes(data[59],
  sysexCount, &sysexData[0]);	// 2	- pitchEnvB (-1.00 - +1.00) sysexCount =
  float_to_sysex2Bytes(data[60], sysexCount, &sysexData[0]);	// 2	-
  driveLevel (0.41 - 1.25) sysexCount = float_to_sysex2Bytes(data[61],
  sysexCount, &sysexData[0]);	// 2	- myFilVelocity (0 - 1.00) sysexCount =
  float_to_sysex2Bytes(data[62], sysexCount, &sysexData[0]);	// 2	-
  myAmpVelocity (0 - 1.00) sysexCount = uint8_to_sysex1Byte(data[63],
  sysexCount, &sysexData[0]);	// 1	- myUnisono (0-2)
  //sysexCount = uint8_to_sysex1Byte(data[64], sysexCount, &sysexData[0]);//
  - dummy
  //sysexCount = uint8_to_sysex1Byte(data[65], sysexCount, &sysexData[0]);//
  - dummy sysexCount = uint8_to_sysex1Byte(data[66], sysexCount, &sysexData[0]);
  // 1	- WShaperNo (0-14) sysexCount = float_to_sysex5Bytes(data[67],
  sysexCount, &sysexData[0]);	// 5	- WShaperDrive (0.10 - 5.00) sysexCount
  = float_to_sysex5Bytes(data[68], sysexCount, &sysexData[0]);	// 5	-
  LFO1phase (0 - 180.0) sysexCount = float_to_sysex5Bytes(data[69], sysexCount,
  &sysexData[0]);	// 5	- LFO2phase (0 - 180.0) sysexCount =
  float_to_sysex5Bytes(data[69], sysexCount, &sysexData[0]);	// 5	- Seq (0
  - 180.0)


  Serial.print("Data: ");
  Serial.println(LFO1phase,5);



  // ..even more dates

  sysexData[sysexCount++] = 0xF7; // 0xF7 End SysEx
  usbMIDI.sendSysEx(sysexCount, sysexData, true);		// send SysEx
  data

  Serial.println("Data sending complete");

  */
}

//*************************************************************************
// Timer Interrupt each 1ms for Enc query
//*************************************************************************
FLASHMEM void EncClockTimer(void) {
  // read Encoder -------------------------------------------------------
  encValue = encoder.read() >> 2;
  if (encValue != previousValue) {
    previousValue = encValue;
    encoderflag = true;
  }
}

//*************************************************************************
// Timer Interrupt each 1ms for Midi Clock
//*************************************************************************
FLASHMEM void MidiClockTimer(void) {
  // read Midi datas ----------------------------------------------------
  MIDI.read(midiChannel); // MIDI 5 Pin DIN
  // usbMIDI.read(midiChannel);		// usbMidi

  // play Sequencer notes -----------------------------------------------
  Sequencer();
}

//*************************************************************************
// Main Loop
//*************************************************************************
FLASHMEM void loop(void) {

  checkPots();
  myPrgChange();

  if (sendSysExFlag == true) {
    mySendSysEx();
    sendSysExFlag = false;
  }

  // draw voice LED into menu pages -------------------------------------
  if (voiceLEDflag == true) {
    drawVoiceLED();
    voiceLEDflag = false;
  }

  // draw red Sequencer step frame --------------------------------------
  if (PageNr == 9 && SEQStepStatus == true) {
    drawSEQStepFrame(SEQselectStepNo);
    SEQStepStatus = false;
  }

  // Enc query ----------------------------------------------------------
  // if ((millis() - timer_encquery) > 27){
  checkEncoder();
  // timer_encquery = millis();
  //}

  // Key query ----------------------------------------------------------
  if ((millis() - timer_keyquery) > 5) {
    checkSwitches();
    timer_keyquery = millis();
  }

  // refresh screen -----------------------------------------------------
  displayThread();

  // set Midi Clock LED -------------------------------------------------
  if (TempoLEDchange == true) {
    setLED(3, TempoLEDstate);
    TempoLEDchange = false;
  }

  // read CPU Temp ------------------------------------------------------
  if ((millis() - timer_Temperature) > 1000) {
    static uint8_t count = 0;
    CPUdegree_temp += tempmonGetTemp();
    count++;
    if (count >= 32) {
      CPUdegree = (CPUdegree_temp >> 5);
      printTemperature();
      CPUdegree_temp = 0;
      count = 0;
    }
    timer_Temperature = millis();
  }

  // CPU Audio Memory ---------------------------------------------------
  if ((millis() - timer_CPUmon) > 500) {
    if (PageNr == 11) {
      CPUaudioMem = AudioProcessorUsageMax(); // AudioMemoryUsageMax();
      printCPUmon();
      AudioProcessorUsageMaxReset(); // AudioMemoryUsageMaxReset();
    }
    // disable scope line if envelopes is stoped
    if (PageNr == 0) {
      if (ampEnvelope1.isActive() == false &&
          ampEnvelope2.isActive() == false &&
          ampEnvelope3.isActive() == false &&
          ampEnvelope4.isActive() == false &&
          ampEnvelope5.isActive() == false &&
          ampEnvelope6.isActive() == false &&
          ampEnvelope7.isActive() == false &&
          ampEnvelope8.isActive() == false) {
        EnvIdelFlag = false;
      }
    }

    // Unisono LED blink
    if (unison == 2) {
      if (unisoFlashStatus == 0) {
        unisoFlashStatus = 1;
        setLED(UnisonoLED, false);

      } else {
        setLED(UnisonoLED, true);
        unisoFlashStatus = 0;
      }
    }
    timer_CPUmon = millis();
  }
}
