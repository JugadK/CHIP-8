#include "stack.h"
#include <stdlib.h>

// TODO method to free stack

struct stack createStack(int capacity) {

  struct stack stk;

  stk.size = capacity;
  stk.stack_pointer = malloc(capacity * sizeof(unsigned int));

  return stk;
}

unsigned int pop(struct stack *stk) {

  // TODO error handling

  int popped_value = *(stk->stack_pointer);

  stk->stack_pointer = stk->stack_pointer - sizeof(unsigned int);

  return popped_value;
}

void push(struct stack *stk, unsigned int data) {

  stk->stack_pointer = stk->stack_pointer + sizeof(unsigned int);

  // TODO error handling

  *(stk->stack_pointer) = data;
}
