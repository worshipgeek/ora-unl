#       Filename: Makefile
#  Creation Date: 07/02/1996
#         Author: Gabe Saporito
#
#    Description: Makefile for the unl tool
#
#      Functions: unl, clean, clobber
#
#        Changes:
#
#           Date:   $Date: 07/02/1996 16:36:36 $
#           Name: $Author: gabes $
#       Revision:
#    Description:

#	Override the standard make rules to make PRO*C and C

include $(ORACLE_HOME)/precomp/lib/env_precomp.mk

PRECOMPHOME		= $(ORACLE_HOME)/precomp/
RDBMSHOME		= $(ORACLE_HOME)/rdbms/
LIBHOME			= $(ORACLE_HOME)/lib/
NETWORKHOME		= $(ORACLE_HOME)/network/
PLSQLHOME		= $(ORACLE_HOME)/plsql/
PRECOMPUBLIC	= $(ORACLE_HOME)/precomp/public
DBMSPUBLIC		= $(ORACLE_HOME)/rdbms/public

CC				= gcc
CLIBS			= $(TTLIBSQA) $(LDLIBS)
CFLAGS			=
I_SYM			= -I
INCLUDE			= $(I_SYM) . $(I_SYM) $(PRECOMPUBLIC) $(I_SYM) $(DBMSPUBLIC) $(APPL_INCLUDE)

PROCPLSFLAGS	= sqlcheck=semantics dbms=v8 auto_connect=yes
INCLUDES		= sys_include=$(ORACLE_HOME)/precomp/public  sys_include=/usr/include sys_include=/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include/ sys_include=/usr/include sys_include=/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include sys_include=/usr/include include=$(ORACLE_HOME)/precomp/public include=$(ORACLE_HOME)/rdbms/demo include=$(ORACLE_HOME)/network/public include=$(ORACLE_HOME)/plsql/public include=$(ORACLE_HOME)/lib ltype=short
OCFLAGS			= $(PROCPLSFLAGS) $(INCLUDES)
PROCPPFLAGS     = code=cpp cpp_suffix=C
PROLDLIBS       = $(LLIBCLNTSH) $(STATICPROLDLIBS)
STATICCPPLDLIBS = $(SCOREPT) $(SSCOREED) $(DEF_ON) $(LLIBCLIENT) $(LLIBSQL) $(DEVTTLIBS)
CPPLDLIBS       = $(LLIBCLNTSH) $(STATICCPPLDLIBS)
ORA_CLIENT_LIB  = shared
LIBSQL          = $(LIBHOME)/libsql8.a
SQLLIBS         = -lsql8
LDFLAGS         = -L$(LIBHOME)
PROC            = $(ORACLE_HOME)/bin/proc

.SUFFIXES:
.SUFFIXES:	.o .a .ec .pc .c .h
.c.o:
			$(CC) $(CFLAGS) -c $*.c

.pc.o:
			$(PROC) $(PROCFLAGS) iname=$*.pc oname=$*.c
			$(CC) $(CFLAGS) $(INCLUDE) -c $*.c
# && rm -f $*.c

CFLAGS	=	-I $(ORACLE_HOME)/precomp/public

#	Library information

# ORALIB  =	 $(ORACLE_HOME)/lib
# ORAOBJ	=    $(ORALIB)/libclntsh.sl
# ORALIBS	=    `cat $(ORACLE_HOME)/rdbms/lib/sysliblist`

# OTHLIBS =	-ldl -lsocket -lnsl -lm -lc -lmp -lfml -lfml32 -lgp \
# 			-lcrypt -lelf -lnsl

#	The UNL utility parts

APPL_LIBHOME	=
APPL_LIBS		=
OBJS			= unl.o message.o utils.o

unl:		$(OBJS)
			$(CC) -o unl $(OBJS) -L$(LIBHOME) $(APPL_LIBHOME) $(APPL_LIBS) $(PROLDLIBS)

unl.o:		unload.h message.h
message.o:	message.h
utils.o:	unload.h message.h

#	Provide cleanup

clean:
			rm -f *.o

clobber:	clean
			rm -f unl
