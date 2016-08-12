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
 * \file res.c
 * Reservation routines.
 *
 * \ingroup res
 * \{
 **/

#include "atts.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <sysexits.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
#include <unistd.h>

#include "mem.h"
#include "events.h"
#include "projects.h"

/**
 * Read reservation names from a generated reservation
 * configuration file.
 *
 * \param[in] filename     The name of the reservation file.
 * \param[out] projects    An array of reservation structures.
 * \param[out] n           The number of reservations.
 * \retval 0               If there file was read successfully.
 * \retval 1               If there was an error reading the file.
 **/
int32_t
project_rsv(const char *filename,
	    struct project **projects)
{
	static const char r[] = "### Reservation ([A-Za-z0-9-]+)-([0-9]{2})z";

	int32_t ierr        = 0;            /* Error number */
	int32_t rlen        = 0;            /* Regex error & match length */
	int32_t seen        = 0;            /* Flag to set if duplicate name */
	FILE *ifp           = NULL;         /* Input file pointer */
	char line[LINE_MAX];                /* Read line from file */
	char *rstr          = NULL;         /* Regex error string */
	regex_t rq          = {0};          /* Compiled regex query */
	regmatch_t rm[3]    = {0};          /* Regex match positions array */
	struct project *p   = NULL;

	memset(line, 0, LINE_MAX * sizeof(char));

	if ((ierr = regcomp(&rq, r, REG_EXTENDED)) != 0) {
		rlen = regerror(ierr, &rq, NULL, 0);
		rstr = xmalloc((rlen+1) * sizeof(char));
		regerror(ierr, &rq, rstr, rlen);
		warnx("unable to compile regex '%s': %s", r, rstr);
		goto rtn_err;
	}

	if ((ifp = fopen(filename, "r")) == NULL) {
		warn("unable to open reservation file %s", filename);
		goto rtn_err;
	}

	while (fgets(line, LINE_MAX, ifp) != NULL) {
		if (regexec(&rq, &line[0], 3, rm, 0) == 0) {
			rlen = (rm[1].rm_eo - rm[1].rm_so +1);
			seen = 0;
			p = *projects;
			while (p != NULL) {
				if (strncmp(p->name,
					    line + rm[1].rm_so, rlen -1) == 0) {
					p->epochs[p->nepochs] = strtol(line +rm[2].rm_so, NULL, 10);
					p->nepochs += 1;
					seen = 1;
				}
				p = p->next;
			}
			if (!seen) {
				rlen *= sizeof(char);
				p = xmalloc(sizeof(struct project));
				p->name = xmalloc(rlen *sizeof(char));
				strncpy(p->name, line + rm[1].rm_so, rlen -1);
				p->nepochs = 1;
				p->epochs[0] = strtol(line +rm[2].rm_so, NULL, 10);
				p->next = *projects;
				*projects = p;
			}
		}
	}
	ierr = 0;

rtn_err:
	regfree(&rq);
	if (rstr) {
		free(rstr);
		rstr = NULL;
	}
	if (ifp) {
		fclose(ifp);
		ifp = NULL;
	}
	return(ierr);
}
/**
 * \}
 **/
