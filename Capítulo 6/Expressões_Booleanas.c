#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "Bool.h"

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

/* PROGRAMA PRINCIPAL */
int main(void)
{
    init(void);
    boolExpression(void);
    return 0;
}
