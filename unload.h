/*
 *  Copyright (C) 1996 Cincinnati Bell Information Systems
 *  Use of this code without permission is prohibited.
 *
 *        Filename: unload.h
 *   Creation Date: 07/02/1996
 *          Author: Gabe Saporito
 *
 *     Description: function prototypes and macros used in unl
 *
 *       Functions:
 *
 *         Changes:
 *
 *            Date:   $Date: 07/02/1996 16:19:49 $
 *            Name: $Author: gabes $
 *        Revision:
 *     Description:
 */
#ifndef __UNLOAD_H__
#define __UNLOAD_H__

#include "message.h"

#define SQLNOTFOUND	(1403)

#define orafix(x)	((x).arr[(x).len] = '\0')
#define Value(p,i)	((*(p)->I[(i)]) == -1 ? "" : (p)->V[(i)])

/*
 * Command-line option flags
 */
extern char	*sqlFileName;			/* use cmd-line for SQL when this is NULL */
extern int	unloadMax;				/* no limit when this is <= 0 */
extern int	skipMax;				/* no skip when this is <= 0 */
extern int	recLen;					/* VL records when this is <= 0 */
extern FILE	*outFp;
extern char	fldDelim;
extern char	recDelim;
extern char	padChar;
extern char	*sqlConnect;
extern int	flgTrailingDelim;
extern int	flgIncludeTime;

#ifdef DBUG
extern int	dbugFlag;

void DumpOptions(void);
#endif

/*
 * UNL utility function prototypes
 */
void	ProcOptions(int ,char * []);
char	*BuildStmt(int,char * [],int *);
int		GetSelListSiz(char *stmt);
char	*ArgToStmt(int,char * [],int *);
char	*FileToStmt(char *,int *);
char	*StdinToStmt(int *);
void	FreeStmt(char *);
char	*ltrim(char *);
char	*rtrim(char *);

#endif
