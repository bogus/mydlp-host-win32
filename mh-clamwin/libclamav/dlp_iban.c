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
#include "dlp_iban.h"

/* detection mode macros for the contains_* functions */
#define DETECT_MODE_DETECT  0
#define DETECT_MODE_COUNT   1

static char *iban_code[IBAN_COUNT] = {
	"AL", "AD", "AT", "BE", "BA", "BG", "HR", "CY", "CZ", "DK", "FO", "GL", "EE", "FI", "FR", 
	"PF", "TF", "YT", "NC",	"PM", "WF", "DE", "GI", "GR", "HU", "IS", "IE", "IL", "IT", "LV", 
	"LB", "LI", "LT", "LU", "MK", "MT", "MU", "MC", "ME", "NL", "NO", "PL", "PT", "RO", "SM", 
	"SA", "RS", "SK", "SI", "ES", "SE", "CH", "TN", "TR", "GB"
};

static char *iban_regex[IBAN_COUNT] = {
	"AL\\s?\\d{2}\\s?\\d{8}\\s?[A-Za-z0-9]{16}",
	"AD\\s?\\d{2}\\s?\\d{4}\\s?\\d{4}\\s?[A-Za-z0-9]{12}",
	"AT\\s?\\d{2}\\s?\\d{5}\\s?\\d{11}",
	"BE\\s?\\d{2}\\s?\\d{3}\\s?\\d{7}\\s?\\d{2}",
	"BA\\s?\\d{2}\\s?\\d{3}\\s?\\d{3}\\s?\\d{8}\\s?\\d{2}",
	"BG\\s?\\d{2}\\s?[A-Z]{4}\\s?\\d{4}\\s?\\d{2}\\s?[A-Za-z0-9]{8}",
	"HR\\s?\\d{2}\\s?\\d{7}\\s?\\d{10}",
	"CY\\s?\\d{2}\\s?\\d{3}\\s?\\d{5}\\s?[A-Za-z0-9]{16}",
	"CZ\\s?\\d{2}\\s?\\d{4}\\s?\\d{6}\\s?\\d{10}",
	"DK\\s?\\d{2}\\s?\\d{4}\\s?\\d{9}\\s?\\d{1}",
	"FO\\s?\\d{2}\\s?\\d{4}\\s?\\d{9}\\s?\\d{1}",
	"GL\\s?\\d{2}\\s?\\d{4}\\s?\\d{9}\\s?\\d{1}",
	"EE\\s?\\d{2}\\s?\\d{2}\\s?\\d{2}\\s?\\d{11}\\s?\\d{1}",
	"FI\\s?\\d{2}\\s?\\d{6}\\s?\\d{7}\\s?\\d{1}",
	"FR\\s?\\d{2}\\s?\\d{5}\\s?\\d{5}\\s?[A-Za-z0-9]{11}\\s?\\d{2}",
	"PF\\s?\\d{2}\\s?\\d{5}\\s?\\d{5}\\s?[A-Za-z0-9]{11}\\s?\\d{2}",
	"TF\\s?\\d{2}\\s?\\d{5}\\s?\\d{5}\\s?[A-Za-z0-9]{11}\\s?\\d{2}",
	"YT\\s?\\d{2}\\s?\\d{5}\\s?\\d{5}\\s?[A-Za-z0-9]{11}\\s?\\d{2}",
	"NC\\s?\\d{2}\\s?\\d{5}\\s?\\d{5}\\s?[A-Za-z0-9]{11}\\s?\\d{2}",
	"PM\\s?\\d{2}\\s?\\d{5}\\s?\\d{5}\\s?[A-Za-z0-9]{11}\\s?\\d{2}",
	"WF\\s?\\d{2}\\s?\\d{5}\\s?\\d{5}\\s?[A-Za-z0-9]{11}\\s?\\d{2}",
	"DE\\s?\\d{2}\\s?\\d{8}\\s?\\d{10}",
	"GI\\s?\\d{2}\\s?[A-Z]{4}\\s?[A-Za-z0-9]{15}",
	"GR\\s?\\d{2}\\s?\\d{3}\\s?\\d{4}\\s?[A-Za-z0-9]{16}",
	"HU\\s?\\d{2}\\s?\\d{3}\\s?\\d{4}\\s?\\d{1}\\s?\\d{15}\\s?\\d{1}",
	"IS\\s?\\d{2}\\s?\\d{4}\\s?\\d{2}\\s?\\d{6}\\s?\\d{10}",
	"IE\\s?\\d{2}\\s?[A-Z]{4}\\s?\\d{6}\\s?\\d{8}",
	"IL\\s?\\d{2}\\s?\\d{3}\\s?\\d{3}\\s?\\d{13}",
	"IT\\s?\\d{2}\\s?[A-Z]{1}\\s?\\d{5}\\s?\\d{5}\\s?[A-Za-z0-9]{12}",
	"LV\\s?\\d{2}\\s?[A-Z]{4}\\s?[A-Za-z0-9]{13}",
	"LB\\s?\\d{2}\\s?\\d{4}\\s?[A-Za-z0-9]{20}",
	"LI\\s?\\d{2}\\s?\\d{5}\\s?[A-Za-z0-9]{12}",
	"LT\\s?\\d{2}\\s?\\d{5}\\s?\\d{11}",
	"LU\\s?\\d{2}\\s?\\d{3}\\s?[A-Za-z0-9]{13}",
	"MK\\s?\\d{2}\\s?\\d{3}\\s?[A-Za-z0-9]{10}\\s?\\d{2}",
	"MT\\s?\\d{2}\\s?[A-Z]{4}\\s?\\d{5}\\s?[A-Za-z0-9]{18}",
	"MU\\s?\\d{2}\\s?[A-Z]{4}\\s?\\d{2}\\s?\\d{2}\\s?\\d{12}\\s?\\d{3}\\s?[A-Z]{3}",
	"MC\\s?\\d{2}\\s?\\d{5}\\s?\\d{5}\\s?[A-Za-z0-9]{11}\\s?\\d{2}",
	"ME\\s?\\d{2}\\s?\\d{3}\\s?\\d{13}\\s?\\d{2}",
	"NL\\s?\\d{2}\\s?[A-Z]{4}\\s?\\d{10}",
	"NO\\s?\\d{2}\\s?\\d{4}\\s?\\d{6}\\s?\\d{1}",
	"PL\\s?\\d{2}\\s?\\d{8}\\s?\\d{1,16}",
	"PT\\s?\\d{2}\\s?\\d{4}\\s?\\d{4}\\s?\\d{11}\\s?\\d{2}",
	"RO\\s?\\d{2}\\s?[A-Z]{4}\\s?[A-Za-z0-9]{16}",
	"SM\\s?\\d{2}\\s?[A-Z]{1}\\s?\\d{5}\\s?\\d{5}\\s?[A-Za-z0-9]{12}",
	"SA\\s?\\d{2}\\s?\\d{2}\\s?[A-Za-z0-9]{18}",
	"RS\\s?\\d{2}\\s?\\d{3}\\s?\\d{13}\\s?\\d{2}",
	"SK\\s?\\d{2}\\s?\\d{4}\\s?\\d{6}\\s?\\d{10}",
	"SI\\s?\\d{2}\\s?\\d{5}\\s?\\d{8}\\s?\\d{2}",
	"ES\\s?\\d{2}\\s?\\d{4}\\s?\\d{4}\\s?\\d{1}\\s?\\d{1}\\s?\\d{10}",
	"SE\\s?\\d{2}\\s?\\d{3}\\s?\\d{16}\\s?\\d{1}",
	"CH\\s?\\d{2}\\s?\\d{5}\\s?[A-Za-z0-9]{12}",
	"TN59\\s?\\d{2}\\s?\\d{3}\\s?\\d{13}\\s?\\d{2}",
	"TR\\s?\\d{2}\\s?\\d{5}\\s?[A-Za-z0-9]{1}\\s?[A-Za-z0-9]{16}",
	"GB\\s?\\d{2}[A-Z]{4}\\s?\\d{6}\\s?\\d{8}"
};

static int contains_iban(const unsigned char *buffer, int length, int detmode)
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

    end = buffer + length;
    idx = buffer;

	for(i = 0; i < IBAN_COUNT; i++) {
		if((rc = pcre_exec(ibans[i].dlp_re, NULL, buffer, length, 0, 0,ovector, 30)) >= 0)
		{
			found_country = ibans[i].country;
			if(detmode == DETECT_MODE_DETECT)
				return 1;

			count++;
		}
	}
    
    return count;
}

int cl_dlp_iban_init() 
{
	const char *error;
    int erroffset = 0;
	int i = 0, valid_count = 0;
	pcre *temp_re;
	
	for(i = 0; i < IBAN_COUNT; i++) {
		temp_re = pcre_compile(iban_regex[i], 0, &error, &erroffset, NULL);
		if(erroffset == 0) {
			ibans[i].dlp_re = temp_re;
			ibans[i].country = iban_code[i];
		}
		else {
			printf("count : %d, error: %s\n", error);
			return -1;
		}
	}
	
	return CL_SUCCESS;
}

int dlp_get_iban_count(const unsigned char *buffer, int length)
{
    return contains_iban(buffer, length, DETECT_MODE_COUNT);	
}

int dlp_has_iban(const unsigned char *buffer, int length)
{
	return contains_iban(buffer, length, DETECT_MODE_DETECT);
}