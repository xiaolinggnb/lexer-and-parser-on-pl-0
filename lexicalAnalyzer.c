//
//  lexicalAnalyzer.c
//  Lexical Analyzer
//
//  UCF - COP 3402: Systems Software
//  Summer 2015
//
//  Team Members:
//  Justin Mackenzie
//  Alan Yepez
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//  Constant declarations.
#define MAX_SIZE 11 + 1
#define MAX_NUMS 5 + 1
#define MAX_SYMS 2 + 1

//  I/O file names.
#define INPUT_FILE "input.txt"
#define CLEAN_OUTPUT_FILE "cleaninput.txt"
#define TABLE_OUTPUT_FILE "lexemetable.txt"
#define LIST_OUTPUT_FILE "lexemelist.txt"

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
    init, firstState, stringState, numberState, symbolState, beginState,
    cState, callState, constState, doState, eState, elseState, endState,
    ifState, oddState, procedureState, readState, thenState, wState,
    whileState, writeState, varState
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

//  Function declarations.
char* initialize();
char* clean(char *code);
token* createToken(token *head, int *stage, int *j);
token* tokenize(char * code);
int isLetter(char ch);
int isNumber(char ch);
int isSymbol(char ch);
int isValid(char ch);
int setSymbolClass(char ch);
void print(token* lexemes);

int main(int argc, const char * argv[]) {
    
    //  Method call to read input.
    char *code = initialize();
    
    //  Method to remove comments.
    char *cleaned = clean(code);
    
    //  Method call to parse tokens.
    token *lexemes = tokenize(cleaned);
    
    //  Print to output files.
    print(lexemes);
    
    //  Free allocated memory.
    free(cleaned);
    
    if ( !lexemes )
        
        free(lexemes);
    
    return 0;
    
}

//
//  Reads input file (PL/0 code) to string and return char pointer.
//
//  @return
//      char*, pointer to the first character in the String.
//
char* initialize( )
{
    
    char *head = NULL;
    
    //  Create pointer to input file.
    FILE* ifp = fopen(INPUT_FILE, "r");
    
    //  Return null if file could not be read.
    if (!ifp)
    {
        
        printf("Error: File not found.\n");
        
        exit(1);
        
    }
    
    //  Determine the length of the input file.
    fseek(ifp, 0, SEEK_END);
    int len = (int)ftell(ifp);
    fseek(ifp, 0, SEEK_SET);
    
    //  Dynamically size string.
    head = (char*)calloc(len + 1, sizeof(char));
    char *index = head;
    char c;
    
    //  Continue appending to String until end of character is reached.
    if ( head )
    {
        while ( (c = getc(ifp)) != EOF )
            
            *index++ = c;
        
        *index = '\0';
        
    }
    else
        
        printf("Error: Memory allocation failed.\n");
    
    //    printf("Input Code:\n%s\n\n",head);  //  bug printing
    
    return head;
    
}

//
//  Removes comments in code from input file.
//
//  @param *code
//      char, pointer to character array.
//  @return
//      char, pointer to the revised character array.
//
char* clean(char *code)
{
    
    //  Create and initalize temp char array.
    char *codeNoComments = (char*)calloc(strlen(code) + 1, sizeof(char));
    
    int i, j;
    
    //  Iterate through input string and only append content not in comments.
    for( i = 0, j = 0; i < strlen(code); i++ )
    {
        
        if( code[i] == '/' && code[i+1] == '*' )
        {
            
            i += 2;
            
            while( code[i] != '*' || code[i+1] != '/' )
            {
                
                i++;
                
            }
            
            i++;
            continue;
            
        }
        
        codeNoComments[j++] = code[i];
        
        
    }
    
    //  Free original input array.
    free(code);
    
    //  Print to output file.
    
    FILE * ofp = fopen(CLEAN_OUTPUT_FILE, "w");
    //    printf("Cleaned Code:\n%s\n\n",codeNoComments);  //  bug printing
    fprintf(ofp,"%s",codeNoComments);
    fclose(ofp);
    
    return codeNoComments;
    
}

//
//  Main logic function. Identifies tokens using states.
//
//  @param *code
//      char, pointer to character array.
//  @return token*
//      pointer to the head of the token datastructure.
//
token* tokenize(char * code)
{
    
    int i = 0, j;
    char ch;
    int state;
    
    //  Create head and 'this' node for iteration.
    token *head = createToken(NULL, &state, &j);
    token *this = head;
    
    //  Set initial state.
    state = firstState;
    
    //  Iterate throught the length of the code.
    while ( i < strlen(code) )
    {
        
        //  Save reference to current character.
        ch = code[i];
        
        //  Check for valid character.
        if ( !isValid(ch) )
        {
            
            if ( j != 0 )
            {
                
                this = createToken(head, &state, &j);
                
                i++;
                
                continue;
                
            }
            else
            {
                
                i++;
                
                continue;
                
            }
            
        }
        
        //  Switch statement for states.
        switch ( state ) {
                
                //  Case for every possible scenario.
            case firstState:
            {
                
                if ( ch == 'b' )
                {
                    this->class = identsym;
                    state = beginState;
                    
                }
                else if ( ch == 'c' )
                {
                    this->class = identsym;
                    state = cState;
                    
                }
                else if ( ch == 'd' )
                {
                    this->class = identsym;
                    state = doState;
                    
                }
                else if ( ch == 'e' )
                {
                    this->class = identsym;
                    state = eState;
                    
                }
                else if ( ch == 'i' )
                {
                    
                    this->class = identsym;
                    state = ifState;
                    
                }
                else if ( ch == 'o' )
                {
                    
                    this->class = identsym;
                    state = oddState;
                    
                }
                else if ( ch == 'p' )
                {
                    
                    this->class = identsym;
                    state = procedureState;
                    
                }
                else if ( ch == 'r' )
                {
                    
                    this->class = identsym;
                    state = readState;
                    
                }
                else if ( ch == 't' )
                {
                    
                    this->class = identsym;
                    state = thenState;
                    
                }
                else if ( ch == 'w' )
                {
                    
                    this->class = identsym;
                    state = writeState;
                    
                }
                else if ( ch == 'v' )
                {
                    
                    this->class = identsym;
                    state = varState;
                    
                }
                else if ( isLetter(ch) )
                {
                    
                    this->class = identsym;
                    state = stringState;
                    
                }
                else if ( isNumber(ch) )
                {
                    
                    this->class = numbersym;
                    state = numberState;
                    
                }
                else if ( isSymbol(ch) )
                {
                    
                    this->class = setSymbolClass(ch);
                    state = symbolState;
                    
                }
                else
                {
                    
                    printf("Error: This shouldn't happen...\n");
                    break;
                    
                }
                
                this->lexeme[j++] = ch;
                this->lexeme[j] = '\0';
                
                break;
                
            }
            case stringState:
            {
                
                if ( !isLetter(ch) && !isNumber(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                if ( j < MAX_SIZE - 1 )
                {
                    this->lexeme[j++] = ch;
                    this->lexeme[j] = '\0';
                    
                }
                
                else
                    
                    printf("Error: Exceeded maximum identifier size with '%c'.\n", ch);
                
                break;
                
            }
            case numberState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                if ( !isNumber(ch) )
                {
                    
                    printf("Error: Variable does not start with letter.\n");
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                if ( j < MAX_NUMS - 1 )
                {
                    this->lexeme[j++] = ch;
                    this->lexeme[j] = '\0';
                }
                else
                    
                    printf("Error: Exceeded maximum number size with '%c'.\n", ch);
                
                break;
                
            }
            case symbolState:
            {
                
                if ( !isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                else if ( j == 1 )
                {
                    
                    if ( this->class ==  lessym )
                    {
                        
                        if ( ch == '=' )
                        {
                            
                            this->class = leqsym;
                            
                        }
                        else if (ch == '>')
                        {
                            
                            this->class = geqsym;
                            
                        }
                        else
                        {
                            
                            printf("Error: Invalid symbol.\n");
                            
                            break;
                            
                        }
                        
                    }
                    else if ( this->class == gtrsym && ch == '=' )
                    {
                        
                        this->class = geqsym;
                        
                    }
                    else if ( *this->lexeme == ':' && ch == '=' )
                    {
                        
                        this->class = becomessym;
                        
                    }
                    else
                    {
                        
                        printf("Error: Invalid symbol.\n");
                        
                        break;
                        
                    }
                    
                }
                
                if ( j < MAX_SYMS - 1 )
                {
                    this->lexeme[j++] = ch;
                    this->lexeme[j] = '\0';
                }
                else
                    
                    printf("Error: Exceeded maximum symbol size with character '%c'.\n", ch);
                
                break;
                
            }
            case beginState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this->class = identsym;
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "begin";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        this->class = beginsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case cState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str1 = "call";
                char* str2 = "const";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                
                if ( ch == str1[j] )
                {
                    
                    state = callState;
                    
                }
                else if ( ch == str2[j] )
                {
                    
                    state = constState;
                    
                }
                else
                    
                    state = stringState;
                
                
                j++;
                
                break;
                
            }
            case callState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "call";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = callsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case constState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "const";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = constsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case doState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "do";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        this->class = dosym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case eState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str1 = "end";
                char* str2 = "else";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str1[j] )
                {
                    
                    state = endState;
                    
                }
                else if ( ch == str2[j] )
                {
                    
                    state = elseState;
                    
                }
                else
                    
                    state = stringState;
                
                
                j++;
                
                break;
                
            }
            case endState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "end";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = endsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case elseState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "else";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = elsesym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case ifState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "if";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = ifsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case oddState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "odd";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = oddsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case procedureState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "procedure";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = procsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case readState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "read";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = readsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case thenState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "then";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = thensym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case wState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str1 = "call";
                char* str2 = "const";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str1[j] )
                {
                    
                    state = callState;
                    
                }
                else if ( ch == str2[j] )
                {
                    
                    state = constState;
                    
                }
                else
                    
                    state = stringState;
                
                
                j++;
                
                break;
                
            }
                
            case whileState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "while";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = readsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case writeState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char* str = "while";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = readsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
            case varState:
            {
                
                if ( isSymbol(ch) )
                {
                    
                    this = createToken(head, &state, &j);
                    
                    continue;
                    
                }
                
                char *str = "var";
                
                this->lexeme[j] = ch;
                this->lexeme[j+1] = '\0';
                
                if ( ch == str[j] )
                {
                    
                    if ( strncmp(this->lexeme, str, strlen(str)) == 0 &&
                        !isNumber(code[i+1]) && !isLetter(code[i+1]) )
                    {
                        
                        this->class = varsym;
                        this = createToken(head, &state, &j);
                        
                        break;
                        
                    }
                    
                }
                else
                    
                    state = stringState;
                
                j++;
                
                break;
                
            }
                
        }
        
        //  Increment code index.
        i++;
        
    }
    
    //  Delete last token node if unused.
    if ( this->class == -1 )
    {
        token *temp = this;
        free(temp);
        this = NULL;
        
    }
    
    return head;
    
}

//
//  Determines whether char is a valid alpha character.
//
//  @param ch
//      char, character from input array.
//  @return
//      int, returns 1 if true, 0 otherwise.
//
int isLetter(char ch)
{
    
    return ( ch >= 'a' && ch <= 'z' );
    
}

//
//  Determines whether char is a valid numerical character.
//
//  @param ch
//      char, character from input array.
//  @return
//      int, returns 1 if true, 0 otherwise.
//
int isNumber(char ch)
{
    
    return ( ch >= '0' && ch <= '9' );
    
}

//
//  Determines whether char is a valid symbol character.
//
//  @param ch
//      char, character from input array.
//  @return
//      int, returns 1 if true, 0 otherwise.
//
int isSymbol (char ch)
{
    
    return ( ch >= ':' && ch <= '>' )  || ( ch >= '(' && ch <= '/' );
    
}

//
//  Determines whether char is either a alpha, numerical, or symbol character.
//
//  @param ch
//      char, character from input array.
//  @return
//      int, returns 1 if true, 0 otherwise.
//
int isValid(char ch)
{
    
    return ( isLetter(ch) || isNumber(ch) || isSymbol(ch) );
    
}

//
//  Allocates memory for a new token struct node.
//
//  @param *head
//      token, pointer to token structure.
//  @param len
//      int, starting size of the String.
//  @return
//      token*, pointer to the newly created struct node.
//
token* createToken(token *head, int *state, int *j)
{
    
    //  Change variables using pointer.
    *state = firstState;
    *j = 0;
    
    token *last = head;
    char empty[12] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
    
    //  Scenario for null head.
    if ( head == NULL )
    {
        
        last = (token*)malloc(sizeof(token));
        last->next = NULL;
        last->class = -1;
        strncpy(last->lexeme, empty, sizeof(sizeof(last->lexeme)));
        
    }
    else
    {
        
        //  Iterate to what is pointed to by last node.
        while (last->next != NULL)
            
            last = last->next;
        
        //  Allocate memory for new token node.
        last->next = (token*)malloc(sizeof(token));
        last = last->next;
        last->next = NULL;
        last->class = -1;
        strncpy(last->lexeme, empty, sizeof(sizeof(last->lexeme)));
        
    }
    
    return last;
    
}

//
//  Determines the numerical value representing a token.
//
//  @param ch
//      char, character from input array.
//  @return
//      int, numerical value representing a token.
//
int setSymbolClass(char ch)
{
    
    switch ( ch ) {
            
        case '+':
            
            return plussym;
            
        case '-':
            
            return minussym;
            
        case '*':
            
            return multsym;
            
        case '/':
            
            return slashsym;
            
        case '=':
            
            return eqlsym;
            
        case '<':
            
            return lessym;
            
        case '>':
            
            return gtrsym;
            
        case '(':
            
            return lparentsym;
            
        case ')':
            
            return rparentsym;
            
        case ',':
            
            return commasym;
            
        case ';':
            
            return semicolonsym;
            
        case '.':
            
            return periodsym;
            
        default:
            
            return -1;
            
    }
    
}

//
//  Prints to output files.
//
//  @param ch
//      char, character from input array.
//  @return
//      int, numerical value representing a token.
//
void print(token *lexemes)
{
    
    token *this = lexemes;
    
    FILE * ofpT = fopen(TABLE_OUTPUT_FILE, "w");
    FILE * ofpL = fopen(LIST_OUTPUT_FILE, "w");
    
    //  Output printing for Lexeme Table file.
    fprintf(ofpT,"%-12s%s", "lexeme","token type");
    
    while ( this->next != NULL )
    {
        
        fprintf(ofpT,"\n%-12s%d", this->lexeme,this->class);
        fprintf(ofpL,"%d ", this->class);
        
        if ( this->class == 2 || this->class == 3 )
    
            fprintf(ofpL,"%s ", this->lexeme);
        
        this = this->next;
        
    }
    
    fclose(ofpT);
    fclose(ofpL);
    
}