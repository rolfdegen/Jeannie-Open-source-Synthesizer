// SysexDump.h


//*************************************************************************
// convert float to SysEx string
//*************************************************************************
FLASHMEM void float_to_string (String value, uint8_t len, uint16_t &sysexCount, byte *sysexData)
{
	for (uint8_t i = 0; i < len; i++) {
		const uint8_t x = 0x7F;
		value[i] = value[i] & x;	// SysEx data no larger than 0x7F
		sysexData[sysexCount++] = value[i];
	}
}

//*************************************************************************
// convert float into SysEx uint7Bit  (float 0 - 0.127)
//*************************************************************************
FLASHMEM void float_to_sysex1Byte (String value, uint16_t &sysexCount, byte *sysexData)
{
	uint8_t val = (value.toFloat() * 100);
	val = val & 0x7F;	// SysEx data no larger than 0x7F
	sysexData[sysexCount++] = val;
}

//*************************************************************************
// convert float into SysEx uint7Bit  (- 127 - +127)
//*************************************************************************
FLASHMEM void int8_to_sysex2Bytes (String value, uint16_t &sysexCount, byte *sysexData)
{
	uint8_t l_byte = (uint8_t)(value.toInt());
	uint8_t h_byte = 0;
	
	if (l_byte > 127) {
		l_byte = 256 - l_byte;			// make positive value
		h_byte = 1;						// minus Sign
	}

	h_byte = h_byte & 0x7F;	// SysEx data no larger than 0x7F
	l_byte = l_byte & 0x7F;	// SysEx data no larger than 0x7F
	
	sysexData[sysexCount++] = h_byte;	// Sign (- = 1 / + = 0)
	sysexData[sysexCount++] = l_byte;	// data
}

//*************************************************************************
// convert string into SysEx int7 (float -1.27 - +1.27)
//*************************************************************************
FLASHMEM void float_to_sysex2Bytes (String value, uint16_t &sysexCount, byte *sysexData)
{
	uint8_t h_byte = int8_t(value.toFloat() * 100);
	int8_t l_byte = h_byte;
	
	// calc h_byte
	h_byte >>= 7;
	
	// l_byte is sign
	if (l_byte < 0) {
		l_byte = l_byte * (-1);
	}
	
	h_byte = h_byte & 0x7F;	// SysEx data no larger than 0x7F
	l_byte = l_byte & 0x7F;	// SysEx data no larger than 0x7F
	
	sysexData[sysexCount++] = h_byte;	// Sign (- = 1 / + = 0)
	sysexData[sysexCount++] = l_byte;	// data
}

//*************************************************************************
// convert float into SysEx uint14 (float 0.00 - 16.383)
//*************************************************************************
FLASHMEM uint16_t float_to_uint14bit (String value, uint16_t sysexCount, byte *sysexData)
{
	uint8_t h_byte = int8_t(value.toFloat() * 100);
	int8_t l_byte = h_byte;
	
	// calc h_byte
	h_byte >>= 7;
	
	// l_byte is sign
	if (l_byte < 0) {
		l_byte = l_byte * (-1);
	}
	
	h_byte = h_byte & 0x7F;	// SysEx data no larger than 0x7F
	l_byte = l_byte & 0x7F;	// SysEx data no larger than 0x7F
	
	sysexData[sysexCount++] = h_byte;	// Sign (- = 1 / + = 0)
	sysexData[sysexCount++] = l_byte;	// data
	
	return sysexCount;
}

//*************************************************************************
// convert 7bit value into SysEx 1Byte ( 0-127)
//*************************************************************************
FLASHMEM void uint8_to_sysex1Byte (String value, uint16_t &sysexCount, byte *sysexData)
{
	uint8_t val =  value.toInt();
	val = val & 0x7F;	// SysEx data no larger than 0x7F
	sysexData[sysexCount++] = val;
}

//*************************************************************************
// convert 14bit value into SysEx 2Byte (0-16383)
//*************************************************************************
FLASHMEM void uint14_to_sysex2Bytes (String value, uint16_t &sysexCount, byte *sysexData)
{
	uint16_t var16 = (value.toInt());
	uint16_t var16_x = var16;
	
	// calc h_byte & l_byte -----------
	var16 = var16 << 1;
	uint8_t h_byte = var16 >> 8;
	uint8_t l_byte = var16_x & 0x7F;
	
	h_byte = h_byte & 0x7F;	// SysEx data no larger than 0x7F
	l_byte = l_byte & 0x7F;	// SysEx data no larger than 0x7F
	
	sysexData[sysexCount++] = h_byte;
	sysexData[sysexCount++] = l_byte;
}


//*************************************************************************
// convert 32bit float value into SysEx 5Byte
//*************************************************************************
FLASHMEM void float_to_sysex5Bytes(String value, uint16_t &sysexCount, byte *sysexData)
{
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
	for (uint8_t i = 0; i < 5; i++){
		sysexBytes[i] = (sysexBytes[i]) & 0x7F;	// SysEx data no larger than 0x7F
		sysexData[sysexCount++] = sysexBytes[i];
	}
}

//*************************************************************************
// print verify SysEx data
//*************************************************************************
FLASHMEM void sysexCompare(uint8_t prg, uint8_t bank, uint16_t startPoint, uint16_t lenght) {
	
	// get Sound File String
	
	uint8_t patchNo = prg;			// Patch No 1-128
	uint8_t currentPatchBank = bank;			// A-P = 0-15
	String numString = (patchNo);
	String bankString = char(currentPatchBank + 65);
	String fileString = (bankString + "/" + numString);
	uint16_t data_len = NO_OF_PARAMS;
	File patchFile = SD.open(fileString.c_str());
	String data[data_len]; //Array of data read in
	recallPatchData(patchFile, data);
	patchFile.close();
	
	for (uint16_t i = startPoint; i < lenght; i++ ) {
		Serial.print(i); Serial.print(": "); Serial.println(data[i]);
	}
	
	Serial.println(" ");
}

//*************************************************************************
// convert 5Byte SysEx to 32bit float
//*************************************************************************
FLASHMEM float sysex5Bytes_to_float(const uint8_t *buffer) {
	
	union {
		float fval;
		byte bval[4];
	} floatAsBytes;
	
	uint8_t bit_val = buffer[4];
	
	for (uint8_t i = 0; i < 4; i++) {
		if ((bit_val & 0x01) > 0) {
			floatAsBytes.bval[3-i] = buffer[3-i] + 0x80;
			} else {
			floatAsBytes.bval[3-i] = buffer[3-i];
		}
		bit_val = bit_val >> 1;
	}
	float f = *(float*)floatAsBytes.bval;
	return f;
}


//*************************************************************************
// SysEx 1Byte to string
//*************************************************************************
FLASHMEM String sysex1Byte_to_string(byte *buffer, uint16_t &count, uint8_t len)
{
	return String(float((buffer[count++]) / 100.0f), len);
}

//*************************************************************************
// SysEx int 7bit to string  (-1.00 - +1.00)
//*************************************************************************
FLASHMEM String sysex2Byte_to_string(byte *buffer, uint16_t &count, uint8_t len)
{
	String str;
	
	if (buffer[count] > 0) { str += '-';}	count++;
	return str + String((float((buffer[count++])) / 100.0f), len);
}


//*************************************************************************
// SysEx 5Byte (float) to string  (0.00000)
//*************************************************************************
FLASHMEM String sysex5Byte_to_string(byte *buffer, uint16_t &count, uint8_t len)
{
	String str;
	byte dataBuf[5];
	
	for (uint8_t i = 0; i < 5; i++) {
		dataBuf[i] = buffer[count++];
	}
	float f =  sysex5Bytes_to_float(&dataBuf[0]);
	str += String(f,len);
	
	return str;
}

//*************************************************************************
// SysEx String to float
//*************************************************************************
FLASHMEM String sysexString_to_float(byte *buffer, uint16_t &count, uint8_t len)
{
	String str;
	
	for (uint8_t i = 0; i < len; i++) {
		str += char(buffer[count++]);
	}
	return str;
}

//*************************************************************************
// SysEx 2Bytes to int8
//*************************************************************************
FLASHMEM String sysex2Byte_to_int8(byte *buffer, uint16_t &count)
{
	String str;
	
	if (buffer[count] > 0) {
		str += '-';
	}
	count++;
	str += String(buffer[count++]);

	return str;
}

//*************************************************************************
// set ProgressBar
//*************************************************************************
FLASHMEM void setProgressbar(boolean enabled, uint8_t style, uint8_t count, uint8_t maxcount, String txtstring){
	
	ProgBar_enabled = enabled;
	ProgBar_style = style;
	ProgBar_count = count;
	Progbar_maxcount = maxcount;
	Progbar_string = txtstring;
}

//*************************************************************************
// usbMIDI receive SystemExclusive Dump
// Patch Dump is 401 Byte
// Bank Dump is 51.328 Byte
// NO_OF_SysEx_Data: 401 Byte
//*************************************************************************
FLASHMEM void get_SysEx_Dump(void) {
		
	// save patch Parameter and ignore data 0-18
	uint16_t count = 19;	// Pointer Data									// (ind) (Value)
	oscALevel = sysex1Byte_to_string(Syx_Buf, count, 2).toFloat();			// (1) oscALevel (0 - 1.00)
	oscBLevel = sysex1Byte_to_string(Syx_Buf, count, 2).toFloat();			// (2) oscALevel (0 - 1.00)
	noiseLevel = sysex2Byte_to_string(Syx_Buf, count, 2).toFloat();			// (3) noiseLevel ( -1.00 - +1.00)
	unison = Syx_Buf[count++];												// (4) unison (0 - 2)
	oscFX = Syx_Buf[count++];												// (5) oscFX (0 - 6)
	detune = sysex5Byte_to_string(Syx_Buf, count, 5).toFloat();				// (6) detune (0 - 1.00000)
	// (7) (not available)
	midiClkTimeInterval = sysexString_to_float(Syx_Buf, count, 4).toFloat();// (8) midiClkTimeInterval
	// (9) (not available)
	keytrackingAmount = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();	// (10) keytrackingAmount (0 - 1.0000)
	glideSpeed = sysex5Byte_to_string(Syx_Buf, count, 5).toFloat();			// (11) glideSpeed (0 - 1.00000)
	oscPitchA = sysex2Byte_to_int8(Syx_Buf, count).toInt();					// (12) oscPitchA (-24 - +24)
	oscPitchB = sysex2Byte_to_int8(Syx_Buf, count).toInt();					// (13) oscPitchB (-24 - +24)
	oscWaveformA = Syx_Buf[count++];										// (14) oscWaveformA (0 - 63)
	oscWaveformB = Syx_Buf[count++];										// (15) oscWaveformB (0 - 63)
	// (16) (not available)
	pwmAmtA = sysex1Byte_to_string(Syx_Buf, count, 2).toFloat();			// (17) pwmAmtA (0 - 0.99)
	pwmAmtB = sysex1Byte_to_string(Syx_Buf, count, 2).toFloat();			// (18) pwmAmtB (0 - 0.99)
	// (19) (not available)
	pwA = sysex2Byte_to_string(Syx_Buf, count, 2).toFloat();				// (20) pwA (-1.00 - +1.00)
	pwB = sysex2Byte_to_string(Syx_Buf, count, 2).toFloat();				// (21) pwB (-1.00 - +1.00)
	filterRes = sysexString_to_float(Syx_Buf, count, 4).toFloat();			// (22) filterRes (0 - 15.0)
	filterFreq = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);				// (23) filterFreq (18 - 12000)
	filterMix = sysexString_to_float(Syx_Buf, count, 4).toFloat();			// (24) filterMix (0 - -99.0)
	filterEnv = sysex2Byte_to_string(Syx_Buf, count, 2).toFloat();			// (25) filterEnv (-1.00 - +1.00)
	oscLfoAmt = sysex5Byte_to_string(Syx_Buf, count, 5).toFloat();			// (26) oscLfoAmt (0 - 1.00000)
	oscLfoRate = sysex5Byte_to_string(Syx_Buf, count, 5).toFloat();			// (27) oscLfoRate (0 - 40.0000)
	oscLFOWaveform = Syx_Buf[count++];										// (28) oscLFOWaveform (0 - 127)
	oscLfoRetrig = Syx_Buf[count++];										// (29) oscLfoRetrig (0 - 1)
	// (30) (not available)
	myFilterLFORateValue = Syx_Buf[count++];								// (31) myFilterLFORateValue (1 - 127)
	myLFO2RateValue = myFilterLFORateValue;
	filterLfoRate = LFOMAXRATE * POWER[myLFO2RateValue];
	filterLfoRetrig = Syx_Buf[count++];										// (32) filterLfoRetrig (0 - 1)
	// (33) (not available)
	filterLfoAmt = sysex5Byte_to_string(Syx_Buf, count, 2).toFloat();		// (34) filterLfoAmt (0 - 1.00000)
	filterLfoWaveform = Syx_Buf[count++];									// (35) filterLfoWaveform (0 - 127)
	filterAttack = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);			// (36) filterAttack (0 - 11880)
	filterDecay = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);				// (37) filterDecay (0 - 11880)
	filterSustain = sysex1Byte_to_string(Syx_Buf, count, 2).toFloat();		// (38) filterSustain (0 - 1.00)
	filterRelease = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);			// (39) filterRelease (0 - 11880)
	ampAttack = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);				// (40) ampAttack (0 - 11880)
	ampDecay = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);				// (41) ampDecay (0 - 11880)
	ampSustain = sysex1Byte_to_string(Syx_Buf, count, 2).toFloat();			// (42) ampSustain (0 - 1.00)
	ampRelease = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);				// (43) ampRelease (0 - 11880)
	// (44) (not available)
	// (45) (not available)
	// (46) (not available)
	velocitySens = Syx_Buf[count++];										// (47) velocitySens (0-4)
	chordDetune = Syx_Buf[count++];											// (48) chordDetune (0 - 127)
	FxPot1value = Syx_Buf[count++];											// (49) FxPot1value (0 - 127)
	FxPot2value = Syx_Buf[count++];											// (50) FxPot2value (0 - 127)
	FxPot3value = Syx_Buf[count++];											// (51) FxPot3value (0 - 127)
	FxPrgNo = Syx_Buf[count++];												// (52) FxPrgNo (0 - 15)
	FxMixValue = Syx_Buf[count++];											// (53) FxMixValue (0 - 127)
	FxClkRate = sysex5Byte_to_string(Syx_Buf, count, 0).toInt();			// (54) FxClkRate (10000 - 60000)
	Osc1WaveBank = Syx_Buf[count++];										// (55) Osc1WaveBank (0 - 15)
	Osc2WaveBank = Syx_Buf[count++];										// (56) Osc2WaveBank (0 - 15)
	myBoost = Syx_Buf[count++];												// (57) myBoost (0 - 1)
	pitchEnvA = sysex5Byte_to_string(Syx_Buf, count, 5).toFloat();			// (58) pitchEnvA (-1.00000 - +1.00000)
	pitchEnvB = sysex5Byte_to_string(Syx_Buf, count, 5).toFloat();			// (59) pitchEnvB (-1.00000 - +1.00000)
	driveLevel = sysex2Byte_to_string(Syx_Buf, count, 2).toFloat();			// (60) driveLevel (0.00 - 1.25)
	myFilVelocity = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();		// (61) myFilVelocity (0 - 1.00)
	myAmpVelocity = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();		// (62) myAmpVelocity (0 - 1.00)
	myUnisono = Syx_Buf[count++];											// (63) myUnisono (0-2)
	// (64) (not available)
	// (65) (not available)
	WShaperNo = Syx_Buf[count++];											// (66) WShaperNo (0-14)
	WShaperDrive = sysex5Byte_to_string(Syx_Buf, count, 3).toFloat();		// (67) WShaperDrive (0.10 - 5.00)
	LFO1phase = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);				// (68) LFO1phase (0 - 180.0)
	LFO2phase = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);				// (69) LFO2phase (0 - 180.0)
	for (uint8_t i = 0; i < 16; i++) {										// (70-85) SeqNote1Buf (0 - 127)
		SeqNote1Buf[i] = Syx_Buf[count++];
	}
	for (uint8_t i = 0; i < 16; i++) {										// (86-101) SeqNoteBufStatus (0 - 1)
		SeqNoteBufStatus[i] = Syx_Buf[count++];
	}
	SEQbpmValue = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);				// (102) SEQbpmValue (101 - 462)
	SEQdivValue = sysex5Byte_to_string(Syx_Buf, count, 6).toFloat();		// (103) SEQdivValue (float)
	SEQstepNumbers = Syx_Buf[count++];										// (104) SEQstepNumbers (0 - 15)
	SEQGateTime = sysex5Byte_to_string(Syx_Buf, count, 6).toFloat();		// (105) SEQGateTime (float)
	SEQdirection = Syx_Buf[count++];										// (106) SEQdirection (0 - 3)
	oscDetuneSync = Syx_Buf[count++];										// (107) oscDetuneSync (0 - 1)
	oscTranspose = sysex2Byte_to_int8(Syx_Buf, count).toFloat();			// (108) oscTranspose (-12 - +12)
	oscMasterTune = sysex5Byte_to_string(Syx_Buf, count, 6).toFloat();		// (109) oscMasterTune (float)
	OscVCFMOD = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();			// (110) OscVCFMOD (float)
	for (uint8_t i = 0; i < 16; i++) {										// (111-126) SeqVeloBuf (0 - 127)
		SeqVeloBuf[i] = Syx_Buf[count++];
	}
	for (uint8_t i = 0; i < 48; i++) {										// (127-174) SeqNote1Buf (0 - 127)
		SeqNote1Buf[16 + i] = Syx_Buf[count++];
	}
	for (uint8_t i = 0; i < 16; i++) {										// (175-190) SeqNoteCount (0 - 3)
		SeqNoteCount[i] = Syx_Buf[count++];
	}
	SEQmode = Syx_Buf[count++];												// (191) SEQmode (0 - 2)
	SEQMidiClkSwitch = Syx_Buf[count++];									// (192) SEQMidiClkSwitch (0 - 1)
	LadderFilterpassbandgain = Syx_Buf[count++];							// (193) LadderFilterpassbandgain (0 - 127)
	LadderFilterDrive = Syx_Buf[count++];									// (194) LadderFilterDrive (1 - 127)
	envelopeType1 = sysex2Byte_to_int8(Syx_Buf, count).toFloat();			// (195) envelopeType1 (-8 - +8)
	envelopeType2 = sysex2Byte_to_int8(Syx_Buf, count).toFloat();			// (196) envelopeType2 (-8 - +8)
	PitchWheelAmt = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();		// (197) PitchWheelAmt (float)
	MODWheelAmt = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();		// (198) MODWheelAmt (float)
	myFilter = Syx_Buf[count++];											// (199) myFilter (1 - 2)
	pwmRateA = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();			// (200) pwmRateA (-10.00 - + 10.00)
	pwmRateB = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();			// (201) pwmRateB (-10.00 - + 10.00)
	LFO1fadeTime = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);			// (202) LFO1fadeIn Time (0 - 12000)
	LFO1releaseTime = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);			// (203) LFO1releaseTime (0 - 12000)
	filterFM = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();			// (204) filterFM Osc1 (0.00000 - 1.00000)
	filterFM2 = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();			// (205) filterFM2 Osc2 (0.00000 - 1.00000)
	LFO2fadeTime = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);			// (206) LFO2fadeIn Time (0 - 12000)
	LFO2releaseTime = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);			// (207) LFO2releaseTime Fade out (0 - 12000)
	Osc1ModAmt = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();			// (208) Osc1ModAmt (0.00000 - 1.00000)
	LFO1envCurve = sysex2Byte_to_int8(Syx_Buf, count).toFloat();			// (209) LFO1envCurve (-8 - +8)
	LFO2envCurve = sysex2Byte_to_int8(Syx_Buf, count).toFloat();			// (210) LFO2envCurve (-8 - +8)
	LFO1mode = Syx_Buf[count++];											// (211) LFO1mode (0 - 1)
	lfo1oneShoot = LFO1mode;
	LFO2mode = Syx_Buf[count++];											// (212) LFO2mode (0 - 1)
	lfo2oneShoot = LFO2mode;
	LFO3envCurve = sysex2Byte_to_int8(Syx_Buf, count).toFloat();			// (213) LFO3envCurve (-8 - +8)
	LFO3mode = Syx_Buf[count++]; 											// (214) LFO3mode (0 - 1)
	lfo3oneShoot = LFO3mode;
	LFO3fadeTime = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);			// (215) LFO3fadeIn Time (0 - 12000)
	LFO3releaseTime = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);			// (216) LFO3releaseTime Fade out (0 - 12000)
	Lfo3amt = sysex5Byte_to_string(Syx_Buf, count, 5).toFloat();			// (217) Lfo3amt (0 - 1.00000)
	AtouchAmt = Syx_Buf[count++];											// (218) AtouchAmt (0 - 127)
	AtouchPitchAmt = Syx_Buf[count++];										// (219) AtouchPitchAmt (0 - 127)
	Lfo3Waveform = Syx_Buf[count++];										// (220) Lfo3Waveform (0 - 127)
	myLFO3RateValue = Syx_Buf[count++];										// (221) myLFO3RateValue (0 - 127)
	Lfo3Rate = LFOMAXRATE2 * POWER[myLFO3RateValue];
	LFO3phase = (Syx_Buf[count++] << 7) + (Syx_Buf[count++]);				// (222) LFO3phase (0 - 180)
	AtouchLFO1Amt = Syx_Buf[count++];										// (223) AtouchLFO1Amt (0 - 127)
	AtouchLFO2Amt = Syx_Buf[count++];										// (224) AtouchLFO2Amt (0 - 127)
	AtouchLFO3Amt = Syx_Buf[count++];										// (225) AtouchLFO3Amt (0 - 127)
	FxPot1amt = Syx_Buf[count++];											// (226) FxPot1amt (0 - 127)
	FxPot2amt = Syx_Buf[count++];											// (227) FxPot2amt (0 - 127)
	FxPot3amt = Syx_Buf[count++];											// (228) FxPot3amt (0 - 127)
	FxCLKamt = Syx_Buf[count++];											// (229) FxCLKamt (0 - 127)
	FxMIXamt = Syx_Buf[count++];											// (230) FxMIXamt (0 - 127)
	AtouchFxP1Amt = Syx_Buf[count++];										// (231) AtouchFxP1Amt (0 - 127)
	AtouchFxP2Amt = Syx_Buf[count++];										// (232) AtouchFxP2Amt (0 - 127)
	AtouchFxP3Amt = Syx_Buf[count++];										// (233) AtouchFxP3Amt (0 - 127)
	AtouchFxClkAmt = Syx_Buf[count++];										// (234) AtouchFxClkAmt (0 - 127)
	AtouchFxMixAmt = Syx_Buf[count++];										// (235) AtouchFxMixAmt (0 - 127)
	PWMaShape = Syx_Buf[count++];											// (236) PWMaShape (0 - 127)
	PWMbShape = Syx_Buf[count++];											// (237) PWMbShape (0 - 127)
	HPF_filterFreq = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();		// (238) HPF_filterFreq (float)
	UserPot1 = Syx_Buf[count++];											// (239) UserPot1 (0 - 127)
	UserPot2 = Syx_Buf[count++];											// (240) UserPot2 (0 - 127)
	UserPot3 = Syx_Buf[count++];											// (241) UserPot3 (0 - 127)
	UserPot4 = Syx_Buf[count++];											// (242) UserPot4 (0 - 127)
	HPFRes = sysex5Byte_to_string(Syx_Buf, count, 4).toFloat();				// (243) HPFRes (float)
	SupersawSpreadA = Syx_Buf[count++];										// (244) SupersawSpreadA (0-127)
	SupersawSpreadB = Syx_Buf[count++];										// (245) SupersawSpreadB (0-127)
	SupersawMixA = Syx_Buf[count++];										// (246) SupersawMixA (0-127)
	SupersawMixB = Syx_Buf[count++];										// (247) SupersawMixB (0-127)
	Voice_mode = Syx_Buf[count++];											// (248) Voice_mode (0...5)
	pan_value = Syx_Buf[count++];											// (249) VCA PAN (0...127)
}