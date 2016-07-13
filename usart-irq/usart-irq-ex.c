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


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "clock.h"
#include "usart.h"
#include "common.h"


/* From lib/common.c */
/* Log file descriptor */
extern FILE *logfd;
/* For verbosity */
extern int verbose;

/* Less magick numbers == more */
#define DELAY	1000


/* Function: main
 * --------------
 *  Initiatialize clocks and USART. Setup output options.
 *  Loop infinitely doing output using a convenience log function and a debug macro.
 *
 *  Returns:	0, never.
 */
int main(void)
{
	clock_init();
	usart_init();
	logfd = stdout;
	verbose = 1;

	printf("\r\n\r\n-------------\r\nStarting up..\r\n-------------\r\n");

	while (2 > 1) {
		c_log('I', "USART is alive!");
		msleep(DELAY);
		DBG();
		msleep(DELAY);
	}

	/* if you're happy and you know it, */
	return 0;
}
