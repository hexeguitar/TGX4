#include "system.h"

// PSRAM speed by KurtE
// https://forum.pjrc.com/index.php?threads/teensy-4-1-speed-up-the-clock-for-psram-and-littlefs_qspi.75696/#post-348454

void update_psram_speed(int speed_mhz)
{
	// What clocks exist:
	static const int flexspio2_clock_speeds[] = {396, 720, 665, 528};

	// See what the closest setting might be:
	uint8_t clk_save, divider_save;
	int min_delta = speed_mhz;
	for (uint8_t clk = 0; clk < 4; clk++)
	{
		uint8_t divider = (flexspio2_clock_speeds[clk] + (speed_mhz / 2)) / speed_mhz;
		int delta = abs(speed_mhz - flexspio2_clock_speeds[clk] / divider);
		if ((delta < min_delta) && (divider < 8))
		{
			min_delta = delta;
			clk_save = clk;
			divider_save = divider;
		}
	}

	// first turn off FLEXSPI2
	CCM_CCGR7 &= ~CCM_CCGR7_FLEXSPI2(CCM_CCGR_ON);

	divider_save--; // 0 biased.
	Serial.printf("Update FLEXSPI2 speed: %u clk:%u div:%u Actual:%u\n", speed_mhz, clk_save, divider_save,
				  flexspio2_clock_speeds[clk_save] / (divider_save + 1));

	// Set the clock settings.
	CCM_CBCMR = (CCM_CBCMR & ~(CCM_CBCMR_FLEXSPI2_PODF_MASK | CCM_CBCMR_FLEXSPI2_CLK_SEL_MASK)) | CCM_CBCMR_FLEXSPI2_PODF(divider_save) | CCM_CBCMR_FLEXSPI2_CLK_SEL(clk_save);

	// Turn FlexSPI2 clock back on
	CCM_CCGR7 |= CCM_CCGR7_FLEXSPI2(CCM_CCGR_ON);
}
