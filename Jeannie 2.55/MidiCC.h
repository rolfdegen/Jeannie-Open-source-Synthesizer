//MIDI CC control numbers
//These broadly follow standard CC assignments
#define   CCmodwheel  1 //pitch LFO amount - less from mod wheel
#define   CCosclfoamt  3 //pitch LFO amount - panel control
#define   CCglide 5
#define   CCvolume  7
#define   CCosc1WaveBank 9
#define   CCosc2WaveBank 10
#define   CCoscwaveformA  14
#define   CCoscwaveformB  15
#define   CCfilterenv 16
#define   CCfiltermixer 19
#define   CCoscLevelA 20
#define   CCoscLevelB 21
#define   CCnoiseLevel  23
#define   CCoscfx 24 
#define   CCpitchA 26
#define   CCpitchB 27
#define   CCpitchenv 28
#define   CCosclforetrig  30//Off/On
#define   CCfilterlforetrig  31//Off/On

#define   CCfilterlforate  60
#define   CCfilterlfoamt 61
#define   CCHPFfilter 62
#define   CCHPFres 63
#define   CCsustain 64

#define   CCfilterfreq  74
#define   CCfilterres 75
#define   CCfilterattack 76
#define   CCfilterdecay  77
#define   CCfiltersustain  78
#define   CCfilterrelease  79
#define   CCampattack 80
#define   CCampdecay  81
#define   CCampsustain  82
#define   CCamprelease  83

#define   CCpwA 85
#define   CCpwB 86
#define   CCpwmRate 87
#define   CCpwmAmt  88//MIDI Only
#define   CCkeytracking 89
#define   CCfilterlfowaveform  90
#define   CCfxamt  91
#define   CCfxmix  93
#define   CCdetune  94

#define   CCoscLfoRate  102
#define   CCoscLfoWaveform  103
#define   CCfilterLFOMidiClkSync 104//Off/On
#define   CCoscLFOMidiClkSync 105//Off/On - MIDI Only
#define   CCpwmSource 106//LFO/Filter Env

#define   CCallnotesoff 123//Panic button
#define   CCunison  126//Off/On
#define   myFxPot1  127
#define   myFxPot2  128
#define   myFxPot3  129
#define	  myFxSel	130
#define	  myFxVal	131
#define   myFxMix   132 // FV-1 Mix
#define   myFxPrg   133 // FV-1 Prg
#define   myFxClkrate 134
#define   setPatchBank 135
#define   myPatchNo 136
#define   myChar 137
#define   myCursor 138
#define   myARPSEQ 139
#define   myWaveBank 140
#define   myLFO1shape 141
#define   myLFO1rate  142
#define   myLFO1amt   143
#define   myLFO2shape 144
#define   myLFO2rate  145
#define   myLFO2amt   146
#define   myLFO1syn   147
#define   myLFO2syn	  148
#define   myMidiCha   149
#define   myOscMix    150
#define	  CCpitchenvA 151
#define	  CCpitchenvB 152
#define   myDrive 153	// Oscillator Level
#define   myVelocity  154
#define   myFilterVelo 155
#define   myAmplifierVelo 156
#define   myUnisonoMode 157
#define   myWaveshaperTable 160
#define   myWaveshaperDrive 161
#define   myTranspose 162
#define   myMasterTune 163
#define   mySEQStep 164
#define   mySEQPitch 165
#define   mySEQRate 166
#define   mySEQdiv 167
#define   mySEQLen 168
#define   mySEQGateTime 169
#define   myPatternNo 170
#define   mySEQdir 171
#define   mySEQmode 172
#define   myOscSync 173
#define   myMidiSyncSwitch 174
#define   myOscVCFMOD 175
#define   myPitchWheel 176
#define   myMODWheel 177
#define   myPRGChange 178
#define   CCBankSelectMSB 179
#define   CCBankSelectLSB 180
#define   CCprgChange 181
#define   CCfxP1 182
#define   CCfxP2 183
#define   CCfxP3 184
#define   CCfxCLK 185
#define   CCfxMIX 186
#define   CCfxPRG 187
#define   myBankSelect 188
#define   CCLadderFilterDrive 189
#define   CCLadderFilterPassbandGain 190
#define   myFilterEnvCurve 191
#define   myAmpEnvCurve 192
#define   myFilterSwitch 193
#define   myLFO1mode 194
#define   myLFO2mode 195
#define   myLFO3mode 196
#define   myFilterFM 197
#define   myLFO2delay 198
#define   myLFO3delay 199
#define   myLFO1fade 200	// FADE-IN
#define   myLFO2fade 201	// FADE-IN
#define   myLFO3fade 202	// FADE-IN
#define   myLFO1fadeOut 203
#define   myLFO2fadeOut 204
#define   myLFO3fadeOut 205
#define   CCpwmRateA 206
#define   CCpwmRateB 207
#define   myOsc1ModAmt 208
#define   myLFO1envCurve 209
#define   myLFO2envCurve 210
#define   mysysexDest 211
#define   mysysexTyp 212
#define   mysysexBank 213
#define   mysysexSource 214
#define   mysysexDump 215
#define   myPickup 216
#define   myLFO3shape 217
#define   myLFO3rate  218
#define   myLFO3amt   219
#define   myLFO3syn   220
#define   myLFO3envCurve 221
#define   myAtouch 222
#define   myAtouchPitch 223
#define   myUserPot1 224
#define   myUserPot2 225
#define   myUserPot3 226
#define   myUserPot4 227
#define   mySpreadA 230
#define   mySpreadB 231
#define   mySupersawMixA 232
#define   mySupersawMixB 233
#define   myVoiceMode 234
#define   myPan_Value 235





// Midi Controller Messages (only 0 - 127)
const static uint8_t PROGMEM MidiCCTabel[128] = {
	
//    intern		Midi Ctrl #
		255,		// 0
		1,			// 1		Modulation Wheel
		255,		// 2
		255,		// 3		
		255,		// 4
		255,		// CC 5		
		255,		// CC 6
		255,		// CC 7		
		255,		// CC 8		
		255,		// CC 9	
		235,		// CC 10	Panorama
		255,		// CC 11
		255,		// CC 12
		255,		// CC 13
		9,			// CC 14	BANK (Osc1)
		10,			// CC 15	BANK (Osc2)
		255,		// CC 16
		255,		// CC 17	
		255,		// CC 18	
		255,		// CC 19
		181,		// CC 20	PrgChange
		14,			// CC 21	Osc1 Waveform
		15,			// CC 22	Osc2 Waveform
		23,			// CC 23	Noise Level (0 with, 63 off, 127 pink)
		153,		// CC 24	Oscillator 1+2 Level
		150,		// CC 25	OscMix
		26,			// CC 26	Osc 1 Pitch
		27,			// CC 27	Osc 2 Pitch
		151,		// CC 28	Osc 1 Pitch Envelope
		152,		// CC 29	Osc 2 Pitch Envelope
		5,			// CC 30	Glide
		94,			// CC 31	Osc 2 Detune
		180,		// CC 32	Bank Select (LSB)
		255,		// CC 33	
		255,		// CC 34
		255,		// CC 35
		255,		// CC 36
		255,		// CC 37
		255,		// CC 38
		255,		// CC 39
		255,		// CC 40
		255,		// CC 41
		255,		// CC 42
		255,		// CC 43
		255,		// CC 44
		255,		// CC 45
		141,		// CC 46	LFO1 SHAPE
		142,		// CC 47	LFO1 RATE
		143,		// CC 48	LFO1 AMT
		144,		// CC 49	LFO2 SHAPE
		145,		// CC 50	LFO2 RATE	
		146,		// CC 51	LFO2 AMT
		217,		// CC 52	LFO3 SHAPE
		218,		// CC 53	LFO3 RATE
		219,		// CC 54	LFO3 AMT
		255,		// CC 55
		255,		// CC 56
		255,		// CC 57
		255,		// CC 58
		255,		// CC 59
		255,		// CC 60
		255,		// CC 61
		62,			// CC 62	HPF Frequency
		63,			// CC 63	HPF Resonance
		64,			// CC 64	Sustain
		255,		// CC 65
		255,		// CC 66
		255,		// CC 67
		255,		// CC 68
		228,		// CC 69	
		229,		// CC 70	
		255,		// CC 71	
		255,		// CC 72	
		255,		// CC 73	
		74,			// CC 74	VCF Cutoff
		75,			// CC 75	VCF Resonance 
		76,			// CC 76	VCF Envelope 1 Attack
		77,			// CC 77	VCF Envelope 1 Decay
		78,			// CC 78	VCF Envelope 2 Sustain
		79,			// CC 79	VCF Envelope 1 Release
		80,			// CC 80	AMP Envelope 2 Attack
		81,			// CC 81	AMP Envelope 2 Decay
		82,			// CC 82	AMP Envelope 2 Sustain
		83,			// CC 83	AMP Envelope 2 Release
		19,			// CC 84	Filter Type (SVF/Ladder)
		85,			// CC 85	Osc1 PWAMT	(Pulse width)
		86,			// CC 86	Osc2 PWAMT	(Pulse width)
		206,		// CC 87	Osc1 PWM Rate (PWMOD)
		207,		// CC 88	Osc2 PWM Rate (PWMOD)
		89,			// CC 89	Key Tracking
		16,			// CC 90	Filter Envelope Amount
		255,		// CC 91
		255,		// CC 92
		255,		// CC 93
		255,		// CC 94	
		255,		// CC 95
		255,		// CC 96
		255,		// CC 97
		255,		// CC 98
		255,		// CC 99
		255,		// CC 100
		255,		// CC 101
		230,		// CC 102	Osc1 Spread
		231,		// CC 103	Osc2 Spread
		232,		// CC 104	Osc1 SawMix
		233,		// CC 105	Osc2 SawMix
		161,		// CC 106	WaveShaper Gain
		160,		// CC 107	WaveShaper
		182,		// CC 108	Fx P1
		183,		// CC 109	Fx P2
		184,		// CC 110	Fx P3
		185,		// CC 111	Fx CLK
		186,		// CC 112	Fx MIX
		187,		// CC 113	Fx PRG
		255,		// CC 114
		255,		// CC 115
		255,		// CC 116
		255,		// CC 117
		255,		// CC 118
		255,		// CC 119
		255,		// CC 120
		255,		// CC 121
		255,		// CC 122
		123,		// CC 123	All Notes Off
		255,		// CC 124
		255,		// CC 125
		126,		// CC 126	Unisono (0 Off, >0 On)
		255			// CC 127
};