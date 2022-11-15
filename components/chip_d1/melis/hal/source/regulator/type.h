/*
 *  drivers/standby/type.h
 *
 * Copyright (c) 2018 Allwinner.
 * 2018-09-14 Written by fanqinghua (fanqinghua@allwinnertech.com).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef __TYPE_H__
#define __TYPE_H__

#define readb(addr)		(*((volatile unsigned char *)(long)(addr)))
#define readw(addr)		(*((volatile unsigned short *)(long)(addr)))
#define readl(addr)		(*((volatile unsigned int *)(long)(addr)))
#define writeb(v, addr)	(*((volatile unsigned char *)(long)(addr)) = (unsigned char)(v))
#define writew(v, addr)	(*((volatile unsigned short *)(long)(addr)) = (unsigned short)(v))
#define writel(v, addr)	(*((volatile unsigned int *)(long)(addr)) = (unsigned int)(v))

typedef char		bool;
typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef int64_t		s64;

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;

#define true	1
#define false	0

#define BIT(nr)			(1 << (nr))

/* Compute the number of elements in the given array */
#define ARRAY_SIZE(a)				\
	(sizeof(a) / sizeof((a)[0]))

#endif /*__TYPE_H__*/
