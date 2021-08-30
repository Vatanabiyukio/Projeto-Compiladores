#include <stdio.h>

#include <stdlib.h>

#include <stdarg.h>

#define MAXNAME 30

#define MAXNUM 5

char look; /* O caracter lido "antecipadamente" (lookahead) */

/* protótipos */

void init(void);
void nextChar(void);

void error(char *fmt, ...);

void fatal(char *fmt, ...);

void expected(char *fmt, ...);

void skipWhite(void);

void match(char c);

void getName(char *name);

void getNum(char *num);

void emit(char *fmt, ...);

int isAddOp(char c);

void ident(void);

void factor(void);

void multiply(void);

void divide(void);

void term(void);

void add(void);

void subtract(void);

void expression(void);

void assignment(void);

/* PROGRAMA PRINCIPAL */

int main(void)

{
    
    init(void);
    
    assignment(void);
    
    if (look != '\n')
        
        expected("NewLine");
    
    return 0;
}

/* inicialização do compilador */

void init(void)

{
    
    nextChar(void);
    
    skipWhite(void);
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

/* pula caracteres de espaço */

void skipWhite(void)

{
    
    while (look == ' ' || look == '\t')
        
        nextChar(void);
}

/* verifica se entrada combina com o esperado */

void match(char c)

{
    
    if (look != c)
        
        expected("'%c'", c);
    
    nextChar(void);
    
    skipWhite(void);
}

/* recebe o nome de um identificador */

void getName(char *name)

{
    
    int i;
    
    if (!isalpha(look))
        
        expected("Name");
    
    for (i = 0; isalnum(look); i++)
    {
        
        if (i >= MAXNAME)
            
            fatal("Identifier too long!");
        
        name[i] = toupper(look);
        
        nextChar(void);
    }
    
    name[i] = '\0';
    
    skipWhite(void);
}

/* recebe um número inteiro */

void getNum(char *num)

{
    
    int i;
    
    if (!isdigit(look))
        
        expected("Integer");
    
    for (i = 0; isdigit(look); i++)
    {
        
        if (i >= MAXNUM)
            
            fatal("Integer too long!");
        
        num[i] = look;
        
        nextChar(void);
    }
    
    num[i] = '\0';
    
    skipWhite(void);
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

/* reconhece operador aditivo */

int isAddOp(char c)

{
    
    return (c == '+' || c == '-');
}

/* analisa e traduz um identificador */

void ident(void)

{
    
    char name[MAXNAME + 1];
    
    getName(name);
    
    if (look == '(')
    {
        
        match('(');
        
        match(')');
        
        emit("CALL %s", name);
    }
    else
        
        emit("MOV AX, [%s]", name);
}

/* analisa e traduz um fator */

void factor(void)

{
    
    char num[MAXNUM + 1];
    
    if (look == '(')
    {
        
        match('(');
        
        expression(void);
        
        match(')');
    }
    else if (isalpha(look))
    {
        
        ident(void);
    }
    else
    {
        
        getNum(num);
        
        emit("MOV AX, %s", num);
    }
}

/* reconhece e traduz uma multiplicação */

void multiply(void)

{
    
    match('*');
    
    factor(void);
    
    emit("POP BX");
    
    emit("IMUL BX");
}

/* reconhece e traduz uma divisão */

void divide(void)

{
    
    match('/');
    
    factor(void);
    
    emit("POP BX");
    
    emit("XCHG AX, BX");
    
    emit("CWD");
    
    emit("IDIV BX");
}

/* analisa e traduz um termo */

void term(void)

{
    
    factor(void);
    
    while (look == '*' || look == '/')
    {
        
        emit("PUSH AX");
        
        switch (look)
        {
                
            case '*':
                
                multiply(void);
                
                break;
                
            case '/':
                
                divide(void);
                
                break;
        }
    }
}

/* reconhece e traduz uma adição */

void add(void)

{
    
    match('+');
    
    term(void);
    
    emit("POP BX");
    
    emit("ADD AX, BX");
}

/* reconhece e traduz uma subtração */

void subtract(void)

{
    
    match('-');
    
    term(void);
    
    emit("POP BX");
    
    emit("SUB AX, BX");
    
    emit("NEG AX");
}

/* analisa e traduz uma expressão */

void expression(void)

{
    
    if (isAddOp(look))
        
        emit("XOR AX, AX");
    
    else
        
        term(void);
    
    while (isAddOp(look))
    {
        
        emit("PUSH AX");
        
        switch (look)
        {
                
            case '+':
                
                add(void);
                
                break;
                
            case '-':
                
                subtract(void);
                
                break;
        }
    }
}

/* analisa e traduz um comando de atribuição */

void assignment(void)

{
    
    char name[MAXNAME + 1];
    
    getName(name);
    
    match('=');
    
    expression(void);
    
    emit("MOV [%s], AX", name);
}
