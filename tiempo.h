#ifndef TIEMPO_H
#define TIEMPO_H

void timeval_sub (struct timeval *res, struct timeval *a, struct timeval *b);
void timeval_add (struct timeval *res, struct timeval *a, struct timeval *b);
void delay_until (struct timeval* next_activation);

#endif
