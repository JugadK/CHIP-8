#include <stdlib.h>
struct stack {

  unsigned int size;
  unsigned int *stack_pointer;

};

struct stack createStack(int capacity);
unsigned int pop(struct stack *stk);
void push(struct stack *stk, unsigned int data);