#ifndef LILY_H
#define LILY_H

/* WARNING: relative order of these token is important. */
#define TOK_INT            1	    
#define TOK_CHAR           2
#define TOK_VOID           3
#define TOK_PINT           4
#define TOK_PCHAR          5		
#define TOK_PVOID		   6		    
#define TOK_IF             7
#define TOK_ELSE           8        
#define TOK_FOR            9        
#define TOK_WHILE          10   
#define TOK_RETURN         11
#define TOK_READ		   12
#define TOK_WRITE		   13



#define TOK_ID			   40
#define TOK_NUM            41		
#define TOK_ASSIGN         42	
#define TOK_PLUS           43			
#define TOK_MINUS      	   44			
#define TOK_MUL            45           
#define TOK_DIV            46          
#define TOK_LPAREN         47        
#define TOK_RPAREN         48        
#define TOK_LSQUARE        49    
#define TOK_RSQUARE        50    
#define TOK_LBRACKET       51		 
#define TOK_RBRACKET       52    
#define TOK_COMMA	       53			
#define TOK_COLON          54 			
#define TOK_SEMI      	   55
#define TOK_SQUTOA		   56
#define TOK_DQUTOA		   57	          	
#define TOK_LE	           58			
#define TOK_LT             59  			
#define TOK_GE			   60
#define TOK_GT             61
#define TOK_EQ             62           
#define TOK_NE             63 
#define TOK_AND			   64
#define TOK_OR			   65
#define TOK_NOT            66
#define TOK_ANDAND         67
#define TOK_OROR		   68

/* 
	end of the get_token
*/
#define TOK_EOF			   1000

/* lexcial state define */
#define START  -1
#define PAUSE  -2
#define OVER   -3
#define FINISH -4

#define true  1
#define false 0
#define TRUE  1
#define FALSE 0

/* syminfo && hash define */
#define HASH_SIZE   211
#define SHIFT 	      4
#define NAME_SIZE    32

#define MEMORY_BASE 0
#define STACK_BASE 40960
#define MEMORY_TOP 168181760  //top = base + 100*segment
#define FUNCTION_SEGMENT 4096  

#define GLOB_VAR 1
#define FUNCTION 2
#define AUTO_VAR 3
#define PARAM    4


typedef struct ListHead
{
	struct ListHead *next;
	struct ListHead *prev;
}ListHead;

#include "list.h"

/*   sym table  */
typedef struct ParamInfo
{
	char name[NAME_SIZE];
	int type;
	int size;
	int seq;
	int offset;
	int is_array;
	int is_pointer;
	ListHead param_list;
}ParamInfo;

typedef struct AutoInfo
{
	char name[NAME_SIZE];
	int type;
	int size;
	int seq;
	int offset;
	int is_array;
	int is_pointer;
	int level;   /*depth*/
	int block_no;/*belong which block*/
	ListHead auto_list;
}AutoInfo;

typedef struct GlobalInfo
{
	int size;
	int type;
	int seq;
	int offset;
	int is_array;
	int is_pointer;
}GlobalInfo;

typedef struct FuncInfo
{
	int id;
	int total_offset;
	ListHead param_head;
	ListHead auto_head;
	int return_type;
	int param_size;
}FuncInfo;

typedef struct Symbol
{
	char name[NAME_SIZE];
	int type;
	union{
		FuncInfo f;
		GlobalInfo g;
	}u;
}Symbol;

typedef struct SymbolTable
{
	Symbol * value;
	struct SymbolTable * next;
}SymbolTable;

/* for parse.c and symtable*/
typedef struct AutoOffset
{
	int	current_block;
	int	current_level;
	int local_seq;
	int local_total_offset;
	int auto_size;
}AutoOffset;


/* op code */
typedef enum OpCode{
	OP_LDA = 0,
	OP_LDC,
	OP_LOD,
	OP_STO,
	OP_STN,
	OP_IND,
	OP_IXA,
	
	OP_MPI,	//7
	OP_SBI,
	OP_ADI,
	OP_DVI,
	
	OP_LAB,	//11
	OP_UJP,
	OP_FJP,
	OP_LEQ,
	OP_LES,
	OP_GRT, //16
	OP_GEQ,
	OP_EQU,
	OP_NEQ,
	OP_ENT,
	OP_CAL,	//21
	OP_RET,
	
	OP_MST,	//23
	OP_STP,	//24
}OpCode;

typedef struct StructCode{
	int num;
	OpCode op;
	int value;
}StructCode;


/*  lexcial */
extern int  save_line;
extern int 	save_num;
extern char save_word[100];
extern char test[][100];
extern int  get_token();
extern int  virtual_get_token();

#endif
