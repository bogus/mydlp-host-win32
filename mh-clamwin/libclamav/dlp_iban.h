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

#ifndef __DLP_IBAN_H_
#define __DLP_IBAN_H_

#include "clamav.h"
#include "others.h"
#include <pcre.h>

#define IBAN_COUNT		55

typedef struct iban_st {
	char *country;
	pcre *dlp_re;
} iban_st;

iban_st ibans[IBAN_COUNT]; 

char *found_country;

/* Searches the supplied buffer for given IBAN and returns
 * the number of IBANs found.
 * Params:
 *      buffer => data buffer to be analyzed.
 *      length => length of buffer.  
 * Returns:
 *      Count of detected regex #'s.
 */
int dlp_get_iban_count(const unsigned char *buffer, int length);

/* Searches the supplied buffer for IBAN #'s.  Bails out as soon as a 
 * given IBAN is detected.
 * Params:
 *      buffer => data buffer to be analyzed.
 *      length => length of buffer.
 * Returns:
 *      found iban country
 */
int dlp_has_iban(const unsigned char *buffer, int length);

#endif  /* __DLP_IBAN_H_ */
