// Voice.h


//*************************************************************************
// Prototyps
//*************************************************************************
void myNoteOn(byte channel, byte note, byte velocity);
void myNoteOff(byte channel, byte note, byte velocity);
void handlePolyphonicNoteOn (uint8_t note, uint8_t velocity);
void handleUnisonNoteOn (uint8_t note, uint8_t velocity);
void handleMonophonicNoteOn (uint8_t note, uint8_t velocity);
void handleChordNoteOn (uint8_t note, uint8_t velocity);
void handlePolyphonicNoteOff (uint8_t note, uint8_t velocity);
void handleUnisonNoteOff (uint8_t note, uint8_t velocity);
void handleMonophonicNoteOff (uint8_t note, uint8_t velocity);
void handleChordNoteOff (uint8_t note, uint8_t velocity);
void set_LFO_sync (void);
void endVoice(int voice);
void Rec_mode (uint8_t note, uint8_t velocity);
uint8_t Trans_mode(uint8_t note);
void myNoteOn2(byte channel, byte note, byte velocity);
void allNotesOff(void);
int getVoiceNo(int note);
void updateVoice(uint8_t voice_no, uint8_t notesOn, uint8_t note);
void get_play_voice(byte note, byte velocity);
void monoNotesOn(byte note, byte velocity, byte notesOn);
void allNotesOn(byte note, byte velocity);
void incNotesOn(void);
void decNotesOn(void);
void updatesAllVoices(void);
void get_glideSpeed(int note, int voice);
void play_voices(int voice, byte note, byte velocity, float level);
void play_monoVoices(int voice, byte note, byte velocity, float level, float pan_a, float pan_b);
void update_panorama(void);
void SequencerRecNotes (uint8_t note, uint8_t velo);


//*************************************************************************
// NoteOn from serial Midi
//*************************************************************************
void myNoteOn(byte channel, byte note, byte velocity) {
	
	current_velocity = velocity;	// velocity need into VCA Pan function

	// Prevent notes hanger -----------------------------------------------
	for (int i = 0; i < NO_OF_VOICES; i++) {
		if (voices[i].note == note && voices[i].voiceOn == 1) {
			endVoice(i + 1);
		}
	}
	
	// if Transpose mode --------------------------------------------------
	if (SEQmode == 1 && SEQrunStatus == true) {
		for (uint8_t i = 0; i < SEQstepNumbers; i++) { 
			if (SeqNoteBufStatus[i] == 1) {
				SeqTranspose = note - SeqNote1Buf[i]; // transpose midi note in buffer
				return;
			}
		}
		SeqTranspose = 0;
		return;
	}
	
	// if Recording mode --------------------------------------------------
	if (SEQmode == 2) {
		Rec_mode(note, velocity);
	}
		
	// if Poly Mode -------------------------------------------------------
	if (unison == 0) {
		handlePolyphonicNoteOn(note, velocity);
		return;
	}
	
	// if Mono Mode -------------------------------------------------------
	if (unison == 1 && Voice_mode <= 4) {
		if (SEQrunStatus == true) {
			return;
		}
		handleMonophonicNoteOn(note, velocity);
		return;
	}
	
	// if Chord Mode ------------------------------------------------------
	if (unison == 1 && Voice_mode == 5) {
		if (SEQrunStatus == true) {
			return;
		}
		handleChordNoteOn(note, velocity);
		return;
	}
}

//*************************************************************************
// NoteOff from serial Midi
//*************************************************************************
void myNoteOff(byte channel, byte note, byte velocity) {

	// Hold mode ----------------------------------------------------------
	if (Midi_hold_flag == true)
	{
		return;
	}
	
	// Poly Mode ----------------------------------------------------------
	if (unison == 0) {
		// note off
		handlePolyphonicNoteOff(note, velocity);
		return;
	}

	// Mono Mode ----------------------------------------------------------
	if (unison == 1 && (Voice_mode <= 4)) {
		// all notes off
		if (voices[0].voiceOn == 1) {
			handleMonophonicNoteOff(note, velocity);
		}
		return;
	}
	
	// Chord Mode ---------------------------------------------------------
	if (unison == 1 && Voice_mode == 5) {
		// all notes off
		if (voices[0].voiceOn == 1) {
			handleChordNoteOff(note, velocity);
		}
		return;
	}
}

//*************************************************************************
// get KeyTracking
//*************************************************************************
void get_Keytraking(int note, int voice) {
	note = note + SeqTranspose;
	if (note >= 127) {
		note = 127;
	}
	keytrackingValue = KEYTRACKINGAMT[note] * keytrackingAmount;
	filterModMixer_a[voice].gain(2, keytrackingValue);
}


//*************************************************************************
// NoteOn handle
//*************************************************************************

// Poly mode NoteOn handle ------------------------------------------------
void handlePolyphonicNoteOn (uint8_t note, uint8_t velocity) {
	
	// Play Midi note
	get_play_voice(note, velocity);
}

// Mono mode NoteOn handle ------------------------------------------------
void handleMonophonicNoteOn (uint8_t note, uint8_t velocity) {
	
	if (NoteStack_ptr < NoteStack_size || NoteStack_ptr == 255) {
		NoteStack_pool[NoteStack_ptr] = note;
		NoteStack_ptr++;
		
		// Play midi note
		uint8_t notesOn = Voice_mode + 1;
		if (Voice_mode == 4) {	// 6 voice
			notesOn = 6;
		}
		
		if (NoteStack_ptr == 1) {
			monoNotesOn(note, velocity, notesOn);
			prevNote = note;
		} else {
			voices[0].note = note;
			voices[0].timeOn = millis();
			
			// set Voices lamp
			for (uint8_t i = 0; i < notesOn; i++) {
				voices[i].voiceOn = 1;
			}
			
			for (uint8_t i = 0; i < notesOn; i++) {
				get_Keytraking(note, i);
				get_glideSpeed(note, i);
			}
			updateVoice(0, notesOn, note);
			prevNote = note;
		}
	}
	//prevNote = note;
	set_LFO_sync();
	
	// Recording note into Sequencer
	if (SEQmode == 2) {
		SequencerRecNotes(note, velocity);
	}
}

// Chord mode NoteOn handle -----------------------------------------------
FLASHMEM void handleChordNoteOn (uint8_t note, uint8_t velocity) {
	
	allNotesOn(note, velocity);
	
	if (NoteStack_ptr < NoteStack_size || NoteStack_ptr == 255) {
		NoteStack_pool[NoteStack_ptr] = note;
		NoteStack_ptr++;
		
		uint8_t notesOn = 1;
		
		if (NoteStack_ptr == 1) {
			allNotesOn(note, velocity);
			} else {
			voices[0].note = note;
			voices[0].timeOn = millis();
			
			// set Voices lamp
			for (uint8_t i = 0; i < 8; i++) {
				voices[i].voiceOn = 1;
			}
			updateVoice(0, notesOn, note);
		}
	}
	prevNote = note;
	set_LFO_sync();
	
	// Recording note into Sequencer
	if (SEQmode == 2) {
		SequencerRecNotes(note, velocity);
	}
}

//*************************************************************************
// NoteOff handle
//*************************************************************************

// Poly mode NoteOff handle -----------------------------------------------
void handlePolyphonicNoteOff (uint8_t note, uint8_t velocity) {

	endVoice(getVoiceNo(note));
	
	// NoteOff in Sequencer Recording mode
	if (SEQmode == 2 && unison == 0) {
		if (SeqRecNoteCount > 0){
			SeqRecNoteCount--;
			if (SeqRecNoteCount == 0) {
				SequencerRecNotes(note, velocity);
			}
		}
	}
}

// Mono mode NoteOff handle -----------------------------------------------
FLASHMEM void handleMonophonicNoteOff (uint8_t note, uint8_t velocity) {
	
	uint8_t last;
	
	// NoteOff in Sequencer Recording mode
	if (SEQmode == 2 && unison == 1) {
		if (SeqRecNoteCount > 0){
			SeqRecNoteCount--;
			if (SeqRecNoteCount == 0) {
				SequencerRecNotes(note, velocity);
			}
		}
	}

	for (uint8_t i = 0; i < NoteStack_size; i++) {
		if (NoteStack_pool[i] == note) {
			
			// clear midi note into NoteStack_pool
			NoteStack_pool[i] = 0xFF;
			NoteStack_ptr--;
			
			if (NoteStack_ptr < 0) {
				NoteStack_ptr = 0;
			}
			last = i;

			// sort midi note into NoteStack_pool
			for (; last < NoteStack_size; last++) {
				NoteStack_pool[last] = NoteStack_pool[last + 1];
			}
			
			// Play midi note
			uint8_t notesOn = Voice_mode + 1;
			if (Voice_mode == 4) {	// 6 voice
				notesOn = 6;
			}
			
			// play previous midi note
			if (NoteStack_ptr > 0) {
				last = NoteStack_ptr - 1;
				note = NoteStack_pool[last];
				voices[0].note = note;
				voices[0].timeOn = millis();
				voices[0].voiceOn = 1;
				
				for (uint8_t i = 0; i < notesOn; i++) {
					get_Keytraking(note, i);
					get_glideSpeed(note, i);
				}
				updateVoice(0, notesOn, note);
				prevNote = note;
				
				break;
			}
			// note off
			else {
				for (uint8_t i = 0; i < notesOn; i++) {
					endVoice(i+1);
				}
			}
		}
	}
}

// Chord mode NoteOff handle ----------------------------------------------
FLASHMEM void handleChordNoteOff (uint8_t note, uint8_t velocity) {
	
	uint8_t last;
	
	// if sequencer in record mode
	if (SEQmode == 2 && unison == 1) {
		SeqRecNoteCount = 0;
		SequencerRecNotes(note, velocity);
	}

	for (uint8_t i = 0; i < NoteStack_size; i++) {
		if (NoteStack_pool[i] == note) {
			
			// clear midi note into NoteStack_pool
			NoteStack_pool[i] = 0xFF;
			NoteStack_ptr--;
			
			if (NoteStack_ptr < 0) {
				NoteStack_ptr = 0;
			}
			last = i;

			// sort midi note into NoteStack_pool
			for (; last < NoteStack_size; last++) {
				NoteStack_pool[last] = NoteStack_pool[last + 1];
			}
			
			
			uint8_t notesOn = 8;
			
			// play previous midi note
			if (NoteStack_ptr > 0) {
				last = NoteStack_ptr - 1;
				note = NoteStack_pool[last];
				voices[0].note = note;
				voices[0].timeOn = millis();
				voices[0].voiceOn = 1;
				updateVoice(0, notesOn, note);
				for (uint8_t i = 0; i < 8; i++) {
					get_Keytraking(note, i);
				}
				break;
			}
			// note off
			else {
				for (uint8_t i = 0; i < notesOn; i++) {
					endVoice(i+1);
				}
			}
		}
	}
	
}

//*************************************************************************
// Sync LFOs
//*************************************************************************
FLASHMEM void set_LFO_sync () {
	// LFO1 sync -----------------------------------------------------
	if (oscLfoRetrig > 0) {
		pitchLfo.sync();
		pitchLfo.phase(0.0f);
	}
	
	// LFO2 sync ----------------------------------------------------
	if (filterLfoRetrig > 0) {
		filterLfo.sync();
		filterLfo.phase(0.0f);
	}
	
	// LFO3 sync ----------------------------------------------------
	if (Lfo3Retrig > 0) {
		ModLfo3.sync();
		ModLfo3.phase(0.0f);
	}
	
	// update LFO randomFlag
	LFO1randomFlag = false;
	LFO2randomFlag = false;
	LFO3randomFlag = false;
	
	// activate a few things ----------------------------------------------
	MidiStatusSymbol = 1;	// draw Midi IN symbol
	EnvIdelFlag = true;		// enabled Oscilloscope
	voiceLEDflag = true;	// draw Voice indicator
}

//*************************************************************************
// endVoice
//*************************************************************************
void endVoice(int voice) {
	
	// more than 8 notes ?
	if (voice < 1) {
		return;
	}
	
	int Voice = voice -1;
	filterEnvelope[Voice].noteOff();
	ampEnvelope[Voice].noteOff();
	LFO1Envelope[Voice].noteOff();
	LFO2Envelope[Voice].noteOff();
	voices[Voice].voiceOn = 0;
	VoicesLEDtime[Voice] = VoicLEDtime;
	
	// noteOff LFO3 envelope
	boolean noteActiv = false;
	for (uint8_t i = 0; i < 8; i++) {
		if (voices[i].voiceOn == 1)
		{
			noteActiv = true;
		}
	}
	if (noteActiv == false) {
		LFO3EnvelopeAmp.noteOff();
	}
}

//*************************************************************************
// Note On from Sequencer
//*************************************************************************
FLASHMEM void myNoteOn2(byte channel, byte note, byte velocity) {
	
	if (SeqRecNoteCount <= 4)SeqRecNoteCount++;
	
	// Unisono Mode 0 -----------------------------------------------------
	if (unison == 0) {
		handlePolyphonicNoteOn(note, velocity);
	}
	else if (unison == 1 && (Voice_mode <= 4)) {
		handleMonophonicNoteOn(note, velocity);
	}
	else if (unison == 1 && Voice_mode == 5) {
		handleChordNoteOn(note, velocity);
	}
	
	// Pitch LFO sync -----------------------------------------------------
	if (oscLfoRetrig == 1) {
		pitchLfo.sync();
		float pha = LFO1phase;
		pitchLfo.phase(pha);
	}
	
	// Filter LFO sync ----------------------------------------------------
	if (filterLfoRetrig == 1 || filterLfoRetrig == 2) {
		filterLfo.sync();
		float pha = LFO2phase;
		filterLfo.phase(pha);
	}

	MidiStatusSymbol = 1;
	EnvIdelFlag = true; // oscilloscope enabled
	//drawVoiceLED ();
	
	// Sequencer Recording notes
	SequencerRecNotes(note, velocity);
	
	// update LFO randomFlag
	LFO1randomFlag = false;
	LFO2randomFlag = false;
	LFO3randomFlag = false;
}


//*************************************************************************
// set all Notes Off
//*************************************************************************
FLASHMEM void allNotesOff() {
	notesOn = 0;
	for (int v = 0; v < NO_OF_VOICES; v++) {
		endVoice(v + 1);
	}
}

//*************************************************************************
// Play poly voices
//*************************************************************************
void play_voices(int voice, byte note, byte velocity, float level) {
	get_Keytraking(note, voice);
	float velo = VELOCITY[velocitySens][velocity];
	float amp = ((0.5 - myAmpVelocity) + (velo * myAmpVelocity)) * level;
	amp = amp * 2;
	if (voice <= 3) {
		float pan_factor = pan_value * (0.004f / (voice + 1));
		voiceMixer1a.gain(voice, amp * (0.5f + pan_factor));
		voiceMixer2a.gain(voice, amp * (0.5f - pan_factor));
	}
	else {
		float pan_factor = pan_value * (0.001f * (voice - 3));
		voiceMixer1b.gain(voice - 4, amp * (0.5f - pan_factor));
		voiceMixer2b.gain(voice - 4, amp * (0.5f + pan_factor));
	}
	filterModMixer_a[voice].gain(3, (velo * myFilVelocity));	// Filter Velocity
	filterEnvelope[voice].noteOn();
	ampEnvelope[voice].noteOn();
	LFO1Envelope[voice].noteOn();
	LFO2Envelope[voice].noteOn();
	LFO3EnvelopeAmp.noteOn();
	get_glideSpeed(note, voice);
}

//*************************************************************************
// Play mono voices
//*************************************************************************
FLASHMEM void play_monoVoices(int voice, byte note, byte velocity, float level, float pan_a, float pan_b) {
	get_Keytraking(note, voice);
	float velo = VELOCITY[velocitySens][velocity];
	float amp = ((0.5 - myAmpVelocity) + (velo * myAmpVelocity)) * level;
	amp = amp * 2;
	if (voice <= 3) {
		voiceMixer1a.gain(voice, amp * pan_a);
		voiceMixer2a.gain(voice, amp * pan_b);
	}
	else {
		voiceMixer1b.gain(voice - 4, amp * pan_a);
		voiceMixer2b.gain(voice - 4, amp * pan_b);
	}
	filterModMixer_a[voice].gain(3, (velo * myFilVelocity));	// Filter Velocity
	filterEnvelope[voice].noteOn();
	ampEnvelope[voice].noteOn();
	LFO1Envelope[voice].noteOn();
	LFO2Envelope[voice].noteOn();
	LFO3EnvelopeAmp.noteOn();	
}

//*************************************************************************
// set Osc glide 
//*************************************************************************
void get_glideSpeed(int note, int voice) {
	if (glideSpeed > 0 && note != prevNote) {
		glide[voice].amplitude((prevNote - note) * DIV24);   // Set glide to previous note frequency (limited to 1 octave max)
		glide[voice].amplitude(0, glideSpeed * GLIDEFACTOR); // Glide to current note
	}
}

//*************************************************************************
// get VoiceNo
//*************************************************************************
int getVoiceNo(int note) {

	voiceToReturn = -1;      //Initialise
	earliestTime = millis(); //Initialise to now
	if (note == -1) {
		//NoteOn() - Get the oldest free voice (recent voices may be still on release stage)
		for (int i = 0; i < NO_OF_VOICES; i++) {
			if (voices[i].voiceOn == 0) {
				if (voices[i].timeOn < earliestTime) {
					earliestTime = voices[i].timeOn;
					voiceToReturn = i;
				}
			}
		}
		if (voiceToReturn == -1) {
			//No free voices, need to steal oldest sounding voice
			earliestTime = millis(); //Reinitialise
			for (int i = 0; i < NO_OF_VOICES; i++) {
				if (voices[i].timeOn < earliestTime) {
					earliestTime = voices[i].timeOn;
					voiceToReturn = i;
				}
			}
		}
		return voiceToReturn + 1;
		} else {
		//NoteOff() - Get voice number from note
		for (int i = 0; i < NO_OF_VOICES; i++) {
			if (voices[i].note == note && voices[i].voiceOn == 1) {
				return i + 1; // voice number
			}
		}
		//Unison - Note on without previous note off?
		return voiceToReturn;
	}
	//Shouldn't get here, return voice 1
	return 1;
}

//*************************************************************************
// update Voices Pitch
//*************************************************************************
void updateVoice(uint8_t voice_no, uint8_t notesOn, uint8_t note)
{

	int pitchNotea = oscPitchA + oscTranspose + SeqTranspose;
	int pitchNoteb = oscPitchB + oscTranspose + SeqTranspose;

	// Poly mode
	if (unison == 0)
	{
		if (Osc1WaveBank >= 15) // original Braids vowel -12 semitons
		{
			waveformModa[voice_no].frequency(NOTEFREQS[voices[voice_no].note + pitchNotea] * oscMasterTune * 0.5f);
			for (size_t i = 0; i < 8; i++)
		{
			waveformModa[voice_no].Osc_pitch_note(note);
		}
		}
		else
			waveformModa[voice_no].frequency(NOTEFREQS[voices[voice_no].note + pitchNotea] * oscMasterTune);

		if (Osc2WaveBank >= 15)
		{
			waveformModb[voice_no].frequency((NOTEFREQS[voices[voice_no].note + pitchNoteb] * detune) * oscMasterTune * 0.5f);
		}
		else
			waveformModb[voice_no].frequency((NOTEFREQS[voices[voice_no].note + pitchNoteb] * detune) * oscMasterTune);
	}

	// Unison/Mono mode
	else if (unison == 1 && Voice_mode <= 4)
	{
		switch (notesOn)
		{
		case 1:
			waveformModa[0].frequency(NOTEFREQS[note + pitchNotea] * oscMasterTune);
			waveformModb[0].frequency((NOTEFREQS[note + pitchNoteb] * (detune + ((1 - detune) * 0.04f))) * oscMasterTune);
			break;
		case 2:
			waveformModa[0].frequency((NOTEFREQS[note + pitchNotea] * (detune + ((1 - detune) * 0.09f))) * oscMasterTune);
			waveformModb[0].frequency((NOTEFREQS[note + pitchNoteb] * (detune + ((1 - detune) * 0.14f))) * oscMasterTune);
			waveformModa[1].frequency((NOTEFREQS[note + pitchNotea] * (1 + (1 - (detune + ((1 - detune) * 0.09f))))) * oscMasterTune);
			waveformModb[1].frequency((NOTEFREQS[note + pitchNoteb] * (1 + (1 - (detune + ((1 - detune) * 0.14f))))) * oscMasterTune);
			break;
		case 3:
			waveformModa[0].frequency(NOTEFREQS[note + pitchNotea] * oscMasterTune);
			waveformModb[0].frequency((NOTEFREQS[note + pitchNoteb] * (detune + ((1 - detune) * 0.04f))) * oscMasterTune);
			waveformModa[1].frequency((NOTEFREQS[note + pitchNotea] * (1 + (1 - (detune + ((1 - detune) * 0.09f))))) * oscMasterTune);
			waveformModb[1].frequency((NOTEFREQS[note + pitchNoteb] * (1 + (1 - (detune + ((1 - detune) * 0.14f))))) * oscMasterTune);
			waveformModa[2].frequency((NOTEFREQS[note + pitchNotea] * (detune + ((1 - detune) * 0.09f))) * oscMasterTune);
			waveformModb[2].frequency((NOTEFREQS[note + pitchNoteb] * (detune + ((1 - detune) * 0.14f))) * oscMasterTune);
			break;
		case 4:
			waveformModa[0].frequency((NOTEFREQS[note + pitchNotea] * (detune + ((1 - detune) * 0.09f))) * oscMasterTune);
			waveformModb[0].frequency((NOTEFREQS[note + pitchNoteb] * (detune + ((1 - detune) * 0.14f))) * oscMasterTune);
			waveformModa[1].frequency((NOTEFREQS[note + pitchNotea] * (detune + ((1 - detune) * 0.70f))) * oscMasterTune);
			waveformModb[1].frequency((NOTEFREQS[note + pitchNoteb] * (detune + ((1 - detune) * 0.74f))) * oscMasterTune);
			waveformModa[2].frequency((NOTEFREQS[note + pitchNotea] * (1 + (1 - (detune + ((1 - detune) * 0.09f))))) * oscMasterTune);
			waveformModb[2].frequency((NOTEFREQS[note + pitchNoteb] * (1 + (1 - (detune + ((1 - detune) * 0.14f))))) * oscMasterTune);
			waveformModa[3].frequency((NOTEFREQS[note + pitchNotea] * (1 + (1 - (detune + ((1 - detune) * 0.70f))))) * oscMasterTune);
			waveformModb[3].frequency((NOTEFREQS[note + pitchNoteb] * (1 + (1 - (detune + ((1 - detune) * 0.74f))))) * oscMasterTune);
			break;
		case 6:
			waveformModa[0].frequency((NOTEFREQS[note + pitchNotea] * (detune + ((1 - detune) * 0.019f))) * oscMasterTune);
			waveformModb[0].frequency((NOTEFREQS[note + pitchNoteb] * (detune + ((1 - detune) * 0.024f))) * oscMasterTune);
			waveformModa[1].frequency((NOTEFREQS[note + pitchNotea] * (detune + ((1 - detune) * 0.41f))) * oscMasterTune);
			waveformModb[1].frequency((NOTEFREQS[note + pitchNoteb] * (detune + ((1 - detune) * 0.45f))) * oscMasterTune);
			waveformModa[2].frequency((NOTEFREQS[note + pitchNotea] * (detune + ((1 - detune) * 0.77f))) * oscMasterTune);
			waveformModb[2].frequency((NOTEFREQS[note + pitchNoteb] * (detune + ((1 - detune) * 0.81f))) * oscMasterTune);
			waveformModa[3].frequency((NOTEFREQS[note + pitchNotea] * (1 + (1 - (detune + ((1 - detune) * 0.76f))))) * oscMasterTune);
			waveformModb[3].frequency((NOTEFREQS[note + pitchNoteb] * (1 + (1 - (detune + ((1 - detune) * 0.80f))))) * oscMasterTune);
			waveformModa[4].frequency((NOTEFREQS[note + pitchNotea] * (1 + (1 - (detune + ((1 - detune) * 0.40f))))) * oscMasterTune);
			waveformModb[4].frequency((NOTEFREQS[note + pitchNoteb] * (1 + (1 - (detune + ((1 - detune) * 0.44f))))) * oscMasterTune);
			waveformModa[5].frequency((NOTEFREQS[note + pitchNotea] * (1 + (1 - (detune + ((1 - detune) * 0.02f))))) * oscMasterTune);
			waveformModb[5].frequency((NOTEFREQS[note + pitchNoteb] * (1 + (1 - (detune + ((1 - detune) * 0.025f))))) * oscMasterTune);
			break;
		}
	}

	// Chord mode
	else if (unison == 1 && Voice_mode == 5)
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (Osc1WaveBank >= 15) // original Braids vowel -12 semitons
			{
				waveformModa[i].frequency((NOTEFREQS[note + pitchNotea + CHORD_DETUNE[i][chordDetune]]) * oscMasterTune * 0.5f);
			}
			else waveformModa[i].frequency((NOTEFREQS[note + pitchNotea + CHORD_DETUNE[i][chordDetune]]) * oscMasterTune);
			
			if (Osc2WaveBank >= 15)
			{
				waveformModb[i].frequency((NOTEFREQS[note + pitchNoteb + CHORD_DETUNE[i][chordDetune]] * CDT_DETUNE) * oscMasterTune * 0.5f);
			}
			else waveformModb[i].frequency((NOTEFREQS[note + pitchNoteb + CHORD_DETUNE[i][chordDetune]] * CDT_DETUNE) * oscMasterTune);
		}
	}
}

//*************************************************************************
// get Voice into poly mode
//*************************************************************************
void get_play_voice(byte note, byte velocity) {
	
	if (unison == 0) {
		float gain = 1.0f;
		uint8_t voice_no = (getVoiceNo(-1)) - 1; // Voice-No: 0 - 7
		if (oscDetuneSync == true) {
			waveformModa[voice_no].sync();
			waveformModb[voice_no].sync();
		}
		voices[voice_no].note = note;
		voices[voice_no].timeOn = millis();
		voices[voice_no].voiceOn = 1;
		play_voices(voice_no, note, velocity, gain);
		updateVoice(voice_no, 0, note);
		prevNote = note;
		set_LFO_sync();
	}
	else {
		uint8_t voice_no = 0; // (getVoiceNo(-1)) - 1; // Voice-No: 0 - 7
		if (oscDetuneSync == true) {
			waveformModa[voice_no].sync();
			waveformModb[voice_no].sync();
		}
		voices[voice_no].note = note;
		voices[voice_no].timeOn = millis();
		voices[voice_no].voiceOn = 1;
		updateVoice(voice_no, 0, note);
		prevNote = note;
		set_LFO_sync();
	}
}

//*************************************************************************
// Play all notes
//*************************************************************************
FLASHMEM void allNotesOn(byte note, byte velocity) {
	
	float gain = 0.25f;
	
	if (oscDetuneSync == true) {
		for (uint8_t i = 0; i < 8; i++) {
			waveformModa[i].sync();
			waveformModb[i].sync();
		}
	}
	voices[0].note = note;
	voices[0].timeOn = millis();
	// set all voices and lamps
	for (uint8_t i = 0; i < 8; i++) {
		play_voices(i, note, velocity, gain);
		voices[i].voiceOn = 1;
	}
	updateVoice(0, note, note);
}

//*************************************************************************
// Play mono notes
//*************************************************************************
FLASHMEM void monoNotesOn(byte note, byte velocity, byte notesOn) {
	
	if (oscDetuneSync == true) {
		for (uint8_t i = 0; i < notesOn; i++) {
			waveformModa[i].sync();
			waveformModb[i].sync();
		}
	}
	voices[0].note = note;
	voices[0].timeOn = millis();
	// set panorama in mono mode	
	float gain;
	int channel = 0;
	float pan_a;
	float pan_b;
	float pan;
	// set voices and lamps
	for (uint8_t i = 0; i < notesOn; i++) {
		pan = pan_value * pan_data[notesOn-1][i];
		gain = pan_gain[notesOn - 1];
		pan_a = 0.5f + pan;	// left pan side
		pan_b = 0.5f - pan;	// right pan side
		gain = pan_gain[notesOn - 1];
		if (channel == 0) {
			play_monoVoices(i, note, velocity, gain, pan_a, pan_b);
			channel++;
			} else {
			play_monoVoices(i, note, velocity, gain, pan_b, pan_a);
			channel = 0;
		}
		voices[i].voiceOn = 1;	// set lamps
	}
	updateVoice(0, notesOn, note);
}

//*************************************************************************
//  Update all voices
//*************************************************************************
void updatesAllVoices() {
	
	if (unison == 0) {
		for (uint8_t i = 0; i < 8; i++) {
			updateVoice(i, note, 0);
		}
	}
	else if (unison == 1) {
		updateVoice(0, Voice_mode + 1, voices[0].note);
	}
}

//*************************************************************************
// update panorama value
//*************************************************************************
void update_panorama(void) {
	
	float gain;
	float pan_a;
	float pan_b;
	float pan;
	float velo;
	float amp;
	
	// change Panorama into Mono mode -------------------------------------
	if (unison == 1 && Voice_mode >= 4) {
		
		uint8_t notesOn = 0;
		switch (Voice_mode) {
			case 4: notesOn = 1; break;	// Mono
			case 5: notesOn = 2; break;	// Mono-2
			case 6: notesOn = 3; break;	// Mono-3
			case 7: notesOn = 4; break;	// Mono-4
			case 8: notesOn = 6; break;	// Mono-6
		}
		
		switch (notesOn) {
			//case 1:	// Mono Pan in the middle
			//break;
			
			case 2:		// Mono-2
			gain = 0.50f;
			velo = VELOCITY[velocitySens][current_velocity];
			amp = ((0.5 - myAmpVelocity) + (velo * myAmpVelocity)) * gain;
			amp = amp * 2;
			pan = pan_value * 0.003f;
			pan_a = 0.5f + pan;	// left pan side
			pan_b = 0.5f - pan;	// right pan side
			voiceMixer1a.gain(0, amp * pan_a);
			voiceMixer2a.gain(0, amp * pan_b);
			voiceMixer1a.gain(1, amp * pan_b);
			voiceMixer2a.gain(1, amp * pan_a);
			break;
			case 3:		// Mono-3
			gain = 0.38f;
			velo = VELOCITY[velocitySens][current_velocity];
			amp = ((0.5 - myAmpVelocity) + (velo * myAmpVelocity)) * gain;
			amp = amp * 2;
			pan = pan_value * 0.003f;
			pan_a = 0.5f + pan;	// left pan side
			pan_b = 0.5f - pan;	// right pan side
			voiceMixer1a.gain(0, amp * pan_a);
			voiceMixer2a.gain(0, amp * pan_b);
			voiceMixer1a.gain(1, amp * pan_b);
			voiceMixer2a.gain(1, amp * pan_a);
			pan = 0.5f;	// middle pan side
			voiceMixer1a.gain(2, amp * pan);
			voiceMixer2a.gain(2, amp * pan);
			break;
			case 4:		// Mono-4
			gain = 0.34f;
			velo = VELOCITY[velocitySens][current_velocity];
			amp = ((0.5 - myAmpVelocity) + (velo * myAmpVelocity)) * gain;
			amp = amp * 2;
			pan = pan_value * 0.003f;
			pan_a = 0.5f + pan;	// left pan side
			pan_b = 0.5f - pan;	// right pan side
			voiceMixer1a.gain(0, amp * pan_a);
			voiceMixer2a.gain(0, amp * pan_b);
			voiceMixer1a.gain(1, amp * pan_b);
			voiceMixer2a.gain(1, amp * pan_a);
			pan = pan_value * 0.0007f;
			pan_a = 0.5f + pan;	// left pan side
			pan_b = 0.5f - pan;	// right pan side
			voiceMixer1a.gain(2, amp * pan_a);
			voiceMixer2a.gain(2, amp * pan_b);
			voiceMixer1a.gain(3, amp * pan_b);
			voiceMixer2a.gain(3, amp * pan_a);
			break;
			case 6:		// Mono-6
			gain = 0.27f;
			velo = VELOCITY[velocitySens][current_velocity];
			amp = ((0.5 - myAmpVelocity) + (velo * myAmpVelocity)) * gain;
			amp = amp * 2;
			pan = pan_value * 0.003f;
			pan_a = 0.5f + pan;	// left pan side
			pan_b = 0.5f - pan;	// right pan side
			voiceMixer1a.gain(0, amp * pan_a);
			voiceMixer2a.gain(0, amp * pan_b);
			voiceMixer1a.gain(1, amp * pan_b);
			voiceMixer2a.gain(1, amp * pan_a);
			pan = pan_value * 0.0007f;
			pan_a = 0.5f + pan;	// left pan side
			pan_b = 0.5f - pan;	// right pan side
			voiceMixer1a.gain(2, amp * pan_a);
			voiceMixer2a.gain(2, amp * pan_b);
			voiceMixer1a.gain(3, amp * pan_b);
			voiceMixer2a.gain(3, amp * pan_a);
			pan = pan_value * 0.0015f;
			pan_a = 0.5f + pan;	// left pan side
			pan_b = 0.5f - pan;	// right pan side
			voiceMixer1b.gain(0, amp * pan_a);
			voiceMixer2b.gain(0, amp * pan_b);
			voiceMixer1b.gain(1, amp * pan_b);
			voiceMixer2b.gain(1, amp * pan_a);
			break;
		}
	}
}