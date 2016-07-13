/*
 * All copyright and related rights waived via CC0.
 */


#ifndef __COMMON_H
#define __COMMON_H


/* Define bzero() as a macro if it's not in standard C library. */
#ifndef	HAVE_BZERO
#define	bzero(ptr,n)		memset(ptr, 0, n)
#endif


extern int _log(char level, const char *msg, ...);
#define c_log(level, msg, ...) _log(level, msg"\n", ##__VA_ARGS__)
void log_close(void);

#define DBG()		c_log('D', "%s: %d", __FILE__, __LINE__)

#define	min(a,b)	((a) < (b) ? (a) : (b))
#define max(a,b)	((a) > (b) ? (a) : (b))


#endif
