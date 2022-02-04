// Teensy AudioPatching.h

// Filtertyp
// 1 = 12dB State Variable (Chamberlin) Filter
// 2 = 24dB LowPass Ladder Filter
#define Filter 1

AudioSynthWaveformDc     constant1Dc;    //xy=69.5,437
AudioSynthWaveformDc     pitchBend;      //xy=128,74
AudioSynthWaveformDc     pwa;            //xy=107,256
AudioSynthWaveformDc     pwb;            //xy=110,301

AudioSynthWaveformDc     FilterVelo1;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo2;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo3;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo4;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo5;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo6;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo7;    //xy=69.5,437
AudioSynthWaveformDc     FilterVelo8;    //xy=69.5,437

AudioSynthWaveformDc     glide1;          //xy=124,152
AudioSynthWaveformDc     glide2;          //xy=124,152
AudioSynthWaveformDc     glide3;          //xy=124,152
AudioSynthWaveformDc     glide4;          //xy=124,152
AudioSynthWaveformDc     glide5;          //xy=124,152
AudioSynthWaveformDc     glide6;          //xy=124,152
AudioSynthWaveformDc     glide7;          //xy=124,152
AudioSynthWaveformDc     glide8;          //xy=124,152

AudioSynthWaveformDc     keytracking1;    //xy=176,745
AudioSynthWaveformDc     keytracking2;    //xy=196,745
AudioSynthWaveformDc     keytracking3;    //xy=216,745
AudioSynthWaveformDc     keytracking4;    //xy=236,745
AudioSynthWaveformDc     keytracking5;    //xy=216,745
AudioSynthWaveformDc     keytracking6;    //xy=236,745
AudioSynthWaveformDc     keytracking7;    //xy=176,745
AudioSynthWaveformDc     keytracking8;    //xy=196,745

AudioSynthWaveformModulatedTS waveformMod1b;  //xy=503,170
AudioSynthWaveformModulatedTS waveformMod1a;  //xy=507,114
AudioSynthWaveformModulatedTS waveformMod2b;  //xy=513,550
AudioSynthWaveformModulatedTS waveformMod4a;  //xy=519,899
AudioSynthWaveformModulatedTS waveformMod2a;  //xy=521,458
AudioSynthWaveformModulatedTS waveformMod4b;  //xy=525,940
AudioSynthWaveformModulatedTS waveformMod3a;  //xy=532,678
AudioSynthWaveformModulatedTS waveformMod3b;  //xy=535,744
AudioSynthWaveformModulatedTS waveformMod5a;  //xy=521,458
AudioSynthWaveformModulatedTS waveformMod5b;  //xy=525,940
AudioSynthWaveformModulatedTS waveformMod6a;  //xy=532,678
AudioSynthWaveformModulatedTS waveformMod6b;  //xy=535,744
AudioSynthWaveformModulatedTS waveformMod7b;  //xy=503,170
AudioSynthWaveformModulatedTS waveformMod7a;  //xy=507,114
AudioSynthWaveformModulatedTS waveformMod8b;  //xy=513,550
AudioSynthWaveformModulatedTS waveformMod8a;  //xy=519,899

AudioSynthNoisePink      pink;           //xy=268,349
AudioSynthNoiseWhite     white;           //xy=268,359
AudioSynthWaveformTS     pwmLfoA;         //xy=92,208
AudioSynthWaveformTS     pwmLfoB;         //xy=92,208
AudioSynthWaveformTS     pitchLfo;         //xy=122,111
AudioSynthWaveformTS     filterLfo;         //xy=166,632



AudioEffectEnvelope      filterEnvelope1;   //xy=628,300
AudioEffectEnvelope      filterEnvelope2;   //xy=640,604
AudioEffectEnvelope      filterEnvelope3;   //xy=636,857
AudioEffectEnvelope      filterEnvelope4;   //xy=617,1107
AudioEffectEnvelope      filterEnvelope5;   //xy=638,300
AudioEffectEnvelope      filterEnvelope6;   //xy=629,1107
AudioEffectEnvelope      filterEnvelope7;   //xy=629,1107
AudioEffectEnvelope      filterEnvelope8;   //xy=638,300

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

AudioMixer4              oscGlobalModMixer;    //xy=296,95
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

AudioEffectDigitalCombine oscFX1;       //xy=663,223
AudioEffectDigitalCombine oscFX2;       //xy=684,542
AudioEffectDigitalCombine oscFX4;       //xy=686,1046
AudioEffectDigitalCombine oscFX3;       //xy=687,803
AudioEffectDigitalCombine oscFX5;       //xy=686,1046
AudioEffectDigitalCombine oscFX6;       //xy=687,803
AudioEffectDigitalCombine oscFX7;       //xy=663,223
AudioEffectDigitalCombine oscFX8;       //xy=684,542



AudioMixer4              waveformMixer1; //xy=824,170
AudioMixer4              waveformMixer2; //xy=829,476
AudioMixer4              waveformMixer3; //xy=846,774
AudioMixer4              waveformMixer4; //xy=854,1023
AudioMixer4              waveformMixer5; //xy=846,774
AudioMixer4              waveformMixer6; //xy=854,1023
AudioMixer4              waveformMixer7; //xy=824,170
AudioMixer4              waveformMixer8; //xy=829,476

AudioMixer4              filterModMixer1;   //xy=845,292
AudioMixer4              filterModMixer2;   //xy=848,637
AudioMixer4              filterModMixer3;   //xy=852,937
AudioMixer4              filterModMixer4;   //xy=855,1099
AudioMixer4              filterModMixer5;   //xy=852,937
AudioMixer4              filterModMixer6;   //xy=855,1099
AudioMixer4              filterModMixer7;   //xy=845,292
AudioMixer4              filterModMixer8;   //xy=848,637

#if Filter == 1
	AudioFilterStateVariable filter1;        //xy=1000,210
	AudioFilterStateVariable filter2;        //xy=994,498
	AudioFilterStateVariable filter3;        //xy=1002,822
	AudioFilterStateVariable filter4;        //xy=1022,1047
	AudioFilterStateVariable filter5;        //xy=1002,822
	AudioFilterStateVariable filter6;        //xy=1022,1047
	AudioFilterStateVariable filter7;        //xy=994,498
	AudioFilterStateVariable filter8;        //xy=1000,210
#else
	AudioFilterLadder filter1;        //xy=1000,210
	AudioFilterLadder filter2;        //xy=994,498
	AudioFilterLadder filter3;        //xy=1002,822
	AudioFilterLadder filter4;        //xy=1022,1047
	AudioFilterLadder filter5;        //xy=1002,822
	AudioFilterLadder filter6;        //xy=1022,1047
	AudioFilterLadder filter7;        //xy=994,498
	AudioFilterLadder filter8;        //xy=1000,210
#endif

AudioFilterStateVariable dcOffsetFilter;     //xy=1564,580

AudioMixer4				filterMixer101;

AudioMixer4              filterMixer1;   //xy=1151,214
AudioMixer4              filterMixer2;   //xy=1144,504
AudioMixer4              filterMixer3;   //xy=1144,825
AudioMixer4              filterMixer4;   //xy=1155,1050
AudioMixer4              filterMixer5;   //xy=1151,214
AudioMixer4              filterMixer6;   //xy=1155,1050
AudioMixer4              filterMixer7;   //xy=1144,504
AudioMixer4              filterMixer8;   //xy=1144,825

AudioAmplifier           WaveshaperAmp1;   //xy=1145,825
AudioAmplifier           WaveshaperAmp2;   //xy=1145,826
AudioAmplifier           WaveshaperAmp3;   //xy=1145,827
AudioAmplifier           WaveshaperAmp4;   //xy=1145,828
AudioAmplifier           WaveshaperAmp5;   //xy=1145,829
AudioAmplifier           WaveshaperAmp6;   //xy=1145,830
AudioAmplifier           WaveshaperAmp7;   //xy=1145,831
AudioAmplifier           WaveshaperAmp8;   //xy=1145,832

AudioEffectWaveshaper	 waveshaper1;		//xy=638,301
AudioEffectWaveshaper	 waveshaper2;		//xy=638,302
AudioEffectWaveshaper	 waveshaper3;		//xy=638,303
AudioEffectWaveshaper	 waveshaper4;		//xy=638,304
AudioEffectWaveshaper	 waveshaper5;		//xy=638,305
AudioEffectWaveshaper	 waveshaper6;		//xy=638,306
AudioEffectWaveshaper	 waveshaper7;		//xy=638,307
AudioEffectWaveshaper	 waveshaper8;		//xy=638,308

AudioEffectEnvelope      ampEnvelope1;   //xy=1327,211
AudioEffectEnvelope      ampEnvelope2;   //xy=1315,503
AudioEffectEnvelope      ampEnvelope3;   //xy=1315,823
AudioEffectEnvelope      ampEnvelope4;   //xy=1321,1045
AudioEffectEnvelope      ampEnvelope5;   //xy=1321,1045
AudioEffectEnvelope      ampEnvelope6;   //xy=1327,211
AudioEffectEnvelope      ampEnvelope7;   //xy=1315,503
AudioEffectEnvelope      ampEnvelope8;   //xy=1315,823

AudioMixer4              voiceMixer1;     //xy=1524,570
AudioMixer4              voiceMixer2;     //xy=1524,570
AudioMixer4              voiceMixerM;     //xy=1544,580

AudioAnalyzePeak		 peak;
Oscilloscope             scope;
AudioOutputI2S			 PCM5102A;

AudioMixer4              LadderMixer1;


// AudioSynthWaveformDc -------------------------------------------------
AudioConnection          patchCord1(constant1Dc, filterEnvelope1);
AudioConnection          patchCord2(constant1Dc, filterEnvelope2);
AudioConnection          patchCord3(constant1Dc, filterEnvelope3);
AudioConnection          patchCord4(constant1Dc, filterEnvelope4);
AudioConnection          patchCord5(constant1Dc, filterEnvelope5);
AudioConnection          patchCord6(constant1Dc, filterEnvelope6);
AudioConnection          patchCord7(constant1Dc, filterEnvelope7);
AudioConnection          patchCord8(constant1Dc, filterEnvelope8);

AudioConnection          patchCord20(pwa, 0, pwMixer1a, 1);
AudioConnection          patchCord21(pwa, 0, pwMixer2a, 1);
AudioConnection          patchCord22(pwa, 0, pwMixer3a, 1);
AudioConnection          patchCord23(pwa, 0, pwMixer4a, 1);
AudioConnection          patchCord24(pwa, 0, pwMixer5a, 1);
AudioConnection          patchCord25(pwa, 0, pwMixer6a, 1);
AudioConnection          patchCord26(pwa, 0, pwMixer7a, 1);
AudioConnection          patchCord27(pwa, 0, pwMixer8a, 1);
AudioConnection          patchCord28(pwb, 0, pwMixer1b, 1);
AudioConnection          patchCord29(pwb, 0, pwMixer2b, 1);
AudioConnection          patchCord30(pwb, 0, pwMixer3b, 1);
AudioConnection          patchCord31(pwb, 0, pwMixer4b, 1);
AudioConnection          patchCord32(pwb, 0, pwMixer5b, 1);
AudioConnection          patchCord33(pwb, 0, pwMixer6b, 1);
AudioConnection          patchCord34(pwb, 0, pwMixer7b, 1);
AudioConnection          patchCord35(pwb, 0, pwMixer8b, 1);

AudioConnection          patchCord40(pitchBend, 0, oscGlobalModMixer, 0);

AudioConnection          patchCord50(keytracking1, 0, filterModMixer1, 2);
AudioConnection          patchCord51(keytracking2, 0, filterModMixer2, 2);
AudioConnection          patchCord52(keytracking3, 0, filterModMixer3, 2);
AudioConnection          patchCord53(keytracking4, 0, filterModMixer4, 2);
AudioConnection          patchCord54(keytracking5, 0, filterModMixer5, 2);
AudioConnection          patchCord55(keytracking6, 0, filterModMixer6, 2);
AudioConnection          patchCord56(keytracking7, 0, filterModMixer7, 2);
AudioConnection          patchCord57(keytracking8, 0, filterModMixer8, 2);

AudioConnection          patchCord70(FilterVelo1, 0, filterModMixer1, 3);
AudioConnection          patchCord71(FilterVelo2, 0, filterModMixer2, 3);
AudioConnection          patchCord72(FilterVelo3, 0, filterModMixer3, 3);
AudioConnection          patchCord73(FilterVelo4, 0, filterModMixer4, 3);
AudioConnection          patchCord74(FilterVelo5, 0, filterModMixer5, 3);
AudioConnection          patchCord75(FilterVelo6, 0, filterModMixer6, 3);
AudioConnection          patchCord76(FilterVelo7, 0, filterModMixer7, 3);
AudioConnection          patchCord77(FilterVelo8, 0, filterModMixer8, 3);

AudioConnection          patchCord100(glide1, 0, oscModMixer1a, 2);
AudioConnection          patchCord101(glide1, 0, oscModMixer1b, 2);
AudioConnection          patchCord102(glide2, 0, oscModMixer2a, 2);
AudioConnection          patchCord103(glide2, 0, oscModMixer2b, 2);
AudioConnection          patchCord104(glide3, 0, oscModMixer3a, 2);
AudioConnection          patchCord105(glide3, 0, oscModMixer3b, 2);
AudioConnection          patchCord106(glide4, 0, oscModMixer4a, 2);
AudioConnection          patchCord107(glide4, 0, oscModMixer4b, 2);
AudioConnection          patchCord108(glide5, 0, oscModMixer5a, 2);
AudioConnection          patchCord109(glide5, 0, oscModMixer5b, 2);
AudioConnection          patchCord110(glide6, 0, oscModMixer6a, 2);
AudioConnection          patchCord111(glide6, 0, oscModMixer6b, 2);
AudioConnection          patchCord112(glide7, 0, oscModMixer7a, 2);
AudioConnection          patchCord113(glide7, 0, oscModMixer7b, 2);
AudioConnection          patchCord114(glide8, 0, oscModMixer8a, 2);
AudioConnection          patchCord115(glide8, 0, oscModMixer8b, 2);

// AudioSynthWaveformModulatedTS ----------------------------------------
AudioConnection          patchCord201(waveformMod1a, 0, waveformMixer1, 0);
AudioConnection          patchCord202(waveformMod2a, 0, waveformMixer2, 0);
AudioConnection          patchCord203(waveformMod3a, 0, waveformMixer3, 0);
AudioConnection          patchCord204(waveformMod4a, 0, waveformMixer4, 0);
AudioConnection          patchCord205(waveformMod5a, 0, waveformMixer5, 0);
AudioConnection          patchCord206(waveformMod6a, 0, waveformMixer6, 0);
AudioConnection          patchCord207(waveformMod7a, 0, waveformMixer7, 0);
AudioConnection          patchCord208(waveformMod8a, 0, waveformMixer8, 0);
AudioConnection          patchCord209(waveformMod1b, 0, waveformMixer1, 1);
AudioConnection          patchCord210(waveformMod2b, 0, waveformMixer2, 1);
AudioConnection          patchCord211(waveformMod3b, 0, waveformMixer3, 1);
AudioConnection          patchCord212(waveformMod4b, 0, waveformMixer4, 1);
AudioConnection          patchCord213(waveformMod5b, 0, waveformMixer5, 1);
AudioConnection          patchCord214(waveformMod6b, 0, waveformMixer6, 1);
AudioConnection          patchCord215(waveformMod7b, 0, waveformMixer7, 1);
AudioConnection          patchCord216(waveformMod8b, 0, waveformMixer8, 1);

AudioConnection          patchCord251(waveformMod1a, 0, oscModMixer1b, 3);
AudioConnection          patchCord252(waveformMod2a, 0, oscModMixer2b, 3);
AudioConnection          patchCord253(waveformMod3a, 0, oscModMixer3b, 3);
AudioConnection          patchCord254(waveformMod4a, 0, oscModMixer4b, 3);
AudioConnection          patchCord255(waveformMod5a, 0, oscModMixer5b, 3);
AudioConnection          patchCord256(waveformMod6a, 0, oscModMixer6b, 3);
AudioConnection          patchCord257(waveformMod7a, 0, oscModMixer7b, 3);
AudioConnection          patchCord258(waveformMod8a, 0, oscModMixer8b, 3);
AudioConnection          patchCord259(waveformMod1b, 0, oscModMixer1a, 3);
AudioConnection          patchCord260(waveformMod2b, 0, oscModMixer2a, 3);
AudioConnection          patchCord261(waveformMod3b, 0, oscModMixer3a, 3);
AudioConnection          patchCord262(waveformMod4b, 0, oscModMixer4a, 3);
AudioConnection          patchCord263(waveformMod5b, 0, oscModMixer5a, 3);
AudioConnection          patchCord264(waveformMod6b, 0, oscModMixer6a, 3);
AudioConnection          patchCord265(waveformMod7b, 0, oscModMixer7a, 3);
AudioConnection          patchCord266(waveformMod8b, 0, oscModMixer8a, 3);

AudioConnection          patchCord301(waveformMod1a, 0, oscFX1, 0);
AudioConnection          patchCord302(waveformMod2a, 0, oscFX2, 0);
AudioConnection          patchCord303(waveformMod3a, 0, oscFX3, 0);
AudioConnection          patchCord304(waveformMod4a, 0, oscFX4, 0);
AudioConnection          patchCord305(waveformMod5a, 0, oscFX5, 0);
AudioConnection          patchCord306(waveformMod6a, 0, oscFX6, 0);
AudioConnection          patchCord307(waveformMod7a, 0, oscFX7, 0);
AudioConnection          patchCord308(waveformMod8a, 0, oscFX8, 0);
AudioConnection          patchCord309(waveformMod1b, 0, oscFX1, 1);
AudioConnection          patchCord310(waveformMod2b, 0, oscFX2, 1);
AudioConnection          patchCord311(waveformMod3b, 0, oscFX3, 1);
AudioConnection          patchCord312(waveformMod4b, 0, oscFX4, 1);
AudioConnection          patchCord313(waveformMod5b, 0, oscFX5, 1);
AudioConnection          patchCord314(waveformMod6b, 0, oscFX6, 1);
AudioConnection          patchCord315(waveformMod7b, 0, oscFX7, 1);
AudioConnection          patchCord316(waveformMod8b, 0, oscFX8, 1);

// AudioSynthWaveformTS -------------------------------------------------
AudioConnection          patchCord351(pwmLfoA, 0, pwMixer1a, 0);
AudioConnection          patchCord352(pwmLfoB, 0, pwMixer2a, 0);
AudioConnection          patchCord353(pwmLfoA, 0, pwMixer3a, 0);
AudioConnection          patchCord354(pwmLfoB, 0, pwMixer4a, 0);
AudioConnection          patchCord355(pwmLfoA, 0, pwMixer5a, 0);
AudioConnection          patchCord356(pwmLfoB, 0, pwMixer6a, 0);
AudioConnection          patchCord357(pwmLfoA, 0, pwMixer7a, 0);
AudioConnection          patchCord358(pwmLfoB, 0, pwMixer8a, 0);
AudioConnection          patchCord359(pwmLfoA, 0, pwMixer1b, 0);
AudioConnection          patchCord360(pwmLfoB, 0, pwMixer2b, 0);
AudioConnection          patchCord361(pwmLfoA, 0, pwMixer3b, 0);
AudioConnection          patchCord362(pwmLfoB, 0, pwMixer4b, 0);
AudioConnection          patchCord363(pwmLfoA, 0, pwMixer5b, 0);
AudioConnection          patchCord364(pwmLfoB, 0, pwMixer6b, 0);
AudioConnection          patchCord365(pwmLfoA, 0, pwMixer7b, 0);
AudioConnection          patchCord366(pwmLfoB, 0, pwMixer8b, 0);

AudioConnection          patchCord401(filterLfo, 0, filterModMixer1, 1);
AudioConnection          patchCord402(filterLfo, 0, filterModMixer2, 1);
AudioConnection          patchCord403(filterLfo, 0, filterModMixer3, 1);
AudioConnection          patchCord404(filterLfo, 0, filterModMixer4, 1);
AudioConnection          patchCord405(filterLfo, 0, filterModMixer5, 1);
AudioConnection          patchCord406(filterLfo, 0, filterModMixer6, 1);
AudioConnection          patchCord407(filterLfo, 0, filterModMixer7, 1);
AudioConnection          patchCord408(filterLfo, 0, filterModMixer8, 1);

AudioConnection          patchCord419(pitchLfo, 0, oscGlobalModMixer, 1);
AudioConnection          patchCord420(filterLfo, 0, oscGlobalModMixer, 2);

AudioConnection          patchCord421(pink, 0, noiseMixer, 0);
AudioConnection          patchCord422(white, 0, noiseMixer, 1);

// AudioEffectEnvelope --------------------------------------------------
AudioConnection          patchCord501(filterEnvelope1, 0, oscModMixer1a, 1);
AudioConnection          patchCord502(filterEnvelope1, 0, oscModMixer1b, 1);
AudioConnection          patchCord503(filterEnvelope2, 0, oscModMixer2a, 1);
AudioConnection          patchCord504(filterEnvelope2, 0, oscModMixer2b, 1);
AudioConnection          patchCord505(filterEnvelope3, 0, oscModMixer3a, 1);
AudioConnection          patchCord506(filterEnvelope3, 0, oscModMixer3b, 1);
AudioConnection          patchCord507(filterEnvelope4, 0, oscModMixer4a, 1);
AudioConnection          patchCord508(filterEnvelope4, 0, oscModMixer4b, 1);
AudioConnection          patchCord509(filterEnvelope5, 0, oscModMixer5a, 1);
AudioConnection          patchCord510(filterEnvelope5, 0, oscModMixer5b, 1);
AudioConnection          patchCord511(filterEnvelope6, 0, oscModMixer6a, 1);
AudioConnection          patchCord512(filterEnvelope6, 0, oscModMixer6b, 1);
AudioConnection          patchCord513(filterEnvelope7, 0, oscModMixer7a, 1);
AudioConnection          patchCord514(filterEnvelope7, 0, oscModMixer7b, 1);
AudioConnection          patchCord515(filterEnvelope8, 0, oscModMixer8a, 1);
AudioConnection          patchCord516(filterEnvelope8, 0, oscModMixer8b, 1);

AudioConnection          patchCord551(filterEnvelope1, 0, pwMixer1a, 2);
AudioConnection          patchCord552(filterEnvelope1, 0, pwMixer1b, 2);
AudioConnection          patchCord553(filterEnvelope2, 0, pwMixer2a, 2);
AudioConnection          patchCord554(filterEnvelope2, 0, pwMixer2b, 2);
AudioConnection          patchCord555(filterEnvelope3, 0, pwMixer3a, 2);
AudioConnection          patchCord556(filterEnvelope3, 0, pwMixer3b, 2);
AudioConnection          patchCord557(filterEnvelope4, 0, pwMixer4a, 2);
AudioConnection          patchCord558(filterEnvelope4, 0, pwMixer4b, 2);
AudioConnection          patchCord559(filterEnvelope5, 0, pwMixer5a, 2);
AudioConnection          patchCord560(filterEnvelope5, 0, pwMixer5b, 2);
AudioConnection          patchCord561(filterEnvelope6, 0, pwMixer6a, 2);
AudioConnection          patchCord562(filterEnvelope6, 0, pwMixer6b, 2);
AudioConnection          patchCord563(filterEnvelope7, 0, pwMixer7a, 2);
AudioConnection          patchCord564(filterEnvelope7, 0, pwMixer7b, 2);
AudioConnection          patchCord565(filterEnvelope8, 0, pwMixer8a, 2);
AudioConnection          patchCord566(filterEnvelope8, 0, pwMixer8b, 2);

AudioConnection          patchCord581(filterEnvelope1, 0, filterModMixer1, 0);
AudioConnection          patchCord582(filterEnvelope2, 0, filterModMixer2, 0);
AudioConnection          patchCord583(filterEnvelope3, 0, filterModMixer3, 0);
AudioConnection          patchCord584(filterEnvelope4, 0, filterModMixer4, 0);
AudioConnection          patchCord585(filterEnvelope5, 0, filterModMixer5, 0);
AudioConnection          patchCord586(filterEnvelope6, 0, filterModMixer6, 0);
AudioConnection          patchCord587(filterEnvelope7, 0, filterModMixer7, 0);
AudioConnection          patchCord588(filterEnvelope8, 0, filterModMixer8, 0);

AudioConnection          patchCord631(ampEnvelope1, 0, voiceMixer1, 0);
AudioConnection          patchCord632(ampEnvelope2, 0, voiceMixer1, 1);
AudioConnection          patchCord633(ampEnvelope3, 0, voiceMixer1, 2);
AudioConnection          patchCord634(ampEnvelope4, 0, voiceMixer1, 3);
AudioConnection          patchCord635(ampEnvelope5, 0, voiceMixer2, 0);
AudioConnection          patchCord636(ampEnvelope6, 0, voiceMixer2, 1);
AudioConnection          patchCord637(ampEnvelope7, 0, voiceMixer2, 2);
AudioConnection          patchCord638(ampEnvelope8, 0, voiceMixer2, 3);

// AudioEffectDigitalCombine --------------------------------------------
AudioConnection          patchCord651(oscFX1, 0, waveformMixer1, 3);
AudioConnection          patchCord652(oscFX2, 0, waveformMixer2, 3);
AudioConnection          patchCord653(oscFX4, 0, waveformMixer4, 3);
AudioConnection          patchCord654(oscFX3, 0, waveformMixer3, 3);
AudioConnection          patchCord655(oscFX5, 0, waveformMixer5, 3);
AudioConnection          patchCord656(oscFX6, 0, waveformMixer6, 3);
AudioConnection          patchCord657(oscFX7, 0, waveformMixer7, 3);
AudioConnection          patchCord658(oscFX8, 0, waveformMixer8, 3);

// AudioFilter ----------------------------------------------------
#if Filter == 1
AudioConnection          patchCord701(filter1, 0, filterMixer1, 0);	// State Variable Filter
AudioConnection          patchCord702(filter1, 1, filterMixer1, 1);
AudioConnection          patchCord703(filter1, 2, filterMixer1, 2);
AudioConnection          patchCord704(filter2, 0, filterMixer2, 0);
AudioConnection          patchCord705(filter2, 1, filterMixer2, 1);
AudioConnection          patchCord706(filter2, 2, filterMixer2, 2);
AudioConnection          patchCord707(filter3, 0, filterMixer3, 0);
AudioConnection          patchCord708(filter3, 1, filterMixer3, 1);
AudioConnection          patchCord709(filter3, 2, filterMixer3, 2);
AudioConnection          patchCord710(filter4, 0, filterMixer4, 0);
AudioConnection          patchCord711(filter4, 1, filterMixer4, 1);
AudioConnection          patchCord712(filter4, 2, filterMixer4, 2);
AudioConnection          patchCord713(filter5, 0, filterMixer5, 0);
AudioConnection          patchCord714(filter5, 1, filterMixer5, 1);
AudioConnection          patchCord715(filter5, 2, filterMixer5, 2);
AudioConnection          patchCord716(filter6, 0, filterMixer6, 0);
AudioConnection          patchCord717(filter6, 1, filterMixer6, 1);
AudioConnection          patchCord718(filter6, 2, filterMixer6, 2);
AudioConnection          patchCord719(filter7, 0, filterMixer7, 0);
AudioConnection          patchCord720(filter7, 1, filterMixer7, 1);
AudioConnection          patchCord721(filter7, 2, filterMixer7, 2);
AudioConnection          patchCord722(filter8, 0, filterMixer8, 0);
AudioConnection          patchCord723(filter8, 1, filterMixer8, 1);
AudioConnection          patchCord724(filter8, 2, filterMixer8, 2);
#else
AudioConnection          patchCord701(filter1, 0, filterMixer1, 0);
//AudioConnection          patchCord702(filter1, 1, filterMixer1, 1);
//AudioConnection          patchCord703(filter1, 2, filterMixer1, 2);
AudioConnection          patchCord704(filter2, 0, filterMixer2, 0);
//AudioConnection          patchCord708(filter2, 1, filterMixer2, 1);
//AudioConnection          patchCord706(filter2, 2, filterMixer2, 2);
AudioConnection          patchCord707(filter3, 0, filterMixer3, 0);
//AudioConnection          patchCord708(filter3, 1, filterMixer3, 1);
//AudioConnection          patchCord709(filter3, 2, filterMixer3, 2);
AudioConnection          patchCord710(filter4, 0, filterMixer4, 0);
//AudioConnection          patchCord711(filter4, 1, filterMixer4, 1);
//AudioConnection          patchCord712(filter4, 2, filterMixer4, 2);
AudioConnection          patchCord713(filter5, 0, filterMixer5, 0);
//AudioConnection          patchCord714(filter5, 1, filterMixer5, 1);
//AudioConnection          patchCord715(filter5, 2, filterMixer5, 2);
AudioConnection          patchCord716(filter6, 0, filterMixer6, 0);
//AudioConnection          patchCord717(filter6, 1, filterMixer6, 1);
//AudioConnection          patchCord718(filter6, 2, filterMixer6, 2);
AudioConnection          patchCord719(filter7, 0, filterMixer7, 0);
//AudioConnection          patchCord720(filter7, 1, filterMixer7, 1);
//AudioConnection          patchCord721(filter7, 2, filterMixer7, 2);
AudioConnection          patchCord722(filter8, 0, filterMixer8, 0);
//AudioConnection          patchCord723(filter8, 1, filterMixer8, 1);
//AudioConnection          patchCord724(filter8, 2, filterMixer8, 2);
#endif

// Mixer ----------------------------------------------------------------
AudioConnection          patchCord751(noiseMixer, 0, waveformMixer1, 2);
AudioConnection          patchCord752(noiseMixer, 0, waveformMixer2, 2);
AudioConnection          patchCord753(noiseMixer, 0, waveformMixer3, 2);
AudioConnection          patchCord754(noiseMixer, 0, waveformMixer4, 2);
AudioConnection          patchCord755(noiseMixer, 0, waveformMixer5, 2);
AudioConnection          patchCord756(noiseMixer, 0, waveformMixer6, 2);
AudioConnection          patchCord757(noiseMixer, 0, waveformMixer7, 2);
AudioConnection          patchCord758(noiseMixer, 0, waveformMixer8, 2);

AudioConnection          patchCord771(pwMixer1a, 0, waveformMod1a, 1);
AudioConnection          patchCord772(pwMixer1b, 0, waveformMod1b, 1);
AudioConnection          patchCord773(pwMixer3b, 0, waveformMod3b, 1);
AudioConnection          patchCord774(pwMixer4b, 0, waveformMod4b, 1);
AudioConnection          patchCord775(pwMixer4a, 0, waveformMod4a, 1);
AudioConnection          patchCord776(pwMixer2a, 0, waveformMod2a, 1);
AudioConnection          patchCord777(pwMixer2b, 0, waveformMod2b, 1);
AudioConnection          patchCord778(pwMixer3a, 0, waveformMod3a, 1);
AudioConnection          patchCord779(pwMixer5a, 0, waveformMod5a, 1);
AudioConnection          patchCord780(pwMixer5b, 0, waveformMod5b, 1);
AudioConnection          patchCord781(pwMixer6a, 0, waveformMod6a, 1);
AudioConnection          patchCord782(pwMixer6b, 0, waveformMod6b, 1);
AudioConnection          patchCord783(pwMixer7a, 0, waveformMod7a, 1);
AudioConnection          patchCord784(pwMixer7b, 0, waveformMod7b, 1);
AudioConnection          patchCord785(pwMixer8a, 0, waveformMod8a, 1);
AudioConnection          patchCord786(pwMixer8b, 0, waveformMod8b, 1);

AudioConnection          patchCord801(oscGlobalModMixer, 0, oscModMixer1a, 0);
AudioConnection          patchCord802(oscGlobalModMixer, 0, oscModMixer1b, 0);
AudioConnection          patchCord803(oscGlobalModMixer, 0, oscModMixer2a, 0);
AudioConnection          patchCord804(oscGlobalModMixer, 0, oscModMixer2b, 0);
AudioConnection          patchCord805(oscGlobalModMixer, 0, oscModMixer3a, 0);
AudioConnection          patchCord806(oscGlobalModMixer, 0, oscModMixer3b, 0);
AudioConnection          patchCord807(oscGlobalModMixer, 0, oscModMixer4a, 0);
AudioConnection          patchCord808(oscGlobalModMixer, 0, oscModMixer4b, 0);
AudioConnection          patchCord809(oscGlobalModMixer, 0, oscModMixer5a, 0);
AudioConnection          patchCord810(oscGlobalModMixer, 0, oscModMixer5b, 0);
AudioConnection          patchCord811(oscGlobalModMixer, 0, oscModMixer6a, 0);
AudioConnection          patchCord812(oscGlobalModMixer, 0, oscModMixer6b, 0);
AudioConnection          patchCord813(oscGlobalModMixer, 0, oscModMixer7a, 0);
AudioConnection          patchCord814(oscGlobalModMixer, 0, oscModMixer7b, 0);
AudioConnection          patchCord815(oscGlobalModMixer, 0, oscModMixer8a, 0);
AudioConnection          patchCord816(oscGlobalModMixer, 0, oscModMixer8b, 0);

AudioConnection          patchCord831(filterModMixer1, 0, filter1, 1);
AudioConnection          patchCord832(filterModMixer2, 0, filter2, 1);
AudioConnection          patchCord833(filterModMixer3, 0, filter3, 1);
AudioConnection          patchCord834(filterModMixer4, 0, filter4, 1);
AudioConnection          patchCord835(filterModMixer5, 0, filter5, 1);
AudioConnection          patchCord836(filterModMixer6, 0, filter6, 1);
AudioConnection          patchCord837(filterModMixer7, 0, filter7, 1);
AudioConnection          patchCord838(filterModMixer8, 0, filter8, 1);

// waveshaper
AudioConnection          patchCord841(filterMixer1, WaveshaperAmp1);
AudioConnection          patchCord842(filterMixer2, WaveshaperAmp2);
AudioConnection          patchCord843(filterMixer3, WaveshaperAmp3);
AudioConnection          patchCord844(filterMixer4, WaveshaperAmp4);
AudioConnection          patchCord845(filterMixer5, WaveshaperAmp5);
AudioConnection          patchCord846(filterMixer6, WaveshaperAmp6);
AudioConnection          patchCord847(filterMixer7, WaveshaperAmp7);
AudioConnection          patchCord848(filterMixer8, WaveshaperAmp8);

AudioConnection          patchCord849(WaveshaperAmp1, waveshaper1);
AudioConnection          patchCord850(WaveshaperAmp2, waveshaper2);
AudioConnection          patchCord851(WaveshaperAmp3, waveshaper3);
AudioConnection          patchCord852(WaveshaperAmp4, waveshaper4);
AudioConnection          patchCord853(WaveshaperAmp5, waveshaper5);
AudioConnection          patchCord854(WaveshaperAmp6, waveshaper6);
AudioConnection          patchCord855(WaveshaperAmp7, waveshaper7);
AudioConnection          patchCord856(WaveshaperAmp8, waveshaper8);

AudioConnection          patchCord857(waveshaper1, ampEnvelope1);
AudioConnection          patchCord858(waveshaper2, ampEnvelope2);
AudioConnection          patchCord859(waveshaper3, ampEnvelope3);
AudioConnection          patchCord860(waveshaper4, ampEnvelope4);
AudioConnection          patchCord861(waveshaper5, ampEnvelope5);
AudioConnection          patchCord862(waveshaper6, ampEnvelope6);
AudioConnection          patchCord863(waveshaper7, ampEnvelope7);
AudioConnection          patchCord864(waveshaper8, ampEnvelope8);

AudioConnection          patchCord871(oscModMixer1a, 0, waveformMod1a, 0);
AudioConnection          patchCord872(oscModMixer1b, 0, waveformMod1b, 0);
AudioConnection          patchCord873(oscModMixer2a, 0, waveformMod2a, 0);
AudioConnection          patchCord874(oscModMixer2b, 0, waveformMod2b, 0);
AudioConnection          patchCord875(oscModMixer3a, 0, waveformMod3a, 0);
AudioConnection          patchCord876(oscModMixer3b, 0, waveformMod3b, 0);
AudioConnection          patchCord877(oscModMixer4a, 0, waveformMod4a, 0);
AudioConnection          patchCord878(oscModMixer4b, 0, waveformMod4b, 0);
AudioConnection          patchCord879(oscModMixer5a, 0, waveformMod5a, 0);
AudioConnection          patchCord880(oscModMixer5b, 0, waveformMod5b, 0);
AudioConnection          patchCord881(oscModMixer6a, 0, waveformMod6a, 0);
AudioConnection          patchCord882(oscModMixer6b, 0, waveformMod6b, 0);
AudioConnection          patchCord883(oscModMixer7a, 0, waveformMod7a, 0);
AudioConnection          patchCord884(oscModMixer7b, 0, waveformMod7b, 0);
AudioConnection          patchCord885(oscModMixer8a, 0, waveformMod8a, 0);
AudioConnection          patchCord886(oscModMixer8b, 0, waveformMod8b, 0);

AudioConnection          patchCord941(waveformMixer1, 0, filter1, 0);
AudioConnection          patchCord942(waveformMixer2, 0, filter2, 0);
AudioConnection          patchCord943(waveformMixer3, 0, filter3, 0);
AudioConnection          patchCord944(waveformMixer4, 0, filter4, 0);
AudioConnection          patchCord945(waveformMixer5, 0, filter5, 0);
AudioConnection          patchCord946(waveformMixer6, 0, filter6, 0);
AudioConnection          patchCord947(waveformMixer7, 0, filter7, 0);
AudioConnection          patchCord948(waveformMixer8, 0, filter8, 0);

AudioConnection			patchCord960(voiceMixerM, 0, dcOffsetFilter, 0);
AudioConnection			patchCord923(dcOffsetFilter, 2, scope, 0);
AudioConnection			patchCord924(dcOffsetFilter, 2, peak, 0);

AudioConnection          patchCord950(voiceMixer1, 0, voiceMixerM, 0);
AudioConnection          patchCord951(voiceMixer2, 0, voiceMixerM, 1);
AudioConnection          patchCord952(constant1Dc, 0, voiceMixerM, 2);

AudioConnection          patchCord998(voiceMixerM, 0, PCM5102A, 0);
AudioConnection          patchCord999(voiceMixerM, 0, PCM5102A, 1);

