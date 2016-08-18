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
 * \file project.h
 * Internal definitions for proecjt information.
 *
 * \ingroup projects
 * \{
 **/

#ifndef PROJECT_H
#define PROJECT_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Maximum number of reservations **/
#define MAX_RESERVATIONS        256

/** Maximum number of epochs **/
#define MAX_EPOCHS               24

/** Linked list structure for a project **/
struct project {
	uint8_t      nepochs;
	uint8_t      epochs[MAX_EPOCHS];
	int64_t      nr;
	int64_t      nj;
	char         *name;
	struct event *reservations;
	struct event *jobs;
	struct project *next;
};

/** Parse a reservation file to get all the reservations**/
int32_t project_rsv(const char *, struct project **);

#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif                          /* PROJECT_H */
/**
 * \}
 **/
