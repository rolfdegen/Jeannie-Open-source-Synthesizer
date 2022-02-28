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
#define   CCfilterres 71
#define   CCamprelease  72
#define   CCampattack 73
#define   CCfilterfreq  74
#define   CCampdecay  75
#define   CCfilterlforate  76
#define   CCfilterlfoamt 77
#define   CCampsustain  79
#define   CCfilterattack 80
#define   CCfilterdecay  81
#define   CCfiltersustain  82
#define   CCfilterrelease  83
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
//#define   CCbankSelect 179
#define   CCprgChange 180
#define   CCBankSelect16 181
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






// Midi Controller Messages (only 0 - 127)
const static uint8_t PROGMEM MidiCCTabel[128] = {
	188,	// CC 0
	1,		// CC 1		Modulation Wheel
	255,	// CC 2
	3,		// CC 3		Pitch LFO Amount
	255,	// CC 4
	5,		// CC 5		Glide
	255,	// CC 6
	153,	// CC 7		Oscillator 1+2 Level
	150,	// CC 8		OscMix
	9,		// CC 9		Osc1 WaveBank
	10,		// CC 10	Osc2 WaveBank
	255,	// CC 11
	255,	// CC 12
	255,	// CC 13
	14,		// CC 14	Osc 1 Waveform
	15,		// CC 15	Osc 2 Waveform
	16,		// CC 16	Filter Envelope Amount
	255,	// CC 17	
	255,	// CC 18	
	19,		// CC 19	Filter Type
	255,	// CC 20	
	255,	// CC 21
	255,	// CC 22
	23,		// CC 23	Noise Level (0 with, 63 off, 127 pink)
	24,	// CC 24	oscMOD
	255,	// CC 25	
	26,		// CC 26	Osc 1 Pitch
	27,		// CC 27	Osc 2 Pitch
	151,	// CC 28	Osc 1 Pitch Envelope
	152,	// CC 29	Osc 2 Pitch Envelope
	30,		// CC 30	Osc LFO SYNC (Retrig)
	31,		// CC 31	Filter LFO SYNC (Retrig)
	181,	// CC 32	Bank Select
	180,	// CC 33	Prg Change
	255,	// CC 34
	255,	// CC 35
	255,	// CC 36
	255,	// CC 37
	255,	// CC 38
	255,	// CC 39
	255,	// CC 40
	255,	// CC 41
	255,	// CC 42
	255,	// CC 43
	255,	// CC 44
	255,	// CC 45
	255,	// CC 46
	255,	// CC 47
	255,	// CC 48
	255,	// CC 49
	255,	// CC 50		
	255,	// CC 51
	255,	// CC 52
	255,	// CC 53
	255,	// CC 54
	255,	// CC 55
	255,	// CC 56
	255,	// CC 57
	255,	// CC 58
	255,	// CC 59
	255,	// CC 60
	255,	// CC 61
	255,	// CC 62
	255,	// CC 63
	255,	// CC 64
	255,	// CC 65
	255,	// CC 66
	255,	// CC 67
	255,	// CC 68
	255,	// CC 69
	255,	// CC 70
	71,		// CC 71	Filter Resonance
	72,		// CC 72	AMP Release Time
	73,		// CC 73	AMP Attack Time
	74,		// CC 74	Filter Cutoff
	75,		// CC 75	AMP Decay Time
	76,		// CC 76	Filter LFO Rate
	77,		// CC 77	Filter LFO Amount
	255,	// CC 78
	79,		// CC 79	AMP Sustain Level
	80,		// CC 80	Filter Attack Time
	81,		// CC 81	Filter Decay Time
	82,		// CC 82	Filter Sustain Time
	83,		// CC 83	Filter Release Time
	255,	// CC 84
	85,		// CC 85	Osc1 PWAMT	(Pulse width)
	86,		// CC 86	Osc2 PWAMT	(Pulse width)
	87,		// CC 87	PWM Rate
	255,	// CC 88
	89,		// CC 89	Key Tracking
	90,		// CC 90	Filter LFO Waveform
	255,	// CC 91
	255,	// CC 92
	255,	// CC 93
	94,		// CC 94	Detune
	255,	// CC 95
	255,	// CC 96
	255,	// CC 97
	255,	// CC 98
	255,	// CC 99
	141,	// CC 100	LFO1 Shape
	142,	// CC 101	LFO1 Rate
	143,	// CC 102	LFO1 Amt 
	255,	// CC 103
	144,	// CC 104	LFO2 Shape
	145,	// CC 105	LFO2 Rate
	146,	// CC 106	LFO2 Amt
	255,	// CC 107
	182,	// CC 108	Fx P1
	183,	// CC 109	Fx P2
	184,	// CC 110	Fx P3
	185,	// CC 111	Fx CLK
	186,	// CC 112	Fx MIX
	187,	// CC 113	Fx PRG
	255,	// CC 114
	255,	// CC 115
	255,	// CC 116
	255,	// CC 117
	255,	// CC 118
	255,	// CC 119
	255,	// CC 120
	255,	// CC 121
	255,	// CC 122
	123,	// CC 123	All Notes Off
	255,	// CC 124
	255,	// CC 125
	126,	// CC 126	Unisono (o Off, >0 On)
	255		// CC 127
};