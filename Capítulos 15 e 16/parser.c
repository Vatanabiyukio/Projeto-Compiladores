/* parser.c */

#include "scanner.h"

#include "codegen.h"

#include "parser.h"
#include "input.h"

/* analisa e traduz uma expressão */

/* analisa e traduz uma operação de soma */

/* coloca registrador primário na pilha */

void asmPush(void)

{
    
    emit("PUSH AX");
    
}

/* adiciona topo da pilha a reg. primário */

void asmPopAdd(void)

{
    
    emit("POP BX");
    
    emit("ADD AX, BX");
    
}

/* subtrari do topo da pilha o reg. primário */

void asmPopSub(void)

{
    
    emit("POP BX");
    
    emit("SUB AX, BX");
    
    asmNegate(void);
    
}

void add(void)

{
    
    match('+');
    
    asmPush(void);
    
    factor(void);
    
    asmPopAdd(void);
    
}

/* analisa e traduz uma operação de subtração */

void subtract(void)

{
    
    match('-');
    
    asmPush(void);
    
    factor(void);
    
    asmPopSub(void);
    
}

/* reconhece um operador multiplicativo */

int isMulOp(char c)

{
    
    return (c == '*' || c == '/' || c == '&');
    
}

/* analisa e traduz uma operação AND */

void boolAnd(void)

{
    
    match('&');
    
    asmPush(void);
    
    factor(void);
    
    asmPopAnd(void);
    
}

/* aplica AND com topo da pilha a reg. primário */

void asmPopAnd(void)

{
    
    emit("POP BX");
    
    emit("AND AX, BX");
    
}

/* analisa e traduz um fator com NOT opcional */

void notFactor(void)

{
    
    if (look == '!') {
        
        match('!');
        
        factor(void);
        
        asmNot(void);
        
    } else
        
        factor(void);
    
}

/* aplica NOT a reg. primário */

void asmNot(void)

{
    
    emit("NOT AX");
    
}


/* analisa e traduz um termo */

void term(void)

{
    
    factor(void);
    
    while (isMulOp(look)) {
        
        switch (look) {
                
            case '*':
                
                multiply(void);
                
                break;
                
            case '/':
                
                divide(void);
                
                break;
                
            case '&':
                
                boolAnd(void);
                
                break;
                
        }
        
    }
    
}

/* adiciona topo da pilha a reg. primário */

void asmPopMul(void)

{
    
    emit("POP BX");
    
    emit("IMUL BX");
    
}

/* subtrari do topo da pilha o reg. primário */

void asmPopDiv(void)

{
    
    emit("POP BX");
    
    emit("XCHG AX, BX");
    
    emit("CWD");
    
    emit("IDIV BX");
    
}

/* analisa e traduz um termo com um sinal opcional */

void signedTerm(void)

{
    
    char sign = look;
    
    if (isAddOp(look))
        
        nextChar(void);
    
    term(void);
    
    if (sign == '-')
        
        asmNegate(void);
    
}

/* reconhece um operador aditivo */

int isAddOp(char c)

{
    
    return (c == '+' || c == '-' || c == '|' || c == '~');
    
}

/* analisa e traduz uma operação OU booleana */

void boolOr(void)

{
    
    match('|');
    
    asmPush(void);
    
    term(void);
    
    asmPopOr(void);
    
}

/* aplica OU com topo da pilha a reg. primário */

void asmPopOr(void)

{
    
    emit("POP BX");
    
    emit("OR AX, BX");
    
}

/* aplica OU-exclusivo com topo da pilha a reg. primário */

void asmPopXor(void)

{
    
    emit("POP BX");
    
    emit("XOR AX, BX");
    
}

/* analisa e traduz uma operação OU-exclusivo booleana */

void boolXor(void)

{
    
    match('~');
    
    asmPush(void);
    
    term(void);
    
    asmPopXor(void);
    
}

/* analisa e traduz uma expressão */

void expression(void)

{
    
    signedTerm(void);
    
    while (isAddOp(look)) {
        
        switch (look) {
                
            case '+':
                
                add(void);
                
                break;
                
            case '-':
                
                subtract(void);
                
                break;
                
            case '|':
                
                boolOr(void);
                
                break;
                
            case '~':
                
                boolXor(void);
                
                break;
                
        }
        
    }
    
}

void asmNegate(void)

{
    
    emit("NEG AX");
    
}

void signedFactor(void)

{
    
    char sign = look;
    
    if (isAddOp(look))
        
        nextChar(void);
    
    factor(void);
    
    if (sign == '-')
        
        asmNegate(void);
    
}

/* armazena valor do registrador primário em variável */

void asmStoreVariable(char *name)

{
    
    emit("MOV [%s], AX", name);
    
}

/* analisa e traduz um comando de atribuição */

void assignment(void)

{
    
    char name[MAXNAME+1];
    
    getName(name);
    
    match('=');
    
    expression(void);
    
    asmStoreVariable(name);
    
}

/* analisa e traduz um fator matemático */

void factor(void)

{
    
    char name[MAXNAME+1], num[MAXNUM+1];
    
    if (look == '(') {
        
        match('(');
        
        expression(void);
        
        match(')');
        
    } else if (isdigit(look)) {
        
        getNum(num);
        
        asmLoadConstant(num);
        
    } else if (isalpha(look)) {
        
        getName(name);
        
        asmLoadVariable(name);
        
    } else
        
        error("Unrecognized character: '%c'", look);
    
}
