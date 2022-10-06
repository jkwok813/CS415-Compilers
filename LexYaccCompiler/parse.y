%{
#include <stdio.h>
#include "attr.h"
#include "instrutil.h"
int yylex();
void yyerror(char * s);
#include "symtab.h"

FILE *outfile;
char *CommentBuffer;

int ty = 0;
 
%}

%union {tokentype token;
        regInfo targetReg;
        list listofids;
        typ typ;
        tf tf;
        count count; 
        ctrl ctrl;
        wst wst;
       }

%token PROG PERIOD VAR 
%token INT BOOL PRT THEN IF DO FI ENDWHILE ENDFOR
%token ARRAY OF 
%token BEG END ASG  
%token EQ NEQ LT LEQ GT GEQ AND OR TRUE FALSE
%token WHILE FOR ELSE 
%token <token> ID ICONST 

%type <targetReg> exp 
%type <targetReg> lhs
%type <listofids> idlist 
%type <typ> type
%type <typ> stype
%type <typ> vardcl
%type <targetReg> condexp
%type <ctrl> ctrlexp
%type <wst> wstmt
%type <wst> ifhead 
%type <wst> ifstmt

%start program

%nonassoc EQ NEQ LT LEQ GT GEQ 
%left '+' '-' AND
%left '*' OR

%nonassoc THEN
%nonassoc ELSE

%%
program : {emitComment("Assign STATIC_AREA_ADDRESS to register \"r0\"");
           emit(NOLABEL, LOADI, STATIC_AREA_ADDRESS, 0, EMPTY);} 
           PROG ID ';' block PERIOD { }
	;

block	: variables cmpdstmt { }
	;

variables: /* empty */
	| VAR vardcls { }
	;

vardcls	: vardcls vardcl ';' { ty = 0; } /*empty*/
	| vardcl ';' { ty = 0; }
	| error ';' { yyerror("***Error: illegal variable declaration\n");}  
	;

vardcl	: idlist ':' type { //printf("vardcl");
                            $$.type = $3.type;                           
                            int x = 0;
                            while(x < ty){
                                int offset = NextOffset(1);
                                //printf("\ndeclaring %s", $1.str[x]);
                                insert($1.str[x], $3.type, offset);
                                x++;
                            }
                            //printf(" waawawaw %s\n", $1.str);
                            //yyerror("***Inserted\n");
                                    } /*Insert everything */
	;

idlist	: idlist ',' ID { //yyerror("***idlist");
                         $$.str[ty] = $3.str; 
                         ty++;
                         //printf(" mumuidlist %s\n", $3.str);
                         }
        | ID		{ /*insertion*/ 
                         //yyerror("***idlist\n");
                         //yyerror("***idlist id");
                         $$.str[ty] = $1.str;  
                         ty++;   
                         //printf(" mumuidlist %s\n", $1.str);                
                        }
	;


type	: ARRAY '[' ICONST ']' OF stype { /*yyerror("***type array");*/ $$.type = $6.type; }

        | stype { /*yyerror("***type stype");*/ $$.type = $1.type; }
	;

stype	: INT { /*yyerror("***stype int");*/ $$.type = TYPE_INT; }
        | BOOL { /*yyerror("***stype bool");*/ $$.type = TYPE_BOOL; }
	;

stmtlist : stmtlist ';' stmt { /*yyerror("***stmtlist1");*/ }
	| stmt { /*yyerror("***stmtlist2");*/ }
        | error { yyerror("***Error: ';' expected or illegal statement \n");}
	;

stmt    : ifstmt {/*yyerror("***stmt + i");*/  } /*empty*/
	| fstmt { /*yyerror("***stmt + f");*/ }
	| wstmt { /*yyerror("***stmt + w"); */}
	| astmt { /*yyerror("***stmt + a");*/ }
	| writestmt { /*yyerror("***stmt + wr");*/ }
	| cmpdstmt { /*yyerror("***stmt + c");*/ }
	;

cmpdstmt: BEG stmtlist END { /*yyerror("***cmpdstmt\n");*/ }
	;

ifstmt :  ifhead { int newReg = NextRegister();
                   int newReg2 = NextRegister();
                                //int newReg3 = NextRegister();
                                //int label1 = NextLabel();
                                //int label2 = NextLabel();
                                //int label3 = NextLabel();
                                //$<wst>$.label1 = label1;
                                //$<wst>$.label2 = label2;
                                //$<wst>$.label3 = label3;
                                //emit($<wst>$.label1, NOP, EMPTY, EMPTY, EMPTY);
                                emit(NOLABEL, CBR, $1.hold, $<wst>$.label1, $<wst>$.label2);                   
            }
          THEN{
          emit($<wst>1.label1, NOP, EMPTY, EMPTY, EMPTY);
          }
          stmt 
  	  ELSE 
  	      {
  	      emit($<wst>1.label2, NOP, EMPTY, EMPTY, EMPTY);
  	      }
          stmt 
          FI {
          emit($<wst>1.label3, NOP, EMPTY, EMPTY, EMPTY);}
	;

ifhead : IF condexp {
        int newReg2 = NextRegister();
        int newReg3 = NextRegister();
        int label1 = NextLabel();
        int label2 = NextLabel();
        int label3 = NextLabel();
        $<wst>$.label1 = label1;
        $<wst>$.label2 = label2;
        $<wst>$.label3 = label3;
        $<wst>$.hold = $2.targetRegister;
        //emit(label1, NOP, EMPTY, EMPTY, EMPTY); 
 }
        ;

writestmt: PRT '(' exp ')' { int printOffset = -4; /* default location for printing */
  	                         sprintf(CommentBuffer, "Code for \"PRINT\" from offset %d", printOffset);
	                         emitComment(CommentBuffer);
                                 emit(NOLABEL, STOREAI, $3.targetRegister, 0, printOffset);
                                 emit(NOLABEL, 
                                      OUTPUTAI, 
                                      0,
                                      printOffset, 
                                      EMPTY);
                               }
	;

fstmt	: FOR ctrlexp DO stmt { 
                int newReg1 = NextRegister();
                int newReg2 = NextRegister();
                int newReg3 = NextRegister();
                /*int newReg2 = $2.val;                  				  
				emit(NOLABEL, ADDI, 0, newReg2, 1); 
				emit(NOLABEL, BR, $2.label1, EMPTY, EMPTY);*/
				emit(NOLABEL, LOADAI, 0, $2.val, newReg2); 
				printf("fst %d\n", $2.val);
				emit(NOLABEL, ADDI, newReg2, 1, newReg3); 
				emitComment("wt");
				emit(NOLABEL, STOREAI, newReg3, 0, $2.val);
				emit(NOLABEL, BR, $2.label1, EMPTY, EMPTY);
				emit($2.label3, NOP, EMPTY, EMPTY, EMPTY);
                }
                ENDFOR 
	;

wstmt	: WHILE { int newReg = NextRegister();
                                int newReg2 = NextRegister();
                                int newReg3 = NextRegister();
                                int label1 = NextLabel();
                                int label2 = NextLabel();
                                int label3 = NextLabel();
                                $<wst>$.label1 = label1;
                                $<wst>$.label2 = label2;
                                $<wst>$.label3 = label3;
                                emit(label1, NOP, EMPTY, EMPTY, EMPTY); 
          }
          condexp {
          emit(NOLABEL, CBR, $3.targetRegister, $<wst>2.label2, $<wst>2.label3);
          emit($<wst>2.label2, NOP, EMPTY, EMPTY, EMPTY);                                
          }          
          DO stmt {
          emit(NOLABEL, BR, $<wst>2.label1, EMPTY, EMPTY);
          emit($<wst>2.label3, NOP, EMPTY, EMPTY, EMPTY);
          }
          ENDWHILE {  }
        ;
  

astmt : lhs ASG exp             { 
 				  if (! ((($1.type == TYPE_INT) && ($3.type == TYPE_INT)) || 
				         (($1.type == TYPE_BOOL) && ($3.type == TYPE_BOOL)))) {
				    printf("*** ERROR ***: Assignment types do not match.\n");
				  }

				  emit(NOLABEL,
                                       STORE, 
                                       $3.targetRegister,
                                       $1.targetRegister,
                                       EMPTY);
                                }
	;

lhs	: ID			{ /* Bogus */
                                  //yyerror("***lhs : id");
                                  int newReg1 = NextRegister();
                                  int newReg2 = NextRegister();
				                  int offset = NextOffset(1);
                                  $$.targetRegister = newReg2;
                                  $$.type = TYPE_INT;
				  SymTabEntry * ptr = lookup($1.str);
				  //yyerror("h");
                  if(ptr == NULL){
                    printf("*** ERROR ***: %s not declared.\n", $1.str);
                    ptr->type == TYPE_ERROR;
                    ptr->offset = NextOffset(0);
                  }                   				  
				  //printf(" \n-==-=-=-=-=-=-=-=- %s\n", $1.str); 
				  //yyerror("help");
				  emit(NOLABEL, LOADI, ptr->offset, newReg1, EMPTY);
				  emit(NOLABEL, ADD, 0, newReg1, newReg2);
				  //yyerror("pls");
                         	  }


                                |  ID '[' exp ']' {   }
                                ;


exp	: exp '+' exp		{   //yyerror("***add\n");
                            int newReg = NextRegister();

                                  if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {
    				    printf("*** ERROR ***: Operator types must be integer.\n");
                                  }
                                  $$.type = $1.type;

                                  $$.targetRegister = newReg;
                                  emit(NOLABEL, 
                                       ADD, 
                                       $1.targetRegister, 
                                       $3.targetRegister, 
                                       newReg);
                                }

        | exp '-' exp		{ int newReg = NextRegister();
                                  if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {
    				    printf("*** ERROR ***: Operator types must be integer.\n");
                                  }
                                  $$.type = $1.type;
                                  
                                  $$.targetRegister = newReg;
                                  emit(NOLABEL, 
                                    SUB,
                                    $1.targetRegister,
                                    $3.targetRegister,
                                    newReg);                                                                            
                                }

        | exp '*' exp		{ int newReg = NextRegister();
                                  if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {
    				    printf("*** ERROR ***: Operator types must be integer.\n");
                                  }
                                  $$.type = $1.type;
                                  
                                  $$.targetRegister = newReg;
                                  emit(NOLABEL, 
                                    MULT,
                                    $1.targetRegister,
                                    $3.targetRegister,
                                    newReg);                                                                            
                                }

        | exp AND exp		{ int newReg = NextRegister();
                                  if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {
    				    printf("*** ERROR ***: Operator types must be integer.\n");
                                  }
                                  $$.type = $1.type;
                                  
                                  $$.targetRegister = newReg;
                                  emit(NOLABEL, 
                                    AND_INSTR,
                                    $1.targetRegister,
                                    $3.targetRegister,
                                    newReg);                                                                            
                                } 


        | exp OR exp       	{ int newReg = NextRegister();
                                  if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {
    				    printf("*** ERROR ***: Operator types must be integer.\n");
                                  }
                                  $$.type = $1.type;
                                  
                                  $$.targetRegister = newReg;
                                  emit(NOLABEL, 
                                    OR_INSTR,
                                    $1.targetRegister,
                                    $3.targetRegister,
                                    newReg);                                                                            
                                }


        | ID			{ /* Fixed? */
                              //yyerror("***ID");
	                          int newReg = NextRegister();
                              SymTabEntry * ptr = lookup($1.str);                                    
	                          $$.targetRegister = newReg;
				              $$.type = TYPE_INT;
				              emit(NOLABEL, LOADAI, 0, ptr->offset, newReg);                                  
	                        }

        | ID '[' exp ']'	{ int newReg = NextRegister();
                                if(!($3.type == TYPE_INT)){
                                    printf("*** ERROR ***: Operator types must be integer.\n");
                                }
                              $$.targetRegister = newReg;
                              $$.type = TYPE_INT;
                              SymTabEntry * ptr = lookup($1.str);
                              emit(NOLABEL, LOADAI, 0, ptr->offset, newReg);
                              }
 


	| ICONST                 { int newReg = NextRegister();
	                           $$.targetRegister = newReg;
				   $$.type = TYPE_INT;
				   emit(NOLABEL, LOADI, $1.num, newReg, EMPTY); }

        | TRUE                   { int newReg = NextRegister(); /* TRUE is encoded as value '1' */
	                           $$.targetRegister = newReg;
				   $$.type = TYPE_BOOL;
				   emit(NOLABEL, LOADI, 1, newReg, EMPTY); }

        | FALSE                   { int newReg = NextRegister(); /* FALSE is encoded as value '0' */
	                           $$.targetRegister = newReg;
				   $$.type = TYPE_BOOL;
				   emit(NOLABEL, LOADI, 0, newReg, EMPTY); }

	| error { yyerror("***Error: illegal expression\n");}  
	;


ctrlexp	: ID ASG ICONST ',' ICONST { 
emitComment("begin");
                                    if($3.num > $5.num){ 
                                        yyerror("***Error: lower bound exceeds upper bound\n");
                                      }
                                     $$.lower = $3.num;
                                     $$.upper = $5.num;
                                     int label1 = NextLabel();
                                     int label2 = NextLabel();
                                     int label3 = NextLabel();
                                     $$.label1 = label1;
                                     $$.label2 = label2;
                                     $$.label3 = label3;
                                     int newReg1 = NextRegister();
                                     int newReg2 = NextRegister();
                                     int newReg3 = NextRegister();
                                     int newReg4 = NextRegister();
                                     int newReg5 = NextRegister();
                                     int newReg6 = NextRegister();
                                     int newReg7 = NextRegister();
 				                     SymTabEntry * ptr = lookup($1.str);				 
                                     if(ptr == NULL){
                                        printf("*** ERROR ***: %s not declared.\n", $1.str);
                                        ptr->type == TYPE_ERROR;
                                        ptr->offset = NextOffset(0);
                                     }
                                     emitComment("begin");
                                     $$.val = ptr->offset;
                                     printf("ctrl %d\n", $$.val);
                                     emit(NOLABEL, LOADI, ptr->offset, newReg4, EMPTY);
                                     emit(NOLABEL, ADD, 0, newReg4, newReg5);
                                     emit(NOLABEL, LOADI, $3.num, newReg6, EMPTY);
                                     emit(NOLABEL, LOADI, $5.num, newReg7, EMPTY);
                                     emit(NOLABEL, STORE, newReg6, newReg4, EMPTY);
                                     //emit(NOLABEL, 
                                     emit(label1, LOADAI, 0, ptr->offset, newReg3); 
                                     $$.targetRegister = newReg2;
                                     emit(NOLABEL, CMPLT, newReg3, newReg7, newReg2);
                                     emit(NOLABEL, CBR, newReg2, label2, label3);
                                     emit(label2, NOP, EMPTY, EMPTY, EMPTY);                                                            
                                     }
        ;

condexp	: exp NEQ exp		{   yyerror("***condexp");
                                int newReg = NextRegister();
                                if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {                               
    				                printf("*** ERROR ***: Operator types must be integer.\n");
                                }
                                $$.targetRegister = newReg; 
				                emit(NOLABEL, CMPNE, $1.targetRegister, $3.targetRegister, newReg); 			                                                                            
                            } 

        | exp EQ exp		{ int newReg = NextRegister();
                                if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {                               
    				                printf("*** ERROR ***: Operator types must be integer.\n");
                                }
                                $$.targetRegister = newReg; 
				                emit(NOLABEL, CMPEQ, $1.targetRegister, $3.targetRegister, newReg); 	
                             } 

        | exp LT exp		{ int newReg = NextRegister();
                                if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {                               
    				                printf("*** ERROR ***: Operator types must be integer.\n");
                                }
                                $$.targetRegister = newReg; 
				                emit(NOLABEL, CMPLT, $1.targetRegister, $3.targetRegister, newReg); 	
                             } 

        | exp LEQ exp		{ int newReg = NextRegister();
                                if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {                               
    				                printf("*** ERROR ***: Operator types must be integer.\n");
                                }
                                $$.targetRegister = newReg; 
				                emit(NOLABEL, CMPLE, $1.targetRegister, $3.targetRegister, newReg); 	
                             } 

	| exp GT exp		{ int newReg = NextRegister();
                                if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {                               
    				                printf("*** ERROR ***: Operator types must be integer.\n");
                                }
                                $$.targetRegister = newReg; 
				                emit(NOLABEL, CMPGT, $1.targetRegister, $3.targetRegister, newReg); 	
                             } 

	| exp GEQ exp		{ int newReg = NextRegister();
                                if (! (($1.type == TYPE_INT) && ($3.type == TYPE_INT))) {                               
    				                printf("*** ERROR ***: Operator types must be integer.\n");
                                }
                                $$.targetRegister = newReg; 
				                emit(NOLABEL, CMPGE, $1.targetRegister, $3.targetRegister, newReg); 	
                             } 

	| error { yyerror("***Error: illegal conditional expression\n"); }  
        ;

%%

void yyerror(char* s) {
        fprintf(stderr,"%s\n",s);
        }


int
main(int argc, char* argv[]) {

  printf("\n     CS415 Spring 2022 Compiler\n\n");

  outfile = fopen("iloc.out", "w");
  if (outfile == NULL) { 
    printf("ERROR: Cannot open output file \"iloc.out\".\n");
    return -1;
  }
  CommentBuffer = (char *) malloc(1961);  
  InitSymbolTable();

  printf("1\t");
  yyparse();
  printf("\n");

  PrintSymbolTable();
  
  fclose(outfile);
  
  return 1;
}




