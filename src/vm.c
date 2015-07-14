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
			    
typedef struct PointerStack{
	int bp;
	int sp;
}PointerStack;

int ptop = 0;
PointerStack pstack[100];

void start_vm()
{
	int i = main_enter;
	int old_bp, old_sp, param_offset, auto_offset;
	int stack[100];//stack for save args
	int return_value, not_main_return = false;
	int j, top, x, offset_addr;
	int current_code = -1;
	
	while(code[i].op != OP_STP)
	{
		if(IN_JUMP(code[i].op) || code[i].op == OP_CAL){
			if(code[i].op == OP_CAL){
				if(code[i].value == -11){
					scanf("%d", &mem[mem[bp+sp-1]]);
					sp-=1;
					i++;
				}else if(code[i].value == -12){
					if(code[i-1].op == OP_LDC || code[i-1].op == OP_IND){					
						printf("%d", mem[bp+sp-1]);
					}else{
						printf("%d", mem[mem[bp+sp-1]]);
					}
					sp-=1;
					i++;
				}else{
					/* save current code */
					current_code = i;
					not_main_return = true;
		
					/* save args */
					top = 0;
					j = param_offset;
					while(j--){
						stack[top++] = mem[bp+sp-1];
						sp--; 
					}
					
					/*
					for(j = i-1, top = 0; code[j].op != OP_MST; j--){
						if(code[j].op == OP_LOD){
								if(code[j].value >= STACK_BASE){
									offset_addr = code[j].value - STACK_BASE;
								}
								stack[top++] = mem[bp+offset_addr];			
						}else{	// OP_LDA
							// not support yet
						}
						sp--;
					}*/
					
					/* save old stack bp sp*/
					pstack[ptop].bp = bp;
					pstack[ptop].sp = sp;
					ptop++;
					/* change stack */
					bp += FUNCTION_SEGMENT;
					
					/* args to param */
					x = bp;
					while(top){
						mem[x++] = stack[top-1];
						top--;
					}
					/* change sp */
					sp = auto_offset + param_offset;
					
					/* jump to function code */
					i = code[i].value;
				}
			}else{
				switch(code[i].op)
				{
					case OP_UJP:
						i = code[i].value;
						break;
					case OP_FJP:
						if(!mem[bp+sp-1]){
						 	i = code[i].value;
						}else{
							i++;
						}
						sp -= 1;  
						break;
					case OP_LEQ:
						if(mem[bp+sp-2] <= mem[bp+sp-1]){
							i = code[i].value;
						}else{
							i++;
						}
						sp -= 2;
						break;
					case OP_LES://
						if(mem[bp+sp-2] < mem[bp+sp-1]){
							i = code[i].value;
						}else{
							i++;
						}
						sp -= 2;
						break;
					case OP_GRT:
						if(mem[bp+sp-2] > mem[bp+sp-1]){
							i = code[i].value;
						}else{
							i++;
						}
						sp -= 2;
						break;
					case OP_GEQ:
						if(mem[bp+sp-2] >= mem[bp+sp-1]){
							i = code[i].value;
						}else{
							i++;
						}
						sp -= 2;
						break;
					case OP_EQU:
						if(mem[bp+sp-2] == mem[bp+sp-1]){
							i = code[i].value;
						}else{
							i++;
						}
						sp -= 2;
						break;
					case OP_NEQ:
						if(mem[bp+sp-2] != mem[bp+sp-1]){
							i = code[i].value;
						}else{
							i++;
						}
						sp -= 2;
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
					if(code[i].value >= STACK_BASE){
						offset_addr = code[i].value - STACK_BASE;
						mem[bp+sp] = bp + offset_addr;
					}else{
						mem[bp+sp] = code[i].value;
					}
					sp++;
					break;
				case OP_LDC:
					mem[bp+sp] = code[i].value;
					sp++;
					break;
				case OP_LOD:
					if(code[i].value >= STACK_BASE){
						offset_addr = code[i].value - STACK_BASE;
						mem[bp+sp] = mem[bp+offset_addr];
					}else{
						mem[bp+sp] = mem[code[i].value];
					}		
					sp++;
					break;
				case OP_STO:
					mem[mem[bp+sp-2]] = mem[bp+sp-1];
					sp-=2;
					break;
				case OP_STN:
					mem[mem[bp+sp-2]] = mem[bp+sp-1];
					sp--; 
					break;
				case OP_IND:
					mem[bp+sp-1] = mem[mem[bp+sp-1]+code[i].value];
					break;
				case OP_IXA:
					mem[bp+sp-2] += (code[i].value * mem[bp+sp-1]);
					sp--;
					break;
				case OP_MPI:
					mem[bp+sp-2] = mem[bp+sp-2] * mem[bp+sp-1];
					sp-=1;
					break;
				case OP_SBI:
					mem[bp+sp-2] = mem[bp+sp-2] - mem[bp+sp-1];
					sp-=1;
					break;
				case OP_ADI:
					mem[bp+sp-2] = mem[bp+sp-2] + mem[bp+sp-1];
					sp-=1;
					break;
				case OP_DVI:
					mem[bp+sp-2] = mem[bp+sp-2] / mem[bp+sp-1];
					sp-=1;
					break;
				case OP_LAB:
					//do nothing	
					break;
				case OP_ENT:
					//sp = code[i].value;
					break;
				case OP_MST:
					auto_offset = code[i].value / 10000;
					param_offset = code[i].value % 10000;
					break;
				case OP_RET:
					return_value = mem[bp+sp-1];
					bp = pstack[ptop-1].bp;
					sp = pstack[ptop-1].sp;
					ptop--;
					mem[bp+sp] = return_value;
					sp++;
					if(not_main_return){
						i = current_code;
						not_main_return = false;
					}
					break;
				default:
					fprintf(stderr, "Not exist OP\n");
					break;
			}
			i++;		
		}		
	}

}
