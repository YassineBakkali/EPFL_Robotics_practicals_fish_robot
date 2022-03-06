SRC_SUBDIR = ../firmware/

# Target file name (without extension).
TARGET = main

# List C source files here. (C dependencies are automatically generated.)
# use file-extension c for "c-only"-files
SRC =

# List C source files here which must be compiled in ARM-Mode.
# use file-extension c for "c-only"-files
SRCARM = $(SRC_SUBDIR)uartISR.c $(SRC_SUBDIR)armVIC.c $(SRC_SUBDIR)radio.c $(SRC_SUBDIR)LPC_CANAll.c $(SRC_SUBDIR)sysTime.c $(SRC_SUBDIR)uart.c $(SRC_SUBDIR)i2c.c $(SRC_SUBDIR)can.c $(SRC_SUBDIR)hardware.c $(SRC_SUBDIR)timerISR.c $(SRC_SUBDIR)adc.c $(SRC_SUBDIR)printf.c $(SRC_SUBDIR)utils.c $(SRC_SUBDIR)lutmath.c $(SRC_SUBDIR)registers.c $(SRC_SUBDIR)robot.c $(TARGET).c

# List C++ source files here.
# use file-extension cpp for C++-files (use extension .cpp)
CPPSRC =

# List C++ source files here which must be compiled in ARM-Mode.
# use file-extension cpp for C++-files (use extension .cpp)
CPPSRCARM =

# List Assembler source files here.
# Make them always end in a capital .S.  Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.
ASRC =

# List Assembler source files here which must be assembled in ARM-Mode..
ASRCARM = $(SRC_SUBDIR)crt0.S

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#EXTRAINCDIRS = ./include
EXTRAINCDIRS = $(SRC_SUBDIR)

include $(SRC_SUBDIR)Makefile.inc
