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
 * \file events.h
 * Functions to interact with MOAB event logs.
 *
 * \ingroup MOAB
 * \{
 **/

#ifndef EVENTS_H
#define EVENTS_H

#ifdef __cplusplus
extern "C"
{
#endif

#define V_STRCMP        _SIDD_UBYTE_OPS|_SIDD_CMP_EQUAL_EACH|_SIDD_MASKED_POSITIVE_POLARITY

#define X_QUOTE(a)      ((#a)[0])
#define X_ENUM(a, b)    a =(int)b
#define X_ARRAY(a, b)   [a] = b,
#define X_PROTO(a, b)   int32_t b(const char *restrict, void *);

/** Event table.
 * Provides a lookup table/array. The elements are
 * - single letter that is the first letter of the event name in the
 *   event log file.
 * - function name to process that event line.
 **/
#define EVENTS_TABLE(X)    \
	X('j', event_job)  \
	X('r', event_rsv)

/** Linked list structure for events **/
struct event {
	int32_t epoch;
	int64_t id;
	int64_t nodes;
	time_t start;
	time_t end;
	char *name;
	struct event *next;
};

/** Function pointer definition for a line matching an event **/
typedef int32_t (*event_fp)(const char *restrict, void *);

/** Obtain the full path to an event log file **/
int32_t event_file(int32_t, const char *, char **);

/** Parse an event log file for reservation records **/
int32_t event_search(const char *, int32_t, void *);

/** Generate event function pointers definitions **/
EVENTS_TABLE(X_PROTO)

#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif                          /* EVENTS_H */
/**
 * \}
 **/
