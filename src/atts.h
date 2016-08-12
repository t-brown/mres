/*
 * Copyright (C) 2016 Timothy Brown
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * \file atts.h
 * Definitions for function attributes
 *
 * \ingroup attributes
 * \{
 **/

#ifndef ATTS_H
#define ATTS_H

#ifdef __cplusplus
extern "C"
{
#endif

/** 
 * Define _GNU_SOURCE so as to expose posix_memalign() on Linux.
 * This also sets:
 * - _BSD_SOURCE
 * - _SVID_SOURCE
 * - _ATFILE_SOURCE
 * - _LARGEFILE64_SOURCE
 * - _ISOC99_SOURCE
 * - _XOPEN_SOURCE_EXTENDED
 * - _POSIX_SOURCE
 * - _POSIX_C_SOURCE (200809L)
 * - _XOPEN_SOURCE (700)
 **/
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif

/**
 * Check to see if the compiler has a biggest alignment size.
 **/
#ifdef __BIGGEST_ALIGNMENT__
#  define ALIGNMENT        __BIGGEST_ALIGNMENT__
#else
#  define ALIGNMENT        64
#endif

/**
 * Define our alignment attributes based on the alignment size.
 **/
#if defined(__INTEL__)
#  define ATT_ALIGN        __attribute__((aligned(ALIGNMENT)))
#  define ASSUME_ALIGNED   __assume_aligned
#elif defined(__GNUC__)
#  define ATT_ALIGN        __attribute__((aligned(ALIGNMENT)))
#  define ASSUME_ALIGNED   __builtin_assume_aligned
#else
#  define ATT_ALIGN
#  define ASSUME_ALIGNED
#endif

/**
 * Library visibility and compiler __attribute__ extensions
 **/
#ifdef HAVE___ATTRIBUTE__
#  define ATT_CONSTR       __attribute__((__constructor__))
#  define ATT_DESTR        __attribute__((__destructor__))
#  define ATT_PUBLIC       __attribute__((__visibility__("default")))
#  define ATT_LOCAL        __attribute__((__visibility__("hidden")))
#  define ATT_DEPRECATED   __attribute__((__deprecated__))
#  define ATT_MSIZE(x)     __attribute__((__alloc_size__(x)))
#  define ATT_MALLOC       __attribute__((__malloc__))
#  define ATT_FMT(x,y)     __attribute__((__format__(printf, x, y)))
#  define ATT_NORETURN     __attribute__((__noreturn__))
#  define ATT_INLINE       __attribute__((__always_inline__))
#  define ATT_ALIAS(x)     __attribute__((__weak__, __alias__(x)))
#else
#  define ATT_CONSTR
#  define ATT_DESTR
#  define ATT_PUBLIC
#  define ATT_LOCAL
#  define ATT_DEPRECATED(msg)
#  define ATT_MSIZE(x)
#  define ATT_MALLOC
#  define ATT_FMT(x,y)
#  define ATT_NORETURN
#  define ATT_INLINE
#  define ATT_ALIAS(x)
#endif

/**
 * Define the number of seconds in a day
 **/
#define SECS_IN_DAY        86400

/**
 * Define the system line length maximum
 **/
#ifndef LINE_MAX
#  define LINE_MAX         sysconf(_SC_LINE_MAX)
#endif

/**
 * Define the system page size
 **/
#ifndef PAGE_SIZE
#  define PAGE_SIZE         sysconf(_SC_PAGE_SIZE)
#endif

#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif                          /* ATTS_H */
/**
 * \}
 **/
