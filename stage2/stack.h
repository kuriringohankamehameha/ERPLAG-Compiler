#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct Stack Stack;

struct Stack {
    int data;
    Stack* next;
    size_t capacity;
    size_t size;
};

Stack* init_stack(int data, size_t capacity);
Stack* init_stacknode(int data);
Stack* stack_push(Stack* top, int data);
Stack* stack_pop(Stack* top);
void print_data(Stack* node);
void print_stack(Stack* top);
void free_stacknode(Stack* top);

#endif
