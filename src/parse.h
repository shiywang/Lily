#ifndef PARSE_H
#define PARSE_H
#include "lily.h"


//extern 
int token;
AutoOffset offset;
int global_seq = 0;
int global_total_offset = 0;

Symbol* current_function;
Symbol* current_global_var;
AutoInfo* current_local_var;

int has_return_value;
int before_assign = false;
int is_array;
int var_in_square = false;
int var_in_read_write = false;
//int has_star_prefix;
//int has_adderess_prefix;

#endif
