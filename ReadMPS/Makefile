###############################################################################
#	FILE		:	Makefile			              
#	DESCRIPTION	:	Project makefile	              
#	AUTHOR		:	M H Khaliq	                              
#	LICENSE		:	MIT	                             
###############################################################################

###############################################################################
#	MAKEFILE DEFINITIONS
###############################################################################
CC		=	g++
CFLAGS		=	-D__C__PLUS__PLUS -DUNIX -Wall \
			-Wpointer-arith -Wcast-qual -Wcast-align \
			-Wconversion -Winline \
			-O3 -c
LFLAGS		=	-lm
OBASEFILES	=	fileio.o hash.o readmps.o sort.o 
OSCMFILES	=	ScmMain.o ScmLib.o
#
###############################################################################
#	EXECUTABLE-SPECIFIC
###############################################################################
all:		ScmMain

ScmMain:	$(OBASEFILES) $(OSCMFILES)
		$(CC) -o ScmMain $(OBASEFILES) $(OSCMFILES) $(LFLAGS)

###############################################################################
#	SCM-SPECIFIC
###############################################################################
ScmMain.o:	ScmMain.h ScmMain.cpp
		$(CC) $(CFLAGS) ScmMain.cpp

ScmLib.o:	ScmLib.h ScmLib.cpp
		$(CC) $(CFLAGS) ScmLib.cpp

###############################################################################
#	READMPS MODULE
###############################################################################
fileio.o:	fileio.h fileio.C
		$(CC) $(CFLAGS) fileio.C

hash.o:		hash.h hash.C
		$(CC) $(CFLAGS) hash.C

readmps.o:	readmps.h readmps.C
		$(CC) $(CFLAGS) readmps.C

sort.o:		sort.h sort.C
		$(CC) $(CFLAGS) sort.C

###############################################################################
#	DEBUG
###############################################################################
debug:		$(OBASEFILES)
		$(CC) -D__DEBUG__SCM $(CFLAGS) -g ScmMain.cpp
		$(CC) -D__DEBUG__SCM $(CFLAGS) -g ScmLib.cpp
		$(CC) -o ScmMain $(OBASEFILES) $(OSCMFILES) $(LFLAGS)
				
###############################################################################
#	INFORMATION
###############################################################################
info:		$(OBASEFILES)
		$(CC) -D__INFO__SCM $(CFLAGS) ScmMain.cpp
		$(CC) -D__INFO__SCM $(CFLAGS) ScmLib.cpp
		$(CC) -o ScmMain $(OBASEFILES) $(OSCMFILES) $(LFLAGS)

###############################################################################
#	PROFILE
###############################################################################
profile:	$(OBASEFILES) $(OSCMFILES)
		$(CC) -pg -o ScmMain $(OBASEFILES) $(OSCMFILES) $(LFLAGS)

###############################################################################
#	PARALLEL AP3000
###############################################################################
parallel:	$(OBASEFILES)
		$(CC) \
			-D__DISTRIBUTED__PARALLEL__MPI \
			-I/opt/FSUNmpiap/include \
			$(CFLAGS) ScmMain.cpp
		$(CC) \
			-D__DISTRIBUTED__PARALLEL__MPI \
			-I/opt/FSUNmpiap/include \
			$(CFLAGS) ScmLib.cpp
		$(CC) \
			-o ScmMain \
			$(OBASEFILES) $(OSCMFILES) \
			-L/opt/FSUNmpiap/lib -lmpi \
			-L/opt/FSUNaprun/lib -lmpl -lemi -lthread \
			$(LFLAGS)

###############################################################################
#	CLEANUP
###############################################################################
cleanall:
		!rm $(OSCMFILES) $(OBASEFILES) ScmMain

clean:
		!rm $(OSCMFILES) ScmMain

###############################################################################
#	END OF MAKEFILE
###############################################################################
