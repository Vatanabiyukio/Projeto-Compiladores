#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#define MAXVAR 26
int var[MAXVAR];

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */
void init(void);
void nextChar(void);
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
char getName(void);
int getNum(void);
void emit(char *fmt, ...);

/* PROGRAMA PRINCIPAL */
int main(void)
{
    init(void);
    do {
        switch (look) {
            case '?':
                input(void);
                break;
            case '!':
                output(void);
                break;
            default:
                assignment(void);
                break;
        }
        newLine(void);
    } while (look != '.');
    
    return 0;
}

/* interpreta um comando de entrada */
void input(void)
{
    char name;
    char buffer[20];
    
    match('?');
    name = getName(void);
    printf("%c? ", name);
    fgets(buffer, 20, stdin);
    var[name - 'A'] = atoi(buffer);
}

/* interpreta um comando de saída */
void output(void)
{
    char name;
    
    match("!");
    name = getName(void);
    printf("%c -> %d", name, var[name - 'A']);
}

/* reconhece operador aditivo */

int isAddOp(char c)

{
    
    return (c == '+' || c == '-');
    
}

int isMulOp(char c)

{
    
    return (c == '*' || c == '/');
    
}

/* inicializa variáveis */
void initVar(void)
{
    int i;
    
    for (i = 0; i < MAXVAR; i++)
        var[i] = 0;
}

/* avalia o resultado de uma expressão */
int expression(void)
{
    int val;
    
    if (isAddOp(look))
        val = 0;
    else
        val = term(void);
    
    while (isAddOp(look)) {
        switch (look) {
            case '+':
                match('+');
                val += term(void);
                break;
            case '-':
                match('-');
                val -= term(void);
                break;
        }
    }
    
    return val;
}

/* avalia um termo */
int term(void)
{
    int val;
    
    val = factor(void);
    while (isMulOp(look)) {
        switch (look) {
            case '*':
                match('*');
                val *= factor(void);
                break;
            case '/':
                match('/');
                val /= factor(void);
                break;
        }
    }
    
    return val;
}

/* inicialização do compilador */
void init(void)
{
    nextChar(void);
    initVar(void);
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

/* recebe o nome de um identificador */
char getName(void)
{
    char name;
    
    if (!isalpha(look))
        expected("Name");
    name = toupper(look);
    nextChar(void);
    
    return name;
}

/* recebe um número inteiro */
int getNum(void)
{
    int i;
    
    i = 0;
    
    if (!isdigit(look))
        expected("Integer");
    
    while (isdigit(look)) {
        i *= 10;
        i += look - '0';
        nextChar(void);
    }
    
    return i;
}

/* avalia um fator */
int factor(void)
{
    int val;
    
    if (look == '(') {
        match('(');
        val = expression(void);
        match(')');
    } else if (isalpha(look))
        val = var[getName(void) - 'A'];
    else
        val = getNum(void);
    
    return val;
}

/* avalia um comando de atribuição */
void assignment(void)
{
    char name;
    
    name = getName(void);
    match('=');
    var[name - 'A'] = expression(void);
}

/* captura um caracter de nova linha */
void newLine(void)
{
    if (look == '\n')
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
