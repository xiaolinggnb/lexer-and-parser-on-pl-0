#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
//	Constant declarations.
#define MAX_SIZE 100 + 1

//	Internal representation of PL\0 Tokens
typedef enum{
	START, INT, WORD, LESS, GT, COLON /*分号*/,INLINE_COMMENT, BLOCK_COMMENT
}state;
//	I/O file names.
#define INPUT_FILE "input1"


char* initialize();	//获取代码段
char*clean(char*code);	//清除注释段
void FSM(char*code);
int main(){
	char*code = initialize();
	char*cleaned = clean(code);
	FSM(cleaned);
	free(cleaned);
	return 0;

}

char* clean(char*code){
/*  先alloc跟code一样的尺寸到tmp
	copy code -> tmp
	然后free code
	然后realloc tmp
	然后返回tmp
*/
	int code_len = strlen(code);
	char*cleaned = (char*)calloc(code_len,sizeof(char));
	int code_id = 0, cleaned_id = 0;
	state cur_state = START;
	char ch;
	while(1){
		ch = code[code_id];
		if(ch == '\0')break;
		switch(cur_state){
		case START:{
			if(ch == '{'){
				cur_state = INLINE_COMMENT;
			}
			else if(ch == '/' && code_id + 1 < code_len && code[code_id + 1] == '*'){
				cur_state = BLOCK_COMMENT;
				code_id++;
			}
			else{
				cleaned[cleaned_id++] = code[code_id];
			}
			code_id++;
			break;
		}
		case INLINE_COMMENT:{
			if(ch == '}'){
				cur_state = START;
			}
			code_id++;
			break;
		}
		case BLOCK_COMMENT:{
			if(ch == '*' && code_id + 1 < code_len && code[code_id + 1] == '/'){
				cur_state = START;
				code_id++;
			}
			code_id++;
			break;
		}
		default:
			cur_state = START;
		}
	}
	free(code);
	cleaned = (char*)realloc(cleaned,strlen(cleaned));
	return cleaned;
}
void FSM(char*code){
	state cur_state = START;
	int index = 0;
	int integer = 0;
	char ch;
	char*word = (char*)calloc(MAX_SIZE,sizeof(char));
	int word_index = 0;
	char buffer;
	while(1){
		ch = code[index++];
		if(ch == '\0')break;
		switch(cur_state){
		case START:{
			if(isdigit(ch)){
				integer = ch - '0';
				cur_state = INT;
			}
			else if(isalpha(ch)){
				memset(word,'\0',MAX_SIZE);	
				word_index = 0;
				word[word_index++] = ch;				
				cur_state = WORD;
			}
			else{
				switch(ch){
				case '+':
				case '-':
				case '/':
				case '*':
				case '#':
				case ';':
				case '.':
				case ',':
					printf("%c\n",ch);
					break;
				case '<':
					cur_state = LESS;
					break;
				case '>':
					cur_state = GT;
					break;
				case ':':
					cur_state = COLON;
					break;
				default:
					break;
				}
			}
			break;
		}
		case INT:{
			if(isdigit(ch)){
				integer = integer * 10 + ch - '0';
				cur_state = INT;
			}
			else{
				printf("%d\n",integer);
				integer = 0;
				cur_state = START;
				index--;
			}
			break;
		}
		case WORD:{
			if(isdigit(ch) || isalpha(ch)){
				word[word_index++] = ch;
				cur_state = WORD;
			}
			else{
				printf("%s\n",word);
				cur_state = START;
				index--;
			}
			break;
		}
		case LESS:{
			if(ch == '='){
				printf("<=\n");
			}
			else{
				index--;
			}
			cur_state = START;
			
			break;
		}
		case GT:{
			if(ch == '='){
				printf(">=\n");
			}
			else index--;
			cur_state = START;
			break;
		}		
		case COLON:{
			if(ch == '='){
				printf(":=\n");
			}
			else index--;
			cur_state = START;
			break;
		}
		default:
			cur_state = START;
		}
	}
	free(word);
	return;
}
char*initialize(){
	FILE* fp;
	if((fp= fopen(INPUT_FILE,"r")) == NULL){
		printf("fopen() failed.\n");
		exit(-1);
	}
	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	rewind(fp);
	char *code = (char*)calloc(size,sizeof(char));
	char ch;
	int index = 0;
	while((ch = fgetc(fp)) != EOF){
		code[index++] = ch;
	}
	fclose(fp);
	return code;
}

