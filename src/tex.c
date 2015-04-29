#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lily.h"

#define MAX_WORD_LEN 20
#define MAX_INPUT_LEN 1000 
#define RESEVER_WORD_NUM 11

char * resever_word[] = 
{
 "int",   "char", "void", "pint", "pchar", 
 "pvoid", "if",   "else", "for",  "while", 
 "return","read","write",
};		


char save_word[100];
char virtual_save_word[100];
int save_num;
int save_line;

static char input_line[105];
static int input_len;
static int pos;
static int vpos;

//int inovke_once = true;
int first_invoke = true;

static int is_resever_word(char * word)
{
	int i;
	for(i = 0; i < RESEVER_WORD_NUM; i++){
		if(!strcmp(resever_word[i], word))
				return i+1;
	}
	return -1; 
}

static int get_next_word(int *p)
{
    if( *p >= input_len )
	{
        if(fgets(input_line,MAX_INPUT_LEN,stdin))
		{	
            *p = 0;
			save_line++;
            input_len = strlen(input_line);
            if(input_len > MAX_INPUT_LEN){
				fprintf( stderr, "input_line out of max range\n" );
				exit( 0 ); 
			}
            return input_line[(*p)++];
		}else{
			return FINISH;
		}
	}else{
        return input_line[(*p)++];
    }
}


int get_token()
{
    int c, len, i, max_word_len, state, token, flag, pause_line, pause_block;

    state = START;
    pause_line = 0;
    pause_block = 0;

    while( state != OVER  )
    {
        if( state == PAUSE )
        {
            if( pause_line )
            {
                pause_line = 0;
                pos = input_len;
                state = START;
            }
            if( pause_block )
            {
                while( (c = get_next_word(&pos)) != FINISH )
                {
                    if( c == '*' && input_line[pos] == '/' )
                    {
                        input_line[pos++];
                        break;
                    }
                }
                if( c == FINISH )
                    return FINISH;
                pause_block = 0;
                state = START;
            }
        }
        c = get_next_word(&pos);
        if( c == FINISH )
            return FINISH;
        if( c == ' ' || c == '\t' || c == '\n' )
            continue;
        if( isalpha( c ) || c == '_' )
        {
            i = 0;
            max_word_len = 0;
            do
            {
                save_word[i++] = c;
                c = input_line[pos++];
                max_word_len++;
            }while( isdigit( c ) || c == '_' || isalpha( c ) );
            pos--;
            if( max_word_len >= 20 )
            {
                fprintf( stderr, "word's max len is 20"); // do a error deal function
                exit( 0 );
            }
            save_word[i] = '\0';
            token = TOK_ID;
            state = OVER;
        }
        else if( isdigit( c ) )
        {
            i = 0;
            if( flag == '-' )
                save_word[i++] = '-';
            while( isdigit( c ) )
            {
                save_word[i++] = c;
                c = input_line[pos++];
            }
            pos--;
            save_word[i] = '\0';
            save_num = atoi( save_word );
            token = TOK_NUM;
            state = OVER;
            flag = 0;
        }
        else
        {
            state = OVER;
            switch( c )
            {
                case '+':
                    token = TOK_PLUS;
                    break;
                case '-':
                    token = TOK_MINUS;
                    break;
                case '*':
                    token = TOK_MUL;
                    break;
                case '/':
                    if( input_line[pos] == '/' )
                    {
                        input_line[pos++];
                        state = PAUSE;
                        pause_line = 1;
                    }
                    else if( input_line[pos] == '*' )
                    {
                        input_line[pos++];
                        state = PAUSE;
                        pause_block = 1;
                    }
                    else
                    {
                        token = TOK_DIV;
                    }
                    break;
                case '=':
                    if( input_line[pos] == '=' )
                    {
                        input_line[pos++];
                        token = TOK_EQ;
                    }
                    else
                        token = TOK_ASSIGN;
                    break;
                case '>':
                    if( input_line[pos] == '=' )
                    {
                        input_line[pos++];
                        token = TOK_GE;
                    }
                    else
                        token = TOK_GT;
                    break;
                case '<':
                    if( input_line[pos] == '=' )
                    {
                        input_line[pos++];
                        token = TOK_LE;
                    }
                    else
                        token = TOK_LT;
                    break;
                case '!':
                    if( input_line[pos] == '=' )
                    {
                        input_line[pos++];
                        token = TOK_NE;
                    }
                    else
                        token = TOK_NOT;
                    break;
                case '&':
                    if( input_line[pos] == '&' )
                    {
                        input_line[pos++];
                        token = TOK_ANDAND;
                    }
                    else
                        token = TOK_AND;
                    break;
                case '|':
                    if( input_line[pos] == '|' )
                    {
                        input_line[pos++];
                        token = TOK_OROR;
                    }
                    else
                        token = TOK_OR;
                    break;
                case ',':
                    token = TOK_COMMA;
                    break;
                case ':'://no gonna use 'condition ?  true : false'
                    token = TOK_COLON;
                    break;
                case ';':
                    token = TOK_SEMI;
                    break;
                case 39:	//  '
                    token = TOK_SQUTOA;
                    break;
                case 34:	//  "
                    token = TOK_DQUTOA;
                    break;
                case '(':
                    token = TOK_LPAREN;
                    break;
                case ')':
                    token = TOK_RPAREN;
                    break;
                case '[':
                    token = TOK_LSQUARE;
                    break;
                case ']':
                    token = TOK_RSQUARE;
                    break;
                case '{':
                    token = TOK_LBRACKET;
                    break;
                case '}':
                    token = TOK_RBRACKET;
                    break;
            }
        }
    }
    if( token == TOK_ID )
    {
        int t;
        if( (t = is_resever_word(save_word)) != -1 )
            token = t;
    }
    return token;
}


int virtual_get_token(int clear)
{
    int c, len, i, max_word_len, state, token, flag, pause_line, pause_block;

    state = START;
    pause_line = 0;
    pause_block = 0;
    
    if(clear){
    	vpos = pos;
    	first_invoke = true;
    }
    
    if(first_invoke){
    	vpos = pos;
    	first_invoke = false;
    }
    
    if(state == OVER){
    	return TOK_EOF;
    }

    while(state != OVER)
    {
        if(state == PAUSE)
        {
            if(pause_line)
            {
                pause_line = 0;
                vpos = input_len;
                state = START;
            }
            if( pause_block )
            {
                while( (c = get_next_word(&vpos)) != FINISH )
                {
                    if( c == '*' && input_line[vpos] == '/' )
                    {
                        input_line[vpos++];
                        break;
                    }
                }
                if( c == FINISH )
                    return FINISH;
                pause_block = 0;
                state = START;
            }
        }
        c = get_next_word(&vpos);
        if( c == FINISH )
            return FINISH;
        if( c == ' ' || c == '\t' || c == '\n' )
            continue;
        if( isalpha( c ) || c == '_' )
        {
            i = 0;
            max_word_len = 0;
            do
            {
                virtual_save_word[i++] = c;
                c = input_line[vpos++];
                max_word_len++;
            }while( isdigit( c ) || c == '_' || isalpha( c ) );
            vpos--;
            if( max_word_len >= 20 )
            {
                fprintf( stderr, "word's max len is 20"); //do a error deal function
                exit( 0 );
            }
            virtual_save_word[i] = '\0';
            token = TOK_ID;
            state = OVER;
        }
        else if( isdigit( c ) )
        {
            i = 0;
            if( flag == '-' )
                virtual_save_word[i++] = '-';
            while( isdigit( c ) )
            {
                virtual_save_word[i++] = c;
                c = input_line[vpos++];
            }
            vpos--;
            virtual_save_word[i] = '\0';
            save_num = atoi( virtual_save_word );
            token = TOK_NUM;
            state = OVER;
            flag = 0;
        }
        else
        {
            state = OVER;
            switch( c )
            {
                case '+':
                    token = TOK_PLUS;
                    break;
                case '-':
                    token = TOK_MINUS;
                    break;
                case '*':
                    token = TOK_MUL;
                    break;
                case '/':
                    if( input_line[vpos] == '/' )
                    {
                        input_line[vpos++];
                        state = PAUSE;
                        pause_line = 1;
                    }
                    else if( input_line[vpos] == '*' )
                    {
                        input_line[vpos++];
                        state = PAUSE;
                        pause_block = 1;
                    }
                    else
                    {
                        token = TOK_DIV;
                    }
                    break;
                case '=':
                    if( input_line[vpos] == '=' )
                    {
                        input_line[vpos++];
                        token = TOK_EQ;
                    }
                    else
                        token = TOK_ASSIGN;
                    break;
                case '>':
                    if( input_line[vpos] == '=' )
                    {
                        input_line[vpos++];
                        token = TOK_GE;
                    }
                    else
                        token = TOK_GT;
                    break;
                case '<':
                    if( input_line[vpos] == '=' )
                    {
                        input_line[vpos++];
                        token = TOK_LE;
                    }
                    else
                        token = TOK_LT;
                    break;
                case '!':
                    if( input_line[vpos] == '=' )
                    {
                        input_line[vpos++];
                        token = TOK_NE;
                    }
                    else
                        token = TOK_NOT;
                    break;
                case '&':
                    if( input_line[vpos] == '&' )
                    {
                        input_line[vpos++];
                        token = TOK_ANDAND;
                    }
                    else
                        token = TOK_AND;
                    break;
                case '|':
                    if( input_line[vpos] == '|' )
                    {
                        input_line[vpos++];
                        token = TOK_OROR;
                    }
                    else
                        token = TOK_OR;
                    break;
                case ',':
                    token = TOK_COMMA;
                    break;
                case ':'://no gonna use 'condition ?  true : false'
                    token = TOK_COLON;
                    break;
                case ';':
                    token = TOK_SEMI;
                    break;
                case 39:	//  '
                    token = TOK_SQUTOA;
                    break;
                case 34:	//  "
                    token = TOK_DQUTOA;
                    break;
                case '(':
                    token = TOK_LPAREN;
                    break;
                case ')':
                    token = TOK_RPAREN;
                    break;
                case '[':
                    token = TOK_LSQUARE;
                    break;
                case ']':
                    token = TOK_RSQUARE;
                    break;
                case '{':
                    token = TOK_LBRACKET;
                    break;
                case '}':
                    token = TOK_RBRACKET;
                    break;
            }
        }
    }
    if( token == TOK_ID )
    {
        int t;
        if( (t = is_resever_word(virtual_save_word)) != -1 )
            token = t;
    }
    return token;
}

