/*
 *        Filename: utils.c
 *   Creation Date: 07/02/1996
 *          Author: Gabe Saporito
 *
 *     Description: utility functions for unl tool
 *
 *       Functions: void ProcOptions(int argc,char *argv[])
 *                  --  process command line options
 *                  void DumpOptions(void)
 *                  --  dump the command line option settings
 *       Functions: char *BuildStmt(int argc,char *argv[],int *stmtlen)
 *                  --  build the stmt from cmd-line,stdin or file
 *                  int GetSelListSiz(char *stmt)
 *                  --  provide a generous estimate of the select list size
 *                  char *StdinToStmt(int *len)
 *                  --  allocate space, read SQL stmt from stdin, return ptr
 *                  --  to space and length.
 *                  char *FileToStmt(char *fileName,int *len)
 *                  --  allocate space, read SQL stmt from file, return ptr
 *                  --  to space and length
 *                  char *ArgToStmt(int argc,char *argv[],int *len)
 *                  --  allocate space, build SQL stmt from command line args,
 *                  --  return ptr to space and length
 *                  void FreeStmt(char *stmt)
 *                  --  free storage allocated for stmt by the funcs above
 *                  char *ltrim(char *s)
 *                  --  trim blanks from left side of string
 *                  char *rtrim(char *s)
 *                  --  trim blanks from right side of string
 *
 *         Changes:
 *
 *            Date:   $Date: 07/02/1996 16:19:49 $
 *            Name: $Author: gabes $
 *        Revision:
 *     Description:
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "unload.h"

/*
 *	Standard command line options processor.
 */
#define USAGE	\
"usage: unl [-t] [-T] [-f delim] [-r delim] [-l rl [-p pad]] [-o file] \n"\
"           [-n num_recs] [-k maxskip] [-s sqlfile] [-c connect_string] \n"\
"           [select_sql]\n"
#define HELP	"\n"\
"       -t                 include a trailing field delimiter\n"\
"       -T                 include time in date field data\n"\
"       -f delim           set field delimiter to delim\n"\
"       -r delim           set record delimiter to delim\n"\
"       -l rl              output fix-length records of rl length\n"\
"       -p pad             set pad character to pad\n"\
"       -o file            send output to file named file\n"\
"       -n num_recs        output at most num_recs records\n"\
"       -k maxskip         skip at most maxskip records between fetches\n"\
"       -s sqlfile         read select statement from file sqlfile (-=stdin)\n"\
"       -c connect_string  connect to Oracle using connect_string\n"

#define usage()	exit((fprintf(stderr,USAGE),fprintf(stderr,HELP),-1))

#ifdef DBUG
#define OPTIONS "f:r:l:p:o:n:k:s:c:tTD"
#else
#define OPTIONS "f:r:l:p:o:n:k:s:c:tT"
#endif

extern char *optarg;
extern int optind;

void ProcOptions(int argc,char *argv[])
{
	int c;

	while ((c=getopt(argc,argv,OPTIONS)) != EOF)
	{
		switch (c)
		{
		case 't':
			flgTrailingDelim = 1;
			break;
		case 'T':
			flgIncludeTime = 1;
			break;
		case 'f':					/* set field delimiter */
			fldDelim = *optarg;
			break;
		case 'r':					/* set record delimiter */
			recDelim = *optarg;
			break;
		case 'l':					/* set to fixed length, specify reclen */
			recLen = atoi(optarg);
			if (recLen <= 0)
			{
				UnlMessage(UF_INV_L,optarg);
			}
			break;
		case 'p':					/* set pad character for fixed len recs */
			padChar = *optarg;
			break;
		case 'o':					/* redirect output file */
			outFp = fopen(optarg,"w");
			if (!outFp)
			{
				UnlMessage(UF_NOOPEN,optarg);
			}
			break;
		case 'n':					/* set maximum number of recs unloaded */
			unloadMax = atoi(optarg);
			if (unloadMax <= 0)
			{
				UnlMessage(UF_INV_N,optarg);
			}
			break;
		case 'k':					/* set a maximum skip val, turn on skip */
			skipMax = atoi(optarg);
			if (skipMax <= 0)
			{
				UnlMessage(UF_INV_K,optarg);
			}
			break;
		case 's':					/* specify file to take SQL from -= stdin */
			if (strcmp(optarg,"-") != 0 && access(optarg,0) != 0)
			{
				UnlMessage(UF_NOACCESS,optarg);
			}
			sqlFileName = optarg;
			break;
		case 'c':					/* specify an Oracle connect string */
			sqlConnect = optarg;
			break;
#ifdef DBUG
		case 'D':					/* turn on debug */
			dbugFlag++;
			break;
#endif
		default:					/* OOPS!!! */
			usage();
			break;
		}
	}

#ifdef DBUG
	if (dbugFlag)
		DumpOptions();				/* dump cmd line options in debug mode */
#endif

}

/*
 * Function to display command line options.
 */
#ifdef DBUG
void DumpOptions(void)
{
	fprintf(stderr,"unl: option settings:\n");
	fprintf(stderr,"    sqlFileName: [%s]\n",sqlFileName?sqlFileName:"NULL");
	fprintf(stderr,"      unloadMax: [%d]\n",unloadMax);
	fprintf(stderr,"        skipMax: [%d]\n",skipMax);
	fprintf(stderr,"          outFp: [%d]\n",outFp->_file);
	fprintf(stderr,"       fldDelim: [\\%03.3o]\n",fldDelim);
	fprintf(stderr,"       recDelim: [\\%03.3o]\n",recDelim);
	fprintf(stderr,"        padChar: [\\%03.3o]\n",padChar);
	fprintf(stderr,"         recLen: [%d]\n",recLen);
	fprintf(stderr,"     sqlConnect: [%s]\n",sqlConnect);
}
#endif

/*
 *	Determine which stmt builder to use and call it.
 */
char *BuildStmt(int argc,char *argv[],int *stmtLen)
{
	char *stmt = NULL;
	char *s;

	if (sqlFileName)						/* read from file or stdin */
	{
		if (strcmp(sqlFileName,"-") == 0)	/* '-' = stdin */
			stmt = StdinToStmt(stmtLen);
		else								/* read from file */
			stmt = FileToStmt(sqlFileName,stmtLen);
	}
	else									/* process command line */
		stmt = ArgToStmt(argc,argv,stmtLen);

	if (!stmt)
	{
		UnlMessage(UF_INCMPSQL);
	}

	rtrim(stmt);
	if (s=strchr(stmt,';'))
		*s = '\0';
	rtrim(stmt);

 	if (!*stmt)
	{
		UnlMessage(UF_INCMPSQL);
	}

#ifdef DBUG
	if (dbugFlag)
	{
		fprintf(stderr,"unl: sel stmt (from %s)\n",
			sqlFileName?"file":"command line");
		fprintf(stderr,"[%s]\n",stmt);
	}
#endif

	return(stmt);
}

/*
 *	Read stmt from stdin
 */
char *StdinToStmt(int *len)
{
	static char buf[BUFSIZ+1];
	char *s;
	int l;
	int stmt_siz = 0;
	int stmt_len = 0;
	char *stmt = NULL;

	if (isatty(fileno(stdin)))			/* print a prompt, if this is a tty */
	{
		UnlMessage(UI_EOFMRK);
	}

	while (fgets(buf,BUFSIZ,stdin))
	{
		if (*buf == '.' || *buf == '/')	/* stop if line starts with . or / */
			break;

		if (s=strchr(buf,'\n'))			/* trim newline */
			*s = '\0';
	/*
	 *	check the allocated buffer and make sure this new part of the string
	 *	will fit.  If it won't reallocate more space
	 */
		l = strlen(buf);
		while ((stmt_len+l+1) > stmt_siz)
		{
			stmt_siz += 512;
			stmt = (char *)realloc(stmt,stmt_siz);
			if (!stmt)
			{
				UnlMessage(UF_MEMORY);
			}
		}
	/*
	 *	add new part to the select statement
	 */
		strcpy(&stmt[stmt_len],buf);
		stmt_len += l;
		strcpy(&stmt[stmt_len]," ");
		stmt_len += 1;
	}
	if (stmt_len)					/* trim trailing blank */
	{
		stmt_len -= 1;
		stmt[stmt_len] = '\0';
	}
	*len = stmt_len;
	return(stmt);
}

/*
 * read the SQL stmt from a file
 */
char *FileToStmt(char *fileName,int *len)
{
	char *s;
	char *stmt = NULL;
	int stmt_len = 0;
	int fd = 0;

	*len = 0;
	fd = open(fileName,0);					/* open the SQL file */

	if (fd < 0)								/* open failed.  exit */
	{
		UnlMessage(UF_NOOPEN,fileName);
	}
	stmt_len = (int)lseek(fd,0L,SEEK_END);	/* set stmt length to file size */
	if (stmt_len <= 0)						/* error getting size.	 exit */
	{
		close(fd);
		UnlMessage(UF_FLEMPTY,fileName);
	}
	lseek(fd,0L,SEEK_SET);					/* rewind file */

	stmt = (char *)malloc(stmt_len+1);		/* allocate memory for stmt */
	if (!stmt)
	{
		UnlMessage(UF_MEMORY);
	}

	if (read(fd,stmt,stmt_len) < stmt_len)	/* read the SQL statement in */
	{
		close(fd);
		UnlMessage(UF_BADFILE,fileName);
	}
	close(fd);

	stmt[stmt_len] = '\0';					/* add terminating NULL */
	*len = stmt_len;
	for (s=stmt ; *s ; s++)					/* eliminate newlines from stmt */
	{
		if (*s == '\n')
			*s = ' ';
	}
	return(stmt);
}

/*
 * Build the SQL statement from command line arguments
 */
char *ArgToStmt(int argc,char *argv[],int *len)
{
	int i;
	int l;
	int stmt_siz = 0;
	int stmt_len = 0;
	char *stmt = NULL;

	*len = 0;
	for (i=optind ; i < argc ; i++)				/* loop over arguments */
	{
	/*
	 *	check the allocated buffer and make sure this new part of the string
	 *	will fit.  If it won't reallocate more space
	 */
		l = strlen(argv[i]);
		while ((stmt_len+l+1) > stmt_siz)
		{
			stmt_siz += 512;
			stmt = (char *)realloc(stmt,stmt_siz);
			if (!stmt)
			{
				UnlMessage(UF_MEMORY);
			}
		}
		strcpy(&stmt[stmt_len],argv[i]);
		stmt_len += l;
		strcpy(&stmt[stmt_len]," ");
		stmt_len += 1;
	}
	if (stmt_len)
	{
		stmt_len -= 1;
		stmt[stmt_len] = '\0';
	}
	*len = stmt_len;
	return(stmt);
}

/*
 *	Free the allocated statement stmt
 */
void FreeStmt(char *stmt)
{
	free((void *)stmt);
}

/*
 *	Provide a generous estimate of the select list size by counting commas
 *	in the select statement and adding one.  If there are no commas in the
 *	statement, then we must assume that we have a "select *" type statement,
 *	so set the size to a ludicrous size.  I don't think we have to worry
 *	about tables, or even a combination of tables resulting in a select list
 *	larger than 256 columns.
 */
int GetSelListSiz(char *stmt)
{
	char *s;
	int listSiz = 0;

	for (s=stmt ; *s ; s++)
	{
		if (*s == ',')
			listSiz++;
	}
	if (listSiz == 0)
		listSiz = 256;
	else
		listSiz++;

	return(listSiz);
}

/*
 *	trim spaces from the left side of a string
 */
char *ltrim(char *s)
{
	while (isspace(*s))
		s++;
	return(s);
}

/*
 *	trim spaces from the right side of a string
 */
char *rtrim(char *s)
{
	char *p;

	if (*s == '\0') return s;

	for (p=(&s[strlen(s)-1]) ; isspace(*p) && p >= s ; p--)
		;
	*++p = '\0';
	return(s);
}
