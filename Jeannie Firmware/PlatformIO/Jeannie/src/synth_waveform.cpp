/* Audio Library for Teensy 3.X
* Copyright (c) 2018, Paul Stoffregen, paul@pjrc.com
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
Added WAVEFORM_SILENT, syncFlag

R.Degen 2023
Added Supersaw and Mutable instruments Braids/Shruthi digital oscillator
Add two modulation inputs in AudioSynthWaveformModulatedTS
Braids oscillator has two modulation parameters_A/B. Shruthi has one
*/

#include <Arduino.h>
#include "synth_waveform.h"
#include "arm_math.h"
#include "utility/dspinst.h"
#include "Entropy.h"

// uncomment for more accurate but more computationally expensive frequency modulation
// #define IMPROVE_EXPONENTIAL_ACCURACY
#define BASE_AMPLITUDE 0x6000 // 0x7fff won't work due to Gibb's phenomenon, so use 3/4 of full range.
#define DIV32768 3.0517578E-5f
#define DIV255 0.00392156

void AudioSynthWaveformTS::update(void)
{
	audio_block_t *block;
	int16_t *bp, *end;
	int32_t val1, val2;
	int16_t magnitude15;
	uint32_t i, ph, index, index2, scale;
	const uint32_t inc = phase_increment;
	uint32_t phaseX;

	if (syncFlag == 1)
	{
		phase_accumulator = 0;
		phaseX = 0;
		syncFlag = 0;
		lfo_randomFlag = false;
	}

	ph = phase_accumulator + phase_offset;
	phaseX = phase_accumulator;

	if (magnitude == 0)
	{
		phase_accumulator += inc * AUDIO_BLOCK_SAMPLES;
		return;
	}
	block = allocate();
	if (!block)
	{
		phase_accumulator += inc * AUDIO_BLOCK_SAMPLES;
		return;
	}
	bp = block->data;

	switch (tone_type)
	{

	// PWM SINE -------------------------------------------------------
	case PWM_WAVEFORM_SINE:
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			index = ph >> 24;
			val1 = AudioWaveformSine[index];
			val2 = AudioWaveformSine[index + 1];
			scale = (ph >> 8) & 0xFFFF;
			val2 *= scale;
			val1 *= 0x10000 - scale;
			*bp++ = multiply_32x32_rshift32(val1 + val2, magnitude);
			ph += inc;
		}
		break;

	// PWM Triangle ---------------------------------------------------
	case PWM_WAVEFORM_TRIANGLE:
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			uint32_t phtop = ph >> 30;
			if (phtop == 1 || phtop == 2)
			{
				*bp++ = ((0xFFFF - (ph >> 15)) * magnitude) >> 16;
			}
			else
			{
				*bp++ = (((int32_t)ph >> 15) * magnitude) >> 16;
			}
			ph += inc;
		}
		break;

	// PWM SAWTOOTH ---------------------------------------------------
	case PWM_WAVEFORM_SAWTOOTH: // normal sawtooth and inv sawtooth
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			*bp++ = signed_multiply_32x16t(magnitude, ph >> 1);
			ph += inc;
		}
		break;

	// PWM SQUARE -----------------------------------------------------
	case PWM_WAVEFORM_SQUARE:
		magnitude15 = signed_saturate_rshift(magnitude, 16, 1);
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			// if (ph & 0x80000000) {
			if (ph & 0x80000000)
			{
				*bp++ = -magnitude15;
			}
			else
			{
				*bp++ = magnitude15;
			}
			ph += inc;
		}
		break;

	// LFO Arbitrary waveform -----------------------------------------
	case LFO_WAVEFORM_ARBITRARY:
		uint8_t lfo_mode_;
		uint8_t lfo_phase_;
		boolean lfo_oneShoot_;
		lfo_mode_ = lfo_mode;
		lfo_phase_ = lfo_phase; // LFO parameter: SYN
		lfo_oneShoot_ = lfo_oneShoot;

		if (!arbdata)
		{
			release(block);
			phase_accumulator += inc * AUDIO_BLOCK_SAMPLES;
			return;
		}
		// len = 256
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			index = ph >> 24;
			index2 = index + 1;
			if (lfo_mode_ == 0)
			{
				if (index2 >= 256)
					index2 = 0; // loop
			}
			else
			{
				if (index2 >= 256)
					index2 = 255; // OneShoot
			}
			val1 = *(arbdata + index);
			val2 = *(arbdata + index2);
			scale = (ph >> 8) & 0xFFFF;
			val2 *= scale;
			val1 *= 0x10000 - scale;
			*bp++ = multiply_32x32_rshift32(val1 + val2, magnitude);
			uint32_t ph_old = ph;
			uint32_t phaseX_old = phaseX;
			if (lfo_phase_ <= 1)
			{ // Shape normal
				ph += inc;
				phaseX += inc;
			}
			else
			{
				ph -= inc;
				phaseX -= inc; // Shape inverse
			}

			if (lfo_oneShoot_ == true && phaseX < inc)
			{
				ph = ph_old;
				phaseX = phaseX_old;
			}
		}
		break;

	// LFO S&H waveform -----------------------------------------------
	case LFO_WAVEFORM_SAMPLE_HOLD:
		boolean randomFlag_;
		boolean oneShoot_;
		randomFlag_ = lfo_randomFlag;
		oneShoot_ = lfo_oneShoot;
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			uint32_t newph = ph + inc;
			if (oneShoot_ == true && randomFlag_ == false)
			{ // one shot
				sample = random(magnitude) - (magnitude >> 1);
				sample2 = sample;
				randomFlag_ = true;
			}
			else if (newph < ph && oneShoot_ == false)
			{
				sample = random(magnitude) - (magnitude >> 1);
			}
			else if (oneShoot_ == true && randomFlag_ == true)
			{
				sample = sample2;
			}

			*bp++ = sample;
			ph = newph;
		}
		lfo_randomFlag = randomFlag_;
		break;
	}

	phase_accumulator = ph - phase_offset;

	if (tone_offset)
	{
		bp = block->data;
		end = bp + AUDIO_BLOCK_SAMPLES;
		do
		{
			val1 = *bp;
			*bp++ = signed_saturate_rshift(val1 + tone_offset, 16, 0);
		} while (bp < end);
	}
	transmit(block, 0);
	release(block);
}

//--------------------------------------------------------------------------------

void AudioSynthWaveformModulatedTS::update(void)
{
	audio_block_t *block, *moddata, *shapedata, *par_A, *par_B;
	// audio_block_t *block, *moddata, *shapedata;
	int16_t *bp, *end, *par_A_mod, *par_B_mod;
	int32_t val1, val2;
	int16_t magnitude15;
	uint32_t i, ph, index, index2, scale, priorphase;
	const uint32_t inc = phase_increment;
	uint32_t phase_spread;
	uint32_t saw_phase_increment;
	uint32_t increments[5];
	uint32_t ph_1;
	uint32_t ph_2;
	uint32_t ph_3;
	uint32_t ph_4;
	uint32_t ph_5;
	uint32_t mod_increment;

	moddata = receiveReadOnly(0);
	shapedata = receiveReadOnly(1);
	par_A = receiveReadOnly(2); // new modulation input for osc parameter_a
	par_B = receiveReadOnly(3); // new modulation input for osc parameter_b

	if (syncFlag == 1)
	{
		phase_accumulator = 0;
		syncFlag = 0;
	}

	// Pre-compute the phase angle for every output sample of this update
	ph = phase_accumulator;
	priorphase = phasedata[AUDIO_BLOCK_SAMPLES - 1];
	if (moddata && modulation_type == 0)
	{

		// Frequency Modulation
		bp = moddata->data;

		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			int32_t n = (*bp++) * modulation_factor; // n is # of octaves to mod
			int32_t ipart = n >> 27;				 // 4 integer bits
			n &= 0x7FFFFFF;							 // 27 fractional bits
#ifdef IMPROVE_EXPONENTIAL_ACCURACY
			// exp2 polynomial suggested by Stefan Stenzel on "music-dsp"
			// mail list, Wed, 3 Sep 2014 10:08:55 +0200
			int32_t x = n << 3;
			n = multiply_accumulate_32x32_rshift32_rounded(536870912, x, 1494202713);
			int32_t sq = multiply_32x32_rshift32_rounded(x, x);
			n = multiply_accumulate_32x32_rshift32_rounded(n, sq, 1934101615);
			n = n + (multiply_32x32_rshift32_rounded(sq,
													 multiply_32x32_rshift32_rounded(x, 1358044250))
					 << 1);
			n = n << 1;
#else
			// exp2 algorithm by Laurent de Soras
			// https://www.musicdsp.org/en/latest/Other/106-fast-exp2-approximation.html
			n = (n + 134217728) << 3;
			n = multiply_32x32_rshift32_rounded(n, n);
			n = multiply_32x32_rshift32_rounded(n, 715827883) << 3;
			n = n + 715827882;
#endif
			uint32_t scale = n >> (14 - ipart);
			mod_increment = n;
			uint64_t phstep = (uint64_t)inc * scale;
			uint32_t phstep_msw = phstep >> 32;
			if (phstep_msw < 0x7FFE)
			{
				ph += phstep >> 16;
			}
			else
			{
				ph += 0x7FFE0000;
			}
			phasedata[i] = ph;
		}
		release(moddata);
	}
	else if (moddata)
	{
		// Phase Modulation
		bp = moddata->data;
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			// more than +/- 180 deg shift by 32 bit overflow of "n"
			uint32_t n = (uint16_t)(*bp++) * modulation_factor;
			phasedata[i] = ph + n;
			ph += inc;
		}
		release(moddata);
	}
	else
	{
		// No Modulation Input
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			phasedata[i] = ph;
			ph += inc;
		}
	}
	phase_accumulator = ph;

	// Read two new Modulation inputs from AudioConnection into AudioPatching.h
	// Braid's oscillator has two modulation parameters par_A_mod_ and par_B_mod_
	// Shruthi oscillator has only one par_A_mod_
	if (par_A)
	{
		par_A_mod = par_A->data;
		par_a_mod_ = *par_A_mod; // save modulation data
		release(par_A);
	}
	if (par_B)
	{
		par_B_mod = par_B->data;
		par_b_mod_ = *par_B_mod; // save modulation data
		release(par_B);
	}

	// Amplitude is always 1 on when oscillator is sounding
	// magnitude must be set to zero, otherwise digital noise comes through
	if (tone_type == WAVEFORM_SILENT)
	{
		magnitude = 0;
	}
	else
	{
		magnitude = 65536.0;
	}
	// If the amplitude is zero, no output, but phase still increments properly
	if (magnitude == 0)
	{
		if (shapedata)
			release(shapedata);
		return;
	}
	block = allocate();
	if (!block)
	{
		if (shapedata)
			release(shapedata);
		return;
	}
	bp = block->data;

	// Now generate the output samples using the pre-computed phase angles

	switch (tone_type)
	{

	// WaveformModulated Sine -----------------------------------------
	case WAVEFORM_SINE:
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			ph = phasedata[i];
			index = ph >> 24;
			val1 = AudioWaveformSine[index];
			val2 = AudioWaveformSine[index + 1];
			scale = (ph >> 8) & 0xFFFF;
			val2 *= scale;
			val1 *= 0x10000 - scale;
			*bp++ = multiply_32x32_rshift32(val1 + val2, magnitude);
		}
		break;

	// WaveformModulated Triangle -------------------------------------
	case WAVEFORM_TRIANGLE:
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			ph = phasedata[i];
			uint32_t phtop = ph >> 30;
			if (phtop == 1 || phtop == 2)
			{
				*bp++ = ((0xFFFF - (ph >> 15)) * magnitude) >> 16;
			}
			else
			{
				*bp++ = (((int32_t)ph >> 15) * magnitude) >> 16;
			}
		}
		break;

	// WaveformModulated MULTISAW -------------------------------------
	case WAVEFORM_MULTISAW:
		int32_t sample;
		uint8_t parameter_a;
		uint8_t parameter_b;
		parameter_a = osc_par_a; // Parameter Spread
		parameter_b = osc_par_b; // Parameter Sawmix

		phase_spread = (phase_increment >> 14) * parameter_a;
		++phase_spread;
		saw_phase_increment = phase_increment & random(0xFFFF);
		for (uint8_t i = 0; i < 5; ++i)
		{
			saw_phase_increment += phase_spread;
			increments[i] = saw_phase_increment;
		}
		// Phase spread and sawmix
		int32_t magnitude_a;
		int32_t magnitude_b;
		magnitude_a = magnitude * ((127 - parameter_b) * 0.0078f);
		magnitude_b = magnitude * (parameter_b * 0.0078f);

		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			data_qs_phase[0] -= increments[0];
			data_qs_phase[1] -= increments[1];
			data_qs_phase[2] += increments[2];
			data_qs_phase[3] += increments[3];
			ph_1 = phasedata[i];
			ph_2 = (ph_1 + data_qs_phase[0]);
			ph_3 = (ph_2 + data_qs_phase[1]);
			ph_4 = (ph_3 + data_qs_phase[2]);
			ph_5 = (ph_4 + data_qs_phase[3]);
			sample = signed_multiply_32x16t(magnitude_a, ph_1) >> 1;
			sample += signed_multiply_32x16t(magnitude_b, ph_2) >> 2;
			sample += signed_multiply_32x16t(magnitude_b, ph_3) >> 2;
			sample += signed_multiply_32x16t(magnitude_b, ph_4) >> 2;
			sample += signed_multiply_32x16t(magnitude_b, ph_5) >> 2;

			float sample_f;
			sample_f = sample * 1.35f * DIV32768;
			// a = 1.5*a - 0.5 * a³  //should be converted to fixed point
			sample_f = 1.50f * sample_f - 0.5f * sample_f * sample_f * sample_f;
			sample = sample_f * 32768;
			*bp++ = (int16_t)((sample * magnitude) >> 16);
		}
		break;

	// WaveformModulated Square ---------------------------------------
	case WAVEFORM_SQUARE:
		magnitude15 = signed_saturate_rshift(magnitude, 16, 1);
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			if (phasedata[i] & 0x80000000)
			{
				*bp++ = -magnitude15;
			}
			else
			{
				*bp++ = magnitude15;
			}
		}
		break;

	// WaveformModulated Pulse ----------------------------------------
	case WAVEFORM_PULSE:
		if (shapedata)
		{
			magnitude15 = signed_saturate_rshift(magnitude, 16, 1);
			for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
			{
				uint32_t width = ((shapedata->data[i] + 0x8000) & 0xFFFF) << 16;
				if (phasedata[i] < width)
				{
					*bp++ = magnitude15;
				}
				else
				{
					*bp++ = -magnitude15;
				}
			}
			break;
		} // else fall through to orginary square without shape modulation

	// WaveformModulated Sample & Hold --------------------------------
	case WAVEFORM_SAMPLE_HOLD:
		sample = 0;
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			ph = phasedata[i];
			if (ph < priorphase)
			{ // does not work for phase modulation
				sample = random(magnitude) - (magnitude >> 1);
			}
			priorphase = ph;
			*bp++ = sample;
		}
		break;

	// WaveformModulated Sawtooth reverse -----------------------------
	case WAVEFORM_SAWTOOTH_REVERSE:
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			*bp++ = signed_multiply_32x16t(0xFFFFFFFFu - magnitude, phasedata[i]);
		}
		break;

	// WaveformModulated Variable Triangle (Triangle -> Saw) ----------
	case WAVEFORM_TRIANGLE_VARIABLE:
		if (shapedata)
		{
			for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
			{
				uint32_t width = (shapedata->data[i] + 0x8000) & 0xFFFF;
				uint32_t rise = 0xFFFFFFFF / width;
				uint32_t fall = 0xFFFFFFFF / (0xFFFF - width);
				uint32_t halfwidth = width << 15;
				uint32_t n;
				ph = phasedata[i];
				if (ph < halfwidth)
				{
					n = (ph >> 16) * rise;
					*bp++ = ((n >> 16) * magnitude) >> 16;
				}
				else if (ph < 0xFFFFFFFF - halfwidth)
				{
					n = 0x7FFFFFFF - (((ph - halfwidth) >> 16) * fall);
					*bp++ = (((int32_t)n >> 16) * magnitude) >> 16;
				}
				else
				{
					n = ((ph + halfwidth) >> 16) * rise + 0x80000000;
					*bp++ = (((int32_t)n >> 16) * magnitude) >> 16;
				}
				ph += inc;
			}
			break;
			// else fall through to orginary triangle without shape modulation
		}

	// WaveformModulated Bandlimit Sawtooth and Sawtooth reverse ------
	case WAVEFORM_BANDLIMIT_SAWTOOTH:
	case WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE:
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			int16_t val = band_limit_waveform.generate_sawtooth(phasedata[i], i);
			val = (int16_t)((val * magnitude) >> 16);
			*bp++ = tone_type == WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE ? (int16_t)-val : (int16_t) + val;
		}
		break;

	// WaveformModulated Bandlimit Square -----------------------------
	case WAVEFORM_BANDLIMIT_SQUARE:
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			int32_t val = band_limit_waveform.generate_square(phasedata[i], i);
			*bp++ = (int16_t)((val * magnitude) >> 16);
		}
		break;

	// WaveformModulated Bandlimit Puls -------------------------------
	case WAVEFORM_BANDLIMIT_PULSE:
		if (shapedata)
		{
			for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
			{
				uint32_t width = ((shapedata->data[i] + 0x8000) & 0xFFFF) << 16;
				int32_t val = band_limit_waveform.generate_pulse(phasedata[i], width, i);
				*bp++ = (int16_t)((val * magnitude) >> 16);
			}
			break;
		} // else fall through to orginary square without shape modulation

	// WaveformModulated Arbitrary ------------------------------------
	case WAVEFORM_ARBITRARY:
		if (!arbdata)
		{
			release(block);
			if (shapedata)
				release(shapedata);
			return;
		}
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			ph = phasedata[i];
			index = ph >> 24;
			index2 = index + 1;
			if (index2 >= 256)
				index2 = 0;
			val1 = *(arbdata + index);
			val2 = *(arbdata + index2);
			scale = (ph >> 8) & 0xFFFF;
			val2 *= scale;
			val1 *= 0x10000 - scale;
			*bp++ = multiply_32x32_rshift32(val1 + val2, magnitude);
		}
		break;

	// WaveformModulated Braids VOWEL ---------------------------------
	case WAVEFORM_BRAIDS_VOWEL:
	{
		uint16_t balance;
		uint16_t balance_;
		uint8_t vowel_index;
		uint16_t formant_shift;

		// parameter + modulation value
		parameter_[0] = (osc_par_a << 8) + par_a_mod_;
		parameter_[1] = (osc_par_b << 8) + par_b_mod_;
		// limit max. value
		if (parameter_[0] >= 32768)
		{
			parameter_[0] = 32768;
		}
		if (parameter_[1] >= 32768)
		{
			parameter_[1] = 32768;
		}

		vowel_index = parameter_[0] >> 12;
		balance = parameter_[0] & 0x0fff;
		balance_ = 0x1000 - balance;
		// formant_shift = (440 + (parameter_[1] >> 5));
		formant_shift = (430 + (parameter_[1] >> 5));

		if (strike_)
		{
			strike_ = false;
			state_vow_consonant_frames = 160;
			uint16_t index = 0;// (Random::GetSample() + 1) & 7;
			for (size_t i = 0; i < 3; i++)
			{
				state_vow_formant_increment[i] = static_cast<uint32_t>(consonant_data[index].formant_frequency[i]) *
												 0x1000 * formant_shift;
				state_vow_formant_amplitude[i] = consonant_data[index].formant_amplitude[i];
			}
		}
		if (state_vow_consonant_frames)
		{
			--state_vow_consonant_frames;
		}
		else
		{
			for (size_t i = 0; i < 3; ++i)
			{
				state_vow_formant_increment[i] = (vowels_data[vowel_index].formant_frequency[i] *
													  balance_ +
												  vowels_data[vowel_index + 1].formant_frequency[i] * balance) *
												 formant_shift;
				state_vow_formant_amplitude[i] = (vowels_data[vowel_index].formant_amplitude[i] *
													  balance_ +
												  vowels_data[vowel_index + 1].formant_amplitude[i] * balance) >>
												 12;
			}
		}

		uint32_t phase_;

		for (uint8_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			phase_ = phasedata[i];
			size_t phaselet;
			int16_t sample = 0;

			for (size_t i = 0; i < 2; i++)
			{
				state_vow_formant_phase[i] += state_vow_formant_increment[i];
				phaselet = (state_vow_formant_phase[i] >> 24) & 0xf0;
				sample += wav_formant_sine[phaselet | state_vow_formant_amplitude[i]];
			}
			state_vow_formant_phase[2] += state_vow_formant_increment[2];
			phaselet = (state_vow_formant_phase[2] >> 24) & 0xf0;
			sample += wav_formant_square[phaselet | state_vow_formant_amplitude[2]];

			sample *= 255 - (phase_ >> 24);

			if (phase_ < phaseOld_)
			{
				state_vow_formant_phase[0] = 0;
				state_vow_formant_phase[1] = 0;
				state_vow_formant_phase[2] = 0;
				sample = 0;
			}
			phaseOld_ = phase_;

			float sample_f;
			sample_f = sample * 1.35f / 32768;
			// a = 1.5*a - 0.5 * a³  //should be converted to fixed point
			sample_f = 1.50f * sample_f - 0.5f * sample_f * sample_f * sample_f;
			sample = sample_f * 32768;
			*bp++ = ~(int16_t)((sample * magnitude) >> 16);
		}
	}
	break;

	// WaveformModulated Shruthi ZSAW ---------------------------------
	case WAVEFORM_SHRUTHI_ZSAW:
	{
		uint16_t parameter;
		parameter = osc_par_a << 1; // parameter_a
		parameter = parameter + (par_a_mod_ >> 6);
		if (parameter >= 255)
		{
			parameter = 255;
		}

		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			ph = phasedata[i] << 1;
			uint8_t phi = ph >> 24;
			uint8_t clipped_phi = phi < 0x20 ? phi << 3 : 0xff;
			uint16_t sum = (clipped_phi * parameter);
			uint16_t mul = phi * (255 - parameter);
			sum = sum + mul;
			int16_t sample = (wav_res_sine16[sum >> 8]) + 32768;
			*bp++ = sample;
		}
	}
	break;

	// WaveformModulated Shruthi ZSYNC ---------------------------------
	case WAVEFORM_SHRUTHI_ZSYNC:
	{
		uint16_t parameter_zs;
		uint16_t phase_2_zs;
		uint16_t phase_integral_zs;
		uint16_t increment_zs;
		parameter_zs = osc_par_a; // parameter_a
		parameter_zs = parameter_zs + (par_a_mod_ >> 7);
		if (parameter_zs >= 255)
		{
			parameter_zs = 255;
		}
		phase_integral_zs = phase_increment >> 15;
		increment_zs = phase_integral_zs + (phase_integral_zs * uint32_t(parameter_zs) >> 3);
		phase_2_zs = OscData_sec_phase;
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			ph = phasedata[i] << 1;
			phase_integral_zs = ph >> 16;
			if (ph < phaseOld_)
			{
				phase_2_zs = 0;
			}
			phaseOld_ = ph;
			phase_2_zs += increment_zs;
			uint16_t carrier = (wav_res_sine16[phase_2_zs >> 8]);
			int16_t sample = (phase_integral_zs < 0x8000 ? carrier : 32768) + 32768;
			*bp++ = sample;
		}
		// update secondary phase
		OscData_sec_phase = phase_2_zs;
	}
	break;

	// WaveformModulated Shruthi ZTRI ---------------------------------
	case WAVEFORM_SHRUTHI_ZTRI:
	{
		uint16_t parameter_zt;
		uint16_t phase_2_zt;
		uint16_t phase_integral_zt;
		uint16_t increment_zt;
		parameter_zt = osc_par_a; // parameter_a
		parameter_zt = parameter_zt + (par_a_mod_ >> 7);
		if (parameter_zt >= 255)
		{
			parameter_zt = 255;
		}
		phase_integral_zt = phase_increment >> 15;
		increment_zt = phase_integral_zt + (phase_integral_zt * uint32_t(parameter_zt) >> 3);
		phase_2_zt = OscData_sec_phase;
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			ph = phasedata[i] << 1;
			uint16_t phase_integral_zt = ph >> 16;
			if (ph < phaseOld_)
			{ // phase.carry
				phase_2_zt = 0;
			}
			phaseOld_ = ph;
			phase_2_zt += increment_zt;
			uint16_t carrier = (wav_res_sine16[phase_2_zt >> 8]);
			uint8_t window = 0;
			window = (phase_integral_zt & 0x8000) ? ~(uint8_t)(phase_integral_zt >> 7) : phase_integral_zt >> 7;
			int16_t sample = ((carrier * window) >> 8) + 32768;
			*bp++ = sample;
		}
		// update secondary phase
		OscData_sec_phase = phase_2_zt;
	}
	break;

	// WaveformModulated Shruthi ZRESO ---------------------------------
	case WAVEFORM_SHRUTHI_ZRESO:
	{
		uint16_t parameter_zr;
		uint16_t phase_2_zr;
		uint16_t phase_integral_zr;
		uint16_t increment_zr;
		parameter_zr = osc_par_a; // parameter_a
		parameter_zr = parameter_zr + (par_a_mod_ >> 7);
		if (parameter_zr >= 255)
		{
			parameter_zr = 255;
		}
		phase_integral_zr = phase_increment >> 15;
		increment_zr = phase_integral_zr + (phase_integral_zr * uint32_t(parameter_zr) >> 3);
		phase_2_zr = OscData_sec_phase;

		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			ph = phasedata[i] << 1;
			phase_integral_zr = ph >> 16;
			if (ph < phaseOld_)
			{ // phase.carry
				phase_2_zr = 0;
			}
			phaseOld_ = ph;
			phase_2_zr += increment_zr;
			uint16_t carrier;
			carrier = (wav_res_sine16[phase_2_zr >> 8]);
			uint8_t window = 0;
			window = ~(phase_integral_zr >> 8);
			int16_t sample_zr = ((carrier * window) >> 8) + 32768;
			*bp++ = sample_zr;
		}
		// update secondary phase
		OscData_sec_phase = phase_2_zr;
	}
	break;

	// WaveformModulated Shruthi ZPULS ---------------------------------
	case WAVEFORM_SHRUTHI_ZPULSE:
	{
		uint16_t parameter_zp;
		uint16_t phase_2_zp;
		uint16_t phase_integral_zp;
		uint16_t increment_zp;
		parameter_zp = osc_par_a; // parameter_a
		parameter_zp = parameter_zp + (par_a_mod_ >> 7);
		if (parameter_zp >= 255)
		{
			parameter_zp = 255;
		}
		phase_integral_zp = phase_increment >> 15;
		increment_zp = (phase_integral_zp + ((phase_integral_zp * uint32_t(parameter_zp) >> 3))) << 1;
		phase_2_zp = OscData_sec_phase;

		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			ph = phasedata[i] << 1;
			uint16_t phase_integral_zp = ph >> 16;
			if (ph < phaseOld_)
			{						// phase.carry
				phase_2_zp = 32768; // 0 = brighter sound
			}
			phaseOld_ = ph;
			phase_2_zp += increment_zp;
			uint16_t result;
			result = (wav_res_sine16[phase_2_zp >> 8]);
			result >>= 1;
			result += 32768;
			if (phase_integral_zp < 0x4000)
			{
				int16_t sample = result + 32768;
				*bp++ = sample;
			}
			else if (phase_integral_zp < 0x8000)
			{
				uint8_t phase_integral_zp_ = ~(phase_integral_zp - 0x4000) >> 6;
				int16_t sample = ((result * phase_integral_zp_) >> 8) + 32768;
				*bp++ = sample;
			}
			else
			{
				*bp++ = -32767;
			}
		}
		// update secondary phase
		OscData_sec_phase = phase_2_zp;
	}
	break;

	// WaveformModulated Shruthi CHRUSHED_SINE -------------------------
	case WAVEFORM_SHRUTHI_CRUSHED_SINE:
	{
		uint16_t parameter_cs;
		uint8_t decimate_cs;
		int16_t held_sample_cs;
		uint32_t index_cs;
		parameter_cs = osc_par_a; // gain parameter_a
		parameter_cs = parameter_cs + (par_a_mod_ >> 7);
		if (parameter_cs >= 255)
		{
			parameter_cs = 255;
		}
		decimate_cs = Osc_data_cr_decimate;
		held_sample_cs = Osc_data_cr_state;

		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			ph = phasedata[i] << 1;
			index_cs = ph >> 24;
			decimate_cs++;
			if (parameter_cs <= 63)
			{
				if (decimate_cs >= parameter_cs + 1)
				{
					decimate_cs = 0;
					val1 = AudioWaveformSine[index_cs];
					val2 = AudioWaveformSine[index_cs + 1];
					scale = (ph >> 8) & 0xFFFF;
					val2 *= scale;
					val1 *= 0x10000 - scale;
					held_sample_cs = (multiply_32x32_rshift32(val1 + val2, magnitude));
				}
			}
			else if (decimate_cs >= 128 - parameter_cs)
			{
				decimate_cs = 0;
				val1 = AudioWaveformSine[index_cs];
				val2 = AudioWaveformSine[index_cs + 1];
				scale = (ph >> 8) & 0xFFFF;
				val2 *= scale;
				val1 *= 0x10000 - scale;
				held_sample_cs = (multiply_32x32_rshift32(val1 + val2, magnitude));
			}
			*bp++ = held_sample_cs;
		}
		Osc_data_cr_decimate = decimate_cs;
		Osc_data_cr_state = held_sample_cs;
	}
	break;

	// WaveformModulated BRAIDS_CSAW -------------------------
	case WAVEFORM_BRAIDS_CSAW:
		/*
			int32_t parameter_A_cs;
			int32_t parameter_B_cs;
			parameter_A_cs = ((osc_par_a << 8) + (par_a_mod_ << 1));
			parameter_B_cs = ((osc_par_b << 8) + (par_b_mod_ << 1));
			if (parameter_A_cs >= 32768)
			{
				parameter_A_cs = 32768;
			}
			if (parameter_B_cs >= 32768)
			{
				parameter_B_cs = 32768;
			}
			parameter_A_cs = 32768 - parameter_A_cs;

			//int16_t shift_A_cs = (parameter_A_cs - 32767);


			for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
			{
				int32_t s = 0; // *bp + (signed_multiply_32x16t(magnitude, phasedata[i])) + shift_A_cs;
				*bp++ = (s * 13) >> 4;
			}
			*/
	break;

	// WaveformModulated BRAIDS_VOSIM -------------------------
	case WAVEFORM_BRAIDS_VOSIM:
	
		parameter_[0] = (osc_par_a << 8) + par_a_mod_;
		parameter_[1] = (osc_par_b << 8) + par_b_mod_;

		if (parameter_[0] >= 32768)
		{
			parameter_[0] = 32768;
		}
		if (parameter_[1] >= 32768)
		{
			parameter_[1] = 32768;
		}

		for (i = 0; i < 2; ++i)
		{
			state_vow_formant_increment[i] = ComputePhaseIncrement(parameter_[i] >> 1);
		}
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			phase_x = phasedata[i];
			int32_t sample = 16384 + 8192;
			state_vow_formant_phase[0] += state_vow_formant_increment[0];
			sample += Interpolate824(wav_sine, state_vow_formant_phase[0]) >> 1;

			state_vow_formant_phase[1] += state_vow_formant_increment[1];
			sample += Interpolate824(wav_sine, state_vow_formant_phase[1]) >> 2;

			sample = sample * Interpolate824ut(lut_bell, phase_x) >> 15;

			if (phase_x < phaseOld_)
			{
				state_vow_formant_phase[0] = 0;
				state_vow_formant_phase[1] = 0;
				sample = 0;
			}
			phaseOld_ = phase_x;
			sample -= 16384 + 8192;
			*bp++ = sample;
		}
	break;

	// WaveformModulated BRAIDS_TOY -------------------------
	case WAVEFORM_BRAIDS_TOY:
	{
		parameter_[0] = 16384 + (osc_par_a << 7) + par_a_mod_; // 16384 Parameter offset value
		parameter_[1] = (osc_par_b << 8) + par_b_mod_;

		if (parameter_[0] >= 32768)
		{
			parameter_[0] = 32768;
		}
		if (parameter_[1] >= 32768)
		{
			parameter_[1] = 32768;
		}

		// 4 times oversampling.
		// uint32_t phase_increment_ = phase_increment >> 2;
		// ph = phasedata[0];

		uint16_t decimation_counter = state_toy_decimation_counter;
		uint16_t decimation_count = 512 - (parameter_[0] >> 6);

		uint8_t held_sample_toy = state_toy_held_sample;
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			int32_t filtered_sample = 0;

			for (size_t tap = 0; tap < 4; ++tap)
			{
				// ph += phase_increment_;  // causes pitch distortion
				ph = phasedata[i];
				if (decimation_counter >= decimation_count)
				{
					uint8_t x = parameter_[1] >> 8;
					held_sample_toy = (((ph >> 24) ^ (x << 1)) & (~x)) + (x >> 1);
					decimation_counter = 0;
				}
				filtered_sample += kFIR4Coefficients[tap] * held_sample_toy;
				++decimation_counter;
			}
			*bp++ = ~((filtered_sample >> 8) - kFIR4DcOffset);
		}
		state_toy_held_sample = held_sample_toy;
		state_toy_decimation_counter = decimation_counter;
	}
	break;

	// WaveformModulated BRAIDS_SAWSWARM -------------------------
	case WAVEFORM_BRAIDS_SAWSWARM:
	{
		parameter_[0] = (osc_par_a << 8) + par_a_mod_; // + par_a_mod_;
		parameter_[1] = (osc_par_b << 8) + par_b_mod_; // + par_b_mod_;

		if (parameter_[0] >= 32768)
		{
			parameter_[0] = 32768;
		}
		if (parameter_[1] >= 32768)
		{
			parameter_[1] = 32768;
		}

		int32_t detune = parameter_[0];
		detune = (detune * detune) >> 9;
		uint32_t increments[7];
		pitch_ = phase_increment;

		for (int16_t i = 0; i < 7; ++i)
		{
			int32_t saw_detune = detune * (i - 3);
			int32_t detune_integral = saw_detune >> 16;
			int32_t detune_fractional = saw_detune & 0xffff;
			int32_t increment_a = (pitch_ * detune_integral) >> 11;
			int32_t increment_b = (pitch_ * detune_integral + 1) >> 11;
			increments[i] = increment_a + (((increment_b - increment_a) * detune_fractional) >> 16);
		}

		if (strike_)
		{
			for (size_t i = 0; i < 6; ++i)
			{
				state_saw_phase[i] = random();
			}
			strike_ = false;
		}

		int32_t hp_cutoff = 8192;

		if (parameter_[1] < 10922)
		{
			hp_cutoff += ((parameter_[1] - 10922) * 24) >> 5;
		}
		else
		{
			hp_cutoff += ((parameter_[1] - 10922) * 12) >> 5;
		}
		if (hp_cutoff < 0)
		{
			hp_cutoff = 0;
		}
		else if (hp_cutoff > 32768)
		{
			hp_cutoff = 32768;
		}

		int32_t f = Interpolate824ut(lut_svf_cutoff, hp_cutoff << 17);
		int32_t damp = lut_svf_damp[0];
		int32_t bap = state_saw_bp;
		int32_t lp = state_saw_lp;

		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			int32_t notch, hp, sample;

			// phase incremnts
			phase_x = phasedata[i];
			phase_x += increments[0];
			state_saw_phase[0] += increments[1];
			state_saw_phase[1] += increments[2];
			state_saw_phase[2] += increments[3];
			state_saw_phase[3] += increments[4];
			state_saw_phase[4] += increments[5];
			state_saw_phase[5] += increments[6];

			// compute a sample.
			sample = -28672;
			sample += phase_x >> 19;
			sample += (phase_x + state_saw_phase[0]) >> 19;
			sample += (phase_x + state_saw_phase[1]) >> 19;
			sample += (phase_x + state_saw_phase[2]) >> 19;
			sample += (phase_x + state_saw_phase[3]) >> 19;
			sample += (phase_x + state_saw_phase[4]) >> 19;
			sample += (phase_x + state_saw_phase[5]) >> 19;

			sample = Interpolate88(ws_moderate_overdrive, sample + 32768);

			notch = sample - (bap * damp >> 15);
			lp += f * bap >> 15;
			lp = CLIP(lp);
			hp = notch - lp;
			bap += f * hp >> 15;
			int32_t result = hp;
			result = CLIP(result);
			*bp++ = result;
		}
		state_saw_lp = lp;
		state_saw_bp = bap;
	}
	break;

	// WaveformModulated BRAIDS_Digital_Filter -------------------------
	case WAVEFORM_BRAIDS_ZLPF:
	case WAVEFORM_BRAIDS_ZPKF:
	case WAVEFORM_BRAIDS_ZBPF:
	case WAVEFORM_BRAIDS_ZHPF:

		parameter_[0] = 8192 + (osc_par_a << 8) + par_a_mod_; // + par_a_mod_;
		parameter_[1] = (osc_par_b << 8) + par_b_mod_;		  // + par_b_mod_;

		if (parameter_[0] >= 32768)
		{
			parameter_[0] = 32768;
		}
		if (parameter_[1] >= 32768)
		{
			parameter_[1] = 32768;
		}

		//pitch_ = (pitch_note + 1) * 128;
		pitch_ = (mod_increment >> 20);
		Serial.println(pitch_);
		
		int16_t shifted_pitch = pitch_ + ((parameter_[0] - 2048) >> 1);
		if (shifted_pitch > 16383)
		{
			shifted_pitch = 16383;
		}

		size_t size = 128;
		uint32_t modulator_phase = state_res_modulator_phase;
		uint32_t square_modulator_phase = state_res_square_modulator_phase;
		int32_t square_integrator = state_res_integrator;

		uint8_t filter_type = shape_digital_filter_; // Filter typ 0-3

		uint32_t modulator_phase_increment = state_res_modulator_phase_increment;
		uint32_t target_increment = ComputePhaseIncrement(shifted_pitch);
		uint32_t modulator_phase_increment_increment =
			modulator_phase_increment < target_increment
				? (target_increment - modulator_phase_increment) / size
				: ~((modulator_phase_increment - target_increment) / size);

		
		for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
		{
			phase_x = phasedata[i];
			modulator_phase_increment += modulator_phase_increment_increment;
			modulator_phase += modulator_phase_increment;
			uint16_t integrator_gain = (modulator_phase_increment >> 14);

			square_modulator_phase += modulator_phase_increment;
			if (phase_x < phaseOld_)
			{
				modulator_phase = kPhaseReset[filter_type];
			}
			if ((phase_x << 1) < (phaseOld_ << 1))
			{
				state_res_polarity = !state_res_polarity;
				square_modulator_phase = kPhaseReset[(filter_type & 1) + 2];
			}
			phaseOld_ = phase_x;

			int32_t carrier = Interpolate824(wav_sine, modulator_phase);
			int32_t square_carrier = Interpolate824(wav_sine, square_modulator_phase);

			uint16_t saw = ~(phase_x >> 16);
			uint16_t double_saw = ~(phase_x >> 15);
			uint16_t triangle = (phase_x >> 15) ^ (phase_x & 0x80000000 ? 0xffff : 0x0000);
			uint16_t window = parameter_[1] < 16384 ? saw : triangle;

			int32_t pulse = (square_carrier * double_saw) >> 16;
			if (state_res_polarity)
			{
				pulse = -pulse;
			}
			square_integrator += (pulse * integrator_gain) >> 16;
			square_integrator = CLIP(square_integrator);

			int16_t saw_tri_signal;
			int16_t square_signal;

			if (filter_type & 2)
			{
				saw_tri_signal = (carrier * window) >> 16;
				square_signal = pulse;
			}
			else
			{
				saw_tri_signal = (window * (carrier + 32768) >> 16) - 32768;
				square_signal = square_integrator;
				if (filter_type == 1)
				{
					square_signal = (pulse + square_integrator) >> 1;
				}
			}
			uint16_t balance = (parameter_[1] < 16384 ? parameter_[1] : ~parameter_[1]) << 2;			
			*bp++ = ~Mix(saw_tri_signal, square_signal, balance);
		}
		state_res_modulator_phase = modulator_phase;
		state_res_square_modulator_phase = square_modulator_phase;
		state_res_integrator = square_integrator;
		state_res_modulator_phase_increment = modulator_phase_increment;
	break;

	}

	if (tone_offset)
	{
		bp = block->data;
		end = bp + AUDIO_BLOCK_SAMPLES;
		do
		{
			val1 = *bp;
			*bp++ = signed_saturate_rshift(val1 + tone_offset, 16, 0);
		} while (bp < end);
	}
	if (shapedata)
		release(shapedata);
	transmit(block, 0);
	release(block);
}

// BandLimitedWaveform
#define SUPPORT_SHIFT 4
#define SUPPORT (1 << SUPPORT_SHIFT)
#define PTRMASK ((2 << SUPPORT_SHIFT) - 1)

#define SCALE 16
#define SCALE_MASK (SCALE - 1)
#define N (SCALE * SUPPORT * 2)

#define GUARD_BITS 8
#define GUARD (1 << GUARD_BITS)
#define HALF_GUARD (1 << (GUARD_BITS - 1))

#define DEG180 0x80000000u

#define PHASE_SCALE (0x100000000L / (2 * BASE_AMPLITUDE))

extern "C"
{
	extern const int16_t bandlimit_step_table[258];
}

int32_t BandLimitedWaveform::lookup(int offset)
{
	int off = offset >> GUARD_BITS;
	int frac = offset & (GUARD - 1);

	int32_t a, b;
	if (off < N / 2) // handle odd symmetry by reflecting table
	{
		a = bandlimit_step_table[off + 1];
		b = bandlimit_step_table[off + 2];
	}
	else
	{
		a = -bandlimit_step_table[N - off];
		b = -bandlimit_step_table[N - off - 1];
	}
	return BASE_AMPLITUDE + ((frac * b + (GUARD - frac) * a + HALF_GUARD) >> GUARD_BITS); // interpolated
}

// create a new step, apply its past waveform into the cyclic sample buffer
// and add a step_state object into active list so it can be added for the future samples
void BandLimitedWaveform::insert_step(int offset, bool rising, int i)
{
	while (offset <= (N / 2 - SCALE) << GUARD_BITS)
	{
		if (offset >= 0)
			cyclic[i & 15] += rising ? lookup(offset) : -lookup(offset);
		offset += SCALE << GUARD_BITS;
		i++;
	}

	states[newptr].offset = offset;
	states[newptr].positive = rising;
	newptr = (newptr + 1) & PTRMASK;
}

// generate value for current sample from one active step, checking for the
// dc_offset adjustment at the end of the table.
int32_t BandLimitedWaveform::process_step(int i)
{
	int off = states[i].offset;
	bool positive = states[i].positive;

	int32_t entry = lookup(off);
	off += SCALE << GUARD_BITS;
	states[i].offset = off;		// update offset in table for next sample
	if (off >= N << GUARD_BITS) // at end of step table we alter dc_offset to extend the step into future
		dc_offset += positive ? 2 * BASE_AMPLITUDE : -2 * BASE_AMPLITUDE;

	return positive ? entry : -entry;
}

// process all active steps for current sample, basically generating the waveform portion
// due only to steps
// square waves use this directly.
int32_t BandLimitedWaveform::process_active_steps(uint32_t new_phase)
{
	int32_t sample = dc_offset;

	int step_count = (newptr - delptr) & PTRMASK;
	if (step_count > 0) // for any steps in-flight we sum in table entry and update its state
	{
		int i = newptr;
		do
		{
			i = (i - 1) & PTRMASK;
			sample += process_step(i);
		} while (i != delptr);
		if (states[delptr].offset >= N << GUARD_BITS) // remove any finished entries from the buffer.
		{
			delptr = (delptr + 1) & PTRMASK;
			// can be upto two steps per sample now for pulses
			if (newptr != delptr && states[delptr].offset >= N << GUARD_BITS)
				delptr = (delptr + 1) & PTRMASK;
		}
	}
	return sample;
}

// for sawtooth need to add in the slope and compensate for all the steps being one way
int32_t BandLimitedWaveform::process_active_steps_saw(uint32_t new_phase)
{
	int32_t sample = process_active_steps(new_phase);

	sample += (int16_t)((((uint64_t)phase_word * (2 * BASE_AMPLITUDE)) >> 32) - BASE_AMPLITUDE); // generate the sloped part of the wave

	if (new_phase < DEG180 && phase_word >= DEG180) // detect wrap around, correct dc offset
		dc_offset += 2 * BASE_AMPLITUDE;

	return sample;
}

// for pulse need to adjust the baseline according to the pulse width to cancel the DC component.
int32_t BandLimitedWaveform::process_active_steps_pulse(uint32_t new_phase, uint32_t pulse_width)
{
	int32_t sample = process_active_steps(new_phase);

	return sample + BASE_AMPLITUDE / 2 - pulse_width / (0x80000000u / BASE_AMPLITUDE); // correct DC offset for duty cycle
}

// Check for new steps using the phase update for the current sample for a square wave
void BandLimitedWaveform::new_step_check_square(uint32_t new_phase, int i)
{
	if (new_phase >= DEG180 && phase_word < DEG180) // detect falling step
	{
		int32_t offset = (int32_t)((uint64_t)(SCALE << GUARD_BITS) * (sampled_width - phase_word) / (new_phase - phase_word));
		if (offset == SCALE << GUARD_BITS)
			offset--;
		if (pulse_state) // guard against two falling steps in a row (if pulse width changing for instance)
		{
			insert_step(-offset, false, i);
			pulse_state = false;
		}
	}
	else if (new_phase < DEG180 && phase_word >= DEG180) // detect wrap around, rising step
	{
		int32_t offset = (int32_t)((uint64_t)(SCALE << GUARD_BITS) * (-phase_word) / (new_phase - phase_word));
		if (offset == SCALE << GUARD_BITS)
			offset--;
		if (!pulse_state) // guard against two rising steps in a row (if pulse width changing for instance)
		{
			insert_step(-offset, true, i);
			pulse_state = true;
		}
	}
}

// Checking for new steps for pulse waveform has to deal with changing frequency and pulse width and
// not letting a pulse glitch out of existence as these change across a single period of the waveform
// now we detect the rising edge just like for a square wave and use that to sample the pulse width
// parameter, which then has to be checked against the instantaneous frequency every sample.
void BandLimitedWaveform::new_step_check_pulse(uint32_t new_phase, uint32_t pulse_width, int i)
{
	if (pulse_state && phase_word < sampled_width && (new_phase >= sampled_width || new_phase < phase_word)) // falling edge
	{
		int32_t offset = (int32_t)((uint64_t)(SCALE << GUARD_BITS) * (sampled_width - phase_word) / (new_phase - phase_word));
		if (offset == SCALE << GUARD_BITS)
			offset--;
		insert_step(-offset, false, i);
		pulse_state = false;
	}
	if ((!pulse_state) && phase_word >= DEG180 && new_phase < DEG180) // detect wrap around, rising step
	{
		// sample the pulse width value so its not changing under our feet later in cycle due to modulation
		sampled_width = pulse_width;

		int32_t offset = (int32_t)((uint64_t)(SCALE << GUARD_BITS) * (-phase_word) / (new_phase - phase_word));
		if (offset == SCALE << GUARD_BITS)
			offset--;
		insert_step(-offset, true, i);
		pulse_state = true;

		if (pulse_state && new_phase >= sampled_width) // detect falling step directly after a rising edge
		// if (new_phase - sampled_width < DEG180) // detect falling step directly after a rising edge
		{
			int32_t offset = (int32_t)((uint64_t)(SCALE << GUARD_BITS) * (sampled_width - phase_word) / (new_phase - phase_word));
			if (offset == SCALE << GUARD_BITS)
				offset--;
			insert_step(-offset, false, i);
			pulse_state = false;
		}
	}
}

// new steps for sawtooth are at 180 degree point, always falling.
void BandLimitedWaveform::new_step_check_saw(uint32_t new_phase, int i)
{
	if (new_phase >= DEG180 && phase_word < DEG180) // detect falling step
	{
		int32_t offset = (int32_t)((uint64_t)(SCALE << GUARD_BITS) * (DEG180 - phase_word) / (new_phase - phase_word));
		if (offset == SCALE << GUARD_BITS)
			offset--;
		insert_step(-offset, false, i);
	}
}

// the generation function pushd new sample into cyclic buffer, having taken out the oldest entry
// to return.  The output is thus 16 samples behind, which allows the non-casual step function to
// work in real time.
int16_t BandLimitedWaveform::generate_sawtooth(uint32_t new_phase, int i)
{
	new_step_check_saw(new_phase, i);
	int32_t val = process_active_steps_saw(new_phase);
	int16_t sample = (int16_t)cyclic[i & 15];
	cyclic[i & 15] = val;
	phase_word = new_phase;
	return sample;
}

int16_t BandLimitedWaveform::generate_square(uint32_t new_phase, int i)
{
	new_step_check_square(new_phase, i);
	int32_t val = process_active_steps(new_phase);
	int16_t sample = (int16_t)cyclic[i & 15];
	cyclic[i & 15] = val;
	phase_word = new_phase;
	return sample;
}

int16_t BandLimitedWaveform::generate_pulse(uint32_t new_phase, uint32_t pulse_width, int i)
{
	new_step_check_pulse(new_phase, pulse_width, i);
	int32_t val = process_active_steps_pulse(new_phase, pulse_width);
	int32_t sample = cyclic[i & 15];
	cyclic[i & 15] = val;
	phase_word = new_phase;
	return (int16_t)((sample >> 1) - (sample >> 5)); // scale down to avoid overflow on narrow pulses, where the DC shift is big
}

void BandLimitedWaveform::init_sawtooth(uint32_t freq_word)
{
	phase_word = 0;
	newptr = 0;
	delptr = 0;
	for (int i = 0; i < 2 * SUPPORT; i++)
		phase_word -= freq_word;
	dc_offset = phase_word < DEG180 ? BASE_AMPLITUDE : -BASE_AMPLITUDE;
	for (int i = 0; i < 2 * SUPPORT; i++)
	{
		uint32_t new_phase = phase_word + freq_word;
		new_step_check_saw(new_phase, i);
		cyclic[i & 15] = (int16_t)process_active_steps_saw(new_phase);
		phase_word = new_phase;
	}
}

void BandLimitedWaveform::init_square(uint32_t freq_word)
{
	init_pulse(freq_word, DEG180);
}

void BandLimitedWaveform::init_pulse(uint32_t freq_word, uint32_t pulse_width)
{
	phase_word = 0;
	sampled_width = pulse_width;
	newptr = 0;
	delptr = 0;
	for (int i = 0; i < 2 * SUPPORT; i++)
		phase_word -= freq_word;

	if (phase_word < pulse_width)
	{
		dc_offset = BASE_AMPLITUDE;
		pulse_state = true;
	}
	else
	{
		dc_offset = -BASE_AMPLITUDE;
		pulse_state = false;
	}

	for (int i = 0; i < 2 * SUPPORT; i++)
	{
		uint32_t new_phase = phase_word + freq_word;
		new_step_check_pulse(new_phase, pulse_width, i);
		cyclic[i & 15] = (int16_t)process_active_steps_pulse(new_phase, pulse_width);
		phase_word = new_phase;
	}
}

BandLimitedWaveform::BandLimitedWaveform()
{
	newptr = 0;
	delptr = 0;
	dc_offset = BASE_AMPLITUDE;
	phase_word = 0;
}

// Braids Vowel waveform
int16_t InterpolateFormantParameter(
	const int16_t table[][kNumFormants][kNumFormants],
	int16_t x,
	int16_t y,
	uint8_t formant)
{
	uint16_t x_index = x >> 13;
	uint16_t x_mix = x << 3;
	uint16_t y_index = y >> 13;
	uint16_t y_mix = y << 3;
	int16_t a = table[x_index][y_index][formant];
	int16_t b = table[x_index + 1][y_index][formant];
	int16_t c = table[x_index][y_index + 1][formant];
	int16_t d = table[x_index + 1][y_index + 1][formant];
	a = a + ((b - a) * x_mix >> 16);
	c = c + ((d - c) * x_mix >> 16);
	return a + ((c - a) * y_mix >> 16);
}
