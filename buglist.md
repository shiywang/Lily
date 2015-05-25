目前存在的bug
==========================
2.目前使用pint pchar 来代替int * char *  ,另外还不支持二级指针，等多级指针


####目录结构：
 
    |-/Test             单元测试               
    |-makefile          makefile文件
    |-grammer.md        文法定义
    |-src/              源代码文
         |-retab.py     用于文件格式的批处理脚本
         |-error.c      lily的错误处理实现
         |-gen.c        lily的代码生成文件
         |-gen.h        gen.c的头文件
         |-getopt.h     getopt的头文件     
         |-getopt.c     Linux/Unix的getopt函数的跨平台实现
         |-lily.c       主函数模块文件
         |-lily.h       存放主要的全局数据结构和宏定义
         |-list.h       Linux内核双向链表操作宏
         |-parse.c      语法制导分析模块
         |-parse.h      语法制导分析的头文件
         |-symbol.c     符号表的操作函数
         |-test.h       单元测试头文件
         |-tex.c        词法分析文件
         |-version.h    软件版本定义
         |-vm.c         虚拟机程序
'''         
####函数执行周期：
init_hash  
init_sym_table       
malloc_struct_code  
get_token
program    
process_struct_code  
init_vm            
start_vm

'''


