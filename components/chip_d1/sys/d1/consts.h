/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/* const.h: Macros for dealing with constants.  */

#ifndef _UAPI_LINUX_CONST_H
#define _UAPI_LINUX_CONST_H

/* Some constant macros are used in both assembler and
 * C code.  Therefore we cannot annotate them always with
 * 'UL' and other type specifiers unilaterally.  We
 * use the following macros to deal with this.
 *
 * Similarly, _AT() will cast an expression with a type in C, but
 * leave it unchanged in asm.
 */

#ifdef __ASSEMBLY__
#define _AC(X,Y)    X
#define _AT(T,X)    X
#else
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define __AC(X,Y)   (X##Y)
#define _AC(X,Y)    __AC(X,Y)
#define _AT(T,X)    ((T)(X))

typedef uint64_t phys_addr_t;
#endif

#define _UL(x)      (_AC(x, UL))
#define _ULL(x)     (_AC(x, ULL))

#define _BITUL(x)   (_UL(1) << (x))
#define _BITULL(x)  (_ULL(1) << (x))

#endif  //_UAPI_LINUX_CONST_H 