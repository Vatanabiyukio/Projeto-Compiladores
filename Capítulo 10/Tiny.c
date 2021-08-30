#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#define SYMTBL_SZ 1000

#define KWLIST_SZ 11

#define MAXTOKEN 16

int lblcount; /* indica o rótulo atual */

/* tabela de símbolos */

char *symtbl[SYMTBL_SZ];

int nsym; /* número de entradas atuais na tabela de símbolos */

/* códigos e lista de palavras-chave */

char kwcode[KWLIST_SZ+1] = "ileweRWvbep";

char *kwlist[KWLIST_SZ] = {"IF", "ELSE", "ENDIF", "WHILE", "ENDWHILE",
    
    "READ", "WRITE", "VAR", "BEGIN", "END", "PROGRAM"};

char look; /* O caracter lido "antecipadamente" (lookahead) */

char token; /* código do token atual */

char value[MAXTOKEN+1]; /* texto do token atual */

/* PROTÓTIPOS */

/* rotinas utilitárias */

void init(void);

void nextchar(void);

void error(char *s);

void fatal(char *s);

void expected(char *s);

void undefined(char *name);

/* reconhecedores */

int isaddop(char c);

int ismulop(char c);

int isorop(char c);

int isrelop(char c);

/* tratamento de símbolos */

int lookup(char *s, char *list[], int size);

int intable(char *name);

void addsymbol(char *name);

/* analisador léxico */

void skipwhite(void);

void newline(void);

void match(char c);

void getname(void);

int getnum(void);

void scan(void);

void matchstring(char *s);

/* rótulos */

int newlabel(void);

/* rotinas de geração de código */

void asm_clear(void);

void asm_negative(void);

void asm_loadconst(int i);

void asm_loadvar(char *name);

void asm_push(void);

void asm_popadd(void);

void asm_popsub(void);

void asm_popmul(void);

void asm_popdiv(void);

void asm_store(char *name);

void asm_not(void);

void asm_popand(void);

void asm_popor(void);

void asm_popxor(void);

void asm_popcompare(void);

void asm_relop(char op);

void asm_jmp(int label);

void asm_jmpfalse(int label);

void asm_read(void);

void asm_write(void);

void header(void);

void prolog(void);

void epilog(void);

/* rotinas do analisador sintático */

/* expressões aritméticas */

void factor(void);

void negfactor(void);

void firstfactor(void);

void multiply(void);

void divide(void);

void term1(void);

void term(void);

void firstterm(void);

void add(void);

void subtract(void);

void expression(void);

/* expressões booleanas e relações */

void relation(void);

void notfactor(void);

void boolterm(void);

void boolor(void);

void boolxor(void);

void boolexpression(void);

/* bloco, estruturas de controle e comandos */

void assignment(void);

void doif(void);

void dowhile(void);

void doread(void);

void dowrite(void);

void block(void);

/* declarações */

void allocvar(char *name);

void decl(void);

void topdecls(void);

/* programa principal */

void mainblock(void);

void prog(void);

/* PROGRAMA PRINCIPAL */

int main(void)

{
    
    init(void);
    
    prog(void);
    
    if (look != '\n')
        
        fatal("Unexpected data after \'.\'");
    
    return 0;
    
}

/* lê próximo caracter da entrada em lookahead */

void nextchar(void)

{
    
    look = getchar(void);
    
}

/* imprime mensagem de erro sem sair */

void error(char *s)

{
    
    fprintf(stderr, "Error: %s\n", s);
    
}

/* imprime mensagem de erro e sai */

void fatal(char *s)

{
    
    error(s);
    
    exit(1);
    
}

/* mensagem de erro para string esperada */

void expected(char *s)

{
    
    fprintf(stderr, "Error: %s expected\n", s);
    
    exit(1);
    
}

/* avisa a respeito de um identificador desconhecido */

void undefined(char *name)

{
    
    int i;
    
    fprintf(stderr, "Error: Undefined identifier %s\n", name);
    
    fprintf(stderr, "Symbol table:\n");
    
    for (i = 0; i < nsym; i++)
        
        fprintf(stderr, "%d: %s\n", i, symtbl[i]);
    
    exit(1);
    
}

/* testa operadores de adição */

int isaddop(char c)

{
    
    return (c == '+' || c == '-');
    
}

/* testa operadores de multiplicação */

int ismulop(char c)

{
    
    return (c == '*' || c == '/');
    
}

/* testa operadores OU */

int isorop(char c)

{
    
    return (c == '|' || c == '~');
    
}

/* testa operadores relacionais */

int isrelop(char c)

{
    
    return (strchr("=#<>", c) != NULL);
    
}

/* pula caracteres em branco */

void skipwhite(void)

{
    
    while (look == ' ' || look == '\t')
        
        nextchar(void);
    
}

/* pula quebras de linha e linhas vazias*/

void newline(void)

{
    
    while (look == '\n') {
        
        nextchar(void);
        
        skipwhite(void);
        
    }
    
}

/* compara lookahead com caracter */

void match(char c)

{
    
    char s[2];
    
    newline(void);
    
    if (look == c)
        
        nextchar(void);
    
    else {
        
        s[0] = c; /* uma conversao rápida (e feia) */
        
        s[1] = '\0'; /* de um caracter em string */
        
        expected(s);
        
    }
    
    skipwhite(void);
    
}

/* procura por string em tabela,
 
 usado para procurar palavras-chave e símbolos */

int lookup(char *s, char *list[], int size)

{
    
    int i;
    
    for (i = 0; i < size; i++) {
        
        if (strcmp(list[i], s) == 0)
            
            return i;
        
    }
    
    return -1;
    
}

/* verifica se "name" consta na tabela de símbolos */

int intable(char *name)

{
    
    if (lookup(name, symtbl, nsym) < 0)
        
        return 0;
    
    return 1;
    
}

/* adiciona novo identificador à tabela de símbolos */

void addsymbol(char *name)

{
    
    char *newsym;
    
    if (intable(name)) {
        
        fprintf(stderr, "Duplicated variable name: %s", name);
        
        exit(1);
        
    }
    
    if (nsym >= SYMTBL_SZ) {
        
        fatal("Symbol table full!");
        
    }
    
    newsym = (char *) malloc(sizeof(char) * (strlen(name) + 1));
    
    if (newsym == NULL)
        
        fatal("Out of memory!");
    
    strcpy(newsym, name);
    
    symtbl[nsym++] = newsym;
    
}

/* analisa e traduz um nome (identificador ou palavra-chave) */

void getname(void)

{
    
    int i;
    
    newline(void);
    
    if (!isalpha(look))
        
        expected("Name");
    
    for (i = 0; isalnum(look) && i < MAXTOKEN; i++) {
        
        value[i] = toupper(look);
        
        nextchar(void);
        
    }
    
    value[i] = '\0';
    
    token = 'x';
    
    skipwhite(void);
    
}

/* analisa e traduz um número inteiro */

int getnum(void)

{
    
    int i;
    
    i = 0;
    
    newline(void);
    
    if (!isdigit(look))
        
        expected("Integer");
    
    while (isdigit(look)) {
        
        i *= 10;
        
        i += look - '0';
        
        nextchar(void);
        
    }
    
    skipwhite(void);
    
    return i;
    
}

/* analisador léxico. analisa identificador ou palavra-chave */

void scan(void)

{
    
    int kw;
    
    getname(void);
    
    kw = lookup(value, kwlist, KWLIST_SZ);
    
    if (kw == -1)
        
        token = 'x';
    
    else
        
        token = kwcode[kw];
    
}

/* compara string com texto do token atual */

void matchstring(char *s)

{
    
    if (strcmp(value, s) != 0)
        
        expected(s);
    
}

/* gera um novo rótulo */

int newlabel(void)

{
    
    return lblcount++;
    
}

/* ROTINAS DE GERAÇÃO DE CÓDIGO */

/* zera o registrador primário */

void asm_clear(void)

{
    
    printf("\txor ax, ax\n");
    
}

/* negativa o reg. primário */

void asm_negative(void)

{
    
    printf("\tneg ax\n");
    
}

/* carrega uma constante numérica no reg. prim. */

void asm_loadconst(int i)

{
    
    printf("\tmov ax, %d\n", i);
    
}

/* carrega uma variável no reg. prim. */

void asm_loadvar(char *name)

{
    
    if (!intable(name))
        
        undefined(name);
    
    printf("\tmov ax, word ptr %s\n", name);
    
}

/* coloca reg. prim. na pilha */

void asm_push(void)

{
    
    printf("\tpush ax\n");
    
}

/* adiciona o topo da pilha ao reg. prim. */

void asm_popadd(void)

{
    
    printf("\tpop bx\n");
    
    printf("\tadd ax, bx\n");
    
}

/* subtrai o reg. prim. do topo da pilha */

void asm_popsub(void)

{
    
    printf("\tpop bx\n");
    
    printf("\tsub ax, bx\n");
    
    printf("\tneg ax\n");
    
}

/* multiplica o topo da pilha pelo reg. prim. */

void asm_popmul(void)

{
    
    printf("\tpop bx\n");
    
    printf("\tmul bx\n");
    
}

/* divide o topo da pilha pelo reg. prim. */

void asm_popdiv(void)

{
    
    printf("\tpop bx\n");
    
    printf("\txchg ax, bx\n");
    
    printf("\tcwd\n");
    
    printf("\tdiv bx\n");
    
}

/* armazena reg. prim. em variável */

void asm_store(char *name)

{
    
    if (!intable(name))
        
        undefined(name);
    
    printf("\tmov word ptr %s, ax\n", name);
    
}

/* inverte reg. prim. */

void asm_not(void)

{
    
    printf("\tnot ax\n");
    
}

/* "E" do topo da pilha com reg. prim. */

void asm_popand(void)

{
    
    printf("\tpop bx\n");
    
    printf("\tand ax, bx\n");
    
}

/* "OU" do topo da pilha com reg. prim. */

void asm_popor(void)

{
    
    printf("\tpop bx\n");
    
    printf("\tor ax, bx\n");
    
}

/* "OU-exclusivo" do topo da pilha com reg. prim. */

void asm_popxor(void)

{
    
    printf("\tpop bx\n");
    
    printf("\txor ax, bx\n");
    
}

/* compara topo da pilha com reg. prim. */

void asm_popcompare(void)

{
    
    printf("\tpop bx\n");
    
    printf("\tcmp bx, ax\n");
    
}

/* altera reg. primário (e flags, indiretamente) conforme a comparação */

void asm_relop(char op)

{
    
    char *jump;
    
    int l1, l2;
    
    l1 = newlabel(void);
    
    l2 = newlabel(void);
    
    switch (op) {
            
        case '=': jump = "je"; break;
            
        case '#': jump = "jne"; break;
            
        case '<': jump = "jl"; break;
            
        case '>': jump = "jg"; break;
            
        case 'L': jump = "jle"; break;
            
        case 'G': jump = "jge"; break;
            
    }
    
    printf("\t%s L%d\n", jump, l1);
    
    printf("\txor ax, ax\n");
    
    printf("\tjmp L%d\n", l2);
    
    printf("L%d:\n", l1);
    
    printf("\tmov ax, -1\n");
    
    printf("L%d:\n", l2);
    
}

/* desvio incondicional */

void asm_jmp(int label)

{
    
    printf("\tjmp L%d\n", label);
    
}

/* desvio se falso (0) */

void asm_jmpfalse(int label)

{
    
    printf("\tjz L%d\n", label);
    
}

/* lê um valor para o registrador primário e armazena em variável */

void asm_read(void)

{
    
    printf("\tcall READ\n");
    
    asm_store(value);
    
}

/* mostra valor do reg. primário */

void asm_write(void)

{
    
    printf("\tcall WRITE\n");
    
}

/* cabeçalho do código assembly */

void header(void)

{
    
    printf(".model small\n");
    
    printf(".stack\n");
    
    printf(".code\n");
    
    printf("extrn READ:near, WRITE:near\n");
    
    printf("PROG segment byte public\n");
    
    printf("\tassume cs:PROG,ds:PROG,es:PROG,ss:PROG\n");
    
}

/* prólogo da rotina principal */

void prolog(void)

{
    
    printf("MAIN:\n");
    
    printf("\tmov ax, PROG\n");
    
    printf("\tmov ds, ax\n");
    
    printf("\tmov es, ax\n");
    
}

/* epílogo da rotina principal */

void epilog(void)

{
    
    printf("\tmov ax,4C00h\n");
    
    printf("\tint 21h\n");
    
    printf("PROG ends\n");
    
    printf("\tend MAIN\n");
    
}

/* analisa e traduz um fator matemático */

void factor(void)

{
    
    newline(void);
    
    if (look == '(') {
        
        match('(');
        
        boolexpression(void);
        
        match(')');
        
    } else if (isalpha(look)) {
        
        getname(void);
        
        asm_loadvar(value);
        
    } else
        
        asm_loadconst(getnum(void));
    
}

/* analisa e traduz um fator negativo */

void negfactor(void)

{
    
    match('-');
    
    if (isdigit(look))
        
        asm_loadconst(-getnum(void));
    
    else {
        
        factor(void);
        
        asm_negative(void);
        
    }
    
}

/* analisa e traduz um fator inicial */

void firstfactor(void)

{
    
    newline(void);
    
    switch (look) {
            
        case '+':
            
            match('+');
            
            factor(void);
            
            break;
            
        case '-':
            
            negfactor(void);
            
            break;
            
        default:
            
            factor(void);
            
            break;
            
    }
    
}

/* reconhece e traduz uma multiplicação */

void multiply(void)

{
    
    match('*');
    
    factor(void);
    
    asm_popmul(void);
    
}

/* reconhece e traduz uma divisão */

void divide(void)

{
    
    match('/');
    
    factor(void);
    
    asm_popdiv(void);
    
}

/* código comum usado por "term" e "firstterm" */

void term1(void)

{
    
    newline(void);
    
    while (ismulop(look))  {
        
        asm_push(void);
        
        switch (look) {
                
            case '*':
                
                multiply(void);
                
                break;
                
            case '/':
                
                divide(void);
                
                break;
                
        }
        
        newline(void);
        
    }
    
}

/* analisa e traduz um termo matemático */

void term(void)

{
    
    factor(void);
    
    term1(void);
    
}

/* analisa e traduz um termo inicial */

void firstterm(void)

{
    
    firstfactor(void);
    
    term1(void);
    
}

/* reconhece e traduz uma adição */

void add(void)

{
    
    match('+');
    
    term(void);
    
    asm_popadd(void);
    
}

/* reconhece e traduz uma subtração*/

void subtract(void)

{
    
    match('-');
    
    term(void);
    
    asm_popsub(void);
    
}

/* analisa e traduz uma expressão matemática */

void expression(void)

{
    
    firstterm(void);
    
    newline(void);
    
    while (isaddop(look))  {
        
        asm_push(void);
        
        switch (look) {
                
            case '+':
                
                add(void);
                
                break;
                
            case '-':
                
                subtract(void);
                
                break;
                
        }
        
        newline(void);
        
    }
    
}

/* analisa e traduz uma relação */

void relation(void)

{
    
    char op;
    
    expression(void);
    
    if (isrelop(look)) {
        
        op = look;
        
        match(op); /* só para remover o operador do caminho */
        
        if (op == '<') {
            
            if (look == '>') { /* <> */
                
                match('>');
                
                op = '#';
                
            } else if (look == '=') {
                
                match('=');
                
                op = 'L';
                
            }
            
        } else if (op == '>' && look == '=') {
            
            match('=');
            
            op = 'G';
            
        }
        
        asm_push(void);
        
        expression(void);
        
        asm_popcompare(void);
        
        asm_relop(op);
        
    }
    
}

/* analisa e traduz um fator booleano com NOT inicial */

void notfactor(void)

{
    
    if (look == '!') {
        
        match('!');
        
        relation(void);
        
        asm_not(void);
        
    } else
        
        relation(void);
    
}

/* analisa e traduz um termo booleano */

void boolterm(void)

{
    
    notfactor(void);
    
    newline(void);
    
    while (look == '&') {
        
        asm_push(void);
        
        match('&');
        
        notfactor(void);
        
        asm_popand(void);
        
        newline(void);
        
    }
    
}

/* reconhece e traduz um "OR" */

void boolor(void)

{
    
    match('|');
    
    boolterm(void);
    
    asm_popor(void);
    
}

/* reconhece e traduz um "xor" */

void boolxor(void)

{
    
    match('~');
    
    boolterm(void);
    
    asm_popxor(void);
    
}

/* analisa e traduz uma expressão booleana */

void boolexpression(void)

{
    
    boolterm(void);
    
    newline(void);
    
    while (isorop(look)) {
        
        asm_push(void);
        
        switch (look) {
                
            case '|':
                
                boolor(void);
                
                break;
                
            case '~':
                
                boolxor(void);
                
                break;
                
        }
        
        newline(void);
        
    }
    
}

/* analisa e traduz um comando de atribuição */

void assignment(void)

{
    
    char name[MAXTOKEN+1];
    
    strcpy(name, value);
    
    match('=');
    
    boolexpression(void);
    
    asm_store(name);
    
}

/* analiza e traduz um comando IF-ELSE-ENDIF */

void doif(void)

{
    
    int l1, l2;
    
    boolexpression(void);
    
    l1 = newlabel(void);
    
    l2 = l1;
    
    asm_jmpfalse(l1);
    
    block(void);
    
    if (token == 'l') {
        
        l2 = newlabel(void);
        
        asm_jmp(l2);
        
        printf("L%d:\n", l1);
        
        block(void);
        
    }
    
    printf("L%d:\n", l2);
    
    matchstring("ENDIF");
    
}

/* analiza e traduz um comando WHILE-ENDWHILE */

void dowhile(void)

{
    
    int l1, l2;
    
    l1 = newlabel(void);
    
    l2 = newlabel(void);
    
    printf("L%d:\n", l1);
    
    boolexpression(void);
    
    asm_jmpfalse(l2);
    
    block(void);
    
    matchstring("ENDWHILE");
    
    asm_jmp(l1);
    
    printf("L%d:\n", l2);
    
}

/* analiza e traduz um comando READ */

void doread(void)

{
    
    match('(');
    
    for (;;) {
        
        getname(void);
        
        asm_read(void);
        
        newline(void);
        
        if (look != ',')
            
            break;
        
        match(',');
        
    }
    
    match(')');
    
}

/* analiza e traduz um comando WRITE */

void dowrite(void)

{
    
    match('(');
    
    for (;;) {
        
        expression(void);
        
        asm_write(void);
        
        newline(void);
        
        if (look != ',')
            
            break;
        
        match(',');
        
    }
    
    match(')');
    
}

/* analiza e traduz um bloco de comandos */

void block(void)

{
    
    int follow = 0;
    
    do {
        
        scan(void);
        
        switch (token) {
                
            case 'i':
                
                doif(void);
                
                break;
                
            case 'w':
                
                dowhile(void);
                
                break;
                
            case 'R':
                
                doread(void);
                
                break;
                
            case 'W':
                
                dowrite(void);
                
                break;
                
            case 'e':
                
            case 'l':
                
                follow = 1;
                
                break;
                
            default:
                
                assignment(void);
                
                break;
                
        }
        
    } while (!follow);
    
}

/* aloca memória para uma declaração de variável (+inicializador) */

void allocvar(char *name)

{
    
    int value = 0, signal = 1;
    
    addsymbol(name);
    
    newline(void);
    
    if (look == '=') {
        
        match('=');
        
        newline(void);
        
        if (look == '-') {
            
            match('-');
            
            signal = -1;
            
        }
        
        value = signal * getnum(void);
        
    }
    
    printf("%s:\tdw %d\n", name, value);
    
}

/* analisa e traduz uma declaração */

void decl(void)

{
    
    for (;;) {
        
        getname(void);
        
        allocvar(value);
        
        newline(void);
        
        if (look != ',')
            
            break;
        
        match(',');
        
    }
    
}

/* analisa e traduz declarações */

void topdecls(void)

{
    
    scan(void);
    
    while (token != 'b') {
        
        switch (token) {
                
            case 'v':
                
                decl(void);
                
                break;
                
            default:
                
                error("Unrecognized keyword.");
                
                expected("BEGIN");
                
                break;
                
        }
        
        scan(void);
        
    }
    
}

/* analisa e traduz o bloco principal do programa*/

void mainblock(void)

{
    
    matchstring("BEGIN");
    
    prolog(void);
    
    block(void);
    
    matchstring("END");
    
    epilog(void);
    
}

/* analisa e traduz um programa TINY */

void prog(void)

{
    
    matchstring("PROGRAM");
    
    header(void);
    
    topdecls(void);
    
    mainblock(void);
    
    match('.');
    
}

/* inicialização do compilador */

void init(void)

{
    
    nsym = 0;
    
    nextchar(void);
    
    scan(void);
    
}
