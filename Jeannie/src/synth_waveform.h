/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.

 ElectroTechnique 2020
 Added WAVEFORM_SILENT, sync()
 */

#ifndef synth_waveform_h_
#define synth_waveform_h_

#include <Arduino.h>
#include "AudioStream.h"
#include "arm_math.h"

// waveforms.c
extern "C"
{
  extern const int16_t AudioWaveformSine[257];
}

// extern int8_t Lfo3Modoutput;

// Waveforms
#define WAVEFORM_SINE 0
#define WAVEFORM_TRIANGLE 1
#define WAVEFORM_SAWTOOTH 3
#define WAVEFORM_SQUARE 2
#define WAVEFORM_PULSE 4
#define WAVEFORM_SAWTOOTH_REVERSE 5
#define WAVEFORM_SAMPLE_HOLD 6
#define WAVEFORM_TRIANGLE_VARIABLE 7
#define WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE 8
#define WAVEFORM_BANDLIMIT_SAWTOOTH 9
#define WAVEFORM_BANDLIMIT_SQUARE 10
#define WAVEFORM_BANDLIMIT_PULSE 11
#define WAVEFORM_ARBITRARY 12
#define WAVEFORM_SILENT 19

// LFO and PWM waveforms
#define LFO_WAVEFORM_ARBITRARY 20
#define LFO_WAVEFORM_SAMPLE_HOLD 21
#define PWM_WAVEFORM_SINE 26
#define PWM_WAVEFORM_TRIANGLE 27
#define PWM_WAVEFORM_SAWTOOTH 28
#define PWM_WAVEFORM_SQUARE 29

// Special waveforms
#define WAVEFORM_MULTISAW 30
#define WAVEFORM_BRAIDS_VOWEL 31
#define WAVEFORM_SHRUTHI_ZSAW 32
#define WAVEFORM_SHRUTHI_ZSYNC 33
#define WAVEFORM_SHRUTHI_ZTRI 34
#define WAVEFORM_SHRUTHI_ZRESO 35
#define WAVEFORM_SHRUTHI_ZPULSE 36
#define WAVEFORM_SHRUTHI_CRUSHED_SINE 37



/*
const uint8_t wav_res_sine[] {
	 2,      2,      2,      3,      2,      3,      3,      4,
	 5,      4,      7,      5,      9,      7,     10,     11,
	 11,     13,     13,     17,     16,     18,     21,     21,
	 23,     25,     27,     28,     32,     31,     36,     36,
	 39,     41,     43,     46,     48,     51,     53,     55,
	 57,     62,     63,     65,     70,     70,     75,     76,
	 81,     82,     85,     89,     92,     94,     97,    100,
	 104,    107,    109,    112,    116,    119,    122,    124,
	 129,    130,    135,    137,    140,    144,    147,    148,
	 154,    155,    158,    163,    163,    169,    169,    174,
	 177,    178,    182,    185,    187,    191,    192,    195,
	 199,    200,    203,    205,    209,    210,    212,    216,
	 216,    220,    221,    223,    226,    227,    230,    230,
	 233,    235,    235,    239,    238,    241,    242,    243,
	 245,    245,    246,    249,    247,    251,    249,    252,
	 251,    252,    253,    253,    253,    254,    254,    254,
	 253,    255,    254,    253,    253,    254,    252,    253,
	 251,    251,    250,    250,    248,    248,    246,    246,
	 245,    242,    243,    240,    239,    238,    236,    234,
	 233,    231,    230,    226,    226,    224,    221,    219,
	 217,    215,    212,    211,    208,    206,    202,    201,
	 198,    196,    192,    190,    188,    184,    182,    179,
	 177,    173,    170,    168,    164,    162,    158,    156,
	 153,    149,    147,    143,    141,    136,    135,    131,
	 128,    125,    122,    118,    116,    113,    109,    106,
	 104,    101,     96,     95,     92,     88,     85,     83,
	 80,     77,     74,     71,     69,     66,     63,     61,
	 57,     56,     53,     50,     49,     45,     43,     42,
	 38,     37,     35,     32,     31,     29,     26,     26,
	 22,     22,     20,     19,     16,     16,     13,     14,
	 11,     10,     10,      8,      8,      6,      6,      5,
	 4,      5,      2,      4,      2,      2,      2,      3,
	 1
};
*/

const int8_t wav_res_formant_square[256] = {
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16
};

const int8_t wav_res_formant_sine[256] = {
	0,      0,      0,      0,      0,      0,      0,      0,
	0,      0,      0,      0,      0,      0,      0,      0,
	0,      2,      2,      3,      3,      4,      5,      6,
	7,      8,     10,     12,     14,     17,     20,     24,
	0,      3,      4,      5,      6,      7,      9,     10,
	12,     15,     18,     21,     26,     31,     37,     45,
	0,      4,      5,      6,      8,      9,     11,     13,
	16,     19,     23,     28,     34,     40,     49,     58,
	0,      5,      6,      7,      8,     10,     12,     15,
	17,     21,     25,     30,     36,     44,     53,     63,
	0,      4,      5,      6,      8,      9,     11,     13,
	16,     19,     23,     28,     34,     40,     49,     58,
	0,      3,      4,      5,      6,      7,      9,     10,
	12,     15,     18,     21,     26,     31,     37,     45,
	0,      2,      2,      3,      3,      4,      5,      6,
	7,      8,     10,     12,     14,     17,     20,     24,
	0,      0,      0,      0,      0,      0,      0,      0,
	0,      0,      0,      0,      0,      0,      0,      0,
	0,     -2,     -2,     -3,     -3,     -4,     -5,     -6,
	-7,     -8,    -10,    -12,    -14,    -17,    -20,    -24,
	0,     -3,     -4,     -5,     -6,     -7,     -9,    -10,
	-12,    -15,    -18,    -21,    -26,    -31,    -37,    -45,
	0,     -4,     -5,     -6,     -8,     -9,    -11,    -13,
	-16,    -19,    -23,    -28,    -34,    -40,    -49,    -58,
	0,     -5,     -6,     -7,     -8,    -10,    -12,    -15,
	-17,    -21,    -25,    -30,    -36,    -44,    -53,    -63,
	0,     -4,     -5,     -6,     -8,     -9,    -11,    -13,
	-16,    -19,    -23,    -28,    -34,    -40,    -49,    -58,
	0,     -3,     -4,     -5,     -6,     -7,     -9,    -10,
	-12,    -15,    -18,    -21,    -26,    -31,    -37,    -45,
	0,     -2,     -2,     -3,     -3,     -4,     -5,     -6,
	-7,     -8,    -10,    -12,    -14,    -17,    -20,    -24
};

const uint16_t wav_res_sine16[] = {
		0,    10,	 39,    88,   157,   245,   352,   479,
	  625,   790,   974,  1178,  1400,  1641,  1901,  2179,
	 2475,  2790,  3122,  3472,  3840,  4225,  4627,  5045,
	 5481,  5932,  6400,  6884,  7382,  7897,  8426,  8969,
     9527, 10098, 10684, 11282, 11893, 12517, 13153, 13800,
    14459, 15129, 15809, 16500, 17200, 17909, 18628, 19355,
    20090, 20832, 21582, 22338, 23100, 23869, 24642, 25421,
    26203, 26990, 27780, 28574, 29369, 30167, 30966, 31767,
    32568, 33369, 34170, 34969, 35768, 36565, 37359, 38151,
    38940, 39724, 40505, 41281, 42052, 42818, 43577, 44330,
    45076, 45815, 46546, 47268, 47982, 48687, 49383, 50068,
    50743, 51408, 52061, 52703, 53332, 53950, 54555, 55147,
    55725, 56290, 56840, 57377, 57898, 58405, 58896, 59372,
    59831, 60275, 60702, 61112, 61506, 61882, 62241, 62582,
    62906, 63211, 63499, 63767, 64018, 64249, 64462, 64656,
    64831, 64987, 65123, 65240, 65338, 65416, 65475, 65514,
    65534, 65534, 65514, 65475, 65416, 65338, 65240, 65123,
    64987, 64831, 64656, 64462, 64249, 64018, 63767, 63499,
    63211, 62906, 62582, 62241, 61882, 61506, 61112, 60702,
    60275, 59831, 59372, 58896, 58405, 57898, 57377, 56840,
    56290, 55725, 55147, 54555, 53950, 53332, 52703, 52061,
    51408, 50743, 50068, 49383, 48687, 47982, 47268, 46546,
    45815, 45076, 44330, 43577, 42818, 42052, 41281, 40505,
    39724, 38940, 38151, 37359, 36565, 35768, 34969, 34170,
    33369, 32568, 31767, 30966, 30167, 29369, 28574, 27780,
    26990, 26203, 25421, 24642, 23869, 23100, 22338, 21582,
    20832, 20090, 19355, 18628, 17909, 17200, 16500, 15809,
    15129, 14459, 13800, 13153, 12517, 11893, 11282, 10684,
    10098,  9527,  8969,  8426,  7897,  7382,  6884,  6400,
     5932,  5481,  5045,  4627,  4225,  3840,  3472,  3122,
     2790,  2475,  2179,  1901,  1641,  1400,  1178,   974,
      790,   625,   479,   352,   245,   157,    88,    39,
	   10
};

const uint8_t wav_res_vowel_data[] = {
	27,     40,     89,     15,     13,      1,      0,     18,
	51,     62,     13,     12,      6,      0,     15,     69,
	93,     14,     12,      7,      0,     10,     84,    110,
	13,     10,      8,      0,     23,     44,     87,     15,
	12,      1,      0,     13,     29,     80,     13,      8,
	0,      0,      6,     46,     81,     12,      3,      0,
	0,      9,     51,     95,     15,      3,      0,      3,
	6,     73,     99,      7,      3,     14,      9,
};

const int16_t wav_formant_square[] = {
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,      1,      1,      2,      2,      3,      3,      4,
	4,      5,      6,      8,      9,     11,     13,     16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
	0,     -1,     -1,     -2,     -2,     -3,     -3,     -4,
	-4,     -5,     -6,     -8,     -9,    -11,    -13,    -16,
};

const int16_t wav_formant_sine[] = {
	0,      0,      0,      0,      0,      0,      0,      0,
	0,      0,      0,      0,      0,      0,      0,      0,
	0,      2,      2,      3,      3,      4,      5,      6,
	7,      8,     10,     12,     14,     17,     20,     24,
	0,      3,      4,      5,      6,      7,      9,     10,
	12,     15,     18,     21,     26,     31,     37,     45,
	0,      4,      5,      6,      8,      9,     11,     13,
	16,     19,     23,     28,     34,     40,     49,     58,
	0,      5,      6,      7,      8,     10,     12,     15,
	17,     21,     25,     30,     36,     44,     53,     63,
	0,      4,      5,      6,      8,      9,     11,     13,
	16,     19,     23,     28,     34,     40,     49,     58,
	0,      3,      4,      5,      6,      7,      9,     10,
	12,     15,     18,     21,     26,     31,     37,     45,
	0,      2,      2,      3,      3,      4,      5,      6,
	7,      8,     10,     12,     14,     17,     20,     24,
	0,      0,      0,      0,      0,      0,      0,      0,
	0,      0,      0,      0,      0,      0,      0,      0,
	0,     -2,     -2,     -3,     -3,     -4,     -5,     -6,
	-7,     -8,    -10,    -12,    -14,    -17,    -20,    -24,
	0,     -3,     -4,     -5,     -6,     -7,     -9,    -10,
	-12,    -15,    -18,    -21,    -26,    -31,    -37,    -45,
	0,     -4,     -5,     -6,     -8,     -9,    -11,    -13,
	-16,    -19,    -23,    -28,    -34,    -40,    -49,    -58,
	0,     -5,     -6,     -7,     -8,    -10,    -12,    -15,
	-17,    -21,    -25,    -30,    -36,    -44,    -53,    -63,
	0,     -4,     -5,     -6,     -8,     -9,    -11,    -13,
	-16,    -19,    -23,    -28,    -34,    -40,    -49,    -58,
	0,     -3,     -4,     -5,     -6,     -7,     -9,    -10,
	-12,    -15,    -18,    -21,    -26,    -31,    -37,    -45,
	0,     -2,     -2,     -3,     -3,     -4,     -5,     -6,
	-7,     -8,    -10,    -12,    -14,    -17,    -20,    -24,
};

struct PhonemeDefinition {
	uint8_t formant_frequency[3];
	uint8_t formant_amplitude[3];
};

static const PhonemeDefinition vowels_data[9] = {
	{ { 27,  40,  89 }, { 15,  13,  1 } },
	{ { 18,  51,  62 }, { 13,  12,  6 } },
	{ { 15,  69,  93 }, { 14,  12,  7 } },
	{ { 10,  84, 110 }, { 13,  10,  8 } },
	{ { 23,  44,  87 }, { 15,  12,  1 } },
	{ { 13,  29,  80 }, { 13,   8,  0 } },
	{ {  6,  46,  81 }, { 12,   3,  0 } },
	{ {  9,  51,  95 }, { 15,   3,  0 } },
	{ {  6,  73,  99 }, {  7,   3,  14 } }
};

static const PhonemeDefinition consonant_data[8] = {
	{ { 6, 54, 121 }, { 9,  9,  0 } },
	{ { 18, 50, 51 }, { 12,  10,  5 } },
	{ { 11, 24, 70 }, { 13,  8,  0 } },
	{ { 15, 69, 74 }, { 14,  12,  7 } },
	{ { 16, 37, 111 }, { 14,  8,  1 } },
	{ { 18, 51, 62 }, { 14,  12,  6 } },
	{ { 6, 26, 81 }, { 5,  5,  5 } },
	{ { 6, 73, 99 }, { 7,  10,  14 } },
};

 static const size_t kNumFormants = 5;
 

typedef struct step_state
{
  int offset ;
  bool positive ;
} step_state ;


class BandLimitedWaveform
{
public:
  BandLimitedWaveform (void) ;
  int16_t generate_sawtooth (uint32_t new_phase, int i) ;
  int16_t generate_square (uint32_t new_phase, int i) ;
  int16_t generate_pulse (uint32_t new_phase, uint32_t pulse_width, int i) ;
  void init_sawtooth (uint32_t freq_word) ;
  void init_square (uint32_t freq_word) ;
  void init_pulse (uint32_t freq_word, uint32_t pulse_width) ;
  

private:
  int32_t lookup (int offset) ;
  void insert_step (int offset, bool rising, int i) ;
  int32_t process_step (int i) ;
  int32_t process_active_steps (uint32_t new_phase) ;
  int32_t process_active_steps_saw (uint32_t new_phase) ;
  int32_t process_active_steps_pulse (uint32_t new_phase, uint32_t pulse_width) ;
  void new_step_check_square (uint32_t new_phase, int i) ;
  void new_step_check_pulse (uint32_t new_phase, uint32_t pulse_width, int i) ;
  void new_step_check_saw (uint32_t new_phase, int i) ;
  
  
  uint32_t phase_word ;
  int32_t dc_offset ;
  step_state states [32] ; // circular buffer of active steps
  int newptr ;         // buffer pointers into states, AND'd with PTRMASK to keep in buffer range.
  int delptr ;
  int32_t  cyclic[16] ;    // circular buffer of output samples
  bool pulse_state ;
  uint32_t sampled_width ; // pulse width is sampled once per waveform
};


class AudioSynthWaveformTS : public AudioStream
{
public:
  AudioSynthWaveformTS(void) : AudioStream(0,NULL),
    phase_accumulator(0), phase_increment(0), phase_offset(0),
    magnitude(0), pulse_width(0x40000000),
    arbdata(NULL), sample(0), tone_type(WAVEFORM_SINE),
    tone_offset(0),syncFlag(0) {
  }

  void frequency(float freq) {
    if (freq < 0.0) {
      freq = 0.0;
    } else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2) {
      freq = AUDIO_SAMPLE_RATE_EXACT / 2;
    }
    phase_increment = freq * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
    if (phase_increment > 0x7FFE0000u) phase_increment = 0x7FFE0000;
  }
  void phase(float angle) {
    if (angle < 0.0) {
      angle = 0.0;
    } else if (angle > 360.0) {
      angle = angle - 360.0;
      if (angle >= 360.0) return;
    }
    phase_offset = angle * (4294967296.0 / 360.0);
  }
  void sync() {
    syncFlag = 1;
  }       
  void amplitude(float n) { // 0 to 1.0
    if (n < 0) {
      n = 0;
    } else if (n > 1.0) {
      n = 1.0;
    }
    magnitude = n * 65536.0;
  }
  void offset(float n) {
    if (n < -1.0) {
      n = -1.0;
    } else if (n > 1.0) {
      n = 1.0;
    }
    tone_offset = n * 32767.0;
  }
  void pulseWidth(float n) {  // 0.0 to 1.0
    if (n < 0) {
      n = 0;
    } else if (n > 1.0) {
      n = 1.0;
    }
    pulse_width = n * 4294967296.0;
  }
  void begin(short t_type) {
    phase_offset = 0;
    tone_type = t_type;
    if (t_type == WAVEFORM_BANDLIMIT_SQUARE)
      band_limit_waveform.init_square (phase_increment) ;
    else if (t_type == WAVEFORM_BANDLIMIT_PULSE)
      band_limit_waveform.init_pulse (phase_increment, pulse_width) ;
    else if (t_type == WAVEFORM_BANDLIMIT_SAWTOOTH || t_type == WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE)
      band_limit_waveform.init_sawtooth (phase_increment) ;
  }
  void begin(float t_amp, float t_freq, short t_type) {
    amplitude(t_amp);
    frequency(t_freq);
    phase_offset = 0;
    begin (t_type);
  }
  void arbitraryWaveform(const int16_t *data, float maxFreq) {
    arbdata = data;
  }
  void LFO_mode (uint8_t lfo_Mode) {
	  lfo_mode = lfo_Mode;
  }
  void LFO_phase (uint8_t lfo_Phase) {	// LFO parameter: SYN
	  lfo_phase = lfo_Phase;
  }
  void LFO_oneShoot (boolean lfo_OneShoot) {
	  lfo_oneShoot = lfo_OneShoot;
  }
  
  virtual void update(void);

private:
  uint32_t phase_accumulator;
  uint32_t phase_increment;
  uint32_t phase_offset;
  int32_t  magnitude;
  uint32_t pulse_width;
  const int16_t *arbdata;
  int16_t  sample; // for WAVEFORM_SAMPLE_HOLD
  int16_t  sample2; // for WAVEFORM_SAMPLE_HOLD and oneShot
  short  tone_type;
  int16_t tone_offset;
  int16_t syncFlag;
  BandLimitedWaveform band_limit_waveform;
  uint8_t lfo_mode;
  uint8_t lfo_phase;
  boolean lfo_oneShoot;
  uint8_t lfo1ph;
  boolean lfo_randomFlag;  
};


class AudioSynthWaveformModulatedTS : public AudioStream
{
public:
  AudioSynthWaveformModulatedTS(void) : AudioStream(4, inputQueueArray),
    phase_accumulator(0), phase_increment(0), modulation_factor(32768),
    magnitude(0), arbdata(NULL), sample(0), tone_offset(0),
    tone_type(WAVEFORM_SINE), modulation_type(0), syncFlag(0), osc_par_a(0),
	osc_par_b(0), par_a_mod_(0), par_b_mod_(0)
	 {
  }
  void frequency(float freq) {
    if (freq < 0.0) {
      freq = 0.0;
    } else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2) {
      freq = AUDIO_SAMPLE_RATE_EXACT / 2;
    }
    phase_increment = freq * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
    if (phase_increment > 0x7FFE0000u) phase_increment = 0x7FFE0000;
  }
  void amplitude(float n) { // 0 to 1.0
    if (n < 0) {
      n = 0;
    } else if (n > 1.0) {
      n = 1.0;
    }
    magnitude = n * 65536.0;
  }
   void sync() {
    syncFlag = 1;
  }              
  void offset(float n) {
    if (n < -1.0) {
      n = -1.0;
    } else if (n > 1.0) {
      n = 1.0;
    }
    tone_offset = n * 32767.0;
  }
  void begin(short t_type) {
    tone_type = t_type;
    if (t_type == WAVEFORM_BANDLIMIT_SQUARE)
      band_limit_waveform.init_square (phase_increment) ;
    else if (t_type == WAVEFORM_BANDLIMIT_PULSE)
      band_limit_waveform.init_pulse (phase_increment, 0x80000000u) ;
    else if (t_type == WAVEFORM_BANDLIMIT_SAWTOOTH || t_type == WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE)
      band_limit_waveform.init_sawtooth (phase_increment) ;
  }
  void begin(float t_amp, float t_freq, short t_type) {
    amplitude(t_amp);
    frequency(t_freq);
    begin (t_type) ;
  }
  void arbitraryWaveform(const int16_t *data, float maxFreq) {
    arbdata = data;
  }
  void frequencyModulation(float octaves) {
    if (octaves > 12.0) {
      octaves = 12.0;
    } else if (octaves < 0.1) {
      octaves = 0.1;
    }
    modulation_factor = octaves * 4096.0;
    modulation_type = 0;
  }
  void phaseModulation(float degrees) {
    if (degrees > 9000.0) {
      degrees = 9000.0;
    } else if (degrees < 30.0) {
      degrees = 30.0;
    }
    modulation_factor = degrees * (65536.0 / 180.0);
    modulation_type = 1;
  }
  void parameter_a(uint8_t Osc_par_a) {		// parameter_a im Osc Menu
	  osc_par_a = Osc_par_a;
  }
  void parameter_b(uint8_t Osc_par_b) {		// parameter_b im Osc Menu
	  osc_par_b = Osc_par_b;
  }
  
  virtual void update(void);
  
private:
  audio_block_t *inputQueueArray[4];
  uint32_t phase_accumulator;
  uint32_t phase_increment;
  uint32_t modulation_factor;
  int32_t  magnitude;
  const int16_t *arbdata;
  uint32_t phasedata[AUDIO_BLOCK_SAMPLES];
  int16_t  sample; // for WAVEFORM_SAMPLE_HOLD
  int16_t  tone_offset;
  uint8_t  tone_type;
  uint8_t  modulation_type;
  int16_t  syncFlag;
  uint32_t data_qs_phase[4];
  uint32_t data_qs_phase_2[4];
  uint8_t Osc_data_cr_decimate;
  uint16_t Osc_data_cr_state;
  uint16_t OscData_sec_phase;
  BandLimitedWaveform band_limit_waveform;
  uint8_t osc_par_a = 0;
  uint8_t osc_par_b = 0;
  uint8_t Osc_vw_update;
  uint32_t state_vow_formant_phase[3];
  uint32_t state_vow_formant_increment[3];
  uint32_t state_vow_formant_amplitude[3];
  boolean strike_;
  uint16_t state_vow_consonant_frames;
  uint32_t state_saw_phase[6];
  uint32_t state_saw_lp;
  uint32_t state_saw_bp;
  uint16_t Osc_vw1_formant_increment[3];
  uint16_t Osc_vw1_formant_amplitude[3];
  uint16_t Osc_vw1_formant_phase[3];
  uint32_t phaseOld_= 0;
  int16_t par_a_mod_ = 0;	// vowel parameter_A
  int16_t par_b_mod_ = 0;	// vowel parameter_B
};

#endif