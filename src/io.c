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
 * \file io.c
 * I/O routines.
 *
 * \ingroup io
 * \{
 **/

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sysexits.h>
#include <string.h>
#include <hdf5.h>

#include "config.h"
#include "atts.h"
#include "mem.h"
#include "events.h"
#include "projects.h"
#include "io.h"


/** Local static functions **/
static int io_write_events(hid_t, const struct event *, int64_t);
static int io_write_data(hid_t, const char *, void *, int64_t, hid_t);

/**
 * Open a HDF5 file.
 *
 * @param[in]  filename The filename to open.
 * @param[out] id       The file id.
 *
 * @retval     0         If it was sucessful
 * @retval     1         If there was an error
 **/
int
io_open(const char *filename, hid_t *id)
{
	hid_t estack = 0;
	H5E_auto2_t efunc = {0};
	void *edata;

	/* Get the default error handling functions */
	H5Eget_auto(estack, &efunc, &edata);

	/* Turn off error handling */
	H5Eset_auto(estack, NULL, NULL);

	*id = H5Fcreate(filename, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
	if (*id < 0) {
		H5Eclear(estack);
		if ((*id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT)) < 0){
			H5Eprint(H5E_DEFAULT, stderr);
			return(EXIT_FAILURE);
		}
	}

	/* Turn on error handling */
	H5Eset_auto(estack, efunc, edata);

	return(EXIT_SUCCESS);
}

/**
 * Close a HDF5 file.
 *
 * @param[in] id         The file id.
 *
 * @retval     0         If it was sucessful
 * @retval     1         If there was an error
 **/
int
io_close(hid_t id)
{

	H5Fclose(id);
	return(EXIT_SUCCESS);
}

/**
 * Write a project to a file.
 *
 * @param[in] id         The file id.
 * @param[in] pjt        The project to write.
 *
 * @retval     0         If it was sucessful
 * @retval     1         If there was an error
 **/
int
io_write(hid_t id, const struct project *p)
{
	hid_t   gid = 0;                  /* Group ID */
	hid_t   eid = 0;                  /* Epoch ID */
	hid_t   did = 0;                  /* Data ID */
	hid_t   rid = 0;                  /* Reservation group ID */
	hid_t   jid = 0;                  /* Job group ID */
	herr_t  ierr = 0;                 /* Error status */
	hsize_t dims = 0;                 /* Tmp dimenstions */


	if ((p->nr == 0) && (p->nj == 0)) {
		return(EXIT_SUCCESS);
	}

	/* Create a group for the reservation project */
	gid = H5Gcreate(id, p->name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	/* Write the project epochs as an attribute */
	dims = p->nepochs;
	did = H5Screate_simple(1, &dims, NULL);
	eid = H5Acreate(gid, "Epochs", H5T_STD_I32LE, did,
			H5P_DEFAULT, H5P_DEFAULT);
	ierr = H5Awrite(eid, H5T_NATIVE_INT, &((p->epochs)[0]));
	ierr = H5Aclose(eid);

	/* Create a group for reservations */
	if (p->nr != 0) {
		rid = H5Gcreate(gid, "reservations", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		io_write_events(rid, p->reservations, p->nr);
		ierr = H5Gclose(rid);
	}

	if (p->nj != 0) {
		/* Create a group for jobs */
		jid = H5Gcreate(gid, "jobs", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		io_write_events(jid, p->jobs, p->nj);
		ierr = H5Gclose(jid);
	}

	ierr = H5Gclose(gid);

	return(EXIT_SUCCESS);
}

/**
 * Write an event linked-list.
 *
 * Note, this changes a linked-list of structs to arrays.
 *
 * @param[in]  id        The id of the group to write under.
 * @param[in]  e         The event linked list to write.
 * @param[in]  n         The number of events in the linked list.
 *
 * @retval     0         If it was sucessful
 * @retval     1         If there was an error
 **/
static
int
io_write_events(hid_t id, const struct event *e, int64_t n)
{
	int64_t i = 0;
	struct event *eptr = NULL;
	int32_t *edata  = NULL;
	int64_t *ids    = NULL;
	int64_t *nodes  = NULL;
	int64_t *starts = NULL;
	int64_t *ends   = NULL;

	edata  = xmemalign(n*sizeof(int32_t));
	ids    = xmemalign(n*sizeof(int64_t));
	nodes  = xmemalign(n*sizeof(int64_t));
	starts = xmemalign(n*sizeof(int64_t));
	ends   = xmemalign(n*sizeof(int64_t));

	eptr = (struct event *)e;
	i = n-1;
	while (eptr != NULL) {
		edata[i]  = eptr->epoch;
		ids[i]    = eptr->id;
		nodes[i]  = eptr->nodes;
		starts[i] = eptr->start;
		ends[i]   = eptr->end;
		eptr = eptr->next;
		--i;
	}

	/* write the data */
	io_write_data(id, "epochs", edata,  n, H5T_NATIVE_INT32);
	io_write_data(id, "ids",    ids,    n, H5T_NATIVE_INT64);
	io_write_data(id, "nodes",  nodes,  n, H5T_NATIVE_INT64);
	io_write_data(id, "starts", starts, n, H5T_NATIVE_INT64);
	io_write_data(id, "ends",   ends,   n, H5T_NATIVE_INT64);

	free(edata);
	free(ids);
	free(nodes);
	free(starts);
	free(ends);

	return(EXIT_SUCCESS);
}


/**
 * Write a 1D data array to the HDF5 file.
 *
 * @param[in]  id        The id to write the data under.
 * @param[in]  name      The name of the dataset.
 * @param[in]  data      The data to write.
 * @param[in]  n         The number of elements in the data array.
 * @param[in]  type      The data type.
 *
 * @retval     0         If it was sucessful
 * @retval     1         If there was an error
 **/
static
int
io_write_data(hid_t id, const char *name, void * restrict data, int64_t n, hid_t type)
{
	hid_t dspace_id = 0;
	hid_t dtype_id  = 0;
	hid_t dset_id   = 0;
	hsize_t dims    = 0;
	int32_t bsize   = 0;

	dims = n;
	/* Create the dataspace */
	dspace_id = H5Screate_simple(1, &dims, NULL);

	/* Set the data type */
	dtype_id = H5Tcopy(type);

	/* Create the data set */
	dset_id = H5Dcreate(id, name, dtype_id, dspace_id,
			    H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	/* Write the data */
	H5Dwrite(dset_id, type, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

	H5Dclose(dset_id);
	H5Tclose(dtype_id);
	H5Sclose(dspace_id);

	return(EXIT_SUCCESS);
}

/**
 * \}
 **/
