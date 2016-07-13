/*
 * Copyright (c) 2016 Centropy <contact@centropy.info>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <unicore-mx/stm32/rcc.h>
#include <unicore-mx/cm3/nvic.h>
#include <unicore-mx/cm3/systick.h>
#include "clock.h"


static volatile uint32_t millis;


/* Function: sys_tick_handler
 * --------------------------
 *  Systick handler.
 */
void sys_tick_handler(void)
{
	millis++;
}


/* Function: msleep
 * ----------------
 *  Sleep for a number of milliseconds.
 *
 *  delay:	Milliseconds to sleep.
 */
void msleep(uint32_t delay)
{
	uint32_t wake = millis + delay;
	while (wake > millis);
}


/* Function: mtime
 * ---------------
 *  Get the number of milliseconds since power-on.
 *
 *  Returns:	Milliseconds since power-on.
 */
uint32_t mtime(void)
{
	return millis;
}


/* Function: clock_init
 * --------------------
 *  Initialize the main clock.
 */
void clock_init(void)
{
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

	systick_set_reload(168000);
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();
	systick_interrupt_enable();
}
