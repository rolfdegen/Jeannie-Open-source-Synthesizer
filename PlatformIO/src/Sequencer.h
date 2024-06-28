// Sequencer.h


//*************************************************************************
// Prototyps
//*************************************************************************
void polySequencer (void);
void Rec_mode (uint8_t note, uint8_t velocity);
uint8_t Trans_mode(uint8_t note);
void SequencerRecNotes (uint8_t note, uint8_t velo);
void clearPatternData(void);
void initPatternData(void);
void setCurrentPatternData(String data[]);
void recallPattern(int patternNo);

//*************************************************************************
// Play sequencer notes from intern clock
//*************************************************************************
FLASHMEM void polySequencer(void)
{
	// Interval and gateTime is calc with Seq.Parameters BPM/DIV/TIME
	
	if (SEQMidiClkSwitch == false || SeqStepClk == true) {
		// Play Sequencer Note
		if ((micros() - timer_intMidiClk) > Interval) {
			if (ARPSEQstatus == 0 && SEQrunStatus == true) {
				timer_intMidiClk = micros();	// clear Interval timer
				ARPSEQstatus = 1;
				SEQStepStatus = true;
				SEQ_CLK_Led_count++;
					if (SEQ_CLK_Led_count == 1)
					{
						TempoLEDstate = true;	// set Tempo LED on
						TempoLEDchange = true;
					}
					else if (SEQ_CLK_Led_count == 2)
					{
						TempoLEDstate = false;	// set Tempo LED off
						TempoLEDchange = true;
					}
					else if (SEQ_CLK_Led_count == 4)
					{
						SEQ_CLK_Led_count = 0;
					}	
				if (SeqNoteBufStatus[SEQselectStepNo] == 1) {
					uint8_t velo = SeqVeloBuf[SEQselectStepNo];
					if (velo <= 0) {
						velo = 127;
					}
					uint8_t noteCount = SeqNoteCount[SEQselectStepNo];
					for (uint8_t i = 0; i < noteCount; i++) {
						int bufAddr = (i * 16);
						int myNote = SeqNote1Buf[SEQselectStepNo + bufAddr];
						myNoteOn2(channel,myNote, velo);
						EnvIdelFlag = true; // oscilloscope enabled
					}
				}
			}
		}
		// If gateTime is over then turn off Sequencer Note
		if ((micros() - timer_intMidiClk) > gateTime) {
			if (ARPSEQstatus == true && SEQrunStatus == true) {
				ARPSEQstatus = false;
				if (SeqNoteBufStatus[SEQselectStepNo] == 1) {
					uint8_t noteCount = SeqNoteCount[SEQselectStepNo];
					for (uint8_t i = 0; i < noteCount; i++) {
						myNoteOff(midiChannel, SeqNote1Buf[(SEQselectStepNo + (i * 16))], 0);
					}
				}
				// play Sequencer note (in Play Mode)
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
						SEQselectStepNo = random(SEQstepNumbers + 1);
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
				TempoLEDstate = false;	// set Tempo LED off
				TempoLEDchange = true;
			}
			SeqStepClk = false;		// if move STEP then stop note
		}
	}
}

//*************************************************************************
// Sequencer Transpose mode
//*************************************************************************
FLASHMEM uint8_t Trans_mode(uint8_t note) {
	
	uint8_t transpose = 0;
	
	if (SEQrunStatus == true && SEQmode == 1) {
		for (uint8_t i = 0; i < SEQstepNumbers; i++) {
			if (SeqNoteBufStatus[i] == 1) {
				SeqTranspose = note - SeqNote1Buf[i];
				return transpose = 1;
			}
		}
		SeqTranspose = 0;
		return transpose = 1;
	}
	
	return transpose = 0;
}

//*************************************************************************
// recording Midi Notes in Sequencer Edit mode
//*************************************************************************
FLASHMEM void Rec_mode (uint8_t note, uint8_t velocity) {
	
		if (PageNr == 11 && SEQrunStatus == false) {
			uint8_t numbers_of_notes = 0;
			if (unison == 0) {
				numbers_of_notes = 4;
			} else numbers_of_notes = 1; // max. 2 notes for unison
			
		if (SeqRecNoteCount <= numbers_of_notes) {
			SeqRecNoteCount++;
		}
		SequencerRecNotes(note, velocity);
	}
}

//*************************************************************************
// Recording notes into sequencer
//*************************************************************************
void SequencerRecNotes (uint8_t note, uint8_t velo) {
	
	if (PageNr == 11 && SEQmode == 2 && SEQrunStatus == false && SeqRecNoteCount <= 4) {
		
		// if all notes Off then next step ----------------------------
		if (SeqRecNoteCount == 0){
			SEQselectStepNo++;
			if (SEQselectStepNo > SEQstepNumbers) {
				SEQselectStepNo = 0;
			}
			SEQRecNoteFlag = true;  // draw next step
			return;
		}
		
		// save notes to seq buffer -----------------------------------
		SeqNote1Buf[(SEQselectStepNo + ((SeqRecNoteCount-1) * 16))] = note; // Seq Note buffer
		
		// if 1.Note than save velo and status
		if (SeqRecNoteCount == 1) {
			SeqNoteCount[SEQselectStepNo] = 1;	// NoteOn Status
			SeqVeloBuf[SEQselectStepNo] = velo;	// Velo from 1.Note
			SeqNoteBufStatus[SEQselectStepNo] = true;
		}
		// next notes save note count and draw notes
		SeqNoteCount[SEQselectStepNo] = SeqRecNoteCount;
		SEQRecNoteFlag = true;  // draw Pitch note
		
		// Sequencer notes available
		SeqNotesAvailable = true;
	}
}

//************************************************************************
//clear Sequencer Pattern data
//************************************************************************
FLASHMEM void clearPatternData(void) {
	patternName = INITPATTERNNAME;
	if (Keylock == true) {
		return;
	}
	for (uint8_t i = 0; i < 64; i++) {
		SeqNote1Buf[i] = 0;	// Midi Note C0
	}
	for (uint8_t i = 0; i < 16; i++) {
		SeqNoteCount[i] = 0;	// Step note count
	}
	for (uint8_t i = 0; i < 16; i++) {
		SeqVeloBuf[i] = 0;	// Velocity
	}
	for (uint8_t i = 0; i < 16; i++) {
		SeqNoteBufStatus[i] = 0;	// all Notes mute
	}
}

//************************************************************************
// int Sequencer Pattern data
//************************************************************************
FLASHMEM void initPatternData(void) {
	patternName = INITPATTERNNAME;
	if (Keylock == true) {
		return;
	}
	for (uint8_t i = 0; i < 64; i++) {
		SeqNote1Buf[i] = 0;	// Midi Note C0
	}
	for (uint8_t i = 0; i < 16; i++) {
		SeqNoteCount[i] = 0;	// Step note count
	}
	for (uint8_t i = 0; i < 16; i++) {
		SeqVeloBuf[i] = 0;	// Velocity
	}
	for (uint8_t i = 0; i < 16; i++) {
		SeqNoteBufStatus[i] = 0;	// all Notes mute
	}
	
	SEQbpmValue = 120;	// BPM = 120
	SEQdivValue = ((1.0f / 8.0f) * 4.0f);	// 1/8
	float bpm = float(SEQbpmValue / SEQdivValue);
	SEQclkRate = (60000000 / bpm);
	SEQstepNumbers = 15;			// 16 Steps
	Interval = SEQclkRate;
	gateTime = 3.94f;  // init value: 32
	SEQdirection = 0;
	SEQdirectionFlag = false;
	SeqSymbol = false;
	SEQmode = 0;
}

//************************************************************************
// set Sequencer Pattern data
//************************************************************************
FLASHMEM void setCurrentPatternData(String data[]) {
	
	patternName = data[0];
	
	if (Keylock == false) {
		
		// Sequencer Step 1.Note ------------------------------------------
		for (uint8_t i = 0; i < 16; i++) {
			int x = 1+i;
			SeqNote1Buf[i] = data[x].toInt();
		}
		for (uint8_t i = 0; i < 16; i++) {
			int x = 17+i;
			SeqNoteBufStatus[i] = data[x].toInt();
		}
		
		SEQbpmValue = data[33].toInt();
		SEQdivValue = data[34].toFloat();
		float bpm = (SEQbpmValue / SEQdivValue);
		SEQclkRate = (60000000 / bpm);
		SEQstepNumbers = data[35].toInt();
		SEQGateTime = data[36].toFloat();
		if (SEQGateTime <= 1.0f || SEQGateTime >= 16.0f) {
			SEQGateTime = (3.94f); // Pot Value 32
		}
		Interval = SEQclkRate;
		gateTime = (float)(SEQclkRate / SEQGateTime);
		SEQdirection = data[37].toInt();
		SEQdirectionFlag = false;
		// Sequencer Velocity data ----------------------------------------
		for (uint8_t i = 0; i < 16; i++) {
			int x = 38+i;
			SeqVeloBuf[i] = data[x].toInt();
		}
		// Sequencer Step 2.Note 3.Note 4.Note ----------------------------
		for (uint8_t i = 0; i < 48; i++) {
			int x = 54+i;
			int addr = 16 + i;
			SeqNote1Buf[addr] = data[x].toInt();
		}
		// Sequencer Step note count --------------------------------------
		for (uint8_t i = 0; i < 16; i++) {
			int x = 102+i;
			SeqNoteCount[i] = data[x].toInt();
		}
	}
}

//*************************************************************************
// recall (load) Sequencer Pattern
//*************************************************************************
FLASHMEM void recallPattern(int patternNo) {
	
	if (SD.exists("SEQ/")) {
		//Serial.println(F("SEQ-Folder ok"));
	}
	else {
		Serial.println(F("SEQ-Folder Error!"));
		SDErrorFlag = 1;
	}
	String numString = (patternNo);
	String fileString = ("SEQ/" + numString);
	//Serial.print(fileString);
	File patternFile = SD.open(fileString.c_str());
	// Patch unavailable
	if (!patternFile) {
		patternFile.close();
		//Serial.println(F("Pattern not found"));
		// load init Patch from Flash
		initPatternData();
	}
	else {
		String data[NO_OF_SEQ_PARAMS]; //Array of data read in
		recallPatternData(patternFile, data);
		setCurrentPatternData(data);
		patternFile.close();
	}
}

