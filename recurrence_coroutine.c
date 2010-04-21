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

/* This is a pre-order iterator over a binary tree.  It navigates a binary tree
 * in the pattern Root -> Left -> Right.
 */
int treePreOrderIterator(Tree *t, CoroutineState *s) {
  initializeCoroutineState(s,TreeIteratorState, mystate); 
  mystate->tree = t;

  yield (s,mystate->tree->value);

  if(mystate->tree->left)
    recur (s, treePreOrderIterator(mystate->tree->left, s->next)); 

  if(mystate->tree->right) 
    recur (s, treePreOrderIterator(mystate->tree->right, s->next)); 

  finalizeCoroutine(s); 
  return 0;
}

/* This is a post-order iterator over a binary tree.  It navigates a binary tree
 * in the pattern Left -> Right -> Root.
 */
int treePostOrderIterator(Tree *t, CoroutineState *s) {
  initializeCoroutineState(s,TreeIteratorState, mystate); 
  mystate->tree = t;

  if(mystate->tree->left)
    recur (s, treePostOrderIterator(mystate->tree->left, s->next)); 

  if(mystate->tree->right) 
    recur (s, treePostOrderIterator(mystate->tree->right, s->next)); 

  yield (s,mystate->tree->value);

  finalizeCoroutine(s); 
  return 0;
}

/* This is a in-order iterator over a binary tree.  It navigates a binary tree
 * in the pattern Left -> Root -> Right.
 */
int treeInOrderIterator(Tree *t, CoroutineState *s) {
  initializeCoroutineState(s,TreeIteratorState, mystate); 
  mystate->tree = t;

  if(mystate->tree->left)
    recur (s, treeInOrderIterator(mystate->tree->left, s->next)); 

  yield (s,mystate->tree->value);

  if(mystate->tree->right) 
    recur (s, treeInOrderIterator(mystate->tree->right, s->next)); 

  finalizeCoroutine(s); 
  return 0;
}

/* This function just computes Fibonacci numbers forever.  Since we 'yield'
 * inside the 'while'-loop, it allows you to treat this as an iterator.
 */
int fibs(CoroutineState *s) {
  initializeCoroutineState(s, FibState, mystate); 

  mystate->a = 0;
  mystate->b = 1;

  while(1) {
    int f;
    yield(s,mystate->b);
    f = mystate->a;
    mystate->a = mystate->b;
    mystate->b += f;
  }
  finalizeCoroutine(s);
  return 0;
}

/* This is just like how you compute fibs efficiently in Haskell.  This function
 * is equivalent to:
 *     fibs x y = x : fibs y (x+y)
 * The Haskell version is obviously an infinite list, where as this one can be
 * thought of as an iterator over the infinite list.
 */

int fibs2(int a, int b, CoroutineState *s) {
  initializeCoroutineState(s, FibState, mystate);

  mystate->a = a; mystate->b = b;
  yield(s,mystate->a);
  recur(s,fibs2(mystate->b, mystate->a+mystate->b, s->next));

  finalizeCoroutine(s);
  return 0;
}

int main() {
  int i;
  int max = 0;

  CoroutineState *s;

  Tree *root = createTree(4);
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
    printf("|%d| ", v);
  }
  printf("\n");

  /* iterate over the whole tree, pre-order */
  s = createCoroutineState();
  while(1) {
    int v = treePreOrderIterator(root, s);
    if(s->done)
      break;
    printf("|%d| ", v);
  }
  printf("\n");

  /* iterate over the whole tree, post-order */
  s = createCoroutineState();
  while(1) {
    int v = treePostOrderIterator(root, s);
    if(s->done)
      break;
    printf("|%d| ", v);
  }
  printf("\n");


  max = 100; 

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
