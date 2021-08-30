#ifndef BOOL1_H_INCLUDED
#define BOOL1_H_INCLUDED


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


/* reconhece uma literal Booleana */
int isBoolean(char c)
{
    return (c == 'T' || c == 'F');
}

/* recebe uma literal Booleana */
int getBoolean(void)
{
    int boolean;
    
    if (!isBoolean(look))
        expected("Boolean Literal");
    boolean = (look == 'T');
    nextChar(void);
    
    return boolean;
}

/* analisa e traduz um fator booleno com NOT opcional */
void notFactor(void)
{
    if (look == '!')
    {
        match('!');
        boolFactor(void);
        emit("NOT AX");
    }
    else
        boolFactor(void);
}

/* reconhece e traduz um operador OR */
void boolOr(void)
{
    match('|');
    boolTerm(void);
    emit("POP BX");
    emit("OR AX, BX");
}

/* reconhece e traduz um operador XOR */
void boolXor(void)
{
    match('~');
    boolTerm(void);
    emit("POP BX");
    emit("XOR AX, BX");
}

/* analisa e traduz uma expressão booleana */
void boolExpression(void)
{
    boolTerm(void);
    while (isOrOp(look))
    {
        emit("PUSH AX");
        switch (look)
        {
            case '|':
                boolOr(void);
                break;
            case '~':
                boolXor(void);
                break;
        }
    }
}

/* reconhece um operador OU */
int isOrOp(char c)
{
    return (c == '|' || c == '~');
}

/* analisa e traduz um termo booleano*/
void boolTerm(void)
{
    notFactor(void);
    while (look == '&')
    {
        emit("PUSH AX");
        match('&');
        notFactor(void);
        emit("POP BX");
        emit("AND AX, BX");
    }
}

/* analisa e traduz um fator booleano */
void boolFactor(void)
{
    if (isBoolean(look))
    {
        if (getBoolean(void))
            emit("MOV AX, -1");
        else
            emit("MOV AX, 0");
    }
    else
        relation(void);
}

/* analisa e traduz uma relação */
void relation(void)
{
    expression(void);
    if (isRelOp(look))
    {
        emit("PUSH AX");
        switch (look)
        {
            case '=':
                equals(void);
                break;
            case '#':
                notEquals(void);
                break;
            case '>':
                greater(void);
                break;
            case '<':
                less(void);
                break;
        }
    }
}

/* reconhece operadores relacionais */
int isRelOp(char c)
{
    return (c == '=' || c == '#' || c == '<' || c == '>');
}

/* reconhece e traduz um operador de igualdade */
void equals(void)
{
    int l1, l2;
    
    match('=');
    l1 = newLabel(void);
    l2 = newLabel(void);
    expression(void);
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JE L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* reconhece e traduz um operador de não-igualdade */
void notEquals(void)
{
    int l1, l2;
    
    match('#');
    l1 = newLabel(void);
    l2 = newLabel(void);
    expression(void);
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JNE L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* reconhece e traduz um operador de maior que */
void greater(void)
{
    int l1, l2;
    
    match('>');
    l1 = newLabel(void);
    l2 = newLabel(void);
    expression(void);
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JG L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* reconhece e traduz um operador de menor que */
void less(void)
{
    int l1, l2;
    
    match('<');
    l1 = newLabel(void);
    l2 = newLabel(void);
    expression(void);
    emit("POP BX");
    emit("CMP BX, AX");
    emit("JL L%d", l1);
    emit("MOV AX, 0");
    emit("JMP L%d", l2);
    postLabel(l1);
    emit("MOV AX, -1");
    postLabel(l2);
}

/* analisa e traduz um identificador */
void ident(void)
{
    char name;
    
    name = getName(void);
    if (look == '(')
    {
        match('(');
        match(')');
        emit("CALL %c", name);
    }
    else
        emit("MOV AX, [%c]", name);
}

/* analisa e traduz um fator matemático */
void factor(void)
{
    
    if (look == '(')
    {
        match('(');
        boolExpression(void);
        match(')');
    }
    else if (isalpha(look))
        ident(void);
    else
        emit("MOV AX, %c", getNum(void));
}

/* analisa e traduz um fator com sinal opcional */
void signedFactor(void)
{
    if (look == '+')
        nextChar(void);
    if (look == '-')
    {
        nextChar(void);
        if (isdigit(look))
            emit("MOV AX, -%c", getNum(void));
        else
        {
            factor(void);
            emit("NEG AX");
        }
    }
    else
        factor(void);
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

/* analisa e traduz um termo matemático */
void term(void)
{
    signedFactor(void);
    while (isMulOp(look))
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

/* reconhece e traduz uma soma */
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

/* analisa e traduz uma expressão matemática */
void expression(void)
{
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

/* reconhece operador aditivo */
int isAddOp(char c)
{
    return (c == '+' || c == '-');
}

/* reconhece operador multiplicativo */
int isMulOp(char c)
{
    return (c == '*' || c == '/');
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

/* reconhece uma linha em branco */
void newLine(void)
{
    if (look == '\n')
        nextChar(void);
}

/* analisa e traduz um bloco de comandos */
void block(int exitLabel)
{
    int follow;
    
    follow = 0;
    
    while (!follow)
    {
        newLine(void);
        switch (look)
        {
            case 'i':
                doIf(exitLabel);
                break;
            case 'w':
                doWhile(void);
                break;
            case 'p':
                doLoop(void);
                break;
            case 'r':
                doRepeat(void);
                break;
            case 'f':
                doFor(void);
                break;
            case 'd':
                doDo(void);
                break;
            case 'b':
                doBreak(exitLabel);
                break;
            case 'e':
            case 'l':
            case 'u':
                follow = 1;
                break;
            default:
                other(void);
                break;
        }
        newLine(void);
    }
}

/* analisa e traduz um comando de atribuição */
void assignment(void)
{
    char name;
    
    name = getName(void);
    match('=');
    boolExpression(void);
    emit("MOV [%c], AX", name);
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
    if (look == 'l')
    {
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

/* analisa e traduz uma condição */
void condition(void)
{
    emit("# condition");
}

#endif // BOOL1_H_INCLUDED
