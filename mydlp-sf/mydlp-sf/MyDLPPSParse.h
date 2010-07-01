/* 
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

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define  Putc(x)  putchar(x);       
#define  TRUE     1
#define  FALSE    0

void psparse(char *source, char *destination)     							
{
FILE *source, *destination;
char *str;
char junk[80];
int ch, para=0, last=0;
while ((ch=fgetc(source)) != EOF)
  {
    switch (ch)
      {
      case '%'  : if (para==0) fgets(junk, 80, source);
      else putchar(ch);
      case '\n' : if (last==1) { puts(""); last=0; } break;
      case '('  : if (para++>0) putchar(ch); break;
      case ')'  : if (para-->1) putchar(ch);
      else putchar(' ');
	last=1; break;

      case '\\' : if (para>0)
	switch(ch=fgetc(source))
	  {
	  case '(' :
	  case ')' :  putchar(ch); break;
	  case 't' :  putchar('\t'); break;
	  case 'n' :  putchar('\n'); break;
	  case '\\':  putchar('\\'); break;
	  case '0' :  case '1' : case '2' : case '3' :
	  case '4' :  case '5' : case '6' : case '7' :
	    putchar('\\');
	  default:  putchar(ch); break;
	  }
	break;
      default:	if (para>0) putchar(ch);
      }
  }
}