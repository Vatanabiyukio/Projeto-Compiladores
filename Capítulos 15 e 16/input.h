#ifndef _INPUT_H

#define _INPUT_H

/* OBS: este pequeno truque serve para evitar que um cabe�alho
 
 seja inclu�do mais de uma vez em caso de refer�ncias recursivas. */

extern char look;

void initInput(void);

void nextChar(void);

#endif
