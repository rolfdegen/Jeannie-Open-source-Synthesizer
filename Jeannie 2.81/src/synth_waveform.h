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

// Special Shruthi and Braids waveforms
#define WAVEFORM_MULTISAW 30
#define WAVEFORM_BRAIDS_VOWEL 31
#define WAVEFORM_SHRUTHI_ZSAW 32
#define WAVEFORM_SHRUTHI_ZSYNC 33
#define WAVEFORM_SHRUTHI_ZTRI 34
#define WAVEFORM_SHRUTHI_ZRESO 35
#define WAVEFORM_SHRUTHI_ZPULSE 36
#define WAVEFORM_SHRUTHI_CRUSHED_SINE 37
#define WAVEFORM_BRAIDS_CSAW 38
#define WAVEFORM_BRAIDS_VOSIM 39
#define WAVEFORM_BRAIDS_TOY 40
#define WAVEFORM_BRAIDS_SAWSWARM 41
#define WAVEFORM_BRAIDS_ZLPF 42
#define WAVEFORM_BRAIDS_ZPKF 43
#define WAVEFORM_BRAIDS_ZBPF 44
#define WAVEFORM_BRAIDS_ZHPF 45

const int16_t wav_sine[] = {
    -32512,
    -32502,
    -32473,
    -32423,
    -32356,
    -32265,
    -32160,
    -32031,
    -31885,
    -31719,
    -31533,
    -31331,
    -31106,
    -30864,
    -30605,
    -30324,
    -30028,
    -29712,
    -29379,
    -29026,
    -28658,
    -28272,
    -27868,
    -27449,
    -27011,
    -26558,
    -26089,
    -25604,
    -25103,
    -24588,
    -24056,
    -23512,
    -22953,
    -22378,
    -21793,
    -21191,
    -20579,
    -19954,
    -19316,
    -18667,
    -18006,
    -17334,
    -16654,
    -15960,
    -15259,
    -14548,
    -13828,
    -13100,
    -12363,
    -11620,
    -10868,
    -10112,
    -9347,
    -8578,
    -7805,
    -7023,
    -6241,
    -5453,
    -4662,
    -3868,
    -3073,
    -2274,
    -1474,
    -674,
    126,
    929,
    1729,
    2527,
    3326,
    4123,
    4916,
    5707,
    6495,
    7278,
    8057,
    8833,
    9601,
    10366,
    11122,
    11874,
    12618,
    13353,
    14082,
    14802,
    15512,
    16216,
    16906,
    17589,
    18260,
    18922,
    19569,
    20207,
    20834,
    21446,
    22045,
    22634,
    23206,
    23765,
    24311,
    24842,
    25357,
    25858,
    26343,
    26812,
    27266,
    27701,
    28123,
    28526,
    28912,
    29281,
    29632,
    29966,
    30281,
    30579,
    30859,
    31118,
    31361,
    31583,
    31788,
    31973,
    32139,
    32286,
    32412,
    32521,
    32608,
    32679,
    32725,
    32757,
    32766,
    32757,
    32725,
    32679,
    32608,
    32521,
    32412,
    32286,
    32139,
    31973,
    31788,
    31583,
    31361,
    31118,
    30859,
    30579,
    30281,
    29966,
    29632,
    29281,
    28912,
    28526,
    28123,
    27701,
    27266,
    26812,
    26343,
    25858,
    25357,
    24842,
    24311,
    23765,
    23206,
    22634,
    22045,
    21446,
    20834,
    20207,
    19569,
    18922,
    18260,
    17589,
    16906,
    16216,
    15512,
    14802,
    14082,
    13353,
    12618,
    11874,
    11122,
    10366,
    9601,
    8833,
    8057,
    7278,
    6495,
    5707,
    4916,
    4123,
    3326,
    2527,
    1729,
    929,
    126,
    -674,
    -1474,
    -2274,
    -3073,
    -3868,
    -4662,
    -5453,
    -6241,
    -7023,
    -7805,
    -8578,
    -9347,
    -10112,
    -10868,
    -11620,
    -12363,
    -13100,
    -13828,
    -14548,
    -15259,
    -15960,
    -16654,
    -17334,
    -18006,
    -18667,
    -19316,
    -19954,
    -20579,
    -21191,
    -21793,
    -22378,
    -22953,
    -23512,
    -24056,
    -24588,
    -25103,
    -25604,
    -26089,
    -26558,
    -27011,
    -27449,
    -27868,
    -28272,
    -28658,
    -29026,
    -29379,
    -29712,
    -30028,
    -30324,
    -30605,
    -30864,
    -31106,
    -31331,
    -31533,
    -31719,
    -31885,
    -32031,
    -32160,
    -32265,
    -32356,
    -32423,
    -32473,
    -32502,
    -32512,
};

const uint16_t lut_bell[] = {
    0,
    670,
    2655,
    5873,
    10191,
    15434,
    21387,
    27805,
    34427,
    40980,
    47198,
    52824,
    57630,
    61417,
    64032,
    65366,
    65534,
    65528,
    65517,
    65500,
    65477,
    65449,
    65415,
    65376,
    65331,
    65280,
    65224,
    65162,
    65095,
    65022,
    64944,
    64860,
    64770,
    64675,
    64574,
    64468,
    64357,
    64240,
    64118,
    63990,
    63857,
    63718,
    63575,
    63426,
    63271,
    63112,
    62947,
    62777,
    62602,
    62421,
    62236,
    62046,
    61850,
    61650,
    61444,
    61234,
    61018,
    60798,
    60573,
    60343,
    60109,
    59870,
    59626,
    59377,
    59124,
    58866,
    58604,
    58338,
    58067,
    57791,
    57512,
    57228,
    56940,
    56648,
    56351,
    56051,
    55746,
    55438,
    55126,
    54810,
    54490,
    54166,
    53839,
    53508,
    53173,
    52835,
    52494,
    52149,
    51801,
    51449,
    51094,
    50736,
    50375,
    50011,
    49645,
    49275,
    48902,
    48526,
    48148,
    47767,
    47384,
    46998,
    46610,
    46219,
    45826,
    45431,
    45033,
    44633,
    44232,
    43828,
    43423,
    43015,
    42606,
    42195,
    41783,
    41369,
    40953,
    40537,
    40118,
    39699,
    39278,
    38856,
    38433,
    38010,
    37585,
    37159,
    36733,
    36306,
    35879,
    35450,
    35022,
    34593,
    34163,
    33734,
    33304,
    32874,
    32445,
    32015,
    31585,
    31156,
    30727,
    30298,
    29869,
    29442,
    29014,
    28588,
    28162,
    27737,
    27312,
    26889,
    26467,
    26045,
    25625,
    25206,
    24789,
    24373,
    23958,
    23545,
    23133,
    22723,
    22315,
    21908,
    21504,
    21101,
    20700,
    20302,
    19905,
    19511,
    19119,
    18730,
    18343,
    17958,
    17576,
    17196,
    16819,
    16445,
    16074,
    15706,
    15340,
    14978,
    14618,
    14262,
    13909,
    13559,
    13212,
    12869,
    12529,
    12193,
    11860,
    11531,
    11206,
    10884,
    10566,
    10252,
    9941,
    9635,
    9332,
    9034,
    8740,
    8450,
    8164,
    7882,
    7604,
    7331,
    7062,
    6798,
    6538,
    6283,
    6032,
    5786,
    5544,
    5307,
    5075,
    4848,
    4625,
    4407,
    4195,
    3987,
    3784,
    3586,
    3393,
    3205,
    3022,
    2844,
    2671,
    2504,
    2342,
    2185,
    2033,
    1887,
    1746,
    1610,
    1479,
    1354,
    1235,
    1121,
    1012,
    909,
    811,
    719,
    632,
    550,
    475,
    405,
    340,
    281,
    228,
    180,
    138,
    101,
    70,
    45,
    25,
    11,
    2,
    0,
    0,
};

const int8_t wav_res_formant_square[256] = {
    0, 1, 1, 2, 2, 3, 3, 4,
    4, 5, 6, 8, 9, 11, 13, 16,
    0, 1, 1, 2, 2, 3, 3, 4,
    4, 5, 6, 8, 9, 11, 13, 16,
    0, 1, 1, 2, 2, 3, 3, 4,
    4, 5, 6, 8, 9, 11, 13, 16,
    0, 1, 1, 2, 2, 3, 3, 4,
    4, 5, 6, 8, 9, 11, 13, 16,
    0, 1, 1, 2, 2, 3, 3, 4,
    4, 5, 6, 8, 9, 11, 13, 16,
    0, 1, 1, 2, 2, 3, 3, 4,
    4, 5, 6, 8, 9, 11, 13, 16,
    0, 1, 1, 2, 2, 3, 3, 4,
    4, 5, 6, 8, 9, 11, 13, 16,
    0, 1, 1, 2, 2, 3, 3, 4,
    4, 5, 6, 8, 9, 11, 13, 16,
    0, -1, -1, -2, -2, -3, -3, -4,
    -4, -5, -6, -8, -9, -11, -13, -16,
    0, -1, -1, -2, -2, -3, -3, -4,
    -4, -5, -6, -8, -9, -11, -13, -16,
    0, -1, -1, -2, -2, -3, -3, -4,
    -4, -5, -6, -8, -9, -11, -13, -16,
    0, -1, -1, -2, -2, -3, -3, -4,
    -4, -5, -6, -8, -9, -11, -13, -16,
    0, -1, -1, -2, -2, -3, -3, -4,
    -4, -5, -6, -8, -9, -11, -13, -16,
    0, -1, -1, -2, -2, -3, -3, -4,
    -4, -5, -6, -8, -9, -11, -13, -16,
    0, -1, -1, -2, -2, -3, -3, -4,
    -4, -5, -6, -8, -9, -11, -13, -16,
    0, -1, -1, -2, -2, -3, -3, -4,
    -4, -5, -6, -8, -9, -11, -13, -16};

const int8_t wav_res_formant_sine[256] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 2, 3, 3, 4, 5, 6,
    7, 8, 10, 12, 14, 17, 20, 24,
    0, 3, 4, 5, 6, 7, 9, 10,
    12, 15, 18, 21, 26, 31, 37, 45,
    0, 4, 5, 6, 8, 9, 11, 13,
    16, 19, 23, 28, 34, 40, 49, 58,
    0, 5, 6, 7, 8, 10, 12, 15,
    17, 21, 25, 30, 36, 44, 53, 63,
    0, 4, 5, 6, 8, 9, 11, 13,
    16, 19, 23, 28, 34, 40, 49, 58,
    0, 3, 4, 5, 6, 7, 9, 10,
    12, 15, 18, 21, 26, 31, 37, 45,
    0, 2, 2, 3, 3, 4, 5, 6,
    7, 8, 10, 12, 14, 17, 20, 24,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -2, -3, -3, -4, -5, -6,
    -7, -8, -10, -12, -14, -17, -20, -24,
    0, -3, -4, -5, -6, -7, -9, -10,
    -12, -15, -18, -21, -26, -31, -37, -45,
    0, -4, -5, -6, -8, -9, -11, -13,
    -16, -19, -23, -28, -34, -40, -49, -58,
    0, -5, -6, -7, -8, -10, -12, -15,
    -17, -21, -25, -30, -36, -44, -53, -63,
    0, -4, -5, -6, -8, -9, -11, -13,
    -16, -19, -23, -28, -34, -40, -49, -58,
    0, -3, -4, -5, -6, -7, -9, -10,
    -12, -15, -18, -21, -26, -31, -37, -45,
    0, -2, -2, -3, -3, -4, -5, -6,
    -7, -8, -10, -12, -14, -17, -20, -24};

const uint16_t wav_res_sine16[] = {
    0, 10, 39, 88, 157, 245, 352, 479,
    625, 790, 974, 1178, 1400, 1641, 1901, 2179,
    2475, 2790, 3122, 3472, 3840, 4225, 4627, 5045,
    5481, 5932, 6400, 6884, 7382, 7897, 8426, 8969,
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
    10098, 9527, 8969, 8426, 7897, 7382, 6884, 6400,
    5932, 5481, 5045, 4627, 4225, 3840, 3472, 3122,
    2790, 2475, 2179, 1901, 1641, 1400, 1178, 974,
    790, 625, 479, 352, 245, 157, 88, 39,
    10};

const uint8_t wav_res_vowel_data[] = {
    27,
    40,
    89,
    15,
    13,
    1,
    0,
    18,
    51,
    62,
    13,
    12,
    6,
    0,
    15,
    69,
    93,
    14,
    12,
    7,
    0,
    10,
    84,
    110,
    13,
    10,
    8,
    0,
    23,
    44,
    87,
    15,
    12,
    1,
    0,
    13,
    29,
    80,
    13,
    8,
    0,
    0,
    6,
    46,
    81,
    12,
    3,
    0,
    0,
    9,
    51,
    95,
    15,
    3,
    0,
    3,
    6,
    73,
    99,
    7,
    3,
    14,
    9,
};

const int16_t wav_formant_square[] = {
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    16,
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    16,
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    16,
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    16,
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    16,
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    16,
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    16,
    0,
    1,
    1,
    2,
    2,
    3,
    3,
    4,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    16,
    0,
    -1,
    -1,
    -2,
    -2,
    -3,
    -3,
    -4,
    -4,
    -5,
    -6,
    -8,
    -9,
    -11,
    -13,
    -16,
    0,
    -1,
    -1,
    -2,
    -2,
    -3,
    -3,
    -4,
    -4,
    -5,
    -6,
    -8,
    -9,
    -11,
    -13,
    -16,
    0,
    -1,
    -1,
    -2,
    -2,
    -3,
    -3,
    -4,
    -4,
    -5,
    -6,
    -8,
    -9,
    -11,
    -13,
    -16,
    0,
    -1,
    -1,
    -2,
    -2,
    -3,
    -3,
    -4,
    -4,
    -5,
    -6,
    -8,
    -9,
    -11,
    -13,
    -16,
    0,
    -1,
    -1,
    -2,
    -2,
    -3,
    -3,
    -4,
    -4,
    -5,
    -6,
    -8,
    -9,
    -11,
    -13,
    -16,
    0,
    -1,
    -1,
    -2,
    -2,
    -3,
    -3,
    -4,
    -4,
    -5,
    -6,
    -8,
    -9,
    -11,
    -13,
    -16,
    0,
    -1,
    -1,
    -2,
    -2,
    -3,
    -3,
    -4,
    -4,
    -5,
    -6,
    -8,
    -9,
    -11,
    -13,
    -16,
    0,
    -1,
    -1,
    -2,
    -2,
    -3,
    -3,
    -4,
    -4,
    -5,
    -6,
    -8,
    -9,
    -11,
    -13,
    -16,
};

const int16_t wav_formant_sine[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    2,
    3,
    3,
    4,
    5,
    6,
    7,
    8,
    10,
    12,
    14,
    17,
    20,
    24,
    0,
    3,
    4,
    5,
    6,
    7,
    9,
    10,
    12,
    15,
    18,
    21,
    26,
    31,
    37,
    45,
    0,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    16,
    19,
    23,
    28,
    34,
    40,
    49,
    58,
    0,
    5,
    6,
    7,
    8,
    10,
    12,
    15,
    17,
    21,
    25,
    30,
    36,
    44,
    53,
    63,
    0,
    4,
    5,
    6,
    8,
    9,
    11,
    13,
    16,
    19,
    23,
    28,
    34,
    40,
    49,
    58,
    0,
    3,
    4,
    5,
    6,
    7,
    9,
    10,
    12,
    15,
    18,
    21,
    26,
    31,
    37,
    45,
    0,
    2,
    2,
    3,
    3,
    4,
    5,
    6,
    7,
    8,
    10,
    12,
    14,
    17,
    20,
    24,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    -2,
    -2,
    -3,
    -3,
    -4,
    -5,
    -6,
    -7,
    -8,
    -10,
    -12,
    -14,
    -17,
    -20,
    -24,
    0,
    -3,
    -4,
    -5,
    -6,
    -7,
    -9,
    -10,
    -12,
    -15,
    -18,
    -21,
    -26,
    -31,
    -37,
    -45,
    0,
    -4,
    -5,
    -6,
    -8,
    -9,
    -11,
    -13,
    -16,
    -19,
    -23,
    -28,
    -34,
    -40,
    -49,
    -58,
    0,
    -5,
    -6,
    -7,
    -8,
    -10,
    -12,
    -15,
    -17,
    -21,
    -25,
    -30,
    -36,
    -44,
    -53,
    -63,
    0,
    -4,
    -5,
    -6,
    -8,
    -9,
    -11,
    -13,
    -16,
    -19,
    -23,
    -28,
    -34,
    -40,
    -49,
    -58,
    0,
    -3,
    -4,
    -5,
    -6,
    -7,
    -9,
    -10,
    -12,
    -15,
    -18,
    -21,
    -26,
    -31,
    -37,
    -45,
    0,
    -2,
    -2,
    -3,
    -3,
    -4,
    -5,
    -6,
    -7,
    -8,
    -10,
    -12,
    -14,
    -17,
    -20,
    -24,
};

const uint16_t lut_svf_cutoff[] = {
    17,
    18,
    19,
    20,
    22,
    23,
    24,
    26,
    27,
    29,
    31,
    33,
    35,
    37,
    39,
    41,
    44,
    46,
    49,
    52,
    55,
    58,
    62,
    66,
    70,
    74,
    78,
    83,
    88,
    93,
    99,
    105,
    111,
    117,
    124,
    132,
    140,
    148,
    157,
    166,
    176,
    187,
    198,
    210,
    222,
    235,
    249,
    264,
    280,
    297,
    314,
    333,
    353,
    374,
    396,
    420,
    445,
    471,
    499,
    529,
    561,
    594,
    629,
    667,
    706,
    748,
    793,
    840,
    890,
    943,
    999,
    1059,
    1122,
    1188,
    1259,
    1334,
    1413,
    1497,
    1586,
    1681,
    1781,
    1886,
    1999,
    2117,
    2243,
    2377,
    2518,
    2668,
    2826,
    2994,
    3172,
    3361,
    3560,
    3772,
    3996,
    4233,
    4485,
    4751,
    5033,
    5332,
    5648,
    5983,
    6337,
    6713,
    7111,
    7532,
    7978,
    8449,
    8949,
    9477,
    10037,
    10628,
    11254,
    11916,
    12616,
    13356,
    14138,
    14964,
    15837,
    16758,
    17730,
    18756,
    19837,
    20975,
    22174,
    23435,
    24761,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
    25078,
};

const uint16_t lut_svf_damp[] = {
    65534,
    49213,
    46125,
    44055,
    42453,
    41129,
    39991,
    38988,
    38086,
    37266,
    36512,
    35812,
    35158,
    34544,
    33965,
    33416,
    32893,
    32395,
    31918,
    31460,
    31021,
    30597,
    30188,
    29793,
    29411,
    29041,
    28681,
    28332,
    27992,
    27661,
    27339,
    27024,
    26717,
    26418,
    26125,
    25838,
    25558,
    25283,
    25014,
    24750,
    24491,
    24236,
    23987,
    23742,
    23501,
    23264,
    23031,
    22802,
    22577,
    22355,
    22136,
    21921,
    21708,
    21499,
    21293,
    21089,
    20889,
    20691,
    20495,
    20302,
    20112,
    19924,
    19738,
    19555,
    19373,
    19194,
    19017,
    18842,
    18668,
    18497,
    18327,
    18160,
    17994,
    17830,
    17667,
    17506,
    17347,
    17189,
    17033,
    16878,
    16725,
    16573,
    16423,
    16274,
    16126,
    15980,
    15834,
    15691,
    15548,
    15407,
    15266,
    15127,
    14989,
    14853,
    14717,
    14582,
    14449,
    14316,
    14185,
    14054,
    13925,
    13796,
    13669,
    13542,
    13416,
    13291,
    13167,
    13044,
    12922,
    12801,
    12680,
    12561,
    12442,
    12324,
    12206,
    12090,
    11974,
    11859,
    11744,
    11631,
    11518,
    11406,
    11294,
    11183,
    11073,
    10964,
    10855,
    10747,
    10639,
    10532,
    10426,
    10321,
    10215,
    10111,
    10007,
    9904,
    9801,
    9699,
    9597,
    9496,
    9396,
    9296,
    9196,
    9097,
    8999,
    8901,
    8804,
    8707,
    8610,
    8514,
    8419,
    8324,
    8230,
    8136,
    8042,
    7949,
    7856,
    7764,
    7672,
    7581,
    7490,
    7400,
    7309,
    7220,
    7131,
    7042,
    6953,
    6865,
    6778,
    6690,
    6604,
    6517,
    6431,
    6345,
    6260,
    6175,
    6090,
    6006,
    5922,
    5839,
    5755,
    5673,
    5590,
    5508,
    5426,
    5345,
    5263,
    5183,
    5102,
    5022,
    4942,
    4862,
    4783,
    4704,
    4626,
    4547,
    4469,
    4391,
    4314,
    4237,
    4160,
    4083,
    4007,
    3931,
    3855,
    3780,
    3705,
    3630,
    3555,
    3481,
    3407,
    3333,
    3259,
    3186,
    3113,
    3040,
    2968,
    2895,
    2823,
    2752,
    2680,
    2609,
    2538,
    2467,
    2396,
    2326,
    2256,
    2186,
    2116,
    2047,
    1978,
    1909,
    1840,
    1771,
    1703,
    1635,
    1567,
    1500,
    1432,
    1365,
    1298,
    1231,
    1164,
    1098,
    1032,
    966,
    900,
    834,
    769,
    704,
    639,
    574,
    510,
    445,
    381,
    317,
    253,
};

const int16_t ws_moderate_overdrive[] = {
    -32766,
    -32728,
    -32689,
    -32648,
    -32607,
    -32564,
    -32519,
    -32474,
    -32427,
    -32378,
    -32328,
    -32277,
    -32224,
    -32170,
    -32113,
    -32056,
    -31996,
    -31935,
    -31872,
    -31807,
    -31740,
    -31671,
    -31600,
    -31527,
    -31451,
    -31374,
    -31294,
    -31212,
    -31128,
    -31041,
    -30951,
    -30859,
    -30765,
    -30667,
    -30567,
    -30464,
    -30358,
    -30250,
    -30138,
    -30022,
    -29904,
    -29782,
    -29657,
    -29529,
    -29397,
    -29261,
    -29122,
    -28979,
    -28832,
    -28681,
    -28526,
    -28367,
    -28204,
    -28036,
    -27864,
    -27688,
    -27507,
    -27321,
    -27131,
    -26936,
    -26736,
    -26531,
    -26321,
    -26106,
    -25886,
    -25660,
    -25429,
    -25192,
    -24950,
    -24702,
    -24449,
    -24190,
    -23925,
    -23654,
    -23377,
    -23094,
    -22805,
    -22510,
    -22209,
    -21902,
    -21588,
    -21268,
    -20942,
    -20609,
    -20270,
    -19924,
    -19573,
    -19215,
    -18850,
    -18479,
    -18102,
    -17718,
    -17328,
    -16932,
    -16530,
    -16121,
    -15707,
    -15286,
    -14859,
    -14427,
    -13989,
    -13545,
    -13095,
    -12640,
    -12180,
    -11715,
    -11244,
    -10769,
    -10289,
    -9804,
    -9315,
    -8822,
    -8324,
    -7823,
    -7319,
    -6810,
    -6299,
    -5785,
    -5268,
    -4748,
    -4226,
    -3703,
    -3177,
    -2650,
    -2121,
    -1592,
    -1062,
    -531,
    0,
    531,
    1062,
    1592,
    2122,
    2650,
    3177,
    3703,
    4227,
    4749,
    5268,
    5785,
    6299,
    6811,
    7319,
    7824,
    8325,
    8822,
    9315,
    9804,
    10289,
    10769,
    11244,
    11715,
    12180,
    12641,
    13095,
    13545,
    13989,
    14427,
    14860,
    15286,
    15707,
    16122,
    16530,
    16933,
    17329,
    17719,
    18102,
    18479,
    18850,
    19215,
    19573,
    19925,
    20270,
    20609,
    20942,
    21268,
    21588,
    21902,
    22209,
    22510,
    22806,
    23094,
    23377,
    23654,
    23925,
    24190,
    24449,
    24703,
    24950,
    25192,
    25429,
    25660,
    25886,
    26106,
    26321,
    26531,
    26736,
    26936,
    27131,
    27322,
    27507,
    27688,
    27865,
    28037,
    28204,
    28367,
    28526,
    28681,
    28832,
    28979,
    29122,
    29262,
    29397,
    29529,
    29658,
    29783,
    29904,
    30023,
    30138,
    30250,
    30359,
    30465,
    30568,
    30668,
    30765,
    30860,
    30952,
    31041,
    31128,
    31212,
    31294,
    31374,
    31452,
    31527,
    31600,
    31671,
    31740,
    31807,
    31872,
    31935,
    31996,
    32056,
    32114,
    32170,
    32224,
    32277,
    32329,
    32379,
    32427,
    32474,
    32520,
    32564,
    32607,
    32648,
    32689,
    32728,
    32728,
};

struct PhonemeDefinition
{
  uint8_t formant_frequency[3];
  uint8_t formant_amplitude[3];
};

static const PhonemeDefinition vowels_data[9] = {
    {{27, 40, 89}, {15, 13, 1}},
    {{18, 51, 62}, {13, 12, 6}},
    {{15, 69, 93}, {14, 12, 7}},
    {{10, 84, 110}, {13, 10, 8}},
    {{23, 44, 87}, {15, 12, 1}},
    {{13, 29, 80}, {13, 8, 0}},
    {{6, 46, 81}, {12, 3, 0}},
    {{9, 51, 95}, {15, 3, 0}},
    {{6, 73, 99}, {7, 3, 14}}};

static const PhonemeDefinition consonant_data[8] = {
    {{6, 54, 121}, {9, 9, 0}},
    {{18, 50, 51}, {12, 10, 5}},
    {{11, 24, 70}, {13, 8, 0}},
    {{15, 69, 74}, {14, 12, 7}},
    {{16, 37, 111}, {14, 8, 1}},
    {{18, 51, 62}, {14, 12, 6}},
    {{6, 26, 81}, {5, 5, 5}},
    {{6, 73, 99}, {7, 10, 14}},
};

static const size_t kNumFormants = 5;

typedef struct step_state
{
  int offset;
  bool positive;
} step_state;

const uint32_t lut_oscillator_increments[] = {
    594573364,
    598881888,
    603221633,
    607592826,
    611995694,
    616430467,
    620897376,
    625396654,
    629928536,
    634493258,
    639091058,
    643722175,
    648386851,
    653085330,
    657817855,
    662584675,
    667386036,
    672222191,
    677093390,
    681999888,
    686941940,
    691919804,
    696933740,
    701984010,
    707070875,
    712194602,
    717355458,
    722553711,
    727789633,
    733063497,
    738375577,
    743726151,
    749115497,
    754543897,
    760011633,
    765518991,
    771066257,
    776653721,
    782281674,
    787950409,
    793660223,
    799411412,
    805204277,
    811039119,
    816916243,
    822835954,
    828798563,
    834804379,
    840853716,
    846946888,
    853084215,
    859266014,
    865492610,
    871764326,
    878081490,
    884444431,
    890853479,
    897308971,
    903811242,
    910360631,
    916957479,
    923602131,
    930294933,
    937036233,
    943826384,
    950665739,
    957554655,
    964493491,
    971482608,
    978522372,
    985613148,
    992755307,
    999949221,
    1007195266,
    1014493818,
    1021845258,
    1029249970,
    1036708340,
    1044220756,
    1051787610,
    1059409296,
    1067086213,
    1074818759,
    1082607339,
    1090452358,
    1098354226,
    1106313353,
    1114330156,
    1122405051,
    1130538461,
    1138730809,
    1146982522,
    1155294030,
    1163665767,
    1172098168,
    1180591675,
    1189146729,
};

const uint32_t kPhaseReset[] = {
    0,
    0x80000000,
    0x40000000,
    0x80000000};

class BandLimitedWaveform
{
public:
  BandLimitedWaveform(void);
  int16_t generate_sawtooth(uint32_t new_phase, int i);
  int16_t generate_square(uint32_t new_phase, int i);
  int16_t generate_pulse(uint32_t new_phase, uint32_t pulse_width, int i);
  void init_sawtooth(uint32_t freq_word);
  void init_square(uint32_t freq_word);
  void init_pulse(uint32_t freq_word, uint32_t pulse_width);

private:
  int32_t lookup(int offset);
  void insert_step(int offset, bool rising, int i);
  int32_t process_step(int i);
  int32_t process_active_steps(uint32_t new_phase);
  int32_t process_active_steps_saw(uint32_t new_phase);
  int32_t process_active_steps_pulse(uint32_t new_phase, uint32_t pulse_width);
  void new_step_check_square(uint32_t new_phase, int i);
  void new_step_check_pulse(uint32_t new_phase, uint32_t pulse_width, int i);
  void new_step_check_saw(uint32_t new_phase, int i);

  uint32_t phase_word;
  int32_t dc_offset;
  step_state states[32]; // circular buffer of active steps
  int newptr;            // buffer pointers into states, AND'd with PTRMASK to keep in buffer range.
  int delptr;
  int32_t cyclic[16]; // circular buffer of output samples
  bool pulse_state;
  uint32_t sampled_width; // pulse width is sampled once per waveform
};

class AudioSynthWaveformTS : public AudioStream
{
public:
  AudioSynthWaveformTS(void) : AudioStream(0, NULL),
                               phase_accumulator(0), phase_increment(0), phase_offset(0),
                               magnitude(0), pulse_width(0x40000000),
                               arbdata(NULL), sample(0), tone_type(WAVEFORM_SINE),
                               tone_offset(0), syncFlag(0)
  {
  }

  void frequency(float freq)
  {
    if (freq < 0.0)
    {
      freq = 0.0;
    }
    else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2)
    {
      freq = AUDIO_SAMPLE_RATE_EXACT / 2;
    }
    phase_increment = freq * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
    if (phase_increment > 0x7FFE0000u)
      phase_increment = 0x7FFE0000;
  }
  void phase(float angle)
  {
    if (angle < 0.0)
    {
      angle = 0.0;
    }
    else if (angle > 360.0)
    {
      angle = angle - 360.0;
      if (angle >= 360.0)
        return;
    }
    phase_offset = angle * (4294967296.0 / 360.0);
  }
  void sync()
  {
    syncFlag = 1;
  }
  void amplitude(float n)
  { // 0 to 1.0
    if (n < 0)
    {
      n = 0;
    }
    else if (n > 1.0)
    {
      n = 1.0;
    }
    magnitude = n * 65536.0;
  }
  void offset(float n)
  {
    if (n < -1.0)
    {
      n = -1.0;
    }
    else if (n > 1.0)
    {
      n = 1.0;
    }
    tone_offset = n * 32767.0;
  }
  void pulseWidth(float n)
  { // 0.0 to 1.0
    if (n < 0)
    {
      n = 0;
    }
    else if (n > 1.0)
    {
      n = 1.0;
    }
    pulse_width = n * 4294967296.0;
  }
  void begin(short t_type)
  {
    phase_offset = 0;
    tone_type = t_type;
    if (t_type == WAVEFORM_BANDLIMIT_SQUARE)
      band_limit_waveform.init_square(phase_increment);
    else if (t_type == WAVEFORM_BANDLIMIT_PULSE)
      band_limit_waveform.init_pulse(phase_increment, pulse_width);
    else if (t_type == WAVEFORM_BANDLIMIT_SAWTOOTH || t_type == WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE)
      band_limit_waveform.init_sawtooth(phase_increment);
  }
  void begin(float t_amp, float t_freq, short t_type)
  {
    amplitude(t_amp);
    frequency(t_freq);
    phase_offset = 0;
    begin(t_type);
  }
  void arbitraryWaveform(const int16_t *data, float maxFreq)
  {
    arbdata = data;
  }
  void LFO_mode(uint8_t lfo_Mode)
  {
    lfo_mode = lfo_Mode;
  }
  void LFO_phase(uint8_t lfo_Phase)
  { // LFO parameter: SYN
    lfo_phase = lfo_Phase;
  }
  void LFO_oneShoot(boolean lfo_OneShoot)
  {
    lfo_oneShoot = lfo_OneShoot;
  }

  virtual void update(void);

private:
  uint32_t phase_accumulator;
  uint32_t phase_increment;
  uint32_t phase_offset;
  int32_t magnitude;
  uint32_t pulse_width;
  const int16_t *arbdata;
  int16_t sample;  // for WAVEFORM_SAMPLE_HOLD
  int16_t sample2; // for WAVEFORM_SAMPLE_HOLD and oneShot
  short tone_type;
  int16_t tone_offset;
  int16_t syncFlag;
  BandLimitedWaveform band_limit_waveform;
  uint8_t lfo_mode;
  uint8_t lfo_phase;
  boolean lfo_oneShoot;
  uint8_t lfo1ph;
  boolean lfo_randomFlag;
};

static const uint16_t kHighestNote = 140 * 128;
static const uint16_t kPitchTableStart = 128 * 128;
static const uint16_t kOctave = 12 * 128;
static const uint32_t kFIR4Coefficients[4] = {10530, 14751, 16384, 14751};
static const uint32_t kFIR4DcOffset = 28208;

class AudioSynthWaveformModulatedTS : public AudioStream
{
public:
  AudioSynthWaveformModulatedTS(void) : AudioStream(4, inputQueueArray),
                                        phase_accumulator(0), phase_increment(0), modulation_factor(32768),
                                        magnitude(0), arbdata(NULL), sample(0), tone_offset(0),
                                        tone_type(WAVEFORM_SINE), modulation_type(0), syncFlag(0), osc_par_a(0),
                                        osc_par_b(0), par_a_mod_(0), par_b_mod_(0), pitch_note(0)
  {
  }
  void frequency(float freq)
  {
    if (freq < 0.0)
    {
      freq = 0.0;
    }
    else if (freq > AUDIO_SAMPLE_RATE_EXACT / 2)
    {
      freq = AUDIO_SAMPLE_RATE_EXACT / 2;
    }
    phase_increment = freq * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
    if (phase_increment > 0x7FFE0000u)
      phase_increment = 0x7FFE0000;
  }
  void amplitude(float n)
  { // 0 to 1.0
    if (n < 0)
    {
      n = 0;
    }
    else if (n > 1.0)
    {
      n = 1.0;
    }
    magnitude = n * 65536.0;
  }
  void sync()
  {
    syncFlag = 1;
  }
  void offset(float n)
  {
    if (n < -1.0)
    {
      n = -1.0;
    }
    else if (n > 1.0)
    {
      n = 1.0;
    }
    tone_offset = n * 32767.0;
  }
  void begin(short t_type)
  {
    tone_type = t_type;
    if (t_type == WAVEFORM_BANDLIMIT_SQUARE)
      band_limit_waveform.init_square(phase_increment);
    else if (t_type == WAVEFORM_BANDLIMIT_PULSE)
      band_limit_waveform.init_pulse(phase_increment, 0x80000000u);
    else if (t_type == WAVEFORM_BANDLIMIT_SAWTOOTH || t_type == WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE)
      band_limit_waveform.init_sawtooth(phase_increment);
  }
  void begin(float t_amp, float t_freq, short t_type)
  {
    amplitude(t_amp);
    frequency(t_freq);
    begin(t_type);
  }
  void arbitraryWaveform(const int16_t *data, float maxFreq)
  {
    arbdata = data;
  }
  void frequencyModulation(float octaves)
  {
    if (octaves > 12.0)
    {
      octaves = 12.0;
    }
    else if (octaves < 0.1)
    {
      octaves = 0.1;
    }
    modulation_factor = octaves * 4096.0;
    modulation_type = 0;
  }
  void phaseModulation(float degrees)
  {
    if (degrees > 9000.0)
    {
      degrees = 9000.0;
    }
    else if (degrees < 30.0)
    {
      degrees = 30.0;
    }
    modulation_factor = degrees * (65536.0 / 180.0);
    modulation_type = 1;
  }
  void parameter_a(uint8_t Osc_par_a)
  { // parameter_a im Osc Menu
    osc_par_a = Osc_par_a;
  }
  void parameter_b(uint8_t Osc_par_b)
  { // parameter_b im Osc Menu
    osc_par_b = Osc_par_b;
  }
  void ShapeDigitalFilter(uint8_t shape_digital_filter)
  { // Bank 15 waveform 12-15
    shape_digital_filter_ = shape_digital_filter;
  }

  void Osc_pitch_note(uint8_t Osc_pitch_note)
  {
    pitch_note = Osc_pitch_note;
  }

  int16_t Interpolate824(const int16_t *table, uint32_t phase)
  { // int16_t table
    int32_t a = table[phase >> 24];
    int32_t b = table[phase >> 24] + 1;
    return a + ((b - a) * static_cast<int32_t>((phase >> 8) & 0xffff) >> 16);
  }

  int16_t Interpolate824u(const uint16_t *table, uint32_t phase)
  { // uint16_t table
    int32_t a = lut_bell[phase >> 24];
    int32_t b = lut_bell[phase >> 24] + 1;
    return (a + ((b - a) * static_cast<int32_t>((phase >> 8) & 0xffff) >> 16)) >> 1;
  }

  int16_t Interpolate824ut(const uint16_t *table, uint32_t phase)
  { // uint16_t table
    int32_t a = table[phase >> 24];
    int32_t b = table[phase >> 24] + 1;
    return (a + ((b - a) * static_cast<int32_t>((phase >> 8) & 0xffff) >> 16)) >> 1;
  }

  int16_t Interpolate88(const int16_t *table, uint16_t index)
  { // int16_t table
    int32_t a = table[index >> 8];
    int32_t b = table[index >> 8] + 1;
    return a + ((b - a) * static_cast<int32_t>((index >> 8) & 0xff) >> 8);
  }
  uint32_t ComputePhaseIncrement(int16_t midi_pitch)
  {
    if (midi_pitch >= kPitchTableStart)
    {
      midi_pitch = kPitchTableStart - 1;
    }

    int32_t ref_pitch = midi_pitch;
    ref_pitch -= kPitchTableStart;

    size_t num_shifts = 0;
    while (ref_pitch < 0)
    {
      ref_pitch += kOctave;
      ++num_shifts;
    }

    uint32_t a = lut_oscillator_increments[ref_pitch >> 4];
    uint32_t b = lut_oscillator_increments[(ref_pitch >> 4) + 1];
    uint32_t phase_increment_cp = a +
                                  (static_cast<int32_t>(b - a) * (ref_pitch & 0xf) >> 4);
    phase_increment_cp >>= num_shifts;
    return phase_increment_cp;
  }

  int32_t CLIP(int32_t value) const
  {
    if (value > max_value)
    {
      value = max_value;
    }
    else if (value < min_value)
    {
      value = min_value;
    }
    return value;
  }

  int16_t Mix(int16_t a, int16_t b, uint16_t balance)
  {
    return (a * (65535 - balance) + b * balance) >> 16;
  }

  inline int32_t ThisBlepSample(uint32_t t) {
    if (t > 65535) {
      t = 65535;
    }
    return t * t >> 18;
  }
  
  inline int32_t NextBlepSample(uint32_t t) {
    if (t > 65535) {
      t = 65535;
    }
    t = 65535 - t;
    return -static_cast<int32_t>(t * t >> 18);
  }

  virtual void update(void);

  struct VowelSynthesizerState
  {
    uint32_t formant_increment[3];
    uint32_t formant_phase[3];
    uint32_t formant_amplitude[3];
    uint16_t consonant_frames;
    // uint16_t noise;
  } state_vow;

  struct ResoSquareState
  {
    uint32_t modulator_phase_increment;
    uint32_t modulator_phase;
    uint32_t square_modulator_phase;
    int32_t integrator;
    bool polarity;
    uint32_t pitch_;
  } state_res;

  struct ToyState {
  uint8_t held_sample;
  uint16_t decimation_counter;
} state_toy;

struct SawSwarmState {
  uint32_t phase[6];
  uint32_t increments[5];
  uint32_t data_qs_phase[4];
  int32_t filter_state[2][2];
  int32_t dc_blocked;
  int32_t lp;
  int32_t bp;
} state_saw;

private:
  audio_block_t *inputQueueArray[4];
  uint32_t phase_accumulator;
  uint32_t phase_increment;
  uint32_t modulation_factor;
  int32_t magnitude;
  const int16_t *arbdata;
  uint32_t phasedata[AUDIO_BLOCK_SAMPLES];
  int16_t sample; // for WAVEFORM_SAMPLE_HOLD
  int16_t tone_offset;
  uint8_t tone_type;
  uint8_t modulation_type;
  int16_t syncFlag;
  uint8_t Osc_data_cr_decimate;
  uint16_t Osc_data_cr_state;
  uint16_t OscData_sec_phase;
  BandLimitedWaveform band_limit_waveform;
  uint8_t osc_par_a = 0;
  uint8_t osc_par_b = 0;
  uint8_t Osc_vw_update;
  boolean strike_ = true;
  uint16_t state_vow_consonant_frames;
  uint32_t phaseOld_ = 0;
  int16_t par_a_mod_ = 0; // parameter_A
  int16_t par_b_mod_ = 0; // parameter_B
  int32_t parameter_[2];
  int32_t pitch_ = 0;
  int32_t min_value = -32767;
  int32_t max_value = 32768;
  uint16_t pitch_note = 0;
  uint16_t balance_x = 0;
  uint8_t shape_digital_filter_;
  int32_t next_sample_;
  bool high_;
  int16_t discontinuity_depth_;
  int16_t aux_parameter_;
};

#endif