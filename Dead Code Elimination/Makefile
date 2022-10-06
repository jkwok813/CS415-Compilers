#/*
# *********************************************
# *  415 Compilers                            *
# *  Spring 2022                              *
# *  Students                                 *
# *********************************************
# */


CCFLAGS = -ggdb -Wall -lm


compile: deadcode.c InstrUtils.c InstrUtils.h Instr.h
	gcc $(CCFLAGS) deadcode.c InstrUtils.c -o deadcode

clean:
	rm -rf deadcode

