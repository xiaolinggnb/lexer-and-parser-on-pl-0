#include"lexer.c"
int main(){
	char*code = initialize();
	char*cleaned = clean(code);
	token*tokens = FSM(cleaned);
	tokens_print(tokens);
	return 0;
}
