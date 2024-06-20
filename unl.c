
/* Result Sets Interface */
#ifndef SQL_CRSR
#  define SQL_CRSR
  struct sql_cursor
  {
    unsigned int curocn;
    void *ptr1;
    void *ptr2;
    unsigned int magic;
  };
  typedef struct sql_cursor sql_cursor;
  typedef struct sql_cursor SQL_CURSOR;
#endif /* SQL_CRSR */

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

/* Object support */
struct sqltvn
{
  unsigned char *tvnvsn; 
  unsigned short tvnvsnl; 
  unsigned char *tvnnm;
  unsigned short tvnnml; 
  unsigned char *tvnsnm;
  unsigned short tvnsnml;
};
typedef struct sqltvn sqltvn;

struct sqladts
{
  unsigned int adtvsn; 
  unsigned short adtmode; 
  unsigned short adtnum;  
  sqltvn adttvn[1];       
};
typedef struct sqladts sqladts;

static struct sqladts sqladt = {
  1,1,0,
};

/* Binding to PL/SQL Records */
struct sqltdss
{
  unsigned int tdsvsn; 
  unsigned short tdsnum; 
  unsigned char *tdsval[1]; 
};
typedef struct sqltdss sqltdss;
static struct sqltdss sqltds =
{
  1,
  0,
};

/* File name & Package Name */
struct sqlcxp
{
  unsigned short fillen;
           char  filnam[7];
};
static struct sqlcxp sqlfpn =
{
    6,
    "unl.pc"
};


static unsigned int sqlctx = 4987;


static struct sqlexd {
   unsigned long  sqlvsn;
   unsigned int   arrsiz;
   unsigned int   iters;
   unsigned int   offset;
   unsigned short selerr;
   unsigned short sqlety;
   unsigned int   occurs;
            short *cud;
   unsigned char  *sqlest;
            char  *stmt;
   sqladts *sqladtp;
   sqltdss *sqltdsp;
   unsigned char  **sqphsv;
   unsigned long  *sqphsl;
            int   *sqphss;
            short **sqpind;
            int   *sqpins;
   unsigned long  *sqparm;
   unsigned long  **sqparc;
   unsigned short  *sqpadto;
   unsigned short  *sqptdso;
   unsigned int   sqlcmax;
   unsigned int   sqlcmin;
   unsigned int   sqlcincr;
   unsigned int   sqlctimeout;
   unsigned int   sqlcnowait;
            int   sqfoff;
   unsigned int   sqcmod;
   unsigned int   sqfmod;
   unsigned char  *sqhstv[4];
   unsigned long  sqhstl[4];
            int   sqhsts[4];
            short *sqindv[4];
            int   sqinds[4];
   unsigned long  sqharm[4];
   unsigned long  *sqharc[4];
   unsigned short  sqadto[4];
   unsigned short  sqtdso[4];
} sqlstm = {12,4};

/* SQLLIB Prototypes */
extern sqlcxt ( void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * );
extern sqlcx2t( void **, unsigned int *,
                   struct sqlexd *, struct sqlcxp * );
extern sqlbuft( void **, char * );
extern sqlgs2t( void **, char * );
extern sqlorat( void **, unsigned int *, void * );

/* Forms Interface */
static int IAPSUCC = 0;
static int IAPFAIL = 1403;
static int IAPFTL  = 535;
extern void sqliem( unsigned char *, signed int * );

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* CUD (Compilation Unit Data) Array */
static short sqlcud0[] =
{12,4130,871,0,0,
5,0,0,0,0,0,27,172,0,0,4,4,0,1,0,1,9,0,0,1,10,0,0,1,10,0,0,1,10,0,0,
36,0,0,2,59,0,1,180,0,0,0,0,0,1,0,
51,0,0,3,48,0,1,184,0,0,0,0,0,1,0,
66,0,0,4,0,0,17,254,0,0,1,1,0,1,0,1,9,0,0,
85,0,0,4,0,0,45,276,0,0,0,0,0,1,0,
100,0,0,4,0,0,20,287,0,0,1,1,0,1,0,3,32,0,0,
119,0,0,4,0,0,14,325,0,0,1,0,0,1,0,2,32,0,0,
138,0,0,4,0,0,15,350,0,0,0,0,0,1,0,
};


/*
 *        Filename: unl.pc
 *   Creation Date: 07/02/1996
 *          Author: Gabe Saporito
 *
 *     Description: The main module and workings for the unl tool, which
 *                  unloads an Oracle database, based on an SQL select stmt.
 *
 *       Functions: int main(int argc,char *argv[]);
 *                  --  main function for unl.  argument checking, etc.
 *                      void ProcOptions(int ,char * []);
 *                  --  command line option processing
 *                  void ConnectToDB(void);
 *                  --  connect to the Oracle database
 *                  int Unl(char *stmt,int listSiz);
 *                  --  SQL driver for unl.  Prepares, sets up and uses the
 *                  --  dynamic SQL, fetches records for printing
 *                  void CoerceSelList(SQLDA *unloadDA);
 *                  --  coerces the dynamic SQL select list information to
 *                  --  make returned data more usable.  Also allocates the
 *                  --  value and indicator variable arrays.
 *                  void FreeSelList(SQLDA *unloadDA);
 *                  --  Frees data allocated by Unl and CoerceSelList
 *                  void UnloadRec(SQLDA *unloadDA,short T[]);
 *                  --  Prints a fetched record
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
#include <stdarg.h>
#include <time.h>
#include "unload.h"

#ifdef CPLUSPLUS_KLUDGE
/*
 * See kludge.c for details on this mess
 */
#define sqlcex		kldg_sqlcex
#define sqlprc		kldg_sqlprc
#define sqlald		kldg_sqlald
#define sqlclu		kldg_sqlclu
#endif

/* EXEC SQL INCLUDE sqlca;
 */ 
/*
 * $Header: sqlca.h 24-apr-2003.12:50:58 mkandarp Exp $ sqlca.h 
 */

/* Copyright (c) 1985, 2003, Oracle Corporation.  All rights reserved.  */
 
/*
NAME
  SQLCA : SQL Communications Area.
FUNCTION
  Contains no code. Oracle fills in the SQLCA with status info
  during the execution of a SQL stmt.
NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************

  If the symbol SQLCA_STORAGE_CLASS is defined, then the SQLCA
  will be defined to have this storage class. For example:
 
    #define SQLCA_STORAGE_CLASS extern
 
  will define the SQLCA as an extern.
 
  If the symbol SQLCA_INIT is defined, then the SQLCA will be
  statically initialized. Although this is not necessary in order
  to use the SQLCA, it is a good pgming practice not to have
  unitialized variables. However, some C compilers/OS's don't
  allow automatic variables to be init'd in this manner. Therefore,
  if you are INCLUDE'ing the SQLCA in a place where it would be
  an automatic AND your C compiler/OS doesn't allow this style
  of initialization, then SQLCA_INIT should be left undefined --
  all others can define SQLCA_INIT if they wish.

  If the symbol SQLCA_NONE is defined, then the SQLCA variable will
  not be defined at all.  The symbol SQLCA_NONE should not be defined
  in source modules that have embedded SQL.  However, source modules
  that have no embedded SQL, but need to manipulate a sqlca struct
  passed in as a parameter, can set the SQLCA_NONE symbol to avoid
  creation of an extraneous sqlca variable.
 
MODIFIED
    lvbcheng   07/31/98 -  long to int
    jbasu      12/12/94 -  Bug 217878: note this is an SOSD file
    losborne   08/11/92 -  No sqlca var if SQLCA_NONE macro set 
  Clare      12/06/84 - Ch SQLCA to not be an extern.
  Clare      10/21/85 - Add initialization.
  Bradbury   01/05/86 - Only initialize when SQLCA_INIT set
  Clare      06/12/86 - Add SQLCA_STORAGE_CLASS option.
*/
 
#ifndef SQLCA
#define SQLCA 1
 
struct   sqlca
         {
         /* ub1 */ char    sqlcaid[8];
         /* b4  */ int     sqlabc;
         /* b4  */ int     sqlcode;
         struct
           {
           /* ub2 */ unsigned short sqlerrml;
           /* ub1 */ char           sqlerrmc[70];
           } sqlerrm;
         /* ub1 */ char    sqlerrp[8];
         /* b4  */ int     sqlerrd[6];
         /* ub1 */ char    sqlwarn[8];
         /* ub1 */ char    sqlext[8];
         };

#ifndef SQLCA_NONE 
#ifdef   SQLCA_STORAGE_CLASS
SQLCA_STORAGE_CLASS struct sqlca sqlca
#else
         struct sqlca sqlca
#endif
 
#ifdef  SQLCA_INIT
         = {
         {'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '},
         sizeof(struct sqlca),
         0,
         { 0, {0}},
         {'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}
         }
#endif
         ;
#endif
 
#endif
 
/* end SQLCA */
/* EXEC SQL INCLUDE sqlda;
 */ 
/*
 * $Header: sqlda.h 08-may-2007.05:58:33 ardesai Exp $ sqlda.h 
 */

/***************************************************************
*      The SQLDA descriptor definition                         *
*--------------------------------------------------------------*
*      VAX/3B Version                                          *
*                                                              *
* Copyright (c) 1987, 2007, Oracle. All rights reserved.  *
***************************************************************/


/* NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************
*/

/*  MODIFIED
    ardesai    05/08/07  - Bug[6037057] Undef Y
    apopat     05/08/02  - [2362423] MVS PE to make lines shorter than 79
    apopat     07/31/99 -  [707588] TAB to blanks for OCCS
    lvbcheng   10/27/98 -  change long to int for sqlda
    lvbcheng   08/15/97 -  Move sqlda protos to sqlcpr.h
    lvbcheng   06/25/97 -  Move sqlda protos to this file
    jbasu      01/29/95 -  correct typo
    jbasu      01/27/95 -  correct comment - ub2->sb2
    jbasu      12/12/94 - Bug 217878: note this is an SOSD file
    Morse      12/01/87 - undef L and S for v6 include files
    Richey     07/13/87 - change int defs to long 
    Clare      09/13/84 - Port: Ch types to match SQLLIB structs
    Clare      10/02/86 - Add ifndef SQLDA
*/

#ifndef SQLDA_
#define SQLDA_ 1
 
#ifdef T
# undef T
#endif
#ifdef F
# undef F
#endif

#ifdef S
# undef S
#endif
#ifdef L
# undef L
#endif

#ifdef Y
 # undef Y
#endif
 
struct SQLDA {
  /* ub4    */ int        N; /* Descriptor size in number of entries        */
  /* text** */ char     **V; /* Ptr to Arr of addresses of main variables   */
  /* ub4*   */ int       *L; /* Ptr to Arr of lengths of buffers            */
  /* sb2*   */ short     *T; /* Ptr to Arr of types of buffers              */
  /* sb2**  */ short    **I; /* Ptr to Arr of addresses of indicator vars   */
  /* sb4    */ int        F; /* Number of variables found by DESCRIBE       */
  /* text** */ char     **S; /* Ptr to Arr of variable name pointers        */
  /* ub2*   */ short     *M; /* Ptr to Arr of max lengths of var. names     */
  /* ub2*   */ short     *C; /* Ptr to Arr of current lengths of var. names */
  /* text** */ char     **X; /* Ptr to Arr of ind. var. name pointers       */
  /* ub2*   */ short     *Y; /* Ptr to Arr of max lengths of ind. var. names*/
  /* ub2*   */ short     *Z; /* Ptr to Arr of cur lengths of ind. var. names*/
  };
 
typedef struct SQLDA SQLDA;
 
#endif

/* ----------------- */
/* defines for sqlda */
/* ----------------- */

#define SQLSQLDAAlloc(arg1, arg2, arg3, arg4) sqlaldt(arg1, arg2, arg3, arg4) 

#define SQLSQLDAFree(arg1, arg2) sqlclut(arg1, arg2) 





/*
 * Function prototypes for PRO*C stuff
 */
extern struct SQLDA *sqlald(int,size_t,size_t);
extern void  sqlclu(struct SQLDA *);
extern void  sqlprc(unsigned long *,int *,int *);
extern void sqlcex(unsigned long *, struct sqlexd *, const struct sqlcxp *);

/*
 * unl.pc function prototypes
 */
void ConnectToDB(void);
int Unl(char *,int);
void CoerceSelList(SQLDA *);
void FreeSelList(SQLDA *);
void UnloadRec(SQLDA *,short []);

/*
 * declared in and used by functions in message.c
 */
extern char *pgmName;

/*
 * Command-line option flags
 */
int		flgTrailingDelim = 0;	/* no trailing delimiter by default */
int		flgIncludeTime = 0;		/* no trailing delimiter by default */
char	*sqlFileName = NULL;	/* use cmd-line for SQL when this is NULL */
int		unloadMax = -1;			/* no limit when this is <= 0 */
int		skipMax = -1;			/* no skip when this is <= 0 */
int		recLen = 0;				/* VL records when this is <= 0 */
FILE	*outFp = NULL;
char	fldDelim = ',';
char	recDelim = '\n';
char	padChar = ' ';
char	*sqlConnect = "/";

#ifdef DBUG
int		dbugFlag = 0;
#endif

main(int argc,char *argv[])
{
	char *stmt;
	int stmtLen;
	int listSiz = 0;
	int numRecs;
	char *s;
	extern int opterr;

	outFp = stdout;

	pgmName = "UNL";
	opterr = 0;
 
/*
 *	process command line arguments
 */
	ProcOptions(argc,argv);

/*
 *	build the SQL statement from stdin, file, command line
 */
	stmt = BuildStmt(argc,argv,&stmtLen);

/*
 *	Connect to database before doing any more processing.
 */
	ConnectToDB();

/*
 *	get a generous estimate for the size of the select list so that Unl can
 *	be sure to allocate enough SQLDA space.
 */
	listSiz = GetSelListSiz(stmt);

/*
 *	Unload the data, based on the stmt and listSiz
 */
	numRecs = Unl(stmt,listSiz);
	if (numRecs < 0)				/* Unl failed, msg already displayed */
		exit(-1);

/*
 *	Free allocated space for stmt
 */
	FreeStmt(stmt);

/*
 *	Print the count message
 */
	UnlMessage(UI_UNLCNT,numRecs);
	exit(0);
}

/*
 * connect to the Oracle database.  If connect fails, print message & exit
 */
void ConnectToDB(void)
{
/* EXEC SQL BEGIN DECLARE SECTION; */ 

	/* VARCHAR connectString[400]; */ 
struct { unsigned short len; unsigned char arr[400]; } connectString;

/* EXEC SQL END DECLARE SECTION; */ 


#ifdef DBUG
	if (dbugFlag)
		fprintf(stderr,"unl: connecting to Oracle...\n");
#endif

	strcpy((char *)connectString.arr,sqlConnect);
	connectString.len = strlen(sqlConnect);
	/* EXEC SQL CONNECT :connectString; */ 

{
 struct sqlexd sqlstm;
 sqlstm.sqlvsn = 12;
 sqlstm.arrsiz = 4;
 sqlstm.sqladtp = &sqladt;
 sqlstm.sqltdsp = &sqltds;
 sqlstm.iters = (unsigned int  )10;
 sqlstm.offset = (unsigned int  )5;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)4352;
 sqlstm.occurs = (unsigned int  )0;
 sqlstm.sqhstv[0] = (unsigned char  *)&connectString;
 sqlstm.sqhstl[0] = (unsigned long )402;
 sqlstm.sqhsts[0] = (         int  )402;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqinds[0] = (         int  )0;
 sqlstm.sqharm[0] = (unsigned long )0;
 sqlstm.sqadto[0] = (unsigned short )0;
 sqlstm.sqtdso[0] = (unsigned short )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqphss = sqlstm.sqhsts;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqpins = sqlstm.sqinds;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlstm.sqpadto = sqlstm.sqadto;
 sqlstm.sqptdso = sqlstm.sqtdso;
 sqlstm.sqlcmax = (unsigned int )100;
 sqlstm.sqlcmin = (unsigned int )2;
 sqlstm.sqlcincr = (unsigned int )1;
 sqlstm.sqlctimeout = (unsigned int )0;
 sqlstm.sqlcnowait = (unsigned int )0;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


	if (sqlca.sqlcode)
	{
		SQLMessage(E_FATAL);
	}

	if (flgIncludeTime)
	{
		/* EXEC SQL ALTER SESSION SET NLS_DATE_FORMAT = 'YYYY-MM-DD HH24:MI:SS'; */ 

{
  struct sqlexd sqlstm;
  sqlstm.sqlvsn = 12;
  sqlstm.arrsiz = 4;
  sqlstm.sqladtp = &sqladt;
  sqlstm.sqltdsp = &sqltds;
  sqlstm.stmt = "alter SESSION SET NLS_DATE_FORMAT = 'YYYY-MM-DD HH24:MI:SS\
'";
  sqlstm.iters = (unsigned int  )1;
  sqlstm.offset = (unsigned int  )36;
  sqlstm.cud = sqlcud0;
  sqlstm.sqlest = (unsigned char  *)&sqlca;
  sqlstm.sqlety = (unsigned short)4352;
  sqlstm.occurs = (unsigned int  )0;
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


	}
	else
	{
		/* EXEC SQL ALTER SESSION SET NLS_DATE_FORMAT = 'YYYY-MM-DD'; */ 

{
  struct sqlexd sqlstm;
  sqlstm.sqlvsn = 12;
  sqlstm.arrsiz = 4;
  sqlstm.sqladtp = &sqladt;
  sqlstm.sqltdsp = &sqltds;
  sqlstm.stmt = "alter SESSION SET NLS_DATE_FORMAT = 'YYYY-MM-DD'";
  sqlstm.iters = (unsigned int  )1;
  sqlstm.offset = (unsigned int  )51;
  sqlstm.cud = sqlcud0;
  sqlstm.sqlest = (unsigned char  *)&sqlca;
  sqlstm.sqlety = (unsigned short)4352;
  sqlstm.occurs = (unsigned int  )0;
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


	}

	if (sqlca.sqlcode)
	{
		SQLMessage(E_FATAL);
	}

#ifdef DBUG
	if (dbugFlag)
		fprintf(stderr,"unl: connected to Oracle\n");
#endif
}

/*
 * unload the data files based on the SQL select in stmt, using a SQLDA struct
 * allocated based on generous estimate in listSiz
 */
int Unl(char *stmt,int listSiz)
{
/* EXEC SQL BEGIN DECLARE SECTION; */ 

	/* VARCHAR sel_stmt[4096]; */ 
struct { unsigned short len; unsigned char arr[4096]; } sel_stmt;

/* EXEC SQL END DECLARE SECTION; */ 


	int i = 0;
	int numFetched = 0;		/* number of recs actually fetched */
	int numUnloaded = 0;	/* number of recs unloaded <= numFetched */
	int skipVal = -1;		/* random # of recs skipped between unl's */
	SQLDA *unloadDA;		/* ptr to the dynamic SQL structure */
	short *holdT;			/* SQL type holder, we coerce the types to STRING */

#ifdef DBUG
	if (dbugFlag)
		fprintf(stderr,"unl: select list contains at most %d items\n",listSiz);
#endif

/*
 *	set up the statement to be prepared
 */
	strcpy((char *)sel_stmt.arr,stmt);
	sel_stmt.len = strlen(stmt);

#ifdef DBUG
	if (dbugFlag)
		fprintf(stderr,"unl: allocating DA...\n");
#endif

/*
 *	allocate the SQLDA space
 */
	unloadDA = sqlald(listSiz*2,listSiz,listSiz);
	if (!unloadDA)
	{
		UnlMessage(UF_MEMORY);
	}

#ifdef DBUG
	if (dbugFlag)
		fprintf(stderr,"unl: EXEC SQL PREPARE...\n");
#endif

/*
 *	Basically, when using dynamic SQL, the order of processing is always the
 *	same:  PREPARE the statemet, DECLARE the cursor, DESCRIBE for the bindlist,
 *	OPEN the cursor, DESCRIBE for the statement list, do any type/length
 *	coersion that is needed, FETCH rows from the cursor, CLOSE the cursor.
 *	For this excersize, we don't have a bindlist, so that step is skipped.  If
 *	any of the SQL fails, we will print the SQL message and exit since we can't
 *	do much without the SQL!
 */
	/* EXEC SQL PREPARE s_sel_stmt FROM :sel_stmt; */ 

{
 struct sqlexd sqlstm;
 sqlstm.sqlvsn = 12;
 sqlstm.arrsiz = 4;
 sqlstm.sqladtp = &sqladt;
 sqlstm.sqltdsp = &sqltds;
 sqlstm.stmt = "";
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )66;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)4352;
 sqlstm.occurs = (unsigned int  )0;
 sqlstm.sqhstv[0] = (unsigned char  *)&sel_stmt;
 sqlstm.sqhstl[0] = (unsigned long )4098;
 sqlstm.sqhsts[0] = (         int  )0;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqinds[0] = (         int  )0;
 sqlstm.sqharm[0] = (unsigned long )0;
 sqlstm.sqadto[0] = (unsigned short )0;
 sqlstm.sqtdso[0] = (unsigned short )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqphss = sqlstm.sqhsts;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqpins = sqlstm.sqinds;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlstm.sqpadto = sqlstm.sqadto;
 sqlstm.sqptdso = sqlstm.sqtdso;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


	if (sqlca.sqlcode)
	{
		SQLMessage(E_FATAL);
	}

#ifdef DBUG
	if (dbugFlag)
		fprintf(stderr,"unl: EXEC SQL DECLARE...\n");
#endif

	/* EXEC SQL DECLARE c_sel_stmt CURSOR FOR s_sel_stmt; */ 

	if (sqlca.sqlcode)
	{
		SQLMessage(E_FATAL);
	}

#ifdef DBUG
	if (dbugFlag)
		fprintf(stderr,"unl: EXEC SQL OPEN...\n");
#endif

	/* EXEC SQL OPEN c_sel_stmt; */ 

{
 struct sqlexd sqlstm;
 sqlstm.sqlvsn = 12;
 sqlstm.arrsiz = 4;
 sqlstm.sqladtp = &sqladt;
 sqlstm.sqltdsp = &sqltds;
 sqlstm.stmt = "";
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )85;
 sqlstm.selerr = (unsigned short)1;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)4352;
 sqlstm.occurs = (unsigned int  )0;
 sqlstm.sqcmod = (unsigned int )0;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


	if (sqlca.sqlcode)
	{
		SQLMessage(E_FATAL);
	}

#ifdef DBUG
	if (dbugFlag)
		fprintf(stderr,"unl: EXEC SQL DESCRIBE...\n");
#endif

	/* EXEC SQL DESCRIBE SELECT LIST FOR s_sel_stmt INTO unloadDA; */ 

{
 struct sqlexd sqlstm;
 sqlstm.sqlvsn = 12;
 sqlstm.arrsiz = 4;
 sqlstm.sqladtp = &sqladt;
 sqlstm.sqltdsp = &sqltds;
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )100;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)4352;
 sqlstm.occurs = (unsigned int  )0;
 sqlstm.sqhstv[0] = (unsigned char  *)unloadDA;
 sqlstm.sqhstl[0] = (unsigned long )0;
 sqlstm.sqhsts[0] = (         int  )0;
 sqlstm.sqindv[0] = (         short *)0;
 sqlstm.sqinds[0] = (         int  )0;
 sqlstm.sqharm[0] = (unsigned long )0;
 sqlstm.sqadto[0] = (unsigned short )0;
 sqlstm.sqtdso[0] = (unsigned short )0;
 sqlstm.sqphsv = sqlstm.sqhstv;
 sqlstm.sqphsl = sqlstm.sqhstl;
 sqlstm.sqphss = sqlstm.sqhsts;
 sqlstm.sqpind = sqlstm.sqindv;
 sqlstm.sqpins = sqlstm.sqinds;
 sqlstm.sqparm = sqlstm.sqharm;
 sqlstm.sqparc = sqlstm.sqharc;
 sqlstm.sqpadto = sqlstm.sqadto;
 sqlstm.sqptdso = sqlstm.sqtdso;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


	if (sqlca.sqlcode)
	{
		SQLMessage(E_FATAL);
	}

#ifdef DBUG
	if (dbugFlag)
		fprintf(stderr,"unl: DESCRIBE found %d items in select list\n",
			unloadDA->F);
#endif

/*
 *	Since we will coerce the data type of all columns to be STRING, we want
 *	to preserve the original type information.  We will need this when we
 *	format the rows for fixed lengtn records.
 */
	holdT = (short *)calloc(unloadDA->F,sizeof(short));
	if (!holdT)
	{
		UnlMessage(UF_MEMORY);
	}
	for (i=0 ; i < unloadDA->F ; i++)
		holdT[i] = unloadDA->T[i];

	CoerceSelList(unloadDA);		/* coerce the select list types and lens */

	if (skipMax > 0)				/* setup for skipping records */
	{
		srand((unsigned int)time(NULL));
		skipVal = (rand() % skipMax) + 1;
	}

	for ( ; ; )
	{
		if (unloadMax > 0 && numUnloaded == unloadMax)
			break;		/* if a threshold is set, stop when we reach it */

		/* EXEC SQL FETCH c_sel_stmt USING DESCRIPTOR unloadDA; */ 

{
  struct sqlexd sqlstm;
  sqlstm.sqlvsn = 12;
  sqlstm.arrsiz = 4;
  sqlstm.sqladtp = &sqladt;
  sqlstm.sqltdsp = &sqltds;
  sqlstm.iters = (unsigned int  )1;
  sqlstm.offset = (unsigned int  )119;
  sqlstm.selerr = (unsigned short)1;
  sqlstm.cud = sqlcud0;
  sqlstm.sqlest = (unsigned char  *)&sqlca;
  sqlstm.sqlety = (unsigned short)4352;
  sqlstm.occurs = (unsigned int  )0;
  sqlstm.sqfoff = (         int )0;
  sqlstm.sqfmod = (unsigned int )2;
  sqlstm.sqhstv[0] = (unsigned char  *)unloadDA;
  sqlstm.sqhstl[0] = (unsigned long )0;
  sqlstm.sqhsts[0] = (         int  )0;
  sqlstm.sqindv[0] = (         short *)0;
  sqlstm.sqinds[0] = (         int  )0;
  sqlstm.sqharm[0] = (unsigned long )0;
  sqlstm.sqadto[0] = (unsigned short )0;
  sqlstm.sqtdso[0] = (unsigned short )0;
  sqlstm.sqphsv = sqlstm.sqhstv;
  sqlstm.sqphsl = sqlstm.sqhstl;
  sqlstm.sqphss = sqlstm.sqhsts;
  sqlstm.sqpind = sqlstm.sqindv;
  sqlstm.sqpins = sqlstm.sqinds;
  sqlstm.sqparm = sqlstm.sqharm;
  sqlstm.sqparc = sqlstm.sqharc;
  sqlstm.sqpadto = sqlstm.sqadto;
  sqlstm.sqptdso = sqlstm.sqtdso;
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


		if (sqlca.sqlcode)
			break;

		numFetched += 1;

		if (skipMax > 0 && (numFetched % skipVal))
			continue;			/* if skip is on, should we skip this rec? */

		UnloadRec(unloadDA,holdT);	/* print the record */
		numUnloaded += 1;

		if (skipMax > 0)		/* if skip is on, set new random skip value */
			skipVal = (rand() % skipMax) + 1;
	}

	free(holdT);				/* free the pre-coerced type array */
	FreeSelList(unloadDA);		/* free the select list values and indicators */
	sqlclu(unloadDA);			/* free the SQLDA for the select list */

	if (sqlca.sqlcode && sqlca.sqlcode != SQLNOTFOUND)
	{
		SQLMessage(E_FATAL);	/* FETCH FAILED */
	}

	/* EXEC SQL CLOSE c_sel_stmt; */ 

{
 struct sqlexd sqlstm;
 sqlstm.sqlvsn = 12;
 sqlstm.arrsiz = 4;
 sqlstm.sqladtp = &sqladt;
 sqlstm.sqltdsp = &sqltds;
 sqlstm.iters = (unsigned int  )1;
 sqlstm.offset = (unsigned int  )138;
 sqlstm.cud = sqlcud0;
 sqlstm.sqlest = (unsigned char  *)&sqlca;
 sqlstm.sqlety = (unsigned short)4352;
 sqlstm.occurs = (unsigned int  )0;
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
}


	if (sqlca.sqlcode)
	{
		SQLMessage(E_FATAL);
	}

	return(numUnloaded);
}

/*
 *	Allocate memory for data and indicator variables and coerce the types and
 *	lengths appropriately.
 */
void CoerceSelList(SQLDA *unlDA)
{
	int i;
	int prec = 0;
	int scal = 0;
	int nullok;

	unlDA->N = unlDA->F;				/* set actual select list size */
	for (i=0 ; i < unlDA->F ; i++)
	{
	/*
	 *	coerce the length of the variables
	 */
		sqlnul(&unlDA->T[i],&unlDA->T[i],&nullok);

		switch (unlDA->T[i])
		{
		case 2:							/* NUMBER(n) */
			sqlprc((unsigned long *)&unlDA->L[i],&prec,&scal);
			if (prec == 0)
				prec = 38;
			if (scal < 0)
				prec += -scal;
			unlDA->L[i] = prec + 2;
			break;
		case 8:							/* LONG */
			unlDA->L[i] = 240;
			break;
		case 11:						/* ROWID */
			unlDA->L[i] = 18;
			break;
		case 12:						/* DATE */
			unlDA->L[i] = flgIncludeTime ? 22 : 14;
			break;
		case 24:						/* LONG RAW */
			unlDA->L[i] = 240;
			break;
		case 1:							/* VARCHAR2 */
		case 23:						/* RAW */
			break;
		}

	/*
	 *	For our purposes, it makes sense to coerce the types of all columns to
	 *	type STRING, because it works well in C, and we only need to be able to
	 *	print the values.  So we set the type to STRING (except for LONG RAWs),
	 *	and increase the length of the data by 1 to hold the NULL character.
	 */
		if (unlDA->T[i] != 24)
		{
			unlDA->T[i] = 5;		/* coerce all types to STRING */
			unlDA->L[i] += 1;		/* make room for NULL */
		}
	/*
	 *	Allocate space for the data variables (V[i]) and indicator variables
	 *	(I[i]).
	 */
		unlDA->V[i] = (char *)malloc(unlDA->L[i]);
		if (!unlDA->V[i])
		{
			UnlMessage(UF_MEMORY);
		}
		unlDA->I[i] = (short *)malloc(sizeof(short));
		if (!unlDA->I[i])
		{
			UnlMessage(UF_MEMORY);
		}
	}
}

/*
 *	Free the space allocated for the data and indicator variables
 */
void FreeSelList(SQLDA *unlDA)
{
	int i;

	for (i=0 ; i < unlDA->F ; i++)
	{
		free(unlDA->V[i]);
		free(unlDA->I[i]);
	}
}

/*
 *	Actually print the unloaded record
 */
void UnloadRec(SQLDA *unlDA,short T[])
{
	int i = 0;
	int rl = 0;		/* current record length, used for fixed length records */
	char *s = NULL;

	for (i=0 ; i < unlDA->F ; i++)		/* loop over each column */
	{
		if (recLen > 0)					/* Fixed length record unload */
		{
			fprintf(outFp,T[i] == 2 ? "%*s" : "%-*s",
				unlDA->L[i]-1,Value(unlDA,i));
			rl += (unlDA->L[i]-1);
		}
		else							/* Variable length, delimited fields */
		{
			if (i)
				putc(fldDelim,outFp);

			fprintf(outFp,"%s",ltrim(rtrim(Value(unlDA,i))));
		}
	}
	if (recLen > 0)						/* pad fixed length records */
	{
		while (rl < recLen)
		{
			putc(padChar,outFp);
			rl++;
		}
	}

	if (recLen <= 0 && flgTrailingDelim)
		putc(fldDelim,outFp);

	putc(recDelim,outFp);
}
