#include <stdio.h>
#include "lily.h"

char test[100][100]=
{	
	"begin",	
	"int",	  "char",  "void" ,   "pint",  "pchar",  
	"pvoid",   "if",    "else",    "for",  "while",         
	"return", "read", "write",   "14","15","16",
	"17","18","19","20","21","22","23",
	"24","25","26","27","28","29","30","31","32","33",
	"34","35","36","37","38","39", "ID",	   "NUM",  
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

FILE *stream;

int lily_version()
{
   printf("    __      _      __    __    __ \n"); 
   printf("   / /     /_/    / /    \\ \\  / / \n");
   printf("  / /      __    / /      \\ \\/ /  \n");
   printf(" / /___   / /   / /___     |  | \n");
   printf("/_____/  /__/  /_____/     |__|   \n");
   printf("\nversion: 0.0.1   author: shiyang.wang \n");
   printf("\n\nThanks for using !!\n");
   printf("my blog: http://shiyang.wang\n\n\n");
   return 0;
}
int main(int argc, char *argv[])
{
	lily_version();
/*	
	stream = fopen("a.c","r+");
	
	hash_init();
	sym_table_init();
	malloc_struct_code();
	program();
	
	process_struct_code();
	init_vm();
	start_vm();
	
    //print_struct_code(); */
	return 0;
}



//if(freopen("./test/test.c", "r", stdin) == NULL)
    //	fprintf(stderr,"error redirecting stdin\n");
    	
    	
    //if(freopen("D:\\output1.txt", "w", stdout) == NULL)
    //    fprintf(stderr,"error redirecting stdout\n");
        /*
		int a;
		while( (a = get_token() ) != FINISH  )
		{
			if(a == TOK_ID){
				virtual_get_token(true);
			}
			if( save_line == current_line )
				printf( "%s\t", test[a] );
			else
			{
				current_line = save_line;
				printf( "\n%s\t", test[a] );
			}
		}
	*/
