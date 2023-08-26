// Teensy AudioPatching.h

AudioSynthWaveformDc     constant1Dc;    //xy=69.5,437
AudioSynthWaveformDc     pitchBend;      //xy=128,74
AudioSynthWaveformDc     pwa;            //xy=107,256
AudioSynthWaveformDc     pwb;            //xy=110,301
AudioSynthWaveformDc     AtouchDc;       //xy=110,302
AudioSynthWaveformDc     DC_FilterModCC; //xy=110,306
AudioSynthWaveformDc     DC_FilterModwheel; //xy=110,306


AudioSynthWaveformDc     FilterVelo1;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo2;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo3;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo4;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo5;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo6;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo7;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo8;    //xy=69.5,437

AudioSynthWaveformDc     glide[8];          //xy=124,152

AudioSynthWaveformDc     keytracking1;    //xy=176,745
AudioSynthWaveformDc     keytracking2;    //xy=196,745
AudioSynthWaveformDc     keytracking3;    //xy=216,745
AudioSynthWaveformDc     keytracking4;    //xy=236,745
AudioSynthWaveformDc     keytracking5;    //xy=216,745
AudioSynthWaveformDc     keytracking6;    //xy=236,745
AudioSynthWaveformDc     keytracking7;    //xy=176,745
AudioSynthWaveformDc     keytracking8;    //xy=196,745

AudioSynthWaveformDc     LFO3ModMixer1Dc;    //xy=69.5,437

AudioSynthWaveformModulatedTS waveformModa[8];  //xy=503,170  Osc1
AudioSynthWaveformModulatedTS waveformModb[8];  //xy=507,114  Osc2

AudioSynthNoisePink      pink;				//xy=268,349
AudioSynthNoiseWhite     white;				//xy=268,359
AudioSynthWaveformTS     pwmLfoA;			//xy=92,208
AudioSynthWaveformTS     pwmLfoB;			//xy=92,208
AudioSynthWaveformTS     pitchLfo;			//xy=122,111
AudioSynthWaveformTS     filterLfo;         //xy=166,632
AudioSynthWaveformTS     ModLfo3;			//xy=166,633

AudioEffectMultiply      multiply1;			//xy=166,634
AudioEffectMultiply      multiply2;
AudioEffectMultiply      multiplyRing[8];

AudioAmplifier           AtouchModAmp1;
AudioAmplifier           AtouchModAmp2;
AudioAmplifier           AtouchModAmp3;
AudioAmplifier           AtouchModAmp4;

AudioEffectEnvelope      LFO1Envelope[8];
AudioEffectEnvelope      LFO2Envelope[8];
AudioEffectEnvelope      LFO3EnvelopeAmp;
AudioEffectEnvelope      filterEnvelope[8];

AudioMixer4              pwMixer1a;       //xy=281,169
AudioMixer4              pwMixer1b;       //xy=287,272
AudioMixer4              pwMixer2a;         //xy=323.75,472.75
AudioMixer4              pwMixer2b;         //xy=323.75,553.75
AudioMixer4              pwMixer3a;         //xy=353.75,688.75
AudioMixer4              pwMixer3b;         //xy=358.75,763.75
AudioMixer4              pwMixer4b;         //xy=313.75,954.75
AudioMixer4              pwMixer4a;         //xy=314.75,887.75
AudioMixer4              pwMixer5a;         //xy=323.75,472.75
AudioMixer4              pwMixer5b;         //xy=323.75,553.75
AudioMixer4              pwMixer6a;         //xy=353.75,688.75
AudioMixer4              pwMixer6b;         //xy=358.75,763.75
AudioMixer4              pwMixer7a;			//xy=281,169
AudioMixer4              pwMixer7b;			//xy=287,272
AudioMixer4              pwMixer8b;         //xy=313.75,954.75
AudioMixer4              pwMixer8a;         //xy=314.75,887.75

AudioMixer4              noiseMixer;       //xy=288,369

AudioMixer4              oscGlobalModMixer1;    //xy=296,95
AudioMixer4              oscGlobalModMixer2;    //xy=296,95
AudioMixer4              oscGlobalModMixer3;    //xy=296,95
AudioMixer4              oscGlobalModMixer4;    //xy=296,95
AudioMixer4              oscGlobalModMixer5;    //xy=296,95
AudioMixer4              oscGlobalModMixer6;    //xy=296,95
AudioMixer4              oscGlobalModMixer7;    //xy=296,95
AudioMixer4              oscGlobalModMixer8;    //xy=296,95
AudioMixer4              oscModMixer1a;         //xy=550,165
AudioMixer4              oscModMixer1b;         //xy=550,195
AudioMixer4              oscModMixer2a;         //xy=550,205
AudioMixer4              oscModMixer2b;         //xy=550,225
AudioMixer4              oscModMixer3a;         //xy=550,245
AudioMixer4              oscModMixer3b;         //xy=550,265
AudioMixer4              oscModMixer4a;         //xy=550,285
AudioMixer4              oscModMixer4b;         //xy=550,305
AudioMixer4              oscModMixer5a;         //xy=550,245
AudioMixer4              oscModMixer5b;         //xy=550,265
AudioMixer4              oscModMixer6a;         //xy=550,285
AudioMixer4              oscModMixer6b;         //xy=550,305
AudioMixer4              oscModMixer7a;         //xy=550,165
AudioMixer4              oscModMixer7b;         //xy=550,195
AudioMixer4              oscModMixer8a;         //xy=550,205
AudioMixer4              oscModMixer8b;         //xy=550,225
AudioMixer4              LFO1envModMixer1;      //xy=550,225
AudioMixer4              LFO1envModMixer2;      //xy=550,225
AudioMixer4				 LFO3ModMixer1;
AudioMixer4              voiceMixer1a;
AudioMixer4              voiceMixer1b;
AudioMixer4              voiceMixer2a;
AudioMixer4              voiceMixer2b;
AudioMixer4              voiceMixerM1;
AudioMixer4              voiceMixerM2;
AudioMixer4              ScopePeakMixer;
AudioMixer4              oscFxMix1;
AudioMixer4              oscFxMix2;
AudioMixer4              oscFxMix3;
AudioMixer4              oscFxMix4;
AudioMixer4              oscFxMix5;
AudioMixer4              oscFxMix6;
AudioMixer4              oscFxMix7;
AudioMixer4              oscFxMix8;

AudioEffectDigitalCombine oscEffect[8];       //xy=663,223


AudioMixer4              waveformMixer1; //xy=824,170
AudioMixer4              waveformMixer2; //xy=829,476
AudioMixer4              waveformMixer3; //xy=846,774
AudioMixer4              waveformMixer4; //xy=854,1023
AudioMixer4              waveformMixer5; //xy=846,774
AudioMixer4              waveformMixer6; //xy=854,1023
AudioMixer4              waveformMixer7; //xy=824,170
AudioMixer4              waveformMixer8; //xy=829,476

AudioMixer4              filterModMixer_a[8];

AudioMixer4              filterModMixer1b;   //xy=845,292
AudioMixer4              filterModMixer2b;   //xy=848,637
AudioMixer4              filterModMixer3b;   //xy=852,937
AudioMixer4              filterModMixer4b;   //xy=855,1099
AudioMixer4              filterModMixer5b;   //xy=852,937
AudioMixer4              filterModMixer6b;   //xy=855,1099
AudioMixer4              filterModMixer7b;   //xy=845,292
AudioMixer4              filterModMixer8b;   //xy=848,637

AudioMixer4              filterModMixAtCC1;   //xy=848,637
AudioMixer4              filterModMixAtCC2;   //xy=848,637
AudioMixer4              filterModMixAtCC3;   //xy=848,637
AudioMixer4              filterModMixAtCC4;   //xy=848,637
AudioMixer4              filterModMixAtCC5;   //xy=848,637
AudioMixer4              filterModMixAtCC6;   //xy=848,637
AudioMixer4              filterModMixAtCC7;   //xy=848,637
AudioMixer4              filterModMixAtCC8;   //xy=848,637


AudioMixer4              LFO2mix1;
AudioMixer4              LFO2mix2;
AudioMixer4              LFO2mix3;
AudioMixer4              LFO2mix4;
AudioMixer4              LFO2mix5;
AudioMixer4              LFO2mix6;
AudioMixer4              LFO2mix7;
AudioMixer4              LFO2mix8;

AudioFilterStateVariable filter1;        //xy=1000,210
AudioFilterStateVariable filter2;        //xy=994,498
AudioFilterStateVariable filter3;        //xy=1002,822
AudioFilterStateVariable filter4;        //xy=1022,1047
AudioFilterStateVariable filter5;        //xy=1002,822
AudioFilterStateVariable filter6;        //xy=1022,1047
AudioFilterStateVariable filter7;        //xy=994,498
AudioFilterStateVariable filter8;        //xy=1000,210

AudioFilterLadder filter2_1;        //xy=1000,210
AudioFilterLadder filter2_2;        //xy=994,498
AudioFilterLadder filter2_3;        //xy=1002,822
AudioFilterLadder filter2_4;        //xy=1022,1047
AudioFilterLadder filter2_5;        //xy=1002,822
AudioFilterLadder filter2_6;        //xy=1022,1047
AudioFilterLadder filter2_7;        //xy=994,498
AudioFilterLadder filter2_8;        //xy=1000,210	

AudioFilterStateVariable AtouchFilter;		 //xy=1564,581
AudioFilterStateVariable FilterModCC;		 //xy=1564,583
AudioFilterStateVariable hpFilter1; 
AudioFilterStateVariable hpFilter2; 

AudioMixer4				filterMixer101;

AudioMixer4              filterMixer1;   //xy=1151,214
AudioMixer4              filterMixer2;   //xy=1144,504
AudioMixer4              filterMixer3;   //xy=1144,825
AudioMixer4              filterMixer4;   //xy=1155,1050
AudioMixer4              filterMixer5;   //xy=1151,214
AudioMixer4              filterMixer6;   //xy=1155,1050
AudioMixer4              filterMixer7;   //xy=1144,504
AudioMixer4              filterMixer8;   //xy=1144,825
AudioMixer4              Lfo3Mixer;			//xy=1144,825
AudioMixer4              LadderMixer1;

AudioAmplifier           WaveshaperAmp[8];		//xy=1145,825
AudioAmplifier			 osc1aFMamp;			//xy=1145,833

AudioEffectWaveshaper	 waveshaper[8];			//xy=638,301

AudioEffectEnvelope      ampEnvelope[8];


AudioAnalyzePeak		 peak;
Oscilloscope             scope;
AudioOutputI2S			 PCM5102A;

// AudioSynthWaveformDc -------------------------------------------------
AudioConnection          patchCord1(constant1Dc, filterEnvelope[0]);	// ok
AudioConnection          patchCord2(constant1Dc, filterEnvelope[1]);	// ok
AudioConnection          patchCord3(constant1Dc, filterEnvelope[2]);	// ok
AudioConnection          patchCord4(constant1Dc, filterEnvelope[3]);	// ok
AudioConnection          patchCord5(constant1Dc, filterEnvelope[4]);	// ok
AudioConnection          patchCord6(constant1Dc, filterEnvelope[5]);	// ok
AudioConnection          patchCord7(constant1Dc, filterEnvelope[6]);	// ok
AudioConnection          patchCord8(constant1Dc, filterEnvelope[7]);	// ok

AudioConnection          patchCord20(pwa, 0, pwMixer1a, 1);	// ok
AudioConnection          patchCord21(pwa, 0, pwMixer2a, 1);	// ok
AudioConnection          patchCord22(pwa, 0, pwMixer3a, 1);	// ok
AudioConnection          patchCord23(pwa, 0, pwMixer4a, 1);	// ok
AudioConnection          patchCord24(pwa, 0, pwMixer5a, 1);	// ok
AudioConnection          patchCord25(pwa, 0, pwMixer6a, 1);	// ok
AudioConnection          patchCord26(pwa, 0, pwMixer7a, 1);	// ok
AudioConnection          patchCord27(pwa, 0, pwMixer8a, 1);	// ok
AudioConnection          patchCord28(pwb, 0, pwMixer1b, 1);	// ok
AudioConnection          patchCord29(pwb, 0, pwMixer2b, 1);	// ok
AudioConnection          patchCord30(pwb, 0, pwMixer3b, 1);	// ok
AudioConnection          patchCord31(pwb, 0, pwMixer4b, 1);	// ok
AudioConnection          patchCord32(pwb, 0, pwMixer5b, 1);	// ok
AudioConnection          patchCord33(pwb, 0, pwMixer6b, 1);	// ok
AudioConnection          patchCord34(pwb, 0, pwMixer7b, 1);	// ok
AudioConnection          patchCord35(pwb, 0, pwMixer8b, 1);	// ok

AudioConnection          patchCord40(pitchBend, 0, oscGlobalModMixer1, 0);	// ok
AudioConnection          patchCord41(pitchBend, 0, oscGlobalModMixer2, 0);	// ok
AudioConnection          patchCord42(pitchBend, 0, oscGlobalModMixer3, 0);	// ok
AudioConnection          patchCord43(pitchBend, 0, oscGlobalModMixer4, 0);	// ok
AudioConnection          patchCord44(pitchBend, 0, oscGlobalModMixer5, 0);	// ok
AudioConnection          patchCord45(pitchBend, 0, oscGlobalModMixer6, 0);	// ok
AudioConnection          patchCord46(pitchBend, 0, oscGlobalModMixer7, 0);	// ok
AudioConnection          patchCord47(pitchBend, 0, oscGlobalModMixer8, 0);	// ok

AudioConnection          patchCord50(keytracking1, 0, filterModMixer_a[0], 2);
AudioConnection          patchCord51(keytracking2, 0, filterModMixer_a[1], 2);
AudioConnection          patchCord52(keytracking3, 0, filterModMixer_a[2], 2);
AudioConnection          patchCord53(keytracking4, 0, filterModMixer_a[3], 2);
AudioConnection          patchCord54(keytracking5, 0, filterModMixer_a[4], 2);
AudioConnection          patchCord55(keytracking6, 0, filterModMixer_a[5], 2);
AudioConnection          patchCord56(keytracking7, 0, filterModMixer_a[6], 2);
AudioConnection          patchCord57(keytracking8, 0, filterModMixer_a[7], 2);

AudioConnection          patchCord70(FilterVelo1, 0, filterModMixer_a[0], 3);
AudioConnection          patchCord71(FilterVelo2, 0, filterModMixer_a[1], 3);
AudioConnection          patchCord72(FilterVelo3, 0, filterModMixer_a[2], 3);
AudioConnection          patchCord73(FilterVelo4, 0, filterModMixer_a[3], 3);
AudioConnection          patchCord74(FilterVelo5, 0, filterModMixer_a[4], 3);
AudioConnection          patchCord75(FilterVelo6, 0, filterModMixer_a[5], 3);
AudioConnection          patchCord76(FilterVelo7, 0, filterModMixer_a[6], 3);
AudioConnection          patchCord77(FilterVelo8, 0, filterModMixer_a[7], 3);

AudioConnection          patchCord78(waveformModa[0], 0, filterModMixer1b, 1);	// OSC1 > Cutoff
AudioConnection          patchCord79(waveformModa[1], 0, filterModMixer2b, 1);
AudioConnection          patchCord80(waveformModa[2], 0, filterModMixer3b, 1);
AudioConnection          patchCord81(waveformModa[3], 0, filterModMixer4b, 1);
AudioConnection          patchCord82(waveformModa[4], 0, filterModMixer5b, 1);
AudioConnection          patchCord83(waveformModa[5], 0, filterModMixer6b, 1);
AudioConnection          patchCord84(waveformModa[6], 0, filterModMixer7b, 1);
AudioConnection          patchCord85(waveformModa[7], 0, filterModMixer8b, 1);

AudioConnection          patchCord1078(waveformModb[0], 0, filterModMixer1b, 2);	// OSC2 > Cutoff
AudioConnection          patchCord1079(waveformModb[1], 0, filterModMixer2b, 2);
AudioConnection          patchCord1080(waveformModb[2], 0, filterModMixer3b, 2);
AudioConnection          patchCord1081(waveformModb[3], 0, filterModMixer4b, 2);
AudioConnection          patchCord1082(waveformModb[4], 0, filterModMixer5b, 2);
AudioConnection          patchCord1083(waveformModb[5], 0, filterModMixer6b, 2);
AudioConnection          patchCord1084(waveformModb[6], 0, filterModMixer7b, 2);
AudioConnection          patchCord1085(waveformModb[7], 0, filterModMixer8b, 2);

AudioConnection          patchCord86(filterModMixer_a[0], 0, filterModMixer1b, 0);	
AudioConnection          patchCord87(filterModMixer_a[1], 0, filterModMixer2b, 0);	
AudioConnection          patchCord88(filterModMixer_a[2], 0, filterModMixer3b, 0);	
AudioConnection          patchCord89(filterModMixer_a[3], 0, filterModMixer4b, 0);	
AudioConnection          patchCord90(filterModMixer_a[4], 0, filterModMixer5b, 0);	
AudioConnection          patchCord91(filterModMixer_a[5], 0, filterModMixer6b, 0);	
AudioConnection          patchCord92(filterModMixer_a[6], 0, filterModMixer7b, 0);	
AudioConnection          patchCord93(filterModMixer_a[7], 0, filterModMixer8b, 0);	



// AudioSynthWaveformModulatedTS ----------------------------------------
AudioConnection          patchCord201(waveformModa[0], 0, waveformMixer1, 0);	// ok
AudioConnection          patchCord202(waveformModa[1], 0, waveformMixer2, 0);	// ok
AudioConnection          patchCord203(waveformModa[2], 0, waveformMixer3, 0);	// ok
AudioConnection          patchCord204(waveformModa[3], 0, waveformMixer4, 0);	// ok
AudioConnection          patchCord205(waveformModa[4], 0, waveformMixer5, 0);	// ok
AudioConnection          patchCord206(waveformModa[5], 0, waveformMixer6, 0);	// ok
AudioConnection          patchCord207(waveformModa[6], 0, waveformMixer7, 0);	// ok
AudioConnection          patchCord208(waveformModa[7], 0, waveformMixer8, 0);	// ok
AudioConnection          patchCord209(waveformModb[0], 0, waveformMixer1, 1);	// ok
AudioConnection          patchCord210(waveformModb[1], 0, waveformMixer2, 1);	// ok
AudioConnection          patchCord211(waveformModb[2], 0, waveformMixer3, 1);	// ok
AudioConnection          patchCord212(waveformModb[3], 0, waveformMixer4, 1);	// ok
AudioConnection          patchCord213(waveformModb[4], 0, waveformMixer5, 1);	// ok
AudioConnection          patchCord214(waveformModb[5], 0, waveformMixer6, 1);	// ok
AudioConnection          patchCord215(waveformModb[6], 0, waveformMixer7, 1);	// ok
AudioConnection          patchCord216(waveformModb[7], 0, waveformMixer8, 1);	// ok

AudioConnection          patchCord251(waveformModa[0], 0, oscModMixer1b, 3);	// ok
AudioConnection          patchCord252(waveformModa[1], 0, oscModMixer2b, 3);	// ok
AudioConnection          patchCord253(waveformModa[2], 0, oscModMixer3b, 3);	// ok
AudioConnection          patchCord254(waveformModa[3], 0, oscModMixer4b, 3);	// ok
AudioConnection          patchCord255(waveformModa[4], 0, oscModMixer5b, 3);	// ok
AudioConnection          patchCord256(waveformModa[5], 0, oscModMixer6b, 3);	// ok
AudioConnection          patchCord257(waveformModa[6], 0, oscModMixer7b, 3);	// ok
AudioConnection          patchCord258(waveformModa[7], 0, oscModMixer8b, 3);	// ok
AudioConnection          patchCord259(waveformModb[0], 0, oscModMixer1a, 3);	// ok
AudioConnection          patchCord260(waveformModb[1], 0, oscModMixer2a, 3);	// ok
AudioConnection          patchCord261(waveformModb[2], 0, oscModMixer3a, 3);	// ok
AudioConnection          patchCord262(waveformModb[3], 0, oscModMixer4a, 3);	// ok
AudioConnection          patchCord263(waveformModb[4], 0, oscModMixer5a, 3);	// ok
AudioConnection          patchCord264(waveformModb[5], 0, oscModMixer6a, 3);	// ok
AudioConnection          patchCord265(waveformModb[6], 0, oscModMixer7a, 3);	// ok
AudioConnection          patchCord266(waveformModb[7], 0, oscModMixer8a, 3);	// ok

AudioConnection OscEffect_connections[32] = {
	{waveformModa[0], 0, oscEffect[0], 0},
	{waveformModa[1], 0, oscEffect[1], 0},
	{waveformModa[2], 0, oscEffect[2], 0},
	{waveformModa[3], 0, oscEffect[3], 0},
	{waveformModa[4], 0, oscEffect[4], 0},
	{waveformModa[5], 0, oscEffect[5], 0},
	{waveformModa[6], 0, oscEffect[6], 0},
	{waveformModa[7], 0, oscEffect[7], 0},
	{waveformModb[0], 0, oscEffect[0], 1},
	{waveformModb[1], 0, oscEffect[1], 1},
	{waveformModb[2], 0, oscEffect[2], 1},
	{waveformModb[3], 0, oscEffect[3], 1},
	{waveformModb[4], 0, oscEffect[4], 1},
	{waveformModb[5], 0, oscEffect[5], 1},
	{waveformModb[6], 0, oscEffect[6], 1},
	{waveformModb[7], 0, oscEffect[7], 1},
	{waveformModa[0], 0, multiplyRing[0], 0},
	{waveformModa[1], 0, multiplyRing[1], 0},
	{waveformModa[2], 0, multiplyRing[2], 0},
	{waveformModa[3], 0, multiplyRing[3], 0},
	{waveformModa[4], 0, multiplyRing[4], 0},
	{waveformModa[5], 0, multiplyRing[5], 0},
	{waveformModa[6], 0, multiplyRing[6], 0},
	{waveformModa[7], 0, multiplyRing[7], 0},
	{waveformModb[0], 0, multiplyRing[0], 1},
	{waveformModb[1], 0, multiplyRing[1], 1},
	{waveformModb[2], 0, multiplyRing[2], 1},
	{waveformModb[3], 0, multiplyRing[3], 1},
	{waveformModb[4], 0, multiplyRing[4], 1},
	{waveformModb[5], 0, multiplyRing[5], 1},
	{waveformModb[6], 0, multiplyRing[6], 1},
	{waveformModb[7], 0, multiplyRing[7], 1}
};

AudioConnection OscGlide_connections[16] = {
	{glide[0], 0, oscModMixer1a, 2},
	{glide[0], 0, oscModMixer1b, 2},
	{glide[1], 0, oscModMixer2a, 2},
	{glide[1], 0, oscModMixer2b, 2},
	{glide[2], 0, oscModMixer3a, 2},
	{glide[2], 0, oscModMixer3b, 2},
	{glide[3], 0, oscModMixer4a, 2},
	{glide[3], 0, oscModMixer4b, 2},
	{glide[4], 0, oscModMixer5a, 2},
	{glide[4], 0, oscModMixer5b, 2},
	{glide[5], 0, oscModMixer6a, 2},
	{glide[5], 0, oscModMixer6b, 2},
	{glide[6], 0, oscModMixer7a, 2},
	{glide[6], 0, oscModMixer7b, 2},
	{glide[7], 0, oscModMixer8a, 2},
	{glide[7], 0, oscModMixer8b, 2}
};

// AudioSynthWaveformTS -------------------------------------------------
AudioConnection          patchCord351(pwmLfoA, 0, pwMixer1a, 0);	// ok
AudioConnection          patchCord352(pwmLfoB, 0, pwMixer1b, 0);	// ok
AudioConnection          patchCord353(pwmLfoA, 0, pwMixer2a, 0);	// ok
AudioConnection          patchCord354(pwmLfoB, 0, pwMixer2b, 0);	// ok
AudioConnection          patchCord355(pwmLfoA, 0, pwMixer3a, 0);	// ok
AudioConnection          patchCord356(pwmLfoB, 0, pwMixer3b, 0);	// ok
AudioConnection          patchCord357(pwmLfoA, 0, pwMixer4a, 0);	// ok
AudioConnection          patchCord358(pwmLfoB, 0, pwMixer4b, 0);	// ok
AudioConnection          patchCord359(pwmLfoA, 0, pwMixer5a, 0);	// ok
AudioConnection          patchCord360(pwmLfoB, 0, pwMixer5b, 0);	// ok
AudioConnection          patchCord361(pwmLfoA, 0, pwMixer6a, 0);	// ok
AudioConnection          patchCord362(pwmLfoB, 0, pwMixer6b, 0);	// ok
AudioConnection          patchCord363(pwmLfoA, 0, pwMixer7a, 0);	// ok
AudioConnection          patchCord364(pwmLfoB, 0, pwMixer7b, 0);	// ok
AudioConnection          patchCord365(pwmLfoA, 0, pwMixer8a, 0);	// ok
AudioConnection          patchCord366(pwmLfoB, 0, pwMixer8b, 0);	// ok

AudioConnection          patchCord417(LFO2mix1, 0, filterModMixer_a[0], 1);	// ok
AudioConnection          patchCord418(LFO2mix2, 0, filterModMixer_a[1], 1);	// ok
AudioConnection          patchCord419(LFO2mix3, 0, filterModMixer_a[2], 1);	// ok
AudioConnection          patchCord421(LFO2mix4, 0, filterModMixer_a[3], 1);	// ok
AudioConnection          patchCord422(LFO2mix5, 0, filterModMixer_a[4], 1);	// ok
AudioConnection          patchCord423(LFO2mix6, 0, filterModMixer_a[5], 1);	// ok
AudioConnection          patchCord424(LFO2mix7, 0, filterModMixer_a[6], 1);	// ok
AudioConnection          patchCord425(LFO2mix8, 0, filterModMixer_a[7], 1);	// ok

AudioConnection filterLfo_connections[16] = {
	{filterLfo, 0, LFO2Envelope[0], 0},
	{filterLfo, 0, LFO2Envelope[1], 0},
	{filterLfo, 0, LFO2Envelope[2], 0},
	{filterLfo, 0, LFO2Envelope[3], 0},
	{filterLfo, 0, LFO2Envelope[4], 0},
	{filterLfo, 0, LFO2Envelope[5], 0},
	{filterLfo, 0, LFO2Envelope[6], 0},
	{filterLfo, 0, LFO2Envelope[7], 0},
	{filterLfo, 0, LFO2mix1, 1},
	{filterLfo, 0, LFO2mix2, 1},
	{filterLfo, 0, LFO2mix3, 1},
	{filterLfo, 0, LFO2mix4, 1},
	{filterLfo, 0, LFO2mix5, 1},
	{filterLfo, 0, LFO2mix6, 1},
	{filterLfo, 0, LFO2mix7, 1},
	{filterLfo, 0, LFO2mix8, 1}
};

AudioConnection pitchLFO_connections[16] = {
	{pitchLfo, 0, LFO1Envelope[0], 0},
	{pitchLfo, 0, LFO1Envelope[1], 0},
	{pitchLfo, 0, LFO1Envelope[2], 0},
	{pitchLfo, 0, LFO1Envelope[3], 0},
	{pitchLfo, 0, LFO1Envelope[4], 0},
	{pitchLfo, 0, LFO1Envelope[5], 0},
	{pitchLfo, 0, LFO1Envelope[6], 0},
	{pitchLfo, 0, LFO1Envelope[7], 0},
	{pitchLfo, 0, oscGlobalModMixer1, 2},
	{pitchLfo, 0, oscGlobalModMixer2, 2},
	{pitchLfo, 0, oscGlobalModMixer3, 2},
	{pitchLfo, 0, oscGlobalModMixer4, 2},
	{pitchLfo, 0, oscGlobalModMixer5, 2},
	{pitchLfo, 0, oscGlobalModMixer6, 2},
	{pitchLfo, 0, oscGlobalModMixer7, 2},
	{pitchLfo, 0, oscGlobalModMixer8, 2}
};

AudioConnection LFO1Envelope_connections[8] = {
	{LFO1Envelope[0], 0, oscGlobalModMixer1, 1},
	{LFO1Envelope[1], 0, oscGlobalModMixer2, 1},
	{LFO1Envelope[2], 0, oscGlobalModMixer3, 1},
	{LFO1Envelope[3], 0, oscGlobalModMixer4, 1},
	{LFO1Envelope[4], 0, oscGlobalModMixer5, 1},
	{LFO1Envelope[5], 0, oscGlobalModMixer6, 1},
	{LFO1Envelope[6], 0, oscGlobalModMixer7, 1},
	{LFO1Envelope[7], 0, oscGlobalModMixer8, 1}
};

AudioConnection LFO2Envelope_connections[8] = {
	{LFO2Envelope[0], 0, LFO2mix1, 0},
	{LFO2Envelope[1], 0, LFO2mix2, 0},
	{LFO2Envelope[2], 0, LFO2mix3, 0},
	{LFO2Envelope[3], 0, LFO2mix4, 0},
	{LFO2Envelope[4], 0, LFO2mix5, 0},
	{LFO2Envelope[5], 0, LFO2mix6, 0},
	{LFO2Envelope[6], 0, LFO2mix7, 0},
	{LFO2Envelope[7], 0, LFO2mix8, 0}	
};

AudioConnection          patchCord460(pink, 0, noiseMixer, 0);	// ok
AudioConnection          patchCord461(white, 0, noiseMixer, 1);	// ok

AudioConnection filterEnvelope_connections[40] = {
	{filterEnvelope[0], 0, oscModMixer1a, 1},
	{filterEnvelope[1], 0, oscModMixer2a, 1},
	{filterEnvelope[2], 0, oscModMixer3a, 1},
	{filterEnvelope[3], 0, oscModMixer4a, 1},
	{filterEnvelope[4], 0, oscModMixer5a, 1},
	{filterEnvelope[5], 0, oscModMixer6a, 1},
	{filterEnvelope[6], 0, oscModMixer7a, 1},
	{filterEnvelope[7], 0, oscModMixer8a, 1},
	{filterEnvelope[0], 0, oscModMixer1b, 1},
	{filterEnvelope[1], 0, oscModMixer2b, 1},
	{filterEnvelope[2], 0, oscModMixer3b, 1},
	{filterEnvelope[3], 0, oscModMixer4b, 1},
	{filterEnvelope[4], 0, oscModMixer5b, 1},
	{filterEnvelope[5], 0, oscModMixer6b, 1},
	{filterEnvelope[6], 0, oscModMixer7b, 1},
	{filterEnvelope[7], 0, oscModMixer8b, 1},
	{filterEnvelope[0], 0, pwMixer1a, 2},
	{filterEnvelope[1], 0, pwMixer2a, 2},
	{filterEnvelope[2], 0, pwMixer3a, 2},
	{filterEnvelope[3], 0, pwMixer4a, 2},
	{filterEnvelope[4], 0, pwMixer5a, 2},
	{filterEnvelope[5], 0, pwMixer6a, 2},
	{filterEnvelope[6], 0, pwMixer7a, 2},
	{filterEnvelope[7], 0, pwMixer8a, 2},
	{filterEnvelope[0], 0, pwMixer1b, 2},
	{filterEnvelope[1], 0, pwMixer2b, 2},
	{filterEnvelope[2], 0, pwMixer3b, 2},
	{filterEnvelope[3], 0, pwMixer4b, 2},
	{filterEnvelope[4], 0, pwMixer5b, 2},
	{filterEnvelope[5], 0, pwMixer6b, 2},
	{filterEnvelope[6], 0, pwMixer7b, 2},
	{filterEnvelope[7], 0, pwMixer8b, 2},
	{filterEnvelope[0], 0, filterModMixer_a[0], 0},
	{filterEnvelope[1], 0, filterModMixer_a[1], 0},
	{filterEnvelope[2], 0, filterModMixer_a[2], 0},
	{filterEnvelope[3], 0, filterModMixer_a[3], 0},
	{filterEnvelope[4], 0, filterModMixer_a[4], 0},
	{filterEnvelope[5], 0, filterModMixer_a[5], 0},
	{filterEnvelope[6], 0, filterModMixer_a[6], 0},
	{filterEnvelope[7], 0, filterModMixer_a[7], 0}	
};



// AudioEffectDigitalCombine --------------------------------------------
AudioConnection          patchCord651(oscEffect[0], 0, oscFxMix1, 0);	// ok
AudioConnection          patchCord652(oscEffect[1], 0, oscFxMix2, 0);	// ok
AudioConnection          patchCord653(oscEffect[2], 0, oscFxMix3, 0);	// ok
AudioConnection          patchCord654(oscEffect[3], 0, oscFxMix4, 0);	// ok
AudioConnection          patchCord655(oscEffect[4], 0, oscFxMix5, 0);	// ok
AudioConnection          patchCord656(oscEffect[5], 0, oscFxMix6, 0);	// ok
AudioConnection          patchCord657(oscEffect[6], 0, oscFxMix7, 0);	// ok
AudioConnection          patchCord658(oscEffect[7], 0, oscFxMix8, 0);	// ok

AudioConnection          patchCord659(multiplyRing[0], 0, oscFxMix1, 1);	// ok
AudioConnection          patchCord660(multiplyRing[1], 0, oscFxMix2, 1);	// ok
AudioConnection          patchCord661(multiplyRing[2], 0, oscFxMix3, 1);	// ok
AudioConnection          patchCord662(multiplyRing[3], 0, oscFxMix4, 1);	// ok
AudioConnection          patchCord663(multiplyRing[4], 0, oscFxMix5, 1);	// ok
AudioConnection          patchCord664(multiplyRing[5], 0, oscFxMix6, 1);	// ok
AudioConnection          patchCord665(multiplyRing[6], 0, oscFxMix7, 1);	// ok
AudioConnection          patchCord666(multiplyRing[7], 0, oscFxMix8, 1);	// ok

AudioConnection          patchCord667(oscFxMix1, 0, waveformMixer1, 3);	// ok
AudioConnection          patchCord668(oscFxMix2, 0, waveformMixer2, 3);	// ok
AudioConnection          patchCord669(oscFxMix3, 0, waveformMixer3, 3);	// ok
AudioConnection          patchCord670(oscFxMix4, 0, waveformMixer4, 3);	// ok
AudioConnection          patchCord671(oscFxMix5, 0, waveformMixer5, 3);	// ok
AudioConnection          patchCord672(oscFxMix6, 0, waveformMixer6, 3);	// ok
AudioConnection          patchCord673(oscFxMix7, 0, waveformMixer7, 3);	// ok
AudioConnection          patchCord674(oscFxMix8, 0, waveformMixer8, 3);	// ok

AudioConnection StateVariableFilter_connections[40] = {
	{filter1, 0, filterMixer1, 0},
	{filter2, 0, filterMixer2, 0},
	{filter3, 0, filterMixer3, 0},
	{filter4, 0, filterMixer4, 0},
	{filter5, 0, filterMixer5, 0},
	{filter6, 0, filterMixer6, 0},
	{filter7, 0, filterMixer7, 0},
	{filter8, 0, filterMixer8, 0},
	{filter1, 1, filterMixer1, 1},
	{filter2, 1, filterMixer2, 1},
	{filter3, 1, filterMixer3, 1},
	{filter4, 1, filterMixer4, 1},
	{filter5, 1, filterMixer5, 1},
	{filter6, 1, filterMixer6, 1},
	{filter7, 1, filterMixer7, 1},
	{filter8, 1, filterMixer8, 1},
	{filter1, 2, filterMixer1, 2},
	{filter2, 2, filterMixer2, 2},
	{filter3, 2, filterMixer3, 2},
	{filter4, 2, filterMixer4, 2},
	{filter5, 2, filterMixer5, 2},
	{filter6, 2, filterMixer6, 2},
	{filter7, 2, filterMixer7, 2},
	{filter8, 2, filterMixer8, 2},
	{filterModMixer1b, 0, filter1, 1},
	{filterModMixer2b, 0, filter2, 1},
	{filterModMixer3b, 0, filter3, 1},
	{filterModMixer4b, 0, filter4, 1},
	{filterModMixer5b, 0, filter5, 1},
	{filterModMixer6b, 0, filter6, 1},
	{filterModMixer7b, 0, filter7, 1},
	{filterModMixer8b, 0, filter8, 1},
	{waveformMixer1, 0, filter1, 0},
	{waveformMixer2, 0, filter2, 0},
	{waveformMixer3, 0, filter3, 0},
	{waveformMixer4, 0, filter4, 0},
	{waveformMixer5, 0, filter5, 0},
	{waveformMixer6, 0, filter6, 0},
	{waveformMixer7, 0, filter7, 0},
	{waveformMixer8, 0, filter8, 0}
};

AudioConnection ladderFilter_connections[24] = {
	{filter2_1, 0, filterMixer1, 3},
	{filter2_2, 0, filterMixer2, 3},
	{filter2_3, 0, filterMixer3, 3},
	{filter2_4, 0, filterMixer4, 3},
	{filter2_5, 0, filterMixer5, 3},
	{filter2_6, 0, filterMixer6, 3},
	{filter2_7, 0, filterMixer7, 3},
	{filter2_8, 0, filterMixer8, 3},
	{filterModMixer1b, 0, filter2_1, 1},
	{filterModMixer2b, 0, filter2_2, 1},
	{filterModMixer3b, 0, filter2_3, 1},
	{filterModMixer4b, 0, filter2_4, 1},
	{filterModMixer5b, 0, filter2_5, 1},
	{filterModMixer6b, 0, filter2_6, 1},
	{filterModMixer7b, 0, filter2_7, 1},
	{filterModMixer8b, 0, filter2_8, 1},
	{waveformMixer1, 0, filter2_1, 0},
	{waveformMixer2, 0, filter2_2, 0},
	{waveformMixer3, 0, filter2_3, 0},
	{waveformMixer4, 0, filter2_4, 0},
	{waveformMixer5, 0, filter2_5, 0},
	{waveformMixer6, 0, filter2_6, 0},
	{waveformMixer7, 0, filter2_7, 0},
	{waveformMixer8, 0, filter2_8, 0}	
};

AudioConnection          patchCord8311(filterModMixer1b, 0, filter2_1, 1);	// ok
AudioConnection          patchCord8321(filterModMixer2b, 0, filter2_2, 1);	// ok
AudioConnection          patchCord8331(filterModMixer3b, 0, filter2_3, 1);	// ok
AudioConnection          patchCord8341(filterModMixer4b, 0, filter2_4, 1);	// ok
AudioConnection          patchCord8351(filterModMixer5b, 0, filter2_5, 1);	// ok
AudioConnection          patchCord8361(filterModMixer6b, 0, filter2_6, 1);	// ok
AudioConnection          patchCord8371(filterModMixer7b, 0, filter2_7, 1);	// ok
AudioConnection          patchCord8381(filterModMixer8b, 0, filter2_8, 1);	// ok


// Mixer ----------------------------------------------------------------
AudioConnection          patchCord751(noiseMixer, 0, waveformMixer1, 2);	// ok
AudioConnection          patchCord752(noiseMixer, 0, waveformMixer2, 2);	// ok
AudioConnection          patchCord753(noiseMixer, 0, waveformMixer3, 2);	// ok
AudioConnection          patchCord754(noiseMixer, 0, waveformMixer4, 2);	// ok
AudioConnection          patchCord755(noiseMixer, 0, waveformMixer5, 2);	// ok
AudioConnection          patchCord756(noiseMixer, 0, waveformMixer6, 2);	// ok
AudioConnection          patchCord757(noiseMixer, 0, waveformMixer7, 2);	// ok
AudioConnection          patchCord758(noiseMixer, 0, waveformMixer8, 2);	// ok

AudioConnection          patchCord771(pwMixer1a, 0, waveformModa[0], 1);	// ok
AudioConnection          patchCord772(pwMixer1b, 0, waveformModb[0], 1);	// ok
AudioConnection          patchCord773(pwMixer2a, 0, waveformModa[1], 1);	// ok
AudioConnection          patchCord774(pwMixer2b, 0, waveformModb[1], 1);	// ok
AudioConnection          patchCord775(pwMixer3a, 0, waveformModa[2], 1);	// ok
AudioConnection          patchCord776(pwMixer3b, 0, waveformModb[2], 1);	// ok
AudioConnection          patchCord777(pwMixer4a, 0, waveformModa[3], 1);	// ok
AudioConnection          patchCord778(pwMixer4b, 0, waveformModb[3], 1);	// ok
AudioConnection          patchCord779(pwMixer5a, 0, waveformModa[4], 1);	// ok
AudioConnection          patchCord780(pwMixer5b, 0, waveformModb[4], 1);	// ok
AudioConnection          patchCord781(pwMixer6a, 0, waveformModa[5], 1);	// ok
AudioConnection          patchCord782(pwMixer6b, 0, waveformModb[5], 1);	// ok
AudioConnection          patchCord783(pwMixer7a, 0, waveformModa[6], 1);	// ok
AudioConnection          patchCord784(pwMixer7b, 0, waveformModb[6], 1);	// ok
AudioConnection          patchCord785(pwMixer8a, 0, waveformModa[7], 1);	// ok
AudioConnection          patchCord786(pwMixer8b, 0, waveformModb[7], 1);	// ok

AudioConnection          patchCord801(oscGlobalModMixer1, 0, oscModMixer1a, 0);	// ok
AudioConnection          patchCord802(oscGlobalModMixer1, 0, oscModMixer1b, 0);	// ok
AudioConnection          patchCord803(oscGlobalModMixer2, 0, oscModMixer2a, 0);	// ok
AudioConnection          patchCord804(oscGlobalModMixer2, 0, oscModMixer2b, 0);	// ok
AudioConnection          patchCord805(oscGlobalModMixer3, 0, oscModMixer3a, 0);	// ok
AudioConnection          patchCord806(oscGlobalModMixer3, 0, oscModMixer3b, 0);	// ok
AudioConnection          patchCord807(oscGlobalModMixer4, 0, oscModMixer4a, 0);	// ok
AudioConnection          patchCord808(oscGlobalModMixer4, 0, oscModMixer4b, 0);	// ok
AudioConnection          patchCord809(oscGlobalModMixer5, 0, oscModMixer5a, 0);	// ok
AudioConnection          patchCord810(oscGlobalModMixer5, 0, oscModMixer5b, 0);	// ok
AudioConnection          patchCord811(oscGlobalModMixer6, 0, oscModMixer6a, 0);	// ok
AudioConnection          patchCord812(oscGlobalModMixer6, 0, oscModMixer6b, 0);	// ok
AudioConnection          patchCord813(oscGlobalModMixer7, 0, oscModMixer7a, 0);	// ok
AudioConnection          patchCord814(oscGlobalModMixer7, 0, oscModMixer7b, 0);	// ok
AudioConnection          patchCord815(oscGlobalModMixer8, 0, oscModMixer8a, 0);	// ok
AudioConnection          patchCord816(oscGlobalModMixer8, 0, oscModMixer8b, 0);	// ok

// waveshaper
AudioConnection          patchCord841(filterMixer1, WaveshaperAmp[0]);	// ok
AudioConnection          patchCord842(filterMixer2, WaveshaperAmp[1]);	// ok
AudioConnection          patchCord843(filterMixer3, WaveshaperAmp[2]);	// ok
AudioConnection          patchCord844(filterMixer4, WaveshaperAmp[3]);	// ok
AudioConnection          patchCord845(filterMixer5, WaveshaperAmp[4]);	// ok
AudioConnection          patchCord846(filterMixer6, WaveshaperAmp[5]);	// ok
AudioConnection          patchCord847(filterMixer7, WaveshaperAmp[6]);	// ok
AudioConnection          patchCord848(filterMixer8, WaveshaperAmp[7]);	// ok

AudioConnection          patchCord849(WaveshaperAmp[0], waveshaper[0]);		// ok
AudioConnection          patchCord850(WaveshaperAmp[1], waveshaper[1]);		// ok
AudioConnection          patchCord851(WaveshaperAmp[2], waveshaper[2]);		// ok
AudioConnection          patchCord852(WaveshaperAmp[3], waveshaper[3]);		// ok
AudioConnection          patchCord853(WaveshaperAmp[4], waveshaper[4]);		// ok
AudioConnection          patchCord854(WaveshaperAmp[5], waveshaper[5]);		// ok
AudioConnection          patchCord855(WaveshaperAmp[6], waveshaper[6]);		// ok
AudioConnection          patchCord856(WaveshaperAmp[7], waveshaper[7]);		// ok


AudioConnection waveshaper_connections[8] = {
	{waveshaper[0], ampEnvelope[0]},
	{waveshaper[1], ampEnvelope[1]},
	{waveshaper[2], ampEnvelope[2]},
	{waveshaper[3], ampEnvelope[3]},
	{waveshaper[4], ampEnvelope[4]},
	{waveshaper[5], ampEnvelope[5]},
	{waveshaper[6], ampEnvelope[6]},
	{waveshaper[7], ampEnvelope[7]}	
};

AudioConnection          patchCord857(waveshaper[0], ampEnvelope[0]);		// ok
AudioConnection          patchCord858(waveshaper[1], ampEnvelope[1]);		// ok
AudioConnection          patchCord859(waveshaper[2], ampEnvelope[2]);		// ok
AudioConnection          patchCord860(waveshaper[3], ampEnvelope[3]);		// ok
AudioConnection          patchCord861(waveshaper[4], ampEnvelope[4]);		// ok
AudioConnection          patchCord862(waveshaper[5], ampEnvelope[5]);		// ok
AudioConnection          patchCord863(waveshaper[6], ampEnvelope[6]);		// ok
AudioConnection          patchCord864(waveshaper[7], ampEnvelope[7]);		// ok

AudioConnection          patchCord871(oscModMixer1a, 0, waveformModa[0], 0);	// ok
AudioConnection          patchCord872(oscModMixer1b, 0, waveformModb[0], 0);	// ok
AudioConnection          patchCord873(oscModMixer2a, 0, waveformModa[1], 0);	// ok
AudioConnection          patchCord874(oscModMixer2b, 0, waveformModb[1], 0);	// ok
AudioConnection          patchCord875(oscModMixer3a, 0, waveformModa[2], 0);	// ok
AudioConnection          patchCord876(oscModMixer3b, 0, waveformModb[2], 0);	// ok
AudioConnection          patchCord877(oscModMixer4a, 0, waveformModa[3], 0);	// ok
AudioConnection          patchCord878(oscModMixer4b, 0, waveformModb[3], 0);	// ok
AudioConnection          patchCord879(oscModMixer5a, 0, waveformModa[4], 0);	// ok
AudioConnection          patchCord880(oscModMixer5b, 0, waveformModb[4], 0);	// ok
AudioConnection          patchCord881(oscModMixer6a, 0, waveformModa[5], 0);	// ok
AudioConnection          patchCord882(oscModMixer6b, 0, waveformModb[5], 0);	// ok
AudioConnection          patchCord883(oscModMixer7a, 0, waveformModa[6], 0);	// ok
AudioConnection          patchCord884(oscModMixer7b, 0, waveformModb[6], 0);	// ok
AudioConnection          patchCord885(oscModMixer8a, 0, waveformModa[7], 0);	// ok
AudioConnection          patchCord886(oscModMixer8b, 0, waveformModb[7], 0);	// ok

AudioConnection          patchCord1200(AtouchDc, 0, AtouchFilter, 0);	// ok
AudioConnection          patchCord1201(AtouchFilter, 0, AtouchModAmp1, 0);	// ok
AudioConnection          patchCord1202(AtouchModAmp1, 0, filterModMixAtCC1, 0);	// ok
AudioConnection          patchCord1203(AtouchModAmp1, 0, filterModMixAtCC2, 0);	// ok
AudioConnection          patchCord1204(AtouchModAmp1, 0, filterModMixAtCC3, 0);	// ok
AudioConnection          patchCord1205(AtouchModAmp1, 0, filterModMixAtCC4, 0);	// ok
AudioConnection          patchCord1206(AtouchModAmp1, 0, filterModMixAtCC5, 0);	// ok
AudioConnection          patchCord1207(AtouchModAmp1, 0, filterModMixAtCC6, 0);	// ok
AudioConnection          patchCord1208(AtouchModAmp1, 0, filterModMixAtCC7, 0);	// ok
AudioConnection          patchCord1209(AtouchModAmp1, 0, filterModMixAtCC8, 0);	// ok

AudioConnection          patchCord1250(DC_FilterModCC, 0, FilterModCC, 0);	// ok
AudioConnection          patchCord1251(FilterModCC, 0, filterModMixAtCC1, 1);	// ok
AudioConnection          patchCord1252(FilterModCC, 0, filterModMixAtCC2, 1);	// ok
AudioConnection          patchCord1253(FilterModCC, 0, filterModMixAtCC3, 1);	// ok
AudioConnection          patchCord1254(FilterModCC, 0, filterModMixAtCC4, 1);	// ok
AudioConnection          patchCord1255(FilterModCC, 0, filterModMixAtCC5, 1);	// ok
AudioConnection          patchCord1256(FilterModCC, 0, filterModMixAtCC6, 1);	// ok
AudioConnection          patchCord1257(FilterModCC, 0, filterModMixAtCC7, 1);	// ok
AudioConnection          patchCord1258(FilterModCC, 0, filterModMixAtCC8, 1);	// ok

AudioConnection          patchCord1260(DC_FilterModwheel, 0, filterModMixAtCC1, 2);	// ok
AudioConnection          patchCord1261(DC_FilterModwheel, 0, filterModMixAtCC2, 2);	// ok
AudioConnection          patchCord1262(DC_FilterModwheel, 0, filterModMixAtCC3, 2);	// ok
AudioConnection          patchCord1263(DC_FilterModwheel, 0, filterModMixAtCC4, 2);	// ok
AudioConnection          patchCord1264(DC_FilterModwheel, 0, filterModMixAtCC5, 2);	// ok
AudioConnection          patchCord1265(DC_FilterModwheel, 0, filterModMixAtCC6, 2);	// ok
AudioConnection          patchCord1266(DC_FilterModwheel, 0, filterModMixAtCC7, 2);	// ok
AudioConnection          patchCord1267(DC_FilterModwheel, 0, filterModMixAtCC8, 2);	// ok

AudioConnection          patchCord1271(filterModMixAtCC1, 0, filterModMixer1b, 3);	// ok
AudioConnection          patchCord1272(filterModMixAtCC2, 0, filterModMixer2b, 3);	// ok
AudioConnection          patchCord1273(filterModMixAtCC3, 0, filterModMixer3b, 3);	// ok
AudioConnection          patchCord1274(filterModMixAtCC4, 0, filterModMixer4b, 3);	// ok
AudioConnection          patchCord1275(filterModMixAtCC5, 0, filterModMixer5b, 3);	// ok
AudioConnection          patchCord1276(filterModMixAtCC6, 0, filterModMixer6b, 3);	// ok
AudioConnection          patchCord1277(filterModMixAtCC7, 0, filterModMixer7b, 3);	// ok
AudioConnection          patchCord1278(filterModMixAtCC8, 0, filterModMixer8b, 3);	// ok



AudioConnection          patchCord1300(AtouchFilter, 0, AtouchModAmp2, 0);	// ok
AudioConnection          patchCord1301(AtouchModAmp2, 0, oscGlobalModMixer1, 3);	// ok
AudioConnection          patchCord1302(AtouchModAmp2, 0, oscGlobalModMixer2, 3);	// ok
AudioConnection          patchCord1303(AtouchModAmp2, 0, oscGlobalModMixer3, 3);	// ok
AudioConnection          patchCord1304(AtouchModAmp2, 0, oscGlobalModMixer4, 3);	// ok
AudioConnection          patchCord1305(AtouchModAmp2, 0, oscGlobalModMixer5, 3);	// ok
AudioConnection          patchCord1306(AtouchModAmp2, 0, oscGlobalModMixer6, 3);	// ok
AudioConnection          patchCord1307(AtouchModAmp2, 0, oscGlobalModMixer7, 3);	// ok
AudioConnection          patchCord1308(AtouchModAmp2, 0, oscGlobalModMixer8, 3);	// ok


//AudioConnection			 patchCord952(ModLfo3, 0, LFO3ModMixer1, 0);
AudioConnection			 patchCord953(ModLfo3, 0, LFO3EnvelopeAmp, 0);
AudioConnection			 patchCord954(LFO3EnvelopeAmp, 0, LFO3ModMixer1, 0);
AudioConnection			 patchCord955(LFO3ModMixer1Dc, 0, LFO3ModMixer1, 1);
AudioConnection			 patchCord956(ModLfo3, 0, LFO3ModMixer1, 2);
AudioConnection			 patchCord957(LFO3ModMixer1, 0, multiply1, 1);
AudioConnection			 patchCord958(LFO3ModMixer1, 0, multiply2, 1);

AudioConnection ampEnvelope_connections[16] = {
		{ampEnvelope[0], 0, voiceMixer1a, 0},
		{ampEnvelope[1], 0, voiceMixer1a, 1},
		{ampEnvelope[2], 0, voiceMixer1a, 2},
		{ampEnvelope[3], 0, voiceMixer1a, 3},
		{ampEnvelope[4], 0, voiceMixer1b, 0},
		{ampEnvelope[5], 0, voiceMixer1b, 1},
		{ampEnvelope[6], 0, voiceMixer1b, 2},
		{ampEnvelope[7], 0, voiceMixer1b, 3},
		{ampEnvelope[0], 0, voiceMixer2a, 0},
		{ampEnvelope[1], 0, voiceMixer2a, 1},
		{ampEnvelope[2], 0, voiceMixer2a, 2},
		{ampEnvelope[3], 0, voiceMixer2a, 3},
		{ampEnvelope[4], 0, voiceMixer2b, 0},
		{ampEnvelope[5], 0, voiceMixer2b, 1},
		{ampEnvelope[6], 0, voiceMixer2b, 2},
		{ampEnvelope[7], 0, voiceMixer2b, 3},
			
};

AudioConnection          patchCord1461(voiceMixer1a, 0, voiceMixerM1, 0);	// ok
AudioConnection          patchCord1462(voiceMixer1b, 0, voiceMixerM1, 1);	// ok
AudioConnection          patchCord1463(voiceMixer2a, 0, voiceMixerM2, 0);	// ok
AudioConnection          patchCord1464(voiceMixer2b, 0, voiceMixerM2, 1);	// ok
AudioConnection          patchCord1465(voiceMixerM1, 0, multiply1, 0);
AudioConnection          patchCord1466(voiceMixerM2, 0, multiply2, 0);
AudioConnection			 patchCord1467(multiply1, 0, hpFilter1, 0);
AudioConnection			 patchCord1468(multiply2, 0, hpFilter2, 0);
AudioConnection          patchCord1469(hpFilter1, 2, PCM5102A, 0);
AudioConnection          patchCord1470(hpFilter2, 2, PCM5102A, 1);
AudioConnection          patchCord1471(hpFilter1, 2, ScopePeakMixer, 0);
AudioConnection          patchCord1472(hpFilter1, 2, ScopePeakMixer, 1);
AudioConnection          patchCord1473(ScopePeakMixer, 0, scope, 0);
AudioConnection          patchCord1474(ScopePeakMixer, 0, peak, 0);

// myConnection.disconnect();
// myConnection.connect();
