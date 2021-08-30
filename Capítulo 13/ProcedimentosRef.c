#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#define SYMTBL_SZ 26
#define PARAMS_SZ 26

int params[PARAMS_SZ]; /* lista de parâmetros formais para os procedimentos */
int nparams; /* número de parâmetros formais */
int base;


char look; /* O caracter lido "antecipadamente" (lookahead) */

char symtbl[SYMTBL_SZ]; /* tabela de símbolos */

/* rotinas utilitárias */

void init(void);

void nextchar(void);

void error(char *s);

void fatal(char *s);

void expected(char *s);

void undefined(char name);

void duplicated(char name);

void unrecognized(char name);

void notvar(char name);

/* tratamento da tabela de símbolos */

char symtype(char name);

char intable(char name);

void addsymbol(char name, char type);

void checkvar(char name);

/* analisador léxico rudimentar */

int isaddop(char c);

int ismulop(char c);

void skipwhite(void);

void newline(void);

void match(char c);

char getname(void);

char getnum(void);

/* geração de código */

void asm_loadvar(char name);

void asm_storevar(char name);

void asm_allocvar(char name);

/* analisador sintático */

void expression(void);

void assignment(char name);

void doblock(void);

void beginblock(void);

void decl(void);

void topdecls(void);


/* analiza e traduz o bloco principal do programa */

void domain(void)

{
    
    char name;
    
    match('P');
    
    name = getname(void);
    
    newline(void);
    
    if (intable(name))
        
        duplicated(name);
    
    prolog(void);
    
    beginblock(void);
    
}

void clearparams(void)

{
    
    int i;
    
    for (i = 0; i < PARAMS_SZ; i++)
        
        params[i] = 0;
    
    nparams = 0;
    
}

/* analiza e traduz as declarações globais */

void topdecls(void)

{
    
    while (look != '.') {
        
        switch (look) {
                
            case 'v':
                
                decl(void);
                
                break;
                
            case 'p':
                
                doproc(void);
                
                break;
                
            case 'P':
                
                domain(void);
                
                break;
                
            default:
                
                unrecognized(look);
                
                break;
                
        }
        
        newline(void);
        
    }
    
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

/* PROGRAMA PRINCIPAL */

int main(void)

{
    
    init(void);
    
    header(void);
    
    topdecls(void);
    
    epilog(void);
    
    return 0;
    
}

void init(void)

{
    
    int i;
    
    for (i = 0; i < SYMTBL_SZ; i++)
        
        symtbl[i] = ' ';
    
    clearparams(void);
    
    nextchar(void);
    
    skipwhite(void);
    
}

void doproc(void)

{
    
    int nlocals;
    
    char name;
    
    match('p');
    
    name = getname(void);
    
    addsymbol(name, 'p');
    
    formallist(void);
    
    nlocals = locdecls(void);
    
    asm_procprolog(name, nlocals);
    
    beginblock(void);
    
    asm_procepilog(void);
    
    clearparams(void);
    
}

/* retorna número indicando a posição do parâmetro */

int paramnum(char name)

{
    
    return params[name - 'A'];
    
}

/* verifica se nome é parâmetro */

int isparam(char name)

{
    
    return (params[name - 'A'] != 0);
    
}

/* adiciona parâmetro à lista */

void addparam(char name)

{
    
    if (isparam(name))
        
        duplicated(name);
    
    params[name - 'A'] = ++nparams;
    
}


int asm_offsetpar(int par)

{
    
    int offset;
    
    /* offset = ret_address + param_size * (base - param_pos) */
    
    offset = 4 + 2 * (base - par);
    
    return offset;
    
}

/* carrega parâmetro em registrador primário */

void asm_loadparam(int par)

{
    
    printf("\tmov ax, word ptr [bp%+d]\n", asm_offsetpar(par));
    
}

/* armazena conteúdo do reg. prim. em parâmetro */

void asm_storeparam(int par)

{
    
    printf("\tmov word ptr [bp%+d], ax\n", asm_offsetpar(par));
    
}

/* analisa e traduz uma declaração local de dados */

void locdecl(void)

{
    
    match('v');
    
    addparam(getname(void));
    
    newline(void);
    
}

/* analisa e traduz declarações locais */

int locdecls(void)

{
    
    int i;
    
    for (i = 0; look == 'v'; i++) {
        
        locdecl(void);
        
    }
    
    return i;
    
}

/* coloca reg. prim. na pilha */

void asm_push(void)

{
    
    printf("\tpush ax\n");
    
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

void undefined(char name)

{
    
    fprintf(stderr, "Error: Undefined identifier %c\n", name);
    
    exit(1);
    
}

/* avisa a respeito de um identificador desconhecido */

void duplicated(char name)

{
    
    fprintf(stderr, "Error: Duplicated identifier %c\n", name);
    
    exit(1);
    
}

/* avisa a respeito de uma palavra-chave desconhecida */

void unrecognized(char name)

{
    
    fprintf(stderr, "Error: Unrecognized keyword %c\n", name);
    
    exit(1);
    
}

/* avisa a respeito de um identificador que não é variável */

void notvar(char name)

{
    
    fprintf(stderr, "Error: %c is not a variable\n", name);
    
    exit(1);
    
}

/* retorna o tipo de um identificador */

char symtype(char name)

{
    
    if (isparam(name))
        
        return 'f';
    
    return symtbl[name - 'A'];
    
}

/* verifica se "name" consta na tabela de símbolos */

char intable(char name)

{
    
    return (symtbl[name - 'A'] != ' ');
    
}

/* adiciona novo identificador à tabela de símbolos */

void addsymbol(char name, char type)

{
    
    if (intable(name))
        
        duplicated(name);
    
    symtbl[name - 'A'] = type;
    
}

/* verifica se identificador é variável */

void checkvar(char name)

{
    
    if (!intable(name))
        
        undefined(name);
    
    if (symtype(name) != 'v')
        
        notvar(name);
    
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

/* reconhece uma quebra de linha */

void newline(void)

{
    
    if (look == '\n')
        
        nextchar(void);
    
}

/* verifica se look combina com caracter esperado */

void match(char c)

{
    
    char s[2];
    
    if (look == c)
        
        nextchar(void);
    
    else {
        
        s[0] = c; /* uma conversao rápida (e feia) */
        
        s[1] = '\0'; /* de um caracter em string */
        
        expected(s);
        
    }
    
    skipwhite(void);
    
}

/* analisa e traduz um nome (identificador ou palavra-chave) */

char getname(void)

{
    
    char name;
    
    if (!isalpha(look))
        
        expected("Name");
    
    name = toupper(look);
    
    nextchar(void);
    
    skipwhite(void);
    
    return name;
    
}

/* analisa e traduz um número inteiro */

char getnum(void)

{
    
    char num;
    
    if (!isdigit(look))
        
        expected("Integer");
    
    num = look;
    
    nextchar(void);
    
    skipwhite(void);
    
    return num;
    
}

/* carrega uma variável no reg. prim. */

void asm_loadvar(char name)

{
    
    checkvar(name);
    
    printf("\tmov ax, word ptr %c\n", name);
    
}

/* processa um parâmetro formal */

void formalparam(void)

{
    
    addparam(getname(void));
    
}

/* coloca parâmetros na pilha */

void asm_pushparam(char name)

{
    
    switch (symtype(name)) {
            
        case 'v':
            
            printf("\tmov ax, offset %c\n", name);
            
            asm_push(void);
            
            break;
            
        case 'f':
            
            printf("\tpush word ptr [bp+%d]\n",
                   
                   asm_offsetpar(paramnum(name)));
            
            break;
            
        default:
            
            printf("Identifier %c cannot be used here!", name);
            
            exit(1);
            
            break;
            
    }
    
}

/* processa um parâmetro de chamada */

void param(void)

{
    char name;
    name = getname(void);
    
    asm_pushparam(name);
    
}

/* processa a lista de parâmetros para uma chamada de procedimento */

int paramlist(void)

{
    
    int i = 0;;
    
    match('(');
    
    if (look != ')') {
        
        for (;;) {
            
            param(void);
            
            i++;
            
            if (look != ',')
                
                break;
            
            match(',');
            
        }
        
    }
    
    match(')');
    
    return i * 2; /* número de parâmetros * bytes por parâmetro */
    
}

/* processa uma chamada de procedimento */

void docallproc(char name)

{
    
    int bytes = paramlist(void);
    
    asm_call(name);
    
    asm_cleanstack(bytes);
    
}

/* ajusta o ponteiro da pilha acima */

void asm_cleanstack(int bytes)

{
    
    if (bytes > 0)
        
        printf("\tadd sp, %d\n", bytes);
    
}

/* escreve o prólogo para um procedimento */

void asm_procprolog(char name, int nlocals)

{
    
    printf("%c:\n", name);
    
    printf("\tpush bp\n");
    
    printf("\tmov bp, sp\n");
    
    printf("\tsub sp, %d\n", nlocals * 2);
    
}

/* escreve o epílogo para um procedimento */

void asm_procepilog(void)

{
    
    printf("\tmov sp, bp\n");
    
    printf("\tpop bp\n");
    
    printf("\tret\n");
    
}

/* processa a lista de parâmetros formais de um procedimento */

void formallist(void)

{
    
    match('(');
    
    if (look != ')') {
        
        formalparam(void);
        
        while (look == ',') {
            
            match(',');
            
            formalparam(void);
            
        }
        
    }
    
    match(')');
    
    newline(void);
    
    base = nparams;
    
    nparams += 2;
    
}

/* armazena reg. prim. em variável */

void asm_storevar(char name)

{
    
    printf("\tmov word ptr %c, ax\n", name);
    
}

/* aloca espaço de armazenamento para variável */

void asm_allocvar(char name)

{
    
    if (intable(name))
        
        duplicated(name);
    
    addsymbol(name, 'v');
    
    printf("%c: dw 0\n", name);
    
}

void asm_return(void){
    printf("RET");
}

/* analisa e traduz uma expressão */

void expression(void)

{
    
    char name = getname(void);
    
    if (isparam(name))
        
        asm_loadparam(paramnum(name));
    
    else
        
        asm_loadvar(name);
    
}


/* analisa e traduz um comando de atribuição */

void assignment(char name)

{
    
    match('=');
    
    expression(void);
    
    asm_storevar(name);
    
}

/* gera uma chamada de procedimento */

void asm_call(char name)

{
    
    printf("\tcall %c\n", name);
    
}

/* analisa e traduz um comando de atribuição ou chamada de procedimento */

void assign_or_call(void)

{
    
    char name;
    
    name = getname(void);
    
    switch (symtype(name)) {
            
        case ' ':
            
            undefined(name);
            
            break;
            
        case 'v':
            
        case 'f':
            
            assignment(name);
            
            break;
            
        case 'p':
            
            docallproc(name);
            
            break;
            
        default:
            
            printf("Identifier %c cannot be used here!", name);
            
            exit(1);
            
            break;
            
    }
    
}

/* analiza e traduz um bloco de comandos */

void doblock(void)

{
    
    while (look != 'e') {
        
        assign_or_call(void);
        
        newline(void);
        
    }
    
}

/* analiza e traduz um bloco begin */

void beginblock(void)

{
    
    match('b');
    
    newline(void);
    
    doblock(void);
    
    match('e');
    
    newline(void);
    
}

/* analiza e traduz a declaração de uma variável */

void decl(void)

{
    
    match('v');
    
    asm_allocvar(getname(void));
    
}
