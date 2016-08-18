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

#include "atts.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <err.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <regex.h>
#include <string.h>
#include <nmmintrin.h>

#include "config.h"
#include "mem.h"
#include "events.h"
#include "projects.h"

/**
 * Generate the full filename for an event log file.
 *
 * @param[in]  offset    The time offset in days (from today).
 * @param[in]  dir       The MOAB stats directory.
 * @param[out] filename  The absolute filename of the event log.
 * @retval     0         If it was sucessful
 * @retval     1         If there was an error
 **/
int32_t
event_file(int32_t offset,
	   const char *dir,
	   char **filename)
{
	long fmax    = 0;
	long pmax    = 0;
	time_t now   = 0;
	struct tm *t = NULL;
	char *tstr   = NULL;

	if ((fmax = pathconf(dir, _PC_NAME_MAX)) == -1) {
		warn("unable to determine filename length for: %s", dir);
		return(EXIT_FAILURE);
	}
	if ((pmax = pathconf(dir, _PC_PATH_MAX)) == -1) {
		warn("unable to determine path length for: %s", dir);
		return(EXIT_FAILURE);
	}

	now = time(NULL) + (offset * SECS_IN_DAY);
	t = gmtime(&now);
	tstr = xmalloc(fmax * sizeof(char));
	strftime(tstr, fmax, "events.%a_%b_%d_%Y", t);

	*filename = xmalloc((pmax + fmax) * sizeof(char));

	sprintf(*filename, "%s/%s", dir, tstr);

	if (tstr) {
		free(tstr);
		tstr = NULL;
	}

	return(EXIT_SUCCESS);
}

int32_t
event_search(const char *stats_dir,
	     int32_t offset,
	     void *vptr)
{
	int32_t ierr   = 0;           /* Error number */
	int32_t eoff   = 0;           /* Offset to the event type */
	size_t i       = 0;           /* Loop indexer */
	size_t lmax    = PAGE_SIZE*4; /* The event log has long lines */
	ssize_t nlen   = 0;           /* Length of the line read */
	size_t nfps    = 0;           /* Size of the fps array */
	int64_t ln     = 0;           /* Current line number */
	FILE *ifp      = NULL;        /* Input file pointer */
	char *filename = NULL;        /* Event log filename */
	char *line     = NULL;        /* Line read from the file */
	const event_fp fps[] = {      /* Array of event function pointers */
		EVENTS_TABLE(X_ARRAY)
	};

	line = xmemalign(lmax * sizeof(char));
	nfps = sizeof(fps)/sizeof(event_fp);

	if ((ierr = event_file(offset, stats_dir, &filename)) !=0) {
		goto rtn_err;
	}

	printf("Event log: %s\n", filename);
	if ((ifp = fopen(filename, "r")) == NULL) {
		warn("unable to open event log %s", filename);
		ierr = EXIT_FAILURE;
		goto rtn_err;
	}

	/* get the first line to figure out the offset to the event type */
	if (fgets(line, lmax, ifp) == NULL) {
		warn("unable to obtain an entry from %s", filename);
		ierr = EXIT_FAILURE;
		goto rtn_err;
	}
	for (i=0; i < lmax; ++i) {
		if (isalpha(line[i])) {
			eoff = i;
			break;
		}
	}

	if (line[eoff] < nfps && fps[line[eoff]] != 0) {
		fps[line[eoff]](line, vptr);
	}

	while ((nlen = getline(&line, &lmax, ifp)) != -1) {
		++ln;
		/*
		if ((ln % 10000) == 0) {
			printf("Processed: %d\n", ln);
		}
		*/
		if (line[eoff] < nfps && fps[line[eoff]] != 0) {
			fps[line[eoff]](line, vptr);
		}
	}

	/*
	printf("# lines: %d\n", ln);
	*/

rtn_err:
	if (line) {
		free(line);
		line = NULL;
	}

	if (ifp) {
		fclose(ifp);
		ifp = NULL;
	}

	return(ierr);
}

int32_t
event_rsv(const char *restrict line,
	  void *vptr
	  )
{
	static const char r[] = "RSVEND.*"
				"NAME=([A-za-z0-9-]+)-([0-9]{2})z\.([0-9]+).*"
				"STARTTIME=([0-9]+).*"
				"ENDTIME=([0-9]+).*"
				"ALLOCTC=([0-9]+).*";
#if 0
				"RSVGROUP=([A-za-z0-9-]+)-([0-9]{2})z";
#endif
	uint8_t update        = 0;
	int32_t ierr          = 0;
	int32_t nlen          = 0;
	char rstr[PAGE_SIZE];
	static regex_t rq     = {0};
	regmatch_t m[7]       = {0};
	struct event *res     = NULL;
	struct event *tmp     = NULL;
	struct project *p     = NULL;
	struct project **projects = (struct project **)vptr;

	/* Create a regex for reservation jobs */
	if ((ierr = regcomp(&rq, r, REG_EXTENDED|REG_NEWLINE)) != 0) {
		regerror(ierr, &rq, rstr, PAGE_SIZE);
		warnx("unable to compile regex '%s': %s", r, rstr);
		return(EXIT_FAILURE);
	}
	if (regexec(&rq, line, 7, m, 0) == 0) {
#if 0
		printf("start: %.*s\tend: %.*s\trsv: %.*s\tnodes: %.*s\n",
		       m[1].rm_eo - m[1].rm_so, line + m[1].rm_so,
		       m[2].rm_eo - m[2].rm_so, line + m[2].rm_so,
		       m[4].rm_eo - m[4].rm_so, line + m[4].rm_so,
		       m[3].rm_eo - m[3].rm_so, line + m[3].rm_so
		       );
#endif
		res = xmalloc(sizeof(struct event));
		nlen = m[1].rm_eo - m[1].rm_so;
		res->name  = xmalloc((nlen +1)*sizeof(char));
		strncpy(res->name, line + m[1].rm_so, nlen);
		res->epoch = strtoul(line + m[2].rm_so, NULL, 10);
		res->id    = strtol(line + m[3].rm_so, NULL, 10);
		res->start = strtol(line + m[4].rm_so, NULL, 10);
		res->end   = strtol(line + m[5].rm_so, NULL, 10);
		res->nodes = strtol(line + m[6].rm_so, NULL, 10);
		/* Search for the reservation within the projects */
		p = *projects;
		while (p != NULL) {
			if (strncmp(res->name, p->name, nlen) == 0) {
				/* MOAB creates a reservation even if all
				 * nodes are not avaliable. It will then
				 * recreate the reservation when more nodes
				 * are added. So we must check to see if
				 * this is an "update" to a reservation.
				 */
				tmp = p->reservations;
				while (tmp != NULL) {
					if (tmp->end == res->end) {
						update = 1;
						break;
					}
					tmp = tmp->next;
				}
				if (update) {
					tmp->start = res->start;
					tmp->nodes = res->nodes;
					free(res->name);
					free(res);
				} else {
					res->next = p->reservations;
					p->reservations = res;
					p->nr += 1;
				}
				break;
			}
			p = p->next;
		}
	}

	/*
	 regfree(&rq);
	 */

	return(EXIT_SUCCESS);
}

int32_t
event_job(const char *restrict line,
	  void *vptr
	  )
{
	int32_t ierr          = 0;
	int32_t nlen          = 0;
	int32_t nodes         = 0;
	static uint32_t joff  = 0;
	char *ptr             = NULL;
	char *sptr            = NULL;
	struct event *job     = NULL;
	struct project *p     = NULL;
	struct project **projects = (struct project **)vptr;
	const char space = ' ';
	const char *jterms[] = {     /* Array of job terms */
		"JOBEND",
		"REQUESTEDNC=",
		"REQRSV=",
		"STARTTIME=",
		"COMPLETETIME=",
		"DRMJID="
	};
	const int jsizes[] = {       /* Size of job terms */
		6, 12, 7, 10, 13, 7
	};

	/* First time looking for JOBEND */
	if (joff == 0) {
		ptr = strstr(line, jterms[0]);
		if (ptr == NULL) {
			return(EXIT_SUCCESS);
		}
		joff = ptr - line;
	}

	/* All other times look for JOBEND */
	if (strncmp(line + joff, jterms[0], jsizes[0]) != 0) {
		return(EXIT_SUCCESS);
	}

	/* Look for REQSRV */
	ptr = strstr(line, jterms[2]);
	if (ptr == NULL) {
		return(EXIT_SUCCESS);
	}

	/* Look for the end of the REQSRV=XXXX string */
	ptr += jsizes[2];
	sptr = strchr(ptr, space);

	/* Create a job event */
	job = xmalloc(sizeof(struct event));

	/* If the job ends with \d\dz, remove them */
	if (*(sptr -1) == 'z'   &&
	    isdigit(*(sptr -2)) &&
	    isdigit(*(sptr -3))) {
		sptr -= 4;  /* remove the leading - too */
		job->epoch = strtoul(sptr+1, NULL, 10);
	}

	nlen = sptr - ptr;
	job->name  = xmalloc((nlen +1)*sizeof(char));
	strncpy(job->name, ptr, nlen);

	/* Search for the job within the projects */
	p = *projects;
	while (p != NULL) {
		if (strcmp(job->name, p->name) == 0) {
			/* Look for the node count */
			job->nodes = 1;
			ptr = strstr(line, jterms[1]);
			if (ptr) {
				ptr += jsizes[1];
				job->nodes = strtol(ptr, NULL, 10);
			}

			/* Look for the STARTTIME */
			sptr = strstr(line, jterms[3]);
			if (!sptr) {
				break;
			}
			sptr += jsizes[3];
			job->start = strtol(sptr, NULL, 10);
			ptr = sptr;

			/* Look for the COMPLETETIME */
			sptr = strstr(ptr, jterms[4]);
			sptr += jsizes[4];
			job->end = strtol(sptr, NULL, 10);
			ptr = sptr;

			/* Look for the TASKMAP */
			/* The problem is this list reports
			 * a node name per task/core.
			sptr = strstr(ptr, jterms[5]);
			sptr += jsizes[5];
			while (*sptr != space) {
				if (*sptr == ',') {
					++nodes;
				}
				++sptr;
			}
			ptr = sptr;
			job->nodes = nodes +1;
			*/

			/* Look for the job id */
			sptr = strstr(ptr, jterms[5]);
			sptr += jsizes[5];
			job->id = strtol(sptr, NULL, 10);
			ptr = sptr;

			p->nj += 1;
			job->next = p->jobs;
			p->jobs = job;
		}
		p = p->next;
	}
	return(EXIT_SUCCESS);
}


/**
 * \}
 **/
