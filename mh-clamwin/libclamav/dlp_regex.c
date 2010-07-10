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
#include <pcre.h>

#include "clamav.h"
#include "others.h"
#include "dlp_regex.h"

/* detection mode macros for the contains_* functions */
#define DETECT_MODE_DETECT  0
#define DETECT_MODE_COUNT   1

static int contains_regex(const unsigned char *buffer, int length, int detmode)
{    
    const unsigned char *idx;
    const unsigned char *end;
    int count = 0, i=0;   
	int rc = -1;
    int ovector[30];
    
    if(buffer == NULL)
    {
        return 0;         
    }
	
	if(regexes == NULL)
	{
		return 0;
	}

    end = buffer + length;
    idx = buffer;

	for(i = 0; i < regex_count; i++) {
		if((rc = pcre_exec(regexes[i].dlp_re, NULL, buffer, length, 0, 0,ovector, 30)) >= 0)
		{
			found_regex_id = regexes[i].id;
			if(detmode == DETECT_MODE_DETECT)	
				return 1;
		
			count++;
		}
	}
    
    return count;
}

int cl_dlp_regex_init(unsigned int ids[], const unsigned char **regex_list, int length) 
{
	const char *error;
    int erroffset = 0;
	int i = 0, valid_count = 0;
	pcre *temp_re;
	pcre **tmpPcres = (pcre **)malloc(sizeof(int) * length);
	int *tmpIds = (int *)malloc(sizeof(int) * length);

	if(length == 0) {
		regexes = NULL;
		regex_count = 0;
		return 0;
	}

	for(i = 0; i < length; i++) {
		temp_re = pcre_compile(regex_list[i], PCRE_CASELESS, &error, &erroffset, NULL);
		if(erroffset == 0) {
			tmpPcres[valid_count] = temp_re;
			tmpIds[valid_count] = ids[i];
			valid_count++;
		}
		else {
			//printf("%d--%s \n", erroffset, error);
		}
	}
	
	regexes = (struct regex_st *)malloc(sizeof(struct regex_st) * valid_count);
	for(i = 0; i < valid_count; i++) {
		regexes[i].dlp_re = tmpPcres[i];
		regexes[i].id = tmpIds[i];
	}

	regex_count = valid_count;

	return 0;
}

int cl_dlp_regex_init2(struct regex_st *valid_regexes, int length) 
{
	regexes = valid_regexes;
	regex_count = length;

	return 0;
}

int dlp_get_regex_count(const unsigned char *buffer, int length)
{
    return contains_regex(buffer, length, DETECT_MODE_COUNT);
}

int dlp_has_regex(const unsigned char *buffer, int length)
{
    return contains_regex(buffer, length, DETECT_MODE_DETECT);
}
