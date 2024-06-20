/*
 *        Filename: message.c
 *   Creation Date: 07/02/1996
 *          Author: Gabe Saporito
 *
 *     Description: User message display handlers for unl.
 *
 *       Functions: void SQLMessage(int level)
 *                  --  SQL Error message printer.  int level is ignored.
 *                  void UnlMessage(const int er_code,...)
 *                  --  Message printer, accepts an error code (as defined in
 *                  --  message.h, and formats and prints it ala printf.
 *
 *         Changes:
 *
 *            Date:   $Date: 07/02/1996 16:19:49 $
 *            Name: $Author: gabes $
 *        Revision:
 *     Description:
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sqlca.h>
#include "message.h"

char *pgmName = "PGM";

/*
 *	Print out the Oracle SQL error message using UnlMessage.  Since UF_SQLERR
 *	is a E_FATAL, level is actually ignored.
 */
void SQLMessage(int level)
{
	char *s;

	sqlca.sqlerrm.sqlerrmc[sqlca.sqlerrm.sqlerrml] = '\0';	/* make C string */
	if (s=strchr(sqlca.sqlerrm.sqlerrmc,'\n'))				/* lose newline */
		*s = '\0';
	UnlMessage(UF_SQLERR,sqlca.sqlerrm.sqlerrmc);			/* dump message */
}

static struct
{
	int  er_code;
	int  er_levl;
	char *er_fmt;
} errorTab[] =							/* error message information */
{
	{ UI_NOERROR,E_INFO,"this is not an error" },
	{ UI_UNLCNT,E_INFO,"%d records unloaded" },
	{ UI_EOFMRK,E_INFO,"end input with a '.' or '/' in the 1st position." },
	{ UF_INV_L,E_FATAL,"invalid record length specified - %s" },
	{ UF_NOOPEN,E_FATAL,"cannot open file '%s'" },
	{ UF_INV_N,E_FATAL,"invalid record max specified - %s" },
	{ UF_INV_K,E_FATAL,"invalid skip max specified - %s" },
	{ UF_NOACCESS,E_FATAL,"cannot access file '%s'" },
	{ UF_INCMPSQL,E_FATAL,"incomplete SQL statement specified" },
	{ UF_FLEMPTY,E_FATAL,"empty SQL file '%s'" },
	{ UF_BADFILE,E_FATAL,"error reading SQL file '%s'" },
	{ UF_MEMORY,E_FATAL,"memory allocation error" },
	{ UF_SQLERR,E_FATAL,"SQL error - %s" },
	{ UI_ERREND,E_INFO,NULL },
};

static char *errLevel[] =			/* string representations of levels codes */
{
	"F",
	"E",
	"W",
	"I"
};

/*
 *	Display a UNL message on the stderr, based on the error code er_code.
 *	The message can take arguments (ala printf formats), and these are passed
 *	in by the user.
 */
void UnlMessage(const int er_code,...)
{
	va_list	ap; //  = NULL;			/* variable list of arguments */
	int i = 0;
	int f = 0;
	int er_levl  = E_FATAL;
	char *er_fmt = "unknown error code specified";

/*
 *	Find the error message for the passed in er_code
 */
	for (i=0 ; i < errorTab[i].er_code != UI_ERREND ; i++)
	{
		if (errorTab[i].er_code == er_code)
		{
			f++;
			break;
		}
	}
	if (f)					/* set the error level and format from the table */
	{
		er_levl = errorTab[i].er_levl;
		er_fmt = errorTab[i].er_fmt;
	}

/*
 *	Print the message to stderr
 */
	fprintf(stderr,"%s-%s%04.4d: ",pgmName,errLevel[er_levl],er_code);

	va_start(ap,er_code);
	vfprintf(stderr,(const char *)er_fmt,ap);
	va_end(ap);

	fprintf(stderr,"\n");

	if (er_levl == E_FATAL)
		exit(-1);
}
