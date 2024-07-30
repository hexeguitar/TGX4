#include "presetDefault.h"
#include "audioComponents.h"
#include "reverbs.h"

const preset_t presets_default[PRESET_MAX_NO_EEP] PROGMEM = 
{
	/* -----------------------  Preset 1 -----------------------
	 *  Basic clean sound
	 * 		- L in (mono)
	 * 		- Compressor OFF, Booster OFF
	 * 		- Gate = -65dB
	 * 		- Amp: Clean2, all controls max
	 * 		- Cabinet IR: G1
	 * 		- Delay OFF
	 * 		- Reverb: Spring, Mix 0.2, Time 0.65, BassCut 0.46, TrebleCut 0.2
	 */ 
	{
		"Clean",
		.flags  = {
				.bypass = 0,
				.inSelect = FLAG_DEFAULT_INPUT_SELECT,
				.compressorEn = 0,
				.wahEn = 0,
				.wahModel = FLAG_DEFAULT_WAH_MODEL,
				.boostEn = 0,
				.octaveEn = 0,
				.ampModelNo = AMP_CLEAN2,
				.doublerEn = 0,
				.cabIrNo = IR_GUITAR7,
				.delayEn = 0,
				.revebType = REVERB_SPRING,
				.reverbEn = 1,
				.reverbFreezeEn = 0,
				.delayFreezeEn = 0,
				.pot1Assign = PRESET_PARAM_AMP_FXMIX+1,
				.pot2Assign = 0,
				.pot3Assign = 0,
				.expAssign = PARAM_DEFAULT_EXP_ASSIGN,
				.ft1Assign = FLAG_DEFAULT_FT1_ASSIGN,
				.ft2Assign = FLAG_DEFAULT_FT2_ASSIGN,
				.drySigEn = 0,
				.idx = 0
		},
		.fxParams = {
				PARAM_DEFAULT_COMP_PREGAIN,			// 0.  compressor pregain
				PARAM_DEFAULT_COMP_THRES,			// 1.  compressor threshold
				PARAM_DEFAULT_COMP_RATIO,			// 2.  compressor ratio
				PARAM_DEFAULT_COMP_ATTACK,			// 3.  compressor attack
				PARAM_DEFAULT_COMP_RELEASE,			// 4.  compressor release
				PARAM_DEFAULT_COMP_POSTGAIN,		// 5.  compressor post gain
				PARAM_DEFAULT_WAH_FREQ,				// 6.  wah frequency
				PARAM_DEFAULT_WAH_MIX,				// 7.  wah mix
				PARAM_DEFAULT_BOOST_DRIVE,			// 8.  boost drive
				PARAM_DEFAULT_BOOST_BOTTOM,			// 9.  boost bottom
				PARAM_DEFAULT_BOOST_PRESENCE,		// 10.  boost presence
				PARAM_DEFAULT_BOOST_MIX,			// 11.  boost mix
				PARAM_DEFAULT_BOOST_LEVEL,			// 12. boost level
				PARAM_DEFAULT_GATE_THRES,			// 13. gate threshold
				PARAM_DEFAULT_AMP_GAIN,				// 14. amp gain
				PARAM_DEFAULT_AMP_BASS,				// 15. amp bass
				PARAM_DEFAULT_AMP_MID,				// 16. amp mid
				PARAM_DEFAULT_AMP_TREBLE,			// 17. amp treble
				PARAM_DEFAULT_AMP_FXMIX,			// 18. amp fx mix
				PARAM_DEFAULT_DELAY_TIME,			// 19. delay time
				PARAM_DEFAULT_DELAY_INERTIA,		// 20. delay inertia
				PARAM_DEFAULT_DELAY_REPEATS,		// 21. delay repeats
				PARAM_DEFAULT_DELAY_MIX,			// 22. delay mix
				PARAM_DEFAULT_DELAY_HICUT,			// 23. delay hi cut
				PARAM_DEFAULT_DELAY_LOWCUT,			// 24. delay low cut
				PARAM_DEFAULT_DELAY_TREBLE,			// 25. delay treble
				PARAM_DEFAULT_DELAY_BASS,			// 26. delay bass
				PARAM_DEFAULT_DELAY_MODRATE,		// 27. delay mod rate
				PARAM_DEFAULT_DELAY_MODDEPTH,		// 28. delay mod depth
				PARAM_DEFAULT_REVERBPL_SIZE,		// 29. plate reverb size
				PARAM_DEFAULT_REVERBPL_DIFF,		// 30. plate reverb diffusion
				PARAM_DEFAULT_REVERBPL_MIX,			// 31. plate reverb mix
				PARAM_DEFAULT_REVERBPL_BASSCUT,		// 32. plate reverb bass cut
				PARAM_DEFAULT_REVERBPL_TREBLECUT,	// 33. plate reverb treble cut
				PARAM_DEFAULT_REVERBPL_TREBLE,		// 34. plate reverb treble
				PARAM_DEFAULT_REVERBPL_BASS,		// 35. plate reverb bass
				PARAM_DEFAULT_REVERBPL_BLEEDIN,		// 36. plate reverb freeze bleed in
				PARAM_DEFAULT_REVERBPL_PITCH,		// 37. plate reverb pitch
				PARAM_DEFAULT_REVERBPL_PITCHMIX,	// 38. plate reverb pitch mix
				PARAM_DEFAULT_REVERBPL_SHIMMER,		// 39. plate reverb shimmer
				PARAM_DEFAULT_REVERBPL_PITCHSHIMM,	// 40. plate reverb shimmer pitch
				PARAM_DEFAULT_REVERBSC_TIME,		// 41. reverb sc time
				PARAM_DEFAULT_REVERBSC_MIX,			// 42. reverb sc mix
				PARAM_DEFAULT_REVERBSC_LOWPASSF,	// 43. reverb sc lowpass freq
				0.65f,								// 44. spring reverb time
				PARAM_DEFAULT_REVERBSP_MIX,			// 45. spring reverb mix
				0.46f,								// 46. spring reverb bass cut
				0.2f,								// 47. spring reverb treble cut
				PARAM_DEFAULT_MASTER_VOLUME,		// 48. master volume
				PARAM_DEFAULT_MASTER_PAN,			// 49. Master pan
				1.0f,								// 50. delay/reverb mix
				0.3f								// 51. master lowCut filter				
		}
	},
	/* -----------------------  Preset 2 -----------------------
	 * Clean + Chorus + Reverb
	 * 		- L in, Compressor OFF, Booster OFF
	 * 		- Gate -65dB
	 * 		- Amp: Clean3, Doubler ON
	 * 		- Cabinet IR: G6
	 * 		- Delay Time 0.05, Inertia 0, Repeats 0, Mix 0.5, Hi/Lo Cut 0, 
	 * 			Treble/Bass 1.0, ModRate 0.25, ModDepth 0.25
	 * 		- Reverb: Plate, Size 0.85, Diff 1.0, Mix 0.28, BassCut 0.17, 
	 * 			TrebleCut 0.67, Treble/Bass 1.0, no pitch/shimm
 	 */
	{
		"Clean Chorus",
		.flags = {
				.bypass = 0,
				.inSelect = FLAG_DEFAULT_INPUT_SELECT,
				.compressorEn = 0,
				.wahEn = 0,
				.wahModel = FLAG_DEFAULT_WAH_MODEL,
				.boostEn = 0,
				.octaveEn = 0,
				.ampModelNo = AMP_CLEAN3,
				.doublerEn = 1,
				.cabIrNo = IR_GUITAR6,
				.delayEn = 1,
				.revebType = REVERB_PLATE,
				.reverbEn = 1,
				.reverbFreezeEn = 0,
				.delayFreezeEn = 0,
				.pot1Assign = PRESET_PARAM_DELAY_MODRATE+1,
				.pot2Assign = 0,
				.pot3Assign = 0,
				.expAssign = PARAM_DEFAULT_EXP_ASSIGN,
				.ft1Assign = FLAG_DEFAULT_FT1_ASSIGN,
				.ft2Assign = FLAG_DEFAULT_FT2_ASSIGN,
				.drySigEn = 0,
				.idx = 1				
		},
		.fxParams = {
				PARAM_DEFAULT_COMP_PREGAIN,			// 0.  compressor pregain
				PARAM_DEFAULT_COMP_THRES,			// 1.  compressor threshold
				PARAM_DEFAULT_COMP_RATIO,			// 2.  compressor ratio
				PARAM_DEFAULT_COMP_ATTACK,			// 3.  compressor attack
				PARAM_DEFAULT_COMP_RELEASE,			// 4.  compressor release
				PARAM_DEFAULT_COMP_POSTGAIN,		// 5.  compressor post gain
				PARAM_DEFAULT_WAH_FREQ,				// 6.  wah frequency
				PARAM_DEFAULT_WAH_MIX,				// 7.  wah mix				
				PARAM_DEFAULT_BOOST_DRIVE,			// 6.  boost drive
				PARAM_DEFAULT_BOOST_BOTTOM,			// 7.  boost bottom
				PARAM_DEFAULT_BOOST_PRESENCE,		// 8.  boost presence
				PARAM_DEFAULT_BOOST_MIX,			// 9.  boost mix
				PARAM_DEFAULT_BOOST_LEVEL,			// 10. boost level
				PARAM_DEFAULT_GATE_THRES,			// 11. gate threshold
				PARAM_DEFAULT_AMP_GAIN,								// 12. amp gain
				PARAM_DEFAULT_AMP_BASS,				// 13. amp bass
				PARAM_DEFAULT_AMP_MID,				// 14. amp mid
				PARAM_DEFAULT_AMP_TREBLE,			// 15. amp treble
				0.6f,								// 16. amp fx mix
#if ARDUINO_TEENSY40
				0.375f,								// 17. delay time - T40 uses shorter delay time, compensate
#else
				0.05f,								// 17. delay time
#endif
				PARAM_DEFAULT_DELAY_INERTIA,		// 18. delay inertia
				0.0f,								// 19. delay repeats
				PARAM_DEFAULT_DELAY_MIX,			// 20. delay mix
				PARAM_DEFAULT_DELAY_HICUT,			// 21. delay hi cut
				PARAM_DEFAULT_DELAY_LOWCUT,			// 22. delay low cut
				PARAM_DEFAULT_DELAY_TREBLE,			// 23. delay treble
				PARAM_DEFAULT_DELAY_BASS,			// 24. delay bass
				0.25f,								// 25. delay mod rate
				0.25f,								// 26. delay mod depth
				0.85f,								// 27. plate reverb size
				PARAM_DEFAULT_REVERBPL_DIFF,		// 28. plate reverb diffusion
				PARAM_DEFAULT_REVERBPL_MIX,			// 29. plate reverb mix
				0.17f,								// 30. plate reverb bass cut
				0.67f,								// 31. plate reverb treble cut
				PARAM_DEFAULT_REVERBPL_TREBLE,		// 32. plate reverb treble
				PARAM_DEFAULT_REVERBPL_BASS,		// 33. plate reverb bass
				PARAM_DEFAULT_REVERBPL_BLEEDIN,		// 34. plate reverb freeze bleed in
				PARAM_DEFAULT_REVERBPL_PITCH,		// 35. plate reverb pitch
				PARAM_DEFAULT_REVERBPL_PITCHMIX,	// 36. plate reverb pitch mix
				PARAM_DEFAULT_REVERBPL_SHIMMER,		// 37. plate reverb shimmer
				PARAM_DEFAULT_REVERBPL_PITCHSHIMM,	// 38. plate reverb shimmer pitch
				PARAM_DEFAULT_REVERBSC_TIME,		// 39. reverb sc time
				PARAM_DEFAULT_REVERBSC_MIX,			// 40. reverb sc mix
				PARAM_DEFAULT_REVERBSC_LOWPASSF,	// 41. reverb sc lowpass freq
				PARAM_DEFAULT_REVERBSP_TIME,		// 42. spring reverb time
				PARAM_DEFAULT_REVERBSP_MIX,			// 43. spring reverb mix
				PARAM_DEFAULT_REVERBSP_BASSCUT,		// 44. spring reverb bass cut
				PARAM_DEFAULT_REVERBSP_TREBLECUT,	// 45. spring reverb treble cut
				PARAM_DEFAULT_MASTER_VOLUME,		// 46. master volume
				PARAM_DEFAULT_MASTER_PAN,			// 47. Master pan
				0.2f,								// 47. delay/reverb mix
				PARAM_DEFAULT_MASTER_LOWCUT			// 48. master lowCut filter
		}
	},
	/* -----------------------  Preset 3 -----------------------
	 * Blues overdrive
	 * 		- L in, Compressor OFF
	 * 		- Booster: Drive 0.24, Bottom 1.0, Presence 0.35, Mix 0, Level 1.0
	 * 		- Doubler OFF, Amp: Clean 4, Cab: G7
	 * 		- Delay OFF
	 * 		- Reverb SC, Mix 0.28, Time 0.72, LowpassF 0.41
 	 */
	{
		"Blues OD",
		.flags = {
				.bypass = 0,
				.inSelect = FLAG_DEFAULT_INPUT_SELECT,
				.compressorEn = 0,
				.wahEn = 0,
				.wahModel = FLAG_DEFAULT_WAH_MODEL,
				.boostEn = 1,
				.octaveEn = 0,
				.ampModelNo = AMP_CLEAN4,
				.doublerEn = 1,
				.cabIrNo = IR_GUITAR7,
				.delayEn = 0,
				.revebType = REVERB_SC,
				.reverbEn = 1,
				.reverbFreezeEn = 0,
				.delayFreezeEn = 0,
				.pot1Assign = PRESET_PARAM_BOOST_DRIVE+1,
				.pot2Assign = 0,
				.pot3Assign = 0,
				.expAssign = PARAM_DEFAULT_EXP_ASSIGN,
				.ft1Assign = FLAG_DEFAULT_FT1_ASSIGN,
				.ft2Assign = FLAG_DEFAULT_FT2_ASSIGN,
				.drySigEn = 0,
				.idx = 2				
		},
		.fxParams = {
				PARAM_DEFAULT_COMP_PREGAIN,			// 0.  compressor pregain
				PARAM_DEFAULT_COMP_THRES,			// 1.  compressor threshold
				PARAM_DEFAULT_COMP_RATIO,			// 2.  compressor ratio
				PARAM_DEFAULT_COMP_ATTACK,			// 3.  compressor attack
				PARAM_DEFAULT_COMP_RELEASE,			// 4.  compressor release
				PARAM_DEFAULT_COMP_POSTGAIN,		// 5.  compressor post gain
				PARAM_DEFAULT_WAH_FREQ,				// 6.  wah frequency
				PARAM_DEFAULT_WAH_MIX,				// 7.  wah mix				
				0.0f,								// 6.  boost drive
				PARAM_DEFAULT_BOOST_BOTTOM,			// 7.  boost bottom
				0.35f,								// 8.  boost presence
				PARAM_DEFAULT_BOOST_MIX,			// 9.  boost mix
				PARAM_DEFAULT_BOOST_LEVEL,			// 10. boost level
				PARAM_DEFAULT_GATE_THRES,			// 11. gate threshold
				PARAM_DEFAULT_AMP_GAIN,				// 12. amp gain
				PARAM_DEFAULT_AMP_BASS,				// 13. amp bass
				PARAM_DEFAULT_AMP_MID,				// 14. amp mid
				PARAM_DEFAULT_AMP_TREBLE,			// 15. amp treble
				PARAM_DEFAULT_AMP_FXMIX,			// 16. amp fx mix
				PARAM_DEFAULT_DELAY_TIME,			// 17. delay time
				PARAM_DEFAULT_DELAY_INERTIA,		// 18. delay inertia
				PARAM_DEFAULT_DELAY_REPEATS,		// 19. delay repeats
				PARAM_DEFAULT_DELAY_MIX,			// 20. delay mix
				PARAM_DEFAULT_DELAY_HICUT,			// 21. delay hi cut
				PARAM_DEFAULT_DELAY_LOWCUT,			// 22. delay low cut
				PARAM_DEFAULT_DELAY_TREBLE,			// 23. delay treble
				PARAM_DEFAULT_DELAY_BASS,			// 24. delay bass
				PARAM_DEFAULT_DELAY_MODRATE,		// 25. delay mod rate
				PARAM_DEFAULT_DELAY_MODDEPTH,		// 26. delay mod depth
				0.5f,								// 27. plate reverb size
				PARAM_DEFAULT_REVERBPL_DIFF,		// 28. plate reverb diffusion
				PARAM_DEFAULT_REVERBPL_MIX,			// 29. plate reverb mix
				PARAM_DEFAULT_REVERBPL_BASSCUT,		// 30. plate reverb bass cut
				PARAM_DEFAULT_REVERBPL_TREBLECUT,	// 31. plate reverb treble cut
				0.5f,								// 32. plate reverb treble
				PARAM_DEFAULT_REVERBPL_BASS,		// 33. plate reverb bass
				PARAM_DEFAULT_REVERBPL_BLEEDIN,		// 34. plate reverb freeze bleed in
				PARAM_DEFAULT_REVERBPL_PITCH,		// 35. plate reverb pitch
				PARAM_DEFAULT_REVERBPL_PITCHMIX,	// 36. plate reverb pitch mix
				PARAM_DEFAULT_REVERBPL_SHIMMER,		// 37. plate reverb shimmer
				PARAM_DEFAULT_REVERBPL_PITCHSHIMM,	// 38. plate reverb shimmer pitch
				0.72f,								// 39. reverb sc time
				PARAM_DEFAULT_REVERBSC_MIX,			// 40. reverb sc mix
				0.41f,								// 41. reverb sc lowpass freq
				PARAM_DEFAULT_REVERBSP_TIME,		// 42. spring reverb time
				PARAM_DEFAULT_REVERBSP_MIX,			// 43. spring reverb mix
				PARAM_DEFAULT_REVERBSP_BASSCUT,		// 44. spring reverb bass cut
				PARAM_DEFAULT_REVERBSP_TREBLECUT,	// 45. spring reverb treble cut
				PARAM_DEFAULT_MASTER_VOLUME,		// 46. master volume
				PARAM_DEFAULT_MASTER_PAN,			// 47. Master pan
				1.0f,								// 47. delay/reverb mix
				PARAM_DEFAULT_MASTER_LOWCUT			// 48. master lowCut filter
		}
	},
	/* -----------------------  Preset 4 -----------------------
	 * Crunch Solo
  	 * 		- L in, Compressor OFF
 	 * 		- Boost: Gain 0.27, Bottom 1.0, Pres. 1.0, Mix 0, Level 1.0
 	 * 		- Amp Crunch2, Gain 0.5 Cab: G7, rest 1.0
 	 * 		- Delay: Time 0.54, Repeats 0.15, Mix 0.2, ModRate 0.25, ModDepth 0.5
 	 * 		- Reverb: Spring, Mix 0.20, Time 0.64, BassCut 0.46, TrebleCut 0.2
	 */
	{
		"Crunch Solo",
		.flags = {
				.bypass = 0,
				.inSelect = FLAG_DEFAULT_INPUT_SELECT,
				.compressorEn = 0,
				.wahEn = 0,
				.wahModel = FLAG_DEFAULT_WAH_MODEL,
				.boostEn = 1,
				.octaveEn = 0,
				.ampModelNo = AMP_CRUNCH2,
				.doublerEn = 0,
				.cabIrNo = IR_GUITAR7,
				.delayEn = 1,
				.revebType = REVERB_SPRING,
				.reverbEn = 1,
				.reverbFreezeEn = 0,
				.delayFreezeEn = 0,
				.pot1Assign = PRESET_PARAM_DELAY_REVERB_MIX+1,
				.pot2Assign = 0,
				.pot3Assign = 0,
				.expAssign = PARAM_DEFAULT_EXP_ASSIGN,
				.ft1Assign = FLAG_DEFAULT_FT1_ASSIGN,
				.ft2Assign = FLAG_DEFAULT_FT2_ASSIGN,
				.drySigEn = 0,
				.idx = 3				
		},
		.fxParams = {
				PARAM_DEFAULT_COMP_PREGAIN,			// 0.  compressor pregain
				PARAM_DEFAULT_COMP_THRES,			// 1.  compressor threshold
				PARAM_DEFAULT_COMP_RATIO,			// 2.  compressor ratio
				PARAM_DEFAULT_COMP_ATTACK,			// 3.  compressor attack
				PARAM_DEFAULT_COMP_RELEASE,			// 4.  compressor release
				PARAM_DEFAULT_COMP_POSTGAIN,		// 5.  compressor post gain
				PARAM_DEFAULT_WAH_FREQ,				// 6.  wah frequency
				PARAM_DEFAULT_WAH_MIX,				// 7.  wah mix				
				0.0f,								// 6.  boost drive
				PARAM_DEFAULT_BOOST_BOTTOM,			// 7.  boost bottom
				PARAM_DEFAULT_BOOST_PRESENCE,		// 8.  boost presence
				PARAM_DEFAULT_BOOST_MIX,			// 9.  boost mix
				PARAM_DEFAULT_BOOST_LEVEL,			// 10. boost level
				PARAM_DEFAULT_GATE_THRES,			// 11. gate threshold
				0.5f,								// 12. amp gain
				PARAM_DEFAULT_AMP_BASS,				// 13. amp bass
				PARAM_DEFAULT_AMP_MID,				// 14. amp mid
				PARAM_DEFAULT_AMP_TREBLE,			// 15. amp treble
				PARAM_DEFAULT_AMP_FXMIX,			// 16. amp fx mix
#if ARDIONO_TEENSY40
				0.54f,								// 17. delay time
#else
				1.0f,								// 17. delay time
#endif				
				PARAM_DEFAULT_DELAY_INERTIA,		// 18. delay inertia
				0.15f,								// 19. delay repeats
				PARAM_DEFAULT_DELAY_MIX,			// 20. delay mix
				PARAM_DEFAULT_DELAY_HICUT,			// 21. delay hi cut
				PARAM_DEFAULT_DELAY_LOWCUT,			// 22. delay low cut
				PARAM_DEFAULT_DELAY_TREBLE,			// 23. delay treble
				PARAM_DEFAULT_DELAY_BASS,			// 24. delay bass
				0.25f,								// 25. delay mod rate
				0.25f,								// 26. delay mod depth
				PARAM_DEFAULT_REVERBPL_SIZE,		// 27. plate reverb size
				PARAM_DEFAULT_REVERBPL_DIFF,		// 28. plate reverb diffusion
				PARAM_DEFAULT_REVERBPL_MIX,			// 29. plate reverb mix
				PARAM_DEFAULT_REVERBPL_BASSCUT,		// 30. plate reverb bass cut
				PARAM_DEFAULT_REVERBPL_TREBLECUT,	// 31. plate reverb treble cut
				PARAM_DEFAULT_REVERBPL_TREBLE,		// 32. plate reverb treble
				PARAM_DEFAULT_REVERBPL_BASS,		// 33. plate reverb bass
				PARAM_DEFAULT_REVERBPL_BLEEDIN,		// 34. plate reverb freeze bleed in
				PARAM_DEFAULT_REVERBPL_PITCH,		// 35. plate reverb pitch
				PARAM_DEFAULT_REVERBPL_PITCHMIX,	// 36. plate reverb pitch mix
				PARAM_DEFAULT_REVERBPL_SHIMMER,		// 37. plate reverb shimmer
				PARAM_DEFAULT_REVERBPL_PITCHSHIMM,	// 38. plate reverb shimmer pitch
				PARAM_DEFAULT_REVERBSC_TIME,		// 39. reverb sc time
				PARAM_DEFAULT_REVERBSC_MIX,			// 40. reverb sc mix
				PARAM_DEFAULT_REVERBSC_LOWPASSF,	// 41. reverb sc lowpass freq
				0.64f,								// 42. spring reverb time
				PARAM_DEFAULT_REVERBSP_MIX,			// 43. spring reverb mix
				0.46f,								// 44. spring reverb bass cut
				0.2f,								// 45. spring reverb treble cut
				PARAM_DEFAULT_MASTER_VOLUME,		// 46. master volume
				PARAM_DEFAULT_MASTER_PAN,			// 47. Master pan
				0.5f,								// 47. delay / reverb mix
				PARAM_DEFAULT_MASTER_LOWCUT			// 48. master lowCut filter
		}
	},
	/*-----------------------  Preset 5 -----------------------
	 * Hi Gain, Rhythm, stereo
	 * 		- L in, Compressor OFF
	 * 		- Boost: Drive 0.14, Bottom 0.65, Presence 0.76, Mix 0, Level 1.0
	 * 		- Amp: Lead2, Cab: G7, Doubler ON
	 * 		- Delay OFF
	 * 		- Reverb: Spring, Mix 0.11, Time 0.64, BassCut 0.46, TrebleCut 0.2
	 */
	{
		"HiGain Rhythm",
		.flags = {
				.bypass = 0,
				.inSelect = FLAG_DEFAULT_INPUT_SELECT,
				.compressorEn = 0,
				.wahEn = 0,
				.wahModel = FLAG_DEFAULT_WAH_MODEL,
				.boostEn = 1,
				.octaveEn = 0,
				.ampModelNo = AMP_LEAD2,
				.doublerEn = 1,
				.cabIrNo = IR_GUITAR7,
				.delayEn = 0,
				.revebType = REVERB_SPRING,
				.reverbEn = 1,
				.reverbFreezeEn = 0,
				.delayFreezeEn = 0,
				.pot1Assign = PRESET_PARAM_BOOST_BOTTOM+1,
				.pot2Assign = 0,
				.pot3Assign = 0,
				.expAssign = PARAM_DEFAULT_EXP_ASSIGN,
				.ft1Assign = FLAG_DEFAULT_FT1_ASSIGN,
				.ft2Assign = FLAG_DEFAULT_FT2_ASSIGN,
				.drySigEn = 0,
				.idx = 4				
		},
		.fxParams = {
				PARAM_DEFAULT_COMP_PREGAIN,			// 0.  compressor pregain
				PARAM_DEFAULT_COMP_THRES,			// 1.  compressor threshold
				PARAM_DEFAULT_COMP_RATIO,			// 2.  compressor ratio
				PARAM_DEFAULT_COMP_ATTACK,			// 3.  compressor attack
				PARAM_DEFAULT_COMP_RELEASE,			// 4.  compressor release
				PARAM_DEFAULT_COMP_POSTGAIN,		// 5.  compressor post gain
				PARAM_DEFAULT_WAH_FREQ,				// 6.  wah frequency
				PARAM_DEFAULT_WAH_MIX,				// 7.  wah mix				
				0.1f,								// 6.  boost drive
				0.5f,								// 7.  boost bottom
				0.76f,								// 8.  boost presence
				PARAM_DEFAULT_BOOST_MIX,			// 9.  boost mix
				PARAM_DEFAULT_BOOST_LEVEL,			// 10. boost level
				PARAM_DEFAULT_GATE_THRES,			// 11. gate threshold
				PARAM_DEFAULT_AMP_GAIN,				// 12. amp gain
				PARAM_DEFAULT_AMP_BASS,				// 13. amp bass
				PARAM_DEFAULT_AMP_MID,				// 14. amp mid
				PARAM_DEFAULT_AMP_TREBLE,			// 15. amp treble
				0.1f,								// 16. amp fx mix
				PARAM_DEFAULT_DELAY_TIME,			// 17. delay time
				PARAM_DEFAULT_DELAY_INERTIA,		// 18. delay inertia
				PARAM_DEFAULT_DELAY_REPEATS,		// 19. delay repeats
				PARAM_DEFAULT_DELAY_MIX,			// 20. delay mix
				PARAM_DEFAULT_DELAY_HICUT,			// 21. delay hi cut
				PARAM_DEFAULT_DELAY_LOWCUT,			// 22. delay low cut
				PARAM_DEFAULT_DELAY_TREBLE,			// 23. delay treble
				PARAM_DEFAULT_DELAY_BASS,			// 24. delay bass
				PARAM_DEFAULT_DELAY_MODRATE,		// 25. delay mod rate
				PARAM_DEFAULT_DELAY_MODDEPTH,		// 26. delay mod depth
				PARAM_DEFAULT_REVERBPL_SIZE,		// 27. plate reverb size
				PARAM_DEFAULT_REVERBPL_DIFF,		// 28. plate reverb diffusion
				PARAM_DEFAULT_REVERBPL_MIX,			// 29. plate reverb mix
				PARAM_DEFAULT_REVERBPL_BASSCUT,		// 30. plate reverb bass cut
				PARAM_DEFAULT_REVERBPL_TREBLECUT,	// 31. plate reverb treble cut
				PARAM_DEFAULT_REVERBPL_TREBLE,		// 32. plate reverb treble
				PARAM_DEFAULT_REVERBPL_BASS,		// 33. plate reverb bass
				PARAM_DEFAULT_REVERBPL_BLEEDIN,		// 34. plate reverb freeze bleed in
				PARAM_DEFAULT_REVERBPL_PITCH,		// 35. plate reverb pitch
				PARAM_DEFAULT_REVERBPL_PITCHMIX,	// 36. plate reverb pitch mix
				PARAM_DEFAULT_REVERBPL_SHIMMER,		// 37. plate reverb shimmer
				PARAM_DEFAULT_REVERBPL_PITCHSHIMM,	// 38. plate reverb shimmer pitch
				PARAM_DEFAULT_REVERBSC_TIME,		// 39. reverb sc time
				PARAM_DEFAULT_REVERBSC_MIX,			// 40. reverb sc mix
				PARAM_DEFAULT_REVERBSC_LOWPASSF,	// 41. reverb sc lowpass freq
				0.64f,								// 42. spring reverb time
				PARAM_DEFAULT_REVERBSP_MIX,			// 43. spring reverb mix
				0.46f,								// 44. spring reverb bass cut
				0.2f,								// 45. spring reverb treble cut
				PARAM_DEFAULT_MASTER_VOLUME,		// 46. master volume
				PARAM_DEFAULT_MASTER_PAN,			// 47. Master pan
				1.0f,								// 47. delay/reverb mix
				0.2f								// 48. master lowCut filter
		}
	},
	/* -----------------------  Preset 6 -----------------------
	 * Hi Gain, Solo
 	 * 		- L in, Compressor OFF
  	 * 		- Boost: Drive 0.2, Bottom 0.65, Presence 0.75, Mix 0, Level 1
 	 * 		- Amp: Lead1, CabIR: G7
 	 * 		- Delay Time 0.45, Repeats 0.45, Mix 0.24, HiCut 0.34, Lowcut 0.6, 
	 * 			Treble 0.53, Bass 1, ModRate 0.06, ModDepth 1
 	 * 		- Reverb: Spring, Mix 0.11, Time 0.64, BassCut 0.45, TrebleCut 0.2
	 */
	{
		"HiGain Solo",
		.flags = {
				.bypass = 0,
				.inSelect = FLAG_DEFAULT_INPUT_SELECT,
				.compressorEn = 0,
				.wahEn = 0,
				.wahModel = FLAG_DEFAULT_WAH_MODEL,
				.boostEn = 1,
				.octaveEn = 0,
				.ampModelNo = AMP_LEAD1,
				.doublerEn = 0,
				.cabIrNo = IR_GUITAR7,
				.delayEn = 1,
				.revebType = REVERB_SPRING,
				.reverbEn = 1,
				.reverbFreezeEn = 0,
				.delayFreezeEn = 0,
				.pot1Assign = PRESET_PARAM_BOOST_DRIVE+1,
				.pot2Assign = 0,
				.pot3Assign = 0,
				.expAssign = PARAM_DEFAULT_EXP_ASSIGN,
				.ft1Assign = FLAG_DEFAULT_FT1_ASSIGN,
				.ft2Assign = FLAG_DEFAULT_FT2_ASSIGN,
				.drySigEn = 0,
				.idx = 5				
		},
		.fxParams = {
				PARAM_DEFAULT_COMP_PREGAIN,			// 0.  compressor pregain
				PARAM_DEFAULT_COMP_THRES,			// 1.  compressor threshold
				PARAM_DEFAULT_COMP_RATIO,			// 2.  compressor ratio
				PARAM_DEFAULT_COMP_ATTACK,			// 3.  compressor attack
				PARAM_DEFAULT_COMP_RELEASE,			// 4.  compressor release
				PARAM_DEFAULT_COMP_POSTGAIN,		// 5.  compressor post gain
				PARAM_DEFAULT_WAH_FREQ,				// 6.  wah frequency
				PARAM_DEFAULT_WAH_MIX,				// 7.  wah mix				
				0.0f,								// 6.  boost drive
				0.65f,								// 7.  boost bottom
				0.75f,								// 8.  boost presence
				PARAM_DEFAULT_BOOST_MIX,			// 9.  boost mix
				PARAM_DEFAULT_BOOST_LEVEL,			// 10. boost level
				PARAM_DEFAULT_GATE_THRES,			// 11. gate threshold
				PARAM_DEFAULT_AMP_GAIN,				// 12. amp gain
				PARAM_DEFAULT_AMP_BASS,				// 13. amp bass
				PARAM_DEFAULT_AMP_MID,				// 14. amp mid
				PARAM_DEFAULT_AMP_TREBLE,			// 15. amp treble
				0.3f,								// 16. amp fx mix
#if ARDIONO_TEENSY40
				0.45f,								// 17. delay time
#else
				0.7f,								// 17. delay time
#endif	
				PARAM_DEFAULT_DELAY_INERTIA,		// 18. delay inertia
				0.29f,								// 19. delay repeats
				PARAM_DEFAULT_DELAY_MIX,			// 20. delay mix
				0.34f,								// 21. delay hi cut
				0.6f,								// 22. delay low cut
				0.53f,								// 23. delay treble
				PARAM_DEFAULT_DELAY_BASS,			// 24. delay bass
				0.06f,								// 25. delay mod rate
				1.0f,								// 26. delay mod depth
				PARAM_DEFAULT_REVERBPL_SIZE,		// 27. plate reverb size
				PARAM_DEFAULT_REVERBPL_DIFF,		// 28. plate reverb diffusion
				PARAM_DEFAULT_REVERBPL_MIX,			// 29. plate reverb mix
				PARAM_DEFAULT_REVERBPL_BASSCUT,		// 30. plate reverb bass cut
				PARAM_DEFAULT_REVERBPL_TREBLECUT,	// 31. plate reverb treble cut
				PARAM_DEFAULT_REVERBPL_TREBLE,		// 32. plate reverb treble
				PARAM_DEFAULT_REVERBPL_BASS,		// 33. plate reverb bass
				PARAM_DEFAULT_REVERBPL_BLEEDIN,		// 34. plate reverb freeze bleed in
				PARAM_DEFAULT_REVERBPL_PITCH,		// 35. plate reverb pitch
				PARAM_DEFAULT_REVERBPL_PITCHMIX,	// 36. plate reverb pitch mix
				PARAM_DEFAULT_REVERBPL_SHIMMER,		// 37. plate reverb shimmer
				PARAM_DEFAULT_REVERBPL_PITCHSHIMM,	// 38. plate reverb shimmer pitch
				PARAM_DEFAULT_REVERBSC_TIME,		// 39. reverb sc time
				PARAM_DEFAULT_REVERBSC_MIX,			// 40. reverb sc mix
				PARAM_DEFAULT_REVERBSC_LOWPASSF,	// 41. reverb sc lowpass freq
				0.64f,								// 42. spring reverb time
				PARAM_DEFAULT_REVERBSP_MIX,			// 43. spring reverb mix
				0.45f,								// 44. spring reverb bass cut
				0.2f,								// 45. spring reverb treble cut
				PARAM_DEFAULT_MASTER_VOLUME,		// 46. master volume
				PARAM_DEFAULT_MASTER_PAN,			// 47. Master pan
				0.25f,								// 47.  delay/reverb mix
				PARAM_DEFAULT_MASTER_LOWCUT			// 48. master lowCut filter
		}
	},
	/* -----------------------  Preset 7 -----------------------
		Bass Basic, Clean1, compressor, B2 IR
	*/
	{
		"Bass Clean",
		.flags = {
				.bypass = 0,
				.inSelect = FLAG_DEFAULT_INPUT_SELECT,
				.compressorEn = 1,
				.wahEn = 0,
				.wahModel = WAH_MODEL_BASS,
				.boostEn = 0,
				.octaveEn = 0,
				.ampModelNo = AMP_CLEAN1,
				.doublerEn = 0,
				.cabIrNo = IR_BASS2,
				.delayEn = 0,
				.revebType = REVERB_SPRING,
				.reverbEn = 0,
				.reverbFreezeEn = 0,
				.delayFreezeEn = 0,
				.pot1Assign = PRESET_PARAM_AMP_GAIN+1,
				.pot2Assign = 0,
				.pot3Assign = 0,
				.expAssign = PARAM_DEFAULT_EXP_ASSIGN,
				.ft1Assign = FLAG_DEFAULT_FT1_ASSIGN,
				.ft2Assign = FLAG_DEFAULT_FT2_ASSIGN,
				.drySigEn = 0,
				.idx = 6				
		},
		.fxParams = {
				PARAM_DEFAULT_COMP_PREGAIN,			// 0.  compressor pregain
				PARAM_DEFAULT_COMP_THRES,			// 1.  compressor threshold
				PARAM_DEFAULT_COMP_RATIO,			// 2.  compressor ratio
				PARAM_DEFAULT_COMP_ATTACK,			// 3.  compressor attack
				PARAM_DEFAULT_COMP_RELEASE,			// 4.  compressor release
				PARAM_DEFAULT_COMP_POSTGAIN,		// 5.  compressor post gain
				PARAM_DEFAULT_WAH_FREQ,				// 6.  wah frequency
				PARAM_DEFAULT_WAH_MIX,				// 7.  wah mix				
				PARAM_DEFAULT_BOOST_DRIVE,			// 6.  boost drive
				PARAM_DEFAULT_BOOST_BOTTOM,			// 7.  boost bottom
				PARAM_DEFAULT_BOOST_PRESENCE,		// 8.  boost presence
				PARAM_DEFAULT_BOOST_MIX,			// 9.  boost mix
				PARAM_DEFAULT_BOOST_LEVEL,			// 10. boost level
				PARAM_DEFAULT_GATE_THRES,			// 11. gate threshold
				0.65f,								// 12. amp gain
				PARAM_DEFAULT_AMP_BASS,				// 13. amp bass
				PARAM_DEFAULT_AMP_MID,				// 14. amp mid
				PARAM_DEFAULT_AMP_TREBLE,			// 15. amp treble
				0.0f,								// 16. amp fx mix
				PARAM_DEFAULT_DELAY_TIME,			// 17. delay time
				PARAM_DEFAULT_DELAY_INERTIA,		// 18. delay inertia
				PARAM_DEFAULT_DELAY_REPEATS,		// 19. delay repeats
				PARAM_DEFAULT_DELAY_MIX,			// 20. delay mix
				PARAM_DEFAULT_DELAY_HICUT,			// 21. delay hi cut
				PARAM_DEFAULT_DELAY_LOWCUT,			// 22. delay low cut
				PARAM_DEFAULT_DELAY_TREBLE,			// 23. delay treble
				PARAM_DEFAULT_DELAY_BASS,			// 24. delay bass
				PARAM_DEFAULT_DELAY_MODRATE,		// 25. delay mod rate
				PARAM_DEFAULT_DELAY_MODDEPTH,		// 26. delay mod depth
				PARAM_DEFAULT_REVERBPL_SIZE,		// 27. plate reverb size
				PARAM_DEFAULT_REVERBPL_DIFF,		// 28. plate reverb diffusion
				PARAM_DEFAULT_REVERBPL_MIX,			// 29. plate reverb mix
				PARAM_DEFAULT_REVERBPL_BASSCUT,		// 30. plate reverb bass cut
				PARAM_DEFAULT_REVERBPL_TREBLECUT,	// 31. plate reverb treble cut
				PARAM_DEFAULT_REVERBPL_TREBLE,		// 32. plate reverb treble
				PARAM_DEFAULT_REVERBPL_BASS,		// 33. plate reverb bass
				PARAM_DEFAULT_REVERBPL_BLEEDIN,		// 34. plate reverb freeze bleed in
				PARAM_DEFAULT_REVERBPL_PITCH,		// 35. plate reverb pitch
				PARAM_DEFAULT_REVERBPL_PITCHMIX,	// 36. plate reverb pitch mix
				PARAM_DEFAULT_REVERBPL_SHIMMER,		// 37. plate reverb shimmer
				PARAM_DEFAULT_REVERBPL_PITCHSHIMM,	// 38. plate reverb shimmer pitch
				PARAM_DEFAULT_REVERBSC_TIME,		// 39. reverb sc time
				PARAM_DEFAULT_REVERBSC_MIX,			// 40. reverb sc mix
				PARAM_DEFAULT_REVERBSC_LOWPASSF,	// 41. reverb sc lowpass freq
				PARAM_DEFAULT_REVERBSP_TIME,		// 42. spring reverb time
				PARAM_DEFAULT_REVERBSP_MIX,			// 43. spring reverb mix
				PARAM_DEFAULT_REVERBSP_BASSCUT,		// 44. spring reverb bass cut
				PARAM_DEFAULT_REVERBSP_TREBLECUT,	// 45. spring reverb treble cut
				0.48f,								// 46. master volume
				PARAM_DEFAULT_MASTER_PAN,			// 47. Master pan
				0.0f,								// 47. delay / reverb mix
				PARAM_DEFAULT_MASTER_LOWCUT			// 48. master lowCut filter
		}
	},
	/* -----------------------  Preset 8 -----------------------
	 * Bass Drive Stereo, Compressor OFF, 
	 * 		- Boost: Drive 1, Bottom 0, Presence 0.5, Mix 0.4, Level 1
 	 * 		- Amp Clean1, Mid 0.5, Cab: B1
	*/
	{
		"Bass Drive",
		.flags = {
				.bypass = 0,
				.inSelect = FLAG_DEFAULT_INPUT_SELECT,
				.compressorEn = 0,
				.wahEn = 0,
				.wahModel = WAH_MODEL_BASS,
				.boostEn = 1,
				.octaveEn = 0,
				.ampModelNo = AMP_CLEAN1,
				.doublerEn = 1,
				.cabIrNo = IR_BASS1,
				.delayEn = 0,
				.revebType = REVERB_SPRING,
				.reverbEn = 0,
				.reverbFreezeEn = 0,
				.delayFreezeEn = 0,
				.pot1Assign = PRESET_PARAM_BOOST_DRIVE+1,
				.pot2Assign = 0,
				.pot3Assign = 0,
				.expAssign = PARAM_DEFAULT_EXP_ASSIGN,
				.ft1Assign = FLAG_DEFAULT_FT1_ASSIGN,
				.ft2Assign = FLAG_DEFAULT_FT2_ASSIGN,
				.drySigEn = 0,
				.idx = 7				
		},
		.fxParams = {
				PARAM_DEFAULT_COMP_PREGAIN,			// 0.  compressor pregain
				PARAM_DEFAULT_COMP_THRES,			// 1.  compressor threshold
				PARAM_DEFAULT_COMP_RATIO,			// 2.  compressor ratio
				PARAM_DEFAULT_COMP_ATTACK,			// 3.  compressor attack
				PARAM_DEFAULT_COMP_RELEASE,			// 4.  compressor release
				PARAM_DEFAULT_COMP_POSTGAIN,		// 5.  compressor post gain
				PARAM_DEFAULT_WAH_FREQ,				// 6.  wah frequency
				PARAM_DEFAULT_WAH_MIX,				// 7.  wah mix				
				1.0f,								// 6.  boost drive
				0.0f,								// 7.  boost bottom
				0.5f,								// 8.  boost presence
				0.4f,								// 9.  boost mix
				PARAM_DEFAULT_BOOST_LEVEL,			// 10. boost level
				PARAM_DEFAULT_GATE_THRES,			// 11. gate threshold
				PARAM_DEFAULT_AMP_GAIN,				// 12. amp gain
				PARAM_DEFAULT_AMP_BASS,				// 13. amp bass
				0.5f,								// 14. amp mid
				PARAM_DEFAULT_AMP_TREBLE,			// 15. amp treble
				0.0f,								// 16. amp fx mix
				PARAM_DEFAULT_DELAY_TIME,			// 17. delay time
				PARAM_DEFAULT_DELAY_INERTIA,		// 18. delay inertia
				PARAM_DEFAULT_DELAY_REPEATS,		// 19. delay repeats
				PARAM_DEFAULT_DELAY_MIX,			// 20. delay mix
				PARAM_DEFAULT_DELAY_HICUT,			// 21. delay hi cut
				PARAM_DEFAULT_DELAY_LOWCUT,			// 22. delay low cut
				PARAM_DEFAULT_DELAY_TREBLE,			// 23. delay treble
				PARAM_DEFAULT_DELAY_BASS,			// 24. delay bass
				PARAM_DEFAULT_DELAY_MODRATE,		// 25. delay mod rate
				PARAM_DEFAULT_DELAY_MODDEPTH,		// 26. delay mod depth
				PARAM_DEFAULT_REVERBPL_SIZE,		// 27. plate reverb size
				PARAM_DEFAULT_REVERBPL_DIFF,		// 28. plate reverb diffusion
				PARAM_DEFAULT_REVERBPL_MIX,			// 29. plate reverb mix
				PARAM_DEFAULT_REVERBPL_BASSCUT,		// 30. plate reverb bass cut
				PARAM_DEFAULT_REVERBPL_TREBLECUT,	// 31. plate reverb treble cut
				PARAM_DEFAULT_REVERBPL_TREBLE,		// 32. plate reverb treble
				PARAM_DEFAULT_REVERBPL_BASS,		// 33. plate reverb bass
				PARAM_DEFAULT_REVERBPL_BLEEDIN,		// 34. plate reverb freeze bleed in
				PARAM_DEFAULT_REVERBPL_PITCH,		// 35. plate reverb pitch
				PARAM_DEFAULT_REVERBPL_PITCHMIX,	// 36. plate reverb pitch mix
				PARAM_DEFAULT_REVERBPL_SHIMMER,		// 37. plate reverb shimmer
				PARAM_DEFAULT_REVERBPL_PITCHSHIMM,	// 38. plate reverb shimmer pitch
				PARAM_DEFAULT_REVERBSC_TIME,		// 39. reverb sc time
				PARAM_DEFAULT_REVERBSC_MIX,			// 40. reverb sc mix
				PARAM_DEFAULT_REVERBSC_LOWPASSF,	// 41. reverb sc lowpass freq
				PARAM_DEFAULT_REVERBSP_TIME,		// 42. spring reverb time
				PARAM_DEFAULT_REVERBSP_MIX,			// 43. spring reverb mix
				PARAM_DEFAULT_REVERBSP_BASSCUT,		// 44. spring reverb bass cut
				PARAM_DEFAULT_REVERBSP_TREBLECUT,	// 45. spring reverb treble cut
				PARAM_DEFAULT_MASTER_VOLUME,		// 46. master volume
				PARAM_DEFAULT_MASTER_PAN,			// 47. Master pan
				0.0f,								// 47. delay / reverb mix
				PARAM_DEFAULT_MASTER_LOWCUT			// 48. master lowCut filter
		}
	}
};