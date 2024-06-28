#include <EEPROM.h>

#define EEPROM_MIDI_CH 0
#define EEPROM_PITCHBEND 1
#define EEPROM_MODWHEEL_DEPTH 2
#define EEPROM_ENCODER_DIR 3
#define EEPROM_PICKUP_ENABLE 4
#define EEPROM_BASSENHANCE_ENABLE 5
#define EEPROM_SCOPE_ENABLE 6
#define EEPROM_MIDI_OUT_CH 7
#define EEPROM_VU_ENABLE 8
#define EEPROM_TRANSPOSE 10				// int  9-10
#define EEPROM_MasterTune 18			// double 11-14 
#define EEPROM_SoundPatchNo 25
#define EEPROM_PatchBankNo 26
#define EEPROM_EEPROMinit 35			// 4 byte (if 010203h EEPROM is init) 
#define EEPROM_MidiClk 50				// Midi Clk on/off
#define EEPROM_PRGchange 52				// PRGchange Switch
#define EEPROM_Pickup 54
#define EEPROM_USBctrl 56

FLASHMEM int getMIDIChannel() {
  byte midiChannel = EEPROM.read(EEPROM_MIDI_CH);
  if (midiChannel < 0 || midiChannel > 16) midiChannel = MIDI_CHANNEL_OMNI;//If EEPROM has no MIDI channel stored
  return midiChannel;
}

FLASHMEM void storeMidiChannel(byte channel){
  EEPROM.update(EEPROM_MIDI_CH, channel);
}

FLASHMEM int getPitchBendRange() {
  byte pitchbend = EEPROM.read(EEPROM_PITCHBEND);
  if (pitchbend < 1 || pitchbend > 12) return pitchBendRange; //If EEPROM has no pitchbend stored
  return pitchbend;
}

FLASHMEM void storePitchBendRange(byte pitchbend){
  EEPROM.update(EEPROM_PITCHBEND, pitchbend);
}

FLASHMEM void storePRGchange(byte PrgChangeSW){
	EEPROM.put(EEPROM_PRGchange, PrgChangeSW);
}

FLASHMEM void getPRGchange(void){
	EEPROM.get(EEPROM_PRGchange, PrgChangeSW);
}

FLASHMEM void storePickup(byte PickupSW){
	EEPROM.put(EEPROM_Pickup, PickupSW);
}

FLASHMEM void getUSBctrl(void){
	EEPROM.get(EEPROM_USBctrl, USBctrl);
}

FLASHMEM void storeUSBctrl(byte USBctrl){
	EEPROM.update(EEPROM_USBctrl, USBctrl);
}

/*
FLASHMEM void getPickup(void){
	EEPROM.get(EEPROM_Pickup, PickupSW);
	pickupFlag = PickupSW;
}*/


FLASHMEM float getModWheelDepth() {
  byte mw = EEPROM.read(EEPROM_MODWHEEL_DEPTH);
  if (mw < 1 || mw > 10) return modWheelDepth; //If EEPROM has no mod wheel depth stored
  return mw / 10.0f;
}

FLASHMEM void storeModWheelDepth(float mwDepth){
  byte mw =  mwDepth * 10;
  EEPROM.update(EEPROM_MODWHEEL_DEPTH, mw);
}

FLASHMEM int getMIDIOutCh() {
  byte mc = EEPROM.read(EEPROM_MIDI_OUT_CH);
  if (mc < 0 || midiOutCh > 16) mc = 0;//If EEPROM has no MIDI channel stored
  return mc;
}

FLASHMEM void storeMidiOutCh(byte channel){
  EEPROM.update(EEPROM_MIDI_OUT_CH, channel);
}

FLASHMEM boolean getEncoderDir() {
  byte ed = EEPROM.read(EEPROM_ENCODER_DIR); 
  if (ed < 0 || ed > 1)return true; //If EEPROM has no encoder direction stored
  return ed == 1 ? true : false;
}

FLASHMEM void storeEncoderDir(byte encoderDir){
  EEPROM.update(EEPROM_ENCODER_DIR, encoderDir);
}

FLASHMEM boolean getPickupEnable() {
  byte pu = EEPROM.read(EEPROM_PICKUP_ENABLE); 
  if (pu < 0 || pu > 1)return false; //If EEPROM has no pickup enable stored
  return pu == 1 ? true : false;
}

FLASHMEM void storePickupEnable(byte pickupEnable){
  EEPROM.update(EEPROM_PICKUP_ENABLE, pickupEnable);
}

FLASHMEM boolean getBassEnhanceEnable() {
  byte eh = EEPROM.read(EEPROM_BASSENHANCE_ENABLE); 
  if (eh < 0 || eh > 1)return false; //If EEPROM has no bass enhance enable stored
  return eh == 1 ? true : false;
}

FLASHMEM void storeBassEnhanceEnable(byte bassEnhanceEnable){
  EEPROM.update(EEPROM_BASSENHANCE_ENABLE, bassEnhanceEnable);
}

FLASHMEM boolean getScopeEnable() {
  byte sc = EEPROM.read(EEPROM_SCOPE_ENABLE); 
  if (sc < 0 || sc > 1)return false; //If EEPROM has no scope enable stored
  return sc == 1 ? true : false;
}

FLASHMEM void storeScopeEnable(byte ScopeEnable){
  EEPROM.update(EEPROM_SCOPE_ENABLE, ScopeEnable);
}

FLASHMEM boolean getVUEnable() {
  byte vu = EEPROM.read(EEPROM_VU_ENABLE); 
  if (vu < 0 || vu > 1)return false; //If EEPROM has no VU enable stored
  return vu == 1 ? true : false;
}

FLASHMEM void storeVUEnable(byte VUEnable){
  EEPROM.update(EEPROM_VU_ENABLE, VUEnable);
}

FLASHMEM int getSoundPachNo() {
	byte SoundPatchNo = EEPROM.read(EEPROM_SoundPatchNo);
	return SoundPatchNo;
}

FLASHMEM void storeSoundPatchNo(uint8_t patchNo){
	EEPROM.update(EEPROM_SoundPatchNo, patchNo);
}

FLASHMEM int getPatchBankNo() {
	byte PatchBankNo = EEPROM.read(EEPROM_PatchBankNo);
	return PatchBankNo;
}

FLASHMEM void storePatchBankNo(uint8_t BankNo){
	EEPROM.update(EEPROM_PatchBankNo, BankNo);
}
FLASHMEM void storeEEPROMinit() {
	EEPROM.put(EEPROM_EEPROMinit, myEEPROMinit);
}
FLASHMEM int getEEPROMinit() {
	uint32_t value1 = 0;
	EEPROM.get(EEPROM_EEPROMinit, value1);
	return value1;
}

FLASHMEM void storeMidiCkl(byte MidiSyncSwitch){
	EEPROM.update(EEPROM_MidiClk, MidiSyncSwitch);
}

FLASHMEM void getMidiCkl(void){
	EEPROM.get(EEPROM_MidiClk, MidiSyncSwitch);
}

