#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

char look; /* O caracter lido "antecipadamente" (lookahead) */
int labelCount; /* Contador usado pelo gerador de rótulos */

/* protótipos */
void init(void);
void nextChar(void);
void error(char *fmt, ...);
void fatal(char *fmt, ...);
void expected(char *fmt, ...);
void match(char c);
char getName(void);
char getNum(void);
void emit(char *fmt, ...);

int newLabel(void);
int postLabel(int lbl);

void condition(void);
void expression(void);

void other(void);
void doIf(int exitLabel);
void doWhile(void);
void doLoop(void);
void doRepeat(void);
void doFor(void);
void doDo(void);
void block(int exitLabel);
void program(void);

/* PROGRAMA PRINCIPAL */
int main(void)
{
    init(void);
    program(void);
    
    return 0;
}

/* inicialização do compilador */
void init(void)
{
    labelCount = 0;
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
char getNum(void)
{
    char num;
    
    if (!isdigit(look))
        expected("Integer");
    num = look;
    nextChar(void);
    
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

/* gera um novo rótulo único */
int newLabel(void)
{
    return labelCount++;
}

/* emite um rótulo */
int postLabel(int lbl)
{
    printf("L%d:\n", lbl);
}

/* analisa e traduz uma condição */
void condition(void)
{
    emit("# condition");
}

/* analisa e traduz uma expressão */
void expression(void)
{
    emit("# expression");
}

/* reconhece e traduz um comando qualquer */
void other(void)
{
    emit("# %c", getName(void));
}

/* analisa e traduz um comando IF */
void doIf(int exitLabel)
{
    int l1, l2;
    
    match('i');
    condition(void);
    l1 = newLabel(void);
    l2 = l1;
    emit("JZ L%d", l1);
    block(exitLabel);
    if (look == 'l') {
        match('l');
        l2 = newLabel(void);
        emit("JMP L%d", l2);
        postLabel(l1);
        block(exitLabel);
    }
    match('e');
    postLabel(l2);
}

/* analisa e traduz um comando WHILE */
void doWhile(void)
{
    int l1, l2;
    
    match('w');
    l1 = newLabel(void);
    l2 = newLabel(void);
    postLabel(l1);
    condition(void);
    emit("JZ L%d", l2);
    block(l2);
    match('e');
    emit("JMP L%d", l1);
    postLabel(l2);
}

/* analisa e traduz um comando LOOP*/
void doLoop(void)
{
    int l1, l2;
    
    match('p');
    l1 = newLabel(void);
    l2 = newLabel(void);
    postLabel(l1);
    block(l2);
    match('e');
    emit("JMP L%d", l1);
    postLabel(l2);
}

/* analisa e traduz um REPEAT-UNTIL*/
void doRepeat(void)
{
    int l1, l2;
    
    match('r');
    l1 = newLabel(void);
    l2 = newLabel(void);
    postLabel(l1);
    block(l2);
    match('u');
    condition(void);
    emit("JZ L%d", l1);
    postLabel(l2);
}

/* analisa e traduz um comando FOR*/
void doFor(void)
{
    int l1, l2;
    char name;
    
    match('f');
    l1 = newLabel(void);
    l2 = newLabel(void);
    name = getName(void);
    match('=');
    expression(void);
    emit("DEC AX");
    emit("MOV [%c], AX", name);
    expression(void);
    emit("PUSH AX");
    postLabel(l1);
    emit("MOV AX, [%c]", name);
    emit("INC AX");
    emit("MOV [%c], AX", name);
    emit("POP BX");
    emit("PUSH BX");
    emit("CMP AX, BX");
    emit("JG L%d", l2);
    block(l2);
    match('e');
    emit("JMP L%d", l1);
    postLabel(l2);
    emit("POP AX");
}

/* analisa e traduz um comando DO*/
void doDo(void)
{
    int l1, l2;
    
    match('d');
    l1 = newLabel(void);
    l2 = newLabel(void);
    expression(void);
    emit("MOV CX, AX");
    postLabel(l1);
    emit("PUSH CX");
    block(l2);
    emit("POP CX");
    emit("LOOP L%d", l1);
    emit("PUSH CX");
    postLabel(l2);
    emit("POP CX");
}

/* analisa e traduz um comando BREAK */
void doBreak(int exitLabel)
{
    match('b');
    if (exitLabel == -1)
        fatal("No loop to break from.");
    emit("JMP L%d", exitLabel);
}

/* analisa e traduz um bloco de comandos */
void block(int exitLabel)
{
    int follow;
    
    follow = 0;
    
    while (!follow) {
        switch (look) {
            case 'i':
                doIf(exitLabel); break;
            case 'w':
                doWhile(void); break;
            case 'p':
                doLoop(void); break;
            case 'r':
                doRepeat(void); break;
            case 'f':
                doFor(void); break;
            case 'd':
                doDo(void); break;
            case 'b':
                doBreak(exitLabel); break;
            case 'e':
            case 'l':
            case 'u':
                follow = 1;
                break;
            default:
                other(void); break;
        }
    }
}

/* analisa e traduz um programa completo */
void program(void)
{
    block(-1);
    if (look != 'e')
        expected("End");
    emit("END");
}
