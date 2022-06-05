#include<stdio.h>
#include<stdlib.h>
#include "lexer.c"
//#define MAX_SIZE 11+1
//typedef enum Tokens{
//    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4,
//    minussym = 5, multsym = 6,  slashsym = 7, oddsym = 8,
//    eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
//    gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
//    commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
//    beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
//    whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
//    varsym = 29, procsym = 30, writesym = 31, readsym = 32,
//    elsesym = 33
//}token_type;
//
//
//typedef struct token{
//	token_type type;
//	char lexeme[MAX_SIZE];
//	struct token*next;
//}token;
//
//
//char *word[] = {
//    "null", "begin", "call", "const", "do", "else", "end",
//    "if", "odd", "procedure","read", "then", "var", "while", "write"
//};

//synchronized symbol
int sym = -1;	//if sym == 0, not OK end the program
token*iter;

void parser();
void block();
void varstm();
void error(char*);
void machine();
void sniff_machine();
void conststm();
void procstm();
void condition();
void expression();
void term();
void factor();
void statement();
void error_lexeme();
int main(){
	iter = lexer();
	tokens_print(iter);
	parser();
}


void error(char*s){
	printf("%s\n,",s);
}
void error_lexeme(){
	printf("error happens in \"%s\", the next token is \"%s\"\n",iter->lexeme,iter->next?iter->next->lexeme:0);
}
void parser(){
	block();
	machine();
	if(sym == periodsym)
		return;
	else{ 
		error("parser()");
		error_lexeme();
	}
}
void machine(){
	iter = iter->next;
	if(!iter){
		error("program illegal break, some part code is missed.");
		exit(1);
	}
	else
		sym = iter->class;
}
void sniff_machine(){
	if(!iter->next)
		sym = nulsym;
	else
		sym = iter->next->class;
}

void block(){
	sniff_machine();

	if(sym == constsym){
		machine();
		conststm();
		sniff_machine();
	}

	if(sym == varsym){
		machine();
		varstm();
		sniff_machine();
	}

	if(sym == procsym){
		do{
			procstm();
			sniff_machine();
		}while(sym == procsym);
	}
	
	statement();

}

void conststm(){
	machine();
	if(sym == identsym){
		machine();
		if(sym == eqlsym){
			machine();
			if(sym == numbersym){
				machine();
				if(sym == commasym)
					conststm();
				else if(sym == semicolonsym)
					return;
				else{
					error("c");
					error_lexeme();
				}
			}
			else {error("c");error_lexeme();}

		}
		else {error("c");error_lexeme();}

	}
	else {error("conststm");		error_lexeme();}

}

void varstm(){
	machine();
	if(sym == identsym){
		machine();
		if(sym == commasym)
			varstm();
		else if(sym == semicolonsym){
			return;
		}
		else error("v");
	}
	else error("v");
}

void procstm(){
	machine();
	if(sym == procsym){
		machine();
		if(sym == identsym){
			machine();
			if(sym == semicolonsym){
				block();
				machine();
				if(sym == semicolonsym){
					return;
				}
				else error("p");
			}
			else error("p");
		}
		else error("p");
	}
	else error("p");
}



void statement(){
	machine();
	if(sym == identsym){
		machine();
		if(sym == becomessym)
			expression();
	}
	else if(sym == callsym){
		machine();
		if(sym == identsym)
			return;
	}
	else if(sym == beginsym){
		statement();
		machine();
		if(sym == semicolonsym){
			sniff_machine();
			while(sym == identsym || sym == callsym || sym == beginsym || sym == ifsym || sym == whilesym){
				statement();
				machine();
				if(sym != semicolonsym){
					error("s");
				error_lexeme();

				}
			}
			machine();
			if(sym == endsym)
				return;
			else
			{error("s");
			error_lexeme();
			}
		}
		else {error("s");error_lexeme();}

	}
	else if(sym == ifsym){
		machine();
		condition();

		machine();
		if(sym == thensym)
			statement();
	}
	else if(sym == whilesym){
		machine();
		condition();

		machine();
		if(sym == dosym)
			statement();
	}
	else error("s");
}

void condition(){
	machine();
	if(sym == oddsym)
		expression();
	else{
		expression();
		machine();
//		if(sym == eqlsym || neqsym || lessym || grtsym || leqsym || gtesym):
//			expression()
		switch(sym){
		case eqlsym:
		case neqsym:
		case lessym:
		case gtrsym:
		case leqsym:
		case geqsym:
			expression();
			break;
		default:
			break;
		}
	}
}

void expression(){
	sniff_machine();
	if(sym == plussym || sym == minussym){
		machine();
		term();
		sniff_machine();
		while(sym == plussym || sym == minussym){
			machine();
			term();
		}
		return;
	}
	else if(sym == identsym || sym == numbersym || sym == lparentsym){
		term();
		sniff_machine();
		while(sym == plussym || sym == minussym){
			machine();
			term();
		}
		return;
	}
	else error("e");
}
void term(){
	factor();
	sniff_machine();
	while(sym == multsym || sym == slashsym){
		factor();
		sniff_machine();
	}
}

void factor(){
	machine();
	if(sym == identsym || sym == numbersym)
		return;
	if(sym == lparentsym){
		expression();
		machine();
		if(sym == rparentsym){
			return;
		}
		else error("f");
	}
}
