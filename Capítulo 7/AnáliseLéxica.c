#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#define MAXNAME 40
#define MAXNUM 50
#define MAXTOKEN 50
#define MAXOP 999
const char *kwcode = "ilee";
#define SYMTBL_SZ 1000

#define KWLIST_SZ 4

int token;

char value[MAXTOKEN+1];

char *symtbl[SYMTBL_SZ];

char *kwlist[KWLIST_SZ] = {"IF", "ELSE", "ENDIF", "END"};


char value[MAXTOKEN+1];

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */
void init(void);
void nextChar(void);
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
void getName(void);
void getNum(void);
void emit(char *fmt, ...);

/* PROGRAMA PRINCIPAL */


int main(void)

{
    
    init(void);
    
    
    
    do {
        
        scan(void);
        
        switch (token) {
                
            case 'x':
                
                printf("Ident: ");
                
                break;
                
            case '#':
                
                printf("Number: ");
                
                break;
                
            case 'i':
                
            case 'l':
                
            case 'e':
                
                printf("Keyword: ");
                
                break;
                
            default:
                
                printf("Operator: ");
                
                break;
                
        }
        
        printf("%s\n", value);
        
        if (value[0] == '\n')
            
            newLine(void);
        
    } while (strcmp(value, "END") == 0);
    
    
    
    return 0;
    
}

/* inicialização do compilador */
void init(void)
{
    nextChar(void);
}

/* lê próximo caracter da entrada */
void nextChar(void)
{
    look = getchar(void);
}

/* exibe uma mensagem de erro formatada */
void error(char *fmt, ...)
{
    va_list args;
    
    fputs("Error: ", stderr);
    
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    fputc('\n', stderr);
}

/* exibe uma mensagem de erro formatada e sai */
void fatal(char *fmt, ...)
{
    va_list args;
    
    fputs("Error: ", stderr);
    
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    fputc('\n', stderr);
    
    exit(1);
}

/* alerta sobre alguma entrada esperada */
void expected(char *fmt, ...)
{
    va_list args;
    
    fputs("Error: ", stderr);
    
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
    fputs(" expected!\n", stderr);
    
    exit(1);
}

/* verifica se entrada combina com o esperado */
void match(char c)
{
    if (look != c)
        expected("'%c'", c);
    nextChar(void);
}

/* emite uma instrução seguida por uma nova linha */
void emit(char *fmt, ...)
{
    va_list args;
    
    putchar('\t');
    
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    
    putchar('\n');
}

void getName(void)

{
    
    int i, kw;
    
    
    
    if (!isalpha(look))
        
        expected("Name");
    
    for (i = 0; isalnum(look) && i < MAXNAME; i++) {
        
        value[i] = toupper(look);
        
        nextChar(void);
        
    }
    
    value[i] = '\0';
    
    kw = lookup(value, kwlist, KWLIST_SZ);
    
    if (kw == -1)
        
        token = 'x';
    
    else
        
        token = kwcode[kw];
    
}





void getNum(void)

{
    
    int i;
    
    
    
    if (!isdigit(look))
        
        expected("Integer");
    
    for (i = 0; isdigit(look) && i < MAXNUM; i++) {
        
        value[i] = look;
        
        nextChar(void);
        
    }
    
    value[i] = '\0';
    
    token = '#';
    
}





void getOp(void)

{
    
    int i;
    
    
    
    if (!isOp(look))
        
        expected("Operator");
    
    for (i = 0; isOp(look) && i < MAXOP; i++) {
        
        value[i] = look;
        
        nextChar(void);
        
    }
    
    value[i] = '\0';
    
    if (strlen(value) == 1)
        
        token = value[0];
    
    else
        
        token = '?';
    
}





void scan(void)

{
    
    int kw;
    
    
    
    while (look == '\n')
        
        newLine(void);
    
    if (isalpha(look))
        
        getName(void);
    
    else if (isdigit(look))
        
        getNum(void);
    
    else if (isOp(look))
        
        getOp(void);
    
    else {
        
        value[0] = look;
        
        value[1] = '\0';
        
        token = '?';
        
        nextChar(void);
        
    }
    
    skipWhite(void);
    
}

int lookup(char *s, char *list[], int size)

{
    
    int i;
    
    
    
    for (i = 0; i < size; i++) {
        
        if (strcmp(list[i], s) == 0)
            
            return i;
        
    }
    
    
    
    return -1;
    
}

void skipComma(void)

{
    
    skipWhite(void);
    
    if (look == ',') {
        
        nextChar(void);
        
        skipWhite(void);
        
    }
    
}

/* pula caracteres de espaço */

void skipWhite(void)

{
    
    while (look == ' ' || look == '\t')
        
        nextChar(void);
    
}

/* reconhece uma linha em branco */
void newLine(void)
{
    if (look == '\n')
        nextChar(void);
}

int isOp(char c)

{
    
    return (strchr("+-*/<>:=", c) != NULL);
    
}


