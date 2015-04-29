#include <stdio.h>
#include "lily.h"


char test[100][100]=
{	
	"begin",	
	"int",	  "char",  "void" ,   "pint",  "pchar",  
	"pvoid",   "if",    "else",    "for",  "while",         
	"return", "ID",	   "NUM",     "14",    "15",
	"16",     "17",    "18",      "19",    "20",        
	"=",      "+",     "-",       "*",     "/",                 
	"(",      ")",     "[",       "]",     "{",      	 
	"}",      ",",	   ":",       ";",     "'",		   
	"\"",	  "<=",	   "<",       ">=",	   ">",             
	"==",     "!=",    "&",		  "|",	   "!",           
	"&&",     "||" 
};


extern int save_line;
int current_line;

/* parse */
extern void program();
/* symbol table */
extern int hash_init();
extern void sym_table_init(void);

extern void malloc_struct_code();
extern void print_struct_code();
extern void process_struct_code();
extern void init_vm();
extern void start_vm();

int main(int argc, char *argv[])
{
	
    if(freopen( "./Test/a.c", "r", stdin ) == NULL)
    	fprintf(stderr,"error redirecting stdin\n");
    	
    //if(freopen("D:\\output1.txt", "w", stdout) == NULL)
    //    fprintf(stderr,"error redirecting stdout\n");
        
	/*	
		while( (a = get_token() ) != FINISH  )
		{
			if( save_line == current_line )
				printf( "%s\t", test[a] );
			else
			{
				current_line = save_line;
				printf( "\n%s\t", test[a] );
			}
		}
	*/
	hash_init();
	sym_table_init();
	malloc_struct_code();
	program();
	process_struct_code();
	
	print_struct_code();


	init_vm();
	
	start_vm();
	
    system("pause");
	return 0;
}
