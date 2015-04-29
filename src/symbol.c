#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "lily.h"

extern int current_block;
extern int current_level;
extern Symbol* current_function;
extern AutoOffset offset;

static int func_seq = 0;

SymbolTable table[HASH_SIZE];

static int hash(const char* key)
{
    int temp=0;
    int i=0;
    while(key[i]!='\0')
    {
        temp = ((temp<<SHIFT)+key[i])%HASH_SIZE;
        i++;
    }
    return temp;
}

//当C的数据结构非常复杂的时候，每次使用时都需要初始化那些空指针
//以及一些复杂的结构体 
// 这就是为什么C++设计构造函数和析构函数的原因吧 
int hash_init(void)
{
    int i;
    memset(table,0,sizeof(SymbolTable)*HASH_SIZE);
}

void sym_table_init(void)
{
    int i;
    for(i=0; i < HASH_SIZE; i++)
    {
        table[i].value = NULL;
        table[i].next = NULL;
    }
}

// 将原来的 lookup_global_var  lookup_global_func 抽象为lookup_global_sym
// 通过用结构体 抽象，使原来不同的返回值变为相同的返回值。
// 通过"结构体解耦"
//   struct{ int a, int b, int c}  总比 穿三个 int a, b,c 参数要方便。 
Symbol* lookup_global_sym(int type, char *name)
{
	assert(name != NULL);
	SymbolTable *t;
    
    t = &table[hash(name)];
    
    while(t->value != NULL)
    {
        if(!strcmp(t->value->name,name)
		   &&t->value->type == type)
        {
            return t->value;
        }
        t = t->next;
    }
    return NULL;
}

Symbol* insert_global_sym(int type, char * name)
{
	assert(name != NULL);
	SymbolTable *t, *l;
    Symbol *pnode;
    
    if(lookup_global_sym(type,name)!=NULL){
        fprintf(stderr,"line %d:func already exist\n", save_line);
        return NULL;
    }

    t = &table[hash(name)];
    
    while(t->next!=NULL){
    	l = t;
        t = t->next;
    }
    
    if(t == NULL){
    	t = (SymbolTable*)malloc(sizeof(SymbolTable));
    	if(!t){
    		fprintf(stderr,"malloc error!!");
        	return NULL;
		}
    	l->next = t;
	}
	
    pnode = (Symbol*)malloc(sizeof(Symbol));
    if(!pnode){
    	free(t);//free the first malloc
        fprintf(stderr,"malloc error!!");
        return NULL;
	}
	
    memset(pnode,0,sizeof(Symbol));
    strncpy(pnode->name, name, NAME_SIZE);
    pnode->type = type;
    
    if(type==FUNCTION){
    	
    	if(!strcmp("main",name)){
    		pnode->u.f.id = -10;
		}else if(!strcmp("read",name)){
			pnode->u.f.id = -11;
		}else if(!strcmp("write",name)){
			pnode->u.f.id = -12;
		}else{
			pnode->u.f.id = func_seq++;
		}
		
		INIT_LIST_HEAD(&pnode->u.f.auto_head);
		INIT_LIST_HEAD(&pnode->u.f.param_head);
	}else{
		
	}
	
	t->value = pnode;
    
    return pnode;
}

void insert_local_var(ListHead *new, AutoOffset *a, ListHead *head)
{
	AutoInfo *auto_var = NULL;
	auto_var = list_entry(new,AutoInfo,auto_list);
	
	auto_var->seq = a->local_seq;
	auto_var->block_no = a->current_block;
	auto_var->level = a->current_level;
	auto_var->offset = STACK_BASE + a->local_total_offset;
	
	a->local_total_offset += auto_var->size;
	a->local_seq++;
	
	list_add_tail(head,new);
}

AutoInfo* lookup_local_var(char *name)
{
    ListHead *temp = NULL;
    AutoInfo * p   = NULL;
    int max = -1;

    list_for_each(temp, &current_function->u.f.auto_head)
    {
        p = list_entry(temp, AutoInfo, auto_list);
        if( !strcmp(p->name,name) )
        {
            if(p->block_no == 0||(p->block_no<=offset.current_block
			&&p->block_no>offset.current_level-offset.current_block))
            {
                if(p->block_no > max)
                {   //foreach list to choose max block_no
                    max = p->block_no;
                }
            }
        }
    }
    list_for_each(temp, &current_function->u.f.auto_head)
    {
        p = list_entry(temp, AutoInfo, auto_list);
        if(!strcmp(p->name,name)&&p->block_no==max)
        {
            return p;
        }
    }
    return NULL;
}

void insert_param(ListHead *new,int seq,ListHead *head)
{
	ParamInfo *_param = NULL;
    _param = list_entry(new, ParamInfo, param_list);
    _param->seq = seq;
    _param->offset = STACK_BASE + seq * _param->size;
    current_function->u.f.param_size += _param->size;
    list_add_tail(head,new);
}

ParamInfo* lookup_param(char *name)
{
	ListHead *temp = NULL;
	ParamInfo *p = NULL;

	list_for_each(temp, &current_function->u.f.param_head)
	{
	    if(!temp)
            return NULL;
	    p = list_entry(temp, ParamInfo, param_list);
	    if(!p)
	        return NULL;
	    if(!strcmp(p->name, name)){
	        return p;
	    }
	}
    return NULL;
}





/*
void list_add_tail(int type, void *head, void *elem)
{
    ParamInfo *head1, *elem1;
    AutoInfo  *head2, *elem2;
    void *p;
    if(type == PARAM)
    {
        head1 = (ParamInfo*)head;
        elem1 = (ParamInfo*)elem;
        while(head1 != NULL)
        {
            head1 = head1->next;
        }
        head1 = elem1;
    }
    if(type == AVAR)
    {
        head2 = (AutoInfo*)head;
        elem2 = (AutoInfo*)elem;
        while(head2 != NULL)
        {
            head2 = head2->next;
        }
        head2 = elem2;
    }
    else
    {
        fprintf(stderr,"wrong type in add_list_tail");
    }
}
*/

/*
struct func_info* insert_global_func(char * name)
{
    assert(name != NULL);

    if(lookup_global_func(name))
    {
        fprintf(stderr,"line %d:func already exist\n", save_line);
        return NULL;
    }
    int key = hash(name);
    if(!table[key].exist)
    {
        goto insert;
    }
    while(!table[++key].exist)
    {
        if(key > HASH_SIZE)
        {
            fprintf(stderr,"line %d:func table full\n", save_line);
                return NULL;
        }
        goto insert;
    }

insert:
    strcpy(table[key].name, name);
    table[key].type = FUN;
    return &table[key].u.f;
}*/


/*
extern int	current_block = 0;
extern int	current_level = 0;
extern int local_seq = 0;
extern int local_total_offset = 0;
extern int auto_size = 0;
extern int global_seq = 0;
extern int global_total_offset = 0;
extern FuncInfo   *current_function;
extern GlobalInfo *current_global_var;
extern AutoInfo   *current_local_var;
*/

