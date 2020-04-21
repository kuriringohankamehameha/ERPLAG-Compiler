// Group 42:
// R.VIJAY KRISHNA 2017A7PS0183P
// ROHIT K 2017A7PS0177P
#include "stack.h"

Stack* init_stack(int data, size_t capacity) {
    // Create our stack. data is a pointer to any random data
    Stack* top = (Stack*) calloc (1, sizeof(Stack));
    top->data = data;
    top->next = NULL;
    top->capacity = capacity;
    top->size = 0;
    return top;
}

Stack* init_stacknode(int data) {
    // Create our stack. data is a pointer to any random data
    Stack* top = (Stack*) calloc (1, sizeof(Stack));
    top->data = data;
    top->next = NULL;
    return top;
}

Stack* stack_push(Stack* top, int data) {
    // Pushes to the top of the stack
    if (top->size >= top->capacity) {
        fprintf(stderr, "Error: Stack Overflow. Stack can only support a maximum capacity of: %lu\n", top->capacity);
        exit(1);
    }
    Stack* node = init_stacknode(data);
    node->capacity = top->capacity;
    node->size = top->size + 1;
    node->next = top;
    top = node;
    return top;
}

Stack* stack_pop(Stack* top) {
    // Pops from top
    if (!top)
        return NULL;
    if (top->next == NULL) {
        free(top);
        return NULL;
    }
    Stack* temp = top;
    top = top->next;
    temp->next = NULL;
    free(temp);
    return top;
}

inline void print_data(Stack* node) {
    printf("%d", node->data);
}

void print_stack(Stack* top) {
    for (Stack* node = top; node; node = node->next) {
        printf("Data: ");
        print_data(node);
        printf(" -> ");
    }
    printf("\n");
}

void free_stacknode(Stack* top) {
    for (Stack* node = top; node; ) {
        if (!node)
            return;
        Stack* temp = node;
        node = temp->next;
        temp->next = NULL;
        free(temp);
    }
}
