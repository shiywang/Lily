#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lily.h"
#include "gen.h"
#include "parse.h"
  

extern Symbol* insert_global_sym(int type, char * name);
extern Symbol* lookup_global_sym(int type, char *name);
extern AutoInfo * lookup_local_var(char *name);
extern ParamInfo* lookup_param(char *name);


int main_stack = 0;


static void declaration(int type);
static GlobalInfo* global_var_declaration(int type, char id[]);
static void fun_declaration(int type, char id[]);
static void params();
static void param_list();
static ListHead* param();
static void compound_stmt();


static ListHead* local_var_declaration(int type);
 
static void statement();
static void statement_list();
static void assign_stmt();
static void expression_stmt();
static void selection_stmt();
static void iteration_stmt();
static void return_stmt();
static void expression();


static void var();
static void simple_expression();
static int  relop();
static void additive_expression();
static void term();
static void factor();


static void call();
static void args();


extern void program();
extern int virtual_get_token(int invoke_once);


static int get_label()
{
	static int label = 0;
	return label++;
}

//static __inline__  more effective
static inline int SIZE(int c)
{
	if(c == TOK_INT)
		return 1;		//temp set int to 1
	if(c == TOK_CHAR)
		return 1;
	if(c == TOK_PINT || c == TOK_PCHAR || c == TOK_PVOID)
		return 4;
	if(c == TOK_VOID)
		return 0;
    return -1;
}

static void match(int type)
{
	if(token == type){
		token = get_token();
	} else{
      	fprintf(stderr, "line %d: expected token %s\n", save_line, test[type]);
  	}
}


void program()
{
	int entry_flag  = 0;
	Symbol *p, *q;
	
	/* before parse insert read && write func */
	p = insert_global_sym(FUNCTION,"read");
	q = insert_global_sym(FUNCTION,"write");
	if(!p || !q){
		fprintf(stderr, "init read write function error\n");
		exit(0);
	}
	
	token = get_token();
	
	while(token == TOK_INT || token == TOK_PVOID
	      || token == TOK_VOID || token == TOK_CHAR
	      || token == TOK_PINT || token == TOK_PCHAR)
	{
		entry_flag = 1;
		declaration(token);
	}
	
	if(entry_flag){
		gen_code("stp\n");
		printf("succeed!!  exit()\n");
	}else{
		fprintf(stderr, "line %d: %s does not name a type\n", save_line, save_word );
	}
	
}

static void declaration(int type)
{
	
	GlobalInfo *global_var;
	char id[NAME_SIZE];
	
	type = token;
	match(type);
	match(TOK_ID);
	strncpy(id, save_word, NAME_SIZE);
	
	if(token == TOK_LPAREN){
		
	    fun_declaration(type, id);
	    
	    if(!strcmp(id,"main")){
			main_stack = current_function->u.f.total_offset;
		}
	    
	} else if(token == TOK_SEMI || token == TOK_LSQUARE) {
		
		global_var = global_var_declaration(type, id);
		global_var->seq = global_seq;
		global_var->offset = global_total_offset;
		global_total_offset += global_var->size;
		global_seq++;	
		
	} else{
		
		fprintf(stderr, "line %d: unexpected token %s\n", save_line, save_word);
	}
	
}

static void fun_declaration(int type, char * id)
{
	
	if(type != TOK_VOID)
		has_return_value = false;
			
	// init AutoOffset
	memset(&offset,0,sizeof(AutoOffset));

	current_function = insert_global_sym(FUNCTION,id);
	
	if(current_function == NULL)
	{
		//insert_func failed!
	}
	
	current_function->u.f.return_type = type;

	gen_code("ent %d\n", current_function->u.f.id);
	
	/* match ( */
	match(TOK_LPAREN);
	
	params();
	
	/*match ) */
	match(TOK_RPAREN);
	
	//add after insert auto_var
	//current_function->u.f.total_offset += current_function->u.f.param_size;
	
	compound_stmt();
	
	if(current_function->u.f.return_type == TOK_VOID && has_return_value){
		fprintf(stderr, "line %d: this fuction doesn't return value'\n", save_line);
	}
	
}


static GlobalInfo* global_var_declaration(int type, char *id)
{
	Symbol * var;
	int size = SIZE(type);
	
	var = insert_global_sym(GLOB_VAR,id);
	if(var == NULL)
	{
        fprintf(stderr, "line %d: malloc global var error\n", save_line);
	}
	var->type = type;
	
	
	/* var */
	if(token == TOK_SEMI) 
	{
		if(type == TOK_PINT || type == TOK_PCHAR)
		{
			var->u.g.is_pointer = true;
		}
		var->u.g.size = size;
		match(TOK_SEMI);
	}
	/* array */
	else if(token == TOK_LSQUARE)
	{
		match(TOK_LSQUARE);
		match(TOK_NUM);
    	match(TOK_RSQUARE);
    	match(TOK_SEMI);
    	var->u.g.is_array = true;
    	var->u.g.size = save_num * size;
	}
	else
		fprintf(stderr, "line %d: param does not a type\n", save_line);
		
	return &var->u.g;	
}

static void params()
{
	if(token == TOK_VOID){
		match(TOK_VOID);
	}
	else if( token == TOK_INT 
		  || token == TOK_CHAR
	 	  || token == TOK_PINT
		  || token == TOK_PCHAR
	 	  || token == TOK_VOID
		  || token == TOK_PVOID){
		param_list();
	}else{
		if(token != TOK_RPAREN)
			fprintf(stderr, "line %d: function params error\n", save_line);
	}
}

static void param_list()
{
	int seq = 0;
	do
	{
		insert_param(param(),seq,&current_function->u.f.param_head);
		seq++;
		if(token == TOK_COMMA)
			match(TOK_COMMA);
		else
			break;
	}while(true);
}

static ListHead* param()
{
    ParamInfo *param = NULL;
	
    param = malloc(sizeof(ParamInfo));
	
	if(param == NULL)
	{
		fprintf(stderr,"param node malloc error");
		exit(0);
	}
	
	memset(param,0,sizeof(ParamInfo));
	INIT_LIST_NODE(&param->param_list);
	
    param->type = token;
    param->size = SIZE(token);
	
	if(token == TOK_INT || token == TOK_CHAR||
	   token == TOK_PINT || token == TOK_PCHAR||
	   token == TOK_VOID || token == TOK_PVOID)
	{
		match(token);
	}
	
	strncpy(param->name,save_word,NAME_SIZE); 	
	match(TOK_ID);	
	
	if(token==TOK_LSQUARE)
	{
		param->is_array = true;
		match(TOK_LSQUARE);
		match(TOK_RSQUARE);
	}	
	
    return &param->param_list;
}


static void compound_stmt()
{
    ListHead *t = NULL;
	
	/* match { */
	match( TOK_LBRACKET );
	
	while(token==TOK_INT||token==TOK_CHAR 
	    ||token==TOK_PINT||token==TOK_PCHAR)
	{	
		t = local_var_declaration(token);
	    insert_local_var(t,&offset,&(current_function->u.f.auto_head));
		offset.current_level++;
		offset.current_block++;
	}
	
	current_function->u.f.total_offset = offset.local_total_offset;
	
	statement_list();
	/* match } */
	match(TOK_RBRACKET);
	offset.current_level--;
}

static ListHead* local_var_declaration(int type)
{
	
	AutoInfo *a = NULL;
	int size = SIZE(type); 
	
    a = malloc(sizeof(AutoInfo));
    
	if(a == NULL){
        fprintf(stderr, "line %d: malloc auto error\n", save_line);
        exit(0);
	}
	
  	memset(a, 0, sizeof(AutoInfo));
    INIT_LIST_NODE(&a->auto_list);
	
	match(token);
	strncpy(a->name,save_word,NAME_SIZE);
	match(TOK_ID);
	
	a->type = type;

	/* var */
	if(token == TOK_SEMI){
        if(type == TOK_PINT || type == TOK_PCHAR)
			a->is_pointer = true;
		a->size = size;
		match(TOK_SEMI);
    }
	/* array */
	else if(token == TOK_LSQUARE){
        /* match [ */
        match(TOK_LSQUARE);
        match(TOK_NUM);
        match(TOK_RSQUARE);
        match(TOK_SEMI);
        a->size = size * save_num;
        a->is_array = true;
    }else{
		fprintf(stderr, "line %d: param does not a type\n", save_line);
	}
	
    return &a->auto_list;	
}

static void statement_list()
{
	
	while(token == TOK_IF || token == TOK_LBRACKET
        ||token == TOK_ID || token == TOK_MUL
        ||token == TOK_AND || token == TOK_WHILE
        ||token == TOK_RETURN || token == TOK_SEMI
		||token == TOK_FOR)
	{		
    	statement();
  	}
  	
}

static void statement()
{
	int second_token;
	
	if(token == TOK_IF){
		selection_stmt();
	}else if(token == TOK_LBRACKET){
		compound_stmt();
	}else if(token == TOK_ID||token == TOK_AND){
		second_token = virtual_get_token(true);
		if(second_token == TOK_ASSIGN){
			assign_stmt();
		}else if(second_token == TOK_LSQUARE){
			
			while(second_token!=TOK_RSQUARE){
				second_token = virtual_get_token(false);
				if(second_token == TOK_ASSIGN 
				   ||second_token == TOK_SEMI
				   ||second_token == TOK_EOF)
				{
					fprintf( stderr, "line %d: error array var", save_line );
					break;
				}
			}
			second_token = virtual_get_token(false);
			
			if(second_token == TOK_ASSIGN){
				assign_stmt();
			}else{
				expression_stmt();
			}
			
		}else{
			expression_stmt();
		}
		
	}else if(token == TOK_WHILE||token == TOK_FOR){
		iteration_stmt();
	}else if(token == TOK_RETURN){
		return_stmt();
	}else if(token == TOK_SEMI){
		match(TOK_SEMI); 
	}else
		fprintf( stderr, "line %d: error statements", save_line );
		
}

static void assign_stmt()
{
	int addr;

	before_assign = true;
	
	var();
		
	match(TOK_ASSIGN);
	
	before_assign = false;
	expression();
	
	gen_code("sto\n");
	
	match(TOK_SEMI);
}

static void expression_stmt()
{
	expression();
	match(TOK_SEMI);
}

static void selection_stmt()
{
	int first_label = -1;
	int second_label = -1;
	
	match(TOK_IF);
	match(TOK_LPAREN);
	expression();
	
	
	first_label = get_label();
	gen_code("fjp %d\n", first_label);
	
	match(TOK_RPAREN);
	
	statement();
	
	second_label = get_label();
	gen_code("ujp %d\n", second_label);
	gen_code("lab %d:\n", first_label);
	
	if(token == TOK_ELSE){
		match(TOK_ELSE);
		statement();
	}
	
	gen_code("lab %d:\n", second_label);
}



static void iteration_stmt()
{
	int first_label = -1;
	int second_label = -1;
	
	if(token == TOK_WHILE)
	{
		first_label = get_label();
		gen_code("lab %d:\n", first_label);
		match(TOK_WHILE);
		match(TOK_LPAREN); /* ( */
		expression();
		
		second_label = get_label();
		gen_code("fjp %d\n", second_label);
		
		match(TOK_RPAREN); /* ) */
		statement();
		gen_code("ujp %d\n", first_label);
		gen_code("lab %d:\n", second_label);
				
	}else if(token == TOK_FOR){
		
		match(TOK_FOR);
		match(TOK_LPAREN);/* ( */	
		expression();
		match(TOK_SEMI);
		first_label = get_label();
		gen_code("lab %d:\n", first_label);
		expression();
		match(TOK_SEMI);
		expression();
		match(TOK_RPAREN);/* ) */
		
		second_label = get_label();
		gen_code("fjp %d\n", second_label);
		//gen_code("etu %d\n", second_label);
		
		statement();
		gen_code("ujp %d\n", first_label);
		gen_code("lab %d:\n", second_label);	
	}
	else{
		
	}
	
}


static void return_stmt()
{
	
	match(TOK_RETURN);
	
	if(token == TOK_SEMI)
	{
		gen_code("ret\n");
		match(TOK_SEMI);
	}
	else
	{
		expression();
		gen_code("ret\n");
		match(TOK_SEMI);
		has_return_value = true;
	}
}


static void expression()
{
	int op_token;
	int first_label = -1;
	int second_label = -1;
	
	
	additive_expression();
	
	if(!(token==TOK_LE||token==TOK_LT
	   ||token==TOK_GT||token==TOK_GE
	   ||token==TOK_EQ||token==TOK_NE))
	{ 	
	 	return ; 
	}
	
	while(token==TOK_LE||token==TOK_LT||
	 	  token==TOK_GT||token==TOK_GE||
		  token==TOK_EQ||token==TOK_NE)
	{
		op_token = token;
		match(token);
		
	 	additive_expression();
		
		//gen_code("sbi\n");
		//gen_code("exc\n");
	 	//gen_code("cmp\n");
	 	
	 	first_label = get_label();
	 	
	 	switch(op_token)
		{
			case TOK_LE:	// <=
				gen_code("leq L%d\n", first_label);
				break;
			case TOK_LT:	// < 
				gen_code("les L%d\n", first_label);
				break;
			case TOK_GT:	// >
				gen_code("grt L%d\n", first_label);
				break;
			case TOK_GE:	// >=
				gen_code("geq L%d\n", first_label);
				break;
			case TOK_EQ:	// ==
				gen_code("equ L%d\n", first_label);
				break;
			case TOK_NE:	// !=
				gen_code("neq L%d\n", first_label);
				break;
		}
		
		second_label = get_label();
		gen_code("ldc %d\n", false);		
      	gen_code("ujp L%d\n", second_label);
      	gen_code("lab %d:\n", first_label);
      	gen_code("ldc %d\n", true);		
      	gen_code("lab %d:\n", second_label);
      	
	}
	
}

static int relop()
{
	switch(token)
	{
		case TOK_LE:
			match(TOK_LE);
			break;
		case TOK_LT:
			match(TOK_LT);
			break;
		case TOK_GT:
			match(TOK_GT);
			break;
		case TOK_GE:
			match(TOK_GE);
			break;
		case TOK_EQ:
			match(TOK_EQ);
			break;
		case TOK_NE:
			match(TOK_NE);
			break;
	}
	return token;
}

static void additive_expression()
{
	int op_token;
	term();
	while(token == TOK_PLUS || token == TOK_MINUS)
	{
		op_token = token;
		
		match(token);
		term();
		
		if(op_token == TOK_PLUS){
			gen_code("adi\n");
		}else if(op_token == TOK_MINUS){
			gen_code("sbi\n");
		}
		else{
			//common_error
		}
		
	}
	
}

static void term()
{
	int op_token;
	factor();
	while(token == TOK_MUL||token == TOK_DIV)
	{
		op_token = token;
		
		match(token);
		factor();
		if(op_token == TOK_MUL){
			gen_code("mpi\n");
		}else if(op_token == TOK_DIV){
			gen_code("dvi\n");
		}else{
			//common_error
			
		}
	}
	
}

static void factor()
{
    int next_token;
	if(token == TOK_LPAREN){
		match(TOK_LPAREN);
		expression();		//does not support (x=x+3)+4
		match(TOK_RPAREN);
		
	}else if(token==TOK_ID||token == TOK_AND){	
        next_token = virtual_get_token(true);
		if(next_token == TOK_LPAREN){
 			call();
		}else{
            var();
        }
	}
	else if(token == TOK_NUM){
		
		gen_code("ldc %d\n", save_num);
		match(TOK_NUM);
		
	}else if(token == TOK_PLUS||token == TOK_MINUS) { 
		
		/*  0 + a ->  +a
		    0 - a ->  -a
		*/
		//first element is zero
	}
	else
        fprintf(stderr, "line %d: unexpected factor %s\n", save_line, save_word);
}

static void var()
{
    int has_prefix = false;
    int var_addr;
    
    AutoInfo * _auto = NULL;
    ParamInfo * _param = NULL;
    Symbol * _global = NULL;
    //Symbol * temp = NULL;
    
	if(token==TOK_MUL||token==TOK_AND){
        has_prefix = true;
		/* if is * or & */
		match(token);
		//如果有前缀 则返回指针地址或者指针值 
	}

    _global = lookup_global_sym(GLOB_VAR,save_word);
    _auto   = lookup_local_var(save_word);
    _param  = lookup_param(save_word);
    
    match(TOK_ID);
    
    /* array */
    if(token == TOK_LSQUARE){
    	
    	if(_auto && _param){
	        fprintf(stderr, "line %d: param and auto can't be same name\n", save_line);
	    }
	
	    if(_param){

	    	gen_code("lda %d\n", _param->offset);
	
	    }else if(_auto){
	    	
			gen_code("lda %d\n", _auto->offset);
			
	    }else if(_global){

	    	gen_code("lda %d\n", _global->u.g.offset);
	
	    }else{
	        fprintf(stderr, "line %d: can't find var %s\n", save_line, save_word);
	    }
	    
		match(TOK_LSQUARE);
		var_in_square = true;
		expression();
		var_in_square = false;
 		match(TOK_RSQUARE);
 		gen_code("ixa elem_size(%d)\n", 1);
 		
    	if(!before_assign){
    		gen_code("ind %d\n",0);	
    	}
 		
	}
	/* not array */
	else
	{
		if(_auto && _param){
	        fprintf(stderr, "line %d: param and auto can't be same name\n", save_line);
	    }
	
	    if(_param){
	    	
	    	var_addr = _param->offset;
	
	    }else if(_auto){
	    	
			var_addr = _auto->offset;
			
	    }else if(_global){
	    
	    	var_addr = _global->u.g.offset;
	
	    }else{
	        fprintf(stderr, "line %d: can't find var %s\n", save_line, save_word);
	    }
	    
		
	    if((before_assign && !var_in_square) || 
		(var_in_read_write && !var_in_square)){
	    	
	    	gen_code("lda %d\n", var_addr);
	    	
	    }else{
	    	
	    	gen_code("lod %d\n", var_addr);
	    	
	    }
    
	}
	
}

static void call()
{
	char func_name[NAME_SIZE];
	Symbol *f;
	
	strcpy(func_name,save_word);
	match(TOK_ID);
	
	f = lookup_global_sym(FUNCTION,func_name);
	if(!f){
		fprintf(stderr,"can not call func %s", func_name);
	}
	
	if(strcmp(save_word,"read") && strcmp(save_word,"write")){
    	gen_code("mst %d\n", f->u.f.total_offset*10000 + f->u.f.param_size);
    }else{
    	var_in_read_write = true;
    }
    
	match(TOK_LPAREN);
	args();
	match(TOK_RPAREN);
	
	var_in_read_write = false;
	
	gen_code("cal %d\n",f->u.f.id);
}

static void args()
{
	if(token == TOK_ID||token == TOK_NUM)
	{
		expression();
		while(token == TOK_COMMA)
		{
			match(TOK_COMMA);
			expression();
		}
	}
}

