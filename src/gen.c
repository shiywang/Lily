#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "gen.h"
#include "lily.h"


#define MAX_LABEL_SIZE 1000
#define MAX_FUNC_SIZE 1000


extern Symbol* current_function;
extern FILE *output_stream;
extern int gen_op_code;

StructCode *code;
int code_line = 0;
int main_enter = 0;


static int malloc_time = 1;
static int malloc_size = 1000;


#define CODE_ASSIGN(_op,_value) do { \
		if(code_line >= malloc_size) {  \
			realloc_struct_code();  \
		}  \
		code[code_line].num = code_line;  \
		code[code_line].op = _op;  \
		code[code_line].value = _value; \
		code_line++;  		\
	} while ( 0 )
	
	
void emit_lda(int addr)
{
	CODE_ASSIGN(OP_LDA,addr);
}

void emit_ldc(int num)
{
	CODE_ASSIGN(OP_LDC,num);
}

void emit_lod(int addr)
{
	CODE_ASSIGN(OP_LOD,addr);
}

void emit_sto()
{
	CODE_ASSIGN(OP_STO,-1);	
}

void emit_stn()
{
	CODE_ASSIGN(OP_STN,-1);	
}

void emit_ind(int offset)
{
	CODE_ASSIGN(OP_IND,offset);
}

void emit_ixa(int offset)
{
	CODE_ASSIGN(OP_IXA,offset);
}

void emit_mpi()
{
	CODE_ASSIGN(OP_MPI,-1);	
}

void emit_sbi()
{
	CODE_ASSIGN(OP_SBI,-1);
}

void emit_adi()
{
	CODE_ASSIGN(OP_ADI,-1);
}

void emit_dvi()
{
	CODE_ASSIGN(OP_DVI,-1);
}

void emit_lab(int label)
{
	CODE_ASSIGN(OP_LAB,label);
}

void emit_ujp(int label)
{
	CODE_ASSIGN(OP_UJP,label);
}

void emit_fjp(int label)
{
	CODE_ASSIGN(OP_FJP,label);
}

void emit_leq(int label)
{
	CODE_ASSIGN(OP_LEQ,label);
}

void emit_les(int label)
{
	CODE_ASSIGN(OP_LES,label);
}

void emit_grt(int label)
{
	CODE_ASSIGN(OP_GRT,label);
}

void emit_geq(int label)
{
	CODE_ASSIGN(OP_GEQ,label);
}

void emit_equ(int label)
{
	CODE_ASSIGN(OP_EQU,label);
}

void emit_neq(int label)
{
	CODE_ASSIGN(OP_NEQ,label);
}

void emit_ent(int func)
{
	CODE_ASSIGN(OP_ENT,func);
}

void emit_cal(int func)
{
	CODE_ASSIGN(OP_CAL,func);	
}

void emit_ret(void)
{
	CODE_ASSIGN(OP_RET,-1);
}

void emit_stp()
{
	CODE_ASSIGN(OP_STP,-1);
}

void emit_mst(int offset)
{
	CODE_ASSIGN(OP_MST,offset);
}



void malloc_struct_code()
{	
	code = (StructCode*)malloc(sizeof(StructCode)*malloc_size);
}

void realloc_struct_code()
{
	StructCode *p;
	
	malloc_size += 1000;
	p = realloc(code,malloc_size);
	if(!p){
		fprintf(stderr,"realloc struct code error");
	}
	code = p;
}

//#define DEBUG

void gen_code(const char *fmt, ...)
{
	char op_name[3];
	
	va_list ap;
	va_start(ap, fmt);
	
	if(gen_op_code){
		if(!(fmt[0] == 'l' && fmt[1] == 'a' && fmt[2] == 'b'))
			printf("  ");
		vfprintf(output_stream,fmt, ap);
		return ;
	}
	
	strncpy(op_name,fmt,3);
	int num = va_arg(ap,int);
	
	switch(op_name[0])
	{
		case 'c':
			emit_cal(num);
			break;
		case 'l':
			switch(op_name[1]){
				case 'a':	
					emit_lab(num); 
					break;	//lab
				case 'd':
					switch(op_name[2]){
						case 'a':	emit_lda(num); break;
						case 'c':	emit_ldc(num); break;
						default:	goto error;
					}
					break;
				case 'o':
					emit_lod(num);
					break;
				case 'e':	//les, leq
					switch(op_name[2]){
						case 's':	emit_les(num); break;
						case 'q':	emit_leq(num); break;
						default:	goto error;
					}
					break;
				default:
					goto error;
			}
			break;
		case 's':
			switch(op_name[1]){
				case 't':
					switch(op_name[2]){
						case 'n':	emit_stn();	break;
						case 'o':	emit_sto(); break;
						case 'p':	emit_stp();	break;
						default:	goto error;	break;
					}
					break;
				case 'b':
					emit_sbi();
					break;
				default:	
					goto error;
					break;
			}
			break;
		case 'i':
			switch(op_name[1]){
				case 'n':	
					emit_ind(num);
					break;
				case 'x':
					emit_ixa(num);
					break;
			}
			break;
		case 'm':
			switch(op_name[1]){
				case 'p':
					emit_mpi();
					break;
				case 's':
					emit_mst(num);
					break;
			}
			break;
		case 'a':
			emit_adi();
			break;
		case 'd':
			emit_dvi();
			break;
		case 'u':
			emit_ujp(num);
			break;
		case 'f':
			emit_fjp(num);
			break;
		case 'g':
			switch(op_name[1]){
				case 'r':	
					emit_grt(num);
					break;
				case 'e':	
					emit_geq(num);
					break;
				default:	
					goto error;
					break;
			}
			break;
		case 'e':
			switch(op_name[1]){
				case 'q':	
					emit_equ(num);
					break;
				case 'n':
					emit_ent(num);
					break;
				default:	
					goto error;
					break;
			}
			break;
		case 'n':
			emit_neq(num);
			break;
		case 'r':
			emit_ret();
			break;
		default:
		error:
			fprintf(stderr, "Does not exist OpCode: %s\n",op_name);
			break;			
	}	
	va_end(ap);
}

void print_struct_code()
{
	int i;
	for(i = 0; i < code_line; i++)
		printf("num:%d op:%d value:%d\n",code[i].num, code[i].op, code[i].value);
}


#define IN_JUMP(x) \
		x == OP_UJP ||  \
		x == OP_FJP ||  \
		x == OP_LEQ ||  \
		x == OP_LES ||  \
		x == OP_GRT ||  \
		x == OP_GEQ ||  \
		x == OP_EQU ||  x == OP_NEQ 

void process_struct_code()
{
	int label[MAX_LABEL_SIZE];
	int func[MAX_FUNC_SIZE];
	int i;
	
	/* process label jump */
	for(i =0; i < code_line; i++){
		if(code[i].op == OP_LAB){
			if(code[i].value > MAX_LABEL_SIZE){
				fprintf(stderr,"max label size overflow\n");
			}
			label[code[i].value] = i;
		} 
	}
	
	for(i = 0; i < code_line; i++){
		if(IN_JUMP(code[i].op)){
			code[i].value = label[code[i].value];
		}	
	}
	
	/* process function jump*/
	for(i =0; i < code_line; i++){
		if(code[i].op == OP_ENT){
			if(code[i].value > MAX_FUNC_SIZE){
				fprintf(stderr,"max function size overflow\n");
			}
			if(code[i].value != -10 && code[i].value != -11 && code[i].value != -12){
				func[code[i].value] = i;
			}
		} 
	}
	
	for(i = 0; i < code_line; i++){
		if(code[i].op == OP_CAL){
			if(code[i].value != -10 && code[i].value != -11 && code[i].value != -12){
				code[i].value = func[code[i].value];
			}
		}	
	}
	
	
	/* find main function enter */
	for(i = 0; i < code_line; i++){
		if(code[i].op == OP_ENT && code[i].value == -10){
			main_enter = i;
		}
	}
	
}

void write_binary_struct_code()
{
	int i;
	
	char lily[3] = {0x92,0x09,0x26}; //lily  920926
	char ver[3] = {0x00,0x00,0x01};	//version 0.0.1
	
	#define FWRITE_OP_HEAD	\
			do{	\
				fwrite(lily,sizeof(lily),1,output_stream);	\
				fwrite(ver,sizeof(ver),1,output_stream);	\
			} while ( 0 )
			
	FWRITE_OP_HEAD;
	/*
	num-------4;
	OpCode----4;
	value-----4;
	*/
	#define FWRITE_OP_CODE	\
			do{	\
				fwrite(&i,sizeof(int),1,output_stream);	\
				fwrite(&code[i].op,sizeof(code[i].op),1,output_stream); \
				fwrite(&code[i].value,sizeof(int),1,output_stream);		\
			} while ( 0 )
			
	for(i = 0; i < code_line; i++){
		switch(code[i].op){
			
				case OP_LDA:
					FWRITE_OP_CODE;
					break;
				case OP_LDC:
					FWRITE_OP_CODE;
					break;
				case OP_LOD:
					FWRITE_OP_CODE;
					break;
				case OP_STO:
					FWRITE_OP_CODE;
					break;
				case OP_STN:
					FWRITE_OP_CODE;
					break;
				case OP_IND:
					FWRITE_OP_CODE;
					break;
				case OP_IXA:
					FWRITE_OP_CODE;
					break;
				case OP_MPI:
					FWRITE_OP_CODE;
					break;
				case OP_SBI:
					FWRITE_OP_CODE;
					break;
				case OP_ADI:
					FWRITE_OP_CODE;
					break;
				case OP_DVI:
					FWRITE_OP_CODE;
					break;	
				case OP_LAB:
					FWRITE_OP_CODE;
					break;
				case OP_UJP:
					FWRITE_OP_CODE;
					break;
				case OP_FJP:
					FWRITE_OP_CODE;
					break;
				case OP_LEQ:
					FWRITE_OP_CODE;
					break;
				case OP_LES:
					FWRITE_OP_CODE;
					break;
				case OP_GRT:
					FWRITE_OP_CODE;
					break;
				case OP_GEQ:
					FWRITE_OP_CODE;
					break;
				case OP_EQU:
					FWRITE_OP_CODE;
					break;
				case OP_NEQ:
					FWRITE_OP_CODE;
					break;
				case OP_ENT:
					FWRITE_OP_CODE;
					break;
				case OP_CAL:
					FWRITE_OP_CODE;
					break;
				case OP_RET:
					FWRITE_OP_CODE;
					break;
				
				case OP_MST:
					FWRITE_OP_CODE;
					break;
				case OP_STP:
					FWRITE_OP_CODE;
					break;
					
		}
		
	}
	
}
