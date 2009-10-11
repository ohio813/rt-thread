/*
 * File      : string.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2008, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2008-08-14     Bernard      the first version
 */
#ifndef __STRING_H__
#define __STRING_H__

#include <rtthread.h>
#include <sys/types.h>

/* replace for standard string library */
#if !defined (RT_USING_NEWLIB) && defined (RT_USING_MINILIBC)

#define ZEROPAD 	(1 << 0)	/* pad with zero */
#define SIGN 		(1 << 1)	/* unsigned/signed long */
#define PLUS 		(1 << 2)	/* show plus */
#define SPACE 		(1 << 3)	/* space if plus */
#define LEFT 		(1 << 4)	/* left justified */
#define SPECIAL 	(1 << 5)	/* 0x */
#define LARGE		(1 << 6)	/* use 'ABCDEF' instead of 'abcdef' */

#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((long)(~0UL>>1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)

#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

void* memset(void *s, int c, size_t n);
void* memcpy(void *dest, const void *src, size_t n);
void* memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

int tolower(int c);
int toupper(int c);

int strcmp (const char *s1, const char *s2);
int strcasecmp(const char *a, const char *b);
int strncasecmp(const char *cs, const char *ct, size_t count);
int sscanf(const char * buf, const char * fmt, ...);
size_t strlen(const char *s);
char * strstr(const char * s1,const char * s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strncat(char *s, const char *t, size_t n) ;
char* strcat(register char* s,register const char* t);
char *strrchr(const char *t, int c);
char *strdup(const char *s);

#endif

#endif
