/**********************************************
        CS415  Project 2
        Spring  2022
        Student Version
**********************************************/

#ifndef ATTR_H
#define ATTR_H

typedef union {int num; char *str;} tokentype;

typedef enum type_expression {TYPE_INT=0, TYPE_BOOL, TYPE_ERROR} Type_Expression;

typedef struct {
        char *str[100];
        struct node * next;
        } list;

typedef struct{
        Type_Expression type;
        } typ;
                                   
typedef struct node{
        Type_Expression type;
        int targetRegister;
        } regInfo;

typedef struct {
        int trfl;
        } tf;
        
typedef struct {
        int lower;
        int upper;
        } count;

typedef struct {
        int label1;
        int label2;
        int label3;
        int lower;
        int upper;
        int val;
        int targetRegister;
        } ctrl;
        
typedef struct {
        int label1;
        int label2;
        int label3;
        int hold;
        } wst;        

#endif


  
