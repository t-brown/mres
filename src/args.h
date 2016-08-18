/*
 *  Copyright (C) 2016  Timothy Brown
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file args.h
 *
 * Command line argument parsing routines.
 *
 * \ingroup arguments
 * \{
 **/

#ifndef ARGS_H
#define ARGS_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Structure for holding the command line arguments **/
struct args {
	int32_t verbose;
	int32_t offset;
	char *output;
	char *res;
	char *stats_dir;
	char *res_file;
};

/** Parse the command line options **/
int32_t args_parse(int32_t, char **, struct args *);

/** Free an argument structure **/
int32_t args_free(struct args *);

/** Print a short usage statment **/
void print_usage(void);

/** Print the program version number **/
void print_version(void);

/** Jet defaults **/
#define MOAB_STATS_DIR          "/misc/moab/moabhome/stats"
#define RESERVATION_FILE        "/misc/moab/moabhome/etc/jet.reservations.cfg"

/** Program name **/
#if HAVE_GETPROGNAME
#define PROG_NAME               getprogname()
#endif

#if HAVE_PROGRAM_INVOCATION_SHORT_NAME
#define PROG_NAME               program_invocation_short_name
#endif

#ifdef __cplusplus
}                               /* extern "C" */
#endif

#endif                          /* ARGS_H */
/**
 * \}
 **/
