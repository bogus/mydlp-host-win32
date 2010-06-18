/* 
 *  Simple library to detect given regexes.
 *
 *  Copyright (C) 2010 Medra Teknoloji
 *
 *  Authors: Burak OGUZ <burak@medra.com.tr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */

#if HAVE_CONFIG_H
#include "clamav-config.h"
#endif


#include <stdio.h>
#include <ctype.h>  
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "clamav.h"
#include "others.h"
#include "dlp_md5db.h"

char *cl_dlp_md5db_init(const unsigned char *md5_list) 
{
	FILE *file;
	char *filename;
#ifdef _WIN32
	filename = _tempnam("c:\\tmp", "myd");
#else
	filename = _tempnam("/tmp/", "myd");
#endif
	tmp_filename = malloc(strlen(filename) + 4);
	strcpy(tmp_filename, filename);
	strcat(tmp_filename, ".hdb");
	if((file = fopen(tmp_filename, "w")) == NULL) {
		cli_dbgmsg("Can't open temp file for MD5 DB\n");
		return NULL;
	}
	fprintf(file, "%s", md5_list);
	fclose(file);
	free(filename);

	return tmp_filename;
}

int cl_dlp_md5db_unlink() 
{
	if(tmp_filename == NULL)
		return 1;
	else
	{
		unlink(tmp_filename);
		tmp_filename = NULL;
	}
	return 0;
}
