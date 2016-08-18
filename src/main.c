/*
 * Copyright (C) 2016  Timothy Brown
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file main.c
 * Main program entry point.
 *
 * \ingroup main
 * \{
 **/

#include "atts.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <hdf5.h>

#include "config.h"
#include "args.h"
#include "mem.h"
#include "events.h"
#include "projects.h"
#include "io.h"

int
main(int argc, char **argv)
{
	int32_t ierr      = 0;
	int32_t i         = 0;
	struct args a     = {0};
	struct project *projects = NULL;
	hid_t  fid        = 0;
	struct project *pptr = NULL;
	/*
	struct event *r   = NULL;
	struct event *j   = NULL;
	struct tm gmts    = {0};
	struct tm gmte    = {0};
	char tstart[20]   = {0};
	char tend[20]     = {0};
	const char tfmt[] = "%Y %m %d %H:%M:%S";
	const char rhdr[] = "Reservation\tEpoch\tStart\t\t\tEnd\t\t\tNodes\n";
	const char rfmt[] = "%-10s\t%02d\t%-20s\t%-20s\t%4d\n";
	const char jfmt[] = "%10d\t\t%-20s\t%-20s\t%4d\n";
	*/


	if (args_parse(argc, argv, &a)) {
		return(EXIT_FAILURE);
	}

	/* Load the reservations */
	if (project_rsv(a.res_file, &projects)) {
		return(EXIT_FAILURE);
	}

	/* Parse the event log */
	if (event_search(a.stats_dir, a.offset, (void *)&projects)) {
		return(EXIT_FAILURE);
	}

	/*
	pptr = projects;
	if (pptr != NULL) {
		printf(rhdr);
	}
	while (pptr != NULL) {
		r = pptr->reservations;
		while (r != NULL) {
			gmtime_r(&(r->start), &gmts);
			gmtime_r(&(r->end),   &gmte);
			strftime(tstart, sizeof(tstart), tfmt, &gmts);
			strftime(tend,   sizeof(tend),   tfmt, &gmte);
			printf(rfmt, r->name, r->epoch, tstart, tend, r->nodes);
			j = pptr->jobs;
			while (j != NULL) {
				if (j->epoch == r->epoch) {
					gmtime_r(&(j->start), &gmts);
					gmtime_r(&(j->end),   &gmte);
					strftime(tstart, sizeof(tstart), tfmt, &gmts);
					strftime(tend,   sizeof(tend),   tfmt, &gmte);
					printf(jfmt, j->id, tstart, tend, j->nodes);
				}
				j = j->next;
			}
			r = r->next;
		}
		pptr = pptr->next;
	}
	*/

	io_open(a.output, &fid);
	pptr = projects;
	while (pptr != NULL) {
		io_write(fid, pptr);
		pptr = pptr->next;
	}
	io_close(fid);

	/* Clean up */
#if 0
	while ((p = pptr) != NULL) {
		pptr = pptr->next;
		r = pptr->reservations;
		while (r1 != NULL) {
			if (r->name) {
				free(r->name);
			}
			free(r);
			r->next;
		}
		j = pptr->jobs;
		while (r != NULL) {
			if (r->name) {
				free(r->name);
			}
			free(r);
		}
		if (pptr->name) {
			free(pptr->name);
		}
		free(p);
	}
#endif

	if (args_free(&a)) {
		return(EXIT_FAILURE);
	}

	return(EXIT_SUCCESS);
}


/**
 * \}
 **/
