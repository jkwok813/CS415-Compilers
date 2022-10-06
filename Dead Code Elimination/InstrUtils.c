/*
 *********************************************
 *  415 Compilers                            *
 *  Spring 2022                              *
 *  Students                                 *
 *********************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Instr.h"
#include "InstrUtils.h"

void PrintInstruction(FILE * outfile, Instruction * instr)
{
	if (!outfile) {
  	    fprintf(stderr, "File error - no output file\n");
	    exit(-1);
	}
	if (instr) {
		switch (instr->opcode) {
		case LOADI:
			fprintf(outfile, "loadI %d => r%d\n", instr->field1,
				instr->field2);
			break;
		case LOAD:
			fprintf(outfile, "load r%d => r%d\n", instr->field1,
				instr->field2);
			break;
		case LOADAI:
			fprintf(outfile, "loadAI r%d, %d => r%d\n", instr->field1,
				instr->field2, instr->field3);
			break;
		case LOADAO:
			fprintf(outfile, "loadAO r%d, r%d => r%d\n", instr->field1,
				instr->field2, instr->field3);
			break;
		case STORE:
			fprintf(outfile, "store r%d => r%d\n", instr->field1,
				instr->field2);
			break;
		case STOREAI:
			fprintf(outfile, "storeAI r%d => r%d, %d\n", instr->field1,
				instr->field2, instr->field3);
			break;
		case STOREAO:
			fprintf(outfile, "storeAO r%d => r%d, r%d\n", instr->field1,
				instr->field2, instr->field3);
			break;
		case LSHIFTI:
			fprintf(outfile, "lshiftI r%d, %d => r%d\n", instr->field1,
				instr->field2, instr->field3);
			break;
		case RSHIFTI:
			fprintf(outfile, "rshiftI r%d, %d => r%d\n", instr->field1,
				instr->field2, instr->field3);
			break;
		case ADD:
			fprintf(outfile, "add r%d, r%d => r%d\n", instr->field1,
				instr->field2, instr->field3);
			break;
		case SUB:
			fprintf(outfile, "sub r%d, r%d => r%d\n", instr->field1,
				instr->field2, instr->field3);
			break;
		case MUL:
			fprintf(outfile, "mult r%d, r%d => r%d\n", instr->field1,
				instr->field2, instr->field3);
			break;
		case DIV:
			fprintf(outfile, "div r%d, r%d => r%d\n", instr->field1,
				instr->field2, instr->field3);
			break;
		case OUTPUTAI:
		  fprintf(outfile, "outputAI r%d, %d\n", instr->field1, instr->field2);
			break;
		default:
	    	    fprintf(stderr, "Illegal instructions\n");
		}
		
	}
}

void PrintInstructionList(FILE * outfile, Instruction * instr)
{
	Instruction *prev;

	while (instr) {
		PrintInstruction(outfile, instr);
		prev = instr;
		instr = instr->next;
		free(prev);
	}
}

Instruction *ReadInstruction(FILE * infile)
{
	static char InstrBuffer[100];
	Instruction *instr = NULL;
	char dummy;

	if (!infile) {
  	    fprintf(stderr, "File error - no input file\n");
	    exit(-1);
	}
	instr = (Instruction *) calloc(1, sizeof(Instruction));
	if (!instr) {
  	    fprintf(stderr, "Calloc failed\n");
	    exit(-1);
	}
	instr->prev = NULL;
	instr->next = NULL;

	fscanf(infile, "%99s", InstrBuffer);
	if (strnlen(InstrBuffer, sizeof(InstrBuffer)) == 0) {
		free(instr);
		return NULL;
	}
        
	/* skip over comments */        
        while (!strcmp(InstrBuffer, "//")) {
	  fscanf(infile, "%*[^\n]\n");
  	  fscanf(infile, "%99s", InstrBuffer);
	}
	
	if (!strcmp(InstrBuffer, "loadI")) {
		instr->opcode = LOADI;
		/* get first operand: immediate constant */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%d", &(instr->field1));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get second operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field2));
 	} else if (!strcmp(InstrBuffer, "load")) {
		instr->opcode = LOAD;
		/* get first operand: base register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get second operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field2));
 	} else if (!strcmp(InstrBuffer, "loadAI")) {
		instr->opcode = LOADAI;
		/* get first operand: base register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* get second operand: immediate constant */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%d", &(instr->field2));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get third operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field3));
	} else if (!strcmp(InstrBuffer, "loadAO")) {
		instr->opcode = LOADAO;
		/* get first operand: base register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* get second operand: offset register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field2));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get third operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field3));
 	} else if (!strcmp(InstrBuffer, "loadAI")) {
		instr->opcode = LOADAI;
		/* get first operand: base register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* get second operand: immediate constant */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%d", &(instr->field2));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get third operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field3));
	} else if (!strcmp(InstrBuffer, "store")) {
		instr->opcode = STORE;
		/* get first operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get second register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field2));
	} else if (!strcmp(InstrBuffer, "storeAI")) {
		instr->opcode = STOREAI;
		/* get first operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get base register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field2));
		/* get second operand: immediate constant */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%d", &(instr->field3));
	} else if (!strcmp(InstrBuffer, "storeAO")) {
		instr->opcode = STOREAO;
		/* get first operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get base register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field2));
		/* get second operand: immediate constant */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field3));
	} else if (!strcmp(InstrBuffer, "lshiftI")) {
		instr->opcode = LSHIFTI;
		/* get first operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* get second operand: immediate constant */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%d", &(instr->field2));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get third operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field3));
	} else if (!strcmp(InstrBuffer, "rshiftI")) {
		instr->opcode = RSHIFTI;
		/* get first operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* get second operand: immediate constant */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%d", &(instr->field2));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get third operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field3));
	} else if (!strcmp(InstrBuffer, "add")) {
		instr->opcode = ADD;
		/* get first operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* get second operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field2));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get third operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field3));
	} else if (!strcmp(InstrBuffer, "sub")) {
		instr->opcode = SUB;
		/* get first operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* get second operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field2));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get third operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field3));
	} else if (!strcmp(InstrBuffer, "mult")) {
		instr->opcode = MUL;
		/* get first operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* get second operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field2));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get third operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field3));
	} else if (!strcmp(InstrBuffer, "div")) {
		instr->opcode = DIV;
		/* get first operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* get second operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field2));
		/* skip over "=>"  */
		fscanf(infile, "%s", InstrBuffer);
		/* get third operand: register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field3));
	} else if (!strcmp(InstrBuffer, "outputAI")) {
		instr->opcode = OUTPUTAI;
		/* get first operand: target register */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%c%d", &dummy, &(instr->field1));
		/* get second operand: immediate constant */
		fscanf(infile, "%s", InstrBuffer);
		sscanf(InstrBuffer, "%d", &(instr->field2));
	} else {
		free(instr);
		return NULL;
	}
	return instr;
}

Instruction *ReadInstructionList(FILE * infile)
{
	Instruction *instr, *head, *tail;

	if (!infile) {
  	    fprintf(stderr, "File error - no input file\n");
	    exit(-1);
	}
	head = tail = NULL;
	while ((instr = ReadInstruction(infile))) {
		if (!head) {
			head = tail = instr;
			continue;
		}
		instr->prev = tail;
		instr->next = NULL;
		tail->next = instr;
		tail = instr;
	}
	return head;
}

Instruction *LastInstruction(Instruction * instr)
{
	if (!instr) {
  	       fprintf(stderr, "No instructions\n");
    	       exit(-1);
	}
	while (instr->next)
		instr = instr->next;
	return instr;
}

void DestroyInstructionList(Instruction * instr)
{
	Instruction *i;

	if (!instr)
		return;
	while (instr) {
		i = instr;
		instr = instr->next;
		free(i);
	}
}
