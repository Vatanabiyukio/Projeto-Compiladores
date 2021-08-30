#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */
void init(void);
void nextChar();
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
char getName(void);
char getNum(void);
void emit(char *fmt, ...);

/* PROGRAMA PRINCIPAL */
int main(void)
{
    init();
    
    return 0;
}

/* inicialização do compilador */
void init(void)
{
    nextChar();
}

/* lê próximo caracter da entrada */
void nextChar()
{
    look = getchar();
}

/* reconhece operador aditivo */

int isAddOp(char c)

{
    
    return (c == '+' || c == '-');
    
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
    nextChar();
}

/* recebe o nome de um identificador */
char getName(void)
{
    char name;
    
    if (!isalpha(look))
        expected("Name");
    name = toupper(look);
    nextChar();
    
    return name;
}

/* recebe um número inteiro */
char getNum(void)
{
    char num;
    
    if (!isdigit(look))
        expected("Integer");
    num = look;
    nextChar();
    
    return num;
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
