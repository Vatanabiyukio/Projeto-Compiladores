/* codegen.c */

#include "output.h"

#include "codegen.h"

#include "input.h"

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

void expression(void)

{
    
    signedFactor(void);
    
    while (isAddOp(look)) {
        
        switch (look) {
                
            case '+':
                
                add(void);
                
                break;
                
            case '-':
                
                subtract(void);
                
                break;
                
        }
        
    }
    
}

/* carrega uma constante no registrador primário */

void asmLoadConstant(char *s)

{
    
    emit("MOV AX, %s", s);
    
}
/* carrega uma variável no registrador primário */

void asmLoadVariable(char *s)

{
    
    emit("MOV AX, [%s]", s);
    
}

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

/* analisa e traduz uma operação de subtração */

void subtract(void)

{
    
    match('-');
    
    asmPush(void);
    
    factor(void);
    
    asmPopSub(void);
    
}

/* analisa e traduz uma operação de soma */

void add(void)

{
    
    match('+');
    
    asmPush(void);
    
    factor(void);
    
    asmPopAdd(void);
    
}

/* analisa e traduz uma expressão */

void expression(void)

{
    
    signedFactor(void);
    
    while (isAddOp(look)) {
        
        switch (look) {
                
            case '+':
                
                add(void);
                
                break;
                
            case '-':
                
                subtract(void);
                
                break;
                
        }
        
    }
    
}
