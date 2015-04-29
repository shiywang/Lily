#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lily.h"


extern StructCode *code;
extern int code_line;
extern int main_enter;
extern int main_stack;

static int bp;		//base pointer
static int sp;		//stack pointer
static int *mem;	//memory

void init_vm()
{
	mem = malloc(sizeof(int)*MEMORY_TOP);
	if(!mem){
		fprintf(stderr,"malloc memory error\n");
		exit(0);
	}
	sp = main_stack;
	bp = STACK_BASE;
}

#define IN_JUMP(x) \
		x == OP_UJP ||  \
		x == OP_FJP ||  \
		x == OP_LEQ ||  \
		x == OP_LES ||  \
		x == OP_GRT ||  \
		x == OP_GEQ ||  \
		x == OP_EQU ||  x == OP_NEQ 
		
#define JMP 1
//		i = code[i].value	
		
#define INC4(x)	do{ \
		        x += 4;  \
		        } while ( 0 )

#define DEC4(x)	do { \
				x -= 4;  \
			    } while ( 0 )

void start_vm()
{
	int i = main_enter;
	
	while(code[i].op != OP_STP)
	{
		if(IN_JUMP(code[i].op) || code[i].op == OP_CAL){
			if(code[i].op == OP_CAL){
				//if(code[i].value == -11){
				//	scanf("%d", mem[bp+sp])
				//}else if(code[i].value == -12){
				//	//write
				//}else{
					//正常函数调用 
				//}
			}else{
				switch(code[i].op)
				{
					case OP_UJP:
						i = code[i].value;
						break;
					case OP_FJP:
						!mem[bp+sp] ? i = code[i].value : 1;  
						break;
					case OP_LEQ:
						mem[bp+sp] <= mem[bp+sp-4] ? i = code[i].value : 1 ;
						sp -= 8;
						break;
					case OP_LES:
						mem[bp+sp] < mem[bp+sp-4] ? i = code[i].value : 1 ;
						sp -= 8;
						break;
					case OP_GRT:
						mem[bp+sp] > mem[bp+sp-4] ? i = code[i].value : 1 ;
						sp -= 8;
						break;
					case OP_GEQ:
						mem[bp+sp] >= mem[bp+sp-4] ? i = code[i].value : 1 ;
						sp -= 8;
						break;
					case OP_EQU:
						mem[bp+sp] == mem[bp+sp-4] ? i = code[i].value : 1 ;
						sp -= 8;
						break;
					case OP_NEQ:
						mem[bp+sp] != mem[bp+sp-4] ? i = code[i].value : 1 ;
						sp -= 8;
						break;
					default:
						break;
				}
			}	
		}
		else
		{
			switch(code[i].op)
			{
				case OP_LDA:
					mem[bp+(sp+=4)] = code[i].value;
					break;
				case OP_LDC:
					mem[bp+(sp+=4)] = code[i].value;
					break;
				case OP_LOD:
					mem[bp+(sp+=4)] = mem[code[i].value];
					break;
				case OP_STO:
					mem[mem[bp+sp-4]] = mem[bp+sp];
					sp -= 8;
					break;
				case OP_STN:
					mem[mem[bp+sp-4]] = mem[bp+sp];
					sp -= 4; 
					break;
				//case OP_IND:
				//	break;
				//case OP_IXA:
				//	break;
				case OP_MPI:
					mem[sp-4] = mem[bp+sp] * mem[bp+sp-4];
					sp--;
					break;
				case OP_SBI:
					mem[sp-4] = mem[bp+sp] - mem[bp+sp-4];
					sp--;
					break;
				case OP_ADI:
					mem[sp-4] = mem[bp+sp] + mem[bp+sp-4];
					sp--;
					break;
				case OP_DVI:
					mem[sp-4] = mem[bp+sp] / mem[bp+sp-4];
					sp--;
					break;
				case OP_LAB:	
					break;
				case OP_ENT:
					//sp = code[i].value;
					break;
				case OP_MST:
					bp += FUNCTION_SEGMENT;
					break;
				case OP_RET:
					bp -= FUNCTION_SEGMENT;
					break;
				default:
					fprintf(stderr, "no exit OP\n");
					break;	
			}
			i++;		
		}		
	}

}
