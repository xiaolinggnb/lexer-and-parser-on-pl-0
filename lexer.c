#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
//	Constant declarations.
#define MAX_SIZE 11 + 1
//  Internal representation of PL\0 Tokens
typedef enum Tokens{
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4,
    minussym = 5, multsym = 6,  slashsym = 7, oddsym = 8,
    eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
    commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
    beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
    whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
    varsym = 29, procsym = 30, writesym = 31, readsym = 32,
    elsesym = 33
}token_type;

typedef enum StateLabels{
	START, INT, WORD, LESS, GT, COLON /*分号*/,INLINE_COMMENT, BLOCK_COMMENT
}state;
//  Token-Struct definition
typedef struct tokenStruct{
    
    int class;
    char lexeme[MAX_SIZE];
    struct tokenStruct* next;
    
}token;

//  List of reserved word names.
char *word[] = {
    "null", "begin", "call", "const", "do", "else", "end",
    "if", "odd", "procedure","read", "then", "var", "while", "write"
};


//	I/O file names.
#define INPUT_FILE "input2"


char* initialize();	//获取代码段
char* clean(char*code);	//清除注释段
token* FSM(char*code);
void tokens_print(token*);
token*Token(int, char*);
_Bool isInWord(char*);
char*itoa(int);
token*lexer();
//int main(){
//	char*code = initialize();
//	char*cleaned = clean(code);
//	token*tokens = FSM(cleaned);
//	tokens_print(tokens);
//	return 0;
//
//}
token*lexer(){
	char*code = initialize();
	char*cleaned = clean(code);
	return FSM(cleaned);
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
_Bool isInWord(char*ch){
	for(int i = 0;i < 15;i++){	//15个保留字在word中
		if(!strcmp(ch,word[i]))
			return 1;
	}
	return 0;
}
void tokens_print(token*tokens){
	token*iter = tokens->next;
	while(iter){
		printf("(class:%3d, lexeme:\"%s\")\n",iter->class,iter->lexeme);
		iter = iter->next;
	}
	printf("\n");

}
token*Token(int class,char*lex){
	token*t = (token*)calloc(1,sizeof(token));
	t->class = class;
	strcpy(t->lexeme,lex);
	t->next = NULL;
	return t;
}
char*itoa(int integer){
	char*lex = (char*)calloc(MAX_SIZE,sizeof(char));
	int size = 0;
	int tmp = integer;
	while(tmp){
		tmp /= 10;
		size++;
	}
	tmp = integer;
	int digit = 0;
	for(int i = size-1;i >= 0;i--){
		digit = integer % 10;
		lex[i] = digit + '0';
		integer /= 10;
	}
	return lex;
}
token* FSM(char*code){
	state cur_state = START;
	int index = 0;
	int integer = 0;
	char ch;
	char*word = (char*)calloc(MAX_SIZE,sizeof(char));
	int word_index = 0;
	char buffer;
	token*head = (token*)calloc(1,sizeof(token));
	token*iter = head;
	int class;
	char lexeme[MAX_SIZE] = {};
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
					class = plussym; 
					printf("%c\n",ch);
					strcpy(lexeme,"+");
					iter->next = Token(class,lexeme);
					iter = iter->next;

					break;
				case '-':
					class = minussym;
					printf("%c\n",ch);
					strcpy(lexeme,"-");
					iter->next = Token(class,lexeme);
					iter = iter->next;

					break;
				case '/':
					class = slashsym;
					printf("%c\n",ch);
					strcpy(lexeme,"/");
					iter->next = Token(class,lexeme);
					iter = iter->next;

					break;
				case '*':
					class = multsym;
					printf("%c\n",ch);
					strcpy(lexeme,"*");
					iter->next = Token(class,lexeme);
					iter = iter->next;

					break;
				case '#':
					class = neqsym;
					printf("%c\n",ch);
					strcpy(lexeme,"#");
					iter->next = Token(class,lexeme);
					iter = iter->next;

					break;
				case ';':
					class = semicolonsym;
					printf("%c\n",ch);
					strcpy(lexeme,";");
					iter->next = Token(class,lexeme);
					iter = iter->next;

					break;
				case '.':
					class = periodsym;
					printf("%c\n",ch);
					strcpy(lexeme,".");
					iter->next = Token(class,lexeme);
					iter = iter->next;

					break;
				case ',':
					class = commasym;
					printf("%c\n",ch);
					strcpy(lexeme,",");
					iter->next = Token(class,lexeme);
					iter = iter->next;
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
				class = numbersym;
				strcpy(lexeme,itoa(integer));
				iter->next = Token(class,lexeme);
				iter = iter->next;
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
				if(isInWord(word)){
					if(!strcmp(word,"null"))
						class = nulsym;
					else if(!strcmp(word,"begin"))
						class = beginsym;
					else if(!strcmp(word,"call"))
						class = callsym;
					else if(!strcmp(word,"const"))
						class = constsym;
					else if(!strcmp(word,"do"))
						class = dosym;
					else if(!strcmp(word,"else"))
						class = elsesym;
					else if(!strcmp(word,"end"))
						class = endsym;
					else if(!strcmp(word,"if"))
						class = ifsym;
					else if(!strcmp(word,"odd"))
						class = oddsym;
					else if(!strcmp(word,"procedure"))
						class = procsym;
					else if(!strcmp(word,"read"))
						class = readsym;
					else if(!strcmp(word,"then"))
						class = thensym;
					else if(!strcmp(word,"var")){
						class = varsym;
					}
					else if(!strcmp(word,"while"))
						class = whilesym;
					else if(!strcmp(word,"write"))
						class = writesym;
					strcpy(lexeme,word);
					iter->next = Token(class,lexeme);
					iter = iter->next;
				}
				else{
					class = identsym;
					strcpy(lexeme,word);
					iter->next = Token(class,lexeme);
					iter = iter->next;

				}
				

			}
			break;
		}
		case LESS:{
			if(ch == '='){
				printf("<=\n");
				strcpy(lexeme,"<=");
				class = leqsym;
				iter->next = Token(class,lexeme);
				iter = iter->next;
			}
			else{
				strcpy(lexeme,"<");
				class = lessym;
				iter->next = Token(class,lexeme);
				iter = iter->next;
				index--;
			}
			cur_state = START;
			
			break;
		}
		case GT:{
			if(ch == '='){
				printf(">=\n");
				strcpy(lexeme,">=");
				class = geqsym;
				iter->next = Token(class,lexeme);
				iter = iter->next;
			}
			else{
				strcpy(lexeme,">");
				class = gtrsym;
				iter->next = Token(class,lexeme);
				iter = iter->next;
				index--;
			}
			cur_state = START;
			break;
		}		
		case COLON:{
			if(ch == '='){
				strcpy(lexeme,":=");
				class = becomessym;
				iter->next = Token(class,lexeme);
				iter = iter->next;

				printf(":=\n");
			}
			else{
				index--;
				strcpy(lexeme,":");
				class = semicolonsym;
				iter->next = Token(class,lexeme);
				iter = iter->next;
			}
			cur_state = START;
			break;
		}
		default:
			cur_state = START;
		}
	}
	free(word);
	return head;
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

