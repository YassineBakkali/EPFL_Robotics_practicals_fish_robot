# What program(s) have to be built
PROGRAMS = ex6_1

# Libraries needed for the executable file
LIBS = -lwsock32

# Dependencies for the program(s) to build
ex6_1: ../common/netutil.o ../common/wperror.o ../common/trkcli.o ../common/remregs.o ../common/utils.o ex6_1.o

# Includes the common Makefile with the various rules
include ../common/Makefile.inc
