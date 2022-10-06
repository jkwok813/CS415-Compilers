/*
 *********************************************
 *  415 Compilers                            *
 *  Spring 2022                              *
 *  Students                                 *
 *********************************************
 */


#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "Instr.h"
#include "InstrUtils.h"

void checker(Instruction *ptr2, int regno, int bool){
    //printf("Looking for reg %d\n", regno);
    while(ptr2->field1!= 1024){
        //printf("%d, %d, %d\n", ptr2->field1, ptr2->field2, ptr2->field3);
        if(bool == 1){ //StoreAI or LoadAI:
            if(ptr2->field2 == regno && ptr2->field1 == 0 && ptr2->field3 == 0){
                ptr2->critical = 1;
                //printf("Found3! %d, %d, %d\n", ptr2->field1, ptr2->field2, ptr2->field3);
                return;
            }
            else if(ptr2->field2 == 0 && ptr2->field3 == regno){
                ptr2->critical = 1;
                //printf("Found4! %d, %d, %d\n", ptr2->field1, ptr2->field2, ptr2->field3);
                return;
            }        
        }
        else{
            if(ptr2->field2 == regno && ptr2->field3 == 0){
                ptr2->critical = 1;
                //printf("Found! %d, %d, %d\n", ptr2->field1, ptr2->field2, ptr2->field3);
                return;
            }
            else if(ptr2->field3 == regno){
                ptr2->critical = 1;
                //printf("Found2! %d, %d, %d\n", ptr2->field1, ptr2->field2, ptr2->field3);
                return;
            }
        }    
        ptr2 = ptr2->prev; 
    }
    printf("wow you shouldn't be here. Must be a LoadI.\n");       
}

int main(int argc, char *argv[])
{
        Instruction *InstrList = NULL;
	
	if (argc != 1) {
  	    fprintf(stderr, "Use of command:\n  deadcode  < ILOC file\n");
		exit(-1);
	}

	fprintf(stderr,"------------------------------------------------\n");
	fprintf(stderr,"        Local Deadcode Elimination\n               415 Compilers\n                Spring 2022\n");
	fprintf(stderr,"------------------------------------------------\n");

        InstrList = ReadInstructionList(stdin);
        //fprintf(stderr, "blah blah blah\n");
        /* HERE IS WHERE YOUR CODE GOES */
        //InstrList is the head
        int length = 1;
        Instruction *tail = InstrList;
        while(tail->next != NULL){
            //printf("%d\n", length);
            //printf("%d\n", tail->opcode);
            length++;
            tail = tail->next;
        }
        //Go to tail
        Instruction *ptr = tail;
        int printed = 0;
        while(ptr->field1 != 1024){
            if(ptr->opcode == 13){ //Check for outputAI Code
                ptr->critical = 1;
                //printf("OutputAI found with registers %d, %d, %d\n", ptr->field1, ptr->field2, ptr->field3);
            }
            if(ptr->critical == 1){
                Instruction *ptr2 = ptr->prev;
                if(ptr->field1 == 0){ //LoadAI or OutputAI
                    //printf("LoadAI or OutputAI\n");
                    checker(ptr2, ptr->field2, 1);
                    printed = 1;
                }
                else if(ptr->field2 == 0){ //StoreAI,
                    //printf("StoreAI\n"); 
                    checker(ptr2, ptr->field1, 0);
                    printed = 1;
                }
                else if(ptr->field2 != 0 && ptr->field3 == 0){ //LoadI
                    //checker(ptr2, ptr->field1);
                    //checker(ptr2, ptr->field2);
                  //  printf("LoadI\n");
                  printed = 1;
                }
                else{ //Add, sub, mul
                    checker(ptr2, ptr->field1, 0);
                    checker(ptr2, ptr->field2, 0);
                    //printf("Add, sub, mul\n");
                    //printf("Error at opcode %d with %d, %d, %d\n", ptr->opcode, ptr->field1, ptr->field2, ptr->field3);
                    printed = 1;
                }    
            }
            ptr = ptr->prev;
        }
        //printf("b\n");
        if(printed == 1){
            InstrList->critical = 1;
            //printf("Original Statement found with registers %d, %d, %d\n", InstrList->field1, InstrList->field2, InstrList->field3);
        }        
        Instruction *FList = InstrList;       
        while(FList){
            if(FList->critical != 1){
                //Delete This Instruction!!!!!!!!
                //printf("Deleting deadcode with registers %d, %d, %d\n", FList->field1, FList->field2, FList->field3);
                if(FList == InstrList){
                    InstrList = NULL;
                } 
                if(FList->next != NULL){
                    FList->next->prev = FList->prev;
                }
                if(FList->prev != NULL){
                    FList->prev->next = FList->next;
                }
            }
            FList = FList->next;
        }        
        PrintInstructionList(stdout, InstrList);

	fprintf(stderr,"\n-----------------DONE---------------------------\n");
	
	return 0;
}
