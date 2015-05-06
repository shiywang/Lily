#include <stdio.h>
#include "lily.h"
#include "test.h"
#include "getopt.h"
#include "version.h"


extern int save_line;
extern void program();
extern int hash_init();
extern void sym_table_init();
extern void malloc_struct_code();
extern void print_struct_code();
extern void process_struct_code();
extern void init_vm();
extern void start_vm();


int current_line;
FILE *input_stream;
FILE *output_stream;
int gen_op_code = false;


void compile()
{
    hash_init();
    sym_table_init();
    malloc_struct_code();
    program();
}

//print_struct_code(); 

void str_find_replace(char *str, char find, char c)
{
    char *t = str;
    while(*t != find && *t != '\0')
        t++;
    if(*t != '\0')
        *t = c;
}

int file_name_vaild(char *str, char last)
{
    char *t = str;
    while(*t != last && *t != '\0')
        t++;
    if(*(t-1) == '.' && *t  == last
       && *t  == '\0') {
        return true;
    }else{
        return false;
    }
}

char* find_source_file(int argc, char *argv[], char *file)
{
    int i;
    char *c;
    for(i = 0; i < argc; i++){
        if(c = strstr(argv[i],".c")){
            if(*(c+2) == '\0'){
                strcpy(file,argv[i]);
                strcpy(argv[i],"-x");
                return file;
            }
        }
    }
    return NULL;
}


int main(int argc, char *argv[])
{
    int c;
    int is_default_name = true;
    int has_obj_name = false;
    char fn_in[100];
    char fn_out[100];
    char temp_name[100];
    
    if(!find_source_file(argc,argv,fn_in)){
        fprintf(stderr,"no input file\n");
    }
    
    while((c=getopt(argc,argv,"xVvHhS:s:o:")) != -1){
        
        switch (c) {
            case 'S':
            case 's':
                strcpy(temp_name,fn_in);
                str_find_replace(temp_name,'.','\0');
                strcat(temp_name,".s");
                strcpy(fn_out,temp_name);
                gen_op_code = true;
                goto end;
                break;
            case 'x':
                break;
            case 'o':
                if(!file_name_vaild(optarg,'o')){
                    fprintf(stderr,"input file name is invaild\n");
                    return 0;
                }
                strcpy(fn_out,optarg);
                is_default_name = false;
                break;
            case 'V':
            case 'v':
                version();             
                return 0;
                 break;
            case 'H':
            case 'h':
                help();
                break;
            default:
                break;
        }
    }
    
end:
    input_stream = fopen(fn_in,"r+");
    
    if(gen_op_code){
        output_stream = fopen(fn_out,"w+");
        compile();
    }else{
        if(is_default_name){
            str_find_replace(fn_in,'.','\0');    
            strcat(fn_in,".out");
            //use fn_in to write file
            output_stream = fopen(fn_in,"w+");
            
        }else{
            output_stream = fopen(fn_out,"w+");
        }
        compile();
        process_struct_code();
        init_vm();
        start_vm();
        
    }
    
    return 0;
}




