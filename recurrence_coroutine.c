#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct coroutine_state coroutine_state;
struct coroutine_state {
  void *state;
  long current;
  unsigned char done;
  coroutine_state *left;
  coroutine_state *right;
  coroutine_state *next;
  coroutine_state *tail;
};


coroutine_state* createCoroutineState() {
  coroutine_state *s = (coroutine_state*) malloc(sizeof (coroutine_state));
  bzero(s, sizeof(coroutine_state));
  return s;
}

coroutine_state *getTail(coroutine_state* s) {
  while(s->next) s = s->next;
  return s;
}

#define initializeCoroutineState(_st, str,my)                                 \
  str *my = (str*) _st->state;                                                \
  switch(_st->current) {                                                      \
  case 0:                                                                     \
  if(_st->state == 0) {                                                       \
    _st->state = (void*) malloc(sizeof (str));                                \
    bzero(_st->state, sizeof(str));                                           \
  }                                                                           \
  my = (str*) _st->state;

#define recur(s,f)                                                            \
  do{s->next = createCoroutineState();                                        \
  while(!s->next->done) {typeof(f) tmp =  f; s->current = __LINE__;           \
  if(!getTail(s)->done) return tmp; case __LINE__: 1;}}while(0);

#define yield(f)                                                              \
  do{s->current = __LINE__;s->next = 0;  return f; case __LINE__: 1;} while(0); 

#define finalizeCoroutine(s) } s->done = 1; s->next = 0;



/* ************************************************************************* */

typedef struct {
  int a;
  int b; 
  int c; 
} fib_state;


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

void printTree(Tree *t) {
  printf("%d ", t->value);
  if(t->left) printTree(t->left);
  if(t->right) printTree(t->right);
}

typedef struct {
  Tree *tree;
} tree_it_state;



void printStack(coroutine_state *s) {
  coroutine_state *u;
  for(u = s; u; u=u->next)  {
    tree_it_state *state = u->state;
    printf("_%ld ", u->current);
    if(state) {
      Tree *tree = ((tree_it_state*) u->state)->tree;
      printf("%d",tree ? tree->value: -1);
    } 
    if(u->done) {printf(" d");} else {printf(" _");}
    printf("_ ");
  }
  printf("@\n");
}


int tree_iterator(Tree *t, coroutine_state *s) {
  initializeCoroutineState(s,tree_it_state, mystate); 
  mystate->tree = t;

  if(mystate->tree->left)
    recur (s,tree_iterator (mystate->tree->left, s->next)); 

  yield (mystate->tree->value);

  if(mystate->tree->right) 
    recur (s,tree_iterator (mystate->tree->right, s->next)); 

  finalizeCoroutine(s); 
  return 0;
}

int fibs(coroutine_state *s) {
  initializeCoroutineState(s,fib_state, mystate); 

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


int fibs2(int a, int b, coroutine_state *s) {
  initializeCoroutineState(s, fib_state, mystate);

  mystate->a = a; mystate->b = b;

  yield(mystate->a);

  recur(s,fibs2(mystate->b, mystate->a+mystate->b, s->next));

  finalizeCoroutine(s);
  return 0;
}

int main() {
  int i;
  int max = 0;
  coroutine_state *s = createCoroutineState();
  Tree *root = createTree(4);
  insert(root, 2);
  insert(root, 1);  /*          4              */
  insert(root, 3);  /*     2         6         */
  insert(root, 6);  /*   1   3     5    7      */
  insert(root, 5);
  insert(root, 7);
  do {
    printf("!%d! ", tree_iterator(root, s));
  } while(!s->done);
  printf("\n");
  max = 20; 
  s = createCoroutineState();

  for(i = 0; i < max; i++) {
    printf("%d ", fibs(s));
  }

  printf("\n");

  s = createCoroutineState();
  for(i = 0; i < max; i++) {
    printf("%d ", fibs2(1,1,s));
  }
  printf("\n");


  return 0;
}
