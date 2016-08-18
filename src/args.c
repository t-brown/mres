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
 * \file args.c
 *
 * Command line argument parsing routines.
 *
 * \ingroup arguments
 * \{
 **/

#include "atts.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <err.h>

#include "config.h"
#include "mem.h"
#include "args.h"

static char *trim(const char *);

/**
 * Parse the command line arguments.
 *
 * @param[in] argc
 * @param[in] argv
 * @param[out] output     The output filename.
 **/
int32_t
args_parse(int32_t argc, char **argv, struct args *arguments)
{

	int32_t opt = 0;
	int32_t idx = 0;
	char *sopts = "hVvo:s:t:r:R:";
	static struct option lopts[] = {
		{"help",         no_argument,       NULL, 'h'},
		{"version",      no_argument,       NULL, 'V'},
		{"verbose",      no_argument,       NULL, 'v'},
		{"outfile",      required_argument, NULL, 'o'},
		{"sdir",         required_argument, NULL, 's'},
		{"offset",       required_argument, NULL, 't'},
		{"reservation",  required_argument, NULL, 'r'},
		{"rfile",        required_argument, NULL, 'R'},
		{NULL,           0,                 NULL,  0 }
	};

	/* Load the defaults for Jet first */
	arguments->verbose = 0;
	arguments->offset  = 0;
	arguments->stats_dir = xmalloc(strlen(MOAB_STATS_DIR)+1 * sizeof(char));
	strcpy(arguments->stats_dir, MOAB_STATS_DIR);
	arguments->res_file = xmalloc(strlen(RESERVATION_FILE)+1 * sizeof(char));
	strcpy(arguments->res_file, RESERVATION_FILE);

	/* Command line arguments override the defaults */
	while ((opt = getopt_long(argc, argv, sopts, lopts, &idx)) != -1) {
		switch(opt) {
			case 'h':
				print_usage();
				break;
			case 'V':
				print_version();
				break;
			case 'v':
				arguments->verbose = strtol(optarg, NULL, 10);
				break;
			case 'o':
				arguments->output = xmalloc((strlen(optarg)+1) *
						  sizeof(char));
				strcpy(arguments->output, optarg);
				break;
			case 's':
				arguments->stats_dir = xmalloc((strlen(optarg)+1) *
						   sizeof(char));
				strcpy(arguments->stats_dir, optarg);
				break;
			case 't':
				arguments->offset = strtol(optarg, NULL, 10);
				break;
			case 'r':
				arguments->res = xmalloc((strlen(optarg)+1) *
						   sizeof(char));
				strcpy(arguments->res, optarg);
				break;
			case 'R':
				arguments->res_file = xmalloc((strlen(optarg)+1) *
						   sizeof(char));
				strcpy(arguments->res_file, optarg);
				break;
		}
	}

	return(EXIT_SUCCESS);
}
/**
 * Free up an argument structure.
 *
 * @param[in] arguments The argument structure.
 **/
int32_t
args_free(struct args *arguments)
{
	if (arguments->output) {
		free(arguments->output);
		arguments->output = NULL;
	}
	if (arguments->res) {
		free(arguments->res);
		arguments->res = NULL;
	}
	if (arguments->stats_dir) {
		free(arguments->stats_dir);
		arguments->stats_dir = NULL;
	}
	if (arguments->res_file) {
		free(arguments->res_file);
		arguments->res_file = NULL;
	}

	return(EXIT_SUCCESS);
}

/**
 * Trim whitespace from the from and end of a string.
 *
 * The returned string should be free()'ed.
 *
 * @param[in] str The string to be trimmmed.
 * @return        A new string trimmed of whitespace.
 **/
static char *
trim(const char *str)
{
	size_t i = 0;
	size_t j = 0;
	size_t n = 0;
	char *new = NULL;

	n =  strlen(str) + 1;
	new = xmalloc(n * sizeof(char));
	for (i=0; i < n; ++i) {
		if ((str[i] != ' ') && (str[i] != '\t')) {
			new[j] = str[i];
			++j;
		}
		if (str[i] == '\n') {
			break;
		}
	}
	return(new);
}

/**
 * Print a short usage statement.
 **/
ATT_NORETURN
void
print_usage(void)
{
	printf("\
usage: %s [-h] [-V] [-v] [-s DIR] [-t OFFSET] [-r RES] [-R FILE] [-o output]\n\
\n\
  -h,   --help          Display this help and exit.\n\
  -V,   --version       Display version information and exit.\n\
  -v,   --verbose       Increase the verbosity level.\n\
  -s,   --sdir          The MOAB statistics directory.\n\
  -t,   --offset        The offset in days from today to query.\n\
  -r,   --reservation   A single reservation name to query.\n\
  -R,   --rfile         A file containing all reservation names.\n\
  -o,   --outfile       A file to write output to.\n\
\n", PROG_NAME);
	exit(EXIT_FAILURE);
}

/**
 * Print the program version number.
 **/
ATT_NORETURN
void
print_version(void)
{
	printf("%s (GNU %s) %s\n"
	       "Copyright (C) %s.\n"
	       "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
	       "This is free software: you are free to change and redistribute it.\n"
	       "There is NO WARRANTY, to the extent permitted by law.\n\n"
	       "Compiled on %s at %s.\n\n",
	       PROG_NAME, PACKAGE, VERSION, PACKAGE_BUGREPORT, __DATE__, __TIME__);

	exit(EXIT_FAILURE);
}

/**
 * \}
 **/
