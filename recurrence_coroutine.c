#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coroutines.h"

typedef struct {
  int a;
  int b; 
  int c; 
} FibState;

typedef struct Tree Tree;

struct Tree {
  int value;
  Tree *left;
  Tree *right;
};

Tree *createTree(int x) {
  Tree *t = (Tree*) malloc(sizeof(Tree));
  bzero(t, sizeof(Tree));
  t->value = x;
  return t;
}

void insert(Tree *t, int x) {
  if(x < t->value) {
    if(t->left) {
      insert(t->left, x);
    } else {
      t->left = createTree(x);
    } 
  } else {
     if(t->right) {
      insert(t->right, x);
    } else {
      t->right = createTree(x);
    } 
  }
}

typedef struct {
  Tree *tree;
} TreeIteratorState;

void printStack(CoroutineState *s) {
  CoroutineState *u;
  for(u = s; u; u=u->next)  {
    TreeIteratorState *state = u->state;
    printf("_%ld ", u->current);
    if(state) {
      Tree *tree = ((TreeIteratorState*) u->state)->tree;
      printf("%d",tree ? tree->value: -1);
    } 
    if(u->done) {printf(" d");} else {printf(" _");}
    printf("_ ");
  }
  printf("@\n");
}

int treePreOrderIterator(Tree *t, CoroutineState *s) {
  initializeCoroutineState(s,TreeIteratorState, mystate); 
  mystate->tree = t;

  yield (mystate->tree->value);

  if(mystate->tree->left)
    recur (s,treePreOrderIterator(mystate->tree->left, s->next)); 

  if(mystate->tree->right) 
    recur (s,treePreOrderIterator(mystate->tree->right, s->next)); 

  finalizeCoroutine(s); 
  return 0;
}

int treePostOrderIterator(Tree *t, CoroutineState *s) {
  initializeCoroutineState(s,TreeIteratorState, mystate); 
  mystate->tree = t;

  if(mystate->tree->left)
    recur (s, treePostOrderIterator(mystate->tree->left, s->next)); 

  if(mystate->tree->right) 
    recur (s, treePostOrderIterator(mystate->tree->right, s->next)); 

  yield (mystate->tree->value);

  finalizeCoroutine(s); 
  return 0;
}


int treeInOrderIterator(Tree *t, CoroutineState *s) {
  initializeCoroutineState(s,TreeIteratorState, mystate); 
  mystate->tree = t;

  if(mystate->tree->left)
    recur (s, treeInOrderIterator(mystate->tree->left, s->next)); 

  yield (mystate->tree->value);

  if(mystate->tree->right) 
    recur (s, treeInOrderIterator(mystate->tree->right, s->next)); 

  finalizeCoroutine(s); 
  return 0;
}

int fibs(CoroutineState *s) {
  initializeCoroutineState(s, FibState, mystate); 

  mystate->a = 0;
  mystate->b = 1;

  while(1) {
    int f;
    yield(mystate->b);
    f = mystate->a;
    mystate->a = mystate->b;
    mystate->b += f;
  }
  finalizeCoroutine(s);
  return 0;
}


int fibs2(int a, int b, CoroutineState *s) {
  initializeCoroutineState(s, FibState, mystate);

  mystate->a = a; mystate->b = b;
  yield(mystate->a);
  recur(s,fibs2(mystate->b, mystate->a+mystate->b, s->next));

  finalizeCoroutine(s);
  return 0;
}

int main() {
  int i;
  int max = 0;
  Tree *root = createTree(4);
  CoroutineState *s;

  insert(root, 2);
  insert(root, 1);  /*          4              */
  insert(root, 3);  /*     2         6         */
  insert(root, 6);  /*   1   3     5    7      */
  insert(root, 5);
  insert(root, 7);

  /* iterate over the whole tree, inorder */
  s = createCoroutineState();
  while(1) {
    int v = treeInOrderIterator(root, s);
    if(s->done)
      break;
    printf("!%d! ", v);
  }
  printf("\n");

  /* iterate over the whole tree, pre-order */
  s = createCoroutineState();
  while(1) {
    int v = treePreOrderIterator(root, s);
    if(s->done)
      break;
    printf("!%d! ", v);
  }
  printf("\n");

  /* iterate over the whole tree, post-order */
  s = createCoroutineState();
  while(1) {
    int v = treePostOrderIterator(root, s);
    if(s->done)
      break;
    printf("!%d! ", v);
  }
  printf("\n");


  max = 20; 

  /* pull the first `max' fibs */
  s = createCoroutineState();
  for(i = 0; i < max; i++) {
    printf("%d ", fibs(s));
  }
  printf("\n");

  /* pull the first `max' fibs */
  s = createCoroutineState();
  for(i = 0; i < max; i++) {
    printf("%d ", fibs2(1,1,s));
  }
  printf("\n");


  return 0;
}
