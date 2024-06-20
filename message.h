/*
 *  Copyright (C) 1996 Cincinnati Bell Information Systems
 *  Use of this code without permission is prohibited.
 *
 *        Filename: message.h
 *   Creation Date: 07/02/1996
 *          Author: Gabe Saporito
 *
 *     Description: header file that defines message codes and prototype
 *                  message functions
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
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

/*
 *	message level codes
 */
#define E_FATAL		0
#define E_ERROR		1
#define E_WARNING	2
#define E_INFO		3

/*
 *	UNL message codes
 */
#define UI_NOERROR	(4000)
#define UI_UNLCNT	(UI_NOERROR + 1)
#define UI_EOFMRK	(UI_UNLCNT + 1)
#define UF_INV_L	(UI_EOFMRK + 1)
#define UF_INV_N	(UF_INV_L + 1)
#define UF_INV_K	(UF_INV_N + 1)
#define UF_NOOPEN	(UF_INV_K + 1)
#define UF_NOACCESS	(UF_NOOPEN + 1)
#define UF_INCMPSQL	(UF_NOACCESS + 1)
#define UF_FLEMPTY	(UF_INCMPSQL + 1)
#define UF_BADFILE	(UF_FLEMPTY + 1)
#define UF_MEMORY	(UF_BADFILE + 1)
#define UF_SQLERR	(UF_MEMORY + 1)
#define UI_ERREND	(-1)

/*
 *	message prototypes
 */
void UnlMessage(const int,...);
void SQLMessage(int);

#endif
