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


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/time.h>
#include "common.h"


int verbose = 0;
FILE *logfd;


uint32_t mtime(void);


/* Function: _log
 * --------------
 *  Log to a file or stdout
 *
 *  level:	'E' for error, 'I' for info, 'D' for debug (verbose).
 *  msg:	Message to log.
 *
 *  Returns:	Value of fprintf, or 1 if message was ignored.
 */
int _log(char level, const char *fmt, ...)
{
	int lasterr = errno;
	va_list arg;
	char buf[1024];
	char ts[64];

	time_t t = (mtime() / 1000);
	struct tm *lt = localtime(&t);
	strftime(ts, 64, "%H:%M:%S", lt);

	if (level != 'D' || verbose) {
		va_start(arg, fmt);
		vsnprintf(buf, 1023, fmt, arg);

		if (level == 'E') {
			int n = strlen(buf) - 1;
			snprintf((buf + n), (1023 - n), ": %s\r", strerror(lasterr));
		}

		va_end(arg);

		return fprintf(logfd, "%c:[%s] %s\r\r", level, ts, buf);
	} else {
		return 1;
	}
}


/* Function: log_close
 * -------------------
 *  Close the log file for shutdown.
 */
void log_close(void)
{
	fclose(logfd);
}
